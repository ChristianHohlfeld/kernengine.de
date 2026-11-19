// The cases of src_lamps.cpp and src64_lamps.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
namespace l = SC::lamps;
namespace {
std::vector<SC::Vec2> road(int points, bench::u64 seed) {
  bench::Gen g(seed);
  std::vector<SC::Vec2> v;
  double x = 0, y = 0;
  for (int i = 0; i < points; ++i) {
    v.push_back({static_cast<SC::f32>(x), static_cast<SC::f32>(y)});
    x += g.range(20, 80);
    y += g.range(-40, 40);
  }
  return v;
}
}  // namespace
bench::u64 SRC(lamps_grid_key)() {
  static const auto in = [] { bench::Gen g(20); std::vector<double> v; for (int i = 0; i < 2 * bench::kMany; ++i) v.push_back(g.range(-5000, 5000)); return v; }();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold(h, static_cast<bench::u64>(l::grid_key(static_cast<SC::f32>(in[i]), static_cast<SC::f32>(in[i + 1]))));
  return h;
}
bench::u64 SRC(lamps_place_road)() {
  static const auto pts = road(24, 21);
  l::Field f;
  f.place_road(pts, 12, 1, 0);
  // (what was placed, read through near() at three points of the road: the test's copy of Field has
  // only what the converted functions use - not all())
  bench::u64 h = 0;
  for (int k : {0, 11, 23})
    for (SC::i32 i : f.near(pts[k].x, pts[k].y, 40)) h = bench::fold(h, static_cast<bench::u64>(i));
  return h;
}
bench::u64 SRC(lamps_near)() {
  static const l::Field f = [] { l::Field f; f.place_road(road(200, 22), 12, 1, 0); return f; }();
  static const auto in = [] { bench::Gen g(23); std::vector<double> v; for (int i = 0; i < 2 * 64; ++i) v.push_back(i % 2 ? g.range(-200, 200) : g.range(0, 9000)); return v; }();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2)
    for (SC::i32 k : f.near(static_cast<SC::f32>(in[i]), static_cast<SC::f32>(in[i + 1]), 40)) h = bench::fold(h, static_cast<bench::u64>(k));
  return h;
}
