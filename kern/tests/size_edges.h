// std::size_t at its edges, as a Blueprint's class computes it (the functions of its nodes, logic::PureNodes) and as C++ itself
// computes it: n - 1 at n = 0, a comparison, the conversions to i32 and f32 / f64. The subset's data model is LP64
// (blueprint_cpp.md, "Zahl-Typen": std::size_t is u64) - on wasm32 C++'s own size_t has 32 bits, the subset's has 64: the
// class gives the same text natively and in WebAssembly (kWant), and C++'s u64 arithmetic gives it too.
// Used by test_main.cpp (natively) and logic_wasm_test.cpp (in WebAssembly, compared in logic_wasm_test.mjs).
#pragma once

#include <string>

#include "kern/logic/logic.h"
#include "kern/logic/nodes.h"

namespace size_edges {
using namespace kern;
using P = logic::PureNodes;

inline std::string text_of(const logic::Value& v) { return logic::number_as_text(v); }

/// The class's way: each step a node's function of the subset's types (u64 for std::size_t).
inline std::string of_the_class() {
  std::string r;
  for (u64 n : {u64{0}, u64{1}, u64{2}, u64{4294967295u}, u64{4294967296u}, u64{9007199254740993u}, ~u64{0}}) {
    const u64 m = P::math_sub<u64>(n, 1);
    r += text_of(logic::Value::of_number(m)) + " " + (P::math_less<u64>(m, n) ? "<" : ">=") + " " + text_of(logic::Value::of_number(P::num_cast<i32, u64>(m))) + " " +
         text_of(logic::Value::of_number(P::num_cast<f32, u64>(m))) + " " + text_of(logic::Value::of_number(P::num_cast<f64, u64>(m))) + " " +
         text_of(logic::Value::of_number(P::num_cast<u32, u64>(P::math_mul<u64>(m, 3)))) + "\n";
  }
  return r;
}

/// C++'s own: the same steps with its operators on u64 (all defined: unsigned arithmetic goes round, a whole number to a
/// narrower one keeps its low bits, an unsigned one to a floating one is the nearest).
inline std::string of_cpp() {
  std::string r;
  for (u64 n : {u64{0}, u64{1}, u64{2}, u64{4294967295u}, u64{4294967296u}, u64{9007199254740993u}, ~u64{0}}) {
    const u64 m = n - 1;
    r += text_of(logic::Value::of_number(m)) + " " + (m < n ? "<" : ">=") + " " + text_of(logic::Value::of_number(static_cast<i32>(m))) + " " +
         text_of(logic::Value::of_number(static_cast<f32>(m))) + " " + text_of(logic::Value::of_number(static_cast<f64>(m))) + " " +
         text_of(logic::Value::of_number(static_cast<u32>(m * 3))) + "\n";
  }
  return r;
}

/// What both give, on every platform (written out once: natively and in WebAssembly the same).
inline constexpr const char* kWant =
    "18446744073709551615 >= -1 1.8446744e+19 18446744073709551616 4294967293\n"
    "0 < 0 0 0 0\n"
    "1 < 1 1 1 3\n"
    "4294967294 < -2 4294967296 4294967294 4294967290\n"
    "4294967295 < -1 4294967296 4294967295 4294967293\n"
    "9007199254740992 < 0 9.007199e+15 9007199254740992 0\n"
    "18446744073709551614 < -2 1.8446744e+19 18446744073709551616 4294967290\n";

}  // namespace size_edges
