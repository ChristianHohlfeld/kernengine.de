# Debug wrapper (not part of the repo): runs tools/cpp_to_blueprint.py and logs, for every Wait raised,
# the reason and the source position of the innermost AST node found in the frames that raised it.
import os, sys, inspect, runpy
tool = sys.argv[1]
sys.argv = [tool] + sys.argv[2:]
sys.path.insert(0, os.path.dirname(tool))
import cpp_to_blueprint as C
LOG = os.environ["WHERE_LOG"]
_lines = {}

def line_of(path, off):
    if path not in _lines:
        try:
            data = open(path, "rb").read()
        except OSError:
            _lines[path] = None
            return None
        starts = [0]
        for i, b in enumerate(data):
            if b == 10:
                starts.append(i + 1)
        _lines[path] = starts
    st = _lines[path]
    if st is None or off is None:
        return None
    import bisect
    return bisect.bisect_right(st, off)

def init(self, *a):
    Exception.__init__(self, *a)
    fr = inspect.currentframe().f_back
    where = []
    depth = 0
    while fr is not None and depth < 40 and len(where) < 3:
        for v in fr.f_locals.values():
            if isinstance(v, dict) and v.get("_at") and v.get("_at")[0] is not None and v.get("_file"):
                ln = line_of(v["_file"], v["_at"][0])
                where.append(f"{os.path.basename(v['_file'])}:{ln}({v.get('kind')})@{fr.f_code.co_name}")
                break
        fr = fr.f_back
        depth += 1
    fn = ""
    fr = inspect.currentframe().f_back
    while fr is not None:
        s = fr.f_locals.get("self")
        g = getattr(s, "f", None)
        if g is not None and getattr(g, "name", None):
            fn = g.name
            break
        fr = fr.f_back
    with open(LOG, "a") as f:
        f.write(f"[{fn}] {a[0] if a else ''} || {' < '.join(where)}\n")

C.Wait.__init__ = init
sys.exit(C.main(sys.argv))
