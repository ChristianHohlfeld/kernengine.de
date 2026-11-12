// Traffic rules: junctions from the lanes, traffic lights with two phase groups, first come first
// served at unsignalled junctions, braking for people in the corridor.
#include "../src/world_traffic.h"

#include "rule_blueprint.h"
#include "test_util.h"

using namespace sc;

namespace {
std::vector<Lane> streets() {
  std::vector<Lane> l;
  l.push_back(Lane{{{-100, 0}, {0, 0}, {100, 0}}, 11, 1, true});   // a main road across ...
  l.push_back(Lane{{{0, -100}, {0, 0}, {0, 100}}, 11, 1, true});   // ... another: traffic lights
  l.push_back(Lane{{{300, -100}, {300, 0}, {300, 100}}, 6, 1, true});  // a side street ...
  l.push_back(Lane{{{300, 0}, {400, 0}}, 6, 1, true});                // ... with a T: no lights
  return l;
}
const std::vector<Vec2> kNobody;
}  // namespace

SC_TEST(traffic_junctions_and_lights) {
  RuleBlueprint traffic_rules("Traffic", &TrafficRules::bind);
  TrafficRules t;
  t.rebuild(streets());
  SC_CHECK(t.junctions().size() == 2, "two junctions (%zu)", t.junctions().size());
  if (t.junctions().size() != 2) return;
  const TrafficRules::Junction* cross = &t.junctions()[0];
  const TrafficRules::Junction* tee = &t.junctions()[1];
  if (cross->arms.size() != 4) std::swap(cross, tee);
  SC_CHECK(cross->arms.size() == 4 && cross->lights, "the main crossing has 4 arms and lights");
  SC_CHECK(tee->arms.size() == 3 && !tee->lights, "the T junction has 3 arms, no lights");
  // The two directions never have green at the same time, and both get green in a cycle.
  bool green_x = false, green_y = false;
  for (f32 now = 0; now < 40; now += 0.25f) {
    const i32 lx = t.light(*cross, {1, 0}, now), ly = t.light(*cross, {0, 1}, now);
    SC_CHECK(!(lx == 0 && ly == 0), "not green both ways at %.2f s", now);
    green_x |= lx == 0;
    green_y |= ly == 0;
  }
  SC_CHECK(green_x && green_y, "both ways get green");
  // A car 30 m before the crossing: stops on red, drives on green.
  std::vector<Vec3> cars{{-30, -2, 0}};
  f32 red_t = -1, green_t = -1;
  for (f32 now = 0; now < 40 && (red_t < 0 || green_t < 0); now += 0.5f) {
    const i32 l = t.light(*cross, {1, 0}, now);
    if (l == 2 && red_t < 0) red_t = now;
    if (l == 0 && green_t < 0) green_t = now;
  }
  TrafficRules::View v;
  const f32 on_red = t.limit(0, {-30, -2}, {1, 0}, 10, false, red_t, cars, kNobody, {}, false, v);
  SC_CHECK(v.light == 2 && v.wait && on_red < 12, "red: brake to the stop line (%.1f m/s, %.1f m)", on_red, v.distance);
  const f32 at_line = t.limit(0, {-cross->radius - 1.2f, -2}, {1, 0}, 3, false, red_t, cars, kNobody, {}, false, v);
  SC_CHECK(at_line < 1, "red: stand at the line (%.2f)", at_line);
  const f32 on_green = t.limit(0, {-30, -2}, {1, 0}, 10, false, green_t, cars, kNobody, {}, false, v);
  SC_CHECK(on_green > 100, "green: free (%.0f)", on_green);
}

SC_TEST(traffic_first_come_and_people) {
  RuleBlueprint traffic_rules("Traffic", &TrafficRules::bind);
  TrafficRules t;
  t.rebuild(streets());
  // Car 0 is inside the T junction, car 1 arrives from the south: it waits.
  std::vector<Vec3> cars{{301, 1, 0}, {302, -14, 0}};
  TrafficRules::View v;
  t.limit(0, {301, 1}, {1, 0}, 5, false, 10, cars, kNobody, {}, false, v);
  const f32 l1 = t.limit(1, {302, -14}, {0, 1}, 6, false, 10.1f, cars, kNobody, {}, false, v);
  SC_CHECK(v.wait && l1 < 7.5f, "waits for the car in the junction (%.1f m/s)", l1);
  // Car 0 has left: free.
  cars[0] = {340, -2, 0};
  t.limit(0, {340, -2}, {1, 0}, 5, false, 11, cars, kNobody, {}, false, v);
  const f32 l2 = t.limit(1, {302, -14}, {0, 1}, 6, false, 11.1f, cars, kNobody, {}, false, v);
  SC_CHECK(!v.wait && l2 > 100, "then it goes (%.0f)", l2);
  // Somebody in the corridor: stop; the player standing there makes the driver honk.
  const std::vector<Vec2> walker{{-50, -1.5f}};
  const f32 l3 = t.limit(0, {-60, -2}, {1, 0}, 8, false, 20, cars, walker, {}, false, v);
  SC_CHECK(l3 == 0 && v.yielding, "brakes for a person in its lane");
  bool honked = false;
  for (f32 now = 30; now < 36; now += 0.2f) {
    t.limit(0, {-60, -2}, {1, 0}, 0, false, now, cars, kNobody, {-55, -2}, true, v);
    honked |= v.honk;
  }
  SC_CHECK(honked, "honks at the player standing in the road");
}

SC_TEST(traffic_keeps_its_distance) {
  RuleBlueprint traffic_rules("Traffic", &TrafficRules::bind);
  // driveSpeed: the nearest vehicle in the own corridor ahead - the centres' distance less both
  // half lengths less 2.5 m, at 1.6 s headway; a speed to aim for (0 = stop), never a reverse
  using V = TrafficRules::Vehicle;
  std::vector<V> all{{{0, 0}, false, true}, {{20, 0.5f}, false, true}, {{30, 0}, true, true}, {{10, 4}, false, true}, {{-8, 0}, false, true}, {{12, 0}, false, false}};
  i32 who = -2;
  // car 0 heading +x: car 1 is 20 m ahead (gap 20 - 3.8 = 16.2, free 13.7 -> 8.56 m/s); the car
  // beside (4 m to the left), the one behind and the one that is not there do not count
  f32 v = TrafficRules::gap_speed(0, {0, 0}, {1, 0}, false, all, &who);
  SC_CHECK(std::fabs(v - (20 - 3.8f - 2.5f) / 1.6f) < 1e-3f && who == 1, "behind a car 20 m ahead (%.2f m/s, limiter %d)", v, who);
  // the bus one further: behind it car 1 has 10 - (1.9 + 4) = 4.1 m -> 1 m/s
  v = TrafficRules::gap_speed(1, {20, 0.5f}, {1, 0}, false, all, &who);
  SC_CHECK(std::fabs(v - (10 - 5.9f - 2.5f) / 1.6f) < 1e-3f && who == 2, "behind the bus (%.2f m/s, limiter %d)", v, who);
  // closer than 2.5 m bumper to bumper: stop
  all[1].p = {6, 0};
  v = TrafficRules::gap_speed(0, {0, 0}, {1, 0}, false, all, &who);
  SC_CHECK(v == 0 && who == 1, "bumper to bumper: stop (%.2f)", v);
  // nobody ahead: free
  v = TrafficRules::gap_speed(2, {30, 0}, {1, 0}, true, all, &who);
  SC_CHECK(v > 1e8f && who == -1, "nobody ahead: free (%.0f, %d)", v, who);
  // a car looks 1.25 + 0.6 m to each side, a bus 1.6 + 0.6 m
  all[1].p = {20, 2.0f};
  all[2].there = false;
  SC_CHECK(TrafficRules::gap_speed(0, {0, 0}, {1, 0}, false, all) > 1e8f, "2 m to the side: not in a car's way");
  SC_CHECK(TrafficRules::gap_speed(0, {0, 0}, {1, 0}, true, all) < 10, "2 m to the side: in a bus' way");
}

SC_TEST(traffic_pursuit_point) {
  RuleBlueprint traffic_rules("Traffic", &TrafficRules::bind);
  // the driver's target: `look` metres on along the lane from where the car is, on its right
  Lane l{{{0, 0}, {100, 0}, {100, 50}}, 8, 1, true};
  // on the first stretch, 30 m along, 3 m beside the line: 10 m on, 2 m to the right (-y)
  TrafficRules::Pursuit p = TrafficRules::pursuit(l, 1, 1, {30, 3}, 10, 2);
  SC_CHECK(std::fabs(p.target.x - 40) < 1e-3f && std::fabs(p.target.y + 2) < 1e-3f && !p.passed, "ahead on the stretch (%.1f, %.1f)", p.target.x, p.target.y);
  SC_CHECK(std::fabs(p.end - 120) < 1e-3f, "the lane's end is 120 m on (%.1f)", p.end);
  // 4 m before the bend: 6 m of the 10 are round it (on the second stretch, right of it is +x)
  p = TrafficRules::pursuit(l, 1, 1, {96, 0}, 10, 2);
  SC_CHECK(std::fabs(p.target.x - 102) < 1e-3f && std::fabs(p.target.y - 6) < 1e-3f && !p.passed, "round the bend (%.1f, %.1f)", p.target.x, p.target.y);
  // at the bend: the lane point ahead is reached
  p = TrafficRules::pursuit(l, 1, 1, {99.8f, 0.5f}, 10, 2);
  SC_CHECK(p.passed, "at the lane point");
  // never beyond the lane's end
  p = TrafficRules::pursuit(l, 2, 1, {100, 45}, 10, 0);
  SC_CHECK(std::fabs(p.target.x - 100) < 1e-3f && std::fabs(p.target.y - 50) < 1e-3f && std::fabs(p.end - 5) < 1e-3f, "the end holds the target (%.1f, %.1f)", p.target.x, p.target.y);
  // the other way: from the end back, the right side is the other one
  p = TrafficRules::pursuit(l, 1, -1, {100, 20}, 10, 2);
  SC_CHECK(std::fabs(p.target.x - 98) < 1e-3f && std::fabs(p.target.y - 10) < 1e-3f, "driving back (%.1f, %.1f)", p.target.x, p.target.y);
}
