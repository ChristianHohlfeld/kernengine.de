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
