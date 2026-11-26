#include "elementary.h"

#include <cmath>
namespace cases {
double logs(double x) { return std::log2(x) + std::log1p(x) + std::log10(x) - std::expm1(x * 0.25); }
float logs32(float x) { return std::log2(x) + std::log1p(x) + std::log10(x) - std::expm1(x * 0.25f); }
double roots(double x) { return std::cbrt(x) + std::exp2(x * 0.5) + std::hypot(x, 3.0) + std::log2(8.0) * std::cbrt(27.0); }
float roots32(float x) { return std::cbrt(x) + std::exp2(x * 0.5f) + std::hypot(x, 3.0f) + std::log2(8.0f) * std::cbrt(27.0f); }
double inverse(double x) { return std::acosh(x + 1.5) + std::atanh(x * 0.25); }
float inverse32(float x) { return std::acosh(x + 1.5f) + std::atanh(x * 0.25f); }
double scaled(double x, int n) { return std::ldexp(x, n % 16) + std::remainder(x, 0.75); }
float scaled32(float x, int n) { return std::ldexp(x, n % 16) + std::remainder(x, 0.75f); }
double none(double x) { return std::log2(x - x); }
float none32(float x) { return std::log2(x - x); }
double beyond(double x) {
  const double a = std::fabs(x);
  if (a > 2.0) return std::atanh(a);
  if (a > 1.0) return std::acosh(2.0 - a) + std::log1p(-a);
  if (a > 0.5) return std::log1p(a - a - 1.0) + a;
  return a > 0.25 ? std::atanh(a - a + 1.0) - a : std::remainder(a, x - x);
}
float beyond32(float x) {
  const float a = std::fabs(x);
  if (a > 2.0f) return std::atanh(a);
  if (a > 1.0f) return std::acosh(2.0f - a) + std::log1p(-a);
  if (a > 0.5f) return std::log1p(a - a - 1.0f) + a;
  return a > 0.25f ? std::atanh(a - a + 1.0f) - a : std::remainder(a, x - x);
}
}
