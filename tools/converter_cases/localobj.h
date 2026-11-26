#pragma once
#include <vector>
#include "kern/core/base.h"
namespace cases {
/// Triangles of a polygon, by the engine's Earcut made where it is needed (a local object of the engine).
kern::i64 triangles(const std::vector<kern::f64>& xy);
}
