// The cases of src_wire.cpp and src64_wire.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
#include "src_helpers.inc"
bench::u64 SRC(wire_put_u32)() {
  static const auto in = [] { bench::Gen g(17); std::vector<SC::u32> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(static_cast<SC::u32>(g.next())); return v; }();
  static std::vector<SC::u8> b;
  b.clear();
  for (SC::u32 v : in) SC::wire::put_u32(b, v);
  bench::u64 h = 0;
  for (SC::u8 x : b) h = bench::fold(h, x);
  return h;
}
bench::u64 SRC(wire_put_f32)() {
  static const auto in = numbers(18, bench::kMany, -1000, 1000);
  static std::vector<SC::u8> b;
  b.clear();
  for (double v : in) SC::wire::put_f32(b, static_cast<f32>(v / 3));
  bench::u64 h = 0;
  for (SC::u8 x : b) h = bench::fold(h, x);
  return h;
}
bench::u64 SRC(wire_get_u32)() {
  static const auto in = [] { bench::Gen g(19); std::vector<SC::u8> v; for (int i = 0; i < 4 * bench::kMany; ++i) v.push_back(static_cast<SC::u8>(g.below(256))); return v; }();
  SC::wire::Reader r;
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold(h, r.get_u32(in));
  return bench::fold(h, r.ok);
}
