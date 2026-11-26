// Traffic rules: junctions from the lanes, traffic lights with two phase groups, first come first
// served at unsignalled junctions, braking for people in the corridor. The rules are the level's Blueprint
// world_traffic_cpp, called as its class - as the program calls it (level_blueprints.h) - beside the level's
// Traffic Blueprint whose laws it takes.
#include <string>

#include "../src/blueprint_call.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

namespace {
using Tr = blueprints::world_traffic_cpp;
/// world_traffic_cpp running as its class, beside the level's Traffic Blueprint.
struct TrafficBlueprint {
  kern::test::BlueprintRun rules{"Traffic", nullptr, true, {"world_traffic_cpp"}};
  bool bound = false;
  TrafficBlueprint() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Tr>(rules.rt);
  }
  Tr& operator*() { return kern::logic::blueprint<Tr>(); }
};
Tr::Lane lane(std::initializer_list<Vec3> points, f32 width) {
  Tr::Lane l;
  for (Vec3 p : points) l.points.push_back(p);
  l.width = width;
  l.tile = Tr::Int64{1};
  l.alive = true;
  return l;
}
Tr::List<Tr::Lane> streets() {
  Tr::List<Tr::Lane> l;
  l.push_back(lane({{-100, 0, 0}, {0, 0, 0}, {100, 0, 0}}, 11));     // a main road across ...
  l.push_back(lane({{0, -100, 0}, {0, 0, 0}, {0, 100, 0}}, 11));     // ... another: traffic lights
  l.push_back(lane({{300, -100, 0}, {300, 0, 0}, {300, 100, 0}}, 6));  // a side street ...
  l.push_back(lane({{300, 0, 0}, {400, 0, 0}}, 6));                    // ... with a T: no lights
  return l;
}
const Tr::List<Tr::Vector> kNobody;
/// TrafficRules::limit in the C++'s order of arguments.
Vec3 v3(Vec2 p) { return {p.x, p.y, 0}; }
f32 limit(Tr& tr, Tr::TrafficRules& t, i32 car, Vec2 pos, Vec2 fwd, f32 speed, bool bus, f32 now, const Tr::List<Tr::Vector>& cars,
          const Tr::List<Tr::Vector>& people, Vec2 player, bool player_on_foot, Tr::TrafficRules_View& v) {
  return static_cast<f32>(tr.TrafficRules_limit(t, cars, people, v, car, v3(pos), v3(fwd), speed, bus, now, v3(player), player_on_foot).Result);
}
f32 gap_speed(Tr& tr, const Tr::TrafficRules& t, i32 self, Vec2 pos, Vec2 fwd, bool bus, const Tr::List<Tr::TrafficRules_Vehicle>& all, i32& who) {
  Tr::I32 limiter = who;
  const f32 v = static_cast<f32>(tr.TrafficRules_gap_speed(t, all, limiter, self, v3(pos), v3(fwd), bus).Result);
  who = static_cast<i32>(limiter);
  return v;
}
Tr::TrafficRules_Pursuit pursuit(Tr& tr, const Tr::Lane& l, i32 index, i32 dir, Vec2 pos, f32 look, f32 off) {
  return tr.TrafficRules_pursuit(l, index, dir, v3(pos), look, off).Result;
}
Tr::TrafficRules_Vehicle vehicle(Vec2 p, bool bus, bool there) {
  Tr::TrafficRules_Vehicle v;
  v.p = v3(p);
  v.bus = bus;
  v.there = there;
  return v;
}
}  // namespace

#define TRAFFIC_BOUND                                                                       \
  TrafficBlueprint b;                                                                       \
  KERN_CHECK(b.bound, "world_traffic_cpp runs as its class: %s", b.rules.problems.c_str()); \
  if (!b.bound) return;                                                                     \
  Tr& tr = *b

KERN_TEST(traffic_junctions_and_lights) {
  TRAFFIC_BOUND;
  Tr::TrafficRules t;
  tr.TrafficRules_take_laws(t);
  tr.TrafficRules_rebuild(t, streets());
  KERN_CHECK(t.junctions_.size() == 2, "two junctions (%zu)", t.junctions_.size());
  if (t.junctions_.size() != 2) return;
  const Tr::TrafficRules_Junction* cross = &t.junctions_[0];
  const Tr::TrafficRules_Junction* tee = &t.junctions_[1];
  if (cross->arms.size() != 4) std::swap(cross, tee);
  KERN_CHECK(cross->arms.size() == 4 && cross->lights, "the main crossing has 4 arms and lights");
  KERN_CHECK(tee->arms.size() == 3 && !tee->lights, "the T junction has 3 arms, no lights");
  // The two directions never have green at the same time, and both get green in a cycle.
  bool green_x = false, green_y = false;
  for (f32 now = 0; now < 40; now += 0.25f) {
    const i32 lx = static_cast<i32>(tr.TrafficRules_light(t, *cross, {1, 0, 0}, now).Result), ly = static_cast<i32>(tr.TrafficRules_light(t, *cross, {0, 1, 0}, now).Result);
    KERN_CHECK(!(lx == 0 && ly == 0), "not green both ways at %.2f s", now);
    green_x |= lx == 0;
    green_y |= ly == 0;
  }
  KERN_CHECK(green_x && green_y, "both ways get green");
  // A car 30 m before the crossing: stops on red, drives on green.
  Tr::List<Tr::Vector> cars{{-30, -2, 0}};
  f32 red_t = -1, green_t = -1;
  for (f32 now = 0; now < 40 && (red_t < 0 || green_t < 0); now += 0.5f) {
    const i32 l = static_cast<i32>(tr.TrafficRules_light(t, *cross, {1, 0, 0}, now).Result);
    if (l == 2 && red_t < 0) red_t = now;
    if (l == 0 && green_t < 0) green_t = now;
  }
  Tr::TrafficRules_View v;
  const f32 on_red = limit(tr, t, 0, {-30, -2}, {1, 0}, 10, false, red_t, cars, kNobody, {}, false, v);
  KERN_CHECK(v.light == 2 && v.wait && on_red < 12, "red: brake to the stop line (%.1f m/s, %.1f m)", on_red, v.distance);
  const f32 at_line = limit(tr, t, 0, {static_cast<f32>(-cross->radius - 1.2f), -2}, {1, 0}, 3, false, red_t, cars, kNobody, {}, false, v);
  KERN_CHECK(at_line < 1, "red: stand at the line (%.2f)", at_line);
  const f32 on_green = limit(tr, t, 0, {-30, -2}, {1, 0}, 10, false, green_t, cars, kNobody, {}, false, v);
  KERN_CHECK(on_green > 100, "green: free (%.0f)", on_green);
}

KERN_TEST(traffic_first_come_and_people) {
  TRAFFIC_BOUND;
  Tr::TrafficRules t;
  tr.TrafficRules_take_laws(t);
  tr.TrafficRules_rebuild(t, streets());
  // Car 0 is inside the T junction, car 1 arrives from the south: it waits.
  Tr::List<Tr::Vector> cars{{301, 1, 0}, {302, -14, 0}};
  Tr::TrafficRules_View v;
  limit(tr, t, 0, {301, 1}, {1, 0}, 5, false, 10, cars, kNobody, {}, false, v);
  const f32 l1 = limit(tr, t, 1, {302, -14}, {0, 1}, 6, false, 10.1f, cars, kNobody, {}, false, v);
  KERN_CHECK(v.wait && l1 < 7.5f, "waits for the car in the junction (%.1f m/s)", l1);
  // Car 0 has left: free.
  cars[0] = {340, -2, 0};
  limit(tr, t, 0, {340, -2}, {1, 0}, 5, false, 11, cars, kNobody, {}, false, v);
  const f32 l2 = limit(tr, t, 1, {302, -14}, {0, 1}, 6, false, 11.1f, cars, kNobody, {}, false, v);
  KERN_CHECK(!v.wait && l2 > 100, "then it goes (%.0f)", l2);
  // Somebody in the corridor: stop; the player standing there makes the driver honk.
  const Tr::List<Tr::Vector> walker{{-50, -1.5f, 0}};
  const f32 l3 = limit(tr, t, 0, {-60, -2}, {1, 0}, 8, false, 20, cars, walker, {}, false, v);
  KERN_CHECK(l3 == 0 && v.yielding, "brakes for a person in its lane");
  bool honked = false;
  for (f32 now = 30; now < 36; now += 0.2f) {
    limit(tr, t, 0, {-60, -2}, {1, 0}, 0, false, now, cars, kNobody, {-55, -2}, true, v);
    honked |= v.honk;
  }
  KERN_CHECK(honked, "honks at the player standing in the road");
}

KERN_TEST(traffic_keeps_its_distance) {
  TRAFFIC_BOUND;
  Tr::TrafficRules t;
  tr.TrafficRules_take_laws(t);
  // driveSpeed: the nearest vehicle in the own corridor ahead - the centres' distance less both
  // half lengths less 2.5 m, at 1.6 s headway; a speed to aim for (0 = stop), never a reverse
  Tr::List<Tr::TrafficRules_Vehicle> all{vehicle({0, 0}, false, true), vehicle({20, 0.5f}, false, true), vehicle({30, 0}, true, true),
                                          vehicle({10, 4}, false, true), vehicle({-8, 0}, false, true), vehicle({12, 0}, false, false)};
  i32 who = -2;
  // car 0 heading +x: car 1 is 20 m ahead (gap 20 - 3.8 = 16.2, free 13.7 -> 8.56 m/s); the car
  // beside (4 m to the left), the one behind and the one that is not there do not count
  f32 v = gap_speed(tr, t, 0, {0, 0}, {1, 0}, false, all, who);
  KERN_CHECK(std::fabs(v - (20 - 3.8f - 2.5f) / 1.6f) < 1e-3f && who == 1, "behind a car 20 m ahead (%.2f m/s, limiter %d)", v, who);
  // the bus one further: behind it car 1 has 10 - (1.9 + 4) = 4.1 m -> 1 m/s
  v = gap_speed(tr, t, 1, {20, 0.5f}, {1, 0}, false, all, who);
  KERN_CHECK(std::fabs(v - (10 - 5.9f - 2.5f) / 1.6f) < 1e-3f && who == 2, "behind the bus (%.2f m/s, limiter %d)", v, who);
  // closer than 2.5 m bumper to bumper: stop
  all[1].p = {6, 0, 0};
  v = gap_speed(tr, t, 0, {0, 0}, {1, 0}, false, all, who);
  KERN_CHECK(v == 0 && who == 1, "bumper to bumper: stop (%.2f)", v);
  // nobody ahead: free
  v = gap_speed(tr, t, 2, {30, 0}, {1, 0}, true, all, who);
  KERN_CHECK(v > 1e8f && who == -1, "nobody ahead: free (%.0f, %d)", v, who);
  // a car looks 1.25 + 0.6 m to each side, a bus 1.6 + 0.6 m
  all[1].p = {20, 2.0f, 0};
  all[2].there = false;
  KERN_CHECK(gap_speed(tr, t, 0, {0, 0}, {1, 0}, false, all, who) > 1e8f, "2 m to the side: not in a car's way");
  KERN_CHECK(gap_speed(tr, t, 0, {0, 0}, {1, 0}, true, all, who) < 10, "2 m to the side: in a bus' way");
}

KERN_TEST(traffic_pursuit_point) {
  TRAFFIC_BOUND;
  // the driver's target: `look` metres on along the lane from where the car is, on its right
  const Tr::Lane l = lane({{0, 0, 0}, {100, 0, 0}, {100, 50, 0}}, 8);
  // on the first stretch, 30 m along, 3 m beside the line: 10 m on, 2 m to the right (-y)
  Tr::TrafficRules_Pursuit p = pursuit(tr, l, 1, 1, {30, 3}, 10, 2);
  KERN_CHECK(std::fabs(p.target.x - 40) < 1e-3f && std::fabs(p.target.y + 2) < 1e-3f && !p.passed, "ahead on the stretch (%.1f, %.1f)", p.target.x, p.target.y);
  KERN_CHECK(std::fabs(p.end - 120) < 1e-3f, "the lane's end is 120 m on (%.1f)", p.end);
  // 4 m before the bend: 6 m of the 10 are round it (on the second stretch, right of it is +x)
  p = pursuit(tr, l, 1, 1, {96, 0}, 10, 2);
  KERN_CHECK(std::fabs(p.target.x - 102) < 1e-3f && std::fabs(p.target.y - 6) < 1e-3f && !p.passed, "round the bend (%.1f, %.1f)", p.target.x, p.target.y);
  // at the bend: the lane point ahead is reached
  p = pursuit(tr, l, 1, 1, {99.8f, 0.5f}, 10, 2);
  KERN_CHECK(p.passed, "at the lane point");
  // never beyond the lane's end
  p = pursuit(tr, l, 2, 1, {100, 45}, 10, 0);
  KERN_CHECK(std::fabs(p.target.x - 100) < 1e-3f && std::fabs(p.target.y - 50) < 1e-3f && std::fabs(p.end - 5) < 1e-3f, "the end holds the target (%.1f, %.1f)", p.target.x, p.target.y);
  // the other way: from the end back, the right side is the other one
  p = pursuit(tr, l, 1, -1, {100, 20}, 10, 2);
  KERN_CHECK(std::fabs(p.target.x - 98) < 1e-3f && std::fabs(p.target.y - 10) < 1e-3f, "driving back (%.1f, %.1f)", p.target.x, p.target.y);
}
