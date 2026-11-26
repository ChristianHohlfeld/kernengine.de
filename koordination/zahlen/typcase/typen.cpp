#include "typen.h"
namespace cases {
bool past_one_after(int frames) {
  f32 s = 0;
  for (int i = 0; i < frames; ++i) s += 1.0f / 60.0f;
  return s > 1.0f;
}
u32 hash32(u32 h) { return h * 2654435761u; }
bool product_is_exact(f32 a, f32 b) { const f32 p = a * b; return static_cast<f64>(p) == static_cast<f64>(a) * static_cast<f64>(b); }
u64 lcg(u64 s) { return s * 6364136223846793005ull + 1442695040888963407ull; }
}
