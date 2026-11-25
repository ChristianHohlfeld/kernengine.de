// The program's runtime (kern/docs/project.md, 3.): ONE logic runtime of the project's program Blueprints, in their
// order, on a host of its own - no world, no renderer, no simulation; the libraries they use are found in the project
// alone. It is started once (Begin Play) and stays until the program ends; the worlds the program starts (a GameWorld
// of a level of the project) are runtimes beside it.
//
// The App of a program built from its project: the engine's object of the ONE program Blueprint that implements the
// interface App (kern::Implements<"App">, Graph::implements) - Engine Object of Blueprint (Runtime::engine_object).
// The engine calls it as any App (configure/extensions before the renderer starts, start, update each frame, stop).
// A program that defines kern_create_app() makes its App itself (that one wins); one that does not gets the app
// runtime's own (kern/src/app/app.cpp, weak), which asks kern_project_app (here) with the project the program
// carries (kern_project_text(), written by the build - SpawnCity: tools/embed_level.py).
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "kern/app/app.h"
#include "kern/game/project.h"

namespace kern {
namespace logic {
class Runtime;
}
namespace game {

/// The App a runtime's logic has: the object of the engine of the one Blueprint there whose graph implements App -
/// null when none does or more than one does.
App* program_app(logic::Runtime& runtime);

/// A program: its project (a copy - the graphs its runtime runs stand in it), its host and its logic runtime. Its host
/// has no world: its time stands at 0, its random numbers start from a fixed seed, what it prints is kept (said) and
/// logged, the objects of the engine it makes or is given are its own.
class Program {
 public:
  Program();
  ~Program();   // ends it (end)
  Program(const Program&) = delete;
  Program& operator=(const Program&) = delete;
  /// Starts the program of `project`: its Blueprints, in their order, Begin Play. False - `error` (and the log) says
  /// why, with the names: it runs already, the project has no program Blueprints, none of them implements App, more
  /// than one does (named), the one gives no App; then nothing runs.
  bool start(const Project& project, std::string* error = nullptr);
  /// start with the project of a project file's text (kern/game/project.h); a text that is no project: false.
  bool start(std::string_view project_text, std::string* error = nullptr);
  bool running() const;
  /// The App: the engine's object of the program Blueprint that implements App (null: not running).
  App* app() const;
  logic::Runtime* runtime() const;
  /// The project it was started from (null: not running).
  const Project* project() const;
  /// What its logic printed (Print String) and the errors it reported ("error: ..."), in order, since the last call
  /// that took them (taken: cleared).
  std::vector<std::string> said(bool take = true);
  /// Ends its logic (End Play) and forgets it, its project and its App; it can start again.
  void end();

 private:
  struct State;
  State* state_ = nullptr;
};

}  // namespace game
}  // namespace kern

/// The App of the program's project (Program::start with the project file's text) - of the one Program of the app
/// runtime, started once: a second call while it runs gives null and logs why. What the app runtime's own
/// kern_create_app gives.
extern "C" kern::App* kern_project_app(const char* project_text, unsigned length);
