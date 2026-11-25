// SpawnCity's level file and the Blueprints in it.
//
// level/spawncity.klevel is the game's project file - a Kern level as the editor reads and writes
// it: world settings, actors, and on the actors the Blueprints that hold the game's rules. The
// game starts from it (Game::build_level) and adds what is still built in code; BLUEPRINTS.md
// says what moves there next. The build embeds the file (tools/embed_level.py -> level_data.cpp).
#pragma once

#include <string>
#include <string_view>

#include "kern/logic/logic.h"

namespace sc::rules {

/// The level file's text.
std::string_view level_text();

}  // namespace sc::rules
