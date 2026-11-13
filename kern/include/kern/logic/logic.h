// Kern Logic: visual scripting in the engine, backed entirely by C++.
//
// There is no script language. A logic graph is data (nodes, links, literal
// values, variables, state machines); every node type is a C++ function in a
// registry (core flow / math / variables here, gameplay nodes in the game
// module). Graphs are checked, compiled to an index table and run by a small
// interpreter in the engine's fixed simulation steps, on the authority only
// (the standalone game or the network server). Their effects replicate like
// everything else the simulation does.
//
// Model (Unreal Blueprint semantics):
//   exec pins     white arrows: what runs next. Event nodes start a chain,
//                 action nodes continue it, flow nodes branch / loop / wait.
//   data pins     typed values. Inputs pull: pure nodes (math, getters) are
//                 evaluated on demand, outputs of action nodes keep the value
//                 they had when the node last ran. Unlinked inputs use the
//                 literal typed into the node.
//   variables     per graph instance, typed, with a default.
//   lists         a pin or a variable may hold a list of values of one type
//                 (Unreal's arrays). A list on a wire is a value like any
//                 other: what reads it has the list as it was. The steps that
//                 change a list (Add, Insert, Set Item, Remove, Clear) change
//                 a list variable; For Each goes through a list as it was
//                 when the loop started.
//   enums         a Blueprint may declare enumerations: a type of its own with named values
//                 (Mode: Closed, Opening, Open). A value of it is one of them - held as
//                 its place in the declaration, the first being 0.
//   structs       a Blueprint may declare structures: a type of its own with named, typed fields
//                 (Weapon: Name, Damage). A value of it holds a value for each field; a field
//                 is a value type, an enum, a struct declared above it, or a list of one.
//   state machines  states and transitions in the style of the
//                 digitalisierungsplanung.de state editor: a transition fires
//                 on an event, a timer, a condition variable or automatically;
//                 entering / leaving a state raises events the graph handles.
//   latent nodes  Delay, Tween, Retriggerable Delay suspend the chain and
//                 resume it from the tick, like Blueprint latent actions.
//   functions     named, with typed inputs and outputs. The body starts at the
//                 function's Function Entry node and ends at a Return node; it
//                 is called from a graph (Call Function, or as a pure node)
//                 and from C++ (Instance::call). A call runs to its end at
//                 once: after a latent node the caller is already gone.
//
// A level has one level graph; any actor can carry its own graph (its Self).
#pragma once

#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "kern/core/base.h"
#include "kern/core/json.h"
#include "kern/math/math.h"

namespace kern::logic {

using ActorRef = u32;  // game actor id (0 = none)

/// What a pin carries: exec, a value, or a list of values of one type (after the value types, in
/// their order). `enumeration`: a value of an enum the Blueprint declares, `structure`: a value of
/// one of its structs - which one says the name beside the type (PinDef::of, Variable::of ...).
enum class PinType : u8 {
  exec, boolean, integer, number, vector, text, actor, enumeration, structure,
  boolean_list, integer_list, number_list, vector_list, text_list, actor_list, enumeration_list, structure_list, count
};
constexpr u8 kValueTypes = 8;  // boolean ... structure
/// A list type, the type of a list's items (of a value: the type itself), and the list of a type.
constexpr bool is_list(PinType t) { return t >= PinType::boolean_list && t < PinType::count; }
constexpr PinType element_of(PinType t) { return is_list(t) ? static_cast<PinType>(static_cast<u8>(t) - kValueTypes) : t; }
constexpr PinType list_of(PinType t) { return t >= PinType::boolean && t <= PinType::structure ? static_cast<PinType>(static_cast<u8>(t) + kValueTypes) : t; }
/// The names of the pin types, in the order of PinType.
inline constexpr std::string_view kPinTypeNames[] = {"exec",      "bool",     "int",        "float",       "vector",    "text",      "actor",      "enum",      "struct",
                                                     "bool_list", "int_list", "float_list", "vector_list", "text_list", "actor_list", "enum_list", "struct_list"};
static_assert(sizeof kPinTypeNames / sizeof kPinTypeNames[0] == static_cast<std::size_t>(PinType::count));
/// "bool", "int", "float", "vector", "text", "actor"; a list: "float_list" ...
const char* pin_type_name(PinType t);
/// The type with that name (also "number", "double", "boolean", "integer", "string").
constexpr bool pin_type_from_name(std::string_view name, PinType& out) {
  for (u32 i = 0; i < static_cast<u32>(PinType::count); ++i)
    if (name == kPinTypeNames[i]) {
      out = static_cast<PinType>(i);
      return true;
    }
  if (name == "number" || name == "double") out = PinType::number;
  else if (name == "boolean") out = PinType::boolean;
  else if (name == "integer") out = PinType::integer;
  else if (name == "string") out = PinType::text;
  else return false;
  return true;
}
/// The type a generic node's setting "Type" names (Select, Reroute): "float" when it names none.
constexpr PinType pin_type_setting(std::string_view name) {
  PinType t = PinType::number;
  return pin_type_from_name(name, t) && t != PinType::exec ? t : PinType::number;
}
/// The type of a list node's items its setting "Type" names: a value type, "float" when it names none.
constexpr PinType item_type_setting(std::string_view name) {
  const PinType t = pin_type_setting(name);
  return is_list(t) ? PinType::number : t;
}
/// Data that can flow from `from` into an input of type `to` (autocasts).
bool convertible(PinType from, PinType to);

/// A number as a whole number (Float to Int, on a wire or in a node): toward zero. Not a number
/// or endless: 0. Beyond what Int holds: its nearest end. Defined for every number - the same in
/// the interpreter and in a Blueprint's C++. Int's ends are 2^63 - 1 and its negative: the same
/// distance from 0, so every Int can be written down in C++ (-2^63 is no literal there).
inline i64 whole(f64 v) {
  if (!std::isfinite(v)) return 0;
  if (v >= 9223372036854775808.0) return std::numeric_limits<i64>::max();
  if (v <= -9223372036854775808.0) return -std::numeric_limits<i64>::max();
  return static_cast<i64>(v);
}

/// A whole number as a Blueprint holds it. Every number of a Blueprint is a double underneath
/// (the interpreter's variables, the JIT): whole numbers are exact up to 2^53, a larger one is
/// the nearest a double holds, and none is past Int's ends. Every Int there is - a value on a
/// pin, a variable of a Blueprint's class - is such a number, so every tier has the same.
constexpr i64 normal(i64 v) {
  constexpr i64 exact = i64{1} << 53;
  if (v >= -exact && v <= exact) return v;
  const f64 held = static_cast<f64>(v);  // (the nearest a double holds)
  if (held >= 9223372036854775808.0) return std::numeric_limits<i64>::max();
  if (held <= -9223372036854775808.0) return -std::numeric_limits<i64>::max();
  return static_cast<i64>(held);
}
/// Int + Int (Add to Variable): added as numbers - defined for every value, where the
/// arithmetic of i64 overflows.
inline i64 whole_add(i64 a, i64 b) {
  constexpr i64 half = i64{1} << 52;
  if (a > -half && a < half && b > -half && b < half) return a + b;  // (the same, without the detour)
  return whole(static_cast<f64>(a) + static_cast<f64>(b));
}

/// An actor on a pin (`Actor` in a Blueprint's C++): its id, 0 = none.
struct ActorHandle {
  ActorRef id = 0;
  bool operator==(const ActorHandle&) const = default;
};

/// The items of a list value: the vector of the list's type holds them (the others are empty).
using ListItems = std::tuple<std::vector<bool>, std::vector<i64>, std::vector<f64>, std::vector<Vec3>, std::vector<std::string>, std::vector<ActorHandle>>;
/// The list type whose items are T (bool, i64, f64, Vec3, std::string, ActorHandle).
template <class T> constexpr PinType list_type_of() = delete;
template <> constexpr PinType list_type_of<bool>() { return PinType::boolean_list; }
template <> constexpr PinType list_type_of<i64>() { return PinType::integer_list; }
template <> constexpr PinType list_type_of<f64>() { return PinType::number_list; }
template <> constexpr PinType list_type_of<Vec3>() { return PinType::vector_list; }
template <> constexpr PinType list_type_of<std::string>() { return PinType::text_list; }
template <> constexpr PinType list_type_of<ActorHandle>() { return PinType::actor_list; }

/// A value on a data pin.
struct Value {
  PinType type = PinType::number;
  bool b = false;
  i64 i = 0;
  f64 n = 0;
  Vec3 v;
  std::string s;
  ActorRef a = 0;
  /// A list's items (none: an empty list). They are shared: a list on a wire is not copied, and
  /// whoever changes one (edit) changes its own.
  std::shared_ptr<const ListItems> list;
  /// A struct's fields, in the order its declaration has them; of a list of structs: the structs.
  /// Shared like a list's items. A struct that holds none (Value::zero) has every field at its
  /// declared default - what a field is, is read through the declaration (logic::field_of).
  std::shared_ptr<const std::vector<Value>> rows;

  static Value boolean(bool x) { Value r; r.type = PinType::boolean; r.b = x; return r; }
  static Value integer(i64 x) { Value r; r.type = PinType::integer; r.i = normal(x); return r; }
  static Value number(f64 x) { Value r; r.type = PinType::number; r.n = x; return r; }
  static Value vector(Vec3 x) { Value r; r.type = PinType::vector; r.v = x; return r; }
  static Value text(std::string x) { Value r; r.type = PinType::text; r.s = std::move(x); return r; }
  static Value actor(ActorRef x) { Value r; r.type = PinType::actor; r.a = x; return r; }
  /// A value of an enum: its place among the enum's values (the first is 0).
  static Value enumeration(i64 index) { Value r; r.type = PinType::enumeration; r.i = index < 0 ? 0 : index; return r; }
  /// A list of values of an enum.
  static Value enumerations(std::vector<i64> indices) {
    for (i64& index : indices) index = index < 0 ? 0 : index;
    Value r = of(std::move(indices));
    r.type = PinType::enumeration_list;
    return r;
  }
  /// A value of a struct: its fields' values, in the order of the declaration.
  static Value structure(std::vector<Value> fields) {
    Value r;
    r.type = PinType::structure;
    r.rows = std::make_shared<std::vector<Value>>(std::move(fields));
    return r;
  }
  /// A list of structs.
  static Value structures(std::vector<Value> items) {
    Value r;
    r.type = PinType::structure_list;
    if (!items.empty()) r.rows = std::make_shared<std::vector<Value>>(std::move(items));
    return r;
  }
  static Value zero(PinType t);
  /// A list of these items (T: bool, i64, f64, Vec3, std::string or ActorHandle; Value: structs).
  /// Whole numbers are held as every Int is (logic::normal).
  template <class T> static Value of(std::vector<T> items) {
    if constexpr (std::is_same_v<T, Value>) {
      return structures(std::move(items));
    } else {
      Value r;
      r.type = list_type_of<T>();
      if constexpr (std::is_same_v<T, i64>)
        for (std::size_t k = 0; k < items.size(); ++k) items[k] = normal(items[k]);
      if (!items.empty()) {
        auto held = std::make_shared<ListItems>();
        std::get<std::vector<T>>(*held) = std::move(items);
        r.list = std::move(held);
      }
      return r;
    }
  }
  /// The items of a list of T - none if this is no such list.
  template <class T> const std::vector<T>& items() const {
    static const std::vector<T> none;
    if constexpr (std::is_same_v<T, Value>) return rows && type == PinType::structure_list ? *rows : none;
    else return list && holds<T>() ? std::get<std::vector<T>>(*list) : none;
  }
  /// Is this a list whose items are held as T? (A list of enum values holds them as i64, a list
  /// of structs as Value.)
  template <class T> bool holds() const {
    if constexpr (std::is_same_v<T, Value>) return type == PinType::structure_list;
    else return type == list_type_of<T>() || (std::is_same_v<T, i64> && type == PinType::enumeration_list);
  }
  /// The items of a list of T, to change them: this value's own (it becomes that list if it is
  /// none).
  template <class T> std::vector<T>& edit() {
    if constexpr (std::is_same_v<T, Value>) {
      if (type != PinType::structure_list) {
        type = PinType::structure_list;
        rows.reset();
      }
      return own_rows();
    } else {
      if (!holds<T>()) {
        type = list_type_of<T>();
        list.reset();
      }
      if (!list) list = std::make_shared<ListItems>();
      else if (list.use_count() > 1) list = std::make_shared<ListItems>(*list);  // (someone else reads these)
      return std::get<std::vector<T>>(const_cast<ListItems&>(*list));  // (made as ListItems, above and in of())
    }
  }
  /// The field of a struct at `index`, as the value holds it (null: it holds none there - the
  /// field is at its declared default).
  const Value* field(std::size_t index) const { return type == PinType::structure && rows && index < rows->size() ? &(*rows)[index] : nullptr; }
  /// How many fields a struct's value holds.
  std::size_t fields() const { return type == PinType::structure && rows ? rows->size() : 0; }
  /// The rows (a struct's fields, the structs of a list), to change them: this value's own.
  std::vector<Value>& own_rows() {
    if (!rows) rows = std::make_shared<std::vector<Value>>();
    else if (rows.use_count() > 1) rows = std::make_shared<std::vector<Value>>(*rows);  // (someone else reads these)
    return const_cast<std::vector<Value>&>(*rows);  // (made as a vector, here and in structure())
  }
  /// How many items a list has (no list: 0).
  std::size_t size() const;
  /// A list's item as a value (beyond its end, or no list: the zero of the item's type).
  Value item(std::size_t index) const;

  bool as_bool() const;
  i64 as_int() const;
  f64 as_number() const;
  Vec3 as_vector() const;
  std::string as_text() const;
  ActorRef as_actor() const;
  Value converted(PinType to) const;
  json::Value to_json() const;
  static Value from_json(const json::Value& j, PinType type);
  /// The same where the value may be a struct's (or hold one): `of` names the struct or the enum
  /// in the Blueprint `g`. A struct is an object of its fields by their names - {"Name": "Knife",
  /// "Damage": 25} -, written with every field; read, a field that is missing has its default.
  json::Value to_json(const struct Graph& g, std::string_view of) const;
  static Value from_json(const json::Value& j, PinType type, const struct Graph& g, std::string_view of);
  bool operator==(const Value& o) const;
};

// ---------------------------------------------------------------------------
// graph data

struct Node {
  u32 id = 0;
  std::string type;                                   // registry key, e.g. "flow.branch"
  f32 x = 0, y = 0;                                   // canvas position (px)
  std::vector<std::pair<std::string, json::Value>> values;  // literals of unlinked inputs, by pin id
  std::vector<std::pair<std::string, std::string>> params;  // node settings (variable name, event name, key...)
  std::string comment;
  const json::Value* value(std::string_view pin) const;
  std::string param(std::string_view key, std::string_view fallback = {}) const;
  void set_value(std::string_view pin, json::Value v);
  void set_param(std::string_view key, std::string v);
};

struct Link {
  u32 from = 0;          // node id
  std::string out;       // output pin id
  u32 to = 0;
  std::string in;        // input pin id
  bool operator==(const Link& o) const { return from == o.from && out == o.out && to == o.to && in == o.in; }
};

struct Variable {
  std::string name;
  PinType type = PinType::number;
  json::Value value;     // default
  std::string help;
  std::string of = {};   // a value of an enum or a struct (or a list of them): its name
};

/// An enumeration a Blueprint declares: a type with named values. A value of it is held as its
/// place among them (the first is 0); in a Blueprint's C++ it is KERN_ENUM(Mode, Closed, Open).
struct EnumType {
  std::string name;
  std::vector<std::string> values;
  std::string help;
  /// A type of the project: every Blueprint of the level that has a project type of this name has
  /// it the same (the editor keeps them so; a level in which they differ does not check). Such a
  /// value means the same in every Blueprint that takes or gives it.
  bool project = false;
};

/// A type as pins, variables and settings name it: the pin type, and for an enum or a struct
/// which one.
struct TypeRef {
  PinType type = PinType::number;
  std::string of;
  bool operator==(const TypeRef&) const = default;
};
/// Can what comes out of a pin of type `from` go into one of type `to`? (convertible, and two
/// enums are the same enum, two structs the same struct.)
bool fits(const TypeRef& from, const TypeRef& to);

/// A structure a Blueprint declares: a type with named fields, each of a type and with a default.
/// A field is a value type, an enum of the Blueprint, a struct declared above this one, or a list
/// of one of these - no struct contains itself. In a Blueprint's C++ it is
/// KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };
struct StructField {
  std::string name;
  TypeRef type;
  json::Value value;     // default
};
struct StructType {
  static constexpr std::size_t kMaxFields = 96;
  std::string name;
  std::vector<StructField> fields;
  std::string help;
  bool project = false;  // a type of the project (see EnumType::project)
  /// The place of the field called `name`, or -1.
  int field(std::string_view name) const;
};
/// The same declaration: the name, and the values / the fields with their types and defaults, in
/// their order (the help text may differ).
bool same_type(const EnumType& a, const EnumType& b);
bool same_type(const StructType& a, const StructType& b);

/// A state machine in the style of the digitalisierungsplanung.de editor.
enum class TriggerKind : u8 { event, timer, condition, automatic };
const char* trigger_kind_name(TriggerKind k);

struct State {
  u32 id = 0;
  std::string name;
  f32 x = 0, y = 0;
  bool initial = false;
};

struct Transition {
  u32 id = 0;
  u32 from = 0, to = 0;  // state ids (from 0 = from any state)
  TriggerKind trigger = TriggerKind::event;
  std::string event;     // event: custom event name that switches
  f32 seconds = 1;       // timer: seconds in the from state
  std::string variable;  // condition: boolean variable that must be true
  std::string label;
};

struct StateMachine {
  u32 id = 0;
  std::string name = "State Machine";
  std::vector<State> states;
  std::vector<Transition> transitions;
  const State* find(u32 state) const;
  const State* find(std::string_view name) const;
};

/// A function of a graph (Blueprint function). Pins are named; a name is the pin's id on the
/// function's nodes (Function Entry: the inputs as outputs; Return: the outputs as inputs; the
/// call nodes: both).
struct FunctionPin {
  std::string name;
  PinType type = PinType::number;
  json::Value value;     // default: an input nobody wired, an output no Return node set
  std::string of = {};   // a value of an enum or a struct (or a list of them): its name
  /// An input that is a reference: the call gives no value but names a place - a variable, a
  /// field or a list entry in one (logic::ref_key) -, and the function works on that place where
  /// it stands. It changes it (Car& car), or - `constant` - only reads it (const Car& car):
  /// nothing is copied in either case. In the function the input is named like a local variable
  /// (the settings "variable" and "scope" of the variable nodes); the Function Entry has no pin
  /// for it. An input by value is the value it had when the call was made.
  bool ref = false;
  bool constant = false;
};

struct Function {
  std::string name;
  std::vector<FunctionPin> inputs, outputs;
  /// Its local variables: each call starts with their defaults, and they are gone when the call
  /// is over (after a Return, at the end of its path, at a wait). In the Blueprint's C++ they are
  /// the locals at the top of the function's body. The variable nodes name one with their
  /// setting "scope" (the function); two functions may each have a local of one name.
  std::vector<FunctionPin> locals;
  bool pure = false;     // offered as a pure node (no exec pins): runs whenever an output is read
  /// A function of another Blueprint: this one says what it looks like (name, inputs, outputs)
  /// and calls it on an actor (Call on Actor) - it has no body here. In the Blueprint's C++:
  /// KERN_EXTERN FnResult Fn(Float x = 0.0);
  bool external = false;
  std::string help;
  const FunctionPin* input(std::string_view name) const;
  const FunctionPin* output(std::string_view name) const;
  const FunctionPin* local(std::string_view name) const;
  FunctionPin* local(std::string_view name);
};

/// A comment box (Blueprint "C"): a titled frame behind a group of nodes; moving it moves them.
struct CommentBox {
  u32 id = 0;
  f32 x = 0, y = 0, w = 400, h = 200;  // canvas px
  std::string text = "Comment";
  u32 color = 0;                       // 0xRRGGBB, 0 = the default grey
};

struct Graph {
  std::vector<Node> nodes;
  std::vector<Link> links;
  std::vector<Variable> variables;
  std::vector<StateMachine> machines;
  std::vector<Function> functions;
  std::vector<EnumType> enums;         // the types it declares
  std::vector<StructType> structs;     // (a struct after the structs its fields are of)
  std::vector<CommentBox> comments;    // editor only: no effect on what runs
  u32 next_id = 1;
  bool empty() const { return nodes.empty() && variables.empty() && machines.empty() && functions.empty() && enums.empty() && structs.empty() && comments.empty(); }
  const EnumType* enumeration(std::string_view name) const;
  EnumType* enumeration(std::string_view name);
  const StructType* structure(std::string_view name) const;
  StructType* structure(std::string_view name);
  Node* find(u32 id);
  const Node* find(u32 id) const;
  const Variable* variable(std::string_view name) const;
  Variable* variable(std::string_view name);
  /// The variable a node's settings name: a variable of the Blueprint, or - with the setting
  /// "scope" - a local variable of that function. `out` holds it (a local as a Variable).
  bool variable_of(const Node& n, Variable& out) const;
  /// What a node's variable is, where its settings name one under `prefix` ("": the node's own
  /// - "variable", "scope", "path" -; "<input>.": the place a call gives the reference input of
  /// that name, logic::ref_key): a variable of the Blueprint, a local variable of the function
  /// the node stands in, or one of that function's inputs. `out` holds it.
  enum class Root : u8 { none, variable, local, reference, constant_reference, input };
  Root root_of(const Node& n, Variable& out, std::string_view prefix = {}) const;
  /// Can what the node names there be changed (a variable, a local variable, a reference input
  /// that is not const)?
  static bool writable(Root r) { return r == Root::variable || r == Root::local || r == Root::reference; }
  /// The place a node's settings name: its variable (variable_of) and from there the path of the
  /// setting "path" - fields and list entries (logic::path_steps). `out` holds what is there, as
  /// a Variable: its type, its default (a field's declared one, the zero of a list's entry), and
  /// as its name the place as C++ writes it ("Cars[].Speed").
  /// A node without a path names its variable. False: no such variable, or the path leads
  /// through something the variable has not - `why` then says what.
  bool place_of(const Node& n, Variable& out, std::string* why = nullptr, std::string_view prefix = {}) const;
  /// A node's path as its steps (logic::path_steps), and for every step that is a field the
  /// struct that has it (its name) - empty for a list entry, and from where the path stops
  /// leading through the variable's type. What renaming a field has to go by. False: the node
  /// names no variable, or its path is none.
  bool path_of(const Node& n, std::vector<std::string>& steps, std::vector<std::string>& owners, std::string_view prefix = {}) const;
  /// The function in whose body a node stands: the one whose Function Entry leads to it along
  /// exec wires - or to a step that reads it. Empty: none does (an event's path, nothing), or
  /// more than one. Where the places a call names are looked up (the call's setting "scope").
  std::string body_of(u32 node, const class Library& lib) const;
  StateMachine* machine(u32 id);
  const StateMachine* machine(std::string_view name) const;
  const Function* function(std::string_view name) const;
  Function* function(std::string_view name);
  bool operator==(const Graph& o) const;  // via JSON
};

/// The steps of a path (the setting "path" of a node that names a place): ".Wheels[].Slip" is
/// the field Wheels, a list entry (an empty step), the field Slip. False: no path.
bool path_steps(std::string_view path, std::vector<std::string>& steps);
/// The path its steps are (path_steps the other way round).
std::string path_text(const std::vector<std::string>& steps);
/// How many list entries a path goes through: the node has an index input for each (path_pin).
std::size_t path_indices(std::string_view path);
/// The id of the index input of a path's `k`-th list entry: "path0", "path1" ...
std::string path_pin(std::size_t k);
/// Is `id` such an input's?
bool is_path_pin(std::string_view id);
/// How a call names the place it gives a reference input (FunctionPin::ref) of the function it
/// calls: its settings "<input>.variable" and "<input>.path" - a variable or a local variable of
/// the function the call stands in (the call's setting "scope"), or one of that function's
/// inputs, and the path from it - and, for the path's list entries, its index inputs
/// "<input>.path0", "<input>.path1" ... `what`: "variable", "path", or such an index input's id.
std::string ref_key(std::string_view input, std::string_view what);

json::Value graph_to_json(const Graph& g);
bool graph_from_json(const json::Value& j, Graph& out, std::string* error = nullptr);

/// The value something of type `t` has when nothing else is said: the zero of a value type, the
/// empty list - and of a struct every field at its declared default.
Value zero_of(const TypeRef& t, const Graph& g);
/// A struct's value with every field at its declared default.
Value default_of(const StructType& s, const Graph& g);
/// The field at `index` of a value of the struct `s`: what the value holds there, or the field's
/// declared default.
Value field_of(const Value& v, const StructType& s, std::size_t index, const Graph& g);
/// Is `v` a value of type `t` as a Blueprint holds it: of that pin type; an enum's value one of
/// its values; a struct with a value of its type for every field; a list of such items?
bool is_value_of(const Value& v, const TypeRef& t, const Graph& g);
/// Makes a struct's value hold a value for every field (one that holds none has them at their
/// declared defaults): what changing one of its fields in place starts from.
void hold_fields(Value& v, const StructType& s, const Graph& g);
/// What comes from outside (Instance::set, the arguments of Instance::call) as a value of type
/// `t`: converted as a wire converts; an enum's value that is none of the enum's is the first; and
/// what is no struct of that type is the struct's default.
Value from_outside(const Value& v, const TypeRef& t, const Graph& g);
/// The structs `s` holds, directly or in its fields' structs (their names) - itself not among them.
std::vector<std::string> structs_in(const StructType& s, const Graph& g);

// ---------------------------------------------------------------------------
// formulas

/// A formula over named numbers, e.g. "max(9000, 16000 - level * 1300)" - what the Formula node
/// computes. Parsed once, evaluated on f64 exactly like the maths nodes (the same functions, the
/// same results in every tier and on every platform).
///
///   numbers      1, 2.5, .5, 1e3            names   letters, digits and _ (each is an input)
///   operators    + - * / %  ^ (power)       unary - and !
///                < <= > >= == !=            1 when true, else 0
///                && ||                      a ? b : c
///   functions    min max clamp(x, lo, hi) abs sign floor ceil round trunc sqrt pow(x, y) exp log
///                sin cos tan asin acos atan atan2(y, x) - in radians; rad(degrees) deg(radians)
///                lerp(a, b, t)
///   constants    pi
/// Dividing by 0 gives 0 (as the Divide node); the root and the logarithm of a negative number
/// are those of 0 and of nothing (0).
class Formula {
 public:
  static constexpr std::size_t kMaxInputs = 16;
  explicit Formula(std::string_view text);
  bool ok() const { return error_.empty(); }
  /// Why it could not be parsed (empty when it could).
  const std::string& error() const { return error_; }
  /// The names the formula reads, in the order they first appear.
  const std::vector<std::string>& inputs() const { return inputs_; }
  /// Its value for these inputs, in the order of inputs() (missing ones are 0); 0 when !ok().
  f64 evaluate(const f64* values, std::size_t count) const;
  /// The parsed formula for a text (parsed once, kept).
  static const Formula& cached(const std::string& text);
  /// The formula written the one canonical way (single spaces, no brackets that change nothing,
  /// / % ^ as operators): what the C++ notation reads a formula back as. Empty when !ok().
  std::string text() const;
  /// The formula as a C++ expression over its inputs as doubles, with the same value: functions
  /// and pi are those of kern::math, / % ^ are its calls div, mod and pow, numbers are double
  /// literals. Read back by from_cpp (the text without "kern::math::" is a formula again).
  std::string cpp() const;

 private:
  friend struct FormulaParser;
  struct Op { u8 code = 0; u8 args = 0; u16 index = 0; f64 number = 0; };
  struct Term { u8 code = 0; u16 index = 0; int a = -1, b = -1, c = -1; f64 number = 0; };  // the formula as a tree
  std::string print(int term, int outer, bool cpp) const;
  std::vector<Term> tree_;
  int root_ = -1;
  std::vector<Op> code_;
  std::vector<std::string> inputs_;
  std::string error_;
};

// ---------------------------------------------------------------------------
// node library

struct PinDef {
  std::string id, label;
  PinType type = PinType::exec;
  json::Value def;       // default literal (data inputs)
  bool hidden = false;   // not drawn (advanced)
  std::string of = {};   // a value of an enum or a struct (or a list of them): its name
};

struct ParamDef {
  std::string key, label;
  /// type: the setting names a type ("float", "text_list" ...) - the "Type" of a generic node
  /// (Select, Reroute, the list nodes). In a Blueprint's C++ it is that type: logic_select<Float>.
  /// field: the setting names a field of the struct variable the node's setting "variable" names
  /// (Set Field). In a Blueprint's C++ it is that member: Held.Damage = 2.0.
  /// scope: the function whose local variable the setting "variable" names; empty for a variable
  /// of the Blueprint. A Blueprint's C++ does not write it: the body the text stands in says it.
  /// path: from the variable the setting "variable" names to the place the node means - fields
  /// and list entries, ".Wheels[].Slip" (empty: the variable itself). Every "[]" has an index
  /// input of the node (path0, path1 ...). In a Blueprint's C++ it is that place:
  /// Cars[i].Wheels[j].Slip.
  enum class Kind : u8 { text, choice, variable, event, machine, state, key, sound, tag, integer, function, type, field, scope, path } kind = Kind::text;
  std::vector<std::string> choices;  // value, label pairs flattened for Kind::choice
  std::string def;
  bool with_lists = false;  // Kind::type: the list types are among them (not for the type of a list's items)
  bool enums_only = false;  // Kind::type: an enum of the Blueprint and nothing else (the enum nodes' "Enum")
  bool structs_only = false;  // Kind::type: a struct of the Blueprint and nothing else (the struct nodes' "Struct")
};
/// The types a Kind::type setting can name in the Blueprint `g` - its enums and structs among
/// them -, in the order they are offered.
std::vector<TypeRef> type_choices(const ParamDef& pd, const Graph& g);
/// The text that names a type in a setting: "float", "text_list"; an enum or a struct by its own
/// name ("Mode"), a list of its values as "Mode_list".
std::string type_text(const TypeRef& t);
/// The type a text names in the Blueprint `g` (false: none).
bool type_from_text(std::string_view text, const Graph& g, TypeRef& out);
/// A type as people read it: "Float", "Integer", "Text List", "Mode", "Mode List".
std::string type_label(const TypeRef& t);
inline std::string type_label(PinType t) { return type_label(TypeRef{t, {}}); }

class Context;

/// event: starts a chain. action: one step that always goes on (one exec output) or ends the
/// chain (none). pure: a value, no exec pins. flow: decides where the chain goes on - several exec
/// outputs, or one it may not take (Do Once, Gate).
enum class NodeKind : u8 { event, action, pure, flow };

struct NodeDef {
  std::string type, label, category, help;
  NodeKind kind = NodeKind::action;
  std::string event;     // events: the Event::type it handles (e.g. "logic.tick", "game.overlap")
  bool latent = false;
  bool loop = false;     // a loop: exec output 0 is its body (run again and again), output 1 follows it
  bool memory = false;   // keeps something between runs (a loop's index, Do Once, a Delay's wait, Random's
                         // sequence): in C++ the node has a member. A value with memory is new on every read.
  bool first_entry_only = false;  // only its first exec input can lead on to an exec output; the others
                         // (Reset, Open, Close, Stop) change what the node keeps, and the path ends there
  int now_outputs = 0;   // latent: its first exec outputs that run with the call itself, not after a wait
                         // (Tween's Update runs with Play); the others always run later
  bool native = false;   // a Blueprint's C++ has it as a function (blueprint.h): a C++ compiler runs a
                         // Blueprint whose nodes all have one (native_ready)
  bool raises = false;   // a step that may run handlers of Blueprints before it goes on (Call Event, Call on
                         // Actor, a gameplay step that raises an event at once): whatever a Blueprint holds may
                         // have changed after it - no call holds a place inside a list across it
  std::vector<PinDef> inputs, outputs;  // exec pins included, in drawing order
  std::vector<ParamDef> params;
  std::string keywords;
  u32 color = 0;         // header colour 0xRRGGBB (0 = by kind)
  /// Pins that depend on params / the graph (variables, Sequence outputs, Switch cases...).
  std::function<void(const NodeDef& def, const Node& node, const Graph& graph, std::vector<PinDef>& in, std::vector<PinDef>& out)> shape;
  /// Title that depends on params ("Set Score", "On Key F").
  std::function<std::string(const NodeDef& def, const Node& node)> title;
  /// What is wrong with this node's settings (a formula that does not parse...), or empty: check() reports it.
  std::function<std::string(const Node& node, const Graph& graph)> problem;
  /// Events: does this node handle `e`? (default: e.key is empty, or equals the node's first param / is "any").
  std::function<bool(const Node& node, const struct Event& e)> accepts;
  /// Runs the node. Returns the index of the exec output to continue with
  /// (among exec outputs), or kStop / kSuspend. Pure nodes write outputs and return kStop.
  std::function<int(Context& c)> run;
};

constexpr int kStop = -1;      // the chain ends here
constexpr int kSuspend = -2;   // latent: resumed later from the tick

class Library {
 public:
  void add(NodeDef def);
  const NodeDef* find(std::string_view type) const;
  /// A node of the library, to complete it: a module that knows what the node does binds that
  /// (NodeDef::run) to a node another one has described (the gameplay nodes). Null: no such node.
  NodeDef* edit(std::string_view type);
  const std::vector<std::unique_ptr<NodeDef>>& all() const { return defs_; }
  /// Resolved pins of a node (static or shaped).
  void pins(const Node& node, const Graph& graph, std::vector<PinDef>& in, std::vector<PinDef>& out) const;
  std::string title(const Node& node) const;
  /// Built-in nodes: events Begin Play / Tick / Custom Event / state machine
  /// events, flow, variables, math, logic, vectors, text, debugging.
  static Library& core();

 private:
  std::vector<std::unique_ptr<NodeDef>> defs_;
  std::unordered_map<std::string, std::size_t> index_;
};

/// The node library a graph uses: core + whatever the game registered.
Library& library();

struct Issue {
  u32 node = 0;          // 0 = the graph itself
  bool error = true;
  std::string text;
};
/// Unknown node types, links between missing pins or incompatible types,
/// unknown variables / events / states / functions, functions without exactly one entry,
/// unlinked required inputs, cycles of pure nodes.
std::vector<Issue> check(const Graph& g, const Library& lib = library());

// ---------------------------------------------------------------------------
// running graphs

/// What logic can do to the world. The game implements it (GameWorld), tests can fake it.
class Host {
 public:
  virtual ~Host() = default;
  virtual f64 time() const = 0;                          // simulation seconds
  virtual void print(const std::string& text, bool screen) = 0;
  virtual void error(const std::string& text) = 0;
  /// Random numbers: deterministic per world (replays and the network agree).
  virtual u32 random() = 0;
  /// The game world behind this host (gameplay nodes), or null.
  virtual void* game() { return nullptr; }
  /// A function of the engine, by the name of its node - what the engine's nodes do in every tier
  /// (kern/logic/engine_api.h). `in`: the node's inputs in their order; `out`: its outputs, given as
  /// their zeros. False: this host has no such function (the outputs stay as they are).
  virtual bool engine(std::string_view function, const std::vector<Value>& in, std::vector<Value>& out);
  /// Native code: the host compiles the WebAssembly module of an instance (asynchronously)
  /// and calls jit_ready(id, function) when it is linked. False: no compiler here (interpreter).
  virtual bool jit_compile(const std::vector<u8>& module, i32 id);
  virtual void jit_release(i32 function);
};

/// The platform's compiler for generated WebAssembly (the browser's, through kern.mjs).
/// False where there is none (native builds): graphs then run in the interpreter or as
/// C++ compiled into the binary.
bool platform_jit_compile(const std::vector<u8>& module, i32 id);
void platform_jit_release(i32 function);

// ---------------------------------------------------------------------------
// The C++ notation (docs/blueprint_cpp.md)
//
// A Blueprint and its C++ text are one thing in two notations. The text is a fixed subset of
// C++: one struct, the variables as members, the functions and events as methods, every node one
// statement or one expression. to_cpp writes it the one canonical way; from_cpp reads it back as
// the same Blueprint and refuses, with the place and the reason, whatever is not in the subset.

/// The Blueprint as C++. False: the graph cannot be written (unknown node types, names that are
/// no C++ names - check() reports the same) and `error` says why.
/// `layout` false: the code alone, without the KERN_LAYOUT block that says where the nodes are in
/// the graph view (what Studio shows; apply_cpp reads it back).
bool to_cpp(const Graph& g, const std::string& name, std::string& out, std::string* error = nullptr, const Library& lib = library(), bool layout = true);
/// The Blueprint a C++ text is. False: `error` says what is not a Blueprint, with line and column.
bool from_cpp(std::string_view text, Graph& out, std::string* name = nullptr, std::string* error = nullptr, const Library& lib = library());
/// The Blueprint a C++ text is, as a change to `before`. A text without its KERN_LAYOUT block
/// keeps, for every node it still has, the id, the place and the comment the node has in
/// `before`; a new node gets a new id and a place next to the node before it; the comment boxes
/// stay. (With a KERN_LAYOUT block the text says all of that itself, as for from_cpp.)
bool apply_cpp(std::string_view text, const Graph& before, Graph& out, std::string* error = nullptr, const Library& lib = library());
/// The one canonical form of a graph - what reading its own C++ gives: the nodes in the order
/// the text names them, only the values and settings that differ from their defaults, formulas
/// in their one spelling. It is the same Blueprint (same_blueprint) and runs exactly as `g`.
bool canonical(const Graph& g, Graph& out, std::string* error = nullptr, const Library& lib = library());
/// The same Blueprint whatever the order its nodes and wires are stored in: variables,
/// functions, state machines, comment boxes, and node by node (by id) type, place, comment,
/// settings and typed-in values as they take effect, and the same wires.
bool same_blueprint(const Graph& a, const Graph& b, std::string* difference = nullptr, const Library& lib = library());
/// A name the notation can write as a C++ name: letters, digits and _, not starting with a
/// digit, no C++ keyword and none of the notation's own names (Float, loop, n12, go12, on12 ...).
bool cpp_name_ok(std::string_view name);
/// The class name for the Blueprint of `owner` (an actor's name, "Level"): the name as a C++
/// name ("Door 1" is Door1) that nothing in the class has - a function called like its class
/// would be its constructor.
std::string cpp_class_name(std::string_view owner, const Graph& g);
/// Why a graph cannot be written as C++ (empty: it can), and the node that is about (0: the
/// graph itself). check() reports the same as an error.
std::string cpp_problem(const Graph& g, u32* node = nullptr, const Library& lib = library());
/// Can a C++ compiler run this Blueprint's text as it stands (kern/logic/blueprint.h has a typed
/// function for each of its nodes, nothing is looked up at run time)? `why`: what is missing.
bool native_ready(const Graph& g, std::string* why = nullptr, const Library& lib = library());
/// Documents from before names had to be C++ names: renames the variables, functions and function
/// pins C++ cannot write ("Ball Seen" becomes BallSeen, "Text" TextValue), and every use with
/// them. Returns what was renamed (old, new); nothing for a graph whose names are fine.
std::vector<std::pair<std::string, std::string>> cpp_names(Graph& g, const Library& lib = library());

/// Generated code (WebAssembly from the JIT) works on this
/// layout: f64 slots for numeric variables and node state, an i32 continuation stack,
/// and per node a table of exec targets. Everything else calls back into the C++ nodes.
struct CodeLayout {
  static constexpr int kNodeSlots = 8;   // per node: 4 state + 4 data outputs
  static constexpr int kStack = 512;     // continuation entries (pc, entry): at least; an instance has as
                                         // many as its graph can ever need (Instance::stack_capacity)
  static constexpr int kStackMost = 1 << 15;  // a graph that could need more is not compiled by the JIT
};

/// Compiles an instance's graph to a WebAssembly module (the JIT's input).
std::vector<u8> compile_wasm(const class Instance& inst);
/// Content hash of a graph (what a Blueprint's C++ class registers under: places and comments
/// are left out, everything else changes it).
u64 graph_hash(const Graph& g);

/// A Blueprint compiled from its C++ text (kern/logic/blueprint.h, docs/blueprint_cpp.md), as the
/// engine reaches it: by name, at the boundary. Inside, the class calls its own functions and
/// reads its own members directly. logic::native_glue writes the class that implements this.
/// What a Blueprint's class has of the world it runs in. The engine sets it (NativeBlueprint::
/// attach); a class somebody just makes has none - nothing is printed, time stands still.
struct BlueprintWorld {
  class Host* host = nullptr;
  class Runtime* runtime = nullptr;
  class Instance* instance = nullptr;
  ActorRef self = 0;
  i32* counters = nullptr;  // the runtime's shared counters ([3] loop passes this step, [1] their limit)
};
class NativeBlueprint {
 public:
  virtual ~NativeBlueprint() = default;
  /// The world the class runs in: set once, before anything runs.
  virtual void attach(const BlueprintWorld& world) = 0;
  /// A chain starts at node `node` (its place in the graph's list): the handler of an event. The
  /// runtime has decided that it runs (Runtime::fire, the same for every tier) and has put the
  /// event's values on the node's outputs (Instance::output).
  virtual void run(int node, int entry) = 0;
  /// As Instance::call (false: no such function, `results` empty).
  virtual bool call(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results) = 0;
  virtual bool get(std::string_view variable, Value& out) const = 0;
  virtual bool set(std::string_view variable, const Value& value) = 0;
};
using NativeBlueprintFactory = std::unique_ptr<NativeBlueprint> (*)();
/// The engine goes into a Blueprint's class (Instance::call, a handler). What a class holds only
/// while it runs - the entries beside its lists (kern::List, nodes.h) - is that visit's: it stays
/// until the engine is out again, and is gone the next time it goes in. The engine may be in a
/// class already - a step of the class that makes the engine run a handler -: that is the same
/// visit.
namespace beside {
inline u64& visit() {
  static u64 count = 0;
  return count;
}
inline int& depth() {
  static int inside = 0;
  return inside;
}
struct Visit {
  Visit() {
    if (depth()++ == 0) ++visit();
  }
  ~Visit() { --depth(); }
  Visit(const Visit&) = delete;
  Visit& operator=(const Visit&) = delete;
};
}  // namespace beside
/// Code compiled into the binary: the class of the Blueprint with this content hash. A graph with
/// that hash then runs as this class (Runtime::native), not in the interpreter or the JIT.
void register_native_blueprint(u64 hash, NativeBlueprintFactory make);
/// The C++ that makes a Blueprint's class reachable by name and registers it under the graph's
/// hash: compiled after the Blueprint's text (to_cpp(g, name)) in the same source. For graphs that
/// are native_ready. It is glue for the engine, not part of the notation: nothing to edit there.
/// `space`: the namespace the class stands in (the glue is written into it too), empty: none.
bool native_glue(const Graph& g, const std::string& name, std::string& out, std::string* error = nullptr, const Library& lib = library(), std::string_view space = {});

struct NodeState {           // per node instance: Do Once, Gate, Flip Flop, loops, latent waits
  f64 time = 0, a = 0, b = 0;
  i64 counter = 0, index = 0, last = 0;
  bool flag = false, flag2 = false;
  Value value;
  std::vector<ActorRef> list;  // a loop over things: what it goes through (For Each Actor with Tag)
};

class Runtime;

/// One graph running for one owner (an actor, or the level).
class Instance {
 public:
  /// A place as the interpreter finds it: the variable that holds it and the steps from there -
  /// the indices as they were when the place was named. A reference input of a function is such
  /// a place for as long as its call runs (the place the caller named).
  struct Spot {
    std::string key;              // the variable's key (a local variable's: "<function>.<name>")
    int entry = -1, output = -1;  // or - an input by value, read only - that output of the function's entry node
    /// Or - the path led beside a list - the entry of its own that stands there, with the zero of
    /// the items' type. Whoever has the place has that entry: a function that was given it as a
    /// reference, and whatever that function gives it to.
    struct Beside {
      TypeRef type;
      Value value;
    };
    std::shared_ptr<Beside> beside;
    struct Step {
      std::string field;          // a field by its name; empty: a list entry
      i64 index = 0;
    };
    std::vector<Step> steps;
  };
  Instance(Runtime& rt, const Graph& graph, ActorRef self, std::string name);
  ~Instance();
  Instance(const Instance&) = delete;
  Instance& operator=(const Instance&) = delete;
  ActorRef self() const { return self_; }
  const std::string& name() const { return name_; }
  const Graph& graph() const { return graph_; }
  Value get(std::string_view variable) const;
  bool set(std::string_view variable, const Value& v);
  /// Current state of a machine ("" = none).
  std::string state(std::string_view machine) const;
  bool set_state(std::string_view machine, std::string_view state);
  bool disabled() const { return disabled_; }
  /// Calls a function of this graph and runs it to its end: `results` gets its outputs in the
  /// function's order (the defaults where no Return node set them). Missing arguments take the
  /// inputs' defaults. False: no such function or no entry node - `results` then holds the
  /// defaults. (A function cannot call itself - a rule, check() - but an event it raises may
  /// call it again while it runs: that is a call like any other, as in the Blueprint's C++.)
  /// A reference input (FunctionPin::ref) takes its argument as the value the function starts
  /// from; what the function made of it stands behind the outputs in `results`, for every
  /// reference input that is not const, in the inputs' order.
  bool call(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results);
  /// The same from a call node of this graph (Call Function, Function Value). The call belongs to
  /// the chain it is made in: it is no chain of its own for how deep events may nest - in the
  /// Blueprint's C++ it is a call like any other.
  bool call_inside(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results);
  /// The same for a function with reference inputs: `places[k]` is the place the call gives the
  /// function's k-th input, if that one is a reference (its entry in `args` is not read).
  bool call_inside(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results, const std::vector<Spot>& places);
  /// A variable's value, to change it in place (a list: Value::edit). Null for a number - those
  /// live in the slots, use set -, for a name that is none, and for a Blueprint that runs as its
  /// class (its variables are that class's members).
  Value* variable(std::string_view name);
  /// The same for the variable a node's settings name ("variable", and "scope" for a local
  /// variable of a function) - what the variable nodes read and change. A local variable is held
  /// here like any other; every call of its function starts it with its default.
  Value get(const Node& n) const;
  bool set(const Node& n, const Value& v);
  Value* variable(const Node& n);
  /// The place a node names under `prefix` ("": its own; "<input>.": what a call gives that
  /// reference input), with the path's list entries at `indices`. Naming it finds it: where the
  /// path leads beside a list, the place goes on in an entry of its own (Spot::beside) - what
  /// kern::List gives there in a Blueprint's class. Naming a place changes nothing. False: it
  /// names none.
  bool spot(const Node& n, std::string_view prefix, const std::vector<i64>& indices, Spot& out) const;
  /// What is at the place a node names (Graph::place_of) - its variable, or what the node's path
  /// leads to in it. `indices`: the path's list entries, in its order. Beside a list there is an
  /// entry with the defaults.
  Value place(const Node& n, const std::vector<i64>& indices) const;
  /// Changes what is at that place: `change` gets the value there and leaves what it is to be.
  /// Beside a list `change` gets an entry with the defaults, and nothing of it is kept - the same
  /// a Blueprint's class does there (kern::List). False: no such place.
  bool change(const Node& n, const std::vector<i64>& indices, const std::function<void(Value&)>& change);
  /// How many chains of this Blueprint are running, one inside the other (more than one: an event
  /// a running chain raised has reached a handler of the same Blueprint).
  int chains() const { return active_; }
  /// The same for functions of numbers: the first output (`fallback` when the call fails).
  f64 call_number(std::string_view function, std::initializer_list<f64> args, f64 fallback = 0);
  bool has_function(std::string_view function) const { return fn_index_.count(std::string(function)) != 0; }
  /// Where this Blueprint's Random nodes start from: drawn from the host's random numbers once,
  /// when a Random node first asks (deterministic per world; a Blueprint without Random draws
  /// nothing). Each node has a sequence of its own from it (logic::random).
  u32 seed();
  /// What output `out` of node `node` holds (both by their place: in the graph's list, among the
  /// node's outputs) - an event's values when its handler starts.
  const Value& output(int node, int out) const { return nodes_[static_cast<std::size_t>(node)].outputs[static_cast<std::size_t>(out)]; }
  /// A Return node: the running call of `function` ends with these outputs (by output name).
  /// False outside that function's own chain (the chain then just ends).
  bool return_values(std::string_view function, const std::vector<std::pair<std::string, Value>>& values);
  /// Runs as machine code (its own C++ class, or JIT-compiled WebAssembly) instead of the
  /// interpreter.
  bool compiled() const { return blueprint_ || jit_fn_ >= 0; }
  const char* tier() const { return blueprint_ ? "native C++" : jit_fn_ >= 0 ? "compiled (WebAssembly)" : jit_pending_ ? "compiling" : "interpreter"; }
  i32 id() const { return id_; }

  // ---- generated code support (the JIT's modules call these) ----
  /// Runs the C++ function of node `node` (returns the exec output ordinal, kStop or kSuspend).
  int call_node(int node, int entry);
  /// Input `pin` (index) of node `node`, as a number.
  f64 read_pin(int node, int pin);
  /// Whether node `node` is generated inline (else generated code calls call_node).
  bool inline_node(int node) const;
  bool inline_pure(int node) const;
  f64* slots() { return slots_.data(); }
  i32* stack() { return stack_.data(); }
  /// How many continuations the stack holds: as many as this graph can ever leave open (a node
  /// leaves some at most once in each running chain, and chains nest 64 events deep at most) -
  /// generated code never runs out of it where the interpreter and the Blueprint's class go on.
  int stack_capacity() const { return stack_capacity_; }
  i32* exec_table() { return exec_table_.data(); }
  int var_slot(std::string_view name) const;
  int var_slot(const Node& n) const;
  int node_slot(int node) const { return node_slot_base_ + node * CodeLayout::kNodeSlots; }
  /// Data output index -> slot (inline nodes with outputs other generated code / C++ read), -1 none.
  int out_slot(int node, int out) const;
  std::size_t node_count() const { return nodes_.size(); }
  const Node& node_at(int i) const { return *nodes_[static_cast<std::size_t>(i)].node; }
  const NodeDef* def_at(int i) const { return nodes_[static_cast<std::size_t>(i)].def; }
  const std::vector<PinDef>& inputs_at(int i) const { return nodes_[static_cast<std::size_t>(i)].in; }
  const std::vector<PinDef>& outputs_at(int i) const { return nodes_[static_cast<std::size_t>(i)].out; }
  struct SourceRef { int node = -1, pin = -1; };
  SourceRef source_of(int node, int input) const {
    const auto& s = nodes_[static_cast<std::size_t>(node)].data_in[static_cast<std::size_t>(input)];
    return {s.node, s.pin};
  }
  const Value& literal_of(int node, int input) const { return nodes_[static_cast<std::size_t>(node)].literal[static_cast<std::size_t>(input)]; }
  std::pair<int, int> exec_target(int node, int ordinal) const;
  int exec_outputs(int node) const { return static_cast<int>(nodes_[static_cast<std::size_t>(node)].exec_out.size()); }
  int exec_table_offset(int node) const { return exec_table_offset_[static_cast<std::size_t>(node)]; }
  /// Generated code ran node `node` (the editor's live view).
  void trace_node(int node);
  /// Shared counters the generated code checks: [0] nodes run this step, [1] limit, [2] tracing.
  i32* counters();

 private:
  friend class Runtime;
  friend class Context;
  void enter_compiled();              // copies interpreter state into the slots (tier switch)
  struct CNode {                     // compiled node
    const NodeDef* def = nullptr;
    const Node* node = nullptr;
    std::vector<PinDef> in, out;
    std::vector<int> exec_out;       // output index -> compiled index of the exec target (-1 none), by exec output order
    std::vector<int> exec_out_entry; // entry (exec input index) at the target
    struct Source { int node = -1, pin = -1; };
    std::vector<Source> data_in;     // per input: linked output (node, out index)
    std::vector<Value> literal;      // per input: literal / default
    std::vector<int> exec_in_index;  // input index -> exec input ordinal (-1 data)
    std::vector<Value> outputs;      // last output values
    NodeState state;
    int running = 0;                 // events: handlers of this node that are running (a handler that raises its own event)
    bool fresh = false;              // a value that is new on every read (Random), or one that passes such a value on
    bool fed_fresh = false;          // one of its inputs reads such a value
  };
  struct Machine {
    const StateMachine* def = nullptr;
    u32 state = 0;
    f64 entered = 0;
  };
  bool call_function(std::string_view function, const std::vector<Value>& args, std::vector<Value>& results, bool inside, const std::vector<Spot>* places = nullptr);
  Value read(const Spot& s) const;
  bool write(const Spot& s, const std::function<void(Value&)>& change);
  std::vector<std::vector<Spot>> refs_;                   // per function: where its reference inputs are, for the call that runs
  Value held(const std::string& key) const;
  bool hold(const std::string& key, const Value& v);
  /// The local variables of every function, as they are now - and back (Runtime::fire: a handler
  /// that starts in a running Blueprint may call a function that is in the middle of a call).
  std::vector<Value> locals() const;
  void set_locals(const std::vector<Value>& values);
  /// What its functions hold of their calls - parameters and local variables - as it is now,
  /// and back: for a call that starts while the Blueprint is in the middle of one.
  struct Held { std::vector<std::vector<Value>> parameters; std::vector<Value> locals; std::vector<std::vector<Spot>> refs; };
  Held held_calls() const;
  void set_held_calls(Held held);
  struct Local { std::string key; Value init; };
  std::vector<std::vector<Local>> locals_;                // per function: its local variables
  std::unordered_map<std::string, TypeRef> var_types_;    // what is held under a key
  struct Frame {                     // a function call that is running
    int function = -1;               // index in graph_.functions
    int depth = 0;                   // Runtime::depth_ of the call's own chain
    i32 stack_base = 0;              // generated code: continuation depth when the call began
    bool returned = false;
    std::vector<Value> results;
  };
  Runtime& rt_;
  Graph graph_;
  ActorRef self_ = 0;
  std::string name_;
  std::vector<CNode> nodes_;
  std::unordered_map<u32, int> index_;  // node id -> compiled index
  std::unordered_map<std::string, Value> vars_;   // non-numeric variables
  std::unordered_map<std::string, int> var_slots_; // numeric variables live in slots
  std::vector<Machine> machines_;
  std::unordered_map<std::string, int> fn_index_;  // function name -> index in graph_.functions
  std::vector<int> fn_entry_;         // per function: compiled index of its entry node (-1 none)
  std::vector<int> events_;           // the event nodes, by node id: the order handlers of one event run in
  std::vector<Frame> frames_;
  std::vector<f64> slots_;            // fixed size: generated code holds absolute addresses
  std::vector<i32> stack_;            // [0] = depth, then (pc, entry) pairs
  int stack_capacity_ = CodeLayout::kStack;
  bool jit_fits_ = true;              // (false: the graph could need more than kStackMost - it stays in the interpreter)
  std::vector<i32> exec_table_;       // per node: (target, entry) per exec output
  std::vector<int> exec_table_offset_;
  int node_slot_base_ = 0;
  i32 id_ = 0;
  i32 jit_fn_ = -1;                   // table index of the compiled run function
  bool jit_pending_ = false;
  std::unique_ptr<NativeBlueprint> blueprint_;  // the Blueprint as its compiled C++ class (register_native_blueprint)
  u32 seed_ = 0;                      // seed()
  bool seeded_ = false;
  int active_ = 0;                    // chains of this instance that are running (nested: events, calls)
  bool stopping_ = false;             // the loop guard: disabled when its outermost chain is over
  bool disabled_ = false;
  bool begun_ = false;
};

/// Event delivered to graphs: type (e.g. "game.overlap"), optional filter
/// (the actor the event is about) and named arguments for the event's outputs.
struct Event {
  std::string type;
  ActorRef target = 0;                 // only instances whose Self is target (0 = every instance)
  bool level = false;                  // with a target: the level graph (Self = none) receives it too
  std::string key;                     // event-specific match (custom event name, key code, action...)
  std::vector<std::pair<std::string, Value>> args;
  const Value* arg(std::string_view name) const;
};

class Runtime {
 public:
  explicit Runtime(Host& host, const Library& lib = library());
  ~Runtime();
  Host& host() { return host_; }
  const Library& lib() const { return lib_; }
  /// Compiles and adds a graph. Errors from check() disable the instance and are reported.
  Instance* add(const Graph& graph, ActorRef self, std::string name);
  void remove(ActorRef self);
  void clear();
  /// Begin Play for instances that have not begun.
  void begin();
  /// One fixed step: latent actions, timers, state machine timers, Tick events.
  void tick(f32 dt);
  /// Raises an event (synchronously runs the handlers).
  void fire(const Event& e);
  /// Custom event by name on every instance (or on `target`'s).
  void call(std::string_view name, ActorRef target = 0, std::vector<std::pair<std::string, Value>> args = {});
  /// Call on Actor, for every tier: runs `function` of the Blueprint of `target` (0: the Level's)
  /// with `args` and gives `results` as `caller` declares that function - its outputs in their
  /// order, each of the declared type; the declared defaults where the target has no Blueprint,
  /// no such function or fewer outputs. The call is a chain of its own (it counts for how deep
  /// chains nest: at most 64, then the call is left out and reported), and a Blueprint that is in
  /// the middle of a call of that function has its own values back afterwards.
  void call_on(Instance& caller, std::string_view function, ActorRef target, const std::vector<Value>& args, std::vector<Value>& results);
  /// For a Blueprint's class (NativeBlueprint): its node with the id `node` waits `seconds`; then
  /// the runtime runs it again (NativeBlueprint::run with `entry`) - the waits of every tier are
  /// kept and served in one place. cancel_waits: the node's waits are off.
  void wait(Instance& inst, u32 node, f64 seconds, int entry);
  void cancel_waits(Instance& inst, u32 node);
  Instance* find(ActorRef self);
  const std::vector<std::unique_ptr<Instance>>& instances() const { return instances_; }
  /// Node ids that ran recently (Studio flashes them while playing): instance name -> ids.
  struct Trace { std::string instance; u32 node = 0; f64 time = 0; };
  const std::vector<Trace>& trace() const { return trace_; }
  void clear_trace() { trace_.clear(); }
  bool tracing = false;
  /// The loop guard: loop passes in one step before the runtime assumes an endless loop. Loops are
  /// the one thing in a Blueprint without a fixed end (paths back, functions calling themselves
  /// and a tween played from its own Update are errors of the graph). Over the limit the loops of
  /// the chain end at once, the rest of the chain runs to its end, and then the Blueprint is
  /// stopped (disabled) and it is said - the same in every tier.
  u32 budget = 200000;
  /// May a loop of `inst` make another pass? (Counts it. False: the guard - the loop ends.)
  bool pass(Instance& inst);
  u64 executed() const { return executed_; }
  /// The host linked the native code of instance `id` (`function` = table index, < 0 failed).
  static void jit_ready(i32 id, i32 function);
  static Instance* instance_by_id(i32 id);
  /// Compile graphs to native code (on by default where the host can).
  bool jit = true;
  /// Run a Blueprint as its compiled C++ class where the binary has one (register_native_blueprint).
  /// Off: the interpreter (or the JIT) runs it - what the class is compared with.
  bool native = true;

 private:
  friend class Context;
  friend class Instance;
  struct Pending {                 // a suspended latent node
    Instance* inst = nullptr;
    int node = -1;
    f64 at = 0;                    // resume time
    int entry = 0;                 // entry passed back to the node
    u64 serial = 0;                // retriggering cancels older waits
  };
  enum class Chain : u8 { event, call, call_inside };  // what starts it (run_from)
  void run_from(Instance& inst, int node, int entry, Chain chain = Chain::event);
  void enter_state(Instance& inst, std::size_t machine, u32 state);
  void check_transitions(Instance& inst, const std::string* event);
  void step_starts();              // a new step: the guard counts from zero
  void chain_ends(Instance& inst); // the outermost chain of a stopped Blueprint is over: it is disabled
  Host& host_;
  const Library& lib_;
  std::vector<std::unique_ptr<Instance>> instances_;
  std::vector<Pending> pending_;
  std::vector<Trace> trace_;
  u64 executed_ = 0, tick_start_ = 0, serial_ = 0;
  int depth_ = 0;
  i32 counters_[5] = {0, 200000, 0, 0, 0};     // shared with generated code: [0] nodes run, [1] the limit of loop
                                               // passes per step, [2] tracing, [3] loop passes this step, [4] a loop
                                               // of the running compiled chain was refused a pass
  bool run_compiled(Instance& inst, int node, int entry);
};

/// What a node function sees while it runs.
class Context {
 public:
  Context(Runtime& rt, Instance& inst, int node, int entry);
  Runtime& runtime() { return rt_; }
  Host& host() { return rt_.host_; }
  Instance& instance() { return inst_; }
  ActorRef self() const { return inst_.self_; }
  const Node& node() const { return *inst_.nodes_[node_].node; }
  /// A setting of the node. One it does not store is its definition's default (ParamDef::def):
  /// `fallback` is for settings the definition does not declare.
  std::string param(std::string_view key, std::string_view fallback = {}) const;
  /// Which exec input started the node (0 for the first, kResume after a latent wait).
  int entry() const { return entry_; }
  /// Data input by index among *all* inputs (exec inputs included) or by pin id.
  Value in(std::size_t index);
  Value in(std::string_view pin);
  bool in_bool(std::string_view pin) { return in(pin).as_bool(); }
  f64 in_number(std::string_view pin) { return in(pin).as_number(); }
  i64 in_int(std::string_view pin) { return in(pin).as_int(); }
  Vec3 in_vector(std::string_view pin) { return in(pin).as_vector(); }
  std::string in_text(std::string_view pin) { return in(pin).as_text(); }
  ActorRef in_actor(std::string_view pin) { return in(pin).as_actor(); }
  /// The list entries of the node's path (the setting "path"): its index inputs, each read once,
  /// in the path's order - what Instance::place and Instance::change take.
  std::vector<i64> path();
  /// The place the node names under `prefix` (Instance::spot), its index inputs read here.
  Instance::Spot spot(std::string_view prefix);
  std::size_t input_count() const { return inst_.nodes_[node_].in.size(); }
  const PinDef& input(std::size_t i) const { return inst_.nodes_[node_].in[i]; }
  const PinDef& output(std::size_t i) const { return inst_.nodes_[node_].out[i]; }
  std::size_t node_outputs() const { return inst_.nodes_[node_].out.size(); }
  void out(std::string_view pin, Value v);
  void out(std::size_t index, Value v);
  /// Exec output ordinal by pin id (for returning from run()).
  int exec(std::string_view pin) const;
  NodeState& state() { return inst_.nodes_[node_].state; }
  /// State slot k (0..3) of this node: shared with generated code (loops, gates, flip flops).
  f64& slot(int k) { return inst_.slots_[static_cast<std::size_t>(inst_.node_slot(node_) + k)]; }
  /// Latent: resume this node after `seconds` with entry kResume (or `entry`).
  int wait(f64 seconds, int entry = kResume);
  /// Cancels this node's pending waits (Retriggerable Delay).
  void cancel_waits();
  /// After the current chain ends, continue at this node's exec output (Sequence, loops).
  void then(int exec_output);
  /// A loop's next pass: re-enter this node with `entry` once the chain started by the return
  /// value (the body) finishes. False: the loop guard refuses the pass (Runtime::budget) - the
  /// loop ends, the node goes on at Completed.
  bool loop_back(int entry);
  static constexpr int kResume = 1000;

 private:
  friend class Runtime;
  friend class Instance;
  Runtime& rt_;
  Instance& inst_;
  int node_;
  int entry_;
  std::vector<std::pair<int, int>> continuations_;  // (node, entry) to run after the chain, LIFO
  bool compiled_ = false;                           // continuations go to the instance's stack
  Value pull(int node, int pin);
};

}  // namespace kern::logic
