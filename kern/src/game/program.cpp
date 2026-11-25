// The program's runtime (kern/game/program.h): the project's program Blueprints, without a world.
#include "kern/game/program.h"

#include <memory>
#include <string>

#include "kern/core/base.h"
#include "kern/logic/engine_host.h"
#include "kern/logic/logic.h"

namespace kern::game {
namespace {

/// The host of the program's logic: no world - its time stands, what it prints is kept (Program::said) and
/// logged, the objects of the engine it makes or is given are its own.
class ProgramHost : public logic::Host {
 public:
  f64 time() const override { return 0; }
  void print(const std::string& text, bool) override {
    said.push_back(text);
    log(LogLevel::info, "%s", text.c_str());
  }
  void error(const std::string& text) override {
    said.push_back("error: " + text);
    log(LogLevel::error, "%s", text.c_str());
  }
  u32 random() override {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
  }
  bool engine(std::string_view function, const std::vector<logic::Value>& in, std::vector<logic::Value>& out) override {
    if (logic::call_engine(objects, function, in, out)) return true;
    engine_failed(function);
    return false;
  }
  logic::EngineObjects* engine_objects() override { return &objects; }
  void engine_failed(std::string_view function) override { error(std::string(function) + ": " + objects.error); }

  std::vector<std::string> said;
  u32 seed = 0x9e3779b9u;
  logic::EngineObjects objects;
};

bool refuse(std::string* error, std::string why) {
  log(LogLevel::error, "%s", why.c_str());
  if (error) *error = std::move(why);
  return false;
}

}  // namespace

struct Program::State {
  Project project;
  ProgramHost host;
  std::unique_ptr<logic::Runtime> runtime;
  App* app = nullptr;
};

App* program_app(logic::Runtime& rt) {
  const logic::Instance* one = nullptr;
  for (const auto& i : rt.instances())
    if (i && !i->disabled() && i->graph().implements == "App") {
      if (one) return nullptr;   // (two Apps: which one is the program's is not said)
      one = i.get();
    }
  logic::EngineObjects* objects = rt.host().engine_objects();
  if (!one || !objects) return nullptr;
  return objects->get<App>(rt.engine_object(one->self()));
}

Program::Program() = default;
Program::~Program() { end(); }

bool Program::start(const Project& project, std::string* error) {
  if (state_) return refuse(error, "the program of the project " + state_->project.name + " runs already - end it first");
  std::string apps;
  int count = 0;
  for (const ProgramBlueprint& b : project.blueprints)
    if (!b.logic.library && b.logic.implements == "App") apps += (count++ ? ", " : "") + b.name;
  if (project.blueprints.empty()) return refuse(error, "the project " + project.name + " has no program Blueprints (\"blueprints\")");
  if (count == 0) return refuse(error, "no Blueprint of the project " + project.name + " implements App");
  if (count > 1) return refuse(error, "more than one Blueprint of the project " + project.name + " implements App: " + apps);
  auto p = std::make_unique<State>();
  p->project = project;
  p->runtime = std::make_unique<logic::Runtime>(p->host);
  p->runtime->blueprints = program_blueprints(p->project);
  // (each program Blueprint is an instance of its own; its Self is its place in the project, from 1 - no actor)
  for (std::size_t i = 0; i < p->project.blueprints.size(); ++i) {
    const ProgramBlueprint& b = p->project.blueprints[i];
    if (!b.logic.empty() && !b.logic.library) p->runtime->add(b.logic, static_cast<logic::ActorRef>(i + 1), b.name);
  }
  p->runtime->begin();
  p->app = program_app(*p->runtime);
  if (!p->app) {
    p->runtime->end();
    return refuse(error, "the Blueprint " + apps + " of the project " + project.name + " gives no App (its object of the engine)");
  }
  state_ = p.release();
  return true;
}

bool Program::start(std::string_view project_text, std::string* error) {
  Project project;
  std::string why;
  if (!from_text(project_text, project, &why)) return refuse(error, "the program's project: " + why);
  return start(project, error);
}

bool Program::running() const { return state_ != nullptr; }
App* Program::app() const { return state_ ? state_->app : nullptr; }
logic::Runtime* Program::runtime() const { return state_ ? state_->runtime.get() : nullptr; }
const Project* Program::project() const { return state_ ? &state_->project : nullptr; }

std::vector<std::string> Program::said(bool take) {
  if (!state_) return {};
  std::vector<std::string> out = state_->host.said;
  if (take) state_->host.said.clear();
  return out;
}

void Program::end() {
  if (!state_) return;
  state_->runtime->end();
  delete state_;
  state_ = nullptr;
}

}  // namespace kern::game

namespace kern {
namespace {
/// The App the app runtime runs for a program started by kern::run(project): the program's App, and at the end the
/// program's end (after App::stop - its Blueprints end with it). An app started again starts its program again.
class ProgramApp final : public App {
 public:
  game::Project project;
  game::Program program;
  /// The program's App - started again from the project if it ended (null: it does not start; the log says why).
  App* app() {
    if (!program.running()) program.start(project);
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
}  // namespace

int run(const game::Project& project) {
  // (the app runtime's one program, kept to the end of the process: the app runtime holds its App)
  static ProgramApp* const app = new ProgramApp;
  if (app->program.running()) {
    log(LogLevel::error, "kern::run: the program of the project %s runs already - kern_main starts one program, once", app->program.project()->name.c_str());
    return 1;
  }
  if (!app->program.start(project)) return 1;
  app->project = project;
  return run(*app, project.settings);
}

}  // namespace kern
