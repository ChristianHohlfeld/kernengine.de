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

/// An actor on a pin (`Actor` in a Blueprint's C++): its id, 0 = none.
struct ActorHandle {
  ActorRef id = 0;
  bool operator==(const ActorHandle&) const = default;
};

/// A value of the interpreter as the C++ type of its pin, and back.
template <class T> T from_value(const Value& v) = delete;
template <> inline bool from_value<bool>(const Value& v) { return v.as_bool(); }
template <> inline i64 from_value<i64>(const Value& v) { return v.as_int(); }
template <> inline f64 from_value<f64>(const Value& v) { return v.as_number(); }
template <> inline Vec3 from_value<Vec3>(const Value& v) { return v.as_vector(); }
template <> inline std::string from_value<std::string>(const Value& v) { return v.as_text(); }
template <> inline ActorHandle from_value<ActorHandle>(const Value& v) { return {v.as_actor()}; }
inline Value to_value(bool v) { return Value::boolean(v); }
inline Value to_value(i64 v) { return Value::integer(v); }
inline Value to_value(f64 v) { return Value::number(v); }
inline Value to_value(Vec3 v) { return Value::vector(v); }
inline Value to_value(std::string v) { return Value::text(std::move(v)); }
inline Value to_value(ActorHandle v) { return Value::actor(v.id); }
template <class T> constexpr PinType pin_type_of() = delete;
template <> constexpr PinType pin_type_of<bool>() { return PinType::boolean; }
template <> constexpr PinType pin_type_of<i64>() { return PinType::integer; }
template <> constexpr PinType pin_type_of<f64>() { return PinType::number; }
template <> constexpr PinType pin_type_of<Vec3>() { return PinType::vector; }
template <> constexpr PinType pin_type_of<std::string>() { return PinType::text; }
template <> constexpr PinType pin_type_of<ActorHandle>() { return PinType::actor; }

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
inline void native_set(f64& variable, const Value& v) { variable = v.as_number(); }
inline void native_set(Vec3& variable, const Value& v) { variable = v.as_vector(); }
inline void native_set(std::string& variable, const Value& v) { variable = v.as_text(); }
inline void native_set(ActorHandle& variable, const Value& v) { variable = {v.as_actor()}; }

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
