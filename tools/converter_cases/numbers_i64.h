#pragma once
namespace cases {
/// A step on a 64-bit whole number (long long) past 2^53, where a double is not exact any more - no overflow (C++ leaves
/// that open), only ^, / and +.
long long mix(long long h, int c);
/// The same from a start past 2^53 (2^60 + 1, as a hash starts from a large basis).
long long mixed_from_the_start();
}
