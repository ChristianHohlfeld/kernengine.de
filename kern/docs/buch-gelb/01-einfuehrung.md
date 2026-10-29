# 1 Willkommen in der Werkstatt

> **Dein Level:** Du betrittst Kern, eine echte 3D-Engine im Browser. Du erfährst, was darin
> steckt, nach welchen Regeln sie gebaut ist und wie du dieses Buch spielst.

> **Wörter, die du brauchst:**
> - **Programmiersprache** – eine Sprache, in der Menschen einem Computer genaue Anweisungen
>   aufschreiben. C++ und JavaScript sind zwei davon.
> - **Maschinensprache** – Befehle, die ein Rechenwerk direkt ausführen kann, ohne sie erst
>   übersetzen zu müssen.
> - **Browser** – das Programm, mit dem du Webseiten öffnest, etwa Chrome, Firefox oder Safari.
> - **Grafikkarte** – der Chip im Computer, der die Bilder zeichnet.
> - **Bit** – die kleinste Einheit im Computer: eine 0 oder eine 1. Alle Zahlen bestehen aus Bits.
> - **Simulation** – eine Welt im Computer, die nach festen Regeln Schritt für Schritt weiterrechnet,
>   zum Beispiel wie ein Würfel fällt.
> - **Level** – eine Spielwelt mit allem, was darin steht.

Hallo und willkommen! Unter jedem Spiel steckt eine **Engine**, der Motor unter der Haube. Das
Spiel sagt nur: „Hier steht ein Auto, bei W fährt es.“ Zeichnen, Physik und Ton erledigt die Engine.

In diesem Buch nimmst du die Engine **Kern** auseinander. Auf https://kernengine.de/ findest du
alles, worum es geht:

- **Kern** ist die Engine: Sie zeichnet, rechnet Physik und spielt Töne, für jedes Spiel, das sie
  benutzt.
- **Kern Studio** ist der Editor: Dort baust du Welten aus fertigen Teilen und spielst sie sofort.
- Die **Micro Duck** ist ein echter zweibeiniger Roboter der Firma Pollen Robotics. In Kern läuft sie
  als Nachbau in der Simulation, gesteuert von ihren echten Netzen.
- Ein **Netz** (genauer: neuronales Netz) ist ein künstliches „Gehirn“ aus vielen Zahlen, das durch
  Üben lernt. Bei der Micro Duck steuert ein Netz die Beine.
- Der **Lehrer** ist das Programm, das solche Netze trainiert: Er lässt viele Enten in der Simulation
  üben, bis sie laufen oder springen können.

Dazu gibt es spielbare Levels wie **Orb Run** und **Physics Lab**.

![Kern Studio mit dem Level Orb Run](../buch/bilder/studio.png)

> **Mission:** Öffne Kern Studio: Klick auf https://kernengine.de/ auf **Editor**. Beim allerersten Start ist **Orb Run** geladen (sonst
> **Datei › Beispiel-Levels › Orb Run** (File › Example Levels › Orb Run)). Drück **Spielen** (Play)
> (Alt+P), fahr mit dem Hovercar los und sammle Orbs. **Esc** beendet das Spiel.

> **Gut zu wissen:** Studio zeigt seine Oberfläche auf Deutsch oder Englisch; ohne Vorgabe folgt es
> der Sprache des Browsers. Dieses Buch nennt die Beschriftungen, wie Studio sie auf Deutsch zeigt, in
> Klammern die englischen, etwa **Datei** (File). Wer Studio auf Englisch nutzt (Knopf **DE | EN**
> unten rechts in der Statusleiste oder `?lang=en` in der Adresse), findet sie dort. Studio merkt sich
> die Wahl auf deinem Gerät.

## 1.1 Was Kern ist

Kern ist in der Programmiersprache **C++** geschrieben. Die ist schnell, aber Browser verstehen sie
nicht. Deshalb wird Kern vorher **übersetzt**, und zwar in **WebAssembly**, eine Maschinensprache,
die jeder moderne Browser versteht und sehr schnell ausführt. Das Zeichnen übernimmt die Grafikkarte;
Kern spricht sie über
**WebGL2** an, den Weg, auf dem eine Webseite der Grafikkarte Zeichenbefehle gibt.

> **So kannst du es dir vorstellen:** Ein Buch erscheint auf Englisch. Damit es in Deutschland jeder
> lesen kann, übersetzt es jemand einmal. Danach liest jeder die Übersetzung. Genauso wird Kern
> einmal von C++ nach WebAssembly übersetzt, und dein Browser führt die Übersetzung aus.

Entstanden ist Kern aus dem Spiel **SpawnCity**. Trotzdem kennt die Engine SpawnCity nicht: **Spiele
sprechen mit der Engine, nie umgekehrt.** Das hat einen guten Grund: Wüsste die Engine etwas von
SpawnCity, würde sie sich auf SpawnCity verlassen, und man könnte mit ihr nur SpawnCity bauen. Weil
sie nichts über ein bestimmtes Spiel weiß, passt sie unter jedes.

> **So kannst du es dir vorstellen:** ein Legostein. Er weiß nicht, ob du ein Schiff oder ein Haus
> baust – und gerade deshalb passt er in beides. Hätte jemand ihn nur für ein Schiff geformt, könntest
> du daraus kein Haus mehr bauen.

Kern besteht aus diesen Teilen (die **Spielwelt** ist alles, was beim Spielen passiert, der
**Kellner** kommt gleich in 1.2):

| Teil | Was er macht | Zum Beispiel |
|---|---|---|
| Grundlagen, Mathe | Zahlen, Fehler, Positionen | wo der Würfel steht |
| Grafik | Bilder zeichnen | Häuser, Straßen, Gelände |
| Physik | Stöße, Gelenke, Schwerkraft | eine Kiste fällt um |
| Ton | Klänge mischen, 3D-Klang | das Quaken wird lauter, wenn die Ente näher kommt |
| Spielwelt | Levels, Spieler, Regeln | Orb einsammeln gibt Punkte |
| Logik, Netzwerk | Spielabläufe, Mehrspieler | Tür geht auf, wenn du davorstehst |
| Studio, Lehrer | Editor und Trainer | Welt bauen, Ente trainieren |
| Kellner (JavaScript) | Brücke zum Browser | Tastendruck weitergeben |

## 1.2 Kerns Regeln

Kern folgt drei großen Regeln.

**Kein fremder Motor fürs Zeichnen.** Kern benutzt keine fremde Grafik-Engine. Der Code, der
zeichnet, ist Kerns eigener und spricht die Grafikkarte direkt an. So weiß man bei jedem Bild genau,
woher es kommt. Zwei Bauteile stammen allerdings aus freien Projekten anderer Leute: die Physik
namens **Box3D** ([Kapitel 6](06-physik.md)) und eine Mathe-Bibliothek ([Kapitel 3](03-grundlagen.md)).
Kern baut die Physik **unverändert** ein und übersetzt beide auf jedem Rechner auf genau dieselbe
Weise.

Außerdem benutzt Kern nur Fähigkeiten, die fast jeder Browser hat. Lieber verzichtet es auf ein paar
neue Tricks, als dass es auf älteren Geräten nicht läuft.

**JavaScript ist nur der Kellner.** Grafikkarte, Tasten, Töne und Dateien erreicht im Browser nur
die Programmiersprache JavaScript. Genau das – und nur das – erledigt Kerns JavaScript-Teil. Was ein
Spiel tut, steht in C++.

> **So kannst du es dir vorstellen:** Der Kellner bringt die Bestellung in die Küche und das Essen
> an den Tisch. Kochen tut er nicht. JavaScript ist der Kellner, C++ die Küche. Und er arbeitet
> sparsam: Bei jedem Bild nimmt er dasselbe Tablett, statt jedes Mal ein neues zu holen.

Der Vorteil: C++ kann man auch direkt auf dem Computer laufen lassen, ganz ohne Browser. JavaScript
braucht dagegen den Browser. Weil alles, was ein Spiel tut, in C++ steht, lässt es sich auch ohne
Browser testen (das zeigt [Kapitel 2](02-bauen-und-testen.md)).

**Überall exakt gleich.** Eine Simulation kommt auf einem PC, einem Mac und im Browser **Bit
für Bit** gleich heraus. Das heißt **Determinismus**. Warum ist das schwer? Computer runden. Teil
1 durch 3, und du bekommst 0,3333… ohne Ende. Irgendwo muss der Computer aufhören und runden. Rundet
ein Rechner an der allerletzten Stelle ein winziges bisschen anders als ein anderer, merkt man das
zuerst gar nicht. Aber eine Simulation rechnet Tausende Schritte, und jeder baut auf dem vorigen
auf. So schaukelt sich der winzige Unterschied auf, bis die Ente nach ein paar Sekunden den Fuß
woanders hinsetzt.

> **So kannst du es dir vorstellen:** Stille Post. Flüstert der Erste nur einen Laut ein bisschen
> anders, kommt am Ende der Reihe ein ganz anderer Satz an. Kern sorgt dafür, dass jeder Rechner
> exakt gleich flüstert.

Wie Kern das schafft, steht in [Kapitel 3](03-grundlagen.md). Wie man es beweist, mit einem
**Fingerabdruck** der Simulation, steht in [Kapitel 2](02-bauen-und-testen.md).

Wozu der Aufwand? Läuft etwas schief, läuft es auf jedem Rechner genau gleich schief. Dann kann man
den Fehler immer wieder herbeiführen, finden und reparieren.

> **Krass, aber wahr:** Weil Kern so exakt rechnet, liegt ein Unterschied zwischen dem echten Roboter
> und der Simulation nie am Zufall. Er zeigt, dass die Beschreibung der Ente im Computer an einer
> Stelle noch nicht stimmt – und genau das will man wissen ([Kapitel 16](16-kalibrierung.md)).

**Noch vier kleine Regeln:**

- **Erst prüfen, dann ändern.** Kern prüft alles, was von außen kommt. Eine kaputte Eingabe ändert
  nichts am Bestehenden.
- **Keine Schummelei.** Die Micro Duck steht nur durch die Motoren in ihren Gelenken und ihren
  Kontakt zum Boden. Keine unsichtbare Hand hält sie fest. Warum so streng? Eine Ente, die nur dank
  einer unsichtbaren Hand steht, würde im Computer laufen und in Wirklichkeit umfallen. Was man in
  der Simulation lernt, soll aber auch für den echten Roboter gelten.
- **Eine Welt für alle.** Editor, exportiertes Spiel und Lehrer benutzen dieselbe Spielwelt,
  dieselben Motoren und dieselbe Physik.
- **Nicht umfallen, wenn die Grafikkarte kurz weg ist.** Manchmal nimmt der Browser einer Webseite
  die Grafikkarte für einen Moment weg. Dann pausiert Kern und baut danach alles neu auf, aus Kopien,
  die es selbst aufbewahrt hat ([Kapitel 4](04-grafik.md)).

> **Für Profis:** Kern verzichtet bewusst auf die Grafik-Engine three.js und den Übersetzer
> Emscripten. Das Modul läuft ab Safari 15 auf jedem Browser mit WebGL2. Die genauen Namen und
> Werte stehen im blauen Buch, Kapitel 1.

## 1.3 Der Bauplan: Stockwerke

Kern ist wie ein Hochhaus: Jedes Stockwerk benutzt nur die darunter, nie die darüber. Die Grafik
weiß also nichts von Spielregeln, aber die Spielregeln können die Grafik benutzen. Im Bild steht
jedes Stockwerk auf denen, die es benutzt. Oben, vor dem Haus, steht der Kellner; die Pfeile zeigen,
was durch die Tür hinein- und hinausgeht.

![Das Hochhaus Kern: oben vor dem Haus der Kellner, im Haus die Tür und die Stockwerke bis zu den Grundlagen](../abbildungen/architektur.svg)

Der Kellner ist **kein Stockwerk**. Er steht vor dem Haus an der Tür, bringt Bestellungen hinein und
Fertiges hinaus. Die Hochhausregel gilt für die Stockwerke im Haus, nicht für ihn.

Nicht jedes Stockwerk ist immer eingebaut. Physik, Spielregeln, Studio und Lehrer kommen nur dazu,
wenn man sie anfordert. Wer Studio will, bekommt Spielregeln und Physik automatisch mit, weil Studio
darauf steht. Grafik, Ton, Logik und Netzwerk sind immer dabei.

Zwischen C++ und JavaScript gibt es für Daten nur **eine Tür**: einen Speicherbereich, auf den beide
Seiten zugreifen dürfen.

> **So kannst du es dir vorstellen:** eine Wand voller nummerierter Schließfächer. Der Browser legt
> Zahlen in die Fächer und sagt der Engine nur: „Schau ab Fach 1000 nach.“ Ein Text geht als „ab
> Fach 2000, so und so viele Zeichen“. Antworten legt die Engine genauso in Fächer zurück.

Weil der Kellner kein Stockwerk ist, darf die Küche ihn auch rufen: Die Engine bittet den Browser
etwa „zeichne das“ oder „wie spät ist es?“. Das verletzt die Hochhausregel nicht.

> **Vorsicht, Falle:** Beide Seiten müssen genau wissen, in welchem Fach welche Zahl liegt. Ändert
> man die Reihenfolge nur auf einer Seite, liest die andere Unsinn. Die Liste aller Fächer
> steht in [Kapitel 17](17-referenz.md) und im blauen Buch, Kapitel 1.

## 1.4 Vier Formen, ein Code

Derselbe C++-Code von Kern steckt in vier verschiedenen Dingen. Je nach Zweck werden andere
Stockwerke zusammengestellt.

| Form | Was es ist | Beispiel |
|---|---|---|
| Spiel ganz in C++ | ein Spiel, das Kern direkt benutzt | die Beispiele Hello und Orb Run |
| Nur die Grafik | eine Webseite, die mit Kern zeichnet | SpawnCity |
| Studio | der Editor, auch für fertige Spiele | dein eigenes Level |
| Lehrer | der Trainer für die Ente | ein neues Sprung-Netz |

**Ein Spiel ganz in C++.** Beim Start sagt das Spiel der Engine, was es braucht, zum Beispiel:
„Ich will Schatten.“ Danach fragt der Browser das Spiel bei jedem Bild, das der Bildschirm zeigt:
„Was ist jetzt passiert?“ Das Spiel bewegt seine Dinge, und die Engine zeichnet das neue Bild. Diese
Runde heißt **Spielschleife** ([Kapitel 12](12-eigene-spiele.md)).

**Nur die Grafik.** So nutzt SpawnCity Kern: Eine Webseite lädt Kern und lässt es nur zeichnen
([Kapitel 4](04-grafik.md)).

**Studio und exportierte Spiele.** Das ganze Studio steckt in **einer** einzigen Datei, die sogar
ohne Internet per Doppelklick läuft. **Datei › Spiel verpacken (HTML)...** (File › Package Game (HTML)...) macht aus deinem Level
ebenfalls eine einzige Datei. Darin spielt dein Level mit genau demselben C++-Code wie im Editor
([Kapitel 11](11-studio.md)).

**Der Lehrer.** Er trainiert Netze für die Micro Duck: als eigenes Programm, das einen ganzen Computer
zum Rechnen nutzt, oder direkt im Browser. Startet man ihn zweimal genau gleich, kommt beide Male Bit für
Bit dasselbe Netz heraus. Studio lädt dieses Netz wie die Netze von Pollen
([Kapitel 14](14-lehrer.md)).

In der Micro Duck greift alles ineinander: Physik für ihre Glieder, Motoren in den Gelenken, Netze
fürs Gehirn, Ton für ihr Quaken, Logik für Befehle – und im Studio ist sie ein Ding im Level wie jedes
andere.

> **Mission:** Stoppe ein laufendes Spiel mit **Esc**, denn beim Spielen sind die Menüeinträge
> gesperrt. Lade **Datei › Beispiel-Levels › Micro Duck** (File › Example Levels › Micro Duck) und
> drück **Spielen** (Play). Erst schaust du zu,
> wie die Ente allein herumläuft. **F8** übernimmt sie: **W** läuft, **A**/**D** drehen, die
> **Leertaste** quakt.

> **Mission:** Öffne von der Startseite aus den **Lehrer**. Wähle bei **Bewegung** „Stehen
> (Schnelltest)“ und drück **Start**. Beobachte die Kurve **Episodenlänge**. Eine **Episode** ist
> ein Versuch, und er endet, sobald eine Ente stürzt. Steigt die Kurve, halten die Enten länger durch – das Netz lernt gerade.
> Mit **Pause** hältst du an.

## 1.5 Wo alles liegt

Der ganze Quellcode von Kern ist **offen** und liegt auf
https://github.com/ChristianHohlfeld/kernengine.de – jeder darf hineinschauen, daraus lernen,
Fehler finden und Kern unter den Regeln der **Lizenz** (den Spielregeln, was man mit dem Code machen
darf) für eigene Projekte benutzen. Und jeder kann
nachprüfen, ob das, was dieses Buch erzählt, wirklich so im Code steht.

> **Für Profis:** Die Engine liegt im Ordner `kern/`, darin Code, Tests, Beispiele, die
> JavaScript-Schicht und die Bücher. Die Lizenz ist die Apache License 2.0. Den ganzen Ordnerbaum
> erklärt das blaue Buch, Kapitel 1.

## 1.6 So spielst du dieses Buch

**Jedes Kapitel ist ein Level.** Oben steht **Dein Level**, gleich darunter **Wörter, die du
brauchst**. Es geht von unten nach oben wie im Hochhaus: Bauen und Grundlagen, Grafik bis Netzwerk,
Studio und eigene Spiele, Micro Duck bis Kalibrierung, zum Schluss ein
[Spickzettel](17-referenz.md).

| Kasten | Was drinsteht |
|---|---|
| **Mission** | Aufgabe im echten Studio |
| **So kannst du es dir vorstellen** | Vergleich aus dem Alltag |
| **Krass, aber wahr** | verblüffende Tatsache |
| **Vorsicht, Falle** | typischer Fehler |
| **Für Profis** | Ausblick für Neugierige |

Am Ende: **Level geschafft!** und die **Rätsel-Ecke**.

**Dein Leseweg.** Drei Bonuslevel sind deine **Nachschlagehilfe**: Anhang
[A](A-grundlagen-neuronale-netze.md) erklärt, wie Netze lernen, Anhang
[B](B-grundlagen-rendering.md), wie aus Dreiecken Bilder werden, und Anhang
[C](C-grundlagen-physik.md), wie eine Physiksimulation denkt. Du musst sie nicht zuerst lesen.
Klemmt es aber, schlag dort nach. Ein guter Tipp: Lies Anhang B vor
[Kapitel 4](04-grafik.md), Anhang C vor [Kapitel 6](06-physik.md) und Anhang A vor
[Kapitel 14](14-lehrer.md).

Du willst gleich loslegen? Für eigene Spiele sind [Kapitel 11](11-studio.md) und
[Kapitel 12](12-eigene-spiele.md) dein Start. Für die Ente liest du [Kapitel 13](13-micro-duck.md),
[14](14-lehrer.md) und [16](16-kalibrierung.md).

**Die Konsole „Cmd“.** In Studio gibt es ganz unten in der Statusleiste ein kleines Eingabefeld mit
der Aufschrift **Cmd**. Das ist die **Konsole**: Dort tippst du kurze Befehle, statt im Menü zu
suchen. Die Antwort erscheint daneben als Meldung in der Statusleiste. Mit ↑ und ↓ holst du frühere
Eingaben zurück, **Esc** verlässt das Feld. Ein paar Missionen in diesem Buch benutzen sie.

> **Mission:** Klick unten auf **Cmd**, tipp `help` und drück Enter: Studio zeigt alle Befehle. Tipp
> dann `tod 0.75` – die Tageszeit springt auf Sonnenuntergang. Mit `tod 0.5` ist wieder Mittag.
> „tod“ hat nichts mit dem Tod zu tun: Es steht für *time of day*, englisch für Tageszeit. Der
> ganze Tag ist 1. Die Hälfte, 0,5, ist also 12 Uhr, drei Viertel, 0,75, sind 18 Uhr. Getippt wird
> mit **Punkt** statt Komma, denn Computer schreiben Kommazahlen auf Englisch: `0.75`, nicht `0,75`.

**Drei Bücher, ein Inhalt.** Du liest das **gelbe Buch**. Das **grüne** erklärt alles einfach für
Erwachsene ohne Vorwissen, das **blaue** geht technisch bis ins letzte Detail und ist die Quelle für
alle drei. Packt dich ein Thema, schlag im blauen Buch nach!

## Level geschafft!

- Du weißt, was Engine, Studio, Micro Duck, Netz und Lehrer sind.
- Du weißt, warum der Browser eine Übersetzung von Kern braucht.
- Du weißt, warum JavaScript nur der Kellner ist.
- Du weißt, warum Kern auf jedem Rechner Bit für Bit gleich rechnen muss.
- Du kennst das Hochhaus: Oben benutzt unten, nie andersherum.
- Du hast Orb Run gespielt, die Micro Duck gesteuert und die Konsole benutzt.

## Rätsel-Ecke

1. Warum läuft Kern nicht direkt als C++ im Browser?
2. Warum darf die Engine nichts über ein bestimmtes Spiel wissen?
3. **Boss-Rätsel:** Warum läuft ein im Lehrer trainiertes Netz im Studio genauso?

### Lösungen

1. Browser verstehen kein C++. Darum wird Kern vorher nach WebAssembly übersetzt, und das versteht
   jeder moderne Browser.
2. Wüsste sie etwas von einem Spiel, würde sie sich darauf verlassen und passte nur zu diesem einen.
   So aber passt sie wie ein Legostein unter beliebig viele Spiele. Wie im Hochhaus benutzt das
   Spiel oben die Engine unten, nie umgekehrt.
3. Lehrer und Studio benutzen dieselbe Spielwelt, dieselben Motoren und dieselbe Physik, und Kern
   rechnet überall Bit für Bit gleich. Das Netz trifft also auf genau dieselbe Welt, in der es
   geübt hat.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
