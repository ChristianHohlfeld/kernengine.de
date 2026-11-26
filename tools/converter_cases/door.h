#pragma once
#include "kern/math/math.h"
namespace cases {
/// How far a door of a world stands open: nothing of the program uses it - a world's Blueprint.
inline kern::f32 opened(kern::f32 at, kern::f32 speed) { kern::f32 to = at + speed; return to > 1.0f ? 1.0f : to; }
kern::f32 swung(kern::f32 at);
}
