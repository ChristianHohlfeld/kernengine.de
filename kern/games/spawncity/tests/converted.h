// What the generated tests of tests/converted/ share (tools/cpp_to_blueprint.py --test writes them).
//
// A source file was converted to a Blueprint of the level. Its test holds the source of every
// converted function once more - the text as it stands in the game, compiled with the numbers and
// the mathematics of the Blueprint language (f32 is a 64-bit number there, std::sin is
// kern::math::sin; a class is compiled along, so that its data is 64-bit too). That is what the
// source computes in the Blueprint language. The Blueprint - in the interpreter and as its
// compiled class - must give exactly that: every number, every vector, every field, bit for bit.
// There is no margin.
//
// One thing is not compared: a call in which the source divides by zero or leaves the numbers
// otherwise has no result in C++ to compare with (the Blueprint's arithmetic gives one: x / 0 is
// 0). Such calls are counted.
#pragma once

#include <bit>
#include <cfenv>
#include <limits>
#include <cmath>
#include <cstdio>
#include <deque>
#include <string>
#include <type_traits>
#include <vector>

#include "../src/blueprint_call.h"
#include "../src/common.h"
#include "kern/logic/nodes.h"
#include "rule_blueprint.h"
#include "test_util.h"

namespace sc::converted {

using Value = kern::logic::Value;

// ---- the numbers of the Blueprint language, for the source a test holds ----
// A number is 64-bit. A vector holds its components in 32 bit; read, a component is a number like any
// other, and what is written into one is brought to 32 bit. What vectors do as a whole (add, dot, cross,
// length, normalize ...) is what the nodes of the language do: the engine's own functions.
namespace wide {
using f32 = double;
using f64 = double;
using kern::i16;
using kern::i32;
using kern::i64;
using kern::i8;
using kern::u16;
using kern::u32;
using kern::u64;
using kern::u8;

/// A component of a vector.
struct Num32 {
  float v = 0;
  constexpr Num32() = default;
  constexpr Num32(double x) : v(static_cast<float>(x)) {}
  constexpr operator double() const { return static_cast<double>(v); }
  constexpr Num32& operator+=(double x) { return *this = Num32(static_cast<double>(v) + x); }
  constexpr Num32& operator-=(double x) { return *this = Num32(static_cast<double>(v) - x); }
  constexpr Num32& operator*=(double x) { return *this = Num32(static_cast<double>(v) * x); }
  constexpr Num32& operator/=(double x) { return *this = Num32(static_cast<double>(v) / x); }
};

struct Vec2 {
  Num32 x, y;
  constexpr Vec2() = default;
  constexpr Vec2(double x_, double y_) : x(x_), y(y_) {}
  constexpr Vec2(kern::Vec2 v) : x(v.x), y(v.y) {}
  constexpr operator kern::Vec2() const { return {x.v, y.v}; }
  constexpr Vec2 operator+(Vec2 o) const { return kern::Vec2(*this) + kern::Vec2(o); }
  constexpr Vec2 operator-(Vec2 o) const { return kern::Vec2(*this) - kern::Vec2(o); }
  constexpr Vec2 operator*(double s) const { return kern::Vec2(*this) * static_cast<float>(s); }
  constexpr Vec2& operator+=(Vec2 o) { return *this = *this + o; }
};
constexpr double dot(Vec2 a, Vec2 b) { return kern::dot(kern::Vec2(a), kern::Vec2(b)); }
constexpr double cross(Vec2 a, Vec2 b) { return kern::cross(kern::Vec2(a), kern::Vec2(b)); }
inline double length(Vec2 a) { return kern::length(kern::Vec2(a)); }

struct Vec3 {
  Num32 x, y, z;
  constexpr Vec3() = default;
  constexpr Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
  constexpr Vec3(kern::Vec3 v) : x(v.x), y(v.y), z(v.z) {}
  constexpr operator kern::Vec3() const { return {x.v, y.v, z.v}; }
  constexpr double operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
  constexpr Num32& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }
  constexpr Vec3 operator+(Vec3 o) const { return kern::Vec3(*this) + kern::Vec3(o); }
  constexpr Vec3 operator-(Vec3 o) const { return kern::Vec3(*this) - kern::Vec3(o); }
  constexpr Vec3 operator-() const { return -kern::Vec3(*this); }
  constexpr Vec3 operator*(double s) const { return kern::Vec3(*this) * static_cast<float>(s); }
  constexpr Vec3 operator*(Vec3 o) const { return kern::Vec3(*this) * kern::Vec3(o); }
  constexpr Vec3 operator/(double s) const { return kern::Vec3(*this) / static_cast<float>(s); }
  constexpr Vec3& operator+=(Vec3 o) { return *this = *this + o; }
  constexpr Vec3& operator-=(Vec3 o) { return *this = *this - o; }
  constexpr Vec3& operator*=(double s) { return *this = *this * s; }
  constexpr bool operator==(const Vec3& o) const { return kern::Vec3(*this) == kern::Vec3(o); }
};
constexpr double dot(Vec3 a, Vec3 b) { return kern::dot(kern::Vec3(a), kern::Vec3(b)); }
constexpr Vec3 cross(Vec3 a, Vec3 b) { return kern::cross(kern::Vec3(a), kern::Vec3(b)); }
inline double length(Vec3 a) { return kern::length(kern::Vec3(a)); }
constexpr double length_sq(Vec3 a) { return kern::length_sq(kern::Vec3(a)); }
inline double distance(Vec3 a, Vec3 b) { return kern::distance(kern::Vec3(a), kern::Vec3(b)); }
constexpr double distance_sq(Vec3 a, Vec3 b) { return kern::distance_sq(kern::Vec3(a), kern::Vec3(b)); }
inline bool finite(Vec3 a) { return kern::finite(kern::Vec3(a)); }
constexpr Vec3 lerp(Vec3 a, Vec3 b, double t) { return kern::lerp(kern::Vec3(a), kern::Vec3(b), static_cast<float>(t)); }
constexpr Vec3 vmin(Vec3 a, Vec3 b) { return kern::vmin(kern::Vec3(a), kern::Vec3(b)); }
constexpr Vec3 vmax(Vec3 a, Vec3 b) { return kern::vmax(kern::Vec3(a), kern::Vec3(b)); }
inline Vec3 normalize(Vec3 a, Vec3 fallback = {0, 0, 1}) { return kern::normalize(kern::Vec3(a), kern::Vec3(fallback)); }

// the functions of numbers a formula does not have itself, as the converter writes them
inline double hypot_bp(double a, double b) { return kern::math::sqrt((a * a) + (b * b)); }
inline double ldexp_bp(double x, double n) { return (x * kern::math::pow(2.0, n)); }
inline double tanh_bp(double x) { return kern::math::div(kern::math::exp(2.0 * x) - 1.0, kern::math::exp(2.0 * x) + 1.0); }
inline double sinh_bp(double x) { return ((kern::math::exp(x) - kern::math::exp(-x)) * 0.5); }
inline double cosh_bp(double x) { return ((kern::math::exp(x) + kern::math::exp(-x)) * 0.5); }
inline double asinh_bp(double x) { return kern::math::log(x + kern::math::sqrt((x * x) + 1.0)); }
inline double log2_bp(double x) { return kern::math::div(kern::math::log(x), 0.6931471805599453); }
inline double log10_bp(double x) { return kern::math::div(kern::math::log(x), 2.302585092994046); }
inline double cbrt_bp(double x) { return (kern::math::sign(x) * kern::math::pow(kern::math::abs(x), 0.3333333333333333)); }
}  // namespace wide

/// The arguments a function is tried with: the same in every run.
struct Draw {
  Rng rng{0x5eed1234u};
  u32 below(u32 n) { return n ? rng.next() % n : 0; }
  f64 number() {
    switch (below(10)) {
      case 0: return 0.0;
      case 1: return below(2) ? 1.0 : -1.0;
      case 2:
      case 3: return (static_cast<int>(below(401)) - 200) / 100.0;  // (decimals of two digits: a threshold is met exactly)
      case 4: return static_cast<f64>(rng.range(-1.0f, 1.0f));
      case 5: return unit() * 2.0 - 1.0;   // (all the digits a 64-bit number has)
      case 6: return unit();
      case 7: return static_cast<f64>(rng.range(-50.0f, 50.0f));
      case 8: return static_cast<f64>(rng.range(0.0f, 1000.0f));
      default: return (static_cast<int>(below(41)) - 20) / 2.0;
    }
  }
  f64 unit() {
    const f64 high = rng.next(), low = rng.next();
    return (high + low / 4294967296.0) / 4294967296.0;
  }
  int whole() { return below(4) ? static_cast<int>(below(12)) - 2 : static_cast<int>(below(200)) - 20; }
  bool flag() { return below(2) != 0; }
  Vec3 vector() { return {static_cast<f32>(number()), static_cast<f32>(number()), static_cast<f32>(number())}; }
  Vec2 vector2() { return {static_cast<f32>(number()), static_cast<f32>(number())}; }
  const char* c_word() {
    static const char* const words[] = {"", "a", "Abc", "x1", "hello world", "north", "7", "A-B"};
    return words[below(8)];
  }
  std::string word() { return c_word(); }
};

/// A list of up to four drawn entries.
template <class Make> auto draw_list(Draw& draw, Make make) {
  std::vector<decltype(make())> out;
  for (u32 n = draw.below(5); n > 0; --n) out.push_back(make());
  return out;
}

// ---- a C++ value as the Blueprint gets it ----
template <class T> inline constexpr bool is_list = false;
template <class T> inline constexpr bool is_list<std::vector<T>> = true;

inline Value arg(f64 x) { return Value::number(x); }
inline Value arg(f32 x) { return Value::number(static_cast<f64>(x)); }
inline Value arg(bool x) { return Value::boolean(x); }
inline Value arg(Vec3 v) { return Value::vector(v); }
inline Value arg(Vec2 v) { return Value::vector({v.x, v.y, 0}); }
inline Value arg(wide::Vec3 v) { return arg(kern::Vec3(v)); }
inline Value arg(wide::Vec2 v) { return arg(kern::Vec2(v)); }
inline Value arg(const std::string& x) { return Value::text(x); }
inline Value arg(const char* x) { return Value::text(x ? x : ""); }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
Value arg(T x) { return Value::integer(static_cast<kern::i64>(x)); }
template <class E>
  requires std::is_enum_v<E>
Value arg(E e) { return Value::enumeration(static_cast<kern::i64>(e)); }
template <class T> Value arg(const std::vector<T>& v) {
  if constexpr (std::is_same_v<T, bool>) {
    return Value::of(std::vector<bool>(v.begin(), v.end()));
  } else if constexpr (std::is_floating_point_v<T>) {
    return Value::of(std::vector<f64>(v.begin(), v.end()));
  } else if constexpr (std::is_integral_v<T>) {
    return Value::of(std::vector<kern::i64>(v.begin(), v.end()));
  } else if constexpr (std::is_enum_v<T>) {
    std::vector<kern::i64> out;
    for (const T& x : v) out.push_back(static_cast<kern::i64>(x));
    return Value::enumerations(std::move(out));
  } else if constexpr (std::is_same_v<T, std::string>) {
    return Value::of(std::vector<std::string>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec3> || std::is_same_v<T, wide::Vec3>) {
    return Value::of(std::vector<Vec3>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec2> || std::is_same_v<T, wide::Vec2>) {
    std::vector<Vec3> out;
    for (const kern::Vec2 x : v) out.push_back({x.x, x.y, 0});
    return Value::of(std::move(out));
  } else if constexpr (is_list<T>) {   // (a list inside a list: a struct that holds it, as the Blueprint has it)
    std::vector<Value> out;
    for (const T& x : v) out.push_back(Value::structure({arg(x)}));
    return Value::structures(std::move(out));
  } else {
    std::vector<Value> out;
    for (const T& x : v) out.push_back(arg(x));
    return Value::structures(std::move(out));
  }
}

// ---- is what the Blueprint gives exactly what the source gives? ----
// (Numbers by their bits: 0.0 and -0.0 are two results.)
inline bool same_bits(f64 a, f64 b) { return std::bit_cast<kern::u64>(a) == std::bit_cast<kern::u64>(b); }
inline bool same(f64 want, const Value& got) { return same_bits(want, got.as_number()); }
inline bool same(f32 want, const Value& got) { return same_bits(static_cast<f64>(want), got.as_number()); }
inline bool same(bool want, const Value& got) { return got.as_bool() == want; }
inline bool same(Vec3 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return same_bits(want.x, v.x) && same_bits(want.y, v.y) && same_bits(want.z, v.z);
}
inline bool same(Vec2 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return same_bits(want.x, v.x) && same_bits(want.y, v.y);
}
inline bool same(wide::Vec3 want, const Value& got) { return same(kern::Vec3(want), got); }
inline bool same(wide::Vec2 want, const Value& got) { return same(kern::Vec2(want), got); }
inline bool same(const char* want, const Value& got) { return got.as_text() == (want ? want : ""); }
inline bool same(const std::string& want, const Value& got) { return got.as_text() == want; }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
bool same(T want, const Value& got) { return got.as_int() == static_cast<kern::i64>(want); }
template <class E>
  requires std::is_enum_v<E>
bool same(E want, const Value& got) { return got.i == static_cast<kern::i64>(want); }
template <class T> bool same(const std::vector<T>& want, const Value& got) {
  if constexpr (std::is_same_v<T, bool>) {
    const auto& g = got.items<bool>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (g[i] != want[i]) return false;
    return true;
  } else if constexpr (std::is_floating_point_v<T>) {
    const auto& g = got.items<f64>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same_bits(static_cast<f64>(want[i]), g[i])) return false;
    return true;
  } else if constexpr (std::is_integral_v<T> || std::is_enum_v<T>) {
    const auto& g = got.items<kern::i64>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (g[i] != static_cast<kern::i64>(want[i])) return false;
    return true;
  } else if constexpr (std::is_same_v<T, std::string>) {
    const auto& g = got.items<std::string>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (g[i] != want[i]) return false;
    return true;
  } else if constexpr (std::is_same_v<T, Vec3> || std::is_same_v<T, Vec2> || std::is_same_v<T, wide::Vec3> || std::is_same_v<T, wide::Vec2>) {
    const auto& g = got.items<Vec3>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same(want[i], Value::vector(g[i]))) return false;
    return true;
  } else if constexpr (is_list<T>) {
    const auto& g = got.items<Value>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!g[i].rows || g[i].rows->size() != 1 || !same(want[i], (*g[i].rows)[0])) return false;
    return true;
  } else {
    const auto& g = got.items<Value>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same(want[i], g[i])) return false;
    return true;
  }
}

/// A result there is something to compare with: every number in it is one.
inline bool defined(f32 x) { return std::isfinite(x); }
inline bool defined(f64 x) { return std::isfinite(x); }
inline bool defined(Vec3 v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); }
inline bool defined(Vec2 v) { return std::isfinite(v.x) && std::isfinite(v.y); }
inline bool defined(wide::Vec3 v) { return defined(kern::Vec3(v)); }
inline bool defined(wide::Vec2 v) { return defined(kern::Vec2(v)); }
inline bool defined(bool) { return true; }
inline bool defined(const std::string&) { return true; }
inline bool defined(const char*) { return true; }
template <class T>
  requires((std::is_integral_v<T> && !std::is_same_v<T, bool>) || std::is_enum_v<T>)
bool defined(T) { return true; }
template <class T> bool defined(const std::vector<T>& v) {
  for (const auto& x : v) if (!defined(static_cast<const T&>(x))) return false;
  return true;
}

// ---- values as text, for a failure's line ----
inline std::string text(f32 x) { char b[40]; std::snprintf(b, sizeof b, "%.9g", static_cast<f64>(x)); return b; }
inline std::string text(f64 x) { char b[40]; std::snprintf(b, sizeof b, "%.17g", x); return b; }
inline std::string text(bool x) { return x ? "true" : "false"; }
inline std::string text(Vec3 v) { return "{" + text(v.x) + ", " + text(v.y) + ", " + text(v.z) + "}"; }
inline std::string text(Vec2 v) { return "{" + text(v.x) + ", " + text(v.y) + "}"; }
inline std::string text(wide::Vec3 v) { return text(kern::Vec3(v)); }
inline std::string text(wide::Vec2 v) { return text(kern::Vec2(v)); }
inline std::string text(const char* s) { return std::string("\"") + (s ? s : "") + "\""; }
inline std::string text(const std::string& s) { return "\"" + s + "\""; }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
std::string text(T x) { return std::to_string(static_cast<long long>(x)); }
template <class E>
  requires std::is_enum_v<E>
std::string text(E e) { return "#" + std::to_string(static_cast<long long>(e)); }
template <class T> std::string text(const std::vector<T>& v) {
  std::string out = "[";
  for (std::size_t i = 0; i < v.size(); ++i) out += (i ? ", " : "") + text(static_cast<T>(v[i]));
  return out + "]";
}

/// A value of the Blueprint as text, every number with all its digits.
inline std::string shown(const Value& v) {
  using kern::logic::PinType;
  switch (v.type) {
    case PinType::number: return text(v.n);
    case PinType::vector: return text(v.v);
    case PinType::structure: {
      std::string out = "{";
      if (v.rows) for (std::size_t i = 0; i < v.rows->size(); ++i) out += (i ? ", " : "") + shown((*v.rows)[i]);
      return out + "}";
    }
    case PinType::number_list: return text(v.items<f64>());
    case PinType::structure_list: {
      std::string out = "[";
      const auto& l = v.items<Value>();
      for (std::size_t i = 0; i < l.size(); ++i) out += (i ? ", " : "") + shown(l[i]);
      return out + "]";
    }
    default: return v.as_text();
  }
}

/// A converted source file's Blueprint, in the interpreter and as its compiled class.
struct Blueprint {
  RuleBlueprint interpreted, compiled;
  explicit Blueprint(const char* actor) : interpreted(actor, [](kern::logic::Instance*) {}, false), compiled(actor, [](kern::logic::Instance*) {}, true) {
    SC_CHECK(interpreted.inst && compiled.inst, "%s: %s%s", actor, interpreted.problems.c_str(), compiled.problems.c_str());
    SC_CHECK(interpreted.problems.empty(), "%s: %s", actor, interpreted.problems.c_str());
  }
};

/// The comparison of one function over its calls.
struct Tally {
  const char* function;
  int compared = 0, outside = 0, wrong = 0;
  explicit Tally(const char* f) : function(f) {}
  /// One call. `want`: what the source gives, computed with the Blueprint language's numbers;
  /// `left`: the source divided by zero or left the numbers on the way - nothing to compare.
  template <class Want, class Describe> void take(Blueprint& bp, const std::vector<Value>& args, const Want& want, bool left, Describe describe) {
    if (!bp.interpreted.inst || !bp.compiled.inst) return;
    if (left || !defined(want)) { ++outside; return; }
    std::vector<Value> a, b;
    const bool ia = bp.interpreted.inst->call(function, args, a), ib = bp.compiled.inst->call(function, args, b);
    ++compared;
    if (ia && ib && a.size() == 1 && b.size() == 1 && a[0] == b[0] && same(want, a[0]) && same(want, b[0])) return;
    if (++wrong <= 3)
      std::printf("  %s(%s): the source gives %s, the Blueprint %s (interpreter) and %s (class)\n", function, describe().c_str(), text(want).c_str(),
                  ia && a.size() == 1 ? shown(a[0]).c_str() : "nothing", ib && b.size() == 1 ? shown(b[0]).c_str() : "nothing");
  }
  ~Tally() {
    SC_CHECK(wrong == 0, "%s: %d of %d calls differ", function, wrong, compared);
    SC_CHECK(compared >= 20, "%s: only %d calls could be compared (in %d the source left the numbers)", function, compared, outside);
  }
};

// ---- the places C++ leaves open ----
// What an entry outside a list, a division of whole numbers by zero or a shift by the width of the number
// or more gives, C++ does not say. The source in a test goes through these there: they give the value where
// there is one and mark the call where there is none - such a call is then not compared (see Watch).
inline bool g_open = false;

template <class C, class I> auto at(C&& c, I i) -> decltype(c[0]) {
  using R = decltype(c[0]);
  const long long n = static_cast<long long>(std::size(c)), k = static_cast<long long>(i);
  if (k >= 0 && k < n) return c[static_cast<std::size_t>(k)];
  g_open = true;
  if constexpr (std::is_reference_v<R>) {
    static std::remove_cvref_t<R> none;
    none = std::remove_cvref_t<R>{};
    return none;
  } else {
    static std::vector<bool> none(1);
    none[0] = false;
    return none[0];
  }
}
/// Taking out an entry a list does not have.
template <class C> void pop_back(C& c) {
  if (c.empty()) {
    g_open = true;
    return;
  }
  c.pop_back();
}
template <class C, class It> auto erase(C& c, It at) -> decltype(c.erase(at)) {
  if (at < c.begin() || !(at < c.end())) {
    g_open = true;
    return c.end();
  }
  return c.erase(at);
}
template <class C, class A, class B> auto erase(C& c, A first, B last) -> decltype(c.erase(first, last)) {
  if (first < c.begin() || c.end() < last || last < first) {
    g_open = true;
    return c.end();
  }
  return c.erase(first, last);
}
template <class C> auto front(C&& c) -> decltype(c[0]) { return at(c, 0); }
template <class C> auto back(C&& c) -> decltype(c[0]) { return at(c, static_cast<long long>(std::size(c)) - 1); }

template <class A, class B> constexpr auto quot(A a, B b) -> decltype(a / b) {
  using R = decltype(a / b);
  if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<long long>(b) == -1)) {
    g_open = true;
    return 0;
  }
  return a / b;
}
template <class A, class B> constexpr auto rem(A a, B b) -> decltype(a % b) {
  using R = decltype(a % b);
  if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<long long>(b) == -1)) {
    g_open = true;
    return 0;
  }
  return a % b;
}
template <class A, class B> constexpr auto shl(A a, B b) -> decltype(a << b) {
  using R = decltype(a << b);
  const long long by = static_cast<long long>(b);
  if (by < 0 || by >= static_cast<long long>(sizeof(R) * 8)) {
    g_open = true;
    return 0;
  }
  return a << b;
}
template <class A, class B> constexpr auto shr(A a, B b) -> decltype(a >> b) {
  using R = decltype(a >> b);
  const long long by = static_cast<long long>(b);
  if (by < 0 || by >= static_cast<long long>(sizeof(R) * 8)) {
    g_open = true;
    return 0;
  }
  return a >> b;
}
template <class A, class B> constexpr A& quot_to(A& a, B b) { return a = static_cast<A>(quot(a, b)); }
template <class A, class B> constexpr A& rem_to(A& a, B b) { return a = static_cast<A>(rem(a, b)); }
template <class A, class B> constexpr A& shl_to(A& a, B b) { return a = static_cast<A>(shl(a, b)); }
template <class A, class B> constexpr A& shr_to(A& a, B b) { return a = static_cast<A>(shr(a, b)); }

/// Runs the source and says whether it left the numbers on the way (see Tally::take): a division by zero
/// or a computation without a result, or one of the places above.
struct Watch {
  Watch() {
    std::feclearexcept(FE_ALL_EXCEPT);
    g_open = false;
  }
  bool left() const { return g_open || std::fetestexcept(FE_DIVBYZERO | FE_INVALID) != 0; }
};

inline constexpr int kCalls = 300;

}  // namespace sc::converted
