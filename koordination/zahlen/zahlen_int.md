# Int der Sprache: Bestand und Entwurf (Agent projekt, 26.11.; NICHT gebaut)

Entscheidung des Koordinators (ii): Die Sprache bekommt eine echte 64-Bit-Ganzzahl (Zweierkomplement).

## Bestand
- **Value** (kern/logic/logic.h): Int steht als `i64 i` im Value, aber jedes Int einer Stufe ist `normal(v)`: bis 2^53 genau,
  darüber der nächste double, gekappt an ±(2^63−1) (logic.h 141–152: „Every number of a Blueprint is a double underneath
  (the interpreter's variables, the JIT)“). Add to Variable: `whole_add` über double ab 2^52. Float→Int: `whole()` (auf null zu,
  NaN → 0, jenseits: Ende).
- **Int64** (eigener Pin-Typ, `logic::Int64` = u64): alle 64 Bit, Knoten int64_* (Add/Mul herum bei 2^64, Div/Rem durch 0 = 0,
  Shifts mod 64, i64/u64 je Knoten). Der Konverter macht u64/i64/uint64_t/int64_t der Quelle zu Int64, aber `long`/`long long`
  zu Int (NUMBER in cpp_to_blueprint.py).
- **Breiten am Platz** (U8…I32, `logic::Sized<T>`): Speichern wie `static_cast<uN>` (untere Bits, Zweierkomplement), auf
  dem Draht ein Int. Produkt zweier U32 nur über `kern::math::mul32`.
- **Bit-Operationen** in Formeln: band/bor/bxor/shl/shr/wrap32/wrap32s auf `Exact` (nodes.h 91–121): auf den Bits der Zahl,
  wenn sie ganz ist.
- **Division/Modulo**: Int durch 0 = 0; negative Zahlen in der Formel als double (`kern::to<Int>(formula(a / b))` → auf null zu,
  wie C++); `%` über die Formel-Funktion.
- **Konverter/Test**: Der Vergleichstest rechnet die Quelle mit `converted::plus/times/…` (converted.h 2371ff.): Wo beide
  Werte „gehalten“ sind (bis 2^53), wird genau gerechnet, sonst wie C++. Ein Literal oder eine Rechnung über 2^53, die
  herumgeht, **wartet** mit Grund: „a 64-bit whole number that wraps around or is shifted (a Blueprint's are exact up to 2^53)“.
- **Speichern/Laden** schreibt Int schon als Text (genau). **Editor**: Int-Felder als Zahl in JS (double). Darüber hinaus nur
  mit Text genau. Web/WASM-Interpreter: double.

## Fall (lokal, nicht in converter_cases eingehängt – er wäre rot und das Gate muss grün bleiben)
$S/intcase/bigint.{h,cpp}: `long long mix(long long h, int c) { return (h ^ c) * 31 + 7; }` und
`mixed_from_the_start() { return mix(1469598103934665603LL, 97); }` (FNV-Startwert, über 2^53).
Konverter (work-projekt): `mix` wird geschrieben als `kern::to<Int>(kern::formula(... bxor(h, c) * 31.0 + 7.0 ...))`, also in
double gerechnet, und nur mit kleinen Argumenten verglichen (grün, aber nicht bitgleich für große h). `mixed_from_the_start`
**wartet** („a 64-bit whole number that wraps around … exact up to 2^53“). Nativ ergibt die Quelle den C++-Wert mit Umbruch,
der Blueprint kann ihn nicht ergeben. Mit echtem Int64 wäre der Fall: beide geschrieben, mit großem Argument verglichen.
Als Fall in converter_cases wird er erst eingehängt, wenn der Bau beginnt (dann alt rot, neu grün).

## Entwurf (kurz)
1. **Int = i64, Zweierkomplement**: Add/Sub/Mul gehen bei 2^64 herum wie u64-Arithmetik (in C++ für i64 UB, im Knoten über u64
   definiert), Div/Rem auf null zu, durch 0 → 0, INT_MIN/−1 → INT_MIN (wie Int64 heute). Shifts mod 64, `>>` arithmetisch.
   Int64 als eigener Typ entfällt (die vorzeichenlosen Knoten bleiben: Less Unsigned, Div Unsigned, Shift Right Unsigned).
   Int→Float: nächster double. Float→Int: wie `whole()`.
2. **Schmale Typen der Quelle** (u8…u32, i8…i32, `int`): analog zur f32-Regel wird in Int gerechnet, mit einem definierten Umbruch
   an EINER Stelle, dem Platz (Speichern in eine Breite = untere Bits, wie heute `Sized<T>`). Was C++ in `int` rechnet und
   überlaufen lässt (UB), ist heute eine Abweichung: Regel des Konverters wie --language-numbers („rechnet in 32 Bit“) oder
   Umbruch am Platz. Zu entscheiden: Eine Rechnung auf `int`, die in C++ überläuft, ist ein Fehler der Quelle (UB) und nicht
   nachzubilden.
3. **Betroffene Stufen**: Value (normal/whole_add weg; i64 direkt), Interpreter und JIT (die Werte als double: größte Arbeit;
   der JIT braucht einen i64-Weg), Klassen (blueprint.h `using Int = logic::Whole` → i64 mit Knoten-Arithmetik), Leser/Schreiber
   (Literale bis ±2^63, Int64{0x…} entfällt oder bleibt als Schreibweise), Editor/Web (Int-Werte als BigInt oder Text in
   JS; der Logik-WASM-Test), Konverter (long/long long/u64 → Int; Wartegrund „exact up to 2^53“ entfällt), Testrahmen
   (converted::plus/times… → echte i64-Umbruch-Arithmetik; Whole/SignedWhole), Speichern (schon Text).
4. **Kosten**: groß, vor allem Interpreter/JIT und Editor-JS. Gewinn: Hashes, Zufall, Bit-Operationen und große Zähler bitgleich.
