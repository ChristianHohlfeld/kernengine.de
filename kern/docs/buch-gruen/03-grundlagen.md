# 3 Grundlagen: das Fundament unter allem

> **Kurz gesagt:** Jedes Stockwerk von Kern steht auf vier Bausteinen: einem **Werkzeugkasten**
> (Zahlen, Fehlermeldungen, Uhr, Helfer), einem **Daten-Leser** für das Format JSON, der
> **Raum-Mathe** für die 3D-Welt und dem **Rechnen ohne Rundungsunterschiede**. Hier erfährst du,
> was jeder Baustein tut und warum er so gebaut ist.

> **Begriffe vorab:**
> - **Funktion** – ein benanntes Programmstück, das eine Aufgabe erledigt und eine Antwort gibt,
>   etwa „Ist diese Datei in Ordnung? – Ja.“
> - **Daten** – alles, was ein Programm verarbeitet: Zahlen, Texte, Listen.
> - **Kommazahl** – im Computer eine Zahl mit fester Stellenzahl; was nicht hineinpasst, wird
>   gerundet ([Kapitel 1](01-einfuehrung.md)).
> - **Koordinaten** – drei Zahlen, die sagen, wo etwas steht: wie weit rechts, vorn und oben.
> - **Achse** – eine gedachte Linie durch den Raum, um die man etwas drehen kann.
> - **Renderer** – der Teil von Kern, der die Bilder malt.
> - **Texteditor** – ein schlichtes Schreibprogramm, unter Windows „Editor“, auf dem Mac „TextEdit“.

Ist der Keller schief, ist es das ganze Haus. Die genauen Namen und Werte zu diesem Kapitel stehen
im blauen Buch, Kapitel 3.

## 3.1 Der Werkzeugkasten

### Zahlen mit fester Stellenzahl

Eine Zahl belegt im Computer eine feste Anzahl von Bit-Kästchen. Wie viele, kann je nach Rechner
verschieden sein, wenn man es nicht festlegt. Kern legt es **immer** fest: 8, 16, 32 oder 64
Kästchen, mit oder ohne Minuszeichen, ganze Zahl oder Kommazahl.

> **Stell dir vor …** Ein Kilometerzähler mit sechs Stellen springt nach 999 999 auf 000 000, einer
> mit sieben Stellen erst später. Wer zwei Autos vergleicht, muss wissen, wie viele Stellen jeder
> Zähler hat.

Ein Zähler aus 8 Kästchen schafft 0 bis 255 und beginnt dann wieder bei 0. Weil Kern die Größe immer
festlegt, rechnet jeder Rechner mit gleich großen Zählern – und was über den gemeinsamen Speicher
zum Browser geht, ist auf beiden Seiten gleich groß.

### Wenn etwas schiefgeht: Nein sagen und einen Zettel hinterlassen

Viele Programmiersprachen haben für Fehler einen **Notfallknopf** (Fachwort: Ausnahme): Das Programm
springt mitten aus der Arbeit heraus, bis irgendwo jemand den Notfall auffängt. Kern verzichtet
darauf. Eine Funktion, die scheitern kann, antwortet schlicht **„Nein“** und hängt vorher einen
Zettel mit dem Grund an eine Pinnwand. Wer sie aufgerufen hat, entscheidet selbst, was nun geschieht.
Nichts reißt mitten in der Arbeit ab.

Bekommt der Renderer zum Beispiel ein 3D-Modell mit kaputten Zahlen, antwortet er „Nein“, pinnt den
Grund an, und das alte Modell bleibt unverändert stehen.

> **Stell dir vor …** Ein Hotel hat genau **eine** Pinnwand. Wer etwas meldet, hängt einen Zettel
> hin und nimmt dabei den alten ab. Wer nachsieht, liest immer nur den neuesten.

So funktioniert Kerns Fehlerkanal: eine Pinnwand für das ganze Programm, die auch die Browser-Seite
lesen und abräumen kann. Nur einige Bausteine, etwa der JSON-Leser, geben ihren Zettel direkt dem
zurück, der sie gerufen hat.

> **Achtung, Falle!** Die Pinnwand zeigt den **neuesten** Zettel – nicht unbedingt den der Aufgabe,
> nach der du fragst. Wer sicher gehen will, räumt die Pinnwand vorher ab.

### Die Uhr gehört nicht in die Simulation

Kern hat eine Uhr, die Millisekunden zählt. Sie dient **Messungen**: Wie lange hat etwas gedauert,
ist das Zeitbudget aufgebraucht? So darf die Sound-Bank beim Start nur eine begrenzte Zeit lang
Klänge vorrechnen.

In der **Simulation** hat diese Uhr nichts verloren. Physik und Roboter zählen **feste Schritte**:
Ein Takt ist immer gleich lang, egal wie schnell der Rechner ihn ausrechnet. Schaute die Ente auf die
Uhr, liefe sie auf einem langsamen Rechner anders als auf einem schnellen
([Kapitel 6](06-physik.md), [Kapitel 13](13-micro-duck.md)).

> **Stell dir vor …** Ein Brettspiel. Man zieht Runde um Runde, nicht nach der Stoppuhr – sonst
> gewinnt, wer schneller würfelt, statt wer besser spielt. Kerns Simulation zählt Runden.

Außerdem liegen im Werkzeugkasten ein **Protokoll**, das Meldungen von „nur zur Fehlersuche“ bis
„Fehler“ sammelt, und kleine Helfer, die etwa einen Wert zwischen zwei Grenzen halten. Kann zum
Beispiel der Glüheffekt nicht starten, protokolliert Kern eine Warnung, und das Spiel läuft ohne
Glühen weiter.

### Ein Fenster auf Daten

> **Stell dir vor …** Ein Zettel: „Regal 4, die nächsten 20 Bücher.“ Er ist leicht, denn man muss
> die Bücher nicht herumtragen. Aber er **besitzt** sie nicht. Räumt jemand das Regal aus, zeigt der
> Zettel ins Leere.

So ein Zettel heißt in Kern **Span** (Spanne): nur zwei Angaben, wo die Daten anfangen und wie viele
es sind. In dieser Form fließen fast alle Daten in die Engine, ohne dass etwas kopiert werden muss.
Daraus folgt eine Regel: Wer die Daten **behalten** will, kopiert sie. Der Renderer etwa behält
Kopien seiner Modelle, um später zu erkennen, worauf du klickst, und um nach dem Verlust der
Grafikkarte alles neu aufzubauen ([Kapitel 4](04-grafik.md)).

### Fingerabdrücke für Daten: Hashes

Ein **Hash** macht aus beliebig vielen Daten eine kurze Zahl. Dieselben Daten ergeben immer
dieselbe Zahl; ändert sich nur ein Buchstabe, kommt eine völlig andere heraus.

> **Stell dir vor …** Eine Quersumme, nur viel gründlicher: Bei der Quersumme ergeben 12 und 21
> dasselbe, ein guter Hash bemerkt die Vertauschung.

Damit erkennt Kern schnell, ob sich etwas geändert hat: Der Renderer schickt einen Wert nur neu an
die Grafikkarte, wenn sein Hash anders ist. Auch der Fingerabdruck der Ente aus
[Kapitel 2](02-bauen-und-testen.md) besteht aus Hashes. Ein zweiter Hash mischt Zahlen
durch, für zufällig aussehende Muster, die trotzdem jedes Mal gleich ausfallen.
Hashes rechnen nur mit ganzen Zahlen, ohne
Rundung – das Ergebnis hängt also nur von den Daten ab, nie vom Rechner.

> **Für Neugierige:** Kern prüft gern „ist **nicht** größer als null“ statt „ist kleiner oder
> gleich null“. Grund ist der Sonderwert **NaN** („not a number“), der etwa bei 0 durch 0 entsteht:
> Jeder Vergleich mit NaN ist falsch, „nicht größer als null“ also wahr – und der Wert fliegt raus.

## 3.2 JSON: Kerns Lieblingsformat

**JSON** ist ein Textformat für Daten, das Menschen und Programme lesen können, etwa
`{"name": "Ente", "beine": 2}`: Jeder Eintrag hat einen Namen und einen Wert. Kern speichert darin
Levels, Befehle zwischen Studio-Seite und Engine, die Motorbeschreibungen der Ente, Übungsdaten des
Lehrers und die Liste der Entennetze. Sein JSON-Baustein gibt drei Versprechen.

**1. Die Reihenfolge bleibt.** Liest Kern eine Datei ein und schreibt sie wieder hinaus, steht alles
an seinem Platz. Ändert sich im Level ein Wert, ändert sich in der Datei nur diese Stelle – so lassen
sich zwei Fassungen bequem vergleichen.

**2. Zahlen sehen aus wie im Browser.** Kern schreibt jede Zahl mit genau so vielen Ziffern, wie
nötig sind, um sie beim Wiedereinlesen exakt zurückzubekommen – nach derselben Regel wie JavaScript.
Eine Datei sieht darum gleich aus, ob Kern oder der Browser sie geschrieben hat.

> **Wusstest du?** Rechnet man im Browser 0,1 + 0,2, kommt **0,30000000000000004** heraus, und Kern
> schreibt genau dasselbe. Das ist Rundung: Computer speichern Kommazahlen im **Zweiersystem**, und
> dort ist 0,1 ein unendlicher Bruch – so wie 1/3 bei uns 0,3333… ist und nie aufgeht. Gespeichert
> wird eine Zahl knapp neben 0,1, ebenso bei 0,2, und ihre Summe landet knapp neben der gespeicherten
> 0,3. Weil sie eine andere Zahl ist, braucht sie mehr Ziffern. Genau solche winzigen Abweichungen
> machen Kern beim Rechnen so penibel ([Kapitel 1](01-einfuehrung.md)).

**3. Misstrauen gegenüber fremden Dateien.** Ein Level kann von irgendwem stammen. Darum lehnt Kern
unendlich große Zahlen ab, absurd tief geschachtelte Einträge und ein paar Eintragsnamen, die auf der
Browser-Seite Unheil anrichten könnten.

Dazu kommt Gelassenheit beim Lesen: Fehlt ein Eintrag, gibt es einen leeren Wert statt eines
Absturzes, bei falschem Typ einen Ersatzwert. Und das Einlesen liefert sein Ergebnis **nur, wenn es
ganz geklappt hat**; sonst nennt es die Stelle, an der es hakte.

### Erst prüfen, dann übernehmen: Servoprofile

Ein **Servoprofil** beschreibt einen Motor der Micro Duck: wie kräftig er ist, wie steif und
gedämpft er nachgibt, wie verzögert er reagiert ([Kapitel 16](16-kalibrierung.md)). Kern prüft
erst, ob der Text überhaupt eine Motorbeschreibung ist, dann jeden Eintrag einzeln: Fehlen darf er
(dann gilt der Standardwert), aber wenn er da ist, muss er eine Zahl im erlaubten Bereich sein –
sonst Abbruch mit dem Namen des schuldigen Eintrags. **Erst ganz am Ende** wird das Profil übernommen.

> **Stell dir vor …** Ein Bankschalter. Erst wird das ganze Überweisungsformular geprüft, dann
> gebucht. Nie wird halb gebucht und dann gemerkt, dass die Kontonummer fehlt.

> **Probier's aus!** Schau einem Level unter die Haube.
> 1. **File → New Level → Basic**: ein Boden und ein Würfel.
> 2. Klick den Würfel an. Rechts in **Details** steht unter **Transform** das Feld **Location** mit
>    X, Y und Z. Tippe bei Z eine **3** und drück Enter: Der Würfel schwebt 3 Meter hoch.
> 3. **File → Save** (Ctrl+S). Chrome und Edge fragen, wohin; andere Browser legen die Datei
>    `Untitled.klevel` in den Download-Ordner.
> 4. Rechtsklick auf die Datei, **Öffnen mit**, dann **Editor** (Windows) bzw. **TextEdit** (Mac).
>
> Ganz oben steht `"format": "kern-level"` – daran erkennt Kern seine Levels. Such nach **Cube**:
> Darunter steht bei **position** die Zahlenfolge 0, 0, 3 – dein Würfel. So sieht ein Level von
> innen aus: Namen mit Werten, in fester Reihenfolge.

## 3.3 Mathematik für den 3D-Raum

### Die Spielregeln

Damit alle Teile dieselbe Welt meinen, gilt überall: Längen in **Metern**, Zeiten in **Sekunden**,
Massen in **Kilogramm**, und **Z zeigt nach oben**.

> **Stell dir vor …** Leg Karopapier auf den Tisch: X zeigt nach rechts, Y nach vorn, Z zur
> Zimmerdecke. Das heißt **rechtshändig**: Spreiz Daumen, Zeige- und Mittelfinger der rechten Hand
> rechtwinklig ab – das sind X, Y und Z.

Winkel stehen im Studio und in Levels in **Grad**. Innen rechnet Kern in **Radiant**, und das ist
ein einfaches Bild: Lauf am Rand eines Kreises mit 1 Meter Radius entlang. Ein halber Umlauf ist etwa
3,14 Meter lang (die Kreiszahl π). Der Winkel in Radiant ist die Strecke am Rand: 180° sind π, 90°
etwa 1,57. Viele Formeln werden so einfacher.

> **Probier's aus!** Öffne **File → New Level → Physics Lab**. Tippe ins Feld **Cmd** unten in der
> Statusleiste `gravity 0 0 -2`, drück Enter und starte **Simulate**. Die Schwerkraft zeigt jetzt
> schwach in Richtung minus Z, nach unten: Alles fällt fast wie auf dem Mond. `gravity` ohne Zahlen
> stellt die Erde wieder her, 0, 0, −9,80665: Fallendes wird jede Sekunde um rund 9,8 Meter pro
> Sekunde schneller.

### Vektoren und Drehungen

Ein **Vektor** ist eine Wegbeschreibung aus Zahlen: „3 Meter nach rechts, 4 nach vorn, 1 nach
oben“, kurz (3, 4, 1). Man kann ihn als Pfeil zeichnen; derselbe Zahlensatz kann auch den Punkt
meinen, an dem man ankommt. Oft braucht man nur die **Richtung** eines Pfeils und bringt ihn dafür
auf die Länge 1. Klickt jemand zweimal auf denselben Punkt, entsteht aber ein Pfeil der Länge null
ohne Richtung. Statt eines Unsinnswerts, der sich wie ein Virus durch alle weiteren Rechnungen
fräße, liefert Kern dann einen **Ersatz**: im Zweifel „nach oben“.

Jede **Drehung** im Raum lässt sich durch eine Achse und einen Winkel beschreiben.

> **Stell dir vor …** Ein Grillhähnchen am Spieß. Der Spieß ist die Achse, und das Hähnchen wird um
> einen Winkel gedreht. Jede Drehung, egal wie schief, ist so ein Spieß mit Winkel.

Kern packt Achse und Winkel in vier Zahlen, eine **Quaternion**. Die Mathematik dahinter ist
knifflig, der Gebrauch einfach: Achse und Winkel hinein, Drehung heraus. Die vier Zahlen entstehen
mit Kerns eigenen Winkelformeln aus Abschnitt 3.4, damit jede Drehung überall gleich ausfällt.

### Matrizen: eine ganze Bewegung in einem Paket

Ein Ding in der 3D-Welt wird meist auf einmal **gestreckt**, **gedreht** und **verschoben**. Eine
**Matrix** packt alle drei Schritte in ein Paket: eine Tabelle aus 4 mal 4, also 16 Zahlen. Man
steckt einen Punkt hinein und bekommt den fertig bewegten Punkt heraus. Der Renderer bekommt für
jedes Ding so ein Paket; im kleinsten Beispielspiel hat jede der sich drehenden Kisten ihr eigenes.

Ein Beispiel auf Karopapier: Der Punkt (1, 0, 0) liegt einen Meter rechts vom Ursprung. Das Paket
sagt: mit 2 strecken, um 90° um die Z-Achse drehen, um (1, 2, 3) verschieben.

1. **Strecken** mit 2: Aus (1, 0, 0) wird (2, 0, 0).
2. **Drehen** um 90° um Z, von oben gesehen gegen den Uhrzeigersinn: Was nach rechts zeigte, zeigt
   danach nach vorn. Aus (2, 0, 0) wird (0, 2, 0).
3. **Verschieben** um (1, 2, 3): Aus (0, 2, 0) wird **(1, 4, 3)**.

Ein Test in Kern prüft genau diese Rechnung.

> **Achtung, Falle!** Die Reihenfolge zählt. Erst zwei Schritte gehen und dann umdrehen ist etwas
> anderes als erst umdrehen und dann gehen. Darum legt Kern sie fest: erst strecken, dann drehen,
> dann verschieben.

Mathematiker schreiben so ein Paket „rückwärts“ auf: Der Punkt steht ganz rechts, die Schritte
links davon – **Verschieben · Drehen · Strecken · Punkt**. Das ist kein Widerspruch, man liest nur
vom Punkt aus, also von rechts nach links: Was dem Punkt am nächsten steht, wirkt zuerst. Wie beim
Anziehen: Außen am Fuß sitzt der Schuh, aber zuerst kam die Socke, die direkt am Fuß anliegt.

Eine Matrix lässt sich auch **umkehren**; dann macht sie die Bewegung rückwärts. Das braucht Kern,
wenn du in die Szene klickst: Der Weg führt vom Bildschirm zurück in die Welt. Kern rechnet diese
Umkehrung mit doppelt so vielen Stellen wie sonst, damit auch Klicks auf ferne Dinge treffen.

### Kamera und Sichtbereich

Die **Kamera** ist bei Kern keine Matrix, sondern eine **Beschreibung**, wie ein Fotograf sie gäbe:
Wo stehe ich? Wohin schaue ich? Wo ist oben? Wie weit ist mein Objektiv geöffnet? Kerns
Standardkamera öffnet **60°**, gemessen von unten nach oben, und malt von 5 Zentimetern bis 1,8
Kilometern Entfernung. Aus der Beschreibung errechnet Kern zwei Matrizen – eine für den Blick, eine
für das Objektiv, das den Raum aufs flache Bild drückt – und fasst sie zu **einer** zusammen: dem
ganzen Weg von der Welt aufs Bild. Eine unmögliche Kamera, etwa mit Auge und Ziel am selben Ort,
weist Kern mit einem Zettel an der Pinnwand zurück.

Neben der Perspektive gibt es die **Parallelansicht** ohne Fluchtpunkt, wie bei einem Bauplan: Ferne
Dinge werden nicht kleiner. Studio nutzt sie für die Ansichten von oben, vorn und von der Seite.

Was die Kamera sieht, ist eine liegende Pyramide mit abgeschnittener Spitze (Bild in
[Anhang B](B-grundlagen-rendering.md)). Was draußen liegt, muss nicht gemalt werden. Damit die
Prüfung schnell geht, steckt Kern jedes Ding in eine unsichtbare **Kugel** oder einen **Karton**.
Liegt die Kugel ganz außerhalb der Pyramide, ist das Ding unsichtbar – eine schnelle Prüfung statt
vieler Dreiecke. Ist eine Kugel ungültig, wird das Ding trotzdem gemalt. Kerns Regel: **im Zweifel
zeichnen** – lieber einmal zu viel als ein Loch in der Welt.

> **Probier's aus!** Öffne Orb Run. Klick in der Leiste oben im Viewport auf **≡** und stell
> **Field of View** erst auf 110°, dann auf 50°: Weitwinkel mit verzerrten Rändern, dann wie
> herangezoomt. Drück im Viewport **Alt+J**: Die Ansicht springt auf **Top**, die Parallelansicht von
> oben, wie ein Stadtplan. **Alt+G** holt die Perspektive zurück.

## 3.4 Mathematik, die überall gleich rechnet

Erinnerst du dich an die Wanderroute aus [Kapitel 1](01-einfuehrung.md)? Ein winziger Unterschied
am Anfang, und abends ist man in einem anderen Dorf. Hier geht es darum, woher solche Unterschiede
kommen.

Manche Rechnungen haben eine eindeutig festgelegte Antwort, die jeder Rechner gleich liefert:
Wurzelziehen, Abrunden, den Betrag einer Zahl. Bei den **Winkelrechnungen** Sinus und Kosinus ist
das **nicht** so.

> **Stell dir vor …** Ein Punkt läuft auf dem Kreis mit 1 Meter Radius aus Abschnitt 3.3. Der
> Kosinus sagt, wie weit er gerade rechts vom Mittelpunkt steht, der Sinus, wie weit oben. Wer etwas
> drehen will, braucht diese Zahlen ständig.

Solche Werte lassen sich nur annähern. Jedes System – Linux, macOS, Android, WebAssembly – nähert auf
seine eigene Weise an, und im allerletzten Bit kommt manchmal etwas anderes heraus. In einer
Robotersimulation wird daraus binnen Sekunden ein anderer Gang, und ein im Browser gelerntes Netz
liefe auf dem eigenen Rechner anders.

Kerns Lösung: **eigene Formeln** für Sinus, Kosinus und Verwandte, aus einer freien
Programmsammlung. Sie werden überall gleich übersetzt, ohne Rechen-Abkürzungen, und tragen eigene
Namen, damit sie nie mit denen des Systems verwechselt werden. Die Physik-Bibliothek hat ohnehin
eigene, überall gleiche Winkelrechnungen. Für neuen Code gilt darum:

- Alles, was die **Simulation beeinflusst**, benutzt Kerns eigene Formeln.
- Keine Summe darf davon abhängen, wie viele Helfer mitrechnen.
- Keine Rechen-Abkürzungen, die „schnell, aber ungefähr“ rechnen.

Reine **Darstellung** – Ton, Funken, Teile des Editors – darf die Formeln des Systems benutzen: Ob ein
Funke einen Millimeter weiter links fliegt, merkt niemand. Fließt so ein Wert aber in Physik,
Steuerung oder Training ein, muss er auf Kerns eigene Formeln umziehen.

> **Wusstest du?** Die eigenen Formeln kosten keine Geschwindigkeit: Sie sind so schnell wie die des
> Systems, und das Tempo der Physik bestimmt ohnehin das Berechnen der Gelenke. Ob alles stimmt,
> verrät der Fingerabdruck aus [Kapitel 2](02-bauen-und-testen.md).

> **Für Neugierige:** Die Formeln stammen aus der Programmsammlung **musl**; jede bekommt eine
> Vorsilbe, damit sie nie mit der Fassung des Systems zusammenstößt.

## Das nimmst du mit

- Kern legt bei jeder Zahl fest, wie viele Bit-Kästchen sie hat.
- Fehler meldet Kern mit „Nein“ und einem Zettel an einer einzigen Pinnwand.
- Die Uhr dient Messungen, nie der Simulation: Die zählt feste Schritte wie ein Brettspiel.
- Ein Span zeigt auf Daten, ohne sie zu besitzen; wer sie behalten will, kopiert sie.
- Kerns JSON hält die Reihenfolge, schreibt Zahlen wie der Browser und misstraut fremden Dateien.
- Meter, Sekunden, Kilogramm, **Z oben**; eine Matrix packt Strecken, Drehen und Verschieben in ein
  Paket, und die Reihenfolge zählt.
- Die Kamera ist eine Beschreibung wie beim Fotografen; Unsichtbares wird aussortiert, im Zweifel
  aber gemalt.
- Für die Simulation gelten eigene Winkelformeln, feste Summen und keine Rechen-Abkürzungen.

## Knobelecke

1. Zwei Aufgaben scheitern kurz nacheinander. Danach liest du die Pinnwand. Wessen Zettel siehst du?
2. Der Punkt (1, 0, 0) wird um 90° um die Z-Achse gedreht und dann um (5, 0, 0) verschoben. Wo
   landet er? (Tipp: Karopapier, X nach rechts, Y nach vorn.)
3. Ein Funkeneffekt benutzt die Sinus-Formel des Systems. Problem? Und wenn die Funken die Ente
   anschubsen sollen?
4. Warum ergibt 0,1 + 0,2 im Computer nicht genau 0,3?
5. Warum darf die Ente nicht auf die Uhr schauen, um zu entscheiden, wie weit sie in einem Takt geht?

### Lösungen

1. Den der zweiten Aufgabe: Jeder neue Zettel ersetzt den alten.
2. Drehen macht aus (1, 0, 0) den Punkt (0, 1, 0), Verschieben dann (5, 1, 0).
3. Für reine Darstellung ist es erlaubt. Schubsen die Funken die Ente, fließen sie in die Physik
   ein, und dann muss Kerns eigene Formel her.
4. Im Zweiersystem sind 0,1 und 0,2 unendliche Brüche. Gespeichert werden Zahlen knapp daneben, und
   ihre Summe liegt knapp neben 0,3.
5. Ein langsamer Rechner zeigte mehr verstrichene Zeit an als ein schneller, und die Ente liefe auf
   beiden verschieden. Mit festen Schritten läuft sie überall gleich.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
