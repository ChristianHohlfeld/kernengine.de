# 9 Logic Graphs

Kern has **no scripting language**. Game logic consists of graphs in the style of Unreal's
Blueprints: a graph is pure data (nodes, wires, literals, variables, state machines), and every node
type is a C++ function in a registry. Graphs are checked, translated into an index table and executed
by a small interpreter in the engine's fixed simulation steps – only on the authority (the
single-player game or the network server). Their effects are replicated like everything else the
simulation does.

So that logic is not slow, Kern translates every graph at runtime into **WebAssembly** (JIT) when
Play or Simulate starts; the browser turns that into machine code. And a graph **is** readable C++:
every Blueprint has exactly one C++ text, a class of its own, and a game's build compiles it in
([blueprint_cpp.md](../blueprint_cpp.md)). All three tiers must deliver exactly the same
result – why, and how this is ensured, is explained in Section 9.9.

```
include/kern/logic/logic.h     data model, Library, check, Runtime, Instance, Context, codegen API
src/logic/logic.cpp            values, graph JSON, Library, check, interpreter
src/logic/core_nodes.cpp       core nodes (events, flow, variables, state machines, math, text …)
src/logic/codegen.cpp          compile_wasm (JIT)
src/logic/notation.cpp         the Blueprint as a C++ class and back (to_cpp, from_cpp, native_glue)
include/kern/logic/blueprint.h what that class is compiled against; nodes.h: what the nodes compute
src/logic/logic_api.cpp        C ABI between engine and JIT modules
src/game/logic_node_shapes.cpp game nodes as the library knows them: names, pins, settings
src/game/logic_nodes.cpp       what the game nodes do (actors, physics, combat, robots …), as functions
include/kern/game/blueprint_nodes.h  those functions (kern::game::nodes) and their calls in a Blueprint's C++
src/game/logic_world.cpp       graphs in the GameWorld: host, events, replication
src/studio/logic_edit.cpp      editing in Studio
web/studio/logic.mjs           Blueprint editor (user interface)
web/kern.mjs                   JIT connection in the browser
tests/logic_cases.h            cases that must run identically in every tier
```

---

## 9.1 The model

From the header comment of `logic.h` (Unreal Blueprint semantics):

- **Exec pins** (white arrows): what runs next. Event nodes begin a chain, action nodes continue it,
  flow nodes branch, loop or wait.
- **Data pins**: typed values. Inputs **pull**: pure nodes (math, getters) are evaluated on demand;
  outputs of action nodes keep the value they had at their last run. Unconnected inputs use the
  literal entered in the node.
- **Variables**: per graph instance, typed, with a default value.
- **Lists**: a pin or a variable may hold a list of values of one type (Unreal's arrays). A list
  on a wire is a value like any other: whoever reads it has the list as it was. The steps that
  change a list (Add, Insert, Remove, Clear) change a list where it stands – a variable, or a
  list inside one; For Each goes through a list as it was when the loop started.
- **Places**: a variable – or what fields and list entries lead to from it, as deep as it goes:
  `Held.Damage`, `Scores[i]`, `Cars[i].Wheels[j].Slip`. Every node that names a variable can name
  a place in it (the setting Path, with an index pin for every list entry): Get, Set and Add,
  and the steps that change a list. What is changed is changed where it stands – nothing is read
  out and written back, in any tier. A place beside a list is an entry of its own with the
  defaults, of which nothing stays: no index leads into the void, and naming a place changes
  nothing anywhere.
- **Reference inputs**: an input of a function is a value – or a reference: then the call names
  a place instead of giving a value (`Drive(Cars[i], 0.5)`), and the function works on that
  place where it stands, to change it (`Car& car`) or only to read it (`const Car& car`).
  Nothing is copied in or out. While a call holds an entry of a list, nothing in it changes how
  long that list is – the check sees to that.
- **Libraries**: a Blueprint that keeps nothing – types and functions, no variables, no events – is
  a library. Other Blueprints use it: they have its types and call its functions directly
  (`Workshop::skid(Spare[i], 1.0)`), also with references. So every function is in the project
  once. In the interpreter a Blueprint runs as one graph with its libraries (`logic::link`); in
  the class the library is a base.
- **Enums**: a Blueprint may declare enums – a type of its own with named values, for example
  `Mode` with the values `Closed`, `Opening`, `Open`. A value of an enum is exactly one of its
  values, held as its place among them (the first is 0). It is neither a number nor a text: it
  converts to nothing, and a wire only connects pins of the same enum. Where a number or a text
  is wanted, a node says what is meant (Enum to Index, Enum to Text).
- **Structs**: a Blueprint may declare structs – a type of its own with named fields, for example
  `Weapon` with the fields `Name`, `Type`, `Damage`. Each field has a type and a default. A value
  of a struct holds a value for each of its fields. It converts to nothing, and a wire only
  connects pins of the same struct. Make builds a value from its fields, Break gives the fields;
  one field of a variable is a place (`Held.Damage`), read and set like the variable itself.
- **State machines**: states and transitions in the style of the state editor of
  digitalisierungsplanung.de. A transition fires on an event, a timer, a condition variable or
  automatically; entering and leaving a state trigger events.
- **Latent nodes**: Delay, Tween and Retriggerable Delay interrupt the chain and continue it in the
  tick, like latent Blueprint actions.

A level has a **level graph** (Self = none); every actor can carry a graph of its own (its Self).

### Data structures

| Structure | Fields |
|---|---|
| `Graph` | `nodes`, `links`, `variables`, `machines`, `comments`, `next_id` |
| `Node` | `id`, `type` (registry key, e.g. `flow.branch`), `x`, `y` (canvas pixels), `values` (literals of unconnected inputs, per pin ID), `params` (settings: variable name, event name, key …), `comment` |
| `Link` | `from` (node), `out` (output pin), `to`, `in` |
| `Variable` | `name`, `type`, `value` (default), `help` |
| `StateMachine` | `id`, `name`, `states`, `transitions` |
| `State` | `id`, `name`, `x`, `y`, `initial` |
| `Transition` | `id`, `from` (0 = from any state), `to`, `trigger` (`event`, `timer`, `condition`, `automatic`), `event`, `seconds` (default 1), `variable`, `label` |
| `CommentBox` | `id`, `x`, `y`, `w` (400), `h` (200), `text`, `color` (0 = gray) – for the editor only, no effect |

### Pin types and values

`PinType` is `exec`, `boolean`, `integer`, `number`, `vector`, `text`, `actor` (in JSON `exec`,
`bool`, `int`, `float`, `vector`, `text`, `actor`; on reading also `number`, `double`, `boolean`,
`integer`, `string`). A `Value` carries a type and the matching content (`b`, `i`, `n`, `v`, `s`,
`a`; `ActorRef` is the actor ID).

After the six value types come the six **list types**, in the same order: `boolean_list` …
`actor_list` (in JSON `bool_list`, `int_list`, `float_list`, `vector_list`, `text_list`,
`actor_list`; `is_list`, `element_of` and `list_of` go from one to the other). A list `Value`
holds its items in `list` (`Value::of(std::vector<T>)`, `items<T>()`, `size()`, `item(k)`); the
items are shared, not copied, when the value is passed on, and whoever changes them (`edit<T>()`)
changes a copy of its own. In JSON a list is the array of its items. A list of whole numbers
holds each of them as every `integer` is held.

A seventh value type and a seventh list type are for what a Blueprint declares itself, its
**enums** (`Graph::enums`; an `EnumType` has `name`, `values` and `help`): `enumeration`, a value
of an enum, comes after `actor`, and `enumeration_list` after `actor_list` (in JSON `enum` and
`enum_list`). Which enum is meant stands beside the type, as the enum's name: `of` in a
`Variable`, a `FunctionPin` and a `PinDef`. The `Value` holds the place of the value among the
enum's values, the first being 0 (`Value::enumeration(k)`; a list: `Value::enumerations`), and
so does the file:

```json
"enums": [
  {"name": "Mode", "values": ["Closed", "Opening", "Open"], "help": "What the door is doing."}
],
"variables": [
  {"name": "State", "type": "enum", "of": "Mode", "value": 0},
  {"name": "Seen", "type": "enum_list", "of": "Mode", "value": [0, 2]}
]
```

`enums` only appears if the Blueprint declares any. A node's setting names the enum
(`"params": {"enum": "Mode"}`), and a type setting names it as `"type": "Mode"`, a list of its
values as `"Mode_list"`. From outside the Blueprint – code that sets a variable or calls a
function of an instance (`Instance::set`, `Instance::call`) – what is no value of the enum is its
first value. In a Blueprint's C++ an enum is `KERN_ENUM(Mode, Closed, Opening, Open);`
([blueprint_cpp.md](../blueprint_cpp.md)).

An eighth value type and an eighth list type are for the Blueprint's **structs**
(`Graph::structs`; a `StructType` has `name`, `fields` and `help`, a `StructField` has `name`,
`type` and `value`, its default): `structure`, a value of a struct, comes after `enumeration`,
and `structure_list` after `enumeration_list` (in JSON `struct` and `struct_list`). Which struct
is meant stands beside the type as the struct's name, in `of`, as with an enum. A field is of a
value type, an enum, a struct declared above this one, or a list of one of these; so no struct
contains itself. A struct has at least one field and at most 96. The `Value` holds one value per
field, in the order of the fields (`Value::structure(fields)`, `field(i)`; a list:
`Value::structures`); copies share the fields until one of them changes. The file holds a value
as an object of its fields by their names:

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

`structs` only appears if the Blueprint declares any. A field missing in a value has its
default. Because a value's JSON goes by the declaration, a struct's value is written and read
with it: `to_json(graph, of)` and `Value::from_json(json, type, graph, of)`; `default_of` gives
a struct's default, `zero_of` the empty value of any type. Two values are equal if all their
fields are. A type setting names the struct as `"type": "Weapon"`, a list as `"Weapon_list"`.
From outside the Blueprint (`Instance::set`, `Instance::call`), what is no value of the struct –
no struct value, one with other fields, one with a field that is not of its type – is the
struct's default; in a list that holds item by item. In a Blueprint's C++ a struct is
`KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };`
([blueprint_cpp.md](../blueprint_cpp.md)).

**Types of the project.** An enum or a struct belongs to the Blueprint that declares it
(`EnumType`, `StructType`). When several Blueprints of a level have the same type, it is a type of
the project (`project = true`, in C++ `KERN_PROJECT_ENUM` / `KERN_PROJECT_STRUCT`): each has it as
a declaration of its own that is the same (`logic::same_type`), and the editor changes it in all of
them at once. A Blueprint stays readable and runnable by itself – interpreter, JIT and class see an
ordinary type – and a value of the type means the same in each when one Blueprint calls a function
of another.

A whole number (`integer`) is the same everywhere: as a Blueprint holds it (`logic::normal`).
Every number is a `double` underneath – in the interpreter's variables, in the JIT – so whole
numbers are exact up to 2^53, a larger one is the nearest a `double` holds, and none is past the
ends of 64 bits. `Value::integer` and the type `Int` of the C++ class (`logic::Whole`) see to
that; a fractional number becomes a whole one with `logic::whole` (toward zero; not a number and
endless: 0; what does not fit: the nearest end). Add to Variable adds as a number and is defined
for every value.

Which wires are allowed is determined by `convertible(from, to)`:

- the same type always; exec only with exec;
- **everything to `text`** ("everything prints") - except an enum's and a struct's values, which convert to nothing;
- numbers among themselves (`bool`, `int`, `float`);
- `actor` → `bool` ("is valid");
- `float`/`int` → `vector` (into all three components);
- a **list** goes into a list of its own type only – and into `text` (`[1, 2.5]`); nothing
  becomes a list;
- an **enum's value** converts to nothing and nothing becomes one – not a number and not `text`
  either, and the same holds for a list of them. A wire connects pins of the same enum only
  (`fits(from, to)`: `convertible`, and two enums are the same enum).
- a **struct's value** converts to nothing and nothing becomes one, and the same holds for a
  list of them. A wire connects pins of the same struct only.

The conversions (`as_bool`, `as_int`, `as_number`, `as_vector`, `as_text`, `as_actor`) are fixed,
e.g. the text "0" or "false" is false, a vector as a number is its length, a number converted to
`int` is truncated, text is parsed as a number.

### The JSON format

Graphs are stored in the level (`logic` in the level and in every actor,
[Chapter 8](08-game-runtime.md)):

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

Wires are arrays `[from, out, to, in]`. Transitions write `event`, `seconds` or `variable` depending
on the trigger; `comments` only appears if there are any. Positions are rounded to whole pixels.

---

## 9.2 Node definitions and the library

Every node type is a `NodeDef`:

| Field | Meaning |
|---|---|
| `type`, `label`, `category`, `help`, `keywords` | key, title, category, help, search words |
| `kind` | `event`, `action`, `pure`, `flow` |
| `event` | events: the `Event::type` the node handles (e.g. `logic.tick`, `game.overlap_begin`) |
| `latent` | waits (Delay, Tween) |
| `inputs`, `outputs` | pins in drawing order, exec pins included (`PinDef`: `id`, `label`, `type`, `def`, `hidden`) |
| `params` | settings (`ParamDef`: `key`, `label`, `kind` – `text`, `choice`, `variable`, `event`, `machine`, `state`, `key`, `sound`, `tag`, `integer` –, `choices`, `def`) |
| `color` | header color (0 = by kind) |
| `shape` | pins that depend on parameters or on the graph (variables, sequence outputs, switch cases) |
| `title` | title that depends on parameters ("Every 3 s", "On Key F") |
| `accepts` | events: does this node handle the event? Default: `key` empty, or equal to the first parameter, or `any` |
| `run` | the C++ function |

`run` returns the **index of the exec output** to continue with (among the exec outputs), or
`kStop` (−1, the chain ends) or `kSuspend` (−2, latent: resumed later from the tick). Pure nodes
write their outputs and return `kStop`.

A node function sees the world through `Context`:

| Method | Purpose |
|---|---|
| `in(index)`, `in(pin)`, `in_bool`, `in_number`, `in_int`, `in_vector`, `in_text`, `in_actor` | inputs (pulls connected pure nodes) |
| `out(pin, value)` | set an output |
| `param(key, fallback)` | setting |
| `exec(pin)` | ordinal of an exec output (for the return value) |
| `entry()` | which exec input started the node (0 = the first; `kResume` = 1000 after a wait) |
| `state()`, `slot(k)` | state of the node instance; `slot(0..3)` lives in memory shared with generated code |
| `wait(seconds, entry)` | latent: continue with `entry` after `seconds`, returns `kSuspend` |
| `cancel_waits()` | discard its own waits (Retriggerable Delay) |
| `then(out)` | continue at this output after the current chain ends (Sequence) |
| `loop_back(entry)` | re-enter this node with `entry` after the current chain ends (loops) |
| `self()`, `host()`, `runtime()`, `instance()` | context |

`Library::core()` contains the built-in nodes; `logic::library()` is core plus whatever the game has
registered. The game nodes register themselves at program start
(`game::logic_nodes_registered()` ensures that they get linked).

A game node is written in two places. **What it looks like** – name, pins, settings – is in
`logic_node_shapes.cpp`; that file needs nothing of the game, so tools that only read and write
Blueprints know the nodes too (`tools/blueprint_cpp`):

```cpp
lib.add(step("game.add_score", "Add Score", "Game", {data("points", "Points", PinType::number, 10.0)}, {},
             "Adds points (wins at the target score when the goal is Score).", "points reward"));
```

**What it does** is a typed function in `kern::game::nodes` (declared in
`kern/game/blueprint_nodes.h`, defined in `logic_nodes.cpp`): the game, Self, then the node's data
inputs in their order. `bind` attaches it to the node:

```cpp
void game_add_score(GameWorld& w, Self, Float points) { w.add_score(static_cast<f32>(points)); }
...
bind(lib, "game.add_score", &nodes::game_add_score);
```

The interpreter calls it through the node, a Blueprint's C++ calls it directly
(`game_add_score(5.0);`, [blueprint_cpp.md](../blueprint_cpp.md)) – both the same function. A node
with several outputs gives a `struct` whose fields are named like the pins; an actor input without
a value means Self inside the function. So a new game node needs: its line in
`logic_node_shapes.cpp`, its function and `bind`, its call in `BlueprintNodesOf`
(`blueprint_nodes.h`) and a place in a Blueprint of the test levels (`tests/game_cases.h`) – the
test `game_blueprints_as_classes` tells which of these is missing.

---

## 9.3 Checking

`check(graph, lib)` returns `Issue`s (`node`, `error`, `text`). Errors:

- unknown node type;
- parameters that point to missing variables, state machines or states; an event without a name;
- a wire to a pin that does not exist;
- an exec wire on a data pin, non-convertible types;
- a data input with more than one wire;
- pure nodes that depend on themselves (cycle);
- a state machine without exactly one initial state; an event transition without an event name; a
  condition transition on a missing variable.

Warning: a node with an exec input that nothing is connected to ("never runs").

A graph with errors is **disabled** at start and reported; the rest of the game keeps running. In
Studio, Compile (F7) shows the results.

---

## 9.4 Execution

### Runtime and instances

A `Runtime` executes all graphs of a world; every graph runs as an `Instance` for an owner (actor or
level).

| Method | Effect |
|---|---|
| `add(graph, self, name)` | translates and adds; check errors disable the instance; afterwards native C++ code (if registered) or a JIT request |
| `remove(self)`, `clear()` | removes (including waiting latent actions) |
| `begin()` | Begin Play for instances that have not begun yet: enter initial states, run `event.begin` |
| `tick(dt)` | one fixed step: due latent actions (sorted by time), then the tick event (`logic.tick` with `delta`), then the state machines' timer and condition transitions |
| `fire(event)` | fires an event synchronously |
| `call(name, target, args)` | custom event on all instances (or the target's) and event transitions with this name |
| `find(self)`, `instances()` | access |
| `trace()`, `tracing` | which nodes ran (for the editor) |
| `budget` | 200,000 loop passes per step (the loop guard) |
| `jit` | enable the JIT (default: on, where the host can) |

An `Instance` offers `get`/`set` for variables, `state`/`set_state` for state machines,
`disabled()`, `compiled()` and `tier()` – `"interpreter"`, `"compiling"`,
`"compiled (WebAssembly)"` or `"native C++"`.

### Events

```cpp
struct Event {
  std::string type;          // e.g. "game.overlap_begin"
  ActorRef target = 0;       // only instances with this Self (0 = all)
  bool level = false;        // with target: the level graph gets it too
  std::string key;           // event-specific: custom event name, key code, action, "Machine/State"
  std::vector<std::pair<std::string, Value>> args;
};
```

`fire` goes over all non-disabled instances (matching the target), looks for event nodes with a
matching `event` and `accepts`, writes the arguments into the outputs of the same name and executes
the chain. Instances that are added in the meantime (spawned actors) are included.

### Running a chain

The interpreter (`Runtime::run_from`) works with a stack of (node, input) pairs:

1. Execute the node; whatever it schedules via `then` or `loop_back` is pushed onto the stack – in
   reverse order, so that what was scheduled first runs first.
2. If it returns a valid exec output, execution continues at the connected node; otherwise this
   branch ends and the next stack entry runs.

This is how Sequence (output 0 immediately, the others scheduled) and loops (execute the body, then
re-enter the node itself via `loop_back` with `kResume`) work.

Data is **pulled**: an input connected to a pure node executes that node's `run` on every read
– as deep as the values read each other (a value that reads itself is a compile error). An input
on an action node reads that node's last stored output.

### Protection against infinite loops

- The loop guard: more than `budget` (200,000) loop passes in one step. Loops are the one thing in
  a graph without a fixed end – a path that leads back into itself without a wait, a function that
  calls itself and a tween played from its own Update are compile errors. Over the limit the loops
  of the chain end at once, the rest of the chain runs to its end, and then the instance is
  disabled ("infinite loop? The Blueprint is stopped."). Every tier counts the same passes
  (`Runtime::pass`) and stops at the same one – also a Blueprint compiled as its C++ class, which
  cannot abort a chain in its middle.
- Events that trigger events may nest at most 64 deep; beyond that, the chain is stopped – in
  every tier at the same event. Function calls do not count (in the C++ class they are ordinary
  calls, and a function cannot call itself). If a function raises an event whose handler calls it
  again, that is a call like any other; when the handler is over, the interrupted call has its
  parameters again.
- The JIT's stack for open sequences and loops is as large as the graph can ever need
  (`Instance::stack_capacity`: 66 × the exec outputs of its flow and loop nodes, at least 512).
  So it never runs full where the interpreter and the C++ class go on; a graph that would need
  more than 32,768 entries is not JIT-compiled.

### State machines

- `begin` enters the initial state.
- Entering and leaving fire `logic.state_enter` / `logic.state_exit` with the key
  `"Machine/State"`; "On State Entered" receives the previous state.
- Transitions are checked by `check_transitions` – **at most one transition per machine and call**
  (automatic chains advance one state per step):
  - `event`: on `call` with this name,
  - `timer`: after `seconds` in the state,
  - `condition`: as long as the boolean variable is true,
  - `automatic`: immediately on the next tick.
- `from = 0` means "from any state" (but not back into the target state itself).
- `Set State` (`state.set`) switches directly.

---

## 9.5 Core nodes

`src/logic/core_nodes.cpp` registers 105 nodes:

| Category | Nodes |
|---|---|
| **Events** | `event.begin` Begin Play · `event.tick` Event Tick (Delta Seconds) · `event.every` Every N Seconds (interval parameter, keeps its own time, output Count) · `event.custom` Custom Event (name; outputs Actor, Value, Text; also switches state machine transitions of the same name) |
| **State machines** | `event.state_enter` On State Entered · `event.state_exit` On State Left · `state.set` Set State · `state.get` Current State · `state.is` Is In State |
| **Flow** | `flow.reroute` / `value.reroute` (nodes for wires) · `flow.branch` Branch · `flow.sequence` Sequence (1..16 outputs) · `flow.for` For Loop (First, Last inclusive; Index) · `flow.for_each` For Each (a list; Item, Index; the list as it is when the loop starts) · `flow.while` While Loop · `flow.do_once` Do Once (Reset, Start Closed) · `flow.do_n` Do N (N, Reset, Counter) · `flow.gate` Gate (Enter, Open, Close, Toggle, Start Closed) · `flow.flip_flop` Flip Flop (A/B, Is A) · `flow.delay` Delay (calls during the wait are ignored) · `flow.retrigger_delay` Retriggerable Delay · `flow.tween` Tween (Play, Stop, Duration; Update every step with Alpha 0→1, easing linear/smooth/in/out, then Finished) · `flow.switch_int` Switch on Int · `flow.switch_text` Switch on Text (cases separated by commas) · `flow.switch_enum` Switch on Enum (setting Enum; an exec output for each value of the enum, labelled with the value's name) · `flow.call` Call Event (Target, Actor, Value, Text) |
| **Variables** | `var.get` · `var.set` · `var.add` (Amount, −1 counts down) – each on a variable or, with the setting Path, on a place in it: a field (`.Damage`), a list entry (`[]`, with an index pin), as deep as it goes (`[].Wheels[].Slip`); Value: what is there now |
| **Lists** | values, generic over the items' type (setting Type): `list.make` Make List (0..16 items) · `list.length` · `list.get` Get Item (beyond the list: the type's zero) · `list.contains` · `list.find` Find Item (index or −1) · `list.find_row` Find Row (a list of structs as a table: the first item whose field is the value; Found, Row, Index) – steps on a list where it stands (settings List and Path: a list variable, or a list at a place in a variable): `list.add` Add to List (gives the index) · `list.insert` (in front of the index; past the end: at the end) · `list.remove_at` Remove Index · `list.remove` Remove Item (every equal item; Removed) · `list.clear` |
| **Enums** | on the values of one enum of the Blueprint (setting Enum; in the editor a node carries the enum's name, "Mode to Text"): `enum.equal` Equal (Enum) (A, B; Result) · `enum.name` Enum to Text (the value's name) · `enum.from_name` Text to Enum (the value with that name; Found – no value has that name: the first value, and Found is false) · `enum.index` Enum to Index (the place of the value among the enum's values, the first is 0) · `enum.from_index` Index to Enum (the value at that place; Valid – no such place: the first value, and Valid is false) · `enum.values` All Values (a list of every value in their order, for a For Each) – the nodes that take any type work in an enum's type too: Get and Set of a variable, Select, Reroute, the list nodes and For Each |
| **Structures** | on one struct of the Blueprint (setting Struct; in the editor a node carries the struct's name, "Make Weapon"): `struct.make` Make Struct (an input per field – without a wire: the field's default; Value) · `struct.break` Break Struct (Value; an output per field) – a field of a variable is set where it stands: `var.set` with the Path to it (in the editor "Set Held.Damage"; documents with the former nodes Set Field and Set List Item are read as that Set) |
| **Functions** | `func.entry` Function Entry · `func.return` Return · `func.call` Call Function · `func.pure` Function Value (a function marked Pure, as a value) · `func.call_on` Call on Actor (a function on the Blueprint of another actor or - without a Target - of the Level; the Blueprint declares what it looks like) |
| **Math** | `math.add`, `sub`, `mul`, `div`, `mod`, `min`, `max`, `clamp`, `abs`, `negate`, `lerp`, `sin`/`cos`/`atan2` (degrees), `sqrt`, `pow`, `round`, `floor`, `random` (float in range), `random_int`, `random_bool` · `math.formula` Formula (every name in it is an input) |
| **Logic** | `math.compare` Compare (`==`, `!=`, `<`, `<=`, `>`, `>=`; equality with 10⁻⁹ tolerance) · `logic.and`, `or`, `xor`, `not` · `logic.select` Select |
| **Values** | `value.float`, `value.int`, `value.bool`, `value.text` |
| **Vector** | `vector.make`, `break`, `add`, `sub`, `cross`, `scale`, `length`, `normalize`, `distance`, `dot`, `lerp` |
| **Text** | `text.append`, `equal`, `contains`, `length` |
| **Utilities** | `time.now` Game Time · `debug.print` Print (Text, On Screen) |

Random numbers come from the host (`Host::random()`) and are **deterministic per world** – replays and
the network agree.

---

## 9.6 Game nodes

The library has 73 game nodes (`logic_node_shapes.cpp`, `logic_nodes.cpp`). They call functions of the GameWorld
([Chapter 8](08-game-runtime.md)); an actor input without a wire (0) means **Self**.

### Events

| Node | Event | Outputs | Triggered by |
|---|---|---|---|
| On Begin Overlap | `game.overlap_begin` | Actor, Other Actor | something enters this actor's sensor collider |
| On End Overlap | `game.overlap_end` | Actor, Other Actor | something leaves it |
| On Entered Sensor | `game.overlapped_begin` | Actor, Sensor Actor | this actor (e.g. a player) enters someone else's sensor |
| On Hit | `game.hit` | Actor, Other, Impact Speed, Point, Normal | Box3D hit from 2 m/s, both sides, at most 32 per step |
| On Collected | `game.pickup` | Actor, Collector, Points | a pickup was collected |
| On Trigger | `game.trigger` | Actor, Player | a player touched this trigger |
| On Damaged | `game.damaged` | Actor, Attacker, Damage, Headshot | damage (requires Health) |
| On Death | `game.death` | Actor, Killer, Headshot | health 0 |
| On Respawn | `game.respawn` | Actor | return after death |
| On Interact | `game.interact` | Actor, Player, Point | Interact (F / Y on the gamepad) while looking at the actor (3 m ray) or next to it (nearest actor with a graph within 2 m) |
| On Player Joined | `game.pawn_added` | Player | a player joined |
| On Game Over | `game.over` | Won, Message | game won or lost (to all graphs) |
| On Input Action | `game.input` | Player, Pressed | action `jump`, `fire`, `aim`, `reload`, `crouch`, `interact`, `boost`; edge pressed/released/both – for every player and every device |
| On Key | `game.key` | Pressed, Player | a key of this keyboard (local player only; parameter "F" → `KeyF`, "1" → `Digit1`) |

Overlaps count the visitor as a whole actor: an overlap begins with the first shape in the sensor and
ends with the last one that leaves it (a player's capsule and head are one visitor).

### Actors

`actor.self` Self · `actor.player` Get Player (0 = this machine's player, 1, 2 … further ones) ·
`actor.player_count` · `actor.find` Find Actor by Name · `actor.find_tag` Find Actor with Tag ·
`actor.all_with_tag` All Actors with Tag (an Actor List) ·
`actor.random_tag` · `actor.has_tag` · `actor.name` · `actor.valid` · `actor.location` ·
`actor.rotation` · `actor.forward` · `actor.velocity` · `actor.distance` · `actor.set_location` ·
`actor.set_rotation` (degrees) · `actor.move_by` · `actor.spawn` Spawn Actor (copy of a template with
children and logic, replicated to all players) · `actor.destroy` · `actor.enable` Set Enabled ·
`actor.set_property` Set Property (any component value via its path as in the level file, e.g.
`components.mesh.material.color` – replicated)

### Physics

`actor.set_velocity` · `actor.impulse` Add Impulse · `actor.force` Add Force (for one step) ·
`actor.launch` Launch (keeps the horizontal velocity, adds this one) · `actor.simulate` Set Simulate
Physics (dynamic ↔ kinematic) · `actor.joint_motor` Set Joint Motor (hinge, slider, rope: doors,
elevators, winches) · `actor.explode` Explosion (Box3D radial impulse with flash, sound and falling-off
damage) · `physics.trace` Line Trace (Box3D ray) · `physics.trace_channels` Line Trace by Channels (the same ray with Box3D's query filter: the collision channels it meets, and its own) · `physics.channels` Channels (the bits of the level's collision channels with these names)

### Game, combat, effects, audio

`game.score` Get Score · `game.message` Show Message (all players see it) · `game.add_score` ·
`game.win` · `game.lose` · `combat.damage` Apply Damage · `combat.heal` · `combat.kill` ·
`combat.ammo` Give Ammo · `actor.health` Get Health · `actor.set_light` · `actor.set_color` ·
`game.particles` Spawn Sparks · `game.shake` Camera Shake (this machine only) · `game.sound` Play
Sound (3D at the location or at Self, or flat) · `flow.for_each_tag` For Each Actor with Tag

### Robots

`walker.move` Walk (like the gamepad: forward up to 0.4 m/s, sideways 0.3, turning 57 °/s – the
trained ranges; overrides the robot's own behavior) · `walker.look` Look (head: yaw up to 80°, pitch
up to 63°) · `walker.autonomous` Set Autonomous · `walker.skill` Robot Move (`sit`, `pick`,
`kick_left`, `kick_right`, `roulade`, `jump`) · `walker.beak` · `walker.pet` · `walker.hear` ·
`walker.holding` · `walker.quack` Chirp · `walker.tof` Robot ToF · `walker.sees` Robot Camera Sees ·
`walker.state` Robot State. Details in [Chapter 13](13-micro-duck.md).

---

## 9.7 Logic in the GameWorld

`GameWorld::start_logic` (`src/game/logic_world.cpp`) only creates the runtime if

- the mode is Play or Simulate and the world is **not a replica** (logic only runs on the
  authority), and
- the level or an actor has a non-empty graph.

Then the level graph (Self 0, name "Level") and every actor graph (Self = actor ID, name = actor
name) are added; `start` then calls `begin`. Spawned actors and new pawns bring their graphs along.

The world's **host** (`GameWorld::LogicHost`):

| Method | Implementation |
|---|---|
| `time()` | `sim_time_` – the simulation time in seconds, computed from the step counter, not the wall clock |
| `print(text, screen)` | "Logic: …" into the messages, with `screen` also as a HUD message |
| `error(text)` | "Logic error: …" into the messages (Studio shows them in the Output Log) |
| `random()` | Xorshift32, seeded from the number of actors – deterministic |
| `game()` | the GameWorld (for the game nodes) |

**Events to graphs** (`logic_fire`): the event goes to the graph of the affected actor or, if it has
none, to that of its nearest ancestor with a graph (the welded-in collider of a door belongs to the
door), and always to the level graph as well. Events without an affected actor (game over) go to
all. The logic tick runs at the end of every fixed step, after the physics events
([Chapter 8](08-game-runtime.md), Section 8.9).

If a level has logic, the GameWorld enables **sensor events on all movable shapes and hit events on
all dynamic shapes** – otherwise there would be no overlaps and hits for the graphs.

**Replication.** Whatever logic changes in the world is recorded: `set_property` (path and value),
`spawn_copy` (template, ID, pose) and moving static bodies (`teleport` writes
`transform.position`/`rotation`). The server sends these changes to all clients, including players
who join later ([Chapter 10](10-networking.md)).

---

## 9.8 Codegen: from graph to machine code

A graph becomes machine code in two ways:

- **In a game's build: its own C++ class.** Every Blueprint has exactly one C++ text
  (`logic::to_cpp`) in which every node is a function call; `tools/blueprint_cpp.sh` writes the
  classes of all Blueprints of a level into a source of the game, and `Runtime::add` runs a graph
  whose class the program has (found by `graph_hash`) as that class – the tier "native C++". This
  is the fastest way: the compiler sees everything. How it works is in
  [blueprint_cpp.md](../blueprint_cpp.md).
- **At run time: the JIT.** `compile_wasm(instance)` (`src/logic/codegen.cpp`) writes a
  WebAssembly module. The host compiles it with the browser's optimising compiler and links it
  into the engine's function table. The graph then runs as native code and calls the engine's C++
  node functions directly (wasm-to-wasm calls, no script engine in between). This is the way for
  a graph that is being edited and played at once.

The rest of this section describes the JIT.

### What is generated inline

Control flow (Branch, Sequence, For, While, Do Once, Do N, Gate, Flip Flop), numeric variables (Get,
Set, Add), math (`add`, `sub`, `mul`, `div`, `mod`, `min`, `max`, `clamp`, `abs`, `negate`, `lerp`,
`sqrt`, `floor`, `round`), comparisons, boolean logic, number/integer/bool literals and a numeric
`Select`. **Everything else** – game nodes, text, vectors, latent nodes – is a call to the node's C++
function (`call_node`), after which execution continues at the chosen exec output (from a jump
table).

### Shared memory

The shared state lives in the instance, at fixed addresses that never change after creation
(`CodeLayout`):

| Area | Content |
|---|---|
| `slots()` | f64 slots: first the numeric variables (bool, int, float), then **8 per node** – 4 for state (loop index, gate open, flip-flop …) and 4 for data outputs of inline-generated exec nodes |
| `stack()` | i32 continuation stack: `[0]` = depth, then pairs (pc, entry); `stack_capacity()` entries (as many as the graph can ever need) |
| `exec_table()` | per node and exec output: (target, input) |
| `counters()` | `[0]` nodes run (a count), `[1]` the limit of loop passes per step (the budget), `[2]` tracing on, `[3]` loop passes in this step, `[4]` a loop of the running chain was refused a pass |

Non-numeric variables (vector, text, actor) live in a map in the instance and are read via node
functions.

### The program

The generated program is a **loop over a program counter** – one case per node. This keeps every exec
wiring valid, including merges and cycles. Continuations of Sequence and loops go onto the
instance's stack, exactly as in the interpreter. Every iteration increments `counters[0]`. A loop
node counts its pass in `counters[3]`; at the limit it takes Completed instead and sets
`counters[4]` (the loop guard). With a full stack the function returns 2.

The WebAssembly module:

- **Types:** `run(pc, entry) → i32`, `call_node(id, node, entry) → i32`,
  `read_pin(id, node, pin) → f64`, `trace(id, node)`.
- **Imports** from `env`: `call_node`, `read_pin`, `trace` and the **engine's memory** – slots, stack
  and table are addressed via their absolute addresses.
- **Export:** `run`.
- **Dispatch:** a `br_table` over the program counter to one block per node.

`graph_hash` is an FNV-1a over the graph JSON **without positions and comments** – moving a node
does not change it, any change of content does. A Blueprint's C++ class registers itself under
this hash; a Blueprint changed since the build therefore never runs as outdated code.

### The sequence in the browser

1. `Runtime::add` checks the graph. If the program has a C++ class for its hash, that wins.
2. Otherwise it calls `host.jit_compile(compile_wasm(instance), id)`. The default implementation of
   `Host::jit_compile`, which the GameWorld's host also uses, forwards this to
   `platform_jit_compile`, which in WebAssembly calls the host function `kern_env.logic_jit`
   (`src/logic/logic_api.cpp`). Natively there is no compiler: the graph stays in the interpreter
   (or runs as its C++ class).
3. `web/kern.mjs` translates the module asynchronously (`WebAssembly.compile`, `instantiate` with the
   engine exports `kern_logic_call_node`, `kern_logic_read_pin`, `kern_logic_trace` and the engine
   memory), places the `run` function into a free slot of the engine's function table and reports
   `kern_logic_jit_ready(id, slot)`. If anything fails, a warning is issued and −1 is reported – the
   graph keeps running in the interpreter.
4. Until then the instance is `"compiling"` and runs in the **interpreter**. From the report onward,
   `run_from` calls the table function directly: an indirect call, no JavaScript in between.
5. When the instance is destroyed, `logic_jit_release` frees the table slot.

According to `docs/studio.md`, the JIT is about 55 times faster than the interpreter; the status line
shows "compiled". The figure is measured by `tests/logic_wasm_test.mjs` with one million loop
iterations.

---

## 9.9 Why the JIT must match the interpreter

The interpreter and the JIT are **interchangeable**, even in the middle of a game (the C++ class
runs from the start – it is part of the program):

- The JIT works asynchronously. A graph starts in the interpreter – Begin Play, the first ticks, Do
  Once, running loop counters – and is taken over as soon as the module is ready. That is why the
  state (variables, loop counters, gates, flip-flops, data outputs of inline-generated nodes) does
  not live in interpreter structures but in **the same slots** that the generated code also reads
  and writes. The interpreter's node functions for these nodes likewise compute via `Context::slot`
  (for instance `flow.for`: "State in slots (index, last): the same in the interpreter and in
  generated code"). A tier switch therefore needs no copying (`enter_compiled` is empty).
- Latent nodes are managed in the interpreter part of the runtime and continue their chain – no
  matter in which tier – with the same semantics.
- Kern is deterministic ([Chapter 6](06-physics.md), Section 6.14). A graph that computed differently
  when compiled in the browser than natively in the interpreter would devalue replays, training and
  debugging. The guard against infinite loops also counts identically in every tier (loop passes
  per step) and stops the same graphs at the same pass.
- The editor debugs with the same semantics: breakpoints and the live display of running nodes also
  work in compiled code (`trace`).

This is checked in all three tiers with the same cases from `tests/logic_cases.h` (`math_loops`,
`gates_and_sequences`, `while_nested`, `mixed_cpp_nodes`, `state_machine`, `infinite_loop`,
`reroutes`, `functions`, `formulas`, `events_and_time`, `event_reentry`, `waits`, `states`,
`random`):

| Test | Tier | Checks |
|---|---|---|
| `logic_tier_cases_interpreter` (`tests/test_main.cpp`) | interpreter | expected values of each case (e.g. `Sum == 315`, `Log == 111111`, infinite loop disables); the WebAssembly module is well-formed (magic, version, sections) |
| `tests/logic_wasm_test.mjs` (with `logic_wasm_test.cpp`) | JIT in the engine's WebAssembly under Node | every case once in the interpreter, once JIT-compiled (translated by V8 and linked into the function table as in the browser); the tier must be `compiled (WebAssembly)` and the result **identical**; plus the speed measurement |
| `blueprint_native_test` (`tests/blueprint_native.h`, built by `run.sh`) | the C++ class | `kern_tests --emit-native` writes the cases' C++ texts and the interpreter's results; `run.sh` compiles the texts; every graph must run as `native C++` and give exactly the interpreter's results – plus more than 2000 single calls with every value node |
| `game_native_test` (`tests/game_cases.h`, `tests/game_native_test.cpp`) | the C++ classes in a game | two levels whose Blueprints use every game node, written as classes by `tools/blueprint_cpp`; every scenario played in the interpreter and as the classes, everything visible the same line by line |

What a node computes is therefore written only **once**: as a typed function
(`kern/logic/nodes.h`, `kern/game/blueprint_nodes.h`) that the interpreter calls through the node
and the C++ class calls directly. Whoever also generates a node inline in the JIT keeps
`WasmGen::node_code` in line with it and adds a case to `logic_cases.h` if needed.

---

## 9.10 Editing in Studio

The Blueprint editor (`web/studio/logic.mjs`) follows Unreal's Blueprint editor; its user interface
is described in [Chapter 11](11-studio.md). The page contains no editor logic: every edit is a
command to the engine (`src/studio/logic_edit.cpp`), **every edit is a level edit and therefore an
undo step**; the page draws what `logic_json()` describes (graph, nodes with pins, problems, while
playing the nodes that ran, live values of variables, states, tier).

| Command | Effect |
|---|---|
| `logicOpen {owner}`, `logicClose`, `logicView {view}` | open or close the graph of an actor (or of the level), view |
| `logicPalette {query, from}` | action menu: search over title, category, help, search words; when dragging from a pin, only nodes that accept the wire; plus "Get/Set/Add to <Variable>", "Call/Event <Name>", "Go to <State>", "On <State> Entered", "Add Reroute Node", the nodes of every enum by its name (for an enum `Mode`: "Switch on Mode", "Mode to Text" …), and those of every struct ("Make Weapon", "Break Weapon", and "Set Damage of Held" for every field of every struct variable); ordered by category, at most 120 |
| `logicAdd {type, x, y, params, from}` | place a node on the 8 px grid; without a variable/state machine one is created; when dropped from a pin it is connected right away |
| `logicConnect`, `logicDisconnect`, `logicDelete`, `logicMove` | wires, deleting, moving (comments including size, states); a generic node (Select, Reroute, the list nodes) whose typed pins are still free takes the type of the wire, an enum's or a struct's type too |
| `logicValue`, `logicParam`, `logicComment` | literal, setting (wires to pins that no longer exist are removed), node comment |
| `logicVariable {op}` | `add`, `remove`, `rename` (also in nodes and transitions), `type` (value is converted), `value` |
| `logicEnum {op}` | the Blueprint's enums: `add` (it starts as `NewEnum` with the values `First` and `Second`), `remove`, `rename`, `help` (its description); its values: `addValue`, `renameValue`, `moveValue` (up or down), `removeValue` |
| `logicStruct {op}` | the Blueprint's structs: `add` (it starts as `NewStruct` with the field `Field`, a Float), `remove`, `rename`, `help` (its description); its fields: `addField`, `renameField`, `moveField` (up or down), `removeField`, `fieldType`, `fieldValue` (its default) |
| `logicMachine {op}` | create, rename and remove state machines and states, initial state, transitions (`addTransition`, `editTransition`, `removeTransition`; a condition without a variable creates a boolean "Ready") |
| `logicCommentBox {op}` | comment boxes |
| `logicPromote` | "Promote to Variable": a new variable of the pin's type, connected via Get (input) or Set (output) |
| `logicInsertReroute` | double-click on a wire: reroute node |
| `logicCopy`, `logicPaste` | as graph JSON |
| `logicCompile` | `check` results |
| `logicExportCpp` | the source a game's build compiles for this Blueprint: its C++ class and the glue (file `blueprint_<name>.cpp`) |
| `logicBreakpoint {node, on}` | F9; if the node runs while playing, the game pauses after this simulation step and the editor jumps to it |
| `logicTrace` | nodes that ran, variables, states, tier |
| `logicTemplate {template}` | insert a template below the existing nodes |
| `logicRemoveGraph` | remove the actor's graph, or clear the level graph |

**Enums in the editor.** My Blueprint has a section **Enums**; `+` adds one. Selecting it shows
it in **Details**: its name, a description and its values, each of which can be renamed, moved up
or down, or removed; **+ Value** adds one. The **Type** dropdown of a variable, and of a
function's inputs and outputs, lists the Blueprint's enums and a list of each (`Mode`,
`Mode List`). A pin of an enum's type that has no wire shows a dropdown with the enum's values;
the default of an enum variable is chosen the same way, and a list of an enum has one dropdown
per item.

An enum that is in use can still be edited. Renaming the enum or a value changes nothing but the
name. Moving a value up or down keeps everything with its value: what is typed into pins and
variables, and the wires of a Switch. When a value is removed, what was that value becomes the
first value, and the wire of its Switch case goes. An enum that something is of (a variable, a
function's pin, a node) cannot be removed; the editor says what uses it. An enum's name and its
values' names are names of C++: letters, digits and `_`, not starting with a digit, no word of
C++ or of the notation such as `Float`. The enum's name does not end in `_list`, no two values of
an enum have the same name, and an enum has at least one value (at most 256).

**Structs in the editor.** My Blueprint has a section **Structures**; `+` adds one, and the
buttons Make and Break on its row place its nodes. Selecting it shows it in **Details**: its
name, a description and its fields, each with a name, a type and a default; a field can be
renamed, moved up or down, or removed, and **+ Field** adds one. A field's **Type** dropdown
lists the value types, the enums and the structs that do not hold this one, and a list of each.
The **Type** dropdown of a variable, and of a function's inputs and outputs, lists the
Blueprint's structs and a list of each (`Weapon`, `Weapon List`). The default of a struct
variable has a row per field with the editor of the field's type, and a list of structs has
these rows per item. A pin of a struct's type that has no wire starts at the struct's default
and shows on the node, in short, what it holds; its value is entered in Details, field by
field. Structs are a blue of their own (`#3f6fd6`).

A struct that is in use can still be edited. Renaming the struct or a field changes nothing but
the name: values, and the wires on Make and Break, stay. Moving a field changes its place only.
A new field has its default everywhere. When a field is removed, its value goes everywhere, and
so do the wires on its pins; a node whose path leads through it reports an error. When a field's type
changes, its value stays where the types convert, and wires that no longer fit go. A field that
takes the type of a struct listed below this one moves this struct below it. A struct that
something is of (a variable, a function's pin, a field, a node) cannot be removed; the editor
says what uses it. A struct's name and its fields' names are names of C++, as with enums; the
struct's name does not end in `_list`, no two fields of a struct have the same name, and no
field has the name of an enum or a struct of the Blueprint.

Several commands with the same `transaction` become one undo step (picking up wires and dropping them
elsewhere), as do consecutive value entries into the same pin. Tests:
`studio_logic_editor_commands`, `studio_logic_lists`, `studio_logic_enums`, `studio_logic_structs`,
`studio_blueprint_breakpoints_pause_the_game`, `studio_blueprint_comments_reroutes_promote`.

---

## 9.11 Examples

All examples are real graphs from the code.

### Score zone (template `score_zone`)

Whoever enters the sensor gets 10 points, a message and a sound – the graph from 9.1:
On Begin Overlap → Add Score (10) → Show Message ("+10") → Play Sound (`builtin:pickup`).

### Door (template `door`)

![The graph of the door template: Flip Flop alternates between two tweens that push the door up and down](../abbildungen/logik-tuer.svg)

Interact alternates between opening and closing; each tween runs for 0.8 s and pushes the door a
little every step. A Sequence lets Interact do both - switch the door and play the sound: an exec
output has exactly one wire.

### Spawner (template `spawner`)

Every 3 s → Branch (Spawned < 10) → Spawn Actor (template "Physics Cube" at (0, 0, 6)) → Add to
Spawned. Spawned copies are replicated to all players.

### Traffic light (template `traffic_light`)

A state machine "Light": Green → (4 s) → Yellow → (1.5 s) → Red → (4 s) → Green, all transitions with
a timer. "On State Entered (any)" shows `Current State` as a message.

### Avoiding obstacles (template `robot_avoid`)

Begin Play turns off the robot's autonomy. Every 0.25 s: if the ToF sees something closer than
1.6 m, it turns on the spot (45 °/s for 0.6 s), otherwise it walks straight ahead (0.3 m/s).

### Micro Duck chirps at the ball

The level graph of the Micro Duck sample level (`micro_duck_level` in `demos.cpp`):

![Every 0.25 s: chirp if the ball is visible and BallSeen is still false; then BallSeen stores the visibility](../abbildungen/logik-ball.svg)

The boolean variable "BallSeen" ensures that the duck chirps only once when the ball comes into
view – not every 0.25 s for as long as it is visible. A comment box in the graph states explicitly
that this is a demo of the robot nodes and not something the robot does on its own.

---

© 2026 Christian Heinrich Hohlfeld · All rights reserved · [Imprint and rights](00-imprint.md)
