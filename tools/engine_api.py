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
A function whose types have no place in the language (a callback, a raw pointer) gets no node;
--list names them with what they wait for.
"""
import importlib.util
import os
import re
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
Api, snake = c2b.Api, c2b.snake
INT64 = {"Int64": ("int64", "l"), "List<Int64>": ("int64_list", "[l]")}   # (the pin type, the form of the value - see EngineStandIn)


class EngineApi(Api):
    """The engine's functions as nodes, with its 64-bit numbers as they are: a u64 or an i64 the engine
    takes or gives (a mask of bits, a key, a counter of 64 bits) is an Int64 pin - 64 bits, as C++ has
    them - and a list of them a List<Int64>. (What a struct of the engine holds in its fields is the
    struct as tools/cpp_to_blueprint.py writes it.)"""

    def wide(self, raw):
        """The Blueprint type of a parameter's or a result's type when it is 64 bits as C++ has them
        (u64, const i64&, std::vector<u64>&, Span<const u64>, const u64* with a count): Int64 or
        List<Int64> - else None."""
        r = re.sub(r"\s*(&&|&|\*\s*(const)?)$", "", raw.strip())
        r = re.sub(r"^const\s+", "", r).strip()
        c = self.u.clean(r)
        if c in c2b.W64:
            return "Int64"
        m = re.match(r"(?:std::vector|(?:kern::)?Span)<\s*(?:const\s+)?(.+?)\s*>$", c)
        if m and self.u.clean(m.group(1)) in c2b.W64:
            return "List<Int64>"
        return None

    def handed(self, raw):
        bt = self.wide(raw)
        if bt is None:
            return super().handed(raw)
        r = raw.strip()
        pointer = bool(re.search(r"\*\s*(const)?$", r))
        writes = (r.endswith("&") and not r.endswith("&&") or pointer) and not re.match(r"const\b", r)
        if pointer and writes:
            raise Wait("a pointer it writes through")
        if pointer:
            bt = "List<Int64>"   # (the data it only reads: one value that may be missing, or the first of several)
        return bt, writes and not pointer, None

    def optional_out(self, p):
        bt = self.wide(qtype(p))
        if bt is not None and re.search(r"\*\s*$", qtype(p).strip()) and not re.match(r"const\b", qtype(p).strip()):
            init = c2b.bare(kids(p)[-1]) if kids(p) else {}
            while init.get("kind") == "ImplicitCastExpr" and kids(init):
                init = c2b.bare(kids(init)[0])
            if init.get("kind") in ("CXXNullPtrLiteralExpr", "GNUNullExpr"):
                return bt
        return super().optional_out(p)

    def pin(self, bt):
        return (INT64[bt][0], "") if bt in INT64 else super().pin(bt)

    def shape(self, bt):
        return INT64[bt][1] if bt in INT64 else super().shape(bt)


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
                out.append(f"{{{c_text(name)}, PinType::{t}, {c_text(of)}, {c_text(api.shape(bt))}}}")
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
    api = EngineApi(tree, unit)
    functions, waiting = [], []
    for d in sorted((x for x in tree.decl.values() if api.declared_only(x)), key=lambda x: (x.get("_file", ""), x.get("_at", (0,))[0] or 0)):
        try:
            functions.append(api.function(d))
        except Wait as w:
            waiting.append((api.home(d) + "::" + d["name"], str(w)))
    types = [f["type"] for f in functions]
    assert len(set(types)) == len(types), [t for t in types if types.count(t) > 1]
    if "--list" in argv:
        for f in functions:
            print(("value " if f["value"] else "step  ") + f["type"] + "(" + ", ".join(f"{bt} {n}" for n, bt, _ in f["ins"]) + ")" + (" -> " + ", ".join(f"{bt} {n}" for n, bt, _ in f["outs"]) if f["outs"] else ""))
        print()
        for name, why in waiting:
            print(f"no node: {name}: {why}")
    for rel, text in (("src/logic/engine_api.gen.cpp", table_text(api, functions)), ("include/kern/logic/engine_nodes.gen.h", class_text(api, functions)),
                      ("tests/engine_case.gen.h", case_text(api, functions))):
        path = os.path.join(KERN, rel)
        if "--list" not in argv and (not os.path.exists(path) or open(path).read() != text):
            open(path, "w").write(text)
    print(f"{len(functions)} functions of the engine are nodes; {len(waiting)} have types without a place in the language")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
