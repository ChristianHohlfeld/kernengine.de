// SpawnCity on Kern, milestone 3 (life): the state of game_life.cpp - animals and street life
// (life.h), ragdolls (ragdoll.h), walkable buildings (interiors.h) and their drawing and sounds.
// Kept apart from game.h so the other modules' headers do not change with it.
#pragma once

#include <array>
#include <string>
#include <vector>

#include "interiors.h"
#include "kern/audio/audio.h"
#include "life.h"
#include "life_art.h"

namespace sc {

struct GameLife {
  Life life;
  LifeArt art;
  LifeOut out;
  Interiors interiors;
  bool ready = false, art_ready = false, sounds_ready = false;
  std::array<kern::audio::ClipId, static_cast<u32>(LifeOut::Sound::kCount)> clips = {};
  // speech for the page (browser speech synthesis), handed out once by the HUD
  std::vector<LifeOut::Say> speech;
  std::string banner;
  f32 banner_until = 0;
  // people with a ragdoll (People index + 1) and what the people did last frame
  std::vector<u64> person_dolls;
  std::vector<u8> prev_state;
  std::vector<f32> prev_hands;            // hands_up_until last frame (the scream when the arms fly up)
  std::vector<std::pair<i32, f32>> blocked;  // honkCheck: since when each traffic car waits (car, s)
  std::vector<std::pair<i32, f32>> next_honk;
  struct Blast { Vec3 at; f32 radius; f32 t; };
  std::vector<Blast> blasts;  // this frame's explosions (how hard a fall is thrown)
  u32 falls = 0, getups = 0;
  u64 test_doll = 0;  // the body of the "ragdoll" test command
  f32 next_honk_check = 0;
};

}  // namespace sc
