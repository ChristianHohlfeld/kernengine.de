#!/usr/bin/env python3
"""trace.py <regex> <converter args...>: the converter, saying for every function whose C++ name matches the
regex why it waits and where: the innermost pieces of source at hand when the Wait was raised."""
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
old_init = m.Wait.__init__
def init(self, *a, **k):
    old_init(self, *a, **k)
    f = sys._getframe(1)
    out, n = [], 0
    while f is not None and n < 30 and len(out) < 5:
        for key, v in f.f_locals.items():
            if isinstance(v, dict) and v.get("_at") and v.get("_file") and v["_at"][0] is not None:
                w = (v["_file"], v["_at"][0], v["_at"][1], v.get("kind"), f.f_code.co_name + "." + key)
                if w[:3] not in [x[:3] for x in out]:
                    out.append(w)
        f = f.f_back; n += 1
    self._where = out
    self._stack = " < ".join(x.name for x in reversed(traceback.extract_stack(limit=16)[:-1]))
m.Wait.__init__ = init
old_fn = m.Unit.function
def function(self, decl_id):
    try:
        return old_fn(self, decl_id)
    except m.Wait as w:
        f = self.functions.get((self.tree.body.get(decl_id) or {}).get("id"))
        name = getattr(f, "cpp", None) or "?"
        if want.search(name) and not getattr(w, "_said", False):
            w._said = True
            sys.stderr.write("---- %s: %s\n" % (name, w))
            for file, a, b, kind, key in getattr(w, "_where", []):
                t = text(file)
                sys.stderr.write("   %s:%d [%s %s] %s\n" % (os.path.basename(file), t.count("\n", 0, a) + 1, kind, key, t[a:b or a + 60].replace("\n", " ")[:200]))
            sys.stderr.write("   stack: %s\n" % getattr(w, "_stack", ""))
        raise
m.Unit.function = function
sys.exit(m.main([os.path.join(tools, "cpp_to_blueprint.py")] + sys.argv[2:]))
