// Kern Logic: what the nodes compute, as typed C++ functions.
//
// A node has one meaning. The interpreter's node library (src/logic/core_nodes.cpp) wraps these
// functions as nodes; a Blueprint's C++ (blueprint.h, docs/blueprint_cpp.md) calls them directly.
// Nothing here knows about graphs.
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <new>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "kern/core/base.h"
#include "kern/core/dmath.h"
#include "kern/logic/logic.h"
#include "kern/math/math.h"

namespace kern {
/// A key of a map or set of a Blueprint's class that is a number of the language (numbers.h Typed): the number's hash.
template <class T> struct KeyHash<logic::num::Typed<T>, void> {
  u64 operator()(const logic::num::Typed<T>& key) const {
    if constexpr (std::is_integral_v<T>) return KeyHash<T>{}(key.v);
    else return KeyHash<u64>{}(logic::num::bits_of(key.v));
  }
};

/// A node's setting as a template argument: flow_switch_int<"3">, math_compare<"<=">.
template <std::size_t N> struct Setting {
  char text[N]{};
  constexpr Setting(const char (&s)[N]) {  // NOLINT (implicit: that is the point)
    for (std::size_t i = 0; i < N; ++i) text[i] = s[i];
  }
  constexpr std::string_view view() const { return {text, N - 1}; }
};

/// Functions of f64 that C++ of the engine and converted tests call (kern::math::sin ...). (A Formula node computes with
/// numbers.h, logic.h Formula.)
namespace math {
inline constexpr f64 pi = 3.14159265358979323846;
inline f64 div(f64 a, f64 b) { return b != 0 ? a / b : 0; }
inline f64 mod(f64 a, f64 b) { return b != 0 ? std::fmod(a, b) : 0; }
/// IEEE 754's remainder: a - n * b with n the whole number nearest to a / b (of two, the even one) -
/// std::remainder, exact (the result is always a number a double holds), so the same bits on every
/// platform; its sign where it is 0 is a's. By 0 it gives 0, as mod.
inline f64 remainder(f64 a, f64 b) { return b != 0 ? std::remainder(a, b) : 0; }
inline f64 pow(f64 a, f64 b) { return dm::pow(a, b); }
inline f64 min(f64 a, f64 b) { return min_of(a, b); }
inline f64 max(f64 a, f64 b) { return max_of(a, b); }
inline f64 clamp(f64 x, f64 lo, f64 hi) { return kern::clamp(x, min_of(lo, hi), max_of(lo, hi)); }
inline f64 abs(f64 a) { return std::fabs(a); }
inline f64 sign(f64 a) { return a > 0 ? 1 : a < 0 ? -1 : 0; }
/// The size of `a` with the sign bit of `b` - also of -0 and of a NaN (copysign(1, -0) is -1,
/// where sign(-0) is 0): IEEE 754's copySign, a bit operation, exact everywhere.
inline f64 copysign(f64 a, f64 b) { return std::copysign(a, b); }
inline f64 floor(f64 a) { return std::floor(a); }
inline f64 ceil(f64 a) { return std::ceil(a); }
inline f64 round(f64 a) { return std::round(a); }
inline f64 trunc(f64 a) { return std::trunc(a); }
inline f64 sqrt(f64 a) { return std::sqrt(max_of(0.0, a)); }
inline f64 exp(f64 a) { return dm::exp(a); }
inline f64 log(f64 a) { return a > 0 ? dm::log(a) : 0; }
inline f64 sin(f64 a) { return dm::sin(a); }
inline f64 cos(f64 a) { return dm::cos(a); }
inline f64 tan(f64 a) { return dm::tan(a); }
inline f64 asin(f64 a) { return dm::asin(kern::clamp(a, -1.0, 1.0)); }
inline f64 acos(f64 a) { return dm::acos(kern::clamp(a, -1.0, 1.0)); }
inline f64 atan(f64 a) { return dm::atan(a); }
inline f64 atan2(f64 y, f64 x) { return dm::atan2(y, x); }
/// The hyperbolic functions: the engine's own (kern/src/math/hyperbolic.c), as std::sinh ... of a program.
inline f64 sinh(f64 a) { return dm::sinh(a); }
inline f64 cosh(f64 a) { return dm::cosh(a); }
inline f64 tanh(f64 a) { return dm::tanh(a); }
inline f64 asinh(f64 a) { return dm::asinh(a); }
inline f64 rad(f64 degrees) { return degrees * (pi / 180); }
inline f64 deg(f64 radians) { return radians * (180 / pi); }
inline f64 lerp(f64 a, f64 b, f64 t) { return a + (b - a) * t; }
}  // namespace math

namespace logic {

/// `Text` in a Blueprint's C++. A text written out in the program - "abc": a value typed into a
/// pin, the default of a variable or of a field - is part of the program: a Text made of it
/// refers to those characters. Nothing is copied and no memory is asked for, however long the
/// text is and however often the value is handed on - as a const char* to a literal in C++. A
/// text made while the Blueprint runs (Append, a number as a text, what the engine gives) holds
/// its characters itself, as a std::string does.
class Text {
 public:
  constexpr Text() : written_() {}
  /// "abc": a text the program holds for good - a literal, a constant of the program. It is not
  /// copied, so it must stay where it is. A buffer that is written to (char b[64]) is refused
  /// here: its characters go into a std::string or a std::string_view, and those are copied.
  template <std::size_t N> constexpr Text(const char (&written)[N]) : written_(written, N - 1), ended_(true) {}  // NOLINT (implicit: a text written out is a Text)
  template <std::size_t N> Text(char (&)[N]) = delete;
  Text(std::string made) : made_(std::move(made)), holds_(true) {}                                // NOLINT (implicit: a text made while running)
  explicit Text(std::string_view copied) : made_(copied), holds_(true) {}
  /// (one character, made where it is held - Text of Code)
  explicit Text(std::size_t count, char c) : made_(count, c), holds_(true) {}
  /// A text the program holds for good, by its characters - the name of an enum's value:
  /// referred to, not copied. (Only for what stays where it is as long as the program runs.)
  static Text of_program(std::string_view held) {
    Text t;
    t.written_ = held;
    return t;
  }
  Text(const Text& o) : holds_(o.holds_), ended_(o.ended_) {
    if (holds_) new (&made_) std::string(o.made_);
    else new (&written_) std::string_view(o.written_);
  }
  Text(Text&& o) noexcept : holds_(o.holds_), ended_(o.ended_) {
    if (holds_) new (&made_) std::string(std::move(o.made_));
    else new (&written_) std::string_view(o.written_);
  }
  Text& operator=(const Text& o) {
    if (this == &o) return *this;
    if (holds_ && o.holds_) made_ = o.made_;  // (into the characters it has: std::string's own assignment)
    else if (o.holds_) hold(std::string(o.made_));
    else refer(o.written_, o.ended_);
    return *this;
  }
  /// `O = text_append(std::move(O), x, y)` (a text handed on, set again to itself and more): what
  /// text_append gives there, worked out where it is set - the others are appended where O's
  /// characters are, as std::string's += does, without handing the characters on and back. Made
  /// into a Text anywhere else, it is what text_append gives.
  struct Appending {
    Text& a;
    const Text& b;
    const Text& c;
    operator Text() && {  // NOLINT (implicit: it is that text)
      if (!a.holds_ || &b == &a || &c == &a) {
        std::string made;
        made.reserve(a.size() + b.size() + c.size());
        made.append(a.view()).append(b.view()).append(c.view());
        return made;
      }
      std::string t = std::move(a.made_);
      t.append(b.view()).append(c.view());
      return t;
    }
  };
  Text& operator=(Appending&& x) {
    if (&x.a == this && holds_ && &x.b != this && &x.c != this) [[likely]] {
      add(x.b.view());
      add(x.c.view());
      return *this;
    }
    return *this = static_cast<Text>(std::move(x));
  }
  Text& operator=(Text&& o) noexcept {
    if (this == &o) return *this;
    if (holds_ && o.holds_) made_ = std::move(o.made_);
    else if (o.holds_) hold(std::move(o.made_));
    else refer(o.written_, o.ended_);
    return *this;
  }
  constexpr ~Text() {
    if (holds_) made_.~basic_string();
  }
  /// Its characters, where they are.
  constexpr std::string_view view() const { return holds_ ? std::string_view(made_) : written_; }
  constexpr operator std::string_view() const { return view(); }  // NOLINT (implicit: read as its characters)
  constexpr std::size_t size() const { return view().size(); }
  constexpr bool empty() const { return view().empty(); }
  /// Does it hold its characters (a text made while running), or refer to the program's?
  constexpr bool holds() const { return holds_; }
  /// Its characters where they stand, if a 0 ends them there - a text written out in the program
  /// ("abc"), or one made while running: what a function that takes a `const char*` reads, without
  /// a copy. Null: nothing ends them where they are (the name of an enum's value, no text).
  const char* ended() const { return holds_ ? made_.c_str() : ended_ ? written_.data() : nullptr; }
  /// The std::string it holds (a text made while running), or null: what a function that takes a
  /// `const std::string&` reads, without a copy.
  const std::string* made() const { return holds_ ? &made_ : nullptr; }
  /// Its characters as a std::string of their own (a copy), and handed on without one where it holds them.
  std::string str() const& { return std::string(view()); }
  std::string str() && { return holds_ ? std::move(made_) : std::string(written_); }
  friend bool operator==(const Text& a, const Text& b) { return a.view() == b.view(); }
  friend auto operator<=>(const Text& a, const Text& b) { return a.view() <=> b.view(); }
  /// Set Code: the character at `index` becomes `c`, where it stands (a text of the program is first made the
  /// text's own). An index outside the text changes nothing.
  void set_at(i64 index, char c) {
    if (index < 0 || static_cast<u64>(index) >= size()) return;
    if (!holds_) hold(std::string(written_));
    made_[static_cast<std::size_t>(index)] = c;
  }

 private:
  /// (appended where its characters are - one character as std::string's += of a char adds it)
  void add(std::string_view v) {
    if (v.size() == 1) made_.push_back(v[0]);
    else if (!v.empty()) made_.append(v);
  }
  void hold(std::string made) {
    if (holds_) made_ = std::move(made);
    else new (&made_) std::string(std::move(made)), holds_ = true;
  }
  void refer(std::string_view written, bool ended) {
    if (holds_) made_.~basic_string(), holds_ = false;
    new (&written_) std::string_view(written);
    ended_ = ended;
  }
  union {
    std::string_view written_;  // a text of the program: its characters there
    std::string made_;          // a text made while running: its characters here
  };
  bool holds_ = false;
  bool ended_ = false;  // (a text of the program that was written out: a 0 stands behind its characters)
};

/// What a list of a Blueprint's class holds of an item: the item - every entry of a list is an
/// object of its own, so that it is a place (Flags[i] = true; Toggle(Flags[i]);). A Bool is held
/// as a Truth: std::vector<bool> packs its entries into bits, and nothing can refer to a bit.
struct Truth {
  bool value = false;
  Truth() = default;
  Truth(bool v) : value(v) {}                      // NOLINT (implicit: it is the Bool)
  operator bool&() { return value; }               // NOLINT
  operator const bool&() const { return value; }   // NOLINT
};
namespace detail {
template <class T> struct Entry { using type = T; };
template <> struct Entry<bool> { using type = Truth; };
template <class T> struct Sought { using type = T; };
}  // namespace detail
/// What an item of a list of T is compared with (Find, Contains, Remove Item): a T.
template <class T> using Sought = typename detail::Sought<T>::type;
template <class A, class B> constexpr bool equal_item(const A& a, const B& b) { return a == b; }

/// Beside the lists. A place that is named beside a list (Cars[9] of two cars) is an entry of
/// its own, with the zero of the items' type - a struct with its defaults: read, it is that
/// zero; what is written there nobody reads; a function that is given it as a reference works on
/// it. One for every time such a place is named, so naming a place changes nothing anywhere -
/// in whatever order a compiler works a statement out. The entries stay while the engine is in
/// a Blueprint's class (beside::Visit, logic.h) and are gone the next time it goes in. (One
/// store for every Blueprint of the program: Blueprints run on one thread.)
namespace beside {
template <class T> T& entry() {
  static std::deque<T> entries;  // (a deque: an entry stays where it is when more come)
  static u64 of = 0;
  if (of != visit()) entries.clear(), of = visit();
  return entries.emplace_back();
}
}  // namespace beside

/// A list in a Blueprint's C++ (List<Float>, List<Weapon> ...): a std::vector of its entries
/// whose [] never reaches beside the list - there it gives an entry of its own (beside). So a
/// place like Cars[i].Wheels[j].Slip is one in every tier, and what it costs where the entry is
/// there is one comparison: the interpreter does the same (Instance::spot).
template <class T> struct List : std::vector<typename detail::Entry<T>::type> {
  using Item = typename detail::Entry<T>::type;
  using Items = std::vector<Item>;
  using Items::Items;
  List() = default;
  List(const Items& items) : Items(items) {}                 // NOLINT (implicit: the vector of its items is the list)
  List(Items&& items) noexcept : Items(std::move(items)) {}  // NOLINT
  /// (a list of numbers a function of C++ gave - std::vector<f32> - is a list of places of that very type: List<f32>)
  template <class R>
    requires(num::is_typed<T> && std::same_as<R, num::raw<T>>)
  List(const std::vector<R>& items) : Items(items.begin(), items.end()) {}  // NOLINT
  template <class I>
    requires num::Integer<num::raw<I>>
  Item& operator[](I index) {
    const i64 at = static_cast<i64>(num::raw_value(index));
    if (at >= 0 && static_cast<std::size_t>(at) < this->size()) return Items::operator[](static_cast<std::size_t>(at));
    return beside::entry<Item>();
  }
  template <class I>
    requires num::Integer<num::raw<I>>
  const Item& operator[](I index) const {
    const i64 at = static_cast<i64>(num::raw_value(index));
    if (at >= 0 && static_cast<std::size_t>(at) < this->size()) return Items::operator[](static_cast<std::size_t>(at));
    static const Item zero{};
    return zero;
  }
};

/// A list of a fixed length in a Blueprint's C++ (Fixed<Float, 16>): what a place is declared as
/// that always has that many entries - a variable, a field of a struct, an input, an output, a
/// local variable. It is std::array: it lies where it stands, no memory is asked for. Its
/// entries are read and set like a list's ([] never reaches beside it either); nothing changes
/// its length. Given a list, it takes that list's entries - cut at its length, or filled up with
/// the zero of its entries; where a list is needed it gives a list of its entries (a copy).
template <class T, std::size_t N> struct Fixed {
  using Item = typename detail::Entry<T>::type;
  std::array<Item, N> items{};
  Fixed() = default;
  Fixed(std::initializer_list<Item> given) { take(given.begin(), given.size()); }                              // Fixed<Float, 3>{1.0, 2.5}
  /// (each entry made where it stands: what Make List is given is handed over, not copied)
  template <class... V>
    requires(sizeof...(V) == N)
  explicit Fixed(std::in_place_t, V&&... entries) : items{Item(std::forward<V>(entries))...} {}
  Fixed(const std::vector<Item>& list) { take(list.data(), list.size()); }                                     // NOLINT (implicit: a list set at the place)
  template <std::size_t M> Fixed(const Fixed<T, M>& other) { take(other.items.data(), M); }                    // NOLINT (implicit: a list of another length)
  template <std::size_t M> Fixed(Fixed<T, M>&& other) {                                                        // NOLINT
    for (std::size_t k = 0; k < N && k < M; ++k) items[k] = std::move(other.items[k]);
  }
  Fixed(const Fixed&) = default;
  Fixed(Fixed&&) = default;
  Fixed& operator=(const Fixed&) = default;
  Fixed& operator=(Fixed&&) = default;
  operator List<T>() const& { return List<T>(typename List<T>::Items(items.begin(), items.end())); }           // NOLINT (implicit: its entries as a list)
  operator List<T>() && { return List<T>(typename List<T>::Items(std::make_move_iterator(items.begin()), std::make_move_iterator(items.end()))); }  // NOLINT
  template <class I>
    requires num::Integer<num::raw<I>>
  Item& operator[](I index) {
    const i64 at = static_cast<i64>(num::raw_value(index));
    if (at >= 0 && static_cast<std::size_t>(at) < N) return items[static_cast<std::size_t>(at)];
    return beside::entry<Item>();
  }
  template <class I>
    requires num::Integer<num::raw<I>>
  const Item& operator[](I index) const {
    const i64 at = static_cast<i64>(num::raw_value(index));
    if (at >= 0 && static_cast<std::size_t>(at) < N) return items[static_cast<std::size_t>(at)];
    static const Item zero{};
    return zero;
  }
  constexpr std::size_t size() const { return N; }
  const Item* data() const { return items.data(); }
  auto begin() const { return items.begin(); }
  auto end() const { return items.end(); }
  bool operator==(const Fixed&) const = default;

 private:
  void take(const Item* from, std::size_t count) {
    for (std::size_t k = 0; k < N && k < count; ++k) items[k] = from[k];
  }
};
/// The entries of a list as a node reads them - of a List or of a Fixed, where they stand:
/// nothing is copied (std::span).
template <class T> struct Items {
  using Item = typename detail::Entry<T>::type;
  const Item* first = nullptr;
  std::size_t count = 0;
  Items(const std::vector<Item>& list) : first(list.data()), count(list.size()) {}                   // NOLINT (implicit: a view of the list)
  template <std::size_t N> Items(const Fixed<T, N>& list) : first(list.data()), count(N) {}          // NOLINT
  std::size_t size() const { return count; }
  const Item* begin() const { return first; }
  const Item* end() const { return first + count; }
  /// The entry at `index`; beyond the list: the zero of its type.
  const Item& operator[](i64 index) const {
    if (index >= 0 && static_cast<u64>(index) < count) return first[index];
    static const Item zero{};
    return zero;
  }
};
/// The entries of a list, whatever its items are (a node over the items of a list of Ints of each width -
/// Text of Bytes, Copy Bytes: in C++ a function over the list's items).
template <class T> Items<T> items_of(const List<T>& list) { return list; }
template <class T, std::size_t N> Items<T> items_of(const Fixed<T, N>& list) { return list; }
template <class T> Items<T> items_of(Items<T> list) { return list; }

/// A value of the interpreter as the C++ type of its pin, and back. A number is the number of its type (the
/// C++ type itself), a list the vector of its items.
namespace detail {
template <class T> struct Held;  // (no such pin type)
/// A struct of a Blueprint's class (KERN_STRUCT). How its value is held says the class's glue,
/// field by field (logic::native_glue writes Held<Class::Struct>: is, from, to).
template <class S>
concept NativeStruct = !std::is_same_v<S, Value> && requires { requires Held<S>::type == PinType::structure; };
// A struct in the interpreter: its value as it is (Value::rows are its fields).
template <> struct Held<Value> {
  static constexpr PinType type = PinType::structure;
  static Value from(const Value& v) { return v; }
};
template <> struct Held<bool> {
  static constexpr PinType type = PinType::boolean;
  static bool from(const Value& v) { return v.as_bool(); }
};
template <class T>
  requires(num::Number<T> && !std::is_same_v<T, bool>)
struct Held<T> {
  static constexpr PinType type = number_type<T>();
  static T from(const Value& v) {
    // (a list of an enum holds its values' places as i64s: an enum's value is its place - no number is converted)
    if constexpr (std::is_same_v<T, i64>)
      if (v.type == PinType::enumeration) return v.i;
    return v.as<T>();
  }
};
// A number of the language in a Blueprint's class (numbers.h Typed): held as the number it is.
template <num::Number T> struct Held<num::Typed<T>> {
  static constexpr PinType type = Held<T>::type;
  static num::Typed<T> from(const Value& v) { return Held<T>::from(v); }
};
template <num::Number T> struct Held<std::vector<num::Typed<T>>> {
  static constexpr PinType type = Held<std::vector<T>>::type;
  static std::vector<num::Typed<T>> from(const Value& v) {
    const auto& items = v.items<T>();
    return std::vector<num::Typed<T>>(items.begin(), items.end());
  }
};
template <> struct Held<Vec3> {
  static constexpr PinType type = PinType::vector;
  static Vec3 from(const Value& v) { return v.as_vector(); }
};
template <> struct Held<std::string> {
  static constexpr PinType type = PinType::text;
  static std::string from(const Value& v) { return v.as_text(); }
};
template <> struct Held<Text> {
  static constexpr PinType type = PinType::text;
  static Text from(const Value& v) { return Text(v.as_text()); }
};
template <> struct Held<ActorHandle> {
  static constexpr PinType type = PinType::actor;
  static ActorHandle from(const Value& v) { return v.as_actor(); }
};
template <class I> struct Held<Object<I>> {
  static constexpr PinType type = PinType::object;
  static Object<I> from(const Value& v) { return Object<I>{v.as_object()}; }
};
template <> struct Held<ObjectRef> {
  static constexpr PinType type = PinType::object;
  static ObjectRef from(const Value& v) { return v.as_object(); }
};
// A list of objects of the engine of a class: held as the list of their references (ListItems).
template <class I> struct Held<std::vector<Object<I>>> {
  static constexpr PinType type = PinType::object_list;
  static std::vector<Object<I>> from(const Value& v) {
    std::vector<Object<I>> r;
    for (const ObjectRef& x : v.items<ObjectRef>()) r.push_back(Object<I>{x});
    return r;
  }
};
template <class T> struct Held<std::vector<T>> {
  static constexpr PinType type = list_type_of<T>();
  static std::vector<T> from(const Value& v) { return v.items<T>(); }
};
template <> struct Held<std::vector<Text>> {
  static constexpr PinType type = PinType::text_list;
  static std::vector<Text> from(const Value& v) {
    const std::vector<std::string>& items = v.items<std::string>();
    return std::vector<Text>(items.begin(), items.end());
  }
};
template <> struct Held<std::vector<Truth>> {
  static constexpr PinType type = PinType::boolean_list;
  static std::vector<Truth> from(const Value& v) {
    const std::vector<bool>& items = v.items<bool>();
    return std::vector<Truth>(items.begin(), items.end());
  }
};
/// How many values an enum of a Blueprint's class has (KERN_ENUM states their names).
template <class E> constexpr std::size_t enum_count() {
  const std::string_view names = kern_enum_names(E{});
  std::size_t count = names.empty() ? 0 : 1;
  for (char c : names) count += c == ',' ? 1 : 0;
  return count;
}
// An enum of a Blueprint's class: a value is its place among the enum's values. What comes in
// from outside and is none of them is the first (as Instance::set and Instance::call hold it).
template <class E>
  requires std::is_enum_v<E>
struct Held<E> {
  static constexpr PinType type = PinType::enumeration;
  static bool is(const Value& v) { return v.type == type && v.i >= 0 && static_cast<u64>(v.i) < enum_count<E>(); }
  static E from(const Value& v) {
    const i64 index = v.as_int();
    return static_cast<E>(index >= 0 && static_cast<u64>(index) < enum_count<E>() ? index : 0);
  }
};
template <class E>
  requires std::is_enum_v<E>
struct Held<std::vector<E>> {
  static constexpr PinType type = PinType::enumeration_list;
  static bool is(const Value& v) {
    if (v.type != type) return false;
    for (i64 index : v.items<i64>())
      if (index < 0 || static_cast<u64>(index) >= enum_count<E>()) return false;
    return true;
  }
  static std::vector<E> from(const Value& v) {
    std::vector<E> r;
    for (i64 index : v.items<i64>()) r.push_back(static_cast<E>(index >= 0 && static_cast<u64>(index) < enum_count<E>() ? index : 0));
    return r;
  }
};
// A list of structs of a Blueprint's class. What comes in from outside and is no struct of that
// type is the struct's default, item by item (as Instance::set and Instance::call hold it).
template <NativeStruct S> struct Held<std::vector<S>> {
  static constexpr PinType type = PinType::structure_list;
  static bool is(const Value& v) {
    if (v.type != type) return false;
    for (const Value& item : v.items<Value>())
      if (!Held<S>::is(item)) return false;
    return true;
  }
  static std::vector<S> from(const Value& v) {
    std::vector<S> r;
    for (const Value& item : v.items<Value>()) r.push_back(Held<S>::from(item));
    return r;
  }
};
// A list of a Blueprint's class (List<Float>, List<Weapon> ...): held as the vector of its items.
template <class T> struct Held<List<T>> : Held<typename List<T>::Items> {
  static List<T> from(const Value& v) { return List<T>(Held<typename List<T>::Items>::from(v)); }
};
// A list of a fixed length: held as the list of its entries (a value on a pin is a list).
template <class T, std::size_t N> struct Held<Fixed<T, N>> : Held<List<T>> {
  static Fixed<T, N> from(const Value& v) { return Fixed<T, N>(Held<List<T>>::from(v)); }
};
}  // namespace detail
template <class T> T from_value(const Value& v) { return detail::Held<T>::from(v); }
namespace detail {
template <class T> inline constexpr bool is_initializer_list = false;
template <class T> inline constexpr bool is_initializer_list<std::initializer_list<T>> = true;
}  // namespace detail
template <class T> constexpr PinType pin_type_of() { return detail::Held<T>::type; }
/// Is `v` a value of the C++ type T as a Blueprint holds it (logic::is_value_of: of that pin
/// type, an enum's value one of its values, a struct with a value of its type for every field)?
template <class T> bool is_value(const Value& v) {
  if constexpr (requires { detail::Held<T>::is(v); }) return detail::Held<T>::is(v);
  else return v.type == pin_type_of<T>();
}
inline Value to_value(bool v) { return Value::boolean(v); }
template <class T>
  requires(num::Number<T> && !std::is_same_v<T, bool>)
Value to_value(T v) {
  return Value::of_number(v);
}
template <num::Number T> Value to_value(num::Typed<T> v) { return to_value(v.v); }
template <num::Number T> Value to_value(const std::vector<num::Typed<T>>& v) { return Value::of(std::vector<T>(v.begin(), v.end())); }
inline Value to_value(Vec3 v) { return Value::vector(v); }
inline Value to_value(std::string v) { return Value::text(std::move(v)); }
inline Value to_value(const Text& v) { return Value::text(v.str()); }
inline Value to_value(Text&& v) { return Value::text(std::move(v).str()); }
inline Value to_value(ActorHandle v) { return Value::actor(v); }
template <class I> Value to_value(Object<I> v) { return Value::object(v.ref); }
inline Value to_value(ObjectRef v) { return Value::object(v); }
template <class I> Value to_value(std::vector<Object<I>> v) {
  std::vector<ObjectRef> refs;
  for (const Object<I>& x : v) refs.push_back(x.ref);
  return Value::of(std::move(refs));
}
inline Value to_value(Value v) { return v; }  // (a struct in the interpreter)
template <class T> Value to_value(std::vector<T> v) { return Value::of(std::move(v)); }
template <detail::NativeStruct S> Value to_value(const S& v) { return detail::Held<S>::to(v); }
template <detail::NativeStruct S> Value to_value(std::vector<S> v) {
  std::vector<Value> items;
  for (const S& item : v) items.push_back(detail::Held<S>::to(item));
  return Value::structures(std::move(items));
}
template <class E>
  requires std::is_enum_v<E>
Value to_value(E v) {
  return Value::enumeration(static_cast<i64>(v));
}
template <class E>
  requires std::is_enum_v<E>
Value to_value(std::vector<E> v) {
  std::vector<i64> places;
  for (E e : v) places.push_back(static_cast<i64>(e));
  return Value::enumerations(std::move(places));
}
inline Value to_value(const std::vector<Truth>& v) { return Value::of(std::vector<bool>(v.begin(), v.end())); }
inline Value to_value(const std::vector<Text>& v) {
  std::vector<std::string> items;
  items.reserve(v.size());
  for (const Text& item : v) items.push_back(item.str());
  return Value::of(std::move(items));
}
template <class T, std::size_t N> Value to_value(const Fixed<T, N>& v) { return to_value(List<T>(v)); }  // (a list of a fixed length: its entries as a list)

/// What the list nodes do, for a list of any item type - the node library and a Blueprint's C++
/// call these. Every one is defined for every index: there is no reading or writing past a
/// list's end.
namespace list {
template <class T> i64 length(const std::vector<T>& items) { return static_cast<i64>(items.size()); }
/// The item at `index`; beyond the list: the zero of its type.
template <class T> T get(const std::vector<T>& items, i64 index) {
  if (index < 0 || static_cast<u64>(index) >= items.size()) return T{};
  return T(items[static_cast<std::size_t>(index)]);
}
/// The first index of an item that equals `item`, or -1.
template <class T> i64 find(const std::vector<T>& items, const std::type_identity_t<T>& item) {
  for (std::size_t k = 0; k < items.size(); ++k) {
    const T& at = items[k];  // (read where it stands: comparing copies nothing)
    if (at == item) return static_cast<i64>(k);
  }
  return -1;
}
template <class T> bool contains(const std::vector<T>& items, const std::type_identity_t<T>& item) { return find(items, item) >= 0; }
/// Do two lists hold the same items in the same order: as many, and each equal to the one at its place -
/// the == of two std::vector of one type. (Read where they stand: comparing copies nothing.)
template <class T> bool equal(const std::vector<T>& a, const std::vector<T>& b) {
  if (a.size() != b.size()) return false;
  for (std::size_t k = 0; k < a.size(); ++k) {
    const T& x = a[k];
    const T& y = b[k];
    if (!(x == y)) return false;
  }
  return true;
}
/// Adds `item` at the end; its index.
template <class T> i64 add(std::vector<T>& items, std::type_identity_t<T> item) {
  items.push_back(std::move(item));
  return static_cast<i64>(items.size()) - 1;
}
/// Puts `item` in front of what is at `index` (before the first: at the start; past the end: at
/// the end).
template <class T> void insert(std::vector<T>& items, i64 index, std::type_identity_t<T> item) {
  const std::size_t at = index <= 0 ? 0 : static_cast<u64>(index) >= items.size() ? items.size() : static_cast<std::size_t>(index);
  items.insert(items.begin() + static_cast<std::ptrdiff_t>(at), std::move(item));
}
/// Takes the item at `index` out; beyond the list: nothing.
template <class T> void remove_at(std::vector<T>& items, i64 index) {
  if (index >= 0 && static_cast<u64>(index) < items.size()) items.erase(items.begin() + static_cast<std::ptrdiff_t>(index));
}
/// Takes every item that equals `item` out; was there one?
template <class T> bool remove(std::vector<T>& items, const Sought<T>& item) {
  std::size_t kept = 0;
  for (std::size_t k = 0; k < items.size(); ++k) {
    {
      const T& at = items[k];
      if (equal_item(at, item)) continue;
    }
    if (kept != k) items[kept] = std::move(items[k]);
    ++kept;
  }
  const bool removed = kept != items.size();
  items.resize(kept);
  return removed;
}
template <class T> void clear(std::vector<T>& items) { items.clear(); }
/// Sort List: the items in order - `less(a, b)`: a goes before b; without it, by their `<` -, items
/// that are equal in that order as they stood (a stable sort: the order it gives is the one order there
/// is, on every machine and in every tier).
template <class T, class Less> void sort(std::vector<T>& items, Less less) { std::stable_sort(items.begin(), items.end(), less); }
template <class T> void sort(std::vector<T>& items) { std::stable_sort(items.begin(), items.end()); }
/// The longest list Set Length makes and Reserve makes room for: a length beyond it is this
/// one (a length is defined for every number, like an index).
inline constexpr i64 kLongest = i64{1} << 24;
/// Makes the list `length` long in one go: cut at its end, or filled up with `value`. Less than
/// 0: empty. It does not grow beyond kLongest.
template <class T> void resize(std::vector<T>& items, i64 length, const std::type_identity_t<T>& value) {
  const std::size_t most = items.size() > static_cast<std::size_t>(kLongest) ? items.size() : static_cast<std::size_t>(kLongest);
  items.resize(length <= 0 ? 0 : static_cast<u64>(length) > most ? most : static_cast<std::size_t>(length), value);
}
/// Makes room for `count` items (std::vector::reserve): nothing one can read changes - the list
/// then grows that far without moving.
template <class T> void reserve(std::vector<T>& items, i64 count) {
  if (count > 0) items.reserve(static_cast<std::size_t>(count > kLongest ? kLongest : count));
}
/// Puts `count` items from `more` on, in their order, in front of what is at `index` (before the
/// first: at the start; past the end: at the end) - in one go. Items of the list itself: as they
/// were.
template <class T> void insert_items(std::vector<T>& items, i64 index, const T* more, std::size_t count) {
  const std::ptrdiff_t at = index <= 0 ? 0 : static_cast<u64>(index) >= items.size() ? static_cast<std::ptrdiff_t>(items.size()) : static_cast<std::ptrdiff_t>(index);
  if (count == 0) return;
  const std::less<const T*> before;
  if (!items.empty() && !before(more, items.data()) && before(more, items.data() + items.size())) {
    const std::vector<T> were(more, more + count);
    items.insert(items.begin() + at, were.begin(), were.end());
    return;
  }
  items.insert(items.begin() + at, more, more + count);
}
/// The same with the items of another list (a list into itself: its items as they were).
template <class T> void insert_list(std::vector<T>& items, i64 index, const std::vector<T>& more) {
  const std::ptrdiff_t at = index <= 0 ? 0 : static_cast<u64>(index) >= items.size() ? static_cast<std::ptrdiff_t>(items.size()) : static_cast<std::ptrdiff_t>(index);
  if (&more == &items) {
    const std::vector<T> were = more;
    items.insert(items.begin() + at, were.begin(), were.end());
    return;
  }
  items.insert(items.begin() + at, more.begin(), more.end());
}
/// Adds the items of `more` at the end, in one go.
template <class T> void append(std::vector<T>& items, const std::vector<T>& more) { insert_list(items, static_cast<i64>(items.size()), more); }
}  // namespace list

/// A Map in a Blueprint's C++ (Map<Int, Float>, Map<Int, List<Int>> ...): kern::Map - its values
/// under their keys, in the order the keys came. A value is a place by its entry's place in that
/// order (Cells[i], Cells[i].Count += 1): [] never reaches beside the map - there it gives an
/// entry of its own (beside), as a List's does. Its keys are found by the map's nodes (map::).
template <class K, class V> struct Map : kern::Map<K, typename detail::Entry<V>::type> {
  using Item = typename detail::Entry<V>::type;
  using Entries = kern::Map<K, Item>;
  using Entries::Entries;
  Map() = default;
  template <class I>
    requires num::Integer<num::raw<I>>
  Item& operator[](I index) {
    const i64 at = static_cast<i64>(num::raw_value(index));
    if (at >= 0 && static_cast<std::size_t>(at) < this->size()) return this->value_at(static_cast<std::size_t>(at));
    return beside::entry<Item>();
  }
  template <class I>
    requires num::Integer<num::raw<I>>
  const Item& operator[](I index) const {
    const i64 at = static_cast<i64>(num::raw_value(index));
    if (at >= 0 && static_cast<std::size_t>(at) < this->size()) return this->value_at(static_cast<std::size_t>(at));
    static const Item zero{};
    return zero;
  }
};
/// A Set in a Blueprint's C++ (Set<Int>): kern::Set - its keys in the order they came.
template <class K> struct Set : kern::Set<K> {
  using kern::Set<K>::Set;
  Set() = default;
};

// A Map and a Set of a Blueprint's class as the interpreter holds them: their entries, keys and
// values each as the value it is.
namespace detail {
template <class K, class V> struct Held<Map<K, V>> {
  static constexpr PinType type = PinType::map;
  static bool is(const Value& v) {
    if (v.type != type) return false;
    for (std::size_t k = 0; k < v.size(); ++k)
      if (!is_value_of<K>(key_value(v.keyed().key_at(k), Held<K>::type)) || !is_value_of<V>(v.keyed().value_at(k))) return false;
    return true;
  }
  static Map<K, V> from(const Value& v) {
    Map<K, V> m;
    m.reserve(v.size());
    for (std::size_t k = 0; k < v.size(); ++k) m.try_emplace(Held<K>::from(key_value(v.keyed().key_at(k), Held<K>::type)), Held<V>::from(v.keyed().value_at(k)));
    return m;
  }
  template <class T> static bool is_value_of(const Value& x) {
    if constexpr (requires { Held<T>::is(x); }) return Held<T>::is(x);
    else return x.type == Held<T>::type;
  }
};
template <class K> struct Held<Set<K>> {
  static constexpr PinType type = PinType::set;
  static bool is(const Value& v) {
    if (v.type != type) return false;
    for (std::size_t k = 0; k < v.size(); ++k)
      if (!Held<Map<K, bool>>::template is_value_of<K>(key_value(v.keyed().key_at(k), Held<K>::type))) return false;
    return true;
  }
  static Set<K> from(const Value& v) {
    Set<K> s;
    s.reserve(v.size());
    for (std::size_t k = 0; k < v.size(); ++k) s.insert(Held<K>::from(key_value(v.keyed().key_at(k), Held<K>::type)));
    return s;
  }
};
}  // namespace detail
template <class K, class V> Value to_value(const Map<K, V>& m) {
  Value r = Value::zero(PinType::map);
  if (m.empty()) return r;
  MapEntries& e = r.own_entries();
  e.reserve(m.size());
  for (const auto& [k, v] : m) e.try_emplace(key_of(to_value(k)), to_value(V(v)));
  return r;
}
template <class K> Value to_value(const Set<K>& s) {
  Value r = Value::zero(PinType::set);
  if (s.empty()) return r;
  MapEntries& e = r.own_entries();
  e.reserve(s.size());
  for (const K& k : s) e.try_emplace(key_of(to_value(k)));
  return r;
}

/// A Callback in a Blueprint's C++ (Callback<Float(Float, Float)>, Make Callback: callback<&Lib::fn>(place)): a
/// function the engine calls and the place it is bound to - two words, as a function pointer with its void* is
/// (kern::HeightSampler). fn is a thunk that calls the library's function on that place; none: unbound (a call
/// gives the zero of R).
template <class S> struct Callback;
template <class R, class... A> struct Callback<R(A...)> {
  R (*fn)(void*, A...) = nullptr;
  void* place = nullptr;
  /// (bound or not - as the interpreter's value says: what it calls, a call says. A Blueprint does not compare one;
  /// a struct that holds one is equal field by field)
  friend bool operator==(const Callback& x, const Callback& y) { return !x.fn == !y.fn; }
  R operator()(A... a) const {
    if constexpr (std::is_void_v<R>) {
      if (fn) fn(place, a...);
    } else {
      return fn ? fn(place, a...) : R{};
    }
  }
};
namespace detail {
template <class S> inline constexpr bool is_callback = false;
template <class S> inline constexpr bool is_callback<Callback<S>> = true;
/// A Callback of a Blueprint's class as a value (the door to the engine, a stand-in in a test): calls the thunk
/// with the values made into its C++ types.
template <class R, class... A> struct NativeCallable final : Callable {
  Callback<R(A...)> held;
  explicit NativeCallable(Callback<R(A...)> c) : held(c) {}
  static constexpr char kind = 0;
  const void* native_kind() const override { return &kind; }
  bool call(std::span<const Value> args, Value& result) const override { return call(args, result, std::index_sequence_for<A...>{}); }
  template <std::size_t... I> bool call(std::span<const Value> args, Value& result, std::index_sequence<I...>) const {
    if (!held.fn) return false;
    if constexpr (std::is_void_v<R>) held.fn(held.place, (I < args.size() ? Held<A>::from(args[I].converted(Held<A>::type)) : A{})...);
    else result = to_value(held.fn(held.place, (I < args.size() ? Held<A>::from(args[I].converted(Held<A>::type)) : A{})...));
    return true;
  }
};
template <class R, class... A> struct Held<Callback<R(A...)>> {
  static constexpr PinType type = PinType::callback;
  /// (one the class made, back as it was; one the interpreter made calls the Blueprint there - in a class it is
  /// unbound: a Callback goes from a Blueprint to the engine, not from one tier into another)
  static Callback<R(A...)> from(const Value& v) {
    if (v.call && v.call->native_kind() == &NativeCallable<R, A...>::kind) return static_cast<const NativeCallable<R, A...>&>(*v.call).held;
    return {};
  }
};
}  // namespace detail
template <class R, class... A> Value to_value(const Callback<R(A...)>& c) {
  return Value::callback(c.fn ? std::make_shared<detail::NativeCallable<R, A...>>(c) : nullptr);
}

/// What the nodes of a map do - on a Map of a Blueprint's class and on the interpreter's entries
/// (MapEntries) alike: the same steps on the same container.
namespace map {
template <class M> i64 length(const M& m) { return static_cast<i64>(m.size()); }
template <class M> bool contains(const M& m, const typename M::key_type& key) { return m.contains(key); }
/// The place of the key's entry in the order of the keys, -1: there is none.
template <class M> i64 index_of(const M& m, const typename M::key_type& key) {
  const std::size_t at = m.index_of(key);
  return at == M::npos ? -1 : static_cast<i64>(at);
}
/// The key of the entry at `at`; beyond the map: the zero of the keys' type.
template <class M> typename M::key_type key_at(const M& m, i64 at) {
  if (at < 0 || static_cast<u64>(at) >= m.size()) return typename M::key_type{};
  return m.key_at(static_cast<std::size_t>(at));
}
/// The key's value (none: the zero of the values' type): (is there one, the value, its place - -1).
template <class M> std::tuple<bool, typename M::mapped_type, i64> find(const M& m, const typename M::key_type& key) {
  const i64 at = index_of(m, key);
  if (at < 0) return {false, typename M::mapped_type{}, -1};
  return {true, m.value_at(static_cast<std::size_t>(at)), at};
}
/// Set: the key gets the value - a new entry at the end when there is none. (was it new, its place)
template <class M> std::pair<bool, i64> set(M& m, const typename M::key_type& key, typename M::mapped_type value) {
  auto [it, fresh] = m.insert_or_assign(key, std::move(value));
  return {fresh, static_cast<i64>(it - m.begin())};
}
/// Add: a new entry at the end with the zero of the values' type, unless the key has one. (was it new, its place)
template <class M> std::pair<bool, i64> add(M& m, const typename M::key_type& key) {
  auto [it, fresh] = m.try_emplace(key);
  return {fresh, static_cast<i64>(it - m.begin())};
}
/// Takes the key's entry out (the others keep their order); was there one?
template <class M> bool remove(M& m, const typename M::key_type& key) { return m.erase(key) > 0; }
/// Takes the entry at `at` out; beyond the map: nothing.
template <class M> void remove_at(M& m, i64 at) {
  if (at >= 0 && static_cast<u64>(at) < m.size()) m.erase_at(static_cast<std::size_t>(at));
}
template <class M> void clear(M& m) { m.clear(); }
/// Room for `count` entries (no more than list::kLongest).
template <class M> void reserve(M& m, i64 count) { m.reserve(count <= 0 ? 0 : count > list::kLongest ? static_cast<std::size_t>(list::kLongest) : static_cast<std::size_t>(count)); }
}  // namespace map

/// What the nodes of a set do (see map::).
namespace set {
template <class S> i64 length(const S& s) { return static_cast<i64>(s.size()); }
template <class S> bool contains(const S& s, const typename S::key_type& key) { return s.contains(key); }
template <class S> i64 index_of(const S& s, const typename S::key_type& key) {
  const std::size_t at = s.index_of(key);
  return at == S::npos ? -1 : static_cast<i64>(at);
}
template <class S> typename S::key_type key_at(const S& s, i64 at) {
  if (at < 0 || static_cast<u64>(at) >= s.size()) return typename S::key_type{};
  return s.key_at(static_cast<std::size_t>(at));
}
/// Add: the key at the end, unless it is there. (was it new, its place)
template <class S> std::pair<bool, i64> add(S& s, const typename S::key_type& key) {
  auto [it, fresh] = s.insert(key);
  return {fresh, static_cast<i64>(it - s.begin())};
}
template <class S> bool remove(S& s, const typename S::key_type& key) { return s.erase(key) > 0; }
template <class S> void remove_at(S& s, i64 at) {
  if (at >= 0 && static_cast<u64>(at) < s.size()) s.erase_at(static_cast<std::size_t>(at));
}
template <class S> void clear(S& s) { s.clear(); }
template <class S> void reserve(S& s, i64 count) { s.reserve(count <= 0 ? 0 : count > list::kLongest ? static_cast<std::size_t>(list::kLongest) : static_cast<std::size_t>(count)); }
}  // namespace set

/// What the enum nodes do. An enum's values have names; a value is its place among them (the
/// first is 0). The node library and a Blueprint's C++ call these.
/// Format Text (text.format): a text made from a format as C's printf has it and values - character
/// for character what snprintf gives in the C locale. The format says the conversions; each takes
/// one value, of the type it names:
///   %d %i            Int, as a signed number   (hh h: 8 / 16 bit, none: 32 bit, l ll j z t: 64 bit)
///   %u %x %X %o      Int, as an unsigned one   (the same widths)
///   %c               Int, the character of that code (8 bit)
///   %f %F %e %E %g %G Float
///   %s               Text
///   %%               a % sign
/// with the flags - + space 0 #, a width and a precision as numbers (not *). Numbers are written
/// exactly rounded (std::to_chars, which is printf's rounding: the decimal nearest the binary
/// value, ties to even) - the same text on every machine. `most`: at most that many bytes of it
/// (snprintf's size - 1); less than 0: all of it.
namespace format {
/// A value a conversion takes.
struct Arg {
  enum class Kind : u8 { whole, number, text } kind = Kind::whole;
  i64 i = 0;
  f64 f = 0;
  std::string_view s;
};
inline Arg arg(i64 v) { return {Arg::Kind::whole, v, 0, {}}; }
/// (a whole number of another type: its bits as an i64 - what %d, %u, %x read of them as C's varargs do; f32 is promoted to double, as there)
template <class T>
  requires(num::Integer<T> && !std::is_same_v<T, i64>)
Arg arg(T v) {
  return arg(static_cast<i64>(num::bits_of(v)));
}
inline Arg arg(f32 v) { return {Arg::Kind::number, 0, static_cast<f64>(v), {}}; }
inline Arg arg(f64 v) { return {Arg::Kind::number, 0, v, {}}; }
inline Arg arg(std::string_view v) { return {Arg::Kind::text, 0, 0, v}; }
/// One conversion of a format.
struct Spec {
  bool left = false, plus = false, space = false, zero = false, alt = false;
  int width = 0, precision = -1;  // (-1: none given)
  int bits = 32;                  // of a whole number: 8, 16, 32 or 64
  char conv = 0;
};
/// The longest precision and width a format may say (what a number can be written with in one piece).
inline constexpr int kLongest = 512;
/// The type of value a conversion takes.
inline Arg::Kind kind_of(char conv) {
  switch (conv) {
    case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': return Arg::Kind::number;
    case 's': return Arg::Kind::text;
    default: return Arg::Kind::whole;
  }
}
/// Reads the conversion at fmt[at] (just after its %). False: it is none this format knows - `why` says.
inline bool read_spec(std::string_view fmt, std::size_t& at, Spec& sp, std::string* why) {
  auto fail = [&](const char* w) {
    if (why) *why = w;
    return false;
  };
  for (; at < fmt.size(); ++at) {
    const char c = fmt[at];
    if (c == '-') sp.left = true;
    else if (c == '+') sp.plus = true;
    else if (c == ' ') sp.space = true;
    else if (c == '0') sp.zero = true;
    else if (c == '#') sp.alt = true;
    else break;
  }
  auto number = [&](int& out) {
    out = 0;
    while (at < fmt.size() && fmt[at] >= '0' && fmt[at] <= '9') {
      out = out * 10 + (fmt[at++] - '0');
      if (out > kLongest) return false;
    }
    return true;
  };
  if (at < fmt.size() && fmt[at] == '*') return fail("a width given by a value (*) is not supported: write the width out");
  if (!number(sp.width)) return fail("a width of more than 512");
  if (at < fmt.size() && fmt[at] == '.') {
    ++at;
    if (at < fmt.size() && fmt[at] == '*') return fail("a precision given by a value (.*) is not supported: write the precision out");
    if (!number(sp.precision)) return fail("a precision of more than 512");
  }
  if (at < fmt.size() && fmt[at] == 'h') {
    sp.bits = 16;
    if (++at < fmt.size() && fmt[at] == 'h') sp.bits = 8, ++at;
  } else if (at < fmt.size() && (fmt[at] == 'l' || fmt[at] == 'j' || fmt[at] == 'z' || fmt[at] == 't')) {
    sp.bits = 64;
    if (fmt[at] == 'l' && at + 1 < fmt.size() && fmt[at + 1] == 'l') ++at;
    ++at;
  }
  if (at >= fmt.size()) return fail("the format ends inside a conversion (a % without its letter)");
  sp.conv = fmt[at++];
  switch (sp.conv) {
    case 'd': case 'i': case 'u': case 'x': case 'X': case 'o': case 'c': case 's':
    case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': case '%': return true;
    default: return fail("a conversion that is not supported (d i u x X o c s f F e E g G %)");
  }
}
/// The types of the values a format takes, in their order. False: the format is none - `why` says.
inline bool kinds(std::string_view fmt, std::vector<Arg::Kind>* out, std::string* why) {
  for (std::size_t at = 0; at < fmt.size();) {
    if (fmt[at++] != '%') continue;
    Spec sp;
    if (!read_spec(fmt, at, sp, why)) return false;
    if (sp.conv != '%' && out) out->push_back(kind_of(sp.conv));
  }
  return true;
}
/// Where the characters go: counted only (to = nullptr), or written.
struct Sink {
  char* to = nullptr;
  std::size_t at = 0;
  void put(char c) {
    if (to) to[at] = c;
    ++at;
  }
  void put(std::string_view t) {
    for (char c : t) put(c);
  }
  void fill(char c, int n) {
    for (int k = 0; k < n; ++k) put(c);
  }
};
/// Writes one conversion: its sign or prefix, its digits, padded to the width.
inline void pad(Sink& out, const Spec& sp, std::string_view sign, std::string_view digits, bool zeros) {
  const int len = static_cast<int>(sign.size() + digits.size());
  const int room = sp.width > len ? sp.width - len : 0;
  if (!sp.left && !zeros) out.fill(' ', room);
  out.put(sign);
  if (!sp.left && zeros) out.fill('0', room);
  out.put(digits);
  if (sp.left) out.fill(' ', room);
}
inline void put_whole(Sink& out, const Spec& sp, i64 v) {
  const bool is_signed = sp.conv == 'd' || sp.conv == 'i';
  u64 u = 0;
  bool negative = false;
  if (is_signed) {
    const i64 x = sp.bits == 8 ? static_cast<i64>(static_cast<i8>(v)) : sp.bits == 16 ? static_cast<i64>(static_cast<i16>(v)) : sp.bits == 32 ? static_cast<i64>(static_cast<i32>(v)) : v;
    negative = x < 0;
    u = negative ? u64{0} - static_cast<u64>(x) : static_cast<u64>(x);
  } else {
    u = sp.bits == 8 ? static_cast<u64>(static_cast<u8>(v)) : sp.bits == 16 ? static_cast<u64>(static_cast<u16>(v)) : sp.bits == 32 ? static_cast<u64>(static_cast<u32>(v)) : static_cast<u64>(v);
  }
  const unsigned base = sp.conv == 'x' || sp.conv == 'X' ? 16 : sp.conv == 'o' ? 8 : 10;
  const char* const digit_chars = sp.conv == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
  char buf[kLongest + 32];
  int n = 0;
  for (u64 x = u; x; x /= base) buf[n++] = digit_chars[x % base];
  const int least = sp.precision < 0 ? 1 : sp.precision;
  while (n < least) buf[n++] = '0';  // (precision 0 and the value 0: no digit at all)
  if (sp.alt && base == 8 && (n == 0 || buf[n - 1] != '0')) buf[n++] = '0';
  char digits[kLongest + 32];
  for (int k = 0; k < n; ++k) digits[k] = buf[n - 1 - k];
  std::string_view sign;
  if (is_signed) sign = negative ? "-" : sp.plus ? "+" : sp.space ? " " : "";
  else if (sp.alt && base == 16 && u != 0) sign = sp.conv == 'X' ? "0X" : "0x";
  pad(out, sp, sign, {digits, static_cast<std::size_t>(n)}, sp.zero && !sp.left && sp.precision < 0);
}
inline void put_number(Sink& out, const Spec& sp, f64 v) {
  const bool upper = sp.conv == 'F' || sp.conv == 'E' || sp.conv == 'G';
  const std::string_view sign = std::signbit(v) ? "-" : sp.plus ? "+" : sp.space ? " " : "";
  const f64 a = std::fabs(v);
  if (!std::isfinite(a)) {
    pad(out, sp, sign, std::isnan(a) ? (upper ? "NAN" : "nan") : (upper ? "INF" : "inf"), false);
    return;
  }
  const int p = sp.precision < 0 ? 6 : sp.precision;
  char buf[kLongest + 400];
  char* const end = buf + sizeof buf - 2;  // (room for a point put in afterwards)
  std::to_chars_result r{};
  char conv = static_cast<char>(sp.conv | 0x20);  // (lower case)
  bool strip = false;
  if (conv == 'g') {
    // %g: %e or %f by the exponent the number has with p significant digits (C 7.21.6.1)
    const int sig = p == 0 ? 1 : p;
    char e[64];
    const std::to_chars_result er = std::to_chars(e, e + sizeof e, a, std::chars_format::scientific, sig - 1);
    const char* ep = e;
    while (ep < er.ptr && *ep != 'e') ++ep;
    int x = 0;
    {
      const char* q = ep + 1;
      const bool minus = q < er.ptr && *q == '-';
      if (q < er.ptr && (*q == '-' || *q == '+')) ++q;
      for (; q < er.ptr; ++q) x = x * 10 + (*q - '0');
      if (minus) x = -x;
    }
    if (sig > x && x >= -4) r = std::to_chars(buf, end, a, std::chars_format::fixed, sig - 1 - x), conv = 'f';
    else r = std::to_chars(buf, end, a, std::chars_format::scientific, sig - 1), conv = 'e';
    strip = !sp.alt;
  } else {
    r = std::to_chars(buf, end, a, conv == 'f' ? std::chars_format::fixed : std::chars_format::scientific, p);
  }
  char* last = r.ptr;
  char* mark = buf;  // (where the exponent begins, or the end)
  while (mark < last && *mark != 'e') ++mark;
  std::size_t mant = static_cast<std::size_t>(mark - buf);
  bool point = false;
  for (std::size_t k = 0; k < mant; ++k) point = point || buf[k] == '.';
  if (strip && point) {
    std::size_t keep = mant;
    while (keep > 0 && buf[keep - 1] == '0') --keep;
    if (keep > 0 && buf[keep - 1] == '.') --keep;
    for (char* q = mark; q < last; ++q) buf[keep + static_cast<std::size_t>(q - mark)] = *q;  // (the exponent moves up)
    last = buf + keep + static_cast<std::size_t>(last - mark);
    mark = buf + keep;
    mant = keep;
  } else if (sp.alt && !point) {
    for (char* q = last; q > mark; --q) *q = q[-1];  // (# : the point stays, also without a digit after it)
    *mark = '.';
    ++last;
  }
  if (upper)
    for (char* q = buf; q < last; ++q)
      if (*q == 'e') *q = 'E';
  pad(out, sp, sign, {buf, static_cast<std::size_t>(last - buf)}, sp.zero && !sp.left);
}
inline void put_text(Sink& out, const Spec& sp, std::string_view s) {
  if (sp.precision >= 0 && static_cast<std::size_t>(sp.precision) < s.size()) s = s.substr(0, static_cast<std::size_t>(sp.precision));
  pad(out, sp, {}, s, false);
}
/// The format with these values, into `out` (nullptr: only counted). How many bytes it has. A value a
/// conversion does not have: the zero of its type; a value of another type: converted as a wire does.
inline std::size_t write(char* to, std::string_view fmt, const Arg* args, std::size_t count) {
  Sink out{to, 0};
  std::size_t next = 0;
  for (std::size_t at = 0; at < fmt.size();) {
    const char c = fmt[at++];
    if (c != '%') {
      out.put(c);
      continue;
    }
    Spec sp;
    if (!read_spec(fmt, at, sp, nullptr)) break;  // (a format that is none: what stands before it - the check reports it)
    if (sp.conv == '%') {
      out.put('%');
      continue;
    }
    const Arg a = next < count ? args[next] : Arg{kind_of(sp.conv), 0, 0, {}};
    ++next;
    switch (kind_of(sp.conv)) {
      case Arg::Kind::number: put_number(out, sp, a.kind == Arg::Kind::number ? a.f : static_cast<f64>(a.i)); break;
      case Arg::Kind::text: put_text(out, sp, a.s); break;
      case Arg::Kind::whole:
        if (sp.conv == 'c') {
          const char ch = static_cast<char>(static_cast<u8>(a.kind == Arg::Kind::whole ? a.i : logic::whole(a.f)));
          pad(out, sp, {}, {&ch, 1}, false);
        } else {
          put_whole(out, sp, a.kind == Arg::Kind::whole ? a.i : logic::whole(a.f));
        }
        break;
    }
  }
  return out.at;
}
/// The text: counted, then written in one piece (one request for memory at most, as a std::string
/// of that length makes).
inline std::string text_of(std::string_view fmt, const Arg* args, std::size_t count, i64 most) {
  std::size_t n = write(nullptr, fmt, args, count);
  std::string r(n, '\0');
  write(r.data(), fmt, args, count);
  if (most >= 0 && static_cast<u64>(most) < n) r.resize(static_cast<std::size_t>(most));
  return r;
}
}  // namespace format

/// The text nodes that read a text as C++ reads a std::string or a std::string_view - the same
/// answers, character for character. A place in a text is an Int; where C++ gives npos (a size_t
/// that is no place), the Int is -1 - the size_t read with its sign, as a Blueprint holds every
/// size_t -, and a place handed in is read back the same way (-1: npos, "to the end").
namespace texts {
inline constexpr std::size_t npos = std::string_view::npos;
/// An Int as the size_t C++ has there (a whole number below 0 is that size_t read with its sign).
constexpr std::size_t place(i64 at) { return static_cast<std::size_t>(static_cast<u64>(at)); }
/// A size_t C++ gives as the Int that holds it (npos: -1).
constexpr i64 found(std::size_t at) { return at == npos ? -1 : static_cast<i64>(at); }
inline i64 find(std::string_view a, std::string_view b, i64 from) { return found(a.find(b, place(from))); }
inline i64 rfind(std::string_view a, std::string_view b, i64 from) { return found(a.rfind(b, place(from))); }
inline i64 find_first_of(std::string_view a, std::string_view b, i64 from) { return found(a.find_first_of(b, place(from))); }
inline i64 find_last_of(std::string_view a, std::string_view b, i64 from) { return found(a.find_last_of(b, place(from))); }
inline i64 find_first_not_of(std::string_view a, std::string_view b, i64 from) { return found(a.find_first_not_of(b, place(from))); }
inline i64 find_last_not_of(std::string_view a, std::string_view b, i64 from) { return found(a.find_last_not_of(b, place(from))); }
/// substr(at, count): from `at` so many characters (count beyond the end, or -1: to the end). An
/// `at` beyond the end gives no text (C++ throws std::out_of_range there - which ends a program
/// that has no exceptions).
constexpr std::string_view sub(std::string_view a, i64 at, i64 count) {
  const std::size_t p = place(at);
  if (p > a.size()) return {};
  return a.substr(p, place(count));
}
/// The ranges insert, erase and replace work on, as C++ cuts them: `at` beyond the end - C++
/// throws std::out_of_range - changes nothing.
inline void erase(std::string& a, i64 at, i64 count) {
  if (place(at) <= a.size()) a.erase(place(at), place(count));
}
inline void insert(std::string& a, i64 at, std::string_view b) {
  if (place(at) <= a.size()) a.insert(place(at), b);
}
inline void replace(std::string& a, i64 at, i64 count, std::string_view b) {
  if (place(at) <= a.size()) a.replace(place(at), place(count), b);
}
/// (no longer than 2^24 characters: a length is defined for every number, as Set Length's - list::kLongest)
inline constexpr i64 kLongest = i64{1} << 24;
inline std::string repeat(std::string_view a, i64 count) {
  std::string r;
  if (count <= 0 || a.empty()) return r;
  const i64 times = std::min(count, kLongest / static_cast<i64>(a.size()));
  r.reserve(a.size() * static_cast<std::size_t>(times));
  for (i64 k = 0; k < times; ++k) r.append(a);
  return r;
}
/// The bytes of a text as whole numbers 0 .. 255 (each char as an unsigned char), put into a list of whole
/// numbers in front of the entry at `index` (0: at the start; past the end: at the end) - in one go, as
/// std::vector::insert of the text's range: what `bytes.insert(bytes.end(), text.begin(), text.end())` does.
template <class W> void insert_bytes(std::vector<W>& items, i64 index, std::string_view text) {
  const std::size_t at = index <= 0 ? 0 : static_cast<u64>(index) >= items.size() ? items.size() : static_cast<std::size_t>(index);
  items.insert(items.begin() + static_cast<std::ptrdiff_t>(at), text.size(), W{});
  for (std::size_t k = 0; k < text.size(); ++k) items[at + k] = W(static_cast<i64>(static_cast<u8>(text[k])));
}
template <class List> std::string of_bytes(const List& bytes, i64 at, i64 count) {
  std::string r;
  if (at < 0 || count <= 0 || static_cast<u64>(at) >= bytes.size()) return r;
  const std::size_t from = static_cast<std::size_t>(at);
  const std::size_t n = std::min(static_cast<std::size_t>(static_cast<u64>(count)), bytes.size() - from);
  r.resize(n);
  for (std::size_t k = 0; k < n; ++k) r[k] = static_cast<char>(static_cast<u8>(static_cast<i64>(num::raw_value(bytes[from + k]))));
  return r;
}
}  // namespace texts

/// Parse Float, Parse Int and Scan Text (text.parse_float, text.parse_int, text.scan): numbers and
/// words read from a text as C reads them - strtod, strtof, strtoll, strtoull (and with them atof,
/// atoi, atol, strtol, strtoul of a 64-bit long) and sscanf -, character for character what glibc
/// (2.39, in the mode of C23 that C++ compiles with: "0b" in front of a binary number) reads in the C
/// locale: the same characters read, the same value, the same count. The text is read as a C string
/// at a place (s.c_str() + at): from `at` up to its first zero byte; an `at` beyond the text (or
/// below 0 - in C a pointer to nowhere) reads an empty one. A finite number's value is what the C
/// library computes from the characters read (correctly rounded, so on every machine the same);
/// infinity, NaN and the payload of a NaN ("nan(0x7f)") are worked out here, as glibc does.
namespace scan {
inline constexpr int kEnd = -1;  // (no character: where C's EOF is)
constexpr bool space(int c) { return c == ' ' || (c >= '\t' && c <= '\r'); }
constexpr bool digit(int c) { return c >= '0' && c <= '9'; }
constexpr int lower(int c) { return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c; }
constexpr bool xdigit(int c) { return digit(c) || (lower(c) >= 'a' && lower(c) <= 'f'); }
/// A digit's value in a base up to 36 (no digit: 99).
constexpr int digit_value(int c) { return digit(c) ? c - '0' : lower(c) >= 'a' && lower(c) <= 'z' ? lower(c) - 'a' + 10 : 99; }
/// The C string a call reads at a place of a text.
constexpr std::string_view c_string(std::string_view s, i64 at) {
  if (at < 0 || static_cast<u64>(at) > s.size()) return {};
  s.remove_prefix(static_cast<std::size_t>(at));
  const std::size_t zero = s.find('\0');
  return zero == std::string_view::npos ? s : s.substr(0, zero);
}

/// What strtoll / strtoull read: the 64 bits, and how many characters (0: none - no number).
struct Whole {
  u64 bits = 0;
  std::size_t end = 0;
};
/// strtoll (is_signed) or strtoull of a C string, in a base 2..36 or 0 (by the number's own prefix:
/// 0x hexadecimal, 0b binary - with `binary` -, 0 octal, else decimal). Beyond the range: the end of
/// it (signed: the largest or the least i64; unsigned: all ones); unsigned, a - in front is the
/// two's complement. Another base reads nothing (glibc: EINVAL).
constexpr Whole whole(std::string_view s, int base, bool is_signed, bool binary = true) {
  Whole r;
  if (base < 0 || base == 1 || base > 36) return r;
  auto at = [&](std::size_t k) -> int { return k < s.size() ? static_cast<unsigned char>(s[k]) : 0; };
  std::size_t i = 0;
  while (space(at(i))) ++i;
  bool minus = false;
  if (at(i) == '-' || at(i) == '+') minus = at(i) == '-', ++i;
  std::size_t prefix = 0;  // (just after a 0x or 0b that was taken as one)
  if (at(i) == '0') {
    if ((base == 0 || base == 16) && lower(at(i + 1)) == 'x') i += 2, base = 16, prefix = i;
    else if (binary && (base == 0 || base == 2) && lower(at(i + 1)) == 'b') i += 2, base = 2, prefix = i;
    else if (base == 0) base = 8;
  } else if (base == 0) {
    base = 10;
  }
  const std::size_t first = i;
  const u64 b = static_cast<u64>(base);
  u64 v = 0;
  bool over = false;
  for (; i < s.size() && digit_value(at(i)) < base; ++i) {
    const u64 d = static_cast<u64>(digit_value(at(i)));
    if (v > (~u64{0} - d) / b) over = true;
    else if (!over) v = v * b + d;
  }
  if (i == first) {
    r.end = prefix ? prefix - 1 : 0;  // (a 0x or 0b with no digit after it: the 0 is the number)
    return r;
  }
  r.end = i;
  if (is_signed) {
    const u64 top = minus ? u64{1} << 63 : (u64{1} << 63) - 1;
    r.bits = over || v > top ? top : minus ? u64{0} - v : v;
  } else {
    r.bits = over ? ~u64{0} : minus ? u64{0} - v : v;
  }
  return r;
}

/// What strtod / strtof read: the value (a float's as a double holds it), and how many characters
/// (0: none - no number).
struct Real {
  f64 value = 0;
  std::size_t end = 0;
};
/// The value of a finite number written out, as the C library computes it (correctly rounded):
/// strtof with `single`, else strtod. `subject` is all of it, with its sign.
inline f64 finite(std::string_view subject, bool single) {
  char held[256];
  std::string longer;
  const char* text = held;
  if (subject.size() < sizeof held) {
    for (std::size_t k = 0; k < subject.size(); ++k) held[k] = subject[k];
    held[subject.size()] = '\0';
  } else {
    longer.assign(subject);
    text = longer.c_str();
  }
  return single ? static_cast<f64>(std::strtof(text, nullptr)) : std::strtod(text, nullptr);
}
/// strtod (or strtof, `single`) of a C string: the characters C reads as a number - spaces, a sign,
/// then decimal digits with a point and an exponent (e), hexadecimal ones after 0x (exponent p),
/// inf, infinity, nan, nan(chars) - and its value. NaN is quiet, its payload the number in the
/// parentheses where they hold one (glibc's way).
inline Real real(std::string_view s, bool single) {
  Real r;
  auto at = [&](std::size_t k) -> int { return k < s.size() ? static_cast<unsigned char>(s[k]) : 0; };
  auto word = [&](std::size_t k, std::string_view w) {
    for (std::size_t j = 0; j < w.size(); ++j)
      if (lower(at(k + j)) != w[j]) return false;
    return true;
  };
  std::size_t i = 0;
  while (space(at(i))) ++i;
  const std::size_t subject = i;
  bool minus = false;
  if (at(i) == '-' || at(i) == '+') minus = at(i) == '-', ++i;
  if (!(at(i) == '.' && digit(at(i + 1))) && !digit(at(i))) {
    if (word(i, "inf")) {
      r.end = i + (word(i + 3, "inity") ? 8 : 3);
      r.value = minus ? -std::numeric_limits<f64>::infinity() : std::numeric_limits<f64>::infinity();
      return r;
    }
    if (word(i, "nan")) {
      i += 3;
      u64 bits = single ? u64{0x7fc00000} : u64{0x7ff8000000000000};
      if (at(i) == '(') {
        std::size_t j = i + 1;
        while (digit(at(j)) || (lower(at(j)) >= 'a' && lower(at(j)) <= 'z') || at(j) == '_') ++j;
        if (at(j) == ')') {
          const std::string_view inside = s.substr(i + 1, j - i - 1);
          const Whole w = whole(inside, 0, false, false);
          if (w.end == inside.size()) bits |= w.bits & (single ? u64{0x3fffff} : u64{0x7ffffffffffff});
          i = j + 1;
        }
      }
      r.end = i;
      r.value = single ? static_cast<f64>(std::bit_cast<f32>(static_cast<u32>(bits))) : std::bit_cast<f64>(bits);
      if (minus) r.value = -r.value;
      return r;
    }
    return r;  // (no number: nothing read)
  }
  const bool hex = at(i) == '0' && lower(at(i + 1)) == 'x';
  if (hex) i += 2;
  const std::size_t digits = i;
  while (at(i) == '0') ++i;
  const int c = at(i);
  const bool more = digit(c) || (hex && xdigit(c)) || (c == '.' && (!hex || i != digits || xdigit(at(i + 1)))) || (hex && i != digits && lower(c) == 'p') ||
                    (!hex && lower(c) == 'e');
  if (!more) {
    // (only zeros - or a 0x with nothing after it: the 0 is the number)
    r.end = i != digits ? i : hex ? i - 1 : 0;
    r.value = minus ? -0.0 : 0.0;
    return r;
  }
  auto is_digit = [&](int d) { return digit(d) || (hex && xdigit(d)); };
  while (is_digit(at(i))) ++i;
  if (at(i) == '.') {
    ++i;
    while (is_digit(at(i))) ++i;
  }
  if (lower(at(i)) == (hex ? 'p' : 'e')) {
    std::size_t j = i + 1;
    if (at(j) == '-' || at(j) == '+') ++j;
    if (digit(at(j))) {
      while (digit(at(j))) ++j;
      i = j;
    }
  }
  r.end = i;
  r.value = finite(s.substr(subject, i - subject), single);
  return r;
}

/// Scan Text: what sscanf reads with a format - glibc's own scanner, step for step. The format's
/// conversions: %d %i %u %o %x %X %b (whole numbers: hh h, none - 8, 16, 32 bit - or l ll j z t q L
/// - 64 bit), %f %F %e %E %g %G %a %A (none: a float, l: a double), %s, %[set] (a word), %n (how
/// many characters were read), %% and * (read, not given); a width; spaces match any spaces, other
/// characters themselves. Each conversion that gives a value has a slot: what was there before
/// stays where the scan does not reach it (as the variable sscanf did not write keeps its value).
enum class Kind : u8 { whole, whole64, number, text };
struct Slot {
  Kind kind = Kind::whole;
  i64 whole = 0;  // whole: the number its C type holds (read as an Int)
  u64 bits = 0;   // whole64: the 64 bits
  f64 number = 0;
  std::string text;
  bool written = false;  // (the scan gave it a value)
};
inline constexpr std::size_t kMostSlots = 16;
/// One conversion of a format, read at fmt[at] (just after its %).
struct Spec {
  bool suppress = false;
  int width = -1;   // (-1: none)
  int size = 0;     // 'H' hh, 'h', 0 none, 'l', 'q' ll (and L), 'j', 'z', 't'
  char conv = 0;
  bool not_in = false;
  bool set[256] = {};
};
constexpr bool read_spec(std::string_view fmt, std::size_t& at, Spec& sp, const char** why) {
  auto fail = [&](const char* w) {
    if (why) *why = w;
    return false;
  };
  if (at < fmt.size() && fmt[at] == '*') sp.suppress = true, ++at;
  if (at < fmt.size() && (fmt[at] == '\'' || fmt[at] == 'I' || fmt[at] == '*')) return fail("a flag that is not supported (only * - read, not given)");
  int width = 0;
  bool has_width = false;
  while (at < fmt.size() && digit(fmt[at])) {
    width = width * 10 + (fmt[at++] - '0');
    has_width = true;
    if (width > 1 << 20) return fail("a width of more than 2^20");
  }
  if (at < fmt.size() && fmt[at] == '$') return fail("a value by its number ($) is not supported");
  sp.width = has_width && width > 0 ? width : -1;
  if (at < fmt.size()) {
    const char m = fmt[at];
    if (m == 'h' || m == 'l') {
      sp.size = m;
      ++at;
      if (at < fmt.size() && fmt[at] == m) sp.size = m == 'h' ? 'H' : 'q', ++at;
    } else if (m == 'q' || m == 'L') sp.size = m == 'L' ? 'L' : 'q', ++at;
    else if (m == 'j' || m == 'z' || m == 't') sp.size = m, ++at;
    else if (m == 'm') return fail("a word sscanf makes room for itself (%m) is not supported");
  }
  if (at >= fmt.size()) return fail("the format ends inside a conversion (a % without its letter)");
  sp.conv = fmt[at++];
  switch (sp.conv) {
    case 'd': case 'i': case 'u': case 'o': case 'x': case 'X': case 'b': case 'n':
      if (sp.size == 'L') sp.size = 'q';  // (glibc: L of a whole number is ll)
      return true;
    case 'e': case 'E': case 'f': case 'F': case 'g': case 'G': case 'a': case 'A':
      if (sp.size == 'L' || sp.size == 'q') return fail("a long double (%Lf) is not supported: a Float is a double");
      if (sp.size != 0 && sp.size != 'l') return fail("a number of a size that is no float or double");
      return true;
    case 's':
      if (sp.size != 0) return fail("a word of wide characters (%ls) is not supported");
      return true;
    case '%':
      return true;
    case '[': {
      if (sp.size != 0) return fail("a word of wide characters (%l[) is not supported");
      if (at < fmt.size() && fmt[at] == '^') sp.not_in = true, ++at;
      if (at < fmt.size() && (fmt[at] == ']' || fmt[at] == '-')) sp.set[static_cast<unsigned char>(fmt[at])] = true, ++at;
      char fc = 0;
      bool closed = false;
      while (at < fmt.size()) {
        fc = fmt[at++];
        if (fc == ']') {
          closed = true;
          break;
        }
        if (fc == '-' && at < fmt.size() && fmt[at] != ']' && static_cast<unsigned char>(fmt[at - 2]) <= static_cast<unsigned char>(fmt[at])) {
          for (int k = static_cast<unsigned char>(fmt[at - 2]); k < static_cast<unsigned char>(fmt[at]); ++k) sp.set[k] = true;
        } else {
          sp.set[static_cast<unsigned char>(fc)] = true;
        }
      }
      if (!closed) return fail("a set of characters without its ] (%[...])");
      return true;
    }
    case 'c': return fail("%c is not supported (its characters have no end): read a word with %s or %[...]");
    default: return fail("a conversion that is not supported (d i u o x X b f F e E g G a A s [ n %)");
  }
}
/// The kind of what a conversion gives (it gives none: suppressed, or %%).
constexpr bool gives(const Spec& sp) { return !sp.suppress && sp.conv != '%'; }
constexpr Kind kind_of(const Spec& sp) {
  switch (sp.conv) {
    case 'e': case 'E': case 'f': case 'F': case 'g': case 'G': case 'a': case 'A': return Kind::number;
    case 's': case '[': return Kind::text;
    case 'n': return sp.size == 'l' || sp.size == 'q' || sp.size == 'j' || sp.size == 'z' || sp.size == 't' ? Kind::whole64 : Kind::whole;
    default: return sp.size == 'l' || sp.size == 'q' || sp.size == 'j' || sp.size == 'z' || sp.size == 't' ? Kind::whole64 : Kind::whole;
  }
}
/// The kinds of the values a format gives, in their order. False: the format is none - `why` says.
struct Plan {
  std::size_t count = 0;
  Kind kinds[kMostSlots] = {};
};
constexpr bool plan(std::string_view fmt, Plan* out, const char** why) {
  Plan p;
  for (std::size_t at = 0; at < fmt.size();) {
    if (fmt[at++] != '%') continue;
    Spec sp;
    if (!read_spec(fmt, at, sp, why)) return false;
    if (!gives(sp)) continue;
    if (p.count == kMostSlots) {
      if (why) *why = "more than 16 values";
      return false;
    }
    p.kinds[p.count++] = kind_of(sp);
  }
  if (out) *out = p;
  return true;
}
/// The value a whole number is stored as by a conversion of this size: C's cast of the long sscanf read.
inline void store_whole(Slot& slot, const Spec& sp, u64 v, bool is_signed) {
  slot.written = true;
  if (kind_of(sp) == Kind::whole64) {
    slot.bits = v;
    return;
  }
  if (sp.size == 'H') slot.whole = is_signed ? static_cast<i64>(static_cast<i8>(v)) : static_cast<i64>(static_cast<u8>(v));
  else if (sp.size == 'h') slot.whole = is_signed ? static_cast<i64>(static_cast<i16>(v)) : static_cast<i64>(static_cast<u16>(v));
  else slot.whole = is_signed ? static_cast<i64>(static_cast<i32>(static_cast<u32>(v))) : static_cast<i64>(static_cast<u32>(v));
}
/// The characters of a number as the scanner gathers them: here, in the call's own memory - a number of
/// more than 255 characters in a std::string.
struct Gathered {
  char held[256] = {};
  std::size_t n = 0;
  std::string more;
  void clear() { n = 0, more.clear(); }
  void push_back(char c) {
    if (n < sizeof held && more.empty()) held[n++] = c;
    else {
      if (more.empty()) more.assign(held, n);
      more.push_back(c);
    }
  }
  std::size_t size() const { return more.empty() ? n : more.size(); }
  bool empty() const { return size() == 0; }
  char back() const { return more.empty() ? held[n - 1] : more.back(); }
  char operator[](std::size_t k) const { return more.empty() ? held[k] : more[k]; }
  operator std::string_view() const { return more.empty() ? std::string_view(held, n) : std::string_view(more); }  // NOLINT
};
/// sscanf(input, fmt, ...): the count it gives (-1: EOF - the input ended before the first value was
/// read), the slots of the values it read written. `slots`: one for each value the format gives
/// (plan), holding what was there before.
inline i64 run(std::string_view in, std::string_view fmt, Slot* slots, std::size_t count) {
  std::size_t pos = 0;
  auto inchar = [&]() -> int { return pos < in.size() ? static_cast<unsigned char>(in[pos++]) : kEnd; };
  auto unget = [&](int c) {
    if (c != kEnd) --pos;
  };
  i64 done = 0;
  std::size_t slot = 0;
  bool skip_space = false;
  bool input_error = false;
  Gathered buf;  // (the characters of a number, as glibc gathers them)
  for (std::size_t f = 0; f < fmt.size();) {
    const char fc = fmt[f++];
    int c = 0;
    if (fc != '%') {
      if (space(static_cast<unsigned char>(fc))) {
        skip_space = true;
        continue;
      }
      c = inchar();
      if (c == kEnd) {
        input_error = true;
        break;
      }
      if (skip_space) {
        while (space(c)) {
          c = inchar();
          if (c == kEnd) break;
        }
        if (c == kEnd) {
          input_error = true;
          break;
        }
        skip_space = false;
      }
      if (c != static_cast<unsigned char>(fc)) {
        unget(c);
        break;
      }
      continue;
    }
    Spec sp;
    if (!read_spec(fmt, f, sp, nullptr)) break;  // (a format that is none: what came before it - the check reports it)
    if (skip_space || (sp.conv != '[' && sp.conv != 'n')) {
      do c = inchar();
      while (space(c));
      unget(c);
      skip_space = false;
    }
    Slot* out = gives(sp) && slot < count ? &slots[slot] : nullptr;
    if (gives(sp)) ++slot;
    int width = sp.width;
    bool failed = false;
    switch (sp.conv) {
      case '%':
        c = inchar();
        if (c == kEnd) input_error = true;
        else if (c != '%') unget(c), failed = true;
        break;
      case 'n':
        if (out) store_whole(*out, sp, static_cast<u64>(pos), true);
        break;
      case 's': {
        c = inchar();
        if (c == kEnd) {
          input_error = true;
          break;
        }
        std::string word;
        do {
          if (space(c)) {
            unget(c);
            break;
          }
          word.push_back(static_cast<char>(c));
        } while ((width <= 0 || --width > 0) && (c = inchar()) != kEnd);
        if (out) out->text = std::move(word), out->written = true, ++done;
        break;
      }
      case '[': {
        if (width < 0) width = std::numeric_limits<int>::max();
        c = inchar();
        if (c == kEnd) {
          input_error = true;
          break;
        }
        std::string word;
        do {
          if (sp.set[c] == sp.not_in) {
            unget(c);
            break;
          }
          word.push_back(static_cast<char>(c));
        } while (--width > 0 && (c = inchar()) != kEnd);
        if (word.empty()) {
          failed = true;
          break;
        }
        if (out) out->text = std::move(word), out->written = true, ++done;
        break;
      }
      case 'd': case 'i': case 'u': case 'o': case 'x': case 'X': case 'b': {
        int base = sp.conv == 'd' || sp.conv == 'u' ? 10 : sp.conv == 'i' ? 0 : sp.conv == 'o' ? 8 : sp.conv == 'b' ? 2 : 16;
        const bool is_signed = sp.conv == 'd' || sp.conv == 'i';
        buf.clear();
        c = inchar();
        if (c == kEnd) {
          input_error = true;
          break;
        }
        if (c == '-' || c == '+') {
          buf.push_back(static_cast<char>(c));
          if (width > 0) --width;
          c = inchar();
        }
        if (width != 0 && c == '0') {
          if (width > 0) --width;
          buf.push_back('0');
          c = inchar();
          if (width != 0 && lower(c) == 'x') {
            if (base == 0) base = 16;
            if (base == 16) {
              if (width > 0) --width;
              c = inchar();
            }
          } else if (width != 0 && lower(c) == 'b') {
            if (base == 0) base = 2;
            if (base == 2) {
              if (width > 0) --width;
              c = inchar();
            }
          } else if (base == 0) {
            base = 8;
          }
        }
        if (base == 0) base = 10;
        while (c != kEnd && width != 0) {
          if (base == 16 ? !xdigit(c) : !digit(c) || c - '0' >= base) break;
          buf.push_back(static_cast<char>(c));
          if (width > 0) --width;
          c = inchar();
        }
        unget(c);
        if (buf.empty() || (buf.size() == 1 && (buf[0] == '+' || buf[0] == '-'))) {
          failed = true;
          break;
        }
        const Whole w = whole(std::string_view(buf), base, is_signed);
        if (w.end == 0) {
          failed = true;
          break;
        }
        if (out) store_whole(*out, sp, w.bits, is_signed), ++done;
        break;
      }
      case 'e': case 'E': case 'f': case 'F': case 'g': case 'G': case 'a': case 'A': {
        buf.clear();
        c = inchar();
        if (width > 0) --width;
        if (c == kEnd) {
          input_error = true;
          break;
        }
        bool got_digit = false, got_dot = false, got_e = false, got_sign = false, hexa = false;
        // (read the next character into c; false: the conversion fails)
        auto next = [&](char want) {
          if (width == 0 || (c = inchar()) == kEnd || lower(c) != want) return false;
          if (width > 0) --width;
          buf.push_back(static_cast<char>(c));
          return true;
        };
        if (c == '-' || c == '+') {
          got_sign = true;
          buf.push_back(static_cast<char>(c));
          if (width == 0 || (c = inchar()) == kEnd) {
            failed = true;
            break;
          }
          if (width > 0) --width;
        }
        bool special = false;
        if (lower(c) == 'n') {
          buf.push_back(static_cast<char>(c));
          if (!next('a') || !next('n')) {
            failed = true;
            break;
          }
          special = true;
        } else if (lower(c) == 'i') {
          buf.push_back(static_cast<char>(c));
          if (!next('n') || !next('f')) {
            failed = true;
            break;
          }
          if (width != 0 && (c = inchar()) != kEnd) {
            if (lower(c) == 'i') {
              if (width > 0) --width;
              buf.push_back(static_cast<char>(c));
              if (!next('n') || !next('i') || !next('t') || !next('y')) {
                failed = true;
                break;
              }
            } else {
              unget(c);
            }
          }
          special = true;
        }
        if (!special) {
          char exp_char = 'e';
          if (width != 0 && c == '0') {
            buf.push_back('0');
            c = inchar();
            if (width > 0) --width;
            if (width != 0 && lower(c) == 'x') {
              buf.push_back(static_cast<char>(c));
              hexa = true;
              exp_char = 'p';
              c = inchar();
              if (width > 0) --width;
            } else {
              got_digit = true;
            }
          }
          while (true) {
            if (digit(c)) {
              buf.push_back(static_cast<char>(c));
              got_digit = true;
            } else if (!got_e && hexa && xdigit(c)) {
              buf.push_back(static_cast<char>(c));
              got_digit = true;
            } else if (got_e && !buf.empty() && buf.back() == exp_char && (c == '-' || c == '+')) {
              buf.push_back(static_cast<char>(c));
            } else if (got_digit && !got_e && lower(c) == exp_char) {
              buf.push_back(exp_char);
              got_e = got_dot = true;
            } else if (!got_dot && c == '.') {
              buf.push_back('.');
              got_dot = true;
            } else {
              unget(c);
              break;
            }
            if (width == 0 || (c = inchar()) == kEnd) break;
            if (width > 0) --width;
          }
          if (buf.size() == (got_sign ? 1u : 0u) || (hexa && buf.size() == (got_sign ? 3u : 2u))) {
            failed = true;
            break;
          }
        }
        const bool single = sp.size != 'l';
        const Real got = real(std::string_view(buf), single);
        if (got.end == 0) {
          failed = true;
          break;
        }
        if (out) out->number = got.value, out->written = true, ++done;
        break;
      }
      default: failed = true; break;
    }
    if (input_error || failed) break;
  }
  if (input_error && done == 0) return -1;
  return done;
}
}  // namespace scan

namespace enums {
/// `index` when it is the place of one of `count` values, else 0 (the first value).
constexpr i64 valid(std::size_t count, i64 index) { return index >= 0 && static_cast<u64>(index) < count ? index : 0; }
constexpr bool is_value(std::size_t count, i64 index) { return index >= 0 && static_cast<u64>(index) < count; }
/// The name of the value at `index` ("" when there is none).
inline std::string name(const std::vector<std::string>& values, i64 index) { return is_value(values.size(), index) ? values[static_cast<std::size_t>(index)] : std::string(); }
/// The place of the value called `name`, or -1.
inline i64 find(const std::vector<std::string>& values, std::string_view name) {
  for (std::size_t k = 0; k < values.size(); ++k)
    if (values[k] == name) return static_cast<i64>(k);
  return -1;
}
/// The names of an enum as KERN_ENUM has them: "Closed, Opening, Open".
inline std::vector<std::string> names(std::string_view list) {
  std::vector<std::string> r;
  std::size_t at = 0;
  while (at < list.size()) {
    std::size_t end = list.find(',', at);
    if (end == std::string_view::npos) end = list.size();
    std::size_t a = at, b = end;
    while (a < b && (list[a] == ' ' || list[a] == '\n' || list[a] == '\t')) ++a;
    while (b > a && (list[b - 1] == ' ' || list[b - 1] == '\n' || list[b - 1] == '\t')) --b;
    if (b > a) r.emplace_back(list.substr(a, b - a));
    at = end + 1;
  }
  return r;
}
}  // namespace enums

/// What the struct nodes do. A struct's value holds a value for each of its fields, in the order
/// its declaration has them. In the interpreter that is a Value (its rows); in a Blueprint's C++
/// it is the aggregate KERN_STRUCT declares - Make is its initializer, Break its members, Set
/// Field the assignment to one of them.
namespace structs {
/// Make: the struct with these fields.
inline Value make(std::vector<Value> fields) { return Value::structure(std::move(fields)); }
/// The field at `index` of the struct `s` becomes `value` (a field it does not hold: nothing -
/// logic::hold_fields makes a value hold them all).
inline void set_field(Value& s, std::size_t index, Value value) {
  if (index < s.fields()) s.own_rows()[index] = std::move(value);
}

/// Stands for a value of any type where a field of an aggregate is initialized (never called).
struct AnyField {
  template <class T> operator T() const;  // NOLINT (implicit: that is the point)
};
template <class S, std::size_t... I> constexpr bool takes_fields(std::index_sequence<I...>) {
  return requires { S{(static_cast<void>(I), AnyField{})...}; };
}
/// How many fields the aggregate S has (a struct of a Blueprint: StructType::kMaxFields at most).
template <class S, std::size_t N = StructType::kMaxFields> constexpr std::size_t field_count() {
  if constexpr (N == 0) return 0;
  else if constexpr (takes_fields<S>(std::make_index_sequence<N>{})) return N;
  else return field_count<S, N - 1>();
}
/// Calls f(fields...) with the fields of the aggregate `s`, in their order.
template <class S, class F> constexpr decltype(auto) fields(S& s, F&& f) {
  constexpr std::size_t N = field_count<std::remove_const_t<S>>();
  if constexpr (N == 1) { auto& [f0] = s; return f(f0); }
  else if constexpr (N == 2) { auto& [f0, f1] = s; return f(f0, f1); }
  else if constexpr (N == 3) { auto& [f0, f1, f2] = s; return f(f0, f1, f2); }
  else if constexpr (N == 4) { auto& [f0, f1, f2, f3] = s; return f(f0, f1, f2, f3); }
  else if constexpr (N == 5) { auto& [f0, f1, f2, f3, f4] = s; return f(f0, f1, f2, f3, f4); }
  else if constexpr (N == 6) { auto& [f0, f1, f2, f3, f4, f5] = s; return f(f0, f1, f2, f3, f4, f5); }
  else if constexpr (N == 7) { auto& [f0, f1, f2, f3, f4, f5, f6] = s; return f(f0, f1, f2, f3, f4, f5, f6); }
  else if constexpr (N == 8) { auto& [f0, f1, f2, f3, f4, f5, f6, f7] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7); }
  else if constexpr (N == 9) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8); }
  else if constexpr (N == 10) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }
  else if constexpr (N == 11) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10); }
  else if constexpr (N == 12) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11); }
  else if constexpr (N == 13) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12); }
  else if constexpr (N == 14) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13); }
  else if constexpr (N == 15) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14); }
  else if constexpr (N == 16) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15); }
  else if constexpr (N == 17) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16); }
  else if constexpr (N == 18) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17); }
  else if constexpr (N == 19) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18); }
  else if constexpr (N == 20) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19); }
  else if constexpr (N == 21) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20); }
  else if constexpr (N == 22) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21); }
  else if constexpr (N == 23) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22); }
  else if constexpr (N == 24) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23); }
  else if constexpr (N == 25) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24); }
  else if constexpr (N == 26) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25); }
  else if constexpr (N == 27) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26); }
  else if constexpr (N == 28) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27); }
  else if constexpr (N == 29) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28); }
  else if constexpr (N == 30) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29); }
  else if constexpr (N == 31) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30); }
  else if constexpr (N == 32) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31); }
  else if constexpr (N == 33) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32); }
  else if constexpr (N == 34) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33); }
  else if constexpr (N == 35) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34); }
  else if constexpr (N == 36) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35); }
  else if constexpr (N == 37) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36); }
  else if constexpr (N == 38) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37); }
  else if constexpr (N == 39) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38); }
  else if constexpr (N == 40) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39); }
  else if constexpr (N == 41) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40); }
  else if constexpr (N == 42) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41); }
  else if constexpr (N == 43) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42); }
  else if constexpr (N == 44) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43); }
  else if constexpr (N == 45) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44); }
  else if constexpr (N == 46) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45); }
  else if constexpr (N == 47) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46); }
  else if constexpr (N == 48) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47); }
  else if constexpr (N == 49) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48); }
  else if constexpr (N == 50) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49); }
  else if constexpr (N == 51) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50); }
  else if constexpr (N == 52) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51); }
  else if constexpr (N == 53) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52); }
  else if constexpr (N == 54) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53); }
  else if constexpr (N == 55) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54); }
  else if constexpr (N == 56) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55); }
  else if constexpr (N == 57) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56); }
  else if constexpr (N == 58) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57); }
  else if constexpr (N == 59) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58); }
  else if constexpr (N == 60) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59); }
  else if constexpr (N == 61) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60); }
  else if constexpr (N == 62) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61); }
  else if constexpr (N == 63) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62); }
  else if constexpr (N == 64) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63); }
  else if constexpr (N == 65) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64); }
  else if constexpr (N == 66) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65); }
  else if constexpr (N == 67) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66); }
  else if constexpr (N == 68) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67); }
  else if constexpr (N == 69) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68); }
  else if constexpr (N == 70) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69); }
  else if constexpr (N == 71) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70); }
  else if constexpr (N == 72) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71); }
  else if constexpr (N == 73) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72); }
  else if constexpr (N == 74) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73); }
  else if constexpr (N == 75) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74); }
  else if constexpr (N == 76) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75); }
  else if constexpr (N == 77) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76); }
  else if constexpr (N == 78) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77); }
  else if constexpr (N == 79) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78); }
  else if constexpr (N == 80) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79); }
  else if constexpr (N == 81) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80); }
  else if constexpr (N == 82) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81); }
  else if constexpr (N == 83) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82); }
  else if constexpr (N == 84) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83); }
  else if constexpr (N == 85) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84); }
  else if constexpr (N == 86) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85); }
  else if constexpr (N == 87) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86); }
  else if constexpr (N == 88) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87); }
  else if constexpr (N == 89) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88); }
  else if constexpr (N == 90) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89); }
  else if constexpr (N == 91) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90); }
  else if constexpr (N == 92) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91); }
  else if constexpr (N == 93) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92); }
  else if constexpr (N == 94) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92, f93] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92, f93); }
  else if constexpr (N == 95) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92, f93, f94] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92, f93, f94); }
  else if constexpr (N == 96) { auto& [f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92, f93, f94, f95] = s; return f(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31, f32, f33, f34, f35, f36, f37, f38, f39, f40, f41, f42, f43, f44, f45, f46, f47, f48, f49, f50, f51, f52, f53, f54, f55, f56, f57, f58, f59, f60, f61, f62, f63, f64, f65, f66, f67, f68, f69, f70, f71, f72, f73, f74, f75, f76, f77, f78, f79, f80, f81, f82, f83, f84, f85, f86, f87, f88, f89, f90, f91, f92, f93, f94, f95); }
  else return f();
}
/// Are two structs equal: every field of one equal to that field of the other (the operator ==
/// of a KERN_STRUCT).
template <class S> constexpr bool equal(const S& a, const S& b) {
  return fields(a, [&b](const auto&... x) { return fields(b, [&x...](const auto&... y) { return (static_cast<bool>(x == y) && ...); }); });
}
}  // namespace structs

/// The bytes of a value (Bytes of Struct, Struct of Bytes): the object representation of a struct
/// of numbers in the layout C++ gives it - what std::bit_cast and std::memcpy between a struct and
/// bytes see. The layout is the node's setting, "20: u32@0 u16@4 i16@6 ...": the size in bytes,
/// then for each field of the struct, in their order, the number C++ holds it as and where it
/// begins. The numbers: bool, u8, i8, u16, i16, u32, i32 (a Bool's or an Int's field; an Int is
/// cut to that many bits, as C++ converts), u64, i64 (an Int64's field), f32, f64 (a Float's). The
/// bytes are little-endian, on every platform the same; a byte no field covers is 0. A number of
/// the bytes is taken as C++ takes it to an unsigned char (its last 8 bits); where there are no
/// bytes - before the list, beyond it - they are 0. A NaN is the one quiet NaN of its type (numbers.h,
/// seen: 0x7fc00000, 0x7ff8000000000000 - on every platform and in every tier the same).
namespace bytes {
enum class Code : u8 { none, boolean, u8, i8, u16, i16, u32, i32, u64, i64, f32, f64 };
struct Field {
  Code code = Code::none;
  u32 at = 0;
};
/// A layout read: the size, the fields - and, when it cannot be read, why (`error`, else nullptr).
struct Layout {
  std::array<Field, StructType::kMaxFields> fields{};
  std::size_t count = 0;
  u32 size = 0;
  const char* error = nullptr;
};
inline constexpr u32 kMaxBytes = 65536;
constexpr u32 width(Code c) {
  switch (c) {
    case Code::boolean: case Code::u8: case Code::i8: return 1;
    case Code::u16: case Code::i16: return 2;
    case Code::u32: case Code::i32: case Code::f32: return 4;
    case Code::u64: case Code::i64: case Code::f64: return 8;
    default: return 0;
  }
}
constexpr bool floating(Code c) { return c == Code::f32 || c == Code::f64; }
constexpr bool is_signed(Code c) { return c == Code::i8 || c == Code::i16 || c == Code::i32 || c == Code::i64; }
constexpr Code code_of(std::string_view w) {
  constexpr std::pair<std::string_view, Code> kCodes[] = {{"bool", Code::boolean}, {"u8", Code::u8},   {"i8", Code::i8},   {"u16", Code::u16},
                                                          {"i16", Code::i16},      {"u32", Code::u32}, {"i32", Code::i32}, {"u64", Code::u64},
                                                          {"i64", Code::i64},      {"f32", Code::f32}, {"f64", Code::f64}};
  for (const auto& [name, code] : kCodes)
    if (name == w) return code;
  return Code::none;
}
/// The layout a setting says (constexpr: a Blueprint's class reads it when it is compiled).
constexpr Layout layout(std::string_view s) {
  Layout r;
  std::size_t i = 0;
  auto blank = [&] { while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) ++i; };
  auto number = [&](u32& out) {
    const std::size_t from = i;
    u64 v = 0;
    while (i < s.size() && s[i] >= '0' && s[i] <= '9' && v <= kMaxBytes) v = v * 10 + static_cast<u64>(s[i++] - '0');
    out = static_cast<u32>(v);
    return i > from && v <= kMaxBytes;
  };
  blank();
  if (!number(r.size) || r.size == 0) { r.error = "the layout begins with its size in bytes (1 to 65536) and a colon: \"20: u32@0 ...\""; return r; }
  blank();
  if (i >= s.size() || s[i] != ':') { r.error = "a colon after the size: \"20: u32@0 ...\""; return r; }
  ++i;
  for (blank(); i < s.size(); blank()) {
    const std::size_t from = i;
    while (i < s.size() && s[i] != '@' && s[i] != ' ' && s[i] != '\t' && s[i] != '\n' && s[i] != '\r') ++i;
    const Code c = code_of(s.substr(from, i - from));
    if (c == Code::none) { r.error = "a field is one of bool u8 i8 u16 i16 u32 i32 u64 i64 f32 f64"; return r; }
    if (i >= s.size() || s[i] != '@') { r.error = "a field says where it begins: u16@4"; return r; }
    ++i;
    Field f{c, 0};
    if (!number(f.at)) { r.error = "where a field begins is a number of bytes: u16@4"; return r; }
    if (f.at + width(c) > r.size) { r.error = "a field reaches beyond the size"; return r; }
    for (std::size_t k = 0; k < r.count; ++k)
      if (f.at < r.fields[k].at + width(r.fields[k].code) && r.fields[k].at < f.at + width(c)) { r.error = "two fields share a byte"; return r; }
    if (r.count == r.fields.size()) { r.error = "more fields than a struct has"; return r; }
    r.fields[r.count++] = f;
  }
  if (r.count == 0) r.error = "a layout has a field at least";
  return r;
}
/// The field's bits into the bytes, little-endian.
template <class B> void put(B* out, const Field& f, u64 v) {
  for (u32 k = 0; k < width(f.code); ++k) out[f.at + k] = B(static_cast<i64>((v >> (8 * k)) & 0xffu));
}
/// The field's bits from the bytes (`byte(i)`: the i-th byte, 0 .. 255), sign-extended where it is signed.
template <class Byte> u64 take(const Field& f, Byte&& byte) {
  u64 v = 0;
  const u32 n = width(f.code);
  for (u32 k = 0; k < n; ++k) v |= static_cast<u64>(byte(f.at + k)) << (8 * k);
  if (is_signed(f.code) && n < 8 && (v >> (8 * n - 1)) & 1u) v |= ~u64{0} << (8 * n);
  return v;
}
/// The number type a code is (Code::none: exec, none).
constexpr PinType type_of(Code c) {
  switch (c) {
    case Code::boolean: return PinType::boolean;
    case Code::u8: return PinType::u8;
    case Code::i8: return PinType::i8;
    case Code::u16: return PinType::u16;
    case Code::i16: return PinType::i16;
    case Code::u32: return PinType::u32;
    case Code::i32: return PinType::i32;
    case Code::u64: return PinType::u64;
    case Code::i64: return PinType::i64;
    case Code::f32: return PinType::f32;
    case Code::f64: return PinType::f64;
    default: return PinType::exec;
  }
}
/// Does a field of a struct of the type t suit the code (a layout's field is the number type of its field itself)?
constexpr bool suits(Code c, PinType t) { return c != Code::none && type_of(c) == t; }
/// The byte of a number of a list of bytes: its last 8 bits.
inline u64 byte_of(i64 v) { return static_cast<u64>(v) & 0xffu; }
/// The i-th byte from `from` on of `count` entries (before them, beyond them: 0).
template <class E> u64 byte_at(const E* items, std::size_t count, i64 from, u32 i) {
  if (from >= 0 && static_cast<u64>(from) >= count) return 0;  // (and no sum beyond the end of an i64)
  const i64 k = from + static_cast<i64>(i);
  return k >= 0 && static_cast<u64>(k) < count ? byte_of(static_cast<i64>(num::raw_value(items[static_cast<std::size_t>(k)]))) : 0;
}

// ---- the bytes of a list of numbers (Copy Bytes) ----
/// Is a list of this type one of entries of the number `c` (Copy Bytes: the list's own type)?
constexpr bool suits_list(Code c, PinType element) { return suits(c, element) && c != Code::boolean; }
/// The bits `w` holds of a number `c` is wide - the rest 0, or 1 where `c` has a sign and its
/// highest bit is one (as C++ reads a number of that width).
constexpr u64 widened(Code c, u64 w) {
  const u32 n = width(c);
  if (n >= 8) return w;
  w &= (u64{1} << (8 * n)) - 1;
  if (is_signed(c) && ((w >> (8 * n - 1)) & 1u)) w |= ~u64{0} << (8 * n);
  return w;
}
/// The bits of an entry as the number `c` holds it (num::bits_of).
template <num::Number T> u64 entry_bits(Code, T v) { return num::seen_bits(v); }  // (a NaN: the one NaN - numbers.h, seen)
/// The entry those bits of the number `c` are.
template <num::Number T> void set_entry(T& to, Code c, u64 w) { to = num::of_bits<T>(widened(c, w)); }
template <num::Number T> u64 entry_bits(Code c, num::Typed<T> v) { return entry_bits(c, v.v); }
template <num::Number T> void set_entry(num::Typed<T>& to, Code c, u64 w) { set_entry(to.v, c, w); }
/// Where the numbers of a list may lie for Copy Bytes: an entry or a byte beyond every list (a list
/// has at most 2^24 entries, of at most 8 bytes) is the same as one further out - what is beyond
/// that is cut to it, so that no sum leaves an i64.
inline constexpr i64 kFarBytes = i64{1} << 40;
constexpr i64 near(i64 v) { return v < -kFarBytes ? -kFarBytes : v > kFarBytes ? kFarBytes : v; }
/// Copy Bytes: `count` bytes of the entries of `from` (`from_count` of them, each the number `of`,
/// little-endian), from its entry `first` on, into the entries of `to` (each the number `into`) from
/// its entry `at` on - what std::memcpy / std::memmove between lists of numbers does. A byte the
/// entries of `from` do not have is 0; a byte beyond `to` is not written - it keeps its length; an
/// entry some of whose bytes are written keeps the others (of its bits as `into` holds it). The
/// entries of `from` are read as they were before (a list into itself, as memmove). One pass over
/// the entries written; nothing is asked for - except where a list is copied into itself as another
/// number than it is (then its entries as they were are taken first).
template <class D, class S> void copy(D* to, std::size_t to_count, Code into, i64 at, const S* from, std::size_t from_count, Code of, i64 first, i64 count) {
  const i64 w = width(into), v = width(of);
  if (w == 0 || v == 0 || count <= 0 || to_count == 0) return;
  count = near(count);
  const i64 d0 = near(at) * w, s0 = near(first) * v;  // (where the bytes begin in each)
  const i64 size = static_cast<i64>(to_count) * w;
  const i64 lo = d0 < 0 ? 0 : d0, hi = d0 + count > size ? size : d0 + count;  // (the bytes of `to` written: lo .. hi - 1)
  if (lo >= hi) return;
  bool same = false;  // (is `from` the list `to` itself?)
  if constexpr (std::is_same_v<D, S>) {
    same = !std::less<const S*>{}(from, to) && std::less<const S*>{}(from, to + to_count);
    if (same && into != of) {
      const std::vector<S> were(from, from + from_count);
      copy(to, to_count, into, at, were.data(), were.size(), of, first, count);
      return;
    }
  }
  // the byte of `from` at s (its entry read once for all its bytes a pass needs)
  i64 held_at = -1;
  u64 held = 0;
  auto source = [&](i64 s) -> u64 {
    const i64 j = s >= 0 ? s / v : -((-s + v - 1) / v);
    if (j < 0 || static_cast<u64>(j) >= from_count) return 0;
    if (j != held_at) held = entry_bits(of, from[static_cast<std::size_t>(j)]), held_at = j;
    return (held >> (8 * (s - j * v))) & 0xffu;
  };
  // (from the last entry back where the bytes move up in a list into itself - each read before it is written)
  const bool down = d0 > s0;
  const i64 e0 = lo / w, e1 = (hi - 1) / w;
  for (i64 k = 0; k <= e1 - e0; ++k) {
    const i64 e = down ? e1 - k : e0 + k;
    D& entry = to[static_cast<std::size_t>(e)];
    u64 bits_ = entry_bits(into, entry);
    for (i64 b = 0; b < w; ++b) {
      const i64 at_byte = e * w + b;
      if (at_byte < lo || at_byte >= hi) continue;
      bits_ = (bits_ & ~(u64{0xff} << (8 * b))) | (source(s0 + (at_byte - d0)) << (8 * b));
    }
    set_entry(entry, into, bits_);
    if (same) held_at = -1;  // (the entry written may be one read next)
  }
}
}  // namespace bytes

/// A whole number written as text, as settings hold them ("3", " 12"): leading blanks, a sign,
/// digits; anything else ends it. Empty or no digits: `fallback`.
constexpr int setting_int(std::string_view s, int fallback) {
  std::size_t i = 0;
  while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r' || s[i] == '\f' || s[i] == '\v')) ++i;
  bool minus = false;
  if (i < s.size() && (s[i] == '+' || s[i] == '-')) minus = s[i++] == '-';
  if (s.empty()) return fallback;
  i64 v = 0;
  for (; i < s.size() && s[i] >= '0' && s[i] <= '9'; ++i) {
    v = v * 10 + (s[i] - '0');
    if (v > 2000000000) { v = 2000000000; break; }
  }
  return static_cast<int>(minus ? -v : v);
}
/// A count setting (Sequence outputs, Switch cases): `fallback` when empty, kept within [lo, hi].
constexpr int setting_count(std::string_view s, int fallback, int lo, int hi) { return kern::clamp(s.empty() ? fallback : setting_int(s, fallback), lo, hi); }
/// The cases of Switch on Text: separated by commas, blanks around them dropped, empty ones left out.
inline std::vector<std::string> setting_cases(std::string_view s) {
  std::vector<std::string> out;
  std::string cur;
  auto flush = [&] {
    while (!cur.empty() && cur.front() == ' ') cur.erase(cur.begin());
    while (!cur.empty() && cur.back() == ' ') cur.pop_back();
    if (!cur.empty()) out.push_back(cur);
    cur.clear();
  };
  for (char ch : s) {
    if (ch == ',') flush();
    else cur += ch;
  }
  flush();
  return out;
}

/// What the engine passes by position to a function of a Blueprint's class, as the C++ type of
/// that input (the conversion of Instance::call); a missing one is the input's default.
namespace detail {
template <class C, class R, class... A> std::tuple<std::remove_cvref_t<A>...> params_of(R (C::*)(A...));
}  // namespace detail
/// The type a function of a Blueprint's class takes as its K-th input, without const and &:
/// NativeParam<&Door::Tune, 0>. What holds a reference input for a call by name - a variable of
/// exactly the class's type, so the function's reference binds to it (logic::native_glue).
template <auto Function, std::size_t K> using NativeParam = std::tuple_element_t<K, decltype(detail::params_of(Function))>;
template <class T> T native_arg(std::span<const Value> args, std::size_t i, const T& fallback) {
  return i < args.size() ? from_value<T>(args[i].converted(pin_type_of<T>())) : fallback;
}
/// A value an event brought, as the C++ type of its handler's parameter: output `out` of the
/// event's node `node` (Runtime::fire puts the event's values there, for every tier).
template <class T> T native_out(const Instance& instance, int node, int out) { return from_value<T>(instance.output(node, out).converted(pin_type_of<T>())); }
/// Instance::set for a variable that is a member of a Blueprint's class: the conversions the
/// interpreter's variables make.
template <class T>
  requires num::Number<T>
void native_set(T& variable, const Value& v) {
  variable = v.as<T>();
}
template <num::Number T> void native_set(num::Typed<T>& variable, const Value& v) { variable = v.as<T>(); }
inline void native_set(Vec3& variable, const Value& v) { variable = v.as_vector(); }
inline void native_set(std::string& variable, const Value& v) { variable = v.as_text(); }
inline void native_set(Text& variable, const Value& v) { variable = Text(v.as_text()); }
inline void native_set(ActorHandle& variable, const Value& v) { variable = v.as_actor(); }
template <class I> void native_set(Object<I>& variable, const Value& v) { variable = Object<I>{v.as_object()}; }
template <class T> void native_set(std::vector<T>& variable, const Value& v) { variable = from_value<std::vector<T>>(v.converted(pin_type_of<std::vector<T>>())); }
template <class T, std::size_t N> void native_set(Fixed<T, N>& variable, const Value& v) { variable = from_value<Fixed<T, N>>(v.converted(pin_type_of<Fixed<T, N>>())); }
template <class E>
  requires std::is_enum_v<E>
void native_set(E& variable, const Value& v) {
  variable = from_value<E>(v.converted(PinType::enumeration));
}
template <detail::NativeStruct S> void native_set(S& variable, const Value& v) { variable = from_value<S>(v); }
template <class K, class V> void native_set(Map<K, V>& variable, const Value& v) { variable = from_value<Map<K, V>>(v.converted(pin_type_of<Map<K, V>>())); }
template <class K> void native_set(Set<K>& variable, const Value& v) { variable = from_value<Set<K>>(v.converted(pin_type_of<Set<K>>())); }
template <class R, class... A> void native_set(Callback<R(A...)>& variable, const Value& v) { variable = from_value<Callback<R(A...)>>(v); }

/// A Random node's own sequence. `m` is the node's memory ([0] where the sequence is, [1] it has
/// begun); it starts from the Blueprint's seed and the node's id. Two Random nodes never share a
/// sequence: in which order they are asked makes no difference to what each gives - in C++ the
/// order in which arguments are worked out is the compiler's.
namespace random {
inline u32 next(f64* m, u32 seed, u32 node) {
  u32 state = static_cast<u32>(m[0]);
  if (m[1] == 0) {
    m[1] = 1;
    state = seed ^ (node * 0x9e3779b9u);
  }
  state += 0x9e3779b9u;
  m[0] = static_cast<f64>(state);
  u32 z = state;
  z ^= z >> 16;
  z *= 0x85ebca6bu;
  z ^= z >> 13;
  z *= 0xc2b2ae35u;
  z ^= z >> 16;
  return z;
}
/// 0 <= x < 1
inline f64 unit(u32 r) { return static_cast<f64>(r >> 8) / static_cast<f64>(1u << 24); }
inline f64 number(f64* m, u32 seed, u32 node, f64 min, f64 max) { return min + (max - min) * unit(next(m, seed, node)); }
/// A whole number from min to max, both included (max below min: min).
inline i64 whole_number(f64* m, u32 seed, u32 node, i64 min, i64 max) {
  if (max < min) max = min;
  const u64 span = static_cast<u64>(max) - static_cast<u64>(min) + 1;  // (0: every Int there is)
  const u32 choices = span == 0 || span > 0x7fffffffull ? 0x7fffffffu : static_cast<u32>(span);
  return min + static_cast<i64>(next(m, seed, node) % choices);
}
inline bool chance(f64* m, u32 seed, u32 node, f64 chance) { return unit(next(m, seed, node)) < chance; }
}  // namespace random

/// The value nodes of the core library. Static, typed, without side effects: the interpreter
/// wraps each as a node, a Blueprint's C++ calls it by this name.
struct PureNodes {
  using Bool = bool;
  /// The defaults of the editor's "Int" and "Float" (logic::kInt, kFloat): i64, f32 - what the vectors are of.
  using Int = i64;
  using Float = f32;
  using Vector = Vec3;
  using Text = std::string;
  using Actor = ActorHandle;
  /// An object of the engine of the class I (kern/docs/engine_object.md): Object<kern::App>.
  template <class I> using Object = logic::Object<I>;

  // ---- numbers: every operation of every number type, as C++ computes it (kern/logic/numbers.h) ----
  // A node of these has the number type as its setting "Type" (math_add<f32>): its pins are of that type. Where C++
  // leaves the result open, numbers.h fixes one (there: why and which).
  template <class P, num::Arithmetic T = num::raw<P>> static T math_add(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::add(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_sub(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::sub(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_mul(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::mul(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_div(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::div(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_rem(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::rem(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_negate(std::type_identity_t<T> a) { return num::neg(a); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_abs(std::type_identity_t<T> a) { return num::abs(a); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_min(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::min(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static T math_max(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::max(a, b); }
  /// Value between Min and Max (the two in either order): max(min(value, hi), lo) of numbers.h.
  template <class P, num::Arithmetic T = num::raw<P>> static T math_clamp(std::type_identity_t<T> value, std::type_identity_t<T> min, std::type_identity_t<T> max) { return num::clamp(value, min, max); }
  /// A + (B - A) × Alpha, each step in T.
  template <class P, num::Floating T = num::raw<P>> static T math_lerp(std::type_identity_t<T> a, std::type_identity_t<T> b, std::type_identity_t<T> alpha) { return num::lerp(a, b, alpha); }
  // (comparisons: of one type, as C++ compares them - a NaN is unequal to everything)
  template <class P, num::Number T = num::raw<P>> static Bool math_equal(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::eq(a, b); }
  template <class P, num::Number T = num::raw<P>> static Bool math_not_equal(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::ne(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static Bool math_less(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::lt(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static Bool math_less_equal(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::le(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static Bool math_greater(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::gt(a, b); }
  template <class P, num::Arithmetic T = num::raw<P>> static Bool math_greater_equal(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::ge(a, b); }
  /// Compare with the operator of the node's setting (== != < <= > >=), as C++ compares.
  template <class P, num::Arithmetic T = num::raw<P>> static Bool compare(std::string_view op, std::type_identity_t<T> a, std::type_identity_t<T> b) {
    if (op == "==" || op == "=") return num::eq(a, b);
    if (op == "!=" || op == "≠") return num::ne(a, b);
    if (op == "<") return num::lt(a, b);
    if (op == "<=" || op == "≤") return num::le(a, b);
    if (op == ">") return num::gt(a, b);
    return num::ge(a, b);
  }
  // (bits of whole numbers; a shift's amount is a whole number of any type, taken mod the width)
  template <class P, num::Integer T = num::raw<P>> static T math_and(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::bit_and(a, b); }
  template <class P, num::Integer T = num::raw<P>> static T math_or(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::bit_or(a, b); }
  template <class P, num::Integer T = num::raw<P>> static T math_xor(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::bit_xor(a, b); }
  template <class P, num::Integer T = num::raw<P>> static T math_not(std::type_identity_t<T> a) { return num::bit_not(a); }
  template <class P, num::Integer T = num::raw<P>> static T math_shift_left(std::type_identity_t<T> a, i64 by) { return num::shl(a, by); }
  template <class P, num::Integer T = num::raw<P>> static T math_shift_right(std::type_identity_t<T> a, i64 by) { return num::shr(a, by); }
  template <class P, num::Integer T = num::raw<P>> static T math_rotate_left(std::type_identity_t<T> a, i64 by) { return num::rotl(a, by); }
  template <class P, num::Integer T = num::raw<P>> static T math_rotate_right(std::type_identity_t<T> a, i64 by) { return num::rotr(a, by); }
  template <class P, num::Integer T = num::raw<P>> static i32 math_count_ones(std::type_identity_t<T> a) { return num::popcount(a); }
  template <class P, num::Integer T = num::raw<P>> static i32 math_leading_zeros(std::type_identity_t<T> a) { return num::countl_zero(a); }
  template <class P, num::Integer T = num::raw<P>> static i32 math_trailing_zeros(std::type_identity_t<T> a) { return num::countr_zero(a); }
  // (the functions of <cmath>: radians, as C++'s; the engine's own sin, exp ... - kern::dm, musl)
  template <class P, num::Floating T = num::raw<P>> static T math_sqrt(std::type_identity_t<T> a) { return num::sqrt(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_floor(std::type_identity_t<T> a) { return num::floor(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_ceil(std::type_identity_t<T> a) { return num::ceil(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_trunc(std::type_identity_t<T> a) { return num::trunc(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_round(std::type_identity_t<T> a) { return num::round(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_copysign(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::copysign(a, b); }
  template <class P, num::Floating T = num::raw<P>> static T math_fmin(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::fmin(a, b); }
  template <class P, num::Floating T = num::raw<P>> static T math_fmax(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::fmax(a, b); }
  template <class P, num::Floating T = num::raw<P>> static T math_sin(std::type_identity_t<T> a) { return num::sin(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_cos(std::type_identity_t<T> a) { return num::cos(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_tan(std::type_identity_t<T> a) { return num::tan(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_asin(std::type_identity_t<T> a) { return num::asin(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_acos(std::type_identity_t<T> a) { return num::acos(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_atan(std::type_identity_t<T> a) { return num::atan(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_atan2(std::type_identity_t<T> y, std::type_identity_t<T> x) { return num::atan2(y, x); }
  template <class P, num::Floating T = num::raw<P>> static T math_exp(std::type_identity_t<T> a) { return num::exp(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_expm1(std::type_identity_t<T> a) { return num::expm1(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_sinh(std::type_identity_t<T> a) { return num::sinh(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_cosh(std::type_identity_t<T> a) { return num::cosh(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_tanh(std::type_identity_t<T> a) { return num::tanh(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_asinh(std::type_identity_t<T> a) { return num::asinh(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_log(std::type_identity_t<T> a) { return num::log(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_log10(std::type_identity_t<T> a) { return num::log10(a); }
  template <class P, num::Floating T = num::raw<P>> static T math_pow(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::pow(a, b); }
  template <class P, num::Floating T = num::raw<P>> static T math_hypot(std::type_identity_t<T> a, std::type_identity_t<T> b) { return num::hypot(a, b); }
  template <class P, num::Floating T = num::raw<P>> static Bool math_isnan(std::type_identity_t<T> a) { return num::isnan(a); }
  template <class P, num::Floating T = num::raw<P>> static Bool math_isinf(std::type_identity_t<T> a) { return num::isinf(a); }
  template <class P, num::Floating T = num::raw<P>> static Bool math_isfinite(std::type_identity_t<T> a) { return num::isfinite(a); }
  template <class P, num::Floating T = num::raw<P>> static Bool math_signbit(std::type_identity_t<T> a) { return num::signbit(a); }
  // (conversions: one node for each, From and To its settings - static_cast<To>(x), numbers.h's cast)
  template <class To0, class From0, num::Number To = num::raw<To0>, num::Number From = num::raw<From0>> static To num_cast(std::type_identity_t<From> x) { return num::cast<To>(x); }
  /// (the same in a formula: num_cast<f64, i64>(n) of the formula's input n - kern::formula, numbers.h Typed)
  template <class To0, class From0, num::Number To = num::raw<To0>, num::Number From = num::raw<From0>> static num::Typed<To> num_cast(num::Typed<From> x) { return num::cast<To>(x.v); }
  template <class To0, class From0, num::Number To = num::raw<To0>, num::Number From = num::raw<From0>> static num::Typed<To> num_bit_cast(num::Typed<From> x) { return num::reinterpret<To>(x.v); }
  /// The bits of x as the type To of the same size (std::bit_cast: f32 <-> u32, f64 <-> u64 ...).
  template <class To0, class From0, num::Number To = num::raw<To0>, num::Number From = num::raw<From0>> static To num_bit_cast(std::type_identity_t<From> x) { return num::reinterpret<To>(x); }
  /// A number as a text (To Text): logic::number_as_text - a whole number in decimal, a floating one the shortest
  /// decimal that gives it again, inf, -inf, nan.
  template <class P, num::Number T = num::raw<P>> static Text num_text(std::type_identity_t<T> x) { return number_as_text(Value::of_number(x)); }
  /// A whole number's bits in hexadecimal (lower case, no 0x) - std::to_chars(..., 16) of the unsigned bits of its type.
  template <class P, num::Integer T = num::raw<P>> static Text num_text_hex(std::type_identity_t<T> x) {
    char b[20];
    auto [end, ec] = std::to_chars(b, b + sizeof b, static_cast<std::make_unsigned_t<T>>(x), 16);
    (void)ec;
    return Text(b, end);
  }
  /// A text as 64 bits: its whole number (decimal, 0x hexadecimal; a - in front: two's complement), 0 if it is none.
  static u64 text_to_u64(const std::string& text) { return int64_parse(text); }
  static u64 int64_parse(std::string_view a) {
    bool minus = !a.empty() && a.front() == '-';
    if (minus) a.remove_prefix(1);
    int base = 10;
    if (a.size() > 2 && a[0] == '0' && (a[1] == 'x' || a[1] == 'X')) base = 16, a.remove_prefix(2);
    u64 v = 0;
    const auto [end, ec] = std::from_chars(a.data(), a.data() + a.size(), v, base);
    if (ec != std::errc() || end != a.data() + a.size()) return 0;
    return minus ? 0 - v : v;
  }

  // ---- logic ----
  static Bool logic_and(Bool a, Bool b) { return a && b; }
  static Bool logic_or(Bool a, Bool b) { return a || b; }
  static Bool logic_xor(Bool a, Bool b) { return a != b; }
  static Bool logic_not(Bool a) { return !a; }

  // ---- values ----
  /// A number written out, of the type of the node's setting.
  template <class P, num::Number T = num::raw<P>> static T value_number(std::type_identity_t<T> value) { return value; }
  static Bool value_bool(Bool value) { return value; }
  static Text value_text(Text value) { return value; }

  // ---- vectors ----
  struct Broken {
    f32 x = 0, y = 0, z = 0;
  };
  static Vector vector_make(f32 x, f32 y, f32 z) { return {x, y, z}; }
  static Broken vector_break(Vector v) { return {v.x, v.y, v.z}; }
  static Vector vector_add(Vector a, Vector b) { return a + b; }
  static Vector vector_sub(Vector a, Vector b) { return a - b; }
  static Vector vector_cross(Vector a, Vector b) { return cross(a, b); }
  static Vector vector_scale(Vector a, f32 b) { return a * b; }
  static Float vector_length(Vector a) { return length(a); }
  static Vector vector_normalize(Vector a) { return normalize(a, {0, 0, 0}); }
  static Float vector_distance(Vector a, Vector b) { return distance(a, b); }
  static Float vector_dot(Vector a, Vector b) { return dot(a, b); }
  static Vector vector_lerp(Vector a, Vector b, f32 alpha) { return lerp(a, b, alpha); }

  // ---- text ----
  static Text text_append(Text a, Text b, Text c) { return a + b + c; }
  static Bool text_equal(Text a, Text b) { return a == b; }
  static Bool text_contains(Text a, Text b) { return a.find(b) != std::string::npos; }
  static Int text_length(Text a) { return static_cast<i64>(a.size()); }
  /// The byte at `index` as a number 0..255 (beyond the text: 0 - as std::string has a 0 after its last).
  static Int text_code_at(Text a, Int index) { return index >= 0 && static_cast<u64>(index) < a.size() ? static_cast<i64>(static_cast<u8>(a[static_cast<std::size_t>(index)])) : 0; }
  /// A text of one byte: the code cut to 8 bits, as a char holds it.
  static Text text_of_code(Int code) { return Text(1, static_cast<char>(static_cast<u8>(code))); }
  // (what std::string and std::string_view do - texts, nodes.h: a place is an Int, npos -1)
  static Int text_find(Text a, Text b, Int from) { return texts::find(a, b, from); }
  static Int text_rfind(Text a, Text b, Int from) { return texts::rfind(a, b, from); }
  static Int text_find_first_of(Text a, Text b, Int from) { return texts::find_first_of(a, b, from); }
  static Int text_find_last_of(Text a, Text b, Int from) { return texts::find_last_of(a, b, from); }
  static Int text_find_first_not_of(Text a, Text b, Int from) { return texts::find_first_not_of(a, b, from); }
  static Int text_find_last_not_of(Text a, Text b, Int from) { return texts::find_last_not_of(a, b, from); }
  static Text text_sub(Text a, Int at, Int count) { return Text(texts::sub(a, at, count)); }
  static Bool text_starts_with(Text a, Text b) { return std::string_view(a).starts_with(b); }
  static Bool text_ends_with(Text a, Text b) { return std::string_view(a).ends_with(b); }
  /// a < b, byte for byte as unsigned chars (std::string's order).
  static Bool text_less(Text a, Text b) { return a < b; }
  static Text text_insert(Text a, Int at, Text b) {
    texts::insert(a, at, b);
    return a;
  }
  static Text text_erase(Text a, Int at, Int count) {
    texts::erase(a, at, count);
    return a;
  }
  static Text text_replace(Text a, Int at, Int count, Text b) {
    texts::replace(a, at, count, b);
    return a;
  }
  // (numbers read from a text at a place, as C reads them there - scan, nodes.h: the value and the place
  // just after what was read; nothing read: the place itself, as C's end pointer is the one it was given)
  // (strtod: an f64, strtof: an f32, strtoll: an i64, strtoull: a u64)
  struct ParsedFloat {
    f64 value = 0;
    Int end = 0;
  };
  struct ParsedFloat32 {
    f32 value = 0;
    Int end = 0;
  };
  struct ParsedWhole {
    i64 value = 0;
    Int end = 0;
  };
  struct ParsedUnsigned {
    u64 value = 0;
    Int end = 0;
  };
  /// The text the bytes of a list are (each number cut to 8 bits, as a char holds it): Count of them from At
  /// on - what std::string(reinterpret_cast<const char*>(bytes) + at, count) makes; beyond the list: up to its end.
  template <num::Integer T> static Text text_of_bytes(const std::vector<T>& bytes, Int at, Int count) { return texts::of_bytes(bytes, at, count); }
  /// Bytes of Text: its bytes - std::vector<u8>(text.begin(), text.end()).
  static std::vector<u8> text_bytes(Text a) {
    std::vector<u8> r;
    texts::insert_bytes(r, 0, a);
    return r;
  }
  /// Reserve Text: the text as it is - room for Count characters is a matter of the class (std::string::reserve).
  static Text text_reserve(Text a, Int count) {
    (void)count;
    return a;
  }
  /// Text so many times one after the other - std::string(count, c) of one character (none: count 0 or less;
  /// at most 2^24 characters).
  static Text text_repeat(Text a, Int count) { return texts::repeat(a, count); }
  static ParsedFloat text_parse_float(Text a, Int at) {
    const scan::Real r = scan::real(scan::c_string(a, at), false);
    return {r.value, at + static_cast<i64>(r.end)};
  }
  static ParsedFloat32 text_parse_float32(Text a, Int at) {
    const scan::Real r = scan::real(scan::c_string(a, at), true);
    return {static_cast<f32>(r.value), at + static_cast<i64>(r.end)};  // (exact: scan gives the f32 it read)
  }
  static ParsedWhole text_parse_int(Text a, Int at, Int base) {
    const scan::Whole r = scan::whole(scan::c_string(a, at), base < 0 || base > 36 ? -1 : static_cast<int>(base), true);
    return {static_cast<i64>(r.bits), at + static_cast<i64>(r.end)};
  }
  static ParsedUnsigned text_parse_uint(Text a, Int at, Int base) {
    const scan::Whole r = scan::whole(scan::c_string(a, at), base < 0 || base > 36 ? -1 : static_cast<int>(base), false);
    return {r.bits, at + static_cast<i64>(r.end)};
  }
};

/// The flow nodes that keep something between runs. `m`: the node's memory, four numbers (the
/// interpreter's slots of the node; kern::Node in a Blueprint's C++). `entry`: the exec input,
/// by its place among the node's exec inputs.
namespace flow {
/// For Loop. start: at its exec input; step: every time round. True: run the body with index().
inline void for_start(f64* m, i64 first, i64 last) {
  m[0] = static_cast<f64>(first);
  m[1] = static_cast<f64>(last);
}
inline void for_step(f64* m) { m[0] += 1; }
inline bool for_runs(const f64* m) { return !(m[0] > m[1]); }
inline i64 for_index(const f64* m) { return whole(m[0]); }
/// Do Once (entry 1: Reset). True: the call goes through.
inline bool do_once(f64* m, int entry, bool start_closed) {
  f64& closed = m[0];
  f64& started = m[1];
  if (entry == 1) {
    closed = 0;
    started = 1;
    return false;
  }
  if (started == 0) {
    started = 1;
    closed = start_closed ? 1 : 0;
  }
  if (closed != 0) return false;
  closed = 1;
  return true;
}
/// Do N (entry 1: Reset). True: the call goes through, and `counter` is its number.
inline bool do_n(f64* m, int entry, i64 n, i64& counter) {
  if (entry == 1) {
    m[0] = 0;
    return false;
  }
  if (m[0] >= static_cast<f64>(n)) return false;
  m[0] += 1;
  counter = whole(m[0]);
  return true;
}
/// Gate (entries: 0 Enter, 1 Open, 2 Close, 3 Toggle). True: Enter goes through.
inline bool gate(f64* m, int entry, bool start_closed) {
  f64& open = m[0];
  f64& started = m[1];
  if (started == 0) {
    started = 1;
    open = start_closed ? 0 : 1;
  }
  switch (entry) {
    case 0: return open != 0;
    case 1: open = 1; return false;
    case 2: open = 0; return false;
    case 3: open = open != 0 ? 0 : 1; return false;
    default: return false;
  }
}
/// Flip Flop: the exec output it takes (0 A, 1 B).
inline int flip_flop(f64* m, bool& is_a) {
  m[0] = m[0] != 0 ? 0 : 1;
  is_a = m[0] != 0;
  return is_a ? 0 : 1;
}
/// Delay. start: true when the wait begins (a call while it waits is ignored); done: it is over.
inline bool delay_start(f64* m) {
  if (m[0] != 0) return false;
  m[0] = 1;
  return true;
}
inline void delay_done(f64* m) { m[0] = 0; }
/// Tween. m: [0] it runs, [1] when it began, [2] how long it takes.
inline void tween_play(f64* m, f64 now, f64 duration) {
  m[0] = 1;
  m[1] = now;
  m[2] = max_of(0.0, duration);
}
inline void tween_stop(f64* m) { m[0] = 0; }
inline bool tween_runs(const f64* m) { return m[0] != 0; }
/// One step of a running tween at time `now`: its alpha (0..1, eased), and whether that was the
/// last one (the tween then no longer runs). `first`: the step that comes with Play - never the
/// last one: Finished comes with the last Update on a later step, also for a tween of no duration.
inline f64 tween_step(f64* m, f64 now, std::string_view ease, bool first, bool& finished) {
  const f64 t = kern::clamp(m[2] > 0 ? (now - m[1]) / m[2] : 1.0, 0.0, 1.0);
  f64 e = t;
  if (ease == "smooth") e = t * t * (3 - 2 * t);
  else if (ease == "in") e = t * t;
  else if (ease == "out") e = 1 - (1 - t) * (1 - t);
  finished = t >= 1 && !first;
  if (finished) m[0] = 0;
  return e;
}
/// Switch on Int with `cases` cases: the exec output (the case, or `cases` for Default).
inline int switch_int(int cases, i64 selection) { return selection >= 0 && selection < cases ? static_cast<int>(selection) : cases; }
/// Switch on Text: the exec output (the case that equals the selection, or the count for Default).
/// (the cases as setting_cases has them - each piece between commas without the blanks around it,
/// empty ones left out -, read where they stand: no list of them is made)
inline int switch_text(std::string_view cases, std::string_view selection) {
  int index = 0, found = -1;
  std::size_t from = 0;
  while (from <= cases.size()) {
    std::size_t to = cases.find(',', from);
    if (to == std::string_view::npos) to = cases.size();
    std::string_view one = cases.substr(from, to - from);
    while (!one.empty() && one.front() == ' ') one.remove_prefix(1);
    while (!one.empty() && one.back() == ' ') one.remove_suffix(1);
    if (!one.empty()) {
      if (found < 0 && one == selection) found = index;
      ++index;
    }
    from = to + 1;
  }
  return found >= 0 ? found : index;
}
}  // namespace flow

}  // namespace logic


/// What a wire from a place of the engine gives: what stands there, as its own type (engine_direct.h). No wire converts
/// a value into another type (a conversion is a node: num_cast<To, From>).
template <class T, class S> T to(const S& v) {
  if constexpr (requires { typename S::kern_place_value; }) return to<T>(static_cast<typename S::kern_place_value>(v));
  else return static_cast<const T&>(v);
}

}  // namespace kern
