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

#include "kern/core/fields.h"
#include "kern/logic/logic.h"

namespace kern::logic {

namespace detail {
template <class T> struct IsVector : std::false_type {};
template <class T> struct IsVector<std::vector<T>> : std::true_type {};
}  // namespace detail

struct ActorBlueprint {
  explicit constexpr ActorBlueprint(const char* name) : actor(name) {}

  /// The actor of the level whose Blueprint this is (its name).
  const char* const actor;
  /// Its running Blueprint (null until the program binds it).
  Instance* running = nullptr;

  /// Runs `function` with `args` - each as a Blueprint takes it: a flag, a whole number, a decimal
  /// number, a vector, a text, an enum's value, a struct (a plain struct of the program whose fields
  /// are such values, in the order of the Blueprint's struct), a list of any of these - and puts its outputs, in their order, into `out`
  /// (std::tie(a, b)). Every output starts at the zero of its type: where no Blueprint is bound or it
  /// has no such function, that is what the call gives - as Call on Actor gives the defaults its
  /// Blueprint declares for the function, and a Blueprint's declaration of a function of another
  /// actor has the zeros.
  /// A function that gives one number, or one flag: what it gives - 0, false where no Blueprint is bound or it
  /// has no such function. Call on Actor with one output, as a value: f64 speed = rules.number("Speed", x);
  template <class... A> f64 number(const char* function, const A&... args) const {
    f64 r = 0;
    call(function, std::tie(r), args...);
    return r;
  }
  template <class... A> bool flag(const char* function, const A&... args) const {
    bool r = false;
    call(function, std::tie(r), args...);
    return r;
  }
  /// The variable `name` of its running Blueprint as a T (as an output of call() is taken) - the zero of T
  /// where none is bound or it has no such variable. Get Variable on Actor in a Blueprint.
  template <class T> T get(const char* name) const {
    T v{};
    if (running) taken(running->get(name), v);
    return v;
  }
  template <class... O, class... A> void call(const char* function, std::tuple<O&...> out, const A&... args) const {
    const std::vector<Value> in{argument(args)...};  // (first the values: one may be read from a place an output goes to)
    std::apply([](O&... o) { ((o = O{}), ...); }, out);
    std::vector<Value> results;
    if (!running || !running->call(function, in, results)) return;
    take(results, out, std::index_sequence_for<O...>{});
  }

 private:
  template <class T> static constexpr bool is_list = detail::IsVector<T>::value;
  /// A plain struct of the program that crosses as a Blueprint's struct (not a vector, not a list).
  template <class T> static constexpr bool is_struct = PlainStruct<T> && !std::is_same_v<T, Vec3> && !is_list<T>;

  template <class T> static Value argument(const T& x) {
    static_assert(!std::is_same_v<T, f32>, "a 32-bit number goes in as the decimal it stands for, a 64-bit one (f64)");
    if constexpr (std::is_same_v<T, bool>) return Value::boolean(x);
    else if constexpr (std::is_enum_v<T>) return Value::enumeration(static_cast<i64>(x));
    else if constexpr (std::is_integral_v<T>) return Value::integer(static_cast<i64>(x));
    else if constexpr (std::is_same_v<T, f64>) return Value::number(x);
    else if constexpr (std::is_same_v<T, Vec3>) return Value::vector(x);
    else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) return Value::text(std::string(x));
    else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) return Value::text(x ? x : "");
    else if constexpr (is_list<T>) return list_argument(x);
    else if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_cv_t<std::remove_extent_t<T>>, char>) return Value::text(x);
    else {
      static_assert(is_struct<T>, "a value is a flag, a whole number, a decimal number (64 bit), a vector, a text, an enum's value, a plain struct of such values or a list of them");
      return std::apply([](const auto&... f) { return Value::structure({argument(f)...}); }, fields_of(x));
    }
  }
  template <class T> static Value list_argument(const std::vector<T>& items) {
    static_assert(!std::is_same_v<T, f32>, "a 32-bit number goes in as the decimal it stands for, a 64-bit one (f64)");
    if constexpr (std::is_same_v<T, bool>) return Value::of(std::vector<bool>(items.begin(), items.end()));
    else if constexpr (std::is_enum_v<T>) {
      std::vector<i64> r;
      r.reserve(items.size());
      for (T e : items) r.push_back(static_cast<i64>(e));
      return Value::enumerations(std::move(r));
    } else if constexpr (std::is_integral_v<T>) return Value::of(std::vector<i64>(items.begin(), items.end()));
    else if constexpr (std::is_same_v<T, f64> || std::is_same_v<T, Vec3> || std::is_same_v<T, std::string>) return Value::of(items);
    else {
      std::vector<Value> r;
      r.reserve(items.size());
      for (const T& x : items) r.push_back(argument(x));
      return Value::structures(std::move(r));
    }
  }

  template <class T> static void taken(const Value& v, T& to) {
    if constexpr (std::is_same_v<T, bool>) to = v.b;
    else if constexpr (std::is_enum_v<T>) to = static_cast<T>(v.i);
    else if constexpr (std::is_integral_v<T>) to = static_cast<T>(v.i);
    else if constexpr (std::is_floating_point_v<T>) to = static_cast<T>(v.as_number());
    else if constexpr (std::is_same_v<T, Vec3>) to = v.as_vector();
    else if constexpr (std::is_same_v<T, std::string>) to = v.as_text();
    else if constexpr (is_list<T>) {
      using I = typename T::value_type;
      to.clear();
      if constexpr (std::is_same_v<I, bool> || std::is_same_v<I, f64> || std::is_same_v<I, Vec3> || std::is_same_v<I, std::string>) {
        const auto& got = v.items<I>();
        to.assign(got.begin(), got.end());
      } else if constexpr (std::is_enum_v<I> || std::is_integral_v<I>) {
        for (i64 x : v.items<i64>()) to.push_back(static_cast<I>(x));
      } else if constexpr (std::is_floating_point_v<I>) {
        for (f64 x : v.items<f64>()) to.push_back(static_cast<I>(x));
      } else {
        to.resize(v.size());
        for (std::size_t k = 0; k < to.size(); ++k) taken(v.items<Value>()[k], to[k]);
      }
    } else {
      static_assert(is_struct<T>, "an output is a flag, a whole number, a decimal number, a vector, a text, an enum's value, a plain struct of such values or a list of them");
      // (a struct's value that holds no fields - the zero of the struct - has every field at its default: T{})
      to = T{};
      std::size_t k = 0;
      std::apply([&](auto&... f) { ((k < v.fields() ? taken(*v.field(k), f) : void(), ++k), ...); }, fields_of(to));
    }
  }
  template <class... O, std::size_t... I> static void take(const std::vector<Value>& results, std::tuple<O&...>& out, std::index_sequence<I...>) {
    ((I < results.size() ? taken(results[I], std::get<I>(out)) : void()), ...);
  }
};

}  // namespace kern::logic
