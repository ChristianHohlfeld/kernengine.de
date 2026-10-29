# 11 Kern Studio: Deine Werkstatt im Browser

> **Kurz gesagt:** Kern Studio ist der Editor der Kern-Engine. Du baust darin Levels aus Actors und
> Komponenten, spielst sie mit einem Klick direkt im Fenster und machst daraus eine einzige HTML-Datei,
> die überall läuft. Dieses Kapitel ist eine geführte Tour: Mach am besten Studio auf und klick mit.

> **Begriffe vorab:**
> - **Editor** – ein Bearbeitungsprogramm, hier: die Werkstatt, in der du Spielwelten baust.
> - **Level** – eine Spielwelt, alles, was zu einem Spiel gehört, in einer Datei.
> - **Actor** – ein Ding im Level: ein Würfel, eine Lampe, eine Spielfigur, eine Zone.
> - **Komponente** – eine Fähigkeit, die du einem Actor ansteckst: „hat eine Form“, „leuchtet“,
>   „fällt runter“.
> - **Transform** – Ort, Drehung und Größe eines Actors.
> - **Mesh** – das Drahtgitter aus Dreiecken, aus dem die sichtbare Form eines Dings besteht.
> - **Gizmo** – der bunte Anfasser mit Pfeilen und Ringen, mit dem du einen Actor verschiebst,
>   drehst oder größer ziehst.
> - **Simulation** – das Level läuft von selbst: Dinge fallen, rollen, Regeln greifen.

Die Tour geht so: Erst schauen wir uns in der Werkstatt um. Dann lernst du, woraus ein Level besteht,
wie du es von allen Seiten anschaust, Dinge auswählst und bewegst und Fehler rückgängig machst. Danach
erweckst du dein Level mit **Play** und **Simulate** zum Leben, gibst ihm mit dem Logik-Editor Regeln
und machst daraus ein fertiges Spiel. Die Begriffe (Actor, Viewport, Outliner …) sind dieselben wie in
großen Profi-Werkzeugen der Spielebranche, etwa der Unreal Engine; wer die kennt, fühlt sich sofort zu
Hause.

![Kern Studio mit dem Level Orb Run: Menüleiste, Toolbar, Viewport, Outliner und Details](../buch/bilder/studio.png)

> **Wusstest du?** Was du im Studio siehst, rechnet **genau derselbe Code**, der später dein
> exportiertes Spiel ausführt. Was im Editor funktioniert, funktioniert also auch im fertigen Spiel
> (11.16).

> **Achtung, Falle!** Auf einem deutschen System zeigt Studio deutsche Beschriftungen („Datei“,
> „Welteinstellungen“ …). Dieses Kapitel nennt die englischen. Klick unten rechts in der Statusleiste
> auf **DE | EN**, um umzuschalten; Studio merkt sich die Wahl. Was du selbst eintippst, etwa Befehle
> in der Konsole, ist in beiden Sprachen gleich.

---

## 11.1 Studio öffnen

Studio ist eine Webseite: Du öffnest auf https://kernengine.de/ den Editor und legst los, auch am
Telefon. Installieren musst du nichts.

Beim allerersten Mal ist das Beispiel-Level **Orb Run** geladen: eine Stadt, die der Computer nach
Regeln selbst erzeugt hat, mit Verkehr, einem schwebenden Hovercar, Energiekugeln (Orbs) zum Einsammeln
und einem Tag, der in die Nacht übergeht. Später öffnet Studio das Level, an dem du zuletzt gearbeitet
hast. Gab es dabei ungespeicherte Änderungen, bietet es an, sie wiederherzustellen (11.12).

> **Probier's aus!** Öffne Studio, klick einmal ins Bild und drücke **Alt+P** (Play). Schon fährst du
> mit dem Hovercar durch Orb Run: W A S D lenkt, Shift gibt Schub. **Esc** bringt dich zurück in den
> Editor.

> **Für Neugierige:** Wer Kern selbst baut, kann Studio auch als einzelne Datei ohne Internet öffnen
> oder direkt aus dem Quellcode starten. Wie das geht, steht im blauen Buch, Kapitel 11.

### Als App installieren

Du kannst Studio wie eine App aufs Gerät holen: In Chrome und Edge über den Knopf **⤓ Install** oben,
auf iPhone und iPad erklärt derselbe Knopf die Schritte **Teilen** und **Zum Home-Bildschirm**.
Installiert startet Studio im Vollbild und funktioniert auch ohne Internet. Gibt es eine neue Version,
holt Studio sie von selbst. Hast du gerade Ungespeichertes, fragt es vorher (**Update** oder
**Later**), und dein Level kommt über den Autosave mit.

---

## 11.2 Die Werkstatt im Überblick

Studio ist aufgebaut wie eine Werkstatt: ein großer Arbeitsplatz in der Mitte, drumherum Listen,
Werkzeuge und Vorräte. Jeder Bereich hat genau eine Aufgabe.

> **Stell dir vor …** eine Werkstatt. In der Mitte die Werkbank mit deinem Modell (der Viewport). Rechts
> an der Wand die Inventarliste (Outliner) und darunter der Zettel mit den Maßen des Teils in deiner Hand
> (Details). Unten eine Schublade voller Bauteile (der Content Drawer). Und ganz oben das Regal mit den
> Werkzeugen (Toolbar).

| Bereich | Lage | Was drin ist |
|---|---|---|
| Menüleiste | ganz oben | Menüs, Levelname |
| Toolbar | darunter | Speichern, Rückgängig, Hinzufügen, Werkzeuge, Play |
| Viewports | Mitte | 1, 2 oder 4 Fenster in die 3D-Welt |
| Outliner | rechts oben | die Liste aller Actors |
| Details / World Settings | rechts unten | Eigenschaften des gewählten Actors bzw. des ganzen Levels |
| Content Drawer | unten, klappbar | Bauteile, Bilder, Töne, Protokoll |
| Statusleiste | ganz unten | Meldungen, Konsole, Ton |

Die Trennlinien zwischen den Bereichen kannst du ziehen; Studio merkt sich die Größen. Ein Punkt neben
dem Levelnamen heißt: Es gibt ungespeicherte Änderungen.

### Die Menüs in einem Satz

- **File:** neues Level, öffnen, speichern, Bild importieren, Spiel exportieren, Beispiel-Levels.
- **Edit:** Rückgängig, Verlauf (**History…**), Kopieren und Einfügen, Einstellungen.
- **Window:** Aufteilung der Fenster, Ansichten, Content Drawer, Protokoll, Vollbild.
- **Play:** Play, Simulate, Pause, Stop, Mehrspieler.
- **Help:** die Tastenübersicht (F1).

**File › New Level** bietet sechs Vorlagen als Karten (Doppelklick erstellt sofort):

| Vorlage | Was drin ist |
|---|---|
| Orb Run | Nachtstadt, Hovercar, Orbs zum Einsammeln |
| Physics Lab | alle Arten von Gelenken: Scharnier, Schiene, Rad, Seilbrücke, Motor, dazu ein Kistenstapel |
| Shooting Range | Ego-Shooter-Soldat, Ziele, Deckung, mehrspielerfähig |
| Micro Duck | die Laufroboter-Ente aus [Kapitel 13](13-micro-duck.md) |
| Basic | Boden, Sonne, Himmel: ein sauberer Anfang |
| Empty | gar nichts |

### Die Toolbar

Die Leiste unter den Menüs hat viele Knöpfe. Die wichtigsten, von links nach rechts:

1. **Diskette:** speichern.
2. **Pfeile:** rückgängig und wiederholen.
3. **+ Add:** ein Bauteil hinzufügen.
4. **Werkzeuge:** Auswählen, Verschieben, Drehen, Skalieren (Tasten Q, W, E, R).
5. **Play-Gruppe:** Play, Simulate, Pause, Stop.

Dazwischen liegen Knöpfe für das Einrasten (11.5), die Aufteilung der Fenster, den Logik-Editor
(**Logic**, 11.10) und den Export (11.13).

### Das Kontextmenü

Ein **Rechtsklick** auf einen Actor (am Touchscreen: lange halten) öffnet ein Menü mit allem, was du mit
genau diesem Ding tun kannst: umbenennen, kopieren, löschen, zum Spieler machen, ihm Logik geben, ihn an
einen anderen hängen oder mit einem Gelenk verbinden. Mit **Place Actor ▸** setzt du ein neues Bauteil
genau an die geklickte Stelle. Die vollständige Liste steht im blauen Buch, Kapitel 11.

### Statusleiste und Konsole

Ganz unten stehen die letzte Meldung, der Lautsprecher (Ton an/aus), wie viele Actors ausgewählt sind
und die laufende Version. Der Knopf **Cmd** öffnet die **Konsole**: eine Zeile, in die du Befehle
tippst, statt zu klicken. `help` listet alle auf. Ein paar Beispiele: `spawn physics_cube` setzt eine
Physik-Kiste, `tod 0.8` stellt die Tageszeit, `gravity 0 0 -1.6` ändert die Schwerkraft.

Die drei Zahlen bei `gravity` sagen, in welche Richtung und wie stark es zieht: links/rechts,
vorn/hinten, oben/unten. In Kern zeigt die dritte Achse nach oben, das Minus heißt also: nach unten.
1,6 ist die Schwerkraft des Mondes, auf der Erde sind es etwa 9,8.

> **Probier's aus!** Neues Level **Basic**. Klick unten auf **Cmd** und tippe `gravity 0 0 -1.6`, dann
> ein paar Mal `spawn physics_cube`. Drück **Simulate** (Alt+S): Die Kisten sinken gemächlich zu Boden,
> wie auf dem Mond. Ein `gravity` ohne Zahlen stellt die Erdschwerkraft wieder her.

---

## 11.3 Ein Level aus Actors und Komponenten

Ein Level ist eine Sammlung: die Einstellungen der Welt, die Actors, Bilder, Töne und die Logik. Jeder
Actor hat einen Namen, sein Transform (Ort, Drehung, Größe) und eine Reihe von Komponenten. Was ein
Actor **ist**, ergibt sich allein aus seinen Komponenten. Gemessen wird in Metern, Sekunden und
Kilogramm.

> **Stell dir vor …** ein Actor ist eine leere Lego-Platte. Komponenten sind die Steine, die du
> draufsteckst: Ein Form-Stein macht ihn sichtbar, ein Physik-Stein lässt ihn fallen, ein Pickup-Stein
> macht ihn einsammelbar. Kombinieren erlaubt!

Ein Beispiel: Eine Kiste, die vom Tisch fallen soll, braucht eine Form (**Static Mesh**) und Physik
(**Rigid Body**). Steckst du noch **Pickup** dazu, kann der Spieler sie einsammeln. Dieselbe Kiste
ohne Physik schwebt einfach dort, wo du sie hingestellt hast.

| Komponente | Was sie tut |
|---|---|
| **Static Mesh** | Form (Würfel, Kugel, Zylinder, Rampe …) mit Farbe und Material |
| **Rigid Body** | Physik: **statisch** (steht fest), **dynamisch** (fällt und rollt) oder **kinematisch** (bewegt sich nach Plan, lässt sich aber nicht schubsen) |
| **Collider** | eine Form zum Anstoßen, mit Reibung und Sprungkraft |
| **Constraint** | ein Gelenk: Scharnier, Schiene, Seil, Rad, Motor … |
| **Player Controller** | macht den Actor steuerbar (Hovercar, Ball, Läufer, Soldat …) |
| **Pickup** | einsammelbar: Punkte, Heilung, Munition |
| **Trigger** | eine unsichtbare Zone: Ziel, Gefahr, Checkpoint, Teleporter … |
| **Weapon, Health** | Waffe und Lebenspunkte |
| **Rotator, Mover** | dreht den Actor bzw. bewegt ihn hin und her |
| **Point Light, Audio Source** | Licht bzw. Klangquelle |
| **Procedural City** | eine ganze Stadt mit Verkehr |

Was die Felder jeder Komponente bedeuten, erklären [Kapitel 6](06-physik.md) (Physik),
[Kapitel 7](07-audio.md) (Ton) und [Kapitel 8](08-spiel-laufzeit.md) (Spielregeln). Neue Komponenten
steckst du in Details mit **+ Add** an.

### Actors anlegen

Fertige Actors liegen im **Content Drawer**, der Bauteil-Schublade unten (**Ctrl+Space**). Unter
**Place Actors** findest du über 40 Stück, sortiert in Gruppen:

- **Players:** Spielfiguren wie Soldat, Läufer, Hovercar, rollende Kugel, Drohne und die Micro Duck.
- **Gameplay:** Orbs und Sterne zum Sammeln, Heilpakete, Ziel, Gefahrenzone, Checkpoint, Sprungfeld,
  Teleporter, bewegliche Plattform, explodierendes Fass …
- **Shapes:** einfache Formen wie Würfel (Cube), Kugel (Sphere), Zylinder, Rampe.
- **Physics:** Kisten, Fässer, ein Kistenstapel, Pendel, Tür, Kette, Wippe und eine Socke in
  Entengröße.
- **Lights, Audio, World:** Lampen, Klangquellen und die Stadt.

Ein **Klick** setzt das Bauteil dorthin, wohin die Kamera blickt; **Ziehen** in den Viewport setzt es
genau unter die Maus. Der neue Actor ist gleich ausgewählt. Setzt du eine Figur aus **Players**, wird sie
dein Spieler.

### Eltern und Kinder

Du kannst einen Actor an einen anderen **anhängen**. Der angehängte heißt **Kind**, der andere
**Eltern-Actor**. Bewegst du den Eltern-Actor, wandert das Kind mit.

> **Stell dir vor …** du setzt einer Figur einen Hut auf und hängst ihn an sie. Läuft die Figur los,
> kommt der Hut mit. Ziehst du nur den Hut ein Stück zur Seite, bleibt die Figur stehen.

So geht's: mehrere Actors wählen, den künftigen Eltern-Actor zuletzt, dann Rechtsklick › **Attach to …**;
oder im Outliner einen Actor auf einen anderen ziehen. **Ctrl+G** packt die Auswahl unter einen neuen,
leeren Gruppen-Actor. **Ordner** im Outliner dagegen dienen nur der Ordnung und bewegen nichts.

> **Für Neugierige:** Beim Simulieren werden Kinder ohne eigene Physik fest mit dem Körper ihres
> Eltern-Actors verschweißt. Die Regeln dazu stehen in [Kapitel 6](06-physik.md).

### Gelenke mit „Connect“

Zwei Actors lassen sich mit einem **Gelenk** verbinden, sodass einer am anderen hängt, schwingt oder
sich dreht. Am schnellsten geht das so:

1. Den Actor wählen, der **hängen** soll.
2. Mit Ctrl den Actor dazuwählen, **an dem** er hängt, und zwar als Letztes.
3. Rechtsklick › **Connect with Constraint ▸** und eine Gelenkart wählen: **Hinge** (Scharnier),
   **Slider** (Schiene), **Ball** (Kugelgelenk), **Fixed** (fest), **Distance / rope** (Seil),
   **Wheel** (Rad) oder **Motor**.

Studio gibt dem hängenden Actor dabei Physik, falls er noch keine hat.

> **Probier's aus!** Setze einen **Cube** und schieb ihn mit dem Gizmo hoch in die Luft, daneben eine
> **Physics Cube**, ebenfalls ein Stück in der Luft. Wähle erst die Physics Cube, dann mit Ctrl den
> Cube, und verbinde sie mit **Distance / rope**. Jetzt **Simulate**: Die Kiste schwingt am Seil wie ein
> Pendel. Warum fällt der Cube nicht mit? Er hat keine Physik-Komponente, also bleibt er wie
> festgenagelt in der Luft. Er ist dein Aufhängepunkt.

### Der Spieler und die Prüfung vor dem Start

Play steuert den Actor mit **Player Controller**. Du bestimmst ihn, indem du eine Figur aus **Players**
setzt oder per Rechtsklick › **Make Player**. Gibt es keinen Spieler, startet Play stattdessen eine
Simulation und meldet „Nothing to control yet“.

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

- **Ansichtstyp:** Perspective oder ein Bauplan von oben, unten, links, rechts, vorn oder hinten.
- **View Mode**, wie gezeichnet wird: **Lit** (mit Licht und Schatten), **Unlit** (ohne Licht),
  **Wireframe** (nur das Drahtgitter der Meshes) oder **Player Collision** (die Formen, an denen der
  Spieler anstößt).
- **Show:** Hilfsmittel ein- und ausblenden, etwa das Bodenraster, Symbole für Lampen oder die
  Formen, mit denen die Physik rechnet.
- ein Knopf zum **Maximieren** eines einzelnen Viewports.

Unten links zeigt das **Achsenkreuz** die drei Richtungen: X rot, Y grün, Z blau. **Z zeigt nach oben.**

> **Probier's aus!** Stell über **Window › Viewport Layout** die Aufteilung auf **4 viewports**.
> Klick in den
> perspektivischen Viewport und stell seinen Ansichtstyp auf **Top** (Alt+J). Er springt von Lit auf
> Wireframe: Im Bauplan geht es ums genaue Ausrichten, und dafür zeigt das Drahtgitter die Kanten
> am klarsten. Alt+G bringt die Perspektive zurück. Mit dem Maximieren-Knopf holst du einen Viewport groß und
> wieder zurück.

### Durch die Welt fliegen

Mit gedrückter **rechter Maustaste** fliegst du wie in einem Computerspiel durch die Welt (der **Fly
Mode**). Das Wichtigste:

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

> **Stell dir vor …** du zoomst mit zwei Fingern auf eine Socke und danach auf einen Häuserblock. Es
> fühlt sich beide Male gleich an, denn Studio misst den Maßstab am Abstand zu dem Punkt unter deinen
> Fingern. Was unter den Fingern liegt, bleibt dort.

---

## 11.5 Auswählen und Bewegen

Bevor du etwas veränderst, wählst du es aus. Bewegt wird dann **nur** mit dem Gizmo, dem bunten
Anfasser. Ein Ziehen mit der linken Maustaste zieht dagegen immer nur einen **Auswahlrahmen** auf, auch
wenn du auf einem Actor anfängst. So verschiebst du nie aus Versehen etwas, wenn du eigentlich nur
auswählen wolltest.

### Auswählen

- **Klick** wählt aus, **Shift+Klick** fügt hinzu, **Ctrl+Klick** schaltet um, ein Klick ins Leere hebt
  die Auswahl auf.
- **Linkes Ziehen** fängt alle Actors ein, die im Rahmen liegen.
- Lampen und leere Actors haben keine sichtbare Form; du wählst sie über ihr Symbol.
- Der **zuletzt** gewählte Actor ist der wichtigste: An ihm sitzt das Gizmo.

### Das Gizmo

- **Move (W):** drei Pfeile zum Verschieben entlang einer Richtung, kleine Quadrate für zwei Richtungen
  zugleich.
- **Rotate (E):** drei Ringe zum Drehen.
- **Scale (R):** drei Griffe zum Strecken und ein Mittelgriff zum gleichmäßigen Vergrößern.

Der Griff unter dem Zeiger leuchtet gelb. Bewegt werden alle ausgewählten Actors, Kinder wandern mit.

Ein Knopf in der Toolbar entscheidet, wonach sich das Gizmo richtet: nach der **Welt** oder nach dem
Actor selbst (**lokal**). Das ist wie „nach Norden“ gegenüber „nach vorn“: Norden ist für alle gleich,
„vorn“ hängt davon ab, wohin der Actor gerade schaut. Ein schräg gestelltes Auto schiebst du lokal
genau in seine Fahrtrichtung.

> **Probier's aus!** Setze einen **Cube** (Content Drawer › Shapes), wähle ihn, halte **Alt** und zieh
> am Move-Gizmo. Du ziehst eine **Kopie** heraus, das Original bleibt stehen. Drück während des
> Ziehens **Esc**, ist alles wie vorher.

### Einrasten

**Snapping** heißt Einrasten: Verschiebst du, springt der Actor in festen Schritten, anfangs alle
0,5 m; drehst du, in 15°-Schritten. So liegen Kanten sauber aneinander, und Wände stehen genau
gerade. Die Schrittweite wählst du in der Toolbar.

> **Stell dir vor …** Magnete auf Kästchenpapier: Sie schnappen immer auf die nächste Linie, so dass
> alles ordentlich in einer Reihe steht.

### Outliner und Details

Der **Outliner** ist die Liste aller Actors. Hier wählst du aus, benennst um (Doppelklick auf einen
Ordner, F2 auf einen Actor), blendest mit dem **Auge** ein und aus und hängst durch Ziehen Actors
aneinander (11.3). Der Actor, den du in Play steuerst, trägt das Abzeichen **PLAYER**.

**Details** zeigt die Eigenschaften der Auswahl: oben Name und Transform, darunter je Komponente einen
Abschnitt. Wählst du mehrere Actors, siehst du, was alle gemeinsam haben, und eine Änderung gilt für
alle. Drei Kniffe:

- In Zahlenfelder darfst du **rechnen**: `2*3` oder `10/4+1`.
- **Zieh am Namen** eines Zahlenfelds seitwärts, um den Wert zu verändern, wie an einem Drehknopf.
- Ein kleiner Pfeil rechts neben einem Feld setzt den Wert auf den Standard zurück.

![Ausgewählter Actor mit Move-Gizmo und seinen Komponenten in Details](../buch/bilder/studio-selected.png)

---

## 11.6 Undo und History

Undo heißt Rückgängigmachen (**Ctrl+Z**). Studio macht das auf die einfachste und sicherste Art: Nach
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

> **Probier's aus!** Verschieb ein paar Actors, ändere eine Farbe, lösche etwas. Dann **Edit ›
> History…**: Da steht jeder Schritt mit Namen. Ein Klick springt genau zu diesem Stand.

Während Play und Simulate ist das Level gesperrt; Studio sagt dann „Stop playing to edit the level“.

---

## 11.7 Bilder, Töne und das Protokoll

Neben den Bauteilen hat der Content Drawer drei weitere Tabs.

**Textures** sind Bilder, die du auf Oberflächen legst, wie Tapete. **Import image** (oder ein Bild
einfach auf die Seite ziehen) holt eigene Fotos herein. Ist dabei etwas ausgewählt, bekommt es das Bild
gleich. Ein **Doppelklick** legt eine Textur auf die Auswahl. Die Bilder stecken danach im Level selbst,
die Level-Datei bleibt also vollständig.

**Sounds:** Ein **Klick** hört einen Ton an, ein **Doppelklick** gibt ihn der Auswahl als Klangquelle,
**Ziehen** in den Viewport setzt eine Klangquelle an diese Stelle. **Import audio** nimmt eigene
Tondateien.

> **Achtung, Falle!** Du hörst nichts? Browser erlauben Ton erst nach dem ersten Klick oder
> Tastendruck. Einmal ins Studio klicken, dann klingt es.

Das **Output Log** ist das Protokoll: alle Meldungen mit Uhrzeit, Warnungen und Fehler farbig. In Play
stehen hier auch Spielereignisse.

---

## 11.8 World Settings

Der Tab **World Settings** neben Details gilt nicht für einen Actor, sondern fürs **ganze Level**.
Er hat vier Gruppen:

- **Game:** die Spielregeln. Wann gewinnt man (alle Orbs gesammelt, Ziel erreicht, Zeit überlebt,
  Punktzahl geschafft)? Gibt es ein Zeitlimit? Und **Kill Z**: eine Höhe, unter die man nicht fallen
  darf. Wer tiefer fällt, fängt von vorn an.
- **Sky and light:** Tageszeit, wie schnell der Tag vergeht, Sonnenstand, Nebel, Wolken, Boden.
- **Physics:** die Schwerkraft und wie genau und wie oft die Physik rechnet ([Kapitel 6](06-physik.md)).
- **Audio:** Lautstärken, Hall und den Doppler-Effekt, den Martinshorn-Effekt: Ein vorbeifahrendes
  Martinshorn klingt erst höher, dann tiefer ([Kapitel 7](07-audio.md)).

> **Probier's aus!** Öffne Orb Run und zieh in World Settings am Namen von **Time of day** langsam
> hin und her. Die Stadt geht in die Nacht über, und die Fenster fangen an zu leuchten.

Alle Felder mit ihren Bereichen stehen im blauen Buch, Kapitel 11.

---

## 11.9 Play und Simulate

Studio kennt drei Zustände: **Bearbeiten**, **Simulate** (die Welt läuft, du schaust zu) und **Play**
(die Welt läuft, du spielst mit). Wichtig: Was in Simulate und Play passiert, verschwindet beim Stop
wieder. Dein Level bleibt unangetastet, außer du willst das Ergebnis ausdrücklich behalten.

> **Stell dir vor …** du baust ein Dominospiel auf dem Tisch auf. **Simulate** heißt: einmal anstoßen und
> zuschauen. **Play** heißt: selbst eine Figur sein, die durchläuft. Und nach dem Stop steht alles wieder
> genau so da wie vorher, wie von Zauberhand neu aufgebaut.

### Play

**Play** (Alt+P) startet das Spiel im aktiven Viewport. Du steuerst den Spieler, die Kamera folgt ihm,
und das HUD (die Anzeige über dem Bild) zeigt Punkte, Zeit und Tempo, beim Soldaten Fadenkreuz,
Gesundheit und Munition. Am Computer fängt Studio dabei die Maus; ist sie frei, klick einmal ins Bild.
Bei Sieg oder Niederlage erscheint **Play again**. **Esc** beendet Play.

![Play im Viewport: das Hovercar in Orb Run mit HUD](../buch/bilder/studio-play.png)

**F8** lässt den Spieler los (**Eject**, aussteigen): Das Spiel läuft weiter, du fliegst frei herum und
kannst Dinge greifen. Noch einmal **F8** übernimmt ihn wieder (**Possess**).

### Simulate

**Simulate** (Alt+S) lässt Physik und Regeln laufen, ohne dass du jemanden steuerst. Das Beste daran:
**Du kannst Dinge anfassen.** Linke Maustaste (oder Finger) auf einen beweglichen Körper greift ihn,
Ziehen bewegt ihn, und Loslassen **wirft** ihn mit Schwung.

> **Wusstest du?** Der Griff ist kein Trick, sondern echte Physik: Eine Art unsichtbares Gummiband
> zieht den gegriffenen Punkt zum Mauszeiger. Darum stößt der Körper überall an, kippt und schwingt wie
> jeder andere.

- **K** (Keep Simulation Changes) übernimmt die Lage der ausgewählten Actors ins Level und beendet die
  Simulation.
- **Pause**, dann **.**, geht Bild für Bild weiter, wie ein Daumenkino.
- Unter **Show** kannst du sichtbar machen, womit die Physik rechnet: ihre Formen, Gelenke und
  Berührungspunkte.

![Simulate mit eingeblendeten Kollisionsformen](../buch/bilder/studio-simulate.png)

> **Probier's aus!** Lade **File › Example Levels › Physics Lab** und drück **Simulate**. Greif die
> Seilbrücke, wirf eine Kiste gegen den Stapel, dreh am Scharnier. Dann stopp mit Esc, setz einen
> **Crate Stack** (Content Drawer › Physics), starte wieder **Simulate** und reiß ihn mit der Maus um.
> Wähle im Outliner die Kisten (die erste anklicken, die letzte mit Shift) und drück **K**: Der
> eingestürzte Stapel bleibt so liegen, als wäre er immer schon umgefallen.

![Physics Lab: alle Gelenktypen in der Simulation](../buch/bilder/studio-physics-lab.png)

### Zuschauen mit der Observer camera

Schaltest du in World Settings die **Observer camera** ein, startet Play nicht hinter dem Spieler,
sondern mit freier Kamera: Das Spiel läuft, du schaust zu und kannst Dinge greifen wie in Simulate.
**F8** übernimmt den Spieler, noch einmal F8 schaut wieder zu. Das Level Micro Duck startet so.

### Am Telefon und mit dem Gamepad

Kern merkt selbst, womit du gerade spielst (Finger, Tastatur oder Gamepad), und schaltet um. Am
Touchscreen liegen unten **zwei Sticks**: links bewegen, rechts schauen. Daneben erscheinen Knöpfe
passend zur Figur, beim Soldaten etwa JUMP, RELOAD und AIM. Oben links stehen **EXIT** und **PAUSE**.

![Play am Telefon im Hochformat: EXIT, PAUSE und die Touch-Sticks](../buch/bilder/phone-play-390.png)

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

Du öffnest den Logik-Editor über **Logic** in der Toolbar (Logik des Levels), über **Logic** in Details
oder per Rechtsklick › **Edit Logic** bzw. **Add Logic (Blueprint)** (Logik eines Actors). Er legt
sich über die Viewports; der Knopf **Viewport** bringt dich zurück.

Ein leerer Graph bietet Startknöpfe an, das sind die Ereignisse, die alles ins Rollen bringen:
**Begin Play** (beim Spielstart), **On Interact** (wenn der Spieler etwas benutzt), **On Begin Overlap**
(wenn jemand eine Zone betritt) und **Event Tick** (in jedem Augenblick). Unter **Templates…** gibt
es fertige Beispiele: eine Tür, die beim Benutzen aufgeht, eine Zone, die Punkte gibt, einen Spawner,
der regelmäßig Würfel erzeugt, eine Ampel und eine Micro Duck, die vor Hindernissen abdreht.

> **Probier's aus!** Neues Level **Basic**, dann **Logic** in der Toolbar und **Begin Play**. Am Knoten
> steht rechts der Ausgang „then“. Zieh von dort mit der Maus einen Draht ins Leere und lass los: Es
> öffnet sich eine Suche. Tippe „show message“ und drück Enter. Der neue Knoten hängt schon am Draht;
> trag in sein Textfeld einen Satz ein. Zurück mit **Viewport**. Das Basic-Level hat noch keine
> Spielfigur, und ohne Figur gibt es kein Play; setz deshalb einen **Rolling Ball** (Content Drawer ›
> Players), er wird dein Spieler. Jetzt **Play**: Dein Text steht im HUD. Dein erstes Programm, ganz
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

**Das Beispiel-Level** (File › Example Levels › Micro Duck) ist ein Zimmer in Entengröße mit Möbeln,
einem Ball und zwei Socken. Play startet mit der Observer camera: Die Ente läuft von selbst, erkundet
den Raum, weicht Hindernissen aus, spielt Ball und steht nach einem Sturz wieder auf. Hebst du sie am
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

> **Probier's aus!** Lade das Micro-Duck-Level, drück **Play** und schau ihr eine Weile zu. Dann greif
> den Ball mit der Maus, wie in Simulate, und leg ihn woanders hin. Zum Schluss **F8** und Taste **7**:
> Die Ente springt!

Jede Bewegung der Ente ist ein eigenes Netz. Das Laufen ist fest eingebaut, die übrigen Bewegungen
(Sitzen, Kicken, Springen …) holt Studio aus dem Internet nach, sobald ein Level eine Ente enthält.
Klappt das nicht, gibt Studio nicht auf: Es versucht es erneut und wartet dazwischen immer etwas
länger, damit es die Leitung nicht verstopft. Bis dahin bleibt die Ente bei dieser Bewegung stehen
und sagt, warum, statt still nichts zu tun.
Trainierst du mit dem Kern Lehrer selbst ein neues ([Kapitel 14](14-lehrer.md)), nimmt Studio
automatisch deins. Die Einzelheiten stehen in [Kapitel 13](13-micro-duck.md).

---

## 11.12 Speichern, Autosave und Einstellungen

Ein Level ist **eine einzige Datei** mit der Endung `.klevel`. Darin steckt wirklich alles: die
Actors, die Logik und sogar deine importierten Bilder und Töne. Du kannst sie also einfach
verschicken.

- **Ctrl+S** speichert. Je nach Browser fragt Studio nach dem Ort oder legt die Datei in deine
  Downloads.
- **Ctrl+O** öffnet eine Level-Datei.
- Oder du **ziehst** eine Datei auf die Seite: Levels öffnen sich, Bilder werden Texturen,
  Tondateien Sounds.

### Autosave

> **Stell dir vor …** ein Schutzengel, der dir über die Schulter schaut, aber nie nervt. Er schreibt
> sich heimlich auf, was du noch nicht gespeichert hast, und zerreißt den Zettel, sobald du speicherst.

Genau so arbeitet der Autosave: Kurz nach jeder Änderung legt Studio eine Kopie im Browser ab, aber nur,
solange etwas ungespeichert ist. Beim nächsten Start bietet es **Restore** (wiederherstellen) oder
**Dismiss** (verwerfen) an. Ein wiederhergestelltes Level gilt weiter als **ungespeichert**, denn
sein Inhalt steht ja noch in keiner Datei. Bis du es speicherst, bietet Studio es bei jedem Start
wieder an.

> **Achtung, Falle!** Der Autosave ist ein Rettungsring, kein Speicherort. Er hält nur **eine** Kopie im
> Browser. Wer den Browserspeicher leert, verliert sie. Wichtige Levels speicherst du mit Ctrl+S als
> Datei.

### Einstellungen

Unter **Edit › Preferences…** stellst du ein, wie schnell die Kamera fliegt, wie empfindlich die Maus ist
und wie fein das Einrasten. Studio merkt sich dabei nur, was **du selbst** geändert hast. Alles andere
folgt den Standardwerten der jeweiligen Version; bringt eine neue Version bessere Standards mit,
bekommst du sie also automatisch, ohne deine eigenen Wünsche zu verlieren. **Edit › Reset Settings to
Defaults…** setzt alles zurück; deine Levels bleiben dabei unberührt.

---

## 11.13 Spiel exportieren

Jetzt kommt der magische Moment: Aus deinem Level wird ein fertiges Spiel, das jeder ohne Studio
spielen kann.

1. **File › Package Game (HTML)...** oder der Paket-Knopf in der Toolbar.
2. **Game title** eintragen.
3. **Package** klicken. Studio lädt eine Datei `<Titel>.html` herunter und bietet **Play it** an.

Diese **eine Datei** ist ein komplettes Spiel: Die Engine und dein Level stecken darin. Sie braucht
keinen Server und kein Internet, läuft in jedem Browser, auch direkt von der Festplatte und am Telefon,
und startet sofort das Spiel statt des Editors. Die Steuerung ist dieselbe wie in Play; **P** pausiert,
**R** startet nach Sieg oder Niederlage neu. Online spielen kann die Datei auch
([Kapitel 10](10-netzwerk.md)).

> **Achtung, Falle!** Spiele mit der Micro Duck brauchen beim Weitergeben zwei Ordner mit den
> nachladbaren Bewegungen der Ente neben der HTML-Datei, sonst kann sie nur laufen. Und das Aussehen
> der Ente darf nur **nicht kommerziell** genutzt werden. Was du darfst und was du mitgeben musst,
> steht in [Kapitel 12](12-eigene-spiele.md).

> **Probier's aus!** Exportiere Orb Run und starte die Datei mit **Play it**. Dann schick sie dir selbst
> aufs Telefon, zum Beispiel als E-Mail-Anhang (manche Mailprogramme blockieren solche Anhänge, dann
> nimm einen Cloud-Speicher oder schick sie per Messenger als Datei), und öffne sie dort. Je nach Telefon öffnet sie sich
> direkt im Browser, sonst wählst du „Öffnen mit“ und deinen Browser. Die Touch-Sticks erscheinen von
> selbst.

---

## 11.14 Mehrspieler

Wie Kern Spieler übers Netz verbindet, erklärt [Kapitel 10](10-netzwerk.md). Hier nur die Bedienung.

### Mehrere Spieler auf einem Rechner

1. Play › Number of Players › **2 Players (listen server)** (bis zu 4).
2. **Play**. Studio schaltet auf vier Viewports: 1 zeigt den Server, 2 den ersten Gast. Jeder hat sein
   eigenes HUD.
3. Nur der Spieler im aktiven Viewport bekommt deine Eingabe. Ein Klick in einen anderen wechselt.
4. **Play › Network Emulation** macht die Leitung absichtlich schlechter, von „Perfect“ bis „Terrible“.

Was du dabei beobachten kannst, beschreibt die Mission in [Kapitel 10](10-netzwerk.md), Abschnitt 10.8.

![Zwei Spieler im Editor: Listen-Server und lokaler Client in eigenen Viewports](../buch/bilder/studio-multiplayer.png)

### Online zwischen Browsern

Alle öffnen **dasselbe Level**. Einer wählt **Play › Host Online Game...**, die anderen **Join Online
Game...**, alle denselben **Room** (Raumnamen, Standard „kern“). Ohne Relay finden sich nur Tabs
desselben Browsers; für verschiedene Rechner tragen alle die Adresse desselben Relays ein (Kapitel 10,
Abschnitt 10.11). **Leave Online Game** verlässt das Spiel.

---

## 11.15 Am Telefon und auf schmalen Bildschirmen

Auf kleinen Bildschirmen räumt Studio die Werkstatt um, damit die Welt Platz hat:

- **Der Viewport bekommt die ganze Fläche.** Outliner, Details und World Settings öffnest du über den
  Knopf **Panels**; sie klappen als Blatt auf, im Hochformat von unten.
- **Play und Simulate sind immer einen Tipp entfernt.** Sie stehen in einem **Dock** am rechten Rand
  der Toolbar, das stehen bleibt, während der Rest der Leiste seitwärts scrollt.
- Weniger Wichtiges wird ausgeblendet.
- **Nichts bleibt gedrückt hängen.** Hebt sich der letzte Finger vom Bildschirm oder wechselt die
  Seite in den Hintergrund, lässt Kern alle Eingaben los. Sonst liefe deine Figur weiter, als hielte
  noch jemand den Stick.

![Studio in einem 820 px breiten Fenster](../buch/bilder/studio-narrow.png)

![Telefon im Hochformat: das Dock mit Play, Simulate und Panels](../buch/bilder/phone-dock-390.png)

> **Probier's aus!** Öffne Studio auf deinem Telefon. Tipp im Dock auf **Play**, dann **EXIT**. Öffne
> **Panels**, wähle im Outliner einen Actor und ändere in **Details** seine Farbe.

---

## 11.16 Warum Studio so gebaut ist

### Ein Programm für Editor und Spiel

Das ganze Studio, also Auswählen, Gizmos, Undo, Play, Speichern, ist in **C++** geschrieben, der
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
> Fliegen, Werkzeuge, Undo, Play, Simulate, Logik, Micro Duck, Mehrspieler, Export und die
> Telefon-Ansicht. Die meisten Bilder in diesem Kapitel stammen aus genau diesem Test.

Die genauen Befehle und Schnittstellen stehen im blauen Buch, Kapitel 11, und in
[Kapitel 17](17-referenz.md).

---

## 11.17 Die wichtigsten Tasten

**F1** zeigt die Kürzel jederzeit im Studio. Für den Anfang reichen diese:

| Taste | Wirkung |
|---|---|
| Q / W / E / R | Auswählen / Verschieben / Drehen / Skalieren |
| Ctrl+Z / Ctrl+Y | rückgängig / wiederholen |
| Ctrl+C / Ctrl+V, Ctrl+D | kopieren / einfügen, verdoppeln |
| Entf | löschen |
| F | Auswahl ins Bild holen |
| Ctrl+S / Ctrl+O | speichern / öffnen |
| Ctrl+Space | Content Drawer auf und zu |
| Alt+P / Alt+S | Play / Simulate |
| Esc | Play beenden, Ziehen abbrechen |
| F8 | Spieler loslassen bzw. übernehmen |

> **Wusstest du?** Kopierte Actors landen als Text in der Zwischenablage deines Computers. Ein Ctrl+V
> in einem **anderen Tab** fügt sie dort ein. So trägst du Bauteile von einem Level ins nächste.

Weitere Tasten stehen in [Kapitel 17](17-referenz.md), die des Logik-Editors in
[Kapitel 9](09-logik.md). Die vollständige Liste steht im blauen Buch, Kapitel 11.

---

## Das nimmst du mit

- Ein Level besteht aus **Actors**, und was ein Actor kann, bestimmen seine **Komponenten**, wie Steine
  auf einer Lego-Platte.
- **Viewports** zeigen die Welt als Foto (perspektivisch) oder als Bauplan (orthografisch).
- Auswählen mit Klick oder Rahmen, bewegen **nur mit dem Gizmo**; Einrasten hält alles ordentlich.
- **Undo** merkt sich nach jedem Schritt den ganzen Stand, darum geht Rückgängig nie schief.
- **Simulate** heißt zuschauen und anfassen, **Play** heißt mitspielen, und nach dem Stop ist alles wie
  vorher (außer du behältst es mit **K**).
- Der **Logik-Editor** gibt dem Level Regeln, ganz ohne Code.
- **Package Game** macht aus deinem Level **eine HTML-Datei**, die überall läuft.
- Editor und Spiel sind dasselbe C++-Programm; jede Handlung ist ein Befehl.

## Knobelecke

1. Du ziehst mit der linken Maustaste über einen Würfel. Warum bewegt er sich nicht?
2. Du hast einen Kistenstapel einstürzen lassen und willst ihn genau so behalten. Wie?
3. Eine Kiste soll vom Tisch fallen, bleibt aber in der Luft hängen. Was fehlt ihr?
4. Warum kann Undo bei Studio nie „danebengehen“?
5. Was ist der Vorteil, dass Editor und exportiertes Spiel dasselbe Programm sind?

### Lösungen

1. Linkes Ziehen zieht immer einen Auswahlrahmen auf. Bewegt wird nur mit dem Gizmo (W).
2. In **Simulate** einstürzen lassen, die Kisten auswählen, **K** drücken.
3. Die Physik-Komponente (Rigid Body, dynamisch). Ohne sie bleibt ein Actor, wo er steht.
4. Studio rechnet keine Schritte rückwärts, sondern springt zu einem gemerkten Gesamtstand zurück.
5. Was du im Editor siehst, verhält sich genau so im fertigen Spiel, und der Editor lässt sich
   automatisch testen.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
