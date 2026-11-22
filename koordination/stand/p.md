# Stand Paket p – Nachfolger 2 (19.11.2026, ab 07:15)

## BEREIT ZUM MERGEN: 11968cee (E20 Schritt 4 + Map-Werte fester Breite) – Belege (22.11.)
- Commits seit 39ce0076: 6ae29b79 (Map<K, List<U8>> / Map<K, U8> in der Sprache; Konverter-Regel 15201aee zurück), c5bcd2fc + e8eb1e4f
  (Konverter: Listen fester Breite in std::unique/std::remove/std::find/==, Feld einer Breite beim std::move, memset 0), Bench-Aufrufe List<U8>,
  Merges a2151031 (bdb99729), 454e4a39 (a2e6dd1e), 11968cee (a06e5ce4: seit 454e4a39 nur game.cpp 1 Zeile aus work-zusammen + bench.cpp).
- engine `work-p-engine-084052-31649` (6ae29b79): rc=0, 55568 checks / 0 failures, blueprint_native_test 71 Blueprints (darin fixed_width_map_values).
- CI 44693119773 (6ae29b79): native, build (JIT == Interpreter über alle logic_cases, darin der neue Fall), determinism, input grün; e2e blueprint
  270/0; rot nur „compiles in the editor“ der ALTEN Level-Blueprints (List<Int> gegen Engine-Pins List<U8>, wie seit Schritt 4 – löst der volle
  Lauf mit neuem Level) und die 3 bekannten Stellen von work-zusammen; native-spawncity ebenso (altes Level).
- konvert `work-p-konvert-100419-30404` (454e4a39): convert rc=0, **1756 geschrieben / 1711 verglichen** (Bezug voll-045421: 1755/1710).
- vergleich `work-p-vergleich-091250-16776` (6ae29b79; nostr qr_code common wire mvt city_stream): 14/14 ok, beide Durchgänge, 0 Abweichungen
  (city_stream/terrain grün). vergleich `work-p-vergleich-094253-24884` (c5bcd2fc; city_stream net_session game_townsync game_destruction_sync wire):
  47/47 ok, beide Durchgänge, 0 Abweichungen.
- Zeit-Test (Level + Tests von konvert 100419, $S/p/lvl6, $S/p/conv6; tools/blueprint_bench.sh --check 1.10 15, alle Prüfsummen gleich):
  **21 von 29 über 1,10 (f32-Spalte), 20 von 29 (64-Bit-Spalte)** (vorher 24/23). Darüber (min64): sha256 4,51, from_hex 3,64, settings_escape 3,49,
  wire_get_u32 2,47, lamps_near 2,22, mulm 1,85, qr_encode 1,62, wrap_pi 1,44, rs_remainder 1,41, perf_governor 1,40, person_hit 1,30,
  settings_num 1,28, gf_mul 1,28, qnormalize 1,27, lamps_place_road 1,22, mvt_read_geometry 1,21, wire_put_f32 1,21, mvt_ring_area 1,17,
  to_local 1,16, audio_render 1,15 (f32-Spalte zusätzlich lamps_grid_key 1,42 – 64-Bit 1,03).

## 08:15 (20.11.) – Nachfolger 8: Map-Werte fester Breite gebaut (6ae29b79), Belege laufen
- work-p = 6ae29b79 (work-zusammen bdb99729 eingemischt a2151031). Sprache: Map<K, List<U8>> … und Map<K, U8> … (Leser, check, Interpreter
  Set at Key/fit/Vorgaben, Typwahl, Klasse/Glue Sized, nativer Test, Web-Editor, Doku), Fall fixed_width_map_values (lokal: Klasse == Interpreter
  == echtes C++), Konverter-Regel 15201aee zurückgenommen.
- Läuft: engine work-p-engine-081106-12834, konvert work-p-konvert-081110-12881, CI 44693119773. Danach vergleich.

## 03:00 – Map<K, List<U8>> entschieden (JA), Bauplan in uebergabe/p.md „Nachfolger 7“; work-p 39ce0076 (0f55c2f4 eingemischt). vergleich 001918 abgebrochen (Hänger, Auswertung offen). Kontext erschöpft – Nachfolger nötig.

## E20 Schritt 4 – Stand 02:45 (20.11.), ac4f6862 (work-zusammen d5f527c5 eingemischt) – NOCH NICHT BEREIT
- engine `work-p-engine-165514-3888` (15201aee): rc=0, 55596 checks, 0 failures. CI 44560240329 (344551db): native, build, input,
  determinism grün; e2e rot (Level auf work-p ist alt: Engine-Pins jetzt List<U8>, alte Blueprints List<Int> – löst der volle Lauf).
- konvert `work-p-konvert-013136-793` (ac4f6862): convert rc=0, **1643 geschrieben / 1598 verglichen** (Bezug 173143: 1699/1654).
  Rest des Rückgangs: 35× „a List<Int> where a List<U32|I32|U8> is needed“ – alles Maps mit Listen fester Breite als Wert
  (city_stream cell_items/cell_old/cell_ids Map<u64, vector<u32>>, game_gaps planks Map<i64, vector<u8>>, lamps/world_traffic grid_
  Map<…, vector<i32>>). Die Sprache verbietet Breiten in Map-Werten (notation.cpp ~2681, keyed_of); 15201aee schreibt solche Werte
  als List<Int> – dann passen sie nicht zu den C++-Plätzen derselben Liste (Parameter, Kopien). Nächster Schritt (allgemein): Map<K, List<U8>>
  in der Sprache (Kodierung im of, Interpreter wickelt beim Speichern in den Wert, Klasse hat es schon) – dann die Regel in btype zurücknehmen.
- vergleich `work-p-vergleich-001918-8866` (d38bc5ac) läuft seit 00:19 (vermutlich ein Hänger), Ergebnis offen. Zeit-Test: nicht gemessen.

## E20 Schritt 4 (Konverter schreibt die Breiten) – gebaut bis 344551db, Belege laufen (16:55)
Commits: 5f0d81f9 (Konverter: Plätze u8…i32 und int → U8…I32, List<U8>…; wire/put, Wurzel-Wegnahme beim Speichern),
4d612317 (Bytes of Text/S geben List<U8>; Engine-Pins in den Breiten ihrer Listen, engine_api.py neu), c2244413 + Folgende
(Klasse: Index fester Breite, logic::pinned in formula, Glue List<U8>-Felder, Sized → i64 implizit + format_arg/==), 0e4667ff/344551db
(Fall „jeder Knoten“). work-zusammen fb51251f eingemischt.
- vergleich `work-p-vergleich-163201-22238` (0e4667ff; nostr qr_code mvt city_jobs terrain): 14 von 15 ok in beiden Durchgängen;
  FAIL nur converted_terrain_cpp (set_water_cell u. a.) – NICHT von p: siehe BEFUND FÜR k.
- Läuft: engine `work-p-engine-165514-3888`, konvert `work-p-konvert-165517-3943`, CI (dispatch 16:55).

## BEFUND FÜR k (16:55)
Terrain::set_water_cell (`WaterBlock& b = water_[key2(cx >> 4, cy >> 4)];`): der Konverter von work-zusammen (fb51251f) schreibt
`Int b_row = 0;` und liest `self.water_[b_row]` – die Zeilen `key = key2(...); map_add(nX, self.water_, key); b_row = …` fehlen
(voll-100601 hatte sie). Lokal mit dem Konverter von origin/work-zusammen allein über src/terrain.cpp nachgestellt (p-tmp/w4/tz).
Folge: Terrain_set_water_cell 300/300, add_water_polygon, add_waterway, build_collision_tile weichen ab (beide Durchgänge).



## BEREIT ZUM MERGEN: 1e91e798 (E20 Schritt 3: List<U8> … List<I32>; Bytes-Knoten nehmen Listen jeder Breite) – Belege (15:40)
- Commits 18c636be, 0c57a404, 794faaa5, 6a00887f, 9dbb7749, 1e91e798 (Schritt 3 + Leser-Literal, place_type, Glue, nativer Erzeuger, Proben).
- Engine-Suite `work-p-engine-150118-30531` auf 1e91e798: engine rc=0, 54788 checks, 0 failures; blueprint_native_test ok (64 Blueprints, 15182 Aufrufe –
  darin fixed_width_lists und Nodes mit den Typen u8 … i32 / u8_list …); logic_fixed_widths ok.
- CI https://github.com/ChristianHohlfeld/kernengine.de/actions/runs/44539294064: native, build (JIT == Interpreter über alle logic_cases), determinism, input
  grün; e2e: blueprint 270 checks 0 failures (Typwahl +6), rot NUR an den 3 Stellen von work-zusammen (game_equipment_cpp/sound_fx_cpp „cannot convert
  int to int64“, „Play runs them as machine code“). native-spawncity lief noch.
- Kein konvert/vergleich nötig: Konverter und Spiel unberührt, kein Klassentext des Spiels ändert sich.
- Zeit-Test: **24 von 29 über 1,10 (f32-Spalte), 23 von 29 (64-Bit-Spalte)** – unverändert, nicht neu gemessen (Klassentexte gleich bis Schritt 4).
- Offen: Schritt 4 (Konverter) NICHT begonnen – Bauplan in uebergabe/p.md „Nachfolger 5“; Schritt 5, (b) danach. Nachfolger nötig (Kontext erschöpft).

## (überholt) E20 Schritt 3 gebaut: 0c57a404 – Belege liefen (13:50)
- Commits 18c636be (Schritt 3), 0c57a404 (Leser: List<Int>{…} in einem Eingang jeder Breite). Was und warum: uebergabe/p.md „Nachfolger 5“.
- Lokal: Fall fixed_width_lists – Klasse == Interpreter == echtes C++ (std::vector<u8>/<int8_t>/<uint32_t>); Fixpunkt aller logic_cases gleich;
  logic_fixed_widths (Leser-Fälle, Werte) grün.
- Belege: engine `work-p-engine-134721-25503`, CI https://github.com/ChristianHohlfeld/kernengine.de/actions/runs/44527163767.
- Konverter und Spiel unberührt (kein konvert/vergleich nötig; Klassentexte des Spiels ändern sich nicht) → Zeit-Test unverändert.

## BEREIT ZUM MERGEN: 62229851 (E20 Schritte 1+2: U8 … I32 – Sprache, Leser/Schreiber, Interpreter, JIT, Klasse, Editor, Doku) – Belege (13:25)
- Commits: 31d75912 (Schritt 1), 15c8ad52 (Schritt 2: Klasse logic::Sized<T>, Fall fixed_widths), 1d4cdd6e (Meldungserwartung Map),
  62229851 (e2e: Typwahl +6 Breiten).
- Engine-Suite `work-p-engine-121704-13222` auf 1d4cdd6e: engine rc=0, 54627 checks, 0 failures; blueprint_native_test ok (62 Blueprints,
  darin fixed_widths: Klasse == Interpreter); logic_fixed_widths ok.
- CI 44519073809 auf 62229851: native, build (Logic JIT == Interpreter über alle logic_cases, darin fixed_widths), determinism, input grün;
  e2e rot NUR an den 3 Stellen, die work-zusammen f0450061 genauso hat (game_equipment_cpp/sound_fx_cpp „cannot convert int to int64“,
  „Play runs them as machine code“) – nicht von p. native-spawncity lief noch.
- Lokal: Interpreter-Ergebnis von fixed_widths = Python-Modell der C++-Regeln, 316 von 316 Werten.
- Kein vergleich nötig: Konverter und Spiel unberührt (Schritt 4 macht u32-Quellen zu U32 – dort konvert+vergleich).
- Zeit-Test: 24 von 29 über 1,10 (f32-Spalte), 23 von 29 (64-Bit-Spalte) – unverändert und nicht neu gemessen: kein Klassentext ändert sich,
  solange der Konverter keine Breite schreibt (Schritt 4).

## E20 Schritt 1 gebaut: 31d75912 (Sprache, Leser/Schreiber, Interpreter) – Belege laufen (12:02)
- Breite am Platz wie Fixed: TypeRef::of neben integer ("u8"…"i32"), Text U8…I32; Draht = Int; Speichern wickelt (fitted, Add, hold);
  JIT erzeugt Set/Add fester Breite nicht selbst. × braucht keinen neuen Knoten (mul32). Lokal: logic_fixed_widths grün (bad=0).
- Belege: engine `work-p-engine-120146-7563`, CI https://github.com/ChristianHohlfeld/kernengine.de/actions/runs/44511064433 (JIT).
- Nächst: Schritt 2 Klasse (U8…I32 in nodes.h/blueprint.h), logic_cases, Editor, Doku.

## 11:45: 1831e165 gemergt (work-zusammen 0cb3f6b6); (b) und E20 entschieden – Bauplan in uebergabe/p.md, Nachfolger nötig (Kontext erschöpft)

## (gemergt) BEREIT ZUM MERGEN: 1831e165 (Punkt 2, Int-Formeln ganz) – Belege (11:35)
- Engine-Suite `work-p-engine-095126-25407` auf 1831e165: engine rc=0, 54610 checks, 0 failures; blueprint_native_test ok (61 Blueprints, 14102 Aufrufe –
  darin formula_int_sums), game_native_test ok.
- vergleich `work-p-vergleich-103059-7878` (src/qr_code.cpp src/nostr.cpp src/settings.cpp src/mvt.cpp, Grundlage 070328): 11 ok von 11 in beiden Durchgängen,
  keine Abweichung; cost-Zeilen nur solche, die schon die Grundlage hat (bind_settings, find_opt, opt_spec, Settings_*, Writer_feature, encode, event_json,
  lift_x – gleiche Zahlen), keine neue.
- Zeit-Test: 24 von 29 über 1,10 (f32-Spalte), 23 von 29 (64-Bit-Spalte).

## Punkt 2 (Int-Formeln ganz) gebaut: 1831e165 (auf work-zusammen e32ba575)
- Formula::int_input(k) / whole_result() / result_sum() (formula.cpp, eine Regel am Ausdruck); Schreiber/Leser je Eingang Int oder Float, -> Int nur
  wo alle Int und whole_result (bei +,−,× an der Wurzel: jeder Leser Int); Klasse: Whole(Sum). Engine-Fall formula_int_sums; Leser-Fälle in test_main;
  Doku blueprint_cpp.md „Formeln“. Lokal: Fixpunkt über alle logic_cases gleich, Leser-Fälle wie erwartet, Lambdas gegen Formula::evaluate bitgleich.
- Zeit-Test ($S/p/p2_1.txt, Klassen neu, Level/Tests 055004, gleich/gleich64 29/29 „=“): **24 von 29 über 1,10 (f32-Spalte), 23 von 29 (64-Bit-Spalte)**
  (vorher 25/23). min/min64: qr_encode 4,96→3,36/2,60, gf_mul 3,46→2,82/2,81, rs 5,09→3,30/2,58, get_u32 3,89→2,97/2,69, read_geometry 2,51→2,03/1,18,
  ring_area 3,44→3,21/1,17, from_hex 5,09→4,79/4,75, sha256 6,64→6,14/6,49, mulm 6,08→6,20/3,49 (Rauschen).

## Punkt 3 (getypter Einstieg func_call_on) – NICHT gebaut, Befund (10:00)
Die cost-Zeilen (GrenadeCook_pull u. a. in controls_cpp, Fun_multiplier … in combat_rules_cpp) kommen aus Struktur-Argumenten/-Ergebnissen. Ziel ist
der Blueprint des Level-Actors „Controls“ (actors[7]) mit EIGENEN Typen: struct Cook {Carry, RegenAt, …}, enum DeviceEvent – der Aufrufer hat
controls_Cook {carry, regen_at, …}, controls_Devices_Event. Die C++-Signaturen sind nie gleich; ein Einstieg „wo die Signatur passt“ greift genau
bei diesen Aufrufen nicht (bei Float/Int/Bool-Aufrufen gibt es heute schon keine Anforderung). Die Bedeutung heute = Umwandlung über Value nach
Stelle (Held::from: gleiche Feldzahl und je Feld is_value, sonst Vorgabe; Enum nach Index; Ergebnis über from_outside des Aufrufers).
Was es bräuchte (Entscheidung): (a) Einstieg mit „Gestalt“ statt Typ – Schlüssel = Gestalt der Signatur (Struktur → Tupel der Feld-Gestalten nach
Stelle über structs::fields, Enum → Index samt Zahl der Werte), Aufrufer wandelt in die Gestalt, Ziel daraus in seine Typen; jede Regel der
Value-Umwandlung (Feldtypen, Enum außerhalb, from_outside) muss dort Bit für Bit nachgebaut und am Rand bewiesen werden; oder (b) Value hält eine
Struktur beim Aufruf ohne Heap (Felder in einem std::array der Aufrufstelle, „geliehen“; Ergebnis-Plätze vorgeformt nach den erklärten Ausgängen,
das Ziel schreibt Feld für Feld hinein) – Bedeutung unverändert, weil derselbe Weg über Value läuft, nur ohne make_shared/vector.
Empfehlung: (b) – kleiner, dieselbe Umwandlung, wirkt auch für Interpreter/JIT-Ziele.

## BEREIT ZUM MERGEN: 9c315bf2 (50e21923 Text::Appending + 9c315bf2 E17 Zeit-Test) – Belege (09:40)
- Engine-Suite `work-p-engine-075125-9242` auf 50e21923: engine rc=0, blueprint_native_test ok (60 Blueprints, 14102 Aufrufe – darin append_in_place),
  game_native_test ok.
- vergleich `work-p-vergleich-092919-18986` (src/settings.cpp, Kopf 9c315bf2, Grundlage 070328): 6 ok von 6 in beiden Durchgängen, keine Abweichung;
  cost-Zeilen dieselben wie vor der Änderung (work-p-vergleich-230429-15316: bind_settings, find_opt, opt_spec, Settings_new/reset_section/
  schema_json/set) – keine neue.
- E17 berührt nur die Bench (tests/bench, tools/blueprint_bench.sh); Beleg ist die Messung $S/p/e17_1.txt (29/29 gleich64).

Zweig work-p, Kopf **50e21923** (gepusht; work-zusammen 821208ac eingemischt). Messung: Level von `work-zusammen-voll-055004-6649`
($S/p/lvl2), `tools/blueprint_bench.sh … --check 1.10 15` (beide Sperren), Rohdaten $S/p/heute.txt (vorher), $S/p/nach_text.txt (Kopf).
Schnell-Neubau ohne neue Klassen: $S/p/rb.sh <runden> [fall…] (CL=Ordner mit classes.h, RD=Bauordner, XF=-g).

## ENTWURF 4 (für Chris, Bit-Fälle) – 19.11. 08:10. Zahlen: Bestzeiten Klasse/C++ (f32-Spalte; 64-Bit-Spalte noch nicht gebaut)
Projektionen = Messungen eines veränderten Klassentexts/Headers (nur zum Messen): B = alle 2^53-Prüfungen weg, P = Punkt 2, W = Wächter aus.
**(a) Breite an Variable/Feld/Parameter.** Weg 1 – feste Breiten als Typen (u8 u16 u32 i8 i16 i32; Speichern wickelt wie C++): Interpreter/JIT
bleiben exakt in double, weil jeder Wert ≤ 2^32 ist; nur × muss als Knoten ganzzahlig rechnen (2^32·2^32 ist in double nicht exakt; heute mul32).
Neu: Pin-/Variablen-Typen, Wandlungsregeln, Leser/Schreiber, Editor-Farben, check(). Der Konverter muss nicht mehr nach jeder Operation
wrap32/band schreiben (Fehlerklasse G7 entfällt), u32-Quelle wird u32-Blueprint. „Kein Float32, Zahlen 64 Bit“ bleibt unberührt (ganze Zahlen,
kein neuer Gleitkommatyp). Weg 2 – Bereichsangabe am Int (`Int [0, 2^32)`): Bedeutung wie heute plus eine Behauptung, die jede Stufe beim
Speichern prüfen (oder wickeln?) müsste – entweder neue Bedeutung (wickeln = Weg 1 in anderer Schreibweise) oder neue Fehlerart (verletzt);
der Konverter schreibt weiter wrap32/band. Projektion (B bzw. P+B, Obergrenze für den Wegfall der Prüfungen, nicht für echte u32-Operationen):
rng_next 1,65 → 0,98; rng_unit 1,79 → 1,08; sha256 6,5 → 3,0; qr_encode 5,0 → 2,3; gf_mul 3,4 → 1,3; rs_remainder 5,1 → 1,6; mulm 6,6 → 5,4;
from_hex 5,1 → 4,1; wire_get 3,9 → 2,4. Echte u32-Operationen ohne wrap32/band (Weg 1) dürften weiter sinken – nicht gemessen.
Empfehlung: Weg 1 (feste Breiten), weil er dieselbe Gestalt hat wie die Quelle und G7 an der Wurzel beseitigt.
**(b) Schleifenwächter.** Ohne Textänderung geht es nicht: `while (loop(c))`/`flow_for` haben keinen Platz für einen Zähler im Register, und ein
Aufruf im Rumpf liest denselben Zähler (gleicher Durchlauf in jeder Stufe). Was der Text sagen müsste: eine Schleife ohne Aufruf im Rumpf
(am Graphen feststellbar) als `for (auto it = flow_for_local(n, a, b); …)`, das den Rest des Budgets beim Eintritt liest, lokal herunterzählt und
beim Verlassen zurückschreibt – dieselbe Stelle des Abbruchs. Gemessen W (aus): wrap_pi 1,50 → 0,85, mulm 6,6 → 5,6, gf_mul (mit P) 2,9 → 1,8,
ring_area (mit P) 2,8 → 2,5; sonst < 5 %. Empfehlung: bauen, aber erst nach (a), weil es mit (a) zusammen erst trägt.
**(c) Bytes-Typ.** Heute List<Int>: 8 Byte je Byte, je Byte eine Prüfung und Whole. Ein Typ `Bytes` (u8-Einträge, Lesen außerhalb gibt 0 wie heute
byte_at) – mit (a) ist das List<u8>, kein eigener Typ nötig. Projektion für wire_*/mvt: nicht gemessen (wire_put 1,5 und mvt_read 2,5 hängen auch an
Anhängen je Element). Empfehlung: als List<u8> aus (a), kein eigener Typ.
**Wie viele der 29 danach unter 1,10 (geschätzt, die 64-Bit-Spalte fehlt noch):** heute 4. Mit E17 vermutlich + die 7 Float-Fälle (in W/P/B
unverändert – ob sie gegen die 64-Bit-Quelle ≤ 1,10 liegen, zeigt erst die Spalte). Mit (a)+(b)+Punkt 2 zusätzlich wahrscheinlich rng_next,
rng_unit, wrap_pi, gf_mul (≈ 14). Trotzdem darüber nach den Projektionen: sha256 (3,0), qr_encode (2,3), rs_remainder (1,6), mulm (5,4),
from_hex (4,1), wire_get/put, mvt_read/ring_area, settings_escape (3,4: Formeln je Zeichen), lamps_near, lamps_grid_key, settings_num –
diese brauchen je eigene Ursachenarbeit (Int64-Knoten, Text je Zeichen, Map), was die Projektionen nicht abdecken.

## Fälle, die auch nach (a)+(b)+Punkt 2 über 1,10 blieben – Ursache nach heutiger Kenntnis (08:20)
- sha256 (P+B 3,0): rotr als Funktionsaufruf mit Result-Struktur, Indizes `4.0 * index` u. ä. als Float-Formel (von P nicht erfasst), Fixed<Int,64>
  mit Prüfung je Zugriff, `w = List<Int>{}` je Block; Summen über Sum mit Prüfung je Stufe. Klasse (Formeln mit ×, Einbetten) – teils behebbar; Rest weiß ich noch nicht.
- qr_encode (2,3) / rs_remainder (1,6): Listen mit erase(0)+push_back wie die Quelle, Vergleich je Durchlauf, gf_mul nicht abgerollt (Wächter).
  Mit (a)+(b) vermutlich nahe; Rest: weiß ich noch nicht.
- mulm (5,4): Int64-Knoten (int64_add/of_int/low) je Schritt, `i - 1.0`; die Int64-Umsetzung habe ich nicht gelesen – weiß ich noch nicht.
- from_hex (4,1): je Zeichen `a >= 128 ? a - 256 : a` und `2*index` als Float-Formel, from_hex_v als Aufruf mit Result; Punkt 2 mit Vergleich
  und × an der Wurzel sollte viel davon nehmen (Klasse/Schreibweise, allgemein); Rest weiß ich noch nicht.
- wire_get/put_u32, put_f32 (1,5–3,9): Bytes als List<Int> (Aufbau der Sprache, (c)), Anhängen je Element statt eines Bereichs (Konverter/Quelle).
- mvt_read_geometry (1,7) / ring_area (1,66): Bytes als List<Int>; ring_area: Listenprüfung je ring[i], whole_add für `i += 1`. Behebbar teils mit
  E2.3 (Index am Graphen bewiesen, zurückgestellt) – sonst Aufbau.
- settings_escape (3,4): Float-Formeln je Zeichen (`a >= 128 ? …`, Vergleich mit 34/92/32), text_of_code als Text je Zeichen. Punkt 2 + Klasse.
- lamps_near (1,8–4,2, Rauschen groß): Map/Liste – nicht untersucht, weiß ich noch nicht.
- lamps_grid_key (1,4): Float → Int64 (Wandlung mit Prüfung) – nicht untersucht.
- settings_num (1,3): text_format<"%g"> gegen snprintf in einen Puffer – Klasse (eigener Formatweg), nicht untersucht.

## E17 gebaut: 9c315bf2 (lokal gemessen, Beleg = dieser Lauf; engine auf mag folgt mit dem nächsten Auftrag)
Messung $S/p/e17_1.txt (Level+Tests 055004, Kopf 9c315bf2): gleich64 29/29 „=“. **Über 1,10: 25 von 29 gegen die Quelle (f32), 23 von 29 gegen
die Quelle in 64 Bit.** min/min64: qr_encode 4,96/3,57, gf_mul 3,46/3,39, rs 5,09/3,94, sha256 6,64/7,06, mulm 6,08/3,45, from_hex 5,09/5,01,
rng_next 1,67/1,63, tiles 1,03/1,03, grid_key 1,37/1,04, zigzag 1,00/1,00, read_geometry 2,51/1,44, put_u32 1,53/1,54, get_u32 3,89/3,57,
escape 3,30/3,33, num 1,28/1,29, wrap_pi 1,48/1,67, damp 1,04/1,00, qnormalize 1,20/1,26, slerp 1,17/1,01, to_local 1,32/1,16, rng_unit 1,82/1,90,
person_hit 1,46/1,28, sphere 0,97/1,00, put_f32 1,54/1,52, ring_area 3,44/1,27, governor 1,46/1,42, audio 1,40/1,27, place_road 1,41/1,28, near 4,55/3,38.
Achtung: die 64-Bit-Quelle ist die Kopie des Tests samt converted::at (Prüfung, wo C++ es offenlässt) – bei qr/mulm/mvt ist sie dadurch langsamer,
nicht die Klasse schneller. BEFUND FÜR t1: die Kopie einer Klasse im Test hält nur, was die geschriebenen Funktionen nutzen (Field::all,
Governor::tier/fps fehlen) – place_road/perf_governor lesen ihre Prüfsumme darum auf allen Seiten über near/update.

## Gebaut (UNBELEGT, Beleg: engine + vergleich settings auf mag folgt)
- 50e21923 Text::Appending: `O = text_append(std::move(O), x, y)` hängt an Ort und Stelle an (ein Zeichen = push_back); Text of Code ohne
  Umweg. Text der Klasse unverändert. settings_escape 7,59 → 3,37. g++ -fsyntax-only der Bench-Klassen ok.

## Zeit-Test heute (Bestzeiten Klasse/C++; „vorher“ = 821208ac, „Kopf“ = 50e21923) – 25 von 29 über 1,10 (vorher 25)
Versuchsspalten (nur Messung, nicht gebaut; je gegen „vorher“): W = Schleifenwächter aus; P = Projektion von Punkt 2 (Int ± Zahl und Vergleich
mit Vorzeichen ganzzahlig, per Ersetzung im Klassentext, Prüfsummen gleich); P+W; B = alle Bereichsprüfungen 2^53 weg (Obergrenze, falsche
Bedeutung – nur, um den Anteil zu sehen); P+B.
| Fall | vorher | Kopf | W | P | P+W | B | P+B | Ursache (Hauptanteil zuerst) | nächster Schritt |
|---|---|---|---|---|---|---|---|---|---|
| qr_encode | 5,00 | 4,97 | 4,77 | 3,27 | 3,24 | 4,28 | 2,33 | Int-Arithmetik als Float-Formel (i±1, Vergleiche), Bereichsprüfungen, Listen | Punkt 2, dann Bereich (s. u.) |
| qr_gf_mul | 3,43 | 3,47 | 3,29 | 2,93 | 1,79 | 3,65 | 1,33 | `i - 1.0` als Float-Kette (14 Takte je Durchlauf), Wächter, Prüfungen; C++ rollt die 8 Durchläufe ab | Punkt 2 + Wächter |
| qr_rs_remainder | 5,08 | 5,04 | 4,87 | 3,28 | 2,82 | 4,23 | 1,59 | wie gf_mul + Vergleich `(i<0)!=(n<0)…` je Durchlauf | Punkt 2 |
| nostr_sha256 | 6,51 | 6,52 | 6,18 | 5,84 | 6,24 | 3,82 | 3,04 | Bereichsprüfungen je Bit-Funktion/Summe; Indizes `index - 15.0` als Float; rotr als Aufruf | Bereich an der Variable (Sprache), Punkt 2 |
| nostr_mulm | 6,55 | 8,56 | 5,56 | 7,15 | 5,59 | 5,40 | 6,48 | Int64-Knoten, `i - 1.0`, Wächter, Fixed-Zugriff (schwankt stark 6,6–8,6) | Punkt 2 + Int64 lesen |
| nostr_from_hex | 5,12 | 5,14 | 4,89 | 5,20 | 4,98 | 4,07 | 4,55 | Text je Zeichen über Float-Formeln (`a>=128?a-256:a`, `2*index`), from_hex_v als Aufruf | Punkt 2 (Wurzel ×, Vergleich) |
| common_rng_next | 1,64 | 1,65 | 1,68 | 1,70 | 1,65 | 0,99 | 0,98 | nur die Bereichsprüfungen (bits_of/normal) in der Kette | Bereich an der Variable (Sprache) |
| common_rng_unit | 1,80 | 1,79 | 1,77 | 1,81 | 1,80 | 1,08 | 1,08 | wie rng_next; dazu `n10 = Rng_next(self)` als Mitglied (BEFUND FÜR e) | Bereich; e |
| wire_get_u32 | 3,91 | 3,93 | 3,87 | 3,00 | 3,04 | 3,98 | 2,40 | Vergleich/`at + 4.0` als Float-Formel, Bytes als List<Int> (byte_at je Byte geprüft) | Punkt 2; Bytes |
| wire_put_u32 / _f32 | 1,52/1,51 | 1,51/1,54 | ~1,5 | ~1,5 | ~1,5 | ~1,5 | ~1,5 | Bytes als List<Int> (8 Byte je Byte, Anhängen je Element) | Bytes (eigener Entwurf nötig) |
| mvt_read_geometry | 2,53 | 2,45 | 2,30 | 1,98 | 1,74 | 2,48 | 1,70 | Bytes als List<Int>, Float-Formeln | Punkt 2; Bytes |
| mvt_ring_area | 3,44 | 3,46 | 3,17 | 2,80 | 2,50 | 2,35 | 1,66 | Vergleich als Float-Formel, Wächter, `i += 1` (whole_add), Listenprüfung | Punkt 2 + Wächter |
| settings_escape | 7,59 | **3,37** | 7,38 | 7,94 | 7,76 | – | – | (behoben: Anhängen) Rest: Float-Formeln je Zeichen | Punkt 2 |
| lamps_near | 2,64 | 2,61 | 2,22 | 2,01 | 2,60 | 2,56 | 2,36 | Map/Liste, schwankt 1,8–4,2 zwischen Läufen | erst Rauschen klären |
| lamps_grid_key | 1,37 | 1,41 | 1,37 | 1,41 | 1,37 | 1,42 | 1,42 | Float→Int64 (Wandlung mit Prüfung) | Int64-Wandlung lesen |
| settings_num | 1,30 | 1,28 | 1,29 | 1,24 | 1,26 | 1,24 | 1,25 | text_format<"%g"> gegen snprintf | lesen |
| common_wrap_pi | 1,50 | 1,24 | **0,85** | 1,29 | 0,88 | 1,33 | 1,55 | Wächter (Float gegen f32) | Wächter |
| Float gegen f32-Quelle: qnormalize 1,20, slerp 1,19, to_local 1,34, person_hit 1,43, perf_governor 1,43, audio_render 1,38, lamps_place_road 1,23 | | | ≈ | ≈ | ≈ | ≈ | ≈ | 64 Bit (Entscheidung 1) gegen das f32 der Quelle; kein Klassen-Anteil messbar | **Entscheidung: Maßstab** |
| unter 1,10: tiles_key_hash 1,04, mvt_zigzag 1,01, damp_alpha 1,03, person_sphere 0,98 | | | | | | | | | |
Ohne Wirkung gemessen (verworfen): formula immer einbetten (always_inline), seltene Wege von whole/normal/bits_of/whole_add ausgelagert
(cold/noinline), Platz-neben-der-Liste ausgelagert, bits_of(Int) ohne Umweg (rng 1,65 → 1,89: cmov in der Kette).

## Ehrlich: was mit dem heutigen Aufbau grundsätzlich nicht unter 1,10 kommt
1. **Die 7 Float-Fälle (1,19–1,46)**: die Quelle rechnet f32, die Klasse 64 Bit (Entscheidung 1, nicht verhandelbar). Kein Klassen-Anteil
   messbar (W, P, B ändern nichts). Unter 1,10 nur, wenn der Maßstab die Quelle in den Zahlen der Sprache ist (wie der Vergleichstest sie
   übersetzt: f32 = double, kern::math) → **Entscheidung nötig** (Koordinator/Chris), dann misst die Bench beide Spalten.
2. **Bit-Fälle (sha256, rng, qr, mulm)**: selbst mit Punkt 2 UND ohne jede Bereichsprüfung bleiben sha256 3,0, qr 2,3, mulm 5,4. Jede
   Bit-Funktion und jede Summe muss heute die 2^53-Grenze prüfen (Bedeutung des Interpreters). Wo der Wert aus derselben Formel kommt
   (wrap32 …), faltet der Compiler die Prüfung schon weg; was bleibt, sind Prüfungen an Werten aus Variablen/Feldern (Rng.s, a…k in sha256),
   deren Bereich der Klasse unbekannt ist (Int ist i64). Nötig: (a) Punkt 2; (b) ein Bereich an der Variable – ein 32-Bit-Typ der Sprache oder
   eine Bereichsangabe (Schreibweise und Sprache, nur mit Entscheidung Chris/Koordinator); (c) der Wächter je Durchlauf (Zähler im Speicher)
   verhindert Abrollen/Register: ohne Textänderung nicht wegzubekommen, weil ein Aufruf im Rumpf denselben Zähler liest.
3. **Bytes als List<Int>** (wire_*, mvt_read_geometry 1,5–2,5): 8 Byte je Byte und Prüfung je Byte; unter 1,10 nur mit einem Bytes-Typ der
   Sprache (Entwurf möglich, groß).

## BEFUND FÜR e
- `n10 = Rng_next(self);` in Rng_unit (und alle `nNN = f(...)`): der Wert eines Schritts als Mitglied der Klasse = Speicher-Schreiben je Aufruf
  und kein Halten im Register; rng_unit liegt 0,15 über rng_next. Nach E16 neu messen.

# Stand Paket p (Leistung der exportierten Klasse) – 18.11.2026, 23:05

Zweig work-p, letzter Commit 85d60a1d (gepusht; CI 44416542751 rot an logic_fixed_lists – E10-Leser lehnte Fixed an einem Eingang ohne Länge mit anderem Text ab, behoben; aede1c3e: format_arg(const char*) – CI 44407872390 war rot: g++ fand text_format("abcdef") mehrdeutig nach 25a7318f; behoben, alle 43 Engine-Fälle als Klasse mit g++ übersetzt; dazu Switch on Text ohne Liste, alt/neu lokal gleich über 121 Paare).
- BEWIESEN: E2.1 Schritt 1 (bis 2416329): mag `work-p-engine-221509-17831` engine rc=0 (197 ok, blueprint_native_test 55 Blueprints/14102 Aufrufe,
  game_native_test ok). Probe `work-p-probe-221512-17868` (alle Quelltexte, vor der 22:25-Regel) abgebrochen – überholt durch den Kopf.
- LÄUFT: `work-p-vergleich-230429-15316` (gestartet 23:14 auf 8a028f9c: E2.1, E2.2, E2.1 Schritt 2, Fix text_format, E3-A) über qr_code, settings,
  nostr, controls, combat_rules. NICHT darin: E10 + Leser-Fix (Commit vor 6c7b9c5f). Danach `engine` auf dem Kopf. CI: run 44407872390 (work-p, 63467c0b).

## BEREIT ZUM MERGEN: 85d60a1d (Paket p, Teil 2) – Belege (00:10)
- **Engine-Suite** `work-p-engine-002417-11420` auf 85d60a1d: engine rc=0, 197 ok, blueprint_native_test ok (60 Blueprints, 14102 Aufrufe – mit den neuen
  Fällen formula_whole_arith, formula_whole_indexed, format_text_of_value, fixed_literal_input, append_in_place), game_native_test ok.
- **vergleich** `work-p-vergleich-230429-15316` (8a028f9c): 12/12 ok in beiden Durchgängen, keine Abweichung (Abschnitt unten).
- **CI** 44420463362 auf 85d60a1d: build, determinism, input, e2e, native (ASan/UBSan) grün; native-spawncity lief bei Abgabe noch.
- Inhalt: E2.1 (95ada08, 2416329), E2.2 (e7538f1), Zeit-Test (133bb74), E2.1 Schritt 2 +,−,× (c064f234), **Fix text_format = 25a7318f + aede1c3e
  (dringend für k: danach 12e3da24 zurücknehmen)**, E3-A Aufrufweg, E10 Fixed-Literal am Eingang + Leser-Fix Int-Formel (vor 6c7b9c5f, 85d60a1d),
  Switch on Text ohne Liste. Text der Klassen ändert sich nur durch E2.1/E2.2/E2.1-Schritt-2/E10 (genehmigt); Fixpunkt in den Fällen gleich.
- **vergleich** `work-p-vergleich-010659-18840` auf 85d60a1d (drone, controls, combat_rules): 14/14 ok in beiden Durchgängen, keine Abweichung;
  **keine cost-Zeile mehr für Arondight45_*** (vorher Runtime_reset/new, FirmwareRuntime_reset/new) → Wirkung von E10 belegt. cost gesamt 29/27.
- CI native-spawncity: bei 45 min abgebrochen (Zeitgrenze des Jobs; auf `work` 44363208469 ebenso rot) – nicht von p.

## Beleg vergleich `work-p-vergleich-230429-15316` (8a028f9c; 23:29)
convert rc=0, bau rc=0, alle rc=0: **12 ok, 0 FAIL, 0 ABBRUCH, 0 LECK, 0 ZEIT von 12 – in beiden Durchgängen (Stellvertreter und SC_ENGINE=real)**,
alle-abweichungen.txt und alle-real-abweichungen.txt leer. Tests: blueprint_call, combat_rules_cpp, common, controls, controls_cpp, engine_math_math,
engine_physics_physics, nostr_cpp, qr_code, qr_code_cpp, settings, settings_cpp. → E2.1, E2.2, E2.1 Schritt 2 Bit für Bit gleich.
E3-A Wirkung: cost-Zeilen controls_cpp 44 → 10, combat_rules_cpp 69 → 11 (vorher: $S/t1/suite203000_cost_note.txt). Was bleibt, sind Aufrufe mit
Struktur-Argumenten/-Ergebnissen (z. B. WorldLook_add: controls_Look als logic::Value – Felder im Vektor des Value), nicht der Aufrufweg.
Fix text_format: hier NICHT belegt – der Stand enthält ks Umgehung 12e3da24 (Text in eigener Variable); belegt lokal mit ASan (alter Header UAF,
neuer sauber) und durch den Engine-Fall format_text_of_value. Laufend: `work-p-engine-002417-11420` (engine auf 85d60a1d; ältere abgebrochen), CI 44420463362.

## E10 (Fixed-Literal am Eingang, genehmigt 23:2x) + Leser-Fix E2.1 – gebaut (Commit vor 6c7b9c5f), lokal geprüft, UNBEWIESEN
- PinDef::count (typed_pin, Funktions-Pins); Schreiber literal_of mit TypeRef → `struct_make<fc_Filters>(Fixed<fc_PT1, 3>{…}, …)`; Leser lehnt falsche Länge ab.
- **Fehler in E2.1 gefunden und behoben:** Leser zählte bei `[](Int …) -> Int` alle beim Lesen der Argumente entstandenen Drähte – ein Argument
  `In[kern::to<Int>(…)]` macht eigene → to_cpp-Text wurde abgelehnt (Arondight45_DroneFC_Core). Jetzt nur Drähte in die Formel. Engine-Fall formula_whole_indexed.
- Beleg offen: vergleich über src/firmware?… (Arondight45: cost-Zeilen Runtime_reset/new) + engine auf dem Kopf.

## E3-A Aufrufweg ohne std::vector – gebaut 2b?/8a028f9c, UNBEWIESEN
- func_call_on: std::array für Argumente/Ergebnisse; Runtime::call_on(span,span), call_on_into, Instance::call_into/call_class; logic::Results
  (Vektor oder Plätze) als Ziel von NativeBlueprint::call; Glue-Signatur angepasst (nicht Teil der Schreibweise). Lokal: übersetzt (Werkzeug,
  Glue des Falls call_on, Bench-Klassen). Beweis offen: engine auf dem Kopf + vergleich über controls.cpp, combat_rules.cpp (cost-Zeilen).

## Fix text_format (Befund von k) – 25a7318f, lokal bewiesen, mag läuft
- blueprint.h `format_arg(const std::string&)`/`(std::string_view)`: Sicht auf den Wert selbst (vorher vorläufiger Text, der vor dem Schreiben verging).
  Engine-Fall `format_text_of_value`. Lokal mit ASan (g++): alter Header heap-use-after-free, neuer sauber; Ausgabe = Interpreter; Fixpunkt gleich.
  Text der Klasse unverändert. Beleg auf mag: vergleich oben (combat_rules.cpp) + engine auf dem Kopf.

## E2.1 Schritt 2 (+, −, ×) – gebaut c064f234, lokal geprüft, auf mag UNBEWIESEN
- formula.cpp `kWholeOperators` (+, −, × neben der Funktionstabelle); `Formula::whole`: ein Eingang darf auch Operand von +,−,× sein, deren Wert als
  ganze Zahl gelesen wird; diese bekommen nur Eingänge, Zahlen, ganzzahlige Funktionen, einander. Wurzel bleibt ganzzahlige Funktion.
- nodes.h `logic::Sum<Op,A,B>` (faul): als f64 genau die doubles der Formel; an band…mul32: i64, wo jede Zahl/jedes Zwischenergebnis ganz und ≤ 2^53, sonst
  bits_of des doubles. Operatoren nur, wo eine Seite Int oder Sum ist (Float-Formeln unberührt; Exact⊕Exact bleibt double).
- Lokal ($S/p/chk, runcase.cpp, chk3, chk2): Fall formula_whole_arith Klasse == Interpreter (md5 gleich, 120 kB), Fixpunkt gleich (auch whole_ints,
  bits_edges, append_in_place), 10 Lesefehler-Tests richtig. classes.h aller Bench-Klassen übersetzt.
- Messung ($S/p/nach_e21s2.txt, Bestzeiten): sha256 12,80 → 6,62; rng_next 2,18 → 1,66; rng_unit 1,75 → 1,81 (Rauschen); sonst unverändert.
  mulm (8×) und ring_area (3,4×) hängen NICHT an +,−,× in Bit-Formeln: mulm = Int64-Knoten (int64_add/of_int/low), `i - 1.0` als Float-Formel an der
  Wurzel, Schleifenwächter, Fixed-Zugriff; ring_area = Wächter, Listenprüfung, `i += 1` über whole_add, Vergleich `(i<0)!=(n<0)?…` als Float-Formel.
  → Block D bzw. ein weiterer Schreibweise-Schritt (Wurzel +,−,× / Vergleich mit Int-Ergebnis – bräuchte Bedingung „Ausgang geht nur in Int“ am Graphen).

## BEREIT ZUM MERGEN (Teil 1: Messwerkzeug + 3a–c) – Stand 7aacebfb
- Commits: f50c07d (Messwerkzeug), 16c059c (math_compare/bits_of/whole/To<Whole>/Exact), 7aacebf (Engine-Fall formula_bits_edges); Merge-Basis 8874e79.
- Beweis: mag `work-p-engine-212302-7164` engine auf 7aacebf: rc=0 (197 ok, blueprint_native_test ok: 54 Blueprints, 14102 Aufrufe; game_native_test ok).
  Lokal $S/p/eq.cpp: 1.737.120 Vergleiche alt/neu, 0 verschieden. Text der Klassen unverändert (nur Header), daher keine Spiel-Suite nötig außer dem vollen Lauf.
- Messung: Tabelle „Früher“ unten (qr encode 28× → 14×, rng 21× → 13×, sha256 22× → 14×; Float-Fälle unverändert).
(Der Stand danach – E2.1/E2.2/Zeit-Test bis 133bb74 – wartet auf work-p-engine-221509-17831 / work-p-probe-221512-17868 und einen `vergleich`.)

## Nach Nachtrag 21:50
- **E2.1** Formel ganzer Zahlen: `whole` steht in der Funktions-Tabelle (formula.cpp `Fn::whole`, band…mul32), `Formula::whole()` (Wurzel eine solche
  Funktion, jeder Eingang nur Argument einer solchen). Schreiber: `[](Int a) -> Int` + Ints ohne kern::to, wenn jeder Eingang ein Draht von einem Int ist.
  Leser: Int-Form nur bei Formula::whole, nur Drähte von Ints, kein kern::to, keine geschriebene Zahl (7 Lesefehler-Tests in test_main.cpp, lokal
  geprüft: alle abgelehnt mit dem erwarteten Text). Klasse: Int → Bits über `bits_of(Whole)` = bits_of(Exact); `-> Int` = Whole(Exact) = normal;
  `kern::formula` gibt bei Int-Lambda `Exact`. Engine-Fall `formula_whole_ints` (23 Int-Randwerte inkl. ±(2^63−1), 2^62, Schiebeweiten 31..65; W als Int,
  R als 64 Bit); Fixpunkt to_cpp→from_cpp→to_cpp lokal gleich (und im Suite-Test logic_notation_tier_cases für jeden Fall). Doku blueprint_cpp.md „Formeln“.
  +, −, × (zweiter Schritt) noch nicht gebaut.
- **E2.2** `O = text_append(std::move(O), x, "")`: `NodeDef::takes_first` (beim Knoten Append), Regel `taken_first` in notation.cpp (ein Leser, der Set derselben
  Variable als Ganzes; die gelesene var.get nur von diesem Wert gelesen). Engine-Fall `append_in_place` (auch T + T), Fixpunkt lokal gleich, Doku.
- **Zeit-Test** `tools/blueprint_bench.sh <level> --check 1.10`: Bestzeiten im Wechsel. Drei Läufe desselben Programms: Spanne des Verhältnisses der
  Bestzeiten im Mittel 1,7 %, höchstens 6,0 %; des Medians im Mittel 2,8 %, höchstens 15 % → Bestzeiten, Grenze 1,10 (6 % + Rand). Heute 25 von 29
  Fällen darüber (rc=1) – läuft als Bericht, nicht in der CI.
- **E3-C (Firmware Arondight45_*):** `FirmwareRuntime_new` legt `fc_Filters{List<fc_PT1>{…3 Einträge…}, …}` an – die Felder sind Listen variabler Länge
  (std::vector, Speicher) wo die Quelle feste Arrays hat; die Sprache hat Listen fester Länge (`Fixed<T, N>`, blueprint_cpp.md „Plätze“). Ursache:
  wie der Konverter die Felder typt (Paket Konverter), nicht die Klasse. BEFUND FÜR den Konverter.
- **E3-A Plan** (noch nicht gebaut): func_call_on baut `std::vector<Value>` für Argumente und Ergebnisse, Runtime::call_on dazu `results` (from_json) und
  `got`; Instance::call füllt `got`. Null Anforderungen: Argumente als `std::span<const Value>` aus einem `std::array` auf dem Stapel, Ergebnisse in einen
  Puffer fester Größe (die Zahl der Ausgänge ist bekannt), Runtime::call_on/Instance::call auf Span umstellen (alle Aufrufer in logic.cpp mit).

## Messung nach Block B/C (Bestzeiten, Verhältnis Klasse/C++; $S/p/nach_e21s2.txt, nach_c.txt) – Prüfsummen alle gleich
| Fall | nach E2.1+E2.2 (min) | nach B (c064f234) | Kopf a53ce271 |
|---|---|---|---|
| qr_encode | 4.99 | 4.94 | 4.94 |
| qr_gf_mul | 3.46 | 3.45 | 3.45 |
| qr_rs_remainder | 5.10 | 5.03 | 5.01 |
| nostr_sha256 | 12.80 | 6.62 | 6.71 |
| nostr_mulm | 6.56 | 8.21 | 7.84 |
| nostr_from_hex | 6.04 | 5.09 | 5.10 |
| common_rng_next | 2.18 | 1.66 | 1.67 |
| tiles_key_hash | 1.02 | 1.06 | 1.03 |
| lamps_grid_key | 1.38 | 1.37 | 1.27 |
| mvt_zigzag | 1.00 | 1.00 | 1.00 |
| mvt_read_geometry | 2.56 | 2.52 | 2.52 |
| wire_put_u32 | 1.53 | 1.53 | 1.52 |
| wire_get_u32 | 3.91 | 3.95 | 3.90 |
| settings_escape | 7.56 | 6.96 | 7.01 |
| settings_num | 1.26 | 1.29 | 1.29 |
| common_wrap_pi | 1.23 | 1.82 | 1.88 |
| common_damp_alpha | 1.03 | 1.05 | 1.05 |
| common_qnormalize | 1.19 | 1.20 | 1.20 |
| common_slerp | 1.20 | 1.19 | 1.19 |
| common_to_local | 1.33 | 1.32 | 1.33 |
| common_rng_unit | 1.75 | 1.81 | 1.82 |
| person_hit | 1.44 | 1.44 | 1.45 |
| person_sphere | 0.84 | 0.98 | 0.97 |
| wire_put_f32 | 1.51 | 1.54 | 1.54 |
| mvt_ring_area | 3.51 | 3.43 | 3.40 |
| perf_governor | 1.45 | 1.44 | 1.45 |
| audio_render | 1.37 | 1.37 | 1.37 |
| lamps_place_road | 1.17 | 1.25 | 1.22 |
| lamps_near | 2.59 | 2.36 | 2.19 |
25 von 29 über 1,10 (unverändert rot, läuft als Bericht).

## Messung nach E2.1 + E2.2 (Verhältnis Klasse/C++)
| Fall | nach a–c (med) | nach E2.1+E2.2: min | med | Klasse µs (min) | C++ µs (min) |
|---|---|---|---|---|---|
| qr_encode | 14.40 | 4.99 | 5.01 | 134.91 | 27.04 |
| qr_gf_mul | 18.53 | 3.46 | 3.43 | 5.16 | 1.49 |
| qr_rs_remainder | 17.98 | 5.10 | 5.11 | 267.43 | 52.48 |
| nostr_sha256 | 14.15 | 12.80 | 12.49 | 39.39 | 3.08 |
| nostr_mulm | 7.53 | 6.56 | 6.30 | 252.55 | 38.53 |
| nostr_from_hex | 5.54 | 6.04 | 5.52 | 6.88 | 1.14 |
| common_rng_next | 12.87 | 2.18 | 2.34 | 1.13 | 0.52 |
| tiles_key_hash | 1.68 | 1.02 | 1.07 | 0.46 | 0.45 |
| lamps_grid_key | 1.37 | 1.38 | 1.29 | 1.02 | 0.74 |
| mvt_zigzag | 1.00 | 1.00 | 1.01 | 0.45 | 0.45 |
| mvt_read_geometry | 2.69 | 2.56 | 2.43 | 4.39 | 1.71 |
| wire_put_u32 | 1.53 | 1.53 | 1.52 | 4.24 | 2.77 |
| wire_get_u32 | 3.95 | 3.91 | 3.89 | 1.38 | 0.35 |
| settings_escape | 9.02 | 7.56 | 7.51 | 14.49 | 1.92 |
| settings_num | 1.29 | 1.26 | 1.26 | 13.78 | 10.97 |
| common_wrap_pi | 1.85 | 1.23 | 1.28 | 0.66 | 0.54 |
| common_damp_alpha | 1.05 | 1.03 | 1.04 | 1.48 | 1.44 |
| common_qnormalize | 1.20 | 1.19 | 1.20 | 1.99 | 1.67 |
| common_slerp | 1.19 | 1.20 | 1.19 | 14.88 | 12.45 |
| common_to_local | 1.33 | 1.33 | 1.32 | 4.15 | 3.11 |
| common_rng_unit | 10.52 | 1.75 | 1.78 | 1.14 | 0.65 |
| person_hit | 1.72 | 1.44 | 1.46 | 18.38 | 12.74 |
| person_sphere | 0.92 | 0.84 | 0.97 | 1.60 | 1.92 |
| wire_put_f32 | 1.54 | 1.51 | 1.64 | 4.21 | 2.78 |
| mvt_ring_area | 4.22 | 3.51 | 3.49 | 0.77 | 0.22 |
| perf_governor | 1.43 | 1.45 | 1.44 | 1.88 | 1.30 |
| audio_render | 1.43 | 1.37 | 1.39 | 140.32 | 102.78 |
| lamps_place_road | 1.19 | 1.17 | 1.17 | 4.31 | 3.70 |
| lamps_near | 3.02 | 2.59 | 2.62 | 31.05 | 12.01 |

## Früher (bis 21:35)
## Commits
- f50c07d Messwerkzeug `tools/blueprint_bench.sh <level> [runden] [fall…]` + kern/games/spawncity/tests/bench/ (29 Fälle, Quelle gegen Klasse,
  clang-18 `-O3 -flto=thin -DNDEBUG -ffp-contract=off -fno-exceptions -fno-rtti`, Klassen per `tools/blueprint_cpp.sh --classes`; Prüfsummen
  gleich, wo die Quelle in den Zahlen der Sprache rechnet). JetSeats' Claim geht über die Laufzeit an den Actor (func_call_on) – nicht messbar allein, herausgenommen.
- 16c059c 3a–c (nur nodes.h/logic.h/blueprint.h, Text der Klasse unverändert):
  a. `math_compare<Op>`: Operator per `if constexpr` (dieselben Ausdrücke wie compare).
  b. `bits_of`/`whole`: eine Prüfung `fabs(a) < 2^63` für das Innere, Rest wie vorher; `To<Whole>::from(f64)` ohne `normal` (normal∘whole = whole, Begründung im Kommentar).
  c. `kern::math::Exact` (i64): band/bor/bxor/shl/shr/wrap32/wrap32s/mul32 geben ihn statt `(f64)i64`; als Zahl ist er genau dieses double; einer
     dieser Funktionen gereicht: `|v| ≤ 2^53` → v, sonst bits_of(double) – dieselben Bits ohne den Umweg.
- 7aacebf Engine-Fall `formula_bits_edges` (logic_cases.h): alle acht Funktionen + Verschachtelungen über 21 Randwerte (0, ±1, ±2^31, ±2^32, ±(2^53−1),
  ±2^53, ±(2^53+2), ±2^63, 0.5, −2.75, NaN, ±∞, −0) paarweise, Ergebnis als 64 Bit (int64.bits_of_float) und als Int über den Draht; Compare mit allen 6 Operatoren.
- Lokal zusätzlich (nicht im Repo, $S/p/eq.cpp): alte gegen neue Umsetzung, 1.737.120 Vergleiche bei -O0 und -O3, 0 verschieden.

## Messung (Verhältnis Klasse/C++, Median über 15 Runden; diese Maschine, Rauschen ±0,3 bei kleinen Fällen, gelegentlich mehr)
| Fall | Art | C++ µs | Klasse µs vorher | vorher | nach a | nach b | nach c | Klasse µs nach c | gleich |
|---|---|---|---|---|---|---|---|---|---|
| qr_encode | Int, Liste, Text | 27.78 | 786.60 | 28.32 | 28.17 | 19.62 | 14.40 | 391.59 | = |
| qr_gf_mul | Int (Bits) | 1.52 | 50.52 | 33.18 | 32.97 | 23.37 | 18.53 | 27.92 | = |
| qr_rs_remainder | Int, Liste | 53.10 | 1885.73 | 35.51 | 35.79 | 22.85 | 17.98 | 965.91 | = |
| nostr_sha256 | Int (Bits), Fixed | 3.00 | 66.20 | 22.08 | 21.64 | 16.19 | 14.15 | 42.03 | = |
| nostr_mulm | Int, Struktur, Fixed | 36.36 | 313.33 | 8.62 | 8.21 | 7.76 | 7.53 | 256.22 | = |
| nostr_from_hex | Text, Fixed | 1.31 | 9.55 | 7.32 | 8.27 | 5.74 | 5.54 | 6.36 | = |
| common_rng_next | Int (Bits), Ref | 0.51 | 10.67 | 20.92 | 20.55 | 16.72 | 12.87 | 6.38 | = |
| tiles_key_hash | Int64 | 0.46 | 0.59 | 1.29 | 1.28 | 1.12 | 1.68 | 1.63 | = |
| lamps_grid_key | Float->Int64 | 0.74 | 1.04 | 1.41 | 1.42 | 1.35 | 1.37 | 1.37 | = |
| mvt_zigzag | Int64 | 0.45 | 0.45 | 1.00 | 1.00 | 1.00 | 1.00 | 1.00 | = |
| mvt_read_geometry | Bytes, Listen | 1.75 | 5.91 | 3.37 | 3.34 | 2.64 | 2.69 | 4.71 | = |
| wire_put_u32 | Bytes, Liste | 2.82 | 4.30 | 1.53 | 1.50 | 1.52 | 1.53 | 5.00 | = |
| wire_get_u32 | Bytes, Struktur | 0.36 | 1.60 | 4.46 | 4.42 | 3.88 | 3.95 | 1.46 | = |
| settings_escape | Text | 1.92 | 20.07 | 10.47 | 10.21 | 8.70 | 9.02 | 9.13 | = |
| settings_num | Float->Text | 11.13 | 13.90 | 1.25 | 1.26 | 1.27 | 1.29 | 1.28 | = |
| common_wrap_pi | Float, Schleife | 0.50 | 0.83 | 1.65 | 1.30 | 1.56 | 1.85 | 0.83 | f32 |
| common_damp_alpha | Float, exp | 1.44 | 1.50 | 1.04 | 1.04 | 1.05 | 1.05 | 1.51 | f32 |
| common_qnormalize | Quat | 1.69 | 2.02 | 1.20 | 1.19 | 1.19 | 1.20 | 1.20 | f32 |
| common_slerp | Quat, Bibl. | 12.58 | 14.99 | 1.19 | 1.19 | 1.19 | 1.19 | 14.97 | f32 |
| common_to_local | Vektor, Bibl. | 3.26 | 4.34 | 1.33 | 1.32 | 1.32 | 1.33 | 4.19 | f32 |
| common_rng_unit | Int->Float | 0.65 | 11.71 | 18.01 | 18.09 | 13.49 | 10.52 | 10.60 | = |
| person_hit | Vektor, Bibl. | 12.78 | 18.64 | 1.46 | 1.46 | 1.45 | 1.72 | 30.42 | f32 |
| person_sphere | Vektor | 1.61 | 1.56 | 0.97 | 0.98 | 0.93 | 0.92 | 0.98 | f32 |
| wire_put_f32 | Float->Bytes | 2.80 | 4.27 | 1.53 | 1.50 | 1.40 | 1.54 | 1.53 | = |
| mvt_ring_area | Liste v. Vektor | 0.23 | 0.77 | 3.39 | 3.48 | 3.62 | 4.22 | 0.77 | = |
| perf_governor | Struktur, Float | 1.37 | 2.18 | 1.59 | 1.47 | 1.44 | 1.43 | 1.88 | f32 |
| audio_render | Float, Liste | 109.23 | 181.30 | 1.66 | 1.67 | 1.51 | 1.43 | 153.42 | f32 |
| lamps_place_road | Map, Set, Liste | 4.00 | 4.76 | 1.19 | 1.22 | 1.19 | 1.19 | 4.50 | f32 |
| lamps_near | Map, Liste | 13.12 | 41.27 | 3.14 | 2.24 | 1.65 | 3.02 | 52.91 | f32 |

## Ursachen (Anteil; Stelle)
1. **Float-Grenze jeder Formel** (Ganzzahl-Fälle: nach a–c der ganze Rest, ~10–18×): hinein `kern::to<Float>(Int)` (cvtsi2sd), im Lambda `bits_of(f64)`
   (Prüfung + cvttsd2si), heraus `-> Float` (cvtsi2sd) und `kern::to<Int>` (Prüfung + cvttsd2si) – vier Wandlungen in der Abhängigkeitskette je Formel,
   wo das C++ einen Befehl hat. Ohne Änderung der Schreibweise nicht zu beheben (Lambda-Parameter ist `Float`). Projektion ($S/p/proj.cpp, Rng::next):
   heute 25,1 ns, ganzzahliges Lambda 2,8 ns, C++ 1,85 ns. → ENTWURF (folgt).
   Dazu: Vergleiche mit Vorzeichen-Nachbildung als Formel (`(i<0)!=(n<0) ? n<0 : i<n`) + `math_compare<">">(…, 0.5)` in Schleifenbedingungen (ring_area, wire_get).
2. (behoben, b) `bits_of`/`whole` mit drei Prüfungen, `normal` nach `whole`: −25–35 % bei Ganzzahl-Fällen.
3. (behoben, c) Zwischenwerte der Bit-Funktionen über double: weitere −20–25 %.
4. (behoben, a) `math_compare` als Text zur Laufzeit: mit clang -O3 kaum messbar (wrap_pi), mit g++ -O2 laut Vorgänger 7,5 %.
5. **Schleifenwächter** `kern_pass()` je Durchlauf (Speicherzähler): Versuch „aus“: wrap_pi 1,85→0,84, ring_area 4,2→3,2, nostr_mulm 7,5→5,6, lamps_near 3,0→2,4.
   Bedeutung muss bleiben (gleicher Durchlauf in jeder Stufe) – nur Straffung möglich (d), offen.
6. **Listenzugriff mit Platz-neben-der-Liste-Prüfung**: Versuch „aus“: settings_escape 9,0→6,5, ring_area 4,2→2,9, lamps_near 3,0→2,3, nostr_mulm 7,5→6,9.
   Wegfallen nur mit Beweis am Graphen → steht im Text → ENTWURF.
7. **`Int +=` über whole_add** (Bereichsprüfung): Versuch „ohne“: ring_area 4,2→2,4, nostr_sha256 14,2→12,9, nostr_mulm 7,5→6,3. Bedeutung (Addition wie double) muss bleiben.
8. **Text anhängen ohne Übergabe**: `o = text_append(o, text_of_code(c), "")` kopiert den ganzen Text je Zeichen (O(n²)); hands_over (notation.cpp ~Z. 1381)
   gibt bei reinen Knoten nie weiter. Abhilfe ändert den Text (std::move) → ENTWURF. (settings_escape ~9×)
9. Bytes als `List<Int>` (8 Byte je Byte, Whole je Byte) – wire_get_u32 ~4×, mvt_read_geometry ~2,7×; Ursache 1 + 6 dazu.
10. Aufruf an einen Actor über die Laufzeit (`func_call_on`, Values) – JetSeats; Architektur, hier nicht gemessen.
11. Float-Fälle (Vektor, Quat, Bibliothek): 1,0–1,5× – Rest ist 64 statt 32 Bit (gewollt) und Ursache 5; kein eigener Befund.
Nicht gemessen: `flow_switch_text` baut je Aufruf `std::vector<std::string>` der Fälle (blueprint.h `setting_cases`) – Kandidat für d.

## ENTSCHEIDUNG NÖTIG (E2) – Entwurf zur Schreibweise (nach Nachtrag 17:50, Punkt 4; Messzahlen nach a–c oben)
Nach a–c bleibt bei Ganzzahl-Fällen 10–18× (qr 14×, sha256 14×, rng 13×), fast ganz an der Float-Grenze der Formel (Ursache 1).
**E2.1 Formel mit ganzzahligen Ein-/Ausgängen.** Text: `kern::to<Int>(kern::formula([](Int s) -> Int { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 13.0))); }, self.s))`
  – der Writer schreibt `Int` statt `Float` für einen Eingang, an dem ein Int hängt, und `-> Int`, wenn das Ergebnis in einen Int-Eingang geht UND der Ausdruck
  nur aus band/bor/bxor/shl/shr/wrap32/wrap32s/mul32, Zahlliteralen und Eingängen besteht (Regel am Ausdruck, keine Namensliste).
  Bedeutung in jeder Stufe: unverändert – Interpreter/JIT rechnen wie heute in double. In der Klasse: Eingang `Int x` geht als `Exact{x}` hinein
  (bits_of(Exact) = bits_of(double(x)) für jedes x, schon bewiesen: eq.cpp + formula_bits_edges), Ergebnis `Exact r` → `Int(r.v)` = normal(r) = whole(double(r))
  (Beweis wie in To<Whole>::from). Für jeden Wert dasselbe, weil jede dieser Funktionen schon heute ein ganzzahliges double liefert.
  Gemessen (Projektion $S/p/proj.cpp, Rng::next): heute 25,1 ns → 2,8 ns, C++ 1,85 ns. Erwartung: Ganzzahl-Fälle von 10–18× auf ~1,5–2×.
  Kosten: Writer (Formel-Typ je Eingang aus den Drahttypen), Leser (from_cpp liest `Int`-Parameter und `-> Int` zurück, Fixpunkt-Tests),
  `kern::formula`-Überladung, Doku blueprint_cpp.md „Formeln“; kein neuer Knoten, keine Graph-/Editor-Änderung, Konverter unberührt (Graph gleich).
  +, −, × zwischen Ints im Ausdruck: nur mit geprüftem Weg (exakt solange |·| ≤ 2^53, sonst double) – als zweiter Schritt.
**E2.2 Weitergabe beim Anhängen (Ursache 8):** hands_over (notation.cpp ~1381) auch für einen reinen Knoten, dessen Ergebnis die Variable setzt, die er als
  ersten Eingang liest: `o = text_append(std::move(o), x, "")` (gibt es im Text schon; der Leser kennt std::move). settings_escape ~9× → erwartet ~2×. Graph gleich.
**E2.3 Index am Graphen bewiesen (Ursache 6):** For Each / For 0..length−1 über dieselbe Liste ohne Längenänderung im Rumpf → Zugriff ohne Platz-daneben-Prüfung,
  im Text sichtbar (etwa `ring.at_known(i)`). Bringt laut Versuch 10–30 % bei Listen-Schleifen. Kleiner als E2.1 – nur wenn gewünscht.
Frage: E2.1 bauen (ja/nein)? E2.2? E2.3?

## ENTSCHEIDUNG NÖTIG (E3) – cost: „instead of 0“ (Suite work-zusammen-voll-203000-750, $S/t1/suite203000_cost_note.txt), Ursachen
- **A, ~74 von 93: Aufruf an einen Actor** – controls_cpp (42), combat_rules_cpp (26), combat_detail_cpp (6): die Funktion der _cpp-Klasse ist
  `func_call_on<"Controls_Smoothstep01">(actor_find("Controls"), …)` (Konverter-Entscheidung: Funktionen des Regel-Actors). Speicher: blueprint.h `func_call_on`
  (vector args + vector results), `Runtime::call_on` (logic.cpp ~3017: results aus from_json, vector got), dahinter Instance::call. Null Anforderungen bräuchte einen
  Aufrufweg ohne std::vector<Value> (Puffer fester Größe / Aufruf der Klasse direkt, wenn das Ziel eine Klasse ist) – in kern/src/logic/logic.cpp und Runtime-API,
  außerhalb meiner Dateiliste. Frage: darf p Runtime::call_on/Instance::call (logic.cpp, logic.h) dafür ändern?
- **B: Listen wachsen anders** (combat_art_cpp put/beam, city_jobs_cpp put_*/read_*, destruction_cpp, city_stream_cpp): Bytes als List<Int>, Anhängen je Element
  statt eines Einfügens/resize – je Fall 1–2 Anforderungen. Noch nicht zugeordnet. Gelesen: CombatArt_put fügt wie die Quelle ein
  (list_insert_list → insert_items = vector::insert eines Bereichs; list_make ist ein Fixed auf dem Stapel) – die 2 Anforderungen kommen nicht aus diesen Knoten;
  zu prüfen mit t1, ob sie im Test beim Umwandeln der Argumente (Mat4/Struktur aus Value) entstehen. BEFUND FÜR t1.
- **C: Runtime_new/reset der Firmware-Einheiten** (Arondight45_*) – Firmware ist nicht zu ändern; Ursache noch nicht gelesen.

## Zeit-Test (Schritt 5) – Entwurf
`tools/blueprint_bench.sh` mit `--check <schwelle>`: Ende ≠ 0, wenn für einen Fall der MEDIAN des Verhältnisses über 15 Runden über der Schwelle liegt.
Schwelle 1,5: das Rauschen dieser Maschine (geteilt) lag bei unveränderten Fällen zwischen zwei Läufen bei bis zu ±0,4 (wrap_pi 1,30/1,56/1,85,
lamps_near 2,24/1,65/3,02 bei gleichem Code) – mit 1,0 wäre der Test rot ohne Grund. Vorschlag CI: zwei Läufe, rot nur wenn beide über 1,5; je Fall eine
Liste „bekannt über der Schwelle, mit Ursache“ bis E2 entschieden ist (heute 13 Fälle). Läuft ohne Engine (nur Klassen + musl), ~1 min Bau.

## (entschieden: E10 = JA, 23:2x) E7 – Literal einer Liste fester Länge an einem Eingang (Befund k, E3-C/Arondight45_*), 23:25
Gelesen (Leser+to_cpp über Arondight45_DroneFC_Core aus work-zusammen-voll-203000-750): Feld-Vorgaben und Struktur-Literale schreibt to_cpp schon als
`Fixed<fc_PT1, 3>{…}` (literal mit TypeRef). Die Anforderungen kommen von einem **Wert an einem Eingang**: `self.filters_ = struct_make<fc_Filters>(List<fc_PT1>{…}, List<fc_PT1>{…}, …)`
(Runtime_reset, Z. 543/632) – der Schreiber schreibt den Wert eines Pins mit `literal(v, of, g)` ohne die Länge des Felds; `List<…>{…}` ist ein std::vector,
der dann in das Fixed des Felds kopiert wird (2 Anforderungen je Aufruf). Vorschlag: der Wert an einem Eingang, dessen Typ eine Liste fester Länge ist
(bei struct.make: das Feld; allgemein: der Pin mit count), wird mit seinem Typ geschrieben: `struct_make<fc_Filters>(Fixed<fc_PT1, 3>{…}, …)` – dieselbe
Schreibweise wie bei Feldern/Variablen (blueprint_cpp.md „Plätze“), der Leser liest sie schon dort. Bedeutung gleich (dieselben Einträge, Rest null),
in keiner Stufe ein anderer Wert; Graph unverändert; ändert den Text der Klasse (betroffene Klassen: jede mit einem Listen-Literal an einem Fixed-Eingang).
Kosten: Schreiber (Pin-Typ mit count an den Literal-Schreiber), Leser prüfen (Fixed-Literal an einem Eingang), Fixpunkt-Fall, Doku. Frage: bauen (ja/nein)?
