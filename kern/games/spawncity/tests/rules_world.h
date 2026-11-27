// The level's world for a test of the rules: the project the game carries, started as the game starts it (kern::run's
// way: the program beside an App), its level a simulated world - the actors Controls and Settings with their Blueprints
// as the game runs them. The program's own functions that call those actors (controls.cpp, settings.cpp) are bound to
// this world, as SpawnCity::start binds them (ActorBlueprint::bind(world)).
#pragma once
#include <cstdio>
#include <memory>

#include "kern/app/app.h"
#include "kern/app/runtime.h"
#include "kern/game/embedded.h"
#include "kern/game/program.h"
#include "kern/game/runtime.h"
#include "kern/kern.h"

namespace sc::test {

struct RulesWorld {
  struct LevelApp final : kern::App {
    std::unique_ptr<kern::game::GameWorld> world;
    bool start(kern::Engine& e) override {
      world = std::make_unique<kern::game::GameWorld>(e);
      return world->start_level("spawncity.klevel", kern::game::SimMode::simulate);
    }
    void update(kern::Engine&, const kern::Frame&) override {}
    void stop(kern::Engine&) override { world.reset(); }
  };
  kern::game::ProgramApp program;
  LevelApp app;
  kern::Renderer renderer;
  kern::AppRuntime rt{renderer};
  bool ok = false;
  RulesWorld() {
    program.project = kern::game::embedded_project();
    program.given = &app;
    ok = rt.give(program, program.project.settings) == 0 && rt.start(320, 240) && app.world != nullptr;
    if (!ok) std::printf("the level's world does not start: %s\n", kern::last_error());
  }
  kern::game::GameWorld& world() { return *app.world; }
};

}  // namespace sc::test
