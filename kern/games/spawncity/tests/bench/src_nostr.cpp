// The source side of the measurement (bench.h): nostr.cpp, as the game builds it.
#include "../../src/nostr.cpp"
#include "bench.h"
namespace n = sc::nostr;
bench::u64 src_nostr_sha256() {
  static const auto in = [] { bench::Gen g(3); std::vector<std::vector<sc::u8>> v(4); for (auto& d : v) for (int i = 0; i < 100; ++i) d.push_back(static_cast<sc::u8>(g.below(256))); return v; }();
  bench::u64 h = 0;
  for (const auto& d : in)
    for (sc::u8 b : n::sha256(d)) h = bench::fold(h, b);
  return h;
}
bench::u64 src_nostr_mulm() {
  static const auto in = [] { bench::Gen g(4); std::vector<n::U256> v(32); for (auto& x : v) for (int k = 0; k < 8; ++k) x.w[k] = static_cast<sc::u32>(g.next()) & (k == 7 ? 0x7fffffffu : 0xffffffffu); return v; }();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) {
    const n::U256 r = n::mulm(in[i], in[i + 1], n::P());
    for (sc::u32 w : r.w) h = bench::fold(h, w);
  }
  return h;
}
bench::u64 src_nostr_from_hex() {
  static const std::string hex = bench::kHex;
  bench::u64 h = 0;
  for (int i = 0; i < 16; ++i) {
    n::Bytes32 out{};
    h = bench::fold(h, n::from_hex(hex, out));
    for (sc::u8 b : out) h = bench::fold(h, b);
  }
  return h;
}
