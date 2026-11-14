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
#include <cstddef>
#include <deque>
#include <functional>
#include <initializer_list>
#include <iterator>
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

/// A node's setting as a template argument: flow_switch_int<"3">, math_compare<"<=">.
template <std::size_t N> struct Setting {
  char text[N]{};
  constexpr Setting(const char (&s)[N]) {  // NOLINT (implicit: that is the point)
    for (std::size_t i = 0; i < N; ++i) text[i] = s[i];
  }
  constexpr std::string_view view() const { return {text, N - 1}; }
};

/// The functions of formulas (the Formula node; Formula::evaluate uses the same ones).
namespace math {
inline constexpr f64 pi = 3.14159265358979323846;
inline f64 div(f64 a, f64 b) { return b != 0 ? a / b : 0; }
inline f64 mod(f64 a, f64 b) { return b != 0 ? std::fmod(a, b) : 0; }
inline f64 pow(f64 a, f64 b) { return dm::pow(a, b); }
inline f64 min(f64 a, f64 b) { return min_of(a, b); }
inline f64 max(f64 a, f64 b) { return max_of(a, b); }
inline f64 clamp(f64 x, f64 lo, f64 hi) { return kern::clamp(x, min_of(lo, hi), max_of(lo, hi)); }
inline f64 abs(f64 a) { return std::fabs(a); }
inline f64 sign(f64 a) { return a > 0 ? 1 : a < 0 ? -1 : 0; }
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
inline f64 rad(f64 degrees) { return degrees * (pi / 180); }
inline f64 deg(f64 radians) { return radians * (180 / pi); }
inline f64 lerp(f64 a, f64 b, f64 t) { return a + (b - a) * t; }
// Whole numbers as a machine has them (a formula's band, bor, bxor, shl, shr, wrap32, wrap32s,
// mul32). A number is taken as the whole number it is (what stands after the point is cut off);
// its bits are those of a 64-bit number in two's complement. wrap32 is the number a 32-bit
// unsigned number holds of it (0 .. 4294967295: what a sum or a shift wraps around to), wrap32s
// the one a signed 32-bit number holds; mul32 is the product of two 32-bit numbers as 32 bits
// hold it (their full product does not fit a formula's number exactly).
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
/// The number as a 32-bit float holds it (a formula's single): what a vector's component is.
inline f64 single(f64 a) { return static_cast<f64>(static_cast<f32>(a)); }
inline f64 mul32(f64 a, f64 b) { return static_cast<f64>(static_cast<u32>(static_cast<u32>(static_cast<u64>(bits_of(a))) * static_cast<u32>(static_cast<u64>(bits_of(b))))); }
}  // namespace math

namespace logic {

/// `Int` in a Blueprint's C++: a whole number as a Blueprint holds it (logic::normal). It is an
/// i64 to every function that takes one. Adding to it - Add to Variable, `Count += 1` - adds as
/// the interpreter and the JIT add: as numbers, defined for every value.
struct Whole {
  i64 v = 0;
  constexpr Whole() = default;
  constexpr Whole(i64 x) : v(normal(x)) {}       // NOLINT (implicit: an i64 is one)
  constexpr operator i64() const { return v; }   // NOLINT
  Whole& operator+=(Whole amount) {
    v = whole_add(v, amount.v);
    return *this;
  }
  u64 hash() const { return KeyHash<i64>{}(v); }  // (a key of a Map, a Set)
};

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
  template <std::size_t N> constexpr Text(const char (&written)[N]) : written_(written, N - 1) {}  // NOLINT (implicit: a text written out is a Text)
  template <std::size_t N> Text(char (&)[N]) = delete;
  Text(std::string made) : made_(std::move(made)), holds_(true) {}                                // NOLINT (implicit: a text made while running)
  explicit Text(std::string_view copied) : made_(copied), holds_(true) {}
  /// A text the program holds for good, by its characters - the name of an enum's value:
  /// referred to, not copied. (Only for what stays where it is as long as the program runs.)
  static Text of_program(std::string_view held) {
    Text t;
    t.written_ = held;
    return t;
  }
  Text(const Text& o) : holds_(o.holds_) {
    if (holds_) new (&made_) std::string(o.made_);
    else new (&written_) std::string_view(o.written_);
  }
  Text(Text&& o) noexcept : holds_(o.holds_) {
    if (holds_) new (&made_) std::string(std::move(o.made_));
    else new (&written_) std::string_view(o.written_);
  }
  Text& operator=(const Text& o) {
    if (this == &o) return *this;
    if (holds_ && o.holds_) made_ = o.made_;  // (into the characters it has: std::string's own assignment)
    else if (o.holds_) hold(std::string(o.made_));
    else refer(o.written_);
    return *this;
  }
  Text& operator=(Text&& o) noexcept {
    if (this == &o) return *this;
    if (holds_ && o.holds_) made_ = std::move(o.made_);
    else if (o.holds_) hold(std::move(o.made_));
    else refer(o.written_);
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
  /// Its characters as a std::string of their own (a copy), and handed on without one where it holds them.
  std::string str() const& { return std::string(view()); }
  std::string str() && { return holds_ ? std::move(made_) : std::string(written_); }
  friend bool operator==(const Text& a, const Text& b) { return a.view() == b.view(); }
  friend auto operator<=>(const Text& a, const Text& b) { return a.view() <=> b.view(); }

 private:
  void hold(std::string made) {
    if (holds_) made_ = std::move(made);
    else new (&made_) std::string(std::move(made)), holds_ = true;
  }
  void refer(std::string_view written) {
    if (holds_) made_.~basic_string(), holds_ = false;
    new (&written_) std::string_view(written);
  }
  union {
    std::string_view written_;  // a text of the program: its characters there
    std::string made_;          // a text made while running: its characters here
  };
  bool holds_ = false;
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
}  // namespace detail

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
  template <class I>
    requires std::is_integral_v<I> || std::is_same_v<I, Whole>
  Item& operator[](I index) {
    const i64 at = static_cast<i64>(index);
    if (at >= 0 && static_cast<std::size_t>(at) < this->size()) return Items::operator[](static_cast<std::size_t>(at));
    return beside::entry<Item>();
  }
  template <class I>
    requires std::is_integral_v<I> || std::is_same_v<I, Whole>
  const Item& operator[](I index) const {
    const i64 at = static_cast<i64>(index);
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
    requires std::is_integral_v<I> || std::is_same_v<I, Whole>
  Item& operator[](I index) {
    const i64 at = static_cast<i64>(index);
    if (at >= 0 && static_cast<std::size_t>(at) < N) return items[static_cast<std::size_t>(at)];
    return beside::entry<Item>();
  }
  template <class I>
    requires std::is_integral_v<I> || std::is_same_v<I, Whole>
  const Item& operator[](I index) const {
    const i64 at = static_cast<i64>(index);
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

/// A value of the interpreter as the C++ type of its pin, and back. A list is the vector of its
/// items; a list of whole numbers is a vector of i64 in the interpreter and of Whole (`Int`) in a
/// Blueprint's class.
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
template <> struct Held<i64> {
  static constexpr PinType type = PinType::integer;
  static i64 from(const Value& v) { return v.as_int(); }
};
template <> struct Held<Whole> {
  static constexpr PinType type = PinType::integer;
  static Whole from(const Value& v) { return v.as_int(); }
};
template <> struct Held<f64> {
  static constexpr PinType type = PinType::number;
  static f64 from(const Value& v) { return v.as_number(); }
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
  static ActorHandle from(const Value& v) { return {v.as_actor()}; }
};
template <> struct Held<Int64> {
  static constexpr PinType type = PinType::int64;
  static Int64 from(const Value& v) { return Int64(v.type == PinType::int64 ? static_cast<u64>(v.i) : 0); }
};
template <class T> struct Held<std::vector<T>> {
  static constexpr PinType type = list_type_of<T>();
  static std::vector<T> from(const Value& v) { return v.items<T>(); }
};
template <> struct Held<std::vector<Whole>> {
  static constexpr PinType type = PinType::integer_list;
  static std::vector<Whole> from(const Value& v) {
    const std::vector<i64>& items = v.items<i64>();
    return std::vector<Whole>(items.begin(), items.end());
  }
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
/// What a function of the engine gives where a Blueprint's own type stands - one of its structs or
/// enums, or a list of them: the value, read as that type where it is used (engine_nodes.gen.h).
namespace detail {
template <class T> inline constexpr bool is_initializer_list = false;
template <class T> inline constexpr bool is_initializer_list<std::initializer_list<T>> = true;
}  // namespace detail
struct EngineValue {
  Value v;
  /// (Not a list written out in braces: a List is assigned as the list it is.)
  template <class T>
    requires(!detail::is_initializer_list<T>)
  operator T() const { return from_value<T>(v); }
};
template <class T> constexpr PinType pin_type_of() { return detail::Held<T>::type; }
/// Is `v` a value of the C++ type T as a Blueprint holds it (logic::is_value_of: of that pin
/// type, an enum's value one of its values, a struct with a value of its type for every field)?
template <class T> bool is_value(const Value& v) {
  if constexpr (requires { detail::Held<T>::is(v); }) return detail::Held<T>::is(v);
  else return v.type == pin_type_of<T>();
}
inline Value to_value(bool v) { return Value::boolean(v); }
inline Value to_value(i64 v) { return Value::integer(v); }
inline Value to_value(Whole v) { return Value::integer(v); }
inline Value to_value(f64 v) { return Value::number(v); }
inline Value to_value(Vec3 v) { return Value::vector(v); }
inline Value to_value(std::string v) { return Value::text(std::move(v)); }
inline Value to_value(const Text& v) { return Value::text(v.str()); }
inline Value to_value(Text&& v) { return Value::text(std::move(v).str()); }
inline Value to_value(ActorHandle v) { return Value::actor(v.id); }
inline Value to_value(Int64 v) { return Value::int64(v.bits); }
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
inline Value to_value(const std::vector<Whole>& v) { return Value::of(std::vector<i64>(v.begin(), v.end())); }
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
template <class T> bool remove(std::vector<T>& items, const std::type_identity_t<T>& item) {
  std::size_t kept = 0;
  for (std::size_t k = 0; k < items.size(); ++k) {
    {
      const T& at = items[k];
      if (at == item) continue;
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
    requires std::is_integral_v<I> || std::is_same_v<I, Whole>
  Item& operator[](I index) {
    const i64 at = static_cast<i64>(index);
    if (at >= 0 && static_cast<std::size_t>(at) < this->size()) return this->value_at(static_cast<std::size_t>(at));
    return beside::entry<Item>();
  }
  template <class I>
    requires std::is_integral_v<I> || std::is_same_v<I, Whole>
  const Item& operator[](I index) const {
    const i64 at = static_cast<i64>(index);
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
inline Arg arg(Whole v) { return arg(static_cast<i64>(v)); }
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
template <class T> T native_arg(const std::vector<Value>& args, std::size_t i, const T& fallback) {
  return i < args.size() ? from_value<T>(args[i].converted(pin_type_of<T>())) : fallback;
}
/// A value an event brought, as the C++ type of its handler's parameter: output `out` of the
/// event's node `node` (Runtime::fire puts the event's values there, for every tier).
template <class T> T native_out(const Instance& instance, int node, int out) { return from_value<T>(instance.output(node, out).converted(pin_type_of<T>())); }
/// Instance::set for a variable that is a member of a Blueprint's class: the conversions the
/// interpreter's variables make.
inline void native_set(bool& variable, const Value& v) { variable = v.as_number() != 0; }
inline void native_set(i64& variable, const Value& v) { variable = whole(v.as_number()); }
inline void native_set(Whole& variable, const Value& v) { variable = whole(v.as_number()); }
inline void native_set(f64& variable, const Value& v) { variable = v.as_number(); }
inline void native_set(Vec3& variable, const Value& v) { variable = v.as_vector(); }
inline void native_set(std::string& variable, const Value& v) { variable = v.as_text(); }
inline void native_set(Text& variable, const Value& v) { variable = Text(v.as_text()); }
inline void native_set(ActorHandle& variable, const Value& v) { variable = {v.as_actor()}; }
template <class T> void native_set(std::vector<T>& variable, const Value& v) { variable = from_value<std::vector<T>>(v.converted(pin_type_of<std::vector<T>>())); }
template <class T, std::size_t N> void native_set(Fixed<T, N>& variable, const Value& v) { variable = from_value<Fixed<T, N>>(v.converted(pin_type_of<Fixed<T, N>>())); }
template <class E>
  requires std::is_enum_v<E>
void native_set(E& variable, const Value& v) {
  variable = from_value<E>(v.converted(PinType::enumeration));
}
template <detail::NativeStruct S> void native_set(S& variable, const Value& v) { variable = from_value<S>(v); }

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
  using Int = i64;
  using Float = f64;
  using Vector = Vec3;
  using Text = std::string;
  using Actor = ActorHandle;
  static constexpr f64 kDegrees = 3.14159265358979323846 / 180.0;

  // ---- Int64: 64 bits, as u64 and i64 of C++ (WebAssembly's i64) ----
  // Adding, subtracting, multiplying and shifting left go round at 2^64; what C++ leaves open is
  // defined here for every value, the same in every tier: dividing by 0 gives 0 (and the
  // smallest i64 by -1 itself), a shift goes by the amount's last 6 bits, a Float that is no
  // number is 0 as an Int64, beyond its ends it is the nearest end.
  using Int64 = logic::Int64;
  static Int64 int64_add(Int64 a, Int64 b) { return Int64(a.bits + b.bits); }
  static Int64 int64_sub(Int64 a, Int64 b) { return Int64(a.bits - b.bits); }
  static Int64 int64_mul(Int64 a, Int64 b) { return Int64(a.bits * b.bits); }
  static Int64 int64_div(Int64 a, Int64 b) {
    const i64 x = static_cast<i64>(a.bits), y = static_cast<i64>(b.bits);
    if (y == 0) return Int64(0);
    if (y == -1) return Int64(0 - a.bits);  // (the smallest i64 by -1: itself, as it goes round)
    return Int64(static_cast<u64>(x / y));
  }
  static Int64 int64_div_unsigned(Int64 a, Int64 b) { return Int64(b.bits ? a.bits / b.bits : 0); }
  static Int64 int64_rem(Int64 a, Int64 b) {
    const i64 x = static_cast<i64>(a.bits), y = static_cast<i64>(b.bits);
    if (y == 0 || y == -1) return Int64(0);
    return Int64(static_cast<u64>(x % y));
  }
  static Int64 int64_rem_unsigned(Int64 a, Int64 b) { return Int64(b.bits ? a.bits % b.bits : 0); }
  static Int64 int64_and(Int64 a, Int64 b) { return Int64(a.bits & b.bits); }
  static Int64 int64_or(Int64 a, Int64 b) { return Int64(a.bits | b.bits); }
  static Int64 int64_xor(Int64 a, Int64 b) { return Int64(a.bits ^ b.bits); }
  static Int64 int64_not(Int64 a) { return Int64(~a.bits); }
  static Int64 int64_shift_left(Int64 a, Int by) { return Int64(a.bits << (static_cast<u64>(by) & 63)); }
  static Int64 int64_shift_right(Int64 a, Int by) { return Int64(static_cast<u64>(static_cast<i64>(a.bits) >> (static_cast<u64>(by) & 63))); }
  static Int64 int64_shift_right_unsigned(Int64 a, Int by) { return Int64(a.bits >> (static_cast<u64>(by) & 63)); }
  static Int64 int64_rotate_left(Int64 a, Int by) { return Int64(std::rotl(a.bits, static_cast<int>(static_cast<u64>(by) & 63))); }
  static Int64 int64_rotate_right(Int64 a, Int by) { return Int64(std::rotr(a.bits, static_cast<int>(static_cast<u64>(by) & 63))); }
  static Bool int64_equal(Int64 a, Int64 b) { return a.bits == b.bits; }
  static Bool int64_less(Int64 a, Int64 b) { return static_cast<i64>(a.bits) < static_cast<i64>(b.bits); }
  static Bool int64_less_unsigned(Int64 a, Int64 b) { return a.bits < b.bits; }
  /// An Int as 64 bits: an i64 (a whole number below 0 is its two's complement).
  static Int64 int64_of_int(Int a) { return Int64(static_cast<u64>(static_cast<i64>(a))); }
  /// The bits as an i64 / a u64, as an Int holds that (exact up to 2^53, beyond: the nearest).
  static Int int64_to_int(Int64 a) { return normal(static_cast<i64>(a.bits)); }
  static Int int64_to_int_unsigned(Int64 a) { return whole(static_cast<f64>(a.bits)); }
  static Float int64_to_float(Int64 a) { return static_cast<f64>(static_cast<i64>(a.bits)); }
  static Float int64_to_float_unsigned(Int64 a) { return static_cast<f64>(a.bits); }
  /// A Float toward zero as an i64 / a u64 (no number: 0; beyond the ends: the nearest end).
  static Int64 int64_of_float(Float a) {
    if (!(a == a)) return Int64(0);
    if (a >= 9223372036854775808.0) return Int64(static_cast<u64>(std::numeric_limits<i64>::max()));
    if (a < -9223372036854775808.0) return Int64(static_cast<u64>(std::numeric_limits<i64>::min()));
    return Int64(static_cast<u64>(static_cast<i64>(a)));
  }
  static Int64 int64_of_float_unsigned(Float a) {
    if (!(a > 0)) return Int64(0);
    if (a >= 18446744073709551616.0) return Int64(~u64{0});
    return Int64(static_cast<u64>(a));
  }
  /// A Float's 64 bits (IEEE 754), and the Float of 64 bits.
  static Int64 int64_bits_of_float(Float a) { return Int64(std::bit_cast<u64>(a)); }
  static Float int64_float_of_bits(Int64 a) { return std::bit_cast<f64>(a.bits); }
  /// Two halves of 32 bits each (an Int each, cut to its last 32 bits), and the halves again (0 .. 2^32 - 1).
  static Int64 int64_of_halves(Int high, Int low) { return Int64((static_cast<u64>(static_cast<u32>(high)) << 32) | static_cast<u32>(low)); }
  static Int int64_high(Int64 a) { return static_cast<i64>(a.bits >> 32); }
  static Int int64_low(Int64 a) { return static_cast<i64>(a.bits & 0xffffffffu); }
  static Int int64_count_ones(Int64 a) { return static_cast<i64>(std::popcount(a.bits)); }
  static Int int64_leading_zeros(Int64 a) { return static_cast<i64>(std::countl_zero(a.bits)); }
  static Int int64_trailing_zeros(Int64 a) { return static_cast<i64>(std::countr_zero(a.bits)); }
  /// The bits as a text: a u64 in decimal, an i64 in decimal, hexadecimal (lower case, no 0x).
  static std::string int64_text(Int64 a) { return std::to_string(a.bits); }
  static std::string int64_text_signed(Int64 a) { return std::to_string(static_cast<i64>(a.bits)); }
  static std::string int64_text_hex(Int64 a) {
    char b[20];
    auto [end, ec] = std::to_chars(b, b + sizeof b, a.bits, 16);
    (void)ec;
    return std::string(b, end);
  }
  /// A text as 64 bits: its whole number (decimal, 0x hexadecimal; a - in front: two's complement), 0 if it is none.
  static Int64 int64_of_text(const std::string& text) { return int64_parse(text); }
  static Int64 int64_parse(std::string_view a) {
    bool minus = !a.empty() && a.front() == '-';
    if (minus) a.remove_prefix(1);
    int base = 10;
    if (a.size() > 2 && a[0] == '0' && (a[1] == 'x' || a[1] == 'X')) base = 16, a.remove_prefix(2);
    u64 v = 0;
    const auto [end, ec] = std::from_chars(a.data(), a.data() + a.size(), v, base);
    if (ec != std::errc() || end != a.data() + a.size()) return Int64(0);
    return Int64(minus ? 0 - v : v);
  }

  // ---- math ----
  static Float math_add(Float a, Float b) { return a + b; }
  static Float math_sub(Float a, Float b) { return a - b; }
  static Float math_mul(Float a, Float b) { return a * b; }
  static Float math_div(Float a, Float b) { return b != 0 ? a / b : 0.0; }
  static Float math_mod(Float a, Float b) { return b != 0 ? std::fmod(a, b) : 0.0; }
  static Float math_min(Float a, Float b) { return min_of(a, b); }
  static Float math_max(Float a, Float b) { return max_of(a, b); }
  static Float math_clamp(Float value, Float min, Float max) { return kern::clamp(value, min_of(min, max), max_of(min, max)); }
  static Float math_abs(Float a) { return std::fabs(a); }
  static Float math_negate(Float a) { return -a; }
  static Float math_lerp(Float a, Float b, Float alpha) { return a + (b - a) * alpha; }
  static Float math_sin(Float degrees) { return dm::sin(degrees * kDegrees); }
  static Float math_cos(Float degrees) { return dm::cos(degrees * kDegrees); }
  static Float math_atan2(Float y, Float x) { return dm::atan2(y, x) / kDegrees; }
  static Float math_sqrt(Float a) { return std::sqrt(max_of(0.0, a)); }
  static Float math_pow(Float a, Float b) {
    const f64 r = dm::pow(a, b);
    return std::isfinite(r) ? r : 0.0;
  }
  static Int math_round(Float a) { return whole(std::round(a)); }  // (half away from zero)
  static Int math_floor(Float a) { return whole(std::floor(a)); }
  /// Compare with the operator of the node's setting (== and != within 1e-9).
  static Bool compare(std::string_view op, Float a, Float b) {
    if (op == "==") return std::fabs(a - b) <= 1e-9;
    if (op == "!=") return std::fabs(a - b) > 1e-9;
    if (op == "<") return a < b;
    if (op == "<=") return a <= b;
    if (op == ">") return a > b;
    return a >= b;
  }

  // ---- logic ----
  static Bool logic_and(Bool a, Bool b) { return a && b; }
  static Bool logic_or(Bool a, Bool b) { return a || b; }
  static Bool logic_xor(Bool a, Bool b) { return a != b; }
  static Bool logic_not(Bool a) { return !a; }

  // ---- values ----
  static Float value_float(Float value) { return value; }
  static Int value_int(Int value) { return value; }
  static Bool value_bool(Bool value) { return value; }
  static Text value_text(Text value) { return value; }

  // ---- vectors ----
  struct Broken {
    Float x = 0, y = 0, z = 0;
  };
  static Vector vector_make(Float x, Float y, Float z) { return {static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(z)}; }
  static Broken vector_break(Vector v) { return {v.x, v.y, v.z}; }
  static Vector vector_add(Vector a, Vector b) { return a + b; }
  static Vector vector_sub(Vector a, Vector b) { return a - b; }
  static Vector vector_cross(Vector a, Vector b) { return cross(a, b); }
  static Vector vector_scale(Vector a, Float b) { return a * static_cast<f32>(b); }
  static Float vector_length(Vector a) { return length(a); }
  static Vector vector_normalize(Vector a) { return normalize(a, {0, 0, 0}); }
  static Float vector_distance(Vector a, Vector b) { return distance(a, b); }
  static Float vector_dot(Vector a, Vector b) { return dot(a, b); }
  static Vector vector_lerp(Vector a, Vector b, Float alpha) { return lerp(a, b, static_cast<f32>(alpha)); }

  // ---- text ----
  static Text text_append(Text a, Text b, Text c) { return a + b + c; }
  static Bool text_equal(Text a, Text b) { return a == b; }
  static Bool text_contains(Text a, Text b) { return a.find(b) != std::string::npos; }
  static Int text_length(Text a) { return static_cast<i64>(a.size()); }
  /// The byte at `index` as a number 0..255 (beyond the text: 0 - as std::string has a 0 after its last).
  static Int text_code_at(Text a, Int index) { return index >= 0 && static_cast<u64>(index) < a.size() ? static_cast<i64>(static_cast<u8>(a[static_cast<std::size_t>(index)])) : 0; }
  /// A text of one byte: the code cut to 8 bits, as a char holds it.
  static Text text_of_code(Int code) { return Text(1, static_cast<char>(static_cast<u8>(code))); }
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
inline int switch_text(std::string_view cases, std::string_view selection) {
  const std::vector<std::string> list = setting_cases(cases);
  for (std::size_t i = 0; i < list.size(); ++i) if (list[i] == selection) return static_cast<int>(i);
  return static_cast<int>(list.size());
}
}  // namespace flow

}  // namespace logic

namespace logic::detail {
/// The conversions of Value::converted, type by type (only the pairs a wire can make).
template <class T> struct To;
template <> struct To<bool> {
  static bool from(bool v) { return v; }
  static bool from(i64 v) { return v != 0; }
  static bool from(f64 v) { return v != 0; }
  static bool from(ActorHandle v) { return v.id != 0; }
};
template <> struct To<i64> {
  static i64 from(bool v) { return v ? 1 : 0; }
  static i64 from(i64 v) { return v; }
  static i64 from(f64 v) { return whole(v); }
};
template <> struct To<Whole> : To<i64> {};
template <> struct To<f64> {
  static f64 from(bool v) { return v ? 1 : 0; }
  static f64 from(i64 v) { return static_cast<f64>(v); }
  static f64 from(f64 v) { return v; }
};
template <> struct To<Vec3> {
  static Vec3 from(Vec3 v) { return v; }
  static Vec3 from(f64 v) {
    const f32 x = static_cast<f32>(v);
    return {x, x, x};
  }
  static Vec3 from(i64 v) { return from(static_cast<f64>(v)); }
};
template <> struct To<std::string> {
  template <class S> static std::string from(const S& v) { return to_value(v).as_text(); }  // (everything prints, as the interpreter prints it)
};
template <> struct To<Text> {
  static Text from(const Text& v) { return v; }
  template <class S> static Text from(const S& v) { return Text(to_value(v).as_text()); }  // (everything prints, as the interpreter prints it)
};
template <> struct To<ActorHandle> {
  static ActorHandle from(ActorHandle v) { return v; }
};
}  // namespace logic::detail

/// What a wire does when its ends have different types: kern::to<Float>(index). The same
/// conversions the interpreter makes (Value::converted).
template <class T, class S> T to(const S& v) { return logic::detail::To<T>::from(v); }

}  // namespace kern
