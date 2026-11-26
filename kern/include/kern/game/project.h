// The project: a game as a whole (kern/docs/project.md) - its settings, its levels, the level it starts with and its
// program (the program's Blueprints and their libraries). One project per game, the file <name>.kproject
// ("format": "kern-project"); a level (kern/game/level.h) holds only its world.
//
//   { "format": "kern-project", "version": 1, "name": "My Game",
//     "settings": { "renderer": { ... }, "profiles": [{ "name": ..., "renderer": { ... } }], "heapReserveMb": 0 },   (kern/app/project.h)
//     "levels": ["levels/start.klevel"], "startLevel": "levels/start.klevel",
//     "blueprints": [ { "name": "MyGame", "logic": { ... } }, { "name": "Math", "folder": "Tools", "logic": { "library": true, ... } } ] }
//
// Where a Blueprint finds the libraries it uses (`uses`, by name): one of the program only in the project; one of a
// world first in its level, then in the project. A name that stands in the level and in the project is an error
// (library_conflicts) - neither wins.
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "kern/app/project.h"
#include "kern/core/json.h"
#include "kern/logic/logic.h"

namespace kern::game {

struct Level;

/// A Blueprint of the program: it belongs to no world and runs in the program's runtime (kern/game/program.h).
struct ProgramBlueprint {
  std::string name;
  logic::Graph logic;
  /// Order in the editor, as an actor's folder in a level ("": none) - it means nothing to the engine; a tool that
  /// writes Blueprints (tools/cpp_to_blueprint.py) knows its own by it.
  std::string folder;
};

struct Project {
  std::string name = "Untitled";
  ProjectSettings settings;                 // what the engine applies before the renderer starts (kern::run)
  std::vector<std::string> levels;          // the project's levels, paths relative to the project file
  std::string start_level;                  // one of `levels` ("": none named - the first, if any)
  std::vector<ProgramBlueprint> blueprints;  // the program's Blueprints and libraries, in their order

  ProgramBlueprint* blueprint(std::string_view name);
  const ProgramBlueprint* blueprint(std::string_view name) const;
  /// The level the editor opens at Play and a program that chooses none starts: start_level, else the first.
  std::string first_level() const;
};

json::Value to_json(const Project& project);
std::string to_text(const Project& project, i32 indent = 2);
/// Reads a project. False - `error` says where and why ("blueprints[2] \"Menu\": ...") - for a document that is not
/// a project, a blueprint without a name or one whose name stands twice, a Blueprint that does not read, a level named
/// twice or a startLevel that is none of the levels. Unknown keys are ignored, missing ones take defaults.
bool from_json(const json::Value& doc, Project& out, std::string* error = nullptr);
bool from_text(std::string_view text, Project& out, std::string* error = nullptr);

/// The Blueprints a program Blueprint finds as libraries: the project's, by name.
logic::Blueprints program_blueprints(const Project& project);
/// The Blueprints a world's Blueprint finds as libraries: its level's (by the name of the actor that has it), then
/// the project's (null: the level's alone).
logic::Blueprints world_blueprints(const Level& level, const Project* project);
/// Why a Blueprint of a world may not have the functions it has: one of them is an export (Function::exported) -
/// exports belong to the program (kern/docs/project.md 4.): "" when none is. A level that holds one is refused when
/// it is read (with the place), and validate says so.
std::string world_export(const logic::Graph& graph);
/// The names that stand in the level (an actor with a Blueprint) and in the project, each with both places:
/// "the Blueprint Math stands in the level (actor 7) and in the project (blueprints[0]) - rename one". Empty: none.
std::vector<std::string> library_conflicts(const Level& level, const Project& project);

}  // namespace kern::game
