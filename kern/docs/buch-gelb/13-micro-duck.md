# 13 Die Micro Duck – ein echter Roboter, Servo für Servo

> **Dein Level:** Jetzt wird es echt. Die Micro Duck ist ein Laufroboter, den es wirklich gibt, und
> Kern lässt im Browser genau die Netze laufen, die auch den echten Roboter steuern. Du lernst ihre
> Motoren, Sensoren und Netze kennen – und alles, was die Ente kann.

Die Micro Duck stammt von der Firma **Pollen Robotics**: rund 25 cm groß, 0,74 kg schwer, mit 14
**Servos** vom Typ Dynamixel XL330 in Beinen und Hals und einem fünfzehnten für den Unterschnabel.
Ein Servo ist ein kleiner Motor, dem man einen Winkel nennt und der dann dorthin fährt.

Laufen hat Pollen der Ente nicht mit Regeln beigebracht, sondern mit **neuronalen Netzen**, trainiert
in der Physiksimulation MuJoCo (Repository `microduck_rl`, Framework mjlab, Lernverfahren rsl_rl).
Die fertigen Netze werden als **ONNX**-Dateien gespeichert, und genau die lädt der echte Roboter.

Kern baut den Roboter auf seiner Physik-Engine Box3D nach und rechnet dieselben Netze. Das Ziel
heißt **Treue**: dasselbe Modell, dieselben Servos, derselbe Takt. Dazu kommen drei eigene Dinge:
der **Lehrer** zum Trainieren ([Kapitel 14](14-lehrer.md), [Kapitel 15](15-eigene-modelle.md)),
der **Sprung** (`jump.onnx`) und die **Spielschicht** (`play.onnx`).

![Das Micro-Duck-Level im Studio (Ente in der eigenen Gestaltung dieses Buchs)](../buch/bilder/duck-level.png)

> **Mission:** Öffne Kern Studio auf https://kernengine.de/ und wähle **File › Example Levels ›
> Micro Duck**. Drück **Play** (Alt+P) und schau mit der Observer camera zu, wie die Ente den Raum
> erkundet. Mit **F8** übernimmst du sie, mit noch einmal F8 schaust du wieder zu.

## 13.1 Wo der Code liegt

Fast alles steht in `kern/src/game/`: `walker.cpp` (Aufbau, Regler, Sensoren, Steuerung),
`walker_policy.cpp` (Netze lesen und rechnen), `servo.cpp` (Servomodell), `walker_brain.cpp`
(selbstständiges Verhalten) und `walker_play.cpp` (Spielschicht). Pollens Netze und Baupläne liegen
in `third_party/microduck/`, Kerns eigene Netze in `policies/`.

## 13.2 Die Grundregel: nicht schummeln

Ein trainiertes Netz funktioniert nur, wenn die Simulation dieselbe Physik rechnet wie im Training:

- Keine Hilfskräfte, kein Festhalten, kein Teleport, kein Überblenden zwischen Netzen. Die Ente
  steht nur durch Servos und Fußkontakte.
- Ein Servo besteht nur aus Box3Ds eigenen Gelenkteilen (Abschnitt 13.4).
- Feste Zeitschritte: Gleiche Eingaben ergeben **Bit für Bit** denselben Gang, auf jedem Gerät
  (**Determinismus**, [Kapitel 14](14-lehrer.md)).

> **Vorsicht, Falle:** Eine kleine unsichtbare Stütze, „damit sie nicht umfällt“, klingt harmlos.
> Aber das Netz würde sich darauf verlassen – und am echten Roboter gibt es keine.

## 13.3 Das Modell

### Bauplan, Glieder, Gelenke

Pollen beschreibt die Ente in einer **MJCF**-Datei, dem Bauplan-Format von MuJoCo. Ein
Import-Werkzeug macht daraus Kerns Modelldaten und bricht ab, wenn etwas nicht zusammenpasst.

In Kern hat die Ente **16 Glieder**: Rumpf, 14 Glieder für die Netze und den Unterschnabel. Jedes
Bein hat fünf Gelenke: Hüfte gieren, rollen, nicken, dann Knie und Knöchel. Dazu kommen Hals, Kopf
nicken, gieren, rollen und als 15. Servogelenk der Mund. **Gieren, Nicken, Rollen** sind die drei
Drehrichtungen: „nein“ schütteln, „ja“ nicken, Kopf zur Schulter neigen.

Jedes Gelenk hat harte Grenzen und einen **Home-Winkel** für die Grundstellung. Winkel misst man in
**Radiant**: Ein Kreis hat 2π ≈ 6,28 rad, 1 rad sind etwa 57°.

> **Krass, aber wahr:** Das Kopf-Gieren reicht von −2,967 bis +2,967 rad, rund ±170°. Die Ente kann
> fast nach hinten schauen.

### Massen und Armatur

Masse, Schwerpunkt und **Trägheit** (wie schwer sich ein Teil in Drehung versetzen lässt) kommen aus
dem Bauplan: Rumpf 0,199 kg, Kopf 0,155 kg, zusammen 0,74 kg.

Im Servo dreht ein winziger Motor über ein Getriebe 288 : 1. Seine Drehmasse heißt **Armatur**.
Box3D kennt so etwas nicht, deshalb bekommt jedes Glied eine gedachte dünne Scheibe auf seiner
Gelenkachse. Nicht exakt, aber die Ente zappelt damit viel weniger (Abschnitt 13.14).

### Was die Welt berührt

Was zusammenstößt, sind **konvexe Hüllen**: wie Geschenkpapier, straff um ein Teil gespannt, ohne
Dellen nach innen. Die Sohlen haben Reibung 1, das Schnabelpolster aus TPU-Gummi auch 1, die
Servogehäuse 0 (wie im Training), die Schalen von Rumpf und Kopf 0,5 – darauf rutscht eine gestürzte
Ente nicht wie auf Eis.

Seit der Überarbeitung der Spielschicht kennt die Physik **Oberflächen** mit eigenen Paarwerten:
Baumwolle auf Baumwolle 0,5, TPU auf Baumwolle 0,8, Baumwolle auf Holzboden 0,4. Alle anderen Paare
mischt Box3D als **geometrisches Mittel** √(a · b). Beispiel: Sohle 1 auf Boden 0,6 ergibt
√0,6 ≈ 0,77.

### Maßstab 4

Box3D ist für Körper von 0,1 bis 10 m gebaut, die Glieder der Ente sind nur 2 bis 4 cm groß. Darum
baut Kern sie **4-mal so groß** und rechnet an genau einer Stelle um, nach der
**Froude-Ähnlichkeit**: Lässt man alles passend mitwachsen, verhält sich die große Welt genau wie
die kleine.

Länge mal 4, Masse mal 4³ = 64, Zeit und Geschwindigkeit mal √4 = 2, Drehgeschwindigkeit mal ½,
Moment und Servosteifigkeit mal 4⁴ = 256, Trägheit mal 4⁵ = 1024. Masse mal 64, weil ein Würfel mit
4-facher Kantenlänge 4 · 4 · 4 = 64-mal so viel Inhalt hat. Das Netz bekommt trotzdem echte
Einheiten: Drehgeschwindigkeiten multipliziert Kern vorher mit 2.

> **So kannst du es dir vorstellen:** Wenn im Film ein Modellhaus einstürzt, sieht es nur echt aus,
> wenn man es in Zeitlupe abspielt. Größere Dinge bewegen sich gemächlicher – genau das sagt der
> Faktor √4 für die Zeit.

### Aussehen, Rollschuhe, Einbau

Das Aussehen stammt aus Pollens 3D-Modellen (Lizenz CC BY-NC-SA 4.0, nicht kommerziell), darum
zeigen die Bilder dieses Buchs eine eigene Gestaltung. Im Katalog gibt es auch **Micro Duck
(Rollers)**: Kufen mit je zwei freien Rädern statt Sohlen, 23,5 mm höher.

```cpp
game::Level level;
const game::ActorId duck =
    game::add_micro_duck(level, {0, 0, 0});
```

Zeile 1 legt ein Level an, Zeile 2–3 setzen die Ente auf (0, 0, 0) und stellen die Welt auf 100
Schritte pro Sekunde mit 16 Unterschritten. Das Studio macht beim Einfügen aus dem Katalog dasselbe.

## 13.4 Das Servomodell

Kern bildet Pollens gemessenes Modell des XL330 als **Gerätemodell** nach: ein kleines Gesetz,
dessen Zahlen in einem **Profil** stehen.

| Zahl | Wert | Bedeutung |
|---|---|---|
| K Steifigkeit | 0,554 N·m/rad | wie stark er zum Ziel zieht |
| D Dämpfung | 0,053 N·m·s/rad | bremst schnelle Bewegung |
| F Reibung | 0,0048 N·m | Reibung im Gelenk |
| T Höchstmoment | 0,963 N·m | mehr Kraft geht nicht |
| A Armatur | 0,0018 kg·m² | Drehmasse des Motors |
| Verzögerung | 20 ms | Befehl kommt später an |

Das Gesetz: Der Servo zieht mit `K · (Ziel − Winkel)`, höchstens mit T. Die Dämpfung bremst, die
Reibung hält dagegen.

**Rechenbeispiel:** 0,5 rad neben dem Ziel zieht er mit 0,554 · 0,5 ≈ 0,28 N·m, nicht einmal ein
Drittel seiner 0,963 N·m. Volle Kraft erst bei 0,963 / 0,554 ≈ 1,7 rad, fast 100°. Die Servos der
Ente sind also **weich**.

> **So kannst du es dir vorstellen:** Ein Gummiband zwischen Gelenk und Ziel. Je weiter weg, desto
> stärker der Zug, bis zu einer Grenze. Die Dämpfung ist wie Honig im Gelenk.

Eine einzige Funktion setzt dieses Gesetz in jedem Physikschritt um, im Studio, im Lehrer und auf dem
Prüfstand aus [Kapitel 16](16-kalibrierung.md). Box3D rechnet die Feder so, dass sie immer stabil
bleibt; die Reibung ist ein Motor mit Sollgeschwindigkeit 0. Jede Ente, sogar jeder Servo, kann ein
eigenes Profil bekommen.

> **Vorsicht, Falle:** Die Firmware des echten Servos kann den Strom auf 1,75 A begrenzen (0,64 N·m). Dann
> kommt die Ente nicht mehr vom Rücken hoch. Pollen trainiert ohne diese Grenze, Kern auch.

## 13.5 Der Takt

Im Training dauert ein Physikschritt 5 ms. In Kern dauert er 10 ms Weltzeit mit 16 Unterschritten.
Warum das passt: In der 4-fach großen Welt läuft die Zeit √4 = 2-mal langsamer, also sind 10 ms
Weltzeit genau 5 ms Roboterzeit.

Alle **4 Physikschritte** fragt Kern das Netz – das ist der **Regeltakt**: 20 ms, also 50-mal pro
Sekunde. Dass nur jeder 4. Schritt zählt, heißt **Decimation**. Auch die Befehlsverzögerung sind
4 Schritte (20 ms, im Training 15–30 ms): Die Servos bekommen immer das Ziel von vor 4 Schritten.
Feste Schritte erklärt [Kapitel 8](08-spiel-laufzeit.md).

> **So kannst du es dir vorstellen:** Der Schlagzeuger spielt Sechzehntel (Physik), die Sängerin
> setzt auf jedem vierten Schlag ein (Regeltakt), der Chor nur auf jedem achten – das ist die
> Spielschicht mit 25 Entscheidungen pro Sekunde.

## 13.6 Die Netze der Ente

Kern lädt **dieselben ONNX-Dateien** wie der Roboter. Das Lauf- und Stehnetz `velstand.onnx` ist
eingebaut, die anderen lädt das Studio nach. Kern liest ONNX selbst und nimmt genau eine Form an:
Eingaben normalisieren, Schichten von Neuronen mit der Funktion **ELU** dazwischen, am Ende eine
lineare Schicht. Alles andere lehnt es mit Begründung ab. Das eingebaute velstand rechnet bis auf
2 Hunderttausendstel genau wie die Referenz.

| Bewegung | Netz |
|---|---|
| Laufen und Stehen | `velstand.onnx` (v8) |
| ältere Gangart | `alpha_walking`, `alpha_stand` |
| Sitzen / Aufstehen | `alpha_sitstand.onnx` |
| Socke aufheben | `alpha_ground_pick.onnx` |
| Kick links / rechts | `ball_kick_left` / `_right` |
| Rolle | `roulade.onnx` |
| Rollschuh, Ducken | `roller`, `roller_crouch` |
| **Sprung** (Kern) | `jump.onnx` |

Laufen läuft dauernd, die anderen sind **Episoden** mit fester Dauer, etwa Kick 0,5 s, Sprung 2 s,
Aufheben 4 s. Der Schnabel ist ein Servo ohne Netz. Die Gangart wählt **Walking network**.

### Wer gerade dran ist

In jedem Regeltakt folgt Kern derselben Reihenfolge wie Pollens Steuerprogramm robotd: abgelaufene
Bewegungen beenden; ein Netz wählen (Vorrang: laufende Bewegung, Aufheben, Sitzen, Rollen, Stehen,
sonst Laufen); rechnen und auf Servoziele umrechnen; die Zeitfenster weiterschalten. Jedes Netz
bekommt genau die Befehle, mit denen es trainiert wurde.

Eine Bewegung startet nur, wenn keine läuft, die Ente nicht sitzt und das Netz geladen ist – sonst
kommt der Grund zurück. Fehlt ein Netz, springt kein anderes ein: Die Servos halten die
Grundstellung.

### Vom Netz zum Servoziel

Jedes Servoziel ist **Home-Winkel + Scale · Ausgabe**, höchstens ±π. Kern fährt jedes Netz **wie
trainiert**: Scale 1, ohne Glättung (nur Rollschuh und Ducken: 0,8). Pollens Programm auf dem echten
Roboter nimmt Scale 0,9 und einen **Tiefpass**: neues Ziel = 0,7 · Netzwert + 0,3 · altes Ziel
(Kopf: 0,5). Das Feld **Actions to servo goals** wählt `training` (Standard) oder `robot`.

> **Mission:** Stell bei der Ente **Actions to servo goals** auf `robot`, übernimm sie mit F8 und
> drück **3** für den Kick. Dann dasselbe mit `training`. Gemessen kippen die Kicks mit dem
> Roboter-Profil um und bleiben mit `training` stehen.

## 13.7 Was das Netz sieht: 61 hinein, 14 heraus

| Plätze | Inhalt |
|---|---|
| 0–2 | wie schnell der Rumpf sich dreht |
| 3–5 | wo „unten“ ist; aufrecht (0, 0, −1) |
| 6–19 | Gelenkwinkel minus Grundstellung |
| 20–33 | Gelenkgeschwindigkeiten (einen Takt alt) |
| 34–47 | die letzte eigene Ausgabe |
| 48–50 | Laufbefehl: vor, seitwärts, drehen |
| 51–54 | Kopfbefehl |
| 55–60 | Körperhaltung; 55/56 nutzt der Sprung |

Der Laufbefehl reicht bis ±0,4 m/s vorwärts, ±0,3 m/s seitwärts, ±1 rad/s Drehung. Er wird geglättet
wie auf dem Roboter: Jeder Takt rückt 20 % näher ans Ziel (**EMA**). Von 0 auf 1 geht es also
0,2 – 0,36 – 0,49 …

velstand rechnet: normalisieren, dann 512, 256 und 128 Neuronen, dann 14 Ausgaben – die 14
Servoziele. Was ein Neuron ist, erklärt [Anhang A](A-grundlagen-neuronale-netze.md).

> **Für Profis:** Warum bekommt das Netz seine letzte Ausgabe zurück? So kennt es seinen letzten
> Befehl, obwohl die Servos ihn erst 20 ms später bekommen. Kern liefert diese Plätze genau wie im
> Training.

## 13.8 Sensoren

Die Ente sieht nur, was der echte Roboter sieht, von denselben Stellen aus.

**Kopf-ToF (VL53L8CX):** „Time of Flight“ misst, wie lange Licht hin und zurück braucht. 8 × 8
Zonen, Sichtfeld 45° × 45°, 4 m Reichweite, 15 Bilder pro Sekunde. In Kern ist jede Zone ein
**Strahltest** (Ray-Cast), der die eigenen Glieder überspringt. Jede Zone gilt als **Boden** (ein
Strahl nach unten, der mindestens 85 % der Sensorhöhe schafft), **zu nah** (waagerecht unter 10 cm)
oder **Hindernis** – dazu Entfernung, Richtung und ob links oder rechts frei ist (blockiert unter
1 m).

**Kopfkamera (IMX219):** hochkant eingebaut, 62° hoch, 37,4° breit. „Sieht sie den Ball?“ heißt:
Seine Mitte liegt im Bild, und ein Strahl dorthin trifft zuerst den Ball.

**IMU:** spürt Drehung und wo unten ist. „Gefallen“ heißt: 200 ms lang deutlich schief (der Wert
für unten über −0,5). Kurzes Stolpern zählt nicht. velstand v8 steht von selbst wieder auf.

**Mund-Encoder:** Ein Encoder misst den Winkel des Servos. Ist der Mund auf „zu“ gestellt und steht
trotzdem mehr als 2° offen, steckt etwas drin. Leer schließt er bei 0,0°, eine Socke hält ihn
3,6–7,8° offen. Er fährt von −5° bis 30°.

> **Mission:** Im Play zeigt das Studio oben rechts **HEAD CAMERA** (Kamerabild und Zustand) und
> **TOF 8×8**: rot Hindernis (heller, je näher), grün Boden, gelb zu nah. Lenk die Ente auf eine
> Wand zu und beobachte das Gitter. Die Anzeige schaltest du unter Window › Show › **Robot Camera**.

## 13.9 Wer steuert?

1. **Hand:** Ein Stick (über 5 %) oder eine Robot-Taste gibt dir 2 s Vorrang.
2. **Logik:** die Blueprint-Knoten **Walk** oder **Look**.
3. **Autonomie:** das eigene Verhalten, wenn **Autonomous** an ist.

Steht die Ente still und hat 2 s niemand den Kopf gesteuert, übernimmt der **Idle-Blick** aus
Pollens Software: Kopfbewegungen auf weichen Kurven, langsames Wandern und Muster, die Pollen
„Dösen“ und „Gähnen“ nennt.

### Hochgehoben

Wird der echte Roboter hochgehoben, sollen die Beine nicht in der Luft weiterlaufen. Pollens kleines
Netz `pickup_detector.onnx` (rund 8000 Zahlen) liest dafür in jedem Regeltakt die letzte Sekunde: 50
Zeilen mit je 63 Messwerten. Heraus kommt eine Wahrscheinlichkeit p für „wird gehalten“.

- **Pause** bei p > 0,8 in 5 Takten hintereinander: Die Servoziele gleiten in 300 ms in die
  Standhaltung, der Zustand heißt „Held“.
- **Weiter** bei p < 0,35 in 4 Takten, frühestens nach 300 ms. Die Netze starten frisch.

Servoströme hat Kern nicht, aber das Netz liest diese Spalten ohnehin nicht.

> **Mission:** Starte **Simulate** (Alt+S) und zieh die Ente mit der linken Maustaste **am Rumpf**
> hoch. Der Griff hält sie in ihrer Lage wie eine Hand. Die Beine stoppen. Setz sie ab – sie läuft
> weiter.

## 13.10 Selbstständig: das Verhaltensprogramm

Steuert niemand, wählt ein Programm das Verhalten (im Code „Gehirn“). Es überträgt Pollens Programm
`autod` (Commit `df9a750` vom 01.10.2026) mit allen Zuständen und Zahlen:

- **Chill** (stehen, 3–7 s), **LookAround** (große Blicke), **Wander** (1–3 m zu einem wenig
  besuchten Ziel), **TurnInPlace** (drehen), **Zoomies** (volles Tempo mit „Wheee“, nur mit über
  1,2 m freier Bahn),
- **Stretch**, **Ruffle**, **Dance**, **Sneeze** – kurze Bewegungs- und Klangabläufe,
- **GroundPick** (aufheben), **Nap** (sitzen, 12–40 s), **Petted** (gestreichelt),
  **BallPlay** und **Kick**.

Eine **Energie** von 0 bis 1 gewichtet den nächsten Zustand. Ein **Hindernismodell** sorgt für
Abstand: Unter 0,45 m bremst die Ente und weicht aus, unter 0,15 m dreht sie weg und meidet die Stelle 5 Minuten. Mikrofon und Berührungssensor hat
Kern nicht; Hören und Streicheln kommen über Befehle. Sogar der Zufall ist deterministisch.

### BallPlay

Pollens frühere, nicht öffentliche Software hatte den Zustand BallPlay: anlaufen, ausrichten,
kicken. Kern baut ihn nach dieser Beschreibung nach, mit Pollens Kick-Netz. Er ist
**standardmäßig an**:

1. **Sehen:** ein Ball (Kugel mit Tag `ball` oder Namen „Ball“, höchstens 8 cm hoch) im Kamerabild.
2. **Anlaufen:** Kopf gesenkt, immer langsamer, bis 35 cm.
3. **Ausrichten:** kleine Schritte, bis der Ball 9 cm vor und 4,2 cm neben dem linken Fuß liegt –
   genau wie im Kick-Training.
4. **Kicken:** Kopf gerade, 3 s ruhig, Kick, Quaklaute.
5. **Aufgeben:** nach 6 s ohne Sicht oder 45 s ohne Kick.

Gekickt wird nur links, weil der rechte Kick in Kern manchmal umkippt. Mit **Kern extras** und
geladenem `play.onnx` übernimmt statt BallPlay die Spielschicht.

## 13.11 Die Spielschicht

Pollens Netze sind Bewegungen; keins davon sieht. Was die Ente mit Ball oder Socke tut – hinschauen,
hingehen, kicken, aufheben, wegtragen, ausspucken –, entscheidet das Netz `play.onnx`. Drumherum gibt
es nur Wahrnehmung, keine handgeschriebenen Spielregeln.

![Die Ente spielt (Ente in der eigenen Gestaltung dieses Buchs)](../buch/bilder/duck-play.png)

Die Spielschicht sitzt an der Stelle eines Gamepads und antwortet 25-mal pro Sekunde mit Laufbefehl,
Kopf, Schnabel und welche Bewegung starten soll (Zustand „Play“). Sie braucht **Kern extras**
(Details › Controller), **Autonomous**, einen Mund-Servo, keine Rollschuhe und `play.onnx`. Sie
greift ein, solange sie ein Objekt kennt, etwas im Schnabel ist oder ihre Bewegung läuft – und
niemand von Hand oder per Logik steuert.

> **Mission:** Schalte im Beispiel-Level bei der Ente **Kern extras** aus. Jetzt spielt nicht das
> gelernte Netz, sondern BallPlay. Beobachte den Unterschied, wenn sie den Ball entdeckt.

### Detektor und Tracker

Der **Detektor** ist wie der Servo ein Gerätemodell. Er meldet je Sorte das nächste sichtbare
Objekt, gemessen in seiner Mitte am Boden. Ein Ball über 8 cm oder eine Socke über 3 cm Höhe zählt
nicht – etwa die Socke im Schnabel. Dazu kommen **Rauschen** und **Aussetzer**.

**Rechenbeispiel** mit den Beispielwerten aus dem Code: Die Schätzung streut um 0,002 m +
0,01 · Entfernung. Bei 1 m sind das 0,002 + 0,01 = 0,012 m, rund 1,2 cm. Je weiter weg, desto ungenauer – wie Schätzen mit einem Auge. Im Studio ist der Detektor
rauschfrei, im Training zieht der Lehrer zufällige Werte.

Der **Tracker** merkt sich ein Objekt, das aus dem Bild verschwindet, 3 s lang (unter 0,35 m vor der
Ente 30 s). Einen gekickten Ball meldet er 15 s nicht, eine weggetragene Socke 40 s nicht.

### 23 hinein, 9 heraus

Die **23 Eingaben** sind alle in echten Metern aus Sicht der Ente (x voraus, y links): wo Ball und
Socke vermutlich liegen und wie alt die Meldung ist, wo der linke Fuß steht, ob etwas im Schnabel
ist, wie weit er offen ist, ob sie läuft, aufhebt oder kickt, wo unten ist, wie weit und lange sie
trägt, wie lange sie steht oder geht. Die wahren Positionen bekommt sie nie.

**Stillstand** misst Kern an den Beinen: beide Sohlen auf etwas, das nicht die Ente ist, und keine
Bewegung läuft. Denn auf „ein wenig vorwärts“ setzt Pollens Gang manchmal gar nicht an.

Die **9 Ausgaben**: vorwärts, seitwärts, drehen, Kopf gieren, Kopf nicken, Schnabel auf, Kick
links, Aufheben, Hals beugen. Die Laufbefehle haben eine **Totzone** wie ein Gamepad-Stick: Unter
20 % steht die Ente. Beim Aufheben steuert das Netz den Schnabel selbst.

### Socke, Ball, Beispiel-Level

Die **Socke** sind sieben Stoffklumpen von 12,5 mm, lose 3,4 cm hoch gehäuft, 25 g, verbunden durch
weiche Federn: Was der Schnabel packt, zieht den Rest mit. Der **Ball** im Beispiel-Level ist 12 cm groß, 50 g schwer,
mit Rollwiderstand 0,03 – sonst rollte er ewig.

Das **Beispiel-Level** ist ein Zimmer von 2,5 × 2,5 m mit Möbeln, Ball und zwei Socken. Ein
Level-Blueprint lässt die Ente einmal quaken, wenn der Ball ins Kamerabild kommt – eine Vorführung des Blueprints, kein Verhalten der Ente.

### Wie gut es klappt

Je 32 Episoden; „Variation“ heißt mit zufälligen Abweichungen bei Bau, Servos und Sensoren:

| Ergebnis | sauber | Variation |
|---|---|---|
| Ball weggekickt | 9 von 11 | 7 von 11 |
| Socke aufgehoben | 4 von 21 | 13 von 21 |
| aufgehoben, getragen, ausgespuckt | 1 von 21 | 0 von 21 |
| Stürze | 2 von 32 | 10 von 32 |

Ehrlich: Kicken klappt gut, Tragen noch kaum.

## 13.12 Die Ente steuern

| Bewegung | Taste | Gamepad / Touch |
|---|---|---|
| Sitzen / Aufstehen | 1 | B / SIT |
| Aufheben (Rollen: Ducken) | 2 | Y / PICK |
| Kick links / rechts | 3 / 4 | – / KICK L, KICK R |
| Rolle (gehalten weiter) | 5 | X / ROLL |
| Schnabel auf / zu | 6 | – / BEAK |
| Quaken | Leertaste | RT / QUACK |
| „Wheee“ (gehalten) | V | LT / WHEEE |
| **Sprung** | **7** | – / **HOP** |

Laufen: W/S vor und zurück, A/D drehen, sonst linker Stick oder MOVE-Stick. Die Laute stammen aus
einer bitgenauen Übertragung von Pollens Synthesizer ([Kapitel 7](07-audio.md)).

**Der Sprung:** Taste 7, HOP oder der Logik-Knoten **Robot Move** mit „jump“. Kern wechselt hart auf
`jump.onnx` und nach 2 s zurück; das Netz erfährt die Zeit über die Plätze 55 und 56, die Pollens
Netze nie benutzen. Im Studio: 3 von 3 echten Sprüngen, 9,0 mm Steigen, 120 ms Flug, danach steht und
läuft die Ente wieder.

> **Krass, aber wahr:** In Pollens Netzsatz gibt es keinen Sprung. Dieses Netz hat Kerns eigener Lehrer
> trainiert, auf derselben Physik, die du im Studio siehst ([Kapitel 14](14-lehrer.md)).

**Details-Panel:** **Autonomous (camera, ToF)**, **Walking network**, **Actions to servo goals**,
**Kern extras**; nur im Level-JSON `rollers` und `servoDelay` (Standard 0,02 s).

**Blueprint-Kategorie Robots** ([Kapitel 9](09-logik.md)): Walk, Look, Set Autonomous, Chirp, Robot
Move (`sit`, `pick`, `kick_left`, `kick_right`, `roulade`, `jump`), Robot Beak, Pet Robot, Robot
Hears, Robot ToF, Robot Camera Sees, Robot State, Robot Holds Something.

## 13.13 Die Ente aus C++ steuern

```cpp
w.walker_command(duck, 0.2f, 0, 0, 3.0f);
w.walker_look(duck, 0.5f, 0.2f, 2.0f);
std::string why = w.walker_skill(duck, "pick");
bool holds = w.walker_holding(duck);
```

Zeile 1: 0,2 m/s vorwärts, 3 s lang. Zeile 2: Kopf gieren und nicken. Zeile 3: Aufheben starten;
leerer Text heißt „gestartet“, sonst steht der Grund drin. Zeile 4: Ist etwas im Schnabel? Mehr,
auch für den Lehrer, in [Kapitel 14](14-lehrer.md).

## 13.14 Nachgemessen

| Fall | Kern | Pollen (MuJoCo) |
|---|---|---|
| Befehl 0 | steht, driftet unter 2 mm | steht |
| vorwärts 0,4 m/s | ≈ 0,135 m/s | ≈ 0,17 m/s |
| drehen 1 rad/s | ≈ 0,61 rad/s | ≈ 0,97 rad/s |
| auf dem Rücken | steht nach ≤ 12 s | steht auf |

Ohne Armatur zappelte Kerns Ente gut dreimal so stark. Der Kick links schießt den Ball 4 m weit.

## 13.15 Was noch fehlt

Getriebespiel (±1°) fehlt, die Armatur ist nur eine Scheibe, Box3D mischt Reibung anders als
MuJoCo, der rechte Kick kippt, BallPlay ist ein Nachbau, Servoströme fehlen. Nur in Kern gibt es
Spielschicht, Schnabel-Timing und Sprung. Wer mit dem echten Roboter arbeitet: Die
Sicherheitsregeln stehen in [Kapitel 16](16-kalibrierung.md) – nur zusammen mit einem Erwachsenen.

## Level geschafft!

- Du kennst die Micro Duck: 25 cm, 0,74 kg, 14 Servos plus Schnabel.
- Du weißt, warum Kern bei der Physik nie schummelt.
- Du rechnest mit dem Maßstab 4 zwischen Kern-Welt und echtem Roboter hin und her.
- Du berechnest das Moment eines Servos aus K und dem Abstand zum Ziel.
- Du weißt, was das Netz sieht (61 Zahlen) und befiehlt (14 Ziele).
- Du kennst ToF, Kamera, IMU, Mund-Encoder und die Rangfolge Hand – Logik – Autonomie.
- Du steuerst alle Bewegungen der Ente, auch Kerns Sprung.

## Rätsel-Ecke

1. Was bewegt der fünfzehnte Servo der Ente?
2. Warum baut Kern die Ente 4-mal so groß?
3. Ein Servo steht 0,2 rad neben seinem Ziel. Mit welchem Moment zieht er?
4. Die Ente hat eine Socke im Schnabel. Warum meldet der Detektor sie nicht?
5. **Boss-Rätsel:** Ein Physikschritt dauert in Kern 10 ms Weltzeit. Wie viel echte Roboterzeit
   liegt zwischen zwei Entscheidungen der Spielschicht?

### Lösungen

1. Den Unterschnabel. Die anderen 14 sitzen in Beinen und Hals.
2. Box3D rechnet am besten mit Körpern von 0,1 bis 10 m; die Glieder sind nur 2–4 cm groß.
3. 0,554 · 0,2 ≈ 0,11 N·m – weit unter dem Höchstmoment 0,963 N·m.
4. Eine Socke zählt nur, wenn ihre Mitte höchstens 3 cm hoch liegt. Im Schnabel ist sie höher.
5. Ein Schritt sind 5 ms Roboterzeit, ein Regeltakt 4 Schritte, und die Spielschicht entscheidet
   jeden zweiten Takt: 8 · 5 ms = 40 ms.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
