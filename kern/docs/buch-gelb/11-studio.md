# 11 Kern Studio: Deine Werkstatt im Browser

> **Dein Level:** Jetzt wird gebaut! Kern Studio ist der Editor, in dem du Welten zusammensetzt,
> Physik ausprobierst, Spiele testest und als eigene Datei verschickst. Dieses Kapitel ist eine
> geführte Tour mit vielen Missionen – am besten hast du Studio beim Lesen offen.

Kern Studio arbeitet wie die Unreal Engine (Kern ist aber kein Produkt von Epic Games). Ein Level
besteht aus **Actors** – allen Dingen deiner Welt. Jeder Actor trägt **Komponenten**, Bausteine wie
„sieht aus wie ein Würfel“ oder „fällt herunter“. Du bearbeitest sie in **Viewports** (Fenstern in
die 3D-Welt), im **Outliner**, in **Details** und in den **World Settings**. **Play** und
**Simulate** laufen direkt im Viewport.

Das Besondere: Der ganze Editor ist C++ und läuft im **selben WebAssembly-Modul** wie Grafik,
Physik und Spiel-Laufzeit. Was du im Viewport siehst, berechnet genau der Code, der später dein
exportiertes Spiel ausführt (11.19). Wie daraus ein Spiel wird, zeigt
[Kapitel 12](12-eigene-spiele.md).

> **Vorsicht, Falle:** Steht dein Browser auf Deutsch, übersetzt Studio viele Beschriftungen – aus
> „File“ wird „Datei“, aus „Play“ wird „Spielen“. Dieses Buch nutzt die englischen Namen. Der Knopf
> **DE | EN** unten in der Statusleiste schaltet um.

---

## 11.1 Studio starten

Drei Wege führen zu Studio: **live** über https://kernengine.de/ (der Editor liegt im Ordner
/editor/, läuft auch am Handy), als **eine Datei** `generated/studio.html`, die `tools/build_studio.sh`
baut und die offline läuft, oder zum **Entwickeln** über `kern/web/studio/index.html` auf einem
Webserver. Die Startseite verlinkt auch Lehrer, Beispielspiele, zwei Levels zum Spielen, Lizenz
(Apache 2.0), Quellcode und Impressum.

Nach „Starting the engine“ ist beim allerersten Start **Orb Run** geladen: eine
Zufallsstadt mit Verkehr, einem Hovercar, 24 Energie-Orbs und einem Tag, der in die Nacht übergeht
(„Kern Studio ready - Orb Run loaded“). Danach öffnet Studio immer das Level, an dem du zuletzt
gearbeitet hast, und bietet **Start fresh** für einen Neuanfang mit Orb Run. Gab es ungespeicherte
Änderungen, fragt es **Restore "…" from your last session?** (11.15).

> **Mission:** Öffne Kern Studio, warte auf Orb Run und drück **Alt+P**. Fahr mit W A S D zu drei
> Orbs. **Esc** bringt dich zurück in den Editor.

### Als App installieren

In Chrome und Edge zeigt die Titelleiste **⤓ Install**; auf iPhone und iPad erklärt derselbe Knopf
die Schritte **Teilen** und **Zum Home-Bildschirm**. Installiert startet Studio im Vollbild, Notch
und Statusleiste bleiben frei. Ein Service Worker hält die letzte Version offline bereit. Studio sucht
regelmäßig (auch alle 60 s) nach Updates. Kann nichts verloren gehen, lädt es sofort neu, sonst kommt
**New version available** mit **Update** und **Later** – dein Level reist per Autosave mit.

---

## 11.2 Die Oberfläche im Überblick

![Ausgewählter Actor mit Move-Gizmo und seinen Komponenten in Details](../buch/bilder/studio-selected.png)

| Bereich | Wo | Inhalt |
|---|---|---|
| Titelleiste | oben | Menüs, Levelname (Punkt = ungespeichert), Bilder pro Sekunde |
| Toolbar | darunter | Speichern, Undo, Add, Werkzeuge, Raster, Play, Logic, Export |
| Viewports | Mitte | 1, 2 oder 4 Blicke in die Welt |
| Outliner | rechts oben | alle Actors als Baum |
| Details / World Settings | rechts unten | Auswahl bzw. ganze Welt |
| Content Drawer | unten, klappbar | Place Actors, Textures, Sounds, Output Log |
| Statusleiste | ganz unten | Konsole, Meldungen, Physik-Zahlen, Ton, Version |

Die Trennlinien kannst du ziehen; Studio merkt sich die Größen.

### Menüleiste

Ist ein Menü offen, wechselst du durch Darüberfahren zum nächsten. Untermenüs klappen mit der Maus
**neben** dem Menü auf; mit dem Finger oder ohne Platz ersetzen sie es an Ort und Stelle, mit
Zurück-Zeile. So liegt nie ein Menü unter deinem Finger. **Esc** schließt, Graues tut gerade nichts.

- **File:** New Level (Ctrl+N), Open Level (Ctrl+O), Save (Ctrl+S), Save As (Ctrl+Shift+S), Import
  Texture, Package Game (HTML) und Example Levels.
- **Edit:** Undo (Ctrl+Z), Redo (Ctrl+Y), History, Cut/Copy/Paste, Duplicate (Ctrl+D), Delete
  (Entf), Select All (Ctrl+A), Deselect (Esc), Group (Ctrl+G), Snap to Floor (End), Preferences,
  Reset Settings to Defaults.
- **Window:** Viewport Layout, Viewport Type, View Mode, Show, Game View (G), Focus Selection (F),
  Content Drawer (Ctrl+Space), Output Log, Fullscreen (F11).
- **Play:** Play (Alt+P), Simulate (Alt+S), Pause, Advance One Frame (.), Stop (Esc),
  Eject/Possess (F8), Restart Game (Ctrl+R), Keep Simulation Changes (K), Number of Players,
  Network Emulation, Host/Join/Leave Online Game.
- **Help:** Controls (F1) und About Kern Studio.

**New Level** hat sechs Vorlagen (Doppelklick erstellt sofort):

| Vorlage | Was drin ist |
|---|---|
| Orb Run | Nachtstadt, Hovercar, 24 Orbs |
| Physics Lab | jedes Box3D-Gelenk: Scharnier, Schiene, Rad, Seilbrücke, Motor, Stapel |
| Shooting Range | Soldat, Zielscheiben, Deckung, mehrspielerfähig |
| Micro Duck | Laufroboter von Pollen Robotics mit trainiertem Netz |
| Basic | Boden, Sonne, Himmel |
| Empty | gar nichts |

Ungespeicherte Änderungen am alten Level bleiben im Autosave, bis du das neue bearbeitest.

### Toolbar

Von links: Speichern, Undo/Redo, **+ Add ▾** (alle Katalog-Gruppen), die Werkzeuge **Select /
Move / Rotate / Scale** (Q / W / E / R), Globus oder Würfel für Welt- oder lokale Achsen (Ctrl+`), drei Snap-Knöpfe (Raster 1 cm bis 100 m; Winkel 1° bis
90°; Skalierung 0,0625 bis 1), Kamerageschwindigkeit 1–8, dann Play, Simulate, Pause, Frame, Stop,
Eject, Layout, **Logic**, das Paket (Export) und auf schmalen Bildschirmen **Panels**.

### Viewports

Jeder Viewport hat eine Leiste: **≡** (Game View, Sichtfeld 50°–110°, Focus, Lesezeichen, Layout),
den **Ansichtstyp** (Perspective, Top, Bottom, Left, Right, Front, Back), den **View Mode** (Lit,
Unlit, Wireframe, Player Collision), **Show** (Grid, Icons, Selection outline, Collision shapes,
Constraints, Contact points, Bounds, Fog, Sky, Stats) und **Maximieren**. Ein Klick darauf macht den
Viewport **aktiv**. In flachen (orthografischen) Ansichten schaltet Studio auf Wireframe.
**Constraints** ist zuerst aus, sonst malt jede Kette Hilfslinien.

Unten links zeigt das **Achsenkreuz** X rot, Y grün, Z blau – **Z zeigt nach oben**. Info- und
Hinweiszeile nennen Tempo, Kameraposition und was gerade geht.

### Outliner

Der Outliner listet alle Actors mit **Auge**, **Label** und **Type** (etwa Player, Pickup, Static
Mesh oder Empty – je nach Komponenten).

| Aktion | Wirkung |
|---|---|
| Klick, Ctrl+Klick, Shift+Klick | wählen, einzeln dazu/weg, alles dazwischen |
| Doppelklick | ins Bild holen; Ordner umbenennen |
| F2 | umbenennen |
| Auge / Pfeil | ein-/ausblenden / Kinder aufklappen |
| auf Actor ziehen | als Kind anhängen |
| auf Ordner / ins Leere ziehen | in den Ordner / lösen |
| Ordner-Knopf | neuer Ordner für die Auswahl |
| Suchfeld | nach Name, Typ und Tags filtern |

Rechtsklick öffnet das Kontextmenü, bei Ordnern auch Rename Folder und Remove Folder (keep actors).
Der gesteuerte Actor trägt **PLAYER**, gesperrte ein Schloss. Ordner gibt es nur für Actors der
obersten Ebene.

### Details

Oben stehen Name, **Logic** (Graph dieses Actors) und **+ Add** (Komponente), darunter Suchfeld und
die Abschnitte **Actor**, **Transform** und je Komponente (zuklappbar, mit Papierkorb). Bei mehreren
Actors erscheinen nur gemeinsame Komponenten; verschiedene Werte zeigen „Multiple“.

- **Zahlen** tippen (Rechnungen wie `2*3` gehen, Komma gilt als Punkt), am Label seitwärts ziehen
  oder mit ↑/↓ ändern – Shift ×10, Ctrl ×0,1. Felder von 0 bis 1 haben einen Regler.
- **Vektoren** haben X, Y, Z; bei Scale gibt es ein Schloss für gleichmäßiges Skalieren. **Farben** per Wähler
  oder Hex-Wert. **Tags** durch Komma getrennt.
- Der **Pfeil** rechts setzt auf Standard zurück; Rechtsklick bietet Reset to Default, Copy Value,
  Paste Value.

> **Krass, aber wahr:** Ziehst du drei Sekunden an einem Regler, ändert sich der Wert hunderte Male
> – trotzdem nimmt **ein** Ctrl+Z alles zurück. Jeder Ziehvorgang bekommt eine Kennung, und alle
> Änderungen mit derselben Kennung werden ein Schritt.

### Statusleiste und Konsole

Unten liegen Content Drawer, Output Log, die Konsole **Cmd** (↑/↓ blättert), die letzte Meldung, in
Simulate/Play die Physik-Zahlen (online auch Ping und Verlust), Lautsprecher, Pegel, Auswahl und
Version.

| Befehl | Wirkung |
|---|---|
| `play`, `simulate`, `stop` | wie die Knöpfe |
| `spawn <id>` | Element platzieren |
| `select <name>` | Actors wählen, deren Name den Text enthält |
| `tod <0..1>` | Tageszeit |
| `gravity <x y z>` | Schwerkraft (leer: 0 0 −9,80665) |
| `camspeed <1-8>`, `stat`, `clear`, `help` | Tempo, Zahlen ins Log, Log leeren, Hilfe |

> **Mission:** Öffne File › Example Levels › **Basic** und tipp `spawn physics_cube`. Setz in
> Details unter **Transform** bei **Location** Z auf 10. Tipp `gravity 0 0 -1.6` (etwa
> Mondschwerkraft) und drück **Alt+S**: Wie langsam fällt die Kiste? **Esc**, dann `gravity` ohne
> Zahlen – zurück auf der Erde.

### Kontextmenü

Rechtsklick (am Touchscreen 0,5 s halten) wählt den Actor unter dem Zeiger und öffnet das Menü – nie
das des Browsers. Mit Auswahl: Focus, Rename, Cut, Copy, Paste, Duplicate, Delete, **Edit Logic**
bzw. **Add Logic (Blueprint)**, **Make Player**, Snap to Floor, Group, ab zwei Actors **Attach to …**
und **Connect with Constraint ▸**, **Detach**, Hide, Lock/Unlock, Select Children und **Place Actor
▸** (Cube, Sphere, Physics Cube, Point Light, Orb oder Empty Actor genau an die Klickstelle). Ohne
Auswahl: Paste, Select All, Show All, Place Actor. In Simulate/Play: Focus und Keep Simulation
Changes.

---

## 11.3 Navigation im Viewport

Die Steuerung folgt Unreal und gilt für den Viewport unter dem Zeiger.

| Eingabe | Wirkung |
|---|---|
| rechte Maustaste + W A S D | fliegen (Pfeile gehen auch) |
| … + Q / E, Maus bewegen | runter / hoch, umsehen |
| … + Shift / Mausrad / Z, C | schneller / Tempo 1–8 / zoomen |
| rechte + linke Taste, mittlere Taste | verschieben, schwenken |
| Alt + links / rechts / Mitte | um die Auswahl kreisen / näher, weiter / schwenken |
| Mausrad | vor/zurück; flach: zum Zeiger zoomen |
| F oder Doppelklick | Auswahl einrahmen (F ohne Auswahl: alles) |
| Ctrl+1 … 9 / 1 … 9 | Lesezeichen setzen / anspringen |

Beim Fliegen schalten W, E, R, Q keine Werkzeuge um. Bewegungen lesen die **Position** der Taste
(WASD liegt auf jeder Tastatur gleich), Kürzel wie Ctrl+Z die **Beschriftung**. Tempo in m/s: Stufe 1 = 1, 2 = 2,5, 3 = 5, **4 = 10
(Standard)**, 5 = 20, 6 = 40, 7 = 80, 8 = 160 – ab Stufe 3 verdoppelt es sich jedes Mal.

**Mit Fingern:** ein Finger kreist (flach: verschiebt), zwei zoomen und verschieben, Tippen wählt,
zweimal Tippen rahmt ein, langes Halten öffnet das Kontextmenü, ein Finger auf dem Gizmo bewegt den
Actor. Was unter deinen Fingern liegt, bleibt beim Zoomen unter ihnen – darum fühlt sich die Geste an
einer Socke so an wie an einem Häuserblock. Die Webseite zoomt nie mit.

> **Mission:** Flieg in Orb Run mit gehaltener rechter Maustaste über die Stadt, dreh am Mausrad bis
> Speed 8. Speichere mit **Ctrl+1** einen Blick auf ein Hochhaus, flieg weg und drück **1**.

---

## 11.4 Tastenkürzel

**F1** zeigt die wichtigsten auch in Studio. Fürs **Bearbeiten**, zusätzlich zu den Menükürzeln:

| Taste | Wirkung |
|---|---|
| Leertaste | Move → Rotate → Scale → Move |
| [ / ] | Raster kleiner / größer (0,01 m bis 100 m) |
| H / Shift+H / Ctrl+H | ausblenden / alle anderen aus / alles ein |
| Esc | Gizmo-Zug abbrechen, sonst Auswahl aufheben |
| Ctrl+Shift+Z | Redo |
| Alt+G / J / H / K | Perspektive / Top / Front / Seite |
| Alt+2 / 3 / 4 / 5 | Wireframe / Unlit / Lit / Player Collision |
| Alt + Gizmo ziehen | Kopie ziehen |

Kopierte Actors landen als Text in der echten Zwischenablage – du kannst sie in einem anderen Tab
einfügen.

**In Play und Simulate:** Esc stoppt, F8 verlässt/übernimmt den Spieler, Shift+F1 gibt die Maus
frei, Pause (oder Alt+Shift+P) hält an, Alt+P beendet Play, Alt+S schaltet auf Simulate, Ctrl+R
startet neu (nicht beim Soldaten – dort ist Ctrl Ducken), „.“ geht pausiert einen Schritt, K, F und G
wie im Editor. Hat der Browser die Maus gefangen, verschluckt er das erste Esc; Studio stoppt dann
trotzdem sofort.

**Die Spieltasten:** W A S D bewegen, Maus schaut, Leertaste springt (Flyer: hoch; runter mit Q),
Shift ist Boost. Soldat: linke Maustaste feuern, rechte zielen, R nachladen, C ducken. F benutzt,
1–7 und V steuern die Micro Duck.

**Im Logik-Editor:** F7 Compile, Ctrl+F Suchen, F9 Breakpoint, Q Drähte geraderichten, Shift+W/A/S/D
ausrichten, B, S, D, N, O, G oder L plus Klick fügt Branch, Sequence, Delay, Do N, Do Once, Gate oder
For Loop ein, C setzt einen Kommentar, F/Home rahmt ein, Esc führt zurück. Hier gehören Buchstaben
dem Graphen; nur Ctrl+Z, Ctrl+Y, Ctrl+S und F1 erreichen das Level.

---

## 11.5 Auswählen, Gizmos und Snapping

Linksklick wählt, Shift ergänzt, Ctrl schaltet um, ein Klick ins Leere hebt auf. **Linkes Ziehen**
zeichnet immer ein Auswahlrechteck, auch auf einem Actor – es bewegt nie Kamera oder Actor. Bewegen
tut **nur der Gizmo**. Das Rechteck nimmt jeden sichtbaren, nicht gesperrten Actor, dessen
Mittelpunkt drinliegt (die Zufallsstadt nicht). Lichter und leere Actors wählst du über ihr Symbol
(Show › Icons); gesperrte gar nicht. Der zuletzt gewählte ist die **primäre** Auswahl – dort sitzt
der Gizmo.

Der **Gizmo** ist der bunte Anfasser: bei **Move** (W) drei Pfeile, drei Flächen für je zwei Achsen
und die Mitte; bei **Rotate** (E) drei Ringe; bei **Scale** (R) drei Achsen und ein Mittelgriff;
**Select** (Q) hat keinen. Rot ist X, grün Y, blau Z; der Griff unter dem Zeiger leuchtet gelb.
Kinder folgen ihren Eltern. **Alt + Ziehen** zieht eine Kopie, das Original bleibt – ein einziger
Undo-Schritt. **Esc** bricht ab.

| Snapping | Standard | Wirkung |
|---|---|---|
| Grid | an, 0,5 m | Move rastet ein |
| Rotation | an, 15° | Rotate in Schritten |
| Scale | aus, 0,25 | Scale in Schritten |

In Weltachsen rastet Move auf feste Positionen ein, in lokalen auf Schritte. Neue Actors landen mit X
und Y auf dem Raster.

> **So kannst du es dir vorstellen:** Snapping ist Karopapier im Matheheft – alle Ecken passen genau.

> **Mission:** Platziere im Basic-Level einen Cube. **W** und am roten Pfeil ziehen: 0,5-m-Schritte.
> **E**: 15° pro Raster. Jetzt **Alt** halten und ziehen – zwei Würfel! Bau so eine Treppe aus fünf
> Stufen.

---

## 11.6 Levels, Actors und Komponenten

Ein Level besteht aus World Settings, Actors, Texturen, Sounds, Logik und Editor-Kamera, gemessen in
Metern, Sekunden, Kilogramm und Grad. Ein Actor hat Name, Eltern, Ordner, Tags, Hidden, Locked,
Transform und Komponenten.

### Actors anlegen

Klick im **Content Drawer** setzt ein Element dorthin, wohin die Kamera blickt (auf ein Objekt, sonst
den Boden, sonst 10 m voraus). **Ziehen** in den Viewport setzt es genau an die Stelle. Oder: **+
Add**, **Place Actor ▸**, `spawn <id>`. Neue Actors heißen eindeutig („Cube“, „Cube 2“) und sind
gewählt. Etwas aus **Players** wird der Spieler: „… is the player now - press Play (Alt+P) to control
it“.

### Eltern, Kinder und Ordner

Zum **Anhängen** wählst du den Eltern-Actor **zuletzt** und nimmst **Attach to …**, oder du ziehst im
Outliner; die Lage bleibt. **Detach** löst. **Ctrl+G** legt einen leeren Actor „Group“ in die Mitte
und hängt alles daran. **Ordner** gibt es über das Feld Outliner folder, den Ordner-Knopf oder
Ziehen; **Select Children** wählt die Kinder.

Ein Kind ohne eigenen Rigid Body wird in der Simulation mit dem Körper seines Elternteils
verschweißt. Formen mit Kollision, Pickups und Trigger ohne Körper bekommen einen festen, mit Rotator
oder Mover einen „kinematischen“, der sich bewegt, aber nicht schubsen lässt
([Kapitel 6](06-physik.md)).

### Komponenten

**+ Add** bietet nur an, was noch fehlt; Collider und Constraints gehen mehrfach, Procedural City
nur bei einem Actor ohne Eltern.

| Komponente | Wichtigstes |
|---|---|
| Static Mesh | Form (Box, Sphere, Ramp …), Kollision, Material, Textur |
| Point Light | Farbe, Stärke, Reichweite |
| Rigid Body | Mobility (Static, Kinematic, Dynamic), Masse, Dämpfung |
| Collider | Form, Dichte, Reibung, Sensor, Kollisionskanäle |
| Constraint | Gelenktyp (Hinge, Slider, Ball …), Grenzen, Motor, Feder, Bruchkraft |
| Player Controller | Hover car, Rolling ball, Character, Flyer, Vehicle, Soldier, Walker; Tempo, Kamera |
| Weapon | Rifle, Pistol, Shotgun, Sniper rifle; Schaden, Munition, Streuung |
| Health | Gesundheit, Rüstung, Respawn, Explosion |
| Pickup | Punkte, Respawn, Heilung, Munition |
| Trigger | Goal, Hazard, Checkpoint, Message, Jump pad, Teleport, Damage zone, Logic event only |
| Rotator / Mover | Drehtempo / Hin- und Herbewegung (Sine, Ping-pong, Loop) |
| Procedural City | Seed, Blöcke, Häuser, Bäume, Autos |
| Audio Source | Sound, Lautstärke, Schleife, 3D, Reichweite, Doppler |

Alle Feldnamen stehen im blauen Buch, ihre Bedeutung in den Kapiteln 6, 7 und 8.

### Gelenke mit „Connect“

Wähle den Actor, der **hängen** soll, dann mit Shift oder Ctrl den, **an dem** er hängt, als Letztes.
Rechtsklick › **Connect with Constraint ▸** › Hinge, Slider, Ball, Fixed (weld), Distance / rope,
Wheel oder Motor. Studio setzt den Anker auf halbem Weg (außer bei Distance und Wheel) und gibt dem
hängenden Actor einen Rigid Body, falls nötig.

> **Mission:** Platziere im Basic-Level einen **Cube** und zieh ihn 4 m nach oben. Platziere eine
> **Physics Sphere** etwas daneben auf halber Höhe. Wähle erst die Kugel, dann mit Ctrl den Würfel,
> Rechtsklick › **Connect with Constraint › Distance / rope**, **Alt+S**: ein Pendel!

### Der Spieler, Fehler und Warnungen

Play steuert einen Actor mit **Player Controller** – festgelegt durch Platzieren aus **Players**,
Rechtsklick › **Make Player (control it in Play)** oder World Settings › **Player**. Gibt es keinen,
simuliert Play nur und meldet „Nothing to control yet“.

Vor dem Start prüft Studio das Level. **Fehler** wie ein Gelenk zu einem gelöschten Actor oder
ungültige Werte verhindern den Start: „Fix the errors above to play“. **Warnungen** stehen nur im
Output Log.

---

## 11.7 Undo und History

Jeder Schritt speichert das **ganze Level**. Undo springt einfach zum vorigen Stand – keine
Rückwärts-Rechnung, die schiefgehen könnte, und das Ergebnis hängt nie vom Timing ab. Studio hält
**500 Schritte**. **Edit › History...** springt per Klick zu jedem. Ein Ziehvorgang ist ein Schritt,
eine Änderung ohne Wirkung keiner. In Play und Simulate ist das Level gesperrt: „Stop playing to edit
the level“.

> **So kannst du es dir vorstellen:** Wie Spielstände. Statt rückwärts zu laufen, lädst du den alten
> Stand – garantiert genau wie damals.

---

## 11.8 Content Drawer und eingebaute Inhalte

Der Content Drawer (Ctrl+Space) hat vier Tabs und ein Suchfeld. In der Konsole heißen die Elemente
mit ihrer ID in Klammern.

**Players:** Soldier (`soldier`, Ego-Shooter, online-fähig), Character (`character`, laufen,
springen, Stufen steigen), Hover Car (`hover_car`), Rolling Ball (`rolling_ball`), Drone (`drone`),
Micro Duck (`micro_duck`) und Micro Duck (Rollers) (`micro_duck_rollers`, auf Rollschuhen).

**Gameplay:** Orb (`orb`, 1 Punkt), Star (10 Punkte), Health Pack (heilt 50), Ammo Box (60 Schuss),
Goal (gewinnt), Hazard (verliert), Damage Zone (25 Schaden je Eintritt), Checkpoint, Jump Pad,
Teleporter (A nach B), Message Trigger (`message`), Moving Platform, Spinner, Target Dummy und
Explosive Barrel.

**Shapes:** Cube (1 m, mit Kollision), Sphere, Cylinder, Cone, Capsule, Plane (10 × 10 m), Torus,
Ramp und Empty Actor (`empty`).

**Physics:** Physics Cube, Physics Sphere, Physics Barrel, Crate Stack (10 Kisten),
Pendulum, Hinged Door, Chain (8 Glieder), See-Saw und Sock (`micro_duck_sock`), eine Socke im Maßstab
der Micro Duck. **Lights:** Point Light. **Audio:** Audio Source, Ambient Wind, City Ambience, Radio.
**World:** Procedural City (`city`). Wo keine ID dabeisteht, ist sie der kleingeschriebene Name mit
`_` statt Leerzeichen oder Bindestrich, etwa `hover_car` oder `see_saw`.

> **Für Profis:** Mit einer Micro Duck stellt Studio die Physik auf **100 Hz mit 16 Sub-Steps**. Ein
> Roboter mit Servos ist eine lange Gelenkkette; mit zu wenig Zwischenschritten würde sie sich
> dehnen wie Kaugummi. Und das Netz der Ente braucht seinen festen Takt.

**Textures:** **Import image** (oder Bild auf die Seite ziehen) holt PNG, JPEG oder WebP, über 2048
px verkleinert. Gewählte Formen bekommen die Textur, ihre Farbe wird weiß. Doppelklick setzt eine
Textur auf die Auswahl, der Papierkorb entfernt sie. Texturen stecken direkt im Level.

**Sounds:** Klick spielt an, Doppelklick setzt den Sound auf die Auswahl (mit Audio Source), Ziehen
in den Viewport platziert eine Audio Source. **Import audio** nimmt WAV, MP3, OGG, M4A, FLAC und mehr,
bis 24 MB. Ton startet erst nach deinem ersten Klick – eine Browser-Regel.

**Output Log:** die letzten 200 Meldungen mit Uhrzeit, Warnungen und Fehler farbig; in Play auch
Spielereignisse.

---

## 11.9 World Settings

Der Tab **World Settings** (schmal: **World**) gilt fürs ganze Level:

- **Game:** Level name, Game title, Player, **Observer camera**, **Win condition** (None, Collect all
  pickups, Reach a goal trigger, Survive the time limit, Reach a target score), Time limit, Target
  score und **Kill Z** – fällt der Spieler darunter, wird er zurückgesetzt.
- **Sky and light:** Time of day (0 bis knapp 1), Day length (0 = steht still), Sonnenrichtung,
  Nebel, Belichtung, Ambient Occlusion, Shadow distance (2–2000 m, Standard 80; kleiner = schärfer),
  Wolken, endloser Boden, Bodenfarbe.
- **Physics:** Gravity, Sub-steps (1–16), Bodenreibung und -sprungkraft, Kontaktwerte, Step rate
  (15–480 Hz).
- **Audio:** Gesamtlautstärke, Musik, Hintergrundklang, Hall, Doppler.

---

## 11.10 Der Logik-Editor

Jeder Actor und das Level können einen **Logik-Graphen** tragen, wie die Blueprints in Unreal:
Knoten und Drähte statt Codezeilen. Du öffnest ihn mit **Logic** in der Toolbar (Level), **Logic** in
Details oder **Edit Logic / Add Logic (Blueprint)** im Kontextmenü (Actor). Er hat eine Toolbar
(**Viewport**, **Compile**, Save, Find, **Templates**, **C++**, Hilfe), die Liste **My Blueprint**
(Graphen, Variablen mit Live-Wert, Events), den Graphen, ein Details-Feld und **Compiler Results**.

Ein leerer Graph bietet **Begin Play**, **On Interact**, **On Begin Overlap**, **Event Tick** und
**Templates…**:

| Vorlage | Was sie tut |
|---|---|
| Door that opens on Interact | Taste F öffnet und schließt, mit Ton |
| Score zone (overlap +10) | Betreten gibt 10 Punkte, „+10“, Ton |
| Spawner (a cube every 3 s) | alle 3 s ein Physics Cube, höchstens 10 |
| Traffic light (state machine) | Ampel Grün, Gelb, Rot |
| Robot: walk and turn away with its ToF | Micro Duck dreht unter 1,6 m Abstand ab |

> **Mission:** Öffne **Basic**, klick **Logic**, dann **Begin Play**. Zieh vom Ausgang „then“ einen
> Draht ins Leere, tipp „show message“, Enter, und trag „Hallo Welt!“ ein. **Viewport**, einen
> **Rolling Ball** platzieren, **Play**: Dein Text steht im HUD.

Beim Start von Play oder Simulate übersetzt Kern jeden Graphen in WebAssembly-Maschinencode.

> **Vorsicht, Falle:** Ein Graph mit Fehlern hält Play **nicht** auf. Kern schaltet beim Start nur
> diesen Graphen ab, meldet „Logic error: …“ im Output Log, und der Rest läuft weiter. Passiert
> „nichts“, schau ins Log – oder drück vorher **F7**, dann stehen die Fehler in den Compiler
> Results.

**C++** lädt den Graphen als lesbaren C++-Text herunter ([Kapitel 12](12-eigene-spiele.md)). **F9**
setzt Breakpoints, an denen das Spiel anhält. Alles Weitere: [Kapitel 9](09-logik.md).

---

## 11.11 Play und Simulate

Studio kennt **Editing**, **Simulating** und **Playing**. Läuft etwas, ist das Level gesperrt; was in
der Simulation geschieht, verschwindet beim Stop – außer mit **Keep Simulation Changes**.

**Play** (Alt+P) startet das Spiel im aktiven Viewport: Die Physikwelt entsteht, die Kamera folgt
deinem Spieler, das HUD zeigt Titel, Pickups, Punkte, Zeit, Bestzeit und Tempo mit Boost-Balken
(beim Soldaten Fadenkreuz, Gesundheit, Munition). Am Computer wird die Maus gefangen; ist sie frei,
heißt es „Click the viewport to control the game“. Sieg oder Niederlage zeigen „Victory“ oder „Game
over“ mit **Play again**; Ctrl+R startet neu, Esc beendet.

**F8 (Eject)** lässt den Spieler los: Das Spiel läuft weiter, du fliegst frei, wählst aus und
greifst Körper (Zustand Simulating). Noch einmal **F8** übernimmt ihn wieder.

![Simulate mit eingeblendeten Kollisionsformen](../buch/bilder/studio-simulate.png)

**Simulate** (Alt+S) lässt Physik und Regeln laufen, ohne Spieler, mit der Editor-Kamera.

- **Greifen:** Linke Maustaste oder Finger auf einen beweglichen Körper. Ziehen bewegt, nach oben
  ziehen hebt, Mausrad oder zweiter Finger holt näher oder schiebt weg, Loslassen **wirft**. Ein
  kurzer Klick wählt nur aus.
- Eine **Micro Duck am Rumpf** hält der Griff gerade wie eine Hand; so hochgehoben pausiert sie wie
  der echte Roboter.
- **K** übernimmt die Lage der gewählten Actors ins Level und beendet die Simulation. **Pause** und
  **.** gehen Schritt für Schritt.
- Show › **Collision Shapes**, **Constraints** und **Contact Points** zeigen Formen, Gelenke und
  Berührungen; die Statusleiste zählt Körper, Gelenke und Kontakte.

> **Für Profis:** Der Griff ist kein Teleport: Ein Motor-Gelenk mit Feder (7,5 Hz, „kritisch
> gedämpft“, also ohne Nachschwingen) zieht den Punkt zum Zeiger. Darum stößt der Körper an und
> kippt wie jeder andere.

> **Mission:** Öffne **Physics Lab**, **Alt+S**. Wirf eine Kiste auf einen Stapel und schalte
> **Show › Contact Points** ein. Wähle eine schön schief liegende Kiste und drück **K**.

**Observer camera** (World Settings) startet Play mit freier Kamera – das Level Micro Duck nutzt
das. Du fliegst mit W A S D ohne Maustaste (am Handy schaut ein Finger, zwei
zoomen), greifst Körper wie in Simulate, und **F8** übernimmt den Spieler.

---

## 11.12 Handy, Touch und Gamepad

Die Engine schaltet von selbst auf das zuletzt benutzte Gerät: Finger auf Touch, Tastendruck auf
Computer (eine Maus am Tablet behält die Touch-Knöpfe), Gamepad ab Knopfdruck oder Stick über 0,22.

![Play am Telefon im Hochformat: EXIT, PAUSE und die Touch-Sticks](../buch/bilder/phone-play-390.png)

**Play am Touchscreen** füllt den Bildschirm; oben links stehen **⟵ EXIT** und **‖ PAUSE**. Unten
liegen **zwei Sticks** (168 px, notfalls kleiner bis 72 px): links bewegen (**MOVE**, bei Fahrzeugen
**STEER / GAS**, beim Flyer **FLY**), rechts schauen (**LOOK**); über den Ring hinaus dreht die
Kamera weiter. Linker Stick ganz nach vorn ist Sprint. Bildschirm ziehen schaut; beim Soldaten mit
Waffe feuert dabei ein Finger. Dazu kommen Knöpfe:

| Spielfigur | Knöpfe |
|---|---|
| Soldier | JUMP, RELOAD, AIM, CROUCH, USE |
| Character | JUMP, USE |
| Rolling ball | JUMP, BOOST |
| Flyer | UP, DOWN, BOOST |
| Hover car, Vehicle | BOOST, HOP, USE |
| Walker (Micro Duck) | QUACK, PICK, SIT, KICK L, KICK R, ROLL, BEAK, WHEEE, HOP |

Sprung-, Boost- und Waffenknöpfe gibt es nur, wenn die Figur das kann. Hebt sich der letzte Finger,
lässt die Engine alles los.

**Gamepad** (Xbox-Belegung): linker Stick bewegen, rechter schauen, RT feuern, LT zielen, A
springen, B ducken, X nachladen, Y benutzen, L3 oder LB Sprint/Boost, beim Flyer RB/LB hoch/runter,
Start Pause.

---

## 11.13 Die Micro Duck steuern

Die Micro Duck ist der zweibeinige Roboter von Pollen Robotics und läuft im Studio auf ihren echten,
trainierten Netzen ([Kapitel 13](13-micro-duck.md)).

**Das Beispiel-Level** ist ein Zimmer in ihrem Maßstab mit Möbeln, Ball und zwei Socken. Die Ente
spielt selbstständig Ball wie in Pollens Original-Software; mit **Kern extras** (hier an) übernimmt
Kerns Spielschicht mit Ball und Socken. Der
Level-Blueprint lässt sie quaken, wenn ihre Kopfkamera den Ball sieht. Play startet mit Observer
camera, **F8** übernimmt sie.

**Ohne Eingabe** (Walker › **Autonomous**) erkundet sie den Raum mit Abstandssensor (ToF) und
Kamera, weicht aus und steht nach einem Sturz selbst auf. Hochgehoben pausiert sie („Held“).

| Taste | Bewegung |
|---|---|
| W / S, A / D | laufen, drehen |
| Leertaste | quaken |
| 1 / 2 | sitzen-aufstehen / aufheben |
| 3 / 4 | links / rechts kicken |
| 5 / 6 | rollen / Schnabel |
| 7 | springen (Kerns `jump.onnx`) |
| V | „Wheee“-Fahrt, solange gehalten |

Am Touchscreen ist **HOP** der Sprung, **QUACK** quakt, **WHEEE** fährt, der Rest entspricht 1–6. Am
Gamepad quakt RT, LT startet Wheee, B setzt, Y hebt auf, X rollt. Logik steuert sie mit den Knoten
der Gruppe **Robots**.

Im Spiel zeigt Studio oben rechts, was die Ente sieht (Show › **Robot Camera**): **HEAD CAMERA** mit
ihrem Zustand und **TOF 8×8**, das Gitter ihres Abstandssensors – rot ein Hindernis (heller =
näher), grün Boden, gelb zu nah, dunkel nichts, dazu der kleinste Abstand oder „clear“.

> **Mission:** Öffne das Level **Micro Duck**, drück **Play** und schau zu. Dann **F8** und Taste
> **7**. Lauf mit W auf eine Wand zu und beobachte, wie das ToF-Gitter rot wird.

---

## 11.14 Policies laden

Die Bewegungen der Ente sind **neuronale Netze** im Format ONNX. Das Laufnetz `velstand.onnx` ist
eingebaut. Pollens Netze (sitzen, aufheben, kicken, rollen, Rollschuhe) kommen aus dem Ordner
`microduck/`, Kerns eigene aus `policies/`: `jump.onnx` für den Sprung und `play.onnx` für die
Spielschicht mit Ball und Socke (nur mit Kern extras). Die Engine meldet, was fehlt, die Seite holt
es, die Engine prüft es. Scheitert eine Datei, versucht die Seite es nach 5, 10, 20 s … erneut,
höchstens jede Minute; bis dahin bleibt die Ente stehen und sagt, warum.

> **Krass, aber wahr:** Was du im Kern Lehrer ([Kapitel 14](14-lehrer.md)) trainierst, lädt Studio
> im selben Browser sofort – ohne eine Datei zu kopieren.

Wer ein exportiertes Spiel mit Ente veröffentlicht, legt `microduck/` und `policies/` neben die
HTML-Datei, dazu Pollens Lizenz und NOTICE. Das Aussehen der Ente ist nur **nicht kommerziell**
erlaubt ([Kapitel 12](12-eigene-spiele.md)).

---

## 11.15 Speichern, Laden, Autosave und Einstellungen

Ein Level ist eine JSON-Textdatei `.klevel` (`"format": "kern-level"`, `"version": 1`) mit Name,
Welt, Kamera, Actors, Texturen, Sounds und Logik – Bilder und Sounds stecken mit drin. Dateien einer
neueren Studio-Version lehnt Studio ab. Das Format beschreibt [Kapitel 8](08-spiel-laufzeit.md).

**Save** (Ctrl+S) schreibt in Chrome und Edge nach dem ersten Dialog direkt in dieselbe Datei, sonst
als Download. **Save As** fragt immer, **Open** (Ctrl+O) öffnet `.klevel` und `.json`. Auf die Seite
gezogen werden Levels geöffnet, Bilder zu Texturen und Audio zu Sounds. Der Dateiname ist der
Levelname, alles außer Buchstaben, Ziffern, `-` und `_` wird zu `_`. Auf Android filtert der Dialog
nicht (er kennt `.klevel` nicht); ist es kein Level, meldet Studio „… is not a Kern level“.

**Autosave:** Spätestens 2 s nach einer Änderung landet eine Kopie im Browser – nur solange etwas
ungespeichert ist, immer höchstens eine. Speichern löscht sie. Beim Verlassen, im Hintergrund und vor
Updates wird sofort geschrieben, in Play und Simulate erst nach Stop. Beim nächsten Start: **Restore**
(lädt die Kopie, bleibt aber ungespeichert) oder **Dismiss** (löscht). Getrennt davon merkt sich
Studio das zuletzt bearbeitete Level.

> **Vorsicht, Falle:** Der Autosave lebt **nur in diesem Browser**. Neuer Rechner oder gelöschte
> Browserdaten – weg. Echte Sicherheit gibt nur **Ctrl+S**.

**Einstellungen:** Studio merkt sich **nur, was du selbst geändert hast**; alles andere folgt den
Standards der jeweiligen Version. **Edit › Preferences** bietet Camera speed (1–8, Standard 4), Mouse sensitivity (0,1–5,
Standard 1), Invert mouse Y, Orbit around selection und die drei Snaps. **Reset Settings to
Defaults** setzt alles zurück, Levels bleiben unberührt.

---

## 11.16 Spiel exportieren

**File › Package Game (HTML)...**: **Game title** prüfen, **Package** klicken. Studio lädt
`<Titel>.html` herunter und meldet „Packaged … (n MB)“ mit **Play it**.

Die Datei ist ein **komplettes Spiel** mit Engine, Spiel-Host und Level. Sie braucht weder Server
noch Internet und läuft sogar per Doppelklick – sofort im Play, ohne Editor. Sie enthält Pollens
3D-Modelle der Ente (CC BY-NC-SA 4.0, nicht kommerziell), außer die Engine wurde kommerziell gebaut.

Bedienung wie in Play; Esc gibt die Maus frei und pausiert, P pausiert, R oder Enter nach dem Ende
und Ctrl+R starten neu. Touchscreens bekommen Sticks und oben links ↻. Online geht mit `?host=raum`, `?join=raum` und
`&relay=` ([Kapitel 10](10-netzwerk.md)). Die nachladbaren Netze der Ente stecken **nicht** drin:
Laufen geht, für alles andere gehören `microduck/` und `policies/` neben die Datei.

> **Mission:** Gib deinem Treppen-Level aus 11.5 einen Rolling Ball als Spieler und exportiere es.
> Klick **Play it** – und schick die Datei einem Freund.

---

## 11.17 Multiplayer

**Play › Number of Players › 2 Players (listen server)**, dann **Play**: Studio schaltet auf vier
Viewports – 1 ist der Server, 2 der erste Client, jeder mit eigenem HUD. Steuern kann nur der aktive
Viewport. **Network Emulation** macht die Leitung schlechter, die Statusleiste zeigt Rolle, Spieler,
Ping, Verlust und Datenrate. Ein Bild findest du in [Kapitel 10](10-netzwerk.md).

**Online:** Host und Gast öffnen dasselbe Level, wählen **Host Online Game...** bzw. **Join Online
Game...**, denselben **Room** (Standard „kern“) und, für verschiedene Rechner, dasselbe **Relay**
(`node tools/net_relay.mjs`, Port 8790, Paket `ws`). Ohne Relay finden sich Tabs desselben Browsers;
fehlt der Host, meldet Join das nach 4 s. Logik läuft nur beim Host; ihre Änderungen erreichen alle,
auch Nachzügler.

---

## 11.18 Schmale Bildschirme und Handys

Bis 900 px Breite oder auf Touch-Geräten mit höchstens 600 px Höhe wird Studio kompakt:

- Der Viewport bekommt die ganze Fläche. Outliner, Details und World öffnen sich über **Panels** als
  Blatt mit großen Tabs – quer rechts, hochkant von unten (64 % der Höhe).
- **Play und Simulate** liegen mit Panels in einem **Dock** rechts in der Toolbar, das immer stehen
  bleibt, während der Rest seitwärts scrollt. Im Spiel stehen dort Pause, Stop und Eject; alle Knöpfe
  sind mindestens 38 px groß.
- Snap-Auswahl, Konsole und Physik-Zahlen verschwinden; die Seite scrollt nie seitwärts.

![Studio in einem 820 px breiten Fenster](../buch/bilder/studio-narrow.png)

![Telefon im Hochformat: das Dock mit Play, Simulate und Panels](../buch/bilder/phone-dock-390.png)

---

## 11.19 Architektur: Wie Studio gebaut ist

Studio ist eine C++-Klasse, `kern::studio::Studio`, die alles besitzt: Level, History, Auswahl,
Viewports, Menüs, Play und Netzwerkspiel. Die Webseite enthält
**keine** Editorlogik. **Warum?** Erstens läuft im Editor genau der Code deines exportierten Spiels –
es gibt keine zweite Version, die abweichen könnte. Zweitens ist er ohne Browser testbar.

Die Seite spricht Studio über C-Funktionen an, die mit `kern_studio_` beginnen. Und:
**Alles, was du tust, ist ein Befehl** – ein JSON-Objekt mit `"cmd"`, auch jeder Menüeintrag.

```json
{"cmd": "spawn", "item": "physics_cube"}
{"cmd": "set", "path": "transform.position",
 "value": [0, 0, 2]}
```

Die erste Zeile platziert eine Physikkiste. Die zweite (auf zwei Zeilen verteilt) setzt die Position
des gewählten Actors: `path` folgt seiner Textform, `value` ist der neue Wert. Es gibt Befehle für
Auswahl, Actors, Eigenschaften, History, Play, Netz, Ansichten, Panels, Dokumente, Ton,
Einstellungen und Logik – alle im blauen Buch und in [Kapitel 17](17-referenz.md).

> **So kannst du es dir vorstellen:** Studio ist die Konsole, die Webseite der Controller. Der
> Controller drückt nur Knöpfe und zeigt an, was die Konsole sagt.

Die Seite `studio.mjs` lässt jeden Frame die Engine rechnen, erledigt deren Aufträge (Maus fangen,
Dialoge, Dateien) und malt Panels nur neu, wenn sich etwas geändert hat. Sie macht nur, was ein
Browser kann und C++ nicht. `game.mjs` ist zugleich der Host jedes exportierten Spiels. Über
`window.KernStudio` bedient ein Browser-Test Studio automatisch; aus ihm stammen die meisten
Studio-Bilder dieses Buchs.

---

## Level geschafft!

- Du kennst alle Bereiche von Studio, die Menüs und die wichtigsten Tasten.
- Du fliegst durch Viewports und bewegst Actors mit Gizmo und Raster.
- Du baust Actors aus Komponenten und verbindest sie mit Gelenken.
- Du testest mit Play, Simulate, Eject und Keep und weißt, wo Fehler stehen.
- Du weißt, dass ein kaputter Logik-Graph nur sich selbst abschaltet.
- Du steuerst die Micro Duck, speicherst, exportierst und spielst zu zweit.
- Du weißt, warum der Editor C++ ist und jede Aktion ein Befehl.

## Rätsel-Ecke

1. Mit welcher Taste wählst du das Drehwerkzeug?
2. Du ziehst mit der linken Maustaste über einen Würfel. Bewegt er sich?
3. Play ohne Actor mit Player Controller – was passiert?
4. Dein Logik-Graph hat einen Fehler. Startet Play? Wo steht die Meldung?
5. **Boss-Rätsel:** Grid-Snap 0,5 m, ein Würfel bei x = 0. Du hältst Alt und ziehst in Weltachsen am
   roten Pfeil 1,3 m nach rechts. Wie viele Würfel gibt es, bei welchem x landet der bewegte, und wie
   oft musst du Ctrl+Z drücken?

### Lösungen

1. **E** (Rotate).
2. **Nein** – linkes Ziehen zeichnet ein Auswahlrechteck. Bewegen tut nur der Gizmo.
3. Studio **simuliert** stattdessen und meldet „Nothing to control yet“.
4. **Ja.** Nur dieser Graph wird abgeschaltet; „Logic error: …“ steht im **Output Log**.
5. **Zwei** Würfel, das Original bleibt bei 0. Die Kopie rastet aufs nächste Vielfache von 0,5 m:
   1,3 liegt näher an 1,5 als an 1,0, also **x = 1,5 m**. **Ein** Ctrl+Z genügt – Kopie und
   Bewegung sind ein Schritt.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
