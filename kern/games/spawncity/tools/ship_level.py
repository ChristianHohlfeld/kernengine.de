#!/usr/bin/env python3
"""Writes the level and the project as the built game carries them: without the Blueprints of one folder.

    tools/ship_level.py <level.klevel> <out.klevel> --without <folder> [--project <name.kproject> <out.kproject>]

build.sh leaves out "Source", the Blueprints tools/cpp_to_blueprint.py writes from the game's C++ (BLUEPRINTS.md):
until a file switches, its C++ is what runs and its Blueprint stands beside it, bound to it by the tests; the built
game would only load and carry them (most of the text). They stand in the level (a world's) and in the project (the
program's: kern/docs/project.md) - each file loses them alike. What the Blueprints that stay use as their libraries
(`uses` of their Blueprints, and what those use) stays with them, wherever it stands: a Blueprint of the program (a
file of the game that is a Blueprint now, in the project) calls libraries of "Source" (BLUEPRINTS.md, "5 - Umschalten").
The engine's kern/tools/embed_project.py then embeds this project with this level (--level), and tools/blueprint_cpp.sh
reads them. The outputs are only rewritten when they change. The tests embed the whole level and project.
"""
import json, pathlib, sys


def libraries(entries, graphs):
    """The names of the Blueprints `graphs` use as libraries - and those these use, each once. `entries`: the
    Blueprints by their names (the level's actors and the project's Blueprints: a name stands once)."""
    def uses(graph):
        return graph.get("uses", []) if isinstance(graph, dict) else []
    todo = [n for g in graphs for n in uses(g)]
    needed = set()
    while todo:
        n = todo.pop()
        if n in needed:
            continue
        if n not in entries:
            sys.exit(f"{level}: a Blueprint uses the library {n}, which neither the level nor the project has")
        needed.add(n)
        todo += uses(entries[n])
    return needed


def logic_of(a):
    return a.get("components", {}).get("logic")


args = sys.argv[1:]
if len(args) not in (4, 7) or args[2] != "--without" or (len(args) == 7 and args[4] != "--project"):
    sys.exit("usage: ship_level.py <level.klevel> <out.klevel> --without <folder> [--project <name.kproject> <out.kproject>]")
level, out, without = pathlib.Path(args[0]), pathlib.Path(args[1]), args[3]
project, project_out = (pathlib.Path(args[5]), pathlib.Path(args[6])) if len(args) == 7 else (None, None)
doc = json.loads(level.read_text(encoding="utf-8"))  # a level that is not JSON stops the build here
if doc.get("format") != "kern-level":
    sys.exit(f"{level}: not a Kern level")
proj = json.loads(project.read_text(encoding="utf-8")) if project else {"blueprints": []}
if project and proj.get("format") != "kern-project":
    sys.exit(f"{project}: not a Kern project")
blueprints = proj.get("blueprints", [])
if all(a.get("folder") != without for a in doc["actors"]) and all(b.get("folder") != without for b in blueprints):
    sys.exit(f"{level}: no Blueprint in the folder {without}" + (f" (nor in {project})" if project else ""))
entries = {a["name"]: logic_of(a) for a in doc["actors"]}
for b in blueprints:
    if b.get("name") in entries:
        sys.exit(f"the Blueprint {b.get('name')} stands in the level and in the project - rename one")
    entries[b.get("name")] = b.get("logic")
# (what stays: the Blueprints of the other folders, and every library they use - by its name, as `uses` says it)
needed = libraries(entries, [logic_of(a) for a in doc["actors"] if a.get("folder") != without]
                   + [b.get("logic") for b in blueprints if b.get("folder") != without] + [doc.get("logic")])
doc["actors"] = [a for a in doc["actors"] if a.get("folder") != without or a["name"] in needed]
outputs = [(out, json.dumps(doc, ensure_ascii=False, indent=1) + "\n")]
if project:
    proj["blueprints"] = [b for b in blueprints if b.get("folder") != without or b.get("name") in needed]
    outputs.append((project_out, json.dumps(proj, ensure_ascii=False, indent=2) + "\n"))
for path, text in outputs:
    if not path.exists() or path.read_text(encoding="utf-8") != text:
        path.write_text(text, encoding="utf-8")
