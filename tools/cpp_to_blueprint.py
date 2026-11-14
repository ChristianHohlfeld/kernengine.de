#!/usr/bin/env python3
"""C++ to Blueprint: a program's ordinary C++, written as Blueprints.

A Blueprint and its C++ are one thing in two notations (kern/docs/blueprint_cpp.md); the notation
is a subset of C++. A game written in ordinary C++ uses more of the language than that subset.
This tool reads such sources with the compiler's own parser (clang's syntax tree) and writes them
in the notation, function by function:

  tools/cpp_to_blueprint.py --convert <namespace> --level <level.klevel> [--folder <name>]
      [--test <dir>] [--texts <dir>] [--program <folder>=<namespace>[,<namespace>...]]...
      <include flags...> -- <file.cpp>...

  - the program is the folders of the sources, in the namespace given - and the further folders named
    with --program, each with the namespaces its files declare (vendored code the program includes:
    --program firmware=fc,hwcontract). Their headers are the program's like the sources' own, each a
    Blueprint; their types are named with their namespace (fc::V3 is fc_V3, as net::Session is
    net_Session);

  - every header and every source file becomes the Blueprint of one actor of the level (terrain.h
    -> "terrain", terrain.cpp -> "terrain_cpp"): a library - a Blueprint that keeps nothing. Its
    functions are the Blueprint's functions, the constants they use its constants, the types it
    defines the Blueprint's types - whoever computes with them uses it. A header's function that
    needs a source file (it calls what stands in one) stands with the source file of the header's
    name. The types the converter makes of the notation's own (a row of a list of lists, a pair)
    stand in the library "shapes" - or, made of the program's types, with the last of them;
  - a function of another file is called where it stands (common::clampf(v, 0.0, 1.0)): every
    function is written once in the project. Of the headers of the engine, what the program calls
    is written the same way (kern/math/math.h -> "engine_math_math");
  - a type has one name in the whole project, made of its own: the namespaces and classes it
    stands in are part of it (net::Session is net_Session, the engine's physics::Body
    physics_Body) - the engine's nodes name the engine's types the same way;
  - a function is written when everything in it has a place in the notation, and so has what it
    calls; the others stay C++ until the notation (or this tool) has a place for what they use;
  - --test writes, for every file, a test that holds the source of each converted function once
    more, compiled with the numbers and the mathematics of the Blueprint language (f32 is double,
    the functions of numbers are kern::math's), and calls it, the Blueprint in the interpreter and
    the Blueprint as its compiled class with the same arguments: what the three give must be the
    same bit for bit;
  - --texts writes each Blueprint's text into a directory, to read.

How the Blueprint computes is the Blueprint language's own arithmetic: numbers are 64-bit and
whole numbers exact, a division by zero gives 0, a vector's components are 32-bit. It is the same
in the interpreter, in the JIT and in the compiled class, on every machine. Where the C++ brings a
number to 32 bit in so many words, so does the Blueprint (the formula's `single`); unsigned 32-bit
arithmetic and bit operations have formula functions of their own. A 64-bit whole number is exact
while it is within 2^53 of zero; an unsigned one is held read with its sign (WIDE_UNSIGNED). A number
literal is the decimal the source spells (0.1f is 0.1).

A 64-bit number the program spells u64, i64, uint64_t or int64_t is a bit pattern - a key made of
two 32-bit numbers, a hash, the state of a random number generator. In the Blueprint it is the
struct Whole64 of its two halves, and what C++ computes with it the functions of
tools/blueprint_whole64.h compute (w64_add, w64_multiply, w64_shift_left ...): C++ of the kind
this tool converts, written as the library "blueprint_whole64" and compared with their source
like the program's own functions. (A number spelled size_t, long ... is one the program counts
with: the Blueprint's whole number, as above.)

A map is the list of its rows - structs of a key (`first`) and what it holds (`second`) -, a set
the list of its keys, in the order the keys were put in (std::map and std::set: in the order of
the keys). An iterator into a map, a set or a list is the place it stands at, a whole number; the
end is the list's length. `auto [a, b] = value;` gives names to the parts of a value: where they
stand, the part is written out.

A parameter `const T*` a function reads entries through (`p[i]`, `*p`), hands on (`p + 2`) or asks
whether it is null is a pointer into a list, like an iterator: the list, read where it stands (a const
reference input), and the place in it (an Int; null is -1). Its list is of the type every call of the
program hands it - a std::array<T, N> is Fixed<T, N> - and `a.data() + k` hands (a, k).

  tools/cpp_to_blueprint.py --check <namespace> <include flags...> -- <file.cpp>...

only counts: how many functions are written, and what the others wait for.

How a run goes: every source file is read once for the plan of the whole program - which
functions there are, which call which, what the types are called. Then every Blueprint is written
after the Blueprints it calls, with what a call of their functions needs to know; at the end the
libraries of types are put together and all of it is read into the level, a library before the
Blueprints that use it.

Nothing is kept between runs: every run reads the compiler's tree fresh and compiles every test it
writes anew.

Needs clang++ (CLANGXX overrides the binary).
"""
import collections
import copy
import io
import itertools
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile

ROOT = os.environ.get("KERN_ROOT") or os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


class Wait(Exception):
    """What keeps a function C++ for now: something the notation has no place for yet."""


# ---- the notation's words ------------------------------------------------------------------------
CPP_WORDS = set("""alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t char16_t char32_t class compl concept const consteval constexpr
constinit const_cast continue co_await co_return co_yield decltype default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline
int long mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public register reinterpret_cast requires return short signed sizeof static
static_assert static_cast struct switch template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq
Bool Int Float Vector Text Actor List loop machine entry detached kern""".split())
FORMULA_WORDS = set("min max clamp abs sign floor ceil round trunc sqrt pow exp log sin cos tan asin acos atan atan2 rad deg lerp div mod pi band bor bxor shl shr wrap32 wrap32s mul32 single".split())
# What nothing of a Blueprint is called: a word of C++ or of the notation, a function of a formula, a node
# (math_add, list_get ... - the engine says which there are: NODES, see main) or a name of the shape the
# notation gives its nodes and paths (n12, go3 ...).
NODES = set()
NODE_SHAPE = re.compile(r"^(n|go|on|it|machine)\d+$")


def is_word(name):
    return name in CPP_WORDS or name in FORMULA_WORDS or name in NODES or bool(NODE_SHAPE.match(name))

NUMBER = {"bool": "Bool", "float": "Float", "double": "Float", "long double": "Float", "int": "Int", "long": "Int", "long long": "Int", "short": "Int", "signed char": "Int",
          "unsigned char": "Int", "unsigned short": "Int", "unsigned int": "Int", "char": "Int", "unsigned long": "Int", "unsigned long long": "Int"}
W64 = ("whole64u", "whole64s")     # (a type's name, cleaned: a 64-bit bit pattern, unsigned and signed)
# A map is, in a Blueprint, the list of its rows - structs of a key (`first`) and what it holds (`second`) -,
# a set the list of its keys: in the order they were put in (std::map and std::set: in the order of the
# keys). An iterator into one, or into a list, is the place it stands at - a whole number; the end is the
# list's length.
KEYED = re.compile(r"std::(unordered_)?(multi)?(map|set)<")
# kern::Map, kern::Set (kern/core/map.h): the language's Map and Set - values under keys, keys, in the order the keys came
KEYED_KERN = re.compile(r"kern::(Map|Set)<")
ITERATOR = re.compile(r"std::__detail::_Node_(?:const_)?iterator(?:_base)?<|std::_Rb_tree_(?:const_)?iterator<|__gnu_cxx::__normal_iterator<")
UNSIGNED = {"unsigned char", "unsigned short", "unsigned int", "unsigned long", "unsigned long long"}
# An unsigned 64-bit whole number is, in a Blueprint, the whole number its bits are read with a sign
# (2^64 - 1 is -1): what C++ adds, subtracts and multiplies around 2^64 comes out the same, exactly,
# while the numbers are within 2^53 of zero; comparisons and the conversion to a number say it.
WIDE_UNSIGNED = {"unsigned long", "unsigned long long"}
TEXT = {"const char *", "std::basic_string<char>", "std::string", "std::basic_string_view<char>", "std::string_view", "std::basic_string<char, std::char_traits<char>, std::allocator<char>>",
        "std::basic_string_view<char, std::char_traits<char>>"}
# (the names the engine writes these types with, where clang gives the name as written)
ALIAS = {"f32": "float", "f64": "double", "i8": "signed char", "i16": "short", "i32": "int", "i64": "whole64s", "u8": "unsigned char", "u16": "unsigned short", "u32": "unsigned int",
         "u64": "whole64u", "uint64_t": "whole64u", "int64_t": "whole64s", "uint8_t": "unsigned char", "uint16_t": "unsigned short",
         "uint32_t": "unsigned int", "int8_t": "signed char", "int16_t": "short", "int32_t": "int", "usize": "unsigned long", "size_t": "unsigned long", "Vec2": "kern::Vec2", "Vec3": "kern::Vec3", "Vec4": "kern::Vec4", "Quat": "kern::Quat",
         "Mat4": "kern::Mat4"}
ZERO = {"Int64": "Int64{0x0}", "Float": "0.0", "Int": "0", "Bool": "false", "Vector": "Vector{0.0, 0.0, 0.0}", "Text": '""'}
# (functions of numbers a formula has, by the names C++ calls them)
MATH = {"sqrt": "sqrt", "sqrtf": "sqrt", "fabs": "abs", "fabsf": "abs", "abs": "abs", "fmin": "min", "fminf": "min", "min": "min", "min_of": "min", "fmax": "max", "fmaxf": "max", "max": "max",
        "max_of": "max", "floor": "floor", "floorf": "floor", "ceil": "ceil", "ceilf": "ceil", "round": "round", "roundf": "round", "lround": "round", "lroundf": "round", "trunc": "trunc",
        "truncf": "trunc", "pow": "pow", "powf": "pow", "exp": "exp", "expf": "exp", "log": "log", "logf": "log", "sin": "sin", "sinf": "sin", "cos": "cos", "cosf": "cos", "tan": "tan",
        "tanf": "tan", "asin": "asin", "acos": "acos", "atan": "atan", "atan2": "atan2", "atan2f": "atan2", "fmod": "mod", "fmodf": "mod", "clamp": "clamp"}


VECTOR_FUNCTIONS = ("length", "dot", "cross", "distance", "normalize", "lerp", "length_sq", "distance_sq", "finite", "min_of", "max_of", "clamp", "hex_color", "vmin", "vmax")
MORE_MATH = ("hypot", "clampf", "lerp", "sign0", "hash_u32", "ldexp", "ldexpf", "tanh", "tanhf", "sinh", "sinhf", "cosh", "coshf", "asinh", "asinhf", "log2", "log2f", "log10", "log10f", "cbrt", "cbrtf")


# The engine's functions as nodes in converted code (BLUEPRINTS.md, 6c): handles for the engine's objects,
# calls as nodes, steps inside conditional parts of an expression. (ENGINE_NODES=0 leaves them out.)
ENGINE_NODES = os.environ.get("ENGINE_NODES", "1") != "0"


class Hoists(Wait):
    """A part of an expression that C++ does not always evaluate needs steps of its own (see Emit.hoist)."""


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
    for ch in (s if isinstance(s, bytes) else s.encode("utf-8")):
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
    """The bytes of a string literal as clang prints it (a C literal, with its escapes)."""
    if not v.startswith('"'):
        return v.encode("utf-8")
    v = v[1:-1]
    out, i = bytearray(), 0
    simple = {"n": 10, "t": 9, "r": 13, "a": 7, "b": 8, "f": 12, "v": 11, "\\": 92, '"': 34, "'": 39, "?": 63}
    while i < len(v):
        c = v[i]
        if c != "\\":
            out += c.encode("utf-8")
            i += 1
            continue
        e = v[i + 1]
        if e in simple:
            out.append(simple[e])
            i += 2
        elif e == "x":
            j = i + 2
            while j < len(v) and v[j] in "0123456789abcdefABCDEF":
                j += 1
            out.append(int(v[i + 2:j], 16) & 255)
            i = j
        elif e in "01234567":
            j = i + 1
            while j < len(v) and j < i + 4 and v[j] in "01234567":
                j += 1
            out.append(int(v[i + 1:j], 8) & 255)
            i = j
        elif e in "uU":
            n = 4 if e == "u" else 8
            out += chr(int(v[i + 2:i + 2 + n], 16)).encode("utf-8")
            i += 2 + n
        else:
            out += e.encode("utf-8")
            i += 2
    return bytes(out)


# ---- the syntax tree ---------------------------------------------------------------------------
def run_clang(path, namespace, flags):
    """The compiler's tree of the declarations of a namespace, as it reads the file now (nothing is kept
    between runs: a tree of another state of the sources is another tree)."""
    # (The filter is an argument of its own: the runs for the several namespaces of one file differ in that short
    # word alone - what the compiler allocates before it reads the file is then the same in each, and so are the
    # addresses its nodes are named by - see STEADY. Written in one, `-ast-dump-filter=hwcontract::` was longer
    # than `-ast-dump-filter=fc::` by more than the allocator rounds away, and every node had another address.)
    cmd = [os.environ.get("CLANGXX", "clang++"), "-std=c++20", "-fsyntax-only", "-Xclang", "-ast-dump=json", "-Xclang", "-ast-dump-filter", "-Xclang", f"{namespace}::", *flags, path]
    if STEADY:
        cmd = ["setarch", "-R", *cmd]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0 and not r.stdout:
        sys.stderr.write(f"{path}: clang failed\n{r.stderr[:2000]}\n")
        return []
    s = r.stdout
    out, dec, i = [], json.JSONDecoder(), 0
    while True:
        j = s.find("{", i)
        if j < 0:
            break
        o, i = dec.raw_decode(s, j)
        out.append(o)
    return out


# (clang names a node by its address. With address randomization off the addresses are the same in
# every run on the same source - the tree of the program and the tree of the engine it uses, dumped
# by two runs, then name each other's nodes.)
STEADY = subprocess.run(["setarch", "-R", "true"], capture_output=True).returncode == 0 if os.name == "posix" else False


def unit_order(path, flags):
    """Where every line of every file stands in the translation unit: {file: [(line, place)]}, from the
    marks the preprocessor leaves (`# 12 "file"`: the next line is line 12 of that file)."""
    r = subprocess.run([os.environ.get("CLANGXX", "clang++"), "-std=c++20", "-E", *flags, path], capture_output=True, text=True, errors="replace")
    marks, place = [], 0
    for line in r.stdout.split("\n"):
        m = re.match(r'# (\d+) "([^"]*)"', line)
        if m:
            marks.append((m.group(2), int(m.group(1)), place))
        else:
            place += 1
    out = {}
    for file, line, place in marks:
        out.setdefault(os.path.abspath(file), []).append((line, place))
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
    """The parts of a node (without the comments and attributes that stand with a declaration)."""
    return [c for c in n.get("inner", []) or [] if isinstance(c, dict) and c.get("kind") and not c["kind"].endswith("Comment") and not c["kind"].endswith("Attr")]


# A 64-bit whole number the program spells u64, i64, uint64_t or int64_t is a bit pattern: in a Blueprint
# the struct Whole64 (tools/blueprint_whole64.h). The compiler's tree says how a type is spelled (its
# "qualType") next to what it is (its "desugaredQualType", where size_t and u64 are the same): where the
# spelling names one of those, the spelling is the type - unless it also names something only the other
# says (a list's value_type, auto).
WHOLE64 = re.compile(r"(?<![\w:])(?:kern::|std::|sc::)?(u64|i64|uint64_t|int64_t)\b")
UNSPELLED = re.compile(r"::(?:value_type|reference|const_reference|size_type|difference_type|key_type|mapped_type|element_type|iterator|const_iterator|pointer)\b|\bauto\b|\bdecltype\b|\btypename\b|__")


def qtype(n):
    t = n.get("type", {})
    spelled = t.get("qualType", "")
    if spelled and WHOLE64.search(spelled) and not UNSPELLED.search(spelled):
        return spelled
    return t.get("desugaredQualType") or spelled


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


def walk_source(n):
    """The tree as the source has it: where a call of a function that names a place was written out as
    that place (Emit.dissolve), the call - the source calls the function, and so does the test's."""
    if n.get("_call") is not None:
        yield from walk_source(n["_call"])
        return
    yield n
    for c in kids(n):
        yield from walk_source(c)


ENGINE = os.environ.get("KERN_ENGINE_NAMESPACE", "kern")


class Tree:
    """One source file as the compiler read it. `namespace` is the program's own namespace (its names are
    the program's names, without it); `spaces` are further namespaces of the program - of folders that are
    the program's too (World.spaces): their declarations are the program's, named with their namespace
    (`fc::V3`, as `net::Session` is)."""

    def __init__(self, path, namespace, flags, spaces=()):
        self.path = os.path.abspath(path)
        self.namespace = namespace
        self.spaces = tuple(s for s in spaces if s != namespace)
        self.dumps = run_clang(path, namespace, flags)
        st = {"file": ""}
        for o in self.dumps:
            annotate(o, st)
        # (the further namespaces of the program, each dumped as the engine's is: by its own name)
        self.more = [(space, run_clang(path, space, flags) if STEADY else []) for space in self.spaces]
        for _, dumps in self.more:
            st = {"file": ""}
            for o in dumps:
                annotate(o, st)
        # the engine the program is written against: its types and the functions its headers define
        self.engine = run_clang(path, ENGINE, flags) if STEADY and ENGINE and ENGINE != namespace else []
        st = {"file": ""}
        for o in self.engine:
            annotate(o, st)
        self.texts = {}
        self.flags = flags
        self.order = None     # (see `place`)
        self.decl = {}        # id -> the declaration
        self.body = {}        # id -> the declaration with the body (of a function declared earlier)
        self.records = {}     # qualified name -> record
        self.declared = set() # qualified names of the records declared without their definition (struct Tile;)
        self.enums = {}       # qualified name -> enum
        self.owner = {}       # id of a method -> its record's qualified name
        self.scope_of = {}    # id -> the qualified scope it stands in ("sc::combat::")
        self.aliases = {}     # a name the program gives a type (using Bytes32 = ...) -> that type
        self.alias_decls = []
        self.template_of = {}  # a function made from a template (its id) -> the template
        self.unnamed = {}     # how the compiler spells the type of an enum without a name -> its underlying type
        self.local_types = {}  # a type a function declares in its body (qualified name) -> that function's scope
        self.local_spans = []  # (file, from, to, scope) of the functions that declare types of their own
        seen = set()
        for o in self.dumps:
            self.take(o, self.scope_from(o), seen)
        for space, dumps in self.more:
            for o in dumps:
                self.take(o, space + "::", seen)
        for o in self.engine:
            self.take(o, ENGINE + "::", seen)
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
        self.take_locals(seen)
        self.number_names()

    def take_locals(self, seen):
        """The types a function declares in its body - `struct Seg { Vec2 m; f32 len; };` between its
        statements - are types of the program like any other, in the scope of that function: its name is
        part of theirs (Destruction::fracture::Seg). They are known where C++ knows them: in that function
        (Unit.find_type)."""
        done = set()
        for d in list(self.body.values()):
            if d["id"] in done or d.get("_at", (None, None))[1] is None:
                continue
            done.add(d["id"])
            body = next((c for c in kids(d) if c.get("kind") == "CompoundStmt"), None)
            local = [c for n in walk(body) if n.get("kind") == "DeclStmt" for c in kids(n) if c.get("kind") in ("CXXRecordDecl", "EnumDecl")] if body else []
            if not local:
                continue
            owner = self.owner.get(d["id"])
            base = (owner + "::" if owner else self.scope_of.get(d["id"], "")) + (d.get("name") or "function")
            scope, k = base + "::", 2
            while any(q.startswith(scope) for q in self.local_types):   # (a function of the same name: its own scope)
                scope, k = f"{base}{k}::", k + 1
            before = set(self.records) | set(self.enums)
            for c in local:
                c["_local"] = d["id"]
                self.take(c, scope, seen)
            for q in (set(self.records) | set(self.enums)) - before:
                self.local_types[q] = scope
            self.local_spans.append((d.get("_file", ""), d["_at"][0], d["_at"][1], scope))

    def local_scope(self, d):
        """The scope of the types a function declares in its body - the function `d` stands in (itself, or
        the function a local function is written in). "": it stands in none that declares types."""
        a = d.get("_at", (None, None))[0]
        best = ""
        for file, lo, hi, scope in self.local_spans:
            if a is not None and file == d.get("_file", "") and lo <= a < hi and len(scope) > len(best):
                best = scope
        return best

    def number_names(self):
        """A value of an enum without a name is a name for a whole number: `enum : u64 { kCatWorld = 1 };`
        declares constants, and no type anybody names. Where one is used, the tree has the number it is,
        of the enum's underlying type - as C++ converts it there. (`_unnamed`: the enum it is of; the
        source a test holds names it.)"""
        for o in [*self.dumps, *(o for _, dumps in self.more for o in dumps), *self.engine]:
            for n in walk(o):
                if n.get("kind") != "DeclRefExpr" or (n.get("referencedDecl") or {}).get("kind") != "EnumConstantDecl":
                    continue
                c = self.decl.get(n["referencedDecl"].get("id"))
                if c is None or "_number" not in c:
                    continue
                n["kind"], n["value"], n["type"], n["_unnamed"] = "IntegerLiteral", str(c["_number"]), dict(c["_under"]), c["_of"]
                del n["referencedDecl"]

    def scope_from(self, o):
        """The scope a dumped declaration stands in: clang says it for those defined outside their class."""
        return ""

    def program_space(self, q):
        """The further namespace of the program a qualified name stands in (`fc::V3`: fc), or None."""
        parts = q.lstrip(":").split("::")
        return parts[0] if len(parts) > 1 and parts[0] in self.spaces else None

    def absolute(self, q):
        """A qualified name as C++ writes it from outside every namespace: a name of the engine or of a further
        namespace of the program has its namespace in it already, one of the program's own namespace stands
        in that."""
        q = q.lstrip(":")
        return "::" + q if q.startswith((ENGINE + "::", "w64::")) or self.program_space(q) else "::" + self.namespace + "::" + q

    def place(self, d):
        """Where a declaration stands in the translation unit (what stands before it can be used in it)."""
        if self.order is None:
            self.order = unit_order(self.path, self.flags)
        a = d.get("_at", (None,))[0]
        file = d.get("_file", "")
        if a is None or not file:
            return (-1, 0)
        line = self.text(file).count("\n", 0, a) + 1
        best = None
        for first, place in self.order.get(os.path.abspath(file), []):
            if first <= line and (best is None or first >= best[0]):
                best = (first, place)
        return (best[1] + line - best[0], a) if best else (10 ** 9, a)

    def text(self, file):
        """A file's bytes, one character each: the compiler's places in a file count bytes. (What is cut
        out of it is read as the text it is by `said`.)"""
        if file not in self.texts:
            try:
                self.texts[file] = open(file, "rb").read().decode("latin-1")
            except OSError:
                self.texts[file] = ""
        return self.texts[file]

    @staticmethod
    def said(piece):
        return piece.encode("latin-1").decode("utf-8", "replace")

    def source(self, n):
        a, b = n.get("_at", (None, None))
        return self.said(self.text(n.get("_file", ""))[a:b]) if a is not None and b is not None else ""

    def take(self, o, scope, seen):
        k = o.get("kind")
        if "id" in o:
            if o["id"] in seen:
                return
            seen.add(o["id"])
            self.decl[o["id"]] = o
            self.scope_of[o["id"]] = scope
        if k in ("TypeAliasDecl", "TypedefDecl") and o.get("name"):
            self.aliases.setdefault(o["name"], qtype(o))
            self.alias_decls.append((o, scope))
        if k == "FunctionTemplateDecl":
            for c in kids(o):
                if c.get("kind") == "FunctionDecl" and "id" in c:
                    self.template_of[c["id"]] = o
        if k == "NamespaceDecl":
            for c in kids(o):
                self.take(c, scope + (o["name"] + "::" if o.get("name") else ""), seen)
        elif k == "EnumDecl" and o.get("name"):
            q = scope + o["name"]
            self.enums[q] = o
            o["_q"] = q
            for c in kids(o):
                if "id" in c:
                    self.decl[c["id"]] = c
                    c["_enum"] = q
        elif k == "EnumDecl":
            # An enum without a name: its values are whole numbers of its underlying type (number_names).
            numbers = enum_numbers(o)
            under = o.get("fixedUnderlyingType") or {"qualType": enum_underlying([v for _, v in numbers])}
            for c in kids(o):
                if c.get("kind") == "EnumConstantDecl" and "id" in c:
                    self.decl[c["id"]] = c
                    self.scope_of[c["id"]] = scope
                    c["_number"], c["_under"], c["_of"] = dict(numbers)[c["name"]], under, o["id"]
                    spelled = re.search(r"\((?:unnamed|anonymous) enum at [^)]*\)", c.get("type", {}).get("qualType", ""))
                    if spelled:
                        self.unnamed[spelled.group(0)] = qtype({"type": under})
        elif k == "CXXRecordDecl" and o.get("name"):
            outer = self.decl.get(o["parentDeclContextId"]) if o.get("parentDeclContextId") else None
            if outer is not None and outer.get("kind") == "CXXRecordDecl" and outer.get("_q"):
                # (a class of a class defined outside it - `struct CityStream::Tile { ... };`: the class's own)
                scope = outer["_q"] + "::"
                self.scope_of[o["id"]] = scope
            q = scope + o["name"]
            if o.get("completeDefinition"):
                self.records[q] = o
                o["_q"] = q
            elif not o.get("isImplicit"):   # (not the name a class has in itself)
                self.declared.add(q)   # (declared here - defined here too, or not: then `records` has it not)
            for c in kids(o):
                if c.get("kind") in ("CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl", "FieldDecl") and "id" in c:
                    self.owner[c["id"]] = q
                self.take(c, q + "::", seen)
        elif k in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl", "VarDecl", "FieldDecl", "FunctionTemplateDecl", "ClassTemplateDecl", "LinkageSpecDecl"):
            if k in ("CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl") and o.get("parentDeclContextId") and o["id"] not in self.owner:
                r = self.decl.get(o["parentDeclContextId"])
                if r is not None and r.get("_q"):
                    self.owner[o["id"]] = r["_q"]


def enum_underlying(values):
    """The type an enum that names none is held as, as the compilers the program is built with choose it:
    without a negative value the first of unsigned int, unsigned long that has all its values; with one,
    the first of int, long."""
    lo, hi = min(values, default=0), max(values, default=0)
    if lo >= 0:
        return "unsigned int" if hi < 2 ** 32 else "unsigned long"
    return "int" if -2 ** 31 <= lo and hi < 2 ** 31 else "long"


def enum_numbers(enum_decl):
    """The numbers of an enum's values, in their order (C++ counts on from the one before)."""
    out, at = [], 0
    for c in kids(enum_decl):
        if c.get("kind") != "EnumConstantDecl":
            continue
        v = next((x.get("value") for x in walk(c) if x.get("kind") == "ConstantExpr" and "value" in x), None)
        if v is not None:
            at = int(v)
        out.append((c["name"], at))
        at += 1
    return out


def template_args(t):
    """The arguments of a template type's name: "std::pair<int, std::vector<float>>" -> ["int", "std::vector<float>"]."""
    a, b = t.find("<"), t.rfind(">")
    if a < 0 or b < a:
        return []
    out, depth, cur = [], 0, ""
    for ch in t[a + 1:b]:
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
    return out


def qualified_scope(tree, namespace):
    """clang dumps the declarations of a namespace one by one, without the namespace around them: the
    scope of each is read from its qualified type names. The types of the program are found by their
    last names instead (see Unit.find_type)."""
    return namespace + "::"


# ---- the program as a whole ------------------------------------------------------------------------
def sane_name(want):
    """A name the notation can write, made of a name of the program: the same for the same name, wherever
    it is asked. (A name another thing has already is told apart by whoever gives it: Unit.fresh.)"""
    name = re.sub(r"_+", "_", re.sub(r"\W", "_", want))[:56] or "x"
    if name[0].isdigit() or (name[0] == "_" and name[1:2].isupper()):
        name = "v" + name
    if name[:5] in ("kern_", "KERN_"):
        name = "K" + name[1:] if name[0] == "k" else "k" + name
    if is_word(name) or name.endswith("_list") or name.endswith("Result"):
        name = "the_" + name + ("_" if name.endswith("_list") or name.endswith("Result") else "")
    return name


def type_name_of(q):
    """The name a type has in the Blueprints, by its own name alone: the namespaces and classes it stands
    in are part of it (`net::Session` is net_Session, `Car::Wheel` Car_Wheel, the engine's
    `kern::physics::Body` physics_Body) - the program's own namespace and the engine's are not, they are
    where everything stands. The engine's nodes name the engine's types the same way (tools/engine_api.py)."""
    q = q.replace("(anonymous namespace)::", "")
    for outer in (ENGINE + "::", "w64::"):
        if q.startswith(outer):
            q = q[len(outer):]
    return sane_name(q.replace("::", "_"))


def function_key(tree, d):
    """A function's name in the whole program: the one the linker knows it by - and the file it is in,
    where every file has its own (static, in a namespace without a name)."""
    first = d
    while first.get("previousDecl") in tree.decl:
        first = tree.decl[first["previousDecl"]]
    key = d.get("mangledName") or first.get("mangledName") or (tree.scope_of.get(d["id"], "") + d.get("name", "?") + " " + qtype(d))
    # (a static function that is no member; in a namespace without a name the linker's name says so: _GLOBAL__N_)
    own = (d.get("kind") == "FunctionDecl" and "static" in (d.get("storageClass"), first.get("storageClass"))) or "_GLOBAL__N_" in key or "(anonymous" in key
    return key + "@" + os.path.abspath(d.get("_file") or first.get("_file") or "?") if own else key


def own_function(tree, decl_id):
    """Is this a function to write as a Blueprint function - one of the program, or one the engine's
    headers define? (Not what the notation has itself: the vectors' own operations, the functions
    of numbers.)"""
    d = tree.body.get(decl_id) or tree.decl.get(decl_id)
    if d is None:
        return False
    owner = tree.owner.get(d["id"]) or tree.owner.get(decl_id) or ""
    if owner in (ENGINE + "::Vec3", ENGINE + "::Vec2"):
        return False
    if not owner and tree.scope_of.get(d["id"], "").startswith(ENGINE + "::") and d.get("name") in VECTOR_FUNCTIONS:
        return False
    return True


class World:
    """What is known of the whole program, the same for every file that is converted: which Blueprint a
    file's functions stand in, what the types are called, and the functions of the Blueprints that are
    written already."""

    def __init__(self, sources=(), flags=(), more=()):
        # The program is the folders of the sources - and further folders, each with the namespaces its files
        # declare (`more`: [(folder, [namespace])], --program): their files are the program's like the
        # sources' headers, each a Blueprint of its own, their functions the program's functions.
        self.program_dirs = sorted({os.path.dirname(os.path.abspath(f)) for f in sources} | {os.path.abspath(d) for d, _ in more})
        self.more = [(os.path.abspath(d), list(spaces)) for d, spaces in more]
        self.spaces = [s for _, spaces in self.more for s in spaces]   # (the program's further namespaces: Tree.spaces)
        self.include_dirs = sorted((os.path.abspath(f[2:]) for f in flags if f.startswith("-I") and len(f) > 2), key=len, reverse=True)
        self.units = {}        # a file -> the Blueprint its functions stand in
        self.type_names = {}   # (a type's qualified name, the file that defines it) -> its name, where it is not type_name_of's
        self.taken = set()     # the names of all types of the program: nothing else is called so
        self.table = {}        # a function's key -> what a call of it needs to know (of the Blueprints written before)
        self.wanted = None     # of a Blueprint that is not the program's own: the functions to write (keys)
        self.bars = {}         # a function's key -> why it is not written this time
        self.nodes = set()     # the names of the engine's nodes (NODES)
        self.function_units = {}   # a function's key -> the Blueprint it stands in (see unit_of_function)
        self.source_units = set()  # the Blueprints of source files: what stands there is not in another file's tree
        self.actors = {}       # the level's actors with a Blueprint: name -> {"library": does it keep nothing, "functions": {name: ([inputs], [outputs])}}
        self.pointer_lists = {}    # (a function's key, the place of a parameter that is a pointer into a list) -> the length of the list every call hands it ("list": any; "mixed", "other": see main)
        self.states = {}       # a class of the program there is one object of (see state_classes) -> the Blueprint it is the state of
        self.state_functions = set()   # (keys of) the functions that are no methods of a state but functions of its Blueprint: they share a variable of the program (main)
        self.globals = {}      # a variable of a namespace of the program with external linkage (its qualified name) -> does it start at zero (as its definition says)
        self.parts = {}        # a part of a Blueprint of several source files (a job: "Game/game_net_cpp") -> that Blueprint
        self.part_numbers = {} # such a part -> its number among the parts of its Blueprint (the block of its nodes' ids)
        self.closures = None   # with tests: a function's key -> the places of the pieces its test holds, as its file's test hands them on (TestWriter.exports)
        self.pieces = None     # those pieces, by their places

    def program(self, file):
        """Is this a file of the program itself - one of those to convert as a whole?"""
        p = os.path.abspath(file)
        return os.path.dirname(p) in self.program_dirs

    def unit_of(self, file):
        """The Blueprint of a file: a header's is called like the header (car.h: car), a source file's
        says that it is one (car.cpp: car_cpp). A header of the engine or of a library is named by its
        path (kern/math/math.h: engine_math_math)."""
        p = os.path.abspath(file)
        if p not in self.units:
            stem = os.path.splitext(os.path.basename(p))[0]
            if not self.program(p):
                for inc in self.include_dirs:
                    if p.startswith(inc + os.sep):
                        stem = os.path.splitext(p[len(inc) + 1:])[0]
                        break
                if re.match(ENGINE + r"\W", stem):
                    stem = "engine" + stem[len(ENGINE):]
            name = re.sub(r"_+", "_", re.sub(r"\W", "_", stem)).strip("_") or "x"
            if not is_header(p):
                name += "_" + (os.path.splitext(p)[1].lstrip(".") or "source")
            if name[0].isdigit() or is_word(name):
                name = "the_" + name
            self.units[p] = name
        return self.units[p]

    def part_of_function(self, tree, d):
        """What a function is converted with (a job of main): the Blueprint of the file its body is in - a
        header's function that needs a source file (it calls what stands in one) stands with that source
        file -, or the part of a Blueprint of several files (a method of a state: the part of its file).
        The plan of the whole program says which (main)."""
        return self.function_units.get(function_key(tree, d), self.unit_of(d.get("_file") or "?"))

    def unit_of_function(self, tree, d):
        """The Blueprint a function stands in."""
        part = self.part_of_function(tree, d)
        return self.parts.get(part, part)


def is_header(path):
    return path.endswith((".h", ".hpp", ".hh", ".inl"))


def constructor_of(tree, n):
    """The constructor a value is made by (a CXXConstructExpr): the one of its class that has this type -
    the compiler's tree names its type, not the declaration. None: the class has none of its own."""
    want = (n.get("ctorType") or {}).get("qualType")
    if not want:
        return None
    last = re.sub(r"<.*", "", qtype(n)).split("::")[-1].replace("const ", "").strip()
    for q, rec in tree.records.items():
        if q.split("::")[-1] != last:
            continue
        for c in kids(rec):
            if c.get("kind") == "CXXConstructorDecl" and not c.get("isImplicit") and c.get("type", {}).get("qualType") == want:
                return c
    return None


SHAPES = "shapes"   # the library of the types the converter makes of the notation's own (a row of a list of lists, a pair)


# ---- one Blueprint -------------------------------------------------------------------------------
class Function:
    def __init__(self, name):
        self.name = name
        self.cpp = name
        self.pure = True
        self.params = []      # (name, type)
        self.kinds = []       # per parameter: "" an input by value, "ref" a reference the function changes (T& x), "const" one it reads (const T& x)
        self.fixed = {}       # per parameter (by place) that is a list of a fixed length (std::array, T[N]): its length - it is declared Fixed<T, N>
        self.result_fixed = None   # the same for what it gives
        self.result = None    # type, None: no result
        self.text = None
        self.wait = None
        self.lines = 0
        self.decl = None
        self.calls = []       # the functions it calls (Function)
        self.indexes = False  # it reads an entry of a list at a place it computes
        self.self_type = None # a method: the struct that is its object (its first parameter)
        self.mutates = False  # a method that changes its object
        self.refs = []        # the parameters (by place) it writes through
        self.value = None     # the type of what the C++ returns
        self.out_fields = []  # what a call changes or gives, as its test compares it: (name, type, "self" | "result" | ("ref", place of the parameter))
        self.ctor = False     # a constructor: it gives the object it makes
        self.lifted = False   # a function a function declares in itself (a lambda): compared with the function it is in
        self.file = ""
        self.unit = None      # a function of another Blueprint (a library this one uses): that Blueprint's name
        self.key = None       # its name in the whole program (function_key)
        self.open = False     # it stands in a source file's Blueprint: a test in another file holds its source as that file's test hands it on
        self.place = False    # it names a place (`const T& spec() const { return s_; }`): no function of the Blueprint - where it is called, the place stands
        self.on_actors = []   # (actor, function): what it calls on the Blueprint of an actor of the level (Call on Actor)
        self.pointers = []    # the parameters (by place) that are a pointer into a list: the list, read where it stands - the next parameter is the place in it (Unit.pointed)
        self.state = False    # a method of a state (World.states): a function of that Blueprint, its fields are the Blueprint's variables

    def order(self):
        """The parameters in the order the Blueprint function has its inputs: those by reference first, then those
        by value - each group as the C++ has them. (An input by value is written with its default, a reference has
        none, and in C++ every parameter after one with a default has one.)"""
        return sorted(range(len(self.params)), key=lambda i: (0 if self.kinds[i] else 1, i))


class Unit:
    """A source file and the Blueprint it becomes."""

    def __init__(self, tree, name, world=None, part=None):
        self.tree = tree
        self.name = name
        self.job = part or name    # the job it is converted in: the Blueprint - or one of its parts (World.parts)
        self.world = world or World()
        self.names = {name} | self.world.taken   # what a name of the Blueprint must not be again
        self.reserved = set()       # what a local variable or an input is not called (see convert_unit): the names the Blueprint has in the end
        self.type_home = {}         # a type (its Blueprint name) -> the Blueprint that declares it (None: a shape, see `element`)
        self.type_line = {}         # a type -> its declaration
        self.type_parts = {}        # a type -> the types its declaration names
        self.function_names = {}    # the functions of its files (decl id) -> their names, given before any is written
        self.stubs = {}             # a function of another Blueprint (its key) -> what a call needs of it (Function)
        self.missing = set()        # (Blueprint or part, key): functions it would call of Blueprints that are not written yet
        self.asked = set()          # the functions of other Blueprints it would call that wait (their keys)
        # the Blueprint of a state (World.states): the class, its fields (C++ name -> (place, variable, declaration)),
        # the types and declarations of the variables used, the mutable ones
        self.state = next((q for q, b in self.world.states.items() if b == name), None)
        self.state_fields, self.state_types, self.state_lines, self.state_mutable = {}, {}, {}, set()
        self.state_statics = {}     # a variable kept by a method of the state, or one of the file only its methods use -> (variable, type)
        self.state_users = None     # (see kept_by_state)
        self.enums = {}             # qualified C++ name -> Blueprint name
        self.structs = {}           # qualified C++ name -> Blueprint name
        self.types = []             # the declarations, in the order they must stand
        self.struct_fields = {}     # Blueprint name -> [(field, type, default, C++ field)]
        self.rows = {}              # a struct that holds one list (a list inside a list) -> that list's type
        self.enum_numbers = {}      # Blueprint enum -> the numbers C++ gives its values, when they are not 0, 1, 2 ...
        self.enum_values = {}       # Blueprint name -> [value]
        self.variables = {}         # decl id -> (name, type)
        self.variable_lines = []
        self.variable_values = {}   # Blueprint variable -> (type, literal, the C++ declaration)
        self.const_deps = {}        # constants other constants are computed from (decl id -> declaration)
        self.functions = {}         # id of the function's body -> Function
        self.order = []             # the written functions, callees first
        self.members = []
        # (the nodes of a part of a Blueprint of several files have ids of their own: a block of 2^20 for each part)
        self.next_node = 10 + (self.world.part_numbers.get(self.job, 0) << 20)
        self.enum_aliases = {}      # (enum, a value's name) -> the earlier value of the same number it is
        self.lambdas = {}           # a local function (the id of its call operator) -> how it is called (see Emit.local_function)
        self.engine = None          # the engine's functions as nodes (Api), made when the first one is called
        self.engine_calls = {}      # the engine's functions this Blueprint calls: decl id -> its node (Api.function)
        self.objects_seen = set()   # the engine's classes it holds handles of
        self.deciding = set()       # (the engine's classes engine_object is deciding about: one that holds itself)
        self.externs = {}           # a function of an actor's Blueprint it calls on that actor -> (its inputs, its outputs: [(name, type)], its declaration)
        self.working = set()
        self.short = collections.defaultdict(list)
        for q in list(tree.records) + list(tree.enums):
            self.short[q.split("::")[-1]].append(q)
        self.local_scope = ""       # the scope of the types the function being written declares in its body (Tree.take_locals)

    # ---- names ----
    def fresh(self, want, taken=None, among=None):
        """A name for something of the Blueprint (`taken`: of a function - its inputs and local variables,
        which also are not called what the Blueprint has in the end). `among`: the names it must not be,
        where those are not the Blueprint's (a field of a type every Blueprint has the same)."""
        name = sane_name(want)
        base, i = name, 1
        held = self.names if among is None else among
        while name in held or (taken is not None and (name in taken or name in self.reserved)):
            i += 1
            name = f"{base}_{i}"
        return name

    def type_name(self, q):
        """The name of a type of the program: its own name (type_name_of) - or the one the whole program
        gives it, where two types would be called the same."""
        d = self.tree.records.get(q) or self.tree.enums.get(q) or {}
        return self.world.type_names.get((q, os.path.abspath(d.get("_file", "") or "?"))) or type_name_of(q)

    def declare(self, name, line, home, parts):
        """A type the Blueprint computes with: where it is declared, and what its declaration names."""
        self.names.add(name)
        self.types.append(line)
        self.type_line[name] = line
        self.type_home[name] = home
        self.type_parts[name] = [p for p in parts if p in self.type_line]

    def part(self, t):
        """The type a declaration names with `t`: the struct or enum (of a list: its entries'; of a map: its
        values' - of a set: its keys')."""
        if t.startswith(("Map<", "Set<")):
            return self.part(template_args(t)[-1])
        return t[5:-1] if t.startswith("List<") else t

    # ---- types ----
    def clean(self, t):
        if "enum at " in t:   # (a place of an enum without a name holds a number of its underlying type)
            t = re.sub(r"(?:[A-Za-z_]\w*::)*(\((?:unnamed|anonymous) enum at [^)]*\))", lambda m: self.tree.unnamed.get(m.group(1), m.group(0)), t)
        t = re.sub(r"\b(const|volatile|struct|class|enum)\b", " ", t).replace("&&", " ").replace("&", " ")
        t = re.sub(r"(?<![\w:])(?:kern::|std::)?(f32|f64|i8|i16|i32|i64|u8|u16|u32|u64|uint64_t|int64_t|uint8_t|uint16_t|uint32_t|int8_t|int16_t|int32_t|usize|Vec2|Vec3|Vec4|Quat|Mat4|size_t)\b", lambda m: ALIAS[m.group(1)], t)
        t = t.replace("std::size_t", "unsigned long").replace("(anonymous namespace)::", "")
        for _ in range(4):   # (the names the program itself gives types)
            before = t
            for name, what in self.tree.aliases.items():
                if name in t and name not in self.short:
                    t = re.sub(r"(?<![\w:])(?:\w+::)*" + re.escape(name) + r"(?![\w:<])", lambda m: self.clean0(what), t)
            if t == before:
                break
        return re.sub(r"\s+", " ", t).strip()

    def clean0(self, t):
        t = re.sub(r"\b(const|volatile|struct|class|enum)\b", " ", t).replace("&&", " ").replace("&", " ").replace("(anonymous namespace)::", "")
        return re.sub(r"(?<![\w:])(?:kern::|std::)?(f32|f64|i8|i16|i32|i64|u8|u16|u32|u64|uint64_t|int64_t|uint8_t|uint16_t|uint32_t|int8_t|int16_t|int32_t|usize|Vec2|Vec3|Vec4|Quat|Mat4|size_t)\b", lambda m: ALIAS[m.group(1)], t)

    def find_type(self, t):
        """The program's record or enum a type name means (its qualified name), or None."""
        if t in self.tree.records or t in self.tree.enums:
            return t
        t = t.replace("(anonymous namespace)::", "")
        cands = [q for q in self.short.get(t.split("::")[-1], []) if q.endswith(t) or t.endswith(q)]
        # (a type a function declares in its body is known in that function - and there it is the one meant)
        local = self.tree.local_types
        cands = [q for q in cands if q not in local or (self.local_scope and local[q] == self.local_scope)]
        if any(q in local for q in cands):
            cands = [q for q in cands if q in local]
        # (a name of a class only declared here - struct Tile; in a class - is that class, not another one of its
        # last name that is defined: what it is, this file does not know)
        last = t.split("::")[-1]
        for q in self.tree.declared:
            if q.split("::")[-1] == last and q not in self.tree.records and (q.endswith("::" + t) or q == t) and not any(q == c or q.endswith("::" + c) for c in cands):
                raise Wait(f"the type {t} is only declared here (its definition is in another file)")
        if len(cands) == 1:
            return cands[0]
        if len(cands) > 1:
            best = [q for q in cands if q == t or q.endswith("::" + t) or t.endswith("::" + q)]
            if best:
                longest = max(len(q) for q in best)
                best = [q for q in best if len(q) == longest]
            if len(best) == 1:
                return best[0]
            raise Wait(f"the name {t} means several types")
        return None

    def element(self, it, n=None):
        """The type of a list's entries: a list inside a list is a struct that holds it (`n`: the inner
        list has a fixed length - its struct declares it so)."""
        if not it.startswith("List<"):
            return it
        # (a shape: its name says what it is made of - the same in every Blueprint; main says where it stands)
        name = re.sub(r"\W+", "_", it[5:-1]).strip("_") + (f"_{n}" if n else "") + "_row"
        if self.rows.get(name) == it:
            return name
        if name in self.type_line:
            raise Wait(f"two types would be called {name}")
        self.rows[name] = it
        self.struct_fields[name] = [("items", it, it + "{}", "items")]
        self.declare(name, f"  KERN_STRUCT({name}) {{ {self.fixed_type(it, n)} items = {it}{{}}; }};", None, [self.part(it)])
        return name

    def entry(self, raw):
        """The type of the entries of a list whose C++ entries are `raw` (see `element`)."""
        return self.element(self.btype(raw), self.entries(raw))

    @staticmethod
    def fixed_type(t, n):
        """What a place is declared as: a list of a fixed length (`n` entries) is Fixed<T, n>."""
        return f"Fixed<{t[5:-1]}, {n}>" if n and t.startswith("List<") else t

    def dtype(self, raw, t=None):
        """What a place of the C++ type `raw` is declared as in the Blueprint: its type - and a list of a
        fixed length (std::array<T, N>, T[N]) with its length: Fixed<T, N>, which lies where it stands
        as the C++ array does. (What flows through a pin and what an expression is, is the list: `btype`.)"""
        return self.fixed_type(t if t is not None else self.btype(raw), self.fixed(raw))

    def fixed(self, raw):
        """The length a place of the C++ type `raw` is declared with (None: none - not a list of a fixed length)."""
        n = self.entries(raw)
        if n is not None and n > 65536:
            raise Wait("a list of more than 65536 entries of fixed length")
        return n if n else None

    def pair(self, a, b):
        """std::pair<A, B>: a struct of its two parts."""
        key = ("pair", a, b)
        if key not in self.structs:
            name = re.sub(r"\W+", "_", f"{a}_{b}").strip("_") + "_pair"
            if name in self.type_line:
                raise Wait(f"two types would be called {name}")
            self.structs[key] = name
            self.struct_fields[name] = [("first", a, self.zero(a), "first"), ("second", b, self.zero(b), "second")]
            self.declare(name, f"  KERN_STRUCT({name}) {{ {a} first = {self.zero(a)}; {b} second = {self.zero(b)}; }};", None, [self.part(a), self.part(b)])
        return self.structs[key]

    def api(self):
        if self.engine is None:
            self.engine = Api(self.tree, self)
        return self.engine

    def object_of(self, raw, by_value=True):
        """The class of the engine a type holds as an object with a life of its own - the physics world, the
        renderer, the mixer: however it is held - by a pointer, a reference, as a value or owned
        (std::unique_ptr). In a Blueprint that is a handle, a whole number the host gives (0: none)."""
        r = raw.strip()
        if not ENGINE_NODES:
            return None
        m = re.match(r"(?:const\s+)?std::unique_ptr<(.+)>$", self.clean(r))
        if m and by_value:
            r = template_args(self.clean(r))[0]
        if r.endswith("&") or re.search(r"\*\s*(const)?$", r):
            r = re.sub(r"(&|\*\s*(const)?)$", "", r).strip()
        elif not by_value:
            return None
        if any(ch in r for ch in "*&<["):
            return None
        try:
            q = self.find_type(self.clean(r).replace("const ", "").strip())
        except Wait:
            return None
        if q is None or not self.engine_object(q):
            return None
        self.objects_seen.add(q)
        return q

    def engine_object(self, q):
        """Is the class `q` of the engine an object with a life of its own (a handle in a Blueprint), not
        plain data? A `class` is one (its state is its own: the engine's convention), and so is a struct
        that cannot be a struct of the Blueprint (a pointer in it, a field without a Blueprint type).
        Api.opaque asks the same."""
        if not q.startswith(ENGINE + "::") or q not in self.tree.records or q in (ENGINE + "::Vec3", ENGINE + "::Vec2"):
            return False
        rec = self.tree.records[q]
        if rec.get("tagUsed") == "class":
            return True
        for c in kids(rec):
            # a struct that cannot be copied (a Blueprint's struct is a value), or that holds an object of the
            # engine itself - as a value or owned (std::unique_ptr): an object too (Mesh: its Bvh)
            if c.get("kind") == "CXXConstructorDecl" and c.get("explicitlyDeleted") and re.fullmatch(r"\s*\w+\s*\(\s*const\s+[\w:]+\s*&\s*\)\s*", qtype(c)):
                return True
            if c.get("kind") == "FieldDecl":
                raw = qtype(c).strip()
                if not re.search(r"(&|\*\s*(const)?)$", raw) and q not in self.deciding:
                    self.deciding.add(q)
                    try:
                        if self.object_of(raw) is not None:
                            return True
                    finally:
                        self.deciding.discard(q)
        try:
            self.struct(q)
            return False   # (plain data: a struct of the Blueprint)
        except Wait:
            return True

    def engine_table(self):
        """The engine's nodes as the engine has them (kern/src/logic/engine_api.gen.cpp): type -> (is it a
        value, its inputs, its outputs - each (pin, type, the struct or enum it names))."""
        if not hasattr(self, "_table"):
            self._table = {}
            try:
                text = open(os.path.join(ROOT, "kern", "src", "logic", "engine_api.gen.cpp")).read()
            except OSError:
                text = ""
            pins = lambda t: [(m.group(1), m.group(2), m.group(3)) for m in re.finditer(r'\{"(\w+)", PinType::(\w+), "(\w*)", "[^"]*"\}', t)]
            for m in re.finditer(r'\{"(engine\.[\w.]+)", (?:"(?:\\.|[^"\\])*", ){4}(true|false),\n\s*\{(.*)\},\n\s*\{(.*)\}\},', text):
                self._table[m.group(1)] = (m.group(2) == "true", pins(m.group(3)), pins(m.group(4)))
        return self._table

    def btype(self, raw):
        """The Blueprint type of a C++ type."""
        if raw.strip() in ("const char *", "const char *const"):
            return "Text"
        if self.object_of(raw):
            return "Int"
        t = self.clean(raw)
        if t in NUMBER:
            return NUMBER[t]
        if t in W64:
            return "Int64"   # (64 bits as C++ holds them: the language's Int64 - what they mean, its nodes say)
        if t in TEXT or t.startswith("std::basic_string<char") or t.startswith("std::basic_string_view<char"):
            return "Text"
        if t in ("kern::Vec3", "kern::Vec2"):
            return "Vector"
        if t == "void":
            return None
        if ITERATOR.match(t):
            return "Int"
        if t == "std::_Bit_reference":
            return "Bool"   # (std::vector<bool>'s entry where it stands: in a List<Bool> a place like any other - see member_call, for_each)
        if "*" in t:
            raise Wait("a pointer")
        m = re.match(r"(?:kern::)?Span<\s*(.+)>$", t)
        if m and ENGINE_NODES:   # (a view of a list: the list)
            return f"List<{self.entry(m.group(1))}>"
        m = KEYED_KERN.match(t)
        if m:
            a = template_args(t)
            if len(a) > (1 if m.group(1) == "Set" else 2):
                raise Wait("a map or set with a hash of its own")
            key = self.btype(a[0])
            if key not in ("Int", "Int64", "Text", "Bool") and key not in self.enum_values:
                raise Wait(f"a map or set whose keys are {key} (a key is a Bool, an Int, an Int64, a Text or an enum's value)")
            if m.group(1) == "Set":
                return f"Set<{key}>"
            value = self.btype(a[1])
            if value.startswith(("Map<", "Set<")):
                raise Wait("a map of maps or sets")
            return f"Map<{key}, {value}>"
        m = KEYED.match(t)
        if m:
            # (std::unordered_map goes through its entries in the order of the library; std::map in the order
            # of its keys: the language's Map in the order the keys came - kern::Map is the one of the program)
            raise Wait("a map or set of the standard library (the program's are kern::Map, kern::Set: in the order the keys came)")
        m = re.match(r"std::vector<(.+)>$", t)
        if m:
            return f"List<{self.entry(template_args(t)[0])}>"
        # (T[3][2] is 3 entries of T[2]: the first length is the outer one)
        m = re.match(r"std::array<(.+), (\d+)>$", t) or re.match(r"(.+?)\s*\[(\d*)\]((?:\s*\[\d*\])*)$", t)
        if m and m.lastindex == 3 and m.group(3):
            return f"List<{self.entry(m.group(1) + m.group(3))}>"
        if m:
            return f"List<{self.entry(m.group(1))}>"
        if t.startswith("std::unique_ptr<") or t.startswith("std::optional<"):
            # what may be there or not: a list of no entry or one
            return f"List<{self.entry(template_args(t)[0])}>"
        if t.startswith("std::initializer_list<"):
            return f"List<{self.entry(template_args(t)[0])}>"
        if t.startswith("std::shared_ptr"):
            raise Wait("a shared pointer (shared_ptr)")
        if t.startswith("std::function"):
            raise Wait("a function value (std::function: the language has no function values)")
        if "(lambda" in t:
            raise Wait("a function value (lambda / std::function)")
        if t.startswith("std::pair<"):
            a_, b_ = template_args(t)
            return self.pair(self.btype(a_), self.btype(b_))
        if t.startswith("std::tuple") or t.startswith("std::variant"):
            raise Wait("tuple / variant")
        if t.startswith("std::priority_queue<"):
            # (which of two entries of the same priority comes first, the library's heap says)
            raise Wait("the type std::priority_queue (a list kept in an order: the language has no step for it)")
        q = self.find_type(t)
        if q in self.tree.enums:
            return self.enum(q)
        if q in self.tree.records:
            return self.struct(q)
        if t.startswith("kern::"):
            raise Wait(f"the engine's {t.split('<')[0]}")
        raise Wait(f"the type {t.split('<')[0]}")

    def pointed(self, raw):
        """A parameter `const T*` the function reads entries through (`p[i]`, `*p`), hands on (`p + 2`) or asks
        whether it is null: a pointer into a list - in the Blueprint the list, read where it stands (a const
        reference input), and the place in it (an Int; null is the place -1). What a call hands it is that pair:
        `a.data() + k` is (a, k), an array is (it, 0), nullptr (an empty list, -1). The Blueprint type of its
        entries; None: the parameter is no such pointer."""
        m = re.match(r"const\s+(.+?)\s*\*$", raw.strip())
        if m is None or self.object_of(raw) or raw.replace(" ", "") in ("constchar*", "constchar*const"):
            return None
        inner = m.group(1).strip()
        if inner == "void" or "*" in inner or "&" in inner:
            return None
        t = self.btype(inner)
        return t if t is not None and not t.startswith("List<") else None

    def param_types(self, p):
        """The Blueprint inputs a parameter of the program is: its type - a pointer into a list two (Unit.pointed)."""
        t = self.pointed(qtype(p))
        return [f"List<{t}>", "Int"] if t is not None else [self.btype(qtype(p))]

    def enum(self, q):
        if q in self.enums:
            return self.enums[q]
        o = self.tree.enums[q]
        numbered, first = [], {}
        for n, x in enum_numbers(o):
            # a value with the number of an earlier one is that value under another name (`kLast = vehicle`)
            if x in first:
                self.enum_aliases[(q, n)] = first[x]
                continue
            first[x] = n
            numbered.append((n, x))
        values = [n for n, _ in numbered]
        if not values:
            raise Wait("an enum without values")
        name = self.type_name(q)
        if name in self.type_line:
            raise Wait(f"two types would be called {name}")
        vals = []
        for v in values:
            vals.append(v + "_" if is_word(v) else v)
        self.enums[q] = name
        self.enum_values[name] = vals
        self.declare(name, f"  KERN_ENUM({name}, {', '.join(vals)});", self.world.unit_of(o.get("_file", "")), [])
        if [x for _, x in numbered] != list(range(len(numbered))):
            # the numbers C++ gives the values, where a value is used as a number: a constant of the Blueprint
            table = self.fresh(name + "_numbers")
            self.names.add(table)
            self.variable_lines.append(f"  /// The number of each value of {name}, in their order")
            self.variable_lines.append(f"  KERN_CONST List<Int> {table} = List<Int>{{{', '.join(str(x) for _, x in numbered)}}};")
            self.enum_numbers[name] = table
        return name

    def struct(self, q):
        if q in self.world.states:
            raise Wait(f"the state of the Blueprint {self.world.states[q]} (its one object is that Blueprint, no value)")
        if q in self.structs:
            if self.structs[q] is None:
                raise Wait(f"the struct {q.split('::')[-1]} contains itself")
            return self.structs[q]
        o = self.tree.records[q]
        if o.get("bases"):
            raise Wait(f"the class {q.split('::')[-1]} (a base class)")
        self.structs[q] = None
        try:
            fields, taken, declared = [], set(), {}
            for c in kids(o):
                if c.get("kind") in ("CXXMethodDecl", "CXXDestructorDecl") and c.get("virtual"):
                    raise Wait(f"the class {q.split('::')[-1]} (virtual functions)")
                if c.get("kind") != "FieldDecl":
                    continue
                try:
                    ft = self.btype(qtype(c))
                except Wait as w:
                    raise Wait(f"the struct {q.split('::')[-1]} (its field {c.get('name')}: {w})")
                # (a field has its own name; no field is called what a type of the program is - in whatever
                # Blueprint the struct is used, it is the same struct)
                fname = self.fresh(c.get("name", "field"), taken, among=self.world.taken)
                taken.add(fname)
                init = kids(c)[-1] if kids(c) and c.get("hasInClassInitializer") else None
                # (an array of C++ is declared with its length - `dtype`, below -: its default says what is
                # not zero - nothing)
                default = self.zero(ft)
                if init is not None:
                    try:
                        default = Const(self).literal(init, ft)
                    except Wait as w:
                        raise Wait(f"the struct {q.split('::')[-1]} (the default of its field {c.get('name')}: {w})")
                fields.append((fname, ft, default, c.get("name")))
                declared[fname] = self.dtype(qtype(c), ft)
            if not fields:
                raise Wait(f"the struct {q.split('::')[-1]} has no fields")
            if len(fields) > 96:
                raise Wait(f"the struct {q.split('::')[-1]} has more than 96 fields")
        except Wait:
            del self.structs[q]
            raise
        name = self.type_name(q)
        if name in self.type_line:
            del self.structs[q]
            raise Wait(f"two types would be called {name}")
        self.structs[q] = name
        self.struct_fields[name] = fields
        # (a field that is an array of C++ is declared with its length: Fixed<Float, 16> - its default is the list of its entries)
        self.declare(name, f"  KERN_STRUCT({name}) {{ " + " ".join(f"{declared[f]} {f} = {d};" for f, t, d, _ in fields) + " };", self.world.unit_of(o.get("_file", "")),
                     [self.part(t) for _, t, _, _ in fields])
        return name

    def entries(self, raw):
        """How many entries a list of fixed length has (`f32 m[16]`, std::array<u8, 32>); None: a list of any length."""
        c = self.clean(raw)
        # (T[3][2] has 3 entries, each a T[2]: the first length is the outer one)
        m = re.match(r"std::array<.*,\s*(\d+)U?L?L?>$", c) or re.match(r"[^\[]*\[(\d+)\](?:\s*\[\d+\])*$", c)
        return int(m.group(1)) if m else None

    def zero(self, t):
        if t in ZERO:
            return ZERO[t]
        if re.match(r"Fixed<(.+), (\d+)>$", t):   # (a list of a fixed length: what its text does not say is zero)
            return t + "{}"
        if t.startswith(("List<", "Map<", "Set<")):
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
            # (a library keeps nothing: what every caller of the file's functions shares has no place in the language)
            raise Wait("state outside a function (a variable of the file: a library keeps nothing)")
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
        self.variable_lines.append(f"  KERN_CONST {self.dtype(raw, t)} {name} = {lit};")   # (an array of C++: a list of that length)
        self.variable_values[name] = (t, lit, d)
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
        doc = self.tree.said(" ".join(x for x in out if x))
        return re.sub(r"\s+", " ", doc.replace("*/", "* /")).strip()

    # ---- functions ----
    def mine(self, d):
        """Is this function one of this Blueprint's?"""
        return bool(d.get("_mine")) or (bool(d.get("_file")) and self.world.part_of_function(self.tree, d) == self.job)

    @staticmethod
    def cpp_name_of(tree, d):
        owner = tree.owner.get(d["id"], "")
        return (owner.split("::")[-1] + "::" if owner else "") + d.get("name", "?")

    def cpp_name(self, d):
        return self.cpp_name_of(self.tree, d)

    def wanted_name(self, d):
        """The name a function of the program would have in the Blueprint."""
        owner = self.tree.owner.get(d["id"], "")
        ops = {"+": "add", "-": "sub", "*": "mul", "/": "div", "==": "equal", "!=": "differs", "<": "less", "[]": "at", "()": "call", "+=": "add_to", "-=": "sub_from", "*=": "scale"}
        want = self.cpp_name(d).replace("::", "_")
        if d.get("kind") == "CXXConstructorDecl":
            want = owner.split("::")[-1] + "_new"
        elif d.get("name", "").startswith("operator") and d["name"][8:] in ops:
            want = (owner.split("::")[-1] + "_" if owner else "") + ops[d["name"][8:]]
        return want

    def name_functions(self, decls):
        """The functions of the Blueprint's files get their names - in the order they stand in, before any
        is written: whichever of them are written this time, each is called the same. (The Blueprint of a
        state: first its variables, then its functions, in the order the class declares them - the class
        is the same in every file, and so is each name in every part.)"""
        if self.state is not None:
            self.plan_state()
            decls = [d for d in decls if d["id"] not in self.function_names]
        for d in decls:
            name = self.fresh(self.wanted_name(d))
            self.names.add(name)
            self.function_names[d["id"]] = name

    PLACE = "it names a place: where it is called, the place stands - here the call could not be written so"

    # ---- the Blueprint of a state (World.states) ----
    def plan_state(self):
        """The names of the variables and the functions of the Blueprint of a state: its fields' and its
        methods', in the order the class has them."""
        rec = self.tree.records.get(self.state)
        if rec is None:
            raise SystemExit(f"{self.job}: the class {self.state} is not known here")
        for c in kids(rec):
            if c.get("kind") == "FieldDecl":
                name = self.fresh(c.get("name") or "field")
                self.names.add(name)
                self.state_fields[c.get("name")] = ((0, len(self.state_fields)), name, c)
        for c in kids(rec):
            if c.get("kind") in ("CXXMethodDecl", "CXXConstructorDecl") and not c.get("isImplicit") and c["id"] not in self.function_names:
                name = self.fresh(c.get("name") if c.get("kind") == "CXXMethodDecl" and not c.get("name", "").startswith("operator") else self.wanted_name(c))
                self.names.add(name)
                self.function_names[c["id"]] = name
                d = self.tree.body.get(c["id"])
                if d is not None:   # (defined outside the class: the definition is called the same)
                    self.function_names[d["id"]] = name

    def state_variable(self, field):
        """The variable a field of the state is: (its name, its type) - declared where it is first used, with
        the field's default. A field whose type the language does not have is no variable: what uses it waits."""
        if field not in self.state_fields:
            raise Wait(f"the state {self.name} has no field {field}")
        at, name, c = self.state_fields[field]
        if field in self.state_types:
            return name, self.state_types[field]
        try:
            t = self.btype(qtype(c))
            init = kids(c)[-1] if kids(c) and c.get("hasInClassInitializer") else None
            default = Const(self).literal(init, t) if init is not None else self.zero(t)
        except Wait as w:
            raise Wait(f"the variable {name} of {self.name}: {w}")
        if c.get("mutable"):
            self.state_mutable.add(name)
        doc = self.comment_of(c)
        self.state_lines[at] = ([f"  /// {doc}"] if doc else []) + [f"  {self.dtype(qtype(c), t)} {name} = {default};"]
        self.state_types[field] = t
        return name, t

    def state_static(self, d, owner):
        """A variable a method of the state keeps from one call to the next (`static int last_tick = 0;`), or one
        of the file only the state's methods use: there is one object of the state, so this is part of its
        state - a variable of its Blueprint, called by where it stands (the method's name or the file's, and
        its own) and starting at its initializer. (its name, its type)"""
        if d["id"] in self.state_statics:
            return self.state_statics[d["id"]]
        shared = d.get("storageClass") != "static" and "_GLOBAL__N_" not in (d.get("mangledName") or "") and d["id"] in self.tree.scope_of
        if shared:   # (a variable the whole program shares: called by its namespaces, the same in every part of the Blueprint)
            q = self.tree.scope_of[d["id"]] + (d.get("name") or "")
            inner = q[len(self.tree.namespace) + 2:] if q.startswith(self.tree.namespace + "::") else q
            owner = "_".join(inner.split("::")[:-1]) or "program"
        name = sane_name(want)
        if name in self.names:
            raise Wait(f"the variable {d.get('name')} kept by {owner}: the Blueprint {self.name} has something called {name}")
        t = self.btype(qtype(d))
        init = kids(d)[-1] if kids(d) and d.get("init") else None
        if shared and init is None and self.world.globals.get(q) is False:
            raise Wait(f"the variable {q} starts with what its definition in another file says")
        try:
            default = Const(self).literal(init, t) if init is not None else self.zero(t)
        except Wait as w:
            raise Wait(f"the variable {d.get('name')} kept by {owner} starts with what is computed when it is first met ({w})")
        if shared and init is not None and starts_at_zero(init):
            default = self.zero(t)   # (written as every part writes it, also those that only see it declared)
        self.names.add(name)
        self.state_lines[(1, name)] = [f"  /// ({'of the program: ' + q if shared else 'kept by ' + owner})", f"  {self.dtype(qtype(d), t)} {name} = {default};"]
        self.state_statics[d["id"]] = (name, t)
        return name, t

    def kept_by_state(self, d):
        """Is this a variable of the program that the state keeps: one the program has once - of a namespace,
        not const -, which the program's one object (the state) reads and changes? Then it is a variable of
        the state's Blueprint. A variable of a file (static, or in a namespace without a name) only when no
        other function of the file names it; one other files name too (extern) as every file sees it."""
        if self.state is None or d.get("kind") != "VarDecl" or d.get("constexpr") or re.search(r"\bconst\b", qtype(d)):
            return False
        if d["id"] not in self.tree.scope_of or self.tree.scope_of[d["id"]].rstrip(":") in self.tree.records:
            return False   # (a local variable - or a static field of a class: the class's)
        if not self.world.program(d.get("_file") or "?") or self.actor_type(qtype(d)):
            return False   # (the engine's - or the program's way to call an actor's Blueprint: Call on Actor names the actor)
        if not (d.get("storageClass") == "static" or "_GLOBAL__N_" in (d.get("mangledName") or "")):
            return True   # (a variable the whole program shares: of its one object)
        if is_header(d.get("_file", "")):
            return False
        if self.state_users is None:
            users = collections.defaultdict(set)
            for f in {id(x): x for x in self.tree.body.values()}.values():
                if f.get("kind") == "VarDecl":
                    continue
                state = self.tree.owner.get(f["id"]) == self.state
                for n in walk(f):
                    if n.get("kind") == "DeclRefExpr" and (n.get("referencedDecl") or {}).get("kind") == "VarDecl":
                        users[n["referencedDecl"]["id"]].add(state)
            self.state_users = users
        return self.state_users.get(d["id"]) == {True}

    def actor_type(self, t):
        """Is this the engine's kern::logic::ActorBlueprint, or a class of the program made of it?"""
        c = re.sub(r"^const\s+", "", self.clean(t)).strip()
        if c == ENGINE + "::logic::ActorBlueprint":
            return True
        try:
            q = self.find_type(c) if c else None
        except Wait:
            return False
        d = self.tree.records.get(q) if q else None
        return d is not None and any(self.actor_type(b.get("type", {}).get("qualType", "")) for b in d.get("bases", []) or [])

    def is_state_variable(self, name):
        return self.state is not None and (any(name == n for _, n, _ in self.state_fields.values()) or any(name == n for n, _ in self.state_statics.values()))

    def place_named(self, d):
        """`const CarSpec& spec() const { return s_; }` - a function of the program that gives a reference
        and does nothing but name what it refers to: a name for a place, as `T& x = a.b[i];` is one. It is
        no function of the Blueprint: where it is called, the place stands (Emit.dissolve) - nothing is
        copied, as nothing is in C++. The expression it returns; None: the function is none such. (Only
        what every caller sees: a function of a header, or one no header declares.)"""
        if d is None or d.get("kind") not in ("FunctionDecl", "CXXMethodDecl") or d.get("virtual") or d.get("variadic") or not d.get("_file"):
            return None
        if not self.world.program(os.path.abspath(d["_file"])):
            return None   # (the engine's own: its objects are handles, its functions nodes)
        ret = qtype(d).split("(")[0].strip()
        if not ret.endswith("&") or ret.endswith("&&"):
            return None
        if not is_header(d["_file"]) and declared_in_header_of(self.tree, d):
            return None
        body = next((c for c in kids(d) if c.get("kind") == "CompoundStmt"), None)
        stmts = kids(body) if body is not None else []
        if len(stmts) != 1 or stmts[0].get("kind") != "ReturnStmt" or not kids(stmts[0]):
            return None
        return kids(stmts[0])[0]

    def function(self, decl_id):
        d = self.tree.body.get(decl_id)
        if d is None or not self.mine(d):
            return self.foreign(decl_id, d)
        f = self.functions.get(d["id"])
        if f is not None:
            if f.place:
                raise Wait(f"{f.cpp}(): {self.PLACE}")
            if f.wait:
                raise Wait(f"it calls {f.cpp}(), which waits")
            if f.text is None:
                raise Wait(f"{f.cpp}() calls itself (a Blueprint's function does not)")
            return f
        name = self.function_names.get(d["id"])
        if name is None:   # (a function a function declares in itself: named when it is met)
            name = self.fresh(self.wanted_name(d))
            self.names.add(name)
        f = Function(name)
        f.cpp = self.cpp_name(d)
        f.decl = d
        f.file = d.get("_file", "")
        f.key = function_key(self.tree, d)
        a, b = d.get("_at", (None, None))
        f.lines = self.tree.text(f.file).count("\n", a, b) + 1 if a is not None else 0
        self.functions[d["id"]] = f
        if self.place_named(d) is not None:
            f.place = True
            raise Wait(f"{f.cpp}(): {self.PLACE}")
        try:
            if f.key in self.world.bars:
                raise Wait(self.world.bars[f.key])
            if d["id"] in getattr(self, "barred", {}):
                raise Wait(self.barred[d["id"]])
            outer, self.local_scope = self.local_scope, self.tree.local_scope(d)
            try:
                Emit(self, f).run()
            finally:
                self.local_scope = outer
            # (what calls a function on the Blueprint of an actor of the level: its test runs that Blueprint - the
            # source calls it as the program binds it, the Blueprint calls it on that actor; TestWriter)
            self.order.append(f)
        except Wait as w:
            f.wait = str(w)
            # (what it had begun to declare stays known - a function written on the way may use it; the
            # Blueprint's text has only the declarations its functions use: see text)
            raise
        return f

    def foreign(self, decl_id, d):
        """A function of another Blueprint - a library this one uses: what a call of it needs to know, as
        that Blueprint was written (the table of the whole program)."""
        decl = d or self.tree.decl.get(decl_id) or {}
        if d is None:
            if decl.get("kind") not in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXConversionDecl") or function_key(self.tree, decl) not in self.world.function_units:
                raise Wait(f"{decl.get('name', 'a function')}() is defined in another file")
            d = decl   # (its body stands in another source file of the program: what a call needs is that Blueprint's)
        key = function_key(self.tree, d)
        f = self.stubs.get(key)
        if f is None:
            part = self.world.part_of_function(self.tree, d)
            e = self.world.table.get(key)
            if part is None:
                raise Wait("what it calls stands in a header and needs a source file that is not converted")
            if e is None:
                self.missing.add((part, key))
                raise Wait(f"{self.cpp_name(d)}() of the Blueprint {self.world.parts.get(part, part)} is not written")
            # (a function of another part of this Blueprint is one of its own: called by its name)
            f = Function(e["name"] if e["unit"] == self.name else f"{e['unit']}::{e['name']}")
            f.unit, f.key, f.cpp, f.decl, f.file = e["unit"], key, e["cpp"], d, d.get("_file", "")
            f.wait = e["wait"]
            f.text = "" if e["wait"] else "(of another Blueprint)"
            f.pure, f.params, f.kinds, f.result, f.indexes = e["pure"], [tuple(x) for x in e["params"]], list(e["kinds"]), e["result"], e["indexes"]
            f.fixed, f.result_fixed = {int(k): v for k, v in e["fixed"].items()}, e["result_fixed"]
            f.self_type, f.mutates, f.refs, f.value, f.ctor = e["self_type"], e["mutates"], list(e["refs"]), e["value"], e["ctor"]
            f.out_fields = [(n, t, tuple(src) if isinstance(src, list) else src) for n, t, src in e["out_fields"]]
            f.open, f.state = e["open"], e["state"]
            if f.state and e["unit"] != self.name and not f.wait:
                f.wait = f"a function of {e['unit']}, the Blueprint of the program's one object: only its own functions call it"
            f.on_actors = [(a, None) for a in e.get("actors", ())]   # (the actors whose Blueprints it calls on them, itself or through what it calls)
            f.pointers = list(e["pointers"])
            if not f.wait and f.open and d["id"] not in self.tree.body and self.world.closures is not None and key not in self.world.closures:
                # A function is written when its test compares it, and the test holds the source of all it
                # calls once more: what stands in another source file, as the test of that file hands it on.
                f.wait = "what it calls stands in another source file, whose test does not hand its source on"
                f.text = ""
            if not f.wait:
                # the types it takes and gives are this Blueprint's too now: as this file's compiler sees them
                # (they are the same types - a type is converted the same wherever it is seen)
                seen = []
                if f.self_type is not None:
                    mine = self.struct(self.tree.owner.get(d["id"]))
                    seen += [mine] if not f.ctor else []
                seen += [t for x in kids(d) if x.get("kind") == "ParmVarDecl" for t in self.param_types(x)]
                if f.value is not None:
                    seen.append(self.btype(qtype(d).split("(")[0].strip()))
                told = [t for _, t in f.params] + ([f.value] if f.value is not None else [])
                if seen != told:
                    raise SystemExit(f"{self.name}: {f.cpp}() of {f.unit} takes and gives {told} there and {seen} here")
            self.stubs[key] = f
        if f.wait:
            if (self.world.table.get(key) or {}).get("wait"):
                self.asked.add(key)   # (it waits where it stands: should it be written later, this Blueprint is converted again - main)
            raise Wait(f.wait if f.open else f"it calls {f.cpp}(), which waits")
        return f

    def entry_of(self, f):
        """What a call of a function of this Blueprint needs to know, for the table of the whole program."""
        return {"unit": self.name, "name": f.name, "cpp": f.cpp, "wait": (f.wait or (self.PLACE if f.place else None)) if not f.text else None, "pure": f.pure, "params": f.params, "kinds": f.kinds, "result": f.result,
                "fixed": dict(f.fixed), "result_fixed": f.result_fixed,
                "indexes": f.indexes, "self_type": f.self_type, "mutates": f.mutates, "refs": f.refs, "value": f.value, "ctor": f.ctor, "out_fields": f.out_fields,
                "open": self.job in self.world.source_units, "state": f.state, "pointers": f.pointers,
                "actors": sorted(actors_called(f))}

    def uses(self):
        """The Blueprints whose functions the written functions call."""
        return sorted({g.unit for f in self.order for g in f.calls if g.unit and g.unit != self.name})

    def parts(self):
        """The Blueprint's text in its parts: its constants, its functions, what its nodes give and keep -
        and the types these name: {name: (the Blueprint that declares it or None for a shape, its
        declaration, the types that names)}."""
        word = re.compile(r"[A-Za-z_]\w*")
        used = set()
        for f in self.order:
            used.update(word.findall(f.text))
        for name, (_, _, lines) in self.externs.items():   # (what a function of an actor's Blueprint looks like: where it is called)
            if name in used:
                used.update(word.findall(" ".join(lines)))
        def name_of(line):
            m = (re.match(r"\s*KERN_(?:ENUM|STRUCT)\((\w+)", line) or re.match(r"\s*(?:KERN_CONST\s+)?[\w:]+(?:<[^=]*>)?\s+(\w+)\s*=", line) or re.search(r"\}\s*(n\d+);", line)
                 or re.match(r"\s*[\w:]+Result (n\d+);", line))
            return m.group(1) if m else None
        variables = []
        self.state_used = {}
        for at in sorted(self.state_lines):   # (the variables of a state, in the order of its fields)
            if name_of(self.state_lines[at][-1]) in used:
                variables += self.state_lines[at]
                self.state_used[at] = self.state_lines[at]
                used.update(word.findall(self.state_lines[at][-1]))
        pending = []
        for line in self.variable_lines:   # (a /// line belongs to the constant below it)
            if line.strip().startswith("///"):
                pending = [line]
                continue
            if name_of(line) in used:
                variables += pending + [line]
                used.update(word.findall(line))
            pending = []
        keep = [False] * len(self.types)
        changed = True
        while changed:   # (a type another kept type is made of is kept too)
            changed = False
            for i, line in enumerate(self.types):
                if not keep[i] and name_of(line) in used:
                    keep[i] = changed = True
                    used.update(word.findall(line))
        types = {}
        for i, line in enumerate(self.types):
            if keep[i]:
                name = name_of(line)
                types[name] = (self.type_home[name], line, self.type_parts[name])
        members = [line for line in self.members if name_of(line) in used]
        externs = ["\n".join(lines) for name, (_, _, lines) in self.externs.items() if name in used]
        return variables, externs + [f.text for f in self.order], members, types

    def text(self):
        """The Blueprint as this file alone knows it: its functions, and of the declarations made on the
        way those they use - the types among them, wherever they are declared in the end (main puts every
        Blueprint together from what all files say)."""
        variables, functions, members, types = self.parts()
        return blueprint_text(self.name, self.uses(), [line for _, line, _ in types.values()], variables, functions, members, state=self.state is not None)


def actors_called(f, met=None):
    """The actors of the level whose Blueprints a function calls on them - itself, or through what it calls."""
    met = set() if met is None else met
    if id(f) in met:
        return set()
    met.add(id(f))
    out = {a for a, _ in f.on_actors}
    for g in f.calls:
        out |= actors_called(g, met)
    return out


def blueprint_text(name, uses, types, variables, functions, members, state=False):
    """A library's text, of its parts - or, with `state`, that of a Blueprint that keeps something: the
    variables of a state (World.states)."""
    # (`virtual` is for a library and for what uses one: a Blueprint that uses none is a kern::Blueprint as such)
    out = [f"struct {name} : {'virtual ' if uses or not state else ''}kern::{'Blueprint' if state else 'Library'}" + "".join(f", virtual {u}" for u in uses) + " {"]
    if types:
        out += ["  // ---- types ----", *types, ""]
    if variables:
        out += ["  // ---- variables ----", *variables, ""]
    if functions:
        out.append("  // ---- functions ----")
        for text in functions:
            out += [text, ""]
    if members:
        out += ["  // ---- what nodes give and keep ----", *members]
    while out[-1] == "":
        out.pop()
    out.append("};")
    return "\n".join(out) + "\n"


# ---- constants -------------------------------------------------------------------------------------
class Const:
    """What an expression is when everything in it is known: a constant's value, a default."""

    def __init__(self, unit):
        self.unit = unit

    BITS = {"whole64u": 64, "whole64s": 64, "unsigned char": 8, "signed char": 8, "char": 8, "unsigned short": 16, "short": 16, "unsigned int": 32, "int": 32,
            "long": 64, "unsigned long": 64, "long long": 64, "unsigned long long": 64}

    def fit(self, v, n):
        """A whole number as the C++ type of `n` holds it (it wraps around at the type's width); a 64-bit
        one as a Blueprint holds it: read with its sign (see WIDE_UNSIGNED)."""
        t = self.unit.clean(qtype(n))
        bits = self.BITS.get(t)
        if bits is None:
            return v
        i = int(v) & ((1 << bits) - 1)
        if (t not in UNSIGNED or bits == 64) and i >= 1 << (bits - 1):
            i -= 1 << bits
        return i

    def number(self, n):
        n = bare(n)
        k = n.get("kind")
        if k in ("ImplicitCastExpr", "CXXStaticCastExpr", "CStyleCastExpr", "CXXFunctionalCastExpr"):
            v = self.number(kids(n)[-1])
            t = self.unit.clean(qtype(n))
            if NUMBER.get(t) == "Int":
                return self.fit(int(v), n)
            if t == "bool":
                return 1.0 if v != 0 else 0.0
            return v
        if k == "FloatingLiteral":
            s = self.unit.tree.source(n)
            m = re.match(r"^[0-9.eE+\-']+", s)
            # (a literal is the decimal the source spells: 43758.5453f is 43758.5453, as a Blueprint's 64-bit number has it)
            return float(m.group(0).replace("'", "")) if m and m.group(0) not in (".", "") else float(n["value"])
        if k == "IntegerLiteral":
            return self.fit(int(n["value"]), n)
        if k == "CXXBoolLiteralExpr":
            return 1.0 if n.get("value") else 0.0
        if k == "CharacterLiteral":
            return float(n["value"])
        if k == "UnaryOperator" and n.get("opcode") in ("-", "+", "!", "~"):
            v = self.number(kids(n)[0])
            if n["opcode"] == "~":
                return self.fit(~int(v), n)
            if n["opcode"] == "-" and NUMBER.get(self.unit.clean(qtype(n))) == "Int":
                return self.fit(-int(v), n)
            return -v if n["opcode"] == "-" else v if n["opcode"] == "+" else (0.0 if v else 1.0)
        if k == "BinaryOperator" and n.get("opcode") == "/" and all(bare(c).get("kind") == "UnaryExprOrTypeTraitExpr" for c in kids(n)):
            # sizeof a / sizeof a[0]: how many entries the array a has (what neither size is, in bytes, does not matter)
            whole, one = (self.unit.clean(sized(bare(c)) or "") for c in kids(n))
            m = re.match(r"(.+?)\s*\[(\d+)\]$", whole)
            same = lambda t: self.unit.find_type(t) or t   # (a type of the program, however its name is spelled)
            if m and one and same(self.unit.clean(m.group(1))) == same(one):
                return self.fit(int(m.group(2)), n)
            raise Wait("a constant computed with sizeof")
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
            elif op == "<<" and isint: r = int(a) << int(b)
            elif op == ">>" and isint: r = int(a) >> int(b)
            elif op == "|" and isint: r = int(a) | int(b)
            elif op == "&" and isint: r = int(a) & int(b)
            elif op == "^" and isint: r = int(a) ^ int(b)
            elif op == "%" and isint and b != 0: r = int(a) - int(a / b) * int(b)
            else:
                raise Wait(f"a constant computed with {op}")
            return self.fit(r, n) if isint else r
        if k == "DeclRefExpr":
            ref = n.get("referencedDecl", {})
            d = self.unit.tree.body.get(ref.get("id"), self.unit.tree.decl.get(ref.get("id")))
            if d is not None and d.get("kind") == "VarDecl" and d.get("init") and kids(d):
                self.unit.const_deps[d["id"]] = d   # (the test compiles it along)
                return self.number(kids(d)[-1])
            if ref.get("kind") == "EnumConstantDecl":
                ed = self.unit.tree.decl.get(ref.get("id"), {})
                q = ed.get("_enum")
                if q in self.unit.tree.enums:
                    return float(dict(enum_numbers(self.unit.tree.enums[q]))[ref["name"]])
                raise Wait("an enum value as a number")
            if ref.get("name") == "kPi":
                return 3.14159265358979323846
            raise Wait(f"the constant {ref.get('name')} is defined elsewhere")
        if k == "CallExpr" and self.limit(n) is not None:
            return self.limit(n)
        if self.inlined(n) is not None:
            return self.number(self.inlined(n))
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
        if k == "InitListExpr" and len(kids(n)) <= 1 and (NUMBER.get(self.unit.clean(qtype(n))) or self.unit.clean(qtype(n)) in W64):
            # a number in braces: `f32 x{}` is zero, `u32 dt{kNominalDtUs}` the number in them
            return self.number(kids(n)[0]) if kids(n) else 0.0
        if k == "MemberExpr" and kids(n) and not n.get("isArrow"):
            # a field of a value that is known: the part of it that sets the field (`gains.d_cutoff_hz` of the
            # `Gains{...}` a constructor was given), else the field's own default
            v = self.field_of(kids(n)[0], n.get("referencedMemberDecl"))
            if v is not None:
                return self.number(v)
        if k in ("CXXNullPtrLiteralExpr", "GNUNullExpr") and ENGINE_NODES:   # (no object of the engine: the handle 0)
            return 0
        raise Wait(f"a constant written as {k}")

    def field_of(self, value, field_id):
        """What sets the field `field_id` in a value made of known parts - an aggregate's braces
        (`Gains{0.003f, ...}`), copied or not -: that part, or the field's default (an ImplicitValueInitExpr:
        zero). None: the value is none such."""
        v = bare(value)
        while v.get("kind") in ("ImplicitCastExpr", "CXXFunctionalCastExpr", "CXXStaticCastExpr") and kids(v):
            v = bare(kids(v)[-1])
        if v.get("kind") == "CXXConstructExpr" and len(kids(v)) == 1 and self.unit.clean(qtype(kids(v)[0])) == self.unit.clean(qtype(v)):
            return self.field_of(kids(v)[0], field_id)   # (a copy)
        if v.get("kind") != "InitListExpr":
            return None
        q = self.unit.find_type(self.unit.clean(qtype(v)))
        record = self.unit.tree.records.get(q)
        if record is None or record.get("bases"):
            return None
        fields = [c for c in kids(record) if c.get("kind") == "FieldDecl"]
        at = next((i for i, c in enumerate(fields) if c.get("id") == field_id), None)
        if at is None:
            return None
        parts = kids(v)
        if at < len(parts) and parts[at].get("kind") not in ("ImplicitValueInitExpr", "CXXDefaultInitExpr"):
            return parts[at]
        f = fields[at]
        return kids(f)[-1] if kids(f) and f.get("hasInClassInitializer") else {"kind": "ImplicitValueInitExpr"}

    def inlined(self, n):
        """A call of a constexpr function that returns one expression of its parameters
        (`constexpr Vec3 hex_color(u32 h) { return {...}; }`), with constants for them: that expression,
        the parameters standing for what the call gives - worked out as the compiler works it out at
        compile time. None: the call is none such."""
        n = bare(n)
        if n.get("kind") != "CallExpr":
            return None
        c = callee(n)
        d = self.unit.tree.body.get(c.get("referencedDecl", {}).get("id")) if c.get("kind") == "DeclRefExpr" else None
        if d is None or not d.get("constexpr") or d.get("kind") != "FunctionDecl":
            return None
        body = next((x for x in kids(d) if x.get("kind") == "CompoundStmt"), None)
        stmts = kids(body) if body is not None else []
        if len(stmts) != 1 or stmts[0].get("kind") != "ReturnStmt" or not kids(stmts[0]):
            return None
        params = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
        args = kids(n)[1:]
        if len(args) != len(params) or any(a.get("kind") == "CXXDefaultArgExpr" for a in args):
            return None
        by = {p["id"]: a for p, a in zip(params, args)}
        out = copy.deepcopy(kids(stmts[0])[0])
        for x in walk(out):
            if x.get("kind") == "DeclRefExpr" and x.get("referencedDecl", {}).get("id") in by:
                arg = copy.deepcopy(by[x["referencedDecl"]["id"]])
                x.clear()
                x.update(arg)
        return out

    def constructed(self, n):
        """A value made by a constructor that only sets fields from what it is given
        (`constexpr Vec4(f32 x_, f32 y_, f32 z_, f32 w_) : x(x_), y(y_), z(z_), w(w_) {}`, `explicit
        PT1(f32 cutoff) : cutoff_(cutoff) {}`): its fields by their C++ names -> the expressions they are
        set to, the parameters standing for what is given. Fields it does not set have their defaults.
        (Constexpr or not, such a constructor makes the same value of the same arguments: what the
        expressions are is worked out as a constant - `number`, `literal` -, or the constant waits.) None:
        the constructor is none such."""
        d = constructor_of(self.unit.tree, n)
        if d is None:
            return None
        params = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
        args = [a for a in kids(n)]
        if len(args) != len(params) or any(a.get("kind") == "CXXDefaultArgExpr" for a in args):
            return None
        body = next((x for x in kids(d) if x.get("kind") == "CompoundStmt"), None)
        if body is None or kids(body) or any(x.get("kind") not in ("ParmVarDecl", "CXXCtorInitializer", "CompoundStmt") for x in kids(d)):
            return None
        by = {p["id"]: a for p, a in zip(params, args)}
        out = {}
        for x in kids(d):
            if x.get("kind") != "CXXCtorInitializer":
                continue
            field = (x.get("anyInit") or {}).get("name")
            if not field or not kids(x):
                return None   # (a base class, a delegating constructor)
            e = kids(x)[0]
            if e.get("kind") == "CXXDefaultInitExpr":
                continue
            e = copy.deepcopy(e)
            for y in walk(e):
                if y.get("kind") == "DeclRefExpr" and y.get("referencedDecl", {}).get("id") in by:
                    arg = copy.deepcopy(by[y["referencedDecl"]["id"]])
                    y.clear()
                    y.update(arg)
            out[field] = e
        return out

    FLOAT_LIMITS = {"float": {"max": 3.4028234663852886e+38, "min": 1.1754943508222875e-38, "epsilon": 1.1920928955078125e-07},
                    "double": {"max": 1.7976931348623157e+308, "min": 2.2250738585072014e-308, "epsilon": 2.220446049250313e-16}}

    def limit(self, n):
        """std::numeric_limits<T>::max(), min(), lowest(), epsilon(): the number it is (None: no such call)."""
        if n.get("kind") != "CallExpr" or len(kids(n)) != 1:
            return None
        m = re.search(r"numeric_limits\s*<\s*(.+?)\s*>\s*::\s*(max|min|lowest|epsilon)$", re.sub(r"\s+", " ", self.unit.tree.source(callee(n))))
        if m is None:
            return None
        t, what = self.unit.clean(m.group(1)), m.group(2)
        if t in self.FLOAT_LIMITS:
            return -self.FLOAT_LIMITS[t]["max"] if what == "lowest" else self.FLOAT_LIMITS[t][what]
        bits = self.BITS.get(t)
        if bits is None or what == "epsilon":
            raise Wait(f"std::numeric_limits<{m.group(1)}>::{what}()")
        signed = t not in UNSIGNED and t != "whole64u"
        top = (1 << (bits - 1)) - 1 if signed else (1 << bits) - 1
        return top if what == "max" else (-top - 1 if signed else 0)

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
        return self.number(n)

    def literal(self, n, t):
        """The value of a constant expression, written as a literal of the Blueprint type t."""
        u = self.unit
        n = bare(n)
        k = n.get("kind")
        while k in ("ImplicitCastExpr", "CXXFunctionalCastExpr", "CXXStaticCastExpr") and t not in ("Float", "Int", "Bool") and kids(n):
            n = bare(kids(n)[-1])
            k = n.get("kind")
        inlined = self.inlined(n)
        if inlined is not None:
            return self.literal(inlined, t)
        if t == "Int64" and k not in ("InitListExpr", "CXXConstructExpr"):
            v = int(self.number(n)) % (1 << 64)   # (a 64-bit number written out: its bits)
            return f"Int64{{{v:#x}}}"
        if t == "Int64":
            return f"Int64{{{int(self.number(kids(n)[0])) % (1 << 64):#x}}}" if kids(n) else "Int64{0x0}"
        if t == "Float":
            return flit(self.decimal(n))
        if t == "Int":
            v = self.number(n)
            if abs(v) >= 2.0 ** 53:
                raise Wait("a 64-bit whole number that wraps around or is shifted (a Blueprint's are exact up to 2^53)")
            return str(int(v))
        if t == "Bool":
            return "true" if self.number(n) != 0 else "false"
        if t == "Text":
            if k == "StringLiteral":
                return text_literal(c_string(n["value"]))
            if k in ("CXXConstructExpr", "InitListExpr") and not kids(n):
                return '""'
            if k == "CXXConstructExpr" and len([c for c in kids(n) if c.get("kind") != "CXXDefaultArgExpr"]) == 1:
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
            if k == "InitListExpr" and len(kids(n)) == 1:   # (`FaultCode fault{kFaultNone}`)
                return self.literal(kids(n)[0], t)
            if k == "DeclRefExpr" and n.get("referencedDecl", {}).get("kind") == "EnumConstantDecl":
                return f"{t}::{enum_value(u, t, n['referencedDecl']['name'])}"
            if k in ("CXXDefaultInitExpr", "ImplicitValueInitExpr", "InitListExpr") and not kids(n):
                return u.zero(t)
            raise Wait("an enum constant that is not one of its values")
        if t in u.rows:
            return t + "{" + self.literal(n, u.rows[t]) + "}"
        if t in u.struct_fields:
            if k in ("InitListExpr", "CXXConstructExpr", "CXXTemporaryObjectExpr", "CXXDefaultInitExpr", "ImplicitValueInitExpr"):
                args = [c for c in kids(n) if c.get("kind") != "CXXDefaultArgExpr"]
                if k == "CXXConstructExpr" and len(args) == 1 and u.clean(qtype(args[0])) == u.clean(qtype(n)):
                    return self.literal(args[0], t)
                if k == "CXXConstructExpr" and args and not u.clean(qtype(n)).startswith("std::pair<"):
                    inits = self.constructed(n)
                    if inits is None:
                        raise Wait("a struct constant made by a constructor")
                    vals = [self.literal(inits[c], ft) if c in inits else d for f, ft, d, c in u.struct_fields[t]]
                    return t + "{" + ", ".join(vals) + "}"
                fields = u.struct_fields[t]
                vals = []
                for i, (f, ft, d, _) in enumerate(fields):
                    if i < len(args) and args[i].get("kind") not in ("ImplicitValueInitExpr", "CXXDefaultInitExpr"):
                        vals.append(self.literal(args[i], ft))
                    else:
                        vals.append(d)
                return t + "{" + ", ".join(vals) + "}"
            if k in ("CallExpr", "CXXOperatorCallExpr", "CXXMemberCallExpr"):
                # (known only once the function has run: a constant of the language is a value written out)
                raise Wait("a struct constant computed by a function")
            raise Wait(f"a struct constant written as {k}")
        if t.startswith("List<"):
            it = t[5:-1]
            if k in ("InitListExpr", "CXXConstructExpr"):
                args = [c for c in kids(n) if c.get("kind") != "CXXDefaultArgExpr"]
                if k == "CXXConstructExpr":
                    if len(args) == 1 and bare(args[0]).get("kind") in ("InitListExpr", "CXXStdInitializerListExpr"):
                        return self.literal(args[0], t)
                    if args:
                        raise Wait("a list constant made by a constructor")
                if len(args) == 1 and bare(args[0]).get("kind") == "InitListExpr" and qtype(args[0]).rstrip().endswith("]"):
                    args = kids(bare(args[0]))   # (std::array's braces: those of the array it holds)
                elif len(args) == 1 and bare(args[0]).get("kind") == "ImplicitValueInitExpr" and qtype(args[0]).rstrip().endswith("]"):
                    args = []   # (`std::array<u16, 16> ch{}`: the array it holds is zero)
                # (an array of C++ - a constant, the default of a field - is declared with its length: what
                # is not written is zero there, in the Blueprint as in C++)
                return t + "{" + ", ".join(self.literal(c, it) for c in args) + "}"
            if k == "CXXStdInitializerListExpr":
                return self.literal(kids(n)[0], t)
            raise Wait(f"a list constant written as {k}")
        raise Wait(f"a constant of type {t}")


def sized(n):
    """The C++ type a sizeof (or alignof) measures: the one it names, or its expression's - None for another node."""
    if n.get("kind") != "UnaryExprOrTypeTraitExpr" or n.get("name") != "sizeof":
        return None
    if n.get("argType"):
        return qtype({"type": n["argType"]})
    return qtype(kids(n)[0]) if kids(n) else None


def enum_value(unit, enum_name, cpp_value):
    q = next((k for k, v in unit.enums.items() if v == enum_name), None)
    cpp_value = unit.enum_aliases.get((q, cpp_value), cpp_value)
    return cpp_value + "_" if is_word(cpp_value) else cpp_value


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
# (The language has no step that puts a list in an order: a loop of Blueprint steps would cost more than the
# library's sort - and of entries the order says are equal, std::sort leaves their order to the library.)
ORDER_WAIT = "algorithm: std::{} (it leaves the order of equal entries open - std::stable_sort is Sort List)"
COMPARE = ("<", "<=", ">", ">=", "==", "!=")


class Emit:
    def __init__(self, unit, f):
        self.u = unit
        self.f = f
        self.tree = unit.tree
        self.names = {}       # decl id -> the Blueprint name of a parameter or local
        self.scopes = []      # per block being written: the objects of the engine made in it, (local, class) - released where it ends
        self.types = {}       # decl id -> its Blueprint type
        self.locals = []      # (name, type): declared at the top of the body
        self.lengths = {}     # the local variables that are lists of a fixed length (name -> length)
        self.taken = set()
        self.index = {}       # decl id of a loop's counter -> "n12.index"
        self.pre = []         # steps to run before the statement being written
        self.loops = []       # the loops around the statement being written: their flags for break and continue
        self.frozen = None    # while the list of a loop is named: the lines that hold its indexes (see `fixed`)
        self.result_struct = None
        self.homes = {}       # decl id of an iterator (or of what insert() gave) -> the list it goes through
        self.pointers = {}    # decl id of a parameter that is a pointer into a list -> (the list's input, the place's input, the entries' type)
        self.keyed_vars = {}  # decl id of a name for an entry of a map or a set (a loop's variable, `auto& [k, v]`) -> (the map or set, its place)
        self.serial = 0       # the number of the statement being written (see `slot`)

    # ---- types ----
    def tp(self, n):
        """The Blueprint type of an expression: what the Blueprint holds there - a variable has the type it was
        declared with, a field the struct's -, else what its C++ type says."""
        t = self.held_type(n)
        if t is not None:
            return self.u.rows.get(t, t)   # (a list inside a list is held as a struct of that list: here, the list)
        if self.u.object_of(qtype(n)):
            return "Int"   # (an object of the engine: its handle)
        return self.u.btype(qtype(n))

    def held_type(self, n):
        b = n if "_bt" in n else bare(n)
        k = b.get("kind")
        if "_bt" in b:
            return b["_bt"]
        if k == "DeclRefExpr" and b.get("referencedDecl", {}).get("id") in self.types:
            return self.types[b["referencedDecl"]["id"]]
        try:
            if k == "MemberExpr" and kids(b) and not self.is_this(kids(b)[0]):
                e = self.entry_of(kids(b)[0])
                st = self.tp(e if e is not None else kids(b)[0])
                if st in self.u.struct_fields and not (b.get("isArrow") and e is None) and st not in self.u.rows:
                    return self.u.field(st, b["name"])[1]
            if k == "CXXOperatorCallExpr" and not b.get("_op"):
                e = self.entry_of(b)
                if e is not None:
                    return e["_bt"]
                if self.is_slot(b):
                    return self.rows(kids(b)[1])[3]
        except Wait:
            pass
        return None

    def copied_type(self, init):
        """The type of the value `auto x = init;` copies, as the Blueprint holds it (None: what C++ says)."""
        b = bare(init)
        while (b.get("kind") == "ImplicitCastExpr" and b.get("castKind") in ("LValueToRValue", "NoOp") or b.get("kind") in ("CXXConstructExpr", "CXXBindTemporaryExpr")) and len(kids(b)) == 1:
            b = bare(kids(b)[0])
        t = self.held_type(b)
        return self.u.rows.get(t, t) if t is not None else None

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
        if t.startswith("List<") and self.u.fixed(qtype(decl)):   # (an array of C++: declared with its length)
            self.lengths[name] = self.u.fixed(qtype(decl))
        return name

    def temp(self, want, t, fixed=None):
        """A local variable of the function's own making (`fixed`: a list of that fixed length - what holds
        a value for a reference of that type)."""
        name = self.u.fresh(want, self.taken)
        self.taken.add(name)
        self.locals.append((name, t))
        if fixed:
            self.lengths[name] = fixed
        return name

    def declared(self, name, t):
        """What a local variable is declared as: its type, a list of a fixed length with its length."""
        return self.u.fixed_type(t, self.lengths.get(name))

    # ---- the function ----
    def run(self):
        d, f, u = self.f.decl, self.f, self.u
        if d.get("variadic"):
            raise Wait("a variadic function")
        ret = qtype(d).split("(")[0].strip()
        f.value = u.btype(ret) if d.get("kind") != "CXXConstructorDecl" else None
        if u.state is not None and (self.tree.owner.get(d["id"]) == u.state or function_key(self.tree, d) in u.world.state_functions):
            # a method of the state the Blueprint is: its fields are the Blueprint's variables - no object is
            # handed in. It changes them unless it says it does not (const) and writes no mutable field.
            if d.get("kind") == "CXXConstructorDecl":
                raise Wait(f"the constructor of the state {u.name} (what it does would run when the Blueprint begins)")
            f.state = True
            for x in walk(d):   # (what the file keeps for the state's methods alone: variables of its Blueprint - Unit.kept_by_state)
                if x.get("kind") == "DeclRefExpr" and (x.get("referencedDecl") or {}).get("kind") == "VarDecl":
                    v = self.tree.decl.get(x["referencedDecl"]["id"])
                    if v is not None and v["id"] not in self.names and u.kept_by_state(v):
                        self.names[v["id"]], self.types[v["id"]] = u.state_static(v, os.path.splitext(os.path.basename(v.get("_file") or "file"))[0])
            method = self.tree.owner.get(d["id"]) == u.state
            f.mutates = (method and not qtype(d).rstrip().endswith("const")) or any(
                x.get("kind") == "MemberExpr" and (u.state_fields.get(x.get("name")) or (None, None, {}))[2].get("mutable") and self.is_this(kids(x)[0] if kids(x) else {"kind": "CXXThisExpr"})
                for x in walk(d))
            # (what it keeps from one call to the next, or of the file - changed or not: it may be)
            f.mutates = f.mutates or any(x.get("kind") == "VarDecl" and x.get("storageClass") == "static" and not x.get("constexpr") and not re.search(r"\bconst\b", qtype(x)) for x in walk(d)) \
                or any(i in self.names and self.names[i] in {n for n, _ in u.state_statics.values()} for i in self.assigned(d))
            if f.mutates:
                f.pure = False   # (it changes the Blueprint's variables: a step)
        if d.get("kind") == "CXXConstructorDecl":
            # a constructor: a function that makes the object - it begins as the struct's default (the
            # defaults of the class's data), then what the constructor sets
            owner = self.tree.owner.get(d["id"])
            if owner is None:
                raise Wait("a constructor of a class that is not known here")
            try:
                f.self_type = u.struct(owner)
            except Wait as w:
                raise Wait(f"the state of its object: {w}")
            f.mutates = f.ctor = True
        if "&" in ret and not re.search(r"\bconst\b", ret):
            raise Wait("a function that gives a reference to something to change")
        body = next(c for c in kids(d) if c.get("kind") == "CompoundStmt")
        self.dissolve(d)   # (a call of a function that names a place: the place)
        # a method: its object is the first parameter - a reference to a value of the struct its class's data is:
        # the place the call names, changed where it stands (a const method reads it there)
        if d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static" and not f.state and any(x.get("kind") == "CXXThisExpr" for x in walk(d)):
            owner = self.tree.owner.get(d["id"])
            if owner is None:
                raise Wait("the state of its object (its class is not known here)")
            try:
                f.self_type = u.struct(owner)
            except Wait as w:
                raise Wait(f"the state of its object: {w}")
            f.mutates = not qtype(d).rstrip().endswith("const")
            name = u.fresh("self", self.taken)
            self.taken.add(name)
            f.params.append((name, f.self_type))
            f.kinds.append("ref" if f.mutates else "const")
            self.self_name = name
        assigned = self.assigned(body)
        copies = []
        for p in kids(d):
            if p.get("kind") != "ParmVarDecl":
                continue
            raw = qtype(p)
            entries = u.pointed(raw)
            if entries is not None:
                # a pointer into a list: the list, read where it stands, and the place in it (Unit.pointed)
                if p["id"] in assigned:
                    raise Wait("a pointer that is moved (the place it holds changes)")
                into = u.world.pointer_lists.get((f.key, [x["id"] for x in kids(d) if x.get("kind") == "ParmVarDecl"].index(p["id"])), "list")
                if into == "mixed":
                    raise Wait("a pointer into lists of different lengths (the language has no view of a list: a reference input is one type)")
                if into == "other":
                    raise Wait("a pointer into something that is no list (the address of a value)")
                if isinstance(into, int):
                    f.fixed[len(f.params)] = into   # (the list it points into: of that fixed length, as every call hands it)
                lname = u.fresh(p.get("name") or f"arg{len(f.params) + 1}", self.taken)
                self.taken.add(lname)
                aname = u.fresh(lname + "_at", self.taken)
                self.taken.add(aname)
                f.pointers.append(len(f.params))
                f.params += [(lname, f"List<{entries}>"), (aname, "Int")]
                f.kinds += ["const", ""]
                self.pointers[p["id"]] = (lname, aname, entries)
                continue
            t = u.btype(raw)
            # A reference of C++ is a reference input: the place the call names. T& - the function changes it
            # there; const T& - it reads it there (a struct, a list, a text: a number, a vector, an enum's value
            # or a handle is read by value either way). Nothing is copied in or out.
            by_ref = "&" in raw and "&&" not in raw and not u.object_of(raw)
            const = bool(re.search(r"\bconst\b", raw))
            kind = "ref" if by_ref and not const else "const" if by_ref and (t in u.struct_fields or t.startswith("List<") or t == "Text") else ""
            name = u.fresh(p.get("name") or f"arg{len(f.params) + 1}", self.taken)
            self.taken.add(name)
            if kind == "ref":
                f.refs.append(len(f.params))
            if u.fixed(raw) and t.startswith("List<"):   # (an array of C++: the input is declared with its length)
                f.fixed[len(f.params)] = u.fixed(raw)
            f.params.append((name, t))
            f.kinds.append(kind)
            self.names[p["id"]] = name
            self.types[p["id"]] = t
            if not kind and p["id"] in assigned:  # (an input by value is not a variable: what the body changes is a local copy)
                copies.append((p, name, t))
        if "ref" in f.kinds:
            f.pure = False   # (it changes a place of its caller: a step, not a value)
        # what a call changes or gives, as the test compares it: the object, what the C++ returns, what it wrote
        # through its parameters. The function itself gives what the C++ returns (a constructor: the object).
        if f.mutates and f.state:
            f.out_fields.append(("State", u.name, "state"))   # (the test compares all the Blueprint's variables)
        elif f.mutates:
            f.out_fields.append(("Self", f.self_type, "self"))
        if f.value is not None:
            f.out_fields.append(("Result", f.value, "result"))
        taken = {"Self", "Result"}
        for place in f.refs:
            field = u.fresh(f.params[place][0], taken)
            taken.add(field)
            f.out_fields.append((field, f.params[place][1], ("ref", place)))
        f.result = f.self_type if f.ctor else f.value
        lines = []
        if f.ctor:
            self.self_name = self.temp("self", f.self_type)
            for init in kids(d):
                if init.get("kind") != "CXXCtorInitializer":
                    continue
                if "anyInit" not in init:
                    raise Wait("a constructor that calls another constructor or a base class")
                e = kids(init)[0] if kids(init) else None
                if e is None or bare(e).get("kind") == "CXXDefaultInitExpr":
                    continue   # (the data's own default: the struct has it)
                at = d.get("loc") or {}
                if e.get("_at", (None,))[0] is not None and e["_at"][0] == at.get("expansionLoc", at).get("offset"):
                    # (what the constructor does not write, the compiler does - it stands where the constructor's
                    # name stands: a field made as its class makes it. The struct has that as its default.)
                    continue
                fname, ft = u.field(f.self_type, init["anyInit"]["name"])
                if u.object_of(qtype(init["anyInit"])) and (self.maker_of(e) is not None or any(callee_name(x) == "make_unique" for x in walk(e) if x.get("kind") == "CallExpr")):
                    # (an object of the engine its owner makes: it ends with its owner - which a value of the
                    # Blueprint has not; see the plan for the fields of objects of the engine)
                    raise Wait("an object of the engine as a field, made with its owner")
                saved, self.pre = self.pre, []
                text = self.ex(e, ft)
                lines += self.pre + [f"{self.self_name}.{fname} = {text};"]
                self.pre = saved
        for p, name, t in copies:
            c = self.temp(name + "_", t, self.u.fixed(qtype(p)) if t.startswith("List<") else None)
            self.names[p["id"]] = c
            lines.append(f"{c} = {name};")
        lines += self.block(kids(body))
        if not self.ends(kids(body)) and f.ctor:
            lines.append(self.ret_line(None))
        head = []
        doc = u.comment_of(d)
        if f.result is not None:
            if f.value is not None and not f.ctor and f.value.startswith("List<"):
                f.result_fixed = u.fixed(ret)   # (it returns an array of C++: its output is declared with its length)
            given = u.fixed_type(f.result, f.result_fixed)
            head.append(f"  struct {f.name}Result {{ {given} Result = {u.zero(given)}; }};")
        if doc:
            head.append(f"  /// {doc}")
        takes = [u.fixed_type(f.params[i][1], f.fixed.get(i)) for i in range(len(f.params))]
        params = ", ".join(f"{takes[i]}& {f.params[i][0]}" if f.kinds[i] == "ref" else f"const {takes[i]}& {f.params[i][0]}" if f.kinds[i] == "const"
                           else f"{takes[i]} {f.params[i][0]} = {u.zero(takes[i])}" for i in f.order())
        head.append(f"  {'KERN_PURE ' if f.pure else ''}{f.name + 'Result' if f.result is not None else 'void'} {f.name}({params}) {{")
        out = head + [f"    {self.declared(n, t)} {n} = {u.zero(self.declared(n, t))};" for n, t in self.locals] + ["    " + ln for ln in lines] + ["  }"]
        f.local_names = set(self.taken)   # (its inputs and local variables: see convert_unit)
        f.text = "\n".join(out)

    def ret_line(self, value):
        """The return of the function: what the C++ returns as it is at this place (a constructor: the object)."""
        if self.f.ctor:
            return f"return {{{self.self_name}}};"
        if self.f.value is None:
            return "return;"
        return f"return {{{value}}};"

    def self_var(self):
        if not self.f.mutates:
            raise Wait("a method that changes its object though it says it does not (const)")
        return self.self_name

    def own(self, decl_id):
        """Is this a function to write as a Blueprint function - one of the program, or one the engine's
        headers define? (Not what the notation has itself: the vectors' own operations, the functions
        of numbers.)"""
        return own_function(self.tree, decl_id)

    def is_this(self, n):
        n = bare(n)
        while n.get("kind") in ("ImplicitCastExpr",) and kids(n):
            n = bare(kids(n)[0])
        if n.get("kind") == "UnaryOperator" and n.get("opcode") == "*" and kids(n):
            return self.is_this(kids(n)[0])
        return n.get("kind") == "CXXThisExpr"

    def hoist(self, lines):
        """Steps that run before the statement being written (a call that gives several things)."""
        if self.cond_depth:
            raise Hoists("a call that changes something, in a part of an expression that does not always run")
        self.pre += lines

    def lazily(self, fn):
        """Writes a part of an expression apart from the statement it stands in: (the steps it needs, its text)."""
        saved, self.pre = self.pre, []
        depth, self.cond_depth = self.cond_depth, 0
        try:
            text = fn()
            return self.pre, text
        finally:
            self.pre, self.cond_depth = saved, depth

    def either(self, op, left, y):
        """a && b, a || b where b needs steps of its own: they run only when C++ would evaluate b."""
        pre, right = self.lazily(lambda: self.bx(y))
        t = self.temp("both" if op == "&&" else "either", "Bool")
        self.hoist([f"{t} = {left};", f"if ({t if op == '&&' else f'logic_not({t})'}) {{", *["  " + ln for ln in pre + [f"{t} = {right};"]], "}"])
        return t, "Bool"

    def chosen(self, cond, x, y, t):
        """c ? x : y where a branch needs steps of its own: an if that sets a variable."""
        (px, tx), (py, ty) = self.lazily(lambda: self.ex(x, t)), self.lazily(lambda: self.ex(y, t))
        tmp = self.temp("chosen", t)
        self.hoist([f"if ({cond}) {{", *["  " + ln for ln in px + [f"{tmp} = {tx};"]], "} else {", *["  " + ln for ln in py + [f"{tmp} = {ty};"]], "}"])
        return tmp, t

    def sometimes(self, fn):
        """Writes a part of an expression that C++ does not always evaluate (the right of && and ||, a branch of ?:)."""
        self.cond_depth += 1
        try:
            return fn()
        finally:
            self.cond_depth -= 1

    pure_ok = True
    self_name = None
    cond_depth = 0

    def root(self, target):
        """The variable an lvalue is a part of (its declaration's id), or None."""
        while target is not None:
            target = bare(target)
            if target.get("kind") == "DeclRefExpr":
                return target.get("referencedDecl", {}).get("id")
            if target.get("kind") in ("MemberExpr", "ArraySubscriptExpr", "ImplicitCastExpr") and kids(target):
                target = kids(target)[0]
            elif target.get("kind") == "CXXOperatorCallExpr" and callee_name(target) == "operator[]":
                target = kids(target)[1]
            else:
                return None
        return None

    def assigned(self, body):
        """The variables the body changes: by assigning, or through a call that writes to its object or
        through a parameter."""
        out = set()
        for n in walk(body):
            k = n.get("kind")
            targets = []
            if k in ("BinaryOperator", "CompoundAssignOperator") and (n.get("opcode") == "=" or k == "CompoundAssignOperator"):
                targets.append(kids(n)[0])
            elif k == "UnaryOperator" and n.get("opcode") in ("++", "--"):
                targets.append(kids(n)[0])
            elif k == "CXXOperatorCallExpr" and callee_name(n) in ("operator=", "operator+=", "operator-=", "operator*=", "operator/="):
                targets.append(kids(n)[1])
            elif k == "CXXMemberCallExpr" and callee(n).get("name") in ("push_back", "emplace_back", "clear", "pop_back", "resize", "erase", "insert", "assign", "emplace", "try_emplace", "insert_or_assign", "swap"):
                if kids(callee(n)):
                    targets.append(kids(callee(n))[0])
            elif k == "CXXMemberCallExpr" and callee(n).get("name") in ("find", "begin", "end") and kids(callee(n)) and re.match(r"std::__detail::_Node_iterator<|std::_Rb_tree_iterator<|__gnu_cxx::__normal_iterator<(?!const )", qtype(n)):
                targets.append(kids(callee(n))[0])   # (an iterator through which the list may be changed)
            elif k == "CXXOperatorCallExpr" and callee_name(n) == "operator[]" and len(kids(n)) == 3 and KEYED_KERN.match(self.u.clean(qtype(kids(n)[1]))):
                targets.append(kids(n)[1])   # (m[key] makes the key's row when there is none)
            if k in ("CallExpr", "CXXMemberCallExpr", "CXXOperatorCallExpr"):
                c = callee(n)
                d = self.tree.decl.get(c.get("referencedMemberDecl") or c.get("referencedDecl", {}).get("id"))
                if d is not None:
                    args = kids(n)[1:]
                    if d.get("kind") == "CXXMethodDecl":
                        obj = kids(c)[0] if k == "CXXMemberCallExpr" and kids(c) else args[0] if k == "CXXOperatorCallExpr" and args else None
                        if k == "CXXOperatorCallExpr":
                            args = args[1:]
                        if obj is not None and not qtype(d).rstrip().endswith("const"):
                            targets.append(obj)
                    params = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
                    for a, p in zip(args, params):
                        raw = qtype(p)
                        if "&" in raw and "&&" not in raw and not re.search(r"\bconst\b", raw):
                            targets.append(a)
            for t in targets:
                i = self.root(t)
                if i is not None:
                    out.add(i)
        return out

    # ---- text formatted into a buffer ----
    def text_buffer(self, v):
        """`char b[64];` that is a text: every use of it is the buffer snprintf writes into (its first
        value) or a read of it as a text (const char*), and `sizeof b`. Its length (None: it is none such)."""
        m = re.fullmatch(r"char\s*\[(\d+)\]", self.u.clean(qtype(v)))
        if not m or self.f.decl is None:
            return None
        cache = self.__dict__.setdefault("_text_buffers", {})
        if v["id"] in cache:
            return cache[v["id"]]
        ok = True
        def uses(n, parents):
            nonlocal ok
            if n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") == v["id"]:
                while parents and parents[-1].get("kind") == "ImplicitCastExpr" and parents[-1].get("castKind") == "NoOp" and "[" in qtype(parents[-1]):
                    parents = parents[:-1]   # (the array as it is)
                up = parents[-1] if parents else {}
                if up.get("kind") == "UnaryExprOrTypeTraitExpr" and up.get("name") == "sizeof":
                    return
                if up.get("kind") == "LambdaExpr":
                    return   # (captured by a local function: what it does with it is among the uses walked here)
                if not (up.get("kind") == "ImplicitCastExpr" and up.get("castKind") == "ArrayToPointerDecay"):
                    ok = False
                    return
                above = parents[-2] if len(parents) > 1 else {}
                if above.get("kind") == "ImplicitCastExpr" and above.get("castKind") == "NoOp" and re.fullmatch(r"const char\s*\*", qtype(above).strip()):
                    return   # (read as a text)
                if above.get("kind") == "CallExpr" and callee_name(above) == "snprintf" and len(kids(above)) > 1 and kids(above)[1] is up:
                    return   # (written by snprintf)
                ok = False
                return
            for c in kids(n):
                uses(c, parents + [n])
        uses(self.f.decl, [])
        cache[v["id"]] = int(m.group(1)) if ok else None
        return cache[v["id"]]

    def text_buffer_of(self, n):
        """The length of the text buffer an expression names (see text_buffer), or None."""
        b = n
        while b.get("kind") in ("ImplicitCastExpr", "ParenExpr") and kids(b):
            b = kids(b)[0]
        if b.get("kind") != "DeclRefExpr":
            return None
        d = self.tree.decl.get(b.get("referencedDecl", {}).get("id")) or b.get("referencedDecl", {})
        return self.text_buffer(d) if d.get("kind") == "VarDecl" and d.get("id") in self.names else None

    @staticmethod
    def format_kinds(fmt):
        """The types of the values a printf format takes (logic::format): Int, Float or Text each - None:
        a format the node Format Text does not know."""
        out, at = [], 0
        while at < len(fmt):
            if fmt[at] != "%":
                at += 1
                continue
            m = re.match(r"%([-+ 0#]*)(\d*)(?:\.(\d*))?(hh|h|ll|l|j|z|t)?([diuxXocsfFeEgG%])", fmt[at:])
            if not m or len(m.group(2)) > 3 or (m.group(3) or "") and len(m.group(3)) > 3:
                return None
            if m.group(2) and int(m.group(2)) > 512 or m.group(3) and int(m.group(3)) > 512:
                return None
            c = m.group(5)
            if c != "%":
                out.append("Float" if c in "fFeEgG" else "Text" if c == "s" else "Int")
            at += m.end()
        return out

    def formatted(self, n):
        """`snprintf(b, sizeof b, "%02d:%06.3f", m, s);` into a text buffer: b is the text of Format Text,
        cut where snprintf cuts it (its size - 1 bytes)."""
        a = kids(n)[1:]
        if len(a) < 3:
            raise Wait("snprintf without a format")
        room = self.text_buffer_of(a[0])
        size = bare(a[1])
        while size.get("kind") in CAST and kids(size):
            size = bare(kids(size)[-1])
        if size.get("kind") == "UnaryExprOrTypeTraitExpr" and size.get("name") == "sizeof" and kids(size) and self.text_buffer_of(kids(size)[0]) == room:
            most = room - 1
        elif size.get("kind") == "IntegerLiteral" and 0 < int(size.get("value", "0")) <= room:
            most = int(size["value"]) - 1
        else:
            raise Wait("snprintf with a size that is not the buffer's")
        f = bare(a[2])
        while f.get("kind") in CAST and kids(f):
            f = bare(kids(f)[-1])
        if f.get("kind") != "StringLiteral":
            raise Wait("a format that is not written out (Format Text has its format as a setting)")
        fmt = c_string(f["value"])
        kinds = self.format_kinds(fmt.decode("latin-1") if isinstance(fmt, bytes) else fmt)
        if kinds is None:
            raise Wait("a format Format Text does not know (d i u x X o c s f e g, flags, width, precision)")
        if len(kinds) != len(a) - 3:
            raise Wait("a format whose conversions are not its values")
        values = [self.ex(x, t) for x, t in zip(a[3:], kinds)]
        b = a[0]
        while b.get("kind") in ("ImplicitCastExpr", "ParenExpr") and kids(b):
            b = kids(b)[0]
        target = self.names[b["referencedDecl"]["id"]]
        return self.take_pre() + [f"{target} = text_format<{text_literal(fmt)}>({', '.join([str(most)] + values)});"]

    # ---- a function that names a place ----
    PLAIN = frozenset(("DeclRefExpr", "MemberExpr", "ImplicitCastExpr", "ParenExpr", "IntegerLiteral", "FloatingLiteral", "CXXBoolLiteralExpr", "CharacterLiteral", "StringLiteral",
                       "ArraySubscriptExpr", "CStyleCastExpr", "CXXStaticCastExpr", "CXXFunctionalCastExpr", "CXXThisExpr", "ConstantExpr", "MaterializeTemporaryExpr",
                       "CXXNullPtrLiteralExpr", "SubstNonTypeTemplateParmExpr"))
    fresh_ids = itertools.count(1)

    def plain(self, n):
        """Can an expression be worked out any number of times - it changes nothing and calls nothing?"""
        for x in walk(n):
            k = x.get("kind")
            if k in self.PLAIN:
                continue
            if k == "UnaryOperator" and x.get("opcode") not in ("++", "--"):
                continue
            if k == "BinaryOperator" and x.get("opcode") not in ("=", ","):
                continue
            return False
        return True

    def copied(self, n):
        """A copy of an expression of the tree, each node an expression of its own (ids of its own)."""
        c = copy.deepcopy(n)
        for x in walk(c):
            x.pop("_call", None)
            if "id" in x and not x.get("kind", "").endswith("Decl"):
                x["id"] = f"{x['id']}~{next(self.fresh_ids)}"
        return c

    def placed(self, n, inside=()):
        """The place a call names, written out: the expression the called function returns (Unit.place_named)
        - of the object the call names, with what the call gives for its parameters. None: the call is none
        of a function that names a place, or it cannot be written so (what it is given would be worked out
        another number of times than the call works it out)."""
        c = callee(n)
        obj, arrow = None, False
        if n.get("kind") == "CXXMemberCallExpr":
            if c.get("kind") != "MemberExpr" or not kids(c):
                return None
            i, obj, arrow = c.get("referencedMemberDecl"), kids(c)[0], bool(c.get("isArrow"))
        elif n.get("kind") == "CallExpr":
            if c.get("kind") != "DeclRefExpr":
                return None
            i = c.get("referencedDecl", {}).get("id")
        else:
            return None
        d = self.tree.body.get(i) if i else None
        e = self.u.place_named(d)
        if e is None or d["id"] in inside:
            return None
        params = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
        args = kids(n)[1:]
        if len(args) != len(params) or any(a.get("kind") == "CXXDefaultArgExpr" for a in args):
            return None
        by = {p["id"]: a for p, a in zip(params, args)}
        uses = collections.Counter(x.get("referencedDecl", {}).get("id") for x in walk(e) if x.get("kind") == "DeclRefExpr")
        if any(uses.get(p["id"], 0) != 1 and not self.plain(a) for p, a in zip(params, args)):
            return None
        if obj is not None and sum(1 for x in walk(e) if x.get("kind") == "CXXThisExpr") != 1 and not self.plain(obj):
            return None
        out = self.copied(e)
        def put(x, member=None):
            k = x.get("kind")
            if k == "DeclRefExpr" and x.get("referencedDecl", {}).get("id") in by:
                arg = self.copied(by[x["referencedDecl"]["id"]])
                x.clear()
                x.update(arg)
                return True
            if k == "CXXThisExpr":
                if obj is None:
                    return True   # (a method called without naming an object: the caller's own - it stays `this`)
                if not arrow and member is None:
                    return False   # (`this` as a pointer: no place stands for that here)
                held = self.copied(obj)
                x.clear()
                x.update(held)
                if not arrow:
                    member["isArrow"] = False
                return True
            if k == "UnaryOperator" and x.get("opcode") == "*" and obj is not None and not arrow and kids(x) and bare(kids(x)[0]).get("kind") == "CXXThisExpr":
                held = self.copied(obj)   # (`*this`: the object)
                x.clear()
                x.update(held)
                return True
            return all(put(y, x if k == "MemberExpr" else member if k in ("ImplicitCastExpr", "ParenExpr") else None) for y in kids(x))
        if not put(out):
            return None
        # (what the place itself calls: written out too)
        for x in list(walk(out)):
            if x.get("kind") in ("CallExpr", "CXXMemberCallExpr"):
                inner = self.placed(x, (*inside, d["id"]))
                if inner is not None:
                    x.clear()
                    x.update(inner)
        return out

    def dissolve(self, node):
        """Every call of a function that names a place, in `node`: written out as that place - as
        `T& x = a.b[i];` makes x a name for a.b[i] (name_references). The call stays with the node (`_call`)
        for the test: its source calls the function, as the program does (walk_source)."""
        for n in list(walk(node)):
            if n.get("kind") not in ("CallExpr", "CXXMemberCallExpr") or n.get("_call") is not None:
                continue
            place = self.placed(n)
            if place is not None:
                call = dict(n)
                n.clear()
                n.update(place)
                n["_call"] = call

    # ---- statements ----
    def name_references(self, stmts, i):
        """`T& x = a.b[i];` - a name for a place: in what follows, x is written out as a.b[i]. (A const
        reference is a copy of the value as long as what it names does not change while it lives.)"""
        s = stmts[i]
        if s.get("kind") != "DeclStmt":
            return
        for v in kids(s):
            raw = qtype(v)
            if v.get("kind") not in ("VarDecl", "DecompositionDecl") or self.init_of(v) is None or "&" not in raw or "&&" in raw or v.get("id") in self.named_places or self.u.object_of(raw):
                continue
            init = self.init_of(v)
            if self.lambda_of(init) is not None:
                continue
            target, root, own = self.unwrap(init), None, False
            while target is not None:   # (the variable the place is a part of - or the object of the method itself)
                target = bare(target)
                if target.get("kind") == "DeclRefExpr":
                    root = target.get("referencedDecl", {}).get("id")
                    break
                if target.get("kind") == "CXXThisExpr":
                    own = True
                    break
                if target.get("kind") in ("MemberExpr", "ArraySubscriptExpr", "ImplicitCastExpr") and kids(target):
                    target = kids(target)[0]
                elif target.get("kind") == "CXXOperatorCallExpr" and callee_name(target) in ("operator[]", "operator*", "operator->") and len(kids(target)) >= 2:
                    target = kids(target)[1]   # (an entry of a list, a map's value - and what an owned value holds: *life_)
                elif target.get("kind") == "UnaryOperator" and target.get("opcode") == "*" and kids(target):
                    target = kids(target)[0]
                else:
                    target = None
            if not own and (root is None or (root not in self.names and root not in self.keyed_vars)):
                continue   # (a place of a variable of the function, of an entry of a map it goes through - or of its object)
            rest = {"kind": "CompoundStmt", "inner": stmts[i + 1:]}
            changed = self.assigned(rest)
            if re.search(r"\bconst\b", raw) and not own and root not in changed:
                continue
            if (self.reads(init) - {root}) & changed:
                raise Wait("a reference to an entry at a place that changes while the reference lives")
            if v.get("kind") == "DecompositionDecl":   # (auto& [a, b] = place; - a and b are parts of the place)
                self.bind(v, self.tp(init))
            for n in walk(rest):
                if n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") == v["id"]:
                    n.clear()
                    n.update(init)
            self.named_places.add(v["id"])

    named_places = frozenset()

    def block(self, stmts):
        self.scopes.append([])
        try:
            out, ended = self.block_of(stmts)
        finally:
            made = self.scopes.pop()
        if made and len(stmts) == 1 and stmts[0].get("kind") == "DeclStmt":
            raise Wait("an object of the engine made in the declaration of an if, a for or a while")
        return out if ended else out + self.releases(made)

    def releases(self, made):
        """What ends the objects of the engine a block made, where its way out is (in the reverse order of
        their making, as C++ destroys them): each is released, if it was made - its handle is none after."""
        out = []
        for name, q in reversed(made):
            out += self.release(q, name) + [f"{name} = 0;"]
        return out

    def release(self, q, handle):
        """The release of an object of the engine (its destructor), if the handle is one: the node <class>.release."""
        u = self.u
        f = u.api().releaser(q)
        if u.engine_table().get(f["type"]) != (False, [(n, *u.api().pin(bt)) for n, bt, _ in f["ins"]], []):
            raise Wait(f"the release of a {q.split('::')[-1]} (its node is another one in the engine)")
        u.engine_calls[f["decl"]["id"] + ":release"] = f
        self.f.pure = False
        return [f'if (math_compare<"!=">(kern::to<Float>({handle}), 0.0)) {{', f'  {f["type"].replace(".", "_")}({handle});', "}"]

    def block_of(self, stmts):
        """The statements of a block, and whether it always returns (then its objects are released by the return)."""
        out = []
        for i, s in enumerate(stmts):
            if not isinstance(self.named_places, set):
                self.named_places = set()
            self.name_references(stmts, i)
            lines, ends = self.stmt(s)
            out += lines
            if ends:
                return out, True
            if s.get("kind") in ("BreakStmt", "ContinueStmt"):
                break
            if self.loops and any(self.jumps(s)):   # (after a statement that may break or continue: the rest only if it did not)
                rest = self.block(stmts[i + 1:])
                if rest:
                    out += [f"if ({self.going()}) {{", *["  " + ln for ln in rest], "}"]
                break
        return out, False

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
                while (s.get("hasInit") or s.get("hasVar")) and len(c) > 2 and c[0].get("kind") == "DeclStmt":
                    c = c[1:]
                if len(c) == 3 and self.ends([c[1]]) and self.ends([c[2]]):
                    return True
        return False

    def stmt(self, s):
        """The lines of one statement, and whether the path ends there."""
        saved, self.pre = self.pre, []
        self.serial += 1
        try:
            lines, ends = self.stmt1(s)
            return self.pre + lines, ends
        finally:
            self.pre = saved

    def take_pre(self):
        lines, self.pre = self.pre, []
        return lines

    def loop_lines(self, cond, cond_pre, body, loop=None):
        """A loop while `cond` holds. Steps the condition needs (a call that gives several things) run
        before every test of it. A break ends it: its flag is part of the condition."""
        stop = loop["stop"] if loop else None
        first = [f"{stop} = false;"] if stop else []
        if not cond_pre:
            if stop:
                cond = f"logic_not({stop})" if cond == "true" else f"logic_and(logic_not({stop}), {cond})"
            return first + [f"while (loop({cond})) {{", *["  " + ln for ln in body], "}"]
        go = self.temp("go", "Bool")
        test = f"logic_and({go}, logic_not({stop}))" if stop else go
        return first + [f"{go} = true;", f"while (loop({test})) {{", *["  " + ln for ln in cond_pre], f"  if ({cond}) {{", *["    " + ln for ln in body], "  } else {", f"    {go} = false;", "  }", "}"]

    def stmt1(self, s):
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
                if v.get("kind") == "DecompositionDecl":
                    # auto [a, b] = value; - the value in a variable of its own, a and b its parts
                    init = self.init_of(v)
                    if v.get("id") in self.named_places:
                        self.value(init)   # (a name for a place: what making the place does - a new row of a map - is done here)
                        out += self.take_pre()
                        continue
                    if init is None:
                        raise Wait("names for the parts of nothing")
                    text, have = self.value(init)
                    name = self.local(v, have)
                    self.bind(v, have)
                    self.note_home(v["id"], init, of_pair=True)
                    out += self.take_pre() + [f"{name} = {text};"]
                    continue
                if v.get("kind") in ("UsingDirectiveDecl", "UsingDecl", "TypeAliasDecl", "TypedefDecl", "StaticAssertDecl", "EnumDecl"):
                    continue   # (a name for what the compiler has resolved; an enum: a type like any other, or - without a name - numbers)
                if v.get("kind") == "CXXRecordDecl" and v.get("_q") in self.tree.local_types:
                    continue   # (a type of the function's own: a struct of the program, declared where it is first used)
                if v.get("kind") != "VarDecl":
                    raise Wait(f"a declaration in a body: {v.get('kind')}")
                if v.get("storageClass") == "static":
                    if v.get("constexpr") or re.search(r"\bconst\b", qtype(v)):
                        self.u.constant(v)
                        continue
                    if self.f.state:   # (kept by a method of the one object there is: a variable of its Blueprint)
                        name, t = self.u.state_static(v, self.f.name)
                        self.names[v["id"]], self.types[v["id"]] = name, t
                        if t.startswith("List<") and self.u.fixed(qtype(v)):
                            self.lengths[name] = self.u.fixed(qtype(v))
                        continue
                    raise Wait("a static local variable (kept from one call to the next: a library keeps nothing)")
                lam = self.lambda_of(kids(v)[-1]) if kids(v) and v.get("init") else None
                if lam is not None:
                    out += self.local_function(v, lam)
                    continue
                if v.get("id") in self.named_places:
                    # (a name for a place: written out where it is used - what making the place does, a new row of a map, is done here)
                    if any(x.get("kind") == "CXXOperatorCallExpr" and self.is_slot(x) for x in walk(kids(v)[-1])):
                        self.value(kids(v)[-1])
                        out += self.take_pre()
                    continue
                raw = qtype(v)
                if ("&" in raw and not re.search(r"\bconst\b", raw) and not self.u.object_of(raw)) or self.u.clean(raw) == "std::_Bit_reference":
                    raise Wait("a reference to something it changes")
                wide_init = kids(v) and v.get("init") and self.u.clean(raw) in ("unsigned long", "long", "unsigned long long", "long long") and self.is64(kids(v)[-1])
                init = kids(v)[-1] if kids(v) and v.get("init") else None
                made = None
                if self.u.object_of(raw) and not ("&" in raw or "*" in raw):
                    if re.match(r"(?:const\s+)?std::unique_ptr<", self.u.clean(raw)):
                        if init is not None and any(callee_name(x) == "make_unique" for x in walk(init) if x.get("kind") == "CallExpr"):
                            raise Wait("an owned object of the engine made into a local variable")
                    else:
                        # (an object of the engine as a local: made here - <class>.make -, released where its block ends)
                        if init is None or self.maker_of(init) is None:
                            raise Wait("an object of the engine as a local that is not made by a constructor of it")
                        made = self.u.object_of(raw)
                kept = None
                if init is not None and "auto" in v.get("type", {}).get("qualType", "") and not wide_init:
                    # (auto: the type is the value's - as the Blueprint holds it)
                    kept = self.copied_type(init)
                if self.text_buffer(v) is not None:
                    # (a buffer a text is formatted into and read from as a text: a Text - see text_buffer)
                    name = self.local(v, "Text")
                    said = bare(init) if init is not None else None
                    if said is not None and said.get("kind") == "StringLiteral":
                        out += [f"{name} = {text_literal(c_string(said['value']))};"]
                    elif said is None or (said.get("kind") == "InitListExpr" and not [x for x in kids(said) if x.get("kind") != "ImplicitValueInitExpr"]):
                        out += [f'{name} = "";']
                    else:
                        raise Wait("a text buffer that begins with something else than a text")
                    continue
                name = self.local(v, self.whole64() if wide_init else kept)
                if made is not None:
                    out += self.take_pre() + [f"{name} = {self.ex(init, 'Int')};"]
                    self.scopes[-1].append((name, made))
                    continue
                if init is not None:
                    # (an array written out: its place - declared with its length - is set to what is written)
                    said = self.written_array(init, self.types[v['id']]) if name in self.lengths else None
                    text = self.made_list(said[0], self.types[v['id']]) if said else self.ex(init, self.types[v['id']])
                    out += self.take_pre() + [f"{name} = {text};"]
                    c = self.ctype(v)
                    if ITERATOR.match(c):
                        self.note_home(v["id"], init)
                    elif c.startswith("std::pair<") and ITERATOR.match(template_args(c)[0]):
                        self.note_home(v["id"], init, of_pair=True)
                else:
                    # (the zero of its type; an array of C++ is declared with its length - set to the
                    # empty list, each of its entries is zero)
                    out += [f"{name} = {self.u.zero(self.types[v['id']])};"]
            return out, False
        if k == "ReturnStmt":
            c = kids(s)
            made = [x for scope in self.scopes for x in scope]
            if not made:
                if not c:
                    return [self.ret_line(None)], True
                return [self.ret_line(self.ex(c[0], self.f.value))], True
            # the objects of the engine the function made are released on its way out - after what it
            # returns is computed (C++: the value first, then the destructors)
            mine = {i for scope in self.scopes for i, _ in scope}
            if c and any(x.get("kind") == "DeclRefExpr" and self.names.get(x.get("referencedDecl", {}).get("id")) in mine for x in walk(c[0])):
                raise Wait("a return of an object of the engine the function made")
            text = self.ex(c[0], self.f.value) if c else None
            if text is None or self.f.ctor or self.f.value is None:
                return self.take_pre() + self.releases(made) + [self.ret_line(None)], True
            kept = self.temp("returned", self.f.value)
            return self.take_pre() + [f"{kept} = {text};"] + self.releases(made) + [self.ret_line(kept)], True
        if k == "IfStmt":
            c = kids(s)
            out = []
            while (s.get("hasInit") or s.get("hasVar")) and len(c) > 2 and c[0].get("kind") == "DeclStmt":
                out += self.block([c[0]])   # (what the if declares: declared before it)
                c = c[1:]
            cond = self.bx(c[0])
            out += self.take_pre()
            then = self.block([c[1]])
            out += [f"if ({cond}) {{", *["  " + ln for ln in then]]
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
            cond = self.bx(c[0])
            cond_pre = self.take_pre()
            loop = self.enter_loop(c[-1])
            body = self.loop_body(loop, [c[-1]])
            return self.loop_lines(cond, cond_pre, body, loop), False
        if k == "CXXForRangeStmt":
            return self.for_each(s), False
        if k == "SwitchStmt":
            return self.switch(s)
        if k in ("BreakStmt", "ContinueStmt"):
            if not self.loops or not self.loops[-1]["stop" if k == "BreakStmt" else "skip"]:
                raise Wait("a break outside a loop")
            return [f"{self.loops[-1]['stop' if k == 'BreakStmt' else 'skip']} = true;"], False
        if k == "DoStmt":
            c = kids(s)
            again = self.temp("again", "Bool")
            loop = self.enter_loop(c[0])
            body = self.loop_body(loop, [c[0]])
            cond = self.bx(c[1])
            test = self.take_pre() + [f"{again} = {cond};"]
            if loop["stop"]:   # (after a break the condition is not asked)
                test = [f"if (logic_not({loop['stop']})) {{", *["  " + ln for ln in test], "}"]
            return [f"{again} = true;"] + self.loop_lines(again, [], body + test, loop), False
        if k in ("GotoStmt", "LabelStmt", "CXXTryStmt"):
            raise Wait(k)
        return self.effect(s), False

    def jumps(self, body):
        """(has break, has continue) of the loop this is the body of."""
        found = [False, False]
        def scan(n, in_switch):
            k = n.get("kind")
            if k == "ContinueStmt":
                found[1] = True
            if k == "BreakStmt" and not in_switch:
                found[0] = True
            if k in ("ForStmt", "WhileStmt", "DoStmt", "CXXForRangeStmt", "LambdaExpr"):
                return
            for c in kids(n):
                scan(c, in_switch or k == "SwitchStmt")
        scan(body, False)
        return found[0], found[1]

    def enter_loop(self, body):
        """The flags a loop's body needs: `stop` is set by break, `skip` by continue. What follows either
        in the body runs only while neither is set (see block)."""
        has_break, has_continue = self.jumps(body)
        loop = {"stop": self.temp("stop", "Bool") if has_break else None, "skip": self.temp("skip", "Bool") if has_continue else None}
        self.loops.append(loop)
        return loop

    def loop_body(self, loop, body_stmts, after=None):
        """The lines of a loop's body: the flags, the statements, and `after` (a for loop's step), which
        continue does not skip but break does."""
        lines = ([f"{loop['skip']} = false;"] if loop["skip"] else []) + self.block(body_stmts)
        self.loops.pop()
        if after:
            lines += [f"if (logic_not({loop['stop']})) {{", *["  " + ln for ln in after], "}"] if loop["stop"] else after
        return lines

    def going(self):
        loop = self.loops[-1]
        flags = [x for x in (loop["stop"], loop["skip"]) if x]
        return f"logic_not({flags[0]})" if len(flags) == 1 else f"logic_not(logic_or({flags[0]}, {flags[1]}))"

    def for_loop(self, s):
        init, _, cond, inc, body = (s.get("inner") or [{}] * 5)[:5]
        has_break = self.jumps(body)[0]
        # the counting loop: for (int i = a; i < b; ++i), with i only read in the body
        v = None
        if init.get("kind") == "DeclStmt" and len(kids(init)) == 1 and kids(init)[0].get("kind") == "VarDecl":
            v = kids(init)[0]
        c = bare(cond) if cond else {}
        i = bare(inc) if inc else {}
        counting = (v is not None and NUMBER.get(self.ctype(v)) == "Int" and v.get("init") and c.get("kind") == "BinaryOperator" and c.get("opcode") in ("<", "<=")
                    and self.refers(kids(c)[0], v["id"]) and i.get("kind") == "UnaryOperator" and i.get("opcode") == "++" and self.refers(kids(i)[0], v["id"])
                    and v["id"] not in self.assigned(body) and not any(x in self.assigned(body) for x in self.reads(kids(c)[1])) and not has_break)
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
            lines = self.loop_body(self.enter_loop(body), [body])
            return self.take_pre() + [f"for (auto it{node} = flow_for(n{node}, {first}, {last}); it{node}.next();) {{", *["  " + ln for ln in lines], "}"]
        out = []
        if init:
            out += self.stmt(init)[0]
        out += self.take_pre()
        condition = self.bx(cond) if cond else "true"
        cond_pre = self.take_pre()
        loop = self.enter_loop(body)
        lines = self.loop_body(loop, [body], self.effect_stmt(inc) if inc else None)
        return out + self.loop_lines(condition, cond_pre, lines, loop)

    def effect_stmt(self, n):
        saved, self.pre = self.pre, []
        self.serial += 1
        try:
            lines = self.effect(n)
            return self.pre + lines
        finally:
            self.pre = saved

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

    def char_of(self, text, index, raw):
        """The byte of a text at an index as the C++ type `raw` holds it: a char (signed here, as on every
        machine the game is built for) is -128..127, an unsigned char 0..255 (Code At gives that)."""
        code = f"text_code_at({text}, {index})"
        c = self.u.clean(re.sub(r"\bconst\b|&", "", raw)).strip()
        if c in ("char", "signed char"):
            return f"kern::to<Int>(kern::formula([](Float a) -> Float {{ return a >= 128.0 ? a - 256.0 : a; }}, kern::to<Float>({code})))"
        if c in ("unsigned char", "u8", "uint8_t", "int", "unsigned int", "u32"):
            return code
        raise Wait(f"a character of a text as a {c}")

    def for_each_char(self, s, v, raw, src, body):
        """for (char c : text): a loop over the bytes of a text - each read where it stands (Code At)."""
        if "&" in raw and not re.search(r"\bconst\b", raw):
            raise Wait("a loop that changes the characters of a text")
        got = self.spot(src)
        before = self.take_pre()
        if got is not None and got[1] == "Text":
            text = got[0]
        else:
            text = self.temp("text", "Text")
            before += [f"{text} = {self.ex(src, 'Text')};"] + self.take_pre()
        has_break = self.jumps(body)[0]
        loop = self.enter_loop(body)
        if has_break:
            at, count = self.temp("at", "Int"), self.temp("count", "Int")
            first = [f"{self.local(v, 'Int')} = {self.char_of(text, at, raw)};"]
            lines = first + self.loop_body(loop, [body], [f"{at} += 1;"])
            cond = f'math_compare<"<">(kern::to<Float>({at}), kern::to<Float>({count}))'
            return before + [f"{count} = text_length({text});", f"{at} = 0;"] + self.loop_lines(cond, [], lines, loop)
        node = self.u.next_node
        self.u.next_node += 1
        self.u.members.append(f"  struct : kern::Node<{node}> {{ Int index = 0; }} n{node};")
        first = [f"{self.local(v, 'Int')} = {self.char_of(text, f'n{node}.index', raw)};"]
        lines = first + self.loop_body(loop, [body])
        last = self.whole_math("-", f"text_length({text})", "1")
        return before + [f"for (auto it{node} = flow_for(n{node}, 0, {last}); it{node}.next();) {{", *["  " + ln for ln in lines], "}"]

    def for_each(self, s):
        """for (auto& x : list): a loop over the list where it stands. The entry of a pass is the place
        list[index] - read and changed there, nothing is copied (a loop variable by value is a copy, as in the
        C++). The list is named once, as the C++ evaluates its range once: an index on the way to it is held in
        a variable; a list that is no place (what a function gives) is held in one itself."""
        c = kids(s)
        body = c[-1]
        var = next((x for x in c if x.get("kind") == "DeclStmt" and kids(x) and kids(x)[0].get("kind") in ("VarDecl", "DecompositionDecl") and not kids(x)[0].get("isImplicit")), None)
        rng = next((x for x in c if x.get("kind") == "DeclStmt" and kids(x) and kids(x)[0].get("name", "").startswith("__range")), None)
        if var is None or rng is None:
            raise Wait("a loop over a range of another shape")
        v = kids(var)[0]
        raw = qtype(v)
        src = kids(kids(rng)[0])[-1]
        lt = self.tp(src)
        if lt == "Text":
            return self.for_each_char(s, v, raw, src, body)
        keyed = lt.startswith(("Map<", "Set<"))
        if not lt.startswith("List<") and not keyed:
            raise Wait("a loop over something that is not a list")
        it = lt[5:-1] if not keyed else None
        mark, self.frozen = len(self.pre), []
        try:
            got = self.spot(src)
        finally:
            held, self.frozen = self.frozen, None
        if got is not None and got[1] == lt:
            place = got[0]
        else:
            del self.pre[mark:]
            place = self.temp("list", lt)
            held = [f"{place} = {self.ex(src, lt)};"]
        before = self.take_pre() + held
        has_break = self.jumps(body)[0]
        if has_break:
            # a loop a break ends: it counts itself, and its condition has the break's flag
            at = self.temp("at", "Int")
            count = self.temp("count", "Int")
            index = at
        else:
            node = self.u.next_node
            self.u.next_node += 1
            self.u.members.append(f"  struct : kern::Node<{node}> {{ Int index = 0; }} n{node};")
            index = f"n{node}.index"
        entry = f"{place}[{index}]"
        loop = self.enter_loop(body)
        first = []
        if keyed:
            # for (auto& [key, value] : map), for (auto& e : map), for (K key : set): the entry at its place - its key
            # read there, a map's value the place place[index]
            kind = "map" if lt.startswith("Map<") else "set"
            src_place = {"kind": "_Text", "_text": place, "_type": lt, "_bt": lt, "_place": True}
            if kind == "set":
                key_text = f"set_key_at<{lt[4:-1]}>({place}, {index})"
                if v.get("kind") == "DecompositionDecl":
                    raise Wait("names for the parts of a set's key")
                first = [f"{self.local(v, lt[4:-1])} = {key_text};"]
            else:
                self.keyed_vars[v["id"]] = (src_place, index)
                if v.get("kind") == "DecompositionDecl":
                    self.bind(v, "Entry")
            count_text = f"{kind}_length<{lt[4:-1]}>({place})"
        elif "&" in raw or self.u.clean(raw) == "std::_Bit_reference":   # (std::vector<bool>'s entry by value: still the entry where it stands)
            self.names[v["id"]] = entry
            self.types[v["id"]] = it
        else:
            first = [f"{self.local(v, it)} = {entry};"]
        length = count_text if keyed else f"list_length<{it}>({place})"
        if has_break:
            lines = first + self.loop_body(loop, [body], [f"{index} += 1;"])
            cond = f'math_compare<"<">(kern::to<Float>({index}), kern::to<Float>({count}))'
            return before + [f"{count} = {length};", f"{index} = 0;"] + self.loop_lines(cond, [], lines, loop)
        lines = first + self.loop_body(loop, [body])
        last = self.whole_math("-", length, "1")
        return before + [f"for (auto it{node} = flow_for(n{node}, 0, {last}); it{node}.next();) {{", *["  " + ln for ln in lines], "}"]

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
            inner = self.unwrap(b)
            if inner.get("kind") == "BinaryOperator" and inner.get("opcode") == "=":   # a = b = v: first b, then a is what b is now
                return self.effect(inner) + self.store(a, lambda t: self.ex(kids(inner)[0], t))
            return self.store(a, lambda t: self.ex(b, t))
        if k == "BinaryOperator" and n.get("opcode") == ",":
            a, b = kids(n)
            return self.effect(a) + self.effect(b)
        if k == "ConditionalOperator" and len(kids(n)) == 3:
            # c ? x() : y(); as a statement: a branch - the one C++ evaluates runs
            c, x, y = kids(n)
            cond = self.bx(c)
            then, other = self.effect_stmt(x), self.effect_stmt(y)
            return [f"if ({cond}) {{", *["  " + ln for ln in then], *(["} else {", *["  " + ln for ln in other]] if other else []), "}"]
        if k == "CompoundAssignOperator":
            a, b = kids(n)
            op = n["opcode"][:-1]
            ta = self.tp(a)
            if op == "+" and self.plain_variable(a) and ta in ("Float", "Int"):
                return [f"{self.lvalue(a)} += {self.ex(b, ta)};"]
            synth = {"kind": "BinaryOperator", "opcode": op, "type": n.get("type", {}) if "computeResultType" not in n else n["computeResultType"], "inner": [a, b]}
            lines = self.store(a, lambda t: self.ex(synth, t))
            if synth.get("_w64"):
                n["_w64"] = True
            return lines
        if k == "UnaryOperator" and n.get("opcode") in ("++", "--"):
            a = kids(n)[0]
            ta = self.tp(a)
            if n["opcode"] == "++" and self.plain_variable(a) and ta in ("Float", "Int"):
                return [f"{self.lvalue(a)} += {'1' if ta == 'Int' else '1.0'};"]
            one = {"kind": "IntegerLiteral", "value": "1", "type": {"qualType": "int"}}
            synth = {"kind": "BinaryOperator", "opcode": "+" if n["opcode"] == "++" else "-", "type": a.get("type", {}), "inner": [a, one]}
            lines = self.store(a, lambda t: self.ex(synth, t))
            if synth.get("_w64"):
                n["_w64"] = True
            return lines
        if self.is_local_call(n):
            self.local_call(n)
            return []
        if k == "CXXOperatorCallExpr":
            name = callee_name(n)
            ops = kids(n)[1:]
            if ops and self.is_iter(ops[0]) and name in ("operator++", "operator--", "operator+=", "operator-="):
                by = "1" if name[-1] != "=" else self.ex(ops[1], "Int")
                if name == "operator++" and self.plain_variable(ops[0]):
                    return [f"{self.lvalue(ops[0])} += 1;"]
                return self.store(ops[0], lambda t: self.whole_math("+" if name in ("operator++", "operator+=") else "-", self.ex(ops[0], "Int"), by))
            if ops and self.is_iter(ops[0]) and name == "operator=":
                target = self.unwrap(ops[0])
                if target.get("kind") == "DeclRefExpr":
                    self.note_home(target.get("referencedDecl", {}).get("id"), ops[1])
                return self.store(ops[0], lambda t: self.ex(ops[1], t))
            if self.is_slot(n):
                self.slot(n)   # (m[key]; - the key's row is made)
                return []
            inner = self.unwrap(ops[1]) if name == "operator=" and len(ops) == 2 else {}
            while inner.get("kind") in ("CXXConstructExpr", "MaterializeTemporaryExpr", "CXXBindTemporaryExpr") and len(kids(inner)) == 1:
                inner = self.unwrap(kids(inner)[0])
            if inner.get("kind") == "CXXOperatorCallExpr" and not inner.get("_op") and callee_name(inner) == "operator=" and len(kids(inner)) == 3:
                # a = b = v of a class (a vector, a struct): first b, then a is what b is now
                return self.effect(inner) + self.store(ops[0], lambda t: self.ex(kids(inner)[1], t))
            if name == "operator=" and self.owned_object(ops[0]):
                # an owned object of the engine (std::unique_ptr) gets another: the new one first, then the
                # old one is released (unique_ptr::operator=: reset - the pointer stored, then the old deleted)
                q = self.owned_object(ops[0])
                old = self.temp("old", "Int")
                lines = [f"{old} = {self.ex(ops[0], 'Int')};"]
                text = "0" if self.nothing(ops[1]) else self.ex(ops[1], "Int")
                lines += self.take_pre()
                return lines + self.store(ops[0], lambda t: text) + self.release(q, old)
            if name == "operator=":
                self.moved_into(ops[1], qtype(callee(n)))
                if self.is_box(ops[0]) and self.nothing(ops[1]):
                    return self.store(ops[0], lambda t: t + "{}")
                if self.is_box(ops[0]) and not self.is_box(ops[1]):   # (an optional gets a value: the list of that one)
                    it = self.tp(ops[0])[5:-1]
                    return self.store(ops[0], lambda t: f'list_make<{it}, "1">({self.ex(ops[1], it)})')
                return self.store(ops[0], lambda t: self.ex(ops[1], t))
            if name in ("operator+=", "operator-=", "operator*=", "operator/=") and self.is_vec(ops[0]):
                synth = {"kind": "CXXOperatorCallExpr", "type": ops[0].get("type", {}), "_op": "operator" + name[8], "inner": [kids(n)[0], ops[0], ops[1]]}
                return self.store(ops[0], lambda t: self.ex(synth, t))
            if name == "operator+=" and self.tp(ops[0]) == "Text":
                return self.store(ops[0], lambda t: f'text_append({self.ex(ops[0], "Text")}, {self.ex(ops[1], "Text")}, "")')
        if k == "CallExpr":
            name = callee_name(n)
            ref = callee(n).get("referencedDecl", {})
            if name == "snprintf" and self.text_buffer_of(kids(n)[1] if len(kids(n)) > 1 else {}) is not None:
                return self.formatted(n)
            d = self.engine_decl(ref.get("id"))
            if d is not None:
                return self.engine(d, None, kids(n)[1:])[0]
            if self.own(ref.get("id")):
                self.invoke(self.u.function(ref["id"]), kids(n)[1:])
                return []  # (what it changes is written back by steps before this place; a value nobody reads is not computed)
            if name == "reverse" and len(kids(n)) == 3:
                (l0, first), (l1, last) = self.place(kids(n)[1]), self.place(kids(n)[2])
                if not self.same_list(l0, l1):
                    raise Wait("call: reverse() between two lists")
                lt = self.tp(l0)
                it = lt[5:-1]
                before, lv, after = self.variable_of(l0, lt)
                a, b, held = self.temp("first", "Int"), self.temp("last", "Int"), self.temp("held", it)
                lines = [f"{a} = {first};", f"{b} = {self.whole_math('-', last, '1')};",
                         f'while (loop(math_compare<"<">(kern::to<Float>({a}), kern::to<Float>({b})))) {{',
                         f"  {held} = list_get<{it}>({lv}, {a});", f"  {lv}[{a}] = list_get<{it}>({lv}, {b});", f"  {lv}[{b}] = {held};",
                         f"  {a} += 1;", f"  {b} = {self.whole_math('-', b, '1')};", "}"]
                return before + lines + after
            if name == "swap" and len(kids(n)) == 3:
                a, b = kids(n)[1:]
                t = self.tp(a)
                tmp = self.temp("swapped", t)
                return [f"{tmp} = {self.ex(a, t)};"] + self.store(a, lambda want: self.ex(b, want)) + self.store(b, self.typed(tmp, t))
            if name in ("sort", "stable_sort") and not self.own(ref.get("id")):
                return self.sorted_list(name, kids(n)[1:])
            if name in ("memmove", "memcpy") and len(kids(n)) == 4 and not self.own(ref.get("id")) and self.byte_places(kids(n)[1:3]) is not None:
                return self.moved_bytes(name, kids(n)[1:])
            if name in ("memset", "memcpy") and len(kids(n)) == 4 and not self.own(ref.get("id")):
                return self.raw_memory(name, kids(n)[1:])
            raise Wait(f"call: {name}()")
        if k == "CXXMemberCallExpr":
            m = callee(n)
            base = kids(m)[0] if kids(m) else None
            name = m.get("name")
            if self.on_actor(n) is not None:
                return self.call_on(n, self.on_actor(n))
            d = self.engine_decl(m.get("referencedMemberDecl"))
            if d is not None:
                return self.engine(d, base, kids(n)[1:])[0]
            if self.own(m.get("referencedMemberDecl")):
                if m.get("isArrow") and not self.is_this(base) and self.box(base) is None:
                    raise Wait("a pointer")
                self.invoke(self.u.function(m["referencedMemberDecl"]), kids(n)[1:], obj=base)
                return []
            if (base is not None and not self.is_this(base) and self.owned_object(base) and name == "reset"
                    and all(x.get("kind") == "CXXDefaultArgExpr" or self.nothing(x) for x in kids(n)[1:])):   # (reset(), reset(nullptr))
                q = self.owned_object(base)
                old = self.temp("old", "Int")
                return [f"{old} = {self.ex(base, 'Int')};"] + self.store(base, lambda t: "0") + self.release(q, old)
            if base is not None and not self.is_this(base) and self.is_box(base):
                if name == "reset" and len(kids(n)) == 1:
                    return self.store(base, lambda t: t + "{}")
                raise Wait(f"{name}() of an owned or optional value")
            if base is not None and not self.is_this(base) and self.tp(base).startswith(("Map<", "Set<")):
                kind, _ = self.keyed(base)
                args = kids(n)[1:]
                if name in ("insert", "emplace", "try_emplace", "insert_or_assign", "erase") and not (name == "erase" and len(args) != 1):
                    return self.keyed_change(base, name, args)[0]
                before, lv, after = self.variable_of(base, self.tp(base))
                if name == "clear" and not args:
                    return before + [f"{kind}_clear({lv});"] + after
                if name == "reserve" and len(args) == 1:
                    return before + [f"{kind}_reserve({lv}, {self.ex(args[0], 'Int')});"] + after
                raise Wait(f"{kind}: {name}()")
            if base is not None and not self.is_this(base) and self.tp(base).startswith("List<"):
                it = self.tp(base)[5:-1]
                args = kids(n)[1:]
                if name in ("shrink_to_fit", "rehash", "max_load_factor") or (name == "reserve" and len(args) != 1):
                    return []
                before, lv, after = self.variable_of(base, self.tp(base))
                if name in ("push_back", "emplace_back") and len(args) == 1 and (name == "push_back" or it not in self.u.struct_fields or self.tp(args[0]) == it):
                    return before + [f"list_add({lv}, {self.ex(args[0], it)});"] + after
                if name == "emplace_back" and it in self.u.struct_fields:
                    # (the entry made of the arguments where it is added, as T(args...) makes it)
                    return before + [f"list_add({lv}, {self.make(it, args)});"] + after
                if name == "clear":
                    return before + [f"list_clear({lv});"] + after
                if name == "pop_back":
                    return before + [f"list_remove_at({lv}, kern::to<Int>(kern::formula([](Float n) -> Float {{ return n - 1.0; }}, kern::to<Float>(list_length<{it}>({lv})))));"] + after
                if name == "reserve":   # (room for that many entries: one request for memory, as in C++)
                    return before + [f"list_reserve({lv}, {self.ex(args[0], 'Int')});"] + after
                return before + self.list_step(name, base, lv, it, args) + after
            raise Wait(f"call: {name}() of an object")
        raise Wait(f"a statement: {k}")

    def byte_places(self, pointers):
        """Pointers to entries of lists of bytes (`buf.data() + i` of a std::array<u8, N>, an array of char):
        [(the list, the place)] - None: they are none such."""
        out = []
        for x in pointers:
            try:
                lst, at = self.place(x)
            except Wait:
                return None
            c = self.ctype(lst)
            entry = template_args(c)[0] if c.startswith(("std::array<", "std::vector<")) else re.sub(r"\s*\[\d*\]$", "", c) if c.endswith("]") else None
            if entry is None or self.u.clean(entry) not in ("unsigned char", "signed char", "char") or self.tp(lst) != "List<Int>":
                return None
            out.append((lst, at))
        return out

    def moved_bytes(self, name, a):
        """memmove / memcpy of bytes between places of lists of bytes: entry by entry, as many as the count
        says - from the first on, or from the last on where the bytes move up in the same list (memmove: what
        is read is read before it is written over). Nothing is asked for; C++ copies the same bytes."""
        (dst, at), (src, frm) = self.byte_places(a[:2])
        before, lv, after = self.variable_of(dst, "List<Int>")
        sv = self.place_arg(src, "List<Int>", "const", [])
        count, to, fro = self.temp("count", "Int"), self.temp("to", "Int"), self.temp("from", "Int")
        lines = [f"{count} = {self.ex(a[2], 'Int')};", f"{to} = {at};", f"{fro} = {frm};"]
        last = self.temp("last", "Int")
        lines.append(f"{last} = {self.whole_math('-', count, '1')};")
        up = self.count_loop("0", last, lambda i: [f"{lv}[{self.whole_math('+', to, i)}] = list_get<Int>({sv}, {self.whole_math('+', fro, i)});"])
        if name == "memcpy" or not self.same_list(dst, src):
            return before + lines + up + after
        k = self.temp("k", "Int")
        down = [f"{k} = {last};", f'while (loop(math_compare<">=">(kern::to<Float>({k}), 0.0))) {{',
                f"  {lv}[{self.whole_math('+', to, k)}] = list_get<Int>({sv}, {self.whole_math('+', fro, k)});", f"  {k} = {self.whole_math('-', k, '1')};", "}"]
        return before + lines + [f'if (math_compare<"<=">(kern::to<Float>({to}), kern::to<Float>({fro}))) {{', *["  " + x for x in up], "} else {", *["  " + x for x in down], "}"] + after

    def addressed(self, n):
        """What a pointer handed to memset / memcpy points at, when it is the address of a place: `&n`, an
        array (`r.m`, the address of its first entry). None: anything else - a pointer of its own."""
        b = bare(n)
        while b.get("kind") == "ImplicitCastExpr" and b.get("castKind") in ("BitCast", "NoOp", "ArrayToPointerDecay") and kids(b):
            decayed = b.get("castKind") == "ArrayToPointerDecay"
            b = bare(kids(b)[0])
            if decayed:
                return b
        if b.get("kind") == "UnaryOperator" and b.get("opcode") == "&" and kids(b):
            return bare(kids(b)[0])
        return None

    def raw_memory(self, name, a):
        """memset(&x, 0, sizeof x) / memcpy(&x, &y, sizeof x) on a whole place: its bytes all zero / those of
        a value of its own type - which is setting it to zero / to that value. Zero bytes are the zero of
        numbers and of lists of numbers (of a struct they are not its defaults: that waits)."""
        dst = self.addressed(a[0])
        size = sized(bare(a[2]))
        if dst is None or (name == "memcpy" and self.addressed(a[1]) is None):
            raise Wait("a pointer")
        if size is None or self.u.clean(size) != self.u.clean(qtype(dst)):
            raise Wait(f"raw memory ({name}) of a part of a place")
        t = self.tp(dst)
        if name == "memcpy":
            src = self.addressed(a[1])
            if self.u.clean(qtype(src)) != self.u.clean(qtype(dst)):
                raise Wait("raw memory (memcpy): the bytes of one type read as another")
            return self.store(dst, lambda want: self.ex(src, want))
        try:
            zero_bytes = Const(self.u).number(a[1]) == 0
        except Wait:
            zero_bytes = False
        it = t[5:-1] if t.startswith("List<") else t
        if not zero_bytes or (it not in ("Int", "Float", "Bool") and it != self.whole64()):
            raise Wait("raw memory (memset) that is not the zero of numbers")
        return self.store(dst, lambda want: self.u.zero(want))

    def written_array(self, n, t):
        """An array of C++ written out - `{}`, `{1, 2}` for a T[N] or a std::array<T, N>: (the values it
        says, N). A place of that length set to the list of these values has them, and zeros after them
        - as the array has in C++. None: `n` is no array written out."""
        b = self.unwrap(n)
        while b.get("kind") in ("CXXBindTemporaryExpr", "MaterializeTemporaryExpr", "ExprWithCleanups", "CXXFunctionalCastExpr") and len(kids(b)) == 1:
            b = self.unwrap(kids(b)[0])
        if b.get("kind") != "InitListExpr" or not t.startswith("List<"):
            return None
        fixed = self.u.fixed(qtype(b))
        if not fixed:
            return None
        said = [x for x in kids(b) if x.get("kind") not in ("CXXDefaultArgExpr", "ImplicitValueInitExpr")]
        if len(said) == 1 and bare(said[0]).get("kind") == "InitListExpr" and self.u.clean(qtype(b)).startswith("std::array<") and self.u.entries(qtype(bare(said[0]))) == fixed:
            # (std::array<T, N>{...}: the array it holds, written out)
            said = [x for x in kids(bare(said[0])) if x.get("kind") not in ("CXXDefaultArgExpr", "ImplicitValueInitExpr")]
        return [self.ex(x, t[5:-1]) for x in said], fixed

    @staticmethod
    def made_list(vals, t):
        """The list of these values, as a value: one Make List (of up to 16)."""
        if len(vals) > 16:
            raise Wait("a list made of more than 16 values")
        return f'list_make<{t[5:-1]}, "{len(vals)}">({", ".join(vals)})' if vals else t + "{}"

    # ---- a function a function declares in itself (a lambda) ----
    def lambda_of(self, n):
        b = self.unwrap(n)
        while b.get("kind") in ("CXXConstructExpr", "CXXBindTemporaryExpr", "MaterializeTemporaryExpr", "ExprWithCleanups") and len(kids(b)) == 1:
            b = self.unwrap(kids(b)[0])
        return b if b.get("kind") == "LambdaExpr" else None

    def local_function(self, v, lam):
        """`auto f = [&](...) { ... };` becomes a function of the Blueprint. What it uses of the function
        it stands in are its first inputs; what it changes of that, further results - stored back at
        every call, like what a function writes through a parameter. Captured by value: the values as
        they are here, where it is declared. Returns the steps of the declaration."""
        u, tree = self.u, self.tree
        rec = next((c for c in kids(lam) if c.get("kind") == "CXXRecordDecl"), None)
        method = next((c for c in kids(rec or {}) if c.get("kind") == "CXXMethodDecl" and c.get("name") == "operator()"), None)
        body = next((c for c in kids(method or {}) if c.get("kind") == "CompoundStmt"), None)
        if body is None:
            # (a lambda with auto inputs is a template: a function for each type it is called with - one that takes
            # functions is one for each function it is given; the language has no function values)
            raise Wait("a function value (lambda / std::function) of another shape: a lambda with auto inputs")
        fields = [qtype(c) for c in kids(rec) if c.get("kind") == "FieldDecl"]
        uses_this = any(x.get("kind") == "CXXThisExpr" for x in walk(body))
        values = [t for t in fields if not (uses_this and t.rstrip().endswith("*") and "&" not in t)]
        by_ref = all("&" in t for t in values)
        if not by_ref and any("&" in t for t in values):
            raise Wait("a local function that captures some things by value and others by reference")
        changed = self.assigned(body)
        free, seen = [], set()
        def take(i, node, is_changed):
            if i in seen:
                for entry in free:
                    if entry[0] == i:
                        entry[2] = entry[2] or is_changed
                return
            seen.add(i)
            free.append([i, node, is_changed])
        for x in walk(body):
            k = x.get("kind")
            if k == "DeclRefExpr" and x.get("referencedDecl", {}).get("id") in self.names:
                take(x["referencedDecl"]["id"], x, x["referencedDecl"]["id"] in changed)
            elif k == "CXXOperatorCallExpr" and callee(x).get("referencedDecl", {}).get("id") in u.lambdas:
                other = u.lambdas[callee(x)["referencedDecl"]["id"]]   # (a local function it calls: what that one uses, it gets too)
                if other["snapshots"]:
                    raise Wait("a local function that calls one that captures by value")
                for i, node, is_changed in other["free"]:
                    take(i, node, is_changed)
        if not by_ref and any(c for _, _, c in free):
            raise Wait("a local function that changes its own copy of what it captured (mutable)")
        lines, args, params = [], [], []
        for entry in free:
            if self.types.get(entry[0]) == "Text" and re.fullmatch(r"(?:const\s+)?char\s*\[\d+\]", re.sub(r"\s*&+\s*$", "", qtype(entry[1]))):
                # (a text buffer of the function: the text it holds - snprintf into it in here changes it)
                entry[2] = entry[2] or any(x.get("kind") == "CallExpr" and callee_name(x) == "snprintf" and len(kids(x)) > 1 and self.refers(kids(x)[1], entry[0]) for x in walk(body))
        for i, node, is_changed in free:
            raw = re.sub(r"\s*&+\s*$", "", qtype(node))
            raw = re.sub(r"^const\s+", "", raw)
            if self.types.get(i) == "Text" and re.fullmatch(r"char\s*\[\d+\]", raw):
                raw = "std::string"
            params.append({"id": i, "kind": "ParmVarDecl", "name": re.sub(r"\W+", "_", self.names[i]).strip("_"), "type": {"qualType": raw + " &" if is_changed else "const " + raw + " &"}})
            if by_ref:
                args.append(node)
            else:
                snap = self.temp(re.sub(r"\W+", "_", self.names[i]).strip("_") + "_then", self.types[i])
                lines.append(f"{snap} = {self.ex(node, self.types[i])};")
                args.append({"kind": "_Text", "_text": snap, "_type": self.types[i]})
        outer = self.f.decl
        raw_type = qtype(method)
        trailing = re.match(r"auto\s*(\(.*\)[^()]*?)\s*->\s*(.+)$", raw_type)
        if trailing:   # (a lambda without its own list of inputs, `[&] { ... }`: its type is spelled "auto () const -> void")
            raw_type = f"{trailing.group(2)} {trailing.group(1)}"
        if uses_this:
            raw_type = re.sub(r"\s*\bconst\s*$", "", raw_type) + (" const" if not self.f.mutates else "")
        synth = {"id": method["id"], "_mine": True, "kind": "CXXMethodDecl" if uses_this else "FunctionDecl", "name": f"{outer.get('name', 'f')}_{v.get('name', 'local')}",
                 "type": {"qualType": raw_type}, "_file": lam.get("_file", ""), "_at": lam.get("_at", (None, None)),
                 "inner": params + [c for c in kids(method) if c.get("kind") == "ParmVarDecl"] + [body]}
        tree.body[method["id"]] = synth
        tree.scope_of[method["id"]] = tree.scope_of.get(outer["id"], "")
        if uses_this:
            tree.owner[method["id"]] = tree.owner.get(outer["id"])
        u.lambdas[method["id"]] = {"free": free, "args": args, "snapshots": not by_ref}
        g = u.function(method["id"])
        g.lifted = True
        return lines

    def local_call(self, n):
        """A call of a local function: (text, type) of what it gives, as `invoke` has it."""
        info = self.u.lambdas[callee(n)["referencedDecl"]["id"]]
        return self.invoke(self.u.function(callee(n)["referencedDecl"]["id"]), info["args"] + kids(n)[2:])

    def is_local_call(self, n):
        return n.get("kind") == "CXXOperatorCallExpr" and callee_name(n) == "operator()" and callee(n).get("referencedDecl", {}).get("id") in self.u.lambdas

    # ---- a function of the Blueprint of an actor of the level: Call on Actor ----
    def on_actor(self, n):
        """Is this C++'s Call on Actor - a call of kern::logic::ActorBlueprint::call (the Blueprint of an actor
        of the level, as the program's C++ calls it)? Then the object it is called on, else None."""
        m = callee(n)
        if n.get("kind") != "CXXMemberCallExpr" or m.get("kind") != "MemberExpr" or m.get("name") != "call" or not kids(m):
            return None
        base = kids(m)[0]
        return base if re.sub(r"^const\s+", "", self.ctype(base)).strip() == ENGINE + "::logic::ActorBlueprint" else None

    def actor_get(self, n):
        """rules.get<f64>("Speed") - kern::logic::ActorBlueprint::get: a variable of the Blueprint of an actor of
        the level, read (Get Variable on Actor). The object it is read of, else None."""
        m = callee(n)
        if n.get("kind") != "CXXMemberCallExpr" or m.get("kind") != "MemberExpr" or m.get("name") != "get" or not kids(m):
            return None
        base = kids(m)[0]
        return base if re.sub(r"^const\s+", "", self.ctype(base)).strip() == ENGINE + "::logic::ActorBlueprint" else None

    def get_on(self, n, obj):
        """Get Variable on Actor: func_get_on<"Speed", Float>(actor_find("Guided")) - the variable as the program
        binds the actor's Blueprint, of the type the C++ reads it as."""
        args = kids(n)[1:]
        named = self.unwrap(args[0]) if len(args) == 1 else {}
        if named.get("kind") != "StringLiteral":
            raise Wait("a variable of an actor's Blueprint that is not named in so many words")
        name, actor = c_string(named["value"]).decode("utf-8"), self.actor_named(obj)
        target = self.u.world.actors.get(actor)
        if target is None:
            raise Wait(f"a variable of the Blueprint of the actor {actor}, which the level does not have")
        if name not in target.get("variables", {}):
            raise Wait(f"the variable {name} of the Blueprint of the actor {actor}, which has no such variable")
        t = self.tp(n)
        if t not in ("Float", "Int", "Bool", "Vector", "Text") and t not in self.u.enum_values:
            raise Wait(f"the variable {name} of the Blueprint of the actor {actor} as a {t}")
        self.f.on_actors.append((actor, name))
        return f'func_get_on<"{name}", {t}>(actor_find({text_literal(actor)}))', t

    def actor_named(self, obj):
        """The actor whose Blueprint `obj` is: named where it is made (ActorBlueprint rules{"Lamps"}). The name
        is a constant of the object (`const char* const actor`) - the same for as long as it lives."""
        b = self.unwrap(obj)
        d = self.tree.decl.get(b.get("referencedDecl", {}).get("id")) if b.get("kind") == "DeclRefExpr" else None
        if d is None or d.get("kind") != "VarDecl" or qtype(d).rstrip().endswith("&"):
            raise Wait("a call on an actor's Blueprint that is not one the program makes and names here (its actor)")
        made = bare(kids(d)[-1]) if kids(d) else {}
        while made.get("kind") in ("ExprWithCleanups", "CXXFunctionalCastExpr", "MaterializeTemporaryExpr") and len(kids(made)) == 1:
            made = bare(kids(made)[0])
        said = [self.unwrap(x) for x in kids(made) if x.get("kind") != "CXXDefaultArgExpr"] if made.get("kind") == "CXXConstructExpr" else []
        if len(said) != 1 or said[0].get("kind") != "StringLiteral":
            raise Wait("a call on an actor's Blueprint whose actor is not named in so many words where it is made")
        return c_string(said[0]["value"]).decode("utf-8")

    def actor_value(self, n):
        """rules.number("Speed", x), rules.flag("Seen", y) - kern::logic::ActorBlueprint's Call on Actor with one
        output, as a value: the object it is called on, else None."""
        m = callee(n)
        if n.get("kind") != "CXXMemberCallExpr" or m.get("kind") != "MemberExpr" or m.get("name") not in ("number", "flag") or not kids(m):
            return None
        base = kids(m)[0]
        return base if re.sub(r"^const\s+", "", self.ctype(base)).strip() == ENGINE + "::logic::ActorBlueprint" else None

    def crosses(self, t, their, of, target, depth=0):
        """Does a value of the Blueprint type `t` (of this Blueprint) cross as a value of the pin type
        `their` (`of`: its struct or enum) of the actor's Blueprint `target`, unchanged? Call on Actor
        hands a value over as it is: a flag, a whole number, a decimal number, a vector, a text, a
        value of an enum with as many values, a struct whose fields cross one by one in their order,
        a list of any of these. None where it does, else what it is instead."""
        u = self.u
        simple = {"Bool": "bool", "Int": "int", "Float": "float", "Vector": "vector", "Text": "text"}
        if depth > 16:
            raise Wait("a struct of an actor's Blueprint that holds itself")
        if t.startswith("List<") and t.endswith(">"):
            if not their.endswith("_list"):
                return f"a {their}, and the call gives a {t}"
            return self.crosses(t[5:-1], their[:-5], of, target, depth + 1)
        if t in simple:   # (a value on its own is converted as on a wire - in every stage the same; a field of a struct and an item of a list are not)
            return None if simple[t] == their or depth == 0 else f"a {their}, and the call gives a {t}"
        if t in u.enum_values:
            if u.enum_numbers.get(t):
                return f"a {their}, and the call gives {t}, an enum whose values are numbers of their own"
            if depth == 0 and their != "enum":
                return None
            if their != "enum" or target.get("enums", {}).get(of) != len(u.enum_values[t]):
                return f"a {their} {of}, and the call gives the enum {t} of {len(u.enum_values[t])} values"
            return None
        if t in u.struct_fields:
            fields = target.get("structs", {}).get(of)
            if their != "struct" or fields is None:
                return f"a {their} {of}, and the call gives the struct {t}"
            mine = u.struct_fields[t]
            if len(mine) != len(fields):
                return f"the struct {of} of {len(fields)} fields, and the call gives the struct {t} of {len(mine)}"
            for (name, ft, _, _), (fthem, fof) in zip(mine, fields):
                why = self.crosses(ft, fthem, fof, target, depth + 1)
                if why:
                    return f"the struct {of}, and the field {name} of the struct {t} the call gives does not cross: {why}"
            return None
        return f"a {their}, and the call gives a {t} (a call on an actor's Blueprint gives and takes flags, whole and decimal numbers, vectors, texts, enum values, structs of them and lists)"

    def call_on(self, n, obj, gives_type=None):
        """rules.call("Fn", std::tie(a, b), x, y): the Blueprint of the actor `rules` names runs its function Fn -
        Call on Actor, on the actor found by its name, as the program binds it. The Blueprint says what Fn
        looks like (KERN_EXTERN): its inputs and outputs as the actor's Blueprint names them, of the types of
        the values the call gives and takes, and the zero of each type as its default - what the call gives
        where the actor's Blueprint has no such function, in C++ as in the Blueprint. What the call gives is
        stored in the places std::tie names, one after the other."""
        u = self.u
        args = kids(n)[1:]
        if len(args) < (1 if gives_type else 2):
            raise Wait("a call on an actor's Blueprint without its function and its outputs")
        named = self.unwrap(args[0])
        if named.get("kind") != "StringLiteral":
            raise Wait("a call on an actor's Blueprint whose function is not named in so many words")
        if gives_type:   # (one output, the value of the call: number(), flag())
            places, values = [None], args[1:]
        else:
            outs = self.unwrap(args[1])
            while outs.get("kind") in ("MaterializeTemporaryExpr", "CXXBindTemporaryExpr", "CXXConstructExpr", "CXXFunctionalCastExpr") and len(kids(outs)) == 1:
                outs = self.unwrap(kids(outs)[0])
            if outs.get("kind") != "CallExpr" or callee_name(outs) != "tie" or not self.ctype(outs).startswith("std::tuple<"):
                raise Wait("a call on an actor's Blueprint whose outputs are not the places std::tie names")
            places, values = kids(outs)[1:], args[2:]
        function, actor = c_string(named["value"]).decode("utf-8"), self.actor_named(obj)
        target = u.world.actors.get(actor)
        if target is None:
            raise Wait(f"a call on the Blueprint of the actor {actor}, which the level does not have")
        if target["library"]:
            raise Wait(f"a call on the Blueprint of the actor {actor}, which is a library: its functions are called directly ({actor}::{function})")
        if function not in target["functions"]:
            raise Wait(f"a call of {function} on the Blueprint of the actor {actor}, which has no such function")
        ins, gives = target["functions"][function]
        if len(ins) != len(values) or len(gives) != len(places):
            raise Wait(f"a call of {function} on the Blueprint of the actor {actor} with {len(values)} values and {len(places)} outputs, where the function takes {len(ins)} and gives {len(gives)}")
        if is_word(function) or any(not re.fullmatch(r"[A-Za-z_]\w*", x) or x in CPP_WORDS for x in [function, *ins, *gives]):
            raise Wait(f"a call of {function} on the Blueprint of the actor {actor}, which names it or a pin of it with a word of C++ or of the Blueprint language")
        in_types, out_types = [self.tp(a) for a in values], [gives_type if p is None else self.tp(p) for p in places]
        theirs = target.get("pins", {}).get(function, ([], []))
        for t, (their, of), what in [*zip(in_types, theirs[0], ins), *zip(out_types, theirs[1], gives)]:
            why = self.crosses(t, their, of, target)
            if why:
                raise Wait(f"a call of {function} on the Blueprint of the actor {actor}: its pin {what} is {why}")
        pins = (list(zip(ins, in_types)), list(zip(gives, out_types)))
        held = u.externs.get(function)
        if held is None:
            if function in u.names or function + "Result" in u.names:
                raise Wait(f"a call of {function} on the Blueprint of the actor {actor}: the Blueprint has something of that name")
            lines = []
            if gives:
                lines.append(f"  struct {function}Result {{ " + " ".join(f"{t} {o} = {u.zero(t)};" for o, t in pins[1]) + " };")
            lines.append(f"  /// (of the Blueprint of the actor {actor}, called on it)")
            lines.append(f"  KERN_EXTERN {function + 'Result' if gives else 'void'} {function}(" + ", ".join(f"{t} {x} = {u.zero(t)}" for x, t in pins[0]) + ");")
            u.names.update({function, function + "Result"})
            u.externs[function] = (*pins, lines)
        elif held[:2] != pins:
            raise Wait(f"two calls of {function} on Blueprints of actors with other values (a Blueprint says once what a function of another one looks like)")
        texts = [self.ex(a, t) for a, t in zip(values, in_types)]
        self.f.pure = False   # (a step: what the other Blueprint does is not known here)
        self.f.on_actors.append((actor, function))
        node = u.next_node
        u.next_node += 1
        if gives:
            u.members.append("  struct { " + " ".join(f"{t} {o} = {u.zero(t)};" for o, t in pins[1]) + f" }} n{node};")
        lines = [f'func_call_on<"{function}">(actor_find({text_literal(actor)}), kern::out({", ".join(f"n{node}.{o}" for o in gives)}){"".join(", " + x for x in texts)});']
        if gives_type:
            self.hoist(lines)
            return f"n{node}.{gives[0]}", gives_type
        for p, (o, t) in zip(places, pins[1]):
            lines += self.store(p, self.typed(f"n{node}.{o}", t))
        return lines

    # ---- the engine: what only it can do is a node ----
    def engine_decl(self, decl_id):
        """The declaration of a function only the engine can do (a header declares it, the engine defines it), or None."""
        d = self.tree.decl.get(decl_id) if ENGINE_NODES else None
        return d if d is not None and self.u.api().declared_only(d) else None

    def as_list(self, a, bt):
        """Data handed to the engine by a pointer or as a view (Span): the list of it."""
        it = bt[5:-1]
        b = self.unwrap(a)
        while b.get("kind") in ("CXXConstructExpr", "CXXFunctionalCastExpr", "CXXBindTemporaryExpr", "MaterializeTemporaryExpr", "CXXMemberCallExpr") and self.tp_or_none(b) != bt:
            if b.get("kind") == "CXXMemberCallExpr":
                if callee(b).get("name") != "data" or not kids(callee(b)):
                    break
                b = self.unwrap(kids(callee(b))[0])
            elif len(kids(b)) == 1:
                b = self.unwrap(kids(b)[0])
            elif not kids(b):
                return bt + "{}"
            else:
                break
        if self.nothing(b):
            return bt + "{}"
        if b.get("kind") == "UnaryOperator" and b.get("opcode") == "&":
            return f'list_make<{it}, "1">({self.ex(kids(b)[0], it)})'
        raw = bare(a)
        while raw.get("kind") == "ImplicitCastExpr" and raw.get("castKind") != "ArrayToPointerDecay" and kids(raw):
            raw = bare(kids(raw)[0])
        if raw.get("kind") == "ImplicitCastExpr" and raw.get("castKind") == "ArrayToPointerDecay" and self.tp_or_none(kids(raw)[0]) == bt:
            return self.ex(kids(raw)[0], bt)
        if self.tp_or_none(b) == bt:
            return self.ex(b, bt)
        raise Wait("data handed to the engine in another shape")

    def engine(self, d, obj, args):
        """A call of a function of the engine: its node. Every call is a step of its own before the statement
        it stands in - it runs once, where C++ calls it. Gives (the steps, the text and the type of what the
        call returns - or None, None)."""
        u = self.u
        try:
            f = u.api().function(d)
        except Wait as w:
            raise Wait(f"the engine's {d.get('name')}() (it has no node: {w})")
        known = u.engine_table().get(f["type"])
        mine = (f["value"], [(n, *u.api().pin(bt)) for n, bt, _ in f["ins"]], [(n, *u.api().pin(bt)) for n, bt, _ in f["outs"]])
        if known != mine:
            raise Wait(f"the engine's {d.get('name')}() (its node is another one in the engine: {'none' if known is None else 'its pins differ'})")
        u.engine_calls[d["id"]] = f
        ins, texts, own = list(f["ins"]), [], None
        if d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static":
            # (of an object of the engine: its handle; of plain data the engine defines the method of - MeshData::as_input
            # -: the value, the node's first input, and what a method that is not const makes of it its output `self`)
            _, bt, handle = ins.pop(0)
            if obj is None or self.is_this(obj):
                raise Wait("a call of the engine on its own object")
            texts.append(self.ex(obj, bt))
            own = obj
        params = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
        nodes = {}   # pin -> the argument it is (an output: where the call wants it - None: nowhere)
        in_pins = {name: bt for name, bt, _ in ins}
        for i, (pin_in, pin_out) in enumerate(f["params"]):
            a = args[i] if i < len(args) else None
            if a is None or a.get("kind") == "CXXDefaultArgExpr":   # (left out: the default the header gives it)
                a = kids(params[i])[-1] if kids(params[i]) else None
                if a is None:
                    raise Wait("a call of the engine with a default argument that is declared elsewhere")
            if pin_in is None:
                # what the function reports if it is asked to (`bool* changed`): into the place the call
                # names (&changed), or nowhere (nullptr) - the node gives it either way
                b = self.unwrap(a)
                if self.nothing(b):
                    nodes[pin_out] = None
                elif b.get("kind") == "UnaryOperator" and b.get("opcode") == "&" and kids(b):
                    nodes[pin_out] = kids(b)[0]
                else:
                    raise Wait("a pointer")
                continue
            name, bt = pin_in, in_pins[pin_in]
            nodes[name] = a
            raw = qtype(params[i]).strip()
            made = self.unwrap(a)
            while made.get("kind") in ("MaterializeTemporaryExpr", "CXXBindTemporaryExpr", "CXXFunctionalCastExpr") and len(kids(made)) == 1:
                made = self.unwrap(kids(made)[0])
            if bt in u.struct_fields and made.get("kind") in ("CXXConstructExpr", "InitListExpr", "CXXTemporaryObjectExpr") and not [x for x in kids(made) if x.get("kind") != "CXXDefaultArgExpr"]:
                texts.append(u.zero(bt))   # ({} - the struct with its defaults)
            elif bt.startswith("List<") and (re.search(r"\*\s*(const)?$", raw) or "Span<" in raw):
                texts.append(self.as_list(a, bt))
            else:
                texts.append(self.ex(a, bt))
        fn = f["type"].replace(".", "_")
        outs = f["outs"]
        if f["value"]:
            tmp = self.temp(d["name"], outs[0][1])
            return [f"{tmp} = {fn}({', '.join(texts)});"], tmp, outs[0][1]
        self.f.pure = False   # (a step of the engine: the function changes something outside itself)
        if not outs:
            return [f"{fn}({', '.join(texts)});"], None, None
        node = u.next_node
        u.next_node += 1
        u.members.append("  struct { " + " ".join(f"{bt} {o} = {u.zero(bt)};" for o, bt, _ in outs) + f" }} n{node};")
        lines, got = [f"{fn}({', '.join([f'n{node}'] + texts)});"], (None, None)
        for o, bt, _ in outs:
            if o == "result":
                tmp = self.temp(d["name"], bt)
                lines.append(f"{tmp} = n{node}.result;")
                got = (tmp, bt)
            elif o == "self":
                lines += self.store(own, self.typed(f"n{node}.self", bt))
            elif nodes.get(o) is not None:
                lines += self.store(nodes[o], self.typed(f"n{node}.{o}", bt))
        return lines, *got

    def engine_value(self, d, obj, args):
        lines, text, t = self.engine(d, obj, args)
        if text is None:
            raise Wait("a call without a result as a value")
        self.hoist(lines)
        return text, t

    # ---- maps and sets (the language's Map, Set); iterators: places ----
    def keyed(self, n):
        """Is this a map or a set? ("map" or "set", False) - else None."""
        t = self.tp_or_none(n) or ""
        return ("map" if t.startswith("Map<") else "set", False) if t.startswith(("Map<", "Set<")) else None

    def is_iter(self, n):
        return ITERATOR.match(self.ctype(n)) is not None

    def rows(self, n):
        """(the map's or set's type, its type's settings - "K, V" or "K" -, a key's type, a value's - None in a set)."""
        lt = self.tp(n)
        args = template_args(lt)
        return lt, ", ".join(args), args[0], (args[1] if lt.startswith("Map<") else None)

    def found(self, kind, row, lst, key):
        """Is there an entry of this key?"""
        return f"{kind}_contains<{row}>({lst}, {key})"

    def found_at(self, kind, row, lst, key):
        """The place of a key's entry, -1 when there is none."""
        return f"map_find<{row}>({lst}, {key}).index" if kind == "map" else f"set_find<{row}>({lst}, {key})"

    def keyed_node(self):
        """A node that puts a key in and gives (was it new, its place) - Add Key, Set at Key."""
        node = self.u.next_node
        self.u.next_node += 1
        self.u.members.append(f"  struct {{ Bool added = false; Int index = 0; }} n{node};")
        return f"n{node}"

    def put_row(self, base, key, value=None, assign=False):
        """Steps that put a key into a map or set unless it is there (insert, emplace, m[key]): (the steps,
        the place of the key's entry, was it new). `key` and `value`: variables of the key's and of the
        values' type (no value: its zero). `assign`: an entry that is there gets the value (insert_or_assign)."""
        kind, _ = self.keyed(base)
        lt, _, kt, vt = self.rows(base)
        before, lv, after = self.variable_of(base, lt)
        n = self.keyed_node()
        if kind == "set":
            return before + [f"set_add({n}, {lv}, {key});"] + after, f"{n}.index", f"{n}.added"
        if assign:
            return before + [f"map_set({n}, {lv}, {key}, {value});"] + after, f"{n}.index", f"{n}.added"
        lines = [f"map_add({n}, {lv}, {key});"]
        if value is not None:   # (what the new entry holds: its value; one that is there keeps its own)
            lines += [f"if ({n}.added) {{", f"  {lv}[{n}.index] = {value};", "}"]
        return before + lines + after, f"{n}.index", f"{n}.added"

    def slot(self, n):
        """m[key]: the place of the key's row. Steps before the statement find it - and add the row when the
        key is new, as C++ does."""
        cached = n.get("_slot")
        if cached is not None and cached[0] == self.serial:
            return cached[1]
        base, key = kids(n)[1], kids(n)[2]
        kt = self.rows(base)[2]
        held = self.temp("key", kt)
        self.hoist([f"{held} = {self.ex(key, kt)};"])
        lines, at, _ = self.put_row(base, held)
        self.hoist(lines)
        n["_slot"] = (self.serial, at)
        return at

    def is_slot(self, n):
        return n.get("kind") == "CXXOperatorCallExpr" and not n.get("_op") and callee_name(n) == "operator[]" and len(kids(n)) == 3 and (self.keyed(kids(n)[1]) or ("", 0))[0] == "map"

    def entry(self, lst, index, row):
        """The entry of a list at a place, as an expression of its own (the place: an expression or a text)."""
        at = index if isinstance(index, dict) else {"kind": "_Text", "_text": index, "_type": "Int"}
        return {"kind": "ArraySubscriptExpr", "_bt": row, "type": {"qualType": "int"}, "inner": [lst, at]}

    def held_by(self, n):
        """m[key] as the place it is: the value of the key's entry."""
        base = kids(n)[1]
        _, _, _, vt = self.rows(base)
        return self.entry(base, self.slot(n), vt)

    def keyed_entry(self, n):
        """An entry of a map or a set as C++ names it - `*it`, `it->`, the variable of a loop over one, a pair of
        its key and value -: (the map or set, its place - a node or a text), else None."""
        b = self.unwrap(n)
        if "_keyed" in b:
            return b["_keyed"]
        if b.get("kind") == "DeclRefExpr" and b.get("referencedDecl", {}).get("id") in self.keyed_vars:
            return self.keyed_vars[b["referencedDecl"]["id"]]
        return None

    def keyed_key(self, e):
        """The key of an entry (keyed_entry), as a value: (text, type)."""
        base, at = e
        kind, _ = self.keyed(base)
        lt, row, kt, _ = self.rows(base)
        return f"{kind}_key_at<{row}>({self.ex(base, lt)}, {at if isinstance(at, str) else self.ex(at, 'Int')})", kt

    def iter_node(self, n):
        """An iterator's expression without what only converts it (an iterator made a const_iterator)."""
        b = self.unwrap(n)
        while b.get("kind") in ("CXXConstructExpr", "CXXFunctionalCastExpr", "CXXStaticCastExpr") and len(kids(b)) == 1 and self.is_iter(kids(b)[0]):
            b = self.unwrap(kids(b)[0])
        return b

    def entry_of(self, n):
        """`*it`, `it->`: the entry an iterator into a list, map or set stands at, as an expression of its own."""
        b = self.unwrap(n)
        if b.get("kind") == "CXXOperatorCallExpr" and not b.get("_op") and callee_name(b) in ("operator->", "operator*") and len(kids(b)) == 2 and self.is_iter(kids(b)[1]):
            home = self.home(kids(b)[1])
            if self.keyed(home):   # (an entry of a map or a set: its key and value - keyed_entry)
                return {"kind": "_Entry", "_keyed": (home, kids(b)[1]), "type": {"qualType": "int"}, "_bt": "Entry"}
            return self.entry(home, kids(b)[1], self.tp(home)[5:-1])
        return None

    def home(self, n):
        """The list an iterator goes through."""
        b = self.iter_node(n)
        k = b.get("kind")
        if k == "DeclRefExpr":
            h = self.homes.get(b.get("referencedDecl", {}).get("id"))
            if not h:
                raise Wait("an iterator whose list is not known where it is used")
            return h
        if k == "CXXMemberCallExpr":
            m = callee(b)
            base = kids(m)[0] if kids(m) else None
            if base is not None and not self.is_this(base) and m.get("name") in ("begin", "cbegin", "end", "cend", "find", "erase", "insert", "lower_bound", "upper_bound") and (self.tp_or_none(base) or "").startswith(("List<", "Map<", "Set<")):
                return base
        if k == "MemberExpr" and b.get("name") == "first" and kids(b):   # (what insert() and emplace() give: the place and whether the key was new)
            x = self.unwrap(kids(b)[0])
            if x.get("kind") == "DeclRefExpr" and self.homes.get(x.get("referencedDecl", {}).get("id")):
                return self.homes[x["referencedDecl"]["id"]]
            if x.get("kind") == "CXXMemberCallExpr" and kids(callee(x)) and callee(x).get("name") in ("insert", "emplace", "try_emplace", "insert_or_assign"):
                return kids(callee(x))[0]
        if k == "CXXOperatorCallExpr" and callee_name(b) in ("operator+", "operator-", "operator++", "operator--", "operator+=", "operator-=") and len(kids(b)) >= 2:
            return self.home(kids(b)[1])
        if k == "CallExpr" and callee_name(b) in ("find", "find_if", "find_if_not", "next", "prev", "lower_bound", "upper_bound", "max_element", "min_element") and len(kids(b)) >= 2:
            return self.home(kids(b)[1])
        raise Wait("an iterator whose list is not known where it is used")

    def steady(self, n):
        """A list an iterator is kept into is named the same way as long as the iterator lives: a variable or
        a field, not an entry at a place that is computed."""
        b = self.unwrap(n)
        while True:
            if b.get("kind") == "DeclRefExpr" or self.is_this(b):
                return n
            if b.get("kind") == "MemberExpr" and not kids(b):
                return n
            if b.get("kind") == "MemberExpr" and (self.is_this(kids(b)[0]) or not b.get("isArrow")):
                b = self.unwrap(kids(b)[0])
                continue
            raise Wait("an iterator kept into a list at a place that is computed")

    def note_home(self, decl_id, source, of_pair=False):
        """A variable is given an iterator (or what insert() gave): the list it goes through from here on."""
        try:
            if of_pair:
                x = self.unwrap(source)
                while x.get("kind") in ("CXXConstructExpr", "CXXBindTemporaryExpr") and len(kids(x)) == 1:
                    x = self.unwrap(kids(x)[0])
                if x.get("kind") == "CXXMemberCallExpr" and kids(callee(x)) and callee(x).get("name") in ("insert", "emplace", "try_emplace", "insert_or_assign"):
                    h = self.steady(kids(callee(x))[0])
                else:
                    return
            else:
                h = self.steady(self.home(source))
        except Wait:
            return
        old = self.homes.get(decl_id)
        if old is None:
            self.homes[decl_id] = h
        elif old and not self.same_list(old, h):
            self.homes[decl_id] = False   # (now this list, now that one: not known where it is used)

    def two_of(self, n):
        """The two parts of a pair written out where it is used ({k, v}, std::make_pair(k, v), std::pair<K, V>(k, v)), or None."""
        b = self.unwrap(n)
        while True:
            k = b.get("kind")
            ks = [x for x in kids(b) if x.get("kind") != "CXXDefaultArgExpr"]
            if k == "CallExpr" and callee_name(b) == "make_pair" and len(kids(b)) == 3:
                return kids(b)[1:]
            if k in ("CXXConstructExpr", "CXXTemporaryObjectExpr", "InitListExpr", "CXXFunctionalCastExpr") and self.ctype(b).startswith("std::pair<"):
                if len(ks) == 2 and k != "CXXFunctionalCastExpr":
                    return ks
                if len(ks) == 1:
                    b = self.unwrap(ks[0])
                    continue
            return None

    def keyed_change(self, base, name, a):
        """A call that changes a map or set, as steps: (the steps, the place of the key's entry, a flag - was
        the key new; of erase: was it there)."""
        kind, _ = self.keyed(base)
        lt, row, kt, vt = self.rows(base)
        if name == "erase" and len(a) == 1:
            if self.is_iter(a[0]):   # (erase(it): the entry at that place goes - the next one is there then)
                at = self.temp("at", "Int")
                place = self.ex(a[0], "Int")
                before, lv, after = self.variable_of(base, lt)
                return before + [f"{at} = {place};", f"{kind}_remove_at({lv}, {at});"] + after, at, "true"
            key = self.temp("key", kt)
            lines = [f"{key} = {self.ex(a[0], kt)};"]
            before, lv, after = self.variable_of(base, lt)
            node = self.u.next_node
            self.u.next_node += 1
            self.u.members.append(f"  struct {{ Bool removed = false; }} n{node};")
            return lines + before + [f"{kind}_remove(n{node}, {lv}, {key});"] + after, "-1", f"n{node}.removed"
        if name in ("insert", "emplace", "try_emplace", "insert_or_assign") and a:
            key_node, value = a[0], None
            if kind == "set":
                if len(a) != 1 or self.is_iter(a[0]) or bare(self.unwrap(a[0])).get("kind") in ("CXXStdInitializerListExpr", "InitListExpr") and self.tp_or_none(a[0]) != kt:
                    raise Wait(f"{name}() of a set with other arguments")
            else:
                rest = a[1:]
                if name in ("insert", "emplace") and len(a) == 1:
                    two = self.two_of(a[0])
                    if two is None:
                        raise Wait(f"{name}() of a map with a pair that is made elsewhere")
                    key_node, rest = two[0], [two[1]]
                elif name == "insert" or (name == "emplace" and len(a) != 2):
                    raise Wait(f"{name}() of a map with other arguments")
                if not rest:
                    value = None
                elif len(rest) == 1 and (vt not in self.u.struct_fields or self.tp_or_none(rest[0]) == vt):
                    value = lambda: self.ex(rest[0], vt)
                elif vt in self.u.struct_fields:
                    value = lambda: self.make(vt, rest)
                else:
                    raise Wait(f"{name}() of a map with other arguments")
            key = self.temp("key", kt)
            lines = [f"{key} = {self.ex(key_node, kt)};"]
            held = None
            if kind == "map" and value is not None:
                held = self.temp("value", vt)
                lines.append(f"{held} = {value()};")
            more, at, fresh = self.put_row(base, key, held, assign=name == "insert_or_assign")
            return lines + more, at, fresh
        raise Wait(f"{name}() of a map or set")

    def keyed_call(self, b, base, name, a):
        """A call of a map or set, as a value."""
        kind, _ = self.keyed(base)
        lt, row, kt, vt = self.rows(base)
        lst = self.ex(base, lt)
        if name == "size" and not a:
            return f"{kind}_length<{row}>({lst})", "Int"
        if name == "empty" and not a:
            return f'math_compare<"==">(kern::to<Float>({kind}_length<{row}>({lst})), 0.0)', "Bool"
        if name in ("begin", "cbegin", "end", "cend") and not a and self.is_iter(b):
            # an iterator is the place it stands at: the first entry's is 0, the end the length
            return ("0" if "begin" in name else f"{kind}_length<{row}>({lst})"), "Int"
        if name in ("find", "count", "contains", "at") and len(a) == 1:
            key = self.ex(a[0], kt)
            if name == "contains":
                return self.found(kind, row, lst, key), "Bool"
            if name == "count":
                return f"kern::to<Int>({self.found(kind, row, lst, key)})", "Int"
            if name == "at":
                if kind != "map":
                    raise Wait("at() of a set")
                return f"map_find<{row}>({lst}, {key}).value", vt
            return (f"kern::to<Int>(kern::formula([](Float at, Float count) -> Float {{ return at < 0.0 ? count : at; }}, "
                    f"kern::to<Float>({self.found_at(kind, row, lst, key)}), kern::to<Float>({kind}_length<{row}>({lst}))))"), "Int"
        if name in ("insert", "emplace", "try_emplace", "insert_or_assign", "erase") and not (name == "erase" and len(a) != 1):
            lines, at, flag = self.keyed_change(base, name, a)
            self.hoist(lines)
            if name == "erase":
                return (at, "Int") if self.is_iter(a[0]) else (f"kern::to<Int>({flag})", "Int")
            pt = self.u.btype(qtype(b))
            if pt not in self.u.struct_fields:
                raise Wait(f"what {name}() of a map or set gives")
            return f"struct_make<{pt}>({at}, {flag})", pt
        if name in ("lower_bound", "upper_bound", "equal_range", "extract", "merge"):
            raise Wait(f"{name}() of a map or set")
        return None

    def bind(self, d, bt):
        """`auto [a, b] = ...;` - a and b are names for the parts of the value: where they are used, the part
        is written out (the first field of the value, the second ...)."""
        fields = self.u.struct_fields.get(bt)
        binds = [c for c in kids(d) if c.get("kind") == "BindingDecl"]
        if bt == "Entry":
            # `auto& [key, value]` of an entry of a map (keyed_vars): its key, and its value where it stands
            if len(binds) != 2:
                raise Wait("names for the parts of an entry of a map")
            names = {binds[0]["id"]: "first", binds[1]["id"]: "second"}
            for n in walk(self.f.decl):
                if n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") in names:
                    whole = {"kind": "DeclRefExpr", "referencedDecl": {"id": d["id"], "kind": "VarDecl", "name": d.get("name", "")}, "type": d.get("type", {})}
                    was = n.get("type", {})
                    part = names[n["referencedDecl"]["id"]]
                    n.clear()
                    n.update({"kind": "MemberExpr", "name": part, "isArrow": False, "type": was, "inner": [whole]})
            return
        lt = self.u.rows.get(bt, bt)
        if lt.startswith("List<") and self.u.fixed(re.sub(r"&", "", qtype(d))) == len(binds):
            # `auto& [x, y, w] = an_array;` - the names are its entries: x is the entry 0, where it stands
            ids = {c["id"]: i for i, c in enumerate(binds)}
            for n in walk(self.f.decl):
                if n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") in ids:
                    whole = {"kind": "DeclRefExpr", "referencedDecl": {"id": d["id"], "kind": "VarDecl", "name": d.get("name", "")}, "type": d.get("type", {}), "_bt": bt}
                    at = {"kind": "IntegerLiteral", "value": str(ids[n["referencedDecl"]["id"]]), "type": {"qualType": "int"}}
                    was = n.get("type", {})
                    n.clear()
                    n.update({"kind": "ArraySubscriptExpr", "type": was, "_bt": lt[5:-1], "inner": [whole, at]})
            return
        if fields is None or len(fields) != len(binds) or any(f[3] is None for f in fields):
            raise Wait("names for the parts of something that is not a struct")
        ids = {c["id"]: i for i, c in enumerate(binds)}
        for n in walk(self.f.decl):
            if n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") in ids:
                f, ft = fields[ids[n["referencedDecl"]["id"]]][3], fields[ids[n["referencedDecl"]["id"]]][1]
                whole = {"kind": "DeclRefExpr", "referencedDecl": {"id": d["id"], "kind": "VarDecl", "name": d.get("name", "")}, "type": d.get("type", {}), "_bt": bt}
                was = n.get("type", {})
                n.clear()
                n.update({"kind": "MemberExpr", "name": f, "isArrow": False, "type": was, "_bt": ft, "inner": [whole]})

    @staticmethod
    def init_of(v):
        """What a declaration starts its variable with, or None."""
        if v.get("kind") == "DecompositionDecl":
            return next((c for c in kids(v) if c.get("kind") != "BindingDecl"), None)
        return kids(v)[-1] if kids(v) and v.get("init") else None

    # ---- lists: places and the steps that change several entries ----
    def whole_math(self, op, a, b):
        """a + b or a - b of whole numbers (a number written out stands in the formula itself)."""
        lit = lambda x: re.fullmatch(r"-?\d+", x) is not None
        if lit(a) and lit(b):
            return str(int(a) + int(b) if op == "+" else int(a) - int(b))
        if op == "+" and a == "0":
            return b
        if lit(b):
            return a if int(b) == 0 else f"kern::to<Int>(kern::formula([](Float a) -> Float {{ return a {op} {flit(float(b))}; }}, {self.conv(a, 'Int', 'Float')}))"
        if lit(a):
            return f"kern::to<Int>(kern::formula([](Float b) -> Float {{ return {flit(float(a))} {op} b; }}, {self.conv(b, 'Int', 'Float')}))"
        return f"kern::to<Int>(kern::formula([](Float a, Float b) -> Float {{ return a {op} b; }}, {self.conv(a, 'Int', 'Float')}, {self.conv(b, 'Int', 'Float')}))"

    def pointer_of(self, n):
        """The parameter a pointer into a list is, where `n` names one: (the list's input, the place's input,
        the entries' type) - else None."""
        b = self.unwrap(n)
        return self.pointers.get(b.get("referencedDecl", {}).get("id")) if b.get("kind") == "DeclRefExpr" else None

    def pointer_arg(self, a, entries, fixed=None):
        """What a call hands a pointer into a list (Unit.pointed): [the list, the place in it] - the list where it
        stands, read there (nothing is copied). `fixed`: the input is a list of that fixed length."""
        lt = f"List<{entries}>"
        if self.nothing(a):   # (null: no place - in a list of nothing)
            return [self.temp("none", lt, fixed), "-1"]
        lst, at = self.place(a)
        if self.tp(lst) != lt:
            raise Wait("a pointer into a list of entries of another type")
        if lst.get("kind") == "_Text":
            return [lst["_text"], at]
        return [self.place_arg(lst, lt, "const", []), at]

    def place(self, n):
        """A place in a list as C++ names it (v.begin() + i, v.end(), an array's first entry + n, v.data(), a
        pointer into a list): the list and the place as a whole number."""
        b = self.unwrap(n)
        k = b.get("kind")
        ptr = self.pointer_of(b)
        if ptr is not None:
            return {"kind": "_Text", "_text": ptr[0], "_type": f"List<{ptr[2]}>", "_bt": f"List<{ptr[2]}>"}, ptr[1]
        while k in ("CXXConstructExpr", "CXXFunctionalCastExpr", "CXXStaticCastExpr") and len(kids(b)) == 1 and not self.tp_or_none(b):
            b = self.unwrap(kids(b)[0])   # (an iterator made of an iterator)
            k = b.get("kind")
        if k == "CXXMemberCallExpr":
            m = callee(b)
            base = kids(m)[0] if kids(m) else None
            if base is not None and not self.is_this(base) and m.get("name") in ("begin", "cbegin", "end", "cend", "data") and not kids(b)[1:] and (self.tp_or_none(base) or "").startswith("List<"):
                lt = self.tp(base)
                return base, ("0" if "begin" in m["name"] or m["name"] == "data" else f"list_length<{lt[5:-1]}>({self.ex(base, lt)})")
        if k in ("CXXOperatorCallExpr", "BinaryOperator"):
            op = callee_name(b)[-1] if k == "CXXOperatorCallExpr" else b.get("opcode")
            ops = kids(b)[1:] if k == "CXXOperatorCallExpr" else kids(b)
            if op in ("+", "-") and len(ops) == 2:
                lst, at = self.place(ops[0])
                return lst, self.whole_math(op, at, self.ex(ops[1], "Int"))
        raw = bare(n)
        while raw.get("kind") == "ImplicitCastExpr" and raw.get("castKind") != "ArrayToPointerDecay" and kids(raw):
            raw = bare(kids(raw)[0])
        if raw.get("kind") == "ImplicitCastExpr" and raw.get("castKind") == "ArrayToPointerDecay" and (self.tp_or_none(kids(raw)[0]) or "").startswith("List<"):
            return kids(raw)[0], "0"
        if self.is_iter(b):   # (an iterator: the place it stands at, in the list it goes through)
            return self.home(b), self.ex(b, "Int")
        raise Wait("a place in a list of another shape")

    def tp_or_none(self, n):
        try:
            return self.tp(n)
        except Wait:
            return None

    def same_list(self, a, b):
        return re.sub(r"\s+", "", self.tree.source(self.unwrap(a))) == re.sub(r"\s+", "", self.tree.source(self.unwrap(b))) != ""

    def count_loop(self, first, last, body):
        """Steps for every whole number from first to last (a For Loop of its own)."""
        node = self.u.next_node
        self.u.next_node += 1
        self.u.members.append(f"  struct : kern::Node<{node}> {{ Int index = 0; }} n{node};")
        return [f"for (auto it{node} = flow_for(n{node}, {first}, {last}); it{node}.next();) {{", *["  " + ln for ln in body(f"n{node}.index")], "}"]

    def list_step(self, name, base, lv, it, args):
        """A call that changes a list (erase, insert, resize, assign) as the steps the language has."""
        length = f"kern::to<Float>(list_length<{it}>({lv}))"
        if name == "erase" and len(args) == 1:
            lst, at = self.place(args[0])
            if not self.same_list(lst, base):
                raise Wait("list: erase() at a place of another list")
            return [f"list_remove_at({lv}, {at});"]
        if name == "erase" and len(args) == 2:
            inner = self.unwrap(args[0])
            while inner.get("kind") in ("CXXConstructExpr",) and len(kids(inner)) == 1:
                inner = self.unwrap(kids(inner)[0])
            if inner.get("kind") == "CallExpr" and callee_name(inner) == "remove" and len(kids(inner)) == 4:
                # v.erase(std::remove(v.begin(), v.end(), x), v.end()): every entry that is x goes
                if it not in ("Int", "Text") and it not in self.u.enum_values:
                    raise Wait(f"list: every {it} that equals a value is removed")
                x = self.temp("gone", it)
                return [f"{x} = {self.ex(kids(inner)[3], it)};", f"while (loop(list_contains<{it}>({lv}, {x}))) {{", f"  list_remove({lv}, {x});", "}"]
            if inner.get("kind") == "CallExpr" and callee_name(inner) == "remove_if" and len(kids(inner)) == 4:
                return self.removed_where(base, lv, it, kids(inner)[1:], args[1])
            if inner.get("kind") == "CallExpr" and callee_name(inner) in ("remove_if", "unique", "partition", "stable_partition"):
                raise Wait("a function value (lambda / std::function)")
            (l0, first), (l1, last) = self.place(args[0]), self.place(args[1])
            if not (self.same_list(l0, base) and self.same_list(l1, base)):
                raise Wait("list: erase() at a place of another list")
            a, b = self.temp("first", "Int"), self.temp("last", "Int")
            return [f"{a} = {first};", f"{b} = {self.whole_math('-', last, '1')};"] + self.count_loop(a, b, lambda i: [f"list_remove_at({lv}, {a});"])
        if name == "insert" and len(args) in (2, 3):
            lst, at = self.place(args[0])
            if not self.same_list(lst, base):
                raise Wait("list: insert() at a place of another list")
            end = "end" in callee(self.unwrap(args[0])).get("name", "") if self.unwrap(args[0]).get("kind") == "CXXMemberCallExpr" else False
            lines = []
            if not end:
                p = self.temp("place", "Int")
                lines.append(f"{p} = {at};")
            def put(value, offset):
                return f"list_add({lv}, {value});" if end else f"list_insert({lv}, {self.whole_math('+', p, offset) if offset != '0' else p}, {value});"
            if len(args) == 3:   # the entries of a list from one place to another
                (s0, first), (s1, last) = self.place(args[1]), self.place(args[2])
                if not self.same_list(s0, s1):
                    raise Wait("list: insert() of a range between two lists")
                st = self.tp(s0)
                if st != f"List<{it}>":
                    raise Wait("list: insert() of entries of another type")
                n = self.u.entries(qtype(bare(s0)))   # (an array of C++ from its first entry to past its last: all of it)
                if self.whole_list(args[1], args[2]) or (n is not None and first.strip() == "0" and last.strip() in (str(n), f"{n}.0")):
                    # the whole list, in one go (one request for memory, as in C++)
                    return lines + [f"list_append({lv}, {self.ex(s0, st)});" if end else f"list_insert_list({lv}, {p}, {self.ex(s0, st)});"]
                src = self.temp("from", st)
                a, b = self.temp("first", "Int"), self.temp("last", "Int")
                lines += [f"{src} = {self.ex(s0, st)};", f"{a} = {first};", f"{b} = {self.whole_math('-', last, '1')};"]
                return lines + self.count_loop(a, b, lambda i: [put(f"list_get<{it}>({src}, {i})", self.whole_math("-", i, a))])
            one = self.unwrap(args[1])
            if one.get("kind") in ("CXXStdInitializerListExpr", "InitListExpr"):
                items = kids(one) if one.get("kind") == "InitListExpr" else kids(self.unwrap(kids(one)[0]))
                vals = []
                for x in items:   # (worked out one after the other, as C++ works out a list in braces)
                    v = self.temp("item", it)
                    lines.append(f"{v} = {self.ex(x, it)};")
                    vals.append(v)
                if not vals:
                    return lines
                # all of them in one go (one request for memory at most, as in C++): a list of that many
                # entries, which lies where it stands
                made = self.made_list(vals, f"List<{it}>")
                return lines + [f"list_append({lv}, {made});" if end else f"list_insert_list({lv}, {p}, {made});"]
            v = self.temp("item", it)
            return lines + [f"{v} = {self.ex(args[1], it)};", put(v, "0")]
        if name in ("resize", "assign") and len(args) in (1, 2) and not (name == "assign" and len(args) == 1):
            # the list that long, in one go (Set Length): cut at its end or filled up
            if name == "resize":
                return [f"list_resize({lv}, {self.ex(args[0], 'Int')}, {self.ex(args[1], it) if len(args) == 2 else self.u.zero(it)});"]
            # (assign: what it is given is worked out before the list is emptied)
            n, fill = self.temp("count", "Int"), self.temp("fill", it)
            return [f"{n} = {self.ex(args[0], 'Int')};", f"{fill} = {self.ex(args[1], it)};", f"list_clear({lv});", f"list_resize({lv}, {n}, {fill});"]
        if name == "fill" and len(args) == 1:
            # every entry this value (std::array::fill): worked out once, then set where each entry stands -
            # the loop C++ runs, nothing asked for
            fill, last = self.temp("fill", it), self.temp("last", "Int")
            return [f"{fill} = {self.ex(args[0], it)};", f"{last} = {self.whole_math('-', f'list_length<{it}>({lv})', '1')};"] + self.count_loop("0", last, lambda i: [f"{lv}[{i}] = {fill};"])
        raise Wait(f"list: {name}()")

    # ---- algorithms over a range of a list (<algorithm>): a loop over its places ----
    def range_of(self, name, first, last):
        """The range first..last of an algorithm - two iterators into one list: (the list, its type, the
        place of first, the place of last). The list is a place of the function that is named without
        steps of its own: every pass reads its entries where they stand."""
        (l0, a), (l1, b) = self.place(first), self.place(last)
        if not self.same_list(l0, l1):
            raise Wait(f"algorithm: std::{name} between two lists")
        lt = self.tp(l0)
        pre, got = self.lazily(lambda: self.spot(l0))
        if got is None or pre or not lt.startswith("List<"):
            raise Wait(f"algorithm: std::{name} in a list that is no place of the function")
        return l0, lt, a, b

    def entry_cpp(self, lst):
        """The C++ type of a list's entries (std::vector<T>, std::array<T, N>, T[N]: T)."""
        c = self.ctype(lst)
        if c.startswith(("std::vector<", "std::array<")):
            return template_args(c)[0]
        m = re.match(r"(.+?)\s*\[\d*\]$", c)
        if m:
            return m.group(1)
        raise Wait(f"the entries of {c.split('<')[0]}")

    def at_place(self, lst, at, it):
        """The entry of a list at the place `at` (a variable), as an expression of the C++ type the entries have."""
        e = self.entry(lst, at, it)
        e["type"] = {"qualType": self.entry_cpp(lst)}
        return e

    def named(self, name, t, cpp):
        """A variable of the function's own making as an expression - a place, like one the C++ declares."""
        i = f"_named:{name}"
        self.names[i], self.types[i] = name, t
        return {"kind": "DeclRefExpr", "referencedDecl": {"id": i, "kind": "VarDecl", "name": name}, "type": {"qualType": cpp}}

    def held_once(self, n, t):
        """A value an algorithm reads on every pass, as C++ binds it once (const T&): the place it is, when
        it is one that is named without steps; else a variable that holds it."""
        pre, got = self.lazily(lambda: self.spot(n))
        if got is not None and not pre and got[1] == t:
            return n
        held = self.temp("value", t)
        self.hoist([f"{held} = {self.ex(n, t)};"])
        return self.named(held, t, self.ctype(n))

    def function_value(self, name, fn):
        """The function an algorithm is given - a lambda written there, or a local function (`auto f =
        [&](...) {...};`): (the Blueprint function, what it is called with before its own inputs, the steps
        its declaration needs: what it captures by value)."""
        lam, lines = self.lambda_of(fn), []
        if lam is None:
            b = self.unwrap(fn)
            while b.get("kind") in ("CXXConstructExpr", "MaterializeTemporaryExpr", "CXXBindTemporaryExpr") and len(kids(b)) == 1:
                b = self.unwrap(kids(b)[0])
            d = self.tree.decl.get(b.get("referencedDecl", {}).get("id")) if b.get("kind") == "DeclRefExpr" else None
            lam = self.lambda_of(kids(d)[-1]) if d is not None and d.get("init") and kids(d) else None
            if lam is None:
                raise Wait(f"algorithm: std::{name} with a function value that is no lambda")
        else:
            lines = self.local_function({"name": "match"}, lam)
        rec = next((c for c in kids(lam) if c.get("kind") == "CXXRecordDecl"), {})
        method = next((c for c in kids(rec) if c.get("kind") == "CXXMethodDecl" and c.get("name") == "operator()"), {})
        info = self.u.lambdas.get(method.get("id"))
        if info is None:
            raise Wait(f"algorithm: std::{name} with a function value of another shape")
        return self.u.function(method["id"]), info["args"], lines

    def is_less(self, x, y, cpp, cmp=None):
        """x < y as the algorithm compares two entries: with the function it is given (`cmp`: function,
        what it is called with first), else with C++'s < of numbers."""
        if cmp is not None:
            got = self.invoke(cmp[0], cmp[1] + [x, y])
            if got[1] != "Bool":
                raise Wait("an order that gives no Bool")
            return got[0]
        if self.tp(x) not in ("Int", "Float") and not self.is64(x):
            raise Wait(f"the order of {self.tp(x)} (an algorithm without its own)")
        return self.bx({"kind": "BinaryOperator", "opcode": "<", "type": {"qualType": "bool"}, "inner": [x, y]})

    def is_equal(self, x, y, it, cpp):
        """x == y as std::find compares an entry with its value: C++'s == of the type."""
        if it == "Text":
            return f"text_equal({self.ex(x, 'Text')}, {self.ex(y, 'Text')})"
        q = self.u.find_type(cpp)
        if q in self.tree.records:
            # (a struct's == is the program's own: its method, or a function of two of it)
            op = next((c["id"] for c in kids(self.tree.records[q]) if c.get("kind") == "CXXMethodDecl" and c.get("name") == "operator==" and c.get("id") in self.tree.body), None)
            if op is None:
                raise Wait("algorithm: std::find of a struct without an == of the program's")
            got = self.invoke(self.u.function(op), [y], obj=x)
            return got[0]
        if it not in ("Int", "Float", "Bool") and it not in self.u.enum_values and not self.is64(x):
            raise Wait(f"algorithm: std::find of a {it}")
        return self.bx({"kind": "BinaryOperator", "opcode": "==", "type": {"qualType": "bool"}, "inner": [x, y]})

    def sorted_list(self, name, a):
        """std::stable_sort(v.begin(), v.end()[, before]): Sort List - the list put in order where it stands,
        stably, as std::stable_sort does: by the function it is given (a lambda that takes two entries and
        gives whether the first goes before the second: a pure function of the Blueprint, asked again and
        again), or by the entries' own < (numbers, whole numbers, flags, texts, enum values, Int64).
        std::sort leaves the order of equal entries open: it waits."""
        if name == "sort":
            raise Wait(ORDER_WAIT.format(name))
        if len(a) not in (2, 3):
            raise Wait(f"algorithm: std::{name} with {len(a)} arguments")
        ends = []
        for x, want in ((a[0], "begin"), (a[1], "end")):
            b = self.unwrap(x)
            while b.get("kind") in ("CXXConstructExpr", "MaterializeTemporaryExpr", "CXXBindTemporaryExpr", "ImplicitCastExpr") and len(kids(b)) == 1:
                b = self.unwrap(kids(b)[0])
            m = callee(b) if b.get("kind") == "CXXMemberCallExpr" else {}
            if m.get("name") != want or not kids(m) or len(kids(b)) != 1:
                raise Wait(f"algorithm: std::{name} of a part of a list (Sort List orders the whole list: begin() to end())")
            ends.append(kids(m)[0])
        if not self.same_list(ends[0], ends[1]):
            raise Wait(f"algorithm: std::{name} between two lists")
        lst = ends[0]
        lt = self.tp(lst)
        if not lt.startswith("List<"):
            raise Wait(f"algorithm: std::{name} of a {lt}")
        it = lt[5:-1]
        if len(a) == 2:
            if it not in ("Int", "Float", "Bool", "Text", "Int64") and it not in self.u.enum_values:
                raise Wait(f"algorithm: std::{name} of {it} by its own < (a list of {it} has no order of its own: a function says it)")
            order = "nullptr"
        else:
            g, given, lines = self.function_value(name, a[2])
            if given or lines:
                raise Wait(f"algorithm: std::{name} with an order that uses what the function it stands in has (an order is a function of its two entries alone)")
            self.f.calls.append(g)
            if g.wait is not None:
                raise Wait(f"it calls {g.cpp}(), which waits")
            if g.text is None or not g.pure or g.result != "Bool" or len(g.params) != 2 or any(t != it for _, t in g.params) or any(k == "ref" for k in g.kinds):
                raise Wait(f"algorithm: std::{name} with an order that is no pure function of two {it} giving a Bool")
            order = f"&{self.u.name}::{g.name}"
        before, lv, after = self.variable_of(lst, lt)
        return before + [f"list_sort<{order}>({'*this, ' if order != 'nullptr' else ''}{lv});"] + after

    def first_match(self, name, a):
        """std::find(first, last, x) / std::find_if(first, last, f) / std::find_if_not: the place of the first
        entry from first on that is x / that f holds for / does not - last when there is none, as C++ gives
        it. The entries are read where they stand, from first on, and f runs for each until one matches -
        as often as in C++, in the same order."""
        l0, lt, first, last = self.range_of(name, a[0], a[1])
        it = lt[5:-1]
        cpp = self.entry_cpp(l0)
        lines = []
        if name == "find":
            if self.u.clean(self.ctype(a[2])) != self.u.clean(cpp) or self.tp(a[2]) != it:
                raise Wait("algorithm: std::find of a value of another type")
            x = self.held_once(a[2], it)
        else:
            g, given, lines = self.function_value(name, a[2])
        at, end, go = self.temp("at", "Int"), self.temp("end", "Int"), self.temp("go", "Bool")
        entry = self.at_place(l0, at, it)
        def test():
            if name == "find":
                return self.is_equal(entry, x, it, cpp)
            got = self.invoke(g, given + [entry])
            if got[1] != "Bool":
                raise Wait(f"algorithm: std::{name} with a function that gives no Bool")
            return got[0] if name == "find_if" else f"logic_not({got[0]})"
        pre, hit = self.lazily(test)
        more = f'math_compare<"<">(kern::to<Float>({at}), kern::to<Float>({end}))'
        self.hoist(lines + [f"{at} = {first};", f"{end} = {last};", f"{go} = {more};", f"while (loop({go})) {{", *["  " + ln for ln in pre],
                            f"  if ({hit}) {{", f"    {go} = false;", "  } else {", f"    {at} += 1;", f"    {go} = {more};", "  }", "}"])
        return at

    def extreme(self, name, a):
        """std::min_element / std::max_element(first, last[, less]): the place of the first smallest / first
        largest entry - last when the range is empty. As C++ goes: every entry after the first is compared
        with the best so far (min: entry < best; max: best < entry), in their order."""
        l0, lt, first, last = self.range_of(name, a[0], a[1])
        it = lt[5:-1]
        cpp = self.entry_cpp(l0)
        lines, cmp = [], None
        if len(a) == 3:
            g, given, lines = self.function_value(name, a[2])
            cmp = (g, given)
        best, at, end = self.temp("best", "Int"), self.temp("at", "Int"), self.temp("end", "Int")
        x, y = self.at_place(l0, at, it), self.at_place(l0, best, it)
        pre, better = self.lazily(lambda: self.is_less(x, y, cpp, cmp) if name == "min_element" else self.is_less(y, x, cpp, cmp))
        more = f'math_compare<"<">(kern::to<Float>({at}), kern::to<Float>({end}))'
        self.hoist(lines + [f"{best} = {first};", f"{end} = {last};", f"{at} = {self.whole_math('+', best, '1')};", f"while (loop({more})) {{", *["  " + ln for ln in pre],
                            f"  if ({better}) {{", f"    {best} = {at};", "  }", f"  {at} += 1;", "}"])
        return best

    def removed_where(self, base, lv, it, a, last):
        """v.erase(std::remove_if(first, v.end(), f), v.end()): every entry from first on that f holds for
        goes, the others stay in their order. As std::remove_if goes: f runs once for every entry, in their
        order; an entry that stays is handed on to the next free place (a Take - nothing is copied), and the
        list is cut after the last that stays (Set Length: no memory is asked for)."""
        l0, lt, first, end = self.range_of("remove_if", a[0], a[1])
        _, at_end = self.place(last)
        here = self.spot(base)
        if not self.same_list(l0, base) or here is None or here[0] != lv or end != at_end or at_end != f"list_length<{it}>({self.ex(base, lt)})":
            raise Wait("algorithm: std::remove_if of a range that does not reach the end of its list")
        g, given, lines = self.function_value("remove_if", a[2])
        read, kept, stop = self.temp("at", "Int"), self.temp("kept", "Int"), self.temp("end", "Int")
        pre, gone = self.lazily(lambda: self.invoke(g, given + [self.at_place(l0, read, it)])[0])
        node = self.u.next_node
        self.u.next_node += 1
        self.u.members.append(f"  struct {{ {it} value = {self.u.zero(it)}; }} n{node};")
        later = f'math_compare<"<">(kern::to<Float>({kept}), kern::to<Float>({read}))'
        return lines + [f"{read} = {first};", f"{kept} = {read};", f"{stop} = {end};",
                        f'while (loop(math_compare<"<">(kern::to<Float>({read}), kern::to<Float>({stop})))) {{', *["  " + ln for ln in pre],
                        f"  if (logic_not({gone})) {{", f"    if ({later}) {{", f"      var_take(n{node}, {lv}[{read}]);", f"      {lv}[{kept}] = n{node}.value;", "    }",
                        f"    {kept} += 1;", "  }", f"  {read} += 1;", "}", f"list_resize({lv}, {kept}, {self.u.zero(it)});"]

    def whole_list(self, first, last):
        """Are these two places the begin and the end of one list - begin()/end(), cbegin()/cend(),
        std::begin(x)/std::end(x)? (`place` has already said that it is one list.)"""
        def which(n):
            n = self.unwrap(n)
            while n.get("kind") in ("CXXConstructExpr", "MaterializeTemporaryExpr", "CXXBindTemporaryExpr", "CXXFunctionalCastExpr") and len(kids(n)) == 1:
                n = self.unwrap(kids(n)[0])
            if n.get("kind") in ("CXXMemberCallExpr", "CallExpr") and len(kids(n)) in (1, 2):
                c = callee(n) if n.get("kind") == "CXXMemberCallExpr" else None
                return c.get("name") if c is not None else callee_name(n)
            return None
        return which(first) in ("begin", "cbegin") and which(last) in ("end", "cend")

    def own_value(self, n):
        """Is this a variable of the function alone - a local variable or an input by value: what nobody
        else sees?"""
        n = bare(n)
        if not self.plain_variable(n):
            return False
        name = self.names[n["referencedDecl"]["id"]]
        return not any(name == p and kind for (p, _), kind in zip(self.f.params, self.f.kinds))

    def taken_out(self, n, lt):
        """What stood where a list is moved out of, the place left empty: a Take (var_take - the list is
        handed on, not copied) where the place is one of the notation - a variable, a field, a list entry.
        Anywhere else (a map's row ...) its value is held and the place is set to the empty list."""
        got = self.spot(n)
        if got is not None and got[1] == lt and self.writable(got[0]):
            node = self.u.next_node
            self.u.next_node += 1
            self.u.members.append(f"  struct {{ {lt} value = {self.u.zero(lt)}; }} n{node};")
            self.hoist([f"var_take(n{node}, {got[0]});"])
            return f"n{node}.value"
        tmp = self.temp("moved", lt)
        self.hoist([f"{tmp} = {self.ex(n, lt)};"] + self.store(n, lambda t: t + "{}"))
        return tmp

    def moved_into(self, n, by):
        """A value that is moved (std::move(x)) and what takes it: a constructor or an assignment that takes
        its argument as T&& - then the value is gone where it was (see `call`)."""
        n = self.unwrap(n)
        if n.get("kind") == "CallExpr" and callee_name(n) == "move" and "&&" in by:
            n["_taken"] = True

    def plain_variable(self, n):
        n = bare(n)
        return n.get("kind") == "DeclRefExpr" and n.get("referencedDecl", {}).get("id") in self.names and re.fullmatch(r"\w+", self.names[n["referencedDecl"]["id"]]) is not None

    def lvalue(self, n):
        n = bare(n)
        if n.get("kind") == "DeclRefExpr":
            i = n.get("referencedDecl", {}).get("id")
            if i in self.names:
                name = self.names[i]
                # (a variable - or the entry a loop over a list is at, where that list can be changed)
                if re.fullmatch(r"\w+", name) is None and not (self.is_local(re.match(r"\w*", name).group(0)) and self.writable(name)):
                    raise Wait("a loop's own counter is changed")
                return name
            raise Wait("state outside the function is changed")
        raise Wait("an assignment to something that is not a variable")

    def stepped_place(self, n):
        """What ++ / -- inside an expression changes, as the text of its place: a variable of the function,
        or a place in one that is named without steps of its own (a field, an entry: `++serial_`,
        `next_id_++`) - read after the step as the variable is."""
        if self.plain_variable(n):
            return self.lvalue(n)
        pre, got = self.lazily(lambda: self.spot(n))
        if got is None or pre or not self.writable(got[0]):
            raise Wait("++ / -- of something that is not a variable, inside an expression")
        return got[0]

    def unwrap(self, n):
        n = bare(n)
        while n.get("kind") == "ImplicitCastExpr" and kids(n):
            n = bare(kids(n)[0])
        return n

    def is_local(self, name):
        """Is this a name a place can start at in the function being written: one of its variables or one of
        its inputs (not what a node gives)?"""
        return any(name == n for n, _ in self.locals) or any(name == n for n, _ in self.f.params) or (self.f.state and self.u.is_state_variable(name))

    def writable(self, place):
        """Can this place be changed: does it start at a variable of the function or at a reference input
        that is not const? (An input by value and a const reference are read.)"""
        root = re.match(r"\w+", place).group(0)
        return (any(root == n for n, _ in self.locals) or any(root == n and kind == "ref" for (n, _), kind in zip(self.f.params, self.f.kinds))
                or (self.f.state and self.u.is_state_variable(root)))   # (a variable of the Blueprint: C++ said whether the method may change it)

    def place_arg(self, n, t, kind, after, fixed=None):
        """What a call gives a reference input: the place the argument is. Where the argument is none of the
        notation's places (a value, a map's row by its key ...), a variable holds it for the call - and is
        stored back after it, when the function changes it (`after`: the lines for that). `fixed`: the input
        is a list of that fixed length - so is the variable (a reference is the place itself)."""
        got = self.spot(n)
        if got is not None and got[1] == t and (kind != "ref" or self.writable(got[0])):
            return got[0]
        held = self.temp("a", t, fixed)
        self.hoist([f"{held} = {self.ex(n, t)};"])
        if kind == "ref":
            after.append(lambda: self.store(n, self.typed(held, t)))
        return held

    def fixed(self, index):
        """The index of a list entry on the way to a place. For the list a loop goes through it is worked out
        once, before the loop (self.frozen: the lines that do that) - what the loop's body changes does not move
        the loop to another list."""
        if self.frozen is None or re.fullmatch(r"-?\d+|n\d+\.index", index):
            return index
        at = self.temp("at", "Int")
        self.frozen.append(f"{at} = {index};")
        return at

    def spot(self, n):
        """The place an lvalue is, as the notation writes one - a variable of the function and from it
        fields and list entries, self_.cur_.path[i] -: (text, type). None when it is none: a value, an
        input of the function, a component of a vector, a map's row by its key (that one is found first)."""
        got = self.spot_held(n)
        if got is not None and got[1] in self.u.rows:   # (a list inside a list is held as a struct of that list: its place is that field)
            return got[0] + ".items", self.u.rows[got[1]]
        return got

    def spot_held(self, n):
        b = self.unwrap(n)
        k = b.get("kind")
        if k == "_Text" and b.get("_place"):   # (a place the converter has named itself: the map a loop goes through)
            return b["_text"], b["_type"]
        if k == "DeclRefExpr":
            i = b.get("referencedDecl", {}).get("id")
            name = self.names.get(i)
            # (a variable or an input of the function - or the entry a loop over a list is at: a place that starts at one)
            return (name, self.types[i]) if name is not None and self.is_local(re.match(r"\w*", name).group(0)) else None
        if self.is_this(b) or (k == "UnaryOperator" and b.get("opcode") == "*" and kids(b) and self.is_this(kids(b)[0])):
            if self.f.state:
                return None   # (the state as a whole is the Blueprint: no place of it)
            return (self.self_name, self.f.self_type) if self.f.self_type is not None and self.is_local(self.self_name) else None
        if k == "UnaryOperator" and b.get("opcode") == "*" and kids(b) and self.pointer_of(kids(b)[0]) is not None:
            lname, aname, et = self.pointer_of(kids(b)[0])   # (what a pointer into a list points at: the entry at its place)
            self.f.indexes = True
            return f"{lname}[{aname}]", et
        holder = self.box(b)
        if holder is not None:   # the value an owned or optional value holds: the one entry of its list
            got = self.spot(holder)
            return (f"{got[0]}[0]", got[1][5:-1]) if got is not None and got[1].startswith("List<") else None
        at_iterator = self.entry_of(b)
        if at_iterator is not None:
            return self.spot(at_iterator)
        if k == "MemberExpr":
            base = kids(b)[0] if kids(b) else None
            if base is None or self.is_this(base):
                if self.f.state:
                    return self.u.state_variable(b["name"])   # (a field of the state: the Blueprint's variable)
                if self.f.self_type is None or not self.is_local(self.self_name):
                    return None
                fname, ft = self.u.field(self.f.self_type, b["name"])
                return f"{self.self_name}.{fname}", ft
            at_iterator = self.entry_of(base)
            if at_iterator is not None:
                base = at_iterator
            elif b.get("isArrow"):
                if self.box(base) is None:
                    return None
                base = self.boxed(self.box(base))
            e = self.keyed_entry(base)
            if e is not None:   # an entry of a map: its value is a place (its key is none - a key is not changed where it stands)
                if b.get("name") != "second" or self.keyed(e[0])[0] != "map":
                    return None
                return self.spot(self.entry(e[0], e[1], self.rows(e[0])[3]))
            if self.is_vec(base):
                return None
            got = self.spot(base)
            if got is None or got[1] not in self.u.struct_fields:
                return None
            fname, ft = self.u.field(got[1], b["name"])
            return f"{got[0]}.{fname}", ft
        if k == "ArraySubscriptExpr" or (k == "CXXOperatorCallExpr" and not b.get("_op") and callee_name(b) == "operator[]" and len(kids(b)) == 3):
            if k == "CXXOperatorCallExpr":
                if self.is_slot(b):
                    return None
                base, idx = kids(b)[1], kids(b)[2]
            else:
                base, idx = kids(b)
                while bare(base).get("kind") == "ImplicitCastExpr":
                    base = kids(bare(base))[0]
                if self.pointer_of(base) is not None:   # (an entry through a pointer into a list: counted from its place)
                    lname, aname, et = self.pointer_of(base)
                    self.f.indexes = True
                    return f"{lname}[{self.fixed(self.whole_math('+', aname, self.ex(idx, 'Int')))}]", et
            if self.is_vec(base):
                return None
            got = self.spot(base)
            if got is not None and got[1].startswith("Map<") and k == "ArraySubscriptExpr":   # (the value of a map's entry at its place: entry)
                self.f.indexes = True
                return f"{got[0]}[{self.fixed(self.ex(idx, 'Int'))}]", template_args(got[1])[1]
            if got is None or not got[1].startswith("List<"):
                return None
            self.f.indexes = True
            return f"{got[0]}[{self.fixed(self.ex(idx, 'Int'))}]", got[1][5:-1]
        if k == "CXXMemberCallExpr" and callee(b).get("name") in ("front", "back", "at") and kids(callee(b)):
            base = kids(callee(b))[0]
            a = kids(b)[1:]
            name = callee(b)["name"]
            if self.is_this(base) or self.keyed(base) or len(a) != (1 if name == "at" else 0):
                return None
            got = self.spot(base)
            if got is None or not got[1].startswith("List<"):
                return None
            self.f.indexes = True
            it = got[1][5:-1]
            at = self.ex(a[0], "Int") if name == "at" else "0" if name == "front" else self.whole_math("-", f"list_length<{it}>({got[0]})", "1")
            return f"{got[0]}[{self.fixed(at)}]", it
        return None

    def variable_of(self, n, t):
        """A place to change for an lvalue: (lines before, its text, lines after). The lvalue itself when
        it is a place of the notation (a variable, a field or an entry in one); otherwise a copy that is
        stored back afterwards."""
        b = self.unwrap(n)
        if self.plain_variable(b):
            return [], self.lvalue(b), []
        got = self.spot(n)
        if got is not None and self.writable(got[0]):
            return [], got[0], []
        tmp = self.temp("t", t)
        return [f"{tmp} = {self.ex(n, t)};"], tmp, self.store(n, self.typed(tmp, t))

    def typed(self, text, have):
        """A value for store: the text, converted to the type the place has."""
        return lambda want: self.conv(text, have, want)

    def box(self, n):
        """Is this the value inside an owned or optional value (*p, p->, opt.value())? Then that value's holder."""
        b = self.unwrap(n)
        k = b.get("kind")
        if k == "_Box":
            return b["inner"][0]
        if k == "CXXOperatorCallExpr" and callee_name(b) in ("operator->", "operator*") and len(kids(b)) == 2 and self.is_box(kids(b)[1]):
            return kids(b)[1]
        if k == "CXXMemberCallExpr" and callee(b).get("name") in ("value", "get") and kids(callee(b)) and self.is_box(kids(callee(b))[0]):
            return kids(callee(b))[0]
        return None

    def is_box(self, n):
        c = self.ctype(n)
        return (c.startswith("std::unique_ptr<") or c.startswith("std::optional<")) and not self.u.object_of(qtype(n))

    def boxed(self, holder):
        """The value an owned or optional value holds, as an expression of its own."""
        return {"kind": "_Box", "inner": [holder], "type": {"qualType": template_args(self.ctype(holder))[0]}}

    def nothing(self, n):
        """Is this C++'s word for "no value" (nullptr, std::nullopt)?"""
        b = self.unwrap(n)
        while b.get("kind") in CAST + ("CXXConstructExpr",) and len(kids(b)) == 1:
            b = self.unwrap(kids(b)[0])
        return b.get("kind") in ("CXXNullPtrLiteralExpr", "GNUNullExpr") or self.ctype(b) in ("std::nullopt_t", "std::nullptr_t", "nullptr_t")

    def make(self, t, arg_nodes, ctor_type=None):
        """A value of the struct t made from arguments: by the class's constructor for them, or field by field."""
        u = self.u
        q = next((x for x, n_ in u.structs.items() if n_ == t and isinstance(x, str)), None)
        rec = self.tree.records.get(q, {})
        ctors = [c for c in kids(rec) if c.get("kind") == "CXXConstructorDecl" and not c.get("isImplicit")]
        fits = [c for c in ctors if (ctor_type and c.get("type", {}).get("qualType") == ctor_type) or (not ctor_type and len([p for p in kids(c) if p.get("kind") == "ParmVarDecl"]) == len(arg_nodes))]
        if len(fits) == 1:
            if fits[0]["id"] not in self.tree.body:
                raise Wait(f"a {t} made by a constructor that is defined in another file")
            return self.invoke(u.function(fits[0]["id"]), arg_nodes)[0]
        if ctors and arg_nodes:
            raise Wait(f"a {t} made by a constructor")
        if not arg_nodes:
            return u.zero(t)
        fields = u.struct_fields[t]
        if len(arg_nodes) > len(fields):
            raise Wait(f"a {t} made of too many values")
        return f"struct_make<{t}>({', '.join([self.ex(a, ft) for a, (_, ft, _, _) in zip(arg_nodes, fields)] + [d for _, _, d, _ in fields[len(arg_nodes):]])})"

    def store(self, target, value):
        """Lines that set an lvalue: a variable, a field, a component of a vector, an entry of a list -
        of a variable, of the method's object, or of such a place inside another (the outer value is
        read, changed and stored back, level by level)."""
        t = self.unwrap(target)
        k = t.get("kind")
        if k == "DeclRefExpr":
            return [f"{self.lvalue(t)} = {value(self.tp(t))};"]
        got = self.spot(t)
        if got is not None and self.writable(got[0]):   # a place of the notation: set where it stands
            return [f"{got[0]} = {value(got[1])};"]
        if k == "CXXOperatorCallExpr":
            at_iterator = self.entry_of(t)
            if at_iterator is not None:   # (*it = v: the entry the iterator stands at)
                return self.store(at_iterator, value)
            if self.is_slot(t):
                # m[key] = v: first the value, then the key's row (new when the key is), then the row gets it
                vt = self.rows(kids(t)[1])[3]
                put = self.temp("put", vt)
                text = value(vt)
                self.hoist([f"{put} = {text};"])
                return self.store(self.held_by(t), lambda _t: put)
        if self.is_this(t):
            if self.f.state:
                raise Wait(f"the whole state of {self.u.name} is set")
            return [f"{self.self_var()} = {value(self.f.self_type)};"]
        holder = self.box(t)
        if holder is not None:   # the value an owned or optional value holds: the one entry of its list
            lt = self.tp(holder)
            before, var, after = self.variable_of(holder, lt)
            return before + [f"{var}[0] = {value(lt[5:-1])};"] + after
        if k == "MemberExpr":
            base = kids(t)[0] if kids(t) else None
            if base is None or self.is_this(base):
                if self.f.state:
                    name, ft = self.u.state_variable(t["name"])
                    return [f"{name} = {value(ft)};"]
                if self.f.self_type is None:
                    raise Wait("the state of its object (a method)")
                fname, ft = self.u.field(self.f.self_type, t["name"])
                return [f"{self.self_var()}.{fname} = {value(ft)};"]
            at_iterator = self.entry_of(base)
            if at_iterator is not None:
                base = at_iterator
            elif t.get("isArrow"):
                if self.box(base) is None:
                    raise Wait("a pointer")
                base = self.boxed(self.box(base))
            if self.is_vec(base):
                if t.get("name") not in ("x", "y", "z"):
                    raise Wait(f"vector: .{t.get('name')}")
                old = self.ex(base, "Vector")
                parts = [f"vector_break({old}).x", f"vector_break({old}).y", f"vector_break({old}).z"]
                parts[["x", "y", "z"].index(t["name"])] = value("Float")
                return self.store(base, lambda _t: f"vector_make({', '.join(parts)})")
            st = self.tp(base)
            if st in self.u.struct_fields:
                fname, ft = self.u.field(st, t["name"])
                before, var, after = self.variable_of(base, st)
                return before + [f"{var}.{fname} = {value(ft)};"] + after
            raise Wait(f"a member of {st} is set")
        if k in ("ArraySubscriptExpr", "CXXOperatorCallExpr"):
            if k == "CXXOperatorCallExpr":
                if callee_name(t) != "operator[]":
                    raise Wait("an assignment to what an operator gives")
                base, idx = kids(t)[1], kids(t)[2]
            else:
                base, idx = kids(t)
            base = self.unwrap(base) if not self.is_this(base) else base
            if self.is_vec(base) and self.ctype(base) != "kern::Vec3":
                raise Wait("a component of a vector by its number is set")
            if self.is_vec(base):
                # a component of a vector by its number, as the vector's operator[] picks it (0: x, 1: y, any
                # other: z) - the number and the value worked out once, the vector set anew where it stands
                at, put = self.temp("at", "Float"), self.temp("put", "Float")
                lines = [f"{at} = {self.ex(idx, 'Float')};", f"{put} = {value('Float')};"]
                old = self.ex(base, "Vector")
                parts = [f"kern::formula([](Float i, Float v, Float c) -> Float {{ return {cond} ? v : c; }}, {at}, {put}, vector_break({old}).{c})"
                         for cond, c in (("i == 0.0", "x"), ("i == 1.0", "y"), ("i != 0.0 && i != 1.0", "z"))]
                return lines + self.store(base, lambda _t: f"vector_make({', '.join(parts)})")
            lt = self.tp(base)
            if not lt.startswith("List<"):
                raise Wait("an entry of something that is not a list is set")
            before, var, after = self.variable_of(base, lt)
            return before + [f"{var}[{self.ex(idx, 'Int')}] = {value(lt[5:-1])};"] + after
        raise Wait(f"an assignment to {k}")

    def invoke(self, g, args, obj=None):
        """A call of one of the program's functions: (text, type) of what the C++ call returns, or
        (None, None). What it changes - its object, what it writes through its parameters - it changes
        where it stands: the call names those places (place_arg). A function that changes something is
        called by a step before the statement being written."""
        self.f.calls.append(g)
        self.f.indexes = self.f.indexes or g.indexes
        if g.state and g.mutates and self.f.state and not self.f.mutates:
            self.f.mutates, self.f.pure = True, False   # (what a function of the state it calls changes, it changes)
        texts = []
        params = g.params
        if any(x.get("kind") == "CXXDefaultArgExpr" for x in args):
            # an argument the call leaves out: the default the function's declaration gives it
            decls, chain = [], g.decl
            while chain is not None:
                decls.append(chain)
                chain = self.tree.decl.get(chain.get("previousDecl"))
            filled = []
            for i, x in enumerate(args):
                if x.get("kind") != "CXXDefaultArgExpr":
                    filled.append(x)
                    continue
                default = None
                for dd in decls:
                    ps = [p for p in kids(dd) if p.get("kind") == "ParmVarDecl"]
                    if i < len(ps) and kids(ps[i]):
                        default = kids(ps[i])[-1]
                        break
                if default is None:
                    raise Wait("a call with a default argument that is declared elsewhere")
                filled.append(default)
            args = filled
        kinds = g.kinds
        after = []   # (what is stored back after the call: reference arguments that are no places of the notation)
        if g.state and obj is not None and not self.is_this(obj):
            raise Wait(f"a function of the Blueprint {self.u.name} on another object of its state")
        if g.self_type is not None and not g.ctor:
            if obj is None or self.is_this(obj):
                if self.f.self_type != g.self_type:
                    raise Wait("a method of another object is called as its own")
                if kinds[0] == "ref":
                    self.self_var()   # (a method that only reads its object does not call one that changes it)
                texts.append(self.self_name)
            else:
                texts.append(self.place_arg(obj, g.self_type, kinds[0], after))
            params = params[1:]
            kinds = kinds[1:]
        first = len(g.params) - len(params)   # (a method: its object is the first input)
        if len(args) != len(params) - len(g.pointers):
            raise Wait("a call with default arguments")
        j = 0
        for a in args:
            if first + j in g.pointers:   # (a pointer into a list: the list and the place in it - Unit.pointed)
                texts += self.pointer_arg(a, params[j][1][5:-1], g.fixed.get(first + j))
                j += 2
                continue
            (_, pt), kind = params[j], kinds[j]
            texts.append(self.place_arg(a, pt, kind, after, g.fixed.get(first + j)) if kind else self.ex(a, pt))
            j += 1
        gives = g.self_type if g.ctor else g.value
        texts = [texts[i] for i in g.order()]   # (as the Blueprint function has its inputs: those by reference first)
        call = f"{g.name}({', '.join(texts)}).Result"
        if not g.pure:
            # a function that changes something outside itself (a place of its caller, a step of the engine) is
            # called as a step: the call keeps what it gave (n12 = F(...); n12.Result), and who calls it is such a
            # function too
            self.f.pure = False
            if gives is None:
                self.hoist([f"{g.name}({', '.join(texts)});"])
            else:
                node = self.u.next_node
                self.u.next_node += 1
                self.u.members.append(f"  {g.name}Result n{node};")
                self.hoist([f"n{node} = {g.name}({', '.join(texts)});"])
                call = f"n{node}.Result"
        for lines in after:
            self.hoist(lines())
        return (call, gives) if gives is not None else (None, None)

    # ---- expressions ----
    def conv(self, text, have, want):
        if want is None or have == want:
            return text
        if want == "Int64" and have in ("Int", "Float", "Bool"):
            return self.to64(text, have)
        if have == "Int64" and want in ("Int", "Float", "Bool"):
            return self.from64(text, "whole64u", "long" if want == "Int" else "double", want)
        if want == "Float" and have == "Int" and re.fullmatch(r"-?\d+", text):
            return flit(float(text))
        if text.startswith("kern::to<") and have in ("Float", "Int", "Bool") and want in ("Float", "Int", "Bool"):
            # (a conversion of a conversion - (int)(float)i: the value in between is a value of its own)
            text = f"value_{have.lower()}({text})"
        if want == "Float" and have in ("Int", "Bool"):
            return f"kern::to<Float>({text})"
        if want == "Int" and have in ("Float", "Bool"):
            return f"kern::to<Int>({text})"
        if want == "Bool" and have == "Float":   # (exactly not 0, as C++ asks)
            return f'math_compare<">">(kern::formula([](Float x) -> Float {{ return x != 0.0; }}, {text}), 0.5)'
        if want == "Bool" and have in ("Float", "Int"):
            return f'math_compare<"!=">({text if have == "Float" else f"kern::to<Float>({text})"}, 0.0)'
        if want in ("Int", "Float") and have in self.u.enum_values:
            index = f"enum_index<{have}>({text})"
            if have in self.u.enum_numbers:   # (the number C++ gives the value, not its place)
                index = f"list_get<Int>({self.u.enum_numbers[have]}, {index})"
            return index if want == "Int" else f"kern::to<Float>({index})"
        if want in self.u.enum_values and have in ("Int", "Float"):
            index = text if have == "Int" else f"kern::to<Int>({text})"
            if want in self.u.enum_numbers:
                index = f"list_find<Int>({self.u.enum_numbers[want]}, {index})"
            return f"enum_from_index<{want}>({index}).value"
        if want in self.u.rows and have == self.u.rows[want]:
            return f"struct_make<{want}>({text})"
        if have in self.u.rows and want == self.u.rows[have]:
            return f"struct_break<{have}>({text}).items"
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
            if self.arithmetic(kids(b)[-1]) or (self.tp(b) == "Int" and self.tp(kids(b)[-1]) == "Float"):
                return True
            if self.tp(b) == "Int" and self.tp(kids(b)[-1]) == "Int":
                try:
                    return self.narrow("x", self.ctype(kids(b)[-1]), self.ctype(b), kids(b)[-1]) != "x"
                except Wait:
                    return True
            return False
        if not self.numeric(b):
            return False
        if k == "BinaryOperator" and b.get("opcode") in ("+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>") and all(self.numeric(c) for c in kids(b)):
            return True
        if k == "UnaryOperator" and b.get("opcode") in ("-", "+", "~"):
            return True
        if k == "ConditionalOperator" and self.tp(b) in ("Float", "Int"):
            return True
        return self.formula_function(b)

    def formula_function(self, b):
        """Is this a call of one of the functions of numbers a formula has - with the arguments the formula
        has it for? (min(a, b) is; a min of three is a call like any other.)"""
        if b.get("kind") != "CallExpr" or not all(self.numeric(a) for a in kids(b)[1:]):
            return False
        name, count = callee_name(b), len(kids(b)) - 1
        if name in ("clamp", "clampf"):
            return count == 3
        if name in MATH:
            return count == {"min": 2, "max": 2, "pow": 2, "atan2": 2, "mod": 2}.get(MATH[name], 1)
        if name in ("hypot", "ldexp", "ldexpf"):
            return count == 2
        if name == "lerp":
            return count == 3
        return name in MORE_MATH and count == 1

    def value(self, n):
        """(text, Blueprint type) of an expression."""
        u = self.u
        b = bare(n)
        k = b.get("kind")
        if k == "_Text":
            return b["_text"], b["_type"]
        e = self.keyed_entry(b) or (self.keyed_entry(self.entry_of(b)) if self.entry_of(b) is not None else None)
        if e is not None:   # an entry of a set: its key; of a map: its key and value are read each by itself
            if self.keyed(e[0])[0] == "set":
                return self.keyed_key(e)
            raise Wait("an entry of a map as a value (its key and its value are read each by itself)")
        if k == "ImplicitCastExpr" and b.get("castKind") in ("NoOp", "ArrayToPointerDecay") and self.text_buffer_of(b) is not None:
            x = b   # (a text buffer read as a text: its text)
            while x.get("kind") in ("ImplicitCastExpr", "ParenExpr") and kids(x):
                x = kids(x)[0]
            return self.names[x["referencedDecl"]["id"]], "Text"
        if k == "_Box":
            got = self.spot(b)
            if got is not None:
                return got
            lt = self.tp(b["inner"][0])
            return f"list_get<{lt[5:-1]}>({self.ex(b['inner'][0], lt)}, 0)", lt[5:-1]
        if k == "CXXRewrittenBinaryOperator":   # (a != b written by the compiler as !(a == b))
            return self.value(kids(b)[0])
        # 64-bit bit patterns (Whole64): what C++ computes with them, the functions of blueprint_whole64.h do
        if k in CAST and kids(b) and b.get("castKind") not in ("LValueToRValue", "NoOp") and self.is64(kids(b)[-1]) and not self.is64(b):
            text, have = self.value(kids(b)[-1])
            want = self.tp(b)
            if have != self.whole64() or want is None:
                return self.conv(text, have, want), want
            return self.from64(text, self.ctype(kids(b)[-1]), self.ctype(b), want), want
        if (k == "IntegerLiteral" or k in CAST or (k == "InitListExpr" and len(kids(b)) <= 1) or (k == "BinaryOperator" and b.get("opcode") in ("+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>"))
                or (k == "UnaryOperator" and b.get("opcode") in ("-", "+", "~")) or k == "ConditionalOperator") and self.is64(b):
            return self.value64(b)
        if k == "BinaryOperator" and b.get("opcode") in COMPARE and (self.is64(kids(b)[0]) or self.is64(kids(b)[1])):
            x, y = kids(b)
            op = b["opcode"]
            signed = self.ctype(x) in ("whole64s", "long", "long long") and self.ctype(y) in ("whole64s", "long", "long long")
            l, r = self.ex64(x), self.ex64(y)
            if op in ("==", "!="):
                same = self.w64("w64_equal", l, r)
                return (same if op == "==" else f"logic_not({same})"), "Bool"
            less = "w64_less_signed" if signed else "w64_less"
            return {"<": self.w64(less, l, r), ">": self.w64(less, r, l), "<=": f"logic_not({self.w64(less, r, l)})", ">=": f"logic_not({self.w64(less, l, r)})"}[op], "Bool"
        if k in ("FloatingLiteral", "IntegerLiteral", "CXXBoolLiteralExpr", "CharacterLiteral"):
            t = self.tp(b)
            v = Const(u).number(b)
            if t == "Float":
                return flit(Const(u).decimal(b)), "Float"
            if t == "Int":
                if abs(v) >= 2.0 ** 53:
                    raise Wait("a 64-bit whole number that wraps around or is shifted (a Blueprint's are exact up to 2^53)")
                return str(int(v)), "Int"
            return ("true" if v else "false"), "Bool"
        if k == "StringLiteral":
            return text_literal(c_string(b["value"])), "Text"
        if k in ("CXXNullPtrLiteralExpr", "GNUNullExpr"):
            raise Wait("a pointer")
        if self.arithmetic(b):
            if k in ("ConditionalOperator", "BinaryOperator") and (k == "ConditionalOperator" or b.get("opcode") in ("&&", "||")):
                mark = len(self.pre)
                try:
                    return self.formula(b)
                except Hoists:
                    if not ENGINE_NODES:
                        raise
                    del self.pre[mark:]
                    if k == "ConditionalOperator":
                        return self.chosen(self.bx(kids(b)[0]), kids(b)[1], kids(b)[2], self.tp(b))
                    return self.either(b["opcode"], self.bx(kids(b)[0]), kids(b)[1])
            return self.formula(b)
        if k in CAST:
            inner = kids(b)[-1]
            ck = b.get("castKind", "")
            if ck == "PointerToBoolean" and self.pointer_of(inner) is not None:   # (a pointer into a list: not null - it has a place)
                return f'math_compare<">=">(kern::to<Float>({self.pointer_of(inner)[1]}), 0.0)', "Bool"
            if ck == "NullToPointer" and self.u.object_of(qtype(b)):
                return "0", "Int"   # (no object of the engine: the handle 0)
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
            if i in self.pointers:
                raise Wait("a pointer into a list used as more than its entries, its place in a call or whether it is null")
            if ref.get("kind") == "EnumConstantDecl":
                d = self.tree.decl.get(i)
                q = d.get("_enum") if d else None
                if q is None:
                    raise Wait("an enum value of an enum without a name")
                e = u.enum(q)
                return f"{e}::{enum_value(u, e, ref['name'])}", e
            if ref.get("kind") == "VarDecl":
                if i in u.variables:   # (a constant of the function's own, `static const Vec3 kHair[6] = {…};` - see stmt1)
                    return u.variables[i]
                d = self.tree.decl.get(i)
                if d is None:
                    if ref.get("name") == "kPi":
                        return self.engine_constant("kPi", 3.14159265358979323846, "Pi")
                    raise Wait(f"the engine's {ref.get('name')}")
                return u.constant(d)
            raise Wait(f"a name that is {ref.get('kind')}")
        if k == "MemberExpr":
            got = self.spot(b)
            if got is not None:   # a field at a place: read where it stands
                return got
            base = kids(b)[0] if kids(b) else None
            e = self.keyed_entry(self.entry_of(base) or base) if base is not None else None
            if e is not None:   # an entry of a map or a set: its key - its value where it stands (above)
                if b.get("name") == "first":
                    return self.keyed_key(e)
                return self.value(self.entry(e[0], e[1], self.rows(e[0])[3]))
            if base is None or self.is_this(base):
                if self.f.state:
                    return u.state_variable(b["name"])
                if self.f.self_type is None:
                    raise Wait("the state of its object (a method)")
                fname, ft = u.field(self.f.self_type, b["name"])
                return f"struct_break<{self.f.self_type}>({self.self_name}).{fname}", ft
            at_iterator = self.entry_of(base)
            if at_iterator is not None:
                base = at_iterator
            elif b.get("isArrow"):
                if self.box(base) is None:
                    raise Wait("a pointer")
                base = self.boxed(self.box(base))
            if self.is_vec(base):
                if b.get("name") not in ("x", "y", "z"):
                    raise Wait(f"vector: .{b.get('name')}")
                return f"vector_break({self.ex(base, 'Vector')}).{b['name']}", "Float"
            st = self.tp(base)
            if st in u.struct_fields:
                fname, ft = u.field(st, b["name"])
                return f"struct_break<{st}>({self.ex(base, st)}).{fname}", ft
            if self.u.object_of(qtype(base)):
                raise Wait("a field of an object of the engine (its state is the engine's)")
            raise Wait(f"a member of {st}")
        if k == "UnaryOperator":
            op = b.get("opcode")
            if op == "!":
                return f"logic_not({self.bx(kids(b)[0])})", "Bool"
            if op == "*" and self.is_this(b) and self.f.state:
                raise Wait(f"the whole state of {self.u.name} as a value")
            if op == "*" and self.is_this(b) and self.f.self_type is not None:
                return self.self_name, self.f.self_type
            if op in ("*", "&") and (self.u.object_of(qtype(b), by_value=True) or self.u.object_of(qtype(kids(b)[0]), by_value=True)):
                return self.ex(kids(b)[0], "Int"), "Int"   # (an object of the engine and a pointer to it: one handle)
            if op in ("*", "&"):
                raise Wait("a pointer")
            if op == "~":
                raise Wait("bit arithmetic (& | ^ << >>)")
            if op in ("++", "--"):
                # ++i inside an expression: the step runs before the statement, the value is the variable
                # (i++: what it was before)
                target = kids(b)[0]
                t = self.tp(target)
                name = self.stepped_place(target)
                if b.get("isPostfix"):
                    old = self.temp("was", t)
                    self.hoist([f"{old} = {name};"] + self.effect(b))
                    return old, t
                self.hoist(self.effect(b))
                return name, t
            raise Wait(f"the operator {op}")
        if k == "BinaryOperator":
            op = b.get("opcode")
            x, y = kids(b)
            if op in ("&&", "||"):
                left = self.bx(x)
                try:
                    return f"logic_{'and' if op == '&&' else 'or'}({left}, {self.sometimes(lambda: self.bx(y))})", "Bool"
                except Hoists:
                    if not ENGINE_NODES:
                        raise
                    return self.either(op, left, y)
            if op in ("==", "!=") and (self.pointer_of(x) is not None and self.nothing(y) or self.pointer_of(y) is not None and self.nothing(x)):
                at = (self.pointer_of(x) or self.pointer_of(y))[1]   # (a pointer into a list and null: has it no place)
                return f'math_compare<"{"<" if op == "==" else ">="}">(kern::to<Float>({at}), 0.0)', "Bool"
            if op in COMPARE:
                tx, ty = self.tp(x), self.tp(y)
                if op in ("==", "!=") and "Float" in (tx, ty) and tx in ("Float", "Int") and ty in ("Float", "Int"):
                    return self.formula(b)   # (exactly equal, as C++ compares: the Compare node's == has a margin)
                if op not in ("==", "!=") and self.ctype(x) in WIDE_UNSIGNED and self.ctype(y) in WIDE_UNSIGNED and not (self.never_negative(x) and self.never_negative(y)):
                    return self.formula(b)   # (two unsigned 64-bit numbers: which is less, the formula says - see fx)
                if tx in ("Float", "Int") and ty in ("Float", "Int") or (tx in ("Float", "Int", "Bool") and ty in ("Float", "Int", "Bool") and op not in ("==", "!=")):
                    return f'math_compare<"{op}">({self.ex(x, "Float")}, {self.ex(y, "Float")})', "Bool"
                if op in ("==", "!="):
                    if tx == "Bool" or ty == "Bool":
                        r = f"logic_xor({self.bx(x)}, {self.bx(y)})"
                        return (r if op == "!=" else f"logic_not({r})"), "Bool"
                    if tx in u.enum_values and ty == tx:
                        r = f"enum_equal<{tx}>({self.ex(x, tx)}, {self.ex(y, tx)})"
                        return (r if op == "==" else f"logic_not({r})"), "Bool"
                if tx in u.enum_values and ty in u.enum_values:   # (which value comes first: by their numbers)
                    return f'math_compare<"{op}">({self.ex(x, "Float")}, {self.ex(y, "Float")})', "Bool"
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
            cond = self.bx(c)
            try:
                return f"logic_select<{t}>({cond}, {self.sometimes(lambda: self.ex(x, t))}, {self.sometimes(lambda: self.ex(y, t))})", t
            except Hoists:
                if not ENGINE_NODES:
                    raise
                return self.chosen(cond, x, y, t)
        if k == "CallExpr":
            return self.call(b)
        if k == "CXXOperatorCallExpr":
            return self.operator(b)
        if k in ("CXXConstructExpr", "CXXTemporaryObjectExpr", "InitListExpr"):
            return self.construct(b)
        if k == "CXXMemberCallExpr":
            return self.member_call(b)
        if k == "ArraySubscriptExpr":
            got = self.spot(b)
            if got is not None:
                return got
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
            raise Wait("a pointer")
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
            self.u.variable_lines.append(f"  KERN_CONST Float {n} = {flit(value)};")
            self.u.variable_values[n] = ("Float", flit(value), {"name": cpp, "_engine": True})
            self.u.variables[key] = (n, "Float")
        return self.u.variables[key]

    # ---- formulas ----
    def formula(self, n):
        leaves = collections.OrderedDict()   # value text -> parameter name
        body = self.fx(n, leaves)
        t = self.tp(n)
        if not leaves:
            v = Const(self.u).number(n)   # (everything in it is known: it is its value)
            if t == "Int":
                return str(int(v)), "Int"
            if t == "Bool":
                return ("true" if v else "false"), "Bool"
            return flit(v), "Float"
        params = ", ".join(f"Float {p}" for p in leaves.values())
        text = f"kern::formula([]({params}) -> Float {{ return {body}; }}, {', '.join(leaves)})"
        if t == "Int":
            return f"kern::to<Int>({text})", "Int"
        if t == "Bool":
            return f'math_compare<">">({text}, 0.5)', "Bool"
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
            if is_word(base) or base in ("kern", "result", "Result"):
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
        if self.is64(b):   # (a 64-bit bit pattern in a computation with numbers: the number it is)
            text, have = self.value(b)
            return self.leaf(None, leaves, self.from64(text, self.ctype(b), "double", "Float") if have == self.whole64() else self.conv(text, have, "Float"))
        if k in CAST and kids(b) and b.get("castKind") not in ("LValueToRValue", "NoOp") and self.is64(kids(b)[-1]):
            text, have = self.value(b)
            return self.leaf(None, leaves, self.conv(text, have, "Float"))
        if k in ("FloatingLiteral", "IntegerLiteral", "CXXBoolLiteralExpr", "CharacterLiteral"):
            v = Const(self.u).decimal(b)
            if k == "IntegerLiteral" and v >= 2.0 ** 63 and self.ctype(b) in WIDE_UNSIGNED:
                v -= 2.0 ** 64   # (read with its sign, as a Blueprint holds a 64-bit number)
            if k == "IntegerLiteral" and abs(v) >= 2.0 ** 53:
                raise Wait("a 64-bit whole number that wraps around or is shifted (a Blueprint's are exact up to 2^53)")
            return flit(v) if v >= 0 else f"({flit(v)})"
        if k in CAST and kids(b) and self.numeric(b) and self.numeric(kids(b)[-1]):
            inner = kids(b)[-1]
            want, have = self.tp(b), self.tp(inner)
            if want == "Int" and have == "Float":
                return f"kern::math::trunc({self.fx(inner, leaves)})"
            if want == "Bool" and have != "Bool":
                return f"({self.fx(inner, leaves)} != 0.0)"
            if want == "Float" and self.ctype(inner) in WIDE_UNSIGNED:
                # an unsigned 64-bit number as a number: a Blueprint holds it read with its sign
                x = self.once(inner, leaves)
                return f"({x} < 0.0 ? {x} + 18446744073709551616.0 : {x})"
            return self.narrow(self.fx(inner, leaves), self.ctype(inner), self.ctype(b), inner)
        if not self.numeric(b):
            if re.search(r"\*\s*(const\s*)?$", self.ctype(b)):
                raise Wait("a pointer")   # (&p - list.data(): where an entry stands, counted by addresses)
            raise Wait("a value that is not a number in a computation")
        if k == "UnaryOperator" and b.get("opcode") in ("-", "+", "!"):
            x = self.fx(kids(b)[0], leaves)
            if b["opcode"] == "-" and self.ctype(b) in WIDE_UNSIGNED:
                return f"(-{x})"   # (64 bit: the number read with its sign, see WIDE_UNSIGNED)
            if b["opcode"] == "-" and self.ctype(b) in UNSIGNED:
                return f"kern::math::wrap32(-{x})"
            return x if b["opcode"] == "+" else f"({'-' if b['opcode'] == '-' else '!'}{x})"
        if k == "UnaryOperator" and b.get("opcode") == "~":
            x = self.fx(kids(b)[0], leaves)
            if self.ctype(b) in WIDE_UNSIGNED:
                return f"(-{x} - 1.0)"
            if self.ctype(b) in UNSIGNED:
                return f"kern::math::wrap32(kern::math::bxor({x}, -1.0))"
            return f"kern::math::bxor({x}, -1.0)"
        if k == "BinaryOperator":
            op = b.get("opcode")
            x, y = kids(b)
            if op == "/" and self.entries_of(b) is not None:
                return flit(float(self.entries_of(b)))
            if op in ("+", "-", "*"):
                if self.ctype(b) in WIDE_UNSIGNED:
                    return f"({self.fx(x, leaves)} {op} {self.fx(y, leaves)})"
                if self.ctype(b) in UNSIGNED:
                    # unsigned arithmetic wraps around at 32 bit: the formula's wrap32 and mul32
                    if op == "*":
                        return f"kern::math::mul32({self.fx(x, leaves)}, {self.fx(y, leaves)})"
                    return f"kern::math::wrap32({self.fx(x, leaves)} {op} {self.fx(y, leaves)})"
                return f"({self.fx(x, leaves)} {op} {self.fx(y, leaves)})"
            if op in ("/", "%") and self.tp(b) == "Int" and not (bare(y).get("kind") == "IntegerLiteral" or (bare(y).get("kind") in CAST and kids(bare(y)) and bare(kids(bare(y))[-1]).get("kind") == "IntegerLiteral")):
                self.f.indexes = True
            if op == "/":
                if self.tp(b) == "Int":
                    return f"kern::math::trunc(kern::math::div({self.fx(x, leaves)}, {self.fx(y, leaves)}))"
                return f"kern::math::div({self.fx(x, leaves)}, {self.fx(y, leaves)})"
            if op == "%":
                return f"kern::math::mod({self.fx(x, leaves)}, {self.fx(y, leaves)})"
            if op in ("<", ">", "<=", ">=") and self.ctype(x) in WIDE_UNSIGNED and self.ctype(y) in WIDE_UNSIGNED and not (self.never_negative(x) and self.never_negative(y)):
                # two unsigned 64-bit numbers, held read with their signs: one that reads negative is the larger
                l, r = (x, y) if op in ("<", ">=") else (y, x)
                lt, rt = self.once(l, leaves), self.once(r, leaves)
                less = f"(({lt} < 0.0) != ({rt} < 0.0) ? ({rt} < 0.0) : ({lt} < {rt}))"
                return less if op in ("<", ">") else f"(!{less})"
            if op in COMPARE and self.numeric(x) and self.numeric(y):
                return f"({self.fx(x, leaves)} {op} {self.fx(y, leaves)})"
            if op in ("&&", "||"):
                return f"({self.fx(x, leaves)} {op} {self.sometimes(lambda: self.fx(y, leaves))})"
            if op in ("&", "|", "^"):
                return f"kern::math::{'band' if op == '&' else 'bor' if op == '|' else 'bxor'}({self.fx(x, leaves)}, {self.fx(y, leaves)})"
            if op in ("<<", ">>") and bare(y).get("kind") != "IntegerLiteral" and not (bare(y).get("kind") in CAST and kids(bare(y)) and bare(kids(bare(y))[-1]).get("kind") == "IntegerLiteral"):
                self.f.indexes = True   # (C++ leaves a shift by the width of the number or more undefined; the formula's shl and shr say what it is)
            if op == ">>":
                return f"kern::math::shr({self.fx(x, leaves)}, {self.fx(y, leaves)})"
            if op == "<<":
                if self.ctype(b) in WIDE_UNSIGNED:
                    # an unsigned 64-bit number shifted up: within what a Blueprint holds only when the bits it has
                    # and the shift stay below 53 together - otherwise it is a bit pattern (a key, a hash)
                    inner = self.unwrap(x)
                    while inner.get("kind") in CAST and kids(inner) and self.ctype(inner) in WIDE_UNSIGNED and self.ctype(kids(inner)[-1]) in WIDE_UNSIGNED:
                        inner = self.unwrap(kids(inner)[-1])
                    has = {"unsigned int": 32, "unsigned short": 16, "unsigned char": 8, "bool": 1}.get(self.ctype(kids(inner)[-1]) if inner.get("kind") in CAST and kids(inner) else "", 64)
                    by = bare(y)
                    while by.get("kind") in CAST and kids(by):
                        by = bare(kids(by)[-1])
                    if by.get("kind") != "IntegerLiteral" or has + int(by["value"]) > 53:
                        raise Wait("a 64-bit whole number that wraps around or is shifted (a Blueprint's are exact up to 2^53)")
                shifted = f"kern::math::shl({self.fx(x, leaves)}, {self.fx(y, leaves)})"
                if self.ctype(b) in ("long", "long long") or self.ctype(b) in WIDE_UNSIGNED:
                    return shifted   # (a 64-bit number: what is shifted stays whole while it is below 2^53)
                return f"kern::math::wrap32({shifted})" if self.ctype(b) in UNSIGNED else f"kern::math::wrap32s({shifted})"
        if k == "ConditionalOperator":
            c, x, y = kids(b)
            return f"({self.fb(c, leaves)} ? {self.sometimes(lambda: self.fx(x, leaves))} : {self.sometimes(lambda: self.fx(y, leaves))})"
        if self.formula_function(b):
            name = callee_name(b)
            if name == "hash_u32" and len(kids(b)) == 2:   # kern::hash_u32 (lowbias32), step by step
                one = f"kern::formula([](Float x) -> Float {{ return kern::math::mul32(kern::math::bxor(x, kern::math::shr(x, 16.0)), 2146121005.0); }}, {self.ex(kids(b)[1], 'Float')})"
                two = f"kern::formula([](Float x) -> Float {{ return kern::math::mul32(kern::math::bxor(x, kern::math::shr(x, 15.0)), 2221713035.0); }}, {one})"
                x = self.leaf(None, leaves, two)
                return f"kern::math::bxor({x}, kern::math::shr({x}, 16.0))"
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
            if name in ("ldexp", "ldexpf") and len(args) == 2:
                return f"({args[0]} * kern::math::pow(2.0, {args[1]}))"
            if name in ("tanh", "tanhf"):
                return f"kern::math::div(kern::math::exp(2.0 * {args[0]}) - 1.0, kern::math::exp(2.0 * {args[0]}) + 1.0)"
            if name in ("sinh", "sinhf"):
                return f"((kern::math::exp({args[0]}) - kern::math::exp(-{args[0]})) * 0.5)"
            if name in ("cosh", "coshf"):
                return f"((kern::math::exp({args[0]}) + kern::math::exp(-{args[0]})) * 0.5)"
            if name in ("asinh", "asinhf"):
                return f"kern::math::log({args[0]} + kern::math::sqrt(({args[0]} * {args[0]}) + 1.0))"
            if name in ("log2", "log2f"):
                return f"kern::math::div(kern::math::log({args[0]}), 0.6931471805599453)"
            if name in ("log10", "log10f"):
                return f"kern::math::div(kern::math::log({args[0]}), 2.302585092994046)"
            if name in ("cbrt", "cbrtf"):
                return f"(kern::math::sign({args[0]}) * kern::math::pow(kern::math::abs({args[0]}), 0.3333333333333333))"
        return self.leaf(b, leaves)

    def fb(self, n, leaves):
        b = bare(n)
        k = b.get("kind")
        if k == "BinaryOperator" and b.get("opcode") in COMPARE + ("&&", "||") and all(self.numeric(c) for c in kids(b)):
            x, y = kids(b)
            if b["opcode"] in ("&&", "||"):
                return f"({self.fb(x, leaves)} {b['opcode']} {self.sometimes(lambda: self.fb(y, leaves))})"
            return f"({self.fx(x, leaves)} {b['opcode']} {self.fx(y, leaves)})"
        if k == "UnaryOperator" and b.get("opcode") == "!":
            return f"(!{self.fb(kids(b)[0], leaves)})"
        if k in CAST and kids(b) and self.numeric(kids(b)[-1]) and self.tp(kids(b)[-1]) != "Bool":
            return f"({self.fx(kids(b)[-1], leaves)} != 0.0)"
        if k in CAST and kids(b):
            return self.fb(kids(b)[-1], leaves)
        return f"({self.leaf(b, leaves, self.conv(self.bx(b), 'Bool', 'Float'))} != 0.0)"

    def entries_of(self, b):
        """`sizeof a / sizeof a[0]`: how many entries the array a has (its type says it)."""
        x, y = (bare(c) for c in kids(b))
        while x.get("kind") in CAST and kids(x):
            x = bare(kids(x)[-1])
        while y.get("kind") in CAST and kids(y):
            y = bare(kids(y)[-1])
        if x.get("kind") != "UnaryExprOrTypeTraitExpr" or y.get("kind") != "UnaryExprOrTypeTraitExpr" or x.get("name") != "sizeof" or y.get("name") != "sizeof" or not kids(x) or not kids(y):
            return None
        n = self.u.entries(qtype(bare(kids(x)[0])))   # (the outer length: T[3][2] has 3 entries)
        return n if n is not None and bare(kids(y)[0]).get("kind") == "ArraySubscriptExpr" else None

    # ---- 64-bit bit patterns ----
    def whole64(self):
        return "Int64"

    def is64(self, n):
        """Is this value a 64-bit bit pattern (a Whole64 in the Blueprint)? Its type's spelling says so; where
        the compiler's tree has lost the spelling, what it is made of does."""
        b = bare(n)
        if "_bt" in b:
            return b["_bt"] == "Int64"
        c = self.ctype(b)
        if c in W64:
            return True
        if c not in ("unsigned long", "long", "unsigned long long", "long long"):
            return False
        k, ks = b.get("kind"), kids(b)
        whole = "Int64"
        if k == "MemberExpr" or (k == "CXXOperatorCallExpr" and callee_name(b) in ("operator*", "operator[]")):
            return self.tp_or_none(b) == whole   # (a field, a key of a map, what an iterator stands at: as the Blueprint holds it)
        if k == "DeclRefExpr":
            return self.types.get(b.get("referencedDecl", {}).get("id")) == whole
        if k in CAST:
            return bool(ks) and self.ctype(ks[-1]) in ("unsigned long", "long", "unsigned long long", "long long", *W64) and self.is64(ks[-1])
        if k in ("BinaryOperator", "CompoundAssignOperator") and len(ks) == 2:
            op = b.get("opcode", "")
            if op in ("<<", ">>") or op.endswith("=") and op not in COMPARE:
                return self.is64(ks[0])
            if op == ",":
                return self.is64(ks[1])
            return op not in COMPARE and op not in ("&&", "||") and (self.is64(ks[0]) or self.is64(ks[1]))
        if k == "UnaryOperator" and ks:
            return self.is64(ks[0])
        if k == "ConditionalOperator" and len(ks) == 3:
            return self.is64(ks[1]) or self.is64(ks[2])
        if (k == "ArraySubscriptExpr" or (k == "CXXOperatorCallExpr" and callee_name(b) == "operator[]")) and len(ks) >= 2:
            return self.tp_or_none(ks[-2]) == f"List<{whole}>"
        if k == "CXXMemberCallExpr" and callee(b).get("name") in ("front", "back", "at") and kids(callee(b)):
            return self.tp_or_none(kids(callee(b))[0]) == f"List<{whole}>"
        return False

    # What C++ computes with 64 bits, the language's Int64 nodes compute (kern/logic/nodes.h, PureNodes::int64_*).
    INT64 = {"w64_add": "int64_add", "w64_subtract": "int64_sub", "w64_multiply": "int64_mul", "w64_bit_and": "int64_and", "w64_bit_or": "int64_or",
             "w64_bit_xor": "int64_xor", "w64_bit_not": "int64_not", "w64_divide": "int64_div_unsigned", "w64_divide_signed": "int64_div",
             "w64_remainder": "int64_rem_unsigned", "w64_remainder_signed": "int64_rem", "w64_shift_left": "int64_shift_left",
             "w64_shift_right": "int64_shift_right_unsigned", "w64_shift_right_signed": "int64_shift_right", "w64_equal": "int64_equal",
             "w64_less": "int64_less_unsigned", "w64_less_signed": "int64_less", "w64_to_number": "int64_to_float_unsigned",
             "w64_to_signed_number": "int64_to_float"}

    def w64(self, name, *texts):
        """What C++ computes with 64 bits: an Int64 node."""
        if name == "w64_negate":
            return f"int64_sub(Int64{{0x0}}, {texts[0]})"
        if name == "w64_is_zero":
            return f"int64_equal({texts[0]}, Int64{{0x0}})"
        if name == "w64_from_number":
            return f"int64_of_float({texts[0]})"
        return f"{self.INT64[name]}({', '.join(texts)})"

    def to64(self, text, have):
        """A number as 64 bits: a whole number as it is (a negative one in two's complement), another one cut off."""
        if have in self.u.enum_values:
            text, have = self.conv(text, have, "Int"), "Int"
        if have == "Float":
            text = f"kern::formula([](Float x) -> Float {{ return kern::math::trunc(x); }}, {text})"
        elif re.fullmatch(r"-?\d+", text):
            return f"Int64{{{int(text) % (1 << 64):#x}}}"
        elif have in ("Int", "Bool"):
            return f"int64_of_int({self.conv(text, have, 'Int')})"   # (a whole number as it is: one below 0 in two's complement)
        return self.w64("w64_from_number", text)

    def ex64(self, n):
        lit = bare(n)
        while lit.get("kind") in CAST and kids(lit):
            lit = bare(kids(lit)[-1])
        if lit.get("kind") == "IntegerLiteral":   # (a number written out: its bits, whatever its size)
            return f"Int64{{{int(lit['value']) % (1 << 64):#x}}}"
        text, have = self.value(n)
        return text if have == self.whole64() else self.to64(text, have)

    def from64(self, text, source, target, want):
        """64 bits as a value of another type, as C++ converts: `source` and `target` are the C++ types."""
        whole = self.whole64()
        signed = source in ("whole64s", "long", "long long")
        if want == "Bool":
            return f"logic_not({self.w64('w64_is_zero', text)})"
        if want == "Float":
            return self.w64("w64_to_signed_number" if signed else "w64_to_number", text)
        if want == "Int" or want in self.u.enum_values:
            if Const.BITS.get(target, 64) >= 64 or target in W64:
                got = f"int64_to_int({text})"   # (a number to count with: read with its sign)
            else:
                low = f"kern::to<Float>(int64_low({text}))"
                cut = self.narrow("x", "unsigned int", target, None)
                got = f"int64_low({text})" if cut == "x" else f"kern::to<Int>(kern::formula([](Float x) -> Float {{ return {cut}; }}, {low}))"
            return got if want == "Int" else self.conv(got, "Int", want)
        raise Wait(f"a 64-bit number where a {want} is needed")

    def value64(self, b):
        """(text, Whole64) of a computation with 64-bit bit patterns."""
        whole = self.whole64()
        k, ks = b.get("kind"), kids(b)
        signed = self.ctype(b) in ("whole64s", "long", "long long")
        b["_w64"] = True   # (said to the test: this is computed around 2^64 - see TestWriter.guard)
        if k == "IntegerLiteral":
            return f"Int64{{{int(b['value']) % (1 << 64):#x}}}", whole
        if k in CAST:
            return self.ex64(ks[-1]), whole
        if k == "InitListExpr":   # (u64{x}, u64{})
            return (self.ex64(ks[0]) if ks else "Int64{0x0}"), whole
        if k == "BinaryOperator":
            op = b["opcode"]
            x, y = ks
            if op in ("<<", ">>"):
                name = "w64_shift_left" if op == "<<" else "w64_shift_right_signed" if signed else "w64_shift_right"
                return self.w64(name, self.ex64(x), self.ex(y, "Int") if not self.is64(y) else self.from64(self.ex64(y), "whole64u", "unsigned int", "Int")), whole
            name = {"+": "add", "-": "subtract", "*": "multiply", "&": "bit_and", "|": "bit_or", "^": "bit_xor",
                    "/": "divide_signed" if signed else "divide", "%": "remainder_signed" if signed else "remainder"}[op]
            return self.w64("w64_" + name, self.ex64(x), self.ex64(y)), whole
        if k == "UnaryOperator":
            x = self.ex64(ks[0])
            return (x if b["opcode"] == "+" else self.w64("w64_bit_not" if b["opcode"] == "~" else "w64_negate", x)), whole
        if k == "ConditionalOperator":
            c = self.bx(ks[0])
            return f"logic_select<{whole}>({c}, {self.sometimes(lambda: self.ex64(ks[1]))}, {self.sometimes(lambda: self.ex64(ks[2]))})", whole
        raise Wait(f"a 64-bit number written as {k}")

    def once(self, n, leaves):
        """A part of a formula that the formula names several times: a variable or a number itself, anything
        else as a value of its own (computed once)."""
        b = self.unwrap(n)
        while b.get("kind") in CAST and kids(b) and self.ctype(b) == self.ctype(kids(b)[-1]):
            b = self.unwrap(kids(b)[-1])
        if b.get("kind") in ("DeclRefExpr", "IntegerLiteral"):
            return self.fx(n, leaves)
        return self.leaf(None, leaves, self.ex(n, "Float"))

    def never_negative(self, n):
        """Is this unsigned 64-bit value one that never reads negative: a list's length, a whole number
        written out, a smaller unsigned number made wider?"""
        b = self.unwrap(n)
        if b.get("kind") == "IntegerLiteral":
            return True
        if b.get("kind") == "CXXMemberCallExpr" and callee(b).get("name") in ("size", "length", "count"):
            return True
        if b.get("kind") in CAST and kids(b) and self.ctype(kids(b)[-1]) in ("unsigned int", "unsigned short", "unsigned char", "bool"):
            return True
        return False

    def narrow(self, text, have, want, inner):
        """A whole number converted to a C++ type that holds less of it: what that type keeps."""
        if have == want or want not in NUMBER or NUMBER[want] != "Int" or NUMBER.get(have) == "Bool":
            return text
        if inner is not None and self.known_positive(inner) and int(bare(inner)["value"]) < {"unsigned char": 256, "unsigned short": 65536}.get(want, 2 ** 31):
            return text
        size = {"unsigned char": 8, "signed char": 8, "char": 8, "unsigned short": 16, "short": 16, "unsigned int": 32, "int": 32}
        if want not in size:   # (64 bit: holds every whole number a Blueprint has; an unsigned one is held read with its sign)
            return text
        if have in size and size[have] < size[want] and (have in UNSIGNED or want not in UNSIGNED):
            return text   # (everything of the smaller type fits)
        if have in size and size[have] == size[want] and (have in UNSIGNED) == (want in UNSIGNED):
            return text
        if want == "unsigned int":
            return f"kern::math::wrap32({text})"
        if want == "int":
            return f"kern::math::wrap32s({text})"
        if want == "unsigned char":
            return f"kern::math::band({text}, 255.0)"
        if want == "unsigned short":
            return f"kern::math::band({text}, 65535.0)"
        if want in ("short", "signed char"):
            # (the low bits, read with their sign - two's complement, as C++20 converts: (v ^ 0x8000) - 0x8000)
            top = 32768.0 if want == "short" else 128.0
            return f"(kern::math::bxor(kern::math::band({text}, {flit(top * 2 - 1)}), {flit(top)}) - {flit(top)})"
        raise Wait(f"a whole number cut to {want}")

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
        d = self.engine_decl(ref.get("id"))
        if d is not None:
            return self.engine_value(d, None, kids(b)[1:])
        if self.own(ref.get("id")) and ref.get("id") in self.tree.body:
            got = self.invoke(u.function(ref["id"]), kids(b)[1:])
            if got[0] is None:
                raise Wait("a call without a result as a value")
            return got
        if Const(u).limit(b) is not None:
            t = self.tp(b)
            return Const(u).literal(b, t), t   # (std::numeric_limits<T>::max(): the number it is)
        vec = [self.is_vec(x) for x in a]
        if name == "isfinite" and len(a) == 1 and self.numeric(a[0]):
            x = self.ex(a[0], "Float")
            return f'math_compare<"==">(kern::formula([](Float x) -> Float {{ return x - x; }}, {x}), 0.0)', "Bool"
        if not self.own(ref.get("id")):
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
                    return (f"kern::formula([](Float ax, Float ay, Float bx, Float by) -> Float {{ return kern::math::single(kern::math::single(ax * by) - kern::math::single(ay * bx)); }}, "
                            f"vector_break({x}).x, vector_break({x}).y, vector_break({y}).x, vector_break({y}).y)"), "Float"
                return f"vector_cross({self.ex(a[0], 'Vector')}, {self.ex(a[1], 'Vector')})", "Vector"
            if name == "lerp" and len(a) == 3 and vec[:2] == [True, True]:
                return f"vector_lerp({self.ex(a[0], 'Vector')}, {self.ex(a[1], 'Vector')}, {self.ex(a[2], 'Float')})", "Vector"
            if name == "normalize" and vec and vec[0]:
                v = self.ex(a[0], "Vector")
                fb = self.ex(a[1], "Vector") if len(a) > 1 else "Vector{0.0, 0.0, 1.0}"
                return f'logic_select<Vector>(math_compare<">">(vector_length({v}), {flit(struct.unpack("f", struct.pack("f", 1e-9))[0])}), vector_normalize({v}), {fb})', "Vector"
            if name == "finite" and vec == [True]:
                v = self.ex(a[0], "Vector")
                return (f'math_compare<"==">(kern::formula([](Float x, Float y, Float z) -> Float {{ return (x - x) + (y - y) + (z - z); }}, '
                        f"vector_break({v}).x, vector_break({v}).y, vector_break({v}).z), 0.0)"), "Bool"
            if name == "move" and len(a) == 1 and self.is_box(a[0]):
                # what is moved out of an owned value is gone there
                lt = self.tp(a[0])
                return self.taken_out(a[0], lt), lt
            if name == "move" and len(a) == 1 and b.get("_taken") and not self.own_value(a[0]):
                # A value a constructor or an assignment takes out of a place that stays - a field of the
                # object, what a reference names: a list (std::vector) is empty there afterwards. Of anything
                # else C++ leaves open what stays.
                lt = self.tp(a[0])
                if not (lt.startswith("List<") and self.ctype(a[0]).startswith("std::vector<")):
                    raise Wait("what stays where a value was moved out of (std::move): C++ leaves it open, but for a list")
                return self.taken_out(a[0], lt), lt
            if name in ("move", "forward") and len(a) == 1:
                return self.value(a[0])
            if name == "make_unique" and u.object_of(qtype(b)):
                # an owned object of the engine made: its constructor of these arguments (one of them by their count)
                q = u.object_of(qtype(b))
                fit = []
                for d in u.api().makers(q):
                    ps = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
                    if len([p for p in ps if not kids(p)]) <= len(a) <= len(ps):
                        fit.append(d)
                if len(fit) != 1:
                    raise Wait(f"std::make_unique of a {q.split('::')[-1]}: " + ("no constructor of it is a node" if not fit else "which of its constructors"))
                return self.made(fit[0], a), "Int"
            if name == "make_unique" and self.is_box(b):
                lt = self.tp(b)
                if lt[5:-1] not in u.struct_fields:
                    raise Wait("an owned value that is not an object")
                return f'list_make<{lt[5:-1]}, "1">({self.make(lt[5:-1], a)})', lt
            if name == "make_pair" and len(a) == 2:
                t = self.tp(b)
                fields = u.struct_fields[t]
                return f"struct_make<{t}>({self.ex(a[0], fields[0][1])}, {self.ex(a[1], fields[1][1])})", t
            if name == "to_string" and len(a) == 1:
                # std::to_string: what sprintf gives with the format of its type (C++ [string.conversions])
                c = self.ctype(a[0])
                fmt = {"int": "%d", "short": "%d", "signed char": "%d", "char": "%d", "unsigned int": "%u", "unsigned short": "%u", "unsigned char": "%u",
                       "long": "%lld", "long long": "%lld", "unsigned long": "%llu", "unsigned long long": "%llu", "float": "%f", "double": "%f"}.get(c)
                if fmt is None:
                    raise Wait(f"std::to_string of a {c}")
                return f'text_format<"{fmt}">(-1, {self.ex(a[0], "Float" if fmt == "%f" else "Int")})', "Text"
            if name in ("snprintf", "printf", "sprintf", "to_string", "strtod", "atof", "atoi", "strcmp", "strlen", "strncmp", "strchr", "strstr"):
                raise Wait("text formatting and parsing (snprintf, to_string, atof ...)")
            if name in ("find", "find_if", "find_if_not") and len(a) == 3 and self.is_iter(b):
                return self.first_match(name, a), "Int"
            if name in ("min_element", "max_element") and len(a) in (2, 3) and self.is_iter(b):
                return self.extreme(name, a), "Int"
            if name in ("sort", "stable_sort"):
                raise Wait(ORDER_WAIT.format(name))
            if name in ("sort", "stable_sort", "remove_if", "find", "find_if", "unique", "any_of", "all_of", "count_if", "swap", "fill", "reverse", "accumulate", "lower_bound", "upper_bound"):
                raise Wait(f"algorithm: std::{name}")
            if name in ("memcpy", "memset", "memcmp"):
                raise Wait("raw memory (memcpy / memset)")
            raise Wait(f"call: {name}()")
        if defaulted:
            raise Wait("a call with default arguments")
        got = self.invoke(u.function(ref["id"]), a)
        if got[0] is None:
            raise Wait("a call without a result as a value")
        return got

    def operator(self, b):
        if not b.get("_op") and self.is_local_call(b):
            got = self.local_call(b)
            if got[0] is None:
                raise Wait("a call that gives nothing, used as a value")
            return got
        name = b.get("_op") or callee_name(b)
        ops = kids(b)[1:]
        ref = callee(b).get("referencedDecl", {}) if not b.get("_op") else {}
        if self.is_slot(b):
            return self.value(self.held_by(b))
        if ops and not b.get("_op") and name in ("operator->", "operator*") and len(ops) == 1 and self.u.object_of(qtype(ops[0])):
            return self.ex(ops[0], "Int"), "Int"   # (an owned object of the engine: its handle)
        if ops and not b.get("_op") and self.is_iter(ops[0]):
            # an iterator is the place it stands at
            if name in ("operator*", "operator->") and len(ops) == 1:
                return self.value(self.entry_of(b))
            if name in ("operator==", "operator!=") and len(ops) == 2:
                x, y = self.iter_node(ops[0]), self.iter_node(ops[1])
                for f_, e_ in ((x, y), (y, x)):   # (m.find(key) != m.end(): is there a row of the key?)
                    if (f_.get("kind") == e_.get("kind") == "CXXMemberCallExpr" and callee(f_).get("name") == "find" and callee(e_).get("name") in ("end", "cend")
                            and kids(callee(f_)) and kids(callee(e_)) and self.keyed(kids(callee(f_))[0]) and self.same_list(kids(callee(f_))[0], kids(callee(e_))[0])):
                        fb = kids(callee(f_))[0]
                        lt, row, kt, _ = self.rows(fb)
                        there = self.found(self.keyed(fb)[0], row, self.ex(fb, lt), self.ex(kids(f_)[1], kt))
                        return (there if name == "operator!=" else f"logic_not({there})"), "Bool"
                same = f'math_compare<"==">(kern::to<Float>({self.ex(ops[0], "Int")}), kern::to<Float>({self.ex(ops[1], "Int")}))'
                return (same if name == "operator==" else f"logic_not({same})"), "Bool"
            if name in ("operator<", "operator>", "operator<=", "operator>=") and len(ops) == 2:
                return f'math_compare<"{name[8:]}">(kern::to<Float>({self.ex(ops[0], "Int")}), kern::to<Float>({self.ex(ops[1], "Int")}))', "Bool"
            if name in ("operator+", "operator-") and len(ops) == 2:
                return self.whole_math(name[-1], self.ex(ops[0], "Int"), self.ex(ops[1], "Int")), "Int"
            if name in ("operator++", "operator--"):
                var = self.stepped_place(ops[0])
                if len(ops) == 2:   # (it++: what it was before)
                    old = self.temp("was", "Int")
                    self.hoist([f"{old} = {var};"] + self.effect(b))
                    return old, "Int"
                self.hoist(self.effect(b))
                return var, "Int"
        if self.own(ref.get("id")) and ref.get("id") in self.tree.body:   # an operator the program itself defines
            d = self.tree.body[ref["id"]]
            g = self.u.function(ref["id"])
            got = self.invoke(g, ops[1:], obj=ops[0]) if d.get("kind") == "CXXMethodDecl" else self.invoke(g, ops)
            if got[0] is None:
                raise Wait("an operator without a result as a value")
            return got
        if ops and self.is_box(ops[0]):
            lt = self.tp(ops[0])
            it = lt[5:-1]
            if name in ("operator->", "operator*") and len(ops) == 1:
                return f"list_get<{it}>({self.ex(ops[0], lt)}, 0)", it
            if name in ("operator==", "operator!=") and len(ops) == 2 and self.nothing(ops[1]):
                empty = f'math_compare<"==">(kern::to<Float>(list_length<{it}>({self.ex(ops[0], lt)})), 0.0)'
                return (empty if name == "operator==" else f"logic_not({empty})"), "Bool"
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
                div = "kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, "
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
            got = self.spot(b)
            if got is not None:
                return got
            self.f.indexes = True
            return f"list_get<{t0[5:-1]}>({self.ex(ops[0], t0)}, {self.ex(ops[1], 'Int')})", t0[5:-1]
        if t0 == "Text" and name == "operator[]" and len(ops) == 2:
            return self.char_of(self.ex(ops[0], "Text"), self.ex(ops[1], "Int"), qtype(b)), "Int"   # (s[i]: its byte - s[s.size()] is 0)
        if t0 == "Text":
            if name == "operator+" and self.tp(ops[1]) == "Text":
                return f'text_append({self.ex(ops[0], "Text")}, {self.ex(ops[1], "Text")}, "")', "Text"
            if name in ("operator==", "operator!=") and self.tp(ops[1]) == "Text":
                r = f'text_equal({self.ex(ops[0], "Text")}, {self.ex(ops[1], "Text")})'
                return (r if name == "operator==" else f"logic_not({r})"), "Bool"
        if len(ops) == 2 and self.tp(ops[1]) == "Text" and name in ("operator==", "operator!=") and t0 == "Text":
            pass
        raise Wait(f"the operator {name[8:]} of {t0}")

    def owned_object(self, n):
        """The engine's class of an owned object of the engine (std::unique_ptr) this names - else None."""
        if not re.match(r"(?:const\s+)?std::unique_ptr<", self.u.clean(qtype(n))):
            return None
        return self.u.object_of(qtype(n))

    def maker_of(self, n):
        """The constructor of the engine's class a value is made by (CXXConstructExpr) that is a node
        <class>.make - else None."""
        n = self.unwrap(n)
        while n.get("kind") in ("ExprWithCleanups", "CXXBindTemporaryExpr", "MaterializeTemporaryExpr") and len(kids(n)) == 1:
            n = self.unwrap(kids(n)[0])
        if n.get("kind") not in ("CXXConstructExpr", "CXXTemporaryObjectExpr"):
            return None
        q = self.u.object_of(qtype(n))
        if q is None:
            return None
        makers = self.u.api().makers(q)
        d = constructor_of(self.tree, n)
        if d is None and not [x for x in kids(n) if x.get("kind") != "CXXDefaultArgExpr"]:
            return makers[0] if len(makers) == 1 and not [p for p in kids(makers[0]) if p.get("kind") == "ParmVarDecl"] else None
        return d if d in makers else None

    def made(self, d, args):
        """An object of the engine made by its constructor `d` (the node <class>.make): its handle. A step."""
        lines, text, _ = self.engine(d, None, args)
        self.hoist(lines)
        return text

    def construct(self, b):
        u = self.u
        for x in kids(b):
            self.moved_into(x, b.get("ctorType", {}).get("qualType", ""))
        if u.object_of(qtype(b)):
            a = [x for x in kids(b) if x.get("kind") != "CXXDefaultArgExpr"]
            if len(a) == 1 and u.object_of(qtype(a[0])) and self.maker_of(b) is None:
                return self.ex(a[0], "Int"), "Int"   # (a handle handed on)
            d = self.maker_of(b)
            if d is None:
                raise Wait("an object of the engine made by what is not a constructor of it")
            return self.made(d, a), "Int"
        t = self.tp(b)
        a = [x for x in kids(b) if x.get("kind") != "CXXDefaultArgExpr"]
        if self.is_box(b):
            it = t[5:-1]
            if not a or self.nothing(a[0]):
                return t + "{}", t
            if len(a) == 1 and self.is_box(a[0]):
                return self.ex(a[0], t), t
            if len(a) == 1:
                return f'list_make<{it}, "1">({self.ex(a[0], it)})', t
            raise Wait("an owned or optional value made of something else")
        if t in u.struct_fields and self.ctype(b).startswith("std::pair<") and len(a) == 2:
            fields = u.struct_fields[t]
            return f"struct_make<{t}>({self.ex(a[0], fields[0][1])}, {self.ex(a[1], fields[1][1])})", t
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
                q = next((x for x, n_ in u.structs.items() if n_ == t), None)
                rec = self.tree.records.get(q, {})
                want = b.get("ctorType", {}).get("qualType")
                for c in kids(rec):   # the constructor the class itself has for these arguments
                    if c.get("kind") == "CXXConstructorDecl" and not c.get("isImplicit") and qtype(c) == want or (c.get("kind") == "CXXConstructorDecl" and not c.get("isImplicit") and c.get("type", {}).get("qualType") == want):
                        if c["id"] not in self.tree.body:
                            raise Wait(f"a {t} made by a constructor that is defined in another file")
                        return self.invoke(u.function(c["id"]), kids(b))
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
            said = self.written_array(b, t) if b.get("kind") == "InitListExpr" else None
            if said is not None:
                # An array of C++ written out, as a value: every entry of it - what is not written is zero.
                # Up to 16 entries it is one Make List; a longer one is a place of that length, set to
                # what is written.
                vals, fixed = said
                if fixed <= 16:
                    return self.made_list(vals + [u.zero(it)] * (fixed - len(vals)), t), t
                made = self.temp("made", t, fixed)
                self.hoist([f"{made} = {self.made_list(vals, t)};"])
                return made, t
            if not a:
                return t + "{}", t
            if b.get("kind") == "InitListExpr" or (len(a) == 1 and bare(a[0]).get("kind") in ("CXXStdInitializerListExpr", "InitListExpr")):
                items = a if b.get("kind") == "InitListExpr" else kids(bare(a[0]) if bare(a[0]).get("kind") == "InitListExpr" else bare(kids(bare(a[0]))[0]))
                vals = [self.ex(x, it) for x in items]
                if len(vals) > 16:
                    raise Wait("a list made of more than 16 values")
                return f'list_make<{it}, "{len(vals)}">({", ".join(vals)})', t
            if len(a) == 1 and self.tp_or_none(a[0]) == t:
                return self.ex(a[0], t), t
            if len(a) == 2 and re.match(r"(?:kern::)?Span<", self.ctype(b)):
                # a view of a list, Span(p, n): with p its first entry (v.data(), an array) and n its length, the whole list
                lst = bare(a[0])
                while lst.get("kind") == "ImplicitCastExpr" and lst.get("castKind") in ("NoOp", "LValueToRValue") and kids(lst):
                    lst = bare(kids(lst)[0])
                if lst.get("kind") == "ImplicitCastExpr" and lst.get("castKind") == "ArrayToPointerDecay" and kids(lst):
                    lst = bare(kids(lst)[0])
                elif lst.get("kind") == "CXXMemberCallExpr" and callee(lst).get("name") == "data" and kids(callee(lst)) and len(kids(lst)) == 1:
                    lst = kids(callee(lst))[0]
                else:
                    lst = None
                if lst is not None and self.tp_or_none(lst) == t:
                    n = self.ex(a[1], "Int")
                    if n in (f"list_length<{it}>({self.ex(lst, t)})", str(u.fixed(qtype(lst)))):
                        return self.ex(lst, t), t
                raise Wait("a view (Span) of a part of a list")
            if len(a) in (1, 2) and self.tp_or_none(a[0]) in ("Int", "Float") and (len(a) == 1 or self.tp_or_none(a[1]) is not None):
                # std::vector<T>(n) and (n, x): n entries, each T's zero or x
                made = self.temp("made", t)
                self.hoist([f"{made} = {t}{{}};", f"list_resize({made}, {self.ex(a[0], 'Int')}, {self.ex(a[1], it) if len(a) == 2 else u.zero(it)});"])
                return made, t
            if len(a) == 2:
                # std::vector<T>(first, last): the entries of a list from one place to another
                (s0, first), (s1, last) = self.place(a[0]), self.place(a[1])
                st = self.tp(s0)
                if not self.same_list(s0, s1) or st != t:
                    raise Wait("a list made of a range of another type")
                if self.whole_list(a[0], a[1]):
                    return self.ex(s0, st), t   # (the whole list: a copy of it)
                made, src, i0, i1 = self.temp("made", t), self.temp("from", st), self.temp("first", "Int"), self.temp("last", "Int")
                self.hoist([f"{made} = {t}{{}};", f"{src} = {self.ex(s0, st)};", f"{i0} = {first};", f"{i1} = {self.whole_math('-', last, '1')};",
                            f"list_reserve({made}, {self.whole_math('+', self.whole_math('-', i1, i0), '1')});"]
                           + self.count_loop(i0, i1, lambda i: [f"list_add({made}, list_get<{it}>({src}, {i}));"]))
                return made, t
            raise Wait("a list made by a constructor")
        raise Wait(f"a {t} made here")

    def member_call(self, b):
        m = callee(b)
        base = kids(m)[0] if kids(m) else None
        name = m.get("name")
        if self.actor_get(b) is not None:
            return self.get_on(b, self.actor_get(b))
        if self.actor_value(b) is not None:
            return self.call_on(b, self.actor_value(b), "Float" if name == "number" else "Bool")
        d = self.engine_decl(m.get("referencedMemberDecl"))
        if d is not None:
            return self.engine_value(d, base, kids(b)[1:])
        if self.own(m.get("referencedMemberDecl")):
            if m.get("isArrow") and not self.is_this(base) and self.box(base) is None:
                raise Wait("a pointer")
            got = self.invoke(self.u.function(m["referencedMemberDecl"]), kids(b)[1:], obj=base)
            if got[0] is None:
                raise Wait("a call without a result as a value")
            return got
        if base is None or self.is_this(base):
            raise Wait("the state of its object (a method)")
        if self.is_box(base):
            lt = self.tp(base)
            it = lt[5:-1]
            if name in ("operator bool", "has_value"):
                return f'math_compare<">">(kern::to<Float>(list_length<{it}>({self.ex(base, lt)})), 0.0)', "Bool"
            if name == "value":
                return f"list_get<{it}>({self.ex(base, lt)}, 0)", it
            if name == "value_or" and len(kids(b)) == 2:
                lst = self.ex(base, lt)
                return f'logic_select<{it}>(math_compare<">">(kern::to<Float>(list_length<{it}>({lst})), 0.0), list_get<{it}>({lst}, 0), {self.ex(kids(b)[1], it)})', it
            raise Wait("a pointer" if name in ("get", "release") else f"{name}() of an owned or optional value")
        bt = self.tp(base)
        a = kids(b)[1:]
        if name == "operator bool" and not a and self.ctype(base) == "std::_Bit_reference":
            return self.value(base)   # (an entry of a std::vector<bool>, read)
        if name == "operator bool" and not a and self.u.object_of(qtype(base)):
            # (what holds an object of the engine - std::unique_ptr - as a Bool: is there one; none is the handle 0)
            return f'math_compare<"!=">(kern::to<Float>({self.ex(base, "Int")}), 0.0)', "Bool"
        if bt.startswith("List<") and name in ("begin", "cbegin", "end", "cend") and not a and self.is_iter(b):
            # an iterator is the place it stands at: the first entry's is 0, the end the list's length
            return ("0" if "begin" in name else f"list_length<{bt[5:-1]}>({self.ex(base, bt)})"), "Int"
        if bt.startswith(("Map<", "Set<")):
            got = self.keyed_call(b, base, name, a)
            if got is not None:
                return got
            raise Wait(f"{'map' if bt.startswith('Map<') else 'set'}: {name}()")
        if bt.startswith("List<"):
            it = bt[5:-1]
            lst = self.ex(base, bt)
            if name == "size" and not a:
                return f"list_length<{it}>({lst})", "Int"
            if name == "empty" and not a:
                return f'math_compare<"==">(kern::to<Float>(list_length<{it}>({lst})), 0.0)', "Bool"
            if name in ("at", "front", "back"):
                got = self.spot(b)
                if got is not None:
                    return got
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
            if name.startswith("operator ") and not a and self.tp_or_none(b) == "Text":
                # (a std::string as a std::string_view, and the like: the same text)
                return self.ex(base, "Text"), "Text"
            raise Wait(f"text: {name}()")
        raise Wait(f"call: {name}() of a {bt}")


# ---- the engine as nodes -----------------------------------------------------------------------------
# Not the engine a game calls: the Blueprint language itself, the engine's own helpers.
ENGINE_SKIP = ("kern::logic::", "kern::json::", "kern::detail::", "kern::dm::", "kern::game::nodes::")
# Functions that give something and change something (the rule below would call them values).
ENGINE_STEPS = {"kern::jobs::submit", "kern::jobs::take"}
ENGINE_PIN = {"Bool": "boolean", "Int": "integer", "Float": "number", "Vector": "vector", "Text": "text"}


def snake(name):
    return re.sub(r"(?<=[a-z0-9])([A-Z])", r"_\1", name).lower()


class Api:
    """The engine's functions as nodes, from a compiler's tree that holds the engine's headers. (The
    converter asks the same object: a call in a game's source and the node it becomes have one name.)"""

    def __init__(self, tree, unit):
        self.tree, self.u = tree, unit
        self.by_class = {}   # a class or a namespace -> its functions that are declared only, in their order
        self.implicit_makers = {}   # a class of the engine without a constructor of its own -> the one C++ gives it

    def opaque(self, q):
        """Is this class an object of the engine with a life of its own (a handle in a Blueprint)?"""
        return self.u.engine_object(q)

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
        if bt in ENGINE_PIN:
            return ENGINE_PIN[bt], ""
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
        if bt in ENGINE_PIN:
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
        """Is this a function only the engine can do: declared in a header, defined in the engine - or a
        method of an object of the engine (a class with a life of its own: Unit.object_of), wherever it is
        defined? What such an object holds is the engine's: a Blueprint reaches it through its node only."""
        if d.get("kind") not in ("FunctionDecl", "CXXMethodDecl") or d.get("isImplicit") or d.get("explicitlyDeleted"):
            return False
        if d["id"] in self.tree.body and not self.of_object(d):
            return False
        scope = self.tree.scope_of.get(d["id"], "")
        if not scope.startswith(ENGINE + "::") or any((scope + "::").startswith(s) or scope.startswith(s) for s in ENGINE_SKIP):
            return False
        if d.get("previousDecl") or d["id"] in self.tree.template_of or not d.get("name") or d["name"].startswith("operator") or "(anonymous" in scope:
            return False
        return not (d.get("kind") == "CXXMethodDecl" and not self.public(d))

    def public(self, d):
        """Is the method `d` public in its class? (The compiler's tree says it by the access specifiers between
        the members, not at a member: a class begins private, a struct public.)"""
        rec = self.tree.records.get(self.tree.owner.get(d["id"], ""))
        if rec is None:
            return d.get("access", "public") == "public"
        access = "private" if rec.get("tagUsed") == "class" else "public"
        for c in kids(rec):
            if c.get("kind") == "AccessSpecDecl":
                access = c.get("access", access)
            elif c.get("id") == d["id"]:
                return access == "public"
        return d.get("access", "public") == "public"

    def of_object(self, d):
        """Is `d` a method of a class of the engine that is an object (not plain data a Blueprint holds)?"""
        owner = self.tree.owner.get(d["id"])
        if d.get("kind") != "CXXMethodDecl" or not owner or not owner.startswith(ENGINE + "::"):
            return False
        return self.u.object_of(owner) is not None

    def siblings(self, d):
        home = self.home(d)
        if home not in self.by_class:
            self.by_class[home] = [x for x in self.tree.decl.values() if self.declared_only(x) and self.home(x) == home]
        return self.by_class[home]

    def type_of(self, d):
        """The node's type: engine.<class or namespace>.<function> (an overload: _2, _3 in the header's order)."""
        home = self.home(d)
        group = snake(home.split("::")[-1]) if home != ENGINE else ""
        same = [x for x in self.siblings(d) if x["name"] == d["name"]]
        nth = same.index(d) + 1 if d in same else 1
        return "engine." + (group + "." if group else "") + d["name"] + (f"_{nth}" if nth > 1 else "")

    # ---- objects of the engine: made and ended ----
    def makers(self, q):
        """The constructors of the engine's class `q` that make an object of it - a node `<class>.make`
        each, in the order of the header: the public ones, not deleted, no copy or move (a handle is
        not copied: the engine's object is one), of a class that is an object of the engine and not
        abstract. (None for any other class.)"""
        rec = self.tree.records.get(q)
        if rec is None or not q.startswith(ENGINE + "::") or not self.opaque(q) or any((q + "::").startswith(s) for s in ENGINE_SKIP):
            return []
        if (rec.get("definitionData") or {}).get("isAbstract") or any(c.get("pure") for c in kids(rec) if c.get("kind") == "CXXMethodDecl"):
            return []
        if any(c.get("kind") == "CXXDestructorDecl" and (c.get("explicitlyDeleted") or not (c.get("isImplicit") or self.public(c))) for c in kids(rec)):
            return []   # (what cannot be ended is not made here)
        name = q.split("::")[-1]
        own = re.compile(r"\s*void\s*\(\s*(const\s+)?(?:[\w:]+::)?" + re.escape(name) + r"\s*&{1,2}\s*\)\s*(noexcept)?\s*")
        declared = [c for c in kids(rec) if c.get("kind") == "CXXConstructorDecl" and not c.get("isImplicit")]
        if any(c.get("kind") == "FieldDecl" and qtype(c).strip().endswith("&") for c in kids(rec)):
            if not declared:
                return []   # (a reference in it: C++ gives it no constructor - it is made by one it declares)
        if not declared and ((rec.get("definitionData") or {}).get("defaultCtor") or {}).get("exists"):
            # (a class that declares no constructor: the one C++ gives it, without parameters)
            if q not in self.implicit_makers:
                self.implicit_makers[q] = {"kind": "CXXConstructorDecl", "id": rec["id"] + ":make", "name": name, "type": {"qualType": "void ()"}, "inner": []}
                self.tree.owner[rec["id"] + ":make"] = q
            return [self.implicit_makers[q]]
        return [c for c in declared if not c.get("explicitlyDeleted") and self.public(c) and not own.fullmatch(qtype(c))]

    def maker(self, d):
        """The node that makes an object of the engine by the constructor `d`: its parameters are the
        node's inputs, the object's handle its result. A step - the host keeps the object until it is
        released (<class>.release)."""
        q = self.tree.owner.get(d["id"])
        makers = self.makers(q)
        if d not in makers:
            raise Wait("a constructor that makes no object of the engine")
        group = snake(q.split("::")[-1])
        nth = makers.index(d) + 1
        ins, params, taken = [], [], {"exec", "then", "result"}
        for i, p in enumerate(x for x in kids(d) if x.get("kind") == "ParmVarDecl"):
            bt, writes, handle = self.handed(qtype(p))
            if writes:
                raise Wait("a constructor that writes through a parameter")
            name = re.sub(r"\W", "_", p.get("name") or f"value{i + 1}")
            if name in CPP_WORDS or name in taken:
                name += "_" + str(i + 1)
            taken.add(name)
            ins.append((name, bt, handle))
            params.append((name, None))
        for _, bt, _ in ins:
            self.pin(bt)
            self.shape(bt)
        return {"type": f"engine.{group}.make" + (f"_{nth}" if nth > 1 else ""), "cpp": q + "::" + q.split("::")[-1], "label": "Make " + q.split("::")[-1],
                "category": "Engine: " + q.split("::")[-1], "help": self.u.comment_of(d) or f"Makes a {q.split('::')[-1]} of the engine: its handle (the host keeps it until it is released).",
                "value": False, "ins": ins, "outs": [("result", "Int", q)], "params": params, "decl": d, "role": "make", "class": q}

    def releaser(self, q):
        """The node that ends an object of the engine that was made (its destructor): <class>.release."""
        if not self.makers(q):
            raise Wait("an object of the engine that is not made here")
        rec = self.tree.records[q]
        group = snake(q.split("::")[-1])
        return {"type": f"engine.{group}.release", "cpp": q + "::~" + q.split("::")[-1], "label": "Release " + q.split("::")[-1],
                "category": "Engine: " + q.split("::")[-1], "help": f"Ends the {q.split('::')[-1]} (its destructor): the handle is none after it.",
                "value": False, "ins": [(group, "Int", q)], "outs": [], "params": [], "decl": rec, "role": "release", "class": q}

    def function(self, d):
        """The node of a declared function of the engine: a dict - or Wait, when its types have no place."""
        tree, u = self.tree, self.u
        if d.get("kind") == "CXXConstructorDecl":
            return self.maker(d)
        if d.get("variadic"):
            raise Wait("a variadic function")
        owner = tree.owner.get(d["id"])
        method = d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static"
        const = qtype(d).rstrip().endswith("const")
        ins, outs, taken = [], [], {"exec", "then", "result"}

        def name_of(want):
            name = re.sub(r"\W", "_", want) or "value"
            if name in CPP_WORDS or name in taken:
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
        changes = False   # (an object of the engine it is handed to change - Renderer&, physics::World*: a step)
        params = []   # per parameter of the C++ function: (its input pin or None, its output pin or None)
        for i, p in enumerate(x for x in kids(d) if x.get("kind") == "ParmVarDecl"):
            optional = self.optional_out(p)
            if optional is not None:
                name = name_of(p.get("name") or f"value{i + 1}")
                outs.append((name, optional, None))
                params.append((None, name))
                continue
            bt, writes, handle = self.handed(qtype(p))
            name = name_of(p.get("name") or f"value{i + 1}")
            ins.append((name, bt, handle))
            if writes:
                outs.append((name, bt, None))
            params.append((name, name if writes else None))
            if handle and re.search(r"(&|\*\s*(const)?)$", qtype(p).strip()) and not re.match(r"const\b", qtype(p).strip()):
                changes = True
        ret = qtype(d).split("(")[0].strip()
        if ret != "void":
            bt, _, handle = self.handed(ret)
            outs.insert(0, ("result", bt, handle))
        qualified = self.home(d) + "::" + d["name"]
        value = ret != "void" and len(outs) == 1 and (const or not method) and not changes and qualified not in ENGINE_STEPS
        for _, bt, _ in ins + outs:
            self.pin(bt)
            self.shape(bt)
        words = d["name"].replace("_", " ").title()
        return {"type": self.type_of(d), "cpp": qualified, "label": words, "category": "Engine: " + self.home(d).split("::")[-1].replace("kern", "Engine"),
                "help": u.comment_of(d), "value": value, "ins": ins, "outs": outs, "params": params, "decl": d}

    def optional_out(self, p):
        """A parameter that is a pointer to plain data the function writes through, null by default
        (`bool* changed = nullptr`, `std::string* error = nullptr`): what the function reports if it is
        asked to - a further output, the Blueprint type of what it writes; None for any other parameter.
        (The engine's convention: such a pointer only says where to put the report; what the function does
        is the same with it or without it.)"""
        raw = qtype(p).strip()
        if not re.search(r"\*\s*$", raw) or re.match(r"const\b", raw) or not kids(p):
            return None
        init = bare(kids(p)[-1])
        while init.get("kind") == "ImplicitCastExpr" and kids(init):
            init = bare(kids(init)[0])
        if init.get("kind") not in ("CXXNullPtrLiteralExpr", "GNUNullExpr"):
            return None
        inner = re.sub(r"\*\s*$", "", raw).strip()
        if "*" in inner or "&" in inner:
            return None
        q = None
        try:
            q = self.u.find_type(self.u.clean(inner))
        except Wait:
            pass
        if q is not None and q in self.tree.records and self.opaque(q):
            return None   # (an object of the engine that may be missing: a handle, 0 for none)
        return self.u.btype(inner)


# ---- the test: the Blueprint next to its source ------------------------------------------------------
WIDE_MATH = {"sin": "kern::math::sin", "cos": "kern::math::cos", "tan": "kern::math::tan", "asin": "kern::math::asin", "acos": "kern::math::acos", "atan": "kern::math::atan",
             "atan2": "kern::math::atan2", "exp": "kern::math::exp", "log": "kern::math::log", "pow": "kern::math::pow", "sqrt": "kern::math::sqrt", "fmod": "kern::math::mod",
             "hypot": "hypot_bp", "tanh": "tanh_bp", "sinh": "sinh_bp", "cosh": "cosh_bp", "asinh": "asinh_bp", "log2": "log2_bp", "log10": "log10_bp", "cbrt": "cbrt_bp", "ldexp": "ldexp_bp"}
# What a test needs where the source reads through a pointer into a list (Unit.pointed): the lists the test
# handed it, so that an entry through such a pointer is checked against its list as converted::at checks one -
# outside it the call is marked (C++ leaves open what it reads there). A pointer into nothing the test handed
# reads as the source reads it; null is marked.
POINTED_PRELUDE = """inline std::vector<std::pair<const unsigned char*, const unsigned char*>> converted_pointed_lists;
template <class T> const T& converted_pointed(const T* p, long long i) {
  const auto at = reinterpret_cast<std::uintptr_t>(p);
  bool held = false;
  for (const auto& [first, last] : converted_pointed_lists) {
    const auto a = reinterpret_cast<std::uintptr_t>(first), b = reinterpret_cast<std::uintptr_t>(last);
    if (at < a || at > b || (at - a) % sizeof(T) != 0) continue;
    held = true;
    const long long e = static_cast<long long>((at - a) / sizeof(T)) + i;
    if (e >= 0 && e < static_cast<long long>((b - a) / sizeof(T))) return reinterpret_cast<const T*>(first)[e];
    break;
  }
  if (p != nullptr && !held) return p[i];
  converted::open_read();
  static T none;
  none = T{};
  return none;
}"""
WIDE_PRELUDE = """// the numbers of the Blueprint language (../converted.h): a number is 64-bit, a vector's components 32-bit
using converted::wide::f32; using converted::wide::f64;
using converted::wide::u8; using converted::wide::u16; using converted::wide::u32; using converted::wide::u64;
using converted::wide::i8; using converted::wide::i16; using converted::wide::i32; using converted::wide::i64;
using converted::wide::Vec2; using converted::wide::Vec3; using converted::wide::Span;
using converted::wide::dot; using converted::wide::cross; using converted::wide::length; using converted::wide::length_sq;
using converted::wide::distance; using converted::wide::distance_sq; using converted::wide::finite; using converted::wide::lerp;
using converted::wide::vmin; using converted::wide::vmax; using converted::wide::normalize; using converted::wide::hex_color; using converted::wide::Span;
using converted::wide::hypot_bp; using converted::wide::ldexp_bp; using converted::wide::tanh_bp; using converted::wide::sinh_bp; using converted::wide::cosh_bp;
using converted::wide::asinh_bp; using converted::wide::log2_bp; using converted::wide::log10_bp; using converted::wide::cbrt_bp;"""




class Held:
    """What a test holds of the source (TestWriter.gather)."""

    def __init__(self):
        self.functions = {}     # id -> a function with its body
        self.records = set()    # the classes - the outermost ones compiled along
        self.enums = set()
        self.consts = {}
        self.unnamed = {}       # the enums without a name whose values the source names: id -> the declaration
        self.templates = {}
        self.freed = {}         # a method that does not use its object, of a class the Blueprint does not have -> (its name as a function, its head, where its body begins)
        self.foreign = collections.OrderedDict()   # what stands in other source files and not here: its place there -> the piece (TestWriter.exports), in their order
        self.wanted = set()     # the places of all pieces of other files' tests that are taken
        self.kept = set()       # methods declared in their class here whose body is a piece of another file
        self.declared = {}      # functions declared here whose body is a piece of another file: id -> the declaration
        self.engine = {}        # the engine's functions those pieces call: id -> the declaration here
        self.objects = set()    # the engine's classes with a life of their own those pieces hold


class TestWriter:
    """The test of one converted file (see tests/converted.h of the game). It holds the source of the
    file's converted functions once more, compiled with the numbers and the mathematics of the
    Blueprint language - classes and constants along with them - and calls each function there and
    in the Blueprint with the same arguments. What they give must be the same, bit for bit."""

    def __init__(self, unit, source_dir, flags):
        self.u = unit
        self.tree = unit.tree
        self.path = unit.tree.path
        self.source_dir = source_dir
        self.flags = flags
        self.names = {}        # a record (qualified) -> its name in the test
        self.helped = collections.OrderedDict()   # a record -> the lines of its helpers (None: being written)
        self.edits = {}        # a file -> the guards in it (see `guard`), in the order of the file
        self.enum_names = {}   # an enum (qualified) -> its name in the test
        self.object_enums = collections.defaultdict(list)   # a class of the engine the test has a stand-in of -> its enums the test needs
        self.made = {}         # a class the test makes by a constructor -> (the constructor, the line that makes it without)
        self.objects = {}      # a class of the engine the source holds objects of -> its stand-in's name in the test
        self.pointed = False   # the source reads through a pointer into a list the test hands it (see POINTED_PRELUDE)
        self.whole = None      # what the test holds (`setup`)
        self.state_info = None # of a part of the Blueprint of a state: its helpers' names (`state_helpers`) - or why there are none
        self.by_place = None   # the declarations of the tree by where they stand (`at`)
        # what the tests of other source files hand on (`exports`): a function's key -> the places of the
        # pieces its test holds, and the pieces by their places
        self.closures, self.pieces = unit.world.closures or {}, unit.world.pieces or {}
        self.more_engine, self.more_objects = {}, set()   # what those pieces call and hold of the engine

    # ---- the source, with a Blueprint's numbers ----
    QUOTED = re.compile(r'''(R"([^()\\\s]{0,16})\((?:.|\n)*?\)\2"|"(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])+'|//[^\n]*|/\*(?:.|\n)*?\*/)''')

    @classmethod
    def in_code(cls, text, change):
        """`change` applied to the code of a piece of C++ - not to its texts and comments."""
        out, at = [], 0
        for m in cls.QUOTED.finditer(text):
            out.append(change(text[at:m.start()]))
            out.append(m.group(0))
            at = m.end()
        return "".join(out) + change(text[at:])

    @classmethod
    def code_and_rest(cls, text):
        """The pieces of a text in turn: code, a text or comment, code, ..."""
        out, at = [], 0
        for m in cls.QUOTED.finditer(text):
            out += [text[at:m.start()], m.group(0)]
            at = m.end()
        return out + [text[at:]]

    @classmethod
    def widen(cls, text):
        """A piece of C++ as the Blueprint language computes it: number literals without the f that
        makes them 32-bit, float as double, the functions of numbers those of kern::math."""
        return cls.in_code(text, cls.widen_code)

    @staticmethod
    def widen_code(text):
        text = re.sub(r"(?<![\w.])((?:\d+\.\d*|\.\d+|\d+)(?:[eE][+-]?\d+)?)[fF]\b", lambda m: m.group(1) if re.search(r"[.eE]", m.group(1)) else m.group(1) + ".0", text)
        text = re.sub(r"\bfloat\b", "double", text)
        text = re.sub(r"\b(?:std::)?(u?)int64_t\b", lambda m: "u64" if m.group(1) else "i64", text)
        text = re.sub(r"\b(?:" + ENGINE + r"::)?dm::(" + "|".join(WIDE_MATH) + r")\s*\(", lambda m: WIDE_MATH[m.group(1)] + "(", text)   # (the engine's own functions of numbers: the formula's)
        return re.sub(r"\bstd::(" + "|".join(WIDE_MATH) + r")f?\s*\(", lambda m: WIDE_MATH[m.group(1)] + "(", text)

    def piece(self, d):
        a, b = d["_at"]
        return self.tree.said(self.cut(d["_file"], a, b))

    def cut(self, file, a, b, edits=None):
        """The bytes of a file from a to b, with the guards of `guard` in them (or the changes `edits`)."""
        raw = self.tree.text(file)
        out, at = "", a
        for pos, _, _, end, text in (self.edits.get(file, ()) if edits is None else edits):
            if pos < a or end > b:
                continue
            out += raw[at:pos] + text + "\n" * raw[pos:end].count("\n")
            at = end
        return out + raw[at:b]

    def guard(self, d):
        """C++ leaves open what an entry outside a list, a division of whole numbers by zero or a shift by
        the width of the number or more gives - and a function tried with any arguments gets there. The
        source of the test says it: such a place goes through a function of converted.h that gives the
        value where there is one and marks the call where there is none (it is then not compared)."""
        file, (lo, hi) = d["_file"], d["_at"]
        raw = self.tree.text(file)
        edits = []
        def span(n):
            a, b = n.get("_at", (None, None))
            if a is None or b is None or n.get("_file") != file or not lo <= a < b <= hi:
                raise Wait("written by a macro")
            return a, b
        def swap(a, b, token, text):
            if raw[a:b].strip() != token:
                raise Wait("written by a macro")
            edits.append((a, 1, 0, b, text))
        def whole(n):
            c = self.u.clean(qtype(n))
            return NUMBER.get(c) == "Int" or self.u.find_type(c) in self.tree.enums
        def wide(n):
            return self.u.clean(qtype(n)) in ("long", "unsigned long", "long long", "unsigned long long")
        def fixed(n):
            n = bare(n)
            while n.get("kind") in ("ImplicitCastExpr", "CStyleCastExpr", "CXXStaticCastExpr", "CXXFunctionalCastExpr") and kids(n):
                n = bare(kids(n)[-1])
            return n.get("kind") == "IntegerLiteral" and n.get("value") not in ("0", None)
        def within(base, index):
            """An entry of a list of fixed length (T[N], std::array<T, N>) at a constant place inside it."""
            inner = bare(base)
            while inner.get("kind") == "ImplicitCastExpr" and inner.get("castKind") in ("ArrayToPointerDecay", "NoOp") and kids(inner):
                inner = bare(kids(inner)[0])
            length = self.u.entries(qtype(inner))
            i = bare(index)
            while i.get("kind") in CAST and kids(i):
                i = bare(kids(i)[-1])
            ref = self.tree.decl.get((i.get("referencedDecl") or {}).get("id")) if i.get("kind") == "DeclRefExpr" else None
            if length is None or not (i.get("kind") == "IntegerLiteral" or (ref is not None and ref.get("kind") == "VarDecl" and (ref.get("constexpr") or re.search(r"\bconst\b", qtype(ref))))):
                return False
            try:
                at = Const(self.u).number(i)
            except Wait:
                return False
            return at == int(at) and 0 <= at < length
        def into_list(n):
            """Is this a parameter that is a pointer into a list (Unit.pointed)?"""
            x = bare(n)
            while x.get("kind") == "ImplicitCastExpr" and kids(x):
                x = bare(kids(x)[0])
            r = x.get("referencedDecl") or {}
            if x.get("kind") != "DeclRefExpr" or r.get("kind") != "ParmVarDecl":
                return False
            try:
                return self.u.pointed(qtype(r)) is not None
            except Wait:
                return False
        visited = set()
        def visit(n, depth):
            if n.get("_call") is not None:   # (a place written out for a call: the source has the call)
                return visit(n["_call"], depth)
            k, ks = n.get("kind"), kids(n)
            if n.get("id") is not None:
                if n["id"] in visited:
                    return   # (a place with a name of its own - `T& x = a[i];` - is in the tree wherever x is used: guarded once, where it is written)
                visited.add(n["id"])
            if k == "LambdaExpr":   # (a local function: its body once - the tree has it in its class as well)
                for c in ks[-1:]:
                    visit(c, depth + 1)
                return
            name = None
            if k == "CXXOperatorCallExpr" and len(ks) == 3:
                callee = bare(ks[0])
                while callee.get("kind") == "ImplicitCastExpr" and kids(callee):
                    callee = bare(kids(callee)[0])
                name = (callee.get("referencedDecl") or {}).get("name")
            if name == "operator[]" and (KEYED.match(self.u.clean(qtype(ks[1]))) or KEYED_KERN.match(self.u.clean(qtype(ks[1])))):
                pass   # (m[key] is said for every key)
            elif name == "operator[]" and self.u.clean(qtype(ks[1])) == "kern::Vec3":
                pass   # (a vector's component by its number: x, y - and z for any other number; there for every call)
            elif (k == "ArraySubscriptExpr" or name == "operator[]") and within(*ks[-2:]):
                pass   # (an entry of an array of fixed length at a constant place inside it: there for every call)
            elif k == "ArraySubscriptExpr" and into_list(ks[0]):
                # an entry through a pointer into a list (Unit.pointed): checked against the list the test handed it
                (a, b), (ba, bb), (ia, ib) = span(n), span(ks[0]), span(ks[1])
                edits.append((a, 2, depth, a, "converted_pointed("))
                swap(bb, ia, "[", ", ")
                swap(ib, b, "]", ")")
                self.pointed = True
            elif k == "UnaryOperator" and n.get("opcode") == "*" and ks and into_list(ks[0]):
                (a, b), (xa, xb) = span(n), span(ks[0])
                swap(a, xa, "*", "converted_pointed(")
                edits.append((b, 0, -depth, b, ", 0)"))
                self.pointed = True
            elif k == "ArraySubscriptExpr" or name == "operator[]":
                base, index = ks[-2:]
                inner = bare(base)
                if k == "ArraySubscriptExpr" and not (inner.get("kind") == "ImplicitCastExpr" and inner.get("castKind") == "ArrayToPointerDecay"):
                    raise Wait("an entry by a pointer")
                if k == "ArraySubscriptExpr" and qtype(kids(inner)[0]).rstrip().endswith("[]"):
                    raise Wait("an entry of a list whose length stands in another file")
                (a, b), (ba, bb), (ia, ib) = span(n), span(base), span(index)
                edits.append((a, 2, depth, a, "converted::at("))
                swap(bb, ia, "[", ", ")
                swap(ib, b, "]", ")")
            elif k == "CXXMemberCallExpr" and ks and ks[0].get("kind") == "MemberExpr" and ks[0].get("name") in ("at", "front", "back") and kids(ks[0]) and not KEYED.match(self.u.clean(qtype(kids(ks[0])[0]))) and not KEYED_KERN.match(self.u.clean(qtype(kids(ks[0])[0]))):
                base = kids(ks[0])[0]
                if ks[0].get("isArrow") or base.get("kind") == "CXXThisExpr":
                    raise Wait("an entry by a pointer")
                (a, b), (ba, bb) = span(n), span(base)
                edits.append((a, 2, depth, a, f"converted::{ks[0]['name']}("))
                if ks[0]["name"] == "at" and len(ks) == 2:
                    ia, ib = span(ks[1])
                    swap(bb, ia, ".at(", ", ")
                    swap(ib, b, ")", ")")
                else:
                    swap(bb, b, f".{ks[0]['name']}()", ")")
            elif n.get("castKind") == "FloatingToIntegral" and ks and self.u.clean(qtype(n)) in Const.BITS:
                # a number as a whole number of a type that does not hold it
                ca, cb = span(ks[-1])
                target = {"whole64u": "converted::Whole", "whole64s": "converted::SignedWhole"}.get(self.u.clean(qtype(n)), self.u.clean(qtype(n)))
                edits.append((ca, 2, depth, ca, f"converted::fits<{target}>("))
                edits.append((cb, 0, -depth, cb, ")"))
            elif (k in ("CXXStaticCastExpr", "CStyleCastExpr", "CXXFunctionalCastExpr") and ks and n.get("castKind") == "IntegralCast"
                  and self.u.find_type(self.u.clean(qtype(n))) in self.tree.enums and NUMBER.get(self.u.clean(qtype(ks[-1]))) == "Int"):
                # a whole number as a value of an enum that has no such value
                ca, cb = span(ks[-1])
                valid = ", ".join(str(v) for _, v in enum_numbers(self.tree.enums[self.u.find_type(self.u.clean(qtype(n)))]))
                edits.append((ca, 2, depth, ca, f"converted::among({{{valid}}}, "))
                edits.append((cb, 0, -depth, cb, ")"))
            elif ((k == "CXXConstructExpr" and re.match(r"(const )?std::vector<", self.u.clean(qtype(n))) and ks and whole(ks[0]) and not n.get("list"))
                  or (k == "CXXMemberCallExpr" and len(ks) > 1 and ks[0].get("kind") == "MemberExpr" and ks[0].get("name") in ("resize", "assign", "reserve") and kids(ks[0])
                      and re.match(r"(const )?std::vector<", self.u.clean(qtype(kids(ks[0])[0]))) and whole(ks[1]))):
                # a list of so many entries: a count the test does not build (a negative one, read without its sign) marks the call
                count = ks[0] if k == "CXXConstructExpr" else ks[1]
                ca, cb = span(count)
                edits.append((ca, 2, depth, ca, "converted::count("))
                edits.append((cb, 0, -depth, cb, ")"))
            elif (k == "CXXMemberCallExpr" and ks and ks[0].get("kind") == "MemberExpr" and ks[0].get("name") in ("erase", "pop_back") and kids(ks[0])
                  and re.match(r"(const )?std::vector<", self.u.clean(qtype(kids(ks[0])[0])))):
                # taking an entry out of a list that does not have it
                base = kids(ks[0])[0]
                if ks[0].get("isArrow") or base.get("kind") == "CXXThisExpr":
                    raise Wait("an entry by a pointer")
                (a, b), (ba, bb) = span(n), span(base)
                edits.append((a, 2, depth, a, f"converted::{ks[0]['name']}("))
                if len(ks) > 1:
                    swap(bb, span(ks[1])[0], f".{ks[0]['name']}(", ", ")
                else:
                    swap(bb, b, f".{ks[0]['name']}()", ")")
            elif n.get("_w64") and k in ("BinaryOperator", "CompoundAssignOperator") and n.get("opcode") in ("+", "-", "*", "/", "%", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=") and len(ks) == 2:
                # a 64-bit bit pattern (a Blueprint's Whole64): computed around 2^64 - only what C++ leaves open marks the call
                op = n["opcode"]
                (a, b), (xa, xb), (ya, yb) = span(n), span(ks[0]), span(ks[1])
                name = {"+": "plus", "-": "minus", "*": "times", "/": "quot", "%": "rem", "<<": "shl", ">>": "shr"}[op[:-1] if k == "CompoundAssignOperator" else op]
                edits.append((a, 2, depth, a, f"converted::{name}_bits" + ("_to(" if k == "CompoundAssignOperator" else "(")))
                swap(xb, ya, op, ", ")
                edits.append((b, 0, -depth, b, ")"))
            elif n.get("_w64") and k == "UnaryOperator" and n.get("opcode") in ("++", "--") and len(ks) == 1:
                (a, b), (xa, xb) = span(n), span(ks[0])
                name = ("post_" if n.get("isPostfix") else "pre_") + ("inc" if n["opcode"] == "++" else "dec") + "_bits"
                if n.get("isPostfix"):
                    edits.append((a, 2, depth, a, f"converted::{name}("))
                    swap(xb, b, n["opcode"], ")")
                else:
                    swap(a, xa, n["opcode"], f"converted::{name}(")
                    edits.append((b, 0, -depth, b, ")"))
            elif k in ("BinaryOperator", "CompoundAssignOperator") and n.get("opcode") in ("+", "-", "*", "+=", "-=", "*=") and len(ks) == 2 and wide(n) and whole(ks[0]) and whole(ks[1]):
                # a 64-bit whole number: a Blueprint holds it while it is within 2^53 of zero (read with its sign)
                op = n["opcode"]
                (a, b), (xa, xb), (ya, yb) = span(n), span(ks[0]), span(ks[1])
                edits.append((a, 2, depth, a, "converted::" + {"+": "plus", "-": "minus", "*": "times"}[op.rstrip("=")] + ("_to(" if op.endswith("=") else "(")))
                swap(xb, ya, op, ", ")
                edits.append((b, 0, -depth, b, ")"))
            elif k == "UnaryOperator" and n.get("opcode") in ("++", "--") and len(ks) == 1 and wide(ks[0]):
                (a, b), (xa, xb) = span(n), span(ks[0])
                name = ("post_" if n.get("isPostfix") else "pre_") + ("inc" if n["opcode"] == "++" else "dec")
                if n.get("isPostfix"):
                    edits.append((a, 2, depth, a, f"converted::{name}("))
                    swap(xb, b, n["opcode"], ")")
                else:
                    swap(a, xa, n["opcode"], f"converted::{name}(")
                    edits.append((b, 0, -depth, b, ")"))
            elif k in ("BinaryOperator", "CompoundAssignOperator") and n.get("opcode") in ("/", "%", "<<", ">>", "/=", "%=", "<<=", ">>=") and len(ks) == 2 and whole(ks[0]) and whole(ks[1]) and (not fixed(ks[1]) or wide(n)):
                op = n["opcode"]
                (a, b), (xa, xb), (ya, yb) = span(n), span(ks[0]), span(ks[1])
                edits.append((a, 2, depth, a, "converted::" + {"/": "quot", "%": "rem", "<<": "shl", ">>": "shr"}[op.rstrip("=")] + ("_to(" if op.endswith("=") else "(")))
                swap(xb, ya, op, ", ")
                edits.append((b, 0, -depth, b, ")"))
            for c in ks:
                visit(c, depth + 1)
        for c in kids(d):
            if c.get("kind") in ("CompoundStmt", "CXXCtorInitializer"):
                visit(c, 0)
        return file, edits

    def held(self, decls):
        """The functions whose source the test holds: those given (of the Blueprint that are written), and all
        they call that has a body here - also what stands in other Blueprints (their declarations)."""
        tree = self.tree
        out, todo = {}, list(decls)
        while todo:
            d = todo.pop()
            if d["id"] in out or d.get("_at", (None, None))[1] is None or not d.get("_file"):
                continue
            out[d["id"]] = d
            for x in walk_source(d):
                ids = [x.get("referencedDecl", {}).get("id"), x.get("referencedMemberDecl")]
                if x.get("kind") == "CXXConstructExpr":
                    ids.append((constructor_of(tree, x) or {}).get("id"))
                for i in ids:
                    t = tree.body.get(i) if i else None
                    if t is not None and t.get("kind") in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXConversionDecl") and not t.get("_mine"):
                        todo.append(t)
        return list(out.values())

    def guard_all(self, decls):
        """The guards of all the functions the test holds the source of (a function with a place the test
        cannot guard: said, by its declaration's id - see `write`)."""
        self.edits, unguarded, done = {}, {}, set()
        for d in self.held(decls):
            if d["id"] in done:
                continue
            done.add(d["id"])
            try:
                file, edits = self.guard(d)
                self.edits.setdefault(file, []).extend(edits)
            except Wait as w:
                unguarded[d["id"]] = str(w)
        for file, edits in self.edits.items():
            self.edits[file] = self.in_turn(edits)
        return unguarded

    @staticmethod
    def in_turn(edits):
        """Changes of a file in the order of the file (a place two functions share - never - or one inside the
        other's: the outer one stays)."""
        kept, at = [], 0
        for e in sorted(dict.fromkeys(edits), key=lambda e: e[:3]):
            if e[0] >= at:
                kept.append(e)
                at = max(at, e[3])
        return kept

    def guarded(self, d, extra=()):
        """A declaration's text as a test holds it: its places guarded (`guard` - a function with a place it
        cannot guard as it stands, as in `write`), and the changes `extra` made."""
        try:
            _, edits = self.guard(d)
        except Wait:
            edits = []
        a, b = d["_at"]
        return self.tree.said(self.cut(d["_file"], a, b, self.in_turn([*edits, *extra])))

    def inside(self, d, others):
        """The declaration among `others` whose text holds d's (a class and what is declared in it)."""
        a, b = d.get("_at", (None, None))
        best = None
        for o in others:
            oa, ob = o.get("_at", (None, None))
            if o is not d and oa is not None and a is not None and o.get("_file") == d.get("_file") and oa <= a and b <= ob and (oa, ob) != (a, b):
                if best is None or oa >= best["_at"][0]:
                    best = o
        return best

    def absolute(self, q):
        return self.tree.absolute(q)

    def local(self, q):
        d = self.tree.records.get(q) or self.tree.enums.get(q)
        return d is not None and os.path.abspath(d.get("_file", "")) == self.path

    # ---- types as the test writes them ----
    @staticmethod
    def wrap(expr, spelled):
        """A value of the test as the Blueprint has it where the two differ by more than their numbers: a
        64-bit bit pattern (the program's u64, i64) is the language's Int64 - also in a list, a pair, a map
        or a set: converted::shaped makes it that, by the type as the program spells it."""
        if not re.search(r"\b[ui]64\b", spelled):
            return expr
        return f"converted::shaped<{re.sub(r'\b[ui]64\b', 'converted::Whole', spelled)}>({expr})"

    def object_name(self, q):
        """The stand-in of a class of the engine in the test (its objects are handles in the Blueprint)."""
        if q not in self.objects:
            name = q.split("::")[-1]
            if name in self.names.values() or name in self.enum_names.values() or name in self.objects.values():
                name = "Engine" + name
            self.objects[q] = self.names[q] = name
        return self.objects[q]

    def spell(self, raw):
        c = self.u.clean(raw)
        if raw.replace(" ", "") in ("constchar*", "constchar*const"):
            return "const char*"
        q = self.u.object_of(raw)
        if q:   # (held as the program holds it: the stand-in by a pointer, a reference, as a value or owned)
            r, name = raw.strip(), self.object_name(q)
            if re.match(r"(?:const\s+)?std::unique_ptr<", self.u.clean(r)):
                return f"std::unique_ptr<{name}>"
            const = "const " if re.match(r"const\b", r) else ""
            return const + name + ("&" if r.endswith("&") else "*" if re.search(r"\*\s*(const)?$", r) else "")
        m = re.match(r"(?:kern::)?Span<\s*(.+)>$", c)
        if m:
            return f"std::vector<{self.spell(m.group(1))}>"
        if c in W64:
            return "u64" if c == "whole64u" else "i64"
        if c in ("float", "double", "long double"):
            return "double"
        if c in NUMBER:
            return c
        if c in ("kern::Vec3", "kern::Vec2") or self.u.find_type(c) in ("kern::Vec3", "kern::Vec2"):
            return c.split("::")[-1]   # (the test's vector: converted::wide)
        if (c in TEXT or c.startswith("std::basic_string<char")) and "string_view" not in c and "*" not in c:
            return "std::string"
        if re.match(r"std::(?:basic_string_view<char(?:,\s*std::char_traits<char>)?>|string_view)$", c):
            return "std::string_view"   # (a view of a text: of one the test keeps - in the Blueprint the text)
        m = re.match(r"std::array<(.+),\s*(\d+)U?L?L?>$", c)
        if m:   # (a list of a fixed length: one of the test's values, a list in the Blueprint)
            return f"std::array<{self.spell(template_args(c)[0])}, {m.group(2)}>"
        m = re.match(r"std::vector<(.+)>$", c)
        if m:
            inner = template_args(c)[0]
            if "[" in inner:
                raise Wait("the test has no values of a list inside a list")
            return f"std::vector<{self.spell(inner)}>"
        m = KEYED_KERN.match(c)
        if m:   # (a map, a set: the program's kern::Map, kern::Set - the language's very containers, of the test's values)
            a = template_args(c)
            return f"converted::wide::Set<{self.spell(a[0])}>" if m.group(1) == "Set" else f"converted::wide::Map<{self.spell(a[0])}, {self.spell(a[1])}>"
        if c.startswith("std::pair<"):
            a = template_args(c)
            return f"std::pair<{self.spell(a[0])}, {self.spell(a[1])}>"
        m = re.match(r"std::(unique_ptr|optional|initializer_list)<", c)
        if m:   # (what may be there or not - in the Blueprint a list of no entry or one; a list in braces: the drawn list, see converted::listed)
            inner = self.spell(template_args(c)[0])
            return f"std::vector<{inner}>" if m.group(1) == "initializer_list" else f"std::{m.group(1)}<{inner}>"
        q = self.record_named(c)
        if q in self.tree.enums:
            if q not in self.enum_names:
                raise Wait(f"the enum {q.split('::')[-1]} is not written")
            return self.enum_names[q]
        if q in self.tree.records:
            if q not in self.names:
                raise Wait(f"the class {q.split('::')[-1]} is not written")
            return self.names[q]
        raise Wait(f"the test has no values of {c}")

    def record_named(self, c):
        """The class or enum of the program a type of the test's values names - the one it names by its full
        name. (A class this file only declares - its definition stands in another source file - is none
        the test has: its values wait.)"""
        named = c[len(self.tree.namespace) + 2:] if c.startswith(self.tree.namespace + "::") else c
        q = named if named in self.tree.records or named in self.tree.enums else self.u.find_type(c)
        if q is not None and "::" in named and q != named and not q.endswith("::" + named):
            raise Wait(f"the test has no values of {c} (its class is defined in another file)")
        return q

    def actor_blueprint(self, t):
        """Is this the type of the program's way to call the Blueprint of an actor of the level - the engine's
        kern::logic::ActorBlueprint, or a class of the program made of it?"""
        c = re.sub(r"^const\s+", "", self.u.clean(t)).strip()
        if c == ENGINE + "::logic::ActorBlueprint":
            return True
        try:
            q = self.record_named(c)
        except Wait:
            return False
        d = self.tree.records.get(q)
        return d is not None and any(self.actor_blueprint(b.get("type", {}).get("qualType", "")) for b in d.get("bases", []) or [])

    def draw(self, raw):
        c = self.u.clean(raw)
        if raw.replace(" ", "") in ("constchar*", "constchar*const"):
            return "draw.c_word()"
        q = self.u.object_of(raw)
        if q:   # (an object of the engine: one of a few the test keeps, by its handle)
            one = f"converted::engine_object<{self.object_name(q)}>(1 + draw.below(6))"
            r = raw.strip()
            if re.match(r"(?:const\s+)?std::unique_ptr<", self.u.clean(r)):
                return f"std::make_unique<{self.object_name(q)}>(*{one})"
            return one if re.search(r"\*\s*(const)?$", r) else "*" + one
        m = re.match(r"(?:kern::)?Span<\s*(?:const\s+)?(.+)>$", c)
        if m:   # (a view: of entries the test keeps - converted::kept_span)
            inner = m.group(1)
            return f"converted::kept_span(draw_list(draw, [&]() -> {self.spell(inner)} {{ return {self.draw(inner)}; }}))"
        if c in W64:
            return f"static_cast<{self.spell(raw)}>(draw.bits())"
        if c in ("float", "double", "long double"):
            return "draw.number()"
        if c == "bool":
            return "draw.flag()"
        if c in NUMBER:
            return f"static_cast<{c}>(draw.below(40))" if c in UNSIGNED else f"static_cast<{c}>(draw.whole())"
        if self.u.find_type(c) in ("kern::Vec3", "kern::Vec2") or c in ("kern::Vec3", "kern::Vec2"):
            return "draw.vector()" if c.endswith("Vec3") else "draw.vector2()"
        m = re.match(r"(?:kern::)?Span<\s*(.+)>$", c)
        if m:   # (a view of a list, handed to a function: the list)
            return self.draw("std::vector<" + re.sub(r"^const\s+", "", m.group(1)) + ">")
        if self.spell(raw) == "std::string":
            return "draw.word()"
        if self.spell(raw) == "std::string_view":
            return "std::string_view(draw.c_word())"   # (the drawn words live as long as the test)
        m = re.match(r"std::array<(.+),\s*(\d+)U?L?L?>$", c)
        if m:
            inner = template_args(c)[0]
            return f"converted::draw_array<{self.spell(raw)}>([&]() -> {self.spell(inner)} {{ return {self.draw(inner)}; }})"
        m = re.match(r"std::vector<(.+)>$", c)
        if m:
            inner = template_args(c)[0]
            return f"draw_list(draw, [&]() -> {self.spell(inner)} {{ return {self.draw(inner)}; }})" if self.u.clean(inner) != "bool" else "draw_list(draw, [&] { return draw.flag(); })"
        m = KEYED_KERN.match(c)
        if m:   # (each key once: one drawn again is put in once, where it came first)
            a = template_args(c)
            key = f"[&]() -> {self.spell(a[0])} {{ return {self.draw(a[0])}; }}"
            if m.group(1) == "Set":
                return f"converted::draw_set<{self.spell(raw)}>(draw, {key})"
            return f"converted::draw_map<{self.spell(raw)}>(draw, {key}, [&]() -> {self.spell(a[1])} {{ return {self.draw(a[1])}; }})"
        if c.startswith("std::pair<"):
            a = template_args(c)
            return f"{self.spell(raw)}({self.draw(a[0])}, {self.draw(a[1])})"
        m = re.match(r"std::(unique_ptr|optional|initializer_list)<", c)
        if m:
            inner = template_args(c)[0]
            one = f"[&]() -> {self.spell(inner)} {{ return {self.draw(inner)}; }}"
            if m.group(1) == "initializer_list":   # (a list written in braces: a few entries)
                return f"converted::draw_few(draw, {one})"
            return f"converted::draw_maybe<{self.spell(raw)}>(draw, {one})"
        q = self.record_named(c)
        if q in self.tree.enums:
            return f"static_cast<{self.spell(raw)}>(draw.below({len(self.u.enum_values[self.u.enums[q]])}))"
        if q in self.tree.records:
            return f"{self.helpers(q)}(draw)"
        raise Wait(f"the test has no values of {c}")

    def result_ok(self, raw):
        """Can the test compare a value of this type? (Writes the helpers it needs.)"""
        c = self.u.clean(raw)
        if raw.strip() in ("const char *", "const char *const"):
            return
        self.spell(raw)
        def parts(c):
            """The classes a type is made of, through lists, maps, sets and pairs."""
            if re.match(r"std::(vector|array|unique_ptr|optional|initializer_list)<", c):
                return parts(template_args(c)[0])
            m = KEYED_KERN.match(c)
            if m or c.startswith("std::pair<"):
                a = template_args(c)
                return parts(a[0]) + (parts(a[1]) if not (m and m.group(1) == "Set") else [])
            return [c]
        for part in parts(c):
            q = self.u.find_type(part)
            if q in self.tree.records and q not in ("kern::Vec3", "kern::Vec2"):
                self.helpers(q)

    def helpers(self, q):
        """The helpers of a class of the test - draw, arg, same, defined, text - and the name of draw."""
        name = "draw_" + re.sub(r"\W", "_", q.strip(":"))
        if q in self.helped:
            if self.helped[q] is None:
                raise Wait("a class that holds itself")
            return name
        if q not in self.names:
            raise Wait(f"the class {q.split('::')[-1]} is not written")
        o = self.tree.records[q]
        t = self.names[q]
        ctors = [c for c in kids(o) if c.get("kind") == "CXXConstructorDecl" and not c.get("isImplicit")]
        # (the constructors the test's class has: those whose source it holds - the others are not part of it)
        held_ctors = [c for c in ctors if c.get("explicitlyDefaulted") or c["id"] in self.whole.kept or self.tree.body.get(c["id"], c)["id"] in self.whole.functions]
        fields = [(c["name"], qtype(c)) for c in kids(o) if c.get("kind") == "FieldDecl"]
        self.helped[q] = None
        try:
            make, built = f"  {t} s{{}};", None
            if any("&" in ft for _, ft in fields):
                # a reference: to a value the test keeps - the object is made with it, field by field (the
                # test's class has no constructor then)
                if held_ctors or any("&&" in ft for _, ft in fields):
                    raise Wait("the test cannot make an object that holds a reference")
                ctors = []
            for c in ctors:   # (the first constructor with arguments the test has values for)
                params = [p for p in kids(c) if p.get("kind") == "ParmVarDecl"]
                if not params or (len(params) == 1 and self.u.find_type(self.u.clean(qtype(params[0]))) == q):
                    continue
                try:
                    built = (c["id"], f"{t}({', '.join(self.draw(qtype(p)) for p in params)})")
                    break
                except Wait:
                    continue
            if ctors and not any(not [p for p in kids(c) if p.get("kind") == "ParmVarDecl"] for c in ctors) and (built is not None or held_ctors):
                # (no constructor without arguments: made by that one, then filled - or, where the test's class
                # has no constructor, made as it is and filled)
                if built is None:
                    raise Wait("the test cannot make an object of a class without a default constructor")
                make = f"  {t} s = {built[1]};"
                self.made[q] = (built[0], f"  {t} s{{}};")
            elif built is not None:
                # a class with a constructor may keep its parts in step (a grid: its size and its cells): every
                # other object is the one the constructor makes, the others have every field drawn on its own
                make = f"  if (draw.flag()) return {built[1]};\n" + make
                self.made[q] = (built[0], f"  {t} s{{}};")
            arrays, lines, viewed, standard = {}, [f"[[maybe_unused]] static {t} {name}(converted::Draw& draw) {{", make], set(), {}
            held = []
            for f, ft in fields:
                if "&" in ft:
                    inner = re.sub(r"^const\s+", "", ft.replace("&", "").strip())   # (kept as a value: the reference may read it only)
                    lines[lines.index(make):lines.index(make)] = [f"  static std::deque<{self.spell(inner)}> kept_{f};", f"  kept_{f}.push_back({self.draw(inner)});"]
                    held.append(f)
                    continue
                m = re.match(r"(?:kern::)?Span<\s*(.+)>$", self.u.clean(ft))
                if m:   # (a view of a list: of one the test keeps - in the Blueprint the list)
                    inner = re.sub(r"^const\s+", "", m.group(1))
                    arrays[f] = f"std::vector<{self.spell(inner)}>"
                    viewed.add(f)
                    lines += [f"  static std::deque<{arrays[f]}> kept_{f};", f"  kept_{f}.push_back({self.draw('std::vector<' + inner + '>')});",
                              f"  s.{f} = {{kept_{f}.back().data(), kept_{f}.back().size()}};"]
                    continue
                if self.u.clean(ft).startswith("std::array<"):
                    # (a std::array: the list of its entries, each drawn on its own - and taken back entry by entry,
                    # where it stands: an entry need not be made of nothing, a class without a default constructor)
                    entry = template_args(self.u.clean(ft))[0]
                    if entry.startswith("std::array<") or "[" in entry:
                        raise Wait("the test has no values of an array of arrays")
                    arrays[f] = f"std::vector<{self.spell(entry)}>"
                    standard[f] = self.spell(entry)
                    lines.append(f"  for (auto& e : s.{f}) e = {self.draw(entry)};")
                    continue
                m = re.match(r"(.+?)\s*((?:\s*\[\d+\])+)$", self.u.clean(ft))   # (T[3][2]: 3 entries of T[2] - a list of lists in the Blueprint)
                if m:
                    rank = m.group(2).count("[")
                    arrays[f] = "std::vector<" * rank + self.spell(m.group(1)) + ">" * rank
                    lines.append(f"  converted::fill_array(s.{f}, [&]() -> {self.spell(m.group(1))} {{ return {self.draw(m.group(1))}; }});")
                else:
                    lines.append(f"  s.{f} = {self.draw(ft)};")
        except Wait:
            del self.helped[q]
            raise
        if held:
            lines[lines.index(make)] = f"  {t} s{{{', '.join(f'.{f} = kept_{f}.back()' for f in held)}}};"
        spelled = {f: arrays[f] if f in arrays else self.spell(ft) for f, ft in fields if f not in held}
        def of(f, obj="s"):
            return self.wrap(f"{arrays[f]}({obj}.{f}.begin(), {obj}.{f}.end())" if f in viewed or f in standard else f"converted::array_list({obj}.{f})" if f in arrays else f"{obj}.{f}", spelled.get(f, ""))
        lines += ["  return s;", "}"]
        lines.append(f"[[maybe_unused]] static converted::Value arg(const {t}& s) {{ return converted::Value::structure({{{', '.join(f'arg({of(f)})' for f, _ in fields)}}}); }}")
        lines.append(f"[[maybe_unused]] static bool same(const {t}& want, const converted::Value& got) {{ return got.rows && got.rows->size() == {len(fields)}"
                     + "".join(f" && same({of(f, 'want')}, (*got.rows)[{i}])" for i, (f, _) in enumerate(fields)) + "; }")
        lines.append(f"[[maybe_unused]] static bool defined(const {t}& s) {{ return " + " && ".join(f"defined({of(f)})" for f, _ in fields) + "; }")
        lines.append(f"[[maybe_unused]] static std::string text(const {t}& s) {{ return std::string(\"{{\") + " + ' + ", " + '.join(f"text({of(f)})" for f, _ in fields) + ' + "}"; }')
        shape = lambda x: re.sub(r"\b[ui]64\b", "converted::Whole", x)
        takes = []
        for i, (f, _) in enumerate(fields):
            if f in held or f in viewed:
                continue   # (a reference, a view: what they show the Blueprint's value does not give them)
            if f in arrays:
                if f in standard:   # (entry by entry where it stands: an entry need not be made of nothing)
                    takes.append(f"{{ const converted::Value& items = (*v.rows)[{i}]; for (std::size_t k = 0; k < items.size() && k < std::size(s.{f}); ++k) converted::take_as<{shape(standard[f])}>(s.{f}[k], items.item(k)); }}")
                else:
                    takes.append(f"{{ {spelled[f]} items; converted::take_as<{shape(spelled[f])}>(items, (*v.rows)[{i}]); converted::array_from(s.{f}, items); }}")
            else:
                takes.append(f"converted::take_as<{shape(spelled[f])}>(s.{f}, (*v.rows)[{i}]);")
        lines.append(f"[[maybe_unused]] static void take({t}& s, const converted::Value& v) {{ if (!v.rows || v.rows->size() != {len(fields)}) return; {' '.join(takes)} }}")
        self.helped[q] = lines
        self.helped.move_to_end(q)   # (after the helpers of the classes it is made of)
        return name

    def state_helpers(self):
        """The helpers of the test of a part of the Blueprint of a state (World.states): an object of the class
        whose fields that are the Blueprint's variables are drawn (the others as the class makes them - no
        function of the Blueprint touches them), those variables as the Blueprint is given them
        (converted::Variables), and what is compared after a call: every one of them. {draw, variables,
        struct: the names}."""
        q = self.u.state
        if self.state_info is None:
            try:
                if q not in self.names:
                    raise Wait(f"the class {q.split('::')[-1]} of the state is not part of the test")
                t, key = self.names[q], re.sub(r"\W", "_", q.strip(":"))
                info = {"draw": f"draw_state_{key}", "variables": f"variables_{key}", "struct": f"State_{key}"}
                draws, values, checks, texts, fine, missing = [], [], [], [], [], []
                for c in kids(self.tree.records[q]):
                    if c.get("kind") != "FieldDecl" or not c.get("name"):
                        continue
                    try:
                        variable, _ = self.u.state_variable(c["name"])
                    except Wait:
                        continue   # (no variable: what touches it waits)
                    ft = qtype(c)
                    try:
                        if "&" in ft or re.match(r"(?:kern::)?Span<", self.u.clean(ft)):
                            raise Wait("a reference, a view")
                        m = re.match(r"(.+?)\s*((?:\s*\[\d+\])+)$", self.u.clean(ft))
                        if m:   # (an array of C++: a list of a fixed length in the Blueprint)
                            rank = m.group(2).count("[")
                            spelled = "std::vector<" * rank + self.spell(m.group(1)) + ">" * rank
                            draws.append(f"  converted::fill_array(s->{c['name']}, [&]() -> {self.spell(m.group(1))} {{ return {self.draw(m.group(1))}; }});")
                            of = lambda o, f=c["name"], sp=spelled: self.wrap(f"converted::array_list({o}{f})", sp)
                        else:
                            spelled = self.spell(ft)
                            draws.append(f"  s->{c['name']} = {self.draw(ft)};")
                            of = lambda o, f=c["name"], sp=spelled: self.wrap(f"{o}{f}", sp)
                        if not self.u.object_of(ft):   # (an object of the engine: a stand-in, by its handle)
                            self.result_ok(ft if m is None else m.group(1))
                    except Wait as w:
                        missing.append(f"{variable} ({w})")
                        continue
                    values.append(f'{{"{variable}", arg({of("s.")})}}')
                    checks.append(f"same({of('w.s->')}, (*got.rows)[{len(checks)}])")
                    fine.append(f"defined({of('w.s->')})")
                    texts.append(f'"{variable} = " + text({of("w.s->")})')
                # what the state keeps besides its fields (Unit.state_static): a variable of a file at the level of a
                # namespace is one of the test's source too - drawn, set and compared by its name there; one a
                # method keeps in itself (static in its body) no code outside the method reaches
                for i, (variable, _) in self.u.state_statics.items():
                    v = self.tree.decl.get(i) or {}
                    if self.in_function(v) or v.get("storageClass") == "static" and v.get("isStaticLocal") or not v.get("name"):
                        missing.append(f"{variable} (kept in the body of a method - static there: the test does not reach it from outside)")
                        continue
                    if i not in self.whole.consts:
                        continue   # (not named by the source the test holds)
                    first = v
                    while first.get("previousDecl") in self.tree.decl:
                        first = self.tree.decl[first["previousDecl"]]
                    held = "::".join(self.classes_of(self.tree.scope_of.get(first["id"], "")) + [v["name"]])
                    try:
                        spelled = self.spell(qtype(v))
                        draws.append(f"  {held} = {self.draw(qtype(v))};")
                    except Wait as w:
                        missing.append(f"{variable} ({w})")
                        continue
                    shown = self.wrap(held, spelled)
                    values.append(f'{{"{variable}", arg({shown})}}')
                    checks.append(f"same({shown}, (*got.rows)[{len(checks)}])")
                    fine.append(f"defined({shown})")
                    texts.append(f'"{variable} = " + text({shown})')
                if missing:   # (every variable is set and compared: one the test has no values of, and it cannot compare any)
                    raise Wait(f"the test has no values of the variables of {self.u.name}: {'; '.join(missing)}")
                n = len(values)
                self.helped[("state", q)] = [
                    f"[[maybe_unused]] static std::unique_ptr<{t}> {info['draw']}(converted::Draw& draw) {{",
                    f"  auto s = std::make_unique<{t}>();", *draws, "  return s;", "}",
                    f"[[maybe_unused]] static converted::Variables {info['variables']}(const {t}& s) {{ return {{{', '.join(values)}}}; }}",
                    f"struct {info['struct']} {{ const {t}* s; }};",
                    f"[[maybe_unused]] static bool same(const {info['struct']}& w, const converted::Value& got) {{ return got.rows && got.rows->size() == {n}" + "".join(f" && {x}" for x in checks) + "; }",
                    f"[[maybe_unused]] static bool defined(const {info['struct']}& w) {{ return " + (" && ".join(fine) or "true") + "; }",
                    f"[[maybe_unused]] static std::string text(const {info['struct']}& w) {{ return std::string(\"{{\")" + "".join(f" + {'\", \" + ' if i else ''}{x}" for i, x in enumerate(texts)) + " + \"}\"; }"]
                self.state_info = info
            except Wait as w:
                self.state_info = w
        if isinstance(self.state_info, Wait):
            raise Wait(str(self.state_info))
        return self.state_info

    def plain_struct(self, name, fields):
        """A struct of the test itself (what a call gives and changes), with what the comparison needs."""
        lines = [f"struct {name} {{ " + " ".join(f"{t} {f};" for f, t in fields) + " };"]
        lines.append(f"[[maybe_unused]] static bool same(const {name}& want, const converted::Value& got) {{ return got.rows && got.rows->size() == {len(fields)}"
                     + "".join(f" && same({self.wrap(f'want.{f}', t)}, (*got.rows)[{i}])" for i, (f, t) in enumerate(fields)) + "; }")
        lines.append(f"[[maybe_unused]] static bool defined(const {name}& s) {{ return " + " && ".join(f"defined({self.wrap(f's.{f}', t)})" if t.startswith("std::array<") else f"defined(s.{f})" for f, t in fields) + "; }")
        lines.append(f"[[maybe_unused]] static std::string text(const {name}& s) {{ return std::string(\"{{\") + " + ' + ", " + '.join(f"text({self.wrap(f's.{f}', t)})" if t.startswith("std::array<") else f"text(s.{f})" for f, t in fields) + ' + "}"; }')
        return lines

    # ---- the engine in the test: stand-ins that keep every call ----
    def stand_in(self, f, cls, plain=False):
        """A function of the engine as the test's source calls it: (its declaration, its definition). It hands
        its arguments to the stand-in of the engine as the values the Blueprint's node gets, and takes what
        the stand-in answers. `plain`: a method of plain data (the test's own struct, MeshData::as_input) - the
        struct is its first value, and what a method that is not const makes of it, the first it takes."""
        d = f["decl"]
        shape = lambda x: re.sub(r"\b[ui]64\b", "converted::Whole", x)
        params, values, takes = [], (["arg(*this)"] if cls else []), []
        if plain and not qtype(d).rstrip().endswith("const"):
            takes.append(("(*this)", cls, False))
        ps = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
        for i, (p, (pin_in, pin_out)) in enumerate(zip(ps, f["params"])):
            raw = qtype(p).strip()
            default = self.tree.source(kids(p)[-1]).strip().lstrip("=").strip() if kids(p) else ""   # (`= {}`: the tree has the sign with it)
            default = " = " + default if default else ""
            if pin_in is None:
                # what the function reports if it is asked to: written where the pointer says, if it says
                inner = self.spell(re.sub(r"\*\s*$", "", raw))
                self.result_ok(re.sub(r"\*\s*$", "", raw))
                params.append((f"{inner}* {pin_out}", default))
                takes.append((pin_out, inner, True))
                continue
            name = pin_in
            if not self.u.object_of(raw) and raw.replace(" ", "") not in ("constchar*", "constchar*const"):
                self.result_ok(re.sub(r"\*\s*(const)?$", "", raw))   # (the helpers of the structs it is made of)
            if self.u.object_of(raw):
                params.append((f"{self.spell(raw)} {name}", default))
                values.append(f"arg({name})")
            elif re.search(r"\*\s*(const)?$", raw) and raw.replace(" ", "") not in ("constchar*", "constchar*const"):
                # a pointer to data: one value that may be missing, or the first of as many as the count beside it says
                inner = self.spell(re.sub(r"\*\s*(const)?$", "", raw))
                after = ps[i + 1] if i + 1 < len(ps) else None
                counted = after is not None and NUMBER.get(self.u.clean(qtype(after))) == "Int" and re.search(r"count|size|len|num|^n$", after.get("name", ""))
                data = f"std::vector<{inner}>({name}, {name} + {after['name']})" if counted else f"({name} ? std::vector<{inner}>{{*{name}}} : std::vector<{inner}>{{}})"
                params.append((f"const {inner}* {name}", default))
                values.append(f"arg({self.wrap(data, f'std::vector<{inner}>')})")
            else:
                t = self.spell(raw)
                writes = raw.endswith("&") and not raw.endswith("&&") and not re.match(r"const\b", raw)
                params.append(((f"{t}& {name}" if writes else f"const {t}& {name}"), default))
                values.append(f"arg({self.wrap(name, t)})")
                if writes:
                    takes.append((name, t, False))
        ret_raw = qtype(d).split("(")[0].strip()
        ret = "void" if ret_raw == "void" else self.spell(ret_raw)
        if ret_raw != "void" and not self.u.object_of(ret_raw):
            self.result_ok(ret_raw)
        # (its own names begin with kern_: a parameter may be called `out` or `result` - hull_mesh's)
        body = [f'const std::vector<converted::Value> kern_out = converted::engine_call("{f["type"]}", {{{", ".join(values)}}});']
        k = 0
        if ret != "void":
            body.append(f"{ret} kern_result{{}}; converted::take_as<{shape(ret)}>(kern_result, kern_out[0]);")
            k = 1
        for name, t, optional in takes:
            body.append(f"if ({name}) converted::take_as<{shape(t)}>(*{name}, kern_out[{k}]);" if optional else f"converted::take_as<{shape(t)}>({name}, kern_out[{k}]);")
            k += 1
        if ret != "void":
            body.append("return kern_result;")
        const = " const" if cls and (not plain or qtype(d).rstrip().endswith("const")) else ""
        return (f"{ret} {d['name']}({', '.join(a + b for a, b in params)}){const};",
                f"inline {ret} {cls + '::' if cls else ''}{d['name']}({', '.join(a for a, _ in params)}){const} {{ {' '.join(body)} }}")

    def engine_bases(self, q):
        """The classes of the engine the class `q` of the engine derives from that are objects of it too."""
        out = []
        for b in self.tree.records[q].get("bases", []) or []:
            try:
                bq = self.u.find_type(self.u.clean(qtype(b)))
            except Wait:
                bq = None
            if bq is not None and bq in self.tree.records and self.u.engine_object(bq):
                out.append(bq)
        return out

    def stand_in_life(self, f, cls):
        """The constructor (role make) or the destructor (role release) of the stand-in `cls`: (its
        declaration, its definition). The constructor asks the stand-in of the engine for the handle of the
        object it makes, and the object is the source's own: its destructor releases it. (The test's own
        objects of the class - converted::engine_object - and copies are not made by the source: quiet.)"""
        if f["role"] == "release":
            return (f"~{cls}();",
                    f'inline {cls}::~{cls}() {{ if (kern_owned && kern_handle && !converted::g_keeping) converted::engine_call("{f["type"]}", {{arg(*this)}}); }}')
        a, b = self.stand_in(f, None)
        head = re.escape(f["decl"]["name"])
        a = re.sub(r"^void\s+" + head + r"\(", cls + "(", a)
        b = re.sub(r"^inline void\s+" + head + r"\(", f"inline {cls}::{cls}(", b)
        if not b.endswith(" }"):
            raise Wait("a constructor's stand-in")
        b = b[:-2] + ' converted::take_made(*this, kern_out[0]); }'
        b = b.replace("{ const std::vector<converted::Value> kern_out", "{ if (converted::g_keeping) return; const std::vector<converted::Value> kern_out", 1)
        return a, b

    @staticmethod
    def no_stand_in(f, why):
        """A function of the engine the test has no stand-in of (STAND_INS=1 names it with why)."""
        if os.environ.get("STAND_INS"):
            sys.stderr.write(f"no stand-in of {f['type']}: {why}\n")

    def stand_ins(self, pairs):
        """The engine as the test's source sees it: for every class of the engine the source holds an object of,
        a class of that name with the functions the source calls; for every free function it calls, that
        function. (Their declarations where the engine's header has them, their definitions after the helpers
        of the test's structs.)"""
        u, tree = self.u, self.tree
        by_class, free = collections.defaultdict(list), []
        for f in [*u.engine_calls.values(), *(f for i, f in self.more_engine.items() if i not in u.engine_calls)]:
            d = f["decl"]
            owner = tree.owner.get(d["id"])
            if f.get("role") in ("make", "release"):
                by_class[f["class"]].append(f)   # (its constructor and its destructor)
            elif owner and d.get("kind") == "CXXMethodDecl" and d.get("storageClass") != "static":
                by_class[owner].append(f)
            else:
                free.append(f)
        chunks, defs = [], []
        for q in sorted(set(by_class) - u.objects_seen):
            if q in tree.records and not u.engine_object(q):
                # methods of plain data only the engine defines: defined after the test's copy of the struct
                for f in by_class[q]:
                    try:
                        if q not in self.names:
                            raise Wait(f"the class {q.split('::')[-1]} is not written")
                        self.result_ok(q)
                        _, b = self.stand_in(f, self.names[q], plain=True)
                    except Wait as w:
                        self.no_stand_in(f, w)
                        continue   # (what it takes the test has no values of: a function that calls it is not compared)
                    defs.append((("engine", f["decl"]["id"]), b))
        objects = {q for q in u.objects_seen | set(by_class) | self.more_objects | set(self.object_enums) if q in tree.records and u.engine_object(q)}
        for q in sorted(objects):   # (the classes of the engine they derive from: stand-ins too - HostTransport is a Transport)
            for b in self.engine_bases(q):
                objects.add(b)
        for q in sorted(objects):
            name = self.object_name(q)
            pairs.append((self.absolute(q), name))
            decls, made = [], False
            for f in by_class.get(q, []):
                try:
                    a, b = self.stand_in_life(f, name) if f.get("role") else self.stand_in(f, name)
                except Wait as w:
                    self.no_stand_in(f, w)
                    continue   # (what it takes the test has no values of: a function that calls it is not compared)
                made = made or (f.get("role") == "make" and not f["ins"])
                decls.append("  " + a)
                defs.append((("object", q), b))
            if any(f.get("role") == "make" for f in by_class.get(q, [])) and not made:
                decls.insert(0, f"  {name}() = default;")   # (the test's own objects of it - see converted::engine_object)
            over = ", ".join(f"public {self.object_name(b)}" for b in self.engine_bases(q)) or "converted::EngineObject"
            chunks.append((tree.place(tree.records[q]), ("object", q), "\n".join([f"struct {name} : {over} {{", *self.object_enums.get(q, []), *decls, "};"])))
        for f in free:
            d = f["decl"]
            try:
                a, b = self.stand_in(f, None)
            except Wait as w:
                self.no_stand_in(f, w)
                continue
            spaces = [x for x in tree.scope_of.get(d["id"], "").strip(":").split("::") if x][1:]
            head, tail = "".join(f"namespace {x} {{ " for x in spaces), " }" * len(spaces)
            # (the stand-ins of the objects it is handed, declared before it: its header may stand before theirs -
            # `bool upsert(Renderer&, …)` in mesh_data.h, which declares Renderer only)
            named = sorted({self.object_name(q) for q in (u.object_of(qtype(p)) for p in kids(d) if p.get("kind") == "ParmVarDecl") if q in tree.records})
            chunks.append((tree.place(d), ("engine", d["id"]), "".join(f"struct {n}; " for n in named) + head + a + tail))
            defs.append((("engine", d["id"]), head + b + tail))
            pairs.append(("::" + tree.scope_of.get(d["id"], "") + d["name"], "::".join(spaces + [d["name"]])))
        if os.environ.get("STAND_INS"):
            for _, key, text in chunks:
                sys.stderr.write(f"{key}: {text}\n")
            for key, text in defs:
                sys.stderr.write(f"{key}: {text}\n")
        return chunks, defs

    # ---- the test ----
    def chain(self, d, decls):
        """The name of a declaration in the test: with the classes compiled along that it stands in."""
        names = [d["name"]]
        outer = self.inside(d, decls)
        while outer is not None:
            names.insert(0, outer["name"])
            outer = self.inside(outer, decls)
        return "::".join(names)

    def renamer(self, pairs, spaces=()):
        """What is compiled along is the test's own: a name of the program or the engine written out in
        full (`kern::Camera`, `sc::Game::Mode`) means the test's (`Camera`, `Mode`). A namespace the
        source gives another name (`namespace ph = kern::physics;`) is that namespace. (A pair may have a
        third part: what the test's name stands in - the namespace of another file's own pieces, see
        `exports`. `spaces`: the names other files give namespaces.)"""
        table = {}
        other = {}   # kern::physics -> [ph]
        named = [(name, what) for file in [self.path, *self.tree.texts] for name, what in re.findall(r"\bnamespace\s+(\w+)\s*=\s*(?:::)?([\w:]+)\s*;", self.tree.text(file))]
        for name, what in [*named, *spaces]:
            other.setdefault(what if what.split("::")[0] in (ENGINE, self.tree.namespace, *self.tree.spaces) else self.tree.namespace + "::" + what, set()).add(name)
        for absolute, local, *within in pairs:
            parts, keep = absolute.strip(":").split("::"), len(local.split("::"))
            local = "".join(within) + local
            for i in range(len(parts) - keep):
                table.setdefault("::".join(parts[i:]), local)
            if parts[0] in self.tree.spaces:   # (a name in a further namespace of the program, `::fc::V3` too: the test's)
                table.setdefault("::".join(parts), local)
            for k in range(1, len(parts)):
                for name in other.get("::".join(parts[:k]), ()):
                    table.setdefault("::".join([name, *parts[k:]]), local)
        if not table:
            return lambda text: text
        pattern = re.compile(r"(?<![\w:])(?:::)?(" + "|".join(re.escape(k) for k in sorted(table, key=len, reverse=True)) + r")\b")
        using = re.compile(r"\busing\s+(?!namespace\b|enum\b)(?:::)?((?:\w+::)+\w+)\s*;")   # (`using kern::Mat4;` of what is the test's own: nothing to say)
        return lambda text: self.in_code(text, lambda code: pattern.sub(lambda m: table[m.group(1)], using.sub(lambda m: "" if m.group(1) in table else m.group(0), code)))

    # ---- what a test holds: the source of what it compares, and all that source names ----
    @staticmethod
    def ident(d):
        """Where a declaration stands - the same in the tree of every file that reads it: (its file, its
        first byte, its end)."""
        a, b = d.get("_at", (None, None))
        return (os.path.abspath(d.get("_file") or "?"), a, b)

    def at(self, place):
        """The declaration of this file's tree that stands at `place` (see `ident`), or None."""
        if self.by_place is None:
            self.by_place = {}
            for d in [*self.tree.decl.values(), *self.tree.body.values()]:
                if d.get("_file") and d.get("_at", (None, None))[1] is not None:
                    self.by_place.setdefault(self.ident(d), d)
        return self.by_place.get(place)

    def top_of(self, q):
        """The class a type declared in a class is compiled along with: the outermost one the Blueprint
        has as a struct. (In a class the Blueprint does not have, it stands for itself.)"""
        parts = q.split("::")
        for i in range(1, len(parts)):
            if "::".join(parts[:i]) in self.structs:
                return "::".join(parts[:i])
        return q

    def steps_of(self, scope):
        """Where a scope stands in the test: its namespaces (within the program's own or the engine's) and
        its classes, outermost first - [(name, is it a class)]. The test's source stands in the namespaces
        the source does ("mvt::Decoder::" -> mvt, Decoder): two types of one name in two namespaces are
        two types there too, and a name is looked up as C++ looks it up in the source. A class the test
        does not compile along is a namespace of its name: what the source needs of it - types, constants,
        functions that do not use an object - stands there, and is named as the source names it."""
        parts = [x for x in scope.strip(":").split("::") if x]
        out = []
        for i, part in enumerate(parts):
            if i == 0 and part == ENGINE:
                continue
            out.append((part, "::".join(parts[:i + 1]) in self.tree.records))
        return out

    def compiled_top(self, q, records):
        """The class among `records` (those the test compiles along, outermost) whose text holds this type - or
        that is it; None: none."""
        d = self.tree.records.get(q) or self.tree.enums.get(q)
        parts = q.split("::")
        for i in range(1, len(parts) + 1):
            p = "::".join(parts[:i])
            if p in records and (p == q or (d is not None and self.inside(d, [self.tree.records[p]]) is not None)):
                return p
        return None

    def home_of(self, q):
        """The class a type is compiled along with: the outermost one the Blueprint has as a struct whose text
        holds it - or itself, defined outside its class (`struct CityStream::Tile { ... };`)."""
        t = self.top_of(q)
        if t == q:
            return q
        d, top = self.tree.records.get(q) or self.tree.enums.get(q), self.tree.records.get(t)
        if d is None or top is None or self.inside(d, [top]) is not None:
            return t
        parts = q.split("::")
        for i in range(len(parts) - 1, 0, -1):   # (in a class defined outside its own: there)
            p = "::".join(parts[:i])
            if p in self.tree.records and p != t and self.inside(d, [self.tree.records[p]]) is not None:
                return self.home_of(p)
        return q

    def wrapped(self, q, text, local=None):
        """A class's text where it stands in the test: in its namespaces - one defined outside its class
        (`struct CityStream::Tile { ... };`) names that class itself."""
        scope = q.rsplit("::", 1)[0] + "::" if "::" in q else ""
        if self.top_of(q) != q:
            names = self.spaces_of(scope) + ([local] if local else [])
            return "".join(f"namespace {c} {{ " for c in names) + text + " }" * len(names)
        return self.in_classes(scope, text, local)

    def classes_of(self, scope):
        """The names of a scope in the test, outermost first (see steps_of)."""
        return [name for name, _ in self.steps_of(scope)]

    def in_classes(self, scope, text, local=None):
        """`text` where it stands in the test (see steps_of) - with `local`: in the namespace of the pieces of
        another file that are that file's own (see `items`), which stands where its namespace without a name
        stood, in the innermost of its namespaces."""
        names = self.local_path(scope, local)
        return "".join(f"namespace {c} {{ " for c in names) + text + " }" * len(names)

    def local_path(self, scope, local=None):
        steps = self.steps_of(scope)
        names = [n for n, _ in steps]
        if local:
            at = next((i for i, (_, is_class) in enumerate(steps) if is_class), len(steps))
            names = names[:at] + [local] + names[at:]
        return names

    def spaces_of(self, scope):
        """The namespaces of a scope, without its classes: where a function defined there stands."""
        out = []
        for name, is_class in self.steps_of(scope):
            if is_class:
                break
            out.append(name)
        return out

    def in_function(self, d):
        return any(f.decl.get("_file") == d.get("_file") and f.decl["_at"][0] <= d["_at"][0] < f.decl["_at"][1] for f in self.u.functions.values() if f.decl.get("_at") and f.decl["_at"][1])

    def absolute_of(self, d, name):
        return self.tree.absolute(self.tree.scope_of.get(d["id"], "") + name)

    def setup(self):
        """What the test of the whole Blueprint holds (`gather`), and its places guarded - once, for `write`
        and for `exports`."""
        if self.whole is None:
            u, tree = self.u, self.tree
            used = set(re.findall(r"[A-Za-z_]\w*", u.text()))
            self.converted = {f.decl["id"]: f for f in u.order if not f.lifted}
            # (a type a function declares in its body stands in the source of that function: not at the top again)
            self.structs = {q for q, n in u.structs.items() if isinstance(q, str) and n and n in used and q in tree.records and q not in tree.local_types}
            self.struct_tops = {self.top_of(q) for q in self.structs}
            # (a constant in the length of an array or an argument of a template is not in the compiler's tree as
            # a use of it: those are found by their names in the text)
            self.const_named = collections.defaultdict(list)
            for v in tree.decl.values():
                if v.get("kind") == "VarDecl" and v.get("name") and (v.get("constexpr") or re.search(r"\bconst\b", qtype(v))) and not v.get("previousDecl"):
                    self.const_named[v["name"]].append(v)
            self.value_named = collections.defaultdict(set)
            for q, e in tree.enums.items():
                for c in kids(e):
                    if c.get("kind") == "EnumConstantDecl" and c.get("name"):
                        self.value_named[c["name"]].add(q)
            self.whole = self.gather(list(self.converted), sorted(self.structs))
            self.unguarded = self.guard_all([*(f.decl for f in u.order if not f.lifted), *self.whole.functions.values()])
        return self.whole

    def gather(self, roots, classes=()):
        """What the source of the functions `roots` (their declarations' ids) and of the classes `classes`
        names, gathered: all a test must hold to compile it. (Nothing else of the program is in reach of the
        test's source: what it names and the test does not hold does not compile - it can not fall back on
        the program's own 32-bit functions.) What stands in another source file, the test of that file hands
        on: its pieces (`exports`) - where this file's tree has one of them too (a header both read), it is
        this tree's, gathered with the rest."""
        u, tree = self.u, self.tree
        vectors = (ENGINE + "::Vec3", ENGINE + "::Vec2")
        converted = self.converted
        G = Held()
        todo, seen_types = [], set()
        def want_record(q):
            t = self.home_of(q)
            if t != self.top_of(q):
                want_record(self.top_of(q))   # (a class defined outside its class: with that class, after it)
            d = tree.records.get(t)
            if t.startswith(ENGINE + "::logic::"):
                return   # (the engine's logic - how C++ calls a Blueprint: in a test the engine's own, which runs the Blueprints)
            if q in vectors or d is None or t in G.records or d.get("_at", (None, None))[1] is None or t in u.objects_seen or t in G.objects or q in tree.local_types:
                return   # (of the engine's classes with a life of their own the test has stand-ins: see stand_ins)
            G.records.add(t)
            todo.append(("record", d))
        def want_unnamed(i):
            """An enum without a name whose values the source names (they are numbers in the Blueprint): its
            declaration is part of the source."""
            d = tree.decl.get(i)
            if d is not None and d.get("_at", (None, None))[1] is not None:
                G.unnamed[i] = d
        def want_enum(q):
            d = tree.enums.get(q)
            if d is None or not d.get("name") or d.get("_at", (None, None))[1] is None or q in tree.local_types:
                return
            if self.top_of(q) != q:
                want_record(self.home_of(q))
            else:
                G.enums.add(q)
        def want_types(text):
            if text in seen_types:
                return
            seen_types.add(text)
            for word in set(re.findall(r"[A-Za-z_][\w:]*", text)):
                try:
                    q = u.find_type(word)
                except Wait:
                    continue
                if q in tree.records:
                    want_record(q)
                elif q in tree.enums:
                    want_enum(q)
        def want_const(i):
            d = tree.body.get(i) or tree.decl.get(i)
            if d is None or d.get("kind") != "VarDecl" or d["id"] in G.consts or d.get("_at", (None, None))[1] is None:
                return
            G.consts[d["id"]] = d
            todo.append(("const", d))
            first = d
            while first.get("previousDecl") in tree.decl:
                first = tree.decl[first["previousDecl"]]
            owner = tree.scope_of.get(first["id"], "").rstrip(":")
            if owner in tree.records and self.top_of(owner) in self.struct_tops:
                want_record(owner)   # (a constant of a class the Blueprint does not have stands for itself)
        def want_function(i):
            e = tree.template_of.get(i) or tree.body.get(i) or tree.decl.get(i) or {}
            if (tree.owner.get(e.get("id")) or tree.owner.get(i) or tree.scope_of.get(e.get("id"), "")).startswith(ENGINE + "::logic"):
                return   # (the engine's logic: its own in the test - see want_record)
            if i in tree.template_of:
                t = tree.template_of[i]
                if t.get("_at", (None, None))[1] is not None and t["id"] not in G.templates:
                    G.templates[t["id"]] = t
                    todo.append(("function", t))
                return
            d = tree.body.get(i)
            if d is None:
                return take(i)
            if d["id"] in G.functions or d["id"] in u.lambdas or d.get("_at", (None, None))[1] is None:
                return
            if d.get("kind") not in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXConversionDecl"):
                return
            if tree.scope_of.get(d["id"], "").startswith(ENGINE + "::dm::"):
                return   # (the engine's functions of numbers: in the test's source the formula's, see `widen`)
            if ENGINE_NODES and (u.api().declared_only(d) or u.api().declared_only(tree.decl.get(i, d))):
                return   # (a node of the engine - a method of its object, defined in its header or not: the test has its stand-in)
            f = converted.get(d["id"])
            owner = tree.owner.get(d["id"]) or tree.owner.get(i)
            if owner in vectors:
                return   # (what a vector does: converted.h has it, as the nodes do it)
            if f is None and not owner and tree.scope_of.get(d["id"], "").startswith(ENGINE + "::") and d.get("name") in VECTOR_FUNCTIONS and re.search(r"\bVec[23]\b", qtype(d)):
                return   # (a vector's functions: the notation's own - converted.h has them)
            G.functions[d["id"]] = d
            if owner and f is not None and f.self_type is None and not f.ctor and not f.state and self.top_of(owner) not in self.struct_tops:
                # a method that does not use its object, of a class the Blueprint does not have: as a function
                body_at = next((c["_at"][0] for c in kids(d) if c.get("kind") == "CompoundStmt" and c.get("_at")), None)
                head = tree.said(self.cut(d["_file"], d["_at"][0], body_at)) if body_at is not None else ""
                m = re.search(r"(?:\b\w+\s*::\s*)*\b" + re.escape(d["name"]) + r"\s*\(", head)
                if m:
                    close = head.rfind(")")
                    head = re.sub(r"\b(?:static|virtual|inline)\b\s*", "", head[:m.start()]) + d["name"] + "(" + head[m.end():close + 1] + re.sub(r"\b(?:const|override|final)\b", "", head[close + 1:])
                    G.freed[d["id"]] = ("::".join(self.classes_of(owner + "::") + [d["name"]]), head, body_at)
            elif owner:
                want_record(owner)
            todo.append(("function", d))
        def take(i):
            """A function whose body stands in another source file: the pieces its test holds, as the test of
            that file hands them on."""
            decl = tree.decl.get(i)
            if decl is None or decl.get("kind") not in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXConversionDecl"):
                return
            for place in self.closures.get(function_key(tree, decl), ()):
                if place in G.wanted:
                    continue
                G.wanted.add(place)
                piece = self.pieces[place]
                kind = piece["kind"]
                if kind == "object":
                    if piece["q"] in tree.records:
                        G.objects.add(piece["q"])
                    continue
                d = self.at(place)
                if d is None:   # (not in this file's reach: the piece as the other file's test has it)
                    if kind not in ("member", "inside", "engine"):
                        G.foreign[place] = piece
                elif kind in ("function", "inside", "freed"):
                    want_function(d["id"])
                elif kind == "template":
                    if d["id"] not in G.templates:
                        G.templates[d["id"]] = d
                        todo.append(("function", d))
                elif kind == "member":   # (a method declared in its class here, its body there)
                    G.kept.add(d["id"])
                    if tree.owner.get(d["id"]):
                        want_record(tree.owner[d["id"]])
                elif kind == "declaration":
                    G.declared[d["id"]] = d
                elif kind == "record" and d.get("_q"):
                    want_record(d["_q"])
                elif kind == "enum" and d.get("_q"):
                    want_enum(d["_q"])
                elif kind == "unnamed":
                    want_unnamed(d["id"])
                elif kind == "const":
                    for n in piece["names"]:
                        c = self.at(tuple(n))
                        if c is not None:
                            want_const(c["id"])
                elif kind == "engine":
                    G.engine[d["id"]] = d
        def scan(n):
            for x in walk_source(n):
                k = x.get("kind")
                t = x.get("type")
                if isinstance(t, dict) and t.get("qualType"):
                    want_types(t["qualType"])
                    if t.get("desugaredQualType"):
                        want_types(t["desugaredQualType"])
                if x.get("_unnamed") is not None:
                    want_unnamed(x["_unnamed"])
                if k == "DeclRefExpr":
                    r = x.get("referencedDecl", {})
                    if r.get("kind") in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXConversionDecl"):
                        want_function(r["id"])
                    elif r.get("kind") == "VarDecl":
                        want_const(r["id"])
                    elif r.get("kind") == "EnumConstantDecl":
                        want_enum(tree.decl.get(r.get("id"), {}).get("_enum", ""))
                elif k == "MemberExpr" and x.get("referencedMemberDecl"):
                    m = tree.decl.get(x["referencedMemberDecl"], {})
                    if m.get("kind") in ("CXXMethodDecl", "CXXConversionDecl"):
                        want_function(m["id"])
                    elif m.get("kind") == "VarDecl":
                        want_const(m["id"])
                elif k == "CXXConstructExpr":
                    c = constructor_of(tree, x)   # (the constructor a value is made by is part of the source too)
                    if c is not None:
                        want_function(c["id"])
        def scan_record(d):
            for base in d.get("bases", []) or []:
                want_types(base.get("type", {}).get("qualType", ""))
            for c in kids(d):
                if c.get("kind") in ("FieldDecl", "VarDecl"):
                    scan(c)
                elif c.get("kind") == "CXXRecordDecl" and c.get("completeDefinition"):
                    scan_record(c)
        def scan_text(d):
            text = tree.source(d)
            for word in set(re.findall(r"[A-Za-z_]\w*", text)):
                if len(self.value_named.get(word, ())) == 1:
                    want_enum(next(iter(self.value_named[word])))
                found = self.const_named.get(word, ())
                if len(found) > 1:
                    found = [v for v in found if re.search(r"\b" + re.escape(tree.scope_of.get(v["id"], "").rstrip(":").split("::")[-1] or "?") + r"\s*::\s*" + re.escape(word) + r"\b", text)]
                if len(found) == 1:
                    want_const(found[0]["id"])
            for word in set(re.findall(r"([A-Za-z_][\w:]*)\s*::\s*[A-Za-z_]\w*", text)):
                try:
                    q = u.find_type(word)
                except Wait:
                    continue
                if q in tree.enums:
                    want_enum(q)
        for i in roots:
            want_function(i)
        for q in classes:
            want_record(q)
        while todo:
            kind, d = todo.pop()
            scan_record(d) if kind == "record" else scan(d)
            scan_text(d)
        G.records.difference_update(G.objects)
        # (a class or an enum declared in a class that is compiled along stands in that class's text - not for
        # itself as well, as one in a class the Blueprint does not have would: in the namespace of its name)
        def held_by_record(q, d):
            parts = q.split("::")
            return any("::".join(parts[:i]) in G.records and self.inside(d, [tree.records["::".join(parts[:i])]]) is not None for i in range(1, len(parts)))
        G.records = {q for q in G.records if not held_by_record(q, tree.records[q])}
        G.enums = {q for q in G.enums if not held_by_record(q, tree.enums[q])}
        return G

    # ---- the pieces of the test's source ----
    def freed_text(self, d, freed, edits=None, local=None):
        """A method that does not use its object, of a class the Blueprint does not have, as the test has it:
        a function in the namespace of its class's name."""
        _, head, body_at = freed
        owner = self.tree.owner.get(d["id"])
        return self.in_classes(owner + "::", "inline " + head + self.tree.said(self.cut(d["_file"], body_at, d["_at"][1], edits)), local)

    def in_spaces(self, d, text, local=None):
        """A function's text where it stands in the test: in the namespaces its definition stands in (see
        steps_of) - with `local`, in the namespace of another file's own pieces there."""
        names = self.spaces_of(self.tree.scope_of.get(d["id"], "")) + ([local] if local else [])
        return "".join(f"namespace {c} {{ " for c in names) + text + " }" * len(names)

    def function_name(self, d, local=None):
        """How the test names a function that stands in a namespace (see in_spaces)."""
        return "::".join(self.spaces_of(self.tree.scope_of.get(d["id"], "")) + ([local] if local else []) + [d["name"]])

    def enum_text(self, q, local=None):
        return self.in_classes(q.rsplit("::", 1)[0] + "::" if "::" in q else "", self.piece(self.tree.enums[q]) + ";", local)

    def statement_start(self, d):
        """Where the statement that declares a constant begins: `const Vec3 kA{...}, kB = ...;` declares kB
        too, and its text is that statement's as far as kB."""
        raw = self.tree.text(d["_file"])
        a = d["_at"][0]
        if not re.match(r"\s*" + re.escape(d.get("name") or "?") + r"\b", raw[a:a + 256]):
            return a
        before = [v for v in self.tree.decl.values() if v.get("kind") == "VarDecl" and v.get("_file") == d["_file"] and v.get("_at", (None, None))[1] is not None
                  and v["_at"][1] <= a and re.fullmatch(r"\s*,\s*", raw[v["_at"][1]:a])]
        return self.statement_start(before[0]) if before else a

    def const_pieces(self, consts, decls, records, local_of=lambda d: None):
        """The constants as the test has them: their own text. (Several declared in one statement - `constexpr
        f32 kA = 1, kB = 2` - are that statement as far as its last one that is needed.) [(the declaration
        whose text it is, the text, the constants it declares that are needed, the names it gives (pairs))].
        `local_of`: of a declaration, the namespace of another file's own pieces it stands in (see `items`)."""
        tree = self.tree
        by_start, said, pairs = {}, collections.defaultdict(list), collections.defaultdict(list)
        for d in consts.values():
            if self.inside(d, decls) is not None or self.in_function(d):
                continue   # (in a class that is compiled along: there)
            key = (d["_file"], self.statement_start(d))
            if key not in by_start or by_start[key]["_at"][1] < d["_at"][1]:
                by_start[key] = d
            said[key].append(d)
            first = d
            while first.get("previousDecl") in tree.decl:
                first = tree.decl[first["previousDecl"]]
            if d.get("name") and self.top_of(tree.scope_of.get(first["id"], "").rstrip(":")) not in records:
                pairs[key].append((self.absolute_of(first, d["name"]), "::".join(self.local_path(tree.scope_of.get(first["id"], ""), local_of(d)) + [d["name"]])))
        out = []
        for key, d in by_start.items():
            first = d
            while first.get("previousDecl") in tree.decl:
                first = tree.decl[first["previousDecl"]]
            text = tree.said(self.cut(d["_file"], key[1], d["_at"][1])) + ";"
            scope = tree.scope_of.get(first["id"], "")
            if self.top_of(scope.rstrip(":")) not in records:
                # where it stands: a constant of a class the Blueprint does not have in the namespace of that
                # name, by its own name
                text = self.in_classes(scope, re.sub(r"(?:\b[A-Za-z_]\w*\s*::\s*)+(?=" + re.escape(d.get("name", "?")) + r"\b)", "", text, count=1) if first is not d else text, local_of(d))
            out.append((d, text, said[key], pairs[key]))
        return out

    def unnamed_pieces(self, unnamed, decls, records, local_of=lambda d: None):
        """The enums without a name whose values the source names, as the test has them: their own text, like
        a constant's. [(the declaration, the text, the names it gives (pairs))]."""
        out = []
        for i, d in unnamed.items():
            if self.inside(d, decls) is not None or self.in_function(d):
                continue   # (in a class that is compiled along, in a function: there)
            scope = self.tree.scope_of.get(i, "")
            text, named = self.piece(d) + ";", []
            if self.top_of(scope.rstrip(":")) not in records:
                text = self.in_classes(scope, text, local_of(d))
                for c in kids(d):
                    if c.get("kind") == "EnumConstantDecl" and c.get("name"):
                        named.append((self.absolute_of(d, c["name"]), "::".join(self.local_path(scope, local_of(d)) + [c["name"]])))
            out.append((d, text, named))
        return out

    def alias_pieces(self, texts, decls, local_of=lambda d: None):
        """The names the program gives types (`using Bytes32 = ...`), where the source in `texts` uses them:
        [(the declaration, its text, the name it gives (a pair))]."""
        numbers = {"f32", "f64", "u8", "u16", "u32", "u64", "i8", "i16", "i32", "i64"}
        words = set(re.findall(r"[A-Za-z_]\w*", "\n".join(texts)))
        out, taken = [], set()
        for _ in range(4):
            for o, scope in self.tree.alias_decls:
                if o["id"] in taken or o.get("name") in numbers or o.get("name") not in words:
                    continue
                if o.get("_at", (None, None))[0] is None or o["_at"][1] is None or scope.rstrip(":") in self.tree.records or self.inside(o, decls) is not None or self.in_function(o):
                    continue
                taken.add(o["id"])
                text = self.piece(o)
                words.update(re.findall(r"[A-Za-z_]\w*", text))
                out.append((o, self.in_classes(scope, text + ";", local_of(o)), (self.absolute_of(o, o["name"]), "::".join(self.local_path(scope, local_of(o)) + [o["name"]]))))
        return out

    def members_of(self, q, functions, templates, kept=()):
        """What a class declares in itself, each a piece of its own (see `assemble` in `write`) - and of those
        the functions the test does not hold the source of: they are not part of it."""
        tree = self.tree
        d = tree.records[q]
        members = [m for m in walk(d) if m is not d and m.get("kind") in ("CXXMethodDecl", "CXXConstructorDecl", "CXXDestructorDecl", "CXXConversionDecl", "FunctionDecl", "FunctionTemplateDecl")
                   and not m.get("isImplicit") and m.get("_file") == d.get("_file") and m.get("_at", (None, None))[0] is not None and m["_at"][1] is not None and d["_at"][0] <= m["_at"][0] and m["_at"][1] <= d["_at"][1]]
        inner_templates, unconverted = set(), set()
        for m in members:
            if m.get("kind") == "FunctionTemplateDecl":
                inner_templates.update(c["id"] for c in walk(m) if c is not m and "id" in c)
                if m["id"] not in templates:
                    unconverted.add(m["id"])
            elif m["id"] in inner_templates:
                continue
            elif tree.body.get(m["id"], m)["id"] not in functions and not m.get("explicitlyDefaulted") and m["id"] not in kept:
                unconverted.add(m["id"])
        return [m for m in members if m["id"] not in inner_templates], unconverted

    # ---- what a test in another file holds of this file's source ----
    def exports(self):
        """The test of a file that calls a function of this Blueprint holds that function's source once more,
        and all it names (`gather`) - what stands in this source file is not in that file's tree. For every
        function of the Blueprint that is written and has a name the linker knows: the places of the pieces
        its test holds, in the order they must stand ({key: [place]}), and the pieces by their places
        ({place: piece}) - main hands them on to the Blueprints that call these, after this one, and they
        hand on what they took along with their own."""
        self.setup()
        closures, pieces = {}, {}
        for f in self.u.order:
            if f.lifted or not f.key or "@" in f.key:
                continue   # (a function of its file alone - static, in a namespace without a name: no other file calls it)
            order = []
            for place, piece in self.items(self.gather([f.decl["id"]])):
                pieces.setdefault(place, piece)
                order.append(place)
            closures[f.key] = order
        return closures, pieces

    def items(self, G):
        """What `gather` gathered, as pieces another file's test takes: [(place, piece)] in the order they
        stand. A piece: its kind, its text as this test has it (its places guarded, not yet made the
        Blueprint's numbers - the test that takes it does that, `widen`), the names it gives (pairs), the
        Blueprint whose source file it stands in (home), and whether it is that file's own (local - not
        declared in a header: in the test that takes it, it stands in a namespace of that file, so that
        what two files have for themselves is told apart; a function of a header that stands there names
        such pieces by that namespace)."""
        u, tree = self.u, self.tree
        main = os.path.abspath(tree.path)
        home = u.job.replace("/", "__")   # (a part of a Blueprint: Game__game_net_cpp)
        within = f"{home}_local"
        decls = [tree.records[q] for q in sorted(G.records)]
        def here(d):
            return os.path.abspath(d.get("_file") or "?") == main
        def chain(d):
            out = []
            while d.get("previousDecl") in tree.decl:
                d = tree.decl[d["previousDecl"]]
                out.append(d)
            return out
        def outside(d):
            """Is `d` a function another file declares too: the definition of a method of a class of a header,
            or of a function a header declares?"""
            before = chain(d)
            owner = tree.owner.get(d["id"]) or next((tree.owner[x["id"]] for x in before if x["id"] in tree.owner), None)
            return any(not here(x) for x in before) or (owner is not None and not here(tree.records.get(owner, {})))
        def own(d):   # (of this file alone)
            return here(d) and not (d.get("kind") in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXConversionDecl", "FunctionTemplateDecl") and outside(d))
        def namespace_level(d):
            return tree.owner.get(d["id"]) is None and tree.scope_of.get(d["id"], "").rstrip(":") not in tree.records
        def local_of(d):   # (the namespace a piece of this file alone stands in, in the test that takes it)
            return within if own(d) else None
        def outer(q):
            return q.rsplit("::", 1)[0] + "::" if "::" in q else ""
        names = {q: "::".join(self.local_path(q + "::", local_of(tree.records[self.compiled_top(q, G.records)]))) for q in tree.records if self.compiled_top(q, G.records)}
        enum_names = {q: "::".join(self.local_path(outer(q), local_of(tree.enums[q] if q in G.enums else tree.records[self.compiled_top(q, G.records)])) + [q.split("::")[-1]])
                      for q in tree.enums if q in G.enums or self.compiled_top(q, G.records)}
        # the types of this file alone the pieces declare, at the level of a namespace: a function of a header
        # that names one has it by a declaration at the start of its body (its name -> its name in the test)
        own_types = {q.split("::")[-1]: names[q] for q in G.records if own(tree.records[q]) and not any(c for _, c in self.steps_of(outer(q)))}
        own_types.update({q.split("::")[-1]: enum_names[q] for q in G.enums if own(tree.enums[q]) and not any(c for _, c in self.steps_of(outer(q)))})
        sources = [tree.source(d) for d in [*G.functions.values(), *G.templates.values(), *G.consts.values(), *decls]]
        aliases = self.alias_pieces(sources, decls, local_of)
        own_types.update({o["name"]: pair[1] for o, _, pair in aliases if own(o)})
        def qualify(d):
            """A function of a header that stands in this file: what it names of this file alone, by the
            namespace of this file's own pieces."""
            if not here(d) or own(d):
                return []
            raw, edits = tree.text(d["_file"]), []
            owner = tree.owner.get(d["id"]) or next((tree.owner[x["id"]] for x in chain(d) if x["id"] in tree.owner), None)
            for c in kids(d):
                if c.get("kind") not in ("CompoundStmt", "CXXCtorInitializer"):
                    continue
                for n in walk_source(c):
                    if n.get("kind") != "DeclRefExpr" or n.get("_file") != d["_file"]:
                        continue
                    rid = (n.get("referencedDecl") or {}).get("id")
                    t = tree.template_of.get(rid) or tree.decl.get(rid)
                    if t is None or not here(t) or not namespace_level(t) or t.get("kind") not in ("FunctionDecl", "FunctionTemplateDecl", "VarDecl", "EnumConstantDecl") or not own(t):
                        continue
                    if t.get("kind") == "EnumConstantDecl":
                        e = tree.enums.get(t.get("_enum", ""))
                        if e is None or not namespace_level(e):
                            continue
                        said = (t["name"], e.get("name"))
                    else:
                        said = (t.get("name"),)
                    a = n.get("_at", (None,))[0]
                    word = re.match(r"[A-Za-z_]\w*", raw[a:a + 256]) if a is not None else None
                    if word and word.group(0) in said and not raw[max(0, a - 64):a].rstrip().endswith(("::", ".", "->")):
                        scope = tree.scope_of.get((tree.enums[t["_enum"]] if t.get("kind") == "EnumConstantDecl" else t)["id"], "")
                        edits.append((a, 3, 0, a, "::".join(self.local_path(scope, within)) + "::"))
                if c.get("kind") == "CompoundStmt" and raw[c["_at"][0]:c["_at"][0] + 1] == "{":
                    named = set(re.findall(r"[A-Za-z_]\w*", raw[c["_at"][0]:c["_at"][1]]))
                    for t in sorted(set(own_types) & named):
                        if not (owner and (f"{owner}::{t}" in tree.records or f"{owner}::{t}" in tree.enums)):
                            edits.append((c["_at"][0] + 1, 3, 0, c["_at"][0] + 1, f" using {own_types[t]};"))
            return edits
        out = []
        def put(d, piece, mine=None):
            piece.setdefault("pairs", [])
            piece["home"] = home if here(d) else None
            piece["local"] = own(d if mine is None else mine)   # (its text and names stand in the namespace `within` then)
            piece["spaces"] = re.findall(r"\bnamespace\s+(\w+)\s*=\s*(?:::)?([\w:]+)\s*;", tree.text(d["_file"]))
            out.append((tree.place(d), self.ident(d), piece))
        for d in [*G.functions.values(), *G.templates.values()]:
            before = chain(d)
            if d["id"] in G.freed:
                owner = tree.owner.get(d["id"])
                put(d, {"kind": "freed", "text": self.freed_text(d, G.freed[d["id"]], self.in_turn([*self.guard_or_none(d), *qualify(d)]), local_of(d)),
                        "pairs": [(self.absolute(owner) + "::" + d["name"], "::".join(self.local_path(owner + "::", local_of(d)) + [d["name"]]))], "decls": []})
                continue
            if self.inside(d, decls) is not None:
                put(d, {"kind": "inside"})   # (declared in its class with its body: there)
                continue
            piece = {"kind": "template" if d["id"] in G.templates else "function", "text": self.in_spaces(d, self.guarded(d, qualify(d)), local_of(d)), "decls": [self.ident(x) for x in before]}
            if tree.owner.get(d["id"]) is None and d.get("name"):
                piece["pairs"] = [(self.absolute_of(d, d["name"]), self.function_name(d, local_of(d)))]
                first = before[-1] if before else d
                if first is not d and first.get("_at", (None, None))[1] is not None and self.inside(first, decls) is None:
                    put(first, {"kind": "declaration", "text": self.in_spaces(first, self.piece(first) + ";", local_of(d))}, d)
            else:
                for x in before:
                    if x["id"] in tree.owner and x.get("_at", (None, None))[1] is not None:
                        put(x, {"kind": "member"}, d)
            put(d, piece)
        for q in sorted(G.records):
            d = tree.records[q]
            members, unconverted = self.members_of(q, G.functions, G.templates)
            raw, (a0, b0) = tree.text(d["_file"]), d["_at"]
            segments, at = [], a0
            for m in sorted(members, key=lambda m: m["_at"]):
                ma, mb = m["_at"]
                if ma < at:
                    continue   # (in a piece that is already taken)
                end = mb
                while raw[end:end + 1] in (" ", "\t"):
                    end += 1
                if raw[end:end + 1] == ";":
                    end += 1
                segments.append((tree.said(self.cut(d["_file"], at, ma, [])), None, 0))
                text = (self.guarded(m) if m.get("kind") != "FunctionTemplateDecl" else tree.said(self.cut(d["_file"], ma, mb, []))) + tree.said(raw[mb:end])
                segments.append((text, None if m.get("explicitlyDefaulted") else self.ident(m), raw[ma:mb].count("\n")))
                at = end
            segments.append((tree.said(self.cut(d["_file"], at, b0, [])), None, 0))
            inside_of = self.local_path(outer(q), local_of(d)) if self.top_of(q) == q else self.spaces_of(outer(q)) + ([local_of(d)] if local_of(d) else [])
            put(d, {"kind": "record", "segments": segments, "head": "".join(f"namespace {c} {{ " for c in inside_of), "tail": " }" * len(inside_of),
                    "pairs": [(self.absolute(x), names[x]) for x in names if self.compiled_top(x, G.records) == q] + [(self.absolute(x), enum_names[x]) for x in enum_names if self.compiled_top(x, G.records) == q and x not in G.enums]})
        for q in sorted(G.enums):
            put(tree.enums[q], {"kind": "enum", "text": self.enum_text(q, local_of(tree.enums[q])), "pairs": [(self.absolute(q), enum_names[q])]})
        for d, text, said, pairs in self.const_pieces(G.consts, decls, G.records, local_of):
            put(d, {"kind": "const", "text": text, "names": [self.ident(x) for x in said], "pairs": pairs,
                    "actors": ["::".join(self.local_path(tree.scope_of.get(x["id"], ""), local_of(x)) + [x["name"]]) for x in said if self.actor_blueprint(qtype(x))]})
        for d, text, named in self.unnamed_pieces(G.unnamed, decls, G.records, local_of):
            put(d, {"kind": "unnamed", "text": text, "pairs": named})
        for o, text, pair in aliases:
            put(o, {"kind": "alias", "text": text, "pairs": [pair]})
        # what of the engine the pieces call and hold: the test that takes them has its own stand-ins
        for d in self.engine_called(G).values():
            if d.get("_at", (None, None))[1] is not None:
                out.append(((-1, 0), self.ident(d), {"kind": "engine"}))
        words = set(re.findall(r"[A-Za-z_]\w*", "\n".join([*sources, *(p.get("text", "") for _, _, p in out)])))
        for q in sorted(u.objects_seen | G.objects):
            if q.split("::")[-1] in words:
                out.append(((-1, 0), ("object", q), {"kind": "object", "q": q}))
        # what this file took of other files: after what its headers declare, before its own source
        first = min((p for p, place, _ in out if place[0] == main), default=(10 ** 9 + 1, 0))
        out += [((first[0], first[1] - 0.5, i), place, piece) for i, (place, piece) in enumerate(G.foreign.items())]
        out.sort(key=lambda x: x[0])
        return [(place, piece) for _, place, piece in out]

    def engine_called(self, G):
        """The engine's functions the source the test holds calls - what only the engine can do (a node):
        {id: declaration}."""
        u, tree = self.u, self.tree
        out = dict(G.engine)
        if not ENGINE_NODES:
            return out
        for d in G.functions.values():
            for x in walk_source(d):
                for i in (x.get("referencedDecl", {}).get("id"), x.get("referencedMemberDecl")):
                    e = tree.decl.get(i) if i else None
                    if e is not None and i not in out and (i in u.engine_calls or u.api().declared_only(e)):
                        out[i] = e
        return out

    def guard_or_none(self, d):
        try:
            return self.guard(d)[1]
        except Wait:
            return []

    def write(self):
        u, tree = self.u, self.tree
        src = tree.said(tree.text(self.path))
        stem = u.job.replace("/", "__")   # (the test is the Blueprint's - or a part's (Game/game_net_cpp: Game__game_net_cpp): named like it)
        skipped = []
        G = self.setup()
        unguarded = self.unguarded
        ns = f"converted_{stem}"
        vectors = (ENGINE + "::Vec3", ENGINE + "::Vec2")
        converted = self.converted
        functions, records, enums, templates = G.functions, G.records, G.enums, G.templates
        top_of, classes_of, in_classes, absolute = self.top_of, self.classes_of, self.in_classes, self.absolute_of

        # the names of the test's own types
        decls = [tree.records[q] for q in sorted(records)]
        for q in tree.records:
            if self.compiled_top(q, records):
                self.names[q] = "::".join(classes_of(q + "::"))
        for q in tree.enums:
            if q in enums or self.compiled_top(q, records):
                self.enum_names[q] = "::".join(classes_of(q.rsplit("::", 1)[0] + "::" if "::" in q else "") + [q.split("::")[-1]])
        top = sorted(records)
        types_at_top = [self.names[q] for q in top] + [self.enum_names[q] for q in enums]
        if len(set(types_at_top)) != len(types_at_top):
            twice = sorted({n for n in types_at_top if types_at_top.count(n) > 1})
            return None, [(f, f"two types of one name ({', '.join(twice)})") for f in u.order if not f.lifted]
        pairs = [(self.absolute(q), self.names[q]) for q in self.names] + [(self.absolute(q), self.enum_names[q]) for q in self.enum_names]
        for q in self.enum_names:   # (the values of an enum that is not scoped stand in the scope around it: `fc::kFaultNone` is the test's kFaultNone)
            if tree.enums[q].get("scopedEnumTag"):
                continue
            outer = q.rsplit("::", 1)[0] + "::" if "::" in q else ""
            pairs += [(tree.absolute(outer + c["name"]), "::".join(classes_of(outer) + [c["name"]])) for c in kids(tree.enums[q]) if c.get("kind") == "EnumConstantDecl" and c.get("name")]
        pairs += [(f"::{ENGINE}::Vec3", "Vec3"), (f"::{ENGINE}::Vec2", "Vec2"), (f"::{ENGINE}::Span", "Span"), (f"::{tree.namespace}::Vec3", "Vec3"), (f"::{tree.namespace}::Vec2", "Vec2")]
        pairs += [(f"::{space}::{n}", n) for space in (ENGINE, tree.namespace) for n in ("f32", "f64", "u8", "u16", "u32", "u64", "i8", "i16", "i32", "i64")]
        # (by their full names: a local variable of a function may have the name of one - `const f32 length = kern::length(d)`)
        pairs += [(f"::{ENGINE}::{n}", n, "converted::wide::") for n in ("dot", "cross", "length", "length_sq", "distance", "distance_sq", "finite", "lerp", "vmin", "vmax", "normalize", "hex_color", "Span", "Map", "Set")]
        # (an enum of a class of the engine the test has a stand-in of - logic::Runtime::Chain - stands in that stand-in)
        self.object_enums.clear()
        in_objects = set()
        for q in sorted(enums):
            owner = q.rsplit("::", 1)[0] if "::" in q else ""
            if owner in tree.records and u.engine_object(owner):
                self.object_enums[owner].append("  " + self.piece(tree.enums[q]) + ";")
                in_objects.add(q)
        enum_chunks = [(tree.place(tree.enums[q]), ("enum", q), self.enum_text(q)) for q in sorted(enums) if q not in in_objects]
        const_chunks, said_consts = [], set()
        for d, text, _, named in self.const_pieces(G.consts, decls, records):
            pairs += named
            if self.widen(text) not in said_consts:   # (the program and the engine both say what pi is: once)
                said_consts.add(self.widen(text))
                const_chunks.append((tree.place(d), ("const", f"{os.path.basename(d['_file'])}:{d['_at'][0]}"), text))
        # the enums without a name whose values the source names: their own text, like a constant's
        for d, text, named in self.unnamed_pieces(G.unnamed, decls, records):
            pairs += named
            if self.widen(text) not in said_consts:
                said_consts.add(self.widen(text))
                const_chunks.append((tree.place(d), ("const", f"{os.path.basename(d['_file'])}:{d['_at'][0]}"), text))
        # the functions
        fn_chunks, cases, outs, case_of = [], [], [], {}
        for d in [*functions.values(), *templates.values()]:
            if d["id"] in G.freed:
                fn_chunks.append((tree.place(d), ("function", d["id"]), self.freed_text(d, G.freed[d["id"]])))
                pairs.append((self.absolute(tree.owner.get(d["id"])) + "::" + d["name"], G.freed[d["id"]][0]))
                continue
            if self.inside(d, decls) is not None:
                continue   # (declared in its class with its body: there)
            fn_chunks.append((tree.place(d), ("function", d["id"]), self.in_spaces(d, self.piece(d))))
            if tree.owner.get(d["id"]) is None and d.get("name"):
                first = d
                while first.get("previousDecl") in tree.decl:
                    first = tree.decl[first["previousDecl"]]
                if first is not d and first.get("_at", (None, None))[1] is not None and self.inside(first, decls) is None:
                    # (declared before it is defined - in its header, as a rule: what stands in between may call it)
                    fn_chunks.append((tree.place(first), ("function", d["id"]), self.in_spaces(first, self.piece(first) + ";")))
                pairs.append((absolute(d, d["name"]), self.function_name(d)))
        for d in G.declared.values():   # (a function declared here, its body in another file's pieces)
            fn_chunks.append((tree.place(d), ("declaration", d["id"]), self.in_spaces(d, self.piece(d) + ";")))
            pairs.append((absolute(d, d["name"]), self.function_name(d)))
        # what stands in other source files, as their tests hand it on (`exports`): after what this file's
        # headers declare, before this file's own source - a constant both say once
        main = os.path.abspath(tree.path)
        first = min((tree.place(d) for d in [*functions.values(), *templates.values(), *G.consts.values(), *decls, *(tree.enums[q] for q in enums), *G.declared.values()]
                     if os.path.abspath(d.get("_file") or "?") == main), default=(10 ** 9 + 1, 0))
        foreign_chunks = []
        for i, (place, piece) in enumerate(G.foreign.items()):
            if piece["kind"] in ("const", "unnamed"):
                if self.widen(piece["text"]) in said_consts:
                    continue
                said_consts.add(self.widen(piece["text"]))
            foreign_chunks.append(((first[0], first[1] - 0.5, i), ("foreign", place), None))
        # The Blueprints of the level's actors the source calls on them (kern::logic::ActorBlueprint): the test runs
        # them (converted::ActorWorld) - for the source, bound to the objects it calls them by, and for the
        # Blueprint, where actor_find finds them.
        actors = sorted({a for f in u.order for a in actors_called(f)})
        bindings = []
        for v in G.consts.values():
            first = v
            while first.get("previousDecl") in tree.decl:
                first = tree.decl[first["previousDecl"]]
            if v.get("name") and self.actor_blueprint(qtype(v)) and self.inside(v, [tree.records[q] for q in sorted(G.records)]) is None:
                bindings.append("::".join(classes_of(tree.scope_of.get(first["id"], "")) + [v["name"]]))
        bindings += [name for p in G.foreign.values() for name in p.get("actors", ())]
        engine_too = bool(actors) and bool(u.engine_calls or self.engine_called(G))
        for f in u.order:
            d = f.decl
            if f.lifted:
                continue   # (a local function: in the source of the function it stands in, compared with it)
            if engine_too:
                skipped.append((f, "its Blueprint calls the engine and the Blueprint of an actor of the level: the world that runs the actor's Blueprint has the engine itself, not the test's stand-in"))
                continue
            owner = tree.owner.get(d["id"])
            free = G.freed.get(d["id"], (None,))[0]
            if free:
                owner = None
            elif owner and owner not in self.names:
                skipped.append((f, "a method of a class that is not written"))
                continue
            if not f.out_fields:
                skipped.append((f, "it gives nothing"))
                continue
            if f.indexes and d["id"] in unguarded:
                skipped.append((f, f"it reads a list at a place, shifts bits or divides whole numbers by an amount it computes, {unguarded[d['id']]}: C++ leaves open what that gives outside the range the function is made for, and the test cannot tell where that is"))
                continue
            try:
                params = [p for p in kids(d) if p.get("kind") == "ParmVarDecl"]
                decl_lines, names, types = [], [], []
                # (what the Blueprint is given is said before the call: the drawn values are then handed over to
                # it - moved, as a value that may only be moved (std::unique_ptr) must be - and what the call
                # made of them is compared)
                moved = set()
                static = d.get("storageClass") == "static" or tree.decl.get(d.get("previousDecl"), {}).get("storageClass") == "static"
                if f.state:
                    # a function of the Blueprint of a state: called on an object of the class whose fields that are
                    # variables are drawn - the Blueprint's variables start from the same values (converted::Variables)
                    state = self.state_helpers()
                    decl_lines.append(f"s = {state['draw']}(draw);")
                if owner and f.self_type is not None and not f.ctor and not static:
                    types.append(self.names[owner])
                    decl_lines.append(f"{types[-1]} a0 = {self.helpers(owner)}(draw);")
                    moved.add("a0")
                    names.append("a0")
                first = len(names)
                held_lists = []   # (the lists the source gets a pointer into: the list and the place - Unit.pointed)
                for p in params:
                    if self.u.pointed(qtype(p)) is not None:
                        entry = re.sub(r"\s*\*$", "", qtype(p).strip())
                        inner = self.spell(entry)
                        lst, at = f"a{len(names)}", f"a{len(names) + 1}"
                        types += [f"std::vector<{inner}>", "long long"]
                        # (never empty - an empty list's data() may be null -; now and then null itself, the place -1)
                        n_fixed = f.fixed.get(len(names))   # (the list it points into has a fixed length: that many entries)
                        decl_lines += [f"std::vector<{inner}> {lst}({n_fixed}); for (auto& e : {lst}) e = {self.draw(entry)};" if n_fixed else
                                       f"std::vector<{inner}> {lst} = draw_list(draw, [&]() -> {inner} {{ return {self.draw(entry)}; }}); {lst}.push_back({self.draw(entry)});",
                                       f"const long long {at} = draw.below(8) == 0 ? -1 : static_cast<long long>(draw.below(static_cast<u32>({lst}.size()) + 1));"]
                        names += [lst, at]
                        held_lists.append(lst)
                        continue
                    types.append(self.spell(qtype(p)))
                    decl_lines.append(f"{types[-1]} a{len(names)} = {self.draw(qtype(p))};")
                    if not types[-1].startswith("const ") and not types[-1].endswith("&"):
                        moved.add(f"a{len(names)}")
                    names.append(f"a{len(names)}")
                if held_lists:
                    self.pointed = True
                    decl_lines.append("converted_pointed_lists = {" + ", ".join(f"{{reinterpret_cast<const unsigned char*>({x}.data()), reinterpret_cast<const unsigned char*>({x}.data() + {x}.size())}}" for x in held_lists) + "};")
                ret = qtype(d).split("(")[0].strip()
                if f.value is not None:
                    self.result_ok(ret)
                if owner and not f.state:
                    self.helpers(owner) if (f.mutates or f.ctor) else None
                # (what the function takes by value is handed over, as a call hands a value it does not need
                # again: the count of what the function asks for memory is of its own work - see converted.h)
                # (a view of a list - kern::Span - shows the list the test keeps: it is not handed over)
                by_value = lambda p: not any(ch in qtype(p) for ch in "&*") and not self.u.object_of(qtype(p)) and not re.match(r"(?:kern::)?Span<", self.u.clean(qtype(p)))
                # (a list the source writes in braces - std::initializer_list - is the drawn list as such a list: converted::listed)
                braced, handed, at_ = {}, [], first
                for p in params:
                    if self.u.pointed(qtype(p)) is not None:   # (the pointer: into the list the run was given, at its place)
                        handed.append(f"({names[at_ + 1]} < 0 ? nullptr : {names[at_]}.data() + {names[at_ + 1]})")
                        at_ += 2
                        continue
                    n = names[at_]
                    if self.u.clean(qtype(p)).startswith("std::initializer_list<"):
                        braced[n] = f"l{n[1:]}"
                    handed.append(braced[n] if n in braced else f"std::move({n})" if by_value(p) else n)
                    at_ += 1
                args = ", ".join(handed)
                if owner and f.self_type is None and not f.ctor and not static and not f.state:
                    self.helpers(owner)   # (a method that does not use its object: called on one as its class makes it)
                here = f"{ns}::"   # (by its full name: the test's own function, whatever its arguments are)
                target = (f"{self.names[owner]}({args})" if f.ctor else f"{self.names[owner]}::{d['name']}({args})" if owner and static
                          else f"s->{d['name']}({args})" if f.state and owner
                          else f"a0.{d['name']}({args})" if owner and f.self_type is not None else f"{self.names[owner]}{{}}.{d['name']}({args})" if owner else f"{here}{free or self.function_name(d)}({args})")
                parts = {"self": "a0", "result": "r"}   # (what the call changed: run's own values, handed back as they are)
                if f.state:
                    parts["state"] = f"{state['struct']}{{s.get()}}"   # (the object after the call: all its variables are compared)
                for place_ in f.refs:
                    parts[("ref", place_)] = f"a{place_}"
                out_lines = []
                want_type = ""   # (how the test spells what the call gives, see `wrap`)
                # the function's own call is counted: how often it asks for memory (converted::counted)
                counted = f"converted::counted([&] {{ return {target}; }})"
                for n, t in reversed([(n, t) for n, t in zip(names, types) if n in braced]):
                    counted = f"converted::listed({n}, [&](std::initializer_list<{t[len('std::vector<'):-1]}> {braced[n]}) {{ return {counted}; }})"
                if f.ctor:
                    give = f"return {counted};"
                elif len(f.out_fields) > 1:
                    fields = [(n, types[0] if src_ == "self" else state["struct"] if src_ == "state" else self.spell(ret) if src_ == "result" else types[src_[1]]) for n, _, src_ in f.out_fields]
                    out_lines = self.plain_struct(f"Out_{f.name}", fields)
                    give = f"{'auto r = ' if f.value is not None else ''}{counted}; return Out_{f.name}{{{', '.join(f'std::move({parts[src_]})' if src_ != "state" else parts[src_] for _, _, src_ in f.out_fields)}}};"
                else:
                    src_ = f.out_fields[0][2]
                    give = f"return {counted};" if src_ == "result" else f"{counted}; return {parts[src_]};"
                    want_type = types[0] if src_ == "self" else "" if src_ == "state" else self.spell(ret) if src_ == "result" else types[src_[1]]
            except Wait as w:
                skipped.append((f, str(w)))
                continue
            # where the Blueprint's call has what is compared: what it returns, then what it made of its
            # reference inputs that are not const, in their order (Instance::call)
            # (of a function of the Blueprint of a state: then its variables, as converted::Tally reads them)
            layout = ((["result"] if f.value is not None else []) + (["self"] if f.mutates and not f.ctor and not f.state else []) + [("ref", place_) for place_ in f.refs]
                      + (["state"] if f.state and f.mutates else []))
            order = [layout.index(src_) for _, _, src_ in f.out_fields] if len(f.out_fields) > 1 else []
            # the arguments as the Blueprint function has its inputs (those by reference first)
            given = f.order() if len(names) == len(f.params) else list(range(len(names)))
            given_args = ", ".join(f"arg({self.wrap(names[i], types[i])})" for i in given)
            case_of[d["id"]] = f
            outs.append((("case", d["id"]), "\n".join(out_lines)))
            cases.append((("case", d["id"]), "\n".join([
                f"  {{  // {f.cpp}",
                f'    converted::Tally tally("{f.name}");',
                "    converted::Draw draw;   // (every function its own arguments, the same in every run)",
                *([f"    std::unique_ptr<{self.names[u.state]}> s;   // (the object of the state: drawn for every call)"] if f.state else []),
                f"    const auto run = [&]({', '.join(f'[[maybe_unused]] const {t}& {n}' if n in held_lists else f'[[maybe_unused]] {t} {n}' for t, n in zip(types, names))}) {{ {give} }};",
                "    for (int i = 0; tally.more(i); ++i) {",
                *[f"      {x}" for x in decl_lines],
                f"      const std::vector<converted::Value> given = {{{given_args}}};   // (what the Blueprint is given: before the call)",
                *([f"      const converted::Variables vars = {state['variables']}(*s);   // (and the variables of its state)"] if f.state else []),
                "      const converted::Watch watch;",
                "      if (setjmp(converted::g_bail) != 0) { converted::may_bail(false); tally.given_up(); continue; }   // (given up: see converted::open_read)",
                "      converted::g_can_bail = true;",
                "      converted::may_bail(true);",
                "      bp.before_source();   // (the Blueprints of the actors it calls, as they started)",
                # (what the source gave, kept: as the Blueprint holds it, a view of it may be what is compared)
                f"      const auto made = run({', '.join(f'std::move({n})' if n in moved else n for n in names)});",
                f"      const auto& want = {self.wrap('made', want_type)};" if self.wrap('made', want_type) != 'made' else "      const auto& want = made;",
                "      converted::g_can_bail = false;",
                "      converted::may_bail(false);",
                f"      tally.take(bp, given, want, watch.left(), [&] {{ return converted::shown_all(given); }}, {{{', '.join(str(k) for k in order)}}}"
                + (f", &vars, {'true' if f.mutates else 'false'}" if f.state else "") + ");",
                "    }",
                "  }"])))
        if not cases:
            return None, skipped
        system, own = ["#include <cfenv>", "#include <cmath>", "#include <cstdio>", "#include <string>", "#include <type_traits>", "#include <vector>"], []
        if self.pointed:
            system.append("#include <cstdint>")
        for m in re.finditer(r'^#include\s+(["<])([^">]+)[">]', src, re.M):
            if m.group(1) == "<":
                if f"#include <{m.group(2)}>" not in system:
                    system.append(f"#include <{m.group(2)}>")
            elif m.group(2).startswith("kern/"):
                own.append(f'#include "{m.group(2)}"')
            else:
                own.append(f'#include "{self.source_dir}/{m.group(2)}"')
        # of the file's own `using` lines only those of the standard library: the program's names are the test's own here
        usings = [m.group(0).strip() for m in re.finditer(r"^using\s+[^;=\n]+;", src, re.M) if re.match(r"using\s+(?:namespace\s+)?std\b", m.group(0))]
        # what a class declares in itself: a piece of its own (see `assemble`). A function of the class that
        # is not converted is not part of the test's source.
        members, unconverted = {}, set()
        for q in top:
            members[q], left_out = self.members_of(q, functions, templates, G.kept)
            unconverted.update(("member", i) for i in left_out)
        # the names the program gives types (`using Bytes32 = ...`), where the test's source uses them
        foreign_texts = [p.get("text") or "".join(t for t, _, _ in p.get("segments", ())) for p in G.foreign.values()]
        texts = [text for _, _, text in [*enum_chunks, *const_chunks, *fn_chunks]] + [self.piece(tree.records[q]) for q in top] + foreign_texts
        alias_chunks = []
        for o, text, pair in self.alias_pieces(texts, decls):
            alias_chunks.append((tree.place(o), ("alias", o["id"]), text))
            pairs.append(pair)
        # what the source the test holds calls of the engine - the functions of other Blueprints too, and the
        # pieces of other files (as this file's tree declares it): the stand-ins have it
        self.more_engine, self.more_objects = {}, set(G.objects)
        for i, d in self.engine_called(G).items():
            if i not in u.engine_calls:
                try:
                    self.more_engine[i] = u.api().function(d)
                except Wait:
                    pass
        object_chunks, object_defs = self.stand_ins(pairs)
        rename = self.renamer(pairs)
        wide = lambda text: rename(self.widen(text))
        # the pieces of other files: named as there - a file's own names first, see `exports`
        outer = [pair for p in G.foreign.values() if not p["local"] for pair in p["pairs"]]
        named_spaces = [tuple(s) for p in G.foreign.values() for s in p.get("spaces", ())]
        renamers = {}
        def wide_of(piece):
            h = piece.get("home")
            if h not in renamers:
                mine = [pair for p in G.foreign.values() if p["local"] and p.get("home") == h for pair in p["pairs"]] if h else []
                renamers[h] = self.renamer([*mine, *pairs, *outer], named_spaces)
            return lambda text: renamers[h](self.widen(text))
        # a name of the program or the engine that is still written out in full: not the test's own
        # (a further namespace of the program - fc:: - stands in the test as the source has it, like one inside
        # the program's own: only from outside every namespace - ::fc:: - it names the program's; kern::logic is
        # the real engine in the test)
        foreign = re.compile(r"(?<![\w:])(?:(?:::)?(?:" + re.escape(tree.namespace) + "|" + re.escape(ENGINE) + r")" + "".join(f"|::{re.escape(x)}" for x in tree.spaces) + r")::(?!math::|logic::)(\w+)")
        def is_record(key):
            return key[0] == "record" or (key[0] == "foreign" and G.foreign[key[1]]["kind"] == "record")

        # all of it in the order of the translation unit: what a piece uses stands before it there
        source = sorted([*enum_chunks, *[(tree.place(tree.records[q]), ("record", q), None) for q in top], *const_chunks, *fn_chunks, *alias_chunks, *object_chunks, *foreign_chunks], key=lambda c: c[0])

        def assemble(dropped):
            """The test without the pieces in `dropped`, and the lines each of its pieces has."""
            lines, where = [], []   # where: (first line, last line, piece), a piece inside a piece after it
            def add(key, text):
                first = len(lines) + 1
                lines.extend(text.split("\n"))
                if key is not None:
                    where.append((first, len(lines), key))
                return first
            add(None, "\n".join([
                f"// Generated by tools/cpp_to_blueprint.py from {os.path.basename(self.path)} - do not edit. The Blueprint \"{u.name}\" of the level is this",
                "// file's C++, converted. Here the source of every converted function stands once more, compiled with the numbers",
                "// and the mathematics of the Blueprint language; the Blueprint must give exactly what it gives (../converted.h).",
                *system, *own, '#include "../converted.h"', "",
                f"namespace {ns} {{", f"namespace converted = ::{tree.namespace}::converted;",
                "using converted::arg; using converted::same; using converted::defined; using converted::text; using converted::take;",
                WIDE_PRELUDE, *([POINTED_PRELUDE] if self.pointed else []), *usings, "",
                f"// ---- {os.path.basename(self.path)} and what it uses, as it stands ----",
                '#pragma GCC diagnostic push', '#pragma GCC diagnostic ignored "-Wunused-function"', '#pragma GCC diagnostic ignored "-Wunused-const-variable"',
                '#pragma GCC diagnostic ignored "-Wunused-variable"', '#pragma GCC diagnostic ignored "-Wnarrowing"', '#pragma GCC diagnostic ignored "-Wunused-but-set-variable"',
                "#define private public", "#define protected public"]))
            for _, key, text in source:
                if key in dropped:
                    continue
                if key[0] == "foreign":
                    piece = G.foreign[key[1]]
                    there = wide_of(piece)   # (a piece of another file alone stands in its namespace: its text says so - see `items`)
                    if piece["kind"] != "record":
                        add(key, there(piece["text"]))
                        continue
                    text, inner, lines_ = "", [], 0
                    for part, place, newlines in piece["segments"]:
                        if place is not None and (place not in G.wanted or ("member", place) in dropped):
                            text += "\n" * newlines
                        else:
                            if place is not None:
                                inner.append((text.count("\n"), text.count("\n") + part.count("\n"), ("member", place)))
                            text += part
                    first = add(key, piece["head"] + re.sub(r"^\s*class\b", "struct", there(text)) + ";" + piece["tail"])
                    where.extend((first + x, first + y, k) for x, y, k in inner)
                    continue
                if key[0] != "record":
                    add(key, wide(text))
                    continue
                q = key[1]
                d = tree.records[q]
                a0, b0 = d["_at"]
                raw = tree.text(d["_file"])
                text, at, inner = "", a0, []
                for m in sorted(members[q], key=lambda m: m["_at"]):
                    ma, mb = m["_at"]
                    if ma < at:
                        continue   # (in a piece that is already taken)
                    if ("member", m["id"]) in dropped:
                        text += self.cut(d["_file"], at, ma) + "\n" * raw[ma:mb].count("\n")
                        at = mb
                        while raw[at:at + 1] in (" ", "\t"):
                            at += 1
                        if raw[at:at + 1] == ";":
                            at += 1
                    else:
                        inner.append((raw[a0:ma].count("\n"), raw[a0:mb].count("\n"), ("member", m["id"])))
                text += self.cut(d["_file"], at, b0)
                first = add(("record", q), self.wrapped(q, re.sub(r"^\s*class\b", "struct", wide(tree.said(text))) + ";"))
                where.extend((first + x, first + y, key) for x, y, key in inner)
            add(None, "\n".join(["#undef private", "#undef protected", '#pragma GCC diagnostic pop', ""]))
            for q, hl in self.helped.items():
                if hl and ("helper", q) not in dropped and not isinstance(q, str):
                    add(("helper", q), "\n".join(hl))   # (of the state: see state_helpers)
                elif hl and ("helper", q) not in dropped:
                    made = self.made.get(q)
                    hl = hl if not made or ("member", made[0]) not in dropped else [hl[0], made[1], *hl[2:]]
                    # (where the class's compiled text stands: its helpers next to it, for a call from converted.h)
                    home = self.compiled_top(q, records) or top_of(q)
                    outer_scope = home.rsplit("::", 1)[0] + "::" if "::" in home else ""
                    # (of a class defined outside its class - struct CityStream::Tile: the namespaces that class stands in)
                    spaces = self.spaces_of(outer_scope) if top_of(home) != home else classes_of(outer_scope)
                    if spaces:   # (of a class in a namespace: there, where a call from converted.h looks for them)
                        hl = [*hl[:-5], "".join(f"namespace {c} {{ " for c in spaces) + "using converted::arg; using converted::same; using converted::defined; using converted::text; using converted::take;",
                              *hl[-5:], "}" * len(spaces)]
                    add(("helper", q), "\n".join(hl))
            for key, text in object_defs:
                if key not in dropped:
                    add(key, wide(text))
            for key, text in outs:
                if key not in dropped and text:
                    add(key, text)
            add(None, "\n".join(["", f"SC_TEST(converted_{stem}) {{",
                                  f'  converted::Blueprint bp("{u.name}"' + (", {" + ", ".join(text_literal(a) for a in actors) + "}" if actors else "") + ");"]))
            for b in dict.fromkeys(bindings):
                if ("bind", b) not in dropped:   # (the source calls the actor's Blueprint as the program binds it)
                    add(("bind", b), f"  {b}.running = bp.source_actor({b}.actor);")
            for key, text in cases:
                if key not in dropped:
                    add(key, text)
            add(None, "\n".join(["}", "", f"}}  // namespace {ns}", ""]))
            return "\n".join(lines), where

        dropped, why = set(unconverted), {}
        for _ in range(400):
            text, where = assemble(dropped)
            if not any(key not in dropped for key, _ in cases):
                break
            # what the source still names of the program in full is not the test's: such a piece goes, like one that does not compile
            errors, lines = [], "".join(part if i % 2 == 0 else re.sub(r"[^\n]", " ", part) for i, part in enumerate(self.code_and_rest(text))).split("\n")
            for first, last, key in where:
                if key[0] in ("helper", "case") or is_record(key):
                    continue
                for at in range(first, last + 1):
                    m = foreign.search(lines[at - 1])
                    if m:
                        errors.append((at, f"it names {m.group(0).lstrip(':')}, which the test does not hold"))
            for first, last, key in where:
                if is_record(key) and not errors:
                    for at in range(first, last + 1):
                        m = foreign.search(lines[at - 1])
                        if m:
                            errors.append((at, f"it names {m.group(0).lstrip(':')}, which the test does not hold"))
            errors = errors or self.compiles(text, stem)
            if not errors:
                break
            found = {}
            for line, message in errors:
                piece = None
                for first, last, key in where:
                    if first <= line <= last and (piece is None or key[0] == "member"):
                        piece = key
                if piece is None:
                    done = [case_of[key[1]] for key, _ in cases if key not in dropped]
                    return None, skipped + [(f, "its source does not compile with a Blueprint's numbers yet: " + message) for f in done]
                found.setdefault(piece, message)
            # a piece of the source that does not compile may be why later ones do not: the first one goes - and with
            # it every other one whose complaint names nothing of a piece that does not compile either (a complaint
            # of its own, not one that follows from another: it would go in a later round all the same)
            only_cases = all(k[0] == "case" for k in found)
            early = min(first for first, last, key in where if key in found and (only_cases or key[0] != "case"))
            def named_by(key):
                d = tree.decl.get(key[1]) if isinstance(key[1], str) else None
                n = (d or {}).get("name") or (key[1].split("::")[-1] if isinstance(key[1], str) else "")
                return n if n and re.fullmatch(r"\w+", n) else None
            failing = {n for n in (named_by(k) for k in found) if n}
            def own_complaint(key, message):
                mine = named_by(key)
                return key[0] != "case" and not any(re.search(rf"\b{re.escape(n)}\b", message) for n in failing if n != mine)
            for key, message in found.items():
                if only_cases or (key[0] != "case" and (min(first for first, last, k in where if k == key) == early or own_complaint(key, message))):
                    dropped.add(key)
                    why[key] = message
                    if key[0] == "function":
                        # a method whose definition goes: its declaration in its class goes with it (what calls it
                        # then does not compile and goes too - instead of compiling and not linking), and its own case
                        before = tree.decl.get(key[1], {}).get("previousDecl")
                        while before:
                            dropped.add(("member", before))
                            why[("member", before)] = message
                            before = tree.decl.get(before, {}).get("previousDecl")
                        if any(k == ("case", key[1]) for k, _ in cases):
                            dropped.add(("case", key[1]))
                            why[("case", key[1])] = message
                    elif key[0] == "foreign":
                        # the same for a function of another file: its declarations - here, or in that file's pieces
                        for place in G.foreign[key[1]].get("decls", ()):
                            d = self.at(place)
                            for k in ([("member", place), ("foreign", place)] if d is None else [("member", d["id"]), ("declaration", d["id"])]):
                                dropped.add(k)
                                why[k] = message
                    if os.environ.get("TEST_ERRORS"):
                        what = tree.decl.get(key[1], {}).get("name", key[1]) if key[0] in ("member", "function", "case") else key[1]
                        sys.stderr.write(f"--- {stem}: without the {key[0]} {what}: {message}\n")
        else:
            return None, skipped + [(case_of[key[1]], "its source does not compile with a Blueprint's numbers yet") for key, _ in cases]
        for key, _ in cases:
            if key in dropped:
                skipped.append((case_of[key[1]], "its source does not compile with a Blueprint's numbers yet: " + why[key]))
        if not any(key not in dropped for key, _ in cases):
            return None, skipped
        return text, skipped

    def compiles(self, text, stem):
        """What the compiler complains of in the test: [(line, complaint)], empty when it compiles."""
        d = tempfile.mkdtemp(prefix="kern-converted-check-")   # (nothing is kept between runs)
        path = os.path.join(d, f"test_{stem}.cpp")
        head = text.split("\n")
        full = text.replace('#include "../converted.h"', f'#include "{os.path.abspath(os.path.join(self.test_dir, "..", "converted.h"))}"').replace(f'#include "{self.source_dir}/', f'#include "{os.path.dirname(self.path)}/')
        assert len(full.split("\n")) == len(head)
        open(path, "w").write(full)
        if os.environ.get("TEST_KEEP"):   # (to look at: every text of the tests, as it was compiled - numbered)
            os.makedirs(os.environ["TEST_KEEP"], exist_ok=True)
            n = len([x for x in os.listdir(os.environ["TEST_KEEP"]) if x.startswith(f"test_{stem}.")])
            shutil.copy(path, os.path.join(os.environ["TEST_KEEP"], f"test_{stem}.{n}.cpp"))
        r = subprocess.run([os.environ.get("CXX", "g++"), "-std=c++20", "-fsyntax-only", "-ffp-contract=off", "-fno-exceptions", "-fno-rtti", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
                            *self.flags, "-I" + os.path.join(ROOT, "kern", "tests"), path], capture_output=True, text=True)
        errors = []
        if r.returncode != 0:
            # a complaint counts for the line of the test it is about: its own, or the one it was "required from"
            out = r.stderr.split("\n")
            mine = re.compile(re.escape(path) + r":(\d+):")
            last = None
            for i, line in enumerate(out):
                m = mine.match(line)
                if m:
                    last = int(m.group(1))
                e = re.search(r"\berror: (.*)", line)
                if not e:
                    continue
                at = int(m.group(1)) if m else last
                if at is None:
                    for later in out[i + 1:i + 12]:
                        m2 = mine.match(later)
                        if m2:
                            at = int(m2.group(1))
                            break
                errors.append((at or 0, e.group(1)[:200]))
            if not errors:
                errors = [(0, r.stderr.strip().split("\n")[0][:200])]
        shutil.rmtree(d, ignore_errors=True)
        return errors


def declared_in_header_of(tree, d):
    pd = d.get("previousDecl")
    while pd:
        prev = tree.decl.get(pd, {})
        if prev.get("_file", "").endswith(".h"):
            return True
        pd = prev.get("previousDecl")
    return False


# ---- the program ---------------------------------------------------------------------------------
POINTER_PARAM = re.compile(r"const\s+(?!void\s*\*|char\s*\*)[^*&]+?\s*\*$")   # (a parameter that may be a pointer into a list: Unit.pointed)


def pointer_source(tree, a):
    """What a call hands a parameter that may be a pointer into a list (Unit.pointed), for the plan of the whole
    program: the fixed length of the list it points into (a.data() of a std::array<T, N>, an array T[N]),
    "list" (one of any length: a std::vector), "null", ("param", id) - the caller's own such parameter -, or
    "other" (the address of anything else)."""
    x = bare(a)
    while x.get("kind") in ("ImplicitCastExpr", "ParenExpr") and x.get("castKind") != "ArrayToPointerDecay" and kids(x):
        x = bare(kids(x)[0])
    k = x.get("kind")
    def length(raw):
        c = re.sub(r"\b(const|volatile)\b|&", " ", raw).strip()
        m = re.match(r"std::array<.*,\s*(\d+)U?L*>$", c) or re.match(r"[^\[]*\[(\d+)\]$", c)
        return int(m.group(1)) if m else "list" if c.startswith("std::vector<") else "other"
    if k in ("CXXNullPtrLiteralExpr", "GNUNullExpr"):
        return "null"
    if k == "CXXMemberCallExpr" and callee(x).get("name") == "data" and len(kids(x)) == 1 and kids(callee(x)):
        return length(qtype(kids(callee(x))[0]))
    if k == "ImplicitCastExpr" and x.get("castKind") == "ArrayToPointerDecay" and kids(x):
        return length(qtype(kids(x)[0]))
    if k == "BinaryOperator" and x.get("opcode") in ("+", "-") and len(kids(x)) == 2:
        left, right = kids(x)
        return pointer_source(tree, left if "*" in qtype(left) else right)
    if k == "DeclRefExpr" and (x.get("referencedDecl") or {}).get("kind") == "ParmVarDecl":
        return ("param", x["referencedDecl"]["id"])
    return "other"


def survey_one(job):
    """One source file as the compiler reads it, for the plan of the whole program: the functions that
    have a body there (their key, their file, the functions they call - with a body or only declared) and
    the types (qualified name, file). Nothing is converted."""
    path, namespace, flags, spaces = job
    tree = Tree(path, namespace, flags, spaces)
    me = os.path.abspath(path)
    functions, pointer_args, params_of = [], [], {}
    for i, d in tree.body.items():
        if i != d["id"] or not d.get("_file") or d.get("kind") == "VarDecl" or d.get("isImplicit") or d.get("explicitlyDefaulted") or d.get("kind") == "CXXDestructorDecl":
            continue
        file = os.path.abspath(d["_file"])
        if file != me and not is_header(file):
            continue
        calls, shares = set(), False
        params_of.update({p["id"]: (function_key(tree, d), k) for k, p in enumerate(x for x in kids(d) if x.get("kind") == "ParmVarDecl")})
        for n in walk(d):
            if n.get("kind") == "DeclRefExpr" and (n.get("referencedDecl") or {}).get("kind") == "VarDecl" and not shares:
                shares = shared_variable(tree, tree.decl.get(n["referencedDecl"]["id"]))
            if n.get("kind") in ("CallExpr", "CXXMemberCallExpr"):
                c = callee(n)
                rid = (c.get("referencedDecl") or {}).get("id") or c.get("referencedMemberDecl")
                t = (tree.body.get(rid) or tree.decl.get(rid)) if rid else None
                if t is not None and t.get("kind") in ("FunctionDecl", "CXXMethodDecl"):
                    for k, (p, a) in enumerate(zip([x for x in kids(t) if x.get("kind") == "ParmVarDecl"], kids(n)[1:])):
                        if POINTER_PARAM.match(qtype(p).strip()):
                            pointer_args.append((function_key(tree, t), k, pointer_source(tree, a)))
            ids = [n.get("referencedDecl", {}).get("id"), n.get("referencedMemberDecl")]
            if n.get("kind") == "CXXConstructExpr":
                ids.append((constructor_of(tree, n) or {}).get("id"))
            for rid in ids:
                t = (tree.body.get(rid) or tree.decl.get(rid)) if rid else None
                if t is None or t.get("kind") not in ("FunctionDecl", "CXXMethodDecl", "CXXConstructorDecl", "CXXConversionDecl") or t["id"] == d["id"]:
                    continue
                # (what the notation has itself is no function of a Blueprint: a vector's operations, the
                # functions of numbers - should one be called as a function after all, its file says so: main)
                if not own_function(tree, t["id"]) or (tree.scope_of.get(t["id"], "").startswith(ENGINE + "::") and (t.get("name") in MATH or t.get("name") in MORE_MATH)):
                    continue
                calls.add(function_key(tree, t))
        a, b = d.get("_at", (None, None))
        functions.append({"key": function_key(tree, d), "file": file, "at": a or 0, "calls": sorted(calls), "lines": tree.text(d["_file"]).count("\n", a, b) + 1 if a is not None else 0,
                          "name": Unit.cpp_name_of(tree, d), "owner": tree.owner.get(d["id"]), "shares": shares, "free": d.get("kind") == "FunctionDecl" and not tree.owner.get(d["id"])})
    types = [(q, os.path.abspath(o.get("_file") or "?")) for q, o in [*tree.records.items(), *tree.enums.items()] if o.get("_file")]
    statics, made = objects_made(tree)
    # (the variables of the program's namespaces that other files name too: whether each starts at zero)
    starts = {}
    for i, d in tree.decl.items():
        if d.get("kind") != "VarDecl" or i not in tree.scope_of or d.get("storageClass") in ("static", "extern") or "_GLOBAL__N_" in (d.get("mangledName") or ""):
            continue
        if d.get("constexpr") or re.search(r"\bconst\b", qtype(d)) or os.path.abspath(d.get("_file") or "?") != me:
            continue
        init = kids(d)[-1] if kids(d) and d.get("init") else None
        starts[tree.scope_of[i] + d.get("name", "")] = init is None or starts_at_zero(init)
    records = {q: {"file": os.path.abspath(tree.records[q].get("_file") or "?"), "bases": bool(tree.records[q].get("bases")),
                   "virtual": any(c.get("virtual") for c in kids(tree.records[q]))} for q in statics if q in tree.records}
    # (what a caller's own parameter hands on: by the caller's key and the parameter's place)
    pointer_args = [(key, k, ("param", *params_of[src[1]]) if isinstance(src, tuple) and src[1] in params_of else "other" if isinstance(src, tuple) else src) for key, k, src in pointer_args]
    return {"path": path, "functions": functions, "types": types, "statics": statics, "made": made, "records": records, "pointer_args": pointer_args, "globals": starts}


def shared_variable(tree, d):
    """Is this a variable the whole program shares - of one of its namespaces (or a file's own), not const,
    not the program's way to call an actor's Blueprint? (Unit.kept_by_state: a variable of the state.)"""
    if d is None or d.get("kind") != "VarDecl" or d.get("constexpr") or re.search(r"\bconst\b", qtype(d)) or d["id"] not in tree.scope_of:
        return False
    scope = tree.scope_of[d["id"]]
    if scope.rstrip(":") in tree.records or not any(scope.startswith(x + "::") for x in (tree.namespace, *tree.spaces)):
        return False
    def actor(t, depth=0):
        c = re.sub(r"^const\s+", "", t).strip()
        if c.endswith("logic::ActorBlueprint"):
            return True
        rec = next((r for q, r in tree.records.items() if q == c or q.endswith("::" + c) or c.endswith("::" + q)), None)
        return depth < 8 and rec is not None and any(actor(b.get("type", {}).get("qualType", ""), depth + 1) for b in rec.get("bases", []) or [])
    return not actor(qtype(d))


def starts_at_zero(init):
    """Is this initializer a zero - 0, 0.0f, false, nullptr, {} - of whatever type it makes?"""
    b = init
    while b.get("kind") in ("ImplicitCastExpr", "ExprWithCleanups", "MaterializeTemporaryExpr", "CXXFunctionalCastExpr", "ParenExpr") and len(kids(b)) == 1:
        b = kids(b)[0]
    k = b.get("kind")
    if k in ("IntegerLiteral", "FloatingLiteral"):
        try:
            return float(b.get("value", "1").rstrip("fFuUlL")) == 0
        except ValueError:
            return False
    if k == "CXXBoolLiteralExpr":
        return not b.get("value")
    if k == "CXXNullPtrLiteralExpr":
        return True
    if k in ("InitListExpr", "CXXConstructExpr") and not kids(b):
        return True
    return False


def objects_made(tree):
    """The objects of the program's classes this file makes: those with static storage - a variable of a
    namespace, or `static T x;` in a function - by their place ({class: {(file, offset)}}), and the classes
    it makes objects of in any other way: a field, a local variable, a parameter or a result by value, a
    list of them, new, a temporary (see state_classes)."""
    short = {q.split("::")[-1]: q for q in tree.records}
    word = re.compile(r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*")
    def by_value(t):
        """The classes a type holds by value (not through a reference or a pointer)."""
        t = re.sub(r"\bconst\b|\bvolatile\b|\bstruct\b|\bclass\b", " ", t)
        out = set()
        for m in word.finditer(t):
            q = short.get(m.group(0).split("::")[-1])
            if q is not None and m.group(0).split("::")[-1] == q.split("::")[-1] and not re.match(r"\s*[&*]", t[m.end():]) and not re.match(r"\s*::", t[m.end():]):
                out.add(q)
        return out
    statics, made = collections.defaultdict(set), set()
    def visit(n, in_function):
        k = n.get("kind")
        t = (n.get("type") or {}).get("qualType", "")
        if k == "VarDecl":
            held = by_value(t)
            static = not in_function or n.get("storageClass") == "static"
            if static and len(held) == 1 and re.sub(r"\bconst\b|\s+|\w+::", "", t) == next(iter(held)).split("::")[-1]:
                statics[next(iter(held))].add((os.path.abspath(n.get("_file") or "?"), n.get("_at", (None,))[0]))
                for c in kids(n):   # (what makes it is that object itself)
                    if c.get("kind") not in ("CXXConstructExpr", "ExprWithCleanups", "InitListExpr", "CXXDefaultInitExpr"):
                        visit(c, in_function)
                return
            made.update(held)
        elif k in ("FieldDecl", "ParmVarDecl", "CXXNewExpr", "CXXTemporaryObjectExpr", "MaterializeTemporaryExpr", "TypeAliasDecl", "TypedefDecl", "CompoundLiteralExpr"):
            made.update(by_value(t))
        elif k in ("FunctionDecl", "CXXMethodDecl") and not n.get("isImplicit"):
            made.update(by_value(t.split("(")[0]))   # (what it gives by value)
            in_function = True
        elif k == "CXXConstructExpr" and in_function:
            made.update(by_value(t))
        elif k in ("CXXConstructorDecl", "CXXDestructorDecl", "LambdaExpr"):
            in_function = True
        for c in kids(n):
            visit(c, in_function)
    for o in [*tree.dumps, *(o for _, dumps in tree.more for o in dumps)]:
        visit(o, False)
    return {q: sorted(v, key=str) for q, v in statics.items()}, sorted(made)


def state_classes(world, surveys):
    """The classes of the program that are the state of a Blueprint: of each the program has exactly one
    object, with static storage (`Game& game() { static Game g; return g; }`), and it makes none in any other
    way - and the class has no base class and no virtual function (an object of the engine's kind). Its
    fields are the variables of a Blueprint, its methods that Blueprint's functions: {class: Blueprint}."""
    statics, made, records = collections.defaultdict(set), set(), {}
    for r in surveys:
        for q, places in r["statics"].items():
            statics[q].update(tuple(p) for p in places)
        made.update(r["made"])
        records.update(r["records"])
    out = {}
    for q, places in statics.items():
        info = records.get(q)
        if len(places) != 1 or q in made or info is None or not world.program(info["file"]) or info["bases"] or info["virtual"]:
            continue
        out[q] = type_name_of(q)
    return out


def convert_unit(job):
    """One Blueprint: the functions of its files, converted as far as they are, and - with a test directory
    - its test written. What main needs of it, as plain values (a process of its own does this for every
    Blueprint). `source`: the source file that is the Blueprint's, or None - then its headers are read by
    themselves. `part`: the job - the Blueprint, or one of its parts (World.parts)."""
    part, source, files, namespace, flags, world, test = job
    name = world.parts.get(part, part)
    NODES.update(world.nodes)
    said = io.StringIO()
    keep, sys.stderr = sys.stderr, said
    scratch = None
    try:
        path = source
        if path is None:   # (headers alone: a file that includes them is what the compiler reads)
            scratch = tempfile.mkdtemp(prefix="kern-cpp-to-blueprint-")
            path = os.path.join(scratch, part.replace("/", "_") + ".cpp")
            open(path, "w").write("".join(f'#include "{os.path.abspath(f)}"\n' for f in files) or "\n")
        tree = Tree(path, namespace, flags, world.spaces)
        main_file = os.path.abspath(source or files[0])
        # its functions, in the order they stand in: those of a header first (a source file's Blueprint has
        # the functions of its header that need the source file), then the source file's
        mine = [d for i, d in tree.body.items() if i == d["id"] and d.get("_file") and d.get("kind") != "VarDecl"
                and not (d.get("isImplicit") or d.get("explicitlyDefaulted") or d.get("kind") == "CXXDestructorDecl") and world.part_of_function(tree, d) == part]
        mine.sort(key=lambda d: (not is_header(d["_file"]), os.path.abspath(d["_file"]), d.get("_at", (0, 0))[0] or 0))
        # A function is written only when its test compares it: what the test cannot compare yet waits,
        # and with it what calls it - the file is converted again without those. And no input or local
        # variable is called what the Blueprint has in the end (a constant met after it was named): then,
        # too, it is converted again, with those names known.
        barred, reserved = {}, set()
        while True:
            writer = None
            unit = Unit(tree, name, world, part)
            unit.barred, unit.reserved = dict(barred), set(reserved)
            unit.name_functions(mine)
            unit.all = []
            for d in mine:
                if world.wanted is not None and function_key(tree, d) not in world.wanted:
                    continue   # (of a Blueprint that is not the program's own: what the program calls)
                try:
                    unit.function(d["id"])
                except Wait:
                    pass
                unit.all.append(unit.functions[d["id"]])
            for f in unit.functions.values():   # (and what those call of it)
                if f not in unit.all and not f.lifted and f.decl["id"] in unit.function_names:
                    unit.all.append(f)
            held = (unit.names - world.taken) | set(unit.type_line)
            clash = {n for f in unit.order for n in getattr(f, "local_names", ()) if n in held}
            if clash - reserved:
                reserved |= held
                continue
            unit.test = (None, [])
            if not (test and unit.order):
                break
            writer = TestWriter(unit, os.path.relpath(os.path.dirname(main_file), test), flags)
            writer.path = main_file
            writer.test_dir = test
            unit.test = writer.write()
            more = {f.decl["id"]: f"its test cannot compare it yet ({why})" for f, why in unit.test[1] if why != "it gives nothing" and f.decl["id"] not in barred}
            if not more:
                break
            barred.update(more)
        # what the tests of other files need of this one to hold the source of its functions they call
        exports = writer.exports() if writer is not None else ({}, {})
    finally:
        sys.stderr = keep
        if scratch:
            shutil.rmtree(scratch, ignore_errors=True)
    fs = unit.all
    written = [f for f in fs if f.text or f.place]   # (a name for a place is written where it is called)
    variables, functions, members, types = unit.parts()
    return {"name": name, "path": main_file, "program": world.program(main_file), "total": len(fs), "done": len(written), "total_lines": sum(f.lines for f in fs),
            "done_lines": sum(f.lines for f in written), "waits": [(f.wait, f.lines) for f in fs if f.wait],
            "said": [f"    {f.cpp} ({f.lines}): " + ("written" if f.text else "a name for a place - written where it is called" if f.place else f.wait) for f in fs],
            "table": {f.key: unit.entry_of(f) for f in unit.functions.values() if not f.lifted and f.key}, "written": len(unit.order),
            "parts": (variables, functions, members), "types": types, "uses": unit.uses(), "missing": sorted(unit.missing), "asked": sorted(unit.asked), "state": unit.state_used,
            "test": (unit.test[0], [(f.cpp, why) for f, why in unit.test[1]]), "exports": exports, "errors": said.getvalue()}


def apply_to_level(level, folder, blueprints, prune=False):
    """Writes the Blueprints into the level: each is the Blueprint of the actor of its name (made when
    the level has none), read from its text by the engine's own reader - in the order given: a library
    before the Blueprints that use it. All of them, or nothing."""
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
    if prune:   # (a file that has nothing written any more has no Blueprint any more)
        made = {name for name, _ in blueprints}
        before = len(doc["actors"])
        doc["actors"] = [a for a in doc["actors"] if a.get("folder") != folder or a["name"] in made]
        changed = changed or len(doc["actors"]) != before
    if changed:
        open(level, "w", encoding="utf-8").write(json.dumps(doc, indent=1, ensure_ascii=False) + "\n")
    if not blueprints:
        return
    scratch = tempfile.mkdtemp(prefix="kern-cpp-to-blueprint-", dir=os.environ.get("TMPDIR") or None)
    pairs = []
    for name, text in blueprints:
        path = os.path.join(scratch, name + ".cpp")
        open(path, "w").write(text)
        pairs += [name, path]
    r = subprocess.run([os.path.join(ROOT, "tools", "blueprint_cpp.sh"), "--apply", level, *pairs], capture_output=True, text=True)
    if r.returncode != 0:
        # (the texts stay where they are, to look at: the one that was refused is named)
        sys.stderr.write((r.stderr or r.stdout).strip()[:3000] + f"\n  (the texts: {scratch})\n")
        raise SystemExit("the Blueprints were not read: the level is as it was")
    shutil.rmtree(scratch, ignore_errors=True)


def plan_names(world, types, units):
    """What every type of the program is called (World.type_names, World.taken): its own name - and where
    two types have the same one, or a type the name of a Blueprint, they are told apart by where they
    stand."""
    reserved = set(units) | {SHAPES}
    by_name = collections.defaultdict(set)
    for q, file in types:
        by_name[type_name_of(q)].add((q, file))
    final = {}
    for name, owners in sorted(by_name.items()):
        for q, file in sorted(owners):
            given = name
            if len(owners) > 1:
                # a type a source file has for itself is that file's; one of the engine or a library is theirs
                if not is_header(file):
                    given = sane_name(os.path.splitext(os.path.basename(file))[0] + "_" + name)
                elif not world.program(file):
                    given = sane_name(world.unit_of(file).split("_")[0] + "_" + name)
            if given in reserved:
                given += "_t"
            if given != name:
                world.type_names[(q, file)] = given
            if given in final and final[given] != (q, file):
                raise SystemExit(f"two types of the program would both be called {given}: {final[given][0]} ({final[given][1]}) and {q} ({file})")
            final[given] = (q, file)
    world.taken = set(final)


def main(argv):
    if len(argv) < 5 or argv[1] not in ("--check", "--convert") or "--" not in argv:
        sys.stderr.write(__doc__)
        return 2
    mode, namespace = argv[1], argv[2]
    cut = argv.index("--")
    opts, files = argv[3:cut], argv[cut + 1:]
    level = folder = test = texts = None
    flags, more = [], []
    i = 0
    while i < len(opts):
        if opts[i] in ("--level", "--folder", "--test", "--texts", "--program"):
            v = opts[i + 1]
            if opts[i] == "--level": level = v
            elif opts[i] == "--folder": folder = v
            elif opts[i] == "--test": test = v
            elif opts[i] == "--program":
                # a further folder of the program and the namespaces its files declare: <folder>=<namespace>[,<namespace>...]
                d, _, spaces = v.partition("=")
                if not d or not spaces or not all(re.fullmatch(r"[A-Za-z_]\w*", s) for s in spaces.split(",")) or not os.path.isdir(d):
                    raise SystemExit(f"--program {v}: give a folder and its namespaces, <folder>=<namespace>[,<namespace>...]")
                more.append((d, spaces.split(",")))
            else: texts = v
            i += 2
        else:
            flags.append(opts[i])
            i += 1
    folder = folder or "Source"
    whole = len(files) > 20   # (a run over the whole program: what it does not write any more goes)
    workers = max(1, min(len(files), int(os.environ.get("KERN_JOBS") or os.cpu_count() or 1)))
    import concurrent.futures
    # the names of the engine's nodes: the engine's own tool says them
    r = subprocess.run([os.path.join(ROOT, "tools", "blueprint_cpp.sh"), "--node-names"], capture_output=True, text=True)
    if r.returncode != 0 or not r.stdout.strip():
        raise SystemExit("the engine's node names are not to be had (tools/blueprint_cpp.sh --node-names): " + (r.stderr or r.stdout).strip()[:500])
    NODES.update(r.stdout.split())
    pool = concurrent.futures.ProcessPoolExecutor(workers)
    world = World(files, flags, more)
    world.nodes = set(NODES)
    if level:   # (the Blueprints of the level's actors: what the program calls on them looks as they have it)
        for a in json.load(open(level, encoding="utf-8")).get("actors", []):
            g = (a.get("components") or {}).get("logic")
            if g and a.get("name"):
                world.actors[a["name"]] = {"library": bool(g.get("library")), "functions": {
                    f["name"]: ([p["name"] for p in f.get("inputs", [])], [p["name"] for p in f.get("outputs", [])]) for f in g.get("functions", []) if f.get("name")},
                    "pins": {f["name"]: ([(p.get("type", "float"), p.get("of", "")) for p in f.get("inputs", [])], [(p.get("type", "float"), p.get("of", "")) for p in f.get("outputs", [])])
                             for f in g.get("functions", []) if f.get("name")},
                    "structs": {t["name"]: [(x.get("type", "float"), x.get("of", "")) for x in t.get("fields", [])] for t in g.get("structs", []) if t.get("name")},
                    "enums": {t["name"]: len(t.get("values", [])) for t in g.get("enums", []) if t.get("name")},
                    "variables": {v["name"]: v.get("type", "float") for v in g.get("variables", []) if v.get("name")}}

    # ---- the plan: every source file read once - its functions, what they call, the types ----
    functions, types, sources = {}, set(), {}
    for path in files:
        unit = world.unit_of(path)
        if unit in sources:
            raise SystemExit(f"{path} and {sources[unit]} would both be the Blueprint {unit}")
        sources[unit] = os.path.abspath(path)
    world.source_units = set(sources)
    surveys = list(pool.map(survey_one, [(path, namespace, flags, world.spaces) for path in files]))
    handed = collections.defaultdict(set)   # (a function, a parameter that may be a pointer into a list) -> what the calls hand it
    for r in surveys:
        for f in r["functions"]:
            functions.setdefault(f["key"], f)
        types.update((q, file) for q, file in r["types"])
        for key, k, src in r["pointer_args"]:
            handed[(key, k)].add(src)
    # A pointer into a list is the list it points into, read where it stands: of the type every call hands it
    # (a std::array<T, N> - Fixed<T, N> -, or a list of any length). Calls that hand lists of other lengths, or
    # the address of anything else, it cannot take (World.pointer_lists).
    def pointer_list(at, open_=()):
        if at in open_:
            return set()
        out = set()
        for src in handed.get(at, ()):
            out |= pointer_list((src[1], src[2]), (*open_, at)) if isinstance(src, tuple) else {src}
        return out
    for at in list(handed):
        got = pointer_list(at) - {"null"}
        world.pointer_lists[at] = "other" if "other" in got else "mixed" if len(got) > 1 else next(iter(got)) if got else "list"
    # The classes there is one object of: each is the state of a Blueprint - no type (state_classes)
    world.states = state_classes(world, surveys)
    for r in surveys:
        for q, zero in r["globals"].items():
            world.globals[q] = world.globals.get(q, True) and zero
    types = {(q, file) for q, file in types if q not in world.states}
    # A header's function stands in the header's Blueprint when all it calls stands in headers too
    # (whoever includes the header has all of it); one that needs a source file - it calls what stands in
    # one - stands with the source file of the header's name.
    closed = {}
    def is_closed(k):
        if k not in closed:
            closed[k] = True   # (functions that call each other: judged by what else they call)
            closed[k] = is_header(functions[k]["file"]) and all(is_closed(c) for c in functions[k]["calls"] if c in functions)
        return closed[k]
    source_of = {os.path.splitext(p)[0]: u for u, p in sources.items()}   # (a source file's Blueprint, by its path without the ending)
    unit_of = {}
    for k, f in functions.items():
        unit_of[k] = world.unit_of(f["file"]) if is_closed(k) or not is_header(f["file"]) else source_of.get(os.path.splitext(f["file"])[0])
    # A method of a state is a function of the state's Blueprint. Its methods stand in many files: each file
    # is a part of the Blueprint, converted in that file's tree ("Game/game_net_cpp"), and main puts the
    # parts together. (A method of a header stands in the part its function would stand in.)
    part_of = {}
    for k, f in functions.items():
        if f.get("owner") in world.states and unit_of[k] is not None:
            part = world.states[f["owner"]] + "/" + unit_of[k]
            part_of[part] = unit_of[k]
            world.parts[part] = world.states[f["owner"]]
            unit_of[k] = part
    # A function that is no method but shares a variable of the program - or calls one that does -, when the
    # program has one object (a state): a function of that state's Blueprint, in the part of its file. (A method
    # of another class that does stays where it is, and waits: it would need its object and the state's.)
    if len(set(world.states.values())) == 1:
        state = next(iter(world.states.values()))
        callers = collections.defaultdict(set)
        for k, f in functions.items():
            for c in f["calls"]:
                callers[c].add(k)
        kept = {k for k, f in functions.items() if f.get("shares") and f.get("free") and world.program(f["file"])}
        todo = list(kept)
        while todo:
            for k in callers.get(todo.pop(), ()):
                f = functions[k]
                if k not in kept and f.get("free") and world.program(f["file"]):
                    kept.add(k)
                    todo.append(k)
        for k in kept:
            if unit_of[k] is not None and not unit_of[k].startswith(state + "/"):
                part = state + "/" + unit_of[k]
                part_of[part] = unit_of[k]
                world.parts[part] = state
                unit_of[k] = part
        world.state_functions = kept
    for blueprint in sorted(set(world.parts.values())):
        for i, part in enumerate(sorted(p for p, b in world.parts.items() if b == blueprint)):
            world.part_numbers[part] = i
    world.source_units |= {p for p, u in part_of.items() if u in sources}
    world.function_units = unit_of
    program = {world.unit_of(f["file"]) for f in functions.values() if world.program(f["file"])} | set(sources) | set(world.parts)
    # of the Blueprints that are not the program's own (headers of the engine): what the program calls
    wanted = collections.defaultdict(set)
    todo, seen = [k for k, f in functions.items() if world.program(f["file"])], set()
    while todo:
        k = todo.pop()
        if k in seen:
            continue
        seen.add(k)
        if unit_of[k] is not None and unit_of[k] not in program:
            wanted[unit_of[k]].add(k)
        todo += [c for c in functions[k]["calls"] if c in functions]
    unit_files, nowhere = collections.defaultdict(set), []
    for k in seen:
        if unit_of[k] is None:
            nowhere.append(functions[k])
        else:
            unit_files[unit_of[k]].add(functions[k]["file"])
    for u, path in sources.items():
        unit_files[u].add(path)
    for p, u in part_of.items():   # (a part is converted in the tree of its file)
        unit_files[p] |= {sources[u]} if u in sources else set()
    for u in set(sources) - set(unit_of.values()):   # (a source file all of whose functions stand in parts has no Blueprint of its own)
        unit_files.pop(u, None)
    # A Blueprint is written after those it calls - the headers' Blueprints and those of other source files:
    # its test holds the source of what it calls there as the tests of those files hand it on (exports).
    deps = {u: set() for u in unit_files}
    for k in seen:
        if unit_of[k] is not None:
            deps[unit_of[k]].update(unit_of[c] for c in functions[k]["calls"] if c in functions and unit_of[c] not in (None, unit_of[k]) and unit_of[c] in deps)
    plan_names(world, types, (set(deps) - set(world.parts)) | set(world.parts.values()) | {world.unit_of(f["file"]) for f in functions.values()} | {world.unit_of(file) for _, file in types})

    def job_of(u):
        files_ = sorted(unit_files[u], key=lambda f: (not is_header(f), f))
        need, stack = set(), list(deps[u])
        while stack:   # (the Blueprints it may call: those it uses, and those they use)
            d = stack.pop()
            if d not in need:
                need.add(d)
                stack += deps.get(d, ())
        w = World(files, flags, more)
        w.units, w.type_names, w.taken, w.nodes, w.function_units, w.source_units = world.units, world.type_names, world.taken, world.nodes, world.function_units, world.source_units
        w.actors, w.states, w.parts, w.part_numbers = world.actors, world.states, world.parts, world.part_numbers
        w.pointer_lists, w.globals, w.state_functions = world.pointer_lists, world.globals, world.state_functions
        w.table = {k: e for d in need for k, e in tables.get(d, {}).items()}
        if test:   # (the pieces of their source their tests hand on: what the test of this one holds of them)
            w.closures, w.pieces = {}, {}
            for d in sorted(need):
                closures, pieces = exports.get(d, ({}, {}))
                w.closures.update(closures)
                for place, piece in pieces.items():
                    w.pieces.setdefault(place, piece)
        w.wanted = None if u in program else set(wanted[u])
        return (u, sources.get(part_of.get(u, u)), files_, namespace, flags, w, test)

    def reaches(a, b):
        stack, met = [a], set()
        while stack:
            x = stack.pop()
            if x == b:
                return True
            if x not in met:
                met.add(x)
                stack += deps.get(x, ())
        return False

    # ---- the Blueprints, each after those it calls ----
    tables, exports, results, done, running, cut_calls = {}, {}, {}, set(), {}, set()
    while len(done) < len(deps):
        ready = sorted(u for u in deps if u not in done and u not in running.values() and all(d in done or (u, d) in cut_calls for d in deps[u]))
        if not ready and not running:
            # Blueprints that call each other (the parts of one Blueprint do): the one that waits for the fewest
            # goes first, and what it would call of those waits - until it is written: then it is converted
            # again (below). What calls itself through them waits for good, as a function that calls itself.
            u = min((u for u in deps if u not in done), key=lambda u: (len([d for d in deps[u] if d not in done]), u))
            print(f"{u} and {', '.join(sorted(d for d in deps[u] if d not in done))} call each other: {u} goes first", flush=True)
            cut_calls.update((u, d) for d in deps[u] if d not in done)
            continue
        for u in ready[:max(0, workers - len(running))]:
            running[pool.submit(convert_unit, job_of(u))] = u
        finished, _ = concurrent.futures.wait(list(running), return_when=concurrent.futures.FIRST_COMPLETED)
        for future in finished:
            u = running.pop(future)
            r = future.result()
            again = False
            for d, key in r["missing"]:
                if (u, d) in cut_calls or key not in functions:
                    continue
                if d not in deps:               # (a Blueprint nothing was known to call of)
                    deps[d] = set()
                    unit_files[d].add(functions[key]["file"])
                    wanted[d].add(key)
                elif d in done and key not in tables[d] and d not in program and key not in wanted[d]:
                    wanted[d].add(key)          # (one more function of it: it is written again)
                    done.discard(d)
                elif d in done and (key in tables[d] and d in deps[u]):
                    continue
                elif d not in done and reaches(d, u):
                    cut_calls.add((u, d))       # (they call each other: see above)
                    continue
                elif d in done and key not in tables[d]:
                    continue                    # (it has no such function: the call waits)
                deps[u].add(d)
                again = True
            if again:
                continue
            done.add(u)
            results[u] = r
            tables[u] = r["table"]
            exports[u] = r["exports"]
            # What waited for a function of this one that is written now is converted again - the functions of
            # a Blueprint only go from not there to waiting to written, so this ends.
            now = {k for k, e in r["table"].items() if not e["wait"]}
            for v in sorted(done - {u}):
                if (set(results[v]["asked"]) | {k for d, k in results[v]["missing"] if d == u}) & now:
                    done.discard(v)
                    print(f"{v} is converted again: what it calls of {u} is written now", flush=True)
            sys.stderr.write(r["errors"])
            print(f"{u:28} {r['done']:4} / {r['total']:4} functions   {r['done_lines']:6} / {r['total_lines']:6} lines" + ("" if u in program else "   (what the program calls of it)"), flush=True)
            if os.environ.get("FUNCTIONS"):
                for line in r["said"]:
                    print(line)
    pool.shutdown()

    # ---- the parts of a Blueprint of several files: one Blueprint ----
    blue = {u: r for u, r in results.items() if u not in world.parts}
    for blueprint in sorted(set(world.parts.values())):
        mine = sorted((p for p in results if world.parts.get(p) == blueprint), key=lambda p: world.part_numbers[p])
        state, constants, said_by, texts_, members, types_, uses_ = {}, [], {}, [], [], {}, set()
        for p in mine:
            r = results[p]
            for at, lines in r["state"].items():
                if state.setdefault(tuple(at), lines) != lines:
                    raise SystemExit(f"the variable {lines[-1].strip()} of {blueprint} is not the same in every part ({p}: {state[tuple(at)]})")
            variables, functions_, members_ = r["parts"]
            held = {line for lines in r["state"].values() for line in lines}
            block = []
            for line in variables:   # (the constants: each once - a /// line belongs to the one below it)
                if line in held:
                    continue
                block.append(line)
                if line.strip().startswith("///"):
                    continue
                key = re.sub(r"\s*=.*", "", line).split()[-1]
                if key in said_by and said_by[key] != block:
                    raise SystemExit(f"{blueprint}: two parts have a constant {key} that is not the same:\n{said_by[key]}\n{block} ({p})")
                if key not in said_by:
                    said_by[key] = block
                    constants += block
                block = []
            texts_ += functions_
            members += members_
            types_.update(r["types"])
            uses_.update(r["uses"])
        blue[blueprint] = {"written": sum(results[p]["written"] for p in mine), "types": types_, "uses": sorted(uses_ - {blueprint}),
                           "parts": ([line for at in sorted(state) for line in state[at]] + constants, texts_, members), "state": True}

    # ---- the types: each declared once, in the Blueprint of the file that defines it ----
    declared = {}   # a type -> (the Blueprint that declares it or None for a shape, its declaration, the types that names)
    for u in sorted(blue):
        for name, (home, line, parts) in blue[u]["types"].items():
            if name in declared and declared[name][1] != line:
                raise SystemExit(f"the type {name} is not the same in every Blueprint:\n{declared[name][1]}\n{line} (in {u})")
            declared.setdefault(name, (home, line, list(parts)))
    written = {u for u, r in blue.items() if r["written"]}
    def components(name):
        """The Blueprints the types stand in that a type is made of (a shape: those its parts are made of)."""
        if name not in declared:
            return set()
        home, _, parts = declared[name]
        return {home} if home is not None else {h for p in parts for h in components(p)}
    # what a Blueprint uses, before the shapes have their place: the Blueprints it calls, and those of the types it names
    base = collections.defaultdict(set)
    for name, (home, _, parts) in declared.items():
        if home is not None:
            base[home].update(h for p in parts for h in components(p) if h != home)
    for u in written:
        base[u].update(blue[u]["uses"])
        base[u].update(h for n in blue[u]["types"] for h in components(n) if h != u)
    depth = {}
    def depth_of(unit, open_=()):
        """How deep a Blueprint stands: above all it uses."""
        if unit not in depth:
            if unit in open_:
                raise SystemExit(f"the Blueprints {', '.join(open_)} use each other (a header that needs a type or a function of one that needs it)")
            depth[unit] = 1 + max((depth_of(h, (*open_, unit)) for h in base.get(unit, ())), default=0)
        return depth[unit]
    def home_of(name):
        """Where a type is declared. A shape made of the notation's own types stands in the library of
        shapes; one made of the program's types with the last of them - the one that stands above the
        others."""
        home, _, parts = declared[name]
        if home is not None:
            return home
        below = components(name)
        return max(below, key=lambda h: (depth_of(h), h)) if below else SHAPES
    homes = collections.defaultdict(list)
    for name in declared:
        homes[home_of(name)].append(name)
    placed = {}
    def place(name):   # (a type after the types its declaration names)
        if name not in placed:
            placed[name] = 1 + max((place(p) for p in declared[name][2] if p in declared), default=0)
        return placed[name]
    uses = {}
    for u in set(homes) | written:
        mine = sorted(homes.get(u, ()), key=lambda n: (place(n), n))
        called = set(blue[u]["uses"]) if u in written else set()
        named = {home_of(p) for n in mine for p in declared[n][2] if p in declared} | ({home_of(n) for n in blue[u]["types"]} if u in written else set())
        uses[u] = sorted((called | named) - {u})
    def through(a, b, met=None):
        met = met if met is not None else set()
        return any(x == b or (x not in met and not met.add(x) and through(x, b, met)) for x in uses.get(a, ()))
    blueprints = {}
    for u in uses:
        mine = sorted(homes.get(u, ()), key=lambda n: (place(n), n))
        called = set(blue[u]["uses"]) if u in written else set()
        # (a Blueprint that comes along with another one it uses is not named again - unless its functions are called)
        direct = [x for x in uses[u] if x in called or not any(y != x and through(y, x) for y in uses[u])]
        variables, texts_, members = blue[u]["parts"] if u in written else ([], [], [])
        blueprints[u] = blueprint_text(u, direct, [declared[n][1] for n in mine], variables, texts_, members, state=bool(blue[u].get("state")) if u in blue else False)
    order, left = [], dict(uses)
    while left:   # (a library before the Blueprints that use it)
        free = sorted(u for u in left if all(x not in left for x in left[u]))
        if not free:
            raise SystemExit("Blueprints that use each other: " + ", ".join(sorted(left)))
        order += free
        for u in free:
            del left[u]

    # ---- what was done ----
    total = done_n = total_lines = done_lines = 0
    waits, wait_lines = collections.Counter(), collections.Counter()
    compared, not_compared, tests_written = 0, collections.Counter(), set()
    for u in sorted(results, key=lambda u: (u not in program, u)):
        r = results[u]
        if u in program:
            total += r["total"]
            done_n += r["done"]
            total_lines += r["total_lines"]
            done_lines += r["done_lines"]
            for w, lines in r["waits"]:
                waits[w] += 1
                wait_lines[w] += lines
        if test and r["written"]:
            body, skipped = r["test"]
            compared += (body or "").count("converted::Tally tally(")
            for cpp, why in skipped:
                not_compared[why] += 1
                if os.environ.get("FUNCTIONS") and why != "it gives nothing":
                    print(f"    not compared: {cpp}: {why}")
            name = os.path.join(test, f"test_{u.replace('/', '__')}.cpp")   # (a part of a Blueprint: Game/game_net_cpp - test_Game__game_net_cpp.cpp)
            if body:
                os.makedirs(test, exist_ok=True)
                if not os.path.exists(name) or open(name).read() != body:
                    open(name, "w").write(body)
                tests_written.add(os.path.abspath(name))
    if texts:
        os.makedirs(texts, exist_ok=True)
        for u in order:
            open(os.path.join(texts, u + ".cpp"), "w").write(blueprints[u])
    if level and mode == "--convert":
        apply_to_level(level, folder, [(u, blueprints[u]) for u in order], prune=whole)
    for f in nowhere:   # (a header's function that needs a source file which is not among those converted)
        if world.program(f["file"]):
            total += 1
            total_lines += f["lines"]
            waits["what it calls stands in a source file that is not converted"] += 1
            wait_lines["what it calls stands in a source file that is not converted"] += f["lines"]
    others = sum(results[u]["done"] for u in results if u not in program)
    states = set(world.parts.values())
    print(f"\n{done_n} of {total} functions of the program are written as Blueprints ({done_lines} of {total_lines} lines)" + (f", and {others} of the engine's headers that they call" if others else "")
          + f": {len(order)} Blueprints - {sum(1 for u in order if u in sources)} of source files, {sum(1 for u in order if u in states)} of states, {sum(1 for u in order if u not in sources and u not in states)} of headers and of shapes")
    if test:
        if os.path.isdir(test) and whole:
            for name in os.listdir(test):   # (tests of files that have nothing converted any more)
                if name.startswith("test_") and name.endswith(".cpp") and os.path.abspath(os.path.join(test, name)) not in tests_written:
                    os.unlink(os.path.join(test, name))
        nothing = not_compared.pop("it gives nothing", 0)
        print(f"{compared} functions of the Blueprints are compared with their source, bit for bit, by the tests in {test} - each once, in the Blueprint it stands in"
              + (f"; {nothing} give nothing" if nothing else "") + ("; not compared: " + ", ".join(f"{n} ({w})" for w, n in not_compared.most_common()) if not_compared else ""))
    print("\nwhat the others wait for: functions (lines)")
    for w, n in sorted(waits.items(), key=lambda kv: -wait_lines[kv[0]])[:int(os.environ.get("TOP", "40"))]:
        print(f"  {n:4} ({wait_lines[w]:6})   {w}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
