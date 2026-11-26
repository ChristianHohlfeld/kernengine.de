#include "hyperbolic.h"

#include <cmath>
namespace cases {
double rise(double x) { return std::sinh(x) + std::cosh(x); }
double damped(double x) { return std::tanh(x * 0.5); }
double back(double x) { return std::asinh(x); }
// (an f32 one - std::tanh(float) - is not here: the language computes it in f64 and keeps the f64, so the second pass
// differs as every f32 intermediate does - numbers_f32, until the number types of the language)
}
