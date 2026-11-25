#!/usr/bin/env python3
"""Writes the level as the built game carries it: without the actors of one folder.

    tools/ship_level.py <level.klevel> <out.klevel> --without <folder>

build.sh leaves out "Source", the Blueprints tools/cpp_to_blueprint.py writes from the game's C++ (BLUEPRINTS.md):
until a file switches, its C++ is what runs and its Blueprint stands beside it, bound to it by the tests; the built
game would only load and carry them (most of the level's text). What the actors that stay use as their libraries
(`uses` of their Blueprints, and what those use) stays with them, wherever it stands: a Blueprint of the folder
"Program" (a file of the game that is a Blueprint now) calls libraries of "Source" (BLUEPRINTS.md, "5 - Umschalten").
The engine's kern/tools/embed_project.py then embeds the project with this level (--level), and tools/blueprint_cpp.sh
reads it. The output is only rewritten when it changes. The tests embed the whole level.
"""
import json, pathlib, sys


def libraries(doc, graphs):
    """The names of the actors whose Blueprints `graphs` use as libraries - and those these use, each once."""
    by_name = {a["name"]: a for a in doc["actors"]}
    def uses(graph):
        return graph.get("uses", []) if isinstance(graph, dict) else []
    todo = [n for g in graphs for n in uses(g)]
    needed = set()
    while todo:
        n = todo.pop()
        if n in needed:
            continue
        if n not in by_name:
            sys.exit(f"{level}: a Blueprint uses the library {n}, which no actor of the level is")
        needed.add(n)
        todo += uses(by_name[n].get("components", {}).get("logic"))
    return needed


def logic_of(a):
    return a.get("components", {}).get("logic")


if len(sys.argv) != 5 or sys.argv[3] != "--without":
    sys.exit("usage: ship_level.py <level.klevel> <out.klevel> --without <folder>")
level, out, without = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), sys.argv[4]
doc = json.loads(level.read_text(encoding="utf-8"))  # a level that is not JSON stops the build here
if doc.get("format") != "kern-level":
    sys.exit(f"{level}: not a Kern level")
if all(a.get("folder") != without for a in doc["actors"]):
    sys.exit(f"{level}: no actor in the folder {without}")
# (what stays: the actors of the other folders, and every library they use - by its actor's name, as `uses` says it)
needed = libraries(doc, [logic_of(a) for a in doc["actors"] if a.get("folder") != without] + [doc.get("logic")])
doc["actors"] = [a for a in doc["actors"] if a.get("folder") != without or a["name"] in needed]
text = json.dumps(doc, ensure_ascii=False, indent=1) + "\n"
if not out.exists() or out.read_text(encoding="utf-8") != text:
    out.write_text(text, encoding="utf-8")
