#pragma once
#include "kern/math/math.h"
namespace cases {
namespace weapons {
enum class Id : kern::u8 { mp, glock };
struct Spec { const char* name; kern::f32 interval_ms; kern::f32 range_m; };
constexpr Spec kTable[] = { {"MP", 55, 180}, {"Glock", 110, 240} };
inline const Spec& spec(Id id) { return kTable[static_cast<kern::u8>(id)]; }
}
namespace format {
/// Another Spec, of its own namespace (as kern::logic::format::Spec beside weapons::Spec).
struct Spec { bool left = false; bool plus = false; kern::i32 width = 0; };
/// The width of a spec, and the interval of the glock.
kern::f32 width_and_glock(Spec s);
}
}
