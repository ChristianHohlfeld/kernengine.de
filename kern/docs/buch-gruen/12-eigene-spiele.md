# 12 Dein eigenes Spiel

> **Kurz gesagt:** Jetzt bist du dran! Kern bietet drei Wege zum eigenen Spiel: im Studio ganz ohne
> Code, in C++ mit der App-API oder aus JavaScript. Alle drei nutzen dieselbe Engine mit demselben
> Renderer und derselben Physik. Wir bauen zuerst Schritt für Schritt ein kleines Sammelspiel im Studio
> und schauen dann, wie Profis Kern von Code aus steuern.

| Weg | Werkzeug | Ergebnis | Ideal für |
|---|---|---|---|
| **A** ohne Code | Kern Studio, Logik-Graphen | eine HTML-Datei | Levels, Prototypen, Unterricht |
| **B** in C++ | die App-API | ein `.wasm` und eine Seite | eigene Simulation, eigene Shader |
| **C** aus JavaScript | die C-ABI des Moduls | deine eigene Seite | bestehende JS-Projekte |

Die Wege schließen sich nicht aus. Es gibt sogar eine Mischform: Logik-Graphen aus Studio lassen sich
als C++ exportieren (12.4).

---

## 12.1 Weg A: ein Spiel im Studio, ohne Code

Wir bauen „Orb Garden“: Eine Figur läuft über eine Fläche mit Hindernissen, sammelt Orbs, weicht einer
Gefahrenzone aus und gewinnt, wenn alles eingesammelt ist. Die Bedienung der Panels kennst du aus
[Kapitel 11](11-studio.md). Los geht's!

### Schritt 1: Ein neues Level

Öffne Kern Studio (über https://kernengine.de/ oder offline als `generated/studio.html`), dann **File › New Level...**, Vorlage **Basic**,
**Create**. Du hast einen Boden (40 × 40 m), Sonne, Himmel und einen Würfel. Trag in **World Settings**
den **Level name** und den **Game title** „Orb Garden“ ein.

> **Achtung, Falle!** Warum nicht **Empty**? Weil Empty wirklich **nichts** hat, auch keinen Boden. Deine
> Figur würde ins Bodenlose fallen. (World Settings hat zwar einen **Endless ground**, aber ein
> sichtbarer Boden mit Farbe ist für den Anfang anschaulicher.)

### Schritt 2: Die Spielfigur

Öffne den Content Drawer (**Ctrl+Space**) und zieh aus **Players** einen **Character** in den Viewport.
Das Log meldet „Character is the player now“, und im Outliner trägt er das Abzeichen **PLAYER**. In
**Details › Player Controller** stellst du Tempo (**Max speed**), **Acceleration**, **Jump speed** und
die Kamera ein (**Chase**, **First person**, **Top down**).

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

Ein **Cube** aus Shapes hat Kollision, bewegt sich aber nicht. Soll er fallen, gib ihm in Details mit
**+ Add › Rigid Body** und **Mobility = Dynamic** Physik, oder nimm gleich eine **Physics Cube**
([Kapitel 6](06-physik.md)).

### Schritt 4: Etwas zum Einsammeln

1. Aus **Gameplay** mehrere **Orb** (1 Punkt) und einen **Star** (10 Punkte) verteilen. Trick: **Alt +
   Gizmo ziehen** zieht bei jedem Zug eine Kopie heraus.
2. Alle Orbs auswählen und mit dem Ordner-Knopf im Outliner in einen Ordner legen.
3. In **World Settings › Win condition** **Collect all pickups** wählen.

### Schritt 5: Gefahr, Checkpoint und Zeitlimit

1. Einen **Hazard** (verliert das Spiel) oder eine **Damage Zone** setzen und mit Scale auf Größe ziehen.
2. Einen **Checkpoint** setzen: Fällt die Figur unter **Kill Z** oder verliert sie, startet sie hier neu.
3. Wer mag, setzt in World Settings ein **Time limit**: Das HUD zählt herunter.

Andere Siegbedingungen: **Reach a goal trigger** (mit einem **Goal**), **Survive the time limit** oder
**Reach a target score**.

### Schritt 6: Logik ohne Programmieren

1. Einen **Message Trigger** setzen und in Details › Trigger die **Action** auf **Logic event only**
   stellen.
2. In Details auf **Logic** klicken.
3. **Templates › Score zone (overlap +10)**: Wer die Zone betritt, bekommt 10 Punkte, eine Meldung „+10“
   und einen Ton.
4. **Compile** (F7), dann **Viewport**.

Weitere Vorlagen: eine Tür, die auf **Interact** (Taste F) aufgeht, ein Spawner, eine Ampel und ein
Verhalten für die Micro Duck. Alle Knoten erklärt [Kapitel 9](09-logik.md).

### Schritt 7: Ton

In **World Settings › Audio** eine **Music** und eine **Ambience** wählen. Aus der Gruppe **Audio**
etwa **Ambient Wind** setzen, oder im Tab **Sounds** eine eigene Datei importieren und in den Viewport
ziehen. Figuren, Pickups und Trigger bringen ihre Geräusche schon mit ([Kapitel 7](07-audio.md)).

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
und bietet **Play it** an. Diese eine Datei enthält Engine, Level und Spiel. Sie läuft offline, direkt
von der Festplatte, von jedem Webserver und am Telefon mit Touch-Sticks.

> **Probier's aus!** Schick dir `Orb_Garden.html` aufs Telefon und öffne sie. Die Engine erkennt den
> Touchscreen und zeichnet Sticks und Knöpfe von selbst. Du hast gerade ein Handyspiel gebaut!

![Dasselbe Spiel auf dem Telefon: die Engine erkennt Touch und zeichnet Sticks und Knöpfe](../buch/bilder/game-phone.png)

---

## 12.2 Weg B: ein Spiel in C++ mit der App-API

Für Spiele, die mehr wollen als Actors und Komponenten, schreibst du das Spiel selbst in C++. Dein Spiel
ist eine Klasse, die von `kern::App` erbt. Die Engine ruft sie jeden Frame auf und zeichnet danach. Um
alles Browserspezifische kümmert sich die Plattformschicht `app.mjs` (12.3).

> **Stell dir vor …** ein Theater. Die Engine ist die Bühnentechnik mit Licht, Vorhang und Kulissen.
> Deine App ist das Stück. Die Technik fragt dich jeden Augenblick: „Was passiert jetzt?“ Du sagst es
> ihr, und sie macht es sichtbar.

### Die Bausteine

Deine App hat ein paar Methoden, die die Engine zum richtigen Zeitpunkt aufruft:

- **`configure()`** einmal vorab: Schatten, Bloom und andere Renderer-Wünsche.
- **`start()`** einmal: Meshes anlegen, alles vorbereiten. Gibt sie `false` zurück, bricht alles ab.
- **`update()`** **jeden Bildschirm-Frame**: Eingabe lesen, Spiel weiterrechnen, Szene ändern.
- **`resized()`** nach Größenänderung, **`stop()`** zum Aufräumen. Dazu `extensions()` für eigene
  Shader (sie müssen so lange leben wie die App).

Dafür bekommt sie die **Engine** mit dem Renderer ([Kapitel 4](04-grafik.md)), der **Eingabe**
(Tasten, Maus, bis zu 10 Finger), `set_status` für einen Text auf der Seite, dem Wunsch nach
Maus-Fangen und Startparametern aus der Adresse (etwa `?time=0.6`). Jeder **Frame** nennt `time`
(Sekunden seit Start), `dt` (Sekunden seit dem letzten Frame, höchstens 0,1) und eine laufende Nummer.
Die Eingabe fragst du so ab: `down` (Taste gehalten), `pressed`/`released` (in diesem Frame gedrückt
bzw. losgelassen), Zeigerposition und -bewegung in Pixeln der Zeichenfläche (auch bei gefangener
Maus), Maustasten, Mausrad, Finger und ob die Seite den Fokus hat. Tasten zählen nach ihrer
**Position**, wie in Studio. Dazu liefert die Engine Größe der Zeichenfläche, die Statistik des
letzten Frames (Draw Calls, Dreiecke) und eine Löschfarbe für Stellen ohne Himmel. Alle Signaturen
stehen im blauen Buch.

### Ein Frame im Ablauf

Die Seite (`app.mjs`) startet das Modul: Startparameter übergeben, `kern_create_app()` holen,
`configure`, Renderer anlegen, `start`. Danach reicht sie Tasten, Maus, Finger und Fokus weiter. Zu
jedem Bildschirmbild ruft sie den Frame auf: erst dein `update`, dann setzt die Engine die
Nur-dieser-Frame-Werte zurück, dann zeichnet sie. Größenänderung führt zu `resized`, ein
Kontextverlust pausiert den Renderer, und `stop()` der Seite ruft dein `stop` und fährt den Renderer
herunter.

> **Achtung, Falle!** „In diesem Frame gedrückt“ (`pressed`), „losgelassen“, Mausbewegung und Mausrad
> gelten nur für **diesen einen** Frame; danach setzt die Engine sie zurück. Lies sie also in `update`
> und speichere sie nicht für später.

### Das kleinste Kern-Spiel: hello

`examples/hello` hat nur 83 Zeilen: ein Feld aus 64 sich drehenden Kisten mit Schatten, eine Kamera, die
du mit Maus oder Pfeiltasten herumkreisen lässt, und ein Klick färbt eine Kiste rot. So fängt es an:

```cpp
class Hello final : public App {
 public:
  RendererOptions configure() override {
    RendererOptions o;
    o.shadows = true;
    o.shadow.resolution = 1024;
    o.shadow.extent = 40;
    return o;
  }
```

Zeile für Zeile: `Hello` ist dein Spiel und erbt von `App`. `configure` legt den Renderer fest, bevor
es ihn gibt: Schatten an, die Schattenkarte hat 1024 Pixel Kantenlänge und deckt 40 m ab. Am Ende gibt
die Funktion die Wünsche zurück.

Ein paar weitere Ideen aus hello, in Worten:

- **Meshes anlegen:** Jedes Mesh bekommt eine Id (`allocate_id`). `upsert` legt es an oder ersetzt
  es; ist es ungültig, bleibt das alte unangetastet und `upsert` liefert `false`, also bricht `start`
  ab. Boden und Kiste kommen aus fertigen Grundformen (`primitives::plane`, `box_on_ground`).
- **Instancing:** Die 64 Kisten sind **ein** Mesh mit 64 Kopien (Instanzen), jede mit eigener Lage
  (eine 4×4-Matrix aus 16 Zahlen) und Farbe (ein „Tint“ aus 4 Zahlen; weiß lässt die orange
  Grundfarbe stehen). Ein einziger Zeichenbefehl (Draw Call) malt alle. Sie stehen in einem 8×8-Raster
  im Abstand von 3 m; jeden Frame lädt `update_instances` die neuen Matrizen hoch, Unverändertes
  erkennt der Renderer und lässt es weg.
- **Bildraten-unabhängig:** Die Pfeiltasten drehen die Kamera um `dt` × 1,5. Auf einem schnellen
  Bildschirm gibt es mehr, aber kleinere Schritte; die Drehung pro Sekunde bleibt gleich. Gehaltene
  linke Maustaste dreht ebenfalls, das Mausrad ändert den Abstand (8 bis 80 m). Die Kamera ist einfach
  ein Feld des Renderers mit Augpunkt und Ziel.
- **Picking:** Ein Klick schießt einen Strahl durch den Pixel. Trifft er eine Kiste, verrät der
  Renderer, welche Instanz es war, und ihre Farbe wird rot.
- **Status:** `set_status` schickt einen Text wie die Zahl der Draw Calls an die Seite; die kleine
  `index.html` startet das Modul mit `runKernApp` und zeigt den Text unten links.

Ganz am Ende steht die eine Funktion, die jedes Spiel haben muss:

```cpp
App* kern_create_app() {
  static Hello app;
  return &app;
}
```

Die Laufzeit ruft sie beim Start genau einmal auf und bekommt dein Spiel. Das `static` sorgt dafür, dass
es so lange lebt wie das Modul.

### Bauen

Du brauchst das Werkzeug wasi-sdk (Release 25). Dann:

```sh
export WASI_SDK=/opt/wasi-sdk-25.0-x86_64-linux
kern/build.sh --app kern/examples/hello/hello.wasm \
  kern/examples/hello/hello.cpp
```

`--app` packt die App-Laufzeit dazu. Weitere Schalter: `--physics` für Box3D, `--game` für Levels und
Spiel-Laufzeit, `--studio` für das ganze Studio, `--lehrer` für den Lehrer. Alles, was auf `.cpp` oder `.cc`
endet, ist Quellcode deines Spiels (gern mehrere Dateien), alle anderen Argumente gehen als Schalter
an den Compiler. Umgebungsvariablen wählen etwa einen Debug-Build (`KERN_BUILD=debug`); die Liste
steht im blauen Buch.

> **Wusstest du?** Der Build merkt sich jede übersetzte Datei unter einem Fingerabdruck aus Quelle,
> Einstellungen und allen eingebundenen Dateien. Ein zweiter Build übersetzt nur, was sich geändert hat, und Studio und
> Beispiele teilen sich die übersetzte Engine.
> Das fertige Modul läuft in jedem Browser mit WebGL2, ab Safari 15 ([Kapitel 2](02-bauen-und-testen.md)).

### Ein ganzes Spiel: orbrun

`examples/orbrun` (726 Zeilen) ist ein komplettes Rennspiel, nur gegen die Engine-API: Ein Hovercar rast
durch eine Stadt und sammelt 24 Orbs, während es Nacht wird. Steuerung: W/S fahren, A/D lenken,
Leertaste Boost, N schnelle Zeit, Enter neue Stadt; am Telefon lenkt die linke Bildschirmhälfte, die
rechte boostet.

> **Achtung, Falle!** Nicht verwechseln: Das Level **Orb Run** im Studio ist ein **anderes** Programm mit
> derselben Idee, gebaut aus Actors und Komponenten. Das C++-Beispiel macht alles selbst, sogar Fahrphysik
> und Kollision.

Jeder Frame von orbrun liest die Eingabe, fährt, bewegt den Verkehr, sammelt ein, stellt Tageszeit und
Kamera nach, lädt das Veränderliche zur Grafikkarte und schreibt die Statuszeile, etwa
`ORBS 5/24 | 01:12.4 | BOOST [######----] | 87 km/h`. `?time=0.6` in der Adresse startet nachts.
Was darin steckt:

- **Die Stadt:** 7 × 7 Blöcke im Abstand von 80 m, 16 m breite Straßen, jeder Block per Zufall (aus einem
  Startwert) mit einer von vier Bebauungen: ein großes Haus (oft mit Innenhof), vier Parzellen, ein
  L-förmiges Haus oder drei Riegel; zur Mitte hin werden die Häuser höher. Bäume sind instanziert. Die Gebäude- und Straßenbauer der Engine
  erzeugen die Meshes ([Kapitel 5](05-szene-und-geometrie.md)).
- **Kollision ohne Physik-Engine:** Jedes Gebäude ist ein Vieleck, das Auto ein Kreis von 1,7 m Radius,
  der aus den Wänden der Nachbarzellen (40-m-Gitter) geschoben wird und abprallt; der Stadtrand ist
  eine feste Grenze.
- **Fahrgefühl:** höchstens 36 m/s, mit Boost 60 m/s. Gas beschleunigt mit 24 m/s², rückwärts mit 21,
  Boost mit zusätzlich 42. Seitliches Rutschen klingt schnell ab, Lenken wirkt erst mit Tempo (voll ab
  9 m/s) und kehrt sich rückwärts um. Boost leert sich mit 0,45 pro Sekunde und lädt mit 0,12 nach. Damit das Auto beim Boost nicht durch Wände „tunnelt“, bewegt es sich in
  Teilschritten von höchstens 0,6 m.
- **Verkehr:** 22 Autos mit 9 bis 15 m/s, 2,8 m neben der Straßenmitte; steht der Spieler weniger als
  10 m vor einem, hält es an. Am Stadtrand tauchen sie gegenüber wieder auf.
- **Orbs:** Näher als 3,2 m, und der Orb sprüht 28 Funken aus dem Partikelsystem der Engine (ein
  Draw Call). Mit dem letzten endet der Lauf mit Bestzeit.
- **Tag und Nacht:** Ein Tag dauert 480 s, mit N nur 40 s. Sonne, Mond, Himmel und Nebel werden
  passend gemischt. Scheinwerfer und Orbs leuchten, nachts stärker; von allen Lichtwünschen gibt ein
  `LightPool` dem Renderer die sechs wichtigsten, denn der Standard-Shader kennt sechs Punktlichter.
- **Kamera:** verfolgt das Auto weich, weicht mit dem Tempo zurück, wackelt bei Stößen, öffnet das
  Sichtfeld über 30 m/s; das Mausrad ändert den Abstand.

Das Glanzstück ist die Shader-Erweiterung **NightWindows**. Ein **Shader** ist ein kleines Programm auf
der Grafikkarte, das jedes Pixel einfärbt. Der Gebäudebauer gibt jeder Wand vier Zusatzwerte mit (Länge,
Höhe, Zufallswert, Geschosshöhe). Die Erweiterung macht daraus ein Raster aus Fenstern, von denen nachts
zufällig manche leuchten, und zwar ohne die Engine anzufassen. Die Werte reisen als `flat` weiter, also
unverändert über die ganze Wand, denn ein Mischen würde das Zufallsmuster der Fenster zerstören. Ein
Regler `uNight` (einmal pro Frame gesetzt) dimmt die Fenster tagsüber. Nur Gebäude bekommen diese
Zusatzwerte, Straßen nicht. In der Ferne verschwimmt das Raster zu einem sanften Schimmer, damit nichts
flimmert. Wie solche Erweiterungen funktionieren, steht in
[Kapitel 4](04-grafik.md).

### Ein eigenes Spiel anlegen

1. Einen Ordner wie `meinspiel/` anlegen, mit `meinspiel.cpp` (eine Klasse, die von `App` erbt, plus
   `kern_create_app()`) und einer `index.html` wie bei hello.
2. Mit `kern/build.sh --app` bauen; für Box3D `--physics`, für Levels `--game` dazunehmen.
3. Den Ordner zusammen mit `kern.mjs` und `app.mjs` über einen Webserver ausliefern (12.5).

---

## 12.3 Die Plattformschicht `app.mjs`

`app.mjs` ist der Gastgeber für C++-Spiele im Browser. Er übernimmt nur, was der Browser in JavaScript
erzwingt. Eine Zeile `runKernApp({canvas, url, onStatus})` startet dein Spiel, und dann kümmert er sich
um:

- **Größe:** Die Zeichenfläche passt sich immer dem Fenster an, in voller Geräteauflösung.
- **Tastatur:** nach Tastenposition; Pfeile und Leertaste scrollen die Seite nicht mehr.
- **Maus und Touch:** Position, Tasten, Rad, Finger; kein Kontextmenü und kein Scrollen auf dem
  Spiel. Wiederholte Tastenereignisse zählen nicht.
- **Fokus:** Verliert das Fenster den Fokus, lässt die Engine alle Tasten los. Nach Alt+Tab bleibt nichts
  hängen.
- **Pointer-Lock** auf Wunsch des Spiels; ist die Seite versteckt, erfährt der Renderer es pro Frame.
- **Kontextverlust:** Verliert der Browser kurz die Grafikkarte, pausiert der Renderer und lädt danach
  alles aus seinen Kopien neu hoch.

Das Modul kann er per Adresse, Bytes oder Base64 laden; weitere Optionen (Fehler-Callback, höchste
Pixeldichte, Startparameter) stehen im blauen Buch. Zurück kommt ein Objekt mit `stop()`. Ein Modul
ohne `--app` lehnt er ab: „module was not built with --app“.

---

## 12.4 Weg C: aus JavaScript

Das Modul bietet seine Funktionen als flache **C-ABI** an: einfache Funktionen, die JavaScript direkt
aufrufen kann. Dafür gibt es zwei fertige Fassaden.

**Den Renderer aus JavaScript nutzen.** `kern.mjs` enthält `KernRenderer`. So arbeitet SpawnCity: Die
Spiellogik ist JavaScript, gezeichnet wird in C++. Nach `loadKern` legst du Meshes unter einem Namen (als Zeichenkette)
mit Zahlen-Arrays an, setzt die Kamera, renderst (zurück kommt eine Statistik) und pickst aufs Dreieck
genau. Die Fassade kopiert die Arrays in einen wiederverwendeten Bereich im Modulspeicher; im Frame
entstehen keine neuen Speicherblöcke. Pro Modul gibt es einen `KernRenderer`. Die Methoden
stehen in [Kapitel 17](17-referenz.md).

**Studio-Levels in deine eigene Seite einbetten.** `game.mjs` bietet `startGame`, denselben Spiel-Host,
den exportierte Spiele enthalten. Das Modul ist das von Studio (gebaut mit `--studio`). Du gibst
`startGame` den Text einer `.klevel`-Datei; es braucht ein Canvas und die fertigen HUD-Elemente aus
`game.mjs`, startet das Modul im Spieler-Modus, lädt bei Bedarf die Netze der Micro Duck und startet
Play. Die Beispielseite `play.html` spielt so die eingebauten Levels (`?level=orbRun`, `physicsLab`,
`shootingRange`, `microDuck`, `basic`). Zurück kommt ein Objekt (auch als `window.KernGame`) mit
`command`, `status`, der Engine, Ton und Netz. Dann
kannst du das Spiel aus deinem Code steuern, etwa mit eigenen Bildschirmknöpfen:

```js
game.command({cmd: "gameInput", moveY: 1, jump: true});
game.command({cmd: "gameInput", moveY: 0, jump: false});
if (game.status.hud?.status === "won") showMyWinScreen();
```

Die erste Zeile lässt die Figur vorwärts laufen und springen (Werte von −1 bis 1 bzw. wahr/falsch), die
zweite hört wieder auf. Die dritte fragt das HUD: Hat der Spieler gewonnen, zeigst du deinen eigenen
Siegerbildschirm. `showMyWinScreen` ist dabei deine eigene Funktion. Deine Eingaben addieren sich zu
Tastatur, Touch und Gamepad. Außer Bewegung und Sprung kennt `gameInput` hoch, Boost, Feuer, Zielen,
Nachladen, Ducken, Interact und Blick. Wer keine Fassade will, ruft die `kern_studio_*`-Funktionen
direkt auf, wie Studio selbst ([Kapitel 11](11-studio.md)).

**Logik-Graphen als C++.** Im Logik-Editor lädt der Knopf **C++** einen Graphen als lesbaren Quelltext
herunter. Er meldet sich über einen Fingerabdruck (Hash) an seinem Graphen an. Baust du ihn mit `--studio` in
ein Modul, läuft der Graph als nativer Code statt im Interpreter oder JIT. Interpreter, JIT
und C++ liefern dasselbe Ergebnis ([Kapitel 9](09-logik.md)).

---

## 12.5 Hosten

Ein C++-Spiel besteht aus wenigen Dateien: deiner `index.html`, deinem `.wasm` und `app.mjs` und
`kern.mjs`, die **nebeneinander** liegen müssen. Weil die Seite das Modul nachlädt, brauchst du einen
Webserver; als lokale Datei doppelklicken klappt nicht. Für den eigenen Rechner reicht ein beliebiger
einfacher Server im Wurzelordner des Repositorys; dann öffnest du `/kern/examples/hello/index.html`.
Die Beispiele erwarten genau diese Lage (`../../web/app.mjs`).

> **Wusstest du?** Ein exportiertes **Studio**-Spiel braucht das alles nicht. Es ist eine einzige Datei,
> die auch direkt von der Festplatte läuft.

---

## 12.6 Veröffentlichen

### Die Rechte

Kern ist Open Source unter der **Apache License 2.0**. Deine Spiele und Netze darfst du benutzen,
weitergeben und **verkaufen**, auch ohne Quellcode. Wer ein exportiertes Spiel weitergibt, legt die
Lizenz und die NOTICE bei (im HTML genügt ein Hinweis mit Link) und lässt die Urheberrechtsvermerke
stehen. Die Namen „Kern“ und „Kern Studio“ und das Logo gehören nicht zur Lizenz: „Made with Kern“ ist
erlaubt, dein eigenes Produkt „Kern“ zu nennen nicht. Bestandteile anderer behalten ihre eigenen Lizenzen
([Anhang D](D-lizenzen.md)).

> **Achtung, Falle!** Die **Micro Duck** ist eine Ausnahme. Das Studio-Modul und jedes exportierte Spiel
> enthalten Pollens 3D-Modelle der Ente, und die dürfen nur **nicht kommerziell** genutzt werden
> (CC BY-NC-SA 4.0). Wer verkaufen will, baut das Modul mit `KERN_COMMERCIAL=1 tools/build_studio.sh` (oder
> `kern/build.sh --commercial`).
> Dann fehlen die Modelle, und die Ente ist unsichtbar. Pollens Netze (Apache 2.0) dürfen mit Lizenz und
> NOTICE mit.

### Studio-Spiele

Die HTML-Datei lädst du auf irgendeinen Webspace, legst sie in ein Repository mit GitHub Pages oder
verschickst sie einfach. Sie läuft offline, im Vollbild, mit Tastatur, Maus, Gamepad und Touch.

- **Mit Micro Duck:** die Ordner `microduck/` und `policies/` daneben legen, sonst kann die Ente nur
  laufen. Zu `microduck/` gehören Pollens Lizenz und NOTICE.
- **Online:** `?host=raum` an die Adresse hängen bietet ein Spiel an, `?join=raum` tritt bei. Für
  verschiedene Rechner läuft der Relay auf einem erreichbaren Server (Port 8790, npm-Paket `ws`), und alle hängen
  `&relay=` mit dessen Adresse an ([Kapitel 10](10-netzwerk.md)).

### C++-Spiele

Hochladen, was 12.5 zeigt, mit denselben relativen Pfaden. Kern veröffentlicht sich selbst genau so auf
GitHub Pages: Die automatische Pipeline baut Studio, hello und orbrun und kopiert alles samt den
Netzordnern der Ente an seinen Platz; Studio bekommt dabei Manifest, Service Worker, Icons und
`build.json` für die App-Installation und Updates. Das Ergebnis steht auf https://kernengine.de/.

### Prüfliste

| Punkt | Studio-Spiel | C++-Spiel |
|---|---|---|
| Keine Fehler | Output Log, Play | Browser-Konsole |
| Telefon | Sticks kommen von selbst | Touch selbst auswerten |
| Ton | startet mit erstem Tipp | Sache des Spiels |
| Dateien | eine HTML (plus Ente-Ordner) | HTML, `.wasm`, zwei `.mjs` |
| Auslieferung | überall, auch von der Platte | Webserver |

---

## Das nimmst du mit

- Drei Wege: **Studio ohne Code**, **C++ mit der App-API**, **JavaScript über die C-ABI**, alle auf
  derselben Engine.
- Ein Studio-Spiel entsteht in zehn Schritten: Level, Figur, Hindernisse, Pickups, Gefahr, Logik, Ton,
  Testen, Speichern, Exportieren.
- **Package Game** macht daraus eine einzige HTML-Datei, die überall läuft, auch am Telefon.
- Eine C++-App erbt von `App`; `update` läuft jeden Frame, `kern_create_app()` liefert das Spiel.
- `hello` zeigt Instancing und Picking, `orbrun` ein ganzes Spiel mit eigener Shader-Erweiterung.
- Aus JavaScript steuerst du den Renderer (`KernRenderer`) oder ganze Studio-Levels (`startGame`).
- Du darfst deine Spiele verkaufen, nur die Micro Duck mit Pollens Modellen ist nicht kommerziell.

## Knobelecke

1. Warum startest du dein Spiel mit **Basic** und nicht mit **Empty**?
2. Dein Würfel aus **Shapes** fällt nicht herunter. Was fehlt?
3. Warum liest man `pressed()` in `update` und speichert es nicht für später?
4. Dein C++-Spiel startet nicht, wenn du `index.html` doppelklickst. Warum?
5. Du willst ein Spiel mit Micro Duck verkaufen. Was musst du beachten?

### Lösungen

1. Empty hat keinen Boden; die Figur würde fallen.
2. Ein **Rigid Body** mit **Mobility = Dynamic** (oder gleich eine **Physics Cube** nehmen).
3. Es gilt nur für diesen einen Frame und wird danach zurückgesetzt.
4. Die Seite lädt das Modul nach; dafür braucht sie einen Webserver.
5. Pollens 3D-Modelle sind nur nicht kommerziell nutzbar. Ein kommerzieller Build lässt sie weg, die Ente
   ist dann unsichtbar.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
