# 1 Willkommen bei Kern!

> **Kurz gesagt:** Kern ist ein Spiele-Motor, der direkt im Browser läuft – ohne Installation. Er
> malt 3D-Welten, rechnet, wie Dinge fallen und zusammenstoßen, spielt Klänge, lässt Spiele laufen
> und bringt sogar einem kleinen Roboter das Gehen bei. Hier lernst du Kern kennen, seine Regeln,
> seinen Aufbau – und wie du dieses Buch liest.

> **Begriffe vorab:**
> - **C++ und JavaScript** – zwei **Programmiersprachen**, also genau festgelegte Schreibweisen für
>   Anweisungen an den Computer, so wie ein Strickmuster eine Schreibweise für Maschen ist. C++ ist
>   sehr schnell und wird für große Programme wie Spiele benutzt; JavaScript versteht jeder Browser
>   von Haus aus.
> - **Übersetzer** (Fachwort: **Compiler**) – ein Programm, das Text in einer Programmiersprache in
>   Befehle verwandelt, die eine Maschine direkt ausführen kann.
> - **WebAssembly** – eine Befehlssprache, die alle modernen Browser ausführen können. Kern wird aus
>   C++ in sie übersetzt. Heraus kommt ein **Modul**: eine einzige Datei, die der Browser lädt.
> - **Bild** (Fachwort: **Frame**) – eines der vielen Einzelbilder pro Sekunde, aus denen eine
>   Bewegung auf dem Bildschirm besteht, wie die Blätter eines Daumenkinos.
> - **Spielen** (Play) und **Simulieren** (Simulate) – zwei Knöpfe in Kern Studio. Bei **Spielen**
>   spielst du selbst mit und steuerst die Spielfigur. Bei **Simulieren** läuft die Welt mit ihrer
>   Physik, und du schaust mit
>   frei beweglicher Kamera zu.
> - **Schnittstelle** – die vereinbarte Übergabestelle zwischen zwei Programmteilen, wie der Tresen
>   zwischen Küche und Gastraum.
> - **Neuronales Netz** – ein Rechenmodell, das durch Übung lernt; hier lernt es, wie die
>   Roboterente ihre Motoren bewegen soll (mehr in [Anhang A](A-grundlagen-neuronale-netze.md)).
> - **Bit** – die kleinste Einheit im Computer: ein Kästchen, in dem 0 oder 1 steht. Alle Zahlen
>   bestehen aus vielen solchen Kästchen.

Schön, dass du da bist! Stell dir eine Werkstatt vor: Auf der **Leinwand** entstehen Bilder (die
Grafik), an der **Werkbank** wird gestoßen, gezogen und fallen gelassen (die Physik), am
**Mischpult** entstehen Klänge (der Ton), und auf der **Bühne** läuft das Spiel. In der Ecke lernt
eine Roboterente laufen. Diese Werkstatt heißt **Kern** und steht offen auf https://kernengine.de/ –
mit **Kern Studio**, dem Editor, in dem du Welten baust und sofort spielst, mit **dem Lehrer**, der
Roboter-Gehirnen das Laufen beibringt, und mit spielbaren Beispiel-Levels.

## 1.1 Was Kern ist

Eine **Engine** ist der Motor unter der Haube eines Spiels: Sie malt, rechnet Stöße, spielt Töne
und liest Maus und Tastatur. Das Spiel sagt nur noch: „Hier steht ein Auto, und bei W fährt es.“

> **Stell dir vor …** Ein Theater. Die Engine ist die Bühnentechnik: Licht, Vorhang, Ton. Das Spiel
> ist das Stück. Dieselbe Technik trägt viele verschiedene Stücke.

Kern ist in **C++** geschrieben, weil C++ schnell ist. Der Browser versteht C++ aber nicht direkt.
Darum wird Kern vorher in **WebAssembly** übersetzt. Heraus kommt ein **Modul**, das der
Browser lädt wie ein Bild. Zum Malen benutzt Kern die **Grafikschnittstelle** des Browsers – die
Übergabestelle, über die eine Webseite der Grafikkarte sagen darf, was sie zeichnen soll.

Im selben Modul stecken alle Teile der Werkstatt:

- die **Grundlagen**: Zahlen, Fehlermeldungen und die Mathematik für den 3D-Raum
  ([Kapitel 3](03-grundlagen.md));
- das **Malen**: Licht, Schatten, Himmel, dazu Bausteine für Häuser, Straßen und Gelände;
- die **Physik**: wie Dinge fallen, rollen, zusammenstoßen und durch Gelenke verbunden sind;
- der **Ton**: Klänge, die von links, rechts, nah oder fern kommen;
- die **Spielregeln**: Levels, Dinge in der Welt, Mehrspieler und Spiellogik zum Zusammenklicken;
- **Kern Studio**, der Editor, und **der Lehrer**, ein Trainer für Roboter-Gehirne, der die
  Simulation auch mit Messungen am echten Roboter vergleicht.

Und dann ist da die **Micro Duck**: ein kleiner zweibeiniger Roboter. Entworfen hat ihn die
Robotik-Firma **Pollen Robotics**, die seine Baupläne und seine trainierten neuronalen Netze frei
veröffentlicht hat. Kern benutzt diese veröffentlichten Pläne und Netze, ist aber ein eigenständiges
Projekt: Mit der Firma selbst ist es nicht verbunden.

Kern ist aus dem Spiel SpawnCity entstanden. Trotzdem kennt die Engine kein einziges Spiel. Im
Theaterbild: Das Stück benutzt die Bühnentechnik – es sagt „Licht an“ und „Vorhang auf“ –, aber die
Technik weiß nichts vom Stück. Darum trägt sie morgen ein ganz anderes Stück genauso gut.

Die genauen Namen und Werte stehen im blauen Buch, Kapitel 1.

## 1.2 Die Regeln, nach denen Kern gebaut ist

### Selbst gekocht

Kern setzt auf keine fremde Grafik-Engine. Das Malen ist eigener Code, der direkt mit der
Grafikschnittstelle spricht. Der Gewinn: Man weiß genau, was passiert.

Ganz ohne fremde Teile geht es trotzdem nicht. Für die Physik nimmt Kern eine fertige
**Physik-Bibliothek** namens Box3D – eine Programmsammlung, die jemand anderes geschrieben und frei
veröffentlicht hat. Kern verändert sie nicht, sondern baut sie so ein, wie sie ist. Was Kern damit
macht, entscheidet es selbst.

> **Stell dir vor …** Ein Koch, der jede Soße selbst rührt, statt Fertigsoße zu nehmen. Ein paar
> Zutaten kauft auch er – das Mehl mahlt er nicht selbst. Aber er nimmt sie, wie sie sind, und
> mischt sie selbst. So weiß er bei jedem Teller, was drin ist.

Kern benutzt außerdem nur die ältesten, einfachsten Fähigkeiten von WebAssembly. Das ist wie ein
Brief in einfacher Sprache: Ihn versteht jeder. So läuft Kern auf jedem Browser, der 3D-Grafik
kann, auch auf älteren Safari-Versionen.

### Keine Engine-Logik in JavaScript

Was ein Spiel tut, steht an **einer** Stelle: im C++-Teil. Manches kann im Browser aber nur
JavaScript – an die Grafikkarte kommen, Maus und Tastatur lesen, Ton ausgeben, Dateien öffnen.
Genau das, und **nur** das, erledigt eine dünne JavaScript-Schicht. Auch Kern Studio hält sich
daran: Jeder Klick im Editor wird ein Befehl an die Engine.

> **Stell dir vor …** Die JavaScript-Schicht ist ein Kellner. Er nimmt deine Bestellung auf und
> bringt das Essen an den Tisch. Gekocht wird aber ausschließlich in der Küche – in C++.

Der Gewinn: Ein Spiel verhält sich im Editor genauso wie als fertiges Spiel, und man kann es auch
ohne Browser automatisch prüfen ([Kapitel 2](02-bauen-und-testen.md)).

### Überall dasselbe Ergebnis

**Determinismus** heißt: gleiche Eingabe, gleiches Ergebnis – **Bit für Bit**, ob auf einem PC,
einem Mac oder im Browser. Das klingt selbstverständlich. Ist es aber nicht, denn Computer
**runden**.

Ein Computer speichert jede Kommazahl mit einer festen Zahl von Stellen. Was nicht hineinpasst, wird
gerundet, nach jedem einzelnen Rechenschritt. Darum ist beim Computer sogar die Reihenfolge beim
Zusammenzählen nicht egal. Ein Beispiel mit einem Taschenrechner, der nur vier Stellen hat:

- (1000 + 0,4) + 0,4: Erst gibt es 1000,4, gerundet 1000. Dann wieder 1000,4, gerundet **1000**.
- 1000 + (0,4 + 0,4): Erst gibt es 0,8. Dann 1000,8, gerundet **1001**.

Echte Computer haben viel mehr Stellen, aber das Prinzip ist dasselbe.

Für Plus, Minus, Mal und Geteilt gibt es eine feste, weltweit gleiche Rundungsregel: Daran halten
sich alle Rechner. Anders bei **Winkelrechnungen**. Winkel braucht ein Spiel ständig – jede Drehung,
jedes Gelenk der Ente ist ein Winkel. Rechnungen wie der Sinus (mehr in
[Kapitel 3](03-grundlagen.md)) haben aber unendlich viele Nachkommastellen und lassen sich nur
annähern. Jeder Computer bringt dafür seine eigenen Formeln mit und landet im letzten Bit manchmal
ein winziges bisschen woanders.

So ein Unterschied in der letzten Stelle wäre egal – wenn er klein bliebe. In einer **Simulation**,
einer nachgerechneten Welt, rechnet aber jeder Schritt mit dem Ergebnis des vorigen weiter. Der
Fehler wird mitgeschleppt und wächst. Bei einer laufenden Roboterente setzt dann nach ein paar
Sekunden ein Fuß anders auf.

> **Stell dir vor …** Zwei Kinder würfeln eine Wanderroute aus, an jeder Kreuzung ein Wurf. Weicht
> nur der erste Wurf ab, landen sie abends in verschiedenen Dörfern.

Darum hält Kern drei Regeln ein:

- **Keine Rechen-Abkürzungen.** Das Rechenwerk im Computer heißt **Prozessor**. Manche Prozessoren
  rechnen „mal und dann plus“ in einem einzigen Zug und runden dabei nur einmal statt zweimal –
  schneller, aber mit einem anderen letzten Bit. Der Übersetzer darf solche Abkürzungen nicht
  benutzen. Auch die Physik-Bibliothek verzichtet auf Sonderbefehle, die nur manche Prozessoren
  haben, und rechnet überall auf dem einfachen Weg, so wie im Browser.
- **Eigene Winkelformeln.** Für Winkelrechnungen bringt Kern seine eigenen Formeln mit, die überall
  gleich rechnen ([Kapitel 3](03-grundlagen.md)).
- **Feste Reihenfolge beim Zusammenzählen.** Erinnerst du dich an den Taschenrechner? Die
  Reihenfolge zählt. Wenn die Ente laufen lernt (beim Lehrer, Abschnitt 1.4), rechnen mehrere
  Helfer gleichzeitig und werden mal in dieser, mal in jener Reihenfolge fertig. Würde man ihre
  Ergebnisse so zusammenzählen, wie sie eintreffen, käme jedes Mal etwas anderes heraus. Darum legt
  Kern die Reihenfolge fest, egal wie viele Helfer mitrechnen.

Bewiesen wird das mit einem **Fingerabdruck**: Die Ente läuft ein Stück, und nach jedem Schritt
wird ihr Zustand zu einer Prüfzahl verdichtet. Diese Liste muss auf allen Rechnern exakt gleich sein
([Kapitel 2](02-bauen-und-testen.md)).

> **Wusstest du?** Dank Determinismus ist ein Fehler der Simulation ein fester, wiederholbarer
> Fehler statt Zufallsrauschen. Läuft die simulierte Ente anders als die echte, liegt das wirklich
> an der Nachbildung der Ente und nicht am Rechner ([Kapitel 16](16-kalibrierung.md)).

### Noch ein paar Hausregeln

- **Kaputte Daten werden abgewiesen, bevor sie Schaden anrichten.** Jede Eingabe wird erst geprüft,
  dann übernommen. Ist sie kaputt, bleibt alles, wie es war.
- **Fehler werden höflich gemeldet.** Eine Aufgabe, die nicht klappt, antwortet „Nein“ und hinterlässt
  einen Zettel mit dem Grund; das Programm läuft weiter ([Kapitel 3](03-grundlagen.md)).
- **Die Physik wird nicht geschönt.** Die Micro Duck steht nur durch ihre eigenen Motoren und Füße –
  keine unsichtbare Hand hält sie fest ([Kapitel 6](06-physik.md), [Kapitel 13](13-micro-duck.md)).
- **Eine Umsetzung für alle Orte.** Editor, fertiges Spiel und Lehrer teilen dieselbe Spielwelt und
  dieselbe Physik. Darum läuft ein Netz, das im Lehrer (dem Trainer für die Ente, Abschnitt 1.4)
  gelernt hat, unverändert im Studio.
- **Kein Wegwerf-Speicher bei jedem Bild.** Bei jedem Bild reicht die Browser-Seite neue Zahlen an
  Kern weiter. Dafür benutzt sie immer denselben Ablageplatz, statt jedes Mal einen neuen anzulegen
  und wegzuwerfen. Sonst müsste der Browser ständig hinter ihr aufräumen, und das kostet Zeit
  mitten im Spiel. Die Kehrseite: Was Kern behalten will, muss es sich kopieren
  ([Kapitel 3](03-grundlagen.md)).
- **Robust:** Ein Browser darf einer Seite die Grafikkarte jederzeit wegnehmen – etwa bei
  Speichermangel, wenn der Grafiktreiber neu startet, oder auf dem Handy, wenn die Seite im
  Hintergrund liegt. Dann wartet Kern und baut danach alles wieder auf ([Kapitel 4](04-grafik.md)).

## 1.3 Ein Haus aus Stockwerken

Kern ist in Schichten gebaut wie ein Haus. Jedes Stockwerk benutzt die darunter, nie die darüber.
Der Vorteil: Man kann oben umbauen, ohne dass unten etwas einstürzt, und jedes Stockwerk lässt sich
für sich prüfen.

![Die Stockwerke von Kern: oben der Browser-Teil, darunter das Programm vom Editor bis zum Fundament](../abbildungen/architektur.svg)

So liest du das Bild: Jedes Stockwerk benutzt die darunter. Ganz oben steht der Browser-Teil, der Kellner: Er
nimmt deine Eingaben entgegen und reicht sie durch die Übergabestelle nach unten weiter.
„Netzwerk“ meint hier das Spielen mit mehreren Leuten über das Internet, nicht das neuronale Netz
der Ente.

Nicht jedes Programm braucht das ganze Haus. Malen, Ton, Logik, Netzwerk und das Fundament sind
immer dabei. Die übrigen Stockwerke – Physik, Spielregeln, Editor, Trainer – wählt man beim Bauen
aus, damit ein Programm nicht unnötig groß wird: Ein Spiel, das nur malt, braucht keinen Editor
([Kapitel 2](02-bauen-und-testen.md)). Wer ein oberes Stockwerk wählt, bekommt die darunter
automatisch mit: Editor und Trainer brauchen die Spielregeln, und die Spielregeln brauchen die
Physik.

C++ und JavaScript reden nur über einen gemeinsamen Speicherbereich miteinander. JavaScript legt
dort Daten an vereinbarte Stellen und sagt der Engine, wo sie liegen. Die Engine antwortet auf
demselben Weg. Braucht sie umgekehrt etwas vom Browser – die Grafikkarte, die Uhrzeit, eine
Zufallszahl –, ruft sie ihn über eine kleine Liste vereinbarter Anfragen.

> **Stell dir vor …** Zwei Nachbarn ohne gemeinsame Sprache teilen sich einen Briefkasten. Einer
> legt Zettel hinein, der andere holt sie ab. Dazu gibt es eine Klingel mit ein paar vereinbarten
> Zeichen: Die Engine darf klingeln und etwa um die Uhrzeit oder eine Zufallszahl bitten. Mehr
> Kontakt gibt es nicht.

Die genauen Namen und Werte stehen im blauen Buch, Kapitel 1; alle Funktionen an dieser Grenze
listet [Kapitel 17](17-referenz.md).

## 1.4 Vier Wege, Kern zu benutzen

Alle vier Wege bestehen aus demselben C++-Code, nur verschieden zusammengestellt.

**Ein Spiel ganz in C++.** Für Programmiererinnen und Programmierer: Man schreibt ein kleines
C++-Programm, das beim Start die Welt anlegt und danach in jedem Bild sagt, was sich bewegt; Kern
malt. Das kleinste Beispiel stellt ein Feld sich drehender Kisten auf einen Boden. Das große
Beispiel ist ein ganzes Spiel mit Stadt, Fahrphysik, Verkehr, Tag und Nacht
([Kapitel 12](12-eigene-spiele.md)).

**Nur das Malen, für eine Webseite.** Webseiten können Kern auch nur als Maler benutzen; so macht
es das Spiel SpawnCity ([Kapitel 4](04-grafik.md)).

**Kern Studio und fertige Spiele.** Für alle, die lieber klicken als tippen. Das ganze Studio ist
**eine** Datei, die sogar ohne Internet per Doppelklick läuft. Du baust ein Level aus **Actors**
(Dingen in der Welt) und probierst es direkt im Fenster aus: mit **Spielen** (Play) spielst du
selbst mit, mit **Simulieren** (Simulate) schaust du der laufenden Welt zu.
**Datei › Spiel verpacken (HTML)...** (File › Package Game (HTML)...) macht daraus ein eigenständiges Spiel, wieder eine einzige Datei, die du
verschicken kannst. Sie läuft mit derselben Engine wie der Editor und verhält sich darum genauso
([Kapitel 8](08-spiel-laufzeit.md), [Kapitel 11](11-studio.md)).

> **Probier's aus!** Öffne Kern Studio auf https://kernengine.de/. Beim ersten Start ist Orb Run
> geladen (sonst: **Datei › Beispiel-Levels › Orb Run** (File › Example Levels › Orb Run)). Drück
> **Spielen** (Alt+P) und fahr mit dem schwebenden Auto, dem Hovercar, los: **W A S D** oder die
> Pfeiltasten lenken und geben Gas, **Umschalt** gibt Schub. Sammle ein paar **Orbs** ein, die
> leuchtenden Energiekugeln auf den Straßen. Dann **Stopp** (Stop) oder Esc. Alles, was du gerade
> gesehen und gehört hast – Licht, Fahrphysik, Ton –, hat die Engine im Browser gerechnet.

> **Wusstest du?** Kern Studio zeigt seine Oberfläche auf Deutsch oder Englisch; ohne Vorgabe folgt
> es der Sprache des Browsers. Dieses Buch nennt die Beschriftungen, wie Studio sie auf Deutsch
> zeigt, in Klammern die englischen. Wer Studio auf Englisch nutzt, findet sie dort: Der Knopf
> **DE | EN** unten rechts in der Statusleiste wechselt die Sprache, ebenso `?lang=de` bzw.
> `?lang=en` in der Adresse; Studio merkt sich die Wahl auf dem Gerät.

**Der Lehrer.** Er trainiert Netze für die Micro Duck: auf deinem Rechner als eigenes Programm oder
im Browser auf der Lehrer-Seite. Dort sammeln mehrere Hintergrund-Helfer gleichzeitig Erfahrungen
mit simulierten Enten, und einer lernt daraus. Beide Fassungen benutzen dieselben Trainingsrezepte
und dieselbe Spielwelt. Am Anfang des Lernens steht ein **Startwert** für den Zufall – wie ein
Kartenspiel, das jedes Mal auf genau dieselbe Weise gemischt wird. Gleicher Startwert, gleich viele
Enten: **dasselbe Netz**, hier wie dort. Ein fertiges Netz speichert der Lehrer im selben Format wie
die Original-Netze von Pollen Robotics; darum lädt das Studio es genauso wie diese
([Kapitel 14](14-lehrer.md)).

An der Ente sieht man alles auf einmal:

- Ihr **Körper** ist nach den Original-Bauplänen von Pollen Robotics nachgebildet: Physik-Teile,
  verbunden durch Gelenke.
- Ihre **Motoren** sind genau nachgebaut, bis hin zu ihrer Kraft und ihrer kleinen Verzögerung.
- Ihr **Gehirn** ist ein neuronales Netz, das Kern selbst rechnet.
- Ihre **Stimme** kommt aus dem Ton-Mischer.
- **Spiellogik** zum Zusammenklicken kann sie steuern.
- Im Studio ist sie einfach ein **Actor** wie jedes andere Ding.

![Das Micro-Duck-Level im Studio](../buch/bilder/duck-level.png)

> **Probier's aus!** Wähle **Datei › Neues Level...** (File › New Level...) und dort eine der sechs
> Vorlagen, zum Beispiel **Micro Duck**. Drück **Spielen** und schau zu, wie die Ente losläuft, gesteuert von ihrem gelernten
> Gehirn. Niemand hat ihr die Schritte vorgeschrieben: Das Netz entscheidet in jedem Augenblick neu,
> wie sich die Motoren bewegen.

### Ein Werkzeug für später: die Konsole

Ganz unten im Studio liegt die **Statusleiste**. Darin steht ein Eingabefeld mit der Aufschrift
**Cmd** (kurz für „command“, Befehl): die **Konsole**. Hier tippst du Befehle an Studio und schickst
sie mit Enter ab; `help` zeigt alle Befehle. Die Antworten erscheinen im **Ausgabeprotokoll** (Output Log), das du über
den gleichnamigen Knopf in der Statusleiste aufklappst. Mit der Cmd-Taste des Mac hat das Feld nichts
zu tun. In den nächsten Kapiteln benutzt du es öfter.

## 1.5 Wo was liegt

Der gesamte Bauplan von Kern ist öffentlich einsehbar:
https://github.com/ChristianHohlfeld/kernengine.de. Kern steht unter der **Apache License 2.0**,
einer Lizenz, die freie Nutzung erlaubt; die Bestandteile anderer Leute sind mit ihren eigenen
Lizenzen aufgeführt.

Die genaue Ordnerkarte steht im blauen Buch, Kapitel 1.

## 1.6 Wie du dieses Buch liest

Du liest das **grüne Buch**. Sein großer Bruder, das **blaue Buch**, hat Kapitel für Kapitel
denselben Inhalt, aber technisch bis in jede Einstellung. Packt dich ein Thema, schlag dort nach.

Unterwegs begegnen dir immer dieselben Kästen: **Kurz gesagt**, **Begriffe vorab** (die Wörter,
die das Kapitel braucht), **Stell dir vor …** (ein Alltagsvergleich), **Probier's aus!** (eine
Aufgabe in Kern Studio), **Wusstest du?**, **Achtung, Falle!** (ein typischer Fehler) und **Für
Neugierige** (ein technisches Detail zum Überspringen). Am Kapitelende stehen **Das nimmst du mit**
und die **Knobelecke**, die Lösungen direkt darunter. Nicht spicken! (Na gut, ein bisschen.)

Das Buch ist von unten nach oben gebaut, wie Kern selbst: erst Bauen und Grundlagen, dann Grafik,
Physik, Ton, Spiel, Logik und Netzwerk, dann Studio, Micro Duck und Lehrer, zum Schluss die
[Referenz](17-referenz.md). Anhänge erklären neuronale Netze ([A](A-grundlagen-neuronale-netze.md)),
Echtzeit-Grafik ([B](B-grundlagen-rendering.md)) und Physik ([C](C-grundlagen-physik.md)).

Wo fängst du an?

- **Du willst einfach verstehen, wie Spiele und lernende Roboter funktionieren, ganz ohne
  Technikvorwissen?** Dann lies nach diesem Kapitel [Anhang B](B-grundlagen-rendering.md) und
  [Kapitel 4](04-grafik.md) (wie Bilder entstehen), dann [Anhang C](C-grundlagen-physik.md) und
  [Kapitel 6](06-physik.md) (wie Dinge fallen und stoßen), dann
  [Anhang A](A-grundlagen-neuronale-netze.md), [Kapitel 13](13-micro-duck.md) und
  [Kapitel 14](14-lehrer.md) (wie die Ente laufen lernt). Zwischendurch lohnt
  [Kapitel 11](11-studio.md), damit du im Studio alles selbst ausprobieren kannst. Kapitel 2 und 3
  kannst du überfliegen und bei Bedarf nachschlagen.
- **Du willst ein Spiel bauen?** Kapitel 11 (Studio) oder 12 (C++), dazu 8 und 9.
- **Du liebst die Ente?** Kapitel 13, 14 und 16.
- **Du willst Kern verändern?** Kapitel 2 und 3, vor Physik oder Roboter auch 6 und 13.
- **Du willst Kern in eine Webseite einbauen?** Kapitel 4 und 17.

## Das nimmst du mit

- Kern ist ein Spiele-Motor, der direkt im Browser läuft – ohne Installation.
- Kern ist in C++ geschrieben und wird für den Browser in WebAssembly übersetzt.
- Malen, Physik, Ton, Spielregeln, Editor und Lehrer stecken im selben Modul.
- JavaScript ist nur der Kellner: Gekocht wird in C++.
- Computer runden; kleine Unterschiede schaukeln sich auf. Darum rechnet Kern überall Bit für Bit
  gleich – ein Fingerabdruck beweist es.
- Aus demselben Code entstehen C++-Spiele, das Malen für Webseiten, Studio und Lehrer.
- Das grüne Buch erklärt einfach, das blaue geht in die Tiefe.

## Knobelecke

1. Warum ist es praktisch, dass keine Engine-Logik in JavaScript steckt?
2. Ein Netz wurde im Browser trainiert. Warum läuft es im Studio genauso?
3. Rechne mit dem Vier-Stellen-Taschenrechner: Ergibt (1000 + 0,3) + 0,3 dasselbe wie
   1000 + (0,3 + 0,3)? Was sagt dir das über Simulationen?
4. Was passiert, wenn der Browser Kern mitten im Spiel die Grafikkarte wegnimmt?

### Lösungen

1. Alles, was ein Spiel tut, steht an einer Stelle in C++: ohne Browser prüfbar und im fertigen
   Spiel genauso wie im Editor.
2. Lehrer und Studio teilen Spielwelt, Motoren und Physik, und Kern rechnet überall gleich.
3. Nein: links kommt 1000 heraus (1000,3 wird zweimal zu 1000 gerundet), rechts 1001 (0,6 dazu
   ergibt 1000,6, gerundet 1001). Beim Computer zählt die Reihenfolge – in einer Simulation wächst so
   ein Unterschied mit jedem Schritt.
4. Das darf der Browser, etwa bei Speichermangel oder wenn der Grafiktreiber neu startet. Kern
   wartet, bis die Grafikkarte zurück ist, und baut dann alles wieder auf.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
