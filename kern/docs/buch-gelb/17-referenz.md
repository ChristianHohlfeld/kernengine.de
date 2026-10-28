# 17 Spickzettel: Tasten, Befehle, Begriffe

> **Dein Level:** Das letzte Level ist dein Nachschlagewerk. Hier findest du schnell die wichtigsten
> Tasten, die Befehle des Lehrers, die Dateiformate, die Schnittstelle zum Browser und ein Glossar
> mit allen Fachwörtern des Buchs. Die vollständige Referenz mit jeder Funktion und jedem Parameter
> steht im blauen Buch.

## 17.1 Tasten im Studio

Die Belegung folgt der Unreal Engine. **F1** zeigt die wichtigsten Kürzel auch im Studio.

### Kamera im Viewport

| Eingabe | Wirkung |
|---|---|
| rechte Maustaste + W A S D | fliegen |
| … + Q / E | runter / hoch |
| … + Shift | 2,5-mal schneller |
| … + Mausrad | Fluggeschwindigkeit 1–8 |
| Alt + linke Maustaste | um die Auswahl kreisen |
| mittlere Maustaste | schwenken |
| F | Auswahl einrahmen |
| Ctrl+1 … 9 / 1 … 9 | Lesezeichen setzen / anspringen |

Am Telefon: ein Finger kreist, zwei Finger zoomen und verschieben, Tippen wählt aus.

### Bearbeiten

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Auswählen / Verschieben / Drehen / Skalieren |
| [ / ] | Raster kleiner / größer |
| Entf | Auswahl löschen |
| End | Auswahl auf den Boden setzen |
| H / Ctrl+H | ausblenden / alles einblenden |
| G | Game View |
| F2 | umbenennen |
| Ctrl+Z / Ctrl+Y | rückgängig / wiederholen |
| Ctrl+C / V / D | kopieren / einfügen / duplizieren |
| Ctrl+G | gruppieren |
| Ctrl+S / Ctrl+O / Ctrl+N | speichern / öffnen / neues Level |
| Ctrl+Space | Content Drawer |
| Alt+2 / 3 / 4 / 5 | Wireframe / Unlit / Lit / Player Collision |
| Alt+P / Alt+S | Play / Simulate |

### Play und Simulate

| Taste | Wirkung |
|---|---|
| Esc | Stop |
| F8 | Spieler verlassen (Eject) / übernehmen (Possess) |
| Pause | anhalten / weiter |
| . | pausiert: einen Schritt weiter |
| Ctrl+R | Spiel neu starten |
| K | in Simulate: Posen ins Level übernehmen |
| Shift+F1 | Maus freigeben |

Im Spiel: W A S D bewegen, Maus umsehen, Leertaste springen, Shift Boost, F benutzen.

### Logik-Editor

F7 kompiliert, F9 setzt einen Haltepunkt (Breakpoint), Ctrl+F sucht, C legt einen Kommentar um die
Auswahl, Esc führt zurück zum Viewport.

> **Mission:** Öffne Kern Studio auf https://kernengine.de/ und drück **F1**. Vergleiche den
> Controls-Dialog mit diesen Tabellen. Probier dann Ctrl+1 auf einer schönen Kameraposition, flieg
> weg und drück **1** – du springst zurück.

## 17.2 Tasten der Micro Duck

W/S laufen, A/D drehen, **1** sitzen, **2** aufheben, **3** und **4** kicken, **5** rollen,
**6** Schnabel, **7** springen, **Leertaste** quaken, **V** „Wheee“. Gamepad und Touch stehen in
[Kapitel 13](13-micro-duck.md).

## 17.3 Befehle des Lehrers

`kern_lehrer` ist der Lehrer als Programm für deinen Rechner (gebaut mit `kern/tools/lehrer.sh`,
[Kapitel 2](02-bauen-und-testen.md), [Kapitel 15](15-eigene-modelle.md)). Er liest alle Optionen,
lädt Pollens Netze und macht dann **das Erste**, das zutrifft:

1. `--fingerprint N`: Determinismus-Fingerabdruck über N Takte.
2. `--eval FILE`: einen Sprung bewerten.
3. `--trace FILE`: eine Episode Takt für Takt zeigen.
4. `--kalibrieren`, `--abgleich`, `--beispiele`: Kalibrierung ([Kapitel 16](16-kalibrierung.md)).
5. `--vorbild-suchen`, `--vorbild-pruefen`: Sprung-Vorbild suchen und prüfen.
6. `--spielen-pruefen`, `--spielen-ueben`, `--spielen-spur`: Spiel-Netz prüfen, üben, verfolgen.
7. `--biss-fenster`, `--halt-messen`: Biss und Nachlauf messen.
8. `--demo-spur`: das Micro-Duck-Beispiellevel abspielen.
9. `--spielen`: Spiel-Netz durch Nachahmung lernen.
10. sonst: **Training** der Aufgabe `--task` (`jump` oder `stand`).

Vor Nummer 2 prüft er, ob `--task` `jump` oder `stand` ist. `--spielen`, `--spielen-spur`,
`--biss-fenster`, `--halt-messen` und `--demo-spur` teilen sich einen Schalter: Es gilt der letzte
auf der Kommandozeile. Eine unbekannte Option zeigt die Kurzhilfe.

### Die wichtigsten Optionen

| Option | Standard | Bedeutung |
|---|---|---|
| `--task` | `jump` | Aufgabe |
| `--threads N` | Hardware-Threads | Rechen-Threads |
| `--envs N` | 16 × Threads | Enten gleichzeitig |
| `--seed N` | 1 | Startwert für Zufall (bei `--demo-spur`: Dauer in s) |
| `--iterations N` | 600 | Trainingsrunden |
| `--episodes N` | 10 | Episoden beim Bewerten |
| `--out FILE` | `<task>.onnx` | Ausgabedatei |
| `--profil FILE` | XL330 | Servoprofil |

**Training (PPO), Rezept `jump` / `stand`:** `--steps` (48 / 24), `--init-std` (0,3 / 1,0),
`--lr` Lernrate (1e-4 / 1e-3), `--entropy` (0,005 / 0,01), `--actor` (512,256,128), `--critic`
(256,128), `--epochs` (5), `--minibatches` (4), `--adaptive-lr` und `--kl` (Lernrate anpassen,
Ziel 0,01), `--substeps` (16), `--warm-start` (`velstand` / `none`).

**Nachahmen:** `--vorbild builtin|demo.json|none`, `--nachahmen` (8 Runden), `--episoden` (96),
`--uebungs-std` (0,1), `--nachahmungs-lohn` (10).

**Speichern:** `--log` (CSV je Runde), `--checkpoint` und `--resume` (Zwischenstand sichern und
fortsetzen), `--save-every` (25), `--keep-every` (Schnappschüsse), `--eval-every` (10; meldet
`FIRST REAL JUMP`), `--stop-after-jump`.

**Sonst:** `--randomized` (mit Variation), `--netz play.onnx` (Spiel-Netz für die Spur),
`--policies DIR` (wo Pollens Netze liegen).

```sh
kern/tools/lehrer.sh
cd kern/build
./kern_lehrer --task jump --iterations 3000
./kern_lehrer --eval jump.onnx --episodes 40
./kern_lehrer --trace jump.onnx
./kern_lehrer --fingerprint 400
```

Zeile 1 baut das Programm, Zeile 2 wechselt in den Ordner. Zeile 3 trainiert den Sprung 3000 Runden
lang. Zeile 4 bewertet ihn in 40 Episoden, Zeile 5 zeigt eine Episode Takt für Takt. Zeile 6 gibt den
Fingerabdruck aus, den man mit `kern/tests/determinism_fingerprint.txt` vergleicht.

> **Für Profis:** Das Ergebnis eines Trainings hängt von der Zahl der Enten (`--envs`) ab, nicht
> von der Zahl der Threads. Mehr Threads machen es schneller, aber nicht anders.

## 17.4 Dateiformate

### Levels (`.klevel`)

Ein Level ist eine JSON-Textdatei mit `"format": "kern-level"` und `"version": 1`. Darin stehen
`name`, `world` (World Settings), `camera`, `nextId`, `actors`, eingebettete `textures`, `sounds`
und, falls vorhanden, `logic`. Einheiten: Meter, Sekunden, Kilogramm, Z zeigt nach oben, Winkel in
Grad, Farben von 0 bis 1. Ein anderes Format lehnt Kern ab („not a Kern level“), eine neuere Version
auch.

> **Mission:** Speichere ein Level mit **Ctrl+S** und öffne die `.klevel`-Datei in einem
> Texteditor. Findest du `"format": "kern-level"` und deine Actors?

### Netze (`.onnx`)

Kern liest und schreibt Netze in der Form, die Pollen für die Micro Duck nutzt:

```
obs → Sub → Div → (Gemm, Elu)* → Gemm → actions
```

Erst werden die Eingaben normalisiert (minus Mittelwert, geteilt durch Streuung), dann folgen
Schichten (**Gemm** heißt: Gewichte mal Eingaben plus Versatz) mit **ELU** dazwischen. Bewegungsnetze
haben 61 Eingaben und 14 Ausgaben, `play.onnx` hat 23 und 9. Anderes lehnt Kern mit Grund ab, etwa
„expected Elu after a hidden Gemm“. Beim Export schreibt der Lehrer **Metadaten** dazu: Gelenknamen,
Grundstellung, Aufgabe und „lehrer“. Pollens Pick-up-Detektor hat eine andere Form und ist fest
eingebaut. Pollens Netze liegen in `kern/third_party/microduck/`, Kerns in `kern/policies/`; im
Browser trainierte Netze speichert die Lehrer-Seite im Browser (IndexedDB); auf demselben Gerät
ersetzen sie die mitgelieferten.

### Servoprofil (`.json`)

```json
{
  "kind": "servo", "name": "xl330",
  "stiffness": 0.554, "damping": 0.053,
  "friction": 0.0048, "torque": 0.963,
  "armature": 0.0018, "delay": 0.02,
  "spreadArmature": 0.1, "spreadFriction": 0.1,
  "delayMin": 0.015, "delayMax": 0.03
}
```

`stiffness` und `torque` müssen größer als 0 sein, Streuungen und Verzögerungen liegen zwischen 0
und 1. Fehlende Felder sind 0. Mögliche Meldungen: `not a servo profile`, `bad value for <feld>`.

### Logs (CSV)

Ein **Prüfstand-Log** hat oben die Last (`# mass`, `# arm`, `# inertia`, `# horizontal`), dann
`time,target,angle`. Ein **Roboter-Log** hat je Regeltakt 61 Beobachtungen und 14 Aktionen.
Einzelheiten in [Kapitel 16](16-kalibrierung.md).

## 17.5 Die C-ABI: wie JavaScript mit Kern spricht

Die Engine ist ein **WebAssembly-Modul**. JavaScript im Browser ruft ihre Funktionen über eine
schlichte Liste auf, die **C-ABI**. Die Regeln:

- Jede Funktion heißt im Modul genauso wie im C++-Code, etwa `kern_render`.
- Der Browser ruft **einmal** `_initialize`, dann die Funktionen (ein „WASI-Reactor“).
- Daten gehen über den gemeinsamen Speicher: Der Browser reserviert Platz mit `kern_alloc`, schreibt
  hinein und übergibt die Adresse (einen **Zeiger**, 32 Bit).
- Rückgabe 1 heißt Erfolg, 0 Fehler; die Meldung liefert `kern_last_error()`.
- Texte gehen als Zeiger plus Länge, in UTF-8.
- Antworten (JSON, Bytes) gehören dem Modul und gelten bis zum nächsten Aufruf derselben Gruppe.

Umgekehrt braucht das Modul vom Browser: die **WebGL2**-Funktionen (`kern_gl`), Uhrzeit, Log,
Höhenabfrage, Statustext, Pointer-Lock und das Übersetzen von Logik (`kern_env`) sowie ein paar Grundfunktionen
(`wasi_snapshot_preview1`). Dateien berührt die Engine nie.

| Gruppe | Wofür |
|---|---|
| `kern_alloc`, `kern_last_error` … | Speicher und Fehler |
| `kern_renderer_*`, `kern_mesh_*` … | Zeichnen: Meshes, Texturen, Kamera, Licht, Himmel, Schatten, Picking |
| `kern_fx_*` | Partikel |
| `kern_geo_*` | Gebäude, Straßen, Gelände |
| `kern_app_*` | eigene C++-Spiele (`--app`) |
| `kern_studio_*` | Kern Studio (mit `--studio`) |
| `kern_lehrer_*` | der Lehrer im Browser (mit `--lehrer`) |
| `kern_logic_*` | wird von übersetzter Logik aufgerufen |

Meshes beschreibt man mit **Deskriptoren**: festen Listen von Zahlen, in denen jede Stelle eine
Bedeutung hat (Positionen, Farben, Flags wie „sichtbar“ oder „wirft Schatten“). Ein Spiel kann
eigene Shader-Erweiterungen mitliefern. Jede Signatur steht im blauen Buch.

## 17.6 Glossar

Alle Fachwörter des Buchs, kurz erklärt. Die Zahl in Klammern nennt das Kapitel, in dem es
ausführlich vorkommt.

### Studio und Spiele

- **Actor:** ein Ding im Level mit Position und Bausteinen (8).
- **Autosave:** automatische Sicherung ungespeicherter Arbeit (11).
- **Blueprint / Logik-Graph:** ein Programm aus Knoten und Verbindungen, ohne Tippen (9).
- **Content Drawer:** die Schublade mit Dingen zum Platzieren (11).
- **Controller:** bestimmt, wie der Spieler sich bewegt: laufen, rollen, fahren, fliegen (8).
- **Details:** das Panel mit allen Einstellungen des gewählten Actors (11).
- **Eject / Possess:** im Spiel den Spieler verlassen / wieder übernehmen (11).
- **Game View:** Ansicht ohne Editor-Hilfen, wie im Spiel (11).
- **Gizmo:** Pfeile und Ringe zum Verschieben, Drehen, Skalieren (11).
- **HUD:** Anzeigen im Spiel wie Punkte und Zeit (8).
- **Knoten:** ein Baustein im Logik-Graphen (9).
- **Komponente:** ein Baustein eines Actors, etwa Mesh, Kollider oder Sound (8).
- **Level:** eine Spielwelt, gespeichert als `.klevel` (8).
- **Observer camera:** Play mit freier Kamera zum Zuschauen (11).
- **Outliner:** die Baumansicht aller Actors (11).
- **Package Game:** ein Level als eigenständige HTML-Datei exportieren (11).
- **Play / Simulate:** spielen / Physik laufen lassen, während der Editor bedienbar bleibt (11).
- **Player-only:** Start ohne Editor, für exportierte Spiele (11).
- **Snapping:** Einrasten auf ein Raster (11).
- **Trigger:** ein Bereich, der etwas auslöst, wenn man ihn betritt (8).
- **Viewport:** das Fenster, durch das du in die 3D-Welt schaust (11).
- **World Settings:** Einstellungen der ganzen Welt, etwa die Schrittrate (11).

### Grafik und Geometrie

- **BVH:** ein Baum aus Schachteln, mit dem man schnell findet, was ein Strahl trifft (5).
- **Bloom:** Leuchten heller Stellen (4).
- **Deskriptor:** eine feste Zahlenliste, die etwa ein Mesh beschreibt (17).
- **Earcut:** Verfahren, das Vielecke in Dreiecke zerlegt (5).
- **Frame:** ein einzelnes Bild; viele pro Sekunde ergeben Bewegung (4).
- **Frustum-Culling:** nicht zeichnen, was außerhalb des Blickfelds liegt (5).
- **GTAO:** dunkelt Ecken und Ritzen realistisch ab (4).
- **Instancing:** dasselbe Mesh vielfach auf einmal zeichnen (4).
- **Kontextverlust:** der Browser nimmt die Grafikkarte kurz weg; Kern baut alles neu auf (4).
- **Material:** wie eine Oberfläche aussieht: Farbe, Rauheit, Metall (4).
- **Mesh:** eine 3D-Form aus Dreiecken (4).
- **NightWindows:** Shader-Erweiterung für erleuchtete Fenster bei Nacht (4).
- **Partikel:** viele kleine Punkte für Funken, Rauch, Staub (4).
- **Pass:** ein Durchgang beim Zeichnen, etwa für Schatten (4).
- **Picking:** herausfinden, was unter dem Mauszeiger liegt (5).
- **Renderer:** der Teil, der die Bilder zeichnet (4).
- **Shader:** kleines Programm auf der Grafikkarte, das Punkte und Farben berechnet (4).
- **ShaderExtension:** Zusatz zum Standard-Shader für eigene Looks (4).
- **Skinning:** ein Mesh mit Knochen verformen, etwa für Figuren (4).
- **Textur:** ein Bild auf einer Oberfläche (4).
- **Vertex:** ein Eckpunkt eines Dreiecks (4).
- **Viewmodel-Ebene:** wird nach dem Löschen der Tiefe gezeichnet und liegt so immer vorn, etwa für Gizmos (4).
- **WebGL2:** die Schnittstelle des Browsers zur Grafikkarte (4).
- **Welding:** doppelte Eckpunkte ohne Verlust zusammenfassen (5).

### Mathe und Technik

- **C++ / JavaScript:** die Programmiersprache der Engine / die Sprache der Webseiten (1).
- **C-ABI:** die Funktionsliste, über die JavaScript mit der Engine spricht (17).
- **CI:** automatische Prüfung jeder Änderung auf einem Server (2).
- **DuckEnv:** die Lehrer-Umgebung: eine Micro Duck in ihrer eigenen Spielwelt (14).
- **Determinismus:** gleiche Eingaben ergeben Bit für Bit dasselbe Ergebnis (6, 14).
- **`dm`:** Kerns Rechenfunktionen, die überall exakt gleich rechnen (3).
- **Fehlerkanal:** Kern meldet Fehler mit `fail()` und `last_error()` (3).
- **Fingerabdruck:** eine Prüfzahl je Takt, die jede Abweichung verrät (2).
- **GameWorld:** spielt ein Level auf Box3D und zeigt es an (8).
- **JIT:** ein Logik-Graph wird während des Spiels in WebAssembly übersetzt (9).
- **Interpreter:** führt einen Logik-Graphen Knoten für Knoten aus (9).
- **JSON:** ein lesbares Textformat für Daten (3).
- **Matrix:** eine Zahlentabelle, die Drehen, Verschieben, Skalieren auf einmal beschreibt (3).
- **Quaternion:** vier Zahlen, die eine Drehung beschreiben (3).
- **Radiant:** Winkelmaß; ein Kreis hat 2π ≈ 6,28 rad (13).
- **Reactor (WASI):** ein WebAssembly-Modul ohne Hauptprogramm, nur mit Funktionen (17).
- **Span:** ein Blick auf eine Reihe von Elementen, ohne sie zu kopieren (3).
- **Tier:** wie ein Graph läuft: Interpreter, WebAssembly oder C++ (9).
- **Vektor:** ein Pfeil mit Richtung und Länge, etwa (x, y, z) (3).
- **wasi-sdk:** das Werkzeug, das C++ in WebAssembly übersetzt (2).
- **WebAssembly:** ein Programmformat, das Browser sehr schnell ausführen (1).
- **Zeiger:** eine Adresse im Speicher (17).

### Physik

- **Box3D:** die Physik-Engine von Erin Catto, die Kern unverändert mitbringt (6).
- **CharacterMover:** bewegt eine Spielfigur als Kapsel, die andere Körper anschiebt (6).
- **Gelenk:** verbindet zwei Körper, etwa als Scharnier (6).
- **Körper (Body) / Form (Shape):** was sich bewegt / welche Gestalt es hat (6).
- **Kollider:** die unsichtbare Form, mit der etwas zusammenstößt (6).
- **Kontakt:** eine Stelle, an der sich zwei Körper berühren (6).
- **Konvexe Hülle:** eine Form ohne Dellen nach innen (13).
- **MouseGrab:** einen Körper mit Maus oder Finger greifen (6).
- **Moment:** eine Drehkraft, gemessen in N·m (13).
- **Ray-Cast:** ein Strahltest: Was trifft ein Strahl zuerst? (6)
- **Reibung / Restitution / Rollwiderstand:** Haften / Abprallen / Bremsen beim Rollen (6).
- **Substeps:** Unterschritte je Physikschritt; für die Ente 16 (6).
- **Trägheit:** wie schwer sich etwas in Bewegung oder Drehung versetzen lässt (6).
- **Zeitschritt:** die feste Zeit, um die die Physik weiterrechnet (6, 8).

### Klang und Netzwerk

- **Hall:** der Nachklang eines Raums (7).
- **Mixer:** mischt alle Klänge zu einem (7).
- **Synthesizer:** erzeugt Klänge aus Rechnung statt aus Aufnahmen (7).
- **3D-Klang:** Klang, der von einer Stelle im Raum kommt (7).
- **Client / Server:** Mitspieler / der Rechner, der das Spiel bestimmt (10).
- **Datagramm:** ein einzelnes Datenpaket im Netz (10).
- **HostTransport:** Netz, dessen Pakete die Browserseite befördert, etwa per BroadcastChannel oder Relay (10).
- **Interpolation:** zwischen zwei bekannten Zuständen weich überblenden (10).
- **LoopbackNetwork:** ein Netz im Speicher, für Tests und Mehrspieler im Editor (10).
- **NetGame:** Kerns Mehrspieler: ein Server bestimmt, alle bekommen Schnappschüsse (10).
- **Relay:** ein Vermittler, der Pakete zwischen Browsern weiterreicht (10).
- **Snapshot:** ein Schnappschuss des Spielzustands (10).

### Roboter

- **Armatur:** Drehmasse des Motors im Servo (13).
- **Encoder:** misst den Winkel eines Servos (13).
- **Gerätemodell:** kleines Modell eines echten Geräts mit Profil (13, 16).
- **Getriebespiel:** kleines Wackeln im Getriebe (16).
- **Home-Pose:** die Grundstellung der Ente (13).
- **IMU:** Lagesensor: Wo ist unten, wie schnell dreht sich der Körper? (13)
- **Kern extras:** Schalter für Kerns Zusätze an der Ente (13).
- **Micro Duck:** zweibeiniger Laufroboter von Pollen Robotics (13).
- **MJCF / MuJoCo:** Bauplan-Format / Physiksimulation, mit der Pollen trainiert (13).
- **Pick-up-Erkennung:** merkt, dass die Ente hochgehoben wird (13).
- **Regeltakt / Decimation:** wie oft das Netz gefragt wird / jeder wievielte Physikschritt (13).
- **Servo:** Motor, der auf einen Winkel fährt (13).
- **Servoprofil:** die Zahlen eines Servotyps als JSON (16).
- **Spielschicht:** Kerns Netz für Ball und Socke (13).
- **Tiefpass / EMA:** Werte glätten, damit nichts ruckt (13).
- **ToF:** Abstandssensor, der mit Lichtlaufzeit misst (13).
- **Walker:** Steuerart für Roboter, die ein Netz fährt (13).

### Lernen

- **Actor / Critic:** der Teil, der handelt / der Teil, der bewertet (14, A).
- **Adam:** ein Verfahren, das Gewichte geschickt anpasst (A).
- **Backpropagation:** rückwärts ausrechnen, welches Gewicht wie viel Schuld am Fehler hat (A).
- **Belohnung:** Punkte, die dem Netz sagen, was gut war (14).
- **CMA-ES:** Suchverfahren, das Kandidaten probiert und sich an die besten herantastet (14, 16).
- **DAgger:** Nachahmen: Das Netz fährt, das Vorbild sagt, was richtig wäre (14).
- **Domänen-Randomisierung:** Training mit zufälligen Abweichungen (16).
- **ELU:** die Aktivierung zwischen den Schichten: Positives bleibt, Negatives wird sanft auf über −1 gedrückt (A).
- **Episode:** ein Durchgang von Anfang bis Ende (14).
- **GAE:** Rechenweg, der schätzt, wie viel besser eine Aktion als erwartet war (A).
- **Gradientenabstieg:** in kleinen Schritten bergab zum kleinsten Fehler (A).
- **Lehrer:** Kerns Trainingssystem (14).
- **Lernrate:** wie große Schritte das Lernen macht (14).
- **Neuronales Netz / Neuron:** Rechennetz aus vielen einfachen Recheneinheiten (A).
- **Normalisieren:** Eingaben auf ähnliche Größe bringen (A).
- **ONNX:** Austauschformat für trainierte Netze (13).
- **Policy:** ein trainiertes Netz: Beobachtungen hinein, Aktionen heraus (13).
- **PPO:** Lernverfahren des Lehrers, Lernen durch Belohnung (14).
- **Reinforcement Learning:** Lernen durch Ausprobieren und Belohnung (A).
- **Rezept:** die Trainingseinstellungen einer Aufgabe (14).
- **Rollout:** gesammelte Erfahrung: beobachtet, getan, belohnt (14).
- **rsl_rl:** die Lernbibliothek, mit der Pollen trainiert (13).
- **Sammler / Lerner:** sammelt Erfahrung / lernt daraus (14).
- **Sim-to-Real:** von der Simulation zum echten Roboter (16).
- **Überanpassung:** auswendig lernen statt verstehen (A).
- **velstand:** Pollens Lauf- und Stehnetz, in Kern eingebaut (13).
- **Vorbild:** eine Vorführung, die das Netz zuerst nachahmt (14).

> **Mission:** Öffne **File › Example Levels › Physics Lab** und starte **Simulate**. Zähl, wie viele
> Glossar-Wörter du dabei siehst: Körper, Gelenk, Kontakt …

## Level geschafft!

- Du findest jede wichtige Taste im Studio in Sekunden.
- Du kennst die Befehle des Lehrers und weißt, welcher zuerst gilt.
- Du weißt, wie ein `.klevel`, ein Netz und ein Servoprofil aufgebaut sind.
- Du verstehst, wie JavaScript über die C-ABI mit der Engine spricht.
- Du hast ein Glossar mit allen Fachwörtern des Buchs.

## Rätsel-Ecke

1. Mit welcher Taste übernimmst du im Play die Ente?
2. Welche Zeile muss in jeder `.klevel`-Datei stehen?
3. Du startest `kern_lehrer --eval jump.onnx --kalibrieren a.csv`. Was passiert?
4. **Boss-Rätsel:** Eine Kern-Funktion gibt 0 zurück. Wie findest du heraus, was schiefging?

### Lösungen

1. **F8** (Possess). Noch einmal F8 verlässt sie wieder.
2. `"format": "kern-level"` (dazu `"version": 1`).
3. Es wird bewertet: `--eval` kommt in der Reihenfolge vor `--kalibrieren`.
4. 0 heißt Fehler. Die Meldung liefert `kern_last_error()`.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
