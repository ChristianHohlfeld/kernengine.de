#pragma once
#include "kern/core/base.h"
namespace cases {
using namespace kern;
/// The product of two f32 kept in f32, then compared with the product in f64 (in C++ the intermediate is an f32).
bool product_is_exact(f32 a, f32 b);
}
