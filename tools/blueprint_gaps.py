#!/usr/bin/env python3
"""What a program still lacks to run from its level's Blueprints alone - the named gaps of the way
"only Blueprints" (BLUEPRINTS.md of a game, "5 - Umschalten"; for every game on Kern):

  tools/blueprint_gaps.py --level <level.klevel> --src <dir> [--src <dir>...] [--log <convert.log[.xz]>]
      [--generated <file name>...] [--out <file>]

  1. functions of the program without a Blueprint: what the converter's log of a run over the whole
     program (tools/cpp_to_blueprint.py, FUNCTIONS=1) says of each function that is not written, by
     its Blueprint, with the reason;
  2. exports: the functions the program's C++ gives the page under a fixed name (KERN_EXPORT(name))
     that no Blueprint of the level gives under that name (a function with "export");
  3. the engine calling the program: classes of the program's C++ made of a class of the engine
     (kern::App, kern::ShaderExtension ...: the engine calls their virtual functions) and function
     values (std::function, a lambda handed on as a C function pointer with its void* user data) - what a Blueprint cannot be today;
  4. the entry: the C++ that hands the engine the program's App (kern_main, kern::run).

Nothing is changed. The files the build writes from the level (--generated, default
level_blueprints.cpp, level_blueprints.h, level_data.cpp) are no sources of the program.
"""
import argparse
import collections
import json
import lzma
import os
import re
import sys

EXPORT = re.compile(r"KERN_EXPORT\((\w+)\)")
MADE_OF_ENGINE = re.compile(r"\b(?:class|struct)\s+(\w+)(?:\s+final)?\s*:\s*(?:public\s+)?(?:virtual\s+)?(kern::[\w:]+)")
FUNCTION_VALUE = re.compile(r"std::function<|\[[^\]]*\]\s*\(\s*(?:const\s+)?void\s*\*")   # (a function value; a function handed on as a C function pointer with its user data)
ENTRY = re.compile(r"\bkern_main\s*\(\s*\)\s*\{")
UNIT = re.compile(r"^(\S+)\s+(\d+) /\s+(\d+) functions")
FUNCTION = re.compile(r"^    (\S.*?) \((\d+)\): (.*)$")
WRITTEN = ("written", "a name for a place - written where it is called")


def read(path):
    if path.endswith(".xz"):
        return lzma.open(path, "rt", encoding="utf-8").read()
    return open(path, encoding="utf-8").read()


def sources(dirs, generated):
    for d in dirs:
        for name in sorted(os.listdir(d)):
            if name in generated or not name.endswith((".cpp", ".h", ".hpp", ".cc")):
                continue
            yield os.path.join(d, name), read(os.path.join(d, name))


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--level", required=True)
    ap.add_argument("--src", action="append", required=True)
    ap.add_argument("--log")
    ap.add_argument("--generated", nargs="*", default=["level_blueprints.cpp", "level_blueprints.h", "level_data.cpp"])
    ap.add_argument("--out")
    a = ap.parse_args(argv)
    level = json.loads(read(a.level))
    given = {}   # export name -> "actor::function"
    for actor in level.get("actors", []):
        g = (actor.get("components") or {}).get("logic") or {}
        for f in g.get("functions", []):
            if f.get("export"):
                given[f["export"]] = f"{actor.get('name')}::{f.get('name')}"
    wanted, engine, values, entry = {}, [], [], []
    for path, text in sources(a.src, set(a.generated)):
        rel = os.path.relpath(path)
        for i, line in enumerate(text.split("\n"), 1):
            for m in EXPORT.finditer(line):
                if not line.lstrip().startswith(("#", "//")):
                    wanted.setdefault(m.group(1), f"{rel}:{i}")
            m = MADE_OF_ENGINE.search(line)
            if m and not line.lstrip().startswith("//"):
                engine.append(f"{rel}:{i}: {m.group(1)} : {m.group(2)}")
            if FUNCTION_VALUE.search(line) and not line.lstrip().startswith("//"):
                values.append(f"{rel}:{i}: {line.strip()[:120]}")
            if ENTRY.search(line):
                entry.append(f"{rel}:{i}")
    out = []
    missing_functions = collections.OrderedDict()
    totals = None
    if a.log:
        unit = None
        for line in read(a.log).split("\n"):
            m = UNIT.match(line)
            if m:
                unit = m.group(1)
                continue
            m = FUNCTION.match(line)
            if m and unit and m.group(3) not in WRITTEN and not m.group(3).startswith("not compared"):
                missing_functions.setdefault(unit, []).append(f"{m.group(1)} ({m.group(2)} lines): {m.group(3)}")
            m = re.match(r"^(\d+) of (\d+) functions of the program are written", line)
            if m:
                totals = (int(m.group(1)), int(m.group(2)))
    n_functions = sum(len(v) for v in missing_functions.values())
    missing_exports = sorted(n for n in wanted if n not in given)
    out.append(f"# Lücken des Wegs „nur aus Blueprints“ – Level {os.path.relpath(a.level)}"
               + (f", Protokoll {os.path.basename(a.log)}" if a.log else ""))
    out.append(f"# Funktionen ohne Blueprint: {n_functions}" + (f" (geschrieben {totals[0]} von {totals[1]})" if totals else "")
               + f" · Exporte ohne Blueprint: {len(missing_exports)} von {len(wanted)} · Engine ruft Programm: {len(engine)} Klassen, "
               + f"{len(values)} Funktionswerte · Einstieg in C++: {len(entry)}")
    out.append("")
    out.append("## 1. Funktionen ohne Blueprint (Blueprint: Funktion (Zeilen): Grund)")
    for u, fs in missing_functions.items():
        for f in fs:
            out.append(f"{u}: {f}")
    if not a.log:
        out.append("(kein Protokoll eines vollen Laufs angegeben)")
    out.append("")
    out.append("## 2. Exporte des Programms, die kein Blueprint gibt (Name: wo das C++ ihn erklärt)")
    out += [f"{n}: {wanted[n]}" for n in missing_exports]
    out.append("")
    out.append("## 3. Die Engine ruft das Programm (Klassen aus Klassen der Engine; Funktionswerte)")
    out += engine + values
    out.append("")
    out.append("## 4. Einstieg (die App des Programms in C++)")
    out += entry
    text = "\n".join(out) + "\n"
    if a.out:
        open(a.out, "w", encoding="utf-8").write(text)
    else:
        sys.stdout.write(text)
    gaps = n_functions + len(missing_exports) + len(engine) + len(values) + len(entry)
    print(f"blueprint_gaps: {gaps} gaps ({n_functions} functions, {len(missing_exports)} exports, {len(engine) + len(values)} calls of the engine into the program, {len(entry)} entry)", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
