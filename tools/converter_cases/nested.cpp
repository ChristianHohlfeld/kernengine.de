#include "nested.h"
namespace cases {
namespace mvt {
kern::f32 first_height(const Stream& s) { return static_cast<kern::f32>(s.tiles.size()) * 0.5f; }
}
kern::i32 tile_count(const Stream& s) { return static_cast<kern::i32>(s.tiles.size()); }
}
