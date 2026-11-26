#!/usr/bin/env python3
"""The numbers of a converter run, from its log (what tools/cpp_to_blueprint.py printed - a full run's convert.log,
a probe's job log as `gh api .../jobs/<id>/logs` gives it, with its time stamps):

  tools/converter_cases/log_numbers.py <convert.log> [<convert.log of another run>]

- what stops the run: the lines that say the Blueprints were not read, an error of the reader, a Traceback;
- how many functions are written and compared (the converter's own totals; without them - a run that stopped -
  counted from the lines of the functions);
- why the others wait, by how often (the converter's words; "it calls X(), which waits" counted once as a reason,
  with the functions most waited on);
- why a test cannot compare a function yet, and the compiler's complaints about a test (TEST_ERRORS=1), by how often.

With a second log: both side by side, and which functions are written in one and not in the other.
"""
import collections
import re
import sys

STAMP = re.compile(r"^\d{4}-\d\d-\d\dT\d\d:\d\d:\d\d(?:\.\d+)?Z ")
FUNCTION = re.compile(r"^    (\S.*?) \((\d+)\): (.*)$")
WRITTEN = re.compile(r"^(\d+) of (\d+) functions of the program are written")
COMPARED = re.compile(r"^(\d+) functions? of the Blueprints (?:is|are) compared")
# (a name for a place - a function that gives a reference to a place of its own, `T& at(i)` - is written where it is called:
# the converter counts it as written, and so does this)
PLACE = "a name for a place - written where it is called"


def is_written(why):
    return why.startswith("written") or why == PLACE


CALLS = re.compile(r"^it calls (.+?)\(\), which waits")
TEST = re.compile(r"its test cannot compare it yet \((.*)\)")
CLANG = re.compile(r"^--- (\S+?): (?:clang: )?\S*?:\d+:\d+: error: (.*)$")
DROPPED = re.compile(r"^--- (\S+?): without the (\w+) (.+?): (.*)$")


def fatal(line):
    """A line that says the run stopped, or that the reader refused a Blueprint."""
    if line.startswith("---"):
        return False   # (a test's complaint: the test writer goes on without that piece)
    return ("the Blueprints were not read" in line or line.startswith("Traceback") or re.search(r"\.cpp: error: ", line) is not None
            or re.match(r"(?:SystemExit|\w+Error)\b", line) is not None or re.match(r"convert rc=[1-9]", line) is not None)


def numbers(path):
    out = {"fatal": [], "functions": {}, "written": None, "compared": None, "clang": collections.Counter(), "dropped": collections.Counter()}
    blueprint = None
    for raw in open(path, encoding="utf-8", errors="replace"):
        line = STAMP.sub("", raw.rstrip("\n"))
        if fatal(line):
            out["fatal"].append(line[:400])
        m = re.match(r"^(\S+)\s+\d+ /\s+\d+ functions", line)
        if m:
            blueprint = m.group(1)
            continue
        m = FUNCTION.match(line)
        if m and blueprint is not None:
            out["functions"][(blueprint, m.group(1))] = m.group(3)
            continue
        if not line.startswith("    "):
            blueprint = None if not line.strip() or line.startswith(("where ", "what ")) else blueprint
        if WRITTEN.match(line):
            out["written"] = tuple(int(x) for x in WRITTEN.match(line).groups())
        if COMPARED.match(line):
            out["compared"] = int(COMPARED.match(line).group(1))
        m = CLANG.match(line)
        if m:
            out["clang"][re.sub(r"converted_\w+?::", "converted_<test>::", m.group(2))] += 1
        m = DROPPED.match(line)
        if m:
            out["dropped"][f"{m.group(2)} {m.group(3)}: {m.group(4)}"[:200]] += 1
    return out


def reason_of(why):
    """The reason a function waits, as it is counted: the call it waits on, its test's reason, or the words themselves."""
    if CALLS.match(why):
        return "it calls X(), which waits"
    m = TEST.search(why)
    if m:
        return "its test cannot compare it yet: " + re.sub(r" \(it went round with .*\)$", "", m.group(1))[:160]
    return re.sub(r" \(it went round with .*\)$", "", why)[:200]


def summary(name, n, top=25):
    lines = [f"== {name}"]
    lines.append("stops the run: " + ("nothing" if not n["fatal"] else f"{len(n['fatal'])} lines"))
    lines += ["  " + x for x in n["fatal"][:20]]
    written = sum(1 for why in n["functions"].values() if is_written(why))
    waiting = {k: why for k, why in n["functions"].items() if not is_written(why)}
    total = n["written"] or (written, len(n["functions"]))
    lines.append(f"written: {total[0]} of {total[1]} functions" + ("" if n["written"] else " (counted from the lines of the functions: the converter's total is missing)"))
    lines.append(f"compared: {n['compared'] if n['compared'] is not None else '-'}")
    tests = sum(1 for why in waiting.values() if TEST.search(why))
    lines.append(f"waiting: {len(waiting)} ({tests} of them only for their test)")
    reasons = collections.Counter(reason_of(why) for why in waiting.values())
    lines.append("why they wait (by how often):")
    lines += [f"  {c:5}  {r}" for r, c in reasons.most_common(top)]
    called = collections.Counter(CALLS.match(why).group(1) for why in waiting.values() if CALLS.match(why))
    if called:
        lines.append("most waited on (it calls X(), which waits):")
        lines += [f"  {c:5}  {x}" for x, c in called.most_common(10)]
    if n["clang"]:
        lines.append(f"the compiler on the tests: {sum(n['clang'].values())} errors")
        lines += [f"  {c:5}  {x[:200]}" for x, c in n["clang"].most_common(top)]
    if n["dropped"]:
        lines.append(f"pieces the tests go without: {sum(n['dropped'].values())}")
        lines += [f"  {c:5}  {x}" for x, c in n["dropped"].most_common(10)]
    return lines


def main(argv):
    if len(argv) not in (2, 3):
        sys.exit(__doc__.strip())
    logs = [(p, numbers(p)) for p in argv[1:]]
    for p, n in logs:
        print("\n".join(summary(p, n)))
        print()
    if len(logs) == 2:
        (pa, a), (pb, b) = logs
        wa = {k for k, why in a["functions"].items() if is_written(why)}
        wb = {k for k, why in b["functions"].items() if is_written(why)}
        print(f"== written in {pb}, not in {pa}: {len(wb - wa)}")
        for k in sorted(wb - wa)[:60]:
            print(f"  {k[0]}: {k[1]}  (before: {a['functions'].get(k, 'not named')[:120]})")
        print(f"== written in {pa}, not in {pb}: {len(wa - wb)}")
        for k in sorted(wa - wb)[:60]:
            print(f"  {k[0]}: {k[1]}  (now: {b['functions'].get(k, 'not named')[:120]})")


if __name__ == "__main__":
    main(sys.argv)
