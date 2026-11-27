#include "worldapp.h"
#include "kern/api/c_api.h"
#include "kern/app/run.h"
namespace cases {
bool Flight::start(kern::Engine& engine) {
  world_ = std::make_unique<kern::game::GameWorld>(engine);
  return worldpads::bind(*world_);
}
void Flight::update(kern::Engine&, const kern::Frame&) { last_ = worldpads::twice(last_ + 1); }
double Flight::reach(double x) const { return worldpads::radius() + worldpads::wide(x) + last_; }
double Flight::steer(double x) { return axis_.step(x); }
double Flight::spread(double x) const {
  double a = 0, b = 0;
  worldpads::both(x, a, b);
  return a - b;
}
Flight& flight() {
  static Flight f;
  return f;
}
}
/// What the host asks the game: exports that reach the App's state, which calls the functions of the second file.
KERN_EXPORT(case_reach) double case_reach(double x) { return cases::flight().reach(x); }
KERN_EXPORT(case_steer) double case_steer(double x) { return cases::flight().steer(x); }
int kern_main() { return kern::run(cases::flight()); }
