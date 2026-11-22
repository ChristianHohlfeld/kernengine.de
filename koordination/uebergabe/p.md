# Übergabe Paket p – 19.11.2026, 09:00 (Nachfolger 2)
Maßgeblich: $S/stand/p.md (oben: E17-Messung, Entwurf 4, Ursachen je Fall, Tabelle mit Versuchsspalten, Befund für e).
Zweig work-p, Kopf 9c315bf2 (gepusht). Darin: 50e21923 Text::Appending (settings_escape 7,59→3,37), 9c315bf2 E17 (Zeit-Test, beide Spalten).
Belege: work-p-engine-075125-9242 (auf 50e21923, lief um 09:00 noch – abholen: ./mag.sh holen …); danach EIN Auftrag
`./mag.sh vergleich origin/work-p src/settings.cpp` (Text::Appending), dann `./mag.sh engine origin/work-p` auf dem Kopf (E17 berührt nur
die Bench; die Engine-Suite baut sie nicht – dennoch einmal). Erst dann „BEREIT ZUM MERGEN 9c315bf2“ an main.
Entschieden (Koordinator 07:55/08:10/08:45): E17 JA (gebaut), Punkt 2 JA (bauen), Punkt 3 JA (bauen); Entwurf 4 (feste Breiten) bei Chris – NICHT bauen.
Paket e (E16) besitzt „wo der Wert eines Schritts steht“ (nNN als Mitglied) – nicht anfassen; nach seinem Merge neu messen.

## Nachfolger 3 (19.11., ab 09:40)
- work-zusammen e32ba575 eingemischt (vorgespult). **Punkt 2 gebaut: 1831e165** (gepusht) – Stand und Zahlen: stand/p.md „Punkt 2“.
  Regel: formula.cpp `int_input(k)`, `whole_result()`, `result_sum()`, Hilfe `whole_operand` (logic.h privat). Schreiber `formula_types`
  (notation.cpp, statt whole_formula), Leser `formula()` mit `ints`, `int_inputs` (Knoten, Eingang) und `int_sums` (Leser-Prüfung in wires()).
  Klasse: `Whole(const Sum&)` (nodes.h, nach Sum definiert). Wichtig: +,−,× zwischen nur Exact/Zahlen ist ein double (kein Sum) – darum verlangt
  whole_result bei einer Operator-Wurzel einen Eingang darin (sonst würde double→i64→Whole still abschneiden).
  Lokaler Prüfer (ohne Suite): /home/claude/p-tmp/chk (chk.cpp: Fixpunkt aller logic_cases + Leser-Fälle aus test_main; sem.cpp: Lambdas gegen
  Formula::evaluate über alle Paare). Bau: Objekte in chk/o, unter heavy3, ~1 min.
- Belegt (stand/p.md „BEREIT ZUM MERGEN: 1831e165“): engine `work-p-engine-095126-25407` rc=0, vergleich `work-p-vergleich-103059-7878` 11/11 beide Durchgänge.
- Nächstes (wenn entschieden): Punkt 3 nach dem gewählten Weg; Rest über 1,10 laut Tabelle (Wächter, Bereich, Bytes = Entwurf 4 bei Chris).
- **Punkt 3 nicht gebaut:** der geplante getypte Einstieg greift bei den cost-Aufrufen nie (Ziel = Level-Actor mit eigenen Struct-/Enum-Typen,
  andere Feldnamen) – Befund und zwei Wege in stand/p.md „Punkt 3“, Entscheidung beim Koordinator.

## Stand 11:45 (Nachfolger 3 endet – Kontext erschöpft) – Entscheidungen des Koordinators 11:30
work-p = 0cb3f6b6 (= work-zusammen, vorgespult, gepusht). Entschieden: **Punkt 3 → Weg (b)**; **E20 = Entwurf 4 Weg 1 JA** (u8/u16/u32/i8/i16/i32 als
Typen, Speichern wickelt wie C++, Interpreter/JIT exakt in double, × als ganzzahliger Knoten; List<u8> statt Bytes-Typ; danach Schleife mit lokalem Wächter
für Rümpfe ohne Aufruf). Reihenfolge: (b) bauen+belegen, dann E20 in kleinen einzeln belegten Schritten (Sprache/Leser/Schreiber/Interpreter/JIT/Klasse/
Editor-Typen/Doku/Engine-Fälle; Konverter als eigener Schritt mit konvert-Beleg). Paket e mischt bald die neue Klassenform ein – work-zusammen einmischen,
sobald der Koordinator es meldet.

### Bauplan (b) – Struktur beim Aufruf ohne Heap (noch nicht begonnen)
Wo die Anforderungen entstehen (GrenadeCook_pull: 5 statt 0): (1) Aufrufer blueprint.h func_call_on: `logic::to_value(struct)` → Value::structure =
make_shared + vector (2); (2) Glue des Ziels (notation.cpp native_glue, `results = {to_value(out.x)…}`) für ein Struktur-Ergebnis (2); (3) Runtime::call_on
(logic.cpp ~3047) `from_outside(results[k], declared, g)` → from_outside_typed + fitted (logic.cpp 678/680) baut die Struktur neu (≥1). Das Ziel liest Argumente
über native_arg<T> → detail::Held<T>::from (Feld für Feld, kein Heap für Zahlen/Bool/Enum).
Value: `std::shared_ptr<const std::vector<Value>> rows` mit Kopie-beim-Schreiben über use_count (own_rows, logic.h ~300); Kopierkonstruktor = default.
Falle: ein „geliehener“ rows-Zeiger (Aliasing-shared_ptr ohne Kontrollblock) hat use_count 0 und überlebt Kopien – das Interpreter/JIT-Ziel kopiert die
Argumente (Instance::call_into: `std::vector<Value>(args…)`) in seine Knoten → hängender Zeiger nach dem Aufruf. Darum NICHT so.
Vorschlag, wie es sauber geht (bitte prüfen, dann bauen):
- Weg über die Glue statt über Value-Felder: NativeBlueprint bekommt `call_fields(function, args, results)` nicht nötig – einfacher: der Aufrufer reicht
  Struktur-Argumente als Value, deren Felder in einem `std::array<Value,N>` der Aufrufstelle liegen, NUR an ein Klassen-Ziel (Instance::blueprint_ gesetzt;
  dort liest Held::from sofort in den eigenen Typ, nichts bleibt liegen). Für Interpreter/JIT-Ziele bleibt der heutige Weg (vorher in eine eigene Value kopieren).
  Dafür braucht Value eine nicht besitzende Sicht auf Felder (z. B. `const Value* view_; u32 view_n_` neben rows; field()/fields() lesen beide; jede Kopie
  einer Sicht macht daraus rows – also Kopierkonstruktor/Zuweisung nicht mehr default, Kosten messen!).
- Ergebnis: Runtime::call_on gibt dem Klassen-Ziel vorgeformte Plätze (Sicht auf ein std::array der Aufrufstelle, Feldzahl nach dem erklärten Ausgang);
  die Glue schreibt `to_value(out.x)` Feld für Feld hinein, wo der Platz eine Sicht gleicher Feldzahl ist, sonst wie heute.
- from_outside für Strukturen: prüfen, ob es bei passender Gestalt (gleiche Feldzahl, Felder schon vom erklärten Typ) den Wert unverändert lassen kann
  (gleiche Bedeutung) – sonst bleibt dort eine Anforderung.
Beleg: Engine-Suite (Fall: Aufruf an einen Actor mit Struktur hin und zurück, Klasse→Klasse und Klasse→Interpreter, Feldtypen abweichend → Vorgabe),
vergleich src/controls.cpp src/combat_rules.cpp – cost-Zeilen GrenadeCook_*, WorldLook_*, Chaos_*, Fun_multiplier, MilitaryClock_update, Race_update,
ZombieRounds_*, choose_enemy, flak_cell weg (Liste: mag/ergebnis-work-zusammen-voll-070328-32111-alle-080335-20196/alle-cost.txt).

### E20 – erste Schritte (Vorschlag)
1. PinType u8…i32 (logic.h), Wandlungsregeln (convertible, Speichern = Wickeln wie static_cast), JSON/Notation-Namen (`U32` …), check(); Engine-Fall Ränder.
2. Interpreter/JIT: Speichern wickelt (double exakt, da ≤ 2^32), × zweier fester Breiten als ganzzahliger Knoten (mul32 heute).
3. Klasse: Typen als u8…i32 (nodes.h/blueprint.h), Leser/Schreiber, Fixpunkt; Editor-Farben (studio/logic_edit.cpp), Doku.
4. List<u8> (Bytes-Knoten auf List<u8>), dann Konverter (tools/cpp_to_blueprint.py: u32-Quelle → u32-Blueprint, wrap32/band entfallen) mit konvert-Beleg.
5. Schleifenform mit lokalem Wächter (Entwurf 4 (b)).

## Werkzeuge
- $S/p/rb.sh <runden> [fall…]: Bench mit den Headern des Worktrees neu bauen, ohne neue Klassen (CL=classes-Ordner, Vorgabe $S/p/b4;
  TESTS=Tests-Ordner, Vorgabe $S/p/conv = tests/converted von 055004; RD=Bauordner; XF=-g). Nach Dateiumbenennungen $RD/o löschen.
- Voller Zeit-Test: `TMPDIR=$S/shared-tmp KERN_JOBS=2 BENCH_DIR=$S/p/b5 flock $S/heavy2.lock flock $S/heavy3.lock tools/blueprint_bench.sh
  $S/p/lvl2/spawncity.klevel --tests $S/p/conv --check 1.10 15` (Klassen neu: ~8 min). Level $S/p/lvl2 = 055004.
- Projektion Punkt 2 (nur Messung): $S/p/b4p/classes.h (Ersetzung `X ± c` und Vergleich mit Vorzeichen im Klassentext).

## Bauplan Punkt 2 (Int-Formeln ganz) – noch nicht begonnen
Heute (E2.1/Schritt 2): Formula::whole (formula.cpp ~347) verlangt eine ganzzahlige Funktion an der Wurzel; Schreiber whole_formula
(notation.cpp ~1163) nur, wenn alle Eingänge Drähte von Ints sind; Leser notation.cpp ~3406.
1. formula.cpp: zwei neue Fragen statt einer (eine Regel am Ausdruck, keine Namen):
   - `whole_result()`: Wurzel ganzzahlige Funktion ODER +,−,× (kWholeOperators), deren Operanden Eingänge, Zahlen, ganzzahlige Funktionen, solche
     Operatoren sind → `-> Int` (Klasse: Whole(Sum) = whole(double) über sum_exact – To<Whole>/Whole-Konstruktor für Sum ergänzen, falls fehlt).
     Zusätzlich am Graphen (Schreiber): JEDER Abnehmer des Ausgangs ist ein Int-Eingang (sonst `-> Float`; Grund: über 2^63 klemmt whole, double nicht).
   - `int_input(k)`: Eingang k wird nur als Operand von +,−,×, eines Vergleichs (== != < <= > >=, andere Seite Eingang/Zahl/Summe) oder als Argument
     einer ganzzahligen Funktion gelesen → Parameter `Int`. Nie bei /, %, pow, sin … (Whole/Whole wäre in C++ ganzzahlige Division!); die stehen ohnehin
     in Aufrufform (kern::math::div), trotzdem ausschließen.
   - Ausgang `-> Float` bei Int-Eingängen erlaubt (Vergleich gibt 0/1; Sum als f64 = genau die double-Rechnung).
2. Klasse (nodes.h): Vergleich Whole gegen double/Whole: Whole → i64 → (gegen double) wie C++ → dasselbe wie im Interpreter, weil jedes normale Int als
   double exakt ist und die Umwandlung die Ordnung erhält (auch ±(2^63−1)). Sum gegen Zahl im Vergleich: Sum → f64. Mehrdeutigkeiten prüfen.
3. Schreiber/Leser: Int-Parameter nur, wo int_input(k) und der Draht von einem Int kommt; Leser lehnt sonst mit eigenem Text ab (Fälle: `Int` bei
   Division, `-> Int` bei Float-Abnehmer, `-> Int` mit sin an der Wurzel). Fixpunkt to_cpp→from_cpp→to_cpp.
4. Engine-Fall (kern/tests/logic_cases.h, wie formula_whole_arith): Ränder ±2^53, ±(2^53+2), ±(2^63−1), gemischte Vorzeichen, `i - 1`, `2 * i + 1`,
   `(i<0)!=(n<0)?…`, Vergleich i64 gegen double (2^53+1 gegen 2^53 als Zahl); Klasse == Interpreter == JIT. Doku blueprint_cpp.md „Formeln“.
5. Messung: Erwartung laut Projektion qr_encode 5,0→3,3, gf_mul 3,4→2,9, rs 5,1→3,3, get_u32 3,9→3,0, read_geometry 2,5→2,0, ring_area 3,4→2,8.
   Beleg: engine + vergleich über qr_code, nostr, settings, wire(=src/wire.h? als Quelle des Konverters), mvt.

## Bauplan Punkt 3 (getypter Einstieg in func_call_on) – noch nicht begonnen
Struktur-Argumente werden heute logic::Value::structure (make_shared + vector: 2 Anforderungen) – controls_cpp/combat_rules_cpp cost-Zeilen.
Die Glue jeder Klasse (tools/blueprint_cpp.cpp) meldet je nicht-latenter Funktion einen getypten Einstieg (Funktionszeiger mit der C++-Signatur,
Schlüssel Blueprint+Name). func_call_on<"Name">(actor, …) (blueprint.h) sucht ihn einmal je Aufrufstelle (static) und ruft direkt, wenn die
Instanz des Ziels als Klasse läuft (NativeBlueprint) und die Signatur passt; sonst der heutige Weg über Value. Bedeutung gleich: NativeBlueprint::call
ruft heute genau diese Methode nach dem Umwandeln. Beleg: Engine-Suite; vergleich src/controls.cpp src/combat_rules.cpp – cost-Zeilen weg.

## Nachfolger 4 (19.11., ab ~11:50) – E20
- Schritt 1 = 31d75912 (Sprache, Leser/Schreiber, Interpreter, JIT-Rückfall), Schritt 2 = 15c8ad52 (Klasse logic::Sized<T>, Fall
  fixed_widths, Web-Editor, Doku). Entscheidung (selbst): Breite am PLATZ wie Fixed – TypeRef::of neben integer ("u8"…"i32"),
  Draht = Int; kern::to<U8> verboten (kern::to<Int>); Add = untere Bits der u64-Summe; × = mul32 (kein neuer Knoten);
  Map ohne Breiten; List<U8> liest der Leser, check() lehnt ab („noch kein Typ“) bis Schritt 3.
- Lokal: /home/claude/p-tmp/w20/run.sh [quellen…] (synchronisiert body.inc → test_main TEST(logic_fixed_widths), baut w);
  case/mk (Text→Klasse/JSON), case/run (Interpreter-Ergebnis), Python-Modell im Verlauf (316 Werte gleich).
- Belege: engine work-p-engine-120146-7563 (startete nach 15c8ad52 → deckt beide), CI 44511064433 (31d7), 44511958238 (15c8).
- Nächst: Schritt 3 List<U8> (Listenknoten wickeln beim Speichern: add/insert/set/fill/resize/copy_bytes; type_choices mit
  Breiten für Listen-Einstellungen; Klasse List<Sized<u8>> – Entry<T>/Held<vector<Sized>>/Bytes-Knoten), Schritt 4 Konverter,
  Schritt 5 Schleife mit lokalem Wächter, dann (b).
- 13:25: Schritte 1+2 belegt (stand/p.md „BEREIT ZUM MERGEN: 62229851“). Wichtig für Schritt 3 (List<U8>), gelesen, nicht gebaut:
  * fits(): int_list ↔ u8_list passen NICHT (in C++ List<U8> ≠ List<Int>). Folge: die Bytes-Knoten (text.of_bytes, list.copy_bytes,
    struct.bytes/of_bytes, list.insert_text …) haben Pins K::integer_list – sie müssen Listen jeder Breite nehmen (Pin-Gestalt nach dem
    Draht oder Einstellung „type“ mit u8_list), und ihre Klassen-Funktionen (blueprint.h list_copy_bytes, BytesNumber, Items<…>) als
    Vorlage über das Element (Whole oder Sized<T>). Das ist der eigentliche Bytes-Gewinn (wire_*, mvt_read_geometry).
  * Speichern in eine Liste fester Breite: Platz-Typ über g.place_of(c.node(), named) → declared(named); wickeln in list.add,
    list.insert (Einzelwert), list.make mit type u8, list.resize-Füllwert, list.insert_text, copy_bytes; ganze Listen über fitted (fit
    wickelt schon Listen). Context::out NICHT allgemein wickeln (O(n) je Get).
  * type_choices(): Breiten anbieten (nicht bei keys_only/enums_only/structs_only), with_lists → „u8_list“; check(): Fehler „noch kein
    Typ“ (logic.cpp width_ok) entfernen; logic.mjs typeOptions: Listen der Breiten wieder zulassen; e2e-Zahlen dann +6.
  * Klasse: List<U8> = logic::List<Sized<u8>> – detail::Entry<Sized<T>>, Held<std::vector<Sized<T>>>, to_value(vector<Sized>), list_type_of.

## Nachfolger 5 (19.11., ab ~13:00) – E20 Schritt 3 gebaut, Schritt 4 vorbereitet
- work-zusammen 73c2ac07 eingemischt (vorgespult). **Schritt 3 = 18c636be + 0c57a404** (gepusht). Entscheidungen (selbst, nach den Zielen):
  * List<U8> … List<I32> ist eigener Typ (fits: Breite gleich); **Eingänge, die die Zahlen einer Liste lesen, wie sie sind** (Text of Bytes,
    Struct of Bytes, Items von Copy Bytes) haben `of = kAnyWidth` ("*", logic.h) – fits/Leser (`same`, Literal `List<Int>{…}`)/Editor
    (logic_edit.cpp: generic "int_list u8_list …") nehmen jede Int-Liste; Klasse: `logic::items_of(L)` (nodes.h) → Items<T>, Funktionen als
    Vorlagen über die Liste (text_of_bytes, struct_of_bytes, list_copy_bytes, list_insert_text).
  * Speichern wickelt (core_nodes.cpp `stored_as`, `width_of_place`): list.add/insert/resize-Wert, list.make (Typ u8), insert_text (I8 → −128..−1).
    Find/Contains/Remove Item vergleichen die ZAHL (C++: std::find eines int in vector<u8> wickelt nicht) – Klasse `logic::Sought<T>` = Whole bei
    Sized, `equal_item`; Interpreter unverändert. Copy Bytes in eine Liste einer Breite: code muss die Breite sein (problem()); Anordnung: ein
    Feld einer Breite hat seine Breite (layout_problem).
  * Typwahl: `ParamDef::widths` (in type_param): Breiten als Item-Typ (type_param(false)), nur Breiten-LISTEN bei with_lists (Select/Reroute/
    get_on: ein einzelnes U8 am Pin wäre eine andere Zahl); Map-Werte nie. Sized: `explicit operator i64`.
  * Schreiber: Vorgabe `List<U8>{…}` (notation.cpp literal). Web: typeOptions mit Breitenlisten, lgTyped "u8_list"; e2e +6 (28/30/32/34).
- Lokal belegt: Fall l/c.txt (p-tmp/w20/l): Klasse == Interpreter == echtes C++ (std::vector<u8>/<int8_t>/<uint32_t>, l/real.cpp);
  Fixpunkt aller logic_cases (chk) bad=0; logic_fixed_widths (body.inc, Leser-Fälle + Werte von fixed_width_lists) bad=0.
  Werkzeug: p-tmp/w20/mk.sh baut case/mk (Text → Klasse, JSON, Glue: `case/mk in.txt out.h out.json glue.h`) und case/run.
- **Belegt (15:40): 1e91e798** – engine `work-p-engine-150118-30531` rc=0 (54788/0, blueprint_native_test 64 Blueprints), CI 44539294064 grün bis auf
  die 3 e2e-Stellen von work-zusammen. Nachträge nach 0c57a404: 794faaa5 `logic::place_type(PinDef)` (ein Platz nach einem kAnyWidth-Pin ist
  List<Int>: Editor „zur Variable“, nativer Fall Nodes), 6a00887f Glue (notation.cpp type_in: List<U8> = kern::List<kern::logic::Sized<kern::u8>>),
  9dbb7749 nativer Erzeuger cpp_type ebenso, 1e91e798 Proben einer List<U8> im nativen Test gewickelt (C++-Aufrufer übergibt eine List<U8>;
  ein einzelnes Int einer Breite bleibt roh – das Wickeln am Eingang wird so mitgeprüft). Struktur-Proben mit Breiten-Feldern sind NICHT gewickelt
  (kein Fall hat sie; käme einer, zeigt die Klasse die gewickelten Argumente → dort ebenso fitted).
- Lokal: p-tmp/w20/e/em.cpp (emit aller nativen Fälle nach e/nat2, ohne den Engine-Link; Objekte in w20/o – nach einem Merge die Engine-Objekte
  neu bauen, sonst scheitert der Fall Engine an neuen Knoten); Treiber streng übersetzen: clang++ -fsyntax-only … nat2/blueprint_native_main.cpp.

### Bauplan Schritt 4 (Konverter, tools/cpp_to_blueprint.py) – NICHT begonnen
- Typen: NUMBER (Z.143) bleibt die Typ-Tabelle der AUSDRÜCKE (Draht = Int). Neu nur für PLÄTZE: unsigned char/short/int, signed char,
  short, int → U8/U16/U32/I8/I16/I32 (Entscheidung offen: `int` → I32? C++-treu ja – i32 ist int, ALIAS macht beides gleich; Folge: fast jede
  Funktion ändert sich, Klasse schneller (Sized speichert ohne normal()), Interpreter wickelt in hold()). Stellen, wo Plätze ihren Typ
  bekommen (btype-Aufrufe): Felder 2225 (state_fields) / 1863, Variablen der Datei 2108, geteilte 2253, Lokale 3066 (`local`), Rückgabe 3109,
  Parameter 3200, Listen-Einträge `entry`/`element` 1545/1556 (List<U8>), Struct-Felder der Tupel 1573. tp()/held_type (2987/2997) geben den
  ERKLÄRTEN Typ zurück → 211 Vergleiche `== "Int"` brechen für U8-Plätze: tp() muss für einzelne Breiten "Int" geben (Draht), für Listen
  `List<U8>` behalten; Listen-Stellen, die "List<Int>" erwarten (10 Literale, z. B. 5750 bytes-Temp, BYTE_FIELD 539), auf Listen jeder Breite.
- Typisierte Listenknoten schreiben den Item-Typ: `list_get<U8>`, `flow_for_each<U8>` (Knotenmitglied `U8 item`).
- wrap32/band entfallen nur beim SPEICHERN: `narrow()` (8973) gibt den Text unverändert, wenn das Ziel ein Platz der Breite `want` ist
  (Zuweisung, Initialisierung, return, list_add/insert/resize in List<U8>, Feld). Innerhalb eines Ausdrucks bleibt wrap32/mul32 (der Draht ist
  ein Int; Vergleiche/Shifts/Division danach brauchen die gewickelte Zahl). Dafür braucht fx() die Breite des Ziels an der Wurzel
  (z. B. `self.store_width`), nur auf der obersten Ebene (+,−: wrap32 weg; ×: mul32 bleibt – 2^32·2^32 ist in double nicht exakt; <<: wrap32
  weg nur, wenn das Produkt < 2^53 bleibt, sonst bleibt shl+wrap).
- BYTE_CODES (531): Copy Bytes nach/von std::vector<u8> → List<U8> mit code u8 (problem() verlangt es jetzt).
- Beleg: `./mag.sh konvert origin/work-p 9` (convert rc=0) + vergleich über src/nostr.cpp src/qr_code.cpp src/common.cpp src/wire.cpp
  (bzw. src/mvt.cpp src/settings.cpp der Bench-Fälle), beide Durchgänge; danach Zeit-Test (rb.sh / blueprint_bench.sh --check 1.10).
- t1 (Test-Schreiber): converted.h `namespace wide` hat u8…u32 schon als die echten Typen (nur f32 = double) – zu prüfen ist, ob der
  erzeugte Test die Argumente/Ergebnisse einer U32-Stelle als Int (Value::integer) übergibt und vergleicht: das passt, solange die Werte im
  Bereich liegen; ein Argument außerhalb (Zufallszahl > 2^32 an einen u32-Parameter) gibt der Quelle die gewickelte Zahl (C++-Umwandlung beim
  Aufruf) – der Blueprint wickelt am Eingang (fitted) genauso. Kein Befund, solange der Test die Zahl über die C++-Signatur reicht.
- Danach Schritt 5 (Schleife mit lokalem Wächter, ENTWURF 4 (b)) und (b) Struktur-Argumente ohne Heap (Bauplan oben).

## Nachfolger 6 (19.11., ab ~15:40) – E20 Schritt 4 gebaut
- Konverter (tools/cpp_to_blueprint.py): `WIDTH`/`WIDTHS`, `wire(t)`, `Unit.place(raw)` (Plätze: local, Parameter, f.value, Struktur-
  felder, Zustand, Konstanten, Paare über entry; `entry()` nutzt place → List<U8>). Intern speichert `self.types` den ERKLÄRTEN Typ
  (U8), `tp()`/`value()`/`conv()` geben Int (wire). `put(n, t)` statt `ex` an Speicherstellen (Zuweisung, Init, return, push_back,
  resize-Wert): `stored()` nimmt an der Wurzel IntegralCasts auf ≥ Breite weg (nicht über * oder <<) und markiert ein unsigned-32-+/−
  bzw. −x (`self.unwrapped`), dessen wrap32 fx dann weglässt. `wraps_sum`: `x += b`/`++x` auf U8…I32 als Add to Variable.
  Bytes: BYTE_FIELD in Breiten; bytes_pointer nach Zeigertyp (u8 → List<U8>); bit_cast/struct_bytes-Temp List<U8>.
- Engine: text.bytes/struct.bytes geben List<U8> (core_nodes `bytes_out`, blueprint.h); Api.pin/shape/tag kennen Breiten,
  Span/Zeiger über place → engine_api.py neu (jobs::submit/take …: List<U8>). engine_direct.h: Int-Pin nimmt Sized.
  Klasse: Index fester Breite (nodes.h requires … || is_sized<I>), `logic::pinned` in kern::formula, Glue held() für List<U8>-
  Felder, **Sized → i64 implizit** (vorher explicit: Funktionen mit i64-Parametern nahmen kein U32) + format_arg(Sized),
  operator== (Whole, ganzzahlig, andere Breite). Lokal geprüft: Klassen der 5 Vergleichsquellen (34 Blueprints) übersetzen.
- Werkzeug lokal: lesen.py → /home/claude/lese/l.klevel, dann `tools/blueprint_cpp.sh l.klevel out.cpp` und
  `clang++ -fsyntax-only -ferror-limit=0 -Ikern/include -Ikern/third_party/box3d/include -Ikern/games/spawncity/src out.cpp`
  (findet alle Klassenfehler auf einmal; der Werkzeugbau nach Engine-Änderung dauert ~18 min unter der Sperre).
- Belege: vergleich 163201 14/15 (terrain: Befund k, nicht p). Offen: engine 165514, konvert 165517, CI, Zeit-Test.
- 20.11. 02:45: ac4f6862 (Merge d5f527c5; engine_api.py neu: 815 Knoten). Danach: Kreuzung an Actor-Blueprints nimmt U8…I32 als int,
  Sort List über Breitenlisten, const void* = List<U8>. konvert 013136: 1643/1598 (Bezug 173143: 1699/1654); Rest = Maps mit
  Breitenlisten als Wert (stand/p.md). Offen: Map<K, List<U8>> in der Sprache, vergleich 001918, Zeit-Test, dann Schritt 5, (b).

### Nachfolger 7: Bauplan Map<K, List<U8>> (entschieden JA, Koordinator 02:50) – NICHT begonnen
Stand: work-p = 39ce0076 (work-zusammen 0f55c2f4 eingemischt, engine_api.py ohne Änderung, gepusht).
- Kodierung ist schon da: keyed_of/type_named schreiben "int_list:u8" (logic.cpp ~172/183), value_type liest es zurück.
- Leser: notation.cpp ~2681 verbietet `has_width(value)` → für Map-Werte zulassen (List<U8> … und wohl auch U8 … als Wert;
  `value.count` (Fixed) bleibt verboten). Schreiber/cpp_type der Map: kern::Map<K, List<Sized<u8>>> (prüfen: type_in für map, Glue held()).
- Interpreter: Speichern in einen Map-Wert wickelt (map.set / Zeile per Index, list.add/insert/resize auf einer Map-Wert-Liste:
  width_of_place/stored_as, core_nodes.cpp ~28; hold/fitted für Variablen vom Typ map: fit() in logic.cpp ~648 kennt nur Liste/Struktur
  → für map die Werte fitten). JIT über die CI. Klasse wickelt von selbst (Sized).
- Engine-Fall (logic_cases.h, wie Fall 1095/1130) + Fixpunkt + Doku blueprint_cpp.md (Abschnitt Breiten: „Map-Werte nie“ ändern),
  Editor-Typwahl (ParamDef::widths: „Map-Werte nie“ in core_nodes/logic_edit prüfen).
- Konverter: in Unit.btype (KEYED_KERN) die Übergangsregel `value[5:-1] in WIDTHS → List<Int>` (15201aee) zurücknehmen.
- Beleg: engine + konvert (Bezug: dann aktuelles work-zusammen ohne p; heute 173143: 1699/1654; meins ac4f6862: 1643/1598, Rest =
  35 Map-Wert-Fälle) + vergleich (nostr qr_code mvt city_jobs lamps) + Zeit-Test, dann BEREIT mit „x von 29 über 1,10“.
- vergleich 001918 (d38bc5ac) hing >2,5 h; 02:55 abgebrochen – `./mag.sh holen work-p-vergleich-001918-8866`, alle*.txt: welcher Test
  ZEIT/ABBRUCH → Befund an t1. BEFUND k (set_water_cell) ist mit k 3af75896 behoben.

## Nachfolger 8 (20.–22.11.) – Map-Werte fester Breite gebaut, Schritt 4 BEREIT (11968cee)
- Gebaut (6ae29b79): Leser (notation.cpp: Map-Werte dürfen eine Breite haben, Fixed bleibt verboten), type_from_text, has_fixed/fit für Maps
  (auch als Struktur-Feld; fit kopiert nur, wenn ein Wert nicht passt; Tiefe zählt nur Strukturen), map.set fittet den Wert, check prüft
  Map-Vorgaben (width_ok rekursiv), ParamDef::width_values (Value der Map-Knoten: U8 … I32 und ihre Listen), Glue/nativer Test: Map-Wert einer
  Breite = Sized<u8>, Proben einer Map gefittet; Web: lgToNamed int:u8 / int_list:u8; Doku blueprint_cpp.md „Ganze Zahlen fester Breite“.
  Fall fixed_width_map_values (logic_cases.h) – lokal Klasse == Interpreter == echtes C++ (p-tmp/w21/m: c.txt, cls.cpp, real.cpp; Werkzeug
  p-tmp/w21/bo.sh baut Logik-Objekte nach w21/o, mk/run wie w20). Leser-Fälle in test_main (Map<Int, U32> gut, U8 256 / I8 128 abgelehnt, from_outside).
- Konverter: Regel 15201aee zurück; c5bcd2fc/e8eb1e4f: Breiten in std::unique/remove(nur Wert desselben C++-Typs)/find/==, std::move-Feld, memset.
  Entscheidung: einzelne Map-Werte (kern::Map<K, int>) schreibt der Konverter weiter als Int (btype, Wickeln beim Speichern wie vor Schritt 4) –
  mit place() würden sie I32; offen als kleiner Folgeschritt (Folge: viele Maps ändern ihren Typ, alle Lesestellen über wire()).
- Belege: stand/p.md „BEREIT ZUM MERGEN: 11968cee“. Zeit-Test 21/29 (f32), 20/29 (64). Bench-Level/-Tests: $S/p/lvl6, $S/p/conv6 (konvert 100419,
  aus dem Zweig mag/ergebnis-<konvert> geholt: Commit „Level und Tests der Blueprints“). Bench-Bau $S/p/b6.
- Nächstes: Schritt 5 (Schleife mit lokalem Wächter) NICHT begonnen. Befund zum Ansatz: kern_pass() (blueprint.h ~310) liest/schreibt
  kern_world.counters[3] je Durchlauf über einen Zeiger; ein Aufruf im Rumpf liest denselben Zähler. Ohne Textänderung nicht exakt (ein
  verschachteltes kern_pass sähe einen veralteten Zähler). Weg laut Entwurf 4 (b): Schreiber erkennt am Graphen Rümpfe ohne Aufruf (kein Function
  Call/Call on/Event/Engine-Knoten, der weiterzählen kann) und schreibt `flow_for_local`/`loop_local`; Klasse: Rest = counters[1]-counters[3] beim
  Eintritt in eine lokale Variable, herunterzählen, beim Verlassen (auch break/return) zurückschreiben; bei 0 runtime->pass wie heute. Interpreter/
  JIT: dieselbe Bedeutung wie flow_for (zählen ohnehin zentral) – nur der Knoten/die Einstellung „local“ und check(), dass im Rumpf kein Aufruf steht.
  Erst messen (Projektion W: wrap_pi 1,50→0,85, gf_mul 2,9→1,8). Danach (b) Struktur-Argumente ohne Heap (Bauplan oben).

## Nachfolger 9 (22.11., ab 16:15) – Map<K, I32> + Schritt 5 gebaut (985ca4ab)
- Stand und Zahlen: stand/p.md „18:25 (22.11.)“. 4d56692b (Konverter: Map-Werte fester Breite über place()), 985ca4ab (Schritt 5: `.local()` /
  `loop_local()`, Schreiber entscheidet – Shape::loop_local; Klasse LocalPass; Fall loops_local).
- Offen: engine 163348 und konvert 162233 abholen (`./mag.sh holen …`); konvert ≥ 1756/1711 → BEREIT; dann Zeit-Test auf dem neuen Level
  (Klassen aus 985ca4ab; lvl/conv aus dem Ergebnis-Zweig holen wie bei 100419).
- Werkzeuge: p-tmp/chk/cnt.cpp (zählt lokale Schleifen, `./cnt <fall>` zeigt den Text), p-tmp/w9/nat/m.cpp (Teil des nativen Tests ohne bp_Engine:
  Objekte aus $S/shared-tmp/kern-tests nach `tests/run.sh`). Lokal: run.sh mit gcc (clang-18 -Werror scheitert an core_nodes.cpp 1609/1641, nicht von p).
- Weg (b) nicht begonnen (Befund in stand/p.md).
