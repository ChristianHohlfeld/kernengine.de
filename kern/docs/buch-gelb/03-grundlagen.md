# 3 Das Fundament

> **Dein Level:** Jedes Hochhaus braucht ein Fundament. Hier lernst du die vier Bausteine, auf denen
> ganz Kern steht: Typen und Fehler, JSON, 3D-Mathematik und eine Mathe-Bibliothek, die auf jedem
> Rechner exakt gleich rechnet.

Die vier Bausteine sind die Header `base.h`, `json.h`, `math.h` und `dmath.h`.

## 3.1 Die Werkzeugkiste: `base.h`

### Typen mit klarer Breite

Kern schreibt bei Zahlen immer die Breite aus: `u8` bis `u64` (ohne Vorzeichen), `i8` bis `i64`
(mit Vorzeichen), `f32` und `f64` (Kommazahlen). Die Zahl sagt, wie viele Bit der Typ hat. An der
Grenze zu WebAssembly zählt das: Dort ist ein Zeiger 32 Bit breit, `f64` ist genau JavaScripts
`Number`, und die Schnittstelle zum Browser spricht in `u32`-Wörtern.

### Fehler ohne Drama

Kern benutzt keine Ausnahmen. Eine Funktion, die scheitern kann, gibt `false` (oder 0) zurück und
hinterlegt eine Meldung mit `fail()`. Weil `fail()` selbst `false` liefert, passt eine Prüfung in
eine Zeile – so beim Picking:

```cpp
if (!camera_valid(camera))
  return fail("invalid camera");
```

Ist die Kamera kaputt, merkt sich Kern „invalid camera“ und meldet `false`. Lesen kann man die
Meldung mit `last_error()`, aus JavaScript mit `kern_last_error()`.

> **Vorsicht, Falle:** Es gibt nur **einen** Meldungspuffer (1024 Zeichen) für alle Threads. Jeder
> `fail()` überschreibt den vorigen. `last_error()` zeigt also die letzte Meldung, nicht
> unbedingt die zu deinem Aufruf. Sicher geht, wer vorher `clear_error()` ruft.

> **Krass, aber wahr:** Kern prüft oft `!(x > 0)` statt `x <= 0`. Der Grund ist **NaN** („keine
> Zahl“, etwa 0 durch 0). Jeder Vergleich mit NaN ist falsch. `x <= 0` ließe NaN durch,
> `!(x > 0)` fängt es.

### Logging, Zeit und Helfer

`log()` kennt die Stufen `debug`, `info`, `warn` und `error`. Im Browser landet der Text mit
`[kern]` davor in der Konsole, nativ in der Ausgabe. Aus dem Bloom-Effekt:

```cpp
supported_ = false;
log(LogLevel::warn, "bloom disabled: %s", last_error());
return false;
```

Klappt Bloom nicht: abschalten, Warnung samt Grund, `false`.

`now_ms()` liefert Millisekunden für Messungen. **In die Simulation gehört sie nie**: Die zählt
feste Schritte, keine Uhrzeit.
`min_of`, `max_of` und `clamp` (auf einen Bereich begrenzen) wollen zwei Werte vom selben Typ.

### `Span`: das Schaufenster

Ein `Span` zeigt auf Werte, die woanders liegen: Zeiger `data` plus Anzahl `size`. Er **besitzt**
sie nicht. Jeder Behälter mit `data()` und `size()` wird von selbst zum Span. In diesem Format
fließen alle Daten in die Engine, aus C++ wie aus JavaScript.

> **So kannst du es dir vorstellen:** Ein Span ist ein Schaufenster: Du siehst die Ware, aber sie
> gehört dir nicht. Räumt der Laden aus, ist das Fenster leer.

Ein Span darf also nie länger leben als seine Daten. Wer Daten behalten will, kopiert sie – der
Renderer hält darum eigene Kopien ([Kapitel 4](04-grafik.md)).

### Hashes

`hash_bytes` (64-Bit-FNV-1a) macht aus beliebig vielen Bytes eine Kennzahl. Damit erkennt der
Renderer unveränderte Werte, und der Fingerabdruck der Ente aus [Kapitel 2](02-bauen-und-testen.md)
ist genau so ein Hash. `hash_u32` (*lowbias32*) mischt eine Ganzzahl, etwa für Rauschen im Spiel.
Beide rechnen nur mit Ganzzahlen: gleiche Bytes, gleiche Kennzahl.

## 3.2 JSON

Levels, Studio-Befehle, Servoprofile: alles **JSON**. Kerns JSON-Baustein verspricht dreierlei:

1. **Reihenfolge bleibt.** Einlesen und Schreiben ergibt Byte für Byte dieselbe Datei. Änderungen an
   Levels sieht man so sauber.
2. **Zahlen wie in JavaScript**, Ziffer für Ziffer wie `JSON.stringify`.
3. **Sicher gegen böse Dateien.** Er lehnt mehr als 80 Ebenen Verschachtelung ab (Studio-Befehle:
   120), nicht endliche Zahlen und die Schlüssel `__proto__`, `prototype` und `constructor`, mit
   denen man JavaScript-Seiten angreifen kann.

```cpp
CHECK(num(0.1) == "0.1");
CHECK(num(-0.0) == "0");
CHECK(num(1e21) == "1e+21");
CHECK(num(NAN) == "null");
```

Dieser Test legt fest: 0,1 bleibt „0.1“, minus null wird „0“, 10²¹ wird „1e+21“, NaN wird `null`.

> **Krass, aber wahr:** 10²⁰ schreibt Kern als „100000000000000000000“ aus, 10²¹ aber als
> „1e+21“ – genau wie JavaScript.

Ein fehlender Schlüssel liefert `null` statt eines Absturzes, `as_number(ersatz)` bei falschem Typ
den Ersatzwert, und `parse` schreibt seine Meldung
in einen eigenen Text (nicht in `last_error()`) mit der Stelle („at offset 17“). Beim Laden, etwa eines Servoprofils, prüft Kern jedes Feld auf Typ und Bereich
und übernimmt erst ganz am Ende. Ein kaputtes Feld lässt das alte Profil unberührt.

> **Mission:** Speichere ein Level mit **Ctrl+S** und öffne die `.klevel`-Datei in einem
> Texteditor. Es ist JSON! Finde `"format": "kern-level"` und die Rotationen deiner Actors in Grad.

## 3.3 Die 3D-Mathematik

| Regel | Festlegung |
|---|---|
| Einheiten | Meter, Sekunden, Kilogramm |
| Oben | **+Z** |
| Händigkeit | rechtshändig |
| Winkel | Radiant im Code, Grad in Levels |
| Matrizen | 4×4, spaltenweise |

**Z oben** gilt überall: Renderer, Physik, Audio, Levels. `math.h` legt keinen Speicher an, damit
der Compiler sie maximal schnell machen kann.

> **Mission:** Lade **File → New Level → Basic** und setz über den Content Drawer (Ctrl+Space) einen
> **Cube**. Unten links zeigt das Achsenkreuz X rot, Y grün, Z blau – nach oben. Zieh mit dem
> Move-Werkzeug (**W**) am blauen Pfeil und beobachte in **Details** das Feld **Location** Z.

### Vektoren

`Vec2`, `Vec3`, `Vec4` sind Pfeile aus 2, 3 oder 4 Zahlen, mit Skalar- und Kreuzprodukt, Länge und
Abstand (Pythagoras!), Mischen (`lerp`). `normalize` macht einen Pfeil 1 lang. Ein Nullpfeil hat
keine Richtung – statt NaN liefert Kern dann einen Ersatz, standardmäßig (0, 0, 1). So verdirbt
ein Klick auf zwei gleiche Punkte nicht die ganze Szene.

### Drehungen: `Quat`

Kern speichert eine Drehung als **Quaternion** (x, y, z, w): eine geschickte Verpackung von „Achse
plus Winkel“. So packt `Quat::axis_angle`:

```cpp
const Vec3 a = normalize(axis);
const f32 s = kern::dm::sin(radians * 0.5f);
return {a.x * s, a.y * s, a.z * s,
        kern::dm::cos(radians * 0.5f)};
```

Für 90° um Z: halber Winkel 45°, sin 45° ≈ 0,707, cos 45° ≈ 0,707. Achse (0, 0, 1)
mal 0,707 gibt x = 0, y = 0, z = 0,707, dazu w = 0,707. Keine Drehung ist (0, 0, 0, 1). Beachte das
`kern::dm::sin` – warum, zeigt 3.4.

### Matrizen: `Mat4`

Eine 4×4-Matrix verschiebt, dreht und skaliert in einem Rutsch. Ihre 16 Zahlen liegen **Spalte für
Spalte** (Zahl = Platz):

```
 0  4  8 12
 1  5  9 13
 2  6 10 14
 3  7 11 15
```

Zeile r, Spalte c steht auf Platz `c * 4 + r`. Die Verschiebung steht auf 12, 13, 14.

`trs(t, q, s)` baut eine Matrix aus Verschiebung, Drehung, Skalierung; bei `A * B` wirkt erst `B`,
dann `A`. Aus dem Test `math_matrices`: (1, 0, 0) mal 2 gestreckt → (2, 0, 0), um 90° um Z gedreht
→ (0, 2, 0), um (1, 2, 3) verschoben → **(1, 4, 3)**.

Dazu kommen `perspective` und `ortho` (Projektion), `look_at` (Kamerablick), `normal_matrix3`
(Licht auf schief gestreckten Flächen) und `invert` in doppelter Genauigkeit, damit Klick-Strahlen
auch weit weg treffen.

> **Mission:** Tipp in **Details** bei **Rotation** Z deines Cubes `45*2` und drück Enter.
> Rechenausdrücke sind erlaubt – der Würfel dreht sich um 90°.

### Kugel, Kiste, Kamera

Eine `Sphere` mit negativem Radius gilt als leer; im Zweifel wird gezeichnet. Eine `Aabb`
(achsenparallele Kiste) startet „umgedreht“, damit der erste Punkt sie festlegt.

Die `Camera` ist eine **Beschreibung**, keine Matrix: Augpunkt, Ziel, Oben (+Z), dazu der
**senkrechte** Öffnungswinkel in Radiant (Standard π/3 = 60°) und nahe und ferne Grenze.
Orthografische Kameras geben statt des Winkels die sichtbare Höhe in Metern an. `camera_valid`
prüft alles. Ein Test prüft, dass das Ziel genau in der Bildmitte landet. Aus JavaScript setzt man
die Kamera mit zwölf Zahlen.

> **Mission:** Drück **Alt+J** für die Draufsicht, eine orthografische Kamera: Die Infozeile zeigt
> jetzt die sichtbare Breite in Metern. **Alt+G** schaltet zurück zur Perspektive.

## 3.4 Mathe, die überall gleich rechnet

`sqrt` und `floor` sind exakt festgelegt, `sin`, `cos`, `exp` oder `atan2` nicht: Die Bibliotheken
von Linux, Apple, Android und WebAssembly runden im letzten Bit verschieden. In einer
Robotersimulation wird aus einem Bit binnen Sekunden ein anderer Gang.

Ein zweiter Bit-Dieb heißt **FMA**: `a*b+c` in einem Schritt mit nur einer Rundung. Rechne
0,15 · 3 − 0,04 und behalte nur eine Nachkommastelle:

| Weg | Rechnung | Ergebnis |
|---|---|---|
| zwei Schritte | 0,45 → 0,5; 0,5 − 0,04 = 0,46 | 0,5 |
| ein Schritt | 0,45 − 0,04 = 0,41 | 0,4 |

Gleiche Aufgabe, andere Antwort. Darum gilt überall `-ffp-contract=off`: kein Zusammenziehen.

**Die Lösung:** Kern bringt die Funktionen der Bibliothek **musl** mit, umbenannt in `kern_musl_*`,
und übersetzt sie auf jeder Plattform gleich streng, ohne FMA. Box3D hat eigene Winkelfunktionen.
Benutzt wird alles mit `kern::dm::` davor: `kern::dm::sin`, `kern::dm::atan2`, `kern::dm::pow`
und so weiter. Exaktes wie `sqrt` und `floor` bleibt bei `std::`.

**Die Regeln für neuen Code:**

- Was die **Simulation füttert**, nimmt `kern::dm::`.
- Summen in fester Reihenfolge, nie abhängig von der Zahl der Threads.
- Keine `-ffast-math`-Optionen.

Reine Darstellung darf die normale Bibliothek nehmen; Ton, Primitive, Partikel, Teile des Editors
und die Kamera von `hello` tun das. Liefert solcher Code aber Werte für Physik, Steuerung oder
Training, muss er auf `kern::dm::` umsteigen.

> **Für Profis:** Jeden Fehler zeigt der Fingerabdruck aus [Kapitel 2](02-bauen-und-testen.md) bis
> auf den Regeltakt genau. Langsamer wird nichts: musl ist so flott wie die Bibliothek des Rechners,
> und die meiste Zeit kostet ohnehin Box3Ds Gelenk-Löser.

## Level geschafft!

- Du meldest Fehler mit `fail()` und fängst NaN mit `!(x > 0)`.
- Du weißt, warum ein Span nicht länger leben darf als seine Daten.
- Du kennst Kerns JSON und seine drei Versprechen.
- Du kennst die Regeln: Meter, **Z oben**, Matrizen spaltenweise, Quaternion (x, y, z, w).
- Du weißt, warum die Simulation `kern::dm::` statt `std::` braucht.

## Rätsel-Ecke

1. In welche Richtung zeigt in Kern „oben“?
2. Warum fängt `!(x > 0)` mehr Fehler als `x <= 0`?
3. Auf welchem Platz der 16 Zahlen steht die Verschiebung in Z?
4. **Boss-Rätsel:** Ein Controller berechnet die Laufrichtung der Ente mit `std::atan2`. Der
   Fingerabdruck auf dem Mac weicht ab. Warum, und wie reparierst du es?

### Lösungen

1. Nach +Z.
2. Jeder Vergleich mit NaN ist falsch: `x <= 0` ließe NaN durch, `!(x > 0)` wird wahr und fängt es.
3. Auf Platz 14 (Spalte 3, Zeile 2: 3 · 4 + 2 = 14).
4. `std::atan2` kommt aus der Bibliothek des Rechners und rundet dort im letzten Bit anders. Weil
   der Wert die Simulation füttert, wächst der Unterschied. Lösung: `kern::dm::atan2`.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
