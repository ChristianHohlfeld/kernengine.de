#pragma once
#include "kern/math/math.h"
namespace cases {
using kern::f32; using kern::Vec2;
f32 dist(Vec2 a, Vec2 b);
f32 turn(f32 x);
f32 yaw(f32 a, f32 b);
}
