// The App of a program built from a level: the engine's object of the one Blueprint of the level that implements
// the interface App (kern::Implements<"App">, Graph::implements) - Engine Object of Blueprint (Runtime::engine_object).
//
// A program that defines kern_create_app() makes its App itself (that one wins). One that does not gets the
// app runtime's own (kern/src/app/app.cpp, weak): it asks kern_level_app (here), which loads the level the program
// carries (kern_level_text(), written by the build - SpawnCity: tools/embed_level.py) once into a world, plays it -
// its logic begins: the level's graph first, then its actors' Blueprints in the order of the level (Runtime::fire) -
// and gives that Blueprint's object as the App. The engine then calls it as any App: configure/extensions before
// the renderer starts, start, update each frame, stop. The world stays, for the program to take (program_world).
#pragma once

#include <string_view>

#include "kern/app/app.h"

namespace kern {
class Renderer;
namespace game {
class GameWorld;

/// The App the world's logic has: the object of the engine of the one Blueprint there whose graph implements App -
/// null when none does, more than one does, or the world plays no logic.
App* level_app(GameWorld& world);
/// Loads the level of `level_text` into a world on `renderer`, plays it and gives its App (level_app); the world is
/// kept (program_world) and made once - a second call gives the App of the first. Null: a broken level, a world
/// that did not start, or a level without such a Blueprint (and then no world is kept).
App* start_program_level(std::string_view level_text, Renderer& renderer);
/// The world start_program_level made (null: none - the program made its own App).
GameWorld* program_world();
/// Ends that world (its logic's End Play) and forgets it and its App.
void end_program_level();

}  // namespace game
}  // namespace kern

/// The level the program carries, as its text (null: none). Weak here; a game built from a level defines it.
extern "C" const char* kern_level_text(unsigned* length);
/// The App of the program's level on the engine's renderer (start_program_level with kern_level_text()): what the
/// app runtime's own kern_create_app gives.
extern "C" kern::App* kern_level_app(kern::Engine& engine);
