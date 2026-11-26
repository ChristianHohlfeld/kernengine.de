#pragma once
namespace cases {
/// The hyperbolic functions of the standard library in a program: the engine's own (kern/src/math/hyperbolic.c).
double rise(double x);    // sinh + cosh
double damped(double x);  // tanh
double back(double x);    // asinh
float soft(float x);      // tanh of an f32
float rise32(float x);    // sinh + cosh of an f32
float back32(float x);    // asinh of an f32
}
