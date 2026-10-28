# 15 Eigene Modelle trainieren – die Werkstatt

> **Kurz gesagt:** Hier wird es praktisch. Du baust das Werkzeug `kern_lehrer`, trainierst einen
> Sprung, liest den Fortschritt, wählst das beste Netz aus und bringst es ins Studio. Zum Schluss
> siehst du, wie eine ganz neue Bewegung dazukommt, und lernst die Fallen kennen, in die schon
> die echten Trainingsläufe getappt sind.

Wie der Lehrer innen rechnet, steht in [Kapitel 14](14-lehrer.md), die Grundlagen in
[Anhang A](A-grundlagen-neuronale-netze.md).

## 15.1 Das Werkzeug bauen

```
kern/tools/lehrer.sh              # baut kern/build/kern_lehrer
kern/tools/lehrer.sh /pfad/out    # baut /pfad/out/kern_lehrer
```

Das Skript läuft auf Linux und macOS, auf x86 und ARM, übersetzt alles mit voller Optimierung,
ohne FMA-Zusammenfassung und Box3D ohne SIMD, und baut nur neu, was sich geändert hat. `CC`/`CXX`
wählen den Compiler, `KERN_JOBS` die Zahl paralleler Compiler, `KERN_NATIVE=0` schaltet die
Optimierung auf den eigenen Prozessor ab.

> **Wusstest du?** Diese Prozessor-Schalter wählen nur Befehle aus, sie ändern **kein** Ergebnis.
> Das Netz bleibt Bit für Bit dasselbe wie im Browser und im Studio
> ([Kapitel 14.7](14-lehrer.md)).

Beim Start lädt `kern_lehrer` Pollens Netze aus `third_party/microduck/` (oder `--policies DIR`).
Es braucht sie für den Warmstart (velstand), das Vorbild und die Spielschicht. Wie Kern selbst
gebaut wird, steht in [Kapitel 2](02-bauen-und-testen.md).

## 15.2 Was das Werkzeug alles kann

| Befehle | Wofür |
|---|---|
| `--task jump` / `stand` | eine Bewegung mit PPO trainieren |
| `--eval`, `--trace` | ein Netz messen; eine Episode Takt für Takt ansehen |
| `--vorbild-suchen`, `--vorbild-pruefen` | ein Sprung-Vorbild mit CMA-ES suchen und testen |
| `--spielen`, `--spielen-pruefen`, `--spielen-ueben`, `--spielen-spur` | die Spielschicht nachahmen, messen, üben, ansehen |
| `--biss-fenster`, `--halt-messen` | Biss und Bremsweg messen |
| `--demo-spur` | das Beispiel-Level wie im Studio abspielen |
| `--fingerprint 400` | Determinismus prüfen |
| `--kalibrieren` und Co. | Kalibrierung ([Kapitel 16](16-kalibrierung.md)) |

Für alle gilt: `--threads N` (Standard: alle Kerne) und `--seed N` (Standard 1).

## 15.3 Den Sprung trainieren

### Der Standardlauf

```
kern/build/kern_lehrer --task jump --iterations 3000 \
    --out jump.onnx --log jump.csv \
    --checkpoint run.ckpt --keep-every 250
```

Genau so entstand `policies/jump.onnx` (32 Enten auf 2 Kernen, Seed 1). Was passiert:

1. **Rezept laden:** Warmstart von velstand, Nachahmung des eingebauten Vorbilds (8 Runden à
   96 Episoden), dann Üben mit Rauschen 0,1, Lernrate 10⁻⁴ und Nachahmungsbelohnung 10. Optionen
   auf der Kommandozeile überschreiben einzelne Werte.
2. **Enten anlegen:** `--envs` (Standard 16 je Thread), abgerundet auf ein Vielfaches der Threads.
3. **Nachahmen** mit DAgger. Danach liegt `jump_nachgeahmt.onnx` da und, mit `--checkpoint`, der
   Lerner in `run.ckpt.nachgeahmt`.
4. **Üben** mit PPO: 30 Iterationen nur der Kritiker, dann beide. Alle 10 Iterationen springt das
   Netz 5-mal auf dem sauberen Roboter zur Probe.
5. **Speichern:** alle 25 Iterationen und am Ende; mit `--keep-every` zusätzlich Schnappschüsse
   wie `jump_it00250.onnx`.

Schafft eine Probe zum ersten Mal mindestens 4 von 5 echten Sprüngen, jubelt das Werkzeug
`FIRST REAL JUMP at iteration …` und speichert sofort. Mit `--stop-after-jump` hört es dann auf.

### Fortsetzen, ohne Vorbild, eigenes Vorbild

`--resume run.ckpt` lädt den ganzen Lerner und überspringt die Nachahmung.

> **Achtung, Falle!** `--iterations` ist beim Fortsetzen das **Ziel**, nicht die Zahl zusätzlicher
> Iterationen. Wer nach 3000 mit `--iterations 3000` fortsetzt, bekommt keine einzige neue. Für
> weitere 3000 also `--iterations 6000`.

Mit `run.ckpt.nachgeahmt` beginnt das Üben neu ab der Nachahmung. `--vorbild none` probiert
reines PPO – der Weg, der den Absprung nicht fand. Damit die Ente ohne Vorbild überhaupt eine
Richtung hat, bekommt die Belohnung dann eine Referenzhöhe (`track_height`). Weitere Schalter ändern
Vorbild, DAgger-Runden, Übungsrauschen und Nachahmungsbelohnung.

Ein eigenes Vorbild suchst du so:

```
kern/build/kern_lehrer --vorbild-suchen demo.json --iterations 120
kern/build/kern_lehrer --vorbild-pruefen demo.json --episodes 20
kern/build/kern_lehrer --task jump --vorbild demo.json \
    --out jump2.onnx
```

Pro Generation erscheint eine Zeile mit Note und Urteil („a real jump“, „no real jump“, „fell“).
Mit `--vorbild start.json` sucht man von einem älteren Ergebnis aus weiter.

**Im Browser** trainiert die Seite `/lehrer/` dasselbe Rezept
([Kapitel 14.11](14-lehrer.md)). Gleicher Seed und gleich viele Enten
insgesamt (Worker × Enten je Worker) ergeben dasselbe Netz wie nativ.

## 15.4 Fortschritt lesen

Jede Übungsiteration schreibt eine Zeile wie `it 512 … return … len … value … std … lr …`, mit
Bewertung zusätzlich `| jumps 4/5 up … rise … flight … tilt … falls …`.

| Feld | Bedeutung | Worauf achten |
|---|---|---|
| `return` | mittlere Rückgabe | soll über Hunderte Iterationen steigen |
| `len` | Episodenlänge in Takten | 100 = 2 s; kürzer: Sturz oder Bodenkontakt |
| `value` | Fehler des Kritikers | erst hoch, dann fallend |
| `std` | Aktionsrauschen σ | sinkt, wenn die Ente sicherer wird (min. 0,001) |
| `lr` | Lernrate | dauerhaft 1e-5: Policy ändert sich zu stark |
| `steps/s` | Entenschritte pro Sekunde | Tempo des Geräts |
| `jumps n/5` | Probe auf dem sauberen Roboter | das eigentliche Ziel |

Beim Nachahmen zeigt jede Runde Beispiele, Fehler und echte Sprünge. Mit `--log jump.csv` landet
jede Iteration als CSV-Zeile in einer Datei, die du direkt in eine Tabellenkalkulation laden kannst (`jumps` ist −1, wo nicht
bewertet wurde).

**Wenn etwas schiefgeht,** zeigt `--trace` eine Episode Takt für Takt: Höhe, Sohlenkontakte,
Bodenkontakt anderer Teile, Neigung und jeden Belohnungsterm.

```
kern/build/kern_lehrer --trace jump_it01000.onnx
kern/build/kern_lehrer --trace builtin          # das Vorbild
kern/build/kern_lehrer --trace jump.onnx --randomized
```

So siehst du, welcher Term die Punkte bringt – und ob die Ente die Belohnung austrickst.

> **Stell dir vor …** die Logzeilen sind die Fieberkurve im Krankenhaus: Eine einzelne Messung sagt
> wenig, der Verlauf über Hunderte Iterationen alles. Und `--trace` ist die Röntgenaufnahme.

## 15.5 Das beste Netz auswählen

Die zuletzt gespeicherte Datei ist nicht immer die beste: PPO kann nach einem guten Stand wieder
schlechter werden, und die Probe im Lauf hat nur 5 Episoden. Darum:

1. Mit `--keep-every N` Schnappschüsse behalten.
2. Jeden mit mehr Episoden messen:
   `for f in jump_it*.onnx; do kern/build/kern_lehrer --eval "$f" --episodes 40; done`.
   Du bekommst zwei Zeilen: „clean robot“ und „randomised“ (volle Variation).
3. Wählen nach: echte Sprünge mit Variation, dann null Stürze, dann Steigen, Flugzeit, Neigung.
4. Im Studio prüfen (15.8).

Für `policies/jump.onnx` ergab das 40 von 40 sauber und 36 von 40 mit Variation, ohne Sturz.

## 15.6 An den Reglern drehen

### Eine Iteration in Zahlen

Was ein Schalter bewirkt, sieht man am besten an einer durchgerechneten Iteration des
Sprung-Rezepts mit 32 Enten:

- **Sammeln:** 32 Enten × 48 Takte = **1536 Zeilen**, jede mit Rauschen σ = 0,1.
- **Normalisierer:** Der des Kritikers lernt aus den 1536 Zeilen, der des Actors ist eingefroren.
- **GAE:** pro Ente rückwärts über 48 Takte, γ = 0,99, λ = 0,95; danach Vorteile auf Mittel 0,
  Streuung 1.
- **Minibatches:** 1536 / 4 = **384 Zeilen**, jede in 8 Teilstücke à 48 Zeilen.
- **Updates:** 5 Epochen × 4 Minibatches = **20 Adam-Schritte**, vor jedem die KL-Messung.

Die Nachahmung hätte höchstens 76 800 Beispiele (8 × 96 Episoden × 100 Takte); es waren 49 936,
weil Episoden bei Sturz oder Bodenkontakt früher enden. Nach jeder Runde wird der Actor in
10 Epochen (Minibatch 256) an alle bisherigen Beispiele angepasst.

### Die wichtigsten Schalter

| Option | Sprung-Rezept | Wirkung |
|---|---|---|
| `--envs N` | 16 je Thread | mehr Daten, ruhigerer Gradient; **bestimmt das Ergebnis** |
| `--threads N` | alle Kerne | die Geschwindigkeit – bei festem `--envs`; ohne `--envs` gibt es 16 Enten je Thread |
| `--steps N` | 48 | Takte je Ente und Iteration |
| `--lr X`, `--kl X` | 10⁻⁴, 0,01 | Startlernrate, Ziel-KL |
| `--uebungs-std X` | 0,1 | Rauschen beim Üben |
| `--entropy X` | 0,005 | bremst das Schrumpfen des Rauschens |
| `--substeps N` | 16 | nicht ändern! |

Dazu kommen Epochen, Minibatches, Netzgrößen (beim Warmstart muss der Actor zu velstand passen),
Anfangsrauschen (`--init-std`), adaptive Lernrate an/aus, Warmstart (nur `velstand` wirkt, alles
andere heißt Zufallsgewichte; von einer Datei startet nur die Spielschicht) und
`--profil servo.json` für kalibrierte Servos ([Kapitel 16](16-kalibrierung.md)); alles Weitere in
[Kapitel 17](17-referenz.md).

> **Stell dir vor …** du hast einem Hund gerade mühsam einen Trick beigebracht. Brüllst du jetzt
> neue Kommandos und wedelst wild mit Leckerlis, ist der Trick schnell wieder weg. Genauso beim
> Netz: Nach der Nachahmung sind **kleine Lernrate (10⁻⁴) und kleines Rauschen (0,1)** Pflicht.

Außerdem: Größeres Rauschen hilft **nicht** beim Finden (ohne Warmstart lernte die Ente damit
nur stehen). Wenige Enten bremsen, weil der Gradient verrauscht. Und weniger Sub-Steps wären
1,6-mal schneller, aber dann lernte das Netz eine andere Physik als die des Studios.

## 15.7 Die Spielschicht trainieren

Sechs Schritte:

1. **Messen, was das Vorbild braucht:** `--biss-fenster` legt eine Socke auf ein 11 × 9-Gitter
   vor die Ente und druckt, wo der Biss hält. `--halt-messen` zeigt den Bremsweg nach „Stopp“.
   Das Training misst den Biss ohnehin selbst; die Befehle sind zum Nachsehen.
2. **Nachahmen:**
   `kern_lehrer --spielen --nachahmen 12 --episoden 32 --out play.onnx --checkpoint play.ckpt`.
   Standard sind 8 Runden à 64 Episoden. Pro Runde misst das Werkzeug das Netz in 16 sauberen
   Episoden.
3. **Prüfen:** `--spielen-pruefen vorbild` und `--spielen-pruefen play.onnx`. Das Vorbild zu messen
   zeigt, was das Netz durch reines Nachahmen höchstens erreichen kann.
4. **Ansehen:** `--spielen-spur --seed 3` zeigt eine Episode Entscheidung für Entscheidung (25 pro
   Sekunde); mit `--netz play.onnx` fährt das Netz, und die Aktionen von Vorbild und Netz stehen
   nebeneinander. So siehst du, wo sie auseinandergehen.
5. **Üben:** `--spielen-ueben play.onnx --iterations 300 --envs 32 --out play_geuebt.onnx`:
   Warmstart von der Datei, Rauschen 0,1, 20 Iterationen nur Kritiker, dann PPO. Alle
   20 Iterationen eine Messung und ein Schnappschuss.
6. **Im Beispiel-Level prüfen:** `--demo-spur --netz play_geuebt.onnx --seed 120` spielt das
   Micro-Duck-Level wie das Studio, 120 s lang (hier ist `--seed` die Dauer!), eine Zeile pro
   Sekunde: Lage, Zustand, was der Tracker (das Gedächtnis der Spielschicht) weiß und was das
   Spielnetz antwortet. Es nimmt `jump.onnx` und dein Spielnetz (sonst `play.onnx`). Mit
   `KERN_JUMP_AT=SEKUNDE` drückt es in dieser Sekunde Taste 7.

> **Achtung, Falle!** `--out` wird beim Nachahmen nach **jeder** Runde überschrieben. Willst du eine
> frühere Runde behalten, kopier die Datei während des Laufs. (Beim alten `play.onnx` war Runde 10
> von 12 die beste.)

## 15.8 Im Studio benutzen

Kerns eigene Netze haben feste Namen: `jump.onnx` und `play.onnx`. Drei Wege ins Studio:

1. **Ins Repository:** nach `kern/policies/` kopieren. Die Entwicklerseite lädt von dort, eine
   ausgelieferte Seite aus `policies/` daneben. Die Engine holt ein Netz erst, wenn eine Ente es
   braucht.
2. **Aus dem Browser-Lehrer:** Die Lehrer-Seite speichert ihr Netz auf demselben Gerät; das Studio
   lädt es beim Start und übernimmt jedes neue sofort, auch in offenen Tabs (nur für den Sprung).
3. **Prüfen:** Micro-Duck-Level öffnen, Play, mit **F8** die Ente übernehmen, Taste **7** für den
   Sprung, oder die Ente in Ruhe lassen (Spielschicht: Autonomous und Kern extras an, Ball oder
   Socke in Sicht). Die Browser-Konsole meldet „Kern: trained network jump.onnx loaded“ oder warnt, wenn etwas nicht lädt.

Das Netz muss die Form eines rsl_rl-Actors haben (Sub, Div, Gemm/Elu …, Gemm) – genau so exportiert
der Lehrer. Eine Spielschicht ohne 23 Eingänge und 9 Ausgänge wird nicht gerechnet (ihre Aktion
bleibt 0).

> **Probier's aus!** Trainiere auf der Lehrer-Seite einen Sprung, lass dabei auf
> https://kernengine.de/ das Micro-Duck-Level offen, starte Play, übernimm die Ente mit **F8** und
> drück **7**, sobald die Konsole „trained network jump.onnx loaded“ meldet. Jetzt springt
> **deine** Ente.

## 15.9 Eine neue Bewegung hinzufügen

Eine neue Bewegung mit Pollens Layout (61 Beobachtungen → 14 Aktionen) braucht vier Dinge, für
schwere Bewegungen oft ein fünftes:

1. **Eine `Task`** in `src/lehrer/`. Das blaue Buch zeigt als Gerüst eine Verbeugung `BowTask`:
   2 s Dauer, zufälliger Start, Phasensignal in den Plätzen 55 und 56, vier Geheimwerte für den
   Kritiker, ein Lehrplan für den Sturz-Malus, eine Belohnung aus benannten Termen. Hilfen wie
   `upright`, `action_rate` und `fell` liegen schon in `tasks.cpp`; eine Fabrikfunktion
   `make_bow_task()` macht die Klasse von außen nutzbar.
2. **Rezept und Name** in `src/lehrer/recipe.cpp`:

   ```cpp
   std::unique_ptr<Task> make_task(const std::string& name) {
     if (name == "jump") return make_jump_task();
     if (name == "stand") return make_stand_task();
     if (name == "bow") return make_bow_task();          // neu
     return nullptr;
   }
   ```

   Die Funktion übersetzt Namen in Aufgaben; die neue Zeile liefert für „bow“ die Verbeugung,
   Unbekanntes ergibt nichts. Dazu bekommt `recipe_for` einen Eintrag (Warmstart, Rauschen,
   Lernrate). Danach kennen `--task bow` und der Browser die Aufgabe; auf der Lehrer-Seite fehlt nur ein Menüeintrag.
3. **Eine Messung**, die prüft, was die Bewegung **ist**, nicht was die Belohnung zählt.
   `JumpMeter` zeigt das Muster: Takt für Takt beobachten, am Ende entscheiden. `--eval` und die
   Probe im Lauf kennen derzeit nur den Sprung; eine neue Aufgabe braucht ihre eigene Bewertung.
4. **Die Verdrahtung im Studio** wie beim Sprung: Bewegungsliste, Taste, Touch-Knopf, Name im
   Logik-Knoten **Robot Move**, Dateiname und die Datei in `kern/policies/`. Im Studio wechselt die
   Ente dann hart auf das neue Netz und nach dessen Dauer zurück; die Beobachtung baut das Studio
   genau wie im Training (mit Phase).
5. **Ein Vorbild (optional):** eine kleine Parametrisierung, eine Note für CMA-ES und ein
   Vorbild-Objekt, das in **jedem** Zustand eine Antwort hat (sonst kein DAgger). CMA-ES und
   Nachahmung sind allgemein; die Vorbild-Funktionen sind derzeit auf den Sprung zugeschnitten.

Eingänge, die velstand nie gesehen hat, müssen beim Warmstart als neu angegeben werden (derzeit
die Phasenplätze 55 und 56). Für ein ganz anderes Layout zeigt die Spielschicht den Weg: eigene
Laufzeit-Ebene, Umgebung, Lerner, Metadaten und Bewertung – PPO und Nachahmung bleiben dieselben.

## 15.10 Gute Aufgaben entwerfen

Was Kerns Netze vorleben:

- **Nur, was der Roboter wirklich hat.** Der Actor sieht Sensorwerte (IMU, Gelenke,
  Kamera-Detektor, Mund-Encoder), nie wahre Positionen; Ball und Socke kennt die Spielschicht nur
  verrauscht und mit Aussetzern. Wahres gehört in die Geheimwerte des Kritikers.
- **Zeit sichtbar machen.** Eine Bewegung mit Ablauf bekommt ein Phasensignal (cos, sin), keinen
  Zähler. Ein Netz ohne Gedächtnis braucht Zeitgrößen: Die Spielschicht sieht, wie lange sie steht,
  geht und trägt.
- **Messen statt befehlen.** „Steht still“ heißt „beide Sohlen am Boden, keine Bewegung läuft“ – was
  die Beine tun, nicht was befohlen wurde.
- **Aktionen wie Bedienelemente:** Laufbefehle mit Totzone wie ein Stick (unter 20 % steht die
  Ente), Bewegungen über eine Schwelle (über 0,5 startet), Grenzen für Kopf und Nacken.
- **Belohnung aus benannten Termen,** Gewicht × Term × dt, mit Bremsen wie bei Pollen
  (Aktionsänderung, Drehrate, Eigenkontakt); Lehrpläne machen Strafen und Variation nach und nach
  strenger.
- **Episodenende hart:** Sturz und Bodenkontakt anderer Teile beenden die Episode.
- **Vorbild ohne Geheimwissen:** Es sieht nur, was das Netz sieht; ein Wunsch, der aus einer Dauer
  folgt, steigt als Rampe an.

> **Probier's aus!** Starte auf https://kernengine.de/ Play im Micro-Duck-Level (Kern extras ist
> dort an) und heb den Ball mit der Maus an eine andere Stelle. Beobachte, wie die Spielschicht nur
> aus ihren Sensoren neu entscheidet: hinschauen, hingehen, stehen, kicken.

## 15.11 Fallstricke aus den echten Läufen

Aus `docs/lehrer.md`, damit du nicht hineintappst:

- **Die Belohnung wird ausgenutzt.** Frühe Enten saßen auf dem Schwanz, machten Hechtsprünge oder
  stützten sich ab. Jedes Mal half nur eine strengere Regel: Bodenkontakt beendet die Episode,
  Neigung kostet, Punkte gibt es nur aufrecht.
- **Die Messung war zu locker.** Die erste ließ eine Ente durch, die sich nur aufrichtete und die
  Füße anzog, mit 45° Kippen. Seitdem zählt nur ein echter Sprung (Steigen in der Luft, Flug,
  höchstens 30° Neigung, Landen).
- **PPO allein findet seltene Bewegungen nicht;** ohne „Kritiker zuerst“ geht Nachgeahmtes
  kaputt; wandernde Normalisierer verfälschen das Update (alles in Kapitel 14).
- **Fehler in der Welt statt im Netz.** Beim Training der Spielschicht kamen drei ans Licht: Die
  Socke kroch von selbst über den Boden, der Ball rollte ewig, und das Gähnen im Leerlauf öffnete
  beim Aufheben den Schnabel. Abhilfe: Rollwiderstand für Socke und Ball, und eine laufende
  Bewegung zählt nicht mehr als Stillstehen. Lernt ein Netz an einer Stelle nicht, zeigen
  `--spielen-spur` und `--trace` oft so einen Fehler.
- **Zahlen von Hand veralten.** Die Bissposition stand früher als Zahl in der Doku; jetzt misst der
  Lehrer sie bei jedem Lauf.
- **Determinismus brechen:** ein `std::sin`, eine Summe in Thread-Reihenfolge oder `-ffast-math`,
  und das Netz ist auf einem anderen Gerät ein anderes. `--fingerprint 400` verrät es.
- **Andere Physik als das Studio:** weniger Sub-Steps, anderer Boden, anderes Servoprofil – und
  das Netz lernt eine andere Welt.

## Das nimmst du mit

- `kern/tools/lehrer.sh` baut das Werkzeug; Prozessor-Schalter ändern kein Ergebnis.
- Ein Standardlauf ahmt erst nach, übt dann und speichert regelmäßig Schnappschüsse.
- `return`, `len` und vor allem `jumps` zeigen den Fortschritt; `--trace` zeigt, was schiefgeht.
- Das letzte Netz ist nicht immer das beste: Schnappschüsse mit `--eval` sauber und variiert messen.
- Die Zahl der Enten bestimmt das Ergebnis, die Zahl der Threads nur das Tempo.
- Nach der Nachahmung: kleine Lernrate, kleines Rauschen, Kritiker zuerst.
- Eine neue Bewegung braucht Aufgabe, Rezept, Messung, Studio-Verdrahtung und oft ein Vorbild.

## Knobelecke

1. Du hast 3000 Iterationen trainiert und willst 1000 weitere. Welchen Wert gibst du bei
   `--iterations` an?
2. Warum misst man die Kandidaten zum Schluss mit 40 Episoden statt sich auf die 5er-Probe zu
   verlassen?
3. Dein Rechner hat plötzlich doppelt so viele Kerne. Ändert sich dein Netz, wenn du `--threads`
   verdoppelst? Und wenn du `--envs` verdoppelst?
4. Warum soll die Messung einer neuen Bewegung nicht einfach die Belohnung aufsummieren?

### Lösungen

1. 4000 – `--iterations` ist das Ziel.
2. Fünf Episoden sind ein sehr kleiner, verrauschter Test; ein Netz kann zufällig gut aussehen.
   Und erst mit Variation zeigt sich, ob es robust ist.
3. Mehr Threads bei festem `--envs`: gleiches Netz, nur schneller. Ohne `--envs` bringen mehr Threads
   aber mehr Enten mit (16 je Thread) – und mehr Enten heißt: anderes Netz.
4. Weil Netze Lücken in der Belohnung ausnutzen. Die Messung muss prüfen, ob die Bewegung
   wirklich passiert ist.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
