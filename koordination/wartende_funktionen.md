# Warum 208 Funktionen warten (Kandidat 8, fa64fb01; 1631/1839 geschrieben, 1588 verglichen)

Kurzantwort: Fast alles hängt am VERGLEICHSTEST (an dem, was der Test-Schreiber in die Test-Kopie nimmt) und an
Ketten dahinter, nicht an fehlenden Sprachmitteln. Eine einzige Ursache (A) trägt 137 der 208. Grundsätzlich nicht
schreibbar (Sprachmittel/Engine-Knoten fehlt) sind nach heutigem Stand nur 4 Funktionen (siehe K2) – und auch die
sind Lücken der Engine-Schnittstelle, keine der Sprache.

Zählung: `python3 $S/roots.py $S/k8.log` (jede "it calls X(), which waits"-Kette bis zur Funktion mit eigenem Grund verfolgt).

| Wurzel | direkt | als Folge | liegt an |
|---|---:|---:|---|
| A Basisklasse der Engine fehlt in der Test-Kopie | 63 | 74 | Test-Schreiber |
| B Typname `Blast` falsch aufgelöst (look_blast) | 1 | 25 | Konverter (Typen) |
| K1 Engine-Objekt als Int | 12 | 11 | Konverter (auf 2436faf4 behoben? an K9 prüfen) |
| E Exporte mit out_len im Test | 6 | 0 | Test-Schreiber |
| D Getter eines anderen Zustands (fixed_stepper, SpawnCityApp) | 3 | 2 | Testrahmen (Plan 3f) |
| T Einzelne Testfehler (gaps_boardable, townsync_update, TouchPlay::pointer) | 3 | 3 | Test-Schreiber, einzeln ansehen |
| K2 Engine hat keinen Knoten / Typ nicht markiert | 4 | 0 | Engine-Schnittstelle |
| (Kette, Ende nicht im Protokoll: Game::stop) | 0 | 1 | – |

## A – die eine große Ursache (137 Funktionen)
- Was fehlt: `sc::rules::Blueprint : kern::logic::ActorBlueprint` steht in blueprint_call.h (Zeile 33). Ruft eine
  Funktion eine Funktion ANDERER Datei (z. B. Game::hit_material → `lamps::pole_radius()` in lamps.cpp, die
  `g_rules.get<f64>("PoleRadius")` liest), nimmt der Test deren Stücke (`rules::Blueprint`, `g_rules{"Lamps"}`)
  in seine Kopie, aber NICHT die Header ihrer Datei: `kern/logic/actor_blueprint.h` ist im Test nie eingebunden.
- Fehlerkette im Test (test_Game__game_audio_web_cpp.cpp:5915): `expected class name` (ActorBlueprint) →
  `excess elements in struct initializer` (g_rules{"Lamps"}) → Stücke fallen weg → `no member named
  'pole_radius' in namespace converted_…::lamps; did you mean sc::lamps::pole_radius`.
- Die Gruppen „stars/wanted“, „controls::aim_ellipse“, „guided::cooldown“, „mp::judge_hit/create_ballot/
  cast_vote/assign_colors“, „g_fun/g_rules in *_cpp_local“, „TrafficRules::rebuild“, „seats::Table::claim“,
  „Jetpack“, „combat::Race“, „to_host“/„dispatch“ (Session::send_fx → to_host → dispatch) sind FOLGEN davon
  (alle mit „that went with Blueprint: expected class name“ bzw. „g_rules/g_sirens: excess elements“).
  Die Namensräume der anderen Dateien sind also nicht eigens kaputt – ihre Stücke fielen mit A weg.
- Behebung (allgemein): der Test bindet zu jedem Stück einer anderen Datei die Header dieser Datei ein.
  Fall tools/converter_cases/far*.cpp (alt rot: reach wartet auf „undeclared identifier 'far'“).

## B – look_blast (26)
`Game::look_blast`: „a List<LookState_Blast> where a List<Blast> is needed“. Drei Structs heißen Blast
(LookState::Blast game_look.h:37, Blast game_combat.h:62, LifeState::Blast game_life.h:34); `L.blasts` ist
`std::vector<LookState::Blast>`, irgendwo wird `Blast` außerhalb der Klasse nachgeschlagen. Daran hängt
Game::explosion (16 Rufer). Konverter (Namensauflösung, wie Fall nested) – kein Zahl-Thema.

## K1 – Engine-Objekt als Int (23)
`kern::Earcut ec;` usw. (CityStream::building_shape, Destruction::tear, slab, Game::look_view_models,
Game::vr_comfort, run_buildings, Game::Game: Mixer). 2436faf4 behebt den lokalen Fall; Rest an K9 prüfen.

## E – out_len (6)
sc_tile_requests, sc_dem_requests, sc_image_requests, sc_touch_view, sc_net_take, sc_audio_render:
„the test has no values of unsigned int *“ – TestWriter kennt den Längen-Ausgang der Exporte nicht.

## D – Getter eines anderen Zustands (5)
Game::take_fixed_step (`fixed_stepper()`), SpawnCityApp::SpawnCityApp/extensions (`look()`, `facades()`):
Testrahmen muss das Engine-Objekt eines anderen Zustands liefern (Plan 3f in stand_konverter.md).

## K2 – grundsätzlich heute nicht schreibbar (4)
- Game::look_lighting: kern::WorldSettings ist nicht KERN_BLUEPRINT_TYPE (Engine-Header markieren).
- Text::view: Funktion des Engine-Objekts Text ohne Knoten (Engine-Knoten fehlt).
- level_text: embedded_level() hat keinen Knoten.
- sc_voice_push: „a view (Span) of a part of a list“ (Sicht auf einen Teil einer Liste – Sprachmittel fehlt).

## Mit work-zahlen
Keine der Wurzeln oben braucht die Zahl-Stellen. (Der zweite Durchgang – *_source – ist deren Sache.)
