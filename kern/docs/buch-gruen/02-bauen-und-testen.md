# 2 Bauen und Testen: die Werkstatt hinter Kern

> **Kurz gesagt:** Hinter Kern stehen drei Prinzipien. Erstens: Der Code wird **übersetzt**, bevor
> er im Browser laufen kann. Zweitens: Hunderte **automatische Prüfungen** bewachen jede Änderung.
> Drittens: Ein **Fingerabdruck** beweist, dass Kern überall exakt gleich rechnet. Ein Fließband
> erledigt alle drei bei jeder Änderung von selbst.

> **Begriffe vorab:**
> - **Quelltext** – der Text eines Programms, so wie Menschen ihn schreiben, hier in C++.
> - **Header** – eine Art Inhaltsverzeichnis im Quelltext, das viele Programmteile gemeinsam lesen.
> - **Speicher** – der Arbeitsplatz eines Programms im Rechner, eine lange Reihe nummerierter Fächer
>   für Zahlen.
> - **Simulation** – eine nachgerechnete Welt: Der Computer berechnet Schritt für Schritt, wie sich
>   Dinge bewegen.
> - **Takt** – ein einzelner Rechenschritt der Simulation. Die Ente entscheidet in jedem Takt neu,
>   wie sie ihre Motoren bewegt.
> - **Server** – ein Computer im Internet, der Aufgaben für andere erledigt.
> - **GitHub** – eine Webseite, auf der der Quelltext von Kern liegt und jede Änderung aufgezeichnet
>   wird.

Stell dir eine Werkstatt mit drei Stationen vor: An der ersten wird gekocht (übersetzt), an der
zweiten wird abgeschmeckt (geprüft), an der dritten wird das Ergebnis mit einem Muster verglichen
(Fingerabdruck). Am Ende steht die Auslieferung: die Webseite https://kernengine.de/. Dieses Kapitel
geht die Stationen der Reihe nach durch.

## 2.1 Übersetzen: vom Rezept zum Gericht

Ein Programm wird in einer Sprache für Menschen geschrieben. Bevor ein Browser es ausführen kann,
muss es in dessen Befehle übersetzt werden. Bei Kern heißt das: Aus dem C++-Quelltext wird ein
WebAssembly-**Modul**, eine einzige Datei, die der Browser lädt.

> **Stell dir vor …** Der Quelltext ist das Rezept, das **Bauskript** ist der Koch, das Modul ist
> das fertige Gericht. Beim Bestellen sagst du, welche Gänge du willst: nur die Grundausstattung
> (Malen, Ton, Logik, Netzwerk), oder dazu Physik, Spielregeln, den Editor oder den Trainer.

Der Koch hat zwei feste Hausregeln. Erstens: **keine Rechen-Abkürzungen.** Manche Prozessoren
können zwei Rechenschritte zu einem verschmelzen und runden dabei anders als andere. Das ist
verboten, damit Kern überall gleich rechnet ([Kapitel 1](01-einfuehrung.md)). Zweitens: Nur was
ausdrücklich nach außen gereicht werden soll, ist von außen sichtbar, und Ungenutztes fliegt raus.

### Der Vorratsschrank

Kern besteht aus vielen Dutzend Quelltext-Stücken. Jedes wird einzeln übersetzt, viele
gleichzeitig, und am Ende zu einem Modul zusammengefügt. Damit nicht bei jedem Bauen alles neu gekocht wird, kommt
jedes übersetzte Stück in einen **Vorratsschrank**. Es trägt ein Etikett, das aus seinem Rezept und
dem Inhalt **aller** Header errechnet wird. Passt das Etikett noch, wird das Stück aus dem Schrank
genommen statt neu gekocht. Studio und Beispielspiele teilen sich den Schrank, und das Fließband
aus Abschnitt 2.4 hebt ihn von Lauf zu Lauf auf.

> **Achtung, Falle!** Du änderst ein einziges Komma in einem Header – und **alles** wird neu
> übersetzt? Absicht! Weil alle Header ins Etikett eingehen, passt nach jeder Header-Änderung kein
> Vorratsstück mehr. Das kostet Zeit, ist aber einfach und sicher: Nie bleibt ein veraltetes Teil
> übrig.

### Studio im Koffer

Ein zweites Skript packt Kern Studio in **eine einzige** HTML-Datei: das Modul, die
JavaScript-Schicht und das Aussehen der Seite, alles in einem Koffer. Das Modul besteht eigentlich
aus Rohdaten; damit es in eine Textdatei passt, wird es als lange Buchstabenfolge verkleidet,
ähnlich wie Morsezeichen Buchstaben als Punkte und Striche verkleiden. Deshalb läuft das Studio per
Doppelklick, ohne Internet und ohne Installation.

### Der Lehrer ohne Bildschirm

Den Lehrer kann man auch als eigenes, schnelles Programm für den eigenen Rechner bauen. Er braucht
keinen Bildschirm, denn Training muss nichts malen. Darum läuft er sogar auf einem Server ohne
Monitor. Wie man ihn bedient, zeigt [Kapitel 14](14-lehrer.md).

Die genauen Namen und Werte stehen im blauen Buch, Kapitel 2.

## 2.2 Automatisch prüfen: hunderte kleine Kontrollen

Ein **Test** ist ein kleines Programm, das ein Stück Kern ausprobiert und nachsieht, ob das
Richtige herauskommt. Kern hat weit über hundert davon, jeder mit vielen einzelnen Prüfungen, und
sie laufen bei jeder Änderung. Wer etwas kaputt macht, merkt es sofort – nicht erst, wenn ein
Spieler es findet.

So klingen Prüfungen in Alltagssprache:

- Startet das Studio mit Orb Run?
- Macht Undo eine Änderung exakt rückgängig?
- Rechnet Kern das Netz der Ente genauso wie die Vorlage von Pollen Robotics?
- Steht die Ente, läuft sie und dreht sie sich, wenn ihr Netz sie steuert?

Jede Prüfung meldet „ok“ oder „FAIL“. Ein einziges FAIL genügt, und der ganze Lauf gilt als
gescheitert.

### Spürhunde

Beim Prüfen wird Kerns eigener Code mit eingebauten **Spürhunden** übersetzt. Einer bellt, wenn ein
Programmteil in ein Speicherfach greift, das ihm nicht gehört. Der andere bellt, wenn der Code etwas
tut, was C++ verbietet. Beide brechen sofort ab und nennen die Stelle. Fremder Code, den Kern
unverändert übernimmt, läuft ohne Spürhunde – geprüft werden soll Kerns eigener. Außerdem sind die
Tests strenger als der normale Bau: Schon jede **Warnung** des Übersetzers gilt als Fehler.

### Die Attrappe

Auf dem Prüfrechner gibt es keine Grafikkarte, die ein Browser ansteuern könnte. Darum ersetzt eine
**Attrappe** die Grafikschnittstelle: Sie malt nichts, schreibt aber genau auf, was Kern von ihr
wollte.

> **Stell dir vor …** Ein Pilot übt im Flugsimulator. Kein Flugzeug hebt ab, aber jeder Knopfdruck
> wird aufgezeichnet. Hinterher sieht man genau: Kam das Fahrwerk rechtzeitig rein?

So prüfen die Tests ohne Grafikkarte, ob das Malen kaputte Eingaben abweist, einmal Berechnetes
wiederverwendet und den Verlust der Grafikkarte übersteht.

### Spielregeln: drei Wege, ein Ergebnis

Spielregeln, die man im Studio als Kästchen und Pfeile zusammenklickt ([Kapitel 9](09-logik.md)),
kann Kern auf drei Arten ausführen. Alle drei müssen exakt dasselbe Ergebnis liefern, und
gemeinsame Testfälle prüfen genau das.

> **Stell dir vor …** Eine Geschichte kann man Satz für Satz vom Zettel vorlesen (langsam, aber
> sofort bereit). Man kann sie kurz vor dem Auftritt in die eigene Sprache übertragen und dann
> flüssig vortragen (schnell, sobald es losgeht). Oder man lässt sie als fertiges Buch drucken. Wer
> zuhört, muss jedes Mal dieselbe Geschichte hören.

Fällt der schnelle Weg einmal aus, liest Kern einfach Satz für Satz weiter – langsamer, aber nichts
geht kaputt.

> **Wusstest du?** Am Ende dieses Tests läuft ein Wettrennen: eine Million Schleifendurchläufe,
> einmal vorgelesen, einmal übersetzt. Ausgegeben wird, wie viel schneller der übersetzte Weg ist.

### Der Testklicker

Die letzte Prüfung ist die größte: Ein ferngesteuerter Browser bedient das Studio wie ein
unermüdlicher Mensch. Er öffnet Menüs, verschiebt Dinge, drückt Play, packt ein Spiel und spielt
es, er tut sogar so, als wäre er ein Telefon. Eine echte Grafikkarte bekommt er nicht, sondern eine
nachgebildete, die die Stadt von Orb Run mit nur etwa **einem Bild pro Sekunde** schafft. Darum
wartet jede Prüfung, bis ihre Wirkung sichtbar ist, statt eine feste Zeit.

Ob überhaupt etwas gemalt wurde, verrät ein einfacher Trick: Der Testklicker zählt, wie viele
**verschiedene Farben** im Bild vorkommen. Ein leeres Bild hat kaum Farben, ein gemaltes viele.
Nebenbei kann er Bildschirmfotos machen – viele Bilder dieses Buchs stammen daher.

![Simulate mit Physik-Ansicht, wie der Testklicker es prüft](../buch/bilder/studio-simulate.png)

> **Probier's aus!** Mach, was der Testklicker macht. Wähle **File → New Level → Physics Lab**.
> Halte die rechte Maustaste gedrückt und flieg mit W nach vorn. Dann **Simulate** (Alt+S): Jetzt
> rechnet die Physik: Das Pendel schwingt, der Drehteller dreht sich, der Rover fährt los. Der
> Testklicker prüft genau solche Abläufe – bei jeder Änderung und ohne müde zu werden.

Die genauen Namen und Werte stehen im blauen Buch, Kapitel 2.

## 2.3 Der Fingerabdruck: überall dasselbe Ergebnis

Wie beweist man, dass Kern auf jedem Rechner **exakt** gleich rechnet? Man lässt alle Rechner
dasselbe tun und vergleicht – aber nicht nur das Endergebnis, sondern jeden einzelnen Schritt.

Dafür braucht man eine **Prüfsumme**: eine Zahl, die aus vielen Daten errechnet wird und sich völlig
ändert, sobald sich auch nur eine Kleinigkeit ändert – wie ein Fingerabdruck. Dieselben Daten
ergeben immer dieselbe Zahl.

Der Test geht so: Die Micro Duck läuft in der Physik des Studios ein Stück geradeaus und dreht sich
dann auf der Stelle, gesteuert von einem Netz von Pollen Robotics. Nach **jedem** Takt wird ihr
ganzer Zustand – wo sie steht, wie sie gedreht ist, wie schnell sie sich bewegt, wie jedes Gelenk
steht – zu einer Prüfsumme. 400 Takte ergeben eine Liste von 400 Prüfsummen. Diese Liste muss auf
einem Linux-PC, auf einem Mac mit Apple-Chip und im WebAssembly-Modul, das auch der Browser benutzt,
**Zeile für Zeile** mit einer gespeicherten Vorlage übereinstimmen.

> **Stell dir vor …** Du fotografierst die Ente 400-mal und vergleichst jedes Foto mit einem
> Referenzalbum. Verrutscht eine Feder um ein Haar, weißt du sogar, auf welchem Foto.

Weicht ein Foto ab, gibt es nur zwei Erklärungen: Entweder wurde die Simulation absichtlich
geändert – dann muss die Vorlage neu erzeugt und die Änderung begründet werden. Oder irgendwo rechnet
Code auf verschiedenen Rechnern verschieden, etwa weil er eine Winkelrechnung des Systems statt
Kerns eigener benutzt ([Kapitel 3](03-grundlagen.md)).

Ein zweiter Test prüft das Lernen: Er trainiert ein Netz mit einem, zwei und vier Helfern, die
zusammen immer acht Enten betreuen. Alle drei Netze müssen bis aufs letzte Bit gleich sein. Das
klappt nur, weil Kern immer in derselben Reihenfolge zusammenzählt, egal wie viele Helfer
mitrechnen.

> **Für Neugierige:** Die Prüfsumme heißt FNV-1a und ist 16 Zeichen lang. Wer den Lehrer auf dem
> eigenen Rechner baut, kann den Fingerabdruck selbst erzeugen und mit der Vorlage vergleichen –
> wie, steht im blauen Buch, Kapitel 2.

## 2.4 Das Fließband: die automatische Prüfstraße

Niemand muss daran denken, die Prüfungen zu starten. Bei jeder Änderung auf GitHub springt ein
**Fließband** an, auf Englisch **CI** (*Continuous Integration*, „ständiges Zusammenführen“). Server
von GitHub erledigen dann alles von selbst: übersetzen, prüfen, Fingerabdruck vergleichen,
veröffentlichen. Ein Lauf dauert etwa 17 Minuten.

```mermaid
flowchart LR
  P["Änderung"] --> N["Tests mit Spürhunden"]
  P --> D["Fingerabdruck auf dem Mac"]
  P --> B["Bauen und Grundprüfungen"]
  B --> DEP["Veröffentlichen"]
  B --> E["Testklicker"]
```

Die Stationen:

- **Tests mit Spürhunden:** alle Prüfungen aus Abschnitt 2.2 auf einem Linux-Rechner.
- **Fingerabdruck auf dem Mac:** Der Lehrer wird auf einem Mac mit Apple-Chip gebaut und muss die
  Ente exakt wie die anderen bewegen.
- **Bauen und Grundprüfungen:** übersetzt Studio, Beispielspiele und Lehrer und prüft dabei schon
  einiges: ob alle JavaScript-Dateien fehlerfrei geschrieben sind, ob der Lehrer im Browser-Modul
  ein kleines Stück jedes Lernverfahrens schafft und den richtigen Fingerabdruck liefert, und ob die
  Spielregeln übersetzt so rechnen wie vorgelesen. Dann stellt es die fertige Webseite zusammen.
- **Veröffentlichen:** stellt die Webseite auf https://kernengine.de/.
- **Testklicker:** bedient das Studio mit genau den Dateien, die veröffentlicht werden.

Wichtig ist, was **wartet** und was nicht. Veröffentlicht wird, sobald die Station „Bauen und
Grundprüfungen“ fertig ist. Scheitert dort eine Prüfung, geht nichts online. Die drei anderen
Prüfstationen laufen dagegen **nebenher**: Die Veröffentlichung wartet nicht auf sie. Darum heißt
es: Diese Tests halten die Veröffentlichung nicht auf. Fällt eine von ihnen durch, ist die neue
Fassung vielleicht schon online – aber auf GitHub leuchtet sofort eine rote Markierung, und die
nächste Änderung muss den Fehler beheben.

> **Stell dir vor …** Eine Bäckerei. Bevor ein Brot in den Laden kommt, muss es durchgebacken sein –
> das ist die Pflicht an der Backstation. Die Verkostung durch die Meisterin läuft parallel zum
> Verkauf. Schmeckt ihr etwas nicht, wird sofort nachgebessert, aber das Brot im Regal bleibt dort.

Der Vorteil: Jede Änderung ist schnell online. Der Preis: Ein Fehler, den nur die nebenher
laufenden Tests finden, kann kurz sichtbar sein. Änderungsvorschläge, die erst noch begutachtet
werden, laufen übrigens ebenfalls über das Fließband, werden aber nie veröffentlicht.

> **Probier's aus!** Im Studio steht in der Statusleiste ganz unten die **Version**, etwa „v 1af26d8“:
> das Kürzel der Änderung, aus der das Fließband diese Fassung gebaut hat. Tippe dann ins Feld **Cmd** in der Statusleiste
> `stat` und drück Enter. Öffne mit dem Knopf **Output Log** das Protokoll: Dort steht jetzt, wie
> viele Bilder pro Sekunde dein Rechner schafft. Vergleich das mit dem Testklicker, der sich mit
> einem Bild pro Sekunde begnügen muss.

> **Wusstest du?** Der installierbare Editor merkt selbst, wenn das Fließband eine neue Version
> veröffentlicht hat: Neben jeder Fassung liegt eine kleine Datei mit ihrer Kennung, und er
> vergleicht.

Wie man das ganze Fließband zu Hause nachstellt und was typische Fehlermeldungen bedeuten, steht im
blauen Buch, Kapitel 2.

## Das nimmst du mit

- Code wird übersetzt, bevor er im Browser läuft; der Bau wählt, welche Teile mitkommen.
- Ein Vorratsschrank mit Etiketten spart Zeit; nach einer Header-Änderung wird bewusst alles neu
  übersetzt.
- Weit über hundert Tests prüfen jede Änderung. Spürhunde finden verbotene Speicherzugriffe, eine Attrappe
  ersetzt die Grafikkarte, ein Testklicker bedient das Studio wie ein Mensch.
- Spielregeln müssen auf allen drei Wegen dasselbe ergeben.
- Der Fingerabdruck vergleicht 400 Takte der Ente auf PC, Mac und im Browser-Modul.
- Das Fließband prüft jede Änderung und veröffentlicht; nur die Bau-Station hält die
  Veröffentlichung an, die übrigen Tests laufen nebenher.

## Knobelecke

1. Warum laufen die Spürhunde nur über Kerns eigenen Code, nicht über übernommenen fremden Code?
2. Du änderst einen Header. Was passiert beim nächsten Bauen, und warum?
3. Warum zählt der Testklicker Farben, statt Bildschirmfotos zu vergleichen?
4. Der Fingerabdruck stimmt bis Takt 211 und weicht ab Takt 212 ab. Was sagt dir das?
5. Die Tests mit Spürhunden schlagen fehl. Ist die Webseite trotzdem aktualisiert worden?

### Lösungen

1. Geprüft werden soll Kerns eigener Code; der fremde wird unverändert übernommen.
2. Alles wird neu übersetzt, denn der Inhalt aller Header steckt in jedem Etikett. Kein veraltetes
   Vorratsstück kann übrig bleiben.
3. Die nachgebildete Grafik ist langsam; die Zahl der Farben verrät einfach und zuverlässig, ob
   überhaupt etwas gemalt wurde.
4. Bei Takt 212 kommt zum ersten Mal etwas anderes heraus. Dort setzt die Fehlersuche an, etwa bei
   einer Winkelrechnung, die nicht Kerns eigene Formel benutzt.
5. Ja, wenn die Bau-Station geklappt hat: Diese Tests laufen nebenher und halten die
   Veröffentlichung nicht auf. Die rote Markierung zeigt aber sofort, dass etwas zu reparieren ist.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
