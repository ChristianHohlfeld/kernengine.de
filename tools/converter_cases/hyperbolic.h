#pragma once
namespace cases {
/// The hyperbolic functions of the standard library in a program: the engine's own (kern/src/math/hyperbolic.c).
double rise(double x);    // sinh + cosh
double damped(double x);  // tanh
double back(double x);    // asinh
}
