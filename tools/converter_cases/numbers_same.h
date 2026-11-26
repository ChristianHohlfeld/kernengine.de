#pragma once
#include "kern/core/base.h"
namespace cases {
using namespace kern;
/// A hash step in u32 that wraps at 2^32 (Knuth's multiplier).
u32 hash32(u32 h);
/// A 64-bit LCG step (u64 wraps at 2^64).
u64 lcg(u64 s);
}
