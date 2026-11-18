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
#include <deque>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "converted.h"
#include "kern/core/fields.h"
#include "kern/core/json.h"

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
/// A map (std::map, std::unordered_map, kern::Map) and a set (std::set, std::unordered_set, kern::Set): by what they
/// are, not by their name - keys, and for a map the values. One of them keeps no order (unordered_*): two are then
/// the same when they hold the same, in any order.
template <class T> concept MapLike = requires { typename T::key_type; typename T::mapped_type; } && requires(const T& m) { m.find(std::declval<const typename T::key_type&>()); };
template <class T> concept SetLike = requires { typename T::key_type; } && !requires { typename T::mapped_type; } &&
                                     std::is_same_v<typename T::key_type, typename T::value_type> && requires(const T& m) { m.find(std::declval<const typename T::key_type&>()); };
template <class T> concept Unordered = requires { typename T::hasher; typename T::local_iterator; };
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
  else if constexpr (std::is_same_v<U, kern::json::Value>) {
    // (a value of JSON: null, a flag, a number, a text - or a list or an object of a few of them)
    switch (d.below(6)) {
      case 0: x = kern::json::Value(); break;
      case 1: x = kern::json::Value(d.flag()); break;
      case 2: x = kern::json::Value(d.number()); break;
      case 3: x = kern::json::Value(d.word()); break;
      case 4: x = kern::json::Value::array(); for (u32 n = d.below(3); n > 0; --n) x.push(kern::json::Value(d.number())); break;
      default: x = kern::json::Value::object(); for (u32 n = d.below(3); n > 0; --n) x.set(d.word(), kern::json::Value(d.number())); break;
    }
  }
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
  } else if constexpr (MapLike<U>) {
    x.clear();
    for (u32 n = d.below(6); n > 0; --n) {
      auto k = drawn<typename U::key_type>(d);
      auto v = drawn<typename U::mapped_type>(d);
      x.insert_or_assign(k, std::move(v));
    }
  } else if constexpr (SetLike<U>) {
    x.clear();
    for (u32 n = d.below(6); n > 0; --n) x.insert(drawn<typename U::key_type>(d));
  } else if constexpr (HasFields<U>) {
    std::apply([&](auto&... f) { (draw(d, f), ...); }, fields(x));
  } else if constexpr (std::is_default_constructible_v<U> && std::is_class_v<U>) {
    // a class of another file that keeps what it holds to itself (the engine's kern::FootprintStream): made by
    // its constructor - the test says so once
    static const bool said = (std::printf("  note: %s: made by its constructor, not drawn (it keeps what it holds to itself)\n", __PRETTY_FUNCTION__), true);
    (void)said;
    x = U{};
  } else {
    static_assert(sizeof(U) == 0, "a value the test cannot draw");
  }
}

// ---- carried: the same value in the type of the other side (a class of the file is two classes, sc_old's
// and sc_new's - field by field, by name: source_diff.py checks that they have the same fields). A list and a
// text get the room the old side's has (its capacity): what the two ask for memory is then compared alike ----
template <class T> kern::logic::Value as_value(const T& x);
/// Whether a type holds values of the Blueprint (kern::logic::Value): the old program kept the Blueprint's structs so;
/// the test draws them on the side where they are structs (draw_pair).
/// So did it hold texts by a pointer to their characters (const char*), where the new holds std::string: those are
/// drawn on the new side too, and the old side's pointers point to texts the test keeps.
template <class T> constexpr bool drawn_on_new();
template <class T> struct holds_value : std::bool_constant<drawn_on_new<std::remove_cv_t<T>>()> {};
template <class T> constexpr bool drawn_on_new() {
  if constexpr (std::is_same_v<T, kern::logic::Value> || std::is_same_v<T, const char*>) return true;
  else if constexpr (is_vector<T>::value || is_array<T>::value || is_optional<T>::value) return drawn_on_new<std::remove_cv_t<typename T::value_type>>();
  else if constexpr (std::is_array_v<T>) return drawn_on_new<std::remove_cv_t<std::remove_extent_t<T>>>();
  else if constexpr (std::is_class_v<T> && HasFields<T>) {
    using F = decltype(fields(std::declval<T&>()));
    return []<std::size_t... I>(std::index_sequence<I...>) { return (drawn_on_new<std::remove_cvref_t<std::tuple_element_t<I, F>>>() || ...); }(std::make_index_sequence<std::tuple_size_v<F>>{});
  } else return false;
}
/// The characters of a text the old side points to: kept for as long as the test runs.
inline const char* kept_text(const std::string& t) {
  static std::deque<std::string> kept;
  if (kept.size() > 100000) kept.clear();   // (the texts of calls long done)
  return kept.emplace_back(t).c_str();
}
template <class A, class B> void carry(const A& a, B& b) {
  using X = std::remove_cv_t<A>;
  using Y = std::remove_cv_t<B>;
  if constexpr (std::is_same_v<Y, kern::logic::Value> && !std::is_same_v<X, kern::logic::Value>) b = as_value(a);   // (the struct as the old program held it)
  else if constexpr (std::is_same_v<Y, const char*> && is_text<X>) b = kept_text(std::string(a));
  else if constexpr (std::is_same_v<X, const char*> && std::is_same_v<Y, std::string>) b = a ? a : "";
  else if constexpr (std::is_same_v<X, Y> && std::is_trivially_copyable_v<X> && !Named<X> && !std::is_array_v<X>) b = a;
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
  } else if constexpr (MapLike<X> && MapLike<Y>) {
    // (in the order the old side gives its entries: the new keeps that one)
    b.clear();
    if constexpr (requires { b.reserve(a.size()); }) b.reserve(a.size());
    for (const auto& [k, v] : a) {
      typename Y::key_type kb{};
      carry(k, kb);
      typename Y::mapped_type vb{};
      carry(v, vb);
      b.insert_or_assign(kb, std::move(vb));
    }
  } else if constexpr (SetLike<X> && SetLike<Y>) {
    b.clear();
    if constexpr (requires { b.reserve(a.size()); }) b.reserve(a.size());
    for (const auto& k : a) {
      typename Y::key_type kb{};
      carry(k, kb);
      b.insert(kb);
    }
  } else if constexpr (std::is_same_v<X, Y> && std::is_class_v<X> && !HasFields<X> && std::is_copy_assignable_v<X>) {
    b = a;   // (a class that keeps what it holds to itself: the same type on both sides)
  } else if constexpr (HasFields<X> && HasFields<Y>) {
    auto from = fields(a);
    auto to = fields(b);
    static_assert(std::tuple_size_v<decltype(from)> == std::tuple_size_v<decltype(to)>, "classes of other fields");
    [&]<std::size_t... I>(std::index_sequence<I...>) { (carry(std::get<I>(from), std::get<I>(to)), ...); }(std::make_index_sequence<std::tuple_size_v<decltype(from)>>{});
  } else {
    static_assert(sizeof(X) == 0, "a value the test cannot carry to the other side");
  }
}

/// A value drawn for both sides: on the old one and carried to the new - or, where the old holds values of the Blueprint,
/// on the new one (its structs) and carried to the old.
template <class P> struct Arg;
template <class O, class N> void draw_pair(Draw& d, O& old, N& now) {
  if constexpr (holds_value<std::remove_cv_t<O>>::value && !holds_value<std::remove_cv_t<N>>::value) {
    draw(d, now);
    carry(now, old);
  } else {
    draw(d, old);
    carry(old, now);
  }
}

// ---- the same, bit for bit ----
template <class T> kern::logic::Value as_value(const T& x);
template <class A, class B> bool same(const A& a, const B& b) {
  using X = std::remove_cv_t<A>;
  using Y = std::remove_cv_t<B>;
  if constexpr (std::is_same_v<X, kern::json::Value> && std::is_same_v<Y, kern::json::Value>) {
    // (a value of JSON, by what it says of itself - its numbers bit for bit)
    if (a.type() != b.type() || a.as_bool() != b.as_bool() || !same(a.as_number(), b.as_number())) return false;
    if (a.is_string() && a.as_string() != b.as_string()) return false;
    if (a.items().size() != b.items().size() || a.members().size() != b.members().size()) return false;
    for (std::size_t k = 0; k < a.items().size(); ++k)
      if (!same(a.items()[k], b.items()[k])) return false;
    for (std::size_t k = 0; k < a.members().size(); ++k)
      if (a.members()[k].first != b.members()[k].first || !same(a.members()[k].second, b.members()[k].second)) return false;
    return true;
  } else if constexpr (std::is_same_v<X, kern::logic::Value> && !std::is_same_v<Y, kern::logic::Value>) {
    return converted::same_value(a, as_value(b));   // (a value of the Blueprint and the struct the new one holds)
  } else if constexpr (std::is_same_v<X, kern::logic::Value> && std::is_same_v<Y, kern::logic::Value>) {
    return converted::same_value(a, b);
  } else if constexpr (std::is_floating_point_v<X>) {
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
  } else if constexpr (std::is_same_v<X, const char*> && is_text<Y>) {
    return a != nullptr && std::string_view(a) == std::string_view(b);   // (a text by its characters, and the text)
  } else if constexpr (is_text<X> && std::is_same_v<Y, const char*>) {
    return b != nullptr && std::string_view(a) == std::string_view(b);
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
  } else if constexpr ((MapLike<X> && MapLike<Y>) || (SetLike<X> && SetLike<Y>)) {
    if (a.size() != b.size()) return false;
    if constexpr (Unordered<X> || Unordered<Y>) {
      // (one keeps no order: every entry of the one is the other's - the keys are the program's own numbers and texts)
      for (const auto& e : a) {
        if constexpr (MapLike<X>) {
          const auto it = b.find(static_cast<typename Y::key_type>(e.first));
          if (it == b.end() || !same(e.first, it->first) || !same(e.second, it->second)) return false;
        } else {
          const auto it = b.find(static_cast<typename Y::key_type>(e));
          if (it == b.end() || !same(e, *it)) return false;
        }
      }
      return true;
    } else {
      auto ia = a.begin();
      auto ib = b.begin();
      for (; ia != a.end(); ++ia, ++ib)
        if (!same(*ia, *ib)) return false;
      return true;
    }
  } else if constexpr (HasFields<X> && HasFields<Y>) {
    auto fa = fields(a);
    auto fb = fields(b);
    if constexpr (std::tuple_size_v<decltype(fa)> != std::tuple_size_v<decltype(fb)>) return false;
    else return [&]<std::size_t... I>(std::index_sequence<I...>) { return (same(std::get<I>(fa), std::get<I>(fb)) && ...); }(std::make_index_sequence<std::tuple_size_v<decltype(fa)>>{});
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
template <class P, class Q> void draw_pair(Draw& d, Arg<P>& o, Arg<Q>& n) { draw_pair(d, o.v, n.v); }
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

/// A whole number made a value of an enum whose type is not fixed: one outside the range of its values marks the call.
template <long long Lo, long long Hi, class N> constexpr N enum_fits(N x) {
  if (static_cast<long long>(x) < Lo || static_cast<long long>(x) > Hi) {
    converted::g_open = true;
    return N{0};
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
struct Region;
inline std::vector<Region>& regions();
/// The place a pointer of another kind names: in a list the test handed, up to its end; else the test does not know its
/// room (it stands as it is - the sanitizers watch it).
inline Place pointer_place(const void* p);
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

// ---- the places the test hands the old text by a pointer (a list's first entry, one value): an entry by a pointer
// is checked against them - one outside marks the call. A pointer into another place (a table of the file, a local
// array) is not the test's: its entry stands as it is (the sanitizers watch it) ----
struct Region {
  const char* begin;
  const char* end;
};
inline std::vector<Region>& regions() {
  static std::vector<Region> r;
  return r;
}
/// A place handed to the old text, for as long as the call runs.
struct Placed {
  template <class T> Placed(T* at, std::size_t n) {
    const char* b = reinterpret_cast<const char*>(at);
    regions().push_back({b, b + n * sizeof(T)});
  }
  ~Placed() { regions().pop_back(); }
  Placed(const Placed&) = delete;
  Placed& operator=(const Placed&) = delete;
};
inline Place pointer_place(const void* p) {
  const char* a = static_cast<const char*>(p);
  for (auto r = regions().rbegin(); r != regions().rend(); ++r)
    if (a >= r->begin && a <= r->end) return {p, static_cast<std::size_t>(r->end - a), true};
  return {p, static_cast<std::size_t>(-1), true};
}
template <class P, class I> P& entry(P* p, I k) {
  const char* a = reinterpret_cast<const char*>(p);
  for (auto r = regions().rbegin(); r != regions().rend(); ++r) {
    if (a < r->begin || a > r->end) continue;
    const long long at = static_cast<long long>((a - r->begin) / static_cast<long long>(sizeof(P))) + static_cast<long long>(k);
    const long long n = static_cast<long long>((r->end - r->begin) / static_cast<long long>(sizeof(P)));
    if (at < 0 || at >= n) {
      converted::open_read();   // (marks the call - and gives it up when it goes on so: a loop to a length that is not there)
      static std::remove_const_t<P> none{};
      none = std::remove_const_t<P>{};
      return none;
    }
    return p[k];
  }
  return p[k];
}

// ---- a value of the Blueprint (kern::logic::Value), as the old program handed one: what the engine makes of a value of
// the program (kern::logic::ActorBlueprint's arguments - a flag, a whole number, a decimal, a vector, a text, an enum's
// value, a plain struct of them, a list) ----
template <class T> kern::logic::Value as_value(const T& x) {
  using kern::logic::Value;
  using U = std::remove_cvref_t<T>;
  if constexpr (std::is_same_v<U, bool>) return Value::boolean(x);
  else if constexpr (std::is_enum_v<U>) return Value::enumeration(static_cast<i64>(x));
  else if constexpr (std::is_integral_v<U>) return Value::integer(static_cast<i64>(x));
  else if constexpr (std::is_floating_point_v<U>) return Value::number(static_cast<f64>(x));
  else if constexpr (is_vec<U>) return Value::vector(kern::Vec3(x));
  else if constexpr (is_text<U>) return Value::text(std::string(x));
  else if constexpr (is_vector<U>::value) {
    std::vector<Value> items;
    for (const auto& e : x) items.push_back(as_value(e));
    return Value::structures(std::move(items));
  } else {
    return std::apply([](const auto&... f) { return Value::structure({as_value(f)...}); }, fields(const_cast<U&>(x)));
  }
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
  /// The running Blueprint of an actor on one side (how the old program bound a file to it).
  kern::logic::Instance* instance(bool old, const char* actor) {
    kern::logic::Runtime& rt = logic(old);
    kern::logic::Instance* i = rt.find(rt.host().find_actor(actor));
    SC_CHECK(i != nullptr, "the actor %s has no running Blueprint", actor);
    return i;
  }
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

/// The rounds of the loops of a call: one that goes round more often than this is given up (not compared).
inline long g_rounds = 0;
inline constexpr long kRounds = 20000000;
inline void tick() {
  if (++g_rounds > kRounds && converted::g_can_bail) {
    converted::g_can_bail = false;
    converted::g_open = true;
    std::longjmp(converted::g_bail, 1);
  }
}

/// One side's call: run with the guards armed. False: C++ leaves open what it gives (a guard marked it, or
/// it read outside a list so long that it was given up - converted::open_read). `asks`: how often it asked
/// for memory.
template <class F> bool run(F&& f, long& asks) {
  converted::g_open = false;
  converted::g_open_reads = 0;
  g_rounds = 0;
  if (setjmp(converted::g_bail) != 0) {
    cost::end();
    converted::may_bail(false);
    asks = 0;
    return false;
  }
  converted::g_can_bail = true;
  converted::may_bail(true);
  cost::begin();
  f();
  asks = cost::end();
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
  bool more(int tried) const { return compared < kCalls && tried < kCalls * 20; }
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

}  // namespace sc::lowered
