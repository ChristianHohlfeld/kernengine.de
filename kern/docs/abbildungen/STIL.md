# Abbildungen – Stil und Pflege

Alle Zeichnungen der drei Bücher (blau, grün, gelb; deutsch und englisch) sind Vektorabbildungen in
**einem** Stil. Zeigen zwei Bände dieselbe Idee, ist es **dieselbe** Abbildung; nur die Beschriftung
darf im grünen oder gelben Band einfacher sein, wo es nötig ist. ASCII-Zeichnungen (Kästchen aus
`─│┌┐`, `+--+`, Pfeile und Achsen aus Zeichen) gibt es in den Büchern nicht.

## Dateien

| Datei | Inhalt |
|---|---|
| `<id>.svg` | eine Abbildung: Formen und Platzhalter `{{key}}` für jedes Wort, keine Stilangaben |
| `texte.json` | die Wörter: `{id: {de: {...}, en: {...}, gruen: {de, en}, gelb: {de, en}}}` |
| `stil.css` | der Stil (Schrift, Linien, Farben, Pfeile); wird beim Erzeugen in jedes SVG gesetzt |
| `kern/tools/buch_abbildungen.py` | erzeugt je Abbildung, Band und Sprache eine Vektor-PDF und prüft sie |

`de`/`en` sind die Wörter des blauen Bands. `gruen`/`gelb` ersetzen nur einzelne Schlüssel. Ein
Element mit `data-nur="blau gelb"` erscheint nur in diesen Bänden (z. B. eine Detailzeile, die im
grünen Band fehlt). Zahlen mit Dezimalkomma sind ebenfalls Platzhalter (`0,8` / `0.8`).

Jede SVG beginnt mit einem Kommentar: was sie zeigt und **woher jede Zahl und Form stammt**
(Abschnitt im blauen Buch, Datei und Funktion im Code, Formel und Abtastung bei Kurven). Kurven
sind gerechnet, nicht frei gezeichnet; was nur schematisch ist, sagt der Kommentar.

## Maße

- **1 SVG-Einheit = 1 pt** gedruckt (1/72 Zoll). Die Abbildung wird in **Originalgröße (100 %)**
  gesetzt, nie skaliert. Dann gelten die Schriftgrößen unten genau.
- **Breite höchstens 380 pt** (134 mm): Satzspiegel 7 × 10 Zoll in `buch_pdf.py` ist
  177,8 − 22 − 17 = 138,8 mm, abzüglich 3,5 mm Absatzeinzug = 135,3 mm = 383,6 pt.
- Höhe so knapp wie möglich; höchstens 185 mm (wie bei Mermaid in `buch_pdf.py`).
- Rand: Text und Linien bleiben innerhalb der viewBox (das Werkzeug prüft Texte).

## Schrift

- **DejaVu Sans** (wie die serifenlose Schrift des Satzes und die Mermaid-Grafiken), in die PDF
  eingebettet. Code-Namen (`kern::game`, Dateien) in **DejaVu Sans Mono** (Klasse `c`, 7,8 pt).
- Normal **8,5 pt**; klein (Klasse `k`) **7,5 pt** – nie unter 7 pt. Titel eines Teilbilds fett
  (Klasse `t`). Kursiv nur für Formelzeichen (`n`).
- Textklassen: `m` zentriert, `e` rechtsbündig, `a` Akzentfarbe, `h` leises Grau, `w` weiß (auf
  dunkler Fläche).

## Farben (auch in Graustufen lesbar)

| Rolle | Farbe | Graustufe ca. |
|---|---|---|
| Linien und Text | `#262626` Dunkelgrau | 15 % |
| Akzent (das Wichtige: Kurve, Strahl, Treffer) | `#1d5c99` Blau | 40 % |
| Akzentfläche | `#d9e5f2` Hellblau | 89 % |
| Fläche (Kasten) | `#ececec` Hellgrau | 93 % |
| feste Materie, Schatten | `#bdbdbd` Mittelgrau | 74 % |
| Hilfslinien | `#8c8c8c` | 55 % |

Weißer Hintergrund. Keine Schatten, keine Verläufe, keine Transparenz. Bedeutung nie nur über
die Farbe: Akzent und Normal unterscheiden sich zusätzlich durch Linienart oder Beschriftung
(z. B. Daten-Drähte gestrichelt, Ablauf durchgezogen).

## Linien und Pfeile

- Klassen: `l` 0,75 pt (Standard), `ld` 0,5 pt (dünn, Mindeststärke), `lk` 1,4 pt (Hauptkurve),
  `la`/`lad` Akzent 1,4 / 0,75 pt, `lg` Hilfslinie grau 0,5 pt; `gs` gestrichelt, `gp` gepunktet.
- Pfeile einheitlich: gefüllte Spitze 7 × 5 pt, unabhängig von der Linienstärke. `p` am Ende,
  `ps` zusätzlich am Anfang (Doppelpfeil); `pa`/`pas` in Akzentfarbe.
- Flächen: `f` Kasten, `fa` Akzentkasten, `fd` feste Materie, `fw` weiß mit Rand;
  `ff`/`ffa`/`ffd` dieselben Flächen ohne Rand. Kästen mit 2–3 pt Eckenradius.
- Diagramme mit Achsen: L-förmiger Rahmen links und unten, Teilstriche nach außen, Zahlen 7,5 pt;
  Nulllinien als graue Hilfslinien.

## Erzeugen und prüfen

```
python3 kern/tools/buch_abbildungen.py                 # alle → kern/build/abbildungen/<id>.<band>.<sprache>.pdf
python3 kern/tools/buch_abbildungen.py --png elu bvh   # nur diese, dazu PNG-Vorschau (300 dpi)
python3 kern/tools/buch_abbildungen.py --edition gelb --lang de
```

Das Werkzeug meldet als Fehler: fehlende Texte, Text außerhalb der Abbildung, zwei sich
überlappende Texte, zu breite Abbildungen, nicht eingebettete Schriften. Was es nicht sieht
(Text auf einer Linie, ein Pfeil, der danebenzielt), prüft man an der PNG-Vorschau – nach jeder
Änderung, in allen drei Bänden und beiden Sprachen.

## Neue Abbildung

1. `<id>.svg` mit `viewBox="0 0 B H"` (B ≤ 380), nur Klassen aus `stil.css`, alle Wörter als
   `{{key}}`, oben ein Kommentar mit den Quellen.
2. Texte in `texte.json` (de, en; gruen/gelb nur wo nötig).
3. Erzeugen mit `--png`, jede Variante ansehen.
4. Im Buch einbinden (siehe `buch_pdf.py`).
