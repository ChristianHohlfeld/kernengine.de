#!/usr/bin/env python3
"""What the comparison with the source says, function by function (kern/docs/blueprint_cpp.md "Der Beweis").

    tools/source_numbers.py <tests folder> <output of the tests> [--right]

<tests folder>: what a run of tools/cpp_to_blueprint.py wrote to --test - for each Blueprint test_<name>.cpp (the source
with its own numbers, as C++ computes it). <output of the tests>: what the built tests printed (their "off:" lines -
kern/test/converted.h, Tally - and each test's ok/FAIL line).

For every function in a test: the same as its source, or differs in n of m calls (the largest difference), or not
compared. Then the counts. With --right the status is 0 only if every function is compared and the same (a case that
is right) - else 1.
"""
import os
import re
import sys


def functions_of(path):
    """The functions a test compares, in its order (each `converted::Tally tally("<name>")`)."""
    return re.findall(r'converted::Tally tally\("([^"]+)"\)', open(path, encoding="utf-8").read()) if os.path.exists(path) else []


def outcome(lines):
    """{test: (status, {function: (off line)}, {function: other complaint})} of the tests' output: a test's lines stand
    before its ok/FAIL line (test_main.cpp prints each test's lines as one piece)."""
    tests, off, other = {}, {}, {}
    for line in lines:
        m = re.match(r"\s+off: (\S+): (\d+) of (\d+) calls differ; the largest difference (\S+) \((.*)\)$", line)
        if m:
            off[m.group(1)] = (int(m.group(2)), int(m.group(3)), m.group(4), m.group(5))
            continue
        m = re.search(r"check failed .*\| (\S+): (only \d+ calls could be compared.*)$", line)
        if m:
            other[m.group(1)] = m.group(2)
            continue
        m = re.match(r"(ok|FAIL)\s+(converted_\S+)", line)
        if m:
            tests[m.group(2)] = (m.group(1), off, other)
            off, other = {}, {}
    return tests


def main(argv):
    if len(argv) < 3:
        sys.exit(__doc__)
    folder, said, right = argv[1], argv[2], "--right" in argv[3:]
    text = open(said, encoding="utf-8", errors="replace").read()
    tests = outcome(text.split("\n"))
    stems = sorted(n[len("test_"):-len(".cpp")] for n in os.listdir(folder) if n.startswith("test_") and n.endswith(".cpp"))
    n = dict(written=0, same=0, off=0, not_compared=0, failed=0)
    worst = []
    for stem in stems:
        t = tests.get(f"converted_{stem}")
        print(f"{stem}:")
        if t is not None and t[0] == "FAIL":
            n["failed"] += 1
        for f in functions_of(os.path.join(folder, f"test_{stem}.cpp")):
            n["written"] += 1
            if t is None:
                said_f = "not run"
                n["not_compared"] += 1
            elif f in t[1]:
                k, m, by, at = t[1][f]
                said_f = f"differs in {k} of {m} calls (the largest difference {by}: {at})"
                n["off"] += 1
                worst.append((-1.0 if by == "-" else float(by), k, m, stem, f, at))   # (-: no number differs - a bool, a text: after those of numbers)
            elif f in t[2]:   # (no call compared - the Blueprint did not run in the test: not a difference, not measured)
                said_f = "not compared: " + t[2][f]
                n["not_compared"] += 1
            else:
                said_f = "same"
                n["same"] += 1
            print(f"  {f}: {said_f}")
    # (what UBSan says of the source - "runtime error:" -: C++ leaves it undefined, the test does not go on with it)
    ub = [line for line in text.split("\n") if "runtime error:" in line]
    print(f"\n{n['written']} functions in a test: {n['same']} the same as their source, {n['off']} differ, {n['not_compared']} not compared"
          + (f"; {n['failed']} tests fail" if n["failed"] else "") + (f"; UBSan: {len(ub)} (the first: {ub[0].strip()[:300]})" if ub else ""))
    if worst:
        print("the largest differences:")
        for by, k, m, stem, f, at in sorted(worst, key=lambda w: (-w[0], -w[1]))[:int(os.environ.get("TOP", "15"))]:
            print(f"  {stem} {f}: {k} of {m} calls, the largest difference {by}: {at[:300]}")
    if right:
        ok = n["written"] > 0 and n["same"] == n["written"] and not n["failed"] and not ub
        return 0 if ok else 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
