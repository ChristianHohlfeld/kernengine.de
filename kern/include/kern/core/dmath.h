// Deterministic transcendental functions: the same bits on every platform Kern runs on (x86 and ARM
// natively, WebAssembly in every browser), so a simulation, a training run or a replayed robot log
// comes out identical everywhere (docs/lehrer.md, determinism).
//
// The platform's own libm (glibc, Apple's, Android's bionic, wasi-libc) rounds sin, exp, atan2 ...
// differently in the last bit; a robot simulation amplifies one bit into a different walk within
// seconds. These are musl's implementations (MIT, kern/third_party/musl_math), compiled the same way
// everywhere: no FMA contraction, no platform-specific paths. sqrt, fabs and floor are exact in IEEE
// arithmetic and stay std::.
//
// Use kern::dm:: instead of std:: for sin, cos, tan, asin, acos, atan, atan2, exp, expm1, log,
// log10, pow, hypot, sinh, cosh, tanh, asinh, acosh, atanh, log2, log1p, exp2, cbrt, ldexp and remainder in everything
// that feeds the simulation. (sinh ... atanh are the engine's own, kern/src/math/hyperbolic.c; log2, log1p, exp2, cbrt,
// ldexp, remainder kern/src/math/elementary.c - musl has them not here. ldexp and remainder are exact.)
#pragma once

#include <type_traits>

extern "C" {
double kern_musl_sin(double); double kern_musl_cos(double); double kern_musl_tan(double);
double kern_musl_asin(double); double kern_musl_acos(double); double kern_musl_atan(double);
double kern_musl_atan2(double, double); double kern_musl_exp(double); double kern_musl_expm1(double);
double kern_musl_log(double); double kern_musl_log10(double); double kern_musl_pow(double, double);
double kern_musl_hypot(double, double);
float kern_musl_sinf(float); float kern_musl_cosf(float); float kern_musl_tanf(float);
float kern_musl_asinf(float); float kern_musl_acosf(float); float kern_musl_atanf(float);
float kern_musl_atan2f(float, float); float kern_musl_expf(float); float kern_musl_expm1f(float);
float kern_musl_logf(float); float kern_musl_log10f(float); float kern_musl_powf(float, float);
float kern_musl_hypotf(float, float);
// The engine's own (kern/src/math/hyperbolic.c, not musl - musl_math has none): from the definitions, over musl's exp,
// expm1 and log and sqrt.
double kern_math_sinh(double); double kern_math_cosh(double); double kern_math_tanh(double); double kern_math_asinh(double);
float kern_math_sinhf(float); float kern_math_coshf(float); float kern_math_tanhf(float); float kern_math_asinhf(float);
double kern_math_acosh(double); double kern_math_atanh(double); float kern_math_acoshf(float); float kern_math_atanhf(float);
// (kern/src/math/elementary.c)
double kern_math_log2(double); double kern_math_log1p(double); double kern_math_exp2(double); double kern_math_cbrt(double);
float kern_math_log2f(float); float kern_math_log1pf(float); float kern_math_exp2f(float); float kern_math_cbrtf(float);
double kern_math_ldexp(double, int); float kern_math_ldexpf(float, int);
double kern_math_remainder(double, double); float kern_math_remainderf(float, float);
}

namespace kern::dm {

#define KERN_DM_UNARY(name)                                                         \
  inline float name(float x) { return kern_musl_##name##f(x); }                     \
  inline double name(double x) { return kern_musl_##name(x); }                      \
  template <class T, class = std::enable_if_t<std::is_integral_v<T>>>               \
  inline double name(T x) { return kern_musl_##name(static_cast<double>(x)); }
KERN_DM_UNARY(sin)
KERN_DM_UNARY(cos)
KERN_DM_UNARY(tan)
KERN_DM_UNARY(asin)
KERN_DM_UNARY(acos)
KERN_DM_UNARY(atan)
KERN_DM_UNARY(exp)
KERN_DM_UNARY(expm1)
KERN_DM_UNARY(log)
KERN_DM_UNARY(log10)
#undef KERN_DM_UNARY
#define KERN_DM_OWN(name)                                                           \
  inline float name(float x) { return kern_math_##name##f(x); }                     \
  inline double name(double x) { return kern_math_##name(x); }                      \
  template <class T, class = std::enable_if_t<std::is_integral_v<T>>>               \
  inline double name(T x) { return kern_math_##name(static_cast<double>(x)); }
KERN_DM_OWN(sinh)
KERN_DM_OWN(cosh)
KERN_DM_OWN(tanh)
KERN_DM_OWN(asinh)
KERN_DM_OWN(acosh)
KERN_DM_OWN(atanh)
KERN_DM_OWN(log2)
KERN_DM_OWN(log1p)
KERN_DM_OWN(exp2)
KERN_DM_OWN(cbrt)
#undef KERN_DM_OWN
// x * 2^n, exact (as std::ldexp: a whole number for n; a whole-number x is a double).
inline float ldexp(float x, int n) { return kern_math_ldexpf(x, n); }
inline double ldexp(double x, int n) { return kern_math_ldexp(x, n); }
template <class T, class = std::enable_if_t<std::is_integral_v<T>>>
inline double ldexp(T x, int n) { return kern_math_ldexp(static_cast<double>(x), n); }

// Two arguments promote as std:: does: both float -> float, anything else -> double.
#define KERN_DM_BINARY(name)                                                                          \
  inline float name(float a, float b) { return kern_musl_##name##f(a, b); }                           \
  template <class A, class B, class = std::enable_if_t<std::is_arithmetic_v<A> && std::is_arithmetic_v<B> && \
                                                       !(std::is_same_v<A, float> && std::is_same_v<B, float>)>> \
  inline double name(A a, B b) { return kern_musl_##name(static_cast<double>(a), static_cast<double>(b)); }
KERN_DM_BINARY(atan2)
KERN_DM_BINARY(pow)
KERN_DM_BINARY(hypot)
#undef KERN_DM_BINARY
// IEEE 754's remainder, exact (kern/src/math/elementary.c); promoted as std::remainder.
inline float remainder(float a, float b) { return kern_math_remainderf(a, b); }
template <class A, class B, class = std::enable_if_t<std::is_arithmetic_v<A> && std::is_arithmetic_v<B> &&
                                                     !(std::is_same_v<A, float> && std::is_same_v<B, float>)>>
inline double remainder(A a, B b) { return kern_math_remainder(static_cast<double>(a), static_cast<double>(b)); }

}  // namespace kern::dm
