#pragma once
#include "kern/core/base.h"
namespace cases {
using namespace kern;
/// Seconds summed in f32 as a game sums frame.dt: after 60 frames of 1/60, C++ (f32) says not yet past 1 - the 61st is.
bool past_one_after(int frames);
/// A hash step in u32 that wraps (Knuth's multiplier).
u32 hash32(u32 h);
/// The product of two f32 kept in f32, then compared (the intermediate is f32 in C++).
bool product_is_exact(f32 a, f32 b);
/// A 64-bit LCG step (u64 wraps at 2^64).
u64 lcg(u64 s);
}
