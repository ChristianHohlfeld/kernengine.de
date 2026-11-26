#pragma once
#include <vector>
#include "kern/geo/mesh_data.h"
namespace cases {
using kern::f32;
using kern::u32;
/// A read-only view (`kern::Span<const f32>` of the engine's MeshData::push_extra and of the program's
/// own sum_of) handed an array of a fixed length, a list, or a pointer into a list.
f32 sum_of(kern::Span<const f32> values);
f32 first_of(const f32* values);
u32 extra_from_an_array(f32 shore);
u32 extra_from_a_list(f32 shore);
f32 sums();
/// A view to write through (kern::Span<f32>): the place, a reference input.
void fill(kern::Span<f32> out, f32 v);
f32 filled();
}
