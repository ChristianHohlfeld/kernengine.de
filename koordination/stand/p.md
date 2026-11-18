# Stand Paket p (Leistung der exportierten Klasse) – 18.11.2026, 23:05

Zweig work-p, letzter Commit a53ce271 (gepusht; dazu Switch on Text ohne Liste, alt/neu lokal gleich über 121 Paare).
- BEWIESEN: E2.1 Schritt 1 (bis 2416329): mag `work-p-engine-221509-17831` engine rc=0 (197 ok, blueprint_native_test 55 Blueprints/14102 Aufrufe,
  game_native_test ok). Probe `work-p-probe-221512-17868` (alle Quelltexte, vor der 22:25-Regel) abgebrochen – überholt durch den Kopf.
- LÄUFT: `work-p-vergleich-230429-15316` (gestartet 23:14 auf 8a028f9c: E2.1, E2.2, E2.1 Schritt 2, Fix text_format, E3-A) über qr_code, settings,
  nostr, controls, combat_rules. NICHT darin: E10 + Leser-Fix (Commit vor 6c7b9c5f). Danach `engine` auf dem Kopf. CI: run 44407872390 (work-p, 63467c0b).

## Beleg vergleich `work-p-vergleich-230429-15316` (8a028f9c; 23:29)
convert rc=0, bau rc=0, alle rc=0: **12 ok, 0 FAIL, 0 ABBRUCH, 0 LECK, 0 ZEIT von 12 – in beiden Durchgängen (Stellvertreter und SC_ENGINE=real)**,
alle-abweichungen.txt und alle-real-abweichungen.txt leer. Tests: blueprint_call, combat_rules_cpp, common, controls, controls_cpp, engine_math_math,
engine_physics_physics, nostr_cpp, qr_code, qr_code_cpp, settings, settings_cpp. → E2.1, E2.2, E2.1 Schritt 2 Bit für Bit gleich.
E3-A Wirkung: cost-Zeilen controls_cpp 44 → 10, combat_rules_cpp 69 → 11 (vorher: $S/t1/suite203000_cost_note.txt). Was bleibt, sind Aufrufe mit
Struktur-Argumenten/-Ergebnissen (z. B. WorldLook_add: controls_Look als logic::Value – Felder im Vektor des Value), nicht der Aufrufweg.
Fix text_format: hier NICHT belegt – der Stand enthält ks Umgehung 12e3da24 (Text in eigener Variable); belegt lokal mit ASan (alter Header UAF,
neuer sauber) und durch den Engine-Fall format_text_of_value. Laufend: `work-p-engine-232958-27563` (engine auf 6c7b9c5f).

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
