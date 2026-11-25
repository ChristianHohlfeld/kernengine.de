// What the generated tests of tests/lowered/ share (tools/source_diff.py writes them).
//
// A source file of the game was brought into the C++ subset (BLUEPRINTS.md, "Absenkung"): rewritten
// so that the converter writes every function of it, with the same behaviour and the same costs. The
// test of such a change holds the file twice - its old text (the commit the change started from) and
// its new text, each in a namespace of its own (sc_old, sc_new), compiled as the game compiles them:
// the program's own numbers. Every function the change touched is called in both with the same drawn
// state and the same drawn arguments (the draws of the converted tests: converted::Draw), and what the
// two give and leave must be the same, bit for bit: what the call returns, every parameter it may
// change, its object, the variables of the file (or the state they became - see source_diff.py) and the
// Blueprints of the actors it calls. There is no margin: 0.0 and -0.0 are two results, so are two NaNs
// with other bits. The new does not ask for memory more often than the old, in no call.
//
// What C++ leaves open - an entry outside a list, a whole number divided by zero or shifted by its
// width or more, a signed whole number that leaves its type, a number made a whole number of a type
// that does not hold it, a list of more entries than a test builds - goes through the guards of the
// converted tests (converted::at, count ...) and the few here; a call that meets one has no result in
// C++ to compare with, and is counted, not compared (as in the converted tests).
#pragma once

#include <bit>
#include <climits>
#include <concepts>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "converted.h"
#include "kern/core/fields.h"
#include "kern/core/map.h"

namespace sc::lowered {

using converted::Draw;

// ---- what a value is made of ----
/// The fields of a class of the changed file, as the test names them (it specializes this for each).
template <class T> struct Fields;
template <class T> concept Named = requires(T& x) { Fields<std::remove_const_t<T>>::of(x); };
/// The values an enum has (the test specializes this for each enum it draws).
template <class E> struct Values;

template <class T> struct is_vector : std::false_type {};
template <class T, class A> struct is_vector<std::vector<T, A>> : std::true_type {};
template <class T> struct is_array : std::false_type {};
template <class T, std::size_t N> struct is_array<std::array<T, N>> : std::true_type {};
template <class T> struct is_span : std::false_type {};
template <class T> struct is_span<kern::Span<T>> : std::true_type {};
template <class T> struct is_optional : std::false_type {};
template <class T> struct is_optional<std::optional<T>> : std::true_type {};
template <class T> struct is_pair : std::false_type {};
template <class A, class B> struct is_pair<std::pair<A, B>> : std::true_type {};
template <class T> struct is_map : std::false_type {};
template <class K, class V, class H> struct is_map<kern::Map<K, V, H>> : std::true_type {};
template <class T> struct is_set : std::false_type {};
template <class K, class H> struct is_set<kern::Set<K, H>> : std::true_type {};
template <class T> struct is_owned : std::false_type {};
template <class T, class D> struct is_owned<std::unique_ptr<T, D>> : std::true_type {};
/// An object held from outside - by a pointer or owned (std::unique_ptr): the engine's physics world, a server.
template <class T> inline constexpr bool is_held = (std::is_pointer_v<T> && std::is_class_v<std::remove_cv_t<std::remove_pointer_t<T>>>) || is_owned<T>::value;
/// The class of the object such a value holds.
template <class T> using Pointee = std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<T&>())>>;
/// The call the test is at (Tally::more counts): what the test makes for one call lives until the next begins.
inline unsigned long long g_call = 0;
/// An object the test makes for a pointer of this call to point at - by its constructor, each side its own.
template <class T> T* made() {
  static std::vector<std::unique_ptr<T>> all;
  static unsigned long long of = 0;
  if (of != g_call) {
    all.clear();
    of = g_call;
  }
  all.push_back(std::make_unique<T>());
  return all.back().get();
}
template <class T> inline constexpr bool is_vec =
    std::is_same_v<T, kern::Vec2> || std::is_same_v<T, kern::Vec3> || std::is_same_v<T, kern::Vec4> || std::is_same_v<T, kern::Quat>;
template <class T> inline constexpr bool is_text = std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>;

/// A struct's fields, in their order: those the test names (Fields), else those of a plain struct.
template <class T> decltype(auto) fields(T& x) {
  if constexpr (Named<T>) return Fields<std::remove_const_t<T>>::of(x);
  else return kern::fields_of(x);
}
template <class T> concept HasFields = Named<T> || kern::PlainStruct<std::remove_const_t<T>>;

template <class V> auto components(V& v) {
  using U = std::remove_const_t<V>;
  if constexpr (std::is_same_v<U, kern::Vec2>) return std::tie(v.x, v.y);
  else if constexpr (std::is_same_v<U, kern::Vec3>) return std::tie(v.x, v.y, v.z);
  else return std::tie(v.x, v.y, v.z, v.w);
}

// ---- drawn: a value of any type the source takes, with the converted tests' draws ----
template <class T> void draw(Draw& d, T& x);
template <class T> T drawn(Draw& d) {
  T x{};
  draw(d, x);
  return x;
}
template <class T> void draw(Draw& d, T& x) {
  using U = std::remove_cv_t<T>;
  if constexpr (std::is_same_v<U, bool>) x = d.flag();
  else if constexpr (std::is_enum_v<U>) {
    if constexpr (requires { Values<U>::all; }) x = static_cast<U>(Values<U>::all[d.below(static_cast<u32>(std::size(Values<U>::all)))]);
    else static_assert(sizeof(U) == 0, "an enum whose values the test does not know (source_diff.py names them)");
  } else if constexpr (std::is_integral_v<U>) x = static_cast<U>(d.whole());
  else if constexpr (std::is_floating_point_v<U>) x = static_cast<U>(d.number());
  else if constexpr (is_vec<U>) std::apply([&](auto&... c) { ((c = static_cast<f32>(d.number())), ...); }, components(x));
  else if constexpr (std::is_same_v<U, std::string>) x = d.word();
  else if constexpr (std::is_same_v<U, const char*>) x = d.c_word();   // (a text that lasts: one of the draws' words, never none)
  else if constexpr (is_vector<U>::value) {
    using E = typename U::value_type;
    if constexpr (std::is_same_v<E, bool>) {
      x.clear();
      for (u32 n = d.below(5); n > 0; --n) x.push_back(d.flag());
    } else {
      x = converted::draw_list(d, [&] { return drawn<E>(d); });
    }
  } else if constexpr (is_array<U>::value || std::is_array_v<U>) {
    for (auto& e : x) draw(d, e);
  } else if constexpr (is_optional<U>::value) {
    if (d.flag()) x = drawn<typename U::value_type>(d);
    else x.reset();
  } else if constexpr (is_pair<U>::value) {
    draw(d, x.first);
    draw(d, x.second);
  } else if constexpr (is_map<U>::value) {
    // (a map: a few entries, in the order their keys are drawn - a key drawn twice is one entry. Made as carry
    // makes the other side's: a new map with room for just its entries - the two then ask for memory alike)
    U all;
    for (u32 n = d.below(5); n > 0; --n) {
      auto key = drawn<typename U::key_type>(d);
      all.try_emplace(std::move(key), drawn<typename U::mapped_type>(d));
    }
    x = U{};
    x.reserve(all.size());
    for (std::size_t k = 0; k < all.size(); ++k) x.try_emplace(all.key_at(k), std::move(all.value_at(k)));
  } else if constexpr (is_set<U>::value) {
    U all;
    for (u32 n = d.below(5); n > 0; --n) all.insert(drawn<typename U::key_type>(d));
    x = U{};
    x.reserve(all.size());
    for (std::size_t k = 0; k < all.size(); ++k) x.insert(all.key_at(k));
  } else if constexpr (is_held<U>) {
    // an object held from outside (a pointer, a std::unique_ptr): one the test makes, each side its own - drawn
    // where the test can draw its class, else as its constructor makes it. Never none: a function that is handed
    // such an object by a pointer reads it. (A class only its own constructor's arguments make - a server on a
    // transport - the test cannot make: none, on both sides.) The test says so once.
    using O = Pointee<U>;
    if constexpr (std::is_default_constructible_v<O>) {
      static const bool said = (std::printf("  note: %s: an object held by a pointer: one the test makes, each side its own (never none)\n", __PRETTY_FUNCTION__), true);
      (void)said;
      if constexpr (is_owned<U>::value) {
        x = std::make_unique<O>();
        draw(d, *x);
      } else {
        O* made_here = made<O>();
        draw(d, *made_here);
        x = made_here;
      }
    } else {
      static const bool said = (std::printf("  note: %s: an object held by a pointer that the test cannot make: none (null) on both sides\n", __PRETTY_FUNCTION__), true);
      (void)said;
      x = nullptr;
    }
  } else if constexpr (HasFields<U>) {
    std::apply([&](auto&... f) { (draw(d, f), ...); }, fields(x));
  } else if constexpr (std::is_default_constructible_v<U> && std::is_class_v<U>) {
    // a class of another file that keeps what it holds to itself (the engine's kern::FootprintStream, the game's
    // CityStream): made by its constructor - the test says so once. (One that cannot be assigned stays as the
    // test made it right before: by its constructor.)
    static const bool said = (std::printf("  note: %s: made by its constructor, not drawn (it keeps what it holds to itself)\n", __PRETTY_FUNCTION__), true);
    (void)said;
    if constexpr (std::is_move_assignable_v<U>) x = U{};
  } else {
    static_assert(sizeof(U) == 0, "a value the test cannot draw");
  }
}

// ---- carried: the same value in the type of the other side (a class of the file is two classes, sc_old's
// and sc_new's - field by field, by name: source_diff.py checks that they have the same fields). A list and a
// text get the room the old side's has (its capacity): what the two ask for memory is then compared alike ----
template <class A, class B> void carry(const A& a, B& b) {
  using X = std::remove_cv_t<A>;
  using Y = std::remove_cv_t<B>;
  if constexpr (std::is_same_v<X, Y> && std::is_trivially_copyable_v<X> && !Named<X> && !std::is_array_v<X>) b = a;
  else if constexpr (std::is_same_v<X, std::string> && std::is_same_v<Y, std::string>) {
    b.clear();
    b.reserve(a.capacity());
    b.append(a);
  } else if constexpr (std::is_enum_v<X> && std::is_enum_v<Y>) b = static_cast<Y>(static_cast<std::underlying_type_t<X>>(a));
  else if constexpr (std::is_arithmetic_v<X> && std::is_arithmetic_v<Y>) {
    static_assert(std::is_same_v<X, Y>, "a number of another type");
    b = a;
  } else if constexpr (is_vector<X>::value && is_vector<Y>::value) {
    b.clear();
    b.shrink_to_fit();
    b.reserve(a.capacity());
    b.resize(a.size());
    for (std::size_t k = 0; k < a.size(); ++k) {
      if constexpr (std::is_same_v<typename Y::value_type, bool>) b[k] = a[k];
      else carry(a[k], b[k]);
    }
  } else if constexpr ((is_array<X>::value || std::is_array_v<X>) && (is_array<Y>::value || std::is_array_v<Y>)) {
    for (std::size_t k = 0; k < std::size(a) && k < std::size(b); ++k) carry(a[k], b[k]);
  } else if constexpr (is_optional<X>::value && is_optional<Y>::value) {
    if (a) carry(*a, b.emplace());
    else b.reset();
  } else if constexpr (is_pair<X>::value && is_pair<Y>::value) {
    carry(a.first, b.first);
    carry(a.second, b.second);
  } else if constexpr (is_map<X>::value && is_map<Y>::value) {
    b = Y{};
    b.reserve(a.size());
    for (std::size_t k = 0; k < a.size(); ++k) {
      typename Y::key_type key{};
      typename Y::mapped_type value{};
      carry(a.key_at(k), key);
      carry(a.value_at(k), value);
      b.try_emplace(std::move(key), std::move(value));
    }
  } else if constexpr (is_set<X>::value && is_set<Y>::value) {
    b = Y{};
    b.reserve(a.size());
    for (std::size_t k = 0; k < a.size(); ++k) {
      typename Y::key_type key{};
      const typename X::key_type from = a.key_at(k);
      carry(from, key);
      b.insert(std::move(key));
    }
  } else if constexpr (is_held<X> && is_held<Y>) {
    // (the other side's own object, with what the first side's holds: see draw)
    using O = Pointee<Y>;
    if (a == nullptr) {
      b = nullptr;
    } else if constexpr (std::is_default_constructible_v<O>) {
      if constexpr (is_owned<Y>::value) {
        b = std::make_unique<O>();
        carry(*a, *b);
      } else {
        O* made_here = made<O>();
        carry(*a, *made_here);
        b = made_here;
      }
    } else {
      b = nullptr;
    }
  } else if constexpr (std::is_same_v<X, Y> && std::is_class_v<X> && !HasFields<X> && std::is_copy_assignable_v<X>) {
    b = a;   // (a class that keeps what it holds to itself: the same type on both sides)
  } else if constexpr (std::is_same_v<X, Y> && std::is_class_v<X> && !HasFields<X> && std::is_default_constructible_v<X>) {
    // (one that cannot be copied - it holds objects of the engine: each side has its own, as its constructor made it)
  } else if constexpr (HasFields<X> && HasFields<Y>) {
    auto from = fields(a);
    auto to = fields(b);
    static_assert(std::tuple_size_v<decltype(from)> == std::tuple_size_v<decltype(to)>, "classes of other fields");
    [&]<std::size_t... I>(std::index_sequence<I...>) { (carry(std::get<I>(from), std::get<I>(to)), ...); }(std::make_index_sequence<std::tuple_size_v<decltype(from)>>{});
  } else {
    static_assert(sizeof(X) == 0, "a value the test cannot carry to the other side");
  }
}

// ---- the same, bit for bit ----
template <class A, class B> bool same(const A& a, const B& b) {
  using X = std::remove_cv_t<A>;
  using Y = std::remove_cv_t<B>;
  if constexpr (std::is_floating_point_v<X>) {
    static_assert(std::is_same_v<X, Y>, "a number of another type");
    if constexpr (sizeof(X) == 4) return std::bit_cast<u32>(a) == std::bit_cast<u32>(b);
    else if constexpr (sizeof(X) == 8) return std::bit_cast<u64>(a) == std::bit_cast<u64>(b);
    else return std::memcmp(&a, &b, sizeof a) == 0;
  } else if constexpr (std::is_enum_v<X> && std::is_enum_v<Y>) {
    return static_cast<long long>(a) == static_cast<long long>(b);
  } else if constexpr (std::is_arithmetic_v<X>) {
    static_assert(std::is_same_v<X, Y>, "a number of another type");
    return a == b;
  } else if constexpr (is_text<X> && is_text<Y>) {
    return std::string_view(a) == std::string_view(b);
  } else if constexpr (std::is_same_v<X, const char*> && std::is_same_v<Y, const char*>) {
    return (a == nullptr) == (b == nullptr) && (a == nullptr || std::strcmp(a, b) == 0);
  } else if constexpr (is_vec<X> && std::is_same_v<X, Y>) {
    auto ca = components(a);
    auto cb = components(b);
    return [&]<std::size_t... I>(std::index_sequence<I...>) { return (same(std::get<I>(ca), std::get<I>(cb)) && ...); }(std::make_index_sequence<std::tuple_size_v<decltype(ca)>>{});
  } else if constexpr ((is_vector<X>::value || is_span<X>::value) && (is_vector<Y>::value || is_span<Y>::value)) {
    if (std::size(a) != std::size(b)) return false;
    for (std::size_t k = 0; k < std::size(a); ++k) {
      if constexpr (std::is_same_v<typename X::value_type, bool>) {
        if (static_cast<bool>(a[k]) != static_cast<bool>(b[k])) return false;
      } else if (!same(a[k], b[k])) {
        return false;
      }
    }
    return true;
  } else if constexpr ((is_array<X>::value || std::is_array_v<X>) && (is_array<Y>::value || std::is_array_v<Y>)) {
    if (std::size(a) != std::size(b)) return false;
    for (std::size_t k = 0; k < std::size(a); ++k)
      if (!same(a[k], b[k])) return false;
    return true;
  } else if constexpr (is_optional<X>::value && is_optional<Y>::value) {
    return a.has_value() == b.has_value() && (!a || same(*a, *b));
  } else if constexpr (is_pair<X>::value && is_pair<Y>::value) {
    return same(a.first, b.first) && same(a.second, b.second);
  } else if constexpr (is_map<X>::value && is_map<Y>::value) {
    // (the entries in their order: a map is gone through in the order its keys came)
    if (a.size() != b.size()) return false;
    for (std::size_t k = 0; k < a.size(); ++k)
      if (!same(a.key_at(k), b.key_at(k)) || !same(a.value_at(k), b.value_at(k))) return false;
    return true;
  } else if constexpr (is_set<X>::value && is_set<Y>::value) {
    if (a.size() != b.size()) return false;
    for (std::size_t k = 0; k < a.size(); ++k) {
      const typename X::key_type ka = a.key_at(k);
      const typename Y::key_type kb = b.key_at(k);
      if (!same(ka, kb)) return false;
    }
    return true;
  } else if constexpr (is_held<X> && is_held<Y>) {
    // (whether each side holds an object, and what the two objects hold)
    return (a == nullptr) == (b == nullptr) && (a == nullptr || same(*a, *b));
  } else if constexpr (HasFields<X> && HasFields<Y>) {
    auto fa = fields(a);
    auto fb = fields(b);
    if constexpr (std::tuple_size_v<decltype(fa)> != std::tuple_size_v<decltype(fb)>) return false;
    else return [&]<std::size_t... I>(std::index_sequence<I...>) { return (same(std::get<I>(fa), std::get<I>(fb)) && ...); }(std::make_index_sequence<std::tuple_size_v<decltype(fa)>>{});
  } else if constexpr (std::is_same_v<X, Y> && std::is_class_v<X> && requires { { a == b } -> std::convertible_to<bool>; }) {
    return a == b;   // (a class that keeps what it holds to itself and says itself when two are the same)
  } else if constexpr (std::is_same_v<X, Y> && std::is_class_v<X>) {
    // a class of another file that keeps what it holds to itself and has no == (the engine's physics world, the
    // game's CityStream): what a call left in it is not compared - the test says so once
    static const bool said = (std::printf("  note: %s: what it holds is its own - not compared\n", __PRETTY_FUNCTION__), true);
    (void)said;
    return true;
  } else {
    static_assert(sizeof(X) == 0, "a value the test cannot compare");
  }
}

// ---- an argument: what a parameter of type P is given, held by the test ----
/// A view (std::string_view, kern::Span, const char*) is given what the test holds: a text, a list.
template <class P> struct Arg {
  using Bare = std::remove_cvref_t<P>;
  static constexpr bool view_text = std::is_same_v<Bare, std::string_view> || std::is_same_v<Bare, const char*>;
  using Held = std::conditional_t<view_text, std::string, Bare>;
  Held v{};
  decltype(auto) get() {
    if constexpr (std::is_same_v<Bare, std::string_view>) return std::string_view(v);
    else if constexpr (std::is_same_v<Bare, const char*>) return v.c_str();
    else return (v);
  }
};
template <class T> struct Arg<kern::Span<T>> {
  std::vector<std::remove_const_t<T>> v;
  kern::Span<T> get() { return kern::Span<T>(v.data(), v.size()); }
};
template <class T> struct Arg<const kern::Span<T>&> : Arg<kern::Span<T>> {};
template <class T> struct Arg<const kern::Span<T>> : Arg<kern::Span<T>> {};
template <class P> void draw(Draw& d, Arg<P>& a) { draw(d, a.v); }
template <class P, class Q> void carry(const Arg<P>& a, Arg<Q>& b) { carry(a.v, b.v); }
template <class P, class Q> bool same(const Arg<P>& a, const Arg<Q>& b) { return same(a.v, b.v); }

// ---- the places C++ leaves open that the converted tests' guards do not name (they hold a number of a
// Blueprint there: exact up to 2^53) - here as C++ says it for the program's own numbers ----
template <class A, class B> constexpr auto quot(A a, B b) -> decltype(a / b) {
  using R = decltype(a / b);
  if constexpr (std::is_integral_v<R>) {
    if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<R>(b) == static_cast<R>(-1))) {
      converted::g_open = true;
      return 0;
    }
  }
  return a / b;
}
template <class A, class B> constexpr auto rem(A a, B b) -> decltype(a % b) {
  using R = decltype(a % b);
  if (b == 0 || (std::is_signed_v<R> && static_cast<R>(a) == std::numeric_limits<R>::min() && static_cast<R>(b) == static_cast<R>(-1))) {
    converted::g_open = true;
    return 0;
  }
  return a % b;
}
template <class A, class B> constexpr auto shl(A a, B b) -> decltype(a << b) {
  using R = decltype(a << b);
  if (static_cast<long long>(b) < 0 || static_cast<unsigned long long>(b) >= sizeof(R) * CHAR_BIT) {
    converted::g_open = true;
    return 0;
  }
  return a << b;
}
template <class A, class B> constexpr auto shr(A a, B b) -> decltype(a >> b) {
  using R = decltype(a >> b);
  if (static_cast<long long>(b) < 0 || static_cast<unsigned long long>(b) >= sizeof(R) * CHAR_BIT) {
    converted::g_open = true;
    return 0;
  }
  return a >> b;
}
template <class A, class B> A& quot_to(A& a, B b) { return a = static_cast<A>(quot(a, b)); }
template <class A, class B> A& rem_to(A& a, B b) { return a = static_cast<A>(rem(a, b)); }
template <class A, class B> A& shl_to(A& a, B b) { return a = static_cast<A>(shl(a, b)); }
template <class A, class B> A& shr_to(A& a, B b) { return a = static_cast<A>(shr(a, b)); }
/// A signed whole number that leaves its type.
template <class A, class B> constexpr auto plus(A a, B b) -> decltype(a + b) {
  using R = decltype(a + b);
  R r{};
  if (__builtin_add_overflow(static_cast<R>(a), static_cast<R>(b), &r)) converted::g_open = true;
  return r;
}
template <class A, class B> constexpr auto minus(A a, B b) -> decltype(a - b) {
  using R = decltype(a - b);
  R r{};
  if (__builtin_sub_overflow(static_cast<R>(a), static_cast<R>(b), &r)) converted::g_open = true;
  return r;
}
template <class A, class B> constexpr auto times(A a, B b) -> decltype(a * b) {
  using R = decltype(a * b);
  R r{};
  if (__builtin_mul_overflow(static_cast<R>(a), static_cast<R>(b), &r)) converted::g_open = true;
  return r;
}
template <class A, class B> A& plus_to(A& a, B b) { return a = static_cast<A>(plus(a, b)); }
template <class A, class B> A& minus_to(A& a, B b) { return a = static_cast<A>(minus(a, b)); }
template <class A, class B> A& times_to(A& a, B b) { return a = static_cast<A>(times(a, b)); }
template <class A> A& pre_inc(A& a) { return a = static_cast<A>(plus(a, 1)); }
template <class A> A& pre_dec(A& a) { return a = static_cast<A>(minus(a, 1)); }
template <class A> A post_inc(A& a) { const A was = a; a = static_cast<A>(plus(a, 1)); return was; }
template <class A> A post_dec(A& a) { const A was = a; a = static_cast<A>(minus(a, 1)); return was; }
template <class A> A negated(A a) { return static_cast<A>(minus(A{0}, a)); }
/// A number made a whole number of the type T: one T does not hold (or none at all) marks the call.
template <class T, class F> constexpr F fits(F x) {
  // (exact: the bounds are powers of two, which F holds - the largest T plus one, the smallest T)
  constexpr long double hi = static_cast<long double>(std::numeric_limits<T>::max()) + 1.0L;
  constexpr long double lo = std::is_signed_v<T> ? static_cast<long double>(std::numeric_limits<T>::min()) : 0.0L;
  const long double v = static_cast<long double>(x);
  if (!(v > lo - 1.0L && v < hi)) {
    converted::g_open = true;
    return F{0};
  }
  return x;
}

// ---- a copy of bytes between places of lists (std::memcpy / std::memmove of the source): checked against the
// lists the source names - a place outside a list, or more bytes than are there, marks the call ----
struct Place {
  const void* at = nullptr;
  std::size_t room = 0;   // the bytes from there to the end of the list
  bool there = false;
};
template <class L> Place at_place(L&& list, long long k) {
  using U = std::remove_cvref_t<L>;
  const long long n = static_cast<long long>(converted::entries(list));
  if (k < 0 || k > n) return {};
  if constexpr (is_span<U>::value) return {list.data + k, static_cast<std::size_t>(n - k) * sizeof(list.data[0]), true};
  else if constexpr (std::is_array_v<U>) return {&list[0] + k, static_cast<std::size_t>(n - k) * sizeof(list[0]), true};
  else return {list.data() + k, static_cast<std::size_t>(n - k) * sizeof(list.data()[0]), true};
}
template <class T> Place value_place(T& v) { return {&v, sizeof v, true}; }
inline void* copy(Place to, Place from, std::size_t n) {
  if (!to.there || !from.there || n > to.room || n > from.room) {
    converted::g_open = true;
    return const_cast<void*>(to.at);
  }
  return n ? std::memcpy(const_cast<void*>(to.at), from.at, n) : const_cast<void*>(to.at);
}
inline void* move(Place to, Place from, std::size_t n) {
  if (!to.there || !from.there || n > to.room || n > from.room) {
    converted::g_open = true;
    return const_cast<void*>(to.at);
  }
  return n ? std::memmove(const_cast<void*>(to.at), from.at, n) : const_cast<void*>(to.at);
}

// ---- the world the two sides run in ----
/// The logic each side's Blueprints of actors run in (the level's actors the file's ActorBlueprints name), and
/// those actors' variables as they started: before every call both start there; after it they must be the same.
struct World {
  RuleBlueprint old_side, new_side;
  converted::ActorsStarted old_actors, new_actors;
  explicit World(const std::vector<std::string>& actors) : old_side(nullptr, nullptr, false, actors), new_side(nullptr, nullptr, false, actors) {
    old_side.rt.budget = new_side.rt.budget = 2000000000u;
    SC_CHECK(old_side.problems.empty() && new_side.problems.empty(), "the actors: %s%s", old_side.problems.c_str(), new_side.problems.c_str());
    old_actors.keep(old_side, actors);
    new_actors.keep(new_side, actors);
  }
  kern::logic::Runtime& logic(bool old) { return old ? old_side.rt : new_side.rt; }
  void before() {
    old_actors.restore();
    new_actors.restore();
  }
  bool same_after() const {
    const std::vector<converted::Value> a = old_actors.now(), b = new_actors.now();
    if (a.size() != b.size()) return false;
    for (std::size_t k = 0; k < a.size(); ++k)
      if (!converted::same_value(a[k], b[k])) return false;
    return true;
  }
};

/// One side's call: run with the guards armed. False: C++ leaves open what it gives (a guard marked it, or
/// it read outside a list so long that it was given up - converted::open_read). `asks`: how often it asked
/// for memory.
/// From where the last two calls asked for memory - the old side's, then the new side's (cost.h): shown where
/// the new asks more often (Tally::take), so that a count that differs says at once from where.
inline cost::Trace g_trace_before, g_trace_last;
template <class F> bool run(F&& f, long& asks) {
  converted::g_open = false;
  converted::g_open_reads = 0;
  if (setjmp(converted::g_bail) != 0) {
    cost::end();
    converted::may_bail(false);
    asks = 0;
    return false;
  }
  converted::g_can_bail = true;
  converted::may_bail(true);
  cost::tracing = true;
  cost::begin();
  f();
  asks = cost::end();
  cost::tracing = false;
  g_trace_before = g_trace_last;
  g_trace_last = cost::now;
  converted::g_can_bail = false;
  converted::may_bail(false);
  return !converted::g_open;
}

/// The comparison of one function over its calls: up to kCalls compared ones (what C++ leaves open is drawn
/// again, up to twenty times as often), twenty at least.
struct Tally {
  static constexpr int kCalls = 300;
  std::string function;
  int compared = 0, outside = 0, wrong = 0, costlier = 0;
  long most_new = 0, with_old = 0;
  explicit Tally(std::string f) : function(std::move(f)) {}
  bool more(int tried) const {
    ++g_call;   // (a new call: what the test made for the last one - made() - is gone)
    return compared < kCalls && tried < kCalls * 20;
  }
  /// One call: whether both sides said what they give (`defined`), what differs (empty: nothing), how often each
  /// asked for memory.
  void take(bool defined, const std::string& differs, long old_asks, long new_asks, int call) {
    if (!defined) {
      ++outside;
      return;
    }
    ++compared;
    if (!differs.empty() && ++wrong <= 3) std::printf("  %s, call %d: %s\n", function.c_str(), call, differs.c_str());
    if (new_asks > old_asks) {
      if (costlier == 0) {   // (the first such call: from where each side asked)
        std::printf("  %s, call %d: the new asks for memory %ld time(s), the old %ld\n", function.c_str(), call, new_asks, old_asks);
        cost::print("the old", g_trace_before, old_asks);
        cost::print("the new", g_trace_last, new_asks);
      }
      ++costlier;
      if (new_asks - old_asks > most_new - with_old) most_new = new_asks, with_old = old_asks;
    }
  }
  ~Tally() {
    SC_CHECK(wrong == 0, "%s: %d of %d calls differ", function.c_str(), wrong, compared);
    SC_CHECK(costlier == 0, "%s: in %d of %d calls the new asks for memory more often than the old (up to %ld times instead of %ld)", function.c_str(), costlier,
             compared, most_new, with_old);
    SC_CHECK(compared >= 20, "%s: only %d calls could be compared (in %d C++ leaves open what they give)", function.c_str(), compared, outside);
    std::printf("  %s: %d calls compared, %d not (C++ leaves them open)\n", function.c_str(), compared, outside);
  }
};

/// What differs: the names of the parts that are not the same, one after the other.
struct Differs {
  std::string text;
  template <class A, class B> void check(const char* what, const A& a, const B& b) {
    if (!same(a, b)) text += (text.empty() ? "" : ", ") + std::string(what);
  }
};


// ---- the form "two builds" (UEBERGABE 4.2; source_diff.py --builds) ----
// Each side is the whole program in its state - the old tree (the sources as they stood at --base) and the new -, and
// the same test is compiled against each and linked into each. The old program writes, call for call, what every
// compared function gave and left as bytes (each part apart: what it returns, its object, a parameter); the new
// draws the same calls (the same draws: the classes drawn have the same fields on both sides - source_diff.py checks
// it) and compares its bytes with those, bit for bit. Nothing of one program is in the other: a class of ~40 files
// (Game), helpers of the same name in two texts, an old text that would not compile against the new headers - each
// is in its own program.

/// The bytes a value leaves: every number by its bits, a list by its length and its entries, an object held by a
/// pointer by whether there is one and what it holds (one already written by its number: the same object twice).
struct Bytes {
  std::string b;
  std::vector<const void*> seen;
  void raw(const void* p, std::size_t n) { b.append(static_cast<const char*>(p), n); }
  template <class N> void num(N v) { raw(&v, sizeof v); }
};
template <class T> void put(Bytes& o, const T& x) {
  using X = std::remove_cv_t<T>;
  if constexpr (std::is_same_v<X, bool>) o.num<unsigned char>(x ? 1 : 0);
  else if constexpr (std::is_enum_v<X>) o.num(static_cast<long long>(x));
  else if constexpr (std::is_arithmetic_v<X>) o.num(x);   // (a float by its bits: -0.0 and 0.0, two NaNs, are other bytes)
  else if constexpr (is_text<X>) {
    o.num<u64>(x.size());
    o.raw(x.data(), x.size());
  } else if constexpr (std::is_same_v<X, const char*> || std::is_same_v<X, char*>) {
    o.num<unsigned char>(x != nullptr);
    if (x != nullptr) put(o, std::string_view(x));
  } else if constexpr (is_vec<X>) {
    std::apply([&](const auto&... c) { (put(o, c), ...); }, components(x));
  } else if constexpr (is_vector<X>::value || is_span<X>::value) {
    o.num<u64>(std::size(x));
    for (std::size_t k = 0; k < std::size(x); ++k) {
      if constexpr (std::is_same_v<typename X::value_type, bool>) put(o, static_cast<bool>(x[k]));
      else put(o, x[k]);
    }
  } else if constexpr (is_array<X>::value || std::is_array_v<X>) {
    for (const auto& e : x) put(o, e);
  } else if constexpr (is_optional<X>::value) {
    o.num<unsigned char>(x.has_value());
    if (x) put(o, *x);
  } else if constexpr (is_pair<X>::value) {
    put(o, x.first);
    put(o, x.second);
  } else if constexpr (is_map<X>::value) {
    o.num<u64>(x.size());
    for (std::size_t k = 0; k < x.size(); ++k) {
      put(o, x.key_at(k));
      put(o, x.value_at(k));
    }
  } else if constexpr (is_set<X>::value) {
    o.num<u64>(x.size());
    for (std::size_t k = 0; k < x.size(); ++k) {
      const typename X::key_type key = x.key_at(k);
      put(o, key);
    }
  } else if constexpr (is_held<X>) {
    o.num<unsigned char>(x != nullptr);
    if (x == nullptr) return;
    const void* at = &*x;
    for (std::size_t k = 0; k < o.seen.size(); ++k)
      if (o.seen[k] == at) {
        o.num<unsigned char>(1);
        o.num<u64>(k);
        return;
      }
    o.num<unsigned char>(0);
    o.seen.push_back(at);
    put(o, *x);
  } else if constexpr (HasFields<X>) {
    std::apply([&](const auto&... f) { (put(o, f), ...); }, fields(x));
  } else if constexpr (std::is_class_v<X>) {
    // (a class of the engine that keeps what it holds to itself: what a call left in it is not written - the test says so once)
    static const bool said = (std::printf("  note: %s: what it holds is its own - not compared\n", __PRETTY_FUNCTION__), true);
    (void)said;
  } else {
    static_assert(sizeof(X) == 0, "a value the test cannot write");
  }
}
template <class T> Bytes bytes_of(const T& x) {
  Bytes b;
  put(b, x);
  return b;
}

/// The object a method is called on: made by its constructor (value-initialized, as `T x{}`), drawn or not; a
/// destructor ends it in the call (end()), else it ends with the call.
template <class T> struct Self {
  void* room = ::operator new(sizeof(T), std::align_val_t(alignof(T)));
  T* p = ::new (room) T{};
  Self() = default;
  Self(const Self&) = delete;
  Self& operator=(const Self&) = delete;
  ~Self() {
    if (p != nullptr) p->~T();
    ::operator delete(room, std::align_val_t(alignof(T)));
  }
  void end() {
    T* was = p;
    p = nullptr;
    was->~T();
  }
  T& operator*() { return *p; }
  T* operator->() { return p; }
};

/// One compared function in one program: SC_LOWERED_BUILDS=write:<file> (the old program) or check:<file> (the new).
/// The old draws calls until kCalls of them are ones C++ says what they give (or twenty times as many were drawn) and
/// writes them all; the new draws exactly as many - the same draws - and compares those both programs say.
struct Builds {
  static constexpr int kCalls = Tally::kCalls;
  struct Call {
    bool defined = false;
    long asks = 0;
    std::vector<std::pair<std::string, std::string>> parts;
  };
  std::string function, file;
  bool writing = false;
  std::vector<Call> old;   // (write: what this program gives; check: what the old program wrote)
  std::size_t at = 0;
  int compared = 0, outside = 0, wrong = 0, costlier = 0, defined_old = 0;
  long most_new = 0, with_old = 0;
  Call now;
  explicit Builds(std::string f) : function(std::move(f)) {
    const char* how = std::getenv("SC_LOWERED_BUILDS");
    const std::string h = how ? how : "";
    writing = h.rfind("write:", 0) == 0;
    SC_CHECK(writing || h.rfind("check:", 0) == 0, "%s: SC_LOWERED_BUILDS is write:<file> or check:<file> (run_tests.sh sets it), not '%s'", function.c_str(), h.c_str());
    file = h.size() > 6 ? h.substr(6) : "";
    if (!writing) read();
  }
  // (the record: per function its name and one field; in it per call a head - defined, asks, the number of parts - and
  // the parts, each a name and its bytes)
  static void text(std::string& out, const std::string& s) {
    const u64 n = s.size();
    out.append(reinterpret_cast<const char*>(&n), sizeof n);
    out += s;
  }
  static bool text(const std::string& in, std::size_t& k, std::string& s) {
    u64 n = 0;
    if (k + sizeof n > in.size()) return false;
    std::memcpy(&n, in.data() + k, sizeof n);
    k += sizeof n;
    if (k + n > in.size()) return false;
    s.assign(in, k, n);
    k += n;
    return true;
  }
  void read() {
    std::string all;
    if (FILE* f = std::fopen(file.c_str(), "rb")) {
      char buf[65536];
      for (std::size_t n; (n = std::fread(buf, 1, sizeof buf, f)) > 0;) all.append(buf, n);
      std::fclose(f);
    }
    std::size_t k = 0;
    std::string name, field;
    while (text(all, k, name)) {
      if (!text(all, k, field)) break;
      if (name != function) continue;
      std::size_t j = 0;
      std::string head, part, bytes;
      while (text(field, j, head)) {
        Call c;
        c.defined = head[0] == 1;
        std::memcpy(&c.asks, head.data() + 1, sizeof c.asks);
        u32 parts = 0;
        std::memcpy(&parts, head.data() + 1 + sizeof c.asks, sizeof parts);
        for (unsigned q = 0; q < parts && text(field, j, part) && text(field, j, bytes); ++q) c.parts.emplace_back(part, bytes);
        old.push_back(std::move(c));
      }
      return;
    }
    SC_CHECK(false, "%s: the old program wrote nothing for it (%s)", function.c_str(), file.c_str());
  }
  bool more(int tried) {
    ++g_call;   // (a new call: what the test made for the last one - made() - is gone)
    now = Call{};
    if (writing) return defined_old < kCalls && tried < kCalls * 20;
    return at < old.size();
  }
  void part(std::string name, Bytes b) { now.parts.emplace_back(std::move(name), std::move(b.b)); }
  /// An object a call leaves: as one part (what it holds, every field, compared as a whole - an object held twice is
  /// the same object), and - where the test has its fields' names - each field once more as a part of its own, so that
  /// where it differs the test says which fields do.
  template <class T> void object(const char* name, const T& x) {
    part(name, bytes_of(x));
    if constexpr (Named<T> && requires { Fields<std::remove_const_t<T>>::names; }) {
      constexpr auto& names = Fields<std::remove_const_t<T>>::names;
      std::size_t k = 0;
      std::apply([&](const auto&... f) { ((part(std::string(name) + ": " + names[k++], bytes_of(f))), ...); }, fields(x));
    }
  }
  void take(bool defined, long asks, int call) {
    now.defined = defined;
    now.asks = defined ? asks : 0;
    if (writing) {
      if (defined) ++defined_old;
      old.push_back(std::move(now));
      return;
    }
    const Call& was = old[at++];
    if (!defined || !was.defined) {
      ++outside;
      return;
    }
    ++compared;
    std::string differs;
    for (std::size_t q = 0; q < std::max(was.parts.size(), now.parts.size()); ++q) {
      if (q < was.parts.size() && q < now.parts.size() && was.parts[q] == now.parts[q]) continue;
      differs += (differs.empty() ? "" : ", ") + (q < now.parts.size() ? now.parts[q].first : was.parts[q].first);
    }
    if (!differs.empty() && ++wrong <= 3) std::printf("  %s, call %d: %s\n", function.c_str(), call, differs.c_str());
    if (now.asks > was.asks) {
      ++costlier;
      if (now.asks - was.asks > most_new - with_old) most_new = now.asks, with_old = was.asks;
    }
  }
  ~Builds() {
    if (writing) {
      std::string field;
      for (const Call& c : old) {
        std::string head(1, c.defined ? 1 : 0);
        head.append(reinterpret_cast<const char*>(&c.asks), sizeof c.asks);
        const u32 parts = static_cast<u32>(c.parts.size());   // (an object of ~250 fields: a part each - more than a byte counts)
        head.append(reinterpret_cast<const char*>(&parts), sizeof parts);
        text(field, head);
        for (const auto& [name, b] : c.parts) {
          text(field, name);
          text(field, b);
        }
      }
      std::string rec;
      text(rec, function);
      text(rec, field);
      FILE* f = std::fopen(file.c_str(), "ab");
      SC_CHECK(f != nullptr, "%s: cannot write %s", function.c_str(), file.c_str());
      if (f != nullptr) {
        std::fwrite(rec.data(), 1, rec.size(), f);
        std::fclose(f);
      }
      std::printf("  %s: %zu calls written by the old program (%d where C++ says what they give)\n", function.c_str(), old.size(), defined_old);
      return;
    }
    SC_CHECK(wrong == 0, "%s: %d of %d calls differ", function.c_str(), wrong, compared);
    SC_CHECK(costlier == 0, "%s: in %d of %d calls the new asks for memory more often than the old (up to %ld times instead of %ld)", function.c_str(), costlier,
             compared, most_new, with_old);
    SC_CHECK(compared >= 20, "%s: only %d calls could be compared (in %d C++ leaves open what they give)", function.c_str(), compared, outside);
    std::printf("  %s: %d calls compared, %d not (C++ leaves them open)\n", function.c_str(), compared, outside);
  }
};

}  // namespace sc::lowered
