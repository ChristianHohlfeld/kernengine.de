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
| `mp=host&room=NAME` / `mp=join&room=NAME` / `mp=auto` | Mehrspieler: hosten / beitreten / beitreten, sonst hosten (Standard: Tabs eines Browsers über BroadcastChannel) |
| `net=public` | dazu: öffentlich wie spawncity.de – WebRTC-Datenkanal, Signalisierung über SpawnCitys öffentliche MQTT-Broker (Ende-zu-Ende verschlüsselt), solange kein Kanal steht über die Broker selbst; ohne `room` die eine öffentliche Lobby |
| `relay=wss://…` | dazu: über das Kern-Relay (`tools/net_relay.mjs`) zwischen Rechnern |
| `ff=1` | Host: Friendly Fire von Anfang an an (SpawnCity-Standard: aus; „der erste Spieler bestimmt“) |

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

Mehrspieler (alle Modi; `voice_control_bindings.mjs`, `ping_marker.mjs`, `world_map_full.mjs`):

| Aktion | Tastatur/Maus | Gamepad | Touch |
|---|---|---|---|
| Mikrofon an/aus (Sprachchat in 70 m) | **H** | **R3** antippen (beim Loslassen) | 🎤 MIC |
| Sprachchat stumm | **K** (in der Drohne ist K Motoren aus) | **R3 + Y** | 🔈 |
| Überall hörbar (Funk) | – | – | ÜBERALL |
| Ping-Marker (gleicher Ort: entfernen) | **Z** / mittlere Maustaste | **R3 + D-Pad →** | 0,6 s lang drücken |
| Große Karte | **M**, Esc schließt | **Menu**; dort LS schieben, LT/RT Zoom, A Wegpunkt, X Ziel löschen, Y Suche, B schließen | KARTE; Tippen = Wegpunkt |
| Abstimmung JA / NEIN | **J** / **N** | – | JA / NEIN |
| Öffentliche Lobby | – | – | MULTI |
| Einladen (QR-Code, Teilen) | – | – | TEILEN |

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
    net_session.* Mehrspieler-Protokoll: Kerns Client/Server über HostTransport, Posen 30 Hz, der Host
                  als Autorität (Treffer, Kills, Rang, Regeln, Abstimmung, Spawn-Anker), Effekte,
                  Pings und Sprache über den Host
    mp_rules.*    reine Mehrspieler-Logik: Posen-Zeitleiste (Jitter-Puffer), Farben, Schaden je Waffe
                  und Zone, Regeneration, Respawn, Gruppen-Spawn, Abstimmung, µ-law-Sprache mit
                  Abstands-Lautstärke, Routen-Hinweise, Karten-Rand, Pings
    qr_code.*     QR-Encoder (Byte-Modus, Stufe L) für den Beitritts-Link
    mp_art.*      Pings, fremde Leuchtspuren und Explosionen, Farbakzente fremder Drohnen, Schutzblase
    game_net.cpp  Mehrspieler im Spiel: eigene Pose, die anderen aus ihren Zeitleisten (auch
                  geparkte Drohnen und Jets), Treffer, Tod/Respawn, Sprache, Pings, Karte/Navigation, HUD-Daten
  web/            index.html + host.mjs (dünner Host, nutzt kern/web/app.mjs, kern.mjs, studio/game.mjs unverändert)
                  + mp_host.mjs (Transporte, Mikrofon, Mehrspieler-HUD, große Karte, Teilen/QR)
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
* **Mehrspieler** (`vs_multiplayer.mjs`, `lan_vs.mjs`, `vs_pose_sync.mjs`, `vs_combat_presentation.mjs`,
  `vs_group_spawn.mjs`, `game_rules_sync.mjs`, `mp_fun_hud.mjs`): jeder simuliert seine Welt in der Stadt
  des Hosts (Kartenursprung im Willkommen). Posen alle 33 ms: der gesteuerte Körper, die Person selbst,
  dazu geparkte Drohne und Jet (alle sehen die abgestellten Fahrzeuge der anderen). Darstellung über
  SpawnCitys Zeitleiste auf der Uhr des Senders: Uhrversatz als gleitendes Minimum, 90 ms Verzögerung,
  Interpolation, höchstens 100 ms Extrapolation, dann Halten; Sprung > 35 m setzt neu an.
  Die **Autorität ist der Host** (bei SpawnCity der älteste Teilnehmer): er prüft jede Schadensanfrage
  (Friendly-Fire-Regel – Standard aus –, 2,5 s Spawn-Schutz, schon am Boden), schickt den genehmigten
  Treffer an das Opfer und eine Trefferbestätigung an den Schützen, zählt Kills/Tode (Killfeed, Tafel),
  gibt Beitretenden einen Spawn-Anker in der größten Gruppe (65 m, Medoid; Ring 7/10/13 m) und führt die
  einstimmige Regelabstimmung (30 s, jeder Spielerwechsel bricht ab). Schaden je Runde wie SpawnCity
  (MP 13, Glock 26, Sniper 95, Fäuste 12, sonst 20; Kopf ×3, Beine ×0,65), Explosionen mit ihrem
  Flächenschaden. Abweichung: das Opfer wendet den genehmigten Treffer auf Kerns Soldaten an (Kerns Tod
  und Respawn bleiben lokal) und meldet seinen Tod der Autorität; die Lebenspunkte gehen in der Pose mit.
  Nach dem Tod Respawn 3–5 m vom Ort (Kern: 3 s), Regeneration nach 5 s ohne Schaden 12 HP/s.
  Effekte: Leuchtspuren (210 m/s, Farbe des Schützen), Explosionen (Blitz, Partikel, Ton, Wackeln),
  Einschläge. Polizei und Passanten bleiben lokal (wie in SpawnCity). Ein Kill an einem Mitspieler ist
  eine Tat (`Crime::player`).
* **Verbindung** (`lan_vs.mjs`, `direct_udp_peer.mjs`, `mqtt_data_relay.mjs`, `share_button.mjs`,
  `qr_pairing.mjs`): die Bytes laufen über `sc_net_push`/`sc_net_take`; `web/mp_host.mjs` trägt sie
  per BroadcastChannel, Kern-Relay oder – wie spawncity.de – öffentlich: WebRTC-Datenkanal (ungeordnet,
  ohne Wiederholung, also UDP-artig; STUN Cloudflare/Google), Signalisierung über SpawnCitys öffentliche
  MQTT-Broker (shiftr.io, EMQX, Mosquitto, HiveMQ), jede Nachricht ECDH-P-256/AES-GCM-versiegelt;
  bis ein Kanal steht, gehen die versiegelten Datagramme über die Broker. MULTI führt in die eine
  öffentliche Lobby (beitreten, sonst hosten). TEILEN zeigt den Beitritts-Link als QR-Code (in C++
  erzeugt) und öffnet das Teilen-Menü des Systems. SpawnCitys zweiter Weg über Nostr-Relays
  (`nostr_data_relay.mjs`) ist nicht nachgebaut: er braucht Schnorr-Signaturen auf secp256k1, die der
  Browser nicht mitbringt (eine eigene Krypto-Bibliothek wäre nötig); der MQTT-Weg deckt dasselbe ab.
  Fällt der Host weg, endet die Runde (SpawnCitys Autoritätswechsel zum nächstältesten ist offen).
* **Sprachchat** (`proximity_voice.mjs`, `voice_codec.mjs`): das Mikrofon öffnet die Seite erst, wenn
  der Spieler es einschaltet (H / R3 / Knopf), und schließt es beim Ausschalten. C++ teilt die Samples
  auf 8 kHz herunter (Schrittweite), µ-law, 640 Samples (80 ms) je Paket; der Host leitet nur an Spieler
  in 70 m weiter (oder an alle bei „überall“). Beim Hörer rechnet C++ die Lautstärke aus den Posen
  (1 bis 5 m, 0 ab 70 m; „überall“ ≥ 0,85), plant jedes Paket 30–150 ms voraus und mischt es in die
  Audioausgabe des Spiels. Der Sprecher bekommt 🎙 am Namensschild.
* **Karte und Navigation** (`world_map_full.mjs`, `nav_ar_route_math.mjs`, `nav_marker_timeline.mjs`,
  `nav_route_request_gate.mjs`, `map_peer_edge.mjs`, `mobile_peer_marker_layout.mjs`, `ping_marker.mjs`):
  Namensschilder (P1…, Entfernung, Leben; außerhalb am Rand mit Pfeil; auf Telefonen weg von den
  Sticks), Pings (ein Marker je Spieler, 90 s, gleicher Ort entfernt ihn), große Karte aus den gestreamten
  Straßen mit Mitspielern (außerhalb am Rand), Pings, Jet, Ziel und Route; Wegpunkt per Tippen, Suche über
  Photon. Die Route rechnet C++ über den Straßengraph (A*); reicht er nicht bis zum Ziel, holt die Seite
  eine Straßenroute bei routing.openstreetmap.de (nur die neueste Anfrage zählt). Führung: eine Zeile
  „↗ 1,2 km · Ziel · rechts in 90 m“ und ein Chevron, wo die Route hingeht; Ankunft in 25 m löscht das Ziel.

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

Mehrspieler-Testbefehle (`SpawnCity.command`): `net remotes`, `net hit N` (Runde auf den ersten
Mitspieler), `net blast`, `mp ff on|off`, `mp vote yes|no`, `mp state`, `voice mic|mute|all|tone N|state`,
`ping [x y]`, `bigmap open|close|toggle|follow R|X Y R`, `nav set X Y Name | geo LON LAT Name | clear`,
`qr TEXT`.

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
Bewegung. Die Mehrspieler-Tests prüfen die Posen-Zeitleiste, Schaden/Regeln/Abstimmung, µ-law und
Sprach-Wiedergabe, die Routen-Mathematik, den QR-Encoder und drei Sitzungen (Host + zwei Clients) über
eine Schleife: Friendly Fire aus, Abstimmung, genehmigter Treffer, Kill und Tafel, Spawn-Schutz, Effekte,
Pings, Sprache nur in Reichweite. Der Smoke-Test spielt zuerst zwei Tabs (BroadcastChannel): sehen sich,
Spawn-Anker, Farben, Namensschild, Friendly Fire aus/Abstimmung/Treffer/Kill/Respawn, Explosion, Ping,
Sprache (Testton), QR-Code, große Karte mit Ziel. Danach lädt er die Seite zweimal (Desktop mit synthetischen Vektorkacheln über den
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
* Mehrspieler: Autoritätswechsel, wenn der Host geht (heute endet die Runde); Nostr als zweiter
  öffentlicher Weg; synchronisierte Gebäudezerstörung und Atombombe (`world_sync.mjs`) – Kern-SpawnCity
  hat beides noch nicht, der Fx-Kanal ist dafür vorbereitet; Mikrofon im Headless-Test nicht prüfbar
  (der Test spricht mit `voice tone N`). Polizei und Passanten bleiben lokal (wie in SpawnCity).
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
