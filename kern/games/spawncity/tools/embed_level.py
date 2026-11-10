#!/usr/bin/env python3
"""Writes the level file into a C++ source the game links (src/level_data.cpp, generated).

    tools/embed_level.py <level.klevel> <out.cpp>

The level (level/spawncity.klevel) is SpawnCity's project file: world settings, actors and their
Blueprints. The game starts from it (rules.h); the build and the tests call this first. The
output is only rewritten when it changes, so nothing recompiles for an unchanged level.
"""
import json, pathlib, sys

level, out = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2])
text = level.read_text(encoding="utf-8")
doc = json.loads(text)  # a level that is not JSON stops the build here
if doc.get("format") != "kern-level":
    sys.exit(f"{level}: not a Kern level")
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
