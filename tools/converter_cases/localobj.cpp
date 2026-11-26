#include "localobj.h"
#include "kern/geo/earcut.h"
namespace cases {
kern::i64 triangles(const std::vector<kern::f64>& xy) {
  std::vector<kern::u32> hs, tris;
  if (xy.size() < 6) return 0;
  kern::Earcut ec;
  ec.triangulate(xy.data(), static_cast<kern::u32>(xy.size() / 2), hs.data(), static_cast<kern::u32>(hs.size()), tris);
  return static_cast<kern::i64>(tris.size() / 3);
}
}
