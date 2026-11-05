// State of the modules sc-m3-gaps closed (PARITY.md): one member `gaps_` in Game, the logic in
// game_gaps.cpp, the pure rules in gaps_rules.h / spawn_guard.h.
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "gaps_rules.h"
#include "lamps.h"
#include "kern/audio/audio.h"
#include "kern/gfx/renderer.h"

namespace sc {

struct GapsState {
  // police_sirens.mjs: one voice for the nearest cruiser, one for the nearest police drone; each is
  // a bright and a dull (air-absorbed) loop cross-faded by distance.
  gaps::SirenVoice car, drone;
  kern::audio::ClipId horn = 0, horn_dull = 0, whoop = 0, whoop_dull = 0;
  kern::audio::VoiceId v_horn = 0, v_horn_dull = 0, v_whoop = 0, v_whoop_dull = 0;
  f32 car_level = 0, drone_level = 0, clock = 0;
  // wanted_dispatch_guard.mjs: a police drone slot waits (unseen) for its arrival
  f32 pd_due[5] = {-1, -1, -1, -1, -1};
  // world_critical_damage_fx.mjs
  gaps::SmokeField smoke{78};
  struct Part { u32 id = 0; std::vector<f32> transforms, tints; };
  Part disc, ember;
  kern::Renderer* renderer = nullptr;
  std::vector<f32> car_crit;   // per traffic car: burn_until seen
  bool jet_burning = false;
  // world_organic_hit_audio.mjs
  f32 death_scan = -1, last_organic = -99;
  u32 organic_plays = 0;
  // impact_explosion_overlay.mjs: flight-fire impacts in the world (projected for the page)
  struct Burst { Vec3 p; f32 t = 0; u32 serial = 0; };
  std::vector<Burst> bursts;
  u32 burst_serial = 0;
  // spawn gates (spawn_visibility_guard.mjs, vehicle_spawn_clearance.mjs)
  f32 world_shown = -1;
  u32 spawn_denied = 0, spawn_allowed = 0, overlap_denied = 0;
  bool camera_lifted = false;
  // visual_pose_stabilization.mjs: the drone as drawn in the follow / third camera
  gaps::VisualPose drone_visual;
  f32 frame_dt = 1.0f / 60;
  // fighter_jets.mjs: one roar voice per low-pass jet (bright + low-passed loop, cross-faded)
  kern::audio::ClipId jet_roar = 0, jet_roar_dull = 0;
  struct JetVoice { f32 x = 0, y0 = 0, z = 0, t0 = 0, pitch = 0; kern::audio::VoiceId bright = 0, dull = 0; bool alive = false; };
  std::vector<JetVoice> jet_voices;
  u32 jet_voice_count = 0;
  // guided_missile.mjs: smoke trail (160 puffs, 2.6 s), motor voice, ignition bang, lock tone
  struct TrailPuff { Vec3 p; f32 born = -1; };
  std::vector<TrailPuff> trail = std::vector<TrailPuff>(160);
  u32 trail_cursor = 0;
  struct MotorVoice { f32 born = 0, lit = -1; kern::audio::VoiceId v = 0; bool alive = false; };
  std::vector<MotorVoice> motors;
  kern::audio::ClipId motor_clip = 0;
  bool guided_sounds = false;
  Part trail_part;
  // wanted_police_drones.mjs reportCrime: the last heat / stars seen, the star tones
  f32 last_heat = 0;
  i32 last_stars = 0;
  bool star_tones = false;
  // jet_mode.mjs passenger (game_seats.cpp)
  bool riding = false, pilot_held = false;
  u8 ride_owner = 0, pending_owner = 0;
  f32 seat_pending_until = -1, ride_seen = 0;
  // street_lamps.mjs (game_lamps.cpp)
  lamps::Field lamps;
  std::size_t lamp_lanes = 0;
  bool lamps_dirty = false, lamp_sounds = false;
  f32 lamp_car_check = -1;
  u32 lamps_broken_events = 0;
  std::vector<kern::physics::Body> fallen;
  Part lamp_pole, lamp_glow, lamp_pool, lamp_fallen;
};

}  // namespace sc
