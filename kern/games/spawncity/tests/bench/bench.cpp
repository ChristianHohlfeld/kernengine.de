// The measurement (bench.h): the class side of every case and the clock. Built and run by
// tools/blueprint_bench.sh, which writes classes.h (tools/blueprint_cpp.sh --classes) first.
//
//   bench [--check <limit>] [rounds] [case ...]     (default 15 rounds, every case)
//
// --check: the time as a test - it fails (exit 1) when for a case the class's best time is more
// than <limit> times that of the source in the numbers of the language (src64, cases.h) (the best of the rounds, both measured in turn: noise only makes
// a run slower, so the best is the steadiest - on this machine the best ratio moved by at most 6 %
// between runs, the median by up to 15 %), or when the checksums differ where they must not.
//
// Prints per case: the time per call of the source and of the class (minimum and median over the
// rounds, each round long enough for the clock), their ratio (class / source), and whether the
// checksums are equal ("=": equal, as they must be where the source computes in the language's
// numbers; "f32": the source computes in f32, the class in 64 bit - not compared here).
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "bench.h"
#include "cases.h"
#include "classes.h"

#define DECLARE_SRC(name, kind, same) bench::u64 src_##name(); bench::u64 src64_##name();
BENCH_CASES(DECLARE_SRC)
#undef DECLARE_SRC

// (never reached: the loop guard's limit is reset before every round - the runtime's way when a loop
// is over it; JetSeats' Claim goes through the runtime to the actor and is not measured here)
bool kern::logic::Runtime::pass(Instance&) { return false; }

namespace {

namespace bp = blueprints;
using Int = kern::logic::Whole;
using Float = kern::f64;
using kern::logic::Int64;
template <class T> using List = kern::List<T>;

// The loop guard as in the game: the runtime's counters, its limit (a compare and a count per pass).
kern::i32 g_counters[5] = {0, 2000000000, 0, 0, 0};
template <class C> C& the() {
  static C c = [] {
    C made;
    made.kern_world.counters = g_counters;
    return made;
  }();
  return c;
}
std::vector<double> numbers(bench::u64 seed, int n, double a, double b) {
  bench::Gen g(seed);
  std::vector<double> v;
  for (int i = 0; i < n; ++i) v.push_back(g.range(a, b));
  return v;
}
using Quat = bp::common::Quat;
Quat quat(const double* q) { return {q[0], q[1], q[2], q[3]}; }
kern::Vec3 vec(const double* v) { return {static_cast<kern::f32>(v[0]), static_cast<kern::f32>(v[1]), static_cast<kern::f32>(v[2])}; }
bench::u64 fold(bench::u64 h, const Quat& q) { return bench::fold_number(bench::fold_number(bench::fold_number(bench::fold_number(h, q.x), q.y), q.z), q.w); }
bench::u64 fold(bench::u64 h, kern::Vec3 v) { return bench::fold_number(bench::fold_number(bench::fold_number(h, v.x), v.y), v.z); }
List<Int> ints(const std::vector<unsigned char>& b) {
  List<Int> r;
  for (unsigned char x : b) r.push_back(Int(x));
  return r;
}
List<kern::Vec3> road(int points, bench::u64 seed) {
  bench::Gen g(seed);
  List<kern::Vec3> v;
  double x = 0, y = 0;
  for (int i = 0; i < points; ++i) {
    v.push_back({static_cast<kern::f32>(x), static_cast<kern::f32>(y), 0});
    x += g.range(20, 80);
    y += g.range(-40, 40);
  }
  return v;
}

}  // namespace

bench::u64 cls_qr_encode() {
  static const kern::logic::Text url = std::string(bench::kUrl);
  bp::qr_code::qr_Code c;
  the<bp::qr_code_cpp>().encode(url, c);
  bench::u64 h = bench::fold(0, static_cast<bench::u64>(static_cast<kern::i64>(c.version)));
  for (const Int& m : c.modules) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(m)));
  return h;
}
bench::u64 cls_qr_gf_mul() {
  static const auto in = [] { bench::Gen g(1); std::vector<int> v; for (int i = 0; i < 2 * bench::kMany; ++i) v.push_back(g.below(256)); return v; }();
  auto& q = the<bp::qr_code_cpp>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(q.gf_mul(in[i], in[i + 1]).Result)));
  return h;
}
bench::u64 cls_qr_rs_remainder() {
  static const auto in = [] { bench::Gen g(2); std::vector<List<Int>> v(8); for (auto& d : v) for (int i = 0; i < 64; ++i) d.push_back(Int(g.below(256))); return v; }();
  auto& q = the<bp::qr_code_cpp>();
  static const List<Int> div = q.rs_divisor(18).Result;
  bench::u64 h = 0;
  for (const auto& d : in)
    for (const Int& b : q.rs_remainder(d, div).Result) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(b)));
  return h;
}
bench::u64 cls_nostr_sha256() {
  static const auto in = [] { bench::Gen g(3); std::vector<List<Int>> v(4); for (auto& d : v) for (int i = 0; i < 100; ++i) d.push_back(Int(g.below(256))); return v; }();
  auto& n = the<bp::nostr_cpp>();
  bench::u64 h = 0;
  for (const auto& d : in)
    for (const Int& b : n.sha256(d).Result) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(b)));
  return h;
}
bench::u64 cls_nostr_mulm() {
  using U256 = bp::nostr_cpp::nostr_U256;
  static const auto in = [] { bench::Gen g(4); std::vector<U256> v(32); for (auto& x : v) for (int k = 0; k < 8; ++k) x.w[k] = Int(static_cast<kern::i64>(static_cast<kern::u32>(g.next()) & (k == 7 ? 0x7fffffffu : 0xffffffffu))); return v; }();
  auto& n = the<bp::nostr_cpp>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) {
    const U256 r = n.mulm(in[i], in[i + 1], bp::nostr_cpp::kP).Result;
    for (const Int& w : r.w) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(w)));
  }
  return h;
}
bench::u64 cls_nostr_from_hex() {
  static const kern::logic::Text hex = std::string(bench::kHex);
  auto& n = the<bp::nostr_cpp>();
  bench::u64 h = 0;
  for (int i = 0; i < 16; ++i) {
    kern::Fixed<Int, 32> out{};
    h = bench::fold(h, n.from_hex(out, hex).Result);
    for (const Int& b : out) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(b)));
  }
  return h;
}
bench::u64 cls_common_rng_next() {
  bp::common::Rng r{Int(12345), 0.0, false};
  auto& c = the<bp::common>();
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(c.Rng_next(r).Result)));
  return h;
}
bench::u64 cls_common_rng_unit() {
  bp::common::Rng r{Int(12345), 0.0, false};
  auto& c = the<bp::common>();
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold_number(h, c.Rng_unit(r).Result);
  return h;
}
bench::u64 cls_tiles_key_hash() {
  static const auto in = [] { bench::Gen g(8); std::vector<bp::tiles::TileKey> v; for (int i = 0; i < bench::kMany; ++i) { const int x = g.below(1 << 14); v.push_back({Int(14), Int(x), Int(g.below(1 << 14))}); } return v; }();
  auto& t = the<bp::tiles>();
  bench::u64 h = 0;
  for (const auto& k : in) h = bench::fold(h, t.TileKey_hash(k).Result.bits);
  return h;
}
bench::u64 cls_lamps_grid_key() {
  static const auto in = [] { bench::Gen g(20); std::vector<double> v; for (int i = 0; i < 2 * bench::kMany; ++i) v.push_back(g.range(-5000, 5000)); return v; }();
  auto& l = the<bp::lamps_cpp>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold(h, l.grid_key(in[i], in[i + 1]).Result.bits);
  return h;
}
bench::u64 cls_mvt_zigzag() {
  static const auto in = [] { bench::Gen g(5); std::vector<Int64> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(Int64(g.next())); return v; }();
  auto& m = the<bp::mvt_cpp>();
  bench::u64 h = 0;
  for (Int64 x : in) h = bench::fold(h, m.zigzag(x).Result.bits);
  return h;
}
bench::u64 cls_mvt_read_geometry() {
  static const List<Int> g = ints(bench::geometry(200));
  List<bp::shapes::Vector_row> parts;
  the<bp::mvt_cpp>().read_geometry(g, parts, bp::mvt_cpp::mvt_Reader{Int(0), Int(static_cast<kern::i64>(g.size())), true}, bp::mvt_cpp::mvt_GeomType::line);
  bench::u64 h = bench::fold(0, parts.size());
  for (const auto& p : parts)
    for (const kern::Vec3& v : p.items) h = bench::fold_number(bench::fold_number(h, v.x), v.y);
  return h;
}
bench::u64 cls_mvt_ring_area() {
  static const auto in = [] { bench::Gen g(6); List<kern::Vec3> v; for (int i = 0; i < bench::kMany; ++i) { const int x = g.below(4096); v.push_back({static_cast<kern::f32>(x), static_cast<kern::f32>(g.below(4096)), 0}); } return v; }();
  return bench::fold_number(0, the<bp::mvt_cpp>().ring_area(in).Result);
}
bench::u64 cls_wire_put_u32() {
  static const auto in = [] { bench::Gen g(17); std::vector<kern::i64> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(static_cast<kern::u32>(g.next())); return v; }();
  static List<Int> b;
  auto& w = the<bp::wire>();
  b.clear();
  for (kern::i64 v : in) w.put_u32(b, v);
  bench::u64 h = 0;
  for (const Int& x : b) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(x)));
  return h;
}
bench::u64 cls_wire_put_f32() {
  static const auto in = numbers(18, bench::kMany, -1000, 1000);
  static List<Int> b;
  auto& w = the<bp::wire>();
  b.clear();
  for (double v : in) w.put_f32(b, v / 3);
  bench::u64 h = 0;
  for (const Int& x : b) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(x)));
  return h;
}
bench::u64 cls_wire_get_u32() {
  static const auto in = [] { bench::Gen g(19); List<Int> v; for (int i = 0; i < 4 * bench::kMany; ++i) v.push_back(Int(g.below(256))); return v; }();
  auto& w = the<bp::wire>();
  bp::wire::wire_Reader r{Int(0), true};
  bench::u64 h = 0;
  for (int i = 0; i < bench::kMany; ++i) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(w.Reader_get_u32(r, in).Result)));
  return bench::fold(h, r.ok);
}
bench::u64 cls_settings_escape() {
  static const auto in = [] {
    bench::Gen g(26);
    std::vector<kern::logic::Text> v;
    for (int k = 0; k < 16; ++k) {
      std::string s;
      for (int i = 0; i < 40; ++i) {
        const int c = g.below(100);
        s += c < 3 ? '"' : c < 5 ? '\\' : c < 7 ? '\t' : static_cast<char>('a' + c % 26);
      }
      v.emplace_back(std::move(s));
    }
    return v;
  }();
  auto& st = the<bp::settings_cpp>();
  static kern::logic::Text out;
  bench::u64 h = 0;
  for (const kern::logic::Text& s : in) {
    out = kern::logic::Text();
    st.escape_into(out, s);
    for (char c : out.view()) h = bench::fold(h, static_cast<unsigned char>(c));
  }
  return h;
}
bench::u64 cls_settings_num() {
  static const auto in = [] { bench::Gen g(27); std::vector<double> v; for (int i = 0; i < 64; ++i) v.push_back(g.range(-1000, 1000)); return v; }();
  auto& st = the<bp::settings_cpp>();
  bench::u64 h = 0;
  for (double x : in)
    for (char c : st.num(x).Result.view()) h = bench::fold(h, static_cast<unsigned char>(c));
  return h;
}
bench::u64 cls_common_wrap_pi() {
  static const auto in = numbers(10, bench::kMany, -20, 20);
  auto& c = the<bp::common>();
  bench::u64 h = 0;
  for (double a : in) h = bench::fold_number(h, c.wrap_pi(a).Result);
  return h;
}
bench::u64 cls_common_damp_alpha() {
  static const auto in = numbers(11, 2 * bench::kMany, 0, 0.5);
  auto& c = the<bp::common>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2) h = bench::fold_number(h, c.damp_alpha(in[i] * 20, in[i + 1] / 5).Result);
  return h;
}
bench::u64 cls_common_qnormalize() {
  static const auto in = numbers(12, 4 * bench::kMany, -1, 1);
  auto& c = the<bp::common>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 4) h = fold(h, c.qnormalize(quat(&in[i])).Result);
  return h;
}
bench::u64 cls_common_slerp() {
  static const auto in = numbers(13, 9 * bench::kMany, -1, 1);
  auto& c = the<bp::common>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 9)
    h = fold(h, c.slerp(c.qnormalize(quat(&in[i])).Result, c.qnormalize(quat(&in[i + 4])).Result, in[i + 8] * 0.5 + 0.5).Result);
  return h;
}
bench::u64 cls_common_to_local() {
  static const auto in = numbers(14, 7 * bench::kMany, -1, 1);
  auto& c = the<bp::common>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 7) h = fold(h, c.to_local(c.qnormalize(quat(&in[i])).Result, vec(&in[i + 4])).Result);
  return h;
}
bench::u64 cls_person_hit() {
  static const auto in = numbers(15, 10 * bench::kMany, -4, 4);
  auto& ph = the<bp::person_hit>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 10) {
    const double* p = &in[i];
    const kern::Vec3 feet{static_cast<kern::f32>(p[0] / 4), static_cast<kern::f32>(p[1] / 4), 0}, from{static_cast<kern::f32>(p[4]), static_cast<kern::f32>(p[5]), static_cast<kern::f32>(p[6] / 4 + 1)};
    const kern::Vec3 seg{static_cast<kern::f32>((p[0] / 4 - p[4]) * 2 + p[7] / 8), static_cast<kern::f32>((p[1] / 4 - p[5]) * 2 + p[8] / 8), static_cast<kern::f32>(p[9] / 8)};
    Float t = 0;
    const auto part = ph.hit(t, feet, p[2], p[3] + 4, from, seg, 1.0, 1.0).Result;
    h = bench::fold_number(bench::fold(h, static_cast<bench::u64>(part)), t);
  }
  return h;
}
bench::u64 cls_person_sphere() {
  static const auto in = numbers(16, 10 * bench::kMany, -4, 4);
  auto& ph = the<bp::person_hit>();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 10) {
    Float t = 0;
    const bool hit = ph.sphere_entry(t, vec(&in[i]), vec(&in[i + 3]), vec(&in[i + 6]), in[i + 9] / 4 + 1.5).Result;
    h = bench::fold_number(bench::fold(h, hit), t);
  }
  return h;
}
bench::u64 cls_perf_governor() {
  static const auto in = [] { bench::Gen g(25); std::vector<double> v; for (int i = 0; i < bench::kMany; ++i) v.push_back(g.range(0.005, 0.06)); return v; }();
  auto& p = the<bp::perf_cpp>();
  bp::perf_cpp::perf_Governor gov;
  bench::u64 h = 0;
  for (double dt : in) h = bench::fold(h, p.Governor_update(gov, dt).Result);
  return h;  // (as src_perf.inc)
}
bench::u64 cls_audio_render() {
  const List<Float> pcm = the<bp::combat_audio_cpp>().render(bp::combat_audio_cpp::combat_audio_Kind::shot, 1, 8000).Result;
  bench::u64 h = bench::fold(0, pcm.size());
  for (Float x : pcm) h = bench::fold_number(h, x);
  return h;
}
bench::u64 cls_lamps_place_road() {
  static const auto pts = road(24, 21);
  auto& l = the<bp::lamps_cpp>();
  bp::lamps_cpp::lamps_Field f;
  l.Field_place_road(f, pts, 12, 1, 0);
  bench::u64 h = 0;  // (as src_lamps.inc: through near() at three points of the road)
  for (int k : {0, 11, 23})
    for (const Int& i : l.Field_near(f, pts[k].x, pts[k].y, 40).Result) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(i)));
  return h;
}
bench::u64 cls_lamps_near() {
  auto& l = the<bp::lamps_cpp>();
  static const bp::lamps_cpp::lamps_Field f = [&] { bp::lamps_cpp::lamps_Field f; l.Field_place_road(f, road(200, 22), 12, 1, 0); return f; }();
  static const auto in = [] { bench::Gen g(23); std::vector<double> v; for (int i = 0; i < 2 * 64; ++i) v.push_back(i % 2 ? g.range(-200, 200) : g.range(0, 9000)); return v; }();
  bench::u64 h = 0;
  for (std::size_t i = 0; i < in.size(); i += 2)
    for (const Int& k : l.Field_near(f, in[i], in[i + 1], 40).Result) h = bench::fold(h, static_cast<bench::u64>(static_cast<kern::i64>(k)));
  return h;
}

namespace {
struct Case {
  const char* name;
  const char* kind;
  bool same;
  bench::u64 (*src)();
  bench::u64 (*src64)();
  bench::u64 (*cls)();
};
#define LIST_CASE(name, kind, same) {#name, kind, same != 0, src_##name, src64_##name, cls_##name},
const Case kCases[] = {BENCH_CASES(LIST_CASE)};
#undef LIST_CASE

double seconds(bench::u64 (*run)(), int reps, bench::u64& sum) {
  g_counters[3] = 0;
  const auto t0 = std::chrono::steady_clock::now();
  for (int r = 0; r < reps; ++r) {
    sum = run();
    bench::keep(sum);
  }
  return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
}
double median(std::vector<double> v) {
  std::sort(v.begin(), v.end());
  return v[v.size() / 2];
}
}  // namespace

int main(int argc, char** argv) {
  double limit = 0;
  if (argc > 2 && std::string(argv[1]) == "--check") {
    limit = std::atof(argv[2]);
    argv += 2, argc -= 2;
  }
  int failed = 0, over_src = 0, over_64 = 0, counted = 0;
  int rounds = argc > 1 ? std::atoi(argv[1]) : 15;
  if (rounds < 1) rounds = 15;
  std::vector<std::string> only(argv + std::min(argc, 2), argv + argc);
  std::printf("%-20s %-20s %9s %9s %9s %7s %7s %7s %7s %-6s %s\n", "Fall", "Art", "C++ min", "C64 min", "Kl. min", "min", "med", "min64", "med64", "gleich", "gleich64");
  for (const Case& c : kCases) {
    if (!only.empty() && std::find(only.begin(), only.end(), c.name) == only.end()) continue;
    bench::u64 s = 0, w = 0, k = 0;
    // (a round runs the source for at least ~2 ms: the clock's step is far below it)
    int reps = 1;
    while (seconds(c.src, reps, s) < 2e-3 && reps < (1 << 20)) reps *= 2;
    std::vector<double> ts, tw, tk;
    for (int r = 0; r < rounds; ++r) {  // (the three in turn: what disturbs a round disturbs each alike)
      ts.push_back(seconds(c.src, reps, s) / reps);
      tw.push_back(seconds(c.src64, reps, w) / reps);
      tk.push_back(seconds(c.cls, reps, k) / reps);
    }
    const double smin = *std::min_element(ts.begin(), ts.end()), wmin = *std::min_element(tw.begin(), tw.end()), kmin = *std::min_element(tk.begin(), tk.end());
    const double smed = median(ts), wmed = median(tw), kmed = median(tk);
    const char* same = s == k ? "=" : c.same ? "ABWEICHUNG" : "f32";
    // (in the numbers of the language the source and the class give the same, bit for bit - as the test compares them)
    const char* same64 = w == k ? "=" : "ABWEICHUNG";
    ++counted;
    over_src += kmin / smin > 1.10 ? 1 : 0;
    over_64 += kmin / wmin > 1.10 ? 1 : 0;
    const bool over = limit > 0 && kmin / wmin > limit;
    failed += (s != k && c.same) || w != k || over ? 1 : 0;
    std::printf("%-20s %-20s %9.2f %9.2f %9.2f %7.2f %7.2f %7.2f %7.2f %-6s %s\n", c.name, c.kind, smin * 1e6, wmin * 1e6, kmin * 1e6, kmin / smin, kmed / smed, kmin / wmin,
                kmed / wmed, same, same64);
    if (over) std::printf("  ^ %s: die Klasse braucht %.2f-mal die Zeit der Quelle in den Zahlen der Sprache (Grenze %.2f)\n", c.name, kmin / wmin, limit);
  }
  std::printf("(Zeiten in µs je Aufruf eines Falls; C++: die Quelle, wie das Spiel sie baut; C64: die Quelle in den Zahlen der Sprache, wie der\n"
              " Vergleichstest sie baut (tests/converted); min/med: Klasse geteilt durch C++, min64/med64: Klasse geteilt durch C64)\n");
  std::printf("gegen die Quelle (f32): %d von %d Fällen über 1,10; gegen die Quelle in 64 Bit: %d von %d Fällen über 1,10\n", over_src, counted, over_64, counted);
  if (failed) std::printf("%d Fall/Fälle über der Grenze oder mit abweichender Prüfsumme\n", failed);
  return failed ? 1 : 0;
}
