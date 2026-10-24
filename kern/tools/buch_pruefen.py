#!/usr/bin/env python3
"""Checks a book interior against Amazon KDP's print rules before it is uploaded.

    python3 kern/tools/buch_pruefen.py kern-buch.pdf

Reports pages where text or a picture reaches into KDP's minimum margins (outside, top and bottom
0.25 in; the gutter by page count: 0.375 in up to 150 pages, 0.5 in up to 300, 0.625 in up to 500,
0.75 in up to 700, 0.875 in above), pictures printed below 300 dpi and pages that are almost empty
(less than an eighth filled, not counting chapter ends before a blank left page). Exit status 1 if
anything is found. Needs pdfplumber.
"""
import sys

import pdfplumber

path = sys.argv[1]
PT = 72.0
problems = []
with pdfplumber.open(path) as pdf:
    n = len(pdf.pages)
    gutter = 0.375 if n <= 150 else 0.5 if n <= 300 else 0.625 if n <= 500 else 0.75 if n <= 700 else 0.875
    outside = 0.25
    sparse = []
    for i, page in enumerate(pdf.pages, start=1):
        W, H = page.width, page.height
        right_page = i % 2 == 1  # odd pages are right-hand pages: the gutter is on the left
        left_min = (gutter if right_page else outside) * PT
        right_max = W - (outside if right_page else gutter) * PT
        top_min, bottom_max = outside * PT, H - outside * PT
        words = page.extract_words(keep_blank_chars=False)
        bad = [w for w in words if w["x0"] < left_min - 0.5 or w["x1"] > right_max + 0.5 or w["top"] < top_min - 0.5 or w["bottom"] > bottom_max + 0.5]
        if bad:
            worst = max(max(left_min - w["x0"], w["x1"] - right_max, top_min - w["top"], w["bottom"] - bottom_max) for w in bad)
            problems.append(f"page {i}: {len(bad)} words in the margin (up to {worst:.1f} pt), e.g. {bad[0]['text'][:40]!r}")
        for im in page.images:
            if im["x0"] < left_min - 0.5 or im["x1"] > right_max + 0.5 or im["top"] < top_min - 0.5 or im["bottom"] > bottom_max + 0.5:
                problems.append(f"page {i}: a picture reaches into the margin")
            sw, sh = im.get("srcsize", (0, 0))
            wi = (im["x1"] - im["x0"]) / PT
            if sw and wi > 0.3 and sw / wi < 299:
                problems.append(f"page {i}: a picture at {sw / wi:.0f} dpi")
        filled = sum((w["x1"] - w["x0"]) * (w["bottom"] - w["top"]) for w in words) + sum((im["x1"] - im["x0"]) * (im["bottom"] - im["top"]) for im in page.images)
        if 0 < filled < 0.02 * W * H and len(words) > 3:
            sparse.append(i)
    for i in sparse:
        nxt = pdf.pages[i].extract_words() if i < n else []
        if nxt or i % 2 == 0:  # a short chapter end before a blank page is normal
            problems.append(f"page {i}: almost empty")
print(f"{path}: {n} pages, gutter {gutter} in, outside {outside} in")
for p in problems: print("  " + p)
print("OK" if not problems else f"{len(problems)} problems")
sys.exit(1 if problems else 0)
