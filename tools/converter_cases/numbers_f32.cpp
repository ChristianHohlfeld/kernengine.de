#include "numbers_f32.h"
namespace cases {
bool product_is_exact(f32 a, f32 b) { const f32 p = a * b; return static_cast<f64>(p) == static_cast<f64>(a) * static_cast<f64>(b); }
}
