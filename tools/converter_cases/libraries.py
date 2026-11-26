"""The engine's libraries and what Blueprints name of them (cpp_to_blueprint.marked_headers, engine_libraries_missing,
engine_names_missing - the one check, which tools/engine_api.py --check runs on texts too):
- every header of the engine that marks a type for Blueprints (KERN_BLUEPRINT_TYPE) has its library in the build;
- a header that marks a type and has no library stops the converter before it converts anything (an engine whose
  kern/include/kern has one more such header: KERN_ROOT), with the header and the reason - not when the level reads the
  texts at the end (before: hours later, "the library engine_input_actions is not there");
- a text that names a library, a type or a function the build does not have is found, with who names it and the line."""
import importlib.util
import os
import subprocess
import sys
import tempfile

root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
spec = importlib.util.spec_from_file_location("cpp_to_blueprint", os.path.join(root, "tools", "cpp_to_blueprint.py"))
c2b = importlib.util.module_from_spec(spec)
spec.loader.exec_module(c2b)
fail = 0


def said(ok, what):
    global fail
    print(("ok   " if ok else "FAIL ") + "libraries: " + what)
    fail |= not ok


lacking = c2b.engine_libraries_missing()
said(not lacking, "every header of the engine that marks a type has its library" + ("" if not lacking else ": " + "; ".join(lacking)))
said("input/actions.h" in c2b.marked_headers(), "kern/input/actions.h (kern::Key) is one of them")
found = c2b.engine_names_missing({"one": "x = engine_math_math::Quat{};\ny = engine_nowhere_at_all::Thing{};\nz = engine_math_math::no_such_function(1);"})
said([(w, line) for w, line, _ in found] == [("one", 2), ("one", 3)] and "engine_nowhere_at_all: no library" in found[0][2] and "no_such_function" in found[1][2],
     "a text naming a library or a function the build does not have: each with who and the line, nothing else")

with tempfile.TemporaryDirectory() as top:
    kern = os.path.join(top, "kern", "include", "kern")
    os.makedirs(kern)
    for n in os.listdir(os.path.join(root, "kern", "include", "kern")):
        os.symlink(os.path.join(root, "kern", "include", "kern", n), os.path.join(kern, n))
    os.makedirs(os.path.join(kern, "extra"))
    open(os.path.join(kern, "extra", "more.h"), "w").write('#pragma once\n#include "kern/core/base.h"\nnamespace kern { struct KERN_BLUEPRINT_TYPE More { int n = 0; }; }\n')
    os.symlink(os.path.join(root, "tools"), os.path.join(top, "tools"))   # (the engine's tools, as they stand)
    os.makedirs(os.path.join(top, "kern", "src", "logic"))
    os.symlink(os.path.join(root, "kern", "src", "logic", "engine_libraries.gen.cpp"), os.path.join(top, "kern", "src", "logic", "engine_libraries.gen.cpp"))
    src = os.path.join(top, "prog")
    os.makedirs(src)
    open(os.path.join(src, "a.cpp"), "w").write("namespace cases { int one() { return 1; } }\n")
    open(os.path.join(src, "l.klevel"), "w").write('{"name":"Cases","nextId":1,"actors":[]}')
    r = subprocess.run([sys.executable, os.path.join(root, "tools", "cpp_to_blueprint.py"), "--convert", "cases", "--folder", "Cases", f"-I{root}/kern/include",
                        "--level", os.path.join(src, "l.klevel"), "--", os.path.join(src, "a.cpp")], capture_output=True, text=True, env={**os.environ, "KERN_ROOT": top}, cwd=src, timeout=600)
    out = r.stdout + r.stderr
    said(r.returncode != 0 and "kern/extra/more.h marks types for Blueprints, but the engine has no library engine_extra_more" in out and "round 1" not in out,
         "a header that marks a type and has no library stops the converter before round 1, with the header" + ("" if r.returncode and "more.h" in out else ": " + out[-600:]))
sys.exit(1 if fail else 0)
