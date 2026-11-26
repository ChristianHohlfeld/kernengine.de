// The numbers of a Blueprint against C++ (kern/logic/numbers.h): every operation of every number type gives what
// C++ gives - with the edge values (going round of the unsigned types, signs, shifts at the width, division and
// remainder of negative numbers, NaN, inf, -0, denormals, f32 intermediate values, 64 bits beyond 2^53) - and where
// C++ leaves the result open, the one result numbers.h fixes. Included by test_main.cpp (TEST, CHECK).
#pragma once

#include <bit>
#include <cmath>
#include <limits>
#include <vector>

#include "kern/logic/numbers.h"

namespace number_cases {
using namespace kern;
namespace num = kern::logic::num;

/// The same value: the same bits - of two NaNs, both NaN (neither C++ nor WebAssembly fix a NaN's bits).
template <class T> bool same(T a, T b) {
  if constexpr (std::is_floating_point_v<T>) return (std::isnan(a) && std::isnan(b)) || std::bit_cast<std::conditional_t<sizeof(T) == 4, u32, u64>>(a) == std::bit_cast<std::conditional_t<sizeof(T) == 4, u32, u64>>(b);
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
      expect(same(num::fmin(a, b), std::fmin(a, b)) && same(num::fmax(a, b), std::fmax(a, b)), "fmin fmax", a, b);
      expect(same(num::copysign(a, b), std::copysign(a, b)), "copysign", a, b);
      expect(same(num::atan2(a, b), static_cast<T>(dm::atan2(a, b))) && same(num::pow(a, b), static_cast<T>(dm::pow(a, b))) && same(num::hypot(a, b), static_cast<T>(dm::hypot(a, b))), "atan2 pow hypot", a, b);
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
    } else {
      expect(same(num::sin(a), kern_musl_sin(a)) && same(num::cos(a), kern_musl_cos(a)) && same(num::tan(a), kern_musl_tan(a)) && same(num::asin(a), kern_musl_asin(a)) && same(num::acos(a), kern_musl_acos(a)) && same(num::atan(a), kern_musl_atan(a)), "trig (musl)", a);
      expect(same(num::exp(a), kern_musl_exp(a)) && same(num::expm1(a), kern_musl_expm1(a)) && same(num::log(a), kern_musl_log(a)) && same(num::log10(a), kern_musl_log10(a)), "exp log (musl)", a);
    }
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

}  // namespace number_cases

TEST(numbers_against_cpp) {
  using namespace number_cases;
  namespace num = kern::logic::num;
  failures = 0;
  whole_numbers<i8>(), whole_numbers<i16>(), whole_numbers<i32>(), whole_numbers<i64>();
  whole_numbers<u8>(), whole_numbers<u16>(), whole_numbers<u32>(), whole_numbers<u64>();
  floating_numbers<f32>(), floating_numbers<f64>();
  every_conversion<bool, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64>();
  CHECK(failures == 0);

  // What C++ says, written out: toward zero, the remainder has the sign of the dividend.
  CHECK(num::div<i32>(-7, 2) == -3 && num::rem<i32>(-7, 2) == -1 && num::div<i32>(7, -2) == -3 && num::rem<i32>(7, -2) == 1);
  CHECK(num::div<i64>(std::numeric_limits<i64>::min(), -1) == std::numeric_limits<i64>::min() && num::div<i32>(5, 0) == 0 && num::rem<u8>(5, 0) == 0);
  // The unsigned types go round, the signed ones (UB in C++) as fixed: in two's complement.
  CHECK(num::add<u32>(0xffffffffu, 1) == 0 && num::sub<u8>(0, 1) == 255 && num::mul<u16>(65535, 65535) == 1 && num::mul<u32>(0x9e3779b9u, 0x85ebca6bu) == static_cast<u32>(0x9e3779b9u * 0x85ebca6bu));
  CHECK(num::add<i32>(std::numeric_limits<i32>::max(), 1) == std::numeric_limits<i32>::min() && num::neg<i8>(-128) == -128 && num::abs<i64>(std::numeric_limits<i64>::min()) == std::numeric_limits<i64>::min());
  // Shifts at the width: the amount mod the width; >> of a signed number brings the sign in.
  CHECK(num::shl<u32>(1u, 32) == 1u && num::shl<u32>(1u, 31) == 0x80000000u && num::shr<i32>(-8, 1) == -4 && num::shr<u32>(0x80000000u, 31) == 1u && num::shl<u8>(0x81, 1) == 0x02);
  // 64 bits beyond 2^53: exact (FNV-1a over one byte; the mix of the converter's case bigint).
  CHECK(num::add<i64>((i64{1} << 53), 1) == (i64{1} << 53) + 1 && num::mul<u64>(0xcbf29ce484222325ull ^ 97u, 0x100000001b3ull) == static_cast<u64>((0xcbf29ce484222325ull ^ 97u) * 0x100000001b3ull));
  CHECK(num::add<i64>(num::mul<i64>(num::bit_xor<i64>(1469598103934665603LL, 97), 31), 7) == static_cast<i64>(static_cast<u64>(1469598103934665603LL ^ 97) * 31u + 7u));
  // Floating numbers to whole numbers beyond their ends (UB in C++): the nearer end, NaN 0.
  CHECK(num::cast<i32>(1e10f) == std::numeric_limits<i32>::max() && num::cast<i32>(-1e10f) == std::numeric_limits<i32>::min() && num::cast<u32>(-1.5) == 0u && num::cast<u32>(-0.5) == 0u);
  CHECK(num::cast<i64>(std::numeric_limits<f64>::quiet_NaN()) == 0 && num::cast<u64>(std::numeric_limits<f32>::infinity()) == std::numeric_limits<u64>::max() && num::cast<i64>(9223372036854775808.0) == std::numeric_limits<i64>::max());
  CHECK(num::cast<i8>(127.9f) == 127 && num::cast<i8>(-128.9) == -128 && num::cast<i8>(-129.5) == -128 && num::cast<u8>(255.9f) == 255 && num::cast<u32>(4294967295.5) == 4294967295u);
  // Whole numbers to narrower ones: the low bits; to floating ones: the nearest.
  CHECK(num::cast<u8>(i32{-1}) == 255 && num::cast<i8>(u32{200}) == -56 && num::cast<i64>(std::numeric_limits<u64>::max()) == -1 && num::cast<f32>(i32{16777217}) == 16777216.0f && num::cast<f64>((i64{1} << 53) + 1) == 9007199254740992.0);
  // f64 to f32: the nearest, beyond f32: inf; f32 keeps its denormals; -0 stays -0.
  CHECK(num::cast<f32>(1e300) == std::numeric_limits<f32>::infinity() && num::cast<f32>(0.1) == 0.1f && num::mul(std::numeric_limits<f32>::denorm_min(), 2.0f) == 2 * std::numeric_limits<f32>::denorm_min() && std::signbit(num::mul(-1.0f, 0.0f)));
  CHECK(num::div(1.0f, -0.0f) == -std::numeric_limits<f32>::infinity() && std::isnan(num::div(0.0, 0.0)) && std::isnan(num::sqrt(-1.0f)) && std::signbit(num::sqrt(-0.0f)) && std::isnan(num::rem(1.0f, 0.0f)));
  // f32 is not f64 rounded at the end: 0.1f * 0.1f is a product of its own, and so is a*b + c.
  {
    const f32 a = 0.1f;
    CHECK(num::mul(a, a) == a * a && static_cast<f64>(num::mul(a, a)) != static_cast<f64>(a) * static_cast<f64>(a));
    int differs = 0;
    for (f32 x : float_edges<f32>())
      for (f32 y : float_edges<f32>())
        for (f32 c : {0.3f, -0.01f, 1.0f})
          if (std::isfinite(x * y + c) && !same(num::add(num::mul(x, y), c), static_cast<f32>(static_cast<f64>(x) * y + c))) ++differs;
    CHECK(differs > 0);  // (so the comparison above would have seen a tier that computes in double)
  }
  // The bits of a value stay its bits - a NaN's too (bits_of / of_bits; a Value holds them so).
  CHECK(num::bits_of(std::bit_cast<f32>(0x7f800001u)) == 0x7f800001u && num::bits_of(std::bit_cast<f64>(0xfff8000000000123ull)) == 0xfff8000000000123ull && num::bits_of(i8{-1}) == ~u64{0} && num::bits_of(u8{255}) == 255u);
  CHECK(num::held_bits(num::Kind::u8, 0x1ff) == 0xff && num::held_bits(num::Kind::i8, 0x80) == ~u64{0x7f} && num::held_bits(num::Kind::f32, 0xffffffff3f800000ull) == 0x3f800000u);
  // The names of the types are the C++ names.
  num::Kind k{};
  CHECK(num::kind_from_name("u32", k) && k == num::Kind::u32 && num::kind_name(num::Kind::f32) == "f32" && !num::kind_from_name("float", k) && !num::kind_from_name("int", k));
  CHECK(num::cast_kind(num::Kind::f32, num::Kind::f64) == num::Cast::floating && num::cast_name(num::cast_kind(num::Kind::i32, num::Kind::f32)) == "IntegralToFloating" && num::is_promotion(num::Kind::u8, num::Kind::i32));
}
