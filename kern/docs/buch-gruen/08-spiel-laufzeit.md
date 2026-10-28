# 8 Die Spiel-Laufzeit – wie aus einem Level ein Spiel wird

> **Kurz gesagt:** Ein Level ist ein Bauplan aus Actors, und jeder Actor ist mit Komponenten
> bestückt wie ein Steckbaustein. Die **GameWorld** erweckt diesen Bauplan zum Leben: Sie baut daraus
> eine Physikwelt, lässt sie in festen Schritten laufen, steuert den Spieler, prüft die Spielregeln
> und schickt Bild und Ton hinaus. Studio und exportierte Spiele benutzen genau denselben Code.

> **Begriffe vorab:**
> - **Box3D** – das Physik-Rechenwerk in Kern, das Fallen, Stoßen und Rollen ausrechnet
>   ([Kapitel 6](06-physik.md)).
> - **Rigid Body und Collider** – der Körper eines Dings (Masse, Tempo) und seine unsichtbare Form,
>   mit der die Physik rechnet.
> - **statisch, kinematisch, dynamisch** – rührt sich nie wie eine Wand / bewegt sich nach Plan wie
>   ein Aufzug, lässt sich aber nicht schubsen / fällt, rollt und prallt frei ab.
> - **Sensor** – eine Form, die nichts wegstößt, aber merkt, wenn etwas hineinkommt, wie eine
>   Lichtschranke.
> - **HUD** – die Anzeige über dem Spielbild: Punkte, Zeit, Tempo, wie das Armaturenbrett im Auto.
> - **Server** – im Netzwerkspiel der eine Rechner, der das Sagen hat; die anderen zeigen Kopien.
> - **Lag** – die Verzögerung im Netz: Was du siehst, ist schon ein paar Hundertstel Sekunden alt.
> - **Netz (neuronales Netz)** – ein Programm, das durch Üben statt durch Programmieren entstanden
>   ist; so eins steuert die Micro Duck ([Kapitel 13](13-micro-duck.md)).

Bisher hast du die Werkzeuge kennengelernt: Grafik, Physik, Ton. Jetzt kommt der Dirigent, der alle
zusammen spielen lässt. Dieser Teil macht aus Kern eine echte Game Engine.

## 8.1 Das Leben einer Spielwelt

Eine Spielwelt hat einen einfachen Lebenslauf: Sie wird angezeigt, gestartet, läuft Schritt für
Schritt und wird wieder gestoppt. Gespielt wird dabei immer auf einer **Kopie** des Levels, darum ist
nach dem Stoppen alles wieder wie vorher.

> **Stell dir vor …** eine Probe im Theater. Das Bühnenbild steht. Bei der Probe wird umgerückt,
> umgeworfen, verschoben – und danach stellen die Bühnenarbeiter alles wieder an seinen Platz.

```mermaid
flowchart LR
  A[Bearbeiten] --> B[Start: Kopie anlegen, Physik bauen]
  B --> C[Schritt, Schritt, Schritt …]
  C --> D[Stop: Kopie wegwerfen]
  D --> A
```

- **Bearbeiten:** Die GameWorld zeigt das Level im Editor an. Ändert sich etwas, lädt sie nur das
  Geänderte neu.
- **Start:** Sie macht eine Kopie des Levels, baut die Physikwelt und startet Simulate oder Play.
- **Jedes Bild:** Die Welt geht einen Schritt weiter, die Kamera folgt, und alles wird gezeichnet.
- **Stop:** Alles wird abgerissen, und du siehst wieder das Level, wie es vorher war.

> **Achtung, Falle!** Was während einer Simulation passiert – umgefallene Kisten, verschobene
> Kugeln –, landet **nicht** im Level. Nach Stop steht alles wieder an seinem Platz. Willst du eine
> simulierte Pose behalten, nimm im Studio **Keep Simulation Changes** (Taste K).

## 8.2 Die Spielregeln der Zahlen

Damit alle Teile von Kern dieselbe Sprache sprechen, gelten feste Abmachungen über Maße, Richtungen
und darüber, wer an wem hängt.

- Gemessen wird in **Metern, Sekunden und Kilogramm**.
- **Z zeigt nach oben** (nicht Y, wie in manchen anderen Programmen).
- Drehungen stehen als **Winkel in Grad** da: Rollen, Nicken, Gieren – genau wie im Editor.
- Farben sind Werte von 0 (aus) bis 1 (voll) für Rot, Grün und Blau.
- Die Lage eines Actors ist immer **relativ zu seinem Eltern-Actor**. Hängst du ein Rad an ein Auto,
  fährt das Rad mit.
- Kind-Actors ohne eigenen Körper werden beim Simulieren in den Körper ihres nächsten Vorfahren
  „eingeschweißt“. Die Kabine eines Autos wird so ein Teil des Autos.

> **Für Neugierige:** Die drei Drehungen werden in einer festen Reihenfolge ausgeführt (erst Gieren,
> dann Nicken, dann Rollen), denn zwei Drehungen nacheinander ergeben je nach Reihenfolge eine andere
> Lage – probier es mit einem Buch aus. Wie Farben in der Datei stehen, zeigt das blaue Buch,
> Kapitel 8.

## 8.3 Level und Actor

Ein Level ist eine Liste von Dingen, den Actors, plus ein paar Einstellungen für die ganze Welt. Jeder
Actor hat Namen, Ort und Schlagworte und bekommt seine Fähigkeiten durch aufgesteckte Bausteine.

Ein **Level** ist das Dokument, das Kern Studio bearbeitet und als Datei speichert. Es enthält:

- einen Namen und die **World Settings** (8.5),
- die Liste der **Actors**,
- importierte **Texturen** (Bilder für Oberflächen) und **Sounds**, direkt in der Datei gespeichert,
- einen **Level-Graphen** für Spielregeln ([Kapitel 9](09-logik.md)),
- die Position der Editor-Kamera.

Welchen Actor du in Play steuerst, steht in den World Settings. Ist dort keiner eingetragen, nimmt
Kern den ersten Actor, der sich steuern lässt.

Ein **Actor** ist ein Ding in der Welt: ein Würfel, eine Lampe, ein Auto, eine Ente. Jeder Actor hat
eine eindeutige Nummer, einen Namen, einen Eltern-Actor und eine **Transform** – Position, Drehung
und Größe. Im **Outliner**, der Liste aller Dinge im Studio, kann er in einem Ordner liegen. Er kann
„versteckt“ sein (unsichtbar) oder „gesperrt“ (im **Viewport**, dem 3D-Fenster, nicht anklickbar).
Dazu kommen freie **Tags** – Schlagworte, über die Spielregeln ihn wiederfinden. Und dann kommen die
Komponenten.

> **Stell dir vor …** einen Steckbaustein mit Noppen. Ein nackter Actor ist nur der Grundstein: Er
> hat einen Ort, sonst nichts. Steck ein **Mesh** drauf, und man sieht ihn. Steck einen **Rigid Body**
> drauf, und er fällt runter. Steck einen **Player Controller** drauf, und du kannst ihn fahren. So
> baust du aus wenigen Teilen alles Mögliche.

Von den meisten Komponenten darf ein Actor höchstens eine haben (ein Mesh, ein Licht, einen Body …).
**Collider** und **Gelenke** dürfen es beliebig viele sein. Dazu kann jeder Actor eigene Spielregeln
als Logik-Graphen tragen.

## 8.4 Die Komponenten – der Steckbaukasten

Jede Komponente gibt einem Actor **genau eine Fähigkeit**. Was ein Ding kann, ergibt sich allein
daraus, welche Bausteine draufstecken. Eine Münze ist nichts Besonderes: Sie ist ein Actor mit
„sichtbar“, „dreht sich“ und „kann eingesammelt werden“.

| Baustein | gibt dem Ding … | Beispiel |
|---|---|---|
| Static Mesh | eine sichtbare Form und Oberfläche | eine rote Kiste |
| Point Light | ein Licht mit Farbe und Reichweite | eine Straßenlaterne |
| Rigid Body | einen Körper für die Physik | die Kiste fällt und kippt |
| Collider | eine unsichtbare Form zum Anstoßen | die Kiste stößt an die Wand |
| Constraint | ein Gelenk zu einem anderen Ding | eine Tür im Scharnier |
| Player Controller | eine Steuerung für den Spieler | das Schwebeauto |
| Weapon | eine Waffe | das Gewehr des Soldaten |
| Health | Lebenspunkte | eine Zielpuppe, die umfällt |
| Pickup | „kann eingesammelt werden“ | eine Münze |
| Trigger | eine Zone, die etwas auslöst | die Ziellinie |
| Audio Source | einen Ton | ein Radio ([Kapitel 7](07-audio.md)) |
| Rotator, Mover | eine Dauerbewegung | eine drehende Münze, ein Lift |
| City | eine ganze Stadt | die Stadt in Orb Run |

Ein paar Bausteine verdienen einen zweiten Blick, weil sie ein Prinzip zeigen:

**Die Physik steckt im Aussehen.** Reibung und Sprungkraft eines Dings stehen beim Mesh (unter
Mesh › Physics), nicht beim Körper. Denn ob etwas glatt ist wie Eis oder griffig wie Gummi, gehört
zum Material – genau wie die Farbe. Schaltest du beim Mesh Kollision ein, bekommt es automatisch
einen passenden Collider.

**Körper und Form sind getrennt.** Der Rigid Body sagt, *wie* sich ein Ding bewegt: statisch,
kinematisch oder dynamisch. Der Collider sagt, *welche Form* dabei zählt – eine Box, eine Kugel, eine
Kapsel oder eine Form, die sich ans Mesh anschmiegt. Gibst du keine Masse an, rechnet Kern sie aus
Größe und **Dichte** der Collider aus. Ab Werk ist die Dichte die von Wasser (1000 kg je
Kubikmeter): Eine Kiste von 1 m × 1 m × 1 m wiegt also eine Tonne. Ein Collider kann außerdem
**Sensor** sein – perfekt für Ziellinien und Münzen – und du kannst festlegen, wer mit wem
zusammenstoßen darf, etwa damit die Teile eines Roboters sich nicht gegenseitig im Weg sind.

**Gelenke verbinden Dinge.** Es gibt neun Arten, vom Scharnier über die Schiene und das Seil bis zum
Rad. Gelenke können Grenzen, Motoren und Federn haben und bei zu viel Kraft **brechen** – so
entsteht eine Seilbrücke, die unter Last reißt. Ein Scharnier kann außerdem ein **Servo** sein: ein
kleiner Motor, der einen Zielwinkel ansteuert. Je weiter das Gelenk vom Ziel weg ist, desto stärker
drückt er zurück, wie ein Gummiband ([Kapitel 6](06-physik.md)).

**Spielfiguren sind auch nur Bausteine.** Der Player Controller hat sieben Bewegungsarten: Schwebeauto
(*Hover Car*), Fahrzeug, rollende Kugel, Figur (*Character*), Flieger, Soldat und Walker (die Micro
Duck). Dazu stellst du Tempo, Sprung, Boost und die Kamera-Art ein. Eine Waffe trifft sofort über
einen unsichtbaren Strahl; Lebenspunkte können sich nach einer Weile erholen, und wer umfällt, kann
wieder auferstehen. Wie Kern diese Regeln umsetzt, zeigen 8.10 und 8.13.

**Bewegung ohne Physik.** Der Rotator dreht ein Ding dauernd, der Mover schiebt es hin und her – sanft
wie ein Pendel oder gleichmäßig. Und die City baut aus einer einzigen Zahl (einem *Seed*) eine
ganze Stadt mit Straßen, Bäumen, Verkehr und Fenstern, die nachts leuchten – jedes Mal dieselbe.

Alle Felder, Standardwerte und die Werte der einzelnen Waffen stehen im blauen Buch, Kapitel 8.

> **Probier's aus!** Öffne den Content Drawer (Ctrl+Space) und klick bei **Place Actors** auf
> **Cube**. Heb den Würfel mit den Pfeilen (dem *Gizmo*) ein Stück an und starte Simulate (Alt+S):
> Er bleibt in der Luft hängen. Stopp, füg ihm im Panel **Details** über **+ Add** einen **Rigid
> Body** hinzu und starte erneut. Jetzt fällt er! Ein Baustein mehr – und schon gilt die Schwerkraft.

## 8.5 World Settings – die Einstellungen fürs ganze Level

Manches gilt nicht für ein einzelnes Ding, sondern für die ganze Welt: die Schwerkraft, die
Tageszeit, die Spielregeln, die Musik. Das steht in den **World Settings**, sortiert in vier Gruppen.

- **Physik:** Schwerkraft, wie oft pro Sekunde gerechnet wird, wie fein jeder Schritt noch einmal
  unterteilt wird (*Unterschritte*), Bodenreibung und wie weich Berührungen nachgeben.
- **Himmel und Licht:** Tageszeit (0,25 Sonnenaufgang, 0,5 Mittag, 0,75 Untergang), wie schnell die
  Zeit vergeht (0 = eingefroren), Sonnenrichtung, Nebel, Helligkeit, Schatten, Wolken und ein endloser
  Boden.
- **Spiel:** Titel, Spieler, Siegbedingung, Zeitlimit, Zielpunktzahl, **Kill Z** (wer tiefer als
  −50 m fällt, wird zurückgesetzt) und die **Observer-Kamera** – eine freie Zuschauerkamera, mit der
  Play beginnen kann.
- **Ton:** Lautstärken, Musik, Hintergrund, Hall, Doppler ([Kapitel 7](07-audio.md)).

## 8.6 Die Leveldatei

Ein Level wird als lesbarer Text gespeichert. Was dem Standard entspricht, wird gar nicht erst
hingeschrieben – so bleibt die Datei schlank.

Die Datei ist im Format **JSON** geschrieben – reiner Text mit Klammern, Namen und Werten, den du
sogar mit einem Texteditor lesen kannst. Ein Actor sieht etwa so aus (gekürzt):

```json
{
  "id": 12, "name": "Door", "parent": 3,
  "transform": {"position": [5.2, -12, 1.5],
                "rotation": [0, 0, 0], "scale": [1, 1, 1]},
  "tags": ["door"],
  "components": { "mesh": {...}, "body": {...} }
}
```

Zeile für Zeile: Eine Tür mit Nummer 12, die an Actor 3 hängt. Sie steht an einer bestimmten
Position, ist nicht gedreht und nicht vergrößert, trägt das Schlagwort „door“ und hat zwei
Bausteine: ein Mesh und einen Body.

### Nachsichtig und streng zugleich

Beim Lesen folgt Kern einem klugen Motto: **nachsichtig bei Fehlendem, streng bei Falschem.**

- Unbekannte Einträge werden ignoriert, fehlende bekommen Standardwerte. So laden alte Dateien auch
  in neuen Versionen.
- Falsche Typen sind Fehler mit klarer Meldung, etwa „friction must be a number“ (Reibung muss eine
  Zahl sein). Die Meldung nennt den betroffenen Actor.
- Zahlen außerhalb ihres Bereichs werden hineingeschoben, unmögliche Werte repariert.
- Fehlende oder doppelte Nummern werden neu vergeben, und hängen Actors im Kreis aneinander, wird der
  Kreis aufgebrochen.
- Eine Datei aus einer **neueren** Kern-Version wird abgelehnt.

### Die Prüfung vor dem Start

Vor Play prüft Kern das Level. **Fehler** verhindern den Start – zum Beispiel ein Gelenk, das ein
Ding mit sich selbst verbindet, oder eine Spielfigur ohne Körper. **Warnungen** landen nur im Output
Log, dem Protokoll unten im Studio – zum Beispiel ein Sound, den es nicht gibt (er bleibt dann
still), oder zwei Spielfiguren in einem Level. Die vollständige Liste steht im blauen Buch,
Kapitel 8.

## 8.7 Vom Bauplan zur Physik

Bevor die Physik loslegen kann, macht Kern aus dem Level einen **Plan**: welche Körper es gibt und
welche Formen zu welchem Körper gehören. Studio zeichnet die Collider im Editor aus genau diesem
Plan. Was du siehst, ist also genau das, was simuliert wird.

Die Regeln in Kurzform:

1. Jeder Actor mit **Rigid Body** wird ein Körper.
2. Ein Ding, das anstoßen oder etwas bemerken soll, aber keinen Body hat, bekommt einen
   **unsichtbaren eigenen Körper**: statisch, oder kinematisch, wenn es sich bewegt.
3. Kinder ohne eigenen Body geben ihre Formen an den Körper ihres Eltern-Actors ab – sie werden
   eingeschweißt (8.2).
4. Eine ganze Stadt wird ein einziger fester Körper mit einer Box je Gebäude.
5. Der **endlose Boden** ist in Wahrheit eine riesige Platte.

Ein Beispiel: Eine leuchtende **Energie-Kugel** in Orb Run ist ein Pickup, dreht sich und bewegt
sich, hat aber keinen Rigid Body. Nach Regel 2 bekommt sie einen unsichtbaren Körper. Weil sie sich
bewegt, ist er kinematisch: Er folgt ihrer Bewegung, lässt sich aber nicht wegschubsen. Und weil sie
ein Pickup ist, ist ihre Form ein Sensor – du fährst durch sie hindurch, und Kern merkt es.

Fehlt einem Ding eine eigene Form, passt Kern eine an das Mesh an: eine Kiste wird eine Box, eine
Kugel eine Kugel, und alles Krumme wird in eine Art straff gespannte Haut gehüllt. Wie diese Formen
genau gebaut werden, erklärt [Kapitel 6](06-physik.md); alle Regeln stehen im blauen Buch, Kapitel 8.

> **Wusstest du?** Der „endlose“ Boden ist gar nicht endlos – er ist nur 20 Kilometer breit. Das
> reicht für jedes Spiel, ohne dass es jemand merkt.

## 8.8 Die Welt starten

Beim Start baut Kern alles in einer festen Reihenfolge auf – vom Prüfen bis zu den Spielregeln. Die
Reihenfolge ist wichtig: Ein Gelenk kann erst entstehen, wenn beide Körper da sind, und die
Spielregeln erst, wenn es etwas zu regeln gibt.

1. Level **prüfen** – ein Fehler bricht ab.
2. Eine **Kopie** des Levels anlegen, das Original merken.
3. Die **Physikwelt** mit Schwerkraft bauen.
4. Actors so ordnen, dass Eltern vor Kindern kommen.
5. **Körper**, dann **Gelenke**, dann die laufenden Roboter (**Walker**) anlegen.
6. Den **Stadtverkehr** starten.
7. Den **Spieler** übernehmen (nur in Play).
8. **Regeln** zurücksetzen: Punkte, Zeit, Status (die Bestzeit bleibt). In Play erscheint der
   Titel als Meldung.
9. Gesundheit und Munition füllen, dann die **Spielregeln aus Kapitel 9** starten.

Zwei Feinheiten merkt man beim Spielen: **Spieler schlafen nie** – ein ruhender Körper darf sonst
„einschlafen“, um Rechenzeit zu sparen, aber deine Figur soll sofort reagieren. Und Fahrzeuge,
Figuren und Flieger **kippen nicht um**: Kern hält sie aufrecht; nur die rollende Kugel und der
Walker dürfen sich frei drehen. Kann eine Form nicht gebaut werden, wird sie übersprungen und im Log
gemeldet.

> **Für Neugierige:** Schnelle Spielerfahrzeuge werden besonders sorgfältig auf Zusammenstöße
> geprüft, damit sie nicht durch dünne Wände „tunneln“ (siehe [Kapitel 6](06-physik.md)). Wie Gelenke
> ausgerichtet werden und warum Federn nicht beliebig hart sein dürfen, steht im blauen Buch,
> Kapitel 8.

### Vier Modi

Eine Spielwelt kann in vier Zuständen sein:

| Modus | Was passiert |
|---|---|
| keiner | Editor, keine Simulation |
| Simulate | Physik und Logik laufen, niemand wird gesteuert; Körper lassen sich greifen |
| Play | wie Simulate, dazu Spieler, Regeln und HUD |
| Replica | Kopie eines Server-Spiels im Netz ([Kapitel 10](10-netzwerk.md)) |

Mit **F8** wechselst du mitten im Lauf zwischen Simulate und Play („Eject“ – aussteigen, und
„Possess“ – wieder übernehmen), ohne dass der Spieler seinen Zustand verliert. Pausieren geht auch,
und im Pausenzustand macht die Taste **.** (Punkt) genau einen Schritt. Ein Neustart lädt das
ursprüngliche Level frisch.

> **Probier's aus!** Öffne das **Physics Lab** (File › Example Levels), starte Play (Alt+P), drück
> nach ein paar Sekunden **F8** und schau dir die Szene von außen an. Pausiere und tippe immer wieder
> auf **.** – du gehst Schritt für Schritt durch die Physik wie durch ein Daumenkino.

## 8.9 Ein Frame – das Herz der Spielschleife

Ein Spiel rechnet immer wieder dasselbe: Eingaben lesen, Welt weiterrechnen, Bild zeichnen. Ein
Durchgang heißt **Frame** – hier im Sinn von „ein Bild“.

> **Stell dir vor …** ein Daumenkino. Jedes Blatt zeigt die Welt einen winzigen Moment später.
> Blätterst du schnell, sieht es aus wie Bewegung. Ein Spiel ist ein Daumenkino, das sich selbst
> zeichnet: Für jedes Blatt rechnet es aus, wo alles jetzt sein muss.

### Feste Schritte, egal wie schnell der Bildschirm ist

Bildschirme zeigen unterschiedlich viele Bilder pro Sekunde: 60, 120, manchmal ruckelt es. Würde
die Physik einfach „so viel Zeit wie seit dem letzten Bild“ rechnen, käme auf jedem Gerät etwas
anderes heraus. Deshalb rechnet Kern immer in **festen Schritten**, normalerweise 1/60 Sekunde.

Das funktioniert wie ein Sparschwein: Jedes Bild wirft die vergangene Zeit hinein. Solange genug für
einen ganzen Schritt drin ist, nimmt Kern einen Schritt heraus und rechnet ihn. Was übrig bleibt,
wartet aufs nächste Bild. Damit ein langer Hänger nicht alles durcheinanderbringt, ist das
Sparschwein begrenzt: Nach einer Pause von mehreren Sekunden holt Kern nicht alles auf einmal nach.
Mehr dazu in [Kapitel 6](06-physik.md).

Und weil ein Bild meist zwischen zwei Schritten liegt, schiebt Kern die Körper für die Anzeige
geschmeidig zwischen der alten und der neuen Lage hin und her. So sieht alles flüssig aus, obwohl
die Physik in festen Häppchen rechnet.

### Was in einem festen Schritt passiert

Jeder Schritt läuft in **derselben Reihenfolge** ab – damit er auf jedem Gerät gleich ausgeht. Würde
einmal zuerst die Münze eingesammelt und einmal zuerst das Zeitlimit geprüft, könnte dasselbe Spiel
einmal gewonnen und einmal verloren sein.

```mermaid
flowchart TD
  A[Eingaben der Spieler anwenden] --> B[Dauerbewegungen: drehen, schieben]
  B --> C[Verkehr, Roboter, Motoren]
  C --> D[Physik-Schritt]
  D --> E[Ereignisse: Münzen, Zonen, Treffer, Töne]
  E --> F[Gesundheit und Spielregeln aus Kapitel 9]
  F --> G[Regeln: Zeit, Absturz, Punkte]
```

1. **Spieler:** Die Eingabe jedes Spielers wird angewendet – je nach Art fährt, läuft oder fliegt er.
2. **Dauerbewegung:** Rotatoren und Mover bekommen ihre neue Lage.
3. **Verkehr** (Autos halten vor dem Spieler), laufende Roboter und ihre Motoren.
4. **Physik:** Box3D rechnet einen Schritt.
5. **Ereignisse:** eingesammelte Münzen, berührte Zonen, Treffer, Kamerawackeln, Aufprall-Töne,
   brechende Gelenke.
6. **Gesundheit** (Heilen, Wiederauferstehen) und die **Spielregeln** aus [Kapitel 9](09-logik.md).
7. **Regeln:** Zeitlimit prüfen, wer unter Kill Z fiel, zurücksetzen, Tempo fürs HUD, Punktziel
   erreicht?

Danach läuft die Tageszeit weiter, die Kamera folgt, und die Welt übergibt alles zum Zeichnen.
Versteckte Dinge, eingesammelte Münzen und benutzte Zonen bleiben dabei unsichtbar. Zum Schluss sagt
die Welt dem Renderer, wie „nachtig“ es ist – davon hängt ab, wie stark die Stadtfenster leuchten.

## 8.10 Controller – fahren, rollen, laufen, fliegen

Alle Spielfiguren bekommen dieselbe Art von Eingabe: vor, zurück, links, rechts, springen, Boost.
Was daraus wird, entscheidet der Controller – dieselbe Taste lässt ein Auto Gas geben und eine Kugel
rollen.

> **Stell dir vor …** du gibst einem Reiter, einem Radfahrer und einem Schwimmer dasselbe Kommando:
> „Vorwärts!“ Jeder setzt es auf seine eigene Weise um.

Bei jedem Bild bekommt der Spieler-Actor eine **Eingabe**: Bewegung, hoch und runter, Springen,
Boost, Kameradrehung und Zoom. Der Soldat bekommt dazu Schießen, Zielen, Nachladen und Ducken, die
Micro Duck ihre Spezialbewegungen wie Sitzen, Kicken oder Zwitschern.

- **Hover Car:** Gas schiebt das Auto nach vorn, eine unsichtbare **Schwebefeder** hält es in der
  Luft über dem Boden. Damit es nicht wie auf Eis rutscht, drückt eine Seitenkraft dagegen. Gelenkt
  wird umso stärker, je schneller es fährt. Das Wippen und Neigen, das du siehst, ist nur Darstellung.
- **Vehicle:** wie das Hover Car, aber ohne Schweben – angetrieben wird nur mit Bodenkontakt.
- **Rolling Ball:** Kraft in Laufrichtung, Sprung als Stoß nach oben, nur am Boden.
- **Character:** eine Lauffigur wie in klassischen Ego-Shootern. Sie nutzt eine fertige Laufhilfe der
  Physik, steigt kleine Stufen von allein hinauf und springt knackig. Ihr Körper ist dabei
  kinematisch: Er geht dorthin, wo die Laufhilfe ihn hinschickt.
- **Flyer:** fliegt in alle Richtungen, die Schwerkraft ist aufgehoben; er dreht sich zur Kamera.
- **Soldier:** eine Ego-Shooter-Figur (8.13).
- **Walker:** Die Eingabe geht an das Netz der Micro Duck, das ihre Servos steuert (8.14).

Bei Kugel, Figur und Flieger ist „vorwärts“ immer **dort, wo die Kamera hinschaut**. Gemeinsam haben
alle: Der Boost ist ein Vorrat, der sich beim Boosten leert und danach langsam auffüllt. Wer hart
landet, hört ein Landegeräusch.

### Pawns – die Spielfiguren

> **Stell dir vor …** ein Brettspiel. Das Brett ist das Level, und jeder Mitspieler hat seine
> **Spielfigur**. In Kern heißt die Spielfigur **Pawn** (englisch für Bauer im Schach).

Ein Pawn merkt sich alles, was zu einem Spieler gehört: welchen Actor er steuert, seine letzte
Eingabe, Blickrichtung, Startpunkt und Boost. Kommt im Netzwerkspiel ein weiterer Spieler dazu,
kopiert Kern die Spielfigur samt allem, was an ihr hängt, und stellt die Kopie auf einen Ring um den
ursprünglichen Start – jede neue Figur ein Stück weiter, damit niemand auf jemandem landet. Geht ein
Spieler, verschwindet seine Kopie wieder.

## 8.11 Die Kamera

Die Kamera folgt der geglätteten Lage des Spielers – von hinten, aus seinen Augen oder von oben. Sie
achtet darauf, nicht in Wänden zu stecken, und lässt hohes Tempo schneller wirken.

- **Chase** („Verfolgung“): Bei Fahrzeugen hinter der Fahrtrichtung, bei hohem Tempo etwas weiter
  weg, weich gedämpft. Sonst kreist sie mit Maus oder Stick um den Spieler. Ein unsichtbarer Strahl
  prüft, ob eine Wand im Weg ist, und holt die Kamera dann näher heran.
- **First Person:** die Augen des Spielers; Fahrzeuge blicken in Fahrtrichtung.
- **Top Down:** von oben.

Bei hohem Tempo weitet sich das Bild ein wenig – so fühlt sich Geschwindigkeit schneller an. Harte
Treffer lassen die Kamera wackeln. Soldaten haben eine eigene Kamera.

## 8.12 Regeln, HUD und Trigger

Ein Spiel braucht ein Ziel und eine Anzeige. Die **Siegbedingung** legt fest, wann man gewinnt, das
HUD zeigt den Stand, und Trigger-Zonen lösen Dinge aus: Ziel, Gefahr, Sprungschanze, Teleporter.

Das **HUD** zeigt Status (spielt, gewonnen, verloren), Titel, Meldungen, Punkte, gesammelte Münzen,
Zeit, Tempo, Boost und Bestzeit – beim Soldaten auch Gesundheit und Munition.

Die **Siegbedingung** stellst du in den World Settings unter *Win condition* ein:

| Auswahl im Studio | Gewonnen, wenn … |
|---|---|
| None (sandbox) | nie – ein Sandkasten zum Herumspielen |
| Collect all pickups | alles eingesammelt ist (und danach das Ziel erreicht, falls es eins gibt) |
| Reach a goal trigger | eine Ziel-Zone erreicht ist |
| Survive the time limit | das Zeitlimit überstanden ist |
| Reach a target score | die Zielpunktzahl erreicht ist |

Mit Zeitlimit verliert man bei null – außer beim Überleben. Eine Gefahren-Zone verliert immer. Am Ende
spielt Kern eine kleine Melodie, merkt sich die Bestzeit und sagt den Spielregeln „Spiel vorbei“.

**Trigger** können viel: Ziel (gewinnt), Gefahr (verliert), Checkpoint (setzt den Startpunkt neu),
Meldung (zeigt einen Text), Sprungschanze (schleudert den Spieler hoch und nach vorn), Teleporter
(beamt ihn zu einem anderen Ding, mit kurzer Sperre gegen sofortiges Zurückbeamen), Schaden (zieht
Lebenspunkte ab) oder nur ein Signal an die Spielregeln. Jeder Trigger hat einen passenden Ton. Ein
einmaliger Trigger ist danach verbraucht – außer Ziel und Gefahr.

**Pickups** werden beim Berühren eingesammelt: Punkte, Ton, Funken, Heilung oder Munition. Ein reines
Heilpaket bleibt übrigens liegen, solange du volle Gesundheit hast – clever, oder? Mit einer
Wartezeit kommt ein Pickup später zurück.

## 8.13 Soldaten

Beim Soldaten machen viele kleine, unsichtbare Regeln das Spielgefühl aus: Man darf knapp nach der
Kante noch springen, Bewegung macht ungenauer, und im Netz wird geprüft, wo der Gegner *war*, als du
geschossen hast. Alles, was man nur sieht, ändert die Simulation nicht.

Der **Soldat** stammt aus SpawnCity, dem Spiel, aus dem Kern entstanden ist. Alles wird in den festen
Schritten berechnet. Einige Feinheiten:

- **Ducken** macht die Figur kleiner; aufstehen geht nur, wenn über dem Kopf Platz ist.
  **Sprinten** geht nur vorwärts, nicht beim Zielen oder Ducken.
- **Sprungpuffer und Coyote-Zeit:** Drückst du Springen kurz *vor* der Landung, zählt es trotzdem.
  Und kurz *nachdem* du über eine Kante gelaufen bist, darfst du noch springen – wie der Kojote im
  Zeichentrick, der kurz in der Luft steht.
- **Fallschaden** gibt es erst bei hartem Aufprall, ein sehr tiefer Sturz ist tödlich.
- **Streuung:** Schüsse werden ungenauer, wenn du dich bewegst, springst oder schnell hintereinander
  schießt, und genauer, wenn du dich duckst. Die Streuung wirkt zufällig, ist aber **deterministisch**
  berechnet – auf jedem Rechner fliegt die Kugel gleich.
- **Schüsse** sind unsichtbare Strahlen aus dem Auge. Getroffene Körper bekommen einen Stoß.
- **Lag-Kompensation:** Wegen des Lags siehst du deinen Gegner dort, wo er vor einem Moment war. Der
  Server dreht darum beim Prüfen die Zeit ein Stück zurück: Er schaut nach, wo die anderen standen,
  *als du geschossen hast*. So zählt der Treffer, den du gesehen hast.
- **Schaden:** Rüstung fängt einen Teil ab; frisch wiederbelebte Spieler sind kurz geschützt. Wer
  umfällt, kehrt nach einer Wartezeit am Start zurück. Ein explodierendes Ding stößt alles weg,
  verletzt mit dem Abstand schwächer, blitzt, knallt und schüttelt die Kamera.

Die Figuren, Waffen in der Hand, Mündungsfeuer, Leuchtspuren und Hülsen sind reine Darstellung – sie
ändern die Simulation nicht. Deshalb zeigen Server, Mitspieler und Editor dasselbe. Alle Zahlen
stehen im blauen Buch, Kapitel 8.

## 8.14 Walker

Ein **Walker** ist ein Roboter mit Gelenken, der von einem gelernten Programm über seine eigenen
Motoren gesteuert wird – mit einer kleinen Verzögerung, genau wie in echt.

Gemeint ist die Micro Duck mit dem Netz von Pollen Robotics. Die GameWorld treibt jeden Walker in
jedem Schritt: Sensoren lesen, ein Verhalten wählen,
regelmäßig das Netz fragen, welche Winkel die Gelenke jetzt haben sollen, und die Servos dann mit
kurzer Verzögerung bewegen – so, wie der echte Roboter es auch erlebt. Das ist ein Thema für sich,
nachzulesen in [Kapitel 13](13-micro-duck.md).

## 8.15 Der Ton

Die Simulation spielt selbst keinen Ton ab. Sie legt **Sound-Cues** ab – kleine Zettel wie „Sprung
hier“ – für Sprünge, Landungen, Schritte, Münzen, Zonen, Sieg und Niederlage, Treffer, Schüsse,
Explosionen, brechende Gelenke und Aufpralle. **GameAudio** holt sie bei jedem Bild ab und spielt sie,
zusammen mit Musik, Hintergrund und dem Motorsound des Spielers. Wie das genau klingt, steht in
[Kapitel 7](07-audio.md).

## 8.16 Touch, Maus und Gamepad

Kern erkennt von selbst, ob du mit dem Finger, mit Maus und Tastatur oder mit einem Gamepad spielst.
Auf dem Telefon bekommst du zwei virtuelle Steuerknüppel und passende Knöpfe.

Ein **Schiedsrichter** entscheidet, welches Gerät du gerade benutzt. Das zuletzt benutzte gewinnt,
mit ein paar klugen Regeln: Ein Finger auf dem Bildschirm schaltet auf Touch, jede echte Taste auf
Tastatur – auch am Handy. Eine angesteckte Maus macht ein Telefon aber nicht zum Computer.

Die **Touch-Steuerung** legt zwei virtuelle Knüppel (*Sticks*) in die unteren Ecken – links zum
Bewegen, rechts zum Schauen. Dazu kommen Knöpfe passend zur Figur: Der Soldat bekommt etwa Springen,
Nachladen und Zielen, die Micro Duck Quaken, Sitzen und Kicken, die Kugel Springen und Boost.

Jeder Finger gehört genau einem Ding: Knöpfe gehen vor Sticks, Sticks vor dem freien Bildschirm. Ein
Finger auf dem freien Bildschirm schießt beim Soldaten, sonst schaut er. Ein paar Kniffe: Den linken
Stick ganz nach vorn drücken heißt Sprinten. Schnelle Wischer auf dem rechten Stick drehen stärker.
Und ziehst du den Stick über den Rand hinaus, dreht die Kamera einfach weiter. Gamepads funktionieren
ebenfalls; Kern glättet ihre Sticks, damit ein leicht zitternder Daumen die Figur nicht wackeln lässt.

> **Für Neugierige:** Mit Maus und Tastatur kann Kern den Mauszeiger „einfangen“, sodass er nicht
> aus dem Fenster rutscht. Größen der Sticks und die vollständige Knopftabelle stehen im blauen Buch,
> Kapitel 8.

## 8.17 Logik in der Welt

Jedes Ding und das Level selbst können eigene Spielregeln tragen. Sie hören auf Ereignisse in der
Welt und können sie verändern.

Diese Spielregeln heißen **Logik-Graphen** ([Kapitel 9](09-logik.md)). Die GameWorld startet sie in
Play und Simulate – aber nur auf dem Rechner, der das Sagen hat (allein oder als Server). Sie schickt
ihnen Ereignisse: etwas betritt eine Zone, etwas prallt auf, eine Münze wird eingesammelt, jemand
nimmt Schaden, eine Taste wird gedrückt, das Spiel ist vorbei, ein neuer Spieler kommt. Die Graphen
können umgekehrt die Welt verändern – Dinge verschieben, erzeugen, anschubsen, Motoren schalten,
Punkte vergeben, das Spiel beenden. Was sie ändern, wird an alle Mitspieler weitergegeben.

## 8.18 Die Beispiel-Levels

Sechs fertige Welten zeigen, was mit den Bausteinen geht. Du findest sie unter File › Example Levels
(und File › New Level).

- **Orb Run:** eine Stadt mit Verkehr. Du fährst ein Schwebeauto und sammelst 24 leuchtende
  Energie-Kugeln. Der Lauf beginnt am späten Nachmittag und endet in der Nacht. Studio startet mit
  diesem Level, dazu läuft treibende Musik und Stadtklang.
- **Physics Lab:** alle Gelenkarten auf einem Spielplatz – Kistenpyramide, Pendel, Tür mit Feder,
  eine Seilbrücke, deren mittlere Gelenke unter Last brechen, Drehteller, Lift und ein
  selbstfahrender Rover. Du bist eine rollende Kugel, sammelst fünf Sterne, weichst einem rotierenden
  Kehrer aus und musst ins Ziel.
- **Shooting Range:** Du bist ein Soldat mit Gewehr, in einem Schießstand mit Zielpuppen, Deckung und
  Turm – auch für mehrere Spieler.
- **Micro Duck:** ein Zimmer im Maßstab des Roboters mit Holzboden, Büchern, Ball und Socken.
  Die Ente läuft selbstständig, die Physik rechnet hier besonders fein, und ein Logik-Graph lässt
  sie zwitschern, wenn sie den Ball sieht.
- **Basic:** Boden und ein Würfel. **Empty:** gar nichts.

> **Probier's aus!** Starte im Studio Orb Run mit Play (Alt+P) und sammle ein paar Kugeln. Dann
> stoppe mit Esc, öffne die World Settings und stell die **Tageszeit** auf 0,75. Starte erneut – jetzt
> fährst du in den Sonnenuntergang.

## 8.19 Was die GameWorld sonst noch kann

Die GameWorld hat noch viele weitere Schalter – für Darstellung, Kampf, Netzwerk, Logik und das
Greifen von Körpern mit der Maus. Eine davon ist besonders hübsch zum Verstehen: die
**Debug-Ansicht**. Sie zeichnet die unsichtbaren Physikformen farbig nach Art – statisch grün,
kinematisch blau, dynamisch orange, Sensor violett, schlafend grau –, dazu Berührungspunkte und
Gelenkachsen. Die volle Liste steht im blauen Buch, Kapitel 8.

Natürlich ist all das durch automatische Tests abgesichert: Kern lädt und speichert Dateien,
plant Körper, baut Städte und spielt Orb Run und Physics Lab selbst durch, um Fehler früh zu finden.

## Das nimmst du mit

- Ein Level besteht aus Actors; Komponenten sind die Steckbausteine, die ihnen Fähigkeiten geben.
- Kern misst in Metern und Sekunden, Z zeigt nach oben, Drehungen in Grad.
- Die Leveldatei ist lesbarer Text: nachsichtig bei Fehlendem, streng bei Falschem.
- Aus dem Bauplan wird ein Physik-Plan – Editor und Simulation sehen dasselbe.
- Die Spielschleife rechnet in festen Schritten und glättet fürs Bild – wie ein Daumenkino.
- Simulate lässt die Welt laufen, Play gibt dir dazu einen Spieler, Regeln und HUD; F8 wechselt.
- Sieben Controller machen Fahren, Rollen, Laufen, Fliegen, Schießen und Watscheln möglich.
- Ton läuft über Cues, Spielregeln über Ereignisse – beide in den festen Schritten, überall gleich.

## Knobelecke

1. Du lässt in Simulate einen Turm einstürzen und drückst Stop. Wo liegen die Steine jetzt?
2. Warum rechnet Kern die Physik in festen Schritten statt mit der echten Zeit seit dem letzten Bild?
3. Ein Würfel ohne Rigid Body, aber mit Mover, soll eine Kugel anschubsen. Welchen Körper bekommt er?
4. Warum bleibt ein Heilpaket liegen, wenn du darüberläufst?
5. Was ist Coyote-Zeit, und warum macht sie ein Spiel angenehmer?

### Lösungen

1. Wieder an ihrem Platz. Die Simulation arbeitet mit einer Kopie; nur Keep Simulation Changes
   übernimmt Posen.
2. Damit auf jedem Gerät exakt dasselbe herauskommt – unabhängig davon, wie schnell der Bildschirm ist.
3. Einen eigenen unsichtbaren Körper, und weil er sich bewegt, einen kinematischen – wie ein Aufzug,
   der nach Plan fährt und dabei die Kugel wegschiebt.
4. Weil du volle Gesundheit hast – es wäre verschwendet.
5. Man darf kurz nach dem Verlassen einer Kante noch springen. Das verzeiht knappe Reaktionen und
   fühlt sich fairer an.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
