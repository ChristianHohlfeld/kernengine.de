# Anhang B – Grundlagen: Wie aus Dreiecken Bilder werden

> **Kurz gesagt:** Dieser Anhang erklärt das Handwerk hinter jedem 3D-Spiel: wie ein Ding an die
> richtige Stelle des Bildschirms kommt, wie die Grafikkarte Dreiecke in Bildpunkte verwandelt und wie
> Licht, Schatten, Glanz und Farbe entstehen – 60-mal pro Sekunde im Browser. Bei jedem Grundbegriff
> erfährst du kurz, wie Kern ihn umsetzt.

> **Begriffe vorab:**
> - **Pixel** – ein einzelner Bildpunkt des Bildschirms; ein Bild besteht aus Millionen davon.
> - **Koordinaten** – drei Zahlen, die sagen, wo etwas ist: wie weit nach rechts, nach vorne, nach oben.
> - **Grafikkarte** (auch GPU) – der Spezialchip, der Millionen Bildpunkte gleichzeitig ausmalt.
> - **Mesh** – das Drahtgitter aus Dreiecken, aus dem ein 3D-Ding besteht.
> - **Shader** – ein kleines Malprogramm, das auf der Grafikkarte läuft.
> - **Textur** – ein Bild, das wie ein Aufkleber auf ein Mesh geklebt wird.

Lies diesen Anhang am besten **vor** [Kapitel 4](04-grafik.md): Dort geht es darum, wie Kern die
Bausteine zusammensetzt, hier um das „Warum“. Die genauen Namen und Werte stehen im blauen Buch,
Anhang B.

## B.1 Koordinatenräume

### Die Matrix: ein Rezept für jeden Punkt

Ein Ding in einem 3D-Spiel muss ständig bewegt werden: verschoben, gedreht, größer oder kleiner
gemacht. Dafür benutzt man eine **Matrix**. Stell sie dir als **Rezept** vor, das man auf jeden
einzelnen Punkt eines Dings anwendet: „Strecke ihn auf das Doppelte, drehe ihn um 90 Grad, schiebe ihn
5 m nach rechts.“ Man wirft einen Punkt hinein, und der neue Punkt kommt heraus. Wendet man dasselbe
Rezept auf alle Ecken eines Meshes an, bewegt sich das ganze Ding.

Aufgeschrieben ist so ein Rezept als kleine Tabelle aus 4 × 4 = 16 Zahlen. Wie man mit ihr rechnet,
musst du nicht können – wichtig sind drei Eigenschaften:

- **Ein Rezept für alles:** Strecken, Drehen und Verschieben passen in eine einzige Matrix.
- **Rezepte lassen sich zusammenfassen:** Zwei Rezepte hintereinander ergeben wieder ein Rezept. Das
  nennt man Matrizen **multiplizieren**. Die Grafikkarte muss dann nur noch ein Rezept anwenden statt
  vieler.
- **Die Reihenfolge zählt:** Erst drehen, dann verschieben ist etwas anderes als erst verschieben,
  dann drehen (dazu gleich die Falle).

### Die vierte Zahl w

Ein Punkt hat drei Koordinaten. Trotzdem hängt man ihm eine **vierte Zahl** an, w genannt. Warum?

Drehen und Strecken sind Rezepte der Art „jede neue Koordinate ist eine Mischung der alten“. Ein
Verschieben ist aber „plus 5 m“ – das ist keine Mischung, sondern eine feste Zugabe, und passt nicht
in so ein Rezept. Der Trick: Man gibt jedem Punkt eine vierte Zahl, die einfach **1** ist. Dann kann
das Rezept sagen: „Nimm 5 m pro Einheit von w dazu.“ Ein Punkt hat w = 1, bekommt also genau 5 m dazu.

Eine **Richtung** dagegen („nach Norden“, „senkrecht nach oben“) bekommt **w = 0**. Sie wird gedreht,
aber nie verschoben – eine Richtung hat keinen Ort. Beispiel mit dem Rezept „5 m nach rechts“:

| Was | vorher (mit w) | nachher |
|---|---|---|
| Punkt | 1, 2, 3 · w = 1 | **6**, 2, 3 |
| Richtung | 1, 2, 3 · w = 0 | 1, 2, 3 (unverändert) |

> **Stell dir vor …** … eine Hausnummer und die Angabe „Richtung Norden“. Zieht die ganze Stadt 5 km
> nach Osten um, hat dein Haus einen neuen Ort. „Norden“ bleibt Norden.

Später hilft w noch ein zweites Mal, bei der Perspektive (weiter unten).

**In Kern:** Es gibt zwei Rechenwege, einen für Punkte (w = 1) und einen für Richtungen (w = 0).

### Warum es mehrere Räume gibt

Der Rumpf der Micro Duck wird einmal in seiner **eigenen** Beschreibung festgelegt: „Der Kopf sitzt
ein Stück über der Mitte.“ Dann steht die Ente irgendwo in der Welt, eine Kamera schaut auf sie, und am Ende
landet sie auf Pixeln. Jeder Schritt ist ein anderes Koordinatensystem – ein anderer **Raum** –, und
für jeden Wechsel gibt es ein Rezept:

```mermaid
flowchart LR
  M["eigene Beschreibung"] -->|"Modell-Rezept"| W["Welt"]
  W -->|"Kamera-Rezept"| K["Blick der Kamera"]
  K -->|"Projektion"| S["Bildschirm"]
```

> **Stell dir vor …** … den Weg zu deinem Lieblingstisch im Café: „hinten links am Fenster“ (eigene
> Beschreibung des Cafés), „das Café liegt Hauptstraße 12“ (Welt), „von mir aus schräg rechts vorne“
> (Blick der Kamera), „auf meinem Handyfoto oben rechts“ (Bildschirm). Ein Tisch, vier Beschreibungen.

Weil alle Schritte Rezepte sind, fasst man sie zu einem einzigen zusammen. So rechnet die Grafikkarte
jede Ecke in einem Rutsch von der eigenen Beschreibung bis auf den Bildschirm.

### Kerns Gewohnheiten

Kern rechnet in **Metern**, und **Z zeigt nach oben** – x und y spannen den Boden auf
([Kapitel 3](03-grundlagen.md)). Die Lage eines Dings baut Kern immer in derselben Reihenfolge: erst
**strecken**, dann **drehen**, dann **verschieben**. Drehungen speichert Kern als **Quaternion**, vier
Zahlen statt drei Winkeln. Drei Winkel nacheinander können sich nämlich gegenseitig blockieren, sodass
plötzlich eine Drehrichtung fehlt; Quaternionen haben diese Tücke nicht.

> **Achtung, Falle!** Reihenfolge zählt! Stell dir einen Spielstein in der Mitte eines Tischs vor.
> Erst drehen, dann 5 m schieben: Der Stein dreht sich auf der Stelle und rutscht dann weg. Erst
> schieben, dann drehen: Die Drehung passiert um die *Tischmitte* – der Stein fährt einen Kreisbogen.
>
> ```
>  erst drehen, dann schieben     erst schieben, dann drehen
>
>     ■ ───────► ■                     ■ · · ·
>   (dreht sich hier)                ·       ·
>                                   ●         ■   ● = Tischmitte
> ```

> **Für Neugierige:** In der Formel schreibt man so ein zusammengesetztes Rezept „rückwärts“ als
> Verschieben · Drehen · Strecken. Das Rezept, das direkt neben dem Punkt steht, wirkt zuerst. Deshalb
> heißt es in Kapitel 3, der Punkt wird „von rechts“ an die Matrix multipliziert – gemeint ist dasselbe.

### Ansicht und Perspektive

Das **Kamera-Rezept** macht etwas Überraschendes: Es bewegt nicht die Kamera, sondern die **ganze
Welt** – so lange, bis die Kamera in der Mitte steht und geradeaus schaut. Wie beim Filmdreh im Auto:
Statt das Auto fahren zu lassen, zieht man die Kulisse am Fenster vorbei. Gebaut wird das Rezept aus
drei Angaben: wo die Kamera steht, wohin sie schaut und wo „oben“ ist.

Schaut die Kamera genau in Richtung „oben“ (oder genau entgegen), geht das nicht: Wer senkrecht
nach oben schaut, weiß nicht mehr, wo oben im Bild sein soll. Kern verweigert dann das Bild, statt Unsinn zu malen.

**Perspektive** heißt: Doppelt so weit weg erscheint halb so groß. Die Position auf dem Bild ist also
die Größe **geteilt durch die Entfernung**.

```
                                     ·┃   2 m hoch, 10 m weit
                            ·         ┃
                   ·┃                 ┃
          ·         ┃ 1 m, 5 m weit   ┃
  Auge ●════════════┻═════════════════┻════
  Ein Sehstrahl streift beide Spitzen: Im Bild sind sie gleich groß.
```

Ein 2 m hoher Baum in 10 m Entfernung erscheint also genauso groß wie ein 1 m hoher in 5 m. Nun kann
ein Rezept zwar mischen und zugeben, aber nicht teilen. Hier hilft wieder w: Das Rezept schreibt die
Entfernung in die vierte Zahl, und die Grafikkarte teilt danach alles durch w. Das heißt
**perspektivische Division**.

Für Editor-Ansichten gibt es auch die **Parallelansicht** ohne Perspektive: Dort wird Fernes nicht
kleiner, wie auf einem Bauplan.

**In Kern:** Die Kamera hat standardmäßig einen Öffnungswinkel von 60° (von der Unter- zur Oberkante
des Bildes) und sieht von 5 cm bis 1800 m vor sich.

Das blaue Buch rechnet einen Punkt 10 m vor der Kamera Schritt für Schritt bis auf seinen Bildpunkt
durch, samt der Zwischenstationen zwischen Kamera und Bildschirm.

> **Probier's aus!** Öffne in Kern Studio oben im Ansichtsfenster das Menü **≡** und schalte den
> **Field of View** (Öffnungswinkel) zwischen 50° und 110° um – bei 110° wirkt die Welt wie durchs
> Weitwinkelobjektiv, an den Rändern stark gestreckt. Mit **Alt+J** (Top) springst du in eine
> Parallelansicht von oben: Ferne Gebäude sind plötzlich so groß wie nahe.

### Tiefe ist ungleich verteilt

Für jeden Bildpunkt merkt sich die Grafikkarte, wie weit das Gemalte entfernt ist (der Tiefenpuffer,
B.2). Diese Tiefe speichert sie aber nicht wie ein normales Maßband. Nah an der Kamera sind die Striche
sehr eng, in der Ferne sehr weit.

> **Stell dir vor …** … ein Lineal, dessen Striche nah am Auge im Abstand von Haaren liegen und hinten
> im Abstand von Metern. Nah kannst du winzige Unterschiede messen, fern nur grobe.

Wie grob, zeigt diese kleine Tabelle mit Kerns Einstellungen:

| Entfernung | kleinster Abstand, den die Tiefe noch trennt |
|---|---|
| 10 m | etwa 0,1 mm |
| 1000 m | etwa 1,2 m |

Wie eng die Striche vorne liegen, hängt vor allem von der **nahen Ebene** ab: der Entfernung, ab der
die Kamera überhaupt etwas sieht (in Kern 5 cm). Alles davor wird abgeschnitten. Schiebt man die nahe
Ebene doppelt so weit hinaus, wird die Tiefe **überall doppelt so genau** – die wertvollen engen Striche
werden nicht mehr für den leeren Raum direkt vor der Linse verschwendet.

> **Wusstest du?** Daher flackern in manchen Spielen ferne, dicht beieinanderliegende Flächen
> („Z-Fighting“): Der Tiefenpuffer kann sie dort schlicht nicht mehr auseinanderhalten und malt mal die
> eine, mal die andere.

### Normalen

Eine **Normale** ist ein gedachter Pfeil, der senkrecht aus einer Fläche zeigt. Sie ist der „Blick“
der Fläche und bestimmt, wie hell die Fläche im Licht ist: Schaut sie zur Sonne, ist sie hell, schaut
sie weg, dunkel (B.4).

Staucht man ein Ding ungleichmäßig, darf man die Normalen nicht einfach mitstauchen – sonst stehen
sie schief. Ein Beispiel: Ein Dach ist 45° geneigt, seine Normale zeigt schräg nach oben. Staucht man
das Haus auf halbe Höhe, wird das Dach flacher – seine Normale müsste also *steiler* nach oben zeigen.
Staucht man den Pfeil aber einfach mit, kippt er *flacher*. Genau falsch herum!

Die Faustregel: Was die Fläche streckt, muss die Normale stauchen, und umgekehrt. Dafür braucht jede
Kopie eines Dings ein eigenes kleines Rezept nur für ihre Normalen.

**In Kern:** Dieses Normalen-Rezept rechnet Kern einmal pro Kopie aus; ein auf Größe null gestauchtes
Ding lehnt er ab, weil es dafür keins gibt.

## B.2 Vom Dreieck zu Bildpunkten

Nach der Projektion liegen die drei Ecken eines Dreiecks als Punkte auf dem Bildschirm. Nun schaut die
Grafikkarte nach, welche Pixel innerhalb des Dreiecks liegen. Das heißt **Rasterisierung**. Für jeden
solchen Pixel entsteht ein **Fragment**, ein Kandidat für einen Bildpunkt.

```
     v2
     /\
    /  \       ■ = Pixelmittelpunkt im Dreieck → Fragment
   / ■■ \
  / ■■■■ \
 /________\
v0        v1
```

Was an den Ecken steht – Farbe, Normale, Texturkoordinate –, wird dazwischen **gemischt**
(interpoliert). Jede Ecke bekommt einen Anteil, und die Anteile ergeben zusammen 100 %. Ein Fragment
genau in der Mitte bekommt von jeder Ecke ein Drittel; eines direkt an Ecke v0 fast alles von v0. So
entstehen weiche Farbverläufe über das Dreieck.

**Der Tiefenpuffer.** Treffen mehrere Dreiecke dasselbe Pixel, muss die Grafikkarte entscheiden, was
vorne ist. Dafür führt sie den **Tiefenpuffer**: ein unsichtbares zweites Bild, in dem für jeden Pixel
steht, wie weit das bisher Gemalte entfernt ist. Ein neues Fragment wird nur gemalt, wenn es näher
ist.

> **Stell dir vor …** … einen Maler, der neben jedem Pinselstrich notiert, wie weit das Gemalte weg
> war. Kommt ein neuer Strich für dieselbe Stelle, schaut er auf seinen Zettel: Liegt der neue weiter
> hinten, malt er ihn gar nicht erst.

Für Undurchsichtiges ist die Reihenfolge damit egal – für die Geschwindigkeit nicht. Kern malt
Undurchsichtiges **von vorne nach hinten**: Verdecktes fällt so früh durch, bevor der teure Shader
läuft. **Durchsichtiges** kommt danach **von hinten nach vorne** und wird mit dem Dahinterliegenden
gemischt; hier zählt die Reihenfolge, wie bei Folien übereinander. Der **Himmel** füllt nur die Lücken,
und die Dinge in deiner Hand (die Ego-Ebene) kommen zuletzt ([Kapitel 4](04-grafik.md), 4.7).

**Rückseiten.** Bei geschlossenen Körpern sind Dreiecke, die von der Kamera wegzeigen, immer verdeckt
– wie die Rückseite eines Würfels. Die Grafikkarte erkennt sie daran, ob ihre Ecken auf dem Bildschirm
im oder gegen den Uhrzeigersinn aufeinanderfolgen, und lässt sie weg (**Backface Culling**). Eine
Spiegelung dreht diesen Umlaufsinn um; deshalb behandelt Kern gespiegelte Kopien gesondert
([Kapitel 4](04-grafik.md), 4.4).

> **Probier's aus!** Schalte in Kern Studio mit **Alt+2** auf **Wireframe** (Drahtgitter) und sieh die
> Dreiecke, aus denen alles besteht. **Alt+3** zeigt **Unlit** (Farbe ohne Licht), **Alt+4** wieder
> **Lit** (mit Licht).

## B.3 Das Fließband der Grafikkarte

Die Grafikkarte arbeitet wie ein Fließband. Jeder Malauftrag – ein **Zeichenaufruf** (englisch *draw
call*), also „Male diese Form“ – läuft durch dieselben Stationen:

```mermaid
flowchart LR
  VB["Ecken-Daten"] --> VS["Vertex-Shader: jede Ecke"]
  VS --> R["Rasterisierung"]
  R --> FS["Fragment-Shader: jeder Bildpunkt"]
  T["Texturen"] --> FS
  FS --> D["Tiefentest, Mischen"]
  D --> FB["fertige Bilder"]
```

Zwei Stationen sind programmierbar: Der **Vertex-Shader** läuft einmal pro Ecke und sagt, wo sie auf
dem Bildschirm landet. Der **Fragment-Shader** läuft einmal pro Fragment und sagt, welche Farbe es
bekommt. Dabei arbeiten sie mit vier Arten von Daten:

| Fachbegriff | Alltagsbild | Beispiel |
|---|---|---|
| Attribut | Steckbrief jeder Ecke (oder Kopie) | Position, Normale |
| Uniform | Ansage für den ganzen Malauftrag | Sonnenrichtung, Materialfarbe |
| Varying | Zwischenwert, der übers Dreieck verläuft | Farbe zwischen den Ecken |
| Textur | aufgeklebtes Bild | Holzmaserung, Schattenkarte |

Drei Tricks machen das Fließband schnell:

- **Instancing (Kopien):** Hundert gleiche Bäume malt man mit **einem** Malauftrag. Die Form ist nur
  einmal da; jede Kopie bringt nur ihren Ort und Farbton mit. Bewegt sich etwas, ändern sich nur diese
  Angaben – die Form selbst bleibt auf der Grafikkarte.
- **Skinning (Haut über Knochen):** Bei Figuren folgt jede Ecke bis zu vier Knochen, jeder mit einem
  Anteil, zusammen 100 % – so biegt sich die Haut am Ellbogen weich.
- **Mehrere Bilder auf einmal:** Ein Fragment-Shader kann gleichzeitig in mehrere Bilder schreiben. So
  spart man sich einen zweiten Durchgang über alle Dreiecke.

**In Kern:** Jede Form wird als Kopien-Paket gemalt, und der eine Standard-Shader schreibt drei Bilder
auf einmal – das fertige Foto, das echte Licht fürs Glühen (B.7) und Notizen für die Eckenschatten
(B.8).

## B.4 Beleuchtung

### Was einen Bildpunkt hell macht

Das Licht, das von einer Fläche in dein Auge kommt, ist ihr **eigenes Leuchten** plus alles Licht, das
auf sie fällt und das sie **in deine Richtung zurückwirft**. Wie eine Oberfläche das tut, ist ihr
„Charakterprofil“: Ein Spiegel wirft alles in eine Richtung, eine Wand verteilt es gleichmäßig.

Alles Licht der Welt aufzusummieren, auch das, was von Wänden zurückprallt, wäre viel zu teuer. Spiele
teilen deshalb auf: **direktes Licht** (Sonne, Lampen) wird genau gerechnet, **indirektes Licht** aus
der Umgebung nur grob geschätzt.

### Matte Flächen

Eine matte Fläche, etwa Papier oder ein Acker, verteilt Licht gleichmäßig in alle Richtungen. Wie hell
sie ist, hängt nur davon ab, **wie steil das Licht auftrifft**.

> **Stell dir vor …** … eine Taschenlampe senkrecht auf den Tisch: ein kleiner, heller Kreis. Kippst
> du sie, wird der Fleck zum langen, blassen Oval – gleiches Licht, mehr Fläche.

| Licht fällt … | Anteil der vollen Helligkeit |
|---|---|
| senkrecht ein | 100 % |
| um 45° gekippt | gut 70 % |
| um 60° gekippt | 50 % |
| streifend (90°) | 0 % |
| von hinten | 0 % |

Dazu kommt die **Albedo**: wie viel Licht eine Fläche überhaupt zurückwirft – Schnee sehr viel,
Asphalt wenig. Eine graue Fläche mit halber Albedo ist also halb so hell wie eine weiße im selben
Licht. Diese Regel trägt den Namen ihres Entdeckers **Lambert**. Metalle haben übrigens keinen matten
Anteil; ihr Licht steckt ganz im Glanz.

**Lampen** werden in Wirklichkeit mit der Entfernung schwächer, aber nie ganz null – jede Lampe müsste
also die ganze Welt beleuchten. Das wäre viel zu teuer.

**In Kern:** Jede Lampe hat eine Reichweite, an der ihr Licht weich auf null ausläuft, und es leuchten
höchstens sechs Lampen gleichzeitig.

### Glanz

Glänzende Flächen werfen Licht gerichtet zurück, daher der helle Fleck auf einem Apfel. Die heutige
Erklärung: Eine Fläche besteht aus Millionen winziger, verschieden schief stehender **Spiegelchen**.

```
   glatt: ‾‾‾‾‾‾‾‾‾‾  alle gleich gerichtet → kleiner, scharfer Punkt
   rau:   /\/‾\_/\/‾  kreuz und quer        → breiter, matter Schimmer
```

Drei Fragen bestimmen den Glanz:

1. **Wie viele Spiegelchen zeigen genau so, dass sie Licht in mein Auge werfen?** Bei glatten Flächen
   wenige Richtungen, dafür sehr hell; bei rauen viele, dafür schwach.
2. **Wie stark spiegelt die Fläche bei diesem Blickwinkel?** Bei flachem Blick viel mehr – wie ein
   See, der schräg betrachtet den Himmel spiegelt, von oben aber durchsichtig wirkt. Das heißt
   **Fresnel-Effekt**. Senkrecht spiegeln Nichtmetalle nur wenig, Metalle dagegen stark und in ihrer
   eigenen Farbe.
3. **Verdecken sich die Spiegelchen gegenseitig?** Bei rauen Flächen werfen sie sich kleine Schatten.

**Indirektes Licht** schätzt man mit wenigen Farben: oben Himmelsfarbe, unten Bodenfarbe, dazwischen
gemischt nach der Neigung der Fläche, dazu eine bunte Kuppel aus Himmel, Horizont und Boden.

**In Kern:** Zwei Regler, **Rauheit** und **Metall**, steuern ein vereinfachtes Glanzmodell mit
Sonnen-Glanzpunkt und Himmelsspiegelung, und das indirekte Licht läuft getrennt mit, damit die
Eckenschatten (B.8) nur dieses abdunkeln.

> **Für Neugierige:** Die Fachnamen der drei Fragen lauten Verteilung (meist nach „GGX“), Fresnel
> (meist nach „Schlick“) und Geometrie. Kern beantwortet die dritte Frage mit der einfachsten Annahme
> und begrenzt die erste nach oben, damit keine einzelnen Riesen-Glanzpunkte flackern.

## B.5 Schattenkarten

Ein Punkt liegt im Schatten, wenn zwischen ihm und der Sonne etwas steht. Also malt man die Szene
einmal **aus Sicht der Sonne** und notiert für jeden Punkt dieser Karte nur, wie weit das erste
getroffene Ding entfernt ist – die **Schattenkarte**. Beim normalen Malen fragt jede Stelle: „Bin ich
weiter weg als das Notierte?“ Wenn ja, steht etwas davor.

```
   Sonne ═══╗  (parallele Strahlen)
            ║  ↓   ↓   ↓   ↓
            ║     ███          notierte Entfernung: Dach
            ║  ↓  ███  ↓   ↓
            ║  ── ─── ─────    Boden dahinter → Schatten
```

Das klingt einfach, hat aber Tücken:

| Problem | Ursache | Gegenmittel |
|---|---|---|
| feine Streifen („Schattenakne“) | Fläche beschattet sich selbst | kleiner Sicherheitsabstand |
| Schatten löst sich vom Ding | Sicherheitsabstand zu groß | Abstand klein halten |
| Treppchenkanten | ein Kartenpunkt deckt viele Pixel | mehrere Nachbarn mischen |
| Flimmern | Karte rutscht um Bruchteile | nur um ganze Punkte schieben |
| unscharf in der Nähe | Karte deckt zu viel ab | nur das Sichtbare abdecken |

Der **Sicherheitsabstand** ist das wichtigste Gegenmittel: Die Fläche gilt erst als beschattet, wenn
sie ein klein wenig weiter weg ist als das Notierte. Sonst würde sie sich wegen der groben Karte selbst
verdunkeln – wie die Streifen auf einem Foto vom Bildschirm. Ist der Abstand zu groß, schwebt der
Schatten dagegen ein Stück neben dem Ding; Fachleute nennen das nach der Märchenfigur „Peter Panning“,
weil Peter Pan seinen Schatten verliert.

**In Kern:** Die Karte deckt nur ab, was man bis zur Schattendistanz sieht, rastet auf ganze Punkte ein
und wird nur neu gemalt, wenn sich etwas ändert – warum das so wichtig ist und wie es zusammenspielt,
steht in [Kapitel 4](04-grafik.md), 4.8.1.

## B.6 Helles Licht, Bildschirm und Farbe

### Tonemapping

Echtes Licht hat einen riesigen Bereich, von fast dunkel bis gleißend hell. Ein Bildschirm kann
dagegen nur von Schwarz bis Weiß. Man rechnet deshalb mit dem **echten** Licht, auch
wenn es viel heller als Weiß ist (Fachwort **HDR**, *high dynamic range*), und presst es erst ganz am
Ende auf den Bildschirmbereich. Dieses Zusammenpressen heißt **Tonemapping**. Eine gute Kurve lässt
mittlere Helligkeiten fast, wie sie sind, und rundet sehr Helles weich ab:

```
 auf dem
 Bildschirm
  Weiß |            ____________
       |        _--
       |      /
       |    /
       |  /
Schwarz|/________________________ echtes Licht
       0    1    2    3    4
```

**In Kern:** Die Kurve ist eine bewährte Filmkurve aus der Kinotechnik, wie sie auch die
Grafikbibliothek three.js benutzt; danach kann noch ein Farbgrading folgen.

> **Probier's aus!** Dreh im Tab **World Settings** die **Exposure** (Belichtung) hoch und runter.
> Helle Stellen laufen weich ins Weiß, statt hart abzuschneiden – die Filmkurve bei der Arbeit.

### Gamma und sRGB

Bildschirme und Bilddateien speichern Helligkeit nicht so, wie Licht sich verhält, sondern so, wie
unser Auge es empfindet: Dunkle Töne bekommen viel mehr der 256 Helligkeitsstufen, weil wir dort
feinere Unterschiede sehen. Diese Schreibweise heißt **sRGB**. Die Schreibweise, die echtes Licht
abbildet, heißt **linear**: Doppelt so viel Licht ist dort doppelt so viel Zahl.

| echtes Licht (linear) | als sRGB-Wert gespeichert |
|---|---|
| 5 % | etwa 25 % |
| 18 % | etwa 46 % |
| 50 % | etwa 74 % |
| 100 % | 100 % |

Die zweite Zeile ist berühmt: 18 % des Lichts empfindet das Auge als **mittleres Grau**, und in sRGB
steht es tatsächlich fast in der Mitte der Skala.

Das Problem: Licht addiert sich **linear**. Zwei Lampen machen doppelt so viel Licht – aber der
sRGB-Wert verdoppelt sich nicht. Wer mit sRGB-Zahlen rechnet, rechnet falsch, ungefähr so, als würde
man Schuhgrößen addieren. Darum gilt: Farben **erst nach linear** umrechnen, linear rechnen, ganz am
Ende **zurück nach sRGB**.

> **Achtung, Falle!** Wer in sRGB rechnet, bekommt zu dunkle Übergänge und falsche Mischfarben. Und
> wer Farben schon linear liefert, muss das Kern sagen – sonst rechnet Kern sie ein zweites Mal um.

### Nebel

Nebel wird mit der Entfernung dichter. Kern mischt ihn so ein, dass die Nähe lange klar bleibt und der
Dunst dann rasch zuzieht. Mit Kerns Standardnebel:

| Entfernung | Anteil Nebel |
|---|---|
| 100 m | kaum, unter 1 % |
| 500 m | etwa 17 % |
| 1000 m | etwa die Hälfte |
| 2000 m | fast alles |

**In Kern:** Auf Wunsch wird der Nebel nach oben dünner und leuchtet in Richtung Sonne in
Sonnenfarbe.

## B.7 Bloom: das Glühen

**Bloom** ist der Lichthof um sehr helle Stellen, wie Gegenlicht im Auge oder in einer Kamera. Das
Rezept: das sehr Helle ausschneiden, unscharf machen und wieder darüberlegen. Das Unscharfmachen
geschieht gern auf verkleinerten Bildern – das ist billiger und gibt einen breiteren Schein.

Unscharf machen heißt: Jeder Bildpunkt wird mit seinen Nachbarn gemischt. Mischt man jeden Punkt mit
allen Nachbarn in einem Quadrat von 9 × 9, sind das 81 Lesezugriffe pro Punkt. Viel schneller geht es
in zwei Durchgängen.

> **Stell dir vor …** … du willst ein Kästchenbild verwischen. Statt jedes Kästchen mit allen 80
> Nachbarn im Quadrat zu mischen, verwischst du erst jede Zeile nach links und rechts und dann jede
> Spalte nach oben und unten. Das Ergebnis ist dasselbe – aber 9 + 9 = 18 statt 81 Zugriffe.

Und noch ein Trick: Liest man genau **zwischen** zwei Bildpunkten, mischt die Grafikkarte sie gratis.
So reichen pro Durchgang fünf statt neun Zugriffe. Die Mischanteile ergeben zusammen genau 100 %,
damit das Bild weder heller noch dunkler wird.

**In Kern:** Das Glühen entsteht aus dem echten Licht des Shaders, wird auf zwei verkleinerten Stufen
verwischt und als feine Folie über das fertige Foto gelegt ([Kapitel 4](04-grafik.md), 4.8.3).

## B.8 Eckenschatten: Umgebungsverdeckung

In Ecken, Fugen und unter Dingen kommt weniger Umgebungslicht an, weil ein Teil des Himmels verdeckt
ist. Diese **Umgebungsverdeckung** (englisch *ambient occlusion*, kurz AO) lässt Dinge auf dem Boden
„stehen“ statt schweben. Sie fragt: *Wie viel Himmel über diesem Punkt ist frei?* Dabei zählt Himmel
direkt über der Fläche mehr als Himmel knapp über dem Horizont – aus demselben Grund wie bei der
Taschenlampe: Steil einfallendes Licht trifft eine Fläche kräftiger als flaches. Die Sonne hat ihre
eigenen Schatten; AO betrifft nur das indirekte Licht.

> **Stell dir vor …** … du liegst in einer Mulde im Gras und schaust nach oben. Je tiefer die Mulde,
> desto kleiner dein Stück Himmel – und desto dunkler ist es bei dir, auch ohne Sonnenschatten.

Strahlen durch die ganze Szene zu schicken, wäre zu teuer. Darum schätzt man die Verdeckung aus dem
fertigen Tiefenpuffer, also aus dem, was ohnehin schon auf dem Bildschirm ist. Ältere Verfahren
prüfen dabei ein paar Stichproben in der Nachbarschaft – eine grobe Schätzung. Das genauere Verfahren
**GTAO** geht systematisch vor: Es schaut von jedem Punkt in ein paar Richtungen, sucht links und
rechts, wie hoch die Umgebung den Himmel verdeckt (den **Horizont**), und rechnet aus, wie viel Himmel
dazwischen frei bleibt.

```
             Horizont links
   Blick ↓      ╲   │   ╱  Horizont rechts
                 ╲  │   ╱
   ████████       ╲ │  ╱      ████
   ████████████────●────────████████
                  Punkt
   freier Himmel: der Bogen zwischen den beiden Horizonten
```

**In Kern:** Kern nutzt GTAO, dunkelt damit nur das indirekte Licht ab, das der Shader nebenbei notiert
hat, und braucht dafür keinen zweiten Durchgang über alle Dreiecke ([Kapitel 4](04-grafik.md), 4.8.4).

## B.9 Himmel und Atmosphäre

Der Himmel ist blau, weil die Luft blaues Licht viel stärker in alle Richtungen streut als rotes. Je
kürzer die Wellenlänge, desto stärker die Streuung, und zwar sehr steil: Blau wird gut viermal so stark
gestreut wie Rot. Das heißt **Rayleigh-Streuung**. Am Horizont ist der Weg durch die Luft länger; das
Licht wird mehrfach gestreut, der Himmel dort heller, und bei tiefer Sonne wird er rötlich. Um die
Sonne herum streuen größere Teilchen das Licht nach vorne und machen den hellen Hof
(**Mie-Streuung**).

Echte Himmelsmodelle verfolgen das Licht Schritt für Schritt entlang jedes Blicks durch die Luft.
**In Kern:** Der Himmel ist bewusst keine solche Lichtrechnung, sondern ein billiges Rezept aus
Farbverlauf, goldener Stunde, Sonne, Mond, Sternen und Wolken, das ähnlich aussieht und nur die Lücken
füllt ([Kapitel 4](04-grafik.md), 4.8.2).

## B.10 Unsichtbares weglassen

Was außerhalb des Bildes liegt, muss man nicht malen. Der Bereich, den eine Kamera sieht, ist eine
liegende Pyramide mit abgeschnittener Spitze, die **Sichtpyramide** (englisch *frustum*). Um zu prüfen,
ob ein Ding darin liegt, steckt man es in eine gedachte **Hüllkugel** und fragt: Liegt die Kugel ganz
hinter einer der Wände der Pyramide? Dann ist das Ding unsichtbar.

```
       \         /
        \  ●    /     ● sichtbar
   ○     \     /      ○ verworfen: ganz hinter der linken Wand
          \   /
           \ /
          Kamera
```

Der Test ist **vorsichtig**: Er wirft nie Sichtbares weg, lässt aber manchmal Unsichtbares durch, etwa
Kugeln nahe einer Ecke der Pyramide. Im Zweifel wird eben gemalt.

**In Kern:** So prüft Kern jedes Ding vor dem Malen – außer Figuren mit Skelett, deren Arme und Beine
aus der Kugel ragen können.

> **Probier's aus!** Schalte in Kern Studio im Menü **Show** die **Bounds** ein und sieh die Hüllen,
> mit denen Kern entscheidet, was in der Sichtpyramide liegt – auch um Bäume, Gebäude und die Ente.

Bei vielen Dingen hilft ein **Baum aus Kästen** (englisch *BVH*): Ganz oben ein Kasten um alles, darin
zwei kleinere Kästen, darin wieder zwei, bis unten einzelne Dreiecke liegen. Trifft ein Strahl einen
Kasten nicht, fällt alles darin auf einen Schlag weg – wie beim Zahlenraten mit „größer oder
kleiner?“, wo zehn Fragen statt tausend Versuchen reichen.

```
            [ alle ]
           /        \
     [ links ]    [ rechts ]
      /    \        /    \
   [△△]  [△△]   [△△]  [△△]   unten: Dreiecke
```

**In Kern:** Der Baum aus Kästen dient dem **Anklicken** – welches Dreieck liegt unter der Maus? –,
nicht dem Malen; die Physik hat ihren eigenen ([Anhang C](C-grundlagen-physik.md)).

## B.11 Wenige große Aufträge

Jeder **Zeichenaufruf** kostet Zeit, bevor die Grafikkarte überhaupt anfängt: Einstellungen prüfen,
Daten bereitlegen, Werte schicken – im Browser zusätzlich der Weg über die Brücke zur Webseite samt
Prüfung. Tausende kleine Aufrufe können länger dauern als die eigentliche Malarbeit. Auch jedes
Umschalten, etwa auf eine andere Textur, kostet.

> **Stell dir vor …** … du gehst einkaufen. Hundertmal für je eine Zutat in den Laden zu laufen dauert
> viel länger als einmal mit vollem Korb – obwohl am Ende dasselbe im Kühlschrank steht.

**In Kern:** Ein einziges Malprogramm, Kopien statt Duplikate, ein Merkzettel gegen doppelte Werte,
Schattenkarte nur bei Bedarf und Formen nur einmal hochladen – und eine Statistik zeigt, wie viele
Aufträge ein Bild gekostet hat ([Kapitel 4](04-grafik.md), 4.7 und 4.12).

## B.12 Warum Kern und Browser so zusammenarbeiten

Kern ist in C++ geschrieben, einer schnellen Programmiersprache. Der Browser versteht aber von Haus aus
nur JavaScript. Die Brücke heißt **WebAssembly**: ein kompaktes Format, das der Browser fast so schnell
wie ein normales Programm ausführt. Ein WebAssembly-Programm darf allerdings nur Zahlen austauschen und
nur das benutzen, was ihm die Webseite beim Start ausdrücklich in die Hand gibt. Die Grafikkarte
erreicht es über **WebGL2**, die Malschnittstelle des Browsers.

```mermaid
flowchart LR
  R["Kern: Malen, Physik, Spiel"] --> G["kurze Liste Malbefehle"]
  G -->|"Nummern, Adresse + Länge"| K["dünne Webseiten-Schicht"]
  K --> W["Grafikkarte über WebGL2"]
```

Daraus folgen Kerns wichtigste Entscheidungen, in Alltagssprache:

| Entscheidung | Warum |
|---|---|
| alles in einer Sprache | dann rechnet es überall gleich – im Browser, in Tests, im Lehrer |
| Webseiten-Schicht hauchdünn | nichts doppelt, nichts, was anders rechnen könnte |
| kurze, feste Liste der Malbefehle | alles Gebrauchte steht an einer Stelle |
| Nummern statt Dingen | über die Brücke passen nur Zahlen (Garderobenmarken) |
| Kopien von allem | Versicherung, falls der Browser die Grafikkarte wegnimmt |
| wenige Aufträge, Merkzettel | jeder Weg über die Brücke kostet |
| Malen ohne Bildschirm möglich | der Lehrer rechnet dieselbe Welt ohne Grafik |

Die genauen Namen der Bauteile stehen im blauen Buch, Anhang B.

## Das nimmst du mit

- Eine **Matrix** ist ein Rezept, das jeden Punkt streckt, dreht und verschiebt; Rezepte lassen sich
  zu einem zusammenfassen, und die Reihenfolge zählt.
- Die **vierte Zahl w** ist 1 für Orte und 0 für Richtungen – die Hausnummer zieht um, „Norden“ nicht.
- Ein Ding wandert wie der Café-Tisch durch vier Beschreibungen bis auf den Bildschirm.
- **Doppelt so weit weg, halb so groß**; die Grafikkarte teilt dafür durch w.
- Der Tiefenpuffer misst nah sehr fein, fern grob; die **nahe Ebene** entscheidet.
- Vertex-Shader rechnen Ecken, Fragment-Shader Bildpunkte.
- Licht = matter Anteil (Taschenlampe) + Glanz (Spiegelchen, See) + grobes Umgebungslicht.
- Schattenkarten sind ein Foto aus Sicht der Sonne, mit kleinem Sicherheitsabstand.
- Gerechnet wird mit **echtem, linearem Licht**; eine **Filmkurve** presst es auf den Bildschirm.
- Wenige große Aufträge machen ein Spiel schnell – einmal einkaufen mit vollem Korb.

## Knobelecke

1. Warum bekommt eine Richtung w = 0 und ein Ort w = 1?
2. Ein Ding ist doppelt so weit weg wie ein gleich großes anderes. Wie groß erscheint es?
3. In der Ferne flackern zwei dicht beieinanderliegende Flächen. Was hilft am meisten?
4. Warum darf man Farben nicht einfach als sRGB-Zahlen addieren?
5. Warum malt Kern den Himmel *nach* den undurchsichtigen Dingen?

### Lösungen

1. Mit w = 0 wird nichts verschoben – eine Richtung hat keinen Ort. Die Hausnummer zieht um, „Norden“
   bleibt Norden.
2. Halb so groß: Die Grafikkarte teilt durch die Entfernung.
3. Die nahe Ebene weiter hinausschieben: Doppelt so weit heißt überall doppelt so genau, denn die
   feinen Striche werden nicht mehr für den leeren Raum direkt vor der Kamera verbraucht.
4. Licht addiert sich linear, sRGB-Zahlen aber nicht – wie Schuhgrößen. Mischungen und Verläufe werden
   dann falsch, meist zu dunkel.
5. Dann kommt der Himmel nur an Stellen durch, auf denen noch nichts liegt, und der teure Himmels-Shader
   läuft nur dort, wo wirklich Himmel ist.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
