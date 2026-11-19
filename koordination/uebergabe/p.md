# Übergabe Paket p – 19.11.2026, 09:00 (Nachfolger 2)
Maßgeblich: $S/stand/p.md (oben: E17-Messung, Entwurf 4, Ursachen je Fall, Tabelle mit Versuchsspalten, Befund für e).
Zweig work-p, Kopf 9c315bf2 (gepusht). Darin: 50e21923 Text::Appending (settings_escape 7,59→3,37), 9c315bf2 E17 (Zeit-Test, beide Spalten).
Belege: work-p-engine-075125-9242 (auf 50e21923, lief um 09:00 noch – abholen: ./mag.sh holen …); danach EIN Auftrag
`./mag.sh vergleich origin/work-p src/settings.cpp` (Text::Appending), dann `./mag.sh engine origin/work-p` auf dem Kopf (E17 berührt nur
die Bench; die Engine-Suite baut sie nicht – dennoch einmal). Erst dann „BEREIT ZUM MERGEN 9c315bf2“ an main.
Entschieden (Koordinator 07:55/08:10/08:45): E17 JA (gebaut), Punkt 2 JA (bauen), Punkt 3 JA (bauen); Entwurf 4 (feste Breiten) bei Chris – NICHT bauen.
Paket e (E16) besitzt „wo der Wert eines Schritts steht“ (nNN als Mitglied) – nicht anfassen; nach seinem Merge neu messen.

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
