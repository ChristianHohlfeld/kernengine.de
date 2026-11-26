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

#include <array>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "kern/logic/engine_nodes.gen.h"
#include "kern/logic/nodes.h"

namespace kern {

namespace logic {
/// A call that gives a function that may run handlers an entry of a list of any length (of a map, of an owned
/// value) - `const kern::logic::Held held_12(Cars, Cars[n4.index].Wheels); Car_step(...); held_after(held_12);`.
/// In C++ the reference would go nowhere if a handler changed how long such a list is (undefined); the language
/// checks it: before the call each list's length and where its entries are, after it the same - or the run
/// stops there with an error (BlueprintCore::held_after, Runtime::held_lost; the interpreter checks the lengths,
/// Instance::call). The lists outermost first: an inner one is looked at only while the one around it stayed.
template <class... L> struct Held {
  std::tuple<const L*...> lists;
  std::array<std::size_t, sizeof...(L)> sizes;
  std::array<const void*, sizeof...(L)> where;
  explicit Held(const L&... l) : lists(&l...), sizes{l.size()...}, where{entries_of(l)...} {}
  template <class T> static const void* entries_of(const T& l) {
    if constexpr (requires { l.data(); }) return static_cast<const void*>(l.data());
    else return nullptr;
  }
  bool same() const { return same_from<0>(); }
  template <std::size_t K> bool same_from() const {
    if constexpr (K == sizeof...(L)) {
      return true;
    } else {
      const auto* l = std::get<K>(lists);
      return l->size() == sizes[K] && entries_of(*l) == where[K] && same_from<K + 1>();
    }
  }
};
}  // namespace logic

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

/// kern::formula<u32, f32>([](auto a, auto s) { return ...; }, a's value, s's value): a Formula node - the types of its
/// inputs, the formula as the lambda's body. Each input is a logic::num::Typed of its type there, whose operators and
/// functions are those of numbers.h: the lambda computes what the node computes, bit for bit (logic.h, Formula).
template <class... T, class F> constexpr auto formula(F&& f, std::type_identity_t<T>... values) {
  return logic::num::untyped(f(logic::num::Typed<T>(values)...));
}

/// kern::out(n7.stars, n7.found): where a call on another Blueprint puts what it gave, in the
/// order of the function's outputs (func_call_on).
template <class... O> struct Out {
  std::tuple<O&...> to;
  void take(std::span<const logic::Value> results) { take(results, std::index_sequence_for<O...>{}); }
  template <std::size_t... I> void take(std::span<const logic::Value> results, std::index_sequence<I...>) {
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

namespace logic {
/// The object of a library a Callback's thunk calls the library's function on (a library keeps nothing: its
/// functions need no other).
template <class L> inline L the_library{};
namespace callbacks {
template <class T> constexpr bool gives_nothing = std::is_void_v<T> || std::is_empty_v<T>;
template <class T> auto only(T&& r) {
  auto&& [x] = r;  // (a function's one output: its Result struct holds it)
  return x;
}
template <class Res> struct given { using type = decltype(only(std::declval<Res>())); };
template <class Res> requires gives_nothing<Res> struct given<Res> { using type = void; };
template <auto F, class L, class Res, class P, class... A> Callback<typename given<Res>::type(A...)> bound(Res (L::*)(const P&, A...), const P& place) {
  using R = typename given<Res>::type;
  return {[](void* p, A... a) -> R {
            if constexpr (std::is_void_v<R>) (the_library<L>.*F)(*static_cast<const P*>(p), a...);
            else return only((the_library<L>.*F)(*static_cast<const P*>(p), a...));
          },
          const_cast<P*>(&place)};
}
template <auto F, class L, class Res, class... A> Callback<typename given<Res>::type(A...)> free(Res (L::*)(A...)) {
  using R = typename given<Res>::type;
  return {[](void*, A... a) -> R {
            if constexpr (std::is_void_v<R>) (the_library<L>.*F)(a...);
            else return only((the_library<L>.*F)(a...));
          },
          nullptr};
}
}  // namespace callbacks
}  // namespace logic

/// The nodes of the core library (src/logic/core_nodes.cpp). kern::Blueprint, below, is these and
/// the gameplay nodes.
struct BlueprintCore : logic::PureNodes {
  // (the numbers are the C++ types themselves - bool, kern::i8 ... kern::u64, kern::f32, kern::f64 - and every operation
  // on them a function of numbers.h: PureNodes)
  /// A text as a Blueprint's class holds it: one written out in the program is referred to, one
  /// made while running is held (logic::Text, nodes.h).
  using Text = logic::Text;
  /// A list of values of one type: List<Float>, List<Int> ...
  template <class T> using List = kern::List<T>;
  template <class T, std::size_t N> using Fixed = kern::Fixed<T, N>;
  /// A map of values under their keys and a set of keys, in the order the keys came: Map<Int, f64>, Set<Int>.
  template <class K, class V> using Map = logic::Map<K, V>;
  template <class K> using Set = logic::Set<K>;
  /// A function of a library the engine calls, with the place it is bound to: Callback<Float(f64, f64)>.
  template <class S> using Callback = logic::Callback<S>;
  /// Make Callback: callback<&Terrain_lib::height_at>(self) - the library's function, its first input bound to
  /// the place (a const reference: read when the engine calls); callback<&Lib::fn>() binds none.
  template <auto F, class P> static auto callback(const P& place) { return logic::callbacks::bound<F>(F, place); }
  template <auto F> static auto callback() { return logic::callbacks::free<F>(F); }

  /// The world the Blueprint runs in (the engine sets it; see logic::BlueprintWorld).
  logic::BlueprintWorld kern_world;

  // ---- the world ----
  void debug_print(const Text& text, Bool screen) {
    if (kern_world.host) kern_world.host->print(text.str(), screen);
  }
  f64 time_now() const { return kern_world.host ? kern_world.host->time() : 0.0; }
  /// Call Event: raises the Custom Event `Name` on Target's graph (on every graph: Actor{}).
  template <Setting Name> void flow_call(Actor target, Actor actor, f64 value, const Text& text) {
    logic::ActorRef to = 0;
    if (logic::Runtime* there = kern_runtime_of(target, "Call Event", to))
      there->call(Name.view(), to, {{"actor", logic::Value::actor(actor)}, {"value", logic::Value::number(value)}, {"text", logic::to_value(text)}});
  }
  /// The runtime that runs the Blueprint of an Actor, and its id there (Runtime::runtime_of: the class's own for an actor
  /// of its world, a world the program runs for a program's Blueprint; null - the host is told why - where none). A
  /// class without a runtime reaches no actor.
  logic::Runtime* kern_runtime_of(Actor a, std::string_view node, logic::ActorRef& id) const {
    id = 0;
    return kern_world.runtime ? kern_world.runtime->runtime_of(kern_world.instance ? std::string_view(kern_world.instance->name()) : std::string_view("a class"), a, node, id) : nullptr;
  }

  /// Call on Actor: runs the function `Name` of the Blueprint of `target` (Actor{}: the Level's)
  /// with `args`, and `out` takes what it gave - func_call_on<"Stars">(who, kern::out(n7.stars), x);
  template <Setting Name, class... O, class... A> void func_call_on(Actor target, Out<O...> out, const A&... args) {
    // (the arguments and what the function gives in places of this call: no memory asked for - Runtime::call_on)
    const std::array<logic::Value, sizeof...(A)> in{logic::to_value(args)...};
    std::array<logic::Value, sizeof...(O)> results{};
    std::size_t given = 0;
    if (kern_world.runtime && kern_world.instance)
      given = kern_world.runtime->call_on(*kern_world.instance, Name.view(), target, std::span<const logic::Value>(in), std::span<logic::Value>(results));
    out.take(std::span<const logic::Value>(results.data(), given));
  }

  /// Get Variable on Actor: what the variable `Name` of the Blueprint of `target` holds, as a T -
  /// func_get_on<"MaxStars", f64>(actor_find("Wanted")).
  /// (Which struct or enum it is, T says: the variable's value becomes a T as a value from outside does -
  /// what is no value of T is T's default.)
  template <Setting Name, class T> T func_get_on(Actor target) {
    logic::Value held;
    logic::ActorRef id = 0;
    logic::Runtime* there = kern_runtime_of(target, "Get Variable on Actor", id);
    if (!there || !there->get_on(id, Name.view(), logic::pin_type_of<T>(), held)) return T{};
    return logic::from_value<T>(held.converted(logic::pin_type_of<T>()));
  }
  /// Has Blueprint: does `target` (Actor{}: the Level) have a Blueprint that runs - what Call on Actor
  /// and Get Variable on Actor reach? func_has_blueprint(actor_find("Wanted")).
  Bool func_has_blueprint(Actor target) const {
    logic::ActorRef id = 0;
    logic::Runtime* there = kern_runtime_of(target, "Has Blueprint", id);
    return there && there->has_blueprint(id);
  }
  /// Engine Object of Blueprint: the object of the engine the Blueprint of `target` is as the interface I
  /// (Runtime::engine_object: what fails is told to the host, as in the interpreter) -
  /// func_engine_object<kern::ShaderExtension>(actor_find("Water")). A class without a world has none, and says so.
  template <class I> Object<I> func_engine_object(Actor target) const {
    static const std::string interface = logic::engine_class_name(logic::cpp_name_of<I>());
    if (kern_world.runtime && kern_world.instance) return Object<I>{kern_world.runtime->engine_object(*kern_world.instance, target, interface)};
    if (kern_world.host) kern_world.host->error("Engine Object of Blueprint: a class without a runtime has no Blueprints to ask");
    return {};
  }
  /// Is Valid Object, Handle of Object (kern/docs/engine_object.md).
  template <class I> static Bool object_valid(Object<I> object) { return object.valid(); }
  template <class I> static Int object_handle(Object<I> object) { return object.ref.handle; }
  template <class I> static Bool object_equal(Object<I> a, Object<I> b) { return a == b; }
  /// Same Actor, Id of Actor (kern/docs/project.md 7): the same world and id; its id as an Int, the one way to a number.
  static Bool actor_equal(Actor a, Actor b) { return a == b; }
  static Int actor_id(Actor a) { return static_cast<i64>(a.id); }

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
  template <class N> f64 math_random(N& n, f64 min, f64 max) { return logic::random::number(n.memory, kern_seed(), N::id, min, max); }
  template <class N> Int math_random_int(N& n, Int min, Int max) { return logic::random::whole_number(n.memory, kern_seed(), N::id, min, max); }
  template <class N> Bool math_random_bool(N& n, f64 chance) { return logic::random::chance(n.memory, kern_seed(), N::id, chance); }

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
  void kern_wait(u32 node, f64 seconds) {
    if (kern_world.runtime) kern_world.runtime->wait(*kern_world.instance, node, seconds, logic::Context::kResume);
  }
  void kern_cancel(u32 node) {
    if (kern_world.runtime) kern_world.runtime->cancel_waits(*kern_world.instance, node);
  }
  /// flow_delay(n, seconds, [this] { ... }): waits, then goes on. A call while it waits is ignored.
  template <class N, class F> void flow_delay(N& n, f64 duration, F&& then) {
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
  template <class N, class F> void flow_retrigger_delay(N& n, f64 duration, F&& then) {
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
  template <Setting Ease, class N> void flow_tween(N& n, std::string_view entry, f64 duration, std::initializer_list<Path> paths) {
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
  /// After a call that held an entry of a list (logic::Held): did the lists stay? Else the run stops here with an error.
  template <class... L> void held_after(const logic::Held<L...>& held) {
    if (!held.same() && kern_world.runtime && kern_world.instance) kern_world.runtime->held_lost(*kern_world.instance);
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
  /// List to Text: the items as the interpreter writes a list ([1, 2.5, 3]).
  template <class T> static Text list_text(logic::Items<T> list) { return Text(logic::to_value(std::vector<T>(list.begin(), list.end())).as_text()); }
  // (an item is sought as C++ compares it: of a list of U8 the number, an Int - logic::Sought)
  template <class T> static Int list_find(logic::Items<T> list, const logic::Sought<T>& item) {
    for (std::size_t k = 0; k < list.size(); ++k)
      if (logic::equal_item(list.first[k], item)) return static_cast<i64>(k);
    return -1;
  }
  template <class T> static Bool list_contains(logic::Items<T> list, const logic::Sought<T>& item) { return list_find<T>(list, item) >= 0; }
  // (the same on a place of the engine - the list the engine holds: read where it stands, as the interpreter
  // reads it; its length without reading it)
  template <class X> static decltype(auto) read_of(const X& x) {
    if constexpr (logic::engine_values::NamesPlace<X>) return x.ref().kern_read();
    else return (x);
  }
  template <class T, logic::engine_values::NamesPlace P> static Int list_length(const P& place) { return static_cast<i64>(place.ref().kern_count()); }
  template <class T, logic::engine_values::NamesPlace P> static T list_get(const P& place, Int index) { return list_get<T>(read_of(place), index); }
  template <class T, logic::engine_values::NamesPlace P> static Int list_find(const P& place, const logic::Sought<T>& item) { return list_find<T>(read_of(place), item); }
  template <class T, logic::engine_values::NamesPlace P> static Bool list_contains(const P& place, const logic::Sought<T>& item) { return list_find<T>(read_of(place), item) >= 0; }
  /// Lists Equal: as many items, and each equal to the one at its place (the == of two std::vector).
  template <class T, class A, class B>
    requires(logic::engine_values::NamesPlace<A> || logic::engine_values::NamesPlace<B>)
  static Bool list_equal(const A& list, const B& other) { return list_equal<T>(logic::Items<T>(read_of(list)), logic::Items<T>(read_of(other))); }
  template <class T> static Bool list_equal(logic::Items<T> list, logic::Items<T> other) {
    if (list.size() != other.size()) return false;
    for (std::size_t k = 0; k < list.size(); ++k)
      if (!(list.first[k] == other.first[k])) return false;
    return true;
  }
  template <class T> static void list_add(List<T>& list, std::type_identity_t<T> item) { logic::list::add(list, std::move(item)); }
  template <class N, class T> static void list_add(N& n, List<T>& list, std::type_identity_t<T> item) { n.index = logic::list::add(list, std::move(item)); }
  template <class T> static void list_insert(List<T>& list, Int index, std::type_identity_t<T> item) { logic::list::insert(list, index, std::move(item)); }
  template <class T> static void list_remove_at(List<T>& list, Int index) { logic::list::remove_at(list, index); }
  template <class T> static void list_remove(List<T>& list, const logic::Sought<T>& item) { logic::list::remove(list, item); }
  template <class N, class T> static void list_remove(N& n, List<T>& list, const logic::Sought<T>& item) { n.removed = logic::list::remove(list, item); }
  template <class T> static void list_clear(List<T>& list) { logic::list::clear(list); }
  /// (in one go: what std::vector does in one request for memory - resize, insert of a range, reserve)
  template <class T> static void list_resize(List<T>& list, Int length, const std::type_identity_t<T>& value) { logic::list::resize(list, length, value); }
  // (the same steps on a place of the engine - the list the engine holds: logic::engine_values::list_add ...)
  template <logic::engine_values::NamesPlace P> static Int list_length(const P& place) { return static_cast<i64>(place.ref().kern_count()); }
  template <logic::engine_values::NamesPlace P, class I> static void list_add(const P& place, const I& item) { logic::engine_values::list_add(place, item); }
  template <class N, logic::engine_values::NamesPlace P, class I> static void list_add(N& n, const P& place, const I& item) { n.index = logic::engine_values::list_add(place, item); }
  template <logic::engine_values::NamesPlace P, class I> static void list_insert(const P& place, Int index, const I& item) { logic::engine_values::list_insert(place, index, item); }
  template <logic::engine_values::NamesPlace P> static void list_remove_at(const P& place, Int index) { logic::engine_values::list_remove_at(place, index); }
  template <logic::engine_values::NamesPlace P, class I> static void list_remove(const P& place, const I& item) { logic::engine_values::list_remove(place, item); }
  template <class N, logic::engine_values::NamesPlace P, class I> static void list_remove(N& n, const P& place, const I& item) { n.removed = logic::engine_values::list_remove(place, item); }
  template <logic::engine_values::NamesPlace P> static void list_clear(const P& place) { logic::engine_values::list_clear(place); }
  template <logic::engine_values::NamesPlace P, class I> static void list_resize(const P& place, Int length, const I& value) { logic::engine_values::list_resize(place, length, value); }
  // ---- maps and sets ----
  // What they do is logic::map and logic::set (nodes.h) - on the same container the interpreter's
  // nodes use. The values take any map or set - its keys' and values' types are their settings,
  // map_find<Int, f64>(Cells, 7); the steps change one at a place - the place is their first value.
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

  /// Sort List: list_sort<&Board::Before>(*this, Scores) - in the order a function of the Blueprint says
  /// (it takes two items and gives whether the first goes before the second; *this: the object it runs
  /// on - the class the call stands in, also where that is a library other Blueprints use);
  /// list_sort<nullptr>(Scores): by the items' `<`. Stable: equal items keep their order.
  template <auto Less, class T> static void list_sort(List<T>& list) {
    static_assert(std::is_null_pointer_v<decltype(Less)>, "an order of the Blueprint runs on its object: list_sort<&Board::Before>(*this, list)");
    logic::list::sort(list);
  }
  template <auto Less, class Self, class T> static void list_sort(Self& self, List<T>& list) {
    logic::list::sort(list, [&self](const auto& a, const auto& b) -> bool {
      const auto [before] = (self.*Less)(a, b);
      return before;
    });
  }
  template <logic::engine_values::NamesPlace P, class L> static void list_append(const P& place, const L& items) {
    using BI = typename std::remove_pointer_t<decltype(decltype(place.ref())::kern_own)>::value_type;
    logic::engine_values::list_append(place, logic::Items<BI>(read_of(items)));
  }
  template <class T, logic::engine_values::NamesPlace P> static void list_append(List<T>& list, const P& items) { list_append<T>(list, read_of(items)); }
  template <class T, logic::engine_values::NamesPlace P> static void list_insert_list(List<T>& list, Int index, const P& items) { list_insert_list<T>(list, index, read_of(items)); }
  template <class T> static void list_append(List<T>& list, std::type_identity_t<logic::Items<T>> items) {
    logic::list::insert_items(list, static_cast<i64>(list.size()), items.first, items.count);
  }
  template <class T> static void list_insert_list(List<T>& list, Int index, std::type_identity_t<logic::Items<T>> items) { logic::list::insert_items(list, index, items.first, items.count); }
  template <class T> static void list_reserve(List<T>& list, Int count) { logic::list::reserve(list, count); }
  /// Copy Bytes: list_copy_bytes<"f32", "u8">(Grid, at, Bytes, from, count) - `count` bytes of the numbers of
  /// Bytes (each the number of the second setting) from its entry `from` on into the entries of the place
  /// (each the number of the first) from `at` on: what std::memcpy between lists of numbers does
  /// (logic::bytes::copy, nodes.h - the interpreter's function). On a list or a list of a fixed length.
  // (Items: a list of numbers of the second setting's type, as they are)
  template <Setting Into, Setting Of, class T, class L>
  static void list_copy_bytes(List<T>& list, Int at, const L& items, Int from, Int count) {
    copy_bytes<Into, Of, T>(list.data(), list.size(), at, logic::items_of(items), from, count);
  }
  template <Setting Into, Setting Of, class T, std::size_t N, class L>
  static void list_copy_bytes(Fixed<T, N>& list, Int at, const L& items, Int from, Int count) {
    copy_bytes<Into, Of, T>(list.items.data(), N, at, logic::items_of(items), from, count);
  }
  template <Setting Into, Setting Of, class L, logic::engine_values::NamesPlace P>
  static void list_copy_bytes(L& list, Int at, const P& items, Int from, Int count) {
    list_copy_bytes<Into, Of>(list, at, read_of(items), from, count);
  }
  template <Setting Into, Setting Of, class T, class Item, class S>
  static void copy_bytes(Item* to, std::size_t size, Int at, const logic::Items<S>& items, Int from, Int count) {
    static constexpr logic::bytes::Code kInto = logic::bytes::code_of(Into.view()), kOf = logic::bytes::code_of(Of.view());
    static_assert(kInto != logic::bytes::Code::none && kInto != logic::bytes::Code::boolean && kOf != logic::bytes::Code::none && kOf != logic::bytes::Code::boolean,
                  "Copy Bytes: a number is one of u8 i8 u16 i16 u32 i32 u64 i64 f32 f64");
    static_assert(logic::bytes::suits_list(kInto, logic::pin_type_of<T>()), "Copy Bytes: the place's numbers are of the first setting's type");
    static_assert(logic::bytes::suits_list(kOf, logic::pin_type_of<S>()), "Copy Bytes: the numbers of Items are of the second setting's type");
    logic::bytes::copy(to, size, kInto, at, items.first, items.count, kOf, from, count);
  }

  // ---- variables ----
  // Get, Set and Add are C++'s own (Heat, Heat = value, Heat += value). A Take gives what
  // stands at a place and leaves the zero of its type there - std::exchange(place, {}): a list,
  // a text is handed on, not copied.
  template <class T>
    requires(!logic::engine_values::NamesPlace<T>)
  static void var_take(T& place) { place = T{}; }
  template <class N, class T>
    requires(!logic::engine_values::NamesPlace<T>)
  static void var_take(N& n, T& place) { n.value = std::exchange(place, T{}); }
  // (a place of the engine: logic::engine_values::take)
  template <logic::engine_values::NamesPlace P> static void var_take(const P& place) { logic::engine_values::take(place); }
  template <class N, logic::engine_values::NamesPlace P> static void var_take(N& n, const P& place) { n.value = logic::engine_values::take(place); }

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
  /// (a text handed on - std::move, `Log = text_append(std::move(Log), x, "")` - that holds its characters
  /// gets the others appended where it is, as std::string's += appends: in the room it has, without a copy)
  /// (Text::Appending: set to the text handed on itself, it appends where that text is)
  static Text::Appending text_append(Text&& a, const Text& b, const Text& c) { return {a, b, c}; }
  static Bool text_equal(const Text& a, const Text& b) { return a == b; }
  static Bool text_contains(const Text& a, const Text& b) { return a.view().find(b.view()) != std::string_view::npos; }
  static Int text_length(const Text& a) { return static_cast<i64>(a.size()); }
  /// (Text to u64: of the text where it stands)
  static u64 num_text_to_u64(const Text& a) { return logic::PureNodes::int64_parse(a.view()); }
  static Int text_code_at(const Text& a, Int index) {
    const std::string_view v = a.view();
    return index >= 0 && static_cast<u64>(index) < v.size() ? static_cast<i64>(static_cast<u8>(v[static_cast<std::size_t>(static_cast<i64>(index))])) : 0;
  }
  /// Set Code: text_set_code(text, index, code) - the character at the index becomes the one of that code (0 .. 255; the
  /// low byte), where it stands: what `s[i] = c` does to a std::string. An index outside the text changes nothing.
  static void text_set_code(Text& text, Int index, Int code) { text.set_at(static_cast<i64>(index), static_cast<char>(static_cast<u8>(static_cast<i64>(code)))); }
  static Text text_of_code(Int code) { return Text(1, static_cast<char>(static_cast<u8>(static_cast<i64>(code)))); }
  // (what std::string and std::string_view do, on the characters where they are - logic::texts)
  static Int text_find(const Text& a, const Text& b, Int from) { return logic::texts::find(a.view(), b.view(), from); }
  static Int text_rfind(const Text& a, const Text& b, Int from) { return logic::texts::rfind(a.view(), b.view(), from); }
  static Int text_find_first_of(const Text& a, const Text& b, Int from) { return logic::texts::find_first_of(a.view(), b.view(), from); }
  static Int text_find_last_of(const Text& a, const Text& b, Int from) { return logic::texts::find_last_of(a.view(), b.view(), from); }
  static Int text_find_first_not_of(const Text& a, const Text& b, Int from) { return logic::texts::find_first_not_of(a.view(), b.view(), from); }
  static Int text_find_last_not_of(const Text& a, const Text& b, Int from) { return logic::texts::find_last_not_of(a.view(), b.view(), from); }
  /// (a part of a text is a text of its own: its characters are copied, as std::string::substr copies them)
  static Text text_sub(const Text& a, Int at, Int count) { return Text(logic::texts::sub(a.view(), at, count)); }
  static Bool text_starts_with(const Text& a, const Text& b) { return a.view().starts_with(b.view()); }
  static Bool text_ends_with(const Text& a, const Text& b) { return a.view().ends_with(b.view()); }
  static Bool text_less(const Text& a, const Text& b) { return a.view() < b.view(); }
  /// (Insert, Erase, Replace change the text they are handed - one handed on with std::move is changed
  /// where it is, as std::string::insert changes it: no copy)
  static Text text_insert(Text a, Int at, const Text& b) {
    std::string t = std::move(a).str();
    logic::texts::insert(t, at, b.view());
    return t;
  }
  static Text text_erase(Text a, Int at, Int count) {
    std::string t = std::move(a).str();
    logic::texts::erase(t, at, count);
    return t;
  }
  static Text text_replace(Text a, Int at, Int count, const Text& b) {
    std::string t = std::move(a).str();
    logic::texts::replace(t, at, count, b.view());
    return t;
  }
  /// Text of Bytes: the text a list of bytes is, from At on (one request for memory, as std::string makes it).
  /// (of a list of Ints of any width - Int, U8 ... I32: each number's last 8 bits; of a place of the engine: read where it stands)
  template <class L> static Text text_of_bytes(const L& bytes, Int at, Int count) { return logic::texts::of_bytes(logic::items_of(read_of(bytes)), at, count); }
  static Text text_repeat(const Text& a, Int count) { return logic::texts::repeat(a.view(), count); }
  /// Bytes of Text: its bytes, a List<U8> (one request for memory, as std::vector<u8> of the text's range makes it).
  static List<u8> text_bytes(const Text& a) {
    List<u8> r;
    logic::texts::insert_bytes(r, 0, a.view());
    return r;
  }
  /// Insert Text Bytes: the bytes of a text into a list where it stands (std::vector::insert of the text's range).
  /// (into a list of Ints of a width: each byte as C++ stores it there - an I8 holds 255 as -1)
  template <class T> static void list_insert_text(List<T>& list, Int index, const Text& text) { logic::texts::insert_bytes(list, index, text.view()); }
  /// Reserve Text: the same text, with room for Count characters (std::string::reserve, at most 2^24) - handed on
  /// with std::move, where it is: the appends after it fill that room.
  static Text text_reserve(Text a, Int count) {
    if (count <= static_cast<i64>(a.size())) return a;
    std::string t = std::move(a).str();
    t.reserve(static_cast<std::size_t>(std::min<i64>(count, logic::texts::kLongest)));
    return t;
  }
  /// Parse f64, Parse Int: the number at a place of a text as C reads it there (logic::scan).
  struct ParsedFloat {
    f64 value = 0.0;
    Int end = 0;
  };
  struct ParsedFloat32 {
    f32 value = 0.0f;
    Int end = 0;
  };
  struct ParsedWhole {
    i64 value = 0;
    Int end = 0;
  };
  struct ParsedUnsigned {
    u64 value = 0;
    Int end = 0;
  };
  static ParsedFloat text_parse_float(const Text& a, Int at) {
    const logic::scan::Real r = logic::scan::real(logic::scan::c_string(a.view(), at), false);
    return {r.value, static_cast<i64>(at) + static_cast<i64>(r.end)};
  }
  static ParsedFloat32 text_parse_float32(const Text& a, Int at) {
    const logic::scan::Real r = logic::scan::real(logic::scan::c_string(a.view(), at), true);
    return {static_cast<f32>(r.value), static_cast<i64>(at) + static_cast<i64>(r.end)};
  }
  static ParsedWhole text_parse_int(const Text& a, Int at, Int base) {
    const logic::scan::Whole r = logic::scan::whole(logic::scan::c_string(a.view(), at), base < 0 || base > 36 ? -1 : static_cast<int>(static_cast<i64>(base)), true);
    return {static_cast<i64>(r.bits), static_cast<i64>(at) + static_cast<i64>(r.end)};
  }
  static ParsedUnsigned text_parse_uint(const Text& a, Int at, Int base) {
    const logic::scan::Whole r = logic::scan::whole(logic::scan::c_string(a.view(), at), base < 0 || base > 36 ? -1 : static_cast<int>(static_cast<i64>(base)), false);
    return {r.bits, static_cast<i64>(at) + static_cast<i64>(r.end)};
  }
  /// Scan Text: text_scan<"%f %f">(n, text, at, x, y) - what sscanf reads (logic::scan), a step: n.count, and
  /// n.v0, n.v1 ... a value for each conversion, of the type the format says (`before`: what stays where the
  /// scan does not reach). Without n: nothing it gives is read.
  template <Setting Format> static constexpr logic::scan::Plan scan_plan() {
    logic::scan::Plan p;
    if (!logic::scan::plan(Format.view(), &p, nullptr)) p = {};
    return p;
  }
  /// (what a conversion had before, into its slot - a text stays where it is: it is taken from there when
  /// the scan does not write it)
  template <logic::scan::Kind K, class V> static void scan_before(logic::scan::Slot& s, const V& v) {
    s.kind = K;
    if constexpr (K == logic::scan::Kind::whole) s.whole = static_cast<i64>(Int(v));
    else if constexpr (K == logic::scan::Kind::whole64) s.bits = static_cast<u64>(v);
    else if constexpr (K == logic::scan::Kind::number) s.number = static_cast<f64>(v);
  }
  template <logic::scan::Kind K, class F, class V> static void scan_after(F& field, logic::scan::Slot& s, const V& before) {
    if constexpr (K == logic::scan::Kind::whole) field = Int(s.whole);
    else if constexpr (K == logic::scan::Kind::whole64) field = s.bits;
    else if constexpr (K == logic::scan::Kind::number) field = s.number;
    else if (s.written) field = Text(std::move(s.text));
    else field = Text(before);
  }
  template <std::size_t K, class N> static auto& scan_value(N& n) {
    if constexpr (K == 0) return n.v0;
    else if constexpr (K == 1) return n.v1;
    else if constexpr (K == 2) return n.v2;
    else if constexpr (K == 3) return n.v3;
    else if constexpr (K == 4) return n.v4;
    else if constexpr (K == 5) return n.v5;
    else if constexpr (K == 6) return n.v6;
    else if constexpr (K == 7) return n.v7;
    else if constexpr (K == 8) return n.v8;
    else if constexpr (K == 9) return n.v9;
    else if constexpr (K == 10) return n.v10;
    else if constexpr (K == 11) return n.v11;
    else if constexpr (K == 12) return n.v12;
    else if constexpr (K == 13) return n.v13;
    else if constexpr (K == 14) return n.v14;
    else return n.v15;
  }
  template <Setting Format, class N, class... V, std::size_t... I> static void text_scan_into(N* n, const Text& a, Int at, std::index_sequence<I...>, const V&... before) {
    static constexpr logic::scan::Plan kPlan = scan_plan<Format>();
    static_assert(kPlan.count == sizeof...(V), "Scan Text: a value before for each value of the format");
    logic::scan::Slot slots[sizeof...(V) + 1];
    (scan_before<kPlan.kinds[I]>(slots[I], before), ...);
    const i64 count = logic::scan::run(logic::scan::c_string(a.view(), at), Format.view(), slots, sizeof...(V));
    if constexpr (!std::is_same_v<N, void>) {
      n->count = count;
      (scan_after<kPlan.kinds[I]>(scan_value<I>(*n), slots[I], before), ...);
    }
  }
  template <Setting Format, class N, class... V> requires(!std::is_convertible_v<const N&, const Text&>)
  static void text_scan(N& n, const Text& a, Int at, const V&... before) {
    text_scan_into<Format>(&n, a, at, std::index_sequence_for<V...>{}, before...);
  }
  template <Setting Format, class... V> static void text_scan(const Text& a, Int at, const V&... before) {
    text_scan_into<Format, void>(static_cast<void*>(nullptr), a, at, std::index_sequence_for<V...>{}, before...);
  }
  /// Format Text: text_format<"%02d:%06.3f">(most, minutes, seconds) - the values in the order of
  /// the format's conversions (logic::format).
  template <Setting Format, class... V> static Text text_format(Int most, const V&... values) {
    const logic::format::Arg args[] = {format_arg(values)..., logic::format::Arg{}};
    return logic::format::text_of(Format.view(), args, sizeof...(V), most);
  }
  template <class T>
    requires(logic::num::Number<T> && !std::is_same_v<T, bool>)
  static logic::format::Arg format_arg(T v) {
    return logic::format::arg(v);   // (a whole number: its bits, as C's varargs; an f32 promoted to a double)
  }
  static logic::format::Arg format_arg(const Text& v) { return logic::format::arg(v.view()); }
  /// (a text that is no Text - what int64_text, int64_text_signed ... give: a view of that value itself, which
  /// lives until text_format ends; made into a Text first, the view would be of a Text that is gone)
  static logic::format::Arg format_arg(const std::string& v) { return logic::format::arg(std::string_view(v)); }
  static logic::format::Arg format_arg(std::string_view v) { return logic::format::arg(v); }
  static logic::format::Arg format_arg(const char* v) { return logic::format::arg(std::string_view(v)); }  // ("abc": the characters where they are)

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
  // (in parentheses, C++20: a number written out goes into a field of any number type that holds it - the notation has
  // checked that it does, struct_make<Frame>(1447121230, 0) for a u32 and a u16 - as it does in braces where it stands)
  template <class S, class... V> static S struct_make(V&&... fields) { return S(std::forward<V>(fields)...); }
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
    Fixed<u8, kLayout.size> out;   // (the bytes: std::array<u8, N>)
    std::size_t k = 0;
    logic::structs::fields(value, [&](const auto&... field) { (put_bytes(out.items.data(), kLayout.fields[k++], field), ...); });
    return out;
  }
  template <class S, Setting Layout, class L> static S struct_of_bytes(const L& list, Int at) {
    decltype(auto) held = read_of(list);   // (a place of the engine: read where it stands)
    const auto bytes = logic::items_of(held);  // (a list of Ints of any width: each number's last 8 bits)
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
  // (a field's own bits: it is of the layout's number type - numbers.h's bits_of)
  template <class B, class T>
    requires logic::num::Number<T>
  static void put_bytes(B* out, const logic::bytes::Field& f, T v) {
    logic::bytes::put(out, f, logic::num::bits_of(v));
  }
  template <class B, class T>
    requires logic::num::Number<T>
  static void take_bytes(T& to, const logic::bytes::Field& f, B&& byte) {
    to = logic::num::of_bits<T>(logic::bytes::take(f, byte));
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
  // (the operator is known when the class is compiled: the same expressions as compare, chosen then)
  template <Setting Op, class T> static Bool math_compare(std::type_identity_t<T> a, std::type_identity_t<T> b) {
    if constexpr (Op.view() == "==") return logic::num::eq(a, b);
    else if constexpr (Op.view() == "!=") return logic::num::ne(a, b);
    else if constexpr (Op.view() == "<") return logic::num::lt(a, b);
    else if constexpr (Op.view() == "<=") return logic::num::le(a, b);
    else if constexpr (Op.view() == ">") return logic::num::gt(a, b);
    else return logic::num::ge(a, b);
  }
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
// (kern::Implements<kern::ShaderExtension>, the base of a Blueprint that is the object of an interface of the engine:
// kern/logic/engine_direct.h)
namespace logic {
/// The engine's object of the interface the class of a Blueprint implements (its last base kern::Implements<I>):
/// each function the engine calls is the class's of that name, called typed - what the engine hands it made into the
/// class's types (an object of the engine its handle in the class's EngineObjects), what it gives into the engine's.
/// The class must outlive it. (The interpreter's way: logic::engine_implement.)
template <class B> auto implementation(B& blueprint) {
  using Adapter = typename decltype(engine_direct::interface_of(blueprint))::template adapter<B>;
  return std::make_unique<Adapter>(blueprint);
}
}  // namespace logic

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
/// KERN_STRUCT(Weapon) { Text Name = ""; f64 Damage = 10.0; }; declares a struct of the
/// Blueprint: the type Weapon with these fields, each with its default. It is a plain aggregate -
/// Weapon{"Knife", 25.0} gives every field in their order - and two of them are equal when all
/// their fields are (logic::structs::equal, nodes.h).
#define KERN_STRUCT(Name)                                                                              \
  struct Name;                                                                                         \
  friend bool operator==(const Name& a, const Name& b) { return ::kern::logic::structs::equal(a, b); } \
  struct Name
/// KERN_PROJECT_STRUCT(Weapon) { ... }; is a struct of the project (see KERN_PROJECT_ENUM).
#define KERN_PROJECT_STRUCT(Name) KERN_STRUCT(Name)
/// KERN_CONST f64 Gravity = 9.81; is a constant: read like a variable, never changed. (One for
/// the class, not one in every Blueprint of it - and a library may have it: it keeps nothing.)
#define KERN_CONST static inline const
/// A function that is offered as a value node (no exec pins).
#define KERN_PURE
/// KERN_OVERRIDE void update(f64 dt = 0.0) { ... }: a function of the interface of the engine the Blueprint
/// implements (kern::Implements<kern::App>) - the engine calls it (Function::overrides). A function of an interface's
/// name without it, and one with it that the interface has not (a typo), are errors of the Blueprint (check).
#define KERN_OVERRIDE
/// KERN_EXTERN FnResult Fn(f64 x = 0.0); says what a function of another Blueprint looks like:
/// it has no body here and is called on an actor (func_call_on<"Fn">).
#define KERN_EXTERN
/// KERN_EXTERN_AS("Update") ZombiesUpdateResult Zombies_Update(...); the same, for a function the other
/// Blueprint has under another name (Call on Actor calls it by that one): two actors' Blueprints may each
/// have an "Update" that looks different.
#define KERN_EXTERN_AS(name)
/// KERN_EXPORT_AS("game_hud", "i32 text -> text") game_hudResult game_hud(...) { ... }: the program gives the
/// function to its host under that C name (logic::Function::exported); the glue the class has writes the export.
#define KERN_EXPORT_AS(name, wire)
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
