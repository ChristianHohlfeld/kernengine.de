# SpawnCity auf Kern

SpawnCity als natives C++20-Spiel gegen die Kern-API: WebAssembly aus `kern/build.sh`, WebGL2-Rendering
und Box3D über Kern. Kein three.js, keine Spiellogik in JavaScript. Die Seite ist ein dünner Host
(Canvas, Eingabe-Events, Kartenkacheln holen, Audio ausgeben, HUD zeichnen).

Vorlage für Inhalt, Werte und Verhalten ist das laufende SpawnCity (`nostack-dev/spawncity-de`, Commit
`9108537`, `sim/*.mjs`, `esp32/*.hpp`). Die Zahlen stehen mit Quellenangabe im Code.

Stand: **Meilenstein 3 (Kampf und Ereignisse)** – dazu Militär, Flak, Kampfjets und Luftschlag, Passanten, die
zurückschlagen, Zombie-Nächte, Sandbox-Gegner, Punkte und Aufträge, Rennstrecke. Meilenstein 2: alle Modi, dazu die Stadt mit Leuten, Polizei und Fahndung, das ganze
Waffenset mit Explosionen und Fahrzeugschaden, Mehrspieler, Minimap und Straßenlaternen.
**Meilenstein 3 (Welt, Branch `sc-m3-world`):** echtes Gelände (Höhenkacheln), siehe „Welt (M3)“ unten.

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
| `dem=0` | ohne Höhenkacheln (flaches Gelände) |
| `demtiles=URL-Vorlage` | andere Terrarium-Höhenquelle (Tests), `{z}/{x}/{y}` |
| `mute=1` | ohne Ton |
| `mp=host&room=NAME` / `mp=join&room=NAME` / `mp=auto` | Mehrspieler: hosten / beitreten / beitreten, sonst hosten (Standard: Tabs eines Browsers über BroadcastChannel) |
| `net=public` | dazu: öffentlich wie spawncity.de – WebRTC-Datenkanal, Signalisierung über SpawnCitys öffentliche MQTT-Broker (Ende-zu-Ende verschlüsselt), solange kein Kanal steht über die Broker selbst; ohne `room` die eine öffentliche Lobby |
| `quality=0…4` | Grafikstufe fest (0 minimal … 4 maximal), sonst automatisch (Desktop startet bei 4, Telefon bei 2) |
| `mp=host&room=NAME` / `mp=join&room=NAME` | Mehrspieler: hosten / beitreten (Tabs eines Browsers über BroadcastChannel) |
| `relay=wss://…` | dazu: über das Kern-Relay (`tools/net_relay.mjs`) zwischen Rechnern |
| `ff=1` | Host: Friendly Fire von Anfang an an (SpawnCity-Standard: aus; „der erste Spieler bestimmt“) |

## Steuerung

Exakt wie spawncity.de auf jedem Gerät: die vollständige Tabelle (Tastatur/Maus, Laptop-Touchpad,
Xbox-Controller, Touch; zu Fuß, Drohne, Auto/Bus, Jet, Fallschirm, Menüs, Luftschlag, Karte, Voice) mit
Quelle je Zeile steht in [CONTROLS.md](CONTROLS.md), dort auch die behobenen Abweichungen.
Kurzfassung:

| Modus | Tastatur/Maus (Pointer-Lock per LMB/RMB) | Xbox | Touch |
|---|---|---|---|
| überall | Esc Hauptmenü-Frage, Tab Menü, M Karte, P MULTI, Z/MMB Ping, H/K Mikro/stumm, L GPS LIVE, 4/J Luftschlag zielen (J/Enter/LMB los, RMB/Esc ab) | D-Pad ↑ Luftschlag, → Ping, ↓ Fuß/Drohne, X ein/aus, VIEW Kamera (halten: Karte), MENU Menü, R3 Mikro, R3+Y stumm | Minikarte ziehen = Kopf, 2× tippen = Karte |
| zu Fuß | WASD, Shift, Leertaste (2× Jetpack), LMB Feuer, RMB zielen, 1 2 3 5 6 7 8, Q/Rad Waffe, T halten Granate, G EMP, E/F einsteigen, V Drohne | LS, RS, RT Feuer, LT zielen, A Sprung, L3 Sprint, LB Granate, Y/←→ Waffe, B EMP | Sticks, Feuer am Finger, JUMP, Waffe, SCOPE, GRANATE, STRIKE, ⇄ DRONE, 🚗/✈, EMP |
| Drohne | WASD, Pfeile, Maus (Rate), RMB Blick, Leertaste/Shift Höhe, LMB Feuer, R ARM, C Kamera, 1/2, Q/Rad, V | LS, RS, RT−LT Höhe, RB Feuer, A ARM, Schema AIM: LB Blick | Sticks, Höhenschieber, Feuer am Finger, Waffe, ARM, STOP, ⇄ FPS, STRIKE, EMP |
| Auto | W/S, A/D, Leertaste Handbremse, Maus Kopf, C Kamera, + − Rad Zoom, E/F aus, N/B Radio | LS lenken, RT/LT, RS Kopf, B Handbremse | LENKEN / GAS, UMSCHAUEN, ↗, CAM, HANDBREMSE, zwei Finger Zoom |
| Jet | W/S, A/D, Pfeile/Maus, Leertaste/LMB Kanone, R RAKETE wählen, B Bombe, V Schweben/Flug, C, E/F | LS, RS, RT Kanone/Rakete, LT RAKETE, RB Bombe, LB Schweben/Flug | STEIGEN / SCHUB, KNÜPPEL, KANONE, Tipp-Feuer, ⇄ FLUG, RAKETE, BOMBE, CAM, EJECT (650 ms halten) |
| Fallschirm | Pfeile / Maus | RS | rechter Stick |

Befehle ohne Taste im Original: `race` (Rennstrecke), `contract` (nächster Auftrag), `dronemode`
(GAME/ANGLE), `scheme aim|classic` (Xbox-Schema), `strike aim|go|cancel`, `look set|release|keep`,
`zoom in|out`, `device ...` (die Seite meldet Gerätewechsel), `controls` (Zustand als JSON).

Mehrspieler (alle Modi; `voice_control_bindings.mjs`, `ping_marker.mjs`, `world_map_full.mjs`):

| Aktion | Tastatur/Maus | Gamepad | Touch |
|---|---|---|---|
| Mikrofon an/aus (Sprachchat in 70 m) | **H** | **R3** antippen (beim Loslassen) | 🎤 MIC |
| Sprachchat stumm | **K** | **R3 + Y** | 🔈 |
| Überall hörbar (Funk) | – | – | ÜBERALL |
| Ping-Marker (gleicher Ort: entfernen) | **Z** / mittlere Maustaste | **D-Pad →** | 0,6 s lang drücken |
| Große Karte | **M**, Esc schließt, **/** Suche | **VIEW** halten; dort LS schieben, LT/RT Zoom, ←→ Zoom-Stufe, A Wegpunkt, X Ziel löschen, Y Suche, R3 mir folgen, B/VIEW schließen | Minikarte 2× tippen |
| Abstimmung JA / NEIN | Knöpfe JA / NEIN | A / B | JA / NEIN |
| Öffentliche Lobby | **P** | – | MULTI |
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
    terrain.*     Gelände (M3): Terrarium-Höhen → Bare-Earth-Raster, Wasserbecken, Krater, Bodenkacheln
                  (Render-LOD mit Schürzen, Box3D-Dreiecksnetze mit Überlappung), eine Höhenfunktion für alles
    game_world.*  Welt (M3): HUD/Befehle des Geländes; Wasserphysik, Spuren, Verkehrsregeln, Tiere usw.
    life.*        Straßenleben (M3): Kühe auf den Wiesen, Taubenschwärme, Vögel über den Dächern, Bäume,
                  die Laufgruppe, Straßenstimmen (Rufe, Hupen, Schreie) - reine Simulation ohne GameWorld
    ragdoll.*     Ragdolls (M3): 11 Box3D-Körper mit begrenzten Gelenken und Gelenkreibung, Aufstehen
    interiors.*   begehbare Gebäude (M3): Stockwerke, Treppenhaus bis aufs Dach, Möbel, Fenster, Türen,
                  eigene Box3D-Kollision des aktiven Gebäudes
    life_art.*    Zeichnen des Straßenlebens (instanzierte, getönte Grundkörper)
    game_life.*   Leben im Spiel (M3): füttert life/ragdoll/interiors, Klänge, Sprache, HUD, Testbefehle
    city_stream.* Kachel-Streaming: Decodieren → BuildingBuilder/RoadBuilder → Meshes → Box3D-Kollision,
                  alles mit Zeitbudget; Entladen ferner Kacheln; Fahrspuren für den Verkehr
    vehicle_art.* prozedurale Fahrzeugmodelle, instanziert
    sound_fx.*    Synth-Patches (Rotor, Motor, Turbine, Jet-Röhren, Wind) auf Kerns Mixer
    controls.*    Steuerungs-Mathematik nach SpawnCity (Totzonen, Kurven, Raten, Latches, Granate, Geräte)
    touch_play.*  Touch-Steuerung aller Modi (Sticks, Feuer am Finger, Höhenschieber, Knöpfe nach
                  edge_hud_layout.mjs; Overlay = Kerns TouchOverlay)
    game_controls.* Eingabe je Modus, Pad-Aktionen, Luftschlag-Zielen, Kopfblick (CONTROLS.md)
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
    settings.*    Einstellungstabelle (Werte, Bereiche, Hilfe aus SpawnCity), `set`/`get`/`settings schema`
    gps_live.*    LIVE-GPS-Filter (Sprünge verwerfen, Vorhersage, Tätigkeit)
    equipment.*   Jetpack, Flug-Logbuch, MusicSynth (Titelmusik, Autoradio)
    nostr.*       SHA-256, secp256k1, BIP-340-Signatur für das Nostr-Ereignis der Ideen-Karte
    game_ui.*     Menü-/Einstellungs-Seite des Spiels: Anwenden der Einstellungen, Pause, RESET, Startort, LIVE GPS
    game_equipment.cpp  Jetpack am Soldaten, Logbuch, Radio, Geräuschkulisse

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
    combat_rules.* reine Kampf-/Ereignisregeln (Militär-Eskalation, Flak-Zellen, Luftschlag-Timing,
                  Temperamente, Zombie-Runden, Sandbox-Gegner + Chaos-Punkte, Aufträge/Risikobank, Rennstrecke)
    combat_art.*  prozedurale Modelle: Kampfhubschrauber, Panzer, Flak, Kampfjets, Bomben, Tore,
                  Leuchtspur, Mündungsfeuer, Luftsprengpunkte, Feuerbälle, Trümmer (instanzierte Grundkörper)
    game_combat.h Zustand von Kampf und Ereignissen (ein Member `combat_` in Game)
    game_military.cpp Militär (Hubschrauber, Panzer, Trupp), Flak, Kampfjets und Luftschlag, Treffer/Explosionen
    game_events.cpp   Passanten, die zurückschlagen, Zombie-Nächte, Sandbox-Gegner, Punkte/Aufträge,
                  Rennstrecke, Auto-Trümmer, Scope/Headshot, HUD-Teil und Testbefehle
  web/            index.html + host.mjs (dünner Host, nutzt kern/web/app.mjs, kern.mjs, studio/game.mjs unverändert),
                  ui.mjs (Menüs, nur DOM), manifest.webmanifest + icon.svg (installierbare App)

    guided.*      GUIDED-Lenkrakete als reine Logik (Zielwahl am Fadenkreuz, Reiseflug, Sturzflug,
                  2,2 g, Zünder 2,4 m, Ausweichen durch Sprinten); game_guided.cpp setzt sie in die Welt
    combat_detail.* kritischer Schaden (Auto/Bus/Polizeidrohne/Drohne), Geschossimpuls p = m·v,
                  Abstimmung über Spielregeln (einstimmig, 30 s); game_details.cpp wendet sie an
    combat_audio.* SpawnCitys Kampf-Klangbank als Synthese (Schuss, Pistole, Treffer, Crack, Schaden,
                  Schrei, Explosion, Schritt, Abprall, Belohnung, Fehlschlag), als „sc:<art><n>“ in Kerns SoundBank
    perf.*        Frame-Zeit-Messung (Logik, Physik, Stadt, Zeichnen; Kerns Render-CPU/GPU) und die
                  automatische Qualitätsstufe (Leiter aus 5 Stufen, Grenzen im Header)
    look.*        Kamera-Wackeln, FPV-Stabilisator, Speed-Lines, Tag/Nacht-Licht, Licht-Umrechnung,
                  Blaulicht-Takte (reine Modelle, in den nativen Tests)
    game_look.*   Look im Spiel: Qualität anwenden, eigene Tageszeit, Punktlichter, Speed-Lines,
                  eigene Ego-Modelle für Werfer/Fäuste/Messer, HUD-Teil und Befehle (`look_` in Game)
    spawn_guard.* Spawn-Regeln: nichts entsteht im Blick eines Spielers, keine überlappenden Fahrzeuge
    gaps_rules.*  reine Regeln der in sc-m3-gaps geschlossenen Lücken (Sirenen, Polizei-Anflug,
                  Rauch des kritischen Schadens, Knacken beim Tod, Darstellungs-Glättung der Drohne)
    lamps.*       Straßenlaternen als reine Regeln (Platz, Schuss, Explosion, Auto)
    game_gaps.*   wendet sie an (`gaps_` in Game); game_lamps.cpp die Laternen
  web/            index.html + host.mjs (dünner Host, nutzt kern/web/app.mjs, kern.mjs, studio/game.mjs unverändert)
                  + mp_host.mjs (Transporte, Mikrofon, Mehrspieler-HUD, große Karte, Teilen/QR)
                  + gaps_hud.mjs (Funkenstern am Einschlag, Polizei-Anflug im HUD)
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
  Gebäude (Wände + Dach). Boden: das Gelände (`terrain.*`, siehe „Welt (M3)“). Ohne Netz baut das Spiel eine
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
* **Kampf und Ereignisse** (Meilenstein 3, `game_military.cpp`, `game_events.cpp`): Ab fünf Sternen,
  40 s gehalten bei Heat ≥ 36 (oder Heat ≥ 55), folgt ein 15-s-Alarm; danach übernimmt das Militär:
  ein Kampfhubschrauber kreist (72 m Radius, MG-Salven zu 7), ein Trupp Soldaten rückt über den
  Straßengraph vor (Dreiersalven, höchstens drei Salven je 1,4 s), ein Panzer fährt A* über die Straßen
  und schießt Granaten (130 m/s, 6 m). Bei ≤ 3 Sternen zieht es ab. Soldaten, Zombies und Gegner sind
  Leute (`People::Kind::soldier/zombie/enemy`, von ihrem Besitzer bewegt), Hubschrauber und Panzer
  leichte Agenten mit Box3D-Strahlen. Flak steht wie in SpawnCity per Hash in 700-m-Zellen
  (32 %), schießt nur auf Drohne/Jet über 18 m AGL (30 Schuss/s, Vorhalt, Luftsprengpunkte 7 m).
  Kampfjets fliegen alle 75–150 s in 55–85 m vorbei (Ton beim Eintreffen des Schalls, Kamerawackeln),
  der Luftschlag (J) legt 10 Bomben (17 m / 170) über 130 m. Aggressive Passanten (6 %) schlagen
  zurück (Faust 8, Messer 16, wenige mit Pistole 10). Nachts kommen Zombies in Runden (6 + 4·Runde,
  ab Runde 3 Sprinter). Ab einem Stern kommen Sandbox-Gegner zu Fuß (COP, SWAT, HEAVY mit Rakete).
  Punkte: Chaos-Score mit Combo (`sandbox_game.mjs`) und Aufträge mit Risikobank, die erst beim
  Entkommen gutgeschrieben wird (`gameplay_fun_logic.mjs`).
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
* **GUIDED und Kampfdetails** (`game_guided.cpp`, `game_details.cpp`): Ein Druck wählt, was dem
  Fadenkreuz am nächsten liegt (70 px, bis 600 m: Leute, Autos/Busse, Polizeidrohnen, Mitspieler;
  sonst den Zielpunkt) und startet sofort: 0,32 s fällt die Rakete von der Schiene, dann Reiseflug auf
  max(Start + 8 m, Ziel + 28 m) und ≥ 18 m über Grund mit 34 m/s, ab 70 m Sturzflug; ein sprintender
  Fußgänger (≥ 5,8 m/s für 0,35 s) schüttelt sie ab. Kritischer Schaden nach
  `critical_damage_logic.mjs`: Bus 3,1 s statt 2,45 s, ein weiterer Treffer verkürzt auf 0,62/0,72 s,
  Polizeidrohnen bei 34 % (2,2 s), die eigene Drohne bei 28 % (2,05 s). Kugeln geben Box3D-Körpern
  ihren Impuls (9 mm 2,9 N·s, 5,56 mm 3,8 N·s, .50 BMG 41 N·s, `ballistics.mjs`). Kampfklänge sind
  SpawnCitys synthetisierte Bank (`combat_audio_bank.mjs`), beim Audio-Start einmal berechnet.
* **Mehrspieler:** wie SpawnCity: jeder simuliert seine Welt, die Spieler tauschen Posen; der Host
  verteilt sie und gibt neuen Spielern seinen Kartenursprung (alle streamen dieselbe Stadt). Treffer auf
  andere Spieler gehen an deren Rechner, der sie anwendet. Polizei und Passanten sind lokal.

## Welt (M3)

* **Gelände** (`terrain.*`, Vorlage `terrain_elevation.mjs`, `terrain_craters.mjs`, `terrain_tiles.mjs`,
  `terrain_surface.mjs`): Quelle wie auf spawncity.de die öffentlichen **Terrarium-Höhenkacheln**
  (Mapzen „Terrain Tiles“ auf AWS Open Data, SRTM/EU-DEM/3DEP u. a.; Namensnennung im Spiel rechts unten).
  Die Seite holt die PNGs, die das Spiel anfordert, und gibt die RGBA-Pixel weiter (`sc_dem_requests`,
  `sc_dem_data`, `sc_dem_failed`); C++ decodiert (h = R·256 + G + B/256 − 32768), tastet ein Raster ab
  und macht aus dem Radar-Oberflächenmodell nacktes Gelände (morphologische Öffnung 160 m, zwei Box-Blurs
  30 m, `bareEarth`). Höhen relativ zum Ursprung (z = 0 bleibt am Startpunkt). Abweichung von SpawnCity:
  z13 und 10-m-Raster über 8 km (statt z14/5 m über 2,6 km), weil Kerns Kachelring ≈ 5 km breit ist;
  nach der 160-m-Öffnung trägt die feinere Stufe keine Information mehr (Quelle ist 30-m-SRTM).
  Neu geladen nach 2 km Weg. Ohne erste Höhen (Dienst nicht erreichbar, `offline=1`, `dem=0`) bleibt
  das Gelände flach – nie erfundene Höhen (wie SpawnCity).
* **Eine Höhenfunktion:** Knoten alle 5 m (Höhe + Krater, Wasserbecken), Dreiecke mit SW–NE-Diagonale.
  Dieselbe Funktion für Box3D-Boden, Bodennetz, Straßen (RoadBuilder drapiert auf ihr), Flächen
  (zerschnitten an Raster und Diagonale wie `drapeTerrainTriangle`), Gebäude (stehen auf dem tiefsten
  Punkt ihres Umrisses, `buildingGroundBase`), Leute, Autos, Spawns, Polizei.
* **Box3D-Boden:** Kern hat keinen Heightfield-Shape. Deshalb je 160-m-Kachel ein statisches
  Dreiecksnetz (32×32 Zellen à 5 m) mit einer Zelle Überlappung und abgesenktem Außenring (5 cm) wie
  `terrain_tiles.mjs` – die Naht ist für beide Kacheln eine innere Kante. Gebaut im Umkreis 420 m um den
  Fokus und 200 m um den Spieler (zeitgeteilt, neues Netz zuerst, dann das alte weg).
* **Bodennetz:** 640-m-Blöcke bis 3,2 km (40-m-Knoten); Blöcke nahe dem Spieler als 160-m-Kacheln
  (5 m nah, 20 m weiter); Schürzen gegen Risse zwischen den Stufen. Straßen und Flächen werden neu
  verlegt, wenn sich der Boden darunter ändert (Wasser einer Nachbarkachel, Krater).
* **Wasser** (`world_water.mjs`, `world_rigid_body_physics.mjs`, `player_walk_mode_v4.mjs`): Seen/Häfen
  (Polygone) und Flüsse/Kanäle/Bäche (Linien, Breite je Klasse 18/12/4,5 m) aus den Vektorkacheln werden
  auf ein 4-m-Raster gelegt; der Boden darunter ist ein Becken 2,6 m tief, die Oberfläche liegt 0,18 m
  unter dem Ufer. Brücken (`brunnel=bridge`) behalten den Boden. Strömung je Zelle aus dem nächsten
  Wasserlauf (Fluss 1,6, Kanal 0,5, sonst 1,1 m/s, bis 30 m neben dem Ufer abklingend). Kräfte im festen
  Schritt (kein Box3D-Eingriff): Auftrieb ρ·g·V·Anteil·0,62 aus dem Box-Volumen, Widerstand
  k = Anteil·(1,6 + 0,9·v)·m·0,35 gegen die Strömung (vertikal ×1,4), Drehdämpfung – für Autos, Bus,
  Polizei, Jet, Fallschirm; die Drohne bekommt wie in SpawnCity keinen Auftrieb, nur Widerstand.
  Zu Fuß schwimmt der Spieler (Augen 0,42 m über der Oberfläche, Wasserwiderstand). Spritzer
  (Partikel + Synth-Klang) beim Eintauchen und bei Explosionen über Wasser (statt Spuren).
  Oberfläche: ein Netz je 320-m-Abschnitt (volle 64-m-Blöcke als ein Viereck, sonst Zeilenläufe),
  Farbe/Rauheit aus `world_water.mjs`. Noch ohne animierte Wellen-Normalen und Ufer-Schaum (bräuchte eine
  eigene Shader-Erweiterung).
* **Krater und Spuren** (`world_marks.*`, Vorlage `terrain_craters.mjs`, `world_impacts.mjs`,
  `facade_marks.mjs`): Große Explosionen (Radius ≥ 8 m: Granaten, Raketen, Bomben) am Boden graben einen
  Krater mit dem Schüssel-und-Wall-Profil von `craterProfile`, auf die Explosion skaliert (Radius 0,45 r,
  höchstens 8 m, Tiefe 0,18 R; SpawnCity kennt Krater nur für die Atombombe mit 210 m) – Box3D-Boden,
  Bodennetz, Straßen und Flächen folgen (höchstens 32 Krater). Dazu ein Brandfleck auf dem Boden (40),
  Ruß an bis zu zwei Wänden in Reichweite, auf die Wand begrenzt (48), und ein Wanddurchbruch je 2-m-Feld,
  der bei jedem weiteren Treffer weiter aufbricht (64), mit Betonbrocken als Box3D-Körpern (2400 kg/m³,
  36 Stück, 25 s). Die Wände kommen aus denselben Umrissen wie die Gebäude (`CityStream::walls_near`).
  Glassprünge in Fenstern fehlen noch: Kerns `BuildingBuilder` legt die Fensterraster im Shader an, das
  Spiel kennt die Scheiben nicht (Einschusslöcher macht Kerns Soldaten-Decal).
* **Verkehr** (`world_traffic.*`, `game_traffic.cpp`): Kreuzungen sind die Knoten der Straßenmittellinien,
  an denen drei oder mehr Arme zusammenkommen. SpawnCity selbst hat keine Ampeln; seine Fahrer bremsen
  für Leute im Fahrkorridor und hupen (`world_procedural_population.mjs` `yieldsToPedestrian`: Reichweite
  6 m, Bus 9 m, + 0,5 s + v²/9, halbe Breite 1,6/1,95 m; `honkCheck`: Spieler nach 2,6 s, Passant nach 7 s,
  dann alle 9–16 s) – übernommen. **Eigene Regeln** (Wunsch für M3): Kreuzungen mit einer Hauptstraße
  (Arm ≥ 9 m, ab secondary) bekommen Ampeln mit zwei Phasen (Achse der breitesten Straße / quer dazu),
  je 14 s Grün, 3 s Gelb, 2 s Alles-Rot (38-s-Umlauf, Versatz je Kreuzung); bei Gelb hält, wer noch bequem
  halten kann (4 m/s²); gebremst wird auf die Haltelinie mit 3,5 m/s². Ohne Ampel: wer in der Kreuzung
  ist, hat sie; die anderen warten am Rand; rechts vor links für gleichzeitig Ankommende (nach 4 s Warten
  fährt man trotzdem – sonst stünden vier Autos ewig). Gezeichnet werden die Ampeln (Mast, Kasten, drei
  Lichter) im Umkreis von 160 m.
* **Carjacking:** Der Fahrer steigt sichtbar aus (1,8 m auf der Fahrerseite, `ejectDriver`) und rennt weg –
  oder bleibt erst 2,6 s stehen und starrt, dann rennt er. Er ist danach ein normaler Passant auf seinem
  Gehweg. (Prügeln wie in SpawnCity gibt es noch nicht: Passanten schlagen hier nicht zurück.)
* **Abschleppdienst** (`tow_service.mjs`): Verkehr, der umgekippt (2,5 s), festgefahren (9 s, nicht beim
  Warten an einer Kreuzung) oder von seiner Straße abgekommen ist (9 m für 7 s), und ausgebrannte Wracks
  nach 20 s bekommen einen gelben Abschleppwagen (höchstens zwei): Start an einem Straßenpunkt 170–320 m
  entfernt und > 140 m vom Spieler, A* über den Straßengraph, Pure Pursuit, Zurücksetzen bei Hängenbleiben,
  Anhaken 1,8 s, Schleppen mit 9 m/s 30 m weiter auf der eigenen Spur, dort aufrecht absetzen (das Auto
  fährt weiter); Wracks nimmt er mit (zurück in den Pool). Danach zurück, weg, wenn weit genug. Eigener
  Box3D-Körper mit dem Fahrzeugmodell der Autos, 110 HP (Explosionen ×1,3), Bett/Ausleger/Haken und
  blinkender Gelbbalken. Übernimmt der Spieler das Auto, bricht der Auftrag ab. Gibt nach 70 s auf.
* Nicht übernommen: `world_traffic_motion_guard_v1.mjs` (richtet in SpawnCity schiebende Autos per Impuls
  gerade, eine Korrektur für SpawnCitys Fahrer-KI; Kerns Fahrer folgen der Spur per Pure Pursuit und
  brauchen sie nicht). `world_traffic_routes.mjs` ist in SpawnCity nur das Einlesen der Routen aus den
  Kacheln – das macht hier `CityStream` (Fahrspuren).

## Leistung und Darstellung (Meilenstein 3)

* **Frame-Zeiten** (`perf.*`): jeder Frame wird in Runden zerlegt – Spiellogik (Eingabe, Stadtleben,
  Kampf), Physik-Schritte (Box3D + Fahrzeugmodelle), Stadt-Streaming, Zeichnen-Vorbereitung – dazu
  Kerns Render-CPU-Zeit und, wenn der Browser den Timer-Query hat, die GPU-Zeit. Im HUD-JSON unter
  `perf.ms`, als Zeile im HUD mit dem Befehl `perf 1` (Menü-Schalter baut die UI).
* **Automatische Qualität** (Vorbild: SpawnCitys „performance first“, `neon_bloom.mjs`,
  `dynamic_lights.mjs`): fünf Stufen (minimal, niedrig, mittel, hoch, maximal) mit Passanten-Budget
  16/30/44/64/80, Schattendistanz 35/50/70/90/110 m, PCF 1/4/4/9/9, AO aus/aus/niedrig-halb/mittel-halb/
  mittel-voll, Laternen-Sichtweite 70–170 m, Leute-Sichtweite 90–220 m, 2–6 Punktlichter, Pixelratio
  ×0,6–1,0. Gleitender Mittelwert der Frame-Zeit (τ 1 s): unter 50 fps für 2 s → eine Stufe runter;
  ab 57 fps für 8 s → eine Stufe hoch; nach jedem Wechsel 3 s Ruhe, 3 s Anlauf nach dem Start. Eine
  Stufe, die zu schwer war, wird 30 s nicht wieder versucht (bei jedem weiteren Scheitern doppelt so
  lange, bis 4 min). Desktop startet bei „maximal“ (die bisherigen Werte), Telefone bei „mittel“ (das
  alte Telefon-Profil) und gehen höchstens bis „hoch“. Die Pixelratio setzt die Seite (`renderScale` im
  HUD-JSON; der Browser besitzt die Canvas-Größe). `quality auto|0…4|Name`, `?quality=N`.
* **Tag und Nacht** (`look.*`, `day_night_sync.mjs`): das Spiel führt die Tageszeit selbst (480 s pro
  Tag, Start 0,40) und setzt Licht, Himmel und Nebel mit demselben Sonnenlauf wie Kerns GameWorld –
  so kann `time 21:30` / `time 0.75` sie setzen. Im Mehrspieler folgt jeder Client der Phase des Hosts
  (sie reist mit der Pose): kleine Abweichung wird halb ausgeglichen, große springt.
* **Punktlichter** (`scene_lighting.mjs` über `dynamic_lights.mjs`, Kerns 6 Slots über `LightPool`):
  die drei nächsten Straßenlaternen bis 70 m ab der Dämmerung (warm, 46/26 m), Scheinwerfer 7 m vor dem
  eigenen Auto, Mündungsfeuer (Drohnen-MG, Bordkanone, Werfer: 55 ms), Leuchtspur-Glanz (45 ms),
  Raketen im Flug, Explosionen (420 ms, Reichweite 4 × Radius), die Jetpack-Flamme, Blaulicht der
  Streifenwagen (110-ms-Takt, `police_ground_units.mjs`) und der Polizeidrohnen (92-ms-Takt,
  `wanted_police_drones.mjs`). Kerns eigene Lichter (Licht-Komponenten, Mündungsfeuer des Soldaten)
  bleiben im Pool. three.js-Stärken (1/d², BRDF/π) werden auf Kerns gefensterten Abfall umgerechnet,
  gleich hell im jeweiligen Bezugsabstand (`kern_intensity`).
* **Kamera-Wackeln** (`camera_shake.mjs`): Trauma 0…1, Abbau 1,35/s, sichtbar Trauma², 0,055 rad /
  0,09 m, 19 Hz; Quellen: Drohnen-MG/Bordkanone (Dauerfeuer bis 0,42), Raketen/Granaten, Explosionen
  nach Abstand und Größe, harte Stöße von Auto und Drohne (ab 2,5 m/s). Zu Fuß wackelt Kerns
  Soldatenkamera selbst (`shake_camera`).
* **FPV-Stabilisator** (`fpv_optical_stabilizer.mjs`): nur in der FPV-Kamera, Position 55 Hz,
  Ausrichtung 45 Hz, Sprung ab 0,35 m / 2,5 m.
* **Speed-Lines** (`speed_lines.mjs`): 26 radiale Striche ab 13 m/s Kamerageschwindigkeit, voll bei
  40 m/s, als ein instanziertes, durchscheinendes Mesh 0,3 m vor dem Auge.
* **Ego-Modelle** (`weapon_models.mjs`, `viewmodel_arms.mjs`): eigene Modelle für BOOMSTICK 40 mm
  (Trommelwerfer mit Warnstreifen, glühender Mündungsring), Raketenwerfer (Rohr, Führungseinheit mit
  rotem Laser), Fäuste (zwei getapte Fäuste, abwechselnde Geraden) und Messer (Klinge mit Hohlkehle,
  Schnitt quer durchs Bild); Ärmel und Handschuh am Griff, Laufwippen, Rückstoß, Hochnehmen nach dem
  Wechsel. Kerns eigenes Ego-Modell wird solange ausgeblendet (über die Renderer-Sichtbarkeit).
* **Neon-Bloom** (`neon_bloom.mjs`): Kerns Bloom ist an und schaltet sich auf langsamen Geräten selbst
  ab; glühende Teile (Laternenköpfe, Blaulicht, Waffenglühen) sind HDR-Farben über 1.
* **Nicht gebaut: 3D nebeneinander** (`stereo_sbs.mjs`): Kerns App-Host rendert genau einen Frame
  nach `update()`; zwei Augen in zwei Bildhälften brauchen eine Engine-Änderung (siehe unten). Die
  Einstellung `cam.stereo` hat deshalb noch keine Wirkung (`look.stereo` im HUD-JSON ist `false`).

## Leben, Innenräume, Ragdolls (M3)

* **Kühe** (`cows.mjs`): jede Wiese (OpenMapTiles-Landcover `grass`, ab 1800 m²) im Umkreis von 420 m
  bekommt eine Herde (2–7, Fellfarbe aus dem Schlüssel der Fläche, höchstens 40). Sie grasen auf einem
  uhrgetriebenen Pfad (alle 24 s ein neuer Fleck im Umkreis von 6 m um den eigenen Platz, 0,55 m/s, Kopf
  unten), muhen ab und zu, scheuen vor Schüssen (Leuchtspur nahe der Herde) und Explosionen. Ein Treffer
  tut weh (100 HP), ein langsames Auto schiebt sie beiseite, ab 8 m/s oder in einer Explosion stirbt sie:
  der 600-kg-Körper fliegt als Box3D-Quader (Auto verliert den übergebenen Impuls). Der Weidepfad läuft
  auf der Weltuhr (wie `Date.now()` in SpawnCity): auf allen Rechnern grasen dieselben Kühe am selben Ort,
  und der Tod einer Kuh geht an die Mitspieler (Fx `cow_death`), die dieselbe Kuh fallen lassen.
* **Tauben** (`pigeons.mjs`): 5 Schwärme (mobil 3) zu 6–12 Vögeln 22–95 m um den Spieler auf freiem
  Boden (nicht in Häusern oder Wasser, Box3D-Strahl); sie picken und nicken, gurren, und fliegen
  gemeinsam auf, wenn man auf 6,6 m herankommt, ein Auto vorbeifährt, ein Schuss (40 m) oder Knall (90 m)
  fällt oder von selbst (25–70 s): Schleifen, dann Gleitflug auf einen neuen Platz 25–55 m weiter.
* **Hunde und Katzen** (`world_procedural_population.mjs`, `animal_audio.mjs`): 20 Tiere (mobil 12) auf
  Kreisbahnen (13–58 m, eigenes Tempo, Schlenker) um den Ankerpunkt des Viertels (neu verankert nach 2,5 km),
  jedes dritte eine Katze, sonst Hunde (Fellfarben aus SpawnCity). Alle zwei Minuten (Weltuhr) ist Tier #1
  die **schwarze Katze**: wer sie anschießt, den greift sie an – faucht und jault, rennt mit 11 m/s heran,
  springt bei 1,6 m und tötet den Spieler, dann schlendert sie davon (1,3 m/s). Die anderen sterben an
  Schuss, Explosion oder Auto (rollen als Box3D-Quader auf die Seite, nach 7 s weg, nach 25 s kommt ein
  neues), Fäuste brauchen drei Schläge (Hund) bzw. zwei (Katze). Jaulen, Miauen, Fauchen, Kreischen als
  Synth-Patches. WELT-Schalter `world.dogs` / `world.cats`.
* **Vögel und Bäume** (`world_liveliness.mjs`): 16 Vögel (mobil 10) kreisen 8–21 m über den Dächern
  (abschießbar: fallen trudelnd, 9 s weg, Kreischen aus `animal_audio.mjs`), Bäume alle 37 m an den
  Straßen, mindestens 2,4 m von Wänden.
* **Laufgruppe** (`jogger_chain.mjs`): nach 60 s, dann alle 170–280 s zehn Läufer in Neongelb im
  Gänsemarsch (1,35 m Abstand, 2,9 m/s) auf dem Gehweg einer echten Straße 35–150 m entfernt, Wenden am
  Ende, nach 95 s weg. Der Vorläufer ruft, die Gruppe antwortet (Sprachausgabe des Browsers, de-DE),
  Stampfen im 0,4-s-Takt. Wer sie umfährt oder trifft, zählt eine Serie (12 s): „JOGGER x2“, die ganze
  Gruppe „KOMPLETTE LAUFGRUPPE!“ mit Jingle.
* **Straßenstimmen** (`street_voices.mjs`): Leute sagen, was sie denken – angerempelt („Hey, pass doch
  auf!“), knapp verfehlt vom Auto, verängstigt (oder ein Schrei aus Synth-Formanten), wütend, wenn sie
  zurückschlagen; Fahrer hupen, wenn der Spieler oder ein Passant vor ihnen auf der Fahrbahn steht. Die
  Zeilen spricht der Browser, die Sprechblase steht über dem Kopf (Position rechnet C++, die Seite zeichnet
  sie: `web/life.mjs`). Drosseln wie im Original (1,4 s global, 6 s je Person, Hupe 1,2 s / 4 s je Auto).
* **Ragdolls** (`world_person_ragdoll.mjs`, `people_impacts.mjs`): Getroffene fallen als Box3D-Körper
  (Becken, Brust, Kopf, Ober-/Unterarme, Ober-/Unterschenkel; Kegel- und Scharniergrenzen,
  Reibungsmotor; Box3D selbst unverändert), Pool 10 (mobil 6). Von einem Auto angefahren, wer noch lebt,
  steht wieder auf („Reverse Ragdoll“: Liegen, Knien, Stehen in 1,15 s) und geht von dort weiter; der
  Wagen verliert den Impuls (75 kg · 0,55). Explosionen stoßen liegende Körper weg.
* **Stöße zwischen Spielern** (`player_push.mjs`): ein Stoß gegen einen Mitspieler geht als Fx `push`
  (Geschwindigkeitsänderung, Schaden, Ziel-Slot) an dessen Rechner, der ihn auf sich selbst anwendet
  (höchstens 40 m/s); heute lösen ihn Explosionen in der Nähe eines Mitspielers aus.
* **Innenräume** (`building_interiors.mjs`): das nächste Gebäude im Umkreis von 30 m wird begehbar
  (eines zur Zeit): Haustür(en) an der längsten Wand (über 260 m² eine zweite gegenüber), das Erdgeschoss
  und alle Stockwerke in der Höhe der Fassadenfenster (Kerns Gebäudenetz: 3,1–3,6 m je Haus, höchstens 12),
  ein Treppenhaus mit Wendeltreppe
  (2 × 1,2-m-Läufe, Podeste 1,3 m) bis aufs Dach mit Treppenhaus-Aufbau, Fensteröffnungen, Deckenlicht,
  Möbel mit Kollision (Empfang in der Lobby, Schreibtisch, Bett, Regal, Sofa, Sessel mit Stehlampe,
  Esstisch mit Stühlen, Küchenzeile, Tisch, Pflanze; Klimageräte auf dem Dach). Die Fensteröffnungen
  sitzen genau dort, wo die Fassade ihre Fenster zeichnet (alle 3 m ab Kantenanfang, 22–78 % der Spalte,
  28–78 % des Stockwerks), mit leicht getönten Glasscheiben (ein transparentes Netz). Die Kollision bringt das
  Gebäude selbst mit; `CityStream::exclude_building` baut solange die Kachel-Kollision ohne dieses Haus
  neu (in Zeitscheiben, der alte Körper bleibt bis der neue fertig ist). Nahe Häuser zeigen geschlossene
  Türen (24 im Umkreis von 95 m). Die Fassade ist von innen nicht gezeichnet (Rückseiten), deshalb zeigen
  die Fensteröffnungen die Stadt; die offene Tür zeigt von außen ein dunkles Türblatt statt eines echten
  Ausschnitts (das Hausnetz ist ein gemeinsames Kachelnetz).
* **WELT-Schalter:** `world.people` schaltet auch die Laufgruppe ab, `world.birds` Tauben und Vögel,
  `world.dogs`/`world.cats` die Hunde und Katzen.
* **Testbefehle:** `life`, `cows`, `cows kill`, `pigeons`, `pigeons spook`, `joggers`, `birds`, `ragdoll`,
  `ragdoll state`, `pets`, `pets kill`, `cat` (die schwarze Katze greift an), `knock` (nächster Passant wird umgestoßen und steht wieder auf), `shout`, `honk`,
  `interior`, `interior go|in|up|roof|door`.

## Menüs, Einstellungen, Ausstattung (Meilenstein 3)

* **Einstellungen** (`settings.*`, `game_ui.cpp`): eine Tabelle im Spiel mit SpawnCitys Werten, Bereichen und
  Hilfezeilen – Maus (Empfindlichkeit 20–300 %, Y invertieren), zu Fuß (Stick-Kurven, Blicktempo,
  Totzone, Invertieren), Drohne (Stick-Kurven, Invertieren, Höchstgeschwindigkeit 5–90 km/h,
  Schwebehöhe), Auto (Lenkung/Gas 55–150 %, 45–180 km/h, rückwärts, Kamera-Abstand/-Höhe), Kamera
  (FPV-Neigung, Sichtfeld, Third-Person-Abstand, 3D nebeneinander), Ton (Hauptschalter, Drohne,
  Schüsse, Effekte, Schritte, Fahrzeuge, Umgebung, Musik) und die WELT-Schalter. Die Seite baut ihre
  Menüs aus `settings schema` und schickt jede Änderung als `set <key> <wert>`; localStorage hält nur
  eine Kopie, die beim Start wieder abgespielt wird (`settings values`, `get <key>`, `settings reset [bereich]`).
* **WELT** (`world_options.mjs`): Leute, Verkehr und Polizei gibt es nicht, wenn sie aus sind (keine
  Körper, keine KI); Jetpack; Friendly Fire (Standard aus, wie SpawnCity: Treffer und Explosionen
  erreichen andere Spieler nur, wenn es an ist); Militär, Zombie-Nächte und Flak schalten die Kampfsysteme
  (`military on|off`, `zombies on|off`, Flak prüft den Schalter selbst). Hunde, Katzen und Vögel hat das
  Spiel auf Kern noch nicht (Tiere: anderer Bereich) – ihre Schalter werden gespeichert und als „bald“ gezeigt.
* **Ansicht und Helfer** (`settings_help.mjs`): 1-m-Raster um den Spieler, Blick halten (das Cockpit
  schwingt nicht zurück), Minimap genordet, Box3D-Kollisionsformen als Drahtgitter (Kerns
  `debug_lines`), Zielhilfe mit Pad und Touch (0–100 %, Standard 55: sanfter Zug zum nächsten Gegner
  innerhalb 6° und 70 m). Im Einstellungsdialog oben „was du steuerst“ (ZU FUSS / DROHNE / AUTO / JET)
  und STARTORT.
* **Menüs** (`web/ui.mjs`, nur DOM): Titelbildschirm (START, WELT, EINSTELLUNGEN, NEU / CHANGELOG;
  automatisierte Browser und `?autostart=1` überspringen ihn, `?menu=1` erzwingt ihn), Pausemenü (Esc,
  MENÜ, Pad-Menütaste, Verlassen des Pointer-Locks), „Zurück zum Hauptmenü?“ (Enter/Y – Esc/N),
  EXIT / MENÜ / RESET oben links, der Moduswechsel unten links mit seinem Ziel („ZUR DROHNE · V“,
  „AUSSTEIGEN · F“, „EJECT · F“; Touch hat ihn im Touch-Layout), Ton- und Musikschalter (im Titel oben
  rechts, im Spiel neben EXIT), HOME SCREEN (Installieren: Chromes Abfrage, auf iOS die zwei Tipps),
  TEILEN (Teilen-Dialog, sonst Link kopieren, sonst ein Post auf X), NEU / CHANGELOG mit den Kern-Meilensteinen
  und darunter dem vollständigen Changelog von spawncity.de. Steuerung der Menüs mit dem Pad (D-Pad, A, B).
  Solange ein Menü offen ist, steht das Spiel (`pause 1`: keine Eingabe, kein Physikschritt, Ton pausiert).
* **RESET** (`game_reset.mjs`, allein): zurück zum Start – Startmodus und Startpunkt, Pistole, volle
  Gesundheit, keine Fahndung, Polizei weg, Fahrzeuge repariert, neue Passanten.
* **Startort** (`start_location.mjs`, `world_location_selector.mjs`, `auto_flight_start.mjs`): der „📍“-Knopf
  im Titel – MEIN STANDORT (GPS, Standard), die Städte-Liste von SpawnCity (+ Konstanz), die letzten fünf
  Orte und eine Suche (Stadt, Straße, Adresse) über Photon von komoot (© OpenStreetMap-Mitwirkende,
  Namensnennung im Suchfenster). Der gewählte Ort gilt nur für diese Browser-Sitzung. START holt einen
  frischen GPS-Fix (nach 12 s die Netzwerkposition; ein genauer Fix bis 60 s danach, der über 300 m
  daneben liegt, verlegt die Welt noch einmal). `?lat=&lon=` in der Adresse ersetzt das GPS. Im Spiel:
  `start <lat> <lon> [name]` streamt die Stadt am neuen Ort und setzt den Spieler dort ab (nicht im
  Mehrspieler – dort gilt die Stadt des Hosts), `origin` meldet den Ursprung.
* **LIVE GPS** (`gps_live.*`, `gps_live_mode.mjs`): Pausemenü → GPS LIVE. Die Seite gibt die Fixes von
  `watchPosition` weiter (`gps fix <lat> <lon> <genauigkeit> [tempo] [kurs]`, Kompass `gps compass`), das
  Spiel filtert sie wie SpawnCity (über 65 m Ungenauigkeit verworfen, Sprünge über 75 m/s verworfen,
  Vorhersage mit konstanter Geschwindigkeit, Neuanfang nach 12 s Lücke, Welt wandert mit ab 40 km) und
  führt den Spieler zu Fuß mit; Tätigkeit aus dem Tempo (steht/gehen/laufen/Rad/Fahrt), Tempo bzw. Pace,
  Strecke, Zeit, Genauigkeit; Ansichten AUGE, FOLGEN, KARTE (`gps view`). Kein Schaden, keine Fahndung.
  Bildschirm bleibt an (Wake Lock). Am Boden der blaue Positionspunkt (Genauigkeitskreis, Ring, Punkt,
  Blickkeil) und die Spurlinie (ein Punkt alle 2 m, höchstens 4000).
* **Jetpack** (`equipment.*`, `game_equipment.cpp`; `player_walk_mode_v4.mjs`, `jetpack_audio.mjs`): WELT-Schalter
  (an). Zweiter Sprung in der Luft zündet (Stoß 0,6 s), Sprung halten fliegt weiter: Steigen bis 8 m/s
  (Beschleunigung g + (8 − vz)·3, höchstens 13,8 m/s²), Luftsteuerung 14 m/s², Tank ≈ 5 s, füllt sich in
  4 s am Boden (in der Luft nach 0,35 s ohne Schub), kurz vor einem harten Aufprall zündet er selbst
  (21 m/s²). Der Schub ist ein Impuls auf Kerns Charakter (75 kg). Ton: Zünd-„Wumm“ (Druckstoß,
  Flammenstoß, Klick), ein oranges Aufblitzen an den Bildrändern, ein Brüllen aus gefiltertem Rauschen mit
  Knistern; die Jetpacks der anderen Spieler hört man in 3-D an ihrer Stelle (Flag `kJetpack`); Tankanzeige unten.
* **Logbuch** (`flight_logbook.mjs`): jeder Drohnenflug von ARM bis DISARM (Dauer, Strecke, Spitze,
  größte Höhe über Grund, Akku Start/Minimum, Grund des Endes, Pfad alle 0,5 s, höchstens 50 Flüge) und
  die Netzereignisse; das Spiel zeichnet auf (`logbook`), die Seite hält die Geschichte in localStorage,
  EXPORT JSON / LEEREN im Pausemenü → LOGBUCH.
* **Musik und Autoradio** (`music_player.mjs`, `car_radio.mjs`): SpawnCity spielt im Menü eine
  Musikdatei und im Auto echte Radiosender der Gegend. Hier ist beides erzeugt (`MusicSynth`: 16tel-Sequencer
  über vier Akkorde, Drums aus Rauschen und Sinus, Bass, Lead, Pad, Echo): die Titelmusik (8-Bit) läuft
  nur in den Menüs, im Auto das Radio mit fünf Sendern (KERN FM Synthwave, BODENSEE BEATS House,
  LOFI LAGUNE, 8-BIT RADIO, NACHTSCHICHT Techno). N / B oder D-Pad →/← wechseln, nach dem letzten
  Sender kommt RADIO AUS; Sendername als Banner; Touch: 📻-Knopf. Lautstärke = MUSIK (Radio mindestens
  55 %, ×0,85; Titel ×0,45 wie SpawnCity). `radio next|prev|on|off`, `music`.
* **Geräuschkulisse** (`immersive_soundscape.mjs`): Stadtrauschen, Vögel am Tag, Grillen in der Nacht
  (Umgebungs-Bus, leiser in der Höhe) und ein dumpfer Schlag bei harten Stößen des eigenen Autos
  (ab 1,8 m/s Geschwindigkeitssprung) mit etwas Kamerawackeln. Schritte und Motor kommen wie bisher
  aus Kerns GameAudio und `sound_fx.*`.
* **Idee fürs nächste Update** (`suggestion_box.mjs`, `nostr.*`): nach 15 min Spielzeit dieser Version fragt
  die Karte im Titel einmal (SENDEN / SPÄTER = 10 min / NEIN DANKE). Wie SpawnCity geht die Zeile anonym als
  signiertes Nostr-Ereignis (Art 30078, Tags `spawncity-suggestion` und Build) mit einem Wegwerf-Schlüssel an
  die öffentlichen Relays relay.damus.io, nos.lol, relay.primal.net und relay-rpi.edufeed.org. SpawnCity
  nutzt dafür `nostr-tools`; hier signiert das Spiel selbst (SHA-256, secp256k1, BIP-340 in `nostr.cpp`,
  geprüft mit den BIP-340-Testvektoren), die Seite öffnet nur die WebSockets.
* **App/Vollbild** (`pwa_fullscreen_policy.mjs`): `web/manifest.webmanifest` (Vollbild, quer) mit
  eigenem SVG-Symbol; `web/sw.js` hält wie SpawnCitys Service Worker eine Kopie für den Start ohne Netz
  (Netz zuerst, nur eigene Dateien); auf Touch-Geräten startet START das Vollbild, außer die installierte
  App läuft schon im eigenen Fenster.

## Parität (sc-m3-gaps)

`PARITY.md` listet alle 250 Module von spawncity.de mit Status und Kern-Dateien. In diesem Schritt
geschlossen (Quellen in Klammern, Werte im Code):

* **Sirenen** (`police_sirens.mjs`): Martinshorn der Streifenwagen (466/622 Hz, 0,66 s je Ton, Säge +
  Rechteck + Dreieck durch einen 1650-Hz-Formanten), steigendes „Whoop“ der Polizeidrohnen
  (760 → 1480 Hz in 0,7 s); je eine Stimme für die nächste Einheit, Pegel 0,06/0,032 ÷ (1 + (d/14)^1,25)
  bis 260 m, Höhenverlust mit der Entfernung (zwei Klänge, hell und tiefpassgefiltert, nach
  5200 − 16·d Hz überblendet – Kerns Mixer hat keinen Filter je Stimme), Doppler aus der
  Abstandsänderung. Ersetzt die alte Heulsirene.
* **Polizei-Anflug** (`wanted_dispatch_guard.mjs`, `wanted_police_pre_guard.mjs`): eine Polizeidrohne
  kommt erst nach der Anfahrzeit (16 s − 1,3 s je Stufe, ≥ 9 s), dann Gnadenfrist
  max(450, 720 − 45·Sterne) ms vor dem ersten Schuss; HUD „POLIZEI-EINSATZ · n DROHNEN IM ANFLUG“.
  Drohnen sehen (und schießen) nur in Feuerreichweite min(35, 28 + 1,4·Stufe) m.
* **Rauch vor der Explosion** (`world_critical_damage_fx.mjs`): flache dunkle Scheiben mit hellem
  Rand, die steigen, wachsen und sich drehen; spät im Countdown Glut. Für Autos/Busse (statt der
  Flammen vor der Explosion), Polizeidrohnen, die eigene Drohne und den Jet.
* **Jet-Schaden** (`jet_mode.mjs damageAirframe`): 400 HP, Schub fällt mit dem Schaden, bei 15 %
  brennt er 12 s, dann zerfällt er (14 m / 180) und nimmt den Piloten mit.
* **Knacken beim Tod** eines Menschen (`world_organic_hit_audio.mjs`), **Funkenstern** am Einschlag
  des Drohnen-MGs (`impact_explosion_overlay.mjs`, die Seite zeichnet ihn am Punkt, den C++ projiziert).
* **Spawn-Regeln** (`spawn_visibility_guard.mjs`, `vehicle_spawn_clearance.mjs`): nichts erscheint
  näher als 24 m an einem Spieler oder in seinem Blickkegel (halbes Sichtfeld + 20°; auch die
  Blicke der Mitspieler), außer ein Gebäude verdeckt es (bis 420 m); Leute ab 150 m (Telefon 125 m),
  Verkehr ab 600 m dürfen auch im Blick ankommen; die ersten 6 s nach dem Start gilt das nicht.
  Fahrzeuge werden nie ineinander gesetzt (Trennachsen-Test, 0,35 m).
* **Kamera** (`camera_collision_guard.mjs`, `fpv_view_height.mjs`, `visual_pose_stabilization.mjs`):
  nie unter dem Boden (+0,12 m), die Verfolgerkamera steigt über das Autodach, wenn eine Wand sie
  heranschiebt; FPV-Auge 4,5 cm höher; die Drohne in follow/third gedämpft gezeichnet.
* **Straßenlaternen** (`street_lamps.mjs`): Platz, Aussehen, Licht wie im Original; ausschießen,
  umknicken (Box3D-Prop), Autos stoppen oder knicken sie, Sync an die Mitspieler, RESET.
* **Kampfjets** (`fighter_jets.mjs`): Triebwerks-Loop je Jet mit Retarded-Time und Doppler (statt
  des einen Knalls); Luftschlag und Überflug gehen an die Mitspieler (Bomben eines Mitspielers sind
  keine eigene Tat). Militär-Leuchtspuren und Panzergranaten sehen die Mitspieler auch.
* **GUIDED** (`guided_missile.mjs`): Lock-Ton, Zündknall, Motorstimme und Rauchspur.
* **Waffen zu Fuß, Werte geprüft** (`gameplay_final_runtime_v2.mjs`, `game_foot_rounds.cpp`):
  Granatwerfer 40 m/s gerade entlang des Blicks, Zünder 2,8 s, prallt nur vom Boden ab (Normale
  z ≥ 0,58; Rückprall 0,58, Dämpfung 0,88, bis 7×), an Wand, Auto, Person geht er hoch; ein Druck,
  solange eine Granate fliegt, zündet sie (Fernzünder, danach 380 ms Pause). Rakete startet mit
  22 m/s, beschleunigt mit 55 m/s² auf 40 m/s und folgt dem Laserpunkt (Fadenkreuz-Treffer bis 420 m,
  höchstens 2,8 rad/s); eine neue Rakete übernimmt den Laser, die alte fliegt ungelenkt weiter;
  Rauchspur alle 25 ms. Schüsse durch eine liegende Granate (0,35 m) zünden sie. Fäuste mit zwei
  Händen (rechts 300 ms / 24, links 250 ms / 16, Kombo binnen 430 ms ×1,35), Messer trifft 130 ms
  nach dem Druck; trifft der Strahl niemanden, trifft es den Nächsten im 40°-Kegel. Handgranate
  4 s Zünder, explodiert in der Hand mit 8,5 m / 125. Keine Magazine, kein Nachladen (das Original
  hat keine Munition). MP trifft bis 180 m, Glock bis 240 m; Glock und Sniper gehen durch bis zu drei
  Körper, der Sniper wirkt erst, wenn die Kugel ankommt (850 m/s).
* **Passanten** (`world_procedural_population.mjs`, `people_impacts.mjs`): 100 HP; ein Treffer zieht
  55 ab (Sniper 200) und wirft den Menschen als Ragdoll um, der zweite nach dem Aufstehen tötet; ein
  Kopftreffer tötet (MP 165, Glock/Sniper 999); wer liegt, ist kein Ziel. Explosionen treffen einmal
  (Abfall > 0,22). Fäuste/Messer: Taumeln, bei ≤ 35 HP zu Boden. Autos: min(260, max(6, (v − 1,5)·8,5))
  ab 2,2 m/s.
* **Fahrzeugschaden** (`walk_world_experience_hotfix.mjs`, `critical_damage_logic.mjs`,
  `police_ground_units.mjs`, `game_vehicle_damage.cpp`): Auto 180 HP / 30 je Treffer, Bus 320 / 26,
  Streifenwagen 300 / 22; kritisch bei 20 % (Streifenwagen 25 %), dann brennt es 2,45 s (Bus 3,1 s,
  Streifenwagen 2,6 s) und jeder weitere Treffer verkürzt; Explosionen geben ceil(3f) bzw. ceil(4f)
  Treffer, Streifenwagen linear; nur Streifenwagen nehmen Crash-Schaden.
* **Jet-Kamera und Schirm** (`jet_mode.mjs`): Verfolger 22 + v/40 m hinter dem Jet, FOV 66 + v/30;
  Schleudersitz entlang der Jet-Hochachse; der offene Schirm sinkt mit gut 12 m/s.
* **Verkehr und Passanten in Originalstärke** (`world_procedural_population.mjs`,
  `game_traffic_far.cpp`): 42 Autos + 8 Busse (Telefon 28 + 5), 100 Passanten (Telefon 68) auf jeder
  Qualitätsstufe. Wie im Original haben Autos nur in der Nähe Physik (unter 150 m an, über 180 m aus);
  weiter weg fahren sie als kinematische Spurfolger. Die KI-Reifen rechnen mit 60 Hz (FIXED_DT 1/60),
  die Kräfte gelten über Kerns 240-Hz-Schritte dazwischen. Gemessen (Headless): Physik 4,65 ms mit
  11 Autos, 8,74 ms mit 50.
* **Pilot** (`player_vitals_runtime.mjs`): 100 HP, jeder Treffer ÷ 1,18, keine Regeneration.
* **EMP** (`wanted_police_drones.mjs`): waagerechter Stoß + Druck nach unten auf 18 kg, Fall mit
  1,35 g, Neustart nach 12 s.
* **Zombie-Barrikaden** (`zombie_nights.mjs`, `building_interiors.mjs`): nachts trägt jede Tür des
  begehbaren Hauses sechs Bretter; Zombies schlagen sich an der Tür (1,15 m davor) Brett für Brett
  durch (je Tür ein Brett alle 3,4 s), steigen 1,8 m hinein und jagen den Spieler im Haus; durch
  intakte Bretter beißt keiner. REPARIEREN · 10 (Knopf oder F, 3,2 m an der Tür) nagelt ein Brett für
  10 Zombie-Punkte wieder an. Testbefehle `barricades`, `repair`.
* **Fahndung** (`wanted_police_drones.mjs reportCrime`): jede Tat bei Fahndung zeigt der Polizei den
  Spieler (letzte Sichtung, Ort), ein neuer Stern spielt den kurzen Rechteck-Chirp.
* **Mitfliegen im Jet** (`jet_mode.mjs`, `jet_seat_table.mjs`, `jet_seat_coordinator.mjs`): F am
  gelandeten Jet eines Mitspielers (9 m, Pilot drin) fragt beim Host den Beifahrersitz an; der Host
  führt die Sitztabelle (nur der Besitzer fliegt, ein Beifahrer, Pilot raus = Beifahrer raus, wer
  geht, verliert seine Sitze). Als Beifahrer sitzt die Kamera im Jet; F steigt aus (über 10 m am
  Schirm). Nachrichten `kSeatReq`/`kSeatState` in `net_session.cpp`.
* Regel-Abstimmung: im Original gibt es genau eine Regel (Friendly Fire, `GAME_RULE_KEYS`); ihre
  Abstimmung läuft schon über das Netz (`Session::propose_rule/vote`), siehe unten.

Testbefehle: `gaps` (Zustand), `canspawn X Y`, `burn N`, `jetdamage N`, `lampbreak 1|2`, `seats`, `board`.

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
| Militär | Heat 36 / 40 s / 55, Alarm 15 s, Ende ≤ 3 Sterne, Trupp 4 (max. 8), HP Soldat 100 / Heli 420 / Panzer 900, Treffer 25 (Panzer 4), Schaden Heli 4/6, Soldat 3/4, Granate 30, Reaktion 0,9 s, 3 Salven je 1,4 s, Trefferchancen, Verstärkung alle 45 s | `military_response.mjs` |
| Flak | Zelle 700 m, 32 %, 1100 m, 18 m AGL, 900 m/s, 30/s, 2,6 s an / 1,7 s aus, Streuung 0,012, 7 m Luftsprengpunkt 14·(1−d/7)+2, HP 60, Sniper 35 / sonst 4 | `flak_battery.mjs` |
| Kampfjets | 255 m/s, 55–85 m, 2600 m Anflug, alle 75–150 s, Luftschlag 30 s, 10 Bomben 17 m / 170 über 130 m | `fighter_jets.mjs` |
| Passanten | 6 % aggressiv, 30 % Läufer; Messer/Pistole; Faust 8, Messer 16, Pistole 10 (Chance 0,52−0,014 d−0,045 v), Abstand 6–14 m | `pedestrian_agents.mjs`, `gangsters.mjs` |
| Zombies | 28 max., HP 100+12·Runde, Treffer 34, Biss 11 alle 1,15 s, 34–70 m, 10/60 Punkte, 9 s Pause | `zombie_nights.mjs` |
| Sandbox | COP/SWAT/HEAVY (Treffer, Tempo, Reichweite, Takt, Salve, Schaden, Genauigkeit, Punkte), Budget 0/1/2/4/6/8, 6 s, Combo 3,2 s × bis 8 | `sandbox_game.mjs` |
| Aufträge | 5 Aufträge, Combo 5,5 s, Overdrive 8 s, Momentum 4, Punkte je Tat, Risikobank, Versicherung 50 %, Drohnenverlust 20 %, Gnade | `gameplay_fun_logic.mjs` |
| Rennen | 8 Tore (×2,25), 3 Runden, Durchflugprüfung, Rundenzeiten | `race_track.mjs` |
| Kampfdetails | Drohnenraketen 31 m/s, 220 m, 950 ms, 8 m / 100, 7,2 s; GUIDED 34 m/s, 2,2 g, 70 px / 600 m, Reiseflug +8/+28/18 m, Sturz 70 m (×1,4 / ×1,12), Zünder 2,4 m, 22 s, Ausweichen 5,8 m/s · 0,35 s; Bus 3,1 s / 0,72 s, Polizeidrohne 34 % 2,2 s / 0,56 s, Drohne 28 % 2,05 s / 0,54 s; Geschossimpuls 9 mm / 5,56 / .50; Auto kritisch 20 %, 2,45 s / 0,62 s; neue Drohne nach 8 s; Trümmer 9 Teile, 6,5 s, Abprall 0,30 / 0,76 | `direct_fire_missiles.mjs`, `guided_balance.mjs`, `guided_missile.mjs`, `ballistics.mjs`, `critical_damage_logic.mjs`, `player_vitals_logic.mjs`, `world_car_explosion.mjs` |

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
Rakete mit Explosion und die Minimap, steuert jeden Modus 3 s und macht Screenshots. `test_life.cpp`
prüft Ragdolls (Fallen, Liegen, Pool, Aufstehen), den Bauplan eines Gebäudes (Stockwerke, Kern, Dach,
Tür in der Kollision, ein Character-Mover läuft die erste Treppe hinauf), Kühe (Treffer, Explosion,
600-kg-Körper), Vögel und die Drosselung der Stimmen; der Smoke-Test die Tiere, einen Ragdoll, das
Aufstehen und das Betreten eines Hauses bis aufs Dach.
Rakete mit Explosion und die Minimap, steuert jeden Modus 3 s und macht Screenshots.
`test_look.cpp` prüft Frame-Zeit-Runden, die Qualitätsleiter (runter unter 50 fps, hoch mit Reserve,
Sperre zu schwerer Stufen, Telefon-Obergrenze, feste Stufe), Kamera-Wackeln, FPV-Stabilisator,
Speed-Lines, die Tageszeit (Licht, `time`-Parser, Mehrspieler-Folgen) und die Licht-Umrechnung/-Takte;
der Smoke-Test die Frame-Zeit-Zeile, die Pixelratio einer festen Stufe, Nacht mit Punktlichtern und
das eigene Messer-Modell.

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
* Höhen: „Terrain Tiles“ (Mapzen, auf AWS Open Data, frei nutzbar mit Namensnennung der Quellen
  SRTM, EU-DEM, 3DEP u. a., siehe https://github.com/tilezen/joerd/blob/master/docs/attribution.md);
  im Spiel rechts unten genannt.
* Gebaut mit `--commercial`: keine Micro-Duck-Meshes (CC BY-NC-SA) im Modul. Keine Pollen-Meshes,
  keine fremden Bilder oder Musik; alle Töne sind Synth-Patches im Spielordner.
* Kein OSM-Snapshot im Repo: die Umgebung dieses Meilensteins hatte keinen Zugriff auf OSM-Quellen,
  im SpawnCity- und im Kern-Repo liegt keiner. Der Offline-Fall nutzt deshalb die prozedurale Stadt.

## Offen / nächste Meilensteine

Der genaue Stand je SpawnCity-Modul steht in `PARITY.md` (Status, Kern-Dateien, Begründung). Offen sind
dort (Stand sc-m3-gaps):

* **Mehrspieler:** Autoritätswechsel zum Nächstältesten, wenn der Host geht (`lan_vs.mjs`; heute endet
  die Runde); Nostr-Relays als zweiter öffentlicher Signalweg (`nostr_data_relay.mjs`; die Signatur
  BIP-340 gibt es schon in `nostr.cpp`); Schnappschüsse von Militär (`mil-snap`/`mil-hit`) und Zombies
  (`zombie-sync`/`zombie-hit`) an die Mitspieler; dieselben Passanten auf allen Rechnern (im Original
  deterministische Agenten, deren Tod per `npc-death` geht).
* **Panzer** als Box3D-Fahrzeug (`military_response.mjs upsertBody kind bus, 9200 kg`), heute ein
  leichter Agent auf dem Straßengraph.
* **Trainingslevel** (`training_test_level_v2.mjs`, `training_showcase.mjs`): die Welt ohne Geodaten ist
  im Original ein fester Hafen mit Straßen, Teich, Brücke, Steg und neun Häusern; auf Kern die
  prozedurale Ersatzstadt.
* **Bodenfarbe aus Luftbildern** (`world_ground.mjs`, Esri World Imagery): Nutzungsbedingungen der
  Quelle klären, bevor sie übernommen wird.
* **Darstellung, die Kerns Renderer nicht einstellbar macht:** Wasser mit Wellen-Normalen, Tiefenfarbe
  und Ufer-Schaum (`world_water.mjs`), Glassprünge in den Fensterrastern (`facade_marks.mjs`),
  Bloom-Stärke/-Schwelle (`neon_bloom.mjs` 0,13/0,08, Schwelle 0,88), 3D nebeneinander (`stereo_sbs.mjs`,
  Engine-Punkt 9), echte Tür-/Lukenausschnitte im Fassadennetz (Engine-Punkt 15).
* Innenräume: bei Nacht verdecken die Bretter der Barrikade die Türöffnung; die Türflügel selbst
  zeichnet die Innenraum-Darstellung weiter (im Original sind sie nachts weg).
* `fpv_optical_stabilizer.mjs` und `world_liveliness.mjs` sind im Original nicht geladen, auf Kern aber
  gebaut (FPV-Stabilisator, Vögel/Bäume) – mit dem Nutzer klären, ob sie bleiben.

## Später in die Engine (gebaut im Spielordner)

1. **Wheel-Joint mit Lenkung** in `kern::physics::JointSettings` (Box3D kann es: `enableSteering`,
   Lenkmotor, Lenkanschläge) – dann kann das Auto auf echte Radkörper wie in SpawnCity zurück.
2. **Fixed-Step-Callback in `GameWorld`** (vor/nach jedem Physikschritt), statt des Logic-Knotens.
3. **Possession zur Laufzeit** (`GameWorld::possess(actor)` / Pawn verstecken), statt
   `set_alive(false)` + `set_mode(simulate)`.
4. **MVT-Decoder und Kachel-Streaming** (`mvt.*`, `city_stream.*`) als `kern/geo`-Bausteine,
   inklusive zeitgeteiltem Hochladen und Kollisionsaufbau.
5. **Eigene Imports im App-Host** (z. B. Audio-Ausgabe, Gamepad) – heute fragt die Seite per Export ab.
6. **Heightfield-Kollision** in `kern::physics` (Box3D hat `b3CreateHeightFieldShape`) – heute je
   Bodenkachel ein Dreiecksnetz (`terrain.cpp`); dazu `destroy_shape`, damit eine Kachel ihren Shape
   tauschen kann statt den ganzen Körper.
7. **Kaskaden-Schatten und LOD/Impostor** für große Sichtweiten (Jet).
8. `build.sh` sollte Header außerhalb von `kern/include`/`kern/src` in den Cache-Schlüssel nehmen.
9. **Mehrere Ansichten pro Frame im App-Host** (z. B. `App::render_views()` oder eine Liste von
   `RenderParams` mit Viewport und Kamera, Schattenkarte geteilt) – für 3D nebeneinander (`stereo_sbs.mjs`).
10. **`GameWorld::set_time_of_day()`** – heute rechnet das Spiel das Tageslicht mit eigener Uhr nach
    (`look::day_lighting`, dieselben Formeln wie `apply_lighting`).
11. **Ego-Modell abschalten / eigenes Ego-Modell** in `WeaponComponent` (heute blendet das Spiel Kerns
    View-Model-Meshes über `Renderer::set_visible` aus) und **Bloom-Parameter** (Stärke, Schwelle) zur Laufzeit.
12. **Zeichenpuffer-Skalierung im App-Host** (`runKernApp` mit änderbarer Pixelratio) – heute setzt die
    Seite die Canvas-Größe selbst nach `renderScale` und ruft `kern_app_resize`.
13. **Öffentlicher Transport in `KernNet`** (`kern/web/studio/game.mjs`): WebRTC-Datenkanal mit
   Signalisierung über MQTT und versiegeltem Broker-Fallback (heute `web/mp_host.mjs` `MpPublicNet`),
   und ein Autoritätswechsel in `kern::net` (neuer Server aus dem ältesten Client), wenn der Host geht.
14. **Sprach-Bus im Mixer** (`kern::audio`): eine gestreamte Quelle je Sprecher mit Jitter-Puffer statt
   des Nachmischens in `Game::audio_render` (`mp::VoiceStream`).
15. **Ragdoll-Baustein** (Catto-Mensch aus Kapseln mit Gelenkgrenzen und Reibung, `ragdoll.*`) und
    ein **Ausschnitt-Mechanismus für Gebäudenetze** (Tür/Dachluke im Kachelnetz, wie SpawnCitys
    `facadeCutouts`), dazu `destroy_shape`, damit ein Haus seinen Kollisions-Shape abgeben kann, ohne
    dass die ganze Kachel neu gebaut wird (`CityStream::exclude_building`).
16. **Steuerung** (gebaut in `touch_play.*`, `controls.*`, `game_controls.cpp`): `TouchControls` mit
    frei belegbarem Layout je Modus (Knopfliste, Halten/Tippen, Höhenschieber, Pinch) statt des festen
    Soldaten-Layouts; `InputArbiter` mit Pad-Eingabe-Zustand und Plattformmeldung; im App-Host ein
    `mousemove`-Strom unter Pointer-Lock (begrenzt je Ereignis) und ein Lock-Status; `Key` um `+ - /`
    erweitern; im Soldaten: Schuss durch einen Bildschirmpunkt (Touch-Feuer am Finger für die
    Kern-Waffen MP/Glock/Sniper), Sprint in jede Richtung, Respawn per Aufruf statt nach Zeit,
    Blickkurve/Raten der Pad-Sticks als Parameter (heute rechnet das Spiel sie und gibt Grad weiter).
