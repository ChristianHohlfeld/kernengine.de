// The standard library's mathematics is the engine's (kern/third_party/musl_math/src/std_names.c, kern/src/math/hyperbolic.c,
// flags.sh): std::sin, sinf, atan2, sinh ... give the bits kern::dm gives - for f32 and f64, over edge values and a series of drawn ones, called
// directly, as a compiler may combine them (sin and cos of one value: sincos) and with a constant the compiler knows (it
// must not compute it itself). Shared by the native tests (test_main.cpp) and the WebAssembly module
// (math_names_wasm_test.cpp): both say how many differ and a digest of all the results - the same on every target
// (math_names_digest.txt).
#pragma once
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

#include "kern/core/dmath.h"

namespace math_names {

inline std::uint64_t bits(double x) { std::uint64_t b; std::memcpy(&b, &x, 8); return b; }
inline std::uint64_t bits(float x) { std::uint32_t b; std::memcpy(&b, &x, 4); return b; }

struct Result {
  int compared = 0, differ = 0;
  std::uint64_t digest = 1469598103934665603ull;
  std::string first;   // (the first that differs)
  template <class T> void take(const char* fn, T x, T y, T std_value, T engine_value) {
    ++compared;
    digest = (digest ^ bits(engine_value)) * 1099511628211ull;
    if (bits(std_value) == bits(engine_value)) return;
    if (++differ == 1) {
      char b[300];
      const int hex = sizeof(T) * 2;
      std::snprintf(b, sizeof b, "%s(%.17g, %.17g): std %.17g (bits %0*llx), the engine %.17g (bits %0*llx)", fn, static_cast<double>(x),
                    static_cast<double>(y), static_cast<double>(std_value), hex, static_cast<unsigned long long>(bits(std_value)),
                    static_cast<double>(engine_value), hex, static_cast<unsigned long long>(bits(engine_value)));
      first = b;
    }
  }
};

/// An opaque value: the compiler does not know it (volatile), so the call happens at run time.
template <class T> T opaque(T x) { volatile T v = x; return v; }

template <class T> std::vector<T> inputs() {
  const T inf = std::numeric_limits<T>::infinity(), nan = std::numeric_limits<T>::quiet_NaN();
  std::vector<T> v = {T(0), -T(0), T(0.5), T(-0.5), T(1), T(-1), T(2), T(3.14159265358979), T(-3.14159265358979), T(1e-30), T(-1e-30),
                      T(1e-7), T(10), T(88.5), T(-88.5), T(700), T(1e10), T(-1e10), T(1e30), inf, -inf, nan,
                      std::numeric_limits<T>::min(), std::numeric_limits<T>::denorm_min(), std::numeric_limits<T>::max()};
  std::uint64_t s = 0x9e3779b97f4a7c15ull;   // (a series of drawn values: magnitudes from 1e-6 to 1e6, both signs)
  for (int i = 0; i < 2000; ++i) {
    s = s * 6364136223846793005ull + 1442695040888963407ull;
    const double u = static_cast<double>(s >> 11) / 9007199254740992.0;
    const double m = std::ldexp(1.0, static_cast<int>(s % 41) - 20);
    v.push_back(static_cast<T>((u * 2 - 1) * m));
  }
  return v;
}

template <class T> void compare_all(Result& r) {
  const std::vector<T> in = inputs<T>();
  for (std::size_t k = 0; k < in.size(); ++k) {
    const T x = opaque(in[k]), y = opaque(in[(k * 7 + 3) % in.size()]);
    r.take("sin", x, x, std::sin(x), kern::dm::sin(x));
    r.take("cos", x, x, std::cos(x), kern::dm::cos(x));
    r.take("tan", x, x, std::tan(x), kern::dm::tan(x));
    r.take("asin", x, x, std::asin(x), kern::dm::asin(x));
    r.take("acos", x, x, std::acos(x), kern::dm::acos(x));
    r.take("atan", x, x, std::atan(x), kern::dm::atan(x));
    r.take("exp", x, x, std::exp(x), kern::dm::exp(x));
    r.take("expm1", x, x, std::expm1(x), kern::dm::expm1(x));
    r.take("log", x, x, std::log(x), kern::dm::log(x));
    r.take("log10", x, x, std::log10(x), kern::dm::log10(x));
    r.take("atan2", x, y, std::atan2(x, y), kern::dm::atan2(x, y));
    r.take("pow", x, y, std::pow(x, y), kern::dm::pow(x, y));
    r.take("pow2", x, T(2), std::pow(x, T(2)), kern::dm::pow(x, T(2)));   // (GCC: -fdisable-tree-powcabs, flags.sh)
    r.take("hypot", x, y, std::hypot(x, y), kern::dm::hypot(x, y));
    // (the engine's own: kern/src/math/hyperbolic.c, under the standard names as musl's)
    r.take("sinh", x, x, std::sinh(x), kern::dm::sinh(x));
    r.take("cosh", x, x, std::cosh(x), kern::dm::cosh(x));
    r.take("tanh", x, x, std::tanh(x), kern::dm::tanh(x));
    r.take("asinh", x, x, std::asinh(x), kern::dm::asinh(x));
    // (sin and cos of one value side by side: what a compiler may make one call of, sincos)
    const T s = std::sin(x), c = std::cos(x);
    r.take("sin beside cos", x, x, s, kern::dm::sin(x));
    r.take("cos beside sin", x, x, c, kern::dm::cos(x));
  }
}

/// Constants the compiler knows: what it gives at compile time must be what the engine gives at run time.
inline void compare_constants(Result& r) {
  r.take("sin(0.5f) constant", 0.5f, 0.5f, std::sin(0.5f), kern::dm::sin(opaque(0.5f)));
  r.take("cos(1.25f) constant", 1.25f, 1.25f, std::cos(1.25f), kern::dm::cos(opaque(1.25f)));
  r.take("atan2(1.5f, -2.f) constant", 1.5f, -2.f, std::atan2(1.5f, -2.f), kern::dm::atan2(opaque(1.5f), opaque(-2.f)));
  r.take("exp(-3.75f) constant", -3.75f, -3.75f, std::exp(-3.75f), kern::dm::exp(opaque(-3.75f)));
  r.take("pow(1.1f, 7.f) constant", 1.1f, 7.f, std::pow(1.1f, 7.f), kern::dm::pow(opaque(1.1f), opaque(7.f)));
  r.take("sin(0.5) constant", 0.5, 0.5, std::sin(0.5), kern::dm::sin(opaque(0.5)));
  r.take("cos(1e5) constant", 1e5, 1e5, std::cos(1e5), kern::dm::cos(opaque(1e5)));
  r.take("log10(7.0) constant", 7.0, 7.0, std::log10(7.0), kern::dm::log10(opaque(7.0)));
  r.take("pow(1.0001, 12345.0) constant", 1.0001, 12345.0, std::pow(1.0001, 12345.0), kern::dm::pow(opaque(1.0001), opaque(12345.0)));
  r.take("sinh(0.75f) constant", 0.75f, 0.75f, std::sinh(0.75f), kern::dm::sinh(opaque(0.75f)));
  r.take("tanh(-1.5f) constant", -1.5f, -1.5f, std::tanh(-1.5f), kern::dm::tanh(opaque(-1.5f)));
  r.take("cosh(2.5) constant", 2.5, 2.5, std::cosh(2.5), kern::dm::cosh(opaque(2.5)));
  r.take("asinh(3.0) constant", 3.0, 3.0, std::asinh(3.0), kern::dm::asinh(opaque(3.0)));
  r.take("hypot(3e200, 4e200) constant", 3e200, 4e200, std::hypot(3e200, 4e200), kern::dm::hypot(opaque(3e200), opaque(4e200)));
}

inline Result run() {
  Result r;
  compare_all<float>(r);
  compare_all<double>(r);
  compare_constants(r);
  return r;
}

}  // namespace math_names
