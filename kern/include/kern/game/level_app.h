// The App of a program built from a level: the engine's object of the one Blueprint of the level's program (the folder its
// project names) that implements the interface App (kern::Implements<"App">, Graph::implements) - Engine Object of Blueprint
// (Runtime::engine_object).
//
// A program that defines kern_create_app() makes its App itself (that one wins). One that does not gets the
// app runtime's own (kern/src/app/app.cpp, weak): it asks kern_level_app (here), which reads the level the program
// carries (kern_level_text(), written by the build - SpawnCity: tools/embed_level.py) once and starts the program's
// logic: ONE runtime of the Blueprints of the folder its project names (ProjectSettings::program, "program" in the
// level's "project" - no folder is the program by its name alone), in the order of the level; the libraries they use
// are found among all the level's actors, on a host of its own - no world, no renderer, no simulation. Its Begin Play
// runs, and that Blueprint's object is the App: the engine calls it as any App (configure/extensions before the
// renderer starts, start, update each frame, stop). The world the program then starts itself (a GameWorld of its
// own level) is a second runtime beside this one, as before.
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "kern/app/app.h"

namespace kern {
namespace logic {
class Runtime;
}
namespace game {

/// The App a runtime's logic has: the object of the engine of the one Blueprint there whose graph implements App -
/// null when none does or more than one does.
App* level_app(logic::Runtime& runtime);
/// Reads the level of `level_text`, starts the logic of the folder its project names as the program (Begin Play) and gives its App
/// (level_app); that logic is kept (program_runtime) and started once - a second call gives the App of the first.
/// Null - and the log says why: a broken level, a project that names no program, no Blueprint in that folder, or
/// none of them (or two) implement App; then nothing is kept.
App* start_program_level(std::string_view level_text);
/// The program's logic runtime start_program_level started (null: none - the program made its own App).
logic::Runtime* program_runtime();
/// What the program's logic printed (Print String) and the errors it reported ("error: ..."), in order, since the
/// last call that took them (taken: cleared).
std::vector<std::string> program_said(bool take = true);
/// Ends that logic (its End Play) and forgets it and its App.
void end_program_level();

}  // namespace game
}  // namespace kern

/// The level the program carries, as its text (null: none). Weak here; a game built from a level defines it.
extern "C" const char* kern_level_text(unsigned* length);
/// The App of the program's level (start_program_level with kern_level_text()): what the app runtime's own
/// kern_create_app gives.
extern "C" kern::App* kern_level_app(kern::Engine& engine);
