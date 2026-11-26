# Zahl-Typen der Sprache = die des C++-Subsets – Bestand und Entwurf (Agent projekt, 26.11.; NICHT gebaut)

Korrektur des Besitzers (10:49): bitgenau heißt, nichts wird umgedeutet oder gerundet, was die Quelle nicht selbst tut.
Die Sprache trägt die Zahl-Typen der Quelle; Umwandlungen nur, wo die Quelle sie hat (ausdrücklich oder als
ImplicitCastExpr von clang). Der Konverter schreibt sie als Knoten.

## Ist (work-projekt 64919bc5)
- **Value** (kern/logic/logic.h): `f64 n` für Float, `i64 i` für Int. Ein Int ist aber `normal()`: bis 2^53 genau, darüber
  der nächste double, gekappt an ±(2^63−1). `whole_add` geht ab 2^52 über double. Dazu Int64 (u64-Bits, eigene Knoten) und
  Vector (3 × f32). Pin-Typen für Zahlen: Bool, Int, Float, Int64, Vector (+ Listen/Maps davon). Breiten nur am **Platz**:
  U8…I32 (`"of":"u32"`, `logic::Sized<T>`): gespeichert wie C++, auf dem Draht ein Int.
- **Interpreter/JIT**: Variablen und Werte als double (Int eingeschlossen). **Klassen** (blueprint.h): `Float = f64`,
  `Int = logic::Whole`, `Vector = Vec3`. **Leser/Schreiber**: Float-Literale als kürzeste double-Form, Int als ganze Zahl,
  Int64 als `Int64{0x…}`. **Editor/Web**: Zahlen in JS als double. **Speichern/Laden**: Int als Text (genau).
- **Konverter** (cpp_to_blueprint.py NUMBER/ALIAS): float, double, f32, f64 → Float (64 Bit, still verbreitert); int, long,
  long long, short, char … → Int; u64/i64/uint64_t → Int64; u8…u32-Plätze → U8…U32. f-Literale fallen weg (`1.0f` → `1.0`).
  Wo die Quelle „in so many words“ auf 32 Bit bringt, nimmt die Formel `single`. Ein Literal oder Umbruch über 2^53 bei Int wartet
  („a 64-bit whole number that wraps around or is shifted (a Blueprint's are exact up to 2^53)“).
- **Testrahmen** (kern/include/kern/test/converted.h `wide`, TestWriter.widen): Die Quelle wird im Vergleichstest UMGESCHRIEBEN
  (`float` → double, f32 = double, f-Suffix weg, Vec3-Komponenten als Num32). Verglichen wird Blueprint gegen die verbreiterte
  Quelle, **nicht gegen C++**. Deshalb sind die Vergleiche grün, wo nativ etwas anderes herauskommt.
- **Engine-Tür** (engine_api.py, engine_calls.gen): Pins Float (f64) → f32-Parameter der Engine per static_cast (gerundet,
  implizit). Funktionen aus Engine-Headern (87, davon 44 mit Zahlen; engine_math_math 44) sind Graphen der Sprache (64 Bit),
  nicht die Funktion der Engine ($S/zahlen_engine_funktionen.md).

## Lücken, belegt (Fälle unter $S/typcase/typen.{h,cpp}, Konverter work-projekt, alle 4 „written“ und „compared“ = grün)
| Fall | C++ nativ | Blueprint / Vergleichstest | warum grün |
|---|---|---|---|
| `past_one_after(60)`: f32-Summe von 1/60 (wie Murmels gewonnen_seit_) | false (erst nach 61) | true (64 Bit) | Test-Kopie `s += 1.0 / 60.0` (widen) |
| `product_is_exact(0.1f, 0.1f)`: f32-Zwischenwert a*b | false | true (Formel in double) | Test-Kopie f32 = double |
| `hash32(h)`: u32-Umbruch | wie u32 | `mul32` in der Formel, U32-Platz: **gleich** | (keine Lücke; Weg über Spezial-Funktion) |
| `lcg(s)`: u64 | wie u64 | `int64_mul/int64_add` auf Int64: **gleich** | (keine Lücke, aber eigener Typ Int64) |
| `long long` über 2^53 (FNV, $S/intcase) | Umbruch wie i64 | **wartet**, bzw. `mix` in double | normal() |
Murmel: App als Blueprint 348/770 gegen C++ 349/772 (gewonnen_seit_). Ohne Zeile für Zeile echte C++-Quelle im Test
bleiben solche Fälle grün. **Die erste Stufe muss deshalb der Testrahmen sein:** Er vergleicht gegen die Quelle, wie C++ sie
rechnet (ohne widen). Dann werden die Fälle rot, und das ist der richtige Ausgangspunkt. Diese Fälle sind NICHT in
converter_cases eingehängt: Heute wären sie dort grün (der Rahmen sieht die Lücke nicht), also kein Beleg. Mit Stufe 1 werden sie als
„erwartet rot bis Zahl-Typen“ eingehängt.

## Soll
- **Zahl-Typen der Sprache = die des Subsets**: bool, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64. Jeder ist ein Pin-Typ
  (auf dem Draht, nicht nur am Platz). Text-Form in C++ sind die C++-Typen selbst (`f32 x = 0.5f;`, `u32 h`).
- **Operationen getypt** wie C++ nach den üblichen arithmetischen Umwandlungen: f32 + f32 rechnet f32, u32 * u32 bricht bei 2^32
  um, i32-Überlauf ist UB in C++ (Konverter: Fehler mit Ort oder ausdrücklich Umbruch-Knoten, nicht still), i64/u64 wie
  Zweierkomplement, Shifts/Bit-Operationen auf der Breite, Division/Modulo auf null zu, durch 0 definiert (Knoten: 0, wie heute;
  in C++ UB → Konverter-Fehler, wo nicht ausgeschlossen). NaN/Inf wie IEEE.
- **Umwandlungen sind Knoten**: jede ImplicitCastExpr (IntegralCast, FloatingCast, IntegralToFloating, FloatingToIntegral,
  Promotion) wird ein sichtbarer Knoten `f32 → f64` usw. Kein Draht wandelt still um. Float→Int auf null zu; außerhalb des
  Bereichs in C++ UB → Knoten legt fest (Ende, NaN → 0) und der Konverter meldet.
- **Engine-Funktionen gerufen** (Entscheidung (i)): Die Engine rechnet f32, die Sprache jetzt auch. Vec3 = 3 × f32, Mat4 = 16 × f32
  ohne Wandlung.
- **Vorgaben im Editor**: „Float“ = **f32** (wie die Engine, Grafik, Physik: der Normalfall eines Spiels, halber Speicher, und
  die Werte der Engine gehen ohne Umwandlung hinein) und „Int“ = **i64** (kein Überlauf im Alltag, Zähler, Ids, Hashes). Die übrigen
  Breiten sind in einem Typ-Menü wählbar („Zahl: f64“, „Ganzzahl: u8 … u64“). Der Editor zeigt die Breite am Pin (Farbe gleich,
  Kürzel).
- Vector bleibt Engine-Typ (kern::Vec3).

## Weg (Stufen, je grün)
1. **Testrahmen ehrlich**: TestWriter vergleicht gegen die Quelle ohne widen (echte f32/u32/i64-Arithmetik von C++). Neue Fälle
   (typen, bigint, Murmel-Summe) als „erwartet rot bis Zahl-Typen“ getrennt in converter_cases/run.sh geführt; der Gate-Teil
   bleibt grün. Für SpawnCity wird der Vergleich vieler Funktionen rot: Das ist der wahre Stand. Er darf das Gate nicht rot
   machen, deshalb im vollen Lauf als eigene Zahl führen (vorher/nachher).
2. **Value und Pin-Typen**: Value hält eine Zahl mit ihrem Typ (kind + 64 Bit Bits). PinType bekommt f32, f64, i8…u64
   (Int64 und die „Breite am Platz“ gehen darin auf). Leser/Schreiber: `"type":"f32"` … (altes `"float"` = Fehler mit Ort, keine
   Rückwärtskompatibilität). Engine-Test je Operation × Typ gegen C++.
3. **Knoten**: arithmetische, vergleichende und Bit-Knoten je Typfamilie (Templates in nodes.h: eine Implementierung für
   Interpreter und Klasse). Umwandlungsknoten. Interpreter: Werte als getypte Bits. JIT: getypte Wege (f32-Ops in wasm/x86 da).
4. **Klassen** (blueprint_cpp): die C++-Typen direkt (`f32`, `u32` …), kein `Whole`.
5. **Konverter**: NUMBER/ALIAS → die Typen selbst. ImplicitCastExpr → Umwandlungsknoten. Wartegrund „exact up to 2^53“ und
   `single`/`mul32`/`wrap32` entfallen. widen ist schon in 1. weg.
6. **Engine-Tür/-Tabelle** (engine_api.py): Pins mit den Typen der Signatur (f32 bleibt f32). Engine-Header-Funktionen als
   Knoten ($S/zahlen_engine_funktionen.md).
7. **Editor/Web**: Typ-Menü, Pin-Kürzel, Werte f32 in JS über Math.fround, i64/u64 als BigInt bzw. Text.
8. **Neu erzeugen**: Engine-Bibliotheken (engine_api.py), Levels/Projekte aller Spiele (Murmel: blueprints.sh --write,
   convert.sh --write; SpawnCity: voller Konverter-Lauf), Spieltests mit den Werten der Quelle (Murmel wieder 349/772 für alle Wege).
- **Risiken**: Stufe 2+3 berühren jedes Level (Format), den JIT und die Web-Seite. Viele SpawnCity-Funktionen werden im
  ehrlichen Vergleich (1.) erst rot. Die Zahl der Pin-Typen steigt, und die Knoten-Bibliothek darf dabei nicht explodieren
  (Templates, eine Familie je Operation).
- **Geschwindigkeit**: Skalar sind f32 und f64 auf x86-64/wasm gleich schnell. Der Interpreter wird durch getypte Werte eher
  langsamer (Dispatch nach Typ), die Klassen eher schneller (f32 halbiert Speicher, SIMD-Breite doppelt). Gemessen ist das noch
  nicht: in Stufe 3 messen, an einem kleinen Fall (Schleife 10^7 f32-Summen in Interpreter und Klasse, f64 gegen f32).

## Float bitgleich: was die Engine schon festlegt (Kurzprüfung; vom Nachfolger zu vertiefen)
- `-ffp-contract=off` überall (kern/build.sh für C und C++, kern/tools/build_native.sh, die Testbauten): keine FMA-Kontraktion,
  nativ wie Wasm. x86-64 und wasm rechnen float in SSE bzw. f32-Befehlen, also ohne höhere Zwischengenauigkeit
  (FLT_EVAL_METHOD 0). Für f32-Ausdrücke heißt das: C++ rundet jeden Zwischenwert auf f32, und der Blueprint muss es ebenso tun.
  Ein f32-Knoten rechnet in f32, nicht in double mit Rundung am Ende (das wäre bei a*b+c nicht dasselbe).
- Eigene Mathematik: kern/third_party/musl_math (60 Dateien, mit f-Varianten wie sinf/cosf/expf/logf/powf …) wird mit
  `-ffp-contract=off -fno-math-errno` für alle Ziele gebaut: dieselbe Funktion nativ und in Wasm. Ein Knoten sin/cos/sqrt in f32
  ruft dann genau diese f-Funktion (sqrtf ist IEEE-genau). Die Funktion der Quelle (std::sin auf float) muss auf dieselbe musl-Funktion
  führen; zu prüfen ist, ob ein nativer Bau mit der libm des Systems linkt statt mit musl_math (kq2.sh linkt musl-Objekte: ja).
- Auswertungsreihenfolge: Die Formel-Knoten bilden den Ausdrucksbaum von clang nach, also dieselbe Klammerung und Reihenfolge. Zu
  prüfen sind die Teilausdrücke, die der Konverter heute umstellt (die Formel-Lambdas fassen Ausdrücke zusammen, `math_compare` …).

## Ergänzung (Agent projekt-zahlen1, 26.11.): UB und Determinismus – Volltext in kern/docs/blueprint_cpp.md „Zahlen (Entwurf)“
### Verhalten, das C++ nicht festlegt, ist ein Fehler des Konverters

Wo die Quelle etwas tut, dessen Ergebnis C++ offenlässt (undefiniertes Verhalten), schreibt der Konverter keinen Blueprint
mit einem stillen Ersatzwert. Er meldet einen Fehler mit Datei, Zeile und Grund. Die Quelle wird dann richtig gemacht
(ein eigener Commit mit Test), oder sie sagt ausdrücklich, was sie will (`wrap_add`, `std::clamp` vor der Umwandlung …).
Das gilt für:
- Überlauf vorzeichenbehafteter Ganzzahlen (`i32`/`i64` `+ - *`, `-INT_MIN`, `INT_MIN / -1`). Vorzeichenlose brechen
  definiert um, die bleiben.
- Float → Int außerhalb des Bereichs des Zieltyps, und NaN → Int. Heute legt `logic::whole` 0 und die Enden fest; das
  bleibt das Verhalten des Knotens, aber eine Quelle, die dorthin kommen kann, ist zu melden.
- Ganzzahl-Division und Modulo durch 0. Heute gibt der Knoten 0 (`math::div`), und der Test vergleicht einen solchen
  Aufruf nicht (`g_open`).
- Schieben um die Breite oder mehr, um eine negative Zahl, und Linksschieben einer negativen Zahl.
- Zugriff außerhalb einer Liste (heute: der Test vergleicht den Aufruf nicht, `g_open`).
Was sich nicht vorher ausschließen lässt (ein Wert aus einer Eingabe), meldet der Konverter als Warnung mit Ort, und der
Vergleichstest zählt die Aufrufe, die dorthin kommen („in n the source left what C++ defines“), statt sie still auszulassen.
Float-Division durch 0 ist kein solcher Fall: IEEE 754 legt Inf/NaN fest, und clang hält sich daran.

### Determinismus der Fließkomma-Rechnung

Eine Rechnung muss in allen Stufen dieselben Bits geben: nativ, im Interpreter, im JIT (Wasm im Browser), in der Klasse,
nativ und in Wasm gebaut. Stand 26.11.2026, jeweils mit Beleg:

| Regel | heute | Beleg |
|---|---|---|
| Zwischenwerte ohne höhere Genauigkeit (FLT_EVAL_METHOD 0) | gilt: x86-64 (SSE) und wasm32 rechnen `float` in 32 und `double` in 64 Bit. Aber: die Sprache rechnet jede Zahl in f64. Ein `f32`-Ausdruck der Quelle wird also in 64 Bit gerechnet und erst am Platz gerundet: das ist ein anderes Ergebnis | Fall `numbers_f32` (175 von 300 Aufrufen), Murmel `punkte` |
| Keine FMA-Kontraktion (`-ffp-contract=off`) | gilt in jedem Bau, der Code erzeugt: `kern/build.sh` (C und C++, nativ und Wasm), `kern/tools/build_native.sh`, `kern/tests/run.sh`, `kern/games/spawncity/tests/run_tests.sh`, `kern/games/murmel/tests/engine.sh`, `tools/blueprint_cpp.sh`, `kern/tools/lehrer.sh`, `tools/converter_cases/compare.sh`, die Prüfung des Konverters (`-fsyntax-only`). Der JIT schreibt einzelne `f64.mul`/`f64.add`, und Wasm hat kein FMA ohne relaxed-simd | `grep -rn ffp-contract` |
| Auswertungsreihenfolge wie die Quelle | die Formel-Knoten bilden den Baum von clang nach. Zu prüfen ist, wo der Konverter zusammenfasst (Formel-Lambdas, `math_compare`) | – |
| sin, cos, tan, asin, acos, atan, atan2, exp, expm1, log, log10, pow, hypot: dieselbe Funktion überall | **die Sprache: ja** (`kern::dm` = musl_math, auch die f-Varianten, gleich gebaut für alle Ziele; Knoten in `nodes.h` `math::sin` … rufen `dm::` in f64). **Die Quelle: nein** – SpawnCity ruft `std::sin` (197-mal), `std::atan2` (123), `std::hypot` (239) …, `kern::dm` nie. Nativ ist das die libm des Systems (glibc), in Wasm die von wasi-libc. Quelle und Blueprint rufen also verschiedene Funktionen, und nativ und Wasm geben im letzten Bit verschiedene Werte | `grep -c "std::sin(" kern/games/spawncity/src/*` ; `kern/include/kern/core/dmath.h` |
| Fehlende Funktionen in musl_math | sinh, cosh, tanh, asinh, acosh, atanh, log2, exp2 (nur Tabelle `exp2f_data.c`), log1p, cbrt, ldexp – jeweils f und d. Der Konverter schreibt sie heute als Formeln aus exp/log/pow (`converted::wide::tanh_bp` …): das ist eine andere Funktion als `std::tanh` | `ls kern/third_party/musl_math/src` |
| sqrt, fabs, floor, ceil, trunc, fmod, remainder, copysign | IEEE-genau, überall gleich (`std::`). Aber der JIT weicht ab: `math.mod` rechnet `a - trunc(a/b)*b` statt `fmod` (für große a/b nicht genau), `math.round` rechnet `copysign(floor(abs(a)+0.5), a)` statt `std::round` (falsch bei 0.49999999999999994 und nahe 2^52) | `kern/src/logic/codegen.cpp` Z. 276-283 |
| NaN und −0 | der Interpreter nimmt `min_of/max_of` (`b < a ? b : a`), der JIT `f64.min/f64.max` von Wasm (NaN gewinnt, −0 < +0). Für NaN und ±0 geben die zwei Stufen verschiedene Werte. Der Vergleichstest sieht es nicht, denn ein Ergebnis mit NaN/Inf gilt als „left the numbers“ und wird nicht verglichen (`converted::defined`) | `codegen.cpp` Z. 266-267, `kern/core/base.h` `min_of` |
| f32-Knoten | fehlen (Stufe 3): ein f32-Knoten rechnet in f32 und ruft die f-Funktion von musl (`sinf` …), nicht f64 mit Rundung am Ende | – |

Daraus folgt für die Stufen: (1) Die Quelle eines Programms ruft die Mathematik der Engine (`kern::dm`/`kern::math`) oder
der Konverter bildet `std::sin` auf genau die musl-Funktion ab. Und nativ wie in Wasm wird dieselbe musl gelinkt, nicht
die libm des Systems. (2) musl_math bekommt die fehlenden Funktionen (aus musl, gleich gebaut). (3) Der JIT nimmt für mod,
round, min und max dieselbe Rechnung wie Interpreter und Klasse (oder ruft deren C++-Funktion). Ein Engine-Test vergleicht
die Stufen an den Rändern: ±0, NaN, ±Inf, 0.5-Grenzen, 2^52 und große Quotienten. (4) Der Vergleichstest vergleicht auch NaN
und Inf bitgenau, statt solche Aufrufe auszulassen.

#### Mathe-Funktionen: eine Funktion für Quelle, Engine und Blueprint (Entwurf)

Regel: Eine Quelle im Teilsatz rechnet sin, cos, tan, asin, acos, atan, atan2, exp, expm1, log, log10, pow, hypot, sinh, tanh …
mit DENSELBEN Funktionen wie die Engine und der Blueprint. Das sind die musl-Fassungen der Engine (`kern::dm`), für f32 die
f-Varianten (`sinf` …), auf jeder Plattform, nativ wie in Wasm. Exakte IEEE-Funktionen (sqrt, fabs, floor, ceil, trunc, round,
fmod, remainder, copysign) dürfen die des Compilers bleiben.

Heute (26.11.2026):
- Die Quelle von SpawnCity bindet `<cmath>` direkt (46 Dateien) und ruft `std::`: sin 197, cos 147, tan 12, asin 9, acos 15,
  atan 5, atan2 123, exp 27, log 2, pow 28, hypot 239, sinh 1, tanh 2. Das sind **807 Stellen in 56 Dateien** von `src/`, dazu
  einige ohne `std::` (sin 3, cos 1, tan 2, exp 2). `kern::dm` ruft sie nie. Die Firmware (`firmware/*.hpp`, nicht änderbar) ruft
  16-mal std::atan2/cos/sin.
- Nativ (`kern/tools/build_native.sh`, die Tests `run_tests.sh`) kommt `std::sin` aus der libm des Systems (`-lm`, glibc). In Wasm
  (`kern/build.sh`, `kern/games/spawncity/build.sh`) kommt es aus der libm von wasi-libc. Die musl der Engine ist umbenannt
  (`kern_musl_sinf` …, `third_party/musl_math/src/kmath.h`) und wird nur über `kern::dm` gerufen. Eine nativ gebaute Quelle ist
  deshalb nicht plattformfest, und sie rechnet anders als ihr Blueprint (Ursache 4 der Messung, z. B. slerp, heading_of).
- Dazu kann clang Aufrufe selbst ändern: Konstanten falten (mit seiner eigenen, korrekt gerundeten Rechnung), `sin`+`cos` zu
  `sincos` zusammenlegen, `pow(x, 2)` zu `x*x` machen. Das hängt vom Compiler ab, nicht von der Quelle.
- In musl_math fehlen sinh und tanh (die die Quelle ruft) sowie asinh, acosh, atanh, log2, exp2, log1p und cbrt.

Vorschlag für den einen Weg: **Der Bau bindet die musl-Mathe der Engine unter den Namen der Standardbibliothek, für jedes Ziel.**
- Ein zweiter Satz der musl-Objekte ohne Umbenennung (oder dünne Hüllen `extern "C" float sinf(float x) { return
  kern_musl_sinf(x); }` …), gelinkt vor der libm. Nativ gewinnt die Definition im Programm über glibc, in Wasm das Objekt über das
  Archiv von wasi-libc. Dazu `-fno-builtin-<name>` für genau diese Funktionen (kein Falten, kein sincos, kein pow→Produkt). So
  rufen `std::sin`, `kern::dm::sin` und der Knoten dieselbe Funktion. Die Quelle und die Firmware bleiben, wie sie sind.
- Der Konverter bildet `std::sin` (auf float: `sinf`) auf diese Funktion ab, wie heute `kern::dm`. Eine Mathe-Funktion, die musl_math
  nicht hat, ist ein Fehler des Konverters mit Ort („std::cbrt: the engine has no cbrt - kern/third_party/musl_math“), keine Formel
  aus exp/log (heute `converted::wide::tanh_bp` …).
- Aufwand: die Hüllen (eine Datei, 26 Namen); in die Bauten eintragen (kern/build.sh, kern/tools/build_native.sh, kern/tests/run.sh,
  run_tests.sh, murmel/tests/engine.sh, tools/blueprint_cpp.sh, tools/converter_cases/compare.sh, all_program.sh,
  level_templates.sh, lehrer.sh, blueprint_bench.sh: ~11 Skripte); sinh/tanh (+f) aus musl nachziehen; ein Engine-Test (std::sin ==
  kern::dm::sin, Bit für Bit, nativ und in Wasm). Klein bis mittel, kein Umschreiben der Quelle.
- Die Alternative (die Quelle ruft `kern::dm::`, `std::sin` ist ein Fehler des Konverters mit dem richtigen Namen) heißt 807 Stellen
  umschreiben, und sie lässt die Firmware (16 Stellen, nicht änderbar) und jede fremde Bibliothek aus. Darum nicht.

