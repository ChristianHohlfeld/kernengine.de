#pragma once
#include <memory>
#include "kern/app/app.h"
#include "kern/game/runtime.h"
namespace cases {
/// A state that makes its world from the program's Engine (GameWorld(Engine&) beside GameWorld(Renderer&, ...): the
/// constructor C++ chooses for these arguments).
class Maker {
 public:
  void make(kern::Engine& engine) { world_ = std::make_unique<kern::game::GameWorld>(engine); }
  void make_drawn(kern::Engine& engine) { world_ = std::make_unique<kern::game::GameWorld>(engine.renderer()); }
 private:
  std::unique_ptr<kern::game::GameWorld> world_;
};
Maker& maker();
}
