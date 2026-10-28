# 17 Zum Nachschlagen

> **Kurz gesagt:** Dieses Kapitel liest man nicht am Stück, man schlägt darin nach. Hier findest du
> die wichtigsten Tasten im Studio und bei der Micro Duck, die Befehle des Lehrer-Werkzeugs, die
> Dateiformate, einen Blick auf die Schnittstelle zwischen Engine und Browser und ein Glossar aller
> Fachwörter des Buchs. Die vollständige Referenz mit jeder Funktion und jeder Option steht in
> Kapitel 17 des blauen Buchs.

> **Stell dir vor …** dieses Kapitel ist der Zettel, der innen an der Werkstatttür hängt: Wo liegt
> welcher Schraubenschlüssel, welcher Schalter macht was, und was heißt eigentlich „Drehmoment“?
> Die Bedienungsanleitung jeder Maschine steht im Regal (im blauen Buch). Für den Alltag reicht
> meistens der Zettel.

## 17.1 Tasten im Studio

Die Steuerung von Kern Studio folgt der Unreal Engine. **F1** zeigt die wichtigsten Kürzel auch im
Studio selbst an. Alle Einzelheiten stehen in [Kapitel 11](11-studio.md).

### Kamera im Viewport

| Eingabe | Wirkung |
|---|---|
| Rechte Maustaste + W A S D | fliegen (Fly Mode) |
| … + Q / E | runter / hoch |
| … + Shift | 2,5-mal schneller |
| … + Mausrad | Fluggeschwindigkeit (Stufe 1–8) |
| Alt + linke Maustaste | um die Auswahl kreisen |
| Mittlere Maustaste ziehen | schwenken |
| F | Auswahl ins Bild holen |
| Ctrl+1 … 9 / 1 … 9 | Lesezeichen setzen / anspringen |

Am Telefon kreist ein Finger, zwei Finger zoomen und verschieben, zweimal Tippen holt die Auswahl ins
Bild, langes Halten öffnet das Kontextmenü.

### Bearbeiten

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Werkzeug Select / Move / Rotate / Scale |
| Leertaste | Move → Rotate → Scale reihum |
| [ / ] | Raster feiner / gröber |
| Entf | Auswahl löschen |
| End | Auswahl auf den Boden setzen |
| H / Ctrl+H | ausblenden / alles einblenden |
| G | Game View (Editor-Helfer aus) |
| F2 | umbenennen |
| Ctrl+Z / Ctrl+Y | rückgängig / wiederholen |
| Ctrl+C / X / V / D | kopieren / ausschneiden / einfügen / duplizieren |
| Ctrl+G | gruppieren |
| Ctrl+S / Ctrl+O / Ctrl+N | speichern / öffnen / neues Level |
| Ctrl+Space | Content Drawer |
| Alt+P / Alt+S | Play / Simulate |

> **Wusstest du?** Kopierte Actors landen als Text in der Zwischenablage deines Systems. Ein Ctrl+V
> in einem anderen Browser-Tab fügt sie dort ein – so ziehst du Dinge von einem Level ins andere.

### Play und Simulate

| Taste | Wirkung |
|---|---|
| Esc | Stop |
| F8 | Eject / Possess (Spieler verlassen / übernehmen) |
| Pause oder Alt+Shift+P | anhalten / weiter |
| . | einen Schritt weiter, wenn pausiert |
| Ctrl+R | Spiel neu starten |
| K | simulierte Posen ins Level übernehmen (Simulate) |

Im Spiel selbst gilt das Übliche: W A S D bewegen, Maus umsehen, Leertaste springen, Shift
sprinten, F benutzen. Im Logik-Editor kompiliert **F7** den Graphen, **F9** setzt einen
Breakpoint, **Ctrl+F** sucht.

> **Probier's aus!** Öffne auf https://kernengine.de/ das Level **Physics Lab**, drück Alt+S für
> Simulate, dann **Pause** (oder Alt+Shift+P) und tippe ein paarmal auf den Punkt. Du siehst die Physik Schritt für
> Schritt – wie ein Daumenkino, das du selbst umblätterst.

## 17.2 Tasten der Micro Duck

| Bewegung | Tastatur | Gamepad | Touch |
|---|---|---|---|
| Laufen / drehen | W S / A D | linker Stick | MOVE |
| Sitzen / Aufstehen | 1 | B | SIT |
| Aufheben (auf Rollen: Ducken) | 2 | Y | PICK |
| Kick links / rechts | 3 / 4 | – | KICK L / R |
| Rolle (halten: weiter) | 5 | X | ROLL |
| Schnabel auf / zu | 6 | – | BEAK |
| Sprung | 7 | – | HOP |
| Quaken / „Wheee“ | Leertaste / V | RT / LT | QUACK / WHEEE |

Jede dieser Tasten gibt dir für 2 Sekunden das Steuer; Gehirn und Spielschicht der Ente machen so
lange Pause ([Kapitel 13](13-micro-duck.md)).

> **Probier's aus!** Lade die Vorlage **Micro Duck**, drück Play und probier alle Tasten von 1 bis 7
> der Reihe nach. Was passiert, wenn du die 3 drückst, während die Ente sitzt?

## 17.3 Befehle des Lehrers (`kern_lehrer`)

Der Lehrer läuft im Browser, es gibt ihn aber auch als Programm für die Kommandozeile. Gebaut wird
es mit `kern/tools/lehrer.sh`, danach liegt es unter `kern/build/kern_lehrer`
([Kapitel 2](02-bauen-und-testen.md), [Kapitel 14](14-lehrer.md)).

Das Programm hat viele **Betriebsarten**. Welche gilt, entscheidet die erste passende Option auf der
Kommandozeile; ohne besondere Option wird trainiert. Die wichtigsten:

| Option | Was passiert |
|---|---|
| `--task jump` oder `stand` | trainieren (Standard: `jump`) |
| `--eval FILE` | einen Sprung bewerten |
| `--trace FILE` | eine Episode Takt für Takt zeigen |
| `--fingerprint N` | Determinismus-Fingerabdruck über N Takte |
| `--kalibrieren LISTE` | Servo an Prüfstand-Logs anpassen |
| `--abgleich FILE` | Roboter-Log gegen die Simulation |
| `--beispiele DIR` | Beispiel-Logs und Profil schreiben |
| `--vorbild-suchen FILE` | ein Sprung-Vorbild mit CMA-ES suchen |
| `--spielen` | Spiel-Netz durch Nachahmung lernen |
| `--spielen-ueben FILE` | Spiel-Netz mit PPO weiterüben |
| `--spielen-pruefen X` | Spiel-Netz oder Vorbild bewerten |
| `--biss-fenster` | messen, wo der Schnabel die Socke greift |
| `--halt-messen` | messen, wie weit die Ente nach „Stopp“ läuft |
| `--demo-spur` | das Micro-Duck-Level abspielen und mitschreiben |

Dazu kommen Stellschrauben: `--threads` und `--envs` (wie viele Rechenkerne und wie viele Enten
gleichzeitig), `--iterations` (wie lange), `--seed` (welcher Zufall), `--out` (wohin), `--profil`
(welches Servoprofil) und eine ganze Reihe für Fortgeschrittene – Lernrate, Netzgröße, Nachahmung,
Checkpoints. Alle stehen mit ihren Standardwerten im blauen Buch.

> **Achtung, Falle!** Das Ergebnis eines Trainings hängt von der Zahl der Enten ab (`--envs`), nicht
> von der Zahl der Rechenkerne. Wer auf einem anderen Rechner mit anderen `--threads` trainiert, aber
> dieselbe Entenzahl nimmt, bekommt dasselbe Netz. Aber Vorsicht: `--envs` wird auf ein Vielfaches
> der Threads abgerundet, und ohne Angabe sind es 16 Enten je Thread – dann ändert sich mit den
> Threads auch die Entenzahl.

Ein paar Beispiele aus dem Alltag:

```sh
kern/tools/lehrer.sh
kern/build/kern_lehrer --task jump --iterations 3000 \
    --out jump.onnx --log jump.csv --checkpoint run.ckpt
kern/build/kern_lehrer --eval jump.onnx --episodes 40
kern/build/kern_lehrer --kalibrieren steps.csv,chirp.csv \
    --pruefen rechteck.csv --out xl330_unit3.json
kern/build/kern_lehrer --fingerprint 400 \
    | diff - kern/tests/determinism_fingerprint.txt
```

Zeile für Zeile: Erst wird das Programm gebaut. Dann trainiert es einen Sprung über 3000
Iterationen, schreibt das Netz nach `jump.onnx`, ein Protokoll nach `jump.csv` und regelmäßig einen
Zwischenstand, von dem man später weitermachen kann. Die dritte Zeile bewertet das fertige Netz über
40 Episoden. Die vierte kalibriert einen Servo ([Kapitel 16](16-kalibrierung.md)). Die letzte
prüft, ob die Simulation auf diesem Rechner Bit für Bit dasselbe rechnet wie die Referenz: Gibt
`diff` nichts aus, ist alles in Ordnung.

## 17.4 Dateiformate

| Datei | Was drin ist | mehr in |
|---|---|---|
| `.klevel` | ein Level als JSON | [Kapitel 8](08-spiel-laufzeit.md), [11](11-studio.md) |
| `.onnx` | ein trainiertes Netz | [Kapitel 13](13-micro-duck.md), [14](14-lehrer.md) |
| Servoprofil (`.json`) | die Zahlen eines Servos | [Kapitel 16](16-kalibrierung.md) |
| Prüfstand-Log (`.csv`) | Zeit, Ziel, Winkel | [Kapitel 16](16-kalibrierung.md) |
| Roboter-Log (`.csv`) | 61 Beobachtungen + 14 Aktionen je Takt | [Kapitel 16](16-kalibrierung.md) |

### Levels (`.klevel`)

Ein Level ist eine JSON-Datei mit dem Kennzeichen `"format": "kern-level"` und `"version": 1`.
Darin stehen der Name, die World Settings, die Editor-Kamera, alle Actors mit ihren Komponenten,
eingebettete Texturen und Sounds und, falls vorhanden, der Logik-Graph des Levels.

Die Einheiten sind Meter, Sekunden und Kilogramm, Z zeigt nach oben, Drehungen stehen in Grad,
Farben als Zahlen von 0 bis 1. Eine Datei mit fremdem Format lehnt Kern ab („not a Kern level“),
ebenso eine aus einem neueren Studio („level was written by a newer Kern Studio“).

### Netze (`.onnx`)

Kern liest und schreibt Netze in genau der Form, in der Pollen Robotics sie für die Micro Duck
exportiert. In Worten: Messwerte hinein, normalisieren, einige Schichten mit ELU dazwischen, eine
lineare Schicht am Ende, Befehle heraus. Bewegungsnetze haben 61 Eingänge und 14 Ausgänge, das
Spiel-Netz `play.onnx` 23 Eingänge und 9 Ausgänge. Passt eine Datei nicht, sagt Kern, warum, etwa
„not an rsl_rl actor“ oder „unsupported ONNX operator“.

Beim Export schreibt Kern Zusatzinfos ins Netz: Gelenknamen, Grundhaltung, Aufgabe und den Vermerk,
dass Kerns Lehrer es trainiert hat. Pollens Netze liegen in `kern/third_party/microduck/`, Kerns
eigene (`jump.onnx`, `play.onnx`) in `kern/policies/`. Ein im Browser trainiertes Netz bleibt auf
deinem Gerät gespeichert und ersetzt dort das mitgelieferte. Der Pick-up-Detektor der Ente ist ein
Sonderfall mit anderer Form; er ist fest eingebaut.

### Servoprofil und Logs

Ein **Servoprofil** ist eine kleine JSON-Datei mit den Zahlen eines Servotyps: Steifigkeit,
Dämpfung, Reibung, Höchstmoment, Armatur, Verzögerung und Streuungen. Mindestens Steifigkeit und
Höchstmoment müssen drinstehen und positiv sein; fehlt etwas anderes, gilt 0.

Ein **Prüfstand-Log** beginnt mit Kommentarzeilen zur Last (`# mass`, `# arm`, `# inertia`,
`# horizontal`) und hat dann Zeilen mit Zeit, Zielwinkel und gemessenem Winkel. Ein **Roboter-Log**
hat eine Kopfzeile und dann je Regeltakt 75 Zahlen. Beispiele erzeugt `kern_lehrer --beispiele`.

## 17.5 Die Schnittstelle zum Browser (C-ABI)

Kern ist in C++ geschrieben und läuft im Browser als **WebAssembly**, einem Format, das Browser fast
so schnell wie echte Programme ausführen. Die Seite drumherum ist JavaScript. Die beiden sprechen
über eine flache Liste von Funktionen miteinander, die **C-ABI**.

> **Stell dir vor …** die Engine sitzt in einem Raum ohne Türen, nur mit einer Durchreiche wie in
> einer Kantine. JavaScript legt etwas hinein („zeichne dieses Bild“, „hier ist eine Taste“), die
> Engine legt etwas zurück. Was durch die Durchreiche passt, ist genau festgelegt – das ist die C-ABI.

Die Grundregeln, kurz:

- Jede Funktion heißt im Modul genauso wie im C++-Code, zum Beispiel `kern_render`.
- Die Seite startet das Modul einmal und ruft dann die Funktionen auf.
- Daten gehen durch einen gemeinsamen Speicher: Die Seite reserviert Platz (`kern_alloc`),
  schreibt hinein und übergibt die Adresse.
- **1 heißt Erfolg, 0 Fehler**; die Meldung holt man mit `kern_last_error()`.
- Texte gehen als UTF-8 hinein. Antworten wie JSON gehören dem Modul und gelten bis zum nächsten
  Aufruf derselben Gruppe.
- Umgekehrt braucht das Modul ein paar Dinge vom Browser: WebGL2 zum Zeichnen, die Uhrzeit,
  Logausgabe und einige Helfer. Dateien fasst die Engine nie an.

Die Funktionen sind in Gruppen sortiert:

| Gruppe | Wofür | mehr in |
|---|---|---|
| `kern_alloc`, `kern_last_error` … | Speicher und Fehler | – |
| `kern_renderer_*`, `kern_mesh_*` … | zeichnen, Kamera, Licht, Picking | [Kapitel 4](04-grafik.md) |
| `kern_fx_*` | Partikel | [Kapitel 4](04-grafik.md) |
| `kern_geo_*` | Gebäude, Straßen, Gelände | [Kapitel 5](05-szene-und-geometrie.md) |
| `kern_app_*` | eigene C++-Spiele | [Kapitel 12](12-eigene-spiele.md) |
| `kern_studio_*` | das Studio und exportierte Spiele | [Kapitel 11](11-studio.md) |
| `kern_lehrer_*` | Training im Browser | [Kapitel 14](14-lehrer.md) |
| `kern_logic_*` | kompilierte Logik-Graphen | [Kapitel 9](09-logik.md) |

Welche Gruppen ein Modul enthält, hängt davon ab, wie es gebaut wurde: Grafik, Partikel, Geometrie
und Logik sind immer dabei, App, Studio und Lehrer nur, wenn man sie einschaltet
([Kapitel 2](02-bauen-und-testen.md)). Jede einzelne Funktion mit ihren Parametern steht im blauen
Buch.

> **Wusstest du?** Die Webseite des Studios enthält keine Editorlogik. Menüs, Outliner, Details,
> Gizmos, Undo – all das ist C++ in der Engine. Die Seite holt sich nur über `kern_studio_state()`
> eine JSON-Beschreibung der Panels und zeichnet sie. Der Vorteil: Im Editor läuft genau derselbe
> Code wie im exportierten Spiel, und der ganze Editor lässt sich ohne Browser testen.

> **Probier's aus!** Öffne im Studio den Content Drawer mit Ctrl+Space und schau ins **Output Log**.
> Es zeigt die letzten 200 Meldungen mit Uhrzeit, Warnungen und Fehler farbig, beim Spielen auch
> Spielereignisse. Auch diese Liste kommt aus der Engine und ist gerade durch die Durchreiche
> gewandert.

## 17.6 Kleines Glossar

Die Fachwörter des ganzen Buchs, kurz erklärt. In Klammern steht, wo sie ausführlich vorkommen.

### Studio und Spiele

| Begriff | Bedeutung |
|---|---|
| **Actor** | ein Ding im Level mit Position und Bausteinen (Komponenten) ([Kap. 8](08-spiel-laufzeit.md)) |
| **Blueprint / Logik-Graph** | Programm aus Knoten und Verbindungen, ohne Tippen ([Kap. 9](09-logik.md)) |
| **Content Drawer** | Schublade des Studios mit Dingen zum Platzieren |
| **Details** | Panel mit allen Einstellungen des ausgewählten Actors |
| **Eject / Possess** | im Spiel den Spieler verlassen / wieder übernehmen |
| **Gizmo** | Pfeile und Ringe zum Verschieben, Drehen, Skalieren |
| **Level** | eine Spielwelt, gespeichert als `.klevel` |
| **Outliner** | Baumansicht aller Actors |
| **Package Game** | Level als eigenständige HTML-Datei exportieren |
| **Play / Simulate** | Spiel spielen / Physik laufen lassen, während du weiter bearbeitest |
| **Viewport** | das Fenster, durch das du in die 3D-Welt schaust |
| **World Settings** | Einstellungen der ganzen Welt, etwa die Schrittrate |

### Technik unter der Haube

| Begriff | Bedeutung |
|---|---|
| **Box3D** | die Physik-Engine von Erin Catto, die Kern unverändert mitbringt ([Kap. 6](06-physik.md)) |
| **C-ABI** | die Funktionsliste, über die JavaScript mit der Engine spricht |
| **Determinismus** | gleiche Eingaben ergeben Bit für Bit dasselbe Ergebnis |
| **Fehlerkanal** | Kern meldet Fehler über `fail()` / `last_error()` statt mit Ausnahmen |
| **GameWorld** | stellt ein Level dar und spielt es auf Box3D |
| **GTAO** | Verfahren, das Ecken und Ritzen realistisch abdunkelt ([Kap. 4](04-grafik.md)) |
| **JIT** | Logik-Graph wird während des Spiels in WebAssembly übersetzt |
| **Mesh** | 3D-Form aus Dreiecken |
| **NetGame** | Mehrspieler: ein Server bestimmt, alle anderen bekommen Schnappschüsse ([Kap. 10](10-netzwerk.md)) |
| **ShaderExtension** | Zusatz für den Standard-Shader, mit dem ein Spiel eigene Looks bekommt |
| **Substeps** | Unterschritte der Physik pro Weltschritt; für die Ente 16 |
| **WebAssembly** | Programmformat, das Browser sehr schnell ausführen |
| **Welding** | doppelte Eckpunkte eines Meshes verlustfrei zusammenfassen |

### Roboter und Lernen

| Begriff | Bedeutung |
|---|---|
| **Actor / Critic** | beim Training: der Teil, der handelt / der Teil, der bewertet |
| **Armatur** | zusätzliche Trägheit durch den schnell drehenden Motor im Servo |
| **CMA-ES** | Suchverfahren, das Kandidaten probiert und sich an die besten herantastet |
| **DAgger** | Nachahmen: Das Netz fährt, das Vorbild sagt, was richtig gewesen wäre |
| **Domänen-Randomisierung** | Training mit zufälligen Abweichungen, damit das Netz robust wird |
| **Gerätemodell** | kleines Modell eines echten Geräts (Servo, Detektor) mit Profil |
| **IMU** | Lagesensor: wo ist unten, wie schnell dreht sich der Körper |
| **Kern extras** | Schalter für Kerns eigene Zusätze an der Ente, z. B. die Spielschicht |
| **Lehrer** | Kerns Trainingssystem für die Netze der Micro Duck ([Kap. 14](14-lehrer.md)) |
| **Micro Duck** | zweibeiniger Laufroboter von Pollen Robotics ([Kap. 13](13-micro-duck.md)) |
| **ONNX** | Austauschformat für trainierte Netze |
| **Policy** | ein trainiertes Netz: Beobachtungen hinein, Aktionen heraus |
| **PPO** | das Lernverfahren des Lehrers (Lernen durch Belohnung) |
| **Rezept** | die Trainingseinstellungen einer Aufgabe |
| **Rollout** | gesammelte Erfahrung: was beobachtet, getan und belohnt wurde |
| **Servo** | Motor, der auf einen vorgegebenen Winkel fährt |
| **Servoprofil** | die Zahlen eines Servotyps als JSON ([Kap. 16](16-kalibrierung.md)) |
| **Spielschicht** | Kerns Netz, das entscheidet, was die Ente mit Ball und Socke tut |
| **ToF** | Abstandssensor, der mit Licht misst (Time of Flight) |
| **velstand** | Pollens Lauf- und Stehnetz, in Kern fest eingebaut |
| **Vorbild** | Vorführung, die ein Netz zuerst nachahmt |
| **Walker** | Steuerart für gelenkige Roboter, die ein Netz fährt |

Das vollständige Glossar mit allen Fachbegriffen steht im blauen Buch.

## Das nimmst du mit

- **F1** im Studio zeigt die wichtigsten Kürzel; Alt+P spielt, Alt+S simuliert, Esc stoppt.
- Die Ente steuerst du mit W A S D und den Tasten 1 bis 7.
- `kern_lehrer` trainiert, bewertet, kalibriert und misst – die erste passende Option entscheidet.
- Levels sind `.klevel`-Dateien in JSON, Netze `.onnx`, Servoprofile JSON, Logs CSV.
- Engine und Browser reden über die C-ABI: 1 heißt Erfolg, 0 Fehler.
- Für jede einzelne Funktion und Option schlägst du im blauen Buch nach.

## Knobelecke

1. Du hast eine Kiste im Viewport ausgewählt, sie ist aber nicht zu sehen. Welche Taste hilft?
2. Eine Funktion der C-ABI gibt 0 zurück. Wie erfährst du, was schiefging?
3. Du willst prüfen, ob Kern auf deinem neuen Laptop genau dasselbe rechnet wie auf dem alten.
   Welche Option von `kern_lehrer` nimmst du?
4. Was ist der Unterschied zwischen Play und Simulate?

### Lösungen

1. **F** holt die Auswahl ins Bild (ein Doppelklick in den Viewport auch).
2. Mit `kern_last_error()`, die die letzte Fehlermeldung liefert.
3. `--fingerprint 400` und das Ergebnis mit `kern/tests/determinism_fingerprint.txt` vergleichen.
4. Play spielt das Level als Spieler. Simulate lässt Physik und Regeln laufen, während der Editor
   bedienbar bleibt: Du kannst Körper greifen und Posen mit K übernehmen.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
