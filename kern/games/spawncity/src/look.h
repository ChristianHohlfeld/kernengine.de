// SpawnCity on Kern: the look - camera shake, FPV optical stabilizer, speed streaks, day and night
// and the scene's point lights - as plain models without GameWorld (game_look.cpp drives them;
// tests/test_look.cpp checks them). Values from SpawnCity (nostack-dev/spawncity-de, sim/):
//   camera_shake.mjs, fpv_optical_stabilizer.mjs, speed_lines.mjs, day_night_sync.mjs,
//   scene_lighting.mjs, dynamic_lights.mjs, police_ground_units.mjs, wanted_police_drones.mjs.
#pragma once

#include "common.h"
#include "kern/gfx/renderer.h"

namespace sc::look {

// ---- camera_shake.mjs: trauma in [0,1], added by events, decays; visible shake = trauma^2 ----
struct Shake {
  static constexpr f32 kDecayPerS = 1.35f, kMaxAngle = 0.055f, kMaxOffset = 0.09f, kFreq = 19;
  f32 trauma = 0, seed = 17.3f;
  void add(f32 amount, f32 cap = 1);
  /// Advances the decay; fills camera-local angles (pitch about right, yaw about up, roll about
  /// forward) and offsets (right, up). False when there is nothing to apply.
  bool sample(f32 dt, f64 time_s, f32 scale, Vec3& angles, Vec2& offset);
};
/// SpawnCity's trauma per weapon (camera_shake.mjs onWeapon) and per blast (onExplosion).
f32 weapon_trauma(bool full_auto, bool heavy, bool drone, f32& cap);
f32 blast_trauma(f32 distance, f32 radius);
/// Hard impacts of the player's car / drone (onImpact): 0 below 2.5 m/s of velocity change.
inline f32 impact_trauma(f32 dv) { return dv < 2.5f ? 0.0f : clampf((dv - 2.5f) / 14, 0, 0.7f); }

/// Applies angles / offsets in the camera's own frame.
void shake_camera(kern::Camera& c, Vec3 angles, Vec2 offset);

// ---- fpv_optical_stabilizer.mjs: the FPV view is filtered like an optical gimbal ----
struct Stabilizer {
  static constexpr f32 kPositionHz = 55, kOrientationHz = 45, kTeleport = 0.35f, kTargetTeleport = 2.5f;
  bool init = false;
  Vec3 pos, target, up;
  f32 lag = 0;  // metres between the raw and the shown eye (debug)
  void reset() { init = false; lag = 0; }
  void apply(kern::Camera& c, f32 dt);
};

// ---- speed_lines.mjs: radial streaks from the screen centre when the camera moves fast ----
struct SpeedLines {
  static constexpr f32 kStart = 13, kFull = 40;  // m/s
  static constexpr int kCount = 26;
  struct Streak { f32 a, r, len, v; bool warm; };
  Streak s[kCount] = {};
  bool seeded = false, have_last = false;
  Vec3 last;
  f32 speed = 0, k = 0;
  Rng rng{0x5eed};
  /// Camera position per frame; returns the strength k in [0,1].
  f32 update(Vec3 eye, f32 dt);
  void respawn(int i);
};

// ---- day and night: one clock (480 s per day, SpawnCity) the game owns ----
constexpr f32 kDayLength = 480;  // s (real_world_bootstrap.mjs / the day-night cycle)
/// The lighting of a time of day: the same sun path and colours as Kern's GameWorld (so nothing
/// changes in look), driven by the game's own clock so "time" can set it.
f32 day_lighting(f32 tod, f32 sun_azimuth_deg, f32 fog_density, f32 exposure, kern::Lighting& l, kern::SkyState& sky);
/// day_night_sync.mjs: a client follows the host's phase - a small drift is eased out (half of it),
/// a big one jumps, below 0.0005 nothing changes. Returns the new phase in [0,1).
f32 follow_phase(f32 mine, f32 authority);
/// "time 0.75", "time 21:30" or "time 21" -> phase; false if not understood.
bool parse_time(const char* text, f32& phase);

// ---- point lights (scene_lighting.mjs over dynamic_lights.mjs) ----
/// three.js lights fall off with 1/d^2 (physically based, BRDF / pi); Kern's shader has a windowed
/// falloff with a 0.12 scale. A SpawnCity intensity maps to Kern's so the lit surface at `d_ref`
/// metres gets the same light.
f32 kern_intensity(f32 three_intensity, f32 range, f32 d_ref);
/// 0xRRGGBB (sRGB) -> linear RGB.
Vec3 hex_linear(u32 rgb);
/// Police light bar (police_ground_units.mjs flashLights): 110 ms slots of six, red in 0 and 2,
/// blue in 3 and 5. Returns 0 dark, 1 red, 2 blue.
int police_flash(f32 now_s);
/// Police drone (wanted_police_drones.mjs): 92 ms slots of eight offset by 37 ms per drone, red 0-1, blue 4-5.
int drone_flash(f32 now_s, int index);
/// Street lamps glow from dusk (scene_lighting.mjs): (night - 0.15) / 0.5.
inline f32 lamp_glow(f32 night) { return clampf((night - 0.15f) / 0.5f, 0, 1); }

}  // namespace sc::look
