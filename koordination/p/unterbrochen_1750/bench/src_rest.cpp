// The source side of the measurement (bench.h): jet_seats.cpp, perf.cpp, combat_audio.cpp, as the
// game builds them (each its own unit: settings.cpp, below in src_settings.cpp).
#include "../../src/jet_seats.cpp"
#include "../../src/perf.cpp"
#include "../../src/combat_audio.cpp"
#include "bench.h"
bench::u64 src_seats_claim() {
  static const auto in = [] { bench::Gen g(24); std::vector<int> v; for (int i = 0; i < 3 * 64; ++i) v.push_back(g.below(8)); return v; }();
  sc::seats::Table t;
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 3)
    h = bench::fold(h, static_cast<bench::u64>(t.claim(static_cast<sc::u8>(in[i]), static_cast<sc::u8>(in[i + 1]), in[i + 2] % 2 ? sc::seats::Seat::passenger : sc::seats::Seat::pilot)));
  return bench::fold(h, t.rows().size());
}
bench::u64 src_perf_governor() {
  static const auto in = [] { bench::Gen g(25); std::vector<double> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(g.range(0.005, 0.06)); return v; }();
  sc::perf::Governor gov;
  bench::u64 h = 0;
  for (double dt : in) h = bench::fold(h, gov.update(static_cast<sc::f32>(dt)));
  return bench::fold_number(bench::fold(h, static_cast<bench::u64>(gov.tier())), gov.fps());
}
bench::u64 src_audio_render() {
  const std::vector<sc::f32> pcm = sc::combat_audio::render(sc::combat_audio::Kind::shot, 1, 8000);
  bench::u64 h = bench::fold(0, pcm.size());
  for (sc::f32 x : pcm) h = bench::fold_number(h, x);
  return h;
}
