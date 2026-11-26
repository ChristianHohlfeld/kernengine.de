#!/usr/bin/env python3
"""The proof of a lowering (kern/games/spawncity/BLUEPRINTS.md, "Absenkung"): a source file of the game,
rewritten into the C++ subset so that tools/cpp_to_blueprint.py writes every function of it, behaves as it
did - bit for bit, at no higher cost.

For every source file named (and its header of the same name), the tool reads the file twice with the
compiler: as it stood at the commit the lowering started from (--base) and as it stands now. It pairs each
function of the old text with the one of the new text that took its place, by the rules of the lowering
(each one once, in general terms - no list of names):

  - the same function: the same place (namespaces, class), the same name, the same parameters;
  - the state of the file made explicit: the same function with more parameters, each a reference to a
    struct of the file's own whose fields are the old variables of the file - a field is called as the
    variable without its prefix `g_` (`std::vector<Option> g_rows` -> `SettingsTable::rows`);
  - a template made one function per type it is used with: the instance f<T> of the old is f_T of the new,
    at the same place, T as the game spells its numbers (u8, i32, u32, f32, f64 ...).

A function of the old that has no such counterpart may be one only its own file knew (in a namespace without
a name, or static): what it did is then compared through the functions that called it - every one of them
must be compared. Any other is an error.

The test (tests/lowered/test_<file>_lowered.cpp, a test of run_tests.sh) holds both texts, each in a
namespace of its own (sc_old_<file>, sc_new_<file>), compiled as the game compiles them, and calls each pair that differs
with the same drawn state and arguments - the variables of the file, the object, every parameter (the draws
of the converted tests, tests/source_diff.h). Compared bit for bit: what it returns, every parameter it may
change, the object, every variable of the file (with its counterpart in the new: the same variable, or the
field it became), the Blueprints of the actors the file calls on. The new must not ask for memory more often
than the old. What C++ leaves open (an entry outside a list, a division by zero ...) goes through guards and
marks the call, which is then not compared.

    tools/source_diff.py --base <commit> --test tests/lowered -I../../include -I../../third_party/box3d/include -Isrc -- src/settings.cpp

(in kern/games/spawncity, like the converter). It prints, per file, every pair it compares and every function
it compares through its callers."""

import argparse
import importlib.util
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))


def _converter():
    spec = importlib.util.spec_from_file_location("cpp_to_blueprint", os.path.join(HERE, "cpp_to_blueprint.py"))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m


C = _converter()
kids, walk = C.kids, C.walk

# The numbers as the game spells them (common.h: kern's u8 ... f64) - the name an instance of a template gets.
SHORT = {"unsigned char": "u8", "signed char": "i8", "unsigned short": "u16", "short": "i16", "unsigned int": "u32", "int": "i32",
         "unsigned long": "u64", "long": "i64", "unsigned long long": "u64", "long long": "i64", "float": "f32", "double": "f64", "bool": "bool"}
FUNCTIONS = ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl")
SIGNED = {"int", "long", "long long"}
INTEGRAL = SIGNED | {"unsigned int", "unsigned long", "unsigned long long"}
LISTS = re.compile(r"^(?:const\s+)?(?:std::vector<|std::array<|std::basic_string<|std::basic_string_view<|kern::Span<|std::string\b|std::string_view\b)")


class Stop(Exception):
    """What the tool cannot do for a file - said with the reason; the file has no test then."""


def plain(t):
    return re.sub(r"\s+", " ", re.sub(r"\(anonymous namespace\)::", "", t or "")).strip()


NUMBER_NAMES = {"u8": "unsigned char", "i8": "signed char", "u16": "unsigned short", "i16": "short", "u32": "unsigned int", "i32": "int",
                "u64": "unsigned long", "i64": "long", "f32": "float", "f64": "double", "size_t": "unsigned long", "usize": "unsigned long"}


def canon(t):
    """A type as the source spells it, with the game's names of numbers as what they are."""
    t = re.sub(r"(?<![\w:])(?:kern::|sc::|std::)?(" + "|".join(NUMBER_NAMES) + r")\b", lambda m: NUMBER_NAMES[m.group(1)], plain(t))
    return re.sub(r"\s*([<>,&*])\s*", r"\1", t)


def strip_comments(text):
    return re.sub(r"//[^\n]*|/\*.*?\*/", " ", text, flags=re.S)


def norm(text):
    """A text of C++ without its comments and with its spaces as one: what is compared to see a change."""
    return re.sub(r"\s+", " ", strip_comments(text)).strip()


def split_params(sig):
    """The parameter types of a function type as the compiler spells it: `void (std::vector<u8> &, T)`."""
    s = re.sub(r"\s*(const|noexcept|&|&&)\s*$", "", sig.strip())
    depth, end = 0, None
    for i in range(len(s) - 1, -1, -1):
        if s[i] == ")":
            if depth == 0:
                end = i
            depth += 1
        elif s[i] == "(":
            depth -= 1
            if depth == 0:
                start = i
                break
    inner = s[start + 1:end]
    ret = s[:start].strip()
    out, depth, cur = [], 0, ""
    for ch in inner:
        if ch in "<([":
            depth += 1
        elif ch in ">)]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return ret, out


def engine_named(t, desugared):
    """A type as the source spells it, where it names a class of the engine by a name a using-declaration gave it
    (`using kern::MeshData;` in vehicle_art.cpp: `const MeshData &`) - by its full name (`const kern::MeshData &`):
    the test does not stand where that declaration does."""
    def full(m):
        w = m.group(0)
        q = re.search(r"(?<![\w:])((?:::)?kern::(?:\w+::)*)" + re.escape(w) + r"\b", desugared or "")
        return q.group(1) + w if q else w
    return re.sub(r"(?<![\w:])(?!(?:const|volatile|unsigned|signed|struct|class|enum)\b)[A-Za-z_]\w*\b(?!\s*::)", full, t)


class Fn:
    """A function of the changed file with its body."""

    def __init__(self, node, scope, owner, version):
        self.node, self.scope, self.owner, self.v = node, scope, owner, version
        self.name = node.get("name")
        self.kind = node["kind"]
        self.file = os.path.abspath(node.get("_file") or "")
        self.at = node.get("_at")
        self.local = "(anonymous)" in scope or node.get("storageClass") == "static" and owner is None
        self.method = self.kind in ("CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl") and owner is not None
        # (a static method defined outside its class says so where it is declared)
        self.static = node.get("storageClass") == "static" or version.decl_by_id.get(node.get("previousDecl"), {}).get("storageClass") == "static"
        self.template = None   # (the pattern's parameter names, {name: the type of this instance})
        sig = (node.get("type") or {}).get("qualType", "")
        self.ret, self.sig_params = split_params(sig) if "(" in sig else ("", [])
        self.const = bool(re.search(r"\)\s*const\b", sig))
        # (a name of the engine the definition spells by a using-declaration of its file: the first declaration - the
        # header's, which every file sees - or the compiler's own spelling says which class it is)
        first = node
        while version.decl_by_id.get(first.get("previousDecl")):
            first = version.decl_by_id[first["previousDecl"]]
        declared = [x for x in kids(first) if x.get("kind") == "ParmVarDecl"]
        ps = [x for x in kids(node) if x.get("kind") == "ParmVarDecl"]
        said = lambda k, p: " ".join([(p.get("type") or {}).get("desugaredQualType") or "", ((declared[k].get("type") or {}).get("qualType", "") if len(declared) == len(ps) else ""), C.qtype(p)])
        self.params = [(p.get("name") or f"p{k}", engine_named((p.get("type") or {}).get("qualType", ""), said(k, p)), C.qtype(p)) for k, p in enumerate(ps)]
        self.body = next((c for c in kids(node) if c.get("kind") == "CompoundStmt"), None)

    @property
    def cpp_scope(self):
        return "::".join(s for s in self.scope if s != "(anonymous)")

    def label(self):
        args = ", ".join(plain(t) for _, t, _ in self.params)
        targs = f"<{', '.join(self.template[1].values())}>" if self.template else ""
        own = f"{self.owner.split('::')[-1]}::" if self.owner else ""
        return f"{own}{self.name}{targs}({args})"

    def text(self):
        a, b = self.at
        return self.v.text(self.file)[a:b]

    def param_types(self):
        """The types of its parameters as the source spells them, its numbers by what they are (u32 is
        unsigned int), without `(anonymous namespace)` - what two texts of one function have alike."""
        return [canon(t) for _, t, _ in self.params]


class Version:
    """One state of the changed files: as the compiler reads the source file (and its header)."""

    def __init__(self, cpp, header, flags, namespace, side, root=None):
        self.side = side   # "old" / "new": its namespace in the test is sc_<side>
        self.root = os.path.abspath(root) if root else os.path.dirname(os.path.abspath(cpp))   # the folder of the game's sources
        self.now_root = self.root   # (the same folder as it stands: the old side's is set by Module)
        self.cpp = os.path.abspath(cpp)
        self.header = os.path.abspath(header) if header else None
        self.files = {self.cpp} | ({self.header} if self.header else set())
        self.flags = flags
        self.namespace = namespace
        self.dumps = C.run_clang(self.cpp, namespace, flags)
        if not self.dumps:
            raise Stop(f"the compiler does not read {cpp}")
        st = {"file": ""}
        for o in self.dumps:
            C.annotate(o, st)
        self.texts = {}
        self.functions = []
        self.records = {}       # qualified name (without anonymous namespaces) -> the definition
        self.record_ids = {}    # id -> qualified name
        self.hidden = set()     # the qualified names of what a class keeps to itself (declared private or protected in it)
        self.enums = {}         # qualified name -> the definition (of the whole namespace: what the test may draw)
        self.aliases = {}       # qualified name -> a typedef / using of a type
        self.declared = []      # (a function the changed files declare without defining it, its scope)
        self.program_ns = namespace   # (the namespace of the program around this side: the old program's for the old)
        self.variables = []     # (qualified name, node) - the variables of the source file's namespaces
        self.decl_by_id = {}
        for o in self.dumps:
            self._ids(o, [namespace])
        for o in self.dumps:
            self._visit(o, [namespace], None)

    def text(self, file):
        f = os.path.abspath(file)
        if f not in self.texts:
            # (each byte a character: where the compiler says something stands, it counts bytes - and a text of the
            # program holds more than ASCII: "🚗", "ä")
            self.texts[f] = open(f, "rb").read().decode("latin-1")
        return self.texts[f]

    def mine(self, node):
        return os.path.abspath(node.get("_file") or "?") in self.files

    def _ids(self, n, scope):
        k = n.get("kind")
        if "id" in n:
            self.decl_by_id[n["id"]] = n
        if k == "NamespaceDecl":
            for c in kids(n):
                self._ids(c, scope + [n.get("name") or "(anonymous)"])
        elif k in ("CXXRecordDecl", "ClassTemplateDecl") and n.get("name"):
            if n.get("parentDeclContextId") in self.record_ids:
                # (a class of a class, defined after it - `struct CityStream::Tile { ... };`: it stands in that class)
                scope = self.record_ids[n["parentDeclContextId"]].split("::")
            q = "::".join(s for s in scope + [n["name"]] if s != "(anonymous)")
            self.record_ids[n["id"]] = q
            # (what the class keeps to itself: the classes and enums it declares where no one else may name them)
            access = "private" if n.get("tagUsed") == "class" else "public"
            for c in kids(n):
                if c.get("kind") == "AccessSpecDecl":
                    access = c.get("access", access)
                elif c.get("kind") in ("CXXRecordDecl", "EnumDecl") and c.get("name") and not c.get("isImplicit") and (access != "public" or q in self.hidden):
                    self.hidden.add(q + "::" + c["name"])
            for c in kids(n):
                self._ids(c, scope + [n["name"]])

    def _visit(self, n, scope, owner):
        k = n.get("kind")
        q = lambda name: "::".join(s for s in scope + [name] if s != "(anonymous)")
        if k == "NamespaceDecl":
            for c in kids(n):
                self._visit(c, scope + [n.get("name") or "(anonymous)"], None)
        elif k in ("CXXRecordDecl", "EnumDecl") and n.get("parentDeclContextId") in self.record_ids and n.get("parentDeclContextId") != (scope and self._scope_id(scope)):
            # (a class or enum of a class defined outside of it - `struct CityStream::Tile { ... }`: in its class)
            self._visit(dict(n, parentDeclContextId=None), self.record_ids[n["parentDeclContextId"]].split("::"), owner)
        elif k == "CXXRecordDecl" and n.get("name"):
            if owner is None and n.get("parentDeclContextId") in self.record_ids:
                scope = self.record_ids[n["parentDeclContextId"]].split("::")   # (see _ids)
                q = lambda name: "::".join(s for s in scope + [name] if s != "(anonymous)")
            if n.get("completeDefinition") and not n.get("isImplicit"):
                self.records.setdefault(q(n["name"]), (n, scope))
            for c in kids(n):
                if c.get("kind") == "CXXRecordDecl" and c.get("isImplicit"):
                    continue
                self._visit(c, scope + [n["name"]], q(n["name"]))
        elif k == "EnumDecl" and n.get("name"):
            self.enums[q(n["name"])] = n
        elif k in ("TypedefDecl", "TypeAliasDecl") and n.get("name"):
            self.aliases[q(n["name"])] = n
        elif k == "FunctionTemplateDecl":
            pattern = next((c for c in kids(n) if c.get("kind") in FUNCTIONS), None)
            names = [c.get("name") for c in kids(n) if c.get("kind") == "TemplateTypeParmDecl"]
            for c in kids(n):
                if c.get("kind") not in FUNCTIONS or c is pattern or not self.mine(pattern or c) or not c.get("isUsed"):
                    continue
                f = self._function(c, scope, owner, pattern)
                if f is None:
                    continue
                psig = (pattern.get("type") or {}).get("qualType", "")
                isig = (c.get("type") or {}).get("qualType", "")
                f.template = (names, self._unify(psig, isig, names))
                f.params = [(nm, t, t) for (nm, _, _), t in zip(f.params, split_params(isig)[1])]
                self.functions.append(f)
        elif k in FUNCTIONS:
            if n.get("isImplicit") or not self.mine(n):
                return
            own = owner
            if own is None and n.get("parentDeclContextId") in self.record_ids:
                own = self.record_ids[n["parentDeclContextId"]]
                scope = own.split("::")
            f = self._function(n, scope, own, None)
            if f is not None:
                self.functions.append(f)
            elif own is None and n.get("kind") == "FunctionDecl" and n.get("_at", (None, None))[0] is not None:
                self.declared.append((n, scope))
        elif k == "VarDecl" and owner is None and os.path.abspath(n.get("_file") or "?") == self.cpp:
            self.variables.append((q(n.get("name") or "?"), n, scope))

    def _scope_id(self, scope):
        q = "::".join(x for x in scope if x != "(anonymous)")
        return next((i for i, name in self.record_ids.items() if name == q), None)

    def _function(self, n, scope, owner, pattern):
        body = next((c for c in kids(n) if c.get("kind") == "CompoundStmt"), None)
        if body is None or n.get("_at", (None, None))[0] is None:
            return None
        return Fn(n, scope, owner, self)

    @staticmethod
    def _unify(pattern, instance, names):
        """The types the template's parameters are in an instance: the pattern's type matched with the instance's.
        (The compiler spells one type in more than one way - `std::vector<Scorch> &`, `const sc::Marks::Scorch &`:
        the same name at the end, more or less of what it stands in before it. The fullest spelling is the type.)"""
        parts = re.split(r"(?<![\w])(" + "|".join(re.escape(nm) for nm in names) + r")(?![\w])", re.sub(r"\s+", " ", pattern))
        rx, met = "", {}
        for k, part in enumerate(parts):
            if k % 2 == 0:
                rx += re.escape(part)
            else:
                met.setdefault(part, []).append(f"p{k}")
                rx += f"(?P<p{k}>.+?)"
        m = re.fullmatch(rx, re.sub(r"\s+", " ", instance))
        if not m:
            raise Stop(f"the instance {instance} of the template {pattern}: its types are not found")
        out = {}
        for nm in names:
            spelled = sorted({m.group(g) for g in met.get(nm, [])}, key=len)
            if not spelled or any(a != spelled[-1] and not spelled[-1].endswith("::" + a) for a in spelled):
                raise Stop(f"the instance {instance} of the template {pattern}: its types are not found")
            out[nm] = spelled[-1]
        return out

    # ---- the game's other files, as this text sees them ----
    def outside(self):
        """The declarations of the game's other files (the folder of its sources, not the changed file and its header) the
        unit reads, in its order: [(scope, node)]."""
        out = []
        def visit(n, scope):
            if n.get("kind") == "NamespaceDecl":
                for c in kids(n):
                    visit(c, scope + [n.get("name") or "(anonymous)"])
                return
            if n.get("isImplicit") or n.get("kind") in ("UsingShadowDecl",):
                return
            f = os.path.abspath(n.get("_file") or "?")
            if f in self.files or not f.startswith(self.root + os.sep) or n.get("_at", (None, None))[0] is None or n["_at"][1] is None:
                return
            out.append((scope, n))
        for o in self.dumps:
            visit(o, [self.namespace])
        return out

    def decl_text(self, node):
        """A declaration's text as it stands in its file, with the `;` that ends it."""
        f = os.path.abspath(node["_file"])
        raw = self.text(f)
        a, b = node["_at"]
        m = re.match(r"\s*;", raw[b:])
        return raw[a:b + (m.end() if m else 0)]

    # ---- what the file keeps ----
    def actor_of(self, node):
        """The actor a variable of the file calls on - one of the program's ActorBlueprints - or None."""
        t = plain(C.qtype(node))
        def is_actor(name, depth=0):
            name = re.sub(r"^(const\s+)?(struct\s+|class\s+)?", "", name).strip()
            if name.endswith("logic::ActorBlueprint"):
                return True
            for q, (rec, _) in self.records.items():
                if q == name or q.endswith("::" + name) or name.endswith("::" + q):
                    return depth < 8 and any(is_actor((b.get("type") or {}).get("qualType", ""), depth + 1) for b in rec.get("bases", []) or [])
            return False
        if not is_actor(t):
            return None
        lit = next((x for x in walk(node) if x.get("kind") == "StringLiteral"), None)
        if lit is None:
            raise Stop(f"the actor of {node.get('name')}: its name is not written where it is made")
        return C.c_string(lit["value"]).decode("utf-8")

    @staticmethod
    def constant(node):
        return bool(node.get("constexpr")) or bool(re.search(r"\bconst\b", (node.get("type") or {}).get("qualType", "")))

    def registration(self, node):
        """A variable whose initializer calls a function at static initialization (KERN_JOB registers a job
        that way): the test does not repeat that - its copies of the functions would take the program's place."""
        init = kids(node)[-1] if node.get("init") and kids(node) else None
        if init is None:
            return False
        # (a variable something of the file reads is no registration, whatever computes it: `const Vec3 kGrey =
        # kern::hex_color(0x8a857c);` is a constant, and the functions that read it need it)
        refers = lambda root: any(x.get("kind") == "DeclRefExpr" and (x.get("referencedDecl") or {}).get("id") == node.get("id") for x in walk(root))
        if any(refers(f.body) for f in self.functions) or any(v is not node and refers(v) for _, v, _ in self.variables):
            return False
        # (a registration hands a function to the one it calls: the address of a function is among the arguments -
        # a value computed at start, std::tan(...), is none)
        for x in walk(init):
            if x.get("kind") == "CallExpr":
                for arg in kids(x)[1:]:
                    for y in walk(arg):
                        if y.get("castKind") == "FunctionToPointerDecay" or y.get("kind") == "UnaryOperator" and y.get("opcode") == "&" and any(
                                (z.get("referencedDecl") or {}).get("kind") in ("FunctionDecl", "CXXMethodDecl") for z in kids(y)):
                            return True
        return False

    def holds_blueprint(self, node):
        """Whether a variable of the file holds the running Blueprint of an actor as the old program bound it - a
        kern::logic::Instance*, a reference to one, or a class of the game that holds one: what the file's binding
        function sets (Module.bindings), not state of the file."""
        t = (node.get("type") or {}).get("qualType", "") + " " + C.qtype(node)
        if re.search(r"\blogic::Instance\b", t):
            return True
        name = re.sub(r"^(const\s+)?(struct\s+|class\s+)?", "", plain(C.qtype(node))).strip()
        for q, (rec, _) in self.records.items():
            if q == name or q.endswith("::" + name) or name.endswith("::" + q):
                return any(re.search(r"\blogic::Instance\b", (c.get("type") or {}).get("qualType", "")) for c in kids(rec) if c.get("kind") == "FieldDecl")
        return False

    def state(self):
        """The variables of the file that hold state: not const, no actor's Blueprint."""
        return [(q, n) for q, n, _ in self.variables if not self.constant(n) and self.actor_of(n) is None and not self.holds_blueprint(n)]

    def actors(self):
        return [(q, self.actor_of(n)) for q, n, _ in self.variables if self.actor_of(n) is not None]

    def users(self):
        """Of each function, the variables of the file it reads or changes - itself or through what it calls in the file."""
        ids = {n["id"]: q for q, n in self.state()}
        by_id = {f.node["id"]: f for f in self.functions}
        direct, calls = {}, {}
        for f in self.functions:
            used, called = set(), set()
            for x in walk(f.body):
                r = x.get("referencedDecl") or {}
                if x.get("kind") == "DeclRefExpr" and r.get("id") in ids:
                    used.add(ids[r["id"]])
                if x.get("kind") in ("DeclRefExpr", "MemberExpr"):
                    rid = r.get("id") if x.get("kind") == "DeclRefExpr" else x.get("referencedMemberDecl")
                    if rid in by_id:
                        called.add(rid)
            direct[f.node["id"]], calls[f.node["id"]] = used, called
        out = {}
        for f in self.functions:
            seen, todo, used = set(), [f.node["id"]], set()
            while todo:
                i = todo.pop()
                if i in seen:
                    continue
                seen.add(i)
                used |= direct.get(i, set())
                todo += list(calls.get(i, ()))
            out[f.node["id"]] = used
        return out

    def callers(self):
        """Of each function, the functions of the file that call it."""
        by_id = {f.node["id"]: f for f in self.functions}
        out = {f.node["id"]: set() for f in self.functions}
        for f in self.functions:
            for x in walk(f.body):
                r = x.get("referencedDecl") or {}
                rid = r.get("id") if x.get("kind") == "DeclRefExpr" else x.get("referencedMemberDecl") if x.get("kind") == "MemberExpr" else None
                if rid in by_id and rid != f.node["id"]:
                    out[rid].add(f.node["id"])
        return out

    # ---- the guards: what C++ leaves open, said where it stands ----
    def guards(self):
        """The changes to the file's text that send what C++ leaves open through the guards (source_diff.h,
        converted.h) - and leave out what registers itself at static initialization: {file: [(at, end, text, key)]}."""
        edits = {f: [] for f in self.files}
        # (a function the changed files declare and another file defines - the level's text, level_data.cpp: the
        # program's, named where the declaration stands)
        defined = {(tuple(f.scope), f.name) for f in self.functions}
        for n, scope in self.declared:
            if (tuple(scope), n.get("name")) in defined:
                continue
            f = os.path.abspath(n.get("_file") or "?")
            if f not in edits:
                continue
            a, b = n["_at"]
            mm = re.match(r"\s*;", self.text(f)[b:])
            chain = "::".join([self.program_ns] + [x for x in scope[1:] if x != "(anonymous)"] + [n["name"]])
            edits[f].append((a, b + (mm.end() if mm else 0), f"using ::{chain};", (a, 2, 0)))
        for f in self.functions:
            self._guard(f.body, edits[f.file], f.file, 0)
            # (and what a constructor makes its fields with: `puffs_(pool)` is a list of `pool` entries)
            for c in kids(f.node):
                if c.get("kind") == "CXXCtorInitializer":
                    for x in kids(c):
                        if os.path.abspath(x.get("_file") or "?") == f.file and x.get("_at", (None, None))[0] is not None:
                            self._guard(x, edits[f.file], f.file, 0)
        for q, n, _ in self.variables:
            if self.registration(n):
                a, b = n["_at"]
                if "expansionLoc" in (n.get("range") or {}).get("begin", {}):
                    b = self.invocation_end(self.cpp, a)   # (written by a macro: the whole invocation)
                edits[self.cpp].append((a, b, "", (a, 3, 0)))
        # (the instances of a template have the text of the template: their guards stand there once - and must be
        # the same in every instance)
        for f, e in edits.items():
            seen = {}
            for x in e:
                if (x[0], x[3]) in seen and seen[(x[0], x[3])] != x:
                    raise Stop(f"the instances of a template meet what C++ leaves open in other places ({os.path.basename(f)}:{self.text(f).count(chr(10), 0, x[0]) + 1})")
                seen[(x[0], x[3])] = x
            edits[f] = list(dict.fromkeys(e))
        return edits

    def invocation_end(self, file, a):
        """Where the invocation of a macro that begins at `a` ends: after its name, the brackets of its arguments."""
        raw = self.text(file)
        m = re.match(r"[A-Za-z_]\w*\s*\(", raw[a:])
        if not m:
            raise Stop(f"a declaration written by a macro without arguments ({os.path.basename(file)}:{raw.count(chr(10), 0, a) + 1})")
        depth, i = 0, a + m.end() - 1
        while i < len(raw):
            depth += {"(": 1, ")": -1}.get(raw[i], 0)
            i += 1
            if depth == 0:
                return i
        raise Stop("a macro's invocation without its end")

    def _guard(self, n, out, file, depth):
        raw = self.text(file)
        k, ks = n.get("kind"), kids(n)
        def span(x):
            a, b = x.get("_at", (None, None))
            if a is None or b is None or os.path.abspath(x.get("_file") or "?") != file:
                raise Stop(f"a {x.get('kind')} written by a macro ({os.path.basename(file)}:{raw.count(chr(10), 0, a or 0) + 1})")
            return a, b
        def wrap(x, head, tail=")"):
            a, b = span(x)
            out.append((a, a, head, (a, 1, depth)))
            out.append((b, b, tail, (b, 0, -depth)))
        def between(x, y, token, text):
            _, xb = span(x)
            ya, _ = span(y)
            if raw[xb:ya].strip() != token:
                raise Stop(f"{token} written by a macro ({os.path.basename(file)}:{raw.count(chr(10), 0, xb) + 1})")
            out.append((xb, ya, text, (xb, 2, 0)))
        ct = lambda x: plain(C.qtype(x))
        def literal(x):
            b = C.bare(x)
            while b.get("kind") in ("ImplicitCastExpr", "CStyleCastExpr", "CXXStaticCastExpr", "CXXFunctionalCastExpr", "ParenExpr") and kids(b):
                b = C.bare(kids(b)[-1])
            if b.get("kind") == "IntegerLiteral":
                return int(b.get("value"))
            return None
        def width(t):
            return {"int": 32, "unsigned int": 32, "long": 64, "unsigned long": 64, "long long": 64, "unsigned long long": 64}.get(t)
        done = False
        if k in ("ForStmt", "WhileStmt", "DoStmt", "CXXForRangeStmt"):
            # (a loop counts its rounds: a call that goes round longer than the test can wait - a loop to a length drawn
            # as four thousand million - is given up, as one that reads outside a list for ever)
            body = ks[0] if k == "DoStmt" else ks[-1] if ks else None
            ba = (body or {}).get("_at", (None, None))[0]
            if body is not None and body.get("kind") == "CompoundStmt" and ba is not None and os.path.abspath(body.get("_file") or "?") == file and raw[ba] == "{":
                out.append((ba + 1, ba + 1, " ::sc::lowered::tick();", (ba + 1, 0, -10 ** 6)))
            else:
                inner = n.get("inner") or []
                cond = (inner[2] if k == "ForStmt" and len(inner) > 2 else ks[1] if k == "DoStmt" and len(ks) > 1 else ks[-2] if k == "WhileStmt" and len(ks) >= 2 else None)
                if cond and cond.get("kind"):
                    wrap(cond, "(::sc::lowered::tick(), ")
        if k == "LambdaExpr":
            for c in ks[-1:]:
                self._guard(c, out, file, depth + 1)
            return
        if k == "ArraySubscriptExpr" and len(ks) == 2:
            base = ks[0]
            inner = C.bare(base)
            by_pointer = not (inner.get("kind") == "ImplicitCastExpr" and inner.get("castKind") == "ArrayToPointerDecay")
            a, b = span(n)
            # (an entry by a pointer: lowered::entry checks it against the lists the test handed - a pointer into
            # another place stands as it is)
            out.append((a, a, "::sc::lowered::entry(" if by_pointer else "::kern::converted::at(", (a, 1, depth)))
            between(base, ks[1], "[", ", ")
            _, ib = span(ks[1])
            if raw[ib:b].strip() != "]":
                raise Stop("] written by a macro")
            out.append((ib, b, ")", (ib, 2, 0)))
            done = True
        elif k == "CXXOperatorCallExpr" and len(ks) == 3:
            callee = C.bare(ks[0])
            while callee.get("kind") == "ImplicitCastExpr" and kids(callee):
                callee = C.bare(kids(callee)[0])
            if (callee.get("referencedDecl") or {}).get("name") == "operator[]" and LISTS.match(ct(ks[1])):
                a, b = span(n)
                out.append((a, a, "::kern::converted::at(", (a, 1, depth)))
                between(ks[1], ks[2], "[", ", ")
                _, ib = span(ks[2])
                if raw[ib:b].strip() != "]":
                    raise Stop("] written by a macro")
                out.append((ib, b, ")", (ib, 2, 0)))
                for c in ks[1:]:
                    self._guard(c, out, file, depth + 1)
                return
        elif k == "CXXMemberCallExpr" and ks and ks[0].get("kind") == "MemberExpr" and kids(ks[0]) and LISTS.match(ct(kids(ks[0])[0])):
            m, base = ks[0], kids(ks[0])[0]
            name = m.get("name")
            if name in ("at", "front", "back", "pop_back", "erase") and not m.get("isArrow"):
                a, b = span(n)
                _, bb = span(base)
                out.append((a, a, f"::kern::converted::{name}(", (a, 1, depth)))
                if len(ks) > 1:
                    xa, _ = span(ks[1])
                    if not re.fullmatch(r"\s*\.\s*" + name + r"\s*\(\s*", raw[bb:xa]):
                        raise Stop(f".{name}( written by a macro")
                    out.append((bb, xa, ", ", (bb, 2, 0)))
                else:
                    if not re.fullmatch(r"\s*\.\s*" + name + r"\s*\(\s*\)", raw[bb:b]):
                        raise Stop(f".{name}() written by a macro")
                    out.append((bb, b, ")", (bb, 2, 0)))
            elif name in ("resize", "reserve", "assign") and len(ks) > 1 and ct(ks[1]) in INTEGRAL | {"unsigned char", "unsigned short", "short", "signed char"}:
                wrap(ks[1], "::kern::converted::count(")
        elif k == "CXXConstructExpr" and re.match(r"^(const )?std::vector<", ct(n)) and ks and not n.get("list") and len(ks) <= 2 and ct(ks[0]) in INTEGRAL:
            wrap(ks[0], "::kern::converted::count(")
        elif k == "CallExpr" and C.callee_name(n) in ("memcpy", "memmove") and len(ks) == 4:
            # a copy between places of lists (or of a value's bytes): checked against the lists it names
            a, b = span(n)
            fa, _ = span(ks[1])
            if not re.fullmatch(r"\s*(?:::)?(?:std::)?mem(?:cpy|move)\s*\(\s*", raw[a:fa]):
                raise Stop(f"memcpy written by a macro ({os.path.basename(file)}:{raw.count(chr(10), 0, a) + 1})")
            out.append((a, fa, f"::sc::lowered::{'copy' if C.callee_name(n) == 'memcpy' else 'move'}(", (a, 2, 0)))
            for x in ks[1:3]:
                self._place(x, out, file, depth + 1)   # (what the places name is written as it stands: no guard inside)
            self._guard(ks[3], out, file, depth + 1)
            return
        elif k == "BinaryOperator" and n.get("opcode") in ("/", "%", "<<", ">>") and len(ks) == 2 and ct(n) in INTEGRAL:
            op = n["opcode"]
            lit = literal(ks[1])
            safe = lit is not None and (op in ("/", "%") and lit not in (0, -1) or op in ("<<", ">>") and 0 <= lit < width(ct(n)))
            if not safe:
                a, _ = span(n)
                out.append((a, a, "::sc::lowered::" + {"/": "quot", "%": "rem", "<<": "shl", ">>": "shr"}[op] + "(", (a, 1, depth)))
                between(ks[0], ks[1], op, ", ")
                _, b = span(n)
                out.append((b, b, ")", (b, 0, -depth)))
        elif k == "BinaryOperator" and n.get("opcode") in ("+", "-", "*") and len(ks) == 2 and ct(n) in SIGNED and not (literal(ks[0]) is not None and literal(ks[1]) is not None) \
                and not any(re.search(r"\*\s*$|\[\d*\]\s*$", ct(x)) for x in ks):   # (the distance of two pointers is no sum of numbers)
            op = n["opcode"]
            a, b = span(n)
            out.append((a, a, "::sc::lowered::" + {"+": "plus", "-": "minus", "*": "times"}[op] + "(", (a, 1, depth)))
            between(ks[0], ks[1], op, ", ")
            out.append((b, b, ")", (b, 0, -depth)))
        elif k == "CompoundAssignOperator" and len(ks) == 2 and plain((n.get("computationResultType") or {}).get("qualType", "")) in INTEGRAL and n.get("opcode") in ("+=", "-=", "*=", "/=", "%=", "<<=", ">>="):
            op = n["opcode"]
            rt = plain((n.get("computationResultType") or {}).get("qualType", ""))
            if op in ("/=", "%=", "<<=", ">>=") or rt in SIGNED:
                lit = literal(ks[1])
                safe = op in ("/=", "%=") and lit is not None and lit not in (0, -1) or op in ("<<=", ">>=") and lit is not None and 0 <= lit < width(rt)
                if not safe and (op in ("/=", "%=", "<<=", ">>=") or rt in SIGNED):
                    a, b = span(n)
                    out.append((a, a, "::sc::lowered::" + {"+=": "plus_to", "-=": "minus_to", "*=": "times_to", "/=": "quot_to", "%=": "rem_to", "<<=": "shl_to", ">>=": "shr_to"}[op] + "(", (a, 1, depth)))
                    between(ks[0], ks[1], op, ", ")
                    out.append((b, b, ")", (b, 0, -depth)))
        elif k == "UnaryOperator" and n.get("opcode") in ("++", "--") and ks and ct(ks[0]) in SIGNED:
            a, b = span(n)
            xa, xb = span(ks[0])
            name = ("post_" if n.get("isPostfix") else "pre_") + ("inc" if n["opcode"] == "++" else "dec")
            if n.get("isPostfix"):
                if raw[xb:b].strip() != n["opcode"]:
                    raise Stop("++ written by a macro")
                out.append((a, a, f"::sc::lowered::{name}(", (a, 1, depth)))
                out.append((xb, b, ")", (xb, 2, 0)))
            else:
                if raw[a:xa].strip() != n["opcode"]:
                    raise Stop("++ written by a macro")
                out.append((a, xa, f"::sc::lowered::{name}(", (a, 2, 0)))
                out.append((b, b, ")", (b, 0, -depth)))
        elif k == "UnaryOperator" and n.get("opcode") == "-" and ks and ct(n) in SIGNED and literal(ks[0]) is None:
            a, b = span(n)
            xa, _ = span(ks[0])
            out.append((a, xa, "::sc::lowered::negated(", (a, 2, 0)))
            out.append((b, b, ")", (b, 0, -depth)))
        elif n.get("castKind") == "FloatingToIntegral" and ks:
            wrap(ks[-1], f"::sc::lowered::fits<{C.qtype(n)}>(")
        elif n.get("castKind") == "IntegralCast" and ks and k.endswith("CastExpr") and k != "ImplicitCastExpr" and self.enum_range(ct(n)) is not None:
            # (a whole number made a value of an enum whose type is not fixed: one outside its values' range C++ leaves open)
            lo, hi = self.enum_range(ct(n))
            wrap(ks[-1], f"::sc::lowered::enum_fits<{lo}LL, {hi}LL>(")
        for c in ks:
            self._guard(c, out, file, depth + 1)

    def enum_range(self, t):
        """The range of the values of an enum of the game whose type is not fixed ([dcl.enum]: the smallest bit field that
        holds every value) - None for one whose type is fixed, or no enum."""
        name = re.sub(r"^(const\s+)?(enum\s+)?", "", plain(t)).strip()
        e = next((e for q, e in self.enums.items() if q == name or q.endswith("::" + name) or q.split("::")[-1] == name.split("::")[-1]), None)
        if e is None or e.get("fixedUnderlyingType") or e.get("scopedEnumTag"):
            return None
        values, last = [], -1
        for c in kids(e):
            if c.get("kind") != "EnumConstantDecl":
                continue
            got = next((x.get("value") for x in walk(c) if x.get("kind") == "ConstantExpr" and x.get("value") is not None), None)
            last = int(got) if got is not None else last + 1
            values.append(last)
        if not values:
            return None
        lo, hi = min(values), max(values)
        k = 1
        if lo >= 0:
            while (1 << k) - 1 < hi:
                k += 1
            return 0, (1 << k) - 1
        while not (-(1 << (k - 1)) <= lo and hi <= (1 << (k - 1)) - 1):
            k += 1
        return -(1 << (k - 1)), (1 << (k - 1)) - 1

    def _place(self, x, out, file, depth):
        """A pointer memcpy is handed, as the place of a list it is (lowered::at_place): `v.data()`, `v.data() + k`,
        `s.data + k` (a view), an array - or the address of a value (`&v`: its own bytes)."""
        raw = self.text(file)
        b = C.bare(x)
        while b.get("kind") in ("ImplicitCastExpr", "ParenExpr") and b.get("castKind") in (None, "BitCast", "NoOp", "LValueToRValue") and kids(b):
            b = C.bare(kids(b)[0])
        a0, b0 = x["_at"]
        def head(n):
            """`v.data()` / `s.data` -> v / s."""
            n = C.bare(n)
            while n.get("kind") in ("ImplicitCastExpr", "ParenExpr") and kids(n):
                n = C.bare(kids(n)[0])
            if n.get("kind") == "CXXMemberCallExpr" and kids(n) and kids(n)[0].get("name") == "data":
                return kids(kids(n)[0])[0]
            if n.get("kind") == "MemberExpr" and n.get("name") == "data" and kids(n) and re.match(r"(const )?kern::Span<", plain(C.qtype(kids(n)[0]))):
                return kids(n)[0]
            if n.get("kind") == "ImplicitCastExpr" and n.get("castKind") == "ArrayToPointerDecay":
                return kids(n)[0]
            return None
        if b.get("kind") == "UnaryOperator" and b.get("opcode") == "&" and kids(b):
            va, vb = kids(b)[0]["_at"]
            out.append((a0, b0, f"::sc::lowered::value_place({raw[va:vb]})", (a0, 2, 0)))
            return
        if b.get("kind") == "BinaryOperator" and b.get("opcode") == "+" and len(kids(b)) == 2 and head(kids(b)[0]) is not None:
            h = head(kids(b)[0])
            ha, hb = h["_at"]
            ka, kb = kids(b)[1]["_at"]
            out.append((a0, b0, f"::sc::lowered::at_place({raw[ha:hb]}, {raw[ka:kb]})", (a0, 2, 0)))
            return
        h = head(x)
        if h is not None:
            ha, hb = h["_at"]
            out.append((a0, b0, f"::sc::lowered::at_place({raw[ha:hb]}, 0)", (a0, 2, 0)))
            return
        # (a pointer of another kind: checked against the places the test handed - lowered::pointer_place)
        out.append((a0, a0, "::sc::lowered::pointer_place(", (a0, 1, depth)))
        out.append((b0, b0, ")", (b0, 0, -depth)))


def apply(text, edits):
    """The text with the changes in it: (at, end, text, key) - insertions where at == end, in the order of key."""
    out, at = [], 0
    for a, b, t, _ in sorted(edits, key=lambda e: e[3]):
        if a < at:
            raise Stop("two guards on one place")
        out.append(text[at:a])
        out.append(t)
        at = b
    out.append(text[at:])
    return "".join(out)


INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*([<"])([^>"]+)[>"][^\n]*$', re.M)


def pasted(version, file, edits, include_dirs, hoisted, old_game=None):
    """A file of the changed ones as the test holds it: without its #include lines (each is collected in
    `hoisted` - one of the changed files is pasted itself) and #pragma once, with the guards in it."""
    text = apply(version.text(file), edits.get(file, []))
    def include(m):
        kind, name = m.group(1), m.group(2)
        if kind == "<":
            hoisted.setdefault(f"#include <{name}>", None)
            return ""
        for d in [os.path.dirname(file), *include_dirs]:
            p = os.path.abspath(os.path.join(d, name))
            if os.path.exists(p):
                if p in version.files:
                    return ""
                if version.side == "old" and p.startswith(version.root + os.sep):
                    old_game.setdefault(os.path.relpath(p, version.root), None)   # (the old program's: run_tests.sh builds it)
                    return ""
                hoisted.setdefault(f'#include "{name}"', None)
                return ""
        raise Stop(f'#include "{name}" of {os.path.basename(file)} is not found')
    text = INCLUDE.sub(include, text)
    return re.sub(r"^[ \t]*#[ \t]*pragma[ \t]+once[^\n]*$", "", text, flags=re.M)


class Module:
    """One source file and its header: the old text, the new text, the pairs of their functions."""

    def __init__(self, src, base, flags, namespace, scratch):
        self.src = src
        self.stem = os.path.splitext(os.path.basename(src))[0]
        header = os.path.splitext(src)[0] + ".h"
        # (a header of the game without a source file of its own - its inline functions - is the file itself)
        self.header = header if os.path.exists(header) and os.path.abspath(header) != os.path.abspath(src) else None
        top = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True, check=True).stdout.strip()
        rel = lambda p: os.path.relpath(os.path.abspath(p), top)
        def old_text(p):
            r = subprocess.run(["git", "show", f"{base}:{rel(p)}"], capture_output=True, text=True, cwd=top)
            return r.stdout if r.returncode == 0 else None
        self.old_cpp_text = old_text(src)
        if self.old_cpp_text is None:
            raise Stop(f"{src} is not in {base}: a new file has no old text to compare with")
        # The old text sees the game as it stood: the folder of the game's sources (the file's own) at the base -
        # the engine (every other folder it includes from) as it stands. What of the game's other files differs
        # from now is then part of the old side of the test (Environment).
        self.src_dir = os.path.dirname(os.path.abspath(src))
        self.base_commit = subprocess.run(["git", "rev-parse", f"{base}^{{commit}}"], capture_output=True, text=True, cwd=top, check=True).stdout.strip()
        # (the old program, as the test links it: the game's sources at the base, in a namespace of their own - run_tests.sh)
        self.base_ns = f"{namespace}_base_{self.base_commit[:12]}"
        self.base_rel = rel(self.src_dir)
        old_tree = os.path.join(scratch, "base")
        old_dir = os.path.join(old_tree, rel(self.src_dir))
        if not os.path.isdir(old_dir):
            os.makedirs(old_tree, exist_ok=True)
            arch = subprocess.run(["git", "archive", base, "--", rel(self.src_dir)], capture_output=True, cwd=top, check=True).stdout
            subprocess.run(["tar", "-x", "-C", old_tree], input=arch, check=True)
        self.old_dir = old_dir
        old_cpp = os.path.join(old_dir, os.path.basename(src))
        old_header = os.path.join(old_dir, os.path.basename(self.header)) if self.header else None
        if old_header and not os.path.exists(old_header):
            old_header = None
        self.include_dirs = [f[2:] for f in flags if f.startswith("-I")]
        old_flags = [f"-I{old_dir}" if f.startswith("-I") and os.path.abspath(f[2:]) == self.src_dir else f for f in flags]
        self.old_include_dirs = [f[2:] for f in old_flags if f.startswith("-I")]
        self.old = Version(old_cpp, old_header, old_flags, namespace, "old", root=old_dir)
        self.old.now_root = self.src_dir
        self.old.program_ns = self.base_ns
        self.new = Version(src, self.header, flags, namespace, "new", root=self.src_dir)
        self.namespace = namespace

    def engine_records(self):
        """The classes of the engine as the new text's unit reads them (the old reads the same engine): {qualified name:
        its definition}."""
        if getattr(self, "_engine", None) is None:
            self._engine, self._engine_enums = {}, {}
            def visit(n, scope):
                k = n.get("kind")
                if k == "NamespaceDecl":
                    for c in kids(n):
                        visit(c, scope + [n.get("name") or "(anonymous)"])
                elif k == "EnumDecl" and n.get("name"):
                    self._engine_enums.setdefault("::".join(x for x in scope + [n["name"]] if x != "(anonymous)"), n)
                elif k == "CXXRecordDecl" and n.get("name") and n.get("completeDefinition") and not n.get("isImplicit"):
                    q = "::".join(x for x in scope + [n["name"]] if x != "(anonymous)")
                    self._engine.setdefault(q, n)
                    for c in kids(n):
                        if not c.get("isImplicit"):
                            visit(c, scope + [n["name"]])
            for o in C.run_clang(self.new.cpp, "kern", self.new.flags):
                visit(o, ["kern"])
        return self._engine

    @staticmethod
    def open_fields(rec):
        """Whether every field of a class is public (as the test reads the engine: its classes as they are)."""
        public = rec.get("tagUsed") != "class"
        for c in kids(rec):
            if c.get("kind") == "AccessSpecDecl":
                public = c.get("access") == "public"
            elif c.get("kind") == "FieldDecl" and not public:
                return False
        return True

    def unchanged(self):
        return self.old.text(self.old.cpp) == self.new.text(self.new.cpp) and (
            (self.old.header and self.new.header and self.old.text(self.old.header) == self.new.text(self.new.header)) or (not self.old.header and not self.new.header))

    # ---- the pairs ----
    def pairs(self):
        """[(old, new, extra)] - `extra`: the new's parameters that are the file's state, by their place, with the
        old variables each field stands for ({field: old variable}); and the old functions without counterpart."""
        olds, news = self.old.functions, self.new.functions
        state_old = [q for q, _ in self.old.state()]
        out, alone = [], []
        def place(f):
            return (f.cpp_scope, f.owner.split("::")[-1] if f.owner else None, f.kind)
        self.bindings, self.binding_gets, self.plans = [], [], {}
        for o in olds:
            if self.binder(o, "old"):
                n = next((n for n in news if place(n) == place(o) and n.name == o.name and self.binder(n, "new")), None)
                if n is None:
                    raise Stop(f"{o.label()} binds the file to a running Blueprint, and the new text has no {o.name}(kern::logic::Runtime &) that binds it")
                extra = {}
                if len(n.params) > 1:
                    got = self.extra_of([], n.param_types()[1:], state_old)
                    if got is None:
                        raise Stop(f"{n.label()} binds the file and is handed more than the state of the file")
                    extra = {j + 1: e for j, e in got.items()}
                self.bindings.append((o, n, self.bound_actors(n), extra))
                continue
            if not o.params and re.search(r"\blogic::Instance\s*\*$", plain(o.ret)) and not any(n.name == o.name and place(n) == place(o) for n in news):
                self.binding_gets.append(o)   # (what gives the Blueprint the file was bound to: gone with that way of binding)
                continue
            same_place = [n for n in news if place(n) == place(o)]
            want = o.name if not o.template else o.name + "_" + "_".join(SHORT.get(plain(t), None) or "?" for t in o.template[1].values())
            named = [n for n in same_place if n.name == want and not n.template]
            # (of two methods that differ in `const` alone - `T& all()`, `const T& all() const` - each has its own)
            const = lambda f: bool(re.search(r"\)\s*const\b", (f.node.get("type") or {}).get("qualType", "")))
            exact = [n for n in named if n.param_types() == o.param_types()]
            exact = [n for n in exact if const(n) == const(o)] or exact
            if exact:
                out.append((o, exact[0], {}))
                continue
            found = None
            for n in named:
                extra = self.extra(o, n, state_old)
                if extra is not None:
                    found = (o, n, extra)
                    break
            for n in ([] if found else named):
                got = self.shape(o, n, state_old)
                if got is not None:
                    self.plans[o.node["id"]] = got[0]
                    found = (o, n, got[1])
                    break
            if not found and o.kind == "CXXConstructorDecl":
                # (a constructor that became a function of its class that makes one: `static T of(...)`)
                cls = o.owner.split("::")[-1]
                for n in news:
                    if n.static and n.owner and n.owner.split("::")[-1] == cls and n.cpp_scope == o.cpp_scope and self.bare_type(n.ret).split("::")[-1] == cls:
                        got = self.shape(o, n, state_old) if n.param_types() != o.param_types() else ([("same", (k,), (k,)) for k in range(len(o.params))], {})
                        if got is not None:
                            self.plans[o.node["id"]] = got[0]
                            found = (o, n, got[1])
                            break
            if found:
                out.append(found)
            else:
                alone.append(o)
        return out, alone

    # ---- what a parameter became ----
    @staticmethod
    def bare_type(t):
        """A type without const and without the namespaces of the game and the engine (two texts name it otherwise)."""
        t = re.sub(r"\bconst\b", "", canon(t))
        t = re.sub(r"(?<![\w:])(?:sc|kern)::", "", t)
        return re.sub(r"\s+", "", t)

    @staticmethod
    def pointee(t):
        m = re.fullmatch(r"\s*(const\s+)?(.+?)\s*\*\s*", canon(t))
        return m.group(2) if m and not m.group(2).endswith("*") and m.group(2) not in ("char", "void") else None

    @staticmethod
    def referee(t):
        m = re.fullmatch(r"\s*(const\s+)?(.+?)\s*&\s*", canon(t))
        return m.group(2) if m else None

    @staticmethod
    def element(t):
        """The entries of a list a parameter is: std::vector<T> (by reference), kern::Span<T>, std::array<T, N> (by reference)."""
        c = canon(t)
        m = re.fullmatch(r"\s*(?:const\s+)?std::vector<(.+)>\s*&\s*", c) or re.fullmatch(r"\s*(?:const\s+)?kern::Span<(?:const\s+)?(.+)>\s*&?\s*", c) or \
            re.fullmatch(r"\s*(?:const\s+)?std::array<(.+),\s*\d+>\s*&\s*", c)
        return m.group(1) if m else None

    def shape(self, o, n, state_old):
        """How the parameters of o became those of n, where the lowering changed their form (BLUEPRINTS.md, "Zeiger"):
        each old one is the new one at its place, or
          - a pointer to one value is a reference to it (`T*` -> `T&`: the old is handed the place of the value);
          - a pointer to entries is a list (`T*` -> `std::vector<T>&`, `kern::Span<T>`, `std::array<T, N>&`), and with
            the number of entries after it, that number is the list's (`const T*, n` -> `kern::Span<const T>`);
          - a pointer to entries the old read one by one is those entries (`const T*` -> `T, T, T`);
          - a value of the Blueprint (kern::logic::Value) is the struct of the Blueprint it holds;
          - a function that gives a value is that value (`std::function<R(...)>` -> `R`);
          - a reference to an entry of a list of the object is the entry's index (`T&` -> a whole number);
          - the old's last parameter, a place it writes what it gives (`T&`, `T*`), is what the new returns;
        and the new may be handed the state of the file besides (extra). [(step, old places, new places)] and the extra;
        None: n is no such counterpart of o."""
        ot = [re.sub(r"^(\s*(?:const\s+)?)(.+?)(\s*[&*]?\s*)$", lambda mm: mm.group(1) + self.new_type(mm.group(2), self.old) + mm.group(3), t) for _, t, _ in o.params]
        # (the new's types with the game's aliases resolved: `Bytes32&` is `std::array<u8, 32>&`)
        nt = [re.sub(r"^(\s*(?:const\s+)?)(.+?)(\s*&?\s*)$", lambda mm: mm.group(1) + self.new_type(mm.group(2)) + mm.group(3), t) for _, t, _ in n.params]
        integral = lambda t: canon(t).replace("const ", "").strip() in INTEGRAL | {"unsigned char", "unsigned short", "short", "signed char", "long long", "unsigned long long"}
        is_value = lambda t: re.sub(r"\s|const|&", "", plain(t)) in ("kern::logic::Value", "logic::Value")
        new_ret = self.new_type(n.ret)
        def fn_result(t):
            m = re.fullmatch(r"\s*(?:const\s+)?std::function<(.+?)\s*\((.*)\)>\s*&?\s*", plain(t))
            return m.group(1) if m else None
        def search(i, j, plan, extra):
            if j == len(nt):
                if i == len(ot):
                    return plan, extra, False
                # (the old's last parameter, the place of what it gives: the new returns it)
                if i == len(ot) - 1 and plain(o.ret) in ("bool", "void") and new_ret and plain(new_ret) != "void":
                    t = ot[i]
                    r, p = self.referee(t), self.pointee(t)
                    writes = not re.match(r"^\s*const\b", canon(t))
                    if writes and r and self.bare_type(r) == self.bare_type(new_ret):
                        return plan + [("returned", (i,), ())], extra, True
                    arr = re.fullmatch(r"\s*std::array<(.+),\s*(\d+)>\s*", canon(new_ret))
                    if writes and p and arr and self.bare_type(p) == self.bare_type(arr.group(1)):
                        return plan + [("returned", (i,), ())], extra, True
                return None
            tries = []
            if i < len(ot):
                o_t, n_t = ot[i], nt[j]
                if self.bare_type(o_t) == self.bare_type(n_t):
                    tries.append((("same", (i,), (j,)), 1, 1))
                p = self.pointee(o_t)
                if p and self.referee(n_t) and not self.element(n_t) and self.bare_type(p) == self.bare_type(self.referee(n_t)):
                    tries.append((("place", (i,), (j,)), 1, 1))
                if p and self.element(n_t) and self.bare_type(p) == self.bare_type(self.element(n_t)):
                    if i + 1 < len(ot) and integral(ot[i + 1]):
                        tries.append((("entries", (i, i + 1), (j,)), 2, 1))
                    tries.append((("entries", (i,), (j,)), 1, 1))
                if p and re.match(r"^\s*const\b", canon(o_t)):
                    k = 0
                    while j + k < len(nt) and self.bare_type(nt[j + k]) == self.bare_type(p):
                        k += 1
                    for kk in range(k, 1, -1):
                        tries.append((("spread", (i,), tuple(range(j, j + kk))), 1, kk))
                if is_value(o_t) and not is_value(n_t):
                    tries.append((("value", (i,), (j,)), 1, 1))
                fr = fn_result(o_t)
                if fr and self.bare_type(fr) == self.bare_type(re.sub(r"^\s*const\s+|\s*&\s*$", "", n_t)):
                    tries.append((("given", (i,), (j,)), 1, 1))
                r = self.referee(o_t)
                if r and o.method and integral(n_t) and self.entry_list(o, r) is not None:
                    tries.append((("index", (i,), (j,)), 1, 1))
            got = self.extra_of([], [n.param_types()[j]], state_old)
            if got:
                tries.append((("state", (), (j,)), 0, 1))
            for step, di, dj in tries:
                e = dict(extra)
                if step[0] == "state":
                    e[j] = got[0]
                r = search(i + di, j + dj, plan + [step], e)
                if r is not None:
                    return r
            return None
        found = search(0, 0, [], {})
        if found is None:
            return None
        plan, extra, returned = found
        same_ret = returned or self.bare_type(o.ret) == self.bare_type(n.ret) or is_value(o.ret)
        return (plan, extra) if same_ret else None

    def new_type(self, t, v=None):
        """A type of the new text (or of v) with its alias (a typedef, a using of the game) resolved."""
        name = plain(t).strip()
        for q, a in (v or self.new).aliases.items():
            if q == name or q.endswith("::" + name):
                return (a.get("type") or {}).get("desugaredQualType") or (a.get("type") or {}).get("qualType") or t
        return t

    def entry_list(self, f, entry):
        """The list of the object of the method f whose entries are of the type `entry` (the one such field), or None."""
        rec = self.old.records.get(f.owner)
        if rec is None:
            return None
        lists = [c.get("name") for c in kids(rec[0]) if c.get("kind") == "FieldDecl" and
                 (m := re.fullmatch(r"\s*std::vector<(.+)>\s*", canon((c.get("type") or {}).get("qualType", "")))) and self.bare_type(m.group(1)) == self.bare_type(entry)]
        return lists[0] if len(lists) == 1 else None

    @staticmethod
    def binder(f, side):
        """A function that binds the file to the Blueprints of the level: the old program handed it the running Blueprint of
        one actor (kern::logic::Instance*), the new hands it the logic they run in (kern::logic::Runtime&)."""
        want = r"^(kern::)?logic::Instance\s*\*$" if side == "old" else r"^(kern::)?logic::Runtime\s*&$"
        # (the new one may be handed the state of the file besides - Module.extra)
        return (len(f.params) == 1 or side == "new" and len(f.params) > 1) and bool(re.match(want, plain(f.params[0][1]))) and (not f.method or f.static)

    def bound_actors(self, n):
        """The actors whose Blueprints a binding function of the new text binds (the ActorBlueprints of the file it calls
        bind on)."""
        names = {q.split("::")[-1]: a for q, a in self.new.actors()}
        got = []
        for x in walk(n.body):
            if x.get("kind") == "MemberExpr" and x.get("name") == "bind":
                ref = next((y for y in walk(x) if y.get("kind") == "DeclRefExpr"), None)
                var = ((ref or {}).get("referencedDecl") or {}).get("name")
                if var in names and names[var] not in got:
                    got.append(names[var])
        if len(got) != 1:
            raise Stop(f"{n.label()} binds {len(got)} actors' Blueprints: the old text's binding was handed one")
        return got[0]

    def extra(self, o, n, state_old):
        """The new's parameters beyond the old's - each a reference to a struct of the changed files whose fields are
        variables of the old file (the state made explicit): {its place: {field: old variable}}; None: n is no
        such counterpart of o."""
        return self.extra_of(o.param_types(), n.param_types(), state_old)

    def extra_of(self, ot, nt, state_old):
        k, extra = 0, {}
        for j, t in enumerate(nt):
            if k < len(ot) and t == ot[k]:
                k += 1
                continue
            rec = re.sub(r"^(const\s+)?(.*?)\s*&$", r"\2", t)
            got = self.new_record(rec)
            if got is None:
                return None
            fields = [c.get("name") for c in kids(got[0]) if c.get("kind") == "FieldDecl"]
            mapping = {}
            for fname in fields:
                var = next((v for v in state_old if v.split("::")[-1] in (fname, "g_" + fname)), None)
                if var is None:
                    return None
                mapping[fname] = var
            extra[j] = mapping
        return extra if k == len(ot) and extra else None

    def new_record(self, t):
        t = plain(re.sub(r"^(struct|class)\s+", "", t))
        for q, rec in self.new.records.items():
            if (q == t or q.endswith("::" + t) or t.endswith("::" + q.split("::")[-1]) and q.split("::")[-1] == t.split("::")[-1]) and Version.mine(self.new, rec[0]):
                return rec
        return None

    def changed(self, o, n, extra):
        return bool(extra) or o.template is not None or norm(o.text()) != norm(n.text()) or o.param_types() != n.param_types() or o.ret != n.ret


class Writer:
    """The test of one module."""

    def __init__(self, m, out_dir):
        self.m = m
        # (each file's test its own namespaces: two tests of one program link together)
        self.ns = {side: f"sc_{side}_{re.sub(r'\W', '_', m.stem)}" for side in ("old", "new")}
        self.out_dir = out_dir
        self.lines = []
        self.engine_enums = set()   # (the enums of the engine the test draws: Writer.reached finds them)
        self.moved = {}             # (variables of the file that became fields of a class: Writer.write)
    def name_of(self, v, q, node):
        """How the test names a class, an enum or a type of the game (q: its qualified name) on the side of v: one of the
        changed files in that side's namespace; another of the old program in the old program's (sc_base_<commit>), of
        the program that stands as it is."""
        if v.mine(node):
            return f"::{self.ns[v.side]}::{q}"
        if v.side == "old":
            return "::" + self.m.base_ns + q[len(self.m.namespace):]
        return f"::{q}"

    def side_records(self, v):
        """The classes, enums and types of the game on the side of v: {name as spelled - unqualified: its name in the test}
        (the changed files' first)."""
        names = {}
        for table in (v.records, v.enums, v.aliases):
            for q, x in table.items():
                node = x[0] if isinstance(x, tuple) else x
                if v.mine(node):
                    names[q.split("::")[-1]] = self.name_of(v, q, node)
        for table in (v.records, v.enums, v.aliases):
            for q, x in table.items():
                node = x[0] if isinstance(x, tuple) else x
                if os.path.abspath(node.get("_file") or "?").startswith(v.root + os.sep) and not q.endswith(")"):
                    names.setdefault(q.split("::")[-1], self.name_of(v, q, node))
        return names

    def spell(self, t, v):
        """A type as the source spells it, as the test names it: the classes of the changed files in sc_<side>."""
        names = self.side_records(v)
        def rep(m):
            chain, name = m.group(1) or "", m.group(2)
            if name in names and not chain.startswith("std::") and not chain.startswith("kern::"):
                return names[name]
            return m.group(0)
        t = re.sub(r"\(anonymous namespace\)::", "", t)
        return re.sub(r"(?<![\w:])((?:\w+::)*)(\w+)\b(?!\s*::)", rep, t)

    def write(self):
        planned = self.plan()
        if planned[0] is None:
            return planned
        pairs, tested, covered = planned
        m = self.m
        self.check_fields(tested)
        text = self.unit(tested)
        os.makedirs(self.out_dir, exist_ok=True)
        path = os.path.join(self.out_dir, f"test_{m.stem}_lowered.cpp")
        open(path, "w", encoding="latin-1").write(text)   # (the bytes of the sources as they are: see Version.text)
        report = self.report(pairs, tested, covered)
        for q_old, _ in self.implicit_inits():
            report.append(f"    compared: {q_old.split('::')[-1]}() - the compiler's constructor (the values its fields are written with changed)")
        for o, n, actor, _ in m.bindings:
            report.append(f"    the binding: {o.label()}  ->  {n.label()}  (the actor {actor}: every call compared runs through it)")
        for o in m.binding_gets:
            report.append(f"    gives the binding: {o.label()}  (checked: the Blueprint it was bound to)")
        return path, tested, covered, report

    @staticmethod
    def report(pairs, tested, covered):
        report = []
        for o, n, e in tested:
            report.append(f"    compared: {o.label()}  ->  {n.label()}" + (f"  (the state: {', '.join(f'{f} = {v}' for x in e.values() for f, v in x.items())})" if e else ""))
        for o, n, e in pairs:
            if (o, n, e) not in tested:
                report.append(f"    the same: {o.label()}")
        for label, through in covered.items():
            report.append(f"    gone, compared through its callers: {label}  <-  {', '.join(through)}")
        return report

    def plan(self):
        """(pairs, the changed ones, the gone ones with the callers they are compared through) - or, where there is
        nothing to compare, (None, tested, covered, report)."""
        m = self.m
        pairs, alone = m.pairs()
        tested = [(o, n, e) for o, n, e in pairs if m.changed(o, n, e)]
        # (a function only its file knows that is handed, gives or belongs to a class of the file whose fields changed -
        # a reader's cursor that became an index: its values cannot be carried; it is compared through its callers)
        reshaped = set()
        for q, (rec, _) in m.old.records.items():
            if not m.old.mine(rec):
                continue
            other = next((r for nq, (r, _) in m.new.records.items() if nq.split("::")[-1] == q.split("::")[-1] and m.new.mine(r)), None)
            if other is not None and [c.get("name") for c in kids(rec) if c.get("kind") == "FieldDecl"] != [c.get("name") for c in kids(other) if c.get("kind") == "FieldDecl"]:
                reshaped.add(q.split("::")[-1])
        def over_reshaped(f):
            toks = set(re.findall(r"[A-Za-z_]\w*", " ".join([t for _, t, _ in f.params] + [f.ret, f.owner or ""])))
            return bool(toks & reshaped)
        demoted = [(o, n, e) for o, n, e in tested if (o.local or o.owner and self.local_class(o.owner)) and over_reshaped(o)]
        tested = [x for x in tested if x not in demoted]
        pairs = [x for x in pairs if x not in demoted]
        alone = alone + [o for o, _, _ in demoted]
        report = []
        # (a binding function is run by the test on both sides - what it calls is compared through it, as through a pair)
        tested_ids = {o.node["id"] for o, _, _ in tested} | {o.node["id"] for o, _, _, _ in m.bindings}
        callers = m.old.callers()
        by_id = {f.node["id"]: f for f in m.old.functions}
        # a function of the old that is gone: one only its file knew, compared through every function that called it
        covered = {}
        for o in alone:
            if not o.local:
                raise Stop(f"{o.label()} of {os.path.basename(m.src)} has no counterpart in the new text (and others than its own file may call it)")
            seen, todo, through = set(), [o.node["id"]], set()
            while todo:
                i = todo.pop()
                if i in seen:
                    continue
                seen.add(i)
                for c in callers.get(i, ()):
                    if c in tested_ids:
                        through.add(by_id[c].label())
                    elif by_id[c] in alone:
                        todo.append(c)
                    else:
                        raise Stop(f"{o.label()} is gone, and {by_id[c].label()}, which calls it, is not compared (it did not change)")
            if not through:
                raise Stop(f"{o.label()} is gone, and nothing compared calls it")
            covered[o.label()] = sorted(through)
        # the new's file state must be the old's
        new_state = {q.split("::")[-1]: q for q, _ in m.new.state()}
        old_state = {q.split("::")[-1]: q for q, _ in m.old.state()}
        # (what the new binding function fills - a table of the Blueprint the new keeps - is the binding's: the test
        # binds before it compares, and leaves it as the binding left it)
        bound = set()
        for _, nb, _, _ in m.bindings:
            for x in walk(nb.body):
                if x.get("kind") == "DeclRefExpr":
                    bound.add(((x.get("referencedDecl") or {}).get("name")))
        for name in new_state:
            if name not in old_state and name not in bound:
                raise Stop(f"the new text keeps a variable of the file the old did not have: {name}")
        users = m.old.users()
        # (a variable of the file that became a field of a class of the file: `g_laws` -> `TrafficRules::laws_` - by its
        # name without the prefix g_, with or without the trailing _ of a member)
        self.moved = {}
        for q, _ in m.old.state():
            name = q.split("::")[-1]
            if name in new_state:
                continue
            base = name[2:] if name.startswith("g_") else name
            for nq, (rec, _) in m.new.records.items():
                if not m.new.mine(rec):
                    continue
                fields = [c.get("name") for c in kids(rec) if c.get("kind") == "FieldDecl"]
                old_rec = next((r for oq, (r, _) in m.old.records.items() if oq.split("::")[-1] == nq.split("::")[-1] and m.old.mine(r)), None)
                old_fields = [c.get("name") for c in kids(old_rec) if c.get("kind") == "FieldDecl"] if old_rec else []
                for f in (base, base + "_"):
                    if f in fields and f not in old_fields:
                        self.moved[q] = (nq.split("::")[-1], f)
        for o, n, extra in tested:
            mapped = {v for e in extra.values() for v in e.values()}
            for var in users.get(o.node["id"], ()):
                if var.split("::")[-1] not in new_state and var not in mapped and not (n.method and not n.static and self.moved.get(var, ("",))[0] == (n.owner or "").split("::")[-1]):
                    raise Stop(f"{o.label()} uses the variable {var} of the file, which the new {n.label()} neither has nor is handed")
        if not tested and not covered:
            # (every function the old had stands in the new as it was - the new only has more: nothing to compare)
            return None, tested, covered, [f"    the same: {o.label()}" for o, _, _ in pairs]
        return pairs, tested, covered

    def local_class(self, owner):
        """Whether a class of the old text is one only its file knows (in a namespace without a name)."""
        rec = self.m.old.records.get(owner)
        return rec is not None and "(anonymous)" in rec[1]

    def check_fields(self, tested):
        """A class of the old and the new of one name that the test carries has the same fields, in the same order (carried
        by name). (One that only the functions of the file use among themselves - a cursor of a reader - is compared
        through what they give.)"""
        m = self.m
        carried = {t for t, _ in self.reached(m.old, tested)}
        self.engine_enums = set()
        for q, (rec, _) in m.old.records.items():
            if not m.old.mine(rec) or self.name_of(m.old, q, rec) not in carried:
                continue
            name = q.split("::")[-1]
            # (the class of the same full name; where the new has none such, the one of the same name at another place -
            # two classes each call an inner one `Snap`: each is compared with its own)
            other = m.new.records.get(q, (None, None))[0]
            if other is None or not m.new.mine(other):
                other = next((r for nq, (r, _) in m.new.records.items() if nq.split("::")[-1] == name and m.new.mine(r) and nq not in m.old.records), None)
            if other is None:
                continue
            a = [c.get("name") for c in kids(rec) if c.get("kind") == "FieldDecl"]
            b = [c.get("name") for c in kids(other) if c.get("kind") == "FieldDecl" and (name, c.get("name")) not in set(self.moved.values())]
            if a != b:
                raise Stop(f"the class {name} has other fields in the new text ({', '.join(a)} / {', '.join(b)}): the test cannot carry its values")

    def reached(self, v, tested):
        """The classes the test draws, carries or compares on the side of v: those of the changed files, and every one a
        type of what is compared names - a parameter, what it returns, the object, a variable of the file -, and in turn
        the types of their fields; of the game and of the engine (whose fields are open): [(its name in the test, its
        fields)]. A class with a bit field or a reference among its fields is left to what source_diff.h makes of it."""
        m = self.m
        by_name = {}
        for q, (rec, _) in v.records.items():
            if rec.get("tagUsed") == "union" or not (v.mine(rec) or os.path.abspath(rec.get("_file") or "?").startswith(v.root + os.sep)):
                continue
            by_name.setdefault(q.split("::")[-1], []).append((q, rec, False))
        engine = m.engine_records()
        def ftypes(rec):
            return [t for c in kids(rec) if c.get("kind") == "FieldDecl" for t in ((c.get("type") or {}).get("qualType", ""), (c.get("type") or {}).get("desugaredQualType", ""))]
        types = []
        for o, n, _ in tested:
            f = o if v.side == "old" else n
            types += [t for _, t, d in f.params for t in (t, d)] + [f.ret] + ([f.owner] if f.owner else [])
        types += [t for _, node in v.state() for t in ((node.get("type") or {}).get("qualType", ""), C.qtype(node))]
        types += [q for pair in self.implicit_inits() for q in pair]
        out, seen, todo = [], set(), list(types)
        while todo:
            t = todo.pop()
            found = []
            for tok in re.findall(r"[A-Za-z_]\w*", t or ""):
                found += by_name.get(tok, [])
            for qn in re.findall(r"(?<![\w:])(kern::(?:\w+::)*\w+)", t or ""):
                if qn in engine:
                    found.append((qn, engine[qn], True))
                if qn in m._engine_enums:
                    self.engine_enums.add(qn)
            for tok in re.findall(r"(?<![\w:])([A-Z]\w*)", t or ""):
                if "kern::" + tok in engine and tok not in by_name:
                    found.append(("kern::" + tok, engine["kern::" + tok], True))
                if "kern::" + tok in m._engine_enums and tok not in by_name:
                    self.engine_enums.add("kern::" + tok)
            for q, rec, eng in found:
                if (q, eng) in seen:
                    continue
                seen.add((q, eng))
                if eng and (not m.open_fields(rec) or rec.get("access") in ("private", "protected")):
                    continue   # (a class of the engine that keeps what it holds: the test leaves it as it is)
                todo += ftypes(rec)
                fields = [c for c in kids(rec) if c.get("kind") == "FieldDecl"]
                if not fields or any(c.get("isBitfield") or re.search(r"&\s*$", (c.get("type") or {}).get("qualType", "")) for c in fields):
                    continue
                out.append(("::" + q if eng else self.name_of(v, q, rec), [c.get("name") for c in fields]))
        return out

    # ---- the text of the test ----
    def unit(self, tested):
        m = self.m
        hoisted = {}
        old_edits, new_edits = m.old.guards(), m.new.guards()
        pieces = []
        old_game = {}
        for v, edits in ((m.old, old_edits), (m.new, new_edits)):
            body = ""
            for f in ([v.header] if v.header else []) + [v.cpp]:
                body += f"// ---- {os.path.basename(f)}, {'as it stood before' if v.side == 'old' else 'as it stands'} ----\n" + pasted(
                    v, f, edits, m.old_include_dirs if v.side == "old" else m.include_dirs, hoisted, old_game) + "\n"
            pieces.append((v.side, body))
        actors = sorted({a for _, a in m.old.actors()} | {a for _, a in m.new.actors()})
        out = [f"// Generated by tools/source_diff.py from {os.path.basename(m.src)} - do not edit. The proof of its lowering into the C++ subset",
               "// (BLUEPRINTS.md, \"Absenkung\"): the old text and the new, each in a namespace of its own, compiled as the game compiles them;",
               "// every function the lowering changed is called in both with the same drawn state and arguments, and what they give and leave",
               "// must be the same, bit for bit (../source_diff.h). The old text sees the game as it stood at the base: the old program,",
               f"// built by run_tests.sh in the namespace {m.base_ns} - the engine and the level's Blueprints as they stand.",
               f"// base: {m.base_commit}"]
        # (the game's headers as the test reads them with their classes open, as the pasted texts are: the test names
        # every field - the old program's with its namespace renamed, as run_tests.sh builds it)
        game = [h for h in hoisted if h.startswith('#include "') and os.path.exists(os.path.join(m.src_dir, h[10:-1]))]
        out += ['#include "../source_diff.h"'] + [h for h in hoisted if h not in game] + ["#define private public", "#define protected public"]
        if any(not any(f.name == n.get("name") and tuple(f.scope) == tuple(sc) for f in m.old.functions) for n, sc in m.old.declared) and m.old.header:
            # (what the old text's header declares and another file of the old program defines: the old program's header
            # names it there)
            old_game.setdefault(os.path.relpath(m.old.header, m.old.root), None)
        if old_game:
            out += [f"#define {m.namespace} {m.base_ns}"] + [f'#include "{m.base_ns}/{h}"' for h in old_game] + [f"#undef {m.namespace}"]
        out += game + [""]
        # (each namespace of the game as each side's own one: what that side does not hold itself is its program's)
        new_chains = sorted({tuple(s) for s, _ in m.new.outside() if "(anonymous)" not in s and len(s) > 1})
        old_chains = sorted({tuple(s) for s, _ in m.old.outside() if "(anonymous)" not in s and len(s) > 1})
        out += [f"namespace {m.base_ns} {{}}"] + [f"namespace {self.ns['old']}::{'::'.join(c)} {{ using namespace ::{'::'.join((m.base_ns,) + c[1:])}; }}" for c in [(m.namespace,)] + old_chains]
        out += [f"namespace {self.ns['new']}::{'::'.join(c)} {{ using namespace ::{'::'.join(c)}; }}" for c in [(m.namespace,)] + new_chains] + [""]
        out += ["#pragma GCC diagnostic push", '#pragma GCC diagnostic ignored "-Wunused-function"', '#pragma GCC diagnostic ignored "-Wunused-variable"',
                '#pragma GCC diagnostic ignored "-Wunused-const-variable"']
        for side, body in pieces:
            out += [f"namespace {self.ns[side]} {{", body, f"}}  // namespace {self.ns[side]}", ""]
        out += ["#undef private", "#undef protected", "#pragma GCC diagnostic pop", ""]
        # the fields of the classes the test draws, carries and compares, and the values of the enums
        out.append("namespace sc::lowered {")
        written = set()
        for v in (m.old, m.new):
            for t, fields in self.reached(v, tested):
                if v.side == "new":
                    fields = [f for f in fields if (t.split("::")[-1], f) not in set(self.moved.values())]
                if t in written:
                    continue
                written.add(t)
                tie = ", ".join(f"x.{f}" for f in fields)
                out.append(f"template <> struct Fields<{t}> {{ static auto of({t}& x) {{ return std::tie({tie}); }} static auto of(const {t}& x) {{ return std::tie({tie}); }} }};")
        enums = {}
        for v in (m.old, m.new):
            for q, e in v.enums.items():
                if q.endswith(")") or not e.get("name") or not (v.mine(e) or os.path.abspath(e.get("_file") or "?").startswith(v.root + os.sep)):
                    continue
                if not v.mine(e) and any(q == h or q.startswith(h + "::") for h in v.hidden):
                    continue   # (an enum a class of another file keeps to itself: the test neither names nor draws it)
                t = self.name_of(v, q, e)
                values = [c.get("name") for c in kids(e) if c.get("kind") == "EnumConstantDecl"]
                if values and t not in enums:
                    enums[t] = values
        for q in sorted(self.engine_enums):
            e = m._engine_enums[q]
            values = [c.get("name") for c in kids(e) if c.get("kind") == "EnumConstantDecl"]
            if values and e.get("access") not in ("private", "protected"):
                enums.setdefault("::" + q, values)
        for t, values in enums.items():
            out.append(f"template <> struct Values<{t}> {{ static constexpr long long all[] = {{{', '.join(f'static_cast<long long>({t}::{x})' for x in values)}}}; }};")
        out += ["}  // namespace sc::lowered", ""]
        out.append(f"namespace {m.namespace} {{")
        out.append(f"KERN_TEST(lowered_{re.sub(r'\W', '_', m.stem)}_cpp) {{")
        actors = sorted(set(actors) | {a for _, _, a, _ in m.bindings})
        if actors or any(self.logic_param(t) for o, n, _ in tested for _, t, _ in o.params + n.params):
            out.append(f"  ::sc::lowered::World world({{{', '.join(C.text_literal(a) for a in actors)}}});")
            for v in (m.old, m.new):
                for q, _ in v.actors():
                    out.append(f"  ::{self.ns[v.side]}::{q}.bind(world.logic({'true' if v.side == 'old' else 'false'}));")
            # (the file bound as each program binds it: the old by the running Blueprint of the actor, the new by the
            # logic - what the two binding functions give must be the same)
            for b, (o, n, actor, extra) in enumerate(m.bindings):
                ocall = f"{self.call(o, 'old', f'world.instance(true, {C.text_literal(actor)})')}"
                held = []
                for j in extra:
                    out.append(f"  ::sc::lowered::Arg<{self.spell(n.params[j][1], m.new)}> b{b}_{j};")
                    held.append(f"b{b}_{j}.get()")
                ncall = f"{self.call(n, 'new', ', '.join(['world.logic(false)'] + held))}"
                if plain(o.ret) != "void" and plain(n.ret) != "void":
                    out.append(f'  {{ const auto o_bound = {ocall}; const auto n_bound = {ncall}; KERN_CHECK(::sc::lowered::same(o_bound, n_bound), "{o.label()} and {n.label()} give other results"); }}')
                else:
                    out += [f"  {ocall};", f"  {ncall};"]
                # (what the binding put into the file's state, the same in both)
                for j, mapping in extra.items():
                    for fname, var in mapping.items():
                        out.append(f'  KERN_CHECK(::sc::lowered::same(::{self.ns["old"]}::{var}, b{b}_{j}.v.{fname}), "{n.label()}: {fname} is not what {o.label()} left in {var.split("::")[-1]}");')
            for o in m.binding_gets:
                actor = {a for _, _, a, _ in m.bindings}
                if len(actor) != 1:
                    raise Stop(f"{o.label()} gives the Blueprint the file was bound to, and the file binds {len(actor)} actors")
                out.append(f'  KERN_CHECK({self.call(o, "old", "")} == world.instance(true, {C.text_literal(actor.pop())}), "{o.label()}: not the Blueprint it was bound to");')
            world = True
        else:
            world = False
        for o, n, extra in tested:
            out += self.case(o, n, extra, world)
        for q_old, q_new in self.implicit_inits():
            out += self.init_case(q_old, q_new)
        if world:
            for v in (m.old, m.new):
                for q, _ in v.actors():
                    out.append(f"  ::{self.ns[v.side]}::{q}.bind(kern::test::no_logic());   // (the test's logic is gone with it)")
            for o, _, _, _ in m.bindings:
                out.append(f"  {self.call(o, 'old', 'nullptr')};   // (the test's logic is gone with it)")
        out += ["}", f"}}  // namespace {m.namespace}", ""]
        return "\n".join(out)

    def implicit_inits(self):
        """The classes of the changed files that the compiler makes with the values their fields are written with, where
        those changed (`f32 hp = kHeliHp` -> `f32 hp = 0`): the compiler's constructor of the class is a function that
        changed - [(old class, new class)] of those that have one without parameters and no written one."""
        m = self.m
        out = []
        for q, (rec, _) in m.old.records.items():
            if not m.old.mine(rec) or rec.get("tagUsed") == "union":
                continue
            other = next(((nq, r) for nq, (r, _) in m.new.records.items() if nq == q and m.new.mine(r)), None)
            if other is None:
                continue
            # (what each field is written with - its initializer's text; a field written without one gives no value)
            def inits(v, r):
                out = []
                for c in kids(r):
                    if c.get("kind") != "FieldDecl":
                        continue
                    init = next((x for x in kids(c) if x.get("kind", "").endswith("Expr") or x.get("kind", "").endswith("Literal")), None) if c.get("hasInClassInitializer") else None
                    out.append(norm(v.text(os.path.abspath(init["_file"]))[init["_at"][0]:init["_at"][1]]) if init and init.get("_at", (None, None))[1] else None)
                return out
            ofields, nfields = inits(m.old, rec), inits(m.new, other[1])
            if all(x is None for x in ofields + nfields):
                continue
            written = any(c.get("kind") == "CXXConstructorDecl" and not c.get("isImplicit") for r in (rec, other[1]) for c in kids(r))
            if ofields != nfields and not written and [c.get("name") for c in kids(rec) if c.get("kind") == "FieldDecl"] == [c.get("name") for c in kids(other[1]) if c.get("kind") == "FieldDecl"]:
                out.append((q, other[0]))
        # (one that holds a class whose fields changed - a list of a reader's entries that became others - cannot be
        # carried: what its constructor makes is compared where the file's functions make it)
        def names(q):
            r = m.old.records.get(q)
            return [c.get("name") for c in kids(r[0]) if c.get("kind") == "FieldDecl"] if r else None
        def reshaped(q, depth=0):
            r = m.old.records.get(q)
            if r is None or depth > 6:
                return False
            nr = m.new.records.get(q)
            if nr is None or names(q) != [c.get("name") for c in kids(nr[0]) if c.get("kind") == "FieldDecl"]:
                return True
            toks = set(re.findall(r"[A-Za-z_]\w*", " ".join((c.get("type") or {}).get("qualType", "") for c in kids(r[0]) if c.get("kind") == "FieldDecl")))
            return any(reshaped(oq, depth + 1) for oq in m.old.records if oq.split("::")[-1] in toks and m.old.mine(m.old.records[oq][0]) and oq != q)
        return [x for x in out if not reshaped(x[0])]

    def init_case(self, q_old, q_new):
        """The comparison of the compiler's constructors of a class: what each makes, bit for bit."""
        label = f"{q_old.split('::')[-1]}::{q_old.split('::')[-1]}() (the compiler's, with the values its fields are written with)"
        return [f"  {{  // {label}", f"    ::sc::lowered::Tally tally({C.text_literal(label)});", "    for (int call = 0; tally.more(call); ++call) {",
                f"      std::optional<::{self.ns['old']}::{q_old}> o_obj;", f"      std::optional<::{self.ns['new']}::{q_new}> n_obj;",
                "      long old_asks = 0, new_asks = 0;", "      const bool old_defined = ::sc::lowered::run([&] { o_obj.emplace(); }, old_asks);",
                "      const bool new_defined = ::sc::lowered::run([&] { n_obj.emplace(); }, new_asks);", "      ::sc::lowered::Differs differs;",
                '      differs.check("the object it makes", o_obj, n_obj);', "      tally.take(old_defined && new_defined, differs.text, old_asks, new_asks, call);", "    }", "  }"]

    @staticmethod
    def logic_param(t):
        return bool(re.match(r"^(kern::)?logic::Runtime\s*&$", plain(t)))

    def call(self, f, side, args):
        """The call of f on its side (sc_<side>), with these arguments."""
        targs = ""
        if f.template:
            targs = "<" + ", ".join(f.template[1].values()) + ">"
        if f.kind == "CXXConstructorDecl":
            return f"o_obj.emplace({args})"
        if f.method and not f.static:
            return f"self.{'template ' if targs else ''}{f.name}{targs}({args})"
        q = f"::{self.ns[side]}::{f.cpp_scope}::{f.name}"
        return f"{q}{targs}({args})"

    def case(self, o, n, extra, world):
        m = self.m
        lines = [f"  {{  // {o.label()}  ->  {n.label()}", f"    ::sc::lowered::Tally tally({C.text_literal(o.label())});", "    ::kern::converted::Draw draw;",
                 "    for (int call = 0; tally.more(call); ++call) {"]
        body = []
        old_state = m.old.state()
        new_state = {q.split("::")[-1]: q for q, _ in m.new.state()}
        # the variables of the file: drawn on the old side, carried to their counterparts
        mapped_vars = {v for e in extra.values() for v in e.values()}
        for q, node in old_state:
            name = q.split("::")[-1]
            if name in new_state:
                body.append(f"::sc::lowered::draw_pair(draw, ::{self.ns['old']}::{q}, ::{self.ns['new']}::{new_state[name]});")
            elif q not in mapped_vars:
                body.append(f"::sc::lowered::draw(draw, ::{self.ns['old']}::{q});")
        # the object
        if o.method and not o.static and o.kind != "CXXConstructorDecl":
            ot, nt = f"::{self.ns['old']}::{o.owner}", f"::{self.ns['new']}::{n.owner}"
            # (as the converted tests make an object: half the time as its constructor makes it - its parts fit together -,
            # half the time field by field)
            body += [f"{ot} o_self{{}};", "if (draw.flag()) ::sc::lowered::draw(draw, o_self);", f"{nt} n_self{{}};", "::sc::lowered::carry(o_self, n_self);"]
            for var, (cls, field) in self.moved.items():
                if cls == n.owner.split("::")[-1]:
                    body.append(f"::sc::lowered::carry(::{self.ns['old']}::{var}, n_self.{field});   // (the variable of the file, now a field)")
        if o.static and n.method and not n.static and n.kind != "CXXConstructorDecl":
            # (a function of the class that became one of its objects: the object holds what the file held - the
            # variables that became its fields; the rest of it as drawn)
            nt = f"::{self.ns['new']}::{n.owner}"
            body += [f"{nt} n_self{{}};", "if (draw.flag()) ::sc::lowered::draw(draw, n_self);"]
            for var, (cls, field) in self.moved.items():
                if cls == n.owner.split("::")[-1]:
                    body.append(f"::sc::lowered::carry(::{self.ns['old']}::{var}, n_self.{field});   // (the variable of the file, now a field)")
        if o.kind == "CXXConstructorDecl":
            body += [f"std::optional<::{self.ns['old']}::{o.owner}> o_obj;", f"std::optional<::{self.ns['new']}::{n.owner}> n_obj;"]
        # the arguments
        oargs, nargs, compares = [], [], []
        k = 0
        plan = m.plans.get(o.node["id"])
        returned = None   # (the old's place of what it gives, which the new returns)
        for step, olds, news in (plan or []):
            j = news[0] if news else None
            if step == "state":
                pn, pt, pd = n.params[j]
                nt = self.spell(pt if not n.template else pd, m.new)
                body.append(f"::sc::lowered::Arg<{nt}> n{j};")
                for fname, var in extra[j].items():
                    body.append(f"::sc::lowered::draw_pair(draw, ::{self.ns['old']}::{var}, n{j}.v.{fname});")
                nargs.append(f"n{j}.get()")
                continue
            i = olds[0]
            on, ot, od = o.params[i]
            ot = self.spell(ot if not o.template else od, m.old)
            writes = not re.match(r"^\s*const\b", ot)
            if step == "returned":
                # (the place the old writes what it gives: it starts as the value of its type)
                el = re.sub(r"\s*[&*]\s*$", "", ot).strip()
                arr = re.fullmatch(r"\s*std::array<.+,\s*(\d+)>\s*", canon(m.new_type(n.ret)))
                held = f"std::array<{el}, {arr.group(1)}>" if arr and ot.strip().endswith("*") else el
                body.append(f"::sc::lowered::Arg<{held}> o{i};")
                if ot.strip().endswith("*"):
                    body.append(f"const ::sc::lowered::Placed o{i}_placed(&o{i}.v, 1);" if not arr else f"const ::sc::lowered::Placed o{i}_placed(o{i}.v.data(), o{i}.v.size());")
                    oargs.append(f"o{i}.v.data()" if arr else f"&o{i}.v")
                else:
                    oargs.append(f"o{i}.get()")
                returned = f"o{i}.v"
                continue
            pn, pt, pd = n.params[j]
            nt = self.spell(pt if not n.template else pd, m.new)
            if step == "same":
                if self.logic_param(ot):
                    oargs.append("world.logic(true)")
                    nargs.append("world.logic(false)")
                    continue
                body += [f"::sc::lowered::Arg<{ot}> o{i};", f"::sc::lowered::Arg<{nt}> n{j};", f"::sc::lowered::draw_pair(draw, o{i}, n{j});"]
                oargs.append(f"o{i}.get()")
                if re.search(r"&\s*$", ot) and writes:
                    compares.append((f"its parameter {on}", f"o{i}", f"n{j}"))
            elif step == "place":
                # (the old is handed the place of the value the new is handed by reference)
                body += [f"::sc::lowered::Arg<{re.sub(r'\*\s*$', '', ot).strip()}> o{i};", f"::sc::lowered::Arg<{nt}> n{j};", f"::sc::lowered::draw_pair(draw, o{i}, n{j});",
                         f"const ::sc::lowered::Placed o{i}_placed(&o{i}.v, 1);"]
                oargs.append(f"&o{i}.v")
                if writes:
                    compares.append((f"its parameter {on}", f"o{i}", f"n{j}"))
            elif step == "entries":
                # (the old is handed the place of the first entry of the list the new is handed - and its length)
                el = re.sub(r"^\s*const\s+", "", re.sub(r"\*\s*$", "", ot)).strip()
                arr = re.search(r"std::array<.+,\s*(\d+)>", canon(m.new_type(re.sub(r"^\s*const\s+|\s*&\s*$", "", pt))))
                held = f"std::array<{el}, {arr.group(1)}>" if arr else f"std::vector<{el}>"
                body += [f"::sc::lowered::Arg<{held}> o{i};", f"::sc::lowered::Arg<{nt}> n{j};", f"::sc::lowered::draw_pair(draw, o{i}, n{j});",
                         f"const ::sc::lowered::Placed o{i}_placed(o{i}.v.data(), o{i}.v.size());"]
                oargs.append(f"o{i}.v.data()")
                if len(olds) > 1:
                    oargs.append(f"static_cast<{self.spell(o.params[olds[1]][1], m.old)}>(o{i}.v.size())")
                if writes:
                    compares.append((f"its parameter {on}", f"o{i}", f"n{j}"))
            elif step == "spread":
                # (the old is handed the place of entries it reads one by one; the new each entry)
                el = re.sub(r"^\s*const\s+", "", re.sub(r"\*\s*$", "", ot)).strip()
                body += [f"::sc::lowered::Arg<std::array<{el}, {len(news)}>> o{i};", f"::sc::lowered::draw(draw, o{i});",
                         f"const ::sc::lowered::Placed o{i}_placed(o{i}.v.data(), o{i}.v.size());"]
                oargs.append(f"o{i}.v.data()")
                for k, jj in enumerate(news):
                    body += [f"::sc::lowered::Arg<{self.spell(n.params[jj][1], m.new)}> n{jj};", f"::sc::lowered::carry(o{i}.v[{k}], n{jj}.v);"]
                    if jj != j:
                        nargs.append(f"n{jj}.get()")
            elif step == "value":
                # (the struct of the Blueprint drawn on the new side; the old is handed the value of the Blueprint it is)
                body += [f"::sc::lowered::Arg<{nt}> n{j};", f"::sc::lowered::draw(draw, n{j});", f"const ::kern::logic::Value o{i} = ::sc::lowered::as_value(n{j}.v);"]
                oargs.append(f"o{i}")
            elif step == "given":
                # (the old is handed a function that gives the value the new is handed)
                body += [f"::sc::lowered::Arg<{nt}> n{j};", f"::sc::lowered::draw(draw, n{j});"]
                oargs.append(f"[&](auto&&...) {{ return n{j}.v; }}")
            elif step == "index":
                # (the old is handed an entry of a list of its object, the new the entry's index)
                field = m.entry_list(o, m.referee(o.params[i][1]))
                body += [f"if (o_self.{field}.empty()) {{ tally.take(false, \"\", 0, 0, call); continue; }}",
                         f"const u32 at{i} = draw.below(static_cast<u32>(o_self.{field}.size()));", f"::sc::lowered::Arg<{nt}> n{j};", f"n{j}.v = static_cast<std::remove_cvref_t<decltype(n{j}.v)>>(at{i});"]
                oargs.append(f"o_self.{field}[at{i}]")
            nargs.append(f"n{j}.get()")
        for j, (pn, pt, pd) in enumerate([] if plan is not None else n.params):
            nt = self.spell(pt if not n.template else pd, m.new)
            if j in extra:
                body.append(f"::sc::lowered::Arg<{nt}> n{j};")
                for fname, var in extra[j].items():
                    body.append(f"::sc::lowered::draw_pair(draw, ::{self.ns['old']}::{var}, n{j}.v.{fname});")
                nargs.append(f"n{j}.get()")
                continue
            on, ot, od = o.params[k]
            ot = self.spell(ot if not o.template else od, m.old)
            if self.logic_param(ot):
                oargs.append("world.logic(true)")
                nargs.append("world.logic(false)")
            else:
                body += [f"::sc::lowered::Arg<{ot}> o{k};", f"::sc::lowered::Arg<{nt}> n{j};", f"::sc::lowered::draw_pair(draw, o{k}, n{j});"]
                oargs.append(f"o{k}.get()")
                nargs.append(f"n{j}.get()")
                if re.search(r"&\s*$", ot) and not re.match(r"^const\b", ot.strip()):
                    compares.append((f"its parameter {on}", f"o{k}", f"n{j}"))   # (what it may change: a reference that is not const)
            k += 1
        # the results
        ret = o.ret and plain(o.ret) != "void" and o.kind != "CXXConstructorDecl"
        def placed(callee, who):
            return callee.replace("self.", f"{who}_self.", 1) if callee.startswith("self.") else callee.replace("o_obj.", f"{who}_obj.", 1)
        ocall = placed(self.call(o, "old", ", ".join(oargs)), "o")
        ncall = placed(self.call(n, "new", ", ".join(nargs)), "n")
        if o.kind == "CXXConstructorDecl" and n.kind != "CXXConstructorDecl":
            ncall = f"n_obj.emplace({ncall})"   # (what the function that took the constructor's place makes)
        rets = {"o": ret, "n": n.ret and plain(n.ret) != "void" and n.kind != "CXXConstructorDecl" and (ret or returned is not None)}
        if rets["o"]:
            body.append(f"std::optional<std::remove_cvref_t<decltype({ocall})>> o_r;")
        if rets["n"]:
            body.append(f"std::optional<std::remove_cvref_t<decltype({ncall})>> n_r;")
        def side(c, who):
            return f"{who}_r.emplace({c});" if rets[who] else f"{c};"
        if world:
            body.append("world.before();")
        body += ["long old_asks = 0, new_asks = 0;", f"const bool old_defined = ::sc::lowered::run([&] {{ {side(ocall, 'o')} }}, old_asks);",
                 f"const bool new_defined = ::sc::lowered::run([&] {{ {side(ncall, 'n')} }}, new_asks);", "::sc::lowered::Differs differs;"]
        if ret and returned is None:
            body.append('differs.check("what it returns", o_r, n_r);')
        if returned is not None:
            body.append(f'differs.check("what it gives (the old in its last parameter, the new as what it returns)", {returned}, *n_r);')
        if o.method and not o.static and o.kind != "CXXConstructorDecl":
            body.append('differs.check("its object", o_self, n_self);')
        if n.method and not n.static and n.kind != "CXXConstructorDecl":
            for var, (cls, field) in self.moved.items():
                if cls == n.owner.split("::")[-1]:
                    body.append(f'differs.check("the variable {var.split("::")[-1]} (now the field {field})", ::{self.ns["old"]}::{var}, n_self.{field});')
        if o.kind == "CXXConstructorDecl":
            body.append('differs.check("the object it makes", o_obj, n_obj);')
        for what, a, b in compares:
            body.append(f"differs.check({C.text_literal(what)}, {a}, {b});")
        for q, node in old_state:
            name = q.split("::")[-1]
            if name in new_state:
                body.append(f"differs.check({C.text_literal('the variable ' + name)}, ::{self.ns['old']}::{q}, ::{self.ns['new']}::{new_state[name]});")
        for j, mapping in extra.items():
            for fname, var in mapping.items():
                body.append(f"differs.check({C.text_literal(f'the variable {var.split(chr(58))[-1]} (now {n.params[j][0]}.{fname})')}, ::{self.ns['old']}::{var}, n{j}.v.{fname});")
        if world:
            body.append('if (!world.same_after()) differs.text += std::string(differs.text.empty() ? "" : ", ") + "the Blueprints of the actors it calls";')
        body.append("tally.take(old_defined && new_defined, differs.text, old_asks, new_asks, call);")
        lines += ["      " + x for x in body]
        lines += ["    }", "  }"]
        return lines


# ---- the form "two builds" (UEBERGABE 4.2) ----
# Each side is the whole program in its state: the old tree - the game's sources as they stood at --base - and the
# new, as they stand. The same test (a driver) is compiled against each and linked into each, with the engine as it
# stands (the lowering rewrites the game's sources, not the engine) and the level as it stands; the old program writes
# what every compared function gave and left as bytes, the new compares (tests/source_diff.h, Builds). So a class of
# many files (Game: game.h and ~40 sources), helpers of one name in two texts and an old text that would not compile
# against the new headers are each in their own program. The test keeps the old tree's sources that differ from the
# new (tests/lowered/builds/<file>/old/...) and the names of those the old did not have (.../gone): run_tests.sh makes
# the old tree from them, with no git.

def spans_files(m):
    """Whether a class a changed method of the file belongs to has methods whose bodies stand in other files (a class
    of many files: its two texts cannot both be in one program)."""
    defined = {(f.owner, f.name) for f in m.new.functions}
    owners = {n.owner for o, n, e in m.pairs()[0] if n.method and m.changed(o, n, e)}
    for q, (rec, _) in m.new.records.items():
        if q not in owners:
            continue
        for c in kids(rec):
            if c.get("kind") not in ("CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl") or c.get("isImplicit"):
                continue
            if c.get("explicitlyDefaulted") or c.get("explicitlyDeleted") or c.get("pure"):
                continue
            if not any(x.get("kind") == "CompoundStmt" for x in kids(c)) and (q, c.get("name")) not in defined:
                return True
    return False


class BuildsModule(Module):
    """One source file of the program: its old text in the old tree (the sources of its folder as they stood at the
    base), its new text in the tree as it stands."""

    def __init__(self, src, base, flags, namespace, scratch):
        self.src = src
        self.base = subprocess.run(["git", "rev-parse", "--short=8", base], capture_output=True, text=True).stdout.strip() or base
        self.stem = os.path.splitext(os.path.basename(src))[0]
        header = os.path.splitext(src)[0] + ".h"
        self.header = header if os.path.exists(header) else None
        self.namespace = namespace
        top = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True, check=True).stdout.strip()
        self.srcdir = os.path.dirname(os.path.abspath(src))
        rel_dir = os.path.relpath(self.srcdir, top)
        # the old tree: the folder as it stands, with the files that differ from the base as they stood there
        r = subprocess.run(["git", "diff", "--no-renames", "--name-status", base, "--", rel_dir], capture_output=True, text=True, cwd=top)
        if r.returncode != 0:
            raise Stop(f"git does not know {base}: {r.stderr.strip()[:200]}")
        self.old_files, self.gone = {}, []
        for line in r.stdout.splitlines():
            st, path = line.split("\t", 1)
            name = os.path.relpath(path, rel_dir)
            if st.startswith("A"):
                self.gone.append(name)
            else:
                t = subprocess.run(["git", "show", f"{base}:{path}"], capture_output=True, cwd=top)
                if t.returncode != 0:
                    raise Stop(f"{path} is not in {base}")
                self.old_files[name] = t.stdout
        name = os.path.relpath(os.path.abspath(src), self.srcdir)
        if name in self.gone:
            raise Stop(f"{src} is not in {base}: a new file has no old text to compare with")
        # (beside it, linked, what stands beside the folder: a header of it names ../firmware/...)
        beside = os.path.join(scratch, "tree-" + self.stem)
        shutil.rmtree(beside, ignore_errors=True)
        os.makedirs(beside)
        parent = os.path.dirname(self.srcdir)
        for e in os.listdir(parent):
            if e != os.path.basename(self.srcdir):
                os.symlink(os.path.join(parent, e), os.path.join(beside, e))
        tree = os.path.join(beside, os.path.basename(self.srcdir))
        shutil.copytree(self.srcdir, tree)
        for g in self.gone:
            os.remove(os.path.join(tree, g))
        for f, data in self.old_files.items():
            os.makedirs(os.path.dirname(os.path.join(tree, f)), exist_ok=True)
            open(os.path.join(tree, f), "wb").write(data)
        self.tree = tree
        old_flags = [f"-I{tree}" if f.startswith("-I") and os.path.abspath(f[2:]) == self.srcdir else f for f in flags]
        self.include_dirs = [f[2:] for f in flags if f.startswith("-I")]
        old_header = os.path.join(tree, os.path.basename(header)) if self.header and os.path.exists(os.path.join(tree, os.path.basename(header))) else None
        self.old = Version(os.path.join(tree, name), old_header, old_flags, namespace, "old")
        self.new = Version(src, self.header, flags, namespace, "new")
        self.flags = flags
        self.engine = None

    def engine_records(self):
        """The engine's classes and enums (namespace kern), as the new text sees them."""
        if self.engine is None:
            self.engine = Version(self.src, self.header, self.flags, "kern", "new")
        return self.engine


class BuildsWriter(Writer):
    """The test of one module in the form "two builds": tests/lowered/builds/<file>/builds_<file>.cpp, with the old
    tree's sources that differ (old/) and the names of those it did not have (gone)."""

    def write(self):
        planned = self.plan()
        if planned[0] is None:
            return planned
        pairs, tested, covered = planned
        m = self.m
        for o, n, e in tested:
            if e or o.template or n.template or o.param_types() != n.param_types() or plain(o.ret) != plain(n.ret) or o.kind != n.kind:
                raise Stop(f"{o.label()} -> {n.label()}: the form \"two builds\" compares a function with its counterpart of the same signature - "
                           "one that changed its parameters (the state made explicit, a template made functions) is not compared in it yet")
            if n.local or o.local:
                raise Stop(f"{o.label()}: only its own file can call it - the form \"two builds\" calls what the program's other files can call")
        for label in covered:
            pass   # (a gone helper is compared through its callers, which are compared here)
        users = m.old.users()
        for o, n, e in tested:
            if users.get(o.node["id"]):
                raise Stop(f"{o.label()} uses the variables of the file ({', '.join(sorted(users[o.node['id']]))}): the form \"two builds\" does not reach them yet")
        if any(True for _ in m.old.actors()) or any(self.logic_param(t) for o, n, _ in tested for _, t, _ in o.params + n.params):
            raise Stop("the file binds Blueprints of actors: the form \"two builds\" does not run a logic for them yet")
        self.reached = {}       # qualified name -> (side "sc"/"kern", record) of the classes the test draws or writes
        self.reached_enums = {}
        roots = []
        for o, n, e in tested:
            if n.owner:
                roots.append(n.owner)
            roots += [t for _, t, _ in n.params] + [t for _, _, t in n.params] + [n.ret or ""]
        self.reach(roots)
        self.check_same()
        text = self.driver(tested)
        folder = os.path.join(self.out_dir, "builds", m.stem)
        shutil.rmtree(folder, ignore_errors=True)
        os.makedirs(os.path.join(folder, "old"), exist_ok=True)
        stale = os.path.join(self.out_dir, f"test_{m.stem}_lowered.cpp")
        if os.path.exists(stale):
            os.remove(stale)   # (the test of the file is in this form now)
        path = os.path.join(folder, f"builds_{m.stem}.cpp")
        open(path, "w", encoding="latin-1").write(text)
        for f, data in m.old_files.items():
            os.makedirs(os.path.dirname(os.path.join(folder, "old", f)), exist_ok=True)
            open(os.path.join(folder, "old", f), "wb").write(data)
        open(os.path.join(folder, "gone"), "w").write("".join(g + "\n" for g in m.gone))
        return path, tested, covered, self.report(pairs, tested, covered)

    # ---- the classes the test draws and writes ----
    @staticmethod
    def names_in(t):
        return re.findall(r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*", re.sub(r"\(anonymous namespace\)::", "", t or ""))

    def find(self, chain):
        """The classes and enums a name of a type may be: [(qualified name, side, node, is_enum)]."""
        m = self.m
        out = []
        tables = [("sc", m.new.records, False), ("sc", m.new.enums, True)]
        if chain.startswith("kern::"):
            e = m.engine_records()
            tables = [("kern", e.records, False), ("kern", e.enums, True)]
        elif chain.startswith("std::"):
            return []
        for side, table, is_enum in tables:
            for q, v in table.items():
                if q == chain or q.endswith("::" + chain):
                    out.append((q, side, v if is_enum else v[0], is_enum))
        return out

    @staticmethod
    def field_type(c):
        t = c.get("type") or {}
        return t.get("desugaredQualType") or t.get("qualType", "")

    def reach(self, roots):
        todo = list(roots)
        seen = set()
        while todo:
            t = todo.pop()
            for chain in self.names_in(t):
                if chain in seen:
                    continue
                seen.add(chain)
                for q, side, node, is_enum in self.find(chain):
                    if is_enum:
                        self.reached_enums.setdefault(q, (side, node))
                        continue
                    if q in self.reached:
                        continue
                    self.reached[q] = (side, node)
                    for c in kids(node):
                        if c.get("kind") == "FieldDecl":
                            todo += [(c.get("type") or {}).get("qualType", ""), self.field_type(c)]
                    for b in node.get("bases") or []:
                        todo.append((b.get("type") or {}).get("qualType", ""))

    def described(self, q):
        """Whether the test names the fields of the class q: a class of the program (in a header), or a plain struct of the engine."""
        side, node = self.reached[q]
        if node.get("tagUsed") == "union" or not node.get("completeDefinition"):
            return False
        if side == "kern":
            dd = node.get("definitionData") or {}
            fields = [c for c in kids(node) if c.get("kind") == "FieldDecl"]
            return bool(dd.get("isAggregate")) and not node.get("bases") and bool(fields) and node.get("tagUsed") != "class"
        return (node.get("_file") or "").endswith((".h", ".hpp"))

    def fields_of(self, q, depth=0):
        """[(the class that declares it, field name, its node)] - those of its bases first."""
        side, node = self.reached[q]
        out = []
        for b in node.get("bases") or []:
            bt = plain((b.get("type") or {}).get("desugaredQualType") or (b.get("type") or {}).get("qualType", ""))
            found = [x for x in self.find(bt) if not x[3]]
            if not found or found[0][0] not in self.reached or not self.described(found[0][0]):
                raise Stop(f"the class {q} is made on {bt}, whose fields the test does not know")
            out += self.fields_of(found[0][0], depth + 1)
        for c in kids(node):
            if c.get("kind") != "FieldDecl":
                continue
            if c.get("isBitfield"):
                raise Stop(f"the field {q}::{c.get('name')} is a field of bits: the form \"two builds\" does not reach it yet")
            if re.search(r"&\s*$", self.field_type(c)):
                raise Stop(f"the field {q}::{c.get('name')} is a reference: the form \"two builds\" does not reach it yet")
            if not c.get("name"):
                raise Stop(f"the class {q} has a field without a name")
            out.append((q, c["name"], c))
        return out

    def check_same(self):
        """Every class the test draws has the same fields, of the same types, in both trees - and every enum the same
        values: the same draws then give the same values on both sides."""
        m = self.m
        for q, (side, node) in self.reached.items():
            if side != "sc" or not self.described(q):
                continue
            old = m.old.records.get(q, (None, None))[0]
            if old is None:
                raise Stop(f"the class {q}, which the test draws, is not in the old tree")
            sig = lambda r: ([(c.get("name"), self.field_type(c)) for c in kids(r) if c.get("kind") == "FieldDecl"],
                             [plain((b.get("type") or {}).get("qualType", "")) for b in r.get("bases") or []])
            if sig(old) != sig(node):
                raise Stop(f"the class {q} has other fields in the old tree: the same draws would not give the two sides the same values")
        for q, (side, node) in self.reached_enums.items():
            if side != "sc":
                continue
            old = m.old.enums.get(q)
            if old is None or self.enum_values(old) != self.enum_values(node):
                raise Stop(f"the enum {q} has other values in the old tree")

    @staticmethod
    def enum_values(e):
        out, nxt = [], 0
        for c in kids(e):
            if c.get("kind") != "EnumConstantDecl":
                continue
            v = next((x.get("value") for x in walk(c) if x.get("kind") == "ConstantExpr" and "value" in x), None)
            nxt = int(v) if v is not None else nxt
            out.append((c.get("name"), nxt))
            nxt += 1
        return out

    # ---- the text of the test ----
    def driver(self, tested):
        m = self.m
        key = re.sub(r"\W", "_", m.stem)
        rob = f"::sc::lowered::builds_{key}"
        headers = set()
        for q, (side, node) in list(self.reached.items()) + list(self.reached_enums.items()):
            f = os.path.abspath(node.get("_file") or "")
            if side == "sc" and f.startswith(m.srcdir + os.sep) and f.endswith((".h", ".hpp")):
                headers.add(os.path.relpath(f, m.srcdir))
        if m.header:
            headers.add(os.path.relpath(os.path.abspath(m.header), m.srcdir))
        for h in headers:
            if h in m.gone:
                raise Stop(f"{h} is not in the old tree")
        out = [f"// Generated by tools/source_diff.py from {os.path.basename(m.src)} - do not edit. The proof of its lowering in the form",
               "// \"two builds\" (UEBERGABE 4.2, ../../../source_diff.h): this test is compiled into the program of the old tree (the sources",
               f"// as they stood at the base, old/ and gone beside it) and into the program as it stands; the old writes what every changed",
               "// function gives and leaves, call for call, the new draws the same calls and compares, bit for bit.",
               f"// base: {m.base}"]
        labels = {n.label() for _, n, _ in tested}
        for label, fields in sorted(getattr(self, "ends", {}).items()):
            if label not in labels:
                raise Stop(f"--ends {label}: no function of the test is called so")
            out.append(f"// ends on purpose (BLUEPRINTS.md, \"Bewusste Abweichungen\"): --ends '{label}={','.join(fields)}'")
        out += [f'#include "{h}"' for h in sorted(headers)]
        out += ['#include "../../../source_diff.h"', ""]
        # the fields, as C++ lets an explicit instantiation reach them (what a class keeps to itself too)
        out += [f"namespace sc::lowered::builds_{key} {{", "template <class Tag, auto M> struct Rob { friend constexpr auto got(Tag) { return M; } };"]
        ties, k = {}, 0
        for q in sorted(self.reached):
            if not self.described(q):
                continue
            side, node = self.reached[q]
            parts, said = [], []
            for owner, fname, c in self.fields_of(q):
                said.append(fname)
                if side == "kern":
                    parts.append(f"x.{fname}")
                    continue
                out.append(f"struct F{k} {{ friend constexpr auto got(F{k}); }};")
                out.append(f"template struct Rob<F{k}, &::{owner}::{fname}>;")
                parts.append(f"x.*got({rob}::F{k}{{}})")
                k += 1
            if parts:
                ties[q] = (parts, said)
        calls = {}
        for j, (o, n, e) in enumerate(tested):
            if n.method and not n.static and n.kind == "CXXMethodDecl" and self.hidden_method(n):
                out.append(f"struct M{j} {{ friend constexpr auto got(M{j}); }};")
                out.append(f"template struct Rob<M{j}, &::{n.owner}::{n.name}>;")
                calls[j] = f"((*self).*got({rob}::M{j}{{}}))"
        out += [f"}}  // namespace sc::lowered::builds_{key}", "", "namespace sc::lowered {"]
        for q, (parts, said) in ties.items():
            # (with the fields' names: where an object differs, the test names the fields that do - Builds::object)
            names = ", ".join(C.text_literal(x) for x in said)
            out.append(f"template <> struct Fields<::{q}> {{ template <class X> static auto of(X& x) {{ return std::tie({', '.join(parts)}); }} "
                       f"static constexpr const char* names[] = {{{names}}}; }};")
        for q, (side, node) in sorted(self.reached_enums.items()):
            values = self.enum_values(node)
            if values and node.get("name"):
                out.append(f"template <> struct Values<::{q}> {{ static constexpr long long all[] = {{{', '.join(str(v) for _, v in values)}}}; }};")
        out += ["}  // namespace sc::lowered", "", f"namespace {m.namespace} {{", f"KERN_TEST(lowered_{key}_cpp) {{"]
        for j, (o, n, e) in enumerate(tested):
            out += self.builds_case(j, n, calls.get(j))
        out += ["}", f"}}  // namespace {m.namespace}", ""]
        return "\n".join(out)

    def hidden_method(self, n):
        """Whether the method is one its class keeps to itself (private or protected): reached by its address."""
        rec = self.m.new.records.get(n.owner, (None, None))[0]
        if rec is None:
            return False
        access = "private" if rec.get("tagUsed") == "class" else "public"
        hidden = None
        for c in kids(rec):
            if c.get("kind") == "AccessSpecDecl":
                access = c.get("access", access)
            elif c.get("kind") == "CXXMethodDecl" and c.get("name") == n.name:
                if hidden is not None:
                    raise Stop(f"{n.label()}: its class has more than one {n.name} - the form \"two builds\" does not take an overloaded method yet")
                hidden = access != "public"
        return bool(hidden)

    def builds_case(self, j, n, robbed):
        label = n.label()
        T = f"::{n.owner}" if n.owner else None
        body = [f"  {{  // {label}", f"    ::sc::lowered::Builds rec({C.text_literal(label)});", "    ::kern::converted::Draw draw;",
                "    for (int call = 0; rec.more(call); ++call) {"]
        inner = []
        method = n.method and not n.static and n.kind in ("CXXMethodDecl", "CXXDestructorDecl")
        if method:
            inner += [f"::sc::lowered::Self<{T}> self;", "if (draw.flag()) ::sc::lowered::draw(draw, *self);"]
        args, writes = [], []
        for k, (pn, pt, pd) in enumerate(n.params):
            t = pd or pt
            inner += [f"::sc::lowered::Arg<{t}> a{k};", f"::sc::lowered::draw(draw, a{k});"]
            args.append(f"a{k}.get()")
            if re.search(r"&\s*$", t) and not re.match(r"^const\b", t.strip()):
                writes.append((f"its parameter {pn}", f"a{k}.v"))
        a = ", ".join(args)
        ret = n.ret and plain(n.ret) != "void" and n.kind not in ("CXXConstructorDecl", "CXXDestructorDecl")
        if n.kind == "CXXDestructorDecl":
            call = "self.end()"
        elif n.kind == "CXXConstructorDecl":
            inner.append(f"std::optional<{T}> made;")
            call = f"made.emplace({a})"
        elif method:
            call = f"{robbed}({a})" if robbed else f"self->{n.name}({a})"
        elif n.owner:
            call = f"{T}::{n.name}({a})"
        else:
            call = f"::{n.cpp_scope}::{n.name}({a})" if n.cpp_scope else f"::{n.name}({a})"
        if ret:
            inner.append(f"std::optional<std::remove_cvref_t<decltype({call})>> r;")
            call = f"r.emplace({call})"
        inner += ["long asks = 0;", f"const bool defined = ::sc::lowered::run([&] {{ {call}; }}, asks);", "if (defined) {"]
        if ret:
            inner.append('  rec.part("what it returns", ::sc::lowered::bytes_of(r));')
        ended = getattr(self, "ends", {}).get(label)
        if ended is not None and not (method and n.kind != "CXXDestructorDecl"):
            raise Stop(f"--ends {label}: only a method leaves fields of its object")
        if method and n.kind != "CXXDestructorDecl" and ended:
            inner.append("  static constexpr const char* ended[] = {" + ", ".join(C.text_literal(f) for f in ended) + "};")
            inner.append('  rec.object_ending("its object", *self, ended);')
        elif method and n.kind != "CXXDestructorDecl":
            inner.append('  rec.object("its object", *self);')
        if n.kind == "CXXConstructorDecl":
            inner.append('  rec.part("the object it makes", ::sc::lowered::bytes_of(made));')
        for what, x in writes:
            inner.append(f"  rec.part({C.text_literal(what)}, ::sc::lowered::bytes_of({x}));")
        inner += ["}", "rec.take(defined, asks, call);"]
        return body + ["      " + x for x in inner] + ["    }", "  }"]


def compiled(path, m, flags, scratch):
    """The test as the suite compiles it (-fsyntax-only, the switches of run_tests.sh; the old program's headers beside
    it as run_tests.sh puts them, sc_base_<commit>/): one that does not compile is no test - the tool says why and
    leaves none (a value of the old it cannot carry, a type it does not spell ...)."""
    extra = []
    if getattr(m, "base_ns", None) and getattr(m, "old_dir", None):
        inc = os.path.join(scratch, "inc-" + m.base_ns)
        os.makedirs(inc, exist_ok=True)
        if not os.path.exists(os.path.join(inc, m.base_ns)):
            os.symlink(m.old_dir, os.path.join(inc, m.base_ns))
        extra.append("-I" + inc)
    r = subprocess.run([C.TestWriter.test_compiler(), "-std=c++20", "-fsyntax-only", "-ffp-contract=off", "-fno-exceptions", "-fno-rtti", "-Wall", "-Wextra",
                        "-Wno-unused-parameter", *flags, "-I" + os.path.join(HERE, "..", "kern", "tests"), *extra, path], capture_output=True, text=True)
    if r.returncode != 0:
        first = next((x for x in r.stderr.split("\n") if " error: " in x), r.stderr.strip().split("\n")[0] if r.stderr.strip() else "?")
        if os.path.basename(os.path.dirname(os.path.dirname(path))) == "builds":
            shutil.rmtree(os.path.dirname(path), ignore_errors=True)
        else:
            os.remove(path)
        raise Stop("its test does not compile: " + re.sub(r"^.*?/(?=[^/]+:\d+:\d+:)", "", first)[:300])


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--base", required=True, help="the commit the lowering started from (the old text)")
    ap.add_argument("--test", required=True, help="the folder of the tests (tests/lowered)")
    ap.add_argument("--namespace", default="sc", help="the program's namespace")
    ap.add_argument("--builds", action="store_true", help="the form \"two builds\" for every file (else for a file whose class has methods in other files)")
    ap.add_argument("--ends", action="append", default=[], metavar="FUNCTION=FIELD,...",
                    help="the form \"two builds\": a method whose change ends these fields of its object on purpose (BLUEPRINTS.md, "
                         "\"Bewusste Abweichungen\"): the new must leave each ended - as {} makes it -, every other field as the old")
    args, rest = ap.parse_known_args(argv[1:])
    ends = {}
    for e in args.ends:
        label, _, fields = e.partition("=")
        if not label or not fields:
            ap.error(f"--ends {e}: FUNCTION=FIELD,...")
        ends[label.strip()] = [f.strip() for f in fields.split(",") if f.strip()]
    if "--" not in rest:
        ap.error("the sources follow --")
    flags, sources = rest[:rest.index("--")], rest[rest.index("--") + 1:]
    flags = ["-std=c++20", *flags] if not any(f.startswith("-std=") for f in flags) else flags
    failed = False
    with tempfile.TemporaryDirectory(prefix="kern-source-diff-") as scratch:
        for src in sources:
            try:
                fl = [f for f in flags if f != "-std=c++20"]
                m = Module(src, args.base, fl, args.namespace, scratch)
                if m.unchanged():
                    print(f"{os.path.basename(src)}: unchanged since {args.base}")
                    continue
                writer = Writer
                if args.builds or spans_files(m):
                    m, writer = BuildsModule(src, args.base, fl, args.namespace, scratch), BuildsWriter
                    print(f"{os.path.basename(src)}: the form \"two builds\" (the old tree and the new, each a program of its own)")
                w = writer(m, args.test)
                w.ends = ends
                path, tested, covered, report = w.write()
                if path is not None:
                    compiled(path, m, fl, scratch)
                if path is None:
                    print(f"{os.path.basename(src)}: every function it had is unchanged since {args.base} (the new text only has more) - no test")
                else:
                    print(f"{os.path.basename(src)}: {len(tested)} functions compared, {len(covered)} compared through their callers -> {path}")
                print("\n".join(report))
            except Stop as s:
                failed = True
                print(f"{os.path.basename(src)}: no test - {s}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
