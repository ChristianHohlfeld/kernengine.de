// What the generated tests of tests/converted/ share (tools/cpp_to_blueprint.py --test writes them).
//
// A source file was converted to a Blueprint of the level. Its test holds the source of every
// converted function once more - the text as it stands in the game, compiled with the numbers and
// the mathematics of the Blueprint language (f32 is a 64-bit number there, std::sin is
// kern::math::sin; a class is compiled along, so that its data is 64-bit too). That is what the
// source computes in the Blueprint language. The Blueprint - in the interpreter and as its
// compiled class - must give exactly that: every number, every vector, every field, bit for bit.
// There is no margin.
//
// One thing is not compared: a call in which the source divides by zero or leaves the numbers
// otherwise has no result in C++ to compare with (the Blueprint's arithmetic gives one: x / 0 is
// 0). Such calls are counted.
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cfenv>
#include <csetjmp>
#include <chrono>
#include <limits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <initializer_list>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "../src/blueprint_call.h"
#include "kern/logic/engine_api.h"
#include "../src/common.h"
#include "kern/core/map.h"
#include "kern/logic/nodes.h"
#include "cost.h"
#include "kern/game/runtime.h"
#include "kern/kern.h"
#include "rule_blueprint.h"
#include "test_util.h"

namespace sc::converted {

using Value = kern::logic::Value;

/// A 64-bit bit pattern of the program (its u64, i64) as the test hands it to a Blueprint: the language's
/// Int64 (Value::int64). In C++ a u64 and a size_t are one type; the program's spelling tells them apart,
/// and the generated test says which is which (shaped, below).
struct Whole {
  unsigned long long v = 0;
  bool operator==(const Whole&) const = default;
  kern::u64 hash() const { return kern::KeyHash<unsigned long long>{}(v); }   // (a key of a map, as the u64 it is)
};
struct SignedWhole {};

// ---- the numbers of the Blueprint language, for the source a test holds ----
// A number is 64-bit. A vector holds its components in 32 bit; read, a component is a number like any
// other, and what is written into one is brought to 32 bit. What vectors do as a whole (add, dot, cross,
// length, normalize ...) is what the nodes of the language do: the engine's own functions.
namespace wide {
using f32 = double;
using f64 = double;
using kern::i16;
using kern::i32;
using kern::i64;
using kern::i8;
using kern::u16;
using kern::u32;
using kern::u64;
using kern::u8;

/// A component of a vector.
struct Num32 {
  float v = 0;
  constexpr Num32() = default;
  // (explicit: read, a component is a number - `c ? p.x : 0.5` is a number, as in the Blueprint; what is
  // written into one is brought to 32 bit)
  constexpr explicit Num32(double x) : v(static_cast<float>(x)) {}
  constexpr Num32& operator=(double x) { v = static_cast<float>(x); return *this; }
  constexpr operator double() const { return static_cast<double>(v); }
  constexpr Num32& operator+=(double x) { return *this = Num32(static_cast<double>(v) + x); }
  constexpr Num32& operator-=(double x) { return *this = Num32(static_cast<double>(v) - x); }
  constexpr Num32& operator*=(double x) { return *this = Num32(static_cast<double>(v) * x); }
  constexpr Num32& operator/=(double x) { return *this = Num32(static_cast<double>(v) / x); }
};

struct Vec2 {
  Num32 x, y;
  constexpr Vec2() = default;
  constexpr Vec2(double x_, double y_) : x(x_), y(y_) {}
  constexpr Vec2(kern::Vec2 v) : x(v.x), y(v.y) {}
  constexpr operator kern::Vec2() const { return {x.v, y.v}; }
  constexpr Vec2 operator+(Vec2 o) const { return kern::Vec2(*this) + kern::Vec2(o); }
  constexpr Vec2 operator-(Vec2 o) const { return kern::Vec2(*this) - kern::Vec2(o); }
  constexpr Vec2 operator*(double s) const { return kern::Vec2(*this) * static_cast<float>(s); }
  constexpr Vec2& operator+=(Vec2 o) { return *this = *this + o; }
};
constexpr double dot(Vec2 a, Vec2 b) { return kern::dot(kern::Vec2(a), kern::Vec2(b)); }
constexpr double cross(Vec2 a, Vec2 b) { return kern::cross(kern::Vec2(a), kern::Vec2(b)); }
inline double length(Vec2 a) { return kern::length(kern::Vec2(a)); }

struct Vec3 {
  Num32 x, y, z;
  constexpr Vec3() = default;
  constexpr Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
  constexpr Vec3(kern::Vec3 v) : x(v.x), y(v.y), z(v.z) {}
  constexpr operator kern::Vec3() const { return {x.v, y.v, z.v}; }
  constexpr double operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
  constexpr Num32& operator[](int i) { return i == 0 ? x : (i == 1 ? y : z); }
  constexpr Vec3 operator+(Vec3 o) const { return kern::Vec3(*this) + kern::Vec3(o); }
  constexpr Vec3 operator-(Vec3 o) const { return kern::Vec3(*this) - kern::Vec3(o); }
  constexpr Vec3 operator-() const { return -kern::Vec3(*this); }
  constexpr Vec3 operator*(double s) const { return kern::Vec3(*this) * static_cast<float>(s); }
  constexpr Vec3 operator*(Vec3 o) const { return kern::Vec3(*this) * kern::Vec3(o); }
  constexpr Vec3 operator/(double s) const { return kern::Vec3(*this) / static_cast<float>(s); }
  constexpr Vec3& operator+=(Vec3 o) { return *this = *this + o; }
  constexpr Vec3& operator-=(Vec3 o) { return *this = *this - o; }
  constexpr Vec3& operator*=(double s) { return *this = *this * s; }
  constexpr bool operator==(const Vec3& o) const { return kern::Vec3(*this) == kern::Vec3(o); }
};
constexpr double dot(Vec3 a, Vec3 b) { return kern::dot(kern::Vec3(a), kern::Vec3(b)); }
constexpr Vec3 cross(Vec3 a, Vec3 b) { return kern::cross(kern::Vec3(a), kern::Vec3(b)); }
inline double length(Vec3 a) { return kern::length(kern::Vec3(a)); }
constexpr double length_sq(Vec3 a) { return kern::length_sq(kern::Vec3(a)); }
inline double distance(Vec3 a, Vec3 b) { return kern::distance(kern::Vec3(a), kern::Vec3(b)); }
constexpr double distance_sq(Vec3 a, Vec3 b) { return kern::distance_sq(kern::Vec3(a), kern::Vec3(b)); }
inline bool finite(Vec3 a) { return kern::finite(kern::Vec3(a)); }
constexpr Vec3 lerp(Vec3 a, Vec3 b, double t) { return kern::lerp(kern::Vec3(a), kern::Vec3(b), static_cast<float>(t)); }
constexpr Vec3 vmin(Vec3 a, Vec3 b) { return kern::vmin(kern::Vec3(a), kern::Vec3(b)); }
constexpr Vec3 vmax(Vec3 a, Vec3 b) { return kern::vmax(kern::Vec3(a), kern::Vec3(b)); }
inline Vec3 normalize(Vec3 a, Vec3 fallback = {0, 0, 1}) { return kern::normalize(kern::Vec3(a), kern::Vec3(fallback)); }
constexpr Vec3 hex_color(kern::u32 h) { return kern::hex_color(h); }   // (a colour's components are 32-bit, as a vector's)
/// A view of a list (kern::Span): the engine's own - of the test's values.
template <class T> using Span = kern::Span<T>;

// the functions of numbers a formula does not have itself, as the converter writes them
inline double hypot_bp(double a, double b) { return kern::math::sqrt((a * a) + (b * b)); }
inline double ldexp_bp(double x, double n) { return (x * kern::math::pow(2.0, n)); }
inline double tanh_bp(double x) { return kern::math::div(kern::math::exp(2.0 * x) - 1.0, kern::math::exp(2.0 * x) + 1.0); }
inline double sinh_bp(double x) { return ((kern::math::exp(x) - kern::math::exp(-x)) * 0.5); }
inline double cosh_bp(double x) { return ((kern::math::exp(x) + kern::math::exp(-x)) * 0.5); }
inline double asinh_bp(double x) { return kern::math::log(x + kern::math::sqrt((x * x) + 1.0)); }
inline double log2_bp(double x) { return kern::math::div(kern::math::log(x), 0.6931471805599453); }
inline double log10_bp(double x) { return kern::math::div(kern::math::log(x), 2.302585092994046); }
inline double cbrt_bp(double x) { return (kern::math::sign(x) * kern::math::pow(kern::math::abs(x), 0.3333333333333333)); }

// A map and a set (kern::Map, kern::Set - the program's): the language's Map and Set are these very
// containers, in the order the keys came (kern/core/map.h). The test's source holds them as they are.
using kern::Map;
using kern::Set;
}  // namespace wide

// ---- the bytes of a value, as the program lays it out (Bytes of Struct, Struct of Bytes) ----
// The test's source holds a 32-bit float of the program as a 64-bit number (wide::f32). Where the program
// takes the bytes of a struct with one (std::bit_cast to bytes, memcpy from bytes, its address handed to a
// pointer to bytes), they are the bytes of the program's struct: each field at the place the program's
// layout gives it, a float as its 32 bits - the number rounded as C++ converts a double to a float, read
// back as C++ widens a float - and every other field as it is. The generated test writes the fields out
// (TestWriter.guard); these functions are the bytes of one field.
namespace layout {
/// A field that is a float in the program: the 32 bits of the number.
inline void single_to(kern::u8* at, double v) {
  const float f = static_cast<float>(v);
  std::memcpy(at, &f, sizeof f);
}
inline void single_from(double& v, const kern::u8* at) {
  float f;
  std::memcpy(&f, at, sizeof f);
  v = static_cast<double>(f);
}
/// std::bit_cast between a float of the program and its 32 bits: of the float the number is, and the float
/// 32 bits are - as a 64-bit number, as the test holds a float.
inline kern::u32 float_bits(double v) { return std::bit_cast<kern::u32>(static_cast<float>(v)); }
inline double float_of_bits(kern::u32 b) { return static_cast<double>(std::bit_cast<float>(b)); }
/// Any other field: its own bytes (the test holds it as the program does).
template <class T> void raw_to(kern::u8* at, const T& v) { std::memcpy(at, &v, sizeof v); }
template <class T> void raw_from(T& v, const kern::u8* at) { std::memcpy(&v, at, sizeof v); }
/// The N bytes of a value (`fields` writes them), and a value set from bytes (`fields` reads them).
template <std::size_t N, class T, class F> std::array<kern::u8, N> bytes_of(const T& v, F fields) {
  std::array<kern::u8, N> out{};
  fields(v, out.data());
  return out;
}
template <class T, class F> void of_bytes(T& v, const void* bytes, F fields) { fields(v, static_cast<const kern::u8*>(bytes)); }
/// std::memcpy of a value of such a struct into bytes: its bytes as the program lays it out (`fields` writes them);
/// gives where they went, as std::memcpy does.
template <class T, class F> void* to_bytes(void* bytes, const T& v, F fields) {
  fields(v, static_cast<kern::u8*>(bytes));
  return bytes;
}
/// std::memcpy / std::memmove between lists of numbers of the program of which one holds floats (Copy
/// Bytes; the test holds such a float as a 64-bit number - ToSingle, FromSingle): the `count` bytes the
/// program copies, a float as its 32 bits and every other number as it is, as std::memmove copies them -
/// in pieces of 64 bytes (whole entries of every number), each read before it is written, the last first
/// where the bytes move up. An entry written in part keeps its other bytes.
template <bool ToSingle, bool FromSingle, class D, class S> void copy_bytes(D* to, const S* from, std::size_t count) {
  constexpr std::size_t kTo = ToSingle ? 4 : sizeof(D), kFrom = FromSingle ? 4 : sizeof(S), kPiece = 64;
  static_assert(kPiece % kTo == 0 && kPiece % kFrom == 0 && (!ToSingle || std::is_same_v<D, double>) && (!FromSingle || std::is_same_v<S, double>));
  const std::size_t pieces = (count + kPiece - 1) / kPiece;
  const bool up = std::less<const void*>{}(static_cast<const void*>(from), static_cast<const void*>(to));
  for (std::size_t p = 0; p < pieces; ++p) {
    const std::size_t at = (up ? pieces - 1 - p : p) * kPiece, n = std::min(kPiece, count - at);
    kern::u8 bytes[kPiece];
    for (std::size_t i = at / kFrom; i * kFrom < at + n; ++i) {   // (the program's bytes of the entries read)
      kern::u8 one[8];
      if constexpr (FromSingle) {
        const float f = static_cast<float>(from[i]);
        std::memcpy(one, &f, sizeof f);
      } else {
        std::memcpy(one, &from[i], sizeof(S));
      }
      for (std::size_t b = 0; b < kFrom; ++b)
        if (i * kFrom + b >= at && i * kFrom + b < at + n) bytes[i * kFrom + b - at] = one[b];
    }
    for (std::size_t j = at / kTo; j * kTo < at + n; ++j) {   // (into the entries written)
      kern::u8 one[8];
      if constexpr (ToSingle) {
        const float f = static_cast<float>(to[j]);
        std::memcpy(one, &f, sizeof f);
      } else {
        std::memcpy(one, &to[j], sizeof(D));
      }
      for (std::size_t b = 0; b < kTo; ++b)
        if (j * kTo + b >= at && j * kTo + b < at + n) one[b] = bytes[j * kTo + b - at];
      if constexpr (ToSingle) {
        float f;
        std::memcpy(&f, one, sizeof f);
        to[j] = static_cast<double>(f);
      } else {
        std::memcpy(&to[j], one, sizeof(D));
      }
    }
  }
}
/// std::memset into a list of numbers of the program (ToSingle: of floats - the test holds such a float as a
/// 64-bit number): the `count` bytes the program sets, a float as its 32 bits. An entry set in part keeps its
/// other bytes.
template <bool ToSingle, class D> void* set_bytes(D* to, int value, std::size_t count) {
  constexpr std::size_t kTo = ToSingle ? 4 : sizeof(D);
  static_assert(!ToSingle || std::is_same_v<D, double>);
  for (std::size_t j = 0; j * kTo < count; ++j) {
    kern::u8 one[8];
    if constexpr (ToSingle) {
      const float f = static_cast<float>(to[j]);
      std::memcpy(one, &f, sizeof f);
    } else {
      std::memcpy(one, &to[j], sizeof(D));
    }
    for (std::size_t b = 0; b < kTo && j * kTo + b < count; ++b) one[b] = static_cast<kern::u8>(value);
    if constexpr (ToSingle) {
      float f;
      std::memcpy(&f, one, sizeof f);
      to[j] = static_cast<double>(f);
    } else {
      std::memcpy(&to[j], one, sizeof(D));
    }
  }
  return to;
}
}  // namespace layout

/// A value of the source as the Blueprint holds it, where the two differ by more than their numbers: a
/// 64-bit bit pattern is a Whole - also in a list, a pair, a map or a set. S is the type as the program
/// spells it, with Whole where it says u64 or i64.
template <class S> struct Shape {
  template <class T> static const T& of(const T& v) { return v; }
};
template <> struct Shape<Whole> {
  template <class T> static Whole of(T v) { return {static_cast<unsigned long long>(v)}; }
};
template <class S, class T> decltype(auto) shaped(const T& v) { return Shape<S>::of(v); }   // (a value that needs no shape: as it is, not a copy)
template <class S, class T> using Shaped = std::decay_t<decltype(Shape<S>::of(std::declval<const T&>()))>;
template <class S> struct Shape<std::vector<S>> {
  template <class T> static auto of(const std::vector<T>& v) {
    std::vector<Shaped<S, T>> out;
    for (const T& x : v) out.push_back(Shape<S>::of(x));
    return out;
  }
};
template <class S, std::size_t N> struct Shape<std::array<S, N>> {   // (a list of a fixed length: a list, as the Blueprint gives it)
  template <class T> static auto of(const std::array<T, N>& v) {
    std::vector<Shaped<S, T>> out;
    for (const T& x : v) out.push_back(Shape<S>::of(x));
    return out;
  }
};
template <class A, class B> struct Shape<std::pair<A, B>> {
  template <class X, class Y> static auto of(const std::pair<X, Y>& p) { return std::pair<Shaped<A, X>, Shaped<B, Y>>(Shape<A>::of(p.first), Shape<B>::of(p.second)); }
};
/// A map or a set seen with its keys and values as the Blueprint holds them (a view: what it holds may be
/// a value that is not to be copied - std::unique_ptr).
template <class SK, class SV, class M> struct ShapedMap {
  const M& m;
};
template <class SK, class M> struct ShapedSet {
  const M& s;
};
template <class SK, class SV> struct Shape<kern::Map<SK, SV>> {
  template <class M> static ShapedMap<SK, SV, M> of(const M& m) { return {m}; }
};
template <class SK> struct Shape<kern::Set<SK>> {
  template <class M> static ShapedSet<SK, M> of(const M& s) { return {s}; }
};

/// The arguments a function is tried with: the same in every run.
struct Draw {
  Rng rng{0x5eed1234u};
  u32 below(u32 n) { return n ? rng.next() % n : 0; }
  f64 number() {
    switch (below(10)) {
      case 0: return 0.0;
      case 1: return below(2) ? 1.0 : -1.0;
      case 2:
      case 3: return (static_cast<int>(below(401)) - 200) / 100.0;  // (decimals of two digits: a threshold is met exactly)
      case 4: return static_cast<f64>(rng.range(-1.0f, 1.0f));
      case 5: return unit() * 2.0 - 1.0;   // (all the digits a 64-bit number has)
      case 6: return unit();
      case 7: return static_cast<f64>(rng.range(-50.0f, 50.0f));
      case 8: return static_cast<f64>(rng.range(0.0f, 1000.0f));
      default: return (static_cast<int>(below(41)) - 20) / 2.0;
    }
  }
  f64 unit() {
    const f64 high = rng.next(), low = rng.next();
    return (high + low / 4294967296.0) / 4294967296.0;
  }
  int whole() { return below(4) ? static_cast<int>(below(12)) - 2 : static_cast<int>(below(200)) - 20; }
  bool flag() { return below(2) != 0; }
  Vec3 vector() { return {static_cast<f32>(number()), static_cast<f32>(number()), static_cast<f32>(number())}; }
  Vec2 vector2() { return {static_cast<f32>(number()), static_cast<f32>(number())}; }
  /// 64 bits: any at all, and now and then a small number - or one drawn before (a key that is looked
  /// up in a map is, now and then, a key the map has).
  unsigned long long bits() {
    if (drawn > 0 && below(3) == 0) return before[below(drawn < 8 ? drawn : 8)];
    unsigned long long x = (static_cast<unsigned long long>(rng.next()) << 32) | rng.next();
    switch (below(4)) {
      case 0: x >>= 58; break;
      case 1: x >>= 32; break;
      default: break;
    }
    before[drawn++ % 8] = x;
    return x;
  }
  unsigned long long before[8] = {};
  u32 drawn = 0;
  const char* c_word() {
    static const char* const words[] = {"", "a", "Abc", "x1", "hello world", "north", "7", "A-B"};
    return words[below(8)];
  }
  std::string word() { return c_word(); }
  /// A word of the texts a function says itself (what it compares a text it is given with: "settings reset",
  /// "gps ") with a tail after it - nothing, numbers as C reads them, a word, the end of a range -, now and
  /// then one of the plain words: what the function does with a text it knows is compared too.
  std::string said_word(std::initializer_list<const char*> said) {
    static const char* const tails[] = {"", "", " ", "1", " 1", " 0", " -2.5", " 0x1F", " 7 8 9", " 1.5 -3", ";a b", " 1e3 2", " x", " 99999999999", " nan", "  3:30",
                                        " a 1;b 2", " -0", " 52.5 13.4 Berlin", " 255 0 1"};
    if (said.size() == 0 || below(4) == 0) return word();
    std::string w = said.begin()[below(static_cast<u32>(said.size()))];
    w += tails[below(static_cast<u32>(sizeof tails / sizeof *tails))];
    return w;
  }
  /// (one as a C string: kept as long as the test runs)
  const char* said_c_word(std::initializer_list<const char*> said) {
    static std::deque<std::string> kept;
    kept.push_back(said_word(said));
    return kept.back().c_str();
  }
};

/// A list of up to four drawn entries; one of numbers is, one time in five, a long one (a function that
/// reads its list at fixed places - the cells of a grid - finds them there).
template <class Make> auto draw_list(Draw& draw, Make make) {
  std::vector<decltype(make())> out;
  u32 n = draw.below(5);
  if constexpr (std::is_arithmetic_v<decltype(make())>) {
    if (draw.below(5) == 0) n = 300 + draw.below(300);
  }
  // (as long as it is, no room to spare: a list the class is given from a Value is that, and a function that
  // appends - put_i32 into the bytes - asks for memory where the class does; spare room on the source's side
  // only would hide that ask - a cost line that is the test's, not the class's)
  out.reserve(n);
  for (; n > 0; --n) out.push_back(make());
  return out;
}

/// What may be there or not (a std::unique_ptr or std::optional of a value - in the Blueprint a list of no
/// entry or one): there one time in two.
template <class P, class Make> P draw_maybe(Draw& draw, Make make) {
  if (!draw.flag()) return P{};
  if constexpr (requires { typename P::element_type; }) {
    return std::make_unique<typename P::element_type>(make());
  } else {
    return P(make());
  }
}
/// A list the source writes in braces (std::initializer_list): up to four entries, as such a list is
/// written - handed to the source by `listed`.
template <class Make> auto draw_few(Draw& draw, Make make) {
  std::vector<decltype(make())> out;
  for (u32 n = draw.below(5); n > 0; --n) out.push_back(make());
  return out;
}
/// A list of a fixed length (std::array): every entry drawn.
template <class A, class Make> A draw_array(Make make) {
  A out{};
  for (auto& e : out) e = make();
  return out;
}
/// `f` called with the drawn list as a list in braces (it lives for the call, as one written there does).
template <class T, class F> decltype(auto) listed(const std::vector<T>& v, F f) {
  switch (v.size()) {
    case 0: return f(std::initializer_list<T>{});
    case 1: return f({v[0]});
    case 2: return f({v[0], v[1]});
    case 3: return f({v[0], v[1], v[2]});
    case 4: return f({v[0], v[1], v[2], v[3]});
    default: break;
  }
  std::abort();   // (draw_few draws no more)
}

/// A C array of any rank (T[3][2]: 3 entries of T[2]) as the Blueprint has it - a list, of lists - and
/// back (as far as both go); `fill_array` draws every entry.
template <class T> auto array_list(const T& a) {
  if constexpr (std::is_array_v<T>) {
    std::vector<decltype(array_list(a[0]))> out;
    for (const auto& e : a) out.push_back(array_list(e));
    return out;
  } else {
    return a;
  }
}
template <class T, class L> void array_from(T& a, const L& items) {
  if constexpr (std::is_array_v<T>) {
    for (std::size_t k = 0; k < items.size() && k < std::size(a); ++k) array_from(a[k], items[k]);
  } else {
    a = items;
  }
}
template <class T, class Make> void fill_array(T& a, Make make) {
  if constexpr (std::is_array_v<T>) {
    for (auto& e : a) fill_array(e, make);
  } else {
    a = make();
  }
}

/// A map of up to four drawn rows, a set of up to four drawn keys.
template <class M, class Key, class Held> M draw_map(Draw& draw, Key key, Held held) {
  M out;
  for (u32 n = draw.below(5); n > 0; --n) {
    auto k = key();
    out.insert_or_assign(std::move(k), held());
  }
  return out;
}
template <class S, class Key> S draw_set(Draw& draw, Key key) {
  S out;
  for (u32 n = draw.below(5); n > 0; --n) out.insert(key());
  return out;
}

// ---- the engine in a test ----
// A converted function that calls the engine calls a node of the engine in its Blueprint (kern/logic/
// engine_api.h). In a test both go to a stand-in that answers from the call itself and keeps every call
// (or, SC_ENGINE=real, to the engine itself, each side with objects of its own - sc::real_engine):
// the source through the stand-in classes the test has for the engine's (they derive from EngineObject -
// an object of the engine is a handle in the Blueprint), the Blueprint through its host. The two must
// have asked the same, call for call (Tally::take).
// An object the source makes (a stand-in's constructor: the engine's <class>.make) is its own: its
// destructor releases it (<class>.release) - and so is one the test puts where the source holds one as a
// value or owns one (`owned`: as the Blueprint has a handle there, it is released where the Blueprint
// releases it). Ownership is the object's, as in C++: moved, it goes with it (what is moved from owns nothing
// any more - the test handing a value to the source and back, the source moving what it made); a copy names
// the same object and owns nothing (a handle handed on). The test's own objects (engine_object), and what
// it makes quietly (what a stand-in gives back, a value it draws - `quiet`), are made by no one: no make, no
// release.
struct EngineObject {
  long long kern_handle = 0;
  bool kern_owned = false;
  EngineObject() = default;
  /// One the source makes with what the engine's class is made with, where it has no constructor of its
  /// own (`kern::LightPool lights{kMaxPointLights, 96}`, a field of a class compiled along): the stand-in
  /// answers what the source asks it, whatever it was made with.
  template <class A, class... B>
    requires(!std::is_base_of_v<EngineObject, std::remove_cvref_t<A>>)
  explicit EngineObject(const A&, const B&...) {}
  EngineObject(const EngineObject& o) : kern_handle(o.kern_handle) {}
  EngineObject(EngineObject&& o) noexcept : kern_handle(o.kern_handle), kern_owned(o.kern_owned) { o.kern_owned = false; }
  /// (set from a copy: the place stays as much the owner of what it holds as it was)
  EngineObject& operator=(const EngineObject& o) {
    kern_handle = o.kern_handle;
    return *this;
  }
  EngineObject& operator=(EngineObject&& o) noexcept {
    kern_handle = o.kern_handle;
    kern_owned = o.kern_owned;
    o.kern_owned = false;
    return *this;
  }
};
/// While the test makes its own objects of a class (engine_object, quiet): no make or release of the source.
inline bool g_keeping = false;
struct Quiet {
  const bool was = g_keeping;
  Quiet() { g_keeping = true; }
  ~Quiet() { g_keeping = was; }
  Quiet(const Quiet&) = delete;
  Quiet& operator=(const Quiet&) = delete;
};
/// A value the test makes for itself (what a stand-in gives back, an entry it takes into a list): what it
/// holds of the engine is made by no one.
template <class T> T quiet() {
  const Quiet q;
  return T{};
}
/// An object of the engine the test puts where the source holds one as a value or owns one: the source's
/// (see EngineObject).
template <class T>
  requires std::is_base_of_v<EngineObject, T>
T owned(const T& object) {
  T out(object);
  out.kern_owned = true;
  return out;
}
/// The one object of a state as the program makes it (static storage: all of it zero, then its constructor).
template <class T> std::unique_ptr<T> made_static() {
  void* at = ::operator new(sizeof(T));
  std::memset(at, 0, sizeof(T));
  return std::unique_ptr<T>(new (at) T());
}
/// That object as it is before its constructor makes anything of the engine: the variables of its Blueprint
/// as they begin (their defaults, and what C++ makes of them that is not the engine's).
template <class T> std::unique_ptr<T> quiet_static() {
  const Quiet q;
  return made_static<T>();
}
/// One of the few objects of an engine class a test keeps, by its handle.
template <class T> T* engine_object(long long handle) {
  static T* const kept = [] {
    g_keeping = true;
    T* k = new T[8];
    g_keeping = false;
    return k;
  }();
  T* object = &kept[static_cast<unsigned long long>(handle) % 8];
  object->kern_handle = static_cast<long long>(static_cast<unsigned long long>(handle) % 8);
  return object;
}
/// What the source asked the engine in the call being compared - and, where the test runs on the engine
/// itself (SC_ENGINE=real: sc::real_engine), the source's objects of the engine in that call.
inline kern::logic::EngineStandIn g_source_engine;
inline kern::logic::EngineObjects g_source_objects;
inline std::vector<Value> engine_call(const char* function, const std::vector<Value>& in) {
  std::vector<Value> out;
  if (const kern::logic::EngineFunction* f = kern::logic::engine_function(function))
    for (const kern::logic::EnginePin& p : f->outputs) out.push_back(Value::zero(p.type));
  test_engine(g_source_engine, g_source_objects, function, in, out);
  return out;
}
/// What a stand-in's constructor is given by the engine: the handle of the object it made, its own.
inline void take_made(EngineObject& object, const Value& handle) {
  object.kern_handle = handle.as_int();
  object.kern_owned = true;
}
inline Value arg(const EngineObject* object) { return Value::integer(object ? object->kern_handle : 0); }
template <class T>
  requires std::is_base_of_v<EngineObject, T>
Value arg(const std::unique_ptr<T>& object) { return arg(static_cast<const EngineObject*>(object.get())); }
inline Value arg(const EngineObject& object) { return Value::integer(object.kern_handle); }

// ---- a C++ value as the Blueprint gets it ----
template <class T> inline constexpr bool is_list = false;
template <class T> inline constexpr bool is_list<std::vector<T>> = true;
template <class T, std::size_t N> inline constexpr bool is_list<std::array<T, N>> = true;   // (a list of a fixed length: a list)
/// What may be there or not (std::unique_ptr of a value of the program, std::optional): in the Blueprint a list
/// of no entry or one - in a list, a list inside a list: a struct that holds it (see arg_one).
template <class T> inline constexpr bool is_box = false;
template <class T> inline constexpr bool is_box<std::unique_ptr<T>> = !std::is_base_of_v<EngineObject, T>;
template <class T> inline constexpr bool is_box<std::optional<T>> = true;
// (a callback of the source - std::function, or a struct of the engine that is one: fn + user, as kern::HeightSampler -: declared
// before the templates that use them, as the lists are below - their types stand in std and kern, not here)
template <class S> concept FnAndUser = requires(const S& s) { s.fn; s.user; } && std::is_pointer_v<decltype(S::fn)>;
template <class R, class... A> Value arg(const std::function<R(A...)>& f);
template <FnAndUser S> Value arg(const S& s);
template <class R, class... A> bool same(const std::function<R(A...)>& want, const Value& got);
template <FnAndUser S> bool same(const S& want, const Value& got);
template <FnAndUser S> bool defined(const S&);
template <FnAndUser S> std::string text(const S& s);
template <class R, class... A> void take(std::function<R(A...)>& to, const Value& v);
template <class R, class... A> bool defined(const std::function<R(A...)>&);
template <class R, class... A> std::string text(const std::function<R(A...)>& f);
template <FnAndUser S> void take(S& to, const Value& v);
template <class T> Value arg(const std::vector<T>& v);
template <class T> bool same(const std::vector<T>& want, const Value& got);
template <class T> bool defined(const std::vector<T>& v);
template <class T> std::string text(const std::vector<T>& v);
// (what a list, a pair, a list of a fixed length and what may be there or not hold: declared before the
// templates that use them - their entries' types may be the test's own, of another namespace)
template <class T, std::size_t N> Value arg(const std::array<T, N>& a);
template <class T, std::size_t N> bool same(const std::array<T, N>& want, const Value& got);
template <class T, std::size_t N> bool defined(const std::array<T, N>& a);
template <class T, std::size_t N> std::string text(const std::array<T, N>& a);
template <class T> Value arg(kern::Span<T> s);
template <class T> bool same(kern::Span<T> want, const Value& got);
template <class T> bool defined(kern::Span<T> s);
template <class T> std::string text(kern::Span<T> s);
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
Value arg(const std::unique_ptr<T>& p);
template <class T> Value arg(const std::optional<T>& p);
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
bool same(const std::unique_ptr<T>& want, const Value& got);
template <class T> bool same(const std::optional<T>& want, const Value& got);
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
bool defined(const std::unique_ptr<T>& p);
template <class T> bool defined(const std::optional<T>& p);
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
std::string text(const std::unique_ptr<T>& p);
template <class T> std::string text(const std::optional<T>& p);
template <class SK, class SV, class M> Value arg(const ShapedMap<SK, SV, M>& w);
template <class SK, class M> Value arg(const ShapedSet<SK, M>& w);
template <class SK, class SV, class M> bool same(const ShapedMap<SK, SV, M>& want, const Value& got);
template <class SK, class M> bool same(const ShapedSet<SK, M>& want, const Value& got);
template <class SK, class SV, class M> bool defined(const ShapedMap<SK, SV, M>& w);
template <class SK, class M> bool defined(const ShapedSet<SK, M>& w);
template <class K, class V, class H> Value arg(const kern::Map<K, V, H>& m);
template <class K, class H> Value arg(const kern::Set<K, H>& s);
template <class K, class V, class H> bool same(const kern::Map<K, V, H>& want, const Value& got);
template <class K, class H> bool same(const kern::Set<K, H>& want, const Value& got);
template <class K, class V, class H> bool defined(const kern::Map<K, V, H>& m);
template <class K, class H> bool defined(const kern::Set<K, H>& s);
template <class K, class V, class H> std::string text(const kern::Map<K, V, H>& m);
template <class K, class H> std::string text(const kern::Set<K, H>& s);
template <class A, class B> Value arg(const std::pair<A, B>& p);
template <class A, class B> bool same(const std::pair<A, B>& want, const Value& got);
template <class A, class B> bool defined(const std::pair<A, B>& p);
template <class A, class B> std::string text(const std::pair<A, B>& p);
/// Entries the test keeps for a view to see (a drawn view, one the engine's stand-in gave): kept to the end.
template <class U> wide::Span<const U> kept_span(std::vector<U> entries) {
  static std::deque<std::vector<U>> kept;
  kept.push_back(std::move(entries));
  return {kept.back().data(), kept.back().size()};
}
/// The same for a view through which the entries are changed.
template <class U> wide::Span<U> kept_span_changed(std::vector<U> entries) {
  static std::deque<std::vector<U>> kept;
  kept.push_back(std::move(entries));
  return {kept.back().data(), kept.back().size()};
}

inline Value arg(f64 x) { return Value::number(x); }
inline Value arg(f32 x) { return Value::number(static_cast<f64>(x)); }
inline Value arg(bool x) { return Value::boolean(x); }
inline Value arg(Whole w) { return Value::int64(w.v); }
inline Value arg(Vec3 v) { return Value::vector(v); }
inline Value arg(Vec2 v) { return Value::vector({v.x, v.y, 0}); }
inline Value arg(wide::Vec3 v) { return arg(kern::Vec3(v)); }
inline Value arg(wide::Vec2 v) { return arg(kern::Vec2(v)); }
inline Value arg(const std::string& x) { return Value::text(x); }
inline Value arg(const char* x) { return Value::text(x ? x : ""); }
inline Value arg(std::string_view x) { return Value::text(std::string(x)); }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
Value arg(T x) { return Value::integer(static_cast<kern::i64>(x)); }
/// The values of an enum, as the Blueprint has them: by their place in the enum (0, 1, 2 ...), not by the number C++ gives
/// them. Where those differ (`enum class ClientState : i8 { denied = -3, ... }`), the test says the numbers in their order
/// (converted::number_enum, written by the test); otherwise a value's place is its number.
template <class E> struct EnumNumbers { static inline std::vector<kern::i64> numbers; };
template <class E> bool number_enum(std::initializer_list<kern::i64> numbers) {
  EnumNumbers<E>::numbers.assign(numbers);
  return true;
}
/// The place of a value of the source (-1: a number that is no value of the enum - the Blueprint has none such).
template <class E> kern::i64 enum_place(E e) {
  const std::vector<kern::i64>& n = EnumNumbers<E>::numbers;
  if (n.empty()) return static_cast<kern::i64>(e);
  for (std::size_t k = 0; k < n.size(); ++k)
    if (n[k] == static_cast<kern::i64>(e)) return static_cast<kern::i64>(k);
  return -1;
}
/// The value at a place (a place the enum does not have: that number, as the source would read it).
template <class E> E enum_value(kern::i64 place) {
  const std::vector<kern::i64>& n = EnumNumbers<E>::numbers;
  if (!n.empty() && place >= 0 && place < static_cast<kern::i64>(n.size())) return static_cast<E>(n[static_cast<std::size_t>(place)]);
  return static_cast<E>(place);
}
template <class E>
  requires std::is_enum_v<E>
Value arg(E e) { return Value::enumeration(enum_place(e)); }
/// A pair - a row of a map: the struct of its two parts.
template <class A, class B> Value arg(const std::pair<A, B>& p) { return Value::structure({arg(p.first), arg(p.second)}); }
template <class T> Value arg(const std::vector<T>& v) {
  if constexpr (std::is_same_v<T, bool>) {
    return Value::of(std::vector<bool>(v.begin(), v.end()));
  } else if constexpr (std::is_floating_point_v<T>) {
    return Value::of(std::vector<f64>(v.begin(), v.end()));
  } else if constexpr (std::is_integral_v<T>) {
    return Value::of(std::vector<kern::i64>(v.begin(), v.end()));
  } else if constexpr (std::is_enum_v<T>) {
    std::vector<kern::i64> out;
    for (const T& x : v) out.push_back(enum_place(x));
    return Value::enumerations(std::move(out));
  } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
    return Value::of(std::vector<std::string>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec3> || std::is_same_v<T, wide::Vec3>) {
    return Value::of(std::vector<Vec3>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec2> || std::is_same_v<T, wide::Vec2>) {
    std::vector<Vec3> out;
    for (const kern::Vec2 x : v) out.push_back({x.x, x.y, 0});
    return Value::of(std::move(out));
  } else if constexpr (std::is_same_v<T, Whole>) {   // (64-bit patterns: a List<Int64>, as the Blueprint holds it)
    std::vector<kern::logic::Int64> out;
    out.reserve(v.size());
    for (const Whole& x : v) out.push_back(kern::logic::Int64{x.v});
    return Value::of(std::move(out));
  } else if constexpr (is_list<T> || is_box<T>) {   // (a list inside a list: a struct that holds it, as the Blueprint has it)
    std::vector<Value> out;
    for (const T& x : v) out.push_back(Value::structure({arg(x)}));
    return Value::structures(std::move(out));
  } else {
    std::vector<Value> out;
    for (const T& x : v) out.push_back(arg(x));
    return Value::structures(std::move(out));
  }
}

// ---- where a result differs: the first place, named (only when a call differs - Tally::take compares again
// with g_where_on; a comparison gives the same with it or without: these only write down where it failed) ----
inline bool g_where_on = false;
inline std::string g_where, g_where_leaf;
/// A step into a value (a field `.x`, an entry `[3]`): where an inner place differed, its step goes before it.
inline bool at_place(bool ok, const std::string& step) {
  if (!ok && g_where_on) g_where = step + g_where;
  return ok;
}
/// The place itself that differs: what the source has there and what the Blueprint has (the first one only).
template <class W, class G> bool differ_at(W&& want, G&& got) {
  if (g_where_on && g_where_leaf.empty()) g_where_leaf = "the source " + want() + ", the Blueprint " + got();
  return false;
}
inline bool lengths(std::size_t want, std::size_t got) {
  return want == got || differ_at([&] { return std::to_string(want) + " entries"; }, [&] { return std::to_string(got); });
}
inline bool rows_of(const Value& got, std::size_t n) {
  return (got.rows && got.rows->size() == n) ||
         differ_at([&] { return std::to_string(n) + " parts"; }, [&] { return got.rows ? std::to_string(got.rows->size()) : std::string("no parts"); });
}
inline std::string exact(f64 x) { char b[40]; std::snprintf(b, sizeof b, "%.17g", x); return b; }

// ---- is what the Blueprint gives exactly what the source gives? ----
// (Numbers by their bits: 0.0 and -0.0 are two results.)
inline bool same_bits(f64 a, f64 b) { return std::bit_cast<kern::u64>(a) == std::bit_cast<kern::u64>(b); }
inline bool same(f64 want, const Value& got) { return same_bits(want, got.as_number()) || differ_at([&] { return exact(want); }, [&] { return exact(got.as_number()); }); }
inline bool same(f32 want, const Value& got) {
  return same_bits(static_cast<f64>(want), got.as_number()) || differ_at([&] { return exact(want); }, [&] { return exact(got.as_number()); });
}
inline bool same(bool want, const Value& got) {
  return got.as_bool() == want || differ_at([&] { return std::string(want ? "true" : "false"); }, [&] { return std::string(got.as_bool() ? "true" : "false"); });
}
inline bool same(Vec3 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return at_place(same_bits(want.x, v.x) || differ_at([&] { return exact(want.x); }, [&] { return exact(v.x); }), ".x") &&
         at_place(same_bits(want.y, v.y) || differ_at([&] { return exact(want.y); }, [&] { return exact(v.y); }), ".y") &&
         at_place(same_bits(want.z, v.z) || differ_at([&] { return exact(want.z); }, [&] { return exact(v.z); }), ".z");
}
inline bool same(Vec2 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return at_place(same_bits(want.x, v.x) || differ_at([&] { return exact(want.x); }, [&] { return exact(v.x); }), ".x") &&
         at_place(same_bits(want.y, v.y) || differ_at([&] { return exact(want.y); }, [&] { return exact(v.y); }), ".y");
}
inline bool same(Whole want, const Value& got) {
  return (got.type == kern::logic::PinType::int64 && static_cast<unsigned long long>(got.i) == want.v) ||
         differ_at([&] { return std::to_string(want.v); },
                   [&] { return std::to_string(static_cast<unsigned long long>(got.i)) + (got.type == kern::logic::PinType::int64 ? "" : " (not an Int64)"); });
}
inline bool same(wide::Vec3 want, const Value& got) { return same(kern::Vec3(want), got); }
inline bool same(wide::Vec2 want, const Value& got) { return same(kern::Vec2(want), got); }
inline bool same(const EngineObject* want, const Value& got) {
  return got.as_int() == (want ? want->kern_handle : 0) ||
         differ_at([&] { return "object " + std::to_string(want ? want->kern_handle : 0); }, [&] { return "object " + std::to_string(got.as_int()); });
}
inline bool same(const EngineObject& want, const Value& got) { return same(&want, got); }
template <class T>
  requires std::is_base_of_v<EngineObject, T>
bool same(const std::unique_ptr<T>& want, const Value& got) { return same(static_cast<const EngineObject*>(want.get()), got); }
inline bool same(std::string_view want, const Value& got) {
  return got.as_text() == want || differ_at([&] { return "\"" + std::string(want) + "\""; }, [&] { return "\"" + std::string(got.as_text()) + "\""; });
}
inline bool same(const char* want, const Value& got) { return same(std::string_view(want ? want : ""), got); }
inline bool same(const std::string& want, const Value& got) { return same(std::string_view(want), got); }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
bool same(T want, const Value& got) {
  return got.as_int() == static_cast<kern::i64>(want) || differ_at([&] { return std::to_string(static_cast<kern::i64>(want)); }, [&] { return std::to_string(got.as_int()); });
}
template <class E>
  requires std::is_enum_v<E>
bool same(E want, const Value& got) {
  return got.i == enum_place(want) || differ_at([&] { return std::to_string(enum_place(want)); }, [&] { return std::to_string(got.i); });
}
template <class A, class B> bool same(const std::pair<A, B>& want, const Value& got) {
  return rows_of(got, 2) && at_place(same(want.first, (*got.rows)[0]), ".first") && at_place(same(want.second, (*got.rows)[1]), ".second");
}
template <class T> bool same(const std::vector<T>& want, const Value& got) {
  if constexpr (std::is_same_v<T, bool>) {
    const auto& g = got.items<bool>();
    if (!lengths(want.size(), g.size())) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!at_place(same(static_cast<bool>(want[i]), Value::boolean(g[i])), "[" + std::to_string(i) + "]")) return false;
    return true;
  } else if constexpr (std::is_floating_point_v<T>) {
    const auto& g = got.items<f64>();
    if (!lengths(want.size(), g.size())) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!at_place(same(static_cast<f64>(want[i]), Value::number(g[i])), "[" + std::to_string(i) + "]")) return false;
    return true;
  } else if constexpr (std::is_integral_v<T> || std::is_enum_v<T>) {
    const auto& g = got.items<kern::i64>();
    if (!lengths(want.size(), g.size())) return false;
    const auto as_held = [](const T& x) -> kern::i64 { if constexpr (std::is_enum_v<T>) return enum_place(x); else return static_cast<kern::i64>(x); };
    for (std::size_t i = 0; i < want.size(); ++i)
      if (!at_place(g[i] == as_held(want[i]) || differ_at([&] { return std::to_string(as_held(want[i])); }, [&] { return std::to_string(g[i]); }), "[" + std::to_string(i) + "]"))
        return false;
    return true;
  } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
    const auto& g = got.items<std::string>();
    if (!lengths(want.size(), g.size())) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!at_place(same(std::string_view(want[i]), Value::text(std::string(g[i]))), "[" + std::to_string(i) + "]")) return false;
    return true;
  } else if constexpr (std::is_same_v<T, Vec3> || std::is_same_v<T, Vec2> || std::is_same_v<T, wide::Vec3> || std::is_same_v<T, wide::Vec2>) {
    const auto& g = got.items<Vec3>();
    if (!lengths(want.size(), g.size())) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!at_place(same(want[i], Value::vector(g[i])), "[" + std::to_string(i) + "]")) return false;
    return true;
  } else if constexpr (std::is_same_v<T, Whole>) {   // (a List<Int64>)
    const auto& g = got.items<kern::logic::Int64>();
    if (!lengths(want.size(), g.size())) return false;
    for (std::size_t i = 0; i < want.size(); ++i)
      if (!at_place(g[i].bits == want[i].v || differ_at([&] { return std::to_string(want[i].v); }, [&] { return std::to_string(g[i].bits); }), "[" + std::to_string(i) + "]")) return false;
    return true;
  } else if constexpr (is_list<T> || is_box<T>) {
    const auto& g = got.items<Value>();
    if (!lengths(want.size(), g.size())) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!at_place(rows_of(g[i], 1) && same(want[i], (*g[i].rows)[0]), "[" + std::to_string(i) + "]")) return false;
    return true;
  } else {
    const auto& g = got.items<Value>();
    if (!lengths(want.size(), g.size())) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!at_place(same(want[i], g[i]), "[" + std::to_string(i) + "]")) return false;
    return true;
  }
}

/// A result there is something to compare with: every number in it is one.
inline bool defined(f32 x) { return std::isfinite(x); }
inline bool defined(f64 x) { return std::isfinite(x); }
inline bool defined(Vec3 v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); }
inline bool defined(Vec2 v) { return std::isfinite(v.x) && std::isfinite(v.y); }
inline bool defined(Whole) { return true; }
inline bool defined(wide::Vec3 v) { return defined(kern::Vec3(v)); }
inline bool defined(wide::Vec2 v) { return defined(kern::Vec2(v)); }
inline bool defined(bool) { return true; }
inline bool defined(const std::string&) { return true; }
inline bool defined(const char*) { return true; }
inline bool defined(std::string_view) { return true; }
inline bool defined(const EngineObject*) { return true; }
inline bool defined(const EngineObject&) { return true; }
template <class T>
  requires std::is_base_of_v<EngineObject, T>
bool defined(const std::unique_ptr<T>&) { return true; }
template <class T>
  requires((std::is_integral_v<T> && !std::is_same_v<T, bool>) || std::is_enum_v<T>)
bool defined(T) { return true; }
template <class A, class B> bool defined(const std::pair<A, B>& p) { return defined(p.first) && defined(p.second); }
template <class T> bool defined(const std::vector<T>& v) {
  for (const auto& x : v) if (!defined(static_cast<const T&>(x))) return false;
  return true;
}

// ---- values as text, for a failure's line ----
inline std::string text(f32 x) { char b[40]; std::snprintf(b, sizeof b, "%.9g", static_cast<f64>(x)); return b; }
inline std::string text(f64 x) { char b[40]; std::snprintf(b, sizeof b, "%.17g", x); return b; }
inline std::string text(bool x) { return x ? "true" : "false"; }
inline std::string text(Vec3 v) { return "{" + text(v.x) + ", " + text(v.y) + ", " + text(v.z) + "}"; }
inline std::string text(Vec2 v) { return "{" + text(v.x) + ", " + text(v.y) + "}"; }
inline std::string text(Whole w) {
  char out[40];
  std::snprintf(out, sizeof out, "0x%llx", w.v);
  return out;
}
inline std::string text(wide::Vec3 v) { return text(kern::Vec3(v)); }
inline std::string text(wide::Vec2 v) { return text(kern::Vec2(v)); }
inline std::string text(const char* s) { return std::string("\"") + (s ? s : "") + "\""; }
inline std::string text(const EngineObject* object) { return "object " + std::to_string(object ? object->kern_handle : 0); }
inline std::string text(const EngineObject& object) { return text(&object); }
template <class T>
  requires std::is_base_of_v<EngineObject, T>
std::string text(const std::unique_ptr<T>& object) { return text(static_cast<const EngineObject*>(object.get())); }
inline std::string text(const std::string& s) { return "\"" + s + "\""; }
inline std::string text(std::string_view s) { return text(std::string(s)); }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
std::string text(T x) { return std::to_string(static_cast<long long>(x)); }
template <class E>
  requires std::is_enum_v<E>
std::string text(E e) { return "#" + std::to_string(static_cast<long long>(e)); }
template <class A, class B> std::string text(const std::pair<A, B>& p) { return "{" + text(p.first) + ", " + text(p.second) + "}"; }
template <class T> std::string text(const std::vector<T>& v) {
  std::string out = "[";
  for (std::size_t i = 0; i < v.size(); ++i) out += (i ? ", " : "") + text(static_cast<const T&>(v[i]));
  return out + "]";
}

// ---- a view of a list (kern::Span): the list it shows ----
template <class T> std::vector<std::remove_const_t<T>> as_list(kern::Span<T> s) { return std::vector<std::remove_const_t<T>>(s.begin(), s.end()); }
template <class T> Value arg(kern::Span<T> s) { return arg(as_list(s)); }
template <class T> bool same(kern::Span<T> want, const Value& got) { return same(as_list(want), got); }
template <class T> bool defined(kern::Span<T> s) { return defined(as_list(s)); }
template <class T> std::string text(kern::Span<T> s) { return text(as_list(s)); }

// ---- a list of a fixed length (std::array): a list, as the Blueprint has it ----
template <class T, std::size_t N> std::vector<T> as_list(const std::array<T, N>& a) { return std::vector<T>(a.begin(), a.end()); }
template <class T, std::size_t N> Value arg(const std::array<T, N>& a) { return arg(as_list(a)); }
template <class T, std::size_t N> bool same(const std::array<T, N>& want, const Value& got) { return same(as_list(want), got); }
template <class T, std::size_t N> bool defined(const std::array<T, N>& a) { return defined(as_list(a)); }
template <class T, std::size_t N> std::string text(const std::array<T, N>& a) { return text(as_list(a)); }

// ---- a map, a set (kern::Map, kern::Set): the language's, their entries in their order ----
template <class K, class V, class H> Value arg(const kern::Map<K, V, H>& m) {
  Value r = Value::zero(kern::logic::PinType::map);
  if (m.empty()) return r;
  kern::logic::MapEntries& e = r.own_entries();
  e.reserve(m.size());
  for (const auto& [k, v] : m) e.try_emplace(kern::logic::key_of(arg(k)), arg(v));
  return r;
}
template <class K, class H> Value arg(const kern::Set<K, H>& s) {
  Value r = Value::zero(kern::logic::PinType::set);
  if (s.empty()) return r;
  kern::logic::MapEntries& e = r.own_entries();
  e.reserve(s.size());
  for (const K& k : s) e.try_emplace(kern::logic::key_of(arg(k)));
  return r;
}
template <class K, class V, class H> bool same(const kern::Map<K, V, H>& want, const Value& got) {
  if (got.type != kern::logic::PinType::map || got.size() != want.size()) return false;
  for (std::size_t k = 0; k < want.size(); ++k)
    if (!(kern::logic::key_of(arg(want.key_at(k))) == got.keyed().key_at(k)) || !same(want.value_at(k), got.keyed().value_at(k))) return false;
  return true;
}
template <class K, class H> bool same(const kern::Set<K, H>& want, const Value& got) {
  if (got.type != kern::logic::PinType::set || got.size() != want.size()) return false;
  std::size_t k = 0;
  for (const K& key : want)
    if (!(kern::logic::key_of(arg(key)) == got.keyed().key_at(k++))) return false;
  return true;
}
template <class SK, class SV, class M> Value arg(const ShapedMap<SK, SV, M>& w) {
  Value r = Value::zero(kern::logic::PinType::map);
  if (w.m.empty()) return r;
  kern::logic::MapEntries& e = r.own_entries();
  e.reserve(w.m.size());
  for (const auto& [k, v] : w.m) e.try_emplace(kern::logic::key_of(arg(shaped<SK>(k))), arg(shaped<SV>(v)));
  return r;
}
template <class SK, class M> Value arg(const ShapedSet<SK, M>& w) {
  Value r = Value::zero(kern::logic::PinType::set);
  if (w.s.empty()) return r;
  kern::logic::MapEntries& e = r.own_entries();
  e.reserve(w.s.size());
  for (const auto& k : w.s) e.try_emplace(kern::logic::key_of(arg(shaped<SK>(k))));
  return r;
}
template <class SK, class SV, class M> bool same(const ShapedMap<SK, SV, M>& want, const Value& got) {
  if (got.type != kern::logic::PinType::map || got.size() != want.m.size()) return false;
  for (std::size_t k = 0; k < want.m.size(); ++k)
    if (!(kern::logic::key_of(arg(shaped<SK>(want.m.key_at(k)))) == got.keyed().key_at(k)) || !same(shaped<SV>(want.m.value_at(k)), got.keyed().value_at(k))) return false;
  return true;
}
template <class SK, class M> bool same(const ShapedSet<SK, M>& want, const Value& got) {
  if (got.type != kern::logic::PinType::set || got.size() != want.s.size()) return false;
  std::size_t k = 0;
  for (const auto& key : want.s)
    if (!(kern::logic::key_of(arg(shaped<SK>(key))) == got.keyed().key_at(k++))) return false;
  return true;
}
template <class SK, class SV, class M> bool defined(const ShapedMap<SK, SV, M>& w) { return defined(w.m); }
template <class SK, class M> bool defined(const ShapedSet<SK, M>&) { return true; }
template <class SK, class SV, class M> std::string text(const ShapedMap<SK, SV, M>& w) { return text(w.m); }
template <class SK, class M> std::string text(const ShapedSet<SK, M>& w) { return text(w.s); }
template <class K, class V, class H> bool defined(const kern::Map<K, V, H>& m) {
  for (const auto& [k, v] : m) if (!defined(v)) return false;
  return true;
}
template <class K, class H> bool defined(const kern::Set<K, H>&) { return true; }
template <class K, class V, class H> std::string text(const kern::Map<K, V, H>& m) {
  std::string out = "{";
  for (const auto& [k, v] : m) out += (out.size() > 1 ? ", " : "") + text(k) + ": " + text(v);
  return out + "}";
}
template <class K, class H> std::string text(const kern::Set<K, H>& s) {
  std::string out = "{";
  for (const K& k : s) out += (out.size() > 1 ? ", " : "") + text(k);
  return out + "}";
}

// ---- what may be there or not (std::unique_ptr, std::optional of a value): as the Blueprint has it, a
// list of no entry or one ----
/// (Without a copy of what is there: a class may be one that is not to be copied.)
template <class T> Value arg_one(const T* p) {
  if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T> || std::is_same_v<T, std::string> || std::is_same_v<T, Vec3> || std::is_same_v<T, Vec2> ||
                std::is_same_v<T, wide::Vec3> || std::is_same_v<T, wide::Vec2> || is_list<T>) {
    return arg(p ? std::vector<T>{*p} : std::vector<T>{});
  } else {
    std::vector<Value> out;
    if (p) out.push_back(arg(*p));
    return Value::structures(std::move(out));
  }
}
template <class T> bool same_one(const T* want, const Value& got) { return got.size() == (want ? 1u : 0u) && (!want || same(*want, got.item(0))); }
template <class T> std::string text_one(const T* p) { return p ? "[" + text(*p) + "]" : "[]"; }
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
Value arg(const std::unique_ptr<T>& p) { return arg_one(p.get()); }
template <class T> Value arg(const std::optional<T>& p) { return arg_one(p ? &*p : nullptr); }
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
bool same(const std::unique_ptr<T>& want, const Value& got) { return same_one(want.get(), got); }
template <class T> bool same(const std::optional<T>& want, const Value& got) { return same_one(want ? &*want : nullptr, got); }
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
bool defined(const std::unique_ptr<T>& p) { return !p || defined(*p); }
template <class T> bool defined(const std::optional<T>& p) { return !p || defined(*p); }
template <class T>
  requires(!std::is_base_of_v<EngineObject, T>)
std::string text(const std::unique_ptr<T>& p) { return text_one(p.get()); }
template <class T> std::string text(const std::optional<T>& p) { return text_one(p ? &*p : nullptr); }

// ---- a value the engine's stand-in answered, as the source gets it ----
inline void take(f64& to, const Value& v) { to = v.as_number(); }
inline void take(bool& to, const Value& v) { to = v.as_bool(); }
inline void take(wide::Vec3& to, const Value& v) { to = wide::Vec3(v.as_vector()); }
inline void take(wide::Vec2& to, const Value& v) { to = wide::Vec2(v.as_vector().x, v.as_vector().y); }
inline void take(std::string& to, const Value& v) { to = v.as_text(); }
inline void take(const char*& to, const Value& v) {
  static std::deque<std::string> kept;   // (a text the source holds by a pointer: kept for it)
  kept.push_back(v.as_text());
  to = kept.back().c_str();
}
inline void take(std::string_view& to, const Value& v) {
  static std::deque<std::string> kept;   // (a text the source holds by a view: kept for it)
  kept.push_back(v.as_text());
  to = kept.back();
}
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
void take(T& to, const Value& v) { to = static_cast<T>(v.as_int()); }
template <class E>
  requires std::is_enum_v<E>
void take(E& to, const Value& v) { to = enum_value<E>(v.i); }
template <class T>
  requires std::is_base_of_v<EngineObject, T>
void take(T*& to, const Value& v) { to = engine_object<std::remove_const_t<T>>(v.as_int()); }   // (one the source may only read: the same object)
template <class T>
  requires std::is_base_of_v<EngineObject, T>
void take(T& to, const Value& v) { to = *engine_object<T>(v.as_int()); }
/// An owned object of the engine (std::unique_ptr): the one of that handle, none for 0.
template <class T>
  requires std::is_base_of_v<EngineObject, T>
void take(std::unique_ptr<T>& to, const Value& v) {
  // (what this replaces is the test's handing over, not the source's: no release - and what it holds then
  // is the source's, as the Blueprint's handle there)
  const Quiet q;
  if (v.as_int() == 0) to.reset();
  else to = std::make_unique<T>(owned(*engine_object<T>(v.as_int())));
}
template <class T> void take(std::vector<T>& to, const Value& v);
/// A view of a list the source holds (kern::Span - what Renderer::meshes() gives): of the list, kept for it
/// (as a text the source holds by a view, see take of a std::string_view).
/// sscanf of the source, its places the test's (a float the test holds as a double or a Num32): each value is
/// read as the C type its conversion says (%f: float, %lf: double, %llu: unsigned long long ...) and then given
/// to its place - what the source reads, as many as it reads (EOF before the first, as sscanf).
template <class A> int scan_one(const char*& p, const std::string& head, char conv, const std::string& len, A* out) {
  int used = 0, r = 0;
  const std::string f = head + "%n";
  auto into = [&](auto tmp) {
    r = std::sscanf(p, f.c_str(), &tmp, &used);
    if constexpr (std::is_arithmetic_v<A> || std::is_same_v<A, wide::Num32>) { if (r == 1) *out = static_cast<A>(tmp); }
  };
  if (conv == 's' || conv == 'c' || conv == '[') {
    if constexpr (std::is_same_v<A, char>) r = std::sscanf(p, f.c_str(), out, &used);
  } else if (conv == 'f' || conv == 'e' || conv == 'g' || conv == 'a') {
    if (len == "l") into(double{}); else if (len == "L") into((long double){}); else into(float{});
  } else if (conv == 'd' || conv == 'i') {
    if (len == "hh") into((signed char){}); else if (len == "h") into(short{}); else if (len == "l") into(long{}); else if (len == "ll") into((long long){}); else into(int{});
  } else {
    if (len == "hh") into((unsigned char){}); else if (len == "h") into((unsigned short){}); else if (len == "l") into((unsigned long){}); else if (len == "ll") into((unsigned long long){}); else into(unsigned{});
  }
  if (r == 1) p += used;
  return r;
}
template <class... A> int scan_c(const char* s, const char* fmt, A*... out) {
  // the format, one piece per value: what stands before its conversion (text, %%, %*...) and the conversion
  std::vector<std::string> heads, lens;
  std::vector<char> convs;
  std::string head;
  for (const char* c = fmt; *c;) {
    if (*c != '%') { head += *c++; continue; }
    const char* at = c++;
    if (*c == '%') { head += "%%"; ++c; continue; }
    const bool skip = *c == '*';
    if (skip) ++c;
    while (*c >= '0' && *c <= '9') ++c;
    std::string len;
    while (*c == 'h' || *c == 'l' || *c == 'L' || *c == 'j' || *c == 'z' || *c == 't') len += *c++;
    if (*c == '[') { ++c; if (*c == ']') ++c; while (*c && *c != ']') ++c; }
    const char conv = *c;
    if (*c) ++c;
    head.append(at, c);
    if (skip) continue;
    heads.push_back(head), lens.push_back(len), convs.push_back(conv == ']' ? '[' : conv);
    head.clear();
  }
  const char* p = s;
  int count = 0;
  std::size_t i = 0;
  bool going = true;
  auto step = [&](auto* o) {
    if (!going || i >= heads.size()) { going = false; return; }
    const int r = scan_one(p, heads[i], convs[i], lens[i], o);
    ++i;
    if (r == 1) ++count;
    else { going = false; if (count == 0 && r == EOF) count = EOF; }
  };
  (step(out), ...);
  return count;
}
template <class T> void take(kern::Span<T>& to, const Value& v) {
  static std::deque<std::vector<std::remove_const_t<T>>> kept;
  kept.emplace_back();
  take(kept.back(), v);
  to = kern::Span<T>(kept.back().data(), kept.back().size());
}
/// By the type as the program spells it (S, with Whole where it says u64 or i64 - see shaped).
/// Numbers the engine gives at a place of its own (`const f64* RoadStream::data()`): the test keeps those its stand-in
/// answers, and the source reads them where the pointer points - as long as the test runs.
/// How many numbers there are where a pointer the test took (take of a const T*) points - none for any other pointer.
inline std::map<const void*, std::size_t>& rooms() {
  static std::map<const void*, std::size_t> r;
  return r;
}
template <class T>
  requires std::is_arithmetic_v<T>
void take(const T*& to, const Value& v) {
  static std::deque<std::vector<T>> kept;
  kept.emplace_back();
  take(kept.back(), v);
  to = kept.back().data();
  rooms()[to] = kept.back().size();
}
/// How many entries the source hands the engine from a pointer, a count beside it: as many as the count says - but where
/// the pointer is one the test took from the stand-in (data() of an object of the engine), the numbers it holds, all
/// of them: the Blueprint hands the engine the list that node gave it, and the stand-in answers size() on its own
/// (the count is handed beside them, and compared as well).
template <class T> std::size_t room(const T* p, std::size_t count) {
  const auto it = rooms().find(static_cast<const void*>(p));
  return it == rooms().end() ? count : it->second;
}
template <class S> struct Take {
  template <class T> static void to(T& target, const Value& v) { take(target, v); }
};
template <> struct Take<Whole> {
  template <class T> static void to(T& target, const Value& v) { target = v.type == kern::logic::PinType::int64 ? static_cast<T>(static_cast<unsigned long long>(v.i)) : T{}; }
};
template <class S> struct Take<std::vector<S>> {
  template <class T> static void to(std::vector<T>& target, const Value& v) {
    target.clear();
    for (std::size_t k = 0; k < v.size(); ++k) {
      T item = quiet<T>();
      if constexpr (is_list<S> || is_box<S>) {   // (a list inside a list: a struct that holds it, as the Blueprint has it)
        const Value row = v.item(k);
        if (row.rows && row.rows->size() == 1) Take<S>::to(item, (*row.rows)[0]);
      } else {
        Take<S>::to(item, v.item(k));
      }
      target.push_back(std::move(item));
    }
  }
  /// (into a view: the entries, kept by the test - see kept_span)
  template <class T> static void to(wide::Span<const T>& target, const Value& v) {
    std::vector<T> entries;
    to(entries, v);
    target = kept_span(std::move(entries));
  }
};
/// A list of a fixed length from a list: cut to its length, what the list does not have zero (as a
/// Blueprint sets a place of a fixed length as a whole).
template <class S, std::size_t N> struct Take<std::array<S, N>> {
  template <class T> static void to(std::array<T, N>& target, const Value& v) {
    std::vector<T> items;
    Take<std::vector<S>>::to(items, v);
    for (std::size_t k = 0; k < N; ++k) target[k] = k < items.size() ? std::move(items[k]) : T{};
  }
};
/// What may be there or not, from the list of no entry or one. (An object of the engine: by its handle. A
/// value whose class cannot be made without one it refers to - it holds a reference - stays as it is
/// where there is none: the Blueprint's list holds no such object.)
template <class S> struct Take<std::unique_ptr<S>> {
  template <class T> static void to(std::unique_ptr<T>& target, const Value& v) {
    if constexpr (std::is_base_of_v<EngineObject, T>) {
      const long long handle = v.as_int();
      target = handle ? std::make_unique<T>(owned(*engine_object<T>(handle))) : nullptr;
    } else {
      if (v.size() == 0) {
        target.reset();
        return;
      }
      if constexpr (std::is_default_constructible_v<T>) {
        if (!target) {
          const Quiet q;
          target = std::make_unique<T>();
        }
      }
      if (target) Take<S>::to(*target, v.item(0));
    }
  }
};
template <class S> struct Take<std::optional<S>> {
  template <class T> static void to(std::optional<T>& target, const Value& v) {
    if (v.size() == 0) {
      target.reset();
      return;
    }
    if (!target) {
      const Quiet q;
      target.emplace();
    }
    Take<S>::to(*target, v.item(0));
  }
};
template <class A, class B> struct Take<std::pair<A, B>> {
  template <class X, class Y> static void to(std::pair<X, Y>& target, const Value& v) {
    if (!v.rows || v.rows->size() != 2) return;
    Take<A>::to(const_cast<std::remove_const_t<X>&>(target.first), (*v.rows)[0]);
    Take<B>::to(target.second, (*v.rows)[1]);
  }
};
/// A map, a set: its entries in their order, each key as the value it is (a key of the type S says).
template <class SK, class K> K key_from(const kern::logic::MapKey& key) {
  K out{};
  Take<SK>::to(out, kern::logic::key_value(key, arg(shaped<SK>(K{})).type));
  return out;
}
template <class SK, class SV> struct Take<kern::Map<SK, SV>> {
  template <class K, class V> static void to(kern::Map<K, V>& target, const Value& v) {
    target.clear();
    if (v.type != kern::logic::PinType::map) return;
    for (std::size_t k = 0; k < v.size(); ++k) {
      V held{};
      Take<SV>::to(held, v.keyed().value_at(k));
      target.insert_or_assign(key_from<SK, K>(v.keyed().key_at(k)), std::move(held));
    }
  }
};
template <class SK> struct Take<kern::Set<SK>> {
  template <class K> static void to(kern::Set<K>& target, const Value& v) {
    target.clear();
    if (v.type != kern::logic::PinType::set) return;
    for (std::size_t k = 0; k < v.size(); ++k) target.insert(key_from<SK, K>(v.keyed().key_at(k)));
  }
};
template <class S, class T> void take_as(T& target, const Value& v) { Take<S>::to(target, v); }
/// What the engine writes through a pointer, their count beside it (kern::logic::EnginePin::count): the
/// entries of the list the stand-in gives, one after the other from `first` on - no more than the count
/// says (the room the call says it has; the stand-in gives no more than that either).
template <class S, class T, class N> void take_into(T* first, const N& count, const Value& v) {
  std::vector<T> items;
  Take<std::vector<S>>::to(items, v);
  for (std::size_t k = 0; k < items.size() && static_cast<long long>(k) < static_cast<long long>(count); ++k) first[k] = std::move(items[k]);
}
template <class T> void take(std::vector<T>& to, const Value& v) { Take<std::vector<T>>::to(to, v); }

/// A value of the Blueprint as text, every number with all its digits.
inline std::string shown(const Value& v) {
  using kern::logic::PinType;
  switch (v.type) {
    case PinType::number: return text(v.n);
    case PinType::vector: return text(v.v);
    case PinType::structure: {
      std::string out = "{";
      if (v.rows) for (std::size_t i = 0; i < v.rows->size(); ++i) out += (i ? ", " : "") + shown((*v.rows)[i]);
      return out + "}";
    }
    case PinType::number_list: return text(v.items<f64>());
    case PinType::structure_list: {
      std::string out = "[";
      const auto& l = v.items<Value>();
      for (std::size_t i = 0; i < l.size(); ++i) out += (i ? ", " : "") + shown(l[i]);
      return out + "]";
    }
    default: return v.as_text();
  }
}
/// The arguments of a call as the Blueprint is given them, for a failure's line.
inline std::string shown_all(const std::vector<Value>& given) {
  std::string out;
  for (std::size_t i = 0; i < given.size(); ++i) out += (i ? ", " : "") + shown(given[i]);
  return out;
}

inline constexpr int kCalls = 300;

// ---- what a call costs ----
// The class of a Blueprint costs what the source it was written from costs: it does not ask for memory more
// often. The allocator counts (cost.h) while the source's function runs - a generated test calls it through
// `counted` - and while the class's function runs: the class's glue says when (logic::native_probe).
// What makes the arguments and takes the results is outside on both sides.
inline long g_source_asks = 0, g_class_asks = 0;
inline cost::Trace g_source_trace, g_class_trace;   // (from where they asked: kept for the function SC_COST_WHERE names - cost.h)
// ---- what C++ leaves open in an object: what nothing sets (a member without an initializer, `i32 on;`) ----
// Its value is indeterminate - C++ does not say it; the Blueprint starts every value at the zero of its type.
// The source's call runs on zeroed memory - its stack below the test (zero_stack), what it gives in zeroed
// room (Zeroed), what it asks the allocator for zeroed (test_main.cpp) -: then what C++ leaves open is that
// zero, which is one of the values C++ allows there. Nothing C++ sets changes: a value set overwrites it.
[[gnu::noinline]] inline void zero_stack() {
  unsigned char room[1 << 16];
  std::memset(room, 0, sizeof room);
  asm volatile("" : : "r"(room) : "memory");   // (written, as if read: not left out)
}
/// What the source's call gives, made in zeroed room (C++17: the value a call gives is made right there).
template <class T> class Zeroed {
 public:
  template <class F> explicit Zeroed(F&& make) {
    std::memset(room_, 0, sizeof room_);
    made_ = ::new (static_cast<void*>(room_)) T(make());
  }
  ~Zeroed() { made_->~T(); }
  Zeroed(const Zeroed&) = delete;
  Zeroed& operator=(const Zeroed&) = delete;
  const T& get() const { return *made_; }
 private:
  alignas(T) unsigned char room_[sizeof(T)];
  T* made_ = nullptr;
};

/// The source's own call, counted: from where it begins until it has returned what it gives.
template <class Call> decltype(auto) counted(Call&& call) {
  struct Ends {
    ~Ends() {
      g_source_asks = cost::end();
      if (cost::tracing) g_source_trace = cost::now;
    }
  };
  cost::begin();
  const Ends ends;
  return call();
}
inline void class_runs(bool begins) {
  static int depth = 0;   // (a function of a class that calls one of another class by name: the outer call counts all of it)
  if (begins) {
    if (depth++ == 0) cost::begin();
  } else if (--depth == 0) {
    g_class_asks = cost::end();
    if (cost::tracing) g_class_trace = cost::now;
  }
}

/// A value as the Blueprint gets it - where a stand-in of the engine passes it on, whose class may have a
/// function of its own called `arg` (Engine::arg): found as everywhere else (and a struct of the test by its
/// own helpers).
template <class T> Value value_of(const T& x) { return arg(x); }

/// Is what the Blueprint gives this very value, bit for bit? (Numbers by their bits; a struct, a list, a map
/// entry for entry.)
inline bool same_value(const Value& a, const Value& b) {
  using kern::logic::PinType;
  if (a.type != b.type) return false;
  if (a.type == PinType::number) return same_bits(a.n, b.n);
  if (a.type == PinType::vector) return same_bits(a.v.x, b.v.x) && same_bits(a.v.y, b.v.y) && same_bits(a.v.z, b.v.z);
  if (a.type == PinType::structure) {
    const std::size_t n = a.rows ? a.rows->size() : 0;
    if (n != (b.rows ? b.rows->size() : 0)) return false;
    for (std::size_t k = 0; k < n; ++k) if (!same_value((*a.rows)[k], (*b.rows)[k])) return false;
    return true;
  }
  if (a.type == PinType::map || a.type == PinType::set) {
    if (a.size() != b.size()) return false;
    for (std::size_t k = 0; k < a.size(); ++k)
      if (!(a.keyed().key_at(k) == b.keyed().key_at(k)) || (a.type == PinType::map && !same_value(a.keyed().value_at(k), b.keyed().value_at(k)))) return false;
    return true;
  }
  if (kern::logic::is_list(a.type)) {
    if (a.size() != b.size()) return false;
    for (std::size_t k = 0; k < a.size(); ++k) if (!same_value(a.item(k), b.item(k))) return false;
    return true;
  }
  return a == b;
}

// ---- callbacks: a function the engine calls back (std::function<R(A...)>, or a struct of the engine that is one -
// `struct HeightSampler { f64 (*fn)(void* user, f64 x, f64 y); void* user; }`). In the Blueprint a Callback
// (PinType::callback): a function of a library and the place it is bound to; only the engine calls it. ----
template <class T> struct CallbackSig;
template <class R, class... A> struct CallbackSig<std::function<R(A...)>> { using Result = R; using Args = std::tuple<A...>; };
template <class R, class... A> struct CallbackSig<R (*)(void*, A...)> { using Result = R; using Args = std::tuple<A...>; };
/// A callback of the source as the Blueprint is handed it: a Callable that calls it, its arguments and result as
/// the test's values (take, arg).
template <class R, class Args, class F> struct SourceCallable;
template <class R, class... A, class F> struct SourceCallable<R, std::tuple<A...>, F> : kern::logic::Callable {
  F f;
  explicit SourceCallable(F g) : f(std::move(g)) {}
  bool call(std::span<const Value> args, Value& result) const override {
    if (args.size() != sizeof...(A)) return false;
    std::tuple<std::remove_cvref_t<A>...> in;
    std::size_t k = 0;
    std::apply([&](auto&... x) { ((take(x, args[k++])), ...); }, in);
    if constexpr (std::is_void_v<R>) {
      std::apply(f, in);
      result = Value{};
    } else {
      result = arg(std::apply(f, in));
    }
    return true;
  }
};
template <class R, class... A> Value arg(const std::function<R(A...)>& f) {
  if (!f) return Value::callback(nullptr);
  return Value::callback(std::make_shared<SourceCallable<R, std::tuple<A...>, std::function<R(A...)>>>(f));
}
template <FnAndUser S> Value arg(const S& s) {
  using Sig = CallbackSig<std::remove_cv_t<decltype(S::fn)>>;
  if (!s.fn) return Value::callback(nullptr);
  const auto call = [fn = s.fn, user = s.user](auto... a) { return fn(user, a...); };
  return Value::callback(std::make_shared<SourceCallable<typename Sig::Result, typename Sig::Args, decltype(call)>>(call));
}
/// The values a callback is called with to compare it (a number, a whole number, a flag, a vector of each input).
template <class X> Value probe_value(f64 v) {
  if constexpr (std::is_same_v<X, bool>) return Value::boolean(v > 0);
  else if constexpr (std::is_integral_v<X>) return Value::integer(static_cast<kern::i64>(v * 4));
  else if constexpr (std::is_floating_point_v<X>) return Value::number(v);
  else return Value::vector({static_cast<f32>(v), static_cast<f32>(-v), static_cast<f32>(v * 0.5)});
}
template <class... A> std::vector<Value> probe_values(std::tuple<A...>*, std::size_t from) {
  static constexpr f64 at[] = {1.5, -2.25, 4, 0, -0.5, 7.25};
  std::size_t k = from;
  return {probe_value<std::remove_cvref_t<A>>(at[k++ % std::size(at)])...};
}
/// Is what the Blueprint gives this callback? The same, bound or not, and where bound, the same answer at a few places
/// (1.5, -2.25, 4, …), bit for bit - what the engine would get from either.
template <class R, class Args> bool same_call(const std::function<bool(std::span<const Value>, Value&)>& want, const Value& got) {
  if (got.type != kern::logic::PinType::callback) return differ_at([] { return std::string("a callback"); }, [&] { return std::string("no callback"); });
  for (int round = 0; round < 3; ++round) {
    const std::vector<Value> in = probe_values(static_cast<Args*>(nullptr), static_cast<std::size_t>(round));
    Value a, b;
    const bool wa = want(in, a);
    const bool gb = got.call && got.call->call(in, b);
    if (wa != gb) return differ_at([&] { return std::string(wa ? "bound" : "unbound"); }, [&] { return std::string(gb ? "bound" : "unbound"); });
    if (!wa) return true;
    if (!same_value(a, b)) return differ_at([&] { return "at call " + std::to_string(round + 1) + ": " + shown(a); }, [&] { return shown(b); });
  }
  return true;
}
template <class R, class... A> bool same(const std::function<R(A...)>& want, const Value& got) {
  const Value w = arg(want);
  return same_call<R, std::tuple<std::remove_cvref_t<A>...>>([&](std::span<const Value> in, Value& out) { return w.call && w.call->call(in, out); }, got);
}
template <FnAndUser S> bool same(const S& want, const Value& got) {
  using Sig = CallbackSig<std::remove_cv_t<decltype(S::fn)>>;
  const Value w = arg(want);
  return same_call<typename Sig::Result, typename Sig::Args>([&](std::span<const Value> in, Value& out) { return w.call && w.call->call(in, out); }, got);
}
template <class R, class... A> std::string text(const std::function<R(A...)>& f) { return f ? "(a callback)" : "(no callback)"; }
template <FnAndUser S> std::string text(const S& s) { return s.fn ? "(a callback)" : "(no callback)"; }
/// What the Blueprint gives, as the source's callback: one that calls it (the value kept as long as the test runs).
template <class R, class... A> void take(std::function<R(A...)>& to, const Value& v) {
  if (!v.call) { to = nullptr; return; }
  to = [held = v](A... a) -> R {
    Value r;
    std::vector<Value> in{arg(a)...};
    held.call->call(in, r);
    if constexpr (!std::is_void_v<R>) {
      R out{};
      take(out, r);
      return out;
    }
  };
}
template <class R, class... A> R value_callback_thunk(void* user, A... a) {
  const Value& held = *static_cast<const Value*>(user);
  Value r;
  std::vector<Value> in{arg(a)...};
  if (held.call) held.call->call(in, r);
  if constexpr (!std::is_void_v<R>) {
    R out{};
    take(out, r);
    return out;
  }
}
template <class R, class... A> auto value_thunk_of(R (*)(void*, A...)) { return &value_callback_thunk<R, A...>; }
template <FnAndUser S> void take(S& to, const Value& v) {
  if (!v.call) { to.fn = nullptr; to.user = nullptr; return; }
  static std::deque<Value> kept;
  kept.push_back(v);
  to.fn = value_thunk_of(decltype(S::fn){});
  to.user = &kept.back();
}
template <class X> f64 callback_number(const X& x) {
  if constexpr (std::is_arithmetic_v<X>) return static_cast<f64>(x);
  else return static_cast<f64>(x.x) + 2.0 * static_cast<f64>(x.y);
}
/// A drawn callback: none (unbound), or one of three plain functions of its arguments (each the same in every run).
template <class R, class... A> struct DrawnCallback {
  static R pick(int which, A... a) {
    if constexpr (std::is_void_v<R>) {
      return;
    } else {
      f64 sum = 0, k = 1;
      ((sum += k * static_cast<f64>(callback_number(a)), k += 0.5), ...);
      const f64 r = which == 0 ? sum : which == 1 ? sum * 0.25 - 1.5 : -sum + 3;
      if constexpr (std::is_same_v<R, bool>) return r > 0;
      else if constexpr (std::is_arithmetic_v<R>) return static_cast<R>(r);
      else return R{static_cast<f32>(r), static_cast<f32>(-r), 0};
    }
  }
  template <int W> static R plain(void*, A... a) { return pick(W, a...); }
};
template <class T> T draw_callback(Draw& draw) {
  const u32 which = draw.below(4);
  T out{};
  if constexpr (FnAndUser<T>) {
    if (which == 3) return out;
    using Sig = CallbackSig<std::remove_cv_t<decltype(T::fn)>>;
    [&]<class... A>(std::tuple<A...>*) {
      using D = DrawnCallback<typename Sig::Result, A...>;
      out.fn = which == 0 ? &D::template plain<0> : which == 1 ? &D::template plain<1> : &D::template plain<2>;
    }(static_cast<typename Sig::Args*>(nullptr));
  } else {
    if (which == 3) return out;
    using Sig = CallbackSig<T>;
    [&]<class... A>(std::tuple<A...>*) {
      using D = DrawnCallback<typename Sig::Result, A...>;
      out = [which](A... a) { return D::pick(static_cast<int>(which), a...); };
    }(static_cast<typename Sig::Args*>(nullptr));
  }
  return out;
}

/// The Blueprints of the level's actors a converted Blueprint calls on them (Call on Actor, Get Variable on
/// Actor - kern::logic::ActorBlueprint in the source), running beside it in its runtime, each by its name
/// (RuleBlueprint: the host's find_actor finds them) - and for the source in a runtime of their own, bound
/// to its ActorBlueprints. The engine is the test's stand-in in all of them. Every call begins with the
/// actors' variables as they were when the runtime started, in all three, and they must be the same after it.
struct ActorsStarted {
  std::vector<std::pair<kern::logic::Instance*, std::vector<std::pair<std::string, Value>>>> started;
  void keep(RuleBlueprint& r, const std::vector<std::string>& actors) {
    for (const std::string& name : actors)
      if (kern::logic::Instance* i = r.find(name)) {
        std::vector<std::pair<std::string, Value>> vars;
        for (const kern::logic::Variable& v : i->graph().variables)
          if (!v.constant) vars.emplace_back(v.name, i->get(v.name));
        started.emplace_back(i, std::move(vars));
      }
  }
  /// The actors' variables as they were at the start.
  void restore() {
    for (auto& [inst, vars] : started)
      for (const auto& [name, value] : vars) inst->set(name, value);
  }
  /// What the actors' variables are now (of those the converted Blueprint calls).
  std::vector<Value> now() const {
    std::vector<Value> out;
    for (const auto& [inst, vars] : started)
      for (const auto& [name, value] : vars) out.push_back(inst->get(name));
    return out;
  }
};

/// A converted source file's Blueprint, in the interpreter and as its compiled class. (`actors`: the
/// level's actors whose Blueprints it calls on them - see ActorsStarted.)
struct Blueprint {
  std::unique_ptr<RuleBlueprint> interpreted, compiled, source;
  ActorsStarted interpreted_actors, compiled_actors, source_actors;
  kern::logic::Instance* interp = nullptr;
  kern::logic::Instance* native = nullptr;
  const char* name;
  std::vector<std::string> others;
  explicit Blueprint(const char* actor, std::vector<std::string> actors = {}) : name(actor), others(actors) {
    interpreted = std::make_unique<RuleBlueprint>(actor, nullptr, false, actors);
    compiled = std::make_unique<RuleBlueprint>(actor, nullptr, true, actors);
    // (The loop guard is the host's: a program whose functions loop long - a sound computed sample by
    // sample, a number raised to a 256-bit power - sets it to what they take.)
    interpreted->rt.budget = compiled->rt.budget = 2000000000u;
    interp = interpreted->inst;
    native = compiled->inst;
    SC_CHECK(interp && native, "%s: %s%s", actor, interpreted->problems.c_str(), compiled->problems.c_str());
    SC_CHECK(interpreted->problems.empty(), "%s: %s", actor, interpreted->problems.c_str());
    if (!actors.empty()) {
      source = std::make_unique<RuleBlueprint>(nullptr, nullptr, true, actors);
      source->rt.budget = 2000000000u;
      source->host.keeps = &g_source_engine;   // (what the actors ask the engine for the source: what the source asks)
      source->host.keeps_objects = &g_source_objects;
      SC_CHECK(source->problems.empty(), "%s: %s", actor, source->problems.c_str());
      interpreted_actors.keep(*interpreted, actors);
      compiled_actors.keep(*compiled, actors);
      source_actors.keep(*source, actors);
    }
    SC_CHECK(native && std::string(native->tier()) == "native C++", "%s runs as its C++ class (it runs: %s)", actor, native ? native->tier() : "not at all");
    kern::logic::native_probe = class_runs;
  }
  /// Did a call stop the Blueprint because it held an entry of a list that handlers changed (Runtime::held_lost)? In C++
  /// the reference would go nowhere - what the call gives C++ leaves open. The Blueprint (class and interpreter) is made
  /// anew - a stopped one takes no call - and true.
  bool held_lost() {
    const auto lost = [](const std::unique_ptr<RuleBlueprint>& r) { return r && r->host.errors.find("a call held an entry of a list while handlers ran") != std::string::npos; };
    if (!lost(interpreted) && !lost(compiled)) return false;
    interpreted = std::make_unique<RuleBlueprint>(name, nullptr, false, others);
    compiled = std::make_unique<RuleBlueprint>(name, nullptr, true, others);
    interpreted->rt.budget = compiled->rt.budget = 2000000000u;
    interp = interpreted->inst;
    native = compiled->inst;
    interpreted_actors = {};
    compiled_actors = {};
    if (!others.empty()) {
      interpreted_actors.keep(*interpreted, others);
      compiled_actors.keep(*compiled, others);
    }
    kern::logic::native_probe = class_runs;
    return true;
  }
  /// The logic the source's ActorBlueprints are bound to (kern::logic::ActorBlueprint::bind): the runtime of
  /// its own in which the actors' Blueprints run - where it calls none, one in which none has a Blueprint.
  kern::logic::Runtime& source_logic() { return source ? source->rt : no_logic(); }
  /// Before the source's call: the actors' Blueprints as they started.
  void before_source() { source_actors.restore(); }
  /// What the Blueprint asked the engine's stand-in.
  const std::vector<std::string>& calls(bool in_class) { return (in_class ? compiled : interpreted)->host.stand_in.calls; }
  void before_call(bool in_class) {
    (in_class ? compiled : interpreted)->host.stand_in.calls.clear();
    (in_class ? compiled : interpreted)->host.stand_in.kept.clear();   // (what objects of the engine were handed before: not of this call - see the source's)
    if (real_engine()) fresh_engine((in_class ? compiled : interpreted)->host.objects);
    (in_class ? compiled_actors : interpreted_actors).restore();
  }
  /// The actors' variables after the calls: the same in the source's runtime and in the Blueprint's.
  bool actors_same(bool both) const {
    if (!source) return true;
    const std::vector<Value> want = source_actors.now(), b = compiled_actors.now(), a = both ? interpreted_actors.now() : b;
    if (want.size() != a.size() || want.size() != b.size()) return false;
    for (std::size_t k = 0; k < want.size(); ++k)
      if (!same_value(want[k], a[k]) || !same_value(want[k], b[k])) return false;
    return true;
  }
};

// ---- the Blueprint of a state (a class of the program there is one object of: its fields are the
// Blueprint's variables, its methods the Blueprint's functions) ----
/// Of a Blueprint of a state whose test holds none of its methods - only free functions that share the
/// program's variables: no object (the variables are the program's own).
struct NoObject {};
/// The variables as the source's object has them before a call: by name, as the Blueprint holds them.
using Variables = std::vector<std::pair<const char*, Value>>;
/// Is this a variable of the Blueprint that a call may change? (A field whose variable no part of the
/// Blueprint uses is none: no function of the Blueprint reads or changes it.)
inline bool changeable(const kern::logic::Instance& inst, const char* name) {
  const kern::logic::Variable* v = inst.graph().variable(name);
  return v && !v->constant;
}
/// The Blueprint's variables set to the source's: false - one of them does not take its value. (A value that
/// is not one of the variable's type - a field of another type somewhere in it - the Blueprint would hold as
/// the type's default: that is no value it takes.)
inline const char* g_untaken = "";   // (the variable that did not take its value)
inline bool set_state(kern::logic::Instance& inst, const Variables& vars) {
  for (const auto& [name, value] : vars) {
    if (!changeable(inst, name)) continue;
    const kern::logic::Variable* v = inst.graph().variable(name);
    if (!kern::logic::is_value_of(value, kern::logic::TypeRef{v->type, v->of, v->count}, inst.graph()) || !inst.set(name, value)) {
      g_untaken = name;
      return false;
    }
  }
  return true;
}
/// The variables after a call, as a struct of them in their order. Where the Blueprint has no such
/// variable, what it was before: the source must have left that field as it was.
inline Value read_state(const kern::logic::Instance& inst, const Variables& vars) {
  std::vector<Value> out;
  for (const auto& [name, value] : vars) out.push_back(changeable(inst, name) ? inst.get(name) : value);
  return Value::structure(std::move(out));
}

/// How much a value holds: one for every value in it (a struct, a list, a map with what is in them).
inline long long weight(const Value& v) {
  using kern::logic::PinType;
  long long w = 1;
  if (v.type == PinType::structure) {
    if (v.rows) for (const Value& r : *v.rows) w += weight(r);
  } else if (v.type == PinType::structure_list) {
    for (const Value& r : v.items<Value>()) w += weight(r);
  } else if (v.type == PinType::map) {
    for (std::size_t k = 0; k < v.size(); ++k) w += 1 + weight(v.keyed().value_at(k));
  } else if (v.type == PinType::set || kern::logic::is_list(v.type)) {
    w += static_cast<long long>(v.size());
  }
  return w;
}

/// The comparison of one function over its calls.
struct Tally {
  const char* function;
  int compared = 0, outside = 0, wrong = 0, class_only = 0;
  int costlier = 0;                  // calls in which the class asked for memory more often than the source
  int refused = 0;                   // calls not compared: the engine itself refused a call of the source (real_engine)
  std::string first_refused;         // (the first such call of the source: said in the note)
  long most_class = 0, with_source = 0;  // (the one with the most: how often the class, how often the source)
  long long most_passes_alone = 0;   // (of the calls compared with the class alone: the most loop passes of one)
  double class_seconds = 0, interpreter_seconds = 0, take_seconds = 0;   // (where the function's time went: said in its "time:" line)
  long long held = 0, passes = 0, most_passes = 0;     // (what the calls were given and their objects held; the loop passes of the class)
  long long work = 0;                                  // (both together: the work of the calls so far - see more())
  mutable int drawn = 0;                               // (the calls drawn: what more() was asked for)
  std::chrono::steady_clock::time_point began = std::chrono::steady_clock::now();
  explicit Tally(const char* f) : function(f) { cost::tracing = cost::where() && std::strcmp(cost::where(), f) == 0; }
  /// Is there another call to make? Up to kCalls compared ones; of a function whose calls are much work, as many
  /// as kWork has room for, but twenty at least. (What the source says nothing for is drawn again.)
  /// The work of a call is counted, not timed - a test gives the same verdict in every run, on any machine: the
  /// values it is given and its object holds (weight: what is drawn, handed to the Blueprints, read back and
  /// compared - one each) and the loop passes of the class (Runtime::budget counts them, the same in every tier) -
  /// one for ten in the class, a hundred for one in the interpreter. (In units of about a microsecond: measured
  /// with work-t1-vergleich-101036-32705 - a value about 1.3 µs, a pass of the class 0.12 µs, of the interpreter 90-280 µs.)
  bool more(int tried) const {
    drawn = tried;
    if (compared >= kCalls || tried >= kCalls * 20) return false;
    if (compared >= 20) return work < kWork;
    return compared < 5 || work < kLongWork;
  }
  static constexpr long long kWork = 1000000;
  /// A function whose calls are each a great deal of work (a number raised to a 256-bit power, limb by limb): five
  /// compared calls at least, and as many more, up to twenty, as kLongWork has room for.
  static constexpr long long kLongWork = 20000000;
  /// A call in whose class loops pass more often than this would take the interpreter minutes: it is compared with
  /// the class alone, and the function's line says so (~Tally).
  static constexpr long long kInterpreterPasses = 5000;
  double seconds() const { return std::chrono::duration<double>(std::chrono::steady_clock::now() - began).count(); }
  /// One call. `want`: what the source gives, computed with the Blueprint language's numbers;
  /// `left`: the source divided by zero or left the numbers on the way - nothing to compare.
  /// `order`: where the Blueprint's call has what `want` holds, part by part. A call by name gives what
  /// the function returns and then what it made of its reference inputs that are not const, in their
  /// order (Instance::call); the test compares the object first, then what the C++ returns, then what was
  /// written through the parameters. Empty: the call gives the one thing that is compared.
  /// `state`: of a function of the Blueprint of a state, its variables as the source's object had them
  /// before the call - both Blueprints start from them; of one that may change them (`changes`), what they
  /// are after the call stands behind the call's outputs (the last part `order` names).
  template <class Want, class Describe>
  void take(Blueprint& bp, const std::vector<Value>& args, const Want& want, bool left, Describe describe, const std::vector<int>& order = {}, const Variables* state = nullptr,
            bool changes = false) {
    struct Took {   // (the whole of take: what the Blueprints' variables, the calls and the comparing cost)
      double& sum;
      std::chrono::steady_clock::time_point at = std::chrono::steady_clock::now();
      ~Took() { sum += std::chrono::duration<double>(std::chrono::steady_clock::now() - at).count(); }
    } const took_all{take_seconds};
    if (!bp.interp || !bp.native) return;
    long long given = 0;
    for (const Value& v : args) given += weight(v);
    if (state) for (const auto& v : *state) given += weight(v.second);
    held += given;
    work += given;
    if (left || !defined(want)) { ++outside; return; }
    // The engine itself (SC_ENGINE=real) refused a call the source made: what the source got there is the
    // zeros of a call that did not happen, not what C++ says - nothing to compare; drawn again (the twenty
    // compared calls a function needs stay twenty). (A call refused on a side of the Blueprint only is in
    // its record of the engine's calls: that differs from the source's - see engine_same.)
    if (g_engine_refused > 0) {
      ++outside;
      if (++refused == 1) first_refused = g_engine_refused_first;
      return;
    }
    if (state && !(set_state(*bp.native, *state) && set_state(*bp.interp, *state))) {
      ++compared;
      if (++wrong <= 3) std::printf("  %s(%s): the Blueprint's variables do not take the values of the source's object (%s)\n", function, describe().c_str(), g_untaken);
      return;
    }
    const auto parts = [&](std::vector<Value>& given) {
      if (order.empty() || given.size() != order.size()) return;
      std::vector<Value> fields;
      for (int k : order) fields.push_back(given[static_cast<std::size_t>(k)]);
      given = {Value::structure(std::move(fields))};
    };
    // The compiled class first. A call whose loops pass very often (a sound of a hundred thousand samples, a
    // number raised to a 256-bit power) would take the interpreter minutes: such a call is compared with the
    // class alone, and the function's line says so (~Tally).
    std::vector<Value> a, b;
    bp.before_call(true);
    const auto before = std::chrono::steady_clock::now();
    const long source_asks = g_source_asks;
    g_class_asks = 0;
    const bool ib = bp.native->call(function, args, b);
    const long class_asks = g_class_asks;
    if (bp.held_lost()) { ++outside; return; }   // (C++ leaves open what a reference that goes nowhere gives: drawn again)
    const long long looped = bp.native->counters()[3];   // (the loop passes of this call - the runtime counts from zero for a call from outside)
    passes += looped;
    work += looped / 10;
    most_passes = std::max(most_passes, looped);
    class_seconds += std::chrono::duration<double>(std::chrono::steady_clock::now() - before).count();
    const bool both = looped <= kInterpreterPasses;
    if (both) work += looped * 100;
    if (both) bp.before_call(false);
    const auto interpreter_began = std::chrono::steady_clock::now();
    const bool ia = both ? bp.interp->call(function, args, a) : ib;
    if (both && bp.held_lost()) { ++outside; return; }
    interpreter_seconds += std::chrono::duration<double>(std::chrono::steady_clock::now() - interpreter_began).count();
    if (state && changes) {
      b.push_back(read_state(*bp.native, *state));
      if (both) a.push_back(read_state(*bp.interp, *state));
    }
    // what the three asked the engine: the same calls, in the same order, with the same values
    const std::vector<std::string>& asked = g_source_engine.calls;
    const bool engine_same = bp.calls(true) == asked && (!both || bp.calls(false) == asked);
    if (!engine_same) {
      ++compared;
      if (++wrong <= 3) {
        const std::vector<std::string>& got = bp.calls(true) == asked ? bp.calls(false) : bp.calls(true);
        std::size_t k = 0;
        while (k < asked.size() && k < got.size() && asked[k] == got[k]) ++k;
        std::printf("  %s(%s): call %zu of the engine - the source asks %s, the Blueprint %s (%zu and %zu calls)\n", function, describe().c_str(), k + 1,
                    k < asked.size() ? asked[k].c_str() : "nothing more", k < got.size() ? got[k].c_str() : "nothing more", asked.size(), got.size());
      }
      return;
    }
    if (!bp.actors_same(both)) {   // (what the call did to the Blueprints of the actors it calls on)
      ++compared;
      if (++wrong <= 3) std::printf("  %s(%s): the Blueprints of the actors it calls are not the same after the call in the source's world and in the Blueprint's\n", function, describe().c_str());
      return;
    }
    parts(b);
    if (both) parts(a);
    if (!both) {
      a = b;
      ++class_only;
      most_passes_alone = std::max(most_passes_alone, looped);
    }
    ++compared;
    if (class_asks > source_asks) {
      if (++costlier == 1 && cost::tracing) {
        // (SC_COST_WHERE names this function: from where the source and the class asked, in the first such call)
        std::printf("  cost of %s(%s):\n", function, describe().c_str());
        cost::print("the source", g_source_trace, source_asks);
        cost::print("the class", g_class_trace, class_asks);
      }
      if (class_asks - source_asks > most_class - with_source) most_class = class_asks, with_source = source_asks;
    }
    if (ia && ib && a.size() == 1 && b.size() == 1 && a[0] == b[0] && same(want, a[0]) && same(want, b[0])) return;
    if (++wrong <= 3) {
      std::printf("  %s(%s): the source gives %s, the Blueprint %s (interpreter) and %s (class)\n", function, describe().c_str(), text(want).c_str(),
                  !both ? "(not run)" : ia && a.size() == 1 ? shown(a[0]).c_str() : "nothing", ib && b.size() == 1 ? shown(b[0]).c_str() : "nothing");
      // the first place that differs, named: compared once more, writing down where (g_where_on)
      const auto where = [&](const Value& got) {
        g_where_on = true;
        g_where.clear();
        g_where_leaf.clear();
        const bool equal = same(want, got);
        g_where_on = false;
        return equal ? std::string("the same") : g_where_leaf.empty() ? std::string("(no place named)") : "at " + (g_where.empty() ? std::string("the whole") : g_where) + ": " + g_where_leaf;
      };
      if (both && ia && a.size() == 1) std::printf("    first difference (interpreter) %s\n", where(a[0]).c_str());
      if (ib && b.size() == 1) std::printf("    first difference (class) %s\n", where(b[0]).c_str());
    }
  }
  /// A call the source was given up in (open_read): counted with those it has nothing to compare for.
  void given_up() { ++outside; }
  ~Tally() {
    cost::tracing = false;
    // (a function that takes the test a second or more: where the time went - the class, the interpreter, setting and reading the
    // Blueprints' variables and comparing, or drawing the arguments and the object and the source's call)
    if (seconds() >= 1.0)
      std::printf("  time: %s: %.1f s (the class %.1f s, the interpreter %.1f s, the Blueprints' variables and comparing %.1f s, drawing and the source %.1f s) - %d calls drawn, %d compared; %lld values given and held, %lld loop passes of the class (up to %lld in a call)\n",
                  function, seconds(), class_seconds, interpreter_seconds, take_seconds - class_seconds - interpreter_seconds, seconds() - take_seconds, drawn, compared, held, passes,
                  most_passes);
    if (class_only > 0)
      std::printf("  note: %s: %d of %d calls compared with the compiled class only (a call in whose class loops pass more than %lld times - here up to %lld - is not run in the interpreter)\n",
                  function, class_only, compared, kInterpreterPasses, most_passes_alone);
    if (refused > 0)
      std::printf("  note: %s: %d calls drawn again - the engine itself refused a call the source made (%d compared; the first: %s)\n", function, refused, compared,
                  first_refused.c_str());
    SC_CHECK(wrong == 0, "%s: %d of %d calls differ", function, wrong, compared);
    if (costlier > 0)
      std::printf("  cost: %s: in %d of %d calls the class asks for memory more often than the source (up to %ld times instead of %ld)\n", function, costlier, compared, most_class, with_source);
    const bool long_calls = compared >= 5 && compared < 20 && work >= kLongWork;
    if (long_calls) std::printf("  note: %s: %d calls compared (their work: %lld values and loop passes; of a function whose calls are less work, twenty at least are)\n", function, compared, work);
    SC_CHECK(compared >= 20 || long_calls, "%s: only %d calls could be compared (in %d the source left the numbers)", function, compared, outside);
  }
};

// ---- the places C++ leaves open ----
// What an entry outside a list, a division of whole numbers by zero or a shift by the width of the number
// or more gives, C++ does not say. The source in a test goes through these there: they give the value where
// there is one and mark the call where there is none - such a call is then not compared (see Watch).
inline bool g_open = false;
// A source that reads outside a list has left what C++ defines; one that goes on doing so in a loop (a loop
// up to a length that is not there) would run for ever. After ten thousand such reads the call is given up:
// the test's function is where it was before the call (g_bail - see the generated tests), nothing is compared.
inline std::jmp_buf g_bail;
/// (What a call that is given up had on the heap stays there: the leak check does not count what the
/// source allocates while it may be given up.)
#if defined(SC_ASAN)
extern "C" void __lsan_disable();
extern "C" void __lsan_enable();
inline void may_bail(bool on) { on ? __lsan_disable() : __lsan_enable(); }
#else
inline void may_bail(bool) {}
#endif
inline bool g_can_bail = false;
inline long g_open_reads = 0;
inline void open_read() {
  g_open = true;
  if (g_can_bail && ++g_open_reads > 10000) {
    g_can_bail = false;
    std::longjmp(g_bail, 1);
  }
}

/// A place in the bytes of a text (`s.c_str() + n`, `s.data() + n` - CText: of c_str(), a C text): in the text or at its
/// end, where a std::string's null stands - as C++ defines it. Beyond the end, or before the start, C++ leaves open what a
/// read there gives (the bytes after the text: what the memory held): the call is marked (open_read), and the place is the
/// text's end - nothing of the text, and nothing behind it, is read from there. (The text is said once, as in the source.)
template <bool CText, class S, class N> auto text_end(S&& s, N n) {
  using M = std::remove_cvref_t<N>;
  static_assert(std::is_integral_v<M>, "a place in a text is a whole number of bytes from its start");
  const auto first = [&] {
    if constexpr (CText) return s.c_str();
    else return s.data();
  }();
  const unsigned long long size = s.size();
  const bool in = std::is_signed_v<M> ? n >= 0 && static_cast<unsigned long long>(n) <= size : static_cast<unsigned long long>(n) <= size;
  if (in) return first + n;
  open_read();
  return first + size;
}

/// What a std::unique_ptr or a std::optional holds, where the source reads it (`*p`, `p->x`): of one that holds
/// nothing C++ says nothing - the call is given up, as one that reads outside a list for ever (g_bail): it
/// is not compared. (In the Blueprint it is a list of no entry or one, its entry read where it stands.)
template <class P> P&& present(P&& p) {
  if (!p) {
    g_open = true;
    if (g_can_bail) {
      g_can_bail = false;
      std::longjmp(g_bail, 1);
    }
  }
  return std::forward<P>(p);
}

/// Does C++ say what the source's callback answers where the test asks it (the places of same_call)? The engine
/// calls it - in the test that is the comparison, after the source's call: what it does there is the source's as
/// well. One that reads where C++ leaves it open (an entry outside a list, a unique_ptr or optional that holds
/// nothing - present, open_read -) or leaves the numbers (a division by zero, a computation without a result)
/// gives nothing to compare: the call is not compared and drawn again, as one whose own body did that (Watch).
/// (Its body is given up where present finds nothing: g_bail, for this callback alone.)
template <class Args> bool callback_defined(const Value& w) {
  if (!w.call) return true;   // (unbound: C++ says - nothing is called)
  std::jmp_buf outer;
  std::memcpy(outer, g_bail, sizeof(std::jmp_buf));
  const bool could = g_can_bail, was_open = g_open;
  const long reads = g_open_reads;
  std::fexcept_t flags;
  std::fegetexceptflag(&flags, FE_ALL_EXCEPT);
  bool fine = true;
  for (int round = 0; round < 3 && fine; ++round) {
    const std::vector<Value> in = probe_values(static_cast<Args*>(nullptr), static_cast<std::size_t>(round));
    Value out;
    std::feclearexcept(FE_ALL_EXCEPT);
    g_open = false;
    g_open_reads = 0;
    if (setjmp(g_bail) != 0) {
      may_bail(false);
      fine = false;
      break;
    }
    g_can_bail = true;
    may_bail(true);
    const bool bound = w.call->call(in, out);
    g_can_bail = false;
    may_bail(false);
    if (g_open || std::fetestexcept(FE_DIVBYZERO | FE_INVALID) != 0) fine = false;
    if (!bound) break;
  }
  std::memcpy(g_bail, outer, sizeof(std::jmp_buf));
  g_can_bail = could;
  g_open = was_open;
  g_open_reads = reads;
  std::fesetexceptflag(&flags, FE_ALL_EXCEPT);
  return fine;
}
template <class R, class... A> bool defined(const std::function<R(A...)>& f) { return callback_defined<std::tuple<std::remove_cvref_t<A>...>>(arg(f)); }
template <FnAndUser S> bool defined(const S& s) { return callback_defined<typename CallbackSig<std::remove_cv_t<decltype(S::fn)>>::Args>(arg(s)); }

/// How many entries a list has: a container, an array - or a view (kern::Span says its length as a field).
template <class C> constexpr std::size_t entries(const C& c) {
  if constexpr (requires { std::size(c); }) {
    return std::size(c);
  } else {
    return c.size;
  }
}
/// What an entry the source reads where there is none is in the test (the call is marked - C++ leaves it open -, and
/// the source runs on with it): zero - and a C text the empty one, not null, which no function of texts takes
/// (`std::string(p)` throws, strlen reads nowhere): the source runs on to the end of the call.
template <class E> E none_entry() {
  if constexpr (std::is_pointer_v<E> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<E>>, char>) {
    static char empty[1] = {};
    return empty;
  } else {
    return E{};
  }
}
template <class T, class U> void copy_entry(T& to, const U& from) {
  if constexpr (std::is_array_v<T>) {
    for (std::size_t k = 0; k < std::extent_v<T>; ++k) copy_entry(to[k], from[k]);
  } else {
    to = from;
  }
}
template <class C, class I> auto at(C&& c, I i) -> decltype(c[0]) {
  using R = decltype(c[0]);
  const long long n = static_cast<long long>(entries(c)), k = static_cast<long long>(i);
  if (k >= 0 && k < n) return c[static_cast<std::size_t>(k)];
  open_read();
  if constexpr (std::is_reference_v<R>) {
    // (an entry that is an array - T[3][2] - too: every entry of it none_entry). Of a list that has entries: a copy of its
    // first one - also an entry of a struct with a C text in it (perf::Tier::name of `kTier[t]`) is then one the source
    // can go on with; what is written to it is not the list's.
    using E = std::remove_cvref_t<R>;
    static E none;
    if constexpr (std::is_copy_assignable_v<std::remove_all_extents_t<E>>) {
      if (n > 0) {
        copy_entry(none, c[0]);
        return none;
      }
    }
    fill_array(none, [] { return none_entry<std::remove_all_extents_t<E>>(); });
    return none;
  } else {
    static std::vector<bool> none(1);
    none[0] = false;
    return none[0];
  }
}
/// A number as a whole number of the type T: one T does not hold (or none at all) marks the call.
template <class T> constexpr double fits(double x);
template <> constexpr double fits<Whole>(double x) {
  if (!(x > -1.0 && x < 18446744073709551616.0)) {
    g_open = true;
    return 0;
  }
  return x;
}
template <> constexpr double fits<SignedWhole>(double x) {
  if (!(x > -9223372036854775809.0 && x < 9223372036854775808.0)) {
    g_open = true;
    return 0;
  }
  return x;
}
template <class T> constexpr double fits(double x) {
  const bool in = std::is_signed_v<T> ? (x > -9007199254740992.0 && x < 9007199254740992.0 && x >= static_cast<double>(std::numeric_limits<T>::min()) - 0.5 &&
                                         (sizeof(T) == 8 || x <= static_cast<double>(std::numeric_limits<T>::max()) + 0.5))
                                      : (x > -1.0 && x < 9007199254740992.0 && (sizeof(T) == 8 || x <= static_cast<double>(std::numeric_limits<T>::max()) + 0.5));
  if (!in) {   // (not a number: every comparison above is false)
    g_open = true;
    return 0;
  }
  return x;
}
/// A whole number as a value of an enum: one the enum has no value for marks the call.
template <class T> constexpr T among(std::initializer_list<long long> valid, T x) {
  for (long long v : valid)
    if (static_cast<long long>(x) == v) return x;
  g_open = true;
  return static_cast<T>(*valid.begin());
}
/// The length a list is made with. A list of more than 100000 entries is one the test does not build (a
/// negative count, read as the unsigned number C++ makes of it, is such a one).
template <class N> constexpr N count(N n) {
  if (static_cast<unsigned long long>(n) > 100000ULL) {
    g_open = true;
    return 0;
  }
  return n;
}
/// Taking out an entry a list does not have.
template <class C> void pop_back(C& c) {
  if (c.empty()) {
    g_open = true;
    return;
  }
  c.pop_back();
}
template <class C, class It> auto erase(C& c, It at) -> decltype(c.erase(at)) {
  if (at < c.begin() || !(at < c.end())) {
    g_open = true;
    return c.end();
  }
  return c.erase(at);
}
template <class C, class A, class B> auto erase(C& c, A first, B last) -> decltype(c.erase(first, last)) {
  if (first < c.begin() || c.end() < last || last < first) {
    g_open = true;
    return c.end();
  }
  return c.erase(first, last);
}
template <class C> auto front(C&& c) -> decltype(c[0]) { return at(c, 0); }
template <class C> auto back(C&& c) -> decltype(c[0]) { return at(c, static_cast<long long>(entries(c)) - 1); }

// A 64-bit whole number is, in a Blueprint, the number its bits are read with a sign - exact while it is
// within 2^53 of zero (unsigned C++ arithmetic around 2^64 comes out the same there). What the source
// computes with 64-bit numbers goes through these: a number out of that range marks the call.
template <class T> constexpr bool held(T v) {
  if constexpr (std::is_integral_v<T> && sizeof(T) == 8) {
    const long long read = static_cast<long long>(v);
    return read > -(1LL << 53) && read < (1LL << 53);
  } else {
    return true;
  }
}
template <class R> constexpr R kept(__int128 exact) {
  if (exact <= -(static_cast<__int128>(1) << 53) || exact >= (static_cast<__int128>(1) << 53)) g_open = true;
  return static_cast<R>(static_cast<unsigned long long>(exact));
}
template <class R, class A, class B> constexpr bool both_held(A a, B b) {
  if constexpr (std::is_integral_v<R> && sizeof(R) == 8) {
    if (!held(static_cast<R>(a)) || !held(static_cast<R>(b))) g_open = true;
    return true;
  } else {
    return false;
  }
}
// A whole number with a sign narrower than 64 bits (i32, and what is promoted to int): C++ leaves open what a
// sum, a difference, a product or a negation gives outside its range (UBSan: "signed integer overflow"). The
// test computes it exactly, marks the call there (open_read - it is given up and drawn again, never compared)
// and goes on with the bits the number keeps, so the source itself never does what C++ leaves open.
template <class R> constexpr bool narrow_signed() { return std::is_integral_v<R> && std::is_signed_v<R> && sizeof(R) < 8; }
template <class R> constexpr R kept_narrow(long long exact) {
  if (exact < static_cast<long long>(std::numeric_limits<R>::min()) || exact > static_cast<long long>(std::numeric_limits<R>::max())) open_read();
  return static_cast<R>(static_cast<std::make_unsigned_t<R>>(static_cast<unsigned long long>(exact)));
}
template <class A> constexpr auto neg(A a) -> decltype(-a) {
  using R = decltype(-a);
  if constexpr (std::is_integral_v<R> && std::is_signed_v<R>) {
    if (static_cast<R>(a) == std::numeric_limits<R>::min()) {
      open_read();
      return static_cast<R>(a);
    }
  }
  return -a;
}
template <class A, class B> constexpr auto plus(A a, B b) -> decltype(a + b) {
  using R = decltype(a + b);
  if constexpr (narrow_signed<R>()) return kept_narrow<R>(static_cast<long long>(a) + static_cast<long long>(b));
  if (both_held<R>(a, b)) return kept<R>(static_cast<__int128>(static_cast<long long>(static_cast<R>(a))) + static_cast<long long>(static_cast<R>(b)));
  return a + b;
}
template <class A, class B> constexpr auto minus(A a, B b) -> decltype(a - b) {
  using R = decltype(a - b);
  if constexpr (narrow_signed<R>()) return kept_narrow<R>(static_cast<long long>(a) - static_cast<long long>(b));
  if (both_held<R>(a, b)) return kept<R>(static_cast<__int128>(static_cast<long long>(static_cast<R>(a))) - static_cast<long long>(static_cast<R>(b)));
  return a - b;
}
template <class A, class B> constexpr auto times(A a, B b) -> decltype(a * b) {
  using R = decltype(a * b);
  if constexpr (narrow_signed<R>()) return kept_narrow<R>(static_cast<long long>(a) * static_cast<long long>(b));
  if (both_held<R>(a, b)) return kept<R>(static_cast<__int128>(static_cast<long long>(static_cast<R>(a))) * static_cast<long long>(static_cast<R>(b)));
  return a * b;
}
template <class A, class B> constexpr A& plus_to(A& a, B b) { return a = static_cast<A>(plus(a, b)); }
template <class A, class B> constexpr A& minus_to(A& a, B b) { return a = static_cast<A>(minus(a, b)); }
template <class A, class B> constexpr A& times_to(A& a, B b) { return a = static_cast<A>(times(a, b)); }
template <class A> constexpr A& pre_inc(A& a) { return a = static_cast<A>(plus(a, 1)); }
template <class A> constexpr A& pre_dec(A& a) { return a = static_cast<A>(minus(a, 1)); }
template <class A> constexpr A post_inc(A& a) { const A was = a; a = static_cast<A>(plus(a, 1)); return was; }
template <class A> constexpr A post_dec(A& a) { const A was = a; a = static_cast<A>(minus(a, 1)); return was; }

template <class A, class B> constexpr auto quot(A a, B b) -> decltype(a / b) {
  using R = decltype(a / b);
  both_held<R>(a, b);
  if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<long long>(b) == -1)) {
    g_open = true;
    return 0;
  }
  return a / b;
}
template <class A, class B> constexpr auto rem(A a, B b) -> decltype(a % b) {
  using R = decltype(a % b);
  both_held<R>(a, b);
  if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<long long>(b) == -1)) {
    g_open = true;
    return 0;
  }
  return a % b;
}
template <class A, class B> constexpr auto shl(A a, B b) -> decltype(a << b) {
  using R = decltype(a << b);
  const long long by = static_cast<long long>(b);
  if (by < 0 || by >= static_cast<long long>(sizeof(R) * 8)) {
    g_open = true;
    return 0;
  }
  if (both_held<R>(a, 0)) return kept<R>(static_cast<__int128>(static_cast<long long>(static_cast<R>(a))) * (static_cast<__int128>(1) << by));
  return a << b;
}
template <class A, class B> constexpr auto shr(A a, B b) -> decltype(a >> b) {
  using R = decltype(a >> b);
  const long long by = static_cast<long long>(b);
  both_held<R>(a, 0);
  if (by < 0 || by >= static_cast<long long>(sizeof(R) * 8)) {
    g_open = true;
    return 0;
  }
  return a >> b;
}
template <class A, class B> constexpr A& quot_to(A& a, B b) { return a = static_cast<A>(quot(a, b)); }
template <class A, class B> constexpr A& rem_to(A& a, B b) { return a = static_cast<A>(rem(a, b)); }
template <class A, class B> constexpr A& shl_to(A& a, B b) { return a = static_cast<A>(shl(a, b)); }
template <class A, class B> constexpr A& shr_to(A& a, B b) { return a = static_cast<A>(shr(a, b)); }

// A 64-bit bit pattern (the program's u64 and i64 - a Blueprint's Int64) is
// computed around 2^64, as C++ computes with unsigned numbers: nothing to mark there. What C++ leaves open
// marks the call: signed arithmetic that leaves the 64 bits, a division by zero, a shift by 64 or more.
template <class R> constexpr R kept_bits(__int128 exact) {
  if (exact < static_cast<__int128>(std::numeric_limits<R>::min()) || exact > static_cast<__int128>(std::numeric_limits<R>::max())) {
    g_open = true;
    return 0;
  }
  return static_cast<R>(exact);
}
template <class A, class B> constexpr auto plus_bits(A a, B b) -> decltype(a + b) {
  using R = decltype(a + b);
  if constexpr (std::is_signed_v<R>) {
    return kept_bits<R>(static_cast<__int128>(static_cast<R>(a)) + static_cast<__int128>(static_cast<R>(b)));
  } else {
    return a + b;
  }
}
template <class A, class B> constexpr auto minus_bits(A a, B b) -> decltype(a - b) {
  using R = decltype(a - b);
  if constexpr (std::is_signed_v<R>) {
    return kept_bits<R>(static_cast<__int128>(static_cast<R>(a)) - static_cast<__int128>(static_cast<R>(b)));
  } else {
    return a - b;
  }
}
template <class A, class B> constexpr auto times_bits(A a, B b) -> decltype(a * b) {
  using R = decltype(a * b);
  if constexpr (std::is_signed_v<R>) {
    return kept_bits<R>(static_cast<__int128>(static_cast<R>(a)) * static_cast<__int128>(static_cast<R>(b)));
  } else {
    return a * b;
  }
}
template <class A, class B> constexpr auto quot_bits(A a, B b) -> decltype(a / b) {
  using R = decltype(a / b);
  if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<long long>(b) == -1)) {
    g_open = true;
    return 0;
  }
  return a / b;
}
template <class A, class B> constexpr auto rem_bits(A a, B b) -> decltype(a % b) {
  using R = decltype(a % b);
  if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<long long>(b) == -1)) {
    g_open = true;
    return 0;
  }
  return a % b;
}
template <class A, class B> constexpr auto shl_bits(A a, B b) -> decltype(a << b) {
  using R = decltype(a << b);
  if (b < 0 || static_cast<unsigned long long>(b) >= sizeof(R) * 8) {
    g_open = true;
    return 0;
  }
  return static_cast<R>(static_cast<std::make_unsigned_t<R>>(a) << b);
}
template <class A, class B> constexpr auto shr_bits(A a, B b) -> decltype(a >> b) {
  using R = decltype(a >> b);
  if (b < 0 || static_cast<unsigned long long>(b) >= sizeof(R) * 8) {
    g_open = true;
    return 0;
  }
  return a >> b;
}
template <class A, class B> constexpr A& plus_bits_to(A& a, B b) { return a = static_cast<A>(plus_bits(a, b)); }
template <class A, class B> constexpr A& minus_bits_to(A& a, B b) { return a = static_cast<A>(minus_bits(a, b)); }
template <class A, class B> constexpr A& times_bits_to(A& a, B b) { return a = static_cast<A>(times_bits(a, b)); }
template <class A, class B> constexpr A& quot_bits_to(A& a, B b) { return a = static_cast<A>(quot_bits(a, b)); }
template <class A, class B> constexpr A& rem_bits_to(A& a, B b) { return a = static_cast<A>(rem_bits(a, b)); }
template <class A, class B> constexpr A& shl_bits_to(A& a, B b) { return a = static_cast<A>(shl_bits(a, b)); }
template <class A, class B> constexpr A& shr_bits_to(A& a, B b) { return a = static_cast<A>(shr_bits(a, b)); }
template <class A> constexpr A& pre_inc_bits(A& a) { return a = static_cast<A>(plus_bits(a, static_cast<A>(1))); }
template <class A> constexpr A& pre_dec_bits(A& a) { return a = static_cast<A>(minus_bits(a, static_cast<A>(1))); }
template <class A> constexpr A post_inc_bits(A& a) { const A was = a; a = static_cast<A>(plus_bits(a, static_cast<A>(1))); return was; }
template <class A> constexpr A post_dec_bits(A& a) { const A was = a; a = static_cast<A>(minus_bits(a, static_cast<A>(1))); return was; }

/// Runs the source and says whether it left the numbers on the way (see Tally::take): a division by zero
/// or a computation without a result, or one of the places above.
struct Watch {
  Watch() {
    std::feclearexcept(FE_ALL_EXCEPT);
    g_open = false;
    g_open_reads = 0;
    g_engine_refused = 0;
    g_source_engine.calls.clear();
    g_source_engine.kept.clear();   // (the Callbacks objects of the engine kept from an earlier call: each call begins with an engine that holds none - both sides alike)
    g_clocks = {&g_source_engine, {}};   // (what the engine's clocks answer the source in this call: the Blueprints get the same - rule_blueprint.h)
    if (real_engine()) fresh_engine(g_source_objects);
  }
  bool left() const { return g_open || std::fetestexcept(FE_DIVBYZERO | FE_INVALID) != 0; }
};

}  // namespace sc::converted
