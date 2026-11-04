// SpawnCity on Kern: the state behind menus, settings, start, equipment and presentation
// (game_ui.cpp). One member of Game (`ui_`), so game.h only carries a few hooks.
#pragma once

#include <string>

#include "gps_live.h"
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
};

}  // namespace sc
