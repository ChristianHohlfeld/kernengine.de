# Schaden messen: Vergleich gegen die ECHTE Quelle (Stufe 1, Agent projekt-zahlen1, 26.11.)

Werkzeug (work-projekt ab fe503eec): `KERN_TEST_SOURCE_NUMBERS=1` beim Konverter. Was geschrieben wird, entscheidet weiter der erste
Test (Zahlen der Sprache). Zusätzlich entsteht je Blueprint test_<x>_source.cpp (die Quelle, wie sie ist: f32 bleibt f32, Typen der
Engine). Bauen und fahren: tools/converter_cases/compare.sh. Je Funktion auswerten: tools/source_numbers.py <tests> <ausgabe>.
CI: `gh workflow run ci.yml --ref work-projekt -f probe="src/a.cpp …" -f source_numbers=1` (leeres Level und Projekt; die Tests werden
gebaut und gefahren, die Tabelle steht am Ende des Protokolls des Jobs probe).

## Fälle (tools/converter_cases, Job converter-cases)
| Fall | geschrieben | wie bisher | gegen die echte Quelle |
|---|---|---|---|
| numbers_same: hash32 (u32-Umbruch), lcg (u64) | 2/2 | gleich | gleich (Gate-Fall, bleibt grün) |
| numbers_f32: product_is_exact (f32-Zwischenwert a*b) | 1/1 | gleich | **abweichend 175/300** (Quelle false, Blueprint true) |
| numbers_sum: past_one_after (f32-Summe 1/60 mit Schwelle) | 1/1 | gleich | **abweichend 1/300** (60 Frames: Quelle false, Blueprint true) |
| numbers_i64: mixed_from_the_start (long long über 2^53) | 1/2 – wartet „exact up to 2^53“ | – | – |
Die letzten drei sind „erwartet rot bis Zahl-Typen“. Wird einer richtig, ist das ein Fehler der Liste (Job rot).

## Murmel (src/murmel.cpp + main.cpp, lokal, $S/mm1; Ausgabe $S/murmel_numbers.txt)
14/14 geschrieben, 13 in Tests. Wie bisher: 13/13 gleich. Gegen die echte Quelle: **10 gleich, 3 abweichend**:
| Funktion | Aufrufe | größte Abweichung | Ursache |
|---|---|---|---|
| Murmel::update | 256/300 | 3,05e-5 an gewonnen_seit_ (975.7294921875 gegen 975.72952270507812) | f32-Summe (`gewonnen_seit_ += frame.dt`: die Quelle rundet jeden Schritt auf f32, der Blueprint hält f64). Dazu Aufrufe der Engine mit Literal: ui_menu(…, 0.6f) und ui_bar(…, 4/6.f). Die Quelle übergibt 0.60000002384185791, der Blueprint 0.59999999999999998: f32-Zwischenwert bzw. Literal, das der Konverter zu f64 macht (der Stellvertreter schreibt den Wert vor der Tür auf; die echte Engine bekäme beide als f32 gleich) |
| Murmel::punkte | 79/300 | 1,5e-5 (309.19744873046875 gegen 309.19746398925781) | f32-Zwischenwert/-Summe im Rückgabewert |
| Murmel::start | 300/300 | 2,9e-8 an kamera_.fov (1.0471975803375244 gegen 1.0471975511965976) | f32-Zwischenwert: `fov = 60 * kPi / 180` ist in der Quelle ein f32, im Blueprint f64 |
Dazu passt der Spieltest: App als Blueprint 348/770 gegen C++ 349/772 (gewonnen_seit_).

## SpawnCity-Proben (CI, Commit 0d4c80e2, leeres Level und Projekt; Testrahmen liest Engine-Bibliotheken)
Gezählt: jede Funktion in einem Test (auch die der Header-Blueprints, die die Probe mitschreibt: common, engine_math_math …,
darum stehen common/engine_* in mehreren Zeilen). „nicht verglichen“: Die Kopie der Quelle mit ihren eigenen Zahlen kompiliert für
diese Funktion nicht; die Gründe stehen unten.
| Probe | CI | geschrieben | in Tests | gleich wie bisher | gleich gegen echte Quelle | abweichend | nicht verglichen |
|---|---|---|---|---|---|---|---|
| nostr.cpp (Ganzzahl/Bit, Hash) | 45806226369 | 59/59 | 59 | 59 | 47 (nostr_cpp 38/38) | 10 (alle in common) | 2 |
| car.cpp | 45806217667 | 50/50 | 51 | 51 | 22 | 24 (car_cpp 9, car 4, common 10, rigid_spec 1) | 5 |
| drone.cpp (+ Firmware DroneFC) | 45806221721 | 120/120 | 114 | 114 | 55 | 53 (Firmware 33, drone_cpp 9, common 10, rigid_spec 1) | 6 |
| road_graph.cpp | 45806230739 | 59/61 | 52 | 52 | 18 | 12 (road_graph_cpp 2, common 10) | 22 |
| tiles.cpp | 45806235132 | 48/50 | 51 | 51 | 27 | 22 (tiles_cpp 11 von 24, common 10, math 1) | 2 |
| combat_rules.cpp + wanted.cpp | 45806240065 | 30/89 | 33 | 33 | 13 | 18 (combat_rules_cpp 7 von 11, common 10, blueprint_call 1) | 2 |
| mp_rules.cpp + look.cpp | 45806244534 | 85/113 | 86 | 86 | 36 | 30 (look_cpp 11 von 19, mp_rules_cpp 6 von 29, look 2, common 10, blueprint_call 1) | 20 |
| game.cpp + main.cpp | 45802042821 (fe503eec) | 339/440 | 287 | 287 | 157 | 59 | 71 (davon 27 net_session, 15 Life …: Helfer draw_<Klasse> mit float als double - behoben in c20b18d5, neue Probe 45808702551) |
**Wie bisher ist ausnahmslos alles gleich (Gate grün). Gegen die echte Quelle weicht in jeder Probe mit f32-Rechnung etwa die Hälfte
der gemessenen Funktionen ab. Ganzzahl-/Bit-Funktionen weichen nirgends ab** (nostr_cpp 38/38, decode_sbus, SbusParser_feed,
encode/decode_navigation_wire, mvt, tiles-Indizes, hash32/lcg).

### Die größten Abweichungen und ihre Ursache
| Funktion | Aufrufe | größte Abweichung | Ursache |
|---|---|---|---|
| look_cpp Shake_sample | 162/300 | 17,1 (angles.x −13947,8 gegen −13930,7) | f32-Summe (Phase wächst, sin der großen Phase verstärkt den Fehler) |
| road_graph_cpp RoadGraph_pursuit_point | 56/300 | 3,98 (x −2,94 gegen 1,03) | f32-Zwischenwert kippt eine Verzweigung (anderer Knoten des Wegs) |
| rigid_spec box_inertia | 275/300 | 2,77 bei 3,8e7 | f32-Zwischenwert (Produkt großer Kanten in f32) |
| look_cpp follow_phase | 197/300 | 1,0 (1 gegen 0) | Schwelle kippt (`d -= std::round(d)`, `< 0.0005f` in f32) |
| Firmware Attitude_run | 271/300 | 0,498 bei −57846 | f32-Summe (Integration) |
| tiles_cpp cut_side | 123/300 | 0,25 bei −4114814 | f32-Zwischenwert (große Koordinate, ulp 0,5) |
| look_cpp Kern_intensity | 121/300 | 0,083 bei 807812 | f32-Zwischenwert |
| engine_math_math Mat4_mul | 300/300 | 0,031 | Engine-Mathe nachgerechnet statt gerufen: Blueprint-Graph rechnet die 16 Summen in f64, die Engine (und die Quelle) in f32 |
| Firmware Runtime_step_command | 20/300 | 0,0165 | Schwelle/Klemme kippt durch f32-Zwischenwert |
| car_cpp Car_wheel_speed | 191/300 | 0,0135 | f32-Zwischenwert |
| common cast | 105/300 | 0,0032 bei −82663 | f32-Zwischenwert |
| common wrap_pi | 201/300 | 6,8e-4 | f32-Summe (`a -= 2 * kPi` in einer Schleife, kPi als f32) |
| common slerp, qnormalize, heading_of, Rng_range, rate_limit | 150–299/300 | 1e-7 … 7e-5 | f32-Zwischenwert; slerp/heading_of auch Bibliothek: die Quelle ruft std::acos/std::atan2 (glibc), der Blueprint kern::dm (musl) |
| Firmware *_new / *_reset | 300/300 | 2,6e-11 (kp 0.003f gegen 0.003) | f32-Literal: der Konverter schreibt `0.003f` als 0.003 |
| combat_rules_cpp rounds_of, run_of …; blueprint_call decimal | 154–296/300 | 2,9e-5 (680.0795 gegen 680.0795288) | Brücke der Quelle: `rules::decimal(f32)` macht aus einem f32 die Dezimalzahl, für die es steht (f64). Der Blueprint gibt das f32 selbst |
| combat_rules_cpp cell_rand | 292/300 | 3e-8 | Ganzzahl-Hash genau gleich, die Division am Ende in f32 (f32-Zwischenwert) |

### Einordnung der Ursachen (alle Proben)
1. **f32-Zwischenwert** (die meisten): Die Quelle rundet jedes Zwischenergebnis auf f32, der Blueprint rechnet in f64 und rundet erst
   am Platz. Typisch 1e-7 … 1e-4 relativ. An großen Koordinaten (tiles, rigid_spec) sind es absolut 0,25 … 2,8.
2. **f32-Summe**: Ein Fehler wächst über die Schritte (Integratoren, Phasen, gewonnen_seit_, wrap_pi-Schleife): bis 17 (Shake_sample).
3. **Kippende Verzweigung/Schwelle** (Folge von 1/2): selten, aber dann ein ganz anderes Ergebnis (follow_phase 1 gegen 0,
   pursuit_point, past_one_after 60 Frames, Runtime_step_command).
4. **Engine-Mathe nachgerechnet statt gerufen**: Funktionen aus Engine-Headern (engine_math_math: Mat4_mul, Quat …) sind Graphen der
   Sprache in f64, die Engine rechnet sie in f32 (Mat4_mul 0,031). Dazu die Bibliothek: Die Quelle ruft std::sin/atan2/acos (glibc
   nativ), der Blueprint kern::dm (musl).
5. **f32-Literal**: `0.003f`, `0.6f`, `-0.8f` werden zu f64-Literalen (Abweichung 1e-11 … 1e-8 immer, in jedem Aufruf).
6. **Brücke der Quelle** (rules::decimal f32→Dezimal-f64): Die Quelle passt ihre Zahlen heute an die Sprache an. Mit den Zahl-Typen
   wird das überflüssig.
7. **Ganzzahl**: keine Abweichung gefunden (u32-Umbruch, u64, Bitfelder, Hashes). Offen ist nur long long über 2^53 (wartet, numbers_i64).
8. **Stellvertreter-Artefakt** (nicht wirklich): Ein Engine-Aufruf mit f32-Parameter (ui_menu(…, 0.6f)) zeigt im Stellvertreter 0.6
   gegen 0.60000002. Die echte Engine bekäme beide als f32 gleich. Gezählt wird es trotzdem als Abweichung, weil der Stellvertreter den
   Wert vor der Tür aufschreibt.

### Was der Messung noch fehlt
- 71 nicht verglichene Funktionen in game+main (Helfer draw_<Klasse>): behoben in c20b18d5, neue Probe 45808702551; life.cpp 45808707629.
- Weiter offen (nicht verglichen): `kern::Quat::axis_angle` in der Kopie (common: der Test hat eine eigene Quat, die Quelle ruft die
  statische Funktion der Engine), `no type named Session in namespace net` (net_session), draw_CityStream/Terrain/Interiors (Zustände
  mit Engine-Objekten), 4× „no matching function for call to same“.
- Beim Bau der Proben kompiliert level_blueprints.cpp nicht („no viable conversion from ActorHandle to Object<kern::Renderer> /
  Object<kern::physics::World>“, blueprint_cpp). Gelinkt wird dann ohne diese Klassen. Die Vergleiche laufen trotzdem, nur Tests, die eine
  solche Klasse binden, nicht. Das ist ein Befund für Konverter/Klassen.

## Abnahmeliste (für den Umbau auf work-zahlen)
### (a) muss mit den Zahl-Typen der Sprache verschwinden: der ehrliche Durchgang zeigt danach 0 Abweichungen
- f32-Zwischenwert (Ursache 1): alle Funktionen der Tabellen oben, die keine Bibliotheks-Funktion rufen. Beispiele: product_is_exact
  (numbers_f32), Murmel punkte/start, tiles_cpp cut_side/clip_*, rigid_spec box_inertia, car_cpp Car_wheel_speed, common cast/rate_limit/
  Rng_range/qnormalize, Firmware PT1_run/Filters_run/mag/decode_icm42688_registers, combat_rules_cpp cell_rand.
- f32-Summe (2): past_one_after (numbers_sum), Murmel update (gewonnen_seit_), Attitude_run, Runtime_calibrate, PID_run, common wrap_pi,
  look Shake_sample.
- kippende Schwelle als Folge davon (3): follow_phase, RoadGraph_pursuit_point, Runtime_step_command.
- Engine-Mathe nachgerechnet statt gerufen (4, der Teil ohne libm): engine_math_math Mat4_mul und die anderen Funktionen der
  Engine-Header (als Knoten der Engine in f32 gerufen).
- f32-Literal (5): Firmware *_new/*_reset (0.003f), command (-0.8f); Murmel ui_menu(…, 0.6f)/ui_bar (der Stellvertreter-Wert, 8).
- long long über 2^53 (numbers_i64): geschrieben und gleich.
Abnahme: converter_cases numbers_f32/numbers_sum/numbers_i64 werden richtig (und gehen zu numbers_same), die Proben oben mit
source_numbers=1 zeigen 0 „differs“ in den Funktionen dieser Liste. Murmel: alle Wege 349/772.
### (b) liegt an der Quelle oder am Bau: je ein eigener Schritt
- B1 libm: Die Quelle ruft std::sin/cos/atan2/acos/hypot … aus glibc (nativ) bzw. wasi-libc (Wasm), Blueprint und Engine kern::dm
  (musl). Betroffen: alle Funktionen mit diesen Aufrufen (z. B. common slerp, heading_of; 807 Stellen in 56 Dateien, Firmware 16).
  Schritt: der Bau bindet die musl der Engine unter den Standardnamen, mit -fno-builtin-<name> (Entwurf in blueprint_cpp.md
  „Mathe-Funktionen“). Bis dahin kann eine Abweichung aus B1 eine aus (a) überdecken: Abnahme von (a) für solche Funktionen erst
  nach B1, oder auf einem Bau mit B1.
- B2 Brücke rules::decimal (f32 → die Dezimalzahl als f64) in der Quelle: blueprint_call decimal und die Werte, die combat_rules_cpp
  damit setzt (rounds_of, run_of, run_of_2, take, take_2, take_3). Schritt: Mit f32 auf dem Draht ist die Brücke überflüssig, also
  die Brücke im Spiel entfernen (Spiel, nicht Engine) und die Aufrufe der Regel-Blueprints mit f32 rufen.
- B3 fehlende Mathe in musl_math (sinh, tanh: 3 Stellen; der Konverter schreibt heute eine Formel aus exp). Schritt: aus musl
  nachziehen, Formel weg, eine fehlende Funktion ist ein Fehler des Konverters.
- B4 JIT ≠ Interpreter (mod, round, min/max bei NaN/−0; blueprint_cpp.md „Determinismus“). Das misst der Vergleichstest nicht (er
  fährt Interpreter und Klasse). Schritt: der JIT ruft dieselbe Rechnung, dazu ein Engine-Test an den Rändern.
- B5 NaN/Inf werden nicht verglichen (converted::defined). Schritt: bitgenau vergleichen statt auslassen.
- B6 GCC (Regel: Quellen des Teilsatzes und die Engine werden mit clang gebaut; ein Bau mit GCC ist für die Bitgenauigkeit nicht
  belegt. GCC schreibt std::pow(x, 2) trotz -fno-builtin-pow selbst als x*x, weil libstdc++ __builtin_pow ruft; der Engine-Test
  math_of_the_standard_library_is_the_engines nennt es). Wo GCC heute im Spiel ist:
  - CI-Job `native`: `CXX=g++ CC=gcc kern/tests/run.sh`. Die Engine-Tests, darunter der Fingerabdruck des Lehrers
    (determinism_fingerprint.txt, die goldenen Werte gelten für GCC x86, clang ARM und Wasm).
  - Vorgaben `c++`/`cc` (auf Ubuntu GCC), wenn CXX/CC nicht gesetzt sind: kern/tests/run.sh:37, kern/tools/build_native.sh:32,
    kern/tools/lehrer.sh:15, kern/games/murmel/tests/engine.sh:6, kern/games/spawncity/tools/all_program.sh:21,
    kern/games/spawncity/tools/level_templates.sh:11, tools/blueprint_cpp.sh:16 (HOST_CXX: das Werkzeug, das die Klassen der Blueprints
    schreibt, und der Klassen-Bau des Konverters). Die CI setzt in den Spiel-Jobs clang, lokal und auf dem anderen Rechner
    (lauf2.sh: c++ = GCC 13.3) nicht.
  - Nicht umgebaut, nur benannt.
- B7 Mathe-Funktionen ohne Knoten der Sprache (Liste: $S/zahlen_mathe_knoten.md; gebaut auf work-zahlen): hypot 239 Stellen
  (Formel sqrt(a²+b²)), ldexp 4 (a·pow(2,n)), asinh 2, tanh 2, sinh 1. sinh/tanh/cosh/asinh warten auf work-projekt ab jetzt
  mit Ort („the engine has no tanh yet“, die musl-Quelle fehlt: B3). Betroffen: car.cpp:422 (tanh), drone.cpp:15 (tanh),
  tiles.cpp:30 (sinh), terrain.cpp:39 und tiles.cpp:23 (asinh).
- B2 Brücke rules::decimal (nur beschrieben, Entscheidung bei main): kern/games/spawncity/src/blueprint_call.h:27. Was sie tut: Ruft
  das Spiel einen Regel-Blueprint (Call on Actor, wanted/combat_rules/controls …), gibt es eine f32-Zahl seines Zustands als die
  KÜRZESTE DEZIMALZAHL weiter, die genau dieses f32 ist (0.1f → 0.1 statt 0.100000001490116). Warum: Die Sprache rechnet in f64, und
  die Regeln nennen ihre Schwellen dezimal (0.1, 2.6). Ein f32 0.1f läge als f64 knapp über der Schwelle 0.1, auf der es in 32 Bit
  genau steht; mit decimal() antwortet jeder Vergleich mit einer Dezimal-Schwelle wie in 32 Bit (eineindeutig, ordnungstreu).
  Warum es abweicht: Die Quelle rechnet decimal() (eine Schleife über Dezimalstellen mit round/trunc in f64) und gibt ein f64, das
  KEIN f32 ist (680.0795). Der Blueprint derselben Funktion bekommt den Wert schon als Float der Sprache und gibt das f32 selbst
  (680.0795288). Dazu schreiben die Aufrufer (combat_rules_cpp rounds_of, run_of …) den Wert in f32-Felder zurück.
  Umfang: 235 Aufrufe in 16 Dateien. Mit f32 als Typ der Sprache (Stufe 2-3) wird die Brücke überflüssig: Der Blueprint bekommt das
  f32 und vergleicht es in f32 mit 0.1f, wie die Quelle. Vorschlag: im Spiel entfernen, sobald die Regel-Blueprints f32-Eingänge
  haben (eine Änderung des Spiels, nicht der Engine).

## Nach B1 (Commit 63416d88: musl unter den Standardnamen, B5 NaN/Inf verglichen, float-Helfer behoben, sinh/tanh/asinh warten)
| Probe | CI vorher (0d4c80e2) | geschrieben vorher → nach B1 | in Tests nach B1 | gleich wie bisher | echt gleich vorher → nach B1 | echt abweichend vorher → nach B1 | nicht verglichen vorher → nach B1 |
|---|---|---|---|---|---|---|---|
| nostr | 45806226369 → 45810827614 | 59 → 59 | 59 | 59 | 47 → 48 | 10 → 11 | 2 → 0 |
| car | 45806217667 → 45810819068 | 50 → 49 (tanh wartet) | 50 | 50 | 22 → 25 | 24 → 25 | 5 → 0 |
| drone (+Firmware) | 45806221721 → 45810823143 | 120 → 115 (tanh wartet, mit Rufern) | 109 | 109 | 55 → 58 | 53 → 51 | 6 → 0 |
| road_graph | 45806230739 → 45810831884 | 59 → 59 | 52 | 52 | 18 → 38 | 12 → 14 | 22 → 0 |
| tiles | 45806235132 → 45810836505 | 48 → 45 (sinh/asinh warten) | 48 | 48 | 27 → 29 | 22 → 19 | 2 → 0 |
| combat_rules+wanted | 45806240065 → 45810841877 | 30 → 30 | 33 | 33 | 13 → 14 | 18 → 19 | 2 → 0 |
| mp_rules+look | 45806244534 → 45810846057 | 85 → 85 | 86 | 86 | 36 → 45 | 30 → 41 | 20 → 0 |
| game+main | 45802042821 → 45810814484 | 339 → 339 | 287 | 287 | 157 → 231 | 59 → 56 | 71 → 0 |
Lesart:
- Wie bisher ist weiter ausnahmslos alles gleich, auch jetzt, wo NaN/Inf/-0 mitverglichen werden (B5: kein neuer Befund in den
  Proben und in Murmel).
- „nicht verglichen“ ist überall 0 (Zustands-Helfer mit float, c20b18d5): Was dazukam, teilt sich in gleich und abweichend.
- B1 allein macht kaum eine Funktion gleich (slerp, heading_of, qnormalize, wrap_pi weichen weiter ab). Die Quelle ruft jetzt dieselbe
  musl wie der Blueprint, aber in f32 (atan2f), der Blueprint in f64 (atan2): Der Rest ist Sorte (a) und verschwindet erst mit den
  Zahl-Typen. B1 ist die Bedingung dafür, dass (a) danach überhaupt auf 0 gehen KANN: Ohne B1 bliebe glibc gegen musl.
