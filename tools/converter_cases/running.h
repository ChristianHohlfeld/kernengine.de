#pragma once
#include <memory>
#include "kern/game/runtime.h"
namespace cases {
/// A state that asks its world of the engine for the logic its Blueprints run in (GameWorld::logic_runtime, declared
/// in kern::game as `logic::Runtime*`) and hands it back to the engine (kern::game::blueprints_running).
class Runner {
 public:
  kern::i32 running() const;
 private:
  std::unique_ptr<kern::game::GameWorld> world_;
};
Runner& runner();
}
