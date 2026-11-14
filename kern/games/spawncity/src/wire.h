// The bytes of a message: what SpawnCity sends to the other players and reads from them
// (net_session.cpp, town_sync.cpp, game_worldsync.cpp, game_destruction_sync.cpp,
// game_projsync.cpp, game_toss.cpp). A message is its numbers one after the other, each in the
// bytes C++ holds it in - little-endian, as on x86 and WebAssembly, the machines the game runs on
// (and what the original's DataView writes with littleEndian set).
//
// A number's bytes are its object representation: each kind of number has a struct of that one
// number (a vector and a rotation: of their components), whose bytes are written with
// std::bit_cast and read with std::memcpy - as a Blueprint, Bytes of Struct and Struct of Bytes in
// the layout C++ gives the struct (kern/docs/blueprint_cpp.md, "Die Bytes eines Werts"). A byte is
// itself.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstring>
#include <vector>

#include "common.h"

namespace sc::wire {

/// One number of a message, as its bytes lie there.
struct I8 { kern::i8 v = 0; };
struct U16 { u16 v = 0; };
struct I16 { kern::i16 v = 0; };
struct U32 { u32 v = 0; };
struct U64 { u64 v = 0; };
struct I64 { i64 v = 0; };
struct F32 { f32 v = 0; };
struct F64 { f64 v = 0; };
/// A vector and a rotation: their components, one after the other (as kern::Vec3 and kern::Quat
/// hold them).
struct V3 { f32 x = 0, y = 0, z = 0; };
struct Q4 { f32 x = 0, y = 0, z = 0, w = 0; };

/// The four bytes of a 32-bit float, the low one first.
inline std::array<u8, 4> f32_bytes(f32 v) { return std::bit_cast<std::array<u8, 4>>(F32{v}); }

// ---- writing: each number's bytes go on the end of the message (one growth of the list each, as
// inserting them in one go does)
inline void put_u8(std::vector<u8>& b, u8 v) { b.push_back(v); }
inline void put_i8(std::vector<u8>& b, kern::i8 v) {
  const std::array<u8, 1> x = std::bit_cast<std::array<u8, 1>>(I8{v});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_u16(std::vector<u8>& b, u16 v) {
  const std::array<u8, 2> x = std::bit_cast<std::array<u8, 2>>(U16{v});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_i16(std::vector<u8>& b, kern::i16 v) {
  const std::array<u8, 2> x = std::bit_cast<std::array<u8, 2>>(I16{v});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_u32(std::vector<u8>& b, u32 v) {
  const std::array<u8, 4> x = std::bit_cast<std::array<u8, 4>>(U32{v});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_u64(std::vector<u8>& b, u64 v) {
  const std::array<u8, 8> x = std::bit_cast<std::array<u8, 8>>(U64{v});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_i64(std::vector<u8>& b, i64 v) {
  const std::array<u8, 8> x = std::bit_cast<std::array<u8, 8>>(I64{v});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_f32(std::vector<u8>& b, f32 v) {
  const std::array<u8, 4> x = f32_bytes(v);
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_f64(std::vector<u8>& b, f64 v) {
  const std::array<u8, 8> x = std::bit_cast<std::array<u8, 8>>(F64{v});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_vec3(std::vector<u8>& b, Vec3 v) {
  const std::array<u8, 12> x = std::bit_cast<std::array<u8, 12>>(V3{v.x, v.y, v.z});
  b.insert(b.end(), x.begin(), x.end());
}
inline void put_quat(std::vector<u8>& b, Quat q) {
  const std::array<u8, 16> x = std::bit_cast<std::array<u8, 16>>(Q4{q.x, q.y, q.z, q.w});
  b.insert(b.end(), x.begin(), x.end());
}
/// A list of bytes: its length (u16), then the bytes.
inline void put_bytes(std::vector<u8>& b, const std::vector<u8>& v) {
  put_u16(b, static_cast<u16>(v.size()));
  b.insert(b.end(), v.begin(), v.end());
}

/// Reading a message: where its next number begins, and whether every number read so far was there.
/// The functions are given the message (the same one every time). A number that is not there - the
/// message ends before it - is 0, and the reader is no longer ok; it then stays where it was.
struct Reader {
  std::size_t at = 0;
  bool ok = true;

  u8 get_u8(const std::vector<u8>& b) {
    if (at + 1 > b.size()) { ok = false; return 0; }
    const u8 v = b[at];
    at += 1;
    return v;
  }
  kern::i8 get_i8(const std::vector<u8>& b) {
    if (at + 1 > b.size()) { ok = false; return 0; }
    I8 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 1;
    return x.v;
  }
  u16 get_u16(const std::vector<u8>& b) {
    if (at + 2 > b.size()) { ok = false; return 0; }
    U16 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 2;
    return x.v;
  }
  kern::i16 get_i16(const std::vector<u8>& b) {
    if (at + 2 > b.size()) { ok = false; return 0; }
    I16 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 2;
    return x.v;
  }
  u32 get_u32(const std::vector<u8>& b) {
    if (at + 4 > b.size()) { ok = false; return 0; }
    U32 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 4;
    return x.v;
  }
  u64 get_u64(const std::vector<u8>& b) {
    if (at + 8 > b.size()) { ok = false; return 0; }
    U64 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 8;
    return x.v;
  }
  i64 get_i64(const std::vector<u8>& b) {
    if (at + 8 > b.size()) { ok = false; return 0; }
    I64 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 8;
    return x.v;
  }
  f32 get_f32(const std::vector<u8>& b) {
    if (at + 4 > b.size()) { ok = false; return 0; }
    F32 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 4;
    return x.v;
  }
  f64 get_f64(const std::vector<u8>& b) {
    if (at + 8 > b.size()) { ok = false; return 0; }
    F64 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 8;
    return x.v;
  }
  Vec3 get_vec3(const std::vector<u8>& b) {
    if (at + 12 > b.size()) { ok = false; return {}; }
    V3 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 12;
    return {x.x, x.y, x.z};
  }
  Quat get_quat(const std::vector<u8>& b) {
    if (at + 16 > b.size()) { ok = false; return {}; }
    Q4 x;
    std::memcpy(&x, b.data() + at, sizeof x);
    at += 16;
    return {x.x, x.y, x.z, x.w};
  }
  /// A list of bytes (put_bytes): empty where it is not all there.
  std::vector<u8> get_bytes(const std::vector<u8>& b) {
    const u16 n = get_u16(b);
    if (!ok || at + n > b.size()) { ok = false; return {}; }
    std::vector<u8> v(b.begin() + static_cast<std::ptrdiff_t>(at), b.begin() + static_cast<std::ptrdiff_t>(at + n));
    at += n;
    return v;
  }
  /// Every number was there, and the message has no more.
  bool done(const std::vector<u8>& b) const { return ok && at == b.size(); }
};

}  // namespace sc::wire
