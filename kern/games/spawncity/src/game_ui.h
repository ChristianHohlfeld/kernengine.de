// SpawnCity on Kern: the state behind menus, settings, start, equipment and presentation
// (game_ui.cpp). One member of Game (`ui_`), so game.h only carries a few hooks.
#pragma once

#include <string>
#include <vector>

#include "equipment.h"
#include "gps_live.h"
#include "kern/audio/audio.h"
#include "settings.h"

namespace sc {

struct UiState {
  Settings settings;
  u32 applied = 0;          // settings version the game last applied
  bool paused = false;      // a menu is open (title, pause, settings): no input, no simulation
  bool audio_paused = false;
  u32 resets = 0;
  bool drone_seen = false;
  f32 head_yaw_before = 0, head_pitch_before = 0;  // keep-look: the cockpit view before this frame's recentring
  u32 assists = 0;                                 // frames the aim assist pulled (tests)
  u32 grid_mesh = 0, collider_mesh = 0;
  Vec3 grid_at{1e9f, 1e9f, 0};
  f32 collider_clock = 0;
  i32 world_military = -1, world_zombies = -1;  // WORLD switches last passed to the combat systems  // the drone of this flight got its hover height
  // start (start_location.mjs, auto_flight_start.mjs, gps_live_mode.mjs)
  GpsLive gps;
  u32 moves = 0;            // how often the world moved to another start place
  std::string place;        // name of the start place the page chose ("" = GPS / default)
  Vec3 gps_cam_eye, gps_cam_look;
  // the blue position puck (accuracy circle, ring, dot, heading wedge) and the track line
  u32 puck_acc = 0, puck_ring = 0, puck_dot = 0, puck_wedge = 0, trail_mesh = 0;
  std::vector<Vec2> trail;
  bool trail_dirty = false;
  f32 trail_upload = 0;
  bool gps_cam_init = false;
  // equipment (game_equipment.cpp)
  Jetpack jetpack;
  bool jump_was = false, jet_pending = false;  // jet_pending: tests ignite at the next airborne frame
  kern::audio::VoiceId remote_jet_voice[8] = {};  // other players' jetpacks (by remote index)
  kern::audio::VoiceId jet_voice = 0, city_voice = 0, birds_voice = 0, crickets_voice = 0;
  kern::audio::ClipId jet_clip = 0, whoomp_clip = 0, thump_clip = 0, city_clip = 0, birds_clip = 0, crickets_clip = 0;
  FlightLog log;
  std::string net_status;
  MusicSynth music;
  i32 station = 0;
  bool radio_on = true;
  std::string banner, banner_sub;
  f32 banner_age = 99;
  Vec3 car_vel;
  bool car_vel_valid = false;
  u32 thumps = 0;
};

}  // namespace sc
