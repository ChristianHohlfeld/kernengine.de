#pragma once
#include <memory>
#include "kern/app/app.h"
#include "kern/game/runtime.h"
namespace cases {
namespace worldrules {
/// A value of the program whose method calls the Blueprint of the world's actor itself (a game's LookAxis::step): a function
/// of the program's Blueprint, its object its first input.
struct Stick {
  double v = 0;
  double step(double x);
};
/// ... and one whose method calls that method (so a function of the program's Blueprint too).
struct Pad {
  Stick s;
  double go(double x);
};
}
/// The state of a program that runs a world of its own (one the program makes - the program's Blueprints are none of
/// its actors) and calls the Blueprint of the actor Lamps of that world's level (worldrules.cpp: an ActorBlueprint bound
/// to the world - in the Blueprint the variable that holds the Actor of it).
class Runner {
 public:
  bool start(kern::Engine& engine);
  float pole(float r) const;
  bool lit() const;
  /// How often the world steps per second (GameWorld::step_hz - its node).
  float hz() const;
  double press(double x);
 private:
  std::unique_ptr<kern::game::GameWorld> world_;
  worldrules::Pad pad_;
};
Runner& runner();
namespace worldrules {
bool bind(kern::game::GameWorld& world);
double radius();
double twice(double x);
}
}
