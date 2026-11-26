"""A case of the test writer's compile check (TestWriter.compiles): the compiler stopping after so many errors
("too many errors emitted") is no complaint of a piece - the errors before it are, each on its own line."""
import importlib.util
import os
import sys

root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
spec = importlib.util.spec_from_file_location("cpp_to_blueprint", os.path.join(root, "tools", "cpp_to_blueprint.py"))
c2b = importlib.util.module_from_spec(spec)
spec.loader.exec_module(c2b)
w = object.__new__(c2b.TestWriter)
w.test_dir, w.source_dir, w.path, w.flags = os.path.join(root, "tools", "converter_cases"), "src", os.path.join(root, "x.cpp"), []
text = "\n".join(f"int f{i}() {{ return undeclared_{i}; }}" for i in range(30))   # (30 errors, one a line: clang stops at 20)
errors = w.compiles(text, "too_many")
said = [m for _, m in errors]
ok = bool(errors) and not any("too many errors" in m for m in said) and all(line >= 1 for line, _ in errors)
print(("ok   " if ok else "FAIL ") + f"compiles: {len(errors)} complaints, each of its line, none 'too many errors emitted'")
sys.exit(0 if ok else 1)
