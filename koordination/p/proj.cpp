// Paket p: was eine Formel mit ganzzahligen Ein-/Ausgängen brächte (Entwurf) - Rng::next dreimal:
// wie die Klasse heute (Float-Lambda), mit ganzzahligem Lambda (Exact hinein, normal heraus), und das C++.
#include <chrono>
#include <cstdio>
#include "kern/logic/blueprint.h"
using namespace kern;
using Int = logic::Whole; using Float = f64;
namespace m = kern::math;
inline Int out(m::Exact r) { return Int(r.v); }  // (normal: what whole((f64)r) gives)
__attribute__((noinline)) Int today(Int s) {
  s = kern::to<Int>(kern::formula([](Float s) -> Float { return m::bxor(s, m::wrap32(m::shl(s, 13.0))); }, kern::to<Float>(s)));
  s = kern::to<Int>(kern::formula([](Float s) -> Float { return m::bxor(s, m::shr(s, 17.0)); }, kern::to<Float>(s)));
  s = kern::to<Int>(kern::formula([](Float s) -> Float { return m::bxor(s, m::wrap32(m::shl(s, 5.0))); }, kern::to<Float>(s)));
  return s;
}
__attribute__((noinline)) Int whole_lambda(Int s) {
  s = out([](m::Exact s) -> m::Exact { return m::bxor(s, m::wrap32(m::shl(s, 13.0))); }(m::Exact{s}));
  s = out([](m::Exact s) -> m::Exact { return m::bxor(s, m::shr(s, 17.0)); }(m::Exact{s}));
  s = out([](m::Exact s) -> m::Exact { return m::bxor(s, m::wrap32(m::shl(s, 5.0))); }(m::Exact{s}));
  return s;
}
__attribute__((noinline)) u32 cpp(u32 s) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
template <class F, class T> double time(F f, T s) {
  double best = 1e9;
  for (int r = 0; r < 15; ++r) {
    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < 1000000; ++i) s = f(s);
    best = std::min(best, std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count());
    asm volatile("" : : "g"(&s) : "memory");
  }
  return best * 1e3;
}
int main() {
  Int a = 2463534242, b = 2463534242; u32 c = 2463534242u;
  for (int i = 0; i < 1000; ++i) { a = today(a); b = whole_lambda(b); c = cpp(c); if (a.v != b.v || a.v != c) { std::printf("verschieden\n"); return 1; } }
  std::printf("heute %.2f ns, ganzzahliges Lambda %.2f ns, C++ %.2f ns je Aufruf\n", time(today, a), time(whole_lambda, b), time(cpp, c));
}
