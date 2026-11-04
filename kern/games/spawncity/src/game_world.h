// SpawnCity on Kern, milestone 3: the world around the town - state of game_world.cpp (terrain
// and water physics, craters and marks, traffic rules, animals, street life, interiors, ragdolls).
// Kept apart from game.h so the other modules' headers do not change with it.
#pragma once

#include <vector>

#include "common.h"
#include "kern/audio/audio.h"
#include "world_art.h"
#include "world_marks.h"

namespace sc {

/// The world's own synth sounds (splash, animals, voices, horn): Kern's mixer, made once.
struct WorldSounds {
  bool ready = false;
  kern::audio::ClipId splash = 0, splash_small = 0, moo = 0, coo = 0, flap = 0, horn = 0, tow = 0, chatter[4] = {};
};

struct WorldLife {
  // water (world_water.mjs, world_rigid_body_physics.mjs)
  bool swimming = false;
  u32 splashes = 0;
  f32 in_water[32] = {};   // per body slot: submerged fraction last step (splash on entry)
  struct Splash { Vec3 at; f32 size; };
  std::vector<Splash> pending_splashes;  // found in the fixed step, shown in the frame
  WorldSounds sounds;
  // drawing and what blasts leave behind (world_art.h, world_marks.h)
  WorldArt art;
  bool art_ready = false;
  Marks marks;
};

}  // namespace sc
