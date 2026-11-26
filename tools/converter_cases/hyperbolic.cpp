#include "hyperbolic.h"

#include <cmath>
namespace cases {
double rise(double x) { return std::sinh(x) + std::cosh(x); }
double damped(double x) { return std::tanh(x * 0.5); }
double back(double x) { return std::asinh(x); }
// (tanhf is tanh in f64, rounded once to f32: the Blueprint's f64 tanh, kept in the f32, is the same in both passes)
float soft(float x) { return std::tanh(x); }
}
