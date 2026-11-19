// The cases of src_qr.cpp and src64_qr.cpp (bench.h): SC is the namespace of the source, SRC(name) the case's function.
namespace q = SC::qr;
bench::u64 SRC(qr_encode)() {
  static const std::string url = bench::kUrl;
  q::Code c;
  q::encode(url, c);
  bench::u64 h = bench::fold(0, static_cast<bench::u64>(c.version));
  for (SC::u8 m : c.modules) h = bench::fold(h, m);
  return h;
}
bench::u64 SRC(qr_gf_mul)() {
  static const auto in = [] { bench::Gen g(1); std::vector<int> v; for (int i = 0; i < 2 * bench::kMany; ++i) v.push_back(g.below(256)); return v; }();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold(h, q::gf_mul(static_cast<SC::u8>(in[i]), static_cast<SC::u8>(in[i + 1])));
  return h;
}
bench::u64 SRC(qr_rs_remainder)() {
  static const auto in = [] { bench::Gen g(2); std::vector<std::vector<SC::u8>> v(8); for (auto& d : v) for (int i = 0; i < 64; ++i) d.push_back(static_cast<SC::u8>(g.below(256))); return v; }();
  static const std::vector<SC::u8> div = q::rs_divisor(18);
  bench::u64 h = 0;
  for (const auto& d : in)
    for (SC::u8 b : q::rs_remainder(d, div)) h = bench::fold(h, b);
  return h;
}
