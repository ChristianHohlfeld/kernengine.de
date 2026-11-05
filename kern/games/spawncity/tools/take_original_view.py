#!/usr/bin/env python3
"""Takes the view layer of the original SpawnCity (spawncity.de) from a page snapshot of
tests/compare.mjs: the markup of everything the player sees (title menu, the game view's HUD,
settings, logbook, exit question) and every CSS rule the page had at run time, including the rules
the modules inject. SpawnCity on Kern shows exactly this DOM; web/view.mjs binds it to the game.

  tools/take_original_view.py <compare-dir>[,<compare-dir>...] <web/view dir>

What is left out: scripts, the engineering panel and telemetry (not shown in the game), the
three.js / MapLibre canvases (Kern's canvas takes the three.js canvas's place), and the inline
positions the original's layout modules write while running (they run again on Kern:
web/view/edge_hud_layout.mjs, thumb_layout.mjs).
"""
import re
import sys
from pathlib import Path

from bs4 import BeautifulSoup, Comment

srcs, dst = [Path(p) for p in sys.argv[1].split(",")], Path(sys.argv[2])
src = srcs[0]
dst.mkdir(parents=True, exist_ok=True)

# The richest states: phone (touch HUD) and desktop (key hints, desktop dock), title and in game.
BASE = next(p.stem for p in sorted(src.glob("phone-*-drone-again-original.html")))
soup = BeautifulSoup((src / f"{BASE}.html").read_text(), "lxml")
body = soup.body
for t in body.find_all(["script", "noscript"]):
    t.decompose()
for c in body.find_all(string=lambda s: isinstance(s, Comment)):
    c.extract()
for sel in ["section.panel", "aside.telemetry", "#pairDialog"]:
    for t in body.select(sel):
        t.decompose()

LAYOUT_PROPS = {"inset", "left", "top", "right", "bottom", "transform", "margin", "position", "width", "height",
                "visibility", "display", "opacity", "flex-direction", "align-items", "gap"}


def clean_style(tag):
    st = tag.get("style")
    if not st:
        return
    keep = []
    for decl in st.split(";"):
        if ":" not in decl:
            continue
        k, v = decl.split(":", 1)
        k = k.strip().lower()
        # positions / sizes the layout modules set at run time (always !important there)
        if "!important" in v and k in LAYOUT_PROPS:
            continue
        keep.append(f"{k}:{v.strip()}")
    if keep:
        tag["style"] = ";".join(keep)
    else:
        del tag["style"]


for t in body.find_all(True):
    clean_style(t)
    for a in ["data-edge-adopted", "data-toolbar-guard-observed", "data-ng-fit-text", "data-ng-fit-width", "data-bound",
              "data-move-bound", "data-direct-look-bound", "data-fire-bound"]:
        if t.has_attr(a):
            del t[a]

# Elements other states created that this one does not have (the quit question, dialogs opened later,
# mode HUDs): taken from the first snapshot that has them, at the same parent (body or #viewport).
have = {t.get("id") for t in body.find_all(True) if t.get("id")}
for f in [f for d in srcs for f in sorted(d.glob("*-original.html"))]:
    if f.name == f"{BASE}.html":
        continue
    other = BeautifulSoup(f.read_text(), "lxml").body
    for parent_id in (None, "viewport", "gameMenu"):
        parent = other if parent_id is None else other.find(id=parent_id)
        mine = body if parent_id is None else body.find(id=parent_id)
        if not parent or not mine:
            continue
        for c in list(parent.find_all(True, recursive=False)):
            cid = c.get("id")
            if not cid or cid in have or c.name in ("script", "noscript") or cid in ("pairDialog",):
                continue
            ids = {t.get("id") for t in c.find_all(True) if t.get("id")}
            if ids & have:
                continue
            for t in c.find_all(["script", "noscript"]):
                t.decompose()
            mine.append(c.extract())
            have |= ids | {cid}
            print("added", parent_id or "body", cid)

# Kern's canvas takes the three.js canvas's place (the bare <canvas> child of #viewport).
vp = body.find(id="viewport")
for c in vp.find_all("canvas", recursive=False):
    if not c.get("id") and not c.get("class"):
        c.replace_with(BeautifulSoup('<canvas id="game" tabindex="0" aria-label="SpawnCity"></canvas>', "lxml").canvas)
        break
# MapLibre's own DOM goes; its attribution line stays (Kern draws the same map data).
geo = vp.find(id="geoViewport")
if geo:
    attrib = geo.select_one(".geo-attribution")
    geo.clear()
    del geo["class"]
    if attrib:
        geo.append(attrib)
# The toggles the edge layout adopted into the game view go back to the page (it adopts them again).
for tid in ["soundSwitch", "musicSwitch", "gpsLiveButton"]:
    t = vp.find(id=tid, recursive=False)
    if t:
        body.append(t.extract())
# Menus start in their idle state; web/view.mjs opens what the game says.
menu = body.find(id="gameMenu")
if menu and menu.has_attr("hidden"):
    del menu["hidden"]

html = "".join(str(c) for c in body.children).strip()
html = re.sub(r"\n\s*\n+", "\n", html)
(dst / "original.html").write_text(
    "<!-- The original SpawnCity's view layer (spawncity.de, taken by tools/take_original_view.py). -->\n" + html + "\n")

# CSS: every rule of every captured state, first appearance order, no duplicates. The Google Fonts
# sheet is replaced by the local Inter (web/view/fonts, SIL OFL 1.1).
rules, seen = [], set()
for f in sorted([f for d in srcs for f in d.glob("*-original.css")], key=lambda p: (p.name != f"{BASE}.css", p.name)):
    text = f.read_text()
    # compare.mjs joined cssRules with "\n"; a rule never starts with whitespace, nested lines do.
    cur = []
    for line in text.split("\n"):
        if line.startswith("/*"):
            continue
        if cur and not line[:1].isspace() and not line.startswith("}"):
            r = "\n".join(cur)
            if r not in seen:
                seen.add(r)
                rules.append(r)
            cur = []
        cur.append(line)
    if cur:
        r = "\n".join(cur)
        if r not in seen:
            seen.add(r)
            rules.append(r)
font = """@font-face { font-family: Inter; font-style: normal; font-weight: 100 900; font-display: swap; src: url("fonts/InterVariable.woff2") format("woff2"); }
@font-face { font-family: Inter; font-style: italic; font-weight: 100 900; font-display: swap; src: url("fonts/InterVariable-Italic.woff2") format("woff2"); }
"""
(dst / "original.css").write_text("/* The original SpawnCity's CSS at run time (spawncity.de, taken by tools/take_original_view.py). */\n" + font + "\n".join(rules) + "\n")
print(f"html {len(html)} bytes, css {len(rules)} rules")
