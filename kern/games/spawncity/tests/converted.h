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
#include "kern/logic/nodes.h"
#include "cost.h"
#include "rule_blueprint.h"
#include "test_util.h"

namespace sc::converted {

using Value = kern::logic::Value;

/// A 64-bit bit pattern of the program (its u64, i64) as the test hands it to a Blueprint: the Blueprint's
/// Whole64, two 32-bit halves (tools/blueprint_whole64.h). In C++ a u64 and a size_t are one type; the
/// program's spelling tells them apart, and the generated test says which is which (shaped, below).
struct Whole {
  unsigned long long v = 0;
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
  constexpr Num32(double x) : v(static_cast<float>(x)) {}
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

// A map and a set as the Blueprint language has them: the rows in the order they were put in (a
// Blueprint's map is the list of its rows, its set the list of its keys). What std::unordered_map and
// std::unordered_set leave open - the order - is this order. They have what the program uses of the
// standard ones. (std::map and std::set keep their keys in order: they are themselves, SortedMap and
// SortedSet.) What C++ leaves open marks the call (g_open, below): at() of a key that is not there.
inline void mark_open();
template <class K, class V> class Map {
 public:
  using key_type = K;
  using mapped_type = V;
  using value_type = std::pair<const K, V>;
  using iterator = typename std::list<value_type>::iterator;
  using const_iterator = typename std::list<value_type>::const_iterator;
  using size_type = std::size_t;
  Map() = default;
  Map(std::initializer_list<value_type> rows) {
    for (const value_type& r : rows) insert(r);
  }
  Map(const Map& o) : rows_(o.rows_) {}
  Map(Map&&) = default;
  Map& operator=(const Map& o) {
    if (this != &o) {
      rows_.clear();
      for (const value_type& r : o.rows_) rows_.push_back(r);
    }
    return *this;
  }
  Map& operator=(Map&&) = default;
  iterator begin() { return rows_.begin(); }
  iterator end() { return rows_.end(); }
  const_iterator begin() const { return rows_.begin(); }
  const_iterator end() const { return rows_.end(); }
  const_iterator cbegin() const { return rows_.begin(); }
  const_iterator cend() const { return rows_.end(); }
  std::size_t size() const { return rows_.size(); }
  bool empty() const { return rows_.empty(); }
  void clear() { rows_.clear(); }
  void reserve(std::size_t) {}
  void rehash(std::size_t) {}
  iterator find(const K& k) {
    for (iterator it = rows_.begin(); it != rows_.end(); ++it)
      if (it->first == k) return it;
    return rows_.end();
  }
  const_iterator find(const K& k) const {
    for (const_iterator it = rows_.begin(); it != rows_.end(); ++it)
      if (it->first == k) return it;
    return rows_.end();
  }
  std::size_t count(const K& k) const { return find(k) != end() ? 1 : 0; }
  bool contains(const K& k) const { return find(k) != end(); }
  V& operator[](const K& k) { return try_emplace(k).first->second; }
  V& at(const K& k) {
    const iterator it = find(k);
    if (it != end()) return it->second;
    mark_open();
    static V none;
    none = V{};
    return none;
  }
  const V& at(const K& k) const { return const_cast<Map*>(this)->at(k); }
  template <class... A> std::pair<iterator, bool> try_emplace(const K& k, A&&... a) {
    const iterator it = find(k);
    if (it != end()) return {it, false};
    rows_.emplace_back(std::piecewise_construct, std::forward_as_tuple(k), std::forward_as_tuple(std::forward<A>(a)...));
    return {std::prev(rows_.end()), true};
  }
  template <class... A> std::pair<iterator, bool> emplace(A&&... a) { return put(value_type(std::forward<A>(a)...)); }
  std::pair<iterator, bool> insert(const value_type& r) { return put(r); }
  template <class P>
    requires std::is_constructible_v<value_type, P&&>
  std::pair<iterator, bool> insert(P&& r) { return put(value_type(std::forward<P>(r))); }
  template <class M> std::pair<iterator, bool> insert_or_assign(const K& k, M&& v) {
    const iterator it = find(k);
    if (it != end()) {
      it->second = std::forward<M>(v);
      return {it, false};
    }
    rows_.emplace_back(k, std::forward<M>(v));
    return {std::prev(rows_.end()), true};
  }
  std::size_t erase(const K& k) {
    const iterator it = find(k);
    if (it == end()) return 0;
    rows_.erase(it);
    return 1;
  }
  iterator erase(iterator it) { return rows_.erase(it); }
  iterator erase(const_iterator it) { return rows_.erase(it); }
  void swap(Map& o) { rows_.swap(o.rows_); }

 private:
  std::pair<iterator, bool> put(const value_type& r) {
    const iterator it = find(r.first);
    if (it != end()) return {it, false};
    rows_.push_back(r);
    return {std::prev(rows_.end()), true};
  }
  std::list<value_type> rows_;
};
template <class K> class Set {
 public:
  using key_type = K;
  using value_type = K;
  using iterator = typename std::list<K>::const_iterator;
  using const_iterator = iterator;
  Set() = default;
  Set(std::initializer_list<K> keys) {
    for (const K& k : keys) insert(k);
  }
  iterator begin() const { return keys_.begin(); }
  iterator end() const { return keys_.end(); }
  iterator cbegin() const { return keys_.begin(); }
  iterator cend() const { return keys_.end(); }
  std::size_t size() const { return keys_.size(); }
  bool empty() const { return keys_.empty(); }
  void clear() { keys_.clear(); }
  void reserve(std::size_t) {}
  void rehash(std::size_t) {}
  iterator find(const K& k) const {
    for (iterator it = keys_.begin(); it != keys_.end(); ++it)
      if (*it == k) return it;
    return keys_.end();
  }
  std::size_t count(const K& k) const { return find(k) != end() ? 1 : 0; }
  bool contains(const K& k) const { return find(k) != end(); }
  std::pair<iterator, bool> insert(const K& k) {
    const iterator it = find(k);
    if (it != end()) return {it, false};
    keys_.push_back(k);
    return {std::prev(keys_.end()), true};
  }
  template <class... A> std::pair<iterator, bool> emplace(A&&... a) { return insert(K(std::forward<A>(a)...)); }
  std::size_t erase(const K& k) {
    const iterator it = find(k);
    if (it == end()) return 0;
    keys_.erase(it);
    return 1;
  }
  iterator erase(iterator it) { return keys_.erase(it); }
  void swap(Set& o) { keys_.swap(o.keys_); }

 private:
  std::list<K> keys_;
};
template <class K, class V> class SortedMap : public std::map<K, V> {
 public:
  using std::map<K, V>::map;
  V& at(const K& k) {
    const auto it = this->find(k);
    if (it != this->end()) return it->second;
    mark_open();
    static V none;
    none = V{};
    return none;
  }
  const V& at(const K& k) const { return const_cast<SortedMap*>(this)->at(k); }
};
template <class K> using SortedSet = std::set<K>;
}  // namespace wide

/// A value of the source as the Blueprint holds it, where the two differ by more than their numbers: a
/// 64-bit bit pattern is a Whole, a map the list of its rows (pairs of key and value), a set the list of
/// its keys. S is the type as the program spells it, with Whole where it says u64 or i64.
template <class S> struct Shape {
  template <class T> static const T& of(const T& v) { return v; }
};
template <> struct Shape<Whole> {
  template <class T> static Whole of(T v) { return {static_cast<unsigned long long>(v)}; }
};
template <class S, class T> auto shaped(const T& v) { return Shape<S>::of(v); }
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
template <class SK, class SV, class M> auto shaped_rows(const M& m) {
  std::vector<std::pair<Shaped<SK, typename M::key_type>, Shaped<SV, typename M::mapped_type>>> rows;
  for (const auto& r : m) rows.emplace_back(Shape<SK>::of(r.first), Shape<SV>::of(r.second));
  return rows;
}
template <class SK, class M> auto shaped_keys(const M& m) {
  std::vector<Shaped<SK, typename M::key_type>> keys;
  for (const auto& k : m) keys.push_back(Shape<SK>::of(k));
  return keys;
}
template <class K, class V> struct Shape<wide::Map<K, V>> {
  template <class M> static auto of(const M& m) { return shaped_rows<K, V>(m); }
};
template <class K, class V> struct Shape<wide::SortedMap<K, V>> {
  template <class M> static auto of(const M& m) { return shaped_rows<K, V>(m); }
};
template <class K> struct Shape<wide::Set<K>> {
  template <class M> static auto of(const M& m) { return shaped_keys<K>(m); }
};
template <class K> struct Shape<std::set<K>> {
  template <class M> static auto of(const M& m) { return shaped_keys<K>(m); }
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
};

/// A list of up to four drawn entries; one of numbers is, one time in five, a long one (a function that
/// reads its list at fixed places - the cells of a grid - finds them there).
template <class Make> auto draw_list(Draw& draw, Make make) {
  std::vector<decltype(make())> out;
  u32 n = draw.below(5);
  if constexpr (std::is_arithmetic_v<decltype(make())>) {
    if (draw.below(5) == 0) n = 300 + draw.below(300);
  }
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
// engine_api.h). In a test both go to a stand-in that answers from the call itself and keeps every call:
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
/// What the source asked the engine in the call being compared.
inline kern::logic::EngineStandIn g_source_engine;
inline std::vector<Value> engine_call(const char* function, const std::vector<Value>& in) {
  std::vector<Value> out;
  if (const kern::logic::EngineFunction* f = kern::logic::engine_function(function))
    for (const kern::logic::EnginePin& p : f->outputs) out.push_back(Value::zero(p.type));
  g_source_engine.engine(function, in, out);
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

inline Value arg(f64 x) { return Value::number(x); }
inline Value arg(f32 x) { return Value::number(static_cast<f64>(x)); }
inline Value arg(bool x) { return Value::boolean(x); }
inline Value arg(Whole w) { return Value::structure({Value::integer(static_cast<kern::i64>(w.v >> 32)), Value::integer(static_cast<kern::i64>(w.v & 0xFFFFFFFFull))}); }
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
template <class E>
  requires std::is_enum_v<E>
Value arg(E e) { return Value::enumeration(static_cast<kern::i64>(e)); }
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
    for (const T& x : v) out.push_back(static_cast<kern::i64>(x));
    return Value::enumerations(std::move(out));
  } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
    return Value::of(std::vector<std::string>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec3> || std::is_same_v<T, wide::Vec3>) {
    return Value::of(std::vector<Vec3>(v.begin(), v.end()));
  } else if constexpr (std::is_same_v<T, Vec2> || std::is_same_v<T, wide::Vec2>) {
    std::vector<Vec3> out;
    for (const kern::Vec2 x : v) out.push_back({x.x, x.y, 0});
    return Value::of(std::move(out));
  } else if constexpr (is_list<T>) {   // (a list inside a list: a struct that holds it, as the Blueprint has it)
    std::vector<Value> out;
    for (const T& x : v) out.push_back(Value::structure({arg(x)}));
    return Value::structures(std::move(out));
  } else {
    std::vector<Value> out;
    for (const T& x : v) out.push_back(arg(x));
    return Value::structures(std::move(out));
  }
}

// ---- is what the Blueprint gives exactly what the source gives? ----
// (Numbers by their bits: 0.0 and -0.0 are two results.)
inline bool same_bits(f64 a, f64 b) { return std::bit_cast<kern::u64>(a) == std::bit_cast<kern::u64>(b); }
inline bool same(f64 want, const Value& got) { return same_bits(want, got.as_number()); }
inline bool same(f32 want, const Value& got) { return same_bits(static_cast<f64>(want), got.as_number()); }
inline bool same(bool want, const Value& got) { return got.as_bool() == want; }
inline bool same(Vec3 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return same_bits(want.x, v.x) && same_bits(want.y, v.y) && same_bits(want.z, v.z);
}
inline bool same(Vec2 want, const Value& got) {
  const Vec3 v = got.as_vector();
  return same_bits(want.x, v.x) && same_bits(want.y, v.y);
}
inline bool same(Whole want, const Value& got) {
  return got.rows && got.rows->size() == 2 && (*got.rows)[0].as_int() == static_cast<kern::i64>(want.v >> 32) && (*got.rows)[1].as_int() == static_cast<kern::i64>(want.v & 0xFFFFFFFFull);
}
inline bool same(wide::Vec3 want, const Value& got) { return same(kern::Vec3(want), got); }
inline bool same(wide::Vec2 want, const Value& got) { return same(kern::Vec2(want), got); }
inline bool same(const EngineObject* want, const Value& got) { return got.as_int() == (want ? want->kern_handle : 0); }
inline bool same(const EngineObject& want, const Value& got) { return got.as_int() == want.kern_handle; }
template <class T>
  requires std::is_base_of_v<EngineObject, T>
bool same(const std::unique_ptr<T>& want, const Value& got) { return same(static_cast<const EngineObject*>(want.get()), got); }
inline bool same(const char* want, const Value& got) { return got.as_text() == (want ? want : ""); }
inline bool same(const std::string& want, const Value& got) { return got.as_text() == want; }
inline bool same(std::string_view want, const Value& got) { return got.as_text() == want; }
template <class T>
  requires(std::is_integral_v<T> && !std::is_same_v<T, bool>)
bool same(T want, const Value& got) { return got.as_int() == static_cast<kern::i64>(want); }
template <class E>
  requires std::is_enum_v<E>
bool same(E want, const Value& got) { return got.i == static_cast<kern::i64>(want); }
template <class A, class B> bool same(const std::pair<A, B>& want, const Value& got) {
  return got.rows && got.rows->size() == 2 && same(want.first, (*got.rows)[0]) && same(want.second, (*got.rows)[1]);
}
template <class T> bool same(const std::vector<T>& want, const Value& got) {
  if constexpr (std::is_same_v<T, bool>) {
    const auto& g = got.items<bool>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (g[i] != want[i]) return false;
    return true;
  } else if constexpr (std::is_floating_point_v<T>) {
    const auto& g = got.items<f64>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same_bits(static_cast<f64>(want[i]), g[i])) return false;
    return true;
  } else if constexpr (std::is_integral_v<T> || std::is_enum_v<T>) {
    const auto& g = got.items<kern::i64>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (g[i] != static_cast<kern::i64>(want[i])) return false;
    return true;
  } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
    const auto& g = got.items<std::string>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (g[i] != want[i]) return false;
    return true;
  } else if constexpr (std::is_same_v<T, Vec3> || std::is_same_v<T, Vec2> || std::is_same_v<T, wide::Vec3> || std::is_same_v<T, wide::Vec2>) {
    const auto& g = got.items<Vec3>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same(want[i], Value::vector(g[i]))) return false;
    return true;
  } else if constexpr (is_list<T>) {
    const auto& g = got.items<Value>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!g[i].rows || g[i].rows->size() != 1 || !same(want[i], (*g[i].rows)[0])) return false;
    return true;
  } else {
    const auto& g = got.items<Value>();
    if (g.size() != want.size()) return false;
    for (std::size_t i = 0; i < want.size(); ++i) if (!same(want[i], g[i])) return false;
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
void take(E& to, const Value& v) { to = static_cast<E>(v.i); }
template <class T>
  requires std::is_base_of_v<EngineObject, T>
void take(T*& to, const Value& v) { to = engine_object<T>(v.as_int()); }
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
/// By the type as the program spells it (S, with Whole where it says u64 or i64 - see shaped).
template <class S> struct Take {
  template <class T> static void to(T& target, const Value& v) { take(target, v); }
};
template <> struct Take<Whole> {
  template <class T> static void to(T& target, const Value& v) {
    target = v.rows && v.rows->size() == 2 ? static_cast<T>((static_cast<unsigned long long>((*v.rows)[0].as_int()) << 32) | static_cast<unsigned long long>((*v.rows)[1].as_int())) : T{};
  }
};
template <class S> struct Take<std::vector<S>> {
  template <class T> static void to(std::vector<T>& target, const Value& v) {
    target.clear();
    for (std::size_t k = 0; k < v.size(); ++k) {
      T item = quiet<T>();
      if constexpr (is_list<S>) {   // (a list inside a list: a struct that holds it, as the Blueprint has it)
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
/// A map from its rows, a set from its keys.
template <class SK, class SV, class M> void take_rows(M& target, const Value& v) {
  target.clear();
  for (std::size_t k = 0; k < v.size(); ++k) {
    const Value row = v.item(k);
    if (!row.rows || row.rows->size() != 2) continue;
    typename M::key_type key{};
    typename M::mapped_type held{};
    Take<SK>::to(key, (*row.rows)[0]);
    Take<SV>::to(held, (*row.rows)[1]);
    target.insert_or_assign(std::move(key), std::move(held));
  }
}
template <class SK, class M> void take_keys(M& target, const Value& v) {
  target.clear();
  for (std::size_t k = 0; k < v.size(); ++k) {
    typename M::key_type key{};
    Take<SK>::to(key, v.item(k));
    target.insert(std::move(key));
  }
}
template <class K, class V> struct Take<wide::Map<K, V>> {
  template <class M> static void to(M& target, const Value& v) { take_rows<K, V>(target, v); }
};
template <class K, class V> struct Take<wide::SortedMap<K, V>> {
  template <class M> static void to(M& target, const Value& v) { take_rows<K, V>(target, v); }
};
template <class K> struct Take<wide::Set<K>> {
  template <class M> static void to(M& target, const Value& v) { take_keys<K>(target, v); }
};
template <class K> struct Take<std::set<K>> {
  template <class M> static void to(M& target, const Value& v) { take_keys<K>(target, v); }
};
template <class S, class T> void take_as(T& target, const Value& v) { Take<S>::to(target, v); }
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

/// A converted source file's Blueprint, in the interpreter and as its compiled class.
struct Blueprint {
  RuleBlueprint interpreted, compiled;
  explicit Blueprint(const char* actor) : interpreted(actor, [](kern::logic::Instance*) {}, false), compiled(actor, [](kern::logic::Instance*) {}, true) {
    // (The loop guard is the host's: a program whose functions loop long - a sound computed sample by
    // sample, a number raised to a 256-bit power - sets it to what they take.)
    interpreted.rt.budget = compiled.rt.budget = 2000000000u;
    SC_CHECK(interpreted.inst && compiled.inst, "%s: %s%s", actor, interpreted.problems.c_str(), compiled.problems.c_str());
    SC_CHECK(interpreted.problems.empty(), "%s: %s", actor, interpreted.problems.c_str());
    SC_CHECK(compiled.inst && std::string(compiled.inst->tier()) == "native C++", "%s runs as its C++ class (it runs: %s)", actor, compiled.inst ? compiled.inst->tier() : "not at all");
    kern::logic::native_probe = class_runs;
  }
};

// ---- the Blueprint of a state (a class of the program there is one object of: its fields are the
// Blueprint's variables, its methods the Blueprint's functions) ----
/// The variables as the source's object has them before a call: by name, as the Blueprint holds them.
using Variables = std::vector<std::pair<const char*, Value>>;
/// Is this a variable of the Blueprint that a call may change? (A field whose variable no part of the
/// Blueprint uses is none: no function of the Blueprint reads or changes it.)
inline bool changeable(const kern::logic::Instance& inst, const char* name) {
  const kern::logic::Variable* v = inst.graph().variable(name);
  return v && !v->constant;
}
/// The Blueprint's variables set to the source's: false - one of them does not take its value.
inline bool set_state(kern::logic::Instance& inst, const Variables& vars) {
  for (const auto& [name, value] : vars)
    if (changeable(inst, name) && !inst.set(name, value)) return false;
  return true;
}
/// The variables after a call, as a struct of them in their order. Where the Blueprint has no such
/// variable, what it was before: the source must have left that field as it was.
inline Value read_state(const kern::logic::Instance& inst, const Variables& vars) {
  std::vector<Value> out;
  for (const auto& [name, value] : vars) out.push_back(changeable(inst, name) ? inst.get(name) : value);
  return Value::structure(std::move(out));
}

/// The comparison of one function over its calls.
struct Tally {
  const char* function;
  int compared = 0, outside = 0, wrong = 0, class_only = 0;
  int costlier = 0;                  // calls in which the class asked for memory more often than the source
  long most_class = 0, with_source = 0;  // (the one with the most: how often the class, how often the source)
  double slowest = 0;
  std::chrono::steady_clock::time_point began = std::chrono::steady_clock::now();
  explicit Tally(const char* f) : function(f) { cost::tracing = cost::where() && std::strcmp(cost::where(), f) == 0; }
  /// Is there another call to make? Up to kCalls compared ones; of a function that takes long, as many
  /// as two seconds have room for, but twenty at least. (What the source says nothing for is drawn again.)
  bool more(int tried) const {
    if (compared >= kCalls || tried >= kCalls * 20) return false;
    if (compared >= 20) return seconds() < 2.0;
    return compared < 5 || seconds() < kLong;
  }
  /// A function whose calls take seconds each (a number raised to a 256-bit power, limb by limb): five
  /// compared calls at least, and as many more as twenty seconds have room for.
  static constexpr double kLong = 20.0;
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
    if (!bp.interpreted.inst || !bp.compiled.inst) return;
    if (left || !defined(want)) { ++outside; return; }
    if (state && !(set_state(*bp.compiled.inst, *state) && set_state(*bp.interpreted.inst, *state))) {
      ++compared;
      if (++wrong <= 3) std::printf("  %s(%s): the Blueprint's variables do not take the values of the source's object\n", function, describe().c_str());
      return;
    }
    const auto parts = [&](std::vector<Value>& given) {
      if (order.empty() || given.size() != order.size()) return;
      std::vector<Value> fields;
      for (int k : order) fields.push_back(given[static_cast<std::size_t>(k)]);
      given = {Value::structure(std::move(fields))};
    };
    // The compiled class first. A call that takes it long (a sound of a hundred thousand samples, a number
    // raised to a 256-bit power) would take the interpreter minutes: such a call is compared with the class
    // alone, and the function's line says so (~Tally).
    std::vector<Value> a, b;
    bp.compiled.host.stand_in.calls.clear();
    bp.interpreted.host.stand_in.calls.clear();
    const auto before = std::chrono::steady_clock::now();
    const long source_asks = g_source_asks;
    g_class_asks = 0;
    const bool ib = bp.compiled.inst->call(function, args, b);
    const long class_asks = g_class_asks;
    const double took = std::chrono::duration<double>(std::chrono::steady_clock::now() - before).count();
    const bool both = took < 0.005;
    const bool ia = both ? bp.interpreted.inst->call(function, args, a) : ib;
    if (state && changes) {
      b.push_back(read_state(*bp.compiled.inst, *state));
      if (both) a.push_back(read_state(*bp.interpreted.inst, *state));
    }
    // what the three asked the engine: the same calls, in the same order, with the same values
    const std::vector<std::string>& asked = g_source_engine.calls;
    const bool engine_same = bp.compiled.host.stand_in.calls == asked && (!both || bp.interpreted.host.stand_in.calls == asked);
    if (!engine_same) {
      ++compared;
      if (++wrong <= 3) {
        const std::vector<std::string>& got = bp.compiled.host.stand_in.calls == asked ? bp.interpreted.host.stand_in.calls : bp.compiled.host.stand_in.calls;
        std::size_t k = 0;
        while (k < asked.size() && k < got.size() && asked[k] == got[k]) ++k;
        std::printf("  %s(%s): call %zu of the engine - the source asks %s, the Blueprint %s (%zu and %zu calls)\n", function, describe().c_str(), k + 1,
                    k < asked.size() ? asked[k].c_str() : "nothing more", k < got.size() ? got[k].c_str() : "nothing more", asked.size(), got.size());
      }
      return;
    }
    parts(b);
    if (both) parts(a);
    if (!both) {
      a = b;
      ++class_only;
      slowest = std::max(slowest, took);
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
    if (++wrong <= 3)
      std::printf("  %s(%s): the source gives %s, the Blueprint %s (interpreter) and %s (class)\n", function, describe().c_str(), text(want).c_str(),
                  !both ? "(not run)" : ia && a.size() == 1 ? shown(a[0]).c_str() : "nothing", ib && b.size() == 1 ? shown(b[0]).c_str() : "nothing");
  }
  /// A call the source was given up in (open_read): counted with those it has nothing to compare for.
  void given_up() { ++outside; }
  ~Tally() {
    cost::tracing = false;
    if (class_only > 0)
      std::printf("  note: %s: %d of %d calls compared with the compiled class only (a call that takes the class more than 5 ms - here up to %.0f ms - is not run in the interpreter)\n",
                  function, class_only, compared, slowest * 1000);
    SC_CHECK(wrong == 0, "%s: %d of %d calls differ", function, wrong, compared);
    if (costlier > 0)
      std::printf("  cost: %s: in %d of %d calls the class asks for memory more often than the source (up to %ld times instead of %ld)\n", function, costlier, compared, most_class, with_source);
    const bool long_calls = compared >= 5 && compared < 20 && seconds() >= kLong;
    if (long_calls) std::printf("  note: %s: %d calls compared (they take %.0f s; of a function whose calls take less, twenty at least are)\n", function, compared, seconds());
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
#if defined(__SANITIZE_ADDRESS__)
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
inline void wide::mark_open() { g_open = true; }

/// How many entries a list has: a container, an array - or a view (kern::Span says its length as a field).
template <class C> constexpr std::size_t entries(const C& c) {
  if constexpr (requires { std::size(c); }) {
    return std::size(c);
  } else {
    return c.size;
  }
}
template <class C, class I> auto at(C&& c, I i) -> decltype(c[0]) {
  using R = decltype(c[0]);
  const long long n = static_cast<long long>(entries(c)), k = static_cast<long long>(i);
  if (k >= 0 && k < n) return c[static_cast<std::size_t>(k)];
  open_read();
  if constexpr (std::is_reference_v<R>) {
    static std::remove_cvref_t<R> none;   // (an entry that is an array - T[3][2] - too: every entry of it zero)
    fill_array(none, [] { return std::remove_all_extents_t<std::remove_cvref_t<R>>{}; });
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
template <class A, class B> constexpr auto plus(A a, B b) -> decltype(a + b) {
  using R = decltype(a + b);
  if (both_held<R>(a, b)) return kept<R>(static_cast<__int128>(static_cast<long long>(static_cast<R>(a))) + static_cast<long long>(static_cast<R>(b)));
  return a + b;
}
template <class A, class B> constexpr auto minus(A a, B b) -> decltype(a - b) {
  using R = decltype(a - b);
  if (both_held<R>(a, b)) return kept<R>(static_cast<__int128>(static_cast<long long>(static_cast<R>(a))) - static_cast<long long>(static_cast<R>(b)));
  return a - b;
}
template <class A, class B> constexpr auto times(A a, B b) -> decltype(a * b) {
  using R = decltype(a * b);
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

// A 64-bit bit pattern (the program's u64 and i64 - a Blueprint's Whole64, see tools/blueprint_whole64.h) is
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
    g_source_engine.calls.clear();
  }
  bool left() const { return g_open || std::fetestexcept(FE_DIVBYZERO | FE_INVALID) != 0; }
};

}  // namespace sc::converted
