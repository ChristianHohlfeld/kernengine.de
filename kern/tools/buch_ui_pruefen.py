#!/usr/bin/env python3
"""Checks that a book names Kern Studio's menus, buttons and fields exactly as Studio shows them.

    python3 kern/tools/buch_ui_pruefen.py kern/docs/buch-gelb            (a German edition)
    python3 kern/tools/buch_ui_pruefen.py --en kern/docs/book-yellow     (an English edition)

Studio shows German labels on German systems (kern/web/studio/game.mjs, I18N_DE and
I18N_PATTERNS). The German editions name a control as Studio shows it in German, with the English
label in parentheses: **Datei › Beispiel-Levels** (File › Example Levels). The English editions
use the English label alone. Shortcuts follow the keyboard: Strg in German, Ctrl in English.

Reported, exit status 1:
- a German label whose English counterpart in parentheses does not translate to it,
- in a German edition: a bold English label that Studio translates, with no German beside it,
- in an English edition: a bold label that is Studio's German text.
"""
import argparse, json, pathlib, re, sys

here = pathlib.Path(__file__).resolve().parent
game = (here.parent / "web" / "studio" / "game.mjs").read_text()


def table(name):
    a = game.index(f"const {name} = {{")
    b = game.index("\n};", a)
    return dict(re.findall(r'"((?:[^"\\]|\\.)*)":\s*"((?:[^"\\]|\\.)*)"', game[a:b]))


DE = table("I18N_DE")
# Patterns that translate a known phrase inside a longer label (same as tr() in game.mjs).
PATTERNS = [(re.compile(r"^Add (.+)$"), "{} hinzufügen"), (re.compile(r"^Remove (.+)$"), "{} entfernen")]
SEP = re.compile(r"\s*[›>]\s*")
SHORTCUT = re.compile(r"^((Ctrl|Strg|Alt|Shift|Umschalt|Cmd)\+\S+|F\d+|Esc|Entf|Del|Home|Pos1|[A-Z0-9.]|Space|Leertaste)$")


def tr(s):
    s = s.strip()
    if s in DE: return DE[s]
    for rx, rep in PATTERNS:
        m = rx.match(s)
        if m and m.group(1) in DE: return rep.format(DE[m.group(1)])
    if re.match(r"^(Ctrl|Shift|Alt)\+", s):
        return s.replace("Ctrl", "Strg").replace("Shift", "Umschalt").replace("Space", "Leertaste").replace("Delete", "Entf")
    return None


def segments(s):
    return [p for p in SEP.split(s.strip()) if p]


ap = argparse.ArgumentParser()
ap.add_argument("folder")
ap.add_argument("--en", action="store_true")
a = ap.parse_args()

german_values = {v for k, v in DE.items() if v != k}
problems, pairs = [], 0
for f in sorted(pathlib.Path(a.folder).glob("*.md")):
    if not re.match(r"^(\d\d|[A-Z])-", f.name): continue
    text = re.sub(r"```.*?```", "", f.read_text(), flags=re.S)
    for no, line in enumerate(text.splitlines(), 1):
        if a.en:
            for m in re.finditer(r"\*\*([^*]+)\*\*", line):
                for seg in segments(m.group(1)):
                    if seg in german_values and seg not in DE:
                        problems.append(f"{f.name}:{no}: German label in the English book: {seg!r}")
            continue
        # German: **Deutsch › Pfad** (English › Path); a shortcut in parentheses is not a label.
        for m in re.finditer(r"\*\*([^*]+)\*\*(?:\s*\(((?:[^()]|\([^()]*\))*)\))?", line):  # one level of brackets inside: (Kill Z (respawn below))
            de, paren = segments(m.group(1)), m.group(2)
            if de and all(SHORTCUT.match(x) for x in de): continue  # **F** (Focus): a key, then what it does
            en = segments(paren) if paren else []
            known = [tr(e) for e in en] if en and not all(SHORTCUT.match(e) for e in en) else []
            if known and any(k is not None for k in known):
                pairs += 1
                if len(de) != len(en):
                    problems.append(f"{f.name}:{no}: {m.group(1)!r} has {len(de)} parts, ({paren}) has {len(en)}")
                    continue
                for d, e, k in zip(de, en, known):
                    if k is not None and d != k:
                        problems.append(f"{f.name}:{no}: {e!r} shows in German as {k!r}, the book says {d!r}")
                continue
            # No English label beside it: the bold text must not be an English label Studio translates.
            if de and all(tr(x) not in (None, x) for x in de) and not any(x in german_values for x in de):
                problems.append(f"{f.name}:{no}: English label without German: {m.group(1)!r} (German: {' › '.join(tr(x) for x in de)})")

print(f"{a.folder}: {pairs} label pairs checked, {len(problems)} problems")
try:
    for p in problems: print("  " + p)
except BrokenPipeError: pass
sys.exit(1 if problems else 0)
