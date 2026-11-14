# Blueprints als C++: die zweite Schreibweise

Ein Blueprint und sein C++-Text sind **dasselbe in zwei Schreibweisen**. Jeder Knoten ist genau
eine Anweisung oder genau ein Ausdruck; jede Anweisung und jeder Ausdruck des Textes ist genau ein
Knoten. Der Weg vom Graphen zum Text und der Weg zurück sind beide vollständig und verlustfrei –
nichts wird zur Laufzeit in einer Knoten-Registry nachgeschlagen, nichts geht beim Hin- und
Herwandeln verloren.

Gespeichert wird der Blueprint (im Level, `.klevel`). Das C++ ist die zweite Ansicht derselben
Sache: Man kann sie lesen und ändern, und was man ändert, ist der Blueprint.

```
kern/include/kern/logic/logic.h      to_cpp, from_cpp, apply_cpp, canonical, same_blueprint, cpp_name_ok, cpp_problem, cpp_names, native_ready
kern/include/kern/logic/blueprint.h  wogegen der Text kompiliert: kern::Blueprint, kern::Node, kern::formula, kern::to, KERN_PURE, KERN_LAYOUT
kern/include/kern/logic/nodes.h      was die Knoten rechnen, als typisierte Funktionen – für den Interpreter und für den Text dieselben
kern/include/kern/game/blueprint_nodes.h  dasselbe für die Gameplay-Knoten (kern::game::nodes) und ihre Aufrufe in der Klasse
kern/src/logic/notation.cpp          Shape (was steht wo), Writer (Graph → Text), Reader (Text → Graph)
kern/src/game/logic_node_shapes.cpp  die Gameplay-Knoten, wie die Bibliothek sie kennt: Namen, Pins, Einstellungen (braucht nichts vom Spiel)
kern/src/game/logic_nodes.cpp        was die Gameplay-Knoten tun (kern::game::nodes), an ihre Knoten gebunden
kern/src/studio/logic_edit.cpp       logicCpp, logicCppApply (die C++-Ansicht des Editors)
kern/tests/test_main.cpp             logic_notation_*, studio_logic_cpp_view, logic_native_blueprints, game_blueprints_as_classes (der Beweis, siehe unten)
kern/tests/blueprint_native.h        die Fälle des nativen Tests: Blueprints, Aufrufe, der erzeugte Treiber
kern/tests/game_cases.h              zwei Levels, deren Blueprints jeden Gameplay-Knoten benutzen, mit ihren Szenarien
kern/tests/game_native_test.cpp      spielt sie im Interpreter und als Klassen und vergleicht
tools/blueprint_cpp.cpp, .sh         die Blueprints eines Levels als C++-Klassen, die ein Spiel einbindet
tools/export_game.sh, .mjs           der native Export: ein Level als Spiel, die Klassen einkompiliert
```

| Funktion | Bedeutung |
|---|---|
| `to_cpp(graph, name, text)` | Der Blueprint als C++, in der einen kanonischen Form. |
| `from_cpp(text, graph)` | Der Blueprint, der dieser Text ist. Was nicht zur Schreibweise gehört, wird mit Zeile, Spalte und Grund abgelehnt. |
| `apply_cpp(text, vorher, graph)` | Der Text als Änderung an einem Blueprint: Ohne Layout-Block behält jeder Knoten, den es noch gibt, Id, Platz und Kommentar; neue bekommen neue Ids und stehen neben ihrem Vorgänger. |
| `canonical(graph)` | `from_cpp(to_cpp(graph))`: derselbe Blueprint in der Form, die sein Text ergibt. |
| `same_blueprint(a, b)` | Derselbe Blueprint – Knoten für Knoten, Draht für Draht, Wert für Wert –, egal in welcher Reihenfolge gespeichert. |
| `cpp_problem(graph)` | Warum ein Graph nicht als C++ geschrieben werden kann (leer: er kann). `check()` meldet dasselbe als Fehler. |
| `cpp_names(graph)` | Für Levels von früher: benennt um, was kein C++-Name ist („Ball Seen“ → `BallSeen`), samt allen Verwendungen. Läuft beim Laden eines Levels. |
| `native_ready(graph, warum)` | Kann ein C++-Compiler den Text dieses Blueprints ausführen, so wie er dasteht? Ja, wenn er fehlerfrei kompiliert und jeder seiner Knoten eine Funktion von `blueprint.h` ist (`NodeDef::native` – in Kerns Bibliothek jeder). |
| `cpp_class_name(owner, graph)` | Der Klassenname für den Blueprint eines Actors: sein Name als C++-Name, den nichts in der Klasse trägt. |
| `native_glue(graph, name, text)` | Der Kleber hinter dem Text: macht die Klasse für die Engine beim Namen erreichbar (`logic::NativeBlueprint`) und meldet sie unter dem Hash des Graphen an. Kein Teil der Schreibweise. |

## Stand

- **Fertig:** die Schreibweise in beide Richtungen, für jeden Knotentyp der Bibliothek, mit den
  Regeln, die sie braucht, und dem Beweis als Test.
- **Fertig:** die C++-Ansicht im Editor (siehe unten): ansehen, ändern, anwenden.
- **Fertig für die ganze Knotenbibliothek, samt Schleifenwächter:** Der Text wird von einem
  C++-Compiler ausgeführt, so wie er dasteht (siehe „Vom Compiler ausgeführt“) – Variablen,
  Funktionen, Formeln, alle Wert-Knoten, Branch, Sequence, Schleifen, Do Once, Do N, Gate, Flip
  Flop, die Switches; alle Events, Call Event, Print, Game Time; Delay, Retriggerable Delay,
  Tween; State Machines; Random; **und alle Gameplay-Knoten** – Actors, Physik, Kampf, Effekte,
  die Events des Spiels, die Roboter. Jeder Knoten ist eine typisierte Funktion; nichts wird zur
  Laufzeit nachgeschlagen. Es gibt keinen Knoten mehr, den ein Compiler nicht ausführt.
- **Fertig: Das Spiel führt diesen Text aus.** Der Build schreibt die Blueprints eines Levels als
  C++-Klassen in eine Quelle des Spiels (`tools/blueprint_cpp.sh`); `logic::Runtime` lässt einen
  Graphen, dessen Klasse im Programm steckt, als diese Klasse laufen (Stufe „native C++“).
  SpawnCitys Regel-Blueprint „Wanted“ läuft so im ausgelieferten Spiel.
- **Fertig: ein C++ je Blueprint.** Den früheren Export (eine Schleife über Knotennummern, die in
  die Knoten-Bibliothek zurückrief) gibt es nicht mehr.
- **Fertig: der native Export.** `tools/export_game.sh level.klevel spiel.html` schreibt ein Level
  als eigenständiges Spiel, dessen Engine-Modul für dieses Level gebaut ist: alle Blueprints als
  ihre C++-Klassen, voll optimiert. Die Seite ist die, die „Package Game“ im Editor schreibt.
- **Fertig: Listen und „For Each“.** Ein Pin oder eine Variable kann eine Liste von Werten eines
  Typs sein (`List<Float>`, `List<Text>` …). Die Wert-Knoten lesen eine Liste, die Schritte ändern
  eine Listen-Variable, For Each geht durch eine Liste – in beiden Schreibweisen und in jeder
  Stufe gleich (siehe „Listen“).
- **Fertig: Aufzählungen (Enums).** Ein Blueprint deklariert eigene Typen mit benannten Werten
  (`KERN_ENUM(Mode, Closed, Opening, Open);`). Eine Variable, eine Liste, die Ein- und Ausgänge
  einer Funktion und Pins können von so einem Typ sein; ein Wert ist genau einer der Werte –
  weder Zahl noch Text. Sieben Knoten arbeiten damit, darunter Switch on Enum mit einem Weg je
  Wert – in beiden Schreibweisen und in jeder Stufe gleich (siehe „Aufzählungen“).
- **Fertig: Strukturen (Structs).** Ein Blueprint deklariert eigene Typen mit benannten Feldern
  (`KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };`). Ein Feld ist ein Wert-Typ,
  eine Aufzählung, eine weiter oben deklarierte Struktur oder eine Liste davon. Eine Variable,
  eine Liste, die Ein- und Ausgänge einer Funktion und Pins können von so einem Typ sein. Make
  baut einen Wert aus seinen Feldern, Break gibt die Felder – in beiden Schreibweisen und in
  jeder Stufe gleich (siehe „Strukturen“).
- **Fertig: Konstanten.** `KERN_CONST Float Gravity = 9.81;` wird gelesen wie eine Variable und
  nie geändert – die Prüfung meldet jeden Knoten, der es täte. Auch eine Bibliothek hat sie.
- **Fertig: Bibliotheken.** Ein Blueprint, der nichts behält, wird von anderen benutzt: Sie haben
  seine Typen und rufen seine Funktionen direkt – `Workshop::skid(Spare[i], 1.0);` –, auch mit
  Referenzen. Jede Funktion steht einmal im Projekt (siehe „Bibliotheken“).
- **Fertig: Lesen kopiert nicht (in der Klasse).** Get Item und Break geben den Eintrag, wie er
  dasteht (`list_get<T>` und `struct_break<S>` sind in der Klasse `const T&`), Find, Contains und
  Remove Item vergleichen ohne Kopie. Die Bedeutung ist die von Werten – in jeder Stufe dieselbe.
- **Fertig: Referenz-Eingänge.** Ein Eingang einer Funktion kann der Platz sein, den der Aufruf
  nennt – `void Skid(Wheel& wheel)`, aufgerufen `Skid(Cars[i].Wheels[j]);` –, zum Ändern oder
  (`const Wheel&`) zum Lesen, ohne Kopie; mit der Regel, dass ein gehaltener Eintrag bleibt, wo er
  ist (siehe „Referenz-Eingänge“).
- **Fertig: Plätze.** Eine Variable, ein Feld davon, ein Eintrag einer Liste darin – beliebig
  tief – wird gelesen, gesetzt, erhöht und als Liste geändert, wo es steht:
  `Cars[i].Wheels[j].Slip += 0.1;`. Kein Herauslesen und Zurückschreiben, in keiner Stufe; ein
  Platz neben einer Liste ist ein eigener Eintrag mit den Defaults, von dem nichts bleibt (siehe
  „Plätze“).
- **Die Stufen stimmen überein:** Was Interpreter, JIT und Klasse verschieden taten – Funktionen,
  die über Events wieder aufgerufen werden, die Tiefe von Events und Werten, ganze Zahlen an
  ihren Enden, der Stapel des JIT –, ist gleichgezogen (21 Tier-Fälle, in jeder Stufe dasselbe).

## Im Editor

Im Blueprint-Editor öffnet **C++** in der Toolbar die zweite Ansicht desselben Blueprints: sein
Code, ohne den Layout-Block (wo die Knoten stehen, bleibt beim Graphen).

- Der Text folgt dem Graphen: Undo, ein umbenannter Pin, ein anderer Blueprint – die Ansicht zeigt
  immer, was der Blueprint gerade ist, solange man nicht darin tippt.
- **Apply** (Ctrl+Enter) macht den Text zum Blueprint, als ein Undo-Schritt. Knoten, die es noch
  gibt, behalten Id, Platz und Kommentar; neue stehen neben ihrem Vorgänger. Danach zeigt die
  Ansicht den Text in der kanonischen Form.
- Was sich nicht lesen lässt, wird nicht angewendet: Die Fehlerzeile nennt Zeile, Spalte und
  Grund, ein Klick setzt den Cursor dorthin. **Reset** verwirft die Änderungen.
- Neben dem Namen steht, dass ein C++-Compiler diesen Text ausführt, so wie er dasteht („runs as
  compiled C++“) – das gilt für jeden Blueprint, der fehlerfrei kompiliert.
- **Download** lädt die Quelle, die ein Spiel-Build für diesen Blueprint kompiliert: seine eigene
  Klasse – der Text und dahinter der Kleber (`blueprint_<name>.cpp`, dasselbe, was
  `tools/blueprint_cpp.sh` für ein ganzes Level schreibt). Ein Blueprint, der nicht kompiliert,
  hat kein C++; Download sagt dann, warum.
- Die Klasse heißt wie der Actor, als C++-Name (`cpp_class_name`): „Door 1“ ist `Door1`; heißt
  schon eine Funktion oder Variable darin so, `Door1Blueprint`.
- Kann der Blueprint gerade nicht als C++ geschrieben werden (ein Regelverstoß, siehe unten),
  steht dort der Grund; behoben wird er im Graphen.

## Ein Beispiel

```cpp
struct Demo : kern::Blueprint {
  // ---- variables ----
  /// How hot it is.
  Float Heat = 1.0;
  Int Count = 0;
  Bool Open = false;

  // ---- functions ----
  struct LevelResult { Float Result = 0.0; Bool Hot = false; };
  /// The level for a heat.
  KERN_PURE LevelResult Level(Float heat = 0.0, Float scale = 2.0) {
    if (math_compare<">">(heat, 10.0)) {
      return {kern::formula([](Float h, Float s) -> Float { return h * s; }, heat, scale), true};
    }
    return {0.0, false};
  }

  void Cool(Float by = 1.0) {
    if (math_compare<"<=">(Heat, 0.0)) {
      return;
    }
    Heat = math_sub(Heat, by);
    if (Open) {
      Count += 1;
    } else if (math_compare<">">(Heat, 5.0)) {
      Count += 2;
    }
    debug_print(kern::to<Text>(Count), false);
  }

  // ---- events ----
  KERN_EVENT(on1, event_tick)
  void on1(Float delta) {
    n1 = {delta};
    Cool(delta);
    flow_delay(n9, 0.5, [this] {
      Heat += n1.delta;
    });
  }

  // ---- what nodes give and keep ----
  struct { Float delta = 0.0; } n1;
  kern::Node<9> n9;
};

KERN_LAYOUT(Demo) {
  node(10, 0, 0);
  ...
  next_id(35);
}
```

## Die Abbildung

| Blueprint | C++ |
|---|---|
| der Blueprint | `struct Name : kern::Blueprint { ... };` |
| Aufzählung (mit Beschreibung) | `/// Beschreibung` und `KERN_ENUM(Mode, Closed, Opening, Open);` – im Abschnitt „types“, über den Variablen |
| Struktur (mit Beschreibung) | `/// Beschreibung` und `KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };` – im Abschnitt „types“, nach den Aufzählungen und über den Variablen; jedes Feld steht da wie eine Variable |
| Variable (mit Beschreibung) | `/// Beschreibung` und `Float Heat = 0.0;` |
| Konstante (gelesen wie eine Variable, nie geändert) | `KERN_CONST Float Gravity = 9.81;` – in C++ `static inline const`: eine für die Klasse. Kein Set, kein Add, kein Listen-Schritt, keine Referenz, die sie ändern könnte; von außen (`Instance::set`) nicht setzbar |
| Funktion mit Ausgängen | `struct FnResult { Float y = 0.0; };` und `FnResult Fn(Float x = 1.5) { ... }` |
| Funktion ohne Ausgänge | `void Fn(...) { ... }` |
| Funktion „pure“ (als Wert-Knoten angeboten) | `KERN_PURE` vor der Funktion |
| Eingang einer Funktion als Wert | `Float by = 1.0` – mit seinem Default |
| Bibliothek (ein Blueprint, der nichts behält) und wer sie benutzt | `struct Workshop : virtual kern::Library { … };` · `struct Garage : virtual kern::Blueprint, virtual Workshop { … };` – Aufruf `Workshop::skid(Spare[i], 1.0);` (siehe „Bibliotheken“) |
| Eingang einer Funktion als Referenz (der Aufruf nennt einen Platz) | `Car& car` – die Funktion ändert den Platz; `const Car& car` – sie liest ihn nur. Im Aufruf steht der Platz: `Drive(Cars[i], 0.5);` (siehe „Referenz-Eingänge“) |
| Typ des Projekts (mehrere Blueprints des Levels haben ihn, alle gleich) | `KERN_PROJECT_ENUM(Mode, Closed, Open);` · `KERN_PROJECT_STRUCT(Door) { Mode State = Mode::Closed; };` – sonst wie `KERN_ENUM` / `KERN_STRUCT` |
| Funktion eines anderen Blueprints | `KERN_EXTERN FnResult Fn(Float x = 1.5);` – wie sie aussieht, ohne Rumpf (mit `struct FnResult { ... };` darüber, wenn sie Ausgänge hat) |
| Call on Actor | `func_call_on<"Fn">(target, kern::out(n7.y), x);` – erst das Target (`Actor{0}`: der Blueprint des Levels), dann `kern::out(...)` mit allen Ausgängen in ihrer Reihenfolge als Felder des Members `n7` (oder `kern::out()`, wenn niemand sie liest), dann die Eingänge |
| Function Entry | der Anfang des Funktionsrumpfs; seine Ausgänge sind die Parameter |
| Lokale Variable einer Funktion | `Float Total = 0.0;` als erste Zeilen des Funktionsrumpfs – eine lokale Variable von C++; gelesen und gesetzt wie jede Variable (`Total`, `Total = x;`, `Total += x;`, `Hit.Damage = x;`, `list_add(n7, Seen, x);`) |
| Platz (eine Variable, ein Feld davon, ein Eintrag einer Liste darin – beliebig tief) | wie C++ ihn schreibt: `Cars[i].Wheels[j].Slip` – die Einstellung „path“ des Knotens (`[].Wheels[].Slip`), jeder Index ein Eingang (siehe „Plätze“) |
| Return | `return {wert, wert};` bzw. `return;` |
| Event | `KERN_EVENT(on5, event_custom<"Hit">)` und `void on5(Actor actor, Float value, Text text) { ... }` |
| State Machine | `kern::Machine machine3 = machine("Door").state(...).on_event(...).after(...).when(...).automatic(...);` |
| Knoten-Einstellungen (Parameter) | Template-Argumente: `flow_tween<"smooth">`, `event_custom<"Hit">` |
| Einstellung, die einen Typ nennt („Type“ von Select, Reroute, den Listen-Knoten; „Enum“ der Knoten einer Aufzählung; „Struct“ von Make und Break) | der Typ selbst: `logic_select<Float>(pick, a, b)`, `value_reroute<List<Text>>(x)`, `list_make<Vector, "2">(a, b)`, `enum_name<Mode>(wert)`, `struct_make<Weapon>(...)` |
| Eingänge eines Knotens | Funktionsargumente, in der Reihenfolge der Pins |
| unverdrahteter Eingang | der eingetippte Wert als Literal |
| Wert-Knoten (pure) | ein Ausdruck: `math_add(a, b)` |
| Get Variable | der Name der Variablen – oder der Platz, den der Knoten nennt: `Held.Damage`, `Cars[i].Wheels[j].Slip` |
| Set / Add to Variable | `Heat = wert;` / `Heat += wert;` – an einem Platz genauso: `Held.Damage = wert;`, `Scores[i] = wert;`, `Cars[i].Speed += wert;`; wird ihr Ergebnis gelesen: `n5.value = (Heat += wert);` |
| Take Variable | `var_take(Punkte);` – der Platz ist das Argument, eine Variable oder ein Platz in ihr (`var_take(Cars[i].Wheels);`); wird sein Ergebnis gelesen: `var_take(n7, Punkte);` und `n7.value`. Gibt, was am Platz steht, und lässt dort den leeren Wert des Typs (siehe „Übergeben statt kopieren“) |
| Schritt, der eine Liste ändert (Add to List, Insert, Remove, Clear – und in einem Zug: Set Length, Append List, Insert List, Reserve) | `list_add(Scores, wert);` – der Platz der Liste ist das erste Argument, eine Variable oder eine Liste darin (`list_add(Cars[i].Wheels, rad);`); wird sein Ergebnis gelesen: `list_add(n7, Scores, wert);` und `n7.index` |
| Wert-Knoten auf einer Liste (Length, Get Item, Contains, Find, Make List) | `list_get<Float>(Scores, 0)`, `list_make<Text, "2">("a", "b")` – die Einstellung „Type“ ist der Typ der Einträge |
| For Each | `for (auto it9 = flow_for_each<Float>(n9, Scores); it9.next();) { ... }` – im Rumpf `n9.item` und `n9.index` |
| Wert-Knoten einer Aufzählung (Equal, Enum to Text, Text to Enum, Enum to Index, Index to Enum, All Values) | `enum_name<Mode>(State)`, `enum_from_name<Mode>(text).value`, `enum_values<Mode>()` – die Einstellung „Enum“ ist die Aufzählung selbst |
| Make und Break einer Struktur | `struct_make<Weapon>("Bow", Kind::Ranged, 12.0)` – ein Wert je Feld, in der Reihenfolge der Felder; `struct_break<Weapon>(Held).Damage` – das Feld bei seinem Namen |
| Call Function | `Fn(args);` – oder `n14 = Fn(args);`, wenn seine Ergebnisse gelesen werden |
| Function Value | `Fn(args).y` |
| Formel-Knoten | `kern::formula([](Float a, Float b) -> Float { return a * b; }, wertA, wertB)` |
| Umwandlung an einem Draht (Int → Float …) | `kern::to<Float>(wert)` |
| ein Wert, den nach diesem Eingang nichts mehr braucht | `std::move(wert)` – der Draht ist derselbe, der Text reicht den Wert weiter, statt ihn zu kopieren (siehe „Übergeben statt kopieren“) |
| Branch | `if (wert) { ... } else { ... }` |
| Sequence | `{ ... } { ... }` – ein Block je Ausgang |
| For Loop (jeder Schleifen-Knoten) | `for (auto it12 = flow_for(n12, 0, 9); it12.next();) { ... }` – danach folgt „Completed“ |
| While Loop | `while (loop(wert)) { ... }` – danach folgt „Completed“ |
| Knoten, der entscheidet, ob es weitergeht (Do Once, Gate, Do N) | `if (flow_do_once(n7, "exec", false)) { ... }` |
| Knoten mit mehreren Wegen (Switch, Flip Flop) | `switch (flow_flip_flop(n13)) { case 0: { ... } break; case 1: { ... } break; }` |
| Switch on Enum | `switch (flow_switch_enum<Mode>(State)) { case Mode::Closed: { ... } break; ... }` – die Fälle sind die Werte der Aufzählung, bei ihren Namen; der Text hat einen für jeden Wert |
| Knoten, der wartet (Delay, Tween) | `flow_delay(n8, 0.5, [this] { ... });` – was danach läuft, ist ein Lambda |
| Schritt, der einfach weitergeht | `debug_print(text, true);` |
| was ein Knoten gibt und anderswo gelesen wird | Member `struct { Float value = 0.0; } n12;`, gelesen als `n12.value` |
| was ein Knoten sich merkt (Schleifenindex, Do Once, Wartezeit, Zufallsfolge) | Member `kern::Node<12> n12;` bzw. `struct : kern::Node<12> { Int index = 0; } n12;` |
| Wert, den mehrere Eingänge lesen | ein Name: `const auto n7 = [&] { return math_add(a, b); };`, gelesen als `n7()` |
| Schritt, zu dem mehrere Drähte führen (oder keiner) | eine eigene Methode `void go9() { ... }`, jeder Draht ist ein Aufruf `go9();` |
| Wert, den niemand liest | `(void)math_add(1.0, 2.0);` in `void detached() { ... }` |
| Position, Kommentar, Kommentarboxen, nächste Id | `KERN_LAYOUT(Name) { node(id, x, y, "Kommentar"); box(...); next_id(n); }` |

Der Name einer Knotenfunktion ist der Knotentyp mit `_` statt `.` (`math.add` → `math_add`).

### Typen und Literale

| Pin-Typ | C++ | Literal |
|---|---|---|
| Boolean | `Bool` | `true`, `false` |
| Integer | `Int` | `12`, `-3` |
| Float | `Float` | `0.5`, `12.0`, `1e+21` |
| Vector | `Vector` | `Vector{1.0, -2.5, 3.0}` |
| Text | `Text` | `"a \"b\"\n"` (Escapes `\n \t \r \" \\` und oktal `\ooo`) |
| Actor | `Actor` | `Actor{7}` |
| Liste eines dieser Typen | `List<Bool>`, `List<Int>`, `List<Float>`, `List<Vector>`, `List<Text>`, `List<Actor>` | `List<Float>{1.0, 2.5}`, leer: `List<Float>{}` |
| Aufzählung des Blueprints, Liste ihrer Werte | ihr Name: `Mode`, `List<Mode>` | `Mode::Open`, `List<Mode>{Mode::Closed, Mode::Open}`, leer: `List<Mode>{}` |
| Struktur des Blueprints, Liste davon | ihr Name: `Weapon`, `List<Weapon>` | `Weapon{"Knife", Kind::Melee, 25.0}` – ein Wert für jedes Feld, in der Reihenfolge der Deklaration; `List<Weapon>{Weapon{...}, Weapon{...}}`, leer: `List<Weapon>{}` |
| Liste fester Länge – der Typ eines **Platzes**: einer Variablen, eines Felds, eines Ein- oder Ausgangs, einer lokalen Variablen (siehe „Listen fester Länge“) | `Fixed<Float, 16>`, `Fixed<Weapon, 4>` | `Fixed<Float, 3>{1.0, 2.5}` – die Einträge bis zum letzten, der nicht null ist: Was nicht dasteht, ist null, wie in C++ (lauter Nullen: `Fixed<Float, 3>{}`); mehr als die Länge ist ein Fehler |

Zahlen stehen in der kürzesten Form, die genau denselben Wert wieder ergibt. Jeder Draht, dessen
Enden verschiedene Typen haben, ist ein `kern::to<Typ>(...)` – keine stille Umwandlung. Der Wert
einer Aufzählung wandelt sich in nichts um, auch nicht mit `kern::to` (siehe „Aufzählungen“);
für den Wert einer Struktur gilt dasselbe (siehe „Strukturen“).

### Listen

Eine Liste hält Werte **eines** Typs, in ihrer Reihenfolge; der erste Eintrag hat den Index 0. In
C++ ist `List<Float>` ein `std::vector`, dessen `[]` nie neben die Liste greift (`kern::List`,
siehe „Plätze“) – eine Listen-Variable ist ein Member wie jede andere. Jeder Eintrag ist ein
eigenes Objekt, auch ein `Bool` (`std::vector<bool>` packt seine Einträge in Bits, und auf ein Bit
kann nichts zeigen): Darum ist jeder Eintrag jeder Liste ein Platz.

```cpp
List<Float> Scores = List<Float>{1.0, 2.5};
List<Text> Names = List<Text>{};

KERN_EVENT(on1, event_custom<"Hit">)
void on1(Actor actor, Float value, Text text) {
  list_add(Scores, value);
  Total = 0.0;
  for (auto it4 = flow_for_each<Float>(n4, Scores); it4.next();) {
    Total += n4.item;
  }
  if (list_contains<Text>(Names, text)) {
    debug_print(kern::to<Text>(Scores), true);
  }
}
```

- **Eine Liste auf einem Draht ist ein Wert.** Wer sie liest, hat die Liste, wie sie in dem
  Moment war – eine Kopie, kein Verweis. Geändert wird nur ein Platz: Die Schritte Add, Insert,
  Remove Index, Remove Item und Clear nennen den Platz ihrer Liste – eine Variable oder eine
  Liste darin – als Einstellung, und in C++ steht er als erstes Argument (nach `n<id>`), weil
  die Funktion genau diese Liste ändert. Eine ganze Liste setzt man wie jede Variable
  (`Scores = werte;`), einen Eintrag an seinem Platz (`Scores[i] = wert;`).
- **Kein Index führt ins Leere.** Get Item jenseits der Liste gibt den Nullwert des Typs, ein
  Platz neben der Liste (`Scores[99]`) ist ein eigener Eintrag mit den Defaults, von dem nichts
  bleibt,
  Remove Index tut dort nichts, Insert fügt vor dem ersten bzw. hinter dem letzten Eintrag ein,
  Find gibt −1. Es gibt kein Lesen oder Schreiben neben der Liste – in keiner Stufe.
- **For Each geht durch die Liste, wie sie beim Start der Schleife war.** Was der Rumpf mit der
  Variablen tut (hinzufügen, leeren), ändert den Lauf nicht. Der Stand der Schleife gehört dem
  Knoten (`n9`), wie der Index einer For-Schleife: Startet der Rumpf dieselbe Schleife noch
  einmal (über ein Event), läuft die innere ganz, und die äußere ist danach zu Ende.
- **Eine Liste wird nur zu Text.** `kern::to<Text>(Scores)` ergibt `[1, 2.5]`; sonst passt eine
  Liste nur in einen Eingang ihres eigenen Typs, und nichts wird still zu einer Liste. (Eine Liste
  einer Aufzählung wird auch nicht zu Text: die Namen ihrer Werte kennt nur die Aufzählung.)
- **In einem Zug.** Was `std::vector` mit einer Anforderung von Speicher tut, tut ein Schritt:
  `list_resize(Scores, 10, 0.0);` (Set Length: am Ende abgeschnitten oder mit dem Wert
  aufgefüllt – kleiner als 0 ist leer, mehr als 16.777.216 Einträge werden es nicht),
  `list_append(Scores, weitere);` und `list_insert_list(Scores, index, weitere);` (eine ganze
  Liste hinein, in ihrer Reihenfolge – auch die Liste in sich selbst: ihre Einträge, wie sie
  waren), `list_reserve(Scores, 64);` (Platz vorhalten: Nichts, was man lesen kann, ändert sich –
  die Liste wächst danach so weit, ohne noch einmal Speicher anzufordern). Eine Schleife aus
  einzelnen Add wächst mehrmals; darum gibt es diese Schritte.
- **Die generischen Knoten** (Length, Get Item, Contains, Find, Make List, For Each, auch Select
  und Reroute) tragen den Typ als Einstellung; im Editor nimmt ein solcher Knoten den Typ des
  ersten Drahts an, der an ihn kommt.
- **Was sie tun, steht einmal:** `kern::logic::list` in `kern/logic/nodes.h` – die Knoten des
  Interpreters und die Funktionen der Klasse rufen dieselben Funktionen auf.
- Gameplay: „All Actors with Tag“ (`actor_all_with_tag(tag)`) gibt eine `List<Actor>`.

### Listen fester Länge

Ein **Platz** kann als Liste fester Länge deklariert sein: Er hat immer genau so viele Einträge.
In C++ ist `Fixed<Float, 16>` ein `std::array` – es liegt dort, wo es steht, und fordert keinen
Speicher an (eine `List` ist ein `std::vector`: Jede hat ihren eigenen Speicher). Damit kostet
eine Matrix im Blueprint, was sie im C++ kostet.

```cpp
KERN_STRUCT(Mat4) {
  Fixed<Float, 16> m = Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
};
Fixed<Float, 3> Axis = Fixed<Float, 3>{0.0, 0.0, 1.0};

struct ScaledResult { Fixed<Float, 3> v = Fixed<Float, 3>{}; };
ScaledResult Scaled(const Fixed<Float, 3>& v, Float by = 2.0) {
  Fixed<Float, 3> Made = Fixed<Float, 3>{};
  for (auto it3 = flow_for(n3, 0, 2); it3.next();) {
    Made[n3.index] = math_mul(v[n3.index], by);
  }
  return {std::move(Made)};
}
```

- **Die Länge gehört zur Deklaration des Platzes**, nicht zum Wert: Eine Variable, ein Feld einer
  Struktur, ein Eingang (als Wert oder als Referenz), ein Ausgang und eine lokale Variable einer
  Funktion können sie haben – 1 bis 65.536 Einträge. Was durch einen Pin fließt, ist eine Liste:
  Ein Pin hat keine Länge, und die Knoten, die Listen lesen (Length, Get Item, Contains, Find, For
  Each), lesen eine Liste fester Länge wie jede andere – in der Klasse dort, wo sie steht
  (`kern::logic::Items`, ohne Kopie).
- **Einträge werden gesetzt und gelesen wie die einer Liste** (`Axis[1] = wert;`,
  `Jetzt.m[i]`), auch neben der Liste gilt dasselbe (siehe „Plätze“).
- **Nichts ändert die Länge.** Add, Insert, Remove, Clear, Set Length, Append, Insert List und
  Reserve an einem solchen Platz sind ein Fehler des Graphen (`"Axis" is a list of a fixed length
  (3): its entries are set …`).
- **Was als Ganzes dort gesetzt wird, hat danach die Länge des Platzes:** eine längere Liste wird
  abgeschnitten, eine kürzere mit dem Nullwert ihrer Einträge aufgefüllt – bei einem Set, bei
  Make einer Struktur, bei dem Wert, den ein Aufruf einem Eingang gibt, bei dem, was ein Return
  gibt, bei einem Wert von außen (`Instance::set`). Ein Take lässt lauter Nullwerte zurück. In
  der Klasse tut das die Zuweisung an `Fixed`, im Interpreter `logic::fitted`.
- **Als Ganzes gelesen ist es eine Liste** – eine Kopie mit eigenem Speicher, wie
  `std::vector(a.begin(), a.end())`. Von einem festen Platz an einen festen (`A.m = B.m;`) wird
  nichts angefordert.
- **Make List gibt, was es bekommt, als das, was es ist** – so viele Einträge, in der Klasse ein
  `Fixed`: an einem festen Platz gesetzt (`Jetzt.m = list_make<Float, "16">(...)`,
  `struct_make<Mat4>(list_make<Float, "16">(...))`) fordert es keinen Speicher an; wo eine Liste
  gebraucht wird, wird es eine.
- **Eine Referenz ist der Platz selbst, nichts wandelt um:** `Fixed<Float, 3>&` bekommt einen
  Platz, der als `Fixed<Float, 3>` deklariert ist – keine `List<Float>` und keine andere Länge.
- **Im Dokument** steht die Länge als `"count"` neben dem Typ; im Editor als „Length“ neben dem
  Typ einer Variablen, eines Felds, eines Pins oder einer lokalen Variablen (0: beliebig lang).

### Aufzählungen

Ein Blueprint kann Aufzählungen (Enums) deklarieren: einen eigenen Typ mit benannten Werten, etwa
`Mode` mit den Werten `Closed`, `Opening`, `Open`. Von diesem Typ kann eine Variable sein, ebenso
eine Liste (`List<Mode>`), die Ein- und Ausgänge einer Funktion und Pins. `KERN_ENUM` deklariert
dafür im `struct` des Blueprints ein `enum class Mode : kern::i64` – auch dieser Text ist C++, das
ein Compiler übersetzt, so wie er dasteht:

```cpp
struct Door : kern::Blueprint {
  // ---- types ----
  /// What the door is doing.
  KERN_ENUM(Mode, Closed, Opening, Open);

  // ---- variables ----
  Mode State = Mode::Closed;
  List<Mode> Seen = List<Mode>{Mode::Closed, Mode::Open};
  Text Log = "";

  // ---- functions ----
  struct TakeResult { Mode now = Mode::Closed; Int count = 0; };
  TakeResult Take(Mode to = Mode::Open, List<Mode> more = List<Mode>{}) {
    State = to;
    Seen = more;
    return {State, list_length<Mode>(more)};
  }

  // ---- events ----
  KERN_EVENT(on1, event_begin)
  void on1() {
    switch (flow_switch_enum<Mode>(State)) {
      case Mode::Closed: {
        State = Mode::Opening;
      } break;
      case Mode::Opening: {
      } break;
      case Mode::Open: {
        Log = text_append(Log, enum_name<Mode>(State), "");
      } break;
    }
    for (auto it7 = flow_for_each<Mode>(n7, enum_values<Mode>()); it7.next();) {
      list_add(Seen, n7.item);
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<7> { Mode item = Mode::Closed; Int index = 0; } n7;
};
```

- **Deklariert wird über der ersten Verwendung.** `KERN_ENUM(Name, Wert, Wert …);` steht im
  Abschnitt „types“, über den Variablen. Ein `///`-Kommentar darüber ist die Beschreibung der
  Aufzählung.
- **Ein Wert ist genau einer ihrer Werte,** geschrieben `Mode::Open`; eine Liste davon ist
  `List<Mode>{Mode::Closed, Mode::Open}`.
- **Nichts wandelt sich um.** Ein Wert einer Aufzählung ist weder Zahl noch Text: `State = 1;`,
  `Count = State;` und `kern::to<Int>(State)` lehnt der Leser ab, mit der Zeile und dem Grund.
  Auch ein Wert der einen Aufzählung passt nicht dorthin, wo eine andere verlangt ist, und eine
  `List<Mode>` wird – anders als die übrigen Listen – nicht zu Text. Wo eine Zahl oder ein Text
  gebraucht wird, sagt ein Knoten, was gemeint ist (Enum to Index, Enum to Text – siehe die
  Tabelle unten).
- **Die Einstellung, die die Aufzählung nennt, ist der Typ selbst** – wie jede Einstellung, die
  einen Typ nennt: `enum_name<Mode>(State)`. Die generischen Knoten stehen im Typ der
  Aufzählung: `logic_select<Mode>(pick, a, b)`, `list_get<Mode>(Seen, 0)`,
  `value_reroute<List<Mode>>(Seen)`.
- **Switch on Enum** ist ein `switch` über den Wert:
  `switch (flow_switch_enum<Mode>(wert)) { case Mode::Closed: { ... } break; ... }`. Die Fälle
  sind die Werte der Aufzählung, bei ihren Namen, und der Text hat einen Fall für jeden Wert (so
  will C++ ein `switch` über ein `enum` haben); ein Fall, an dem kein Draht hängt, ist leer. Beim
  Lesen ist ein Fall, der fehlt, dasselbe wie ein leerer.
- **Von außen** – Code, der eine Variable einer Instanz setzt oder eine ihrer Funktionen aufruft
  (`Instance::set`, `Instance::call`) – ist, was kein Wert der Aufzählung ist, ihr erster Wert.
- **In jeder Stufe gleich:** Interpreter, JIT und die Klasse des Blueprints liefern dieselben
  Ergebnisse – Wert für Wert geprüft, wie bei jedem Knoten.
- **Was sie tun, steht einmal:** `kern::logic::enums` in `kern/logic/nodes.h`, für die Knoten des
  Interpreters und für die Funktionen der Klasse.

Die Knoten einer Aufzählung und ihre Funktionen (`E` ist die Aufzählung):

| Knoten | C++ | Ergebnis |
|---|---|---|
| Switch on Enum | `flow_switch_enum<E>(wert)` | der Weg des Werts: ein `case` je Wert |
| Equal (Enum) | `enum_equal<E>(a, b)` | `Bool`: Sind A und B derselbe Wert? |
| Enum to Text | `enum_name<E>(wert)` | der Name des Werts |
| Text to Enum | `enum_from_name<E>(text)` | `.value`: der Wert mit diesem Namen; `.found`: Gibt es ihn? Heißt kein Wert so, ist `.value` der erste Wert und `.found` falsch |
| Enum to Index | `enum_index<E>(wert)` | die Stelle des Werts unter den Werten der Aufzählung; die erste ist 0 |
| Index to Enum | `enum_from_index<E>(index)` | `.value`: der Wert an dieser Stelle; `.valid`: Gibt es sie? Wenn nicht, ist `.value` der erste Wert und `.valid` falsch |
| All Values | `enum_values<E>()` | eine `List<E>` mit jedem Wert, in ihrer Reihenfolge – etwa für ein For Each |

### Strukturen

Ein Blueprint kann Strukturen (Structs) deklarieren: einen eigenen Typ mit benannten Feldern,
etwa `Weapon` mit den Feldern `Name`, `Type`, `Damage`. Jedes Feld hat einen Typ und einen
Default. Von diesem Typ kann eine Variable sein, ebenso eine Liste (`List<Weapon>`), die Ein- und
Ausgänge einer Funktion und Pins. `KERN_STRUCT` deklariert dafür im `struct` des Blueprints ein
gewöhnliches C++-`struct` – auch dieser Text ist C++, das ein Compiler übersetzt, so wie er
dasteht:

```cpp
struct Arena : kern::Blueprint {
  // ---- types ----
  KERN_ENUM(Kind, Melee, Ranged);
  /// What a weapon does.
  KERN_STRUCT(Weapon) {
    Text Name = "";
    Kind Type = Kind::Melee;
    Float Damage = 10.0;
    List<Float> Falloff = List<Float>{};
  };

  // ---- variables ----
  Weapon Held = Weapon{"Knife", Kind::Melee, 25.0, List<Float>{}};
  List<Weapon> Rack = List<Weapon>{};

  // ---- events ----
  KERN_EVENT(on1, event_begin)
  void on1() {
    Held.Damage = math_mul(struct_break<Weapon>(Held).Damage, 2.0);
    list_add(Rack, struct_make<Weapon>("Bow", Kind::Ranged, 12.0, List<Float>{1.0, 0.5}));
  }
};
```

- **Deklariert wird über der ersten Verwendung.** `KERN_STRUCT(Name) { Felder };` steht im
  Abschnitt „types“, nach den Aufzählungen und über den Variablen. Ein `///`-Kommentar darüber
  ist die Beschreibung der Struktur. Ein Feld steht da wie eine Variable: `Typ Name = Default;`.
- **Ein Feld** ist ein Wert-Typ, eine Aufzählung des Blueprints, eine Struktur, die **über** dieser
  deklariert ist, oder eine Liste von einem davon. Keine Struktur enthält sich selbst – auch nicht
  über andere. Eine Struktur hat mindestens ein Feld und höchstens 96.
- **Ein Wert steht mit jedem Feld da,** in der Reihenfolge der Deklaration:
  `Weapon{"Knife", Kind::Melee, 25.0, List<Float>{}}`; eine Struktur darin ebenso
  (`Slot{Weapon{...}, 1}`), eine Liste davon ist `List<Weapon>{Weapon{...}, Weapon{...}}`.
- **Make** baut einen Wert aus einem Wert je Feld: `struct_make<Weapon>(name, kind, damage, falloff)`.
  **Break** gibt die Felder eines Werts; gelesen wird ein Feld bei seinem Namen:
  `struct_break<Weapon>(Held).Damage`. Ein Feld einer Variablen ist ein **Platz**: gelesen
  `Held.Damage`, gesetzt `Held.Damage = wert;` – die anderen Felder bleiben, wie sie sind (siehe
  „Plätze“). Wird gelesen, was der Schritt gibt (das Feld, wie es jetzt ist):
  `n5.value = (Held.Damage = wert);`.
- **Nichts wandelt sich um.** Ein Wert einer Struktur ist weder Zahl noch Text, und er passt nur
  dorthin, wo dieselbe Struktur verlangt ist; `kern::to<Text>(Held)` lehnt der Leser ab. Auch
  eine `List<Weapon>` wird nicht zu Text.
- **Die Einstellung, die die Struktur nennt, ist der Typ selbst.** Die generischen Knoten stehen
  im Typ der Struktur: `logic_select<Weapon>(pick, a, b)`, `list_get<Weapon>(Rack, 0)`,
  `flow_for_each<Weapon>(n7, Rack)`. Ein Eintrag jenseits der Liste ist der Default der Struktur –
  jedes Feld mit seinem Default.
- **Gleich sind zwei Werte, wenn alle ihre Felder gleich sind.** So vergleichen Contains, Find
  Item und Remove Item (`list_contains<Weapon>(Rack, Held)`); in C++ ist das der Operator `==`
  der Struktur, den `KERN_STRUCT` mitbringt.
- **Von außen** – Code, der eine Variable einer Instanz setzt oder eine ihrer Funktionen aufruft
  (`Instance::set`, `Instance::call`) – ist, was keine Struktur dieses Typs ist, ihr Default:
  kein Struktur-Wert, einer mit anderen Feldern, einer, in dem ein Feld nicht von seinem Typ ist.
  In einer Liste gilt das Eintrag für Eintrag.
- **In der Datei des Graphen** ist ein Wert ein Objekt seiner Felder bei ihren Namen
  (`{"Name": "Knife", "Damage": 25}`), eine Liste davon ein Array solcher Objekte. Ein Feld, das
  dort fehlt, hat seinen Default.
- **In jeder Stufe gleich:** Interpreter, JIT und die Klasse des Blueprints liefern dieselben
  Ergebnisse – Wert für Wert geprüft, wie bei jedem Knoten. Der Klebstoff der Klasse
  (`logic::native_glue`) wandelt zwischen dem Wert der Engine und dem C++-`struct` Feld für Feld,
  mit Code, der je Struktur geschrieben wird.
- **Was sie tun, steht einmal:** `kern::logic::structs` in `kern/logic/nodes.h`, für die Knoten
  des Interpreters und für die Klasse.

Die Knoten einer Struktur (`S` ist die Struktur):

| Knoten | C++ | Ergebnis |
|---|---|---|
| Make S | `struct_make<S>(feld, feld, …)` | ein Wert der Struktur aus einem Wert je Feld |
| Break S | `struct_break<S>(wert).Feld` | das Feld des Werts |
| Find Row (Find S by Feld) | `list_find_row<&S::Feld>(liste, wert)` | `.found`, `.row`, `.index`: der erste Eintrag einer Liste von S, dessen Feld der Wert ist |
| Set an einem Platz (Set Variable mit Pfad) | `Variable.Feld = wert;` | ändert das eine Feld der Variablen; gibt das Feld, wie es jetzt ist |

### Plätze

Ein **Platz** ist eine Variable – oder was von ihr aus über Felder und Listeneinträge erreicht
wird, beliebig tief: `Held.Damage`, `Scores[i]`, `Cars[i].Wheels[j].Slip`. C++ schreibt ihn so, und
so steht er im Text. Im Graphen nennt der Knoten seine Variable (Einstellung „variable“, bei einer
lokalen dazu „scope“) und den Weg von ihr zum Platz (Einstellung „path“: `[].Wheels[].Slip` – ein
Feld bei seinem Namen, ein Listeneintrag als `[]`); für jeden Listeneintrag hat der Knoten einen
Index-Eingang (`path0`, `path1` …).

```cpp
KERN_STRUCT(Wheel) { Float Slip = 0.5; List<Float> Marks = List<Float>{}; };
KERN_STRUCT(Car) { Float Speed = 0.0; List<Wheel> Wheels = List<Wheel>{}; };
List<Car> Cars = List<Car>{};

void Skid(Int car = 0, Int wheel = 0, Float by = 0.1) {
  Cars[car].Wheels[wheel].Slip += by;                    // Add to Variable an einem Platz
  Cars[car].Speed = math_mul(Cars[car].Speed, 0.5);      // Set und Get an einem Platz
  list_add(Cars[car].Wheels[wheel].Marks, by);           // ein Listen-Schritt auf einer Liste an einem Platz
}
```

- **Jeder Knoten, der eine Variable nennt, kann einen Platz in ihr nennen:** Get, Set, Add to
  Variable und Take und die Schritte, die eine Liste ändern (Add, Insert, Remove Index, Remove
  Item, Clear). Es gibt keinen Unterschied zwischen einer Variablen und einem Platz in ihr: Was an
  dem einen geht, geht an dem anderen.
- **Geändert wird am Platz.** Nichts wird herausgelesen und zurückgeschrieben: In der Klasse ist
  `Cars[car].Wheels[wheel].Slip += by;` genau diese Zeile C++ auf den Membern; der Interpreter
  ändert den Wert der Variablen an derselben Stelle (`Instance::change`).
- **Ein Platz neben einer Liste ist ein eigener Eintrag mit den Defaults, von dem nichts
  bleibt.** Führt ein Index des Wegs neben seine Liste, geht der Platz dort in einem eigenen
  Eintrag weiter, der die Defaults seines Typs hat – eine Struktur die Defaults ihrer Felder.
  Gelesen wird dort dieser Default; was dort geschrieben wird, liest niemand; eine Funktion, die
  den Platz als Referenz bekommt, arbeitet auf diesem Eintrag, und nach dem Aufruf ist er weg.
  Was ein Schritt gibt, ist, was er auf diesem Eintrag gibt (`n5.value = (Cars[99].Speed +=
  1.0);` gibt den Default plus 1).
- **Einen Platz zu nennen ändert nichts.** Jede Nennung neben einer Liste hat ihren eigenen
  Eintrag: Zwei solche Plätze sind zwei Einträge, und was eine Funktion hält, bleibt, was es
  ist, wenn irgendwo ein anderer Platz genannt wird – auch in derselben Anweisung, gleich in
  welcher Reihenfolge ein Compiler sie ausrechnet. In jeder Stufe dasselbe: in der Klasse durch
  `kern::List` (`kern/logic/nodes.h`), deren `[]` neben der Liste einen solchen Eintrag gibt (er
  bleibt, bis die Engine die Klasse wieder verlassen hat – `beside::Visit`); im Interpreter durch
  `Instance::spot`. Wo der Eintrag da ist, kostet `[]` in der Klasse einen Vergleich.
- **Ein Platz ist vom Typ dessen, was dort steht.** Die Pins des Knotens haben diesen Typ (der
  Wert eines Set, die Menge eines Add, der Eintrag eines Listen-Schritts); Add zählt Zahlen, ein
  Listen-Schritt braucht eine Liste. Führt der Weg durch etwas, das die Variable nicht hat –
  ein Feld, das es nicht gibt, ein Eintrag von etwas, das keine Liste ist –, meldet `check()` es
  mit der Stelle (`Held.Damage is float: it has no entries`).
- **Dokumente von früher:** Set Field und Set List Item waren eigene Knoten. Beim Laden wird
  jeder zu dem Set an seinem Platz, das er ist (`Held.Damage = wert;`, `Scores[index] = wert;`).

### Übergeben statt kopieren

Eine Liste, ein Text, eine Struktur mit einem von beiden hält eigenen Speicher. Wo C++ so einen
Wert weiterreicht, statt ihn zu kopieren, tut es die Klasse eines Blueprints auch – sonst kostete
sie mehr als der Quelltext, aus dem sie kommt. Dafür gibt es zwei Dinge: `std::move` im Text,
und den Baustein **Take**.

```cpp
struct PfadResult { List<Vector> punkte = List<Vector>{}; };
PfadResult Pfad(List<Vector> start = List<Vector>{}, Int n = 3) {
  List<Vector> Punkte = List<Vector>{};
  Punkte = std::move(start);           // der Eingang wird hier zum letzten Mal gelesen
  for (auto it3 = flow_for(n3, 1, n); it3.next();) {
    n4 = Glatt(std::move(Punkte));     // Punkte wird gesetzt, bevor es wieder gelesen wird:
    Punkte = std::move(n4.punkte);     // Punkte = Glatt(Punkte) kopiert nichts
  }
  return {std::move(Punkte)};          // der Aufruf ist vorbei
}

struct LeerenResult { List<Float> log = List<Float>{}; };
LeerenResult Leeren() {
  var_take(n6, Log);                   // Take: gibt die Liste, Log ist danach leer
  return {std::move(n6.value)};        // was der Schritt direkt davor gab
}
```

- **`std::move(wert)` steht genau dort, wo den Wert danach nichts mehr braucht.** Das sind zwei
  Fälle, und der Text eines Graphen hat es in genau diesen:
  - **eine lokale Variable der Funktion oder einer ihrer Eingänge als Wert, als Ganzes, wo sie zum
    letzten Mal gelesen wird:** Nichts sonst, was der Schritt liest, nennt sie (`return {Punkte,
    list_length<Vector>(Punkte)};` kopiert), und auf keinem Weg von dem Schritt aus wird sie
    gelesen, bevor ein Set sie als Ganzes neu setzt. In einem Return ist das immer so – C++ tut
    dort dasselbe bei `return Punkte;`. In einer Schleife zählt die nächste Runde mit: Was jede
    Runde liest, wird kopiert; was jede Runde erst neu setzt, wird übergeben. Jeder Aufruf hat
    seine lokalen Variablen für sich (in der Klasse sind es die von C++), darum ist das allein
    eine Frage der Schritte dieses Aufrufs.
  - **was der Schritt direkt davor gab** (`n4.punkte`), wenn dieser Eingang es als einziger liest.
    „Direkt davor“: ein einfacher Schritt, dessen einer Weg zu diesem Schritt führt, und kein
    anderer Weg führt hierher – der Schritt läuft genau einmal nach jedem seiner Läufe, und
    dazwischen läuft nichts. (Was ein Schritt gibt, steht in einem Member der Klasse, den alle
    Läufe teilen: Läge ein Schritt dazwischen, der einen Handler auslösen kann, könnte ein zweiter
    Lauf den Wert nehmen, den der erste noch liest.)

  Das gilt für jeden Schritt (ein Set, ein Aufruf, ein Return, ein Knoten der Engine); ein While
  liest seine Bedingung vor jeder Runde, ein Wert-Knoten, wann immer er gelesen wird – sie
  übergeben nichts. Variablen des Blueprints bleiben, wo sie sind: Die nimmt nur ein Take.
- **Der Graph sagt dazu nichts.** Der Draht ist derselbe Draht; übergeben oder kopiert, der Wert,
  der ankommt, ist derselbe, in jeder Stufe. `std::move` ist allein Sache des Textes:
  `Writer::hands_over` (notation.cpp) entscheidet es aus dem Graphen – „wird noch gebraucht“ ist
  dort eine Rechnung über die Schritte eines Aufrufs (`Writer::needed_after`: wohin die Wege
  führen, was jeder Schritt liest und setzt; wo ein Weg im Rumpf einer Schleife endet, folgt die
  Schleife wieder, in einem Block einer Sequence die Blöcke danach).
- **Der Leser nimmt `std::move` nur dort an.** Fehlt es, wo der Text es hätte, ist es derselbe
  Graph (und `to_cpp` schreibt es hin). Steht es woanders – an einem Wert, der noch gebraucht
  wird, an einer Variablen des Blueprints, an einem Wert-Knoten, an einer Zahl –, wird der Text
  mit der Stelle abgelehnt: C++ ließe dort einen Platz leer, den der Blueprint noch liest.
- **Take** (`var.take`, „Take Variable“) ist ein Schritt an einem Platz, wie Set: Er gibt, was dort
  steht, und lässt den leeren Wert des Typs zurück – eine Liste ohne Einträge, einen leeren Text,
  0, eine Struktur mit ihren Defaults. In der Klasse ist das `std::exchange(platz, {})`: Die Liste
  wird weitergereicht, nicht kopiert. Im Interpreter tauscht der Knoten den Wert am Platz gegen
  den leeren (`Instance::change`). Neben einer Liste gibt er den Default, wie jedes Lesen dort.
  Für die Regel der gehaltenen Listeneinträge (Referenz-Eingänge) ist ein Take ein Set: Er setzt
  etwas an die Stelle dessen, was dort stand.

### Texte des Programms

Ein Text, der im Blueprint ausgeschrieben ist – der Wert an einem Pin, der Default einer Variablen
oder eines Felds, ein Eintrag einer ausgeschriebenen Liste –, gehört zum Programm. In der Klasse
ist `Text` deshalb kein `std::string`, sondern `kern::logic::Text` (`nodes.h`):

- **Ein ausgeschriebener Text wird nicht kopiert.** `Text Name = "spawncity-facades";` verweist auf
  die Zeichen, die das Programm ohnehin hält – wie ein `const char*` auf ein Literal in C++. Den
  Wert weiterzugeben, in eine Struktur zu setzen, aus einer Funktion zu geben, zu vergleichen
  fordert keinen Speicher an, wie lang der Text auch ist.
- **Ein Text, der beim Laufen entsteht**, hält seine Zeichen selbst, so wie ein `std::string`:
  Append, eine Zahl als Text, was die Engine gibt. Ihn zu kopieren kostet, was es in C++ kostet;
  wo er zum letzten Mal gelesen wird, reicht `std::move` ihn weiter.
- **Die Knoten lesen die Zeichen, wo sie stehen** (`text_equal`, `text_contains`, `text_length`);
  `text_append` fordert für das Ergebnis einmal Speicher an. Der Name eines Werts einer
  Aufzählung (`enum_name`) ist ebenfalls ein Text des Programms.
- **Im Interpreter ändert sich nichts**: Dort ist ein Text ein Wert wie jeder andere. Was ein
  Blueprint rechnet, ist in jeder Stufe dasselbe, Zeichen für Zeichen.

Wer die Klasse von Hand aus C++ ruft: Ein `std::string` wird zu einem `Text` (er wird gehalten),
ein Literal auch (es wird referenziert). Ein Puffer, in den geschrieben wird (`char b[64]`), wird
abgelehnt – seine Zeichen gehören in einen `std::string`. Gelesen wird ein `Text` als
`std::string_view` (`text.view()`), als eigener `std::string` mit `text.str()`.

### Text formatieren

**Format Text** (`text.format`) macht einen Text aus einem Format, wie es `printf` in C hat, und den
Werten seiner Umwandlungen – Zeichen für Zeichen, was `snprintf` gibt:

```cpp
Text Zeit = text_format<"%02d:%06.3f">(-1, minuten, sekunden);   // "03:07.250"
Text Kurz = text_format<"%s">(3, name);                          // höchstens 3 Bytes, wie snprintf mit Größe 4
```

- Das Format ist die Einstellung; seine Umwandlungen bestimmen die Eingänge, in ihrer Reihenfolge:
  `%d %i %u %x %X %o %c` nehmen ein Int, `%f %F %e %E %g %G` ein Float, `%s` einen Text, `%%` ist ein
  Prozentzeichen. Flags `- + Leerzeichen 0 #`, Breite und Genauigkeit als Zahlen (bis 512); `*`
  gibt es nicht. Breiten der ganzen Zahlen wie in C: `hh` 8, `h` 16, ohne 32, `l ll j z t` 64 Bit.
- Der erste Eingang **Most**: höchstens so viele Bytes (das `size - 1` von `snprintf`); kleiner als
  0: alles.
- Zahlen werden exakt gerundet (`std::to_chars`, die Rundung von `printf`: die nächste Dezimalzahl
  zum binären Wert, bei Gleichstand gerade) – auf jeder Maschine derselbe Text. (Einzige bekannte
  Abweichung von glibc: `%#g`, wo glibc gegen den C-Standard eine Null zu wenig schreibt.)
- Ein Format, das der Knoten nicht kennt, ist ein Fehler des Graphen, mit Grund.

### Referenz-Eingänge

Ein Eingang einer Funktion ist ein **Wert** – der Wert, den er beim Aufruf hatte – oder eine
**Referenz**: Dann gibt der Aufruf keinen Wert, sondern nennt einen Platz (eine Variable, ein
Feld, einen Listeneintrag – siehe „Plätze“), und die Funktion arbeitet auf diesem Platz, wo er
steht. Nichts wird hinein- oder herauskopiert.

```cpp
/// Ändert das Rad, das der Aufruf nennt.
void Skid(Wheel& wheel, Float by = 0.5) {
  wheel.Slip += by;
  list_add(wheel.Marks, wheel.Slip);
}
/// Liest das Auto, wo es steht.
KERN_PURE WearResult Wear(const Car& car, Int wheel = 0) {
  return {car.Wheels[wheel].Slip, car.Name};
}

void Drive(Car& car, Int wheel = 0) {
  car.Speed += 1.0;
  Skid(car.Wheels[wheel], 0.25);          // ein Platz in dem, was die Funktion selbst bekommen hat
}

KERN_EVENT(on1, event_tick)
void on1(Float delta) {
  Drive(Cars[2], 0);                       // der Platz: ein Eintrag einer Liste
  Heat = Wear(Mine, 1).slip;
}
```

- **`Car& car`** – die Funktion liest und ändert den Platz. **`const Car& car`** – sie liest ihn
  nur; so ein Eingang kann auch ein Platz sein, der selbst nur gelesen wird (ein Eingang als
  Wert, eine andere const-Referenz), und eine Funktion, die als Wert gelesen wird (`KERN_PURE`),
  hat nur solche. Ein Referenz-Eingang hat keinen Default: Der Aufruf nennt seinen Platz.
- **Die Referenzen stehen vor den Werten** (Regel, `check()`): Ein Eingang als Wert steht mit
  seinem Default im Kopf (`Float by = 0.5`), und in C++ hat nach einem Parameter mit Default jeder
  weitere einen. Der Editor hält diese Reihenfolge (ein Eingang, der Referenz wird, rückt hinter
  die letzte Referenz), der Leser meldet einen Kopf, der sie nicht hat.
- **In der Funktion ist er wie eine Variable benannt:** gelesen `car`, `car.Speed`,
  `car.Wheels[i].Slip`; gesetzt und geändert wie jeder Platz. Im Graphen sind das die
  Variablen-Knoten mit den Einstellungen „variable“ (der Eingang) und „scope“ (die Funktion); der
  Function Entry hat für einen Referenz-Eingang keinen Pin. Auch in einem Eingang als Wert kann
  ein Platz gelesen werden (`given.Wheels[0].Slip`) – geändert wird er nicht.
- **Der Aufruf nennt den Platz** statt eines Werts: eine Variable des Blueprints, eine lokale
  Variable oder ein Eingang der Funktion, in der der Aufruf steht, und von dort Felder und
  Listeneinträge. Im Graphen sind das Einstellungen des Call-Knotens (`<Eingang>.variable`,
  `<Eingang>.path`, je Listeneintrag ein Index-Eingang `<Eingang>.path0` …; „scope“ nennt die
  Funktion, in der der Aufruf steht). Der Platz hat genau den Typ des Eingangs – nichts wandelt
  sich um.
- **In jeder Stufe dasselbe.** In der Klasse ist der Eingang eine Referenz von C++. Im
  Interpreter ist er für die Dauer des Aufrufs der Platz, den der Aufruf genannt hat
  (`Instance::Spot`): Jeder Knoten, der ihn liest oder ändert, tut es dort. Nennt der Aufruf
  einen Platz neben einer Liste, arbeitet die Funktion auf dessen eigenem Eintrag (siehe
  „Plätze“) – und wem sie ihn weitergibt, auf demselben.
- **Ein Eintrag, der gehalten wird, bleibt, wo er ist** (Regel, `check()`): Nennt ein Aufruf
  einen Platz in einer Liste (`Cars[i]`, `Cars[i].Wheels[j]`), dann ändert nichts, was die
  Funktion tut – sie selbst oder was sie aufruft –, die Länge dieser Liste oder setzt eine andere
  an ihre Stelle; und sie führt keinen Schritt aus, der Handler starten kann (Call Event, Call on
  Actor, ein Gameplay-Schritt: `NodeDef::raises`) – ein Handler könnte dasselbe tun. So bleibt
  „kein Index führt ins Leere“ auch dort wahr, wo die Referenz eine
  Referenz von C++ ist. Die Liste selbst als Referenz zu geben und zu ändern
  (`Fill(Cars[i].Marks)`), ein Feld des Eintrags zu ändern oder eine andere Liste – das alles
  geht.
- **Von außen** (`Instance::call`, der Kleber der Klasse) nimmt ein Referenz-Eingang einen Wert:
  Er ist dann eine eigene Variable des Aufrufs, die mit diesem Wert beginnt, und was die
  Funktion aus ihr gemacht hat, steht hinter den Ausgängen in `results` – für jeden
  Referenz-Eingang, der nicht const ist, in der Reihenfolge der Eingänge.
- **Call on Actor gibt Werte:** Eine Funktion eines anderen Blueprints (`KERN_EXTERN`) hat keine
  Referenz-Eingänge.

### Bibliotheken

Eine **Bibliothek** ist ein Blueprint, der nichts behält: Er hat Typen, Konstanten
(`KERN_CONST`) und Funktionen, keine Variablen, keine Events, keine State Machines – und keinen Knoten, der zwischen zwei Aufrufen
etwas behält (Do Once, Gate, Flip Flop, die eigene Folge eines Random, ein Warten). Andere
Blueprints **benutzen** sie: Sie haben dann ihre Typen und rufen ihre Funktionen direkt auf. So
steht jede Funktion einmal im Projekt.

```cpp
struct Workshop : virtual kern::Library {                       // eine Bibliothek
  KERN_STRUCT(Wheel) { Float Slip = 0.5; List<Float> Marks = List<Float>{}; };

  /// Ändert das Rad, das der Aufruf nennt.
  void skid(Wheel& wheel, Float by = 0.5) {
    wheel.Slip += by;
    list_add(wheel.Marks, wheel.Slip);
  }
  struct wearResult { Float slip = 0.0; };
  KERN_PURE wearResult wear(const Wheel& wheel) {
    return {wheel.Slip};
  }
};

struct Garage : virtual kern::Blueprint, virtual Workshop {     // ein Blueprint, der sie benutzt
  List<Wheel> Spare = List<Wheel>{};                            // Wheel: der Typ der Bibliothek
  Float Seen = 0.0;

  void Check(Int at = 0) {
    Workshop::skid(Spare[at], 1.0);                             // direkt, mit einem Platz des Aufrufers
    Seen = Workshop::wear(Spare[at]).slip;
  }
};
```

- **Im Text ist die Bibliothek eine Basis.** `virtual kern::Library` sagt, dass der Blueprint
  eine Bibliothek ist (`kern::Library` ist `kern::Blueprint`); wer Bibliotheken benutzt, nennt sie
  als weitere Basen: `struct Garage : virtual kern::Blueprint, virtual Workshop, virtual Parts`.
  Auch eine Bibliothek kann Bibliotheken benutzen (`virtual kern::Library, virtual Parts`). Das
  `virtual` heißt: Ein Blueprint und seine Bibliotheken teilen, was ein Blueprint ist – es gibt
  eine Welt, ein Budget, jede Bibliothek einmal.
- **Der Aufruf nennt die Bibliothek:** `Workshop::skid(Spare[at], 1.0);`, was ein Aufruf gab, ist
  `Workshop::twiceResult n5;`. In C++ ist das der Aufruf einer Funktion einer Basisklasse: einmal
  übersetzt, direkt. Eine Referenz ist ein Platz des Aufrufers, wie bei einer eigenen Funktion.
- **Der Name einer Bibliothek ist ein Name von C++ wie jeder andere:** Eine Variable, eine
  Funktion, ein Eingang darf so heißen wie eine Bibliothek, die der Blueprint benutzt (`Rng rng`
  neben `rng::next(rng)`) – wo die Bibliothek gemeint ist, steht `Bibliothek::name`, und C++ sucht
  dort nur nach einem Typ. Darum heißt kein Typ des Blueprints wie eine seiner Bibliotheken. Die
  Eingänge und lokalen Variablen einer Bibliotheksfunktion sind Namen der Bibliothek: Sie stoßen
  an keinen Namen dessen, der sie benutzt.
- **Die Typen der Bibliothek hat, wer sie benutzt** – auch die der Bibliotheken, die sie selbst
  benutzt –, ohne sie noch einmal zu deklarieren: In C++ ist `Wheel` *ein* Typ, darum bindet eine
  Referenz. Ein Name ist ein Typ: Nennt ein Blueprint einen Typ, den zwei seiner Bibliotheken
  deklarieren – oder er selbst und eine Bibliothek –, wird er abgelehnt. Was er nicht nennt,
  stört ihn nicht.
- **Die Konstanten einer Bibliothek sind ihre eigenen.** Ihre Funktionen lesen sie; wer die
  Bibliothek benutzt, hat seine eigenen – auch unter demselben Namen (in C++ verdeckt der eigene
  Member den der Basis; im einen Graphen des Interpreters heißt die der Bibliothek
  `Workshop::Step`).
- **Der Graph hält, was er von seinen Bibliotheken benutzt – und nur das:** wie die Funktionen
  aussehen, die er aufruft (Funktionen ohne Rumpf, `from` und der Name `Workshop::skid`), und die
  Typen, die er nennt (`from`: die Bibliothek, die sie deklariert) – in seinen Variablen, in dem,
  was seine Funktionen nehmen, geben und behalten, in den Einstellungen seiner Knoten –, samt
  denen, aus denen ihre Felder sind. Der Text zeigt beides nicht noch einmal. Was er nicht
  benutzt, hält er nicht: Bekommt eine Bibliothek einen Typ oder eine Funktion dazu, bleiben ihre
  Benutzer, wie sie sind – ihr Graph, ihr Hash, ihre übersetzte Klasse. Wer einen Text liest, der
  Bibliotheken nennt, braucht sie (`from_cpp(…, blueprints)`) – wie ein C++-Compiler ihren
  Header: Beim Lesen und bei einem Schritt im Editor steht alles der Bibliotheken zur Wahl
  (`take_libraries(…, whole)`), danach hält der Graph wieder, was er benutzt.
- **Interpreter und JIT führen einen Graphen aus:** Beim Start wird der Blueprint mit seinen
  Bibliotheken gebunden (`logic::link`): Die Funktionen, die er aufruft – und die sie aufrufen –,
  stehen dann mit ihrem Rumpf darin, unter ihrem Namen `Workshop::skid`. Ein Aufruf ist danach ein
  Aufruf wie jeder andere, in jeder Stufe dasselbe – mit den Typen, die diese Rümpfe nennen
  (der Blueprint selbst muss sie nicht halten). Die Regeln, die Aufrufen folgen (kein
  Selbstaufruf, „nur rechnen“, gehaltene Einträge), urteilen über diesen einen Graphen. Hält der
  Blueprint etwas anders, als seine Bibliothek es jetzt hat, bindet er nicht – und sagt, was.
- **Die Klasse** ist registriert unter dem Hash dieses einen Graphen: Ändert sich eine
  Bibliothek, passt die übersetzte Klasse ihrer Benutzer nicht mehr, und sie laufen im
  Interpreter, bis neu gebaut ist.

### Formeln

Eine Formel ist der Ausdruck selbst, als Lambda über ihre Eingänge. Operatoren, die in C++ etwas
anderes bedeuten oder fehlen, sind dort Aufrufe: `/` ist `kern::math::div` (durch 0 ergibt 0), `%`
ist `kern::math::mod`, `^` ist `kern::math::pow`; die Funktionen und `pi` stehen in `kern::math`.
Für ganze Zahlen, wie eine Maschine sie hat, gibt es `band`, `bor`, `bxor`, `shl`, `shr` (die Bits
einer Zahl) und `wrap32`, `wrap32s`, `mul32` (Rechnen, das bei 32 Bit umbricht), dazu `single` (die Zahl, wie
eine 32-Bit-Kommazahl sie hält) – in jeder Stufe
dieselben Funktionen von `kern::math`.
Zurückgelesen wird der Lambda-Rumpf als Formel, in ihrer einen kanonischen Schreibweise
(`Formula::text()`).

## Wo was steht

**Parameter oder Member.** Die Werte eines Events oder einer Funktion sind in ihrem eigenen Rumpf
die Parameter (`delta`). Wer sie von woanders liest – nach einem Warten, aus einer `go`-Methode,
aus einem anderen Event –, liest den Member `n1.delta`; dafür beginnt der Rumpf mit
`n1 = {delta};`.

**Nach einem Warten.** Was auf einen wartenden Knoten folgt, läuft später und ist ein Lambda, das
nur `this` behält. Darin gibt es keine Parameter mehr, nur Member und Variablen.

**Ein Event, das sich selbst wieder auslöst.** Die Werte eines Events gehören dem einen Lauf
seines Handlers – wie die Parameter eines Methodenaufrufs. Löst ein Handler sein eigenes Event
aus (Call Event), hat der innere Lauf seine Werte, und wenn er zu Ende ist, hat der äußere wieder
die eigenen: im Rumpf ohnehin (Parameter), und auch für den, der sie von außen liest (`n1.value`).
Graph und Text halten das gleich (`Runtime::fire`, der Kleber).

**Eine Funktion, die wieder aufgerufen wird, während sie läuft.** Sich selbst aufrufen darf eine
Funktion nicht (Regel). Löst sie aber ein Event aus, kann dessen Handler sie noch einmal aufrufen
– das ist ein Aufruf wie jeder andere, in C++ wie im Graphen. Jeder Aufruf hat seine Parameter;
ist der Handler zu Ende, hat der unterbrochene Aufruf die eigenen wieder, auch für den, der sie
von außen liest (`n20.x` aus einer `go`-Methode). Für den Graphen macht das `Runtime::fire`, für
die Klasse ihr Kleber – beide an derselben Stelle: um den Handler, der in einem laufenden
Blueprint startet.

**Lokale Variablen.** Eine lokale Variable einer Funktion ist in C++ eine lokale Variable ihres
Rumpfs: Jeder Aufruf beginnt mit ihrem Default, und mit dem Aufruf ist sie vorbei. Darum steht
jeder Knoten, der sie liest oder setzt, im Rumpf ihrer Funktion – nicht in einem Lambda nach einem
Warten, nicht in einer `go`-Methode, nicht in einem Event (Regel, wie bei Return). Im Graphen nennt
der Knoten die Funktion in seiner Einstellung „scope“; der Text schreibt sie nicht, denn der Rumpf,
in dem er steht, sagt es. Der Interpreter und das JIT halten sie wie die anderen Variablen (Zahlen
in Slots), setzen sie bei jedem Aufruf auf den Default und geben – wie bei den Parametern – einem
unterbrochenen Aufruf die eigenen zurück, wenn ein Handler die Funktion währenddessen noch einmal
aufgerufen hat (`Runtime::fire`).

**Aufrufe auf anderen Blueprints.** Call on Actor geht in jeder Stufe denselben Weg:
`Runtime::call_on` sucht den Blueprint des Targets (ohne Target den des Levels), ruft die Funktion
beim Namen und gibt die Ergebnisse so zurück, wie der Aufrufer sie erklärt hat – der Reihe nach,
umgewandelt wie an einem Draht, und die erklärten Defaults, wo das Ziel die Funktion nicht hat. In
der Klasse ist `func_call_on` genau dieser Aufruf (`kern::out` nimmt die Ergebnisse entgegen);
`KERN_EXTERN` erklärt nur, nichts davon wird aufgerufen. So ein Aufruf zählt mit, wenn Ketten
ineinander liegen (höchstens 64), denn er kann im Kreis gehen: Eine Funktion, die sich auf ihrem
eigenen Actor aufruft, läuft noch einmal, während sie läuft – jeder Aufruf mit seinen eigenen
Parametern und lokalen Variablen, in C++ ohnehin, im Interpreter und im JIT, weil `call_on` dem
unterbrochenen Aufruf die seinen zurückgibt. Was Knoten geben und behalten (`n7`), gehört wie immer
dem Blueprint, nicht dem Aufruf.

**Wie tief es geht.** Gezählt werden Events, die aus Handlern heraus Handler erreichen: höchstens
64 ineinander, dann wird die Kette gestoppt und es wird gemeldet – in jeder Stufe beim selben
Event. Funktionsaufrufe zählen nicht mit: Sie können sich nicht selbst erreichen, und in C++ sind
sie gewöhnliche Aufrufe. Werte lesen einander so tief, wie der Graph es vorgibt (ein Ausdruck in
C++ hat auch keine Grenze).

**Wege, die sich wieder treffen.** Führen alle Wege eines Branch (oder Switch) in denselben Knoten,
steht der hinter dem `if` – wie man es in C++ schreibt:

```cpp
if (Open) {
  Count += 1;
} else {
  Count += 2;
}
debug_print("done", false);
```

Kehrt der True-Weg immer zurück (`return`), steht der False-Weg ohne `else` dahinter. Ein Branch
im False-Weg ist ein `else if`.

**Wege, die sich anders treffen.** Führen Drähte aus verschiedenen Zusammenhängen in einen Knoten
(oder durch einen anderen als seinen ersten Exec-Eingang), ist er eine Methode `go<id>`, bei
mehreren Exec-Eingängen `void go11(Text entry)` mit Aufrufen wie `go11("open");`.

**Ein Weg endet.** Nach `return` ist Schluss. Nach einem wartenden Knoten, einer Sequence, einem
Knoten ohne Exec-Ausgang oder einem `go<id>()` ist der Weg im Graphen ebenfalls zu Ende – in C++
ginge es aber mit der nächsten Zeile weiter. Deshalb steht danach in derselben Liste nichts mehr,
und ein `if`, dessen Zweig so endet, hat keine gemeinsame Fortsetzung. Der Leser lehnt Texte ab,
in denen C++ weiterliefe, wo der Graph aufhört.

**Funktionen mit Ausgängen.** Endet ein Weg ohne Return-Knoten, behalten die Ausgänge ihre
Vorgaben: Der Rumpf endet dann mit `return {};` (kein Knoten).

## Regeln: was kein Blueprint sein darf

Damit jeder Blueprint genau ein C++ hat, gelten für Blueprints diese Regeln. `check()` meldet
Verstöße als Fehler am betroffenen Knoten; der Editor nimmt solche Namen gar nicht erst an.

- **Namen** von Variablen, Funktionen und Funktions-Pins sind C++-Namen: Buchstaben, Ziffern,
  `_`; kein Wort von C++ (`int`, `class` …) und keins der Schreibweise (`Bool`, `Int`, `Float`,
  `Vector`, `Text`, `Actor`, `loop`, `machine`, `entry`, `detached`, `kern`, `n12`, `go12`,
  `on12`, `it12`, `machine12`); nicht der Name eines Knotens (`math_add`).
- Variablen und Funktionen teilen sich einen Namensraum; `FnResult` gehört der Funktion `Fn`.
  Ein Funktions-Eingang heißt nicht wie eine Variable oder Funktion, eine Variable nicht wie der
  Ausgang eines Events im selben Blueprint (`value`, `actor`, `delta` …).
- Der Name einer **Aufzählung** und die Namen ihrer Werte sind ebenfalls C++-Namen: Buchstaben,
  Ziffern und `_`, keine Ziffer am Anfang, kein Wort von C++ und keins der Schreibweise
  (`Float` …). Der Name einer Aufzählung endet nicht auf `_list`, und sie heißt nicht wie eine
  Variable oder Funktion. Keine zwei Werte einer Aufzählung heißen gleich, und jede Aufzählung
  hat mindestens einen Wert.
- Der Name einer **Struktur** und die Namen ihrer Felder sind ebenfalls C++-Namen, mit denselben
  Regeln: kein Wort von C++ und keins der Schreibweise, nicht der Name eines Knotens. Der Name
  einer Struktur endet nicht auf `_list`, und sie heißt nicht wie eine Variable, eine Funktion
  oder eine Aufzählung. Keine zwei Felder einer Struktur heißen gleich, und kein Feld heißt wie
  eine Aufzählung oder Struktur des Blueprints. Eine Struktur hat mindestens ein Feld und
  höchstens 96; ein Feld ist von einem Typ, der über der Struktur deklariert ist – keine Struktur
  enthält sich selbst.
- **Ein Exec-Ausgang hat einen Draht.** Mehreres nacheinander ist eine Sequence.
- **Return** liegt auf einem Weg seiner eigenen Funktion: nicht nach einem Warten, nicht in einem
  Event; treffen sich Wege davor, dann alle aus einem Branch oder Switch.
- **Ein Wert, der bei jedem Lesen neu ist** (Random …), wird von genau einem Eingang gelesen –
  und alles, was ihn weiterreicht, ebenfalls. Zwei Leser bekämen zwei verschiedene Zahlen; wer
  dieselbe Zahl zweimal braucht, legt sie in eine Variable.
- Die Eingänge einer Formel sind C++-Namen.
- **Kein Weg führt ohne Warten in sich selbst zurück.** Schritte wiederholt ein Schleifen-Knoten
  (While, For). Mit einem Delay dazwischen ist es ein Timer – das geht. Ein Gate, das sich selbst
  schließt, oder ein Do Once, das sich zurücksetzt, ist kein Weg zurück: In Reset, Open, Close,
  Toggle und Stop endet der Weg (`NodeDef::first_entry_only`). Das Update eines Tweens läuft
  schon mit Play (`NodeDef::now_outputs`): denselben Tween aus seinem eigenen Update neu zu
  starten, ist ein Weg zurück. Aus Finished ist es ein Tween, der sich wiederholt – Finished
  kommt immer erst mit dem letzten Update in einem späteren Schritt, nie mit Play selbst (auch
  bei Dauer 0).
- **Eine lokale Variable** wird auf einem Weg ihrer eigenen Funktion gelesen und gesetzt: nicht
  nach einem Warten, nicht in einem Event oder einer anderen Funktion; treffen sich Wege davor,
  dann alle aus einem Branch oder Switch. Sie heißt nicht wie eine Variable, eine Funktion oder
  ein Typ des Blueprints und nicht wie ein Pin ihrer Funktion; zwei Funktionen dürfen je eine
  desselben Namens haben.
- **Eine Funktion eines anderen Blueprints** (`KERN_EXTERN`) hat hier keinen Rumpf: kein Function
  Entry, kein Return, kein Call und kein Function Value mit ihrem Namen, keine lokalen Variablen,
  nicht pure. Aufgerufen wird sie mit Call on Actor – ein Schritt.
- **Eine Funktion läuft nicht noch einmal, während sie läuft:** Sie ruft sich nicht selbst auf,
  auch nicht über andere Funktionen. (Nach einem Warten ist sie zu Ende; sich dann aufzurufen,
  geht.)
- **Eine Funktion, die als Wert gelesen wird (pure), rechnet nur:** kein Set oder Add (außer auf ihre eigenen lokalen Variablen), kein
  Warten, kein Do Once, Gate oder Flip Flop, keine Aktion – und was sie als Schritt aufruft,
  rechnet ebenfalls nur. Verzweigen, Schleifen und andere Werte lesen darf sie. So ist es gleich,
  wie oft und in welcher Reihenfolge ein Wert gelesen wird.
- **Ein Referenz-Eingang** nennt im Aufruf einen Platz seines Typs; geändert wird er nur, wenn er
  nicht const ist und der Platz geändert werden darf (kein Eingang als Wert, keine
  const-Referenz). Ein Eingang als Wert und eine const-Referenz werden gelesen, nicht gesetzt.
  Eine Funktion, die als Wert gelesen wird, hat nur const-Referenzen; eine Funktion eines
  anderen Blueprints (`KERN_EXTERN`) keine. Die Referenzen stehen vor den Eingängen als Wert
  (die haben einen Default, und in C++ hat nach einem Parameter mit Default jeder weitere einen).
- **Ein Eintrag, der gehalten wird, bleibt, wo er ist:** Solange ein Aufruf einen Platz in einer
  Liste hält, ändert nichts in ihm die Länge dieser Liste oder ersetzt sie, und nichts in ihm
  kann Handler starten (siehe „Referenz-Eingänge“).
- **Add to Variable** zählt Zahlen: Integer oder Float. Menge und Ergebnis haben den Typ der
  Variablen – eine ganze Zahl wird ganzzahlig erhöht (`Count += 1;`).

Außerdem gilt, unabhängig von der Speicherreihenfolge: Die Handler eines Events laufen in der
Reihenfolge ihrer Knoten-Ids.

## Was der Leser annimmt – und was nicht

`from_cpp` liest C++, wie man es schreibt, und `to_cpp` schreibt es danach in der kanonischen
Form:

- `if (c) { return {1.0}; } return {2.0};` (frühes Return), `else if`, Anweisungen nach einem
  `if` (sie hängen an allen offenen Enden seiner Zweige).
- ganze Zahlen, wo ein Float steht (`Heat = 1;`).
- Anweisungen nach den Blöcken einer Sequence sind ihr letzter Block.
- Fehlt bei Switch on Enum der `case` eines Werts, ist das dasselbe wie ein leerer.
- Fehlt bei einem Feld einer Struktur oder bei einer Variablen `= Wert`, ist ihr Default der leere
  Wert des Typs (bei einer Struktur: jedes Feld mit seinem Default).
- Ohne `KERN_LAYOUT`-Block bekommen neue Knoten Ids und Plätze automatisch.
- Ein Wert ohne `std::move`, wo der Text des Graphen es hat (derselbe Graph).
- Eine ausgeschriebene Liste (`List<Float>{1.0, 2.0}`) als Default eines Platzes fester Länge: Es
  sind seine Einträge, wie C++ sie nähme (nicht mehr, als er hat).

Abgelehnt wird – immer mit Zeile, Spalte und dem Grund, oft mit der richtigen Schreibweise:

- alles außerhalb der Schreibweise (`int x = 3;`, `break`, Operatoren wie `a + b` außerhalb einer
  Formel, eigene Klassen),
- die Knoten, die ihre eigene Form haben, als Aufruf (`var_set<"Heat">(wert);` – es heißt
  `Heat = wert;`; `list_set(Scores, 0, wert);` – es heißt `Scores[0] = wert;`),
- ein Weg, der durch etwas führt, das die Variable nicht hat (`Heat.X = 1.0;`,
  `Held.Damage[0]`),
- unbekannte Namen, falsche Anzahl von Werten, falscher Typ ohne `kern::to`,
- eine Struktur, die nicht mit jedem Feld ausgeschrieben ist (`Weapon has 3 field(s) (Label, Type,
  Damage): Weapon{...} gives a value for each, in that order`), ein Feld vom falschen Typ, ein
  Feld, das die Struktur nicht hat (`Held.Range = 1.0;`), eine Struktur, die über ihrer
  Deklaration benutzt wird oder sich selbst enthält,
- `std::move` an einem Wert, der noch gebraucht wird (`std::move hands over a value nothing needs
  again …`),
- ein Parameter in einem Lambda nach einem Warten,
- fehlende oder falsche Deklarationen (`n12 is not declared: struct { Float value = 0.0; } n12;`),
- ein Layout, das nicht zu den Knoten des Textes passt.

## Die kanonische Form

`canonical(g)` ist der Blueprint so, wie sein Text ihn ergibt: die Knoten nach Id sortiert, die
Drähte in fester Reihenfolge, jede Einstellung und jeder eingetippte Wert ausgeschrieben, Formeln
in ihrer einen Schreibweise. Es ist derselbe Blueprint (`same_blueprint`) und läuft genau gleich.
Nicht erhalten bleibt nur, was keine Wirkung hat: ein eingetippter Wert unter einem Draht, Felder
einer Transition, die ihre Art nicht benutzt.

## Vom Compiler ausgeführt

Der Text ist gültiges C++20. Er beginnt mit `#include "kern/logic/blueprint.h"` und braucht sonst
nichts: Der Blueprint ist ein `struct`, seine Variablen sind Member, seine Funktionen Methoden,
jeder Knoten ein Funktionsaufruf, jede Formel ein Lambda. Kein Rückruf in eine Knoten-Bibliothek,
kein Nachschlagen eines Pins oder einer Variablen beim Namen – der Compiler sieht alles und
optimiert es wie handgeschriebenen Code.

**Eine Bedeutung je Knoten.** Was ein Knoten rechnet, steht genau einmal: als typisierte Funktion
in `kern/logic/nodes.h` (`math_clamp`, `text_append`, `kern::math::…` für Formeln, die
Umwandlungen `kern::to`, die Knoten mit Gedächtnis wie Do Once und Gate auf ihren vier Zahlen).
Der Interpreter ruft für seinen Knoten dieselbe Funktion auf wie der Text. Graph und Text können
deshalb nicht verschieden rechnen – es gibt nur eine Stelle, an der gerechnet wird.

**Was so läuft: alles.** Jeder Knoten der Bibliothek ist eine Funktion von `blueprint.h` oder
eine Form der Schreibweise (`NodeDef::native`; die Tests kompilieren einen Blueprint mit jedem
einzelnen). `logic::native_ready(graph, &warum)` sagt es für einen Blueprint: Er muss fehlerfrei
kompilieren (`check()`), mehr nicht. Bringt ein Spiel eigene Knoten mit, die noch keine Funktion
sind, nennt `native_ready` den Knoten; so ein Blueprint läuft im Interpreter oder im JIT.

**Gameplay-Knoten.** Was ein Gameplay-Knoten tut, steht genau einmal: als typisierte Funktion in
`kern::game::nodes` (`kern/game/blueprint_nodes.h`, definiert in `src/game/logic_nodes.cpp`) –
`actor_location(spiel, self, actor)`, `combat_damage(spiel, self, ziel, schaden, verursacher)`.
Der Knoten der Bibliothek ruft sie auf, und die Klasse ruft sie auf:

```cpp
KERN_EVENT(on1, game_overlap_begin)
void on1(Actor actor, Actor other) {
  Visits += 1;
  game_add_score(5.0);
  if (actor_has_tag(other, "player")) {
    actor_spawn<"Crate">(n7, Actor{0}, actor_location(other), Vector{0.0, 0.0, 0.0});
    actor_impulse(n7.spawned, Vector{0.0, 0.0, 300.0});
  }
}
```

- Das Spiel und der eigene Actor kommen aus der Welt der Klasse (`kern_world`); ein leer
  gelassener Actor-Eingang (`Actor{0}`) meint Self – in der Funktion, also für Graph und Text
  gleich.
- Ein Wert-Knoten mit mehreren Ausgängen gibt ein `struct`, dessen Felder wie die Pins heißen:
  `actor_velocity(a).speed`, `walker_tof(Actor{0}).distance`.
- Ein Schritt, dessen Ausgänge gelesen werden, bekommt seinen Member als erstes Argument und
  füllt ihn: `physics_trace(n40, start, end, Actor{0});`, danach `n40.hit`, `n40.location`.
- Einstellungen sind Template-Argumente: `actor_spawn<"Crate">`, `game_sound<"builtin:confirm">`,
  `game_input<"jump", "pressed">`.
- For Each Actor with Tag ist eine Schleife wie jede: `for (auto it9 = flow_for_each_tag(n9,
  "crate"); it9.next();) { ... n9.actor ... }`. Die Liste gilt, wie sie beim Start der Schleife
  war, und gehört dem Knoten (wie der Index eines For Loop).
- Die Events des Spiels (On Begin Overlap, On Hit, On Damaged, On Input Action …) sind Handler
  wie alle Events; wer welches bekommt, entscheidet die Laufzeit.
- Ohne Spiel (eine Klasse, die jemand einfach anlegt; ein Test ohne Welt) tun diese Knoten
  nichts und geben nichts – ihre Eingänge werden trotzdem gelesen, wie Argumente eben.

Wie ein Gameplay-Knoten *aussieht* (Name, Pins, Einstellungen), steht getrennt von dem, was er
tut: `src/game/logic_node_shapes.cpp` braucht nichts vom Spiel. Deshalb kennt
`tools/blueprint_cpp` die Knoten, ohne die Engine zu bauen; `logic_nodes.cpp` bindet die
Funktionen an (`Library::edit`).

**Eingänge und Zufall.** Jeder Eingang eines Knotens wird genau einmal ausgewertet, ob der Knoten
ihn dann benutzt oder nicht – wie die Argumente eines Aufrufs (Select liest beide Seiten). In
welcher Reihenfolge C++ die Argumente auswertet, legt der Compiler fest; damit das nichts ändert,
rechnen Werte nur (Regel für Pure-Funktionen), und **jeder Random-Knoten hat seine eigene
Folge** (`logic::random`, in seinem Member `kern::Node<id>`): Sie beginnt beim Seed des
Blueprints (einmal aus den Zufallszahlen der Welt gezogen, sobald ein Random-Knoten zum ersten
Mal fragt – deterministisch je Welt) und der Id des Knotens. Was ein Random-Knoten liefert, hängt
also nicht davon ab, welcher andere vor ihm dran war.

**Events.** Welches Event welchen Handler erreicht, entscheidet die Laufzeit (`Runtime::fire`:
Typ, Name, Ziel, die Reihenfolge nach Knoten-Id) – für Interpreter, JIT und Klasse dieselbe
Stelle. Die Klasse hat den Handler als Methode `on<id>`; der Kleber ruft sie mit den Werten des
Events. `KERN_EVENT(on5, event_custom<"Hit">)` sagt dem Leser und dem Editor, welcher Knoten die
Methode ist. Die Welt der Klasse (`kern_world`: Host, Laufzeit, Instanz) setzt die Engine; Print,
Game Time und Call Event gehen darüber. Ein Event, das selbst entscheidet, ob es fällig ist
(Every N Seconds zählt seine Zeit), tut das auf der Seite der Laufzeit – derselbe Code für jede
Stufe –, dann läuft der Handler.

**Warten.** `flow_delay(n8, 0.5, [this] { ... });` – was folgt, ist ein Lambda, das der Knoten in
seinem Member behält (`kern::Node::later`). Die Wartezeiten aller Stufen führt die Laufzeit an
einer Stelle (`Runtime::wait`); ist eine um, läuft der Knoten über den Kleber weiter
(`flow_delay_resume`). Was Delay, Retriggerable Delay und Tween dabei rechnen (läuft schon?
welches Alpha?), sind dieselben Funktionen wie im Interpreter (`logic::flow::delay_start`,
`tween_step` …) auf den vier Zahlen des Knotens.

**State Machines.** `kern::Machine machine3 = machine("Door").state(...).on_event(...)` schreibt
die Maschine hin; geführt wird sie von der Laufzeit (Timer, Bedingungen, Events – derselbe Code
für jede Stufe) für den Graphen, aus dem die Klasse geschrieben wurde. Die Klasse fragt und setzt
Zustände darüber (`state_set`, `state_get`, `state_is`) und hat die State-Events als Handler.

**Zahlen.** `Float` ist `double`. Aus einer Kommazahl wird eine ganze Zahl überall gleich
(`kern::logic::whole`): zur Null hin abgeschnitten; „keine Zahl“ und unendlich ergeben 0; was
nicht mehr hineinpasst, das nächste Ende. Round und Floor benutzen dieselbe Regel.

`Int` ist eine ganze Zahl, **wie ein Blueprint sie hält** (`kern::logic::Whole`, `logic::normal`):
Unter jeder Zahl eines Blueprints liegt ein `double` (die Variablen des Interpreters, der JIT).
Ganze Zahlen sind deshalb bis 2^53 exakt; eine größere ist die nächste, die ein `double` hält,
und keine geht über die Enden hinaus: 2^63 − 1 und dasselbe negativ (gleich weit von 0, damit
sich jeder `Int` in C++ hinschreiben lässt). Das gilt für jeden `Int`, den es gibt – einen
Wert an einem Pin, eine Variable der Klasse –, also in jeder Stufe gleich. `Count += 1` addiert,
wie der Graph addiert: als Zahl, für jeden Wert definiert (eine Variable, die sich 70-mal
verdoppelt, bleibt am oberen Ende stehen; die Arithmetik von `i64` wäre dort undefiniert). Für
jede Funktion, die ein `i64` nimmt, ist ein `Int` eins.

**Im Spiel.** `tools/blueprint_cpp.sh level.klevel out.cpp` schreibt für jeden Blueprint des
Levels, der so laufen kann, seinen Text und dahinter den Kleber (`logic::native_glue`): eine
Klasse `Name_Native`, über die die Engine Funktionen und Variablen beim Namen erreicht
(`Instance::call`, `get`, `set`), angemeldet unter dem Hash des Graphen
(`register_native_blueprint`). Ein Spiel, das `out.cpp` einbindet, führt diese Blueprints als
kompilierten Code aus; `Instance::tier()` sagt dann „native C++“. Beim Namen wird nur an dieser
Grenze nachgesehen – in der Klasse ist jeder Aufruf ein direkter Aufruf.

An dieser Grenze baut der Kleber zuerst die Argumente, ruft dann die Funktion und gibt danach
zurück, was sie gab; ein Eingang per Wert wird der Funktion übergeben, nicht kopiert. Wer messen
will, was die Funktion selbst kostet, setzt `logic::native_probe`: Der Kleber meldet dort, wann
die Funktion beginnt und wann sie zurück ist – Umwandeln von Argumenten und Ergebnissen liegt
außerhalb. (Ein Test zählt so, wie oft die Klasse Speicher anfordert, und vergleicht das mit dem
C++, aus dem der Blueprint stammt.) Ohne Zuhörer ist es ein Vergleich mit null.

**Als fertiges Spiel.** `tools/export_game.sh level.klevel spiel.html` macht beides in einem
Zug: die Klassen schreiben, die Engine mit ihnen bauen (`kern/build.sh --studio`, ThinLTO – der
Compiler sieht Blueprint und Engine zusammen), die Seite schreiben. Der Editor im Browser kann
kein C++ kompilieren; sein „Package Game“ legt das Level neben das Modul des Editors, und der
Browser übersetzt die Blueprints beim Start (JIT). Der native Export ist derselbe Inhalt,
vorab kompiliert. Der Browser-Test `kern/tests/e2e/export_e2e.mjs` spielt ein so exportiertes
Level und prüft, dass seine Blueprints als Klassen laufen.

Das Level bleibt die eine Quelle. Die Klasse führt nur genau den Graphen aus, aus dem sie
geschrieben wurde: Der Hash lässt Positionen und Kommentare aus, alles andere ändert ihn. Ein
Blueprint, der seit dem Build geändert wurde, läuft im Interpreter, bis der Build die Datei neu
schreibt – nie als veralteter Code. `Runtime::native = false` lässt den Interpreter laufen, auch
wenn es die Klasse gibt (so vergleichen die Tests beide).

**Endlosschleifen.** Schleifen sind das Einzige in einem Blueprint ohne festes Ende (Wege zurück,
Selbstaufrufe und ein Tween aus seinem eigenen Update sind Regelverstöße). Die Laufzeit zählt die
Schleifendurchläufe eines Schritts für alle Stufen an einer Stelle (`Runtime::pass`,
`Runtime::budget` = 200 000); `while (loop(...))` und `it.next()` fragen dort nach. Ist die Grenze
überschritten, enden die Schleifen der Kette sofort, der Rest der Kette läuft zu Ende, dann ist
der Blueprint gestoppt und es wird gemeldet. Das ist für Graph und Text dasselbe – der Text kann
eine Kette nicht mittendrin abbrechen, also tut es auch der Interpreter nicht mehr.

## Der Beweis (Tests)

`kern/tests/run.sh logic_notation` – für jeden Graphen `g`:

1. `to_cpp(g)` gelingt, `from_cpp` liest den Text.
2. `same_blueprint(g, gelesen)`.
3. Der Text ist ein Fixpunkt: `to_cpp(gelesen)` ist Zeichen für Zeichen derselbe Text.
4. Der Graph ist ein Fixpunkt: nochmal gelesen ist es derselbe Graph (`==`); `canonical` ändert
   ihn nicht mehr.
5. Der gelesene Blueprint spielt das Szenario der Tier-Tests mit demselben Ergebnis.

Geprüft werden: alle Tier-Fälle (`logic_cases.h`), ein Graph mit **jedem Knotentyp der
Bibliothek** (jeder Eingang mit einem Wert, der nicht die Vorgabe ist; einmal alle Ausgänge
verdrahtet, einmal keiner), von Hand geschriebene Texte mit allen Formen, die Ablehnungen mit
ihren Stellen und die Regeln. In SpawnCity prüft `rules_wanted_blueprint_is_its_cpp` dasselbe
für den Blueprint „Wanted“ des Levels.

Für die native Ausführung: `kern/tests/run.sh` schreibt die Texte von acht Blueprints genau so,
wie `to_cpp` sie liefert (`kern_tests --emit-native`), kompiliert sie mit dem Compiler des
Systems (die Schalter der Engine: `-Wall -Wextra -Werror`, ASan, UBSan) und vergleicht **jeden
Aufruf Zeichen für Zeichen mit dem Interpreter** – Ergebnisse und alle Variablen
(`blueprint_native_test`, über 4500 Aufrufe). Jeder Aufruf läuft zweimal: direkt auf der Klasse
und beim Namen durch eine `logic::Instance`, die den Graphen als diese Klasse ausführt. Dazu
spielen die Tier-Fälle (`logic_cases.h`), die ein Compiler schon ausführt, als Klassen dasselbe
Szenario wie Interpreter und JIT – Begin Play, Events, Ticks, Wartezeiten, State Machines – mit
demselben Ergebnis: alle 24, auch Aufrufe auf einem Blueprint (`call_on`: eine Funktion, die sich
so selbst aufruft, eine erklärte Funktion, die niemand hat, und Aufrufe ohne Ende), lokale Variablen (`locals`: Zahlen und ein Text, zwei
Funktionen mit einer gleichnamigen, eine Pure-Funktion, die ihre eigene setzt, und ein Aufruf, den
ein zweiter Aufruf derselben Funktion unterbricht), Übergeben (`handing_over`: ein Return, das eine
lokale Variable und Eingänge weiterreicht, ein Set und ein Aufruf, die nehmen, was der Schritt davor
gab – und daneben dieselben Werte dort, wo sie noch gebraucht und also kopiert werden; Take an einer
Variablen, einem Feld, einem Listeneintrag, neben einer Liste und in einer lokalen Variablen), Listen in
einem Zug (`lists_at_once`: Set Length kürzt und füllt, Append und Insert List vorn, mittendrin, hinter dem
Ende und in sich selbst, Reserve), Listen fester Länge (`fixed_lists`: Variable, Feld – auch von Strukturen in
einer festen Liste –, Ein- und Ausgang, lokale Variable, Referenz; gesetzt mit längeren und kürzeren Listen,
über Make und Take), Listen (`lists`: jeder Schritt, For Each über eine Liste, die
ihr Rumpf ändert, und eine Schleife, deren Rumpf sie selbst wieder startet), Aufzählungen
(`enums`: eine Variable und eine Liste vom Typ einer Aufzählung, Switch on Enum, der Weg zu Index
und Name und zurück, alle Werte in einem For Each), Strukturen (`structs`: eine Variable und eine
Liste vom Typ einer Struktur mit einer Aufzählung, einer Struktur und Listen als Feldern; Make,
Break und Set Field, Vergleich Feld für Feld, ein Eintrag jenseits der Liste), die Endlosschleife (`infinite_loop`: der Wächter stoppt sie in
jeder Stufe beim selben Durchlauf), ein Handler, der sein eigenes Event auslöst
(`event_reentry`), eine Funktion, die über ein Event wieder aufgerufen wird, und Events, die
tiefer als 64 ineinander liegen (`function_reentry`), ganze Zahlen an ihren Enden
(`whole_numbers`), Tweens und Timer (`waits`), Zustände (`states`) und Zufall (`random`).
Die Blueprints: alle Ablaufformen (`Flow`), Funktionen als Wert und als Schritt (`Calls`), jeder
Draht zwischen verschiedenen Typen (`Casts`), Listen jedes Typs mit jedem Index, auch weit
jenseits der Liste (`Lists`), Aufzählungen als Variablen, in Listen und durch Funktionen, Switch
on Enum, der Weg zu Index und Name und zurück (`Enums`), Strukturen als Variablen, in Listen und
durch Funktionen, eine Struktur in einer Struktur, Make, Break und Set Field, die Listen-Schritte
auf einer Liste von Strukturen – und Werte von außen, die keine Struktur des verlangten Typs
sind (`Structs`) –, Return in einer Sequence (`Functions`) und **jeder
Wert-Knoten in jeder Einstellung als eigene Funktion** (`Nodes`, dazu 20 Formeln), jeweils mit
vielen Werten samt 0, −0, 1e19, unendlich und „keine Zahl“. `logic_native_blueprints` stellt
sicher, dass `native_ready` keinen Knoten durchlässt, der in diesen Fällen (und denen des Spiels,
siehe unten) nicht vorkommt – und dass das inzwischen jeder Knoten der Bibliothek ist.
Für die Gameplay-Knoten: zwei Levels (`game_cases.h`) – die **Arena**, deren sieben Blueprints
jeden Actor-, Physik-, Kampf- und Effekt-Knoten und jedes Event des Spiels benutzen, und der
Raum des **Roboters** mit allen Robots-Knoten. `kern_tests --emit-game-native` schreibt sie als
Level-Dateien, `tools/blueprint_cpp` – das Werkzeug, das auch der Build eines Spiels benutzt –
ihre Blueprints als Klassen; `game_native_test` bindet sie mit der ganzen Engine und spielt jedes
Szenario zweimal, im Interpreter und als Klassen (`GameWorld::logic_native`): Überlappungen,
Treffer, Pickup, Trigger, Schaden, Tod und Respawn, Eingaben, ein zweiter Spieler, Spielende; der
Roboter liest Kamera und ToF und wird gefahren. Verglichen wird alles, was man sehen kann – jede
Variable jedes Blueprints, Ort und Gesundheit jedes Actors, das HUD, die Meldungen des Spiels –,
Zeile für Zeile, an vielen Stellen des Szenarios. `game_blueprints_as_classes` stellt sicher,
dass die Szenarien wirklich auslösen, was sie sollen, und dass kein Gameplay-Knoten fehlt.
SpawnCitys Tests machen dasselbe mit dem Blueprint „Wanted“ des Levels (`wanted_native_test`);
dort laufen außerdem alle Regel-Tests auf der Klasse, wie im Spiel, und ein Level, dessen
Blueprint ohne neuen Build geändert wurde, fiele auf (`rules_wanted_blueprint` prüft die Stufe).

Für die Ansicht: `logic_notation_apply_keeps_ids_and_places` (der Code ohne Layout, auf seinen
Blueprint angewendet, ändert nichts; eine eingefügte, geänderte oder gelöschte Zeile lässt alle
übrigen Knoten, wo sie sind), `studio_logic_cpp_view` (ein Blueprint, im Editor als C++ aus dem
Nichts geschrieben, gespielt, im Text geändert, Undo) und im Browser `kern/tests/e2e/blueprint_e2e.mjs`
(Schritt 1b: mit Maus und Tastatur).
