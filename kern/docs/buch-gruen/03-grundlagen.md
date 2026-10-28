# 3 Grundlagen: das Fundament unter allem

> **Kurz gesagt:** Jedes Stockwerk von Kern steht auf vier Bausteinen: einem Werkzeugkasten mit
> Zahlentypen, Fehlermeldungen und Helfern, einem eigenen JSON-Leser, Mathematik für den 3D-Raum und
> Winkelfunktionen, die auf jedem Rechner exakt dieselben Bits liefern – samt dem Grund für jede
> Entscheidung.

Ist der Keller schief, ist es das ganze Haus. Die vier Bausteine heißen `core/base.h`,
`core/json.h`, `math/math.h` und `core/dmath.h`.

## 3.1 Der Werkzeugkasten: `base.h`

### Zahlen mit klarer Größe

Ein `int` kann je nach Rechner unterschiedlich groß sein. Kern schreibt die Breite darum immer dazu:
`u8` bis `u64` (ohne Vorzeichen), `i8` bis `i64` (mit Vorzeichen), `f32` und `f64` (Kommazahlen); die
Zahl nennt die Bits. An der Grenze zu WebAssembly zählt jedes Bit: Dort ist ein Zeiger 32 Bit breit,
`f64` ist genau JavaScripts `Number`, und die Daten der Schnittstelle sind 32-Bit-Wörter
([Kapitel 17](17-referenz.md)).

### Wenn etwas schiefgeht: `fail()` und `last_error()`

Kern verzichtet auf „Ausnahmen“, den Notfallknopf vieler Sprachen. Eine Funktion, die scheitern
kann, gibt `false` zurück (oder 0 bzw. einen leeren Zeiger) und hinterlegt vorher mit `fail()` eine
lesbare Meldung. Der Trick: `fail()` gibt **immer** `false` zurück, so wird jede Prüfung eine Zeile:

```cpp
if (!camera_valid(camera))
  return fail("invalid camera");
bool ok = false;
const Mat4 view = Mat4::look_at(camera.eye, camera.target,
                                camera.up, &ok);
if (!ok) return fail("degenerate camera direction/up");
```

Ungültige Kamera: Meldung hinterlegen, `false` zurück. Sonst wird die Blickrichtung berechnet, und
`ok` sagt, ob das klappte (nicht etwa, wenn Auge und Ziel am selben Punkt stehen).

> **Stell dir vor …** Ein Hotel hat genau **eine** Pinnwand. Wer etwas meldet, hängt einen Zettel
> hin und nimmt dabei den alten ab. Wer nachsieht, liest immer nur den neuesten.

So verhält sich der Fehlerkanal: ein Speicher für das ganze Programm, 1024 Zeichen, von allen
Threads geteilt; jede Meldung überschreibt die vorige. JavaScript liest ihn mit `kern_last_error()`
und leert ihn mit `kern_clear_error()`. Der JSON-Parser und einige andere Module schreiben ihre
Meldung dagegen auf einen eigenen Zettel.

> **Achtung, Falle!** `last_error()` zeigt die Meldung des **letzten** `fail()` – nicht unbedingt
> die des Aufrufs, der gerade scheiterte. Wer sicher gehen will, ruft vorher `clear_error()`.

Kern prüft gern `!(x > 0)` statt `x <= 0`. Das fängt **NaN** („not a number“, etwa 0 durch 0):
Jeder Vergleich mit NaN ist falsch, also ist `!(NaN > 0)` wahr.

### Logging, Uhr und kleine Helfer

`log()` schreibt in vier Stufen (`debug`, `info`, `warn`, `error`): im Browser mit dem Vorsatz
`[kern]` in die Konsole – Fehler als `console.error`, Warnungen als `console.warn`, sonst
`console.log` – oder an einen eigenen Empfänger des Hosts; nativ Warnungen und Fehler nach
`stderr`, den Rest nach `stdout`. Kann etwa der Glüheffekt Bloom nicht starten, loggt Kern
`bloom disabled:` plus die Meldung von der Pinnwand – und das Spiel läuft ohne Glühen weiter.

`now_ms()` liefert Millisekunden einer Uhr, die nur vorwärts läuft (im Browser
`performance.now()`) – für Messungen und Zeitbudgets, etwa wenn die Sound-Bank nur begrenzt lange Klänge vorrechnen darf.

> **Achtung, Falle!** In der Simulation hat die Uhr nichts verloren! Physik und Roboter zählen
> **feste Schritte**. Schaute die Ente auf die Uhr, liefe sie auf einem langsamen Rechner anders als
> auf einem schnellen ([Kapitel 6](06-physik.md), [Kapitel 13](13-micro-duck.md)).

`min_of`, `max_of` und `clamp` liefern das Kleinere, das Größere und „zwischen zwei Grenzen
halten“; bei gemischten Typen nennt man den Typ, z. B. `min_of<u32>(…)`.

### `Span`: ein Fenster auf Daten

Ein `Span` besteht nur aus `data` (wo fangen die Werte an?) und `size` (wie viele?). Er **besitzt**
die Daten nicht, er zeigt nur darauf – wie `std::span`, nur leichter, und `data` und `size` sind
einfache Felder statt Methoden. Jeder Container mit `data()` und `size()`, etwa ein
`std::vector`, wird automatisch zum Span. So fließen Daten in die Engine, aus C++ wie aus
JavaScript: Auch `MeshInput`, das Paket für ein Mesh, besteht aus Spans, und die C-Schnittstelle
baut sie direkt aus Zeigern in den Speicher.

> **Stell dir vor …** Ein Span ist ein Zettel: „Regal 4, die nächsten 20 Bücher.“ Leicht und
> praktisch – aber räumt jemand das Regal aus, zeigt er ins Leere.

Darum darf ein Span nicht länger leben als seine Daten. Wer sie behalten will, **kopiert** sie – so
wie der Renderer, der Kopien für das Picking und den Neuaufbau nach einem Grafikverlust hält
([Kapitel 4](04-grafik.md)), oder die Geometrie-Aufträge, weil JavaScript seinen Zwischenspeicher
gleich wiederverwendet.

### Hashes

Ein **Hash** macht aus beliebig vielen Bytes eine kurze, feste Zahl. Mit `hash_bytes` (**FNV-1a**,
64 Bit, Startwert wählbar) erkennt der Renderer unveränderte Werte und den Schlüssel seiner
Schattenkarte – und auch der Fingerabdruck
der Ente aus [Kapitel 2](02-bauen-und-testen.md) ist ein FNV-1a. `hash_u32` (*lowbias32*) mischt
Zahlen, etwa für Rauschen, das trotzdem immer gleich ausfällt. Beide rechnen nur mit Ganzzahlen: Das
Ergebnis hängt nur von den Bytes ab, nie vom Rechner.

## 3.2 JSON: Kerns Lieblingsformat

**JSON** ist ein Textformat für strukturierte Daten, etwa `{"name": "Ente", "beine": 2}`. Kern
braucht es für Levels (`.klevel`), Studio-Befehle, Servoprofile, Trainingsdaten und die Netzliste
der Micro Duck. Sein JSON-Baustein verspricht drei Dinge:

1. **Die Reihenfolge bleibt.** Hat Kern oder JavaScript die Datei geschrieben, ergibt Einlesen und
   wieder Schreiben Byte für Byte dasselbe. Ändert sich ein Wert im Level, ändert sich nur eine Zeile der Datei.
2. **Zahlen sehen aus wie in JavaScript** – die kürzeste Ziffernfolge, die die Zahl eindeutig
   beschreibt, genau wie bei `JSON.stringify`.
3. **Misstrauen gegenüber fremden Dateien.** Abgelehnt werden unendliche Zahlen, zu tiefe
   Verschachtelung (mehr als 80 Ebenen; Studio-Befehle dürfen 120) und die Schlüssel `__proto__`,
   `prototype` und `constructor`, die auf der JavaScript-Seite Unheil anrichten könnten
   (*Prototype Pollution*).

> **Wusstest du?** Kern schreibt `0.30000000000000004` – genau wie JavaScript bei 0.1 + 0.2. Ein Test
> prüft solche Fälle, etwa 10²¹ → `1e+21` und NaN → `null`.

Ein `json::Value` ist null, Wahrheitswert, Zahl, Text, Liste oder Objekt. Er kann seinen Typ nennen,
Werte liefern (`truthy()` sogar mit JavaScripts Wahrheitsregeln), Listen und Objekte bearbeiten und
sich mit `==` vergleichen; die ganze Funktionsliste steht im blauen Buch. Sicher wird Code damit so:

- Ein fehlender Schlüssel liefert **null** statt eines Absturzes; `v["a"][2]["b"]` geht immer.
- `as_number` und `as_bool` liefern bei falschem Typ einen Ersatzwert. Wer „fehlt“ von „falscher
  Typ“ unterscheiden will, fragt mit `find` und `is_number`.
- `set` ersetzt einen Schlüssel **an seinem Platz**; `push` macht aus einer Nicht-Liste erst eine
  leere Liste.
- `parse` schreibt das Ergebnis **nur bei Erfolg**. Fehlermeldungen nennen die Stelle
  (`at offset 17`); Text hinter dem Wert gibt `trailing characters`; ein UTF-8-BOM am Anfang ist
  erlaubt. Tiefe und Schlüsselsperre lassen sich in den `ParseOptions` einstellen.
- `stringify(v, indent)` arbeitet wie `JSON.stringify(value, null, indent)`: kompakt bei
  `indent <= 0`, sonst eingerückt; nicht endliche Zahlen (unendlich, NaN) werden `null`, −0 wird `0`.

### Ein Beispiel: Servoprofile

Ein **Servoprofil** beschreibt einen Motor der Micro Duck ([Kapitel 16](16-kalibrierung.md)). Kern
liest es nach einem festen Muster: Text parsen; ist er kein Objekt oder ist `kind` nicht `servo`,
abbrechen. Jedes bekannte Feld einzeln suchen – fehlen darf es. Ist es da, muss es eine Zahl im
erlaubten Bereich sein, sonst Abbruch mit `bad value for` und Feldname. **Erst ganz am Ende** wird
das fertige Profil übernommen; ein halb kaputtes landet nie im Programm. Geschrieben wird es mit
zwei Leerzeichen Einrückung.

> **Probier's aus!** Speichere im Studio ein Level mit **File → Save** (Ctrl+S) und öffne die
> `.klevel`-Datei in einem Texteditor. Das ist Kerns JSON: Schlüssel in fester Reihenfolge, Zahlen so,
> wie JavaScript sie schreiben würde.

## 3.3 Mathematik für den 3D-Raum

`math.h` steht ganz im Header und legt nie Speicher an, damit der Compiler sie besonders schnell
machen kann. Zuerst die Spielregeln:

| Regel | Festlegung |
|---|---|
| Einheiten | Meter, Sekunden, Kilogramm |
| Koordinaten | rechtshändig, **+Z ist oben** |
| Winkel | im Code Radiant, in Levels Grad |
| Matrizen | 4×4, spaltenweise gespeichert |

> **Stell dir vor …** Leg Karopapier auf den Tisch: X zeigt nach rechts, Y nach vorn, Z senkrecht zur
> Zimmerdecke. Rechtshändig heißt: Daumen, Zeige- und Mittelfinger der rechten Hand rechtwinklig
> abspreizen – das sind X, Y und Z.

Spaltenweise heißt: Die 16 Zahlen liegen Spalte für Spalte im Speicher, die Verschiebung in den
Plätzen 12, 13 und 14 – wie bei OpenGL. Punkte werden von rechts an die Matrix multipliziert
(`p' = M * p`). In der Kamerasicht blickt man entlang −Z, und die Projektion landet im
OpenGL-Bildraum mit Tiefe von −1 bis 1.

Z oben gilt überall: Renderer, Physik, Ton, Levels, Grundformen. In **Radiant** ist ein halber
Kreis π (etwa 3,14) statt 180°; Levels und das Details-Panel im Studio zeigen Grad
([Kapitel 8](08-spiel-laufzeit.md)).

> **Probier's aus!** Öffne **File → New Level → Physics Lab**, tippe unten in die Konsole (**Cmd**)
> `gravity 0 0 -2` und starte **Simulate**. Die Schwerkraft zeigt schwach Richtung minus Z – alles
> fällt fast wie auf dem Mond. `gravity` ohne Werte stellt 0 0 −9,80665 wieder her.

### Vektoren und Quaternionen

`Vec2`, `Vec3` und `Vec4` sind Pfeile (oder Punkte) aus zwei, drei oder vier Zahlen, mit Länge,
Abstand, Mischen (`lerp`), Minimum/Maximum, Skalar- und Kreuzprodukt, dazu `finite` (alles
endlich?) und `hex_color(0xRRGGBB)` für Farben. Schlau ist `normalize`, das einen Pfeil auf Länge 1
bringt: Hat er Länge null – etwa nach zwei Klicks auf denselben Punkt –, liefert es einen
**Ersatzwert** (standardmäßig nach oben) statt NaN, das sich wie ein Virus durch die Szene fräße.

Drehungen speichert Kern als **Quaternion**: vier Zahlen `(x, y, z, w)`, Standard `w = 1` heißt
„keine Drehung“. Klingt furchteinflößend, ist im Gebrauch einfach: `axis_angle(achse, winkel)`
erzeugt eine Drehung (die Achse wird dabei auf Länge 1 gebracht), `a * b` dreht erst um `b`, dann
um `a`, `rotate` dreht einen Vektor.

### Matrizen

Eine **Matrix** ist eine 4×4-Tabelle, die einen Punkt auf einen Schlag verschiebt, dreht und
vergrößert. Die wichtigsten Werkzeuge:

- `trs` baut sie aus diesen drei Teilen.
- `perspective` und `ortho` bauen Ansichten mit und ohne Fluchtpunkt, `look_at` die Blickmatrix der
  Kamera. Ist die Blickrichtung unmöglich, liefert `look_at` die Einheitsmatrix (die nichts
  verändert) und `ok = false`.
- `transform_point` bewegt einen Punkt, `transform_vector` nur eine Richtung (ohne Verschieben),
  `transform` rechnet mit allen vier Zahlen, etwa auf dem Weg zum Bildschirm.
- `invert` kehrt eine Matrix um, in doppelter Genauigkeit, damit Klicks auch in großer Entfernung
  treffen. Ist sie (fast) nicht umkehrbar, meldet es `false`.
- `normal_matrix3` sorgt dafür, dass Normalen – Pfeile, die senkrecht aus einer Fläche zeigen – auch
  nach ungleichmäßigem Strecken senkrecht bleiben.

Bei `A * B` wirkt **erst B**, dann A. Ein Test prüft `trs` am Punkt (1, 0, 0): mit 2 skalieren
→ (2, 0, 0), um 90° um Z drehen → (0, 2, 0), um (1, 2, 3) verschieben → **(1, 4, 3)**. Genau so
bekommt in `hello` jede Kiste ihre Matrix, 16 Zahlen je Kiste.

> **Achtung, Falle!** Erst gehen, dann drehen ist etwas anderes als erst drehen, dann gehen – du
> landest woanders. Darum legt `trs` die Reihenfolge fest.

### Kugeln, Kisten, Kamera

`Sphere` (Kugel) und `Aabb` (achsenparallele Kiste) helfen beim Aussortieren des Unsichtbaren. Eine
ungültige Kugel (Radius −1) wird trotzdem gezeichnet: **im Zweifel zeichnen**. Eine leere Kiste
startet „verkehrt herum“, sodass der erste Punkt sie richtig setzt.

Die **Kamera** ist keine Matrix, sondern eine Beschreibung: Auge, Ziel, oben. Standard: Auge
(0, −18, 8), Ziel (0, 0, 2), oben +Z, Öffnungswinkel π/3 = **60°**, gemessen **senkrecht**,
gezeichnet von 0,05 m bis 1800 m. Studios Drauf-, Vorder- und Seitenansicht
nutzen eine orthografische Kamera mit sichtbarer Höhe (Standard 20 m). Ungültiges (etwa `far`
nicht größer als `near`) weist `camera_valid` zurück. `projection_matrix` wählt Perspektive oder
Parallelansicht. Projektion mal Blickmatrix ergibt die View-Projection-Matrix, den ganzen Weg von
der Welt auf den Bildschirm. Aus ihr gewinnt `Frustum::from_matrix` die sechs Wände des
Sichtbereichs, mit denen Kern Unsichtbares aussortiert. JavaScript setzt die Kamera mit zwölf Zahlen
über `kern_camera_set`, das Ungültiges mit `invalid camera` ablehnt.

## 3.4 Mathematik, die überall gleich rechnet

Wurzel, Betrag und Abrunden liefern auf jedem Rechner dieselben Bits. Sinus, Kosinus,
Exponentialfunktion und Co. **nicht**: Linux, macOS, Android und WebAssembly runden im letzten Bit
verschieden. In einer Robotersimulation wird aus einem Bit binnen Sekunden ein anderer Gang – und ein
im Browser gelerntes Netz liefe nativ anders.

> **Stell dir vor …** Zwei Kinder würfeln eine Wanderroute aus, an jeder Kreuzung ein Wurf. Weicht nur
> der erste ab, landen sie abends in verschiedenen Dörfern.

Die Lösung: Kern bringt die Funktionen der C-Bibliothek **musl** mit und übersetzt sie in allen
drei Bauwegen gleich, ohne verschmolzene Rechenschritte. Neue Namen (aus `sin` wird `kern_musl_sin`)
verhindern Verwechslungen mit dem System. Box3D rechnet im Skalar-Pfad und hat ohnehin eigene
deterministische Winkelfunktionen.

Benutzt wird das im Namensraum `kern::dm` genau wie `std::`: `sin`, `cos`, `tan`, `asin`, `acos`,
`atan`, `exp`, `expm1`, `log`, `log10`, `atan2`, `pow` und `hypot`. Mit `float` rechnet die
`float`-Fassung, sonst die `double`-Fassung. `sqrt`, `fabs` und `floor` bleiben bei `std::`, weil sie
ohnehin exakt sind. Aus Kerns Mathematik:

```cpp
static Quat axis_angle(Vec3 axis, f32 radians) {
  const Vec3 a = normalize(axis);
  const f32 s = kern::dm::sin(radians * 0.5f);
  return {a.x * s, a.y * s, a.z * s,
          kern::dm::cos(radians * 0.5f)};
}
```

Achse auf Länge 1 bringen, Sinus des halben Winkels – mit `kern::dm::sin`! – an die Achse
multiplizieren, Kosinus als vierte Zahl. So ergibt jede Drehung überall dieselben Bits. Auch
`perspective` nutzt `kern::dm::tan`.

Die Regel für neuen Code:

- Alles, was die **Simulation speist**, nutzt `kern::dm::` statt `std::`.
- Keine Summe darf von der Zahl der Threads abhängen.
- Keine `-ffast-math`-Optionen.

Reine **Darstellung** – Ton, Partikel, Teile des Editors, die Kamera von `hello` – darf die
System-Mathematik nutzen. Geht solcher Code in Physik, Controller oder Training ein, muss er auf
`kern::dm::` umziehen.

> **Wusstest du?** Die eigene Mathematik kostet keine Geschwindigkeit: musl ist so schnell wie das
> System, und das Tempo der Physik bestimmt ohnehin Box3Ds Gelenk-Löser. Ob alles
> stimmt, verrät der Fingerabdruck aus [Kapitel 2](02-bauen-und-testen.md).

## Das nimmst du mit

- Fehler meldet Kern mit `false` und `fail()`; die Meldung hängt an einer einzigen Pinnwand.
- `!(x > 0)` statt `x <= 0` fängt auch NaN.
- Die Uhr dient Messungen, nie der Simulation.
- Ein `Span` ist ein Fenster auf Daten; wer sie behält, kopiert.
- Kerns JSON hält die Reihenfolge, schreibt Zahlen wie JavaScript und misstraut fremden Dateien.
- Meter, rechtshändig, **Z oben**, Matrizen spaltenweise, Öffnungswinkel senkrecht in Radiant.
- Für die Simulation: `kern::dm::`, `-ffp-contract=off`, feste Summen.

## Knobelecke

1. Warum gibt `fail()` immer `false` zurück?
2. (1, 0, 0) wird um 90° um Z gedreht und dann um (5, 0, 0) verschoben. Wo landet der Punkt?
3. Ein Partikeleffekt nutzt `std::sin`. Problem? Und wenn die Partikel die Ente anschubsen sollen?

### Lösungen

1. Damit `return fail("…");` in einer Zeile Meldung hinterlegt und Misserfolg meldet.
2. Drehen ergibt (0, 1, 0), Verschieben (5, 1, 0).
3. Für reine Darstellung erlaubt. Beeinflussen die Partikel die Physik, muss `kern::dm::sin` her.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
