# Paket k – Stand

## Nachfolger 3 (ab 07:30, work-k auf work-zusammen 821208ac)

## BEREIT ZUM MERGEN: 977e515b (E18: 26e4c7c7 Sprache + b622d9a8 Konverter, eingemischt work-zusammen 0cb3f6b6) – Beleg CI 44496602788 + work-k-vergleich-105013-14336
- Sprache: CI auf 26e4c7c7 native 200 ok/0 FAIL (test_main, blueprint_native_test), build „Logic JIT: all cases match“ (held_entry), determinism/input ok.
- Konverter: 105013 (game_traffic, game_net, game_townsync, game_traffic_far, vehicle_art, game.cpp) alle rc=0, 54/54 in beiden Durchgängen; die Funktionen
  mit „may run handlers“ / „~Car“ warten dort nur noch an Teilen, die die Probe nicht hat (focus_position, mp_local_ready, …) – geschrieben/verglichen zeigt
  erst der volle Lauf. Merge 977e515b nur lokal auf py_compile geprüft (Konflikte keine).
- Offen für den Nachfolger: Destruktoren mit Rumpf (Unit.life → body; Funktion für ~Car/~Ragdolls schreiben und in ended_parts vor den Teilen aufrufen; std::move-Regel
  bei Zeile ~1918 wartet ebenfalls an „destructor“), „a pointer“ (5). Im Vergleichstest muss ein held_lost der Klasse „C++ leaves open“ sein (t1).

## E18 (V1) gebaut – 26e4c7c7 (Sprache), b622d9a8 (Konverter) – unbelegt
- Angefasst (für Paket e): logic.cpp NUR Instance::call (Längen vor/nach), neu Instance::lengths_on, Runtime::held_lost (neben Runtime::pass); logic.h
  Deklarationen; notation.cpp places_ok (Shape::held_calls statt Abbruch), Writer func.call + held_lists, Reader statement (Held/held_after = nichts);
  blueprint.h logic::Held + BlueprintCore::held_after; tests: test_main (Rundreise), logic_cases „held_entry“; docs/blueprint_cpp.md.
- Konverter: place_arg ohne raising-Zweig (kein Wait, keine Kopie); movable_entry/may_raise entfernt.
- **Sprache belegt (CI 44496602788 auf 26e4c7c7):** native 200 ok / 0 FAIL (test_main mit den neuen Fällen, blueprint_native_test: 61 Klassen), build „Logic JIT: all cases match“ (held_entry im JIT), determinism, input ok. (mag-Engine-Lauf 102727 abgebrochen – doppelt.)
- Konverter b622d9a8: Beleg work-k-vergleich-105013-14336 (game_traffic, game_net, game_townsync, game_traffic_far, vehicle_art, game.cpp) läuft.

## ENTSCHEIDUNG NÖTIG (10:05) – E18 (A): wie erfährt die KLASSE von einem gehaltenen Eintrag?
- Interpreter: Instance::call mit `places` (logic.cpp ~2590) kann vor dem Aufruf je Liste beliebiger Länge auf dem Weg des Platzes Länge (und Identität des
  Speichers) merken und danach vergleichen → Fehler. JIT: ruft für func.call die C++-Funktion des Knotens (codegen.cpp: nur Fluss/Zahlen inline) → bekommt
  dieselbe Prüfung ohne eigenen Code. Leser places_ok: den Abbruch (notation.cpp ~875) durch „erlaubt, geprüft“ ersetzen.
- Die Klasse ist der Text der Notation, übersetzt gegen blueprint.h: der Aufruf ist `f(a[i])` mit `T&` – an der Aufrufstelle gibt es nichts, das prüft.
  V1 (Vorschlag): der Schreiber setzt um genau solche Aufrufe (Funktion mit raises, Platz durch eine Liste beliebiger Länge) eine Prüfung in den Text,
  z. B. `{ const kern::logic::Held held_1(a); f(a[i]); held_1.after(); }` (Länge + data() vorher/nachher → festgelegter Fehler); from_cpp erkennt die Form
  und liest denselben Graphen (keine neue Knotenart, Rundreise bleibt). Kosten nur an diesen Aufrufen (zwei Vergleiche). Wie E18: Prüfung NACH dem Aufruf –
  dazwischen greift die Klasse wie C++ in freigegebenen Speicher (ASan meldet das im Test).
  V2: jede Listenoperation (logic::list::add/insert/remove/clear/resize/reserve, Set einer ganzen Liste) prüft gegen eine Liste gerade gehaltener Listen →
  Fehler schon beim Ändern (vor jedem toten Zugriff). Braucht die Anmeldung an der Aufrufstelle trotzdem (also auch V1-Text) und kostet in JEDER Klasse bei
  JEDER Listenänderung einen Sprung – gegen die Leistungsregel.
- Offen dazu: „Speicher“ zwischen den Stufen gleich? Interpreter-Listen (vector<Value>) und Klassen-Listen (vector<T>) wachsen gleich (Elemente), aber
  reserve/Zuweisung können abweichen. Vorschlag: Kriterium Länge in allen Stufen, data() zusätzlich nur in der Klasse; weicht eine Stufe ab, zeigt es der
  Vergleich (nie still gleich).
- Umfang V1: notation.cpp (Schreiber call_args, Leser), blueprint.h (Held), logic.cpp (Instance::call), places_ok, Fälle in kern/tests (bleibt / Länge ändert /
  Map / besessener Wert), docs/blueprint_cpp.md, dann der Konverter (place_arg: raising-Zweig weg). Paket e: ich fasse in logic.cpp nur Instance::call an.

## BEREIT ZUM MERGEN: 3878258a (4e72f8b4 Terrain_run_job + ed63dca3 Reihenfolge der Argumente + 3878258a) – Beleg work-k-vergleich-081204-23050, 091538-11174
- 081204 (game_net, net_session, combat_rules, game_events, terrain, game.cpp): Stellvertreter 58/58 ok – converted_terrain_cpp ok, Terrain::run_job geschrieben
  und verglichen (vorher 13/300 bzw. im Einzel-Lauf 074913 8/184); Engine 57/58 – nur Terrain_run_job über j.id der echten Engine (BEFUND FÜR d).
- 091538 (+ mp_art, combat_art): alle rc=0, 54/54 in beiden Durchgängen, kein Rückschritt durch ed63dca3.
- mp_hud_json selbst war in beiden Proben nicht geschrieben (fehlende Teile der Kette: mp_local_ready → mp_art, dann safe_mobile_marker_point → mp_rules);
  095224-26050 (+mp_rules, combat_rules): 60/60 in beiden Durchgängen ok, aber mp_hud_json wieder nicht geschrieben (player_position wartet), draw_events
  ebenso (CityStream::ground_height) – die Kette ist zu lang für eine Probe: **mp_hud_json unbelegt, zeigt der volle Lauf work-zusammen-voll-100601-31771.**

| Abweichung | Ursache | Commit | Beleg | Stand |
|---|---|---|---|---|
| mp_hud_json 7/20 (Stellv.), 4/20 (Engine): "connected" Quelle false / Blueprint true; gegen die Engine „source asks server.running, Blueprint server.connected_clients“ | Reihenfolge: ein Argument mit Schritten (`esc(net_.status())` → `a_2 = Session_status(net_)` gehoben) lief vor den Argumenten links davon (hosting(), connected() standen in der Anweisung) – status() ruft connected() und die Engine | ed63dca3 (operand: Eingänge von Aufrufen des Programms/der Engine/Formaten von links; Links-Stehendes, das die Schritte sehen/ändern können oder das selbst ruft, wird vorher gehalten) | 081204-23050: mp_hud_json NICHT geschrieben (Probe: mp_local_ready → MpLocal::init in mp_art.cpp fehlte); neu work-k-vergleich-091538-11174 (+mp_art, combat_art) | gebaut |
| Terrain_run_job 13/300 (Stellv.), 48/300 (Engine): jobs.submit, Bytes ab 16 | Test-Schreiber: `memcpy(bytes, &win, sizeof win)` einer Struktur mit f32 kopierte die Anordnung des Tests (f32 = double) – nur memcpy(&rec, bytes) war behandelt | 4e72f8b4 (converted::layout::to_bytes) | work-k-vergleich-081204-23050 (3878258a): Stellvertreter 58/58 ok, converted_terrain_cpp ok – Terrain::run_job geschrieben und verglichen; gegen die Engine 57/58: run_job 35/237 nur noch j.id (BEFUND FÜR d) | **belegt (Stellvertreter)** |
| Session_update 1/56, life_hud 1/33 (Engine) | in 043356 (mit den Fixen des Koordinators) keine Abweichung mehr, nur cost | – | work-zusammen-suite-alle-043356-8771 | erledigt (Einzel-Lauf auf neuem Level bestätigt) |
| draw_events 9/20 (`.combat_art_.parts_[0].transforms`) | kein Konverterfehler: die abweichenden Matrizen sind Pfosten der Renn-Tore (CombatArt::gate, box). Der Test gibt `combat_kGates` (7, 12) vor, `combat_rules_g_gates` aber weder vor noch vergleicht er es – Blueprint: leere Liste (Tor = Nullen), Quelle: `g_gates[k]` einer Liste, die kGates nicht entspricht (jenseits des Endes / Rest früherer Aufrufe) | – | Einzelausgabe 043356 | BEFUND FÜR t1 |

### ENTSCHEIDUNG NÖTIG (Aufgabe 5, größter Grund): „an entry of a list handed to change to a function that may run handlers“ (8 Funktionen, 811 Zeilen) – und vermutlich ein Teil von „~Car … not written yet“
- Das ist keine Regel des Konverters, sondern die Prüfung der SPRACHE (kern/src/logic/notation.cpp places_ok, ~Z. 875): ein Eintrag einer Liste beliebiger
  Länge / einer Map / eines besessenen Werts (unique_ptr = Liste 0/1) darf nicht als Referenz an eine Funktion gehen, die Handler laufen lassen kann
  (Call on Actor, Call Event: `raises`) – „Give it the entry's index instead, or the value“. Der Konverter folgt ihr: Referenz zum Ändern → Wait;
  const-Referenz → er übergibt eine KOPIE (place_arg, raising). Die Kopie ist bei Typen mit Destruktor falsch: C++ bindet nur, beendet nichts – der Blueprint
  müsste ~Car() der Kopie laufen lassen (detach(): Engine-Aufrufe!). Vermutung: draw_vehicles/draw_remote_vehicles/swarm_draw_vehicles warten darum an „~Car“
  (`art_.car(…, *c.car, …)`; Kleinstfall /tmp/k_car ohne Handler: als Platz geschrieben, kein ~Car). Unbelegt, ob VehicleArt::car `raises` hat.
- C++ selbst: die Referenz bleibt; ändert ein Handler die Länge der Liste, ist das UB. Wo C++ ein Verhalten hat (die Liste bleibt), hält die Referenz.
- Möglichkeiten: (A) Sprache: ein gehaltener Eintrag über einen Schritt mit `raises` ist erlaubt; jede Stufe (Interpreter, JIT, Klasse) prüft, dass die Liste
  in der Zeit ihre Länge/ihren Speicher behält, sonst bricht der Lauf mit Fehler ab – genau dort, wo C++ UB hat (Kosten in der Klasse: ein Vergleich nach dem
  Aufruf). (B) Sprache, statisch: Call on Actor nennt die Funktion nur beim Namen, das Ziel ist zur Laufzeit beliebig – statisch nicht entscheidbar, verworfen.
  (C) Absenkung der 8 Funktionen (Index statt Referenz; Game-Dateien von g1/g2) mit Differenztest „zwei Bauten“. (D) Kopie hinein/heraus – ändert das Verhalten,
  wenn der Handler den Eintrag ändert: verworfen.
- Mein Vorschlag: (A) – allgemein, ohne Absenkung, ohne Kopie; gehört zur Sprache (Paket d/Koordinator), nicht zum Konverter. Danach fällt im Konverter der
  Zweig raising in place_arg weg (Wait und Kopie).

### BEFUND FÜR d (08:55) – „a member of Int is set“ (vr_comfort, look_draw, begin_roads; 3 Funktionen)
- Beispiel vr_comfort: `kern::TextureDesc td; td.rgba = px; td.width = 64; …` – TextureDesc ist wegen `const u8* rgba` ein Objekt der Engine mit Handle
  (engine.texture_desc.make/release in engine_api.gen.cpp), im Blueprint Int; ein Feld dahinter zu setzen hat keinen Knoten. Allgemein: Felder eines
  Engine-Objekts lesen/setzen (oder TextureDesc als Daten mit Zeiger = Liste+Platz) – Gebiet Engine-Objekte (d), keine Regel des Konverters.

### BEFUND FÜR d (09:20) – Terrain_run_job gegen die Engine (work-k-vergleich-081204-23050: 35 von 237)
- Gegen den Stellvertreter ok. Gegen die Engine weicht nur `j.id` ab (Quelle 4, Interpreter 6, Klasse 5; bzw. 7/9/8): jeder Durchgang reicht bei der ECHTEN
  Engine einen Job ein (kern::jobs::submit), ihr Zähler läuft weiter – wie die echte Uhr (clock_ms). Kein Konverterfehler.

### BEFUND FÜR t1 (10:15) – Session_update 1/56, 2/64 (Einzel-Lauf 080335, Stellvertreter)
- „call 3: the source asks engine.client.advance_time, the Blueprint engine.client.connect“ – net_session.cpp:967 `client_->state() < ClientState::disconnected`.
- Ursache im Testrahmen: converted.h:667 `arg(E e) { return Value::enumeration(static_cast<i64>(e)); }` gibt die C++-ZAHL des Enums; ein Enum der Sprache ist
  sein INDEX (der Blueprint: `list_get<Int>(net_ClientState_numbers, enum_index(...))`, Zahlen {-3,…,3}). disconnected (0) wird so Index 0 = denied (−3) → der
  Blueprint verbindet. Betrifft jedes Enum, dessen Zahlen nicht 0…n−1 sind, wo der Stellvertreter einen Wert der Engine gibt (und Listen: Value::enumerations).
  Allgemein: Zahl → Index über die Zahlen des Enums (der TestWriter kennt sie: Unit.enum_numbers). Kein Konverterfehler.

### BEFUND FÜR t1 (07:35) – draw_events
- `gate(i)` = `g_gates[((i % kGates) + kGates) % kGates]` (combat_rules.cpp:478); Vorbedingung des Programms: kGates == g_gates.size() (bind_race setzt beides).
  Der Test von Game::draw_events gibt combat_kGates zufällig (7, 12), g_gates (Datei-Zustand von combat_rules, in Game `combat_rules_g_gates`) steht nicht
  in seinem Zustand – die Quelle liest außerhalb der Liste (UB) bzw. was ein früherer Aufruf dort ließ. Erste Abweichung: Pfosten (`put(box, …)`, m[10] = c.z − h/2:
  Quelle 25.115 = race_origin.z − 0.5, Blueprint 25.615 mit h = 0). Kein Konverterfehler.


## BEREIT ZUM MERGEN: 09dd6818 (B3 Index mit Schritt, auf B0–B2) – Beleg work-k-vergleich-062739-13858
- vergleich game_foot_rounds, game_military, game_gaps, gaps_rules, road_graph, game.cpp auf 09dd6818: convert rc=0, bau rc=0; Stellvertreter 54/54 ok;
  Engine selbst 53/54 – converted_Game__game_gaps_cpp nur durch LECK (MadeWith<GameWorld>, engine_make_default: Paket d), 0 Abweichungen.
  Game::rocket_smoke_emit und Game::gaps_guided geschrieben und verglichen (vorher 2/20 bzw. 10/20). spawn_squad/spawn_tank warten in der Probe
  (CityStream::ground_height, gaps_spawn_viewers) – gleiche Gestalt, zeigt der volle Lauf.

## BEREIT ZUM MERGEN: 5d804704 (B1 cdcd2f51 + B2 5d804704, auf B0) – Beleg work-k-vergleich-054057-841
- vergleich sound_fx.cpp, game_life.cpp, game.cpp auf 5d804704: convert rc=0, bau rc=0, Stellvertreter 46/46 ok, Engine selbst 46/46 ok, keine Abweichung.
  Mat4::perspective, projection_matrix, SoundFx::drive, SoundFx::update geschrieben und verglichen. life_hud (Game/game_life_cpp) war in der Probe nicht
  geschrieben (0/11, Teile fehlen) – zeigt erst der volle Lauf. 5d804704 ändert die Engine-Tabelle (8 voice-Eingänge Int64): Engine-Suite im vollen Lauf.

## BEREIT ZUM MERGEN: 8b8c0e10 (B0 kQuiet) – Beleg work-k-konvert-044336-12397
- konvert origin/work-k (8b8c0e10, mit work-zusammen 12438d1c + work-t1 c5b8ab99): **convert rc=0** (05:40, 56 min auf 9 Kernen).
  1695 von 1844 Funktionen geschrieben (21450 von 30978 Zeilen; 020121: 1637 / 19218), 1652 verglichen (020121: 1594).
  „cannot compare it yet“: 20 (020121: ~116) – übrig: get_u8/dispatch-Familie (gaps_send_jets, melee_hit, gaps_seat_press, gaps_send_military, hit_remotes),
  SoundBank::request_level (audio_init), Graph (vehicle_actor) – alles Test-Schreiber (t1).
- Hinweis: work-k hat danach cdcd2f51 (B1) und 5d804704 (B2, mit neu erzeugter Engine-Tabelle) – noch unbelegt; für B0 allein 8b8c0e10 mergen.

## 04:45 B0 kQuiet
- work-k = 8b8c0e10 (mit work-zusammen 12438d1c, work-t1 c5b8ab99; dc7a12cc = Rücknahme 90ecf01f nach E13).
- Regel: Konstanten, die eine Quelldatei für sich hat (Namensumgebung / `static const` einer Funktion), die in mehreren Quelldateien der Teile
  EINES Blueprints vorkommen, heißen `<datei>_<name>` (survey_one → World.constant_names → Unit.constant; wie plan_names bei Typen).
- Kleinstfall /tmp/k_mini2 (Zustand W, w_a.cpp kQ=0.6, w_b.cpp kQ=0.5): alt rc=1 „W: two parts have a constant kQ …“, neu rc=0 (w_a_kQ, w_b_kQ).
- Beleg: `work-k-konvert-044336-12397` (konvert origin/work-k 37) – läuft.

| Abweichung | Ursache | Commit | Beleg | Stand |
|---|---|---|---|---|
| B0 kQuiet (convert rc=1) | dateieigene Konstanten zweier Teile nach Namen zusammengeführt | 8b8c0e10 | work-k-konvert-044336-12397: convert rc=0, 1695/1652, 20 wartend | belegt |
| B1 Mat4_perspective 300/300, projection_matrix 62/300 (.m[15] Quelle 1) | Test-Schreiber: `memset(r.m, 0, sizeof r.m)` → sizeof des Programms (64), im Test sind die 16 f32 aber 64-bittig: nur m[0..7] genullt | cdcd2f51 (converted::layout::set_bytes) | work-k-vergleich-054057-841: beide Durchgänge 46/46 ok (Mat4::perspective, projection_matrix geschrieben und verglichen) | belegt |
| B2 SoundFx_drive/update `.l.voice` | qtype löste `VoiceId` (= u64) zu `unsigned long` → Int; Engine-Tabelle ebenso (voice-Eingänge der Mixer-Knoten Int) | 5d804704 (WIDE_NAMES; engine_api neu erzeugt: 8 Eingänge Int64) | work-k-vergleich-054057-841: SoundFx::drive/update verglichen, beide Durchgänge ok; Level las die Texte (convert rc=0) | belegt (Engine-Suite noch offen) |
| B3 rocket_smoke_emit 2/20, spawn_squad 10/20, spawn_tank 5/20 (rng_.s), vermutl. gaps_guided | Name für einen Eintrag, dessen Index etwas ändert (`r = g.a[g.cur++ % n]`, `L = lanes[rng_.next() % n]`): steps_in sah Änderungen an Feldern des Objekts / hinter *ptr nicht → Index je Nutzung neu (4× ++, mehrere rng-Züge) | 09dd6818 | work-k-vergleich-062739-13858: rocket_smoke_emit, gaps_guided geschrieben, beide Durchgänge ohne Abweichung (gegen die Engine nur das bekannte LECK GameWorld aus engine_make_default, d); spawn_squad/tank in der Probe nicht geschrieben | belegt für rocket_smoke_emit/gaps_guided; spawn_* zeigt der volle Lauf |
| B3 life_hud 1/33 (gegen die Engine) | `"bubbles"` x 0.5578/0.5579 – projection_matrix im Test der Quelle (B1) | cdcd2f51 | – | vermutlich mit B1 |
| B3 number_at 8/300, game_details `.Result` | kein Konverterfehler: alle 8 Fälle mit from > s.size() – `s.c_str() + from` jenseits des Endes (UB) | – | Einzelausgabe 032626 | BEFUND FÜR t1 |

## BEFUND FÜR t1 (06:05, aus work-zusammen-suite-alle-043356-8771)
- **converted_Game__game_equipment_cpp ASan SEGV** in Game::ui_radio (strlen(nullptr) in `std::string(…) + MusicSynth::kStations[ui_.station].name`):
  `ui_.station` kommt als beliebige Zahl aus dem Test; nur 0 ≤ station < station_count() ist ein Zustand des Programms (ein negativer Rest von
  `(station + dir + n) % n` bzw. station ≥ n liest außerhalb der Tabelle). Dasselbe Bild (strlen/operator+) bei look_apply_tier (game_look_cpp).
  Kein Konverterfehler – Testeingaben bzw. Tabelle im Test (t1).

## OFFEN (k, nicht weiter verfolgt)
- mp_hud_json 7/20 (Stellvertreter), 4/20 (Engine): `"connected"` Quelle false, Blueprint true (Session::connected: `client_ && client_->connected()
  && have_origin_`). Text des Blueprints und des Tests stimmen in der Gestalt; Verdacht: Zahl/Reihenfolge der Aufrufe engine.client.connected vor
  dieser Stelle (Antworten des Stellvertreters der Reihe nach) – zuerst mit dem Lauf nach 09dd6818 erneut ansehen.
- draw_events 9/20: `.combat_art_.parts_[0].transforms[45]` +0.5, [12]/[79] −0.2465 – Ursache nicht gefunden (Wrack-Schleife im Text richtig).

## BEFUND FÜR t1 (04:55)
- **number_at 8/300 (converted_game_details_cpp):** `number_at(s, from, …)` liest `strtoll(s.c_str() + from, …)`; alle abweichenden Fälle haben
  from > s.size() (hello world/13, Abc/22, hello world/25) – C++ hat dafür kein Verhalten (Zeiger hinter das Ende), die Quelle liest Speicherrest.
  Der Test darf nur from ≤ s.size() geben (Vorbedingung aus den Aufrufern: next_at liefert npos oder eine Stelle in s).

# (früher) Auftrag 22:25

## BEREIT ZUM MERGEN: 20cb5259 (enthält 2a6ef5b3, 12e3da24) – Beleg work-k-vergleich-224717-8097
- combat_rules.cpp, drone.cpp, city_stream.cpp auf 20cb5259, beide Durchgänge: converted_combat_rules_cpp ok (flak_cell, G4),
  converted_drone_cpp ok (Drone_step, G7), CityStream_upload keine Abweichung mehr (G6); 17 ok von 19.
- Übrig dort, nicht von diesen Commits: CityStream_buildings_near 5/300 (u64-Schlüssel – Fix fa3dc97d, Beleg läuft) und
  rigid_spec/create_body (BEFUND FÜR d, inertia[6]); LECK bei city_stream(_cpp) wie im Lauf 222045 (vor t1s Testrahmen).
- 12e3da24 ist ein Ausweichen vor dem Fehler der Klasse (siehe unten) – zurücknehmen, sobald ps Fix in work-zusammen ist.

| Abweichung | Ursache | Commit | Beleg | Stand |
|---|---|---|---|---|
| G4 flak_cell 201/255 | snprintf %lld eines i64 ging über int64_to_int (2^53); danach ASan use-after-free (Text-Temporär in text_format) → Text in eigener Variable | 2a6ef5b3 + 12e3da24 | work-k-vergleich-224717-8097 | belegt |
| G7 Drone_step (u16 20268/85804) | `x++`/`x op= y` eines u16/u8 ohne Rückwandlung in den Typ (wrap32 statt band 65535) | 2a6ef5b3 | work-k-vergleich-222911-25281: converted_drone_cpp ok in beiden Durchgängen | belegt (auch 224717-8097) |
| G5 Car_new 300/300 | kein Konverterfehler: `Applied::on` unbestimmt (C++ lässt offen) | – | Quelle car.h:110 | BEFUND FÜR t1 |
| E3-C (p) fc_Filters `List<fc_PT1>` in FirmwareRuntime_new | kein Konverterfehler: Felder sind Fixed<fc_PT1, 3>; ein Wertliteral einer Struktur schreibt ihre festen Felder als List (Sprache, notation.cpp literal()) – die Klasse legt dafür einen std::vector an | – | Kleinstfall /tmp/k_mini, lesen.py | BEFUND FÜR d/p |
| G6 CityStream_upload 29/243 | `m[key].push_back(x)`: variable_of hob `put = t_2` (Zurückschreiben) vor `t_2 = m[key]` – zurück ging der alte Inhalt | 20cb5259 | work-k-vergleich-224717-8097 | belegt |
| CityStream_buildings_near 5/300 (t1) | Schlüssel u64 eines Map-Eintrags (`[k, t]`) ging als Int (Compiler nennt `unsigned long`) | fa3dc97d | work-k-vergleich-230203-14801: converted_city_stream_cpp ok in beiden Durchgängen | belegt |
| G6 CityStream_feed_terrain | im Lauf 222045-20677 nicht geschrieben (wartet) – nichts zu vergleichen | – | – | offen |

## BEFUND FÜR t1
- **Car_new 300/300 (converted_car):** `struct Applied { Vec3 p, f; i32 on; }; Applied applied_[6];` (car.h:110f.) hat für `on`
  keinen Initialisierer; `explicit Car(const CarSpec& s) : s_(s) {}` lässt `applied_[i].on` unbestimmt (default-initialisiert,
  C++ [dcl.init]/[basic.indet]). Der Quelltext gibt einen Speicherrest (8), der Blueprint 0. Der Test vergleicht einen Wert, den
  C++ offen lässt – „C++ leaves open“: nicht vergleichen (oder die Funktion so begründet warten lassen), keine Konverter-Regel.

## BEFUND FÜR d/p (E3-C, von p an k gegeben)
- Der Konverter typt die Felder richtig: `KERN_STRUCT(fc_Filters) { Fixed<fc_PT1, 3> gyro = List<fc_PT1>{…}; … }`. Die Liste in
  `fc_FirmwareRuntime{fc_StateRuntime{…fc_Filters{List<fc_PT1>{…}, …}}}` (FirmwareRuntime_new, Result-Vorgabe, `self = …`) ist das
  Wertliteral der Sprache: ein Strukturwert gibt jedes Feld, eine Liste darin ist `List<T>{…}` (blueprint_cpp.md Tabelle der Werte;
  notation.cpp `literal()` schreibt es so). Versuch (Kleinstfall /tmp/k_mini, lesen.py): `Fixed<PT, 3>{…}` im Wertliteral weist der
  Leser ab („a list of a fixed length is what a place is declared as - a value written out is List<PT>{...}“). Die Anforderung
  entsteht also in der Klasse: das Literal wird ein std::vector und dann in Fixed kopiert. Allgemeiner Weg (Klasse, nicht Konverter):
  ein Listenliteral an der Stelle eines festen Felds als `{…}` (Fixed hat einen initializer_list-Konstruktor) bzw. wie Make List
  in place schreiben. Nicht mein Gebiet – nicht umgebaut.

## BEFUND FÜR d
- **converted_rigid_spec / create_body 300/300** (work-k-vergleich-222911-25281, beide Durchgänge): call 6 der Engine
  `world.set_mass_data(b, mass, center, inertia)` – der Stellvertreter des Quelltexts zeichnet von `const f32 inertia[6]`
  (kern/include/kern/physics/physics.h:256) 1 Zahl auf, der Blueprint gibt 6. Länge eines Zeiger-Eingangs ohne Count (wie Gruppe 3).
- **Klasse text_format** (kern/include/kern/logic/blueprint.h:571ff.): `format_arg(const Text&)` macht eine Sicht; ein Wert, der kein
  Text ist (der std::string von PureNodes::int64_text*), wird dafür in einen vorläufigen Text umgewandelt, der vor `format::write`
  vergeht (ASan heap-use-after-free, 222911-25281 combat_rules_cpp). Der Konverter umgeht es jetzt (Text in eigener Variable,
  12e3da24); allgemein gehört ein `format_arg(const std::string&)` (Sicht auf den Wert selbst) in die Klasse.

## Zu 12e3da24 (Frage des Koordinators 22:5x)
- Was es ändert: der Text eines Int64 in einem Format wird vorher in eine eigene Text-Variable gesetzt (`digits = int64_text_signed(cx);
  key = text_format<"flak:l%s:%s">(63, digits, digits_2);`) statt direkt als Wert übergeben. Bedeutung gleich, eine Variable mehr.
- Urteil: **nur Ausweichen** vor dem Fehler der Klasse. Ich nehme es zurück, sobald ps Fix in work-zusammen ist (dann wieder
  `text_format<…>(63, int64_text_signed(cx), …)`), und belege das mit einem Vergleich über combat_rules.cpp.
- Kleinstfall für den Engine-Fall (Klasse, ASan): `kern::BlueprintCore::text_format<"%s">(63, kern::logic::PureNodes::int64_text_signed(
  kern::logic::Int64{0x69d6b3a6c1e2f7ee}))` – genauer: jeder Wert für `%s`, der kein `Text` ist (hier std::string, lang genug für den
  Heap, z. B. 19 Ziffern), wird in `format_arg(const Text&)` in einen vorläufigen Text gewandelt; `args[]` hält eine Sicht darauf, der
  Text vergeht am Ende der Initialisierung von args → heap-use-after-free in format::Sink::put (nodes.h:894), gesehen in
  work-k-vergleich-222911-25281, converted_combat_rules_cpp (flak_cell). Allgemeine Behebung: `format_arg(const std::string& v)`
  → `arg(std::string_view(v))` (der Temporär-String lebt als Argument von text_format bis zu dessen Ende).

## (entschieden: E9 – (b) ja, (a) später) Destruction_collapse 7/25, Destruction_fracture 11/20 / 12/28 (Lauf 222045-20677)
- Befund: Interpreter = Klasse ≠ Quelltext, erste Stelle: `pieces_[k].size` (x und z), p und v gleich. Ursache ist **keine Regel**:
  destruction.cpp:191/194 `launch({…}, {s.len * rng.range(0.85f, 1), 0.45f, band * rng.range(0.8f, 1)}, s.yaw, tint(0.18f));`
  – zwei Argumente ziehen aus demselben `rng` (size über rng.range, tint über rng.unit). In welcher Reihenfolge C++ die Argumente
  eines Aufrufs ausrechnet, lässt es offen ([expr.call]/8: indeterminately sequenced). GCC (der Test) rechnet hier von rechts
  (tint zuerst), der Blueprint – wie clang und wie das Original in JS – von links. Darum stimmen size.x/size.z nicht.
- Vorschlag: (a) **Konverter, allgemein:** ein Aufruf, bei dem mehr als ein Argument etwas ändert, das ein anderes Argument liest
  oder ändert (Aufrufe mit Wirkung, ++, Zuweisung), wartet mit „C++ leaves open in which order the arguments of a call are worked
  out“ (wie clear() bei 4068). Dafür braucht es eine Wirkungsanalyse der Argumente (Lambda mit Referenz-Fang wie `tint` zählt).
  (b) Die Quelle (destruction.cpp, Paket f2) wird gesenkt: die Werte in der Reihenfolge des Originals (links nach rechts) benannt,
  vor dem Aufruf. Das ändert ihr Verhalten unter GCC – der Differenztest (tools/source_diff.py) kann dann nicht grün sein,
  denn die heutige Quelle hat unter GCC ein anderes Ergebnis. Die Entscheidung, welches Verhalten gilt, liegt beim Koordinator/Chris.
  Ich schlage (a) + (b) vor: (a) macht jede solche Stelle sichtbar, (b) legt die Reihenfolge des Originals fest (Differenztest mit
  clang als Bezug oder als dokumentierte Ausnahme – das braucht eure Entscheidung).

## E9 umgesetzt: 90ecf01f – Absenkung destruction.cpp (Argumentreihenfolge, clang = von links)
- fracture: size/Farbe bzw. at/size/yaw/Farbe vor launch benannt; in launch vx, vy, up. Differenztest tests/lowered/test_destruction_lowered.cpp
  (1 Paar: Destruction::fracture). Kleinstfall /tmp/k_order/order.cpp: clang++ alt = neu, g++ alt ≠ neu.
- WARTET AUF: t1s Umstellung (tests/lowered mit clang) – dann Differenztest grün belegen, erst dann BEREIT ZUM MERGEN für 90ecf01f.
  Danach `vergleich` über destruction.cpp (unter GCC sollten collapse/fracture dann gleich sein).

## BEREIT ZUM MERGEN: fa3dc97d (+ Merge ad36eee9) – Beleg work-k-vergleich-230203-14801
- city_stream.cpp, destruction.cpp auf ad36eee9 (mit t1s Testrahmen): converted_city_stream_cpp ok in beiden Durchgängen (buildings_near,
  upload), 11 ok von 12, keine LECK. Übrig nur converted_destruction_cpp (collapse/fracture/blast: pieces_[i].size/.p – die
  Argumentreihenfolge, E9; die Absenkung 90ecf01f kam nach diesem Stand). Beleg dafür: work-k-vergleich auf 90ecf01f (destruction.cpp).

## E13 (ändert E9): Spiel-Suite baut mit clang – 90ecf01f wird NICHT gemeldet
- 90ecf01f (Absenkung destruction.cpp) wird auf work-k zurückgenommen („zurückgenommen wegen E13“), sobald t1s Umstellung da ist und
  ein vergleich über destruction.cpp unter clang collapse/fracture ohne Abweichung zeigt. work-k-vergleich-231448-21204 abgebrochen.
- Kleinstfall für E13: $S/k/order_e13.cpp (clang++ alt = neu, g++ alt ≠ neu). Kleinstfälle der Regeln: $S/k/mini_regeln.cpp.
- **Destruction_blast 1/161 (bzw. 1/171):** kein fehlender Map-Eintrag – erste Stelle `.Self.pieces_[1].size.x` (Quelle 6.409…, Blueprint
  5.904…), dieselbe Argumentreihenfolge (blast → collapse → fracture → launch). Fällt mit E13 weg.

## Voller Lauf 232247-24031: convert rc=1 (Game-Einheiten pendeln ab Runde 9)
- Diagnose im Ablauf: c32f607b (nur Meldung „goes round: <Einheit>: f: written -> <Grund>; exports differ: …“ vor dem Abbruch) –
  bitte in work-zusammen mischen, dann zeigt der nächste volle Lauf, was im Kreis geht. Einziger Hinweis im Protokoll: Runde 10
  „Game::combat_hud() was written and waits now (its test … combat::Fun has no member named ‘mul…“ (Test-Schreiber/g).

## WARTET AUF
- t1: Spiel-Suite mit clang (E13) → dann vergleich destruction.cpp unter clang, danach 90ecf01f zurücknehmen.
- p: text_format-Fix der Klasse → dann 12e3da24 zurücknehmen (vergleich combat_rules.cpp).
- einen gültigen vollen Lauf → Liste für meine Dateien (city_stream, destruction, combat_*, drone, car, Firmware-Einheiten).
