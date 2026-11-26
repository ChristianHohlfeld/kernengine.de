#pragma once
#include <memory>
#include "kern/app/app.h"
#include "kern/game/runtime.h"
namespace cases {
/// The state of a program that runs a world of its own (one the program makes - the program's Blueprints are none of
/// its actors) and calls the Blueprint of the actor Lamps of that world's level (worldrules.cpp: an ActorBlueprint bound
/// to the world - in the Blueprint the variable that holds the Actor of it).
class Runner {
 public:
  bool start(kern::Engine& engine);
  float pole(float r) const;
  bool lit() const;
 private:
  std::unique_ptr<kern::game::GameWorld> world_;
};
Runner& runner();
namespace worldrules {
bool bind(kern::game::GameWorld& world);
double radius();
double twice(double x);
}
}
