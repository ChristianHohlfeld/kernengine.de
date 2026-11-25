// The App of a program built from a level (kern/game/level_app.h): the program's logic runtime, without a world.
#include "kern/game/level_app.h"

#include <memory>
#include <string>

#include "kern/core/base.h"
#include "kern/game/level.h"
#include "kern/logic/engine_host.h"
#include "kern/logic/logic.h"

namespace kern::game {
namespace {

/// The host of the program's logic: no world - its time stands, what it prints is kept (program_said) and
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

/// The program's logic: its level (the graphs the runtime runs stand in it), its host, its runtime.
struct Program {
  Level level;
  ProgramHost host;
  std::unique_ptr<logic::Runtime> runtime;
  App* app = nullptr;
};

std::unique_ptr<Program>& kept() {
  static std::unique_ptr<Program> p;
  return p;
}

}  // namespace

App* level_app(logic::Runtime& rt) {
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

App* start_program_level(std::string_view level_text) {
  if (kept()) return kept()->app;
  auto p = std::make_unique<Program>();
  std::string error;
  if (!from_text(level_text, p->level, &error)) {
    log(LogLevel::error, "the program's level is broken: %s", error.c_str());
    return nullptr;
  }
  p->runtime = std::make_unique<logic::Runtime>(p->host);
  // (the libraries a Blueprint uses: the Blueprints of the level, by the name of the actor that has them - in
  // whatever folder they stand)
  const Level& level = p->level;
  p->runtime->blueprints = [&level](std::string_view name) -> const logic::Graph* {
    for (const Actor& a : level.actors)
      if (a.logic && a.name == name) return &*a.logic;
    return nullptr;
  };
  bool any = false;
  for (const Actor& a : level.actors)
    if (a.folder == kProgramFolder && a.logic && !a.logic->empty() && !a.logic->library) {
      p->runtime->add(*a.logic, static_cast<logic::ActorRef>(a.id), a.name);
      any = true;
    }
  if (!any) {
    log(LogLevel::error, "the program's level has no Blueprint in the folder %s", std::string(kProgramFolder).c_str());
    return nullptr;
  }
  p->runtime->begin();
  p->app = level_app(*p->runtime);
  if (!p->app) {
    log(LogLevel::error, "the program's level has no one Blueprint that implements App");
    p->runtime->end();
    return nullptr;
  }
  kept() = std::move(p);
  return kept()->app;
}

logic::Runtime* program_runtime() { return kept() ? kept()->runtime.get() : nullptr; }

std::vector<std::string> program_said(bool take) {
  if (!kept()) return {};
  std::vector<std::string> out = kept()->host.said;
  if (take) kept()->host.said.clear();
  return out;
}

void end_program_level() {
  if (kept()) kept()->runtime->end();
  kept().reset();
}

}  // namespace kern::game

extern "C" __attribute__((weak)) const char* kern_level_text(unsigned* length) {
  *length = 0;
  return nullptr;
}

extern "C" kern::App* kern_level_app(kern::Engine&) {
  unsigned n = 0;
  const char* text = kern_level_text(&n);
  if (!text) return nullptr;
  return kern::game::start_program_level(std::string_view(text, n));
}
