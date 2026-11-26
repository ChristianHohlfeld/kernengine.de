#pragma once
#include "kern/math/math.h"
namespace cases {
struct PadState { bool connected = false; kern::f32 lx = 0; kern::u32 buttons = 0, pressed = 0; };
struct Holder { PadState pad; };
Holder& holder();
/// What the host hands on: written into the holder's pad - a step.
void take_pad(kern::i32 connected, kern::f32 lx, kern::u32 buttons);
/// Only computes.
kern::f32 doubled(kern::f32 x);
}
namespace cases {
/// A function of another state, Other, that reads the holder's pad: no function of Holder.
struct Other { kern::f32 seen = 0; void look(); };
Other& other();
}
