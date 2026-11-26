# Stand Zahl-Typen, Stufen 2–4 (Agent zahlen, Zweig work-zahlen, Worktree /home/claude/w-zahlen)

Basis: origin/work-projekt 8e3dc4c6. Entwurf: $S/zahlen_typen.md. Nur Push auf work-zahlen.

## Bauplan

### Grundentscheidungen (Umsetzung des Entwurfs)
- **Ein Kopf für die Zahlen**: neu `kern/include/kern/logic/numbers.h` (Namensraum `kern::logic::num`). Darin, als Templates über
  die C++-Typen selbst (bool, i8 … u64, f32, f64): jede Operation (add sub mul div rem neg, and or xor not shl shr, ==,<,<= …,
  min max abs, die Mathe-Funktionen) und jede Umwandlung (IntegralCast, FloatingCast, IntegralToFloating, FloatingToIntegral,
  Promotion, bool↔Zahl). Das ist DIE eine Implementierung: PureNodes (Interpreter), Klasse (blueprint.h) und JIT-Rückfall rufen sie.
- **Festgelegt, wo C++ es offenlässt** (dokumentiert in numbers.h und blueprint_cpp.md):
  - Ganzzahl +,−,× auf allen Breiten: Umbruch im Zweierkomplement (wie u-Arithmetik; C++: signed-Überlauf UB → Konverter meldet).
  - ÷ und % durch 0: 0. MIN ÷ −1: MIN, MIN % −1: 0. Sonst auf null zu (wie C++).
  - Shift: Weite mod Breite (wie x86/wasm); `>>` auf signed arithmetisch (C++20). Linksshift negativer Zahlen: Umbruch (C++20).
  - Fließkomma: IEEE wie C++ (÷0 = ±inf/NaN, sqrt(−1) = NaN) – nichts umgedeutet. Mathe über kern::dm (musl, f-Varianten).
  - Float→Ganzzahl: auf null zu; NaN → 0; außerhalb: das nähere Ende des Typs (wie wasm trunc_sat). f64→f32: IEEE (±inf).
  - Ganzzahl→Ganzzahl: modulo 2^N (C++20 festgelegt). bool→Zahl 0/1, Zahl→bool: ≠ 0.
- **PinType**: `boolean, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64` statt `integer, number, int64`; je Typ eine Liste.
  Namen im Text/JSON: `"bool","i8",…,"f64"`, Listen `"f32_list"`. Altes `"int"`, `"float"`, `"int64"`, `"number"` … = Fehler mit
  Ort und Grund. „Breite am Platz“ (`of:"u32"`, Sized<T>, U8…I32) und `Int64` gehen darin auf und entfallen.
- **Value**: Art (`type`) + die Bits in `i` (ganze Zahlen vorzeichen-/null-erweitert, f32 = seine 32 Bit, f64 = seine 64 Bit,
  per bit_cast – kein Umweg über double, NaN-Bits bleiben). `n` entfällt. Zugriff `Value::num<T>()`, Bau `Value::of_num<T>(x)`.
  `normal()`, `whole_add()` entfallen.
- **Interpreter**: Slots als u64-Bits statt f64; gelesen/geschrieben im statischen Typ des Pins. Flow-Speicher (`f64* m`) wird u64.
- **JIT** (codegen.cpp, wasm): zuerst überlässt er jeden Zahl-Knoten ausdrücklich der Funktion des Knotens (Aufruf, kein Inline);
  Slots als i64 geladen/gespeichert. Inline-Wege je Typ (i32/i64/f32/f64-Befehle von wasm) danach, Stück für Stück.
- **Klassen** (blueprint.h, notation.cpp-Schreiber): die C++-Typen direkt: `bool, i8 … u64, f32, f64` (kern/core/base.h hat sie).
  `Whole`, `Sized`, `Exact`, `Sum`, `Int64` fallen weg. Operatoren der Klasse = Aufruf der num-Funktion (nicht der eingebaute
  Operator, wo C++ UB hat), damit Klasse == Interpreter.
- **Vorgaben** im Editor/Textleser: „Float“ = f32, „Int“ = i64 (nur Vorgaben neuer Pins/Variablen; geschrieben wird immer der Typ).
- **Engine-Tür** (engine_api.py, Stufe 6 – nicht meins): bis dahin erzeugt sie Pins vom Typ der Signatur, so weit das ohne
  Stufe 6 geht; sonst `f64` wie heute (dokumentiert als offen).

### Entscheidungen des Koordinators (26.11.)
1. Float ÷ 0, sqrt(−1), fmod(x,0) …: IEEE wie C++ (inf/NaN). Alte 0-Ergebnisse entfallen → Doku-Liste „was sich gegenüber den alten
   Knoten ändert“ (blueprint_cpp.md).
2. Kein Draht wandelt still um, ohne Ausnahme: auch Zahl→Text, Zahl→Vector-Splat, bool↔Zahl nicht. Umwandlung = sichtbarer Knoten;
   für Text „To Text“ je Typ mit festgelegter Darstellung (dieselbe eine Funktion, die C++/Engine heute nimmt – prüfen).
   **Stufe 7 (Editor):** beim Verbinden verschiedener Typen setzt der Editor den Umwandlungsknoten sichtbar ein bzw. bietet ihn an.
3. Formel-Knoten bleibt, als GENAU dieselbe Rechnung wie die Knoten: getypte Eingänge/Ergebnis, jede Operation über numbers.h,
   Reihenfolge wie geschrieben, keine höhere Zwischengenauigkeit; gemischte Typen = Fehler mit Stelle; Umwandlungen ausdrücklich im
   Text (Schreibweise wie im C++-Text); Literale mit C++-Typ (0.5f / 0.5 / 7u). Engine-Test: Formel gegen Knotenbaum und C++ je Typ.
   Bau am Ende von Stufe 3.

### Reihenfolge (je ein Commit oder wenige)
1. `numbers.h` + Engine-Test „numbers: Operation × Typ gegen C++“ (Randwerte) + Prüfung „Knoten rufen kern::dm“. Additiv, grün.
2. PinType/Value/JSON (logic.h, logic.cpp: Namen, zero, as_*, converted→nur gleiche Typen, to_json/from_json, MapKey,
   ListItems mit 11 Zahl-Vektoren, list_type_of). `convertible()`: kein stiller Zahl-Draht mehr.
3. Knoten (core_nodes.cpp, nodes.h PureNodes): Familien `num.add<T>` … als EINE Registrierung über alle Typen (Schleife über
   Typen, Typ-Einstellung des Knotens); Umwandlungsknoten `num.cast` (Von/Nach als Einstellung, Art als Name: IntegralCast …).
   Alte `math.*`/`int64.*`/Float-Knoten gehen darin auf (Formel-Knoten: rechnet getypt über num).
4. Interpreter-Slots u64; JIT: Zahl-Knoten als Aufruf.
5. Leser/Schreiber des C++-Texts (notation.cpp): Typnamen = C++-Typen, Literale (`0.5f`, `7u`? – Literal mit Typ des Pins),
   blueprint.h Klassen-Typen.
6. Tests umstellen (test_main.cpp, logic_cases.h, blueprint_native.h, engine_case.gen.h neu erzeugen), *.gen.* neu
   (tools/engine_api.py, --libraries). Doku blueprint_cpp.md („Zahlen“, „Ganze Zahlen fester Breite“, „Int64“ → „Zahl-Typen“).
7. Editor/Web nur so weit, dass nichts abstürzt (Typnamen-Liste im JS).

### Rote Jobs auf work-zahlen (laufend)
- Basis 70bce4fe (CI 45799372031): native (Engine-Suite inkl. numbers_against_cpp), build, murmel, murmel-web,
  converter-cases, determinism, native-build, input, e2e grün; native-spawncity-build und e2e-spawncity rot (schon auf
  work-projekt so: SpawnCity-Level/Spiel, nicht meins, Stufe 8).
- Ab 328f5783: Engine-Suite (native) rot bis die semantisch betroffenen Tests umgestellt sind (siehe Offen). Murmel,
  SpawnCity, converter-cases, e2e (Editor) rot: alte Levels/Texte mit "float"/"int"/Float/Int sind jetzt Fehler (Stufe 5/7/8).

## Fertig
- 70bce4fe numbers.h + numbers_against_cpp (CI 45799372031: native grün).
- 328f5783 PinType/Value/JSON/Knoten/Interpreter/JIT/notation/Engine-Tür, *.gen.* neu (Bibliothek baut mit -Werror).
- 9ef673ab Klassen (blueprint.h, blueprint_class.h Exporte, engine_direct.h), Tests kompilieren, numbers_nodes_interpreter.
- bb69342c fmin/fmax ±0 festgelegt, Klassen-Fall "Numbers" (blueprint_native.h), Doku „Zahl-Typen“ (blueprint_cpp.md).
- e1ea5498 Tests teils umgestellt, kAnyWidth-Eingang, Web-Editor (Typnamen, kein umwandelnder Draht).
- lokal grün: numbers_against_cpp, numbers_nodes_interpreter (>20.000 Aufrufe), dazu 66 weitere Tests (Lauf 6: 68 ok, 11 FAIL,
  dann Abbruch in logic_native_blueprints – samples() ohne neue Zahltypen, behoben im nächsten Commit).
- 8c49463a Beispielwerte aller Zahltypen (blueprint_native samples), Abstürze bei Typen ohne Beispiele abgefangen.
- 3615820e Fließkomma-Lesen über scan::real statt std::from_chars (libc++/macOS hat kein from_chars für float: CI 45808572877
  determinism scheiterte daran). CI 45809444360 (3615820e) gestartet – Ergebnis offen.
- Lokal (Lauf 9, 8c49463a): 68 ok, 11 FAIL (Liste unter Offen 2), dann Segfault in logic_native_blueprints: die Texte der
  Klassen-Fälle (kFlow, kStructs …) lesen nicht (kern::to<Text>, f64 in f32-Eingang …), der Test liest dann ins Leere.
- CI 45808572877 (e1ea5498): native-build rot (Projekt-Test: Level „eins“ liest nicht – alte Typnamen im Projekt/Level des
  Tests), murmel/murmel-web/converter-cases rot (alte Levels/Konverter, Stufe 5/8), determinism rot (from_chars, behoben).

## Offen (Datei:Stelle) – für den Nachfolger
1. **Engine-Bibliotheken** (src/logic/engine_libraries.gen.cpp) enthalten noch "float"/"int" (erzeugt von
    über cpp_to_blueprint.py --engine = Konverter, Stufe 5/6). Folge: jeder Test, der eine
   Engine-Bibliothek liest (game_*, logic_* mit engine_app_app …), scheitert mit „float … before the number types“. Weg: im
   Konverter NUMBER/ALIAS auf die C++-Typen (f32 bleibt f32), dann `python3 tools/engine_api.py --libraries`.
2. **Tests semantisch umstellen** (mechanisch erledigt mit /tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad/zq/port_tests.py): Drähte zwischen Zahltypen brauchen
   num.cast, Zahl→Text num.text (`num_text<i64>(x)` im Text), math.compare ohne 1e-9-Toleranz, sin/cos in Radiant, round/floor
   geben den Float-Typ, Random float f64/int i64. Fehlend lokal (Lauf 6): logic_formulas (Formel f64, s. 3), logic_function_export
   (Exporte: Pins i32/u32/f32/f64 exakt, bytes = u8_list, floats = f32_list), logic_actor_blueprint_from_cpp, logic_lists
   (test_main.cpp:3350 erwartet fits(i64→f64) – jetzt false: Erwartung umdrehen), logic_enums,
   logic_random_nodes_have_their_own_sequences, game_logic_graphs_drive_the_world, logic_tier_cases_interpreter
   (test_main.cpp:4232 erwartet kValueTypes == 10 → 17; dazu die 53 Fälle in logic_cases.h), logic_structs,
   logic_notation_tier_cases, logic_notation_every_node_type; blueprint_native.h-Texte: kern::to<Text>/<Bool>/<Vector>
   (z. B. „Flow line 19: a f64 cannot become a Text“). test_main.cpp:3232 erwartet das alte Int-Ende (normal): jetzt i64::min.
   Nach logic_native_blueprints ist der Rest der Suite (≈170 Tests) lokal noch nicht gelaufen.
3. **Formel-Knoten** (src/logic/formula.cpp, nodes.h math::, blueprint.h kern::formula, notation.cpp formula_types, Texte mit
   f64/i64): rechnet noch f64; Neubau nach Entscheidung 3 (getypt über numbers.h, Literale mit Typ, gemischte Typen = Fehler).
4. **JIT** (codegen.cpp): inline nur Fluss; Zahl-Knoten über read_pin (Knotenfunktion). Inline-Wege je Typ fehlen; die Zähler
   von For/Do N stehen im Knotenspeicher als f64 (nodes.h flow::, auch Interpreter) – i64-Schleifen jenseits 2^53 ungenau.
5. **Engine-Tür** Int=i64, Float=f64, Int64=u64 (cpp_to_blueprint.py ENGINE_PIN, engine_api.py EngineNodesOf) – Stufe 6.
6. **Web/Editor** (web/studio/logic.mjs): nur Namen/Farben/Vorgabe; Typ-Menü, Umwandlungsknoten beim Verbinden (Entscheidung 2),
   i64/u64 als Text in JS, f32 über Math.fround – Stufe 7. tests/e2e erwarten alte Typnamen.
7. Kleinere: Literal i64::min nur in literal_here (notation.cpp); Struct of Bytes kopiert die Liste in i64 (core_nodes.cpp,
   struct.of_bytes – Geschwindigkeit); Sort über NaN wie std::stable_sort (keine strenge Ordnung).

## Befund B4 für zahlen3 (von Agent projekt-zahlen1, work-projekt 63416d88): JIT ≠ Interpreter
kern/src/logic/codegen.cpp (JIT, Wasm) rechnet anders als der Interpreter und die Klasse (kern/include/kern/logic/nodes.h math::):
- math.mod (Z. ~282): `a - trunc(a/b)*b` statt std::fmod: für große a/b nicht genau (z. B. a=1e17, b=3).
- math.round (Z. ~273-276): `copysign(floor(|a|+0.5), a)` statt std::round: falsch bei 0.49999999999999994 (gibt 1) und nahe 2^52.
- math.min/math.max (Z. 266-267): f64.min/f64.max von Wasm (NaN gewinnt, min(-0,+0) = -0) statt min_of/max_of (`b < a ? b : a`).
- math.sqrt (Z. 271): `sqrt(max(a, 0))` mit f64.max: sqrt(NaN) = NaN, im Interpreter max_of(0, NaN) = 0.
- convert zu Int (Z. 198-208): eigene Rechnung; zu prüfen gegen logic::whole.
Bitte prüfen, ob es auf work-zahlen damit weg ist (dort rechnet der JIT keine Zahl mehr selbst). Sonst beheben: der JIT ruft
die C++-Funktion des Knotens. Ein Engine-Test auf work-projekt ist noch NICHT gebaut (Vorschlag: ein Fall in
kern/tests/logic_cases.h mit diesen Rändern; logic_wasm_test.mjs vergleicht Interpreter und JIT, und er wird rot).


## Nachfolger (Agent zahlen, zweite Sitzung, 26.11.) – Stand
### Fertig (Commits auf work-zahlen)
- CI 45809444360 (3615820e) ausgewertet: determinism grün; native-build rot (Engine-Bibliotheken mit float/int), murmel rot
  (Murmel-Texte mit Int, bruecke.Sprungfeld.cpp:3), converter-cases rot (pad: level/project lesen nicht), native-spawncity-build rot
  (spawncity.klevel: Wanted.Heat "float"). Alles erwartet (Stufe 5/8).
- 5bd… (erster Commit dieser Sitzung): Segfault logic_native_blueprints: cases() las `.front()` leerer Beispiele, wenn ein Klassen-Text
  nicht las -> Case::error, Test prüft ihn (blueprint_native.h read(), first_of/last_of; test_main.cpp logic_native_blueprints).
- work-projekt (c1e07544) eingemischt (Testrahmen zweiter Durchgang, converter_cases numbers_*).
- Konverter Typen (tools/cpp_to_blueprint.py): EXACT/INTS/FLOATS/NUMS, exact_type, f32_of/f32_of_text/f32_literal/number_literal;
  btype/place geben die C++-Typen (float f32, int i32, unsigned u32, long/size_t u64/i64 (!), Count<Per> u32, Iterator i64);
  Const.number rechnet im C++-Typ (f32-Literal aus dem Dezimaltext korrekt gerundet, jeder Schritt gerundet, ganze Zahlen exakt,
  u64 vorzeichenlos); literal() schreibt Literale mit Typ (0.05f); "Bool" heißt "bool"; --engine: KEINE Graphen mehr für Funktionen,
  die ein Header definiert (nur virtuelle bleiben) - Entscheidung main (Stufe 6 vorgezogen). KERN_DEBUG_WAIT=1 druckt Wartegründe.
- Engine-Bibliotheken neu (lesen wieder). Fehlend: Konstanten kPi/kEarthRadius/kRad/kWidthNames (Entscheidung main: als Konstanten
  der Engine mit C++-Typ erzeugen - OFFEN), ShaderExtension_material_uniforms ("no default for Int": Zeiger-Parameter, offen).
- Stufe 6 Tür: engine_values::Num<T> (engine_host.h Conv/Held, engine_direct.h Direct), engine_api.py tag()/PLAIN/plain, Api.pin/shape/
  NUM_SHAPE (Formen f f32, w i32, u u32, s i16, r u16, k i8, y u8 in engine_api.cpp answer()), Callback-Typen exakt, written_list
  Zahltypen, Struct-Bindung field_<k>. 857 Knoten wie vorher, Pins jetzt mit Signaturtypen (f32 bleibt f32). *.gen.* neu.
- Knoten list.text (List to Text; Klasse list_text<T>), Entscheidung main: in Ordnung (Doku fehlt noch: blueprint_cpp.md).
- Notation: kern::to ganz abgewiesen; Listenliteral anderen Typs = Fehler; Meldungen mit neuen Typnamen; Schreiber ohne kern::to.
- Tests umgestellt (semantisch, Cast-Knoten): blueprint_native.h (alle 20 Texte lesen), logic_cases.h (Porter $S/rd/port_cases.py:
  num.cast/num.text/list.text auf jedem Draht zwischen Typen; int64.* -> getypte Knoten; copy_bytes/bytes/functions von Hand typisiert),
  game_cases.h, test_main.cpp (Werte nach C++: whole_numbers i64-Umbruch, inf/NaN; Probe-Klassen f32 bitgleich gegen Engine), Editor-
  Vorlagen (src/studio/logic_edit.cpp, Vorgaben f32/i64). Werkzeuge: $S/rd/{rd,rdc,lnk,port_raw.py,port_cases.py,each.sh}.
- ddce2e54: Stand gepusht, CI 45820290037 gestartet.

### Zuordnung der entfernten Tests (logic_fixed_widths, logic_int64_is_u64_and_i64) -> neue Tests
| alte Aussage | neu in |
|---|---|
| Platz einer Breite hält die niedrigen Bits (Variable, Feld, Eingang, Ausgang, lokal; Set/Add/Aufruf/von außen), alle Stufen | logic_number_types_at_places (Interpreter+JIT: u8 += Umbruch, num_cast i8/i32/u16, u32-Umbruch, set von außen, Twice i16->u8); Klasse: Fall Numbers (blueprint_native) |
| wrapped() an den Enden jeder Breite | numbers_against_cpp ("Whole numbers to narrower ones: the low bits"), numbers_nodes_interpreter (num.cast jede Typ-Kombination) |
| Text/JSON-Rundreise mit Breiten, type_text/type_from_text | logic_number_types_at_places; logic_notation_tier_cases (alle Fälle, auch fixed_widths) |
| Literal außerhalb der Breite (Variable, Map-Wert, Listen-Eintrag), Map/Fixed, Schlüsseltypen | logic_number_types_at_places (says ...) |
| List<U8> ist keine List<Int>; Copy Bytes / Struct of Bytes Breite | logic_number_types_at_places; logic_copy_bytes; logic_bytes_of_struct |
| check(): Vorgabe außerhalb | logic_number_types_at_places (u16 70000) |
| fixed_width_lists in allen Stufen | logic_tier_cases_* (Interpreter/JIT/Klasse gleich); Werte gedruckt in logic_number_types_at_places (OFFEN: feste Erwartung eintragen) |
| alle u64/i64-Operationen gegen C++ mit Zufallsbits und Rändern, Div/Rem/Shift signed/unsigned, Rotate, Vergleich | numbers_against_cpp + numbers_nodes_interpreter (Operation x Typ, Ränder, 64 Bit über 2^53) |
| bits of float (f32/u32, NaN-Nutzlast) | numbers_against_cpp ("The bits of a value stay its bits - a NaN's too"), num.bit_cast im Knotentest |
| Text u64/i64/hex, of_text-Ränder | logic_number_types_at_places (num_text, num_text_hex, text_to_u64 ""/zz/Überlauf/-5/0x1F) |
| Float->Int64 an den Enden, NaN | numbers_against_cpp (Float -> Integer beyond the ends, NaN 0) |
| JSON des u64 (Text), from_json Zahl/"0x10" | logic_number_types_at_places (Rundreise ~0, 5.0) - "0x10" entfällt (JSON ist Zahl oder Dezimaltext) |
| convertible u64<->f64 usw. false | logic_number_types_at_places (Schleife), numbers_nodes_interpreter |
| list_of/element_of/pin_type_name u64_list | logic_number_types_at_places |

### Offen (Datei:Stelle)
1. Engine-Suite lokal: letzter Stand siehe kqz_run5.log; Formel (Stufe 3) NICHT neu gebaut: formula.cpp rechnet f64, nur shl-Erwartung
   (test_main.cpp logic_formulas) an numbers.h angepasst. Formel-Fälle in logic_cases.h (93 math.formula) sind nur mit Casts portiert.
2. Konverter Stufe 5 (Ausdrücke): conv()/cast()/Formel-Lambdas/synthetische Temporäre "Int"/"Float" (cpp_to_blueprint.py ~10180
   conv, 6082 cast, ~281 Stellen "Int"/"Float"/"Int64") schreiben noch die alte Sprache -> jeder Programm-Lauf (Murmel, SpawnCity,
   converter_cases) bricht. Plan: synthetische Typen i64/f64, conv() schreibt num_cast<want, have> zwischen Zahltypen, jede
   ImplicitCastExpr ein num_cast, Formel-Lambdas getypt (nach Stufe 3).
3. Entscheidung main 1: long/size_t/ptrdiff_t/long double = Konverter-Fehler mit Vorschlag; .size()/Index = u64 (kern::usize),
   Umwandlung sichtbar. NICHT gebaut (EXACT bildet long/unsigned long noch auf i64/u64 ab). Zählung in SpawnCity/Murmel offen.
4. Stufe 6 Rest: Inline-Funktionen der Header als Engine-Knoten (engine_api.py: declared_only/Filter "defines"), Konverter ruft sie;
   Konstanten der Engine; Engine-Test Knoten == Header-Funktion bitgleich.
5. Mathe-Knoten (Nachricht main): hypot/log10/expm1 sind Knoten (core_nodes.cpp add_math); cosh/asinh/log2/cbrt/ldexp fehlen
   (nach Einmischen von work-projekt fbf8eee2, musl std_names.c).
6. Entscheidung B2 (main): rules::decimal (kern/games/spawncity/src/blueprint_call.h:27, 235 Aufrufe) wird bei Stufe 8 im Spiel
   ENTFERNT, Regel-Blueprints mit f32 gerufen.
7. Doku blueprint_cpp.md: list.text, Num<T>-Tür, kein kern::to, Literale u64 dezimal.

## Performance-Schritt: JIT getypt wieder inline (von Agent projekt, 26.11. ~12:45)
- work-projekt d449961d (B4): math.mod/round/min/max/clamp/sqrt sind im JIT nicht mehr inline (Instance::inline_pure), der JIT ruft die
  Funktion des Knotens (Richtigkeit vor Tempo). Fall number_edges in kern/tests/logic_cases.h (9 Werte an NaN/-0/Rändern + FloorZero).
- Folge im Bench von logic_wasm_test (Job build, For über 1e6 mit math.mod im Rumpf): JIT 9,1 ms (CI 45818297493) -> 209,3 ms (CI 45819295104),
  Interpreter ~1500-1800 ms. Später: getypte Knoten wieder inline, aber bitgleich (fmod/round wie der Knoten), Beleg number_edges.

### Nachtrag (Ende dieser Sitzung)
- a6b47e9d: Engine-Suite LOKAL GRÜN (kqz.sh, 57279 Prüfungen, 0 Fehler). CI 45822208759 gestartet (native/determinism/build/
  native-build abwarten). CI 45820290037 (ddce2e54): determinism rot (macOS: u64 = unsigned long long ≠ size_t) -> behoben in
  a6b47e9d (Conv<Num<T>>::fits: gleiche Zahl unter anderem Namen, std::size_t ≡ u64); native-build rot (mini.kproject "int") -> behoben;
  build rot (kern_lehrer.wasm fehlte - vermutlich dieselbe size_t-Prüfung auf wasm32; mit a6b47e9d prüfen).
- Entscheidung main: Datenmodell LP64 (size_t ≡ u64 …), in blueprint_cpp.md „Zahl-Typen“ + static_assert (number_tests.h).
  long double/intptr_t/uintptr_t = Konverter-Wait. OFFEN: Engine-Test „Klasse in Wasm == nativ für size_t-Arithmetik am Rand“.
- fixed_width_lists W (logic_number_types_at_places) ist der Wert des Interpreters (Stufen gleich), nicht unabhängig gegen C++ geprüft.
- Stufe 6 Inline-Knoten: Api.engine_inline gebaut, hinter KERN_ENGINE_INLINE_NODES=1 (mit Schalter 933 statt 857 Knoten; engine_calls
  von app kompiliert nicht: std::initializer_list<std::string_view> an der Tür). Bis dahin sind die Header-Funktionen (dot, Mat4_mul …)
  weder Graph noch Knoten -> konvertierte Programme, die sie rufen, warten.
- Rote Jobs auf work-zahlen erwartet bis Stufe 5/8: murmel, murmel-web, converter-cases, native-spawncity-*, e2e (Editor-JS Stufe 7).
- Nicht getan: Formel neu (Stufe 3), Konverter-Ausdrücke (Stufe 5 Rest), Stufe 4/5 (work-projekt typen/bigint/Murmel-Summe grün),
  Stufe 8 Murmel, Mathe-Knoten cosh/asinh/log2/cbrt/ldexp (work-projekt fbf8eee2 einmischen).
