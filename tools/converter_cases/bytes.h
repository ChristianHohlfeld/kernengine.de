#pragma once
#include "kern/math/math.h"
#include "kern/core/base.h"
namespace cases {
struct Bytes { kern::u32 sum = 0; kern::u32 count = 0; };
Bytes& bytes();
/// The sum of the bytes a view shows.
kern::u32 sum_of(kern::Span<const kern::u8> b);
}
