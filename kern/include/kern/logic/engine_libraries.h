#pragma once
// The engine's own libraries: what the engine says about itself as Blueprints.
//
// Every header of the engine that a Blueprint can use is a library of the engine - its types (the
// structs and enums the header declares that a Blueprint can hold) and the functions the header
// defines (kern::math::lerp ...), named by the header's path: kern/physics/physics.h is the library
// "engine_physics_physics". They are written when the engine is built, from its headers
// (tools/engine_api.py --libraries -> src/logic/engine_libraries.gen.cpp), and every runtime, reader,
// tool and editor has them - no level holds them. A Blueprint names one in `uses`; what it holds of
// it (the types it names, how the functions look that it calls: logic::take_libraries) is always the
// engine's, and no level holds it: a level that holds a library of the engine, or a copy of one of its
// types or functions, is refused - with the place and the reason.
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "kern/core/json.h"
#include "kern/logic/logic.h"

namespace kern::logic {

/// The libraries of the engine by name, in the order they are declared (a library after those it uses).
const std::vector<std::string>& engine_library_names();
/// The engine's library of that name, or null: none.
const Graph* engine_library(std::string_view name);
/// Is `name` the name of a library of the engine? (Such a name is the engine's: a Blueprint of a
/// level of that name is an old copy, never the library.)
bool is_engine_library(std::string_view name);
/// The Blueprints of a project with the engine's libraries: a name of the engine finds the engine's
/// library, any other the project's Blueprint. The one way every reader finds a library
/// (libraries_of, take_libraries, link, from_cpp ... do it themselves).
Blueprints with_engine(const Blueprints& project);

/// Gives the Blueprint `g` what it uses of the engine's libraries (logic::take_libraries with the engine):
/// a level does not hold it (graph_to_json leaves it out, graph_from_json refuses it), the Blueprint
/// has it once it is read. `project`: the level's other Blueprints. False: `error` says why.
bool hold_engine(Graph& g, const Blueprints& project, std::string* error = nullptr);

/// A Blueprint as a level holds it: without what it holds of the engine's libraries (the types it names
/// and how the functions look that it calls - hold_engine gives them again when it is read).
json::Value graph_to_level_json(const Graph& g);
/// A Blueprint as a level holds it, read. False: it holds a copy of a type or a function of the engine's
/// libraries (or is not a Blueprint) - `error` says which, and why.
/// `marks`: the marks the level recorded for the engine's libraries ("engine": {library: mark}).
bool graph_from_level_json(const json::Value& j, Graph& out, std::string* error = nullptr, std::map<std::string, std::string>* marks = nullptr);
/// "engine_physics_physics: written with 1a2b..., the engine has 3c4d..." for each recorded mark that is not the
/// engine's now ("" when all are): what a message about a Blueprint that no longer fits the engine adds.
std::string engine_changes(const std::map<std::string, std::string>& marks);

/// The generated texts (src/logic/engine_libraries.gen.cpp): each library as its JSON.
struct EngineLibraryText {
  const char* name;
  const char* mark;  // the library's mark: the hash of its text - it changes with what the library is
  const char* json;
};
/// The mark of the engine's library of that name ("": none). A level records, for each library of the
/// engine its Blueprints use, the mark it was written with (graph_to_level_json: "engine"): what changed
/// since is then known, not guessed - a Blueprint that names something the library no longer has says so
/// where it names it, with both marks.
std::string engine_library_mark(std::string_view name);
const std::vector<EngineLibraryText>& engine_library_texts();
/// For the run that writes them (tools/engine_api.py --libraries: the level it converts the headers into
/// holds the new ones): this process knows no library of the engine. Before the first use; tools/blueprint_cpp
/// calls it when KERN_ENGINE_LIBRARIES=off.
void engine_libraries_off();

}  // namespace kern::logic
