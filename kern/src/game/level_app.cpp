// The App of a program built from a level (kern/game/level_app.h).
#include "kern/game/level_app.h"

#include <memory>
#include <string>

#include "kern/core/base.h"
#include "kern/game/level.h"
#include "kern/game/runtime.h"
#include "kern/logic/engine_host.h"

namespace kern::game {
namespace {
std::unique_ptr<GameWorld>& kept_world() {
  static std::unique_ptr<GameWorld> w;
  return w;
}
App*& kept_app() {
  static App* a = nullptr;
  return a;
}
}  // namespace

App* level_app(GameWorld& world) {
  logic::Runtime* rt = world.logic_runtime();
  if (!rt) return nullptr;
  const logic::Instance* one = nullptr;
  for (const auto& i : rt->instances())
    if (i && !i->disabled() && i->graph().implements == "App") {
      if (one) return nullptr;   // (two Apps: which one is the program's is not said)
      one = i.get();
    }
  logic::EngineObjects* objects = rt->host().engine_objects();
  if (!one || !objects) return nullptr;
  return objects->get<App>(rt->engine_object(one->self()));
}

App* start_program_level(std::string_view level_text, Renderer& renderer) {
  if (kept_world()) return kept_app();
  Level level;
  std::string error;
  if (!from_text(level_text, level, &error)) {
    log(LogLevel::error, "the program's level is broken: %s", error.c_str());
    return nullptr;
  }
  auto world = std::make_unique<GameWorld>(renderer);
  if (!world->start(level, SimMode::play)) return nullptr;
  App* app = level_app(*world);
  if (!app) {
    log(LogLevel::error, "the program's level has no one Blueprint that implements App");
    world->stop();
    return nullptr;
  }
  kept_world() = std::move(world);
  kept_app() = app;
  return app;
}

GameWorld* program_world() { return kept_world().get(); }

void end_program_level() {
  if (kept_world()) kept_world()->stop();
  kept_world().reset();
  kept_app() = nullptr;
}

}  // namespace kern::game

extern "C" __attribute__((weak)) const char* kern_level_text(unsigned* length) {
  *length = 0;
  return nullptr;
}

extern "C" kern::App* kern_level_app(kern::Engine& engine) {
  unsigned n = 0;
  const char* text = kern_level_text(&n);
  if (!text) return nullptr;
  return kern::game::start_program_level(std::string_view(text, n), engine.renderer());
}
