#!/usr/bin/env python3
"""Writes the level file into a C++ source the game links (src/level_data.cpp, generated).

    tools/embed_level.py <level.klevel> <out.cpp> [--without <folder> --level-out <file.klevel> --split-program]

The level (level/spawncity.klevel) is SpawnCity's project file: world settings, actors and their
Blueprints. The game starts from it (rules.h); the build and the tests call this first. The
output is only rewritten when it changes, so nothing recompiles for an unchanged level.

--without <folder>: the game that is built does not carry the actors of this folder - build.sh leaves out
"Source", the Blueprints tools/cpp_to_blueprint.py writes from the game's C++ (BLUEPRINTS.md): until a file
switches, its C++ is what runs and its Blueprint stands beside it, bound to it by the tests; the built game
would only load and carry them (most of the level's text). What the actors that stay use as their libraries
(`uses` of their Blueprints, and what those use) stays with them, wherever it stands: a Blueprint of the folder
"Program" (a file of the game that is a Blueprint now) calls libraries of "Source" (BLUEPRINTS.md, "5 - Umschalten").
The tests embed the whole level. The member "project" (the settings the engine applies at start,
kern/app/project.h) goes out on its own too: kern_project_text(). --level-out writes the level as it is embedded, for the tools that follow
(tools/blueprint_cpp.sh).

--split-program: the level is two - the program and its world (kern/game/level_app.h). kern_level_text() is the program's:
the actors of the folder the project names as the program ("project": {"program": ...}) with every library they use,
and the project; rules::level_text() the world's: the level without those actors (but with the libraries the others
use). Without it both are the whole level (as embedded). Used from the switch on (schaltschritt.md).
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


level, out = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2])
options = sys.argv[3:]
without, level_out, split = None, None, False
while options:
    if len(options) >= 2 and options[0] == "--without":
        without = options[1]
    elif options[0] == "--split-program":
        split = True
        options = options[1:]
        continue
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
    needed = libraries(doc, [logic_of(a) for a in doc["actors"] if a.get("folder") != without] + [doc.get("logic")])
    doc["actors"] = [a for a in doc["actors"] if a.get("folder") != without or a["name"] in needed]
    text = json.dumps(doc, ensure_ascii=False, indent=1) + "\n"
program_text = None
if split:
    # (the program: the folder the project names, with its libraries; the world: the rest, with the libraries it uses)
    folder = doc.get("project", {}).get("program") if isinstance(doc.get("project"), dict) else None
    if not folder:
        sys.exit(f"{level}: --split-program: the project names no program (\"project\": {{\"program\": <folder>}})")
    mine = [a for a in doc["actors"] if a.get("folder") == folder]
    if not mine:
        sys.exit(f"{level}: --split-program: no actor in the folder {folder} the project names as the program")
    theirs = libraries(doc, [logic_of(a) for a in mine])
    program = {k: v for k, v in doc.items() if k != "logic"}
    program["actors"] = [a for a in doc["actors"] if a.get("folder") == folder or a["name"] in theirs]
    program_text = json.dumps(program, ensure_ascii=False, indent=1) + "\n"
    rest = [a for a in doc["actors"] if a.get("folder") != folder]
    kept = libraries(doc, [logic_of(a) for a in rest] + [doc.get("logic")])
    doc["actors"] = [a for a in doc["actors"] if a.get("folder") != folder or a["name"] in kept]
    text = json.dumps(doc, ensure_ascii=False, indent=1) + "\n"
if level_out is not None and (not level_out.exists() or level_out.read_text(encoding="utf-8") != text):
    level_out.write_text(text, encoding="utf-8")
if ')KLEVEL"' in text or program_text and ')KLEVEL"' in program_text:
    sys.exit(f"{level}: contains the embedding's end mark")


def literal(t):
    """The text as a string literal: compilers limit one (64 KiB in the standard) - pieces of 8 KiB, joined by the compiler."""
    pieces = [t[i:i + 8192] for i in range(0, len(t), 8192)] or [""]
    return "\n".join('    R"KLEVEL(' + p + ')KLEVEL"' for p in pieces)


body = literal(text)
if program_text is None:
    program_source = """  const std::string_view text = sc::rules::level_text();
  *length = static_cast<unsigned>(text.size());
  return text.data();"""
else:
    program_source = "  static const char text[] =\n" + literal(program_text) + ";\n  *length = sizeof text - 1;\n  return text;"
# the project's settings the engine applies at start (kern/app/project.h): the member "project", on its own
project = json.dumps(doc["project"], ensure_ascii=False, separators=(",", ":")) if isinstance(doc.get("project"), dict) else ""
source = f"""// Generated from level/{level.name} by tools/embed_level.py - do not edit (edit the level).
#include "rules.h"

namespace sc::rules {{

std::string_view level_text() {{
  static const char text[] =
{body};
  return {{text, sizeof text - 1}};
}}

}}  // namespace sc::rules

// The level as the program carries it, for the engine (kern/game/level_app.h: the App of a program that makes none).
extern "C" const char* kern_level_text(unsigned* length) {{
{program_source}
}}

// The level's "project" for the engine (kern_app_start, kern/app/project.h).
extern "C" const char* kern_project_text(unsigned* length) {{
  static const char text[] = R"KLEVEL({project})KLEVEL";
  *length = sizeof text - 1;
  return text;
}}
"""
if not out.exists() or out.read_text(encoding="utf-8") != source:
    out.write_text(source, encoding="utf-8")
