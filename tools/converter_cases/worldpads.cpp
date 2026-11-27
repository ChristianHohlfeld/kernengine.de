#include "worldpads.h"
#include <tuple>
namespace cases::worldpads {
namespace {
Rules g_rules{"Lamps"};
}
bool bind(kern::game::GameWorld& world) { return g_rules.bind(world); }
double radius() { return g_rules.get<double>("PoleRadius"); }
double twice(double x) { return g_rules.number("Twice", x); }
void both(double x, double& a, double& b) {
  double r = 0;
  g_rules.call("Twice", std::tie(r), x);
  a = r;
  b = r + radius();
}
double wide(double x) { return twice(x) + radius(); }
double Axis::step(double x) {
  v = v + g_rules.number("Twice", x);
  return v;
}
}
