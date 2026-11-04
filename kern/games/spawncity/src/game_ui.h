// SpawnCity on Kern: the state behind menus, settings, start, equipment and presentation
// (game_ui.cpp). One member of Game (`ui_`), so game.h only carries a few hooks.
#pragma once

#include <string>

#include "settings.h"

namespace sc {

struct UiState {
  Settings settings;
  u32 applied = 0;          // settings version the game last applied
  bool paused = false;      // a menu is open (title, pause, settings): no input, no simulation
  bool audio_paused = false;
  u32 resets = 0;
  bool drone_seen = false;  // the drone of this flight got its hover height
};

}  // namespace sc
