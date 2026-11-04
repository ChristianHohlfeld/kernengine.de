# 6 Physik – Box3D an der Leine

> **Dein Level:** Kisten stapeln, Türen schwingen, Brücken brechen. Hier siehst du, wie Kern
> Box3D benutzt, welche Regeln es sich gibt und wie ein echter Servo zum Modell wird.

> **Wörter, die du brauchst:**
>
> - **Starrer Körper** – ein Ding, das sich nie verbiegt oder eindrückt, wie ein Holzklotz.
> - **Newton (N)** und **Gewichtskraft** – Newton ist die Einheit der Kraft. Die Erde zieht an
>   jedem Kilogramm mit 9,81 N, also ist die Gewichtskraft **Masse · 9,81**. Eine Tafel
>   Schokolade (100 g) drückt mit knapp 1 N auf deine Hand.
> - **Impuls** – ein einzelner kurzer Stoß, wie ein Tritt gegen einen Ball. Er ändert das Tempo
>   auf einen Schlag: Impuls = Masse · Tempoänderung.
> - **Drehmoment** – wie stark etwas dreht: Kraft mal Hebel. 1 N am Ende eines 1 m langen
>   Schraubenschlüssels ist 1 Newtonmeter (N·m). Langer Schlüssel, gleiche Kraft, mehr Drehmoment.
> - **Trägheitsmoment** – wie schwer sich ein Ding andrehen lässt. Auf dem Bürostuhl drehst du dich
>   mit angezogenen Armen schneller als mit ausgestreckten: Masse weit außen macht träge.
> - **Radiant (rad)** – ein anderes Winkelmaß. Leg eine Schnur so lang wie der Radius auf den
>   Kreisrand: Der Winkel dazu ist 1 rad, etwa 57°. Ganz herum passt die Schnur gut 6,28-mal.
> - **Hertz (Hz)** – wie oft pro Sekunde. Eine Feder mit 2 Hz wippt zweimal pro Sekunde hin und
>   her; 60 Hz Takt heißt 60 Rechenschritte pro Sekunde.
> - **Dämpfung** – die Bremse beim Schwingen, wie der Stoßdämpfer am Fahrrad. Viel Dämpfung: Die
>   Feder kommt ohne Nachwippen zur Ruhe.

Kern simuliert **starre Körper** mit **Box3D**, einer freien Physik-Engine von Erin Catto. Box3D
liegt **unverändert** in Kern. Wie Physik im Computer grundsätzlich denkt, erklärt
[Anhang C](C-grundlagen-physik.md); hier geht es darum, wie Kern damit arbeitet.

Die Grundregel: **Box3D wird nie an Kern angepasst.** Passt ein Modell nicht zu Box3D, wird **das
Modell** angepasst: vergrößert, Massen anders verteilt, feiner gerechnet. Das ist wie beim
Fußball: Wenn deine Taktik nicht funktioniert, änderst du die Taktik, nicht die Spielregeln. Ein
echtes Gerät wie ein Servo bekommt ein eigenes kleines Modell (6.17).

![Das Beispiel-Level Physics Lab beim Simulieren: Stapel, Pendel, Tür, Seilbrücke, Drehteller, Lift und Rover](../buch/bilder/studio-physics-lab.png)

> **Mission:** Öffne auf https://kernengine.de/ **Datei › Beispiel-Levels › Physics Lab** (File ›
> Example Levels › Physics Lab) und drück **Simulieren** (Simulate) (Alt+S). Pack eine Kiste der Pyramide mit der linken Maustaste, heb sie hoch und lass
> los: Sie fliegt mit Schwung weiter. Stoß das Pendel an und öffne die Tür.

## 6.1 Aufbau der Schicht

Zwischen dem Spiel und Box3D liegt eine dünne **Schicht**. Sie ist die **einzige Stelle**, an der
Kern mit Box3D spricht. Dort wird jeder unsinnige Wert abgefangen, etwa ein negativer Radius oder
eine Lage, die keine Zahl ist (das passiert, wenn bei einer Rechnung vorher zum Beispiel 0 : 0
herauskam). Box3D bekommt nur Sauberes. Wie bei einem Türsteher: Wer nicht
passt, kommt gar nicht erst hinein, und der Fehler wird gemeldet ([Kapitel 3](03-grundlagen.md)).

```mermaid
%% Bildunterschrift: Der einzige Weg zu Box3D führt durch die Physik-Schicht
flowchart LR
  L["Level"] --> R["Spiel-Laufzeit"]
  R --> P["Physik-Schicht"]
  P --> B["Box3D"]
```

Die **Spiel-Laufzeit** im Bild ist der Teil von Kern, der ein Level als Spiel ablaufen lässt
([Kapitel 8](08-spiel-laufzeit.md)).

Die Schicht rechnet in **Metern, Sekunden und Kilogramm**, mit **Z nach oben**, genau wie die
Grafik. Winkel sind in **Radiant**. Grad siehst du nur in Kern Studio und in Leveldateien; die
Umrechnung passiert **an genau einer Stelle**, beim Aufbau. Warum nur an einer? Stell dir vor, in
einer Klasse rechnet jeder selbst Euro in Dollar um. Vergisst es einer oder rechnet einer doppelt,
stimmt am Ende die Kasse nicht, und niemand weiß, wo der Fehler steckt. Gibt es nur eine Stelle, die
umrechnet, weiß man sofort, wo man suchen muss. Eine **Welt** besitzt alles, was in ihr entsteht,
und räumt es beim Schließen mit auf.

Die genauen Namen und Befehle stehen im blauen Buch, Kapitel 6.

## 6.2 Die richtige Größe

Box3D fühlt sich mit Dingen zwischen **10 cm und 10 m** am wohlsten. Der Grund: Es arbeitet mit
festen Toleranzen in Metern. Zum Beispiel duldet es bis zu **5 mm** Überlappung zwischen zwei
Körpern, bevor es sie auseinanderdrückt. Bei einer Kiste von 1 m fällt das nicht auf. Bei einem
Roboterbein von 2 cm ist das ein Viertel des Beins.

Daher Regel eins: **Bewegte Körper zwischen 0,1 m und 10 m, nie unter 1 cm.** Kern hebt winzige
bewegte Formen automatisch auf mindestens 1 cm an.

> **Krass, aber wahr:** Die Micro Duck in echter Größe hat Glieder von 2 bis 4 cm. Ihr Rumpf sank in
> 10 Bildern 9 cm durch die eigenen Beine. Vierfach vergrößert halten die Gelenke.

### Skalieren statt Regeln verstellen

Box3D könnte seine Toleranzen umstellen, aber nur für die ganze Welt, und in einem Level stehen
Autos neben kleinen Robotern. Darum macht Kern es andersherum: Ein kleines echtes Modell wird als
Ganzes mit einem **Faktor S** vergrößert, und jede Größe wird passend umgerechnet. Für die Micro
Duck ist **S = 4**:

| Größe | Faktor | bei S = 4 |
|---|---|---|
| Länge | S | ×4 |
| Masse | S · S · S | ×64 |
| Zeit | Wurzel aus S | ×2 |

**Masse ×64**, weil ein Würfel mit vierfacher Kante 4 · 4 · 4 = 64-mal so viel Volumen hat. Aus
demselben Holz ist er also 64-mal so schwer.

**Zeit ×2**, weil ein viermal so langes Pendel halb so schnell schwingt. Die vergrößerte Ente
bewegt sich also wie in Zeitlupe, halb so schnell. Eine echte Sekunde sind zwei Sekunden in der
Welt. In der Tabelle steht dafür „Wurzel aus S“. Die **Wurzel** aus einer Zahl ist die Zahl, die
mit sich selbst malgenommen die erste ergibt: Die Wurzel aus 4 ist 2, denn 2 · 2 = 4. Die Wurzel
aus 9 ist 3.

> **Probier's aus:** Lass einen Schlüsselbund an einer kurzen Schnur pendeln (etwa 25 cm) und zähl
> die Schwünge in 10 Sekunden. Dann nimm eine viermal so lange Schnur (1 m). Du zählst nur noch etwa
> halb so viele Schwünge.

Warum muss die Zeit überhaupt mitwachsen? Weil die **Schwerkraft gleich bleibt**. Die große Ente
fällt beim Stolpern einen viermal so langen Weg, aber die Erde zieht nicht stärker. Darum dauert
jedes Fallen, Kippen und Schwingen länger, genau wie beim langen Pendel. Lässt Kern die Zeit
passend mitwachsen, sieht jede Bewegung genauso aus wie bei der kleinen Ente, nur größer und
langsamer.

> **So kannst du es dir vorstellen:** Filmemacher drehen Szenen mit einer Modelleisenbahn und
> spielen den Film langsamer ab. Dann sieht der kleine Zug aus wie ein echter großer Zug. Bei der
> Ente ist es umgekehrt: Einen Film der großen, langsamen Ente müsstest du nur doppelt so schnell
> abspielen und auf ein Viertel verkleinern, und du sähst genau die kleine Ente.

In diesem Sinn ist es **dieselbe Ente**: gleiche Haltungen, gleiche Schritte, gleiches Kippen.
Rechnet man Längen durch 4 und Zeiten durch 2 zurück, kommt die kleine Ente heraus. Darum geht
nach außen alles wieder in echten Einheiten: an das **Netz** (das lernende Programm, das die Ente
steuert), in Logs und an [Kapitel 16](16-kalibrierung.md).

Und wenn sich Simulation und echter Roboter trotzdem unterscheiden? Dann liegt es **nicht an der
Vergrößerung**, sondern an den Modellen: wie genau Servo, Kontakte und Massen nachgebaut sind.
Darum wird am echten Roboter gemessen und nachgebessert ([Kapitel 16](16-kalibrierung.md)). Ein
paar Box3D-Werte stehen fest in echten Metern und wachsen nicht mit, zum Beispiel wie weich sich
Kontakte anfühlen. Die rechnet Kern nicht um, sondern stellt sie so ein, dass sie zur Messung
passen: **kalibriert, nicht skaliert**. Eine Garantie für den echten Roboter ist eine Simulation
aber nie.

> **Für Profis:** Auch Trägheitsmoment (×1024), Drehmoment (×256) und Dämpfung (×512) werden
> umgerechnet. Das heißt Froude-Ähnlichkeit. Ein kleines Testbein rechnet frei hängend in einfacher
> und vierfacher Größe sogar Bit für Bit gleich (Anhang C.9). Alle Werte stehen im blauen Buch,
> Kapitel 6.

## 6.3 Die Welt

Eine **Welt** ist der Raum, in dem die Physik rechnet. Sie hat eine **Schwerkraft** von 9,81 m/s²
in Richtung **minus Z**: Z zeigt nach oben, also zieht minus Z nach unten.

Die Einheit m/s² (Meter pro Sekunde pro Sekunde) heißt: **Jede Sekunde wird ein fallendes Ding um
9,81 m/s schneller.** Nach 1 Sekunde fällt es mit 9,81 m/s, nach 2 Sekunden mit 19,62 m/s, wenn
nichts bremst. Das ist dieselbe Zahl wie die 9,81 N pro Kilogramm aus dem Wörterkasten, nur von
der anderen Seite gesehen: Genau diese Kraft auf jedes Kilogramm macht genau dieses Schnellerwerden.

Die Welt wird nicht fließend weitergerechnet, sondern in **kleinen, festen Schritten**, wie ein
Daumenkino Bild für Bild. Warum die Schritte immer gleich lang sind, zeigt 6.13.

Box3D rechnet bei Kern auf einem **einzigen Rechenkern**. Ein Rechenkern ist ein Teil des
Prozessors, der Rechnungen ausführt. Moderne Computer haben mehrere, die gleichzeitig arbeiten
könnten. Dann wäre aber mal der eine, mal der andere zuerst fertig, und die Rechnungen kämen jedes
Mal in einer anderen Reihenfolge an. Warum schon die Reihenfolge das Ergebnis ändert, zeigt 6.14.

Die Standardwerte der Welt stehen im blauen Buch, Kapitel 6.

## 6.4 Körper

Ein **Körper** ist ein Ding, das sich in der Physik bewegen kann. Es gibt drei Arten. In Kern
Studio wählst du sie in **Details** unter **Starrkörper (Box3D)** (Rigid Body (Box3D)) ›
**Mobilität** (Mobility):

- **Statisch** (Static): bewegt sich nie. Boden, Wände, Häuser.
- **Kinematisch** (Kinematic): bewegt sich, wie du es sagst, und lässt sich durch nichts aufhalten.
  Fahrende Plattformen, Drehbalken, die Autos der Stadt.
- **Dynamisch (Physik simulieren)** (Dynamic (simulate physics)): reagiert auf Kräfte, Stöße und
  Schwerkraft. Echte Physik.

> **So kannst du es dir vorstellen:** Statisch ist die Hallenwand, kinematisch ist die Rolltreppe
> (sie fährt, egal wer drauf steht), dynamisch ist der Ball.

Ein Körper hat außerdem Lage, Drehung, Anfangstempo und eine Bremse gegen Bewegung (Dämpfung). Er
kann **schlafen**: Liegt er lange still, rechnet Box3D ihn nicht mehr, bis ihn etwas anstößt. Das
spart viel Zeit. Ein **Bullet** ist ein sehr schneller Körper, den Box3D besonders sorgfältig gegen
alles prüft, damit er nicht durch Dinge hindurchfliegt.

Bewegen kann man einen Körper auf zwei Arten: mit einer **Kraft**, die nur einen Schritt lang wirkt
(wie Schieben), oder mit einem **Impuls**, der das Tempo auf einen Schlag ändert (wie ein Tritt).
Man kann auch einzelne Richtungen **sperren**, sodass ein Körper zum Beispiel nicht umkippt.

Am besten entsteht ein Körper gleich **an seinem Platz und mit der richtigen Art**. Ihn erst
woanders zu bauen und dann hinzubeamen oder ihm nachträglich eine andere Art zu geben, kostet
Box3D doppelte Arbeit, wie ein Schrank, den du erst im Flur aufbaust und dann ins Zimmer trägst.

**Masse und Trägheitsmoment** rechnet Box3D aus der Form und der **Dichte** (wie viel ein Kubikmeter
wiegt). Man kann sie auch selbst vorgeben. So bekommen nachgebaute Roboter ihre gemessenen Werte,
und die Form sorgt dann nur noch dafür, wo sie anstoßen.

## 6.5 Formen

Ein Körper besteht aus einer oder mehreren einfachen **Formen**: **Box**, **Kugel**, **Kapsel**
(ein Zylinder mit runden Enden, wie eine Tablette), **Hülle** und **Mesh**. In Kern Studio heißt so
eine Physikform **Kollider** (Collider). Hat ein Actor keinen eigenen, baut Studio automatisch einen
passenden zu seiner Grundform.

Eine **Hülle** ist die Form, die entsteht, wenn du ein Gummiband um eine Handvoll Nägel spannst,
nur in 3D: außen herum, ohne Dellen nach innen. Solche Formen ohne Dellen heißen **konvex**, und mit
ihnen kann Box3D besonders schnell und zuverlässig rechnen (Anhang C.3). Eine Box ist für Box3D auch
eine Hülle.

Ein **Mesh** aus beliebigen Dreiecken ist nur für **feste** Körper erlaubt, zum Beispiel für einen
hügeligen Boden. Warum? Ein Mesh darf Dellen haben, und es ist nur eine dünne Haut ohne Inneres.
Box3D kann daraus nicht ausrechnen, wie schwer es ist, und rechnet bei Kontakten damit, dass sich
das Mesh nicht bewegt. Bei einem Boden ist beides egal: Er muss nichts wiegen und bleibt immer, wo
er ist. Bewegte Dinge baut man darum aus konvexen Formen.

Beim Mesh passt Kern auf, dass nichts an der Naht zwischen zwei Dreiecken hängen bleibt. Ohne diese
Vorsicht könnte eine rollende Kugel an der Naht hüpfen, als wäre da eine unsichtbare Kante. Das
heißt **Geisterkollision**.

### Material

Jede Form hat ein **Material**. Die Standardwerte in Kern Studio:

| Feld | Standard | Bedeutung |
|---|---|---|
| Dichte | 1000 kg/m³ | wie Wasser |
| Reibung | 0,6 | wie stark sie bremst |
| Rückprall (Elastizität) | 0 | Sprungkraft, 0 bis 1 |
| Rollwiderstand | 0 | bremst Rollen |
| Oberfläche | keine | für Paar-Reibung (6.6) |
| Sensor (nur Überlappung) | aus | nur Überlappung melden |

Ein **Sensor** stößt nicht an, er meldet nur, wer hineinläuft. So funktionieren Zielzonen und
Trigger.

### Wer stößt mit wem zusammen?

Jede Form kann auf **Kanälen** sagen, zu welcher Gruppe sie gehört und mit welchen Gruppen sie
zusammenstoßen will. Ein Beispiel: Kugeln sollen durch Geister hindurchfliegen, aber an Wänden
abprallen. Dann gehören Geister zu einem Kanal, auf den die Kugeln nicht achten.

Für Roboter gibt es noch etwas Einfacheres: Alle Teile eines Roboters kommen in **eine
gemeinsame Gruppe**, deren Mitglieder sich nie gegenseitig anstoßen. So stolpert ein Roboter
nicht über seine eigenen Beine.

## 6.6 Oberflächen mit Paar-Reibung

Berühren sich zwei Formen mit verschiedener Reibung, braucht der Kontakt **einen** Wert. Box3D nimmt
eine Faustregel: die Wurzel aus dem Produkt der beiden Werte, √(a · b). Bei 0,5 und 0,5 sind das
0,5, bei 0 und irgendetwas immer 0. Warum gerade so, erklärt Anhang C.4.

Für echte Materialpaare stimmt die Faustregel aber nicht immer. Das zeigt die Micro Duck: Sie hebt
eine Socke mit dem Schnabel auf. Das weiche Polster am Schnabel muss auf der Socke **greifen**, die
Socke muss aber auf dem Holzboden **rutschen**. Ein einziger Reibwert pro Form kann nicht beides.

Darum kennt Kern **Oberflächen**: Baumwollstrick (*cloth*), weiches gedrucktes Gummi (*tpu*) und
Holz- oder Laminatboden (*floor*). Für Paare aus diesen Oberflächen gibt es eigene Werte, gewählt nach Messwerten aus der Fachliteratur:

| Paar | Reibung | Wie im Alltag |
|---|---|---|
| Strick auf Strick | 0,5 | Socke auf Socke |
| Gummi auf Strick | 0,8 | Schnabel greift Socke |
| Strick auf Holz | 0,4 | Socke rutscht übers Parkett |

Box3D bleibt dafür unverändert. Es fragt bei jedem Kontakt: „Welche Reibung gilt hier?“ Kern
antwortet: Hat das Paar einen eigenen Wert, gilt der, sonst die Faustregel. Sprungkraft und
Rollwiderstand bleiben, wie sie sind.

Die Sohlen der Ente haben **keine** Oberfläche. Sie behalten die Reibung, mit der Pollen Robotics
ihr Netz trainiert hat (1,0 auf einem Boden mit 1,0, also √(1 · 1) = 1). Mehr zur Ente in
[Kapitel 13](13-micro-duck.md).

## 6.7 Reibung, Sprungkraft, Rollwiderstand

Drei Werte bestimmen, wie sich Dinge berühren:

- **Reibung** bremst das Rutschen. 0 ist Eis, 1 ist Gummi auf Asphalt.
- **Restitution** (Sprungkraft) bestimmt, wie viel Tempo nach einem Aufprall übrig bleibt. 0 bleibt
  liegen, 1 springt perfekt zurück. Box3D nimmt das **Größere** von beiden Formen: Ein Gummiball
  springt auch auf Beton. Stöße unter 1 m/s prallen gar nicht ab, sonst würde Ruhendes ewig zittern:
  Winzige Rechenreste würden es immer wieder ein klein wenig hochhüpfen lassen.
- **Rollwiderstand** bremst das Rollen. Eine perfekte Kugel auf einem perfekten Boden würde ewig
  rollen. Echte Bälle und Böden geben ein wenig nach, und das bremst.

In Kern Studio stellst du sie unter **Details › Statisches Mesh › Physik** (Details › Static Mesh ›
Physics) ein (für den automatischen Kollider) oder unter **Details › Kollider** (Details › Collider).
Den endlosen Boden stellst du unter **Welteinstellungen** (World Settings) › **Physik (Box3D)**
(Physics (Box3D)) › **Bodenreibung** (Ground friction) und **Bodenelastizität** (Ground bounciness) ein. Dort steht auch, wie weich
Dinge aufeinanderliegen: in den Feldern **Kontaktsteifigkeit** (Contact stiffness) und
**Kontaktdämpfung** (Contact damping ratio). Sie gelten nicht pro Form, sondern für die ganze Welt.

Rollwiderstand setzt Kern bei runden Dingen. Der Ball im Micro-Duck-Level hat 0,03, denn „ein
Gummiball auf Holzboden rollt aus“. Die Socke besteht aus runden Stoffklumpen und hat 0,5, denn „ein
Stoffklumpen rollt nicht“. Ohne ihn kroch die Socke von allein über den Boden. **Roboterfüße**
brauchen keinen Rollwiderstand: Sie sind breit, flach und griffig gebaut, damit sie gar nicht erst
rollen oder rutschen.

> **Mission:** Läuft noch **Simulieren** (Simulate), stopp mit Esc. Wähle im Physics Lab die oberste
> Kiste, den Actor „Crate 15“. Setz in **Details › Kollider › Reibung** (Details › Collider › Friction)
> den Wert von 0,7 auf 0 (die Kisten im Physics Lab haben 0,7 statt des Standards 0,6). Starte
> **Simulieren**, setz die
> Kiste mit der Maus auf den Boden und schubs sie an. Sie gleitet wie auf Eis. Warum? 0 mal
> irgendetwas ist 0, und die Wurzel aus 0 ist auch 0.

## 6.8 Gelenke

Ein **Gelenk** verbindet zwei Körper so, dass sie sich nur noch auf eine bestimmte Weise
gegeneinander bewegen können. Jedes Gelenk hat eine **Achse**: Bei einem Scharnier ist es die
Linie, um die sich etwas dreht, bei einer Schiene die Linie, entlang der etwas gleitet.

![Ein Scharnier dreht um seine Achse (Tür von oben), eine Schiene lässt entlang ihrer Achse gleiten (Lift von der Seite)](../abbildungen/gelenke.svg)

Die wichtigsten Gelenke in Kern Studio (Feld **Typ** (Type) im Abschnitt **Gelenk** (Constraint)):

| Im Studio | Wofür |
|---|---|
| **Scharnier (Drehgelenk)** (Hinge (revolute)) | Tür, Brücke |
| **Schiene (Schubgelenk)** (Slider (prismatic)) | Lift |
| **Kugelgelenk** (Ball (spherical)) | wie die Schulter |
| **Abstand / Seil** (Distance / rope) | fester Abstand oder Seil: Pendel |
| **Rad (Federung)** (Wheel (suspension)) | Rad mit Federung und Antrieb |

> **Für Profis:** Dazu kommen Spezialgelenke: **Fest (verschweißt)** (Fixed (weld)) verbindet starr
> oder leicht federnd, **Motor** zieht ein Teil zu einer Ziellage, **Parallel** hält ein Teil weich
> aufrecht (mehr in 6.15), und **Keine Kollision (Filter)** (No collision (filter)) verhindert nur,
> dass zwei Körper zusammenstoßen.

Je nach Gelenk gibt es eine **Feder** (wie oft sie pro Sekunde schwingen will, in Hz, und wie stark
sie gedämpft ist), **Grenzen** (die Tür geht nur bis zur Wand auf) und einen **Motor** (dreht oder
schiebt mit einem Tempo, aber nie stärker als seine Höchstkraft). Normalerweise stoßen zwei
verbundene Körper nicht zusammen.

**In der Ruhelage bauen:** Ein Gelenk misst seinen Winkel ab der Lage, in der es gebaut wurde. Dort
ist der Winkel 0. Grenzen und Federn beziehen sich auf diese 0. Baust du eine Tür halb offen ein,
hält sie „halb offen“ für zu, und die Grenze „bis zur Wand“ sitzt an der falschen Stelle. Darum
baut man ein Gelenk so, wie es in Ruhe aussieht.

**Brechen:** Man kann einem Gelenk eine **Bruchkraft** geben. Zieht mehr Kraft daran, meldet Box3D
das. Die Spiel-Laufzeit zerstört das Gelenk dann, zählt es, meldet es im **Ausgabeprotokoll**
(Output Log) und spielt ein
Bruchgeräusch. Im Physics Lab haben die drei mittleren Gelenke der Brücke so eine Schwelle: die
Gelenke von Plank 06, 07 und 08, mit je 60 000 N. Das ist eine hohe Schwelle.

> **Mission:** Läuft **Simulieren** (Simulate), stopp mit Esc. Wähle den Actor „Plank 06“ und trag in
> **Details › Gelenk › Bruch** (Details › Constraint › Breaking) als **Bruchkraft (0 = nie)** (Break
> force (0 = never)) 20 000 ein, also eine viel niedrigere Schwelle als die 60 000. Starte
> **Simulieren**: Die Brücke hält noch. Pack die Planke mit der Maus (wie das Greifen funktioniert,
> erklärt 6.11) und zieh sie kräftig zwei, drei Meter hoch. Das Gelenk bricht, und das
> **Ausgabeprotokoll** meldet es. Warum? Die straff
> gespannte Brücke zieht schon in Ruhe mit über 10 000 N an diesem Gelenk. Das reicht noch nicht,
> aber dein Griff legt etwas drauf. Probier danach auch 2000: Dann reißt das Gelenk schon gleich
> nach dem Start, ganz ohne dich.

> **Für Profis:** Box3D will Gelenkfedern in Hertz, ein Servo rechnet in N·m pro Radiant. Kern
> rechnet an der Grenze um, jeden Schritt neu, weil es dabei auf die aktuelle Lage der Glieder
> ankommt (Anhang C.5). Die englischen Box3D-Namen der Gelenke stehen im blauen Buch, Kapitel 6.

## 6.9 Abfragen

Man kann die Physikwelt auch **fragen**, ohne etwas zu bewegen:

- **Strahl:** Was trifft eine Linie zuerst? Beispiel: Steht die Figur auf dem Boden? Schick einen
  Strahl ein Stück nach unten. Trifft er, steht sie.
- **Kugel schieben:** wie ein Strahl, aber mit einer Kugel statt einer dünnen Linie, wie ein dicker
  Strahl. Er rutscht nicht durch eine schmale Lücke oder an einer Kante vorbei.
- **Explosion:** Alle Körper in einem Umkreis bekommen einen Stoß nach außen. Mit negativem Wert
  ziehen sie nach innen (Implosion).
- **Schieben:** Läuft eine Spielfigur gegen eine Kiste, bekommt die Kiste genau so viel Stoß, dass
  sie vor der Figur nicht langsamer ist als diese.

Ein Treffer verrät die Form, den Körper, den Punkt und die Normale. Strahlen ignorieren dabei
Sensoren, und man kann Körper ausnehmen. Ein Robotersensor sieht so seine eigenen Glieder nicht.

## 6.10 Kontakte und Ereignisse

Nach jedem Schritt meldet die Physik, was in **diesem** Schritt passiert ist: Wer hat angefangen
oder aufgehört, wen zu berühren? Wer ist in einen Sensor gelaufen oder wieder heraus? Wer ist hart
aufgeprallt (ein *Hit*, mit Punkt und Tempo)? Und welches Gelenk ist über seiner Schwelle?

**Meldungen kosten Zeit.** Darum schaltet die Spiel-Laufzeit sie nur dort ein, wo das Spiel sie auch
liest: an Sensoren, am Spieler, an Dingen mit Aufprallgeräusch und in Levels mit Logik (den
Spielregeln aus [Kapitel 9](09-logik.md)).

Man kann einen Körper auch direkt fragen: An welchen Punkten berührst du gerade etwas? Wen berührst
du, und wie fest? So spürt die Micro Duck, ob ihr Fuß den Boden berührt und ob sie sich selbst
anstößt ([Kapitel 13](13-micro-duck.md)).

> **Mission:** Schalte im Physics Lab unter **Fenster › Anzeigen** (Window › Show) – oder im Menü
> **Anzeigen** oben im Viewport – **Kollisionsformen** (Collision Shapes), **Gelenke** (Constraints)
> und **Kontaktpunkte** (Contact Points) ein und starte **Simulieren** (Simulate). Jetzt siehst
> du, womit Box3D wirklich rechnet: die einfachen Formen statt der hübschen Meshes, die Gelenke und
> die Punkte, an denen sich Dinge berühren.

## 6.11 Der Maus-Griff

Wenn du beim **Simulieren** (Simulate) etwas mit der Maus packst, hängt es an einer **unsichtbaren Feder**. Deine Maus
zieht am einen Ende, das Ding hängt am anderen.

![Der Maus-Griff: Die Maus zieht über eine Feder an der Kiste](../abbildungen/maus-feder.svg)

Weil es eine Feder ist und kein Teleport, bleibt das Ding ein echter Physikkörper: Es stößt an,
kippt, schwingt und fliegt beim Loslassen mit seinem Schwung weiter. Die Feder ist so gedämpft,
dass sie nicht nachwippt. Und sie schafft höchstens das **100-Fache der Gewichtskraft**. Eine
15-kg-Brückenplanke wiegt 15 · 9,81 ≈ 147 N, die Feder zieht also höchstens mit rund 14 700 N.
Zusammen mit den über 10 000 N, mit denen die Brücke schon in Ruhe zieht, reicht das, um das
Gelenk in der Mission oben (Schwelle 20 000 N) zu brechen.

Normalerweise darf sich das Ding am Griff frei drehen. Hebst du die Micro Duck hoch, hält eine
zweite Feder sie aufrecht, wie eine Hand, statt dass sie am Griffpunkt baumelt. Feste und
kinematische Körper kann man nicht greifen.

## 6.12 Der Character Mover

Spielfiguren laufen anders als Kisten rollen. Kern nutzt dafür Box3Ds **Character Mover**: Die
Figur ist eine **Kapsel**, die ihre Bewegung selbst ausrechnet, statt sich von Kontakten
herumschubsen zu lassen. Läuft sie gegen eine Kiste, schiebt sie die Kiste an.

Der Trick für Treppen ist eine **Pogo-Feder**: Die Kapsel schwebt auf einer unsichtbaren Feder ein
Stück über dem Boden, wie auf einem Pogo-Stab. Kommt eine Stufe, die niedriger ist als dieser
Abstand, federt die Kapsel einfach hinauf.

![Die Kapsel schwebt auf einer Pogo-Feder; eine Stufe, die niedriger ist als die Feder, nimmt sie federnd mit](../abbildungen/pogo-feder.svg)

Pro Schritt rechnet der Mover: bremsen, beschleunigen, springen, fallen. Dann schaut er, welche
Wände und Böden er berührt, und schneidet die Bewegung an ihnen ab. **Gegen eine Wand drücken macht
also nicht schneller**, du gleitest an ihr entlang. Zu steile Flächen gelten nicht als Boden.

Die Spielertypen **Figur** (Character) und **Soldat (Ego-Shooter)** (Soldier (first person shooter))
nutzen den Mover ([Kapitel 8](08-spiel-laufzeit.md)). Die Figur fällt dabei 1,5-mal so stark wie die
Welt, damit Sprünge knackiger wirken; der Soldat fällt mit der echten Schwerkraft. Die genauen Werte stehen im blauen Buch, Kapitel 6.

## 6.13 Der Takt: Zeitschritt und Sub-Steps

Box3D bekommt **immer gleich lange Schritte**, egal wie schnell dein Bildschirm ist. Lange Schritte
würden Federn aufschaukeln, und jeder Computer käme woanders an (Anhang C.2). Die **Schrittrate**
stellst du im Level ein: Standard sind 60 Schritte pro Sekunde, nie weniger als 30 verwenden.

Wie passt das zu Bildern, die mal 10 ms und mal 25 ms dauern? Mit einem **Zeit-Sparschwein**: Jedes
Bild wirft seine Zeit hinein. Immer wenn genug für einen ganzen Schritt drin ist, rechnet die Physik
einen Schritt und nimmt die Zeit wieder heraus. Ein Beispiel bei 60 Hz (ein Schritt dauert
1/60 s ≈ 16,7 ms): Ein Bild dauert 25 ms, also ein Schritt, 8,3 ms bleiben im Schwein. Das nächste
Bild dauert wieder 25 ms, jetzt sind 33,3 ms drin, also zwei Schritte.

Kerns Sicherungen dabei:

- **Kein Teufelskreis:** Hängt der Computer einmal lange, rechnet Kern pro Bild höchstens 0,1 s
  nach. Sonst könnte jeder Nachholschritt das nächste Bild noch langsamer machen, und die Physik
  holte nie mehr auf – wie jemand, der so viele Hausaufgaben nachholen will, dass er dabei die
  neuen verpasst. Im Notfall fallen lieber ein paar Schritte weg.
- **Schritte zählen:** Die Spielzeit ist „Anzahl Schritte geteilt durch Schrittrate“. Würde Kern
  stattdessen jedes Mal 0,0167 dazuzählen, sammelten sich winzige Rundungsfehler an, denn ein
  Computer rundet nach jeder Rechnung ein klein wenig (mehr dazu in 6.14).
- **Eingaben gelten pro Schritt.** Auch das Netz, das die Ente steuert, zählt Schritte, nicht
  Sekunden.

Gezeichnet wird **zwischen** den letzten beiden Physikständen: Ist vom nächsten Schritt erst 40 %
der Zeit vergangen, zeigt das Bild den Körper bei 40 % des Weges. So sieht die Bewegung bei jedem
Bildschirm flüssig aus. Die Kamera folgt dieser Lage. Würde sie stattdessen dem letzten echten
Physikstand folgen, wäre sie mal ein bisschen vor, mal ein bisschen hinter dem gemalten Auto, und
ein schnelles Auto würde im Bild hin und her wackeln.

**Sub-Steps** teilen jeden Schritt noch einmal (Standard 4, im Level 1 bis 16). Kleinere Schritte
halten lange **Gelenkketten** fester zusammen. Der Grund ist wie bei **Stiller Post**: Zieht man an
einem Ende der Kette, muss die Korrektur von Glied zu Glied weitergesagt werden, und pro
Teilschritt kommt sie nur ein Stück weit. Mehr Teilschritte heißt: Die Nachricht kommt öfter und
weiter durch (Anhang C.2). An der Hüfte der Ente wuchs die Lücke bei 4 Sub-Steps unter Last auf
5 cm, bei 16 blieb sie unter 1 mm. Fügt Studio einen Roboter ein, stellt es darum
16 Sub-Steps und den Takt des Roboters ein (Micro Duck: 100 Hz) und sagt dir das.

Eine Feder darf nicht schneller schwingen, als die Sub-Steps „hinschauen“. Sonst sieht die Physik
die Feder nur zufällig mal hier, mal da und rechnet Unsinn. Um ein Hin-und-Her zu erkennen, muss
man es **mindestens zweimal pro Schwingung** anschauen: einmal, wenn die Feder hin ist, einmal,
wenn sie her ist. Schaut man nur einmal pro Schwingung, sieht die Feder jedes Mal gleich aus, als
stünde sie still. Darum darf eine Feder höchstens halb so oft schwingen, wie hingeschaut wird.
Kern begrenzt jede Feder so:

**höchste Federfrequenz = 0,5 · Schrittrate · Sub-Steps**

Bei 60 Hz und 4 Sub-Steps schaut die Physik 60 · 4 = 240-mal pro Sekunde hin. Die Hälfte davon
sind 0,5 · 240 = 120 Hz: höchstens 120 Schwingungen pro Sekunde.

> **Mission:** Starte im Physics Lab **Simulieren** (Simulate) und stoß das Pendel an. Drück dann die
> Taste **Pause**, auf Laptops ohne diese Taste **Alt+Umschalt+P** (oder im Menü **Spielen › Pause**
> (Play › Pause)), und danach mehrmals **.** (Punkt). Jeder
> Druck ist genau ein fester Schritt: Das Pendel rückt jedes Mal ein winziges Stück weiter.
> Schrittrate und Sub-Steps findest du unter **Welteinstellungen** (World Settings) ›
> **Physik (Box3D)** (Physics (Box3D)) als **Schrittrate** (Step rate) und **Teilschritte** (Sub-steps).

> **Für Profis:** Box3D selbst begrenzt Gelenke und Kontakte noch strenger. Servogelenke bekommen
> eine besonders steife Einstellung, damit leichte Glieder sich nicht dehnen. Die Werte stehen im
> blauen Buch, Kapitel 6.

## 6.14 Determinismus

**Deterministisch** heißt: gleicher Anfang, gleiche Eingaben, **genau** gleiches Ergebnis, bis auf
das letzte Bit. Kern schafft das auf PCs, auf Macs und als WebAssembly, der Form, in
der Kern im Browser läuft.

Warum ist das schwer? Computer **runden** nach jeder Rechnung, und dann zählt die Reihenfolge:

```
(0,1 + 0,2) + 0,3  =  0,6000000000000001
0,1 + (0,2 + 0,3)  =  0,6
```

Ein winziger Unterschied, aber Physik ist **chaotisch**: Aus einem Unterschied im letzten Bit wird
in Sekunden ein anderer Schritt der Ente. Würde jeder Computer ein bisschen anders runden, lernte
der Roboter auf jedem Computer etwas anderes, und Fehler ließen sich nicht nachstellen.

Kern sorgt deshalb dafür, dass überall **in derselben Reihenfolge und mit denselben Regeln**
gerechnet wird:

- **Fester Takt** (6.13).
- **Ein einziger Rechenkern** für Box3D (6.3), damit die Reihenfolge immer gleich ist.
- **Keine Abkürzungen beim Übersetzen.** Kern ist als Text geschrieben. Bevor ein Computer es
  ausführen kann, wird dieser Text in Computerbefehle übersetzt. Dabei darf nichts umsortiert oder
  zusammengefasst werden, sonst ändert sich wie oben die Reihenfolge.
- **Eigene Rechenregeln für sin, cos und Co.** Das sind Rechenarten für Winkel, die du später in
  Mathe lernst. Die fertigen Rechenregeln, die jedes System mitbringt, liefern dafür je nach
  Computer minimal andere Ergebnisse. Darum bringt Kern seine eigenen mit.

Geprüft wird das mit einem **Fingerabdruck**: Die Ente läuft 400 **Regeltakte**. Ein Regeltakt
ist ein Mal, in dem das Netz der Ente neue Befehle gibt. Nach jedem Takt wird ihr ganzer Zustand
zu einer langen Prüfnummer verrechnet. Ändert sich auch nur ein Bit, ändert sich die Nummer. Alle
400 Nummern müssen eine gespeicherte Liste treffen. Wie das genau geht, erklärt
[Kapitel 2](02-bauen-und-testen.md), Abschnitt 2.9.

> **Für Profis:** Geprüft wird mit GCC auf x86, Apple clang auf ARM und WebAssembly. Box3D läuft
> ohne Vektorbefehle, alles wird ohne zusammengefasste Multiplikation-plus-Addition übersetzt, und
> Kern bringt eine eigene Mathe-Bibliothek mit (Anhang C.9 und blaues Buch, Kapitel 6).

## 6.15 Die Regeln

Kern hat ein verbindliches Regelwerk für alle, die Physik ändern oder einen Roboter, ein Fahrzeug
oder eine Gliederpuppe bauen. Es stützt sich auf Erin Cattos eigene Ratschläge und auf Messungen in
Kern. Die wichtigsten Grundsätze, jeder mit seinem Grund:

1. **Lieber das Modell anpassen als die Physik.** Box3D bleibt, wie es ist. Passt etwas nicht, wird
   das Modell skaliert, anders gebaut oder feiner gerechnet.
2. **Nicht zu klein, nicht zu ungleich.** Bewegte Teile zwischen 10 cm und 10 m (6.2). Nachbarteile
   höchstens etwa 10-mal so schwer wie das andere. Hängt Schweres an Leichtem, muss die Physik
   jede Korrektur viele Runden lang durch das leichte Teil weiterreichen, und bis dahin dehnen sich
   die Gelenke (Anhang C.6). Ist ein Glied sehr leicht, bekommt es lieber eine **Mindestmasse**,
   statt dass man die Physik verstellt: Die 5-g-Hüftbügel der Ente wiegen in Kern mindestens 20 g
   (in echter Größe gerechnet). Und ein starres Teil ist **ein** Körper mit mehreren Formen, nicht
   mehrere zusammengeklebte Körper. Denn zusammengeklebte Körper hält die Physik nie ganz starr
   zusammen; sie wackeln gegeneinander wie ein Turm aus Klötzen mit Klebeband.
3. **Immer gleich lange Schritte** (6.13), für Roboter mehr Sub-Steps.
4. **Gelenke in der Ruhelage bauen**, und jeder Motor bekommt eine **Höchstkraft**. Ein Motor ohne
   Grenze wäre unendlich stark, so etwas gibt es nicht.
5. **Keine Hilfskräfte, die es in echt nicht gibt.** Kein heimliches Aufrichten, kein Festhalten,
   kein direktes Setzen von Tempo bei Körpern mit Gelenken. Einmal wurde versucht, die Ente mit so
   einem „Kreisel“ direkt über ihr Tempo gerade zu halten. Das kämpfte gegen die Gelenke, und der
   Rumpf sprang. Ein Netz, das mit Hilfskräften laufen lernt, kann es am echten Roboter nicht.
   Soll ein Körper sanft aufrecht bleiben, nimmt man stattdessen ein **Parallel-Gelenk**: eine
   weiche Feder mit Höchstkraft, die ihn aufrichtet, wie eine Hand im Rücken, die nachgibt.
6. **Ein Roboter muss fallen können.** Bei Laufrobotern wird darum nie eine Richtung gesperrt. Und
   Spieler und Roboter schlafen nie, denn langsames Gehen sähe für Box3D wie Stillstand aus.
7. **Dauerkräfte jeden Schritt neu geben**, denn eine Kraft gilt nur einen Schritt. Ein Sprung ist
   dagegen ein einzelner Impuls.
8. **Kinematische Körper fahren, statt zu springen.** Eine Plattform bekommt ein Tempo oder ein
   Ziel, das sie bis zum nächsten Schritt erreichen soll – wie eine Rolltreppe, die fährt, statt sich
   woandershin zu beamen. Mitten im Spiel einfach umsetzen kostet Box3D doppelt Arbeit. Und Vorsicht:
   Ein Ziel, zu dem der Körper nur ganz langsam kriechen müsste, „verschläft“ Box3D, weil so ein
   langsamer Körper für Box3D wie ein schlafender aussieht (6.4).

Zu jedem neuen Roboter gehört ein **Test**, der drei Dinge prüft: Er steht. Seine Gelenke halten
(die Lücke in einem Gelenk bleibt unter 1 % der Gliedlänge, bei einem 10-cm-Bein also unter 1 mm).
Und er macht jedes Mal genau dasselbe. Das ganze Regelwerk mit Checkliste steht im blauen Buch,
Kapitel 6.

## 6.16 Wie die Spiel-Laufzeit die Physik nutzt

Ein Spiel baut seine Körper nicht von Hand, sondern aus dem Level: Erst **plant** die
Spiel-Laufzeit, welche Körper und Formen nötig sind, dann baut sie die **Körper**, dann die
**Gelenke** (ein Gelenk braucht ja zwei fertige Körper, die es verbinden kann).

Warum erst ein Plan? Weil Kern Studio **denselben Plan** benutzt, um im Editor die Kollider zu
zeichnen. Was du im Editor als Physikform siehst, ist also genau das, womit später im Spiel
gerechnet wird. Es gibt keine zweite Fassung, die heimlich anders sein könnte.

In jedem Schritt geht es dann in fester Reihenfolge: erst **Steuerung** (Spieler, Animationen,
Verkehr und die Servos aus 6.17 sagen, was sie wollen), dann rechnet **Box3D**, dann kommen die
**Meldungen**. So rechnet die Physik immer mit den frischen Befehlen dieses Schritts, und das
Spiel liest danach, was dabei passiert ist. Wie das im Einzelnen abläuft, steht in
[Kapitel 8](08-spiel-laufzeit.md).

## 6.17 Das Servomodell – Brücke zur Micro Duck

Ein **Servo** ist ein kleiner Motor mit Getriebe und eingebautem Regler, der ein Gelenk auf einen
befohlenen **Zielwinkel** dreht. Box3D kennt keine Servos, nur Körper und Gelenke. Darum baut Kern
den Servo als kleines **Rechenmodell** nach, mit Werten, die am echten Servo gemessen wurden.

> **So kannst du es dir vorstellen:** Der Servo zieht das Gelenk wie ein **Gummiband** zum
> Zielwinkel. Je weiter es weg ist, desto stärker zieht er. Aber nie stärker, als sein Motor kann.
> Dazu kommt ein Stoßdämpfer, der zu schnelles Drehen bremst.

Das Modell hat vier Teile:

1. **Feder:** zieht zum Ziel, umso stärker, je größer der Abstand ist.
2. **Höchstkraft:** Ab einem bestimmten Abstand zieht der Servo nicht mehr stärker. Kern erreicht
   das mit einem Trick: Es setzt das Ziel nie weiter weg, als der Motor ziehen kann. Beim Gummiband
   heißt das: Hältst du sein Ende nie weiter als 10 cm weg, kann es nie stärker ziehen als bei 10
   cm, egal wie weit weg du eigentlich hinwillst.
3. **Reibung im Getriebe:** Ein kleines Gegendrehmoment hält das Gelenk fest, bis genug Kraft
   anliegt.
4. **Verzögerung:** Ein Befehl kommt erst etwa 20 ms nach dem Senden beim Servo an. Kern lässt die
   Befehle darum durch eine **Warteschlange** laufen, wie Briefe, die einen Tag unterwegs sind.

Kern baut das nur aus Box3Ds eigenen Teilen:

- **Die Feder ist die Gelenkfeder des Scharniers.** Box3D rechnet diese Feder so, dass sie bei
  jeder Steifigkeit ruhig bleibt und sich nie aufschaukelt. Darum ist sie die sichere Wahl.
- **Die Reibung ist ein Gelenkmotor mit Tempo 0.** Klingt seltsam, passt aber: Ein Motor, der
  stillstehen will, aber nur wenig Kraft hat, hält das Gelenk fest, bis jemand stärker drückt.
  Dann gibt er nach. Genau so fühlt sich Reibung an.

Der Servo ist dabei mit Absicht **nachgiebig** wie ein Gummiband und keine starre Regelung, die das
Gelenk stur auf ein Tempo zwingt. Der Grund: Das Netz der Ente wurde mit einem weichen Servo
trainiert. Bekäme es in Kern einen harten, wäre das ein anderer Roboter.

Dieselbe Rechnung treibt Kern Studio, das Training ([Kapitel 14](14-lehrer.md)) und den Prüfstand
für die Kalibrierung.

Die Werte stehen in einem **Profil**: Steifigkeit, Dämpfung, Reibung, Höchstkraft, Trägheit des
Motors und Verzögerung. Die **Trägheit des Motors** kommt so zustande: Im Servo dreht sich ein
kleiner Motor über ein Getriebe sehr schnell mit. Dadurch lässt sich das Gelenk schwerer andrehen,
als das Bein allein vermuten lässt. Box3D kennt so etwas nicht. Kern täuscht es nicht mit einer
erfundenen Zahl vor. Stattdessen setzt Kern eine
echte dünne Scheibe mit derselben Trägheit auf die Gelenkachse, also etwas, das es in Box3D wirklich
geben kann. Das ist eine feste Regel in Kern: Was Box3D gar nicht abbilden kann, wird
aufgeschrieben, nicht mit erfundenen Zahlen vorgetäuscht.

Ein anderer Servo ist einfach ein anderes Profil. Weil keine Messung ganz genau ist, steht im Profil
auch, **wie sicher** jeder Wert ist. Beim Training bekommt jeder Servo absichtlich leicht
verschiedene Werte aus diesem Bereich. So lernt das Netz, mit leicht verschiedenen Servos
klarzukommen, statt sich auf eine Zahl zu verlassen, die niemand genau weiß – wie eine Fußballerin,
die auf vielen verschiedenen Plätzen trainiert und darum auf jedem Rasen sicher spielt. Eingebaut ist der Servo
der Micro Duck, so wie Pollen Robotics ihn trainiert hat. Wie Profile gemessen werden, zeigt
[Kapitel 16](16-kalibrierung.md).

Die Vergrößerung aus 6.2 passiert **genau einmal**, an der Grenze zu Box3D (aus dem Grund aus 6.1).
Auch die Verzögerung wird umgerechnet: Die Zeit läuft in der Welt doppelt so langsam, also werden
aus 20 ms echt 40 ms Weltzeit. Bei 100 Schritten pro Sekunde dauert ein Schritt 10 ms, die
Warteschlange ist also **4 Schritte** lang.

> **Für Profis:** Als Formel: Drehmoment = Steifigkeit · (Ziel − Winkel), begrenzt auf die
> Höchstkraft, minus Dämpfung · Drehtempo. Anhang C.7 rechnet das mit den echten Zahlen des Servos
> vor. Die Werte des Profils und alle Namen stehen im blauen Buch, Kapitel 6.

## Level geschafft!

- Du weißt, warum Kern das Modell anpasst und nie Box3D.
- Du kannst erklären, warum die Ente vierfach groß, 64-mal schwer und halb so schnell simuliert
  wird und sich trotzdem genauso bewegt: Die Schwerkraft bleibt gleich, also wächst die Zeit mit.
- Du kennst die drei Körperarten, die Formen und wofür die Gelenke da sind.
- Du verstehst Paar-Reibung, Rollwiderstand und brechende Gelenke.
- Du weißt, warum die Physik in festen Schritten rechnet und auf jedem Gerät gleich herauskommt.
- Du kannst erklären, wie ein Servo als Gummiband mit Grenze und Verzögerung nachgebaut wird.

## Rätsel-Ecke

1. Welche Körperart passt zu einer Plattform, die stur ihrer Bahn folgt?
2. Socke (Strick) auf Holzboden: Welche Reibung gilt? Und ohne Oberflächen, wenn eine Form
   Reibung 0 und die andere 0,9 hat?
3. 100 Hz, 16 Sub-Steps: Wie oft pro Sekunde darf eine Feder höchstens schwingen?
4. **Boss-Rätsel:** Ein Roboter hat 20 ms Verzögerung, wird mit S = 9 vergrößert und läuft mit
   150 Schritten pro Sekunde. Wie lang ist die Warteschlange?

### Lösungen

1. **Kinematisch** (Kinematic): Sie folgt der Vorgabe und lässt sich nicht bremsen.
2. Mit Oberflächen der Paarwert **0,4**. Ohne Oberflächen gilt die Faustregel, und
   0 mal 0,9 ist 0, also **0**: Die Form bleibt rutschig wie Eis.
3. Die Physik schaut 100 · 16 = 1600-mal pro Sekunde hin. Eine Feder muss mindestens zweimal pro
   Schwingung gesehen werden, also höchstens 1600 ÷ 2 = **800 Hz**.
4. Die Zeit wächst mit der Wurzel aus S, und die Wurzel aus 9 ist 3 (denn 3 · 3 = 9). Aus 20 ms
   werden 60 ms Weltzeit. In einer Sekunde (1000 ms) gibt es 150 Schritte, in 60 ms also
   150 · 60 ÷ 1000 = **9 Schritte**.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
