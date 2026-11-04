// The town: street graph routes for the police, people hit by bullets and cars, crimes they report.
#include "../src/people.h"
#include "../src/road_graph.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

SC_TEST(town_road_graph_route) {
  // A grid of two parallel streets joined at both ends; the route goes round the block.
  std::vector<Lane> lanes;
  lanes.push_back(Lane{{{0, 0}, {50, 0}, {100, 0}}, 6, 1, true});
  lanes.push_back(Lane{{{100, 0}, {100, 60}}, 6, 1, true});
  lanes.push_back(Lane{{{0, 60}, {100, 60}}, 6, 1, true});
  lanes.push_back(Lane{{{0, 0.5f}, {0, 60}}, 6, 1, true});  // joins within the merge distance
  RoadGraph g;
  g.rebuild(lanes);
  SC_CHECK(g.size() == 5, "five street nodes (%zu)", g.size());
  std::vector<Vec2> path;
  SC_CHECK(g.path({50, 1}, {50, 59}, path), "a route exists");
  SC_CHECK(path.size() >= 4, "round the block (%zu points)", path.size());
  const Vec2 ahead = RoadGraph::pursuit_point(path, {50, 1}, 20);
  SC_CHECK(std::fabs(ahead.y) < 2 && std::fabs(ahead.x - 50) > 15, "pure pursuit point stays on the street (%.1f %.1f)", ahead.x, ahead.y);
  lanes[1].alive = lanes[3].alive = false;
  SC_CHECK(lane_signature(lanes) != g.built_from, "the signature sees dead lanes");
  g.rebuild(lanes);
  SC_CHECK(!g.path({50, 1}, {50, 59}, path), "no route once the cross streets are gone");
}

SC_TEST(town_people_hits_and_crimes) {
  kern::physics::World w;
  make_flat_world(w);
  People people;
  // An officer is a person like any other for bullets: four Glock hits (30) or two to the head.
  const i32 cop = people.add_officer({10, 0, 0}, 0, 0, 7);
  Vec3 to{30, 0, 1.2f};
  SC_CHECK(people.shot({0, 0, 1.2f}, to, 30, true, 1) == cop, "the bullet hits the officer");
  SC_CHECK(std::fabs(to.x - (10 - 0.32f)) < 0.05f, "the shot ends at his body (%.2f)", to.x);
  SC_CHECK(people.at(cop).hp == 70 && people.at(cop).state != People::State::down, "a body hit takes 30 of 100");
  to = {30, 0, 1.62f};
  people.shot({0, 0, 1.62f}, to, 30, true, 2);
  SC_CHECK(people.at(cop).state == People::State::down, "a headshot (x3) finishes him");
  std::vector<CrimeReport> reports;
  people.take_reports(reports);
  SC_CHECK(reports.size() == 1 && reports[0].crime == wanted::Crime::police_officer && reports[0].witnessed, "killing an officer is always seen");
  // A miss above the head, and a dead body is not hit again.
  to = {30, 0, 2.3f};
  SC_CHECK(people.shot({0, 0, 2.3f}, to, 30, true, 3) < 0, "over the head: no hit");
  SC_CHECK(people.count(People::Kind::officer) == 0 && people.count(People::Kind::officer, false) == 1, "the body stays");
}

SC_TEST(town_people_flee_and_walls) {
  kern::physics::World w;
  make_flat_world(w);
  make_static_box(w, {5, 0, 2}, {1, 60, 4});  // a long wall 4.5 m east
  People people;
  const i32 p = people.add_officer({0, 0, 0}, 0, 0, 3);
  People::Person& who = people.at(p);
  // free_move never passes the wall: walking east it slides along it instead.
  for (int i = 0; i < 200; ++i) people.free_move(w, who, {1, 0}, 0.05f, false);
  SC_CHECK(who.pos.x < 4.4f, "stopped at the wall (x %.2f)", who.pos.x);
  SC_CHECK(std::fabs(who.pos.y) > 0.5f, "slid along it (y %.2f)", who.pos.y);
}
