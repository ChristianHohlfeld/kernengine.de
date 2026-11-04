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
| Kampf/Ereignisse | **J** Luftschlag auf den anvisierten Punkt (30 s), **H** nächster Auftrag, **L** Rennstrecke (Drohne) | – | – |

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
    settings.*    Einstellungstabelle (Werte, Bereiche, Hilfe aus SpawnCity), `set`/`get`/`settings schema`
    gps_live.*    LIVE-GPS-Filter (Sprünge verwerfen, Vorhersage, Tätigkeit)
    equipment.*   Jetpack, Flug-Logbuch, MusicSynth (Titelmusik, Autoradio)
    game_ui.*     Menü-/Einstellungs-Seite des Spiels: Anwenden der Einstellungen, Pause, RESET, Startort, LIVE GPS
    game_equipment.cpp  Jetpack am Soldaten, Logbuch, Radio, Geräuschkulisse
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
  erreichen andere Spieler nur, wenn es an ist). Hunde, Katzen, Vögel, Zombies, Militär und Flak hat das
  Spiel auf Kern noch nicht – ihre Schalter werden gespeichert und als „bald“ gezeigt.
* **Menüs** (`web/ui.mjs`, nur DOM): Titelbildschirm (START, WELT, EINSTELLUNGEN, NEU / CHANGELOG;
  automatisierte Browser und `?autostart=1` überspringen ihn, `?menu=1` erzwingt ihn), Pausemenü (Esc,
  MENÜ, Pad-Menütaste, Verlassen des Pointer-Locks), „Zurück zum Hauptmenü?“ (Enter/Y – Esc/N),
  EXIT / MENÜ / RESET oben links, Steuerung der Menüs mit dem Pad (D-Pad, A, B). Solange ein Menü offen
  ist, steht das Spiel (`pause 1`: keine Eingabe, kein Physikschritt, Ton pausiert).
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
  Bildschirm bleibt an (Wake Lock). Offen: der blaue Positionspunkt und die Spurlinie am Boden.
* **Jetpack** (`equipment.*`, `game_equipment.cpp`; `player_walk_mode_v4.mjs`, `jetpack_audio.mjs`): WELT-Schalter
  (an). Zweiter Sprung in der Luft zündet (Stoß 0,6 s), Sprung halten fliegt weiter: Steigen bis 8 m/s
  (Beschleunigung g + (8 − vz)·3, höchstens 13,8 m/s²), Luftsteuerung 14 m/s², Tank ≈ 5 s, füllt sich in
  4 s am Boden (in der Luft nach 0,35 s ohne Schub), kurz vor einem harten Aufprall zündet er selbst
  (21 m/s²). Der Schub ist ein Impuls auf Kerns Charakter (75 kg). Ton: Zünd-„Wumm“ (Druckstoß,
  Flammenstoß, Klick) und ein Brüllen aus gefiltertem Rauschen mit Knistern; Tankanzeige unten.
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
* **Idee fürs nächste Update** (`suggestion_box.mjs`): nach 15 min Spielzeit einmal gefragt. SpawnCity
  verschickt die Zeile als signiertes Nostr-Ereignis (`nostr-tools`, secp256k1-Schnorr); eine
  Fremdbibliothek und ein eigener Sammeldienst passen nicht zum dünnen Host. Hier: Kopieren oder als
  Vorschlag im öffentlichen GitHub-Repository öffnen.
* **App/Vollbild** (`pwa_fullscreen_policy.mjs`): `web/manifest.webmanifest` (Vollbild, quer) mit
  eigenem SVG-Symbol; auf Touch-Geräten startet START das Vollbild, außer die installierte App läuft
  schon im eigenen Fenster.

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
| Kampfdetails | Drohnenraketen 31 m/s, 220 m, 950 ms, 8 m / 100, 7,2 s; GUIDED 2,2 g; Auto kritisch 20 %, 2,45 s / 0,62 s; neue Drohne nach 8 s; Trümmer 9 Teile, 6,5 s, Abprall 0,30 / 0,76 | `direct_fire_missiles.mjs`, `guided_balance.mjs`, `critical_damage_logic.mjs`, `player_vitals_logic.mjs`, `world_car_explosion.mjs` |

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
* Höhen: „Terrain Tiles“ (Mapzen, auf AWS Open Data, frei nutzbar mit Namensnennung der Quellen
  SRTM, EU-DEM, 3DEP u. a., siehe https://github.com/tilezen/joerd/blob/master/docs/attribution.md);
  im Spiel rechts unten genannt.
* Gebaut mit `--commercial`: keine Micro-Duck-Meshes (CC BY-NC-SA) im Modul. Keine Pollen-Meshes,
  keine fremden Bilder oder Musik; alle Töne sind Synth-Patches im Spielordner.
* Kein OSM-Snapshot im Repo: die Umgebung dieses Meilensteins hatte keinen Zugriff auf OSM-Quellen,
  im SpawnCity- und im Kern-Repo liegt keiner. Der Offline-Fall nutzt deshalb die prozedurale Stadt.

## Offen / nächste Meilensteine

* Tiere; Fahrer, die beim Carjacking sichtbar aussteigen.
* **Nuke: bewusst nicht gebaut.** Auf spawncity.de ist sie nicht erreichbar: `drone_weapon_modes.mjs`
  sperrt die Drohnen-Nuke (nur GUN/ROCKETS/GUIDED), der Kommentar „LAUNCHER NUKE“ in
  `gameplay_final_runtime_v2.mjs` hat keinen Code-Zweig mehr (`footShotAt` feuert normale Raketen),
  `world_options.mjs` hat keinen Nuke-Schalter, und niemand löst `arondight:nuke-impact` oder eine
  Explosion `kind:"nuke"` aus. `nuke_destruction.mjs`/`nuke_shock_field.mjs` sind dort toter Code.
  Ebenso ist der Pilotenschild (`player_shield.mjs`) „retired“ (immer aus) – nicht nachgebaut.
* Zombie-Barrikaden (Türen mit Brettern, REPARIEREN) brauchen begehbare Innenräume, die Kern-SpawnCity
  noch nicht hat; die Zombies laufen deshalb nur draußen.
* Der Panzer ist ein leichter Agent auf dem Straßengraph (Strahl gegen Wände), kein Box3D-Fahrzeug wie
  in SpawnCity (`physics.upsertBody kind:"bus"`); Box3D-Körper zur Laufzeit wären ein Level-Actor mehr.
* Militär, Zombies, Gegner und Flak sind lokal (SpawnCity schickt Schnappschüsse an die Mitspieler);
  die Abstimmung über Spielregeln (`rules_vote_engine.mjs`) braucht Netz-Nachrichten – lokal gibt es die
  Schalter als Befehle (`military on|off`, `zombies on|off`).
* Trainingslevel (`training_test_level_v2.mjs`, `training_showcase.mjs`) ersetzen in SpawnCity die
  Welt ohne Geodaten; auf Kern übernimmt das die prozedurale Ersatzstadt (`city_stream`) – nicht
  nachgebaut, um die Stadt-Streaming-Arbeit nicht zu kreuzen. Die Rennstrecke (L) gibt es überall.
* Kampfton: Kerns eingebaute Klänge (`builtin:smg/pistol/explosion/boost/...`) statt der eigenen
  Synth-Patches aus `combat_audio_bank.mjs`, kein Retarded-Time-Doppler-Loop für die Jets (nur der
  verspätete Überflug-Knall). Geschossflug mit Fallen (`projectile_ballistics.mjs`) für MP/Sniper:
  Kerns Soldatenwaffen sind Hitscan. `combat_visual_polish.mjs` betrifft nur three.js-Materialien.
* Zerstörung an Gebäuden (Krater, Fassadenspuren), Ragdolls.
* Mehrspieler: Sprachchat; Polizei und Passanten nur lokal (wie in SpawnCity).
* Wasser: Wellen-Normalen, Tiefenfarbe und Schaum im Shader. Innenräume,
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
6. **Heightfield-Kollision** in `kern::physics` (Box3D hat `b3CreateHeightFieldShape`) – heute je
   Bodenkachel ein Dreiecksnetz (`terrain.cpp`); dazu `destroy_shape`, damit eine Kachel ihren Shape
   tauschen kann statt den ganzen Körper.
7. **Kaskaden-Schatten und LOD/Impostor** für große Sichtweiten (Jet).
8. `build.sh` sollte Header außerhalb von `kern/include`/`kern/src` in den Cache-Schlüssel nehmen.
