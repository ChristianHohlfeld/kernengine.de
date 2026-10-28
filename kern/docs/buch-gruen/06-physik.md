# 6 Physik – Wie Dinge fallen, rollen und zusammenhalten

> **Kurz gesagt:** Kern lässt Kisten fallen, Bälle rollen, Türen schwingen und Roboter laufen. Die
> Rechenarbeit macht **Box3D**, eine Physik-Engine von Erin Catto (der 3D-Nachfolger von Box2D v3).
> Kern legt eine schmale Schicht darüber. Du lernst Körper, Formen, Materialien, Gelenke, Kontakte,
> den Takt der Simulation und das Servomodell der Micro Duck kennen.

Die Grundlagen dahinter (Integration, Kollision, Solver, Regelkreise, Determinismus) erklärt
[Anhang C](C-grundlagen-physik.md). Hier geht es darum, **wie Kern sie benutzt**.

![Das Beispiel-Level Physics Lab in Kern Studio: Stapel, Pendel, Tür, Seilbrücke, Drehteller, Lift und Rover](../buch/bilder/studio-physics-lab.png)

Eine Grundregel vorweg, weil sie alles prägt: **Box3D wird nie an Kern angepasst.** Weder sein
Quelltext noch seine Konstanten werden verändert, und Kern dreht nicht an Solver-Stellschrauben, um ein
Modell „zum Laufen zu bringen“. Passt ein Modell nicht, wird das **Modell** angepasst (skaliert, Massen
verteilt, mehr Sub-Steps), nicht die Physik. Was ein echtes Gerät zusätzlich tut, etwa ein Servo mit
Regler und Reibung, bekommt ein eigenes kleines Gerätemodell in echten Einheiten (Abschnitt 6.17).

## 6.1 Aufbau der Schicht

Box3D liegt unverändert unter `third_party/box3d` und wird in dasselbe WebAssembly-Modul übersetzt wie
der Rest. Darüber sitzt `kern::physics` – die **einzige Stelle**, an der Kern mit Box3D spricht.
Gebaut wird sie mit `kern/build.sh --physics` ([Kapitel 2](02-bauen-und-testen.md)).

Die Abmachungen:

- Meter, Sekunden, Kilogramm, **Z oben** – wie im Renderer.
- Winkel sind in der Physik **Radiant**. Grad gibt es nur in Leveldateien und im Details-Panel.
- Körper, Formen und Gelenke sind **Handles** (Nummern, 0 heißt „keins“); die `World` besitzt alles.
- Fehler liefern ein ungültiges Handle und eine Erklärung in `last_error()`
  ([Kapitel 3](03-grundlagen.md)). Jede Eingabe wird geprüft: Eine Pose mit `NaN` oder ein negativer
  Radius erreicht Box3D nie.

## 6.2 Die richtige Größe

Box3D rechnet mit festen Toleranzen **in Metern**: Körper dürfen sich um 5 mm durchdringen („Linear
Slop“), Kontakte entstehen schon ab 20 mm Abstand („Speculative Distance“), Hüllquader haben bis 50 mm
Reserve. Daraus folgt die erste Regel: **Bewegte Körper gehören zwischen 0,1 m und 10 m, nie unter
1 cm.** Die Spiel-Laufzeit setzt das durch: Bewegte Formen werden auf mindestens 1 cm Kantenlänge
angehoben (Kugeln 5 mm Radius), feste dürfen bis 2 mm dünn sein.

> **Wusstest du?** Gemessen: Die Micro Duck in echter Größe hat Glieder von 2–4 cm. Ihr Rumpf sank
> innerhalb von 10 Frames 9 cm durch die Beine. In vierfacher Größe halten die Gelenke.

### Skalieren statt Toleranzen verstellen

Box3D könnte seine Toleranzen umstellen, aber nur für die **ganze** Welt – und in einem Kern-Level
stehen Autos neben kleinen Robotern. Kern skaliert deshalb das **Modell** mit einem Faktor S, und zwar
jede Größe passend zu ihrer Dimension (Froude-Ähnlichkeit). Für die Micro Duck ist S = 4:

| Größe | Faktor | bei S = 4 |
|---|---|---|
| Länge | S | ×4 |
| Zeit, Geschwindigkeit | √S | ×2 |
| Masse | S³ | ×64 |
| Drehmoment, Steifigkeit | S⁴ | ×256 |
| Trägheitsmoment | S⁵ | ×1024 |
| Schwerkraft | 1 | bleibt 9,81 m/s² |

> **Stell dir vor …** Filmleute drehen Szenen mit Modellschiffen und spielen sie langsamer ab, damit
> die Wellen „echt“ wirken. Kern macht es umgekehrt: Der Roboter wird größer, und eine echte Sekunde
> dauert in der Welt zwei. Die Bewegung bleibt dieselbe – nur in Einheiten, mit denen Box3D gut umgehen
> kann.

Das ist ein **Wechsel der Einheiten**, kein größerer Roboter; nach außen (Netze, Logs, Kalibrierung)
geht alles in echten Einheiten. Ein Test baut ein Servo-Bein in 1-, 4- und 8-facher Größe: Frei hängend
laufen 1× und 4× **Bit für Bit gleich**. Auf dem Boden, wo die festen Toleranzen wirken, liegen sie nur
0,23 bis 0,27 mm auseinander.

Fest eingestellte Werte (Kontaktsteifigkeit 30 Hz, Gelenkweichheit der Servos 240 Hz …) werden
**kalibriert, nicht skaliert**. Abweichungen zum echten Roboter kommen also aus den Modellen von Servo,
Kontakt und Massen, die an Messungen abgeglichen werden ([Kapitel 16](16-kalibrierung.md)). Eine
Garantie für sicheres Verhalten auf echter Hardware ist die Simulation nicht.

## 6.3 Die Welt

Alles beginnt mit einer Welt und ihren Einstellungen:

```cpp
physics::WorldSettings ws;
physics::World world;
if (!world.create(ws)) { /* last_error() */ }
world.step(1.0f / 60, 4);   // ein Schritt, 4 Sub-Steps
```

Zeile 1–3 erschaffen eine Welt mit Standardeinstellungen (oder melden, warum nicht). Zeile 4 rechnet
eine Sechzigstelsekunde weiter, aufgeteilt in vier Teilschritte.

Die wichtigsten Standards: Schwerkraft 9,80665 m/s² nach unten, ruhende Körper dürfen schlafen,
Continuous Collision ist an, Stöße unter 1 m/s prallen nicht ab, Kontakte sind 30 Hz steif mit
Dämpfung 10 und werden höchstens mit 3 m/s auseinandergedrückt, Höchstgeschwindigkeit 400 m/s.

Zwei Dinge setzt Kern immer selbst: **einen einzigen Arbeitsthread** (WebAssembly läuft einfädig, und
es macht die Reihenfolge eindeutig) und eine eigene **Reibungsfunktion** für Materialpaare (6.6). Weil
der Lehrer viele Welten auf mehreren Threads betreibt ([Kapitel 14](14-lehrer.md)), schützt nativ ein
Schloss das Erzeugen und Zerstören von Welten.

Ein `step` leert die Ereignisse des letzten Schritts, ignoriert unsinnige Zeitschritte, rechnet mit 1 bis
64 Sub-Steps, zählt mit und sammelt die neuen Ereignisse (6.10). Wie groß der Schritt ist, entscheidet
der Aufrufer – im Spiel `GameWorld::step` (6.13).

## 6.4 Körper

Box3D kennt drei Arten von Körpern (Bodies):

> **Stell dir vor …** ein Theater. Der Bühnenboden bewegt sich nie: **fixed** (statisch). Die
> Hebebühne fährt genau so, wie die Technik es befiehlt, und schiebt alles mit, was draufsteht:
> **kinematic**. Und der Ball, den ein Schauspieler wirft, gehorcht Schwerkraft, Stößen und Reibung:
> **dynamic**.

Beim Anlegen gibst du Art, Anfangspose, Geschwindigkeiten, Dämpfung, Schwerkraftfaktor und einen
freien Benutzerwert an. Mit `bullet` prüft Box3D einen schnellen Körper auch gegen andere bewegte
Körper lückenlos.

Zur Laufzeit kannst du Pose und Geschwindigkeit lesen und setzen, einen kinematischen Körper zu einer
Zielpose fahren lassen, Achsen sperren (`set_motion_locks`) und Kräfte oder Impulse aufbringen:

- Eine **Kraft** wirkt genau **einen** Schritt. Dauerlasten müssen jeden Schritt neu gesetzt werden.
- Ein **Impuls** ist ein einmaliger Stoß (Newtonsekunden), etwa für einen Sprung.

Masse, Schwerpunkt und Trägheit ergeben sich normalerweise aus den Formen und ihrer Dichte. Mit
`set_mass` setzt du nur die Masse (die Trägheit wächst passend mit). Mit `set_mass_data` setzt du alles
einzeln – der Weg für importierte Roboterbeschreibungen (MJCF, URDF), die ihre gemessenen Werte
mitbringen. Die Kollisionsformen bestimmen dann nur noch den Kontakt.

## 6.5 Formen

Damit ein Körper anstoßen kann, bekommt er **Formen** (Shapes), gern auch mehrere (ein *Compound*).

| Form | Was du angibst |
|---|---|
| `box` | volle Kantenlängen, Lage |
| `sphere` | Radius |
| `capsule` | Radius, Länge entlang Z |
| `hull` | mindestens 4 Punkte: konvexe Hülle |
| `mesh` | Dreiecksnetz, **nur an festen Körpern** |

Eine **konvexe Hülle** ist die Form, die entsteht, wenn du ein Gummituch um eine Punktwolke spannst
(mehr in [Anhang C](C-grundlagen-physik.md)). Abgelehnt wird alles Unsinnige: Radius ≤ 0, flache
Hüllen, ein Mesh an einem bewegten Körper. Bei Meshes verschweißt Kern Ecken mit 0,5 mm Toleranz und
lässt Box3D innere Kanten erkennen. So stolpert nichts über „Geisterkanten“ zwischen zwei Dreiecken.

### Material und Filter

Jede Form hat ein Material: **Dichte** (Standard 1 kg/m³), **Reibung** (0,6), **Restitution** (Sprungkraft,
0), **Rollwiderstand** (0), eine **Oberfläche** (6.6) und den Schalter **Sensor** – dann meldet die Form
nur Überlappungen und stößt nichts an. Dazu kommen Kollisionsfilter:

- **Kanäle:** Jede Form hat Kategorie-Bits und Masken-Bits. Zwei Formen kollidieren nur, wenn jede in
  der Maske der anderen vorkommt.
- **Gruppen:** Gleiche positive Gruppe kollidiert immer, gleiche negative nie. Praktisch für Roboter,
  deren Glieder sich nicht gegenseitig stören sollen.
- **Ereignis-Schalter:** welche Meldungen die Form erzeugt (6.10).

`hull_mesh` baut die Dreiecke genau der Hülle, die kollidiert; Studio zeichnet sie als Linien.

## 6.6 Oberflächen mit Paar-Reibung

Treffen zwei Formen aufeinander, braucht der Kontakt **einen** Reibwert. Box3D nimmt normalerweise
√(a · b), die Wurzel aus dem Produkt (warum, steht in [Anhang C](C-grundlagen-physik.md)). Für
Restitution nimmt es das Maximum. Für die meisten Dinge reicht das. Aber der Quelltext sagt es treffend:
*TPU auf Baumwolle hat seinen eigenen gemessenen Wert, nicht das Mittel aus TPU auf TPU und Baumwolle
auf Baumwolle.*

Deshalb kennt Kern **Oberflächenmaterialien**: `none`, `cloth`, `tpu` und `floor`. Für bestimmte Paare
gibt es eigene Werte (Haftreibung trockener Paare):

| Paar | Reibung | Herkunft |
|---|---|---|
| cloth – cloth | 0,5 | Baumwollstrick auf Strick |
| cloth – tpu | 0,8 | weiche Schnabelpolster auf Baumwolle |
| cloth – floor | 0,4 | Baumwolle auf Holz/Laminat |

Kern ändert Box3D dafür nicht, sondern nutzt zwei vorgesehene Andockstellen: Die Oberfläche wird als
Material-Nummer in die Form geschrieben, und die Welt bekommt Kerns Reibungsfunktion:

```cpp
float pair_friction(float fa, uint64_t ma,
                    float fb, uint64_t mb) {
  const f32 f = ma && mb && ma <= 3 && mb <= 3
      ? surface_friction(Surface(ma), Surface(mb)) : -1.0f;
  return f >= 0 ? f : sqrtf(fa * fb);
}
```

Box3D ruft sie bei jedem Kontakt mit den Reibwerten (`fa`, `fb`) und Materialnummern (`ma`, `mb`)
beider Formen auf. Zeile 3–4: Haben beide eine echte Oberfläche (1 bis 3), wird in der Paartabelle
nachgeschaut; gibt es keinen Eintrag, kommt −1 zurück. Zeile 5: Gibt es einen Wert, gilt er, sonst die
übliche Wurzel. Ist eine Seite `none`, zählen also immer die Reibwerte der Formen.

> **Achtung, Falle!** Wer eine neue Oberfläche ergänzt, muss auch die Grenze `<= 3` anpassen, ihren
> Namen eintragen und ihre Paare in die Tabelle schreiben. Sonst wird sie stillschweigend ignoriert.

Im Level steht die Oberfläche als Text (`"cloth"` usw.) an drei Stellen: am automatischen Collider
eines Meshes, an einem eigenen Collider und am endlosen Boden. Ein eigenes Feld im Details-Panel gibt es
dafür (Stand dieses Codes) nicht; sie wird im Leveldokument gesetzt. Unbekannte Namen werden `none`.

Die Micro Duck nutzt alle drei ([Kapitel 13](13-micro-duck.md)): Die Klumpen ihrer Socke sind `cloth`,
die gedruckten Polster am Schnabel `tpu`, und im Beispiel-Level ist der Holzboden `floor`. Ihre Sohlen
haben **keine** Oberfläche: Sie behalten die Reibwerte, mit denen ihr Netz trainiert wurde (1,0 auf
1,0 ergibt √1 = 1).

## 6.7 Reibung, Sprungkraft, Rollwiderstand

**Reibung** stellst du pro Form ein. Im Editor: Details › Mesh › **Physics** für den automatischen
Collider oder Details › Collider für eigene. Den endlosen Boden findest du unter World Settings ›
Physics (Box3D) › **Ground friction** und **Ground bounciness**.

**Restitution** (0 bis 1) entscheidet, wie stark etwas abprallt; Box3D nimmt das Maximum beider
Formen. Stöße unter 1 m/s prallen gar nicht ab. **Kontaktweichheit** (Contact stiffness 30 Hz, Contact
damping ratio 10, Contact push-out speed 3 m/s) steht ebenfalls in den World Settings.

**Rollwiderstand** bremst rollende Dinge. Box3D nimmt das Maximum beider Formen mal einen „Rollradius“
(Radius bei Kugeln und Kapseln, ein Viertel des Innenradius bei Hüllen – auch Quader sind in Box3D
Hüllen) und begrenzt damit das bremsende Moment. Kern setzt ihn gezielt ein:

- der Ball im Micro-Duck-Level: 0,03 („ein Gummiball auf Holzboden rollt aus“),
- die Reifen des Rovers im Physics Lab: 0,02,
- die Sockenklumpen: 0,5 („ein Stoffklumpen rollt nicht; runde Klumpen würden es, und der Haufen
  krabbelt“).

**Roboterfüße** sind breite, flache Quader oder Hüllen mit hoher Reibung.

> **Probier's aus!** Öffne über **File › Example Levels** das **Physics Lab**, wähle eine Kiste und
> setze im Details-Panel unter **Collider** die Restitution hoch (die Kisten haben eigene Collider;
> Mesh › Physics gilt nur für den automatischen). Starte Simulate (Alt+S) und wirf die Kiste
> mit der Maus auf den Boden.

## 6.8 Gelenke

Ein **Gelenk** verbindet zwei Körper und schränkt ein, wie sie sich zueinander bewegen dürfen. Das
Lehrbuchbeispiel ist eine Tür:

```cpp
physics::JointSettings j;
j.type = physics::JointType::revolute;
j.body_a = frame_body;  j.body_b = door;
j.frame_a = physics::frame_from_axis(
    world.local_point(frame_body, hinge), {0, 0, 1}, false);
j.frame_b = physics::frame_from_axis(
    world.local_point(door, hinge), {0, 0, 1}, false);
j.enable_limit = true;  j.lower = -1.7f;  j.upper = 1.7f;
const physics::Joint joint = world.create_joint(j);
```

Zeile 2 wählt ein Drehgelenk (*revolute*, ein Scharnier). Zeile 3 sagt: Es verbindet Türrahmen und
Tür. Zeile 4–7 bauen für beide Körper einen **Frame** (ein kleines Koordinatenkreuz) am Scharnierpunkt,
dessen Z-Achse senkrecht nach oben zeigt – um diese Achse dreht die Tür. Zeile 8 begrenzt den Winkel
auf ±1,7 Radiant (gut ±97°), damit die Tür nicht durch die Wand schwingt. Zeile 9 baut das Gelenk.

Ein paar Abmachungen: Drehgelenke, Kugelgelenke und Parallel-Gelenke arbeiten **um die Z-Achse** ihres
Frames, Schiebe- und Radgelenke **entlang der X-Achse**. Der Nullwinkel liegt dort, wo beide Frames
übereinstimmen.

| Gelenk (Editor) | Box3D | Alltagsbild |
|---|---|---|
| Hinge | revolute | Türscharnier |
| Slider | prismatic | Schublade |
| Ball | spherical | Schultergelenk, mit Kegelgrenze |
| Fixed | weld | Schweißnaht (auch weich) |
| Distance / rope | distance | Stange oder Seil |
| Wheel | wheel | Rad mit Federung und Antrieb |
| Motor | motor | zieht B zu einer Lage relativ zu A |
| Parallel | parallel | hält etwas weich aufrecht |
| No collision | filter | verhindert nur Kollision |

Viele Gelenke haben drei Extras: eine **Feder** (Frequenz in Hertz und Dämpfungsgrad), **Grenzen** und
einen **Motor** (Sollgeschwindigkeit und Höchstkraft bzw. -moment). Ein Seil ist ein Distance-Gelenk
mit Höchstlänge. Verbundene Körper kollidieren standardmäßig nicht miteinander. Und jedes Gelenk hat
eine **Weichheit im Solver** (Box3Ds Standard: 60 Hz, Dämpfung 2). Ein Gelenk, das einen Körper mit sich
selbst verbinden will, wird abgelehnt.

Zur Laufzeit kannst du Motoren schalten, Scharnierwinkel lesen, die **Zwangskraft** abfragen (wie stark
das Gelenk gerade hält) und Gelenke entfernen.

### Wenn Gelenke brechen

Mit Bruchschwellen (im Editor *Break force* und *Break torque*) meldet Box3D ein Gelenk, sobald es zu
stark belastet wird. Die Spiel-Laufzeit zerstört es dann, zählt mit, schreibt „Joint on "…" broke“ ins
Log und spielt den Bruch-Sound. So reißt die Seilbrücke im Physics Lab.

> **Probier's aus!** Starte im Physics Lab Simulate und ziehe mit der Maus kräftig an einer Planke der
> Seilbrücke: Die Kette gibt nach und schwingt. (Reißen wird sie so kaum – der Maus-Griff hat eine
> begrenzte Kraft, die schwachen Gelenke brechen erst unter großer Last.) Schalte unter
> **Window › Show** die Punkte **Collision Shapes** und **Constraints** ein, dann siehst du Formen
> und Gelenke.

### Die Gelenkfeder in echten Einheiten

Box3D stellt Federn als Frequenz und Dämpfungsgrad ein. Servos und Roboterbeschreibungen rechnen aber
in Newtonmeter pro Radiant (Steifigkeit) und Newtonmetersekunden pro Radiant (Dämpfung).
`set_joint_spring` rechnet an der Grenze um, genau wie Box3D intern: über die **effektive Masse** des
Gelenks, also wie schwer sich die Drehung um die Achse anfühlt. Weil die von der Lage abhängt, wird die
Funktion **jeden Schritt** aufgerufen. Box3D löst die Feder implizit; sie ist bei jeder Steifigkeit
stabil. Die Formeln stehen in [Anhang C](C-grundlagen-physik.md).

## 6.9 Abfragen: Strahlen, Kugeln, Explosionen

- **`cast_ray`** schießt einen Strahl und meldet den nächsten Treffer: Punkt, Normale, Körper, Form und
  wie weit entlang der Strecke. Sensoren werden übersprungen, ebenso ein Körper, den du ausnimmst – oder
  eine ganze Liste (ein Robotersensor sieht so seine eigenen Glieder nicht).
- **`cast_sphere`** schiebt eine Kugel statt eines Strahls: ein „dicker Strahl“ für Bodenproben, die
  keine Kante verfehlen.
- **`explode`** verteilt einen Stoß kugelförmig um einen Punkt; ein negativer Wert macht eine Implosion.
- **`push_body`** schubst einen Körper gerade so stark, dass er einem bewegten Schieber nicht im Weg
  bleibt. Dazu kommen zwei Kapsel-Abfragen für den Character Mover (6.12).

## 6.10 Kontakte und Ereignisse

Nach jedem Schritt liegen die Ereignisse **dieses** Schritts bereit: Formen beginnen oder hören auf
sich zu berühren, ein Besucher betritt oder verlässt einen Sensor, ein Stoß war heftig genug für ein
**Hit-Event** (mit Punkt und Aufprallgeschwindigkeit), ein Gelenk hat seine Bruchschwelle überschritten.

> **Achtung, Falle!** Ereignisse kosten Rechenzeit. Die Spiel-Laufzeit schaltet sie nur dort ein, wo das
> Spiel sie liest: Sensor-Ereignisse an Sensoren und Spielern (bei Logik-Graphen an allen bewegten
> Formen), Kontakt-Ereignisse nur am Spieler (für Pickups und Trigger), Hit-Ereignisse an Spieler,
> Körpern mit Aufprall-Sound oder bei Logik.

Zwei Abfragen lesen Kontaktdaten direkt: `contact_points` liefert alle Berührpunkte eines Körpers mit
Normale, Eindringtiefe und Normalimpuls – Studio zeichnet sie mit dem Show-Flag **Contact Points**. Und
`touching` sagt, welche Körper einer gerade berührt und wie fest. Das ist genau das, was ein
Kontaktsensor meldet, etwa eine Sohle auf dem Boden; die Micro Duck nutzt es für ihre Füße.

## 6.11 MouseGrab: Körper mit der Maus ziehen

> **Stell dir vor …** du hängst einen Gegenstand an ein Gummiband und ziehst am anderen Ende. Er folgt
> dir, schwingt, stößt an, und wenn du loslässt, fliegt er mit Schwung weiter.

Genau so funktioniert `MouseGrab`: Ein unsichtbarer kinematischer „Maus-Körper“ folgt dem Zeiger, und
ein Motor-Gelenk mit Feder verbindet ihn mit dem Griffpunkt. Die Feder hat 7,5 Hz und ist kritisch
gedämpft (sie schwingt nicht nach). Ihre Kraft ist auf das Hundertfache des Gewichts begrenzt. Drehen
darf sich der Körper frei – er kippt und pendelt am Griff. Mit `hold_rotation` hält der Griff zusätzlich
die Ausrichtung fest, wie eine Hand. Das nutzt Studio für den Rumpf der Micro Duck, damit sie
hochgehoben aufrecht bleibt. Feste und kinematische Körper lassen sich nicht greifen.

> **Probier's aus!** In Simulate greifst du jeden dynamischen Körper mit der linken Maustaste (oder dem
> Finger). Zeiger nach oben hebt, Mausrad holt näher oder schiebt weg, Loslassen wirft.

## 6.12 Der Character Mover

Für Spielfiguren gibt es den `CharacterMover`, gebaut auf Box3Ds experimenteller Mover-Schnittstelle.
Die Figur ist eine Kapsel, die **außerhalb des Solvers** läuft: Sie kämpft nicht gegen Kontakte, sondern
rechnet ihre Bewegung selbst und schubst dynamische Körper an, in die sie läuft.

Standards: 0,4 m Radius, 1,8 m Höhe, 6 m/s, Flächen über 50° sind kein Boden, und eine eigene,
knackigere Schwerkraft von 20 m/s².

Pro Schritt passiert:

1. Reibung, Beschleunigung Richtung Wunsch, Sprung, Schwerkraft.
2. **Pogo-Feder:** Eine Kugelprobe misst den Abstand zum Boden, und eine gedämpfte Feder hält die Kapsel
   auf Stufenhöhe (0,45 m). So läuft sie Treppen und Rampen hinauf.
3. Bis zu fünfmal: berührte Ebenen sammeln, die Bewegung dagegen lösen und den Rest an den Ebenen
   abschneiden. Gegen eine Wand drücken baut also keine Geschwindigkeit auf.
4. Dynamische Körper in den berührten Ebenen bekommen einen Stoß.

Die Spiel-Laufzeit nutzt den Mover für die Spieler „Character“ und „Soldier“ ([Kapitel 8](08-spiel-laufzeit.md)).

## 6.13 Der Takt: Zeitschritt und Sub-Steps

> **Stell dir vor …** ein Daumenkino. Jedes Blatt ist ein Physikschritt. Wenn die Blätter immer
> gleich weit auseinanderliegen, läuft der Film ruhig. Würdest du mal ein, mal fünf Blätter
> überspringen, wäre alles ruckelig – und eine Feder könnte sich „aufschaukeln“.

Box3D bekommt **nie einen schwankenden Zeitschritt**. Die Schrittrate ist eine Level-Einstellung
(**Step rate**, Standard 60 Hz, erlaubt 15 bis 480 Hz; nie gröber als 30 Hz). `GameWorld::step` nutzt
einen **Akkumulator** nach Glenn Fiedler („Fix Your Timestep“):

- Jeder Frame zahlt seine Zeit in ein Sparschwein ein. Die Physik hebt sie in festen Portionen ab; der
  Rest bleibt für das nächste Mal.
- Ein langer Frame (etwa ein Hintergrund-Tab) zählt höchstens 0,25 s, und pro Frame laufen höchstens
  0,1 s Simulation. Das verhindert die **„Spiral of Death“**, in der die Physik nie aufholt. Ist man
  trotzdem im Rückstand, fallen ganze Schritte weg.
- Die Simulationszeit **zählt Schritte**, statt Kommazahlen aufzusummieren, die mit der Zeit driften.
- Eingaben gelten pro Schritt; eine Steuerung, die mit 50 Hz rechnet, zählt Schritte, nicht Sekunden.

Gezeichnet wird **zwischen** den beiden letzten Physikständen, gewichtet mit dem Rest im Sparschwein.
So ist Bewegung bei jeder Bildrate glatt. Die Kamera folgt dieser geglätteten Pose; mit der rohen Pose
wackelten schnelle Autos im Bild.

### Sub-Steps

Jeder Schritt wird in Box3D noch einmal in **Sub-Steps** geteilt (World Settings › **Sub-steps**,
Standard 4, im Level 1 bis 16). Lange Gelenkketten dehnen sich mit mehr Sub-Steps weniger.

> **Wusstest du?** Gemessen an der Hüfte der Ente: Bei 4 Sub-Steps wuchs die Gelenklücke unter Last auf
> 5 cm. Bei 16 Sub-Steps bleibt sie unter 1 mm. Fügt Studio einen Roboter ein, stellt es deshalb
> 16 Sub-Steps und dessen Schrittrate ein (Micro Duck: 100 Hz) und meldet das.

Auch die Steifigkeit hängt an den Sub-Steps: Box3D begrenzt Gelenkfedern auf ein Viertel und
Kontaktfedern auf ein Achtel der Sub-Step-Rate. Kern hält zusätzlich jede Feder des Levels unter der
halben Sub-Step-Frequenz. Servogelenke bekommen eine Gelenkweichheit von 240 Hz (oder weniger, wenn die
Grenze es verlangt). Steifer geht also nur mit mehr Sub-Steps.

## 6.14 Determinismus

Kern ist **Bit für Bit deterministisch**: Gleiche Eingaben ergeben auf allen geprüften Plattformen
(GCC auf x86, Apple clang auf ARM, WebAssembly) exakt dasselbe Ergebnis. Die Physik trägt bei durch
festen Zeitschritt, einen einzigen Thread, Box3D ohne SIMD-Abkürzungen, das Abschalten von
FMA-Zusammenfassungen und Kerns eigene Mathebibliothek für Sinus und Co. Warum jeder dieser Punkte nötig
ist, erklärt [Anhang C](C-grundlagen-physik.md).

Geprüft wird mit einem **Fingerabdruck**: Die Ente geht 400 Regeltakte, jeder wird gehasht, und die
Hashes müssen nativ, als WebAssembly und auf ARM dieselbe Referenzdatei treffen
([Kapitel 14](14-lehrer.md)). Die Regel für neuen Code: `kern::dm::` statt `std::` für sin, cos, exp,
log & Co., keine Summen, deren Reihenfolge von Threads abhängt, keine `-ffast-math`.

## 6.15 Die Regeln

`docs/physics.md` ist verbindlich für jeden Code, der Körper, Formen, Gelenke oder Motoren anlegt. Die
Regeln stützen sich auf Erin Cattos eigene Dokumentation und Vorträge und auf Messungen in Kern. Das
Wichtigste:

- **Größen:** Meter, Kilogramm, Sekunden. Bewegte Körper 0,1 bis 10 m. Kleine Modelle als Ganzes
  skalieren, Einheiten nur an der Grenze zu Box3D umrechnen. Tabu: Box3Ds Längeneinheit verstellen.
- **Massen:** Nachbarmassen höchstens etwa 10 : 1. Ein starres Teil ist **ein** Körper mit mehreren
  Formen, nicht mehrere verschweißte. Leichte Glieder bekommen eine Mindestmasse (die 5-g-Hüftbügel der
  Ente wiegen mindestens 20 g).
- **Takt:** fester Zeitschritt, Interpolation, Kamera an der geglätteten Pose, lange Ketten mit mehr
  Sub-Steps, Federn unter der halben Sub-Step-Frequenz.
- **Gelenke:** in der Ruhepose bauen, Grenzen müssen die Null enthalten. Eigenkollision per negativer
  Gruppe oder No-collision-Gelenk filtern. Box3Ds Standard-Weichheit (60 Hz, 2) nicht anfassen, außer
  bei Servos.
- **Motoren:** Immer eine **endliche** Höchstkraft setzen. Servos sind Gelenkfedern (6.17).
  Gelenkreibung ist ein Motor mit Geschwindigkeit 0. Aufrecht halten per Parallel-Gelenk.
- **Kräfte:** Dauerlasten jeden Schritt als Kraft, Sprünge als Impuls, Dämpfung über Box3Ds eingebaute
  Dämpfung.
- **Tabu: die Physik umgehen.** Keine Zusatzmomente, die etwas „nachbilden“, keine gesetzten
  Geschwindigkeiten an Körpern mit Gelenken oder Kontakten, keine Stützkräfte für Roboter. Gemessen:
  Ein „Gyro“ auf Geschwindigkeitsebene ließ den Rumpf der Ente springen.
- **Körper:** an der Endpose erzeugen statt teleportieren, Typ beim Erzeugen festlegen. Bewegungssperren
  nur bei Fahrzeugen, Figuren und Fliegern, **nie bei Walkern** – ein Roboter muss fallen können, sonst
  lernt er nichts Echtes.
- **Schlaf und Ereignisse:** Continuous Collision anlassen, schnelle Spielerfahrzeuge sind Bullets.
  Spieler und Roboter schlafen nie (eine langsame Gangart liegt unter der Schlafschwelle von 5 cm/s).

Die Checkliste für einen neuen gelenkigen Preset fasst das zusammen und verlangt am Ende einen Test:
Der Preset steht, seine Gelenke halten (Lücke unter 1 % der Gliedlänge), und er erfüllt
deterministisch seine Aufgabe.

## 6.16 Wie die Spiel-Laufzeit die Physik nutzt

Spiele bauen ihre Physik aus dem Level ([Kapitel 8](08-spiel-laufzeit.md)): `plan_bodies` plant Körper
und Formen (Studio zeichnet mit demselben Plan die Collider), `create_body` und `create_joints` setzen
ihn um (samt Grad → Radiant, Materialien, Bruchschwellen), und `fixed_step` treibt jeden Schritt
Controller, Verkehr, Servos und Maus-Griff an, ruft `World::step` und verarbeitet die Ereignisse.

## 6.17 Das Servomodell – Brücke zur Micro Duck

Box3D rechnet starre Körper. Ein echter Servo tut mehr: Er regelt, hat Reibung und reagiert verzögert.
Dafür hat Kern ein kleines **Gerätemodell** mit Werten in echten Einheiten, das **Profil**. Ein anderer
Servo ist ein anderes Profil.

Das Gesetz in Worten: Das Moment ist die Steifigkeit K mal Abstand zum Ziel (aber höchstens das
Höchstmoment T), minus die Dämpfung D mal Geschwindigkeit. Dazu kommen Reibung F im Getriebe, die
Trägheit des Rotors (Armature) und eine Verzögerung, bis ein Befehl ankommt. Umgesetzt wird es mit
Box3Ds eigenen Gelenkteilen:

```cpp
void servo_step(physics::World& world,
                const physics::Joint& joint, f32 target,
                f32 stiffness, f32 damping,
                f32 friction, f32 torque) {
  const f32 q = world.joint_angle(joint);
  const f32 k = max_of(1e-6f, stiffness);
  const f32 reach = torque > 0 ? torque / k : 1e9f;
  world.set_joint_spring(joint,
      q + clamp(target - q, -reach, reach), k,
      max_of(0.0f, damping));
  world.set_joint_motor(joint, friction > 0, 0,
                        max_of(0.0f, friction));
}
```

Zeile 5 liest den
aktuellen Winkel. Zeile 7 rechnet aus, wie weit das Ziel höchstens entfernt sein darf: T geteilt durch
K. Zeile 8–10 stellen die Gelenkfeder auf ein Ziel, das nie weiter weg ist – so übersteigt die Kraft nie
das Höchstmoment, genau wie bei einem echten Servo, dessen Ansteuerung sättigt. Zeile 11–12 schalten einen
Motor mit Sollgeschwindigkeit 0 und der Reibung als Höchstmoment ein: Er hält das Gelenk fest, bis mehr
Kraft anliegt. Das ist Erin Cattos Rezept für Gelenkreibung.

Dieselbe Funktion läuft im Studio, im Lehrer und auf dem Kalibrier-Prüfstand, für jedes Servo-Gelenk in
jedem Physikschritt.

### Servos im Level

Ein Hinge-Gelenk wird mit `servo = true` zum Servo. Standards: Ziel 0°, Steifigkeit 50 N·m/rad,
Dämpfung 2 N·m·s/rad, Reibung 0,1 N·m; das Höchstmoment ist die Motorkraft des Gelenks. Servo-Gelenke
bekommen die steifere Gelenkweichheit aus 6.13, damit leichte Glieder sich nicht dehnen.

### Profile und der XL330

Ein Profil (`ServoProfile`, als JSON speicherbar) enthält K, D, F, T, Armature und Verzögerung – und
dazu **Streuungen**. Sie sagen, wie sicher die Werte sind. Das Training zieht für jeden Servo eigene
Werte innerhalb dieser Grenzen, damit das Netz nicht an dem hängt, was die Messung nicht festlegen
konnte. Unsinnige Werte werden abgelehnt.

Das Standardprofil ist der Dynamixel XL330-M288 so, wie Pollen Robotics ihn trainiert (Modell „m6“ aus
BAM – Better Actuator Models von Rhoban – bei 7,4 V):

| Wert | XL330 |
|---|---|
| Steifigkeit K | 0,554 N·m/rad |
| Dämpfung D | 0,053 N·m·s/rad |
| Reibung F | 0,0048 N·m |
| Höchstmoment T | 0,963 N·m |
| Armature A | 0,0018 kg·m² |
| Verzögerung | 20 ms (Training 15–30 ms) |
| Streuung | Armature, Reibung je ±10 % |

### Die Grenze zu Box3D

Beim Eintragen in die Ente wird skaliert – genau an dieser einen Stelle: Steifigkeit, Reibung und
Höchstmoment mal S⁴, Dämpfung mal S⁴ · √S, Trägheit mal S⁵. Die Armature kommt als dünne Scheibe auf
der Gelenkachse dazu. (Eine Trägheit nur um die Achse wäre kein echter starrer Körper, und Box3D würde
damit Energie gewinnen.) Die Verzögerung läuft durch eine Warteschlange in ganzen Physikschritten: bei
100 Hz und S = 4 sind 20 ms echt genau 4 Schritte.

Wie die Micro Duck damit läuft, steht in [Kapitel 13](13-micro-duck.md), wie Profile gemessen werden,
in [Kapitel 16](16-kalibrierung.md).

## 6.18 Tests

Acht Tests sichern alles ab, von Körpern und Ereignissen über alle neun Gelenktypen, Skalierung und
Materialien bis zum Physics Lab samt brechender Brücke, dem Character Mover und dem Fingerabdruck.

## Das nimmst du mit

- Box3D rechnet, Kern passt nie Box3D an, sondern das Modell.
- Bewegte Körper gehören zwischen 0,1 und 10 m; kleine Roboter werden als Ganzes skaliert (Ente: S = 4).
- Es gibt feste, kinematische und dynamische Körper; Formen und Materialien bestimmen den Kontakt.
- Oberflächen wie `cloth`, `tpu`, `floor` geben Materialpaaren eigene, gemessene Reibung.
- Gelenke verbinden Körper, haben Federn, Grenzen, Motoren und können brechen.
- Fester Zeitschritt, Akkumulator und Interpolation sorgen für glatte, wiederholbare Bewegung;
  Sub-Steps machen Gelenkketten steif.
- Ein Servo ist eine Gelenkfeder mit begrenztem Ziel plus ein Reibungsmotor – mit Werten aus einem Profil.

## Knobelecke

1. Warum stellt Kern nicht einfach Box3Ds Längeneinheit um, statt die Ente zu skalieren?
2. Ein Ball hüpft nie, obwohl seine Restitution 0,9 ist. Er fällt nur langsam. Woran liegt's?
3. Socke (`cloth`) liegt auf einem Boden ohne Oberfläche (`none`). Welche Reibung gilt?
4. Du setzt in jedem Schritt die Geschwindigkeit des Ente-Rumpfs, damit er aufrecht bleibt. Was ist
   daran falsch?
5. Wie viele Physikschritte Verzögerung hat ein Servo mit 30 ms bei 100 Hz und S = 4?

### Lösungen

1. Die Toleranzen gelten für die ganze Welt, und im selben Level stehen Autos neben kleinen Robotern.
2. Stöße unter der Restitutionsschwelle von 1 m/s prallen nicht ab.
3. Keine Paar-Reibung, weil eine Seite `none` ist: √(a · b) aus den Reibwerten der Formen.
4. Gesetzte Geschwindigkeiten an Körpern mit Gelenken übergehen den Solver und pumpen Energie hinein
   (gemessen: der Rumpf sprang). Und ein Roboter, der Hilfe bekommt, lernt nichts für die echte Welt.
5. 30 ms × √4 = 60 ms Weltzeit, bei 100 Hz also 6 Schritte.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
