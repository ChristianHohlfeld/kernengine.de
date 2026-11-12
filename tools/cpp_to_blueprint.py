#!/usr/bin/env python3
"""C++ to Blueprint: a program's ordinary C++, written as Blueprints.

A Blueprint and its C++ are one thing in two notations (kern/docs/blueprint_cpp.md); the notation
is a subset of C++. A game written in ordinary C++ uses more of the language than that subset.
This tool reads such sources with the compiler's own parser (clang's syntax tree) and writes them
in the notation, function by function:

  tools/cpp_to_blueprint.py --convert <namespace> --level <level.klevel> [--folder <name>]
      [--test <dir>] [--texts <dir>] <include flags...> -- <file.cpp>...

  - every source file becomes the Blueprint of one actor of the level (terrain.cpp -> "Terrain"):
    its functions are the Blueprint's functions, its constants the Blueprint's variables, the
    enums and structs it computes with the Blueprint's types;
  - a function is written when everything in it has a place in the notation, and so has what it
    calls; the others stay C++ until the notation (or this tool) has a place for what they use;
  - --test writes, for every file, a test that runs each converted function next to the C++ it
    was read from, on the same arguments, and compares what they give: the C++ as it stands, the
    C++ compiled once more with 64-bit numbers (as a Blueprint computes), the Blueprint in the
    interpreter and the Blueprint as its compiled class;
  - --texts writes each Blueprint's text into a directory, to read.

How the Blueprint computes is the Blueprint language's own arithmetic: numbers are 64-bit and
whole numbers exact, a division by zero gives 0, a vector's components are 32-bit. It is the same
in the interpreter, in the JIT and in the compiled class, on every machine. Where the C++ computed
in 32 bit, the Blueprint is at least as exact; a place whose meaning would change (32-bit
wrap-around, bit arithmetic) is not converted as long as the notation has no node for it. A number
literal is the decimal the source spells (0.1f is 0.1).

  tools/cpp_to_blueprint.py --check <namespace> <include flags...> -- <file.cpp>...

only counts: how many functions are written, and what the others wait for.

Needs clang++ (CLANGXX overrides the binary).
"""
import collections
import gzip
import hashlib
import json
import os
import re
import struct
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


class Wait(Exception):
    """What keeps a function C++ for now: something the notation has no place for yet."""


# ---- the notation's words ------------------------------------------------------------------------
CPP_WORDS = set("""alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t char16_t char32_t class compl concept const consteval constexpr
constinit const_cast continue co_await co_return co_yield decltype default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline
int long mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public register reinterpret_cast requires return short signed sizeof static
static_assert static_cast struct switch template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq
Bool Int Float Vector Text Actor List loop machine entry detached kern value actor delta text other self""".split())
FORMULA_WORDS = set("min max clamp abs sign floor ceil round trunc sqrt pow exp log sin cos tan asin acos atan atan2 rad deg lerp div mod pi".split())
NODE_NAME = re.compile(r"^(math|logic|flow|list|text|vector|struct|enum|value|func|var|debug|random|time|event|actor|physics|combat|game|n\d+|go\d+|on\d+|it\d+|machine\d+)(_|$)")

NUMBER = {"bool": "Bool", "float": "Float", "double": "Float", "long double": "Float", "int": "Int", "long": "Int", "long long": "Int", "short": "Int", "signed char": "Int",
          "unsigned char": "Int", "unsigned short": "Int", "unsigned int": "Int", "char": "Int", "unsigned long": "Int", "unsigned long long": "Int"}
UNSIGNED = {"unsigned char", "unsigned short", "unsigned int", "unsigned long", "unsigned long long"}
TEXT = {"const char *", "std::basic_string<char>", "std::string", "std::basic_string_view<char>", "std::string_view", "std::basic_string<char, std::char_traits<char>, std::allocator<char>>",
        "std::basic_string_view<char, std::char_traits<char>>"}
# (the names the engine writes these types with, where clang gives the name as written)
ALIAS = {"f32": "float", "f64": "double", "i8": "signed char", "i16": "short", "i32": "int", "i64": "long", "u8": "unsigned char", "u16": "unsigned short", "u32": "unsigned int",
         "u64": "unsigned long", "usize": "unsigned long", "size_t": "unsigned long", "Vec2": "kern::Vec2", "Vec3": "kern::Vec3", "Vec4": "kern::Vec4", "Quat": "kern::Quat",
         "Mat4": "kern::Mat4"}
ZERO = {"Float": "0.0", "Int": "0", "Bool": "false", "Vector": "Vector{0.0, 0.0, 0.0}", "Text": '""'}
# (functions of numbers a formula has, by the names C++ calls them)
MATH = {"sqrt": "sqrt", "sqrtf": "sqrt", "fabs": "abs", "fabsf": "abs", "abs": "abs", "fmin": "min", "fminf": "min", "min": "min", "min_of": "min", "fmax": "max", "fmaxf": "max", "max": "max",
        "max_of": "max", "floor": "floor", "floorf": "floor", "ceil": "ceil", "ceilf": "ceil", "round": "round", "roundf": "round", "lround": "round", "lroundf": "round", "trunc": "trunc",
        "truncf": "trunc", "pow": "pow", "powf": "pow", "exp": "exp", "expf": "exp", "log": "log", "logf": "log", "sin": "sin", "sinf": "sin", "cos": "cos", "cosf": "cos", "tan": "tan",
        "tanf": "tan", "asin": "asin", "acos": "acos", "atan": "atan", "atan2": "atan2", "atan2f": "atan2", "fmod": "mod", "fmodf": "mod", "clamp": "clamp"}


def flit(x):
    """A number as the notation writes it: the shortest text that is this very number."""
    if x != x or x in (float("inf"), float("-inf")):
        raise Wait("a number that is not finite")
    if x == int(x) and abs(x) < 1e15:
        return f"{int(x)}.0"
    return repr(float(x))


def f32_decimal(x):
    """The shortest decimal that is this 32-bit number (0.1f -> 0.1)."""
    f = struct.unpack("f", struct.pack("f", x))[0]
    for digits in range(1, 10):
        c = float(f"{f:.{digits}g}")
        if struct.unpack("f", struct.pack("f", c))[0] == f:
            return c
    return f


def text_literal(s):
    out = []
    for ch in s.encode("utf-8"):
        c = chr(ch)
        if c == "\n": out.append("\\n")
        elif c == "\t": out.append("\\t")
        elif c == "\r": out.append("\\r")
        elif c == '"': out.append('\\"')
        elif c == "\\": out.append("\\\\")
        elif 32 <= ch < 127: out.append(c)
        else: out.append(f"\\{ch:03o}")
    return '"' + "".join(out) + '"'


def c_string(v):
    """The bytes of a string literal as clang prints it."""
    return json.loads(v) if v.startswith('"') else v


# ---- the syntax tree ---------------------------------------------------------------------------
def run_clang(path, namespace, flags):
    h = hashlib.sha1()
    h.update(open(path, "rb").read())
    d = os.path.dirname(os.path.abspath(path))
    for name in sorted(os.listdir(d)):
        if name.endswith(".h"):
            h.update(f"{name}:{os.path.getmtime(os.path.join(d, name))}".encode())
    h.update(" ".join([namespace, *flags]).encode())
    cache = os.path.join(os.environ.get("TMPDIR", "/tmp"), "kern-cpp-to-blueprint")
    os.makedirs(cache, exist_ok=True)
    cached = os.path.join(cache, h.hexdigest()[:20] + ".json.gz")
    if os.path.exists(cached):
        s = gzip.open(cached, "rt").read()
    else:
        cmd = [os.environ.get("CLANGXX", "clang++"), "-std=c++20", "-fsyntax-only", "-Xclang", "-ast-dump=json", "-Xclang", f"-ast-dump-filter={namespace}::", *flags, path]
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0 and not r.stdout:
            sys.stderr.write(f"{path}: clang failed\n{r.stderr[:2000]}\n")
            return []
        s = r.stdout
        with gzip.open(cached + ".tmp", "wt", compresslevel=3) as f:
            f.write(s)
        os.replace(cached + ".tmp", cached)
    out, dec, i = [], json.JSONDecoder(), 0
    while True:
        j = s.find("{", i)
        if j < 0:
            break
        o, i = dec.raw_decode(s, j)
        out.append(o)
    return out


def annotate(o, st):
    """Every place of the tree gets the file it is in (clang names a file only where it changes)."""
    if isinstance(o, list):
        for c in o:
            annotate(c, st)
        return
    if not isinstance(o, dict):
        return
    if "offset" in o and "kind" not in o:
        if "file" in o:
            st["file"] = o["file"]
        o["_file"] = st["file"]
        return
    for k, v in o.items():
        if isinstance(v, (dict, list)):
            annotate(v, st)
    if "kind" in o and "range" in o:
        b = o["range"].get("begin", {})
        b = b.get("expansionLoc", b)
        e = o["range"].get("end", {})
        e = e.get("expansionLoc", e)
        o["_file"] = b.get("_file", "")
        o["_at"] = (b.get("offset"), (e.get("offset") or 0) + e.get("tokLen", 0) if e.get("offset") is not None else None)


def kids(n):
    return [c for c in n.get("inner", []) or [] if isinstance(c, dict) and c.get("kind")]


def qtype(n):
    t = n.get("type", {})
    return t.get("desugaredQualType") or t.get("qualType", "")


SKIP = ("ParenExpr", "ExprWithCleanups", "MaterializeTemporaryExpr", "CXXBindTemporaryExpr", "ConstantExpr", "SubstNonTypeTemplateParmExpr", "FullExpr")


def bare(n):
    """The expression without what only stands around it (brackets, temporaries)."""
    while n.get("kind") in SKIP and kids(n):
        n = kids(n)[0]
    return n


def walk(n):
    yield n
    for c in kids(n):
        yield from walk(c)


class Tree:
    """One source file as the compiler read it."""

    def __init__(self, path, namespace, flags):
        self.path = os.path.abspath(path)
        self.namespace = namespace
        self.dumps = run_clang(path, namespace, flags)
        st = {"file": ""}
        for o in self.dumps:
            annotate(o, st)
        self.texts = {}
        self.decl = {}        # id -> the declaration
        self.body = {}        # id -> the declaration with the body (of a function declared earlier)
        self.records = {}     # qualified name -> record
        self.enums = {}       # qualified name -> enum
        self.owner = {}       # id of a method -> its record's qualified name
        self.scope_of = {}    # id -> the qualified scope it stands in ("sc::combat::")
        seen = set()
        for o in self.dumps:
            self.take(o, self.scope_from(o), seen)
        for i, n in list(self.decl.items()):
            if n.get("kind") in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl") and any(c.get("kind") == "CompoundStmt" for c in kids(n)):
                self.body[i] = n
                p = n.get("previousDecl")
                while p:
                    self.body[p] = n
                    p = self.decl.get(p, {}).get("previousDecl")
        for i, n in list(self.decl.items()):  # (a constant declared in a header and defined later)
            if n.get("kind") == "VarDecl" and n.get("init") and n.get("previousDecl"):
                self.body.setdefault(n["previousDecl"], n)

    def scope_from(self, o):
        """The scope a dumped declaration stands in: clang says it for those defined outside their class."""
        return ""

    def text(self, file):
        if file not in self.texts:
            try:
                self.texts[file] = open(file, encoding="utf-8", errors="replace").read()
            except OSError:
                self.texts[file] = ""
        return self.texts[file]

    def source(self, n):
        a, b = n.get("_at", (None, None))
        return self.text(n.get("_file", ""))[a:b] if a is not None and b is not None else ""

    def take(self, o, scope, seen):
        k = o.get("kind")
        if "id" in o:
            if o["id"] in seen:
                return
            seen.add(o["id"])
            self.decl[o["id"]] = o
            self.scope_of[o["id"]] = scope
        if k == "NamespaceDecl":
            for c in kids(o):
                self.take(c, scope + o.get("name", "") + "::", seen)
        elif k == "EnumDecl" and o.get("name"):
            q = scope + o["name"]
            self.enums[q] = o
            o["_q"] = q
            for c in kids(o):
                if "id" in c:
                    self.decl[c["id"]] = c
                    c["_enum"] = q
        elif k == "CXXRecordDecl" and o.get("name"):
            q = scope + o["name"]
            if o.get("completeDefinition"):
                self.records[q] = o
                o["_q"] = q
            for c in kids(o):
                if c.get("kind") in ("CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl", "FieldDecl") and "id" in c:
                    self.owner[c["id"]] = q
                self.take(c, q + "::", seen)
        elif k in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl", "VarDecl", "FieldDecl", "FunctionTemplateDecl", "ClassTemplateDecl", "LinkageSpecDecl"):
            if k in ("CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl") and o.get("parentDeclContextId") and o["id"] not in self.owner:
                r = self.decl.get(o["parentDeclContextId"])
                if r is not None and r.get("_q"):
                    self.owner[o["id"]] = r["_q"]


def qualified_scope(tree, namespace):
    """clang dumps the declarations of a namespace one by one, without the namespace around them: the
    scope of each is read from its qualified type names. The types of the program are found by their
    last names instead (see Unit.find_type)."""
    return namespace + "::"


# ---- one Blueprint -------------------------------------------------------------------------------
class Function:
    def __init__(self, name):
        self.name = name
        self.cpp = name
        self.pure = True
        self.params = []      # (name, type)
        self.result = None    # type, None: no result
        self.text = None
        self.wait = None
        self.lines = 0
        self.decl = None
        self.calls = []       # the functions it calls (Function)
        self.indexes = False  # it reads an entry of a list at a place it computes
        self.file = ""


class Unit:
    """A source file and the Blueprint it becomes."""

    def __init__(self, tree, name):
        self.tree = tree
        self.name = name
        self.names = {name}         # what a name of the Blueprint must not be again
        self.enums = {}             # qualified C++ name -> Blueprint name
        self.structs = {}           # qualified C++ name -> Blueprint name
        self.types = []             # the declarations, in the order they must stand
        self.struct_fields = {}     # Blueprint name -> [(field, type, default, C++ field)]
        self.enum_values = {}       # Blueprint name -> [value]
        self.variables = {}         # decl id -> (name, type)
        self.variable_lines = []
        self.functions = {}         # id of the function's body -> Function
        self.order = []             # the written functions, callees first
        self.members = []
        self.next_node = 10
        self.working = set()
        self.short = collections.defaultdict(list)
        for q in list(tree.records) + list(tree.enums):
            self.short[q.split("::")[-1]].append(q)

    # ---- names ----
    def fresh(self, want, taken=None):
        name = re.sub(r"_+", "_", re.sub(r"\W", "_", want))[:56] or "x"
        if name[0].isdigit() or (name[0] == "_" and name[1:2].isupper()):
            name = "v" + name
        if name[:5] in ("kern_", "KERN_"):
            name = "K" + name[1:] if name[0] == "k" else "k" + name
        if name in CPP_WORDS or name in FORMULA_WORDS or NODE_NAME.match(name) or name.endswith("_list") or name.endswith("Result"):
            name = "the_" + name + ("_" if name.endswith("_list") or name.endswith("Result") else "")
        base, i = name, 1
        while name in self.names or (taken is not None and name in taken):
            i += 1
            name = f"{base}_{i}"
        return name

    # ---- types ----
    def clean(self, t):
        t = re.sub(r"\b(const|volatile|struct|class|enum)\b", " ", t).replace("&&", " ").replace("&", " ")
        t = re.sub(r"(?<![\w:])(?:kern::)?(f32|f64|i8|i16|i32|i64|u8|u16|u32|u64|usize|Vec2|Vec3|Vec4|Quat|Mat4|size_t)\b", lambda m: ALIAS[m.group(1)], t)
        t = t.replace("std::size_t", "unsigned long")
        return re.sub(r"\s+", " ", t).strip()

    def find_type(self, t):
        """The program's record or enum a type name means (its qualified name), or None."""
        if t in self.tree.records or t in self.tree.enums:
            return t
        cands = [q for q in self.short.get(t.split("::")[-1], []) if q.endswith(t) or t.endswith(q)]
        if len(cands) == 1:
            return cands[0]
        if len(cands) > 1:
            best = [q for q in cands if q == t or q.endswith("::" + t)]
            if len(best) == 1:
                return best[0]
            raise Wait(f"the name {t} means several types")
        return None

    def btype(self, raw):
        """The Blueprint type of a C++ type."""
        if raw.strip() in ("const char *", "const char *const"):
            return "Text"
        t = self.clean(raw)
        if t in NUMBER:
            return NUMBER[t]
        if t in TEXT or t.startswith("std::basic_string<char") or t.startswith("std::basic_string_view<char"):
            return "Text"
        if t in ("kern::Vec3", "kern::Vec2"):
            return "Vector"
        if t == "void":
            return None
        if t == "kern::Quat":
            raise Wait("Quat (no rotation value in the language)")
        if t in ("kern::Mat4", "kern::Vec4"):
            raise Wait(f"{t[6:]} (no such value in the language)")
        if "*" in t:
            raise Wait("a pointer")
        m = re.match(r"std::vector<(.+)>$", t)
        if m:
            inner = m.group(1).split(", std::allocator")[0]
            it = self.btype(inner)
            if it.startswith("List<"):
                raise Wait("a list of lists")
            return f"List<{it}>"
        m = re.match(r"std::array<(.+), (\d+)>$", t) or re.match(r"(.+?)\s*\[(\d*)\]$", t)
        if m:
            it = self.btype(m.group(1))
            if it.startswith("List<"):
                raise Wait("a list of lists")
            return f"List<{it}>"
        if t.startswith("std::unique_ptr") or t.startswith("std::shared_ptr"):
            raise Wait("an owning pointer (unique_ptr / shared_ptr)")
        if t.startswith("std::unordered_map") or t.startswith("std::map") or t.startswith("std::unordered_set") or t.startswith("std::set"):
            raise Wait("a map or set (the language has lists)")
        if t.startswith("std::function") or "(lambda" in t:
            raise Wait("a function value (lambda / std::function)")
        if t.startswith("std::pair") or t.startswith("std::tuple") or t.startswith("std::optional") or t.startswith("std::initializer_list"):
            raise Wait("pair / tuple / optional")
        q = self.find_type(t)
        if q in self.tree.enums:
            return self.enum(q)
        if q in self.tree.records:
            return self.struct(q)
        if t.startswith("kern::"):
            raise Wait(f"the engine's {t.split('<')[0]}")
        raise Wait(f"the type {t.split('<')[0]}")

    def enum(self, q):
        if q in self.enums:
            return self.enums[q]
        o = self.tree.enums[q]
        values, at = [], 0
        for c in kids(o):
            if c.get("kind") != "EnumConstantDecl":
                continue
            v = next((x.get("value") for x in walk(c) if x.get("kind") == "ConstantExpr" and "value" in x), None)
            if v is not None and int(v) != at:
                raise Wait(f"the enum {q.split('::')[-1]} (its values are not 0, 1, 2 ...)")
            values.append(c["name"])
            at += 1
        if not values:
            raise Wait("an enum without values")
        name = self.fresh(q.split("::")[-1])
        self.names.add(name)
        vals = []
        for v in values:
            vals.append(v + "_" if v in CPP_WORDS or NODE_NAME.match(v) else v)
        self.enums[q] = name
        self.enum_values[name] = vals
        self.types.append(f"  KERN_ENUM({name}, {', '.join(vals)});")
        return name

    def struct(self, q):
        if q in self.structs:
            if self.structs[q] is None:
                raise Wait(f"the struct {q.split('::')[-1]} contains itself")
            return self.structs[q]
        o = self.tree.records[q]
        if o.get("bases"):
            raise Wait(f"the class {q.split('::')[-1]} (a base class)")
        self.structs[q] = None
        try:
            fields, taken = [], set()
            for c in kids(o):
                if c.get("kind") in ("CXXMethodDecl", "CXXDestructorDecl") and c.get("virtual"):
                    raise Wait(f"the class {q.split('::')[-1]} (virtual functions)")
                if c.get("kind") != "FieldDecl":
                    continue
                try:
                    ft = self.btype(qtype(c))
                except Wait as w:
                    raise Wait(f"the struct {q.split('::')[-1]} (its field {c.get('name')}: {w})")
                fname = self.fresh(c.get("name", "field"), taken)
                taken.add(fname)
                init = kids(c)[-1] if kids(c) else None
                default = self.zero(ft)
                if init is not None:
                    try:
                        default = Const(self).literal(init, ft)
                    except Wait as w:
                        raise Wait(f"the struct {q.split('::')[-1]} (the default of its field {c.get('name')}: {w})")
                fields.append((fname, ft, default, c.get("name")))
            if not fields:
                raise Wait(f"the struct {q.split('::')[-1]} has no fields")
            if len(fields) > 32:
                raise Wait(f"the struct {q.split('::')[-1]} has more than 32 fields")
        except Wait:
            del self.structs[q]
            raise
        name = self.fresh(q.split("::")[-1])
        self.names.add(name)
        self.structs[q] = name
        self.struct_fields[name] = fields
        self.types.append(f"  KERN_STRUCT({name}) {{ " + " ".join(f"{t} {f} = {d};" for f, t, d, _ in fields) + " };")
        return name

    def zero(self, t):
        if t in ZERO:
            return ZERO[t]
        if t.startswith("List<"):
            return t + "{}"
        if t in self.enum_values:
            return f"{t}::{self.enum_values[t][0]}"
        if t in self.struct_fields:
            return t + "{" + ", ".join(d for _, _, d, _ in self.struct_fields[t]) + "}"
        raise Wait(f"no default for {t}")

    def field(self, struct_name, cpp_field):
        for f, t, _, c in self.struct_fields[struct_name]:
            if c == cpp_field:
                return f, t
        raise Wait(f"{struct_name} has no field {cpp_field}")

    # ---- constants: the Blueprint's variables ----
    def constant(self, decl):
        d = self.tree.body.get(decl["id"], decl)
        if d["id"] in self.variables:
            return self.variables[d["id"]]
        if decl["id"] in self.variables:
            return self.variables[decl["id"]]
        raw = qtype(d)
        if not (d.get("constexpr") or re.search(r"\bconst\b", raw)):
            raise Wait("state outside a function (a variable of the file)")
        t = self.btype(raw)
        init = kids(d)[-1] if kids(d) and d.get("init") else None
        if init is None:
            raise Wait(f"the constant {d.get('name')} is defined elsewhere")
        lit = Const(self).literal(init, t)
        name = self.fresh(d.get("name", "k"))
        self.names.add(name)
        doc = self.comment_of(d)
        if doc:
            self.variable_lines.append(f"  /// {doc}")
        self.variable_lines.append(f"  {t} {name} = {lit};")
        self.variables[d["id"]] = self.variables[decl["id"]] = (name, t)
        return name, t

    def comment_of(self, d):
        """The comment that stands with a declaration: the lines above it, or what follows on its line."""
        text = self.tree.text(d.get("_file", ""))
        a, b = d.get("_at", (None, None))
        if a is None or not text:
            return ""
        line_start = text.rfind("\n", 0, a) + 1
        out = []
        i = line_start
        while i > 0:
            j = text.rfind("\n", 0, i - 1) + 1
            line = text[j:i - 1].strip()
            if line.startswith("//"):
                out.insert(0, line.lstrip("/").strip())
                i = j
            else:
                break
        if not out and b is not None:
            end = text.find("\n", b)
            m = re.search(r"//+\s*(.*)$", text[b:end if end >= 0 else len(text)])
            if m:
                out = [m.group(1).strip()]
        doc = " ".join(x for x in out if x)
        return re.sub(r"\s+", " ", doc.replace("*/", "* /")).strip()

    # ---- functions ----
    def function(self, decl_id):
        d = self.tree.body.get(decl_id)
        if d is None:
            raise Wait(f"{self.tree.decl.get(decl_id, {}).get('name', 'a function')}() is defined in another file")
        f = self.functions.get(d["id"])
        if f is not None:
            if f.wait:
                raise Wait(f"it calls {f.cpp}(), which waits")
            if f.text is None:
                raise Wait(f"{f.cpp}() calls itself (a Blueprint's function does not)")
            return f
        owner = self.tree.owner.get(d["id"], "")
        cpp = (owner.split("::")[-1] + "::" if owner else "") + d.get("name", "?")
        f = Function(self.fresh(cpp.replace("::", "_")))
        f.cpp = cpp
        f.decl = d
        f.file = d.get("_file", "")
        a, b = d.get("_at", (None, None))
        f.lines = self.tree.text(f.file).count("\n", a, b) + 1 if a is not None else 0
        self.functions[d["id"]] = f
        self.names.add(f.name)
        marks = (len(self.types), len(self.variable_lines), len(self.members), self.next_node, dict(self.enums), dict(self.structs), dict(self.variables), set(self.names),
                 dict(self.struct_fields), dict(self.enum_values))
        try:
            Emit(self, f).run()
            self.order.append(f)
        except Wait as w:
            f.wait = str(w)
            # (what it had begun to declare is not declared - unless a function written meanwhile uses it;
            # those were written before it began, so nothing is)
            del self.types[marks[0]:]
            del self.variable_lines[marks[1]:]
            del self.members[marks[2]:]
            self.enums, self.structs, self.variables, self.struct_fields, self.enum_values = marks[4], marks[5], marks[6], marks[8], marks[9]
            self.names = marks[7] | {f.name}
            raise
        return f

    def text(self):
        out = [f"struct {self.name} : kern::Blueprint {{"]
        if self.types:
            out += ["  // ---- types ----", *self.types, ""]
        if self.variable_lines:
            out += ["  // ---- variables ----", *self.variable_lines, ""]
        out.append("  // ---- functions ----")
        for f in self.order:
            out += [f.text, ""]
        if self.members:
            out += ["  // ---- what nodes give and keep ----", *self.members]
        while out[-1] == "":
            out.pop()
        out.append("};")
        return "\n".join(out) + "\n"


# ---- constants -------------------------------------------------------------------------------------
class Const:
    """What an expression is when everything in it is known: a constant's value, a default."""

    def __init__(self, unit):
        self.unit = unit

    def number(self, n):
        n = bare(n)
        k = n.get("kind")
        if k in ("ImplicitCastExpr", "CXXStaticCastExpr", "CStyleCastExpr", "CXXFunctionalCastExpr"):
            v = self.number(kids(n)[-1])
            t = self.unit.clean(qtype(n))
            if NUMBER.get(t) == "Int":
                return float(int(v))
            if t == "float":
                return struct.unpack("f", struct.pack("f", v))[0]
            if t == "bool":
                return 1.0 if v != 0 else 0.0
            return v
        if k == "FloatingLiteral":
            s = self.unit.tree.source(n)
            m = re.match(r"^[0-9.eE+\-']+", s)
            # (a literal is the decimal the source spells: 43758.5453f is 43758.5453, as a Blueprint's 64-bit number has it)
            return float(m.group(0).replace("'", "")) if m and m.group(0) not in (".", "") else float(n["value"])
        if k == "IntegerLiteral":
            return float(int(n["value"]))
        if k == "CXXBoolLiteralExpr":
            return 1.0 if n.get("value") else 0.0
        if k == "CharacterLiteral":
            return float(n["value"])
        if k == "UnaryOperator" and n.get("opcode") in ("-", "+", "!"):
            v = self.number(kids(n)[0])
            return -v if n["opcode"] == "-" else v if n["opcode"] == "+" else (0.0 if v else 1.0)
        if k == "BinaryOperator":
            a, b = (self.number(c) for c in kids(n))
            op = n["opcode"]
            isint = NUMBER.get(self.unit.clean(qtype(n))) == "Int"
            isf32 = self.unit.clean(qtype(n)) == "float"
            if op == "+": r = a + b
            elif op == "-": r = a - b
            elif op == "*": r = a * b
            elif op == "/":
                if b == 0:
                    raise Wait("a constant divided by zero")
                r = float(int(a / b)) if isint else a / b
            elif op == "<<" and isint: r = float(int(a) << int(b))
            elif op == ">>" and isint: r = float(int(a) >> int(b))
            elif op == "|" and isint: r = float(int(a) | int(b))
            elif op == "&" and isint: r = float(int(a) & int(b))
            else:
                raise Wait(f"a constant computed with {op}")
            return struct.unpack("f", struct.pack("f", r))[0] if isf32 else r
        if k == "DeclRefExpr":
            ref = n.get("referencedDecl", {})
            d = self.unit.tree.body.get(ref.get("id"), self.unit.tree.decl.get(ref.get("id")))
            if d is not None and d.get("kind") == "VarDecl" and d.get("init") and kids(d):
                return self.number(kids(d)[-1])
            if ref.get("kind") == "EnumConstantDecl":
                raise Wait("an enum value as a number")
            if ref.get("name") == "kPi":
                return struct.unpack("f", struct.pack("f", 3.14159265358979323846))[0]
            raise Wait(f"the constant {ref.get('name')} is defined elsewhere")
        if k == "CallExpr":
            name = callee_name(n)
            args = [self.number(c) for c in kids(n)[1:]]
            import math
            if name in ("sqrt", "sqrtf") and args[0] >= 0:
                return math.sqrt(args[0])
            if name in ("min", "min_of", "fmin"):
                return min(args)
            if name in ("max", "max_of", "fmax"):
                return max(args)
            raise Wait(f"a constant computed with {name}()")
        if k == "CXXDefaultInitExpr" or k == "ImplicitValueInitExpr" or k == "CXXScalarValueInitExpr":
            return 0.0
        raise Wait(f"a constant written as {k}")

    def decimal(self, n):
        """A constant number as a Blueprint has it: a literal as it is spelled, a computed 32-bit
        constant as the shortest decimal that is this 32-bit number."""
        b = bare(n)
        sign = 1.0
        while True:
            if b.get("kind") in CAST and kids(b) and NUMBER.get(self.unit.clean(qtype(b))) == "Float":
                b = bare(kids(b)[-1])
            elif b.get("kind") == "UnaryOperator" and b.get("opcode") in ("-", "+") and kids(b):
                sign = -sign if b["opcode"] == "-" else sign
                b = bare(kids(b)[0])
            else:
                break
        if b.get("kind") in ("FloatingLiteral", "IntegerLiteral"):
            return sign * self.number(b)
        v = self.number(n)
        return f32_decimal(v) if self.unit.clean(qtype(bare(n))) == "float" else v

    def literal(self, n, t):
        """The value of a constant expression, written as a literal of the Blueprint type t."""
        u = self.unit
        n = bare(n)
        k = n.get("kind")
        while k in ("ImplicitCastExpr", "CXXFunctionalCastExpr", "CXXStaticCastExpr") and t not in ("Float", "Int", "Bool") and kids(n):
            n = bare(kids(n)[-1])
            k = n.get("kind")
        if t == "Float":
            return flit(self.decimal(n))
        if t == "Int":
            return str(int(self.number(n)))
        if t == "Bool":
            return "true" if self.number(n) != 0 else "false"
        if t == "Text":
            if k == "StringLiteral":
                return text_literal(c_string(n["value"]))
            if k in ("CXXConstructExpr", "InitListExpr") and not kids(n):
                return '""'
            if k == "CXXConstructExpr" and len(kids(n)) == 1:
                return self.literal(kids(n)[0], t)
            raise Wait("a text constant that is not a literal")
        if t == "Vector":
            if k in ("CXXConstructExpr", "InitListExpr", "CXXTemporaryObjectExpr"):
                args = [c for c in kids(n) if c.get("kind") != "CXXDefaultArgExpr"]
                if len(args) == 1 and u.clean(qtype(args[0])) in ("kern::Vec3", "kern::Vec2"):
                    return self.literal(args[0], t)
                vals = [flit(self.decimal(c)) for c in args]
                if len(vals) not in (0, 2, 3):
                    raise Wait("a vector constant of another shape")
                vals += ["0.0"] * (3 - len(vals))
                return "Vector{" + ", ".join(vals) + "}"
            if k == "CXXDefaultInitExpr" or k == "ImplicitValueInitExpr":
                return ZERO["Vector"]
            raise Wait(f"a vector constant written as {k}")
        if t in u.enum_values:
            if k == "DeclRefExpr" and n.get("referencedDecl", {}).get("kind") == "EnumConstantDecl":
                return f"{t}::{enum_value(u, t, n['referencedDecl']['name'])}"
            if k in ("CXXDefaultInitExpr", "ImplicitValueInitExpr", "InitListExpr") and not kids(n):
                return u.zero(t)
            raise Wait("an enum constant that is not one of its values")
        if t in u.struct_fields:
            if k in ("InitListExpr", "CXXConstructExpr", "CXXTemporaryObjectExpr", "CXXDefaultInitExpr", "ImplicitValueInitExpr"):
                args = kids(n)
                if k == "CXXConstructExpr" and args:
                    raise Wait("a struct constant made by a constructor")
                fields = u.struct_fields[t]
                vals = []
                for i, (f, ft, d, _) in enumerate(fields):
                    if i < len(args) and args[i].get("kind") not in ("ImplicitValueInitExpr", "CXXDefaultInitExpr"):
                        vals.append(self.literal(args[i], ft))
                    else:
                        vals.append(d)
                return t + "{" + ", ".join(vals) + "}"
            raise Wait(f"a struct constant written as {k}")
        if t.startswith("List<"):
            it = t[5:-1]
            if k in ("InitListExpr", "CXXConstructExpr"):
                args = kids(n)
                if k == "CXXConstructExpr":
                    if len(args) == 1 and bare(args[0]).get("kind") in ("InitListExpr", "CXXStdInitializerListExpr"):
                        return self.literal(args[0], t)
                    if args:
                        raise Wait("a list constant made by a constructor")
                return t + "{" + ", ".join(self.literal(c, it) for c in args) + "}"
            if k == "CXXStdInitializerListExpr":
                return self.literal(kids(n)[0], t)
            raise Wait(f"a list constant written as {k}")
        raise Wait(f"a constant of type {t}")


def enum_value(unit, enum_name, cpp_value):
    return cpp_value + "_" if cpp_value in CPP_WORDS or NODE_NAME.match(cpp_value) else cpp_value


def callee(n):
    c = kids(n)[0] if kids(n) else {}
    while c.get("kind") in ("ImplicitCastExpr", "ParenExpr") and kids(c):
        c = kids(c)[0]
    return c


def callee_name(n):
    c = callee(n)
    return c.get("referencedDecl", {}).get("name", c.get("name", "?"))


# ---- a function's body -------------------------------------------------------------------------------
CAST = ("ImplicitCastExpr", "CXXStaticCastExpr", "CStyleCastExpr", "CXXFunctionalCastExpr")
COMPARE = ("<", "<=", ">", ">=", "==", "!=")


class Emit:
    def __init__(self, unit, f):
        self.u = unit
        self.f = f
        self.tree = unit.tree
        self.names = {}       # decl id -> the Blueprint name of a parameter or local
        self.types = {}       # decl id -> its Blueprint type
        self.locals = []      # (name, type): declared at the top of the body
        self.taken = set()
        self.index = {}       # decl id of a loop's counter -> "n12.index"
        self.result_struct = None

    # ---- types ----
    def tp(self, n):
        return self.u.btype(qtype(n))

    def ctype(self, n):
        return self.u.clean(qtype(n))

    def is_vec(self, n):
        return self.ctype(n) in ("kern::Vec3", "kern::Vec2")

    def local(self, decl, want_type=None):
        t = want_type or self.u.btype(qtype(decl))
        name = self.u.fresh(decl.get("name") or "v", self.taken)
        self.taken.add(name)
        self.names[decl["id"]] = name
        self.types[decl["id"]] = t
        self.locals.append((name, t))
        return name

    def temp(self, want, t):
        name = self.u.fresh(want, self.taken)
        self.taken.add(name)
        self.locals.append((name, t))
        return name

    # ---- the function ----
    def run(self):
        d, f, u = self.f.decl, self.f, self.u
        if d.get("kind") == "CXXConstructorDecl":
            raise Wait("a constructor")
        if d.get("kind") == "CXXMethodDecl" and not d.get("storageClass") == "static":
            if any(x.get("kind") == "CXXThisExpr" for x in walk(d)):
                raise Wait("the state of its object (a method)")
        if d.get("variadic"):
            raise Wait("a variadic function")
        ret = qtype(d).split("(")[0].strip()
        f.result = u.btype(ret)
        body = next(c for c in kids(d) if c.get("kind") == "CompoundStmt")
        assigned = self.assigned(body)
        copies = []
        for p in kids(d):
            if p.get("kind") != "ParmVarDecl":
                continue
            raw = qtype(p)
            if "&" in raw and not re.search(r"\bconst\b", raw):
                raise Wait("a parameter the function writes through (a reference)")
            t = u.btype(raw)
            name = u.fresh(p.get("name") or f"arg{len(f.params) + 1}", self.taken)
            self.taken.add(name)
            f.params.append((name, t))
            self.names[p["id"]] = name
            self.types[p["id"]] = t
            if p["id"] in assigned:  # (a function's input is not a variable: what the body changes is a local copy)
                copies.append((p, name, t))
        lines = []
        for p, name, t in copies:
            c = self.temp(name + "_", t)
            self.names[p["id"]] = c
            lines.append(f"{c} = {name};")
        lines += self.block(kids(body))
        if not self.pure_ok:
            f.pure = False
        head = []
        doc = u.comment_of(d)
        if f.result is not None:
            head.append(f"  struct {f.name}Result {{ {f.result} Result = {u.zero(f.result)}; }};")
        if doc:
            head.append(f"  /// {doc}")
        params = ", ".join(f"{t} {n} = {u.zero(t)}" for n, t in f.params)
        head.append(f"  {'KERN_PURE ' if f.pure else ''}{f.name + 'Result' if f.result is not None else 'void'} {f.name}({params}) {{")
        out = head + [f"    {t} {n} = {u.zero(t)};" for n, t in self.locals] + ["    " + ln for ln in lines] + ["  }"]
        f.text = "\n".join(out)

    pure_ok = True

    def assigned(self, body):
        out = set()
        for n in walk(body):
            k = n.get("kind")
            target = None
            if k in ("BinaryOperator", "CompoundAssignOperator") and (n.get("opcode") == "=" or k == "CompoundAssignOperator"):
                target = kids(n)[0]
            elif k == "UnaryOperator" and n.get("opcode") in ("++", "--"):
                target = kids(n)[0]
            elif k == "CXXOperatorCallExpr" and callee_name(n) in ("operator=", "operator+=", "operator-=", "operator*=", "operator/="):
                target = kids(n)[1]
            elif k == "CXXMemberCallExpr" and callee(n).get("name") in ("push_back", "emplace_back", "clear", "pop_back", "resize", "erase", "insert", "assign"):
                target = kids(callee(n))[0] if kids(callee(n)) else None
            while target is not None:
                target = bare(target)
                if target.get("kind") == "DeclRefExpr":
                    out.add(target.get("referencedDecl", {}).get("id"))
                    break
                if target.get("kind") in ("MemberExpr", "ArraySubscriptExpr", "ImplicitCastExpr") and kids(target):
                    target = kids(target)[0]
                elif target.get("kind") == "CXXOperatorCallExpr" and callee_name(target) == "operator[]":
                    target = kids(target)[1]
                else:
                    break
        return out

    # ---- statements ----
    def block(self, stmts):
        out = []
        for i, s in enumerate(stmts):
            lines, ends = self.stmt(s)
            out += lines
            if ends:
                break
        return out

    def ends(self, stmts):
        """Does a list of statements always return?"""
        for s in stmts:
            k = s.get("kind")
            if k == "ReturnStmt":
                return True
            if k == "CompoundStmt" and self.ends(kids(s)):
                return True
            if k == "IfStmt":
                c = kids(s)
                if len(c) == 3 and self.ends([c[1]]) and self.ends([c[2]]):
                    return True
        return False

    def stmt(self, s):
        """The lines of one statement, and whether the path ends there."""
        k = s.get("kind")
        if k == "CompoundStmt":
            lines = self.block(kids(s))
            return lines, self.ends(kids(s))
        if k == "NullStmt":
            return [], False
        if k == "DeclStmt":
            out = []
            for v in kids(s):
                if v.get("kind") in ("StaticAssertDecl", "UsingDecl", "TypeAliasDecl", "TypedefDecl"):
                    continue
                if v.get("kind") != "VarDecl":
                    raise Wait(f"a declaration in a body: {v.get('kind')}")
                if v.get("storageClass") == "static":
                    if v.get("constexpr") or re.search(r"\bconst\b", qtype(v)):
                        self.u.constant(v)
                        continue
                    raise Wait("a static local variable")
                raw = qtype(v)
                if "&" in raw and not re.search(r"\bconst\b", raw):
                    raise Wait("a reference to something it changes")
                name = self.local(v)
                init = kids(v)[-1] if kids(v) and v.get("init") else None
                if init is not None:
                    out.append(f"{name} = {self.ex(init, self.types[v['id']])};")
                else:
                    out.append(f"{name} = {self.u.zero(self.types[v['id']])};")
            return out, False
        if k == "ReturnStmt":
            c = kids(s)
            if not c:
                return ["return;"], True
            return [f"return {{{self.ex(c[0], self.f.result)}}};"], True
        if k == "IfStmt":
            if s.get("hasInit") or s.get("hasVar"):
                raise Wait("an if that declares a variable")
            c = kids(s)
            cond = self.bx(c[0])
            then = self.block([c[1]])
            out = [f"if ({cond}) {{", *["  " + ln for ln in then]]
            if len(c) > 2:
                other = self.block([c[2]])
                if other and other[0].startswith("if (") and bare(c[2]).get("kind") == "IfStmt":
                    out.append("} else " + other[0])
                    out += other[1:]
                    return out, self.ends([s])
                out += ["} else {", *["  " + ln for ln in other]]
            out.append("}")
            return out, self.ends([s])
        if k == "ForStmt":
            return self.for_loop(s), False
        if k == "WhileStmt":
            c = kids(s)
            self.no_jumps(c[-1])
            cond = self.bx(c[0])
            body = self.block([c[-1]])
            return [f"while (loop({cond})) {{", *["  " + ln for ln in body], "}"], False
        if k == "CXXForRangeStmt":
            return self.for_each(s), False
        if k == "SwitchStmt":
            return self.switch(s)
        if k in ("BreakStmt", "ContinueStmt"):
            raise Wait("break / continue in a loop")
        if k == "DoStmt":
            raise Wait("a do-while loop")
        if k in ("GotoStmt", "LabelStmt", "CXXTryStmt"):
            raise Wait(k)
        return self.effect(s), False

    def no_jumps(self, body):
        def scan(n, in_switch):
            k = n.get("kind")
            if k == "ContinueStmt" or (k == "BreakStmt" and not in_switch):
                raise Wait("break / continue in a loop")
            if k in ("ForStmt", "WhileStmt", "DoStmt", "CXXForRangeStmt", "LambdaExpr"):
                return
            for c in kids(n):
                scan(c, in_switch or k == "SwitchStmt")
        scan(body, False)

    def for_loop(self, s):
        init, _, cond, inc, body = (s.get("inner") or [{}] * 5)[:5]
        self.no_jumps(body)
        # the counting loop: for (int i = a; i < b; ++i), with i only read in the body
        v = None
        if init.get("kind") == "DeclStmt" and len(kids(init)) == 1 and kids(init)[0].get("kind") == "VarDecl":
            v = kids(init)[0]
        c = bare(cond) if cond else {}
        i = bare(inc) if inc else {}
        counting = (v is not None and NUMBER.get(self.ctype(v)) == "Int" and v.get("init") and c.get("kind") == "BinaryOperator" and c.get("opcode") in ("<", "<=")
                    and self.refers(kids(c)[0], v["id"]) and i.get("kind") == "UnaryOperator" and i.get("opcode") == "++" and self.refers(kids(i)[0], v["id"])
                    and v["id"] not in self.assigned(body) and not any(x in self.assigned(body) for x in self.reads(kids(c)[1])))
        if counting:
            node = self.u.next_node
            self.u.next_node += 1
            first = self.ex(kids(v)[-1], "Int")
            last = self.ex(kids(c)[1], "Int")
            if c["opcode"] == "<":
                last = self.minus_one(kids(c)[1], last)
            self.u.members.append(f"  struct : kern::Node<{node}> {{ Int index = 0; }} n{node};")
            self.names[v["id"]] = f"n{node}.index"
            self.types[v["id"]] = "Int"
            lines = self.block([body])
            return [f"for (auto it{node} = flow_for(n{node}, {first}, {last}); it{node}.next();) {{", *["  " + ln for ln in lines], "}"]
        out = []
        if init:
            out += self.stmt(init)[0]
        condition = self.bx(cond) if cond else "true"
        lines = self.block([body])
        if inc:
            lines += self.effect(inc)
        return out + [f"while (loop({condition})) {{", *["  " + ln for ln in lines], "}"]

    def minus_one(self, n, text):
        b = bare(n)
        while b.get("kind") in CAST and kids(b):
            b = bare(kids(b)[-1])
        if b.get("kind") == "IntegerLiteral":
            return str(int(b["value"]) - 1)
        return f"kern::to<Int>(kern::formula([](Float n) -> Float {{ return n - 1.0; }}, kern::to<Float>({text})))"

    def refers(self, n, decl_id):
        n = bare(n)
        while n.get("kind") == "ImplicitCastExpr" and kids(n):
            n = bare(kids(n)[0])
        return n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") == decl_id

    def reads(self, n):
        return {x.get("referencedDecl", {}).get("id") for x in walk(n) if x.get("kind") == "DeclRefExpr"}

    def for_each(self, s):
        c = kids(s)
        body = c[-1]
        self.no_jumps(body)
        var = next((x for x in c if x.get("kind") == "DeclStmt" and kids(x) and kids(x)[0].get("kind") == "VarDecl" and not kids(x)[0].get("isImplicit")), None)
        rng = next((x for x in c if x.get("kind") == "DeclStmt" and kids(x) and kids(x)[0].get("name", "").startswith("__range")), None)
        if var is None or rng is None:
            raise Wait("a loop over a range of another shape")
        v = kids(var)[0]
        raw = qtype(v)
        if "&" in raw and not re.search(r"\bconst\b", raw):
            raise Wait("a loop that changes the entries of its list")
        src = kids(kids(rng)[0])[-1]
        lt = self.tp(src)
        if not lt.startswith("List<"):
            raise Wait("a loop over something that is not a list")
        it = lt[5:-1]
        node = self.u.next_node
        self.u.next_node += 1
        self.u.members.append(f"  struct : kern::Node<{node}> {{ {it} item = {self.u.zero(it)}; Int index = 0; }} n{node};")
        self.names[v["id"]] = f"n{node}.item"
        self.types[v["id"]] = it
        lst = self.ex(src, lt)
        lines = self.block([body])
        return [f"for (auto it{node} = flow_for_each<{it}>(n{node}, {lst}); it{node}.next();) {{", *["  " + ln for ln in lines], "}"]

    def switch(self, s):
        c = kids(s)
        if s.get("hasInit") or s.get("hasVar"):
            raise Wait("a switch that declares a variable")
        sel, body = c[0], c[-1]
        st = self.tp(sel)
        # the cases: [labels], statements - each must end with break or return (no fall through)
        cases, cur = [], None
        def flat(n, labels):
            k = n.get("kind")
            if k == "CaseStmt":
                kk = kids(n)
                return flat(kk[-1], labels + [kk[0]])
            if k == "DefaultStmt":
                return flat(kids(n)[-1], labels + [None])
            return labels, n
        for x in kids(body):
            if x.get("kind") in ("CaseStmt", "DefaultStmt"):
                labels, first = flat(x, [])
                cur = [labels, [first]]
                cases.append(cur)
            elif cur is None:
                raise Wait("a switch of another shape")
            else:
                cur[1].append(x)
        out_cases = []
        all_end = True
        for labels, stmts in cases:
            body_stmts, closed = [], False
            for x in stmts:
                if x.get("kind") == "BreakStmt":
                    closed = True
                    break
                if x.get("kind") == "CompoundStmt" and kids(x) and kids(x)[-1].get("kind") == "BreakStmt" and not self.ends(kids(x)[:-1]):
                    body_stmts += kids(x)[:-1]
                    closed = True
                    break
                body_stmts.append(x)
                if self.ends([x]):
                    closed = True
                    break
            if not closed and (labels, stmts) is not (cases[-1][0], cases[-1][1]) and cases[-1] is not None and labels is not cases[-1][0]:
                raise Wait("a switch case that falls through")
            for x in body_stmts:
                self.no_switch_break(x)
            ends = self.ends(body_stmts)
            all_end = all_end and ends
            out_cases.append((labels, body_stmts, ends))
        has_default = any(None in labels for labels, _, _ in out_cases)
        if st in self.u.enum_values:
            value = self.ex(sel, st)
            lines = [f"switch (flow_switch_enum<{st}>({value})) {{"]
            covered = set()
            default_body = next((b for labels, b, _ in out_cases if None in labels), None)
            def one(name, stmts):
                body = self.block(stmts) if stmts is not None else []
                return [f"  case {st}::{name}: {{", *["    " + ln for ln in body], "  } break;"]
            by_value = {}
            for labels, stmts, _ in out_cases:
                for lab in labels:
                    if lab is None:
                        continue
                    b = bare(lab)
                    while b.get("kind") in CAST + ("ConstantExpr",) and kids(b):
                        b = bare(kids(b)[-1])
                    if b.get("kind") != "DeclRefExpr":
                        raise Wait("a case that is not an enum value")
                    by_value[enum_value(self.u, st, b["referencedDecl"]["name"])] = stmts
            for name in self.u.enum_values[st]:
                if name in by_value:
                    lines += one(name, by_value[name])
                elif default_body is not None:
                    lines += one(name, default_body)
                else:
                    lines += one(name, None)
            lines.append("}")
            return lines, all_end and (has_default or len(by_value) == len(self.u.enum_values[st]))
        if st != "Int":
            raise Wait(f"a switch on {st}")
        sel_name = self.temp("selected", "Int")
        lines = [f"{sel_name} = {self.ex(sel, 'Int')};"]
        first = True
        default = None
        tail = 0
        for labels, stmts, _ in out_cases:
            if None in labels:
                default = stmts
            vals = [lab for lab in labels if lab is not None]
            if not vals:
                continue
            conds = [f'math_compare<"==">(kern::to<Float>({sel_name}), {flit(Const(self.u).number(lab))})' for lab in vals]
            cond = conds[0]
            for x in conds[1:]:
                cond = f"logic_or({cond}, {x})"
            body = self.block(stmts)
            lines.append(("if" if first else "} else if") + f" ({cond}) {{")
            lines += ["  " + ln for ln in body]
            first = False
        if default is not None:
            body = self.block(default)
            if first:
                lines += body
            else:
                lines += ["} else {", *["  " + ln for ln in body]]
        if not first:
            lines.append("}")
        return lines, all_end and has_default

    def no_switch_break(self, n):
        k = n.get("kind")
        if k == "BreakStmt":
            raise Wait("a break in the middle of a switch case")
        if k in ("ForStmt", "WhileStmt", "DoStmt", "CXXForRangeStmt", "SwitchStmt", "LambdaExpr"):
            return
        for c in kids(n):
            self.no_switch_break(c)

    # ---- statements that change something ----
    def effect(self, s):
        n = bare(s)
        k = n.get("kind")
        if k in CAST and self.ctype(n) == "void":
            return []  # (void)x;
        if k == "BinaryOperator" and n.get("opcode") == "=":
            a, b = kids(n)
            return self.assign(a, lambda t: self.ex(b, t))
        if k == "BinaryOperator" and n.get("opcode") == ",":
            a, b = kids(n)
            return self.effect(a) + self.effect(b)
        if k == "CompoundAssignOperator":
            a, b = kids(n)
            op = n["opcode"][:-1]
            ta = self.tp(a)
            if op == "+" and self.plain_variable(a) and ta in ("Float", "Int"):
                return [f"{self.lvalue(a)} += {self.ex(b, ta)};"]
            synth = {"kind": "BinaryOperator", "opcode": op, "type": n.get("type", {}) if "computeResultType" not in n else n["computeResultType"], "inner": [a, b]}
            return self.assign(a, lambda t: self.ex(synth, t))
        if k == "UnaryOperator" and n.get("opcode") in ("++", "--"):
            a = kids(n)[0]
            ta = self.tp(a)
            if n["opcode"] == "++" and self.plain_variable(a) and ta in ("Float", "Int"):
                return [f"{self.lvalue(a)} += {'1' if ta == 'Int' else '1.0'};"]
            one = {"kind": "IntegerLiteral", "value": "1", "type": {"qualType": "int"}}
            synth = {"kind": "BinaryOperator", "opcode": "+" if n["opcode"] == "++" else "-", "type": a.get("type", {}), "inner": [a, one]}
            return self.assign(a, lambda t: self.ex(synth, t))
        if k == "CXXOperatorCallExpr":
            name = callee_name(n)
            ops = kids(n)[1:]
            if name == "operator=":
                return self.assign(ops[0], lambda t: self.ex(ops[1], t))
            if name in ("operator+=", "operator-=", "operator*=", "operator/=") and self.is_vec(ops[0]):
                synth = {"kind": "CXXOperatorCallExpr", "type": ops[0].get("type", {}), "_op": "operator" + name[8], "inner": [kids(n)[0], ops[0], ops[1]]}
                return self.assign(ops[0], lambda t: self.ex(synth, t))
            if name == "operator+=" and self.tp(ops[0]) == "Text":
                return self.assign(ops[0], lambda t: f'text_append({self.ex(ops[0], "Text")}, {self.ex(ops[1], "Text")}, "")')
        if k == "CallExpr":
            name = callee_name(n)
            ref = callee(n).get("referencedDecl", {})
            if ref.get("id") in self.tree.decl or ref.get("id") in self.tree.body:
                g = self.u.function(ref["id"])
                if g.pure:
                    return []  # (a value nobody reads)
                self.pure_ok = False
                return [f"{g.name}({self.args(n, g)});"]
            raise Wait(f"call: {name}()")
        if k == "CXXMemberCallExpr":
            m = callee(n)
            base = kids(m)[0] if kids(m) else None
            name = m.get("name")
            if base is not None and self.tp(base).startswith("List<"):
                it = self.tp(base)[5:-1]
                lv = self.list_variable(base)
                args = kids(n)[1:]
                if name in ("push_back", "emplace_back") and len(args) == 1:
                    return [f"list_add({lv}, {self.ex(args[0], it)});"]
                if name == "clear":
                    return [f"list_clear({lv});"]
                if name == "pop_back":
                    return [f"list_remove_at({lv}, kern::to<Int>(kern::formula([](Float n) -> Float {{ return n - 1.0; }}, kern::to<Float>(list_length<{it}>({lv})))));"]
                if name == "reserve":
                    return []
                raise Wait(f"list: {name}()")
            raise Wait(f"call: {name}() of an object")
        raise Wait(f"a statement: {k}")

    def plain_variable(self, n):
        n = bare(n)
        return n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") in self.names and "." not in self.names[n["referencedDecl"]["id"]]

    def lvalue(self, n):
        n = bare(n)
        if n.get("kind") == "DeclRefExpr":
            i = n.get("referencedDecl", {}).get("id")
            if i in self.names:
                if "." in self.names[i]:
                    raise Wait("a loop's own counter or item is changed")
                return self.names[i]
            raise Wait("state outside the function is changed")
        raise Wait("an assignment to something that is not a variable")

    def list_variable(self, n):
        n = bare(n)
        if n.get("kind") == "DeclRefExpr":
            return self.lvalue(n)
        raise Wait("a list inside something else is changed")

    def assign(self, target, value):
        """Lines that set a variable, a field of one, a component of a vector or an entry of a list."""
        t = bare(target)
        k = t.get("kind")
        if k == "DeclRefExpr":
            return [f"{self.lvalue(t)} = {value(self.tp(t))};"]
        if k == "MemberExpr":
            base = kids(t)[0]
            bb = bare(base)
            if self.is_vec(base):
                if bb.get("kind") != "DeclRefExpr":
                    raise Wait("a component of a vector inside something else is set")
                v = self.lvalue(bb)
                parts = [f"vector_break({v}).x", f"vector_break({v}).y", f"vector_break({v}).z"]
                at = "xyz".index(t["name"])
                parts[at] = value("Float")
                return [f"{v} = vector_make({', '.join(parts)});"]
            st = self.tp(base)
            if st in self.u.struct_fields and bb.get("kind") == "DeclRefExpr":
                fname, ft = self.u.field(st, t["name"])
                return [f"{self.lvalue(bb)}.{fname} = {value(ft)};"]
            raise Wait("a field inside something else is set")
        if k in ("ArraySubscriptExpr", "CXXOperatorCallExpr"):
            if k == "CXXOperatorCallExpr":
                if callee_name(t) != "operator[]":
                    raise Wait("an assignment to what an operator gives")
                base, idx = kids(t)[1], kids(t)[2]
            else:
                base, idx = kids(t)
            while bare(base).get("kind") == "ImplicitCastExpr":
                base = kids(bare(base))[0]
            lt = self.tp(base)
            if not lt.startswith("List<"):
                raise Wait("an entry of something that is not a list is set")
            return [f"list_set({self.list_variable(base)}, {self.ex(idx, 'Int')}, {value(lt[5:-1])});"]
        raise Wait(f"an assignment to {k}")

    # ---- expressions ----
    def conv(self, text, have, want):
        if want is None or have == want:
            return text
        if want == "Float" and have == "Int" and re.fullmatch(r"-?\d+", text):
            return flit(float(text))
        if want == "Float" and have in ("Int", "Bool"):
            return f"kern::to<Float>({text})"
        if want == "Int" and have in ("Float", "Bool"):
            return f"kern::to<Int>({text})"
        if want == "Bool" and have in ("Float", "Int"):
            return f'math_compare<"!=">({text if have == "Float" else f"kern::to<Float>({text})"}, 0.0)'
        if want == "Int" and have in self.u.enum_values:
            return f"enum_index<{have}>({text})"
        if want == "Float" and have in self.u.enum_values:
            return f"kern::to<Float>(enum_index<{have}>({text}))"
        if want in self.u.enum_values and have in ("Int", "Float"):
            return f"enum_from_index<{want}>({text if have == 'Int' else f'kern::to<Int>({text})'}).value"
        raise Wait(f"a {have} where a {want} is needed")

    def numeric(self, n):
        try:
            return self.tp(n) in ("Float", "Int", "Bool")
        except Wait:
            return False

    def ex(self, n, want=None):
        """An expression as a value of the notation, of the type `want` when one is given."""
        text, have = self.value(n)
        return self.conv(text, have, want)

    def bx(self, n):
        return self.ex(n, "Bool")

    def arithmetic(self, n):
        """Is this a computation with numbers a formula writes (not just a value read somewhere)?"""
        b = bare(n)
        k = b.get("kind")
        if k in CAST:
            if not kids(b) or not self.numeric(b) or not self.numeric(kids(b)[-1]):
                return False
            return self.arithmetic(kids(b)[-1]) or (self.tp(b) == "Int" and self.tp(kids(b)[-1]) == "Float")
        if not self.numeric(b):
            return False
        if k == "BinaryOperator" and b.get("opcode") in ("+", "-", "*", "/", "%"):
            return True
        if k == "UnaryOperator" and b.get("opcode") in ("-", "+"):
            return True
        if k == "ConditionalOperator" and self.tp(b) in ("Float", "Int"):
            return True
        if k == "CallExpr" and callee_name(b) in MATH | {"hypot": 1, "clampf": 1, "lerp": 1, "sign0": 1, "isfinite": 0} and all(self.numeric(a) for a in kids(b)[1:]):
            return callee_name(b) != "isfinite"
        return False

    def value(self, n):
        """(text, Blueprint type) of an expression."""
        u = self.u
        b = bare(n)
        k = b.get("kind")
        if k in ("FloatingLiteral", "IntegerLiteral", "CXXBoolLiteralExpr", "CharacterLiteral"):
            t = self.tp(b)
            v = Const(u).number(b)
            if t == "Float":
                return flit(Const(u).decimal(b)), "Float"
            if t == "Int":
                return str(int(v)), "Int"
            return ("true" if v else "false"), "Bool"
        if k == "StringLiteral":
            return text_literal(c_string(b["value"])), "Text"
        if k in ("CXXNullPtrLiteralExpr", "GNUNullExpr"):
            raise Wait("a pointer")
        if self.arithmetic(b):
            return self.formula(b)
        if k in CAST:
            inner = kids(b)[-1]
            ck = b.get("castKind", "")
            if ck in ("FunctionToPointerDecay", "BuiltinFnToFnPtr"):
                raise Wait("a function as a value")
            if ck == "ToVoid":
                raise Wait("a value thrown away")
            want = self.tp(b)
            text, have = self.value(inner)
            if ck in ("ArrayToPointerDecay",) and have == "Text":
                return text, "Text"
            if want is None:
                raise Wait("a cast to nothing")
            if want == have:
                return text, have
            if have == "Vector" and want == "Vector":
                return text, have
            return self.conv(text, have, want), want
        if k == "DeclRefExpr":
            ref = b.get("referencedDecl", {})
            i = ref.get("id")
            if i in self.names:
                return self.names[i], self.types[i]
            if ref.get("kind") == "EnumConstantDecl":
                d = self.tree.decl.get(i)
                q = d.get("_enum") if d else None
                if q is None:
                    raise Wait("an enum value of an enum without a name")
                e = u.enum(q)
                return f"{e}::{enum_value(u, e, ref['name'])}", e
            if ref.get("kind") == "VarDecl":
                d = self.tree.decl.get(i)
                if d is None:
                    if ref.get("name") == "kPi":
                        return self.engine_constant("kPi", 3.14159265358979323846, "Pi")
                    raise Wait(f"the engine's {ref.get('name')}")
                return u.constant(d)
            raise Wait(f"a name that is {ref.get('kind')}")
        if k == "MemberExpr":
            base = kids(b)[0] if kids(b) else None
            if base is None or bare(base).get("kind") == "CXXThisExpr":
                raise Wait("the state of its object (a method)")
            if b.get("isArrow"):
                raise Wait("a pointer")
            if self.is_vec(base):
                if b.get("name") not in ("x", "y", "z"):
                    raise Wait(f"vector: .{b.get('name')}")
                return f"vector_break({self.ex(base, 'Vector')}).{b['name']}", "Float"
            st = self.tp(base)
            if st in u.struct_fields:
                fname, ft = u.field(st, b["name"])
                return f"struct_break<{st}>({self.ex(base, st)}).{fname}", ft
            raise Wait(f"a member of {st}")
        if k == "UnaryOperator":
            op = b.get("opcode")
            if op == "!":
                return f"logic_not({self.bx(kids(b)[0])})", "Bool"
            if op in ("*", "&"):
                raise Wait("a pointer")
            if op == "~":
                raise Wait("bit arithmetic (& | ^ << >>)")
            if op in ("++", "--"):
                raise Wait("++ / -- inside an expression")
            raise Wait(f"the operator {op}")
        if k == "BinaryOperator":
            op = b.get("opcode")
            x, y = kids(b)
            if op == "&&":
                return f"logic_and({self.bx(x)}, {self.bx(y)})", "Bool"
            if op == "||":
                return f"logic_or({self.bx(x)}, {self.bx(y)})", "Bool"
            if op in COMPARE:
                tx, ty = self.tp(x), self.tp(y)
                if tx in ("Float", "Int") and ty in ("Float", "Int") or (tx in ("Float", "Int", "Bool") and ty in ("Float", "Int", "Bool") and op not in ("==", "!=")):
                    return f'math_compare<"{op}">({self.ex(x, "Float")}, {self.ex(y, "Float")})', "Bool"
                if op in ("==", "!="):
                    if tx == "Bool" or ty == "Bool":
                        r = f"logic_xor({self.bx(x)}, {self.bx(y)})"
                        return (r if op == "!=" else f"logic_not({r})"), "Bool"
                    if tx in u.enum_values and ty == tx:
                        r = f"enum_equal<{tx}>({self.ex(x, tx)}, {self.ex(y, tx)})"
                        return (r if op == "==" else f"logic_not({r})"), "Bool"
                    if tx == "Text" and ty == "Text":
                        raise Wait("a pointer")  # (two const char* compared are addresses)
                raise Wait(f"{tx} {op} {ty}")
            if op in ("&", "|", "^", "<<", ">>"):
                raise Wait("bit arithmetic (& | ^ << >>)")
            if op == ",":
                raise Wait("the comma operator")
            if op == "=":
                raise Wait("an assignment inside an expression")
            raise Wait(f"the operator {op}")
        if k == "CompoundAssignOperator":
            raise Wait("an assignment inside an expression")
        if k == "ConditionalOperator":
            c, x, y = kids(b)
            t = self.tp(b)
            if t is None:
                raise Wait("a choice between nothing")
            return f"logic_select<{t}>({self.bx(c)}, {self.ex(x, t)}, {self.ex(y, t)})", t
        if k == "CallExpr":
            return self.call(b)
        if k == "CXXOperatorCallExpr":
            return self.operator(b)
        if k in ("CXXConstructExpr", "CXXTemporaryObjectExpr", "InitListExpr"):
            return self.construct(b)
        if k == "CXXMemberCallExpr":
            return self.member_call(b)
        if k == "ArraySubscriptExpr":
            base, idx = kids(b)
            while bare(base).get("kind") == "ImplicitCastExpr":
                base = kids(bare(base))[0]
            lt = self.tp(base)
            if not lt.startswith("List<"):
                raise Wait("an entry of something that is not a list")
            self.f.indexes = True
            return f"list_get<{lt[5:-1]}>({self.ex(base, lt)}, {self.ex(idx, 'Int')})", lt[5:-1]
        if k == "CXXDefaultArgExpr":
            raise Wait("a default argument")
        if k in ("ImplicitValueInitExpr", "CXXScalarValueInitExpr", "CXXDefaultInitExpr"):
            t = self.tp(b)
            return u.zero(t), t
        if k == "CXXThisExpr":
            raise Wait("the state of its object (a method)")
        if k == "LambdaExpr":
            raise Wait("a function value (lambda / std::function)")
        if k == "CXXStdInitializerListExpr":
            return self.value(kids(b)[0])
        if k == "UnaryExprOrTypeTraitExpr":
            raise Wait("sizeof")
        raise Wait(f"an expression: {k}")

    def engine_constant(self, cpp, value, name):
        key = "engine:" + cpp
        if key not in self.u.variables:
            n = self.u.fresh(name)
            self.u.names.add(n)
            self.u.variable_lines.append(f"  Float {n} = {flit(f32_decimal(value))};")
            self.u.variables[key] = (n, "Float")
        return self.u.variables[key]

    # ---- formulas ----
    def formula(self, n):
        leaves = collections.OrderedDict()   # value text -> parameter name
        body = self.fx(n, leaves)
        t = self.tp(n)
        if not leaves:
            raise Wait("a computation without inputs")
        params = ", ".join(f"Float {p}" for p in leaves.values())
        text = f"kern::formula([]({params}) -> Float {{ return {body}; }}, {', '.join(leaves)})"
        if t == "Int":
            return f"kern::to<Int>({text})", "Int"
        if t == "Bool":
            return f'math_compare<"!=">({text}, 0.0)', "Bool"
        return text, "Float"

    def leaf(self, n, leaves, text=None):
        if text is None:
            text = self.ex(n, "Float")
        if re.fullmatch(r"-?\d+(\.\d+)?(e[+-]?\d+)?", text):
            return f"({text})" if text.startswith("-") else text
        if text not in leaves:
            inner = re.fullmatch(r"kern::to<Float>\(([\w.]+)\)", text)
            plain = inner.group(1) if inner else text
            base = re.sub(r"\W", "", plain.split(".")[-1]) if re.fullmatch(r"[\w.]+", plain) else ""
            m = re.search(r"\)\.(\w+)$", text)
            if not base and m:
                base = m.group(1)
            base = (base[:1].lower() + base[1:]) if base else "a"
            if base[0].isdigit() or base[0] == "_":
                base = "a" + base
            if base in FORMULA_WORDS or base in CPP_WORDS or NODE_NAME.match(base) or base in ("kern", "result", "Result"):
                base = "the_" + base
            name, i = base, 1
            while name in leaves.values():
                i += 1
                name = f"{base}_{i}"
            leaves[text] = name
        return leaves[text]

    def fx(self, n, leaves):
        """An expression of numbers as the text of a formula over `leaves`."""
        b = bare(n)
        k = b.get("kind")
        if k in ("FloatingLiteral", "IntegerLiteral", "CXXBoolLiteralExpr", "CharacterLiteral"):
            v = Const(self.u).decimal(b)
            return flit(v) if v >= 0 else f"({flit(v)})"
        if k in CAST and kids(b) and self.numeric(b) and self.numeric(kids(b)[-1]):
            inner = kids(b)[-1]
            want, have = self.tp(b), self.tp(inner)
            if want == "Int" and have == "Float":
                return f"kern::math::trunc({self.fx(inner, leaves)})"
            if want == "Bool" and have != "Bool":
                return f"({self.fx(inner, leaves)} != 0.0)"
            if self.ctype(b) in UNSIGNED and self.ctype(inner) not in UNSIGNED and have != "Bool" and not self.known_positive(inner):
                raise Wait("unsigned arithmetic (wraps around at 32 bit)")
            return self.fx(inner, leaves)
        if not self.numeric(b):
            raise Wait("a value that is not a number in a computation")
        if k == "UnaryOperator" and b.get("opcode") in ("-", "+", "!"):
            x = self.fx(kids(b)[0], leaves)
            if b["opcode"] == "-" and self.ctype(b) in UNSIGNED:
                raise Wait("unsigned arithmetic (wraps around at 32 bit)")
            return x if b["opcode"] == "+" else f"({'-' if b['opcode'] == '-' else '!'}{x})"
        if k == "BinaryOperator":
            op = b.get("opcode")
            x, y = kids(b)
            if op in ("+", "-", "*"):
                if self.ctype(b) in UNSIGNED and not (op == "+" and False):
                    raise Wait("unsigned arithmetic (wraps around at 32 bit)")
                return f"({self.fx(x, leaves)} {op} {self.fx(y, leaves)})"
            if op == "/":
                if self.tp(b) == "Int":
                    return f"kern::math::trunc(kern::math::div({self.fx(x, leaves)}, {self.fx(y, leaves)}))"
                return f"kern::math::div({self.fx(x, leaves)}, {self.fx(y, leaves)})"
            if op == "%":
                return f"kern::math::mod({self.fx(x, leaves)}, {self.fx(y, leaves)})"
            if op in COMPARE and self.numeric(x) and self.numeric(y):
                return f"({self.fx(x, leaves)} {op} {self.fx(y, leaves)})"
            if op in ("&&", "||"):
                return f"({self.fx(x, leaves)} {op} {self.fx(y, leaves)})"
            if op in ("&", "|", "^", "<<", ">>"):
                raise Wait("bit arithmetic (& | ^ << >>)")
        if k == "ConditionalOperator":
            c, x, y = kids(b)
            return f"({self.fb(c, leaves)} ? {self.fx(x, leaves)} : {self.fx(y, leaves)})"
        if k == "CallExpr" and all(self.numeric(a) for a in kids(b)[1:]) and (callee_name(b) in MATH or callee_name(b) in ("hypot", "clampf", "lerp", "sign0")):
            name = callee_name(b)
            if name in ("clamp", "clampf") and len(kids(b)) == 4:
                trial = collections.OrderedDict(leaves)
                args = [self.fx(a, trial) for a in kids(b)[1:]]
            else:
                args = [self.fx(a, leaves) for a in kids(b)[1:]]
            if name in ("clamp", "clampf") and len(args) == 3:
                # C++'s clamp, as it is written: v below lo is lo, v above hi is hi (the language's own clamp
                # orders its bounds first - not the same where lo is above hi)
                simple = re.compile(r"\(?-?[\w.]+\)?$")
                if all(simple.match(x) for x in args):
                    v, lo, hi = [self.fx(a, leaves) for a in kids(b)[1:]]
                else:
                    v, lo, hi = "v", "lo", "hi"
                    inner = [self.ex(x, "Float") for x in kids(b)[1:]]
                    return self.leaf(None, leaves, f"kern::formula([](Float v, Float lo, Float hi) -> Float {{ return v < lo ? lo : (hi < v ? hi : v); }}, {', '.join(inner)})")
                return f"({v} < {lo} ? {lo} : ({hi} < {v} ? {hi} : {v}))"
            if name in MATH and len(args) == {"min": 2, "max": 2, "pow": 2, "atan2": 2, "mod": 2}.get(MATH[name], 1):
                return f"kern::math::{MATH[name]}({', '.join(args)})"
            if name == "hypot" and len(args) == 2:
                return f"kern::math::sqrt(({args[0]} * {args[0]}) + ({args[1]} * {args[1]}))"
            if name == "lerp" and len(args) == 3:
                return f"kern::math::lerp({', '.join(args)})"
            if name == "sign0":
                return f"kern::math::sign({args[0]})"
        return self.leaf(b, leaves)

    def fb(self, n, leaves):
        b = bare(n)
        k = b.get("kind")
        if k == "BinaryOperator" and b.get("opcode") in COMPARE + ("&&", "||") and all(self.numeric(c) for c in kids(b)):
            x, y = kids(b)
            if b["opcode"] in ("&&", "||"):
                return f"({self.fb(x, leaves)} {b['opcode']} {self.fb(y, leaves)})"
            return f"({self.fx(x, leaves)} {b['opcode']} {self.fx(y, leaves)})"
        if k == "UnaryOperator" and b.get("opcode") == "!":
            return f"(!{self.fb(kids(b)[0], leaves)})"
        if k in CAST and kids(b) and self.numeric(kids(b)[-1]) and self.tp(kids(b)[-1]) != "Bool":
            return f"({self.fx(kids(b)[-1], leaves)} != 0.0)"
        if k in CAST and kids(b):
            return self.fb(kids(b)[-1], leaves)
        return f"({self.leaf(b, leaves, self.conv(self.bx(b), 'Bool', 'Float'))} != 0.0)"

    def known_positive(self, n):
        b = bare(n)
        return b.get("kind") == "IntegerLiteral"

    # ---- calls ----
    def args(self, n, g):
        a = [x for x in kids(n)[1:]]
        if len(a) != len(g.params):
            raise Wait("a call with default arguments")
        return ", ".join(self.ex(x, t) for x, (_, t) in zip(a, g.params))

    def call(self, b):
        u = self.u
        name = callee_name(b)
        ref = callee(b).get("referencedDecl", {})
        a = kids(b)[1:]
        if any(x.get("kind") == "CXXDefaultArgExpr" for x in a):
            a = [x for x in a if x.get("kind") != "CXXDefaultArgExpr"]
            defaulted = True
        else:
            defaulted = False
        vec = [self.is_vec(x) for x in a]
        if name == "isfinite" and len(a) == 1 and self.numeric(a[0]):
            x = self.ex(a[0], "Float")
            return f'math_compare<"==">(kern::formula([](Float x) -> Float {{ return x - x; }}, {x}), 0.0)', "Bool"
        if ref.get("id") not in self.tree.decl and ref.get("id") not in self.tree.body or name in ("length", "dot", "cross", "distance", "normalize", "lerp", "length_sq", "distance_sq"):
            # the engine's functions of vectors
            if name == "length" and vec == [True]:
                return f"vector_length({self.ex(a[0], 'Vector')})", "Float"
            if name == "length_sq" and vec == [True]:
                v = self.ex(a[0], "Vector")
                return f"vector_dot({v}, {v})", "Float"
            if name == "dot" and vec == [True, True]:
                return f"vector_dot({self.ex(a[0], 'Vector')}, {self.ex(a[1], 'Vector')})", "Float"
            if name == "distance" and vec == [True, True]:
                return f"vector_distance({self.ex(a[0], 'Vector')}, {self.ex(a[1], 'Vector')})", "Float"
            if name == "distance_sq" and vec == [True, True]:
                d = f"vector_sub({self.ex(a[0], 'Vector')}, {self.ex(a[1], 'Vector')})"
                return f"vector_dot({d}, {d})", "Float"
            if name == "cross" and vec == [True, True]:
                if self.ctype(a[0]) == "kern::Vec2":
                    x, y = self.ex(a[0], "Vector"), self.ex(a[1], "Vector")
                    return (f"kern::formula([](Float ax, Float ay, Float bx, Float by) -> Float {{ return ax * by - ay * bx; }}, "
                            f"vector_break({x}).x, vector_break({x}).y, vector_break({y}).x, vector_break({y}).y)"), "Float"
                return f"vector_cross({self.ex(a[0], 'Vector')}, {self.ex(a[1], 'Vector')})", "Vector"
            if name == "lerp" and len(a) == 3 and vec[:2] == [True, True]:
                return f"vector_lerp({self.ex(a[0], 'Vector')}, {self.ex(a[1], 'Vector')}, {self.ex(a[2], 'Float')})", "Vector"
            if name == "normalize" and vec and vec[0]:
                v = self.ex(a[0], "Vector")
                fb = self.ex(a[1], "Vector") if len(a) > 1 else "Vector{0.0, 0.0, 1.0}"
                return f'logic_select<Vector>(math_compare<">">(vector_length({v}), 1e-09), vector_normalize({v}), {fb})', "Vector"
            if name == "finite" and vec == [True]:
                v = self.ex(a[0], "Vector")
                return (f'math_compare<"==">(kern::formula([](Float x, Float y, Float z) -> Float {{ return (x - x) + (y - y) + (z - z); }}, '
                        f"vector_break({v}).x, vector_break({v}).y, vector_break({v}).z), 0.0)"), "Bool"
            if name in ("move", "forward") and len(a) == 1:
                return self.value(a[0])
            if name in ("snprintf", "printf", "sprintf", "to_string", "strtod", "atof", "atoi", "strcmp", "strlen", "strncmp", "strchr", "strstr"):
                raise Wait("text formatting and parsing (snprintf, to_string, atof ...)")
            if name in ("sort", "stable_sort", "remove_if", "find", "find_if", "unique", "any_of", "all_of", "count_if", "swap", "fill", "reverse", "accumulate", "lower_bound", "upper_bound"):
                raise Wait(f"algorithm: std::{name}")
            if name in ("memcpy", "memset", "memcmp"):
                raise Wait("raw memory (memcpy / memset)")
            raise Wait(f"call: {name}()")
        if defaulted:
            raise Wait("a call with default arguments")
        g = u.function(ref["id"])
        self.f.calls.append(g)
        self.f.indexes = self.f.indexes or g.indexes
        if g.result is None:
            raise Wait("a call without a result as a value")
        if not g.pure:
            raise Wait("a call that changes something, inside an expression")
        return f"{g.name}({self.args(b, g)}).Result", g.result

    def operator(self, b):
        name = b.get("_op") or callee_name(b)
        ops = kids(b)[1:]
        if all(self.is_vec(x) or self.numeric(x) for x in ops) and any(self.is_vec(x) for x in ops):
            v = [self.is_vec(x) for x in ops]
            if name in ("operator+", "operator+=") and v == [True, True]:
                return f"vector_add({self.ex(ops[0], 'Vector')}, {self.ex(ops[1], 'Vector')})", "Vector"
            if name in ("operator-", "operator-=") and v == [True, True]:
                return f"vector_sub({self.ex(ops[0], 'Vector')}, {self.ex(ops[1], 'Vector')})", "Vector"
            if name == "operator-" and v == [True]:
                return f"vector_scale({self.ex(ops[0], 'Vector')}, -1.0)", "Vector"
            if name in ("operator*", "operator*=") and v == [True, False]:
                return f"vector_scale({self.ex(ops[0], 'Vector')}, {self.ex(ops[1], 'Float')})", "Vector"
            if name in ("operator*", "operator*=") and v == [True, True]:
                x, y = self.ex(ops[0], "Vector"), self.ex(ops[1], "Vector")
                mul = "kern::formula([](Float a, Float b) -> Float { return a * b; }, "
                return f"vector_make({mul}vector_break({x}).x, vector_break({y}).x), {mul}vector_break({x}).y, vector_break({y}).y), {mul}vector_break({x}).z, vector_break({y}).z))", "Vector"
            if name in ("operator/", "operator/=") and v == [True, False]:
                x, s = self.ex(ops[0], "Vector"), self.ex(ops[1], "Float")
                div = "kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, b); }, "
                return f"vector_make({div}vector_break({x}).x, {s}), {div}vector_break({x}).y, {s}), {div}vector_break({x}).z, {s}))", "Vector"
            if name == "operator[]" and v == [True, False]:
                x, i = self.ex(ops[0], "Vector"), self.ex(ops[1], "Float")
                return f"kern::formula([](Float i, Float x, Float y, Float z) -> Float {{ return i == 0.0 ? x : (i == 1.0 ? y : z); }}, {i}, vector_break({x}).x, vector_break({x}).y, vector_break({x}).z)", "Float"
            if name in ("operator==", "operator!=") and v == [True, True]:
                x, y = self.ex(ops[0], "Vector"), self.ex(ops[1], "Vector")
                same = (f'math_compare<"!=">(kern::formula([](Float ax, Float ay, Float az, Float bx, Float by, Float bz) -> Float {{ return ax == bx && ay == by && az == bz; }}, '
                        f"vector_break({x}).x, vector_break({x}).y, vector_break({x}).z, vector_break({y}).x, vector_break({y}).y, vector_break({y}).z), 0.0)")
                return (same if name == "operator==" else f"logic_not({same})"), "Bool"
            raise Wait(f"vector: {name}")
        t0 = self.tp(ops[0]) if ops else None
        if t0 and t0.startswith("List<") and name == "operator[]":
            self.f.indexes = True
            return f"list_get<{t0[5:-1]}>({self.ex(ops[0], t0)}, {self.ex(ops[1], 'Int')})", t0[5:-1]
        if t0 == "Text":
            if name == "operator+" and self.tp(ops[1]) == "Text":
                return f'text_append({self.ex(ops[0], "Text")}, {self.ex(ops[1], "Text")}, "")', "Text"
            if name in ("operator==", "operator!=") and self.tp(ops[1]) == "Text":
                r = f'text_equal({self.ex(ops[0], "Text")}, {self.ex(ops[1], "Text")})'
                return (r if name == "operator==" else f"logic_not({r})"), "Bool"
        if len(ops) == 2 and self.tp(ops[1]) == "Text" and name in ("operator==", "operator!=") and t0 == "Text":
            pass
        raise Wait(f"the operator {name[8:]} of {t0}")

    def construct(self, b):
        u = self.u
        t = self.tp(b)
        a = [x for x in kids(b) if x.get("kind") != "CXXDefaultArgExpr"]
        if t == "Vector":
            if len(a) == 1 and self.is_vec(a[0]):
                return self.ex(a[0], "Vector"), "Vector"
            if not a:
                return ZERO["Vector"], "Vector"
            if len(a) in (2, 3) and all(self.numeric(x) for x in a):
                parts = [self.ex(x, "Float") for x in a] + ["0.0"] * (3 - len(a))
                if all(re.fullmatch(r"-?[\d.e+-]+", p) for p in parts):
                    return "Vector{" + ", ".join(parts) + "}", "Vector"
                return f"vector_make({', '.join(parts)})", "Vector"
            raise Wait("a vector made of something else")
        if t == "Text":
            if not a:
                return '""', "Text"
            if len(a) == 1 and self.tp(a[0]) == "Text":
                return self.ex(a[0], "Text"), "Text"
            raise Wait("a text made of something else")
        if t in u.struct_fields:
            fields = u.struct_fields[t]
            if b.get("kind") != "InitListExpr":
                if len(a) == 1 and self.tp(a[0]) == t:
                    return self.ex(a[0], t), t
                if a:
                    raise Wait(f"a {t} made by a constructor")
                return u.zero(t), t
            vals = []
            for i, (f, ft, d, _) in enumerate(fields):
                if i < len(a) and a[i].get("kind") not in ("ImplicitValueInitExpr", "CXXDefaultInitExpr"):
                    vals.append(self.ex(a[i], ft))
                else:
                    vals.append(d)
            return f"struct_make<{t}>({', '.join(vals)})", t
        if t in ("Float", "Int", "Bool") and len(a) == 1:
            return self.ex(a[0], t), t
        if t in ("Float", "Int", "Bool") and not a:
            return u.zero(t), t
        if t in u.enum_values and not a:
            return u.zero(t), t
        if t.startswith("List<"):
            it = t[5:-1]
            if not a:
                return t + "{}", t
            if b.get("kind") == "InitListExpr" or (len(a) == 1 and bare(a[0]).get("kind") in ("CXXStdInitializerListExpr", "InitListExpr")):
                items = a if b.get("kind") == "InitListExpr" else kids(bare(a[0]) if bare(a[0]).get("kind") == "InitListExpr" else bare(kids(bare(a[0]))[0]))
                if len(items) > 16:
                    raise Wait("a list made of more than 16 values")
                return f'list_make<{it}, "{len(items)}">({", ".join(self.ex(x, it) for x in items)})', t
            if len(a) == 1 and self.tp(a[0]) == t:
                return self.ex(a[0], t), t
            raise Wait("a list made by a constructor")
        raise Wait(f"a {t} made here")

    def member_call(self, b):
        m = callee(b)
        base = kids(m)[0] if kids(m) else None
        name = m.get("name")
        if base is None or bare(base).get("kind") == "CXXThisExpr":
            raise Wait("the state of its object (a method)")
        bt = self.tp(base)
        a = kids(b)[1:]
        if bt.startswith("List<"):
            it = bt[5:-1]
            lst = self.ex(base, bt)
            if name == "size" and not a:
                return f"list_length<{it}>({lst})", "Int"
            if name == "empty" and not a:
                return f'math_compare<"==">(kern::to<Float>(list_length<{it}>({lst})), 0.0)', "Bool"
            if name in ("at", "front", "back"):
                self.f.indexes = True
            if name in ("at",) and len(a) == 1:
                return f"list_get<{it}>({lst}, {self.ex(a[0], 'Int')})", it
            if name == "front" and not a:
                return f"list_get<{it}>({lst}, 0)", it
            if name == "back" and not a:
                return f"list_get<{it}>({lst}, kern::to<Int>(kern::formula([](Float n) -> Float {{ return n - 1.0; }}, kern::to<Float>(list_length<{it}>({lst})))))", it
            raise Wait(f"list: {name}()")
        if bt == "Text":
            if name in ("size", "length") and not a:
                return f"text_length({self.ex(base, 'Text')})", "Int"
            if name == "empty" and not a:
                return f'math_compare<"==">(kern::to<Float>(text_length({self.ex(base, "Text")})), 0.0)', "Bool"
            if name in ("c_str", "data") and not a:
                return self.ex(base, "Text"), "Text"
            raise Wait(f"text: {name}()")
        raise Wait(f"call: {name}() of a {bt}")


# ---- the test: a converted function next to its C++ -----------------------------------------------
class TestWriter:
    """The test of one converted file (see tests/converted.h of the game): its Blueprint and the C++ it
    was read from are called with the same arguments. The C++ of a function nothing outside its file
    can call (static, or of a source the tests do not link) is copied into the test as it stands."""

    def __init__(self, unit, source_dir):
        self.u = unit
        self.tree = unit.tree
        self.path = unit.tree.path
        self.source_dir = source_dir
        self.helpers = collections.OrderedDict()   # struct (qualified) -> its helper functions

    def local(self, q):
        """Is this type declared in the source file itself (and so copied into the test)?"""
        d = self.tree.records.get(q) or self.tree.enums.get(q)
        return d is not None and os.path.abspath(d.get("_file", "")) == self.path

    def spell(self, raw):
        c = self.u.clean(raw)
        q = self.u.find_type(c)
        if q is not None:
            return q.split("::")[-1] if self.local(q) else "::" + self.tree.namespace + "::" + q.lstrip(":")
        if c in NUMBER or c.startswith("kern::"):
            return c
        raise Wait(f"the test has no values of {c}")

    def draw(self, raw):
        c = self.u.clean(raw)
        if c in ("float", "double"):
            return "draw.number()"
        if c == "bool":
            return "draw.flag()"
        if c in NUMBER:
            return f"static_cast<{c}>(draw.below(40))" if c in UNSIGNED else f"static_cast<{c}>(draw.whole())"
        if c == "kern::Vec3":
            return "draw.vector()"
        if c == "kern::Vec2":
            return "draw.vector2()"
        q = self.u.find_type(c)
        if q in self.tree.enums:
            return f"static_cast<{self.spell(raw)}>(draw.below({len(self.u.enum_values[self.u.enums[q]])}))"
        if q in self.tree.records:
            return f"{self.struct(q)}(draw)"
        raise Wait(f"the test has no values of {c}")

    def comparable(self, raw):
        c = self.u.clean(raw)
        if c in NUMBER or c in ("kern::Vec3", "kern::Vec2") or raw.strip() in ("const char *", "const char *const") or c in TEXT or c.startswith("std::basic_string<char"):
            return True
        q = self.u.find_type(c)
        if q in self.tree.enums:
            return True
        if q in self.tree.records:
            self.struct(q)
            return True
        raise Wait(f"the test cannot compare a {c}")

    def struct(self, q):
        """The helpers of a struct - draw, arg, same, defined, text - and the name of its draw function."""
        short = re.sub(r"\W", "_", q.strip(":"))
        name = f"draw_{short}"
        if q in self.helpers:
            return name
        self.helpers[q] = None
        o = self.tree.records[q]
        t = self.spell(q if not q.startswith("::") else q)
        fields = [(c["name"], qtype(c)) for c in kids(o) if c.get("kind") == "FieldDecl"]
        for _, ft in fields:
            self.comparable(ft)
        lines = [f"[[maybe_unused]] static {t} {name}(converted::Draw& draw) {{", f"  {t} s{{}};"]
        lines += [f"  s.{f} = {self.draw(ft)};" for f, ft in fields]
        lines += ["  return s;", "}"]
        lines.append(f"[[maybe_unused]] static converted::Value arg(const {t}& s) {{ using converted::arg; return converted::Value::structure({{{', '.join(f'arg(s.{f})' for f, _ in fields)}}}); }}")
        lines.append(f"[[maybe_unused]] static bool same(const {t}& want, const converted::Value& got) {{ using converted::same; return got.rows && got.rows->size() == {len(fields)}"
                     + "".join(f" && same(want.{f}, (*got.rows)[{i}])" for i, (f, _) in enumerate(fields)) + "; }")
        lines.append(f"[[maybe_unused]] static {t} near({t} s, int k, int slot) {{ using converted::near; " + " ".join(f"s.{f} = near(s.{f}, k, slot * 7 + {i});" for i, (f, _) in enumerate(fields)) + " return s; }")
        lines.append(f"[[maybe_unused]] static bool defined(const {t}& s) {{ using converted::defined; return " + " && ".join(f"defined(s.{f})" for f, _ in fields) + "; }")
        lines.append(f"[[maybe_unused]] static std::string text(const {t}& s) {{ using converted::text; return std::string(\"{{\") + " + ' + ", " + '.join(f"text(s.{f})" for f, _ in fields) + ' + "}"; }')
        self.helpers[q] = (lines, self.local(q), q)
        return name

    def write(self):
        u, tree = self.u, self.tree
        src = tree.text(self.path)
        linked = '#include "game.h"' not in src
        stem = os.path.splitext(os.path.basename(self.path))[0]
        space = f"converted_{stem}"
        copies = []      # (offset, text)
        wide = []        # the functions once more, to be compiled with 64-bit numbers
        wide_consts = []
        wide_renames = []
        spaces = set()   # the namespaces the functions stand in
        renames = []
        cases = []
        skipped = []
        for f in u.order:
            d = f.decl
            in_file = os.path.abspath(d.get("_file", "")) == self.path
            declared_in_header = False
            pd = d.get("previousDecl")
            while pd:
                prev = tree.decl.get(pd, {})
                if prev.get("_file", "").endswith(".h"):
                    declared_in_header = True
                pd = prev.get("previousDecl")
            if not tree.owner.get(d["id"]):
                fa, fb = d["_at"]
                wide.append(self.widen(tree.text(d["_file"])[fa:fb]))
                if declared_in_header_of(tree, d):
                    wide_renames.append(d["name"])
            call = d.get("name")
            scope = [x for x in tree.scope_of.get(d["id"], "").split("::") if x]
            if scope and not any(x in u.short and any(q in tree.records for q in u.short[x]) for x in scope):
                spaces.add("::".join(scope))
            if tree.owner.get(d["id"]):
                skipped.append((f, "a method"))
                continue
            if in_file and (not declared_in_header or not linked):
                a, b = d["_at"]
                copies.append((a, src[a:b]))
                if declared_in_header:
                    renames.append(d["name"])
                    call = d["name"] + "_as_written"
            if not f.pure or f.result is None:
                skipped.append((f, "it changes something"))
                continue
            if f.indexes:
                # (C++ reads beside an array where the Blueprint gives the type's zero value; which places
                # a function is made for, the test does not know)
                skipped.append((f, "it reads a list at a place it computes: the test does not know the range it is made for"))
                continue
            try:
                params = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
                decls, names = [], []
                for i, p in enumerate(params):
                    self.comparable(qtype(p))
                    decls.append(f"const {self.spell(qtype(p))} a{i} = {self.draw(qtype(p))};")
                    names.append(f"a{i}")
                self.comparable(qtype(d).split("(")[0].strip())
            except Wait as w:
                skipped.append((f, str(w)))
                continue
            shown = ' + ", " + '.join(f"text({n})" for n in names) if names else 'std::string()'
            cases.append("\n".join([
                f"  {{  // {f.cpp}",
                f'    converted::Tally tally("{f.name}");',
                "    for (int i = 0; i < converted::kCalls; ++i) {",
                *[f"      {x}" for x in decls],
                "      const converted::Watch watch;",
                f"      const auto want = {call}({', '.join(names)});",
                "      const bool outside = watch.outside();",
                f"      tally.take(bp, {{{', '.join(f'arg({n})' for n in names)}}}, want, wide::{d['name']}({', '.join(f'wide_arg({n})' for n in names)}), outside, [&] {{ return {shown}; }}, "
                f"[&]([[maybe_unused]] int k) {{ return {call}({', '.join(f'near({n}, k, {i})' for i, n in enumerate(names))}); }});",
                "    }",
                "  }"]))
        if not cases:
            return None, skipped
        for i, (name, t) in u.variables.items():
            d = tree.decl.get(i) if not i.startswith("engine:") else None
            if d is not None and os.path.abspath(d.get("_file", "")) == self.path and not self.inside_function(d):
                a, b = d["_at"]
                if re.search(r"::\s*" + re.escape(d.get("name", "?")) + r"\b", src[a:b].split("=")[0]):
                    continue   # (a class's own constant, defined outside the class: the class has it)
                wide_consts.append((a, self.widen(src[a:b]) + ";"))
                copies.append((a, src[a:b] + ";"))
        for q in list(u.enums) + list(u.structs):
            if self.local(q):
                d = tree.records.get(q) or tree.enums.get(q)
                a, b = d["_at"]
                copies.append((a, src[a:b] + ";"))
        copies.sort()
        merged = []
        for a, t in copies:   # (declarations that share their start: `constexpr f32 kA = 1, kB = 2` - the longest is all of them)
            if merged and merged[-1][0] == a:
                if len(t) > len(merged[-1][1]):
                    merged[-1] = (a, t)
            elif merged and a < merged[-1][0] + len(merged[-1][1]):
                continue
            else:
                merged.append((a, t))
        includes = []
        for m in re.finditer(r'^#include\s+(["<])([^">]+)[">]', src, re.M):
            includes.append(f'#include "{self.source_dir}/{m.group(2)}"' if m.group(1) == '"' and not m.group(2).startswith("kern/") else f"#include {m.group(1)}{m.group(2)}{'>' if m.group(1) == '<' else chr(34)}")
        out = [f"// Generated by tools/cpp_to_blueprint.py from {os.path.basename(self.path)} - do not edit. The Blueprint \"{u.name}\" of the level",
               "// is this file's C++, converted; here every converted function runs next to the C++ it was read from.",
               *includes, '#include "../converted.h"', "",
               f"namespace {tree.namespace} {{"]
        outer = collections.OrderedDict()   # namespace -> the helpers of its structs
        for q, h in self.helpers.items():
            if h and not h[1]:
                parts = [x for x in q.split("::")[:-1] if x]
                while parts and any(r in tree.records for r in u.short.get(parts[-1], [])):
                    parts.pop()
                outer.setdefault("::".join(parts), []).extend(h[0])
        for ns, lines in outer.items():
            out += ([f"namespace {ns} {{"] if ns else []) + lines + ([f"}}  // namespace {ns}"] if ns else [])
        # (the file's own `using` lines: its copied code is written with them)
        usings = [m.group(0).strip() for m in re.finditer(r"^using\s+[^;\n]+;", src, re.M)]
        out += [f"namespace {space} {{", "using namespace converted;", "using converted::arg; using converted::same; using converted::defined; using converted::text; using converted::near;",
                *[f"using ::{tree.namespace}{'::' + ns if ns else ''}::{fn};" for ns in outer for fn in ("arg", "same", "defined", "text", "near")],
                *[f"using namespace ::{tree.namespace}::{x};" for x in sorted(spaces)], *usings]
        if merged:
            out += ["", f"// ---- {os.path.basename(self.path)}, as it stands ----", '#pragma GCC diagnostic push', '#pragma GCC diagnostic ignored "-Wunused-function"',
                    '#pragma GCC diagnostic ignored "-Wunused-const-variable"', '#pragma GCC diagnostic ignored "-Wunused-variable"']
            out += [f"#define {r} {r}_as_written" for r in renames]
            out += [t for _, t in merged]
            out += [f"#undef {r}" for r in renames]
            out += ['#pragma GCC diagnostic pop', ""]
        for q, h in self.helpers.items():
            if h and h[1]:
                out += h[0]
        wide_consts.sort()
        seen_at = set()
        out += ["", "// ---- the same functions with 64-bit numbers, as a Blueprint computes ----", "namespace wide {", "using f32 = double;",
                '#pragma GCC diagnostic push', '#pragma GCC diagnostic ignored "-Wunused-function"', '#pragma GCC diagnostic ignored "-Wunused-const-variable"',
                '#pragma GCC diagnostic ignored "-Wunused-variable"', '#pragma GCC diagnostic ignored "-Wnarrowing"']
        out += [t for a, t in wide_consts if not (a in seen_at or seen_at.add(a))]
        out += wide
        out += ['#pragma GCC diagnostic pop', "}  // namespace wide", ""]
        out += [f"SC_TEST(converted_{stem}) {{", f'  converted::Blueprint bp("{u.name}");', "  converted::Draw draw;", *cases, "}", "", f"}}  // namespace {space}",
                f"}}  // namespace {tree.namespace}", ""]
        return "\n".join(out), skipped

    @staticmethod
    def widen(text):
        """A piece of C++ for its 64-bit run: number literals without the f that makes them 32-bit."""
        return re.sub(r"(?<![\w.])((?:\d+\.\d*|\.\d+|\d+)(?:[eE][+-]?\d+)?)[fF]\b", lambda m: m.group(1) if re.search(r"[.eE]", m.group(1)) else m.group(1) + ".0", text)

    def inside_function(self, d):
        a = d.get("_at", (None,))[0]
        for f in self.u.functions.values():
            fa, fb = f.decl.get("_at", (None, None))
            if fa is not None and a is not None and f.decl.get("_file") == d.get("_file") and fa <= a < fb:
                return True
        return False


def declared_in_header_of(tree, d):
    pd = d.get("previousDecl")
    while pd:
        prev = tree.decl.get(pd, {})
        if prev.get("_file", "").endswith(".h"):
            return True
        pd = prev.get("previousDecl")
    return False


# ---- the program ---------------------------------------------------------------------------------
def actor_name(path):
    stem = os.path.splitext(os.path.basename(path))[0]
    return "".join(p[:1].upper() + p[1:] for p in re.split(r"[_\W]+", stem) if p)


def convert_file(path, namespace, flags):
    """A source file's functions, converted as far as they are; returns the Unit."""
    tree = Tree(path, namespace, flags)
    unit = Unit(tree, actor_name(path))
    mine = [d for i, d in tree.body.items() if i == d["id"] and d.get("_file") and os.path.abspath(d["_file"]) == tree.path and d.get("kind") != "VarDecl"]
    mine.sort(key=lambda d: d.get("_at", (0, 0))[0] or 0)
    unit.all = []
    for d in mine:
        if d.get("isImplicit") or d.get("explicitlyDefaulted") or d.get("kind") == "CXXDestructorDecl":
            continue
        try:
            unit.function(d["id"])
        except Wait:
            pass
        unit.all.append(unit.functions[d["id"]])
    return unit


def apply_to_level(level, folder, blueprints):
    """Writes the Blueprints into the level: each is the Blueprint of the actor of its name (made when
    the level has none), read from its text by the engine's own reader."""
    raw = open(level, encoding="utf-8").read()
    doc = json.loads(raw)
    names = {a["name"]: a for a in doc["actors"]}
    changed = False
    for name, _ in blueprints:
        if name in names:
            if names[name].get("folder") != folder:
                raise SystemExit(f"{level}: the actor {name} is not one of the folder {folder}")
            continue
        doc["actors"].append({"id": doc["nextId"], "name": name, "folder": folder, "transform": {"position": [0, 0, 0], "rotation": [0, 0, 0], "scale": [1, 1, 1]},
                              "components": {"logic": {"kind": "kern-logic", "version": 1, "nextId": 1, "variables": [], "nodes": [], "links": [], "machines": [], "functions": []}}})
        doc["nextId"] += 1
        changed = True
    if changed:
        open(level, "w", encoding="utf-8").write(json.dumps(doc, indent=1, ensure_ascii=False) + "\n")
    tmp = os.path.join(os.environ.get("TMPDIR", "/tmp"), f"kern-cpp-to-blueprint-{os.getpid()}.cpp")
    failed = 0
    for name, text in blueprints:
        open(tmp, "w").write(text)
        r = subprocess.run([os.path.join(ROOT, "tools", "blueprint_cpp.sh"), "--apply", level, name, tmp], capture_output=True, text=True)
        if r.returncode != 0:
            failed += 1
            sys.stderr.write(f"{name}: {(r.stderr or r.stdout).strip()[:1500]}\n")
    os.unlink(tmp)
    if failed:
        raise SystemExit(f"{failed} Blueprint(s) were not read")


def main(argv):
    if len(argv) < 5 or argv[1] not in ("--check", "--convert") or "--" not in argv:
        sys.stderr.write(__doc__)
        return 2
    mode, namespace = argv[1], argv[2]
    cut = argv.index("--")
    opts, files = argv[3:cut], argv[cut + 1:]
    level = folder = test = texts = None
    flags = []
    i = 0
    while i < len(opts):
        if opts[i] in ("--level", "--folder", "--test", "--texts"):
            v = opts[i + 1]
            if opts[i] == "--level": level = v
            elif opts[i] == "--folder": folder = v
            elif opts[i] == "--test": test = v
            else: texts = v
            i += 2
        else:
            flags.append(opts[i])
            i += 1
    total = done = total_lines = done_lines = 0
    waits = collections.Counter()
    wait_lines = collections.Counter()
    units = []
    compared, not_compared, tests_written = 0, collections.Counter(), set()
    for path in files:
        unit = convert_file(path, namespace, flags)
        fs = unit.all
        written = [f for f in fs if f.text]
        total += len(fs)
        done += len(written)
        total_lines += sum(f.lines for f in fs)
        done_lines += sum(f.lines for f in written)
        for f in fs:
            if f.wait:
                waits[f.wait] += 1
                wait_lines[f.wait] += f.lines
        print(f"{os.path.basename(path):28} {len(written):4} / {len(fs):4} functions   {sum(f.lines for f in written):6} / {sum(f.lines for f in fs):6} lines", flush=True)
        if os.environ.get("FUNCTIONS"):
            for f in fs:
                print(f"    {f.cpp} ({f.lines}): " + ("written" if f.text else f.wait))
        if texts and unit.order:
            os.makedirs(texts, exist_ok=True)
            open(os.path.join(texts, unit.name + ".cpp"), "w").write(unit.text())
        if test and unit.order:
            body, skipped = TestWriter(unit, os.path.relpath(os.path.dirname(os.path.abspath(path)), test)).write()
            compared += (body or "").count("converted::Tally tally(")
            for f, why in skipped:
                not_compared[why] += 1
            name = os.path.join(test, "test_" + os.path.splitext(os.path.basename(path))[0] + ".cpp")
            if body:
                os.makedirs(test, exist_ok=True)
                if not os.path.exists(name) or open(name).read() != body:
                    open(name, "w").write(body)
                tests_written.add(os.path.abspath(name))
        units.append((path, unit.name, [(f.name, f.cpp, f.params, f.result, f.pure) for f in unit.order], unit.text() if unit.order else ""))
        unit.tree = None
        del unit
    if level and mode == "--convert":
        apply_to_level(level, folder or "Source", [(u[1], u[3]) for u in units if u[2]])
    print(f"\n{done} of {total} functions are written as Blueprints ({done_lines} of {total_lines} lines), in {sum(1 for u in units if u[2])} of {len(files)} files")
    if test:
        if os.path.isdir(test):
            for name in os.listdir(test):   # (tests of files that have nothing converted any more)
                if name.startswith("test_") and name.endswith(".cpp") and os.path.abspath(os.path.join(test, name)) not in tests_written and len(files) > 20:
                    os.unlink(os.path.join(test, name))
        print(f"{compared} of them are compared with their C++ by the tests in {test}" + ("; not compared: " + ", ".join(f"{n} ({w})" for w, n in not_compared.most_common()) if not_compared else ""))
    print("\nwhat the others wait for: functions (lines)")
    for w, n in sorted(waits.items(), key=lambda kv: -wait_lines[kv[0]])[:int(os.environ.get("TOP", "40"))]:
        print(f"  {n:4} ({wait_lines[w]:6})   {w}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
