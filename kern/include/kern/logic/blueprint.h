// Kern Logic: what a Blueprint's C++ is compiled against (docs/blueprint_cpp.md).
//
// A Blueprint written as C++ (logic::to_cpp) is a struct that derives from kern::Blueprint. Its
// variables are its members, its functions its methods; every node it uses is a function here -
// typed, inline, the very function the interpreter's node calls (nodes.h). Nothing is looked up
// at run time.
//
// So far this covers: variables, functions, formulas, the value nodes, Branch / Sequence / While /
// For, Do Once, Do N, Gate, Flip Flop, the switches; the events (Begin Play, Tick, Every N
// Seconds, Custom Event, the state events), Call Event, Print, Game Time; Delay, Retriggerable
// Delay, Tween; state machines. Random and the gameplay nodes follow (logic::native_ready tells).
#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <string>
#include <string_view>
#include <type_traits>

#include "kern/logic/nodes.h"

namespace kern {

/// A node's setting as a template argument: flow_switch_int<"3">, math_compare<"<=">.
template <std::size_t N> struct Setting {
  char text[N]{};
  constexpr Setting(const char (&s)[N]) {  // NOLINT (implicit: that is the point)
    for (std::size_t i = 0; i < N; ++i) text[i] = s[i];
  }
  constexpr std::string_view view() const { return {text, N - 1}; }
};

/// The C++ type of a pin type named in a setting ("float", "int", "bool", "vector", "text", "actor").
template <Setting Type>
using pin_t = std::conditional_t<
    Type.view() == "bool", bool,
    std::conditional_t<Type.view() == "int", i64,
                       std::conditional_t<Type.view() == "vector", Vec3,
                                          std::conditional_t<Type.view() == "text", std::string, std::conditional_t<Type.view() == "actor", logic::ActorHandle, f64>>>>>;

/// What a node keeps between runs: the member n<id> of a node with memory (a loop's index, Do
/// Once, Gate ...). The same four numbers the interpreter keeps for the node.
template <u32 Id> struct Node {
  static constexpr u32 id = Id;
  f64 memory[4] = {0, 0, 0, 0};
  /// A node that waits: what follows it (by exec output), kept until the wait is over.
  std::function<void(int)> later;
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

/// Where the nodes are in the graph view (KERN_LAYOUT): for the editor, nothing at run time.
struct Layout {
  void node(u32, f64, f64, const char* = "") {}
  void box(u32, f64, f64, f64, f64, const char*, u32 = 0) {}
  void next_id(u32) {}
};

struct Blueprint : logic::PureNodes {
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
    flow_tween_resume<Ease>(n);
  }
  template <Setting Ease, class N> void flow_tween_resume(N& n) {
    if (!logic::flow::tween_runs(n.memory)) return;
    bool finished = false;
    n.alpha = logic::flow::tween_step(n.memory, time_now(), Ease.view(), finished);
    const auto later = n.later;
    if (!finished) kern_wait(N::id, 0);  // again next step
    if (!later) return;
    later(0);
    if (finished) later(1);
  }

  // ---- loops ----
  /// while (loop(condition)): a While Loop.
  bool loop(Bool condition) { return condition; }
  /// for (auto it = flow_for(n, first, last); it.next();): a For Loop; n.index is the index.
  template <class N> struct ForLoop {
    N& n;
    bool first = true;
    bool next() {
      if (!first) logic::flow::for_step(n.memory);
      first = false;
      if (!logic::flow::for_runs(n.memory)) return false;
      n.index = logic::flow::for_index(n.memory);
      return true;
    }
  };
  template <class N> ForLoop<N> flow_for(N& n, Int first, Int last) {
    logic::flow::for_start(n.memory, first, last);
    return {n};
  }

  // ---- steps ----
  void flow_reroute() {}
  /// A Return nothing leads to (a method of its own that nothing calls).
  template <Setting Function, class... V> static void func_return(const V&...) {}
  /// if (flow_do_once(n, "exec", closed)): the call goes through once ("reset": again).
  template <class N> bool flow_do_once(N& n, std::string_view entry, Bool closed) { return logic::flow::do_once(n.memory, entry == "reset" ? 1 : 0, closed); }
  template <class N> bool flow_do_n(N& n, std::string_view entry, Int count) { return logic::flow::do_n(n.memory, entry == "reset" ? 1 : 0, count, n.counter); }
  template <class N> bool flow_gate(N& n, std::string_view entry, Bool closed) {
    return logic::flow::gate(n.memory, entry == "open" ? 1 : entry == "close" ? 2 : entry == "toggle" ? 3 : 0, closed);
  }
  /// switch (flow_flip_flop(n)): case 0 is A, case 1 is B.
  template <class N> int flow_flip_flop(N& n) { return logic::flow::flip_flop(n.memory, n.is_a); }
  template <Setting Count> static int flow_switch_int(Int selection) { return logic::flow::switch_int(logic::setting_count(Count.view(), 3, 1, 32), selection); }
  template <Setting Cases> static int flow_switch_text(const Text& selection) { return logic::flow::switch_text(Cases.view(), selection); }

  // ---- values with a setting ----
  template <Setting Op> static Bool math_compare(Float a, Float b) { return compare(Op.view(), a, b); }
  template <Setting Type> static pin_t<Type> logic_select(Bool pick, pin_t<Type> a, pin_t<Type> b) { return pick ? a : b; }
  template <Setting Type> static pin_t<Type> value_reroute(pin_t<Type> in) { return in; }
};

}  // namespace kern

/// A function that is offered as a value node (no exec pins).
#define KERN_PURE
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
