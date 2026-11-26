#pragma once
#include "kern/app/app.h"
#include "kern/core/numbers.h"
#include "kern/math/math.h"
namespace cases {
using namespace kern;
/// A clock of the program in the language's numbers (kern::Float): the engine's numbers (Frame::dt, the components of
/// a kern::Vec3) are read as they are, and a Float that goes into one is rounded there.
struct Clock {
  Float seconds = 0;
  Vec3 at{};
  /// Adds a frame's seconds (the engine's f32, read exactly).
  void tick(const Frame& frame);
  /// The product of two components, read as Floats first.
  Float area() const;
  /// A Float into a component of the engine's vector (rounded there).
  void place(Float x);
};
}
