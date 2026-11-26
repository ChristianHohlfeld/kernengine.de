"""Cases of where the converter places the Blueprints (place_blueprints, apply_to_level; kern/docs/project.md 7a.):
the program's stand in the project, a world's in the level - by what a Blueprint is and who uses it, never by a
folder's name.
  - a Blueprint that stands in the project and that the run does not write (a file of the program that is a Blueprint
    now: its C++ calls it as its class) is the program's: what it uses is the program's too - into the project;
  - a Blueprint of another folder (a source of its own) that the program uses and that stands in the level is never
    moved: the run stops with its name, folder and reason, and the files stay as they are;
  - --classes takes the header only (which Blueprints the program calls as their classes says the project)."""
import importlib.util
import json
import os
import subprocess
import sys
import tempfile

root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
spec = importlib.util.spec_from_file_location("cpp_to_blueprint", os.path.join(root, "tools", "cpp_to_blueprint.py"))
c2b = importlib.util.module_from_spec(spec)
spec.loader.exec_module(c2b)
fail = 0


def check(ok, what):
    global fail
    print(("ok   " if ok else "FAIL ") + "placement: " + what)
    fail |= not ok


# the run writes `lib` (used by the project's `switched`, which it does not write) and `door` (used by no one)
placed = c2b.place_blueprints({"lib", "door"}, {"lib": [], "door": []}, {}, {"lib": "", "door": ""},
                              {"switched": ["lib"], "lib": [], "door": []}, in_project={"switched"})
check(placed.get("lib", ("", ""))[0] == "project" and "switched" in placed["lib"][1],
      f"what a Blueprint of the project uses is the program's ({placed.get('lib')})")
check(placed.get("door", ("", ""))[0] == "level", f"a Blueprint no one of the program uses is a world's ({placed.get('door')})")
check(c2b.place_blueprints({"lib"}, {"lib": []}, {}, {"lib": ""}, {"lib": []})["lib"][0] == "level",
      "without a Blueprint of the program, the same library is a world's")

# a Blueprint of another folder that the program uses, in the level: the run stops, nothing is written
d = tempfile.mkdtemp()
logic = {"kind": "kern-logic", "version": 1, "nextId": 1, "library": True, "variables": [], "nodes": [], "links": [], "machines": [], "functions": []}
level = {"format": "kern-level", "version": 1, "name": "L", "nextId": 2, "actors": [
    {"id": 1, "name": "handmade", "folder": "Rules", "transform": {}, "components": {"logic": logic}}]}
project = {"format": "kern-project", "version": 1, "name": "P", "blueprints": []}
lp, pp = os.path.join(d, "l.klevel"), os.path.join(d, "p.kproject")
json.dump(level, open(lp, "w"))
json.dump(project, open(pp, "w"))
before = open(lp).read(), open(pp).read()
try:
    c2b.apply_to_level(lp, "Cases", [], project=pp, placed={"handmade": ("project", "the program's App uses it")})
    check(False, "a source of its own that the program uses, in the level, stops the run")
except SystemExit as e:
    said = str(e)
    check("handmade" in said and "folder Rules" in said and "its place is the project" in said and "the program's App uses it" in said,
          f"a source of its own that the program uses, in the level, stops the run with its name, folder and reason ({said[:160]})")
check((open(lp).read(), open(pp).read()) == before, "... and the level and the project stay as they are")

# --classes: the header only
r = subprocess.run([sys.executable, os.path.join(root, "tools", "cpp_to_blueprint.py"), "--convert", "cases", "--folder", "Cases", "--level", lp, "--project", pp,
                    "--classes", "level_blueprints.h=Program", "--", "x.cpp"], capture_output=True, text=True)
check(r.returncode != 0 and "no folder" in (r.stderr + r.stdout), "--classes <header.h>=<folder> is refused: the project says which Blueprints the program calls as their classes")
sys.exit(1 if fail else 0)
