# 8 Die Spiel-Laufzeit – vom Bauplan zum Spiel

> **Dein Level:** Hier wird aus Kern eine echte Game Engine. Du erfährst, woraus ein Level besteht,
> wie es gespeichert wird und was in jedem einzelnen Bild passiert, während du spielst. Danach weißt
> du, wie Fahrzeuge fahren, Trigger auslösen und Spiele gewonnen werden.

> **Wörter, die du brauchst:**
> - **Actor** – ein Ding im Level: eine Kiste, eine Lampe, ein Auto, die Spielfigur.
> - **Komponente** – ein Baustein, der einem Actor eine Fähigkeit gibt, etwa „sichtbar sein“.
> - **Collider** – die unsichtbare Form, mit der die Physik prüft, ob Dinge zusammenstoßen.
> - **Sensor** – ein Collider, der nichts aufhält, sondern nur meldet, wenn etwas hineinkommt.
> - **statisch / kinematisch / dynamisch** – steht fest / wird vom Spiel bewegt, lässt sich aber
>   nicht schubsen / echte Physik mit Fallen und Stoßen.
> - **fester Schritt** – die Physik rechnet in immer gleich langen Zeitstücken, meist 60 pro Sekunde.
> - **Server** – im Mehrspieler der eine Computer, der entscheidet, was wirklich passiert.
> - **Netz** – ein Programm, das durch langes Üben gelernt hat, statt Regel für Regel programmiert
>   zu sein.

Ein **Level** ist ein Dokument aus Actors, die Komponenten tragen. Kern Studio bearbeitet Levels und
speichert sie als Datei mit der Endung `.klevel`. Die **GameWorld** spielt ein Level ab: Sie zeigt es
an und lässt es mit der Physik-Engine Box3D laufen, mit Steuerung, Kamera, Regeln, Triggern, Ton und
Logik. Studio benutzt dieselbe GameWorld zum Bearbeiten, für Simulate und für Play. Exportierte Spiele
laufen mit genau demselben Programm.

> **So kannst du es dir vorstellen:** Das Level ist das Rezept, die GameWorld ist die Küche. Das
> Rezept liegt ordentlich im Kochbuch. Gekocht wird mit einer Kopie, damit das Buch sauber bleibt.

## 8.1 Das ganze Leben eines Spiels

Ein Spiel durchläuft immer dieselben vier Stationen: vorbereiten, starten, Schritt für Schritt
weiterrechnen, stoppen.

```mermaid
flowchart LR
  A[Vorbereiten] --> B[Start: Kopie anlegen]
  B --> C[Schritt]
  C --> C
  C --> D[Stopp]
  D --> A
```

1. **Vorbereiten:** Die GameWorld bringt die 3D-Modelle auf den Stand des Levels und schickt sie an
   die Grafikkarte. Hast du nur eine Kiste verschoben, wird auch nur sie neu geschickt.
2. **Start:** Die GameWorld legt eine **Kopie** des Levels an, baut daraus die Physikwelt und startet
   Simulate oder Play.
3. **Schritt:** In jedem Bild rechnet sie weiter, bewegt die Kamera und zeichnet (8.9).
4. **Stopp:** Alles wird abgebaut, und du siehst wieder dein unverändertes Level.

Was während des Spiels passiert, landet also **nicht** in deinem Level. Umgekippte Kisten stehen nach
Stopp wieder ordentlich da. Wer eine simulierte Lage behalten will, nimmt in Studio **Keep Simulation
Changes** (Taste K, 8.8).

Die genauen Namen der Programmteile stehen im blauen Buch, Kapitel 8.

## 8.2 Die Spielregeln für Zahlen

Damit alle Teile von Kern dasselbe meinen, gelten feste Regeln:

- **Einheiten:** Meter, Sekunden, Kilogramm.
- **Z zeigt nach oben.** X und Y liegen flach auf dem Boden.
- **Drehungen** stehen als drei Winkel in Grad.

> **So kannst du es dir vorstellen:** ein Flugzeug. **Rollen** heißt, es kippt um seine Längsachse
> und eine Tragfläche geht nach unten (Drehung um x). **Nicken** heißt, die Nase geht hoch oder
> runter (um y). **Gieren** heißt, die Nase schwenkt nach links oder rechts wie beim Lenken (um z).

- **Eltern und Kinder:** Ein Actor kann einem anderen untergeordnet sein. Seine Position gilt dann
  **relativ zu seinem Eltern-Actor**. Ein Rad als Kind eines Autos steht zum Beispiel „1,2 m vor der
  Automitte“, egal wo das Auto gerade ist. Fährt das Auto, fährt das Rad mit.
- **Angeschweißt:** Hat ein Kind keinen eigenen Physik-Körper, wird es beim Simulieren an den Körper
  seines Eltern-Actors „angeschweißt“. Beim Hover Car in Orb Run sind Kabine, Spoiler und Düsen solche
  Kinder: Für die Physik ist das ganze Fahrzeug **ein** Körper aus mehreren Formen.

## 8.3 Level und Actor

Ein **Level** enthält:

- einen Namen und die **World Settings** (8.5), also Einstellungen für die ganze Welt,
- alle Actors,
- importierte Bilder und Sounds (sie stecken direkt in der Datei),
- einen Logik-Graphen für das ganze Level ([Kapitel 9](09-logik.md)),
- die Position der Editor-Kamera.

Der **Spieler** ist der Actor, der in den World Settings als Player eingetragen ist. Fehlt der
Eintrag, nimmt Kern den ersten Actor mit Steuerung.

Ein **Actor** hat:

- eine eindeutige **Nummer** und einen **Namen**. Den Namen siehst du im **Outliner**, der Liste
  aller Actors in Studio,
- einen **Eltern-Actor** (oder keinen) und optional einen Ordner im Outliner,
- **Hidden** (unsichtbar) und **Locked** (im Viewport nicht anklickbar),
- eine **Transform**: Position, Drehung und Größe,
- **Tags**, freie Schlagworte, nach denen die Logik suchen kann,
- **Komponenten**: von den meisten Arten höchstens eine, von Collidern und Gelenken beliebig viele,
- einen eigenen **Logik-Graphen**.

## 8.4 Die Komponenten

Komponenten sind wie Lego-Aufsätze: Ein nackter Actor ist nur ein Punkt im Raum. Steckst du ein Mesh
drauf, wird er sichtbar. Steckst du einen Rigid Body dazu, fällt er herunter. So baust du alles aus
denselben Bausteinen zusammen.

| Komponente | Was sie kann | Beispiel |
|---|---|---|
| Static Mesh | sichtbare Form mit Material | Kiste, Kugel, Rampe |
| Point Light | Lampe mit Farbe und Reichweite | Straßenlaterne |
| Rigid Body | macht den Actor zum Physik-Körper | Kiste, die umfällt |
| Collider | Form für Zusammenstöße | Kopf eines Soldaten |
| Constraint | Gelenk zu einem anderen Actor | Tür im Scharnier |
| Player Controller | macht den Actor steuerbar | Hover Car, Kugel |
| Weapon, Health | Waffe und Lebenspunkte | Schießstand, Fass |
| Pickup | etwas zum Einsammeln | Orb, Stern |
| Trigger | unsichtbarer Schalter | Ziel, Sprungfeld |
| Audio Source | Schallquelle ([Kapitel 7](07-audio.md)) | Radio |
| Rotator | dreht sich ständig | Drehtür, Stern |
| Mover | pendelt hin und her | Aufzug, Plattform |
| City | baut eine ganze Stadt | Orb Run |

Ein paar Bausteine im Einzelnen:

**Static Mesh** ist die sichtbare Form: Box, Kugel, Zylinder, Kegel, Kapsel, Ebene, Ring, Rampe oder
ein eingebautes Modell, etwa ein Servomotor der Micro Duck. Das **Material** bestimmt das Aussehen:
Farbe, wie rau oder glänzend, ob metallisch, ob es selbst leuchtet, ob man durchsehen kann. Ein Mesh
kann automatisch einen passenden Collider mitbringen. Dessen Reibung, Sprungkraft und Oberfläche
([Kapitel 6](06-physik.md)) stellst du gleich hier ein, unter Mesh › **Physics**. Sie gehören
neben das Aussehen, denn sie sind die Physik des Dings.

**Rigid Body** macht den Actor zum Physik-Körper. In Studio wählst du **Static** (bewegt sich nie),
**Kinematic** (wird bewegt, lässt sich aber nicht schubsen) oder **Dynamic** (echte Physik). Begegnet
dir in [Kapitel 6](06-physik.md) der Name „fixed“: Das ist derselbe Typ, nur sein Name im Programm.
Mit dem Gelenk **Fixed (weld)** hat er nichts zu tun. Dazu kommen Masse,
Schwerkraft-Faktor, „Bullet“ für sehr schnelle Körper und Startgeschwindigkeit.

**Collider** geben die Form für Zusammenstöße an: automatisch ans Mesh angepasst, Box, Kugel, Kapsel
oder eine eng anliegende Hülle. Jeder Collider hat eine **Dichte**, aus der Kern die Masse
ausrechnet. Im Level ist sie anfangs 1000 kg/m³, so dicht wie Wasser. (Wer in [Kapitel 6](06-physik.md)
direkt mit der Physik arbeitet, ohne Level, startet dort mit 1 kg/m³. Jeder Collider aus einem Level
bringt seine 1000 mit.) Ein Würfel mit 1 m Kantenlänge wiegt also eine Tonne, wenn du nichts änderst.
Ein Collider kann ein **Sensor** sein. Mit Kanälen und Gruppen legst du fest, wer mit wem überhaupt
zusammenstoßen kann.

**Constraint** ist ein Gelenk: Scharnier, Schiene, Kugelgelenk, verschweißt, Seil, Rad und einige
mehr. Gelenke können Grenzen, einen Motor und eine Feder haben und bei zu großer Kraft **brechen**.
Ein Scharnier kann ein **Servo** sein, wie die Motoren echter Roboter: Je weiter das Gelenk von
seinem Zielwinkel weg ist, desto stärker drückt es zurück, und je schneller es sich dreht, desto
stärker bremst es. Doppelt so weit weg heißt doppelt so starke Drehkraft, wie bei einem Gummiband.

**Rotator** und **Mover** bewegen Dinge ohne Logik: Der Rotator dreht gleichmäßig, der Mover pendelt
weich, hin und zurück oder springt am Ende zurück an den Anfang. Beide rechnen mit der Spielzeit, also
laufen sie bei jedem Start gleich.

**City** baut aus einer einzigen Zahl (dem Seed) eine ganze Stadt mit Straßen, Häusern, Bäumen,
Autos und nachts leuchtenden Fenstern ([Kapitel 5](05-szene-und-geometrie.md)).

Die übrigen Bausteine (Controller, Waffe, Pickup, Trigger) kommen in 8.10 bis 8.13 dran.

> **Für Profis:** Alle Felder jeder Komponente mit ihren Standardwerten stehen im blauen Buch,
> Kapitel 8. Ein Beispiel daraus: Ein Point Light hat anfangs die Helligkeit 20 und reicht 12 m weit.

> **Mission:** Öffne auf https://kernengine.de/ über File › Example Levels das Level **Physics Lab**.
> Klick die Tür an und schau in Details: Welche Komponenten hat sie? Finde beim Constraint den Typ,
> die Grenzen und die Feder. Starte danach **Simulate** (Alt+S) und schalte unter Window › Show
> (oder mit dem Knopf **Show** oben im Viewport) **Collision Shapes** und **Constraints** ein. Die
> Farben verraten den Typ: statisch grün, kinematisch blau, dynamisch orange, Sensor violett,
> schlafend grau.

## 8.5 Die World Settings

Manche Einstellungen gelten nicht für ein Ding, sondern für die ganze Welt. Die stehen in den
**World Settings**:

| Bereich | Beispiele |
|---|---|
| Physik | Stärke der Schwerkraft, wie viele feste Schritte pro Sekunde |
| Himmel | Tageszeit (0,25 Aufgang, 0,5 Mittag, 0,75 Untergang), Tageslänge |
| Bild | Nebel, Helligkeit, Wolken |
| Boden | endloser Boden an oder aus, Farbe, Reibung |
| Spiel | Titel, Siegbedingung, Zeitlimit, Spieler, Kill-Z |
| Audio | Lautstärken, Musik, Hall, Doppler ([Kapitel 7](07-audio.md)) |

Die Tageszeit ist ein Bruchteil des Tages: 0,5 ist die Hälfte, also Mittag. Zwei Besonderheiten:
**Kill-Z** ist eine Höhe. Fällt ein Spieler tiefer, etwa von der Welt herunter, wird er zurückgesetzt.
Und mit **Observer camera** startet Play mit einer freien Kamera; mit F8 übernimmst du dann den
Spieler.

## 8.6 Das `.klevel`-Format

Ein Level wird als **JSON** gespeichert. Das ist ganz normaler, lesbarer Text mit Klammern, Namen und
Werten. Du kannst eine `.klevel`-Datei mit jedem Texteditor öffnen. Ein Actor sieht so aus
(gekürzt):

```json
{
  "id": 12, "name": "Door", "parent": 3,
  "transform": {"position": [5.2, -12, 1.5]},
  "tags": ["door"],
  "components": {"mesh": {}, "body": {}}
}
```

Zeile für Zeile: Der Actor hat die Nummer 12, heißt „Door“ und ist ein Kind von Actor 3. Er steht bei
x = 5,2 m, y = −12 m, z = 1,5 m (relativ zu seinem Eltern-Actor). Er trägt das Schlagwort „door“ und
hat ein Mesh und einen Rigid Body. Was in den geschweiften Klammern fehlt, bekommt Standardwerte.
Kern schreibt Standardwerte meist gar nicht erst hin, das hält die Datei klein.

**Beim Lesen** gilt ein schlauer Grundsatz: **nachsichtig bei Fehlendem, streng bei Falschem.**

- **Fehlt** etwas, nimmt Kern den Standardwert. Unbekannte Einträge überspringt es. So laden alte
  Dateien auch in neuen Versionen von Kern.
- **Falsch** ist zum Beispiel Text, wo eine Zahl hingehört. Dann meldet Kern einen Fehler, etwa
  „friction must be a number“, und nennt den Actor. Eine Datei aus einer neueren Kern-Version lehnt
  es ab.
- Zahlen außerhalb ihres sinnvollen Bereichs werden auf den Rand gesetzt, eine Reibung von 50 also auf
  10.
- Fehlende oder doppelte Nummern werden neu vergeben. Ein Eltern-Actor, den es nicht gibt, wird zu
  „keiner“, und **Kreise** in der Familie werden aufgebrochen.

> **Vorsicht, Falle:** Ein Kreis heißt: A ist Elternteil von B, und B ist Elternteil von A. So etwas
> entsteht leicht, wenn man Dateien von Hand bearbeitet. Ohne Prüfung würde das Programm endlos „nach
> oben“ suchen und hängen bleiben.

**Prüfen vor dem Start:** Bevor Play beginnt, sucht Kern nach Problemen. **Fehler** verhindern Play,
zum Beispiel eine Größe von 0, ein Gelenk, das einen Actor mit sich selbst verbindet, oder eine
Steuerung ohne Physik-Körper. **Warnungen** landen nur im Output Log, zum Beispiel ein Sound-Name, den
es nicht gibt (dann bleibt es still), oder mehr als ein Spieler. Die vollständigen Listen und
Wertebereiche stehen im blauen Buch, Kapitel 8.

## 8.7 Vom Level zur Physikwelt

Bevor simuliert wird, macht Kern einen **Plan**: Welche Physik-Körper braucht dieses Level, und welche
Formen gehören zu welchem Körper? Das Wichtige daran: **Derselbe Plan** zeichnet auch die Collider im
Editor. Was du im Editor mit Show › Collision Shapes siehst, ist also genau das, womit später
gerechnet wird.

```mermaid
flowchart LR
  L[Level] --> P[Plan: Körper und Formen]
  P --> W[Physikwelt]
  P --> E[Collider im Editor]
```

Die Regeln des Plans:

1. Jeder Actor mit Rigid Body bekommt einen Körper. Ein statischer Körper mit Rotator oder Mover wird
   kinematisch, er muss sich ja bewegen.
2. Manche Dinge brauchen einen Körper, obwohl du keinen Rigid Body gesetzt hast: eine Wand mit
   Collider, ein Stern zum Einsammeln, ein Trigger. Sie bekommen einen **unsichtbaren eigenen
   Körper**, statisch oder, wenn sie sich bewegen, kinematisch.
3. Jeder Actor gibt seine Formen an seinen Körper, auch angeschweißte Kinder (8.2). Ein automatischer
   Collider wird ans Mesh angepasst.
4. Pickups und Trigger werden zu Sensoren. Haben sie gar keine Form, bekommen sie eine Sensor-Kugel
   mit 1 m Radius.
5. Die Stadt wird ein einziger statischer Körper mit einer Box je Haus.
6. Der endlose Boden ist eine riesige flache Box, deren Oberseite genau bei z = 0 liegt.

## 8.8 Die GameWorld starten

Drückst du Play, geht die GameWorld diese Schritte durch:

1. **Prüfen** (8.6). Ein Fehler bricht ab.
2. **Kopie anlegen.** Gespielt wird mit der Kopie, das Original bleibt unberührt.
3. **Physikwelt bauen:** Schwerkraft setzen, dann alle Körper, Gelenke und Laufroboter nach dem Plan
   aus 8.7 anlegen, Eltern vor ihren Kindern. Die Autos der Stadt fahren los.
4. **Spieler übernehmen** (nur bei Play). Er bekommt einen **Pawn**: ein Notizblatt, auf dem Kern
   sich alles über die Steuerung dieses Spielers merkt, also letzte Eingabe, Blickrichtung,
   Startpunkt und Boost.
5. **Regeln und Logik starten:** Punkte, Zeit, Gesundheit und Munition auf Anfang, die Bestzeit
   bleibt. Dann laufen die Logik-Graphen los.

Spieler-Fahrzeuge bekommen dabei immer die Sonderbehandlung „Bullet“, damit sie bei hohem Tempo nicht
durch Wände rutschen. Fahrzeuge, Figuren und Flieger werden gegen Umkippen gesichert, nur die
Rollkugel und die laufende Ente dürfen kippen. Was genau beim Anlegen von Körpern und Gelenken
passiert, steht im blauen Buch, Kapitel 8.

**Die Modi:**

| Modus | Bedeutung |
|---|---|
| none | keine Simulation, nur Editor |
| simulate | Physik und Logik laufen, niemand wird gesteuert, Körper lassen sich greifen |
| play | wie Simulate, dazu Spieler, Regeln und HUD |
| replica | Kopie eines Server-Spiels bei einem Mitspieler ([Kapitel 10](10-netzwerk.md)) |

Zwischen Simulate und Play kannst du mitten im Lauf wechseln (Studio: F8), der Spieler behält dabei
seinen Zustand. Du kannst neu starten (Ctrl+R), pausieren und genau einen Schritt weitergehen (Taste
`.`).

> **Mission:** Starte im Physics Lab **Simulate**, drücke **Pause** und dann mehrmals die Taste
> **.** – die Welt läuft Schritt für Schritt. Greif zwischendurch eine Kiste mit der linken
> Maustaste. Wähle dann im Outliner eine umgefallene Kiste aus und drück **K** (Play › **Keep Simulation
> Changes**). Nach Esc steht die Kiste in deinem Level genau so, wie sie lag.

## 8.9 Was in einem Bild passiert

Die Bildrate deines Computers schwankt, die Physik will aber **feste Schritte** (warum, erklärt
[Kapitel 6](06-physik.md)). Darum sammelt die GameWorld die vergangene Zeit wie in einer Spardose und
zahlt sie in festen Schritten aus. Hängt ein Bild sehr lange, rechnet sie höchstens 0,1 s nach,
damit kein Rechenstau entsteht.

> **So kannst du es dir vorstellen:** Ein Bus fährt genau alle 10 Minuten, egal wann die Fahrgäste
> kommen. Kommen viele auf einmal, fahren eben zwei Busse hintereinander. Die Physik ist der Bus,
> die Bilder sind die Fahrgäste.

Ein fester Schritt:

```mermaid
flowchart TD
  A[Spieler-Eingaben] --> B[Rotator und Mover]
  B --> C[Verkehr, Laufroboter, Servos, Griff]
  C --> D[Physik rechnen]
  D --> E[Ereignisse]
  E --> F[Gesundheit und Logik]
  F --> G[Zeitlimit, Kill-Z, Sieg]
```

1. **Spieler:** Für jeden Spieler die Eingabe holen und sein Fahrzeug oder seine Figur bewegen.
2. **Animation:** Rotator und Mover bekommen ihr Ziel für diesen Schritt.
3. **Verkehr** (Autos halten vor dem Spieler), Laufroboter, Servos und der **Maus-Griff**.
4. **Box3D rechnet** einen Physikschritt.
5. **Ereignisse:** Was wurde eingesammelt, welcher Trigger berührt, wer getroffen, welches Gelenk
   gebrochen? Daraus entstehen auch die Aufprallgeräusche.
6. **Gesundheit** (Heilen, Wiederkehr) und die **Logik** ([Kapitel 9](09-logik.md)).
7. **Regeln:** Zeitlimit, Kill-Z, Punktestand, gewonnen oder verloren?

Danach wird gezeichnet. Bildschirme zeigen oft mehr Bilder pro Sekunde, als die Physik Schritte macht.
Darum zeichnet Kern nicht einfach den letzten Schritt, sondern einen **Zwischenstand** zwischen den
letzten beiden Schritten. Das heißt **interpolieren**. Liegt ein Bild genau in der Mitte, steht die
Kiste genau in der Mitte zwischen ihren zwei letzten Positionen. So bleibt die Bewegung auch bei 144
Bildern pro Sekunde flüssig. Zuletzt bewegt sich die Kamera, eingesammelte Pickups verschwinden, und
nachts leuchten die Fenster.

**Im Studio** kommt pro Bild noch die Arbeit des Editors dazu, etwa Haltepunkte der Logik prüfen und
Meldungen ins Output Log schreiben.

## 8.10 Controller: fahren, rollen, laufen, fliegen

Ein **Controller** macht aus Tasten Bewegung. Pro Bild bekommt jeder Spieler eine **Eingabe**: vor
und zurück, links und rechts (je von −1 bis 1), hoch und runter, Springen, Boost, Kameradrehung, beim
Soldaten Feuern, Zielen, Nachladen, Ducken und Benutzen. Bei der Micro Duck kommen ihre Bewegungen
als **Bits** dazu (Sitzen, Aufheben, Kicks, Rolle, Schnabel, Zwitschern, Wheee, Sprung).

> **Krass, aber wahr:** Neun Entenbewegungen passen in eine einzige Zahl. Jede Bewegung ist ein Bit
> mit dem Wert 1, 2, 4, 8, 16 … Willst du gleichzeitig aufheben (2) und zwitschern (64), schickst du
> einfach 66. Weil jeder Wert nur einmal vorkommt, lässt sich 66 nur als 64 + 2 zerlegen.

Bei Rolling Ball, Character und Flyer ist die Bewegung **kamerarelativ**: „Vor“ heißt dorthin, wohin
die Kamera blickt.

| Modus | So bewegt er sich |
|---|---|
| Hover Car | schwebt auf einer unsichtbaren Feder über dem Boden, Gas schiebt, eine Seitenkraft verhindert Rutschen |
| Vehicle | wie das Hover Car, aber auf dem Boden; Gas wirkt nur mit Bodenkontakt |
| Rolling Ball | eine Kugel, die du anschubst; springen nur vom Boden |
| Character | eine Spielfigur, die läuft, springt und Stufen hochsteigt |
| Flyer | fliegt in jede Richtung, auch hoch und runter, die Schwerkraft wird aufgehoben |
| Soldier | wie Character, für Ego-Shooter (8.13) |
| Walker | die Micro Duck läuft mit ihren eigenen Motoren (8.14) |

Character und Soldier benutzen den **Character Mover** aus [Kapitel 6](06-physik.md). Der hat dort
von sich aus eine eigene Schwerkraft von 20 m/s². Im Spiel stellt Kern sie neu ein: Beim Character
auf das 1,5-Fache der Weltschwerkraft (also etwa 14,7 m/s²), damit Sprünge knackiger wirken, beim
Soldaten auf die echte Schwerkraft, wie in Ego-Shootern üblich.

Für alle gilt: **Boost** ist ein Vorrat von 0 bis 1. Beim Boosten sinkt er um 0,45 pro Sekunde, sonst
wächst er um 0,12 pro Sekunde. Nach einem tiefen Fall spielt eine Landung den Sound `land`.

Rechne mal: Ein voller Boost-Vorrat hält 1 / 0,45 ≈ 2,2 Sekunden. Ganz neu füllen dauert
1 / 0,12 ≈ 8,3 Sekunden.

**Mehrere Spieler:** Jeder Spieler hat seinen eigenen **Pawn** (8.8). Kommt im Mehrspieler jemand
dazu, kopiert Kern die Spielfigur samt Kindern und stellt die Kopie auf eine Spirale um den
Startpunkt, jeden neuen Spieler ein Stück weiter außen. So steht keiner auf dem anderen.

## 8.11 Kamera

Die Kamera folgt der **interpolierten** Lage des Spielers (8.9), also immer dem flüssigen
Zwischenstand:

- **Chase bei Fahrzeugen:** hinter dem Fahrzeug, umso weiter weg, je schneller es fährt.
- **Chase sonst:** kreist mit Maus oder Stick um den Spieler. Ein Prüfstrahl hält sie aus Wänden
  heraus.
- **First Person:** die Augen am Körper.
- **Top Down:** von oben.

Über 30 m/s wird das Sichtfeld etwas weiter, das fühlt sich schneller an. Harte Treffer schütteln die
Kamera. Soldaten haben eine eigene Kamera.

## 8.12 HUD, Regeln und Trigger

Das **HUD** ist die Anzeige über dem Spiel: Status, Titel, Meldung, Punkte, Pickups, Zeit, Restzeit,
Tempo, Boost, Bestzeit und beim Soldaten Gesundheit und Munition.

![Play im Studio mit HUD: Orb Run direkt nach dem Start, mit Zähler für die Orbs, Punkten, Zeit und Tempo](../buch/bilder/studio-play.png)

**Siegbedingungen** (in Studio: World Settings › Win condition):

| Ziel | Gewonnen, wenn … |
|---|---|
| none | nie – Sandkasten |
| collectAll | alle Pickups gesammelt (gibt es ein Goal, danach noch dorthin) |
| reachGoal | Goal-Trigger erreicht |
| survive | Zeitlimit überstanden |
| score | Zielpunkte erreicht |

Mit Zeitlimit verliert man bei 0, außer bei „survive“. Ein Hazard verliert immer. Am Ende setzt Kern
den Status, spielt eine kurze Melodie, merkt sich die Bestzeit und meldet der Logik „Spiel vorbei“.

**Pickups:** Berührt ein Spieler eines, bekommt er Punkte, Heilung oder Munition, dazu Sound und
Funken, und die Logik erfährt davon. Ein reines Heilpaket bleibt liegen, solange du volle Gesundheit
hast. Mit Wiederkehr-Zeit taucht es später wieder auf.

**Trigger-Aktionen:** Ein Trigger ist ein unsichtbarer Schalter. Was er tut, legt seine Aktion fest:

| Aktion | Wirkung | Fertig im Katalog |
|---|---|---|
| goal | gewinnt | Goal |
| hazard | verliert | Hazard |
| checkpoint | neuer Startpunkt 1 m darüber | Checkpoint |
| message | zeigt eine Meldung | Message Trigger |
| launch | schleudert hoch und nach vorn | Jump Pad |
| teleport | versetzt zum Ziel-Actor | Teleporter |
| damage | zieht Lebenspunkte ab | Damage Zone |
| event | nur ein Ereignis für die Logik | – |

Jede Aktion spielt ihren passenden Sound. Der Teleporter sperrt sich 1 s lang, damit du nicht sofort
zurückgebeamt wirst. Jeder Trigger meldet sich vorher bei der Logik. Mit „nur einmal“ ist er danach
verbraucht, außer Goal und Hazard.

> **Mission:** Lege mit File › New Level... ein **Basic**-Level an. Platziere aus Place Actors einen
> **Rolling Ball** (Players), drei **Stars** und ein **Goal** (Gameplay). Stell in World Settings die
> **Win condition** auf „Collect all pickups“ und drück **Play** (Alt+P). Was passiert, wenn du das
> Goal vor dem letzten Stern berührst? Und was danach?

> **Mission:** Baue in dein Level ein **Jump Pad** und einen **Teleporter** (beide unter Gameplay)
> und probiere sie mit dem Rolling Ball aus. Der Teleporter besteht aus zwei Feldern: Du betrittst A
> und kommst bei B an.

## 8.13 Soldaten und Figuren

Der **Soldat** ist das Spielmodell für Ego-Shooter, übernommen aus Kerns Vorgänger-Spiel SpawnCity.
Er läuft mit dem Character Mover, duckt sich, sprintet und springt. Ein Sprungdruck kurz vor der
Landung oder kurz nach einer Kante zählt noch, das fühlt sich fair an.

Ein Schuss ist ein gerader **Prüfstrahl** aus dem Auge. Die Kugel fliegt nicht wirklich, Kern prüft
nur, was auf der Linie zuerst getroffen wird. Die Streuung ist **deterministisch**: Sie wird aus der
Nummer des Schützen und der Zahl seiner Schüsse berechnet, statt gewürfelt. So rechnen der Server und
jede Wiederholung genau dieselben Treffer aus.

**Lag-Kompensation:** Im Mehrspieler kommt dein Schuss mit etwas Verspätung beim Server an. In der
Zwischenzeit sind die anderen weitergelaufen. Der Server schaut deshalb nach, **wo der Gegner war, als
du abgedrückt hast**, und prüft den Treffer dort, höchstens eine halbe Sekunde zurück.

> **So kannst du es dir vorstellen:** ein Schiedsrichter mit Videobeweis. Er spult zu dem Moment
> zurück, in dem du geschossen hast, und entscheidet dann.

Dazu kommen Rüstung (fängt einen Teil des Schadens ab), kurzer Schutz nach dem Wiedereinstieg,
Fallschaden bei harten Landungen und **Wiederkehr**: Ein Spieler startet nach einer Wartezeit neu,
andere Actors verschwinden und tauchen, wenn eingestellt, an ihrem Platz wieder auf. Ein Fass mit
Explosionsradius explodiert und schubst, was in der Nähe ist. Figur und Waffe sind aus Grundformen
gebaut und reine Darstellung: Sie ändern nichts an der Simulation. Die genauen Werte stehen im blauen
Buch, Kapitel 8.

## 8.14 Walker

Ein **Walker** ist ein Roboter mit Gelenken, der mit seinen eigenen Servos läuft. In Kern ist das die
Micro Duck. Gesteuert wird sie nicht von Regeln, die jemand programmiert hat, sondern von einem
**Netz**, das Pollen Robotics trainiert hat: Es hat durch sehr viel Üben gelernt, welche
Motorbewegung zu welcher Lage passt.

In jedem Schritt läuft ein Kreislauf:

```mermaid
flowchart LR
  S[Fühlen: Sensoren lesen] --> V[Verhalten wählen]
  V --> N[Netz fragen]
  N --> M[Servos bewegen]
  M --> S
```

Das Netz wird regelmäßig gefragt, aber nicht in jedem einzelnen Physikschritt. Wie bei echten Servos
kommt sein Befehl mit einer kleinen Verzögerung an den Motoren an, und auch die bildet Kern nach.
Alles Weitere über die Ente steht in [Kapitel 13](13-micro-duck.md).

## 8.15 Sound-Anbindung

Die Simulation spielt nie selbst Ton, sondern schreibt in ihren festen Schritten **Cues**,
Bestellzettel für Geräusche: für Sprünge, Landungen, Pickups, Trigger, Treffer, Explosionen,
Aufpralle und mehr. Darum bleibt sie deterministisch. Wie die Zettel entstehen und abgespielt werden,
steht in [Kapitel 7](07-audio.md), Abschnitte 7.7 und 7.8.

## 8.16 Touch, Maus und Gamepad

Auch die Touch-Steuerung stammt aus SpawnCity und ist in die Engine eingebaut, damit Studio und
exportierte Spiele sie gleich nutzen.

Ein **Schiedsrichter** entscheidet, welches Gerät gerade steuert: Das zuletzt benutzte gewinnt. Ein
Finger auf dem Bildschirm schaltet auf Touch, jede echte Taste auf Maus und Tastatur, auch am Handy.
Eine Maus am Tablet schaltet aber **nicht** auf Desktop. Den **Pointer-Lock** gibt es nur mit Maus:
Dabei bleibt der Mauszeiger im Spiel gefangen, damit du dich endlos umdrehen kannst.

Die **Touch-Steuerung** hat zwei Sticks in den unteren Ecken. Links bewegst du dich (MOVE, bei
Fahrzeugen STEER / GAS, beim Flyer FLY), rechts schaust du (LOOK). Daneben liegen Knöpfe je nach
Controller:

| Controller | Knöpfe |
|---|---|
| Soldier | JUMP, RELOAD, AIM, CROUCH, USE |
| Walker | QUACK, PICK, SIT, KICK L, KICK R, ROLL, BEAK, WHEEE, HOP |
| Character | JUMP, USE |
| Rolling Ball | JUMP, BOOST |
| Flyer | UP, DOWN, BOOST |
| Hover Car, Vehicle | BOOST, HOP, USE |

Knöpfe gehen vor Sticks, Sticks vor dem freien Bildschirm. Linker Stick ganz nach vorn heißt
Sprinten. Ziehst du den rechten Stick bis an den Rand, drehst du dich einfach weiter (**Edge Turn**).
Gamepads bekommen **Totzonen**: Ganz kleine Wackler am Stick werden ignoriert.

## 8.17 Logik in der Welt

Jeder Actor und das Level können einen Logik-Graphen tragen ([Kapitel 9](09-logik.md)). Die GameWorld
startet sie in Play und Simulate, meldet ihnen **Ereignisse** (Überlappung, Treffer, Pickup, Trigger,
Schaden, Tod, Taste, Spielende, neuer Spieler …) und lässt sie **Dinge tun** (Werte ändern, Kopien
erzeugen, schieben, Punkte geben, Spiel beenden …).

Logik läuft nur dort, wo das Spiel **„die Wahrheit hat“**. Allein ist das dein eigener Computer. Im
Mehrspieler ist es nur der **Server**: Er entscheidet, was passiert, und die Mitspieler bekommen das
Ergebnis geschickt. Würde jeder Computer seine eigene Logik rechnen, könnte bei einem eine Tür offen
und beim anderen zu sein. Darum schreibt Kern alles mit, was Logik verändert, und der Server schickt
es an alle ([Kapitel 10](10-netzwerk.md)).

## 8.18 Die Demo-Levels

Unter File › Example Levels und File › New Level findest du die eingebauten Levels:

- **Orb Run:** eine Stadt mit Verkehr, ein Hover Car und 24 Orbs zum Einsammeln. Es ist später
  Nachmittag, und der Lauf endet in der Nacht. Studio startet mit diesem Level.
- **Physics Lab:** alle Gelenkarten – Kistenpyramide, Pendel, Tür, Seilbrücke (drei Gelenke können
  brechen), Drehteller, Lift, ein selbstfahrender Rover. Du rollst als Kugel, sammelst fünf Sterne
  und erreichst das Goal, vorbei an einem drehenden Hazard.
- **Shooting Range:** ein Schießstand mit Zielscheiben in 10 bis 60 m, Deckung, Kisten, einem Fass
  und einem Turm. Ziel: 150 Punkte. Auch im Mehrspieler spielbar.
- **Micro Duck:** ein Zimmer im Maßstab des Roboters mit Ball und Socken. Die Ente läuft
  selbstständig. Ein Level-Graph lässt sie zwitschern, wenn ihre Kopfkamera den Ball sieht. Play
  beginnt hier mit der Observer camera, mit **F8** übernimmst du die Ente.
- **Basic:** Boden und ein Würfel.
- **Empty:** gar nichts, nicht einmal Boden.

Automatische Tests spielen diese Levels durch und prüfen zum Beispiel, dass ein Goal gewinnt und ein
Hazard verliert.

> **Mission:** Öffne **Orb Run** und starte **Play** (Alt+P). Sammle ein paar Orbs, dann drück
> **F8**: Du steigst aus, das Spiel läuft weiter, und du fliegst frei herum. Noch einmal **F8**, und
> du sitzt wieder am Steuer. Wie viele Sekunden brauchst du für alle 24?

## Level geschafft!

- Du weißt, wie Level, Actors und Komponenten zusammenhängen.
- Du kennst die wichtigsten Komponenten und ihre Aufgaben.
- Du kannst eine `.klevel`-Datei lesen und weißt: nachsichtig bei Fehlendem, streng bei Falschem.
- Du verstehst, warum Editor und Spiel denselben Plan für die Physik benutzen.
- Du weißt, was in jedem festen Schritt passiert und warum Kern zwischen zwei Schritten interpoliert.
- Du kennst die Controller, Siegbedingungen und Trigger-Aktionen.
- Du kannst ein kleines Spiel mit Ziel selbst bauen.

## Rätsel-Ecke

1. Was passiert mit deinen Änderungen, die während Simulate entstehen, wenn du stoppst?
2. Ein Pickup ohne Mesh und ohne Collider – womit merkt Kern, dass du es berührst?
3. Die Win condition ist „Collect all pickups“, und es gibt einen Goal-Trigger. Du hast alle Orbs.
   Hast du gewonnen?
4. Bei der Micro Duck soll gleichzeitig „Sitzen“ (1) und „Zwitschern“ (64) gedrückt werden. Welche
   Zahl schickt die Eingabe?
5. **Boss-Rätsel:** Die Physik läuft mit 60 Schritten pro Sekunde. Dein Computer schafft gerade nur
   20 Bilder pro Sekunde. Wie viele feste Schritte rechnet Kern im Schnitt pro Bild? Und wie viele
   wären es höchstens, wenn ein Bild eine ganze Sekunde hängt?

### Lösungen

1. Sie verschwinden. Simuliert wird an einer Kopie. Behalten kannst du die Lage ausgewählter Actors
   nur mit „Keep Simulation Changes“ (K).
2. Mit einer Sensor-Kugel von 1 m Radius, die Kern beim Planen der Physik automatisch anlegt.
3. Noch nicht. Erst wenn du danach das Goal erreichst.
4. 1 + 64 = 65.
5. Ein Bild dauert 1/20 s = 0,05 s, das sind 3 Schritte zu je 1/60 s. Hängt ein Bild eine Sekunde,
   rechnet Kern trotzdem höchstens 0,1 s nach, also 6 Schritte. Die Welt läuft dann kurz langsamer,
   statt in einem Rechenstau stecken zu bleiben.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
