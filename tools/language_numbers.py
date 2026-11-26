#!/usr/bin/env python3
"""The numbers of the language in a program's sources (kern/docs/blueprint_cpp.md, "Zahlen").

A source of the C++ subset holds and computes its numbers with a fraction as kern::Float (64 bit, kern/core/numbers.h),
the language's; the converter says so with --language-numbers. This tool rewrites the sources of a program to it, or -
with --dry-run - only says what it would do:

  tools/language_numbers.py --dry-run [--report <out.md>] <folder or file>...
  tools/language_numbers.py --write <folder or file>...

What it changes, in code (not in comments, texts or characters):
  - a type f32, f64, float, double (also kern::f32 ...) that holds or computes a number of the program -> Float
    (kern::Float where the source wrote kern:: or float/double), and the include of kern/core/numbers.h in each file
    it changed;
  - a literal with f (0.5f, 1e-6f) -> without it (0.5, 1e-6);
  - a cast to one (`static_cast<f32>(n)`) -> to Float, counted of its own: where it rounded a Float to 32 bit in so
    many words, that rounding is gone (the language rounds only where a Float goes into the engine's f32).
What it cannot see without the compiler - a computation in 32 bit with no type written (`60 * kPi / 180` of the engine's
f32 constant) - the converter says with --language-numbers.
What it leaves as it is and names - each a place where the program meets the engine (or the bytes of a number), to
look at by hand:
  - a line with `override` (the signature of a function of the engine's interface);
  - a list, array, pointer or view of f32/float (`std::vector<f32>`, `f32 v[3]`, `const f32*`, Span<...>): the
    engine's buffer, or a list of the program's own;
  - the bytes of a number (sizeof, bit_cast, memcpy, reinterpret_cast);
  - an export of the program (extern "C");
And it counts the calls of the engine's functions of numbers (the inline functions of kern/math/math.h): there the
program's Float goes into the engine's f32 (rounded) - in the Blueprint today written as a graph of the language.
It does not compile anything: after --write, the program's build and the converter (--language-numbers) say the rest.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TYPE = re.compile(r"(?<![\w:.])(kern::)?(long\s+double|f32|f64|float|double)\b")
LITERAL = re.compile(r"(?<![\w.])((?:\d+\.\d*|\.\d+|\d+(?=[eE]))(?:[eE][+-]?\d+)?)[fF]\b")
KEEP = [
    ("override", "the engine's interface (override)", re.compile(r"\boverride\b")),
    ("bytes", "the bytes of a number (sizeof, bit_cast, memcpy, reinterpret_cast)", re.compile(r"\bsizeof\b|\bbit_cast\b|\bmemcpy\b|\breinterpret_cast\b")),
    ("export", "an export of the program (extern \"C\")", re.compile(r'extern\s+"C"')),
    ("list", "a list, array, pointer or view of f32 (the engine's buffer or a list of the program's own)",
     re.compile(r"(?:vector|array|Span|span|List|Fixed)\s*<\s*(?:const\s+)?(?:kern::)?(?:f32|float)\b|(?:\b(?:f32|float)\s*(?:const\s*)?\*)|\b(?:f32|float)\s+\w+\s*\[")),
]
CAST = re.compile(r"(?:static_cast\s*<\s*(?:const\s+)?|\()(?:kern::)?(?:f32|f64|float|double)\s*[>)]")
SOURCE = (".h", ".hpp", ".cpp", ".cc", ".cxx")


def code_mask(text):
    """For each character of the text: is it code (not a comment, a text, a character)?"""
    mask = bytearray(len(text))
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            i = n if j < 0 else j + 2
        elif c == 'R' and text.startswith('R"', i) and (i == 0 or not (text[i - 1].isalnum() or text[i - 1] == "_")):
            m = re.match(r'R"([^(\s]*)\(', text[i:])
            if not m:
                mask[i] = 1
                i += 1
                continue
            j = text.find(")" + m.group(1) + '"', i)
            i = n if j < 0 else j + len(m.group(1)) + 2
        elif c in "\"'":
            if c == "'" and i > 0 and text[i - 1].isalnum():   # (a digit separator: 1'000)
                mask[i] = 1
                i += 1
                continue
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            i = j + 1
        else:
            mask[i] = 1
            i += 1
    return mask


def engine_number_functions():
    """The names of the engine's functions of numbers (the inline functions kern/math/math.h defines)."""
    path = os.path.join(ROOT, "kern", "include", "kern", "math", "math.h")
    names = set()
    for m in re.finditer(r"^\s*(?:template\s*<[^>]*>\s*)?(?:inline|constexpr|static)[^;{(]*?\b([a-z_][a-z0-9_]*)\s*\(", open(path, encoding="utf-8").read(), re.M):
        names.add(m.group(1))
    return names - {"operator", "if", "for", "while", "return", "sizeof"}


def look(path, engine_names):
    """What the tool finds in one file: (the new text, [(line, category, the line)], {engine function: calls})."""
    text = open(path, encoding="utf-8").read()
    mask = code_mask(text)
    lines = text.split("\n")
    starts, at = [], 0
    for line in lines:
        starts.append(at)
        at += len(line) + 1
    code_lines = ["".join(ch if mask[starts[k] + j] else " " for j, ch in enumerate(line)) for k, line in enumerate(lines)]
    found, edits, calls = [], [], {}
    for k, code in enumerate(code_lines):
        kept = next(((key, why) for key, why, rx in KEEP if rx.search(code)), None)
        for m in TYPE.finditer(code):
            if kept:
                found.append((k + 1, kept[0], lines[k].strip()))
                continue
            found.append((k + 1, "cast" if any(c.start() <= m.start() < c.end() for c in CAST.finditer(code)) else "changed", lines[k].strip()))
            qualified = m.group(1) or m.group(2) in ("float", "double") or re.match(r"long\s+double", m.group(2))
            edits.append((starts[k] + m.start(), starts[k] + m.end(), "kern::Float" if qualified else "Float"))
        for m in LITERAL.finditer(code):
            if kept:
                found.append((k + 1, kept[0] + " (literal)", lines[k].strip()))
                continue
            found.append((k + 1, "literal", lines[k].strip()))
            edits.append((starts[k] + m.start(), starts[k] + m.end(), m.group(1)))
        for m in re.finditer(r"(?<![\w.>])(?:kern::)?([a-z_][a-z0-9_]*)\s*\(", code):
            if m.group(1) in engine_names:
                calls[m.group(1)] = calls.get(m.group(1), 0) + 1
    out = text
    for a, b, new in sorted(edits, reverse=True):
        out = out[:a] + new + out[b:]
    if edits and '#include "kern/core/numbers.h"' not in out:
        incs = list(re.finditer(r'^#include [<"][^>"]+[>"][^\n]*$', out, re.M))
        if incs:
            e = incs[-1].end()
            out = out[:e] + '\n#include "kern/core/numbers.h"' + out[e:]
        else:
            m = re.search(r"^#pragma once[^\n]*$", out, re.M)
            e = m.end() if m else 0
            out = out[:e] + ('\n' if m else '') + '#include "kern/core/numbers.h"\n' + out[e:]
    return out, found, calls


def files_of(paths):
    out = []
    for p in paths:
        if os.path.isdir(p):
            for d, _, names in os.walk(p):
                out += [os.path.join(d, n) for n in sorted(names) if n.endswith(SOURCE)]
        elif os.path.isfile(p):
            out.append(p)
        else:
            raise SystemExit(f"language_numbers.py: {p}: no such folder or file")
    return sorted(out)


def report(results, engine_calls):
    why = {key: text for key, text, _ in KEEP}
    total = {}
    for _, found, _ in results:
        for _, cat, _ in found:
            total[cat] = total.get(cat, 0) + 1
    mine = ("changed", "literal", "cast")
    changed = sum(total.get(c, 0) for c in mine)
    kept = sum(v for c, v in total.items() if c not in mine)
    out = ["# The numbers of the language: what tools/language_numbers.py would change", "",
           f"{len(results)} files, {sum(1 for _, f, _ in results if any(c in ('changed', 'literal', 'cast') for _, c, _ in f))} of them changed.", "",
           "| what | places |", "|---|---|",
           f"| a type f32/f64/float/double of the program -> Float | {total.get('changed', 0)} |",
           f"| a literal with f -> without | {total.get('literal', 0)} |",
           f"| a cast to f32/f64/float/double -> to Float (where it rounded to 32 bit: that rounding is gone - look) | {total.get('cast', 0)} |"]
    for key, text, _ in KEEP:
        n = total.get(key, 0) + total.get(key + " (literal)", 0)
        out.append(f"| left as it is - {text} | {n} |")
    out += ["", f"Changed: {changed}. Left, a place where the program meets the engine or the bytes of a number: {kept}.", ""]
    n_calls = sum(engine_calls.values())
    out += [f"## Calls of the engine's functions of numbers (kern/math/math.h): {n_calls}", "",
            "Where they are called with a Float, the Float goes into the engine's f32 (rounded). "
            "The Blueprint computes them today as a graph of the language (64 bit).", ""]
    out += [", ".join(f"{k} {v}" for k, v in sorted(engine_calls.items(), key=lambda kv: (-kv[1], kv[0]))), ""]
    out += ["## Per file", "", "| file | changed types | literals | casts | left |", "|---|---|---|---|---|"]
    for path, found, _ in results:
        t = sum(1 for _, c, _ in found if c == "changed")
        l = sum(1 for _, c, _ in found if c == "literal")
        c = sum(1 for _, c, _ in found if c == "cast")
        k = len(found) - t - l - c
        if found:
            out.append(f"| {os.path.relpath(path)} | {t} | {l} | {c} | {k} |")
    out += ["", "## Left as it is, by kind (the first lines of each)", ""]
    for key, text, _ in KEEP:
        rows = [(p, line, src) for p, f, _ in results for line, c, src in f if c in (key, key + " (literal)")]
        if not rows:
            continue
        out += [f"### {text}: {len(rows)}", ""]
        seen = set()
        for p, line, src in rows:
            if (p, line) in seen:
                continue
            seen.add((p, line))
            out.append(f"- `{os.path.relpath(p)}:{line}`: `{src[:140]}`")
            if len(seen) >= 12:
                break
        out.append("")
    return "\n".join(out) + "\n"


def main(argv):
    mode = argv[1] if len(argv) > 1 else ""
    if mode not in ("--dry-run", "--write"):
        sys.stderr.write(__doc__)
        return 2
    rest, out_md = argv[2:], None
    if rest[:1] == ["--report"]:
        if len(rest) < 2:
            raise SystemExit("language_numbers.py: --report <out.md>")
        out_md, rest = rest[1], rest[2:]
    if not rest:
        raise SystemExit("language_numbers.py: give the program's folders or files")
    engine_names = engine_number_functions()
    results, engine_calls = [], {}
    for path in files_of(rest):
        new, found, calls = look(path, engine_names)
        results.append((path, found, calls))
        for k, v in calls.items():
            engine_calls[k] = engine_calls.get(k, 0) + v
        if mode == "--write" and new != open(path, encoding="utf-8").read():
            open(path, "w", encoding="utf-8").write(new)
    text = report(results, engine_calls)
    if out_md:
        open(out_md, "w", encoding="utf-8").write(text)
    print(text if not out_md else text.split("\n## Calls")[0])
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
