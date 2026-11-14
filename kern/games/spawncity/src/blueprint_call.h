// How the game's C++ calls a rule that is a Blueprint of the level (wanted.cpp, combat_detail.cpp,
// guided.cpp ...): the running Blueprint of one actor, its functions by name with their values,
// its variables. Nothing is computed here. A call with typed values and outputs
// (kern::logic::ActorBlueprint::call) is what Call on Actor is in a Blueprint: a struct the game hands
// over is one of its own whose fields are the Blueprint's, in their order - a 32-bit number of the
// game as the decimal it stands for (decimal(), below).
#pragma once
#include <cmath>
#include <string>
#include <vector>

#include "common.h"
#include "kern/logic/actor_blueprint.h"
#include "kern/logic/logic.h"

namespace sc::rules {

/// A number of the game's 32-bit state as a Blueprint gets it: the decimal number it stands for.
/// A Blueprint computes with 64-bit numbers (as the original's JavaScript does) and its rules
/// name their thresholds in decimals - 0.1, 2.6, 0.42. The game keeps its state in 32 bit, where
/// "0.1" is 0.100000001490116...; passed on as that, it would lie just beyond a threshold of 0.1
/// it stands exactly on. So a 32-bit number crosses over as the shortest decimal that is this
/// very 32-bit number (0.1f -> 0.1): every comparison of it with a decimal threshold then
/// answers exactly as the comparison in 32 bit did, on both sides of the threshold and on it.
/// The way is one to one and keeps the order: back in 32 bit it is the same number again, and of
/// two numbers the larger stays the larger. (Whole numbers, and numbers beyond the range a game
/// counts in, are what they are.)
inline f64 decimal(f32 x) {
  const f64 d = static_cast<f64>(x);
  const f64 a = std::fabs(d);
  if (!(a >= 1e-9 && a < 1e7) || d == std::trunc(d)) return d;
  f64 scale = 1;  // 10^(digits - 1 - exponent): the candidate with `digits` significant digits is round(d * scale) / scale
  for (f64 m = a; m < 1; m *= 10) scale *= 10;
  for (f64 m = a; m >= 10; m /= 10) scale /= 10;
  for (int digits = 1; digits <= 9; ++digits, scale *= 10) {
    const f64 c = scale >= 1 ? std::round(d * scale) / scale : std::round(d * scale) * std::round(1 / scale);
    if (static_cast<f32>(c) == x) return c;
  }
  return d;
}
/// (A 64-bit number - a time in milliseconds, a number counted in 64 bit - is passed as it is: no
/// decimal() of it.)

/// The Blueprint of one of the level's actors - Blueprint g_rules{"Lamps"}: the game binds `running` once its
/// world has started, tests bind the one they run.
struct Blueprint : kern::logic::ActorBlueprint {
  using ActorBlueprint::ActorBlueprint;
};

}  // namespace sc::rules
