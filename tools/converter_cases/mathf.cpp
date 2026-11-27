#include "mathf.h"
#include <cmath>
namespace cases {
f32 dist(Vec2 a, Vec2 b) { return std::hypot(a.x - b.x, a.y - b.y); }
f32 turn(f32 x) { return std::cos(x) + std::sin(x); }
f32 yaw(f32 a, f32 b) { return std::atan2(a, b) * 57.29578f; }
}
