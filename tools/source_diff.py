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
        self.static = node.get("storageClass") == "static"
        self.template = None   # (the pattern's parameter names, {name: the type of this instance})
        sig = (node.get("type") or {}).get("qualType", "")
        self.ret, self.sig_params = split_params(sig) if "(" in sig else ("", [])
        self.const = bool(re.search(r"\)\s*const\b", sig))
        self.params = [(p.get("name") or f"p{k}", (p.get("type") or {}).get("qualType", ""), C.qtype(p)) for k, p in enumerate(x for x in kids(node) if x.get("kind") == "ParmVarDecl")]
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

    def __init__(self, cpp, header, flags, namespace, side):
        self.side = side   # "old" / "new": its namespace in the test is sc_<side>
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
        elif k == "VarDecl" and owner is None and os.path.abspath(n.get("_file") or "?") == self.cpp:
            self.variables.append((q(n.get("name") or "?"), n, scope))

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
        for x in walk(init):
            if x.get("kind") == "CallExpr":
                ref = next((y.get("referencedDecl") for y in walk(x) if y.get("kind") == "DeclRefExpr" and (y.get("referencedDecl") or {}).get("kind") == "FunctionDecl"), None)
                d = self.decl_by_id.get((ref or {}).get("id"), {})
                if not d.get("constexpr"):
                    return True
        return False

    def state(self):
        """The variables of the file that hold state: not const, no actor's Blueprint."""
        return [(q, n) for q, n, _ in self.variables if not self.constant(n) and self.actor_of(n) is None]

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
        for f in self.functions:
            self._guard(f.body, edits[f.file], f.file, 0)
        # (a registration is there for what its initializer does: nothing of the file reads it - a constant the
        # functions read stays, also where a function works it out when the program starts)
        read = {(x.get("referencedDecl") or {}).get("id") for f in self.functions for x in walk(f.body) if x.get("kind") == "DeclRefExpr"}
        for q, n, _ in self.variables:
            if self.registration(n) and n["id"] not in read:
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
        if k == "LambdaExpr":
            for c in ks[-1:]:
                self._guard(c, out, file, depth + 1)
            return
        if k == "ArraySubscriptExpr" and len(ks) == 2:
            base = ks[0]
            inner = C.bare(base)
            if not (inner.get("kind") == "ImplicitCastExpr" and inner.get("castKind") == "ArrayToPointerDecay"):
                raise Stop(f"an entry by a pointer ({os.path.basename(file)}:{raw.count(chr(10), 0, span(n)[0]) + 1}): the test cannot say whether it is there")
            a, b = span(n)
            out.append((a, a, "::sc::converted::at(", (a, 1, depth)))
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
                out.append((a, a, "::sc::converted::at(", (a, 1, depth)))
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
                out.append((a, a, f"::sc::converted::{name}(", (a, 1, depth)))
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
                wrap(ks[1], "::sc::converted::count(")
        elif k == "CXXConstructExpr" and re.match(r"^(const )?std::vector<", ct(n)) and ks and not n.get("list") and len(ks) <= 2 and ct(ks[0]) in INTEGRAL:
            wrap(ks[0], "::sc::converted::count(")
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
        elif k == "BinaryOperator" and n.get("opcode") in ("+", "-", "*") and len(ks) == 2 and ct(n) in SIGNED and not (literal(ks[0]) is not None and literal(ks[1]) is not None):
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
        for c in ks:
            self._guard(c, out, file, depth + 1)

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
            while n.get("kind") in ("ImplicitCastExpr", "ParenExpr") and n.get("castKind") != "ArrayToPointerDecay" and kids(n):
                n = C.bare(kids(n)[0])   # (an array handed as a pointer stays: it is the list)
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
        if b.get("kind") == "BinaryOperator" and b.get("opcode") == "+" and len(kids(b)) == 2:
            # `v.data() + k`, and with more added (`s.data + h + w * 4`, read from the left: the place first, then
            # everything added to it)
            first = b
            while first.get("kind") == "BinaryOperator" and first.get("opcode") == "+" and len(kids(first)) == 2 and head(kids(first)[0]) is None:
                first = C.bare(kids(first)[0])
                while first.get("kind") == "ParenExpr" and kids(first):
                    first = C.bare(kids(first)[0])
            if first.get("kind") == "BinaryOperator" and first.get("opcode") == "+" and len(kids(first)) == 2:
                h = head(kids(first)[0])
                ha, hb = h["_at"]
                _, pb = kids(first)[0]["_at"]
                _, kb = b["_at"]
                plus = re.match(r"\s*\+", raw[pb:kb])
                if plus and "(" not in raw[b["_at"][0]:ha]:   # (no bracket opened before the place)
                    out.append((a0, b0, f"::sc::lowered::at_place({raw[ha:hb]}, {raw[pb + plus.end():kb].strip()})", (a0, 2, 0)))
                    return
        h = head(x)
        if h is not None:
            ha, hb = h["_at"]
            out.append((a0, b0, f"::sc::lowered::at_place({raw[ha:hb]}, 0)", (a0, 2, 0)))
            return
        raise Stop(f"memcpy through a pointer the test cannot follow ({os.path.basename(file)}:{raw.count(chr(10), 0, a0) + 1})")


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


def pasted(version, file, edits, include_dirs, hoisted):
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
        self.header = header if os.path.exists(header) else None
        top = subprocess.run(["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True, check=True).stdout.strip()
        rel = lambda p: os.path.relpath(os.path.abspath(p), top)
        def old_text(p):
            r = subprocess.run(["git", "show", f"{base}:{rel(p)}"], capture_output=True, text=True, cwd=top)
            return r.stdout if r.returncode == 0 else None
        self.old_cpp_text = old_text(src)
        if self.old_cpp_text is None:
            raise Stop(f"{src} is not in {base}: a new file has no old text to compare with")
        old_dir = os.path.join(scratch, self.stem)
        os.makedirs(old_dir, exist_ok=True)
        old_cpp = os.path.join(old_dir, os.path.basename(src))
        open(old_cpp, "w").write(self.old_cpp_text)
        old_header = None
        if self.header:
            t = old_text(self.header)
            if t is not None:
                old_header = os.path.join(old_dir, os.path.basename(self.header))
                open(old_header, "w").write(t)
        self.include_dirs = [f[2:] for f in flags if f.startswith("-I")]
        self.old = Version(old_cpp, old_header, flags, namespace, "old")
        self.new = Version(src, self.header, flags, namespace, "new")
        self.namespace = namespace

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
        for o in olds:
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
            if found:
                out.append(found)
            else:
                alone.append(o)
        return out, alone

    def extra(self, o, n, state_old):
        """The new's parameters beyond the old's - each a reference to a struct of the changed files whose fields are
        variables of the old file (the state made explicit): {its place: {field: old variable}}; None: n is no
        such counterpart of o."""
        ot, nt = o.param_types(), n.param_types()
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

    def side_records(self, v):
        """The classes and enums of the changed files: {name as spelled - unqualified: its name in the test}."""
        names = {}
        for q, (rec, scope) in v.records.items():
            if v.mine(rec):
                names[q.split("::")[-1]] = f"::{self.ns[v.side]}::{q}"
        for q, e in v.enums.items():
            if v.mine(e):
                names[q.split("::")[-1]] = f"::{self.ns[v.side]}::{q}"
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
        m = self.m
        pairs, alone = m.pairs()
        tested = [(o, n, e) for o, n, e in pairs if m.changed(o, n, e)]
        report = []
        tested_ids = {o.node["id"] for o, _, _ in tested}
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
        for name in new_state:
            if name not in old_state:
                raise Stop(f"the new text keeps a variable of the file the old did not have: {name}")
        users = m.old.users()
        for o, n, extra in tested:
            mapped = {v for e in extra.values() for v in e.values()}
            for var in users.get(o.node["id"], ()):
                if var.split("::")[-1] not in new_state and var not in mapped:
                    raise Stop(f"{o.label()} uses the variable {var} of the file, which the new {n.label()} neither has nor is handed")
        if not tested and not covered:
            # (every function the old had stands in the new as it was - the new only has more: nothing to compare)
            return None, tested, covered, [f"    the same: {o.label()}" for o, _, _ in pairs]
        self.check_fields()
        text = self.unit(tested)
        os.makedirs(self.out_dir, exist_ok=True)
        path = os.path.join(self.out_dir, f"test_{m.stem}_lowered.cpp")
        open(path, "w", encoding="latin-1").write(text)   # (the bytes of the sources as they are: see Version.text)
        for o, n, e in tested:
            report.append(f"    compared: {o.label()}  ->  {n.label()}" + (f"  (the state: {', '.join(f'{f} = {v}' for x in e.values() for f, v in x.items())})" if e else ""))
        for o, n, e in pairs:
            if (o, n, e) not in tested:
                report.append(f"    the same: {o.label()}")
        for label, through in covered.items():
            report.append(f"    gone, compared through its callers: {label}  <-  {', '.join(through)}")
        return path, tested, covered, report

    def check_fields(self):
        """A class of the old and the new of one name has the same fields, in the same order (carried by name)."""
        m = self.m
        for q, (rec, _) in m.old.records.items():
            if not m.old.mine(rec):
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
            b = [c.get("name") for c in kids(other) if c.get("kind") == "FieldDecl"]
            if a != b:
                raise Stop(f"the class {name} has other fields in the new text ({', '.join(a)} / {', '.join(b)}): the test cannot carry its values")

    # ---- the text of the test ----
    def unit(self, tested):
        m = self.m
        hoisted = {}
        old_edits, new_edits = m.old.guards(), m.new.guards()
        pieces = []
        for v, edits in ((m.old, old_edits), (m.new, new_edits)):
            body = ""
            for f in ([v.header] if v.header else []) + [v.cpp]:
                body += f"// ---- {os.path.basename(f)}, {'as it stood before' if v.side == 'old' else 'as it stands'} ----\n" + pasted(v, f, edits, m.include_dirs, hoisted) + "\n"
            pieces.append((v.side, body))
        actors = sorted({a for _, a in m.old.actors()} | {a for _, a in m.new.actors()})
        out = [f"// Generated by tools/source_diff.py from {os.path.basename(m.src)} - do not edit. The proof of its lowering into the C++ subset",
               "// (BLUEPRINTS.md, \"Absenkung\"): the old text and the new, each in a namespace of its own, compiled as the game compiles them;",
               "// every function the lowering changed is called in both with the same drawn state and arguments, and what they give and leave",
               "// must be the same, bit for bit (../source_diff.h)."]
        out += list(hoisted) + ['#include "../source_diff.h"', ""]
        out += [f"namespace {self.ns[side]}::{m.namespace} {{ using namespace ::{m.namespace}; }}" for side in ("old", "new")] + [""]
        out += ["#pragma GCC diagnostic push", '#pragma GCC diagnostic ignored "-Wunused-function"', '#pragma GCC diagnostic ignored "-Wunused-variable"',
                '#pragma GCC diagnostic ignored "-Wunused-const-variable"', "#define private public", "#define protected public"]
        for side, body in pieces:
            out += [f"namespace {self.ns[side]} {{", body, f"}}  // namespace {self.ns[side]}", ""]
        out += ["#undef private", "#undef protected", "#pragma GCC diagnostic pop", ""]
        # the fields of the classes of the changed files, and the values of the enums the test draws
        out.append("namespace sc::lowered {")
        for v in (m.old, m.new):
            for q, (rec, _) in v.records.items():
                if not v.mine(rec) or rec.get("tagUsed") == "union":
                    continue
                fields = [c.get("name") for c in kids(rec) if c.get("kind") == "FieldDecl"]
                if not fields:
                    continue
                t = f"::{self.ns[v.side]}::{q}"
                tie = ", ".join(f"x.{f}" for f in fields)
                out.append(f"template <> struct Fields<{t}> {{ static auto of({t}& x) {{ return std::tie({tie}); }} static auto of(const {t}& x) {{ return std::tie({tie}); }} }};")
        enums = {}
        for v in (m.old, m.new):
            for q, e in v.enums.items():
                if q.endswith(")") or not e.get("name"):
                    continue
                if not v.mine(e) and any(q == h or q.startswith(h + "::") for h in v.hidden):
                    continue   # (an enum a class of another file keeps to itself: the test neither names nor draws it)
                t = f"::{self.ns[v.side]}::{q}" if v.mine(e) else f"::{q}"
                values = [c.get("name") for c in kids(e) if c.get("kind") == "EnumConstantDecl"]
                if values and t not in enums:
                    enums[t] = values
        for t, values in enums.items():
            out.append(f"template <> struct Values<{t}> {{ static constexpr long long all[] = {{{', '.join(f'static_cast<long long>({t}::{x})' for x in values)}}}; }};")
        out += ["}  // namespace sc::lowered", ""]
        out.append(f"namespace {m.namespace} {{")
        out.append(f"SC_TEST(lowered_{re.sub(r'\W', '_', m.stem)}_cpp) {{")
        if actors or any(self.logic_param(t) for o, n, _ in tested for _, t, _ in o.params + n.params):
            out.append(f"  ::sc::lowered::World world({{{', '.join(C.text_literal(a) for a in actors)}}});")
            for v in (m.old, m.new):
                for q, _ in v.actors():
                    out.append(f"  ::{self.ns[v.side]}::{q}.bind(world.logic({'true' if v.side == 'old' else 'false'}));")
            world = True
        else:
            world = False
        for o, n, extra in tested:
            out += self.case(o, n, extra, world)
        if world:
            for v in (m.old, m.new):
                for q, _ in v.actors():
                    out.append(f"  ::{self.ns[v.side]}::{q}.bind(no_logic());   // (the test's logic is gone with it)")
        out += ["}", f"}}  // namespace {m.namespace}", ""]
        return "\n".join(out)

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
        lines = [f"  {{  // {o.label()}  ->  {n.label()}", f"    ::sc::lowered::Tally tally({C.text_literal(o.label())});", "    ::sc::converted::Draw draw;",
                 "    for (int call = 0; tally.more(call); ++call) {"]
        body = []
        old_state = m.old.state()
        new_state = {q.split("::")[-1]: q for q, _ in m.new.state()}
        # the variables of the file: drawn on the old side, carried to their counterparts
        for q, node in old_state:
            body.append(f"::sc::lowered::draw(draw, ::{self.ns['old']}::{q});")
            name = q.split("::")[-1]
            if name in new_state:
                body.append(f"::sc::lowered::carry(::{self.ns['old']}::{q}, ::{self.ns['new']}::{new_state[name]});")
        # the object
        if o.method and not o.static and o.kind != "CXXConstructorDecl":
            ot, nt = f"::{self.ns['old']}::{o.owner}", f"::{self.ns['new']}::{n.owner}"
            # (as the converted tests make an object: half the time as its constructor makes it - its parts fit together -,
            # half the time field by field)
            body += [f"{ot} o_self{{}};", "if (draw.flag()) ::sc::lowered::draw(draw, o_self);", f"{nt} n_self{{}};", "::sc::lowered::carry(o_self, n_self);"]
        if o.kind == "CXXConstructorDecl":
            body += [f"std::optional<::{self.ns['old']}::{o.owner}> o_obj;", f"std::optional<::{self.ns['new']}::{n.owner}> n_obj;"]
        # the arguments
        oargs, nargs, compares = [], [], []
        k = 0
        for j, (pn, pt, pd) in enumerate(n.params):
            nt = self.spell(pt if not n.template else pd, m.new)
            if j in extra:
                body.append(f"::sc::lowered::Arg<{nt}> n{j};")
                for fname, var in extra[j].items():
                    body.append(f"::sc::lowered::carry(::{self.ns['old']}::{var}, n{j}.v.{fname});")
                nargs.append(f"n{j}.get()")
                continue
            on, ot, od = o.params[k]
            ot = self.spell(ot if not o.template else od, m.old)
            if self.logic_param(ot):
                oargs.append("world.logic(true)")
                nargs.append("world.logic(false)")
            else:
                body += [f"::sc::lowered::Arg<{ot}> o{k};", f"::sc::lowered::draw(draw, o{k});", f"::sc::lowered::Arg<{nt}> n{j};", f"::sc::lowered::carry(o{k}, n{j});"]
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
        if ret:
            body += [f"std::optional<std::remove_cvref_t<decltype({ocall})>> o_r;", f"std::optional<std::remove_cvref_t<decltype({ncall})>> n_r;"]
        def side(c, who):
            return f"{who}_r.emplace({c});" if ret else f"{c};"
        if world:
            body.append("world.before();")
        body += ["long old_asks = 0, new_asks = 0;", f"const bool old_defined = ::sc::lowered::run([&] {{ {side(ocall, 'o')} }}, old_asks);",
                 f"const bool new_defined = ::sc::lowered::run([&] {{ {side(ncall, 'n')} }}, new_asks);", "::sc::lowered::Differs differs;"]
        if ret:
            body.append('differs.check("what it returns", o_r, n_r);')
        if o.method and not o.static and o.kind != "CXXConstructorDecl":
            body.append('differs.check("its object", o_self, n_self);')
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


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--base", required=True, help="the commit the lowering started from (the old text)")
    ap.add_argument("--test", required=True, help="the folder of the tests (tests/lowered)")
    ap.add_argument("--namespace", default="sc", help="the program's namespace")
    args, rest = ap.parse_known_args(argv[1:])
    if "--" not in rest:
        ap.error("the sources follow --")
    flags, sources = rest[:rest.index("--")], rest[rest.index("--") + 1:]
    flags = ["-std=c++20", *flags] if not any(f.startswith("-std=") for f in flags) else flags
    failed = False
    with tempfile.TemporaryDirectory(prefix="kern-source-diff-") as scratch:
        for src in sources:
            try:
                m = Module(src, args.base, [f for f in flags if f != "-std=c++20"], args.namespace, scratch)
                if m.unchanged():
                    print(f"{os.path.basename(src)}: unchanged since {args.base}")
                    continue
                path, tested, covered, report = Writer(m, args.test).write()
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
