#!/usr/bin/env python3
"""Why a class's state is no struct of the Blueprint language yet: EVERY field without a type, with the reason,
and for a field of another class of the program that class's fields in turn (once each).

  python3 why_struct.py <tools dir> <source.cpp> <Class> [<Class> ...] -- <compiler flags>
"""
import os
import sys

tools, source = sys.argv[1], sys.argv[2]
rest = sys.argv[3:]
names, flags = rest[:rest.index("--")], rest[rest.index("--") + 1:]
sys.path.insert(0, tools)
import cpp_to_blueprint as c2b   # noqa: E402

tree = c2b.Tree(source, "sc", flags)
world = c2b.World([source], flags)
unit = c2b.Unit(tree, "why", world)
seen = set()


def record_of(raw):
    """The program's class a C++ type is made of (through containers), or None."""
    t = unit.clean(raw)
    for word in __import__("re").findall(r"[A-Za-z_][\w:]*", t):
        try:
            q = unit.find_type(word)
        except c2b.Wait:
            continue
        if q in tree.records:
            return q
    return None


def report(q, depth=0):
    if q in seen:
        return
    seen.add(q)
    o = tree.records[q]
    fields = [c for c in c2b.kids(o) if c.get("kind") == "FieldDecl"]
    bad = []
    for c in fields:
        try:
            unit.btype(c2b.qtype(c))
        except c2b.Wait as w:
            bad.append((c.get("name"), c2b.qtype(c), str(w)))
        except Exception as e:   # noqa: BLE001
            bad.append((c.get("name"), c2b.qtype(c), "error: " + repr(e)))
    pad = "  " * depth
    extra = " (has base classes)" if o.get("bases") else ""
    print(f"{pad}{q}: {len(fields)} fields, {len(bad)} without a type{extra}")
    for name, raw, why in bad:
        print(f"{pad}  - {name}: {raw}  ->  {why}")
    for name, raw, why in bad:
        inner = record_of(raw)
        if inner and inner != q:
            report(inner, depth + 2)


for n in names:
    q = unit.find_type(n) if n not in tree.records else n
    if q not in tree.records:
        print(f"{n}: no such class in this file")
        continue
    report(q)
