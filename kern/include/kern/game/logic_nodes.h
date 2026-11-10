// Kern Logic: the gameplay nodes in the node library (logic::library()).
#pragma once

#include <string>
#include <vector>

namespace kern::game {

/// The gameplay nodes as the library knows them - their names, pins and settings
/// (src/game/logic_node_shapes.cpp). That needs nothing of the game: a tool that only reads and
/// writes Blueprints (tools/blueprint_cpp) links it alone. Registers them on the first call; true.
bool logic_node_shapes_registered();
/// The types of those nodes ("actor.location", "game.hit" ...), in the order they were added.
const std::vector<std::string>& logic_node_types();

/// ... and what they do, bound to the game (src/game/logic_nodes.cpp). Always true; referenced
/// from GameWorld so that file is always linked.
bool logic_nodes_registered();

}  // namespace kern::game
