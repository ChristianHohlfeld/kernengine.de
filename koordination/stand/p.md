# Stand Paket p (Leistung der exportierten Klasse) – 18.11.2026, 21:35

Zweig work-p, letzter Commit 7aacebf (gepusht; origin/work-zusammen 8874e79 eingemischt).
IN ARBEIT: Beweis für a–c läuft auf mag (`work-p-engine-212302-7164`, engine auf 7aacebf, Start 21:23). Danach: Spiel-Suite-Probe,
die neuen cost:-Zeilen („instead of 0“, $S/t1/suite203000_cost_note.txt), Entwurf zu E, Zeit-Test.

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
