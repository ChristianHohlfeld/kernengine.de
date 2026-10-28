# 12 Dein eigenes Spiel

> **Dein Level:** Jetzt baust du ein ganzes Spiel – mit Spielfigur, Hindernissen, Orbs, Gefahr,
> Logik und Ton – und verschickst es als eine Datei. Danach schaust du Profis über die Schulter, die
> Spiele in C++ oder JavaScript schreiben.

Kern bietet **drei Wege** zum eigenen Spiel, alle mit derselben Grafik, Physik und Plattformschicht:

| Weg | Womit | Gut für |
|---|---|---|
| **A** Studio, ohne Code | Kern Studio, Logik-Graphen | schnelle Spiele, Unterricht; Ergebnis: eine HTML-Datei |
| **B** C++ | eine Klasse, die von `kern::App` erbt | eigene Simulation, eigene Grafik, eigene Shader |
| **C** JavaScript | die C-Schnittstelle der Engine | eigene Seiten, die die Engine steuern (wie SpawnCity) |

Dazu kommt eine Mischform: Logik-Graphen aus Studio als C++ exportieren (12.4).

---

## 12.1 Weg A: Orb Garden, ein Spiel ohne Code

Wir bauen ein Sammelspiel: Eine Figur läuft über eine Fläche mit Hindernissen, sammelt Orbs, weicht
einer Gefahr aus und gewinnt, wenn alles eingesammelt ist. Die Fenster erklärt
[Kapitel 11](11-studio.md).

### Schritt 1: Ein neues Level

**File › New Level...**, Vorlage **Basic**, **Create** – mit Boden (40 × 40 m), Sonne, Himmel und
einem Würfel. In **World Settings** setzt du **Level name** und **Game title** auf „Orb Garden“.
Warum nicht Empty? Da gibt es nicht mal einen Boden. **Endless ground** ginge zwar, aber ein
sichtbarer Boden ist für den Anfang anschaulicher.

### Schritt 2: Die Spielfigur

Content Drawer (**Ctrl+Space**) › **Players** › **Character** in den Viewport ziehen. Das Log meldet
„Character is the player now“, im Outliner steht **PLAYER**. In **Details › Player Controller**
stellst du **Max speed**, **Acceleration**, **Jump speed** und die Kamera ein (**Chase**, **First
person**, **Top down**). **Play** (Alt+P): W A S D läuft, die Leertaste springt, **Esc** führt
zurück. Spieler ist die zuletzt platzierte Figur oder die per Rechtsklick › **Make Player**.

> **Mission:** Bau Schritt 1 und 2 nach. Spiel einmal mit **Top down** und einmal mit **First
> person**. Welche Sicht passt besser zu einem Sammelspiel?

### Schritt 3: Hindernisse und Physik

1. Aus **Shapes** ein paar **Cube**, eine **Ramp** und eine **Plane** platzieren und mit **W**, **E**,
   **R** zurechtrücken; das Raster (0,5 m) hält die Kanten sauber, **End** setzt auf den Boden.
2. Aus **Physics** einen **Crate Stack** und eine **Hinged Door** – sie reagieren auf deine Figur.
3. Aus **Gameplay** eine **Moving Platform** und einen **Spinner**, die sich von selbst bewegen.
4. Mit **Simulate** (Alt+S) prüfen, Kisten greifen und werfen. **K** übernimmt die Lage der gewählten
   Actors – praktisch für einen Stapel, der natürlich umgefallen aussehen soll.

> **Vorsicht, Falle:** Ein **Cube** aus Shapes hat Kollision, fällt aber nicht. Gib ihm mit **+ Add ›
> Rigid Body** einen Körper und stell **Mobility** auf **Dynamic** – oder nimm die **Physics Cube**
> ([Kapitel 6](06-physik.md)).

### Schritt 4: Etwas zum Einsammeln

Verteile **Orbs** (1 Punkt) und einen **Star** (10 Punkte); mit **Alt + Gizmo ziehen** kopierst du
schnell. Wähle alle Orbs und leg sie mit dem Ordner-Knopf im Outliner in einen Ordner. Dann **World
Settings › Win condition** auf **Collect all pickups**.

### Schritt 5: Gefahr, Checkpoint und Zeitlimit

Ein **Hazard** (verliert das Spiel) oder eine **Damage Zone**, mit Scale auf Größe gezogen. Ein
**Checkpoint**: Fällt die Figur unter **Kill Z** oder verliert sie, startet sie dort neu. Wer mag,
setzt ein **Time limit**; das HUD zählt herunter. Andere Ziele: **Reach a goal trigger** (mit
**Goal**), **Survive the time limit**, **Reach a target score**.

> **Mission:** Bau Schritt 3 bis 5 mit 60 Sekunden Zeitlimit. Zu leicht? Leg den Star auf die Moving
> Platform.

### Schritt 6: Logik ohne Programmieren

1. Einen **Message Trigger** platzieren, in Details › Trigger die **Action** auf **Logic event
   only**.
2. In Details auf **Logic** klicken; der Logik-Editor öffnet den Graphen dieses Actors.
3. **Templates › Score zone (overlap +10)**: Betreten gibt 10 Punkte, „+10“ und einen Ton.
4. **Compile** (F7) prüft, **Viewport** führt zurück.

Weitere Vorlagen: eine Tür auf **Interact** (Taste F), ein Spawner, eine Ampel und ein Verhalten für
die Micro Duck ([Kapitel 9](09-logik.md)).

> **Vorsicht, Falle:** Ein Graph mit Fehlern hält Play nicht auf. Kern schaltet beim Start nur
> diesen Graphen ab, schreibt „Logic error: …“ ins Output Log, und der Rest läuft weiter. Passiert in
> deiner Zone nichts, schau ins Log.

### Schritt 7: Ton

In **World Settings › Audio** eine **Music** und eine **Ambience** wählen. Aus **Audio** etwa
**Ambient Wind** platzieren, oder im Tab **Sounds** eine eigene Datei importieren und in den
Viewport ziehen. Figuren, Pickups und Trigger bringen ihre Geräusche schon mit
([Kapitel 7](07-audio.md)).

### Schritt 8 bis 10: Testen, Speichern, Exportieren

- **Play** testet, **F8** lässt die Figur los und übernimmt sie wieder. **Play › Number of Players ›
  2 Players** testet zu zweit. **Fehler im Level**, etwa ein Gelenk zu einem gelöschten Actor,
  verhindern Play und stehen im Output Log.
- **Ctrl+S** speichert eine `.klevel`-Datei samt Texturen und Sounds, die du weitergeben und in jedem
  Studio öffnen kannst. Ungespeichertes sichert Studio zusätzlich im Browser.
- **File › Package Game (HTML)...**, Titel prüfen, **Package**: Studio lädt `Orb_Garden.html`
  herunter (das Leerzeichen wurde zu `_`) und bietet **Play it** an.

Die Datei enthält Engine, Level und Spiel-Host. Sie läuft offline, als einfache Datei, von jedem
Webserver und auf dem Handy mit Touch-Sticks.

![Dasselbe Spiel auf dem Telefon: die Engine erkennt Touch und zeichnet Sticks und Knöpfe](../buch/bilder/game-phone.png)

> **Mission:** Exportiere Orb Garden, klick **Play it** und spiel eine Runde. Öffne die Datei dann
> auf einem Handy oder Tablet: Die Touch-Sticks erscheinen von allein.

---

## 12.2 Weg B: ein Spiel in C++

Hier ist dein Spiel eine **Klasse**, die von `kern::App` erbt. Die Engine ruft sie jeden Frame auf
und malt danach; Browser-Kram erledigt `kern/web/app.mjs`.

> **So kannst du es dir vorstellen:** Die Engine ist Spielfeld und Anzeigetafel, deine Klasse die
> Mannschaft. Jeden Moment heißt es „Du bist dran“ – sie macht ihren Zug, die Tafel zeigt ihn.

### Die Bausteine

Deine **App** kann `configure()` (Grafik-Optionen wie Schatten und Bloom), `extensions()` (eigene
Shader), `start()` (Formen anlegen; „falsch“ bricht ab), `update()` (jeden Frame), `resized()` und
`stop()` füllen. Die **Engine** gibt dir den `renderer` (Formen, Kamera, Licht, Anklicken,
[Kapitel 4](04-grafik.md)), `input`, `set_status(text)` für die Seite, `request_pointer_lock`,
Startwerte aus der Adresse (`arg`, etwa `?time=0.6`), Größe, `stats()` und `clear_color`.

**Input** kennt `down(Key)` (gehalten), `pressed`/`released` (in diesem Frame), Zeiger, Maustasten,
Mausrad, bis zu 10 Finger und den Fokus. Tasten heißen `a` bis `z`, `digit0` bis `digit9`, `left`,
`space`, `enter` und so weiter – nach ihrer **Position**. **Frame** liefert `time`, `dt` (Sekunden
seit dem letzten Frame, höchstens 0,1) und `index`.

`update` läuft **genau einmal** pro Bild, dann wird gemalt. `pressed`, `released`, Zeigerbewegung und
Mausrad gelten nur für diesen Frame – lies sie in `update`, statt sie aufzuheben.

### Das kleinste Kern-Spiel: hello

`kern/examples/hello/hello.cpp` (83 Zeilen): 64 Kisten, die sich drehen und Schatten werfen, eine
Kamera, die du kreisen lässt, und Anklicken. `configure` schaltet Schatten an (1024er-Karte, 40 m).
`start` holt für jede Form eine Nummer (`allocate_id()`) und legt sie mit `upsert` an.

> **Krass, aber wahr:** Die 64 Kisten sind **eine** Form mit 64 Kopien („Instanzen“), jede mit
> eigener 4×4-Matrix (16 Zahlen für Lage, Drehung, Größe) und Farbe. Die Grafikkarte malt alle mit
> **einem** Zeichenbefehl.

```cpp
Input& in = engine.input;
if (in.down(Key::left)) yaw_ += frame.dt * 1.5f;
if (in.down(Key::right)) yaw_ -= frame.dt * 1.5f;
```

Zeile 1 holt die Eingabe. Hältst du ←, wächst der Drehwinkel `yaw_` um `dt × 1,5` (Zeile 2), bei →
schrumpft er (Zeile 3). Rechne nach: Bei 60 Bildern pro Sekunde kommen pro Bild 1,5 ÷ 60 = 0,025
dazu, nach einer Sekunde 1,5. Bei 30 Bildern 30 × 0,05 = 1,5. Gleich schnell – egal wie flott dein
Rechner ist. (1,5 im Bogenmaß sind etwa 86°.)

Die Maus dreht ebenfalls, das Mausrad ändert den Abstand (8 bis 80 m), dann wird die Kamera neu
gesetzt (Z oben). Jede Kiste dreht sich mit eigenem Tempo; `update_instances` lädt nur Geändertes
hoch. Ein Klick schießt einen Strahl durch den Pixel – die getroffene Kiste wird rot. `set_status`
schreibt Zeichenbefehle und Dreiecke unten auf die Seite.

```cpp
App* kern_create_app() {
  static Hello app;
  return &app;
}
```

Diese Funktion braucht jedes Spiel: Die Engine ruft sie beim Start einmal auf und bekommt dein Spiel.
`static` lässt es so lange leben wie das Modul. Die Seite dazu ist winzig: ein Canvas, eine
Statuszeile und `runKernApp` (12.3).

### Bauen

Du brauchst **wasi-sdk**, Release 25 (gibt es auf GitHub):

```sh
export WASI_SDK=/opt/wasi-sdk-25.0-x86_64-linux
kern/build.sh --app \
  kern/examples/hello/hello.wasm \
  kern/examples/hello/hello.cpp
```

Zeile 1 sagt, wo wasi-sdk liegt; der Rest ist ein Befehl (`\` setzt ihn fort): baue aus
`hello.cpp` die Datei `hello.wasm`. Schalter fügen Teile hinzu: `--app` (App-Laufzeit), `--physics`
(Box3D), `--game` (Levels und Spiel-Laufzeit), `--studio` (Kern Studio), `--lehrer` (der Lehrer).
Dateien auf `.cpp` oder `.cc` sind Quellen, der Rest geht an den Compiler. `KERN_BUILD=debug` baut
zum Fehlersuchen, `KERN_JOBS` und `KERN_OBJ_CACHE` regeln Tempo und Zwischenspeicher – übersetzt
wird nur, was sich geändert hat. Das Ergebnis läuft in jedem Browser mit WebGL2, auch Safari ab 15
([Kapitel 2](02-bauen-und-testen.md)).

### Ein ganzes Spiel: orbrun

`kern/examples/orbrun/orbrun.cpp` (726 Zeilen): Ein Hovercar sammelt in einer Zufallsstadt 24 Orbs,
während es Nacht wird. W/S fahren, A/D lenken, Leertaste Boost, N schnelle Zeit, Enter neue Stadt;
am Handy lenkt die linke Hälfte, die rechte boostet.

> **Vorsicht, Falle:** Das Studio-Level **Orb Run** ist ein anderes Programm mit derselben Idee: aus
> Actors gebaut, mit Box3D. Das C++-Beispiel macht alles selbst.

- **Stadt:** 7 × 7 Blöcke, 80 m Abstand, 16 m breite Straßen, je Block eine von vier Bebauungen,
  zur Mitte hin höher ([Kapitel 5](05-szene-und-geometrie.md)).
- **Kollision ohne Physik-Engine:** Das Auto ist ein Kreis (Radius 1,7 m), der aus den Häusern
  geschoben wird und an Wänden abprallt. Beim Boost bewegt es sich in Schritten von höchstens 0,6 m,
  damit es nicht durch Wände flutscht.
- **Fahrphysik:** Höchsttempo 36 m/s, mit Boost 60 m/s – das sind 36 × 3,6 = **129,6 km/h** und
  **216 km/h**. Lenken wirkt erst mit Tempo.
- **Verkehr:** 22 Autos, 9 bis 15 m/s; stehst du weniger als 10 m vor einem, hält es.
- **Orbs:** näher als 3,2 m – weg, 28 Funken, und am Ende eine Bestzeit.
- **Tag und Nacht:** 480 s pro Tag, mit N 40 s; Sonne, Mond, Himmel, Nebel und bis zu sechs
  Punktlichter folgen.

> **Für Profis:** Die Shader-Erweiterung **NightWindows** zeigt, wie ein Spiel dem Standard-Shader
> etwas hinzufügt, ohne die Engine anzufassen: Jede Hauswand bekommt vier Zahlen (Länge, Höhe,
> Zufallswert, Geschosshöhe), daraus malt der Shader ein Fensterraster, das nachts leuchtet. In der
> Ferne verschwimmt es, damit nichts flimmert ([Kapitel 4](04-grafik.md)).

### Ein eigenes Spiel anlegen

Lege einen Ordner `meinspiel/` mit `meinspiel.cpp` (deine App-Klasse plus `kern_create_app()`) und
`index.html` an, baue mit `kern/build.sh --app` (für Box3D zusätzlich `--physics`, für Levels
`--game`) und liefere den Ordner mit `kern.mjs` und `app.mjs` über einen Webserver aus (12.5).

---

## 12.3 Die Plattformschicht app.mjs

`kern/web/app.mjs` macht nur, was ein Browser nur in JavaScript erlaubt.

```js
const app = await runKernApp({
  canvas: document.getElementById("game"),
  url: "./meinspiel.wasm",
});
```

Zeile 1 startet dein Spiel und wartet, bis es läuft; Zeile 2 sagt, wo gemalt wird; Zeile 3, welche
Datei geladen wird. Optional gibt es `onStatus`, `onError`, `maxPixelRatio` und Startwerte; zurück
kommt ein Objekt mit `stop()`. Der Gastgeber hält die Zeichenfläche passend groß, liest Tasten nach
Position (Pfeile scrollen nicht), reicht Maus und Finger weiter, lässt bei Fokusverlust alle Tasten
los, fängt auf Wunsch die Maus und baut nach einem Grafikkarten-Absturz alles wieder auf. Ohne
`--app` gebaut heißt es „module was not built with --app“.

---

## 12.4 Weg C: aus JavaScript

**Grafik aus JavaScript (wie SpawnCity):** `kern/web/kern.mjs` mit `loadKern` und `KernRenderer`.
Spiellogik in JavaScript, Malen in C++: Formen mit Namen anlegen (`upsert`), Kopien ändern, Kamera
setzen, `render()`, Dreiecke anklicken mit `pick`. Im Frame wird nichts neu angelegt; es gibt einen
Renderer pro Modul ([Kapitel 17](17-referenz.md)).

**Studio-Levels einbauen:** `startGame` aus `kern/web/studio/game.mjs` ist derselbe Spiel-Gastgeber
wie in exportierten Dateien. `play.html` spielt eingebaute Levels (`?level=orbRun`, `physicsLab`,
`shootingRange`, `microDuck`, `basic`); ein eigenes gibst du als Text deiner `.klevel`-Datei mit.
Zurück kommt ein Objekt (auch `window.KernGame`):

```js
game.command({cmd: "gameInput", moveY: 1, jump: true});
game.command({cmd: "gameInput", moveY: 0, jump: false});
if (game.status.hud?.status === "won") showMyWin();
```

Zeile 1 lässt die Figur vorwärts laufen und springen – wie eigene Bildschirmknöpfe. Zeile 2 lässt
los. Zeile 3 fragt, ob gewonnen wurde, und zeigt dann deinen Siegerbildschirm. `gameInput` kennt auch
`moveX`, `boost`, `fire`, `aim`, `crouch`, `interact`, `lookX` und mehr; die Werte addieren sich zu
Tastatur, Touch und Gamepad.

**Logik-Graphen als C++:** Der Knopf **C++** im Logik-Editor lädt den Graphen als C++-Text. Mit
`kern/build.sh --studio` eingebaut, läuft er als Maschinencode statt über Interpreter oder JIT – mit
demselben Ergebnis ([Kapitel 9](09-logik.md)).

---

## 12.5 Hosten

Ein C++-Spiel besteht aus `meinspiel/index.html`, `meinspiel.wasm` und `kern/web/app.mjs` mit
`kern.mjs` direkt daneben. Weil die Seite das Modul übers Netz lädt, brauchst du einen
**Webserver**; für den eigenen Rechner reicht ein einfacher im Hauptordner des Repositorys.

> **So kannst du es dir vorstellen:** Das exportierte Studio-Spiel ist ein fertiges Lunchpaket in
> einer Dose. Das C++-Spiel ist Kochen in der Schulküche – du brauchst die Küche dazu.

---

## 12.6 Veröffentlichen

**Die Rechte:** Kern ist Open Source (Apache License 2.0). Eigene Spiele und Netze darfst du nutzen,
weitergeben und verkaufen. Wer ein exportiertes Spiel weitergibt, legt Lizenz und NOTICE bei (im
HTML reicht ein Hinweis mit Link) und lässt die Urheberrechtsvermerke drin. „Made with Kern“ ist
erlaubt, dein Produkt „Kern“ zu nennen nicht. Teile von anderen behalten ihre Lizenzen
([Anhang D](D-lizenzen.md)).

**Ausnahme Micro Duck:** Pollens 3D-Modelle der Ente (CC BY-NC-SA 4.0) sind **nur nicht
kommerziell** erlaubt. Zum Verkaufen baust du mit `KERN_COMMERCIAL=1 tools/build_studio.sh`; dann
hat die Ente kein Aussehen. Pollens Netze (Apache 2.0) dürfen mit Lizenz und NOTICE mit.

**Studio-Spiele** lädst du auf einen Webspace, legst sie in ein Repository mit GitHub Pages oder
verschickst sie direkt. Mit Ente gehören `microduck/` und `policies/` daneben, sonst kann sie nur
laufen ([Kapitel 11](11-studio.md)). Online spielen geht mit `?host=raum`, `?join=raum` und für
verschiedene Rechner einem Relay (`node tools/net_relay.mjs`, Port 8790) plus `&relay=`
([Kapitel 10](10-netzwerk.md)).

**C++-Spiele** lädst du mit derselben Ordnerstruktur hoch. Kern selbst baut so in seiner CI
(`.github/workflows/ci.yml`) Studio, hello und orbrun und veröffentlicht alles auf
https://kernengine.de/; der Quellcode liegt auf https://github.com/ChristianHohlfeld/kernengine.de.

| Prüfliste | Studio-Spiel | C++-Spiel |
|---|---|---|
| keine Fehler | Output Log | Browser-Konsole, `onError` |
| Handy | Sticks kommen von selbst | Touch selbst auswerten |
| Ton | nach erstem Klick | Sache des Spiels |
| Dateien | eine HTML (plus Ente-Ordner) | HTML, `.wasm`, `kern.mjs`, `app.mjs` |
| Auslieferung | beliebig, auch als Datei | Webserver |

> **Mission:** Geh die Prüfliste für Orb Garden durch: Log ohne Fehler? Läuft es am Handy? Musik nach
> dem ersten Tipp? Dann: veröffentlichen!

---

## Level geschafft!

- Du kennst die drei Wege zu einem Kern-Spiel.
- Du hast ein Sammelspiel in zehn Schritten gebaut, gespeichert und exportiert.
- Du weißt, dass ein kaputter Logik-Graph das Spiel nicht stoppt – und wo du nachschaust.
- Du verstehst `update` und `dt` in einem C++-Spiel.
- Du weißt, warum 64 Kisten mit einem Zeichenbefehl gemalt werden.
- Du kennst die Regeln fürs Weitergeben und Verkaufen.

## Rätsel-Ecke

1. Warum ist **Basic** ein besserer Start als **Empty**?
2. Dein Shapes-Würfel fällt nicht. Was fehlt?
3. Warum läuft das exportierte Studio-Spiel per Doppelklick, das C++-Spiel nicht?
4. **Boss-Rätsel:** Ein Spiel bewegt eine Figur mit `x += 5 * frame.dt`. Rechner A schafft 50
   Bilder pro Sekunde, B 25. Wie weit kommt die Figur in 2 Sekunden? Und mit `x += 0.1` ohne `dt`?

### Lösungen

1. Empty hat **keinen Boden**.
2. Ein **Rigid Body** mit **Mobility = Dynamic** – oder gleich eine Physics Cube.
3. Das Studio-Spiel hat **alles in einer Datei**; das C++-Spiel lädt `.wasm` und Module nach, was der
   Browser nur über einen Webserver erlaubt.
4. Mit `dt` beide **10 m** (5 m pro Sekunde mal 2 s). Ohne `dt` kommt A 100 × 0,1 = 10 m weit, B nur
   50 × 0,1 = **5 m**. Darum rechnen Spiele mit `dt`.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
