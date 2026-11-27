#include "worldrules.h"
#include <tuple>
#include "kern/logic/actor_blueprint.h"
namespace cases {
namespace worldrules {
namespace {
/// The Blueprint of the actor Lamps of the world the program runs: bound to that world (bind), not to a logic.
kern::logic::ActorBlueprint g_lamps{"Lamps"};
}
bool bind(kern::game::GameWorld& world) { return g_lamps.bind(world); }
double radius() { return g_lamps.get<double>("PoleRadius"); }
double Stick::step(double x) {
  v = v + g_lamps.number("Twice", x);
  return v;
}
double Pad::go(double x) { return s.step(x) + 1; }
double twice(double x) {
  double r = 0;
  g_lamps.call("Twice", std::tie(r), x);
  return r;
}
}
bool Runner::start(kern::Engine& engine) {
  world_ = std::make_unique<kern::game::GameWorld>(engine);
  return worldrules::bind(*world_);
}
float Runner::pole(float r) const { return static_cast<float>(worldrules::radius() + worldrules::twice(r)); }
bool Runner::lit() const { return worldrules::radius() > 0.1; }
double Runner::press(double x) { return pad_.go(x); }
float Runner::hz() const { return world_->step_hz(); }
Runner& runner() { static Runner m; return m; }
}
