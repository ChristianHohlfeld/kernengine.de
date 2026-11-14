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
#include <utility>
#include <vector>

#include "kern/logic/engine_nodes.gen.h"
#include "kern/logic/nodes.h"

namespace kern {

/// A list in a Blueprint's C++: List<Float>, List<Int>, List<Text> ... - a std::vector whose []
/// never reaches beside the list (logic::List, nodes.h).
template <class T> using List = logic::List<T>;
template <class T, std::size_t N> using Fixed = logic::Fixed<T, N>;

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
  /// A text as a Blueprint's class holds it: one written out in the program is referred to, one
  /// made while running is held (logic::Text, nodes.h).
  using Text = logic::Text;
  /// A list of values of one type: List<Float>, List<Int> ...
  template <class T> using List = kern::List<T>;
  template <class T, std::size_t N> using Fixed = kern::Fixed<T, N>;
  /// A map of values under their keys and a set of keys, in the order the keys came: Map<Int, Float>, Set<Int>.
  template <class K, class V> using Map = logic::Map<K, V>;
  template <class K> using Set = logic::Set<K>;

  /// The world the Blueprint runs in (the engine sets it; see logic::BlueprintWorld).
  logic::BlueprintWorld kern_world;

  // ---- the world ----
  void debug_print(const Text& text, Bool screen) {
    if (kern_world.host) kern_world.host->print(text.str(), screen);
  }
  Float time_now() const { return kern_world.host ? kern_world.host->time() : 0.0; }
  /// Call Event: raises the Custom Event `Name` on Target's graph (on every graph: Actor{0}).
  template <Setting Name> void flow_call(Actor target, Actor actor, Float value, const Text& text) {
    if (kern_world.runtime)
      kern_world.runtime->call(Name.view(), target.id, {{"actor", logic::Value::actor(actor.id)}, {"value", logic::Value::number(value)}, {"text", logic::to_value(text)}});
  }

  /// Call on Actor: runs the function `Name` of the Blueprint of `target` (Actor{0}: the Level's)
  /// with `args`, and `out` takes what it gave - func_call_on<"Stars">(who, kern::out(n7.stars), x);
  template <Setting Name, class... O, class... A> void func_call_on(Actor target, Out<O...> out, const A&... args) {
    std::vector<logic::Value> results;
    if (kern_world.runtime && kern_world.instance) kern_world.runtime->call_on(*kern_world.instance, Name.view(), target.id, {logic::to_value(args)...}, results);
    out.take(results);
  }

  /// Get Variable on Actor: what the variable `Name` of the Blueprint of `target` holds, as a T -
  /// func_get_on<"MaxStars", Float>(actor_find("Wanted")).
  template <Setting Name, class T> T func_get_on(Actor target) {
    if (!kern_world.runtime || !kern_world.instance) return T{};
    return logic::from_value<T>(kern_world.runtime->get_on(*kern_world.instance, Name.view(), target.id, logic::TypeRef{logic::pin_type_of<T>(), {}}));
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
  /// Make List gives its values as what they are - that many entries, each made once where it
  /// stands (what is handed over is not copied): a list of a fixed length. Where a list is
  /// needed it becomes one; set at a place of a fixed length it asks for no memory at all.
  template <class T, Setting Count, class... V> static Fixed<T, sizeof...(V)> list_make(V&&... items) {
    return Fixed<T, sizeof...(V)>(std::in_place, T(std::forward<V>(items))...);
  }
  // (the values read the entries where they stand - of a List or of a Fixed: logic::Items)
  template <class T> static Int list_length(logic::Items<T> list) { return static_cast<i64>(list.size()); }
  template <class T> static const T& list_get(logic::Items<T> list, Int index) { return list[index]; }
  template <class T> static Int list_find(logic::Items<T> list, const std::type_identity_t<T>& item) {
    for (std::size_t k = 0; k < list.size(); ++k)
      if (list.first[k] == item) return static_cast<i64>(k);
    return -1;
  }
  template <class T> static Bool list_contains(logic::Items<T> list, const std::type_identity_t<T>& item) { return list_find<T>(list, item) >= 0; }
  template <class T> static void list_add(List<T>& list, std::type_identity_t<T> item) { logic::list::add(list, std::move(item)); }
  template <class N, class T> static void list_add(N& n, List<T>& list, std::type_identity_t<T> item) { n.index = logic::list::add(list, std::move(item)); }
  template <class T> static void list_insert(List<T>& list, Int index, std::type_identity_t<T> item) { logic::list::insert(list, index, std::move(item)); }
  template <class T> static void list_remove_at(List<T>& list, Int index) { logic::list::remove_at(list, index); }
  template <class T> static void list_remove(List<T>& list, const std::type_identity_t<T>& item) { logic::list::remove(list, item); }
  template <class N, class T> static void list_remove(N& n, List<T>& list, const std::type_identity_t<T>& item) { n.removed = logic::list::remove(list, item); }
  template <class T> static void list_clear(List<T>& list) { logic::list::clear(list); }
  /// (in one go: what std::vector does in one request for memory - resize, insert of a range, reserve)
  template <class T> static void list_resize(List<T>& list, Int length, const std::type_identity_t<T>& value) { logic::list::resize(list, length, value); }
  // ---- maps and sets ----
  // What they do is logic::map and logic::set (nodes.h) - on the same container the interpreter's
  // nodes use. The values take any map or set - its keys' and values' types are their settings,
  // map_find<Int, Float>(Cells, 7); the steps change one at a place - the place is their first value.
  template <class V> struct MapFound {
    Bool found = false;
    V value{};
    Int index = -1;
  };
  template <class K, class V> static Int map_length(const Map<K, V>& m) { return logic::map::length(m); }
  template <class K, class V> static Bool map_contains(const Map<K, V>& m, const std::type_identity_t<K>& key) { return logic::map::contains(m, key); }
  template <class K, class V> static K map_key_at(const Map<K, V>& m, Int index) { return logic::map::key_at(m, index); }
  template <class K, class V> static MapFound<V> map_find(const Map<K, V>& m, const std::type_identity_t<K>& key) {
    MapFound<V> r;
    const i64 at = logic::map::index_of(m, key);
    if (at >= 0) r.found = true, r.value = V(m.value_at(static_cast<std::size_t>(at))), r.index = at;
    return r;
  }
  template <class K, class V> static void map_set(Map<K, V>& m, const std::type_identity_t<K>& key, std::type_identity_t<V> value) { logic::map::set(m, key, std::move(value)); }
  template <class N, class K, class V> static void map_set(N& n, Map<K, V>& m, const std::type_identity_t<K>& key, std::type_identity_t<V> value) {
    const auto [added, at] = logic::map::set(m, key, std::move(value));
    n.added = added, n.index = at;
  }
  template <class K, class V> static void map_add(Map<K, V>& m, const std::type_identity_t<K>& key) { logic::map::add(m, key); }
  template <class N, class K, class V> static void map_add(N& n, Map<K, V>& m, const std::type_identity_t<K>& key) {
    const auto [added, at] = logic::map::add(m, key);
    n.added = added, n.index = at;
  }
  template <class K, class V> static void map_remove(Map<K, V>& m, const std::type_identity_t<K>& key) { logic::map::remove(m, key); }
  template <class N, class K, class V> static void map_remove(N& n, Map<K, V>& m, const std::type_identity_t<K>& key) { n.removed = logic::map::remove(m, key); }
  template <class K, class V> static void map_remove_at(Map<K, V>& m, Int index) { logic::map::remove_at(m, index); }
  template <class K, class V> static void map_clear(Map<K, V>& m) { logic::map::clear(m); }
  template <class K, class V> static void map_reserve(Map<K, V>& m, Int count) { logic::map::reserve(m, count); }
  template <class K> static Int set_length(const Set<K>& s) { return logic::set::length(s); }
  template <class K> static Bool set_contains(const Set<K>& s, const std::type_identity_t<K>& key) { return logic::set::contains(s, key); }
  template <class K> static K set_key_at(const Set<K>& s, Int index) { return logic::set::key_at(s, index); }
  template <class K> static Int set_find(const Set<K>& s, const std::type_identity_t<K>& key) { return logic::set::index_of(s, key); }
  template <class K> static void set_add(Set<K>& s, const std::type_identity_t<K>& key) { logic::set::add(s, key); }
  template <class N, class K> static void set_add(N& n, Set<K>& s, const std::type_identity_t<K>& key) {
    const auto [added, at] = logic::set::add(s, key);
    n.added = added, n.index = at;
  }
  template <class K> static void set_remove(Set<K>& s, const std::type_identity_t<K>& key) { logic::set::remove(s, key); }
  template <class N, class K> static void set_remove(N& n, Set<K>& s, const std::type_identity_t<K>& key) { n.removed = logic::set::remove(s, key); }
  template <class K> static void set_remove_at(Set<K>& s, Int index) { logic::set::remove_at(s, index); }
  template <class K> static void set_clear(Set<K>& s) { logic::set::clear(s); }
  template <class K> static void set_reserve(Set<K>& s, Int count) { logic::set::reserve(s, count); }

  /// Sort List: list_sort<&Board::Before>(Scores) - in the order a function of the Blueprint says (it
  /// takes two items and gives whether the first goes before the second); list_sort<nullptr>(Scores):
  /// by the items' `<`. Stable: equal items keep their order.
  template <auto Less, class T> void list_sort(List<T>& list) {
    if constexpr (std::is_null_pointer_v<decltype(Less)>) {
      logic::list::sort(list);
    } else {
      auto& self = static_cast<typename kern_member_of<decltype(Less)>::type&>(*this);
      logic::list::sort(list, [&self](const auto& a, const auto& b) -> bool {
        const auto [before] = (self.*Less)(a, b);
        return before;
      });
    }
  }
  template <class M> struct kern_member_of;
  template <class C, class F> struct kern_member_of<F C::*> { using type = C; };
  template <class T> static void list_append(List<T>& list, std::type_identity_t<logic::Items<T>> items) {
    logic::list::insert_items(list, static_cast<i64>(list.size()), items.first, items.count);
  }
  template <class T> static void list_insert_list(List<T>& list, Int index, std::type_identity_t<logic::Items<T>> items) { logic::list::insert_items(list, index, items.first, items.count); }
  template <class T> static void list_reserve(List<T>& list, Int count) { logic::list::reserve(list, count); }

  // ---- variables ----
  // Get, Set and Add are C++'s own (Heat, Heat = value, Heat += value). A Take gives what
  // stands at a place and leaves the zero of its type there - std::exchange(place, {}): a list,
  // a text is handed on, not copied.
  template <class T> static void var_take(T& place) { place = T{}; }
  template <class N, class T> static void var_take(N& n, T& place) { n.value = std::exchange(place, T{}); }

  // ---- text ----
  // The text nodes on a Blueprint's Text: read where the characters are - a text is copied only
  // where a new one is made (what they give is what logic::PureNodes gives, character for character).
  static Text value_text(Text value) { return value; }
  static Text text_append(const Text& a, const Text& b, const Text& c) {
    std::string made;
    made.reserve(a.size() + b.size() + c.size());
    made.append(a.view()).append(b.view()).append(c.view());
    return made;
  }
  static Bool text_equal(const Text& a, const Text& b) { return a == b; }
  static Bool text_contains(const Text& a, const Text& b) { return a.view().find(b.view()) != std::string_view::npos; }
  static Int text_length(const Text& a) { return static_cast<i64>(a.size()); }
  /// (Text to Int64: of the text where it stands)
  static Int64 int64_of_text(const Text& a) { return logic::PureNodes::int64_parse(a.view()); }
  static Int text_code_at(const Text& a, Int index) {
    const std::string_view v = a.view();
    return index >= 0 && static_cast<u64>(index) < v.size() ? static_cast<i64>(static_cast<u8>(v[static_cast<std::size_t>(static_cast<i64>(index))])) : 0;
  }
  static Text text_of_code(Int code) { return std::string(1, static_cast<char>(static_cast<u8>(static_cast<i64>(code)))); }
  /// Format Text: text_format<"%02d:%06.3f">(most, minutes, seconds) - the values in the order of
  /// the format's conversions (logic::format).
  template <Setting Format, class... V> static Text text_format(Int most, const V&... values) {
    const logic::format::Arg args[] = {format_arg(values)..., logic::format::Arg{}};
    return logic::format::text_of(Format.view(), args, sizeof...(V), most);
  }
  static logic::format::Arg format_arg(Int v) { return logic::format::arg(static_cast<i64>(v)); }
  static logic::format::Arg format_arg(Float v) { return logic::format::arg(v); }
  static logic::format::Arg format_arg(const Text& v) { return logic::format::arg(v.view()); }

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
  /// (The names of an enum's values are part of the program: the text refers to the name.)
  template <class E> static Text enum_name(std::type_identity_t<E> value) {
    const std::vector<std::string>& names = kern_enum_values<E>();
    const i64 at = static_cast<i64>(value);
    return logic::enums::is_value(names.size(), at) ? Text::of_program(names[static_cast<std::size_t>(at)]) : Text();
  }
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
  // field by field: struct_break<Weapon>(Held).Damage. (A field of a variable is set where it
  // is: Held.Damage = 2.0; - the Set at that place, no function.)
  template <class S, class... V> static S struct_make(V&&... fields) { return S{std::forward<V>(fields)...}; }
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
  template <class S> static const S& struct_break(const std::type_identity_t<S>& value) { return value; }
  /// The bytes of a struct of numbers in the layout of the setting (logic::bytes, nodes.h):
  /// struct_bytes<Frame, "20: u32@0 u16@4 ...">(frame) - as many as the layout's size, a list of a
  /// fixed length (where it lies, nothing is asked for); struct_of_bytes<Frame, "...">(bytes, at)
  /// the struct whose bytes begin at `at` in the list.
  template <class S, Setting Layout> static auto struct_bytes(const std::type_identity_t<S>& value) {
    static constexpr logic::bytes::Layout kLayout = logic::bytes::layout(Layout.view());
    static_assert(kLayout.error == nullptr, "the layout of Bytes of Struct cannot be read");
    static_assert(kLayout.count == logic::structs::field_count<S>(), "the layout has a field for each field of the struct");
    Fixed<Int, kLayout.size> out;
    std::size_t k = 0;
    logic::structs::fields(value, [&](const auto&... field) { (put_bytes(out.items.data(), kLayout.fields[k++], field), ...); });
    return out;
  }
  template <class S, Setting Layout> static S struct_of_bytes(logic::Items<Int> bytes, Int at) {
    static constexpr logic::bytes::Layout kLayout = logic::bytes::layout(Layout.view());
    static_assert(kLayout.error == nullptr, "the layout of Struct of Bytes cannot be read");
    static_assert(kLayout.count == logic::structs::field_count<S>(), "the layout has a field for each field of the struct");
    S out{};
    const i64 from = at;
    auto byte = [&](u32 i) { return logic::bytes::byte_at(bytes.first, bytes.count, from, i); };
    std::size_t k = 0;
    logic::structs::fields(out, [&](auto&... field) { (take_bytes(field, kLayout.fields[k++], byte), ...); });
    return out;
  }
  static void put_bytes(Int* out, const logic::bytes::Field& f, Bool v) { logic::bytes::put(out, f, v ? 1 : 0); }
  static void put_bytes(Int* out, const logic::bytes::Field& f, Int v) { logic::bytes::put(out, f, static_cast<u64>(static_cast<i64>(v))); }
  static void put_bytes(Int* out, const logic::bytes::Field& f, logic::Int64 v) { logic::bytes::put(out, f, v.bits); }
  static void put_bytes(Int* out, const logic::bytes::Field& f, Float v) { logic::bytes::put(out, f, logic::bytes::bits(f.code, v)); }
  template <class B> static void take_bytes(Bool& to, const logic::bytes::Field& f, B&& byte) { to = logic::bytes::take(f, byte) != 0; }
  template <class B> static void take_bytes(Int& to, const logic::bytes::Field& f, B&& byte) { to = static_cast<i64>(logic::bytes::take(f, byte)); }
  template <class B> static void take_bytes(logic::Int64& to, const logic::bytes::Field& f, B&& byte) { to = logic::Int64(logic::bytes::take(f, byte)); }
  template <class B> static void take_bytes(Float& to, const logic::bytes::Field& f, B&& byte) {
    const u64 b = logic::bytes::take(f, byte);
    to = f.code == logic::bytes::Code::f32 ? logic::bytes::single_value(static_cast<u32>(b)) : std::bit_cast<f64>(b);
  }

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
struct Blueprint : game::BlueprintNodesOf<logic::EngineNodesOf<BlueprintCore>> {};
/// A library is a Blueprint that keeps nothing: types and functions. The Blueprints that use it
/// have its types and call its functions - it is one of their bases:
///   struct Random : virtual kern::Library { ... };
///   struct Door : virtual kern::Blueprint, virtual Random { ... n5 = Random::range(Seed, 0.0, 1.0); ... };
/// (virtual: a Blueprint and the libraries it uses share what a Blueprint is.)
using Library = Blueprint;

}  // namespace kern

/// KERN_ENUM(Mode, Closed, Opening, Open); declares an enum of the Blueprint: the type Mode with
/// the values Mode::Closed ..., a value being its place among them (the first is 0). The names
/// are stated once - the enum nodes (Enum to Text ...) read them from here.
#define KERN_ENUM(Name, ...)                                                                  \
  enum class Name : ::kern::i64 { __VA_ARGS__ };                                              \
  friend constexpr ::std::string_view kern_enum_names(Name) { return #__VA_ARGS__; }         \
  static_assert(true)
/// KERN_PROJECT_ENUM(Mode, ...) is an enum of the project: declared like one of the Blueprint's,
/// and the same in every Blueprint of the level that has it (see EnumType::project, logic.h).
#define KERN_PROJECT_ENUM(Name, ...) KERN_ENUM(Name, __VA_ARGS__)
/// KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; }; declares a struct of the
/// Blueprint: the type Weapon with these fields, each with its default. It is a plain aggregate -
/// Weapon{"Knife", 25.0} gives every field in their order - and two of them are equal when all
/// their fields are (logic::structs::equal, nodes.h).
#define KERN_STRUCT(Name)                                                                              \
  struct Name;                                                                                         \
  friend bool operator==(const Name& a, const Name& b) { return ::kern::logic::structs::equal(a, b); } \
  struct Name
/// KERN_PROJECT_STRUCT(Weapon) { ... }; is a struct of the project (see KERN_PROJECT_ENUM).
#define KERN_PROJECT_STRUCT(Name) KERN_STRUCT(Name)
/// KERN_CONST Float Gravity = 9.81; is a constant: read like a variable, never changed. (One for
/// the class, not one in every Blueprint of it - and a library may have it: it keeps nothing.)
#define KERN_CONST static inline const
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
