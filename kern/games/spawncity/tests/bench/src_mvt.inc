// The cases of src_mvt.cpp and src64_mvt.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
namespace m = SC::mvt;
bench::u64 SRC(mvt_zigzag)() {
  static const auto in = [] { bench::Gen g(5); std::vector<SC::u64> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(g.next()); return v; }();
  bench::u64 h = 0;
  for (SC::u64 x : in) h = bench::fold(h, static_cast<bench::u64>(m::zigzag(x)));
  return h;
}
bench::u64 SRC(mvt_read_geometry)() {
  static const std::vector<SC::u8> g = bench::geometry(200);
  std::vector<std::vector<SC::Vec2>> parts;
  m::read_geometry(kern::Span<const SC::u8>(g.data(), g.size()), m::Reader{0, g.size()}, m::GeomType::line, parts);
  bench::u64 h = bench::fold(0, parts.size());
  for (const auto& p : parts)
    for (const SC::Vec2& v : p) h = bench::fold_number(bench::fold_number(h, v.x), v.y);
  return h;
}
bench::u64 SRC(mvt_ring_area)() {
  static const auto in = [] { bench::Gen g(6); std::vector<SC::Vec2> v; for (int i = 0; i < bench::kMany; ++i) v.push_back({static_cast<SC::f32>(g.below(4096)), static_cast<SC::f32>(g.below(4096))}); return v; }();
  return bench::fold_number(0, m::ring_area(in));
}
