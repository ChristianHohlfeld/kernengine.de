// Kern Logic: what the nodes compute, as typed C++ functions.
//
// A node has one meaning. The interpreter's node library (src/logic/core_nodes.cpp) wraps these
// functions as nodes; a Blueprint's C++ (blueprint.h, docs/blueprint_cpp.md) calls them directly.
// Nothing here knows about graphs.
#pragma once

#include <cmath>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "kern/core/base.h"
#include "kern/core/dmath.h"
#include "kern/logic/logic.h"
#include "kern/math/math.h"

namespace kern {

/// A node's setting as a template argument: flow_switch_int<"3">, math_compare<"<=">.
template <std::size_t N> struct Setting {
  char text[N]{};
  constexpr Setting(const char (&s)[N]) {  // NOLINT (implicit: that is the point)
    for (std::size_t i = 0; i < N; ++i) text[i] = s[i];
  }
  constexpr std::string_view view() const { return {text, N - 1}; }
};

/// The functions of formulas (the Formula node; Formula::evaluate uses the same ones).
namespace math {
inline constexpr f64 pi = 3.14159265358979323846;
inline f64 div(f64 a, f64 b) { return b != 0 ? a / b : 0; }
inline f64 mod(f64 a, f64 b) { return b != 0 ? std::fmod(a, b) : 0; }
inline f64 pow(f64 a, f64 b) { return dm::pow(a, b); }
inline f64 min(f64 a, f64 b) { return min_of(a, b); }
inline f64 max(f64 a, f64 b) { return max_of(a, b); }
inline f64 clamp(f64 x, f64 lo, f64 hi) { return kern::clamp(x, min_of(lo, hi), max_of(lo, hi)); }
inline f64 abs(f64 a) { return std::fabs(a); }
inline f64 sign(f64 a) { return a > 0 ? 1 : a < 0 ? -1 : 0; }
inline f64 floor(f64 a) { return std::floor(a); }
inline f64 ceil(f64 a) { return std::ceil(a); }
inline f64 round(f64 a) { return std::round(a); }
inline f64 trunc(f64 a) { return std::trunc(a); }
inline f64 sqrt(f64 a) { return std::sqrt(max_of(0.0, a)); }
inline f64 exp(f64 a) { return dm::exp(a); }
inline f64 log(f64 a) { return a > 0 ? dm::log(a) : 0; }
inline f64 sin(f64 a) { return dm::sin(a); }
inline f64 cos(f64 a) { return dm::cos(a); }
inline f64 tan(f64 a) { return dm::tan(a); }
inline f64 asin(f64 a) { return dm::asin(kern::clamp(a, -1.0, 1.0)); }
inline f64 acos(f64 a) { return dm::acos(kern::clamp(a, -1.0, 1.0)); }
inline f64 atan(f64 a) { return dm::atan(a); }
inline f64 atan2(f64 y, f64 x) { return dm::atan2(y, x); }
inline f64 rad(f64 degrees) { return degrees * (pi / 180); }
inline f64 deg(f64 radians) { return radians * (180 / pi); }
inline f64 lerp(f64 a, f64 b, f64 t) { return a + (b - a) * t; }
}  // namespace math

namespace logic {

/// `Int` in a Blueprint's C++: a whole number as a Blueprint holds it (logic::normal). It is an
/// i64 to every function that takes one. Adding to it - Add to Variable, `Count += 1` - adds as
/// the interpreter and the JIT add: as numbers, defined for every value.
struct Whole {
  i64 v = 0;
  constexpr Whole() = default;
  constexpr Whole(i64 x) : v(normal(x)) {}       // NOLINT (implicit: an i64 is one)
  constexpr operator i64() const { return v; }   // NOLINT
  Whole& operator+=(Whole amount) {
    v = whole_add(v, amount.v);
    return *this;
  }
};

/// A value of the interpreter as the C++ type of its pin, and back. A list is the vector of its
/// items; a list of whole numbers is a vector of i64 in the interpreter and of Whole (`Int`) in a
/// Blueprint's class.
namespace detail {
template <class T> struct Held;  // (no such pin type)
/// A struct of a Blueprint's class (KERN_STRUCT). How its value is held says the class's glue,
/// field by field (logic::native_glue writes Held<Class::Struct>: is, from, to).
template <class S>
concept NativeStruct = !std::is_same_v<S, Value> && requires { requires Held<S>::type == PinType::structure; };
// A struct in the interpreter: its value as it is (Value::rows are its fields).
template <> struct Held<Value> {
  static constexpr PinType type = PinType::structure;
  static Value from(const Value& v) { return v; }
};
template <> struct Held<bool> {
  static constexpr PinType type = PinType::boolean;
  static bool from(const Value& v) { return v.as_bool(); }
};
template <> struct Held<i64> {
  static constexpr PinType type = PinType::integer;
  static i64 from(const Value& v) { return v.as_int(); }
};
template <> struct Held<Whole> {
  static constexpr PinType type = PinType::integer;
  static Whole from(const Value& v) { return v.as_int(); }
};
template <> struct Held<f64> {
  static constexpr PinType type = PinType::number;
  static f64 from(const Value& v) { return v.as_number(); }
};
template <> struct Held<Vec3> {
  static constexpr PinType type = PinType::vector;
  static Vec3 from(const Value& v) { return v.as_vector(); }
};
template <> struct Held<std::string> {
  static constexpr PinType type = PinType::text;
  static std::string from(const Value& v) { return v.as_text(); }
};
template <> struct Held<ActorHandle> {
  static constexpr PinType type = PinType::actor;
  static ActorHandle from(const Value& v) { return {v.as_actor()}; }
};
template <class T> struct Held<std::vector<T>> {
  static constexpr PinType type = list_type_of<T>();
  static std::vector<T> from(const Value& v) { return v.items<T>(); }
};
template <> struct Held<std::vector<Whole>> {
  static constexpr PinType type = PinType::integer_list;
  static std::vector<Whole> from(const Value& v) {
    const std::vector<i64>& items = v.items<i64>();
    return std::vector<Whole>(items.begin(), items.end());
  }
};
/// How many values an enum of a Blueprint's class has (KERN_ENUM states their names).
template <class E> constexpr std::size_t enum_count() {
  const std::string_view names = kern_enum_names(E{});
  std::size_t count = names.empty() ? 0 : 1;
  for (char c : names) count += c == ',' ? 1 : 0;
  return count;
}
// An enum of a Blueprint's class: a value is its place among the enum's values. What comes in
// from outside and is none of them is the first (as Instance::set and Instance::call hold it).
template <class E>
  requires std::is_enum_v<E>
struct Held<E> {
  static constexpr PinType type = PinType::enumeration;
  static bool is(const Value& v) { return v.type == type && v.i >= 0 && static_cast<u64>(v.i) < enum_count<E>(); }
  static E from(const Value& v) {
    const i64 index = v.as_int();
    return static_cast<E>(index >= 0 && static_cast<u64>(index) < enum_count<E>() ? index : 0);
  }
};
template <class E>
  requires std::is_enum_v<E>
struct Held<std::vector<E>> {
  static constexpr PinType type = PinType::enumeration_list;
  static bool is(const Value& v) {
    if (v.type != type) return false;
    for (i64 index : v.items<i64>())
      if (index < 0 || static_cast<u64>(index) >= enum_count<E>()) return false;
    return true;
  }
  static std::vector<E> from(const Value& v) {
    std::vector<E> r;
    for (i64 index : v.items<i64>()) r.push_back(static_cast<E>(index >= 0 && static_cast<u64>(index) < enum_count<E>() ? index : 0));
    return r;
  }
};
// A list of structs of a Blueprint's class. What comes in from outside and is no struct of that
// type is the struct's default, item by item (as Instance::set and Instance::call hold it).
template <NativeStruct S> struct Held<std::vector<S>> {
  static constexpr PinType type = PinType::structure_list;
  static bool is(const Value& v) {
    if (v.type != type) return false;
    for (const Value& item : v.items<Value>())
      if (!Held<S>::is(item)) return false;
    return true;
  }
  static std::vector<S> from(const Value& v) {
    std::vector<S> r;
    for (const Value& item : v.items<Value>()) r.push_back(Held<S>::from(item));
    return r;
  }
};
}  // namespace detail
template <class T> T from_value(const Value& v) { return detail::Held<T>::from(v); }
template <class T> constexpr PinType pin_type_of() { return detail::Held<T>::type; }
/// Is `v` a value of the C++ type T as a Blueprint holds it (logic::is_value_of: of that pin
/// type, an enum's value one of its values, a struct with a value of its type for every field)?
template <class T> bool is_value(const Value& v) {
  if constexpr (requires { detail::Held<T>::is(v); }) return detail::Held<T>::is(v);
  else return v.type == pin_type_of<T>();
}
inline Value to_value(bool v) { return Value::boolean(v); }
inline Value to_value(i64 v) { return Value::integer(v); }
inline Value to_value(Whole v) { return Value::integer(v); }
inline Value to_value(f64 v) { return Value::number(v); }
inline Value to_value(Vec3 v) { return Value::vector(v); }
inline Value to_value(std::string v) { return Value::text(std::move(v)); }
inline Value to_value(ActorHandle v) { return Value::actor(v.id); }
inline Value to_value(Value v) { return v; }  // (a struct in the interpreter)
template <class T> Value to_value(std::vector<T> v) { return Value::of(std::move(v)); }
template <detail::NativeStruct S> Value to_value(const S& v) { return detail::Held<S>::to(v); }
template <detail::NativeStruct S> Value to_value(std::vector<S> v) {
  std::vector<Value> items;
  for (const S& item : v) items.push_back(detail::Held<S>::to(item));
  return Value::structures(std::move(items));
}
template <class E>
  requires std::is_enum_v<E>
Value to_value(E v) {
  return Value::enumeration(static_cast<i64>(v));
}
template <class E>
  requires std::is_enum_v<E>
Value to_value(std::vector<E> v) {
  std::vector<i64> places;
  for (E e : v) places.push_back(static_cast<i64>(e));
  return Value::enumerations(std::move(places));
}
inline Value to_value(const std::vector<Whole>& v) { return Value::of(std::vector<i64>(v.begin(), v.end())); }

/// What the list nodes do, for a list of any item type - the node library and a Blueprint's C++
/// call these. Every one is defined for every index: there is no reading or writing past a
/// list's end.
namespace list {
template <class T> i64 length(const std::vector<T>& items) { return static_cast<i64>(items.size()); }
/// The item at `index`; beyond the list: the zero of its type.
template <class T> T get(const std::vector<T>& items, i64 index) {
  if (index < 0 || static_cast<u64>(index) >= items.size()) return T{};
  return T(items[static_cast<std::size_t>(index)]);
}
/// The first index of an item that equals `item`, or -1.
template <class T> i64 find(const std::vector<T>& items, const std::type_identity_t<T>& item) {
  for (std::size_t k = 0; k < items.size(); ++k)
    if (T(items[k]) == item) return static_cast<i64>(k);
  return -1;
}
template <class T> bool contains(const std::vector<T>& items, const std::type_identity_t<T>& item) { return find(items, item) >= 0; }
/// Adds `item` at the end; its index.
template <class T> i64 add(std::vector<T>& items, std::type_identity_t<T> item) {
  items.push_back(std::move(item));
  return static_cast<i64>(items.size()) - 1;
}
/// Puts `item` in front of what is at `index` (before the first: at the start; past the end: at
/// the end).
template <class T> void insert(std::vector<T>& items, i64 index, std::type_identity_t<T> item) {
  const std::size_t at = index <= 0 ? 0 : static_cast<u64>(index) >= items.size() ? items.size() : static_cast<std::size_t>(index);
  items.insert(items.begin() + static_cast<std::ptrdiff_t>(at), std::move(item));
}
/// Replaces the item at `index`; beyond the list: nothing.
template <class T> void set(std::vector<T>& items, i64 index, std::type_identity_t<T> item) {
  if (index >= 0 && static_cast<u64>(index) < items.size()) items[static_cast<std::size_t>(index)] = std::move(item);
}
/// Takes the item at `index` out; beyond the list: nothing.
template <class T> void remove_at(std::vector<T>& items, i64 index) {
  if (index >= 0 && static_cast<u64>(index) < items.size()) items.erase(items.begin() + static_cast<std::ptrdiff_t>(index));
}
/// Takes every item that equals `item` out; was there one?
template <class T> bool remove(std::vector<T>& items, const std::type_identity_t<T>& item) {
  std::size_t kept = 0;
  for (std::size_t k = 0; k < items.size(); ++k) {
    if (T(items[k]) == item) continue;
    if (kept != k) items[kept] = T(items[k]);
    ++kept;
  }
  const bool removed = kept != items.size();
  items.resize(kept);
  return removed;
}
template <class T> void clear(std::vector<T>& items) { items.clear(); }
}  // namespace list

/// What the enum nodes do. An enum's values have names; a value is its place among them (the
/// first is 0). The node library and a Blueprint's C++ call these.
namespace enums {
/// `index` when it is the place of one of `count` values, else 0 (the first value).
constexpr i64 valid(std::size_t count, i64 index) { return index >= 0 && static_cast<u64>(index) < count ? index : 0; }
constexpr bool is_value(std::size_t count, i64 index) { return index >= 0 && static_cast<u64>(index) < count; }
/// The name of the value at `index` ("" when there is none).
inline std::string name(const std::vector<std::string>& values, i64 index) { return is_value(values.size(), index) ? values[static_cast<std::size_t>(index)] : std::string(); }
/// The place of the value called `name`, or -1.
inline i64 find(const std::vector<std::string>& values, std::string_view name) {
  for (std::size_t k = 0; k < values.size(); ++k)
    if (values[k] == name) return static_cast<i64>(k);
  return -1;
}
/// The names of an enum as KERN_ENUM has them: "Closed, Opening, Open".
inline std::vector<std::string> names(std::string_view list) {
  std::vector<std::string> r;
  std::size_t at = 0;
  while (at < list.size()) {
    std::size_t end = list.find(',', at);
    if (end == std::string_view::npos) end = list.size();
    std::size_t a = at, b = end;
    while (a < b && (list[a] == ' ' || list[a] == '\n' || list[a] == '\t')) ++a;
    while (b > a && (list[b - 1] == ' ' || list[b - 1] == '\n' || list[b - 1] == '\t')) --b;
    if (b > a) r.emplace_back(list.substr(a, b - a));
    at = end + 1;
  }
  return r;
}
}  // namespace enums

/// What the struct nodes do. A struct's value holds a value for each of its fields, in the order
/// its declaration has them. In the interpreter that is a Value (its rows); in a Blueprint's C++
/// it is the aggregate KERN_STRUCT declares - Make is its initializer, Break its members, Set
/// Field the assignment to one of them.
namespace structs {
/// Make: the struct with these fields.
inline Value make(std::vector<Value> fields) { return Value::structure(std::move(fields)); }
/// Set Field: the field at `index` of the struct `s` becomes `value` (a field it does not have:
/// nothing).
inline void set_field(Value& s, std::size_t index, Value value) {
  if (index < s.fields()) s.own_rows()[index] = std::move(value);
}

/// Stands for a value of any type where a field of an aggregate is initialized (never called).
struct AnyField {
  template <class T> operator T() const;  // NOLINT (implicit: that is the point)
};
template <class S, std::size_t... I> constexpr bool takes_fields(std::index_sequence<I...>) {
  return requires { S{(static_cast<void>(I), AnyField{})...}; };
}
/// How many fields the aggregate S has (a struct of a Blueprint: StructType::kMaxFields at most).
template <class S, std::size_t N = StructType::kMaxFields> constexpr std::size_t field_count() {
  if constexpr (N == 0) return 0;
  else if constexpr (takes_fields<S>(std::make_index_sequence<N>{})) return N;
  else return field_count<S, N - 1>();
}
/// Calls f(fields...) with the fields of the aggregate `s`, in their order.
template <class S, class F> constexpr decltype(auto) fields(S& s, F&& f) {
  constexpr std::size_t N = field_count<std::remove_const_t<S>>();
  if constexpr (N == 1) { auto& [f0] = s; return f(f0); }
  else if constexpr (N == 2) { auto& [f0, f1] = s; return f(f0, f1); }
  else if constexpr (N == 3) { auto& [f0, f1, f2] = s; return f(f0, f1, f2); }
  else if constexpr (N == 4) { auto& [f0, f1, f2, f3] = s; return f(f0, f1, f2, f3); }
  else if constexpr (N == 5) { auto& [f0, f1, f2, f3, f4] = s; return f(f0, f1, f2, f3, f4); }
  else if constexpr (N == 6) { auto& [f0, f1, f2, f3, f4, f5] = s; return f(f0, f1, f2, f3, f4, f5); }
  else if constexpr (N == 7) { auto& [f0, f1, f2, f3, f4, f5, f6] = s; return f(f0, f1, f2, f3, f4, f5, f6); }
  else if constexpr (N == 8) { auto& [f0, f1, f2, f3, f4, f5, f6, f7] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7); }
  else if constexpr (N == 9) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8); }
  else if constexpr (N == 10) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }
  else if constexpr (N == 11) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10); }
  else if constexpr (N == 12) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11); }
  else if constexpr (N == 13) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12); }
  else if constexpr (N == 14) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13); }
  else if constexpr (N == 15) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14); }
  else if constexpr (N == 16) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15); }
  else if constexpr (N == 17) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16); }
  else if constexpr (N == 18) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17); }
  else if constexpr (N == 19) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18); }
  else if constexpr (N == 20) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19); }
  else if constexpr (N == 21) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20); }
  else if constexpr (N == 22) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21); }
  else if constexpr (N == 23) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22); }
  else if constexpr (N == 24) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23); }
  else if constexpr (N == 25) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24); }
  else if constexpr (N == 26) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25); }
  else if constexpr (N == 27) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26); }
  else if constexpr (N == 28) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27); }
  else if constexpr (N == 29) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28); }
  else if constexpr (N == 30) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29); }
  else if constexpr (N == 31) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30); }
  else if constexpr (N == 32) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31); }
  else return f();
}
/// Are two structs equal: every field of one equal to that field of the other (the operator ==
/// of a KERN_STRUCT).
template <class S> constexpr bool equal(const S& a, const S& b) {
  return fields(a, [&b](const auto&... x) { return fields(b, [&x...](const auto&... y) { return (static_cast<bool>(x == y) && ...); }); });
}
}  // namespace structs

/// A whole number written as text, as settings hold them ("3", " 12"): leading blanks, a sign,
/// digits; anything else ends it. Empty or no digits: `fallback`.
constexpr int setting_int(std::string_view s, int fallback) {
  std::size_t i = 0;
  while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r' || s[i] == '\f' || s[i] == '\v')) ++i;
  bool minus = false;
  if (i < s.size() && (s[i] == '+' || s[i] == '-')) minus = s[i++] == '-';
  if (s.empty()) return fallback;
  i64 v = 0;
  for (; i < s.size() && s[i] >= '0' && s[i] <= '9'; ++i) {
    v = v * 10 + (s[i] - '0');
    if (v > 2000000000) { v = 2000000000; break; }
  }
  return static_cast<int>(minus ? -v : v);
}
/// A count setting (Sequence outputs, Switch cases): `fallback` when empty, kept within [lo, hi].
constexpr int setting_count(std::string_view s, int fallback, int lo, int hi) { return kern::clamp(s.empty() ? fallback : setting_int(s, fallback), lo, hi); }
/// The cases of Switch on Text: separated by commas, blanks around them dropped, empty ones left out.
inline std::vector<std::string> setting_cases(std::string_view s) {
  std::vector<std::string> out;
  std::string cur;
  auto flush = [&] {
    while (!cur.empty() && cur.front() == ' ') cur.erase(cur.begin());
    while (!cur.empty() && cur.back() == ' ') cur.pop_back();
    if (!cur.empty()) out.push_back(cur);
    cur.clear();
  };
  for (char ch : s) {
    if (ch == ',') flush();
    else cur += ch;
  }
  flush();
  return out;
}

/// What the engine passes by position to a function of a Blueprint's class, as the C++ type of
/// that input (the conversion of Instance::call); a missing one is the input's default.
template <class T> T native_arg(const std::vector<Value>& args, std::size_t i, const T& fallback) {
  return i < args.size() ? from_value<T>(args[i].converted(pin_type_of<T>())) : fallback;
}
/// A value an event brought, as the C++ type of its handler's parameter: output `out` of the
/// event's node `node` (Runtime::fire puts the event's values there, for every tier).
template <class T> T native_out(const Instance& instance, int node, int out) { return from_value<T>(instance.output(node, out).converted(pin_type_of<T>())); }
/// Instance::set for a variable that is a member of a Blueprint's class: the conversions the
/// interpreter's variables make.
inline void native_set(bool& variable, const Value& v) { variable = v.as_number() != 0; }
inline void native_set(i64& variable, const Value& v) { variable = whole(v.as_number()); }
inline void native_set(Whole& variable, const Value& v) { variable = whole(v.as_number()); }
inline void native_set(f64& variable, const Value& v) { variable = v.as_number(); }
inline void native_set(Vec3& variable, const Value& v) { variable = v.as_vector(); }
inline void native_set(std::string& variable, const Value& v) { variable = v.as_text(); }
inline void native_set(ActorHandle& variable, const Value& v) { variable = {v.as_actor()}; }
template <class T> void native_set(std::vector<T>& variable, const Value& v) { variable = from_value<std::vector<T>>(v.converted(pin_type_of<std::vector<T>>())); }
template <class E>
  requires std::is_enum_v<E>
void native_set(E& variable, const Value& v) {
  variable = from_value<E>(v.converted(PinType::enumeration));
}
template <detail::NativeStruct S> void native_set(S& variable, const Value& v) { variable = from_value<S>(v); }

/// A Random node's own sequence. `m` is the node's memory ([0] where the sequence is, [1] it has
/// begun); it starts from the Blueprint's seed and the node's id. Two Random nodes never share a
/// sequence: in which order they are asked makes no difference to what each gives - in C++ the
/// order in which arguments are worked out is the compiler's.
namespace random {
inline u32 next(f64* m, u32 seed, u32 node) {
  u32 state = static_cast<u32>(m[0]);
  if (m[1] == 0) {
    m[1] = 1;
    state = seed ^ (node * 0x9e3779b9u);
  }
  state += 0x9e3779b9u;
  m[0] = static_cast<f64>(state);
  u32 z = state;
  z ^= z >> 16;
  z *= 0x85ebca6bu;
  z ^= z >> 13;
  z *= 0xc2b2ae35u;
  z ^= z >> 16;
  return z;
}
/// 0 <= x < 1
inline f64 unit(u32 r) { return static_cast<f64>(r >> 8) / static_cast<f64>(1u << 24); }
inline f64 number(f64* m, u32 seed, u32 node, f64 min, f64 max) { return min + (max - min) * unit(next(m, seed, node)); }
/// A whole number from min to max, both included (max below min: min).
inline i64 whole_number(f64* m, u32 seed, u32 node, i64 min, i64 max) {
  if (max < min) max = min;
  const u64 span = static_cast<u64>(max) - static_cast<u64>(min) + 1;  // (0: every Int there is)
  const u32 choices = span == 0 || span > 0x7fffffffull ? 0x7fffffffu : static_cast<u32>(span);
  return min + static_cast<i64>(next(m, seed, node) % choices);
}
inline bool chance(f64* m, u32 seed, u32 node, f64 chance) { return unit(next(m, seed, node)) < chance; }
}  // namespace random

/// The value nodes of the core library. Static, typed, without side effects: the interpreter
/// wraps each as a node, a Blueprint's C++ calls it by this name.
struct PureNodes {
  using Bool = bool;
  using Int = i64;
  using Float = f64;
  using Vector = Vec3;
  using Text = std::string;
  using Actor = ActorHandle;
  static constexpr f64 kDegrees = 3.14159265358979323846 / 180.0;

  // ---- math ----
  static Float math_add(Float a, Float b) { return a + b; }
  static Float math_sub(Float a, Float b) { return a - b; }
  static Float math_mul(Float a, Float b) { return a * b; }
  static Float math_div(Float a, Float b) { return b != 0 ? a / b : 0.0; }
  static Float math_mod(Float a, Float b) { return b != 0 ? std::fmod(a, b) : 0.0; }
  static Float math_min(Float a, Float b) { return min_of(a, b); }
  static Float math_max(Float a, Float b) { return max_of(a, b); }
  static Float math_clamp(Float value, Float min, Float max) { return kern::clamp(value, min_of(min, max), max_of(min, max)); }
  static Float math_abs(Float a) { return std::fabs(a); }
  static Float math_negate(Float a) { return -a; }
  static Float math_lerp(Float a, Float b, Float alpha) { return a + (b - a) * alpha; }
  static Float math_sin(Float degrees) { return dm::sin(degrees * kDegrees); }
  static Float math_cos(Float degrees) { return dm::cos(degrees * kDegrees); }
  static Float math_atan2(Float y, Float x) { return dm::atan2(y, x) / kDegrees; }
  static Float math_sqrt(Float a) { return std::sqrt(max_of(0.0, a)); }
  static Float math_pow(Float a, Float b) {
    const f64 r = dm::pow(a, b);
    return std::isfinite(r) ? r : 0.0;
  }
  static Int math_round(Float a) { return whole(std::round(a)); }  // (half away from zero)
  static Int math_floor(Float a) { return whole(std::floor(a)); }
  /// Compare with the operator of the node's setting (== and != within 1e-9).
  static Bool compare(std::string_view op, Float a, Float b) {
    if (op == "==") return std::fabs(a - b) <= 1e-9;
    if (op == "!=") return std::fabs(a - b) > 1e-9;
    if (op == "<") return a < b;
    if (op == "<=") return a <= b;
    if (op == ">") return a > b;
    return a >= b;
  }

  // ---- logic ----
  static Bool logic_and(Bool a, Bool b) { return a && b; }
  static Bool logic_or(Bool a, Bool b) { return a || b; }
  static Bool logic_xor(Bool a, Bool b) { return a != b; }
  static Bool logic_not(Bool a) { return !a; }

  // ---- values ----
  static Float value_float(Float value) { return value; }
  static Int value_int(Int value) { return value; }
  static Bool value_bool(Bool value) { return value; }
  static Text value_text(Text value) { return value; }

  // ---- vectors ----
  struct Broken {
    Float x = 0, y = 0, z = 0;
  };
  static Vector vector_make(Float x, Float y, Float z) { return {static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(z)}; }
  static Broken vector_break(Vector v) { return {v.x, v.y, v.z}; }
  static Vector vector_add(Vector a, Vector b) { return a + b; }
  static Vector vector_sub(Vector a, Vector b) { return a - b; }
  static Vector vector_cross(Vector a, Vector b) { return cross(a, b); }
  static Vector vector_scale(Vector a, Float b) { return a * static_cast<f32>(b); }
  static Float vector_length(Vector a) { return length(a); }
  static Vector vector_normalize(Vector a) { return normalize(a, {0, 0, 0}); }
  static Float vector_distance(Vector a, Vector b) { return distance(a, b); }
  static Float vector_dot(Vector a, Vector b) { return dot(a, b); }
  static Vector vector_lerp(Vector a, Vector b, Float alpha) { return lerp(a, b, static_cast<f32>(alpha)); }

  // ---- text ----
  static Text text_append(Text a, Text b, Text c) { return a + b + c; }
  static Bool text_equal(Text a, Text b) { return a == b; }
  static Bool text_contains(Text a, Text b) { return a.find(b) != std::string::npos; }
  static Int text_length(Text a) { return static_cast<i64>(a.size()); }
};

/// The flow nodes that keep something between runs. `m`: the node's memory, four numbers (the
/// interpreter's slots of the node; kern::Node in a Blueprint's C++). `entry`: the exec input,
/// by its place among the node's exec inputs.
namespace flow {
/// For Loop. start: at its exec input; step: every time round. True: run the body with index().
inline void for_start(f64* m, i64 first, i64 last) {
  m[0] = static_cast<f64>(first);
  m[1] = static_cast<f64>(last);
}
inline void for_step(f64* m) { m[0] += 1; }
inline bool for_runs(const f64* m) { return !(m[0] > m[1]); }
inline i64 for_index(const f64* m) { return whole(m[0]); }
/// Do Once (entry 1: Reset). True: the call goes through.
inline bool do_once(f64* m, int entry, bool start_closed) {
  f64& closed = m[0];
  f64& started = m[1];
  if (entry == 1) {
    closed = 0;
    started = 1;
    return false;
  }
  if (started == 0) {
    started = 1;
    closed = start_closed ? 1 : 0;
  }
  if (closed != 0) return false;
  closed = 1;
  return true;
}
/// Do N (entry 1: Reset). True: the call goes through, and `counter` is its number.
inline bool do_n(f64* m, int entry, i64 n, i64& counter) {
  if (entry == 1) {
    m[0] = 0;
    return false;
  }
  if (m[0] >= static_cast<f64>(n)) return false;
  m[0] += 1;
  counter = whole(m[0]);
  return true;
}
/// Gate (entries: 0 Enter, 1 Open, 2 Close, 3 Toggle). True: Enter goes through.
inline bool gate(f64* m, int entry, bool start_closed) {
  f64& open = m[0];
  f64& started = m[1];
  if (started == 0) {
    started = 1;
    open = start_closed ? 0 : 1;
  }
  switch (entry) {
    case 0: return open != 0;
    case 1: open = 1; return false;
    case 2: open = 0; return false;
    case 3: open = open != 0 ? 0 : 1; return false;
    default: return false;
  }
}
/// Flip Flop: the exec output it takes (0 A, 1 B).
inline int flip_flop(f64* m, bool& is_a) {
  m[0] = m[0] != 0 ? 0 : 1;
  is_a = m[0] != 0;
  return is_a ? 0 : 1;
}
/// Delay. start: true when the wait begins (a call while it waits is ignored); done: it is over.
inline bool delay_start(f64* m) {
  if (m[0] != 0) return false;
  m[0] = 1;
  return true;
}
inline void delay_done(f64* m) { m[0] = 0; }
/// Tween. m: [0] it runs, [1] when it began, [2] how long it takes.
inline void tween_play(f64* m, f64 now, f64 duration) {
  m[0] = 1;
  m[1] = now;
  m[2] = max_of(0.0, duration);
}
inline void tween_stop(f64* m) { m[0] = 0; }
inline bool tween_runs(const f64* m) { return m[0] != 0; }
/// One step of a running tween at time `now`: its alpha (0..1, eased), and whether that was the
/// last one (the tween then no longer runs). `first`: the step that comes with Play - never the
/// last one: Finished comes with the last Update on a later step, also for a tween of no duration.
inline f64 tween_step(f64* m, f64 now, std::string_view ease, bool first, bool& finished) {
  const f64 t = kern::clamp(m[2] > 0 ? (now - m[1]) / m[2] : 1.0, 0.0, 1.0);
  f64 e = t;
  if (ease == "smooth") e = t * t * (3 - 2 * t);
  else if (ease == "in") e = t * t;
  else if (ease == "out") e = 1 - (1 - t) * (1 - t);
  finished = t >= 1 && !first;
  if (finished) m[0] = 0;
  return e;
}
/// Switch on Int with `cases` cases: the exec output (the case, or `cases` for Default).
inline int switch_int(int cases, i64 selection) { return selection >= 0 && selection < cases ? static_cast<int>(selection) : cases; }
/// Switch on Text: the exec output (the case that equals the selection, or the count for Default).
inline int switch_text(std::string_view cases, const std::string& selection) {
  const std::vector<std::string> list = setting_cases(cases);
  for (std::size_t i = 0; i < list.size(); ++i) if (list[i] == selection) return static_cast<int>(i);
  return static_cast<int>(list.size());
}
}  // namespace flow

}  // namespace logic

namespace logic::detail {
/// The conversions of Value::converted, type by type (only the pairs a wire can make).
template <class T> struct To;
template <> struct To<bool> {
  static bool from(bool v) { return v; }
  static bool from(i64 v) { return v != 0; }
  static bool from(f64 v) { return v != 0; }
  static bool from(ActorHandle v) { return v.id != 0; }
};
template <> struct To<i64> {
  static i64 from(bool v) { return v ? 1 : 0; }
  static i64 from(i64 v) { return v; }
  static i64 from(f64 v) { return whole(v); }
};
template <> struct To<Whole> : To<i64> {};
template <> struct To<f64> {
  static f64 from(bool v) { return v ? 1 : 0; }
  static f64 from(i64 v) { return static_cast<f64>(v); }
  static f64 from(f64 v) { return v; }
};
template <> struct To<Vec3> {
  static Vec3 from(Vec3 v) { return v; }
  static Vec3 from(f64 v) {
    const f32 x = static_cast<f32>(v);
    return {x, x, x};
  }
  static Vec3 from(i64 v) { return from(static_cast<f64>(v)); }
};
template <> struct To<std::string> {
  template <class S> static std::string from(const S& v) { return to_value(v).as_text(); }  // (everything prints, as the interpreter prints it)
};
template <> struct To<ActorHandle> {
  static ActorHandle from(ActorHandle v) { return v; }
};
}  // namespace logic::detail

/// What a wire does when its ends have different types: kern::to<Float>(index). The same
/// conversions the interpreter makes (Value::converted).
template <class T, class S> T to(const S& v) { return logic::detail::To<T>::from(v); }

}  // namespace kern
