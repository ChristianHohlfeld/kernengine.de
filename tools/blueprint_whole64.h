// Whole numbers of 64 bit for Blueprints - part of tools/cpp_to_blueprint.py.
//
// A Blueprint's whole number is exact up to 2^53 (it is a double underneath). A 64-bit number
// that is a bit pattern - a key made of two 32-bit numbers, a hash, the state of a random number
// generator - does not fit: in a Blueprint it is the struct Whole64, its upper and its lower 32
// bits, and what C++ does with such a number these functions do with the struct. They are C++ of
// the kind the converter converts (32-bit unsigned arithmetic only): a Blueprint that computes
// with a 64-bit number gets the functions it uses, converted like its own, and their tests compare
// them bit for bit with this source like every other function.
//
// A number the program spells u64, i64, uint64_t or int64_t is such a struct. One it spells
// size_t, long ... is a number it counts with: a Blueprint's whole number (see the converter).
#pragma once

#include <cmath>

namespace w64 {

using u32 = unsigned int;
using i32 = int;

struct Whole64 {
  u32 high = 0;
  u32 low = 0;
};

inline Whole64 w64_make(u32 high, u32 low) { return {high, low}; }

// ---- to and from a Blueprint's own numbers ----
/// A whole number a Blueprint holds (within 2^53 of zero) as 64 bits, a negative one in two's complement.
inline Whole64 w64_from_number(double x) {
  const double high = std::floor(x / 4294967296.0);
  const double low = x - high * 4294967296.0;
  return {static_cast<u32>(high < 0.0 ? high + 4294967296.0 : high), static_cast<u32>(low)};
}
/// 64 bits as a number, read without a sign (the nearest number where it has more than 53 bits).
inline double w64_to_number(Whole64 a) { return static_cast<double>(a.high) * 4294967296.0 + static_cast<double>(a.low); }
/// 64 bits as a number, read with a sign.
inline double w64_to_signed_number(Whole64 a) {
  return a.high >= 2147483648u ? (static_cast<double>(a.high) - 4294967296.0) * 4294967296.0 + static_cast<double>(a.low) : w64_to_number(a);
}
inline bool w64_is_zero(Whole64 a) { return a.high == 0 && a.low == 0; }

// ---- bits ----
inline Whole64 w64_bit_and(Whole64 a, Whole64 b) { return {a.high & b.high, a.low & b.low}; }
inline Whole64 w64_bit_or(Whole64 a, Whole64 b) { return {a.high | b.high, a.low | b.low}; }
inline Whole64 w64_bit_xor(Whole64 a, Whole64 b) { return {a.high ^ b.high, a.low ^ b.low}; }
inline Whole64 w64_bit_not(Whole64 a) { return {~a.high, ~a.low}; }
/// Shifted up by 0 to 63 bits (C++ says nothing for more: 0 here).
inline Whole64 w64_shift_left(Whole64 a, u32 by) {
  if (by == 0) return a;
  if (by >= 64) return {0, 0};
  if (by >= 32) return {a.low << (by - 32), 0};
  return {(a.high << by) | (a.low >> (32 - by)), a.low << by};
}
/// Shifted down, zeros coming in (an unsigned number).
inline Whole64 w64_shift_right(Whole64 a, u32 by) {
  if (by == 0) return a;
  if (by >= 64) return {0, 0};
  if (by >= 32) return {0, a.high >> (by - 32)};
  return {a.high >> by, (a.low >> by) | (a.high << (32 - by))};
}
/// Shifted down, the sign coming in (a signed number).
inline Whole64 w64_shift_right_signed(Whole64 a, u32 by) {
  const u32 fill = a.high >= 2147483648u ? 4294967295u : 0u;
  if (by == 0) return a;
  if (by >= 64) return {fill, fill};
  if (by >= 32) return {fill, by == 32 ? a.high : (a.high >> (by - 32)) | (fill << (64 - by))};
  return {(a.high >> by) | (fill << (32 - by)), (a.low >> by) | (a.high << (32 - by))};
}

// ---- arithmetic (around 2^64, as C++ computes with unsigned numbers) ----
inline Whole64 w64_add(Whole64 a, Whole64 b) {
  const u32 low = a.low + b.low;
  return {a.high + b.high + (low < a.low ? 1u : 0u), low};
}
inline Whole64 w64_subtract(Whole64 a, Whole64 b) {
  const u32 low = a.low - b.low;
  return {a.high - b.high - (a.low < b.low ? 1u : 0u), low};
}
inline Whole64 w64_negate(Whole64 a) { return w64_subtract({0, 0}, a); }
/// The upper 32 bits of the product of two 32-bit numbers (the lower 32 are a * b).
inline u32 w64_multiply_high(u32 a, u32 b) {
  const u32 a0 = a & 65535u, a1 = a >> 16, b0 = b & 65535u, b1 = b >> 16;
  const u32 low = a0 * b0, mid1 = a1 * b0, mid2 = a0 * b1;
  const u32 carry = ((low >> 16) + (mid1 & 65535u) + (mid2 & 65535u)) >> 16;
  return a1 * b1 + (mid1 >> 16) + (mid2 >> 16) + carry;
}
inline Whole64 w64_multiply(Whole64 a, Whole64 b) { return {w64_multiply_high(a.low, b.low) + a.low * b.high + a.high * b.low, a.low * b.low}; }

// ---- which is less ----
inline bool w64_equal(Whole64 a, Whole64 b) { return a.high == b.high && a.low == b.low; }
inline bool w64_less(Whole64 a, Whole64 b) { return a.high != b.high ? a.high < b.high : a.low < b.low; }
inline bool w64_less_signed(Whole64 a, Whole64 b) {
  const u32 x = a.high ^ 2147483648u, y = b.high ^ 2147483648u;
  return x != y ? x < y : a.low < b.low;
}

// ---- division (unsigned; by zero: C++ says nothing, 0 here) ----
struct Whole64Divided {
  Whole64 quotient;
  Whole64 remainder;
};
inline Whole64Divided w64_divide_both(Whole64 a, Whole64 b) {
  Whole64Divided out;
  if (w64_is_zero(b)) return out;
  for (i32 bit = 63; bit >= 0; --bit) {
    out.remainder = w64_shift_left(out.remainder, 1);
    const u32 taken = bit >= 32 ? (a.high >> static_cast<u32>(bit - 32)) & 1u : (a.low >> static_cast<u32>(bit)) & 1u;
    out.remainder.low = out.remainder.low | taken;
    if (!w64_less(out.remainder, b)) {
      out.remainder = w64_subtract(out.remainder, b);
      if (bit >= 32) {
        out.quotient.high = out.quotient.high | (1u << static_cast<u32>(bit - 32));
      } else {
        out.quotient.low = out.quotient.low | (1u << static_cast<u32>(bit));
      }
    }
  }
  return out;
}
inline Whole64 w64_divide(Whole64 a, Whole64 b) { return w64_divide_both(a, b).quotient; }
inline Whole64 w64_remainder(Whole64 a, Whole64 b) { return w64_divide_both(a, b).remainder; }
/// Signed division and its remainder (toward zero, as C++ divides).
inline Whole64 w64_divide_signed(Whole64 a, Whole64 b) {
  const bool minus_a = a.high >= 2147483648u, minus_b = b.high >= 2147483648u;
  const Whole64 q = w64_divide(minus_a ? w64_negate(a) : a, minus_b ? w64_negate(b) : b);
  return minus_a != minus_b ? w64_negate(q) : q;
}
inline Whole64 w64_remainder_signed(Whole64 a, Whole64 b) {
  const bool minus_a = a.high >= 2147483648u, minus_b = b.high >= 2147483648u;
  const Whole64 r = w64_remainder(minus_a ? w64_negate(a) : a, minus_b ? w64_negate(b) : b);
  return minus_a ? w64_negate(r) : r;
}

}  // namespace w64
