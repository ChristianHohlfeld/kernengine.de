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
gewonnen wird. Das ist wie Lego mit Regeln. Wir bauen „Orb Garden“: Eine Figur läuft über eine Fläche
mit Hindernissen, sammelt leuchtende Kugeln (**Orbs**) ein, weicht einer Gefahrenzone aus und gewinnt,
wenn alles eingesammelt ist. Die Bedienung der Panels kennst du aus [Kapitel 11](11-studio.md). Los
geht's!

### Schritt 1: Ein neues Level

Öffne Kern Studio über https://kernengine.de/, dann **File › New Level...**, Vorlage **Basic**,
**Create**. Du hast einen Boden von 40 × 40 m, Sonne, Himmel und einen Würfel. Trag in **World
Settings** den **Level name** und den **Game title** „Orb Garden“ ein.

> **Achtung, Falle!** Warum nicht **Empty**? Weil Empty wirklich **nichts** hat, auch keinen Boden.
> Deine Figur würde ins Bodenlose fallen.

### Schritt 2: Die Spielfigur

Öffne den Content Drawer (**Ctrl+Space**) und zieh aus **Players** einen **Character** in den Viewport.
Das Log meldet „Character is the player now“, und im Outliner trägt er das Abzeichen **PLAYER**. In
**Details › Player Controller** stellst du Tempo (**Max speed**), Beschleunigung (**Acceleration**),
Sprungkraft (**Jump speed**) und die Kamera ein: von hinten (**Chase**), aus den Augen der Figur
(**First person**) oder von oben (**Top down**).

> **Probier's aus!** Drück sofort **Play** (Alt+P). W A S D läuft, die Maus schaut, die Leertaste
> springt, **Esc** bringt dich zurück. Tausch zum Spaß den Character gegen einen **Rolling Ball** oder
> eine **Drone** aus **Players**: Der zuletzt platzierte ist der Spieler, oder du wählst einen mit
> Rechtsklick › **Make Player**.

### Schritt 3: Hindernisse und Physik

1. Aus **Shapes** ein paar **Cube**, eine **Ramp** und eine **Plane** setzen. Mit **W**, **E**, **R** und
   dem Gizmo zurechtrücken; das 0,5-m-Raster hält die Kanten sauber, **End** setzt auf den Boden.
2. Aus **Physics** einen **Crate Stack** und eine **Hinged Door**: Die reagieren auf deine Figur.
3. Aus **Gameplay** eine **Moving Platform** und einen **Spinner**, die sich von selbst bewegen.
4. Mit **Simulate** (Alt+S) prüfen. Du kannst Körper greifen und werfen, und **K** übernimmt die
   simulierte Lage, etwa für einen Kistenstapel, der natürlich eingestürzt aussehen soll.

Ein **Cube** aus Shapes ist ein festes Hindernis: Man stößt dagegen, aber er bewegt sich nicht. Soll er
fallen, gib ihm in Details mit **+ Add › Rigid Body** und **Mobility = Dynamic** Physik, oder nimm
gleich eine **Physics Cube** ([Kapitel 6](06-physik.md)).

### Schritt 4: Etwas zum Einsammeln

1. Aus **Gameplay** mehrere **Orb** (1 Punkt) und einen **Star** (10 Punkte) verteilen. Trick: **Alt +
   Gizmo ziehen** zieht bei jedem Zug eine Kopie heraus.
2. Alle Orbs auswählen und mit dem Ordner-Knopf im Outliner in einen Ordner legen.
3. In **World Settings › Win condition** **Collect all pickups** wählen.

### Schritt 5: Gefahr, Checkpoint und Zeitlimit

1. Einen **Hazard** setzen und mit Scale auf Größe ziehen: Wer hineinläuft, hat das Spiel verloren,
   da hilft auch kein Checkpoint.
2. Einen **Checkpoint** setzen. Hier startet die Figur neu, wenn sie zu tief fällt. Wie tief „zu tief“
   ist, legt **Kill Z** in World Settings fest: eine Höhe, unter der das Spiel die Figur zurückholt.
3. Wer mag, setzt in World Settings ein **Time limit**: Das HUD zählt herunter.

> **Achtung, Falle!** Statt eines Hazards gibt es auch eine **Damage Zone**, die Gesundheit kostet.
> Sie wirkt aber nur auf Figuren, die überhaupt Gesundheit haben. Der Character hat von Haus aus
> keine; gib sie ihm in Details mit **+ Add › Health**, sonst läuft er unbeschadet durch die Zone.

Andere Siegbedingungen: **Reach a goal trigger** (mit einem **Goal**), **Survive the time limit** oder
**Reach a target score**.

### Schritt 6: Logik ohne Programmieren

Bis hierher hat jeder Baustein sein eingebautes Verhalten. Willst du eigenes Verhalten, baust du es
aus **Logik-Graphen**: Kästchen, die du mit Drähten verbindest ([Kapitel 9](09-logik.md)). Wir machen
eine Zone, die Punkte gibt:

1. Einen **Message Trigger** setzen und in Details › Trigger die **Action** auf **Logic event only**
   stellen.
2. In Details auf **Logic** klicken. Der Logik-Editor öffnet den Graphen dieses Triggers.
3. **Templates › Score zone (overlap +10)**: Wer die Zone betritt, bekommt 10 Punkte, eine Meldung „+10“
   und einen Ton.
4. **Compile** (F7), dann **Viewport**.

Warum **Logic event only**? Normalerweise tut ein Trigger selbst etwas, sobald jemand hineinläuft: Er
lässt gewinnen, verlieren oder zeigt eine Meldung mit Ton. Mit „Logic event only“ tut er von sich aus
nichts, sondern sagt nur deiner Logik Bescheid: „Da ist jemand drin.“ So entscheidet allein dein
Graph, was passiert, und nichts passiert doppelt. Wie ein Türsensor, der nicht selbst klingelt,
sondern nur meldet: Was dann geschieht, bestimmst du.

Warum **Compile**? Compile prüft deinen Graphen auf Fehler, etwa einen Draht ins Leere, und zeigt eine
Ampel: OK, Warnung oder Fehler. Das ist wichtig, denn ein Graph mit Fehlern wird beim Spielstart
abgeschaltet. Das Spiel läuft dann zwar weiter, aber deine Zone gibt keine Punkte, und du wunderst
dich. Erst prüfen, dann spielen.

Weitere Vorlagen: eine Tür, die auf **Interact** (Taste F) aufgeht, ein Spawner, eine Ampel und ein
Verhalten für die Micro Duck.

### Schritt 7: Ton

In **World Settings › Audio** eine **Music** und eine **Ambience** (Hintergrundklang) wählen. Aus der
Gruppe **Audio** etwa **Ambient Wind** setzen, oder im Tab **Sounds** eine eigene Datei importieren
und in den Viewport ziehen. Figuren, Pickups und Trigger bringen ihre Geräusche schon mit
([Kapitel 7](07-audio.md)).

### Schritt 8: Testen

**Play** und spielen! **F8** lässt die Figur los, die Welt läuft weiter, du schaust dich um; noch einmal
F8 übernimmt sie. Mit **Play › Number of Players › 2 Players** testest du zu zweit. Fehler im Level
(etwa ein Gelenk, das auf einen gelöschten Actor zeigt) verhindern Play und stehen im **Output Log**.

### Schritt 9: Speichern

**Ctrl+S** speichert eine `.klevel`-Datei. Sie enthält alles, auch Texturen und importierte Sounds. Du
kannst sie weitergeben und in jedem Studio öffnen (Ctrl+O oder auf die Seite ziehen). Ungespeicherte
Arbeit sichert Studio zusätzlich im Browser und bietet sie beim nächsten Start an.

### Schritt 10: Exportieren

**File › Package Game (HTML)...**, Titel prüfen, **Package**. Studio lädt `Orb_Garden.html` herunter
und bietet **Play it** an. Diese eine Datei enthält Engine, Level und Spiel. Sie läuft ohne Internet,
direkt von der Festplatte, von jeder Homepage und am Telefon mit Touch-Sticks.

> **Probier's aus!** Bring `Orb_Garden.html` aufs Telefon: Am zuverlässigsten lädst du sie auf deine
> Homepage und öffnest die Adresse im Browser des Telefons. Du kannst sie dir auch schicken und mit dem
> Browser des Telefons öffnen. Die Engine erkennt den Touchscreen und zeichnet Sticks und Knöpfe von
> selbst. Du hast gerade ein Handyspiel gebaut!

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
vergangen** ist. Programmierer nennen das **dt** (für „Delta Zeit“). Das Spiel rechnet: Strecke =
Tempo mal vergangene Zeit.

Ein Beispiel: Die Kamera soll sich 1,5 Einheiten pro Sekunde drehen.

| Bildschirm | Zeit pro Bild | Drehung pro Bild | Drehung pro Sekunde |
|---|---|---|---|
| 60 Bilder/s | 1/60 s | 0,025 | 60 · 0,025 = 1,5 |
| 120 Bilder/s | 1/120 s | 0,0125 | 120 · 0,0125 = 1,5 |

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
  werden die Häuser höher. Der Zufall kommt aus einer Startzahl: Gleiche Startzahl, gleiche Stadt.
  Enter wählt eine neue Startzahl und damit eine neue Stadt. Bäume und Verkehr sind gestempelt
  (Instancing).
- **Zusammenstöße ohne Physik-Engine.** Das Auto ist für die Rechnung ein Kreis, jedes Haus ein
  Vieleck. Steckt der Kreis in einer Wand, schiebt das Spiel ihn hinaus und lässt ihn abprallen. Für
  ein Rennspiel reicht das völlig.
- **Nicht durch Wände „tunneln“.** Ein schnelles Auto springt von Bild zu Bild ein ganzes Stück weiter.
  Ist der Sprung größer als eine Wand dick ist, steht das Auto im einen Bild vor der Wand und im
  nächsten dahinter, und die Wand hat es nie „berührt“. Darum zerlegt orbrun jede Bewegung in kleine
  Teilschritte von höchstens 60 cm und prüft jeden davon.
- **Verkehr mit Rücksicht.** Die anderen Autos halten an, wenn du vor ihnen stehst, und wer am
  Stadtrand hinausfährt, taucht auf der anderen Seite wieder auf.
- **Tag und Nacht.** Sonne, Mond, Himmel und Nebel werden mit der Tageszeit gemischt. Nachts leuchten
  Scheinwerfer und Orbs stärker. Weil die Grafik nur sechs Punktlichter gleichzeitig kann, wählt das
  Spiel in jedem Bild die sechs wichtigsten aus.

Das Glanzstück ist eine **Shader-Erweiterung**, die nachts Fenster leuchten lässt. Der Gebäudebauer
gibt jeder Wand ein paar Zusatzinformationen mit: wie lang und hoch sie ist, wie hoch ein Stockwerk
ist, und eine Zufallszahl. Das kleine Programm auf der Grafikkarte macht daraus beim Einfärben ein
Raster aus Fenstern, von denen nachts zufällig manche hell sind. Das Besondere: Das Spiel hat dem
Standard-Shader damit einen eigenen Look hinzugefügt, **ohne die Engine selbst zu ändern**. Und in der
Ferne verschwimmt das Fensterraster zu einem sanften Schimmer, damit nichts flimmert. Wie solche
Erweiterungen funktionieren, steht in [Kapitel 4](04-grafik.md).

> **Probier's aus!** Öffne Orb Run im Studio und zieh in World Settings an **Time of day**. Auch hier
> gehen nachts die Fenster an. Dieselbe Idee, nur im Studio-Level.

> **Für Neugierige:** Die Zusatzwerte der Wand reisen „flach“ über die ganze Wand, also unverändert
> statt sanft gemischt. Ein Mischen würde die Zufallszahl verwischen, und das Fenstermuster zerfiele.
> Alle Zahlen zu Stadt, Fahrphysik und Verkehr stehen im blauen Buch, Kapitel 12.

---

## 12.3 Der Browser-Helfer

Ein C++-Spiel will sich nicht um Browser-Kleinkram kümmern. Das übernimmt eine kleine Helferdatei, die
Kern mitliefert. Sie startet das Spiel und kümmert sich um:

- **Größe:** Die Zeichenfläche passt sich immer dem Fenster an, in voller Bildschirmschärfe.
- **Tastatur:** Tasten zählen nach ihrer **Position**, nicht nach ihrer Beschriftung. W A S D liegt also
  auch auf einer französischen Tastatur richtig. Pfeile und Leertaste scrollen die Seite nicht mehr.
- **Maus und Touch:** Position, Tasten, Rad und Finger; kein Kontextmenü auf dem Spiel.
- **Fokus:** Wechselst du kurz in ein anderes Fenster, lässt die Engine alle Tasten los. Sonst würde die
  Figur nach Alt+Tab ewig weiterlaufen, weil das Spiel das Loslassen nie mitbekommen hat.
- **Maus fangen:** Auf Wunsch des Spiels verschwindet der Mauszeiger, und die Maus steuert nur noch den
  Blick.
- **Grafikkarte kurz weg:** Verliert der Browser einmal kurz den Zugriff auf die Grafikkarte, pausiert
  der Renderer und lädt danach alles aus seinen Kopien neu.

Die genauen Namen und Optionen stehen im blauen Buch, Kapitel 12.

---

## 12.4 Weg C: aus JavaScript

Weg C ist für Leute, die schon eine eigene Webseite oder ein JavaScript-Spiel haben und Kern als
Baustein darin nutzen wollen. Es gibt zwei fertige Anschlüsse:

**Nur die Grafik von Kern nutzen.** Die Spiellogik bleibt in JavaScript, Kern malt. So arbeitet zum
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
Programmtext herunter. Ein Programmierer kann ihn fest in Kern einbauen. Dann läuft der Graph als
schnellster übersetzter Code. Wichtig: Ob dein Graph so läuft, im Browser übersetzt wird oder Schritt
für Schritt abgearbeitet wird, das Ergebnis ist immer genau dasselbe, nur die Geschwindigkeit
unterscheidet sich ([Kapitel 9](09-logik.md)).

Alle Befehle und Namen stehen im blauen Buch, Kapitel 12, und in [Kapitel 17](17-referenz.md).

---

## 12.5 Hosten: Warum C++-Spiele einen Webserver brauchen

Ein selbst geschriebenes C++-Spiel besteht aus ein paar Dateien: der Webseite, dem Modul und den
beiden Helferdateien von Kern. Beim Start lädt die Webseite das Modul **nach**. Und genau das erlauben
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
benutzen. Kerns Lizenz heißt **Apache License 2.0** und ist großzügig. Deine Spiele und Netze darfst du
benutzen, weitergeben und **verkaufen**, auch ohne deinen eigenen Programmtext offenzulegen. Weil dein
exportiertes Spiel die Engine enthält, legst du beim Weitergeben den Lizenztext und die
Hinweisdatei (NOTICE, eine kurze Liste, wer woran mitgewirkt hat) bei. Im exportierten Spiel genügt
ein Hinweis mit Link. Die Namen „Kern“ und „Kern Studio“ und das Logo gehören nicht zur Lizenz: „Made
with Kern“ darfst du schreiben, dein eigenes Produkt „Kern“ nennen nicht. Teile von anderen behalten
ihre eigenen Lizenzen ([Anhang D](D-lizenzen.md)).

> **Achtung, Falle!** Die **Micro Duck** ist eine Ausnahme. Das Studio und jedes exportierte Spiel
> enthalten Pollens 3D-Modelle der Ente, und die dürfen nur **nicht kommerziell** genutzt werden. Wer
> verkaufen will, braucht eine Fassung ohne diese Modelle; die baut ein Programmierer mit einem
> eigenen Schalter (blaues Buch, Kapitel 12). Dann fehlt das Aussehen, und die Ente ist unsichtbar.
> Pollens Netze, also das, was die Ente laufen lässt, dürfen dagegen mit ihrer Lizenz mit.

### Studio-Spiele

Die HTML-Datei lädst du auf deine Homepage oder verschickst sie einfach. Sie läuft ohne Internet, im
Vollbild, mit Tastatur, Maus, Gamepad und Touch.

- **Mit Micro Duck:** Die nachladbaren Netze der Ente stecken nicht in der Datei. Leg die beiden
  Netzordner von Kern (microduck und policies) neben die HTML-Datei, sonst kann die Ente nur laufen.
  Zu Pollens Ordner gehören seine Lizenz und NOTICE. Warum die Netze nachgeladen werden, erklärt
  [Kapitel 13](13-micro-duck.md).
- **Online spielen:** Wie mehrere Leute über das Internet gemeinsam spielen, steht in
  [Kapitel 10](10-netzwerk.md).

### C++-Spiele

Hochladen, was 12.5 beschreibt, also Seite, Modul und die beiden Helferdateien, in derselben
Anordnung. Kern selbst wird genau so veröffentlicht: Ein automatischer Ablauf baut Studio und die
Beispiele und stellt alles auf https://kernengine.de/.

### Prüfliste

| Punkt | Studio-Spiel | C++-Spiel |
|---|---|---|
| Fehler finden | Output Log, Play | Fehleranzeige des Browsers |
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
  Testen, Speichern, Exportieren. **Package Game** macht daraus eine Datei, die überall läuft.
- „Logic event only“ macht einen Trigger zum reinen Melder für deine Logik; **Compile** prüft die Logik
  vor dem Spielen.
- Jedes Spiel ist eine **Spielschleife**: einmal vorbereiten, dann in jedem Frame Eingabe lesen, Welt
  weiterrechnen, malen.
- **Bildraten-unabhängig:** Wer mit der vergangenen Zeit rechnet, ist auf jedem Bildschirm gleich schnell.
- **Instancing** ist ein Stempel: ein Ding, viele Abdrücke, ein Zeichenbefehl.
- Schnelle Dinge bewegen sich in **Teilschritten**, damit sie nicht durch Wände tunneln.
- Ein **Shader** ist ein Programm auf der Grafikkarte; eine Erweiterung gibt dem Spiel einen eigenen Look.
- Du darfst deine Spiele verkaufen, nur das Aussehen der Micro Duck ist nicht kommerziell.

## Knobelecke

1. Warum startest du dein Spiel mit **Basic** und nicht mit **Empty**?
2. Dein Würfel aus **Shapes** fällt nicht herunter. Was fehlt?
3. Lena spielt hello auf einem Bildschirm mit 120 Bildern pro Sekunde, Tom auf einem mit 60. Beide
   halten eine Sekunde lang die Pfeiltaste. Wessen Kamera hat sich weiter gedreht?
4. Dein selbst geschriebenes C++-Spiel startet nicht, wenn du die Seite doppelklickst. Dein
   exportiertes Studio-Spiel schon. Woran liegt der Unterschied?
5. Du willst ein Spiel mit Micro Duck verkaufen. Was musst du beachten?

### Lösungen

1. Empty hat keinen Boden; die Figur würde ins Bodenlose fallen.
2. Physik: ein **Rigid Body** mit **Mobility = Dynamic** (oder gleich eine **Physics Cube** nehmen).
3. Keine, beide gleich weit. Lenas Spiel dreht pro Bild nur halb so weit, macht aber doppelt so viele
   Bilder, denn das Spiel rechnet mit der vergangenen Zeit.
4. Das C++-Spiel lädt sein Modul nach, und das erlauben Browser einer Seite von der Festplatte nicht;
   es braucht einen Webserver. Im Studio-Spiel steckt alles in der einen Datei, es lädt nichts nach.
5. Pollens 3D-Modelle der Ente sind nur nicht kommerziell nutzbar. Eine Fassung ohne die Modelle lässt
   sie weg, die Ente ist dann unsichtbar.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
