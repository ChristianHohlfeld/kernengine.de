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
- **Als Nächstes:** Der bisherige Export (eine Schleife über Knotennummern) entfällt; die letzten
  Randfälle zwischen den Stufen (siehe `kern/games/spawncity/BLUEPRINTS.md`); der native Export
  eines ganzen Spiels aus dem Editor.

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
- **Download** lädt die Quelle, die ein Spiel-Build für diesen Blueprint kompiliert: für einen
  Blueprint, den der Compiler so ausführt, seine eigene Klasse – der Text und dahinter der Kleber
  (`blueprint_<name>.cpp`, dasselbe, was `tools/blueprint_cpp.sh` für ein ganzes Level schreibt);
  sonst noch der bisherige Export.
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
| Set / Add to Variable | `Heat = wert;` / `Heat += wert;` – wird ihr Ergebnis gelesen: `n5.value = (Heat += wert);` |
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

**Ein Event, das sich selbst wieder auslöst.** Die Werte eines Events gehören dem einen Lauf
seines Handlers – wie die Parameter eines Methodenaufrufs. Löst ein Handler sein eigenes Event
aus (Call Event), hat der innere Lauf seine Werte, und wenn er zu Ende ist, hat der äußere wieder
die eigenen: im Rumpf ohnehin (Parameter), und auch für den, der sie von außen liest (`n1.value`).
Graph und Text halten das gleich (`Runtime::fire`, der Kleber).

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
- **Eine Funktion läuft nicht noch einmal, während sie läuft:** Sie ruft sich nicht selbst auf,
  auch nicht über andere Funktionen. (Nach einem Warten ist sie zu Ende; sich dann aufzurufen,
  geht.)
- **Eine Funktion, die als Wert gelesen wird (pure), rechnet nur:** kein Set oder Add, kein
  Warten, kein Do Once, Gate oder Flip Flop, keine Aktion – und was sie als Schritt aufruft,
  rechnet ebenfalls nur. Verzweigen, Schleifen und andere Werte lesen darf sie. So ist es gleich,
  wie oft und in welcher Reihenfolge ein Wert gelesen wird.
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
- Ohne `KERN_LAYOUT`-Block bekommen neue Knoten Ids und Plätze automatisch.

Abgelehnt wird – immer mit Zeile, Spalte und dem Grund, oft mit der richtigen Schreibweise:

- alles außerhalb der Schreibweise (`int x = 3;`, `break`, Operatoren wie `a + b` außerhalb einer
  Formel, eigene Klassen),
- die Knoten, die ihre eigene Form haben, als Aufruf (`var_set<"Heat">(wert);` – es heißt
  `Heat = wert;`),
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

**Zahlen.** `Float` ist `double`, `Int` eine ganze Zahl mit 64 Bit. Aus einer Kommazahl wird eine
ganze Zahl überall gleich (`kern::logic::whole`): zur Null hin abgeschnitten; „keine Zahl“ und
unendlich ergeben 0; was nicht mehr hineinpasst, das nächste Ende. Round und Floor benutzen
dieselbe Regel. Ganze Zahlen sind in allen Stufen bis 2^53 exakt (der Interpreter hält
Zahlen-Variablen als `double`).

**Im Spiel.** `tools/blueprint_cpp.sh level.klevel out.cpp` schreibt für jeden Blueprint des
Levels, der so laufen kann, seinen Text und dahinter den Kleber (`logic::native_glue`): eine
Klasse `Name_Native`, über die die Engine Funktionen und Variablen beim Namen erreicht
(`Instance::call`, `get`, `set`), angemeldet unter dem Hash des Graphen
(`register_native_blueprint`). Ein Spiel, das `out.cpp` einbindet, führt diese Blueprints als
kompilierten Code aus; `Instance::tier()` sagt dann „native C++“. Beim Namen wird nur an dieser
Grenze nachgesehen – in der Klasse ist jeder Aufruf ein direkter Aufruf.

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

Für die native Ausführung: `kern/tests/run.sh` schreibt die Texte von fünf Blueprints genau so,
wie `to_cpp` sie liefert (`kern_tests --emit-native`), kompiliert sie mit dem Compiler des
Systems (die Schalter der Engine: `-Wall -Wextra -Werror`, ASan, UBSan) und vergleicht **jeden
Aufruf Zeichen für Zeichen mit dem Interpreter** – Ergebnisse und alle Variablen
(`blueprint_native_test`, über 2000 Aufrufe). Jeder Aufruf läuft zweimal: direkt auf der Klasse
und beim Namen durch eine `logic::Instance`, die den Graphen als diese Klasse ausführt. Dazu
spielen die Tier-Fälle (`logic_cases.h`), die ein Compiler schon ausführt, als Klassen dasselbe
Szenario wie Interpreter und JIT – Begin Play, Events, Ticks, Wartezeiten, State Machines – mit
demselben Ergebnis: alle 15, auch die Endlosschleife (`infinite_loop`: der Wächter stoppt sie in
jeder Stufe beim selben Durchlauf), ein Handler, der sein eigenes Event auslöst
(`event_reentry`), Tweens und Timer (`waits`), Zustände (`states`) und Zufall (`random`).
Die Blueprints: alle Ablaufformen (`Flow`), Funktionen als Wert und als Schritt (`Calls`), jeder
Draht zwischen verschiedenen Typen (`Casts`), Return in einer Sequence (`Functions`) und **jeder
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
