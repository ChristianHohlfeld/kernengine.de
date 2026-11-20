#include "kern/logic/nodes.h"
#include <cstdio>
#include <cstring>
namespace old { using namespace kern; using kern::i64; using kern::u64; using kern::u32; using kern::i32; using kern::f64;
inline i64 bits_of(f64 a) {
  if (!(a == a)) return 0;
  if (a >= 9223372036854775808.0) return std::numeric_limits<i64>::max();
  if (a <= -9223372036854775808.0) return std::numeric_limits<i64>::min();
  return static_cast<i64>(a);
}
inline f64 band(f64 a, f64 b) { return static_cast<f64>(bits_of(a) & bits_of(b)); }
inline f64 bor(f64 a, f64 b) { return static_cast<f64>(bits_of(a) | bits_of(b)); }
inline f64 bxor(f64 a, f64 b) { return static_cast<f64>(bits_of(a) ^ bits_of(b)); }
inline f64 shl(f64 a, f64 n) { return n >= 0 && n < 64 ? static_cast<f64>(static_cast<i64>(static_cast<u64>(bits_of(a)) << static_cast<int>(n))) : 0; }
inline f64 shr(f64 a, f64 n) { return n >= 0 && n < 64 ? static_cast<f64>(bits_of(a) >> static_cast<int>(n)) : (bits_of(a) < 0 ? -1 : 0); }
inline f64 wrap32(f64 a) { return static_cast<f64>(static_cast<u32>(static_cast<u64>(bits_of(a)))); }
inline f64 wrap32s(f64 a) { return static_cast<f64>(static_cast<i32>(static_cast<u32>(static_cast<u64>(bits_of(a))))); }
inline f64 mul32(f64 a, f64 b) { return static_cast<f64>(static_cast<u32>(static_cast<u32>(static_cast<u64>(bits_of(a))) * static_cast<u32>(static_cast<u64>(bits_of(b))))); }
inline i64 whole(f64 v) {
  if (!std::isfinite(v)) return 0;
  if (v >= 9223372036854775808.0) return std::numeric_limits<i64>::max();
  if (v <= -9223372036854775808.0) return -std::numeric_limits<i64>::max();
  return static_cast<i64>(v);
}
}
using namespace kern;
namespace m = kern::math;
static u64 B(f64 x) { u64 b; std::memcpy(&b, &x, 8); return b; }
static long bad = 0, n = 0;
#define EQI(newv, oldv) do { ++n; if (static_cast<i64>(newv) != static_cast<i64>(oldv)) { if (bad++ < 10) std::printf("%s a=%a\n", #newv, a); } } while (0)
#define EQ(newv, oldv) do { ++n; if (B(static_cast<f64>(newv)) != B(static_cast<f64>(oldv))) { if (bad++ < 10) std::printf("%s a=%a b=%a: %a %a\n", #newv, a, b, static_cast<f64>(newv), static_cast<f64>(oldv)); } } while (0)
int main() {
  std::vector<f64> v;
  const f64 e[] = {0, 1, 2, 3, 7, 13, 31, 32, 63, 64, 65, 2147483648.0, 4294967295.0, 4294967296.0, 9007199254740991.0, 9007199254740992.0, 9007199254740994.0,
                   4503599627370496.0, 9223372036854775807.0, 9223372036854774784.0, 18446744073709551616.0, 0.5, 2.75, 1e300, INFINITY};
  for (f64 x : e) { v.push_back(x); v.push_back(-x); }
  v.push_back(NAN); v.push_back(-NAN);
  u64 s = 1;
  for (int i = 0; i < 300; ++i) { s = s * 6364136223846793005ull + 1442695040888963407ull; v.push_back(static_cast<f64>(static_cast<i64>(s) >> (s % 64))); }
  for (f64 a : v) { const f64 b = 0; (void)b;
    { const i64 x = static_cast<i64>(s = s * 6364136223846793005ull + 1442695040888963407ull); EQI(m::bits_of(m::Exact{x}), old::bits_of(static_cast<f64>(x))); EQI(m::bits_of(m::Exact{x >> 9}), old::bits_of(static_cast<f64>(x >> 9))); }
    EQ(m::wrap32(a), old::wrap32(a)); EQ(m::wrap32s(a), old::wrap32s(a)); EQI(logic::whole(a), old::whole(a)); EQI(m::bits_of(a), old::bits_of(a));
    EQI(logic::detail::To<logic::Whole>::from(a).v, logic::Whole(old::whole(a)).v);
    for (f64 b2 : v) { const f64 b = b2;
      EQ(m::band(a, b), old::band(a, b)); EQ(m::bor(a, b), old::bor(a, b)); EQ(m::bxor(a, b), old::bxor(a, b));
      EQ(m::shl(a, b), old::shl(a, b)); EQ(m::shr(a, b), old::shr(a, b)); EQ(m::mul32(a, b), old::mul32(a, b));
      // handed on (Exact into another): the same as the double into it
      EQ(m::bxor(m::shl(a, 13), m::shr(b, 17)), old::bxor(old::shl(a, 13), old::shr(b, 17)));
      EQ(m::band(m::bor(a, b), m::wrap32s(m::bxor(a, b))), old::band(old::bor(a, b), old::wrap32s(old::bxor(a, b))));
      EQ(m::shl(m::wrap32(a), m::shr(b, 60)), old::shl(old::wrap32(a), old::shr(b, 60)));
      EQ(m::shr(m::shl(a, 10), 3.0), old::shr(old::shl(a, 10), 3.0));
      EQ(m::mul32(m::shr(a, 1.0), m::band(b, 4294967295.0)), old::mul32(old::shr(a, 1.0), old::band(b, 4294967295.0)));
      EQ(m::shl(a, m::band(b, 63.0)), old::shl(a, old::band(b, 63.0)));
      EQ(m::bor(m::shl(a, 1.0), m::shl(b, 2.0)), old::bor(old::shl(a, 1.0), old::shl(b, 2.0)));
      EQ(m::shr(m::bxor(a, b), m::wrap32(b)), old::shr(old::bxor(a, b), old::wrap32(b)));
    }
  }
  std::printf("%ld Vergleiche, %ld verschieden\n", n, bad);
  return bad != 0;
}
