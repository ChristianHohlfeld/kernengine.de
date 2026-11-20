// SpawnCity on Kern: frame-time meter and the automatic quality level (performance first).
//
// Meter: the game's frame is cut into laps (game logic, physics steps, city streaming, draw
// preparation); each lap's milliseconds are smoothed and shown in the HUD JSON and, switched on,
// as a line in the HUD. The renderer's CPU time and the GPU time (when the browser has the timer
// query) come from Kern's RenderStats of the previous frame.
//
// Governor: SpawnCity's rule is "performance first" (neon_bloom.mjs switches its bloom off when
// a device cannot keep up; dynamic_lights.mjs takes fewer lights on phones). Here one ladder of
// five levels steps down while the smoothed rate stays below 50 fps and back up when there is
// reserve. Every limit and value is in this header; tests/test_perf.cpp pins the behaviour.
// No GameWorld: compiled into the native tests.
#pragma once

#include "common.h"

namespace sc::perf {

enum class Slot : u8 { logic, physics, stream, draw, count };
const char* slot_name(Slot s);

/// One quality level. Desktop starts at the top (4), phones at 2 (main.cpp's old phone profile
/// was level 2: shadows 70 m, AO low at half resolution, 44 people, pixel ratio 1.5).
struct Tier {
  const char* name;
  i32 people;            // not used for the crowd any more: PERSON_SLOTS 100 / phone 68 at every level (look_people_budget)
  f32 shadow_m;          // shadow distance (stylized_world_style.mjs: 110 desktop, 70 phone)
  i32 shadow_filter;     // PCF taps (Kern: 9 desktop, 4 phone)
  i32 ao;                // 0 off, 1 low at half resolution, 2 medium at half resolution, 3 medium full resolution
  f32 lamp_m;            // street lamps drawn within (game_town.cpp: 170 m before M3)
  f32 people_m;          // people drawn within (game_town.cpp: 220 m before M3)
  i32 lights;            // point lights used (dynamic_lights.mjs POOL: 6 desktop, 4 mobile)
  f32 render_scale;      // drawing-buffer density relative to the host's pixel ratio
};
constexpr i32 kTiers = 5;
constexpr Tier kTier[kTiers] = {
    {"minimal", 16, 35, 1, 0, 70, 90, 2, 0.6f},
    {"niedrig", 30, 50, 4, 0, 110, 140, 3, 0.75f},
    {"mittel", 44, 70, 4, 1, 140, 180, 4, 1.0f},  // the original draws at its full ceiling until frames are slow (phones: dynamic_lights.mjs 4)
    {"hoch", 64, 90, 9, 2, 170, 220, 6, 1.0f},    // the original's desktop pool: 6
    {"maximal", 80, 110, 9, 3, 170, 220, 10, 1.0f},  // Kern: more on the strongest machines (lamps, headlights)
};
constexpr i32 kDesktopStart = 4, kMobileStart = 2, kMobileMax = 3;

/// The governor's limits.
constexpr f32 kAverageTau = 1.0f;     // s: time constant of the frame-time average
constexpr f32 kDownFps = 50;          // below this (averaged) ...
constexpr f32 kDownHold = 2.0f;       // ... for this long: one level down
constexpr f32 kUpFps = 57;            // at or above this (vsync 60 Hz shows ~59-60) ...
constexpr f32 kUpHold = 8.0f;         // ... for this long: one level up
constexpr f32 kSettle = 3.0f;         // s after any change before the next decision (new level settles)
constexpr f32 kWarmup = 3.0f;         // s after start / reset without decisions (loading, shader warm-up)
constexpr f32 kBlockFirst = 30.0f;    // a level that was too heavy is not tried again for 30 s ...
constexpr f32 kBlockMax = 240.0f;     // ... doubling each time it fails again, up to 4 min

class Meter {
 public:
  void begin_frame(f64 now_ms) { start_ = last_ = now_ms; }
  /// Time since the previous lap (or begin_frame) goes to `slot`.
  void lap(Slot s, f64 now_ms);
  /// Smoothing of the finished frame (alpha 0.1 per frame); render / gpu: Kern's last RenderStats (gpu < 0: unknown).
  void end_frame(f64 now_ms, f32 render_cpu_ms, f32 gpu_ms);
  f32 ms(Slot s) const { return avg_[static_cast<u32>(s)]; }
  f32 update_ms() const { return update_; }
  f32 render_ms() const { return render_; }
  f32 gpu_ms() const { return gpu_; }

 private:
  f64 start_ = 0, last_ = 0;
  f32 cur_[static_cast<u32>(Slot::count)] = {};
  f32 avg_[static_cast<u32>(Slot::count)] = {};
  f32 update_ = 0, render_ = 0, gpu_ = -1;
};

class Governor {
 public:
  /// Starts at `tier` with `max_tier` as the ceiling (phones: kMobileMax).
  void reset(i32 tier, i32 max_tier);
  /// One display frame of `dt` seconds. Returns true when the level changed.
  bool update(f32 dt);
  i32 tier() const { return tier_; }
  i32 max_tier() const { return max_; }
  bool automatic() const { return auto_; }
  /// Fixed level (auto off) or back to automatic (keeps the current level).
  void set_fixed(i32 tier);
  void set_auto() { auto_ = true; since_change_ = 0; low_ = high_ = 0; }
  f32 fps() const { return avg_dt_ > 0 ? 1.0f / avg_dt_ : 0; }
  u32 changes() const { return changes_; }
  /// Seconds until `tier` may be tried again (0 = free).
  f32 blocked(i32 tier) const { return tier >= 0 && tier < kTiers ? std::fmax(0.0f, block_until_[tier] - clock_) : 0; }

 private:
  i32 tier_ = kDesktopStart, max_ = kTiers - 1;
  bool auto_ = true;
  f32 avg_dt_ = 1.0f / 60, clock_ = 0, since_change_ = 0, low_ = 0, high_ = 0;
  f32 block_until_[kTiers] = {}, block_len_[kTiers] = {};
  u32 changes_ = 0;
};

}  // namespace sc::perf
