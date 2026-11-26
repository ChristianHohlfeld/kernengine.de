#include "hyperbolic.h"

#include <cmath>
namespace cases {
double rise(double x) { return std::sinh(x) + std::cosh(x); }
double damped(double x) { return std::tanh(x * 0.5); }
double back(double x) { return std::asinh(x); }
// (of an f32: the f-variants - tanhf, sinhf, coshf, asinhf -, each step in f32)
float soft(float x) { return std::tanh(x); }
float rise32(float x) { return std::sinh(x) + std::cosh(x * 0.5f); }
float back32(float x) { return std::asinh(x) * 2.0f; }
}
