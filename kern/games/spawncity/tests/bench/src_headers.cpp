// The source side of the measurement (bench.h): what the game's headers define (common.h,
// person_hit.h, wire.h, tiles.h), as the game builds it.
#include "../../src/common.h"
#include "../../src/person_hit.h"
#include "../../src/tiles.h"
#include "../../src/wire.h"
#include "bench.h"
using sc::f32;
namespace {
std::vector<double> numbers(bench::u64 seed, int n, double a, double b) {
  bench::Gen g(seed);
  std::vector<double> v;
  for (int i = 0; i < n; ++i) v.push_back(g.range(a, b));
  return v;
}
sc::Quat quat(const double* q) { return {static_cast<f32>(q[0]), static_cast<f32>(q[1]), static_cast<f32>(q[2]), static_cast<f32>(q[3])}; }
sc::Vec3 vec(const double* v) { return {static_cast<f32>(v[0]), static_cast<f32>(v[1]), static_cast<f32>(v[2])}; }
bench::u64 fold(bench::u64 h, sc::Quat q) { return bench::fold_number(bench::fold_number(bench::fold_number(bench::fold_number(h, q.x), q.y), q.z), q.w); }
bench::u64 fold(bench::u64 h, sc::Vec3 v) { return bench::fold_number(bench::fold_number(bench::fold_number(h, v.x), v.y), v.z); }
}  // namespace
bench::u64 src_common_rng_next() {
  sc::Rng r{12345u};
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold(h, r.next());
  return h;
}
bench::u64 src_common_rng_unit() {
  sc::Rng r{12345u};
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold_number(h, r.unit());
  return h;
}
bench::u64 src_tiles_key_hash() {
  static const auto in = [] { bench::Gen g(8); std::vector<sc::TileKey> v; for (int i = 0; i < bench::kMany; ++i) v.push_back({14, g.below(1 << 14), g.below(1 << 14)}); return v; }();
  bench::u64 h = 0;
  for (const sc::TileKey& k : in) h = bench::fold(h, k.hash());
  return h;
}
bench::u64 src_common_wrap_pi() {
  static const auto in = numbers(10, bench::kMany, -20, 20);
  bench::u64 h = 0;
  for (double a : in) h = bench::fold_number(h, sc::wrap_pi(static_cast<f32>(a)));
  return h;
}
bench::u64 src_common_damp_alpha() {
  static const auto in = numbers(11, 2 * bench::kMany, 0, 0.5);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold_number(h, sc::damp_alpha(static_cast<f32>(in[i] * 20), static_cast<f32>(in[i + 1] / 5)));
  return h;
}
bench::u64 src_common_qnormalize() {
  static const auto in = numbers(12, 4 * bench::kMany, -1, 1);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 4) h = fold(h, sc::qnormalize(quat(&in[i])));
  return h;
}
bench::u64 src_common_slerp() {
  static const auto in = numbers(13, 9 * bench::kMany, -1, 1);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 9)
    h = fold(h, sc::slerp(sc::qnormalize(quat(&in[i])), sc::qnormalize(quat(&in[i + 4])), static_cast<f32>(in[i + 8] * 0.5 + 0.5)));
  return h;
}
bench::u64 src_common_to_local() {
  static const auto in = numbers(14, 7 * bench::kMany, -1, 1);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 7) h = fold(h, sc::to_local(sc::qnormalize(quat(&in[i])), vec(&in[i + 4])));
  return h;
}
bench::u64 src_person_hit() {
  static const auto in = numbers(15, 10 * bench::kMany, -4, 4);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 10) {
    const double* p = &in[i];
    const sc::Vec3 feet{static_cast<f32>(p[0] / 4), static_cast<f32>(p[1] / 4), 0}, from{static_cast<f32>(p[4]), static_cast<f32>(p[5]), static_cast<f32>(p[6] / 4 + 1)};
    const sc::Vec3 seg{static_cast<f32>((p[0] / 4 - p[4]) * 2 + p[7] / 8), static_cast<f32>((p[1] / 4 - p[5]) * 2 + p[8] / 8), static_cast<f32>(p[9] / 8)};
    f32 t = 0;
    const sc::BodyPart part = sc::person_hit::hit(feet, static_cast<f32>(p[2]), static_cast<f32>(p[3] + 4), from, seg, t);
    h = bench::fold_number(bench::fold(h, static_cast<bench::u64>(part)), t);
  }
  return h;
}
bench::u64 src_person_sphere() {
  static const auto in = numbers(16, 10 * bench::kMany, -4, 4);
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 10) {
    f32 t = 0;
    const bool hit = sc::person_hit::sphere_entry(vec(&in[i]), vec(&in[i + 3]), vec(&in[i + 6]), static_cast<f32>(in[i + 9] / 4 + 1.5), t);
    h = bench::fold_number(bench::fold(h, hit), t);
  }
  return h;
}
bench::u64 src_wire_put_u32() {
  static const auto in = [] { bench::Gen g(17); std::vector<sc::u32> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(static_cast<sc::u32>(g.next())); return v; }();
  static std::vector<sc::u8> b;
  b.clear();
  for (sc::u32 v : in) sc::wire::put_u32(b, v);
  bench::u64 h = 0;
  for (sc::u8 x : b) h = bench::fold(h, x);
  return h;
}
bench::u64 src_wire_put_f32() {
  static const auto in = numbers(18, bench::kMany, -1000, 1000);
  static std::vector<sc::u8> b;
  b.clear();
  for (double v : in) sc::wire::put_f32(b, static_cast<f32>(v / 3));
  bench::u64 h = 0;
  for (sc::u8 x : b) h = bench::fold(h, x);
  return h;
}
bench::u64 src_wire_get_u32() {
  static const auto in = [] { bench::Gen g(19); std::vector<sc::u8> v; for (int i = 0; i < 4 * bench::kMany; ++i) v.push_back(static_cast<sc::u8>(g.below(256))); return v; }();
  sc::wire::Reader r;
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold(h, r.get_u32(in));
  return bench::fold(h, r.ok);
}
