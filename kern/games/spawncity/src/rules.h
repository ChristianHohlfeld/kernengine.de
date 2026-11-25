// SpawnCity's level file and the Blueprints in it.
//
// level/spawncity.klevel is the game's project file - a Kern level as the editor reads and writes
// it: world settings, actors, and on the actors the Blueprints that hold the game's rules. The
// game starts from it (Game::build_level) and adds what is still built in code; BLUEPRINTS.md
// says what moves there next. Its project, level/spawncity.kproject, holds the settings the engine applies at start.
// The build embeds both (kern/tools/embed_project.py -> level_data.cpp, kern/game/embedded.h).
#pragma once

#include <string>
#include <string_view>

#include "kern/game/embedded.h"
#include "kern/logic/logic.h"

namespace sc::rules {

/// The level file's text.
inline std::string_view level_text() { return kern::game::embedded_level("spawncity.klevel"); }

}  // namespace sc::rules
