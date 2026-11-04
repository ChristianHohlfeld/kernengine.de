// SpawnCity on Kern: the state behind menus, settings, start, equipment and presentation
// (game_ui.cpp). One member of Game (`ui_`), so game.h only carries a few hooks.
#pragma once

#include <string>

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
  bool drone_seen = false;  // the drone of this flight got its hover height
  // start (start_location.mjs, auto_flight_start.mjs, gps_live_mode.mjs)
  GpsLive gps;
  u32 moves = 0;            // how often the world moved to another start place
  std::string place;        // name of the start place the page chose ("" = GPS / default)
  Vec3 gps_cam_eye, gps_cam_look;
  bool gps_cam_init = false;
  // equipment (game_equipment.cpp)
  Jetpack jetpack;
  bool jump_was = false, jet_pending = false;  // jet_pending: tests ignite at the next airborne frame
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
