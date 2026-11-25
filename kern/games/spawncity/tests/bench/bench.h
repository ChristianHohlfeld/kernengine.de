// The time a Blueprint's class takes against the C++ it was converted from (tools/blueprint_bench.sh).
//
// Kern's plan (BLUEPRINTS.md, "Der Weg ab hier"): every building block of the language costs in
// the class what the C++ it stands for costs; "the time is measured by an export build for the
// same calls against the source". This is that measurement: for a list of functions of the game,
// the source as the game builds it (src/, f32 where the game has f32) and the class of the
// Blueprint made of it (tools/blueprint_cpp.sh --classes) are called with the same inputs, in the
// game's release build, many rounds in turn; the minimum and the median per call are compared.
//
// A case is two functions without arguments: each goes once through the case's inputs (made by
// the same generator, each side in its own types) and gives a checksum of what came out. Where the
// source computes in the numbers of the language (whole numbers, bits, texts), both checksums must
// be equal; where it computes with f32, the class computes in 64 bit (the language's numbers) and
// the checksums differ - that is not compared here (the tests of tests/converted compare it).
#pragma once

#include <bit>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace bench {

using u64 = std::uint64_t;

/// The inputs of a case: the same numbers on both sides (splitmix64).
struct Gen {
  u64 s;
  explicit Gen(u64 seed) : s(seed) {}
  u64 next() {
    u64 z = (s += 0x9e3779b97f4a7c15ull);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
  }
  /// A number in [a, b), on a grid of 1/1024 (a number an f32 holds exactly, so both sides start equal).
  double range(double a, double b) { return a + static_cast<double>(next() % static_cast<u64>((b - a) * 1024.0)) / 1024.0; }
  int below(int n) { return static_cast<int>(next() % static_cast<u64>(n)); }
};

/// The checksum of what came out.
inline u64 fold(u64 h, u64 v) { return (h ^ v) * 0x100000001b3ull + 0x9e37ull; }
inline u64 fold_number(u64 h, double v) { return fold(h, std::bit_cast<u64>(v)); }

/// Keeps the compiler from computing a value once for every round: the value is used, and memory is
/// what it may have been changed to.
template <class T> inline void keep(const T& v) { asm volatile("" : : "g"(&v) : "memory"); }

/// The bytes of whole numbers as protobuf writes them (a varint: 7 bits a byte, the low bits first).
inline void put_varint(std::vector<unsigned char>& o, u64 v) {
  while (v >= 0x80) {
    o.push_back(static_cast<unsigned char>(v | 0x80));
    v >>= 7;
  }
  o.push_back(static_cast<unsigned char>(v));
}
inline u64 zz(std::int64_t v) { return (static_cast<u64>(v) << 1) ^ static_cast<u64>(v >> 63); }

// ---- the inputs, as numbers both sides read ----
constexpr int kMany = 256;  // (inputs of a case that takes numbers)
/// A geometry of a vector tile: one MoveTo, then a LineTo through `points` points (mvt read_geometry).
inline std::vector<unsigned char> geometry(int points) {
  std::vector<unsigned char> g;
  Gen r(7);
  put_varint(g, (1u << 3) | 1u);
  put_varint(g, zz(r.below(4096)));
  put_varint(g, zz(r.below(4096)));
  put_varint(g, (static_cast<u64>(points) << 3) | 2u);
  for (int i = 0; i < points; ++i) {
    put_varint(g, zz(r.below(200) - 100));
    put_varint(g, zz(r.below(200) - 100));
  }
  return g;
}
inline const char* const kUrl = "https://kernengine.de/kern/games/spawncity/web/index.html?mp=join&room=lobby&net=public";
inline const char* const kHex = "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798";

}  // namespace bench
