# 5 Szene und Geometrie – Was die Kamera sieht und woraus die Stadt gebaut ist

> **Kurz gesagt:** Neben dem Renderer – dem Teil von Kern, der das Bild malt – liegen zwei
> Werkzeugkisten. Die erste beantwortet Fragen wie „Muss ich das überhaupt malen?“ und „Was liegt
> unter dem Mauszeiger?“. Die zweite baut Formen: Würfel, Häuser aus Grundrissen, Straßen, Gelände.
> Am Ende entsteht daraus eine ganze Stadt – aus einer einzigen Startzahl.

> **Begriffe vorab:**
> - **Dreiecksnetz** – jede 3D-Form auf dem Bildschirm ist in Wahrheit ein Netz aus lauter kleinen,
>   flachen Dreiecken, wie ein Drahtgitter, das man mit Stoff bespannt.
> - **Mesh** – so ein Dreiecksnetz als fertiges Ding im Speicher: ein Auto, ein Haus, ein Baum.
> - **Ecke** – ein Eckpunkt des Netzes. Jede Ecke trägt ein paar Angaben: wo sie ist, welche Farbe
>   sie hat, wohin die Fläche dort schaut.
> - **Normale** – ein unsichtbarer Pfeil, der senkrecht aus einer Fläche zeigt: „hierhin schaue
>   ich“. Das Licht braucht ihn, um zu wissen, wie hell die Fläche ist.
> - **UV** – die Angabe, welche Stelle eines Bildes (einer **Textur**, etwa Ziegelmuster) auf eine
>   Ecke geklebt wird, wie die Faltmarken auf Geschenkpapier.
> - **Index** – eine Nummer, die auf eine Ecke zeigt. Statt jede Ecke für jedes Dreieck neu
>   aufzuschreiben, notiert man „Dreieck aus Ecke 3, 7 und 8“.
> - **Instanz** – eine Kopie desselben Meshes an einem anderen Ort, etwa hundert gleiche Bäume.
> - **Seed** – eine Startzahl für den Zufall. Dieselbe Zahl ergibt immer dieselben „Zufälle“.

Wie der Renderer malt, zeigt [Kapitel 4](04-grafik.md); die Mathematik dahinter [Anhang B](B-grundlagen-rendering.md).

## 5.1 Die Spielregeln

Damit alle Teile von Kern zusammenpassen, gelten überall dieselben Abmachungen: **Z zeigt nach
oben**, und gemessen wird in **Metern** – in Grafik, Physik, Ton und in allen Formbauern.

> **Stell dir vor …** eine Baustelle, auf der jeder Handwerker ein anderes Maßband benutzt: der eine
> in Zoll, der andere in Zentimetern, und für den dritten ist „oben“ dort, wo die anderen „Norden“
> sagen. Nichts würde passen. Kern hat ein Maßband für alle.

Alle Formbauer liefern ihr Ergebnis im **selben Behälter**: eine Liste von Ecken, und zu jeder Ecke
Ort, Normale, Farbe und UV. So kann der Renderer jedes Ergebnis gleich behandeln, egal ob es ein Haus
oder eine Straße ist.

Es gibt zwei Arten, die Dreiecke aufzuschreiben:

- **Dreieckssuppe:** je drei Ecken hintereinander sind ein Dreieck. Einfach, aber eine Ecke, an der
  sechs Dreiecke zusammenstoßen, steht sechsmal in der Liste.
- **Mit Index:** jede Ecke steht einmal da, und eine Nummernliste sagt, welche drei Ecken ein Dreieck
  bilden. Das spart Platz (mehr dazu in 5.7).

Wer einen eigenen Spezialeffekt malen will, kann jeder Ecke noch ein paar Extra-Zahlen mitgeben. So
bekommen die Häuser später ihre leuchtenden Fenster (5.8).

> **Probier's aus!** Öffne Kern Studio auf https://kernengine.de/, hole dir mit dem Content Drawer, der
> Bauteil-Kiste (Ctrl+Space), aus der Gruppe **Shapes** eine **Sphere** und schalte mit **Alt+2** auf
> **Wireframe**. Die glatte Kugel zerfällt in ein Netz aus vielen kleinen, flachen Flächen. Das ist
> ihr Dreiecksnetz. Mit **Alt+4** kommst du zurück zum normalen Bild.

## 5.2 Frustum-Culling: Was nicht im Bild ist, wird nicht gemalt

Die Kamera sieht nur einen Ausschnitt der Welt, geformt wie eine liegende Pyramide mit
abgeschnittener Spitze: das **Frustum**. Was ganz außerhalb liegt, muss nicht gemalt werden. Dieses
Aussortieren heißt **Culling**. Es spart viel Arbeit, denn in einer Stadt liegt meist der größte Teil
hinter oder neben dir.

> **Stell dir vor …** du stehst im Theater mit einer Taschenlampe. Bevor du jemanden anleuchtest,
> fragst du: Steht er überhaupt auf der Bühne? Wer hinter dem Vorhang wartet, kostet dich nichts.

Der Trick: Der Test muss viel billiger sein als das Malen, sonst lohnt er nicht. Kern prüft darum
nicht jedes Dreieck, sondern legt um jedes Mesh eine **unsichtbare Kugel** und fragt nur: Liegt diese
Kugel ganz hinter einer der **sechs Wände** der Pyramide (links, rechts, oben, unten, vorn, hinten)?
Sechs kleine Rechnungen statt tausender Ecken.

```
   Blick von oben auf die Kamera-Pyramide

        \                         /
    (A)  \     (B)               /
          \                     /       (A) ganz draußen: weg
           \         (C)       /        (B) drinnen: malen
            \                 /         (C) halb am Rand: malen
             \_______________/
                   Kamera
```

Der Test ist **vorsichtig**: Eine Kugel nahe einer Ecke der Pyramide wird manchmal unnötig gemalt,
aber **nie** fälschlich weggelassen. Lieber einmal zu viel arbeiten, als dass ein Haus fehlt.

Eine Kugel legt Kern nicht nur um ein einzelnes Mesh, sondern auch um alle seine Instanzen zusammen.

> **Achtung, Falle!** Alle Kopien eines Meshes teilen sich **eine** Kugel. Ein Wald, dessen Bäume
> über die ganze Karte verstreut sind, hat eine riesige Kugel und ist darum fast immer „sichtbar“.
> Teile solche Kopien besser in räumliche Gruppen auf, dann fällt jede Gruppe außerhalb des Bildes weg.

Nie aussortiert werden Figuren mit Knochen (etwa eine laufende Figur), weil ihre Arme und Beine die
Kugel verlassen können. Auch die Schatten nutzen denselben Test, nur aus Sicht der Sonne.

> **Für Neugierige:** Die sechs Wände liest Kern direkt aus der Kamera-Rechnung ab (ein Verfahren
> von Gribb und Hartmann), und zwar so, dass jede Rechnung den Abstand in echten Metern ergibt. Dann
> lässt er sich direkt mit dem Kugelradius vergleichen.

> **Probier's aus!** Schalte in Kern Studio unter **Window › Show** den Punkt **Bounds** ein. Um jedes
> Ding erscheint ein gelblicher Linienkasten: seine Hülle, also der Raum, den es höchstens einnimmt.
> Um genau so eine Hülle legt Kern die Kugel für den Test. Verschiebe ein Ding, und der Kasten wandert
> mit.

## 5.3 Die BVH: Kartons in Kartons

Klickst du in eine Stadt, muss Kern das getroffene Dreieck unter zehntausenden finden. Alle einzeln
prüfen? Viel zu langsam. Die Lösung heißt **BVH** (*Bounding Volume Hierarchy*, auf Deutsch etwa
„Stufenordnung von Hüllkästen“): Dreiecke werden in Kästen gepackt, die Kästen in größere Kästen.

> **Stell dir vor …** du suchst eine Schraube im Keller. Die Schrauben liegen in Schachteln, die
> Schachteln in Kisten, die Kisten in Regalen. Steht auf einem Regal „nur Farbe“, öffnest du darin
> keine einzige Schachtel.

**Packen:** Kern schaut, in welcher Richtung die Dreiecke am weitesten verteilt sind, und teilt sie
dort in zwei gleich große Hälften. Jede Hälfte bekommt einen Kasten drumherum. Dann wird jede Hälfte
wieder geteilt, und so weiter, bis in jedem Kasten nur noch eine Handvoll Dreiecke liegt.

**Suchen:** Gesucht wird mit einem **Strahl**, einer gedachten Linie von der Maus in die Szene (5.4).
Verfehlt der Strahl einen Kasten, ist alles darin erledigt, ohne dass ein einziges Dreieck geprüft
wird. Trifft er ihn, schaut Kern in die beiden Unterkästen, **zuerst in den näheren**. Findet er dort
früh einen Treffer, fallen alle weiter entfernten Kästen beim nächsten Vergleich sofort heraus.

Eine BVH lohnt sich nur für große Dinge, die stillstehen, etwa die Häuser der Stadt. Wird so ein Ding
bewegt, wirft Kern die Kästen weg und packt neu, sobald es wieder ruht. Kleine oder bewegte Dinge
prüft Kern einfach Dreieck für Dreieck.

> **Für Neugierige:** Weil jede Stufe halbiert, wächst der Kartonturm nur sehr langsam: Für doppelt
> so viele Dreiecke braucht es nur eine Stufe mehr. Die genauen Bauregeln stehen im blauen Buch,
> Kapitel 5.

## 5.4 Picking: Was liegt unter dem Mauszeiger?

**Picking** heißt: herausfinden, welches Ding du angeklickt hast. Kern schickt dazu einen Strahl vom
Mauszeiger geradeaus in die Szene und sucht das **nächste** Dreieck, das er trifft.

> **Stell dir vor …** du hältst einen Laserpointer genau an die Stelle des Bildschirms, auf die du
> klickst, und leuchtest damit „durch das Glas“ in die 3D-Welt hinein. Der erste Gegenstand, auf dem
> der rote Punkt landet, ist dein Treffer.

Bei einer normalen Kamera mit Perspektive startet der Strahl im Auge des Betrachters und geht durch
den Mauspunkt. In Ansichten ohne Perspektive (etwa einer Draufsicht wie auf einem Bauplan) laufen alle
Strahlen parallel.

Dann geht Kern jedes Mesh durch, das infrage kommt:

1. **Grob:** Verfehlt der Strahl die unsichtbare Kugel des Meshes, oder liegt sie weiter weg als der
   beste Treffer bisher? Dann ist dieses Mesh erledigt.
2. **Fein:** Sonst geht es durch die Kartons der BVH oder Dreieck für Dreieck. Bei Figuren mit
   Knochen rechnet Kern dafür die aktuelle Haltung aus. **Was du siehst, kannst du anklicken.**

Als Ergebnis weiß Kern: welches Mesh, welche Kopie, welches Dreieck, wie weit weg und an welcher
Stelle. In einem kleinen Beispielprogramm färbt sich so die angeklickte Kiste rot.

Kern Studio wählt deine Objekte genau so aus. Den Boden und gesperrte Objekte durchdringt der Strahl
einfach. Lichter und leere Objekte haben keine Dreiecke; die wählst du über ihr kleines Symbol.

> **Für Neugierige:** Für den Test „Trifft der Strahl dieses Dreieck?“ benutzt Kern ein bewährtes
> Rechenverfahren von Möller und Trumbore. Das Beispiel mit der roten Kiste steht als Code im blauen
> Buch, Kapitel 5.

> **Probier's aus!** Klicke im Viewport (dem großen 3D-Fenster) verschiedene Objekte an. Links im
> Outliner, der Liste aller Objekte, springt die Markierung mit. Jeder Klick ist ein Strahl auf der
> Suche nach dem nächsten Dreieck.

## 5.5 Earcut: Vielecke in Dreiecke schneiden

Ein Hausgrundriss ist ein Vieleck, oft L-förmig und manchmal mit Löchern (Innenhöfen). Der
Grafikprozessor kann aber nur Dreiecke. Also muss jemand das Vieleck in Dreiecke zerschneiden. In
Kern macht das **Earcut**, das „Ohrenschneiden“.

> **Stell dir vor …** du hast ein Papiervieleck und eine Schere. Du suchst eine Ecke, deren Dreieck
> mit den beiden Nachbarecken ganz im Papier liegt – ein **Ohr**. Schnipp! Übrig bleibt ein Vieleck
> mit einer Ecke weniger. Weiter, bis nur ein Dreieck bleibt.

**Durchgerechnet:** Ein Fünfeck hat fünf Ecken. Erstes Ohr ab: vier Ecken bleiben. Zweites Ohr ab:
drei Ecken, also das letzte Dreieck. Macht drei Dreiecke. Allgemein werden aus n Ecken immer
**n − 2 Dreiecke**. Ein Ohr findet man immer: Jedes Vieleck ohne Selbstüberschneidung hat mindestens
zwei davon.

Drei Sonderfälle muss Earcut beherrschen:

- **Schmutzige Daten:** Echte Kartendaten haben doppelte Punkte oder Ränder, die sich kreuzen. Findet
  Earcut kein Ohr mehr, räumt es auf, repariert kleine Fehler und teilt notfalls das Vieleck in zwei.
- **Löcher:** Ohren gehen nur bei einem einzigen Rand. Darum schneidet Earcut von jedem Innenhof
  einen hauchschmalen **Schnitt** zum Außenrand, wie ein Weg durch eine Hecke. Danach ist alles ein
  einziger, verwinkelter Rand.
- **Große Vielecke:** Bei sehr vielen Ecken wäre es zu langsam, für jedes Ohr alle Ecken zu prüfen.
  Earcut gibt dann jeder Ecke eine Art Hausnummer, bei der nahe Punkte ähnliche Nummern haben, und
  schaut nur noch in der Nachbarschaft nach.

> **Wusstest du?** Nach dem Schneiden vergleicht Kern die Fläche aller Dreiecke mit der des
> Grundrisses. Weicht sie um mehr als 0,7 % ab, wird das Gebäude weggelassen, statt mit kaputtem Dach
> in der Stadt zu stehen.

> **Für Neugierige:** Kerns Earcut ist eine Übertragung des bekannten Earcut von Mapbox aus
> JavaScript und liefert dieselben Ergebnisse. Herkunft und Lizenz stehen im blauen Buch, Kapitel 5,
> und in Anhang D.

## 5.6 Primitive: der Baukasten

**Primitive** sind fertige Grundformen: Quader, Kugel, Kapsel, Torus (Donut), Zylinder, Kegel und
Ebene, alle mit Z nach oben. Man kann mehrere davon verschoben und gedreht **zu einem einzigen Mesh
zusammenkleben**.

> **Stell dir vor …** du schickst drei Briefe an dieselbe Adresse. Drei Umschläge, drei Mal zum
> Briefkasten. Steckst du alle in einen Umschlag, ist es ein Weg.

Genau so beim Malen: Für jedes Mesh bekommt die Grafikkarte einen eigenen Auftrag, und jeder Auftrag
kostet Verwaltungszeit. Ein zusammengeklebtes Ding ist **ein** Auftrag statt drei. Orb Run baut so
sein Auto aus drei Quadern, und alle Bodenplatten der Stadt werden ein einziges Mesh.

## 5.7 Welding: doppelte Ecken verschmelzen

Häuser- und Straßenbauer liefern Dreieckssuppe (5.1). Wo sechs Dreiecke zusammenstoßen, steht
dieselbe Ecke sechsmal im Speicher. **Welding** („Verschweißen“) macht daraus ein Netz mit Index: Jede
Ecke steht einmal da, und die Dreiecke zeigen per Nummer auf sie. Das spart Speicher und Zeit beim
Hochladen auf die Grafikkarte.

> **Stell dir vor …** ein Klassenfoto, auf dem jedes Kind für jeden Nachbarn neben sich noch einmal
> abgebildet ist. Welding streicht die Doppelten und schreibt stattdessen „siehe Kind Nr. 7“.

Die wichtigste Regel: Kern verschmilzt nur Ecken, die in **allem** exakt gleich sind, nicht nur im
Ort, sondern auch in Normale, Farbe und UV. Darum ändert sich am Aussehen nichts.

**Der Würfel als Beispiel:** An einer Würfelkante liegen zwei Ecken am selben Ort. Die eine gehört zur
Oberseite, ihre Normale zeigt nach oben. Die andere gehört zur Seitenwand, ihre Normale zeigt zur
Seite. Gleicher Ort, aber verschiedene Blickrichtung – also **nicht** dieselbe Ecke. Sie bleiben
getrennt, und die Kante bleibt scharf. Würde Kern sie verschmelzen, müsste die Ecke sich für eine
Richtung entscheiden, und der Würfel sähe an den Kanten weich und verwaschen aus.

Kern verschweißt außerdem nur, wenn es wirklich spart. Bringt es zu wenig, bleibt die Suppe, wie sie
ist.

> **Wusstest du?** Bei einem echten Ausschnitt von Konstanz mit 1300 Gebäuden brauchen Bau und Welding
> in Kern etwa 45 ms – die frühere JavaScript-Variante etwa 1260 ms.

Zum Schluss bekommt jedes fertige Stück einen **Fingerabdruck**: eine kurze Prüfzahl, die sich bei
der kleinsten Änderung völlig ändert. So merkt ein Programm, ob ein neu gebautes Stück überhaupt
anders ist als das alte.

## 5.8 Häuser aus Grundrissen

Ein Haus entsteht aus seinem **Grundriss**, also der Form, die es von oben gesehen hat. Kern zieht
diese Form in die Höhe.

> **Stell dir vor …** eine Plätzchenform, die du in eine dicke Teigplatte drückst: oben und unten die
> Form des Grundrisses, dazwischen senkrechte Wände. Dieses In-die-Höhe-Ziehen heißt **Extrudieren**.

Pro Haus bekommt Kern nur wenig: den Umriss, die Umrisse der Innenhöfe, die Höhe von Fuß und
Dachkante und einen Seed. Daraus macht er drei Dinge:

1. **Aufräumen:** Punkte, die fast aufeinanderliegen, und winzige Umrisse fliegen raus. Alle
   Außenränder werden in dieselbe Umlaufrichtung gebracht, alle Innenhöfe in die Gegenrichtung. So
   weiß Kern bei jeder Kante, wo „außen“ ist, und alle Wände schauen nach außen.
2. **Bauen:** Das Dach entsteht aus den Earcut-Dreiecken (5.5), Innenhöfe bleiben offen. Entlang jeder
   Kante entsteht eine Wand. Auf Wunsch kommt ein Gesims dazu, das vorstehende Band unter der
   Dachkante. Wandfarbe, Dachfarbe und Stockwerkshöhe würfelt der Seed aus.
3. **Fenster vormerken:** Jede Ecke bekommt Extra-Zahlen mit, etwa wie lang und hoch ihre Wand ist.
   Daraus malt ein Spezialeffekt aus [Kapitel 4](04-grafik.md) nachts ein Raster leuchtender Fenster
   – ohne dass ein einziges Fenster als Dreieck gebaut werden muss.

Was nicht sauber gebaut werden kann (etwa weil die Fläche nicht stimmt), wird weggelassen.

**Häppchenweise bauen.** Eine große Stadt mit tausenden Häusern auf einmal zu bauen, würde das Bild
einfrieren. Darum arbeitet der Bauer mit einem **Zeitbudget**: ein paar Millisekunden bauen, dann
darf wieder ein Bild gemalt werden, dann weiterbauen.

**Stadtviertel-Pakete.** Die Häuser landen in quadratischen **Chunks**, wie Stadtviertel auf einem
Stadtplan. Jeder Chunk wird ein eigenes Mesh. Das ist ein Abwägen: Große Chunks bedeuten wenige
Aufträge an die Grafikkarte (5.6), kleine Chunks lassen sich besser aussortieren (5.2).

> **Für Neugierige:** Die Stockwerkshöhe liegt je nach Seed zwischen 3,1 und 3,6 m. Alle weiteren
> Maße und die Namen der Bauer stehen im blauen Buch, Kapitel 5.

## 5.9 Straßen

Straßen sind **flache Bänder**, die auf dem Gelände liegen: unten ein breites Band für den Bordstein,
darauf die Fahrbahn, obendrauf die gestrichelte Mittellinie. Damit die Bänder Hügeln folgen, schneidet
Kern jede Straße in kurze Stücke, und jede Ecke fragt nach der Bodenhöhe an ihrer Stelle.

> **Stell dir vor …** du legst Klebeband auf eine Bettdecke. Ein langes, steifes Brett würde über den
> Hügeln schweben; viele kurze Stücke Klebeband schmiegen sich an.

| Schicht | Höhe über Gelände | Breite |
|---|---|---|
| Bordstein | 9 cm | Straße + 3,4 m |
| Fahrbahn | 11,2 cm | Straßenbreite |
| Markierung | 13,5 cm | 42 cm |

> **Achtung, Falle!** Liegen zwei Flächen exakt gleich hoch, weiß der Grafikprozessor nicht, welche
> vorne ist, und sie flackern um die Wette: **Z-Fighting**. Die um wenige Zentimeter gestaffelten
> Höhen verhindern das.

Die Bodenhöhe kommt von einer **Höhenauskunft**: Man fragt „Wie hoch ist der Boden bei diesem Punkt?“
und bekommt eine Zahl. Gibt es keine, ist der Boden flach. Wichtig: Das Spiel soll **dieselbe**
Höhenauskunft auch seiner Physik geben. Dann fahren Autos genau auf der Straße, die du siehst, statt
darüber zu schweben oder darin zu versinken.

Straßen, die weiter weg liegen als ein gewählter Umkreis, werden übersprungen. So lässt sich ein
Ausschnitt einer großen Karte bauen.

## 5.10 Gelände-Kacheln

Hügelige Landschaft baut Kern aus **Kacheln**: quadratischen Stücken, die nebeneinander liegen.

> **Stell dir vor …** du legst ein Fischernetz über eine Landschaft. Jeder Knoten fragt die
> Höhenauskunft: „Wie hoch ist es hier?“ und rutscht auf diese Höhe. Zwischen den Knoten spannt Kern
> Dreiecke. Fertig ist ein Stück Gelände.

Das Wichtigste: Nachbarkacheln passen **nahtlos** zusammen. Damit das Licht an der Grenze nicht
springt, braucht jede Ecke die richtige Normale, und die ergibt sich daraus, wie steil es zu den
Nachbarknoten hinauf- und hinuntergeht. Darum schaut jede Kachel eine Reihe weit über ihren eigenen
Rand hinaus. So kennen auch die Randknoten ihre Nachbarn, und beide Kacheln rechnen an der Naht
dasselbe aus.

Für echte Orte gibt es einen Umrechner: Er macht aus GPS-Koordinaten (Längen- und Breitengrad)
„so viele Meter östlich und nördlich von hier“. So kommen Grundrisse aus Kartendaten direkt in Kerns
Welt. Die genauen Namen und Werte stehen im blauen Buch, Kapitel 5.

Alle Bauer aus diesem Kapitel lassen sich auch aus JavaScript, der Programmiersprache des Browsers,
benutzen. Wie, steht ebenfalls im blauen Buch, Kapitel 5.

## 5.11 Die prozedurale Stadt

Jetzt kommt alles zusammen. **Procedural City** ist der Stadtgenerator von Orb Run als Baustein für
jedes Level. „Prozedural“ heißt: Niemand hat diese Stadt von Hand gebaut. Sie entsteht aus **Regeln**
und einem **Seed**. Gebäude, Straßen, Bodenplatten, Bäume, Kollisionskörper und Verkehr – alles aus
einer einzigen Zahl.

> **Stell dir vor …** ein Würfelspiel, bei dem du nach festen Regeln eine Stadt aufs Papier
> zeichnest: „Bei einer 1 kommt ein großes Haus mit Hof, bei einer 2 vier kleine Häuser …“ Gibst du
> jemandem die Regeln und **genau dieselbe Reihenfolge der Würfe**, zeichnet er exakt deine Stadt nach.
> Der Seed ist diese Reihenfolge der Würfe.

**Gleicher Seed, gleiche Stadt** – auf jedem Rechner, jedes Mal. Darum muss niemand die Stadt
speichern; es reicht, sich die Zahl und die paar Einstellungen zu merken.

![Ein exportiertes Spiel: die prozedurale Stadt mit Gebäuden, Straßen und Bäumen](../buch/bilder/game.png)

Die Regeln in Worten:

1. **Raster:** Die Stadt ist ein Schachbrett aus Blöcken mit Straßen dazwischen.
2. **Mitte hoch, Rand niedrig:** Je näher ein Block an der Stadtmitte liegt, desto höher dürfen seine
   Häuser werden. In der Mitte entsteht eine Hochhausgegend.
3. **Bebauung:** Jeder Block bekommt zufällig eine von vier Bebauungsarten: ein großes Haus (oft mit
   Innenhof), vier Einzelhäuser mit Gassen, ein L-förmiges Haus mit einem kleinen daneben oder drei
   lange Riegel.
4. **Straßen** an allen Blockkanten, mit Bordstein und Mittellinie, gebaut mit dem Straßenbauer aus
   5.9.
5. **Bäume** entlang der Blockränder, mit zufälligen Lücken, Größen und Grüntönen.

Gebaut wird mit den Werkzeugen dieses Kapitels: Häuser aus Grundrissen (5.8) in Stadtviertel-Paketen,
alles verschweißt (5.7). Alle Baumstämme sind ein einziges Mesh mit vielen Instanzen, alle Kronen ein
zweites.

**Klug neu bauen:** Verschiebst du die Stadt, rechnet Kern nichts neu; er verschiebt nur das fertige
Ergebnis. Neu gebaut wird nur, wenn sich eine Einstellung wirklich ändert.

**Physik und Verkehr:** Für die Physik ([Kapitel 6](06-physik.md)) wird die Stadt ein einziger
unbeweglicher Körper aus vielen einfachen Kästen, einem pro Gebäudeteil und Bodenplatte. Die Autos
stehen im Editor still. Sobald das Spiel läuft, fahren sie ihre Spur entlang, tauchen am Stadtrand auf
der anderen Seite wieder auf und halten an, wenn du vor ihnen stehst.

> **Für Neugierige:** Alle Einstellungen der Stadt mit ihren Standardwerten und Grenzen stehen im
> blauen Buch, Kapitel 5. Der Zufallsgenerator ist derselbe wie in Orb Run, darum sieht die Stadt
> dort und im Level gleich aus.

> **Probier's aus!** Öffne den Content Drawer (Ctrl+Space) und setze aus der Gruppe **World** eine
> **Procedural City** ins Level. Ändere rechts in **Details** den **Seed**: Jede Zahl ergibt eine
> neue Stadt. Tipp die alte Zahl wieder ein, und die alte Stadt ist zurück, Haus für Haus. Mit
> **Blocks per side** wird die Stadt größer oder kleiner. Mit Simulate (Alt+S), dem Probelauf mit Physik, fährt
> der Verkehr los.

## 5.12 Formen im Level und eingebaute Meshes

Jedes sichtbare Ding im Level hat eine Form: eine der Grundformen aus 5.6, dazu eine Rampe (ein Keil)
oder ein fertiges Mesh. Kern baut die Form weiß, die Farbe kommt vom Material. Damit eine Textur auf
einer großen Wand genauso groß aussieht wie auf einer kleinen, sind die UVs in Metern bemessen. Zu
jeder Form kann Kern außerdem einen passenden Kollisionskörper vorschlagen – mehr in
[Kapitel 6](06-physik.md).

Die Form **Mesh** zeigt auf ein fest eingebautes Dreiecksnetz. Heute sind das die Glieder der Micro
Duck: die Teile von Pollen Robotics in vereinfachter Form. Sie dürfen nur nicht kommerziell genutzt
werden, sind darum hier nicht abgebildet, und eine kommerzielle Fassung von Kern lässt sie weg.

> **Wusstest du?** Ein unbekannter Mesh-Name ergibt absichtlich einen 10-cm-Würfel. So fällt der
> Fehler im Level sofort auf, statt dass einfach nichts zu sehen ist.

Die Namen der Teile, wie platzsparend sie gespeichert sind und wie Kern sie beim ersten Gebrauch
auspackt, stehen im blauen Buch, Kapitel 5. Wie daraus die Ente wird, erzählt
[Kapitel 13](13-micro-duck.md).

## Das nimmst du mit

- Jede 3D-Form ist ein Netz aus Dreiecken; jede Ecke weiß, wo sie ist, welche Farbe sie hat und
  wohin ihre Fläche schaut (Normale).
- Culling prüft eine unsichtbare Kugel pro Mesh gegen die Kamera-Pyramide – billig und nie
  fälschlich unsichtbar.
- Eine BVH sind Kartons in Kartons; sie macht das Anklicken schnell.
- Picking schickt einen Strahl von der Maus in die Szene und nimmt das nächste Dreieck.
- Earcut schneidet Vielecke Ohr für Ohr in Dreiecke; Innenhöfe werden vorher über einen schmalen
  Schnitt mit dem Rand verbunden.
- Welding verschmilzt nur Ecken, die in allem gleich sind – darum bleiben harte Kanten hart.
- Häuser entstehen, indem der Grundriss in die Höhe gezogen wird, häppchenweise und in
  Stadtviertel-Paketen; Straßen sind Bänder, die dieselbe Höhenauskunft nutzen wie die Physik.
- Die prozedurale Stadt entsteht aus Regeln und einem Seed: gleiche Zahl, gleiche Stadt.

## Knobelecke

1. Warum testet Kern beim Culling eine Kugel statt aller Dreiecke?
2. Ein Wald aus 5000 Bäumen als ein Mesh mit vielen Kopien wird nie aussortiert. Was hilft?
3. Ein Vieleck hat 12 Ecken und keine Löcher. Wie viele Dreiecke macht Earcut daraus?
4. An einer Würfelkante liegen zwei Ecken am selben Ort. Warum verschmilzt Welding sie nicht?
5. Was passiert, wenn Fahrbahn und Mittellinie exakt gleich hoch lägen?
6. Deine Freundin hat in ihrer Stadt ein schönes Hochhaus gefunden. Was muss sie dir schicken, damit
   du dieselbe Stadt bekommst?

### Lösungen

1. Der Test muss viel billiger sein als das Malen; sechs kleine Rechnungen kosten fast nichts.
2. Die Bäume in räumliche Gruppen mit eigener Kugel aufteilen; Gruppen außerhalb des Bildes fallen weg.
3. n − 2 = 10 Dreiecke.
4. Die eine gehört zur Oberseite und schaut nach oben, die andere zur Seitenwand und schaut zur Seite.
   Ihre Normalen sind verschieden, also sind sie nicht in allem gleich – die Kante bleibt scharf.
5. Z-Fighting: Die Flächen flackern, weil nicht klar ist, welche vorne liegt.
6. Den Seed und die übrigen Einstellungen der Stadt. Gleiche Zahl, gleiche Regeln, gleiche Stadt.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
