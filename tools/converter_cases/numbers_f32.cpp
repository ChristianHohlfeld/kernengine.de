#include "numbers_f32.h"
namespace cases {
void Clock32::tick(const Frame& frame) {
  seconds += frame.dt;
  if (seconds > 1.0f) seconds = 0;
}
f32 Clock32::area() const { return at.x * at.y; }
Float Clock32::more(f64 x) const { return static_cast<float>(x) + 0.5; }
}
