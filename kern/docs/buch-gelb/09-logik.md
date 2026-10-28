# 9 Logik-Graphen – programmieren mit Kästchen und Drähten

> **Dein Level:** Jetzt lernst du, wie Spiele „denken“: Türen, die aufgehen, Ampeln, die umschalten,
> Punkte für eine Zone. In Kern schreibst du dafür keinen Code, sondern verbindest Kästchen mit
> Drähten. Und du erfährst, wie Kern diese Bilder im Hintergrund blitzschnell macht.

> **Wörter, die du brauchst:**
> - **Graph** – ein Bild aus Kästchen und Verbindungslinien.
> - **Knoten** – ein Kästchen im Graphen, das eine Sache tut oder etwas ausrechnet.
> - **Pin** – ein Anschluss am Rand eines Knotens, an den ein Draht kommt.
> - **Event** – ein Ereignis, das etwas in Gang setzt, etwa „Spiel beginnt“ oder „etwas berührt mich“.
> - **Variable** – ein beschriftetes Fach im Speicher, in dem sich der Graph einen Wert merkt.
> - **Schritt** – ein fester Rechenschritt der Simulation, meist 60 pro Sekunde
>   ([Kapitel 8](08-spiel-laufzeit.md)).
> - **Server** – im Mehrspieler der eine Computer, der entscheidet, was wirklich passiert.
> - **deterministisch** – bei gleichen Eingaben kommt immer genau dasselbe heraus.

> **So kannst du es dir vorstellen:** wie Scratch – Bausteine statt Text. Nur stapelst du sie nicht,
> du verbindest sie mit Drähten.

Kern hat **keine Skriptsprache**, in die man Text tippt. Spiellogik sind **Graphen**, gebaut wie die
Blueprints der Unreal Engine: Knoten und Drähte. Hinter jedem Kästchen steckt ein kleines Stück
Programm in der Engine. Kern prüft deinen Graphen und führt ihn in den festen Schritten der
Simulation aus.

**Wo läuft die Logik?** Nur dort, wo das Spiel **„die Wahrheit hat“**. Spielst du allein, ist das dein
Computer. Im Mehrspieler ist es nur der **Server**. Er rechnet die Logik, und die Mitspieler bekommen
das Ergebnis geschickt: Geht die Tür auf, sehen alle sie offen. So kann es nie passieren, dass sie bei
dir offen und bei deinem Freund zu ist.

**Wie läuft die Logik?** Dafür hat Kern drei Stufen, die alle genau dasselbe tun, nur verschieden
schnell:

> **So kannst du es dir vorstellen:** ein Buch in einer fremden Sprache.
> - **Vorlesen:** Ein Dolmetscher liest Satz für Satz und übersetzt jeden Satz in dem Moment, in dem
>   er ihn vorliest. Das klappt sofort, ist aber langsam.
> - **Übersetzen:** Das Buch wird einmal ganz übersetzt. Danach liest man es direkt, viel schneller.
>   Nur das Übersetzen selbst dauert einen Moment.
> - **Gedruckt mitliefern:** Die fertige Übersetzung wird gleich mit ins Spiel gepackt.

Kern beginnt immer mit dem Vorlesen, übersetzt im Hintergrund und wechselt dann um. Wie das geht und
warum dabei kein einziges Ergebnis anders sein darf, steht in 9.8 und 9.9.

## 9.1 Das Modell

Ein Graph hat zwei Arten von Drähten. **Weiße Drähte** (zwischen den **Exec-Pins**) sagen, **was als
Nächstes passiert**. **Farbige Drähte** (zwischen den **Daten-Pins**) tragen **Werte**, zum Beispiel
eine Zahl oder einen Text.

```mermaid
flowchart LR
  B[Begin Play] -- weißer Draht --> M[Show Message]
  T[Text: Hallo!] -. Daten-Draht .-> M
```

Lies das Bild so: Wenn das Spiel beginnt, läuft Show Message. Woher kommt der Text? Über den
Daten-Draht vom Textkästchen.

- Ein **Event-Knoten** startet eine Kette, etwa Begin Play. **Aktionen** setzen sie fort, etwa Show
  Message. **Flow-Knoten** verzweigen (Branch: wenn … dann …), wiederholen oder warten.
- **Rechenknoten** wie Plus oder Mal haben keine weißen Pins. Sie rechnen erst, wenn ein anderer
  Knoten ihr Ergebnis gerade braucht, so wie du einen Taschenrechner erst benutzt, wenn du das
  Ergebnis brauchst. Ein Eingang ohne Draht nimmt den Wert, der im Knoten eingetragen ist.
- **Variablen** gehören zum Graphen, haben einen Typ und einen Startwert.
- **Wartende Knoten** wie **Delay** (warte ein paar Sekunden) und **Tween** (zähle in einer Zeit weich
  von 0 bis 1) halten die Kette an und machen später weiter. Ruft man ein wartendes Delay noch einmal
  auf, überhört es das. Die Variante **Retriggerable Delay** fängt dann stattdessen von vorn an zu
  warten.
- **Kommentarrahmen** ordnen den Graphen für dich. Auf das Spiel wirken sie nicht.

Das Level hat einen **Level-Graphen**, und jeder Actor kann einen eigenen haben. Für ihn ist „Self“
der Actor selbst.

### Zustandsautomaten am Beispiel Ampel

Eine Ampel ist immer in genau **einem Zustand**: Grün, Gelb oder Rot. Nach einer Zeit wechselt sie in
den nächsten. Genau das ist eine **State Machine** (Zustandsautomat).

```mermaid
flowchart LR
  G[Green] -- nach 4 s --> Y[Yellow]
  Y -- nach 1,5 s --> R[Red]
  R -- nach 4 s --> G
```

Die Pfeile heißen **Übergänge**. Ein Übergang kann auf vier Arten auslösen: bei einem bestimmten
Event, nach einer Zeit (wie bei der Ampel), solange eine Bedingung wahr ist, oder sofort. Betritt
oder verlässt die Maschine einen Zustand, gibt es dafür ein Event. Daran hängst du Aktionen, etwa
„zeige den Namen des neuen Zustands an“. Genau diese Ampel gibt es als Vorlage (Mission in 9.10).

### Die Datentypen

Jeder Daten-Pin hat einen **Typ**. Scratch kennt Zahl, Text und wahr/falsch. Kern unterscheidet
genauer:

| Typ | bedeutet | Beispiel |
|---|---|---|
| Exec | „jetzt weiter“, kein Wert | der weiße Draht |
| Bool | wahr oder falsch | „Tür offen?“ |
| Int | ganze Zahl | 3, −7, 120 |
| Float | Kommazahl | 3,9 oder 0,25 |
| Vector | drei Zahlen x, y, z | eine Position (5, −12, 1,5) |
| Text | Buchstaben und Zeichen | „Hallo!“ |
| Actor | ein Ding im Level | die Tür, der Spieler |

Ein Vector ist wie ein Pfeil im Raum. Seine **Länge** rechnest du mit Pythagoras aus: Der Pfeil
(3, 4, 0) ist √(9 + 16) = 5 lang.

**Welche Drähte sind erlaubt?**

- gleicher Typ immer, ein weißer Exec-Draht nur mit Exec,
- **alles nach Text** („alles lässt sich ausdrucken“),
- Zahlen untereinander (Bool, Int, Float),
- Actor nach Bool („gibt es ihn?“),
- Zahl nach Vector (die Zahl kommt in alle drei Stellen).

Die Umwandlungen sind genau festgelegt: Der Text „0“ oder „false“ wird zu falsch, ein Vector als Zahl
ist seine Länge, eine Kommazahl als Int wird abgeschnitten (3,9 wird 3, nicht gerundet).

## 9.2 Knoten und die Bibliothek

Hinter jedem Kästchen steckt ein kleines Stück Programm in der Engine. Alle Knotentypen zusammen
bilden die **Bibliothek**. Jeder Typ hat dort eine Beschreibung: Titel, Kategorie, Hilfetext, seine
Pins und Einstellungen. Das Aktionsmenü im Editor durchsucht genau diese Bibliothek.

Wenn ein Knoten gelaufen ist, sagt er, wie es weitergeht: an welchem weißen Ausgang (ein Branch nimmt
**True** oder **False**), ob die Kette hier endet oder ob er erst einmal wartet. Manche Knoten ändern
ihr Aussehen mit ihren Einstellungen, zum Beispiel heißt Every N Seconds mit 3 Sekunden „Every 3 s“.

Wie ein Knoten im Programm genau aussieht, steht im blauen Buch, Kapitel 9.

## 9.3 Prüfen

Bevor ein Graph läuft, sucht Kern nach **Fehlern**: unbekannte Knoten, Drähte zu Pins, die es nicht
gibt, Typen, die nicht zusammenpassen, zwei Drähte in denselben Dateneingang, Einstellungen, die auf
eine Variable zeigen, die es nicht gibt, oder eine State Machine ohne genau einen Anfangszustand.
Eine **Warnung** gibt es für Knoten, die nie laufen können, weil kein weißer Draht zu ihnen führt.

Ein Graph mit Fehlern wird beim Start abgeschaltet und gemeldet. Der Rest des Spiels läuft weiter.
Im Studio zeigt **Compile** (F7) das Ergebnis.

## 9.4 Ausführung

Jeder Graph läuft für seinen **Besitzer**: ein Actor oder das Level. Spawnst du zehn Kopien einer
Kiste mit Graph, laufen zehn eigene Ausführungen, jede mit ihren eigenen Variablen. Wie zehn Schüler,
die alle dasselbe Arbeitsblatt bekommen, aber jeder schreibt in sein eigenes.

**In jedem Schritt** passiert der Reihe nach: Wartende Knoten, deren Zeit um ist, machen weiter. Dann
kommt das Event **Tick**. Dann prüfen die State Machines ihre Übergänge. Ein **Event** wie „etwas
überlappt“ geht an alle passenden Graphen, auch an die von Actors, die gerade erst entstanden sind.

Eigene Events baust du mit **Custom Event**: Du gibst ihm einen Namen, und ein **Call Event** mit
diesem Namen löst es aus, in allen Graphen oder nur beim Ziel-Actor. Derselbe Name kann auch einen
Übergang einer State Machine schalten.

**Der Stapel.** Manche Knoten haben mehrere Ausgänge nacheinander, etwa **Sequence** („erst das,
dann das, dann das“). Kern macht den ersten sofort und legt sich die anderen auf einen **Stapel**.
Endet ein Zweig, nimmt es den obersten Eintrag vom Stapel und macht dort weiter.

> **So kannst du es dir vorstellen:** ein Stapel Hausaufgabenzettel. Du arbeitest die aktuelle
> Aufgabe ab. Fällt dir unterwegs ein „danach noch Mathe“, legst du den Zettel oben drauf. Bist du
> fertig, nimmst du den obersten.

**Schutz vor Endlosschleifen:** Kern zählt mit, wie viele Knoten ein Graph in einem Schritt ausführt.
Sind es mehr als **200 000**, schaltet Kern diesen Graphen ab und meldet „infinite loop? The graph is
stopped.“ Ebenso stoppt eine Kette, wenn Events immer wieder neue Events auslösen und das zu tief
verschachtelt wird.

> **Vorsicht, Falle:** Ein While Loop, dessen Bedingung nie falsch wird, friert in vielen Engines das
> ganze Spiel ein. In Kern stoppt nur dein Graph – mit Meldung. Lies sie im Output Log!

**State Machines im Takt:** Beim Start betritt jede Maschine ihren Anfangszustand. Pro Schritt darf
jede Maschine **höchstens einen** Übergang machen. Hätte die Ampel einen Übergang „sofort“ von Gelb
nach Rot, stünde sie trotzdem einen Schritt lang auf Gelb. Mit **Set State** springst du direkt in
einen Zustand.

> **Für Profis:** Daten-Drähte werden bei jedem Lesen neu ausgerechnet, durch höchstens 48
> Rechenknoten hintereinander. Die übrigen Grenzen stehen im blauen Buch, Kapitel 9.

## 9.5 Kern-Knoten

Kern bringt **76** eingebaute Knoten mit:

| Kategorie | Beispiele |
|---|---|
| Events | Begin Play, Event Tick, Every N Seconds, Custom Event |
| State Machines | On State Entered/Left, Set State, Current State |
| Flow | Branch, Sequence, For, While, Do Once, Do N, Gate, Flip Flop, Delay, Tween, Switch |
| Variablen | Get, Set, Add |
| Mathe | plus, minus, mal, geteilt, Rest, Wurzel, Zufall, Winkelfunktionen |
| Logik | Compare, AND, OR, XOR, NOT, Select |
| Vektor | zusammenbauen, zerlegen, Länge, Abstand |
| Text, Werte | Append, Contains, feste Werte |
| Utilities | Game Time, Print |

Ein paar Flow-Knoten erklärt: **Do Once** lässt nur beim ersten Mal durch, **Do N** genau N-mal.
**Gate** ist eine Tür für die Kette, die man öffnen und schließen kann. **Flip Flop** nimmt
abwechselnd Ausgang A und B, wie ein Lichtschalter. **Select** wählt je nach wahr oder falsch einen
von zwei Werten.

**Zufall** kommt von der Welt und ist **deterministisch**: Er beginnt bei jedem Start desselben Levels
mit derselben Startzahl.
So stimmen Wiederholungen und alle Mitspieler im Netz überein.

## 9.6 Spiel-Knoten

Dazu kommen **71** Spiel-Knoten. Ein Actor-Eingang ohne Draht bedeutet „Self“.

**Events:** On Begin/End Overlap (etwas betritt oder verlässt den Sensor), On Entered Sensor, On Hit
(Aufprall ab 2 m/s), On Collected, On Trigger, On Damaged, On Death, On Respawn, On Interact (Taste F
oder Y am Gamepad, wenn du den Actor anschaust oder direkt daneben stehst), On Player Joined, On Game
Over, On Input Action und On Key. Eine Überlappung zählt den Besucher als Ganzes: Körper und Kopf
eines Spielers sind ein Besucher.

**Actors:** Self, Get Player, Find Actor by Name oder Tag, Ort, Drehung, Bewegen, Spawn Actor (eine
Kopie einer Vorlage), Destroy und Set Property. Set Property ändert jeden Wert, den du auch in Details
einstellen kannst, etwa die Farbe.

**Physik:** Tempo, Stoß, Kraft, Hochschleudern, Physik an/aus, Gelenkmotor, Explosion und **Line
Trace**, ein Prüfstrahl, der meldet, was er zuerst trifft.

**Spiel und Effekte:** Punkte, Show Message, Win, Lose, Schaden, Heilen, Munition, Licht, Farbe,
Funken, Kamerawackeln, Play Sound und For Each Actor with Tag.

**Roboter:** Die Micro Duck lässt sich lenken (in den Grenzen, die ihr Netz trainiert hat), den Kopf
drehen, Bewegungen auslösen (Sitzen, Aufheben, Kicks, Rolle, Sprung), zwitschern („Chirp“), den
Schnabel bewegen, die Autonomie an- und ausschalten. Man kann ihren Abstandssensor (**ToF**, misst wie
weit das Nächste vor ihr ist), ihre Kamera und ihren Zustand abfragen. Einzelheiten:
[Kapitel 13](13-micro-duck.md).

> **Mission:** Öffne auf https://kernengine.de/ ein **Basic**-Level und klicke in der Toolbar auf
> **Logic**. Klicke **Begin Play**, zieh vom Ausgang „then“ einen Draht ins Leere, tippe „show
> message“, Enter, und trage einen Text ein. Zurück mit **Viewport**, platziere einen **Rolling
> Ball** und drück **Play**. Dein Text erscheint im HUD!

## 9.7 Logik in der GameWorld

Die GameWorld startet Logik nur in Play oder Simulate, nur auf dem Computer, der die Wahrheit hat
(siehe Kapitelanfang), und nur, wenn es überhaupt Graphen gibt.

Die Welt gibt der Logik drei Dinge: die **Zeit**, die **Ausgabe** und den **Zufall**. Die Zeit ist
**Spielzeit**, gezählt in festen Schritten, nicht die Uhr an der Wand. Pausierst du das Spiel, steht
auch die Zeit der Logik. Und ein Graph tickt auf einem langsamen Computer genauso wie auf einem
schnellen. Was ein Graph ausgibt, erscheint als „Logic: …“ im Output Log.

Ein Ereignis geht an den Graphen des betroffenen Actors oder, wenn der keinen hat, an den seines
nächsten Vorfahren mit Graph. Der Collider einer Tür meldet sich also bei der Tür. Der Level-Graph
bekommt jedes Ereignis zusätzlich. Die Logik läuft am Ende jedes Schritts, nach der Physik.

Damit Graphen überhaupt von Berührungen erfahren, schaltet Kern in Levels mit Logik die
Berührungsmeldungen der Physik für alle beweglichen Körper ein ([Kapitel 6](06-physik.md)). Ohne
Logik spart es sich diese Arbeit.

Was Logik ändert, etwa einen Wert, eine neue Kopie oder eine versetzte Wand, schreibt Kern mit. Der
Server schickt es an alle Mitspieler, auch an die, die erst später dazukommen
([Kapitel 10](10-netzwerk.md)).

## 9.8 Vom Graphen zu schnellem Code

Kern übersetzt deinen Graphen beim Start von Play oder Simulate **im Hintergrund** in schnellen Code.
Bis die Übersetzung fertig ist, läuft der Graph im langsamen „Vorlese“-Modus, aber schon richtig.
Danach ist er etwa **55-mal schneller**. Gemessen wird das mit einer Million Schleifendurchläufen.
Klappt das Übersetzen einmal nicht, bleibt es einfach beim Vorlesen.

Wie beim Buch wird nicht alles übersetzt: Einfache Dinge wie Verzweigungen, Schleifen, Zahlen und
Rechnungen werden direkt übersetzt. Für alles andere, etwa „Spawn Actor“, ruft der übersetzte Code
das fertige Programmstück des Knotens in der Engine auf.

Einen Graphen kannst du auch **exportieren**, als fertigen Programmtext, der mit ins Spiel kommt.
Damit Kern weiß, zu welchem Graphen der Export gehört, bekommt er einen **Fingerabdruck** (*Hash*):
eine Zahl, die aus dem Inhalt des Graphen berechnet wird. Ändert sich der Inhalt, ändert sich die
Zahl. Positionen und Kommentare zählen nicht mit. Knoten verschieben macht den Export also nicht
ungültig, jede inhaltliche Änderung schon. Dann musst du neu exportieren. Gibt es zu einem Graphen
einen passenden Export, nimmt Kern gleich den.

> **Für Profis:** Das Übersetzen im laufenden Spiel heißt **JIT** („just in time“, gerade
> rechtzeitig). Kern übersetzt in **WebAssembly**, die Sprache, die der Browser blitzschnell in
> Maschinencode verwandelt. Der Export ist lesbares **C++**. Beide Übersetzer bauen dasselbe
> Programm: eine Schleife mit einem Zähler, der auf den gerade laufenden Knoten zeigt.

## 9.9 Warum alle drei Stufen gleich rechnen müssen

Der Wechsel vom Vorlesen zum übersetzten Code passiert **mitten im Spiel**. Ein Graph beginnt im
Vorlese-Modus, macht Begin Play, die ersten Ticks, zählt vielleicht schon eine Schleife hoch, und
dann übernimmt der schnelle Code.

> **So kannst du es dir vorstellen:** ein Staffellauf. Der zweite Läufer muss genau dort
> weiterlaufen, wo der erste aufgehört hat, mit demselben Stab in der Hand. Kern sorgt dafür, dass
> beide Stufen ihre Zahlen (Variablen, Schleifenzähler, offene Gates) am selben Platz im Speicher
> ablegen. Beim Wechsel muss nichts übergeben werden, der Stab liegt schon da.

Und Kern ist **deterministisch** ([Kapitel 6](06-physik.md)). Würde der übersetzte Code auch nur ein
winziges bisschen anders rechnen als das Vorlesen, liefe eine Wiederholung anders als das Original,
und beim Training der Ente oder bei der Fehlersuche könnte man den Ergebnissen nicht mehr trauen.
Auch die Grenze gegen Endlosschleifen zählt überall gleich, und Haltepunkte funktionieren auch im
übersetzten Code.

Geprüft wird das mit **denselben Testfällen** in allen drei Stufen: Schleifen, Gates, verschachtelte
While Loops, State Machines, eine absichtliche Endlosschleife. Das Ergebnis muss **identisch** sein.

## 9.10 Bearbeiten im Studio

Der Logik-Editor sieht aus wie Unreals Blueprint-Editor; seine Oberfläche beschreibt
[Kapitel 11](11-studio.md). Jede Bearbeitung ist **ein Undo-Schritt** im Level, du kannst also alles
mit Ctrl+Z zurücknehmen.

Du suchst Knoten im **Aktionsmenü**. Ziehst du dabei von einem Pin, zeigt es nur Knoten, die dazu
passen. Du verbindest Knoten, legst Variablen und State Machines an, machst aus einem Pin mit
**Promote to Variable** eine Variable, prüfst mit **Compile** und exportierst. Mit **F9** setzt du
einen **Breakpoint** (Haltepunkt): Läuft der Knoten, hält das Spiel nach dem Schritt an, und der
Editor springt hin. Beim Spielen siehst du live, welche Knoten liefen und welche Werte die Variablen
haben.

> **Mission:** Füge im Level-Graphen deines Levels mit dem Rolling Ball über **Templates** die
> Vorlage **Traffic light (state machine)** ein und drück **Play**: Das HUD zeigt nacheinander
> Green, Yellow, Red. Stoppe mit **Esc**, wähle im Graphen einen Knoten, setze mit **F9** einen
> Breakpoint und drück wieder **Play** – das Spiel hält an, sobald der Knoten läuft.

## 9.11 Beispiele

Alle sind echte Graphen aus Kern, die meisten gibt es als Vorlage unter **Templates**.

**Punktezone:** On Begin Overlap → Add Score (10) → Show Message („+10“) → Play Sound (pickup). Wer
den Sensor betritt, bekommt 10 Punkte, eine Meldung und einen Ton.

**Tor:**

```mermaid
flowchart LR
  I[On Interact] --> F[Flip Flop]
  F -- A --> T1[Tween 0,8 s]
  T1 --> M1[Move By +0,05 hoch]
  F -- B --> T2[Tween 0,8 s]
  T2 --> M2[Move By −0,05 runter]
  I --> S[Play Sound confirm]
```

Interact wechselt per Flip Flop zwischen Öffnen und Schließen. Jeder Tween läuft 0,8 s und schiebt
das Tor in jedem Schritt um 5 cm nach oben oder unten, wie ein Garagentor. Bei 60 Schritten pro
Sekunde sind das rund 48 Schritte, also etwa 2,4 m.

**Spawner:** Alle 3 s, solange die Variable „Spawned“ unter 10 liegt: einen Physics Cube in 6 m Höhe
erzeugen und „Spawned“ um 1 erhöhen. Die Kopien sehen alle Mitspieler.

**Ampel:** die State Machine aus 9.1. Beim Betreten jedes Zustands wird sein Name angezeigt.

**Hindernis ausweichen:** Die Micro Duck läuft nicht mehr allein. Alle 0,25 s: Sieht ihr ToF etwas
näher als 1,6 m, dreht sie 0,6 s auf der Stelle, sonst geht sie geradeaus.

**Die Ente zwitschert beim Ball:** Im Level Micro Duck fragt ein Graph alle 0,25 s: Sieht die
Kopfkamera den Ball, und hat sie ihn vorher **nicht** gesehen? Dann zwitschert sie. Eine Bool-Variable
„Ball Seen“ merkt sich den letzten Stand, sonst ginge das Zwitschern alle 0,25 s los, solange der Ball
im Bild ist. Ein Kommentar im Graphen sagt: Das ist eine Demo, kein Verhalten des echten Roboters.

> **Mission:** Öffne File › Example Levels › **Micro Duck** und dort den Level-Graphen (**Logic**).
> Finde die Variable „Ball Seen“ und den Branch. Starte Play. Play beginnt hier mit der freien
> Kamera: Greif den Ball mit der linken Maustaste und zieh ihn vor den Kopf der Ente. Sie zwitschert
> einmal, und erst wieder, wenn der Ball weg war und neu auftaucht.

## Level geschafft!

- Du kennst weiße Exec-Drähte, Daten-Drähte, Variablen und State Machines.
- Du weißt, was Bool, Int, Float und Vector sind, welche Drähte erlaubt sind und wie Werte umgewandelt
  werden.
- Du verstehst, wie Kern mit einem Stapel arbeitet und Endlosschleifen abfängt.
- Du weißt, warum Logik nur auf dem Computer mit der Wahrheit läuft.
- Du kennst die drei Stufen Vorlesen, Übersetzen und Mitliefern und weißt, warum sie gleich rechnen
  müssen.
- Du kannst im Studio einen eigenen Graphen bauen und mit Haltepunkten untersuchen.

## Rätsel-Ecke

1. Was zeigt ein weißer Draht zwischen zwei Knoten an?
2. Darf man einen Actor-Ausgang mit einem Text-Eingang verbinden?
3. Ein Text-Eingang bekommt die Zahl 3,9, ein Int-Eingang auch. Was kommt jeweils an?
4. Du verschiebst nach dem Export ein paar Knoten. Musst du neu exportieren?
5. **Boss-Rätsel:** Ein Custom Event führt bei jedem Aufruf 4000 Knoten aus, sonst läuft keine
   Logik. Wie oft darf es in einem Schritt laufen, bevor Kern den Graphen abschaltet?

### Lösungen

1. Die Reihenfolge: was als Nächstes passiert.
2. Ja. Alles lässt sich in Text umwandeln.
3. Beim Text kommt die Zahl als Text an, beim Int wird abgeschnitten: 3.
4. Nein. Positionen zählen nicht zum Fingerabdruck. Erst eine inhaltliche Änderung verlangt einen
   neuen Export.
5. 200 000 / 4000 = 50-mal. Das sind genau 200 000 Knoten, also noch erlaubt. Beim 51. Mal wäre die
   Grenze überschritten.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
