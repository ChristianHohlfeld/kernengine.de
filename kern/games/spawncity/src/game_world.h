// SpawnCity on Kern, milestone 3: the world around the town - state of game_world.cpp (terrain
// and water physics, craters and marks, traffic rules, animals, street life, interiors, ragdolls).
// Kept apart from game.h so the other modules' headers do not change with it.
#pragma once

#include "common.h"

namespace sc {

struct WorldLife {
  // water (world_water.mjs, world_rigid_body_physics.mjs)
  f32 swim_since = -1;     // the player swims (on foot in deep water)
  u32 splashes = 0;
  bool in_water[32] = {};  // per body slot: last step in the water (splash on entry)
};

}  // namespace sc
