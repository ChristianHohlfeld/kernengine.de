# 6 Physik – Wie Dinge fallen, rollen und zusammenhalten

> **Kurz gesagt:** Kern lässt Kisten fallen, Bälle rollen, Türen schwingen und Roboter laufen. Die
> Rechenarbeit macht **Box3D**, eine fertige Physik-Engine von Erin Catto. Kern legt eine schmale
> Schicht darüber. Du lernst Körper, Formen, Materialien, Gelenke, Kontakte, den Takt der Simulation
> und das Servomodell der Micro Duck kennen.

> **Begriffe vorab:**
> - **Servo** – ein kleiner Stellmotor, der auf Befehl einen Winkel anfährt und hält, wie er die
>   Ruder eines Modellflugzeugs bewegt. Die Micro Duck hat in jedem Gelenk einen.
> - **Hertz (Hz)** – „wie oft pro Sekunde“. 60 Hz heißt 60-mal pro Sekunde. Bei einer Feder sagt
>   es, wie schnell sie hin- und herschwingt: je mehr Hertz, desto steifer.
> - **Radiant** – das Winkelmaß der Mathematiker. Ein voller Kreis sind gut 6,28 Radiant, ein
>   rechter Winkel gut 1,57; ein Radiant sind etwa 57 Grad. Die Physik rechnet so, du siehst im
>   Studio Grad.
> - **Drehmoment** – Drehkraft. Ein Schraubenschlüssel mit langem Griff gibt bei gleicher Kraft
>   mehr Drehmoment. Gemessen in Newtonmeter (N·m).
> - **Impuls** – ein kurzer Stoß, wie ein Tritt gegen einen Ball. Eine **Kraft** dagegen schiebt
>   eine Weile.
> - **Collider** – die unsichtbare, einfache Form, mit der ein Ding anstößt. Oft gröber als das,
>   was man sieht: Ein Auto stößt etwa als Kiste an.
> - **Solver** – der Teil von Box3D, der in jedem Schritt alle Regeln auf einmal erfüllt: Nichts
>   steckt im anderen, Gelenke halten zusammen.
> - **Netz** – das gelernte Steuerprogramm der Ente, ein neuronales Netz. Es wurde in der
>   Simulation durch viel Ausprobieren trainiert ([Kapitel 13](13-micro-duck.md) und
>   [14](14-lehrer.md)).

![Das Beispiel-Level Physics Lab in Kern Studio: Stapel, Pendel, Tür, Seilbrücke, Drehteller, Lift und Rover](../buch/bilder/studio-physics-lab.png)

**Die Grundregel: Box3D wird nie an Kern angepasst.** Kern verändert weder Box3Ds Programm noch
seine festen Werte, und Kern verstellt keine geheimen Regler darin, um ein Modell „zum Laufen zu
bringen“. Passt ein Modell nicht, wird das **Modell** angepasst, nicht die Physik. Was ein echtes
Gerät zusätzlich tut, etwa ein Servo mit Regler und Reibung, bekommt ein eigenes kleines Modell in
echten Einheiten (6.15).

> **Stell dir vor …** ein Schiedsrichter, den alle Mannschaften anerkennen. Passt einem Team das Spiel
> nicht, trainiert es anders. Den Schiedsrichter zu bestechen, ist tabu.

Kern spricht mit Box3D nur an **einer einzigen Übergabestelle**. Dort wird jede Eingabe geprüft,
damit Unsinn wie ein negativer Radius nie in die Physik gelangt. Gerechnet wird in Metern, Sekunden
und Kilogramm, mit Z nach oben – wie im Renderer.

Die Grundlagen dahinter (wie eine Physiksimulation überhaupt denkt) erklärt
[Anhang C](C-grundlagen-physik.md). Hier geht es darum, **wie Kern sie benutzt**.

## 6.1 Die richtige Größe

Box3D rechnet mit einem festen **Spielraum von ein paar Millimetern**: Körper dürfen sich um etwa
5 mm durchdringen, und Kontakte werden schon kurz vor der Berührung angelegt. Für eine Kiste ist das
winzig. Für ein Glied von 2 cm Länge ist es riesig.

> **Stell dir vor …** ein Lineal mit Millimeterstrichen. Für einen Schrank ist es genau genug, für
> eine Ameise nicht.

Daraus folgt die erste Regel: **Bewegte Körper gehören zwischen 10 cm und 10 m.** Viel kleiner wird
es wackelig. Darum macht die Spiel-Laufzeit bewegte Formen nie kleiner als 1 cm.

> **Wusstest du?** Gemessen: Die Micro Duck in echter Größe hat Glieder von 2–4 cm. Ihr Rumpf sank
> innerhalb von 10 Bildern 9 cm durch die Beine. In vierfacher Größe halten die Gelenke.

### Skalieren statt Spielraum verstellen

Box3D könnte seinen Spielraum umstellen, aber nur für die **ganze** Welt – und in einem Kern-Level
stehen Autos neben kleinen Robotern. Kern vergrößert darum das **Modell**.

> **Stell dir vor …** Filmleute drehen Szenen mit Modellschiffen und spielen sie langsamer ab, damit
> die Wellen „echt“ wirken. Kern macht es umgekehrt: Der Roboter wird größer, und eine echte Sekunde
> dauert in der Welt länger. Die Bewegung bleibt dieselbe – nur in Maßen, mit denen Box3D gut umgehen
> kann.

Für die Micro Duck heißt das: **Länge ×4, Zeit ×2, die Schwerkraft bleibt gleich.** Alles andere
(Masse, Kraft, Schwung) wird passend dazu umgerechnet, sodass die Bewegung genau gleich aussieht, nur
größer und langsamer. Das ist **nur ein Wechsel der Maßeinheit**, kein anderer Roboter. Nach außen,
etwa zum Netz oder in Messprotokollen, spricht Kern weiter in echten Einheiten.

Ein Test beweist es: Ein frei hängendes Servo-Bein bewegt sich in einfacher und vierfacher Größe
**Bit für Bit gleich**, also bis in die letzte Stelle jeder Zahl. Auf dem Boden, wo der feste
Spielraum mitspielt, liegen sie weniger als einen drittel Millimeter auseinander.

Werte, die für die ganze Welt gelten (etwa wie weich Kontakte nachgeben), werden nicht mitskaliert.
Wo die Simulation vom echten Roboter abweicht, liegt es an den Modellen für Servo, Kontakt und Massen,
und die werden an Messungen angepasst ([Kapitel 16](16-kalibrierung.md)). Eine Garantie, dass der
echte Roboter sich sicher verhält, ist die Simulation nicht. Ein Roboter anderer Größe bekommt einfach
einen anderen Faktor.

> **Für Neugierige:** Bei vierfacher Größe wirkt Box3Ds Spielraum von 5 mm am echten Roboter wie nur
> 1,25 mm – feiner als in echter Größe. Die vollständige Umrechnungstabelle steht im blauen Buch,
> Kapitel 6.

## 6.2 Die Welt und ihre Körper

Alles lebt in einer **Welt**. Sie hält alle Körper, Formen und Gelenke und rechnet sie Schritt für
Schritt weiter, wie die Blätter eines Daumenkinos (6.12). In dieser Welt gibt es drei Arten von
**Körpern**:

> **Stell dir vor …** ein Theater. Der Bühnenboden bewegt sich nie: **fest**. Die Hebebühne fährt
> genau so, wie die Technik es befiehlt, und schiebt alles mit, was draufsteht: **kinematisch**. Und
> der Ball, den ein Schauspieler wirft, gehorcht Schwerkraft, Stößen und Reibung: **dynamisch**.

Einen dynamischen Körper bewegst du auf zwei Arten:

- Eine **Kraft** wirkt genau **einen** Schritt lang. Was dauerhaft schieben soll, etwa ein Wind,
  muss jeden Schritt neu wirken.
- Ein **Impuls** ist ein einmaliger Stoß, etwa für einen Sprung.

Wie schwer ein Körper ist und wie träge er sich dreht, ergibt sich normalerweise aus seinen Formen
und deren Material. Importierte Roboter bringen dagegen ihre gemessenen Werte mit; dann bestimmen die
Formen nur noch, wo sie anstoßen.

Damit schnelle Dinge nicht durch dünne Wände huschen, prüft Box3D ihre Bahn zwischen zwei Schritten
lückenlos gegen feste Dinge; besonders schnelle, etwa Spielerfahrzeuge, auch gegen bewegte. Ruhende Körper dürfen „schlafen“: Sie werden nicht mehr gerechnet, bis sie jemand anstößt.

Spiele bauen ihre Körper nicht von Hand, sondern aus dem Level ([Kapitel 8](08-spiel-laufzeit.md)).
Studio zeichnet die Collider im Editor aus **demselben Plan**, nach dem das Spiel die Physik baut.
Was du siehst, wird also genau so simuliert.

> **Für Neugierige:** Die Welt rechnet mit nur **einem** Rechenfaden. Dann ist die Reihenfolge aller
> Rechnungen immer gleich – eine Voraussetzung dafür, dass überall dasselbe herauskommt (6.13).

## 6.3 Formen

Damit ein Körper anstoßen kann, bekommt er **Formen**, gern auch mehrere. Box3D kennt fünf:

| Form | Alltagsbild |
|---|---|
| Kiste | ein Schuhkarton |
| Kugel | ein Ball |
| Kapsel | eine Tablette: Zylinder mit zwei Halbkugeln an den Enden |
| Hülle | ein Gummituch, straff um eine Punktwolke gespannt |
| Dreiecksnetz | beliebige Form, aber **nur für Dinge, die sich nie bewegen** |

Die **Hülle** ist der Allrounder: Sie passt sich an fast jede Form an, solange sie keine Dellen hat
(mehr in [Anhang C](C-grundlagen-physik.md)). Beliebige Dreiecksnetze sind nur als fester Boden oder
feste Wand erlaubt, weil Zusammenstöße zwischen zwei solchen Netzen viel zu teuer wären. Unsinniges
wie ein Radius von null oder eine flache Hülle wird abgelehnt.

Bei festen Dreiecksnetzen achtet Kern auf die Nähte zwischen den Dreiecken. Sonst bliebe ein rollender
Ball an der Naht zwischen zwei Bodenplatten hängen, als wäre dort eine Kante – eine sogenannte
**Geisterkante**.

> **Probier's aus!** Öffne über **File › Example Levels** das **Physics Lab** und schalte unter
> **Window › Show** den Punkt **Collision Shapes** ein. Jetzt siehst du die Collider als Linien über
> den Dingen: Kästen, Kugeln und andere Formen. Vergleiche: Wo ist die Form gröber als das sichtbare Ding?

## 6.4 Material

Jede Form hat ein **Material**. Es sagt, wie schwer, wie rutschig und wie sprungfreudig sie ist:

- **Dichte** – wie schwer der Stoff ist. Im Level startet jeder Collider mit **1000 kg/m³**, der
  Dichte von Wasser. Ein Würfel von 10 cm Kantenlänge wiegt damit 1 kg, einer von 1 m eine Tonne.
- **Reibung** – wie griffig (6.6).
- **Restitution** – wie stark etwas abprallt (6.6).
- **Rollwiderstand** – wie schnell Rollendes ausrollt (6.6).
- **Oberfläche** – woraus es ist, etwa Stoff oder Holz (6.5).
- **Sensor** – ein Schalter: Dann stößt die Form nichts an, sondern meldet nur, wenn etwas in sie
  hineinkommt, wie eine Lichtschranke.

> **Für Neugierige:** Wer Box3D in eigenem Programmcode ohne Level benutzt, startet bei Box3Ds
> Grundwert von 1 kg/m³. Jeder Collider eines Levels – auch der automatische eines Meshes – bekommt
> von der Spiel-Laufzeit aber die 1000 kg/m³.

### Wer mit wem zusammenstößt

Nicht alles soll mit allem kollidieren. Die Glieder eines Roboters etwa sollen sich nicht gegenseitig
anstoßen, sonst verhaken sie sich.

> **Stell dir vor …** eine Gästeliste. Jede Form sagt, zu welchem Verein sie gehört und mit welchen
> Vereinen sie sich anlegt. Zwei Formen stoßen nur zusammen, wenn **beide** mit dem Verein der
> anderen etwas zu tun haben wollen.

Dazu kommen **Gruppen**: Alle Teile eines Roboters bekommen dieselbe Gruppennummer, die heißt „unter
uns nie“. Und Körper, die direkt durch ein Gelenk verbunden sind, stoßen ohnehin nicht zusammen.

## 6.5 Oberflächen mit Paar-Reibung

Treffen zwei Formen aufeinander, braucht der Kontakt **einen** Reibwert. Normalerweise bildet Box3D
aus den Reibwerten beider Formen eine Art Mittelwert, der näher am rutschigeren liegt: Eis auf Gummi
bleibt rutschig. Für die meisten Dinge reicht das.

Aber nicht für alle. Der Quelltext sagt es treffend: *TPU auf Baumwolle hat seinen eigenen gemessenen
Wert, nicht das Mittel aus TPU auf TPU und Baumwolle auf Baumwolle.* (TPU ist ein weicher,
gummiartiger Kunststoff aus dem 3D-Drucker.)

> **Stell dir vor …** die Micro Duck soll eine Socke mit dem Schnabel greifen. Die Socke soll am
> Schnabel haften, auf dem Holzboden aber rutschen. Mit einem einzigen Reibwert pro Ding geht das nicht:
> Ist die Socke griffig, klebt sie auch am Boden. Es kommt auf das **Paar** an.

Deshalb kennt Kern **Oberflächen**: Stoff, TPU und Boden. Für bestimmte Paare gibt es eigene,
gemessene Werte (Haftreibung trockener Paare):

| Paar | Reibung | Herkunft |
|---|---|---|
| Stoff auf Stoff | 0,5 | Baumwollstrick auf Strick |
| Stoff auf TPU | 0,8 | weiche Schnabelpolster auf Baumwolle |
| Stoff auf Boden | 0,4 | Baumwolle auf Holz/Laminat |

Hat eine Seite keine Oberfläche, gilt wieder der übliche Mittelwert aus den Reibwerten der Formen.
Die Micro Duck nutzt alle drei ([Kapitel 13](13-micro-duck.md)): Die Klumpen ihrer Socke sind Stoff,
die gedruckten Polster am Schnabel TPU, der Holzboden im Beispiel-Level ist Boden. Ihre Sohlen haben
**keine** Oberfläche: Sie behalten genau die Reibung, mit der ihr Netz trainiert wurde.

Ein Feld dafür im Details-Panel gibt es nicht; die Oberfläche steht in der Leveldatei.

> **Für Neugierige:** Kern ändert Box3D dafür nicht. Box3D hat eine vorgesehene Andockstelle für
> eine eigene Reibungsregel, und dort hängt Kern seine Paartabelle ein. Der Code steht im blauen Buch,
> Kapitel 6; warum der übliche Mittelwert so gebildet wird, erklärt [Anhang C](C-grundlagen-physik.md).

## 6.6 Reibung, Sprungkraft, Rollwiderstand

Drei Material-Werte bestimmen, wie sich ein Ding anfühlt:

- **Reibung** macht rutschig (Eis) oder griffig (Gummi).
- **Restitution** macht hüpfig: 0 heißt „plumps, liegt“, 1 heißt „springt ohne Verlust zurück“. Von
  zwei Formen zählt die sprungfreudigere. Ganz leichte Stöße prallen nie ab, sonst würde alles ewig
  zittern.
- **Rollwiderstand** bremst Rollendes, weil echte Bälle und Böden etwas nachgeben.

Kern setzt den Rollwiderstand gezielt ein, und die Kommentare im Code erklären, warum:

- der Ball im Micro-Duck-Level: „ein Gummiball auf Holzboden rollt aus“,
- die Reifen des Rovers im Physics Lab rollen leicht,
- die Sockenklumpen bremsen stark: „ein Stoffklumpen rollt nicht; runde Klumpen würden es, und der
  Haufen krabbelt“.

**Roboterfüße** sind breite, flache Formen mit hoher Reibung.

Im Editor findest du die Werte im Details-Panel: unter **Mesh › Physics** für den automatischen
Collider eines Dings, unter **Collider** für einen eigenen. Den endlosen Boden stellst du unter
**World Settings › Physics (Box3D)** mit **Ground friction** und **Ground bounciness** ein.

> **Probier's aus!** Öffne das **Physics Lab** und wähle eine Kiste. Die Kisten haben eigene
> Collider, also stellst du im Details-Panel unter **Collider** die **Restitution** auf **0,9**.
> Starte Simulate (Alt+S), den Probelauf mit Physik, greife die Kiste mit der Maus, heb sie hoch und lass sie fallen. Sie springt
> jetzt mehrmals auf, jedes Mal etwas niedriger. Stell den Wert auf 0: Sie plumpst und bleibt
> liegen. Gleiche Kiste, anderes Material.

> **Für Neugierige:** In den World Settings gibt es außerdem Felder dafür, wie weich Kontakte
> nachgeben und wie schnell Box3D Ineinandergeratenes wieder auseinanderdrückt.

## 6.7 Gelenke

Ein **Gelenk** verbindet zwei Körper und erlaubt nur bestimmte Bewegungen. Box3D kennt neun Arten,
und jede hat ein Vorbild im Alltag:

| Gelenk (Editor) | Alltagsbild |
|---|---|
| Hinge | Türscharnier: dreht um eine Achse |
| Slider | Schublade: gleitet entlang einer Linie |
| Ball | Schultergelenk, mit Kegel als Grenze |
| Fixed | Schweißnaht (darf auch etwas federn) |
| Distance / rope | Stange oder Seil |
| Wheel | Rad mit Federung und Antrieb |
| Motor | zieht B zu einer Lage relativ zu A |
| Parallel | hält etwas weich aufrecht |
| No collision | verbindet nichts, verhindert nur Zusammenstöße |

**Das Beispiel Tür:** Ein Scharnier verbindet Türrahmen und Tür. Kern legt am Scharnierpunkt eine
senkrechte Achse fest, um die sich die Tür drehen darf. Eine **Grenze** von gut 97 Grad in jede
Richtung sorgt dafür, dass die Tür nicht durch die Wand schwingt. Der Winkel null ist die Lage, in
der Tür und Rahmen gebaut wurden – darum baut man Gelenke immer in ihrer Ruhelage.

Viele Gelenke haben drei Extras:

- eine **Feder**, die zu einer Lage zurückzieht, wie ein Türschließer,
- **Grenzen**, die die Bewegung begrenzen, wie ein Türstopper,
- einen **Motor**, der mit einer Wunschgeschwindigkeit dreht, aber nur bis zu seiner Höchstkraft.

Während des Spiels kann man fragen, wie stark ein Gelenk gerade hält, und Motoren an- und
ausschalten.

### Wenn Gelenke brechen

Ein Gelenk kann eine **Bruchschwelle** haben (im Editor *Break force* und *Break torque*). Wird es
stärker belastet, bricht es: Die Spiel-Laufzeit entfernt es, schreibt eine Meldung ins Log und spielt
den Bruch-Sound. So reißt die Seilbrücke im Physics Lab.

> **Probier's aus!** Starte im Physics Lab Simulate und zieh mit der Maus kräftig an einer Planke der
> Seilbrücke: Die Kette gibt nach und schwingt, reißt aber kaum, denn der Maus-Griff hat eine
> begrenzte Kraft. Willst du sie reißen sehen? Wähle im Editor die Planke **Plank 07** (Ordner
> **Bridge**) und setze in Details unter **Breaking** die **Break force** auf einen kleinen Wert,
> etwa 500. Starte Simulate erneut: Je nach Wert reißt die Brücke schon unter ihrem eigenen Gewicht
> oder sobald du an ihr ziehst. Schalte unter **Window › Show** auch **Constraints** (die Gelenke)
> ein, dann siehst du, welches Gelenk fehlt.

Servos und Roboterbeschreibungen geben Federn in anderen Einheiten an als Box3D. Kern rechnet sie in
jedem Schritt an der Übergabestelle um. Wie, steht im blauen Buch, Kapitel 6.

## 6.8 Abfragen: Strahlen, Kugeln, Explosionen

Man kann der Physik auch Fragen stellen, ohne etwas zu bewegen:

- **Strahl-Frage:** „Was trifft eine gedachte Linie von hier nach dort als Erstes?“ Die Antwort:
  Punkt, Richtung der Fläche, Körper. Sensoren werden übersprungen, und man kann Körper ausnehmen.
  So sieht ein Robotersensor seine eigenen Glieder nicht.
- **Kugel-Frage:** wie die Strahl-Frage, aber mit einer geschobenen Kugel statt einer dünnen Linie.
  Ein „dicker Strahl“ verfehlt keine Kante, gut für Bodenproben.
- **Explosion:** ein Stoß, der kugelförmig von einem Punkt ausgeht und alles in der Nähe wegschubst.
  Mit negativer Stärke wird daraus ein Sog.

Über die Vereine aus 6.4 lässt sich wählen, wen eine Abfrage überhaupt sieht.

## 6.9 Kontakte und Ereignisse

Nach jedem Schritt meldet die Physik, was passiert ist: Zwei Formen fangen an oder hören auf, sich zu
berühren. Etwas betritt oder verlässt einen Sensor. Ein Aufprall war heftig genug für ein Geräusch.
Ein Gelenk ist gebrochen.

> **Stell dir vor …** ein Nachrichtenticker nach jedem Spielzug. Der Ticker kostet aber Arbeit, also
> abonniert man nur die Meldungen, die man auch liest.

> **Achtung, Falle!** Ereignisse kosten Rechenzeit. Die Spiel-Laufzeit schaltet sie darum nur dort
> ein, wo das Spiel sie wirklich liest – etwa am Spieler, damit er Dinge einsammeln und unsichtbare Auslöser
> betreten kann.

Man kann außerdem direkt fragen: „Welche Körper berührt dieser Körper gerade, und wie fest?“ So weiß
die Micro Duck, ob ihr Fuß gerade den Boden berührt – genau wie ein Berührungssensor in einer echten
Sohle. Studio zeigt die Berührpunkte mit **Window › Show › Contact Points**.

## 6.10 MouseGrab: Körper mit der Maus ziehen

> **Stell dir vor …** du hängst einen Gegenstand an ein Gummiband und ziehst am anderen Ende. Er folgt
> dir, schwingt, stößt an, und wenn du loslässt, fliegt er mit Schwung weiter.

Genau so funktioniert der Maus-Griff: Ein unsichtbarer Punkt folgt dem Mauszeiger, und eine
gedämpfte Feder verbindet ihn mit der Stelle, an der du gegriffen hast. Die Feder schwingt nicht nach
und zieht nie stärker als ein Vielfaches des Gewichts. Drehen darf sich der Körper frei: Er kippt und
pendelt am Griff. Für den Rumpf der Micro Duck hält der Griff zusätzlich die Ausrichtung fest, wie
eine Hand, damit sie hochgehoben aufrecht bleibt. Feste und kinematische Körper lassen sich nicht
greifen.

> **Probier's aus!** In Simulate greifst du jeden dynamischen Körper mit der linken Maustaste (oder
> dem Finger). Zeiger nach oben hebt, das Mausrad holt näher oder schiebt weg, Loslassen wirft.

## 6.11 Der Character Mover

Spielfiguren laufen **nicht** ganz echt physikalisch. Echte Physik würde eine Figur an jeder
Bordsteinkante hängen lassen und an Wänden kleben. Darum rechnet die Figur ihre Bewegung selbst aus,
außerhalb des Solvers, und schubst dabei Dinge an, in die sie läuft.

> **Stell dir vor …** die Figur steht auf einem Springstock (Pogo-Stick), der sie immer ein Stück
> über dem Boden hält. Eine kleine Stufe rutscht einfach unter dem Stock durch, und die Figur wird
> sanft auf die neue Höhe gehoben. So läuft sie Treppen und Rampen hinauf, ohne zu stolpern.

Die Figur selbst ist eine Kapsel. In jedem Schritt beschleunigt sie in die gewünschte Richtung, prüft,
welche Flächen sie berührt, und gleitet an ihnen entlang statt hineinzulaufen. Drückt sie gegen eine
Wand, baut sich also keine Geschwindigkeit auf. Zu steile Flächen gelten nicht als Boden. Die
Spiel-Laufzeit nutzt den Character Mover für die Spieler „Character“ und „Soldier“
([Kapitel 8](08-spiel-laufzeit.md)).

> **Für Neugierige:** Der Character Mover hat seine eigene, knackigere Schwerkraft, damit Sprünge sich
> gut anfühlen. Alle Standardwerte stehen im blauen Buch, Kapitel 6.

## 6.12 Der Takt: Zeitschritt und Sub-Steps

> **Stell dir vor …** ein Daumenkino. Jedes Blatt ist ein Physikschritt. Wenn die Blätter immer
> gleich weit auseinanderliegen, läuft der Film ruhig. Würdest du mal ein, mal fünf Blätter
> überspringen, wäre alles ruckelig – und eine Feder könnte sich „aufschaukeln“.

Darum bekommt Box3D **immer gleich große Zeitschritte**, egal wie schnell dein Bildschirm ist.
Standard sind 60 Schritte pro Sekunde (60 Hz). Die Level-Einstellung **Step rate** erlaubt 15 bis 480;
die Regeln raten aber, nie unter 30 zu gehen.

Aber Bildschirme liefern ihre Bilder nicht im Takt der Physik. Wie passt das zusammen?

> **Stell dir vor …** ein Sparschwein. Jedes Bild zahlt die Zeit ein, die seit dem letzten vergangen
> ist. Die Physik hebt sie in festen Portionen ab, so viele, wie drin sind. Was übrig bleibt, wartet
> auf das nächste Bild.

Hängt der Rechner einmal (etwa weil ein Browser-Tab im Hintergrund lag), wird nicht unbegrenzt
nachgerechnet. Sonst bräuchte das Aufholen so lange, dass man noch weiter zurückfiele, ein
Teufelskreis. Lieber fallen ein paar Schritte weg.

Die Simulationszeit **zählt Schritte**, statt Sekundenbruchteile aufzusummieren, bei denen sich
winzige Rundungsfehler mit der Zeit ansammeln würden. Auch Eingaben gelten pro Schritt.

**Zwischenbilder:** Gezeichnet wird ein Zwischenstand der beiden letzten Physikschritte, je nachdem,
wie viel Rest im Sparschwein liegt. So sieht Bewegung bei jeder Bildrate glatt aus. Die Kamera folgt
diesem geglätteten Stand; mit dem rohen Stand wackelten schnelle Autos im Bild.

### Sub-Steps

Jeder Schritt wird in Box3D noch einmal in **Sub-Steps** (Teilschritte) zerlegt, einstellbar unter
**World Settings › Sub-steps** (Standard 4, im Level 1 bis 16). Mehr Teilschritte bedeuten, dass
lange Gelenkketten besser zusammenhalten.

> **Wusstest du?** Gemessen an der Hüfte der Ente: Bei 4 Sub-Steps wuchs die Gelenklücke unter Last auf
> 5 cm. Bei 16 Sub-Steps bleibt sie unter 1 mm. Fügt Studio einen Roboter ein, stellt es deshalb
> 16 Sub-Steps und dessen Schrittrate ein (Micro Duck: 100 Schritte pro Sekunde) und meldet das.

Auch die Steifigkeit hängt an den Teilschritten: **Je steifer eine Feder, desto mehr Teilschritte
braucht sie.** Eine Feder, die schneller schwingt, als die Physik hinschaut, gerät außer Kontrolle.
Darum begrenzen Box3D und Kern jede Feder auf das, was die Zahl der Teilschritte noch sauber fassen
kann. Steifer geht nur mit mehr Teilschritten.

## 6.13 Determinismus

Kern ist **Bit für Bit deterministisch**: Gleiche Eingaben ergeben auf einem Linux-PC, auf einem Mac
und im Browser exakt dasselbe Ergebnis, bis in die letzte Stelle jeder Zahl. Alle drei werden
regelmäßig geprüft.

Warum so streng? Weil Physik winzige Unterschiede in Sekunden aufschaukelt. Ein Unterschied in der
letzten Stelle wird bei einem laufenden Roboter schnell zu einem ganz anderen Schritt. Und Kern
braucht Gleichheit: Ein Training soll auf jedem Gerät dasselbe Netz ergeben, ein Fehler soll sich
nachstellen lassen, und eine Aufzeichnung soll überall gleich ablaufen.

Die Physik trägt dazu bei mit dem festen Zeitschritt, einem einzigen Rechenfaden und Kerns eigenen
Rechenregeln für Sinus und Co. Warum jeder dieser Punkte nötig ist, erklärt
[Anhang C](C-grundlagen-physik.md).

Geprüft wird mit einem **Fingerabdruck**: Die Ente läuft eine kurze Strecke, nach jedem Schritt ihrer Steuerung wird eine
kurze Prüfzahl ihres Zustands gebildet, und die Liste muss auf allen Plattformen dieselbe sein
([Kapitel 14](14-lehrer.md)).

> **Für Neugierige:** Für neuen Code gilt darum: keine Rechen-Abkürzungen, die das Ergebnis in der
> letzten Stelle ändern dürfen, und Sinus & Co. nur aus Kerns eigener Mathebibliothek. Die genauen
> Regeln stehen im blauen Buch, Kapitel 6.

## 6.14 Vier Grundsätze

Kern hat für alles, was Körper, Formen, Gelenke oder Motoren anlegt, eine verbindliche Regelliste.
Sie stützt sich auf Erin Cattos eigene Ratschläge und auf Messungen in Kern. Im Kern sind es vier
Grundsätze:

1. **Die richtige Größe.** Bewegte Dinge zwischen 10 cm und 10 m. Was kleiner ist, wird als Ganzes
   vergrößert (6.1), statt an der Physik zu drehen.
2. **Ähnliche Massen.** Benachbarte Teile sollen nicht allzu verschieden schwer sein, höchstens etwa
   zehnmal. Hängt ein Klotz an einem Fädchen, dehnen sich die Gelenke. Ein starres Teil ist **ein**
   Körper mit mehreren Formen, nicht mehrere zusammengeschweißte. Zu leichte Glieder bekommen eine
   Mindestmasse – die 5-g-Hüftbügel der Ente wiegen mindestens 20 g.
3. **Die Physik nie überlisten.** Keine heimlichen Zusatzkräfte, keine einfach gesetzten
   Geschwindigkeiten an Körpern mit Gelenken oder Kontakten, keine selbst gebastelte Bremse. Das
   kämpft gegen den Solver und pumpt Energie hinein. Gemessen: Ein solcher Trick ließ den Rumpf der
   Ente springen.
4. **Roboter müssen fallen dürfen.** Keine Stützen, keine gesperrten Achsen bei Laufrobotern. In der
   echten Welt gibt es sie auch nicht – und ein Roboter, der in der Simulation gestützt wird, lernt
   nichts, was ihm draußen hilft.

> **Für Neugierige:** Die vollständige Liste mit allen Regeln, ihren Gründen und einer Checkliste für
> neue Roboter steht im blauen Buch, Kapitel 6.

## 6.15 Das Servomodell – Brücke zur Micro Duck

Box3D rechnet starre Körper. Ein echter Servo tut mehr: Er regelt sich selbst zum Ziel, hat Reibung
im Getriebe und reagiert verzögert. Dafür hat Kern ein kleines **Gerätemodell** mit Werten in echten
Einheiten.

> **Stell dir vor …** eine Hand, die einen Hebel zu einer Markierung drückt. Je weiter der Hebel weg
> ist, desto fester drückt sie, aber nie stärker, als ihre Kraft reicht. Wird der Hebel schnell, bremst
> sie ihn etwas. Das Getriebe dazwischen hakt ein wenig. Und die Anweisung „Drück dorthin!“ erreicht
> die Hand immer einen Augenblick zu spät.

Das sind die vier Zutaten:

- **Ziehen:** Je weiter vom Ziel, desto mehr Drehmoment – wie eine Drehfeder.
- **Höchstkraft:** Ab einem gewissen Abstand drückt der Motor schon mit voller Kraft; weiter weg bringt
  nichts mehr.
- **Reibung:** Das Getriebe hält das Gelenk fest, bis genug Kraft anliegt.
- **Verzögerung:** Befehle kommen ein paar Hundertstelsekunden später an.

Kern baut das nur aus Box3Ds **eigenen** Gelenkteilen, ohne Zusatzkräfte. Die Feder des Scharniers
übernimmt das Ziehen. Für die Höchstkraft gibt es einen Trick: Kern rückt das Ziel der Feder nie
weiter weg, als die Feder mit voller Kraft ziehen kann. So kann sie nie stärker ziehen als der echte
Motor. Die Reibung ist ein Motor, der „Geschwindigkeit null“ will und dafür nur wenig Kraft hat: Er
hält das Gelenk, bis jemand stärker drückt.

Dieselbe Rechnung läuft im Studio, im Lehrer (dem Trainingsprogramm) und auf dem Prüfstand für die
Messungen, für jedes Servo-Gelenk in jedem Physikschritt.

> **Für Neugierige:** Die Rechnung ist nur ein paar Zeilen lang und steht samt Erklärung im blauen
> Buch, Kapitel 6. Das Rezept für die Gelenkreibung stammt von Erin Catto selbst.

### Servos im Level

Jedes Scharnier (Hinge) im Level lässt sich zum Servo machen. Es bekommt dann einen Zielwinkel, eine
Steifigkeit, eine Dämpfung und eine Reibung; seine Höchstkraft ist die Motorkraft des Gelenks.
Servo-Gelenke werden außerdem etwas steifer gerechnet, damit leichte Glieder sich nicht dehnen.

### Profile und Streuung

Alle Werte eines Servotyps stehen in einem **Profil**. Ein anderer Servo ist einfach ein anderes
Profil. Profile werden an Messungen des echten Geräts angepasst und an anderen Messungen geprüft
([Kapitel 16](16-kalibrierung.md)). Das Standardprofil ist der Dynamixel XL330, der Servo der Micro
Duck, mit den Werten, mit denen Pollen Robotics ihn trainiert hat.

Jedes Profil sagt außerdem, wie **sicher** seine Werte sind. Beim Training zieht Kern für jeden Servo
leicht verschiedene Werte innerhalb dieser Grenzen.

> **Stell dir vor …** ein Basketballspieler, der immer nur mit demselben Ball übt. Im Turnier ist
> der Ball etwas härter – und er trifft nicht mehr. Wer mit leicht verschiedenen Bällen übt, trifft mit
> jedem. Genauso lernt das Netz, mit Servos umzugehen, die ein bisschen anders sind als gemessen.

### Die Grenze zu Box3D

Die Ente lebt in der vierfach vergrößerten Welt (6.1), die Profile sprechen echte Einheiten. Kern
rechnet genau an **einer** Stelle um, wenn die Werte in die Ente geschrieben werden. Auch die
Verzögerung wird umgerechnet, und zwar in ganze Physikschritte.

**Durchgerechnet:** Der XL330 reagiert 20 ms (Tausendstelsekunden) verzögert. In der vergrößerten Welt
läuft die Zeit doppelt so lang, also werden daraus 40 ms. Die Ente rechnet 100 Schritte pro Sekunde,
ein Schritt dauert also 10 ms. 40 ms sind damit genau **4 Schritte**. Die Befehle stehen so lange in
einer Warteschlange, bevor sie beim Servo ankommen.

> **Für Neugierige:** Die Trägheit des kleinen Motors im Servo kommt als dünne Scheibe auf die
> Gelenkachse. Eine bequemere Abkürzung hätte Box3D Energie aus dem Nichts gewinnen lassen. Was Box3D
> nicht abbilden kann, wird aufgeschrieben, nicht vorgetäuscht.

Wie die Micro Duck damit läuft, steht in [Kapitel 13](13-micro-duck.md), wie Profile gemessen werden,
in [Kapitel 16](16-kalibrierung.md). Automatische Tests sichern das alles ab, von den Körpern über
alle neun Gelenkarten bis zum Fingerabdruck.

## Das nimmst du mit

- Box3D rechnet. Kern passt nie Box3D an, sondern das Modell.
- Bewegte Körper gehören zwischen 10 cm und 10 m; kleine Roboter werden als Ganzes vergrößert (Ente:
  Länge ×4, Zeit ×2) – nur ein Wechsel der Maßeinheit.
- Es gibt feste, kinematische und dynamische Körper; Formen und Materialien bestimmen den Kontakt.
- Manche Paare haben eine eigene, gemessene Reibung: Die Socke haftet am Schnabel und rutscht auf dem
  Boden.
- Gelenke verbinden Körper, haben Federn, Grenzen, Motoren und können brechen.
- Die Physik rechnet in festen Schritten, gespart wird wie im Sparschwein, gezeichnet wird ein
  Zwischenstand. Mehr Teilschritte halten Gelenkketten fester zusammen.
- Die Physik nie überlisten, Roboter fallen lassen.
- Ein Servo zieht wie eine Feder zum Ziel, aber nur bis zu seiner Höchstkraft, mit etwas Reibung und
  Verspätung – mit Werten aus einem Profil.

## Knobelecke

1. Warum stellt Kern nicht einfach Box3Ds Spielraum um, statt die Ente zu vergrößern?
2. Ein Ball hüpft nie, obwohl seine Restitution 0,9 ist. Er fällt nur langsam. Woran liegt's?
3. Eine Socke (Oberfläche Stoff) liegt auf einem Boden ganz ohne Oberfläche. Welche Reibung gilt?
4. Du setzt in jedem Schritt die Geschwindigkeit des Ente-Rumpfs, damit er aufrecht bleibt. Was ist
   daran falsch?
5. Ein anderer Servo reagiert 30 ms verzögert. Die Ente lebt in der Welt, in der die Zeit doppelt so
   lang läuft, und rechnet 100 Schritte pro Sekunde. Wie viele Schritte Verzögerung sind das?

### Lösungen

1. Der Spielraum gilt für die ganze Welt, und im selben Level stehen Autos neben kleinen Robotern.
2. Er ist zu langsam: Ganz leichte Stöße prallen nie ab, damit nichts ewig zittert.
3. Für dieses Paar gibt es keinen eigenen Wert, weil der Boden keine Oberfläche hat. Also gilt der
   übliche Mittelwert aus den Reibwerten der beiden Formen.
4. Das überlistet die Physik: Es kämpft gegen den Solver und pumpt Energie hinein (gemessen: der Rumpf
   sprang). Und ein Roboter, der Hilfe bekommt, lernt nichts für die echte Welt.
5. 30 ms × 2 = 60 ms in der Welt. Ein Schritt dauert 10 ms, also 6 Schritte.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
