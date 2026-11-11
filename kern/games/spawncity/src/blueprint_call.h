// How the game's C++ calls a rule that is a Blueprint of the level (wanted.cpp, combat_detail.cpp,
// guided.cpp ...): the running Blueprint of one actor, its functions by name with their values,
// its variables. Nothing is computed here. (This goes when the callers are Blueprints themselves.)
#pragma once
#include <string>
#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::rules {

struct Blueprint {
  using Value = kern::logic::Value;
  kern::logic::Instance* running = nullptr;  // (the game binds it once its world has started; tests bind the one they run)

  /// Calls a function; false (and no results) while no Blueprint is bound or it has no such function.
  bool call(const char* function, const std::vector<Value>& args, std::vector<Value>& results) const {
    results.clear();
    return running && running->call(function, args, results);
  }
  /// A function's first result as a number (0 without one).
  f64 number(const char* function, const std::vector<Value>& args) const {
    std::vector<Value> r;
    return call(function, args, r) && !r.empty() ? r[0].as_number() : 0;
  }
  bool flag(const char* function, const std::vector<Value>& args) const {
    std::vector<Value> r;
    return call(function, args, r) && !r.empty() && r[0].b;
  }
  f64 variable(const char* name) const { return running ? running->get(name).as_number() : 0; }
};

/// A value of one of the Blueprint's enums: the C++ enum has the same values in the same order.
template <class E> kern::logic::Value of_enum(E e) { return kern::logic::Value::enumeration(static_cast<i64>(e)); }

}  // namespace sc::rules
