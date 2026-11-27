#include "clampc.h"
#include <algorithm>
namespace cases {
f32 unit(f32 a, f32 b) { return std::clamp(a / (b + a), 0.0f, 1.0f); }
f32 step(f32 dt) { return kern::clamp(dt, 0.0002f, 0.02f); }
i32 gear(i32 g) { return std::clamp(g, -1, 5); }
}
