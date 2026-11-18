#include <cstdint>
#include <cstdio>
#include <string>
#include <array>
namespace sc {
using u16 = std::uint16_t; using u32 = std::uint32_t; using i64 = std::int64_t; using u64 = std::uint64_t; using u8 = std::uint8_t;
struct Seq { u16 seq = 1; u8 small = 0; u32 big = 0; };
u16 bump(Seq& s, int n) { u16 last = 0; for (int i = 0; i < n; ++i) last = s.seq++; return last; }
u16 add16(u16 a, u16 b) { a += b; return a; }
u8 sub8(u8 a) { a -= 3; --a; return a; }
u32 inc32(u32 x) { ++x; x += 7u; return x; }
void member(Seq& s) { s.small += 200; s.big++; ++s.seq; }
std::string key(i64 cx, i64 cy, u64 h) {
  char k[64];
  std::snprintf(k, sizeof k, "f:%lld:%-22lld|%llu|%llx", static_cast<long long>(cx), static_cast<long long>(cy), static_cast<unsigned long long>(h), static_cast<unsigned long long>(h));
  return std::string(k);
}
std::string str(i64 v, u64 w) { return std::to_string(static_cast<long long>(v)) + std::to_string(w); }
}
namespace sc {
struct PT { float c = 0; float y = 0; bool init = false; PT() = default; explicit PT(float cc) : c(cc) {} };
struct Filt { std::array<PT, 3> g{PT(100.0f), PT(100.0f), PT(100.0f)}; float raw[4] = {1, 2}; struct Ap { int on; float p; }; Ap ap[2]; };
struct Rt { Filt f; int n = 0; };
float run(Rt& r, float x) { r.f.g[1].y += x; r.n++; return r.f.g[1].y + r.f.raw[1] + r.f.g[0].c; }
Rt make() { return Rt{}; }
}
#include <kern/core/map.h>
#include <vector>
namespace sc {
struct Cells { kern::Map<u64, std::vector<u32>> ids; };
u64 ck(int x, int y) { return (static_cast<u64>(static_cast<u32>(x)) << 32) | static_cast<u32>(y); }
void put_id(Cells& c, int x, int y, u32 id) { c.ids[ck(x, y)].push_back(id); }
}
namespace sc {
struct Ref { u64 tile = 0; u32 index = 0; };
struct Tiles { kern::Map<u64, std::vector<u32>> m; };
void refs(const Tiles& t, std::vector<Ref>& out) {
  for (const auto& [k, v] : t.m)
    for (std::size_t i = 0; i < v.size(); ++i) out.push_back({k, static_cast<u32>(i)});
}
u64 first_key(const Tiles& t) { for (const auto& [k, v] : t.m) return k; return 0; }
}
