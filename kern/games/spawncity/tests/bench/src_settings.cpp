// The source side of the measurement (bench.h): settings.cpp, as the game builds it.
#include "../../src/settings.cpp"
#include "bench.h"
namespace {
std::vector<std::string> texts() {
  bench::Gen g(26);
  std::vector<std::string> v(16);
  for (std::string& s : v)
    for (int i = 0; i < 40; ++i) {
      const int c = g.below(100);
      s += c < 3 ? '"' : c < 5 ? '\\' : c < 7 ? '\t' : static_cast<char>('a' + c % 26);
    }
  return v;
}
}  // namespace
bench::u64 src_settings_escape() {
  static const auto in = texts();
  static std::string out;
  bench::u64 h = 0;
  for (const std::string& s : in) {
    out.clear();
    sc::escape_into(out, s);
    for (char c : out) h = bench::fold(h, static_cast<unsigned char>(c));
  }
  return h;
}
bench::u64 src_settings_num() {
  static const auto in = [] { bench::Gen g(27); std::vector<double> v; for (int i = 0; i < 64; ++i) v.push_back(g.range(-1000, 1000)); return v; }();
  bench::u64 h = 0;
  for (double x : in)
    for (char c : sc::num(static_cast<sc::f32>(x))) h = bench::fold(h, static_cast<unsigned char>(c));
  return h;
}
