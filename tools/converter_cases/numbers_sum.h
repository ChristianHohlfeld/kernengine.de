#pragma once
#include "kern/core/base.h"
namespace cases {
using namespace kern;
/// Seconds summed in f32 as a game sums frame.dt: after 60 frames of 1/60, C++ (f32) says not
/// yet past 1 - the 61st is.
bool past_one_after(int frames);
}
