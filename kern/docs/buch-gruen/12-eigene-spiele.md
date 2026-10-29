# 12 Dein eigenes Spiel

> **Kurz gesagt:** Jetzt bist du dran! Kern bietet drei Wege zum eigenen Spiel. Weg A geht ganz ohne
> Programmieren im Studio, und den gehen wir hier Schritt für Schritt: Am Ende hast du ein kleines
> Sammelspiel, das sogar am Telefon läuft. Weg B und C sind für Leute, die programmieren. Wir schauen
> ihnen über die Schulter und holen die Ideen heraus, die in jedem Spiel stecken.

> **Begriffe vorab:**
> - **Frame** – ein einzelnes Bild auf dem Bildschirm; ein Spiel malt 60 oder mehr davon pro Sekunde, wie ein Daumenkino
> - **C++ und JavaScript** – zwei Programmiersprachen; Kern selbst ist in C++ geschrieben, Webseiten sprechen JavaScript
> - **Modul (.wasm)** – die fertig übersetzte Engine als eine Datei, die jeder Browser ausführen kann
> - **Webserver** – ein Programm, das Dateien so ausliefert, wie eine Internetseite sie ausliefert
> - **Open Source** – der Programmtext ist offen; jeder darf ihn lesen, nutzen und weitergeben, nach den Regeln der Lizenz
> - **Lizenz** – die Spielregeln, was man mit einem Werk tun darf
> - **Shader** – ein kleines Programm, das auf der Grafikkarte läuft und jedes Pixel einfärbt
> - **Trigger** – eine unsichtbare Zone, die etwas auslöst, wenn jemand hineinläuft
>
> Eine **HTML-Datei** ist das Dateiformat von Webseiten; jeder Browser öffnet sie.

| Weg | Was du brauchst | Was herauskommt |
|---|---|---|
| **A** im Studio | nur die Maus, kein Programmieren | eine HTML-Datei: dein Spiel |
| **B** in C++ | Programmierkenntnisse | ein Spiel, bei dem du alles selbst schreibst |
| **C** aus JavaScript | Programmierkenntnisse | Kern als Baustein in deiner eigenen Webseite |

Ehrlich gesagt: Für Weg B und C muss man programmieren können. Weg A schaffst du sofort. Alle drei
Wege benutzen dieselbe Engine, also dieselbe Grafik und dieselbe Physik, und man kann sie mischen.

---

## 12.1 Weg A: ein Spiel im Studio, ohne Code

Ein Spiel zu bauen heißt hier: fertige Bausteine in eine Welt stellen und ihnen sagen, wie das Spiel
gewonnen wird. Das ist wie ein Baukasten mit Regeln. Wir bauen „Orb Garden“: Eine Figur läuft über eine Fläche
mit Hindernissen, sammelt leuchtende Kugeln (**Orbs**) ein, weicht einer Gefahrenzone aus und gewinnt,
wenn alles eingesammelt ist. Die Bedienung der Panels kennst du aus [Kapitel 11](11-studio.md). Los
geht's!

### Schritt 1: Ein neues Level

Öffne Kern Studio über https://kernengine.de/, dann **Datei › Neues Level...** (File › New Level...),
Vorlage **Grundformen** (Basic), **Erstellen** (Create). Du hast einen Boden von 40 × 40 m, Sonne,
Himmel und einen Würfel. Trag in den **Welteinstellungen** (World Settings) als **Level-Name** (Level name)
und als **Spieltitel** (Game title) „Orb Garden“ ein.

> **Achtung, Falle!** Warum nicht **Leer** (Empty)? Weil die Vorlage **Leer** wirklich **nichts** hat, auch keinen Boden.
> Deine Figur würde ins Bodenlose fallen.

### Schritt 2: Die Spielfigur

Kurz zur Orientierung im Studio: Der **Viewport** ist das große Bild der Welt in der Mitte. Die
**Inhaltsablage** (Content Drawer) ist die Bauteil-Schublade unten. Der **Outliner** ist die Liste aller Dinge im
Level, und **Details** zeigt die Einstellungen des Dings, das du gerade ausgewählt hast. Ein
beschriftetes Bild des Studios findest du in [Kapitel 11](11-studio.md).

Öffne die **Inhaltsablage** (**Strg+Leertaste**) und zieh aus **Spieler** (Players) eine **Figur** (Character) in den Viewport.
Das Log meldet „Character is the player now“ (Meldungen im Log zeigt Studio auf Englisch), und im
Outliner trägt der neue Actor „Character“ das Abzeichen **Spieler** (Player): Diese Figur ist jetzt
der Spieler. In **Details › Spieler-Steuerung** (Details › Player Controller) stellst du ein, wie sie sich anfühlt:
Tempo, Beschleunigung, Sprungkraft und die Kamera, die von hinten, aus den Augen der Figur oder von oben
schauen kann.

> **Probier's aus!** Drück sofort **Spielen** (Play) (Alt+P). W A S D läuft, die Maus schaut, die Leertaste
> springt, **Esc** bringt dich zurück. Tausch zum Spaß die Figur gegen eine **Rollende Kugel** (Rolling
> Ball) oder eine **Drohne** (Drone) aus **Spieler**: Der zuletzt platzierte ist der Spieler, oder du
> wählst einen mit Rechtsklick › **Zum Spieler machen (beim Spielen steuern)** (Make Player (control it in Play)).

### Schritt 3: Hindernisse und Physik

1. Aus **Formen** (Shapes) ein paar **Würfel** (Cube), eine **Rampe** (Ramp) und eine **Ebene** (Plane) setzen und mit dem **Gizmo**
   zurechtrücken. Das Gizmo sind die bunten Anfasser-Pfeile am ausgewählten Ding: Daran ziehst du, um
   es zu verschieben, zu drehen oder größer zu machen.
2. Aus **Physik** (Physics) einen **Kistenstapel** (Crate Stack) und eine **Scharniertür** (Hinged Door): Die reagieren auf deine Figur.
3. Aus **Spielablauf** (Gameplay) eine **Bewegliche Plattform** (Moving Platform) und einen **Drehbalken** (Spinner), die sich von selbst bewegen.
4. Mit **Simulieren** (Simulate) (Alt+S) prüfen. Du kannst Körper greifen und werfen.

Ein **Würfel** aus **Formen** ist ein festes Hindernis: Man stößt dagegen, aber er bewegt sich nicht. Soll er
fallen, musst du ihm Physik geben: in Details mit **Hinzufügen › Starrkörper** (Add › Rigid Body) und
**Mobilität** (Mobility) auf **Dynamisch (Physik simulieren)** (Dynamic (simulate physics)), oder du
nimmst gleich einen **Physik-Würfel** (Physics Cube) ([Kapitel 6](06-physik.md)).

> **Für Neugierige:** Nach **Simulieren** übernimmt **K** die simulierte Lage ins Level, etwa für einen
> Kistenstapel, der natürlich eingestürzt aussehen soll.

### Schritt 4: Etwas zum Einsammeln

1. Aus **Spielablauf** mehrere **Orb** (1 Punkt) und einen **Stern** (Star) (10 Punkte) verteilen. Trick: **Alt +
   Gizmo ziehen** zieht bei jedem Zug eine Kopie heraus.
2. Alle Orbs auswählen und mit dem Ordner-Knopf im Outliner in einen Ordner legen.
3. In **Welteinstellungen › Siegbedingung** (World Settings › Win condition) **Alle Pickups sammeln**
   (Collect all pickups) wählen. **Pickups** heißt alles zum Einsammeln, hier also Orbs und Stern.

### Schritt 5: Gefahr, Checkpoint und Zeitlimit

1. Eine **Gefahr** (Hazard) setzen und mit **Skalierung** (Scale) auf Größe ziehen: Wer hineinläuft,
   hat das Spiel verloren, da hilft auch kein Checkpoint.
2. Einen **Checkpoint** setzen. Hier startet die Figur neu, wenn sie zu tief fällt. Wie tief „zu tief“
   ist, legt **Kill-Z (darunter Wiedereinstieg)** (Kill Z (respawn below)) in den
   **Welteinstellungen** fest: eine Höhe, unter der das Spiel die Figur zurückholt.
3. Wer mag, setzt in den **Welteinstellungen** ein **Zeitlimit (0 = keins)** (Time limit (0 = none)):
   Das **HUD** (die Anzeige am Bildschirmrand mit Punkten und Zeit) zählt herunter.

> **Achtung, Falle!** Statt einer **Gefahr** gibt es auch eine **Schadenszone** (Damage Zone), die Gesundheit kostet.
> Sie wirkt aber nur auf Figuren, die überhaupt Gesundheit haben. Die Figur „Character“ hat von Haus aus
> keine; gib sie ihr in Details mit **Hinzufügen › Gesundheit** (Add › Health), sonst läuft sie unbeschadet durch die Zone.

Andere Siegbedingungen: **Ein Ziel erreichen** (Reach a goal trigger) (mit einem **Ziel** (Goal)),
**Das Zeitlimit überleben** (Survive the time limit) oder **Eine Punktzahl erreichen** (Reach a target score).

### Schritt 6: Logik ohne Programmieren

Bis hierher hat jeder Baustein sein eingebautes Verhalten. Willst du eigenes Verhalten, baust du es
aus **Logik-Graphen**: Kästchen, die du mit Drähten verbindest ([Kapitel 9](09-logik.md)). Wir machen
eine Zone, die Punkte gibt:

1. Einen **Nachrichten-Trigger** (Message Trigger) setzen und in **Details › Trigger** die **Aktion**
   (Action) auf **Nur Logik-Ereignis** (Logic event only) stellen.
2. In Details auf **Logik** (Logic) klicken. Der Logik-Editor öffnet den Graphen dieses Triggers. Der
   Logik-Editor ist nicht übersetzt: Seine Knöpfe heißen auch in der deutschen Oberfläche englisch.
3. **Templates › Score zone (overlap +10)**: Wer die Zone betritt, bekommt 10 Punkte, eine Meldung „+10“
   und einen Ton.
4. **Compile** (F7), dann **Viewport**.

Warum **Nur Logik-Ereignis**? Normalerweise tut ein Trigger selbst etwas, sobald jemand hineinläuft: Er
lässt gewinnen, verlieren oder zeigt eine Meldung mit Ton. Mit **Nur Logik-Ereignis** tut er von sich aus
nichts, sondern sagt nur deiner Logik Bescheid: „Da ist jemand drin.“ So entscheidet allein dein
Graph, was passiert, und nichts passiert doppelt. Wie ein Türsensor, der nicht selbst klingelt,
sondern nur meldet: Was dann geschieht, bestimmst du.

Warum **Compile**? Compile prüft deinen Graphen auf Fehler, etwa einen Draht ins Leere, und zeigt eine
Ampel: OK, Warnung oder Fehler. Das ist wichtig, denn ein Graph mit Fehlern wird beim Spielstart
abgeschaltet. Das Spiel läuft dann zwar weiter, aber deine Zone gibt keine Punkte, und du wunderst
dich. Erst prüfen, dann spielen.

Weitere Vorlagen: eine Tür, die auf **Interact** (Taste F) aufgeht, ein Spawner (ein Erzeuger, der
Dinge auftauchen lässt), eine Ampel und ein Verhalten für die Micro Duck.

### Schritt 7: Ton

In **Welteinstellungen › Audio** eine **Musik** (Music) und eine **Atmosphäre** (Ambience)
(Hintergrundklang) wählen. Aus der Gruppe **Audio** etwa **Wind** (Ambient Wind) setzen, oder im
Reiter **Sounds** der **Inhaltsablage** eine eigene Datei importieren
und in den Viewport ziehen. Figuren, Pickups und Trigger bringen ihre Geräusche schon mit
([Kapitel 7](07-audio.md)).

### Schritt 8: Testen

Drück **Spielen** und leg los! Mit **F8** steuerst du die Figur nicht mehr, sondern schaust mit einer freien
Kamera zu, während das Spiel weiterläuft; noch einmal F8, und du steuerst wieder. Mit
**Spielen › Anzahl der Spieler › 2 Players (listen server)** (Play › Number of Players › 2 Players (listen server))
testest du zu zweit. Findet Studio einen Fehler im Level, startet das Spiel nicht, und der Grund
steht im **Ausgabeprotokoll** (Output Log), dem Protokoll unten in der **Inhaltsablage**.

### Schritt 9: Speichern

**Strg+S** speichert eine `.klevel`-Datei. Sie enthält alles, auch Texturen (die Oberflächenbilder) und
importierte Sounds. Du
kannst sie weitergeben und in jedem Studio öffnen (Strg+O oder auf die Seite ziehen). Ungespeicherte
Arbeit sichert Studio zusätzlich im Browser und bietet sie beim nächsten Start an.

### Schritt 10: Exportieren

**Datei › Spiel verpacken (HTML)...** (File › Package Game (HTML)...), Titel prüfen, **Verpacken**
(Package). Studio lädt `Orb_Garden.html` herunter und bietet **Spielen** (Play it) an. Diese eine Datei enthält Engine, Level und Spiel. Sie läuft ohne Internet,
direkt von der Festplatte, von jeder Homepage und am Telefon mit Touch-Sticks. Eine Ausnahme gibt es:
Die Bewegungen der Micro Duck außer dem Laufen sind eigene Dateien, die das Spiel nachlädt (12.6 und
[Kapitel 13](13-micro-duck.md)).

> **Probier's aus!** Spiel `Orb_Garden.html` zuerst am Computer: Doppelklick, und sie öffnet sich im
> Browser. Wer eine eigene Homepage hat, lädt die Datei dort hoch und öffnet die Adresse im Browser des
> Telefons. Dann erkennt die Engine den Touchscreen und zeichnet Sticks und Knöpfe von selbst. Du hast
> gerade ein Handyspiel gebaut! (Eine Datei per Messenger aufs Telefon zu schicken klappt nicht auf
> jedem Telefon, weil manche sie nicht im Browser öffnen.)

![Dasselbe Spiel auf dem Telefon: die Engine erkennt Touch und zeichnet Sticks und Knöpfe](../buch/bilder/game-phone.png)

---

## 12.2 Weg B: ein Spiel in C++

> **Zum Reinschauen:** Dieser Abschnitt und die nächsten beiden zeigen, wie Profis Kern von Code aus
> steuern. Programmieren musst du dafür nicht können. Lies sie als Blick hinter die Kulissen: Die
> Ideen darin stecken in jedem Spiel, auch in deinem Studio-Spiel.

Für Spiele, die mehr wollen als die fertigen Bausteine, schreibt man das ganze Spiel selbst in C++. Die
Engine übernimmt dabei das Zeichnen und alles, was mit dem Browser zu tun hat. Das Spiel sagt ihr nur,
was in der Welt ist und was sich bewegt.

> **Stell dir vor …** ein Theater. Die Engine ist die Bühnentechnik mit Licht, Vorhang und Kulissen.
> Dein Spiel ist das Stück. Die Technik fragt dich jeden Augenblick: „Was passiert jetzt?“ Du sagst es
> ihr, und sie macht es sichtbar.

### Die Spielschleife

Jedes Spiel, ob von Kern oder von sonst wem, folgt demselben Grundmuster, der **Spielschleife**:

1. **Einmal am Anfang vorbereiten:** Welche Dinge gibt es? Wie sehen sie aus? Wo steht die Kamera?
2. **Dann in jedem Frame, immer wieder:** Eingabe lesen (welche Tasten sind gedrückt?), die Welt ein
   Stück weiterrechnen (Auto fährt, Kiste dreht sich), Bild malen.
3. **Am Ende aufräumen.**

Das ist ein Daumenkino, das sich selbst zeichnet: Für jede Seite fragt die Engine dein Spiel, was sich
seit der letzten Seite geändert hat, und malt dann die neue Seite.

Ein kleiner Haken: Was „in diesem Frame“ passiert ist, etwa „die Leertaste wurde gerade gedrückt“,
gilt auch nur für diesen einen Frame. Danach setzt die Engine es zurück. Ein Programm muss so etwas
also sofort auswerten, sonst hat es den Tastendruck verpasst.

> **Für Neugierige:** Das Spiel ist in C++ eine Klasse mit Teilen für „vorbereiten“, „jeden Frame“,
> „Fenster hat neue Größe“ und „aufräumen“. Die genauen Namen und Werte stehen im blauen Buch,
> Kapitel 12.

### Gleich schnell auf jedem Bildschirm

Ein schneller Bildschirm zeigt 120 Bilder pro Sekunde, ein langsamer vielleicht 60. Würde das Spiel in
jedem Bild die Kamera um ein festes Stück drehen, drehte sie sich auf dem schnellen Bildschirm doppelt
so schnell. Das wäre unfair und verwirrend.

Die Lösung: Die Engine verrät dem Spiel in jedem Frame, **wie viel Zeit seit dem letzten Bild
vergangen** ist. Das Spiel rechnet: Strecke = Tempo mal vergangene Zeit.

Ein Beispiel: Die Kamera soll sich 90 Grad pro Sekunde drehen.

| Bildschirm | Zeit pro Bild | Drehung pro Bild | Drehung pro Sekunde |
|---|---|---|---|
| 60 Bilder/s | 1/60 s | 1,5 Grad | 60 · 1,5 = 90 Grad |
| 120 Bilder/s | 1/120 s | 0,75 Grad | 120 · 0,75 = 90 Grad |

Mehr Bilder, aber kleinere Schritte, und pro Sekunde kommt genau dasselbe heraus. Damit nach einer
Pause (etwa wenn du kurz einen anderen Tab offen hattest) nicht ein riesiger Sprung entsteht, zählt
Kern höchstens eine Zehntelsekunde pro Frame.

### Das kleinste Kern-Spiel: hello

Das Beispiel **hello** zeigt auf einem Feld 64 orange Kisten, die sich drehen und Schatten werfen. Mit
Maus oder Pfeiltasten kreist du um sie herum, und ein Klick färbt eine Kiste rot. Dahinter stecken zwei
schöne Ideen:

**Instancing, das Stempel-Prinzip.** Man könnte 64 Kisten einzeln an die Grafikkarte schicken. Das
wären 64 Zeichenbefehle, und jeder kostet Zeit. Stattdessen schickt hello **eine** Kiste und eine
Liste mit 64 Einträgen: wo jede steht, wie sie gedreht ist, welche Farbe sie hat. Die Grafikkarte
stempelt die eine Kiste dann 64-mal an verschiedene Stellen, mit einem einzigen Zeichenbefehl. So
malen Spiele ganze Wälder oder Straßen voller Autos. Und weil sich in jedem Frame nur die Liste ändert,
schickt das Spiel auch nur die Liste neu.

> **Stell dir vor …** du verzierst eine Geburtstagskarte mit 64 Sternen. Einzeln malen dauert ewig.
> Mit einem Sternstempel geht es im Nu, und mit verschiedenen Stempelkissen bekommt jeder Stern sogar
> seine eigene Farbe.

**Picking, auf etwas zeigen.** Woher weiß das Spiel, welche Kiste du angeklickt hast? Die Engine schießt
einen unsichtbaren Strahl von der Kamera durch die Stelle, auf die du geklickt hast, hinein in die
Welt. Das erste, was der Strahl trifft, ist die angeklickte Kiste. Bei der Stempel-Kiste verrät die
Engine sogar, welcher der 64 Abdrücke getroffen wurde.

### Bauen

Programmtext in C++ kann der Browser nicht direkt ausführen. Ein **Übersetzungsprogramm** (Compiler)
macht daraus ein Modul, eine `.wasm`-Datei, die jeder Browser versteht. Kern merkt sich dabei jede
übersetzte Datei. Beim zweiten Mal wird nur übersetzt, was sich geändert hat, und das geht viel
schneller. Die Befehle und Schalter dafür stehen im blauen Buch, Kapitel 12, mehr zum Bauen in
[Kapitel 2](02-bauen-und-testen.md).

### Ein ganzes Spiel: orbrun

Das Beispiel **orbrun** ist ein komplettes Rennspiel, nur aus Programmtext: Ein schwebendes Auto
(Hovercar) rast durch eine Stadt und sammelt 24 Orbs, während es langsam Nacht wird. Steuerung: W/S
fahren, A/D lenken, Leertaste Boost, N lässt die Zeit schneller laufen, Enter baut eine neue Stadt. Am
Telefon lenkt die linke Bildschirmhälfte, die rechte boostet.

> **Achtung, Falle!** Nicht verwechseln: Das Level **Orb Run** im Studio ist ein **anderes** Programm mit
> derselben Idee, gebaut aus Studio-Bausteinen. Das Beispiel orbrun macht alles selbst, sogar Fahrphysik
> und Zusammenstöße.

Was man daran lernen kann:

- **Eine Stadt aus dem Zufall.** Jeder Block bekommt per Zufall eine von vier Bebauungen, zur Mitte hin
  werden die Häuser höher. Der Zufall im Computer ist ein Rechenverfahren, das aus einer Startzahl
  eine lange, bunt gemischte Zahlenreihe macht, wie ein Kartenstapel, der bei gleicher Startzahl immer
  gleich gemischt wird. Gleiche Startzahl, gleiche Stadt. Enter wählt eine neue Startzahl und damit
  eine neue Stadt. Bäume und Verkehr sind gestempelt (Instancing).
- **Große Arbeit in Häppchen.** Der Gebäudebauer kann seine Arbeit über mehrere Bilder verteilen, ein
  Stück pro Bild, damit das Spiel beim Bauen nicht stockt. orbrun baut seine Stadt beim Start trotzdem
  in einem Rutsch.
- **Zusammenstöße ohne Physik-Engine.** Das Auto ist für die Rechnung ein Kreis, jedes Haus ein
  Vieleck. Steckt der Kreis in einer Wand, schiebt das Spiel ihn hinaus und lässt ihn abprallen. Für
  ein Rennspiel reicht das völlig.
- **Nicht durch Wände „tunneln“.** Ein schnelles Auto springt von Bild zu Bild ein ganzes Stück weiter.
  Ist der Sprung größer als eine Wand dick ist, steht das Auto im einen Bild vor der Wand und im
  nächsten dahinter, und die Wand hat es nie „berührt“. Darum zerlegt orbrun jede Bewegung in kleine
  Teilschritte von höchstens 60 cm und prüft jeden davon.
- **Verkehr mit Rücksicht.** Die anderen Autos halten an, wenn du vor ihnen stehst, und wer am
  Stadtrand hinausfährt, taucht auf der anderen Seite wieder auf.
- **Spielgefühl statt echter Physik.** Das Auto rutscht in Kurven kurz und fängt sich, Lenken wirkt erst
  mit Tempo, und die Kamera weicht mit dem Tempo zurück. Das ist keine genaue Physik, sondern so
  gemacht, dass es sich gut anfühlt.
- **Tag und Nacht.** Sonne, Mond, Himmel und Nebel werden mit der Tageszeit gemischt. Nachts leuchten
  Scheinwerfer und Orbs stärker. Jedes davon ist ein **Punktlicht**, eine Lampe, die wie eine
  Glühbirne in alle Richtungen strahlt. Das Einfärbe-Programm, das Kern für alle Dinge benutzt (der
  Standard-Shader), rechnet mit höchstens sechs Punktlichtern gleichzeitig. Darum wählt das Spiel in
  jedem Bild die sechs wichtigsten aus.

Das Glanzstück ist eine **Shader-Erweiterung**, die nachts Fenster leuchten lässt. Der Gebäudebauer
gibt jeder Wand ein paar Zusatzinformationen mit: wie lang und hoch sie ist, wie hoch ein Stockwerk
ist, und eine Zufallszahl. Das kleine Programm auf der Grafikkarte macht daraus beim Einfärben ein
Raster aus Fenstern, von denen nachts zufällig manche hell sind. Das Besondere: Das Spiel hat dem
Standard-Shader damit einen eigenen Look hinzugefügt, **ohne die Engine selbst zu ändern**. In der
Ferne werden die Fenster kleiner als ein Bildschirmpunkt. Dann würde das Raster flackern wie ein feines
Karomuster im Fernsehen, darum verschwimmt es dort zu einem sanften Schimmer. Wie solche
Erweiterungen funktionieren, steht in [Kapitel 4](04-grafik.md).

> **Probier's aus!** Öffne im Studio **Datei › Beispiel-Levels › Orb Run** (File › Example Levels › Orb Run)
> und zieh in den **Welteinstellungen** (World Settings) an **Tageszeit** (Time of day). Auch hier
> gehen nachts die Fenster an. Dieselbe Idee, nur im Studio-Level.

> **Für Neugierige:** Die Zusatzwerte der Wand reisen „flach“ über die ganze Wand, also unverändert
> statt sanft gemischt. Ein Mischen würde die Zufallszahl verwischen, und das Fenstermuster zerfiele.
> Alle Zahlen zu Stadt, Fahrphysik und Verkehr stehen im blauen Buch, Kapitel 12.

---

## 12.3 Der Browser-Helfer

Ein C++-Spiel will sich nicht um Browser-Kleinkram kümmern. Das übernehmen zwei kleine Helferdateien,
die Kern mitliefert und die immer zusammen nebeneinanderliegen: Die eine startet das Spiel und kümmert
sich um den Browser, die andere verbindet sie mit dem Modul. Zusammen kümmern sie sich um:

- **Größe:** Die Zeichenfläche passt sich immer dem Fenster an, in voller Bildschirmschärfe.
- **Tastatur:** Tasten zählen nach ihrer **Position**, nicht nach ihrer Beschriftung. W A S D liegt also
  auch auf einer französischen Tastatur richtig. Pfeile und Leertaste scrollen die Seite nicht mehr.
- **Maus und Touch:** Position, Tasten, Rad und Finger; ein Rechtsklick öffnet auf dem Spiel kein
  Menü des Browsers.
- **Fokus:** Wechselst du kurz in ein anderes Fenster (etwa mit Alt+Tab), lässt die Engine alle Tasten
  los. Sonst würde die Figur ewig weiterlaufen, weil das Spiel das Loslassen nie mitbekommen hat.
- **Maus fangen:** Auf Wunsch des Spiels verschwindet der Mauszeiger, und die Maus steuert nur noch den
  Blick.
- **Grafikkarte kurz weg:** Verliert der Browser einmal kurz den Zugriff auf die Grafikkarte, pausiert
  der Zeichenteil der Engine und lädt danach alles aus seinen Kopien neu.

Die genauen Namen und Optionen stehen im blauen Buch, Kapitel 12.

---

## 12.4 Weg C: aus JavaScript

Weg C ist für Leute, die schon eine eigene Webseite oder ein JavaScript-Spiel haben und Kern als
Baustein darin nutzen wollen. Es gibt zwei fertige Anschlüsse:

**Nur die Grafik von Kern nutzen.** Die Spiellogik, also die Regeln und Abläufe des Spiels, bleibt in
JavaScript, und Kern malt. So arbeitet zum
Beispiel das Drohnenspiel SpawnCity.

**Ein Studio-Level in die eigene Seite einbauen.** Deine Seite lädt ein Level aus Studio und spielt es,
genau wie ein exportiertes Spiel. Dann kann sie dem Spiel Befehle schicken, als wäre sie ein Gamepad
aus Programmtext, und nachfragen, wie es steht. Das sieht so aus:

```js
game.command({cmd: "gameInput", moveY: 1, jump: true});
game.command({cmd: "gameInput", moveY: 0, jump: false});
if (game.status.hud?.status === "won") showMyWinScreen();
```

Zeile für Zeile: Die erste Zeile sagt dem Spiel „lauf vorwärts (1 heißt voll vorwärts) und spring“. Die
zweite sagt „hör wieder auf“. Die dritte fragt die Anzeige des Spiels: Ist der Stand „gewonnen“? Dann
zeigt die Seite ihren eigenen Siegerbildschirm (`showMyWinScreen` ist dabei eine Funktion, die du
selbst schreibst). Solche Befehle mischen sich mit Tastatur, Touch und Gamepad. Neben Laufen und
Springen gibt es zum Beispiel Boost, Feuer, Ducken und Umschauen.

**Logik-Graphen als C++.** Im Logik-Editor lädt der Knopf **C++** deinen Graphen als lesbaren
Programmtext herunter. Ein Programmierer kann ihn fest in Kern einbauen, dann läuft er am schnellsten.
Dein Graph kann also auf drei Arten laufen, die sich nur im Tempo unterscheiden, nie im Ergebnis. Das
ist wie ein Text, den man Wort für Wort vorliest, als vorab übersetzte Fassung oder als fest gedrucktes
Buch hat: Der Inhalt ist derselbe ([Kapitel 9](09-logik.md)).

Alle Befehle und Namen stehen im blauen Buch, Kapitel 12, und in [Kapitel 17](17-referenz.md).

---

## 12.5 Hosten: Warum C++-Spiele einen Webserver brauchen

Ein selbst geschriebenes C++-Spiel besteht aus ein paar Dateien: der Webseite, dem Modul und den
beiden Helferdateien von Kern aus 12.3. Beim Start lädt die Webseite das Modul **nach**. Und genau das erlauben
Browser einer Seite, die du einfach von der Festplatte öffnest, aus Sicherheitsgründen nicht: Sonst
könnte jede heruntergeladene Datei in deinen anderen Dateien stöbern. Darum braucht ein C++-Spiel
einen **Webserver**, also ein Programm, das die Dateien so ausliefert, wie es eine echte Internetseite
tut. Für den eigenen Rechner genügt ein ganz einfacher; im Netz ist es deine Homepage. Welche Datei
wo liegen muss, steht im blauen Buch, Kapitel 12.

> **Wusstest du?** Ein exportiertes **Studio**-Spiel braucht das alles nicht. Es lädt nichts nach,
> weil alles schon in der einen Datei steckt. Darum läuft es auch direkt von der Festplatte.

---

## 12.6 Veröffentlichen

### Die Rechte

Kern ist **Open Source**: Der Programmtext ist offen, und jeder darf ihn nach den Regeln seiner Lizenz
benutzen. Kerns Lizenz heißt **Apache License 2.0** und ist großzügig. Deine Spiele und deine Netze
(trainierte neuronale Netze, die zum Beispiel Bewegungen steuern, mehr in
[Kapitel 13](13-micro-duck.md)) darfst du benutzen, weitergeben und **verkaufen**, auch ohne deinen eigenen Programmtext offenzulegen. Weil dein
exportiertes Spiel die Engine enthält, legst du beim Weitergeben den Lizenztext und die
Hinweisdatei (NOTICE, eine kurze Liste, wer woran mitgewirkt hat) bei. Im exportierten Spiel genügt
ein Hinweis mit Link. Die Namen „Kern“ und „Kern Studio“ und das Logo gehören nicht zur Lizenz: „Made
with Kern“ darfst du schreiben, dein eigenes Produkt „Kern“ nennen nicht. Teile von anderen behalten
ihre eigenen Lizenzen ([Anhang D](D-lizenzen.md)).

> **Achtung, Falle!** Die **Micro Duck** ist eine Ausnahme. Sie ist ein kleiner Laufroboter von
> Pollen Robotics, einer Firma, die den echten Roboter baut. Das Studio und jedes exportierte Spiel
> enthalten Pollens 3D-Modelle der Ente, und die dürfen nur **nicht kommerziell** genutzt werden. Wer
> verkaufen will, braucht eine Fassung ohne diese Modelle; die baut ein Programmierer mit einem
> eigenen Schalter (blaues Buch, Kapitel 12). Dann fehlt das Aussehen, und die Ente ist unsichtbar.
> Pollens Netze, also das, was die Ente laufen lässt, dürfen dagegen mit ihrer Lizenz mit. Wer ein
> Spiel mit der Ente kostenlos weitergibt, lässt den Hinweis auf Pollen Robotics und die Lizenz in der
> Datei stehen und nennt Pollen Robotics, wenn er Bilder der Ente zeigt.

### Studio-Spiele

Die HTML-Datei lädst du auf deine Homepage oder verschickst sie einfach. Sie läuft ohne Internet, im
Vollbild, mit Tastatur, Maus, Gamepad und Touch.

- **Mit Micro Duck:** Das ist die Ausnahme aus Schritt 10. Die Bewegungen der Ente sind eigene
  Dateien, die das Spiel erst nachlädt; nur das Laufen steckt in der HTML-Datei. Leg darum die beiden
  Ordner mit diesen Dateien, die Kern mitliefert (microduck und policies), neben die HTML-Datei, sonst
  kann die Ente nur laufen.
  Zu Pollens Ordner gehören seine Lizenz und NOTICE. Warum die Netze nachgeladen werden, erklärt
  [Kapitel 13](13-micro-duck.md).
- **Online spielen:** Einer eröffnet das Spiel, die anderen treten bei. Ohne Hilfe finden sich so nur
  Tabs im selben Browser. Für Spieler an verschiedenen Rechnern braucht es einen Vermittler im Netz,
  ein kleines Programm auf einem Server, über das alle verbunden sind ([Kapitel 10](10-netzwerk.md)).

### C++-Spiele

Hochladen, was 12.5 beschreibt, also Seite, Modul und die beiden Helferdateien, in derselben
Anordnung. Kern selbst wird genau so veröffentlicht: Ein automatischer Ablauf baut Studio und die
Beispiele und stellt alles auf https://kernengine.de/.

### Prüfliste

| Punkt | Studio-Spiel | C++-Spiel |
|---|---|---|
| Fehler finden | **Ausgabeprotokoll** (Output Log), **Spielen** (Play) | Fehleranzeige des Browsers |
| Telefon | Sticks kommen von selbst | Touch selbst programmieren |
| Ton | startet mit dem ersten Tipp | Sache des Spiels |
| Dateien | eine HTML (plus Ente-Ordner) | Seite, Modul, zwei Helfer |
| Auslieferung | überall, auch von der Platte | nur über einen Webserver |

Der erste Tipp startet den Ton übrigens, weil Browser Seiten erst Geräusche machen lassen, wenn du
einmal geklickt oder getippt hast.

---

## Das nimmst du mit

- Drei Wege zum Spiel: **Studio ohne Code**, **C++** und **JavaScript**, alle auf derselben Engine.
- Ein Studio-Spiel entsteht in zehn Schritten: Level, Figur, Hindernisse, Pickups, Gefahr, Logik, Ton,
  Testen, Speichern, Exportieren. **Spiel verpacken** (Package Game) macht daraus eine Datei, die überall läuft.
- **Nur Logik-Ereignis** (Logic event only) macht einen Trigger zum reinen Melder für deine Logik; **Compile** prüft die Logik
  vor dem Spielen.
- Jedes Spiel ist eine **Spielschleife**: einmal vorbereiten, dann in jedem Frame Eingabe lesen, Welt
  weiterrechnen, malen.
- **Bildraten-unabhängig:** Wer mit der vergangenen Zeit rechnet, ist auf jedem Bildschirm gleich schnell.
- **Instancing** ist ein Stempel: ein Ding, viele Abdrücke, ein Zeichenbefehl.
- Schnelle Dinge bewegen sich in **Teilschritten**, damit sie nicht durch Wände tunneln.
- Ein **Shader** ist ein Programm auf der Grafikkarte; eine Erweiterung gibt dem Spiel einen eigenen Look.
- Du darfst deine Spiele verkaufen, nur das Aussehen der Micro Duck ist nicht kommerziell.

## Knobelecke

1. Warum startest du dein Spiel mit der Vorlage **Grundformen** (Basic) und nicht mit **Leer** (Empty)?
2. Dein Würfel aus **Formen** (Shapes) fällt nicht herunter. Was fehlt?
3. Lena spielt hello auf einem Bildschirm mit 120 Bildern pro Sekunde, Tom auf einem mit 60. Beide
   halten eine Sekunde lang die Pfeiltaste. Wessen Kamera hat sich weiter gedreht?
4. Dein selbst geschriebenes C++-Spiel startet nicht, wenn du die Seite doppelklickst. Dein
   exportiertes Studio-Spiel schon. Woran liegt der Unterschied?
5. Du willst ein Spiel mit Micro Duck verkaufen. Was musst du beachten?

### Lösungen

1. **Leer** hat keinen Boden; die Figur würde ins Bodenlose fallen.
2. Physik. Ein Würfel aus **Formen** ist ein festes Hindernis; erst mit Physik kann er fallen (oder du
   nimmst gleich einen **Physik-Würfel** (Physics Cube)).
3. Keine, beide gleich weit. Lenas Spiel dreht pro Bild nur halb so weit, macht aber doppelt so viele
   Bilder, denn das Spiel rechnet mit der vergangenen Zeit.
4. Das C++-Spiel lädt sein Modul nach, und das erlauben Browser einer Seite von der Festplatte nicht;
   es braucht einen Webserver. Im Studio-Spiel steckt alles in der einen Datei, es lädt nichts nach.
5. Pollens 3D-Modelle der Ente sind nur nicht kommerziell nutzbar. Eine Fassung ohne die Modelle lässt
   sie weg, die Ente ist dann unsichtbar.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
