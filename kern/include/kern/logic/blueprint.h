// Kern Logic: what a Blueprint's C++ is compiled against (docs/blueprint_cpp.md).
//
// A Blueprint written as C++ (logic::to_cpp) is a struct that derives from kern::Blueprint. Its
// variables are its members, its functions its methods; every node it uses is a function here -
// typed, inline, the very function the interpreter's node calls (nodes.h). Nothing is looked up
// at run time.
//
// So far this covers what a Blueprint of functions needs: variables, functions, formulas, the
// value nodes, Branch / Sequence / While / For, Do Once, Do N, Gate, Flip Flop, the switches.
// Events, waits, state machines and the gameplay nodes follow (logic::native_ready tells).
#pragma once

#include <cstddef>
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
/// KERN_LAYOUT(Name) { node(id, x, y); ... }: the layout of the Blueprint Name.
#define KERN_LAYOUT(Name)                \
  struct Name##_Layout : ::kern::Layout { \
    void place();                        \
  };                                     \
  inline void Name##_Layout::place()
