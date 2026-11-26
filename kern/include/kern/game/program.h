// The program's runtime (kern/docs/project.md, 3.): ONE logic runtime of the project's program Blueprints, in their
// order, on a host of its own - no world, no renderer, no simulation; the libraries they use are found in the project
// alone. It is started once (Begin Play) and stays until the program ends; the worlds the program starts (a GameWorld
// of a level of the project) are runtimes beside it.
//
// The App of a program built from its project: the engine's object of the ONE program Blueprint that implements the
// interface App (kern::Implements<kern::App>, Graph::implements) - Engine Object of Blueprint (Runtime::engine_object).
// The engine calls it as any App (extensions before the renderer starts, start, update each frame, stop).
// A game built from its project starts it from its entry: `int kern_main() { return kern::run(kern::game::embedded_project()); }`
// (kern/docs/project.md 4.; kern/game/embedded.h).
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "kern/app/app.h"
#include "kern/app/run.h"
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
/// logged, the objects of the engine it makes or is given are its own. A world it makes (a GameWorld of its host) is
/// given the project when it is made (GameWorld::set_project): its Blueprints find libraries in its level, then in
/// the project; it must end before the program does (it ends with the program's objects at the latest).
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
  /// Starts the program of `project` with an App of C++ (kern::run(App&, const Project&)): the project's Blueprints run
  /// beside it - menus, libraries the App calls as their classes -, none of them may implement App (one App: false,
  /// named); a project without Blueprints is an empty program.
  bool start(const Project& project, App& app, std::string* error = nullptr);
  /// start with the project of a project file's text (kern/game/project.h); a text that is no project: false.
  bool start(std::string_view project_text, std::string* error = nullptr);
  bool running() const;
  /// The App: the engine's object of the program Blueprint that implements App, or the App of C++ it was started with
  /// (null: not running).
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
  bool begin(const Project& project, App* app, std::string* error);
  struct State;
  State* state_ = nullptr;
};

}  // namespace game

namespace detail {
/// The App the app runtime runs for a program started by kern::run(project): the program's App, and at the end the
/// program's end (after App::stop - its Blueprints end with it). An app started again starts its program again.
class ProgramApp final : public App {
 public:
  game::Project project;
  game::Program program;
  App* given = nullptr;   // (an App of C++: kern::run(App&, const Project&))
  /// The program's App - started again from the project if it ended (null: it does not start; the log says why).
  App* app() {
    if (!program.running()) given ? program.start(project, *given) : program.start(project);
    return program.app();
  }
  Span<ShaderExtension* const> extensions() override { App* a = app(); return a ? a->extensions() : Span<ShaderExtension* const>{}; }
  bool start(Engine& engine) override { App* a = app(); return a && a->start(engine); }
  void update(Engine& engine, const Frame& frame) override { if (App* a = program.app()) a->update(engine, frame); }
  void resized(Engine& engine) override { if (App* a = program.app()) a->resized(engine); }
  void stop(Engine& engine) override {
    if (App* a = program.app()) a->stop(engine);
    program.end();
  }
};
}  // namespace detail

/// The entry of a game whose App is a Blueprint of its project (kern/docs/project.md 4.): starts the program of
/// `project` (one Program of the app runtime) and hands the app runtime its App with the project's settings
/// (kern::run(App&, ProjectSettings)). When the app stops, the program ends after App::stop. Returns 0; when the
/// program does not start (Program::start says why, in the log) or an App was given already: 1.
/// (Inline: a program that links the game runtime without the app runtime - a tool, a test - does not need run.)
namespace detail {
inline int run_program(const game::Project& project, App* given) {
  // (the app runtime's one program, kept to the end of the process: the app runtime holds its App)
  static detail::ProgramApp* const app = new detail::ProgramApp;
  if (app->program.running()) {
    log(LogLevel::error, "kern::run: the program of the project %s runs already - kern_main starts one program, once", app->program.project()->name.c_str());
    return 1;
  }
  if (!(given ? app->program.start(project, *given) : app->program.start(project))) return 1;
  app->project = project;
  app->given = given;
  return run(*app, project.settings);
}
}  // namespace detail
inline int run(const game::Project& project) { return detail::run_program(project, nullptr); }

/// The entry of a game whose App is C++ and that has a project (kern/docs/project.md 4.): the project's program runs
/// beside the App (its Blueprints bound as their classes - what the App's C++ calls, its exports - from before
/// App::start to after App::stop), with the project's settings. One App: a Blueprint of the project that implements
/// App refuses the start (1, the log names it). (A game without a project: kern::run(App&, settings).)
inline int run(App& app, const game::Project& project) { return detail::run_program(project, &app); }

}  // namespace kern
