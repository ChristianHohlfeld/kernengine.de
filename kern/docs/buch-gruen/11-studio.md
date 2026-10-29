# 11 Kern Studio: Deine Werkstatt im Browser

> **Kurz gesagt:** Kern Studio ist der Editor der Kern-Engine. Du baust darin Levels aus Actors und
> Komponenten, spielst sie mit einem Klick direkt im Fenster und machst daraus eine einzige HTML-Datei
> (HTML ist das Dateiformat von Webseiten, jeder Browser kann es öffnen), die überall läuft. Dieses Kapitel ist eine geführte Tour: Mach am besten Studio auf und klick mit.

> **Begriffe vorab:**
> - **Editor** – ein Bearbeitungsprogramm, hier: die Werkstatt, in der du Spielwelten baust.
> - **Level** – eine Spielwelt, alles, was zu einem Spiel gehört, in einer Datei.
> - **Actor** – ein Ding im Level: ein Würfel, eine Lampe, eine Spielfigur, eine Zone.
> - **Komponente** – eine Fähigkeit, die du einem Actor ansteckst: „hat eine Form“, „leuchtet“,
>   „fällt runter“.
> - **Transformation** (Transform) – Ort, Drehung und Größe eines Actors.
> - **Mesh** – das Drahtgitter aus Dreiecken, aus dem die sichtbare Form eines Dings besteht.
> - **Gizmo** – der bunte Anfasser mit Pfeilen und Ringen, mit dem du einen Actor verschiebst,
>   drehst oder größer ziehst.
> - **Simulation** – das Level läuft von selbst: Dinge fallen, rollen, Regeln greifen.

Die Tour geht so: Erst schauen wir uns in der Werkstatt um. Dann lernst du, woraus ein Level besteht,
wie du es von allen Seiten anschaust, Dinge auswählst und bewegst und Fehler rückgängig machst. Danach
erweckst du dein Level mit **Spielen** (Play) und **Simulieren** (Simulate) zum Leben, gibst ihm
mit dem Logik-Editor Regeln und machst daraus ein fertiges Spiel. Die Begriffe (Actor, Viewport,
Outliner …), die du in 11.2 kennenlernst, sind dieselben wie in
großen Profi-Werkzeugen der Spielebranche, etwa der Unreal Engine; wer die kennt, fühlt sich sofort zu
Hause.

![Kern Studio mit Orb Run: oben Menüleiste und Toolbar, in der Mitte der Viewport, rechts Outliner und Details, unten die Statusleiste](../buch/bilder/studio.png)

> **Wusstest du?** Was du im Studio siehst, rechnet **genau derselbe Code**, der später dein
> exportiertes Spiel ausführt. Was im Editor funktioniert, funktioniert also auch im fertigen Spiel
> (11.16).

> **Wusstest du?** Auf einem deutschen System zeigt Studio deutsche Beschriftungen. Dieses Buch nennt
> die Beschriftungen, wie Studio sie auf Deutsch zeigt, in Klammern die englischen, etwa
> **Datei** (File). Wer Studio auf Englisch nutzt (Knopf **DE | EN** unten rechts in der
> Statusleiste), findet sie dort. Nur der Logik-Editor, das Protokoll und die Konsole bleiben auch
> in der deutschen Oberfläche englisch (11.10). Was du selbst eintippst, etwa Befehle in der Konsole, ist in
> beiden Sprachen gleich.

---

## 11.1 Studio öffnen

Studio ist eine Webseite: Du öffnest auf https://kernengine.de/ den Editor und legst los, auch am
Telefon. Installieren musst du nichts.

Beim allerersten Mal ist das Beispiel-Level **Orb Run** geladen: eine Stadt, die der Computer nach
Regeln selbst erzeugt hat, mit Verkehr, einem schwebenden Hovercar, Energiekugeln (Orbs) zum Einsammeln
und einem Tag, der in die Nacht übergeht. Später öffnet Studio das Level, an dem du zuletzt gearbeitet
hast. Gab es dabei ungespeicherte Änderungen, bietet es an, sie wiederherzustellen (11.12).

> **Probier's aus!** Öffne Studio, klick einmal ins Bild und drücke **Alt+P**, die Taste für
> **Spielen** (Play). Schon fährst du mit dem Hovercar durch Orb Run: W A S D lenkt, Umschalt gibt
> Schub. **Esc** bringt dich zurück in den Editor.

> **Für Neugierige:** Wer Kern selbst baut, kann Studio auch als einzelne Datei ohne Internet öffnen
> oder direkt aus dem Quellcode starten. Wie das geht, steht im blauen Buch, Kapitel 11.

### Als App installieren

Du kannst Studio wie eine App aufs Gerät holen: In Chrome und Edge über den Knopf
**⤓ Installieren** (⤓ Install) oben, auf iPhone und iPad erklärt derselbe Knopf die Schritte
**Teilen** (Share) und **Zum Home-Bildschirm** (Add to Home Screen). Installiert startet Studio im
Vollbild und funktioniert auch ohne Internet. Gibt es eine neue Version, holt Studio sie von selbst.
Beim Bearbeiten lädt es sofort neu, und ungespeicherte Änderungen kommen über den Autosave mit
(11.12). Läuft gerade ein Spiel, wartet es bis zum Stopp und bietet so lange **Aktualisieren**
(Update) oder **Später** (Later) an.

---

## 11.2 Die Werkstatt im Überblick

Studio ist aufgebaut wie eine Werkstatt: ein großer Arbeitsplatz in der Mitte, drumherum Listen,
Werkzeuge und Vorräte. Jeder Bereich hat genau eine Aufgabe.

> **Stell dir vor …** eine Werkstatt. In der Mitte die Werkbank mit deinem Modell (der Viewport). Rechts
> an der Wand die Inventarliste (Outliner) und darunter der Zettel mit den Maßen des Teils in deiner Hand
> (Details). Unten eine Schublade voller Bauteile (die Inhaltsablage). Und ganz oben das Regal mit den
> Werkzeugen (Toolbar).

| Bereich | Lage | Was drin ist |
|---|---|---|
| Menüleiste | ganz oben | Menüs, Levelname |
| Toolbar | darunter | Speichern, Rückgängig, Hinzufügen, Werkzeuge, Spielen |
| Viewports | Mitte | 1, 2 oder 4 Fenster in die 3D-Welt |
| **Outliner** | rechts oben | die Liste aller Actors |
| **Details** / **Welteinstellungen** (World Settings) | rechts unten | Eigenschaften des gewählten Actors bzw. des ganzen Levels |
| **Inhaltsablage** (Content Drawer) | unten, klappbar | Bauteile, Bilder, Töne, Protokoll |
| Statusleiste | ganz unten | Meldungen, Konsole, Ton |

Die Trennlinien zwischen den Bereichen kannst du ziehen; Studio merkt sich die Größen. Ein Punkt neben
dem Levelnamen heißt: Es gibt ungespeicherte Änderungen.

### Die Menüs in einem Satz

- **Datei** (File): neues Level, öffnen, speichern, Bild importieren, Spiel exportieren,
  **Beispiel-Levels** (Example Levels).
- **Bearbeiten** (Edit): Rückgängig, **Verlauf...** (History...), Kopieren und Einfügen, Einstellungen.
- **Fenster** (Window): Aufteilung der Fenster, Ansichten, Inhaltsablage, Protokoll, Vollbild.
- **Spielen** (Play): Spielen, Simulieren, Pause, Stopp, Mehrspieler.
- **Hilfe** (Help): die Tastenübersicht (F1).

**Datei › Neues Level...** (File › New Level...) bietet sechs Vorlagen als Karten (Doppelklick
erstellt sofort):

| Vorlage | Was drin ist |
|---|---|
| **Orb Run** | Nachtstadt, Hovercar, Orbs zum Einsammeln |
| **Physics Lab** | viele Arten von Gelenken: Scharnier, Schiene, Rad, Seil (Pendel), Seilbrücke, Motor, dazu ein Kistenstapel |
| **Shooting Range** | Ego-Shooter-Soldat, Ziele, Deckung, mehrspielerfähig |
| **Micro Duck** | die Laufroboter-Ente aus [Kapitel 13](13-micro-duck.md) |
| **Basis** (Basic) | Boden, Sonne, Himmel und ein Würfel: ein sauberer Anfang |
| **Leer** (Empty) | gar nichts |

### Die Toolbar

Die Leiste unter den Menüs hat viele Knöpfe. Die wichtigsten, von links nach rechts:

1. Diskette: speichern.
2. Pfeile: rückgängig und wiederholen.
3. **Hinzufügen** (Add): ein Bauteil hinzufügen.
4. Werkzeuge: **Auswählen (Q)**, **Verschieben (W)**, **Drehen (E)** und **Skalieren (R)**, auf
   Englisch Select, Move, Rotate und Scale.
5. Abspielknöpfe: Spielen, Simulieren, Pause, Stopp.

Zwischen Werkzeugen und Abspielknöpfen liegen die Knöpfe fürs Einrasten (11.5); rechts davon folgen
die Aufteilung der Fenster, der Logik-Editor (**Logik** (Logic), 11.10) und der Export (11.13).

### Das Kontextmenü

Ein Rechtsklick auf einen Actor (am Touchscreen: lange halten) öffnet ein Menü mit allem, was du mit
genau diesem Ding tun kannst: umbenennen, kopieren, löschen, zum Spieler machen, ihm Logik geben, ihn an
einen anderen hängen oder mit einem Gelenk verbinden. Mit **Actor platzieren ▸** (Place Actor ▸) setzt du ein neues Bauteil genau an die geklickte Stelle.
Die vollständige Liste steht im blauen Buch, Kapitel 11.

### Statusleiste und Konsole

Ganz unten stehen die letzte Meldung, der Lautsprecher (Ton an/aus), wie viele Actors ausgewählt sind
und die laufende Version. Das Feld **Cmd** ist die **Konsole**: eine Zeile, in die du Befehle
tippst, statt zu klicken. `help` listet alle auf. Ein paar Beispiele: `spawn physics_cube` setzt eine
Physik-Kiste, `tod 0.8` stellt die Tageszeit (englisch „time of day“; die Zahl läuft von 0 bis knapp
1 einmal durch den ganzen Tag), `gravity 0 0 -1.6` ändert die Schwerkraft.

Die drei Zahlen bei `gravity` sagen, in welche Richtung und wie stark es zieht: links/rechts,
vorn/hinten, oben/unten. In Kern zeigt die dritte Achse nach oben, das Minus heißt also: nach unten.
1,6 ist die Schwerkraft des Mondes, auf der Erde sind es etwa 9,8.

> **Probier's aus!** Neues Level **Basis** (Basic). Klick unten ins Feld neben **Cmd** und tippe
> `gravity 0 0 -1.6`, dann ein paar Mal `spawn physics_cube`. Drück **Simulieren** (Simulate) oder
> Alt+S: Die Kisten sinken gemächlich zu Boden, wie auf dem Mond. Ein `gravity` ohne Zahlen stellt die Erdschwerkraft wieder her.

---

## 11.3 Ein Level aus Actors und Komponenten

Ein Level ist eine Sammlung: die Einstellungen der Welt, die Actors, Bilder, Töne und die Logik. Jeder
Actor hat einen Namen, seine Transformation (Ort, Drehung, Größe) und eine Reihe von Komponenten. Was ein
Actor **ist**, ergibt sich allein aus seinen Komponenten. Gemessen wird in Metern, Sekunden und
Kilogramm.

> **Stell dir vor …** ein Actor ist eine leere Grundplatte aus dem Baukasten. Komponenten sind die Steine, die du
> draufsteckst: Ein Form-Stein macht ihn sichtbar, ein Physik-Stein lässt ihn fallen, ein Pickup-Stein
> macht ihn einsammelbar. Kombinieren erlaubt!

Ein Beispiel: Eine Kiste, die vom Tisch fallen soll, braucht eine Form, die Komponente
**Statisches Mesh** (Static Mesh), und Physik, die Komponente **Starrkörper** (Rigid Body). Steckst du
noch **Pickup** dazu, kann der Spieler sie einsammeln. Dieselbe Kiste ohne Physik schwebt einfach
dort, wo du sie hingestellt hast.

| Komponente | Was sie tut |
|---|---|
| **Statisches Mesh** | Form (Würfel, Kugel, Zylinder, Rampe …) mit Farbe und Material |
| **Starrkörper** | Physik; das Feld **Mobilität** (Mobility) wählt: **Statisch** (Static) steht fest, **Dynamisch (Physik simulieren)** (Dynamic (simulate physics)) fällt und rollt, **Kinematisch** (Kinematic) bewegt sich nach Plan, lässt sich aber nicht schubsen |
| **Kollider** (Collider) | eine Form zum Anstoßen, mit Reibung und Sprungkraft |
| **Gelenk** (Constraint) | Scharnier, Schiene, Seil, Rad, Motor … |
| **Spieler-Steuerung** (Player Controller) | macht den Actor steuerbar (Hovercar, rollende Kugel, Figur, Soldat, Läufer wie die Ente …) |
| **Pickup** | einsammelbar: Punkte, Heilung, Munition |
| **Trigger** | eine unsichtbare Zone: Ziel, Gefahr, Checkpoint, Teleporter … |
| **Waffe** (Weapon), **Gesundheit** (Health) | Waffe und Lebenspunkte |
| **Rotator**, **Beweger** (Mover) | dreht den Actor bzw. bewegt ihn hin und her |
| **Punktlicht** (Point Light), **Audioquelle** (Audio Source) | Licht bzw. Klangquelle |
| **Prozedurale Stadt** (Procedural City) | eine ganze Stadt mit Verkehr |

Was die Felder jeder Komponente bedeuten, erklären [Kapitel 6](06-physik.md) (Physik),
[Kapitel 7](07-audio.md) (Ton) und [Kapitel 8](08-spiel-laufzeit.md) (Spielregeln). Neue Komponenten
steckst du in **Details** mit **Hinzufügen** (Add) an.

### Actors anlegen

Fertige Actors liegen in der **Inhaltsablage** (Content Drawer), der Bauteil-Schublade unten
(**Strg+Leertaste**). Unter **Actors platzieren** (Place Actors) findest du über 40 Stück, sortiert in
Gruppen:

- **Spieler** (Players): Spielfiguren wie **Figur** (Character), Soldat, Hovercar, rollende Kugel,
  Drohne und die Micro Duck.
- **Spielablauf** (Gameplay): Orbs und Sterne zum Sammeln, Heilpakete, Ziel, Gefahrenzone,
  Checkpoint, Sprungfeld, Teleporter, bewegliche Plattform, explodierendes Fass …
- **Formen** (Shapes): einfache Formen wie **Würfel** (Cube), **Kugel** (Sphere), **Zylinder**
  (Cylinder), **Rampe** (Ramp).
- **Physik** (Physics): Kisten wie der **Physik-Würfel** (Physics Cube), Fässer, ein Kistenstapel, Pendel, Tür, Kette, Wippe und eine Socke in
  Entengröße.
- **Lichter** (Lights), **Audio**, **Welt** (World): Lampen, Klangquellen und die Stadt.

Ein Klick setzt das Bauteil dorthin, wohin die Kamera blickt; Ziehen in den Viewport setzt es
genau unter die Maus. Der neue Actor ist gleich ausgewählt. Setzt du eine Figur aus **Spieler**, wird
sie dein Spieler.

### Eltern und Kinder

Du kannst einen Actor an einen anderen **anhängen**. Der angehängte heißt **Kind**, der andere
**Eltern-Actor**. Bewegst du den Eltern-Actor, wandert das Kind mit.

> **Stell dir vor …** du setzt einer Figur einen Hut auf und hängst ihn an sie. Läuft die Figur los,
> kommt der Hut mit. Ziehst du nur den Hut ein Stück zur Seite, bleibt die Figur stehen.

So geht's: mehrere Actors wählen, den künftigen Eltern-Actor zuletzt, dann Rechtsklick ›
**Anhängen an …** (Attach to …); dahinter steht der Name des Eltern-Actors. Oder im
**Outliner** einen Actor auf einen anderen ziehen. **Strg+G** (Rechtsklick › **Gruppieren** (Group))
packt die Auswahl unter einen neuen, leeren Gruppen-Actor. Ordner im Outliner dagegen dienen nur der
Ordnung und bewegen nichts.

> **Für Neugierige:** Beim Simulieren werden Kinder ohne eigene Physik fest mit dem Körper ihres
> Eltern-Actors verschweißt. Die Regeln dazu stehen in [Kapitel 6](06-physik.md).

### Gelenke mit „Mit Gelenk verbinden“

Zwei Actors lassen sich mit einem Gelenk verbinden, sodass einer am anderen hängt, schwingt oder
sich dreht. Am schnellsten geht das so:

1. Den Actor wählen, der *hängen* soll.
2. Mit Strg den Actor dazuwählen, *an dem* er hängt, und zwar als Letztes.
3. Rechtsklick › **Mit Gelenk verbinden ▸** (Connect with Constraint ▸) und eine Gelenkart wählen:
   **Scharnier** (Hinge), **Schiene** (Slider), **Kugel** (Ball) für ein Kugelgelenk,
   **Fest (verschweißt)** (Fixed (weld)), **Abstand / Seil** (Distance / rope), **Rad** (Wheel) oder
   **Motor**.

Studio gibt dem hängenden Actor dabei Physik, falls er noch keine hat.

> **Probier's aus!** (Wie Auswählen und das Gizmo genau gehen, steht in 11.5; für diese Mission
> genügt: Klick wählt aus, an den bunten Pfeilen ziehst du.) Setze aus **Inhaltsablage › Formen**
> (Content Drawer › Shapes) einen **Würfel** (Cube) und schieb ihn am blauen Pfeil des Gizmos hoch in
> die Luft, daneben aus **Inhaltsablage › Physik** (Content Drawer › Physics) einen **Physik-Würfel**
> (Physics Cube), ebenfalls ein Stück in der Luft. Im Outliner heißen
> die beiden neuen Actors etwa „Cube 2“ und „Physics Cube 2“, denn Studio vergibt jeden Namen nur
> einmal. Wähle erst den neuen Physik-Würfel, dann mit Strg den neuen Würfel, und verbinde
> sie mit **Abstand / Seil**. Jetzt **Simulieren** (Simulate): Die Kiste schwingt am Seil wie ein
> Pendel. Warum fällt der Würfel nicht mit? Er hat keine Physik-Komponente, also bleibt er wie
> festgenagelt in der Luft. Er ist dein Aufhängepunkt.

### Der Spieler und die Prüfung vor dem Start

**Spielen** (Play) steuert den Actor mit **Spieler-Steuerung** (Player Controller). Du bestimmst ihn,
indem du eine Figur aus **Spieler** (Players) setzt oder per Rechtsklick ›
**Zum Spieler machen** (Make Player). Gibt es keinen Spieler, startet Spielen stattdessen eine
Simulation und meldet „Noch nichts zu steuern“ (Nothing to control yet).

Vor jedem Start prüft Studio das Level. **Fehler** (etwa ein Gelenk, das auf einen gelöschten Actor
zeigt) verhindern den Start; **Warnungen** stehen nur im Protokoll.

---

## 11.4 Ansichten: Fenster in die Welt

Ein **Viewport** ist ein Fenster in deine 3D-Welt. Du kannst bis zu vier gleichzeitig öffnen und jedes
anders schauen lassen, damit du ein Ding von allen Seiten im Blick hast.

> **Stell dir vor …** ein Foto und einen Bauplan von deinem Haus. Auf dem Foto wird hinten alles
> kleiner, das wirkt echt (die **perspektivische** Ansicht). Auf dem Bauplan sind alle Wände so lang,
> wie sie wirklich sind, egal wie weit weg, und du schaust genau von oben, vorn oder von der Seite (die
> **orthografische** Ansicht). Zum Anschauen nimmst du das Foto, zum genauen Ausrichten den Bauplan.

Jeder Viewport hat oben eine eigene Leiste:

- Ansichtstyp: **Perspektive** (Perspective) oder ein Bauplan von **Oben** (Top),
  **Unten** (Bottom), **Links** (Left), **Rechts** (Right), **Vorne** (Front) oder **Hinten** (Back).
- **Ansichtsmodus** (View Mode), wie gezeichnet wird: **Beleuchtet** (Lit) mit Licht und Schatten,
  **Unbeleuchtet** (Unlit) ohne Licht, **Drahtgitter** (Wireframe) mit nur dem Drahtgitter der Meshes
  oder **Spieler-Kollision** (Player Collision) mit den Formen, an denen der Spieler anstößt.
- **Anzeigen** (Show): Hilfsmittel ein- und ausblenden, etwa das Bodenraster, Symbole für Lampen oder
  die Formen, mit denen die Physik rechnet.
- ein Knopf zum Maximieren eines einzelnen Viewports.

Unten links zeigt das **Achsenkreuz** die drei Richtungen: X rot, Y grün, Z blau. **Z zeigt nach oben.**

> **Probier's aus!** Stell über **Fenster › Viewport-Aufteilung** (Window › Viewport Layout) die
> Aufteilung auf **4 Viewports**. Klick in den perspektivischen Viewport und stell seinen Ansichtstyp
> auf **Oben** (Alt+J). Er springt von **Beleuchtet** auf **Drahtgitter**: Im Bauplan geht es ums
> genaue Ausrichten, und dafür zeigt das Drahtgitter die Kanten am klarsten. Alt+G bringt die
> Perspektive zurück. Mit dem Maximieren-Knopf holst du einen Viewport groß und wieder zurück.

### Durch die Welt fliegen

Mit gedrückter rechter Maustaste fliegst du wie in einem Computerspiel durch die Welt (der
Flugmodus, in der Unreal Engine „Fly Mode“). Das Wichtigste:

| Eingabe | Wirkung |
|---|---|
| Rechte Maustaste + W A S D | vor, links, zurück, rechts fliegen |
| … + Q / E | runter / hoch |
| … + Mausrad | schneller oder langsamer fliegen |
| Mausrad allein | vor und zurück |
| F | den ausgewählten Actor ins Bild holen |

Alle weiteren Kombinationen (Kreisen, Schwenken, Kamera-Lesezeichen) stehen in
[Kapitel 17](17-referenz.md).

**Mit den Fingern** steuerst du wie in einem 3D-Betrachter: Ein Finger kreist um den Punkt darunter, zwei
Finger zoomen und verschieben, Tippen wählt aus, zweimal Tippen holt die Auswahl ins Bild, langes Halten
öffnet das Kontextmenü.

> **Stell dir vor …** du ziehst eine Landkarte mit zwei Fingern größer: Der Ort unter deinen Fingern
> bleibt unter deinen Fingern, egal ob du auf eine Socke oder auf einen Häuserblock zoomst.

---

## 11.5 Auswählen und Bewegen

Bevor du etwas veränderst, wählst du es aus. Bewegt wird dann **nur** mit dem Gizmo, dem bunten
Anfasser. Ein Ziehen mit der linken Maustaste zieht dagegen immer nur einen **Auswahlrahmen** auf, auch
wenn du auf einem Actor anfängst. So verschiebst du nie aus Versehen etwas, wenn du eigentlich nur
auswählen wolltest.

### Auswählen

- Im Viewport: Klick wählt aus, Umschalt+Klick fügt hinzu, Strg+Klick schaltet um, ein Klick ins
  Leere hebt die Auswahl auf.
- Im Outliner wirkt Umschalt+Klick wie bei einer Dateiliste: Es fügt **alles zwischen** dem zuletzt
  gewählten und dem angeklickten Actor hinzu. Strg+Klick nimmt einzelne dazu oder heraus.
- Linkes Ziehen fängt alle Actors ein, die im Rahmen liegen.
- Lampen und leere Actors haben keine sichtbare Form; du wählst sie über ihr Symbol.
- Der **zuletzt** gewählte Actor ist der wichtigste: An ihm sitzt das Gizmo.

### Das Gizmo

- **Verschieben (W)**, englisch Move: drei Pfeile zum Verschieben entlang einer Richtung, kleine
  Quadrate für zwei Richtungen zugleich.
- **Drehen (E)**, englisch Rotate: drei Ringe zum Drehen.
- **Skalieren (R)**, englisch Scale: drei Griffe zum Strecken und ein Mittelgriff zum gleichmäßigen
  Vergrößern.

Der Griff unter dem Zeiger leuchtet gelb. Bewegt werden alle ausgewählten Actors, Kinder wandern mit.

Ein Knopf in der Toolbar, **Welt- / lokale Koordinaten** (World / local coordinates), entscheidet,
wonach sich das Gizmo richtet: nach der Welt oder nach dem Actor selbst (lokal). Das ist wie
„nach Norden“ gegenüber „nach vorn“: Norden ist für alle gleich,
„vorn“ hängt davon ab, wohin der Actor gerade schaut. Ein schräg gestelltes Auto schiebst du lokal
genau in seine Fahrtrichtung.

> **Probier's aus!** Setze aus **Inhaltsablage › Formen** (Content Drawer › Shapes) einen
> **Würfel** (Cube), wähle ihn, halte **Alt** und zieh am Verschiebe-Gizmo. Du ziehst eine **Kopie**
> heraus, das Original bleibt stehen. Drück während des
> Ziehens **Esc**, ist alles wie vorher.

### Einrasten

Snapping heißt Einrasten: Verschiebst du, springt der Actor in festen Schritten, anfangs alle
0,5 m; drehst du, in 15°-Schritten. So liegen Kanten sauber aneinander, und Wände stehen genau
gerade. Die Schrittweite wählst du in der Toolbar neben den Knöpfen **Am Raster ausrichten**
(Grid snap), **Drehung einrasten** (Rotation snap) und **Skalierung einrasten** (Scale snap).

> **Stell dir vor …** Magnete auf Kästchenpapier: Sie schnappen immer auf die nächste Linie, so dass
> alles ordentlich in einer Reihe steht.

### Outliner und Details

Der **Outliner** ist die Liste aller Actors. Hier wählst du aus, holst einen Actor mit einem
**Doppelklick** auf seinen Namen ins Bild, benennst um (F2 auf einen Actor, Doppelklick auf einen
Ordner), blendest mit dem Auge ein und aus und hängst durch Ziehen Actors
aneinander (11.3). Der Actor, den du beim Spielen steuerst, trägt das Abzeichen **SPIELER** (PLAYER).

**Details** zeigt die Eigenschaften der Auswahl: oben Name und **Transformation** (Transform), darunter
je Komponente einen Abschnitt. Wählst du mehrere Actors, siehst du, was alle gemeinsam haben, und
eine Änderung gilt für alle. Drei Kniffe:

- In Zahlenfelder darfst du **rechnen**: `2*3` oder `10/4+1`.
- **Zieh am Namen** eines Zahlenfelds seitwärts, um den Wert zu verändern, wie an einem Drehknopf.
- Ein kleiner Pfeil rechts neben einem Feld, **Auf Standard zurücksetzen** (Reset to default), setzt
  den Wert auf den Standard zurück.

![Ein ausgewählter Actor: am Actor das Gizmo, rechts in Details seine Transformation und Komponenten](../buch/bilder/studio-selected.png)

---

## 11.6 Rückgängig und Verlauf

Undo heißt Rückgängigmachen (**Strg+Z**, im Menü **Bearbeiten › Rückgängig** (Edit › Undo)).
Studio macht das auf die einfachste und sicherste Art: Nach
jedem Schritt merkt es sich den **ganzen Stand** des Levels. Rückgängig springt einfach zum vorigen
Stand zurück.

> **Stell dir vor …** du fotografierst nach jedem Handgriff deinen ganzen Basteltisch. Willst du zurück,
> legst du alles so hin wie auf dem vorigen Foto. Die Alternative wäre, sich nur die Handgriffe zu
> merken und sie rückwärts auszuführen: „Würfel 2 m nach links schieben“ wird zu „2 m nach rechts“.
> Das geht schief, sobald ein Rückwärts-Schritt nicht ganz genau passt. Mit Fotos kann das nicht
> passieren.

Studio hält bis zu **500 Schritte**. Ein langes Ziehen am Gizmo, an einem Zahlenfeld oder im Farbwähler
zählt dabei als **ein** Schritt, egal wie lange du ziehst: Studio erkennt, dass alles zu einem Handgriff
gehört. Und eine Änderung, die nichts ändert, macht keinen Schritt.

> **Probier's aus!** Verschieb ein paar Actors, ändere eine Farbe, lösche etwas. Dann
> **Bearbeiten › Verlauf...** (Edit › History...): Da steht jeder Schritt mit Namen. Ein Klick springt
> genau zu diesem Stand.

Beim Spielen und Simulieren ist das Level gesperrt; Studio meldet dann etwa „Beende das Spiel, um
rückgängig zu machen“ (Stop playing to undo).

---

## 11.7 Bilder, Töne und das Protokoll

Neben den Bauteilen hat die **Inhaltsablage** (Content Drawer) drei weitere Tabs.

**Texturen** (Textures) sind Bilder, die du auf Oberflächen legst, wie Tapete. Der Knopf
**Bild importieren** (Import image) holt eigene Fotos herein, ebenso ein Bild, das du einfach auf die
Seite ziehst. Ist dabei etwas ausgewählt, bekommt es das Bild
gleich. Ein **Doppelklick** legt eine Textur auf die Auswahl. Die Bilder stecken danach im Level selbst,
die Level-Datei bleibt also vollständig.

**Sounds:** Ein Klick hört einen Ton an, ein **Doppelklick** gibt ihn der Auswahl als Klangquelle,
Ziehen in den Viewport setzt eine Klangquelle an diese Stelle. **Audio importieren** (Import audio)
nimmt eigene Tondateien in den üblichen Formaten.

> **Achtung, Falle!** Du hörst nichts? Browser erlauben Ton erst nach dem ersten Klick oder
> Tastendruck. Einmal ins Studio klicken, dann klingt es.

Das **Ausgabeprotokoll** (Output Log) ist das Protokoll: alle Meldungen mit Uhrzeit, Warnungen und
Fehler farbig. Beim Spielen stehen hier auch Spielereignisse.

---

## 11.8 Welteinstellungen

Der Tab **Welteinstellungen** (World Settings) neben **Details** gilt nicht für einen Actor, sondern
fürs ganze Level. Er hat vier Gruppen:

- **Spiel** (Game): die Spielregeln. Wann gewinnt man (alle Orbs gesammelt, Ziel erreicht, Zeit
  überlebt, Punktzahl geschafft)? Gibt es ein Zeitlimit? Und **Kill-Z** (Kill Z): eine Höhe, unter die
  man nicht fallen darf; Z ist die Achse nach oben (Achsenkreuz, 11.4). Wer tiefer fällt, wird
  zurückgesetzt, etwa an den letzten Checkpoint, und spielt weiter; verloren hat er damit nicht.
- **Himmel und Licht** (Sky and light): Tageszeit, wie schnell der Tag vergeht, Sonnenstand, Nebel,
  Wolken, Boden.
- **Physik (Box3D)** (Physics (Box3D)): die Schwerkraft und wie genau und wie oft die Physik rechnet.
  Box3D ist die Physik-Engine, die in Kern steckt ([Kapitel 6](06-physik.md)).
- **Audio**: Lautstärken, Hall und den Doppler-Effekt, den Martinshorn-Effekt: Ein vorbeifahrendes
  Martinshorn klingt erst höher, dann tiefer ([Kapitel 7](07-audio.md)).

> **Probier's aus!** Öffne Orb Run und zieh in den **Welteinstellungen** am Namen von **Tageszeit**
> (Time of day) langsam hin und her. Die Stadt geht in die Nacht über, und die Fenster fangen an zu
> leuchten.

Alle Felder mit ihren Bereichen stehen im blauen Buch, Kapitel 11.

---

## 11.9 Spielen und Simulieren

Studio kennt drei Zustände: Bearbeiten, **Simulieren** (Simulate), wobei die Welt läuft und du
zuschaust, und **Spielen** (Play), wobei die Welt läuft und du mitspielst. Wichtig: Was beim
Simulieren und Spielen passiert, verschwindet beim **Stopp** (Stop) wieder. Dein Level bleibt unangetastet, außer du willst das
Ergebnis ausdrücklich behalten.

> **Stell dir vor …** du baust ein Dominospiel auf dem Tisch auf. Simulieren heißt: einmal anstoßen
> und zuschauen. Spielen heißt: selbst eine Figur sein, die durchläuft. Und nach dem Stopp steht alles
> wieder genau so da wie vorher, wie von Zauberhand neu aufgebaut.

### Spielen

**Spielen** (Alt+P) startet das Spiel im aktiven Viewport. Du steuerst den Spieler, die Kamera folgt ihm,
und das HUD (die Anzeige über dem Bild) zeigt Punkte, Zeit und Tempo, beim Soldaten Fadenkreuz,
Gesundheit und Munition. Am Computer fängt Studio dabei die Maus; ist sie frei, klick einmal ins Bild.
Bei Sieg oder Niederlage erscheint **Nochmal spielen** (Play again). **Esc** beendet das Spielen.

![Spielen im Viewport: das Hovercar in Orb Run mit HUD](../buch/bilder/studio-play.png)

**F8** lässt den Spieler los (**Spielen › Aus dem Spieler aussteigen** (Play › Eject from Player)):
Das Spiel läuft weiter, du fliegst frei herum und kannst Dinge greifen. Noch einmal **F8** übernimmt
ihn wieder (**Spielen › Spieler übernehmen** (Play › Possess Player)).

### Simulieren

**Simulieren** (Alt+S) lässt Physik und Regeln laufen, ohne dass du jemanden steuerst. Das Beste daran:
**Du kannst Dinge anfassen.** Linke Maustaste (oder Finger) auf einen beweglichen Körper greift ihn,
Ziehen bewegt ihn, und Loslassen **wirft** ihn mit Schwung.

> **Wusstest du?** Der Griff ist kein Trick, sondern echte Physik: Eine Art unsichtbares Gummiband
> zieht den gegriffenen Punkt zum Mauszeiger. Darum stößt der Körper überall an, kippt und schwingt wie
> jeder andere.

- **K**, im Menü **Spielen › Simulationsänderungen übernehmen** (Play › Keep Simulation Changes),
  übernimmt die Lage der ausgewählten Actors ins Level und beendet die Simulation.
- **Pause**, dann **.**, geht Bild für Bild weiter, wie ein Daumenkino.
- Unter **Fenster › Anzeigen** (Window › Show) kannst du sichtbar machen, womit die Physik rechnet:
  **Kollisionsformen** (Collision Shapes), **Gelenke** (Constraints) und
  **Kontaktpunkte** (Contact Points).

![Simulieren mit eingeblendeten Kollisionsformen](../buch/bilder/studio-simulate.png)

> **Probier's aus!** Lade
> **Datei › Beispiel-Levels › Physics Lab** (File › Example Levels › Physics Lab) und drück
> **Simulieren**. Greif die Seilbrücke, wirf eine Kiste gegen den Stapel, dreh am
> Scharnier. Dann stopp mit Esc, setz aus **Inhaltsablage › Physik** (Content Drawer › Physics) einen
> **Kistenstapel** (Crate Stack), starte wieder **Simulieren** und reiß ihn mit der Maus um. Wähle im
> Outliner die Kisten (die erste anklicken, die letzte mit Umschalt) und drück **K**: Der eingestürzte
> Stapel bleibt so liegen, als wäre er immer schon umgefallen.

![Physics Lab beim Simulieren: Seilbrücke, Tür, Pendel und Kistenstapel; die Statusleiste zählt Körper und Gelenke](../buch/bilder/studio-physics-lab.png)

### Zuschauen mit der Beobachterkamera

Schaltest du in den **Welteinstellungen** (World Settings) die **Beobachterkamera** (Observer camera)
ein, startet Spielen nicht hinter dem Spieler, sondern mit freier Kamera: Das Spiel läuft, du schaust
zu und kannst Dinge greifen wie beim Simulieren. **F8** übernimmt den Spieler, noch einmal F8 schaut
wieder zu. Das Level Micro Duck startet so.

### Am Telefon und mit dem Gamepad

Kern merkt selbst, womit du gerade spielst (Finger, Tastatur oder Gamepad), und schaltet um. Am
Touchscreen liegen unten zwei Sticks: Der linke bewegt, der rechte schaut (**SCHAUEN**, LOOK). Ihre
Beschriftung und die Knöpfe daneben passen sich der Figur an: Beim Soldaten heißt der linke Stick
**LAUFEN** (MOVE), daneben liegen etwa **SPRUNG** (JUMP), **LADEN** (RELOAD) und **ZIELEN** (AIM);
beim Hovercar heißt er **LENKEN / GAS** (STEER / GAS), dazu kommen **BOOST**, **DRIFT** und **LIFT**. Oben links stehen **⟵ ENDE** (⟵ EXIT) und **❚❚ PAUSE**.

![Orb Run am Telefon im Hochformat: oben ⟵ ENDE und ❚❚ PAUSE, unten die Touch-Sticks LENKEN / GAS und SCHAUEN](../buch/bilder/phone-play-390.png)

Ein **Gamepad** (Spielecontroller) funktioniert im Studio und im fertigen Spiel: linker Stick bewegen,
rechter schauen. Kleines Wackeln der Sticks ignoriert Kern. Die ganze Belegung und alle
Touch-Knöpfe stehen im blauen Buch, Kapitel 11.

---

## 11.10 Logik: Regeln ohne Code

Jeder Actor und das Level selbst können einen **Logik-Graphen** tragen: Statt Programmcode zu
schreiben, verbindest du Kästchen (**Knoten**) mit Drähten. Jedes Kästchen tut eine Sache, die Drähte
sagen, was danach kommt. In der Unreal Engine heißt so etwas **Blueprint** (Bauplan), darum taucht das
Wort auch im Studio auf. Wie die Graphen innen funktionieren, erklärt [Kapitel 9](09-logik.md).

> **Stell dir vor …** eine Kettenreaktion aus Dominosteinen: Ein Ereignis stößt den ersten Stein an,
> und der Draht leitet den Stoß zum nächsten Kästchen weiter, das etwas tut.

Du öffnest den Logik-Editor über **Logik** (Logic) in der Toolbar (Logik des Levels), über **Logik**
in **Details** oder per Rechtsklick › **Logik bearbeiten** (Edit Logic) bzw.
**Logik hinzufügen (Blueprint)** (Add Logic (Blueprint)) (Logik eines Actors). Er legt sich über die
Viewports; der Knopf **Viewport** bringt dich zurück. Im Logik-Editor selbst bleiben die
Beschriftungen englisch, auch in der deutschen Oberfläche.

Ein leerer Graph bietet Startknöpfe an, das sind die Ereignisse, die alles ins Rollen bringen:
**Begin Play** (beim Spielstart), **On Interact** (wenn der Spieler etwas benutzt), **On Begin Overlap**
(wenn jemand eine Zone betritt) und **Event Tick** (in jedem Augenblick). Unter **Templates…** gibt
es fertige Beispiele: eine Tür, die beim Benutzen aufgeht, eine Zone, die Punkte gibt, einen Spawner,
der regelmäßig Würfel erzeugt, eine Ampel und eine Micro Duck, die vor Hindernissen abdreht.

> **Probier's aus!** Neues Level **Basis** (Basic), dann **Logik** in der Toolbar und
> **Begin Play**. Am Knoten steht rechts der Ausgang „then“. Zieh von dort mit der Maus einen Draht ins Leere und lass los: Es
> öffnet sich eine Suche. Tippe „show message“ und drück Enter. Der neue Knoten hängt schon am Draht;
> trag in sein Textfeld einen Satz ein. Zurück mit **Viewport**. Das Level hat noch keine
> Spielfigur, und ohne Figur gibt es nichts zu spielen; setz deshalb aus
> **Inhaltsablage › Spieler** (Content Drawer › Players) eine **Rollende Kugel** (Rolling Ball), sie
> wird dein Spieler. Jetzt **Spielen** (Play): Dein Text steht im HUD. Dein erstes Programm, ganz
> ohne Code!

**Compile** (F7) prüft den Graphen schon vorher und zeigt Fehler mit einer Ampel an.

> **Für Neugierige:** Beim Start übersetzt Kern jeden Graphen blitzschnell in Maschinencode, damit er
> so schnell läuft wie geschriebenes Programm. Der Knopf **C++** speichert den Graphen sogar als
> lesbaren Programmtext ([Kapitel 12](12-eigene-spiele.md)). Alle Knoten und Tasten des Logik-Editors
> stehen in [Kapitel 9](09-logik.md).

---

## 11.11 Die Micro Duck steuern

Die Micro Duck ist ein kleiner zweibeiniger Roboter von Pollen Robotics. In Kern läuft sie mit
denselben trainierten **Netzen** wie der echte Roboter: Programmen, die das Laufen nicht vorgeschrieben
bekommen, sondern durch Üben gelernt haben. Wie das funktioniert, erzählt
[Kapitel 13](13-micro-duck.md); hier geht es ums Bedienen.

Das Beispiel-Level **Datei › Beispiel-Levels › Micro Duck** (File › Example Levels › Micro Duck) ist
ein Zimmer in Entengröße mit Möbeln, einem Ball und zwei Socken. **Spielen** (Play) startet mit der
Beobachterkamera: Die Ente läuft von selbst, erkundet den Raum, weicht Hindernissen aus, spielt Ball und steht nach einem Sturz wieder auf. Hebst du sie am
Rumpf hoch, hält sie still und läuft weiter, sobald du sie absetzt. Mit **F8** übernimmst du sie.

| Taste | Bewegung |
|---|---|
| W / S, A / D | laufen, drehen |
| Leertaste | quaken |
| 1 / 2 | hinsetzen oder aufstehen / etwas aufheben |
| 3 / 4 | mit links / rechts kicken |
| 7 | springen |

Weitere Tasten und die Belegung am Telefon und Gamepad stehen in [Kapitel 17](17-referenz.md).

Während des Spiels zeigt Studio oben rechts, was die Ente wahrnimmt: das Bild ihrer **Kopfkamera** und
das Gitter ihres **Abstandssensors** (ToF, ein Abstandsmesser mit Licht, der 8 × 8 Richtungen
gleichzeitig abtastet). Rot ist ein Hindernis, heller je näher, grün der Boden, gelb zu nah.

> **Probier's aus!** Lade das Micro-Duck-Level, drück **Spielen** und schau ihr eine Weile zu. Dann
> greif den Ball mit der Maus, wie beim Simulieren, und leg ihn woanders hin. Zum Schluss **F8** und
> Taste **7**: Die Ente springt!

Jede Bewegung der Ente ist ein eigenes Netz. Das Laufen ist fest eingebaut, die übrigen Bewegungen
(Sitzen, Kicken, Springen …) holt Studio aus dem Internet nach, sobald ein Level eine Ente enthält.
Klappt das nicht, versucht Studio es später erneut; bis dahin läuft die Ente mit ihrem eingebauten
Laufnetz weiter.
Trainierst du mit dem Kern Lehrer selbst ein neues ([Kapitel 14](14-lehrer.md)), nimmt Studio
automatisch deins. Die Einzelheiten stehen in [Kapitel 13](13-micro-duck.md).

---

## 11.12 Speichern, Autosave und Einstellungen

Ein Level ist **eine einzige Datei** mit der Endung `.klevel`. Darin steckt wirklich alles: die
Actors, die Logik und sogar deine importierten Bilder und Töne. Du kannst sie also einfach
verschicken.

- **Strg+S** (**Datei › Speichern** (File › Save)) speichert. Je nach Browser fragt Studio nach dem
  Ort oder legt die Datei in deine Downloads.
- **Strg+O** (**Datei › Level öffnen...** (File › Open Level...)) öffnet eine Level-Datei.
- Oder du **ziehst** eine Datei auf die Seite: Levels öffnen sich, Bilder werden Texturen,
  Tondateien Sounds.

### Autosave

> **Stell dir vor …** ein Schutzengel, der dir über die Schulter schaut, aber nie nervt. Er schreibt
> sich heimlich auf, was du noch nicht gespeichert hast, und zerreißt den Zettel, sobald du speicherst.

Genau so arbeitet der Autosave: Kurz nach jeder Änderung legt Studio eine Kopie im Browser ab, aber nur,
solange etwas ungespeichert ist. Beim nächsten Start bietet es **Wiederherstellen** (Restore) an, um das Level
wiederherzustellen, oder **Schließen** (Dismiss), um die Kopie zu
verwerfen. Ein wiederhergestelltes Level gilt weiter als **ungespeichert**, denn
sein Inhalt steht ja noch in keiner Datei. Bis du es speicherst, bietet Studio es bei jedem Start
wieder an.

> **Achtung, Falle!** Der Autosave ist ein Rettungsring, kein Speicherort. Er hält nur **eine** Kopie im
> Browser. Wer den Browserspeicher leert, verliert sie. Wichtige Levels speicherst du mit Strg+S als
> Datei.

### Einstellungen

Unter **Bearbeiten › Einstellungen...** (Edit › Preferences...) stellst du ein, wie schnell die
Kamera fliegt, wie empfindlich die Maus ist
und wie fein das Einrasten. Studio merkt sich dabei nur, was **du selbst** geändert hast. Alles andere
folgt den Standardwerten der jeweiligen Version; bringt eine neue Version bessere Standards mit,
bekommst du sie also automatisch, ohne deine eigenen Wünsche zu verlieren.
**Bearbeiten › Einstellungen zurücksetzen...** (Edit › Reset Settings to Defaults...) setzt alles
zurück; deine Levels bleiben dabei unberührt.

---

## 11.13 Spiel exportieren

Jetzt kommt der magische Moment: Aus deinem Level wird ein fertiges Spiel, das jeder ohne Studio
spielen kann.

1. **Datei › Spiel verpacken (HTML)...** (File › Package Game (HTML)...) oder der Paket-Knopf in der
   Toolbar.
2. **Spieltitel** (Game title) eintragen.
3. **Verpacken** (Package) klicken. Studio lädt eine Datei `<Titel>.html` herunter und bietet
   **Spielen** (Play it) an.

HTML ist das Dateiformat von Webseiten. Darum öffnet ein Doppelklick auf die Datei sie in deinem
Browser, genau wie eine Webseite. Diese **eine Datei** ist ein komplettes Spiel: Die Engine und dein Level stecken darin. Sie braucht
keinen Server und kein Internet, läuft in jedem Browser, auch direkt von der Festplatte und am Telefon,
und startet sofort das Spiel statt des Editors. Die Steuerung ist dieselbe wie beim Spielen im Studio;
**P** pausiert, **R** startet nach Sieg oder Niederlage neu. Online spielen kann die Datei auch
([Kapitel 10](10-netzwerk.md)).

> **Achtung, Falle!** Spiele mit der Micro Duck brauchen beim Weitergeben zwei Ordner mit den
> nachladbaren Bewegungen der Ente neben der HTML-Datei, sonst kann sie nur laufen. Und das Aussehen
> der Ente darf nur **nicht kommerziell** genutzt werden. Was du darfst und was du mitgeben musst,
> steht in [Kapitel 12](12-eigene-spiele.md).

> **Probier's aus!** Exportiere Orb Run und starte die Datei mit **Spielen**. Dann bring sie aufs
> Telefon: Am zuverlässigsten legst du sie in einen Cloud-Speicher, den du auch am Telefon benutzt
> (E-Mail-Anhänge mit HTML-Dateien blockieren manche Mailprogramme). Öffne sie am Telefon; zeigt das
> Telefon sie nicht gleich als Spiel, wähl „Öffnen mit“ und deinen Browser. Die Touch-Sticks
> erscheinen von selbst.

---

## 11.14 Mehrspieler

Wie Kern Spieler übers Netz verbindet, erklärt [Kapitel 10](10-netzwerk.md). Hier nur die Bedienung.

### Mehrere Spieler auf einem Rechner

1. **Spielen › Anzahl der Spieler** (Play › Number of Players) und dort
   **2 Spieler (Listen-Server)** (2 Players (listen server)), bis zu 4.
2. **Spielen**. Aus der Einzelansicht schaltet Studio auf vier Viewports: 1 zeigt den Server, 2 den ersten Gast. Jeder hat
   sein eigenes HUD.
3. Nur der Spieler im aktiven Viewport bekommt deine Eingabe. Ein Klick in einen anderen wechselt.
4. **Spielen › Netzwerk-Emulation** (Play › Network Emulation) macht die Leitung absichtlich
   schlechter, von **Perfekt** (Perfect) bis **Sehr schlecht (300 ms, 20 % Verlust)** (Terrible (300 ms, 20% loss)).

Was du dabei beobachten kannst, beschreibt die Mission in [Kapitel 10](10-netzwerk.md), Abschnitt 10.8.

![Zwei Spieler auf einem Rechner: Server und Gast in eigenen Viewports, jeder mit eigenem HUD](../buch/bilder/studio-multiplayer.png)

### Online zwischen Browsern

Alle öffnen dasselbe Level. Einer wählt
**Spielen › Online-Spiel hosten...** (Play › Host Online Game...), die anderen
**Spielen › Online-Spiel beitreten...** (Play › Join Online Game...), alle denselben Raumnamen im Feld
**Raum** (Room, Standard „kern“). Ohne Eintrag im Feld **Relay (optional)** finden sich
nur Tabs desselben Browsers; für verschiedene Rechner tragen alle dort die Adresse desselben Relays ein (Kapitel 10, Abschnitt 10.11).
**Spielen › Online-Spiel verlassen** (Play › Leave Online Game) verlässt das Spiel.

---

## 11.15 Am Telefon und auf schmalen Bildschirmen

Auf kleinen Bildschirmen räumt Studio die Werkstatt um, damit die Welt Platz hat:

- **Der Viewport bekommt die ganze Fläche.** **Outliner**, **Details** und
  **Welteinstellungen** (World Settings) öffnest du über den Knopf
  **Outliner und Details** (Outliner and Details), ein Symbol ohne Text; sie klappen als Blatt auf,
  im Hochformat von unten.
- **Spielen und Simulieren sind immer einen Tipp entfernt.** Die Knöpfe zum Spielen und Simulieren
  stehen in einem Dock am rechten Rand der Toolbar, das stehen bleibt, während der Rest der Leiste
  seitwärts scrollt.
- Weniger Wichtiges wird ausgeblendet.
- **Nichts bleibt gedrückt hängen.** Hebt sich der letzte Finger vom Bildschirm oder wechselt die
  Seite in den Hintergrund, lässt Kern alle Eingaben los. Sonst liefe deine Figur weiter, als hielte
  noch jemand den Stick.

![Studio in einem schmalen Fenster: Der Viewport bekommt fast die ganze Fläche](../buch/bilder/studio-narrow.png)

![Studio am Telefon im Hochformat: rechts in der Toolbar das Dock mit Spielen und Simulieren, oben rechts das Symbol für Outliner und Details](../buch/bilder/phone-dock-390.png)

> **Probier's aus!** Öffne Studio auf deinem Telefon. Tipp im Dock auf Spielen, dann auf
> **⟵ ENDE** (⟵ EXIT). Öffne **Outliner und Details**, wähle im Outliner einen Actor und ändere in
> **Details** seine Farbe.

---

## 11.16 Warum Studio so gebaut ist

### Ein Programm für Editor und Spiel

Das ganze Studio, also Auswählen, Gizmos, Rückgängig, Spielen, Speichern, ist in **C++** geschrieben, der
Programmiersprache, in der auch die Engine steckt. Es läuft im selben Programmpaket wie das fertige
Spiel. Das hat zwei große Vorteile. Erstens gibt es keine zweite Fassung, die anders rechnen könnte:
Was im Editor klappt, klappt genauso im exportierten Spiel. Zweitens lässt sich der ganze Editor
automatisch testen, ganz ohne Browser.

### Alles ist ein Bestellzettel

> **Stell dir vor …** ein Restaurant. Die Browserseite ist der Kellner: Sie nimmt deine Bestellung auf
> (Klick, Taste), bringt sie in die Küche und trägt das Essen raus (zeichnet die Listen und Knöpfe).
> Gekocht wird nur in der Küche, im C++-Teil. Und jede Bestellung ist ein ordentlicher Zettel.

**Alles**, was du im Studio tust, auch jeder Menüeintrag, wird zu so einem Zettel, einem kurzen
**Befehl**. Weil jede Handlung ein Zettel ist, kann ein automatischer Test genau dieselben Zettel
schreiben wie du und so das ganze Studio bedienen. Die Browserseite selbst erledigt nur, was ein
Browser kann und C++ nicht: Dateien öffnen und speichern, die Zwischenablage, Bilder und Töne
entpacken, den Ton abspielen und Netzpakete verschicken.

> **Wusstest du?** Ein automatischer Browser-Test bedient das ganze Studio wie ein Mensch: Menüs,
> Fliegen, Werkzeuge, Rückgängig, Spielen, Simulieren, Logik, Micro Duck, Mehrspieler, Export und die
> Telefon-Ansicht. Die meisten Bilder in diesem Kapitel stammen aus genau diesem Test.

Die genauen Befehle und Schnittstellen stehen im blauen Buch, Kapitel 11, und in
[Kapitel 17](17-referenz.md).

---

## 11.17 Die wichtigsten Tasten

**F1** zeigt die Kürzel jederzeit im Studio. Für den Anfang reichen diese:

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Auswählen / Verschieben / Drehen / Skalieren |
| Strg+Z / Strg+Y | rückgängig / wiederholen |
| Strg+C / Strg+V, Strg+D | kopieren / einfügen, verdoppeln |
| Entf | löschen |
| F | Auswahl ins Bild holen |
| Strg+S / Strg+O | speichern / öffnen |
| Strg+Leertaste | Inhaltsablage auf und zu |
| Alt+P / Alt+S | Spielen / Simulieren |
| Esc | Spielen beenden, Ziehen abbrechen |
| F8 | Spieler loslassen bzw. übernehmen |

> **Wusstest du?** Kopierte Actors landen als Text in der Zwischenablage deines Computers. Ein Strg+V
> in einem **anderen Tab** fügt sie dort ein. So trägst du Bauteile von einem Level ins nächste.

Weitere Tasten stehen in [Kapitel 17](17-referenz.md), die des Logik-Editors in
[Kapitel 9](09-logik.md). Die vollständige Liste steht im blauen Buch, Kapitel 11.

---

## Das nimmst du mit

- Ein Level besteht aus **Actors**, und was ein Actor kann, bestimmen seine **Komponenten**, wie Steine
  auf einer Grundplatte aus dem Baukasten.
- **Viewports** zeigen die Welt als Foto (perspektivisch) oder als Bauplan (orthografisch).
- Auswählen mit Klick oder Rahmen, bewegen **nur mit dem Gizmo**; Einrasten hält alles ordentlich.
- **Rückgängig** (Undo) merkt sich nach jedem Schritt den ganzen Stand, darum geht es nie schief.
- **Simulieren** (Simulate) heißt zuschauen und anfassen, **Spielen** (Play) heißt mitspielen, und
  nach dem Stopp ist alles wie vorher (außer du behältst es mit **K**).
- Der Logik-Editor gibt dem Level Regeln, ganz ohne Code.
- **Spiel verpacken** (Package Game) macht aus deinem Level eine HTML-Datei, die überall läuft.
- Editor und Spiel sind dasselbe C++-Programm; jede Handlung ist ein Befehl.

## Knobelecke

1. Du ziehst mit der linken Maustaste über einen Würfel. Warum bewegt er sich nicht?
2. Du hast einen Kistenstapel einstürzen lassen und willst ihn genau so behalten. Wie?
3. Eine Kiste soll vom Tisch fallen, bleibt aber in der Luft hängen. Was fehlt ihr?
4. Warum kann Rückgängig bei Studio nie „danebengehen“?
5. Was ist der Vorteil, dass Editor und exportiertes Spiel dasselbe Programm sind?

### Lösungen

1. Linkes Ziehen zieht immer einen Auswahlrahmen auf. Bewegt wird nur mit dem Gizmo (W).
2. Beim **Simulieren** (Simulate) einstürzen lassen, die Kisten auswählen, **K** drücken.
3. Die Physik-Komponente **Starrkörper** (Rigid Body), **Mobilität** (Mobility) auf **Dynamisch (Physik simulieren)**.
   Ohne sie bleibt ein Actor, wo er steht.
4. Studio rechnet keine Schritte rückwärts, sondern springt zu einem gemerkten Gesamtstand zurück.
5. Was du im Editor siehst, verhält sich genau so im fertigen Spiel, und der Editor lässt sich
   automatisch testen.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
