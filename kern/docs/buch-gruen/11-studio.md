# 11 Kern Studio: Deine Werkstatt im Browser

> **Kurz gesagt:** Kern Studio ist der Editor der Kern-Engine. Du baust darin Levels aus Actors und
> Komponenten, spielst sie mit einem Klick direkt im Fenster und machst daraus eine einzige HTML-Datei,
> die überall läuft. Dieses Kapitel ist eine geführte Tour: Mach am besten Studio auf und klick mit.

Kern Studio arbeitet wie die Unreal Engine (Kern ist kein Produkt von Epic Games). Wer Unreal kennt,
fühlt sich sofort zu Hause; wer es nicht kennt, lernt hier Begriffe, die in der Spielebranche jeder
versteht. Ein Level besteht aus **Actors** (allem, was in der Welt steht), jeder Actor trägt
**Komponenten** (Fähigkeiten wie „hat eine Form“, „ist ein Licht“, „fällt runter“). Bearbeitet wird in
**Viewports** (Fenstern in die 3D-Welt), im **Outliner** (der Liste aller Actors), in **Details** (den
Eigenschaften) und in den **World Settings** (den Einstellungen fürs ganze Level).

> **Wusstest du?** Die ganze Editorlogik von Studio ist C++ und läuft im selben WebAssembly-Modul wie
> Renderer, Box3D-Physik und Spiel-Laufzeit. Was du im Viewport siehst, rechnet also **genau der Code**,
> der später dein exportiertes Spiel ausführt. Die Browserseite malt nur die Panels und reicht deine
> Klicks weiter (11.19).

Was du im Studio zusammenbaust, nutzt die Logik-Graphen aus [Kapitel 9](09-logik.md), die Physik aus
[Kapitel 6](06-physik.md), den Ton aus [Kapitel 7](07-audio.md) und die Spiel-Laufzeit aus
[Kapitel 8](08-spiel-laufzeit.md). Wie du daraus ein eigenes Spiel machst, zeigt
[Kapitel 12](12-eigene-spiele.md).

![Kern Studio mit dem Level Orb Run: Menüleiste, Toolbar, Viewport, Outliner und Details](../buch/bilder/studio.png)

> **Achtung, Falle!** Auf einem deutschen System zeigt Studio deutsche Beschriftungen („Datei“,
> „Welteinstellungen“ …). Dieses Kapitel nennt die englischen. Klick unten rechts in der Statusleiste
> auf **DE | EN** (Studio merkt sich die Wahl) oder öffne Studio mit `?lang=en` am Ende der Adresse.
> Befehle, Konsole, Level-Dateien und alles, was du selbst eintippst, sind in beiden Sprachen gleich.

---

## 11.1 Studio starten

Es gibt drei Wege zum Studio:

| Weg | Was du tust | Wofür |
|---|---|---|
| Live | auf https://kernengine.de/ den Editor öffnen | sofort loslegen, auch am Telefon |
| Eine Datei | `generated/studio.html` bauen und öffnen | offline, ohne Server |
| Entwicklung | Repository ausliefern, Studio-Seite öffnen | am Studio selbst arbeiten |

Die Startseite des Projekts verlinkt außerdem den Lehrer, die beiden Beispielspiele und zwei Levels zum
direkten Spielen (Orb Run und Physics Lab). Ihre Fußzeile nennt den Rechteinhaber und verlinkt Lizenz
(Apache 2.0), Bestandteile Dritter, Quellcode, Dokumentation, Impressum und Datenschutz.

**Was beim Start passiert:** Ein Splash-Bildschirm „Starting the engine“ erscheint, während das Modul
lädt. Beim allerersten Mal ist **Orb Run** geladen: eine prozedurale Stadt (vom Computer nach Regeln
erzeugt) mit Verkehr, einem Box3D-Hovercar, 24 Energie-Orbs und einem Tag, der in die Nacht übergeht.
Das Output Log meldet „Kern Studio ready - Orb Run loaded“.
Später öffnet Studio das Level, an dem du zuletzt gearbeitet hast, und bietet mit **Start fresh** an,
doch wieder mit Orb Run zu beginnen. Gab es ungespeicherte Änderungen, fragt es **Restore „…“ from your
last session?** (11.15).

> **Probier's aus!** Öffne Studio und drücke **Alt+P** (Play). Schon fährst du mit dem Hovercar durch
> Orb Run: W A S D lenkt, Shift boostet. **Esc** bringt dich zurück in den Editor.

### Als App installieren

Studio ist eine installierbare Web-App. Chrome und Edge zeigen oben einen Knopf **⤓ Install**; auf
iPhone und iPad erklärt derselbe Knopf die zwei Schritte **Teilen** und **Zum Home-Bildschirm**.
Installiert startet Studio im Vollbild (Notch und Statusleiste bleiben frei) und funktioniert auch
offline mit der zuletzt geladenen Version; ist Netz da, holt es die Seite trotzdem zuerst frisch.

Studio sucht nach einer neuen Version: beim Start, bei jeder Rückkehr zur App, wenn das Netz
zurückkommt, und alle 60 s. Kann nichts verloren gehen (du bearbeitest, nichts ist ungespeichert),
lädt es sofort neu; sonst fragt es **New version available** mit **Update** und **Later**, und dein
Level wandert über den Autosave mit. Unten rechts in der Statusleiste steht die laufende Version, mit
Hinweis, wenn online eine neuere bereitliegt.

---

## 11.2 Die Oberfläche im Überblick

> **Stell dir vor …** eine Werkstatt. In der Mitte die Werkbank mit deinem Modell (der Viewport). Rechts
> an der Wand die Inventarliste (Outliner) und darunter der Zettel mit den Maßen des Teils in deiner Hand
> (Details). Unten eine Schublade voller Bauteile (der Content Drawer). Und ganz oben das Regal mit den
> Werkzeugen (Toolbar).

| Bereich | Lage | Was drin ist |
|---|---|---|
| Titelleiste | oben | Menüs, Levelname, Bilder pro Sekunde |
| Toolbar | darunter | Speichern, Undo, **Add**, Werkzeuge, Raster, Play |
| Viewports | Mitte | 1, 2 oder 4 Ansichten |
| Outliner | rechts oben | alle Actors als Baum |
| Details / World Settings | rechts unten | Eigenschaften als zwei Tabs |
| Content Drawer | unten, klappbar | Bauteile, Texturen, Sounds, Log |
| Statusleiste | ganz unten | Meldungen, Konsole, Physik, Ton |

Die Trennlinien zwischen Viewport und Seitenleiste (Seitenleiste 240 px bis 60 % der Breite) und
zwischen Outliner und Details kannst du ziehen; Studio merkt sich die Größen und ob der Content Drawer
offen war. Ein Punkt neben dem Levelnamen heißt: Es gibt ungespeicherte Änderungen.

### Die Menüleiste

Fünf Menüs: **File**, **Edit**, **Window**, **Play**, **Help**. Ist eins offen, wechselst du durch bloßes
Überfahren zum nächsten. Untermenüs klappen **neben** dem Menü auf, nie darüber, sodass nie ein Menü
unter dem Finger liegt, der es geöffnet hat. Ist daneben kein Platz oder tippst du (Finger, Stift),
ersetzt das Untermenü das Menü an Ort und Stelle, mit einer Zurück-Zeile; Menüzeilen sind am
Touchscreen 40 px hoch. **Esc** schließt jedes Menü. Einträge, die gerade nichts tun könnten
(etwa **Delete** ohne Auswahl), sind ausgegraut.

- **File:** New Level (Ctrl+N), Open (Ctrl+O), Save (Ctrl+S), Save As, Import Texture, Package Game,
  **Example Levels** (Orb Run, Physics Lab, Shooting Range, Micro Duck, Basic, Empty).
- **Edit:** Undo und Redo (mit dem Namen des Schritts), **History…**, Cut, Copy, Paste, Duplicate,
  Delete, Select All, Deselect, Group, Snap to Floor, Preferences und Reset Settings to Defaults.
- **Window:** Layout, Ansichtstyp, View Mode, Show-Flags, Game View, Focus Selection, Content Drawer,
  Output Log, Vollbild (F11).
- **Play:** Play, Simulate, Pause, Advance One Frame, Stop, Eject/Possess, Restart Game, Keep
  Simulation Changes, Mehrspieler (11.17) und Online-Spiel.
- **Help:** Controls (F1) und About Kern Studio.

**New Level** bietet sechs Vorlagen als Karten (Doppelklick erstellt sofort):

| Vorlage | Was drin ist |
|---|---|
| Orb Run | Nachtstadt, Hovercar, 24 Orbs |
| Physics Lab | jedes Box3D-Gelenk: Scharnier, Schiene, Rad, Seilbrücke, Motor, Stapel |
| Shooting Range | Ego-Shooter-Soldat, Ziele, Deckung, mehrspielerfähig |
| Micro Duck | Pollen Robotics' Laufroboter auf seinem trainierten Netz |
| Basic | Boden, Sonne, Himmel: ein sauberer Anfang |
| Empty | gar nichts |

Hat dein aktuelles Level ungespeicherte Änderungen, sagt der Dialog dir: Sie bleiben im Autosave, bis du
das neue Level bearbeitest.

### Die Toolbar

Von links nach rechts: Diskette (Speichern), Undo/Redo, **+ Add** (alle Bauteile als Menü), die vier
Werkzeuge **Select/Move/Rotate/Scale** (Q/W/E/R), der Knopf für Welt- oder lokale Achsen (Ctrl+`), drei
Rasterknöpfe mit Auswahl für Verschieben (1 cm bis 100 m), Drehen (1° bis 90°) und Skalieren (0,0625
bis 1), die Kamerageschwindigkeit (1 bis 8), die Play-Gruppe (Play, Simulate, Pause, Frame, Stop,
Eject), Layout, **Logic** (Logik-Graph des Levels), der Paket-Knopf zum Exportieren und auf schmalen
Bildschirmen **Panels**. **+ Add** ist während Play gesperrt; Play während Simulate übernimmt den
Spieler.

### Viewports

Jeder Viewport hat oben eine eigene Leiste: das **≡**-Menü (Game View, Sichtfeld 50° bis 110°, Focus,
Kamera-Lesezeichen, Layout), den **Ansichtstyp** (Perspective oder flache „orthografische“ Ansichten von
oben, unten, links, rechts, vorn, hinten), den **View Mode** (Lit, Unlit, Wireframe, Player Collision),
die **Show**-Liste (Grid, Icons, Selection Outline, Collision Shapes, Constraints, Contact Points,
Bounds, Fog, Sky, Stats; sie bleibt beim Umschalten offen) und einen Knopf zum Maximieren. Ein Klick
auf die Leiste macht den Viewport zum aktiven.

Wechselst du in eine orthografische Ansicht, schaltet Studio auf Drahtgitter (Wireframe), zurück in die
Perspektive wieder auf Lit. **Constraints** ist anfangs aus, sonst würde jede Kette und jede Tür ihre
Hilfslinien malen.

Im Viewport selbst siehst du unten links das **Achsenkreuz**: X rot, Y grün, Z blau. **Z zeigt nach
oben.** Eine Infozeile nennt Tempo und Kameraposition (in orthografischen Ansichten die sichtbare
Breite in Metern), mit dem Show-Flag **Stats** auch Bilder pro Sekunde und in der Simulation Bodies,
Kontakte und Schritte. Eine Hinweiszeile nennt die gerade
gültigen Tasten, und beim Ziehen am Gizmo steht da, wie weit du verschoben, gedreht oder skaliert hast.

> **Probier's aus!** Stell über die Toolbar **Layout** auf vier Viewports. Klick in den perspektivischen
> Viewport, stell seinen Ansichtstyp auf **Top** (Alt+J) und schau, wie er von Lit auf Wireframe
> springt; Alt+G bringt die Perspektive zurück. Mit dem Maximieren-Knopf holst du einen Viewport groß
> und wieder zurück.

### Outliner

Der Outliner listet alle Actors mit Auge (Sichtbarkeit), **Label** (Name) und **Type**. Der Typ ergibt
sich aus den Komponenten: Player, Trigger, Pickup, Point Light, Physics Body, Static Mesh, Empty und so
weiter.

- **Klick** wählt, **Shift+Klick** wählt einen Bereich, **Ctrl+Klick** fügt einzeln hinzu oder nimmt
  heraus.
- **Doppelklick** holt den Actor ins Bild (bei einem Ordner: umbenennen), **F2** benennt direkt in der
  Zeile um (Enter bestätigt, Esc bricht ab).
- Das **Auge** blendet ein und aus, bei Ordnern alles darin; der **Pfeil** klappt Kinder und Ordner auf
  und zu.
- **Ziehen** auf einen anderen Actor hängt ihn als Kind an (seine Lage in der Welt bleibt), auf einen
  Ordner verschiebt ihn dorthin, ins Leere löst ihn wieder.
- Der Ordner-Knopf legt einen Ordner „New Folder“ für die Auswahl an; das Suchfeld filtert nach Name,
  Typ und Tags.
- **Rechtsklick** öffnet das Kontextmenü wie im Viewport, bei Ordnern zusätzlich **Rename Folder** und
  **Remove Folder (keep actors)**.

Der Actor, den du in Play steuerst, trägt das Abzeichen **PLAYER**; gesperrte Actors ein Schloss. Die
Fußzeile zählt Actors und Auswahl.

### Details

Details zeigt die Auswahl: oben Name, die Knöpfe **Logic** (Graph dieses Actors) und **+ Add**
(Komponente hinzufügen), darunter ein Suchfeld und die Abschnitte **Actor**, **Transform** und je
Komponente einen eigenen. Jeder Abschnitt klappt zu; Komponenten haben einen Papierkorb. Wählst du
mehrere Actors, siehst du nur, was alle gemeinsam haben; unterschiedliche Werte zeigen „Multiple“, und
eine Eingabe gilt für alle.

Ein paar Bedienkniffe, die du lieben wirst:

- In Zahlenfelder darfst du **rechnen**: `2*3` oder `10/4+1`. Ein Komma gilt als Punkt.
- **Zieh das Label** eines Zahlenfelds seitwärts, um den Wert zu verändern. Shift macht es zehnmal so
  schnell, Ctrl zehnmal so fein. Die Pfeiltasten im Feld tun dasselbe.
- Felder von 0 bis 1 haben einen Schieberegler. Farben haben einen Farbwähler, der sofort wirkt und
  offen bleibt, solange du darin ziehst, oder einen Hex-Wert wie `#ff8800`.
- Vektoren haben drei Felder X, Y, Z; bei **Scale** sorgt ein Schloss für proportionales Skalieren.
  Auswahlfelder sind Listen (Form, Typ, Sound, Ziel-Actor …), Tags trennst du mit Kommas.
- Weicht ein Wert vom Standard ab, erscheint rechts ein Pfeil zum **Zurücksetzen**. Rechtsklick bietet
  **Reset to Default**, **Copy Value** und **Paste Value** (in Textfeldern auch Ausschneiden, Kopieren,
  Einfügen, Alles auswählen).

> **Wusstest du?** Ein ganzer Zieh-Vorgang am Label, am Regler oder im Farbwähler ist **ein einziger**
> Undo-Schritt, egal wie lange du ziehst. Studio gibt jedem zusammenhängenden Vorgang eine Kennung, und
> die Engine fasst alle Änderungen mit dieser Kennung zusammen. Und jede Änderung wird genauso geprüft,
> als käme sie aus einer gespeicherten Datei.

![Ausgewählter Actor mit Move-Gizmo und seinen Komponenten in Details](../buch/bilder/studio-selected.png)

### Statusleiste und Konsole

Unten findest du Content Drawer, Output Log, die Konsole **Cmd**, die letzte Meldung, die
Physik-Statistik (in Simulate und Play: Bodies, wache Bodies, Gelenke, Kontakte; online auch Rolle,
Ping, Verlust), den Lautsprecher (Ton an/aus), den Pegel links/rechts, die Auswahl („2 of 40
selected“) und die laufende Version.

In der Konsole tippst du Befehle; `help` listet sie. Zum Beispiel `spawn physics_cube` (ein Bauteil
setzen), `select orb` (alles mit „orb“ im Namen wählen), `tod 0.8` (Tageszeit), `gravity 0 0 -1.6`
(Schwerkraft), `camspeed 6`, `stat` und `clear`. Mit ↑/↓ blätterst du durch frühere Eingaben, Esc
verlässt das Feld.

> **Probier's aus!** Öffne die Konsole und tippe `gravity 0 0 -1.6`. Dann ein paar `spawn physics_cube`
> und **Simulate** (Alt+S). Mondschwerkraft! Ein `gravity` ohne Werte stellt die Erdschwerkraft
> (−9,80665) wieder her.

### Das Kontextmenü

Ein Rechtsklick (ohne zu ziehen) im Viewport wählt den Actor darunter und öffnet ein Menü; am Touchscreen
genügt langes Halten (mindestens 0,5 s). Darin: Focus, Rename, Cut, Copy, Paste, Duplicate, Delete,
**Edit Logic** bzw. **Add Logic (Blueprint)**, **Make Player**, Snap to Floor, Group, **Attach to …** und
**Connect with Constraint ▸** (ab zwei Actors), Detach, Hide, Lock, Select Children und **Place Actor ▸**
(Cube, Sphere, Physics Cube, Point Light, Orb oder Empty Actor genau an die geklickte Stelle). Ohne
Auswahl gibt es Paste, Select All, Show All und Place Actor; in Simulate und Play (mit Auswahl) Focus
und Keep Simulation Changes. Das Browsermenü taucht im Studio nirgends auf.

---

## 11.3 Navigation im Viewport

Die Kamerasteuerung folgt der Unreal Engine. Sie gilt immer für den Viewport unter dem Zeiger.

### Maus und Tastatur

Das Herzstück ist der **Fly Mode**: Halte die **rechte Maustaste** und du fliegst wie in einem
Computerspiel.

| Eingabe | Wirkung |
|---|---|
| Rechte Maustaste + W A S D | vor, links, zurück, rechts |
| … + Q / E | runter / hoch |
| … + Shift | 2,5-mal schneller |
| … + Mausrad | Tempo-Stufe 1 bis 8 |
| … + Z / C | heran- / wegzoomen |
| Rechte + linke Maustaste | hoch, runter, seitwärts schieben |
| Mittlere Maustaste ziehen | schwenken |
| Alt + linke / rechte / mittlere | kreisen / näher oder weg / schwenken |
| Mausrad | vor und zurück |
| F oder Doppelklick | Auswahl ins Bild holen (F ohne Auswahl: alles) |
| Ctrl+1 … 9 / 1 … 9 | Lesezeichen setzen / anspringen |

Die acht Tempostufen reichen von 1 m/s bis 160 m/s; jede ist etwa doppelt so schnell wie die vorige.
Stufe 4 (10 m/s) ist der Standard. In orthografischen Ansichten verschieben rechte und mittlere
Maustaste, das Mausrad zoomt zum Cursor.

Alt+links kreist um die Auswahl, solange die Einstellung **Orbit around selection** an ist (Standard);
ohne Auswahl kreist die Kamera um den Punkt unter dem Cursor. Während du fliegst, gehören W, E, R, Q
den Bewegungen; die Werkzeuge schalten dann nicht um.

> **Wusstest du?** Studio liest beim Fliegen die **Position** der Tasten, nicht ihre Beschriftung. Auf
> einer französischen AZERTY-Tastatur liegen W A S D also an derselben Stelle wie bei dir. Kürzel wie
> Ctrl+Z dagegen folgen der Beschriftung.

### Mit den Fingern

Am Telefon oder Tablet steuerst du die Kamera wie in einem 3D-Betrachter: **ein Finger** kreist um den
Punkt darunter, **zwei Finger** zoomen und verschieben, **tippen** wählt aus, **zweimal tippen** holt
die Auswahl ins Bild, **lange halten** öffnet das Kontextmenü. Finger auf einem Gizmo-Griff bewegen,
drehen und skalieren wie die Maus, ein Finger in einem anderen Viewport macht ihn aktiv. In
orthografischen Ansichten verschiebt ein Finger. Die Seite selbst zoomt nie mit.

> **Stell dir vor …** du zoomst mit zwei Fingern auf eine Socke und danach auf einen Häuserblock. Es
> fühlt sich beide Male gleich an, denn Studio misst den Maßstab am Abstand zu dem Punkt unter deinen
> Fingern. Was unter den Fingern liegt, bleibt dort, und der nächste Ein-Finger-Zug kreist um genau
> diesen Punkt.

---

## 11.4 Tastenkürzel

**F1** zeigt die wichtigsten Kürzel jederzeit im Studio. Die vollständige Liste steht im blauen Buch und
in [Kapitel 17](17-referenz.md); hier die, die du ständig brauchst.

**Beim Bauen:** Q/W/E/R wählt das Werkzeug, die **Leertaste** schaltet Move → Rotate → Scale weiter,
**[** und **]** machen das Raster kleiner oder größer (von 1 cm bis 100 m), **End** setzt die Auswahl
auf den Boden, **H** blendet sie aus, **Shift+H** alles andere, **Ctrl+H** alles wieder ein, **G**
schaltet die Game View (alle Helfer aus), **Esc** bricht einen Gizmo-Zug ab oder hebt die Auswahl auf.
Dazu die üblichen Ctrl+Z/Y, C/X/V, D, A, G, S, O, N, Entf oder Rücktaste löscht, F2 benennt um.
Alt+G/J/H/K wechseln die Ansicht, Alt+2/3/4/5 den View Mode.

> **Wusstest du?** Kopierte Actors landen als Text in der Zwischenablage deines Systems. Ein Ctrl+V in
> einem **anderen Tab** fügt sie dort ein. So trägst du Bauteile von einem Level ins nächste.

**Beim Spielen:** W A S D und Pfeile bewegen, die Maus schaut, die Leertaste springt, Shift boostet oder
sprintet. Der Soldat feuert mit links, zielt mit rechts, lädt mit R nach, duckt sich mit C, benutzt mit
F. **Esc** stoppt, **F8** verlässt bzw. übernimmt den Spieler, **Shift+F1** gibt die Maus frei, **Pause**
hält an, **.** geht einen Schritt weiter, **Ctrl+R** startet neu (außer beim Soldaten, da ist Ctrl
Ducken und R Nachladen). Alt+P beendet Play, Alt+S schaltet auf Simulate. Der Flyer steigt mit
Leertaste oder E und sinkt mit Q, Shift oder Ctrl; das Mausrad zoomt die Spielkamera. Der Klick, der
die Maus fängt, feuert nicht. Die Micro Duck hat eigene Tasten (11.13).

> **Achtung, Falle!** Hat der Browser die Maus gefangen, schluckt er normalerweise das erste Esc, um sie
> freizugeben. Studio erkennt das und stoppt trotzdem sofort. Ein Fensterwechsel dagegen gibt nur die
> Maus frei, ohne zu stoppen.

**Im Logik-Editor:** F7 kompiliert, Ctrl+F sucht, F9 setzt einen Breakpoint, Buchstabe plus Klick fügt
häufige Knoten ein (B Branch, S Sequence, D Delay …), C legt einen Kommentar um die Auswahl, Q richtet
Drähte gerade, Shift+W/A/S/D richtet Knoten aus, F bzw. Home rahmt Auswahl bzw. alles ein, Esc geht
zurück. Solange er offen ist, gehören Buchstaben und Entf dem Graphen; nur Ctrl+Z, Ctrl+Y, Ctrl+S und
F1 erreichen das Level.

---

## 11.5 Auswählen, Gizmos und Snapping

### Auswählen

Linksklick wählt, Shift ergänzt, Ctrl schaltet um, ein Klick ins Leere hebt die Auswahl auf. **Linkes
Ziehen zieht immer ein Auswahlrechteck**, nie die Kamera und nie einen Actor, auch wenn du auf einem
Actor anfängst. Bewegt wird nur mit dem Gizmo, genau wie in Unreal. Das Rechteck fängt jeden
sichtbaren, nicht gesperrten Actor, dessen Ursprung darin liegt; die prozedurale Stadt bleibt außen vor.

Lichter und leere Actors haben kein Mesh; du wählst sie über ihr Symbol (Show › Icons). Gesperrte Actors
lassen Klicks durch zu dem, was dahinter liegt. Der **zuletzt** gewählte Actor ist die **primäre**
Auswahl: An ihm sitzt der Gizmo.

### Gizmos

Ein **Gizmo** ist der bunte Anfasser am ausgewählten Actor:

- **Move (W):** drei Pfeile, drei kleine Quadrate (für zwei Achsen zugleich) und ein Mittelpunkt.
- **Rotate (E):** drei Ringe.
- **Scale (R):** drei Achsen und ein Mittelgriff fürs gleichmäßige Vergrößern.

Der Griff unter dem Zeiger leuchtet gelb. Mit dem Globus- bzw. Würfel-Knopf richtet sich der Gizmo nach
der Welt oder nach dem primären Actor. Bewegt werden alle ausgewählten Actors; Kinder folgen ihren
Eltern.

> **Probier's aus!** Wähle einen Würfel, halte **Alt** und zieh am Move-Gizmo. Du ziehst eine
> **Kopie** heraus, das Original bleibt stehen. Kopie und Bewegung sind zusammen ein Undo-Schritt.
> Drück während des Ziehens **Esc**, ist alles wie vorher.

### Snapping

**Snapping** heißt Einrasten. Anfangs rastet Move auf ein **0,5-m-Raster** und Rotate auf
**15°-Schritte**; das Skalierungsraster (0,25) ist aus. In Weltachsen rastet Move auf feste
Positionen, in lokalen Achsen in Schritten. Auch neu platzierte Actors landen auf dem Raster. Wählst
du in der Toolbar eine Rastergröße, schaltet sich das Raster gleich mit ein.

---

## 11.6 Levels, Actors und Komponenten

Ein Level besteht aus World Settings, Actors, Texturen, Sounds, dem Logik-Graphen des Levels und der
Editor-Kamera. Einheiten sind **Meter, Sekunden, Kilogramm**, Z zeigt nach oben, Drehungen stehen in
Grad (x Roll, y Pitch, z Yaw), Farben in sRGB. Ein Actor hat Name, Eltern-Actor, Ordner, Tags,
Hidden, Locked, Transform und Komponenten.

### Actors anlegen

1. Content Drawer öffnen (**Ctrl+Space**) und ein Bauteil anklicken: Es erscheint dort, wohin die
   Kamera blickt (auf dem getroffenen Ding, sonst am Boden, sonst 10 m voraus).
2. Oder das Bauteil in den Viewport **ziehen**: Es landet genau unter der Maus.
3. Oder **+ Add** in der Toolbar, **Place Actor ▸** im Kontextmenü oder `spawn` in der Konsole.

Neue Actors bekommen eindeutige Namen („Cube“, „Cube 2“ …) und sind gleich ausgewählt. Setzt du ein
Bauteil aus **Players**, wird es dein Spieler; das Log sagt „… is the player now - press Play (Alt+P) to
control it“.

### Eltern, Kinder und Ordner

- **Anhängen:** mehrere Actors wählen, den künftigen Elternteil zuletzt, dann **Attach to …**; oder im
  Outliner draufziehen.
- **Lösen:** **Detach** oder im Outliner ins Leere ziehen.
- **Gruppieren:** **Ctrl+G** setzt einen leeren Actor „Group“ in die Mitte und hängt die Auswahl daran.
- **Ordner** (Feld **Outliner folder** in Details oder im Outliner) helfen nur der Übersicht, für
  Actors der obersten Ebene. **Select Children** wählt alle Kinder.

Kind-Actors ohne eigenen Rigid Body werden beim Simulieren mit dem Körper ihres Vorfahren
verschweißt. Dinge mit Kollision, aber ohne Body bekommen automatisch einen unbeweglichen; drehen oder
bewegen sie sich per Rotator oder Mover, einen „kinematischen“, der sich bewegt, aber nicht schubsen
lässt ([Kapitel 6](06-physik.md)).

### Komponenten

> **Stell dir vor …** ein Actor ist eine leere Lego-Platte. Komponenten sind die Steine, die du
> draufsteckst: Ein Form-Stein macht ihn sichtbar, ein Physik-Stein lässt ihn fallen, ein Pickup-Stein
> macht ihn einsammelbar. Kombinieren erlaubt!

**+ Add** in Details bietet nur an, was noch fehlt. Collider und Constraints dürfen mehrfach vorkommen,
Procedural City nur an einem einzelnen Actor ohne Eltern. Die wichtigsten Komponenten:

| Komponente | Was sie tut |
|---|---|
| **Static Mesh** | Form (Box, Kugel, Zylinder, Kegel, Kapsel, Ebene, Ring, Rampe …), Kollision, Material |
| **Point Light** | farbiges Licht mit Reichweite |
| **Rigid Body** | Box3D-Physik: statisch, kinematisch oder dynamisch, Masse, Dämpfung |
| **Collider** | Kollisionsform und Material (Reibung, Sprungkraft) |
| **Constraint** | Gelenk: Scharnier, Schiene, Kugel, fest, Seil, Rad, Motor … |
| **Player Controller** | macht den Actor steuerbar (Hovercar, Ball, Läufer, Flyer, Soldat, Walker …) |
| **Weapon, Health** | Waffe und Lebenspunkte |
| **Pickup** | einsammelbar: Punkte, Heilung, Munition |
| **Trigger** | Zone: Ziel, Gefahr, Checkpoint, Meldung, Sprungfeld, Teleport … |
| **Rotator, Mover** | dreht bzw. bewegt den Actor hin und her |
| **Procedural City** | eine ganze Stadt mit Verkehr |
| **Audio Source** | Klangquelle im Raum |

Jede hat viele Felder (die vollständige Liste steht im blauen Buch); was sie bedeuten, erklären
[Kapitel 6](06-physik.md), [Kapitel 7](07-audio.md) und [Kapitel 8](08-spiel-laufzeit.md).

### Gelenke mit „Connect“

Schneller als ein Constraint von Hand:

1. Den Actor wählen, der **hängen** soll.
2. Mit Shift oder Ctrl den Actor dazuwählen, **an dem** er hängt, und zwar als Letztes.
3. Rechtsklick › **Connect with Constraint ▸** › Hinge, Slider, Ball, Fixed, Distance / rope, Wheel
   oder Motor.

Studio setzt den Gelenkpunkt auf halbem Weg zwischen beide (außer bei Seil und Rad) und gibt dem
hängenden Actor einen Rigid Body, falls er keinen hat. Mehrere hängende Actors bekommen je ein Gelenk
zum zuletzt gewählten.

> **Probier's aus!** Setze einen **Cube** und schieb ihn mit dem Gizmo hoch in die Luft, daneben eine
> **Physics Cube**, ebenfalls ein Stück in der Luft. Wähle erst die Physics Cube, dann mit Ctrl den Cube, und verbinde sie mit
> **Distance / rope**. Jetzt **Simulate**: Die Kiste schwingt am Seil wie ein Pendel.

### Der Spieler und die Prüfung vor dem Start

Play steuert einen Actor mit **Player Controller**. Du bestimmst ihn, indem du ein Bauteil aus
**Players** setzt, per Rechtsklick › **Make Player (control it in Play)** oder in World Settings ›
**Player**. Gibt es keinen, startet Play stattdessen eine Simulation und meldet „Nothing to control yet“.

Vor Play und Simulate prüft Studio das Level. **Fehler** (etwa ein Gelenk, das auf einen gelöschten Actor
zeigt) verhindern den Start: „Fix the errors above to play“. **Warnungen** stehen nur im Output Log.

---

## 11.7 Undo und History

Jeder Schritt speichert das **ganze Dokument** nach der Änderung. Undo springt einfach zum vorigen Stand
zurück. Das ist exakt: Es gibt keine Rückwärts-Rechnungen, die schiefgehen könnten. Studio hält bis zu
**500 Schritte**.

> **Probier's aus!** Verschieb ein paar Actors, ändere eine Farbe, lösche etwas. Dann **Edit ›
> History…**: Da steht jeder Schritt mit Namen. Ein Klick springt genau zu diesem Stand.

Eine Änderung, die nichts ändert, erzeugt keinen Schritt. Während Play und Simulate ist das Level
gesperrt: „Stop playing to edit the level“.

---

## 11.8 Content Drawer und eingebaute Inhalte

Der Content Drawer (**Ctrl+Space**) öffnet sich unter den Viewports, mit vier Tabs und einem Suchfeld.

### Place Actors

Hier liegen über 40 fertige Bauteile, sortiert in Gruppen. Ein Klick platziert, Ziehen setzt genau an
die Stelle. Der Tooltip beschreibt jedes. Eine Auswahl:

- **Players:** Soldier (Ego-Shooter, online-fähig), Character (Läufer, der springt und Stufen steigt),
  Hover Car, Rolling Ball, Drone, Micro Duck und Micro Duck (Rollers), die Ente auf Rollschuhen.
- **Gameplay:** Orb (1 Punkt), Star (10 Punkte), Health Pack (heilt 50), Ammo Box (60 Schuss), Goal
  (gewinnt), Hazard (verliert), Damage Zone, Checkpoint, Jump Pad, Teleporter (zwei verbundene Felder),
  Message Trigger, Moving Platform, Spinner, Target Dummy und Explosive Barrel.
- **Shapes:** Cube, Sphere, Cylinder, Cone, Capsule, Plane (10 × 10 m), Torus, Ramp, Empty Actor.
- **Physics:** Physics Cube, Sphere und Barrel, Crate Stack (10 Kisten), Pendulum, Hinged Door, Chain
  (8 Glieder), See-Saw und Sock, eine zerknüllte Socke im Maßstab der Micro Duck.
- **Lights, Audio, World:** Point Light, Audio Source, Ambient Wind, City Ambience, Radio und die
  Procedural City.

> **Wusstest du?** Setzt du eine Micro Duck ein, stellt Studio die Physik automatisch auf 100 Hz mit
> 16 Sub-Steps (Zwischenschritten) und meldet das im Log. Ein Roboter mit Servos ist eine lange
> Gelenkkette, die ohne genug Zwischenschritte auseinandergezogen würde, und sein Netz braucht einen
> festen Takt.

### Textures

Hier siehst du alle Texturen (Bilder für Oberflächen) des Levels. **Import image** (oder ein Bild auf die
Seite ziehen) holt PNG, JPEG oder WebP herein; über 2048 px verkleinert Studio. Ist dabei etwas
ausgewählt, bekommt es die Textur gleich, und seine Farbe wird weiß. **Doppelklick** setzt eine Textur
auf die Auswahl, der Papierkorb entfernt sie (Meshes verlieren sie dann). Texturen stecken im Level
selbst, die Datei bleibt also vollständig.

### Sounds

Ein **Klick** hört einen Sound an, ein **Doppelklick** gibt ihn der Auswahl als Audio Source, **Ziehen**
in den Viewport setzt eine Audio Source an diese Stelle. **Import audio** nimmt WAV, MP3, OGG, M4A, FLAC
und alles, was der Browser kann, bis 24 MB pro Datei; Undo nimmt einen Import zurück. Der Knopf
„Playing preview“ oben im Tab stoppt das Vorhören. Auch jedes Sound-Feld in Details hat eine
Vorhör-Taste.

> **Achtung, Falle!** Du hörst nichts? Browser erlauben Ton erst nach dem ersten Klick oder Tastendruck.
> Einmal ins Studio klicken, dann klingt es.

### Output Log

Alle Meldungen mit Uhrzeit, Warnungen und Fehler farbig, die letzten 200 Einträge. In Play stehen hier
auch Spielereignisse; `clear` leert es.

---

## 11.9 World Settings

Der Tab **World Settings** neben Details gilt fürs ganze Level. Er hat vier Gruppen:

- **Game:** Level name, Game title, welcher Actor der Player ist, **Observer camera** (11.11), die
  **Win condition** (None, Collect all pickups, Reach a goal trigger, Survive the time limit, Reach a
  target score), Time limit, Target score und **Kill Z** (wer tiefer fällt, startet neu).
- **Sky and light:** Tageszeit (0 bis knapp 1), Tageslänge (0 = eingefroren), Sonnenrichtung, Nebel,
  Belichtung, Ambient Occlusion, Schattenreichweite (2 bis 2000 m, Standard 80; kleiner heißt
  schärfer), Wolken, unendlicher Boden und Bodenfarbe.
- **Physics (Box3D):** Schwerkraft, Sub-steps (1 bis 16), Reibung und Sprungkraft des Bodens,
  Steifigkeit und Dämpfung der Kontakte, wie schnell ineinandersteckende Körper auseinandergedrückt
  werden, und Step rate (15 bis 480 Hz).
- **Audio:** Gesamtlautstärke, Musik, Hintergrundklang, Hall, Doppler.

> **Probier's aus!** Öffne Orb Run und zieh in World Settings am Label von **Time of day** langsam
> hin und her. Die Stadt geht in die Nacht über, und die Fenster fangen an zu leuchten.

---

## 11.10 Der Logik-Editor

Jeder Actor und das Level selbst können einen **Logik-Graphen** tragen, nach dem Vorbild der Blueprints
in Unreal: Statt Code verbindest du Kästchen (Knoten) mit Drähten. Du öffnest ihn über **Logic** in der
Toolbar (Level), **Logic** in Details oder **Edit Logic** / **Add Logic (Blueprint)** im Kontextmenü
(Actor). Er legt sich dann über die Viewports.

Der Editor hat eine Toolbar (**Viewport** zurück, **Compile** mit Ampel OK/Warnung/Fehler, Save,
Find, Wahl des Blueprints (Level oder Actor), Templates, **C++**-Export, Rechteckauswahl für Touch,
Hilfe), links **My Blueprint** mit Graphen (Event Graph, State Machines), Variablen (beim Spielen mit
Live-Wert) und Events, in der Mitte den Graphen mit Tabs und rechts Details zu Knoten, Variable,
Kommentar, State oder Transition. Unten listen **Compiler Results** und **Find Results** Probleme und
Treffer; ein Klick springt zum Knoten.

Ein leerer Graph bietet Startknöpfe an: **Begin Play**, **On Interact**, **On Begin Overlap**, **Event
Tick** und **Templates…**. Die fünf Vorlagen: eine Tür, die auf Interact aufgeht; eine Punktezone
(+10); ein Spawner (alle 3 s ein Würfel, höchstens 10); eine Ampel als State Machine; und eine Micro
Duck, die läuft und abdreht, wenn ihr Abstandssensor näher als 1,6 m etwas sieht.

> **Probier's aus!** Neues Level **Basic**, dann **Logic** in der Toolbar, **Begin Play**. Zieh vom
> Ausgang „then“ einen Draht ins Leere, tippe „show message“, Enter, und trag einen Text ein. Zurück mit
> **Viewport**, einen **Rolling Ball** platzieren, **Play**: Dein Text steht im HUD. Dein erstes
> Programm, ganz ohne Code!

Beim Start von Play kompiliert Kern jeden Graphen zu WebAssembly-Maschinencode; ein Graph mit Fehlern
wird abgeschaltet und im Output Log gemeldet, der Rest des Spiels läuft weiter. **Compile** (F7) zeigt
die Fehler schon vorher. **C++** lädt den Graphen als lesbaren Quelltext herunter; in ein eigenes
Spiel gebaut, läuft er als nativer Code ([Kapitel 12](12-eigene-spiele.md)). Mit **F9** gesetzte
Breakpoints halten das Spiel an, sobald der Knoten läuft. Alles Weitere steht in
[Kapitel 9](09-logik.md).

---

## 11.11 Play und Simulate

Studio kennt drei Zustände: **Editing**, **Simulating** und **Playing**. In den beiden laufenden ist das
Level gesperrt, und was in der Simulation passiert, verschwindet beim Stop wieder (außer du behältst es).

> **Stell dir vor …** du baust ein Dominospiel auf dem Tisch auf. **Simulate** heißt: einmal anstoßen und
> zuschauen. **Play** heißt: selbst eine Figur sein, die durchläuft. Und nach dem Stop steht alles wieder
> genau so da wie vorher, wie von Zauberhand neu aufgebaut.

### Play

**Play** (Alt+P) startet das Spiel im aktiven Viewport. Der Spieler bekommt die Steuerung, die Kamera
folgt ihm, und das HUD zeigt Titel, Pickups, Punkte, Zeit, Bestzeit und Tempo mit Boost-Balken (beim
Soldaten Fadenkreuz, Gesundheit und Munition).

Am Desktop fängt Studio die Maus. Ist sie frei, steht da „Click the viewport to control the game · Esc
stops · F8 ejects“. Bei Sieg oder Niederlage erscheint „Victory“ oder „Game over“ mit **Play again**.
**Ctrl+R** startet neu, **Esc** beendet.

![Play im Viewport: das Hovercar in Orb Run mit HUD](../buch/bilder/studio-play.png)

### Eject und Possess

**F8** lässt den Spieler los: Das Spiel läuft weiter, Studio wechselt nach Simulating, du fliegst frei
herum, wählst aus und greifst Körper. Noch einmal **F8** (oder Play) übernimmt ihn wieder.

### Simulate

**Simulate** (Alt+S) lässt Physik und Regeln laufen, ohne dass du jemanden steuerst. Das Beste daran:
**Du kannst Dinge anfassen.** Linke Maustaste (oder Finger) auf einen beweglichen Körper greift ihn.
Ziehen bewegt ihn, nach oben ziehen hebt ihn hoch, das Mausrad (oder ein zweiter Finger) holt ihn
näher oder schiebt ihn weg, und Loslassen **wirft** ihn mit Schwung. Ein kurzer Klick wählt nur aus.

> **Wusstest du?** Der Griff ist kein Trick, sondern echte Physik: Ein federndes Motorgelenk zieht den
> gegriffenen Punkt zum Zeiger. Darum stößt der Körper überall an, kippt und schwingt wie jeder andere.
> Eine Micro Duck hält der Griff zusätzlich gerade, wie eine Hand den Roboter hält, und hochgehoben
> pausiert sie wie der echte Roboter ([Kapitel 13](13-micro-duck.md)).

- **K** (Keep Simulation Changes) übernimmt die simulierten Lagen der ausgewählten Actors ins Level,
  als einen Undo-Schritt, und beendet die Simulation.
- **Pause**, dann **.**, geht Bild für Bild weiter, wie ein Daumenkino.
- Die Show-Flags **Collision Shapes**, **Constraints** und **Contact Points** machen sichtbar, was Box3D
  sieht. Die Statusleiste zählt Bodies, wache Bodies, Gelenke, gebrochene Gelenke und Kontakte.

![Simulate mit eingeblendeten Kollisionsformen](../buch/bilder/studio-simulate.png)

> **Probier's aus!** Lade **File › Example Levels › Physics Lab** und drück **Simulate**. Greif die
> Seilbrücke, wirf eine Kiste gegen den Stapel, dreh am Scharnier. Dann stopp mit Esc, setz einen
> **Crate Stack**, starte wieder **Simulate** und reiß ihn mit der Maus um. Wähle im Outliner alle
> Kisten (Shift+Klick) und drück **K**: Der eingestürzte Stapel bleibt so liegen, als wäre er immer
> schon umgefallen.

![Physics Lab: alle Gelenktypen in der Simulation](../buch/bilder/studio-physics-lab.png)

### Observer camera

Mit World Settings › **Observer camera** startet Play nicht hinter dem Spieler, sondern mit freier
Kamera: Das Spiel läuft, du schaust zu und greifst ein. Am Desktop fliegst du dabei mit W A S D, Q/E
und Shift, ohne Maustaste, und Ziehen ins Leere schaut umher; am Telefon schaut ein Finger umher,
zwei zoomen. Körper greifen geht wie in Simulate, **F8** übernimmt den Spieler, noch einmal F8 schaut
wieder zu. Das Level Micro Duck startet so.

---

## 11.12 Mobil, Touch und Gamepad

### Welches Gerät steuert gerade?

Kern merkt selbst, womit du spielst, und schaltet auf das zuletzt benutzte Gerät um: Ein Finger schaltet
auf Touch, jede echte Taste auf Desktop (auch am Telefon), ein Gamepad, sobald ein Knopf gedrückt oder
ein Stick über 0,22 ausgelenkt wird. Eine Maus schaltet nur an Geräten mit feinem Zeiger auf Desktop;
am Tablet behält sie das Touch-Layout. Die Maus fangen (Pointer-Lock) gibt es nur mit Maus.

### Play am Touchscreen

Play auf dem Telefon füllt den ganzen Bildschirm mit einem Viewport. Oben links stehen **⟵ EXIT** und
**‖ PAUSE** (bzw. **▶ RESUME**). Unten liegen **zwei Sticks** von 168 px (kleiner nur, wenn zwei nicht
passen): links bewegen (**MOVE**, bei Fahrzeugen **STEER / GAS**, beim Flyer **FLY**),
rechts schauen (**LOOK**). Ziehst du den rechten Stick über seinen Ring hinaus, dreht die Kamera weiter.
Den linken Stick ganz nach vorn drücken heißt sprinten; der Ring leuchtet dann. Ziehen auf dem
Bildschirm schaut ebenfalls umher, und beim Soldaten mit Waffe feuert ein Finger auf dem Bildschirm
und zielt weiter, solange er liegt.

Daneben erscheinen Knöpfe je nach Figur: beim Soldaten JUMP, RELOAD, AIM, CROUCH, USE; beim Läufer JUMP
und USE; beim Ball JUMP und BOOST; beim Flyer UP, DOWN, BOOST; bei Fahrzeugen BOOST, HOP und USE; bei
der Micro Duck QUACK, PICK, SIT, KICK L, KICK R, ROLL, BEAK, WHEEE und HOP. Knöpfe erscheinen nur, wenn
die Figur die Fähigkeit hat. Hebt sich der letzte Finger oder geht die Seite in den Hintergrund,
lässt die Engine alles los; nichts bleibt hängen.

![Play am Telefon im Hochformat: EXIT, PAUSE und die Touch-Sticks](../buch/bilder/phone-play-390.png)

### Gamepad

Gamepads mit Xbox-Belegung funktionieren im Studio und im exportierten Spiel; wie in SpawnCity
filtert Kern dabei kleines Wackeln heraus (Dead-Zones) und lässt die Sticks sanft ansprechen
(Kurven). Linker Stick bewegen, rechter schauen, RT feuern, LT zielen, A springen, B ducken, X
nachladen, Y benutzen, L3 oder LB sprinten, Start pausiert. Beim Flyer gehen RB und LB hoch und
runter.

---

## 11.13 Die Micro Duck steuern

Die Micro Duck ist der zweibeinige Roboter von Pollen Robotics, der in Kern auf seinen echten,
trainierten Netzen läuft. Wie sie innen funktioniert, erzählt [Kapitel 13](13-micro-duck.md); hier geht
es ums Bedienen.

**Das Beispiel-Level** (File › Example Levels › Micro Duck) ist ein Zimmer in ihrem Maßstab mit Möbeln,
einem Ball und zwei Socken. Die Ente läuft von selbst. Schon ohne Zusätze spielt sie Ball wie in Pollens
ursprünglicher Software: hinlaufen, ausrichten, mit links kicken. Mit **Kern extras** (im Beispiel an)
übernimmt stattdessen Kerns eigene Spielschicht mit Ball und Socken. Der Level-Graph lässt sie
zwitschern (Knoten „Chirp“), sobald ihre Kopfkamera den Ball sieht. Play startet mit der Observer
camera; **F8** übernimmt die Ente.

**Ohne Eingabe** ist sie **Autonomous**: Sie erkundet den Raum mit Abstandssensor (ToF) und Kamera,
weicht aus und steht nach einem Sturz selbst auf. Hebst du sie am Rumpf hoch, pausiert sie („Held“) und
läuft abgesetzt weiter.

| Taste | Bewegung |
|---|---|
| W / S, A / D | laufen, drehen |
| Leertaste | quaken |
| 1 / 2 | hinsetzen oder aufstehen / aufheben |
| 3 / 4 | mit links / rechts kicken |
| 5 / 6 | rollen / Schnabel auf und zu |
| 7 | springen (Kerns eigene Bewegung) |
| V | „Wheee“-Fahrt, solange gehalten |

Am Touchscreen ist **HOP** der Sprung, QUACK quakt, WHEEE fährt, die übrigen Knöpfe entsprechen 1–6;
am Gamepad quakt RT, LT startet die Wheee-Fahrt (beide öffnen den Schnabel), B lässt sie sitzen
oder aufstehen, Y hebt auf, X rollt. In Logik-Graphen steuerst du sie mit den Knoten der Kategorie
**Robots** (Walk, Look, Robot ToF, Robot Camera Sees, Robot Move mit „jump“ …).

Während des Spiels zeigt Studio oben rechts, was die Ente wahrnimmt (Window › Show › **Robot Camera**):
**HEAD CAMERA**, das Bild ihrer Kopfkamera mit ihrem Zustand (oder „Fallen“), und **TOF 8×8**, das
Gitter ihres Abstandssensors: rot ein Hindernis (heller, je näher), grün Boden, gelb zu nah, dunkel
nichts, dazu der kleinste Abstand in Metern oder „clear“.

> **Probier's aus!** Lade das Micro-Duck-Level, drück **Play** und schau ihr eine Weile zu. Dann heb den
> Ball mit der Maus hoch und leg ihn woanders hin. Zum Schluss **F8** und Taste **7**: Die Ente springt!

---

## 11.14 Policies laden (jump.onnx, play.onnx)

Die Bewegungen der Ente sind neuronale Netze (**Policies**) im Dateiformat ONNX. Das Laufnetz
`velstand.onnx` ist fest eingebaut; alle anderen lädt die Seite nach, sobald ein Level eine Ente enthält:

- **Pollens Netze** (aus dem Ordner `microduck/`) für Sitzen, Aufheben, Kicken, Rollen, Rollschuhe.
- **`jump.onnx`** (aus `policies/`), vom Kern Lehrer trainiert: der Sprung.
- **`play.onnx`** (aus `policies/`), ebenfalls vom Lehrer: die Spielschicht mit Ball und Socke, nur mit
  **Kern extras**.

Die Engine meldet, welche Dateien fehlen, die Seite holt sie, die Engine prüft jede und merkt sie sich
unter ihrem Namen. Klappt eine Datei nicht, versucht die Seite es nach 5 s, 10 s, 20 s … erneut;
der Abstand wächst bis höchstens eine Minute. Bis dahin bleibt die Ente bei dieser Bewegung stehen
und sagt, warum.

> **Wusstest du?** Netze, die du selbst mit dem Kern Lehrer im Browser trainierst
> ([Kapitel 14](14-lehrer.md)), landen im Speicher desselben Browsers. Studio lädt sie beim Start und
> sofort wieder, wenn der Lehrer ein neueres meldet. Dein eigenes `jump.onnx` ersetzt also das
> mitgelieferte, ohne dass du eine Datei kopierst.

Wer ein exportiertes Spiel mit Ente selbst ins Netz stellt, legt die Ordner `microduck/` und `policies/`
neben die HTML-Datei, samt Lizenz und NOTICE von Pollen (11.16 und [Kapitel 12](12-eigene-spiele.md)).

---

## 11.15 Speichern, Laden, Autosave und Einstellungen

### Die Datei `.klevel`

Ein Level ist eine Textdatei im Format JSON mit der Endung `.klevel`. Darin stehen Name, World Settings,
Editor-Kamera, Actors, Logik und sogar Texturen und importierte Sounds. Eine Datei ist also
**vollständig**, du kannst sie verschicken. Stammt sie von einer neueren Studio-Version, lehnt Studio
sie mit „level was written by a newer Kern Studio“ ab.

### Speichern und Öffnen

**Ctrl+S** speichert. In Chrome und Edge fragt Studio beim ersten Mal nach dem Ort und schreibt danach
direkt in dieselbe Datei; in anderen Browsern lädt es `<Levelname>.klevel` herunter. **Ctrl+Shift+S**
fragt immer neu. Der Dateiname ist der Levelname; alles außer Buchstaben, Ziffern, `-` und `_` wird
darin zu `_`. **Ctrl+O** öffnet `.klevel`- und `.json`-Dateien. Oder du ziehst eine Datei einfach auf
die Seite: Levels öffnen sich, Bilder werden Texturen, Audiodateien Sounds.

Auf Android kennt der Dateidialog die Endung `.klevel` nicht. Studio filtert dort deshalb nicht; wählst
du etwas anderes, heißt es „… is not a Kern level“, und alles läuft weiter.

### Autosave

> **Stell dir vor …** ein Schutzengel, der dir über die Schulter schaut, aber nie nervt. Er schreibt
> sich heimlich auf, was du noch nicht gespeichert hast, und zerreißt den Zettel, sobald du speicherst.

Genau so arbeitet der Autosave: Spätestens 2 s nach einer Änderung legt Studio eine Kopie im Browser
ab, aber nur, solange etwas ungespeichert ist. Nach dem Speichern (oder wenn du alles bis zum
gespeicherten Stand rückgängig machst) verschwindet sie wieder. Öffnest du ein anderes Level, bleibt
die Kopie, bis das neue eigene Änderungen bekommt. Beim Verlassen der Seite, wenn sie in den
Hintergrund geht, und vor einem Update wird sofort geschrieben; gibt es Ungespeichertes, fragt der
Browser beim Verlassen nach. Während Play und Simulate wird nichts geschrieben.

Beim nächsten Start bietet Studio **Restore** oder **Dismiss** an. Restore lädt die Kopie, das Level
bleibt aber als ungespeichert markiert, denn es steht ja in keiner Datei; bis du speicherst, wird es
bei jedem Start erneut angeboten. Dismiss löscht die Kopie. Getrennt davon merkt sich Studio das
zuletzt bearbeitete Level und öffnet es wieder.

> **Achtung, Falle!** Der Autosave ist ein Rettungsring, kein Speicherort. Er hält nur **eine** Kopie im
> Browser. Wer den Browserspeicher leert, verliert sie. Wichtige Levels speicherst du mit Ctrl+S als
> Datei.

### Einstellungen

Studio merkt sich nur, was **du selbst** geändert hast: Preferences, Layout, Raster, Achsen, Ton und pro
Viewport Ansicht, View Mode, Sichtfeld und Show-Flags. Alles andere folgt den Standards der jeweiligen
Version. Unter **Edit › Preferences…** stellst du Kamerageschwindigkeit, Mausempfindlichkeit,
invertierte Maus, das Kreisen um die Auswahl und die drei Raster ein (Standard: Tempo 4,
Empfindlichkeit 1, Raster 0,5 m und 15° an, Skalierung 0,25 aus). **Edit › Reset Settings to
Defaults…** fragt nach und setzt alles zurück, auch Panelgrößen und Content Drawer; deine Levels
bleiben unberührt.

---

## 11.16 Spiel exportieren

Jetzt kommt der magische Moment. **File › Package Game (HTML)...** oder der Paket-Knopf öffnet den
Dialog **Package Game**:

1. **Game title** eintragen.
2. **Package** klicken.
3. Studio lädt `<Titel>.html` herunter und bietet **Play it** an.

Diese **eine Datei** ist ein komplettes Spiel: Engine, Spiel-Host und Level stecken darin (die Engine
als Base64-Text, das Level als JSON). Sie braucht keinen Server und kein Internet und läuft im
Browser, auch direkt von der Festplatte (`file://`) und am Telefon. Beim Start öffnet sie kein
Editorfenster, sondern sofort das Spiel.

Im exportierten Spiel gilt die Steuerung aus Play. Dazu: Ein Klick fängt die Maus, **Esc** gibt sie frei
und pausiert, **P** pausiert, **R** oder Enter nach Sieg oder Niederlage startet neu, ebenso
**Ctrl+R**; **F8** wechselt bei Observer camera zum Spieler und zurück; am Touchscreen gibt es oben
links ein ↻. Unten steht ein Tastenhinweis (am Touchscreen ausgeblendet). Die Datei kann auch online
spielen (`?host=`, `?join=`, siehe [Kapitel 10](10-netzwerk.md)).

> **Achtung, Falle!** Zwei Dinge musst du beim Weitergeben wissen. Erstens stecken die nachladbaren
> Netze der Micro Duck **nicht** in der Datei: Laufen geht ohne sie, für Sitzen, Kicken, Springen und
> Spielen gehören die Ordner `microduck/` und `policies/` daneben. Zweitens: Das Modul enthält Pollens 3D-Modelle der Micro Duck,
> die nur nicht kommerziell genutzt werden dürfen. Wer verkaufen will, baut eine kommerzielle Fassung
> ohne diese Modelle ([Kapitel 12](12-eigene-spiele.md)).

> **Probier's aus!** Exportiere Orb Run, öffne die Datei mit **Play it** und schick sie dir selbst aufs
> Telefon. Dort erscheinen die Touch-Sticks von selbst.

---

## 11.17 Multiplayer

### Mehrere Spieler im Editor

**Play › Number of Players** startet Play mit 2, 3 oder 4 Spielern. Die Welt ist dann ein Listen-Server
(ein Server, der selbst mitspielt), jeder weitere Spieler ein echter Netzwerk-Client mit eigener Welt und
eigenem Viewport.

1. Play › Number of Players › **2 Players (listen server)**.
2. **Play**. Studio schaltet auf vier Viewports: 1 zeigt den Server, 2 den ersten Client. Jeder hat sein
   eigenes HUD.
3. Nur der Spieler im aktiven Viewport bekommt deine Eingabe. Ein Klick in einen anderen wechselt.
4. **Play › Network Emulation** macht die Leitung schlechter, von „Perfect“ bis „Terrible (300 ms, 20%
   loss)“.
5. Die Statusleiste zeigt Rolle, Spieler, Ping, Verlust und Datenrate.

![Zwei Spieler im Editor: Listen-Server und lokaler Client in eigenen Viewports](../buch/bilder/studio-multiplayer.png)

### Online zwischen Browsern

Beide öffnen **dasselbe Level**. Der Host wählt **Play › Host Online Game...**, der Gast **Join Online
Game...**, beide denselben **Room** (Standard „kern“) und, für verschiedene Rechner, dasselbe **Relay**.
Ohne Relay finden sich Tabs desselben Browsers; gibt es im Raum keinen Host, meldet Join das nach 4 s.
Für verschiedene Rechner startet jemand `node tools/net_relay.mjs` (braucht das npm-Paket `ws`,
Standardport 8790), alle tragen seine Adresse ein. **Leave Online Game** verlässt das Spiel;
der Host bemerkt, wenn ein Gast geht.

Ein Host im Hintergrund-Tab läuft weiter, damit seine Gäste nicht aus der Verbindung fliegen. Logik läuft
nur beim Host; Änderungen erreichen alle, auch wer später dazukommt. Wie das unter der Haube
funktioniert, steht in [Kapitel 10](10-netzwerk.md).

---

## 11.18 Schmale Bildschirme und Telefone

Bis 900 px Fensterbreite oder auf Touchgeräten mit höchstens 600 px Höhe schaltet Studio auf ein
kompaktes Layout:

- **Der Viewport bekommt die ganze Fläche.** Outliner, Details und World öffnest du über den
  **Panels**-Knopf als Blatt mit großen Tabs, im Querformat rechts, im Hochformat von unten (64 % der
  Höhe). Ohne Auswahl öffnet es mit dem Outliner.
- **Play und Simulate sind immer einen Tipp entfernt.** Sie stehen mit Panels in einem **Dock** am
  rechten Rand der Toolbar, das stehen bleibt, während der Rest seitwärts scrollt. Beim Spielen stehen
  dort Pause, Stop und Eject; was gerade nichts tut, verschwindet, der Rest hat Touch-Größe (ab 38 px).
- Weniger Wichtiges (Rasterauswahl, Achsen-Knopf, Konsole, Physik-Statistik) wird ausgeblendet, der
  Content Drawer nimmt höchstens knapp die halbe Höhe, und die Seite scrollt nie seitwärts.

![Studio in einem 820 px breiten Fenster](../buch/bilder/studio-narrow.png)

![Telefon im Hochformat: das Dock mit Play, Simulate und Panels](../buch/bilder/phone-dock-390.png)

> **Probier's aus!** Öffne Studio auf deinem Telefon. Tipp im Dock auf **Play**, dann **EXIT**. Öffne
> **Panels**, wähle im Outliner einen Actor und ändere in **Details** seine Farbe.

---

## 11.19 Architektur: warum Studio so gebaut ist

### Die Editorlogik ist C++

Studio ist eine einzige C++-Klasse. Sie besitzt das Dokument, die History, die Auswahl, die vier
Viewports, Gizmos, Details, den Bauteil-Katalog, Menüs, Dialoge, Einstellungen, Play und das
Netzwerkspiel. Die Browserseite enthält **keine** Editorlogik.

Das hat zwei große Vorteile. Erstens läuft im Editor exakt der Code des exportierten Spiels; es gibt
keine zweite Fassung, die abweichen könnte. Zweitens lässt sich der ganze Editor ohne Browser testen:
Kerns Tests bedienen Navigation, Gizmos, Play, Speichern und Autosave direkt in C++.

### Alles ist ein Befehl

> **Stell dir vor …** ein Restaurant. Die Browserseite ist der Kellner: Sie nimmt deine Bestellung auf
> (Klick, Taste), bringt sie in die Küche und trägt das Essen raus (zeichnet die Panels). Gekocht wird
> nur in der Küche, im C++-Modul. Und jede Bestellung ist ein ordentlicher Zettel.

Diese Zettel sind kleine JSON-Befehle wie `{"cmd": "spawn", …}`. **Alles**, was du im Studio tust, ist
so ein Befehl, auch jeder Menüeintrag. Es gibt Befehle für Auswahl, Actors, Eigenschaften, History,
Play, Netz, Ansichten, Panels, Dokumente, Ton, Einstellungen und Logik. Die Seite spricht das Modul über
eine Reihe flacher C-Funktionen an (die **C-ABI**, deren Namen mit `kern_studio_` beginnen), etwa eine
für jeden Frame, eine für Tasten und eine für Befehle. Eigenschaften ändert der Befehl `set` über
Pfade in der JSON-Form des Actors, etwa `transform.position`. Die vollständige Liste steht im blauen
Buch und in [Kapitel 17](17-referenz.md).

### Die dünne Browserseite

Pro Bildschirm-Frame meldet die Seite die Viewport-Rechtecke, fragt das Gamepad ab, lässt das Modul
rechnen und zeichnen, füllt den Ton nach und verschickt Netzpakete. Dann holt sie Aufträge ab
(Maus fangen, Zwischenablage, Dialog, Datei speichern …) und zeichnet ein Panel nur neu, wenn sich
dessen Inhalt geändert hat. Alle 200 ms lädt sie Texturen, Sounds und Netze nach, alle 500 ms prüft
sie Autosave und Einstellungen. Sie erledigt nur, was ein Browser kann und C++ nicht: Dateidialoge,
Downloads, Zwischenablage, Bilder und Audio dekodieren, Browserspeicher, Tonausgabe und Netzpäckchen.

> **Wusstest du?** Ein automatischer Browser-Test bedient das ganze Studio wie ein Mensch: Menüs, Fly
> Mode, Werkzeuge, Outliner, Details mit Undo, Play, Simulate, Logik, Micro Duck, Multiplayer, Export,
> Wiederherstellen und Telefon-Layouts. Die meisten Bilder in diesem Kapitel stammen aus genau diesem
> Test.

---

## Das nimmst du mit

- Kern Studio arbeitet wie Unreal: **Actors** mit **Komponenten**, bearbeitet in Viewports, Outliner,
  Details und World Settings.
- **Rechte Maustaste + W A S D** fliegt, **Q/W/E/R** wählt das Werkzeug, **Alt + Gizmo** zieht eine
  Kopie, **F** holt die Auswahl ins Bild.
- Der **Content Drawer** hat über 40 fertige Bauteile, von Orb bis Micro Duck.
- **Simulate** lässt dich Körper greifen und werfen, **K** behält das Ergebnis, **Play** macht dich zum
  Spieler, **F8** wechselt zwischen beidem.
- Undo ist exakt (bis zu 500 Schritte), der **Autosave** rettet Ungespeichertes, aber ersetzt kein
  Ctrl+S.
- **Package Game** macht aus deinem Level **eine HTML-Datei**, die überall läuft.
- **Number of Players** und **Host/Join** bringen Mehrspieler, auch auf dem Telefon klappt alles.
- Die Editorlogik ist C++ im selben Modul wie das Spiel; alles ist ein JSON-Befehl.

## Knobelecke

1. Du ziehst mit der linken Maustaste über einen Würfel. Warum bewegt er sich nicht?
2. Du hast einen Kistenstapel einstürzen lassen und willst ihn genau so behalten. Wie?
3. Ein Freund bekommt dein exportiertes Spiel mit Micro Duck, aber die Ente kann nur laufen. Was fehlt?
4. Warum ist ein langer Zug am Farbwähler nur ein einziger Undo-Schritt?
5. Was ist der Vorteil, dass die Editorlogik im selben C++-Modul läuft wie das exportierte Spiel?

### Lösungen

1. Linkes Ziehen zeichnet immer ein Auswahlrechteck. Bewegt wird nur mit dem Gizmo (W).
2. In **Simulate** einstürzen lassen, die Kisten auswählen, **K** drücken.
3. Die Ordner `microduck/` und `policies/` mit den nachladbaren Netzen neben der HTML-Datei.
4. Studio gibt dem ganzen Vorgang eine Kennung, und die Engine fasst alle Änderungen mit dieser Kennung
   zu einem Schritt zusammen.
5. Was du im Editor siehst, verhält sich exakt so wie im fertigen Spiel, und der Editor lässt sich ohne
   Browser testen.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
