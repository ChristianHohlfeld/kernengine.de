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
| Ende (End) | Auswahl auf den Boden setzen |
| H / Ctrl+H | ausblenden / alles einblenden |
| G | Game View: Ansicht wie im Spiel |
| F2 | umbenennen |
| Ctrl+Z / Ctrl+Y | rückgängig / wiederholen |
| Ctrl+C / V / D | kopieren / einfügen / duplizieren |
| Ctrl+G | gruppieren |
| Ctrl+S / Ctrl+O / Ctrl+N | speichern / öffnen / neues Level |
| Ctrl+Space | Content Drawer (Schublade) |
| Alt+2 | nur Drahtgitter (Wireframe) |
| Alt+3 | Farben ohne Licht (Unlit) |
| Alt+4 | normal mit Licht (Lit) |
| Alt+5 | zeigt, wo der Spieler anstößt |
| Alt+P / Alt+S | Play / Simulate |

### Play und Simulate

Bei **Play** spielst du selbst. Bei **Simulate** läuft nur die Physik, und du kannst weiter im Editor
arbeiten.

| Taste | Wirkung |
|---|---|
| Esc | Stop |
| F8 | Spieler verlassen / übernehmen |
| Pause | anhalten / weiter |
| . | pausiert: einen Schritt weiter |
| Ctrl+R | Spiel neu starten |
| K | in Simulate: Lage der Dinge ins Level übernehmen |
| Shift+F1 | Maus freigeben |

Die Taste **K** ist praktisch: Lass in Simulate einen Stapel Kisten umfallen, wähl sie aus und drück
K. Dann liegen sie auch nach dem Stoppen so da, wie die Physik sie hingelegt hat.

Im Spiel: W A S D bewegen, Maus umsehen, Leertaste springen, Shift Boost, F benutzen.

### Logik-Editor

- **F7** kompiliert: Der Logik-Graph wird in schnellen Maschinencode übersetzt.
- **F9** setzt einen **Haltepunkt**: Das Spiel hält an, wenn dieser Knoten drankommt. So kannst du
  nachsehen, was gerade passiert.
- **Ctrl+F** sucht, **C** legt einen Kommentar um die Auswahl, **Esc** führt zurück zum Viewport.

> **Mission:** Öffne Kern Studio auf https://kernengine.de/ und drück **F1**. Vergleiche den
> Controls-Dialog mit diesen Tabellen. Probier dann Ctrl+1 auf einer schönen Kameraposition, flieg
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
> **2 Worker** mit je **4 Enten**. Notiere nach ein paar Iterationen die Rückgabe. Lade neu und
> starte mit **1 Worker** und **8 Enten**. Beide Male sind es 8 Enten insgesamt – die Zahlen stimmen
> überein. Probier zum Vergleich 2 Worker mit je 8 Enten: Jetzt sind es 16 Enten, und die Zahlen sind
> anders.

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
Boden wie die Achsen auf Karopapier, Z ragt senkrecht in die Höhe. Winkel stehen in Grad, Farben als
Zahlen von 0 bis 1.

> **Mission:** Speichere ein Level mit **Ctrl+S**. Je nach Browser fragt Studio nach einem
> Speicherort, oder die Datei landet in deinem Download-Ordner. Öffne die `.klevel`-Datei mit einem
> Texteditor (Editor unter Windows, TextEdit am Mac). Findest du `"kern-level"` und die Namen deiner
> Actors?

### Netze

Trainierte Netze werden als **ONNX**-Datei gespeichert. ONNX ist für Netze, was PDF für Texte ist: ein
Format, das viele Programme lesen können, auf dem Roboter, im Browser und auf dem Server. Kern liest
Pollens Netze und schreibt seine eigenen im selben Format. Netze, die du im Browser trainierst,
bleiben auf deinem Gerät und ersetzen dort die mitgelieferten.

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
Kapitel 17. Ein schönes Prinzip daraus: **Die Engine fasst nie selbst Dateien an.** Alles, was sie
braucht, reicht ihr der Browser herein.

## 17.6 Glossar

Alle Fachwörter des Buchs, kurz erklärt. Die Zahl in Klammern nennt das Kapitel, in dem es
ausführlich vorkommt (A = Anhang A).

### Studio und Spiele

- **Actor:** ein Ding im Level, mit Position und Bausteinen (8).
- **Autosave:** automatische Sicherung ungespeicherter Arbeit (11).
- **Blueprint / Logik-Graph:** ein Programm aus Knoten und Verbindungen, ohne Tippen (9).
- **Content Drawer:** die Schublade mit Dingen zum Platzieren (11).
- **Controller:** bestimmt, wie der Spieler sich bewegt: laufen, rollen, fahren, fliegen (8).
- **Details:** das Panel mit allen Einstellungen des gewählten Actors (11).
- **Eject / Possess:** im Spiel den Spieler verlassen / wieder übernehmen (11).
- **Engine:** das Programm, das eine Spielwelt zeichnet, rechnet und abspielt (1).
- **Game View:** Ansicht ohne Editor-Hilfen, wie im Spiel (11).
- **Gizmo:** Pfeile und Ringe zum Verschieben, Drehen, Skalieren (11).
- **Haltepunkt:** Das Spiel hält an, wenn ein bestimmter Knoten drankommt (9).
- **HUD:** Anzeigen im Spiel wie Punkte und Zeit (8).
- **Kill Z:** eine Höhe; fällt der Spieler darunter, wird er zurückgesetzt (11).
- **Knoten:** ein Baustein im Logik-Graphen (9).
- **Komponente:** ein Baustein eines Actors, etwa Mesh, Kollider oder Sound (8).
- **Level:** eine Spielwelt, gespeichert als `.klevel` (8).
- **Observer camera:** Play mit freier Kamera zum Zuschauen (11).
- **Outliner:** die Liste aller Actors, wie ein Inhaltsverzeichnis des Levels (11).
- **Package Game:** ein Level als eigenständige Webseiten-Datei exportieren, die man weitergeben
  kann (11).
- **Pawn:** die Spielfigur, die einem Spieler gehört (8).
- **Play / Simulate:** selbst spielen / nur die Physik laufen lassen, während der Editor bedienbar
  bleibt (11).
- **Player-only:** Start ohne Editor, für exportierte Spiele (11).
- **Snapping:** Einrasten auf ein Raster (11).
- **Transform:** Position, Drehung und Größe eines Actors (8).
- **Trigger:** ein Bereich, der etwas auslöst, wenn man ihn betritt (8).
- **Viewport:** das Fenster, durch das du in die 3D-Welt schaust (11).
- **World Settings:** Einstellungen der ganzen Welt, etwa die Schrittrate (11).

### Grafik und Geometrie

- **Bloom:** helle Stellen leuchten über ihren Rand hinaus (4).
- **BVH:** Kisten in Kisten. Große Kisten enthalten kleinere, und ganz innen liegen die Dreiecke. Ein
  Strahl muss nur in die Kisten schauen, die er trifft – das spart viel Suchen (5).
- **Ego-Ebene:** wird als Letztes gezeichnet und liegt darum immer vorne, etwa für die Gizmos (4).
- **Frame:** ein einzelnes Bild; viele pro Sekunde ergeben Bewegung (4).
- **Frustum-Culling:** nicht zeichnen, was außerhalb des Blickfelds liegt (5).
- **Instancing:** dasselbe Mesh vielfach auf einmal zeichnen (4).
- **Kontextverlust:** Der Browser nimmt die Grafikkarte kurz weg; Kern baut alles neu auf (4).
- **Material:** wie eine Oberfläche aussieht: Farbe, Rauheit, Metall (4).
- **Mesh:** eine 3D-Form aus Dreiecken (4).
- **Partikel:** viele kleine Punkte für Funken, Rauch, Staub (4).
- **Pass:** ein Durchgang beim Zeichnen, etwa für Schatten (4).
- **Picking:** herausfinden, was unter dem Mauszeiger liegt (5).
- **Renderer:** der Teil, der die Bilder zeichnet (4).
- **Shader:** kleines Programm auf der Grafikkarte, das Punkte und Farben berechnet (4).
- **Skinning:** ein Mesh mit Knochen verformen, etwa für Figuren (4).
- **Textur:** ein Bild auf einer Oberfläche (4).
- **Umgebungsverdeckung:** dunkelt Ecken und Ritzen ab, in die wenig Licht kommt (4).
- **Vertex:** ein Eckpunkt eines Dreiecks (4).
- **WebGL2:** der Weg, über den der Browser die Grafikkarte benutzt (4).
- **Welding:** doppelte Eckpunkte zu einem zusammenfassen (5).

### Mathe und Technik

- **Bit:** die kleinste Einheit im Computer: 0 oder 1. „Bit für Bit gleich“ heißt: ganz exakt
  gleich (3).
- **C++ / JavaScript:** die Programmiersprache der Engine / die Sprache der Webseiten (1).
- **CI:** automatische Prüfung jeder Änderung auf einem Server (2).
- **Compiler / kompilieren:** ein Programm, das Code in Maschinensprache übersetzt, die der Prozessor
  direkt ausführt (2).
- **CSV:** eine Tabelle als Textdatei, die Werte durch Kommas getrennt (16).
- **Determinismus:** gleiche Eingaben ergeben Bit für Bit dasselbe Ergebnis, auf jedem Gerät (6, 14).
- **Fingerabdruck:** eine Prüfzahl je Takt, die jede kleinste Abweichung verrät (14).
- **Hertz (Hz):** wie oft etwas pro Sekunde passiert. 50 Hz heißt 50-mal pro Sekunde (16).
- **Interpreter:** führt einen Logik-Graphen Knoten für Knoten aus (9).
- **JIT:** Ein Logik-Graph wird während des Spiels in schnellen Code übersetzt (9).
- **JSON:** ein lesbares Textformat für Daten: Namen und Werte in Klammern (3).
- **Matrix:** eine Zahlentabelle, die Drehen, Verschieben und Skalieren auf einmal beschreibt (3).
- **Quaternion:** vier Zahlen, die eine Drehung beschreiben (3).
- **Radiant (rad):** ein anderes Winkelmaß als Grad. 1 rad sind etwa 57°, ein ganzer Kreis etwa
  6,28 rad (13).
- **Seed:** die Startzahl für den Zufall im Computer. Gleicher Seed, gleiche Zufallszahlen – so lässt
  sich jeder Lauf genau wiederholen (14).
- **Terminal:** ein Fenster, in das man Befehle tippt, statt zu klicken (2).
- **Thread:** ein Rechenstrang. Ein Prozessor mit mehreren Kernen rechnet mehrere Threads
  gleichzeitig (14).
- **Vektor:** ein Pfeil mit Richtung und Länge, etwa (x, y, z) (3).
- **WebAssembly:** ein Programmformat, das Browser sehr schnell ausführen (1).
- **Web Worker:** ein Rechenhelfer im Browser, der neben der Webseite herläuft (14).

### Physik

- **Box3D:** die Physik-Engine von Erin Catto, die Kern unverändert mitbringt (6).
- **Gelenk:** verbindet zwei Körper, etwa als Scharnier (6).
- **Kinematisch:** ein Körper, der einer Vorgabe folgt und sich von nichts bremsen lässt (6).
- **Körper / Form:** was sich bewegt / welche Gestalt es hat (6).
- **Kollider:** die unsichtbare Form, mit der etwas zusammenstößt (6).
- **Kontakt:** eine Stelle, an der sich zwei Körper berühren (6).
- **Konvexe Hülle:** eine Form ohne Dellen nach innen (13).
- **Moment (Drehmoment):** eine Drehkraft, wie wenn du an einem Schraubenschlüssel ziehst (13).
- **Ray-Cast:** ein Strahltest: Was trifft ein Strahl zuerst? (6)
- **Reibung / Restitution / Rollwiderstand:** Haften / Abprallen / Bremsen beim Rollen (6).
- **Substeps:** Die Physik teilt jeden Schritt in kleinere Stücke, damit sie genauer rechnet; für die
  Ente in 16 (6).
- **Trägheit:** wie schwer sich etwas in Bewegung oder Drehung versetzen lässt (6).
- **Zeitschritt:** die feste Zeit, um die die Physik jedes Mal weiterrechnet (6, 8).

### Klang und Netzwerk

- **3D-Klang:** Klang, der von einer Stelle im Raum kommt (7).
- **Hall:** der Nachklang eines Raums (7).
- **Mixer:** mischt alle Klänge zu einem (7).
- **Synthesizer:** erzeugt Klänge aus Rechnung statt aus Aufnahmen (7).
- **Client / Server:** Mitspieler / der Rechner, der das Spiel bestimmt (10).
- **Datagramm:** ein einzelnes Datenpaket im Netz (10).
- **Interpolation:** zwischen zwei bekannten Zuständen weich überblenden (10).
- **Ping:** wie lange eine Nachricht zum anderen und zurück braucht (10).
- **Relay:** ein Vermittler, der Pakete zwischen Browsern weiterreicht (10).
- **Snapshot:** ein Schnappschuss des Spielzustands, den der Server an alle schickt (10).

### Roboter

- **Abgleich:** die simulierte Ente spielt die Befehle des echten Roboters nach; man vergleicht, was
  beide messen (16).
- **Armatur (Schwung des Motors):** Der kleine Motor im Servo dreht sich durch das Getriebe sehr
  schnell. Darum ist er schwer in Schwung zu bringen – das zählt wie zusätzliches Gewicht am Arm (13, 16).
- **Encoder (Winkelmesser):** misst, wie ein Gelenk gerade steht (13).
- **Gerätemodell:** ein kleines Modell eines echten Geräts, beschrieben durch ein Profil (13, 16).
- **Getriebespiel:** kleines Wackeln im Getriebe, wenn die Zahnräder nicht ganz fest ineinandergreifen
  (16).
- **Home-Pose:** die Grundstellung der Ente (13).
- **IMU (Lagesensor):** spürt, wo unten ist und wie schnell sich der Körper dreht (13).
- **Kalibrieren:** die Simulation so lange nachstellen, bis sie sich wie das echte Gerät verhält (16).
- **Kern extras:** Schalter für Kerns Zusätze an der Ente (13).
- **Maßstab 4:** Kern rechnet intern mit einer viermal so großen Ente, weil die Physik mit so
  winzigen Teilen schlecht rechnet (13).
- **Micro Duck:** zweibeiniger Laufroboter von Pollen Robotics (13).
- **Pick-up-Erkennung:** merkt, dass die Ente hochgehoben wird (13).
- **Prüfmessung:** eine Messung, die beim Anpassen nicht benutzt wird. Nur sie zeigt, ob das Modell
  wirklich passt (16).
- **Prüfstand:** ein eingespannter Servo mit Arm und Gewicht, an dem man misst (16).
- **Regeltakt:** ein Mal „Netz fragen und Befehl geben“; das passiert 50-mal pro Sekunde (13).
- **RMS-Fehler:** fasst viele Abweichungen zu einer Zahl zusammen: quadrieren, mitteln, Wurzel ziehen.
  Große Ausreißer zählen stark (16).
- **Servo:** Motor mit Getriebe, der auf einen Winkel fährt (13).
- **Servoprofil:** die Zahlen eines Servos: Feder, Kraftgrenze, Bremse, Reibung, Schwung,
  Verzögerung und wie sicher jede Zahl ist (16).
- **Spielschicht:** Kerns Netz, das entscheidet, was die Ente mit Ball und Socke tut (13).
- **Tiefpass:** glättet Werte, damit nichts ruckt (13).
- **ToF:** Abstandssensor, der misst, wie lange Licht hin und zurück braucht (13).
- **Walker:** Steuerart für Roboter, die ein Netz fährt (13).

### Lernen

- **Actor (beim Lernen):** das Netz, das handelt. Nur der Actor kommt später auf den Roboter (14, A).
- **Adam:** ein kluges Verfahren, das jeden Regler mit passender Schrittgröße und etwas Schwung
  verstellt (A).
- **Agent / Umgebung:** der, der handelt (das Netz) / die Welt, die darauf antwortet (A).
- **Aktionsrauschen (σ, sprich „Sigma“):** wie stark die Ente beim Lernen zufällig von ihrem Plan
  abweicht, um Neues auszuprobieren. Es wird beim Lernen kleiner (14, A).
- **Aktivierung / ELU:** der Knick nach jedem Neuron: Positives bleibt, Negatives wird
  zusammengedrückt, nie unter −1 (A).
- **Asymmetrischer Kritiker:** Der Kritiker sieht mehr als die Ente, etwa ihre wahre Höhe. Das darf er,
  weil es ihn nur beim Lernen gibt (14, A).
- **Backpropagation:** rückwärts ausrechnen, wie viel jeder Regler am Fehler schuld ist (A).
- **Belohnung:** Punkte, die dem Netz sagen, was gut war (14, A).
- **Bias:** eine Grundeinstellung, die ein Neuron immer dazuzählt (A).
- **Checkpoint:** ein gespeicherter Zwischenstand des Lernens, von dem man weitermachen kann (15).
- **CMA-ES:** Suchverfahren wie die Evolution: viele Varianten ausprobieren, die besten behalten, dort
  weitersuchen (14, 16, A).
- **DAgger:** Das Netz fährt selbst, das Vorbild sagt in jeder Lage, was richtig wäre (14, A).
- **Entropie:** wie unvorhersehbar die Ente noch handelt. Ein kleiner Bonus dafür hält sie neugierig,
  damit sie nicht zu früh aufhört auszuprobieren (A).
- **Epoche:** einmal durch alle gesammelten Daten lernen (A).
- **Episode:** ein Durchgang von Anfang bis Ende, etwa ein Sprung (14).
- **GAE:** Rechenweg, der schätzt, wie gut eine Aktion war – auch durch das, was danach kam (A).
- **Gewicht:** ein Regler im Netz: wie wichtig eine Eingabe ist, auch negativ (A).
- **Gradient:** die Steigung für alle Regler zusammen. Er zeigt, in welche Richtung der Fehler
  am schnellsten wächst; man geht in die Gegenrichtung (A).
- **Gradientenabstieg:** in kleinen Schritten bergab zum kleinsten Fehler, wie im Nebel (A).
- **Hyperparameter:** Einstellungen des Lernens selbst, etwa Lernrate oder Zahl der Enten. Sie werden
  nicht gelernt, sondern vorher festgelegt (15).
- **Iteration:** eine Runde des Trainings: Erfahrung sammeln, dann daraus lernen (14).
- **Kritiker:** das zweite Netz beim Lernen. Er schätzt, wie gut eine Lage ist, wie ein Trainer am
  Spielfeldrand. Auf den Roboter kommt er nicht (14, A).
- **KL-Divergenz:** misst, wie verschieden die alte und die neue Ente handeln. Kern passt danach die
  Lernrate an (A).
- **Lehrer:** Kerns Trainingssystem, im Browser und als Programm (14).
- **Lehrplan:** Schwieriges kommt erst später dazu, etwa die Stöße. Zu viel Chaos am Anfang würde das
  Lernen verhindern (14, A).
- **Lernrate:** wie große Schritte das Lernen macht (A).
- **MLP (mehrschichtiges Perzeptron):** ein Netz aus mehreren Schichten hintereinander (A).
- **Minibatch:** eine kleine Auswahl der Daten, aus der ein Lernschritt gemacht wird – wie eine
  Umfrage unter 30 statt unter allen 800 Schülern (A).
- **Neuronales Netz / Neuron:** Rechennetz aus vielen einfachen Recheneinheiten (A).
- **Normalisieren:** alle Eingaben auf ähnliche Größe bringen, damit große Zahlen nicht alles
  übertönen (A).
- **ONNX:** Dateiformat für trainierte Netze, so etwas wie PDF für Netze (13, A).
- **Phasensignal:** ein Taktgeber, der dem Sprungnetz sagt, wo im Sprung es gerade ist (14).
- **Policy:** die Entscheidungsregel der Ente: Beobachtung hinein, Aktion heraus. Beim Lernen ist sie
  etwas zufällig, auf dem Roboter nicht (13, A).
- **PPO:** das Lernverfahren des Lehrers. Es verbessert die Ente pro Schritt nur ein bisschen, wie
  mit einem Sicherheitsgurt (14, A).
- **Reinforcement Learning:** Lernen durch Ausprobieren und Belohnung (A).
- **Return (Rückgabe):** alle künftigen Belohnungen zusammengezählt, wobei Späteres weniger zählt (A).
- **Rezept:** die Trainingseinstellungen einer Aufgabe (14).
- **Rollout:** gesammelte Erfahrung: was die Ente sah, tat und dafür bekam (14).
- **Sammler / Lerner:** sammelt Erfahrung / lernt daraus (14).
- **Schicht:** viele Neuronen nebeneinander, die alle dieselben Eingaben bekommen (A).
- **Sim-to-Real:** von der Simulation zum echten Roboter (16, A).
- **Streuung:** wie weit Werte typischerweise vom Mittelwert entfernt liegen (16, A).
- **Überanpassung:** auswendig lernen statt verstehen (A).
- **Variation (Domain Randomization):** Jede Übungsrunde bekommt eine etwas andere Ente in einer etwas
  anderen Welt. Dann ist die echte Ente nur eine Variante mehr (14, 16, A).
- **velstand:** Pollens Netz zum Laufen und Stehen, in Kern eingebaut (13).
- **Verlust:** eine Zahl dafür, wie falsch das Netz gerade liegt (A).
- **Verteilungsverschiebung:** Wer nur zuschaut, kennt die Lagen nicht, in die er selbst gerät – wie
  Radfahren nur durch Zuschauen (A).
- **Vorbild:** eine Vorführung, die das Netz zuerst nachahmt (14).
- **Vorteil:** War eine Aktion besser oder schlechter als erwartet? Positiv heißt besser (A).
- **Warmstart:** Das Training beginnt nicht bei null, sondern mit einem fertigen Netz, das schon
  balancieren kann (14, A).
- **Wert:** wie viele Belohnungen man von einer Lage aus im Schnitt noch erwartet (A).

> **Mission:** Öffne **File › Example Levels › Physics Lab**, schalte unter Window › Show **Contact
> Points** ein und starte **Simulate**. Zähl, wie viele Glossar-Wörter du dabei siehst: Körper,
> Gelenk, Kontakt …

## Level geschafft!

- Du findest jede wichtige Taste im Studio in Sekunden.
- Du weißt, warum die Zahl der Enten das Netz ändert, die Zahl der Threads aber nur das Tempo.
- Du weißt, was in einer `.klevel`-Datei steht und wie man sie anschaut.
- Du kennst die Dateien für Netze, Servoprofile und Messungen.
- Du hast ein Glossar mit allen Fachwörtern des Buchs.

## Rätsel-Ecke

1. Mit welcher Taste übernimmst du im Play die Ente?
2. Was ist der Unterschied zwischen Play und Simulate?
3. Welches Glossar-Wort passt: „Der Trainer darf die wahre Höhe der Ente sehen, die Ente selbst
   nicht“?
4. **Boss-Rätsel:** Lena trainiert mit 16 Enten auf 2 Threads, Tom mit 16 Enten auf 8 Threads, Mia
   mit 32 Enten auf 2 Threads. Alle nehmen denselben Seed. Wessen Netze sind gleich, und wer ist am
   schnellsten fertig?

### Lösungen

1. **F8**. Noch einmal F8 verlässt sie wieder.
2. Bei Play spielst du selbst. Bei Simulate läuft nur die Physik, und du kannst weiter im Editor
   arbeiten.
3. **Asymmetrischer Kritiker.** Den Kritiker gibt es nur beim Lernen, darum darf er mehr sehen.
4. Lena und Tom bekommen genau dasselbe Netz, weil beide 16 Enten haben. Tom ist schneller, weil
   mehr Threads gleichzeitig rechnen (wenn sein Rechner so viele Kerne hat). Mias Netz ist anders,
   weil sie mehr Enten hat.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
