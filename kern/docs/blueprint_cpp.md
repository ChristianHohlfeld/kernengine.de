# Blueprints als C++: die zweite Schreibweise

Ein Blueprint und sein C++-Text sind **dasselbe in zwei Schreibweisen**. Jeder Knoten ist genau
eine Anweisung oder genau ein Ausdruck; jede Anweisung und jeder Ausdruck des Textes ist genau ein
Knoten. Der Weg vom Graphen zum Text und der Weg zurück sind beide vollständig und verlustfrei –
nichts wird zur Laufzeit in einer Knoten-Registry nachgeschlagen, nichts geht beim Hin- und
Herwandeln verloren.

Gespeichert wird der Blueprint (im Level, `.klevel`). Das C++ ist die zweite Ansicht derselben
Sache: Man kann sie lesen und ändern, und was man ändert, ist der Blueprint.

```
kern/include/kern/logic/logic.h   to_cpp, from_cpp, apply_cpp, canonical, same_blueprint, cpp_name_ok, cpp_problem, cpp_names
kern/src/logic/notation.cpp       Shape (was steht wo), Writer (Graph → Text), Reader (Text → Graph)
kern/src/studio/logic_edit.cpp    logicCpp, logicCppApply (die C++-Ansicht des Editors)
kern/tests/test_main.cpp          logic_notation_*, studio_logic_cpp_view (der Beweis, siehe unten)
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

## Stand

- **Fertig:** die Schreibweise in beide Richtungen, für jeden Knotentyp der Bibliothek, mit den
  Regeln, die sie braucht, und dem Beweis als Test.
- **Fertig:** die C++-Ansicht im Editor (siehe unten): ansehen, ändern, anwenden.
- **Als Nächstes:** die native Ausführung genau dieses Textes (`kern/logic/blueprint.h`,
  typisierte Knotenfunktionen, kein Rückruf) und der native Export. Der Fahrplan steht in
  `kern/games/spawncity/BLUEPRINTS.md`. Bis dahin führen Interpreter, JIT und der bisherige Export
  die Blueprints aus; der Text ist noch nicht das, was der Compiler bekommt.

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
- **Download** lädt die Quelle, die der heutige Spiel-Build für diesen Graphen kompiliert.
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
      Count += 1.0;
    } else if (math_compare<">">(Heat, 5.0)) {
      Count += 2.0;
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
| Variable (mit Beschreibung) | `/// Beschreibung` und `Float Heat = 0.0;` |
| Funktion mit Ausgängen | `struct FnResult { Float y = 0.0; };` und `FnResult Fn(Float x = 1.5) { ... }` |
| Funktion ohne Ausgänge | `void Fn(...) { ... }` |
| Funktion „pure“ (als Wert-Knoten angeboten) | `KERN_PURE` vor der Funktion |
| Function Entry | der Anfang des Funktionsrumpfs; seine Ausgänge sind die Parameter |
| Return | `return {wert, wert};` bzw. `return;` |
| Event | `KERN_EVENT(on5, event_custom<"Hit">)` und `void on5(Actor actor, Float value, Text text) { ... }` |
| State Machine | `kern::Machine machine3 = machine("Door").state(...).on_event(...).after(...).when(...).automatic(...);` |
| Knoten-Einstellungen (Parameter) | Template-Argumente: `flow_tween<"smooth">`, `event_custom<"Hit">` |
| Eingänge eines Knotens | Funktionsargumente, in der Reihenfolge der Pins |
| unverdrahteter Eingang | der eingetippte Wert als Literal |
| Wert-Knoten (pure) | ein Ausdruck: `math_add(a, b)` |
| Get Variable | der Name der Variablen |
| Set / Add to Variable | `Heat = wert;` / `Heat += wert;` |
| Call Function | `Fn(args);` – oder `n14 = Fn(args);`, wenn seine Ergebnisse gelesen werden |
| Function Value | `Fn(args).y` |
| Formel-Knoten | `kern::formula([](Float a, Float b) -> Float { return a * b; }, wertA, wertB)` |
| Umwandlung an einem Draht (Int → Float …) | `kern::to<Float>(wert)` |
| Branch | `if (wert) { ... } else { ... }` |
| Sequence | `{ ... } { ... }` – ein Block je Ausgang |
| For Loop (jeder Schleifen-Knoten) | `for (auto it12 = flow_for(n12, 0, 9); it12.next();) { ... }` – danach folgt „Completed“ |
| While Loop | `while (loop(wert)) { ... }` – danach folgt „Completed“ |
| Knoten, der entscheidet, ob es weitergeht (Do Once, Gate, Do N) | `if (flow_do_once(n7, "exec", false)) { ... }` |
| Knoten mit mehreren Wegen (Switch, Flip Flop) | `switch (flow_flip_flop(n13)) { case 0: { ... } break; case 1: { ... } break; }` |
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

Zahlen stehen in der kürzesten Form, die genau denselben Wert wieder ergibt. Jeder Draht, dessen
Enden verschiedene Typen haben, ist ein `kern::to<Typ>(...)` – keine stille Umwandlung.

### Formeln

Eine Formel ist der Ausdruck selbst, als Lambda über ihre Eingänge. Operatoren, die in C++ etwas
anderes bedeuten oder fehlen, sind dort Aufrufe: `/` ist `kern::math::div` (durch 0 ergibt 0), `%`
ist `kern::math::mod`, `^` ist `kern::math::pow`; die Funktionen und `pi` stehen in `kern::math`.
Zurückgelesen wird der Lambda-Rumpf als Formel, in ihrer einen kanonischen Schreibweise
(`Formula::text()`).

## Wo was steht

**Parameter oder Member.** Die Werte eines Events oder einer Funktion sind in ihrem eigenen Rumpf
die Parameter (`delta`). Wer sie von woanders liest – nach einem Warten, aus einer `go`-Methode,
aus einem anderen Event –, liest den Member `n1.delta`; dafür beginnt der Rumpf mit
`n1 = {delta};`.

**Nach einem Warten.** Was auf einen wartenden Knoten folgt, läuft später und ist ein Lambda, das
nur `this` behält. Darin gibt es keine Parameter mehr, nur Member und Variablen.

**Wege, die sich wieder treffen.** Führen alle Wege eines Branch (oder Switch) in denselben Knoten,
steht der hinter dem `if` – wie man es in C++ schreibt:

```cpp
if (Open) {
  Count += 1.0;
} else {
  Count += 2.0;
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
- **Ein Exec-Ausgang hat einen Draht.** Mehreres nacheinander ist eine Sequence.
- **Return** liegt auf einem Weg seiner eigenen Funktion: nicht nach einem Warten, nicht in einem
  Event; treffen sich Wege davor, dann alle aus einem Branch oder Switch.
- **Ein Wert, der bei jedem Lesen neu ist** (Random …), wird von genau einem Eingang gelesen –
  und alles, was ihn weiterreicht, ebenfalls. Zwei Leser bekämen zwei verschiedene Zahlen; wer
  dieselbe Zahl zweimal braucht, legt sie in eine Variable.
- Die Eingänge einer Formel sind C++-Namen.

Außerdem gilt, unabhängig von der Speicherreihenfolge: Die Handler eines Events laufen in der
Reihenfolge ihrer Knoten-Ids.

## Was der Leser annimmt – und was nicht

`from_cpp` liest C++, wie man es schreibt, und `to_cpp` schreibt es danach in der kanonischen
Form:

- `if (c) { return {1.0}; } return {2.0};` (frühes Return), `else if`, Anweisungen nach einem
  `if` (sie hängen an allen offenen Enden seiner Zweige).
- `var_set<"Heat">(wert);` statt `Heat = wert;`; ganze Zahlen, wo ein Float steht (`Heat = 1;`).
- Anweisungen nach den Blöcken einer Sequence sind ihr letzter Block.
- Ohne `KERN_LAYOUT`-Block bekommen neue Knoten Ids und Plätze automatisch.

Abgelehnt wird – immer mit Zeile, Spalte und dem Grund, oft mit der richtigen Schreibweise:

- alles außerhalb der Schreibweise (`int x = 3;`, `break`, Operatoren wie `a + b` außerhalb einer
  Formel, eigene Klassen),
- unbekannte Namen, falsche Anzahl von Werten, falscher Typ ohne `kern::to`,
- ein Parameter in einem Lambda nach einem Warten,
- fehlende oder falsche Deklarationen (`n12 is not declared: struct { Float value = 0.0; } n12;`),
- ein Layout, das nicht zu den Knoten des Textes passt.

## Die kanonische Form

`canonical(g)` ist der Blueprint so, wie sein Text ihn ergibt: die Knoten nach Id sortiert, die
Drähte in fester Reihenfolge, jede Einstellung und jeder eingetippte Wert ausgeschrieben, Formeln
in ihrer einen Schreibweise. Es ist derselbe Blueprint (`same_blueprint`) und läuft genau gleich.
Nicht erhalten bleibt nur, was keine Wirkung hat: ein eingetippter Wert unter einem Draht, Felder
einer Transition, die ihre Art nicht benutzt.

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

Für die Ansicht: `logic_notation_apply_keeps_ids_and_places` (der Code ohne Layout, auf seinen
Blueprint angewendet, ändert nichts; eine eingefügte, geänderte oder gelöschte Zeile lässt alle
übrigen Knoten, wo sie sind), `studio_logic_cpp_view` (ein Blueprint, im Editor als C++ aus dem
Nichts geschrieben, gespielt, im Text geändert, Undo) und im Browser `kern/tests/e2e/blueprint_e2e.mjs`
(Schritt 1b: mit Maus und Tastatur).
