#!/usr/bin/env python3
"""The book's cover for an Amazon KDP hardcover (case laminate): back, spine and front in one PDF.

    python3 kern/tools/buch_umschlag.py --pages 452 [--en] [--out kern-buch-umschlag.pdf]
    python3 kern/tools/buch_umschlag.py --pages 300 --gruen   (the green edition: "Einfach erklärt")
    python3 kern/tools/buch_umschlag.py --pages 250 --gelb    (the yellow edition for 12 to 16: "Für Entdecker")

The cover's size follows from the trim size and the page count:
  width  = 2 x trim width + spine + 2 x hinge + 2 x (wrap + overhang)
  height = trim height + 2 x (wrap + overhang)
  spine  = pages x paper + boards
with KDP's case-laminate allowances (wrap 0.591 in, hinge 0.197 in, board overhang 0.118 in) and
white paper (0.002252 in a page) plus 0.19 in for the boards. Check the numbers against the cover
template KDP offers for the final page count before uploading: --spine sets the spine directly.
Text, logo and frame are vector; the screenshot is embedded at its full resolution.
"""
import argparse, base64, pathlib, sys

here = pathlib.Path(__file__).resolve().parent
img_dir = here.parent / "docs" / "buch" / "bilder"

ap = argparse.ArgumentParser()
ap.add_argument("--pages", type=int, required=True)
ap.add_argument("--en", action="store_true")
ap.add_argument("--out", default=None)
ap.add_argument("--trim", default="7x10", help="trim size in inches")
ap.add_argument("--spine", type=float, default=0, help="spine width in inches (default: from the page count)")
ap.add_argument("--gruen", action="store_true", help="the green edition (kern/docs/buch-gruen): same content, explained more simply")
ap.add_argument("--gelb", action="store_true", help="the yellow edition (kern/docs/buch-gelb) for readers from 12 to 16")
ap.add_argument("--image", default="studio.png", help="the front picture (in kern/docs/buch/bilder); no duck on the cover of a book that is sold: the robot is Pollen Robotics'")
a = ap.parse_args()

W, H = (float(x) for x in a.trim.split("x"))
# KDP's cover calculator (hardcover, white paper): spine = pages x 0.002252 in + 0.189 in; the full
# cover is 2 x trim width + spine + 2 x 0.787 in wide and trim height + 2 x 0.708 in high (checked:
# 482 pages -> 1213.1 x 822 pt, spine 1.274 in; 458 pages -> 1209.2 x 822 pt, spine 1.220 in).
# Outside the trim: 0.708 in of wrap and board overhang on the outer edges, 0.079 in at each side
# of the spine. Safe zones: text 0.635 in from every trim edge and 0.4 in from the spine (hinge).
MX, MY, SPINE_SIDE = 0.7083, 0.70833, 0.0789  # 2 x (MX + SPINE_SIDE) = 1.5744 in, 2 x MY = 1.41667 in (822 pt high)
SAFE, HINGE_SAFE = 0.635, 0.4
S = a.spine or round(a.pages * 0.002252 + 0.189, 3)
TW, TH = 2 * W + S + 2 * SPINE_SIDE + 2 * MX, H + 2 * MY
M = MY
out = pathlib.Path(a.out or ("kern-book-cover.pdf" if a.en else "kern-gruen-umschlag.pdf" if a.gruen else "kern-gelb-umschlag.pdf" if a.gelb else "kern-buch-umschlag.pdf")).resolve()
# Each edition has its colour: blue for the technical book, green for the simpler one.
C = (dict(bg1="#7a5c00", bg2="#3d2e00", bg3="#1e1700", acc="#ffd84a", line="#ffc400", light="#fff1c2", blurb="#fff6dc", rights="#c9b47a", grid="#ffd84a")
     if a.gelb else
     dict(bg1="#1d4a33", bg2="#0f2a1d", bg3="#08170f", acc="#a8e6b8", line="#3ddc84", light="#d8f0df", blurb="#e0f2e6", rights="#93b9a0", grid="#a8e6b8")
     if a.gruen else
     dict(bg1="#1b3358", bg2="#0d1830", bg3="#070d1a", acc="#9cc4ff", line="#3d8bff", light="#d5e2f5", blurb="#dbe5f4", rights="#8ea3c4", grid="#9cc4ff"))

def data(name):
    p = img_dir / name
    if not p.exists(): p = img_dir / "studio.png"
    return "data:image/png;base64," + base64.b64encode(p.read_bytes()).decode()

logo = f'<img src="{data("kern-logo.png")}" alt="Kern">'  # sRGB raster: the same in every viewer
de = not a.en
t = {
    "book": "Das Buch" if de else "The Book",
    "sub": ("Die Engine, Kern Studio, ein Laufroboter und der Lehrer" if de
            else "The Engine, Kern Studio, a Walking Robot and the Lehrer"),
    "tag": ("Verstehen, benutzen, weiterentwickeln" if de else "Understand it, use it, build on it"),
    "blurb": ("""<p><b>Kern</b> ist eine 3D-Engine in C++20, die im Browser läuft: WebAssembly und WebGL2, Physik mit Box3D,
eine eigene Sound-Engine, Logik-Graphen mit JIT und Netzwerkspiel. Dazu <b>Kern Studio</b>, ein Editor im Stil der großen Engines,
und <b>der Lehrer</b>, ein Trainingssystem für neuronale Netze in C++ – nativ und im Browser, bit-identisch.</p>
<p>Mit ihm lernt ein kleiner zweibeiniger Laufroboter, auf den Original-Netzen seines Herstellers zu laufen, zu springen,
einen Ball zu kicken und eine Socke zu holen.</p>
<p>Dieses Buch erklärt das Ganze von Grund auf: wie die Engine aufgebaut ist und warum, wie man sie bedient und eigene Spiele baut,
wie Rendering, Physik und neuronale Netze funktionieren – und wie man eigene Modelle trainiert.</p>
<p><b>Christian Heinrich Hohlfeld</b>, Softwareentwickler seit 2010, hat Kern entworfen und mit KI gebaut.</p>
<p>Die Software ist Open Source und kostenlos: <b>kernengine.de</b></p>""" if de else
"""<p><b>Kern</b> is a 3D engine in C++20 that runs in the browser: WebAssembly and WebGL2, physics with Box3D,
its own sound engine, logic graphs with a JIT and networked play. With it come <b>Kern Studio</b>, an editor in the style of the big engines,
and <b>the Lehrer</b>, a training system for neural networks in C++ – native and in the browser, bit for bit identical.</p>
<p>With it a small two-legged walking robot learns, on its maker's original networks, to walk, to jump,
kick a ball and fetch a sock.</p>
<p>This book explains it all from the ground up: how the engine is built and why, how to use it and build your own games,
how rendering, physics and neural networks work – and how to train your own models.</p>
<p><b>Christian Heinrich Hohlfeld</b>, a software engineer since 2010, designed Kern and built it with AI.</p>
<p>The software is open source and free: <b>kernengine.de</b></p>"""),
}
if a.gelb:
    t = {
        "book": "Für Entdecker",
        "sub": "Wie Spiele, Physik und lernende Roboter funktionieren – für Jugendliche ab 12",
        "tag": "Missionen, Rätsel und echte Technik",
        "blurb": """<p>Wie kommt ein 3D-Spiel in deinen Browser? Warum fällt ein Würfel genau so, wie er fällt? Und wie lernt ein
kleiner Roboter laufen, ohne dass ihm jemand jede Bewegung vorschreibt?</p>
<p><b>Kern</b> ist eine echte 3D-Engine, die im Browser läuft – mit Grafik, Physik, Sound, Mehrspieler und dem Editor
<b>Kern Studio</b>, in dem du eigene Level und Spiele baust. Mit dem <b>Lehrer</b> lernt darin die Micro Duck, ein
zweibeiniger Laufroboter, zu laufen, zu springen, einen Ball zu kicken und eine Socke zu holen.</p>
<p>Dieses Buch erklärt alles davon – vollständig und ohne Abkürzungen, mit Mathe aus der Schule, mit Missionen zum
Selbermachen und einer Rätsel-Ecke in jedem Kapitel. Die gelbe Ausgabe hat denselben Inhalt wie
„Kern – Das Buch“ und „Kern – Einfach erklärt“.</p>
<p><b>Christian Heinrich Hohlfeld</b>, Softwareentwickler seit 2010, hat Kern entworfen und mit KI gebaut.</p>
<p>Die Software ist Open Source und kostenlos: <b>kernengine.de</b></p>""",
    }
elif a.gruen:
    t = {
        "book": "Einfach erklärt",
        "sub": "Die Engine, Kern Studio, ein Laufroboter und der Lehrer – für alle, die verstehen wollen, wie Spiele und lernende Roboter funktionieren",
        "tag": "Mitmachen, ausprobieren, verstehen",
        "blurb": """<p>Wie kommt ein 3D-Spiel in den Browser? Warum fällt ein Würfel genau so, wie er fällt? Und wie bringt man einem
kleinen Roboter das Laufen bei, ohne ihm jede Bewegung vorzuschreiben?</p>
<p><b>Kern</b> ist eine echte 3D-Engine, die im Browser läuft – mit Grafik, Physik, Klang, Netzwerkspiel und dem Editor
<b>Kern Studio</b>. Mit dem <b>Lehrer</b> lernt darin die Micro Duck, ein zweibeiniger Laufroboter, zu laufen, zu springen,
einen Ball zu kicken und eine Socke zu holen.</p>
<p>Dieses Buch erklärt alles davon – vollständig, aber ohne Vorwissen: mit Alltagsbildern statt Formelwüsten, mit
Experimenten zum Selbermachen und einer Knobelecke in jedem Kapitel. Die grüne Ausgabe hat denselben Inhalt wie
„Kern – Das Buch“, nur einfacher erzählt.</p>
<p><b>Christian Heinrich Hohlfeld</b>, Softwareentwickler seit 2010, hat Kern entworfen und mit KI gebaut.</p>
<p>Die Software ist Open Source und kostenlos: <b>kernengine.de</b></p>""",
    }
if a.en and a.gelb:
    t = {
        "book": "For Explorers",
        "sub": "How Games, Physics and Learning Robots Work – for Young Readers Aged 12 and Up",
        "tag": "Missions, puzzles and real technology",
        "blurb": """<p>How does a 3D game get into your browser? Why does a cube fall exactly the way it does? And how does a
small robot learn to walk without anyone telling it every move?</p>
<p><b>Kern</b> is a real 3D engine that runs in the browser – with graphics, physics, sound, multiplayer and the editor
<b>Kern Studio</b>, where you build your own levels and games. With the <b>Lehrer</b>, the Micro Duck, a two-legged
walking robot, learns to walk, to jump, to kick a ball and to fetch a sock.</p>
<p>This book explains all of it – completely and without shortcuts, with school math, with missions to try yourself
and a puzzle corner in every chapter. The yellow edition has the same content as
<i>Kern – The Book</i> and <i>Kern – Simply Explained</i>.</p>
<p><b>Christian Heinrich Hohlfeld</b>, a software engineer since 2010, designed Kern and built it with AI.</p>
<p>The software is open source and free: <b>kernengine.de</b></p>""",
    }
elif a.en and a.gruen:
    t = {
        "book": "Simply Explained",
        "sub": "The Engine, Kern Studio, a Walking Robot and the Lehrer – for Everyone Who Wants to Understand How Games and Learning Robots Work",
        "tag": "Try it, play with it, understand it",
        "blurb": """<p>How does a 3D game get into the browser? Why does a cube fall exactly the way it does? And how do you teach
a small robot to walk without prescribing every move?</p>
<p><b>Kern</b> is a real 3D engine that runs in the browser – with graphics, physics, sound, networked play and the editor
<b>Kern Studio</b>. With the <b>Lehrer</b>, the Micro Duck, a two-legged walking robot, learns to walk, to jump,
to kick a ball and to fetch a sock.</p>
<p>This book explains all of it – completely, but without prior knowledge: with everyday pictures instead of walls of
formulas, with experiments to try yourself and brain teasers in every chapter. The green edition has the same
content as <i>Kern – The Book</i>, told more simply.</p>
<p><b>Christian Heinrich Hohlfeld</b>, a software engineer since 2010, designed Kern and built it with AI.</p>
<p>The software is open source and free: <b>kernengine.de</b></p>""",
    }
html = f"""<!doctype html><html><head><meta charset="utf-8"><style>
@page {{ size: {TW}in {TH}in; margin: 0 }}
html, body {{ margin: 0; padding: 0 }}
body {{ width: {TW}in; height: {TH}in; position: relative; overflow: hidden;
  background: radial-gradient(ellipse at 75% 30%, {C["bg1"]} 0%, {C["bg2"]} 55%, {C["bg3"]} 100%);
  font-family: 'DejaVu Sans', sans-serif; color: #e8eef8 }}
.grid {{ position: absolute; inset: 0; opacity: .07;
  background-image: linear-gradient({C["grid"]} 1px, transparent 1px), linear-gradient(90deg, {C["grid"]} 1px, transparent 1px);
  background-size: 0.5in 0.5in }}
.panel {{ position: absolute; top: {M}in; height: {H}in }}
.back {{ left: {MX}in; width: {W}in }}
.spine {{ left: {MX + W + SPINE_SIDE}in; width: {S}in }}
.front {{ left: {MX + W + SPINE_SIDE + S + SPINE_SIDE}in; width: {W}in }}
.front .logo img {{ width: 1.15in; height: 1.15in }}
.front .logo {{ position: absolute; left: 0.75in; top: 0.85in }}
.front h1 {{ position: absolute; left: 0.75in; top: 2.05in; margin: 0; font-size: 92pt; letter-spacing: -2pt; font-weight: bold; color: #fff }}
.front .book {{ position: absolute; left: 0.8in; top: 3.45in; font-size: 24pt; color: {C["acc"]} }}
.front .sub {{ position: absolute; left: 0.8in; right: 0.7in; top: 4.0in; font-size: 14pt; line-height: 1.35; color: {C["light"]} }}
.front .shot {{ position: absolute; left: 0.75in; right: 0.7in; top: 5.05in; height: 3.45in; border-radius: 10px; overflow: hidden;
  box-shadow: 0 0 0 1.5px {C["line"]}, 0 10px 40px rgba(0,0,0,.6) }}
.front .shot img {{ display: block; width: 100%; height: 100%; object-fit: cover; object-position: 50% 60% }}
.front .author {{ position: absolute; left: 0.8in; bottom: 1.0in; font-size: 17pt; color: #fff }}
.front .tag {{ position: absolute; left: 0.8in; bottom: 0.72in; font-size: 10pt; color: {C["acc"]}; letter-spacing: .5pt }}
.spine .inner {{ position: absolute; left: 50%; top: 50%; transform: translate(-50%, -50%) rotate(90deg);
  white-space: nowrap; font-size: {min(16, S * 72 * 0.42):.1f}pt; display: flex; align-items: center; gap: 0.25in }}
.spine .inner b {{ color: #fff }} .spine .inner span {{ color: {C["acc"]} }}
.spine .mark {{ position: absolute; left: 50%; bottom: 0.72in; transform: translateX(-50%) }}
.spine .mark img {{ width: {min(0.6, S * 0.7):.2f}in; height: {min(0.6, S * 0.7):.2f}in }}
.back .logo img {{ width: 0.55in; height: 0.55in }}
.back .logo {{ position: absolute; left: 0.75in; top: 0.85in }}
.back .title {{ position: absolute; left: 1.45in; top: 0.9in; font-size: 20pt; font-weight: bold }}
.back .title span {{ font-weight: normal; color: {C["acc"]} }}
.back .blurb {{ position: absolute; left: 0.75in; right: 0.65in; top: 1.75in; font-size: 11pt; line-height: 1.5; color: {C["blurb"]} }}
.back .blurb b {{ color: #fff }}
.back .shots {{ position: absolute; left: 0.75in; right: 0.65in; top: 6.3in; display: flex; gap: 0.18in }}
.back .shots img {{ width: 50%; border-radius: 6px; box-shadow: 0 0 0 1px {C["line"]} }}
.back .rights {{ position: absolute; left: 0.75in; bottom: 0.72in; width: 3.4in; font-size: 7.5pt; line-height: 1.4; color: {C["rights"]} }}
/* KDP prints the ISBN barcode in a 2 x 1.2 in white box in the lower right corner of the back
   cover (at least 0.76 in from the bottom, 0.25 in from the hinge): the area stays empty. */
</style></head><body><div class="grid"></div>
<div class="panel back">
  <div class="logo">{logo}</div>
  <div class="title">Kern <span>– {t["book"]}</span></div>
  <div class="blurb">{t["blurb"]}</div>
  <div class="shots"><img src="{data('studio-selected.png')}"><img src="{data('studio-physics-lab.png')}"></div>
  <div class="rights">© 2026 Christian Heinrich Hohlfeld · ORCID 0009-0003-6634-9045<br>
  {"Alle Rechte am Buch vorbehalten. Die Software Kern ist Open Source (Apache 2.0). Kein Produkt von Pollen Robotics oder Hugging Face und nicht von ihnen geprüft." if de else "Book: all rights reserved. The Kern software is open source (Apache 2.0). Not a product of Pollen Robotics or Hugging Face, nor reviewed by them."}</div>
</div>
<div class="panel spine"><div class="inner"><b>Kern</b><span>{t["book"]}</span><span style="color:{C["light"]}">Christian Heinrich Hohlfeld</span></div>
  <div class="mark">{logo}</div></div>
<div class="panel front">
  <div class="logo">{logo}</div>
  <h1>Kern</h1>
  <div class="book">{t["book"]}</div>
  <div class="sub">{t["sub"]}</div>
  <div class="shot"><img src="{data(a.image)}"></div>
  <div class="author">Christian Heinrich Hohlfeld</div>
  <div class="tag">{t["tag"]}</div>
</div></body></html>"""

work = out.with_suffix(".html")
work.write_text(html)
from playwright.sync_api import sync_playwright
with sync_playwright() as p:
    b = p.chromium.launch()
    page = b.new_page()
    page.goto(work.as_uri())
    page.pdf(path=str(out), width=f"{TW}in", height=f"{TH}in", print_background=True, prefer_css_page_size=True)
    b.close()
work.unlink()
# Chromium rounds the page to whole CSS pixels; KDP checks the size to a few hundredths of a point,
# so the page (all boxes) is set to the exact calculator size.
from pypdf import PdfReader, PdfWriter
from pypdf.generic import RectangleObject
reader, writer = PdfReader(out), PdfWriter()
for pg in reader.pages:
    pg.scale_to(round(TW * 72, 2), round(TH * 72, 2))
    for box in ("cropbox", "trimbox", "bleedbox", "artbox"): setattr(pg, box, RectangleObject(pg.mediabox))
    writer.add_page(pg)
writer.write(out)
print(f"{out}: {TW:.3f} x {TH:.3f} in ({TW * 25.4:.1f} x {TH * 25.4:.1f} mm), spine {S:.3f} in ({S * 25.4:.1f} mm) for {a.pages} pages")
