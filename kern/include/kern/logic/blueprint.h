// Kern Logic: what a Blueprint's C++ is compiled against (docs/blueprint_cpp.md).
//
// A Blueprint written as C++ (logic::to_cpp) is a struct that derives from kern::Blueprint. Its
// variables are its members, its functions its methods; every node it uses is a function here -
// typed, inline, the very function the interpreter's node calls (nodes.h). Nothing is looked up
// at run time.
//
// This covers the core library - variables, functions, formulas, the value nodes, lists, Branch /
// Sequence / While / For / For Each, Do Once, Do N, Gate, Flip Flop, the switches; the events,
// Call Event, Print, Game Time; Delay, Retriggerable Delay, Tween; state machines; Random - and the gameplay
// nodes (kern/game/blueprint_nodes.h). logic::native_ready tells what a Blueprint still lacks.
#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include "kern/logic/nodes.h"

namespace kern {

/// A list in a Blueprint's C++: List<Float>, List<Int>, List<Text> ...
template <class T> using List = std::vector<T>;

/// What a node keeps between runs: the member n<id> of a node with memory (a loop's index, Do
/// Once, Gate ...). The same four numbers the interpreter keeps for the node.
template <u32 Id> struct Node {
  static constexpr u32 id = Id;
  f64 memory[4] = {0, 0, 0, 0};
  /// A node that waits: what follows it (by exec output), kept until the wait is over.
  std::function<void(int)> later;
  /// A loop over things: what it goes through, as it was when the loop started (For Each Actor
  /// with Tag).
  std::vector<logic::ActorRef> list;
  /// For Each: the list as it was when the loop started (a List of the loop's item type).
  std::shared_ptr<const void> kept;
};

/// One way on of a node that waits and has several: {"update", [this] { ... }}.
struct Path {
  std::string_view pin;
  std::function<void()> run;
};

/// kern::Machine machine3 = machine("Door").state(...).on_event(...).after(...): a state machine
/// of the Blueprint, written down. It is the graph's: the runtime runs the machines of the graph
/// the class was written from (timers, conditions, events - the same code for every tier); the
/// class asks and sets states through it (state_set, state_get, state_is) and has the state
/// events as handlers.
struct Machine {
  Machine& state(u32, std::string_view, f64, f64, bool = false) { return *this; }
  Machine& on_event(u32, u32, u32, std::string_view, std::string_view = {}) { return *this; }
  Machine& after(u32, u32, u32, f64, std::string_view = {}) { return *this; }
  Machine& when(u32, u32, u32, std::string_view, std::string_view = {}) { return *this; }
  Machine& automatic(u32, u32, u32, std::string_view = {}) { return *this; }
};

/// kern::formula(lambda, values...): a Formula node. The lambda is the formula.
template <class F, class... A> constexpr f64 formula(F&& f, A... values) { return static_cast<f64>(f(values...)); }

/// kern::out(n7.stars, n7.found): where a call on another Blueprint puts what it gave, in the
/// order of the function's outputs (func_call_on).
template <class... O> struct Out {
  std::tuple<O&...> to;
  void take(const std::vector<logic::Value>& results) { take(results, std::index_sequence_for<O...>{}); }
  template <std::size_t... I> void take(const std::vector<logic::Value>& results, std::index_sequence<I...>) {
    ((I < results.size() ? void(std::get<I>(to) = logic::from_value<O>(results[I])) : void()), ...);
  }
};
template <class... O> Out<O...> out(O&... o) { return {std::tuple<O&...>(o...)}; }

/// Where the nodes are in the graph view (KERN_LAYOUT): for the editor, nothing at run time.
struct Layout {
  void node(u32, f64, f64, const char* = "") {}
  void box(u32, f64, f64, f64, f64, const char*, u32 = 0) {}
  void next_id(u32) {}
};

/// The nodes of the core library (src/logic/core_nodes.cpp). kern::Blueprint, below, is these and
/// the gameplay nodes.
struct BlueprintCore : logic::PureNodes {
  /// A whole number as a Blueprint holds it (the functions of nodes.h take and give i64).
  using Int = logic::Whole;
  /// A list of values of one type: List<Float>, List<Int> ...
  template <class T> using List = kern::List<T>;

  /// The world the Blueprint runs in (the engine sets it; see logic::BlueprintWorld).
  logic::BlueprintWorld kern_world;

  // ---- the world ----
  void debug_print(const Text& text, Bool screen) {
    if (kern_world.host) kern_world.host->print(text, screen);
  }
  Float time_now() const { return kern_world.host ? kern_world.host->time() : 0.0; }
  /// Call Event: raises the Custom Event `Name` on Target's graph (on every graph: Actor{0}).
  template <Setting Name> void flow_call(Actor target, Actor actor, Float value, const Text& text) {
    if (kern_world.runtime)
      kern_world.runtime->call(Name.view(), target.id, {{"actor", logic::Value::actor(actor.id)}, {"value", logic::Value::number(value)}, {"text", logic::Value::text(text)}});
  }

  /// Call on Actor: runs the function `Name` of the Blueprint of `target` (Actor{0}: the Level's)
  /// with `args`, and `out` takes what it gave - func_call_on<"Stars">(who, kern::out(n7.stars), x);
  template <Setting Name, class... O, class... A> void func_call_on(Actor target, Out<O...> out, const A&... args) {
    std::vector<logic::Value> results;
    if (kern_world.runtime && kern_world.instance) kern_world.runtime->call_on(*kern_world.instance, Name.view(), target.id, {logic::to_value(args)...}, results);
    out.take(results);
  }

  // ---- random ----
  /// Where the Random nodes start from: the instance's seed (a class without a world: its host's
  /// first number, or 0).
  u32 kern_seed() {
    if (!kern_seeded) {
      kern_seeded = true;
      kern_seed_value = kern_world.instance ? kern_world.instance->seed() : kern_world.host ? kern_world.host->random() : 0;
    }
    return kern_seed_value;
  }
  u32 kern_seed_value = 0;
  bool kern_seeded = false;
  template <class N> Float math_random(N& n, Float min, Float max) { return logic::random::number(n.memory, kern_seed(), N::id, min, max); }
  template <class N> Int math_random_int(N& n, Int min, Int max) { return logic::random::whole_number(n.memory, kern_seed(), N::id, min, max); }
  template <class N> Bool math_random_bool(N& n, Float chance) { return logic::random::chance(n.memory, kern_seed(), N::id, chance); }

  // ---- state machines ----
  static Machine machine(std::string_view) { return {}; }
  template <Setting Name, Setting State> void state_set() {
    if (kern_world.instance) kern_world.instance->set_state(Name.view(), State.view());
  }
  template <Setting Name> Text state_get() const { return kern_world.instance ? kern_world.instance->state(Name.view()) : Text(); }
  template <Setting Name, Setting State> Bool state_is() const { return kern_world.instance && kern_world.instance->state(Name.view()) == State.view(); }

  // ---- waiting ----
  // The runtime keeps the waits and runs the node again when one is over (<node>_resume, through
  // the glue); what follows the node is kept in its member until then.
  void kern_wait(u32 node, Float seconds) {
    if (kern_world.runtime) kern_world.runtime->wait(*kern_world.instance, node, seconds, logic::Context::kResume);
  }
  void kern_cancel(u32 node) {
    if (kern_world.runtime) kern_world.runtime->cancel_waits(*kern_world.instance, node);
  }
  /// flow_delay(n, seconds, [this] { ... }): waits, then goes on. A call while it waits is ignored.
  template <class N, class F> void flow_delay(N& n, Float duration, F&& then) {
    if (!logic::flow::delay_start(n.memory)) return;
    n.later = [f = std::forward<F>(then)](int) { f(); };
    kern_wait(N::id, duration);
  }
  template <class N> void flow_delay_resume(N& n) {
    logic::flow::delay_done(n.memory);
    const auto later = n.later;  // (what follows may start the wait again)
    if (later) later(0);
  }
  /// Retriggerable Delay: every new call starts the wait again.
  template <class N, class F> void flow_retrigger_delay(N& n, Float duration, F&& then) {
    kern_cancel(N::id);
    n.later = [f = std::forward<F>(then)](int) { f(); };
    kern_wait(N::id, duration);
  }
  template <class N> void flow_retrigger_delay_resume(N& n) {
    const auto later = n.later;
    if (later) later(0);
  }
  /// flow_tween<"smooth">(n, "play", seconds, {{"update", [this] { ... }}, {"finished", [this] { ... }}}):
  /// Update runs every step with n.alpha going 0 to 1, then Finished. "stop" ends it.
  template <Setting Ease, class N> void flow_tween(N& n, std::string_view entry, Float duration, std::initializer_list<Path> paths) {
    kern_cancel(N::id);
    if (entry == "stop") {
      logic::flow::tween_stop(n.memory);
      return;
    }
    logic::flow::tween_play(n.memory, time_now(), duration);
    std::function<void()> update, finished;
    for (const Path& p : paths) (p.pin == "update" ? update : finished) = p.run;
    n.later = [update, finished](int output) {
      const std::function<void()>& run = output == 0 ? update : finished;
      if (run) run();
    };
    flow_tween_resume<Ease>(n, true);
  }
  template <Setting Ease, class N> void flow_tween_resume(N& n, bool first = false) {
    if (!logic::flow::tween_runs(n.memory)) return;
    bool finished = false;
    n.alpha = logic::flow::tween_step(n.memory, time_now(), Ease.view(), first, finished);
    const auto later = n.later;
    if (!finished) kern_wait(N::id, 0);  // again next step
    if (!later) return;
    later(0);
    if (finished) later(1);
  }

  // ---- loops ----
  /// The loop guard (Runtime::budget): may a loop make another pass? The runtime counts the passes
  /// of every tier in one place; over the limit the loops of the chain end at once - an endless
  /// loop ends, the rest of the chain runs to its end, then the Blueprint is stopped.
  bool kern_pass() {
    i32* count = kern_world.counters;
    if (!count) return true;  // (no world: a class somebody just made)
    if (count[3] < count[1]) {
      ++count[3];
      return true;
    }
    return kern_world.runtime->pass(*kern_world.instance);
  }
  /// while (loop(condition)): a While Loop.
  bool loop(Bool condition) { return condition && kern_pass(); }
  /// for (auto it = flow_for(n, first, last); it.next();): a For Loop; n.index is the index.
  template <class N> struct ForLoop {
    BlueprintCore& self;
    N& n;
    bool first = true;
    bool next() {
      if (!first) logic::flow::for_step(n.memory);
      first = false;
      if (!logic::flow::for_runs(n.memory) || !self.kern_pass()) return false;
      n.index = logic::flow::for_index(n.memory);
      return true;
    }
  };
  template <class N> ForLoop<N> flow_for(N& n, Int first, Int last) {
    logic::flow::for_start(n.memory, first, last);
    return {*this, n};
  }

  /// for (auto it = flow_for_each<Float>(n, list); it.next();): every item of the list as it
  /// is when the loop starts; n.item and n.index are the pass's. Where the loop is, is the node's
  /// (n.kept, n.memory[0]) - like the index of a For Loop.
  template <class N, class T> struct ListLoop {
    BlueprintCore& self;
    N& n;
    bool first = true;
    bool next() {
      if (!first) n.memory[0] += 1;
      first = false;
      const List<T>* list = static_cast<const List<T>*>(n.kept.get());
      const std::size_t at = static_cast<std::size_t>(n.memory[0]);
      if (!list || at >= list->size() || !self.kern_pass()) return false;
      n.item = T((*list)[at]);
      n.index = static_cast<i64>(at);
      return true;
    }
  };
  template <class T, class N> ListLoop<N, T> flow_for_each(N& n, std::type_identity_t<List<T>> list) {
    n.kept = std::make_shared<const List<T>>(std::move(list));
    n.memory[0] = 0;
    return {*this, n};
  }

  // ---- lists ----
  // What they do is logic::list (nodes.h): the functions the interpreter's nodes call. The values
  // take any list - the type of its items is their setting, list_get<Float>(Scores, 0); the steps
  // change a list variable - the member itself is their first value.
  template <class T, Setting Count, class... V> static List<T> list_make(const V&... items) { return List<T>{T(items)...}; }
  template <class T> static Int list_length(const std::type_identity_t<List<T>>& list) { return logic::list::length(list); }
  template <class T> static T list_get(const std::type_identity_t<List<T>>& list, Int index) { return logic::list::get(list, index); }
  template <class T> static Bool list_contains(const std::type_identity_t<List<T>>& list, const std::type_identity_t<T>& item) { return logic::list::contains(list, item); }
  template <class T> static Int list_find(const std::type_identity_t<List<T>>& list, const std::type_identity_t<T>& item) { return logic::list::find(list, item); }
  template <class T> static void list_add(List<T>& list, std::type_identity_t<T> item) { logic::list::add(list, std::move(item)); }
  template <class N, class T> static void list_add(N& n, List<T>& list, std::type_identity_t<T> item) { n.index = logic::list::add(list, std::move(item)); }
  template <class T> static void list_insert(List<T>& list, Int index, std::type_identity_t<T> item) { logic::list::insert(list, index, std::move(item)); }
  template <class T> static void list_set(List<T>& list, Int index, std::type_identity_t<T> item) { logic::list::set(list, index, std::move(item)); }
  template <class T> static void list_remove_at(List<T>& list, Int index) { logic::list::remove_at(list, index); }
  template <class T> static void list_remove(List<T>& list, const std::type_identity_t<T>& item) { logic::list::remove(list, item); }
  template <class N, class T> static void list_remove(N& n, List<T>& list, const std::type_identity_t<T>& item) { n.removed = logic::list::remove(list, item); }
  template <class T> static void list_clear(List<T>& list) { logic::list::clear(list); }

  // ---- enums ----
  // An enum the Blueprint declares with KERN_ENUM; the nodes' setting "Enum" is the type:
  // enum_name<Mode>(State). What they do is logic::enums (nodes.h).
  template <class E> static const std::vector<std::string>& kern_enum_values() {
    static const std::vector<std::string> names = logic::enums::names(kern_enum_names(E{}));
    return names;
  }
  template <class E> struct EnumFound {
    E value{};
    Bool found = false;
  };
  template <class E> struct EnumValid {
    E value{};
    Bool valid = false;
  };
  template <class E> static Bool enum_equal(std::type_identity_t<E> a, std::type_identity_t<E> b) { return a == b; }
  template <class E> static Text enum_name(std::type_identity_t<E> value) { return logic::enums::name(kern_enum_values<E>(), static_cast<i64>(value)); }
  template <class E> static EnumFound<E> enum_from_name(const Text& name) {
    const i64 at = logic::enums::find(kern_enum_values<E>(), name);
    return {static_cast<E>(at < 0 ? 0 : at), at >= 0};
  }
  template <class E> static Int enum_index(std::type_identity_t<E> value) { return static_cast<i64>(value); }
  template <class E> static EnumValid<E> enum_from_index(Int index) {
    const std::size_t count = kern_enum_values<E>().size();
    return {static_cast<E>(logic::enums::valid(count, index)), logic::enums::is_value(count, index)};
  }
  template <class E> static List<E> enum_values() {
    List<E> all;
    for (std::size_t k = 0; k < kern_enum_values<E>().size(); ++k) all.push_back(static_cast<E>(k));
    return all;
  }
  /// switch (flow_switch_enum<Mode>(State)) { case Mode::Closed: { } break; ... }: the way on is
  /// the value's - a case for every value of the enum, as C++ wants a switch over an enum to be.
  template <class E> static constexpr E flow_switch_enum(std::type_identity_t<E> value) { return value; }

  // ---- structs ----
  // A struct the Blueprint declares with KERN_STRUCT; the nodes' setting "Struct" is the type.
  // Make is the struct from a value for each field, in their order; Break is the struct, read
  // field by field: struct_break<Weapon>(Held).Damage. (Set Field is the assignment
  // Held.Damage = 2.0; - no function.)
  template <class S, class... V> static S struct_make(const V&... fields) { return S{fields...}; }
  /// list_find_row<&Weapon::Name>(Rack, "Bow"): the first item of a list of structs whose field
  /// is the value - a table's row by its key. Not found: the struct's defaults, index -1.
  template <class S> struct FoundRow {
    Bool found = false;
    S row{};
    Int index = -1;
  };
  template <class F, class S> static S owner_of(F S::*);
  template <class F, class S> static F field_of(F S::*);
  template <auto Field>
  static auto list_find_row(const List<decltype(owner_of(Field))>& list, const std::type_identity_t<decltype(field_of(Field))>& value) {
    FoundRow<decltype(owner_of(Field))> r;
    for (std::size_t k = 0; k < list.size(); ++k)
      if (list[k].*Field == value) return FoundRow<decltype(owner_of(Field))>{true, list[k], static_cast<Int>(k)};
    return r;
  }
  template <class S> static S struct_break(std::type_identity_t<S> value) { return value; }

  // ---- steps ----
  void flow_reroute() {}
  /// A Return nothing leads to (a method of its own that nothing calls).
  template <Setting Function, class... V> static void func_return(const V&...) {}
  /// if (flow_do_once(n, "exec", closed)): the call goes through once ("reset": again).
  template <class N> bool flow_do_once(N& n, std::string_view entry, Bool closed) { return logic::flow::do_once(n.memory, entry == "reset" ? 1 : 0, closed); }
  template <class N> bool flow_do_n(N& n, std::string_view entry, Int count) {
    i64 counter = n.counter;
    const bool through = logic::flow::do_n(n.memory, entry == "reset" ? 1 : 0, count, counter);
    n.counter = counter;
    return through;
  }
  template <class N> bool flow_gate(N& n, std::string_view entry, Bool closed) {
    return logic::flow::gate(n.memory, entry == "open" ? 1 : entry == "close" ? 2 : entry == "toggle" ? 3 : 0, closed);
  }
  /// switch (flow_flip_flop(n)): case 0 is A, case 1 is B.
  template <class N> int flow_flip_flop(N& n) { return logic::flow::flip_flop(n.memory, n.is_a); }
  template <Setting Count> static int flow_switch_int(Int selection) { return logic::flow::switch_int(logic::setting_count(Count.view(), 3, 1, 32), selection); }
  template <Setting Cases> static int flow_switch_text(const Text& selection) { return logic::flow::switch_text(Cases.view(), selection); }

  // ---- values with a setting ----
  template <Setting Op> static Bool math_compare(Float a, Float b) { return compare(Op.view(), a, b); }
  /// The setting "Type" of a generic node is the type itself: logic_select<Float>, value_reroute<List<Text>>.
  template <class T> static T logic_select(Bool pick, std::type_identity_t<T> a, std::type_identity_t<T> b) { return pick ? a : b; }
  template <class T> static T value_reroute(std::type_identity_t<T> in) { return in; }
};

}  // namespace kern

#include "kern/game/blueprint_nodes.h"

namespace kern {

/// What a Blueprint's C++ derives from: every node of the library as a function - the core's
/// (BlueprintCore) and the game's (game::BlueprintNodesOf; they need the game only where a
/// Blueprint uses them).
struct Blueprint : game::BlueprintNodesOf<BlueprintCore> {};

}  // namespace kern

/// KERN_ENUM(Mode, Closed, Opening, Open); declares an enum of the Blueprint: the type Mode with
/// the values Mode::Closed ..., a value being its place among them (the first is 0). The names
/// are stated once - the enum nodes (Enum to Text ...) read them from here.
#define KERN_ENUM(Name, ...)                                                                  \
  enum class Name : ::kern::i64 { __VA_ARGS__ };                                              \
  friend constexpr ::std::string_view kern_enum_names(Name) { return #__VA_ARGS__; }         \
  static_assert(true)
/// KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; }; declares a struct of the
/// Blueprint: the type Weapon with these fields, each with its default. It is a plain aggregate -
/// Weapon{"Knife", 25.0} gives every field in their order - and two of them are equal when all
/// their fields are (logic::structs::equal, nodes.h).
#define KERN_STRUCT(Name)                                                                              \
  struct Name;                                                                                         \
  friend bool operator==(const Name& a, const Name& b) { return ::kern::logic::structs::equal(a, b); } \
  struct Name
/// A function that is offered as a value node (no exec pins).
#define KERN_PURE
/// KERN_EXTERN FnResult Fn(Float x = 0.0); says what a function of another Blueprint looks like:
/// it has no body here and is called on an actor (func_call_on<"Fn">).
#define KERN_EXTERN
/// KERN_EVENT(on5, event_custom<"Hit">) says that the method on5 is the handler of that event
/// node (for the reader and the editor). Which events reach it decides the runtime, for every
/// tier the same; the engine calls the method through the Blueprint's glue (logic::native_glue).
#define KERN_EVENT(method, ...)
/// KERN_LAYOUT(Name) { node(id, x, y); ... }: the layout of the Blueprint Name.
#define KERN_LAYOUT(Name)                \
  struct Name##_Layout : ::kern::Layout { \
    void place();                        \
  };                                     \
  inline void Name##_Layout::place()
