# 9 Logik-Graphen

Kern hat **keine Skriptsprache**. Spiellogik sind Graphen im Stil von Unreals Blueprints: Ein Graph
ist reines Datum (Knoten, Drähte, Literale, Variablen, State Machines), und jeder Knotentyp ist eine
C++-Funktion in einer Registry. Graphen werden geprüft, in eine Indextabelle übersetzt und von einem
kleinen Interpreter in den festen Simulationsschritten der Engine ausgeführt – nur auf der Autorität
(dem Einzelspiel oder dem Netzwerk-Server). Ihre Wirkungen werden wie alles andere, was die
Simulation tut, repliziert.

Damit Logik nicht langsam ist, übersetzt Kern jeden Graphen beim Start von **Spielen** (Play) oder
**Simulieren** (Simulate) zur Laufzeit nach **WebAssembly** (JIT); der Browser macht daraus Maschinencode. Und ein Graph
**ist** lesbares C++: Jeder Blueprint hat genau einen C++-Text, seine eigene Klasse, und der Build
eines Spiels kompiliert sie hinein ([blueprint_cpp.md](../blueprint_cpp.md)). Alle drei Stufen müssen exakt
dasselbe Ergebnis liefern – warum und wie das gesichert ist, steht in Abschnitt 9.9.

```
include/kern/logic/logic.h     Datenmodell, Library, check, Runtime, Instance, Context, Codegen-API
src/logic/logic.cpp            Werte, Graph-JSON, Library, check, Interpreter
src/logic/core_nodes.cpp       Kern-Knoten (Events, Flow, Variablen, State Machines, Mathe, Text …)
src/logic/codegen.cpp          compile_wasm (JIT)
src/logic/notation.cpp         der Blueprint als C++-Klasse und zurück (to_cpp, from_cpp, native_glue)
include/kern/logic/blueprint.h wogegen diese Klasse kompiliert; nodes.h: was die Knoten rechnen
src/logic/logic_api.cpp        C-ABI zwischen Engine und JIT-Modulen
src/game/logic_node_shapes.cpp Spiel-Knoten, wie die Bibliothek sie kennt: Namen, Pins, Einstellungen
src/game/logic_nodes.cpp       was die Spiel-Knoten tun (Actors, Physik, Kampf, Roboter …), als Funktionen
include/kern/game/blueprint_nodes.h  diese Funktionen (kern::game::nodes) und ihre Aufrufe im C++ eines Blueprints
src/game/logic_world.cpp       Graphen in der GameWorld: Host, Events, Replikation
src/studio/logic_edit.cpp      Bearbeitung im Studio
web/studio/logic.mjs           Blueprint-Editor (Oberfläche)
web/kern.mjs                   JIT-Anbindung im Browser
tests/logic_cases.h            Fälle, die in jeder Stufe gleich laufen müssen
```

---

## 9.1 Das Modell

Aus dem Kopfkommentar von `logic.h` (Unreal-Blueprint-Semantik):

- **Exec-Pins** (weiße Pfeile): was als Nächstes läuft. Event-Knoten beginnen eine Kette,
  Aktionsknoten setzen sie fort, Flow-Knoten verzweigen, schleifen oder warten.
- **Daten-Pins**: typisierte Werte. Eingänge **ziehen**: Pure-Knoten (Mathe, Getter) werden bei
  Bedarf ausgewertet; Ausgänge von Aktionsknoten behalten den Wert, den sie beim letzten Lauf
  hatten. Unverbundene Eingänge nutzen das im Knoten eingetragene Literal.
- **Variablen**: pro Graph-Instanz, typisiert, mit Standardwert.
- **Listen**: Ein Pin oder eine Variable kann eine Liste von Werten eines Typs halten (Unreals
  Arrays). Eine Liste auf einem Draht ist ein Wert wie jeder andere: Wer sie liest, hat die
  Liste, wie sie war. Die Schritte, die eine Liste ändern (Add, Insert, Remove, Clear), ändern eine
  Liste, wo sie steht – eine Variable oder eine Liste darin; For Each geht durch eine Liste, wie
  sie beim Start der Schleife war.
- **Plätze**: eine Variable – oder was von ihr aus über Felder und Listeneinträge erreicht wird,
  beliebig tief: `Held.Damage`, `Scores[i]`, `Cars[i].Wheels[j].Slip`. Jeder Knoten, der eine
  Variable nennt, kann einen Platz in ihr nennen (Einstellung Path, mit einem Index-Pin je
  Listeneintrag): Get, Set und Add und die Schritte, die eine Liste ändern. Geändert wird, wo es
  steht – nichts wird herausgelesen und zurückgeschrieben, in keiner Stufe. Neben einer Liste
  steht ein Eintrag mit den Defaults, der nichts behält: Kein Index führt ins Leere.
- **Aufzählungen** (Enums): Ein Blueprint kann eigene Typen mit benannten Werten deklarieren,
  etwa `Mode` mit den Werten `Closed`, `Opening`, `Open`. Ein Wert einer Aufzählung ist genau
  einer ihrer Werte, gehalten als seine Stelle unter ihnen (die erste ist 0). Er ist weder Zahl
  noch Text: Er wandelt sich in nichts um, und ein Draht verbindet nur Pins derselben Aufzählung.
  Wo eine Zahl oder ein Text gebraucht wird, sagt ein Knoten, was gemeint ist (Enum to Index,
  Enum to Text).
- **Strukturen** (Structs): Ein Blueprint kann eigene Typen mit benannten Feldern deklarieren,
  etwa `Weapon` mit den Feldern `Name`, `Type`, `Damage`. Jedes Feld hat einen Typ und einen
  Standardwert. Ein Wert einer Struktur hält einen Wert für jedes ihrer Felder. Er wandelt sich
  in nichts um, und ein Draht verbindet nur Pins derselben Struktur. Make baut einen Wert aus
  seinen Feldern, Break gibt die Felder; ein Feld einer Variablen ist ein Platz (`Held.Damage`),
  gelesen und gesetzt wie die Variable selbst.
- **State Machines**: Zustände und Übergänge im Stil des Zustandseditors von
  digitalisierungsplanung.de. Ein Übergang feuert auf ein Event, einen Timer, eine Bedingungsvariable
  oder automatisch; Betreten und Verlassen eines Zustands lösen Events aus.
- **Latente Knoten**: Delay, Tween und Retriggerable Delay unterbrechen die Kette und setzen sie im
  Tick fort, wie latente Blueprint-Aktionen.

Ein Level hat einen **Level-Graphen** (Self = keiner); jeder Actor kann einen eigenen Graphen tragen
(sein Self).

### Datenstrukturen

| Struktur | Felder |
|---|---|
| `Graph` | `nodes`, `links`, `variables`, `machines`, `comments`, `next_id` |
| `Node` | `id`, `type` (Registry-Schlüssel, z. B. `flow.branch`), `x`, `y` (Canvas-Pixel), `values` (Literale unverbundener Eingänge, je Pin-ID), `params` (Einstellungen: Variablenname, Eventname, Taste …), `comment` |
| `Link` | `from` (Knoten), `out` (Ausgangs-Pin), `to`, `in` |
| `Variable` | `name`, `type`, `value` (Standard), `help` |
| `StateMachine` | `id`, `name`, `states`, `transitions` |
| `State` | `id`, `name`, `x`, `y`, `initial` |
| `Transition` | `id`, `from` (0 = aus jedem Zustand), `to`, `trigger` (`event`, `timer`, `condition`, `automatic`), `event`, `seconds` (Standard 1), `variable`, `label` |
| `CommentBox` | `id`, `x`, `y`, `w` (400), `h` (200), `text`, `color` (0 = grau) – nur für den Editor, ohne Wirkung |

### Pin-Typen und Werte

`PinType` ist `exec`, `boolean`, `integer`, `number`, `vector`, `text`, `actor` (im JSON `exec`,
`bool`, `int`, `float`, `vector`, `text`, `actor`; beim Lesen auch `number`, `double`, `boolean`,
`integer`, `string`). Ein `Value` trägt einen Typ und den passenden Inhalt (`b`, `i`, `n`, `v`, `s`,
`a`; `ActorRef` ist die Actor-ID).

Nach den sechs Werttypen kommen die sechs **Listentypen**, in derselben Reihenfolge:
`boolean_list` … `actor_list` (im JSON `bool_list`, `int_list`, `float_list`, `vector_list`,
`text_list`, `actor_list`; `is_list`, `element_of` und `list_of` führen vom einen zum anderen).
Ein Listen-`Value` hält seine Einträge in `list` (`Value::of(std::vector<T>)`, `items<T>()`,
`size()`, `item(k)`); beim Weitergeben werden die Einträge geteilt, nicht kopiert, und wer sie
ändert (`edit<T>()`), ändert eine eigene Kopie. Im JSON ist eine Liste das Array ihrer Einträge.
Eine Liste ganzer Zahlen hält jede so, wie jedes `integer` gehalten wird.

Ein siebter Werttyp und ein siebter Listentyp gehören dem, was ein Blueprint selbst deklariert:
seinen **Aufzählungen** (`Graph::enums`; ein `EnumType` hat `name`, `values` und `help`).
`enumeration`, der Wert einer Aufzählung, steht nach `actor`, `enumeration_list` nach `actor_list`
(im JSON `enum` und `enum_list`). Welche Aufzählung gemeint ist, steht als ihr Name neben dem Typ:
`of` in einer `Variable`, einem `FunctionPin` und einem `PinDef`. Das `Value` hält die Stelle des
Werts unter den Werten der Aufzählung, die erste ist 0 (`Value::enumeration(k)`; eine Liste:
`Value::enumerations`) – und dasselbe steht in der Datei:

```json
"enums": [
  {"name": "Mode", "values": ["Closed", "Opening", "Open"], "help": "What the door is doing."}
],
"variables": [
  {"name": "State", "type": "enum", "of": "Mode", "value": 0},
  {"name": "Seen", "type": "enum_list", "of": "Mode", "value": [0, 2]}
]
```

`enums` erscheint nur, wenn der Blueprint welche deklariert. Die Einstellung eines Knotens nennt
die Aufzählung (`"params": {"enum": "Mode"}`); eine Typ-Einstellung nennt sie als
`"type": "Mode"`, eine Liste ihrer Werte als `"Mode_list"`. Von außen – Code, der eine Variable
einer Instanz setzt oder eine ihrer Funktionen aufruft (`Instance::set`, `Instance::call`) – ist,
was kein Wert der Aufzählung ist, ihr erster Wert. Im C++ eines Blueprints ist eine Aufzählung
`KERN_ENUM(Mode, Closed, Opening, Open);` ([blueprint_cpp.md](../blueprint_cpp.md)).

Ein achter Werttyp und ein achter Listentyp gehören den **Strukturen** des Blueprints
(`Graph::structs`; ein `StructType` hat `name`, `fields` und `help`, ein `StructField` hat `name`,
`type` und `value`, seinen Standardwert). `structure`, der Wert einer Struktur, steht nach
`enumeration`, `structure_list` nach `enumeration_list` (im JSON `struct` und `struct_list`).
Welche Struktur gemeint ist, steht als ihr Name neben dem Typ, in `of`, wie bei einer Aufzählung.
Ein Feld ist von einem Wert-Typ, einer Aufzählung, einer Struktur, die über dieser deklariert
ist, oder eine Liste von einem davon; so enthält keine Struktur sich selbst. Eine Struktur hat
mindestens ein Feld und höchstens 96. Der `Value` hält einen Wert je Feld, in der Reihenfolge der
Felder (`Value::structure(fields)`, `field(i)`; eine Liste: `Value::structures`); Kopien teilen
sich die Felder, bis eine von ihnen sich ändert. Die Datei hält einen Wert als Objekt seiner
Felder bei ihren Namen:

```json
"structs": [
  {"name": "Weapon", "fields": [
    {"name": "Name", "type": "text", "value": ""},
    {"name": "Type", "type": "enum", "of": "Kind", "value": 0},
    {"name": "Damage", "type": "float", "value": 10}
  ], "help": "What a weapon does."}
],
"variables": [
  {"name": "Held", "type": "struct", "of": "Weapon", "value": {"Name": "Knife", "Type": 0, "Damage": 25}},
  {"name": "Rack", "type": "struct_list", "of": "Weapon", "value": []}
]
```

`structs` steht nur da, wenn der Blueprint welche deklariert. Ein Feld, das in einem Wert fehlt,
hat seinen Standardwert. Weil das JSON eines Werts sich nach der Deklaration richtet, wird der
Wert einer Struktur mit ihr geschrieben und gelesen: `to_json(graph, of)` und
`Value::from_json(json, type, graph, of)`; `default_of` gibt den Standardwert einer Struktur,
`zero_of` den leeren Wert jedes Typs. Zwei Werte sind gleich, wenn alle ihre Felder gleich sind.
Eine Typ-Einstellung nennt die Struktur als `"type": "Weapon"`, eine Liste als `"Weapon_list"`.
Von außerhalb des Blueprints (`Instance::set`, `Instance::call`) ist, was kein Wert der Struktur
ist – kein Struktur-Wert, einer mit anderen Feldern, einer, in dem ein Feld nicht von seinem Typ
ist –, der Standardwert der Struktur; in einer Liste gilt das Eintrag für Eintrag. Im C++ eines
Blueprints ist eine Struktur `KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };`
([blueprint_cpp.md](../blueprint_cpp.md)).

**Typen des Projekts.** Eine Aufzählung oder Struktur gehört dem Blueprint, der sie erklärt
(`EnumType`, `StructType`). Haben mehrere Blueprints eines Levels denselben Typ, ist er ein Typ des
Projekts (`project = true`, im C++ `KERN_PROJECT_ENUM` / `KERN_PROJECT_STRUCT`): Jeder hat ihn als
eigene, gleiche Erklärung (`logic::same_type`), und der Editor ändert ihn in allen zugleich. Ein
Blueprint bleibt damit für sich lesbar und ausführbar – Interpreter, JIT und Klasse sehen einen
gewöhnlichen Typ –, und ein Wert des Typs bedeutet in jedem dasselbe, wenn ein Blueprint die
Funktion eines anderen ruft.

Eine ganze Zahl (`integer`) ist überall dieselbe: so, wie ein Blueprint sie hält (`logic::normal`).
Unter jeder Zahl liegt ein `double` – in den Variablen des Interpreters, im JIT –, also sind
ganze Zahlen bis 2^53 exakt, eine größere ist die nächste, die ein `double` hält, und keine geht
über die Enden von 64 Bit hinaus. `Value::integer` und der Typ `Int` der C++-Klasse
(`logic::Whole`) sorgen dafür; aus einer Kommazahl wird eine ganze mit `logic::whole` (zur Null
hin; „keine Zahl“ und unendlich: 0; was nicht passt: das nächste Ende). Add to Variable addiert
als Zahl und ist für jeden Wert definiert.

Welche Drähte erlaubt sind, sagt `convertible(from, to)`:

- gleicher Typ immer; Exec nur mit Exec;
- **alles nach `text`** („everything prints“) – außer den Werten einer Aufzählung und einer Struktur, die zu nichts werden;
- Zahlen untereinander (`bool`, `int`, `float`);
- `actor` → `bool` („ist gültig“);
- `float`/`int` → `vector` (in alle drei Komponenten);
- eine **Liste** passt nur in eine Liste ihres eigenen Typs – und nach `text` (`[1, 2.5]`);
  nichts wird zu einer Liste;
- der **Wert einer Aufzählung** wandelt sich in nichts um, und nichts wird zu einem – keine Zahl
  und auch kein `text`; dasselbe gilt für eine Liste davon. Ein Draht verbindet nur Pins
  derselben Aufzählung (`fits(from, to)`: `convertible`, und zwei Aufzählungen sind dieselbe).
- der **Wert einer Struktur** wandelt sich in nichts um, und nichts wird zu einem; dasselbe gilt
  für eine Liste davon. Ein Draht verbindet nur Pins derselben Struktur.

Die Umrechnungen (`as_bool`, `as_int`, `as_number`, `as_vector`, `as_text`, `as_actor`) sind
festgelegt, z. B. Text „0“ oder „false“ ist falsch, ein Vektor als Zahl ist seine Länge, eine Zahl
nach `int` wird abgeschnitten, Text wird als Zahl geparst.

### Das JSON-Format

Graphen stehen im Level (`logic` im Level und in jedem Actor, [Kapitel 8](08-spiel-laufzeit.md)):

```json
{
  "kind": "kern-logic", "version": 1, "nextId": 5,
  "variables": [{"name": "Spawned", "type": "int", "value": 0}],
  "nodes": [
    {"id": 1, "type": "game.overlap_begin", "x": 0, "y": 0},
    {"id": 2, "type": "game.add_score", "x": 320, "y": 0, "values": {"points": 10}},
    {"id": 3, "type": "game.message", "x": 640, "y": 0, "values": {"text": "+10"}},
    {"id": 4, "type": "game.sound", "x": 960, "y": 0, "params": {"sound": "builtin:pickup"}}
  ],
  "links": [[1, "then", 2, "exec"], [2, "then", 3, "exec"], [3, "then", 4, "exec"]],
  "machines": [],
  "comments": [{"id": 9, "x": -64, "y": -112, "w": 1472, "h": 592, "text": "…"}]
}
```

Drähte sind Arrays `[from, out, to, in]`. Übergänge schreiben je nach Trigger `event`, `seconds`
oder `variable`; `comments` erscheinen nur, wenn es welche gibt. Positionen werden auf ganze Pixel
gerundet.

---

## 9.2 Knotendefinitionen und die Library

Jeder Knotentyp ist eine `NodeDef`:

| Feld | Bedeutung |
|---|---|
| `type`, `label`, `category`, `help`, `keywords` | Schlüssel, Titel, Kategorie, Hilfe, Suchwörter |
| `kind` | `event`, `action`, `pure`, `flow` |
| `event` | Events: der `Event::type`, den der Knoten behandelt (z. B. `logic.tick`, `game.overlap_begin`) |
| `latent` | wartet (Delay, Tween) |
| `inputs`, `outputs` | Pins in Zeichenreihenfolge, Exec-Pins eingeschlossen (`PinDef`: `id`, `label`, `type`, `def`, `hidden`) |
| `params` | Einstellungen (`ParamDef`: `key`, `label`, `kind` – `text`, `choice`, `variable`, `event`, `machine`, `state`, `key`, `sound`, `tag`, `integer` –, `choices`, `def`) |
| `color` | Kopffarbe (0 = nach Art) |
| `shape` | Pins, die von Parametern oder dem Graphen abhängen (Variablen, Sequence-Ausgänge, Switch-Fälle) |
| `title` | Titel, der von Parametern abhängt („Every 3 s“, „On Key F“) |
| `accepts` | Events: Behandelt dieser Knoten das Event? Standard: `key` leer, oder gleich dem ersten Parameter, oder `any` |
| `run` | die C++-Funktion |

`run` liefert den **Index des Exec-Ausgangs**, mit dem es weitergeht (unter den Exec-Ausgängen),
oder `kStop` (−1, die Kette endet) bzw. `kSuspend` (−2, latent: wird später aus dem Tick
fortgesetzt). Pure-Knoten schreiben ihre Ausgänge und liefern `kStop`.

Eine Knotenfunktion sieht die Welt über `Context`:

| Methode | Zweck |
|---|---|
| `in(index)`, `in(pin)`, `in_bool`, `in_number`, `in_int`, `in_vector`, `in_text`, `in_actor` | Eingänge (zieht verbundene Pure-Knoten) |
| `out(pin, value)` | Ausgang setzen |
| `param(key, fallback)` | Einstellung |
| `exec(pin)` | Ordinalzahl eines Exec-Ausgangs (für den Rückgabewert) |
| `entry()` | welcher Exec-Eingang den Knoten startete (0 = der erste; `kResume` = 1000 nach einer Wartezeit) |
| `state()`, `slot(k)` | Zustand der Knoteninstanz; `slot(0..3)` liegt im gemeinsamen Speicher mit generiertem Code |
| `wait(seconds, entry)` | latent: nach `seconds` mit `entry` fortsetzen, liefert `kSuspend` |
| `cancel_waits()` | eigene Wartezeiten verwerfen (Retriggerable Delay) |
| `then(out)` | nach Ende der aktuellen Kette an diesem Ausgang weitermachen (Sequence) |
| `loop_back(entry)` | nach Ende der aktuellen Kette diesen Knoten mit `entry` erneut betreten (Schleifen) |
| `self()`, `host()`, `runtime()`, `instance()` | Kontext |

`Library::core()` enthält die eingebauten Knoten; `logic::library()` ist core plus das, was das Spiel
registriert hat. Die Spiel-Knoten registrieren sich beim Start des Programms selbst
(`game::logic_nodes_registered()` sorgt dafür, dass sie gelinkt werden).

Ein Spiel-Knoten steht an zwei Stellen. **Wie er aussieht** – Name, Pins, Einstellungen – steht
in `logic_node_shapes.cpp`; diese Datei braucht nichts vom Spiel, deshalb kennen auch Werkzeuge
die Knoten, die nur Blueprints lesen und schreiben (`tools/blueprint_cpp`):

```cpp
lib.add(step("game.add_score", "Add Score", "Game", {data("points", "Points", PinType::number, 10.0)}, {},
             "Adds points (wins at the target score when the goal is Score).", "points reward"));
```

**Was er tut**, ist eine typisierte Funktion in `kern::game::nodes` (deklariert in
`kern/game/blueprint_nodes.h`, definiert in `logic_nodes.cpp`): das Spiel, Self, dann die
Dateneingänge des Knotens in ihrer Reihenfolge. `bind` hängt sie an den Knoten:

```cpp
void game_add_score(GameWorld& w, Self, Float points) { w.add_score(static_cast<f32>(points)); }
...
bind(lib, "game.add_score", &nodes::game_add_score);
```

Der Interpreter ruft sie über den Knoten auf, das C++ eines Blueprints direkt
(`game_add_score(5.0);`, [blueprint_cpp.md](../blueprint_cpp.md)) – beide dieselbe Funktion. Ein
Knoten mit mehreren Ausgängen gibt ein `struct`, dessen Felder wie die Pins heißen; ein
Actor-Eingang ohne Wert meint in der Funktion Self. Ein neuer Spiel-Knoten braucht also: seine
Zeile in `logic_node_shapes.cpp`, seine Funktion samt `bind`, seinen Aufruf in
`BlueprintNodesOf` (`blueprint_nodes.h`) und einen Platz in einem Blueprint der Test-Levels
(`tests/game_cases.h`) – der Test `game_blueprints_as_classes` meldet, was davon fehlt.

---

## 9.3 Prüfen

`check(graph, lib)` liefert `Issue`s (`node`, `error`, `text`). Fehler:

- unbekannter Knotentyp;
- Parameter, die auf fehlende Variablen, State Machines oder Zustände zeigen; ein Event ohne Namen;
- ein Draht zu einem Pin, den es nicht gibt;
- ein Exec-Draht an einem Daten-Pin, nicht umwandelbare Typen;
- ein Dateneingang mit mehr als einem Draht;
- Pure-Knoten, die von sich selbst abhängen (Zyklus);
- eine State Machine ohne genau einen Anfangszustand; ein Event-Übergang ohne Eventnamen; ein
  Bedingungsübergang auf eine fehlende Variable.

Warnung: ein Knoten mit Exec-Eingang, an dem nichts hängt („never runs“).

Ein Graph mit Fehlern wird beim Start **abgeschaltet** und gemeldet; der Rest des Spiels läuft
weiter. Im Studio zeigt Compile (F7) die Ergebnisse.

---

## 9.4 Ausführung

### Runtime und Instanzen

Eine `Runtime` führt alle Graphen einer Welt aus; jeder Graph läuft als `Instance` für einen
Besitzer (Actor oder Level).

| Methode | Wirkung |
|---|---|
| `add(graph, self, name)` | übersetzt und fügt hinzu; Prüffehler schalten die Instanz ab; danach nativer C++-Code (falls registriert) oder JIT-Auftrag |
| `remove(self)`, `clear()` | entfernt (samt wartender Latenter) |
| `begin()` | Begin Play für Instanzen, die noch nicht begonnen haben: Anfangszustände betreten, `event.begin` ausführen |
| `tick(dt)` | ein fester Schritt: fällige latente Aktionen (nach Zeit sortiert), dann das Tick-Event (`logic.tick` mit `delta`), dann Timer- und Bedingungsübergänge der State Machines |
| `fire(event)` | löst ein Event synchron aus |
| `call(name, target, args)` | Custom Event auf allen Instanzen (oder der des Ziels) und Event-Übergänge mit diesem Namen |
| `find(self)`, `instances()` | Zugriff |
| `trace()`, `tracing` | welche Knoten liefen (für den Editor) |
| `budget` | 200 000 Schleifendurchläufe pro Schritt (der Schleifenwächter) |
| `jit` | JIT einschalten (Standard: an, wo der Host kann) |

Eine `Instance` bietet `get`/`set` von Variablen, `state`/`set_state` von State Machines,
`disabled()`, `compiled()` und `tier()` – `"interpreter"`, `"compiling"`,
`"compiled (WebAssembly)"` oder `"native C++"`.

### Events

```cpp
struct Event {
  std::string type;          // z. B. "game.overlap_begin"
  ActorRef target = 0;       // nur Instanzen mit diesem Self (0 = alle)
  bool level = false;        // mit target: der Level-Graph bekommt es auch
  std::string key;           // eventspezifisch: Custom-Event-Name, Tastencode, Aktion, "Machine/State"
  std::vector<std::pair<std::string, Value>> args;
};
```

`fire` geht über alle nicht abgeschalteten Instanzen (die zum Ziel passen), sucht Event-Knoten mit
passendem `event` und `accepts`, schreibt die Argumente in die gleichnamigen Ausgänge und führt die
Kette aus. Instanzen, die währenddessen dazukommen (gespawnte Actors), werden mitgenommen.

### Eine Kette laufen lassen

Der Interpreter (`Runtime::run_from`) arbeitet mit einem Stapel aus (Knoten, Eingang):

1. Knoten ausführen; was er per `then` oder `loop_back` vormerkt, kommt auf den Stapel – in umgekehrter
   Reihenfolge, sodass das zuerst Vorgemerkte zuerst läuft.
2. Liefert er einen gültigen Exec-Ausgang, geht es am verbundenen Knoten weiter; sonst endet dieser
   Zweig, und der nächste Stapeleintrag läuft.

So arbeiten Sequence (Ausgang 0 sofort, die übrigen vorgemerkt) und Schleifen (Körper ausführen,
danach den Knoten selbst per `loop_back` mit `kResume` erneut betreten).

Daten werden **gezogen**: Ein Eingang, der mit einem Pure-Knoten verbunden ist, führt dessen `run`
bei jedem Lesen aus – so tief, wie die Werte einander lesen (ein Wert, der sich selbst liest, ist
ein Compile-Fehler). Ein Eingang an einem Aktionsknoten liest dessen zuletzt gespeicherten Ausgang.

### Schutz vor Endlosschleifen

- Der Schleifenwächter: mehr als `budget` (200 000) Schleifendurchläufe in einem Schritt.
  Schleifen sind das Einzige in einem Graphen ohne festes Ende – ein Weg, der ohne Warten in sich
  selbst zurückführt, eine Funktion, die sich selbst aufruft, und ein Tween, der aus seinem
  eigenen Update neu gestartet wird, sind Compile-Fehler. Über der Grenze enden die Schleifen der
  Kette sofort, der Rest der Kette läuft zu Ende, und dann wird die Instanz abgeschaltet
  („infinite loop? The Blueprint is stopped.“). Jede Stufe zählt dieselben Durchläufe
  (`Runtime::pass`) und hört beim selben auf – auch ein Blueprint, der als seine C++-Klasse
  kompiliert ist und eine Kette nicht mittendrin abbrechen kann.
- Events, die Events auslösen, dürfen höchstens 64 tief verschachteln; darüber wird die Kette
  gestoppt – in jeder Stufe beim selben Event. Funktionsaufrufe zählen dabei nicht mit (in der
  C++-Klasse sind sie gewöhnliche Aufrufe, und sich selbst aufrufen kann eine Funktion nicht).
  Löst eine Funktion ein Event aus, dessen Handler sie noch einmal aufruft, ist das ein Aufruf
  wie jeder andere; ist der Handler zu Ende, hat der unterbrochene Aufruf seine Parameter wieder.
- Der Stapel des JIT für offene Sequences und Schleifen ist so groß, wie der Graph ihn je brauchen
  kann (`Instance::stack_capacity`: 66 × die Exec-Ausgänge seiner Flow- und Schleifen-Knoten,
  mindestens 512). Er läuft deshalb nie voll, wo Interpreter und C++-Klasse weiterlaufen; ein
  Graph, der mehr als 32 768 Einträge bräuchte, wird nicht JIT-kompiliert.

### State Machines

- `begin` betritt den Anfangszustand.
- Betreten und Verlassen feuern `logic.state_enter` / `logic.state_exit` mit dem Schlüssel
  `"Maschine/Zustand"`; „On State Entered“ bekommt den vorigen Zustand.
- Übergänge prüft `check_transitions` – **höchstens ein Übergang pro Maschine und Aufruf**
  (automatische Ketten gehen einen Zustand pro Schritt weiter):
  - `event`: bei `call` mit diesem Namen,
  - `timer`: nach `seconds` im Zustand,
  - `condition`: solange die boolesche Variable wahr ist,
  - `automatic`: sofort beim nächsten Tick.
- `from = 0` heißt „aus jedem Zustand“ (aber nicht in den Zielzustand selbst zurück).
- `Set State` (`state.set`) wechselt direkt.

---

## 9.5 Kern-Knoten

`src/logic/core_nodes.cpp` registriert 105 Knoten:

| Kategorie | Knoten |
|---|---|
| **Events** | `event.begin` Begin Play · `event.tick` Event Tick (Delta Seconds) · `event.every` Every N Seconds (Intervall-Parameter, zählt eigene Zeit, Ausgang Count) · `event.custom` Custom Event (Name; Ausgänge Actor, Value, Text; schaltet auch gleichnamige State-Machine-Übergänge) |
| **State Machines** | `event.state_enter` On State Entered · `event.state_exit` On State Left · `state.set` Set State · `state.get` Current State · `state.is` Is In State |
| **Flow** | `flow.reroute` / `value.reroute` (Knoten für Drähte) · `flow.branch` Branch · `flow.sequence` Sequence (1..16 Ausgänge) · `flow.for` For Loop (First, Last inklusive; Index) · `flow.for_each` For Each (eine Liste; Item, Index; die Liste, wie sie beim Start der Schleife ist) · `flow.while` While Loop · `flow.do_once` Do Once (Reset, Start Closed) · `flow.do_n` Do N (N, Reset, Counter) · `flow.gate` Gate (Enter, Open, Close, Toggle, Start Closed) · `flow.flip_flop` Flip Flop (A/B, Is A) · `flow.delay` Delay (Aufrufe während des Wartens werden ignoriert) · `flow.retrigger_delay` Retriggerable Delay · `flow.tween` Tween (Play, Stop, Duration; Update jeden Schritt mit Alpha 0→1, Easing linear/smooth/in/out, dann Finished) · `flow.switch_int` Switch on Int · `flow.switch_text` Switch on Text (Fälle durch Kommas getrennt) · `flow.switch_enum` Switch on Enum (Einstellung Enum; ein Exec-Ausgang je Wert der Aufzählung, beschriftet mit dem Namen des Werts) · `flow.call` Call Event (Target, Actor, Value, Text) |
| **Variablen** | `var.get` · `var.set` · `var.add` (Amount, −1 zählt herunter) – jeder auf einer Variablen oder, mit der Einstellung Path, auf einem Platz in ihr: einem Feld (`.Damage`), einem Listeneintrag (`[]`, mit einem Index-Pin), beliebig tief (`[].Wheels[].Slip`); Value: was dort jetzt steht |
| **Listen** | Werte, generisch über den Typ der Einträge (Einstellung Type): `list.make` Make List (0..16 Einträge) · `list.length` · `list.get` Get Item (jenseits der Liste: der Nullwert des Typs) · `list.contains` · `list.find_row` Find Row (eine Liste einer Struktur als Tabelle: der erste Eintrag, dessen Feld der Wert ist; Found, Row, Index) · `list.find` Find Item (Index oder −1) – Schritte auf einer Liste, wo sie steht (Einstellungen List und Path: eine Listen-Variable oder eine Liste an einem Platz in einer Variablen): `list.add` Add to List (gibt den Index) · `list.insert` (vor dem Index; hinter dem Ende: ans Ende) · `list.remove_at` Remove Index · `list.remove` Remove Item (jeden gleichen Eintrag; Removed) · `list.clear` |
| **Aufzählungen** | auf den Werten einer Aufzählung des Blueprints (Einstellung Enum; im Editor trägt der Knoten ihren Namen, „Mode to Text“): `enum.equal` Equal (Enum) (A, B; Result) · `enum.name` Enum to Text (der Name des Werts) · `enum.from_name` Text to Enum (der Wert mit diesem Namen; Found – heißt kein Wert so: der erste Wert, und Found ist falsch) · `enum.index` Enum to Index (die Stelle des Werts unter den Werten der Aufzählung, die erste ist 0) · `enum.from_index` Index to Enum (der Wert an dieser Stelle; Valid – gibt es die Stelle nicht: der erste Wert, und Valid ist falsch) · `enum.values` All Values (eine Liste aller Werte in ihrer Reihenfolge, für ein For Each) – die Knoten für jeden Typ arbeiten auch im Typ einer Aufzählung: Get und Set einer Variablen, Select, Reroute, die Listen-Knoten und For Each |
| **Strukturen** | auf einer Struktur des Blueprints (Einstellung Struct; im Editor trägt der Knoten ihren Namen, „Make Weapon“): `struct.make` Make Struct (ein Eingang je Feld – ohne Draht: der Standardwert des Felds; Value) · `struct.break` Break Struct (Value; ein Ausgang je Feld) – ein Feld einer Variablen wird gesetzt, wo es steht: `var.set` mit dem Path dorthin (im Editor „Set Held.Damage“; Dokumente mit den früheren Knoten Set Field und Set List Item werden als dieses Set gelesen) |
| **Funktionen** | `func.entry` Function Entry · `func.return` Return · `func.call` Call Function · `func.pure` Function Value (eine als Pure markierte Funktion, als Wert) · `func.call_on` Call on Actor (eine Funktion auf dem Blueprint eines anderen Actors oder – ohne Target – des Levels; der Blueprint erklärt, wie sie aussieht) |
| **Mathe** | `math.add`, `sub`, `mul`, `div`, `mod`, `min`, `max`, `clamp`, `abs`, `negate`, `lerp`, `sin`/`cos`/`atan2` (Grad), `sqrt`, `pow`, `round`, `floor`, `random` (Float im Bereich), `random_int`, `random_bool` · `math.formula` Formula (jeder Name darin ist ein Eingang) |
| **Logik** | `math.compare` Compare (`==`, `!=`, `<`, `<=`, `>`, `>=`; Gleichheit mit 10⁻⁹ Toleranz) · `logic.and`, `or`, `xor`, `not` · `logic.select` Select |
| **Werte** | `value.float`, `value.int`, `value.bool`, `value.text` |
| **Vektor** | `vector.make`, `break`, `add`, `sub`, `cross`, `scale`, `length`, `normalize`, `distance`, `dot`, `lerp` |
| **Text** | `text.append`, `equal`, `contains`, `length` |
| **Utilities** | `time.now` Game Time · `debug.print` Print (Text, On Screen) |

Zufallszahlen kommen vom Host (`Host::random()`) und sind **deterministisch pro Welt** – Replays und
das Netz stimmen überein.

---

## 9.6 Spiel-Knoten

Die Bibliothek hat 73 Spiel-Knoten (`logic_node_shapes.cpp`, `logic_nodes.cpp`). Sie rufen Funktionen der GameWorld
([Kapitel 8](08-spiel-laufzeit.md)); ein Actor-Eingang ohne Draht (0) bedeutet **Self**.

### Events

| Knoten | Event | Ausgänge | Ausgelöst von |
|---|---|---|---|
| On Begin Overlap | `game.overlap_begin` | Actor, Other Actor | etwas betritt den Sensor-Collider dieses Actors |
| On End Overlap | `game.overlap_end` | Actor, Other Actor | etwas verlässt ihn |
| On Entered Sensor | `game.overlapped_begin` | Actor, Sensor Actor | dieser Actor (z. B. ein Spieler) betritt einen fremden Sensor |
| On Hit | `game.hit` | Actor, Other, Impact Speed, Point, Normal | Box3D-Hit ab 2 m/s, beide Seiten, höchstens 32 pro Schritt |
| On Collected | `game.pickup` | Actor, Collector, Points | ein Pickup wurde eingesammelt |
| On Trigger | `game.trigger` | Actor, Player | ein Spieler berührte diesen Trigger |
| On Damaged | `game.damaged` | Actor, Attacker, Damage, Headshot | Schaden (braucht Health) |
| On Death | `game.death` | Actor, Killer, Headshot | Gesundheit 0 |
| On Respawn | `game.respawn` | Actor | Rückkehr nach dem Tod |
| On Interact | `game.interact` | Actor, Player, Point | Interact (F / Y am Gamepad) mit Blick auf den Actor (Strahl 3 m) oder daneben (nächster Actor mit Graph innerhalb 2 m) |
| On Player Joined | `game.pawn_added` | Player | ein Spieler kam dazu |
| On Game Over | `game.over` | Won, Message | Spiel gewonnen oder verloren (an alle Graphen) |
| On Input Action | `game.input` | Player, Pressed | Aktion `jump`, `fire`, `aim`, `reload`, `crouch`, `interact`, `boost`; Flanke pressed/released/both – für jeden Spieler und jedes Gerät |
| On Key | `game.key` | Pressed, Player | eine Taste dieser Tastatur (nur der lokale Spieler; Parameter „F“ → `KeyF`, „1“ → `Digit1`) |

Überlappungen zählen den Besucher als ganzen Actor: Eine Überlappung beginnt mit der ersten Form im
Sensor und endet mit der letzten, die ihn verlässt (Kapsel und Kopf eines Spielers sind ein
Besucher).

### Actors

`actor.self` Self · `actor.player` Get Player (0 = Spieler dieser Maschine, 1, 2 … weitere) ·
`actor.player_count` · `actor.find` Find Actor by Name · `actor.find_tag` Find Actor with Tag ·
`actor.all_with_tag` All Actors with Tag (eine Actor-Liste) ·
`actor.random_tag` · `actor.has_tag` · `actor.name` · `actor.valid` · `actor.location` ·
`actor.rotation` · `actor.forward` · `actor.velocity` · `actor.distance` · `actor.set_location` ·
`actor.set_rotation` (Grad) · `actor.move_by` · `actor.spawn` Spawn Actor (Kopie einer Vorlage mit
Kindern und Logik, an alle Spieler repliziert) · `actor.destroy` · `actor.enable` Set Enabled ·
`actor.set_property` Set Property (jeder Komponentenwert über seinen Pfad wie in der Leveldatei,
z. B. `components.mesh.material.color` – repliziert)

### Physik

`actor.set_velocity` · `actor.impulse` Add Impulse · `actor.force` Add Force (für einen Schritt) ·
`actor.launch` Launch (behält die horizontale Geschwindigkeit, addiert diese) · `actor.simulate` Set
Simulate Physics (dynamisch ↔ kinematisch) · `actor.joint_motor` Set Joint Motor (Scharnier, Slider,
Seil: Türen, Lifte, Winden) · `actor.explode` Explosion (Box3D-Radialimpuls mit Blitz, Sound und
fallendem Schaden) · `physics.trace` Line Trace (Box3D-Strahl) · `physics.trace_channels` Line Trace by Channels (derselbe Strahl mit Box3Ds Abfragefilter: welche Kollisionskanäle er trifft, und seine eigenen) · `physics.channels` Channels (die Bits der Kollisionskanäle des Levels mit diesen Namen)

### Spiel, Kampf, Effekte, Audio

`game.score` Get Score · `game.message` Show Message (alle Spieler sehen sie) · `game.add_score` ·
`game.win` · `game.lose` · `combat.damage` Apply Damage · `combat.heal` · `combat.kill` ·
`combat.ammo` Give Ammo · `actor.health` Get Health · `actor.set_light` · `actor.set_color` ·
`game.particles` Spawn Sparks · `game.shake` Camera Shake (nur diese Maschine) · `game.sound` Play
Sound (3D am Ort bzw. an Self, oder flach) · `flow.for_each_tag` For Each Actor with Tag

### Roboter

`walker.move` Walk (wie das Gamepad: vorwärts bis 0,4 m/s, seitwärts 0,3, drehen 57 °/s – die
trainierten Bereiche; übersteuert das eigene Verhalten) · `walker.look` Look (Kopf: Gieren bis 80°,
Nicken bis 63°) · `walker.autonomous` Set Autonomous · `walker.skill` Robot Move (`sit`, `pick`,
`kick_left`, `kick_right`, `roulade`, `jump`) · `walker.beak` · `walker.pet` · `walker.hear` ·
`walker.holding` · `walker.quack` Chirp · `walker.tof` Robot ToF · `walker.sees` Robot Camera Sees ·
`walker.state` Robot State. Einzelheiten in [Kapitel 13](13-micro-duck.md).

---

## 9.7 Logik in der GameWorld

`GameWorld::start_logic` (`src/game/logic_world.cpp`) legt die Runtime nur an, wenn

- der Modus `play` oder `simulate` ist und die Welt **keine Replika** ist (Logik läuft nur auf der
  Autorität), und
- das Level oder ein Actor einen nicht leeren Graphen hat.

Dann wird der Level-Graph (Self 0, Name „Level“) und jeder Actor-Graph (Self = Actor-ID, Name =
Actorname) hinzugefügt; `start` ruft anschließend `begin`. Gespawnte Actors und neue Pawns bringen
ihre Graphen mit.

Der **Logik-Host** der Welt (`GameWorld::LogicHost`):

| Methode | Umsetzung |
|---|---|
| `time()` | `sim_time_` – die Simulationszeit in Sekunden, aus dem Schrittzähler berechnet, nicht die Wanduhr |
| `print(text, screen)` | „Logic: …“ in die Meldungen, mit `screen` auch als HUD-Meldung |
| `error(text)` | „Logic error: …“ in die Meldungen (Studio zeigt sie im **Ausgabeprotokoll** (Output Log)) |
| `random()` | Xorshift32, Startwert aus der Zahl der Actors – deterministisch |
| `game()` | die GameWorld (für die Spiel-Knoten) |

**Ereignisse an Graphen** (`logic_fire`): Das Event geht an den Graphen des betroffenen Actors oder,
falls der keinen hat, an den seines nächsten Vorfahren mit Graph (der eingeschweißte Collider einer
Tür gehört zur Tür), und immer auch an den Level-Graphen. Events ohne betroffenen Actor
(Spielende) gehen an alle. Der Logik-Tick läuft am Ende jedes festen Schritts nach den
Physik-Events ([Kapitel 8](08-spiel-laufzeit.md), Abschnitt 8.9).

Hat ein Level Logik, schaltet die GameWorld **Sensor-Events an allen beweglichen und Hit-Events an
allen dynamischen Formen** ein – sonst gäbe es keine Überlappungen und Treffer für die Graphen.

**Replikation.** Was Logik an der Welt ändert, wird mitgeschrieben: `set_property` (Pfad und Wert),
`spawn_copy` (Vorlage, ID, Pose) und das Versetzen statischer Körper (`teleport` schreibt
`transform.position`/`rotation`). Der Server schickt diese Änderungen an alle Clients, auch an
Spieler, die später dazukommen ([Kapitel 10](10-netzwerk.md)).

---

## 9.8 Codegen: vom Graphen zu Maschinencode

Ein Graph wird auf zwei Wegen zu Maschinencode:

- **Im Build eines Spiels: seine eigene C++-Klasse.** Jeder Blueprint hat genau einen C++-Text
  (`logic::to_cpp`), in dem jeder Knoten ein Funktionsaufruf ist; `tools/blueprint_cpp.sh` schreibt
  die Klassen aller Blueprints eines Levels in eine Quelle des Spiels, und `Runtime::add` führt
  einen Graphen, dessen Klasse im Programm steckt (gefunden über `graph_hash`), als diese Klasse
  aus – Stufe „native C++“. Das ist der schnellste Weg: Der Compiler sieht alles. Wie das geht,
  steht in [blueprint_cpp.md](../blueprint_cpp.md).
- **Zur Laufzeit: der JIT.** `compile_wasm(instance)` (`src/logic/codegen.cpp`) schreibt ein
  WebAssembly-Modul. Der Host übersetzt es mit dem optimierenden Compiler des Browsers und hängt
  es in die Funktionstabelle der Engine. Der Graph läuft dann als nativer Code und ruft die
  C++-Knotenfunktionen der Engine direkt auf (wasm-zu-wasm-Aufrufe, keine Skript-Engine
  dazwischen). Das ist der Weg für einen Graphen, der gerade bearbeitet und sofort gespielt wird.

Der Rest dieses Abschnitts beschreibt den JIT.

### Was inline erzeugt wird

Kontrollfluss (Branch, Sequence, For, While, Do Once, Do N, Gate, Flip Flop), numerische Variablen
(Get, Set, Add), Mathe (`add`, `sub`, `mul`, `div`, `mod`, `min`, `max`, `clamp`, `abs`, `negate`,
`lerp`, `sqrt`, `floor`, `round`), Vergleiche, boolesche Logik, Zahl-/Integer-/Bool-Literale und ein
numerisches `Select`. **Alles andere** – Spielknoten, Text, Vektoren, latente Knoten – ist ein Aufruf
der C++-Funktion des Knotens (`call_node`), danach geht es am gewählten Exec-Ausgang weiter
(aus einer Sprungtabelle).

### Gemeinsamer Speicher

Der gemeinsame Zustand liegt in der Instanz, an festen Adressen, die sich nach dem Anlegen nie
ändern (`CodeLayout`):

| Bereich | Inhalt |
|---|---|
| `slots()` | f64-Slots: zuerst die numerischen Variablen (bool, int, float), dann **8 pro Knoten** – 4 für Zustand (Schleifenindex, Gate offen, Flip-Flop …) und 4 für Datenausgänge inline erzeugter Exec-Knoten |
| `stack()` | i32-Fortsetzungsstapel: `[0]` = Tiefe, dann Paare (pc, entry); `stack_capacity()` Einträge (so viele, wie der Graph je brauchen kann) |
| `exec_table()` | je Knoten und Exec-Ausgang: (Ziel, Eingang) |
| `counters()` | `[0]` gelaufene Knoten (eine Zählung), `[1]` Grenze der Schleifendurchläufe pro Schritt (das Budget), `[2]` Tracing an, `[3]` Schleifendurchläufe in diesem Schritt, `[4]` einer Schleife der laufenden Kette wurde ein Durchlauf verweigert |

Nicht numerische Variablen (Vektor, Text, Actor) liegen in einer Map der Instanz und werden über
Knotenfunktionen gelesen.

### Das Programm

Das erzeugte Programm ist eine **Schleife über einen Programmzähler** – ein Fall je Knoten. So bleibt
jede Exec-Verdrahtung gültig, auch Zusammenführungen und Zyklen. Fortsetzungen von Sequence und
Schleifen gehen auf den Stapel der Instanz, genau wie im Interpreter. Jeder Durchlauf zählt
`counters[0]` hoch. Ein Schleifen-Knoten zählt seinen Durchlauf in `counters[3]`; an der Grenze
nimmt er stattdessen Completed und setzt `counters[4]` (der Schleifenwächter). Bei vollem Stapel
liefert die Funktion 2.

Das WebAssembly-Modul:

- **Typen:** `run(pc, entry) → i32`, `call_node(id, node, entry) → i32`,
  `read_pin(id, node, pin) → f64`, `trace(id, node)`.
- **Importe** aus `env`: `call_node`, `read_pin`, `trace` und der **Speicher der Engine** – Slots,
  Stapel und Tabelle werden über ihre absoluten Adressen angesprochen.
- **Export:** `run`.
- **Dispatch:** ein `br_table` über den Programmzähler auf einen Block je Knoten.

`graph_hash` ist ein FNV-1a über das Graph-JSON **ohne Positionen und Kommentare** – einen Knoten zu
verschieben ändert ihn nicht, jede inhaltliche Änderung schon. Unter diesem Hash meldet sich die
C++-Klasse eines Blueprints an; ein seit dem Build geänderter Blueprint läuft deshalb nie als
veralteter Code.

### Der Ablauf im Browser

1. `Runtime::add` prüft den Graphen. Steckt für seinen Hash eine C++-Klasse im Programm, gewinnt die.
2. Sonst ruft er `host.jit_compile(compile_wasm(instance), id)`. Die Standard-Implementierung von
   `Host::jit_compile`, die auch der Host der GameWorld nutzt, leitet das an `platform_jit_compile` weiter, das in WebAssembly die Host-Funktion `kern_env.logic_jit` aufruft
   (`src/logic/logic_api.cpp`). Nativ gibt es keinen Compiler: Der Graph bleibt im Interpreter
   (oder läuft als seine C++-Klasse).
3. `web/kern.mjs` übersetzt das Modul asynchron (`WebAssembly.compile`, `instantiate` mit den
   Engine-Exporten `kern_logic_call_node`, `kern_logic_read_pin`, `kern_logic_trace` und dem
   Engine-Speicher), legt die `run`-Funktion in einen freien Platz der Funktionstabelle der Engine
   und meldet `kern_logic_jit_ready(id, slot)`. Schlägt etwas fehl, wird gewarnt und mit −1
   gemeldet – der Graph läuft weiter im Interpreter.
4. Bis dahin ist die Instanz `"compiling"` und läuft im **Interpreter**. Ab der Meldung ruft
   `run_from` die Tabellenfunktion direkt auf: ein indirekter Aufruf, kein JavaScript dazwischen.
5. Beim Zerstören der Instanz gibt `logic_jit_release` den Tabellenplatz frei.

Laut `docs/studio.md` ist der JIT etwa 55-mal schneller als der Interpreter; die Statuszeile zeigt
„compiled“. Den Wert misst `tests/logic_wasm_test.mjs` mit einer Million Schleifendurchläufen.

---

## 9.9 Warum der JIT dem Interpreter gleichen muss

Interpreter und JIT sind **austauschbar**, und zwar mitten im Spiel (die C++-Klasse läuft von
Anfang an – sie steckt im Programm):

- Der JIT arbeitet asynchron. Ein Graph beginnt im Interpreter – Begin Play, die ersten Ticks, Do
  Once, laufende Schleifenzähler – und wird übernommen, sobald das Modul bereit ist. Deshalb liegt
  der Zustand (Variablen, Schleifenzähler, Gates, Flip-Flops, Datenausgänge inline erzeugter
  Knoten) nicht in Strukturen des Interpreters, sondern in **denselben Slots**, die auch der
  generierte Code liest und schreibt. Die Knotenfunktionen des Interpreters für diese Knoten
  rechnen ebenfalls über `Context::slot` (etwa `flow.for`: „State in slots (index, last): the same
  in the interpreter and in generated code“). Ein Stufenwechsel braucht so keine Umkopie
  (`enter_compiled` ist leer).
- Latente Knoten werden im Interpreter-Teil der Runtime verwaltet und setzen ihre Kette – egal in
  welcher Stufe – mit derselben Semantik fort.
- Kern ist deterministisch ([Kapitel 6](06-physik.md), Abschnitt 6.14). Ein Graph, der im Browser
  kompiliert anders rechnete als nativ im Interpreter, würde Replays, das Training und
  Fehlersuche entwerten. Auch der Wächter gegen Endlosschleifen zählt in jeder Stufe gleich
  (Schleifendurchläufe pro Schritt) und stoppt dieselben Graphen beim selben Durchlauf.
- Der Editor debuggt mit derselben Semantik: Breakpoints und die Live-Anzeige laufender Knoten
  funktionieren auch im kompilierten Code (`trace`).

Geprüft wird das in allen drei Stufen mit denselben Fällen aus `tests/logic_cases.h`
(`math_loops`, `gates_and_sequences`, `while_nested`, `mixed_cpp_nodes`, `state_machine`,
`infinite_loop`, `reroutes`, `functions`, `formulas`, `events_and_time`, `event_reentry`, `waits`,
`states`, `random`):

| Test | Stufe | Prüft |
|---|---|---|
| `logic_tier_cases_interpreter` (`tests/test_main.cpp`) | Interpreter | erwartete Werte jedes Falls (z. B. `Sum == 315`, `Log == 111111`, Endlosschleife schaltet ab); das WebAssembly-Modul ist wohlgeformt (Magic, Version, Sektionen) |
| `tests/logic_wasm_test.mjs` (mit `logic_wasm_test.cpp`) | JIT im Engine-WebAssembly unter Node | jeder Fall einmal im Interpreter, einmal JIT-kompiliert (von V8 übersetzt und in die Funktionstabelle gelinkt wie im Browser); die Stufe muss `compiled (WebAssembly)` sein und das Ergebnis **identisch**; dazu die Geschwindigkeitsmessung |
| `blueprint_native_test` (`tests/blueprint_native.h`, von `run.sh` gebaut) | die C++-Klasse | `kern_tests --emit-native` schreibt die C++-Texte der Fälle und die Interpreter-Ergebnisse; `run.sh` kompiliert die Texte; jeder Graph muss als `native C++` laufen und genau die Interpreter-Ergebnisse liefern – dazu über 2000 einzelne Aufrufe mit jedem Wert-Knoten |
| `game_native_test` (`tests/game_cases.h`, `tests/game_native_test.cpp`) | die C++-Klassen in einem Spiel | zwei Levels, deren Blueprints jeden Spiel-Knoten benutzen, von `tools/blueprint_cpp` als Klassen geschrieben; jedes Szenario im Interpreter und als Klassen gespielt, alles Sichtbare Zeile für Zeile gleich |

Was ein Knoten rechnet, steht deshalb nur **einmal**: als typisierte Funktion (`kern/logic/nodes.h`,
`kern/game/blueprint_nodes.h`), die der Interpreter über den Knoten und die C++-Klasse direkt
aufruft. Wer einen Knoten zusätzlich im JIT inline erzeugt, hält `WasmGen::node_code` damit gleich
und ergänzt bei Bedarf einen Fall in `logic_cases.h`.

---

## 9.10 Bearbeitung im Studio

Der Blueprint-Editor (`web/studio/logic.mjs`) folgt Unreals Blueprint-Editor; seine Oberfläche
beschreibt [Kapitel 11](11-studio.md). Er ist auch in der deutschen Oberfläche englisch beschriftet
(Knoten, Kategorien, Compile; `#logicEditor` steht in `I18N_SKIP` in `web/studio/game.mjs`). Die
Seite enthält keine Editorlogik: Jede Bearbeitung ist ein
Befehl an die Engine (`src/studio/logic_edit.cpp`), **jede Bearbeitung ist eine Level-Bearbeitung
und damit ein Undo-Schritt**; die Seite zeichnet, was `logic_json()` beschreibt (Graph, Knoten mit
Pins, Probleme, beim Spielen die gelaufenen Knoten, Live-Werte der Variablen, Zustände, Stufe).

| Befehl | Wirkung |
|---|---|
| `logicOpen {owner}`, `logicClose`, `logicView {view}` | Graph eines Actors (oder des Levels) öffnen, schließen, Ansicht |
| `logicPalette {query, from}` | Aktionsmenü: Suche über Titel, Kategorie, Hilfe, Suchwörter; beim Ziehen von einem Pin nur Knoten, die den Draht annehmen; dazu „Get/Set/Add to <Variable>“, „Call/Event <Name>“, „Go to <Zustand>“, „On <Zustand> Entered“, „Add Reroute Node“, die Knoten jeder Aufzählung bei ihrem Namen (für eine Aufzählung `Mode`: „Switch on Mode“, „Mode to Text“ …) und die jeder Struktur („Make Weapon“, „Break Weapon“ und „Set Damage of Held“ für jedes Feld jeder Struktur-Variablen); nach Kategorien geordnet, höchstens 120 |
| `logicAdd {type, x, y, params, from}` | Knoten auf das 8-px-Raster setzen; ohne Variable/State Machine wird eine angelegt; von einem Pin fallen gelassen wird er gleich verbunden |
| `logicConnect`, `logicDisconnect`, `logicDelete`, `logicMove` | Drähte, Löschen, Verschieben (Kommentare samt Größe, Zustände); ein generischer Knoten (Select, Reroute, die Listen-Knoten), dessen typisierte Pins noch frei sind, nimmt den Typ des Drahts an, auch den einer Aufzählung oder Struktur |
| `logicValue`, `logicParam`, `logicComment` | Literal, Einstellung (Drähte zu weggefallenen Pins werden entfernt), Knotenkommentar |
| `logicVariable {op}` | `add`, `remove`, `rename` (auch in Knoten und Übergängen), `type` (Wert wird umgewandelt), `value` |
| `logicEnum {op}` | die Aufzählungen des Blueprints: `add` (sie beginnt als `NewEnum` mit den Werten `First` und `Second`), `remove`, `rename`, `help` (ihre Beschreibung); ihre Werte: `addValue`, `renameValue`, `moveValue` (nach oben oder unten), `removeValue` |
| `logicStruct {op}` | die Strukturen des Blueprints: `add` (sie beginnt als `NewStruct` mit dem Feld `Field`, einem Float), `remove`, `rename`, `help` (ihre Beschreibung); ihre Felder: `addField`, `renameField`, `moveField` (nach oben oder unten), `removeField`, `fieldType`, `fieldValue` (sein Standardwert) |
| `logicMachine {op}` | State Machines und Zustände anlegen, umbenennen, entfernen, Anfangszustand, Übergänge (`addTransition`, `editTransition`, `removeTransition`; Bedingung ohne Variable legt eine boolesche „Ready“ an) |
| `logicCommentBox {op}` | Kommentarrahmen |
| `logicPromote` | „Promote to Variable“: neue Variable vom Typ des Pins, über Get (Eingang) oder Set (Ausgang) angeschlossen |
| `logicInsertReroute` | Doppelklick auf einen Draht: Reroute-Knoten |
| `logicCopy`, `logicPaste` | als Graph-JSON |
| `logicCompile` | `check`-Ergebnisse |
| `logicExportCpp` | die Quelle, die ein Spiel-Build für diesen Blueprint kompiliert: seine C++-Klasse und der Kleber (Datei `blueprint_<name>.cpp`) |
| `logicBreakpoint {node, on}` | F9; läuft der Knoten beim Spielen, hält das Spiel nach diesem Simulationsschritt an, und der Editor springt hin |
| `logicTrace` | gelaufene Knoten, Variablen, Zustände, Stufe |
| `logicTemplate {template}` | Vorlage unter den vorhandenen Knoten einfügen |
| `logicRemoveGraph` | Graph des Actors entfernen bzw. Level-Graph leeren |

**Aufzählungen im Editor.** My Blueprint hat einen Abschnitt **Enums**; **+** legt eine an. Ist
sie ausgewählt, zeigt **Details** ihren Namen, eine Beschreibung und ihre Werte: Jeden Wert kann
man umbenennen, nach oben oder unten schieben oder entfernen; **+ Value** fügt einen an. Die
Auswahlliste *Type* einer Variablen und der Ein- und Ausgänge einer Funktion führt die
Aufzählungen des Blueprints und eine Liste von jeder (`Mode`, `Mode List`). Ein Pin vom Typ einer
Aufzählung, an dem kein Draht hängt, zeigt eine Auswahlliste mit ihren Werten; den Standardwert
einer Variablen dieses Typs wählt man genauso, und eine Liste einer Aufzählung hat eine
Auswahlliste je Eintrag.

Eine Aufzählung, die benutzt wird, lässt sich weiter bearbeiten. Die Aufzählung oder einen Wert
umzubenennen, ändert nichts außer dem Namen. Einen Wert nach oben oder unten zu schieben, lässt
alles bei seinem Wert: was in Pins und Variablen eingetragen ist, und die Drähte eines Switch.
Wird ein Wert entfernt, ist, was dieser Wert war, danach der erste Wert, und der Draht an seinem
Switch-Ausgang fällt weg. Eine Aufzählung, von deren Typ noch etwas ist (eine Variable, ein Pin
einer Funktion, ein Knoten), lässt sich nicht entfernen; der Editor sagt, was sie benutzt. Der
Name einer Aufzählung und die Namen ihrer Werte sind C++-Namen: Buchstaben, Ziffern und `_`,
keine Ziffer am Anfang, kein Wort von C++ und keins der Schreibweise (`Float` …). Der Name einer
Aufzählung endet nicht auf `_list`, keine zwei Werte einer Aufzählung heißen gleich, und eine
Aufzählung hat mindestens einen Wert (höchstens 256).

**Strukturen im Editor.** My Blueprint hat einen Abschnitt **Structures**; **+** legt eine an,
und die Knöpfe Make und Break an ihrer Zeile setzen ihre Knoten. Ist sie ausgewählt, zeigt
**Details** ihren Namen, eine Beschreibung und ihre Felder, jedes mit Namen, Typ und
Standardwert; ein Feld kann man umbenennen, nach oben oder unten schieben oder entfernen, und
**+ Field** fügt eins an. Die Auswahlliste **Type** eines Felds führt die Wert-Typen, die
Aufzählungen und die Strukturen, die diese nicht enthalten, und eine Liste von jedem. Die
Auswahlliste **Type** einer Variablen und der Ein- und Ausgänge einer Funktion führt die
Strukturen des Blueprints und eine Liste von jeder (`Weapon`, `Weapon List`). Der Standardwert
einer Struktur-Variablen hat eine Zeile je Feld mit dem Editor des Feld-Typs, eine Liste von
Strukturen hat diese Zeilen je Eintrag. Ein Pin vom Typ einer Struktur, an dem kein Draht
hängt, beginnt beim Standardwert der Struktur und zeigt am Knoten in Kurzform, was er hält;
eingetragen wird sein Wert in Details, Feld für Feld. Strukturen haben ein eigenes Blau
(`#3f6fd6`).

Eine Struktur, die benutzt wird, lässt sich weiter bearbeiten. Die Struktur oder ein Feld
umzubenennen, ändert nichts außer dem Namen: Die Werte und die Drähte an Make und Break bleiben.
Ein Feld zu verschieben, ändert nur seinen Platz. Ein neues Feld hat überall seinen
Standardwert. Wird ein Feld entfernt, fällt sein Wert überall weg, ebenso die Drähte an seinen
Pins; ein Knoten, dessen Weg durch es führt, meldet einen Fehler. Bekommt ein Feld einen anderen Typ,
bleibt sein Wert, wo die Typen sich umwandeln, und Drähte, die nicht mehr passen, fallen weg.
Nimmt ein Feld den Typ einer Struktur an, die unter dieser steht, rückt diese Struktur unter
sie. Eine Struktur, von deren Typ noch etwas ist (eine Variable, ein Pin einer Funktion, ein
Feld, ein Knoten), lässt sich nicht entfernen; der Editor sagt, was sie benutzt. Der Name einer
Struktur und die Namen ihrer Felder sind C++-Namen wie bei Aufzählungen; der Name einer Struktur
endet nicht auf `_list`, keine zwei Felder einer Struktur heißen gleich, und kein Feld heißt wie
eine Aufzählung oder Struktur des Blueprints.

Mehrere Befehle mit derselben `transaction` werden ein Undo-Schritt (Drähte aufnehmen und
woanders ablegen), aufeinanderfolgende Werteingaben in denselben Pin ebenfalls. Test:
`studio_logic_editor_commands`, `studio_logic_lists`, `studio_logic_enums`, `studio_logic_structs`,
`studio_blueprint_breakpoints_pause_the_game`, `studio_blueprint_comments_reroutes_promote`.

---

## 9.11 Beispiele

Alle Beispiele sind echte Graphen aus dem Code.

### Punktezone (Vorlage `score_zone`)

Wer den Sensor betritt, bekommt 10 Punkte, eine Meldung und einen Ton – der Graph aus 9.1:
On Begin Overlap → Add Score (10) → Show Message („+10“) → Play Sound (`builtin:pickup`).

### Tür (Vorlage `door`)

![Der Graph der Vorlage door: Flip Flop wechselt zwischen zwei Tweens, die die Tür hoch und runter schieben](../abbildungen/logik-tuer.svg)

Interact wechselt zwischen Öffnen und Schließen; jeder Tween läuft 0,8 s und schiebt die Tür jeden
Schritt ein Stück. Eine Sequence lässt Interact beides tun – die Tür schalten und den Ton spielen:
Ein Exec-Ausgang hat genau einen Draht.

### Spawner (Vorlage `spawner`)

Every 3 s → Branch (Spawned < 10) → Spawn Actor (Vorlage „Physics Cube“ bei (0, 0, 6)) → Add to
Spawned. Gespawnte Kopien werden an alle Spieler repliziert.

### Ampel (Vorlage `traffic_light`)

Eine State Machine „Light“: Green → (4 s) → Yellow → (1,5 s) → Red → (4 s) → Green, alle Übergänge
mit Timer. „On State Entered (any)“ zeigt `Current State` als Meldung.

### Hindernis ausweichen (Vorlage `robot_avoid`)

Begin Play schaltet die Autonomie des Roboters ab. Alle 0,25 s: Sieht der ToF etwas näher als 1,6 m,
dreht er auf der Stelle (45 °/s für 0,6 s), sonst geht er geradeaus (0,3 m/s).

### Micro Duck zwitschert beim Ball

Der Level-Graph des Beispiel-Levels Micro Duck (`micro_duck_level` in `demos.cpp`):

![Alle 0,25 s: zwitschern, wenn der Ball sichtbar ist und BallSeen noch falsch ist; danach merkt sich BallSeen die Sichtbarkeit](../abbildungen/logik-ball.svg)

Die boolesche Variable „BallSeen“ sorgt dafür, dass die Ente nur einmal zwitschert, wenn der Ball
ins Bild kommt – nicht alle 0,25 s, solange er sichtbar ist. Ein Kommentarrahmen im Graphen sagt
ausdrücklich, dass das eine Demo der Roboter-Knoten ist und nichts, was der Roboter von selbst tut.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
