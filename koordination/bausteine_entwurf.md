# Bausteine – Entwurf und Bestandsaufnahme (26.11.2026)

Lesestand: `origin/work-projekt` ac491f28 (Worktree /home/claude/w-bausteine, nur gelesen). Pfade relativ zu `kern/`,
SpawnCity = `kern/games/spawncity/`. Zeilen = `wc -l`. Nichts gebaut, nichts geändert.

Grundlage: Entscheidung des Besitzers (26.11.) – Soldat, Waffen, Micro Duck, Autos … sind **mitgelieferte Bausteine** der
Engine, nicht Spielcode. Zwei Schichten: **Kern** (Laufzeit, Physik, Renderer, Sprache, Editor – kein Wort von
Soldat/Waffe/Ente) und **Bausteine** (Pakete aus Vorlagen, Blueprint-Bibliotheken, Assets). Regeln: nur öffentliche
Schnittstelle, Blueprints statt Sondercode, je Projekt abwählbar.

„Öffentliche Schnittstelle“ heißt hier genau: was ein Blueprint erreicht – die Knoten der Engine (`tools/engine_api.py`,
heute 820 Knoten, u. a. 135 `game_world.*`, 73 `world.*` der Physik, 49 `renderer.*`, 12 `character_mover.*`), die
Gameplay-Knoten (`include/kern/game/blueprint_nodes.h`) und die Typen mit `KERN_BLUEPRINT_TYPE`. Was nur über
`runtime_internal.h`, private Felder von `GameWorld` oder ungekennzeichnete Typen (`Level`, `Actor`, Komponenten) geht,
ist **Innerei**.

---

## 1. Bestand

### 1.1 Übersicht

| Kandidat | Ort heute | Zeilen | Art | Innereien (warum heute nicht öffentlich) | Daten/Assets | wie allgemein |
|---|---|---|---|---|---|---|
| **Soldat / Fuß-Steuerung** | `src/game/soldier.cpp` (648), Teile von `runtime.cpp` (Fall `ControlMode::soldier`/`character`, 23 Stellen), `include/kern/game/level.h` (`ControlMode`, Soldat-Felder im `ControllerComponent`), `runtime.h` (`PlayInput.view_pitch/speed_scale/look_stick_*`, `soldier_camera`, `aim_blend`, `aim_view_model`, `set_view_hand`) | ~900 + Verzweigungen | Engine-C++ | Methoden von `GameWorld` auf privaten Feldern (`pw_` 104×, `hud_` 48×, `health_`, `camera_yaw_`, `recoil_*`, `bob_*`, `fov_now_`); `Pawn` (intern) mit `CharacterMover`; Eingabe als festes `PlayInput` je Pawn | keine eigenen | Der **Bewegungsteil** (CharacterMover: Gehen, Sprint, Ducken, Sprung, Stufen) ist allgemein und schon öffentlich (`character_mover.*`, `physics.h:357ff.`). Soldat-eigen: Rückstoß, Zielen über Kimme, Kopftreffer, Respawn-Regeln. SpawnCity nutzt ihn als Fußgänger (`world_->soldier_state`, `damage`, `shots`, `apply_soldier_state`: game_controls.cpp, game_events.cpp, game_audio_web.cpp). |
| **Waffen (hitscan)** | `soldier.cpp` `fire_weapon`, `damage`, `update_health`; `level.h` `WeaponKind`, `WeaponComponent`, `weapon_preset`, `HealthComponent`, `PickupComponent.ammo/heal`, `TriggerAction::damage`; Knoten `combat_damage`, `combat_ammo`, `actor_health`, `actor_explode` | ~350 + ~80 Decl. | Engine-C++ | private Felder; Lag-Kompensation des Servers im Inneren; `ShotEvent`-Liste `shots_` | Klänge im Synth: `src/audio/synth.cpp` „SpawnCity's combat bank“ (~250 von 1.088) | Hitscan + Schaden + Munition ist für jeden Shooter gleich; die vier festen `WeaponKind` und ihre Zahlen sind Daten. SpawnCitys weitere Waffen (`equipment`, `guided`, `combat_rules`, `game_weapons`, ~1.700) bleiben Spiel. |
| **Soldat-Darstellung** | `src/game/soldier_art.cpp` (729), `soldier_models.{h,cpp}` (855) | 1.584 | Engine-C++ | `runtime_internal.h`; Renderer direkt (`upsert`, `remove`, `allocate_id`, Knochen); Mündungsfeuer/Tracer/Hülsen/Decals | prozedurale Modelle (Soldat, MP, Pistole, Sniper, Werfer, Handschuhe) | Inhalt (Aussehen) – gehört als **Asset** in den Baustein, nicht als C++-Bauplan. |
| **HUD** | `runtime.h` `struct Hud` (Soldat-Felder: health, ammo, magazine, reload, spread, hit_*, damage_*, respawn_in, aiming, fov), `SoldierState`; `web/studio/game.mjs` `soldierHtml` (~150 von 1.644); `net_game.cpp` `serialize_soldier`, `has_soldier` (~40) | ~250 | Engine-C++ + JS | `Hud` ist `KERN_BLUEPRINT_TYPE`, aber fest; Zeichnung im JS-Host des Editors am App-Widget-Weg vorbei | – | Allgemein ist nur: „Werte einer Figur anzeigen“ (app.md 6: Widgets). Fadenkreuz/Trefferanzeige/Vignette sind Baustein. |
| **Micro Duck / Walker** | `src/game/walker.cpp` (1.572), `walker_brain.cpp` (1.053), `walker_policy.cpp` (469), `walker_play.cpp` (310), `servo.cpp` (78), `include/kern/game/walker*.h`, `servo.h`; `runtime.h` ~40 `walker_*`-Methoden und Typen (`WalkerSensors`, `WalkerProbe`, `WalkerJoint`, `DetectorModel`), `ControllerComponent` Walker-Felder, `MotionKey.joints[14]`; Knoten `walker_*` (12) | ~3.550 | Engine-C++ | `runtime_internal.h` (`WalkerBrain`, `WalkerMove`), eigene Servo-Treiber im Schritt (`drive_servos`), Beobachtung 61 Werte aus Innerem, eigene Kopfkamera | `micro_duck_velstand.inc` (24.770) + `micro_duck_model.inc` (198) einkompiliert; `third_party/microduck/` 14 MB (ONNX-Netze, Meshes, Apache 2.0 Pollen Robotics); `src/audio/duck_voice.cpp` (780); `kern/policies/` | Nichts davon allgemein außer: **Servo-Gelenk** (Physik, bleibt Kern), **MLP-Netz ausführen/ONNX lesen** (Kern: `lehrer/nn`), Gitter-ToF = Strahlen. 44 Engine-Tests `micro_duck_*`/`lehrer_*` in `tests/test_main.cpp`. |
| **Lehrer (RL)** | `src/lehrer/*` (3.770), `include/kern/lehrer/*`, `src/studio/trainer.cpp`, `web/studio/trainer.mjs`, `tools/kern_lehrer.cpp` | ~6.350 | Engine-C++ | `duck_env.cpp`, `spielen`, `kalibrierung` greifen in Walker-Innereien (`set_walker_external`, `walker_observation`, `MotionKey[14]`) | Netze in `kern/policies` | PPO/CMA-ES/NN/ONNX allgemein (Kern); die Umgebung ist Duck (Baustein). |
| **Fahrzeug (Kern)** | `runtime.cpp` `ControlMode::hovercar/vehicle` (Kraft am Schwerpunkt, Seitenhaftung), Kamera chase/interior | ~200 | Engine-C++ | `pw_`, `physics_` direkt | `engine_sound`-Loop | Arcade-Modell, schwächerer zweiter Weg neben SpawnCitys Auto. |
| **Fahrzeug (SpawnCity Auto/Bus)** | SpawnCity `src/car.{h,cpp}` (587); als Blueprint-Bibliotheken `car` + `car_cpp` (Ordner „Source“, `level/spawncity.klevel`); Vorlagen `Car`, `Bus` (Ordner „Vehicles“) | 587 | C++ **und** Blueprint (konvertiert) | **keine** – nutzt nur `physics::World` (öffentlich: `create_body`, `create_joint` wheel/distance, `set_wheel_steering`, `apply_force`, `pose`); aber eigene Rad-Körper **neben** dem Akteur-System (nicht als Kind-Akteure) | Zahlen in `CarSpec` (car/bus) | Sehr allgemein (Reifenkurve, ABS, Servolenkung, Federung auf Box3D). SpawnCity-eigen: `rigid_spec`, `common` (Spiel-Bibliotheken), Treibstoff/Schaden im Spiel. Bester Kandidat. |
| **Jet / Sitze** | SpawnCity `jet.{h,cpp}` (405), `jet_seats.{h,cpp}` (131), Blueprints `jet`, `jet_cpp`, `jet_seats` | 536 | C++ + Blueprint | keine (Physik öffentlich) | – | Flugmodell = optionaler Baustein; „Sitze/Einsteigen“ allgemein für jedes Fahrzeug. |
| **Ragdoll** | SpawnCity `ragdoll.{h,cpp}` (365), Blueprints `ragdoll`, `ragdoll_cpp`, `PeopleArt_ragdolls` | 365 | C++ + Blueprint | keine (Box3D-Gelenke öffentlich); Darstellung über `people_art` (Spiel) | Farben der Figur (`Colors`) | Allgemein (Figur fällt, steht wieder auf). Braucht ein Skelett, auf das es wirkt (heute SpawnCitys Personenmodell). |
| **Drohne** | Kern: `ControlMode::flyer` in `runtime.cpp` (~60), Katalog „Drone“; SpawnCity: `drone.{h,cpp}` (488) mit `firmware/` | 60 / 488 | Engine-C++ / Spiel | Kern-Flyer: `pw_`; SpawnCity-Drohne: Firmware (Regel: nicht ändern) | – | Kern-Flyer allgemein (klein); SpawnCity-FPV-Drohne bleibt Spiel. |
| **Kamera-Rigs** | Kern: `CameraMode {chase, first_person, top_down}`, `GameWorld::update_camera`, `soldier_camera`, Fahrzeug-Innenansicht, `shake_camera`; SpawnCity `look.{h,cpp}` (303), `game_look.cpp` (822), `game_vr.cpp` (697) | ~400 / 1.800 | Engine-C++ / Spiel | Kamera läuft je **Frame** in C++ (`update_camera`), Welt-Blueprints nur im festen Schritt; `set_view` ist Knoten, aber ohne Frame-Ereignis nutzlos | – | Verfolger/Ego/Draufsicht/Orbit + Wackeln/FOV-Kick: allgemein. FPV-Stabilisator, Tag/Nacht-Look: Spiel. VR-Rig: allgemein, später. |
| **Pickups / Trigger** | Kern: `PickupComponent`, `TriggerComponent` (goal, hazard, checkpoint, message, launch, teleport, damage, event), `GameGoal`; Katalog Orb, Star, Health Pack, Ammo Box, Jump Pad, Teleporter, Damage Zone, Target Dummy, Explosive Barrel (`src/studio/catalog.cpp` 654) | ~150 + Laufzeit | Engine-C++ (Daten-Komponenten) | private Behandlung in `handle_events`/`collect`/`trigger` | Klänge `builtin:*` | Pickup/Trigger als Komponente: allgemein, **bleibt Kern**. Aber `ammo`, `heal`, `damage` gehören zu Waffen/Leben; die Katalog-Einträge sind schon „Presets in C++“ – sie werden Vorlagen eines Grund-Pakets. |
| **Netz-Sitzung / Lobby** | SpawnCity `net_session.{h,cpp}` (1.327), Blueprint `net_session` (82 kB); dazu `wire.h` (199, zweiter Serialisierer), `mp_rules`, `jet_seats` | 1.327 | C++ + Blueprint | keine Engine-Innereien (nutzt `kern/net/transport`, `client_server` – öffentlich); aber SpawnCity-Bibliotheken `mp_rules`, `jet_seats`, `wire` | Signalisierung/Raum-Codes; QR-Blueprint `qr_code` im Projekt | Räume, Host, Beitreten, Neuverbinden: allgemein. Regeln, Sitze, Nachrichtenarten: Spiel. |
| **Kern-Netz-Spiel** | `src/game/net_game.cpp` (653) | 653 | Engine-C++ | repliziert Posen, Property-Pfade, Spawns, **festen Soldaten-Block**; **keine Blueprint-Variablen** | – | Kern (allgemein), bis auf `serialize_soldier`. |
| **Gelände / Kacheln / Straßen** | SpawnCity `terrain.{h,cpp}` (1.668), `tiles` (641), `mvt` (459), `road_graph` (207), `city_stream` (1.406), Blueprints `terrain`, `tiles`, `mvt`, `road_graph`; Kern `geo/roads.h`, `geo/buildings.h`, `CityComponent` + `geometry.cpp build_city` (Orb-Run-Stadt), `NightWindows` | ~4.400 / ~250 | C++ + Blueprint / Engine-C++ | terrain: Renderer direkt, `core/jobs`, Netz-Upload; Physik: **kein Heightfield** (`ShapeKind {box, sphere, capsule, hull, mesh}`) | Höhen-/Vektorkacheln aus dem Netz | mvt/tiles: reine Algorithmen (allgemein, Bibliothek). Terrain-Höhenfeld: allgemein, aber an Kacheln gebunden. City-Stream, OSM-Schichten: Spiel. Orb-Run-Stadt: Baustein (Beispiel). |
| **Touch / Steuerkurven** | Kern `src/game/touch.cpp` (438, „nach SpawnCitys foot_touch_controller“), `src/studio/ui.cpp`/`viewport.cpp` SpawnCity-Konstanten; SpawnCity `controls` (381), `game_controls.cpp` (1.367), `touch_play` (639) | ~550 / 2.400 | Engine-C++ / Spiel | Kern-Touch kennt `ControlMode` (14 Fälle) | Kurven, Layouts | Aktionen sind schon Kern (app.md 4); Touch-Layout je Spielfigur ist Teil des Bausteins (Daten). |
| **Kleinere** | SpawnCity `spawn_guard` (95, schon Blueprint-Akteur), `perf` (181), `sound_fx` (213), `world_marks` (467, Decals), `water_look` (94) | ~1.000 | C++ + Blueprint | sound_fx: Synth öffentlich; world_marks: Renderer | – | spawn_guard, perf, sound_fx „Motor als Loop“: allgemein, kleine Bausteine. |

### 1.2 Befunde, die den Entwurf bestimmen

1. **Zwei Herkünfte.** Was im Kern liegt (Soldat, Waffen, HUD, Duck, Kern-Fahrzeug, Flyer, Kamera) ist C++ auf
   privaten Feldern von `GameWorld` – es muss **neu als Blueprint** geschrieben werden (oder konvertiert, nachdem es
   aus `GameWorld` herausgelöst und auf öffentliche Aufrufe umgeschrieben ist). Was in SpawnCity liegt (Auto, Ragdoll,
   Jet, Sitze, Sitzung, mvt/tiles/road_graph/terrain) ist **schon Blueprint** (Konverter-Ausgabe im Level, Ordner
   „Source“) und nutzt fast nur Öffentliches (Physik-Knoten) – dort ist die Arbeit: Spiel-Bibliotheken (`common`,
   `rigid_spec`, `mp_rules`, `wire`, `shapes`) herauslösen und in Akteure/Vorlagen fassen.
2. **Der Editor-Katalog ist heute schon „Presets in C++“** (`catalog.cpp`: 50 Einträge in 7 Gruppen, jeder ein
   handgebauter `game::Actor`). Die Bausteine-Schicht ersetzt ihn – ein Weg statt zwei.
3. **Geschlossene Listen im Kern** sind die Ursache, warum Soldat und Duck dort gelandet sind: `ControlMode`
   (7 Fälle, verzweigt in runtime.cpp, touch.cpp, catalog.cpp, studio.cpp, api.cpp, level.cpp, logic_world.cpp),
   feste Komponenten in `Actor` (`std::optional<…>`), `PlayInput` mit festen Feldern, `Hud` mit festen Feldern,
   `game.input` mit sieben festen Namen (`logic_world.cpp:74`).
4. **Ein Akteur hat genau einen Blueprint** (`Actor::logic`). Zusammensetzen geht über Bibliotheken (`uses`) und
   Kind-Akteure (Waffe als Kind der Hand) – das reicht, siehe 2.6.
5. **Engine-Bibliotheken** gelten nur als Kopie-frei, wenn das Level sie nicht hält. Im SpawnCity-Level steht noch ein
   Akteur `engine_geo_roads` (eine Kopie) – nicht meins, nur Befund für den Leser-Agenten.

---

## 2. Entwurf der Schicht

### 2.1 Was ein Baustein-Paket ist

Ein Verzeichnis, das die Engine mitliefert:

```
kern/bausteine/<id>/
  <id>.kpackage          Paket-Datei (JSON, "format": "kern-package") – die eine Quelle
  assets/                Meshes (.glb), Klänge, Texturen, Netze (.onnx) – Dateien, nie einkompiliert
  beispiel.klevel        kleines Beispiel-Level (nutzt nur dieses Paket und seine requires)
  test/                  deterministischer Test (siehe 2.8)
  README.md              Doku (siehe 2.9)
```

Paket-Datei – bewusst wie ein kleines Projekt, mit denselben Lesern:

```json
{
  "format": "kern-package", "version": 1,
  "id": "fahrzeug",                       // [a-z][a-z0-9_]*, zugleich Präfix aller Namen
  "name": "Fahrzeug", "release": 3,       // Fassung des Pakets (ganze Zahl, steigt bei jeder Änderung)
  "requires": [ { "id": "kamera", "release": 2 } ],
  "engine": { "engine_physics_physics": "<marke>", … },   // Marken der Engine-Bibliotheken, die es nutzt
  "input": { "actions": [ … ], "axes": [ … ] },           // Aktionen, die es erwartet, mit Vorgabe-Bindungen
  "templates": [ { "name": "fahrzeug_Auto", "template": true, "components": { …, "logic": { … } } } ],
  "blueprints": [ { "name": "fahrzeug_Reifen", "logic": { "library": true, … } } ],
  "program":   [ ],                       // Programm-Blueprints (nur Pakete wie „sitzung“)
  "assets": { "meshes": [ … ], "sounds": [ … ], "textures": [ … ], "files": [ … ] },
  "license": { "spdx": "Apache-2.0", "assets": [ { "path": "assets/auto.glb", "spdx": "CC0-1.0", "by": "…" } ] }
}
```

- **Vorlagen** sind Akteure wie im Level (`"template": true`, runtime_actors.md), samt Kindern, Komponenten, Blueprint.
- **Blueprints**: Bibliotheken (Typen, Funktionen = Knoten) und Welt-Blueprints, die die Vorlagen tragen.
- **program**: Programm-Blueprints, die in die Programm-Laufzeit gehen (Lobby, HUD-Zeichner) – Einordnung wie
  project.md 7a (was ein Programm-Blueprint ist, folgt aus dem, was er ist, nicht aus dem Ordner).
- Unbekannte Schlüssel sind Fehler mit Ort (app.md 2, `json::unknown_key`), wie bei Level und Projekt.

**Namen.** Jeder Name eines Pakets (Vorlage, Blueprint, Typ auf oberster Ebene) beginnt mit `<id>_`. Ein Projekt oder
Level darf keinen Namen haben, der mit dem Präfix eines **gewählten** Pakets beginnt; zwei Pakete haben verschiedene
Ids. Damit kann ein Name nicht an zwei Orten stehen – nach Bau, nicht nach Glück; die Prüfung meldet es trotzdem
ausdrücklich (beide Stellen, wie `library_conflicts`).

### 2.2 Wie die Engine sie mitliefert

- Quelle: `kern/bausteine/<id>/` im Repo; Engine-Bau bettet **nichts** davon ein.
- Der Editor (Web) bekommt einen Index `kern/bausteine/index.json` (erzeugt, `*.gen.*`-Regel) mit Id, Name, Fassung,
  Kurzbeschreibung, Gruppe, Symbol – lädt den Paket-Inhalt erst bei Wahl.
- Ein exportiertes Spiel trägt nur die gewählten Pakete (siehe 2.4).
- Kein C++ in Paketen (siehe 5.3). Was ein Paket an Rechenkraft braucht, kommt aus der Klasse des Blueprints
  (blueprint_cpp, beim nativen/`--classes`-Bau) oder aus einer allgemeinen Kern-Funktion.

### 2.3 Wählen und abwählen

Im Projekt (`.kproject`), neuer Schlüssel:

```json
"packages": [ { "id": "spielfigur", "release": 4 }, { "id": "fahrzeug", "release": 3 } ]
```

- Fehlt ein `requires`, ist das ein Fehler mit Grund („fahrzeug needs kamera (release ≥ 2) – add it to packages“), kein
  stilles Nachladen. Der Editor bietet beim Wählen an, die benötigten mitzuwählen (sichtbar).
- **Aktionen:** Wählen schreibt die Aktionen/Achsen des Pakets, die das Projekt noch nicht hat, in
  `settings.input` (sichtbar, ein Schritt im Verlauf, danach Projekt-Daten). Beim Lesen gilt nur das Projekt; fehlt
  eine Aktion, die ein Paket braucht: Fehler mit Paket, Aktion und vorhandenen Namen. So steht jede Aktion an einem Ort.
- **Abwählen:** verboten, solange das Projekt oder ein Level etwas davon benutzt (Akteur aus seiner Vorlage, `uses`,
  `spawn`-Name) – die Meldung nennt die Benutzer (wie beim Entfernen einer benutzten Bibliothek, studio.md).

Was das heißt:

| | gewählt | nicht gewählt |
|---|---|---|
| Bau (`build_native.sh`, `build.sh --project`, `export_game.sh`) | `embed_project.py` bettet die Paket-Dateien + Assets ein; `--classes` kompiliert ihre Blueprints mit | nichts davon im Modul – weder Bytes noch Klassen |
| Größe | Paket + Assets | 0 (heute: Duck 25.000 Z. + Soldat in **jedem** Engine-Bau) |
| Knotenliste (Palette) | Funktionen ihrer Bibliotheken, wo ein Blueprint sie `uses` (wie Projekt-Bibliotheken, project.md 6.2) | unsichtbar |
| Place Actors | Vorlagen unter ihrer Gruppe | nicht da |
| `spawn("fahrzeug_Auto")` | findet die Vorlage | Fehler mit den Vorlagen, die es gibt |

### 2.4 Bibliothekssuche – wo die Pakete stehen

Heute (project.md 5, blueprint_cpp.md „Bibliotheken der Engine“): Welt-Blueprint → Level, dann Projekt; Programm →
nur Projekt; ein Name der Engine (`engine_*`) ist immer die Engine (`with_engine`).

Neu, eine Kette ohne Überdeckung:

```
Welt-Blueprint:    Level  →  Projekt  →  gewählte Pakete  →  Engine
Programm-Blueprint:           Projekt  →  gewählte Pakete  →  Engine
Paket-Blueprint:                         eigenes Paket + seine requires  →  Engine
```

- Weil die Namensräume durch Präfixe getrennt sind (`engine_`, `<paket>_`, alles andere), ist die Reihenfolge nur die
  Suchreihenfolge, nie ein Gewinner: Steht ein Name doch an zwei Orten, ist es ein Fehler beim Laden mit beiden Stellen.
- Ein Paket nutzt **nie** Projekt oder Level (sonst wäre es nicht abwählbar/kopierbar) – geprüft beim Lesen des Pakets.
- Ein Level/Projekt hält **keine Kopie** eines Paket-Blueprints, nur `uses` und die Marke (wie bei Engine-Bibliotheken:
  `"packages": {"fahrzeug_Reifen": "<marke>"}` im Graph). Passt die Marke nicht, sagt die Meldung, was sich geändert hat.
- Umsetzung: eine Funktion `with_packages(blueprints, project)` neben `with_engine`, die jeder Leser geht
  (libraries_of, take_libraries, link, from_cpp, apply_cpp) – kein zweiter Leseweg.

### 2.5 Vorlagen aus Paketen im Level

Zwei Wege wären möglich (Kopie ins Level oder Verweis); **ein** Weg wird gewählt: **Verweis.**

- Ein Akteur im Level kann **Instanz einer Paket-Vorlage** sein: `{"name": "Auto 1", "instance": "fahrzeug_Auto",
  "transform": …, "set": { "MaxSpeed": 40 } }` – er hält nur Name, Lage, Eltern, Ordner und geänderte
  Variablen-Vorgaben (`set`, geprüft gegen die Variablen des Blueprints der Vorlage, Fehler mit Namen). Beim Start
  macht die Welt daraus die Kopie, wie `spawn` sie macht (`place_copy`, gleiche Ids-Regel). Komponenten einer Instanz
  sind die der Vorlage (nicht änderbar; wer das will: kopieren, 2.7).
- `GameWorld::spawn(name)` sucht Vorlagen in Level **und** gewählten Paketen (Namen getrennt durch Präfix).
  `world.pawnTemplate` darf eine Paket-Vorlage nennen.
- Ziehen aus der Bibliothek legt so eine Instanz an.

### 2.6 Zusammensetzen, ohne den Kern zu öffnen

- **Ein Blueprint je Akteur bleibt.** Zusammengesetzt wird über **Kind-Akteure** (Waffe als Kind der Figur, Räder als
  Kinder des Autos, Kamera-Rig als eigener Akteur) und **Bibliotheken** (`leben_Leben` als Struct-Variable + Funktionen).
- Verbindung zwischen Bausteinen über **Schnittstellen** (`kern::Implements<…>` gibt es für Engine-Schnittstellen;
  für Blueprint-Schnittstellen zwischen Paketen siehe Lücke L9) oder über **Call Event** mit festen Namen, die ein Paket
  dokumentiert (`Damage(amount, instigator, head)`, `Possess(pawn)`).

### 2.7 Editor

- **Bibliothek** (Content Drawer, ersetzt „Place Actors“ aus `catalog.cpp`): Gruppen der gewählten Pakete; Ziehen in
  den Viewport legt eine Instanz an (2.5). Die Grundformen (Cube, Light, Physics Cube …) werden selbst ein Paket
  `grund` (immer gewählt im neuen Projekt, aber abwählbar) – damit hat der Editor **keinen** festen Katalog mehr.
- **Pakete** in den Projekt-Einstellungen: Liste aller mitgelieferten, Haken = gewählt, Fassung, Lizenz, „needs …“.
- **Schreibgeschützt:** Paket-Blueprints öffnen sich lesbar (wie „Engine“), mit Knopf **„In mein Projekt kopieren“**:
  kopiert Vorlagen + Blueprints mit neuem Präfix (`<projektwahl>_…`, z. B. `mein_Auto`) ins Projekt/Level, schreibt
  `uses`/Instanzen des Projekts auf die Kopie um (ausdrücklich: Liste der Änderungen vorher zeigen), Assets in den
  Projektordner. Danach ist es Projekt-Inhalt; das Paket kann abgewählt werden.
- **Neues Level aus Vorlage** (heute `demos.cpp`: Orb Run, Physics Lab, Shooting Range, Micro Duck): die
  `beispiel.klevel` der Pakete; `demos.cpp` entfällt (Level als Dateien).

### 2.8 Fassung und Kompatibilität

- `release` (ganze Zahl) je Paket; das Projekt nennt die gewählte. Die Engine liefert je Paket **eine** Fassung (die
  aktuelle). Ist sie höher als die des Projekts: Laden geht, wenn die Marken aller benutzten Blueprints passen; sonst
  Fehler „fahrzeug: written with release 3, the engine has 4 – <was sich an fahrzeug_Reifen geändert hat>“ und das
  Angebot im Editor, auf 4 zu heben (ein Schritt, sichtbar). Ist sie niedriger: Fehler („written by a newer Kern“).
- Kein „alte Fassungen parallel“ – wer eine alte braucht, kopiert ins Projekt (2.7).
- Die Marken der Engine-Bibliotheken im Paket (`"engine"`) sagen, gegen welche Engine es geschrieben ist (vorhandener
  Mechanismus).

### 2.9 Tests und Doku

- **Je Paket ein deterministischer Test** `test/<id>_test.cpp` mit `KERN_TEST` (kern/test/test.h), gefahren von einem
  Engine-Lauf `kern/tests/run.sh bausteine[:id]` über `build_native.sh --project test/test.kproject --classes`:
  Beispiel-Level laden, N feste Schritte mit festem Eingabeskript (Aktionen), erwartete Werte prüfen
  (Lage, Geschwindigkeit, Treffer, Punkte) – im Interpreter **und** als Klasse, gleich Bit für Bit (wie heute die
  Spieltests von Murmel). Kein Test eines Pakets braucht `games/`.
- **Wächter des Kerns** (Engine-Test): `grep -i "soldier|weapon|ammo|duck|walker|spawncity"` in `include/` und `src/`
  (ohne `bausteine/`) ist leer. Wird mit jedem Schritt von Teil 4 enger.
- **Abnehmer Murmel**: Murmel wählt `spielfigur` (Kugel ist heute `ControlMode::rolling` → `spielfigur_Kugel`) und
  `kamera`; sein CI-Job beweist, dass ein zweites Spiel nur über Pakete auskommt. Später ein drittes Mini-Spiel
  „in einem Tag“ (Figur + Auto + Pickups) als Beleg des Ziels.
- **Doku je Baustein** `README.md`: was es ist, Vorlagen (Variablen mit Einheit und Vorgabe), Aktionen, Ereignisse/
  Schnittstellen, die es anbietet/erwartet, Beispiel-Level, Test, Lizenz der Assets, „In mein Projekt kopieren“.
  Dazu `kern/docs/bausteine.md` (die Schicht) und ein Abschnitt in project.md („packages“).

### 2.10 Fehler mit Ort und Grund (Liste)

Paket liest nicht (Stelle), Name ohne Präfix, Paket nutzt Projekt/Level, fehlendes `requires`, unbekanntes Paket
(Liste der vorhandenen), Fassung zu neu/zu alt, Marke passt nicht (was sich änderte), Name an zwei Orten (beide),
Abwählen eines benutzten Pakets (Benutzer), fehlende Aktion (Paket, Name, vorhandene), Instanz einer unbekannten
Vorlage (Vorlagen der gewählten Pakete), `set` auf eine Variable, die der Blueprint nicht hat (vorhandene).

---

## 3. Lücken der öffentlichen Schnittstelle

Was der Kern anbieten muss, damit jeder Kandidat nur über ihn auskommt. Umfang: klein (≤ 1 Tag), mittel (2–4 Tage),
groß (≥ 1 Woche).

| # | Lücke | was fehlt | wer braucht es | Umfang |
|---|---|---|---|---|
| L1 | **Eingabe je Pawn in der Welt** | Welt-Blueprints bekommen heute nur `game.input` mit 7 festen Namen (`logic_world.cpp:74`) und lesen kein `PlayInput`; die Aktionen des Projekts (app.md 4) gibt es nur an der `Engine` der App. Nötig: Knoten „Action/Axis of Pawn“ (`pawn_action(actor, name)`, `pawn_axis`) – der Wert der Aktion **für diesen Pawn** (lokal: diese Maschine; entfernt: was der Server vom Client bekam). `PlayInput` wird dadurch ersetzt durch „Aktionen eines Pawns“ (Name → Wert), repliziert. | Spielfigur, Fahrzeug, Drohne, Waffen, Kamera | groß |
| L2 | **Besitz (Possess) statt `ControlMode`** | „Dieser Akteur ist der Spieler“ ohne Bewegungsart: `ControllerComponent` ohne `mode` (oder mit `mode: "blueprint"`), der Blueprint des Akteurs bewegt ihn. Knoten Possess/Unpossess (Einsteigen ins Auto). | alle Spielfiguren | mittel |
| L3 | **Darstellung je Frame** | Ereignis „Event Present(dt, alpha)“ für Welt-Blueprints (nach `step`, vor `present`), das nur Darstellung ändern darf (Kamera, Mesh-Lage zwischen Schritten, Knochen) und **nicht** die Simulation – geprüft (kein Physik-/Variablen-Schreiben, das ins Snapshot geht), damit Determinismus bleibt. | Kamera, Soldat-Arme, Duck-Kopfkamera, Fahrzeug-Räder | mittel |
| L4 | **Kamera aus dem Blueprint** | `set_view(Camera)` gibt es als Knoten; fehlt: „die Kamera dieser Maschine gehört diesem Akteur“ (statt `update_camera` mit `CameraMode`), Kollision der Kamera (Kugel-Cast gibt es), FOV, Wackeln als Daten. | Kamera-Paket | klein (nach L3) |
| L5 | **Gelenke und Körper eines Akteurs als Handle** | `actor_body` gibt es; fehlt `actor_joint(actor, index)` → `physics::Joint` (für `set_wheel_steering`, `joint_angle`, Feder), und Körper der Kinder. Ohne das muss ein Auto seine Räder neben dem Akteur-System bauen (wie SpawnCity heute). | Fahrzeug, Ragdoll, Duck (Servos) | klein |
| L6 | **Mesh-Asset, Skelett, Animation** | Meshes nur eingebaut/prozedural; Skinning im Renderer da (`update_bones`, ≤ 32 Knochen), aber ohne Asset, Clip, Überblenden. Nötig: glTF-Import (Mesh, Material, Skelett, Clips) als Asset eines Levels/Pakets, Komponente „Skinned Mesh“, Knoten Play Clip / Blend / Set Bone. `MotionAsset` (14 Servowinkel) wird Clip auf beliebigem Skelett. | Soldat (Figur, Arme, Waffe), Ragdoll (wirkt auf Skelett), Duck (Modell), Auto (Modell) | groß |
| L7 | **Treffer-Abfrage mit Rückspulen** | `line_trace` mit Kanälen da; fehlt: Treffer gegen die Posen von vor Δt (Lag-Kompensation, heute im Soldaten-C++), Treffer mit Kollider-Kennung (Kopf: heute `headshot` an Kollider-Rolle). Knoten „Line Trace at Time“, Kollider-Tags. | Waffen | mittel |
| L8 | **Replikation von Blueprint-Variablen** | `net_game` repliziert Posen, Property-Pfade, Spawns und einen **festen Soldaten-Block**; keine Variablen. Nötig: Variable „replicated“ (Server → alle / → Besitzer), Ereignis „Server RPC“ vom Client (Schuss), deterministische Reihenfolge. Ersetzt `SoldierState`, `serialize_soldier`. | Waffen, Leben, HUD, Fahrzeug-Sitze, jedes Mehrspieler-Paket | groß |
| L9 | **Blueprint-Schnittstellen zwischen Blueprints** | `Implements` gibt es für Engine-Schnittstellen. Für „alles, was Schaden nehmen kann“ fehlt eine Schnittstelle aus einer Bibliothek (`leben_Beschaedigbar`), die ein Akteur-Blueprint implementiert und Call on Actor typisiert ruft. Ersatz bis dahin: Call Event mit dokumentiertem Namen. | Waffen↔Leben, Fahrzeug↔Sitze | mittel |
| L10 | **Widgets aus der Welt und mehr Widgets** | Widgets (app.md 6) nur an der `Engine` der App; „Play“ eines Levels im Editor hat keine App. Nötig: Widgets auch für Welt-Blueprints (über die Engine der Welt bzw. den Host des Editors) und die Arten Bild, Fadenkreuz/Form, Vollbild-Tönung (Treffer-Vignette). | HUD des Soldaten, Fahrzeug-Tacho, Duck-Kamerabild | mittel |
| L11 | **Effekte als Knoten** | Partikel-Burst da; fehlt: Decal (Einschuss), Spur/Tracer, kurzes Licht (Mündungsfeuer) als Renderer-Funktionen ohne Soldaten-Bezug (heute `soldier_art.cpp`, SpawnCity `world_marks`). | Waffen, Fahrzeug (Reifenspuren) | mittel |
| L12 | **Klang: Synth-Patches als Daten** | Synth öffentlich, aber die Kampf-Bank ist im Synth-C++ fest (`synth.cpp` 557ff.), Duck-Stimme ein eigenes C++-Modul. Nötig: Patch als Daten-Asset (Parameter des Synth), „Patch als Loop rendern“ (SpawnCity `sound_fx`). | Waffen, Fahrzeug-Motor, Duck-Stimme | mittel |
| L13 | **Netz (MLP) und ONNX als allgemeine Kern-Funktion** | `WalkerPolicy::run`/`parse_onnx_policy` stecken im Walker; `lehrer/nn` hat ein Netz. Nötig: ein Kern-Objekt „Netz“ (aus einer Asset-Datei .onnx laden, `run(obs) → action`, deterministisch, als Knoten). | Duck, jeder lernende Gegner | mittel |
| L14 | **Servo-Gelenk als Knoten** | Servo-Felder am Hinge gibt es im Level; der Treiber (`drive_servos`, Latenz) läuft im Walker-Inneren. Nötig: Servo-Ziel setzen/lesen je Gelenk (über L5) im Kern. | Duck, jeder Roboter | klein |
| L15 | **Heightfield** | Physik hat kein Höhenfeld (`ShapeKind`), Renderer kein Gelände-Mesh-Streaming. | Gelände-Paket | groß |
| L16 | **Lehrer-Umgebung allgemein** | `duck_env` greift in Walker-Innereien. Nötig: Schnittstelle „Umgebung“ (Beobachtung, Aktion, Belohnung, Zurücksetzen) als `Implements` eines Blueprints. | Duck-Training | groß |
| L17 | **Paket-Laufzeit** (die Schicht selbst) | `kpackage`-Leser, `with_packages`, Instanzen (2.5), `spawn` aus Paketen, Bau/Einbetten, Editor-Bibliothek, „kopieren“. | alle | groß |

Was **nicht** fehlt (schon öffentlich): Box3D-Physik ganz (`world.*`: Körper, Formen, Gelenke inkl. wheel und
Lenkung, Kräfte, Strahlen, Kugel-Cast, Explosion), CharacterMover, Renderer (Meshes, Knochen, Instanzen), Mixer,
Synth, SoundBank, Netz-Transport, Akteure (`spawn`, `destroy_actor`, `teleport`, Velocity, Tags, Call on Actor),
Speicher, Aktionen an der App.

---

## 4. Reihenfolge

Hebel für „ein zweites Spiel in einem Tag“. Vor allem: **L17 (die Schicht) + L1/L2/L3 (Eingabe je Pawn, Besitz,
Frame)** – ohne sie kann kein Spielfigur-Baustein ein Blueprint sein.

| Schritt | Baustein(e) | braucht | verschwindet aus dem Kern | SpawnCity danach |
|---|---|---|---|---|
| 0 | **Schicht** + Paket `grund` (Grundformen, Physik-Beispiele, Licht, Ton aus `catalog.cpp`), `spielelemente` (Orb, Star, Goal, Checkpoint, Jump Pad, Teleporter, Moving Platform …) | L17 | `catalog.cpp` (654), `demos.cpp` `orb_run_level`/`physics_lab_level` → Dateien | unverändert |
| 1 | **`spielfigur`** (Läufer 3rd-Person, Kugel, Flyer) + **`kamera`** (Verfolger, Ego, Draufsicht, Orbit, Wackeln) | L1, L2, L3, L4 | `ControlMode::character/rolling/flyer`, `CameraMode`, `update_camera`-Fälle, `touch.cpp` Modusfälle, `PlayInput`-Sonderfelder | unverändert; **Murmel** stellt auf `spielfigur_Kugel` + `kamera_Verfolger` um (Beleg) |
| 2 | **`fahrzeug`** (Auto, Bus aus SpawnCity `car`; Arcade-Hovercar) + **`sitze`** (Einsteigen, aus `jet_seats`) | L5, (L6 fürs Modell; vorher Primitive) | `ControlMode::hovercar/vehicle` + Fahrzeug-Kamera/Innenansicht | Vorlagen `Car`/`Bus` werden Instanzen von `fahrzeug_Auto`/`_Bus` mit SpawnCitys Zahlen als `set`; `car`/`car_cpp`/`rigid_spec`-Teil verschwinden aus dem Level |
| 3 | **`leben`** (Health, Rüstung, Respawn, Schadenszone) + **`waffen`** (hitscan, Magazin, Nachladen, Streuung, Rückstoß, Munitionskiste) + **`hud`** (Programm-Blueprint, Widgets) + **`soldat`** (= spielfigur + waffen + leben + kamera Ego, Arme, Figur) | L6, L7, L8, L9, L10, L11, L12 | `soldier.cpp`, `soldier_art.cpp`, `soldier_models.*`, `WeaponComponent`, `HealthComponent`, `PickupComponent.ammo/heal`, `TriggerAction::damage`, Soldat-Felder in `Hud`/`ControllerComponent`/`PlayInput`, `SoldierState`, `serialize_soldier`, Kampf-Bank im Synth, `shooting_range_level`, Studio-Sonderfälle (Ctrl+R, `soldier_pawn`), `soldierHtml` | Fußgänger = Instanz von `soldat_Soldat`; `game_controls.cpp`/`game_events.cpp` rufen statt `world_->damage/soldier_state/shots` die Ereignisse/Variablen des Pakets |
| 4 | **`ragdoll`** (aus SpawnCity), **`sitzung`** (Lobby/Räume aus `net_session`, ohne `mp_rules`; `wire` → `kern/net` serialize; `qr_code` dazu) | L6 (Skelett), L8 | – (kam aus SpawnCity) | SpawnCity nutzt die Pakete statt eigener Kopien |
| 5 | **`micro_duck`** (Walker, Gehirn, Spiel, Stimme, Netze als Assets) | L5, L6, L12, L13, L14, L16 | `walker*.cpp`, `servo`-Treiber, `micro_duck_*.inc` (25.000 Z.), `duck_voice`, 40 `walker_*`-Methoden, 12 Knoten, `MotionKey[14]`, `duck_env`, `micro_duck_level`; 44 Engine-Tests ziehen als Paket-Tests um | unberührt |
| 6 | optional: `gelaende` (Höhenfeld), `geo_kacheln` (mvt/tiles als Bibliothek), `wege` (road_graph A*), `stadt` (Orb-Run-Stadt, `CityComponent`), `jet`, `vr_rig` | L15 u. a. | `CityComponent`, `build_city`, `NightWindows`-Bezug | wie Schritt 4 |

**Risiken**
- **Leistung:** Soldat-Darstellung und Duck-Steuerung (61→512→256→128→14-Netz je Steuertakt, 100 Hz Physik) laufen
  heute als C++. Als Blueprint im Editor (Interpreter/JIT) könnte die Duck zu langsam sein; im exportierten Spiel
  kompiliert `--classes` sie zu C++. Gegenmittel: das Netz ist Kern (L13), nur die Logik ist Blueprint. Vorher messen
  (`tools/blueprint_bench.sh`-Art) – Schritt 5 erst, wenn Schritt 3 Zahlen hat.
- **Determinismus und Mehrspieler:** L1/L8 ändern den Kern des Netzspiels; der Soldat ist heute
  server-autoritativ mit Lag-Kompensation. Ohne L7/L8 ist `soldat` nur für Einzelspieler. Nicht halb ausliefern:
  Schritt 3 nur mit Mehrspieler-Test (`net_game_soldier_replication` als Paket-Test).
- **Verhaltensgleichheit SpawnCity:** Nach Nachtrag 22:00 kein Tor – aber Spieltests (Fußgänger-Treffer, Fahrphysik)
  müssen grün bleiben; Abweichungen in `$S/werte_64.md`-Art festhalten.
- **Zwei Wege während des Umzugs:** Je Schritt wird der alte Kern-Weg im **selben** Schritt entfernt (kein
  Nebeneinander von `ControlMode::soldier` und `soldat_Soldat`).
- **Lizenz/Assets:** Duck-Netze und -Meshes Apache 2.0 (Pollen) mit Namensnennung (LICENSES.md) – im Paket mitführen.
- **Größe des Umbaus L6** (Skelett/Animation/glTF) ist der längste Pfad für Soldat und Duck.

**Abhängigkeit vom Umbau der Zahl-Typen** (Blueprints rechnen in f32/i64 … wie die Quelle)
- Alle Pakete, die aus C++ kommen (Auto, Ragdoll, Soldat, Duck), rechnen in `f32`; heute würde der Blueprint in
  `Float` (64 Bit) rechnen und anders fahren/fallen als die Quelle. Darum: Bausteine erst **nach** dem Umbau
  konvertieren/schreiben; sonst bekommt jedes Paket nach dem Umbau neue Erwartungswerte in seinem Test.
- Unabhängig davon und vorher möglich: L17 (Paketform, Leser, Suche, Editor-Bibliothek), L2, L5, L10, L14, Paket
  `grund`/`spielelemente` (reine Daten-Vorlagen ohne Rechnung) – diese Arbeit kann jetzt beginnen.
- Der Konverter muss Paket-Ziele kennen (`--package <id.kpackage>` neben `--level`/`--project`, project.md 7a), damit
  die SpawnCity-Blueprints (`car_cpp` …) ins Paket geschrieben werden.

---

## 5. Offene Entscheidungen (für Besitzer/Koordinator)

1. **Name der Schicht.** „Bausteine“ (deutsch, passt zu „Kern“, beschreibt Zusammensetzen) oder „Presets“ (vertraut aus
   anderen Engines, aber im Code schon belegt: `weapon_preset`, physics.md „eigene Presets“). **Empfehlung:**
   „Bausteine“ in Doku/Editor (deutsch), Code und Dateien englisch `package` / `kern-package` / `kern/packages/`
   – oder, wenn deutsche Pfade gewünscht sind, durchgängig `bausteine`. Eine Entscheidung, dann überall gleich.
2. **Paketform.** Ein Verzeichnis mit einer `.kpackage`-Datei (wie oben) gegen ein Archiv/eine Datei. **Empfehlung:**
   Verzeichnis im Repo (lesbar, diffbar, Assets als Dateien); beim Einbetten wird daraus wie bei Projekten ein Block.
3. **Vorlage im Level: Verweis oder Kopie** (2.5). **Empfehlung:** Verweis mit `set`-Überschreibungen; Kopie nur
   ausdrücklich über „In mein Projekt kopieren“. Sonst kann ein Projekt ein Paket nie abwählen oder heben.
4. **C++-Anteile in Bausteinen.** **Empfehlung: strikt Blueprint.** Tempo kommt aus der Klasse des Blueprints
   (`--classes`) und aus allgemeinen Kern-Funktionen (Netz L13, Effekte L11, Synth-Patches L12). Braucht ein Baustein
   etwas, das Blueprint nicht schnell genug kann, ist das eine Kern-Fähigkeit für alle (mit Engine-Test), nie
   Sondercode eines Pakets. Prüfstein bleibt die Frage aus GEMEINSAM.md 22:05.
5. **Health/Leben im Kern oder als Baustein.** Allgemein brauchbar, aber mit Schaden/Respawn/Explosion spielhaft.
   **Empfehlung:** Baustein `leben` (der Kern kennt dann kein Health, `GameGoal::survive` hängt am Ereignis des Pakets
   oder entfällt); Pickup/Trigger bleiben Kern-Komponenten ohne `ammo`/`heal`/`damage`.
6. **Mehrere Blueprints je Akteur.** **Empfehlung:** nein (ein Weg), Zusammensetzen über Kind-Akteure + Bibliotheken
   + Blueprint-Schnittstellen (L9). Neu bewerten, wenn Schritt 3 zeigt, dass es nicht reicht.
7. **Lizenz der Pakete und Assets.** **Empfehlung:** Code der Pakete Apache 2.0 wie Kern; jedes Asset mit SPDX und
   Urheber in der Paket-Datei (`license.assets`), CI prüft, dass kein Asset ohne Eintrag ist; Fremd-Assets nur mit
   freier Lizenz (CC0/CC-BY/Apache). Duck: Pollen-Namensnennung aus `kern/LICENSES.md` ins Paket. Name/Logo „Kern“
   bleibt außen vor (TRADEMARKS.md).
8. **Eigene Pakete von Projekten** (ein Studio liefert eigene Bausteine an mehrere Spiele). **Empfehlung:** später;
   zuerst nur mitgelieferte. Die Form erlaubt es (Pfad statt Id), aber ein zweiter Fundort wäre eine zweite Regel.
9. **Reihenfolge Schritt 3 ohne Mehrspieler?** **Empfehlung:** nein – SpawnCity braucht den Soldaten online;
   lieber L8 vor dem Umzug des Soldaten.
