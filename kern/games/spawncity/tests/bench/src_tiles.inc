// The cases of src_tiles.cpp and src64_tiles.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
#include "src_helpers.inc"
bench::u64 SRC(tiles_key_hash)() {
  static const auto in = [] { bench::Gen g(8); std::vector<SC::TileKey> v; for (int i = 0; i < bench::kMany; ++i) v.push_back({14, g.below(1 << 14), g.below(1 << 14)}); return v; }();
  bench::u64 h = 0;
  for (const SC::TileKey& k : in) h = bench::fold(h, k.hash());
  return h;
}
