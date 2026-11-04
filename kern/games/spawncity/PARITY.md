# SpawnCity auf Kern – Paritätsliste

Alle 250 Module aus `nostack-dev/spawncity-de/sim/*.mjs` (Commit `9108537`) und wo ihr Verhalten auf
Kern steht. Ziel: SpawnCity auf Kern **genau** wie spawncity.de.

Status:

* **gebaut** – Verhalten und Werte auf Kern nachgebaut (bei „Stichprobe“ Werte gegen das JS geprüft).
* **teilweise** – ein Teil fehlt oder weicht ab (Spalte „Begründung“ sagt was).
* **fehlt** – nicht nachgebaut.
* **entfällt** – nur mit Grund: (a) three.js-/WebGL-Infrastruktur, die Kerns Renderer ersetzt (das
  sichtbare Verhalten steht in der Zeile); (b) DOM-/Browser-Hilfsschicht oder Werkzeug ohne
  Spielverhalten; (c) im Original toter oder abgeschalteter Code. „Im Original nicht geladen“ heißt:
  vom Spiel (`drone_simulator.html` → `sim/real_world_bootstrap.mjs`, `sim/auto_flight_start.mjs`,
  `sim/edge_hud_layout.mjs` und alle statischen/dynamischen Importe) nie importiert.
* **in Arbeit (sc-m3-…)** – gehört einem anderen laufenden Branch (Steuerung, Leben, Menüs).

Stand: gebaut: 112 · entfällt: 61 · in Arbeit (sc-m3-controls): 22 · teilweise: 16 · in Arbeit (sc-m3-ui): 15 · fehlt: 13 · in Arbeit (sc-m3-life): 11

| Modul | Zweck | Status | Kern-Datei(en) | Begründung / Beleg |
|---|---|---|---|---|
| `airframe_collision_envelope` | Propellerkreise als zusätzliche Box3D-Kollider der Drohne | gebaut | `drone.cpp`, `drone.h` |  |
| `animal_audio` | Tierstimmen (Vogel, Hund, Katze, Kuh) als Synthese | in Arbeit (sc-m3-life) |  |  |
| `audio_settings` | Ton-Einstellungen (Hauptschalter, Busse, Mix v5) | gebaut | `settings.cpp`, `settings.h`, `game_ui.cpp` | Stichprobe: Drohne 50 %, Fahrzeug 58 % (README-Tabelle) |
| `auto_flight_start` | START mit frischem GPS-Fix, Netzwerkposition nach 12 s | gebaut | `game_ui.cpp`, `web/ui.mjs` |  |
| `ballistics` | Geschossimpuls p = m·v je Kaliber | gebaut | `combat_detail.h`, `game_details.cpp` | Stichprobe: 9 mm 2,9 N·s, 5,56 3,8, .50 BMG 41 N·s |
| `box3d_collider_debug` | Neon-Linienmaterialien/Debug-Zeichnen der Kollider | entfällt |  | (b) three.js-Materialhilfe (LineBasicMaterial/Fat-Lines); das Debug-Zeichnen ist im Spiel aus, der Neon-Stil der Welt kommt auf Kern aus Kerns Materialien (`stylized_world_style`). |
| `box3d_hitscan` | Eigene Box3D-Welt nur für Treffer-Strahlen | entfällt | game_weapons.cpp (`hit_scan`) | (b) Technik: eine zweite Box3D-Welt, weil three.js-Gebäude nicht in der Physikwelt lagen. Auf Kern liegen Gebäude, Boden und Körper in EINER Box3D-Welt; Treffer sind Box3D-Strahlen darin (`Game::hit_scan`). Sichtbares Verhalten (Hitscan, Boden = Geländefläche) gleich. |
| `box3d_human` | Box3D-Ragdoll-Mensch (Port von human.c) | in Arbeit (sc-m3-life) |  |  |
| `building_interiors` | Begehbare Innenräume aller Gebäude | in Arbeit (sc-m3-life) |  |  |
| `camera_collision_guard` | Kamera nie in Wänden/unter Boden; Verfolgerkamera steigt über das Autodach | teilweise | game.cpp (`collide`), game_gaps.cpp (`gaps_camera`) | Box3D-Kugelsweep gebaut; fehlt: Bodenhöhe +0,12 m und das Steigen über das Autodach bei Kollision. |
| `camera_pitch_contract` | Neigungsgrenzen zu Fuß (Eingabe vor Anzeige-Grenze) | in Arbeit (sc-m3-controls) | `game.cpp` |  |
| `camera_settings` | Kamera-Einstellungen (FPV-Neigung, FOV, Third-Abstand) | gebaut | `settings.cpp`, `game.cpp`, `game_ui.cpp` | Stichprobe: Neigung −15°, FOV 105 |
| `camera_shake` | Trauma-Modell fürs Kamerawackeln | gebaut | `look.cpp`, `look.h`, `game_look.cpp` | Stichprobe: Abbau 1,35/s, 0,055 rad, 19 Hz |
| `camera_stabilization` | Gedämpfte Verfolgerkamera (Profile follow/third) | gebaut | game.cpp (`rig`) | Stichprobe: 4,8/7,0/7,5/0,18 und 3,6/5,2/3,8/0,36 |
| `car_radio` | Autoradio | in Arbeit (sc-m3-ui) | `equipment.cpp`, `game_equipment.cpp` |  |
| `changelog_menu` | Titel: NEU / CHANGELOG | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `character_model` | Gelenkfigur für alle Menschen (Spieler, Mitspieler, Zombies) | gebaut | `people_art.cpp`, `people_art.h`, `game_net.cpp` | Prozedurale Gelenkfiguren (Hüfte/Knie/Schulter/Ellbogen) mit Gang, Zielen, Liegen; Outfits Polizei/Soldat/Zombie. Kerns eigener Soldat für den Spieler. |
| `combat_audio_bank` | Kampfklänge als vorab berechnete PCM-Bank | gebaut | `combat_audio.cpp`, `combat_audio.h`, `game_details.cpp` | Alle 11 Arten der Bank (shot, pistol, hit, crack, damage, scream, explosion, step, bounce, reward, fail) nachsynthetisiert. Sniper/Werfer/Nachladen: die Bank hat dafür keine Einträge (im Original Wiedergabe von `shot`/`explosion` mit anderer Rate). |
| `combat_hit_stack_guard` | Hält die Kette der Treffer-Router (`registerWorldPopulationHit`) zusammen | entfällt | game_town.cpp (`handle_shots`) | (b) Monkey-Patch-Schutz für überschriebene JS-Funktionen ohne eigenes Verhalten. Auf Kern verteilt `handle_shots` jeden Treffer in fester Reihenfolge (Kampf, Leute, Fahrzeuge, Mitspieler). |
| `combat_visual_polish` | three.js-Materialpolitur für Kampfeffekte | entfällt |  | (b) nur three.js-Materialeinstellungen (README); die Effekte selbst zeichnet `combat_art`. |
| `component_mass_model` | Komponentenmassen und Trägheit der Drohne | gebaut | `drone.cpp`, `drone.h` | Stichprobe: ≈ 0,72 kg |
| `control_semantics` | Höhengesetz GAME, Stickkurven, Grenzen | gebaut | `drone.cpp`, `game.cpp` | Stichprobe: 12 m/s, Totband 0,08 |
| `control_settings` | Einstellungsseiten der Steuerung | in Arbeit (sc-m3-controls) |  |  |
| `controller` | Controller-Seite (Handy als Fernsteuerung, drone_controller.html) | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): eigene Seite `drone_controller.html` für die echte Drohne/Fernsteuerung, nicht Teil des Spiels. |
| `cows` | Kühe auf Weiden | in Arbeit (sc-m3-life) |  |  |
| `critical_damage_logic` | Kritischer Schaden: Zeiten bis zur Explosion | gebaut | `combat_detail.cpp`, `combat_detail.h` | Stichprobe: Bus 3,1/0,72 s, Polizeidrohne 34 % 2,2 s |
| `crowd_characters` | Instanzierte Menschen-Darstellung | gebaut | `people_art.cpp`, `people_art.h` |  |
| `day_night_sync` | Tageszeit des Hosts für alle | gebaut | `look.h`, `game_look.cpp`, `net_session.h` |  |
| `desktop_controls` | Maus/Tastatur | in Arbeit (sc-m3-controls) |  |  |
| `direct_fire_missiles` | Gerade Drohnenraketen | gebaut | `game_weapons.cpp`, `weapons.h` | Stichprobe: 31 m/s, 220 m, 8 m / 100 |
| `direct_udp_peer` | WebRTC-Datenkanal (UDP-artig) | gebaut | `web/mp_host.mjs` |  |
| `dom_mutation_filter` | Filter für MutationObserver der HUD-Module | entfällt |  | (b) reine DOM-Hilfe ohne Spielverhalten. |
| `drone_control_settings` | Drohnen-Steuerungseinstellungen | in Arbeit (sc-m3-controls) | `settings.h` |  |
| `drone_weapon_modes` | Drohnenwaffen GUN/ROCKETS/GUIDED (ohne Nuke) | gebaut | `game_weapons.cpp`, `weapons.h` |  |
| `dynamic_lights` | Fester Pool von Punktlichtern | gebaut | `game_look.cpp`, `look.h` |  |
| `edge_hud_layout` | Touch-Knöpfe an den Rändern | gebaut | `touch_pad.h`, `web/ui.mjs` |  |
| `emp_button_layout` | EMP-Knopf-Platz | in Arbeit (sc-m3-controls) |  |  |
| `facade_marks` | Ruß, Durchbrüche und Glassprünge an Fassaden | teilweise | `world_marks.cpp`, `city_stream.h`, `game_world.cpp` | Ruß und Wanddurchbrüche gebaut; Glassprünge in Fenstern fehlen (Kerns BuildingBuilder legt Fensterraster im Shader an, das Spiel kennt die Scheiben nicht). |
| `fighter_jets` | Kampfjets im Tiefflug, Luftschlag | teilweise | `combat_art.cpp`, `combat_rules.h`, `game_military.cpp` | Flug, Knall, Luftschlag gebaut. Fehlt: Triebwerks-Loop mit Retarded-Time-Doppler (`jetEngineAudio`), Senden von Luftschlag/Überflug an Mitspieler (`sendFx kind strike/flyover`). |
| `first_person_control_settings` | Einstellungen zu Fuß | in Arbeit (sc-m3-controls) | `settings.cpp` |  |
| `first_person_controller_v5` | Zielsteuerung zu Fuß | in Arbeit (sc-m3-controls) |  |  |
| `first_person_fire_contract_v1` | Feuer-Vertrag zu Fuß | in Arbeit (sc-m3-controls) |  |  |
| `first_person_weapon_runtime_v3` | Ego-Waffen-Laufzeit | in Arbeit (sc-m3-controls) |  |  |
| `flak_battery` | Stationäre Flak | gebaut | `combat_art.cpp`, `combat_rules.cpp`, `game_military.cpp` | Stichprobe: Zelle 700 m, 32 %, 18 m AGL |
| `flight_fire_fx` | Drohnen-MG: Hitscan, Leuchtspur, Einschlag, Rückstoß | gebaut | `game_weapons.cpp` | Hitscan 650 m, 92 ms Takt, Decals/Leuchtspuren (Kern), Geschossimpuls. |
| `flight_first_cleanup` | Touch-Feuer zu Fuß, SMG-Takt | in Arbeit (sc-m3-controls) |  |  |
| `flight_logbook` | Flug-Logbuch | in Arbeit (sc-m3-ui) | `equipment.cpp`, `game_equipment.cpp` |  |
| `foot_look_capture` | Blick-Erfassung zu Fuß | in Arbeit (sc-m3-controls) |  |  |
| `foot_touch_controller` | Touch zu Fuß | in Arbeit (sc-m3-controls) |  |  |
| `fps_control_math` | Stickkurven zu Fuß | in Arbeit (sc-m3-controls) |  |  |
| `fpv_optical_stabilizer` | FPV-Bildstabilisator | entfällt | `look.cpp`, `game_look.cpp` | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): trotzdem nachgebaut (README „FPV-Stabilisator“); stört nicht, da nur in der FPV-Kamera. Hinweis an den Nutzer: für exakte Parität müsste er AUS sein. |
| `fpv_view_height` | FPV-Kamera 4,5 cm höher entlang der Rumpf-Hochachse | teilweise | game.cpp (FPV-Auge) | FPV-Auge 0,028 m über der Rumpfmitte; die zusätzlichen 0,045 m fehlen. |
| `game_exit_button` | EXIT-Knopf | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `game_reset` | RESET | gebaut | `game_town.cpp`, `game_ui.cpp` |  |
| `game_rules_sync` | Regeln des ersten Spielers, Abstimmung | gebaut | `net_session.cpp`, `mp_rules.h`, `game_net.cpp` | Einzige Regel im Original: friendlyFire (`GAME_RULE_KEYS`); einstimmig, 30 s, Rosterwechsel bricht ab – über das Netz in `Session::propose_rule/vote`. |
| `gameplay_final_input_extras` | Zusatzeingaben | in Arbeit (sc-m3-controls) |  |  |
| `gameplay_final_realism_patch` | Mündungsfeuer tiefengetestet hinter dem Ego-Modell | entfällt |  | (b) three.js-Material-/Render-Order-Patch; Kerns Ego-Modell und Mündungsfeuer zeichnen tiefengetestet. |
| `gameplay_final_runtime` | Ältere Waffen-Laufzeit | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch gameplay_final_runtime_v2. |
| `gameplay_final_runtime_v2` | Waffen, Granaten, Explosionsphysik, Fußgänger-Animation | gebaut | `game_weapons.cpp`, `game.cpp`, `weapons.h` | Stichprobe: Granate 9,81, Aufschlagzünder außer Boden, Explosion Smoothstep-Abfall + LOS gegen Gebäude. |
| `gameplay_final_ui_dock` | Waffenknöpfe andocken | in Arbeit (sc-m3-controls) |  |  |
| `gameplay_fun_logic` | Punkte, Combo, Aufträge, Risikobank | gebaut | `combat_rules.cpp`, `combat_rules.h` |  |
| `gameplay_fun_runtime` | HUD der Punkte/Aufträge, Highscore | gebaut | `game_events.cpp`, `web/host.mjs` |  |
| `gameplay_polish` | Ältere Politur (Dichte, Spawn-Versatz) | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch gameplay_polish_lite. |
| `gameplay_polish_lite` | Treffer-Ton, Autoexplosions-Ton, getrennter Erst-Spawn im MP | gebaut | `game_events.cpp`, `mp_rules.cpp` | Keine Zusatzautos/-leute (`extraCars 0`); Erst-Spawn über den Gruppen-Anker. |
| `gangsters` | Bewaffnete Passanten | gebaut | `game_events.cpp`, `combat_rules.h` |  |
| `gps_live_mode` | LIVE GPS | gebaut | `gps_live.cpp`, `game_ui.cpp` |  |
| `gta_runtime_v1` | Alte GTA-Laufzeit | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code. |
| `guided_balance` | GUIDED-Werte, Ausweichen | gebaut | `guided.cpp`, `game_guided.cpp` |  |
| `guided_missile` | GUIDED-Lenkrakete | teilweise | `guided.cpp`, `game_guided.cpp` | Lenkung, Zielwahl, Zünder gebaut; Rauchschweif und eigener Motorton fehlen. |
| `hand_grenade` | Handgranate (Kochen) | gebaut | `game_weapons.cpp`, `weapons.h` |  |
| `headshot_feedback` | Kopftreffer-Klick | gebaut | `game_events.cpp` |  |
| `immersive_soundscape` | Geräuschkulisse | in Arbeit (sc-m3-ui) | `game_equipment.cpp` |  |
| `impact_explosion_overlay` | Funkenstern am Einschlagpunkt des Drohnen-MGs | fehlt | `game_gaps.cpp`, `web/host.mjs` | Funkenstern am Einschlagpunkt fehlt. |
| `jet_flight_dynamics` | 6-DoF-Jet | gebaut | `jet.cpp`, `jet.h` |  |
| `jet_mode` | Jet: Einsteigen, Cockpit, Schleudersitz, Mitflieger | teilweise | `jet.h`, `game.cpp`, `game_weapons.cpp` | Pilot, Schweben/Flug, Schleudersitz gebaut; Mitfliegen im Jet eines Mitspielers fehlt (siehe jet_seat_*). |
| `jet_seat_coordinator` | Sitzvergabe Pilot/Beifahrer über die Autorität | fehlt |  | Mitflieger-Sitz im Mehrspieler nicht gebaut. |
| `jet_seat_table` | Sitz-Zustandsautomat | fehlt |  | wie jet_seat_coordinator. |
| `jetpack_audio` | Jetpack-Ton | gebaut | `game_equipment.cpp` |  |
| `jogger_chain` | Laufgruppe | in Arbeit (sc-m3-life) |  |  |
| `lan_vs` | Mehrspieler-Sitzung, Autorität, Transport | teilweise | `net_session.cpp`, `web/mp_host.mjs` | Autoritätswechsel zum Nächstältesten, wenn der Host geht, fehlt (die Runde endet). |
| `map_peer_edge` | Mitspieler am Kartenrand | gebaut | `mp_rules.h`, `web/mp_host.mjs` |  |
| `military_response` | Militär ab 5 Sternen | teilweise | `combat_rules.cpp`, `game_military.cpp` | Ablauf und Werte gebaut. Abweichung: Panzer ist ein leichter Agent statt Box3D-Körper (`upsertBody kind bus 9200 kg`); Leuchtspuren/Granaten gehen nicht an Mitspieler (`sendFx tracer/shell`). |
| `mobile_gameplay_ui` | Kompaktes Handy-HUD | gebaut | `web/ui.mjs`, `game.cpp` |  |
| `mobile_peer_marker_layout` | Namensschilder weg von den Sticks | gebaut | `mp_rules.h` |  |
| `mobile_zoom_guard` | Kein Doppeltipp-Zoom | in Arbeit (sc-m3-controls) |  |  |
| `motor_sound` | Motorklang Drohne | gebaut | `sound_fx.cpp`, `sound_fx.h` |  |
| `mouse_settings` | Mausempfindlichkeit, Y invertieren | gebaut | `settings.cpp`, `game.cpp` | Stichprobe: 20–300 % |
| `mp_fun_hud` | Mehrspieler-HUD (Killfeed, Tafel) | gebaut | `game_net.cpp`, `web/mp_host.mjs` |  |
| `mqtt_data_relay` | Signalisierung/Fallback über MQTT | gebaut | `web/mp_host.mjs` |  |
| `multiplayer_navigation` | Navigationspose aus Mitspieler-Posen | gebaut | `mp_rules.h`, `game_net.cpp` | Kern-Posen tragen die aktive Pose direkt (Modus, Position, Gierwinkel); die Karte nutzt `nav`-Zeitleiste. |
| `music_player` | Musik | in Arbeit (sc-m3-ui) | `equipment.cpp`, `game_equipment.cpp` |  |
| `native_airframe_presentation` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Drohne/Jet über `VehicleArt`. |
| `native_city_geometry` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Gebäude über Kerns `BuildingBuilder` (city_stream.cpp). |
| `native_city_indexed` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Index-Kompaktierung macht Kerns Mesh-Upload. |
| `native_city_stream` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: `CityStream`. |
| `native_flight_pose_view` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: GameWorld interpoliert Posen. |
| `native_gameplay_camera` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Kameras in `Game::update_camera`. |
| `native_osm_projector` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Projektion in tiles.cpp. |
| `native_pedestrian_visuals` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: `PeopleArt`. |
| `native_peer_visuals` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: game_net.cpp/mp_art. |
| `native_point_lights` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Kerns 6 Licht-Slots über `LightPool` (Laternen, Blaulicht, Mündungsfeuer, Explosionen – sichtbar im Nacht-Smoke-Test). |
| `native_population_view` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: People. |
| `native_render_culling` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Kerns Frustum-Culling; Sichtweiten je Qualitätsstufe (perf.h). |
| `native_render_math` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: kern/math. |
| `native_rigid_visuals` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: GameWorld zeichnet Actors. |
| `native_road_geometry` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Kerns `RoadBuilder`, auf dem Gelände drapiert. |
| `native_road_stream` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: CityStream. |
| `native_scene_bvh` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Box3D-Strahlen gegen die Gebäudenetze. |
| `native_scene_picking` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Fadenkreuz-Strahl `aim_ray` + Box3D. |
| `native_scene_renderer` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Kerns Renderer. |
| `native_sun_shadow` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Kerns Sonnenschatten (2048/110 m, mobil 1024/70 m, je Stufe 35–110 m). |
| `native_terrain_geometry` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: terrain.cpp (5-m-Knoten, SW–NE-Diagonale). |
| `native_terrain_stream` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: terrain.cpp. |
| `native_viewport` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Pixelratio über `renderScale`. |
| `native_webgl2_static` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: Kerns statische Meshes. |
| `native_world_physics_view` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: GameWorld. |
| `native_world_presenter` | Baustein des nativen WebGL2-Renderers | entfällt |  | (a) three.js/WebGL-Schicht des abgebrochenen nativen Renderers; im Original nicht geladen. Kerns Renderer ersetzt sie: GameWorld/App. |
| `nav_ar_route_math` | Routen-Hinweise | gebaut | `mp_rules.h` |  |
| `nav_marker_timeline` | Glättung entfernter Fahrzeuge für die Navigation | gebaut | `mp_rules.h` |  |
| `nav_route_request_gate` | Nur neueste Routenanfrage zählt | gebaut | `web/mp_host.mjs` |  |
| `neon_bloom` | Bloom | teilweise | `game_look.cpp`, `perf.h` | Bloom an; Stärke 0,13/0,08 und Schwelle 0,88 lassen sich in Kern nicht einstellen (Engine-Punkt 11, mit dem Nutzer klären). |
| `neon_ui_theme` | UI-Skin | gebaut | `web/ui.mjs` |  |
| `nostr_data_relay` | Zweiter öffentlicher Weg über Nostr-Relays | fehlt |  | braucht Schnorr-Signaturen auf secp256k1 (im Browser nicht eingebaut). |
| `nuke_destruction` | Atombombe: Zerstörung | entfällt |  | (c) im Original nicht erreichbar (README „Nuke: bewusst nicht gebaut“). |
| `nuke_shock_field` | Atombombe: Stoßfront | entfällt |  | (c) wie nuke_destruction. |
| `p2p_link` | Kopplung Spiel ↔ Handy-Controller (QR, WebRTC) | entfällt |  | (b) Kopplung mit der Controller-Seite für die echte Drohne (`controller.mjs`, `drone_controller.html`), außerhalb des Spiels; im Spiel nur das Fallback-Panel der Werkbank (`.panel`, im Spiel ausgeblendet). |
| `pad_actions` | Xbox-Aktionen | in Arbeit (sc-m3-controls) |  |  |
| `pedestrian_agents` | Fußgänger-Agenten | gebaut | `people.cpp`, `combat_rules.cpp` |  |
| `peer_drone_model` | Drohne der Mitspieler | gebaut | `mp_art.h` |  |
| `people_impacts` | Fahrzeuge treffen Leute | in Arbeit (sc-m3-life) |  |  |
| `physics_validation` | Abnahmegrenzen für Messflug-Kalibrierung | entfällt |  | (b) nur in der Werkbank-Ansicht (Log-Import, Fit; `.panel` ist im Spiel per `body.solo-flight` ausgeblendet). Mit dem Nutzer klären, falls die Werkbank Teil des Klons sein soll. |
| `pigeons` | Tauben | in Arbeit (sc-m3-life) |  |  |
| `ping_marker` | Ping-Marker | gebaut | `mp_art.cpp`, `web/mp_host.mjs` |  |
| `player_box3d_capsule_runtime` | Box3D-Kapsel zu Fuß | gebaut | `Kerns CharacterMover` | Kerns Box3D-Kapsel-Mover; Schüsse gehen durch (`pass-through`). |
| `player_camera_mode_policy` | Drohnenkamera follow/third/fpv, zu Fuß „walk“ | gebaut | game.cpp (`drone_cam_`) | Standard follow, C wechselt follow → third → fpv. |
| `player_capsule_collision` | Kapselmaße zu Fuß | gebaut | `Kerns CharacterMover` |  |
| `player_car_mode` | Auto fahren | gebaut | `game.cpp` |  |
| `player_push` | Kräfte zwischen Spielern | in Arbeit (sc-m3-life) |  |  |
| `player_runtime_hotfix` | Reset, Eigentreffer, Zielen | gebaut | `game_ui.cpp`, `game_weapons.cpp` |  |
| `player_settings_sections` | Einstellungs-Abschnitte | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `player_shield` | Pilotenschild | entfällt |  | (c) „retired“, immer aus. |
| `player_vehicle_runtime` | Alte Fahrzeug-Laufzeit | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch v2. |
| `player_vehicle_runtime_v2` | Drohne ausstellen/einpacken, Pilot mit VR-Brille | gebaut | `game.cpp`, `game_net.cpp` |  |
| `player_vitals_logic` | Lebenspunkte, Drohnen-Ersatz | gebaut | `game_details.cpp` |  |
| `player_vitals_runtime` | −18 % Schaden für Spieler | gebaut | `game.cpp` | Stichprobe: Gesundheit 118 |
| `player_walk_mode` | Alter Fußmodus | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch v4. |
| `player_walk_mode_v4` | Zu Fuß, Schwimmen, Jetpack | gebaut | `game.cpp`, `equipment.cpp` |  |
| `police_ground_units` | Streifenwagen mit Beamten | gebaut | `game.cpp`, `game_town.cpp` |  |
| `police_sirens` | Martinshorn 466/622 Hz, Drohnen-Whoop, Doppler | teilweise | `game_gaps.cpp`, `sound_fx.cpp` | Kern spielt eine Heulsirene 650–1450 Hz statt Martinshorn 466/622 Hz; keine Drohnen-Sirene. |
| `projectile_ballistics` | Geschossflug mit Fallen | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): nur Bibliothek + Architekturtest; live ist jeder Schuss Hitscan (`flight_fire_fx.mjs`: „damage is immediate hitscan now“). Kern ebenso. |
| `propulsion_authority` | Akku/Motor-Grenzen | gebaut | `drone.cpp` |  |
| `proximity_voice` | Sprachchat | gebaut | `game_net.cpp`, `mp_rules.cpp`, `web/mp_host.mjs` |  |
| `pwa_fullscreen_policy` | Vollbild-Regel | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `qr_pairing` | QR-Code | gebaut | `qr_code.h`, `game_net.cpp` |  |
| `quit_confirm` | Zurück zum Hauptmenü? | in Arbeit (sc-m3-ui) | `game_ui.cpp`, `web/ui.mjs` |  |
| `race_track` | Rennstrecke | gebaut | `combat_rules.cpp`, `game_events.cpp` |  |
| `real_world_bootstrap` | Echte Welt: Karte, Ursprung, Stadt laden | gebaut | `tiles.cpp`, `city_stream.cpp`, `look.h` |  |
| `render_fast_paths` | Leere Instanz-Meshes nicht zeichnen | entfällt |  | (a) three.js-Draw-Call-Abkürzung; Kerns Renderer zeichnet leere Instanzlisten nicht. |
| `render_stability` | Pixelratio-Profil je Gerät | gebaut | `perf.h`, web/host.mjs (`renderScale`) |  |
| `rules_vote_engine` | Einstimmige Abstimmung (Zustandsautomat) | gebaut | `mp_rules.h`, `net_session.cpp`, `combat_detail.h` | Netz-Abstimmung in `Session` (mp::Ballot); `combat_detail` ist die gleiche Logik für den Testbefehl `ballot`. |
| `sandbox_game` | Sandbox-Gegner, Chaos-Punkte | gebaut | `combat_rules.cpp`, `game_events.cpp` |  |
| `scene_lighting` | Licht der leuchtenden Dinge | gebaut | `game_look.cpp` |  |
| `settings_controller_nav` | Menüs mit dem Pad | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `settings_help` | Hilfezeilen der Einstellungen | in Arbeit (sc-m3-ui) | `settings.h`, `web/ui.mjs` |  |
| `shader_prewarm` | Shader vorkompilieren | entfällt |  | (a) three.js kompiliert beim ersten Zeichnen; Kern baut seine Programme beim Start (kein Ruckler beim ersten Effekt). |
| `share_button` | TEILEN | gebaut | `web/mp_host.mjs` |  |
| `shared_sticks` | Ein Stick-Paar für alles | gebaut | `touch_pad.h` |  |
| `simulator` | Drohnensimulator (Flugmodell, Sensoren, Firmware) + Werkbank | gebaut | `drone.cpp`, `drone.h`, `game.cpp`, `firmware/` | Werkbank-Panel (Stückliste, Log-Fit) ist im Spiel ausgeblendet (`body.solo-flight .panel{display:none}`), siehe physics_validation. |
| `site_config` | Produktname, Host-URLs | entfällt |  | (b) Konfiguration der Website ohne Spielverhalten. |
| `sniper_scope` | Zielfernrohr | gebaut | `game_events.cpp`, `web/host.mjs` |  |
| `solo_layout` | Solo-HUD-Layout | in Arbeit (sc-m3-controls) |  |  |
| `spawn_visibility_guard` | Nichts entsteht im Blick eines Spielers | teilweise | `spawn_guard.cpp`, `spawn_guard.h`, game_military.cpp (`spawn_unseen`) | `spawn_unseen` ohne 24-m-Mindestabstand, ohne Mitspieler-Blick, nur für Kampf-Spawns. |
| `speed_lines` | Speed-Lines | gebaut | `look.cpp`, `game_look.cpp` |  |
| `start_location` | Startort | in Arbeit (sc-m3-ui) | `game_ui.cpp`, `web/ui.mjs` |  |
| `start_pose_guard` | Drohne beim Start auf dem Boden, ARM-Knopf oben | gebaut | game.cpp (`place_world`) | Start auf dem Boden (unterster Punkt = Geländehöhe); ARM-Knopf im HUD. |
| `stereo_sbs` | 3D nebeneinander | fehlt | `settings.cpp` | braucht mehrere Ansichten pro Frame im App-Host (Engine-Punkt 9) – mit dem Nutzer klären. |
| `street_lamps` | Laternen: umknicken, dunkel, Sync | teilweise | `game_town.cpp` | Laternen stehen und leuchten; fehlt: Umknicken ab 3,5 m/s (Box3D-Prop 140 kg), Dunkelwerden bei Treffern/Explosionen, Sync an Mitspieler. |
| `street_voices` | Hupen, Rufe | in Arbeit (sc-m3-life) |  |  |
| `stylized_world_style` | Look der Welt | gebaut | `game.cpp`, `game_town.cpp`, `main.cpp` |  |
| `suggestion_box` | Ideen-Box | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `terrain_craters` | Krater, gemeinsame Höhenfunktion | gebaut | `terrain.cpp`, `world_marks.cpp` |  |
| `terrain_elevation` | Höhenkacheln | gebaut | `terrain.cpp`, `web/host.mjs` |  |
| `terrain_surface` | Dreiecks-Diagonale | gebaut | `terrain.cpp` |  |
| `terrain_tiles` | Box3D-Bodenkacheln | gebaut | `terrain.cpp` |  |
| `thumb_layout` | Daumen-Layout | gebaut | `touch_pad.h` |  |
| `tow_service` | Abschleppdienst | gebaut | `game_traffic.cpp`, `world_traffic.cpp` |  |
| `training_showcase` | Trainingswelt-Deko | fehlt |  | Welt ohne Geodaten: im Original das Trainingslevel, auf Kern die prozedurale Ersatzstadt. |
| `training_test_level_v2` | Trainingslevel (Welt ohne Geodaten) | fehlt |  | wie training_showcase. |
| `vehicle_control_settings` | Fahrzeug-Einstellungen | in Arbeit (sc-m3-controls) | `settings.cpp` |  |
| `vehicle_models` | Fahrzeugmodelle | gebaut | `vehicle_art.h`, `vehicle_art.cpp` |  |
| `vehicle_spawn_clearance` | Keine überlappenden Fahrzeug-Spawns (SAT, 0,35 m) | fehlt | `spawn_guard.h`, game.cpp (`manage_traffic`) | Verkehrs-Spawns prüfen keine Überlappung. |
| `viewmodel_arms` | Ego-Arme | gebaut | `game_look.cpp` |  |
| `viewmodel_pass` | Ego-Waffe in eigenem Pass (nie in der Wand) | gebaut | `Kerns WeaponComponent-View-Model` | Kern zeichnet das Ego-Modell mit eigenem Tiefenbereich; eigene Modelle (game_look.cpp) ebenso über Kerns View-Model-Ebene. |
| `visual_pose_stabilization` | Geglättete Drohnen-Darstellung in follow/third | fehlt |  | Darstellung der Drohne folgt der Physik ungeglättet. |
| `voice_codec` | µ-law | gebaut | `mp_rules.h` |  |
| `voice_control_bindings` | Mikrofon-Tasten | gebaut | `game_net.cpp` |  |
| `voice_hud_layout` | Sprach-Knöpfe | in Arbeit (sc-m3-controls) |  |  |
| `vs_combat_presentation` | Treffer/Tod im MP | gebaut | `game_net.cpp`, `mp_rules.h` |  |
| `vs_fx_geo_adapter` | Effekte mit Geo-Koordinaten (verschiedene Ursprünge) | entfällt | `net_session.cpp` | (b) Technik: im Original hat jeder Client eigenen Kartenursprung. Auf Kern übernimmt jeder Client den Ursprung des Hosts (`rebase_origin`), Effekte kommen in gemeinsamen Metern – sichtbar am selben Ort (Mehrspieler-Smoke: Explosion/Ping). |
| `vs_group_spawn` | Gruppen-Spawn | gebaut | `mp_rules.h`, `net_session.h` |  |
| `vs_multiplayer` | Mehrspieler-Spielschleife | gebaut | `game_net.cpp`, `mp_art.cpp` |  |
| `vs_multiplayer_guard` | MATES-Knopf-Text bei Peer-Ereignissen | entfällt |  | (b) DOM-Knopfbeschriftung; der Mehrspieler-HUD zeigt die Mitspielerzahl. |
| `vs_pose_sync` | Posen-Zeitleiste | gebaut | `mp_rules.cpp`, `mp_rules.h` |  |
| `walk_collision_math` | Punkt-in-Polygon / Ausweg aus Gebäuden | gebaut | `Kerns CharacterMover + Box3D-Gebäudenetze` | Box3D-Kapsel gegen Gebäudenetze ersetzt die Polygonprüfung; Ausweg aus Gebäuden über den Spawn-Schutz `clear_spot`. |
| `walk_input_lifecycle_guard` | Eingabe-Lebenszyklus zu Fuß | in Arbeit (sc-m3-controls) |  |  |
| `walk_profile_contract` | Datensatz-Marker des Lauf-Profils | entfällt |  | (b) setzt nur `data-walk-control-profile` am DOM. |
| `walk_ui_layout_hotfix` | Handy-HUD zu Fuß im Bild halten | gebaut | `web/ui.mjs` |  |
| `walk_world_experience_hotfix` | Panik, Ziel-HP, Schrei | gebaut | `people.cpp`, `game_town.cpp` |  |
| `wanted_dispatch_guard` | Polizeidrohnen erst nach Anfahrzeit sichtbar, „DISPATCH“-HUD | fehlt | `game_gaps.cpp`, `game_town.cpp` | Polizeidrohnen erscheinen sofort. |
| `wanted_police_drones` | Polizeidrohnen | gebaut | `game_town.cpp`, `game_details.cpp` |  |
| `wanted_police_fairness_v5` | Fairness-Regeln Polizei | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code. |
| `wanted_police_pre_guard` | Polizeidrohnen sehen nur in Feuerreichweite | fehlt | `game_gaps.cpp`, `game_town.cpp` | Polizeidrohnen sehen im ganzen Erfassungsradius (59–87 m) statt nur in Feuerreichweite. |
| `wanted_system_logic` | Fahndungsregeln | gebaut | `wanted.cpp` | Stichprobe: 4/9/15/22/30, EMP 30 m / 18 s, Anfahrzeit, Trefferchance |
| `weapon_models` | Ego-Waffenmodelle | gebaut | `game_look.cpp` |  |
| `world_action_feedback` | Partikel, Decals, Ton und Reaktion bei jedem Schuss | gebaut | `game_town.cpp`, `game_weapons.cpp` |  |
| `world_actor_roots` | Wurzelobjekt eines Akteurs finden | entfällt |  | (b) three.js-Szenengraph-Hilfe; Kern kennt Akteure direkt. |
| `world_building_collision_physics` | Box3D-Gebäudekörper, Kamerapfad, Startpunkt frei | gebaut | `city_stream.cpp`, `game.cpp` |  |
| `world_building_collisions` | Gebäudeumrisse aus den Kacheln (Dubletten) | gebaut | `city_stream.cpp`, `tiles.cpp` |  |
| `world_building_depth_occlusion` | Gebäude als Tiefen-/Umriss-Geometrie | entfällt | `city_stream.cpp` | (a) three.js-Tiefenpass für Gebäudeumrisse; auf Kern sind die Gebäude echte Meshes (Tiefe, Verdeckung, Schatten). |
| `world_camera_math` | Meter je Pixel, FPV-Zielabstand | entfällt |  | (b) Kartenmathematik für die alte 2D-Karte. |
| `world_camera_projection` | Kamera-Projektion für Audio/Marker | gebaut | game_net.cpp (`project_screen`) |  |
| `world_car_explosion` | Autoexplosion, Trümmer | gebaut | `game_events.cpp` |  |
| `world_city_buildings` | Gebäude auf dem Gelände | gebaut | `city_stream.cpp` |  |
| `world_city_roads` | Straßen, Parks, Wasser | gebaut | `city_stream.cpp`, `tiles.h` |  |
| `world_critical_damage_fx` | Rauch (dunkle Scheiben) und Glut vor der Explosion | teilweise | `game_gaps.cpp`, `game_details.cpp` | Kritischer Schaden zählt herunter und raucht mit Kern-Partikeln, ohne die dunklen Scheiben und die Glut. |
| `world_crowd_density_v1` | Passanten-Dichte | in Arbeit (sc-m3-life) |  |  |
| `world_destruction_state` | Gebäudeschaden (Dach tiefer, eingeebnet) | entfällt |  | (c) einzige Schreiber sind die Atombombe (`nuke_destruction.mjs`) und deren Sync – im Original nicht erreichbar. |
| `world_ground` | Bodenfarbe aus Luftbildern (Albedo je 10-m-Zelle) | fehlt |  | Kern färbt den Boden aus den Flächen der Vektorkacheln. |
| `world_impacts` | Spuren jeder Aktion | gebaut | `game_world.cpp`, `world_marks.cpp` |  |
| `world_liveliness` | Lebendigkeit der Welt | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code (in der Liste von sc-m3-life). |
| `world_location_selector` | Ortswahl | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `world_map_full` | Große Karte | gebaut | `game_net.cpp`, `web/mp_host.mjs` |  |
| `world_options` | WELT-Schalter | gebaut | `game_ui.cpp`, `mp_rules.h` |  |
| `world_options_menu` | WELT-Menü | in Arbeit (sc-m3-ui) | `web/ui.mjs` |  |
| `world_organic_hit_audio` | Knacken beim Tod von Mensch/Tier | fehlt | `game_gaps.cpp` | Kein Knacken beim Tod. |
| `world_person_ragdoll` | Box3D-Ragdoll | in Arbeit (sc-m3-life) |  |  |
| `world_population` | Alte Bevölkerung | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code. |
| `world_procedural_population` | Bevölkerung: Leute, Verkehr | teilweise | `game_town.cpp`, `game_traffic.cpp`, `people.cpp` | Leben gebaut; der Tod eines Passanten geht nicht an Mitspieler (`sendFx npc-death`). |
| `world_rigid_body_physics` | Box3D-Welt der Körper | gebaut | `game_world.cpp`, `common.h` |  |
| `world_rigid_body_runtime` | Fester Physikschritt, Fehlerfang | gebaut | game.cpp (Logic-Knoten, `240 Hz)` | Fester Schritt über Kern; Kern fängt Fehler im Schritt selbst. |
| `world_spawn_guard` | Startpunkt nicht in einem Gebäude | gebaut | game.cpp (`clear_spot`) |  |
| `world_sync` | Atombombe und Gebäudeschaden an Mitspieler | entfällt |  | (c) synchronisiert nur Nuke und Nuke-Gebäudeschaden (`world_destruction_state`), beides im Original nicht erreichbar. |
| `world_traffic_motion_guard_v1` | Gerade richten schiebender Autos | entfällt |  | (b) Korrektur für SpawnCitys Fahrer-KI; Kerns Fahrer folgen der Spur (README Verkehr). |
| `world_traffic_routes` | Routen aus Kacheln | gebaut | `city_stream.cpp` |  |
| `world_transform_numeric` | Numerische Posen ohne three.js | entfällt |  | (b) Hilfe für den nativen Renderer. |
| `world_vehicle_dynamics` | Fahrzeugmodell | gebaut | `car.cpp`, `car.h` |  |
| `world_water` | Wasser | teilweise | `terrain.cpp`, `city_stream.cpp`, `game_world.cpp` | Physik und Fläche gebaut; Wellen-Normalen, Tiefenfarbe und Ufer-Schaum im Shader fehlen. |
| `xbox_control_mode` | Xbox-Modus | in Arbeit (sc-m3-controls) |  |  |
| `xbox_gamepad` | Gamepad | gebaut | `game.cpp` | Totzone 0,14 |
| `zombie_nights` | Zombie-Nächte | teilweise | `combat_rules.cpp`, `game_events.cpp` | Runden gebaut; Barrikaden (brauchen Innenräume von sc-m3-life) und Sync an Mitspieler (`sendFx zombie-*`) fehlen. |
