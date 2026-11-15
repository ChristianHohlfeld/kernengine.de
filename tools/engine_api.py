#!/usr/bin/env python3
"""The engine as nodes: what the engine's headers declare, as nodes of the Blueprint language.

  tools/engine_api.py [--list]

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

Every tier calls them through one door, logic::Host::engine(function, in, out): the game binds the
engine there, a test a stand-in.

This tool reads the headers with the compiler's own parser (as tools/cpp_to_blueprint.py does - it
shares its types) and writes
  kern/src/logic/engine_api.gen.cpp           the table of the functions and their pins
  kern/include/kern/logic/engine_nodes.gen.h  the functions of a Blueprint's class
  kern/tests/engine_case.gen.h                the test's Blueprint that calls every one of them
  kern/src/<component>/engine_calls.gen.cpp   the call of every node on the real function (kern/logic/engine_host.h):
                                              its inputs made into what the C++ function takes, what it gives and
                                              writes made into its outputs - for the headers of that component
A function whose types have no place in the language (a callback, a raw pointer) gets no node;
--list names them with what they wait for.
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
HEADERS = ["core/base.h", "core/jobs.h", "app/app.h", "audio/audio.h", "audio/synth.h", "physics/physics.h", "gfx/mesh.h", "gfx/renderer.h",
           "geo/mesh_data.h", "geo/primitives.h", "geo/earcut.h", "geo/buildings.h", "geo/roads.h", "game/level.h", "game/sound.h", "game/runtime.h",
           "net/client_server.h"]
Api, snake = c2b.Api, c2b.snake   # (the converter's own: a call in a game's source and its node have the same pins)


def c_text(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"').replace("\n", " ") + '"'


def table_text(api, functions):
    lines = ["// The engine's functions as nodes: the table (written by tools/engine_api.py from the engine's headers -",
             "// see kern/logic/engine_api.h; not edited by hand).",
             '#include "kern/logic/engine_api.h"', "", "namespace kern::logic {", "",
             "const std::vector<EngineFunction>& engine_functions() {", "  static const std::vector<EngineFunction> all = {"]
    for f in functions:
        def pins(ps):
            out = []
            for name, bt, _ in ps:
                t, of = api.pin(bt)
                count = f.get("counts", {}).get(name) if ps is f["outs"] else None   # (an output list the engine writes through a pointer: its count)
                per = f.get("per", {}).get(name, 1) if count else 1                  # (... and how many numbers each it counts is)
                out.append(f"{{{c_text(name)}, PinType::{t}, {c_text(of)}, {c_text(api.shape(bt))}" + (f", {c_text(count)}" if count else "") + (f", {per}" if per != 1 else "") + "}")
            return "{" + ", ".join(out) + "}"
        lines.append(f"      {{{c_text(f['type'])}, {c_text(f['cpp'])}, {c_text(f['label'])}, {c_text(f['category'])}, {c_text(f['help'])}, {'true' if f['value'] else 'false'},")
        lines.append(f"       {pins(f['ins'])},")
        lines.append(f"       {pins(f['outs'])}}},")
    lines += ["  };", "  return all;", "}", "", "}  // namespace kern::logic", ""]
    return "\n".join(lines)


def class_text(api, functions):
    """The functions of a Blueprint's class: each hands its inputs to the host and takes what it gives."""
    plain = {"Bool": "Bool", "Int": "Int", "Float": "Float", "Vector": "Vector", "Text": "const Text&", "Int64": "Int64"}
    lines = ["// The engine's functions as a Blueprint's class calls them (written by tools/engine_api.py from the",
             "// engine's headers - see kern/logic/engine_api.h; not edited by hand). Included by kern/logic/blueprint.h.",
             "#pragma once", "", "#include <initializer_list>", "#include <string_view>", "#include <type_traits>", "#include <vector>", "", '#include "kern/logic/nodes.h"', "",
             "namespace kern::logic {", "",
             "template <class Core> struct EngineNodesOf : Core {",
             "  using Bool = bool;", "  using Int = logic::Whole;", "  using Float = f64;", "  using Vector = Vec3;", "  using Text = logic::Text;", "  using Int64 = logic::Int64;",
             "  template <class T> using List = logic::List<T>;", "",
             "  /// One call of the engine: the inputs as values to the host, what it gives as values back (zeros where it has no such function).",
             "  std::vector<Value> kern_engine(std::string_view function, const std::vector<Value>& in, std::initializer_list<PinType> outputs) const {",
             "    std::vector<Value> out;", "    for (PinType t : outputs) out.push_back(Value::zero(t));",
             "    if (this->kern_world.host) this->kern_world.host->engine(function, in, out);", "    return out;", "  }", ""]
    for f in functions:
        name = f["type"].replace(".", "_")
        templates, params, values = [], [], []
        for name_, bt, _ in f["ins"]:
            if bt in plain:
                params.append(f"{plain[bt]} {name_}")
            elif bt.startswith("List<") and bt[5:-1] in plain:
                params.append(f"const List<{bt[5:-1]}>& {name_}")
            else:
                templates.append(f"class T{len(templates) + 1}")
                params.append(f"const T{len(templates)}& {name_}")
            values.append(f"to_value({name_})")
        zeros = ", ".join(f"PinType::{api.pin(bt)[0]}" for _, bt, _ in f["outs"])
        call = f"this->kern_engine({c_text(f['type'])}, {{{', '.join(values)}}}, {{{zeros}}})"

        def typed(bt):
            if bt in plain:
                return bt
            if bt.startswith("List<") and bt[5:-1] in plain:
                return bt
            return None
        if f["help"]:
            lines.append(f"  /// {f['help']}")
        if f["value"]:
            bt = f["outs"][0][1]
            head = (f"  template <{', '.join(templates)}> " if templates else "  ")
            if typed(bt):
                lines.append(f"{head}{typed(bt)} {name}({', '.join(params)}) const {{ return from_value<{typed(bt)}>({call}[0].converted(pin_type_of<{typed(bt)}>())); }}")
            else:
                lines.append(f"{head}EngineValue {name}({', '.join(params)}) const {{ return {{{call}[0]}}; }}")
            continue
        # a step: what it gives goes to the node's member (n.result ...), when the text reads it
        head = f"  template <{', '.join(templates)}> " if templates else "  "
        lines.append(f"{head}void {name}({', '.join(params)}) {{ {call}; }}")
        if f["outs"]:
            takes = " ".join(f"kern_n.{o} = from_value<std::remove_cvref_t<decltype(kern_n.{o})>>(kern_out[{i}]);" for i, (o, _, _) in enumerate(f["outs"]))
            lines.append(f"  template <{', '.join(['class KernNode'] + templates)}> void {name}({', '.join(['KernNode& kern_n'] + params)}) {{ const std::vector<Value> kern_out = {call}; {takes} }}")
    lines += ["};", "", "}  // namespace kern::logic", ""]
    return "\n".join(lines)


def case_text(api, functions):
    """The test's Blueprint: every function of the engine called once, what it gives kept in a variable."""
    u = api.u
    plain = {"Bool": "false", "Int": "0", "Float": "0.0", "Vector": "Vector{0.0, 0.0, 0.0}", "Text": '""', "Int64": "Int64{0x0}"}
    zero = lambda bt: plain[bt] if bt in plain else u.zero(bt)
    variables, members, bodies, inputs = [], [], [], {}

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
        else:
            node += 1
            members.append("  struct { " + " ".join(f"{bt} {o} = {zero(bt)};" for o, bt, _ in f["outs"]) + f" }} n{node};")
            lines.append(f"{name}({', '.join([f'n{node}'] + args)});")
            for j, (o, bt, _) in enumerate(f["outs"]):
                variables.append(f"  {bt} R{k}_{j} = {zero(bt)};")
                lines.append(f"R{k}_{j} = n{node}.{o};")
        bodies.append(lines)
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
    runs, per = [], 40
    for a in range(0, len(bodies), per):
        runs.append(f"Run{a // per + 1}")
        out += [f"  void {runs[-1]}() {{", *["    " + ln for body in bodies[a:a + per] for ln in body], "  }", ""]
    out += [*members, "};"]
    text = "\n".join(out) + "\n"
    return ("// The engine's nodes in a test: a Blueprint that calls every function of the engine once and keeps what it\n"
            "// gives (written by tools/engine_api.py - see kern/logic/engine_api.h; not edited by hand).\n#pragma once\n\n"
            "namespace engine_case {\n\ninline const char* kEngine = R\"kn(\n" + text + ")kn\";\n\n"
            "inline const char* const kRuns[] = {" + ", ".join(c_text(r) for r in runs) + "};\n\n}  // namespace engine_case\n")


# ---- the calls: every node on the real function (kern/logic/engine_host.h) ----
SCALAR_TAG = {"Bool": "Bool", "Int": "Int", "Float": "Float", "Vector": "Vector", "Text": "Text", "Int64": "Int64"}


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

    # ---- the Blueprint type of a pin as a tag (engine_values) ----
    def tag(self, bt, need):
        u = self.u
        if bt in SCALAR_TAG:
            return "ev::" + SCALAR_TAG[bt]
        if bt.startswith("List<"):
            return f"ev::List<{self.tag(bt[5:-1], need)}>"
        if bt in u.enum_values:
            need.add(("enum", bt))
            return f"ev::Enum<E_{bt}>"
        if bt in u.rows:
            return f"ev::Row<{self.tag(u.rows[bt], need)}>"
        q = self.struct_q.get(bt)
        if isinstance(q, tuple) and q[0] == "pair":
            return f"ev::Pair<{self.tag(q[1], need)}, {self.tag(q[2], need)}>"
        if isinstance(q, str):
            if ("struct", bt) not in need:
                need.add(("struct", bt))
                for _, ft, _, _ in u.struct_fields[bt]:
                    self.tag(ft, need)
            return f"ev::Struct<S_{bt}>"
        raise SystemExit(f"engine_api.py: no tag for the Blueprint type {bt}")

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

    def types_text(self, need):
        u, lines, defs = self.u, [], []
        for kind, bt in sorted(need):
            if kind == "enum":
                q = self.enum_q[bt]
                seen, values = set(), []
                for n, x in c2b.enum_numbers(self.tree.enums[q]):
                    if x not in seen:
                        seen.add(x)
                        values.append(f"{q}::{n}")
                lines.append(f"struct E_{bt} {{ using type = {q}; static constexpr type values[] = {{{', '.join(values)}}}; }};")
        for kind, bt in sorted(need):
            if kind == "struct":
                q = self.struct_q[bt]
                lines.append(f"struct S_{bt} {{ static Value to(EngineObjects& o, const {q}& x); static void from(EngineObjects& o, const Value& v, {q}& x); }};")
                fields = u.struct_fields[bt]
                tags = [self.tag(ft, set()) for _, ft, _, _ in fields]
                to = ", ".join(f"ev::Conv<{tg}>::to(o, x.{c})" for (_, _, _, c), tg in zip(fields, tags))
                defs.append(f"Value S_{bt}::to(EngineObjects& o, const {q}& x) {{ return Value::structure({{{to}}}); }}")
                body = " ".join(f"if (const Value* f = v.field({k})) ev::Conv<{tg}>::from(o, *f, x.{c});" for k, ((_, _, _, c), tg) in enumerate(zip(fields, tags)))
                defs.append(f"void S_{bt}::from(EngineObjects& o, const Value& v, {q}& x) {{ {body} }}")
        return lines + defs

    def body(self, f, need):
        """The lines of the call of a node: its inputs made into the arguments, the call, its outputs."""
        d, role, tree = f["decl"], f.get("role"), self.tree
        if role == "release":
            q = f["class"]
            return [f"if (!o.release<{q}>(in.empty() ? 0 : in[0].as_int())) {{",
                    f"  o.error = \"the handle names no {q.split('::')[-1]} that was made\";", "  return false;", "}", "return true;"]
        mangled = d.get("mangledName")
        ptypes = parameters_of(self.names[mangled]) if mangled else []
        params = f["params"]
        assert len(ptypes) == len(params), (f["type"], ptypes, params)
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
                args.append(f"{a}.c_str()")
            elif re.match(r"std::basic_string_view<char\b", P):
                lines += [f"std::string {a};", f"ev::take<{tg}>(o, in, {k}, {a});"]
                args.append(a)
            elif pointer:
                x = re.sub(r"\s*\bconst$", "", P[:-1].strip())
                lines += [f"std::vector<{x}> {a};", f"ev::take<{tg}>(o, in, {k}, {a});"]
                args.append(f"{a}.empty() ? nullptr : {a}.data()")
                nxt = params[i + 1][0] if i + 1 < len(params) else None
                if nxt is not None and re.search(r"count|size|len|num|^n$", nxt) and pin.get(nxt, ("", None))[0] == "Int":
                    # (the first of as many as the count beside it says: no more than the list has - the engine reads them)
                    after_take = f'if (ev::room(a{i + 1}) > {a}.size()) {{ o.error = "the list {pin_in} has fewer entries than {nxt} says"; return false; }}'
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
        if role == "make":
            q = f["class"]
            made = f"new {q}({', '.join(args)})" if mangled else f"new {q}()"
            lines += [f"out[0] = Value::integer(o.own<{self.kinds(q)}>(std::unique_ptr<{q}>({made})));", "return true;"]
            return lines
        call = f"{obj}{d['name']}({', '.join(args)})" if method else f"{f['cpp']}({', '.join(args)})"
        ret = c2b.qtype(d).split("(")[0].strip()
        if ret == "void":
            lines.append(call + ";")
        else:
            lines.append(f"decltype(auto) r = {call};")
            bt, h = outs["result"]
            h = h or (self.u.object_of(ret) if bt == "Int" else None)   # (one it gives owned - std::unique_ptr<Transport>: its handle)
            if h:
                after.insert(0, f"out[0] = Value::integer(ev::handle_of<{self.kinds(h)}>(o, std::forward<decltype(r)>(r)));")
            else:
                after.insert(0, f"ev::give<{self.tag(bt, need)}>(o, out, 0, r);")
        return lines + after + ["return true;"]

    def file_of(self, f):
        d = f["decl"]
        if f.get("role") in ("make", "release"):
            return self.tree.records[f["class"]].get("_file", "")
        return d.get("_file", "")

    def texts(self):
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
                if f.get("role") == "make" and all(c2b.kids(p) for p in c2b.kids(f["decl"]) if p.get("kind") == "ParmVarDecl"):
                    q = f["class"]
                    if not any(m[0] == q for m in makers):
                        makers.append((q, f"{{EngineObjects::kind<{q}>(), [](EngineObjects& o, i64 handle) -> void* {{ return o.put_at<{self.kinds(q)}>(handle, std::unique_ptr<{q}>(new {q}())); }}}}"))
            text = ["// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare",
                    "// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).",
                    *[f'#include "{h}"' for h in sorted(headers)], '#include "kern/logic/engine_host.h"', "",
                    "namespace kern::logic {", "namespace {", "", "namespace ev = engine_values;", "",
                    *self.types_text(need), "", *bodies,
                    "const EngineCall kCalls[] = {", *[f'    {{"{f["type"]}", &c{k}}},' for k, f in enumerate(fs)], "};",
                    "const EngineCalls kAll(kCalls, std::size(kCalls));"]
            if makers:
                text += ["", "const EngineMaker kMakers[] = {", *[f"    {m}," for _, m in makers], "};", "const EngineMakers kAllMakers(kMakers, std::size(kMakers));"]
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


def main(argv):
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
    types = [f["type"] for f in functions]
    assert len(set(types)) == len(types), [t for t in types if types.count(t) > 1]
    if "--list" in argv:
        for f in functions:
            print(("value " if f["value"] else "step  ") + f["type"] + "(" + ", ".join(f"{bt} {n}" for n, bt, _ in f["ins"]) + ")" + (" -> " + ", ".join(f"{bt} {n}" for n, bt, _ in f["outs"]) if f["outs"] else ""))
        print()
        for name, why in waiting:
            print(f"no node: {name}: {why}")
    for rel, text in (("src/logic/engine_api.gen.cpp", table_text(api, functions)), ("include/kern/logic/engine_nodes.gen.h", class_text(api, functions)),
                      ("tests/engine_case.gen.h", case_text(api, functions)), *Calls(api, functions).texts().items()):
        path = os.path.join(KERN, rel)
        if "--list" not in argv and (not os.path.exists(path) or open(path).read() != text):
            open(path, "w").write(text)
    print(f"{len(functions)} functions of the engine are nodes; {len(waiting)} have types without a place in the language")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
