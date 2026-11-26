#pragma once
#include <memory>
#include "kern/core/map.h"
#include "kern/math/math.h"
namespace cases {
namespace mvt {
/// A Tile of its own namespace (a tile of a vector map).
struct Tile { kern::i32 layer = 0; };
}
/// A stream with a Tile of its own, declared before its field and defined after it: the field names it by the
/// short name, as C++ looks it up inside the struct.
struct Stream {
  struct Tile;
  kern::Map<kern::u64, std::unique_ptr<Tile>> tiles;
  struct Tile { kern::f32 height = 0; bool ready = false; };
};
namespace mvt {
/// Named first from inside mvt: Stream's field is still Stream::Tile, not mvt::Tile.
kern::f32 first_height(const Stream& s);
}
kern::i32 tile_count(const Stream& s);
}
