#!/usr/bin/env python3
"""trace.py <substring> <converter args...>: the converter, saying where a Wait with that text is raised."""
import importlib.util, os, sys, traceback
tools = "/home/claude/w-g1/tools"
spec = importlib.util.spec_from_file_location("cpp_to_blueprint", os.path.join(tools, "cpp_to_blueprint.py"))
m = importlib.util.module_from_spec(spec); sys.modules["cpp_to_blueprint"] = m; spec.loader.exec_module(m)
want = sys.argv[1]
old = m.Wait.__init__
def init(self, *a, **k):
    old(self, *a, **k)
    if want and any(w in str(a[0] if a else "") for w in want.split("|")):
        who = "?"
        fr = sys._getframe(1)
        while fr is not None:
            me = fr.f_locals.get("self")
            f = getattr(me, "f", None)
            if f is not None and getattr(f, "cpp", None):
                who = f.cpp
                break
            fr = fr.f_back
        sys.stderr.write("---- WAIT in %s: %s\n" % (who, a[0]) + "".join(traceback.format_stack(limit=int(os.environ.get('TRACE_DEPTH','10')))[:-1]))
m.Wait.__init__ = init
sys.exit(m.main([os.path.join(tools, "cpp_to_blueprint.py")] + sys.argv[2:]))
