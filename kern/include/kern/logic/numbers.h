// The numbers of a Blueprint: the number types of the C++ subset, and what every operation on them gives.
//
// A Blueprint has the number types of C++ itself - bool, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64 - and
// each is a type of its own on a wire. An operation computes in its type, as C++ computes it: f32 + f32 rounds
// to f32 (every intermediate value, so a*b + c is not the double a*b + c rounded once), u32 * u32 goes round at
// 2^32. Nothing on a wire turns one type into another by itself: a conversion is a node of its own, one for each
// conversion C++ knows (IntegralCast, FloatingCast, IntegralToFloating, FloatingToIntegral, Promotion).
//
// This header is the one implementation of all of it: the interpreter's nodes, a Blueprint's C++ class and the
// JIT (where it does not compute a node itself) call these functions, so every tier gives the same bits.
//
// Where C++ leaves the result open (undefined behaviour), the functions here give one fixed result, the same on
// every platform - and the converter reports such a place of a source as an error, so a Blueprint made from a
// source never depends on it:
//   - whole numbers: +, -, * go round in two's complement at their width (a signed overflow is UB in C++);
//     unary - and abs of the smallest value give it itself;
//   - / and % by 0 give 0; the smallest signed value / -1 gives it itself, % -1 gives 0; otherwise toward zero,
//     as C++;
//   - a shift goes by the amount's low bits (amount mod the width, as x86 and WebAssembly do; C++: UB from the
//     width on, and for a negative amount); >> of a signed number brings the sign in (C++20);
//   - a floating number to a whole number: toward zero; not a number: 0; beyond the type's ends: the nearer end
//     (WebAssembly's trunc_sat; C++: UB);
//   - floating numbers are IEEE 754, as C++ has them: x / 0 is +-inf (0 / 0 NaN), sqrt of a negative NaN.
//     Which NaN an operation gives (its sign and payload bits) neither C++ nor WebAssembly fix; a NaN stays a NaN.
//     fmin/fmax of 0 and -0: -0 and 0 (C leaves it open).
// The rest is C++'s own: a whole number to a narrower one keeps its low bits (C++20), a number to bool is != 0,
// bool to a number 0 or 1, a whole number to a floating one the nearest, f64 to f32 the nearest (beyond f32's
// range: +-inf, IEEE). sin, exp, pow ... are the engine's own (kern::dm, musl: the same bits natively and in
// WebAssembly), sqrt, floor, fmod ... are exact in IEEE arithmetic.
#pragma once

#include <bit>
#include <cmath>
#include <concepts>
#include <limits>
#include <string_view>
#include <type_traits>

#include "kern/core/base.h"
#include "kern/core/dmath.h"

namespace kern::logic::num {

/// The number types, in this order everywhere (PinType has them in it).
enum class Kind : u8 { boolean, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64, count };
inline constexpr std::string_view kKindNames[] = {"bool", "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64", "f32", "f64"};
static_assert(sizeof kKindNames / sizeof kKindNames[0] == static_cast<std::size_t>(Kind::count));

template <class T> inline constexpr bool is_number = false;
template <> inline constexpr bool is_number<bool> = true;
template <> inline constexpr bool is_number<i8> = true;
template <> inline constexpr bool is_number<i16> = true;
template <> inline constexpr bool is_number<i32> = true;
template <> inline constexpr bool is_number<i64> = true;
template <> inline constexpr bool is_number<u8> = true;
template <> inline constexpr bool is_number<u16> = true;
template <> inline constexpr bool is_number<u32> = true;
template <> inline constexpr bool is_number<u64> = true;
template <> inline constexpr bool is_number<f32> = true;
template <> inline constexpr bool is_number<f64> = true;
/// A number type of the language (bool included).
template <class T> concept Number = is_number<T>;
/// A whole number type (not bool).
template <class T> concept Integer = Number<T> && std::is_integral_v<T> && !std::same_as<T, bool>;
template <class T> concept Floating = std::same_as<T, f32> || std::same_as<T, f64>;
/// What arithmetic takes: a whole or a floating number (bool has the nodes of logic: and, or, not).
template <class T> concept Arithmetic = Integer<T> || Floating<T>;

template <Number T> constexpr Kind kind_of() {
  if constexpr (std::same_as<T, bool>) return Kind::boolean;
  else if constexpr (std::same_as<T, i8>) return Kind::i8;
  else if constexpr (std::same_as<T, i16>) return Kind::i16;
  else if constexpr (std::same_as<T, i32>) return Kind::i32;
  else if constexpr (std::same_as<T, i64>) return Kind::i64;
  else if constexpr (std::same_as<T, u8>) return Kind::u8;
  else if constexpr (std::same_as<T, u16>) return Kind::u16;
  else if constexpr (std::same_as<T, u32>) return Kind::u32;
  else if constexpr (std::same_as<T, u64>) return Kind::u64;
  else if constexpr (std::same_as<T, f32>) return Kind::f32;
  else return Kind::f64;
}
constexpr std::string_view kind_name(Kind k) { return k < Kind::count ? kKindNames[static_cast<u8>(k)] : std::string_view("?"); }
constexpr bool kind_from_name(std::string_view name, Kind& out) {
  for (u8 k = 0; k < static_cast<u8>(Kind::count); ++k)
    if (name == kKindNames[k]) return out = static_cast<Kind>(k), true;
  return false;
}
constexpr bool is_integer(Kind k) { return k >= Kind::i8 && k <= Kind::u64; }
constexpr bool is_signed(Kind k) { return (k >= Kind::i8 && k <= Kind::i64) || k == Kind::f32 || k == Kind::f64; }
constexpr bool is_floating(Kind k) { return k == Kind::f32 || k == Kind::f64; }
/// How many bits a value of the type has (bool: 1).
constexpr int width(Kind k) {
  constexpr int kWidth[] = {1, 8, 16, 32, 64, 8, 16, 32, 64, 32, 64};
  return k < Kind::count ? kWidth[static_cast<u8>(k)] : 0;
}

/// Calls f(std::type_identity<T>{}) with the C++ type of the kind (a kind beyond them: f64's).
template <class F> constexpr decltype(auto) visit(Kind k, F&& f) {
  switch (k) {
    case Kind::boolean: return f(std::type_identity<bool>{});
    case Kind::i8: return f(std::type_identity<i8>{});
    case Kind::i16: return f(std::type_identity<i16>{});
    case Kind::i32: return f(std::type_identity<i32>{});
    case Kind::i64: return f(std::type_identity<i64>{});
    case Kind::u8: return f(std::type_identity<u8>{});
    case Kind::u16: return f(std::type_identity<u16>{});
    case Kind::u32: return f(std::type_identity<u32>{});
    case Kind::u64: return f(std::type_identity<u64>{});
    case Kind::f32: return f(std::type_identity<f32>{});
    default: return f(std::type_identity<f64>{});
  }
}

// ---- a value's bits (how a Value, the interpreter's slots and the JIT hold a number) ----
// A whole number: its value as 64 bits (a signed one sign-extended, an unsigned one with zeros above); bool: 0 or 1;
// f32: its 32 bits (IEEE 754) with zeros above; f64: its 64 bits. No detour through another type: every value and
// every NaN keeps its bits.
template <Number T> constexpr u64 bits_of(T x) {
  if constexpr (std::same_as<T, bool>) return x ? 1u : 0u;
  else if constexpr (std::same_as<T, f32>) return std::bit_cast<u32>(x);
  else if constexpr (std::same_as<T, f64>) return std::bit_cast<u64>(x);
  else if constexpr (std::is_signed_v<T>) return static_cast<u64>(static_cast<i64>(x));
  else return static_cast<u64>(x);
}
template <Number T> constexpr T of_bits(u64 b) {
  if constexpr (std::same_as<T, bool>) return (b & 1u) != 0;
  else if constexpr (std::same_as<T, f32>) return std::bit_cast<f32>(static_cast<u32>(b));
  else if constexpr (std::same_as<T, f64>) return std::bit_cast<f64>(b);
  else return static_cast<T>(b);  // (the low bits: two's complement, C++20)
}
/// The bits a value of the kind has when they are given as any 64 bits: the low bits of the type, held as bits_of holds them.
constexpr u64 held_bits(Kind k, u64 b) {
  return visit(k, [b](auto t) { return bits_of(of_bits<typename decltype(t)::type>(b)); });
}

// ---- conversions: one for each conversion C++ knows ----
/// What a conversion is, as clang names the ImplicitCastExpr (and a static_cast does the same).
enum class Cast : u8 { none, integral, floating, integral_to_floating, floating_to_integral, integral_to_boolean, floating_to_boolean, boolean_to_integral, boolean_to_floating };
constexpr Cast cast_kind(Kind from, Kind to) {
  if (from == to) return Cast::none;
  if (to == Kind::boolean) return is_floating(from) ? Cast::floating_to_boolean : Cast::integral_to_boolean;
  if (from == Kind::boolean) return is_floating(to) ? Cast::boolean_to_floating : Cast::boolean_to_integral;
  if (is_floating(from)) return is_floating(to) ? Cast::floating : Cast::floating_to_integral;
  return is_floating(to) ? Cast::integral_to_floating : Cast::integral;
}
/// The name clang gives the conversion (IntegralCast, FloatingCast, IntegralToFloating, FloatingToIntegral,
/// IntegralToBoolean, FloatingToBoolean; bool to a number is an IntegralCast / IntegralToFloating there).
constexpr std::string_view cast_name(Cast c) {
  constexpr std::string_view kNames[] = {"NoOp", "IntegralCast", "FloatingCast", "IntegralToFloating", "FloatingToIntegral", "IntegralToBoolean", "FloatingToBoolean", "IntegralCast", "IntegralToFloating"};
  return kNames[static_cast<u8>(c)];
}
/// Is the conversion a promotion of C++ (a whole number narrower than int to int - i32 -, f32 to f64 where C++
/// promotes it: a variadic argument)? It is the same conversion; the converter names it so.
constexpr bool is_promotion(Kind from, Kind to) {
  return (to == Kind::i32 && (from == Kind::boolean || from == Kind::i8 || from == Kind::i16 || from == Kind::u8 || from == Kind::u16)) || (from == Kind::f32 && to == Kind::f64);
}

/// A floating number toward zero as the whole number To: not a number 0, beyond To's ends the nearer end.
template <Integer To, Floating F> constexpr To floating_to_integral(F x) {
  constexpr F hi = static_cast<F>(std::numeric_limits<To>::max() / 2 + 1) * F(2);  // (2^digits: exact in F)
  if (!(x == x)) return 0;
  if (x >= hi) return std::numeric_limits<To>::max();
  if constexpr (std::is_signed_v<To>) {
    if (x <= -hi) return std::numeric_limits<To>::min();
  } else {
    if (x <= F(-1)) return 0;
  }
  return static_cast<To>(x);
}
/// The number x as the type To, as static_cast<To>(x) gives it in C++ - with the one result fixed where C++ has
/// none (a floating number beyond a whole type's ends, see above).
template <Number To, Number From> constexpr To cast(From x) {
  if constexpr (std::same_as<To, From>) return x;
  else if constexpr (std::same_as<To, bool>) return x != From(0);
  else if constexpr (Floating<From> && Integer<To>) return floating_to_integral<To>(x);
  else return static_cast<To>(x);
}
// ---- the one NaN: where the bits of a floating number are seen ----
// Which NaN an operation gives - its sign, its payload - IEEE 754 and C++ leave open (x86 gives a negative one for 0 * inf,
// and a compiler may swap the operands of a + b, so the NaN of the other one comes through). The language fixes it as it
// fixes a division by 0: a NaN is "a NaN" while it is computed with (no step pays for it), and wherever its bits are SEEN -
// Bit Cast, the bytes of a struct or a list (Struct to Bytes, Copy Bytes), a number as a text, saving and loading, a
// comparison of results - it is the one quiet NaN: 0x7fc00000 (f32), 0x7ff8000000000000 (f64). The same in every tier.
template <Floating T> constexpr T seen(T x) {
  if (x == x) return x;
  if constexpr (std::same_as<T, f32>) return std::bit_cast<f32>(u32{0x7fc00000u});
  else return std::bit_cast<f64>(u64{0x7ff8000000000000ull});
}
/// The bits of x as they are seen: bits_of, a NaN the one NaN of its type.
template <Number T> constexpr u64 seen_bits(T x) {
  if constexpr (Floating<T>) return bits_of(seen(x));
  else return bits_of(x);
}
/// Are a and b the same number as the language sees them: the same bits - two NaNs the same (the one NaN), -0 not 0.
template <Number T> constexpr bool same_seen(T a, T b) { return seen_bits(a) == seen_bits(b); }

/// The bits of x as the type To of the same size (std::bit_cast: f32 <-> u32 / i32, f64 <-> u64 / i64) - of a NaN the one
/// NaN's (seen, above).
template <Number To, Number From>
  requires(sizeof(To) == sizeof(From) && !std::same_as<To, bool> && !std::same_as<From, bool>)
constexpr To reinterpret(From x) {
  if constexpr (Floating<From>) return std::bit_cast<To>(seen(x));
  else return std::bit_cast<To>(x);
}

// ---- arithmetic ----
namespace detail {
template <Integer T> constexpr T wrapped(u64 x) { return static_cast<T>(x); }  // (the low bits, C++20)
template <Integer T> constexpr u64 wide(T x) { return static_cast<u64>(x); }   // (congruent mod 2^width)
template <Integer T> constexpr int shift_of(u64 n) { return static_cast<int>(n & static_cast<u64>(sizeof(T) * 8 - 1)); }
}  // namespace detail

template <Arithmetic T> constexpr T add(T a, T b) {
  if constexpr (Floating<T>) return a + b;
  else return detail::wrapped<T>(detail::wide(a) + detail::wide(b));
}
template <Arithmetic T> constexpr T sub(T a, T b) {
  if constexpr (Floating<T>) return a - b;
  else return detail::wrapped<T>(detail::wide(a) - detail::wide(b));
}
template <Arithmetic T> constexpr T mul(T a, T b) {
  if constexpr (Floating<T>) return a * b;
  else return detail::wrapped<T>(detail::wide(a) * detail::wide(b));
}
template <Arithmetic T> constexpr T div(T a, T b) {
  if constexpr (Floating<T>) {
    return a / b;
  } else {
    if (b == 0) return 0;
    if constexpr (std::is_signed_v<T>)
      if (b == T(-1)) return detail::wrapped<T>(0 - detail::wide(a));  // (the smallest by -1: itself)
    return static_cast<T>(a / b);
  }
}
/// a % b of whole numbers (toward zero, the sign of a); of floating numbers std::fmod (exact).
template <Arithmetic T> constexpr T rem(T a, T b) {
  if constexpr (Floating<T>) {
    return std::fmod(a, b);
  } else {
    if (b == 0) return 0;
    if constexpr (std::is_signed_v<T>)
      if (b == T(-1)) return 0;
    return static_cast<T>(a % b);
  }
}
template <Arithmetic T> constexpr T neg(T a) {
  if constexpr (Floating<T>) return -a;
  else return detail::wrapped<T>(0 - detail::wide(a));
}
/// |a|: std::abs of a whole number (of the smallest signed one: itself), std::fabs of a floating one.
template <Arithmetic T> constexpr T abs(T a) {
  if constexpr (Floating<T>) return std::fabs(a);
  else if constexpr (std::is_signed_v<T>) return a < 0 ? neg(a) : a;
  else return a;
}
/// std::min and std::max: (b < a) ? b : a, (a < b) ? b : a - a NaN in a stays, one in b is not taken.
template <Arithmetic T> constexpr T min(T a, T b) { return b < a ? b : a; }
template <Arithmetic T> constexpr T max(T a, T b) { return a < b ? b : a; }

/// Value between lo and hi (the two in either order): max(min(value, hi), lo) - the Clamp node, the formula's clamp.
template <Arithmetic T> constexpr T clamp(T value, T lo, T hi) { return min(max(value, min(lo, hi)), max(lo, hi)); }
/// a + (b - a) * t, each step in T - the Lerp node, the formula's lerp.
template <Floating T> constexpr T lerp(T a, T b, T t) { return add(a, mul(sub(b, a), t)); }

// ---- comparisons (of one type, as C++ compares them; a NaN is unequal to everything, itself included) ----
template <Number T> constexpr bool eq(T a, T b) { return a == b; }
template <Number T> constexpr bool ne(T a, T b) { return a != b; }
template <Arithmetic T> constexpr bool lt(T a, T b) { return a < b; }
template <Arithmetic T> constexpr bool le(T a, T b) { return a <= b; }
template <Arithmetic T> constexpr bool gt(T a, T b) { return a > b; }
template <Arithmetic T> constexpr bool ge(T a, T b) { return a >= b; }

// ---- bits (whole numbers) ----
template <Integer T> constexpr T bit_and(T a, T b) { return static_cast<T>(a & b); }
template <Integer T> constexpr T bit_or(T a, T b) { return static_cast<T>(a | b); }
template <Integer T> constexpr T bit_xor(T a, T b) { return static_cast<T>(a ^ b); }
template <Integer T> constexpr T bit_not(T a) { return static_cast<T>(~a); }
/// a << n: the bits of a moved n to the left, n taken mod the width of T (the amount may be of any whole type).
template <Integer T, Integer N> constexpr T shl(T a, N n) { return detail::wrapped<T>(detail::wide(a) << detail::shift_of<T>(static_cast<u64>(n))); }
/// a >> n: a signed a brings its sign in (arithmetic), an unsigned one zeros; n mod the width of T.
template <Integer T, Integer N> constexpr T shr(T a, N n) {
  const int s = detail::shift_of<T>(static_cast<u64>(n));
  if constexpr (std::is_signed_v<T>) return static_cast<T>(static_cast<i64>(a) >> s);
  else return static_cast<T>(static_cast<u64>(a) >> s);
}
template <Integer T> constexpr T rotl(T a, i64 n) { return static_cast<T>(std::rotl(static_cast<std::make_unsigned_t<T>>(a), static_cast<int>(n & (sizeof(T) * 8 - 1)))); }
template <Integer T> constexpr T rotr(T a, i64 n) { return static_cast<T>(std::rotr(static_cast<std::make_unsigned_t<T>>(a), static_cast<int>(n & (sizeof(T) * 8 - 1)))); }
template <Integer T> constexpr i32 popcount(T a) { return std::popcount(static_cast<std::make_unsigned_t<T>>(a)); }
template <Integer T> constexpr i32 countl_zero(T a) { return std::countl_zero(static_cast<std::make_unsigned_t<T>>(a)); }
template <Integer T> constexpr i32 countr_zero(T a) { return std::countr_zero(static_cast<std::make_unsigned_t<T>>(a)); }

// ---- the functions of <cmath> on floating numbers: the engine's own (kern::dm) or exact in IEEE ----
template <Floating T> inline T sqrt(T a) { return std::sqrt(a); }
template <Floating T> inline T floor(T a) { return std::floor(a); }
template <Floating T> inline T ceil(T a) { return std::ceil(a); }
template <Floating T> inline T trunc(T a) { return std::trunc(a); }
template <Floating T> inline T round(T a) { return std::round(a); }  // (half away from zero)
template <Floating T> inline T fabs(T a) { return std::fabs(a); }
template <Floating T> inline T copysign(T a, T b) { return std::copysign(a, b); }
template <Floating T> inline T fmod(T a, T b) { return std::fmod(a, b); }
/// std::fmin / std::fmax: a NaN on one side gives the other. Of 0 and -0 C leaves open which it gives (and two calls of
/// std::fmin in one program give different ones - the builtin and the library): here fmin gives -0, fmax 0, as IEEE
/// 754-2019's minimumNumber and maximumNumber do.
template <Floating T> inline T fmin(T a, T b) {
  if (a != a) return b;
  if (b != b) return a;
  if (a == b) return std::signbit(a) ? a : b;
  return a < b ? a : b;
}
template <Floating T> inline T fmax(T a, T b) {
  if (a != a) return b;
  if (b != b) return a;
  if (a == b) return std::signbit(a) ? b : a;
  return a < b ? b : a;
}
template <Floating T> inline T sin(T a) { return dm::sin(a); }
template <Floating T> inline T cos(T a) { return dm::cos(a); }
template <Floating T> inline T tan(T a) { return dm::tan(a); }
template <Floating T> inline T asin(T a) { return dm::asin(a); }
template <Floating T> inline T acos(T a) { return dm::acos(a); }
template <Floating T> inline T atan(T a) { return dm::atan(a); }
template <Floating T> inline T atan2(T y, T x) { return dm::atan2(y, x); }
template <Floating T> inline T exp(T a) { return dm::exp(a); }
template <Floating T> inline T expm1(T a) { return dm::expm1(a); }
/// sinh, cosh, tanh, asinh: the engine's own (kern/src/math/hyperbolic.c, kern::dm), the f-variant for f32.
template <Floating T> inline T sinh(T a) { return dm::sinh(a); }
template <Floating T> inline T cosh(T a) { return dm::cosh(a); }
template <Floating T> inline T tanh(T a) { return dm::tanh(a); }
template <Floating T> inline T asinh(T a) { return dm::asinh(a); }
template <Floating T> inline T log(T a) { return dm::log(a); }
template <Floating T> inline T log10(T a) { return dm::log10(a); }
template <Floating T> inline T pow(T a, T b) { return dm::pow(a, b); }
template <Floating T> inline T hypot(T a, T b) { return dm::hypot(a, b); }
template <Floating T> inline bool isnan(T a) { return std::isnan(a); }
template <Floating T> inline bool isinf(T a) { return std::isinf(a); }
template <Floating T> inline bool isfinite(T a) { return std::isfinite(a); }
template <Floating T> inline bool signbit(T a) { return std::signbit(a); }

// ---- a place of the language in C++: a number of one type whose operators are the functions above ----
/// What a number of the language is in a Blueprint's C++ class: a variable, a field of a struct, a local variable, an
/// input or output of a function, an entry of a list, an input of a Formula node (kern::formula, blueprint.h) - the
/// class names them i64, f32 ... (the names of the language in the class's scope: engine_nodes.gen.h). Its operators
/// and functions are those of this header: Count += x on a Typed<i64> is add(Count, x), and goes round at 2^64 as the
/// Add node does; a * b + 1u on Typed<u32> is add(mul(a, b), 1). C++'s own operators would promote a u8, and leave a
/// signed overflow, a division by 0 or a shift by the width open - none of them is ever applied to a place: an
/// operator of a Typed and something it does not take (a Typed of another type, a floating number of another type)
/// is refused by the compiler, as the formula refuses it (num_cast<To, From> says the conversion). A whole number
/// written out takes the type of what it meets (7 with a u8 is a u8); a floating one is its own type (0.5f an f32,
/// 0.5 an f64). Where a function of C++ takes the number itself (the engine's, a node's), the Typed gives it - as
/// that very type, never as another (that would be a conversion: a Typed<i32> is no f64).
template <Number T> struct Typed;
namespace detail {
template <class X> struct raw_of { using type = X; };
template <class X> struct raw_of<Typed<X>> { using type = X; };
template <class X> inline constexpr bool is_typed = false;
template <class X> inline constexpr bool is_typed<Typed<X>> = true;
/// What a Typed<T> takes as a T without a conversion: a T, and - for a whole number - a whole number written out.
template <class T, class L> concept Takes = std::same_as<L, T> || (Integer<T> && std::is_integral_v<L> && !std::same_as<L, bool>);
}  // namespace detail
/// The number type of a place (i64 of a Typed<i64>), or the type itself.
template <class X> using raw = typename detail::raw_of<std::remove_cvref_t<X>>::type;
template <class X> inline constexpr bool is_typed = detail::is_typed<std::remove_cvref_t<X>>;
/// The number of a place, or the number itself.
template <class X> constexpr raw<X> raw_value(const X& x) {
  if constexpr (is_typed<X>) return x.v;
  else return x;
}

/// A signature of a function with the numbers themselves (a Callback the engine calls: f64(f64, f64) of the class's places).
template <class S> struct raw_signature;
template <class R, class... A> struct raw_signature<R(A...)> { using type = raw<R>(raw<A>...); };

template <Number T> struct Typed {
  T v{};
  constexpr Typed() = default;
  template <class L>
    requires detail::Takes<T, L>
  constexpr Typed(L x) : v(static_cast<T>(x)) {}  // NOLINT (implicit: a number of the type, or one written out)
  /// The number itself, to a function of C++ that takes it - as T, never as another type.
  template <class U>
    requires std::same_as<U, T>
  constexpr operator U() const { return v; }  // NOLINT

  template <class B> requires detail::Takes<T, raw<B>> && (!is_typed<B> || std::same_as<raw<B>, T>) && Arithmetic<T>
  constexpr Typed& operator+=(const B& b) { v = add(v, static_cast<T>(raw_value(b))); return *this; }
  template <class B> requires detail::Takes<T, raw<B>> && (!is_typed<B> || std::same_as<raw<B>, T>) && Arithmetic<T>
  constexpr Typed& operator-=(const B& b) { v = sub(v, static_cast<T>(raw_value(b))); return *this; }
  template <class B> requires detail::Takes<T, raw<B>> && (!is_typed<B> || std::same_as<raw<B>, T>) && Arithmetic<T>
  constexpr Typed& operator*=(const B& b) { v = mul(v, static_cast<T>(raw_value(b))); return *this; }
  template <class B> requires detail::Takes<T, raw<B>> && (!is_typed<B> || std::same_as<raw<B>, T>) && Arithmetic<T>
  constexpr Typed& operator/=(const B& b) { v = div(v, static_cast<T>(raw_value(b))); return *this; }

  friend constexpr Typed operator-(Typed a) requires Arithmetic<T> { return neg(a.v); }
  friend constexpr Typed operator+(Typed a) requires Arithmetic<T> { return a; }
  friend constexpr Typed operator~(Typed a) requires Integer<T> { return bit_not(a.v); }
  friend constexpr Typed operator!(Typed a) requires std::same_as<T, bool> { return !a.v; }

  // (the formula's functions of one number: found by their argument - sqrt(a) of a Typed<f32>; of two and three below)
  friend constexpr Typed abs(Typed a) requires Arithmetic<T> { return num::abs(a.v); }
#define KERN_TYPED_1(name) friend Typed name(Typed a) requires Floating<T> { return num::name(a.v); }
#define KERN_TYPED_IS(name) friend Typed<bool> name(Typed a) requires Floating<T> { return num::name(a.v); }
  KERN_TYPED_1(sqrt) KERN_TYPED_1(floor) KERN_TYPED_1(ceil) KERN_TYPED_1(trunc) KERN_TYPED_1(round)
  KERN_TYPED_1(sin) KERN_TYPED_1(cos) KERN_TYPED_1(tan) KERN_TYPED_1(asin) KERN_TYPED_1(acos) KERN_TYPED_1(atan)
  KERN_TYPED_1(exp) KERN_TYPED_1(expm1) KERN_TYPED_1(sinh) KERN_TYPED_1(cosh) KERN_TYPED_1(tanh) KERN_TYPED_1(asinh) KERN_TYPED_1(log) KERN_TYPED_1(log10)
  KERN_TYPED_IS(isnan) KERN_TYPED_IS(isinf) KERN_TYPED_IS(isfinite) KERN_TYPED_IS(signbit)
#undef KERN_TYPED_1
#undef KERN_TYPED_IS
};
namespace detail {
/// The type two operands of an operator compute in: a Typed's, where the other is of it or a number it takes.
template <class A, class B> struct meet {};
template <class T, class B> requires(is_typed<B> ? std::same_as<raw<B>, T> : Takes<T, B>) struct meet<Typed<T>, B> { using type = T; };
template <class A, class T> requires(!is_typed<A> && Takes<T, A>) struct meet<A, Typed<T>> { using type = T; };
template <class A, class B> concept Meet = requires { typename meet<std::remove_cvref_t<A>, std::remove_cvref_t<B>>::type; };
template <class A, class B> using met = typename meet<std::remove_cvref_t<A>, std::remove_cvref_t<B>>::type;
template <class A, class B> concept Mismatch = (is_typed<A> || is_typed<B>) && !Meet<A, B>;
template <class L> using written = std::conditional_t<std::is_signed_v<L>, std::conditional_t<sizeof(L) <= 4, i32, i64>, std::conditional_t<sizeof(L) <= 4, u32, u64>>;
template <class L> concept Written = std::is_integral_v<L> && !std::same_as<L, bool>;
}  // namespace detail
// (the formula's functions of two and three numbers: a Typed and what it meets - min(a, 2.0) of a Typed<f64>)
namespace detail {
template <class A, class B, class C> struct meet3 {};
template <class A, class B, class C> requires Meet<A, B> && Meet<A, C> struct meet3<A, B, C> { using type = met<A, B>; };
template <class A, class B, class C> requires(!Meet<A, B> && Meet<B, C> && Takes<met<B, C>, std::remove_cvref_t<A>>) struct meet3<A, B, C> { using type = met<B, C>; };
template <class A, class B, class C> concept Meet3 = requires { typename meet3<A, B, C>::type; };
}  // namespace detail
#define KERN_TYPED_FN2(name, need)                                                                                    \
  template <class A, class B> requires detail::Meet<A, B> && need<detail::met<A, B>>                                 \
  constexpr Typed<detail::met<A, B>> name(const A& a, const B& b) {                                                  \
    using T = detail::met<A, B>;                                                                                     \
    return num::name(static_cast<T>(raw_value(a)), static_cast<T>(raw_value(b)));                                  \
  }
KERN_TYPED_FN2(min, Arithmetic) KERN_TYPED_FN2(max, Arithmetic)
KERN_TYPED_FN2(atan2, Floating) KERN_TYPED_FN2(pow, Floating) KERN_TYPED_FN2(hypot, Floating) KERN_TYPED_FN2(copysign, Floating)
KERN_TYPED_FN2(fmin, Floating) KERN_TYPED_FN2(fmax, Floating) KERN_TYPED_FN2(fmod, Floating)
#undef KERN_TYPED_FN2
template <class A, class B, class C> requires detail::Meet3<A, B, C> && Arithmetic<typename detail::meet3<A, B, C>::type>
constexpr auto clamp(const A& x, const B& lo, const C& hi) {
  using T = typename detail::meet3<A, B, C>::type;
  return Typed<T>(num::clamp(static_cast<T>(raw_value(x)), static_cast<T>(raw_value(lo)), static_cast<T>(raw_value(hi))));
}
template <class A, class B, class C> requires detail::Meet3<A, B, C> && Floating<typename detail::meet3<A, B, C>::type>
constexpr auto lerp(const A& a, const B& b, const C& t) {
  using T = typename detail::meet3<A, B, C>::type;
  return Typed<T>(num::lerp(static_cast<T>(raw_value(a)), static_cast<T>(raw_value(b)), static_cast<T>(raw_value(t))));
}
// (the operators of two operands: a Typed and one it meets - else no operator at all: not C++'s own either, which
// would convert and compute as C++ does, not as the language)
#define KERN_TYPED_OP(op, fn, need, R)                                                                              \
  template <class A, class B> requires detail::Meet<A, B> && need<detail::met<A, B>>                                 \
  constexpr R operator op(const A& a, const B& b) {                                                                  \
    using T = detail::met<A, B>;                                                                                     \
    return fn(static_cast<T>(raw_value(a)), static_cast<T>(raw_value(b)));                                          \
  }                                                                                                                  \
  template <class A, class B> requires detail::Mismatch<A, B> void operator op(const A&, const B&) = delete;
template <class T> concept IsBool = std::same_as<T, bool>;
#define KERN_R Typed<detail::met<A, B>>
KERN_TYPED_OP(+, add, Arithmetic, KERN_R)
KERN_TYPED_OP(-, sub, Arithmetic, KERN_R)
KERN_TYPED_OP(*, mul, Arithmetic, KERN_R)
KERN_TYPED_OP(/, div, Arithmetic, KERN_R)
KERN_TYPED_OP(%, rem, Integer, KERN_R)
KERN_TYPED_OP(&, bit_and, Integer, KERN_R)
KERN_TYPED_OP(|, bit_or, Integer, KERN_R)
KERN_TYPED_OP(^, bit_xor, Integer, KERN_R)
KERN_TYPED_OP(==, eq, Number, Typed<bool>)
KERN_TYPED_OP(!=, ne, Number, Typed<bool>)
KERN_TYPED_OP(<, lt, Arithmetic, Typed<bool>)
KERN_TYPED_OP(<=, le, Arithmetic, Typed<bool>)
KERN_TYPED_OP(>, gt, Arithmetic, Typed<bool>)
KERN_TYPED_OP(>=, ge, Arithmetic, Typed<bool>)
#undef KERN_R
#undef KERN_TYPED_OP
template <class A, class B> requires detail::Meet<A, B> && IsBool<detail::met<A, B>>
constexpr Typed<bool> operator&&(const A& a, const B& b) { return static_cast<bool>(raw_value(a)) && static_cast<bool>(raw_value(b)); }
template <class A, class B> requires detail::Meet<A, B> && IsBool<detail::met<A, B>>
constexpr Typed<bool> operator||(const A& a, const B& b) { return static_cast<bool>(raw_value(a)) || static_cast<bool>(raw_value(b)); }
template <class A, class B> requires detail::Mismatch<A, B> void operator&&(const A&, const B&) = delete;
template <class A, class B> requires detail::Mismatch<A, B> void operator||(const A&, const B&) = delete;
// (a shift: of the type of the value shifted, the amount a whole number of any type. The value is a Typed itself - or a
// whole number written out, which is then C++'s own type, an i32 for 1 in 1 << n, and takes no type from the amount)
template <Integer T, Integer N> constexpr Typed<T> operator<<(Typed<T> a, Typed<N> n) { return shl(a.v, n.v); }
template <Integer T, Integer N> constexpr Typed<T> operator>>(Typed<T> a, Typed<N> n) { return shr(a.v, n.v); }
template <Integer T, detail::Written L> constexpr Typed<T> operator<<(Typed<T> a, L n) { return shl(a.v, n); }
template <Integer T, detail::Written L> constexpr Typed<T> operator>>(Typed<T> a, L n) { return shr(a.v, n); }
template <detail::Written L, Integer N> constexpr Typed<detail::written<L>> operator<<(L a, Typed<N> n) { return shl(static_cast<detail::written<L>>(a), n.v); }
template <detail::Written L, Integer N> constexpr Typed<detail::written<L>> operator>>(L a, Typed<N> n) { return shr(static_cast<detail::written<L>>(a), n.v); }
template <class A, class B> requires(is_typed<A> || is_typed<B>) && (!Integer<raw<A>> || !Integer<raw<B>>) void operator<<(const A&, const B&) = delete;
template <class A, class B> requires(is_typed<A> || is_typed<B>) && (!Integer<raw<A>> || !Integer<raw<B>>) void operator>>(const A&, const B&) = delete;
template <Integer T, class B> requires detail::Meet<Typed<T>, B> constexpr Typed<T>& operator%=(Typed<T>& a, const B& b) { a.v = rem(a.v, static_cast<T>(raw_value(b))); return a; }
template <Integer T, class B> requires detail::Meet<Typed<T>, B> constexpr Typed<T>& operator&=(Typed<T>& a, const B& b) { a.v = bit_and(a.v, static_cast<T>(raw_value(b))); return a; }
template <Integer T, class B> requires detail::Meet<Typed<T>, B> constexpr Typed<T>& operator|=(Typed<T>& a, const B& b) { a.v = bit_or(a.v, static_cast<T>(raw_value(b))); return a; }
template <Integer T, class B> requires detail::Meet<Typed<T>, B> constexpr Typed<T>& operator^=(Typed<T>& a, const B& b) { a.v = bit_xor(a.v, static_cast<T>(raw_value(b))); return a; }
template <Integer T, class B> requires Integer<raw<B>> constexpr Typed<T>& operator<<=(Typed<T>& a, const B& n) { a.v = shl(a.v, raw_value(n)); return a; }
template <Integer T, class B> requires Integer<raw<B>> constexpr Typed<T>& operator>>=(Typed<T>& a, const B& n) { a.v = shr(a.v, raw_value(n)); return a; }

/// What a formula gives in C++: the number of a Typed, a number written out as it is.
template <class R> constexpr auto untyped(R r) {
  if constexpr (requires { r.v; }) return r.v;
  else return r;
}

}  // namespace kern::logic::num
