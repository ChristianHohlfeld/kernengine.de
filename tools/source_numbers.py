#!/usr/bin/env python3
"""What the two passes of the comparison say, function by function (kern/docs/blueprint_cpp.md "Zahlen").

    tools/source_numbers.py <tests folder> <output of the tests> [--right]

<tests folder>: what a run of tools/cpp_to_blueprint.py with KERN_TEST_SOURCE_NUMBERS=1 wrote to --test - for each
Blueprint test_<name>.cpp (the first pass: the source with the numbers of the Blueprint language, as always) and
test_<name>_source.cpp (the second: the source with its own numbers, as C++ computes it). <output of the tests>: what the
built tests printed (their "off:" lines - kern/test/converted.h, Tally - and each test's ok/FAIL line).

For every function compared: written; compared as before: same or differs; compared with the source as it is: same or
differs in n of m calls (the largest difference), or not compared there. Then the counts. With --right the status is 0
only if every function is compared in both passes and the same in both (a case that is right) - else 1.
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
    tests = outcome(open(said, encoding="utf-8", errors="replace").read().split("\n"))
    stems = sorted(n[len("test_"):-len(".cpp")] for n in os.listdir(folder) if n.startswith("test_") and n.endswith(".cpp") and not n.endswith("_source.cpp"))
    n = dict(written=0, before=0, before_same=0, source=0, source_same=0, source_off=0, source_not=0)
    worst = []
    for stem in stems:
        first = functions_of(os.path.join(folder, f"test_{stem}.cpp"))
        second = set(functions_of(os.path.join(folder, f"test_{stem}_source.cpp")))
        t1 = tests.get(f"converted_{stem}")
        t2 = tests.get(f"converted_{stem}_source")
        print(f"{stem}:")
        for f in first:
            n["written"] += 1
            if t1 is None:
                before = "not run"
            else:
                n["before"] += 1
                if f in t1[1]:
                    before = f"differs in {t1[1][f][0]} of {t1[1][f][1]} calls"
                elif f in t1[2]:
                    before = "not compared: " + t1[2][f]
                    n["before"] -= 1
                else:
                    before = "same"
                    n["before_same"] += 1
            if f not in second:
                source = "not compared (its source does not compile with its own numbers in the test - the converter's log says why)"
                n["source_not"] += 1
            elif t2 is None:
                source = "not run"
                n["source_not"] += 1
            else:
                n["source"] += 1
                if f in t2[1]:
                    k, m, by, at = t2[1][f]
                    source = f"differs in {k} of {m} calls (the largest difference {by}: {at})"
                    n["source_off"] += 1
                    worst.append((-1.0 if by == "-" else float(by), k, m, stem, f, at))   # (-: no number differs - a bool, a text: after those of numbers)
                elif f in t2[2]:   # (no call compared - the Blueprint did not run in the test: not a difference, not measured)
                    source = "not compared: " + t2[2][f]
                    n["source"] -= 1
                    n["source_not"] += 1
                else:
                    source = "same"
                    n["source_same"] += 1
            print(f"  {f}: written; compared as before: {before}; compared with the source as it is: {source}")
    print(f"\n{n['written']} functions written and in a test; as before (the language's numbers): {n['before_same']} of {n['before']} the same; "
          f"with the source as it is (its own numbers): {n['source_same']} the same, {n['source_off']} differ, {n['source_not']} not compared")
    if worst:
        print("the largest differences (with the source as it is):")
        for by, k, m, stem, f, at in sorted(worst, key=lambda w: (-w[0], -w[1]))[:int(os.environ.get("TOP", "15"))]:
            print(f"  {stem} {f}: {k} of {m} calls, the largest difference {by}: {at[:300]}")
    if right:
        ok = n["written"] > 0 and n["before_same"] == n["written"] and n["source_same"] == n["written"]
        return 0 if ok else 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
