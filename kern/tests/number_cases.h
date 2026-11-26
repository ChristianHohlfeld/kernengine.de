// The numbers of a Blueprint against C++ (kern/logic/numbers.h): every operation of every number type gives what
// C++ gives - with the edge values (going round of the unsigned types, signs, shifts at the width, division and
// remainder of negative numbers, NaN, inf, -0, denormals, f32 intermediate values, 64 bits beyond 2^53) - and where
// C++ leaves the result open, the one result numbers.h fixes. Included by test_main.cpp (TEST, CHECK).
#pragma once

#include <bit>
#include <cmath>
#include <limits>
#include <vector>

#include "kern/core/json.h"
#include "kern/logic/logic.h"
#include "kern/logic/numbers.h"
#include "logic_cases.h"

namespace number_cases {
using namespace kern;
namespace num = kern::logic::num;

/// The same value: the same bits - of two NaNs, both NaN (neither C++ nor WebAssembly fix a NaN's bits).
template <class T> bool same(T a, T b) {
  // (the language's rule, numbers.h seen: two NaNs are the one NaN, every other value its bits - -0 is not 0)
  if constexpr (num::Number<T>) return num::same_seen(a, b);
  else return a == b;
}

/// The edge values of a whole type: 0, 1, 2, 3, 7, its ends and their neighbours, -1 .. -3 and the signs' borders,
/// a pattern of bits, and (of the 64-bit types) numbers beyond 2^53.
template <class T> std::vector<T> edges() {
  using L = std::numeric_limits<T>;
  std::vector<T> v = {T(0), T(1), T(2), T(3), T(7), T(100), L::max(), T(L::max() - 1), L::min(), T(L::min() + 1), T(L::max() / 2), T(L::max() / 2 + 1), static_cast<T>(0x5555555555555555ull), static_cast<T>(0xa5a5a5a5a5a5a5a5ull)};
  if constexpr (std::is_signed_v<T>) for (T x : {T(-1), T(-2), T(-3), T(-7), T(-100)}) v.push_back(x);
  if constexpr (sizeof(T) == 8) for (u64 x : {(u64{1} << 53), (u64{1} << 53) + 1, (u64{1} << 63) - 1, u64{1469598103934665603ull}, u64{0xcbf29ce484222325ull}}) v.push_back(static_cast<T>(x));
  return v;
}
template <class T> std::vector<T> float_edges() {
  using L = std::numeric_limits<T>;
  return {T(0), -T(0), T(1), T(-1), T(0.1), T(-0.1), T(1) / T(3), T(2.5), T(-2.5), T(0.5), T(-0.5), T(1.5), T(16777216), T(16777217), T(9007199254740993.0), L::max(), -L::max(), L::min(), L::denorm_min(), -L::denorm_min(), L::epsilon(), L::infinity(), -L::infinity(), L::quiet_NaN(), T(1e10), T(-1e10), T(3e9), T(-3e9), T(4294967295.5), T(9.3e18), T(1.9e19), T(-9.3e18), T(255.9), T(-128.9), T(-129.5), T(127.9)};
}

int failures = 0;
template <class T> void expect(bool ok, const char* what, T a, T b = T(0)) {
  if (ok) return;
  if (++failures <= 20) std::printf("  numbers: %s (%s) wrong for %.17g, %.17g\n", what, num::kind_name(num::kind_of<T>()).data(), static_cast<double>(a), static_cast<double>(b));
}

/// Whole numbers: + - * against the builtins (which go round in two's complement, as numbers.h fixes for a
/// signed overflow), / % against C++ and its laws, shifts against C++ below the width.
template <class T> void whole_numbers() {
  for (T a : edges<T>()) {
    for (T b : edges<T>()) {
      T r;
      __builtin_add_overflow(a, b, &r);
      expect(num::add(a, b) == r, "add", a, b);
      __builtin_sub_overflow(a, b, &r);
      expect(num::sub(a, b) == r, "sub", a, b);
      __builtin_mul_overflow(a, b, &r);
      expect(num::mul(a, b) == r, "mul", a, b);
      if (b == 0) {
        expect(num::div(a, b) == 0 && num::rem(a, b) == 0, "by 0", a, b);
      } else if (std::is_signed_v<T> && a == std::numeric_limits<T>::min() && b == T(-1)) {
        expect(num::div(a, b) == a && num::rem(a, b) == 0, "smallest by -1", a, b);
      } else {
        expect(num::div(a, b) == static_cast<T>(a / b) && num::rem(a, b) == static_cast<T>(a % b), "div rem", a, b);
        // (C++'s laws: toward zero, the remainder has a's sign)
        expect(static_cast<T>(num::add(num::mul(num::div(a, b), b), num::rem(a, b))) == a, "div*b+rem", a, b);
        expect(num::rem(a, b) == 0 || ((num::rem(a, b) < 0) == (a < 0)), "rem sign", a, b);
      }
      expect(num::bit_and(a, b) == static_cast<T>(a & b) && num::bit_or(a, b) == static_cast<T>(a | b) && num::bit_xor(a, b) == static_cast<T>(a ^ b), "bits", a, b);
      expect(num::lt(a, b) == (a < b) && num::le(a, b) == (a <= b) && num::gt(a, b) == (a > b) && num::ge(a, b) == (a >= b) && num::eq(a, b) == (a == b) && num::ne(a, b) == (a != b), "compare", a, b);
      expect(num::min(a, b) == std::min(a, b) && num::max(a, b) == std::max(a, b), "min max", a, b);
    }
    T n;
    __builtin_sub_overflow(T(0), a, &n);
    expect(num::neg(a) == n, "neg", a);
    expect(num::abs(a) == (std::is_signed_v<T> && a < 0 ? n : a), "abs", a);
    expect(num::bit_not(a) == static_cast<T>(~a), "not", a);
    constexpr int w = sizeof(T) * 8;
    for (int s = 0; s < w; ++s) {
      // (C++20: << of any int and >> of a signed one are defined below the width; a narrow type is promoted to int)
      expect(num::shl(a, s) == static_cast<T>(static_cast<std::conditional_t<(sizeof(T) < 4), std::conditional_t<std::is_signed_v<T>, i32, u32>, T>>(a) << s), "shl", a, T(s));
      expect(num::shr(a, s) == static_cast<T>(a >> s), "shr", a, T(s));
      expect(num::shl(a, s + w) == num::shl(a, s) && num::shr(a, s + w) == num::shr(a, s), "shift mod width", a, T(s));
    }
    expect(num::shl(a, -1) == num::shl(a, w - 1) && num::shr(a, i64{-1}) == num::shr(a, w - 1), "shift by -1", a);
    expect(num::popcount(a) == std::popcount(static_cast<std::make_unsigned_t<T>>(a)), "popcount", a);
  }
}

/// The whole numbers ldexp is given: none, small ones, into the subnormal numbers and beyond both ends, int's own ends.
inline const std::vector<int>& ldexp_edges() {
  static const std::vector<int> v = {0, 1, -1, 16, -16, 127, 128, -126, -149, -150, 1023, 1024, -1074, -1075, 2098, -2099, std::numeric_limits<int>::max(), std::numeric_limits<int>::min()};
  return v;
}

/// Floating numbers: + - * / against C++ in the type itself (f32 rounds every intermediate value), the functions of
/// <cmath> against C++, sin, exp ... against the engine's musl functions.
template <class T> void floating_numbers() {
  for (T a : float_edges<T>()) {
    for (T b : float_edges<T>()) {
      expect(same(num::add(a, b), static_cast<T>(a + b)), "add", a, b);
      expect(same(num::sub(a, b), static_cast<T>(a - b)), "sub", a, b);
      expect(same(num::mul(a, b), static_cast<T>(a * b)), "mul", a, b);
      expect(same(num::div(a, b), static_cast<T>(a / b)), "div", a, b);
      expect(same(num::rem(a, b), static_cast<T>(std::fmod(a, b))), "fmod", a, b);
      expect(num::lt(a, b) == (a < b) && num::le(a, b) == (a <= b) && num::eq(a, b) == (a == b) && num::ne(a, b) == (a != b), "compare", a, b);
      expect(same(num::min(a, b), std::min(a, b)) && same(num::max(a, b), std::max(a, b)), "min max", a, b);
      if (a == 0 && b == 0)   // (of 0 and -0 C leaves it open: numbers.h gives -0 and 0)
        expect(same(num::fmin(a, b), (std::signbit(a) || std::signbit(b)) ? -T(0) : T(0)) && same(num::fmax(a, b), (std::signbit(a) && std::signbit(b)) ? -T(0) : T(0)), "fmin fmax of zeros", a, b);
      else expect(same(num::fmin(a, b), std::fmin(a, b)) && same(num::fmax(a, b), std::fmax(a, b)), "fmin fmax", a, b);
      expect(same(num::copysign(a, b), std::copysign(a, b)), "copysign", a, b);
      expect(same(num::atan2(a, b), static_cast<T>(dm::atan2(a, b))) && same(num::pow(a, b), static_cast<T>(dm::pow(a, b))) && same(num::hypot(a, b), static_cast<T>(dm::hypot(a, b))), "atan2 pow hypot", a, b);
      // (remainder: IEEE 754's, exact - so C++'s own std::remainder gives the same bits, also by 0 and of infinity: NaN)
      expect(same(num::remainder(a, b), std::remainder(a, b)), "remainder", a, b);
      for (T c : {T(0), T(1), T(-0.01), T(0.3)}) {
        const T r = a * b + c;  // (C++ in T: -ffp-contract=off, no FMA - each step rounded to T)
        expect(same(num::add(num::mul(a, b), c), r), "a*b+c", a, b);
      }
    }
    expect(same(num::neg(a), static_cast<T>(-a)) && same(num::abs(a), std::fabs(a)), "neg abs", a);
    expect(same(num::sqrt(a), std::sqrt(a)) && same(num::floor(a), std::floor(a)) && same(num::ceil(a), std::ceil(a)) && same(num::trunc(a), std::trunc(a)) && same(num::round(a), std::round(a)), "sqrt floor ceil trunc round", a);
    if constexpr (std::is_same_v<T, f32>) {
      expect(same(num::sin(a), kern_musl_sinf(a)) && same(num::cos(a), kern_musl_cosf(a)) && same(num::tan(a), kern_musl_tanf(a)) && same(num::asin(a), kern_musl_asinf(a)) && same(num::acos(a), kern_musl_acosf(a)) && same(num::atan(a), kern_musl_atanf(a)), "trig (musl f)", a);
      expect(same(num::exp(a), kern_musl_expf(a)) && same(num::expm1(a), kern_musl_expm1f(a)) && same(num::log(a), kern_musl_logf(a)) && same(num::log10(a), kern_musl_log10f(a)), "exp log (musl f)", a);
      expect(same(num::sinh(a), kern_math_sinhf(a)) && same(num::cosh(a), kern_math_coshf(a)) && same(num::tanh(a), kern_math_tanhf(a)) && same(num::asinh(a), kern_math_asinhf(a)) && same(num::acosh(a), kern_math_acoshf(a)) && same(num::atanh(a), kern_math_atanhf(a)), "hyperbolic (the engine's f)", a);
      expect(same(num::log2(a), kern_math_log2f(a)) && same(num::log1p(a), kern_math_log1pf(a)) && same(num::exp2(a), kern_math_exp2f(a)) && same(num::cbrt(a), kern_math_cbrtf(a)), "log2 log1p exp2 cbrt (the engine's f)", a);
      for (int n : ldexp_edges()) expect(same(num::ldexp(a, n), kern_math_ldexpf(a, n)) && same(num::ldexp(a, n), std::ldexp(a, n)), "ldexp (the engine's f, exact: C++'s)", a, T(n));
    } else {
      expect(same(num::sin(a), kern_musl_sin(a)) && same(num::cos(a), kern_musl_cos(a)) && same(num::tan(a), kern_musl_tan(a)) && same(num::asin(a), kern_musl_asin(a)) && same(num::acos(a), kern_musl_acos(a)) && same(num::atan(a), kern_musl_atan(a)), "trig (musl)", a);
      expect(same(num::exp(a), kern_musl_exp(a)) && same(num::expm1(a), kern_musl_expm1(a)) && same(num::log(a), kern_musl_log(a)) && same(num::log10(a), kern_musl_log10(a)), "exp log (musl)", a);
      expect(same(num::sinh(a), kern_math_sinh(a)) && same(num::cosh(a), kern_math_cosh(a)) && same(num::tanh(a), kern_math_tanh(a)) && same(num::asinh(a), kern_math_asinh(a)) && same(num::acosh(a), kern_math_acosh(a)) && same(num::atanh(a), kern_math_atanh(a)), "hyperbolic (the engine's)", a);
      expect(same(num::log2(a), kern_math_log2(a)) && same(num::log1p(a), kern_math_log1p(a)) && same(num::exp2(a), kern_math_exp2(a)) && same(num::cbrt(a), kern_math_cbrt(a)), "log2 log1p exp2 cbrt (the engine's)", a);
      for (int n : ldexp_edges()) expect(same(num::ldexp(a, n), kern_math_ldexp(a, n)) && same(num::ldexp(a, n), std::ldexp(a, n)), "ldexp (the engine's, exact: C++'s)", a, T(n));
    }
  }
  // Outside what a function is defined for it gives what C++'s gives - IEEE 754's NaN and infinities, never a 0 in their place.
  {
    const T inf = std::numeric_limits<T>::infinity(), nan = std::numeric_limits<T>::quiet_NaN();
    auto is = [&](T got, T want, const char* what) { expect(same(got, want), what, got, want); };
    is(num::log2(T(0)), -inf, "log2(0) is -inf"), is(num::log2(-T(0)), -inf, "log2(-0) is -inf"), is(num::log2(T(-1)), nan, "log2(-1) is NaN"), is(num::log2(-inf), nan, "log2(-inf) is NaN"), is(num::log2(inf), inf, "log2(inf)");
    is(num::log1p(T(-1)), -inf, "log1p(-1) is -inf"), is(num::log1p(T(-2)), nan, "log1p(-2) is NaN"), is(num::log1p(inf), inf, "log1p(inf)"), is(num::log1p(-inf), nan, "log1p(-inf) is NaN");
    is(num::acosh(T(0.5)), nan, "acosh(0.5) is NaN"), is(num::acosh(T(0)), nan, "acosh(0) is NaN"), is(num::acosh(-inf), nan, "acosh(-inf) is NaN"), is(num::acosh(T(1)), T(0), "acosh(1) is 0"), is(num::acosh(inf), inf, "acosh(inf)");
    is(num::atanh(T(1)), inf, "atanh(1) is inf"), is(num::atanh(T(-1)), -inf, "atanh(-1) is -inf"), is(num::atanh(T(2)), nan, "atanh(2) is NaN"), is(num::atanh(-inf), nan, "atanh(-inf) is NaN");
    is(num::exp2(-inf), T(0), "exp2(-inf) is 0"), is(num::exp2(inf), inf, "exp2(inf)"), is(num::exp2(T(100000)), inf, "exp2 overflows to inf"), is(num::cbrt(-inf), -inf, "cbrt(-inf)"), is(num::cbrt(T(-27)), T(-3), "cbrt(-27) is -3");
    is(num::remainder(T(1), T(0)), nan, "remainder(1, 0) is NaN"), is(num::remainder(inf, T(1)), nan, "remainder(inf, 1) is NaN"), is(num::remainder(T(5), inf), T(5), "remainder(5, inf) is 5"), is(num::remainder(T(5), T(2)), T(1), "remainder(5, 2)"), is(num::remainder(T(7), T(2)), T(-1), "remainder(7, 2) is -1");
    is(num::ldexp(T(1), std::numeric_limits<int>::max()), inf, "ldexp(1, INT_MAX) is inf"), is(num::ldexp(T(-1), std::numeric_limits<int>::min()), -T(0), "ldexp(-1, INT_MIN) is -0"), is(num::ldexp(nan, 3), nan, "ldexp(NaN)");
    is(num::sinh(inf), inf, "sinh(inf)"), is(num::cosh(-inf), inf, "cosh(-inf)"), is(num::tanh(-inf), T(-1), "tanh(-inf)"), is(num::asinh(-inf), -inf, "asinh(-inf)"), is(num::sinh(nan), nan, "sinh(NaN)");
  }
}

/// A conversion of every value of From to To: C++'s static_cast where C++ defines it, the fixed result where not.
template <class To, class From> void conversion() {
  std::vector<From> values;
  if constexpr (std::is_same_v<From, bool>) values = {false, true};
  else if constexpr (std::is_floating_point_v<From>) values = float_edges<From>();
  else values = edges<From>();
  for (From x : values) {
    const To got = num::cast<To>(x);
    if constexpr (std::is_floating_point_v<From> && std::is_integral_v<To> && !std::is_same_v<To, bool>) {
      using L = std::numeric_limits<To>;
      // In To's range after cutting toward zero: C++ defines it.
      const long double t = std::trunc(static_cast<long double>(x));
      if (std::isnan(x)) expect(got == 0, "NaN to whole", x);
      else if (t > static_cast<long double>(L::max())) expect(got == L::max(), "above the end", x);
      else if (t < static_cast<long double>(L::min())) expect(got == L::min(), "below the end", x);
      else expect(got == static_cast<To>(x), "toward zero", x);
    } else {
      expect(same(got, static_cast<To>(x)), "static_cast", static_cast<double>(x));
    }
    // A value's bits stay what they are.
    expect(num::of_bits<From>(num::bits_of(x)) == x || (std::is_floating_point_v<From> && x != x), "bits", static_cast<double>(x));
  }
}
template <class From, class... To> void conversions_from() { (conversion<To, From>(), ...); }
template <class... T> void every_conversion() { (conversions_from<T, bool, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64>(), ...); }


// ---------------------------------------------------------------------------
// The nodes of the numbers, in every tier: a Blueprint of one pure function per operation and type (add_f32,
// shl_u8, cast_f32_i64 ...), each the one node of that operation. The interpreter's answers are checked against
// numbers.h here (numbers.h against C++ above); the Blueprint's class gives the interpreter's answers, call for
// call (blueprint_native.h: the case "numbers").

/// An operation of the nodes: its function's name, the node, its family, how many inputs.
struct NodeOp {
  const char* name;
  const char* node;
  int family;   // 0 arithmetic, 1 whole, 2 floating, 3 compare (param op)
  int inputs;
  const char* op = nullptr;  // (math.compare: the operator)
};
inline const std::vector<NodeOp>& node_ops() {
  static const std::vector<NodeOp> ops = {
      {"add", "math.add", 0, 2}, {"sub", "math.sub", 0, 2}, {"mul", "math.mul", 0, 2}, {"div", "math.div", 0, 2}, {"rem", "math.rem", 0, 2},
      {"min", "math.min", 0, 2}, {"max", "math.max", 0, 2}, {"neg", "math.negate", 0, 1}, {"abs", "math.abs", 0, 1},
      {"eq", "math.compare", 3, 2, "=="}, {"ne", "math.compare", 3, 2, "!="}, {"lt", "math.compare", 3, 2, "<"}, {"le", "math.compare", 3, 2, "<="},
      {"gt", "math.compare", 3, 2, ">"}, {"ge", "math.compare", 3, 2, ">="},
      {"and", "math.and", 1, 2}, {"or", "math.or", 1, 2}, {"xor", "math.xor", 1, 2}, {"not", "math.not", 1, 1}, {"shl", "math.shift_left", 1, 2}, {"shr", "math.shift_right", 1, 2},
      {"sqrt", "math.sqrt", 2, 1}, {"floor", "math.floor", 2, 1}, {"ceil", "math.ceil", 2, 1}, {"trunc", "math.trunc", 2, 1}, {"round", "math.round", 2, 1},
      {"sin", "math.sin", 2, 1}, {"cos", "math.cos", 2, 1}, {"tan", "math.tan", 2, 1}, {"atan", "math.atan", 2, 1}, {"exp", "math.exp", 2, 1}, {"log", "math.log", 2, 1},
      {"pow", "math.pow", 2, 2}, {"atan2", "math.atan2", 2, 2}, {"hypot", "math.hypot", 2, 2}, {"copysign", "math.copysign", 2, 2}, {"fmin", "math.fmin", 2, 2},
      // the functions of <cmath> that are the engine's own (kern/src/math): each a node of f32 and of f64
      {"asin", "math.asin", 2, 1}, {"acos", "math.acos", 2, 1}, {"expm1", "math.expm1", 2, 1}, {"log10", "math.log10", 2, 1}, {"fmax", "math.fmax", 2, 2},
      {"sinh", "math.sinh", 2, 1}, {"cosh", "math.cosh", 2, 1}, {"tanh", "math.tanh", 2, 1}, {"asinh", "math.asinh", 2, 1}, {"acosh", "math.acosh", 2, 1}, {"atanh", "math.atanh", 2, 1},
      {"log2", "math.log2", 2, 1}, {"log1p", "math.log1p", 2, 1}, {"exp2", "math.exp2", 2, 1}, {"cbrt", "math.cbrt", 2, 1}, {"remainder", "math.remainder", 2, 2}, {"ldexp", "math.ldexp", 2, 2}};
  return ops;
}
inline const char* const kNumberTypes[] = {"i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "f32", "f64"};
inline bool op_takes(const NodeOp& op, const std::string& t) {
  const bool fl = t[0] == 'f';
  return op.family == 0 || op.family == 3 || (op.family == 1 && !fl) || (op.family == 2 && fl);
}
/// The input pins of a node of the operation (a, b; the shifts: a, by; atan2: y, x; copysign: a, b).
inline std::pair<const char*, const char*> op_pins(const NodeOp& op) {
  if (std::string(op.name) == "shl" || std::string(op.name) == "shr") return {"a", "by"};
  if (std::string(op.name) == "atan2") return {"y", "x"};
  if (std::string(op.name) == "ldexp") return {"a", "n"};
  return {"a", "b"};
}
/// The Blueprint: one pure function per operation and type, and per conversion (cast_<to>_<from>).
inline std::string numbers_graph_json() {
  std::string fns, nodes, links;
  u32 id = 1;
  auto function = [&](const std::string& name, const std::vector<std::pair<std::string, std::string>>& in, const std::string& out_type, const std::string& node, const std::string& params,
                      const std::vector<std::pair<std::string, std::string>>& wires) {
    fns += std::string(fns.empty() ? "" : ",") + "{\"name\":\"" + name + "\",\"pure\":true,\"inputs\":[";
    for (std::size_t k = 0; k < in.size(); ++k) fns += std::string(k ? "," : "") + "{\"name\":\"" + in[k].first + "\",\"type\":\"" + in[k].second + "\",\"value\":0}";
    fns += "],\"outputs\":[{\"name\":\"r\",\"type\":\"" + out_type + "\",\"value\":" + (out_type == "bool" ? "false" : "0") + "}]}";
    const u32 e = id++, n = id++, r = id++;
    nodes += std::string(nodes.empty() ? "" : ",") + "{\"id\":" + std::to_string(e) + ",\"type\":\"func.entry\",\"params\":{\"function\":\"" + name + "\"}}";
    nodes += ",{\"id\":" + std::to_string(n) + ",\"type\":\"" + node + "\",\"params\":{" + params + "}}";
    nodes += ",{\"id\":" + std::to_string(r) + ",\"type\":\"func.return\",\"params\":{\"function\":\"" + name + "\"}}";
    links += std::string(links.empty() ? "" : ",") + "[" + std::to_string(e) + ",\"then\"," + std::to_string(r) + ",\"exec\"]";
    for (const auto& [from, to] : wires) links += ",[" + std::to_string(e) + ",\"" + from + "\"," + std::to_string(n) + ",\"" + to + "\"]";
    links += ",[" + std::to_string(n) + ",\"result\"," + std::to_string(r) + ",\"r\"]";
  };
  for (const NodeOp& op : node_ops())
    for (const char* t : kNumberTypes) {
      if (!op_takes(op, t)) continue;
      const auto [pa, pb] = op_pins(op);
      const std::string by = std::string(pb) == "by" ? "i64" : std::string(pb) == "n" ? "i32" : t;
      std::vector<std::pair<std::string, std::string>> in{{"a", t}}, wires{{"a", pa}};
      if (op.inputs == 2) in.push_back({"b", by}), wires.push_back({"b", pb});
      const std::string params = std::string(op.op ? "\"op\":\"" + std::string(op.op) + "\"," : "") + "\"type\":\"" + t + "\"";
      function(std::string(op.name) + "_" + t, in, op.family == 3 ? "bool" : t, op.node, params, wires);
    }
  for (const char* to : kNumberTypes)
    for (const char* from : kNumberTypes) function(std::string("cast_") + to + "_" + from, {{"a", from}}, to, "num.cast", "\"to\":\"" + std::string(to) + "\",\"from\":\"" + from + "\"", {{"a", "a"}});
  for (const char* t : {"bool"})
    for (const char* to : kNumberTypes) {
      function(std::string("cast_") + to + "_" + t, {{"a", t}}, to, "num.cast", "\"to\":\"" + std::string(to) + "\",\"from\":\"bool\"", {{"a", "a"}});
      function(std::string("cast_bool_") + to, {{"a", to}}, "bool", "num.cast", "\"to\":\"bool\",\"from\":\"" + std::string(to) + "\"", {{"a", "a"}});
    }
  return "{\"functions\":[" + fns + "],\"nodes\":[" + nodes + "],\"links\":[" + links + "]}";
}
/// The edge values of a type as values (a few, so that every pair is a call): see edges<T>, float_edges<T>.
inline std::vector<logic::Value> node_edges(const std::string& t) {
  std::vector<logic::Value> r;
  logic::PinType pt{};
  logic::pin_type_from_name(t, pt);
  logic::with_number(pt, [&](auto tag) {
    using T = typename decltype(tag)::type;
    if constexpr (std::is_same_v<T, bool>) {
      r = {logic::Value::boolean(false), logic::Value::boolean(true)};
    } else if constexpr (std::is_floating_point_v<T>) {
      for (T x : {T(0), -T(0), T(1), T(-1.5), T(0.1), T(1) / T(3), std::numeric_limits<T>::max(), std::numeric_limits<T>::denorm_min(), std::numeric_limits<T>::infinity(),
                  -std::numeric_limits<T>::infinity(), std::numeric_limits<T>::quiet_NaN(), T(1e10), T(-3e9), T(255.9)})
        r.push_back(logic::Value::of_number(x));
    } else {
      using L = std::numeric_limits<T>;
      std::vector<T> v = {T(0), T(1), T(2), T(7), L::max(), L::min(), T(L::max() / 2 + 1), static_cast<T>(0xa5a5a5a5a5a5a5a5ull)};
      if constexpr (std::is_signed_v<T>) v.insert(v.end(), {T(-1), T(-7), T(L::min() + 1)});
      if constexpr (sizeof(T) == 8) v.push_back(static_cast<T>((u64{1} << 53) + 1));
      for (T x : v) r.push_back(logic::Value::of_number(x));
    }
  });
  return r;
}
/// numbers.h's answer for a function of the Blueprint (what the node must give).
inline logic::Value node_reference(const std::string& fn, const std::vector<logic::Value>& args) {
  using logic::Value;
  const std::size_t u = fn.find('_');
  const std::string op = fn.substr(0, u), rest = fn.substr(u + 1);
  if (op == "cast") {
    const std::size_t v = rest.find('_');
    logic::PinType to{};
    logic::pin_type_from_name(rest.substr(0, v), to);
    return logic::with_number(to, [&](auto t) { return Value::of_number(args[0].as<typename decltype(t)::type>()); });
  }
  logic::PinType pt{};
  logic::pin_type_from_name(rest, pt);
  return logic::with_number(pt, [&](auto tag) -> Value {
    using T = typename decltype(tag)::type;
    if constexpr (std::is_same_v<T, bool>) return Value();
    else {
      const T a = args[0].as<T>();
      const T b = args.size() > 1 ? args[1].as<T>() : T(0);
      const i64 by = args.size() > 1 ? args[1].as<i64>() : 0;
      if (op == "add") return Value::of_number(num::add(a, b));
      if (op == "sub") return Value::of_number(num::sub(a, b));
      if (op == "mul") return Value::of_number(num::mul(a, b));
      if (op == "div") return Value::of_number(num::div(a, b));
      if (op == "rem") return Value::of_number(num::rem(a, b));
      if (op == "min") return Value::of_number(num::min(a, b));
      if (op == "max") return Value::of_number(num::max(a, b));
      if (op == "neg") return Value::of_number(num::neg(a));
      if (op == "abs") return Value::of_number(num::abs(a));
      if (op == "eq") return Value::boolean(num::eq(a, b));
      if (op == "ne") return Value::boolean(num::ne(a, b));
      if (op == "lt") return Value::boolean(num::lt(a, b));
      if (op == "le") return Value::boolean(num::le(a, b));
      if (op == "gt") return Value::boolean(num::gt(a, b));
      if (op == "ge") return Value::boolean(num::ge(a, b));
      if constexpr (num::Integer<T>) {
        if (op == "and") return Value::of_number(num::bit_and(a, b));
        if (op == "or") return Value::of_number(num::bit_or(a, b));
        if (op == "xor") return Value::of_number(num::bit_xor(a, b));
        if (op == "not") return Value::of_number(num::bit_not(a));
        if (op == "shl") return Value::of_number(num::shl(a, by));
        if (op == "shr") return Value::of_number(num::shr(a, by));
      } else {
        if (op == "sqrt") return Value::of_number(num::sqrt(a));
        if (op == "floor") return Value::of_number(num::floor(a));
        if (op == "ceil") return Value::of_number(num::ceil(a));
        if (op == "trunc") return Value::of_number(num::trunc(a));
        if (op == "round") return Value::of_number(num::round(a));
        if (op == "sin") return Value::of_number(num::sin(a));
        if (op == "cos") return Value::of_number(num::cos(a));
        if (op == "tan") return Value::of_number(num::tan(a));
        if (op == "atan") return Value::of_number(num::atan(a));
        if (op == "exp") return Value::of_number(num::exp(a));
        if (op == "log") return Value::of_number(num::log(a));
        if (op == "pow") return Value::of_number(num::pow(a, b));
        if (op == "atan2") return Value::of_number(num::atan2(a, b));
        if (op == "hypot") return Value::of_number(num::hypot(a, b));
        if (op == "copysign") return Value::of_number(num::copysign(a, b));
        if (op == "fmin") return Value::of_number(num::fmin(a, b));
        if (op == "fmax") return Value::of_number(num::fmax(a, b));
        if (op == "asin") return Value::of_number(num::asin(a));
        if (op == "acos") return Value::of_number(num::acos(a));
        if (op == "expm1") return Value::of_number(num::expm1(a));
        if (op == "log10") return Value::of_number(num::log10(a));
        if (op == "sinh") return Value::of_number(num::sinh(a));
        if (op == "cosh") return Value::of_number(num::cosh(a));
        if (op == "tanh") return Value::of_number(num::tanh(a));
        if (op == "asinh") return Value::of_number(num::asinh(a));
        if (op == "acosh") return Value::of_number(num::acosh(a));
        if (op == "atanh") return Value::of_number(num::atanh(a));
        if (op == "log2") return Value::of_number(num::log2(a));
        if (op == "log1p") return Value::of_number(num::log1p(a));
        if (op == "exp2") return Value::of_number(num::exp2(a));
        if (op == "cbrt") return Value::of_number(num::cbrt(a));
        if (op == "remainder") return Value::of_number(num::remainder(a, b));
        if (op == "ldexp") return Value::of_number(num::ldexp(a, args[1].as<i32>()));
      }
      return Value();
    }
  });
}
/// Every call of the Blueprint: each function with every pair of edge values of its inputs' types (a shift's
/// amount: 0, 1, 7, the width, beyond it, -1).
inline std::vector<std::pair<std::string, std::vector<logic::Value>>> numbers_calls(const logic::Graph& g) {
  std::vector<std::pair<std::string, std::vector<logic::Value>>> r;
  for (const logic::Function& f : g.functions) {
    std::vector<std::vector<logic::Value>> each;
    for (const logic::FunctionPin& p : f.inputs) {
      if (p.name == "b" && (f.name.rfind("shl_", 0) == 0 || f.name.rfind("shr_", 0) == 0)) {
        std::vector<logic::Value> by;
        for (i64 n : {0, 1, 7, 8, 31, 32, 63, 64, 65, -1}) by.push_back(logic::Value::integer(n));
        each.push_back(by);
      } else if (p.name == "b" && f.name.rfind("ldexp_", 0) == 0) {
        std::vector<logic::Value> n;
        for (int k : ldexp_edges()) n.push_back(logic::Value::of_number(static_cast<i32>(k)));
        each.push_back(n);
      } else {
        each.push_back(node_edges(logic::pin_type_name(p.type)));
      }
    }
    if (each.size() == 1)
      for (const logic::Value& a : each[0]) r.push_back({f.name, {a}});
    else
      for (const logic::Value& a : each[0])
        for (const logic::Value& b : each[1]) r.push_back({f.name, {a, b}});
  }
  return r;
}
/// The same value: of the same type and with the same bits (two NaNs: both NaN - numbers.h).
inline bool same_value(const logic::Value& a, const logic::Value& b) {
  if (a.type != b.type) return false;
  if (logic::is_floating(a.type)) {
    const f64 x = a.as<f64>(), y = b.as<f64>();
    if (std::isnan(x) || std::isnan(y)) return std::isnan(x) && std::isnan(y);
  }
  return a.bits() == b.bits();
}

}  // namespace number_cases
