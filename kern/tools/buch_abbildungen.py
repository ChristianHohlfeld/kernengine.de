#!/usr/bin/env python3
"""Prints the books' figures (kern/docs/abbildungen) as vector PDFs, one per figure, edition and
language.

    python3 kern/tools/buch_abbildungen.py [--out kern/build/abbildungen] [--png] [id ...]

Every figure is one SVG (kern/docs/abbildungen/<id>.svg) whose words are placeholders {{key}};
kern/docs/abbildungen/texte.json holds them per language:

    {"<id>": {"de": {"key": "Text"}, "en": {...},
              "gruen": {"de": {...}, "en": {...}}, "gelb": {...}}}

"de" and "en" are the blue book's words; "gruen" and "gelb" replace single keys for the green and
the yellow edition, only where a simpler word is needed. An element with data-nur="blau gelb" is
kept only in those editions (an extra line of detail, say). stil.css (fonts, lines, colours) and the
arrowheads are put into every SVG, so the files themselves carry no styling.

Output: <out>/<id>.<edition>.<lang>.pdf for edition blau, gruen, gelb and lang de, en, the page
exactly the figure's size (1 SVG unit = 1 pt), set at that size in the book (100 %). The SVG that
was printed lies next to it (.svg), with --png also a 300 dpi preview (.png).

Checks, each an error (exit status 1): a placeholder without text, a text running out of the
figure, two texts overlapping, a figure wider than the text block, fonts not embedded (pdffonts).
Needs pypdf, Playwright with Chromium (python3 -m pip install playwright; python3 -m playwright install
chromium), pdffonts and pdftoppm (poppler-utils), and the DejaVu fonts.
"""
import argparse, html, json, pathlib, re, shutil, subprocess, sys

here = pathlib.Path(__file__).resolve().parent
src = here.parent / "docs" / "abbildungen"
EDITIONS, LANGS = ("blau", "gruen", "gelb"), ("de", "en")
TEXT_W = (177.8 - 39.0 - 3.5) / 25.4 * 72  # pt: the text block of buch_pdf.py less the paragraph indent (383.6 pt)

ap = argparse.ArgumentParser()
ap.add_argument("ids", nargs="*", help="only these figures (default: all)")
ap.add_argument("--out", default=str(here.parent / "build" / "abbildungen"))
ap.add_argument("--png", action="store_true", help="also a 300 dpi PNG of each PDF, to look at")
ap.add_argument("--edition", choices=EDITIONS, action="append", help="only this edition (repeatable)")
ap.add_argument("--lang", choices=LANGS, action="append", help="only this language (repeatable)")
a = ap.parse_args()

out = pathlib.Path(a.out).resolve()
out.mkdir(parents=True, exist_ok=True)
texts = json.loads((src / "texte.json").read_text())
css = (src / "stil.css").read_text()
svgs = sorted(src.glob("*.svg"))
if a.ids:
    svgs = [p for p in svgs if p.stem in a.ids]
    missing = set(a.ids) - {p.stem for p in svgs}
    if missing: sys.exit("no such figure: " + ", ".join(sorted(missing)))

DEFS = """<defs>
<marker id="pfeil" viewBox="0 0 7 5" refX="6.6" refY="2.5" markerWidth="7" markerHeight="5" markerUnits="userSpaceOnUse" orient="auto-start-reverse"><path d="M0,0 L7,2.5 L0,5 z" fill="#262626"/></marker>
<marker id="pfeil-a" viewBox="0 0 7 5" refX="6.6" refY="2.5" markerWidth="7" markerHeight="5" markerUnits="userSpaceOnUse" orient="auto-start-reverse"><path d="M0,0 L7,2.5 L0,5 z" fill="#1d5c99"/></marker>
</defs>"""

errors = []


def words(fid, edition, lang):
    t = texts.get(fid)
    if t is None: return None
    w = dict(t.get(lang, {}))
    w.update(t.get(edition, {}).get(lang, {}) if edition != "blau" else {})
    return w


def instance(svg, fid, edition, lang):
    """The SVG for one edition and language: words in, other editions' elements out, style in."""
    w = words(fid, edition, lang) or {}
    def put(m):
        k = m.group(1)
        if k not in w:
            errors.append(f"{fid} [{edition}/{lang}]: no text for {{{{{k}}}}}")
            return "??"
        return html.escape(w[k], quote=False)
    s = svg
    # data-nur="blau gelb": the element stays only in these editions. Elements are kept simple
    # (one line, or a <g> ... </g> without nested <g>), so a regular expression is enough.
    def only(m):
        return m.group(0) if edition in m.group(2).split() else ""
    s = re.sub(r"<g\b[^>]*\bdata-nur=\"([^\"]*)\"[^>]*>.*?</g>", lambda m: m.group(0) if edition in m.group(1).split() else "", s, flags=re.S)
    s = re.sub(r"<(\w+)\b[^>]*\bdata-nur=\"([^\"]*)\"[^>]*?(/>|>.*?</\1>)", only, s, flags=re.S)
    s = re.sub(r"\{\{\s*([\w.-]+)\s*\}\}", put, s)
    s = re.sub(r"(<svg\b[^>]*>)", lambda m: m.group(1) + "\n<style>\n" + css + "</style>\n" + DEFS, s, count=1)
    return s


def exact(pdf, w, h):
    """Sets the PDF's page (media and crop box) to exactly w x h pt at its top left."""
    from pypdf import PdfReader, PdfWriter
    from pypdf.generic import RectangleObject
    r = PdfReader(str(pdf))
    if len(r.pages) != 1: errors.append(f"{pdf.name}: {len(r.pages)} pages")
    pg = r.pages[0]
    top = float(pg.mediabox.top)
    box = RectangleObject([0, top - h, w, top])
    pg.mediabox = box; pg.cropbox = box; pg.trimbox = box; pg.bleedbox = box
    wr = PdfWriter()
    wr.add_page(pg)
    wr.add_metadata({"/Producer": "kern/tools/buch_abbildungen.py"})
    with open(pdf, "wb") as f: wr.write(f)


def size(svg):
    m = re.search(r'<svg\b[^>]*\bviewBox="\s*([-\d.]+)[ ,]+([-\d.]+)[ ,]+([\d.]+)[ ,]+([\d.]+)"', svg)
    if not m: sys.exit("an SVG without viewBox")
    return float(m.group(3)), float(m.group(4))


# In the browser: every <text> must lie inside the figure, and no two texts may overlap.
CHECK = """() => {
  const svg = document.querySelector('svg'), box = svg.getBoundingClientRect(), r = [];
  const texts = [...svg.querySelectorAll('text')].filter(t => t.textContent.trim());
  const rects = texts.map(t => t.getBoundingClientRect());
  texts.forEach((t, i) => {
    const b = rects[i];
    if (b.left < box.left - 0.3 || b.right > box.right + 0.3 || b.top < box.top - 0.3 || b.bottom > box.bottom + 0.3)
      r.push('outside: ' + t.textContent.trim());
    for (let j = 0; j < i; j++) {
      const c = rects[j];
      const dx = Math.min(b.right, c.right) - Math.max(b.left, c.left), dy = Math.min(b.bottom, c.bottom) - Math.max(b.top, c.top);
      if (dx > 0.8 && dy > 1.6) r.push('overlap: "' + texts[j].textContent.trim() + '" / "' + t.textContent.trim() + '"');
    }
  });
  return r;
}"""


def main():
    from playwright.sync_api import sync_playwright
    editions, langs = a.edition or EDITIONS, a.lang or LANGS
    made = 0
    with sync_playwright() as pw:
        browser = pw.chromium.launch()
        page = browser.new_page()
        for p in svgs:
            fid, svg = p.stem, p.read_text()
            if fid not in texts: errors.append(f"{fid}: not in texte.json")
            w, h = size(svg)
            if w > TEXT_W + 0.1: errors.append(f"{fid}: {w:.0f} pt wide, the text block has {TEXT_W:.0f} pt")
            for ed in editions:
                for lang in langs:
                    s = instance(svg, fid, ed, lang)
                    s = re.sub(r'(<svg\b[^>]*?)\s(width|height)="[^"]*"', r"\1", s, count=2)
                    s = s.replace("<svg", f'<svg width="{w}pt" height="{h}pt"', 1)
                    base = out / f"{fid}.{ed}.{lang}"
                    pathlib.Path(f"{base}.svg").write_text(s)
                    page.set_content('<!doctype html><html><head><meta charset="utf-8"><style>@page{margin:0}html,body{margin:0;padding:0;background:#fff}svg{display:block}</style></head><body>' + s + "</body></html>")
                    page.evaluate("document.fonts.ready")
                    for problem in page.evaluate(CHECK): errors.append(f"{fid} [{ed}/{lang}]: {problem}")
                    pdf = pathlib.Path(f"{base}.pdf")
                    # Chromium rounds the page size to its own grid: print a little taller, then
                    # cut the page to the figure's exact size (the figure sits at the top left).
                    page.pdf(path=str(pdf), width=f"{w / 72:.4f}in", height=f"{(h + 4) / 72:.4f}in", print_background=True, page_ranges="1")
                    exact(pdf, w, h)
                    fonts = subprocess.run(["pdffonts", str(pdf)], capture_output=True, text=True).stdout.splitlines()[2:]
                    for f in fonts:
                        cols = f.split()
                        if "DejaVu" not in f: errors.append(f"{fid} [{ed}/{lang}]: unexpected font {cols[0]}")
                        if len(cols) >= 5 and cols[-5] != "yes": errors.append(f"{fid} [{ed}/{lang}]: font not embedded: {cols[0]}")
                    if a.png:
                        subprocess.run(["pdftoppm", "-png", "-r", "300", "-singlefile", str(pdf), str(base)], check=True)
                    made += 1
        browser.close()
    print(f"{made} PDFs in {out}")
    if errors:
        print(f"{len(errors)} problems:", file=sys.stderr)
        for e in errors: print("  " + e, file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    if not shutil.which("pdffonts"): sys.exit("needs pdffonts (poppler-utils)")
    main()
