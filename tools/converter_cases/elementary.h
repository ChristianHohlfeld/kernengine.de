#pragma once
namespace cases {
/// The functions of <cmath> a program calls beside sin and exp - each the engine's own (kern::dm: musl, kern/src/math),
/// of an f64 and of an f32 (its f-variant): log2 log1p exp2 cbrt acosh atanh ldexp remainder log10 expm1 hypot.
double logs(double x);         // log2, log1p, log10, expm1
float logs32(float x);
double roots(double x);        // cbrt, exp2, hypot
float roots32(float x);
double inverse(double x);      // acosh, atanh
float inverse32(float x);
double scaled(double x, int n);   // ldexp, remainder
float scaled32(float x, int n);
// Outside what they are defined for they give what C++ gives (IEEE 754: -inf, NaN) - never a 0 in its place.
double none(double x);         // log2 of 0: -inf
float none32(float x);
double beyond(double x);       // atanh of 1 and more: inf, NaN; acosh below 1: NaN; log1p of -1 and less: -inf, NaN
float beyond32(float x);
}
