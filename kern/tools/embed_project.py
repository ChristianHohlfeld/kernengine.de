#!/usr/bin/env python3
"""Writes a project file and its levels into a C++ source the game links (generated - do not edit it).

    kern/tools/embed_project.py <name.kproject> <out.cpp> [--level <path> <file.klevel>]...

The project (kern/game/project.h, kern/docs/project.md) is read and checked: it must be a Kern project, and each
level its "levels" name (relative to the project file) a Kern level - one that still holds the project's settings
(the member "project") stops the build with the place and the reason. The source defines
  kern::game::embedded_project_text(), kern::game::embedded_level(path)   (kern/game/embedded.h)
- data of the project, no entry: the program's kern_main() hands it to the engine (kern::run(embedded_project())).
--level <path> <file>: embed `file` as the level `path` of the project (a build that carries a level other than the
one the project file names - e.g. one with fewer actors). The output is only rewritten when it changes, so nothing
recompiles for an unchanged project.
"""
import json, pathlib, sys

END = ')KERN"'


def fail(text):
    sys.exit(f"embed_project.py: {text}")


args = sys.argv[1:]
if len(args) < 2:
    fail("usage: embed_project.py <name.kproject> <out.cpp> [--level <path> <file.klevel>]...")
project_file, out = pathlib.Path(args[0]), pathlib.Path(args[1])
instead = {}
rest = args[2:]
while rest:
    if len(rest) >= 3 and rest[0] == "--level":
        instead[rest[1]] = pathlib.Path(rest[2])
        rest = rest[3:]
    else:
        fail(f"{rest[0]}: not an option (--level <path> <file.klevel>)")

project_text = project_file.read_text(encoding="utf-8")
try:
    project = json.loads(project_text)
except json.JSONDecodeError as e:
    fail(f"{project_file}: not JSON ({e})")
if not isinstance(project, dict) or project.get("format") != "kern-project":
    fail(f"{project_file}: format: not a Kern project (format must be \"kern-project\")")
levels = project.get("levels", [])
if not isinstance(levels, list) or not all(isinstance(p, str) and p for p in levels):
    fail(f"{project_file}: levels: must be a list of paths")
for path in instead:
    if path not in levels:
        fail(f"--level {path}: is none of the project's levels ({', '.join(levels) or 'none'})")

texts = []
for i, path in enumerate(levels):
    file = instead.get(path, project_file.parent / path)
    if not file.exists():
        fail(f"{project_file}: levels[{i}] \"{path}\": no such file ({file})")
    text = file.read_text(encoding="utf-8")
    try:
        doc = json.loads(text)
    except json.JSONDecodeError as e:
        fail(f"{file}: not JSON ({e})")
    if not isinstance(doc, dict) or doc.get("format") != "kern-level":
        fail(f"{file}: format: not a Kern level (format must be \"kern-level\")")
    if "project" in doc:
        fail(f"{file}: project: a level holds only its world - the settings belong in the project file ({project_file.name})")
    texts.append((path, text))

compact = json.dumps(project, ensure_ascii=False, separators=(",", ":"))
for what, t in [(project_file.name, compact)] + texts:
    if END in t:
        fail(f"{what}: contains the embedding's end mark {END}")


def literal(t):
    """The text as a string literal: compilers limit one (64 KiB in the standard) - pieces of 8 KiB, joined by the compiler."""
    pieces = [t[i:i + 8192] for i in range(0, len(t), 8192)] or [""]
    return "\n".join('    R"KERN(' + p + END for p in pieces)


def cpp_string(s):
    return json.dumps(s, ensure_ascii=False)


level_arrays = "".join(f"const char level{i}[] =\n{literal(t)};\n" for i, (_, t) in enumerate(texts))
level_cases = "".join(f"  if (path == {cpp_string(p)}) return {{level{i}, sizeof level{i} - 1}};\n" for i, (p, _) in enumerate(texts))
source = f"""// Generated from {project_file.name} by kern/tools/embed_project.py - do not edit (edit the project and its levels).
#include "kern/game/embedded.h"

namespace {{
const char project[] =
{literal(compact)};
{level_arrays}}}  // namespace

namespace kern::game {{

std::string_view embedded_project_text() {{ return {{project, sizeof project - 1}}; }}

std::string_view embedded_level(std::string_view path) {{
{level_cases}  return {{}};
}}

}}  // namespace kern::game
"""
if not out.exists() or out.read_text(encoding="utf-8") != source:
    out.write_text(source, encoding="utf-8")
