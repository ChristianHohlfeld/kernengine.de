#!/usr/bin/env python3
"""Die Texte eines vollen Laufs (mag-zwischen/texts) lokal ins Level lesen - wie der Konverter am Ende (apply_to_level),
eine Bibliothek vor denen, die sie benutzen. Zeigt den Lesefehler, ohne einen ganzen Lauf auf mag.
   lesen.py <worktree> <texts-dir> [name=datei.patch ...]"""
import importlib.util, os, re, shutil, sys
wt, texts = sys.argv[1], sys.argv[2]
spec = importlib.util.spec_from_file_location("c2b", os.path.join(wt, "tools", "cpp_to_blueprint.py"))
c = importlib.util.module_from_spec(spec); argv, sys.argv = sys.argv, ["x"]; spec.loader.exec_module(c); sys.argv = argv
T = {f[:-4]: open(os.path.join(texts, f)).read() for f in sorted(os.listdir(texts)) if f.endswith(".cpp")}
uses = {}
for n, t in T.items():
    head = t[t.index("struct "):t.index("{")]
    uses[n] = [u for u in re.findall(r"virtual\s+([A-Za-z_]\w*(?:::\w+)*)", head) if u in T and u != n]
order, seen = [], set()
def visit(n):
    if n in seen: return
    seen.add(n)
    for u in uses[n]: visit(u)
    order.append(n)
for n in sorted(T): visit(n)
level = "/home/claude/lese/l.klevel"
shutil.copy(os.path.join(wt, "kern/games/spawncity/level/spawncity.klevel"), level)
print(len(order), "Blueprints; zuerst:", order[:4], "zuletzt:", order[-2:])
try:
    c.apply_to_level(level, "Source", [(n, T[n]) for n in order], prune=True)
    print("GELESEN: das Level nimmt alle Texte an")
except SystemExit as e:
    print("NICHT GELESEN:", e)
