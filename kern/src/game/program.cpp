// The program's runtime (kern/game/program.h): the project's program Blueprints, without a world.
#include "kern/game/program.h"

#include <memory>
#include <string>

#include "kern/core/base.h"
#include "kern/game/runtime.h"
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
  /// The program reaches the actors of the worlds it runs (kern/docs/project.md 7): the logic runtime of the GameWorld of
  /// its objects that runs as `world` - none where that world ended (stopped, or started again: a new world) or is none
  /// the program made.
  logic::Runtime* world_runtime(logic::WorldId world, std::string& why) override {
    for (GameWorld* w : objects.all<GameWorld>())
      if (w->world_id() == world) {
        if (w->running() && w->logic_runtime()) return w->logic_runtime();
        why = w->running() ? "its world runs no Blueprints" : "its world does not run any more (the GameWorld was stopped)";
        return nullptr;
      }
    why = "no world of this program runs as world " + std::to_string(world) + " (it ended and started again as a new world, or the program did not make it)";
    return nullptr;
  }
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
  std::string profile;   // (set_profile: the device profile a world the program makes runs on)
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
  std::string why;
  return objects->get<App>(rt.engine_object(one->self(), "App", why));
}

Program::Program() = default;
Program::~Program() { end(); }

bool Program::start(const Project& project, std::string* error) { return begin(project, nullptr, error); }
bool Program::start(const Project& project, App& app, std::string* error) { return begin(project, &app, error); }

bool Program::begin(const Project& project, App* given, std::string* error) {
  if (state_) return refuse(error, "the program of the project " + state_->project.name + " runs already - end it first");
  std::string apps;
  int count = 0;
  for (const ProgramBlueprint& b : project.blueprints)
    if (!b.logic.library && b.logic.implements == "App") apps += (count++ ? ", " : "") + b.name;
  if (given && count > 0)
    return refuse(error, "the program's App is C++, and the project " + project.name + " has one too: " + apps + " implements App - a program has one App");
  if (!given && project.blueprints.empty()) return refuse(error, "the project " + project.name + " has no program Blueprints (\"blueprints\")");
  if (!given && count == 0) return refuse(error, "no Blueprint of the project " + project.name + " implements App");
  if (count > 1) return refuse(error, "more than one Blueprint of the project " + project.name + " implements App: " + apps);
  auto p = std::make_unique<State>();
  p->project = project;
  // a world the program makes (GameWorld, an object of the engine of its host) is a world of the project: its
  // Blueprints find libraries in its level, then in the project (kern/docs/project.md 3., 5.)
  // Blueprints find libraries in its level, then in the project (kern/docs/project.md 3., 5.) - and it runs on the
  // program's device profile (set_profile), as a world an App of C++ makes from its Engine (GameWorld(Engine&))
  p->host.objects.on_made(
      [](void* state, logic::EngineObjects::Kind kind, void* object) {
        if (kind != logic::EngineObjects::kind<GameWorld>()) return;
        const State& s = *static_cast<const State*>(state);
        GameWorld& w = *static_cast<GameWorld*>(object);
        w.set_project(&s.project);
        w.set_profile(s.profile);
      },
      p.get());
  p->runtime = std::make_unique<logic::Runtime>(p->host);
  p->runtime->blueprints = program_blueprints(p->project);
  p->runtime->budget = p->project.settings.loop_budget;   // (the loop guard is the project's: settings "loopBudget")
  // (each program Blueprint is an instance of its own - a library too, as an actor's in a world: the program's C++
  // calls it as its class on that object; its Self is its place in the project, from 1 - no actor)
  for (std::size_t i = 0; i < p->project.blueprints.size(); ++i) {
    const ProgramBlueprint& b = p->project.blueprints[i];
    if (!b.logic.empty()) p->runtime->add(b.logic, static_cast<logic::ActorRef>(i + 1), b.name);
  }
  p->runtime->begin();
  // its Blueprints that run as their classes are the program's (kern/logic/blueprint_class.h): each one bound now, for
  // the program's whole run - what the program's C++ calls (blueprint<T>()) and what an export calls, never an
  // instance that happened to start elsewhere
  for (const auto& i : p->runtime->instances())
    if (i && !i->bind_class()) {
      const std::string why = last_error();
      for (const auto& j : p->runtime->instances())
        if (j) j->unbind_class();
      p->runtime->end();
      return refuse(error, "the program of the project " + project.name + ": the class of its Blueprint " + i->name() + " - " + why);
    }
  p->app = given ? given : program_app(*p->runtime);
  if (!p->app) {
    for (const auto& j : p->runtime->instances())
      if (j) j->unbind_class();
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

void Program::set_profile(std::string name) {
  if (state_) state_->profile = std::move(name);
}
const std::string& Program::profile() const {
  static const std::string none;
  return state_ ? state_->profile : none;
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
  // (its classes are bound no more: an export called after the end says so - export_unbound)
  for (const auto& i : state_->runtime->instances())
    if (i) i->unbind_class();
  state_->runtime->end();
  delete state_;
  state_ = nullptr;
}

}  // namespace kern::game
