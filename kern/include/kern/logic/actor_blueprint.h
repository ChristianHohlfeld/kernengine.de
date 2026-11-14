// How a program's own C++ calls a function of the Blueprint of one of the level's actors: what Call
// on Actor is in a Blueprint (docs/blueprint_cpp.md, "Aufrufe aus C++"). The C++ names the actor,
// the program binds its running Blueprint once the world has started, and a call gives its values
// in and takes the function's outputs back, typed:
//
//   kern::logic::ActorBlueprint rules{"Military"};
//   f64 chance = 0;
//   rules.call("HitChance", std::tie(chance), distance, speed);
//
// is, written as a Blueprint, func_call_on<"HitChance">(actor_find("Military"), kern::out(n7.chance),
// distance, speed); - the same call in every stage.
#pragma once

#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "kern/logic/logic.h"

namespace kern::logic {

struct ActorBlueprint {
  explicit constexpr ActorBlueprint(const char* name) : actor(name) {}

  /// The actor of the level whose Blueprint this is (its name).
  const char* const actor;
  /// Its running Blueprint (null until the program binds it).
  Instance* running = nullptr;

  /// Runs `function` with `args` - each as a Blueprint takes it: a flag, a whole number, a decimal
  /// number, a vector, a text, an enum's value - and puts its outputs, in their order, into `out`
  /// (std::tie(a, b)). Every output starts at the zero of its type: where no Blueprint is bound or it
  /// has no such function, that is what the call gives - as Call on Actor gives the defaults its
  /// Blueprint declares for the function, and a Blueprint's declaration of a function of another
  /// actor has the zeros.
  template <class... O, class... A> void call(const char* function, std::tuple<O&...> out, const A&... args) const {
    static_assert(!(std::is_same_v<A, f32> || ...), "a 32-bit number goes in as the decimal it stands for, a 64-bit one (f64)");
    std::apply([](O&... o) { ((o = O{}), ...); }, out);
    std::vector<Value> results;
    if (!running || !running->call(function, {argument(args)...}, results)) return;
    take(results, out, std::index_sequence_for<O...>{});
  }

 private:
  static Value argument(bool b) { return Value::boolean(b); }
  template <std::integral I>
    requires(!std::same_as<I, bool>)
  static Value argument(I i) {
    return Value::integer(static_cast<i64>(i));
  }
  static Value argument(f64 x) { return Value::number(x); }
  static Value argument(Vec3 v) { return Value::vector(v); }
  static Value argument(std::string_view s) { return Value::text(std::string(s)); }
  static Value argument(const char* s) { return Value::text(s ? s : ""); }
  template <class E>
    requires std::is_enum_v<E>
  static Value argument(E e) {
    return Value::enumeration(static_cast<i64>(e));
  }

  template <class T> static void taken(const Value& v, T& to) {
    if constexpr (std::is_same_v<T, bool>) to = v.b;
    else if constexpr (std::is_enum_v<T>) to = static_cast<T>(v.i);
    else if constexpr (std::is_integral_v<T>) to = static_cast<T>(v.i);
    else if constexpr (std::is_floating_point_v<T>) to = static_cast<T>(v.as_number());
    else if constexpr (std::is_same_v<T, Vec3>) to = v.as_vector();
    else {
      static_assert(std::is_same_v<T, std::string>, "an output is a flag, a whole number, a decimal number, a vector, a text or an enum's value");
      to = v.as_text();
    }
  }
  template <class... O, std::size_t... I> static void take(const std::vector<Value>& results, std::tuple<O&...>& out, std::index_sequence<I...>) {
    ((I < results.size() ? taken(results[I], std::get<I>(out)) : void()), ...);
  }
};

}  // namespace kern::logic
