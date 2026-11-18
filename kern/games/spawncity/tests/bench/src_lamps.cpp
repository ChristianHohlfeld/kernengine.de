// The source side of the measurement (bench.h): lamps.cpp, as the game builds it.
#include "../../src/lamps.cpp"
#include "bench.h"
namespace l = sc::lamps;
namespace {
std::vector<sc::Vec2> road(int points, bench::u64 seed) {
  bench::Gen g(seed);
  std::vector<sc::Vec2> v;
  double x = 0, y = 0;
  for (int i = 0; i < points; ++i) {
    v.push_back({static_cast<sc::f32>(x), static_cast<sc::f32>(y)});
    x += g.range(20, 80);
    y += g.range(-40, 40);
  }
  return v;
}
}  // namespace
bench::u64 src_lamps_grid_key() {
  static const auto in = [] { bench::Gen g(20); std::vector<double> v; for (int i = 0; i < 2 * bench::kMany; ++i) v.push_back(g.range(-5000, 5000)); return v; }();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold(h, static_cast<bench::u64>(l::grid_key(static_cast<sc::f32>(in[i]), static_cast<sc::f32>(in[i + 1]))));
  return h;
}
bench::u64 src_lamps_place_road() {
  static const auto pts = road(24, 21);
  l::Field f;
  f.place_road(pts, 12, 1, 0);
  bench::u64 h = bench::fold(0, f.all().size());
  for (const l::Lamp& lp : f.all()) h = bench::fold(bench::fold_number(h, lp.yaw), static_cast<bench::u64>(lp.key));
  return h;
}
bench::u64 src_lamps_near() {
  static const l::Field f = [] { l::Field f; f.place_road(road(200, 22), 12, 1, 0); return f; }();
  static const auto in = [] { bench::Gen g(23); std::vector<double> v; for (int i = 0; i < 2 * 64; ++i) v.push_back(i % 2 ? g.range(-200, 200) : g.range(0, 9000)); return v; }();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2)
    for (sc::i32 k : f.near(static_cast<sc::f32>(in[i]), static_cast<sc::f32>(in[i + 1]), 40)) h = bench::fold(h, static_cast<bench::u64>(k));
  return h;
}
