#pragma once
#include <memory>
#include "kern/app/app.h"
#include "kern/game/runtime.h"
#include "worldpads.h"
namespace cases {
/// The program's App that keeps the game's state itself and runs a world of its own; the game's input maths stands in a
/// second file (worldpads.cpp) as free functions that call the Blueprint of an actor of that world.
class Flight final : public kern::App {
 public:
  bool start(kern::Engine& engine) override;
  void update(kern::Engine&, const kern::Frame&) override;
  double reach(double x) const;
  double steer(double x);
  double spread(double x) const;

 private:
  std::unique_ptr<kern::game::GameWorld> world_;
  worldpads::Axis axis_;
  double last_ = 0;
};
Flight& flight();
}
