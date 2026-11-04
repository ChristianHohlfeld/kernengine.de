# SpawnCity auf Kern

SpawnCity als natives C++20-Spiel gegen die Kern-API: WebAssembly aus `kern/build.sh`, WebGL2-Rendering
und Box3D über Kern. Kein three.js, keine Spiellogik in JavaScript. Die Seite ist ein dünner Host
(Canvas, Eingabe-Events, Kartenkacheln holen, Audio ausgeben, HUD zeichnen).

Vorlage für Inhalt, Werte und Verhalten ist das laufende SpawnCity (`nostack-dev/spawncity-de`, Commit
`9108537`, `sim/*.mjs`, `esp32/*.hpp`). Die Zahlen stehen mit Quellenangabe im Code.

Stand: **Meilenstein 2** – alle Modi, dazu die Stadt mit Leuten, Polizei und Fahndung, das ganze
Waffenset mit Explosionen und Fahrzeugschaden, Mehrspieler, Minimap und Straßenlaternen.

## Starten

```sh
kern/games/spawncity/build.sh              # wasm + generated/spawncity.html (Einzeldatei)
python3 -m http.server 8080                # im Repo-Wurzelverzeichnis
# http://127.0.0.1:8080/kern/games/spawncity/web/index.html
```

`generated/spawncity.html` enthält alles (Module und wasm als base64) und läuft auch von `file://`.

Startparameter (Query-String):

| Parameter | Wirkung |
|---|---|
| `lat=…&lon=…` | Startort (Standard: Konstanz, Marktstätte 47.6600 / 9.1766) |
| `offline=1` | keine Kartendaten, prozedurale Ersatzstadt |
| `start=drone\|car\|jet` | Startmodus |
| `mobile=1` / `mobile=0` | Telefon-Profil erzwingen (Schatten 1024, AO niedrig, Pixelratio 1,5) |
| `fpvtilt=-15…50` | Neigung der FPV-Kamera (SpawnCity-Standard −15°) |
| `mouse=20…300` | Mausempfindlichkeit in % |
| `tilejson=URL`, `tiles=URL-Vorlage` | andere Kartenquelle (Tests) |
| `mute=1` | ohne Ton |
| `mp=host&room=NAME` / `mp=join&room=NAME` | Mehrspieler: hosten / beitreten (Tabs eines Browsers über BroadcastChannel) |
| `relay=wss://…` | dazu: über das Kern-Relay (`tools/net_relay.mjs`) zwischen Rechnern |

## Steuerung

| Modus | Tastatur/Maus (Pointer-Lock per Klick) | Gamepad (Xbox) | Touch |
|---|---|---|---|
| zu Fuß | WASD, Shift Sprint, Leertaste Sprung, Strg ducken, LMB Feuer, RMB zielen, R nachladen, **F** einsteigen, **V** Drohne | LS laufen, RS schauen, A Sprung, B ducken, RT Feuer, LT zielen, RB nachladen, **X** einsteigen, **D-Pad ↓** Drohne | Kerns Soldaten-Layout (Sticks, Feuer am Finger, Knöpfe) + DROHNE / EINSTEIGEN |
| Drohne | R ARM / Landen, WASD fliegen (GAME: Geschwindigkeit), Leertaste/Shift Höhe, Maus X drehen, Maus Y Körperneigung, Q/E drehen, C Kamera (FPV/Follow/Third), G GAME/ANGLE, K Motoren aus, **V** zurück | LS fliegen, RS drehen/neigen, LT/RT Höhe, A ARM, View Kamera | linker Stick fliegen, rechter Stick Höhe/Drehen, ARM, KAMERA, ZU FUSS |
| Auto | W Gas, S Bremse/rückwärts, A/D lenken, Leertaste Handbremse, C Cockpit, Maus Kopf, **F** aussteigen | LS lenken, RT Gas, LT Bremse, B Handbremse, X aussteigen | links lenken, rechts Gas/Bremse, HANDBREMSE, AUSSTEIGEN |
| Jet | W/S Schub bzw. Steigen/Sinken, A/D Seitenruder, Maus nicken/rollen (Rate), Pfeile, **V** Schweben/Flug, C Cockpit, **F** aussteigen (am Boden) bzw. Schleudersitz | LS Schub/Seite, RS nicken/rollen, LB Schweben/Flug, X aussteigen | links Schub/Seite, rechts nicken/rollen, FLUG/SCHWEBEN, EJECT |
| Fallschirm | WASD steuern | LS | linker Stick |
| Waffen zu Fuß | **1** MP, **2** Glock, **3** Sniper, **5** Granatwerfer, **6** Raketenwerfer, **7** Fäuste, **8** Messer, **T** Handgranate, **X** EMP (gegen Polizeidrohnen, 30 m, 18 s) | D-Pad ←/→ Waffe, **Y** Granate, D-Pad ↑ EMP | WAFFE, GRANATE, EMP |
| Waffen Drohne | LMB feuern, **1** MG, **2** Raketen, **3** GUIDED (folgt der Kamera) | RB feuern, Y wechseln | FEUER, Waffenknopf |
| Waffen Jet | LMB / Leertaste Bordkanone, **R** Rakete, **B** Bombe | RT, RB, Y | FEUER, RAKETE, BOMBE |

Flugmodi der Drohne: **GAME** (Zustandsregler mit Höhenhaltung, SpawnCitys Standard) und **ANGLE**
(Winkelmodus mit Gashebel). Einen reinen Acro-Modus hat die SpawnCity-Firmware nicht
(`Arondight45_DroneFC_Core.hpp`: der manuelle Modus ist der Winkelmodus); er wäre eine Änderung an der
Firmware selbst und ist deshalb nicht nachgebaut.

Ein Steig-Befehl auf einer kalibrierten, nicht scharfen Drohne schaltet ARM ein (wie der ARM-Knopf
des SpawnCity-HUDs); ARM in der Luft startet die kontrollierte Landung (`simulator.mjs`).

## Architektur

```
kern/games/spawncity/
  firmware/       Arondight45-Flugregler, unverändert aus spawncity-de/esp32 (Core, StateControl,
                  HardwareSensors, FirmwareRuntime) – dieselbe Quelle wie auf dem ESP32
  src/
    main.cpp      kern::App (Renderer-Optionen, NightWindows) und die sc_*-Exporte für die Seite
    game.*        das Spiel: Level, Modi, Eingabe, fester Physikschritt, Kameras, HUD-JSON
    drone.*       Quad-Strecke (Motor/Prop/Akku/IMU/NAV/SBUS) + Firmware bei ~1 kHz
    jet.*         6-DoF-Aerodynamik, Triebwerk, RCS, Fahrwerk, Fly-by-Wire (Port von jet_flight_dynamics.mjs)
    car.*         Fahrzeugmodell (Reifen, Lenkung, Antrieb aus world_vehicle_dynamics.mjs) auf Raycast-Fahrwerk
    mvt.*         Mapbox-Vector-Tile-Decoder (zeitgeteilt) + kleiner Writer (Tests)
    tiles.*       Kachelmathematik, OpenMapTiles-Ebenen → Gebäude/Straßen/Flächen, prozedurale Ersatzstadt
    city_stream.* Kachel-Streaming: Decodieren → BuildingBuilder/RoadBuilder → Meshes → Box3D-Kollision,
                  alles mit Zeitbudget; Entladen ferner Kacheln; Fahrspuren für den Verkehr
    vehicle_art.* prozedurale Fahrzeugmodelle, instanziert
    sound_fx.*    Synth-Patches (Rotor, Motor, Turbine, Jet-Röhren, Wind) auf Kerns Mixer
    touch_pad.*   Touch-Sticks/Knöpfe der Fahrzeuge (Overlay = Kerns TouchOverlay)
    wanted.*      Wanted-Regeln (Heat, Sterne, Polizei-Antwort, Fluchtzeit, Trefferchance, EMP)
    people.*      Fußgänger auf den Gehwegen (Routen von Straße zu Straße, Panik, Zeugen) und Beamte:
                  leichte Agenten, Box3D-Strahlen gegen Wände, Treffer auf Kapseln, Überfahren
    people_art.*  prozedurale Menschen (Gang, Hände hoch, zielen, liegen), Blaulicht, Polizeidrohnen,
                  Feuer, Projektile, Straßenlaternen
    road_graph.*  Straßengraph aus den Fahrspuren, A* und Pure Pursuit für die Streifenwagen
    game_town.cpp Stadt: Schüsse, Polizei (Streifenwagen + Beamte, Polizeidrohnen, EMP), Fahndung mit
                  Fluchtuhr, Fahrzeugschaden, Brand und Explosion, Minimap, Laternen
    weapons.h     Waffentabelle mit SpawnCity-Werten
    game_weapons.cpp  Waffen: Kerns Soldatenwaffen (MP/Glock/Sniper), Werfer, Nahkampf, Handgranate,
                  Drohne (MG, Raketen, GUIDED), Jet (Kanone, Rakete, Bombe), Projektile
    net_session.* Mehrspieler: Kerns Client/Server über HostTransport, Posen 15 Hz, Treffer
    game_net.cpp  Mehrspieler im Spiel: eigene Pose, andere Spieler zeichnen, Treffer senden/anwenden
  web/            index.html + host.mjs (dünner Host, nutzt kern/web/app.mjs, kern.mjs, studio/game.mjs unverändert)
  tests/          native Unit-Tests (run_tests.sh) und Playwright-Smoke-Test (smoke.mjs)
  build.sh        wasm + Einzeldatei-HTML
```

* **Eine Welt:** Alles läuft in einer `kern::game::GameWorld` (Level aus C++ gebaut). Der Spieler ist
  Kerns Soldat (Box3D-CharacterMover, Waffen, Gesundheit, Soldaten-Rig, Kamera) mit SpawnCity-Werten:
  5,5 / 8,1 m/s, Sprung 3,6 m/s, Augenhöhe 1,68 m, 90° horizontales Sichtfeld, Gesundheit 118 (+18 %),
  Startwaffe Pistole. Drohne, Jet, Verkehrsautos, Bus und Fallschirm sind Level-Actors mit
  Box3D-Körpern (Kollider und Massen aus `RigidSpec`); GameWorld interpoliert ihre Posen fürs Rendern.
* **Fester Physikschritt:** 240 Hz, 4 Substeps. Box3D setzt Kräfte nach jedem Schritt zurück. Deshalb
  registriert das Spiel einen eigenen Kern-Logic-Knoten (`spawncity.fixed_step`, Event „logic.tick“),
  der nach jedem Box3D-Schritt die Kraftmodelle für den nächsten Schritt rechnet (Drohne, Jet, Autos,
  Fallschirm). Box3D wird nicht verändert oder getunt.
* **Drohne:** Pro Physikschritt 4 Firmware-Takte (≈1042 µs, im erlaubten Fenster 600–1600 µs):
  rohe ICM-42688-Registerbytes aus spezifischer Kraft und Drehraten (Rauschen, Bias-Drift wie
  `simulator.mjs`), SBUS-Frames und NAV1-Frames mit 100 Hz (AGL per Box3D-Strahl), ESC-Pulse →
  Motormodell (Gegen-EMK, Wicklungswiderstand, Strom- und Packgrenzen, Rotorträgheit, Propmoment) →
  vier Schubkräfte an den Armen + Reaktionsmomente, Widerstand, Rotationsdämpfung.
* **Auto:** Kern stellt den Box3D-Wheel-Joint ohne Lenkung bereit. Deshalb hängt die Federung als
  Feder/Dämpfer je Rad entlang eines Strahls (Steifigkeit q·g/Einfederung, Dämpfung 0,45, Anschläge);
  darüber unverändert SpawnCitys Reifen- und Antriebsmodell.
* **Welt:** Der Browser holt OpenFreeMap-Vektorkacheln (z14, OpenMapTiles-Schema), C++ decodiert sie.
  3×3 Kacheln um den Spieler (≈ 5 km), Entladen jenseits eines Rings. Jede Stufe hat ein Zeitbudget
  (3–4 ms pro Frame), auch das Hochladen einzelner Chunk-Meshes. Kollision: ein Box3D-Dreiecksnetz je
  Gebäude (Wände + Dach). Boden: Kerns Ebene (Höhenmodell kommt später). Ohne Netz baut das Spiel eine
  prozedurale Ersatzstadt; das HUD sagt das deutlich („OFFLINE · prozedurale Ersatzstadt“).
* **Host-Schnittstelle** (`main.cpp`): `sc_tile_requests/sc_tile_data/sc_tile_failed/sc_offline`,
  `sc_gamepad`, `sc_touch_frame/sc_touch/sc_touch_view`, `sc_hud`, `sc_command`,
  `sc_audio_init/sc_audio_render`, `sc_net_push/sc_net_take`. JavaScript fragt ab (Pull-Modell),
  Kern-Imports bleiben unverändert.
* **Stadt und Polizei:** Leute sind leichte Agenten wie in SpawnCity (kein Box3D-Körper je Person):
  Gehweg neben der Fahrspur, Weiterlaufen an Straßenenden, Flucht bei Schüssen (10 m alle, bis 22 m
  ein Teil), Tiefflug unter 14 m, Explosionen; freie Bewegung prüft den Weg mit einem Box3D-Strahl in
  Hüfthöhe. Zeugen (lebende Passanten im Umkreis von 45 m) melden Taten an `wanted::Tracker`. Ab einem
  Stern kommen Polizeidrohnen (Ring um den Spieler auf seiner Höhe, Sicht im Erfassungsradius, Schüsse
  alle 4–5 s) und nach der Anfahrzeit Streifenwagen (Spawn 70–125 m, A* über den Straßengraph, Pure
  Pursuit, Rangieren bei Hängenbleiben, zwei Beamte steigen aus, halten 6–15 m Abstand und schießen).
  Die Fluchtuhr läuft erst, wenn die Polizei da ist. Tod löscht die Fahndung.
* **Mehrspieler:** wie SpawnCity: jeder simuliert seine Welt, die Spieler tauschen Posen; der Host
  verteilt sie und gibt neuen Spielern seinen Kartenursprung (alle streamen dieselbe Stadt). Treffer auf
  andere Spieler gehen an deren Rechner, der sie anwendet. Polizei und Passanten sind lokal.

## Übernommene SpawnCity-Werte (Auszug)

| Bereich | Werte | Quelle |
|---|---|---|
| Drohne | span 225 mm, 5"-Prop, Kv 2300, R 0,080 Ω, J 1,1e-5, Ct 0,105, Cq 0,012, 4S 1,5 Ah 0,055 Ω, 45 A, Pack 180 A, ρ 1,225, dragScale 0,35, Bodeneffekt 0,18, Komponentenmassen (≈ 0,72 kg), IMU-Rauschen und Bias, NAV 100 Hz α 0,42, SBUS 100 Hz | `drone_simulator.html`, `simulator.mjs`, `component_mass_model.mjs`, `propulsion_authority.mjs` |
| Flugregler | komplette Firmware (GAME 25 m/s, 0,5–120 m, Winkel-Modus 32°/40°, PIDs) | `esp32/*.hpp` |
| Höhe GAME | Stickkurve 12 m/s, Totband 0,08, Vorhaltelead nach FC-Gesetz, Standard-Schwebehöhe 1,2 m, Landeablauf | `control_semantics.mjs`, `simulator.mjs` |
| Kameras | FPV vorn 0,102 / oben 0,028, Neigung −15°, FOV 105; Follow 1,65/0,78/0,38/0,10, FOV 52; Third; Stabilisierung 4,8/7,0/7,5/0,18; Auto 5,4 m/2,15 m, Bus ×1,75; Cockpit FOV 80; Jet-Cockpit (0;3,5;1,42) | `camera_settings.mjs`, `camera_stabilization.mjs`, `player_car_mode.mjs`, `jet_mode.mjs` |
| Jet | `JET_SPEC` vollständig (12 t, Trägheit, Aerobeiwerte, Schub 105/185 kN, Fahrwerk, Grenzen, Schwebewerte), FBW-Gesetze, Schleudersitz +28 m/s, Schirm nach 1,1 s, −6,5 m/s, Lenken 4 m/s | `jet_flight_dynamics.mjs`, `jet_mode.mjs` |
| Auto/Bus | `VEHICLE_SPECS` (Masse, Schwerpunkt, Räder, Federung, Momente, Leistung, cdA, Bremsen, Lenkung), Reifenkurve, TCS/ABS, EPS, KI-Fahrer, Einsteigeradius 5,2 m, 115/32 km/h | `world_vehicle_dynamics.mjs`, `vehicle_control_settings.mjs`, `player_car_mode.mjs` |
| zu Fuß | 5,5/8,1/3,6 m/s, 1,68 m, Maus 0,0026 rad/px, Gamepad-Totzone 0,14 | `player_walk_mode_v4.mjs`, `xbox_gamepad.mjs` |
| Welt | Straßenbreiten und -farben, Standardhöhe 8 m, Nebel 0,00085, Tag/Nacht 480 s, Schatten 2048/110 m (mobil 1024/70 m) | `world_city_roads.mjs`, `real_world_bootstrap.mjs`, `stylized_world_style.mjs` |
| Regeln | Wanted-Schwellen 4/9/15/22/30, Schwere je Tat, Einheiten, Radien, Fluchtzeit, Polizeischaden, Trefferchance, EMP | `wanted_system_logic.mjs` |
| Ton-Mix | Drohne 50 %, Fahrzeug 58 % | `audio_settings.mjs` |

## Tests

```sh
kern/games/spawncity/tests/run_tests.sh          # nativ, ohne GL, ASan/UBSan
node kern/games/spawncity/tests/smoke.mjs [dir]  # Browser (Playwright, Chromium mit SwiftShader)
```

Die nativen Tests prüfen Flugregler + Drohnenstrecke (Kalibrieren, ARM, Schweben, Vorwärtsflug entlang
der Nase, Gieren, Steigen, ANGLE-Modus, Höhenstick-Gesetz), Jet (Stehen auf dem Fahrwerk, senkrechter
Start, Schweben, Übergang in den Flügelflug, Hochziehen, Kurve, weiche Landung), Auto und Bus
(Fahrhöhe, Beschleunigen, Bremsen/Rückwärts, Lenkrichtung, KI-Fahrer), MVT-Decoder und
OpenMapTiles-Auswertung, das Streaming (Zeitscheiben, Kollision, Entladen, Ersatzstadt) und die
Wanted-Regeln, den Straßengraph (A*, Pure Pursuit), Treffer und Taten der Leute und Wände bei freier
Bewegung. Der Smoke-Test lädt die Seite zweimal (Desktop mit synthetischen Vektorkacheln über den
echten MVT-Pfad, Telefon mit Touch offline), prüft Passanten, Polizei (Wagen und Drohnen), eine
Rakete mit Explosion und die Minimap, steuert jeden Modus 3 s und macht Screenshots.

## CI

`.github/workflows/ci.yml` führt die nativen Tests (`run_tests.sh`, Job `native`) aus und baut das Spiel
im Job `build` (`build.sh`, `node --check web/host.mjs`). Noch nicht eingerichtet: SpawnCity auf der
Website (die Seite `kern/games/spawncity/web/` nach `_site/` kopieren) und der Playwright-Smoke-Test in
CI (`node kern/games/spawncity/tests/smoke.mjs` nach dem Build, Screenshots als Artefakt).

## Lizenzen und Namensnennung

* Spielcode: Apache License 2.0 wie Kern. Die Firmware unter `firmware/` stammt aus SpawnCity
  (`nostack-dev/spawncity-de/esp32`, Christian Hohlfeld) und steht hier ebenfalls unter Apache 2.0.
* Kartendaten: © OpenStreetMap-Mitwirkende, ODbL 1.0; Vektorkacheln von OpenFreeMap im
  OpenMapTiles-Schema (© OpenMapTiles). Die Namensnennung steht dauerhaft im Spiel (rechts unten).
* Gebaut mit `--commercial`: keine Micro-Duck-Meshes (CC BY-NC-SA) im Modul. Keine Pollen-Meshes,
  keine fremden Bilder oder Musik; alle Töne sind Synth-Patches im Spielordner.
* Kein OSM-Snapshot im Repo: die Umgebung dieses Meilensteins hatte keinen Zugriff auf OSM-Quellen,
  im SpawnCity- und im Kern-Repo liegt keiner. Der Offline-Fall nutzt deshalb die prozedurale Stadt.

## Offen / nächste Meilensteine

* Militär (ab hohen Sternen), Tiere, Gangster, Passanten, die zurückschlagen; Fahrer, die beim
  Carjacking sichtbar aussteigen.
* Zerstörung an Gebäuden (Krater, Fassadenspuren), Ragdolls.
* Mehrspieler: Sprachchat; Polizei und Passanten nur lokal (wie in SpawnCity).
* Gelände (DEM), Wasser mit Auftrieb (die Kacheln liefern Wasserflächen, aber ohne Tiefe), Innenräume,
  echte Punktlichter der Laternen (heute leuchten nur die Köpfe), GPS-Start.
* Verkehr: Vorfahrt und Ampeln (heute bremsen die Fahrer vor jedem Straßenende und vor Hindernissen),
  mehr Fahrzeugtypen.

## Später in die Engine (gebaut im Spielordner)

1. **Wheel-Joint mit Lenkung** in `kern::physics::JointSettings` (Box3D kann es: `enableSteering`,
   Lenkmotor, Lenkanschläge) – dann kann das Auto auf echte Radkörper wie in SpawnCity zurück.
2. **Fixed-Step-Callback in `GameWorld`** (vor/nach jedem Physikschritt), statt des Logic-Knotens.
3. **Possession zur Laufzeit** (`GameWorld::possess(actor)` / Pawn verstecken), statt
   `set_alive(false)` + `set_mode(simulate)`.
4. **MVT-Decoder und Kachel-Streaming** (`mvt.*`, `city_stream.*`) als `kern/geo`-Bausteine,
   inklusive zeitgeteiltem Hochladen und Kollisionsaufbau.
5. **Eigene Imports im App-Host** (z. B. Audio-Ausgabe, Gamepad) – heute fragt die Seite per Export ab.
6. **Heightfield-Kollision** in `kern::physics` (Box3D hat `b3CreateHeightFieldShape`).
7. **Kaskaden-Schatten und LOD/Impostor** für große Sichtweiten (Jet).
8. `build.sh` sollte Header außerhalb von `kern/include`/`kern/src` in den Cache-Schlüssel nehmen.
