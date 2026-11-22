# Stand d (18.11., Nachfolger)

## work-d-fail = 64275dd8 (8432b275 + work-zusammen 8871ab35, Wunsch Koordinator 18:00) – Beleg läuft: work-d-fail-vergleich-180357-32687
(8432b275 war belegt mit work-d-fail-vergleich-162023-5935, ließ sich aber nicht konfliktfrei in 8871ab35 mischen.) work-d = 20bf03dc (+ 8871ab35); die wartenden Aufträge 174829/162227 nehmen den Kopf beim Start.
(Nachfolger 8, 22.11.) work-d-fail = 5500074b (engine.fail/engine.log, printf-Knoten) + origin/work-zusammen a06e5ce4 (Konflikt nur engine_case.gen.h: neu erzeugt).
vergleich src/city_stream.cpp: convert/bau/alle rc=0, Stellvertreter 10/10 ok, Engine selbst 10/10 ok, 0 ABBRUCH/LECK. GeoProjector::init geschrieben (in 045421:
„the engine's fail() … a variadic function“). Kein Rückfall gegenüber 045421: die 14 dort geschriebenen city_stream-Funktionen, die hier warten, warten nur auf
„… is defined in another file“ bzw. eine Kette davon (Teil-Probe). Engine-Suite davor: work-d-fail-engine-170043-6435 (5500074b) rc=0.

## Nachfolger 8 (22.11., ab 16:10)
- **Aufgabe 3 (Befund g: Session_push, Länge eines Zeiger-Eingangs, 206/300 + ASan real): ERLEDIGT, kein neuer Commit.** Behoben durch work-d-count f384bdfd
  (Count<Per> im Typ, in work-zusammen). Beleg: voller Lauf work-zusammen-voll-045421-10862: Session_push „285 calls drawn, 285 compared“ in beiden Durchgängen,
  converted_net_session und converted_net_session_cpp ok (Stellvertreter und Engine selbst), kein ABBRUCH. gs Befund (232903) war vor diesem Fix.
- **Aufgabe 1:** work-d = c4cd0b69 (work-zusammen a06e5ce4 eingemischt; Konflikte converted.h Watch + cpp_to_blueprint.py OWN_LOGIC/Felder; die Wartestelle
  „it gives a place of the engine“ bleibt entfernt; Generator unverändert) + **3976cdfc** (Fix): die Engine-Suite work-d-engine-185841-1710 (e9ed65ca) war **rot**
  (blueprint_native_test, Fall „Engine“ auf der Engine selbst): beim Binden eines Felds (engine.mesh.id …) sagte die Klasse „no object“ unter dem Knotennamen, die
  Tür nichts (Binden eines Felds ist kein Aufruf). Jetzt bindet die Klasse ein Feld ohne Nachschlagen; ein fehlendes Objekt sagt sie beim ersten Zugriff als
  <Knoten>@get wie die Tür. Belege laufen: vergleich `work-d-vergleich-161943-5651` (c4cd0b69; alle src/game*.cpp + people.cpp + city_jobs.cpp),
  Engine-Suite `work-d-engine-162227-8504` (3976cdfc).
- **Aufgabe 4 (übrige Wartegründe „Platz der Engine“ in 045421):** die 8 Funktionen mit „the node that binds it is not written yet“ (update_sound, update_fighters_back,
  foot_rounds_update, gaps_lamps_update, look_lighting, step_soldier, step_cop, ui_mix_cues) und „a reference to something it changes“ an Plätzen (handle_shots
  `auto& shots = world_->shots()`, run_buildings `auto& chunks = b.chunks()`, CityStream::work `auto& chunks = t.road_builder.chunks()`) sind auf work-d gebaut
  (Beleg: vergleich 161943 für game*/people/city_jobs; CityStream::work dort nicht dabei). Game::ctl_view_json („a reference …“) ist KEIN Platz der Engine
  (`ViewTrack& V = g_view`, eine Datei-Variable) – nicht d. Übrig war Game::ui_mix_audio („an assignment to CXXMemberCallExpr“: `m.bus(Bus::sfx).volume = …`).
  **ENTSCHIEDEN (d, nach den Zielen):** ein Eintrag eines Arrays fester Länge des Objekts, `return feld_[p];` mit p dem einen Parameter, ist ein Platz der Engine
  (am Baum geprüft, wie `return feld_;`); ein Index außerhalb ist, was C++ offen lässt: der Aufruf scheitert in jeder Stufe gleich („the input b names no entry:
  there are 6“). Gebaut: **work-d-index d03ccf8c** (auf work-d 3976cdfc): Api.looks_up/„index“, Tür (@get/@set mit Eingängen, Index geprüft – auch im Knoten
  selbst, wo C++ heute ins Leere läse), Klasse (Place hält P::In in C++-Typen, P::at prüft), Engine-Test engine_place_at_an_entry_of_an_array, Doku.
  Belege laufen: vergleich `work-d-index-vergleich-172026-30784` (game.cpp, game_ui.cpp), Engine-Suite `work-d-index-engine-172028-30823`.
- Nicht abbrechen durfte ich (Rechte verweigert): die veralteten eigenen Aufträge work-d-belege2-vergleich-055022-6799, work-d-probe-173035-30178 (warten auf mag).

- **19:00 (Nachfolger 7):** work-d = **e9ed65ca** (+ work-zusammen 789f8463 Rückrufe c: 2ff9ce33, Konflikt nur Doku/engine_case.gen.h; + e9ed65ca).
  (2) **run_buildings behoben:** der Stellvertreter eines Platzes gab den Typ ohne `&` zurück (spell) – `auto& chunks = b.chunks()` band ein Temporäres. Lokal
  (Konverter über city_jobs.cpp, TEST_ERRORS): run_buildings geschrieben, übersetzt. (3) **Feld-Plätze gebaut** (Entwurf oben): Api.fields → 46 Knoten
  engine.<klasse>.<feld> (EngineFunction::field: Binden ohne Aufruf), Konverter Emit.engine_field (lesen/setzen/als Platz), Stellvertreter-Felder +
  converted::FieldSync, Doku „Felder eines Objekts der Engine“. Unbelegt; Engine-Suite `work-d-engine-185841-1710` abgelegt; danach vergleich über
  game*.cpp + people.cpp + city_jobs.cpp (look_view_models, look_command, vr_comfort, run_buildings).
  BEFUND FÜR t1: keine – die Probe 173035 kam nicht mehr zum Zug (abgebrochen); der fremde Rückfall ist laut Koordinator nicht von d.

## Nachfolger 7 (ab 15:00, 19.11.; nach Server-Neustart 15:40 weiter)
- work-d = **e2f1dc6f** = 275d3ddf + work-zusammen 0f7f5a46 (0651fb31; Konflikt Emit.engine: asks_engine behalten, Wartestelle bleibt weg) + work-zusammen 4ac2e4f9
  (7e836a51; blueprint.h Sought<T>/Listen jeder Breite (p) + Platz-Fassungen/read_of (d); is_local/writable bound + sees_state) + Fix e2f1dc6f.
- `work-d-vergleich-150328-5241` (0651fb31, alle game*.cpp): convert rc=0, die Platz-Funktionen jetzt GESCHRIEBEN (look_lighting/shots, sound_cues …), aber
  **bau rc=123**: 3 Fehler in level_blueprints.cpp – ein Feld eines Platzes (n.result[i].volume) als Argument von kern::formula (Field nicht kopierbar) und in
  kern::to<Float>(n.result[i].actor). Fix e2f1dc6f (Field kopierbar, kern::to liest einen Platz zuerst: PlaceOps::kern_place_value). handle_shots wartet dort nur auf
  frighten() (people.cpp). Neu: `work-d-vergleich-154229-25045` (e2f1dc6f, game*.cpp + people.cpp). `work-d-konvert-125538-27289` (275d3ddf) rechnet noch.
- **Warteliste, gebaut: work-d-fail = 5500074b** (auf work-zusammen 0f7f5a46, unbelegt): Funktion mit printf-Format (`__attribute__((format(printf…)))`, Format = letzter
  benannter const char*) ist ein Knoten mit Eingang `text` (Schritt); Tür/Klasse: `fail("%s", text)`; Konverter: `text_format<fmt>(-1, werte)`; Stellvertreter der Quelle:
  vsnprintf (converted::formatted). Neue Knoten engine.fail, engine.log. Gegen GeoProjector::init (CityStream::start, rebase_origin, run_tile, ui_move_world).
- **Entwurf (nicht gebaut): Felder eines Engine-Objekts** (look_view_models/look_command `m->visible`, `m->material.viewmodel`; „a member of Int is set“ TextureDesc
  td.width = …): ein öffentliches Feld eines Objekts mit Handle ist ein Platz der Engine wie `T& f() { return f_; }` (Knoten engine.<klasse>.<feld>, @get/@set).
  Offen ist die Quellseite im Test: `m->visible` lässt sich in C++ nicht abfangen → Stellvertreter-Felder als echte Glieder, abgeglichen wie converted::engine_place
  (vor jedem Engine-Aufruf setzen, danach lesen); das Binden eines Felds ist kein Aufruf.

- **18:10 Stand (Nachfolger 7):** work-d = **739ca173** (+ work-zusammen fb51251f Rekursion, ohne Konflikt, Generator unverändert).
  Belege: `work-d-vergleich-154229-25045` (e2f1dc6f; alle game*.cpp + people.cpp): convert/bau rc=0, **99/99 ok in beiden Durchgängen, 0 LECK**; Game::ui_mix_cues
  (sound_cues()-Platz, Einträge am Platz geändert) geschrieben, 20/20 Aufrufe verglichen. Engine-Suite `work-d-engine-163715-25680` (739ca173) rc=0, 55620 checks, 0 failures.
  **NICHT BEREIT – Rückfall im ganzen Spiel:** `work-d-konvert-125538-27289` (275d3ddf): convert rc=0, aber **1633 statt 1725 geschrieben, 370× „was written and waits now“**
  (Läufe 070328/100601/123021: 0) – „its source does not compile with a Blueprint's numbers yet: use of undeclared identifier 'Game' … that went with Car / Server“
  (86 Funktionen) und run_buildings „non-const lvalue reference to type 'vector<...>' cannot bind to a temporary“ (auto& chunks = b.chunks() – ein Platz). Verdacht:
  der Stellvertreter eines Platzes (converted::engine_place) passt nicht zur Quelle mit den Zahlen des Blueprints. Probe mit TEST_ERRORS:
  `work-d-probe-173035-30178` (src/city_jobs.cpp) wartet auf mag. Gegenprobe ohne d: `work-zusammen-voll-143319-13859` (0f7f5a46) läuft noch.
  Shots()/handle_shots/look_lighting im ganzen Spiel erst nach diesem Fix belegbar.
- **work-d-fail 5500074b:** `work-d-fail-vergleich-163726-27574` (src/city_stream.cpp): 10/10 beide Durchgänge, GeoProjector::init geschrieben (converted_engine_geo_roads ok);
  Engine-Suite `work-d-fail-engine-170043-6435` wartet auf mag.

## Nachfolger 6 (ab 07:45, 19.11.)
- **Aufgabe 1 Uhren gegen die Engine – Fix gebaut:** Zweig **work-d-uhr = 4b7074b1** (ein Commit auf work-zusammen 821208ac; nur tests/rule_blueprint.h + converted.h).
  sc::Clocks: die Antworten einer Uhr (EngineFunction::clock, aus KERN_CLOCK) an die Quelle je Stelle ihrer Aufzeichnung; ein Blueprint-Aufruf an derselben
  Stelle mit demselben Aufruftext bekommt sie, sonst fragt er die Engine (Reihenfolge wird wie immer verglichen). Watch beginnt neu. Stellvertreter unverändert.
  Beleg läuft: `work-d-uhr-vergleich-075109-9150` (city_stream, terrain, mvt, game_look).
- **Beleg 08:25:** `work-d-uhr-vergleich-075109-9150` (4b7074b1; city_stream, terrain, mvt, game_look): convert/bau rc=0; **Stellvertreter 46/47 ok, Engine selbst
  46/47 ok, 0 LECK, 0 ABBRUCH**. clock_ms (vorher real 300/300), CityStream_select_cells (300/300), CityStream_take_requests (88/287, 92/300) weichen gegen die Engine
  nicht mehr ab (converted_game_look_cpp, converted_city_stream(_cpp) ok). Einziges FAIL in BEIDEN Durchgängen: converted_terrain_cpp Terrain_run_job (13/300 bzw. 48/300) –
  jede gezeigte Abweichung ist „call 1 … engine.jobs.submit(3, [Bytes])“: die Quelle legt andere Bytes in den Auftrag als der Blueprint (Anordnung/Breite der Zahlen) =
  die Regel von Paket k, keine Uhr. (Die 2 LECK in 070253 waren vor den gemergten Fixen.)

- **Aufgabe 3 (Platz der Engine, Schritt 7):** work-d = e757c565 = 9949fd00 + origin/work-zusammen 821208ac (7297dc01; Konflikt EngineObjects::end: leck-Fassung + ++generation_)
  + work-d-uhr. Die Wartestelle aus 1007cb7e („the node that binds it is not written yet“) ist entfernt – Emit.engine() bindet den Platz (e02ea240). Befund: vor 1007cb7e
  las engine_table() Einträge mit place gar nicht (alle Platz-Knoten „its pins differ“), Schritt 7 lief also nie an einer Quelle. Generator reproduziert.
  Beleg läuft: `work-d-vergleich-082602-26333` (game.cpp + game_events, game_lamps, game_look, game_foot_rounds, game_military, game_town, game_ui). Danach konvert 9, Engine-Suite.

- **Aufgabe 2 (Einzel-Lauf `ergebnis-work-zusammen-voll-070328-32111-alle-080335-20196`, 821208ac ohne Uhren-Fix), 09:35:** Stellvertreter 115 ok / 3 FAIL / 34 ZEIT,
  Engine selbst 115 ok / 4 FAIL / 33 ZEIT, 0 LECK, 0 ABBRUCH in beiden; kein „refused“, kein „only N calls could be compared“, kein „this build has no“.
  Nur gegen die Engine rot: converted_city_stream_cpp (select_cells, step_time 300/300 und 237/237, take_requests, update_collision 55/112 – erste Abweichung jeweils
  an einer Zeit: .vis_ms_, .ms_.t_collision, .near_.at_ms) und converted_game_look_cpp (clock_ms) – alles Uhren, im Lauf work-d-uhr-vergleich-075109-9150 dieselben
  Funktionen (auch step_time 267 Aufrufe, update_collision 159) ohne Abweichung → mit work-d-uhr (in cf71d466) erledigt. Rest ist in BEIDEN Durchgängen rot, nicht
  von d: Terrain_run_job (k, Bytes von jobs.submit), number_at (8 bzw. 17/300, t1), converted_game_details_cpp, die ZEIT der Game-Tests (t1). Nur gegen den Stellvertreter:
  Session_update (k), ZEIT converted_Game__mp_rules_cpp. **Für d bleibt aus dieser Liste nichts.**

- **Aufgabe 3, Beleg 10:05:** `work-d-vergleich-082602-26333` (work-d 0c4c551f; game.cpp + game_events, game_lamps, game_look, game_foot_rounds, game_military,
  game_town, game_ui): convert/bau rc=0, **58/58 ok in beiden Durchgängen, 0 LECK** – kein Rückschritt. Die 9 Platz-Funktionen werden in der Teil-Probe aber nicht
  geschrieben („X() is defined in another file“) – Schritt 7 belegt erst ein Lauf über alle Game-Teile (konvert 9 / vergleich über alle game*.cpp). Offen.
- **Befund t2 (optional, 10:05) – Fix gebaut:** **work-d-opt = 2226dbe9** (auf work-zusammen 18526372): Api.shape schreibt aus dem C++-Typ `[?item]` für std::optional /
  std::unique_ptr eines Werts (Feld, Ergebnis, Parameter, Eintrag einer std::vector); answer gibt dort 0 oder 1 Eintrag. Generator: 29 Einträge (Level-Akteure,
  Client::receive). Engine-Test engine_stand_in_one_or_none, Doku.
- **Befund k (j.id, 10:05) – Fix gebaut:** **work-d-jobs = a12da91a**: Ursache – die Seiten teilen den Zustand der Aufträge der Engine (Nummern zählten weiter: Quelle 1,
  Klasse 2, Interpreter 3). fresh_engine setzt jetzt auch jobs zurück (set_threads(false) + neues kern::jobs::reset()); Knoten der Engine, Generator neu,
  Engine-Test jobs_reset_begins_again. CityStream_step_time/update_collision = Uhren → von work-d-uhr gedeckt (Lauf 075109: ohne Abweichung).
- Beleg-Zweig **work-d-belege3 = 20eeaaf0** (18526372 + opt + jobs, Generator reproduziert). Läuft: vergleich (terrain, city_stream, game.cpp); danach Engine-Suite.

- **Beleg 11:05:** `work-d-belege3-vergleich-101159-615` (20eeaaf0; terrain, city_stream, game.cpp): convert/bau rc=0, **48/48 ok in beiden Durchgängen, keine
  Abweichung, 0 LECK** – Terrain_run_job (geschrieben, 300 Aufrufe) gegen die Engine gleich (vorher 35/237 j.id). Game::audio_init dort nicht geschrieben (Teil fehlt) →
  `work-d-belege3-vergleich-110539-22562` mit den Quellen von t2s Beleg (game, game_equipment, sound_fx, game_details, equipment, combat_audio). Danach Engine-Suite.

- **Beleg 12:10:** `work-d-belege3-vergleich-110539-22562` (game, game_equipment, sound_fx, game_details, equipment, combat_audio): Game::audio_init/ui_audio_init geschrieben;
  audio_init gegen den Stellvertreter 1/20 (vorher 8/20), gegen die Engine 0. Die eine übrige ist anderer Art: „call 122 – the source asks
  engine.sound_bank.request(0, "sc:shot0"), the Blueprint nothing more (191 and 121 calls)“ – der Blueprint endet vor combat_audio_init (BEFUND FÜR t2/k, nicht die Form).
  Sonst: number_at 14/300 (t1), converted_game_details_cpp real (bekannt). Engine-Suite läuft: `work-d-belege3-engine-120959-12020`.
- work-d = **fa753bcb** (work-zusammen 6998a56e eingemischt: Konflikt logic.h/.cpp – bound_now und lengths_on (e) beide behalten; + belege3; Generator neu). Syntax ✓.

- **15:00 Stand Platz:** `work-d-konvert-125538-27289` (work-d 275d3ddf, 9 Kerne) rechnet noch. Nächste Schritte: uebergabe/d.md „15:00“.

## BEREIT ZUM MERGEN (d, 14:57): work-d-make 49e5c319 (Macher geben nie Handle 0, Form `h`) – auf work-zusammen dc4229b5
- Belege: `work-d-make-vergleich-131505-2655` (game, game_equipment, sound_fx, game_details, equipment, combat_audio): **52/52 ok in beiden Durchgängen, keine
  Abweichung** (audio_init vorher 1/20); Engine-Suite `work-d-make-engine-141127-4668` rc=0 (55404 checks, 0 failures; engine_stand_in_makes_an_object ok).
- Beim Mischen Generator neu (engine_api.gen.cpp).

## BEREIT ZUM MERGEN (d, 12:56): work-d-belege3 3c10e298 = work-d-opt cbed8b27 (2226dbe9 + Testfix) + work-d-jobs a12da91a auf work-zusammen 18526372
- Belege: `work-d-belege3-vergleich-101159-615` (terrain, city_stream, game: 48/48 beide Durchgänge, Terrain_run_job gegen die Engine gleich – vorher j.id),
  `work-d-belege3-vergleich-110539-22562` (audio_init: Engine 0, Stellvertreter 1/20 statt 8/20 – Rest anderer Art, s. o.), Engine-Suite
  `work-d-belege3-engine-121830-13658` rc=0 (54673 checks, 0 failures; jobs_reset_begins_again, engine_stand_in_one_or_none ok).
- Beim Mischen: Generator neu (engine_api.gen.cpp, engine_case.gen.h). Zwei Merge-Commits (3c10e298, 275d3ddf) haben den Standardtext ohne Attribution.

## BEREIT ZUM MERGEN (d, 08:30): work-d-uhr 4b7074b1 – Beleg work-d-uhr-vergleich-075109-9150
- Ein Commit auf work-zusammen 821208ac, nur Testrahmen (rule_blueprint.h sc::Clocks + test_engine; converted.h Watch eine Zeile). Keine Engine-Änderung → keine Engine-Suite nötig.
- In work-d eingemischt (e757c565; Konflikte mit dem Platz der Engine: @get/@set-Zeile + g_places.clear() behalten).
- BEFUND FÜR k: Terrain_run_job – Bytes von engine.jobs.submit weichen in beiden Durchgängen ab (siehe oben; real 48/300, Stellvertreter 13/300).
- Hinweis (nicht abbrechbar für mich): `work-d-belege2-vergleich-055022-6799` (Vorgänger, überholt durch 055045) wartet noch auf mag – bitte herausnehmen.
- fertig+bewiesen+gemergt: Stufe 4 (d) (CI 44337346531)

## A – rote Engine-Suite (R129): Fix gebaut, Beweis läuft
- Fix-Commit **6f62201** (Eltern: origin/work 90b5ab8 – per cherry-pick/merge ohne Konflikt übernehmbar; enthält NUR den Fix),
  in work-d gemergt als 5367e14 (Konflikte nur mit edae85f in engine_api.h/engine_api.py/cpp_to_blueprint.py, Generator neu).
- Ursache: Jobs liefen nativ auf Worker-Threads – ob der Job des Falls noch in der Schlange lag, wenn take_queued kam, hing vom Ablauf ab
  (Klasse schnell → noch da, 16 Einträge; Tür langsam → Worker hatte ihn). take_queued/done nativ zudem ohne Sperre (Datenrennen).
  R122_0 (Job-Nummer), R126 (pending), R118 (last_error) waren nur „nicht wiederholbar“, weil die drei Spiele in einem Prozess den
  Zustand der Engine weitertrugen. Echt nicht wiederholbar: R120 now_ms, R121 wall_ms (Uhren).
- Lösung: jobs::set_threads(false) (Jobs in pump auf dem Frame-Thread, wie Web ohne Worker; allgemein, zwei neue Knoten), Sperren in
  take_queued/done; jedes Spiel per fork vom selben Zustand der Engine; Uhren aus der Tabelle (KERN_CLOCK → EngineFunction::clock),
  Variablen über die Drähte bestimmt; jede andere Abweichung (auch Tür/Tür) ist jetzt ein FAIL.
- Beweis läuft: mag work-d-engine-174905-11207 und work-d-engine-174911-11261 (Stand 5367e14), CI 44351625418.

## C – Antworten
- `Mixer::bus(Bus)` → `BusSettings&`: trägt der Entwurf NICHT (Platz nur ohne weitere Eingänge, Rumpf genau `return member_;`).
  Allgemein bräuchte es: Plätze mit Eingängen als Wert – gebunden mit den Werten der Eingänge zum Zeitpunkt des Bindens (wie C++ das
  Argument einmal auswertet), @get/@set bekommen sie mit (in = [Handle, Eingänge…]); Erkennung am Baum: Rumpf `return m_[p];`
  (ArraySubscript auf ein Feld fester Länge, Index nur aus einem Parameter, keine Aufrufe). Achtung: `Bus` hat den Wert `count`
  → `buses_[count]` liegt neben dem Feld (UB in C++); dafür gälte die Regel „Platz neben einer Liste“ (eigener Default-Eintrag, im Test
  markiert) oder die Stelle wartet – keine stille Annahme.
- `TextureDesc::rgba` (`const u8*` in einer Struktur der Engine, Länge width*height*4 nur im Kommentar): trägt der Entwurf NICHT (er
  betrifft Funktionen, die T& geben, keine Zeiger-Felder). Allgemein bräuchte es eine Länge, die der Typ sagt: z. B. das Feld als
  `Span<const u8>` (Zeiger + Länge) ↔ Liste in der Struktur des Blueprints (Conv/Direct: Zeiger auf die Daten der Liste, ohne Kopie,
  wo der Elementtyp gleich ist – bei List<Int> (64 Bit) gegen u8 ist eine Kopie unvermeidlich, solange die Sprache keine Byte-Liste hat).
  Aus einem Kommentar die Länge abzuleiten wäre eine Sonderregel.
- engine_api.py: Änderungen für „Platz der Engine“ noch nicht fertig (Vermerk folgt hier, sobald gepusht – Paket c wartet).

## B – Platz der Engine (in Arbeit, Nachfolger 2)
- A erledigt: origin/work-zusammen (8874e79) eingemischt – Fast-Forward (work-zusammen enthielt work-d schon), gepusht.
- Schritt 3 (Interpreter) fertig, Syntax ✓, gepusht: 6310a9e (Root::bound, Spot::bound, Context::bind, read/write über @get/@set,
  EngineStandIn::places). In Arbeit: Schritt 4 (Klasse).
- Übergabe: $S/uebergabe/d.md

## BEFUND earcut (Koordinator 21:2x) – Fix gebaut, Probe läuft
- Commit **690ea4f** (work-d): Zeiger-Eingänge zählen Count<Per>: earcut.h `Count<2> vertex_count` (triangulate, deviation),
  ev::numbers() in Tür/Klasse (engine_api.py, .gen neu), Test-Schreiber-Stellvertreter (cpp_to_blueprint.py, Zweig der
  Zeiger-Eingänge ~Z. 10956: `per * count` Werte). Allgemein, keine Namensliste. Für t1: kleine Änderung in TestWriter (eine Zeile `per`).
- Probe: mag `work-d-probe-212615-8375` auf 690ea4f: convert rc=0; CityStream::building_shape, Destruction::tear/collapse/fracture
  „written“, Test übersetzt. Bit für Bit zeigt erst der nächste `voll` (Spiel-Suite).

## BEREIT ZUM MERGEN (earcut / Count<Per> der Zeiger-Eingänge) – nur Commit 690ea4f per cherry-pick (s. o.), Beleg: Probe work-d-probe-212615-8375
- Achtung: work-d trägt zugleich den unbewiesenen Zwischenstand „Platz der Engine“ (6310a9e, 64cb433). Nur den Fix
  übernehmen: `git cherry-pick 690ea4f` (setzt nur auf earcut/engine_host/engine_api.py/cpp_to_blueprint.py auf; danach
  `python3 tools/engine_api.py`, da engine_nodes.gen.h auch die Place-Teile hat).

## engine_api.py (für Paket c)
- Änderungen für „Platz der Engine“ (P_<Knoten>, Fields je Struktur, <Knoten>_place, Binden) und Count<Per> der
  Zeiger-Eingänge sind gepusht (64cb433, 690ea4f). Weitere Änderungen an engine_api.py durch d: nur noch Fall-Text (case_text).

## Platz der Engine – Stand 21:40 (Nachfolger 2, Lauf endet: Kontingent)
- Schritt 3 ✓ (6310a9e), Schritt 4 ~ (64cb433, nur Header-Syntax, nie instanziiert), Schritt 5 ~ (Schreibweise 64cb433,
  check() 2250736; Palette offen), Schritt 6 offen (Engine-Test, nativer Fall in case_text, Fall mit jedem Knoten, Doku), C/D offen.
- Einzelheiten und nächste Schritte: $S/uebergabe/d.md, Abschnitt B „Stand der 7 Schritte“.

## Platz der Engine – Nachfolger 3 (ab 22:20)
- work-zusammen (8844531a) eingemischt: d6869e1. Commit **c0d6d24**: Klasse instanziiert (nativer Fall EnginePlaces, Engine-Test
  engine_places_class_names_the_engines_list gegen C++ BuildingBuilder::chunks, Fall mit jedem Knoten mit `bound`, case_text mit Plätzen);
  Leser/Schreiber-Fehler behoben (Member `_place` wurde abgelehnt bzw. nicht geschrieben). Lokal (Harness $S/d/): Interpreter = Klasse auf
  Stellvertreter und auf der Engine (geo), Fixpunkt, g++/clang++ -Werror. libc++ (wasi-sdk fehlt hier) prüft nur CI „build“.
- Beweis läuft: mag `work-d-engine-223942-31317`, CI 44403776223 (Stand c0d6d24).
- engine_api.py (für Paket c): case_text schreibt Plätze als `_place`-Member mit Lese/Setz-/Listenschritten und einem Lauf auf gemachten
  Objekten (Klasse mit Macher ohne Eingänge + Freigeber). Sonst unverändert seit 690ea4f.

## ENTSCHEIDUNG NÖTIG (d, 22:55): Wachstum von `list_append` auf einem Platz der Engine
- Befund: Werte sieht der Vergleich nie anders (dieselben Einträge). Die `cost:`-Zeile zählt Anforderungen: heute reserviert
  `engine_values::list_append` (engine_direct.h) genau `size+n` und hängt einzeln an. Die erste Anforderung stimmt mit C++
  `v.insert(v.end(), a, b)` überein (beide 1, wenn die Kapazität nicht reicht), die Kapazität danach aber nicht: Beispiel size 4,
  cap 4, +1 → hier cap 5, libstdc++ cap 8 (size+max(size,n)), libc++ max(2·cap, size+n). Ein folgendes push_back fordert hier
  Speicher an, in C++ nicht → `cost:` weicht um 1 ab. Blueprint-Listen (logic::list::insert_items) nutzen schon vector::insert und
  wachsen wie C++.
- Vorschlag: auf einem Platz `list.insert(list.end(), n, blank<C>())` (Füll-Einfügen: dieselbe Wachstumsregel der Bibliothek wie
  das Bereichs-Einfügen, in libstdc++ und libc++), danach jeden neuen Eintrag mit Direct::from setzen – eine Anforderung wie C++,
  dieselbe Kapazität danach, innere Listen je Eintrag wie beim Kopieren. Konform (kein Proxy-Iterator). Nicht umgesetzt – wartet auf Antwort.
- E7 (Antwort Koordinator: JA) umgesetzt: **cf8786c** – list_append am Platz = insert(end, n, blank) + Direct::from; andere
  Listenschritte am Platz rufen schon dieselben logic::list-Funktionen (add/insert/remove_at/resize) – keine weitere Angleichung nötig.
  Nativer Fall EnginePlaces hat Lauf „Append“ (zweiter Platz auf dieselbe Liste).
- Weitere Commits: 42c86fe Palette („Places of the Engine“, Editor-Test), 62d0978 Doku. mag `work-d-engine-223942-31317` wartet noch
  (rechnet den Stand bei Start, also ≥ cf8786c); CI 44403776223 auf c0d6d24 läuft.
- Offen: Schritt 7 (Stellvertreter im Quelltext-Test, Konverter-Regel).

## Befund k (Array-Parameter fester Länge) – Fix gebaut, Beleg läuft
- **50832229** (Eltern: work-zusammen e9eec02, nur dieser Fix; in work-d gemergt als 54ae4f7): `T name[N]` liest N Werte –
  c2b.declared_length (aus der Quelle der ParmVarDecl), TestWriter zeichnet N Werte auf, Tür/Klasse weisen eine kürzere Liste ab.
  Generator neu (physics/game engine_calls.gen.cpp, engine_nodes.gen.h). Beleg: mag `work-d-vergleich-225255-10477` (src/drone.cpp →
  converted_rigid_spec), läuft. engine_api.py (für c): Prüfung der festen Länge bei Zeiger-Eingängen.
- Schritt 7 (Konverter + Stellvertreter der Quelle) gebaut: e02ea24, unbewiesen (Probe/vergleich über game_events.cpp o. ä. folgt).

## BEREIT ZUM MERGEN: 50832229 (Array-Parameter fester Länge, Befund k) – Beleg work-d-vergleich-225255-10477
- Eltern: work-zusammen e9eec02, enthält nur den Fix (cherry-pick oder merge sauber). mag `vergleich origin/work-d src/drone.cpp` auf ddb54b72:
  convert rc=0, bau rc=0; **converted_rigid_spec ok in beiden Durchgängen** (Stellvertreter und SC_ENGINE=real; vorher 300 of 300 calls differ).
  Übrig: converted_drone_cpp „Drone_step: 1 of 5 calls differ“ in beiden – das ist G7 von k (u16-Rückwandlung, 2a6ef5b3, nur auf work-k), nicht dieser Fix.

## STAND 23:40 (Nachfolger 3 endet – Kontingent): Befund-Block Zeiger/Array-Längen OFFEN, an Nachfolger 4
- Auftrag Koordinator (23:3x): VOR weiterem Platz-Beweis als ein Block, je Fix eigener Commit auf work-zusammen (5a445af8+), Beleg `vergleich`:
  (1) set_pcm (t2: gaps_guided_sounds 12/38) – `set_pcm(id, const f32* samples, u32 frames, u32 channels, u32 rate)` liest frames·channels;
      Stellvertreter der Quelle zeichnet 1 Zahl auf (Name „frames“ trifft die Namensregel nicht). Beleg: vergleich src/game_gaps.cpp.
  (2) matrix_of 88/88 (g): Form `{[n]}` für Mat4 (`f32 m[16]`) – ein Feld/Ergebnis `T m[N]` hat genau N Werte (Antwort des Stellvertreters,
      Beschreibung/Form). Beleg: vergleich über eine Datei, die actor_matrix ruft.
  (3) net_session ABBRUCH (g): `HostTransport::push_incoming(from, const u8* data, int bytes)` – Zahl neben Zeiger per TYP sagen
      (kern::Count<Per> in den Erklärungen der Engine), Namensregel `count|size|len|num|^n$` (engine_api.py Calls.body/direct ~Z. 457/593,
      TestWriter.stand_in Zweig Zeiger-Eingänge) abschaffen; alle Erklärungen mit Zeiger + Zahl umstellen; was danach ohne Count neben
      einem Zeiger steht, meldet der Erzeuger. Beleg: vergleich src/net_session.cpp.
  Hinweis zu (1): frames·channels ist kein festes Per – Count<Per> trägt es nicht. Möglichkeit: Zahl der Samples als Count<1> (frames = n/channels
  in sound.cpp; alle Spiel-Aufrufer geben channels=1, studio.cpp:82 gibt frames·channels) – das ändert die Bedeutung eines Engine-Parameters:
  mit dem Koordinator klären, falls nicht offensichtlich.
- Vorbild: 690ea4f (Count<Per> earcut), 50832229 (declared_length, Array-Parameter) – beides gemergt.

## Platz der Engine – Belege (23:58)
- **Engine-Suite mag `work-d-engine-223942-31317` auf ddb54b72: rc=0** – u. a. ok logic_notation_every_node_type, engine_calls_every_node,
  engine_places_class_names_the_engines_list, studio_logic_places_of_the_engine, blueprint_native_test (55 Blueprints, 14107 Aufrufe, mit
  EnginePlaces und Engine-Fall mit Plätzen), game_native_test.
- CI 44403776223 (c0d6d24): native ✓, build (libc++/wasm) ✓, determinism ✓, input ✓, **e2e ✗**: „Game: its Blueprint compiles in the editor …
  Sound Cues: its output "result" is a place of the engine - read it with a node that names the place, not through a wire“ (+ Folgefehler
  „Play runs them as machine code“). Ursache: das Level auf dem Zweig ist noch von einem alten Lauf – dort liest ein Draht den Ausgang von
  engine.game_world.sound_cues (früher eine Kopie). Der Konverter schreibt jetzt `tmp = nX.result` (Get mit bound). Erst ein neuer voller
  Lauf, der das Level neu schreibt, macht e2e grün. Deshalb ist „Platz der Engine“ NOCH NICHT „bereit zum Mergen“ (CI nicht grün): der
  Koordinator entscheidet, ob es mit dem nächsten `voll` gemergt wird. CI auf dem letzten Stand: 44414094027 (läuft).
- Schritt 7 (e02ea24): vergleich `work-d-vergleich-231118-18156` (game_weapons.cpp + game.cpp, ddb54b72): convert rc=0, bau rc=0; in keinem
  geschriebenen Text steht ein Platz – die Funktionen mit shots() (drone_attack, jet_attack) warten aus anderen Gründen (aim_ray, ctl_screen_ray).
  Schritt 7 ist also gebaut und bricht nichts, aber noch nicht an einer Quelle belegt. FAIL dort: eject, state_of (auch in früheren Läufen),
  matrix_of + net_session (Befund-Block oben) – nicht von Schritt 7.

## Nachfolger 4 (ab 00:05, 19.11.) – Befund-Block Zeiger/Array-Längen
- Zweig **work-d-count** (auf work-zusammen e4aa2e8d), zwei Commits (E14 und Count hängen zusammen: ohne Count im Typ prüft niemand
  `samples`, und mit der Typ-Regel meldet der Erzeuger set_pcm/create_clip, solange dort `u32 frames` steht):
  - **0c6840cc** E14: `set_pcm(id, const f32* data, Count<1> samples, channels, rate)`, ebenso `Mixer::create_clip`; frames = samples/channels,
    Rest ≠ 0 abgewiesen; Studio::sound_pcm gibt frames·channels; Spiel unverändert (channels = 1). Engine-Test + Doku.
  - **f384bdfd** Count im Typ: Namensregel weg (engine_api.py ×2, TestWriter), count_per nur Count<Per>; Api.unsaid_count meldet jede ganze Zahl
    ohne Count neben einem Daten-Zeiger, engine_api.py hält an. 30 Erklärungen umgestellt (net/geo/scene/gfx/audio/game). Generator reproduziert.
  - Syntax lokal: g++ -Werror über 41 betroffene Dateien + test_main.cpp ✓; py_compile ✓.
- Beleg läuft: mag `work-d-count-vergleich-000943-8780` (net_session.cpp + game_gaps.cpp); Engine-Suite (Filter) folgt.
- **Beleg Punkt 1 (00:27):** `work-d-count-vergleich-000943-8780` (f384bdfd; net_session.cpp + game_gaps.cpp): convert rc=0, bau rc=0;
  Stellvertreter 45/45 ok, real 44 ok / 1 FAIL. **Session_push weicht in keinem Durchgang mehr ab, kein ABBRUCH converted_net_session (real ok).**
  Einziges FAIL (real, converted_net_session_cpp): Session_connected/join/stop „only 0 calls could be compared“ – wie vorher (g: Engine weist ab), nicht neu.
  Punkt 3 hier NICHT belegt: ohne game.cpp warten die Game-Funktionen („the struct Game has more than 96 fields“) – gaps_guided_sounds nicht verglichen.
- Punkt 2 gebaut: Zweig **work-d-array** = **83733dad** (auf work-zusammen e4aa2e8d): Api.shape schreibt `[N item]` für Felder fester Länge
  (Mat4 `{[16n]}`), answer gibt genau N; Engine-Test engine_stand_in_fixed_length. Syntax ✓, Generator reproduziert.
- Beleg-Zweig **work-d-belege** = work-zusammen 310c6e10 + work-d-count + work-d-array (Generator reproduziert). Läuft:
  `work-d-belege-vergleich-002858-13659` (game.cpp + game_gaps.cpp: matrix_of, gaps_guided_sounds). Danach Engine-Suite auf work-d-belege.
- **BEFUND (real-Durchgang, für t1/Koordinator):** actor_matrix (und jede Funktion auf GameWorld: eject, hide_soldier, step_parachute …) wird im Durchgang
  gegen die Engine abgewiesen, weil `GameWorld` keinen Default-Macher hat (Konstruktor `GameWorld(Renderer&, NightWindows* = nullptr)`;
  engine_make_default kennt nur Klassen mit Konstruktor ohne Eingänge) → on_missing macht kein Objekt → call_engine false → „refused“.
  Das ist keine Frage der Form. Vorschlag (allgemein, im Generator): ein Default-Macher auch für Klassen, deren Konstruktor nur Objekte der Engine
  nimmt, die selbst einen Default-Macher haben (und Parameter mit Default) – die mitgemachten Objekte unter eigenen Handles im selben EngineObjects,
  freigegeben mit ihm. Nicht umgesetzt (über Punkt 2 hinaus; Testrahmen real = t1) – Entscheidung des Koordinators.
- **Beleg Punkt 2+3 (00:57):** `work-d-belege-vergleich-002858-13659` (work-d-belege 07bd7074: work-zusammen 310c6e10 + work-d-count + work-d-array;
  game.cpp + game_gaps.cpp): convert rc=0, bau rc=0; **Stellvertreter 47/47 ok** – matrix_of weicht nicht mehr ab (Basis t2 223555: 88/88 und
  90/90), gaps_guided_sounds weicht nicht mehr ab (Basis 11/37, 12/38). **Real:** gaps_guided_sounds 39 verglichen, keine Abweichung.
  Real-FAIL nur „only 0 calls could be compared“ bei body, eject, hide_soldier, matrix_of, state_of, step_parachute – alles Aufrufe auf GameWorld,
  die die Engine abweist (kein Default-Macher, BEFUND oben); in der Basis genauso (matrix_of real 0 verglichen). Nichts Neues.
  cost-Zeile gaps_guided_sounds (Klasse fordert bis 3× Speicher, Quelle 0) – in der Basis genauso.
- Engine-Suite läuft: `work-d-belege-engine-005831-17971` (ganze Suite: E14-Test, engine_stand_in_fixed_length, geänderte Signaturen).
- **Punkt 4 (Auftrag Koordinator 01:0x: Macher für Klassen ohne Standard-Konstruktor)** gebaut: Zweig **work-d-maker** = **b317557a**
  (auf work-zusammen 2dacbbb5). Calls.default_makers: Werte → Null (ev::blank), Zeiger auf Objekt → null, Referenz auf Objekt der Engine →
  mitgemacht ohne Handle (ev::MadeWith, EngineObjects::put_made, Entry::owner); abstrakte Referenz → die eine abgeleitete machbare Klasse.
  Neu gemacht: Engine, GameAudio, GameWorld, SoundBank, net::Client, net::Server. `--list`: nicht gemacht nur kern::net::Transport (abstrakt).
  Hinweis: GameAudio(Mixer&, SoundBank&) bekommt zwei unabhängige Mixer (jedes Objekt für sich gemacht – keine Annahme über Teilen).
  Engine-Test engine_calls_default_objects_made_with, Doku. Syntax ✓, Generator reproduziert. Beleg folgt (vergleich net_session real; Engine-Suite).

## BEREIT ZUM MERGEN (d, 01:35): work-d-count = 0c6840cc (E14 set_pcm/create_clip) + f384bdfd (Count im Typ) – Belege work-d-count-vergleich-000943-8780, work-d-belege-vergleich-002858-13659, work-d-belege-engine-005831-17971
- Auf work-zusammen e4aa2e8d, nur diese zwei Commits (E14 zuerst: mit der Typ-Regel meldet der Erzeuger sonst set_pcm/create_clip). Session_push ok,
  kein ABBRUCH converted_net_session (real); gaps_guided_sounds ok in beiden Durchgängen. **Engine-Suite rc=0** (54469 checks, 0 failures) auf work-d-belege 07bd7074.

## BEREIT ZUM MERGEN (d, 01:35): work-d-array = 83733dad (Form `[N item]`, Feld T m[N]) – Belege work-d-belege-vergleich-002858-13659, work-d-belege-engine-005831-17971
- Auf work-zusammen e4aa2e8d. matrix_of: Stellvertreter ok (Basis 88/88). Real weiter „0 compared“ – Ursache GameWorld ohne Macher → Punkt 4 (work-d-maker).
  engine_stand_in_fixed_length ok in der Suite.
- Punkt 4 (work-d-maker b317557a): Beleg läuft – vergleich net_session.cpp + game.cpp (real: Session_connected/join/stop, matrix_of …).
- **Beleg Punkt 4 (01:54):** `work-d-maker-vergleich-013431-7648` (b317557a, ohne die Fixe 1–3; net_session.cpp + game.cpp): convert rc=0, bau rc=0.
  **Real:** converted_Game__game_cpp ok – matrix_of, eject, state_of, hide_soldier, step_parachute, body werden jetzt verglichen, keine Abweichung
  (vorher „only 0 calls could be compared“). Session_connected/join/stop: 300 verglichen, keine Abweichung (vorher 0). Übrig real: ABBRUCH-artiger
  ASan heap-buffer-overflow in converted_net_session = push_incoming (Punkt 1, auf diesem Zweig nicht enthalten); note Session_join/stop:
  „engine.client.release(n) (the handle names no Client that was made)“ in 52–62 von 300 Aufrufen neu gezogen (die Quelle gibt einen Client frei,
  den die Engine nie gesehen hat – Testrahmen, BEFUND FÜR t1). Stellvertreter: matrix_of / Session_push weichen ab = Punkte 2/1 (nicht auf diesem Zweig).
  alle-real.txt nennt converted_net_session_cpp FAIL, seine Einzelausgabe sagt „0 failures“ (wie bei game_gaps in 002858) – ungeklärt, Bitte an t1/Koordinator.
- Engine-Suite läuft: `work-d-maker-engine-015441-9451`.

## Nachfolger 5 (ab 04:40, 19.11.) – Arbeitsliste D (KOORDINATOR 04:40)
- **D1 Lecks – Fix gebaut:** Zweig **work-d-leck = 16d72b56** (auf work-zusammen 12438d1c). Ursache: Handles eines Tests sind Nummern je Klasse
  (Stellvertreter kept[handle % 8] je Klasse); fragte ein Aufruf eine Nummer als Klasse B, unter der schon ein A stand, machte on_missing ein B
  und EngineObjects::place ÜBERSCHRIEB den Eintrag des A (nie freigegeben, als A verloren). Jetzt: je Handle eine Liste von Einträgen (gemachtes
  Objekt steht neben dem ersten; release/clear beenden jedes). Engine-Test engine_calls_default_objects_by_class; Kleinstfall ASan alt 20120 B
  verloren, neu nichts. Beleg läuft: `work-d-leck-vergleich-044258-11385` (world_marks, net_session, terrain).
- **D2 weld – Fix gebaut:** Zweig **work-d-weld = f31ea939** (auf 12438d1c). Ursache: der Test zieht chunk.mesh mit Listen beliebiger Länge;
  kern::weld las normals/colors/uvs je Ecke ohne Prüfung (upsert prüft dieselben Längen). Jetzt prüft weld wie upsert, sonst bleibt die Suppe.
  Engine-Test weld_and_hash erweitert. Beleg folgt (vergleich city_stream.cpp + Engine-Suite Filter weld).
- **D1 belegt (05:00):** `work-d-leck-vergleich-044258-11385` (16d72b56; world_marks, net_session, terrain): convert rc=0, bau rc=0;
  **Stellvertreter 17/17 ok, Engine selbst 17/17 ok, 0 LECK** (Basis 032626: LECK net_session(_cpp), terrain(_cpp), world_marks_cpp).
  interiors/game_gaps/game_town (gleiche Ursache, Renderer/GameWorld/Mixer) laufen im nächsten vollen Lauf mit. Engine-Suite folgt (auf work-d-belege2).
- **D3 Ursache gefunden, Fix gebaut:** die note nennt „engine.input.down(4, 9) (this build has no function engine.input.down)“ bzw.
  „engine.engine.aspect(3) (this build has no function …)“ – keine fehlende Klasse, sondern die Komponente app (kern::Engine, kern::Input mit ihren
  Machern) war nicht im Testprogramm gelinkt (run_tests.sh: core … game ohne app; das Spiel baut mit --app --game). Zweig **work-d-app = d11cb0cf**:
  run_tests.sh linkt app, test_main.cpp gibt kern_create_app() = nullptr (wie kern/tests). Keine Regel des Makers nötig.
- Beleg-Zweig **work-d-belege2 = f5030c78** (work-zusammen 12438d1c + leck + weld + app, Generator reproduziert). Läuft:
  `work-d-belege2-vergleich-050059-20415` (city_stream.cpp, game_controls.cpp, game.cpp). Danach Engine-Suite auf work-d-belege2.
- 05:50: `work-d-belege2-vergleich-050059-20415` bau rc=1 (app.cpp braucht kern::c_api_renderer → auch kern/src/api linken; work-d-app 3cb92099).
  `work-d-belege2-vergleich-052701-29611` (city_stream, game_controls, game): **Stellvertreter 49/49 ok; real 48 ok, 0 LECK**, converted_city_stream(_cpp)
  ok (D2: kein ASan in weld mehr), converted_Game__game_cpp ok; einziges FAIL real converted_game_controls_cpp: UBSan app.h:40 Input::down(Key::count)
  (Test zieht count als Wert von Key) → Zweig **work-d-key = 0c8e767f** (Input prüft den Bereich wie bus_name).
- Lauf 043356 gegen die Engine: übrig „the handle names no Client/RoadBuilder/Earcut/LightPool that was made“ bei release (BEFUND t1: look_apply_tier)
  → Zweig **work-d-release = 28b67b2b**: release eines Handles ohne Objekt beendet das, das on_missing dort macht (LightPool hat einen Macher).
- Beleg-Zweig work-d-belege2 = 7b1a05ed (+key, +release). Läuft: `work-d-belege2-vergleich-055045-6922` (city_stream, game_controls, game, net_session,
  game_look). Danach Engine-Suite auf work-d-belege2.
- **Beleg 06:21:** `work-d-belege2-vergleich-055045-6922` (7b1a05ed = work-zusammen 12438d1c + leck + weld + app + key + release; city_stream, game_controls,
  game, net_session, game_look): convert/bau rc=0; **Stellvertreter 52/53 ok, Engine selbst 52/53 ok, 0 LECK, 0 ABBRUCH, kein „refused“ mehr in
  irgendeinem Test** (vorher: key/update_camera … „this build has no function“, client.release / LightPool-release abgewiesen). converted_city_stream(_cpp),
  converted_Game__game_cpp, converted_game_controls_cpp, converted_net_session(_cpp) ok in beiden Durchgängen.
  Einziges FAIL: converted_Game__game_look_cpp in BEIDEN Durchgängen – SEGV strlen(nullptr) in look_apply_tier (Testtext :5610, std::string aus nullptr);
  genauso in der Basis 032626 und 043356 → nicht von d (Familie „std::string aus nullptr“ wie converted_life_cpp, t1).
- Engine-Suite läuft: `work-d-belege2-engine-062159-12230`.
- Berichtigung: work-d-key = **2562e856**, work-d-release = **7b1a05ed**, work-d-belege2 = **28b67b2b** (oben vertauscht genannt).
- **Engine-Suite `work-d-belege2-engine-062159-12230` auf 28b67b2b: engine rc=0** (u. a. ok weld_and_hash, engine_calls_default_objects(_made_with, _by_class),
  app_input_and_args).

## BEREIT ZUM MERGEN (d, 07:05) – fünf kurze Zweige, je ein Commit auf work-zusammen 12438d1c (sauber mischbar; zusammen = work-d-belege2 28b67b2b)
1. **work-d-leck 16d72b56** (D1, Engine: EngineObjects je Handle mehrere Einträge – kein Überschreiben) – Belege `work-d-leck-vergleich-044258-11385`
   (17/17 beide Durchgänge, 0 LECK), `work-d-belege2-vergleich-055045-6922`, Engine-Suite 062159 rc=0.
2. **work-d-weld f31ea939** (D2, Engine: weld prüft Längen der Ströme wie upsert) – converted_city_stream(_cpp) ok gegen die Engine in 052701 und 055045; Engine-Suite rc=0.
3. **work-d-app d11cb0cf + 3cb92099** (D3, Testbau: app und api gelinkt wie im Spiel, kern_create_app in tests/test_main.cpp) – kein „this build has no function“ mehr
   (055045; converted_Game__game_cpp, converted_game_controls_cpp ok gegen die Engine).
4. **work-d-key 2562e856** (Engine: Input liest keine Taste ab Key::count) – nötig mit 3: sonst UBSan app.h:40 in game_controls (052701); in 055045 ok; Engine-Suite rc=0.
5. **work-d-release 7b1a05ed** (Engine-Objekte: release eines Handles ohne Objekt beendet das von on_missing gemachte – Client/RoadBuilder/Earcut/LightPool,
   BEFUND t1 look_apply_tier) – in 055045 kein „refused“ mehr; Engine-Suite rc=0.
- Nicht von d: converted_Game__game_look_cpp FAIL in beiden Durchgängen (SEGV std::string aus nullptr in look_apply_tier, Testtext :5610) – wie in 032626/043356.
- Hinweis beim Mischen in work-d (Platz der Engine): work-d zählt in EngineObjects::place/end/clear eine Generation (++generation_) – beim Mischen von 1 dort beibehalten.
