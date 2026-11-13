#!/usr/bin/env python3
"""The engine as nodes: what the engine's headers declare, as nodes of the Blueprint language.

  tools/engine_api.py [--list]

A function a header of the engine declares and does not define there is something only the engine
can do: cast a ray through the physics world, draw a mesh, play a sound. Each is a node - for
every game, not for one:

  kern::physics::World::cast_ray   is the node   engine.world.cast_ray
                                   in a Blueprint's C++   engine_world_cast_ray(world, from, to ...)

  - a const method and a function that only gives something is a value, everything else a step;
  - what a function writes through a parameter (std::vector<u64>& out) is a further output;
  - an object of the engine with a life of its own (a class: the physics world, the renderer, the
    mixer) is a handle, a whole number the host gives - the first input of its class's nodes;
  - a plain struct of the engine (Pose, BodySettings, RayHit) is a struct the Blueprint declares
    with the same fields: the pin names it.

Every tier calls them through one door, logic::Host::engine(function, in, out): the game binds the
engine there, a test a stand-in.

This tool reads the headers with the compiler's own parser (as tools/cpp_to_blueprint.py does - it
shares its types) and writes
  kern/src/game/engine_api.gen.cpp           the table of the functions and their pins
  kern/include/kern/game/engine_nodes.gen.h  the functions of a Blueprint's class
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
# Not the engine a game calls: the Blueprint language itself, the engine's own helpers.
SKIP = ("kern::logic::", "kern::json::", "kern::detail::", "kern::dm::", "kern::game::nodes::")
# Functions that give something and change something (the rule below would call them values).
STEPS = {"kern::jobs::submit", "kern::jobs::take"}
PIN = {"Bool": "boolean", "Int": "integer", "Float": "number", "Vector": "vector", "Text": "text"}


def snake(name):
    return re.sub(r"(?<=[a-z0-9])([A-Z])", r"_\1", name).lower()


class Api:
    """The engine's functions as nodes, from a compiler's tree that holds the engine's headers. (The
    converter asks the same object: a call in a game's source and the node it becomes have one name.)"""

    def __init__(self, tree, unit):
        self.tree, self.u = tree, unit
        self.by_class = {}   # a class or a namespace -> its functions that are declared only, in their order

    def opaque(self, q):
        """Is this class an object of the engine with a life of its own (a handle in a Blueprint)?"""
        d = self.tree.records.get(q)
        if d is None or not q.startswith(c2b.ENGINE + "::") or q in (c2b.ENGINE + "::Vec3", c2b.ENGINE + "::Vec2"):
            return False
        if d.get("tagUsed") == "class":
            return True
        try:
            self.u.struct(q)
            return False
        except Wait:
            return True

    def handed(self, raw):
        """How a parameter is handed over: (its Blueprint type, does the function write through it, the
        engine class it is a handle of or None)."""
        r = raw.strip()
        if r.replace(" ", "") in ("constchar*", "constchar*const"):
            return "Text", False, None
        writes = pointer = False
        if r.endswith("&&"):
            r = r[:-2].strip()
        elif r.endswith("&"):
            r = r[:-1].strip()
            writes = not re.match(r"const\b", r)
        elif re.search(r"\*\s*(const)?$", r):
            r = re.sub(r"\*\s*(const)?$", "", r).strip()
            writes, pointer = not re.match(r"const\b", r), True
        r = re.sub(r"^const\s+", "", r)
        c = self.u.clean(r)
        m = re.match(r"(?:kern::)?Span<\s*(.+)>$", c)
        if m:   # a view of a list: the list
            return f"List<{self.u.element(self.u.btype(m.group(1)))}>", False, None
        q = None
        try:
            q = self.u.find_type(c)
        except Wait:
            pass
        if q is not None and q in self.tree.records and self.opaque(q):
            return "Int", False, q
        if pointer:
            # a pointer to data the function only reads: a value that may be missing (teleport's rotation) or
            # the first of several (samples, with their count beside them) - the list of them, none for null
            if writes:
                raise Wait("a pointer it writes through")
            inner = self.u.btype(r)
            if inner is None:
                raise Wait("a pointer to anything (void*)")
            return f"List<{self.u.element(inner)}>", False, None
        return self.u.btype(r), writes, None

    def pin(self, bt):
        """(pin type, the struct or enum it names) of a Blueprint type."""
        if bt in PIN:
            return PIN[bt], ""
        if bt in self.u.enum_values:
            return "enumeration", bt
        if bt in self.u.struct_fields:
            return "structure", bt
        if bt.startswith("List<"):
            t, of = self.pin(bt[5:-1])
            if t.endswith("_list"):
                raise Wait("a list in a list")
            return t + "_list", of
        raise Wait(f"a {bt}")

    def shape(self, bt):
        """A value's form, written out: n i b v t, e<values>, {fields}, [items]."""
        if bt in PIN:
            return {"Bool": "b", "Int": "i", "Float": "n", "Vector": "v", "Text": "t"}[bt]
        if bt in self.u.enum_values:
            return f"e{len(self.u.enum_values[bt])}"
        if bt in self.u.struct_fields:
            return "{" + "".join(self.shape(t) for _, t, _, _ in self.u.struct_fields[bt]) + "}"
        if bt.startswith("List<"):
            return "[" + self.shape(bt[5:-1]) + "]"
        raise Wait(f"a {bt}")

    def home(self, d):
        """The class of a method, else the namespace a function stands in."""
        return self.tree.owner.get(d["id"]) or self.tree.scope_of.get(d["id"], "").rstrip(":")

    def declared_only(self, d):
        """Is this a function only the engine can do: declared in a header, defined in the engine?"""
        if d.get("kind") not in ("FunctionDecl", "CXXMethodDecl") or d["id"] in self.tree.body or d.get("isImplicit") or d.get("explicitlyDeleted"):
            return False
        scope = self.tree.scope_of.get(d["id"], "")
        if not scope.startswith(c2b.ENGINE + "::") or any((scope + "::").startswith(s) or scope.startswith(s) for s in SKIP):
            return False
        if d.get("previousDecl") or d["id"] in self.tree.template_of or not d.get("name") or d["name"].startswith("operator") or "(anonymous" in scope:
            return False
        return not (d.get("kind") == "CXXMethodDecl" and d.get("access", "public") != "public")

    def siblings(self, d):
        home = self.home(d)
        if home not in self.by_class:
            self.by_class[home] = [x for x in self.tree.decl.values() if self.declared_only(x) and self.home(x) == home]
        return self.by_class[home]

    def type_of(self, d):
        """The node's type: engine.<class or namespace>.<function> (an overload: _2, _3 in the header's order)."""
        home = self.home(d)
        group = snake(home.split("::")[-1]) if home != c2b.ENGINE else ""
        same = [x for x in self.siblings(d) if x["name"] == d["name"]]
        nth = same.index(d) + 1 if d in same else 1
        return "engine." + (group + "." if group else "") + d["name"] + (f"_{nth}" if nth > 1 else "")

    def function(self, d):
        """The node of a declared function of the engine: a dict - or Wait, when its types have no place."""
        tree, u = self.tree, self.u
        if d.get("variadic"):
            raise Wait("a variadic function")
        owner = tree.owner.get(d["id"])
        method = d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static"
        const = qtype(d).rstrip().endswith("const")
        ins, outs, taken = [], [], {"exec", "then", "result"}

        def name_of(want):
            name = re.sub(r"\W", "_", want) or "value"
            if name in c2b.CPP_WORDS or name in taken:
                k = 2
                while f"{name}_{k}" in taken:
                    k += 1
                name = f"{name}_{k}"
            taken.add(name)
            return name
        if method:
            if self.opaque(owner):
                ins.append((name_of(snake(owner.split("::")[-1])), "Int", owner))
            else:
                st = u.struct(owner)
                ins.append((name_of("self"), st, None))
                if not const:
                    outs.append(("self", st, None))
        for i, p in enumerate(x for x in kids(d) if x.get("kind") == "ParmVarDecl"):
            bt, writes, handle = self.handed(qtype(p))
            name = name_of(p.get("name") or f"value{i + 1}")
            ins.append((name, bt, handle))
            if writes:
                outs.append((name, bt, None))
        ret = qtype(d).split("(")[0].strip()
        if ret != "void":
            bt, _, handle = self.handed(ret)
            outs.insert(0, ("result", bt, handle))
        qualified = self.home(d) + "::" + d["name"]
        value = ret != "void" and len(outs) == 1 and (const or not method) and qualified not in STEPS
        for _, bt, _ in ins + outs:
            self.pin(bt)
            self.shape(bt)
        words = d["name"].replace("_", " ").title()
        return {"type": self.type_of(d), "cpp": qualified, "label": words, "category": "Engine: " + self.home(d).split("::")[-1].replace("kern", "Engine"),
                "help": u.comment_of(d), "value": value, "ins": ins, "outs": outs, "decl": d}


def c_text(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"').replace("\n", " ") + '"'


def table_text(api, functions):
    lines = ["// The engine's functions as nodes: the table (written by tools/engine_api.py from the engine's headers -",
             "// see kern/game/engine_api.h; not edited by hand).",
             '#include "kern/game/engine_api.h"', "", "namespace kern::game {", "",
             "const std::vector<EngineFunction>& engine_functions() {", "  using logic::PinType;", "  static const std::vector<EngineFunction> all = {"]
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
    lines += ["  };", "  return all;", "}", "", "}  // namespace kern::game", ""]
    return "\n".join(lines)


def class_text(api, functions):
    """The functions of a Blueprint's class: each hands its inputs to the host and takes what it gives."""
    plain = {"Bool": "Bool", "Int": "Int", "Float": "Float", "Vector": "Vector", "Text": "const Text&"}
    lines = ["// The engine's functions as a Blueprint's class calls them (written by tools/engine_api.py from the",
             "// engine's headers - see kern/game/engine_api.h; not edited by hand). Included by kern/logic/blueprint.h.",
             "#pragma once", "", '#include "kern/logic/nodes.h"', "", "namespace kern::game {", "",
             "template <class Core> struct EngineNodesOf : Core {",
             "  using Bool = bool;", "  using Int = logic::Whole;", "  using Float = f64;", "  using Vector = Vec3;", "  using Text = std::string;",
             "  template <class T> using List = std::vector<T>;", ""]
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
            values.append(f"logic::to_value({name_})")
        zeros = ", ".join(f"logic::PinType::{api.pin(bt)[0]}" for _, bt, _ in f["outs"])
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
                lines.append(f"{head}{typed(bt)} {name}({', '.join(params)}) const {{ return logic::from_value<{typed(bt)}>({call}[0]); }}")
            else:
                lines.append(f"{head}logic::EngineValue {name}({', '.join(params)}) const {{ return {{{call}[0]}}; }}")
            continue
        # a step: what it gives goes to the node's member (n.result ...), when the text reads it
        head = f"  template <{', '.join(templates)}> " if templates else "  "
        lines.append(f"{head}void {name}({', '.join(params)}) {{ {call}; }}")
        if f["outs"]:
            takes = " ".join(f"n.{o} = logic::from_value<std::remove_cvref_t<decltype(n.{o})>>(out[{i}]);" for i, (o, _, _) in enumerate(f["outs"]))
            lines.append(f"  template <{', '.join(['class N'] + templates)}> void {name}({', '.join(['N& n'] + params)}) {{ const std::vector<logic::Value> out = {call}; {takes} }}")
    lines += ["};", "", "}  // namespace kern::game", ""]
    return "\n".join(lines)


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
    types = [f["type"] for f in functions]
    assert len(set(types)) == len(types), [t for t in types if types.count(t) > 1]
    if "--list" in argv:
        for f in functions:
            print(("value " if f["value"] else "step  ") + f["type"] + "(" + ", ".join(f"{bt} {n}" for n, bt, _ in f["ins"]) + ")" + (" -> " + ", ".join(f"{bt} {n}" for n, bt, _ in f["outs"]) if f["outs"] else ""))
        print()
        for name, why in waiting:
            print(f"no node: {name}: {why}")
    for rel, text in (("src/game/engine_api.gen.cpp", table_text(api, functions)), ("include/kern/game/engine_nodes.gen.h", class_text(api, functions))):
        path = os.path.join(KERN, rel)
        if "--list" not in argv and (not os.path.exists(path) or open(path).read() != text):
            open(path, "w").write(text)
    print(f"{len(functions)} functions of the engine are nodes; {len(waiting)} have types without a place in the language")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
