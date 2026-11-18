// The source side of the measurement (bench.h): mvt.cpp, as the game builds it.
#include "../../src/mvt.cpp"
#include "bench.h"
namespace m = sc::mvt;
bench::u64 src_mvt_zigzag() {
  static const auto in = [] { bench::Gen g(5); std::vector<sc::u64> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(g.next()); return v; }();
  bench::u64 h = 0;
  for (sc::u64 x : in) h = bench::fold(h, static_cast<bench::u64>(m::zigzag(x)));
  return h;
}
bench::u64 src_mvt_read_geometry() {
  static const std::vector<sc::u8> g = bench::geometry(200);
  std::vector<std::vector<sc::Vec2>> parts;
  m::read_geometry(kern::Span<const sc::u8>(g.data(), g.size()), m::Reader{0, g.size()}, m::GeomType::line, parts);
  bench::u64 h = bench::fold(0, parts.size());
  for (const auto& p : parts)
    for (const sc::Vec2& v : p) h = bench::fold_number(bench::fold_number(h, v.x), v.y);
  return h;
}
bench::u64 src_mvt_ring_area() {
  static const auto in = [] { bench::Gen g(6); std::vector<sc::Vec2> v; for (int i = 0; i < bench::kMany; ++i) v.push_back({static_cast<sc::f32>(g.below(4096)), static_cast<sc::f32>(g.below(4096))}); return v; }();
  return bench::fold_number(0, m::ring_area(in));
}
