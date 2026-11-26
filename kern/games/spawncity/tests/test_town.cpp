// The town: street graph routes for the police, people hit by bullets and cars, crimes they report. The street
// graph and the people are the level's Blueprints road_graph_cpp and people_cpp, called as their classes - as the
// program calls them (level_blueprints.h); the people on the engine itself, the physics world the test's.
#include <string>

#include "../src/blueprint_call.h"
#include "kern/physics/physics.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"
#include "world_util.h"

using namespace sc;

namespace {
using Rg = blueprints::road_graph_cpp;
using Pp = blueprints::people_cpp;
/// A Blueprint of the level running as its class, on the engine itself (kern::test::BlueprintRun::Engine::direct).
template <class T>
struct ClassBlueprint {
  kern::test::BlueprintRun rules;
  bool bound = false;
  explicit ClassBlueprint(const char* actor) : rules(actor, nullptr, true, {}, kern::test::BlueprintRun::Engine::direct) {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<T>(rules.rt);
  }
  T& operator*() { return kern::logic::blueprint<T>(); }
};
Rg::Lane lane(std::initializer_list<Vec3> points) {
  Rg::Lane l;
  for (Vec3 p : points) l.points.push_back(p);
  l.width = 6;
  l.tile = Rg::Int64{1};
  l.alive = true;
  return l;
}
/// People::shot with the C++'s defaults written out (melee_ns -1, self_r 0.5).
i32 shot(Pp& pp, Pp::People& people, Vec3 from, Vec3& to, f32 damage, bool by_player, f32 now, f32 melee_ns = -1) {
  return static_cast<i32>(pp.People_shot(people, to, from, damage, by_player, now, melee_ns, 0.5f).Result);
}
}  // namespace

KERN_TEST(town_road_graph_route) {
  ClassBlueprint<Rg> b("road_graph_cpp");
  KERN_CHECK(b.bound, "road_graph_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Rg& rg = *b;
  // A grid of two parallel streets joined at both ends; the route goes round the block.
  Rg::List<Rg::Lane> lanes;
  lanes.push_back(lane({{0, 0, 0}, {50, 0, 0}, {100, 0, 0}}));
  lanes.push_back(lane({{100, 0, 0}, {100, 60, 0}}));
  lanes.push_back(lane({{0, 60, 0}, {100, 60, 0}}));
  lanes.push_back(lane({{0, 0.5f, 0}, {0, 60, 0}}));  // joins within the merge distance
  Rg::RoadGraph g;
  rg.RoadGraph_rebuild(g, lanes);
  const std::size_t size = static_cast<std::size_t>(rg.RoadGraph_size(g).Result);
  KERN_CHECK(size == 5, "five street nodes (%zu)", size);
  Rg::List<Rg::Vector> path;
  KERN_CHECK(rg.RoadGraph_path(g, path, {50, 1, 0}, {50, 59, 0}).Result, "a route exists");
  KERN_CHECK(path.size() >= 4, "round the block (%zu points)", static_cast<std::size_t>(path.size()));
  const Vec3 ahead = rg.RoadGraph_pursuit_point(path, {50, 1, 0}, 20).Result;
  KERN_CHECK(std::fabs(ahead.y) < 2 && std::fabs(ahead.x - 50) > 15, "pure pursuit point stays on the street (%.1f %.1f)", static_cast<f64>(ahead.x), static_cast<f64>(ahead.y));
  lanes[1].alive = lanes[3].alive = false;
  KERN_CHECK(rg.lane_signature(lanes).Result != g.built_from, "the signature sees dead lanes");
  rg.RoadGraph_rebuild(g, lanes);
  KERN_CHECK(!rg.RoadGraph_path(g, path, {50, 1, 0}, {50, 59, 0}).Result, "no route once the cross streets are gone");
}

KERN_TEST(town_people_hits_and_crimes) {
  ClassBlueprint<Pp> b("people_cpp");
  KERN_CHECK(b.bound, "people_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Pp& pp = *b;
  kern::physics::World w;
  make_flat_world(w);
  Pp::People people;
  auto at = [&](i32 i) -> Pp::People_Person& { return people.people_[static_cast<std::size_t>(i)]; };
  using State = Pp::People_State;
  using Kind = Pp::People_Kind;
  // An officer (police_ground_units.mjs): 100 HP, 25 per counted hit - an MP round counts once
  // (head 3 times), a Glock round 6 times.
  const i32 cop = static_cast<i32>(pp.People_add_officer(people, {10, 0, 0}, 0, 0, 7).Result);
  Vec3 to{30, 0, 1.2f};
  KERN_CHECK(shot(pp, people, {0, 0, 1.2f}, to, 13, true, 1) == cop, "the bullet hits the officer");
  KERN_CHECK(std::fabs(to.x - (10 - 0.15f * 1.08f)) < 0.05f, "the shot ends at his body - the torso front as drawn (%.2f)", static_cast<f64>(to.x));
  KERN_CHECK(at(cop).hp == 75 && at(cop).state != State::down, "an MP body hit takes 25 of 100");
  to = {30, 0, 1.62f};
  shot(pp, people, {0, 0, 1.62f}, to, 13, true, 2);
  KERN_CHECK(at(cop).state == State::down, "an MP headshot (3 x 25) finishes him");
  Pp::List<Pp::CrimeReport> reports;
  pp.People_take_reports(people, reports);
  KERN_CHECK(reports.size() == 1 && reports[0].crime == Pp::wanted_Crime::police_officer && reports[0].witnessed, "killing an officer is always seen");
  // A miss above the head, and a dead body is not hit again.
  to = {30, 0, 2.3f};
  KERN_CHECK(shot(pp, people, {0, 0, 2.3f}, to, 30, true, 3) < 0, "over the head: no hit");
  KERN_CHECK(pp.People_count(people, Kind::officer, true).Result == 0 && pp.People_count(people, Kind::officer, false).Result == 1, "the body stays");
}

KERN_TEST(town_passerby_hit_model) {
  ClassBlueprint<Pp> b("people_cpp");
  KERN_CHECK(b.bound, "people_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Pp& pp = *b;
  // world_procedural_population.mjs: 100 hp, a round 55 knocks down, lying there no target, the
  // next round after getting up kills; headshot 165; fists stagger, at 35 down
  Pp::People people;
  auto at = [&](i32 i) -> Pp::People_Person& { return people.people_[static_cast<std::size_t>(i)]; };
  using State = Pp::People_State;
  const i32 a = static_cast<i32>(pp.People_add_citizen(people, {10, 0, 0}, 0, 3).Result);
  Vec3 to{30, 0, 1.2f};
  KERN_CHECK(shot(pp, people, {0, 0, 1.2f}, to, 13, true, 1) == a, "the MP round hits the passer-by");
  KERN_CHECK(at(a).hp == 45 && at(a).state != State::down && at(a).knocked_until > 1, "55 off 100: knocked down, alive (%.0f)", static_cast<f64>(at(a).hp));
  to = {30, 0, 1.2f};
  KERN_CHECK(shot(pp, people, {0, 0, 1.2f}, to, 13, true, 2) < 0, "lying there: no target");
  pp.People_stand_up(people, a, {10, 0, 0}, 0);
  to = {30, 0, 1.2f};
  shot(pp, people, {0, 0, 1.2f}, to, 13, true, 10);
  KERN_CHECK(at(a).state == State::down, "the second round kills");
  const i32 bb = static_cast<i32>(pp.People_add_citizen(people, {10, 5, 0}, 0, 4).Result);
  to = {30, 5, 1.62f};
  shot(pp, people, {0, 5, 1.62f}, to, 13, true, 11);
  KERN_CHECK(at(bb).state == State::down, "an MP headshot (165) kills");
  const i32 c = static_cast<i32>(pp.People_add_citizen(people, {1.5f, -5, 0}, 0, 5).Result);
  to = {3, -5, 1.2f};
  shot(pp, people, {0, -5, 1.2f}, to, 24, true, 12, 40);
  KERN_CHECK(at(c).hp == 76 && at(c).knocked_until <= 12 && at(c).pos.x > 1.6f, "a fist staggers (%.0f hp, x %.2f)", static_cast<f64>(at(c).hp), static_cast<f64>(at(c).pos.x));
  to = {3, -5, 1.2f};
  shot(pp, people, {0, -5, 1.2f}, to, 45, true, 13, 12);
  KERN_CHECK(at(c).hp == 31 && at(c).knocked_until > 13, "at 35 or less: down");
}

KERN_TEST(town_people_flee_and_walls) {
  ClassBlueprint<Pp> b("people_cpp");
  KERN_CHECK(b.bound, "people_cpp runs as its class: %s", b.rules.problems.c_str());
  if (!b.bound) return;
  Pp& pp = *b;
  kern::physics::World w;
  make_flat_world(w);
  make_static_box(w, {5, 0, 2}, {1, 60, 4});  // a long wall 4.5 m east
  const i64 world = b.rules.host.objects.adopt(w);
  Pp::People people;
  const i32 p = static_cast<i32>(pp.People_add_officer(people, {0, 0, 0}, 0, 0, 3).Result);
  Pp::People_Person& who = people.people_[static_cast<std::size_t>(p)];
  // free_move never passes the wall: walking east it slides along it instead.
  for (int i = 0; i < 200; ++i) pp.People_free_move(who, world, {1, 0, 0}, 0.05f, false);
  KERN_CHECK(who.pos.x < 4.4f, "stopped at the wall (x %.2f)", static_cast<f64>(who.pos.x));
  KERN_CHECK(std::fabs(who.pos.y) > 0.5f, "slid along it (y %.2f)", static_cast<f64>(who.pos.y));
}
