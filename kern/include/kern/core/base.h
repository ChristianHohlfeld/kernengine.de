// Kern engine - core definitions shared by every module.
//
// Kern is built without exceptions and without RTTI. Fallible operations
// return bool (or 0 / nullptr) and record a human readable message with
// fail(); hosts read it with last_error() (kern_last_error in the C ABI).
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

/// A type of the engine a Blueprint holds as a value: a struct or an enum of the engine's libraries
/// (kern/logic/engine_libraries.h; tools/engine_api.py). Only a type so marked is part of the engine
/// for Blueprints - a node that takes or gives another type of the engine is no node, and the engine's
/// other types (logic::Graph, game::Level ...) stay inside it. In front of the name:
/// struct KERN_BLUEPRINT_TYPE Pose { ... };  enum class KERN_BLUEPRINT_TYPE BodyType : u8 { ... };
#if defined(__clang__)
#define KERN_BLUEPRINT_TYPE [[clang::annotate("kern_blueprint_type")]]
#else
#define KERN_BLUEPRINT_TYPE
#endif

/// KERN_CLOCK before a function of the engine that reads a clock: what it gives changes by itself, from one
/// call to the next with nothing else changed. Its node says so (tools/engine_api.py: EngineFunction::clock) -
/// a test that plays a program twice does not expect it to give the same twice.
#define KERN_CLOCK

namespace kern {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;

/// Records the last error (printf-style). Always returns false so call sites
/// can write `return fail("...")`.
bool fail(const char* format, ...) __attribute__((format(printf, 1, 2)));
/// The message of the most recent fail(); empty string when none.
const char* last_error();
void clear_error();

enum class KERN_BLUEPRINT_TYPE LogLevel : i32 { debug = 0, info = 1, warn = 2, error = 3 };
void log(LogLevel level, const char* format, ...) __attribute__((format(printf, 2, 3)));

/// Monotonic milliseconds (performance.now() in the browser).
KERN_CLOCK f64 now_ms();
/// The wall clock: whole milliseconds since 1970-01-01 00:00 UTC (Date.now() in the browser) - a time
/// several machines agree on (a shared round, a time of day); for measuring durations, now_ms.
KERN_CLOCK f64 wall_ms();

template <class T>
constexpr T min_of(T a, T b) { return b < a ? b : a; }
template <class T>
constexpr T max_of(T a, T b) { return a < b ? b : a; }
template <class T>
constexpr T clamp(T v, T lo, T hi) { return v < lo ? lo : (hi < v ? hi : v); }

/// A non-owning view of contiguous elements (std::span without the header weight).
template <class T>
struct Span {
  T* data = nullptr;
  std::size_t size = 0;
  constexpr Span() = default;
  constexpr Span(T* d, std::size_t n) : data(d), size(n) {}
  template <class C>
    requires(!std::is_same_v<std::remove_cv_t<C>, Span> && requires(C& c) { c.data(); c.size(); })
  constexpr Span(C& c) : data(c.data()), size(c.size()) {}
  constexpr T& operator[](std::size_t i) const { return data[i]; }
  constexpr T* begin() const { return data; }
  constexpr T* end() const { return data + size; }
  constexpr bool empty() const { return size == 0; }
};

/// A count beside a pointer a function writes numbers through, where each thing it counts is `Per`
/// numbers: `void render(f32* out, Count<2> frames)` writes 2 * frames numbers (interleaved stereo).
/// It is the whole number it holds - given as one, read as one; what a function writes there is said
/// by its signature (the engine's nodes give the room it says: kern/logic/engine_api.h).
template <u32 Per>
struct Count {
  static_assert(Per > 0);
  static constexpr u32 per = Per;
  u32 n = 0;
  constexpr Count() = default;
  constexpr Count(u32 v) : n(v) {}
  constexpr operator u32() const { return n; }
};

/// FNV-1a, used for string keys and cache signatures.
constexpr u64 hash_bytes(const void* data, std::size_t n, u64 seed = 1469598103934665603ull) {
  const u8* p = static_cast<const u8*>(data);
  u64 h = seed;
  for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 1099511628211ull; }
  return h;
}

/// 32-bit integer mix (lowbias32), for spatial hashing.
constexpr u32 hash_u32(u32 x) {
  x ^= x >> 16; x *= 0x7feb352du;
  x ^= x >> 15; x *= 0x846ca68bu;
  x ^= x >> 16;
  return x;
}

}  // namespace kern
