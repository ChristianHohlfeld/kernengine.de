#!/usr/bin/env python3
"""see.py <regex> <converter args...>: the converter as a diagnosis - for every function whose C++ name matches the
regex, EVERY statement that waits is named with its reason and its place, and the conversion of the function goes
on behind it (a statement that waits is left out). What the run writes is worthless (every such function is made
to wait at its end); what it says is what stands behind the first reason - the next layers."""
import importlib.util, os, re, sys, traceback
tools = "/home/claude/w-g2/tools"
spec = importlib.util.spec_from_file_location("cpp_to_blueprint", os.path.join(tools, "cpp_to_blueprint.py"))
m = importlib.util.module_from_spec(spec); sys.modules["cpp_to_blueprint"] = m; spec.loader.exec_module(m)
want = re.compile(sys.argv[1])
texts = {}
def text(f):
    if f not in texts:
        try: texts[f] = open(f, errors="replace").read()
        except OSError: texts[f] = ""
    return texts[f]
def line_of(n):
    if not isinstance(n, dict) or not n.get("_at") or n["_at"][0] is None or not n.get("_file"):
        return "?"
    t = text(n["_file"])
    return "%s:%d" % (os.path.basename(n["_file"]), t.count("\n", 0, n["_at"][0]) + 1)
said = set()
def note(fn, msg, s):
    key = (fn, msg, line_of(s))
    if key in said:
        return
    said.add(key)
    a, b = (s.get("_at") or (None, None)) if isinstance(s, dict) else (None, None)
    piece = text(s.get("_file", ""))[a:b].replace("\n", " ")[:110] if a is not None and b is not None else ""
    sys.stderr.write("SEE %s | %s | %s | %s\n" % (fn, line_of(s), msg, piece))
old_stmt = m.Emit.stmt
def stmt(self, s):
    fn = getattr(self.f, "cpp", "?")
    if not want.search(fn):
        return old_stmt(self, s)
    try:
        return old_stmt(self, s)
    except m.Wait as w:
        note(fn, str(w), s)
    except Exception as e:   # (what follows from a statement left out: a name that is not there ...)
        note(fn, "(follows from what was left out: %s %s)" % (type(e).__name__, str(e)[:80]), s)
    self._left_out = getattr(self, "_left_out", 0) + 1
    if s.get("kind") == "DeclStmt":   # (its variables are known all the same, by their types - what uses them goes on)
        for v in m.kids(s):
            if v.get("kind") == "VarDecl" and v.get("id") not in self.names:
                try:
                    self.local(v)
                except Exception:
                    try:
                        self.local(v, "Int")
                    except Exception:
                        pass
    self.pre = []
    return [], False
m.Emit.stmt = stmt
old_run = m.Emit.run
def run(self):
    out = old_run(self)
    if getattr(self, "_left_out", 0):
        raise m.Wait("(diagnosis: %d statements of it wait)" % self._left_out)
    return out
m.Emit.run = run
sys.exit(m.main([os.path.join(tools, "cpp_to_blueprint.py")] + sys.argv[2:]))
