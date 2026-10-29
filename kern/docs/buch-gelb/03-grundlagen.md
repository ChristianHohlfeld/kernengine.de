# 3 Das Fundament

> **Dein Level:** Jedes Hochhaus braucht ein Fundament. Hier lernst du die Ideen, auf denen ganz Kern
> steht: wie ein Computer Zahlen speichert und rundet, wie er Fehler meldet, wie er Daten als Text
> aufschreibt, wie er im Raum rechnet – und warum er auf jedem Rechner exakt gleich rechnen muss.

> **Wörter, die du brauchst:**
> - **Funktion** – ein kleines Teilprogramm mit Namen: Es bekommt Werte, rechnet und gibt ein
>   Ergebnis zurück.
> - **Bibliothek** – eine Sammlung fertiger Funktionen, die viele Programme mitbenutzen, zum
>   Beispiel für Mathe.
> - **Daten** – alle Zahlen und Texte, mit denen ein Programm arbeitet.
> - **Renderer** – der Teil der Engine, der die Bilder zeichnet.
> - **Browser-Konsole** – ein verstecktes Fenster im Browser, in dem Webseiten Meldungen hinterlassen.
>   In vielen Browsern öffnet es sich mit F12.
> - **Datei** – ein gespeichertes Stück Daten mit einem Namen, etwa ein Foto oder ein Level.
> - **Servo** – einer der kleinen Motoren in den Gelenken der Micro Duck. Sein **Profil** beschreibt
>   in ein paar Zahlen, wie stark, wie steif und wie schnell er ist.

## 3.1 Zahlen im Computer

Ein Computer speichert jede Zahl in einer festen Zahl von **Bits**. Mehr Bits heißt: größere Zahlen
oder mehr Stellen. Kern schreibt bei jeder Zahl dazu, wie viele Bits sie hat und ob sie ein
Vorzeichen haben darf.

> **So kannst du es dir vorstellen:** ein Zahlenschloss, dessen Rädchen nur 0 und 1 kennen. Mit
> einem Rädchen gibt es 2 Möglichkeiten, mit zwei Rädchen 2 · 2 = 4, mit acht Rädchen
> 2 · 2 · 2 · 2 · 2 · 2 · 2 · 2 = 256. Eine Zahl aus 8 Bit kann also die Werte 0 bis 255 haben.

Kern benutzt Zahlen mit 8, 16, 32 oder 64 Bit. Warum ist die genaue Breite so wichtig? An der Tür
zwischen C++ und JavaScript ([Kapitel 1](01-einfuehrung.md)) müssen beide Seiten wissen, wie groß
jedes Schließfach ist. Liest die eine Seite 32 Bit, wo die andere 64 hingelegt hat, kommt Unsinn
heraus.

**Kommazahlen werden gerundet.** Auch Kommazahlen haben nur eine feste Zahl von Bits. Eine
Kommazahl mit 32 Bit hat ungefähr 7 Ziffern, eine mit 64 Bit ungefähr 16. Alles dahinter wird
gerundet. 1 : 3 ergibt also nicht 0,3333… ohne Ende, sondern zum Beispiel 0,3333333. Fast jede
Rechnung mit Kommazahlen endet deshalb mit einem kleinen Rundungsschritt. Das ist kein Fehler, das
ist einfach so. Wichtig wird es in 3.9.

> **Für Profis:** Kern nennt die Zahlentypen nach Art und Breite, etwa „u8“ (Ganzzahl ohne
> Vorzeichen, 8 Bit) oder „f64“ (Kommazahl, 64 Bit, genau JavaScripts Zahlentyp). Alle Typen stehen
> im blauen Buch, Kapitel 3.

## 3.2 Fehler sauber melden

### Fehler ohne Drama

Manches kann schiefgehen: eine kaputte Datei, eine unmögliche Kamera, ein Effekt, den die
Grafikkarte nicht kann. Viele Programme werfen dann einen „Alarm“ aus, eine sogenannte **Ausnahme**,
die quer durchs Programm fliegt, bis sie jemand auffängt – oder bis das Programm abstürzt. Kern macht
das nie. Bei Kern antwortet eine Funktion, die scheitern kann, einfach mit **„hat nicht geklappt“**
und legt vorher einen Zettel mit dem Grund hin.

> **So kannst du es dir vorstellen:** Du bittest deinen Bruder, Milch zu holen. Er kommt zurück und
> sagt: „Hat nicht geklappt – der Laden war zu.“ Kein Geschrei, aber du weißt, was los ist, und
> kannst entscheiden, was du jetzt machst.

Ein Beispiel: Beim Anklicken eines Dings prüft Kern zuerst, ob die Kamera gültig ist, denn Kern muss
ausrechnen, wohin du durch die Kamera zeigst. Ist sie es
nicht, schreibt Kern „invalid camera“ (ungültige Kamera) auf den Zettel und meldet „hat nicht
geklappt“. Wer wissen will, warum, liest den Zettel – auch von JavaScript aus.

> **Für Profis:** Es gibt nur **einen** Zettel für alle. Jede neue Fehlermeldung überschreibt die
> vorige. Wer sicher die Meldung zu seinem eigenen Aufruf lesen will, wischt den Zettel vorher leer.
> Die Funktionsnamen stehen im blauen Buch, Kapitel 3.

### Die Keine-Zahl: NaN

Rechnet ein Computer 0 durch 0, kommt keine Zahl heraus, sondern **NaN** („Not a Number“, keine
Zahl). NaN hat eine verrückte Eigenschaft: **Jeder Vergleich mit NaN ist falsch.** „NaN ist größer
als 0“ – falsch. „NaN ist kleiner oder gleich 0“ – auch falsch.

Das ist eine Falle. Angenommen, eine Größe x muss positiv sein, und Kern will alles Ungültige
abfangen. Rechne beide Prüfungen mit x = NaN durch:

| Prüfung | mit x = NaN | Fehler bemerkt? |
|---|---|---|
| „Ist x kleiner oder gleich 0?“ | falsch | nein, NaN rutscht durch |
| „Ist x **nicht** größer als 0?“ | „größer als 0“ ist falsch, „nicht falsch“ ist wahr | ja |

Darum fragt Kern oft „Ist x nicht größer als 0?“ statt „Ist x kleiner oder gleich 0?“. Für normale
Zahlen bedeuten beide dasselbe. Nur die zweite Frage fängt auch NaN.

### Meldungen aufschreiben

Kern schreibt Meldungen auf, von harmlosen Hinweisen bis zu Fehlern. Im Browser
landen sie in der Browser-Konsole, mit „[kern]“ davor. Ein Beispiel: Kann die Grafikkarte den
Leuchteffekt um helle Lichter (**Bloom**) nicht, schaltet Kern ihn ab, schreibt eine Warnung samt
Grund auf und macht ohne ihn weiter. Das Bild ist dann schlichter, aber das Spiel läuft.

## 3.3 Die Uhr bleibt draußen

Kern kann die Uhr lesen, auf die Tausendstelsekunde genau. Das braucht es zum **Messen**: Wie lange
dauert ein Bild? Wie viel Zeit bleibt noch, um Klänge vorzubereiten? Aber in die **Simulation** gehört
die Uhrzeit nie. Eine Simulation zählt Schritte, keine Sekunden auf der Uhr.

> **So kannst du es dir vorstellen:** ein Daumenkino. Jedes Blatt ist ein Schritt. Ob du schnell
> oder langsam blätterst, die Geschichte auf den Blättern bleibt dieselbe.

Warum ist das so wichtig? Stell dir vor, die Simulation richtet sich nach der Uhr. Ein schneller PC
schafft in einer Sekunde viele kleine Schritte, ein altes Tablet nur wenige große. Jeder Schritt endet
mit einem kleinen Rundungsschritt (3.1), also runden beide unterschiedlich oft an unterschiedlichen
Stellen. Nach ein paar Sekunden steht die Ente auf dem PC woanders als auf dem Tablet, und der
Fingerabdruck aus [Kapitel 2](02-bauen-und-testen.md) wäre kaputt.

Darum zählt Kern feste Schritte. Die Physik der Micro Duck zum Beispiel rechnet 100 feste Schritte
pro Sekunde Simulationszeit. Braucht ein langsamer Rechner für diese 100 Schritte zwei echte Sekunden, läuft
die Ente langsamer über den Bildschirm, aber sie macht genau dieselben Schritte.

## 3.4 Ausleihen und wiedererkennen

### Der Span: das Schaufenster

Daten zu kopieren kostet Zeit, besonders wenn es Tausende Zahlen sind, etwa die Ecken eines
3D-Modells. Darum gibt man der Engine oft keine Kopie, sondern nur einen Hinweis: „Die Zahlen liegen
dort, es sind so und so viele.“ Diesen Hinweis nennt Kern einen **Span**. Ein Span **besitzt** die
Daten nicht, er zeigt nur darauf. In diesem Format fließen alle Daten in die Engine, aus C++ wie aus
JavaScript.

> **So kannst du es dir vorstellen:** Ein Span ist ein Schaufenster: Du siehst die Ware, aber sie
> gehört dir nicht. Das ist viel schneller, als alles zu kaufen. Räumt der Laden aber aus, ist das
> Fenster leer.

Daraus folgt eine Regel: Ein Span darf nie länger benutzt werden, als seine Daten leben. Wer Daten
behalten will, kopiert sie. Der Renderer hält darum eigene Kopien von allem, was er zeichnet. Mit
ihnen findet er heraus, was unter der Maus liegt, und baut alles neu auf, wenn der Browser ihm kurz
die Grafikkarte weggenommen hat ([Kapitel 1](01-einfuehrung.md), [Kapitel 4](04-grafik.md)).

### Hashes: die Kennzahl

Einen **Hash** kennst du schon aus [Kapitel 2](02-bauen-und-testen.md): eine Kennzahl, die aus vielen
Daten entsteht. Statt 1000 Zahlen einzeln zu vergleichen, vergleicht man zwei Kennzahlen. Sind sie
verschieden, hat sich etwas geändert. Der Renderer erkennt so zum Beispiel, ob er einen Wert gar nicht
neu an die Grafikkarte schicken muss.

Eine zweite Sorte Hash mischt eine einzelne Zahl kräftig durch. Damit macht das Spiel „Zufall“, der
zufällig aussieht, aber bei jedem Start gleich ausfällt. Ein Geräusch klingt dann zum Beispiel nicht
jedes Mal exakt gleich, sondern mal etwas höher, mal etwas tiefer. Warum soll der Zufall jedes Mal
gleich sein? Echter Zufall fiele auf jedem Rechner und bei jedem Start anders aus. Dann liefe ein
Spiel nie zweimal gleich ab, und einen Fehler könnte man nicht wiederholen, um ihn zu finden
([Kapitel 1](01-einfuehrung.md)). Beide Sorten rechnen nur
mit ganzen Zahlen, also ohne Runden: Gleiche Daten ergeben auf jedem Rechner dieselbe Kennzahl.

## 3.5 Daten als JSON

Levels, Studio-Befehle, Einstellungen der Servos: Kern schreibt das alles als **JSON** auf. JSON ist
eine einfache Art, Daten als Text zu speichern, die Menschen und Programme lesen können. So sieht ein
Würfel in einem Level ungefähr aus:

```json
{
  "name": "Cube",
  "transform": {
    "position": [0, 0, 1],
    "rotation": [0, 0, 90]
  }
}
```

In geschweiften Klammern stehen Paare aus **Name: Wert**. Ein Wert kann eine Zahl sein, ein Text in
Anführungszeichen, eine Liste in eckigen Klammern oder wieder ein Paar-Block. Die Namen sind
englisch: „position“ ist der Ort, „rotation“ die Drehung, und „transform“ fasst beide zusammen,
also die Lage des Dings. Hier heißt der Actor „Cube“. Er ist um 90° um die Hochachse gedreht (die
dritte Zahl bei „rotation“), und er steht 1 Meter
über dem Boden, denn bei „position“ ist die dritte Zahl die Höhe. Warum gerade die dritte, zeigt 3.6.

> **So kannst du es dir vorstellen:** ein ausgefüllter Steckbrief. Jede Zeile hat eine Überschrift
> und einen Eintrag. Ein Steckbrief kann sogar einen kleineren Steckbrief enthalten.

> **Mission:** Speichere ein Level mit **Strg+S** (auf dem Mac **Cmd+S**). Beim ersten Mal fragt dich der Browser nach einem
> Ort, oder die Datei landet in deinem Download-Ordner. Die Endung „.klevel“ kennt dein Computer
> nicht, ein Doppelklick hilft also nicht. Klick die Datei stattdessen mit der rechten Maustaste an,
> wähl **Öffnen mit** und dann einen einfachen Texteditor (unter Windows „Editor“, auf dem Mac
> „TextEdit“). Es ist JSON! Finde
> ganz oben `"format": "kern-level"` – diese Zeile sagt jedem Programm: „Ich bin eine Kern-Level-Datei.“
> Such dann bei deinen Actors die Drehungen in Grad.

Kerns JSON-Baustein verspricht drei Dinge:

1. **Die Reihenfolge bleibt.** Liest Kern eine Datei ein und schreibt sie unverändert wieder hinaus,
   kommt Zeichen für Zeichen dieselbe Datei heraus. Ändert man ein Level, sieht man beim Vergleichen
   von alter und neuer Datei nur die Zeilen, die sich wirklich geändert haben.
2. **Zahlen wie im Browser.** Kern schreibt jede Zahl genau so auf, wie JavaScript es tut. Dieselbe
   Zahl ergibt also auf beiden Seiten denselben Text. Wozu? Hat JavaScript eine Datei geschrieben
   und Kern liest sie ein und speichert sie wieder, kommt Zeichen für Zeichen dieselbe Datei heraus.
   Versprechen 1 hält also auch bei Zahlen.
3. **Sicher gegen böse Dateien.** Eine Datei aus dem Internet kann kaputt sein oder absichtlich gemein
   gebaut. Kern lehnt zum Beispiel Steckbriefe ab, die zu tief ineinandergeschachtelt sind, wie eine
   Matroschka mit Hunderten Puppen, und Zahlen, die unendlich groß wären.

Fehlt ein Eintrag, bekommt Kern „nichts“ zurück statt eines Absturzes.

> **Für Profis:** Sehr große Zahlen schreibt Kern wie JavaScript: 10²⁰ ganz aus, mit 20 Nullen, aber
> 10²¹ als „1e+21“. Die genaue Tiefengrenze und die verbotenen Namen, mit denen man sonst
> JavaScript-Seiten angreifen könnte, stehen im blauen Buch, Kapitel 3.

### Erst prüfen, dann übernehmen

Das ist eines der wichtigsten Prinzipien in Kern: **Erst wird alles geprüft, dann wird übernommen –
alles oder nichts.**

Ein Beispiel: Kern lädt das Profil eines Servos (aus dem Wörterkasten) aus einer Datei, sagen wir
fünf Werte. Kern
liest Wert 1, prüft ihn: Ist es eine Zahl, liegt sie im erlaubten Bereich? Dann Wert 2, Wert 3. Wert 4
ist kaputt. Jetzt bricht Kern ab und meldet den Grund. Das alte Servoprofil bleibt **vollständig**
unberührt. Erst wenn alle fünf Werte geprüft sind, ersetzt Kern das alte Profil in einem Rutsch durch
das neue.

> **So kannst du es dir vorstellen:** Kochen nach Rezept. Du checkst zuerst, ob alle Zutaten da sind,
> und fängst erst dann an. Sonst stehst du mit halb verrührtem Teig da, und das Ei fehlt.

Warum so streng? Ein halb übernommenes Profil – drei neue Werte, zwei alte – wäre eine Mischung, die
niemand wollte. Genauso liefert der JSON-Leser sein Ergebnis nur, wenn die ganze Datei in Ordnung
war, und seine Fehlermeldung nennt die Stelle, an der es klemmt.

## 3.6 Positionen und Richtungen als Pfeile

### Kerns Regeln für den Raum

| Regel | Festlegung |
|---|---|
| Einheiten | Meter, Sekunden, Kilogramm |
| Oben | **+Z** |
| Achsen | „rechtshändig“ |

**Z zeigt nach oben**, überall: in der Grafik, der Physik, beim Ton und in den Levels. X und Y liegen
flach auf dem Boden wie das Koordinatensystem in deinem Heft.

„Rechtshändig“ kannst du mit deiner rechten Hand prüfen: Streck den Daumen nach rechts, den
Zeigefinger nach vorn und den Mittelfinger nach oben. Daumen ist X, Zeigefinger Y, Mittelfinger Z. Mit
der linken Hand klappt das nicht – dort zeigt der Mittelfinger nach unten.

> **Mission:** Wähl **Datei › Neues Level...** (File › New Level...), die Vorlage **Basis**
> (Basic) und **Erstellen** (Create). Setz über die **Inhaltsablage** (Content Drawer) – der Knopf
> ganz links in der Statusleiste – einen **Würfel** (Cube). Unten links zeigt das Achsenkreuz X rot, Y grün,
> Z blau – nach oben. Zieh mit dem Werkzeug **Verschieben (W)** (Move (W)) am blauen Pfeil und
> beobachte in **Details** das Feld **Position** (Location) Z.

### Vektoren: Pfeile aus Zahlen

Ein **Vektor** ist ein Pfeil. In der Ebene beschreiben ihn zwei Zahlen: wie weit nach rechts, wie
weit nach vorn. Im Raum kommt eine dritte dazu: wie weit nach oben. Mit Vektoren beschreibt Kern
Positionen (ein Pfeil vom Nullpunkt (0, 0, 0) zum Punkt), Richtungen und Geschwindigkeiten.

Zeichne in dein Heft einen Pfeil von (0, 0) nach (3, 4): 3 Kästchen nach rechts, 4 nach oben. Wie
lang ist er? Leg das Lineal an und miss in Kästchen: genau **5**. Man kann das auch ausrechnen, mit
dem Satz des Pythagoras, den du meist in Klasse 8 oder 9 lernst: 3 · 3 + 4 · 4 = 9 + 16 = 25, und
25 ist 5 · 5. Hier reicht: Die Länge eines Pfeils lässt sich aus seinen Zahlen ausrechnen, und Kern
macht das ständig.

> **Für Profis:** Im Raum geht es genauso, nur mit drei Zahlen. Der Pfeil (2, 3, 6) hat
> 2 · 2 + 3 · 3 + 6 · 6 = 4 + 9 + 36 = 49, und 49 ist 7 · 7. Er ist also **7** Meter lang.

Oft braucht man nur die **Richtung** eines Pfeils, nicht seine Länge. Dann teilt man jede Zahl durch
die Länge: (3, 4) geteilt durch 5 ergibt (0,6; 0,8). Der Pfeil zeigt noch genauso, ist aber genau 1
lang.

> **Vorsicht, Falle:** Der Pfeil (0, 0, 0) hat die Länge 0 und gar keine Richtung. Teilt man durch 0,
> kommt NaN heraus (3.2), und das würde sich durch die ganze Szene fressen. Das passiert schnell:
> Ein Pfeil von einem Punkt zu genau demselben Punkt hat die Länge 0, etwa wenn jemand zweimal
> denselben Punkt anklickt. Kern gibt in diesem Fall einen Ersatzpfeil zurück,
> der nach oben zeigt.

Weitere Pfeil-Rechnungen, etwa Abstände oder das Mischen zweier Pfeile, stehen im blauen Buch,
Kapitel 3.

## 3.7 Drehen

### Grad und Radiant

In Studio und in den Level-Dateien misst du Winkel in **Grad**: Eine ganze Runde hat 360°. Im
C++-Code rechnet Kern dagegen in **Radiant**. Radiant heißt: Wie weit wanderst du auf einem Kreis mit
1 Meter Radius? Eine ganze Runde ist der ganze Umfang. Der Umfang eines Kreises ist 2 · π · Radius,
wobei die Kreiszahl π ungefähr 3,14 ist. Bei 1 Meter Radius sind das etwa 6,28 Meter. Eine halbe
Runde (180°) ist etwa 3,14, eine Viertelrunde (90°) etwa 1,57. Für Mathe-Formeln ist Radiant
bequemer, für Menschen Grad. Darum rechnet Kern zwischen beiden um.

> **Mission:** Tipp in **Details** bei **Drehung** (Rotation) Z deines Würfels `45*2` und drück Enter.
> Rechenausdrücke sind erlaubt – der Würfel dreht sich um 90°.

### Drehungen in vier Zahlen

Jede Drehung im Raum lässt sich so beschreiben: **um diese Achse, um so viel Grad.**

> **So kannst du es dir vorstellen:** Steck einen Spieß durch einen Apfel. Der Spieß ist die Achse.
> Jetzt drehst du den Apfel um den Spieß – um 30°, um 90°, wie du willst. Jede Drehung, die es gibt,
> ist so ein Spieß plus ein Winkel.

Kern packt Achse und Winkel geschickt in **vier Zahlen**. Diese Verpackung heißt **Quaternion**. Die
Rechnung dahinter brauchst du nicht. Wichtig ist, warum Kern sie benutzt: Zwei Drehungen kann man
damit sauber zu einer einzigen zusammensetzen.

Bei zwei Drehungen hintereinander zählt die **Reihenfolge**. Probier es mit einem Buch auf dem Tisch:

- Erst um 90° um die Hochachse drehen, dann um 90° nach vorn kippen.
- Jetzt das Ganze andersherum: erst kippen, dann drehen.

Das Buch liegt am Ende verschieden da! Darum legt Kern genau fest, welche Drehung zuerst wirkt.

> **Für Profis:** Die vier Zahlen sind **nicht** einfach drei Zahlen für die Achse plus eine für den
> Winkel. Sie werden daraus mit Sinus und Kosinus umgerechnet (9. oder 10. Klasse). Darum sieht
> „keine Drehung“ ungewohnt aus: (0, 0, 0, 1). Die Rechnung steht im blauen Buch, Kapitel 3.

### Matrizen: alles in einem Rutsch

Ein Ding im Level wird oft **gestreckt**, **gedreht** und **verschoben**. Kern packt alle drei
Bewegungen in eine Tabelle aus 4 × 4 = 16 Zahlen, eine **Matrix**. Warum 4 und nicht 3, obwohl der
Raum nur drei Achsen hat? Fürs Strecken und Drehen würden 3 × 3 Zahlen reichen. Die zusätzliche
Spalte nimmt das Verschieben auf, damit alle drei Bewegungen in eine einzige Tabelle passen. Wie eine Matrix einen Punkt
verändert, erklärt Anhang [B](B-grundlagen-rendering.md) im Abschnitt „Matrizen ohne Angst“. Hier
genügt das Prinzip: Eine Matrix ist ein Rezept „erst strecken, dann drehen, dann verschieben“, das
man auf jeden Punkt anwenden kann.

Ein Beispiel aus Kerns Tests, das du im Heft nachzeichnen kannst:

1. Start: der Punkt (1, 0, 0).
2. Mal 2 gestreckt: (2, 0, 0).
3. Um 90° um die Z-Achse gedreht: Der Punkt wandert von der X-Achse auf die Y-Achse, also (0, 2, 0).
4. Um (1, 2, 3) verschoben: (0 + 1, 2 + 2, 0 + 3) = **(1, 4, 3)**.

Ändert man die Reihenfolge, kommt etwas anderes heraus, genau wie beim Buch. Darum legt Kern genau
fest, in welcher Reihenfolge zwei Matrizen wirken. Die Regel steht im blauen Buch, Kapitel 3.

Matrizen unterscheiden **Punkte** und **Richtungen**. Ein Punkt wird gestreckt, gedreht und
verschoben. Eine Richtung, etwa „nach Norden“, wird nur gestreckt und gedreht, nie verschoben. Denn
eine Richtung hat keinen Ort.

> **Für Profis:** Im Speicher liegen die 16 Zahlen nicht als Tabelle, sondern in einer langen
> Reihe, **Spalte für Spalte**. Weil Matrizen durch die Schließfachwand aus
> [Kapitel 1](01-einfuehrung.md) wandern, müssen C++ und JavaScript genau dieselbe Reihenfolge
> benutzen. Und um herauszufinden, was unter der Maus liegt, rechnet Kern eine Matrix „rückwärts“,
> mit doppelt so vielen Bits. Alles Weitere steht im blauen Buch, Kapitel 3.

## 3.8 Die Kamera

Eine Kamera ist in Kern keine Matrix, sondern eine **Beschreibung**:

- **Augpunkt** – wo die Kamera steht,
- **Ziel** – wohin sie schaut,
- **Oben** – welche Richtung im Bild oben ist (normalerweise +Z),
- **Öffnungswinkel** – wie weit sie von oben nach unten sieht, normalerweise 60°,
- **nahe und ferne Grenze** – was näher oder weiter weg ist, wird nicht gezeichnet.

> **So kannst du es dir vorstellen:** eine Taschenlampe im Dunkeln. Wo du stehst, wohin du leuchtest,
> wie breit der Lichtkegel ist und wie weit das Licht reicht – mehr braucht man nicht, um zu wissen,
> was du siehst.

Aus dieser Beschreibung rechnen Renderer, Anklicken und Editor ihre Matrizen selbst aus. Bevor Kern
eine Kamera benutzt, prüft es sie: Liegt die ferne Grenze hinter der nahen? Ist der Winkel größer als
0° und kleiner als 180°? Ungültige Kameras weist Kern mit „invalid camera“ zurück (3.2).

Neben der normalen Kamera mit Perspektive gibt es die **Draufsicht ohne Perspektive**: Dort wird
nichts kleiner, wenn es weiter weg ist, wie auf einer Landkarte. Statt eines Winkels sagt man hier,
wie viele Meter das Bild zeigt, so wie der Maßstab einer Landkarte.

> **Mission:** Drück **Alt+J** für die Draufsicht. Die kleine graue Schrift unten links im Viewport
> (dem 3D-Fenster) zeigt jetzt, wie viele Meter breit das Bild ist. Dreh am Mausrad und schau, wie
> sich die Zahl ändert. **Alt+G** schaltet zurück zur Perspektive.

**Was die Kamera nicht sieht, wird nicht gezeichnet.** Der Sichtbereich einer Kamera sieht aus wie
eine Pyramide, die vor der Kamera liegt und deren Spitze an der nahen Grenze abgeschnitten ist – wie
ein Lampenschirm, der mit der schmalen Seite zu dir zeigt. Ein Bild davon findest du in
[Kapitel 5](05-szene-und-geometrie.md), Abschnitt 5.2. Sie hat sechs Wände: links, rechts, oben, unten, nah, fern. Um jedes Ding legt Kern eine einfache
Hülle, eine Kugel oder eine Kiste. Liegt die Hülle ganz außerhalb der Wände, spart sich Kern das
Zeichnen ([Kapitel 5](05-szene-und-geometrie.md)). Ist eine Hülle ungültig, etwa weil sie noch leer
ist, weiß Kern nicht, wo das Ding liegt, und zeichnet es trotzdem. Im Zweifel lieber einmal zu viel zeichnen als ein Loch im Bild.

> **Für Profis:** Eine neue Kiste startet „umgedreht“: ihr kleinster Wert riesig groß, ihr größter
> riesig klein. Der erste Punkt, den man hinzufügt, legt sie dann sofort richtig fest – wie eine
> Bestenliste, die mit einem unschlagbar schlechten Ergebnis beginnt.

## 3.9 Mathe, die überall gleich rechnet

Erinnerst du dich an die Stille Post aus [Kapitel 1](01-einfuehrung.md)? Hier siehst du, wo die
kleinen Unterschiede herkommen und wie Kern sie verhindert. Es gibt drei Quellen.

### Quelle 1: Rechenfunktionen, die verschieden runden

Manche Rechnungen sind auf jedem Rechner bis aufs letzte Bit festgelegt, zum Beispiel Wurzelziehen
oder Abrunden. Andere nicht: etwa **Sinus** und **Kosinus**, mit denen man Drehungen ausrechnet (sie
kommen in Klasse 9 oder 10), und ein paar weitere Funktionen aus der Oberstufen-Mathe. Dort darf
jede Mathe-Bibliothek ein bisschen anders runden, und die von Linux, vom Mac, von Android und die im
Browser tun das auch – an der allerletzten Stelle. In einer Robotersimulation wird aus diesem einen
Bit binnen Sekunden ein anderer Gang.

**Kerns Lösung:** Kern bringt seine eigene Mathe-Bibliothek mit. Sie stammt aus einem freien Projekt
und wird auf jedem Rechner auf genau dieselbe Weise übersetzt. So rundet sie überall gleich. Die
Physik Box3D aus [Kapitel 1](01-einfuehrung.md) hat ihre eigenen, ebenso festen Winkelfunktionen.
Langsamer wird dadurch nichts.

### Quelle 2: zusammengezogene Rechenschritte

Manche Prozessoren können „mal und dann plus oder minus“ in **einem** Schritt rechnen und runden dabei nur einmal
statt zweimal. Das klingt harmlos, ändert aber das Ergebnis. Rechne selbst! Aufgabe: 0,15 · 3 − 0,04.
Wir runden wie in der Schule auf eine Nachkommastelle, der Computer macht es genauso, nur weit
hinten:

| Weg | Rechnung | Ergebnis |
|---|---|---|
| zwei Schritte | 0,15 · 3 = 0,45 → 0,5, dann 0,5 − 0,04 = 0,46 → 0,5 | **0,5** |
| ein Schritt | 0,15 · 3 − 0,04 = 0,41 → 0,4 | **0,4** |

Gleiche Aufgabe, andere Antwort. Darum verbietet Kern dem Compiler überall, Schritte zusammenzuziehen
([Kapitel 2](02-bauen-und-testen.md)).

### Quelle 3: die Reihenfolge beim Zusammenzählen

In der Schule lernst du: 1 + 2 + 3 = 3 + 2 + 1, die Reihenfolge ist egal. Beim Computer stimmt das
nicht ganz, weil nach jedem Schritt gerundet wird. Wieder auf eine Nachkommastelle:

| Reihenfolge | Rechnung | Ergebnis |
|---|---|---|
| 1 + 0,04 + 0,04 | 1 + 0,04 = 1,04 → 1,0, dann 1,0 + 0,04 = 1,04 → 1,0 | **1,0** |
| 0,04 + 0,04 + 1 | 0,04 + 0,04 = 0,08 → 0,1, dann 0,1 + 1 = 1,1 | **1,1** |

Rechnen mehrere Helfer gleichzeitig an einer Summe, hängt die Reihenfolge davon ab, wer zuerst fertig
ist. Darum zählt Kern Summen immer in einer festen Reihenfolge zusammen, egal wie viele Helfer
mitrechnen.

Dasselbe gilt für den Zufall beim Training: Jede Ente bekommt ihren eigenen festen **Startwert**,
aus dem ihr „Zufall“ ausgerechnet wird, wie bei den Hashes aus 3.4. Welcher Helfer sie gerade
rechnet, spielt keine Rolle ([Kapitel 14](14-lehrer.md)).

### Die Regeln

- Was die **Simulation füttert**, benutzt Kerns eigene Mathe-Bibliothek. Das gilt auch für das
  Packen einer Drehung in vier Zahlen aus 3.7, denn dafür braucht man Sinus und Kosinus.
- Summen werden immer in derselben Reihenfolge gebildet.
- Abkürzungen, die den Compiler schneller machen, aber je nach Rechner anders rechnen lassen, sind
  verboten.
- Die Physik verzichtet auf ihren Turbo-Weg mit Spezialbefehlen und rechnet überall auf demselben
  einfachen Weg wie im Browser ([Kapitel 2](02-bauen-und-testen.md)).

Was man nur **sieht oder hört**, etwa Ton, Partikel oder Teile des Editors, darf die gewöhnliche
Bibliothek des Rechners benutzen. Sobald solcher Code aber
Werte für die Physik, die Steuerung oder das Training liefert, muss er auf Kerns eigene Bibliothek
umsteigen.

> **Für Profis:** Die eigene Bibliothek stammt aus musl. Man ruft sie mit `kern::dm::` auf, etwa
> `kern::dm::sin` statt `std::sin`. Jeden Verstoß zeigt der Fingerabdruck aus
> [Kapitel 2](02-bauen-und-testen.md) bis auf den Regeltakt genau. Die Namen aller Funktionen und
> Compiler-Optionen stehen im blauen Buch, Kapitel 3.

## Level geschafft!

- Du weißt, wie viele Werte in 8 Bit passen und warum Kommazahlen gerundet werden.
- Du weißt, wie Kern Fehler meldet und warum „nicht größer als 0“ auch NaN fängt.
- Du weißt, warum die Uhrzeit nie in die Simulation darf.
- Du weißt, was JSON ist und warum Kern erst alles prüft und dann übernimmt.
- Du kennst Kerns Raum: Meter, **Z oben**, rechte Hand; Pfeile, Drehungen und Matrizen.
- Du weißt, was eine Kamera beschreibt und warum Unsichtbares nicht gezeichnet wird.
- Du weißt, warum Kern seine eigene Mathe mitbringt.

## Rätsel-Ecke

1. In welche Richtung zeigt in Kern „oben“?
2. x ist NaN. Warum bemerkt „Ist x kleiner oder gleich 0?“ den Fehler nicht, „Ist x nicht größer als
   0?“ aber schon?
3. Mach mit dem Punkt (1, 0, 0) aus 3.7 dieselben drei Schritte in umgekehrter Reihenfolge: erst
   um (1, 2, 3) verschieben, dann um 90° um die Z-Achse drehen, dann mal 2 strecken. Kommt wieder
   (1, 4, 3) heraus? Tipp: Die Drehung ist eine Vierteldrehung, von oben gesehen gegen den
   Uhrzeigersinn. Was nach rechts zeigte, zeigt danach nach vorn (so wurde im Beispiel (2, 0, 0) zu
   (0, 2, 0)), und was nach vorn zeigte, zeigt danach nach links.
4. Eine Servodatei hat fünf Werte, der vierte ist kaputt. Was passiert mit dem alten Profil?
5. **Boss-Rätsel:** Ein Steuerprogramm berechnet die Laufrichtung der Ente mit der gewöhnlichen
   Mathe-Bibliothek des Rechners. Der Fingerabdruck auf dem Mac weicht ab. Warum, und wie reparierst
   du es?

### Lösungen

1. Nach +Z.
2. Jeder Vergleich mit NaN ist falsch. „Kleiner oder gleich 0?“ ist also falsch, und der Fehler
   rutscht durch. „Größer als 0?“ ist ebenfalls falsch – und „nicht falsch“ ist wahr. So wird NaN
   gefangen.
3. Nein. Verschieben: (1 + 1, 0 + 2, 0 + 3) = (2, 2, 3). Drehen um die Z-Achse: Die Höhe 3 bleibt,
   aus „2 nach rechts, 2 nach vorn“ wird „2 nach links, 2 nach vorn“, also (−2, 2, 3). Strecken:
   (−4, 4, 6). Das ist ein ganz anderer Punkt als (1, 4, 3) – bei Matrizen zählt die Reihenfolge.
4. Nichts. Kern prüft erst alle Werte und übernimmt nur, wenn alle stimmen. Das alte Profil bleibt
   vollständig erhalten, und eine Meldung nennt den kaputten Wert.
5. Die Bibliothek des Rechners rundet auf dem Mac an der letzten Stelle anders als auf dem PC. Weil
   der Wert die Simulation füttert, schaukelt sich der Unterschied von Schritt zu Schritt auf.
   Lösung: Die Richtung mit Kerns eigener Mathe-Bibliothek berechnen, die überall gleich rundet.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
