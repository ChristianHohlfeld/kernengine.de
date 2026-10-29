# 17 Spickzettel: Tasten, Dateien, Begriffe

> **Dein Level:** Das letzte Level ist dein Nachschlagewerk. Hier findest du schnell die wichtigsten
> Tasten in Kern Studio, die Dateien, mit denen Kern arbeitet, und ein großes Glossar mit allen
> Fachwörtern des Buchs, von „Actor“ bis „Warmstart“. Vergessen ist erlaubt – nachschlagen auch.

## 17.1 Tasten im Studio

Tastenkürzel sind Abkürzungen: Statt dich durch Menüs zu klicken, drückst du eine Taste. Kern
Studio übernimmt die Belegung der Unreal Engine, eines großen Profi-Werkzeugs für Spiele. Wer eins
kennt, findet sich im anderen zurecht. **F1** zeigt die wichtigsten Kürzel auch im Studio.

### Kamera im Viewport

Der **Viewport** ist das Fenster, durch das du in die 3D-Welt schaust.

| Eingabe | Wirkung |
|---|---|
| rechte Maustaste + W A S D | fliegen |
| … + Q / E | runter / hoch |
| … + Umschalt | 2,5-mal schneller |
| … + Mausrad | Fluggeschwindigkeit 1–8 |
| Alt + linke Maustaste | um die Auswahl kreisen |
| mittlere Maustaste | schwenken |
| F | Auswahl einrahmen |
| Strg+1 … 9 / 1 … 9 | Lesezeichen setzen / anspringen |

Am Telefon: ein Finger kreist, zwei Finger zoomen und verschieben, Tippen wählt aus.

### Bearbeiten

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Auswählen / Verschieben / Drehen / Skalieren |
| [ / ] | Raster kleiner / größer |
| Entf | Auswahl löschen |
| Ende | Auswahl auf den Boden setzen |
| H / Strg+H | ausblenden / alles einblenden |
| G | **Spielansicht** (Game View): Ansicht wie im Spiel |
| F2 | umbenennen |
| Strg+Z / Strg+Y | rückgängig / wiederholen |
| Strg+C / V / D | kopieren / einfügen / duplizieren |
| Strg+G | gruppieren |
| Strg+S / Strg+O / Strg+N | speichern / öffnen / neues Level |
| Strg+Leertaste | **Inhaltsablage** (Content Drawer) |
| Alt+2 | nur **Drahtgitter** (Wireframe) |
| Alt+3 | Farben ohne Licht: **Unbeleuchtet** (Unlit) |
| Alt+4 | normal mit Licht: **Beleuchtet** (Lit) |
| Alt+5 | **Spieler-Kollision** (Player Collision): alle Kollisionshüllen über dem abgedunkelten Bild |
| Alt+P / Alt+S | **Spielen** (Play) / **Simulieren** (Simulate) |

### Spielen und Simulieren

Beim **Spielen** spielst du selbst. Beim **Simulieren** läuft die Welt ohne Spielfigur: Du schaust mit
der Editor-Kamera zu, greifst Körper und übernimmst Lagen mit K – bearbeiten kannst du das Level erst
nach dem Stoppen.

| Taste | Wirkung |
|---|---|
| Esc | **Stopp** (Stop) |
| F8 | Spieler verlassen / übernehmen |
| Pause | anhalten / weiter |
| . | pausiert: einen Schritt weiter |
| Strg+R | Spiel neu starten (nicht beim Soldaten) |
| K | beim Simulieren: **Simulationsänderungen übernehmen** (Keep Simulation Changes), also die Lage der Dinge ins Level übernehmen |
| Umschalt+F1 | Maus freigeben |

Die Taste **K** ist praktisch: Lass beim Simulieren einen Stapel Kisten umfallen, wähl sie aus und
drück K. Dann liegen sie auch nach dem Stoppen so da, wie die Physik sie hingelegt hat.

Im Spiel: W A S D bewegen, Maus umsehen, Leertaste springen, Umschalt Boost, F benutzen.

### Logik-Editor

- **F7** kompiliert: Der Logik-Graph wird in schnellen Maschinencode übersetzt.
- **F9** setzt einen **Haltepunkt**: Das Spiel hält an, wenn dieser Knoten drankommt. So kannst du
  nachsehen, was gerade passiert.
- **Strg+F** sucht, **C** legt einen Kommentar um die Auswahl, **Esc** führt zurück zum Viewport.

> **Mission:** Öffne Kern Studio auf https://kernengine.de/ und drück **F1** – das ist
> **Hilfe › Steuerung...** (Help › Controls...). Vergleiche das Fenster mit diesen Tabellen. Probier
> dann Strg+1 auf einer schönen Kameraposition, flieg
> weg und drück **1** – du springst zurück.

## 17.2 Tasten der Micro Duck

W/S laufen, A/D drehen, **1** sitzen, **2** aufheben, **3** und **4** kicken, **5** rollen,
**6** Schnabel, **7** springen, **Leertaste** quaken, **V** „Wheee“. Gamepad und Touch stehen in
[Kapitel 13](13-micro-duck.md).

## 17.3 Der Lehrer: Enten ändern das Netz, Threads nur das Tempo

Wenn du selbst ein Netz trainierst, gibt es eine Regel, die du kennen solltest: **Die Zahl der Enten
bestimmt, welches Netz herauskommt. Die Zahl der Rechenhelfer bestimmt nur, wie schnell es geht.**

Ein **Thread** ist ein Rechenstrang: Ein Prozessor mit mehreren Kernen kann mehrere Threads
gleichzeitig rechnen. Im Browser heißen die Helfer **Web Worker**. Jeder Helfer übernimmt eine
Gruppe Enten und sammelt deren Erfahrung.

> **So kannst du es dir vorstellen:** Eine Klasse mit 32 Schülern füllt einen Fragebogen aus. Ob zwei
> oder vier Lehrkräfte die Bögen einsammeln, ist egal, solange danach alle Bögen nach Schülernummer
> sortiert und in derselben Reihenfolge zusammengezählt werden. Das Ergebnis ist dasselbe, nur das
> Einsammeln geht schneller. Fragt man aber 64 Schüler, kommen andere Antworten dazu, und das
> Ergebnis ändert sich.

Genau so macht es Kern:

- Jede Ente bekommt ihren eigenen Zufall aus ihrer **Nummer**, nicht aus dem Helfer, der sie rechnet.
- Bevor gelernt wird, sortiert Kern alle gesammelten Daten in eine **feste Reihenfolge**.
- Zusammengezählt wird immer in denselben festen Stücken, egal wie viele Helfer rechnen.

**Beispiel:** 32 Enten auf 2 Threads und 32 Enten auf 4 Threads ergeben genau dasselbe Netz, Bit für
Bit. Der zweite Lauf ist nur schneller. 64 Enten ergeben ein anderes Netz.

> **Mission:** Öffne den Lehrer auf https://kernengine.de/, wähle „Stehen (Schnelltest)“, Seed 1,
> **2 Worker** mit je **4 Enten** und trage bei „Iterationen“ **20** ein. Drücke Start, warte, bis
> die Seite „Fertig“ meldet, und schreib die Zahl bei „mittlere Episoden-Rückgabe“ auf. Lade neu und
> starte genauso, nur mit **1 Worker** und **8 Enten** (wieder 20 Iterationen). Beide Male sind es
> 8 Enten insgesamt – die Zahlen stimmen genau überein. Probier zum Vergleich 2 Worker mit je 8 Enten:
> Jetzt sind es 16 Enten, und die Zahl ist anders. Vergleiche immer erst bei „Fertig“: Mitten im Lauf
> stehen die beiden Versuche vielleicht gerade bei verschiedenen Iterationen.

Für junge Entdecker ist der Lehrer im Browser der einfachste Weg. Den Lehrer gibt es auch als
Programm für das **Terminal** (ein Fenster, in das man Befehle tippt). Alle seine Befehle und
Einstellungen stehen im blauen Buch, Kapitel 17.

## 17.4 Dateien

### Levels

Ein Level wird als **Textdatei** mit der Endung `.klevel` gespeichert. Darin steht alles über deine
Welt: Name, Einstellungen, Kamera, alle Actors, eingebettete Bilder und Klänge und die Logik. Das
Format heißt **JSON**: Namen und Werte, ordentlich in Klammern verpackt. Der Anfang sieht etwa so aus:

```
{
  "format": "kern-level",
  "version": 1,
  "name": "Mein Level",
  "actors": [ … ]
}
```

- Zeile 1 und 6: Die geschweiften Klammern umschließen das ganze Level.
- Zeile 2: Daran erkennt Kern, dass es ein Level ist. Fehlt sie, lehnt Kern die Datei ab.
- Zeile 3: die Version des Formats. Eine Datei aus einem neueren Studio lehnt Kern ab.
- Zeile 4: der Name des Levels.
- Zeile 5: In den eckigen Klammern steht die Liste aller Actors.

Kern rechnet in Metern, Sekunden und Kilogramm. **Z zeigt nach oben**: X und Y liegen flach auf dem
Boden wie die Achsen auf Karopapier, Z ragt senkrecht in die Höhe. Winkel stehen in der Datei in
Grad (intern, etwa bei den Gelenken der Ente, rechnet Kern in Radiant, [Kapitel 13](13-micro-duck.md)),
Farben als Zahlen von 0 bis 1.

> **Mission:** Speichere ein Level mit **Strg+S**. Je nach Browser fragt Studio nach einem
> Speicherort, oder die Datei landet in deinem Download-Ordner. Öffne die `.klevel`-Datei mit einem
> Texteditor (Editor unter Windows, TextEdit am Mac). Findest du `"kern-level"` und die Namen deiner
> Actors?

### Netze

Trainierte Netze werden als **ONNX**-Datei gespeichert. ONNX ist für Netze, was PDF für Texte ist: ein
Format, das viele Programme lesen können, auf dem Roboter, im Browser und auf dem Server. Kern liest
Pollens Netze und schreibt seine eigenen im selben Format. Der Umrechner, der die Messwerte auf
eine Skala bringt ([Kapitel 14](14-lehrer.md)), steckt mit in der Datei – das Netz kommt also mit
seinem eigenen Maßband. Netze, die du im Browser trainierst, bleiben auf deinem Gerät und ersetzen
dort die mitgelieferten.

### Servoprofil

Ein **Servoprofil** ist eine kleine Textdatei mit den Zahlen eines Servos: wie stark seine Feder
zieht, wie stark er bremst, wie viel Kraft er höchstens hat und wie unsicher jede Zahl ist
([Kapitel 16](16-kalibrierung.md)).

### Logs

Messungen werden als **CSV**-Datei gespeichert: eine Tabelle als Text, die Werte durch Kommas
getrennt. Sie öffnet sich auch in einer Tabellenkalkulation. Ein Prüfstand-Log enthält Zeit,
Zielwinkel und gemessenen Winkel eines Servos. Ein Roboter-Log enthält 50-mal pro Sekunde, was die
Sensoren der Ente melden und welche Befehle ihr Netz gibt.

Die genauen Felder aller Dateien stehen im blauen Buch, Kapitel 17.

## 17.5 Für Entwickler: das blaue Buch

Wer eigene Programme mit Kern schreiben will, braucht die Liste aller Funktionen, über die der
Browser mit der Engine spricht, und alle Befehle des Lehrers. Das steht vollständig im blauen Buch,
Kapitel 17. Zwei Prinzipien daraus lohnen sich auch für dich:

**Die Engine fasst nie selbst Dateien an.** Alles, was sie braucht – Levels, Bilder, Netze –, reicht
ihr ihr Gastgeber herein: im Studio der Browser, in den Tests und im Lehrer-Programm das jeweilige
Programm. Darum läuft dieselbe Engine überall unverändert; nur der Gastgeber, der ihr die Daten
reicht, ist ein anderer.

> **So kannst du es dir vorstellen:** ein Koch, der nie selbst einkaufen geht. Er kocht nur, was ihm
> durch die Durchreiche gegeben wird. Darum kann er in jeder Küche arbeiten, ob im Restaurant oder
> auf einem Schiff.

**Fehler werden gemeldet, nicht verschluckt.** Geht etwas schief, etwa weil eine Datei kaputt ist,
stürzt die Engine nicht einfach ab. Sie sagt „hat nicht geklappt“ und hinterlegt eine Meldung, was
falsch war. So erfährst du, woran es lag.

## 17.6 Glossar

Alle Fachwörter des Buchs, kurz erklärt. Die Zahl in Klammern nennt das Kapitel, in dem es
ausführlich vorkommt (A = Anhang A).

### Studio und Spiele

Bedienelemente von Studio stehen hier mit ihrer deutschen Beschriftung, in Klammern die englische.

- **Actor:** ein Ding im Level, mit Position und Bausteinen (8, 11). Nicht verwechseln mit dem
  **Actor beim Lernen** (siehe Lernen).
- **Aussteigen / übernehmen (F8)** (Eject / possess (F8)): im Spiel den Spieler verlassen / wieder
  übernehmen (11).
- **Ausgabeprotokoll** (Output Log): das Logbuch mit allen Meldungen von Studio; viele Meldungen
  darin bleiben englisch (11).
- **Autosave:** automatische Sicherung ungespeicherter Arbeit, nur in diesem Browser (11).
- **Beobachterkamera (Spielen: fliegen, zuschauen, Körper ziehen)** (Observer camera (Play: fly,
  watch, drag bodies)): Spielen mit freier Kamera zum Zuschauen (11).
- **Beweger** (Mover) / **Rotator:** Komponenten, die ein Ding hin und her fahren / sich drehen
  lassen (8, 11). Nicht verwechseln mit dem **Character Mover** (siehe Physik).
- **Blueprint / Logik-Graph:** ein Programm aus Knoten und Verbindungen, ohne Tippen (9).
- **Box / Quader:** dieselbe Form; Studio nennt sie Box, die Geometrie Quader (5, 8).
- **Controller:** bestimmt, wie der Spieler sich bewegt: laufen, rollen, fahren, fliegen (8).
- **Details:** das Panel mit allen Einstellungen des gewählten Actors (11).
- **Dock:** die Knöpfe zum Spielen und Simulieren, die auf schmalen Bildschirmen immer sichtbar
  bleiben (11).
- **dt:** die Zeit seit dem letzten Bild in Sekunden; Bewegung wird damit malgenommen (12).
- **Engine:** das Programm, das eine Spielwelt zeichnet, rechnet und abspielt (1).
- **Event:** ein Ereignis, das im Logik-Graphen etwas in Gang setzt (9).
- **Gizmo:** Pfeile und Ringe zum Verschieben, Drehen, Skalieren (11).
- **Haltepunkt:** Das Spiel hält an, wenn ein bestimmter Knoten drankommt (9).
- **HUD:** Anzeigen im Spiel wie Punkte und Zeit (8).
- **Inhaltsablage** (Content Drawer): die Schublade mit Dingen zum Platzieren (11).
- **Kill-Z (darunter Wiedereinstieg)** (Kill Z (respawn below)): eine Höhe; fällt der Spieler
  darunter, wird er zurückgesetzt (11).
- **Knoten:** ein Baustein im Logik-Graphen (9).
- **Komponente:** ein Baustein eines Actors, etwa Mesh, Kollider oder Sound (8, 11).
- **Konsole** (Feld **Cmd**): Zeile in Studios Statusleiste, in die man Befehle tippt (11).
- **Level:** eine Spielwelt, gespeichert als `.klevel` (8).
- **Listen-Server:** ein Server, der selbst mitspielt (10).
- **Logik-Editor:** Studios Werkzeug für Logik-Graphen; seine Beschriftungen bleiben englisch (9, 11).
- **Modul:** die übersetzte Datei der Engine oder eines Spiels, die der Browser lädt (2, 12).
- **Outliner:** die Liste aller Actors rechts oben, wie ein Inhaltsverzeichnis des Levels (11).
- **Pawn:** die Spielfigur, die einem Spieler gehört (8, 10).
- **Pin / Variable:** Anschluss am Rand eines Knotens / beschriftetes Fach, in dem sich ein Graph
  einen Wert merkt (9).
- **Plattformschicht:** kleiner JavaScript-Helfer zwischen C++-Spiel und Browser (12).
- **Snapping:** Einrasten auf ein Raster (11).
- **Spiel verpacken** (Package Game): ein Level als eigenständige Webseiten-Datei exportieren, die
  man weitergeben kann (11).
- **Spielansicht** (Game View): Ansicht ohne Editor-Hilfen, wie im Spiel (11).
- **Spielen** (Play) / **Simulieren** (Simulate): selbst spielen / die Welt ohne Spielfigur laufen
  lassen und zuschauen; bearbeiten geht erst nach dem Stoppen (11).
- **Spielschleife:** Eingabe lesen, Welt weiterrechnen, Bild malen – für jedes Bild neu (12).
- **Starrkörper** (Rigid Body): die Komponente, die ein Ding zum Physik-Körper macht (11).
- **Transformation** (Transform): Position, Drehung und Skalierung eines Actors (8).
- **Trigger:** ein Bereich, der etwas auslöst, wenn man ihn betritt (8, 12).
- **Viewport:** das Fenster, durch das du in die 3D-Welt schaust (11).
- **Webserver:** ein Programm, das Dateien so ausliefert wie eine Internetseite (12).
- **Welteinstellungen** (World Settings): Einstellungen der ganzen Welt, etwa Schwerkraft und
  Schrittrate (6, 11).

### Grafik und Geometrie

- **Albedo:** wie viel Licht eine Farbe zurückwirft: weiß 1, schwarz 0 (B).
- **Bildpuffer:** unsichtbares Bild, in das die Grafikkarte malt, bevor es auf den Bildschirm kommt (B).
- **Bloom (Glühen):** helle Stellen leuchten über ihren Rand hinaus (4, B).
- **BVH:** Kisten in Kisten. Große Kisten enthalten kleinere, und ganz innen liegen die Dreiecke. Ein
  Strahl muss nur in die Kisten schauen, die er trifft – das spart viel Suchen (5, B).
- **Ecke** (Vertex): ein Eckpunkt eines Dreiecks, mit Lage und oft Farbe, Normale und UV (5, B).
- **Ego-Ebene:** wird nach dem Löschen der Tiefe als Letztes gezeichnet und liegt darum immer vorne,
  etwa für die Waffe in der Hand oder die Gizmos (4, B).
- **Fragment:** ein Pixel-Kandidat, für den die Grafikkarte die Farbe ausrechnet (4, B).
- **Frame:** ein einzelnes Bild; viele pro Sekunde ergeben Bewegung (4, 12).
- **Fresnel:** Flach draufschauen spiegelt mehr – wie bei einer nassen Straße (B).
- **Frustum-Culling:** nicht zeichnen, was außerhalb der Sichtpyramide liegt (5, B).
- **Gamma / sRGB:** Umrechnung zwischen „so viel Licht“ und „so, wie das Auge es sieht“; dunkle Töne
  bekommen mehr Stufen (4, B).
- **Grafikkarte** (GPU): der Chip, der Bilder malt und tausende kleine Aufgaben gleichzeitig
  rechnet (4, B).
- **Indizes:** Nummernliste, welche drei Ecken ein Dreieck bilden (5).
- **Instanz / Instancing:** dasselbe Mesh noch einmal an einem anderen Ort / alle Kopien mit einem
  einzigen Malbefehl zeichnen (4, 5, B).
- **Kontextverlust:** Der Browser nimmt die Grafikkarte kurz weg; Kern baut alles neu auf (4, B).
- **Malbefehl:** ein Auftrag an die Grafikkarte: „Mal dieses Mesh!“ (4, B).
- **Material:** wie eine Oberfläche aussieht: Farbe, Rauheit, Metall (4).
- **Mesh:** eine 3D-Form aus Dreiecken (4).
- **Normale:** ein Pfeil, der senkrecht aus einer Fläche zeigt; mit ihm rechnet das Licht (5, B).
- **Partikel:** viele kleine Punkte für Funken, Rauch, Staub (4).
- **Pass:** ein Durchgang beim Zeichnen, etwa für Schatten (4).
- **PBR:** „physikalisch begründetes Malen“ mit winzigen Spiegelchen (Mikrofacetten) (B).
- **Picking:** herausfinden, was unter dem Mauszeiger liegt (5).
- **Rasterisierung:** prüfen, welche Pixelmitten in einem Dreieck liegen (B).
- **Renderer:** der Teil, der die Bilder zeichnet (3, 4).
- **Schattenkarte:** ein Bild aus Sicht der Sonne, das sich die Entfernung zur ersten Fläche
  merkt (4, B).
- **Shader:** kleines Programm auf der Grafikkarte, das Punkte und Farben berechnet (4, B).
- **Skalarprodukt:** eine Zahl dafür, wie sehr zwei Pfeile in dieselbe Richtung zeigen (B).
- **Skinning:** ein Mesh mit Knochen verformen, etwa für Figuren (4, B).
- **Textur / Texel:** ein Bild auf einer Oberfläche / eines seiner Pixel (4).
- **Tiefenpuffer:** unsichtbares Bild, in dem für jedes Pixel steht, wie weit das bisher vorderste
  Ding weg ist (4, B).
- **Tonemapping:** sehr helles Licht am Ende sanft auf den Bereich des Bildschirms drücken (4, B).
- **Umgebungsverdeckung:** dunkelt Ecken und Ritzen ab, in die wenig Himmel scheint (4, B).
- **Umlaufsinn:** die Reihenfolge der Ecken verrät, ob man ein Dreieck von vorn oder hinten sieht (4, B).
- **UV:** zwei Zahlen pro Ecke, welche Stelle einer Textur dort aufgeklebt wird (5).
- **WebGL2:** der Weg, über den der Browser die Grafikkarte benutzt (4, B).
- **Welding:** doppelte Eckpunkte zu einem zusammenfassen (5).

### Mathe und Technik

- **Bibliothek:** eine Sammlung fertiger Funktionen, die viele Programme mitbenutzen (3).
- **Bit:** die kleinste Einheit im Computer: 0 oder 1. „Bit für Bit gleich“ heißt: ganz exakt
  gleich (3, 10).
- **Browser-Konsole:** verstecktes Fenster im Browser für Meldungen von Webseiten, oft mit F12 (3, 15).
- **C++ / JavaScript:** die Programmiersprache der Engine / die Sprache der Webseiten (1, 4).
- **CI:** automatische Prüfung jeder Änderung auf einem Server (2).
- **Compiler / kompilieren:** ein Programm, das Code in Maschinensprache übersetzt, die der Prozessor
  direkt ausführt (2, 12). Im Logik-Editor heißt der Knopf dafür **Compile** (11).
- **CSV:** eine Tabelle als Textdatei, die Werte durch Kommas getrennt (17.4).
- **Determinismus / deterministisch:** gleiche Eingaben ergeben Bit für Bit dasselbe Ergebnis, auf
  jedem Gerät (6, 7, 9, 14).
- **Fingerabdruck:** eine Prüfzahl, die jede kleinste Abweichung verrät – je Takt für die Simulation
  (2, 14), für einen Logik-Graphen sein Inhalt (9).
- **Funktion:** ein kleines Teilprogramm mit Namen, das Werte bekommt und ein Ergebnis liefert (3).
- **GitHub:** die Webseite, auf der der Quelltext von Kern liegt (2).
- **Hertz (Hz):** wie oft etwas pro Sekunde passiert. 50 Hz heißt 50-mal pro Sekunde (6, 7, 16).
- **JSON:** ein lesbares Textformat für Daten: Namen und Werte in Klammern (3, 17.4).
- **Matrix:** eine Zahlentabelle, die Drehen, Verschieben und Skalieren auf einmal beschreibt (3, B).
- **Prozessor / Kerne:** das Rechenwerk im Computer / seine Teile, die gleichzeitig rechnen
  können – mit der Engine Kern haben sie nichts zu tun (2, 14).
- **Quaternion:** vier Zahlen, die eine Drehung beschreiben (3, B).
- **Quelltext:** der Text eines Programms, so wie Menschen ihn schreiben (2).
- **Radiant (rad):** ein anderes Winkelmaß als Grad. 1 rad sind etwa 57°, ein ganzer Kreis etwa
  6,28 rad (6, 13).
- **Seed:** die Startzahl für den Zufall im Computer. Gleicher Seed, gleiche Zufallszahlen – so lässt
  sich jeder Lauf genau wiederholen (10, 14).
- **Terminal / Kommandozeile:** ein Fenster, in das man Befehle tippt, statt zu klicken (15, 16, 17.3).
- **Thread:** ein Rechenstrang. Ein Prozessor mit mehreren Kernen rechnet mehrere Threads
  gleichzeitig (17.3, A).
- **Vektor (Pfeil):** ein Pfeil mit Richtung und Länge, etwa (x, y, z) (3, B).
- **Vorlesen / Übersetzen (JIT):** ein Logik-Graph wird Knoten für Knoten ausgeführt / während des
  Spiels in schnellen Code übersetzt (9).
- **WebAssembly:** ein Programmformat, das Browser sehr schnell ausführen (1, B).
- **Web Worker:** ein Rechenhelfer im Browser, der neben der Webseite herläuft; auf der Lehrer-Seite
  heißen sie „Worker“ (14).

### Physik

- **Box3D:** die Physik-Engine von Erin Catto, die Kern unverändert mitbringt (6).
- **Character Mover:** Box3Ds Helfer für Spielfiguren: Die Figur schwebt wie auf einer Pogo-Feder (6).
- **Dämpfung:** die Bremse beim Schwingen, wie ein Stoßdämpfer (6, 13).
- **Drehmoment:** Kraft mal Hebel, gemessen in Newtonmeter (N·m) – wie am Schraubenschlüssel
  (6, 13, C).
- **Gelenk:** verbindet zwei Körper, etwa als Scharnier (6).
- **Impuls:** ein kurzer Stoß, der das Tempo auf einen Schlag ändert (6, C).
- **Körper / Form:** was sich bewegt / welche Gestalt es hat (6).
- **Kollider:** die unsichtbare Form, mit der etwas zusammenstößt (6, 8).
- **Kontakt:** eine Stelle, an der sich zwei Körper berühren (6).
- **Konvexe Hülle:** eine Form ohne Dellen nach innen (13).
- **Newton (N):** die Einheit der Kraft; 100 g drücken mit knapp 1 N (6, C).
- **Ray-Cast (Strahltest):** Was trifft ein Strahl zuerst? (5, 6, 13)
- **Reibung / Restitution / Rollwiderstand:** Haften / Abprallen / Bremsen beim Rollen (6, 13).
- **Schrittrate:** wie viele feste Schritte die Physik pro Sekunde rechnet; Standard 60, bei der
  Micro Duck 100 pro Sekunde Weltzeit (6, 13).
- **Sensor:** ein Kollider, der nichts aufhält, sondern nur meldet, wenn etwas hineinkommt (8).
- **Solver:** der Teil der Physik, der alle Kontakte und Gelenke gleichzeitig einzuhalten versucht (C).
- **Starrer Körper:** ein Ding, das sich nie verbiegt, wie ein Holzklotz (6).
- **statisch / kinematisch / dynamisch:** steht fest / folgt einer Vorgabe und lässt sich nicht
  schubsen / echte Physik mit Fallen und Stoßen (6, 8).
- **Sub-Steps (Teilschritte):** Die Physik teilt jeden Schritt in kleinere Stücke, damit sie genauer
  rechnet; bei der Micro Duck 16 je Schritt (6, 13, C).
- **Trägheit / Trägheitsmoment:** wie schwer sich etwas in Bewegung / in Drehung versetzen lässt (6, 13).
- **Zeitschritt (fester Schritt):** die feste Zeit, um die die Physik jedes Mal weiterrechnet (6, 8).

### Klang und Netzwerk

- **3D-Klang:** Klang, der von einer Stelle im Raum kommt (7).
- **Clip / Schleife:** ein fertiges Tonstück / ein Ton, der sich endlos wiederholt (7).
- **Filter / Tiefpass:** ein Sieb für Töne; ein Tiefpass lässt nur das Tiefe durch und glättet
  damit auch Werte, damit nichts ruckt (7, 13).
- **Frequenz:** wie oft ein Ton pro Sekunde zittert; hoch = hoher Ton (7).
- **Hall:** der Nachklang eines Raums (7).
- **Mixer:** mischt alle Klänge zu einem (7).
- **Mono / Stereo:** ein Tonkanal / zwei Kanäle, links und rechts (7).
- **Pan-Wert:** wie weit links (−1) oder rechts (+1) ein Ton klingt (7).
- **Synthesizer:** erzeugt Klänge aus Rechnung statt aus Aufnahmen (7, 13).
- **Autoritativ:** Nur der Server entscheidet, was wirklich passiert – wie ein Schiedsrichter (10).
- **Client / Server:** Mitspieler / der Rechner, der das Spiel bestimmt (8, 10).
- **Datagramm:** ein einzelnes Datenpäckchen im Netz, ohne Garantie (10).
- **Einschreiben / Postkarte:** die zwei Kanäle: kommt sicher und in Reihenfolge an / einmal
  geschickt, verloren ist verloren (10).
- **Interpolation:** zwischen zwei bekannten Fotos weich überblenden; dafür zeigt der Client die Welt
  0,1 s in der Vergangenheit (10).
- **Jitter:** wie stark die Verzögerung im Netz schwankt (10).
- **KernNet:** Studios kleiner Bote, der Päckchen zwischen Tabs und Rechnern trägt (10).
- **Lag-Kompensation:** Der Server dreht für die Treffer-Prüfung die Zeit zurück (8, 10).
- **Ping:** wie lange eine Nachricht zum anderen und zurück braucht (10).
- **Prioritätstopf:** Punktekonto je Ding: Bewegtes kommt oft in den Snapshot, Ruhendes selten (10).
- **Prüfsumme (CRC32):** eine Kontrollzahl, die kaputte oder fremde Päckchen verrät (10).
- **Raum** (Room): der Name, unter dem sich Host und Gäste online treffen (10).
- **Relay:** ein Vermittler, der Päckchen zwischen Browsern weiterreicht, ohne sie zu öffnen (10).
- **Replika:** die Nachbildung der Welt beim Client, ohne eigene Physik und Logik (10).
- **Salt / Challenge:** die Zufallszahl des Clients beim Anklopfen / die Antwort des Servers mit
  seiner eigenen Zufallszahl (10).
- **Serialisierung:** Zahlen und Texte eng in Bits packen und wieder auspacken (10).
- **Slot:** die Spielernummer, die der Server einem Client zuteilt (10).
- **Snapshot:** ein Foto des Spielzustands, das der Server 30-mal pro Sekunde schickt (10).

### Roboter

- **Abgleich:** die simulierte Ente spielt die Befehle des echten Roboters nach; man vergleicht, was
  beide messen (16).
- **Armatur (Schwung des Motors):** Der kleine Motor im Servo dreht sich durch das Getriebe sehr
  schnell. Darum ist er schwer in Schwung zu bringen – das zählt wie zusätzliches Gewicht am Arm (13, 16).
- **Encoder (Winkelmesser):** misst, wie ein Gelenk gerade steht (13).
- **Gerätemodell:** ein kleines Modell eines echten Geräts, beschrieben durch ein Profil (13, 16).
- **BallPlay:** Kerns Nachbau von Pollens Ballspiel-Verhalten, ein Rezept in fünf Schritten (13).
- **Daemon:** das Steuerprogramm, das auf dem echten Roboter läuft (13, 16).
- **Detektor / Tracker:** schätzt aus dem Kamerabild, wo Ball und Socke liegen / merkt sich, wo sie
  zuletzt waren (13).
- **Getriebespiel:** kleines Wackeln im Getriebe, wenn die Zahnräder nicht ganz fest ineinandergreifen
  (13, 16).
- **Gieren / Nicken / Rollen:** die drei Drehrichtungen: „nein“ schütteln, „ja“ nicken, zur Seite
  kippen (11, 13).
- **Grundstellung / Home-Winkel:** die Ruhehaltung der Ente / der Winkel jedes Gelenks darin (13).
- **Hochgehoben (Erkennung):** Pollens kleines Netz merkt, dass die Ente gehalten wird; dann
  pausieren die Beine (13).
- **IMU (Lagesensor):** spürt, wo unten ist und wie schnell sich der Körper dreht (13).
- **Kalibrieren:** die Simulation so lange nachstellen, bis sie sich wie das echte Gerät verhält (16).
- **Kern-Extras (Ballspiel, Socke holen, Schnabel beim Greifen)** (Kern extras (ball play, sock
  fetching, beak in the pick)): Schalter für Kerns Zusätze an der Ente (13).
- **Leerlauf-Blick:** weiche Kopfbewegungen aus Pollens Software, wenn die Ente still steht (13).
- **Maßstab 4:** Kern rechnet intern mit einer viermal so großen Ente, weil die Physik mit so
  winzigen Teilen schlecht rechnet (13).
- **Micro Duck:** zweibeiniger Laufroboter von Pollen Robotics (13).
- **Neuheitsraster:** merkt sich, wo die Ente schon war, damit sie lieber neue Orte besucht (13).
- **Prüfmessung:** eine Messung, die beim Anpassen nicht benutzt wird. Nur sie zeigt, ob das Modell
  wirklich passt (16).
- **Prüfstand:** ein eingespannter Servo mit Arm und Gewicht, an dem man misst (16).
- **Regeltakt:** ein Mal „Netz fragen und Befehl geben“; das passiert 50-mal pro Sekunde in der Zeit der echten Ente (13).
- **RMS-Fehler:** fasst viele Abweichungen zu einer Zahl zusammen: quadrieren, mitteln, Wurzel ziehen.
  Große Ausreißer zählen stark (16).
- **Servo:** Motor mit Getriebe, der auf einen Winkel fährt (13).
- **Servoprofil:** die Zahlen eines Servos: Feder, Kraftgrenze, Bremse, Reibung, Schwung,
  Verzögerung und wie sicher jede Zahl ist (16).
- **Spielschicht:** Kerns Netz, das entscheidet, was die Ente mit Ball und Socke tut (13, 14).
- **Tiefpass:** siehe Filter (13).
- **ToF:** Abstandssensor, der misst, wie lange Licht hin und zurück braucht (13).
- **Läufer** (Walker): Steuerart für Roboter, die ein Netz fährt (13).
- **Verhaltensprogramm:** Pollens Programm, das ohne Eingabe die nächste Tätigkeit der Ente wählt (13).
- **Weltzeit / Roboterzeit:** die Uhr in Kerns viermal so großer Welt / die Uhr des echten Roboters;
  1 Sekunde Weltzeit sind ½ Sekunde Roboterzeit (13).

### Lernen

- **Actor (beim Lernen):** das Netz, das handelt – die „Spielerin“. Nur der Actor kommt später auf
  den Roboter (14, A). Nicht verwechseln mit dem Actor im Studio (11).
- **Adam:** ein kluges Verfahren, das jeden Regler mit passender Schrittgröße und etwas Schwung
  verstellt (A).
- **Agent / Umgebung:** der, der handelt (das Netz) / die Welt, die darauf antwortet (14, A).
- **Aktion / Beobachtung:** was das Netz antwortet / was es in einem Takt zu sehen bekommt (15, A).
- **Aktionsrauschen (σ, sprich „Sigma“):** wie stark die Ente beim Lernen zufällig von ihrem Plan
  abweicht, um Neues auszuprobieren. Es wird beim Lernen kleiner (14, A).
- **Aktivierung / ELU:** der Knick nach jedem Neuron: Positives bleibt, Negatives wird
  zusammengedrückt, nie unter −1 (A).
- **Asymmetrischer Kritiker:** Der Kritiker sieht mehr als die Ente, etwa ihre wahre Höhe. Das darf er,
  weil es ihn nur beim Lernen gibt (14, A).
- **Auswerter:** = Lerner; liest alle Notizen der Helfer und macht daraus eine neue Policy (14).
- **Backpropagation:** rückwärts ausrechnen, wie viel jeder Regler am Fehler schuld ist (A).
- **Belohnung:** Punkte, die dem Netz sagen, was gut war (14, A).
- **Bias:** eine Grundeinstellung, die ein Neuron immer dazuzählt (A).
- **Checkpoint:** ein gespeicherter Zwischenstand des Lernens, von dem man weitermachen kann. Im Text
  heißt er **Spielstand** (der ganze Lernstand) oder **Schnappschuss** (ein Zwischenstand des Netzes)
  (14, 15). Nicht verwechseln mit dem **Snapshot** im Netzwerk (10).
- **CMA-ES:** Suchverfahren wie die Evolution: viele Varianten ausprobieren, die besten behalten, dort
  weitersuchen (14, 16, A).
- **DAgger:** Das Netz fährt selbst, das Vorbild sagt in jeder Lage, was richtig wäre (14, A).
- **Entropie:** wie unvorhersehbar die Ente noch handelt. Ein kleiner Bonus dafür hält sie neugierig,
  damit sie nicht zu früh aufhört auszuprobieren (A).
- **Episode:** ein Durchgang von Anfang bis Ende, etwa ein Sprung (14).
- **Epoche:** einmal durch alle gesammelten Daten lernen (A).
- **GAE:** Rechenweg, der schätzt, wie gut eine Aktion war – auch durch das, was danach kam (A).
- **Gewicht:** ein Regler im Netz: wie wichtig eine Eingabe ist, auch negativ (A).
- **Gradient:** die Steigung für alle Regler zusammen. Er zeigt, in welche Richtung der Fehler
  am schnellsten wächst; man geht in die Gegenrichtung (A).
- **Gradientenabstieg:** in kleinen Schritten bergab zum kleinsten Fehler, wie im Nebel (A).
- **Hyperparameter:** Einstellungen des Lernens selbst, etwa Lernrate oder Zahl der Enten. Sie werden
  nicht gelernt, sondern vorher festgelegt (15).
- **Iteration:** eine Runde des Trainings: Erfahrung sammeln, dann daraus lernen (14).
- **KL-Divergenz:** misst, wie verschieden die alte und die neue Ente handeln. Kern passt danach die
  Lernrate an (A).
- **Kritiker:** das zweite Netz beim Lernen. Er schätzt, wie gut eine Lage ist, wie ein Trainer am
  Spielfeldrand. Auf den Roboter kommt er nicht (14, A).
- **Lehrer:** Kerns Trainingsprogramm, im Browser und als Programm für die Kommandozeile (14, 15).
  Nur der Kritiker heißt im Bild „Trainer“.
- **Lehrplan:** Schwieriges kommt erst später dazu, etwa die Stöße. Zu viel Chaos am Anfang würde das
  Lernen verhindern (14, A).
- **Lernrate:** wie große Schritte das Lernen macht (A).
- **Minibatch:** eine kleine Auswahl der Daten, aus der ein Lernschritt gemacht wird – wie eine
  Umfrage unter 30 statt unter allen 800 Schülern (A).
- **MLP (mehrschichtiges Perzeptron):** ein Netz aus mehreren Schichten hintereinander (A).
- **Neugier-Bonus:** = Entropie-Bonus; ein paar Extrapunkte dafür, dass die Ente noch etwas
  ausprobiert (14, A).
- **Neuronales Netz / Neuron:** Rechennetz aus vielen einfachen Recheneinheiten (A).
- **Normalisieren:** alle Eingaben auf ähnliche Größe bringen, damit große Zahlen nicht alles
  übertönen (A).
- **Notbremse:** kürzt einen Lernschritt, der zu lang würde, auf eine feste Höchstlänge (14, A).
- **ONNX:** Dateiformat für trainierte Netze, so etwas wie PDF für Netze (13, A).
- **Phasensignal (Zeiger-Uhr):** eine Uhr mit einem Zeiger, die dem Sprungnetz sagt, wo im Sprung es
  gerade ist (14, 15).
- **Policy:** die Entscheidungsregel der Ente: Beobachtung hinein, Aktion heraus. Beim Lernen ist sie
  etwas zufällig, auf dem Roboter nicht (14, A).
- **PPO:** das Lernverfahren des Lehrers. Es verbessert die Ente pro Schritt nur ein bisschen, wie
  mit einem Sicherheitsgurt (14, A).
- **Reinforcement Learning:** Lernen durch Ausprobieren und Belohnung (A).
- **Return (Rückgabe):** alle Punkte eines Versuchs zusammengezählt (15). Beim Lernen zählen spätere
  Punkte dabei etwas weniger, mit dem Zukunftsfaktor (A).
- **Rezept (Trainingsplan):** die Trainingseinstellungen einer Aufgabe (14, 15).
- **Sammler / Lerner:** sammelt Erfahrung / lernt daraus. Im Text: „Helfer mit Klemmbrett“ und
  „Auswerter“ (14).
- **Schicht:** viele Neuronen nebeneinander, die alle dieselben Eingaben bekommen (A).
- **Schnappschuss:** siehe Checkpoint.
- **Sim-to-Real:** von der Simulation zum echten Roboter (16, A).
- **Spielstand:** siehe Checkpoint.
- **Streuung:** wie weit Werte typischerweise vom Mittelwert entfernt liegen (16, A).
- **Suchwolke:** die vielen Kandidaten, die CMA-ES um eine Mitte herum ausprobiert (14, A).
- **Totzone:** ein Bereich, in dem eine kleine Eingabe noch nichts bewirkt, wie beim Gamepad-Stick
  (8, 13, 15).
- **Überanpassung:** auswendig lernen statt verstehen (A).
- **Überraschung (TD-Fehler):** wie viel besser oder schlechter ein einzelner Schritt lief, als der
  Kritiker erwartet hatte (A).
- **Umrechner:** = Normalisierer; siehe Normalisieren (14, A).
- **Variation (Domain Randomization):** Jede Übungsrunde bekommt eine etwas andere Ente in einer etwas
  anderen Welt. Dann ist die echte Ente nur eine Variante mehr (14, 16, A).
- **velstand:** Pollens Netz zum Laufen und Stehen, in Kern eingebaut; Startpunkt für Kerns
  Sprung (13, 14).
- **Verlust:** eine Zahl dafür, wie falsch das Netz gerade liegt (A).
- **Verteilungsverschiebung:** Wer nur zuschaut, kennt die Lagen nicht, in die er selbst gerät – wie
  Radfahren nur durch Zuschauen (A).
- **Vorbild:** eine Vorführung, die das Netz zuerst nachahmt (14).
- **Vorteil:** War eine Aktion besser oder schlechter als erwartet? Positiv heißt besser (A).
- **Wackelstärke:** = Aktionsrauschen σ (A).
- **Warmstart:** Das Training beginnt nicht bei null, sondern mit einem fertigen Netz, das schon
  balancieren kann (14, A).
- **Wert:** wie viele Belohnungen man von einer Lage aus im Schnitt noch erwartet (A).

> **Mission:** Öffne **Datei › Beispiel-Levels › Physics Lab** (File › Example Levels › Physics Lab),
> schalte unter **Fenster › Anzeigen › Kontaktpunkte** (Window › Show › Contact Points) ein und
> starte **Simulieren**. Zähl, wie viele Glossar-Wörter du dabei siehst: Körper,
> Gelenk, Kontakt …

## Level geschafft!

- Du findest jede wichtige Taste im Studio in Sekunden.
- Du weißt, warum die Zahl der Enten das Netz ändert, die Zahl der Threads aber nur das Tempo.
- Du weißt, was in einer `.klevel`-Datei steht und wie man sie anschaut.
- Du kennst die Dateien für Netze, Servoprofile und Messungen.
- Du hast ein Glossar mit allen Fachwörtern des Buchs.

## Rätsel-Ecke

1. Mit welcher Taste übernimmst du beim Spielen die Ente?
2. Was ist der Unterschied zwischen **Spielen** (Play) und **Simulieren** (Simulate)?
3. Welches Glossar-Wort passt: „Der Trainer darf die wahre Höhe der Ente sehen, die Ente selbst
   nicht“?
4. **Boss-Rätsel:** Lena trainiert mit 16 Enten auf 2 Threads, Tom mit 16 Enten auf 8 Threads, Mia
   mit 32 Enten auf 2 Threads. Alle nehmen denselben Seed. Wessen Netze sind gleich, und wer ist am
   schnellsten fertig?

### Lösungen

1. **F8**. Noch einmal F8 verlässt sie wieder.
2. Beim **Spielen** spielst du selbst. Beim **Simulieren** läuft die Welt ohne Spielfigur: Du schaust
   mit der Editor-Kamera zu und greifst Körper; bearbeiten kannst du das Level erst nach dem Stoppen.
3. **Asymmetrischer Kritiker.** Den Kritiker gibt es nur beim Lernen, darum darf er mehr sehen.
4. Lena und Tom bekommen genau dasselbe Netz, weil beide 16 Enten haben. Tom ist schneller, weil
   mehr Threads gleichzeitig rechnen (wenn sein Rechner so viele Kerne hat). Mias Netz ist anders,
   weil sie mehr Enten hat.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
