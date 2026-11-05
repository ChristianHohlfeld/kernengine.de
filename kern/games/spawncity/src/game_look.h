// SpawnCity on Kern: the state behind performance and the look (game_look.cpp). One member of
// Game (`look_`), so game.h only carries a few hooks.
#pragma once

#include <string>
#include <vector>

#include "kern/gfx/particles.h"
#include "look.h"
#include "perf.h"

namespace sc {

struct LookState {
  // performance (perf.h)
  perf::Meter meter;
  perf::Governor gov;
  bool show_perf = false;        // the HUD's frame-time line ("perf 1")
  bool software = false;         // a software rasterizer (the page: "render software"): no shadows, like the original
  bool started = false;
  i32 applied_tier = -1;
  u32 applied_settings = 0;
  f32 lamp_m = 170, people_m = 220;  // draw distances of the current level (game_town.cpp)
  // camera
  look::Shake shake;
  look::Stabilizer stab;
  look::SpeedLines lines;
  Vec3 last_vel;
  bool vel_valid = false;
  i32 last_mode = -1;
  // day and night (own clock, so "time" can set it; the host's phase in multiplayer)
  f32 tod = 0.40f;
  f32 sync_clock = 0;
  // point lights (scene_lighting.mjs over dynamic_lights.mjs)
  kern::LightPool lights{kern::kMaxPointLights, 96};
  u32 pool_size = kern::kMaxPointLights;
  struct Blast { Vec3 p; f32 at, ms, peak, r; };
  std::vector<Blast> blasts;
  struct Glint { Vec3 a, b; f32 at; };
  std::vector<Glint> glints;
  f32 flash_until = -1, lamp_pick_at = -99;
  std::vector<Vec3> near_lamps;
  u32 lights_used = 0;
  // speed streaks: one instanced quad mesh
  u32 streak_mesh = 0;
  std::vector<f32> streak_t, streak_tint;
  // first-person models for the game's own weapons (launchers, melee): body + glow per model
  u32 vm_body[4] = {}, vm_glow[4] = {};
  bool vm_built = false;
  i32 vm_shown = -1;
  f32 vm_fire_at = -99, vm_prev_next_attack = 0, vm_bob = 0, vm_bob_w = 0, vm_switch_at = -99;
  i32 vm_prev_weapon = -1, vm_punch = 0;
  std::vector<u32> kern_vm_hidden;  // Kern's view-model meshes hidden while ours is shown
};

}  // namespace sc
