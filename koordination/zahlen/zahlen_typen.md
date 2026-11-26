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
