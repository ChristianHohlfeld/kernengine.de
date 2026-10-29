#!/usr/bin/env python3
"""Sets the book (kern/docs/buch, or kern/docs/book) as one print-ready PDF.

    python3 kern/tools/buch_pdf.py [--en] [--out kern-buch.pdf] [--format 170x240]
    python3 kern/tools/buch_pdf.py --src buch-gruen --edition "Einfach erklärt" --out kern-gruen.pdf

The same book exists in editions for different readers (kern/docs/buch: the technical one,
kern/docs/buch-gruen: the same content explained more simply); --src picks the folder, --edition
and --subtitle the title page.

Needs pandoc, xelatex (with German hyphenation for the German book) and mmdc (Mermaid CLI).
The chapters are read in order (00 imprint, 01-17, appendices A-C), Mermaid diagrams are
rendered to vector PDFs, links between chapters become links inside the book, and the result is
set with KOMA-Script (scrbook): chapters open on the right, running heads, table of contents.
"""
import argparse, pathlib, re, shutil, subprocess, sys, tempfile

here = pathlib.Path(__file__).resolve().parent
docs = here.parent / "docs"

ap = argparse.ArgumentParser()
ap.add_argument("--en", action="store_true", help="the English book (kern/docs/book)")
ap.add_argument("--out", default=None)
ap.add_argument("--format", default="177.8x254", help="trim size in mm, width x height (default: Amazon KDP hardcover 7 x 10 in)")
ap.add_argument("--keep", action="store_true", help="keep the intermediate files (prints where)")
ap.add_argument("--src", default=None, help="the book's folder in kern/docs (default: buch, or book with --en)")
ap.add_argument("--edition", default=None, help="the line under 'Kern' on the title page (default: Das Buch / The Book)")
ap.add_argument("--subtitle", default=None, help="the subtitle on the title page")
ap.add_argument("--boxes", action="store_true", help="block quotes as grey boxes with a bar (the green edition's 'Probier's aus!' and friends)")
ap.add_argument("--code-size", default="footnotesize", help="LaTeX size of code blocks (small keeps code at 7 pt or more)")
a = ap.parse_args()

src = docs / (a.src or ("book" if a.en else "buch"))
out = pathlib.Path(a.out or ("kern-book.pdf" if a.en else "kern-buch.pdf")).resolve()
w, h = (float(x) for x in a.format.lower().split("x"))
files = sorted(p for p in src.glob("*.md") if re.match(r"^(\d\d|[A-Z])-", p.name))
files = [p for p in files if p.name[0].isdigit()] + [p for p in files if p.name[0].isalpha()]
if not files: sys.exit(f"no chapters in {src}")

work = pathlib.Path(tempfile.mkdtemp(prefix="kern-buch-"))
(work / "fig").mkdir()
ids = {p.name: "kap-" + p.stem.split("-")[0].lower() for p in files}
mermaid_n = 0

MERMAID_CFG = ('{"theme":"base","themeVariables":{"fontFamily":"DejaVu Sans","fontSize":"14px",'
               '"primaryColor":"#ececec","primaryBorderColor":"#262626","primaryTextColor":"#262626",'
               '"lineColor":"#262626","secondaryColor":"#d9e5f2","secondaryBorderColor":"#1d5c99",'
               '"tertiaryColor":"#ffffff","clusterBkg":"#f7f7f7","clusterBorder":"#8c8c8c",'
               '"edgeLabelBackground":"#ffffff","noteBkgColor":"#d9e5f2","noteBorderColor":"#1d5c99"},'
               '"flowchart":{"curve":"linear","rankSpacing":24,"nodeSpacing":24}}')  # the style of kern/docs/abbildungen (STIL.md)


def mermaid(code):
    """A Mermaid diagram as a vector PDF in the figures' style, set at its natural size or narrower."""
    global mermaid_n
    mermaid_n += 1
    mmd, pdf = work / "fig" / f"m{mermaid_n}.mmd", work / "fig" / f"m{mermaid_n}.pdf"
    mmd.write_text(code)
    cfg = work / "fig" / "mmdc.json"
    cfg.write_text(MERMAID_CFG)
    for attempt in range(3):  # headless Chromium now and then fails to start on a busy machine
        if subprocess.run(["mmdc", "-q", "-i", str(mmd), "-o", str(pdf), "--pdfFit", "-c", str(cfg)]).returncode == 0: break
    else: sys.exit(f"mmdc failed on diagram {mermaid_n}:\n{code}")
    from pypdf import PdfReader
    box = PdfReader(pdf).pages[0].mediabox
    pw, ph = float(box.width), float(box.height)
    text_w, max_h = w - 39.0 - 3.5, 185.0           # mm: the text block's width less the paragraph indent, the tallest a diagram may be
    natural = pw / 72 * 25.4 * 0.85                  # mm, a little smaller than on screen (the book's type is smaller than the web's)
    width = min(text_w, natural, max_h * pw / ph)
    import pdfplumber
    with pdfplumber.open(pdf) as d: sizes = [c["size"] for c in d.pages[0].chars]
    printed = min(sizes) * width / (pw / 72 * 25.4) if sizes else 99
    if printed < 7: print(f"MERMAID TOO SMALL: diagram {mermaid_n} prints at {printed:.1f} pt (< 7 pt): {code.splitlines()[0]} / {code.splitlines()[1] if len(code.splitlines()) > 1 else ''}", file=sys.stderr)
    return f"![]({pdf}){{width={width:.1f}mm}}"


edition_name = "gelb" if "gelb" in src.name or "yellow" in src.name else "gruen" if "gruen" in src.name or "green" in src.name else "blau"
lang_code = "en" if a.en or src.name.startswith("book") else "de"
figures_built = False


def figure(m):
    """A figure of kern/docs/abbildungen (one SVG, texts per edition and language) as a vector PDF
    at its own size (kern/tools/buch_abbildungen.py checks that it fits the text block)."""
    global figures_built
    if not figures_built:
        r = subprocess.run([sys.executable, str(here / "buch_abbildungen.py"), "--edition", edition_name, "--lang", lang_code, "--out", str(work / "abb")])
        if r.returncode: sys.exit("buch_abbildungen.py failed")
        figures_built = True
    f = work / "abb" / f"{m.group(2)}.{edition_name}.{lang_code}.pdf"
    if not f.exists(): sys.exit(f"no figure {m.group(2)} for {edition_name}/{lang_code}")
    # Set centred with its caption underneath (small, sans serif), both kept on one page.
    cap = m.group(1).replace("`", "").replace("*", "")
    cap = re.sub(r"([&%$#_{}])", r"\\\1", cap).replace("~", r"\textasciitilde{}").replace("^", r"\textasciicircum{}")
    cap = r"\par\vspace{3pt}{\small\sffamily " + cap + "}" if cap.strip() else ""
    return ("`\\begin{center}\\begin{minipage}{\\linewidth}\\centering\\includegraphics{" + f.as_posix() + "}"
            + cap + "\\end{minipage}\\end{center}`{=latex}")


TEXT_W, TEXT_H = w - 39.0, h - 54.0  # mm: the text block (inner 22 + outer 17, top 20 + bottom 24 + head)


def print_width(f):
    """A picture's printed width: the text width at most, never below 300 dpi, never taller than
    60 % of the text block (a tall phone screenshot would leave half a page empty), the author's
    photo 45 mm."""
    from PIL import Image
    pw, ph = Image.open(f).size
    if f.name.startswith("autor"): return min(45.0, pw / 300 * 25.4)
    return min(TEXT_W - 3.5, pw / 300 * 25.4, 0.6 * TEXT_H * pw / ph)  # 3.5 mm: the paragraph indent


def print_copy(f):
    """Dark screenshots (the Studio's dark UI) printed in black and white turn into dark grey
    areas: for print they are lightened (gamma 0.55, black lifted to 12 %), the original stays."""
    from PIL import Image, ImageStat
    im = Image.open(f).convert("RGB")
    lum = ImageStat.Stat(im.convert("L")).mean[0] / 255
    if lum >= 0.42 or f.name.startswith("autor"): return f
    lut = [round(255 * (0.12 + 0.88 * (v / 255) ** 0.55)) for v in range(256)]  # #1e1e1e panels -> light grey, white stays white
    out = work / "fig" / ("hell-" + f.name)
    im.point(lut * 3).save(out)
    return out


parts = []
for i, p in enumerate(files):
    t = p.read_text()
    t = re.sub(r"\n---\n\n© 20\d\d Christian Heinrich Hohlfeld[^\n]*\n?$", "\n", t)  # the per-chapter notice (the imprint has it)
    t = re.sub(r"```mermaid\n(.*?)```", lambda m: mermaid(m.group(1)), t, flags=re.S)
    # Code blocks without a language: plain text (set by the verbatim environment, wrapped).
    # Images: from the chapter's folder; missing ones (screenshots still to come) are left out.
    def img(m):
        alt, path = m.group(1), m.group(2)
        f = (src / path).resolve()
        local = f.parent / lang_code / f.name  # screenshots of Studio in the book's language (bilder/de, bilder/en)
        if local.exists(): f = local
        if not f.exists():
            print("missing image", path, file=sys.stderr)
            return ""
        return f"![{alt}]({print_copy(f)}){{width={print_width(f):.1f}mm}}"
    t = re.sub(r"!\[([^\]]*)\]\(((?:\.\./buch/)?bilder/[^)\s]+)\)", img, t)
    t = re.sub(r"!\[([^\]]*)\]\(\.\./abbildungen/([\w-]+)\.svg\)", figure, t)
    # Links to other chapters (with or without an anchor) point into the book.
    def link(m):
        text, target, anchor = m.group(1), m.group(2), m.group(3)
        if target in ids: return f"[{text}](#{anchor[1:] if anchor else ids[target]})"
        return m.group(0)
    t = re.sub(r"\[([^\]]+)\]\(([0-9A-Z][^)#\s]*\.md)(#[^)\s]+)?\)", link, t)
    if p.name.startswith("00"):  # in print: the imprint alone (the title page and the table of contents are the book's)
        t = re.sub(r"\A.*?^## ", lambda m: "```{=latex}\n\\hypertarget{kap-00}{}\\thispagestyle{empty}\\begingroup\\footnotesize\\raggedright\n```\n\n## ", t, count=1, flags=re.S | re.M)
        t = re.sub(r"^## (.+)$", r"## \1 {.unnumbered}", t, flags=re.M)
        t = re.sub(r"^## (Inhalt|Contents)\b.*\Z", "", t, flags=re.S | re.M)
    # The chapter's first heading carries its id; the imprint and the appendices are not numbered.
    first = None if p.name.startswith("00") else re.search(r"^# (.+)$", t, flags=re.M)
    if first:
        attrs = "{#" + ids[p.name] + (" .unnumbered" if p.name.startswith("00") else "") + "}"
        head = first.group(1)
        short = head
        if len(short) > 44:  # the running head has one line
            cut = max(short.rfind(sep, 0, 44) for sep in (",", " – ", " ("))
            short = short[:cut] if cut > 10 else short[:44]
        mark = "" if short == head else "\n\n```{=latex}\n\\chaptermark{" + short.replace("&", "\\&") + "}\n```\n"
        t = t[:first.start()] + f"# {head} {attrs}" + mark + t[first.end():]
    if p.name[0].isalpha() and (i == 0 or files[i - 1].name[0].isdigit()):
        t = "```{=latex}\n\\appendix\n```\n\n" + t
    if p.name.startswith("00"):
        t = t + "\n\n```{=latex}\n\\par\\endgroup\\normalsize\n\\tableofcontents\n\\kernmainmatter\n```\n"
    parts.append(t)

(work / "buch.md").write_text("\n\n".join(parts))
lang = "en" if a.en else "de"
header = work / "header.tex"
# The green edition's boxes ("Kurz gesagt", "Probier's aus!", ...) are block quotes: printed in black
# and white as a light grey box with a dark bar on the left, which may break across pages.
BOXES = r"""
\usepackage[most]{tcolorbox}
\renewenvironment{quote}{\begin{tcolorbox}[breakable,enhanced,colback=black!6,colframe=black!6,
  borderline west={2.2pt}{0pt}{black!70},arc=0pt,outer arc=0pt,boxrule=0pt,left=8pt,right=6pt,top=4pt,bottom=4pt,
  before skip=8pt,after skip=8pt]\setlength{\parindent}{0pt}\setlength{\parskip}{3pt}}{\end{tcolorbox}}
"""
logo_tex = work / "kern_logo.tex"
logo_tex.write_text((here / "kern_logo.tex").read_text().replace("KERNLOGO", (docs / "buch" / "bilder" / "kern-logo.png").as_posix()))
header.write_text(r"""
\usepackage{polyglossia}
\setmainlanguage%s
\setmonofont{DejaVu Sans Mono}[Scale=0.82]
\setsansfont{DejaVu Sans}[Scale=0.9]
\usepackage{fvextra}
\usepackage{xurl}
\usepackage{adjustbox}
\makeatletter\renewcommand*\@pnumwidth{2.4em}\makeatother  %% three-digit page numbers in the contents
\binoppenalty=100
\relpenalty=100
\tolerance=2000
\hbadness=10000
\DefineVerbatimEnvironment{verbatim}{Verbatim}{breaklines,breakanywhere,fontsize=\%s}
\newsavebox\kernfitbox  %% formulas: shrink to the line only when wider (adjustbox would obey pandoc's Gin defaults and shrink them to dust)
\newcommand\kernfit[1]{\sbox\kernfitbox{#1}\ifdim\wd\kernfitbox>\linewidth\resizebox{\linewidth}{!}{\usebox\kernfitbox}\else\usebox\kernfitbox\fi}
\usepackage[headsepline]{scrlayer-scrpage}
\pagestyle{scrheadings}
\automark[section]{chapter}
\setcounter{tocdepth}{1}
\setcounter{secnumdepth}{-1}  %% the headings carry their own numbers
\renewcommand{\arraystretch}{1.15}
\setlength{\emergencystretch}{3em}
\input{%s}
\let\kernmainmatter\mainmatter
\renewcommand{\mainmatter}{}  %%%% pandoc says it before the body; the book says it after the contents
%s
\frontmatter
""" % ("[variant=american]{english}" if a.en else "[spelling=new]{german}", a.code_size, logo_tex.as_posix(), BOXES if a.boxes else ""))
edition = a.edition or ("The Book" if a.en else "Das Buch")
title = "Kern – " + edition
titlepage = work / "titlepage.tex"
sub = a.subtitle or ("The Engine, Kern Studio, a Walking Robot and the Lehrer" if a.en
       else "Die Engine, Kern Studio, ein Laufroboter und der Lehrer")
titlepage.write_text(r"""
\thispagestyle{empty}
\begin{center}
\vspace*{18mm}
\kernlogo{34mm}\par
\vspace{14mm}
{\sffamily\bfseries\fontsize{46}{50}\selectfont Kern\par}
\vspace{4mm}
{\sffamily\Large %s\par}
\vspace{10mm}
{\large %s\par}
\vfill
{\Large Christian Heinrich Hohlfeld\par}
\vspace{2mm}
{\small ORCID 0009-0003-6634-9045\par}
\vspace{12mm}
\end{center}
\newpage
""" % (edition, sub))
cmd = ["pandoc", str(work / "buch.md"), "-s", "-o", str(work / "buch.tex"), "--lua-filter", str(here / "buch_code.lua"), "--columns=50", "--no-highlight",
       "-f", "markdown+pipe_tables+tex_math_dollars+raw_attribute+autolink_bare_uris-yaml_metadata_block-implicit_figures",
       "--top-level-division=chapter", "--toc-depth=1",
       "-V", "documentclass=scrbook", "-V", "classoption=openright,twoside,fontsize=10pt,headings=normal",
       "-V", f"geometry=paperwidth={w}mm,paperheight={h}mm,inner=22mm,outer=17mm,top=20mm,bottom=24mm",
       "-V", "fontfamily=fontspec", "-V", "mainfont=DejaVu Serif", "-V", "mainfontoptions=Scale=0.92", "-V", "mathfont=DejaVu Math TeX Gyre", "-V", "colorlinks=true", "-V", "linkcolor=black", "-V", "urlcolor=black",
       "-V", f"title-meta={title}", "-V", "author-meta=Christian Heinrich Hohlfeld", "-B", str(titlepage),
       "-H", str(header), "--resource-path", str(src)]
r = subprocess.run(cmd, cwd=work)
if r.returncode: sys.exit(f"pandoc failed (intermediate files in {work})")
# Table cells: the first word of a cell may be hyphenated too (\hspace{0pt} gives it glue before it).
tex = (work / "buch.tex").read_text()
tex = tex.replace("\\raggedright\\arraybackslash}", "\\raggedright\\arraybackslash\\hspace{0pt}}")
tex = tex.replace("\\begin{minipage}[b]{\\linewidth}\\raggedright\n", "\\begin{minipage}[b]{\\linewidth}\\raggedright\\hspace{0pt}\n")
(work / "buch.tex").write_text(tex)
# Links inside the book must reach a heading: pandoc drops leading digits from heading ids
# ("## 14.7 Determinismus" -> "determinismus"), so an anchor like #147-determinismus leads nowhere.
targets = set(re.findall(r"\\(?:hypertarget|label)\{([^}]+)\}", tex))
dead = sorted(set(re.findall(r"\\hyperlink\{([^}]+)\}", tex)) - targets)
if dead: print(f"DEAD LINKS: {len(dead)}: " + ", ".join(dead[:30]), file=sys.stderr)
def latex(runs=3):
    for run in range(runs):  # contents, references and running heads settle after three runs
        r = subprocess.run(["xelatex", "-interaction=nonstopmode", "-halt-on-error", "buch.tex"], cwd=work, capture_output=True, text=True)
        if r.returncode: sys.exit(f"xelatex failed (intermediate files in {work}):\n{r.stdout[-3000:]}")


def short_chapter_ends():
    """Pages that end a chapter with only a few lines on them (the next page opens a chapter):
    returns (page, first words, number of lines)."""
    import pdfplumber
    found = []
    with pdfplumber.open(work / "buch.pdf") as pdf:
        n = len(pdf.pages)
        for i in range(1, n - 1):
            page, nxt = pdf.pages[i], pdf.pages[i + 1]
            lines = {}
            for wd in page.extract_words(x_tolerance=1.2):
                if wd["top"] < 45 or wd["bottom"] > page.height - 45: continue  # running head, page number
                lines.setdefault(round(wd["top"] / 3), []).append(wd)
            if not 0 < len(lines) <= 3: continue  # four more lines still keep KDP's bottom margin
            big = [c for c in nxt.chars if c["size"] > 14]  # a chapter heading opens the next page
            if not big and nxt.extract_words(): continue
            first = sorted(lines.items())[0][1]
            words = [w["text"] for w in sorted(first, key=lambda w: w["x0"])]
            found.append((i + 1, words, len(lines)))
    return found


def pull_back(tex, words, n):
    """Puts \\enlargethispage before the paragraph holding these words, so that the page before
    takes the few lines too."""
    tokens = [t for w in words for t in re.findall(r"\w+", w)]
    while tokens and (tokens[0].isdigit() or len(tokens[0]) < 2): tokens.pop(0)  # list numbers
    tokens = [t for t in tokens if not t.endswith("-")][:5]
    if len(tokens) < 2 or sum(len(t) for t in tokens) < 10: return tex, False
    pat = r"[\W_]+(?:\\[a-zA-Z]+\{|\{\})*".join(re.escape(t) for t in tokens)
    m = re.search(pat, tex)
    if not m: return tex, False
    start = tex.rfind("\n\n", 0, m.start()) + 2
    return tex[:start] + f"\\enlargethispage{{{n + 1}\\baselineskip}}\n" + tex[start:], True


latex()
for attempt in range(2):
    shorts = short_chapter_ends()
    if not shorts: break
    tex, changed = (work / "buch.tex").read_text(), False
    for pg, words, n in shorts:
        tex, ok = pull_back(tex, words, n)
        changed |= ok
        print(f"short chapter end on page {pg} ({n} lines): {'pulled back' if ok else 'not found'}", file=sys.stderr)
    if not changed: break
    (work / "buch.tex").write_text(tex)
    latex()
shutil.copy(work / "buch.pdf", out)
# Nothing may reach into the margin: every overfull line is reported with its page (KDP rejects
# text outside the margins).
log = (work / "buch.log").read_text(errors="replace")
over, page = [], 1
for line in log.splitlines():
    for m in re.finditer(r"\[(\d+)", line): page = int(m.group(1)) + 1
    m = re.match(r"Overfull \\hbox \(([\d.]+)pt too wide\)", line)
    if m and float(m.group(1)) > 0.5: over.append((page, float(m.group(1))))
if over:
    print(f"OVERFULL: {len(over)} lines, worst {max(o[1] for o in over):.1f} pt; pages (approx.): " + ", ".join(f"{p}:{v:.0f}pt" for p, v in over[:40]), file=sys.stderr)
pages = subprocess.run(["pdfinfo", str(out)], capture_output=True, text=True).stdout
pages = re.search(r"Pages:\s+(\d+)", pages)
print(f"{out}: {pages.group(1) if pages else '?'} pages, {mermaid_n} diagrams, {w} x {h} mm")
if a.keep: print("intermediate files:", work)
else: shutil.rmtree(work)
