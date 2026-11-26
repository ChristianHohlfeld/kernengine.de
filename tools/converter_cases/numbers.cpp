#include "numbers.h"
namespace cases {
void Clock::tick(const Frame& frame) {
  seconds += frame.dt;
  if (seconds > 1.0) seconds = seconds - 1.0;
}
Float Clock::area() const { return Float(at.x) * at.y; }
void Clock::place(Float x) { at.x = x / 3.0; }
}
