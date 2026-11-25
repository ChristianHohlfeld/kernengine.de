#!/usr/bin/env python3
"""Writes the level file into a C++ source the game links (src/level_data.cpp, generated).

    tools/embed_level.py <level.klevel> <out.cpp> [--without <folder> --level-out <file.klevel>]

The level (level/spawncity.klevel) is SpawnCity's project file: world settings, actors and their
Blueprints. The game starts from it (rules.h); the build and the tests call this first. The
output is only rewritten when it changes, so nothing recompiles for an unchanged level.

--without <folder>: the game that is built does not carry the actors of this folder - build.sh leaves out
"Source", the Blueprints tools/cpp_to_blueprint.py writes from the game's C++ (BLUEPRINTS.md): until a file
switches, its C++ is what runs and its Blueprint stands beside it, bound to it by the tests; the built game
would only load and carry them (most of the level's text). What the actors that stay use as their libraries
(`uses` of their Blueprints, and what those use) stays with them, wherever it stands: a Blueprint of the folder
"Program" (a file of the game that is a Blueprint now) calls libraries of "Source" (BLUEPRINTS.md, "5 - Umschalten").
The tests embed the whole level. --level-out writes the level as it is embedded, for the tools that follow
(tools/blueprint_cpp.sh).
"""
import json, pathlib, sys

level, out = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2])
options = sys.argv[3:]
without, level_out = None, None
while options:
    if len(options) >= 2 and options[0] == "--without":
        without = options[1]
    elif len(options) >= 2 and options[0] == "--level-out":
        level_out = pathlib.Path(options[1])
    else:
        sys.exit(f"embed_level.py: {options[0]}: not an option (--without <folder>, --level-out <file>)")
    options = options[2:]
text = level.read_text(encoding="utf-8")
doc = json.loads(text)  # a level that is not JSON stops the build here
if doc.get("format") != "kern-level":
    sys.exit(f"{level}: not a Kern level")
if without is not None:
    if all(a.get("folder") != without for a in doc["actors"]):
        sys.exit(f"{level}: no actor in the folder {without}")
    # (what stays: the actors of the other folders, and every library they use - by its actor's name, as `uses` says it)
    by_name = {a["name"]: a for a in doc["actors"]}
    def uses(graph):
        return graph.get("uses", []) if isinstance(graph, dict) else []
    todo = [n for a in doc["actors"] if a.get("folder") != without for n in uses(a.get("components", {}).get("logic"))]
    todo += uses(doc.get("logic"))
    needed = set()
    while todo:
        n = todo.pop()
        if n in needed:
            continue
        if n not in by_name:
            sys.exit(f"{level}: a Blueprint uses the library {n}, which no actor of the level is")
        needed.add(n)
        todo += uses(by_name[n].get("components", {}).get("logic"))
    doc["actors"] = [a for a in doc["actors"] if a.get("folder") != without or a["name"] in needed]
    text = json.dumps(doc, ensure_ascii=False, indent=1) + "\n"
if level_out is not None and (not level_out.exists() or level_out.read_text(encoding="utf-8") != text):
    level_out.write_text(text, encoding="utf-8")
if ')KLEVEL"' in text:
    sys.exit(f"{level}: contains the embedding's end mark")
# compilers limit one string literal (64 KiB in the standard): pieces of 8 KiB, joined by the compiler
pieces = [text[i:i + 8192] for i in range(0, len(text), 8192)] or [""]
body = "\n".join('    R"KLEVEL(' + p + ')KLEVEL"' for p in pieces)
source = f"""// Generated from level/{level.name} by tools/embed_level.py - do not edit (edit the level).
#include "rules.h"

namespace sc::rules {{

std::string_view level_text() {{
  static const char text[] =
{body};
  return {{text, sizeof text - 1}};
}}

}}  // namespace sc::rules
"""
if not out.exists() or out.read_text(encoding="utf-8") != source:
    out.write_text(source, encoding="utf-8")
