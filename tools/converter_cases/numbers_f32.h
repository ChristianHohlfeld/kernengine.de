#pragma once
#include "kern/app/app.h"
#include "kern/core/numbers.h"
#include "kern/math/math.h"
namespace cases {
using namespace kern;
using Real = float;
/// The same clock in numbers that are not the language's: each a mistake with its place (--language-numbers).
struct Clock32 {
  f32 seconds = 0;
  double total = 0;
  Vec3 at{};
  Real scale = 1;
  void tick(const Frame& frame);
  f32 area() const;
  Float more(f64 x) const;
};
}
