# 5 Szene und Geometrie – Was die Kamera sieht und woraus die Stadt gebaut ist

> **Kurz gesagt:** Hier geht es um zwei Werkzeugkisten neben dem Renderer. Die erste (`kern::scene`)
> beantwortet Fragen wie „Muss ich das überhaupt zeichnen?“ und „Was liegt unter dem Mauszeiger?“.
> Die zweite (`kern::geo`) baut Formen: Würfel, Häuser aus Grundrissen, Straßen, Gelände. Am Ende
> zaubert Kern daraus eine ganze Stadt.

Den Renderer zeigt [Kapitel 4](04-grafik.md), die Mathematik [Anhang B](B-grundlagen-rendering.md).

## 5.1 Die Spielregeln

- **Z zeigt nach oben**, gemessen wird in Metern – in Grafik, Physik, Ton und allen Bauern.
- Alle Bauer liefern ihr Ergebnis im selben Behälter, **`MeshData`**: Listen mit Positionen,
  Normalen (wohin eine Fläche „schaut“), Farben in sRGB, Texturkoordinaten (UVs) und optional Indizes.
- Ohne Indizes ist ein Mesh eine **Triangle Soup** („Dreieckssuppe“): je drei Ecken ein Dreieck.
- Dazu kann ein **Zusatzstrom** kommen: ein paar Extra-Zahlen pro Ecke für einen eigenen Shader. Der
  Renderer hat dafür zwei Steckplätze; `as_input` reicht das Mesh samt Zusatzstrom an den Renderer.
  Wählt man einen Steckplatz ab 2, bleibt der Zusatzstrom weg.

## 5.2 Frustum-Culling: Was nicht im Bild ist, wird nicht gezeichnet

Die Kamera sieht nur einen Ausschnitt der Welt in Form einer abgeschnittenen Pyramide, das
**Frustum**. Was ganz draußen liegt, muss nicht gezeichnet werden. Das Aussortieren heißt **Culling**.

> **Stell dir vor …** du stehst im Theater mit einer Taschenlampe. Bevor du jemanden anleuchtest,
> fragst du: Steht er überhaupt auf der Bühne? Wer hinter dem Vorhang wartet, kostet dich nichts.

Kern prüft **eine Kugel pro Mesh** gegen die **sechs Wände** des Frustums: sechs kleine Rechnungen statt tausender Ecken – viel
billiger als das Zeichnen. Die Wände liest Kern direkt aus der Kameramatrix ab (Verfahren nach Gribb und Hartmann) und
normiert sie so, dass jede Rechnung den echten Abstand in Metern liefert. Liegt der Kugelmittelpunkt
mehr als einen Radius hinter einer Wand, ist die Kugel sicher draußen.

Der Test ist **vorsichtig**: Eine Kugel nahe einer Ecke der Pyramide wird manchmal unnötig gezeichnet,
aber nie fälschlich weggelassen. Kaputte Kugeln gelten als sichtbar.

Die Kugeln rechnet der Renderer beim Hochladen aus: eine um das Mesh und eine in der Welt um alle
seine **Instanzen** (Kopien an verschiedenen Orten). Für deren Größe nimmt er eine Kennzahl der Matrix
(die Frobenius-Norm), die garantiert mindestens so stark streckt wie die Matrix selbst.

> **Achtung, Falle!** Alle Kopien eines instanzierten Meshes teilen sich **eine** Kugel. Ein Wald,
> dessen Bäume über die ganze Karte verstreut sind, ist darum fast immer „sichtbar“. Teile solche
> Instanzen besser in räumliche Gruppen auf.

Nie aussortiert werden **geskinnte** Meshes (Figuren mit Knochen, deren Pose die Kugel verlassen kann)
und Meshes, deren Material das Culling abschaltet. Der Schattenpass nutzt denselben Test gegen das
Frustum der Sonne.

> **Probier's aus!** Öffne Kern Studio auf https://kernengine.de/ und schalte unter
> **Window › Show** den Punkt **Bounds** ein. Jetzt siehst du die Hüllen, mit denen Kern rechnet.

## 5.3 Die BVH: Kartons in Kartons

Klickst du in eine Stadt, muss Kern das getroffene Dreieck unter zehntausenden finden. Alle einzeln
prüfen? Viel zu langsam. Die Lösung heißt
**BVH** (*Bounding Volume Hierarchy*, Hierarchie von Hüllquadern).

> **Stell dir vor …** du suchst eine Schraube im Keller. Die Schrauben liegen in Schachteln, die
> Schachteln in Kisten, die Kisten in Regalen. Steht auf einem Regal „nur Farbe“, öffnest du darin
> keine einzige Schachtel.

**Bauen:** Jedes Dreieck bekommt eine Box. Kern sucht die Richtung, in der die Dreiecke am weitesten
verteilt sind, und teilt sie dort genau in der Mitte (am **Median**). Jede Hälfte bekommt eine Box
drumherum, und das geht weiter, bis höchstens 8 Dreiecke in einer Box liegen. Weil jede Ebene
halbiert, ist der Baum höchstens 32 Ebenen tief.

**Suchen:** Trifft der Strahl eine Box, geht es zu beiden Kindern – **zuerst zum näheren**. Findet
Kern dort früh einen Treffer, fallen alle ferneren Boxen beim nächsten Test raus.

Eine BVH bekommt nur **stillstehende Weltgeometrie**: keine Linien, eine einzige unbewegte Instanz,
nicht geskinnt, mindestens 36 Dreiecke. Wird das Mesh bewegt oder kopiert, verwirft der Renderer sie;
steht es wieder still, baut er sie neu. Alles andere wird Dreieck für Dreieck geprüft.

## 5.4 Picking: Was liegt unter dem Mauszeiger?

Kern rechnet den Pixel (gemessen im Zeichenpuffer) in Koordinaten von −1 bis 1 um und projiziert ihn rückwärts durch die Kamera
auf die nahe und die ferne Wand des Frustums. Die Verbindung ist der **Bildschirmstrahl**. Bei einer
perspektivischen Kamera startet er im Auge, bei einer orthografischen (ohne Perspektive) laufen alle
Strahlen parallel.

Dann, für jedes Mesh, das infrage kommt:

1. **Grobtest:** Verfehlt der Strahl die Weltkugel oder liegt sie hinter dem besten Treffer? Erledigt.
2. **Feintest:** mit BVH durch den Baum, sonst jedes Dreieck jeder Instanz. Bei animierten Figuren
   rechnet Kern die aktuelle Pose aus den Knochen aus. **Was du siehst, kannst du anklicken.**
3. **Dreieckstest** nach Möller-Trumbore. Rückseiten zählen nur bei zweiseitigen Materialien.

Der Treffer verrät Mesh, Instanz, Dreieck, Entfernung, Trefferpunkt und Flächenrichtung. Im Beispiel
`examples/hello` färbt ein Klick die getroffene Kiste rot:

```cpp
PickHit hit;
if (engine.renderer.pick(in.pointer().x, in.pointer().y,
                         {}, hit) && hit.mesh == boxes_) {
  tints_[hit.instance * 4 + 0] = 1.0f;
  tints_[hit.instance * 4 + 1] = 0.25f;
  tints_[hit.instance * 4 + 2] = 0.2f;
}
```

Zeile 1 legt einen leeren Treffer an. Zeile 2–3 fragen: Liegt etwas unter dem Mauszeiger, und ist es
das Kisten-Mesh? `{}` heißt Standardeinstellungen: bis 1800 m weit, unsichtbare Meshes und Linien
zählen nicht. Man kann außerdem ein Mesh ausnehmen und Ebenen (Layer) wählen; eine kleine Statistik
zählt geprüfte Dreiecke und per Kugel verworfene Meshes. Die drei Zeilen
danach färben genau diese Kiste: viel Rot, wenig Grün und Blau.

Kern Studio pickt genauso (nur auf der Szenen-Ebene), ordnet Meshes ihren Actors zu und schießt den Strahl durch Boden und
gesperrte Actors weiter. Lichter und leere Actors wählst du über ihr Symbol.

> **Probier's aus!** Klicke im Viewport verschiedene Objekte an und beobachte, wie der Outliner
> mitspringt. Jeder Klick ist ein Strahl auf der Suche nach dem nächsten Dreieck.

## 5.5 Earcut: Vielecke in Dreiecke schneiden

Ein Grundriss ist ein Vieleck, oft L-förmig (nicht konvex) und manchmal mit Löchern (Innenhöfen). Der
Grafikprozessor kann nur Dreiecke. Kern zerschneidet Vielecke mit **Earcut**, einer C++-Umsetzung von
Mapbox Earcut v3.0.2 (ISC-Lizenz), die laut ihrem Kopfkommentar dieselben Ergebnisse liefert wie
die JavaScript-Vorlage.

> **Stell dir vor …** du hast ein Papiervieleck und eine Schere. Du suchst eine Ecke, deren Dreieck
> mit den beiden Nachbarecken ganz im Papier liegt – ein **Ohr**. Schnipp! Übrig bleibt ein Vieleck
> mit einer Ecke weniger. Weiter, bis nur ein Dreieck bleibt.

Jedes einfache Vieleck hat mindestens zwei Ohren; aus n Ecken werden so n − 2 Dreiecke. Ein Ohr ist
eine nach außen gewölbte Ecke, in deren Dreieck keine andere Ecke liegt.

- **Schmutzige Daten:** Findet Earcut einmal rundherum kein Ohr, entfernt es doppelte und auf einer
  Linie liegende Punkte, repariert dann kleine Selbstüberschneidungen und teilt notfalls das Vieleck
  an einer Diagonale in zwei.
- **Löcher:** Ohren gehen nur bei einem einzigen Rand. Darum schneidet Earcut von jedem Loch eine
  schmale **Brücke** zum äußeren Rand (Verfahren nach Eberly), beginnend beim Loch, das am weitesten
  links liegt. Danach ist alles ein Rand.
- **Große Vielecke:** Ab mehr als 80 Ecken bekommt jede Ecke eine Hausnummer auf einer **z-Kurve**:
  Die Bits von x und y werden wie ein Reißverschluss verzahnt. Nahe Punkte haben meist ähnliche
  Nummern, und Earcut prüft nur noch die Nachbarschaft statt aller Ecken.

Die Funktion `deviation` misst, wie genau die Dreiecke die Fläche treffen (0 heißt exakt).

> **Wusstest du?** Nach dem Schneiden vergleicht Kern die Fläche aller Dreiecke mit der des
> Grundrisses. Weicht sie um mehr als 0,7 % ab, wird das Gebäude weggelassen, statt mit kaputtem Dach
> in der Stadt zu stehen.

## 5.6 Primitive: der Baukasten

`kern::primitives` baut Grundformen mit Z nach oben: Quader (auch auf dem Boden stehend), Kugel,
Kapsel, Torus (Donut), Zylinder, Kegel und Ebene. Mit `append` hängst du Formen mit Verschiebung und
Drehung aneinander, sodass **ein** Mesh entsteht, das schneller zu zeichnen ist (die Normalen dreht
`append` richtig mit, den Zusatzstrom kopiert es nicht). Orb Run baut so sein Auto aus drei Quadern,
und alle Bodenplatten der Stadt werden ein einziges Mesh.

## 5.7 Welding: doppelte Ecken verschmelzen

Häuser- und Straßenbauer liefern Dreieckssuppe. Wo sechs Dreiecke zusammenstoßen, steht dieselbe Ecke
sechsmal im Speicher.

Ein **indiziertes Mesh** speichert jede Ecke einmal und dazu eine Liste von Nummern. **Welding** macht
aus Suppe ein indiziertes Mesh. Kern verschmilzt nur Ecken, die in **allem**
bitgenau gleich sind: Position, Normale, Farbe, UV, Zusatzwerte. Darum ist es verlustfrei: Eine harte
Kante bleibt hart, weil die Flächen dort verschiedene Normalen haben. Die Zwillinge findet eine
Hashtabelle (ein Schnellsuchverzeichnis). Und Kern indiziert nur, wenn die Ersparnis an Bytes eine
wählbare Mindestgröße übersteigt; sonst bleibt die Suppe, wie sie ist. Welding nimmt nur Suppe an
(ohne Indizes) und höchstens vier Zusatzwerte pro Ecke; eine kleine Statistik meldet Ecken und Bytes
vorher und nachher.

> **Wusstest du?** Bei einem echten Ausschnitt von Konstanz mit 1300 Gebäuden brauchen Bau und Welding
> in Kern etwa 45 ms – die frühere JavaScript-Variante etwa 1260 ms.

Ein Fingerabdruck aller Zahlen (`content_hash`) verrät, ob ein neu gebautes Stück sich vom alten
unterscheidet.

## 5.8 Häuser aus Grundrissen

> **Stell dir vor …** eine Plätzchenform, die du in eine dicke Teigplatte drückst: oben und unten die
> Form des Grundrisses, dazwischen senkrechte Wände. Dieses In-die-Höhe-Ziehen heißt **Extrudieren**.

Der `BuildingBuilder` extrudiert Grundrisse zu Häusern mit Dach, Wänden und optional einem Gesims (dem
vorstehenden Band unter der Dachkante). Die Grundrisse kommen als lange Zahlenliste, damit auch
JavaScript sie leicht übergeben kann. Pro Haus stehen darin ein **Seed** (Zufallszahl für Farben und
Fassade), Fuß- und Traufhöhe, optional ein Mittelpunkt und die Ringe: erst der äußere Rand, dann die
Innenhöfe. In C++ schreibt `FootprintStream` diese Liste für dich. Ohne Mittelpunkt entscheidet der
erste Punkt des Außenrands, in welchen Chunk das Haus fällt.

Pro Haus passiert:

1. **Aufräumen:** Punkte näher als 1,5 cm am Vorgänger fliegen raus; zu kleine Ringe (unter 3 Punkte
   oder 0,08 m²) werden leer.
2. **Ausrichten:** Außenrand gegen den Uhrzeigersinn, Löcher im Uhrzeigersinn.
3. **Aussortieren:** leere Ringe, Wände unter 10 cm, mehr als 0,7 % Flächenfehler.
4. **Farben** und Stockwerkshöhe (3,1 bis 3,6 m) bestimmt der Seed.
5. **Dach** aus den Earcut-Dreiecken, nach oben gedreht, Innenhöfe bleiben offen; **Wände** an jeder
   Kante (auch der Höfe), mit UVs in Metern entlang und über dem Fuß.
6. **Gesims:** 0,55 m hoch, 0,24 m vorstehend, nur an Kanten über 1,2 m und Wänden über 3,5 m.

Jede Ecke trägt dabei vier **Zusatzwerte**: Wandlänge (oder eine Markierung für Dach bzw. Gesims),
Wandhöhe, einen Wert aus dem Seed und die Stockwerkshöhe. Daraus zeichnet die Shader-Erweiterung
`NightWindows` aus [Kapitel 4](04-grafik.md) nachts leuchtende Fensterraster.

Damit eine große Stadt das Bild nicht einfriert, baut der Bauer mit **Zeitbudget**: `step(4.0)`
arbeitet etwa 4 ms und meldet, ob er fertig ist; dazwischen wird weitergezeichnet. Die Häuser landen in
quadratischen **Chunks** (standardmäßig 320 m, mindestens 20 m; höchstens 2600 Gebäude), jeder ein
eigenes Mesh. Größere Chunks heißen weniger Zeichenaufrufe, kleinere besseres Culling. Orb Run und die
Spiel-Laufzeit bauen ihre kleine Stadt mit `step(1e9)` in einem Zug. Zähler melden gebaute und
übersprungene Häuser und Dreiecke; Gesims und Zusatzwerte lassen sich abschalten.

## 5.9 Straßen

Der `RoadBuilder` legt Straßen als **Bänder** aufs Gelände: optional Bordstein, dann Fahrbahn und
gestrichelte Mittellinie (3 m lange Striche alle 7 m, ab 1 m). Eingabe ist wieder eine Zahlenliste: pro
Straße Breite, Schalter, zwei Farbnummern aus einer Palette und die Punkte. Damit sie Hügeln folgen, wird jedes Stück in
Abschnitte von höchstens 6 m geteilt, und jede Ecke fragt nach der Bodenhöhe.

| Schicht | Höhe über Gelände | Breite |
|---|---|---|
| Bordstein | 9 cm | Straße + 3,4 m |
| Fahrbahn | 11,2 cm | Straßenbreite |
| Markierung | 13,5 cm | 42 cm |

> **Achtung, Falle!** Liegen zwei Flächen exakt gleich hoch, weiß der Grafikprozessor nicht, welche
> vorne ist, und sie flackern um die Wette: **Z-Fighting**. Die gestaffelten Höhen verhindern das.

Die Bodenhöhe liefert ein **HeightSampler**, eine kleine Funktion „gib mir die Höhe bei (x, y)“; ohne
ihn ist der Boden flach. Der Quelltext nennt ihn „physics truth“: Das Spiel soll dieselbe Funktion wie
seine Physik liefern, damit Autos genau auf der sichtbaren Straße fahren. Liefert er keine endliche
Zahl, bricht der Bauer mit Fehler ab. In JavaScript setzt du einfach `kern.heightSampler = (x, y) =>
…`. Straßen außerhalb eines gewählten Radius werden übersprungen – so baust du einen Ausschnitt einer
großen Karte.

## 5.10 Gelände-Kacheln

`build_terrain_tile` baut Landschaft als indiziertes Gitter mit 1 bis 128 Feldern pro Seite; jede
Gitterecke fragt den HeightSampler, jedes Feld wird an der Diagonale Südwest–Nordost in zwei Dreiecke
geteilt. Die UVs laufen über die Kachel von 0 bis 1, und die Funktion meldet die kleinste und größte
Höhe (nützlich für Hüllen oder Detailstufen). Die Normalen ergeben sich aus dem Höhenunterschied der Nachbarn. Weil die Funktion dafür
eine Reihe über den Rand hinaus abtastet, passen Nachbarkacheln **nahtlos** aneinander.

Der `GeoProjector` rechnet GPS-Koordinaten in Meter östlich und nördlich eines Ursprungs um
(Erdradius 6 378 137 m). So kommen Grundrisse aus Kartendaten direkt in Kerns Welt.

## 5.11 Geometrie aus JavaScript

Alle Bauer gibt es auch in JavaScript: Job häppchenweise laufen lassen, dann jeden Chunk mit `commit`
verschweißen, hochladen und seinen Speicher freigeben (der Renderer hat ja eine eigene Kopie);
`commit` meldet die Bytes vor und nach dem Welding. Mit `extraKey` wählst du den
Erweiterungsstrom für die Zusatzwerte; einen unbekannten Namen lehnt `commit` mit Fehler ab, statt still
einen anderen zu nehmen. `upsertTerrainTile` baut und lädt eine Gelände-Kachel in einem Aufruf, und mit
`weld: true` verschweißt Kern eine selbst hochgeladene reine Dreieckssuppe mit Normalen vorher (ohne
Skinning, Linien und mit höchstens einem Zusatzstrom; sonst kommt sie unverändert an).

## 5.12 Die prozedurale Stadt

Jetzt kommt alles zusammen. **Procedural City** ist der Stadtgenerator von Orb Run als Baustein für
jedes Level. „Prozedural“ heißt: nicht von Hand modelliert, sondern aus Regeln und einem Seed erzeugt –
Gebäude, Straßen, Bodenplatten, Bäume, Kollisionskörper und Verkehr.

![Ein exportiertes Spiel: die prozedurale Stadt mit Gebäuden, Straßen und Bäumen](../buch/bilder/game.png)

| Einstellung | Standard | Grenzen / Bedeutung |
|---|---|---|
| Seed | 20261022 | Startwert des Zufalls |
| Blocks per side | 7 | 1 bis 16 |
| Block pitch | 80 m | Straße zu Straße, ab 30 |
| Street width | 16 m | ab 6, bis halber Pitch |
| Tallest building | 84 m | in der Mitte |
| Trees / Traffic cars | an / 22 | Bäume, Autos |
| Lit windows at night | an | leuchtende Fenster |

Gleicher Seed, gleiche Stadt (derselbe Zufallsgenerator wie in Orb Run):

1. **Raster:** Blöcke mit 18 cm hohen Bodenplatten, dazwischen Straßen.
2. **Höhe:** Je näher an der Mitte, desto höher: Die Höchstgrenze wächst von 14 m am Rand bis zur
   eingestellten Höhe in der Mitte (mit dem Quadrat der Zentrumsnähe). Die Mitte wird Hochhausgegend.
3. **Bebauung** pro Block (Grundstück 3 m eingerückt), eine von vier Arten: ein blockgroßes Haus (in
   60 % der Fälle mit 14 m tiefem Innenhof, Kollision dann als vier Flügel), vier Einzelhäuser mit 5 m Gassen, ein L-Haus plus ein kleines oder drei Riegel.
4. **Straßen** an allen Blockkanten (Bandbreite drei Viertel der Straßenbreite) mit Bordstein und
   Mittellinie, 60 m über die Stadt hinaus. Gebaut wird mit den Bauern von oben (Chunks 200 m, sechs
   Wand- und drei Dachfarben), alles geweldet.
5. **Bäume** alle 11 m entlang der Blockränder, mit 35 % Lücken, zufälliger Größe, Drehung und
   Grünton; alle Stämme (Zylinder) sind ein
   instanziertes Mesh, alle Kronen (Kegel) ein zweites.

Beim Hochladen wird jeder Chunk ein Mesh. Verschiebst du die Stadt, ändert sich nur eine Matrix; neu
gebaut wird nur, wenn sich eine Einstellung wirklich ändert (das prüft ein Fingerabdruck der
Einstellungen). Die Zusatzwerte für die Fenster bekommen Gebäude nur, wenn „Lit windows at night“ an
ist. Straßen werfen keinen Schatten und sind zweiseitig und ganz matt. Steht die Stadt im Ursprung
ohne Drehung, bekommt jeder Chunk eine BVH, und Picking in der Stadt läuft über sie.

Für die Physik wird die ganze Stadt **ein** statischer Box3D-Körper mit einem Quader pro Gebäudeflügel
und Bodenplatte (Reibung 0,5, siehe [Kapitel 6](06-physik.md)). Die Autos parken im Editor. In Play und
Simulate fahren sie als kinematische Körper ihre Spur entlang (9 bis 15 m/s, fünf Farben), springen am
Stadtrand auf die andere Seite und halten, wenn der Spieler weniger als 10 m vor ihnen steht. Alle Autos sind ein einziges instanziertes
Mesh. Eine
Hilfsfunktion liefert zufällige Straßenpunkte, etwa zum Verteilen von Orbs.

> **Probier's aus!** Öffne den Content Drawer (Ctrl+Space) und setze aus der Gruppe **World** eine
> **Procedural City** ins Level. Ändere in Details den **Seed** oder **Blocks per side** – jedes Mal
> eine neue Stadt. Mit Simulate (Alt+S) fährt der Verkehr los.

## 5.13 Formen im Level und eingebaute Meshes

Ein **Static Mesh** hat eine Form: Box, Sphere, Cylinder, Cone, Capsule, Plane, Torus, Ramp oder Mesh.
Kern baut daraus ein weißes Mesh (die Farbe kommt vom Material). Quader, Ebene und Rampe bekommen
Texturkoordinaten in Metern, damit eine Textur überall gleich groß aussieht. Jede Form sitzt mittig
auf dem Actor. Die Rampe ist ein Keil: an +X voll hoch, an −X flach. Helfer liefern dazu die Hülle
der Form, Punkte für eine konvexe Hülle und mit `fit_collider` einen passenden Kollisionskörper (Box,
Kugel, Kapsel, sonst Hülle) – mehr in [Kapitel 6](06-physik.md).

Die Form **Mesh** verweist auf ein fest einkompiliertes Dreiecksmesh. Heute sind das die Glieder der
Micro Duck, die Meshes von Pollen Robotics in vereinfachter Form, mit Namen wie `duck/trunk_base` oder
`duck/neck` (die volle Liste liefert `builtin_mesh_names()`); vereinfacht hat sie das Werkzeug
`tools/micro_duck_import.py`. Sie stehen unter CC BY-NC-SA 4.0 (nur nicht kommerziell), sind darum hier nicht abgebildet,
und ein kommerzieller Build lässt sie weg. Gespeichert sind sie platzsparend (Positionen in 16 Bit,
Normalen und Farben in 8 Bit, dazu Hüllpunkte für die Kollision) und werden beim ersten Zugriff
ausgepackt. Weil der Lehrer ([Kapitel 14](14-lehrer.md)) auf mehreren Threads baut, schützt nativ ein
Schloss (Mutex) diesen Vorrat; im Browser gibt es nur einen Thread. Im Level lässt sich ein solches
Mesh pro Achse strecken; bei einer Spiegelung dreht Kern den Umlaufsinn der Dreiecke mit um.

> **Wusstest du?** Ein unbekannter Mesh-Name ergibt absichtlich einen 10-cm-Würfel. So fällt der
> Fehler im Level sofort auf, statt dass einfach nichts zu sehen ist.

Wie daraus die Ente wird, erzählt [Kapitel 13](13-micro-duck.md).

## Das nimmst du mit

- Z ist oben, Einheit ist der Meter, alle Bauer liefern `MeshData`.
- Culling testet eine Kugel pro Mesh gegen sechs Wände – billig und nie fälschlich unsichtbar.
- Eine BVH sind Kartons in Kartons; sie macht Picking schnell.
- Picking schießt einen Strahl vom Pixel in die Szene, auch auf animierte Figuren.
- Earcut schneidet Ohren ab, verbindet Löcher über Brücken und nutzt bei großen Vielecken eine z-Kurve.
- Welding verschmilzt nur bitgleiche Ecken – verlustfrei und nur, wenn es spart.
- Häuser und Straßen entstehen in Chunks unter Zeitbudget; die Höhenfunktion teilen sie mit der Physik.
- Procedural City macht aus einem Seed eine Stadt mit Bäumen, Kollision und Verkehr.

## Knobelecke

1. Warum testet Kern beim Culling eine Kugel statt aller Dreiecke?
2. Ein Wald aus 5000 Bäumen als ein instanziertes Mesh wird nie aussortiert. Was hilft?
3. Ein Vieleck hat 12 Ecken und keine Löcher. Wie viele Dreiecke macht Earcut daraus?
4. Warum bleibt eine harte Würfelkante nach dem Welding hart?
5. Was passiert, wenn Fahrbahn und Mittellinie exakt gleich hoch lägen?

### Lösungen

1. Der Test muss viel billiger sein als das Zeichnen; sechs Abstandsrechnungen kosten fast nichts.
2. Die Bäume in räumliche Gruppen mit eigener Kugel aufteilen; Gruppen außerhalb des Bildes fallen weg.
3. n − 2 = 10 Dreiecke.
4. An der Kante haben die Flächen verschiedene Normalen, die Ecken sind also nicht bitgleich.
5. Z-Fighting: Die Flächen flackern, weil nicht klar ist, welche vorne liegt.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
