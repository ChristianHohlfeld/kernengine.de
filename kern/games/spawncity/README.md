# SpawnCity auf Kern

SpawnCity als natives C++20-Spiel gegen die Kern-API: WebAssembly aus `kern/build.sh`, WebGL2-Rendering
und Box3D über Kern. Kein three.js, keine Spiellogik in JavaScript. Die Seite ist ein dünner Host
(Canvas, Eingabe-Events, Kartenkacheln holen, Audio ausgeben, HUD zeichnen).

Vorlage für Inhalt, Werte und Verhalten ist das laufende SpawnCity (`nostack-dev/spawncity-de`, Commit
`9108537`, `sim/*.mjs`, `esp32/*.hpp`). Die Zahlen stehen mit Quellenangabe im Code.

**Stand 26.11.2026: das kleine Spiel – eine Drohne über der echten Stadt.** Die Stadt kommt aus Kartendaten
(Vektorkacheln, Höhen, Luftbilder; ohne Netz die prozedurale Ersatzstadt), die Drohne ist die Vorlage
„Drone“ des Levels, geflogen von ihrer Flugregler-Firmware (`firmware/`, unverändert aus SpawnCity) auf der
Welt der Engine (`kern::game::GameWorld`). Alles andere des großen Spiels – zu Fuß, Waffen, Autos, Verkehr,
Passanten, Jet, Fallschirm, Mehrspieler, VR, GPS live – ist herausgenommen und kommt als Baustein der Engine
zurück: `BLUEPRINTS.md`, „Herausgenommen am 26.11.“ (der letzte Stand mit allem: `d90426f3`).

## Starten

```sh
kern/games/spawncity/build.sh              # wasm + generated/spawncity.html (Einzeldatei)
python3 -m http.server 8080                # im Repo-Wurzelverzeichnis
# http://127.0.0.1:8080/kern/games/spawncity/web/index.html
```

`generated/spawncity.html` enthält alles (Module und wasm als base64) und läuft auch von `file://`.
`tools/dropin.sh <dir>` legt das Spiel an die Adresse des Originals (`drone_simulator.html`, das Drop-in für
spawncity.de).

Startparameter (Query-String):

| Parameter | Wirkung |
|---|---|
| `lat=…&lon=…` | Startort (Standard: Konstanz, Marktstätte 47.6600 / 9.1766) |
| `offline=1` | keine Kartendaten, prozedurale Ersatzstadt |
| `world=town\|training` | Welt ohne Kartendaten: Ersatzstadt / Trainingslevel des Originals |
| `mobile=1` / `mobile=0` | Telefon-Profil erzwingen |
| `fpvtilt=-15…50` | Neigung der FPV-Kamera (SpawnCity-Standard −15°) |
| `tilejson=URL`, `tiles=URL-Vorlage` | andere Kartenquelle (Tests) |
| `dem=0` | ohne Höhenkacheln (flaches Gelände) |
| `demtiles=URL-Vorlage`, `imagery=URL-Vorlage` | andere Höhen- / Luftbildquelle (Tests), `{z}/{x}/{y}` |
| `view=kern` | Kerns eigene Oberfläche statt der des Originals (`web/view/original.html`) |
| `mute=1`, `audio=kern` | ohne Ton / Kerns Mischer statt des Tons des Originals |
| `menu=1`, `autostart=1` | Titelmenü zeigen / gleich starten |
| `perf=1`, `hq=1`, `rotate=0\|1` | Leistungsanzeige, volle Pixeldichte in Software-WebGL, Hochformat drehen |

## Steuerung

Kurz (alles mit Quellen in `CONTROLS.md`): **WASD** linker Stick, **Pfeile** / **Maus** (gesperrt) rechter
Stick, **Leertaste / Shift** steigen / sinken, **R** ARM (in der Luft: kontrollierte Landung), **C** nächste
Kamera (FPV, Verfolger, Dritte Person). Xbox-Pad: Sticks, RT/LT Höhe, A ARM, B STOP, Y Kamera. Touch:
zwei Sticks, Höhen-Pad, ARM, STOP. Auf der Seite: **TON**, **MUSIK**, **RESET** (die Drohne zurück an den
Start, auf dem Boden).

## Aufbau

| Teil | Dateien (`src/`) |
|---|---|
| Spiel (Zustand, Frame, HUD, Befehle) | `spiel.{h,cpp}` – `sc::SpawnCity`, das eine Objekt `game()` |
| Einstieg, Exporte der Seite | `main.cpp` – `SpawnCityApp` (die App der Engine), `sc_*`-Exporte |
| Drohne | `drone`, `drone_art`, `rigid_spec.h`; Firmware `firmware/` |
| Stadt | `city_stream`, `city_jobs`, `tiles`, `mvt`, `terrain`, `wire.h` |
| Eingabe, Einstellungen, Ton | `controls`, `touch_play`, `settings`, `sound_fx` |
| Level, Projekt | `level/spawncity.klevel`, `level/spawncity.kproject` (aus `tools/small_level.py`): Vorlage „Drone“, Regeln `Controls` und `Settings`, im Ordner „Source“ die Blueprints der Quellen (`tools/cpp_to_blueprint.py`) |
| Seite | `web/` – `host.mjs` (Host), `ui.mjs` (Kerns Oberfläche), `view.mjs` + `view/` (die des Originals), `audio.mjs` + `audio/` (sein Ton) |

Der Weg zum Projekt ohne eigenes C++ (jede Datei wird Blueprint): `BLUEPRINTS.md`.

`SpawnCity.command(...)` in der Browser-Konsole: `drone`,
`camera`, `cam [fpv|follow|third]`, `reset`, `pause 0|1`, `start <lat> <lon>`, `origin`, `settings schema`,
`get <key>`, `set <key> <value>`.

## Tests

```sh
kern/games/spawncity/tests/run_tests.sh          # nativ, ohne GL, ASan/UBSan
node kern/games/spawncity/tests/smoke.mjs [dir]  # Browser (Playwright, Chromium mit SwiftShader)
```

Die nativen Tests prüfen das Spiel auf der App-Laufzeit (`test_spiel`: Autopilot über der ganzen
Ersatzstadt, zweimal dieselbe Spur; Befehle, TON / MUSIK / RESET), den Einstieg (`test_entry`), Flugregler und
Drohnenstrecke (`test_drone`: Kalibrieren, ARM, Schweben, Vorwärtsflug, Gieren, Steigen, Höhenstick-Gesetz),
MVT-Decoder und Karten (`test_map`), das Streaming (`test_stream`: Zeitscheiben, Kollision, Entladen,
Ersatzstadt), das Gelände (`test_terrain`), Steuerung und Einstellungen (`test_controls`, `test_settings`).
`tests/converted` vergleicht jede Funktion mit ihrem Blueprint Bit für Bit, `tests/lowered` die Differenztests.
Der Smoke-Test lädt die Seite zweimal (Desktop mit synthetischen Vektor- und Höhenkacheln über den echten
Pfad, Telefon mit Touch offline), fliegt die Drohne (ARM, Steigen, Vorwärts, drei Kameras), drückt TON, MUSIK
und RESET und macht Screenshots. `tests/input_check.mjs` prüft die Eingabe (Maus unter Lock, Tasten, Pad,
Finger). Auf Abruf (Workflows): `compare.mjs` und `touch_compare.mjs` (Original und Kern nebeneinander),
`profile.mjs` und `touch_check.mjs`, `view_check.mjs` (die Ansicht des Originals auf einer Seite).

## CI

`.github/workflows/ci.yml`: `native-spawncity-build` baut die nativen Tests, `native-spawncity` führt sie in
zwei Durchgängen (gegen den Ersatz der Engine und gegen die Engine selbst) in 12 Teilen aus; `build` baut das
Spiel (`build.sh`, `build.sh --only-blueprints`), prüft die Module der Seite und legt die Website samt Drop-in
(`tools/dropin.sh`) an; `e2e-spawncity` öffnet das Level im Editor, `input` prüft die Eingabe am Drop-in.
`.github/workflows/spawncity.yml` (Zweige `sc-*`, `spawncity-*`, auf Abruf): Bau und Smoke-Test.

## Lizenzen und Namensnennung

* Spielcode: Apache License 2.0 wie Kern. Die Firmware unter `firmware/` stammt aus SpawnCity
  (`nostack-dev/spawncity-de/esp32`, Christian Hohlfeld) und steht hier ebenfalls unter Apache 2.0.
* Kartendaten: © OpenStreetMap-Mitwirkende, ODbL 1.0; Vektorkacheln von OpenFreeMap im
  OpenMapTiles-Schema (© OpenMapTiles). Die Namensnennung steht dauerhaft im Spiel (rechts unten).
* Höhen: „Terrain Tiles“ (Mapzen, auf AWS Open Data, frei nutzbar mit Namensnennung der Quellen
  SRTM, EU-DEM, 3DEP u. a., siehe https://github.com/tilezen/joerd/blob/master/docs/attribution.md);
  im Spiel rechts unten genannt.
* Gebaut mit `--commercial`: keine Micro-Duck-Meshes (CC BY-NC-SA) im Modul. Keine Pollen-Meshes,
  keine fremden Bilder. Der Ton der Seite ist der von SpawnCity (spawncity.de, derselbe Autor): seine
  Musikdatei und seine Audio-Module in `web/audio/`, `web/sim/audio/`; Kerns eigene Töne sind
  Synth-Patches im Spielordner.
* Kein OSM-Snapshot im Repo: die Umgebung dieses Meilensteins hatte keinen Zugriff auf OSM-Quellen,
  im SpawnCity- und im Kern-Repo liegt keiner. Der Offline-Fall nutzt deshalb die prozedurale Stadt.

