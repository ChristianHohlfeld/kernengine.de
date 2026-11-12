// What the generated tests of tests/converted/ share (tools/cpp_to_blueprint.py --test writes them):
// the Blueprint a source file was converted to, running next to the C++ it was read from. Every
// converted function is called with the same arguments in C++, in the interpreter and as the
// Blueprint's compiled class; the three results are compared.
#pragma once

#include <cfenv>
#include <cmath>
#include <cstdio>
#include <string>
#include <type_traits>
#include <vector>

#include "../src/blueprint_call.h"
#include "../src/common.h"
#include "rule_blueprint.h"
#include "test_util.h"

namespace sc::converted {

using Value = kern::logic::Value;

/// The arguments a function is tried with: the same in every run.
struct Draw {
  Rng rng{0x5eed1234u};
  u32 below(u32 n) { return n ? rng.next() % n : 0; }
  f32 number() {
    switch (below(10)) {
      case 0: return 0.0f;
      case 1: return below(2) ? 1.0f : -1.0f;
      case 2:
      case 3: return static_cast<f32>(static_cast<int>(below(401)) - 200) / 100.0f;  // (decimals of two digits: a threshold is met exactly)
      case 4:
      case 5: return rng.range(-1.0f, 1.0f);
      case 6: return rng.range(0.0f, 1.0f);
      case 7: return rng.range(-50.0f, 50.0f);
      case 8: return rng.range(0.0f, 1000.0f);
      default: return static_cast<f32>(static_cast<int>(below(41)) - 20) / 2.0f;
    }
  }
  int whole() { return below(4) ? static_cast<int>(below(12)) - 2 : static_cast<int>(below(200)) - 20; }
  bool flag() { return below(2) != 0; }
  Vec3 vector() { return {number(), number(), number()}; }
  Vec2 vector2() { return {number(), number()}; }
  std::string word() {
    static const char* const words[] = {"", "a", "Abc", "x1", "hello world", "north", "7", "A-B"};
    return words[below(8)];
  }
};

// ---- a C++ value as the Blueprint gets it ----
inline Value arg(f32 x) { return rules::number(x); }
inline Value arg(f64 x) { return Value::number(x); }
inline Value arg(bool x) { return Value::boolean(x); }
inline Value arg(Vec3 v) { return Value::vector(v); }
inline Value arg(Vec2 v) { return Value::vector({v.x, v.y, 0}); }
inline Value arg(const std::string& x) { return Value::text(x); }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
Value arg(T x) { return Value::integer(static_cast<kern::i64>(x)); }
template <class E>
  requires std::is_enum_v<E>
Value arg(E e) { return Value::enumeration(static_cast<kern::i64>(e)); }

// ---- is what the Blueprint gives what the C++ gives? ----
/// The C++ computes in 32 bit, the Blueprint in 64: equal means equal as far as 32 bit can tell.
inline bool close(f64 want, f64 got) { return std::fabs(want - got) <= 2e-4 * std::fmax(1.0, std::fmax(std::fabs(want), std::fabs(got))); }
inline bool same(f32 want, const Value& got) { return close(want, got.as_number()); }
inline bool same(f64 want, const Value& got) { return close(want, got.as_number()); }
inline bool same(bool want, const Value& got) { return got.as_bool() == want; }
inline bool same(Vec3 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return close(want.x, v.x) && close(want.y, v.y) && close(want.z, v.z);
}
inline bool same(Vec2 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return close(want.x, v.x) && close(want.y, v.y);
}
inline bool same(const char* want, const Value& got) { return got.as_text() == (want ? want : ""); }
inline bool same(const std::string& want, const Value& got) { return got.as_text() == want; }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
bool same(T want, const Value& got) { return got.as_int() == static_cast<kern::i64>(want); }
template <class E>
  requires std::is_enum_v<E>
bool same(E want, const Value& got) { return got.i == static_cast<kern::i64>(want); }

/// A result the comparison has a meaning for: the C++ of a function called outside what it is
/// made for (the root of a negative number, a division by zero) gives no number; the Blueprint's
/// arithmetic gives one everywhere. Such a call compares nothing.
inline bool defined(f32 x) { return std::isfinite(x); }
inline bool defined(f64 x) { return std::isfinite(x); }
inline bool defined(Vec3 v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); }
inline bool defined(Vec2 v) { return std::isfinite(v.x) && std::isfinite(v.y); }
template <class T> bool defined(const T&) { return true; }

// ---- the neighbours of an argument ----
/// The k-th neighbour of a 32-bit number: up to two of the smallest steps it can make, up or down.
/// Where a function jumps (a threshold, the wrap of an angle), the last bit of an argument decides
/// on which side a call lands - and the 32-bit C++ and the 64-bit Blueprint may round that bit
/// differently. The neighbours show both sides of such a jump.
inline f32 near(f32 x, int k, int slot) {
  const int steps = static_cast<int>(kern::hash_u32(static_cast<u32>(k * 131 + slot)) % 5u) - 2;
  for (int i = 0; i < steps; ++i) x = std::nextafter(x, 3.0e38f);
  for (int i = 0; i > steps; --i) x = std::nextafter(x, -3.0e38f);
  return x;
}
inline f64 near(f64 x, int k, int slot) { return static_cast<f64>(near(static_cast<f32>(x), k, slot)) + (x - static_cast<f64>(static_cast<f32>(x))); }
inline Vec3 near(Vec3 v, int k, int slot) { return {near(v.x, k, slot * 3 + 11), near(v.y, k, slot * 3 + 12), near(v.z, k, slot * 3 + 13)}; }
inline Vec2 near(Vec2 v, int k, int slot) { return {near(v.x, k, slot * 3 + 11), near(v.y, k, slot * 3 + 12)}; }
template <class T>
  requires(std::is_integral_v<T> || std::is_enum_v<T>)
T near(T x, int, int) { return x; }

// ---- values as text, for a failure's line ----
inline std::string text(f32 x) { char b[40]; std::snprintf(b, sizeof b, "%.9g", static_cast<f64>(x)); return b; }
inline std::string text(f64 x) { char b[40]; std::snprintf(b, sizeof b, "%.17g", x); return b; }
inline std::string text(bool x) { return x ? "true" : "false"; }
inline std::string text(Vec3 v) { return "{" + text(v.x) + ", " + text(v.y) + ", " + text(v.z) + "}"; }
inline std::string text(Vec2 v) { return "{" + text(v.x) + ", " + text(v.y) + "}"; }
inline std::string text(const char* s) { return std::string("\"") + (s ? s : "") + "\""; }
inline std::string text(const std::string& s) { return "\"" + s + "\""; }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
std::string text(T x) { return std::to_string(static_cast<long long>(x)); }
template <class E>
  requires std::is_enum_v<E>
std::string text(E e) { return "#" + std::to_string(static_cast<long long>(e)); }
inline std::string text(const Value& v) { return v.as_text(); }

inline bool near(bool x, int, int) { return x; }
inline std::string near(std::string x, int, int) { return x; }

// ---- lists ----
/// A list of up to four drawn entries.
template <class Make> auto draw_list(Draw& draw, Make make) {
  std::vector<decltype(make())> out;
  for (u32 n = draw.below(5); n > 0; --n) out.push_back(make());
  return out;
}
template <class T> Value arg(const std::vector<T>& v) {
  if constexpr (std::is_same_v<T, bool>) {
    return Value::of(std::vector<bool>(v.begin(), v.end()));
  } else if constexpr (std::is_floating_point_v<T>) {
    std::vector<f64> out;
    for (const T& x : v) out.push_back(arg(x).as_number());
    return Value::of(std::move(out));
  } else if constexpr (std::is_integral_v<T>) {
    return Value::of(std::vector<kern::i64>(v.begin(), v.end()));
  } else if constexpr (std::is_enum_v<T>) {
    std::vector<kern::i64> out;
    for (const T& x : v) out.push_back(static_cast<kern::i64>(x));
    return Value::enumerations(std::move(out));
  } else if constexpr (std::is_same_v<T, std::string>) {
    return Value::of(std::vector<std::string>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec3>) {
    return Value::of(std::vector<Vec3>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec2>) {
    std::vector<Vec3> out;
    for (const Vec2& x : v) out.push_back({x.x, x.y, 0});
    return Value::of(std::move(out));
  } else {
    std::vector<Value> out;
    for (const T& x : v) out.push_back(arg(x));
    return Value::structures(std::move(out));
  }
}
template <class T> bool same(const std::vector<T>& want, const Value& got) {
  if constexpr (std::is_same_v<T, bool>) {
    const auto& g = got.items<bool>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (g[i] != want[i]) return false;
    return true;
  } else if constexpr (std::is_floating_point_v<T>) {
    const auto& g = got.items<f64>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!close(want[i], g[i])) return false;
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
  } else if constexpr (std::is_same_v<T, Vec3> || std::is_same_v<T, Vec2>) {
    const auto& g = got.items<Vec3>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same(want[i], Value::vector(g[i]))) return false;
    return true;
  } else {
    const auto& g = got.items<Value>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same(want[i], g[i])) return false;
    return true;
  }
}
template <class T> bool defined(const std::vector<T>& v) {
  for (const auto& x : v) if (!defined(static_cast<const T&>(x))) return false;
  return true;
}
template <class T> std::string text(const std::vector<T>& v) {
  std::string out = "[";
  for (std::size_t i = 0; i < v.size(); ++i) out += (i ? ", " : "") + text(static_cast<T>(v[i]));
  return out + "]";
}
template <class T> std::vector<T> near(std::vector<T> v, int k, int slot) {
  if constexpr (!std::is_same_v<T, bool>)
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = near(v[i], k, slot * 5 + static_cast<int>(i) + 3);
  return v;
}

// ---- a value as its numbers ----
/// Every number in a value, in order (a truth value as 0 or 1, a text as a number that is its
/// own, a list as its length and its entries): what the C++ gives and what the Blueprint gives
/// are laid next to each other number by number.
inline void flat(f32 x, std::vector<f64>& o) { o.push_back(x); }
inline void flat(f64 x, std::vector<f64>& o) { o.push_back(x); }
inline void flat(bool x, std::vector<f64>& o) { o.push_back(x ? 1 : 0); }
inline void flat(Vec3 v, std::vector<f64>& o) { o.insert(o.end(), {v.x, v.y, v.z}); }
inline void flat(Vec2 v, std::vector<f64>& o) { o.insert(o.end(), {v.x, v.y, 0}); }
inline void flat(const std::string& x, std::vector<f64>& o) { o.push_back(static_cast<f64>(kern::hash_bytes(x.data(), x.size()) & 0xffffffffu)); }
inline void flat(const char* x, std::vector<f64>& o) { flat(std::string(x ? x : ""), o); }
template <class T>
  requires((std::is_integral_v<T> && !std::is_same_v<T, bool>) || std::is_enum_v<T>)
void flat(T x, std::vector<f64>& o) { o.push_back(static_cast<f64>(static_cast<kern::i64>(x))); }
template <class T> void flat(const std::vector<T>& v, std::vector<f64>& o) {
  o.push_back(static_cast<f64>(v.size()));
  for (const auto& x : v) flat(static_cast<T>(x), o);
}
inline void flat(const Value& v, std::vector<f64>& o) {
  using kern::logic::PinType;
  switch (v.type) {
    case PinType::boolean: o.push_back(v.b ? 1 : 0); break;
    case PinType::integer:
    case PinType::enumeration: o.push_back(static_cast<f64>(v.i)); break;
    case PinType::number: o.push_back(v.n); break;
    case PinType::vector: flat(v.v, o); break;
    case PinType::text: flat(v.s, o); break;
    case PinType::structure:
      if (v.rows) for (const Value& f : *v.rows) flat(f, o);
      break;
    case PinType::boolean_list: { const auto& l = v.items<bool>(); o.push_back(static_cast<f64>(l.size())); for (bool x : l) o.push_back(x ? 1 : 0); } break;
    case PinType::integer_list:
    case PinType::enumeration_list: { const auto& l = v.items<kern::i64>(); o.push_back(static_cast<f64>(l.size())); for (kern::i64 x : l) o.push_back(static_cast<f64>(x)); } break;
    case PinType::number_list: { const auto& l = v.items<f64>(); o.push_back(static_cast<f64>(l.size())); for (f64 x : l) o.push_back(x); } break;
    case PinType::vector_list: { const auto& l = v.items<Vec3>(); o.push_back(static_cast<f64>(l.size())); for (Vec3 x : l) flat(x, o); } break;
    case PinType::text_list: { const auto& l = v.items<std::string>(); o.push_back(static_cast<f64>(l.size())); for (const std::string& x : l) flat(x, o); } break;
    case PinType::structure_list: { const auto& l = v.items<Value>(); o.push_back(static_cast<f64>(l.size())); for (const Value& x : l) flat(x, o); } break;
    default: break;
  }
}

/// An argument as the 64-bit run of the C++ gets it: a 32-bit number as the decimal it stands
/// for - what the Blueprint gets (rules::decimal).
inline f64 wide_arg(f32 x) { return rules::decimal(x); }
template <class T> const T& wide_arg(const T& x) { return x; }

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
  int compared = 0, undefined = 0, as_wide = 0, at_a_jump = 0, wrong = 0;
  explicit Tally(const char* f) : function(f) {}
  /// One call. `want`: what the C++ gives; `outside`: the C++ divided by zero or computed something
  /// that is no number on the way (the Blueprint's arithmetic gives a number there: x / 0 is 0) -
  /// such a call compares nothing; `around(k)`: what the C++ gives for the k-th neighbour of the
  /// arguments.
  /// `wide`: what the same C++ gives when its numbers are 64-bit, as a Blueprint's are (the
  /// test compiles the function a second time with f32 as double): where the 32-bit run has lost
  /// digits on the way - a loop that adds up, a product with a large number - the Blueprint is this.
  template <class Want, class Wide, class Describe, class Around>
  void take(Blueprint& bp, const std::vector<Value>& args, const Want& want, const Wide& wide, bool outside, Describe describe, Around around) {
    if (!bp.interpreted.inst || !bp.compiled.inst) return;
    if (outside || !defined(want)) { ++undefined; return; }
    std::vector<Value> a, b;
    const bool ia = bp.interpreted.inst->call(function, args, a), ib = bp.compiled.inst->call(function, args, b);
    ++compared;
    const bool both = ia && ib && a.size() == 1 && b.size() == 1 && a[0] == b[0];
    if (both && same(want, a[0])) return;
    if (both && defined(wide) && same(wide, a[0])) { ++as_wide; return; }
    if (both) {
      // Is the difference of the size the last bits of an argument make in the C++ itself? Its
      // neighbours say: every number of the Blueprint's result lies where the C++ results around the
      // call lie (a threshold, the wrap of an angle, the sine of a large number).
      std::vector<f64> g, lo, hi, w;
      flat(a[0], g);
      const auto widen = [&](const Want& x) {
        w.clear();
        flat(x, w);
        if (w.size() != g.size()) return;
        if (lo.empty()) { lo = hi = w; return; }
        for (std::size_t i = 0; i < w.size(); ++i) { lo[i] = std::fmin(lo[i], w[i]); hi[i] = std::fmax(hi[i], w[i]); }
      };
      widen(want);
      for (int k = 1; k <= 48; ++k) {
        const Want x = around(k);
        if (defined(x)) widen(x);
      }
      bool inside = !lo.empty() || g.empty();
      for (std::size_t i = 0; inside && i < lo.size(); ++i) {
        // (what the 32-bit run rounds on the way is of the size of a few last bits of its arguments:
        // the Blueprint may lie a few widths of the neighbours' spread beside them)
        const f64 slack = 2e-4 * std::fmax(1.0, std::fmax(std::fabs(g[i]), std::fmax(std::fabs(lo[i]), std::fabs(hi[i])))) + 3 * (hi[i] - lo[i]);
        inside = g[i] >= lo[i] - slack && g[i] <= hi[i] + slack;
      }
      if (inside) { ++at_a_jump; return; }
    }
    if (++wrong <= 3)
      std::printf("  %s(%s): the C++ gives %s (in 64 bit: %s), the Blueprint %s (interpreter) and %s (class)\n", function, describe().c_str(), text(want).c_str(), text(wide).c_str(),
                  ia && a.size() == 1 ? a[0].as_text().c_str() : "nothing", ib && b.size() == 1 ? b[0].as_text().c_str() : "nothing");
  }
  ~Tally() {
    SC_CHECK(wrong == 0, "%s: %d of %d calls differ", function, wrong, compared);
    SC_CHECK(compared >= 20, "%s: only %d calls could be compared (%d gave no number in C++)", function, compared, undefined);
    SC_CHECK(at_a_jump * 4 <= compared, "%s: %d of %d calls differ at a jump of the function - too many to be the last bit", function, at_a_jump, compared);
  }
};

/// Runs the C++ and says whether it left the numbers on the way (see Tally::take).
struct Watch {
  Watch() { std::feclearexcept(FE_ALL_EXCEPT); }
  bool outside() const { return std::fetestexcept(FE_DIVBYZERO | FE_INVALID) != 0; }
};

inline constexpr int kCalls = 300;

}  // namespace sc::converted
