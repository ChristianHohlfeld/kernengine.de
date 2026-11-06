# SpawnCity auf Kern – Paritätsliste

Alle 250 Module aus `nostack-dev/spawncity-de/sim/*.mjs` (Commit `9108537`) und wo ihr Verhalten auf
Kern steht. Ziel: SpawnCity auf Kern **genau** wie spawncity.de.

Status:

* **original** – auf der Seite läuft der Code des Originals selbst (unverändert, `web/audio/`).
* **gebaut** – Verhalten und Werte auf Kern nachgebaut (bei „Stichprobe“ Werte gegen das JS geprüft).
* **teilweise** – ein Teil fehlt oder weicht ab (Spalte „Begründung“ sagt was).
* **fehlt** – nicht nachgebaut.
* **entfällt** – nur mit Grund: (a) three.js-/WebGL-Infrastruktur, die Kerns Renderer ersetzt (das
  sichtbare Verhalten steht in der Zeile); (b) DOM-/Browser-Hilfsschicht oder Werkzeug ohne
  Spielverhalten; (c) im Original toter oder abgeschalteter Code. „Im Original nicht geladen“ heißt:
  vom Spiel (`drone_simulator.html` → `sim/real_world_bootstrap.mjs`, `sim/auto_flight_start.mjs`,
  `sim/edge_hud_layout.mjs` und alle statischen/dynamischen Importe) nie importiert.
* **in Arbeit (sc-m3-…)** – gehört einem anderen laufenden Branch (Steuerung, Leben, Menüs).

Stand: gebaut: 177 · entfällt: 61 · teilweise: 7 · fehlt: 5

| Modul | Zweck | Status | Kern-Datei(en) | Begründung / Beleg |
|---|---|---|---|---|
| `airframe_collision_envelope` | Propellerkreise als zusätzliche Box3D-Kollider der Drohne | gebaut | `drone.cpp`, `drone.h` |  |
| `animal_audio` | Tierstimmen (Vogel, Hund, Katze, Kuh) als Synthese | original | `game_life.cpp` | Auf der Seite läuft das Original unverändert (`web/audio/animal_audio.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `audio_settings` | Ton-Einstellungen (Hauptschalter, Busse, Mix v5) | original | `settings.cpp`, `settings.h`, `game_ui.cpp` | Auf der Seite läuft das Original unverändert (`web/audio/audio_settings.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `auto_flight_start` | START mit frischem GPS-Fix, Netzwerkposition nach 12 s | gebaut | `game_ui.cpp`, `web/ui.mjs` |  |
| `ballistics` | Geschossimpuls p = m·v je Kaliber | gebaut | `combat_detail.h`, `game_details.cpp` | Stichprobe: 9 mm 2,9 N·s, 5,56 3,8, .50 BMG 41 N·s |
| `box3d_collider_debug` | Neon-Linienmaterialien/Debug-Zeichnen der Kollider | entfällt |  | (b) three.js-Materialhilfe (LineBasicMaterial/Fat-Lines); das Debug-Zeichnen ist im Spiel aus, der Neon-Stil der Welt kommt auf Kern aus Kerns Materialien (`stylized_world_style`). |
| `box3d_hitscan` | Eigene Box3D-Welt nur für Treffer-Strahlen | entfällt | game_weapons.cpp (`hit_scan`) | (b) Technik: eine zweite Box3D-Welt, weil three.js-Gebäude nicht in der Physikwelt lagen. Auf Kern liegen Gebäude, Boden und Körper in EINER Box3D-Welt; Treffer sind Box3D-Strahlen darin (`Game::hit_scan`). Sichtbares Verhalten (Hitscan, Boden = Geländefläche) gleich. |
| `box3d_human` | Box3D-Ragdoll-Mensch (Port von human.c) | gebaut | `game_life.cpp` | README „Ragdolls“: Box3D-Mensch aus Kapseln mit Kegel-/Scharniergrenzen und Reibungsmotor (sc-m3-life). |
| `building_interiors` | Begehbare Innenräume aller Gebäude | gebaut | `game_life.cpp` | README „Innenräume“: Türen, Stockwerke, Treppenhaus, Dach, Möbel, Fenster (sc-m3-life). Offen dort: echter Türausschnitt im Fassadennetz (Engine-Punkt 15). |
| `camera_collision_guard` | Kamera nie in Wänden/unter Boden; Verfolgerkamera steigt über das Autodach | gebaut | game.cpp (`collide`), game_gaps.cpp (`gaps_camera`) | Boden +0,12 m, Near 0,045–0,08, über das Dach bei Kollision < 3,2 m (Lift 1,25 + 0,75·(3,2−h)), Blick auf das Auto; Jet/Schirm frei. Neu in sc-m3-gaps. |
| `camera_pitch_contract` | Neigungsgrenzen zu Fuß (Eingabe vor Anzeige-Grenze) | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `camera_settings` | Kamera-Einstellungen (FPV-Neigung, FOV, Third-Abstand) | gebaut | `settings.cpp`, `game.cpp`, `game_ui.cpp` | Stichprobe: Neigung −15°, FOV 105 Werte geprüft: follow-FOV clamp(52·FOV/105, 30, 90), third clamp(62·FOV/105, 35, 100) (vorher beide 30–100). |
| `camera_shake` | Trauma-Modell fürs Kamerawackeln | gebaut | `look.cpp`, `look.h`, `game_look.cpp` | Stichprobe: Abbau 1,35/s, 0,055 rad, 19 Hz |
| `camera_stabilization` | Gedämpfte Verfolgerkamera (Profile follow/third) | gebaut | game.cpp (`rig`) | Stichprobe: 4,8/7,0/7,5/0,18 und 3,6/5,2/3,8/0,36 |
| `car_radio` | Autoradio | original | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | Auf der Seite läuft das Original unverändert (`web/audio/car_radio.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `changelog_menu` | Titel: NEU / CHANGELOG | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `character_model` | Gelenkfigur für alle Menschen (Spieler, Mitspieler, Zombies) | gebaut | `people_art.cpp`, `people_art.h`, `game_net.cpp` | Prozedurale Gelenkfiguren (Hüfte/Knie/Schulter/Ellbogen) mit Gang, Zielen, Liegen; Outfits Polizei/Soldat/Zombie. Kerns eigener Soldat für den Spieler. |
| `combat_audio_bank` | Kampfklänge als vorab berechnete PCM-Bank | original | `combat_audio.cpp`, `combat_audio.h`, `game_details.cpp` | Auf der Seite läuft das Original unverändert (`web/audio/combat_audio_bank.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `combat_hit_stack_guard` | Hält die Kette der Treffer-Router (`registerWorldPopulationHit`) zusammen | entfällt | game_town.cpp (`handle_shots`) | (b) Monkey-Patch-Schutz für überschriebene JS-Funktionen ohne eigenes Verhalten. Auf Kern verteilt `handle_shots` jeden Treffer in fester Reihenfolge (Kampf, Leute, Fahrzeuge, Mitspieler). |
| `combat_visual_polish` | three.js-Materialpolitur für Kampfeffekte | entfällt |  | (b) nur three.js-Materialeinstellungen (README); die Effekte selbst zeichnet `combat_art`. |
| `component_mass_model` | Komponentenmassen und Trägheit der Drohne | gebaut | `drone.cpp`, `drone.h` | Stichprobe: ≈ 0,72 kg |
| `control_semantics` | Höhengesetz GAME, Stickkurven, Grenzen | gebaut | `drone.cpp`, `game.cpp` | Stichprobe: 12 m/s, Totband 0,08 |
| `control_settings` | Einstellungsseiten der Steuerung | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `controller` | Controller-Seite (Handy als Fernsteuerung, drone_controller.html) | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): eigene Seite `drone_controller.html` für die echte Drohne/Fernsteuerung, nicht Teil des Spiels. |
| `cows` | Kühe auf Weiden | gebaut | `game_life.cpp` | README „Kühe“: Herden auf Wiesen, Weltuhr-Pfad, Tod an Mitspieler (Fx `cow_death`) (sc-m3-life). |
| `critical_damage_logic` | Kritischer Schaden: Zeiten bis zur Explosion | gebaut | `combat_detail.cpp`, `combat_detail.h` | Stichprobe: Bus 3,1/0,72 s, Polizeidrohne 34 % 2,2 s |
| `crowd_characters` | Instanzierte Menschen-Darstellung | gebaut | `people_art.cpp`, `people_art.h` |  |
| `day_night_sync` | Tageszeit des Hosts für alle | gebaut | `look.h`, `game_look.cpp`, `net_session.h` |  |
| `desktop_controls` | Maus/Tastatur | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `direct_fire_missiles` | Gerade Drohnenraketen | gebaut | `game_weapons.cpp`, `weapons.h` | Stichprobe: 31 m/s, 220 m, 8 m / 100 |
| `direct_udp_peer` | WebRTC-Datenkanal (UDP-artig) | gebaut | `web/mp_host.mjs` |  |
| `dom_mutation_filter` | Filter für MutationObserver der HUD-Module | entfällt |  | (b) reine DOM-Hilfe ohne Spielverhalten. |
| `drone_control_settings` | Drohnen-Steuerungseinstellungen | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `drone_weapon_modes` | Drohnenwaffen GUN/ROCKETS/GUIDED (ohne Nuke) | gebaut | `game_weapons.cpp`, `weapons.h` |  |
| `dynamic_lights` | Fester Pool von Punktlichtern | gebaut | `game_look.cpp`, `look.h` |  |
| `edge_hud_layout` | Touch-Knöpfe an den Rändern | gebaut | `touch_pad.h`, `web/ui.mjs` |  |
| `emp_button_layout` | EMP-Knopf-Platz | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `facade_marks` | Ruß, Durchbrüche und Glassprünge an Fassaden | teilweise | `world_marks.cpp`, `city_stream.h`, `game_world.cpp` | Ruß und Wanddurchbrüche gebaut; Glassprünge in Fenstern fehlen (Kerns BuildingBuilder legt Fensterraster im Shader an, das Spiel kennt die Scheiben nicht). |
| `fighter_jets` | Kampfjets im Tiefflug, Luftschlag | gebaut | `combat_art.cpp`, `combat_rules.h`, `game_military.cpp`, game_gaps.cpp (`gaps_jet_audio`) | Flug, Luftschlag; Triebwerks-Loop je Jet (4 s Rumpeln/Brüllen/Zischen + Turbine 1180/2360 Hz) mit Retarded-Time und Doppler (0,45 … 2,6), Pegel min(1,8; (85/d)^1,35)·0,9·0,85, Tiefpass 900 + 60000/d; Luftschlag/Überflug an Mitspieler (Fx `strike`/`flyover`). Neu in sc-m3-gaps. |
| `first_person_control_settings` | Einstellungen zu Fuß | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `first_person_controller_v5` | Zielsteuerung zu Fuß | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `first_person_fire_contract_v1` | Feuer-Vertrag zu Fuß | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `first_person_weapon_runtime_v3` | Ego-Waffen-Laufzeit | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `flak_battery` | Stationäre Flak | gebaut | `combat_art.cpp`, `combat_rules.cpp`, `game_military.cpp` | Stichprobe: Zelle 700 m, 32 %, 18 m AGL Stichprobe: alle Konstanten (Zelle 700, 32 %, 1100 m, 18 m, 900 m/s, 30/s, 2,6/1,7 s, 0,012, 7 m, HP 60) wie `flak_battery.mjs`. |
| `flight_fire_fx` | Drohnen-MG: Hitscan, Leuchtspur, Einschlag, Rückstoß | gebaut | `game_weapons.cpp` | Hitscan 650 m, 92 ms Takt, Decals/Leuchtspuren (Kern), Geschossimpuls. Werte geprüft: 92 ms, 650 m stimmen; Treffer auf Passanten nach dem neuen Treffermodell. |
| `flight_first_cleanup` | Touch-Feuer zu Fuß, SMG-Takt | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `flight_logbook` | Flug-Logbuch | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `foot_look_capture` | Blick-Erfassung zu Fuß | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `foot_touch_controller` | Touch zu Fuß | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `fps_control_math` | Stickkurven zu Fuß | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `fpv_optical_stabilizer` | FPV-Bildstabilisator | entfällt | `look.cpp`, `game_look.cpp` | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): trotzdem nachgebaut (README „FPV-Stabilisator“); stört nicht, da nur in der FPV-Kamera. Hinweis an den Nutzer: für exakte Parität müsste er AUS sein. |
| `fpv_view_height` | FPV-Kamera 4,5 cm höher entlang der Rumpf-Hochachse | gebaut | game.cpp (FPV-Auge) | Auge jetzt 0,028 + 0,045 m über der Rumpfmitte (neu in sc-m3-gaps). |
| `game_exit_button` | EXIT-Knopf | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `game_reset` | RESET | gebaut | `game_town.cpp`, `game_ui.cpp` |  |
| `game_rules_sync` | Regeln des ersten Spielers, Abstimmung | gebaut | `net_session.cpp`, `mp_rules.h`, `game_net.cpp` | Einzige Regel im Original: friendlyFire (`GAME_RULE_KEYS`); einstimmig, 30 s, Rosterwechsel bricht ab – über das Netz in `Session::propose_rule/vote`. |
| `gameplay_final_input_extras` | Zusatzeingaben | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `gameplay_final_realism_patch` | Mündungsfeuer tiefengetestet hinter dem Ego-Modell | entfällt |  | (b) three.js-Material-/Render-Order-Patch; Kerns Ego-Modell und Mündungsfeuer zeichnen tiefengetestet. |
| `gameplay_final_runtime` | Ältere Waffen-Laufzeit | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch gameplay_final_runtime_v2. |
| `gameplay_final_runtime_v2` | Waffen, Granaten, Explosionsphysik, Fußgänger-Animation | gebaut | `game_foot_rounds.cpp`, `game_weapons.cpp`, `game.cpp`, `weapons.h` | Werte geprüft (Projektile, Nahkampf, Munition). Korrigiert: Granatwerfer 40 m/s gerade (vorher +2,5 m/s z), Zünder 2,8 s, TTL 5,6 s, Boden-Abprall 0,58/0,88 bis 7×, Ruhe-Regel, Aufschlag auf Wand/Auto/Person, Fernzündung (Bereit nach 380 ms); Rakete 22 → 40 m/s (55 m/s²), Laserlenkung 2,8 rad/s auf den Fadenkreuz-Punkt ≤ 420 m, neue Rakete macht alte ungelenkt (1,2 m/s²), TTL 9 s, Rauchspur 25 ms / 90 Puffs; Schüsse durch Granaten zünden sie (0,35 m); Fäuste zwei Hände (300/250 ms, 24/16, Combo 430 ms ×1,35), Messer trifft 130 ms nach dem Druck, Nahkampf-Kegel cos 0,76; keine Magazine/Nachladen (das Original kennt keine Munition). Zweiter Stapel: MP 180 m, Glock 240 m; Glock und Sniper durchschlagen bis zu 3 Körper (0,6 m weiter, Wände stoppen); Sniper wirkt erst bei Ankunft (Entfernung / 850 m/s). Offen: Impulse auf lose Körper (pushHitBody), softTargetNear (Laternen/Kühe am Projektil). Explosionen: Spieler nimmt erst ab 2 Schaden, Polizeidrohnen in jeder Explosion min(3, round(Betrag/34)) Treffer zu 34 (vorher nur Projektile, Radius ×1,3). Dritter Stapel: Faust/Messer stoßen lose Körper (pushHitBody, N·s am Treffpunkt); Werfergranaten und Raketen zünden an Laternen und Kühen (softTargetNear); Touch-Schüsse mit Glock/Sniper durchschlagen ebenfalls bis zu 3 Körper; Spieler-Explosionsschaden mit den Klammern 2–18 m / 10–160. |
| `gameplay_final_ui_dock` | Waffenknöpfe andocken | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `gameplay_fun_logic` | Punkte, Combo, Aufträge, Risikobank | gebaut | `combat_rules.cpp`, `combat_rules.h` |  |
| `gameplay_fun_runtime` | HUD der Punkte/Aufträge, Highscore | gebaut | `game_events.cpp`, `web/host.mjs` |  |
| `gameplay_polish` | Ältere Politur (Dichte, Spawn-Versatz) | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch gameplay_polish_lite. |
| `gameplay_polish_lite` | Treffer-Ton, Autoexplosions-Ton, getrennter Erst-Spawn im MP | gebaut | `game_events.cpp`, `mp_rules.cpp` | Keine Zusatzautos/-leute (`extraCars 0`); Erst-Spawn über den Gruppen-Anker. |
| `gangsters` | Bewaffnete Passanten | gebaut | `game_events.cpp`, `combat_rules.h` |  |
| `gps_live_mode` | LIVE GPS | gebaut | `gps_live.cpp`, `game_ui.cpp` |  |
| `gta_runtime_v1` | Alte GTA-Laufzeit | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code. |
| `guided_balance` | GUIDED-Werte, Ausweichen | gebaut | `guided.cpp`, `game_guided.cpp` |  |
| `guided_missile` | GUIDED-Lenkrakete | gebaut | `guided.cpp`, `game_guided.cpp`, game_gaps.cpp (`gaps_guided`) | Lenkung, Zielwahl, Zünder; Lock-Ton (2 × 1760 Hz), Zündknall (110 → 40 Hz), Motorstimme (Rauschen durch 700-Hz-Bandpass + 1300-Hz-Säge, 0,5 → 0,28), Rauchspur (160 Wölkchen 0,9 m hinter der Rakete, 1,6 m, 0,42, 2,6 s, steigend). Ton und Rauch neu in sc-m3-gaps. |
| `hand_grenade` | Handgranate (Kochen) | gebaut | `game_controls.cpp`, `game_foot_rounds.cpp`, `weapons.h` | Werte geprüft. Korrigiert: Zünder 4 s (FUSE_MS, vorher 2,5), Abprall wie Werfergranate (0,58/0,88, nie Aufschlagzünder, Kollisionssperre 60 ms), Zündung in der Hand 8,5 m / 125 (explodeAt, vorher 6 / 90). Kochen/Wurf 9+13·Kraft, 3 Stück, 20 s Nachschub: sc-m3-controls. |
| `headshot_feedback` | Kopftreffer-Klick | original | `game_events.cpp` | Auf der Seite läuft das Original unverändert (`web/audio/headshot_feedback.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `immersive_soundscape` | Geräuschkulisse | original | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | Auf der Seite läuft das Original unverändert (`web/audio/immersive_soundscape.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `impact_explosion_overlay` | Funkenstern am Einschlagpunkt des Drohnen-MGs | gebaut | `game_gaps.cpp`, `web/host.mjs` | Bildschirm-Burst (Kern, Ring, 9 Funken, 330 ms, Pool 12) am projizierten Einschlagpunkt; C++ liefert die Punkte im HUD-JSON (`gaps.bursts`), die Seite zeichnet. Neu in sc-m3-gaps. |
| `jet_flight_dynamics` | 6-DoF-Jet | gebaut | `jet.cpp`, `jet.h` |  |
| `jet_mode` | Jet: Einsteigen, Cockpit, Schleudersitz, Schaden, Mitflieger | gebaut | `jet.h`, `game.cpp`, `game_weapons.cpp`, game_gaps.cpp (`gaps_jet_damage`), `game_seats.cpp` | Pilot, Schweben/Flug, Schleudersitz; Schaden wie `damageAirframe` (400 HP, Schub 0,35 + hp/200·0,65, bei 15 % 12 s Brand mit Rauch, dann Zerfall: Explosion 14 m / 180, Pilot tot daneben); Mitfliegen (F am gelandeten Jet eines Mitspielers mit Pilot, 9 m; Kamera (0,48; 2,5; 1,25), 72°; Aussteigen über 10 m am Schirm, sonst 5 m voraus). Schaden und Mitflieger neu in sc-m3-gaps. Werte geprüft (Waffen): Bordkanone 55 ms, ±0,003 Streuung, 1800 m, 34 % Platzer 3,2 m / 70 stimmten; korrigiert: Raketen abwechselnd aus den Flügelbehältern (±3,2 m, 2 m vor, 0,6 m tiefer), entlang des Tipp-Ziels, nicht am Boden, fallen mit 0,15 g, nach 5 s ohne Treffer weg (ohne Explosion); Bomben mit Luftwiderstand 0,04/s. Kamera: Verfolger 22 + min(14, v/40) m hinter / 6,5 m über dem Jet im Jet-Rahmen, 1 − e^(−6 dt), Blick 30 m voraus, up 35 % zur Welt, FOV 66 + v/30 (66–90) auch im Cockpit (vorher 24 m Welt-Rahmen, 60–72°; Cockpit 57,6°); Fallschirm-Kamera 14 m südlich / 7 m hoch, FOV 70 (vorher 9 m hinter, 60). Schleudersitz 2,8 m / 28 m/s entlang der Jet-Hochachse (vorher Welt-z); Schirm: waagerecht 0,25/s gedämpft, offen v.z → −6,5 mit 1,6/s gegen die Schwerkraft (≈ 12,6 m/s Sinken; vorher quadratischer Widerstand, 6,5 m/s), vor dem Öffnen kein senkrechter Widerstand. |
| `jet_seat_coordinator` | Sitzvergabe Pilot/Beifahrer über die Autorität | gebaut | net_session.cpp (`seat_request`, `kSeatReq/kSeatState)`, `game_seats.cpp` | Der Host führt die Tabelle, Anfragen gehen an ihn, die Tabelle alle 1,5 s und nach jeder Änderung an alle; wer geht, verliert seine Sitze; allein lokal. Neu in sc-m3-gaps. |
| `jet_seat_table` | Sitz-Zustandsautomat | gebaut | `jet_seats.cpp`, `jet_seats.h`, `tests/test_gaps.cpp` | Nur der Besitzer fliegt, ein Beifahrer nur mit Pilot, ein Sitz je Spieler, Pilot raus = Beifahrer raus, Laden mit Prüfung. Neu in sc-m3-gaps. |
| `jetpack_audio` | Jetpack-Ton | original | `game_equipment.cpp` | Auf der Seite läuft das Original unverändert (`web/audio/jetpack_audio.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `jogger_chain` | Laufgruppe | gebaut | `game_life.cpp`, `web/life.mjs` | README „Laufgruppe“ (sc-m3-life). |
| `lan_vs` | Mehrspieler-Sitzung, Autorität, Transport | teilweise | `net_session.cpp`, `web/mp_host.mjs` | Autoritätswechsel zum Nächstältesten, wenn der Host geht, fehlt (die Runde endet). |
| `map_peer_edge` | Mitspieler am Kartenrand | gebaut | `mp_rules.h`, `web/mp_host.mjs` |  |
| `military_response` | Militär ab 5 Sternen | teilweise | `combat_rules.cpp`, `game_military.cpp`, `game_gaps.cpp` | Ablauf und Werte gebaut; Leuchtspuren und Granaten gehen an Mitspieler (Fx `mil_tracer`/`mil_shell`, neu in sc-m3-gaps). Fehlt: Panzer als Box3D-Körper (`upsertBody kind bus 9200 kg`, heute leichter Agent) und die Schnappschüsse der Einheiten an Mitspieler (`mil-snap`/`mil-hit`). Stichprobe: Heat 36/40 s/55, Alarm 15 s, ≤ 3 Sterne, Trupp 4/8, HP 100/420/900, Treffer 25, Schaden 4/6/3/4, Granate 30, 0,9 s, 3 Salven/1,4 s, Verstärkung 50/45 s.  |
| `mobile_gameplay_ui` | Kompaktes Handy-HUD | gebaut | `web/ui.mjs`, `game.cpp` |  |
| `mobile_peer_marker_layout` | Namensschilder weg von den Sticks | gebaut | `mp_rules.h` |  |
| `mobile_zoom_guard` | Kein Doppeltipp-Zoom | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `motor_sound` | Motorklang Drohne | original | `sound_fx.cpp`, `sound_fx.h` | Auf der Seite läuft das Original unverändert (`web/audio/motor_sound.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `mouse_settings` | Mausempfindlichkeit, Y invertieren | gebaut | `settings.cpp`, `game.cpp` | Stichprobe: 20–300 % |
| `mp_fun_hud` | Mehrspieler-HUD (Killfeed, Tafel) | gebaut | `game_net.cpp`, `web/mp_host.mjs` |  |
| `mqtt_data_relay` | Signalisierung/Fallback über MQTT | gebaut | `web/mp_host.mjs` |  |
| `multiplayer_navigation` | Navigationspose aus Mitspieler-Posen | gebaut | `mp_rules.h`, `game_net.cpp` | Kern-Posen tragen die aktive Pose direkt (Modus, Position, Gierwinkel); die Karte nutzt `nav`-Zeitleiste. |
| `music_player` | Musik | original | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | Auf der Seite läuft das Original unverändert (`web/audio/music_player.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
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
| `nuke_destruction` | Gebäude durch Waffen zerstören („everyday destructibles“); Atombombe | teilweise | `destruction.cpp`, `city_stream.cpp` | Waffen brechen Stockwerke ab, Einsturz zum Stumpf, Abriss-Stück und Bruchtrümmer: gebaut (eigene Umsetzung, destruction.h). Atombombe selbst: im Original nicht erreichbar (`drone_weapon_modes.mjs` entfernt `fireNuke`). Offen: Schwanken (sway) der getroffenen Häuser. |
| `nuke_shock_field` | Atombombe: Stoßfront | entfällt |  | (c) wie nuke_destruction. |
| `p2p_link` | Kopplung Spiel ↔ Handy-Controller (QR, WebRTC) | entfällt |  | (b) Kopplung mit der Controller-Seite für die echte Drohne (`controller.mjs`, `drone_controller.html`), außerhalb des Spiels; im Spiel nur das Fallback-Panel der Werkbank (`.panel`, im Spiel ausgeblendet). |
| `pad_actions` | Xbox-Aktionen | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `pedestrian_agents` | Fußgänger-Agenten | gebaut | `people.cpp`, `combat_rules.cpp` |  |
| `peer_drone_model` | Drohne der Mitspieler | gebaut | `mp_art.h` |  |
| `people_impacts` | Fahrzeuge treffen Leute | gebaut | `game_life.cpp`, `people.cpp` | README „Ragdolls“: Anfahren, Aufstehen, Impulsverlust 75 kg · 0,55 (sc-m3-life). Werte geprüft: Schaden min(260, max(6, (v − 1,5)·8,5)) ab 2,2 m/s (vorher (v − 2)·14 ab 2,5), Rand 0,32 m (vorher 0,35). |
| `physics_validation` | Abnahmegrenzen für Messflug-Kalibrierung | entfällt |  | (b) nur in der Werkbank-Ansicht (Log-Import, Fit; `.panel` ist im Spiel per `body.solo-flight` ausgeblendet). Mit dem Nutzer klären, falls die Werkbank Teil des Klons sein soll. |
| `pigeons` | Tauben | gebaut | `game_life.cpp` | README „Tauben“ (sc-m3-life). |
| `ping_marker` | Ping-Marker | gebaut | `mp_art.cpp`, `web/mp_host.mjs` |  |
| `player_box3d_capsule_runtime` | Box3D-Kapsel zu Fuß | gebaut | `Kerns CharacterMover` | Kerns Box3D-Kapsel-Mover; Schüsse gehen durch (`pass-through`). |
| `player_camera_mode_policy` | Drohnenkamera follow/third/fpv, zu Fuß „walk“ | gebaut | game.cpp (`drone_cam_`) | Standard follow, C wechselt follow → third → fpv. |
| `player_capsule_collision` | Kapselmaße zu Fuß | gebaut | `Kerns CharacterMover` |  |
| `player_car_mode` | Auto fahren | gebaut | `game.cpp` |  Kamera geprüft: Ziel 1,05 m vor / 1,2 m über dem Auto, Auge d hinter und h über dem Ziel (vorher am Auto), FOV min(80, 70 + |v|·0,25) (vorher 58–72), Cockpit 80° (vorher 60°). |
| `player_push` | Kräfte zwischen Spielern | gebaut | `game_life.cpp` | README „Stöße zwischen Spielern“ (Fx `push`) (sc-m3-life); das „Halten“ gehört zur Gravity-Gun. |
| `player_runtime_hotfix` | Reset, Eigentreffer, Zielen | gebaut | `game_ui.cpp`, `game_weapons.cpp` |  |
| `player_settings_sections` | Einstellungs-Abschnitte | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `player_shield` | Pilotenschild | entfällt |  | (c) „retired“, immer aus. |
| `player_vehicle_runtime` | Alte Fahrzeug-Laufzeit | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch v2. |
| `player_vehicle_runtime_v2` | Drohne ausstellen/einpacken, Pilot mit VR-Brille | gebaut | `game.cpp`, `game_net.cpp` |  |
| `player_vitals_logic` | Lebenspunkte, Drohnen-Ersatz | gebaut | `game_details.cpp` |  |
| `player_vitals_runtime` | −18 % Schaden für Spieler | gebaut | `game.cpp` | Stichprobe: Gesundheit 118 Werte geprüft: PLAYER_MAX_HP 100, jeder Treffer ÷ 1,18; Kern hält 118 und zeigt health/maxHealth als 0–100 (vorher 118 im HUD); keine Regeneration (vorher Kern-Standard 12/s nach 5 s). |
| `player_walk_mode` | Alter Fußmodus | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): ersetzt durch v4. |
| `player_walk_mode_v4` | Zu Fuß, Schwimmen, Jetpack | gebaut | `game.cpp`, `equipment.cpp` | Stichprobe Jetpack: 21, 0,6 s, 0,2/s, 4 s, 14, 0,02, 0,35 s, 13,8, 8 m/s. |
| `police_ground_units` | Streifenwagen mit Beamten | gebaut | `game.cpp`, `game_town.cpp` |  Werte geprüft (Treffer): 100 HP, 25 je gezähltem Treffer; MP zählt 1× (Kopf 3×), Glock/Sniper 6× (Kopf 10×) – vorher Kern-Schaden 13/26/95 ×3. Streifenwagen: 300 HP / 22, kritisch 25 % (2,6 s, bei 0 noch ≤ 0,5 s), Explosion linear über r + 2,5 m ×2,4, Crash ab Δv 7 (×16), Wrack 7 m / 110 und 1450·5,5 hoch, vom Spieler gefahren unverwundbar; Beamte in der Explosion 100·(1 − d/r)·1,6 + 20. |
| `police_sirens` | Martinshorn 466/622 Hz, Drohnen-Whoop, Doppler | original | `game_gaps.cpp`, `sound_fx.cpp` | Auf der Seite läuft das Original unverändert (`web/audio/police_sirens.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `projectile_ballistics` | Geschossflug mit Fallen | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): nur Bibliothek + Architekturtest; live ist jeder Schuss Hitscan (`flight_fire_fx.mjs`: „damage is immediate hitscan now“). Kern ebenso. |
| `propulsion_authority` | Akku/Motor-Grenzen | gebaut | `drone.cpp` |  |
| `proximity_voice` | Sprachchat | gebaut | `game_net.cpp`, `mp_rules.cpp`, `web/mp_host.mjs` |  |
| `pwa_fullscreen_policy` | Vollbild-Regel | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `qr_pairing` | QR-Code | gebaut | `qr_code.h`, `game_net.cpp` |  |
| `quit_confirm` | Zurück zum Hauptmenü? | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `race_track` | Rennstrecke | gebaut | `combat_rules.cpp`, `game_events.cpp` |  |
| `real_world_bootstrap` | Echte Welt: Karte, Ursprung, Stadt laden | gebaut | `tiles.cpp`, `city_stream.cpp`, `look.h` |  |
| `render_fast_paths` | Leere Instanz-Meshes nicht zeichnen | entfällt |  | (a) three.js-Draw-Call-Abkürzung; Kerns Renderer zeichnet leere Instanzlisten nicht. |
| `render_stability` | Pixelratio-Profil je Gerät | gebaut | `perf.h`, web/host.mjs (`renderScale`) |  |
| `rules_vote_engine` | Einstimmige Abstimmung (Zustandsautomat) | gebaut | `mp_rules.h`, `net_session.cpp`, `combat_detail.h` | Netz-Abstimmung in `Session` (mp::Ballot); `combat_detail` ist die gleiche Logik für den Testbefehl `ballot`. |
| `sandbox_game` | Sandbox-Gegner, Chaos-Punkte | gebaut | `combat_rules.cpp`, `game_events.cpp` | Stichprobe: Budget 0/1/2/4/6/8, höchstens 8, 6 s, Combo 3,2 s. |
| `scene_lighting` | Licht der leuchtenden Dinge | gebaut | `game_look.cpp` |  |
| `settings_controller_nav` | Menüs mit dem Pad | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `settings_help` | Hilfezeilen der Einstellungen | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `shader_prewarm` | Shader vorkompilieren | entfällt |  | (a) three.js kompiliert beim ersten Zeichnen; Kern baut seine Programme beim Start (kein Ruckler beim ersten Effekt). |
| `share_button` | TEILEN | gebaut | `web/mp_host.mjs` |  |
| `shared_sticks` | Ein Stick-Paar für alles | gebaut | `touch_pad.h` |  |
| `simulator` | Drohnensimulator (Flugmodell, Sensoren, Firmware) + Werkbank | gebaut | `drone.cpp`, `drone.h`, `game.cpp`, `firmware/` | Werkbank-Panel (Stückliste, Log-Fit) ist im Spiel ausgeblendet (`body.solo-flight .panel{display:none}`), siehe physics_validation. |
| `site_config` | Produktname, Host-URLs | entfällt |  | (b) Konfiguration der Website ohne Spielverhalten. |
| `sniper_scope` | Zielfernrohr | gebaut | `game_events.cpp`, `web/host.mjs` |  |
| `solo_layout` | Solo-HUD-Layout | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `spawn_visibility_guard` | Nichts entsteht im Blick eines Spielers | gebaut | `spawn_guard.cpp`, `spawn_guard.h`, game_military.cpp (`spawn_unseen`) | Mindestabstand 24 m, Sichtkegel = halbes Sichtfeld + 20°, verdeckt nur durch Gebäude bis 420 m, bis 1500 m, auch die Blickkegel der Mitspieler; für Polizei, Militär, Zombies, Gegner, Passanten und Verkehr. Neu in sc-m3-gaps. |
| `speed_lines` | Speed-Lines | gebaut | `look.cpp`, `game_look.cpp` |  |
| `start_location` | Startort | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `start_pose_guard` | Drohne beim Start auf dem Boden, ARM-Knopf oben | gebaut | game.cpp (`place_world`) | Start auf dem Boden (unterster Punkt = Geländehöhe); ARM-Knopf im HUD. |
| `stereo_sbs` | 3D nebeneinander | fehlt | `settings.cpp` | braucht mehrere Ansichten pro Frame im App-Host (Engine-Punkt 9) – mit dem Nutzer klären. |
| `street_lamps` | Laternen: Abstand, Licht, umknicken, dunkel, Sync | gebaut | `lamps.cpp`, `lamps.h`, `game_lamps.cpp`, `game_look.cpp` | Alle 34 m, abwechselnd, 1,15 m hinter dem Rand, Mast 6,2 m, Kopf 1,42 m draußen, Glühen 0xffb35a×3,2 und Lichtfleck 13 m ab der Dämmerung; Schuss zu Fuß durch den Kopf (0,38 m) → dunkel + Glas; Explosion: innen 60 % Mast fällt (Box3D-Prop 140 kg, Stoß 140·min(6; 1,2+0,45·v)), sonst dunkel; Auto > 3,5 m/s knickt den Mast (und verliert min(0,22 v; 2,4) m/s), langsamer wird es gestoppt (Klang); höchstens 14 liegende Masten; Sync an Mitspieler (Fx `lamp`), RESET stellt alle auf. Neu in sc-m3-gaps. |
| `street_voices` | Hupen, Rufe | gebaut | `game_life.cpp`, `web/life.mjs` | README „Straßenstimmen“: Zeilen, Drosseln 1,4 s / 6 s, Hupe 1,2 s / 4 s (sc-m3-life). |
| `stylized_world_style` | Look der Welt | gebaut | `game.cpp`, `game_town.cpp`, `main.cpp` |  |
| `suggestion_box` | Ideen-Box | gebaut | `game_ui.cpp`, `nostr.cpp`, `web/ui.mjs` | README: nach 15 min, signiertes Nostr-Ereignis (Art 30078) an die öffentlichen Relays, BIP-340 in nostr.cpp (sc-m3-ui). |
| `terrain_craters` | Krater, gemeinsame Höhenfunktion | gebaut | `terrain.cpp`, `world_marks.cpp` |  |
| `terrain_elevation` | Höhenkacheln | gebaut | `terrain.cpp`, `web/host.mjs` |  |
| `terrain_surface` | Dreiecks-Diagonale | gebaut | `terrain.cpp` |  |
| `terrain_tiles` | Box3D-Bodenkacheln | gebaut | `terrain.cpp` |  |
| `thumb_layout` | Daumen-Layout | gebaut | `touch_pad.h` |  |
| `tow_service` | Abschleppdienst | gebaut | `game_traffic.cpp`, `world_traffic.cpp`, `game_gaps.cpp` | Stichprobe: 2 Wagen, 2,5/9/7 s, 9 m, 170–320 m, Haken 7,5 m / 1,8 s, 9/14 m/s, 70 s, 110 HP. Neu in sc-m3-gaps: Kugeln des Spielers zu Fuß treffen den Abschleppwagen (MP 5, Glock 9, Sniper 70, sonst 8; `onTracer`). |
| `training_showcase` | Trainingswelt-Deko | fehlt |  | Welt ohne Geodaten: im Original das Trainingslevel, auf Kern die prozedurale Ersatzstadt. |
| `training_test_level_v2` | Trainingslevel (Welt ohne Geodaten) | fehlt |  | wie training_showcase. |
| `vehicle_control_settings` | Fahrzeug-Einstellungen | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `vehicle_models` | Fahrzeugmodelle | gebaut | `vehicle_art.h`, `vehicle_art.cpp` |  |
| `vehicle_spawn_clearance` | Keine überlappenden Fahrzeug-Spawns (SAT, 0,35 m) | gebaut | `spawn_guard.h`, game.cpp (`manage_traffic`) | Neu in sc-m3-gaps. |
| `viewmodel_arms` | Ego-Arme | gebaut | `game_look.cpp` |  |
| `viewmodel_pass` | Ego-Waffe in eigenem Pass (nie in der Wand) | gebaut | `Kerns WeaponComponent-View-Model` | Kern zeichnet das Ego-Modell mit eigenem Tiefenbereich; eigene Modelle (game_look.cpp) ebenso über Kerns View-Model-Ebene. |
| `visual_pose_stabilization` | Geglättete Drohnen-Darstellung in follow/third | gebaut | gaps_rules.cpp (`VisualPose`), game_gaps.cpp (`gaps_drone_matrix`) | Versatz zum Kamera-Anker gedämpft (follow 9/s, third 5,2/s), Drehung 12/7 pro s, weiche/harte Grenzen 0,025/0,10 m und 3°/10° (third 0,05/0,18 m, 5°/14°). Neu in sc-m3-gaps. |
| `voice_codec` | µ-law | gebaut | `mp_rules.h` |  |
| `voice_control_bindings` | Mikrofon-Tasten | gebaut | `game_net.cpp` |  |
| `voice_hud_layout` | Sprach-Knöpfe | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `vs_combat_presentation` | Treffer/Tod im MP | gebaut | `game_net.cpp`, `mp_rules.h` |  |
| `vs_fx_geo_adapter` | Effekte mit Geo-Koordinaten (verschiedene Ursprünge) | entfällt | `net_session.cpp` | (b) Technik: im Original hat jeder Client eigenen Kartenursprung. Auf Kern übernimmt jeder Client den Ursprung des Hosts (`rebase_origin`), Effekte kommen in gemeinsamen Metern – sichtbar am selben Ort (Mehrspieler-Smoke: Explosion/Ping). |
| `vs_group_spawn` | Gruppen-Spawn | gebaut | `mp_rules.h`, `net_session.h` |  |
| `vs_multiplayer` | Mehrspieler-Spielschleife | gebaut | `game_net.cpp`, `mp_art.cpp` |  |
| `vs_multiplayer_guard` | MATES-Knopf-Text bei Peer-Ereignissen | entfällt |  | (b) DOM-Knopfbeschriftung; der Mehrspieler-HUD zeigt die Mitspielerzahl. |
| `vs_pose_sync` | Posen-Zeitleiste | gebaut | `mp_rules.cpp`, `mp_rules.h` |  |
| `walk_collision_math` | Punkt-in-Polygon / Ausweg aus Gebäuden | gebaut | `Kerns CharacterMover + Box3D-Gebäudenetze` | Box3D-Kapsel gegen Gebäudenetze ersetzt die Polygonprüfung; Ausweg aus Gebäuden über den Spawn-Schutz `clear_spot`. |
| `walk_input_lifecycle_guard` | Eingabe-Lebenszyklus zu Fuß | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `walk_profile_contract` | Datensatz-Marker des Lauf-Profils | entfällt |  | (b) setzt nur `data-walk-control-profile` am DOM. |
| `walk_ui_layout_hotfix` | Handy-HUD zu Fuß im Bild halten | gebaut | `web/ui.mjs` |  |
| `walk_world_experience_hotfix` | Panik, Ziel-HP, Schrei | gebaut | `people.cpp`, `game_town.cpp`, `game_vehicle_damage.cpp` |  Werte geprüft (Fahrzeuge): Auto 180 HP / 30 je Treffer, Bus 320 / 26 (vorher alle 300 / 22), Sniper 34 % der HP; kritisch bei 20 % (Auto 2,45 s, Bus 3,1 s), jeder weitere Treffer verkürzt auf 0,62 / 0,72 s; Explosion ceil(3f) bzw. ceil(4f) Treffer (höchstens 5, f mit Gebäudeschutz), Radius 2–18 m; Wrack-Explosion 8 m / 100; keine Crash-Schäden an zivilen Autos. |
| `wanted_dispatch_guard` | Polizeidrohnen erst nach Anfahrzeit sichtbar, „DISPATCH“-HUD | gebaut | `game_gaps.cpp`, `game_town.cpp` | Drohne wartet unsichtbar am Spawnpunkt bis Spawn + Anfahrzeit (16 s − 1,3 s·Stufe, ≥ 9 s), dann Gnadenfrist 720 − 45·Sterne (≥ 450 ms) + 120 ms je Index bis zum ersten Schuss; HUD „POLICE DISPATCH · n DRONES INBOUND · x s“. Neu in sc-m3-gaps. |
| `wanted_police_drones` | Polizeidrohnen | gebaut | `game_town.cpp`, `game_details.cpp`, `game_gaps.cpp` | Dazu `reportCrime`: jede Tat bei Fahndung setzt letzte Sichtung und Ort auf den Spieler, ein neuer Stern spielt den Rechteck-Chirp 520 + 90·s → 760 + 100·s Hz (0,16 s) – neu in sc-m3-gaps. Werte geprüft: 5 Drohnen, 100 HP, 34 je Treffer, 9 s Neustart, Sensor 170 ms, Reaktion 750 / Salve 650 ms, Spieler-Treffer-Pause 1050 ms stimmen; korrigiert: EMP-Stoß waagerecht + max(16, 0,16·J) N·s nach unten auf 18 kg (vorher 3D + 1,5 m/s hoch auf 6 kg), HP auf ≤ 34, Fall mit 1,35 g, Neustart nach EMP 12 s; jede Explosion trifft sie (min(3, round(Betrag/34)) × 34). |
| `wanted_police_fairness_v5` | Fairness-Regeln Polizei | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code. |
| `wanted_police_pre_guard` | Polizeidrohnen sehen nur in Feuerreichweite | gebaut | `game_gaps.cpp`, `game_town.cpp` | Sicht nur bis min(35, 28 + 1,4·Stufe) m und nur, wenn die Drohne schon da ist. Neu in sc-m3-gaps. |
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
| `world_critical_damage_fx` | Rauch (dunkle Scheiben) und Glut vor der Explosion | gebaut | `game_gaps.cpp`, `game_details.cpp` | Rauchwolken je Effekt: Takt 155 − 72·Fortschritt ms, Glut ab 67 % (jede 5.), Lebensdauer 1,25–1,77 s, Pool 78 (mobil 48). Neu in sc-m3-gaps. |
| `world_crowd_density_v1` | Passanten-Dichte | gebaut | `people.cpp`, `game_town.cpp` | Passanten sind eigene Agenten, keine an Läufer geklebten Zusatzleute (das, was v1 im Original abschaltet). |
| `world_destruction_state` | Gebäudeschaden (Dach tiefer, eingeebnet) | gebaut | `city_stream.cpp` | Der Schaden steht am Kartenmerkmal des Gebäudes (eine Wahrheit für Grafik, Kollision, Minimap) und überdauert neu geladene Kacheln bis RESET; im Mehrspieler bricht jede Waffenexplosion dieselben Häuser auf allen Rechnern. |
| `world_ground` | Bodenfarbe aus Luftbildern (Albedo je 5-m-Knoten) | gebaut | `terrain.cpp`, `web/host.mjs` | Luftbild-Kacheln (Esri World Imagery, z15) der 1600 m um den Spieler, neu nach 350 m; 64-px-Mosaik je Kachel, 3×3 linear gemittelt, albedo(); Flächen färben darüber (×0,45 + Farbe ×0,55); ohne Bild die Ersatzfarbe (.16, .2, .1). |
| `world_impacts` | Spuren jeder Aktion | gebaut | `game_world.cpp`, `world_marks.cpp` |  |
| `world_liveliness` | Lebendigkeit der Welt | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code (in der Liste von sc-m3-life). |
| `world_location_selector` | Ortswahl | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `world_map_full` | Große Karte | gebaut | `game_net.cpp`, `web/mp_host.mjs` |  |
| `world_options` | WELT-Schalter | gebaut | `game_ui.cpp`, `mp_rules.h` |  |
| `world_options_menu` | WELT-Menü | gebaut | `game_ui.cpp`, `game_equipment.cpp`, `web/ui.mjs` | README „Menüs, Einstellungen, Ausstattung“ (sc-m3-ui). |
| `world_organic_hit_audio` | Knacken beim Tod von Mensch/Tier | original | `game_gaps.cpp` | Auf der Seite läuft das Original unverändert (`web/audio/world_organic_hit_audio.mjs`, gespeist von `web/audio.mjs` / `game_audio_web.cpp`); der Kern-Nachbau links bleibt für `?audio=kern` und native Builds. |
| `world_person_ragdoll` | Box3D-Ragdoll | gebaut | `game_life.cpp` | README „Ragdolls“: Pool 10 (mobil 6), Reverse Ragdoll 1,15 s (sc-m3-life). |
| `world_population` | Alte Bevölkerung | entfällt |  | Im Original nicht geladen (von `drone_simulator.html` aus nicht importiert, Prüfung per Import-Graph): toter Code. |
| `world_procedural_population` | Bevölkerung: Leute, Verkehr | teilweise | `game_town.cpp`, `game_traffic.cpp`, `people.cpp` | Leben gebaut. Fehlt: gleiche Passanten auf allen Rechnern (im Original deterministische Agenten mit gemeinsamer Id, deren Tod per `npc-death` an die Mitspieler geht); auf Kern sind Passanten je Rechner zufällig. Werte geprüft (Treffer): 100 HP (vorher 40); ein Geschoss 55 (Sniper 200), Kopf 165 (MP) / 999 (Glock, Sniper); nicht tödlich = Ragdoll, steht wieder auf, liegend kein Ziel; Explosion: ein Treffer bei Abfall > 0,22; Faust/Messer: punch() (bei ≤ 35 HP zu Boden, sonst Taumeln 0,22·max(0,5, N·s/30) m). Sichtweiten Telefon 140 / 160 m (vorher 170 / 190 wie Desktop). Verkehr wie im Original: 42 Autos + 8 Busse (Telefon 28 + 5), Platz 45–240 m, Versetzen ab 330 m; Physik nur in der Nähe (PHYS_NEAR_M 150 / PHYS_FAR_M 180), weiter weg kinematische Spurfolger (game_traffic_far.cpp); KI-Reifen mit 60 Hz wie FIXED_DT 1/60 (dazwischen gehaltene Kräfte). Messung Physik je Frame (Headless, Stufe 2): 11 Autos 4,65 ms → 50 Autos 8,74 ms (ohne 60 Hz 20,2 ms). Passanten 100 Plätze (Telefon 68) unabhängig von der Qualitätsstufe. |
| `world_rigid_body_physics` | Box3D-Welt der Körper | gebaut | `game_world.cpp`, `common.h` |  |
| `world_rigid_body_runtime` | Fester Physikschritt, Fehlerfang | gebaut | game.cpp (Logic-Knoten, `240 Hz)` | Fester Schritt über Kern; Kern fängt Fehler im Schritt selbst. |
| `world_spawn_guard` | Startpunkt nicht in einem Gebäude | gebaut | game.cpp (`clear_spot`) |  |
| `world_sync` | Atombombe und Gebäudeschaden an Mitspieler | entfällt |  | (c) synchronisiert nur Nuke und Nuke-Gebäudeschaden (`world_destruction_state`), beides im Original nicht erreichbar. |
| `world_traffic_motion_guard_v1` | Gerade richten schiebender Autos | entfällt |  | (b) Korrektur für SpawnCitys Fahrer-KI; Kerns Fahrer folgen der Spur (README Verkehr). |
| `world_traffic_routes` | Routen aus Kacheln | gebaut | `city_stream.cpp` |  |
| `world_transform_numeric` | Numerische Posen ohne three.js | entfällt |  | (b) Hilfe für den nativen Renderer. |
| `world_vehicle_dynamics` | Fahrzeugmodell | gebaut | `car.cpp`, `car.h` |  |
| `world_water` | Wasser | teilweise | `terrain.cpp`, `city_stream.cpp`, `game_world.cpp` | Physik und Fläche gebaut; Wellen-Normalen, Tiefenfarbe und Ufer-Schaum im Shader fehlen. |
| `xbox_control_mode` | Xbox-Modus | gebaut | `controls.cpp`, `game_controls.cpp`, `touch_play.cpp`, `web/host.mjs` | CONTROLS.md (sc-m3-controls): jede Eingabe mit Quelle `datei.mjs:Zeile`. |
| `xbox_gamepad` | Gamepad | gebaut | `game.cpp` | Totzone 0,14 |
| `zombie_nights` | Zombie-Nächte | teilweise | `combat_rules.cpp`, `game_events.cpp`, `game_barricades.cpp` | Runden; Barrikaden (6 Bretter je Tür des begehbaren Hauses bei Nacht, ein Brett je Tür alle 3,4 s, danach rein und durchs Haus, kein Biss durch intakte Bretter, REPARIEREN/F für 10 Punkte in 3,2 m) neu in sc-m3-gaps. Fehlt: Schnappschüsse an Mitspieler (`zombie-sync`/`zombie-hit`). |
