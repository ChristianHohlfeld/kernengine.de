#pragma once
#include "kern/geo/mesh_data.h"
namespace cases {
using kern::f32;
using kern::u32;
/// The engine's MeshData::push_extra (`const f32* values`, a view to read) handed an array of a fixed length - every
/// call of the program hands one (`const f32 rec[3]`), the engine's library takes a list of any length.
u32 extra_of_the_shore(f32 shore);
}
