#!/usr/bin/env python3
"""The engine as nodes: what the engine's headers declare, as nodes of the Blueprint language.

  tools/engine_api.py [--list]
  tools/engine_api.py --check <level.klevel | texts dir>   what it names of the engine's libraries that they do not have
  tools/engine_api.py --libraries    the engine's libraries (kern/logic/engine_libraries.h): src/logic/engine_libraries.gen.cpp

A function a header of the engine declares and does not define there is something only the engine
can do: cast a ray through the physics world, draw a mesh, play a sound. Each is a node - for
every game, not for one:

  kern::physics::World::cast_ray   is the node   engine.world.cast_ray
                                   in a Blueprint's C++   engine_world_cast_ray(world, from, to ...)

  - a const method and a function that only gives something is a value, everything else a step (a
    function handed an object of the engine to change - Renderer& - changes something: a step);
  - what a function writes through a parameter (std::vector<u64>& out) is a further output, and so is
    what it reports through a pointer that is null by default (bool* changed = nullptr: the node gives
    it; the call takes it where it wants it, or nowhere);
  - an object of the engine with a life of its own (a class: the physics world, the renderer, the
    mixer; a struct that cannot be copied or holds such an object itself: a Mesh) is a handle, a whole
    number the host gives - the first input of its class's nodes. Every public method of such an
    object is a node, also where its header defines it: what the object holds is the engine's;
  - a method of plain data the engine only declares (MeshData::as_input) takes the value as its first
    input; what a method that is not const makes of it is its output `self`;
  - a plain struct of the engine (physics::Pose, physics::BodySettings, physics::RayHit) is a
    struct the Blueprint declares with the same fields: the pin names it, by the name every type
    has in a Blueprint - the namespaces it stands in are part of it (physics_Pose; see
    type_name_of in tools/cpp_to_blueprint.py).

The interpreter and the JIT call them through one door, logic::Host::engine(function, in, out): the game
binds the engine there, a test a stand-in. A Blueprint's class calls the function of the engine itself, typed
(kern/logic/engine_direct.h: no value is packed, nothing is looked up by a name), where its host gives it the
engine's objects - and through the door where it does not. Both are written here, from the same functions.

This tool reads the headers with the compiler's own parser (as tools/cpp_to_blueprint.py does - it
shares its types) and writes
  kern/src/logic/engine_api.gen.cpp           the table of the functions and their pins
  kern/include/kern/logic/engine_nodes.gen.h  the functions of a Blueprint's class: each the call of the real function
                                              on the class's own C++ types, and the call through the door
  kern/tests/engine_case.gen.h                the test's Blueprint that calls every one of them
  kern/src/<component>/engine_calls.gen.cpp   the call of every node on the real function (kern/logic/engine_host.h):
                                              its inputs made into what the C++ function takes, what it gives and
                                              writes made into its outputs - for the headers of that component
A function whose types have no place in the language (a callback, a raw pointer) gets no node;
--list names them with what they wait for.
--check writes nothing and fails when a generated file is not what the headers give (or one is left that no run
writes any more); a run removes such a stale file (stale_outputs). --selftest: the tool's own test.
"""
import collections
import importlib.util
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location("cpp_to_blueprint", os.path.join(HERE, "cpp_to_blueprint.py"))
c2b = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(c2b)
Wait, kids, qtype = c2b.Wait, c2b.kids, c2b.qtype

KERN = os.path.join(os.path.dirname(HERE), "kern")
# The headers whose functions are nodes: what a game is written against.
HEADERS = ["core/base.h", "core/jobs.h", "app/app.h", "audio/audio.h", "audio/synth.h", "physics/physics.h", "gfx/mesh.h", "gfx/renderer.h", "app/project.h",
           "geo/mesh_data.h", "geo/primitives.h", "geo/earcut.h", "geo/buildings.h", "geo/roads.h", "game/level.h", "game/sound.h", "game/runtime.h",
           "net/client_server.h"]
Api, snake = c2b.Api, c2b.snake   # (the converter's own: a call in a game's source and its node have the same pins)


def c_text(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"').replace("\n", " ") + '"'


def table_text(api, functions, interfaces=(), classes=()):
    lines = ["// The engine's functions as nodes: the table (written by tools/engine_api.py from the engine's headers -",
             "// see kern/logic/engine_api.h; not edited by hand).",
             '#include "kern/logic/engine_api.h"', "", "namespace kern::logic {", "",
             "const std::vector<EngineFunction>& engine_functions() {", "  static const std::vector<EngineFunction> all = {"]
    for f in functions:
        def pins(ps):
            out = []
            for name, bt, h in ps:
                # (an object of the engine is an Object<I> of its class at every pin - never a number: kern/docs/engine_object.md)
                if h is not None and c2b.object_class(bt) != h:
                    raise SystemExit(f"engine_api.py: {f['type']} has the pin {name} of an object of the engine ({h}) as {bt}, not Object<{h}> - "
                                     "an object of the engine is never a number (kern/docs/engine_object.md)")
                t, of = api.pin(bt)
                count = f.get("counts", {}).get(name) if ps is f["outs"] else None   # (an output list the engine writes through a pointer: its count)
                per = f.get("per", {}).get(name, 1) if count else 1                  # (... and how many numbers each it counts is)
                out.append(f"{{{c_text(name)}, PinType::{t}, {c_text(of)}, {c_text(f.get('shapes', {}).get(name) or api.shape(bt, f.get('raws', {}).get(name)))}" + (f", {c_text(count)}" if count else "") + (f", {per}" if per != 1 else "") + "}")
            return "{" + ", ".join(out) + "}"
        lines.append(f"      {{{c_text(f['type'])}, {c_text(f['cpp'])}, {c_text(f['label'])}, {c_text(f['category'])}, {c_text(f['help'])}, {'true' if f['value'] else 'false'},")
        lines.append(f"       {pins(f['ins'])},")
        flags = [bool(f.get("place")), bool(f.get("clock")), bool(f.get("field"))]
        while flags and not flags[-1]:
            flags.pop()
        lines.append(f"       {pins(f['outs'])}" + "".join(", true" if x else ", false" for x in flags) + "},")
    lines += ["  };", "  return all;", "}", ""]
    # the interfaces of the engine (EngineInterface): a Blueprint can be the object the engine calls
    lines += ["const std::vector<EngineInterface>& engine_interfaces() {", "  static const std::vector<EngineInterface> all = {"]
    for q, name, virtuals, waits in interfaces:
        fs = ", ".join(f"{{{c_text(f['decl']['name'])}, {c_text(f['type'])}, {'true' if pure else 'false'}" + (f", {c_text(why)}" if why else "") + "}" for f, pure, why in virtuals)
        lines.append(f"      {{{c_text(name)}, {c_text(q)}, {{{fs}}}" + (f", {c_text(waits)}" if waits else "") + "},")
    lines += ["  };", "  return all;", "}", ""]
    # the classes of the engine a Blueprint holds objects of (EngineClass): Object<kern::physics::World> - with the
    # classes each derives from that are objects of the engine too (an Object of one goes where one of those is wanted)
    lines += ["const std::vector<EngineClass>& engine_classes() {", "  static const std::vector<EngineClass> all = {"]
    for q, bases in classes:
        lines.append(f"      {{{c_text(c2b.type_name_of(q))}, {c_text(q)}, {{{', '.join(c_text(c2b.type_name_of(b)) for b in bases)}}}}},")
    lines += ["  };", "  return all;", "}", "", "}  // namespace kern::logic", ""]
    return "\n".join(lines)


def case_text(api, functions):
    """The test's Blueprint: every function of the engine called once, what it gives kept in a variable. The
    functions called on an object of the engine (a method of one, its release) and those that make one without
    inputs stand in the functions Run<n>, all others in Free<n>: a test plays the first on the engine itself -
    with no object behind the handle 0 each of those calls fails before the engine is asked, and an object is
    made as C++ makes it without arguments; what a free function gives may depend on what the test does not
    hold (the clock, the threads of the jobs), and its inputs here are zeros nobody chose."""
    u = api.u
    plain = {"Bool": "false", "Int": "0", "Float": "0.0", "Vector": "Vector{0.0, 0.0, 0.0}", "Text": '""', "Int64": "Int64{0x0}"}
    zero = lambda bt: plain[bt] if bt in plain else u.zero(bt)
    variables, members, bodies, free, inputs = [], [], [], [], {}

    def value(bt):
        if bt in plain:
            return plain[bt]
        if bt.startswith("List<") and bt[5:-1] in plain:
            return bt + "{}"
        if bt not in inputs:   # (a struct, an enum, a list of them: a variable of that type, at its default)
            inputs[bt] = "In" + str(len(inputs) + 1)
            variables.append(f"  {bt} {inputs[bt]} = {zero(bt)};")
        return inputs[bt]
    node = 10

    def place_lines(f, name, at, args, r, binder):
        bt = f["outs"][0][1]
        out = [f"{name}({', '.join([f'n{binder}'] + args)});"]
        variables.append(f"  {bt} {r}_0 = {zero(bt)};")
        out.append(f"{r}_0 = {at};")
        if bt.startswith("List<"):
            item = bt[5:-1]
            variables.append(f"  Int {r}_1 = 0;")
            variables.append(f"  {item} {r}_2 = {zero(item)};")
            variables.append(f"  {bt} {r}_3 = {zero(bt)};")
            out += [f"list_add({at}, {value(item)});", f"{at}[0] = {value(item)};", f"{r}_1 = list_length<{item}>({at});",
                    f"{r}_2 = {at}[9];", f"{r}_3 = {at};"]
        else:
            variables.append(f"  {bt} {r}_3 = {zero(bt)};")
            out += [f"{at} = {value(bt)};", f"{r}_3 = {at};"]
        return out

    # (where the class of a place has a maker without inputs and a releaser: the place of an object it made - and of
    # the same handle after the object ended; its further inputs - the index of an entry - at their defaults, which name an entry)
    made = {}
    for f in functions:
        if f.get("role") in ("make", "release") and (f["role"] == "release" or not f["ins"]):
            made.setdefault(f.get("class"), {})[f["role"]] = f
    for k, f in enumerate(functions):
        name = f["type"].replace(".", "_")
        args = [value(bt) for _, bt, _ in f["ins"]]
        lines = []
        if f["value"]:
            bt = f["outs"][0][1]
            variables.append(f"  {bt} R{k} = {zero(bt)};")
            lines.append(f"R{k} = {name}({', '.join(args)});")
        elif not f["outs"]:
            lines.append(f"{name}({', '.join(args)});")
        elif f.get("place"):
            # (a place of the engine: the node binds it, the nodes of places name it - read, set, and a list's
            # length, an item added, an entry set, one beside it read; here on the handle of no object)
            node += 1
            members.append(f"  struct {{ {name}_place<{f['outs'][0][1]}> {f['outs'][0][0]} = {{}}; }} n{node};")
            lines += place_lines(f, name, f"n{node}.{f['outs'][0][0]}", args, f"R{k}", node)
        else:
            node += 1
            members.append("  struct { " + " ".join(f"{bt} {o} = {zero(bt)};" for o, bt, _ in f["outs"]) + f" }} n{node};")
            lines.append(f"{name}({', '.join([f'n{node}'] + args)});")
            for j, (o, bt, _) in enumerate(f["outs"]):
                variables.append(f"  {bt} R{k}_{j} = {zero(bt)};")
                lines.append(f"R{k}_{j} = n{node}.{o};")
        d = f["decl"]
        method = f.get("role") is None and d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static"
        on_object = (method and bool(f["ins"]) and f["ins"][0][2] is not None) or f.get("role") == "release" or (f.get("role") == "make" and not f["ins"])
        (bodies if on_object else free).append(lines)
    places = []
    for k, f in enumerate(functions):
        life = made.get(f["ins"][0][2] if f.get("place") and f["ins"] else None, {})
        if "make" not in life or "release" not in life:
            continue
        node += 1
        maker, name, mk = life["make"]["type"].replace(".", "_"), f["type"].replace(".", "_"), node
        ot = life["make"]["outs"][0][1]   # (what the maker gives: an Object of the class)
        variables.append(f"  {ot} H{k} = {ot}{{}};")
        members.append(f"  struct {{ {ot} result = {ot}{{}}; }} n{mk};")
        node += 1
        members.append(f"  struct {{ {name}_place<{f['outs'][0][1]}> {f['outs'][0][0]} = {{}}; }} n{node};")
        lines = [f"{maker}(n{mk});", f"H{k} = n{mk}.result;", *place_lines(f, name, f"n{node}.{f['outs'][0][0]}", [f"H{k}"] + [value(bt) for _, bt, _ in f["ins"][1:]], f"P{k}", node)]
        lines.append(f"{life['release']['type'].replace('.', '_')}(H{k});")
        variables.append(f"  {f['outs'][0][1]} P{k}_4 = {zero(f['outs'][0][1])};")
        lines += [f"P{k}_4 = n{node}.{f['outs'][0][0]};", f"n{node}.{f['outs'][0][0]} = {value(f['outs'][0][1])};"]
        places.append(lines)
    if places:
        bodies.append([ln for body in places for ln in body])
    word = re.compile(r"[A-Za-z_]\w*")
    used = set(word.findall("\n".join(variables + members)))
    keep, changed = [False] * len(u.types), True
    while changed:
        changed = False
        for i, line in enumerate(u.types):
            m = re.match(r"\s*KERN_(?:ENUM|STRUCT)\((\w+)", line)
            if not keep[i] and m and m.group(1) in used:
                keep[i] = changed = True
                used.update(word.findall(line))
    out = ["struct Engine : kern::Blueprint {", *[line for i, line in enumerate(u.types) if keep[i]], "", *variables, ""]
    runs, on_objects, per = [], [], 40
    for name, group in (("Run", bodies), ("Free", free)):
        for a in range(0, len(group), per):
            runs.append(f"{name}{a // per + 1}")
            out += [f"  void {runs[-1]}() {{", *["    " + ln for body in group[a:a + per] for ln in body], "  }", ""]
        if name == "Run":
            on_objects = list(runs)
    out += [*members, "};"]
    text = "\n".join(out) + "\n"
    return ("// The engine's nodes in a test: a Blueprint that calls every function of the engine once and keeps what it\n"
            "// gives (written by tools/engine_api.py - see kern/logic/engine_api.h; not edited by hand).\n#pragma once\n\n"
            "namespace engine_case {\n\ninline const char* kEngine = R\"kn(\n" + text + ")kn\";\n\n"
            "inline const char* const kRuns[] = {" + ", ".join(c_text(r) for r in runs) + "};\n"
            "// (those whose calls are on objects of the engine: played once more on the engine itself)\n"
            "inline const char* const kRunsOnObjects[] = {" + ", ".join(c_text(r) for r in on_objects) + "};\n\n}  // namespace engine_case\n")


# ---- the calls: every node on the real function (kern/logic/engine_host.h) ----
SCALAR_TAG = {"Bool": "Bool", "Int": "Int", "Float": "Float", "Vector": "Vector", "Text": "Text", "Int64": "Int64", "Actor": "Actor"}


def demangled(names):
    """The C++ names of mangled names, with every type spelled in full (c++filt): {mangled: demangled}."""
    names = sorted({n for n in names if n})
    if not names:
        return {}
    out = subprocess.run(["c++filt"], input="\n".join(names) + "\n", capture_output=True, text=True, check=True).stdout.splitlines()
    assert len(out) == len(names)
    return dict(zip(names, out))


def portable(t):
    """A type as the library spells it underneath (std::__cxx11::basic_string, std::__1::vector) as the standard
    spells it: the inline namespaces of libstdc++ and libc++ are left out - the type is the same in both."""
    return re.sub(r"\b(?:__cxx11|__1)::", "", t).strip()


# Which whole number a type of this platform is, by the name the declaration gives it: the compiler spells the
# type underneath (unsigned long), and that is a different type on another platform (u64 is unsigned long long on
# macOS, size_t is unsigned long there) - so the call is written with the declaration's own name for it.
WIDTH_NAMES = {"unsigned long": [(r"\b(?:std::)?size_t\b", "std::size_t"), (r"\b(?:kern::)?u64\b|\b(?:std::)?uint64_t\b", "kern::u64")],
               "long": [(r"\b(?:std::)?ptrdiff_t\b", "std::ptrdiff_t"), (r"\b(?:kern::)?i64\b|\b(?:std::)?int64_t\b", "kern::i64")]}
WIDTH_TYPE = re.compile(r"(?<![\w ])(unsigned long|long)(?! long| double)\b|(?<=[<(, ])(unsigned long|long)(?! long| double)\b")


def fixed_width(P, declared, aliases):
    """A parameter type with each whole number of a platform's width (unsigned long, long) spelled as the
    declaration names it (std::size_t, kern::u64, ...) - a name the program gives a type (using VoiceId = u64)
    stands for the type it names."""
    known = {"size_t", "ptrdiff_t", "u64", "uint64_t", "i64", "int64_t"}   # (the names WIDTH_NAMES tells by)
    for _ in range(8):
        named = re.sub(r"\b(?:\w+::)*(\w+)\b", lambda m: m.group(0) if m.group(1) in known or m.group(1) not in aliases else f"({aliases[m.group(1)]})", declared)
        if named == declared:
            break
        declared = named
    def name(m):
        t = m.group(1) or m.group(2)
        found = {spell for pattern, spell in WIDTH_NAMES[t] if re.search(pattern, declared)}
        assert len(found) == 1, f"the type {P} (declared {declared}): which {t} it is cannot be told from its name"
        return found.pop()
    return WIDTH_TYPE.sub(name, P)


def parameters_of(name):
    """The parameter types of a demangled function name: kern::World::cast(kern::Pose const&, unsigned int) const."""
    s = re.sub(r"(\s*(const|volatile|&|&&|noexcept))+\s*$", "", name.strip())
    assert s.endswith(")"), name
    depth, at = 0, len(s) - 1
    for i in range(len(s) - 1, -1, -1):
        depth += {")": 1, "(": -1}.get(s[i], 0)
        if depth == 0:
            at = i
            break
    inner = s[at + 1:-1]
    parts, depth, cur = [], 0, ""
    for ch in inner:
        if ch in "<([":
            depth += 1
        elif ch in ">)]":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip() and cur.strip() != "void":
        parts.append(cur)
    return [portable(x) for x in parts]


def component_of(path):
    """The part of the engine a header belongs to: kern/include/kern/<part>/ - its sources are kern/src/<part>/ (a
    build links the parts kern/build.sh says)."""
    m = re.search(r"/include/kern/(\w+)/", path or "")
    assert m and os.path.isdir(os.path.join(KERN, "src", m.group(1))), path
    return m.group(1)


class Calls:
    """The calls of the nodes on the engine itself, one file per component (src/<component>/engine_calls.gen.cpp)."""

    def __init__(self, api, functions):
        self.api, self.u, self.tree = api, api.u, api.tree
        self.functions = functions
        self.names = demangled([f["decl"].get("mangledName") for f in functions if f.get("role") != "release"])
        self.enum_q = {name: q for q, name in self.u.enums.items()}
        self.struct_q = {name: q for q, name in self.u.structs.items() if name}

    # ---- the interfaces of the engine: a Blueprint as the object the engine calls (kern/docs/blueprint_cpp.md,
    # "Schnittstellen der Engine") ----
    def interfaces(self):
        """Every class of the engine with virtual functions that are nodes: [(class, its Blueprint name, [(node, pure,
        why a Blueprint has it not)], why no Blueprint implements it - "" where one can)], in the order of the table.
        The adapter hands what the engine hands a virtual function to the Blueprint as values - an object of the
        engine as its handle - and takes what it gives back; a function the engine hands a pointer, or one that
        writes through what it is handed, has no such way yet: the interface's own runs there - and where it has
        none (a pure one), no Blueprint implements the interface."""
        by = collections.OrderedDict()
        for f in self.functions:
            d = f["decl"]
            if f.get("role") is None and d.get("kind") == "CXXMethodDecl" and d.get("virtual"):
                by.setdefault(self.tree.owner[d["id"]], []).append(f)
        out = []
        for q, fs in by.items():
            virtuals = [(f, bool(f["decl"].get("pure")), self.unhanded(f)) for f in fs]
            waits = next((f"{f['decl']['name']}() {why}" for f, pure, why in virtuals if pure and why), "")
            out.append((q, c2b.type_name_of(q), virtuals, waits))
        return out

    def handed(self, f):
        """How the adapter hands each argument of the virtual function f to a Blueprint and takes back what it writes
        there: [(kind, ...)] per parameter, or a text - why it cannot. Kinds: ("handle", class) an object of the engine;
        ("value", pin) a value; ("list", pin, item type, its count's parameter, per) the numbers a pointer shows, as
        many as the Count beside it says (`send(to, const u8* data, Count<1> bytes)`); ("both", pin) a value it writes
        through a reference - handed in, set to what the Blueprint gives as that output (`receive(Address& from, ...)`);
        ("into", out pin, its count's parameter, per) the numbers the Blueprint gives as that output, written through
        the pointer - no more than the count says room for (`receive(..., u8* data, Count<1> max_bytes)`)."""
        params, ptypes, out = f["params"], self.ptypes(f), []
        names = [pin_in for pin_in, _ in params]
        for i, ((pin_in, pin_out), P) in enumerate(zip(params, ptypes)):
            pointer = P.rstrip().endswith("*")
            if pin_in is None:
                if pin_out not in f.get("counts", {}):
                    return "writes through a pointer without its count"
                out.append(("into", pin_out, names.index(f["counts"][pin_out]), f.get("per", {}).get(pin_out, 1)))
                continue
            bt, h = next((bt, h) for n_, bt, h in f["ins"] if n_ == pin_in)
            if h:
                if pointer:
                    return "is handed an object of the engine that may be missing"
                out.append(("handle", h))
                continue
            if pointer:
                if pin_out is not None or not re.search(r"\bconst\s*\*\s*$|^const\b", P.strip()):
                    return "writes through a pointer it is handed"
                per = c2b.count_per(ptypes[i + 1]) if i + 1 < len(ptypes) else None
                if per is None:
                    return "is handed a pointer without its count"
                x = re.sub(r"\s*\bconst\s*\*\s*$|\*\s*$", "", re.sub(r"^const\s+", "", P.strip())).strip()
                out.append(("list", bt, x, i + 1, per))
                continue
            if pin_out is not None:
                out.append(("both", bt, pin_out))
                continue
            out.append(("value", bt))
        return out

    def unhanded(self, f):
        """Why the adapter cannot hand the engine's arguments of the virtual function f to a Blueprint ("": it can)."""
        how = self.handed(f)
        return how if isinstance(how, str) else ""

    def adapter_text(self, q, name, virtuals, need):
        """The engine's object of the interface q that a Blueprint is (engine_host.h, Implementer): each virtual
        function calls the Blueprint's of its name - one that is not pure the interface's own where the Blueprint
        has none."""
        cls, lines, kept = f"Implements_{name}", [], []
        for f, pure, why in virtuals:
            if why:
                continue   # (the interface's own runs: interfaces)
            d = f["decl"]
            m, const = d["name"], " const" if c2b.qtype(d).rstrip().endswith("const") else ""
            ptypes = self.ptypes(f)
            ret = f"decltype(std::declval<{'const ' if const else ''}{q}&>().{m}({', '.join(f'std::declval<{P}>()' for P in ptypes)}))"
            params = ", ".join(f"{P} kern_p{k}" for k, P in enumerate(ptypes))
            lines.append(f"  {ret} {m}({params}){const} override {{")
            if not pure:
                lines.append(f'    if (!by_->has("{m}")) return {q}::{m}({", ".join(f"kern_p{k}" for k in range(len(ptypes)))});')
            args, back = [], []
            outs_at = {o: j for j, (o, _, _) in enumerate(f["outs"])}
            for k, how in enumerate(self.handed(f)):
                if how[0] == "handle":
                    args.append(f"Value::object(o_.ref(o_.adopt<{how[1]}>(kern_p{k})))")
                elif how[0] == "value":
                    args.append(f"ev::Conv<{self.tag(how[1], need)}>::to(o_, kern_p{k})")
                elif how[0] == "list":
                    _, bt, x, at, per = how
                    args.append(f"ev::Conv<{self.tag(bt, need)}>::to(o_, kern::Span<const {x}>(kern_p{k}, static_cast<std::size_t>(ev::numbers(kern_p{at}))))")
                elif how[0] == "both":
                    args.append(f"ev::Conv<{self.tag(how[1], need)}>::to(o_, kern_p{k})")
                    back.append(f"if (kern_given && kern_out.size() > {outs_at[how[2]]}) ev::Conv<{self.tag(how[1], need)}>::from(o_, kern_out[{outs_at[how[2]]}], kern_p{k});")
                else:   # (into: the numbers the Blueprint gives, no more than the room the count says)
                    _, o_name, at, per = how
                    bt = f["outs"][outs_at[o_name]][1]
                    j = outs_at[o_name]
                    back += [f"if (kern_given && kern_out.size() > {j}) {{",
                             f"  std::vector<std::remove_pointer_t<decltype(kern_p{k})>> kern_w;",
                             f"  ev::Conv<{self.tag(bt, need)}>::from(o_, kern_out[{j}], kern_w);",
                             f"  std::copy_n(kern_w.begin(), std::min(kern_w.size(), static_cast<std::size_t>(ev::numbers(kern_p{at}))), kern_p{k});",
                             "}"]
            lines.append(f"    const std::vector<Value> kern_in{{{', '.join(args)}}};" if args else "    const std::vector<Value> kern_in;")
            lines.append("    std::vector<Value> kern_out;")
            if not f["outs"]:
                lines += [f'    by_->call("{m}", kern_in, kern_out);', "  }"]
                continue
            if back:
                lines.append(f'    const bool kern_given = by_->call("{m}", kern_in, kern_out) && !kern_out.empty();')
                lines += ["    " + x for x in back]
                if c2b.qtype(d).split("(")[0].strip() == "void":
                    lines.append("  }")
                    continue
                bt = f["outs"][0][1]
                lines += [f"    {ret} kern_r = ev::blank<{ret}>();", f"    if (kern_given) ev::Conv<{self.tag(bt, need)}>::from(o_, kern_out[0], kern_r);", "    return kern_r;", "  }"]
                continue
            bt = f["outs"][0][1]
            tg = self.tag(bt, need)
            raw = c2b.qtype(d).split("(")[0].strip()
            lines.append(f'    const bool kern_given = by_->call("{m}", kern_in, kern_out) && !kern_out.empty();')
            if re.match(r"(?:kern::)?Span<", raw):
                # (a view of what the Blueprint gave: kept here until the engine asks again)
                member = f"kern_kept_{m}"
                kept.append(f"  mutable std::vector<std::remove_cv_t<std::remove_pointer_t<decltype(std::declval<{ret}>().data)>>> {member};")
                lines += [f"    {member}.clear();", f"    if (kern_given) ev::Conv<{tg}>::from(o_, kern_out[0], {member});",
                          f"    return {ret}({member}.data(), {member}.size());", "  }"]
                continue
            zero = '""' if re.fullmatch(r"const char\s*\*", raw) else f"ev::blank<{ret}>()"
            lines += [f"    {ret} kern_r = {zero};", f"    if (kern_given) ev::Conv<{tg}>::from(o_, kern_out[0], kern_r);", "    return kern_r;", "  }"]
        return [f"// {q}: the object of the engine a Blueprint is (Graph::implements \"{name}\")",
                f"class {cls} final : public {q} {{",
                " public:",
                f"  {cls}(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {{}}",
                *lines,
                "",
                " private:",
                "  EngineObjects& o_;",
                "  std::shared_ptr<Implementer> by_;",
                *kept,
                "};",
                f"i64 implements_{name}(EngineObjects& o, std::shared_ptr<Implementer> by) {{",
                f"  return o.own<{cls}, {q}>(std::make_unique<{cls}>(o, std::move(by)));",
                "}", ""]

    def class_adapter_text(self, q, name, virtuals, need):
        """The engine's object of the interface q that a Blueprint's class is (kern::logic::implementation; the class
        derives from kern::Implements<q>): each virtual function calls the class's own function of its name, typed
        - what the engine hands it made into the class's types where the class reads it (engine_values::Given; an
        object of the engine its handle), what the function gives made into the engine's (Direct::from); one the class
        has not, and one the engine cannot hand to a Blueprint, is the interface's own."""
        EV, cls, lines, kept, has = self.EV, f"ClassImplements_{name}", [], [], []
        for f, pure, why in virtuals:
            d = f["decl"]
            m, const = d["name"], " const" if c2b.qtype(d).rstrip().endswith("const") else ""
            ptypes = self.ptypes(f)
            ret = f"decltype(std::declval<{'const ' if const else ''}{q}&>().{m}({', '.join(f'std::declval<{P}>()' for P in ptypes)}))"
            params = ", ".join(f"{P} kern_p{k}" for k, P in enumerate(ptypes))
            own = f"{q}::{m}({', '.join(f'kern_p{k}' for k in range(len(ptypes)))})"
            has.append(f"  template <class X> static constexpr bool kern_has_{m} = requires {{ requires std::is_same_v<typename MemberOf<decltype(&X::{m})>::type, X>; }};")
            lines.append(f"  {ret} {m}({params}){const} override {{")
            if why:
                lines += [f"    return {own};   // (the engine cannot hand it to a Blueprint: it {why})", "  }"]
                continue
            if pure:   # (one the interface has no body of: the class has it - check() says so of its graph, the compiler here)
                lines.append(f'    static_assert(kern_has_{m}<B>, "a Blueprint that implements {name} has its function {m}");')
                lines.append("    {")
            else:
                lines.append(f"    if constexpr (!kern_has_{m}<B>) {{")
                lines.append(f"      return {own};")
                lines.append("    } else {")
            lines.append("      ::kern::logic::EngineObjects& kern_o = kern_objects();")
            args, back = [], []
            outs_at = {o: j for j, (o, _, _) in enumerate(f["outs"])}
            for k, how in enumerate(self.handed(f)):
                P = ptypes[k]
                if how[0] == "handle":
                    args.append(f"{EV}object_as<::kern::logic::Object<{how[1]}>>(kern_o, kern_o.adopt<{how[1]}>(kern_p{k}))")
                elif how[0] in ("value", "both"):
                    args.append(f"{EV}Given<{self.dtag(how[1], need)}, {P}{'' if P.rstrip().endswith('&') else ' const&'}>(kern_o, kern_p{k})")
                    if how[0] == "both":
                        back.append(f"{EV}Direct<{self.dtag(how[1], need)}>::from(kern_o, kern_g{outs_at[how[2]]}, kern_p{k});")
                elif how[0] == "list":
                    _, bt, x, at, per = how
                    args.append(f"{EV}Given<{self.dtag(bt, need)}, ::kern::Span<const {x}>>(kern_o, ::kern::Span<const {x}>(kern_p{k}, static_cast<std::size_t>({EV}numbers(kern_p{at}))))")
                else:
                    _, o_name, at, per = how
                    j = outs_at[o_name]
                    back += [f"std::vector<std::remove_pointer_t<decltype(kern_p{k})>> kern_w{k};",
                             f"{EV}Direct<{self.dtag(f['outs'][j][1], need)}>::from(kern_o, kern_g{j}, kern_w{k});",
                             f"std::copy_n(kern_w{k}.begin(), std::min(kern_w{k}.size(), static_cast<std::size_t>({EV}numbers(kern_p{at}))), kern_p{k});"]
            call = f"b_.{m}({', '.join(args)})"
            if not f["outs"]:
                lines += [f"      {call};", "    }", "  }"]
                continue
            tg = self.dtag(f["outs"][0][1], need)
            raw = c2b.qtype(d).split("(")[0].strip()
            if back:
                lines.append(f"      const auto& [{', '.join(f'kern_g{j}' for j in range(len(f['outs'])))}] = {call};")
                lines += ["      " + x for x in back]
                if raw == "void":
                    lines += ["    }", "  }"]
                    continue
                lines += [f"      auto kern_r = {EV}blank<{ret}>();", f"      {EV}Direct<{tg}>::from(kern_o, kern_g0, kern_r);", "      return kern_r;", "    }", "  }"]
                continue
            lines.append(f"      const auto& [kern_given] = {call};")
            if re.match(r"(?:kern::)?Span<", raw):
                member = f"kern_kept_{m}"
                kept.append(f"  mutable std::vector<std::remove_cv_t<std::remove_pointer_t<decltype(std::declval<{ret}>().data)>>> {member};")
                lines += [f"      {member}.clear();", f"      {EV}Direct<{tg}>::from(kern_o, kern_given, {member});",
                          f"      return {ret}({member}.data(), {member}.size());", "    }", "  }"]
                continue
            lines += [f"      auto kern_r = {EV}blank<{ret}>();", f"      {EV}Direct<{tg}>::from(kern_o, kern_given, kern_r);", "      return kern_r;", "    }", "  }"]
        return [f"// {q} that a Blueprint's class is (kern::Implements<{q}>: kern::logic::implementation)",
                f"template <class B> class {cls} final : public {q} {{",
                " public:",
                f"  explicit {cls}(B& b) : b_(b) {{}}",
                *has,
                *lines,
                "",
                " private:",
                "  B& b_;",
                "  mutable ::kern::logic::EngineObjects kern_own_;   // (a class without the engine's objects: the handles it is handed are these)",
                "  ::kern::logic::EngineObjects& kern_objects() const { return b_.kern_world.engine ? *b_.kern_world.engine : kern_own_; }",
                *kept,
                "};",
                f'template <> struct InterfaceOf<{q}> {{',
                f"  template <class B> using adapter = {cls}<B>;",
                "};", ""]

    # ---- the Blueprint type of a pin as a tag (engine_values) ----
    def tag(self, bt, need, ev="ev::", types=""):
        """`ev`, `types`: how the file the tag stands in names engine_values and the tool's own types (E_, S_)."""
        u = self.u
        if c2b.wire(bt) in SCALAR_TAG:   # (an entry of a fixed width - of a List<U8> - is an Int)
            return ev + SCALAR_TAG[c2b.wire(bt)]
        if c2b.object_class(bt):   # (an object of the engine: Object<I>, its ObjectRef)
            return ev + "Handle"
        if c2b.callback_parts(bt):
            r, a = c2b.callback_parts(bt)
            return f"{ev}Callback<{self.tag(r, need, ev, types) if r else 'void'}({', '.join(self.tag(x, need, ev, types) for x in a)})>"
        if bt.startswith("List<"):
            return f"{ev}List<{self.tag(bt[5:-1], need, ev, types)}>"
        if bt in u.enum_values:
            need.add(("enum", bt))
            return f"{ev}Enum<{types}E_{bt}>"
        if bt in u.rows:
            return f"{ev}Row<{self.tag(u.rows[bt], need, ev, types)}>"
        q = self.struct_q.get(bt)
        if isinstance(q, tuple) and q[0] == "pair":
            return f"{ev}Pair<{self.tag(q[1], need, ev, types)}, {self.tag(q[2], need, ev, types)}>"
        if isinstance(q, str):
            if ("struct", bt) not in need:
                need.add(("struct", bt))
                for _, ft, _, _ in u.struct_fields[bt]:
                    self.tag(ft, need, ev, types)
            return f"{ev}Struct<{types}S_{bt}>"
        raise SystemExit(f"engine_api.py: no tag for the Blueprint type {bt}")

    def holds_object(self, f):
        """Does an input of f that is no object itself hold one (a field of a struct, an entry of a list: Object<I>)?
        Then a reference in it of another world, or of no object, fails the call before the engine is called
        (engine_values::object_in) - the door and the class alike."""
        for _, bt, h in f["ins"]:
            if h:
                continue
            need = set()
            self.tag(bt, need)
            types = [bt] + [ft for kind, sb in need if kind == "struct" for _, ft, _, _ in self.u.struct_fields[sb]]
            while types:
                t = types.pop()
                if c2b.object_class(t):
                    return True
                if t.startswith("List<"):
                    types.append(t[5:-1])
                elif t in self.u.rows:
                    types.append(self.u.rows[t])
                elif isinstance(self.struct_q.get(t), tuple):
                    types += [self.struct_q[t][1], self.struct_q[t][2]]
        return False

    def bases(self, q):
        """The classes of the engine `q` derives from that are objects of it too, the nearest first."""
        out = []
        for b in self.tree.records[q].get("bases", []) or []:
            try:
                bq = self.u.find_type(self.u.clean(c2b.qtype(b)))
            except Wait:
                bq = None
            if bq is not None and bq in self.tree.records and self.u.engine_object(bq) and bq not in out:
                out.append(bq)
                out += [x for x in self.bases(bq) if x not in out]
        return out

    def kinds(self, q):
        return ", ".join([q, *self.bases(q)])

    def enums_text(self, need):
        """The enums of the engine a pin names: their values in the order of the Blueprint's enum."""
        lines = []
        for kind, bt in sorted(need):
            if kind == "enum":
                q = self.enum_q[bt]
                seen, values = set(), []
                for n, x in c2b.enum_numbers(self.tree.enums[q]):
                    if x not in seen:
                        seen.add(x)
                        values.append(f"{q}::{n}")
                lines.append(f"struct E_{bt} {{ using type = {q}; static constexpr type values[] = {{{', '.join(values)}}}; }};")
        return lines

    def types_text(self, need):
        u, lines, defs = self.u, self.enums_text(need), []
        for kind, bt in sorted(need):
            if kind == "struct":
                q = self.struct_q[bt]
                lines.append(f"struct S_{bt} {{ [[maybe_unused]] static Value to(EngineObjects& o, const {q}& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, {q}& x); }};")
                fields = u.struct_fields[bt]
                tags = [self.tag(ft, set()) for _, ft, _, _ in fields]
                to = ", ".join(f"ev::Conv<{tg}>::to(o, x.{c})" for (_, _, _, c), tg in zip(fields, tags))
                defs.append(f"Value S_{bt}::to(EngineObjects& o, const {q}& x) {{ return Value::structure({{{to}}}); }}")
                body = " ".join(f"if (const Value* f = v.field({k})) ev::Conv<{tg}>::from(o, *f, x.{c});" for k, ((_, _, _, c), tg) in enumerate(zip(fields, tags)))
                defs.append(f"void S_{bt}::from(EngineObjects& o, const Value& v, {q}& x) {{ {body} }}")
        return lines + defs

    def ptypes(self, f):
        """The types of the C++ function's parameters, as the compiler has them (from its mangled name), each
        whole number of a platform's width by the name the declaration gives it (fixed_width)."""
        d = f["decl"]
        mangled = d.get("mangledName")
        ptypes = parameters_of(self.names[mangled]) if mangled else []
        if f.get("printf") and ptypes and ptypes[-1].strip() == "...":
            ptypes = ptypes[:-1]   # (the values of its format: the node takes the text they make - Api.printf_format)
        declared = [x.get("type", {}).get("qualType", "") for x in d.get("inner", []) or [] if x.get("kind") == "ParmVarDecl"]
        if len(declared) == len(ptypes):
            ptypes = [fixed_width(P, t, self.tree.aliases) for P, t in zip(ptypes, declared)]
        assert len(ptypes) == len(f["params"]), (f["type"], ptypes, f["params"])
        return ptypes

    @staticmethod
    def outside_text(f):
        """What the engine says where the index a place of an entry of an array is bound at names no entry
        (EngineFunction::place with Api.function1's "index": C++ leaves such a call open)."""
        return f"the input {f['ins'][1][0]} names no entry: there are {f['index']}"

    def body(self, f, need):
        """The lines of the call of a node: its inputs made into the arguments, the call, its outputs."""
        d, role, tree = f["decl"], f.get("role"), self.tree
        if role == "release":
            q = f["class"]
            pin0 = c_text(f["ins"][0][0])
            return ["ObjectRef r;", f"if (!ev::object_ref(o, in, 0, {pin0}, r)) return false;",
                    f"return ev::release_of<{q}>(o, r, {pin0}, {c_text(q.split('::')[-1])});"]
        if f.get("field"):   # (a field of the object: what stands there - "<type>@get" - nothing is called)
            q, (pin0, _, _) = f["class"], f["ins"][0]
            return [f'auto* self = ev::object<{q}>(o, in, 0, {c_text(pin0)});', "if (!self) return false;",
                    f"ev::give<{self.tag(f['outs'][0][1], need)}>(o, out, 0, self->{d['name']});", "return true;"]
        mangled = d.get("mangledName")
        ptypes = self.ptypes(f)
        parms = [p for p in c2b.kids(d) if p.get("kind") == "ParmVarDecl"]   # (their declarations: an array's length)
        params = f["params"]
        in_at = {name: k for k, (name, _, _) in enumerate(f["ins"])}
        out_at = {name: k for k, (name, _, _) in enumerate(f["outs"])}
        pin = {name: (bt, h) for name, bt, h in f["ins"]}
        outs = {name: (bt, h) for name, bt, h in f["outs"]}
        lines, later, after, args = [], [], [], []
        method = role is None and d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static"
        obj = ""
        if method:
            owner = tree.owner[d["id"]]
            name0, bt0, h0 = f["ins"][0]
            if h0:
                lines += [f'auto* self = ev::object<{owner}>(o, in, 0, "{name0}");', "if (!self) return false;"]
                obj = "self->"
            else:
                lines += [f"auto self = ev::blank<{owner}>();", f"ev::take<{self.tag(bt0, need)}>(o, in, 0, self);"]
                obj = "self."
                if "self" in out_at:
                    after.append(f"ev::give<{self.tag(outs['self'][0], need)}>(o, out, {out_at['self']}, self);")
        arg_of = {}
        for i, ((pin_in, pin_out), P) in enumerate(zip(params, ptypes)):
            a = f"a{i}"
            if pin_in is not None:
                arg_of[pin_in] = a
        for i, ((pin_in, pin_out), P) in enumerate(zip(params, ptypes)):
            a = f"a{i}"
            pointer = P.endswith("*")
            ref = P.endswith("&") and not P.endswith("&&")
            if pin_in is None and pin_out in f.get("counts", {}):
                x = P[:-1].strip()
                per = f.get("per", {}).get(pin_out, 1)
                later += [f"ev::Written<{x}> {a}(ev::room({arg_of[f['counts'][pin_out]]}) * {per});"]
                args.append(f"{a}.data()")
                after += [f'if (!{a}.kept(o, "{pin_out}")) return false;', f"ev::give<{self.tag(outs[pin_out][0], need)}>(o, out, {out_at[pin_out]}, {a}.room);"]
                continue
            if pin_in is None:   # (what it reports if it is asked to: here it is asked)
                x = P[:-1].strip()
                later.append(f"auto {a} = ev::blank<{x}>();")
                args.append(f"&{a}")
                after.append(f"ev::give<{self.tag(outs[pin_out][0], need)}>(o, out, {out_at[pin_out]}, {a});")
                continue
            bt, h = pin[pin_in]
            k = in_at[pin_in]
            if h:
                if re.match(r"std::unique_ptr<", P):
                    raise SystemExit(f"engine_api.py: {f['type']} takes over an object of the engine (std::unique_ptr) - no node can hand it one")
                if pointer:
                    lines += [f'{h}* {a} = ev::maybe<{h}>(o, in, {k}, "{pin_in}");', "if (!o.error.empty()) return false;"]
                    args.append(a)
                else:
                    lines += [f'{h}* {a} = ev::object<{h}>(o, in, {k}, "{pin_in}");', f"if (!{a}) return false;"]
                    args.append(f"*{a}")
                continue
            tg = self.tag(bt, need)
            if P in ("char const*", "char const* const"):
                lines += [f"std::string {a};", f"ev::take<{tg}>(o, in, {k}, {a});"]
                if pin_in == f.get("printf"):   # (the text its format makes: written as it is)
                    args.append('"%s"')
                args.append(f"{a}.c_str()")
            elif re.match(r"std::basic_string_view<char\b", P):
                lines += [f"std::string {a};", f"ev::take<{tg}>(o, in, {k}, {a});"]
                args.append(a)
            elif pointer:
                x = re.sub(r"\s*\bconst$", "", P[:-1].strip())
                lines += [f"std::vector<{x}> {a};", f"ev::take<{tg}>(o, in, {k}, {a});"]
                args.append(f"{a}.empty() ? nullptr : {a}.data()")
                fixed = c2b.declared_length(tree, parms[i]) if i < len(parms) else None
                if fixed:   # (an array of a fixed length: the engine reads so many)
                    later.append(f'if ({a}.size() < {fixed}) {{ o.error = "the list {pin_in} has fewer than {fixed} entries"; return false; }}')
                nxt = params[i + 1][0] if i + 1 < len(params) else None
                if nxt is not None and c2b.count_per(ptypes[i + 1]) is not None:   # (the type says it: kern::Count<Per>)
                    # (the first of as many as the count beside it says - Count<Per>: Per numbers each -: no more than the list has - the engine reads them)
                    after_take = f'if (ev::numbers(a{i + 1}) > {a}.size()) {{ o.error = "the list {pin_in} has fewer entries than {nxt} says"; return false; }}'
                    later.append(after_take)
            elif re.match(r"kern::Span<", re.sub(r"\s*\bconst\s*&$|\s*&$", "", P)):
                span = re.sub(r"\s*\bconst\s*&$|\s*&$", "", P)
                x = re.sub(r"\s*\bconst$", "", c2b.template_args(span)[0].strip())
                lines += [f"std::vector<{x}> {a};", f"ev::take<{tg}>(o, in, {k}, {a});"]
                args.append(f"{span}({a}.data(), {a}.size())")
            else:
                lines += [f"auto {a} = ev::blank<std::remove_cvref_t<{P}>>();", f"ev::take<{tg}>(o, in, {k}, {a});"]
                args.append(a if ref else f"std::move({a})")   # (a value it takes: handed over, as nothing reads it after)
                if pin_out is not None:
                    after.append(f"ev::give<{self.tag(outs[pin_out][0], need)}>(o, out, {out_at[pin_out]}, {a});")
        lines += later
        if self.holds_object(f):   # (an Object in a value that names nothing here: object_in said why)
            lines.append("if (!o.error.empty()) return false;")
        if f.get("index"):   # (a place of an entry of an array the object holds: its index names one of them, or the call fails)
            a = arg_of[f["ins"][1][0]]
            lines.append(f"if (static_cast<std::size_t>({a}) >= {f['index']}) {{ o.error = {c_text(self.outside_text(f))}; return false; }}")
        if role == "make":
            q = f["class"]
            made = f"new {q}({', '.join(args)})" if mangled else f"new {q}()"
            lines += [f"out[0] = Value::object(o.ref(o.own<{self.kinds(q)}>(std::unique_ptr<{q}>({made}))));", "return true;"]
            return lines
        call = f"{obj}{d['name']}({', '.join(args)})" if method else f"{f['cpp']}({', '.join(args)})"
        ret = c2b.qtype(d).split("(")[0].strip()
        if ret == "void":
            lines.append(call + ";")
        else:
            lines.append(f"decltype(auto) r = {call};")
            bt, h = outs["result"]
            h = h or c2b.object_class(bt)   # (one it gives owned - std::unique_ptr<Transport>: an Object of it)
            if h:
                after.insert(0, f"out[0] = Value::object(o.ref(ev::handle_of<{self.kinds(h)}>(o, std::forward<decltype(r)>(r))));")
            else:
                after.insert(0, f"ev::give<{self.tag(bt, need)}>(o, out, 0, r);")
        return lines + after + ["return true;"]

    # ---- the class: every node as a function of a Blueprint's class (kern/logic/engine_direct.h) ----
    EV, TY = "::kern::logic::engine_values::", "::kern::logic::engine_direct::"

    def dtag(self, bt, need):
        return self.tag(bt, need, self.EV, self.TY)

    # (the pins the class has as a C++ type of the language: an input of any other type is a template - it may name a
    # place of the engine, an entry of `b.chunks()` or a field of it: read where the input is taken, input_value)
    PLAIN = {"Bool": "Bool", "Int": "Int", "Float": "Float", "Vector": "Vector", "Text": "const Text&", "Int64": "Int64", "Actor": "Actor"}

    def templated(self, bt):
        return bt not in self.PLAIN and not (bt.startswith("List<") and bt[5:-1] in self.PLAIN) and not c2b.object_class(bt)

    @staticmethod
    def object_cpp(bt):
        """The C++ type of the class of an Object<q> pin (an object of a class that derives from q goes there too)."""
        return f"::kern::logic::Object<::{c2b.object_class(bt)}>"

    def taken_in(self, name, bt):
        """An input of the class's function as it is taken: what stands at a place it names, or the value."""
        return f"{self.EV}input_value({name})" if self.templated(bt) else name

    def direct(self, f, need, mode, fail):
        """The lines of the call of a node by a Blueprint's class itself: the same steps in the same order as
        `body` writes them for the door, on the C++ types of the class instead of values. `mode`: "value" (a
        value node whose pin the class holds as a C++ type of the language - the function returns it), "given"
        (a value node of a struct, an enum or a list of them: it returns what the function gave, read as the
        Blueprint's type where it is used), "read" (a step whose outputs go to the node's member kern_n) or
        "unread" (a step nobody reads the outputs of). `fail(given)`: what a call that fails does, when the
        outputs `given` have been given already."""
        d, role, tree, EV = f["decl"], f.get("role"), self.tree, self.EV
        O = "*kern_o"
        given = []

        def give(pin, bt, value):
            given.append(pin)
            tg = self.dtag(bt, need)
            if mode != "read":
                return f"{EV}Direct<{tg}>::pass({O}, {value});"
            return f"kern_n.{pin} = {EV}Direct<{tg}>::template make<std::remove_cvref_t<decltype(kern_n.{pin})>>({O}, {value});"

        def give_handle(pin, handle):
            given.append(pin)
            return f"kern_n.{pin} = {EV}object_as<std::remove_cvref_t<decltype(kern_n.{pin})>>({O}, {handle});" if mode == "read" else f"(void){handle};"
        lines = ["kern_o->begin_call();"]
        if role == "release":
            q = f["class"]
            name0 = f["ins"][0][0]
            return lines + [f'if (!{EV}release_of<{q}>({O}, {name0}.ref, "{name0}", "{q.split("::")[-1]}")) {fail(given)}'], None
        mangled = d.get("mangledName")
        ptypes = self.ptypes(f)
        parms = [p for p in c2b.kids(d) if p.get("kind") == "ParmVarDecl"]   # (their declarations: an array's length)
        params = f["params"]
        in_at = {name: k for k, (name, _, _) in enumerate(f["ins"])}
        pin = {name: (bt, h) for name, bt, h in f["ins"]}
        outs = {name: (bt, h) for name, bt, h in f["outs"]}
        later, after, args, declared = [], [], [], []
        method = role is None and d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static"
        obj = ""
        if method:
            owner = tree.owner[d["id"]]
            name0, bt0, h0 = f["ins"][0]
            if h0:
                lines += [f'auto* const kern_self = {EV}object_of<{owner}>({O}, {name0}.ref, "{name0}");', f"if (!kern_self) {fail(given)}"]
                obj = "kern_self->"
            else:
                lines += [f"auto kern_self = {EV}blank<{owner}>();", f"{EV}Direct<{self.dtag(bt0, need)}>::from({O}, {self.taken_in(name0, bt0)}, kern_self);"]
                obj = "kern_self."
                if "self" in outs:
                    after.append(lambda: give("self", outs["self"][0], "kern_self"))
        arg_of = {pin_in: f"kern_a{i}" for i, (pin_in, _) in enumerate(params) if pin_in is not None}
        for i, ((pin_in, pin_out), P) in enumerate(zip(params, ptypes)):
            a = f"kern_a{i}"
            declared.append(f"std::declval<{P}>()")
            pointer = P.endswith("*")
            ref = P.endswith("&") and not P.endswith("&&")
            reads = bool(re.search(r"\bconst\s*&$", P)) and pin_out is None   # (handed over as it stands, where it can be)
            if pin_in is None and pin_out in f.get("counts", {}):
                x = P[:-1].strip()
                per = f.get("per", {}).get(pin_out, 1)
                later += [f"{EV}Written<{x}> {a}({EV}room({arg_of[f['counts'][pin_out]]}) * {per});"]
                args.append(f"{a}.data()")
                after.append(lambda a=a, pin_out=pin_out: f'if (!{a}.kept({O}, "{pin_out}")) {fail(given)}')
                after.append(lambda a=a, pin_out=pin_out: give(pin_out, outs[pin_out][0], f"{a}.room"))
                continue
            if pin_in is None:   # (what it reports if it is asked to: here it is asked)
                x = P[:-1].strip()
                later.append(f"auto {a} = {EV}blank<{x}>();")
                args.append(f"&{a}")
                after.append(lambda a=a, pin_out=pin_out: give(pin_out, outs[pin_out][0], a))
                continue
            bt, h = pin[pin_in]
            if h:
                if pointer:
                    lines += [f'{h}* const {a} = {EV}maybe_of<{h}>({O}, {pin_in}.ref, "{pin_in}");', f"if (!kern_o->error.empty()) {fail(given)}"]
                    args.append(a)
                else:
                    lines += [f'{h}* const {a} = {EV}object_of<{h}>({O}, {pin_in}.ref, "{pin_in}");', f"if (!{a}) {fail(given)}"]
                    args.append(f"*{a}")
                continue
            tg = self.dtag(bt, need)
            given_in = self.taken_in(pin_in, bt)
            if P in ("char const*", "char const* const"):
                lines.append(f"const {EV}Chars {a}({pin_in});")
                if pin_in == f.get("printf"):   # (the text its format makes: written as it is)
                    args.append('"%s"')
                args.append(f"{a}.c_str()")
            elif re.match(r"std::basic_string_view<char\b", P):
                lines.append(f"const std::string_view {a} = {pin_in}.view();")
                args.append(a)
            elif bt == "Text" and reads and re.match(r"std::basic_string<char\b", P):
                lines.append(f"const {EV}String {a}({pin_in});")
                args.append(f"{a}.get()")
            elif pointer:
                x = re.sub(r"\s*\bconst$", "", P[:-1].strip())
                lines.append(f"decltype(auto) {a} = {EV}taken<{tg}, std::vector<{x}>>({O}, {given_in});")
                args.append(f"{a}.empty() ? nullptr : {a}.data()")
                fixed = c2b.declared_length(tree, parms[i]) if i < len(parms) else None
                if fixed:   # (an array of a fixed length: the engine reads so many)
                    later.append(f'if ({a}.size() < {fixed}) {{ kern_o->error = "the list {pin_in} has fewer than {fixed} entries"; {fail(given)} }}')
                nxt = params[i + 1][0] if i + 1 < len(params) else None
                if nxt is not None and c2b.count_per(ptypes[i + 1]) is not None:   # (the type says it: kern::Count<Per>)
                    # (the first of as many as the count beside it says - Count<Per>: Per numbers each -: no more than the list has - the engine reads them)
                    later.append(f'if ({EV}numbers(kern_a{i + 1}) > {a}.size()) {{ kern_o->error = "the list {pin_in} has fewer entries than {nxt} says"; {fail(given)} }}')
            elif re.match(r"kern::Span<", re.sub(r"\s*\bconst\s*&$|\s*&$", "", P)):
                span = re.sub(r"\s*\bconst\s*&$|\s*&$", "", P)
                item = c2b.template_args(span)[0].strip()
                x = re.sub(r"\s*\bconst$", "", item)
                if item != x:   # (a view of entries it only reads)
                    lines.append(f"decltype(auto) {a} = {EV}taken<{tg}, std::vector<{x}>>({O}, {given_in});")
                else:
                    lines += [f"std::vector<{x}> {a};", f"{EV}Direct<{tg}>::from({O}, {given_in}, {a});"]
                args.append(f"{span}({a}.data(), {a}.size())")
            elif reads:
                lines.append(f"decltype(auto) {a} = {EV}taken<{tg}, std::remove_cvref_t<{P}>>({O}, {given_in});")
                args.append(a)
            else:
                lines += [f"auto {a} = {EV}blank<std::remove_cvref_t<{P}>>();", f"{EV}Direct<{tg}>::from({O}, {given_in}, {a});"]
                args.append(a if ref else f"std::move({a})")   # (a value it takes: handed over, as nothing reads it after)
                if pin_out is not None:
                    after.append(lambda a=a, pin_out=pin_out: give(pin_out, outs[pin_out][0], a))
        lines += later
        if self.holds_object(f):   # (as the door: an Object in a value that names nothing here fails the call)
            lines.append(f"if (!kern_o->error.empty()) {fail(given)}")
        if f.get("index"):   # (a place of an entry of an array the object holds: its index names one of them, or the call fails)
            a = arg_of[f["ins"][1][0]]
            lines.append(f"if (static_cast<std::size_t>({a}) >= {f['index']}) {{ kern_o->error = {c_text(self.outside_text(f))}; {fail(given)} }}")
        if role == "make":
            q = f["class"]
            made = f"new {q}({', '.join(args)})" if mangled else f"new {q}()"
            handle = f"kern_o->own<{self.kinds(q)}>(std::unique_ptr<{q}>({made}))"
            return lines + [give_handle("result", handle)], None
        call = f"{obj}{d['name']}({', '.join(args)})" if method else f"{f['cpp']}({', '.join(args)})"
        returned = (f"std::declval<{tree.owner[d['id']]}&>().{d['name']}" if method else f["cpp"]) + f"({', '.join(declared)})"
        ret = c2b.qtype(d).split("(")[0].strip()
        if ret == "void":
            lines.append(call + ";")
        else:
            lines.append(f"decltype(auto) kern_r = {call};")
            bt, h = outs["result"]
            h = h or c2b.object_class(bt)   # (one it gives owned - std::unique_ptr<Transport>: an Object of it)
            handle = f"{EV}handle_of<{self.kinds(h)}>({O}, std::forward<decltype(kern_r)>(kern_r))" if h else None
            if mode == "value":
                lines += [f"return {EV}object_as<kern_T>({O}, {handle});" if h else f"return {EV}Direct<{self.dtag(bt, need)}>::template make<kern_T>({O}, kern_r);"]
            elif mode == "given":
                self.dtag(bt, need)
            elif h:
                lines.append(give_handle("result", handle))
            else:
                lines.append(give("result", bt, "kern_r"))
        lines += [x() for x in after]
        return lines, returned

    def class_text(self):
        """The functions of a Blueprint's class: each calls the function of the engine itself where the host
        gives the class the engine's objects - and else hands its inputs to the host's door and takes what it
        gives."""
        api, EV = self.api, self.EV
        plain = self.PLAIN
        need, bodies, headers, places = set(), [], set(), []

        def typed(bt):
            if bt in plain:
                return bt
            if bt.startswith("List<") and bt[5:-1] in plain:
                return bt
            if c2b.object_class(bt):
                return self.object_cpp(bt)
            return None
        for f in self.functions:
            headers.add(re.sub(r"^.*/include/", "", self.file_of(f)))
            name, node = f["type"].replace(".", "_"), c_text(f["type"])
            templates, params, values = [], [], []
            for name_, bt, _ in f["ins"]:
                if bt in plain:
                    params.append(f"{plain[bt]} {name_}")
                elif c2b.object_class(bt):
                    params.append(f"{self.object_cpp(bt)} {name_}")
                elif bt.startswith("List<") and bt[5:-1] in plain:
                    params.append(f"const List<{bt[5:-1]}>& {name_}")
                else:
                    templates.append(f"class T{len(templates) + 1}")
                    params.append(f"const T{len(templates)}& {name_}")
                values.append(f"to_value({self.taken_in(name_, bt)})")
            zeros = ", ".join(f"PinType::{api.pin(bt)[0]}" for _, bt, _ in f["outs"])
            door = f"this->kern_engine({node}, {{{', '.join(values)}}}, {{{zeros}}})"
            failed = f"this->kern_engine_failed({node});"
            lines = []

            def function(head, direct, through, start=()):
                lines.append(f"  {head} {{")
                lines.extend("    " + x for x in start)
                if direct is None:   # (the same in both ways)
                    lines.extend("    " + x for x in through)
                    lines.append("  }")
                    return
                lines.append("    if (::kern::logic::EngineObjects* const kern_o = this->kern_world.engine) {")
                lines.extend("      " + x for x in direct)
                lines.append("    }")
                lines.extend("    " + x for x in through)
                lines.append("  }")
            if f["help"]:
                lines.append(f"  /// {f['help']}")
            head = f"template <{', '.join(templates)}> " if templates else ""
            if f["value"]:
                bt = f["outs"][0][1]
                if typed(bt):
                    T = typed(bt)
                    direct, _ = self.direct(f, need, "value", lambda given: f"{{ {failed} return {T}{{}}; }}")
                    function(f"{head}{T} {name}({', '.join(params)}) const", [x.replace("kern_T", T) for x in direct],
                             [f"return from_value<{T}>({door}[0].converted(pin_type_of<{T}>()));"])
                else:
                    direct, returned = self.direct(f, need, "given", lambda given: f"{{ {failed} return kern_G(*kern_o); }}")
                    # (what it returns is named before the function: a function whose return type the compiler works
                    # out is made where it is first used - in the class, before the class's glue says how its
                    # structs are held as values)
                    lines.append(f"  using kern_given_{name} = {EV}Given<{self.dtag(bt, need)}, decltype({returned})>;")
                    function(f"{head}kern_given_{name} {name}({', '.join(params)}) const", direct + ["return kern_G(*kern_o, std::forward<decltype(kern_r)>(kern_r));"],
                             [f"return kern_G({door}[0]);"], [f"using kern_G = kern_given_{name};"])
                bodies += lines
                continue
            # a step: what it gives goes to the node's member (n.result ...), when the text reads it
            if not f.get("field"):   # (a field binds only: nothing is called - see below)
                direct, _ = self.direct(f, need, "unread", lambda given: f"{{ {failed} return; }}")
                function(f"{head}void {name}({', '.join(params)})", direct + ["return;"], [f"{door};"])
            if f.get("place"):
                # a place of the engine: the node binds it - its member n.result is the place (ev::Place), `auto& r =
                # object->f();` once; through the door the function is called as it is there, and the place is read
                # and set through it after (<node>@get, <node>@set)
                pin, _, q = f["ins"][0]
                m, out = f["decl"]["name"], f["outs"][0]
                named = m if f.get("field") else m + "()"   # (a field of the object, or what the method names)
                # (its further inputs as C++ has them - the index of an entry of an array the object holds: In; at() is
                # null where the index names no entry, and says so as the door says it - outside_text)
                more = [(n_, bt_, P_) for (n_, bt_, _), P_ in zip(f["ins"][1:], self.ptypes(f))]
                cpp_in = ", ".join(f"std::remove_cvref_t<{P_}>" for _, _, P_ in more)
                looked = m + "(" + ", ".join(f"std::declval<std::remove_cvref_t<{P_}>>()" for _, _, P_ in more) + ")" if more else named
                if more:
                    (n0, _, _), = more
                    at = [f"  static C* at(EngineObjects& o, Object& self, const In& in) {{",
                          f"    if (static_cast<std::size_t>(std::get<0>(in)) >= {f['index']}) {{ o.error = {c_text(self.outside_text(f))}; return nullptr; }}",
                          f"    return &self.{m}(std::get<0>(in));", "  }"]
                else:
                    at = [f"  static C* at(EngineObjects&, Object& self, const In&) {{ return &self.{named}; }}"]
                places.append([f"struct P_{name} {{",
                               f"  using Object = {q};",
                               f"  using C = std::remove_reference_t<decltype((std::declval<{q}&>().{looked}))>;",
                               f"  using Tag = {self.dtag(out[1], need)};",
                               f"  using In = std::tuple<{cpp_in}>;",
                               f"  static constexpr const char* pin = {c_text(pin)};",
                               f"  static constexpr const char* get = {c_text(f['type'] + '@get')};",
                               f"  static constexpr const char* set = {c_text(f['type'] + '@set')};",
                               *at,
                               "};"])
                lines.append(f"  /// The place {f['cpp']} gives (kern/logic/engine_direct.h, Place): the member of the node that binds it.")
                lines.append(f"  template <class B> using {name}_place = {EV}Place<engine_direct::P_{name}, B>;")
                handle = f"{pin}.ref"
                P_name = f"engine_direct::P_{name}"
                taken = []
                for k_, (n_, bt_, P_) in enumerate(more):
                    taken += [f"auto kern_a{k_} = {EV}blank<std::remove_cvref_t<{P_}>>();",
                              f"{EV}Direct<{self.dtag(bt_, need)}>::from(*kern_o, {self.taken_in(n_, bt_)}, kern_a{k_});"]
                bind = ["kern_o->begin_call();", f"auto* const kern_self = {EV}object_of<{q}>(*kern_o, {handle}, {c_text(pin)});",
                        f"typename {P_name}::In kern_in{{}};"]
                if more:
                    bind += ["if (kern_self) {", *["  " + x for x in taken], f"  kern_in = typename {P_name}::In{{{', '.join(f'kern_a{k_}' for k_ in range(len(more)))}}};", "}"]
                bind += [f"auto* const kern_at = kern_self ? {P_name}::at(*kern_o, *kern_self, kern_in) : nullptr;",
                         f"kern_n.{out[0]}.kern_bind(this->kern_world, {handle}, kern_in, kern_at);",
                         f"if (!kern_at) {failed}", "return;"]
                # (a field: binding it is no call - the door says nothing there; the object is looked up where the place
                # is first used, and a missing one is told there as <node>@get, as the door tells it)
                function(f"template <{', '.join(['class KernNode'] + templates)}> void {name}(KernNode& kern_n, {', '.join(params)})",
                         None if f.get("field") else bind,
                         [f"kern_n.{out[0]}.kern_bind(this->kern_world, {handle}, {{}}, nullptr);"] if f.get("field") else
                         [f"{door};", f"kern_n.{out[0]}.kern_bind(this->kern_world, {{{', '.join(values)}}});"])
                bodies += lines
                continue
            if f["outs"]:
                order = [o for o, _, _ in f["outs"]]

                def fail(given):
                    return "{ " + " ".join(f"{EV}none(kern_n.{o});" for o in order if o not in given) + f" {failed} return; }}"
                direct, _ = self.direct(f, need, "read", fail)
                takes = [f"kern_n.{o} = from_value<std::remove_cvref_t<decltype(kern_n.{o})>>(kern_out[{i}]);" for i, o in enumerate(order)]
                function(f"template <{', '.join(['class KernNode'] + templates)}> void {name}({', '.join(['KernNode& kern_n'] + params)})", direct + ["return;"],
                         [f"const std::vector<Value> kern_out = {door};", *takes])
            bodies += lines
        adapters = [x for q, name, virtuals, waits in self.interfaces() if not waits for x in self.class_adapter_text(q, name, virtuals, need)]
        text = ["// The engine's functions as a Blueprint's class calls them (written by tools/engine_api.py from the",
                "// engine's headers - see kern/logic/engine_api.h; not edited by hand). Included by kern/logic/blueprint.h.",
                "//",
                "// Each calls the function of the engine itself, typed, where the host gives the class the engine's objects",
                "// (BlueprintWorld::engine - kern/logic/engine_direct.h), and else hands its inputs as values to the host's",
                "// door (logic::Host::engine). Both do the same; the calls behind the door are written from the same table",
                "// (src/<component>/engine_calls.gen.cpp).",
                "#pragma once", "", "#include <algorithm>", "#include <initializer_list>", "#include <memory>", "#include <string_view>", "#include <type_traits>", "#include <utility>", "#include <vector>", "",
                *[f'#include "{h}"' for h in sorted(headers)], '#include "kern/logic/engine_direct.h"', '#include "kern/logic/nodes.h"', "",
                "namespace kern::logic {", "",
                "// The engine's enums and structs a pin names, between the type a Blueprint's class holds and the engine's",
                "// (engine_values::Direct): an enum's values in the order of the Blueprint's enum; a struct field by field -",
                "// the Blueprint's struct has the engine's fields in their order.",
                "namespace engine_direct {", "namespace ev = engine_values;", "", *self.direct_types_text(need), "",
                "// The places of the engine (engine_values::Place): the object's class, what it holds there, how it is reached.",
                *[x for p in places for x in p], "",
                "// The interfaces of the engine a Blueprint's class can be the object of (kern::logic::implementation).",
                *adapters, "}  // namespace engine_direct", "",
                "template <class Core> struct EngineNodesOf : Core {",
                "  using Bool = bool;", "  using Int = logic::Whole;", "  using Float = f64;", "  using Vector = Vec3;", "  using Text = logic::Text;", "  using Int64 = logic::Int64;", "  using Actor = logic::ActorHandle;",
                "  template <class T> using List = logic::List<T>;", "",
                "  /// One call of the engine through the host's door: the inputs as values to the host, what it gives as values back (zeros where it has no such function).",
                "  std::vector<Value> kern_engine(std::string_view function, const std::vector<Value>& in, std::initializer_list<PinType> outputs) const {",
                "    std::vector<Value> out;", "    for (PinType t : outputs) out.push_back(Value::zero(t));",
                "    if (this->kern_world.host) this->kern_world.host->engine(function, in, out);", "    return out;", "  }",
                "  /// A call of the engine the class made itself failed (EngineObjects::error says why): the host is told, as its door tells it.",
                "  void kern_engine_failed(std::string_view function) const {", "    if (this->kern_world.host) this->kern_world.host->engine_failed(function);", "  }", "",
                *bodies, "};", "", "}  // namespace kern::logic", ""]
        return "\n".join(text)

    def direct_types_text(self, need):
        u, lines = self.u, self.enums_text(need)
        names = [bt for kind, bt in sorted(need) if kind == "struct"]
        lines += [f"struct S_{bt};" for bt in names]
        for bt in names:
            q, fields = self.struct_q[bt], u.struct_fields[bt]
            tags = [self.tag(ft, set()) for _, ft, _, _ in fields]
            bound = ", ".join(f"f{k}" for k in range(len(fields)))
            parts = lambda way, order: " ".join(f"ev::Direct<{tg}>::{way}(o, {order(f'f{k}', f'x.{c}')});" for k, ((_, _, _, c), tg) in enumerate(zip(fields, tags)))
            lines += [f"struct S_{bt} {{",
                      f"  template <class B> static void from(EngineObjects& o, const B& b, {q}& x) {{ " + (f"const auto& [{bound}] = b; " if fields else "") + parts("from", lambda b, x: f"{b}, {x}") + " }",
                      f"  template <class B> static B make(EngineObjects& o, const {q}& x) {{ B b{{" + ", ".join(f"ev::made<{tg}>(o, x.{c})" for (_, _, _, c), tg in zip(fields, tags)) + "}; " +
                      (f"[[maybe_unused]] const auto& [{bound}] = b; " if fields else "") + "return b; }",
                      f"  template <class C> static void pass(EngineObjects& o, const C& x) {{ " + " ".join(f"ev::Direct<{tg}>::pass(o, x.{c});" for (_, _, _, c), tg in zip(fields, tags)) + " }",
                      # (a place of the engine that is such a struct, or an entry of one: its fields by their names - ev::Field)
                      "  template <class D, class C, class B> struct Fields {",
                      *[f"    ev::Field<D, {tg}, C, B, decltype({q}::{c}), decltype(B::{n}), &{q}::{c}, &B::{n}> {n};" for (n, _, _, c), tg in zip(fields, tags)],
                      "    explicit Fields(D* kern_d)" + (" : " + ", ".join(f"{n}(kern_d)" for n, _, _, _ in fields) if fields else "") + " { (void)kern_d; }",
                      "  };",
                      "};"]
        return lines

    def file_of(self, f):
        d = f["decl"]
        if f.get("role") in ("make", "release"):
            return self.tree.records[f["class"]].get("_file", "")
        return d.get("_file", "")

    def default_makers(self):
        """How a host makes an object of the engine for a handle it has none of (EngineObjects::on_missing; a test
        makes one there, the same on every side it compares): class -> (the constructor's node, its arguments) -
        each argument ("value", C++ type): its zero, as C++ makes it without arguments (ev::blank); ("null",): a
        pointer to an object, its zero; ("object", class): an object of the engine the constructor takes by
        reference, made the same way with it (ev::MadeWith - it has no handle). A constructor whose further
        parameters all have defaults stops there. Of the constructors the first that can be made this way, one
        whose parameters all have defaults first. A reference to a class the engine has no constructor of
        (an abstract Transport&) is made as the one class deriving from it that can be made (none or several:
        not made). Also self.unmade: class -> why there is no such way (for --list)."""
        makes = collections.defaultdict(list)
        for f in self.functions:
            if f.get("role") == "make":
                makes[f["class"]].append(f)
        plans, unmade = {}, {}

        def plan(q, stack):
            if q in plans:
                return plans[q]
            if q in stack:
                return None
            if not makes.get(q):
                unmade.setdefault(q, "no constructor of it is a node")
                return None
            fs = sorted(makes[q], key=lambda f: not all(c2b.kids(p) for p in c2b.kids(f["decl"]) if p.get("kind") == "ParmVarDecl"))
            why = []
            for f in fs:
                parms = [p for p in c2b.kids(f["decl"]) if p.get("kind") == "ParmVarDecl"]
                args, ok = [], True
                for p, P, (name, bt, h) in zip(parms, self.ptypes(f), f["ins"]):
                    if c2b.kids(p):
                        break   # (it and every one after it have defaults)
                    if h is None:
                        args.append(("value", P))
                    elif P.endswith("*"):
                        args.append(("null",))
                    else:
                        d = provider(h, stack | {q})
                        if d is None:
                            why.append(f"{name}: {unmade.get(h, 'nothing makes a ' + h)}")
                            ok = False
                            break
                        args.append(("object", d))
                if ok:
                    plans[q] = (f, args)
                    unmade.pop(q, None)
                    return plans[q]
            unmade[q] = "its constructors take what cannot be made: " + "; ".join(why)
            return None

        def provider(h, stack):
            if plan(h, stack) is not None:
                return h
            found = [c for c in sorted(makes) if c != h and h in self.bases(c) and plan(c, stack) is not None]
            if len(found) == 1:
                return found[0]
            unmade[h] = (f"several classes deriving from it can be made ({', '.join(found)})" if found else unmade.get(h, "no constructor of it is a node"))
            return None

        for q in sorted(makes):
            plan(q, frozenset())
        self.unmade = unmade
        return plans

    def maker_text(self, q, plans, headers):
        """The maker of class q for a handle a host has none of (see default_makers)."""
        f, args = plans[q]
        if not args and all(c2b.kids(p) for p in c2b.kids(f["decl"]) if p.get("kind") == "ParmVarDecl"):
            return f"{{EngineObjects::kind<{q}>(), [](EngineObjects& o, i64 handle) -> void* {{ return o.put_at<{self.kinds(q)}>(handle, std::unique_ptr<{q}>(new {q}())); }}}}"
        lines, n = [], [0]

        def spelled(c, args):
            out = []
            for a in args:
                if a[0] == "value":
                    out.append(f"ev::blank<std::remove_cvref_t<{a[1]}>>()")
                elif a[0] == "null":
                    out.append("nullptr")
                else:
                    d = a[1]
                    headers.add(re.sub(r"^.*/include/", "", self.tree.records[d].get("_file", "")))
                    inner = spelled(d, plans[d][1])
                    v = f"d{n[0]}"
                    n[0] += 1
                    lines.append(f"auto& {v} = made->keep(new {d}({', '.join(inner)}));")
                    out.append(v)
            return out
        top = spelled(q, args)
        if not lines:
            return (f"{{EngineObjects::kind<{q}>(), [](EngineObjects& o, i64 handle) -> void* {{ return o.put_at<{self.kinds(q)}>(handle, "
                    f"std::unique_ptr<{q}>(new {q}({', '.join(top)}))); }}}}")
        body = " ".join([f"auto made = std::make_unique<ev::MadeWith<{q}>>();", *lines, f"made->object.emplace({', '.join(top)});",
                         f"return o.put_made<{self.kinds(q)}>(handle, std::move(made));"])
        return f"{{EngineObjects::kind<{q}>(), [](EngineObjects& o, i64 handle) -> void* {{ {body} }}}}"

    def texts(self):
        plans = self.default_makers()
        by = collections.defaultdict(list)
        for f in self.functions:
            by[component_of(self.file_of(f))].append(f)
        out = {}
        for comp in sorted(by):
            fs = by[comp]
            need, bodies, makers, headers = set(), [], [], set()
            for k, f in enumerate(fs):
                path = self.file_of(f)
                headers.add(re.sub(r"^.*/include/", "", path))
                lines = self.body(f, need)
                bodies.append(f"// {f['type']}: {f['cpp']}")
                bodies.append(f"bool c{k}(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {{")
                bodies += ["  " + x for x in lines]
                bodies += ["}", ""]
                if f.get("place"):
                    # a place of the engine, for the interpreter and a class without the engine's objects: read through
                    # <node>@get (what the node gives: the place, as a value), changed through <node>@set (the value it
                    # has then, put in its place)
                    pin, _, q = f["ins"][0]
                    tg = self.tag(f['outs'][0][1], need)
                    if f.get("field"):
                        set_lines = [f"auto* self = ev::object<{q}>(o, in, 0, {c_text(pin)});", "if (!self) return false;",
                                     f"ev::take<{tg}>(o, in, 1, self->{f['decl']['name']});"]
                    else:
                        # (the call as the node makes it - its inputs, the index checked -, and the value after them put there)
                        got = lines[:next(i for i, x in enumerate(lines) if x.startswith("decltype(auto) r = "))]
                        call = next(x for x in lines if x.startswith("decltype(auto) r = "))[len("decltype(auto) r = "):-1]
                        set_lines = got + [f"ev::take<{tg}>(o, in, {len(f['ins'])}, {call});"]
                    bodies += [f"// {f['type']}@set: what stands at the place {f['cpp']} gives",
                               f"bool c{k}_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {{",
                               *["  " + x for x in set_lines], "  return true;", "}", ""]
                if f.get("role") == "make" and f["class"] in plans and plans[f["class"]][0] is f:
                    # (a host without an object at a handle - a test - makes one there: see default_makers)
                    makers.append((f["class"], self.maker_text(f["class"], plans, headers)))
            adapters = []   # (the interfaces of this component a Blueprint can be the object of)
            for q, name, virtuals, waits in self.interfaces():
                if not waits and component_of(self.file_of(virtuals[0][0])) == comp:
                    bodies += self.adapter_text(q, name, virtuals, need)
                    adapters.append(name)
            text = ["// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare",
                    "// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).",
                    "#include <algorithm>", *[f'#include "{h}"' for h in sorted(headers)], '#include "kern/logic/engine_host.h"', "",
                    "namespace kern::logic {", "namespace {", "", "namespace ev = engine_values;", "",
                    *self.types_text(need), "", *bodies,
                    "const EngineCall kCalls[] = {", *[f'    {{"{f["type"]}", &c{k}}},' for k, f in enumerate(fs)],
                    *[f'    {{"{f["type"]}@get", &c{k}}},\n    {{"{f["type"]}@set", &c{k}_set}},' for k, f in enumerate(fs) if f.get("place")], "};",
                    "const EngineCalls kAll(kCalls, std::size(kCalls));"]
            if makers:
                text += ["", "const EngineMaker kMakers[] = {", *[f"    {m}," for _, m in makers], "};", "const EngineMakers kAllMakers(kMakers, std::size(kMakers));"]
            if adapters:
                text += ["", "const EngineImplement kAdapters[] = {", *[f'    {{"{a}", &implements_{a}}},' for a in adapters], "};",
                         "const EngineImplements kAllAdapters(kAdapters, std::size(kAdapters));"]
            text += ["", "}  // namespace", "}  // namespace kern::logic", ""]
            out[f"src/{comp}/engine_calls.gen.cpp"] = "\n".join(text)
        return out


def read_engine(flags=()):
    """The compiler's tree of the engine's headers, and the Blueprint types they are made of."""
    work = os.path.join(tempfile.gettempdir(), "kern-engine-api")
    os.makedirs(work, exist_ok=True)
    path = os.path.join(work, "engine_headers.cpp")
    text = "".join(f'#include "kern/{h}"\n' for h in HEADERS)
    if not os.path.exists(path) or open(path).read() != text:
        open(path, "w").write(text)
    tree = c2b.Tree(path, "kern_engine_api_none", [f"-I{KERN}/include", f"-I{KERN}/third_party/box3d/include", *flags])
    return tree, c2b.Unit(tree, "Engine")


# The headers whose types and functions are the engine's libraries (kern/logic/engine_libraries.h): each one library,
# named by its path (kern/physics/physics.h: engine_physics_physics).
LIBRARY_HEADERS = ["app/app.h", "audio/audio.h", "audio/synth.h", "core/base.h", "game/level.h", "game/runtime.h", "geo/buildings.h", "geo/mesh_data.h", "geo/roads.h",
                   "gfx/mesh.h", "gfx/passes.h", "gfx/renderer.h", "gfx/shader.h", "logic/logic.h", "math/math.h", "net/client_server.h",
                   "net/connection.h", "net/reliable.h", "net/transport.h", "physics/physics.h"]


def libraries_text(level):
    """The engine's libraries as the build of the engine has them: each library's JSON - with what it holds of the
    engine's libraries it uses, as every Blueprint holds what it uses (they are written together, from the same
    headers) -, a library after those it uses, with its mark (the hash of its text: what a level that uses it records)."""
    import hashlib, json
    graphs = {a["name"]: a["components"]["logic"] for a in level["actors"] if a.get("components", {}).get("logic")}
    bad = [n for n in graphs if not n.startswith("engine_")]
    if bad:
        raise SystemExit(f"engine_api.py --libraries: the engine's libraries would use {', '.join(sorted(bad))} - a library of the engine uses no library of a level")
    order, seen = [], set()
    def visit(n, open_=()):
        if n in seen:
            return
        if n in open_:
            raise SystemExit(f"engine_api.py --libraries: the engine's libraries {', '.join(open_)} use each other")
        for u in graphs[n].get("uses", []):
            if u not in graphs:
                raise SystemExit(f"engine_api.py --libraries: {n} uses {u}, which is no library of the engine")
            visit(u, (*open_, n))
        seen.add(n)
        order.append(n)
    for n in sorted(graphs):
        visit(n)
    lines = ["// The engine's own libraries: the types and functions of its headers as Blueprints (written by tools/engine_api.py",
             "// --libraries from the engine's headers - see kern/logic/engine_libraries.h; not edited by hand).",
             '#include "kern/logic/engine_libraries.h"', "", "namespace kern::logic {", "",
             "const std::vector<EngineLibraryText>& engine_library_texts() {", "  static const std::vector<EngineLibraryText> all = {"]
    for n in order:
        text = json.dumps(graphs[n], separators=(",", ":"), ensure_ascii=False, sort_keys=True)
        assert ')kl"' not in text
        mark = hashlib.sha256(text.encode()).hexdigest()[:16]
        lines.append(f'      {{"{n}", "{mark}", R"kl({text})kl"}},')
    lines += ["  };", "  return all;", "}", "", "}  // namespace kern::logic", ""]
    return "\n".join(lines)


def write_libraries():
    """Converts the engine's headers into its libraries (tools/cpp_to_blueprint.py --engine) and writes them."""
    import json, shutil
    work = tempfile.mkdtemp(prefix="kern-engine-libraries-")
    try:
        driver = os.path.join(work, "program", "engine_libraries.cpp")
        os.makedirs(os.path.dirname(driver))
        open(driver, "w").write("".join(f'#include "kern/{h}"\n' for h in LIBRARY_HEADERS))
        level = os.path.join(work, "engine.klevel")
        json.dump({"format": "kern-level", "version": 1, "name": "engine", "world": {}, "nextId": 1, "actors": []}, open(level, "w"))
        cmd = [sys.executable, os.path.join(HERE, "cpp_to_blueprint.py"), "--convert", "kern_engine_libraries", "--level", level, "--folder", "Engine"]
        for h in LIBRARY_HEADERS:
            cmd += ["--engine", os.path.join(KERN, "include", "kern", h)]
        cmd += [f"-I{KERN}/include", f"-I{KERN}/third_party/box3d/include", "--", driver]
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0 or "were not read" in r.stdout + r.stderr:
            raise SystemExit("engine_api.py --libraries: the engine's headers do not convert:\n" + (r.stdout + r.stderr)[-4000:])
        text = libraries_text(json.load(open(level)))
    finally:
        shutil.rmtree(work, ignore_errors=True)
    path = os.path.join(KERN, "src", "logic", "engine_libraries.gen.cpp")
    if not os.path.exists(path) or open(path).read() != text:
        open(path, "w").write(text)
    print(f"the engine's libraries: {path}")


def built_libraries():
    """The engine's libraries as engine_libraries.gen.cpp has them: name -> their JSON."""
    import json
    text = open(os.path.join(KERN, "src", "logic", "engine_libraries.gen.cpp"), encoding="utf-8").read()
    return {m.group(1): json.loads(m.group(2)) for m in re.finditer(r'\{"(engine_\w+)", "\w*", R"kl\((.*?)\)kl"\}', text, re.S)}


def check_level(path):
    """What a level - or a directory of Blueprint texts (tools/cpp_to_blueprint.py --texts) - names of the engine's
    libraries that they do not have: each type and function, with who names it. 0: all there."""
    import json
    have = {}
    for name, g in built_libraries().items():
        have[name] = {"types": {x["name"] for k in ("structs", "enums") for x in g.get(k, []) if not x.get("from")},
                      "functions": {x["name"] for x in g.get("functions", []) if not x.get("from")}}
    named = collections.defaultdict(set)   # (library, kind, name) -> who names it
    if os.path.isdir(path):
        for f in sorted(os.listdir(path)):
            if not f.endswith(".cpp") or f.startswith("engine_"):
                continue
            text = open(os.path.join(path, f), encoding="utf-8").read()
            who = f[:-4]
            for lib, fn in re.findall(r"\b(engine_\w+?)::(\w+)", text):
                if lib in have and fn not in have[lib]["types"] and not fn.endswith("Result"):
                    named[(lib, "function", fn)].add(who)
            for lib in re.findall(r"virtual (engine_\w+)\b", text):
                if lib not in have:
                    named[(lib, "library", lib)].add(who)
    else:
        for a in json.load(open(path, encoding="utf-8")).get("actors", []):
            g = (a.get("components") or {}).get("logic") or {}
            if a["name"].startswith("engine_"):
                continue
            for lib in g.get("uses", []):
                if lib.startswith("engine_") and lib not in have:
                    named[(lib, "library", lib)].add(a["name"])
            for k, kind in (("structs", "type"), ("enums", "type"), ("functions", "function")):
                for x in g.get(k, []):
                    lib = x.get("from", "")
                    if lib.startswith("engine_"):
                        n = x["name"].split("::")[-1]
                        named[(lib, kind, n)].add(a["name"])
    missing = []
    for (lib, kind, n), who in sorted(named.items()):
        if lib not in have:
            missing.append(f"{lib}: no library of the engine (named by {', '.join(sorted(who)[:5])})")
        elif kind != "library" and n not in have[lib]["types" if kind == "type" else "functions"]:
            missing.append(f"{lib}: the {kind} {n} - the engine does not write it (named by {', '.join(sorted(who)[:5])}{' ...' if len(who) > 5 else ''})")
    for m in missing:
        print(m)
    print(f"{len(missing)} of {len(named)} things of the engine's libraries that {path} names are not in them"
          + ("" if not missing else " (tools/engine_api.py --libraries; a type a game needs is marked KERN_BLUEPRINT_TYPE)"))
    return 1 if missing else 0


def stale_outputs(kern, wanted):
    """The files an earlier run wrote that this one does not (the calls of a component that has no node of the engine
    any more: src/<component>/engine_calls.gen.cpp): what this tool writes is exactly the files of its run, so these
    go. `wanted`: the paths of this run, relative to `kern`."""
    import glob
    found = glob.glob(os.path.join(kern, "src", "*", "engine_calls.gen.cpp"))
    return sorted(p for p in found if os.path.relpath(p, kern).replace(os.sep, "/") not in set(wanted))


def selftest():
    """The tool's own test (python3 tools/engine_api.py --selftest): a component's calls that are no longer written are
    stale; those written now and other files are not."""
    import tempfile
    with tempfile.TemporaryDirectory() as k:
        for comp in ("game", "logic", "app"):
            os.makedirs(os.path.join(k, "src", comp))
            open(os.path.join(k, "src", comp, "engine_calls.gen.cpp"), "w").write("//")
        open(os.path.join(k, "src", "logic", "logic.cpp"), "w").write("//")
        got = [os.path.relpath(p, k) for p in stale_outputs(k, ["src/game/engine_calls.gen.cpp", "src/app/engine_calls.gen.cpp"])]
        assert got == [os.path.join("src", "logic", "engine_calls.gen.cpp")], got
        assert stale_outputs(k, [f"src/{c}/engine_calls.gen.cpp" for c in ("game", "logic", "app")]) == []
    print("engine_api.py: selftest ok")
    return 0


def main(argv):
    if "--selftest" in argv:
        return selftest()
    if "--check" in argv:   # (what a level names of the engine's libraries that they do not have - no clang, no run)
        return check_level(argv[argv.index("--check") + 1])
    if "--libraries" in argv:   # (the engine's libraries: a run of the converter over the headers - minutes, not seconds)
        write_libraries()
        return 0
    tree, unit = read_engine()
    api = Api(tree, unit)
    functions, waiting = [], []
    for d in sorted((x for x in tree.decl.values() if api.declared_only(x)), key=lambda x: (x.get("_file", ""), x.get("_at", (0,))[0] or 0)):
        try:
            functions.append(api.function(d))
        except Wait as w:
            waiting.append((api.home(d) + "::" + d["name"], str(w)))
    # the objects of the engine a Blueprint makes and ends: their constructors and their destructor
    for q in sorted(q for q in tree.records if q.startswith("kern::")):
        made = 0
        for d in api.makers(q):
            try:
                functions.append(api.maker(d))
                made += 1
            except Wait as w:
                waiting.append((q + "::" + q.split("::")[-1], str(w)))
        if made:
            functions.append(api.releaser(q))
        for f, c in api.fields(q):   # (its fields: places of the engine - Api.fields)
            if isinstance(f, Wait):
                waiting.append((q + "::" + c["name"], str(f)))
            else:
                functions.append(f)
    if api.unsaid:   # (a whole number beside a pointer must say in its type that it is its count: no length is guessed)
        raise SystemExit("engine_api.py: a whole number beside a pointer that does not say in its type that it counts what the pointer shows - "
                         "declare it kern::Count<Per> (Per numbers each):\n  " + "\n  ".join(sorted(set(api.unsaid))))
    if api.false_places:   # (KERN_PLACE says it is a place of the engine: one that cannot be is no copy in its stead)
        raise SystemExit("engine_api.py: a declaration says KERN_PLACE but is no place of the engine (kern/docs/blueprint_cpp.md, \"Plätze der Engine\"):\n  "
                         + "\n  ".join(sorted(set(api.false_places))))
    types = [f["type"] for f in functions]
    assert len(set(types)) == len(types), [t for t in types if types.count(t) > 1]
    if "--list" in argv:
        for f in functions:
            print(("value " if f["value"] else "place " if f.get("place") else "step  ") + f["type"] + "(" + ", ".join(f"{bt} {n}" for n, bt, _ in f["ins"]) + ")" + (" -> " + ", ".join(f"{bt} {n}" for n, bt, _ in f["outs"]) if f["outs"] else ""))
        print()
        for name, why in waiting:
            print(f"no node: {name}: {why}")
    calls = Calls(api, functions)
    if "--list" in argv:   # (the objects of the engine a host cannot make for a handle it has none of - and why)
        calls.default_makers()
        for q, why in sorted(calls.unmade.items()):
            print(f"not made for a handle: {q}: {why}")
    # (every class of the engine a node takes or gives an object of, and every interface - with their bases)
    held = {h for f in functions for _, _, h in [*f["ins"], *f["outs"]] if h} | {q for q, _, _, _ in calls.interfaces()}
    held = sorted(held | {b for q in held for b in calls.bases(q)})
    classes = [(q, calls.bases(q)) for q in held]
    outputs = (("src/logic/engine_api.gen.cpp", table_text(api, functions, calls.interfaces(), classes)), ("include/kern/logic/engine_nodes.gen.h", calls.class_text()),
               ("tests/engine_case.gen.h", case_text(api, functions)), *calls.texts().items())
    differ = []   # (--check: what a run would change - written anew or removed)
    for rel, text in outputs:
        path = os.path.join(KERN, rel)
        if not os.path.exists(path) or open(path).read() != text:
            differ.append(rel)
            if "--list" not in argv and "--check" not in argv:
                open(path, "w").write(text)
    for path in stale_outputs(KERN, [rel for rel, _ in outputs]):
        differ.append(os.path.relpath(path, KERN) + " (removed)")
        if "--list" not in argv and "--check" not in argv:
            os.remove(path)
            print(f"removed {os.path.relpath(path, KERN)}: its component has no node of the engine any more")
    if "--check" in argv and differ:
        print("engine_api.py --check: the generated files are not those of the headers:\n  " + "\n  ".join(differ))
        return 1
    print(f"{len(functions)} functions of the engine are nodes; {len(waiting)} have types without a place in the language")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
