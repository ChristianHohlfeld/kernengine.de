// sc-m3-gaps: spawn visibility and vehicle clearance (spawn_guard.h), sirens, dispatch, critical
// smoke, organic hit audio (gaps_rules.h).
#include <memory>

#include "../src/gaps_rules.h"
#include "../src/jet_seats.h"
#include "../src/lamps.h"
#include "../src/rules.h"
#include "../src/spawn_guard.h"
#include "../src/world_traffic.h"
#include "kern/game/runtime.h"
#include "kern/kern.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

KERN_TEST(gaps_crossing_junctions) {
  kern::test::BlueprintRun traffic_rules("Traffic", &TrafficRules::bind);
  // two streets laid across each other without a shared vertex still meet in a junction
  std::vector<Lane> lanes(2);
  lanes[0].points = {{-100, 0}, {100, 0}};
  lanes[0].width = 12;
  lanes[1].points = {{0, -100}, {0, 100}};
  lanes[1].width = 6;
  TrafficRules t;
  t.rebuild(lanes);
  KERN_CHECK(t.junctions().size() == 1 && t.junctions()[0].arms.size() == 4 && t.junctions()[0].lights, "junctions %zu", t.junctions().size());
}

namespace {
/// The level's actor "SpawnGuard" running in a game world of its own - the rule asks the world
/// whether buildings hide a point (a ray against the channel "World") - bound to spawn_guard.h.
/// With a wall: a building across the view, 40 m ahead.
struct SpawnGuardWorld {
  kern::Renderer renderer;
  kern::game::NightWindows windows;
  std::unique_ptr<kern::game::GameWorld> world;
  kern::logic::Instance* inst = nullptr;
  std::string problems;
  explicit SpawnGuardWorld(bool wall, bool vehicle_wall = false) {
    kern::ShaderExtension* ext[1] = {&windows};
    kern::RendererOptions o;
    renderer.init(o, kern::Span<kern::ShaderExtension* const>(ext, 1));
    world = std::make_unique<kern::game::GameWorld>(renderer, &windows);
    kern::game::Level level;
    level.world.channels = "World, Vehicle, Drone, Query";
    level.world.ground = false;
    kern::game::Actor guard;
    guard.name = "SpawnGuard";
    guard.logic.emplace();
    std::string error;
    if (!kern::test::read_blueprint("SpawnGuard", *guard.logic, error)) problems = error;
    for (const kern::logic::Issue& i : kern::logic::check(*guard.logic)) problems += (i.error ? "error: " : "warning: ") + i.text + "\n";
    const kern::game::ActorId id = level.add(guard).id;
    if (wall) {
      kern::game::Actor a;
      a.name = "Wall";
      a.transform.position = {40, 0, 30};
      a.mesh.emplace();
      a.mesh->shape = kern::game::Shape::box;
      a.mesh->size = {1, 400, 60};
      if (vehicle_wall) {  // (not a building: a thing of the channel "Vehicle")
        a.mesh->collision = false;
        kern::game::ColliderComponent c;
        c.size = a.mesh->size;
        c.category = 2;
        a.colliders.push_back(c);
      }
      level.add(a);
    }
    if (!world->start(level, kern::game::SimMode::simulate)) problems += "the world does not start\n";
    inst = world->logic_runtime() ? world->logic_runtime()->find(id) : nullptr;
    if (world->logic_runtime()) spawn::bind(*world->logic_runtime());
  }
  ~SpawnGuardWorld() {
    spawn::bind(kern::test::no_logic());
    world.reset();
    renderer.shutdown();
  }
};
}  // namespace

KERN_TEST(spawn_guard_rules) {
  SpawnGuardWorld open(false);
  KERN_CHECK(open.inst && open.problems.empty() && !open.inst->disabled(), "the level's SpawnGuard Blueprint compiles and runs: %s", open.problems.c_str());
  if (!open.inst) return;
  KERN_CHECK(std::string(open.inst->tier()) == "native C++", "the SpawnGuard Blueprint runs as its C++ class (it runs: %s)", open.inst->tier());
  spawn::Viewer v;
  v.eye = {0, 0, 1.7f};
  v.forward = {1, 0, 0};
  v.half_angle = spawn::cone_half_angle(60 * kDeg, 16.0f / 9);
  v.local = true;
  const std::vector<spawn::Viewer> vs{v};
  // the 60 deg vertical / 16:9 view is ~91 deg wide: half 45.7 + 20 deg
  KERN_CHECK(std::fabs(v.half_angle - (45.7f + 20) * kDeg) < 0.6f * kDeg, "half %.2f", v.half_angle / kDeg);
  KERN_CHECK(std::fabs(spawn::default_far() - 1500) < 1e-3f, "the distances are its variables");
  KERN_CHECK(!spawn::can_spawn(vs, {10, -15, 0}, 0, 2.2f), "closer than 24 m is always seen");
  KERN_CHECK(!spawn::can_spawn(vs, {-20, 5, 0}, 0, 2.2f), "behind but within 24 m");
  KERN_CHECK(spawn::can_spawn(vs, {-60, 0, 0}, 0, 2.2f), "behind, far enough");
  KERN_CHECK(!spawn::can_spawn(vs, {80, 20, 0}, 0, 2.2f), "in view");
  KERN_CHECK(spawn::seen_by(v, {80, 20, 0}, 0, 2.2f) && !spawn::seen_by(v, {-60, 0, 0}, 0, 2.2f), "one viewer alone");
  KERN_CHECK(spawn::can_spawn(vs, {1600, 0, 0}, 0, 2.2f), "beyond the far plane");
  KERN_CHECK(spawn::can_spawn(vs, {300, 0, 0}, 250, 2.2f), "far entry allowed in view");
  // a mate looking the other way sees what we do not
  spawn::Viewer mate;
  mate.eye = {0, 0, 1.7f};
  mate.forward = {-1, 0, 0};
  mate.half_angle = v.half_angle;
  KERN_CHECK(!spawn::can_spawn({v, mate}, {-60, 0, 0}, 0, 2.2f), "in the mate's view");
  {
    // a building in between hides it - both the feet and the top
    SpawnGuardWorld town(true);
    KERN_CHECK(town.inst && town.problems.empty(), "(a world with a building: %s)", town.problems.c_str());
    KERN_CHECK(spawn::can_spawn(vs, {80, 20, 0}, 0, 2.2f), "hidden by a building");
    KERN_CHECK(!spawn::can_spawn(vs, {80, 20, 0}, 0, 200.0f), "something taller than the building shows above it");
    KERN_CHECK(!spawn::can_spawn(vs, {500, 0, 0}, 0, 2.2f), "beyond 420 m buildings do not count");
    KERN_CHECK(!spawn::can_spawn(vs, {30, 5, 0}, 0, 2.2f), "in front of the building");
  }
  {
    // only buildings hide: a thing of another collision channel in the way does not
    SpawnGuardWorld traffic(true, true);
    KERN_CHECK(!spawn::can_spawn(vs, {80, 20, 0}, 0, 2.2f), "a vehicle in between hides nothing");
  }
}

KERN_TEST(vehicle_clearance_sat) {
  SpawnGuardWorld bp(false);
  const spawn::Footprint a{{0, 0}, 0, {2.3f, 0.95f}};
  KERN_CHECK(spawn::footprints_overlap(a, {{4.0f, 0}, 0, {2.3f, 0.95f}}), "nose to tail 4 m apart overlap (4.6 m long)");
  KERN_CHECK(!spawn::footprints_overlap(a, {{5.2f, 0}, 0, {2.3f, 0.95f}}), "5.2 m apart: clear with 0.35 margin");
  KERN_CHECK(spawn::footprints_overlap(a, {{4.9f, 0}, 0, {2.3f, 0.95f}}), "4.9 m apart: inside the margin");
  KERN_CHECK(!spawn::footprints_overlap(a, {{0, 2.5f}, 0, {2.3f, 0.95f}}), "side by side 2.5 m");
  KERN_CHECK(spawn::footprints_overlap(a, {{2.5f, 1.4f}, kPi / 2, {2.3f, 0.95f}}), "crossing at a right angle");
}

KERN_TEST(gaps_sirens) {
  kern::test::BlueprintRun bp("Sirens", &gaps::bind_sirens);
  KERN_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Sirens Blueprint compiles and runs: %s", bp.problems.c_str());
  KERN_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
  KERN_CHECK(gaps::horn_hz(0.1f) == gaps::horn_low() && gaps::horn_hz(0.7f) == gaps::horn_high() && gaps::horn_hz(1.4f) == gaps::horn_low(), "two tones of 0.66 s");
  KERN_CHECK(std::fabs(gaps::whoop_hz(0) - 760) < 1 && std::fabs(gaps::whoop_hz(0.6999f) - 1480) < 2 && gaps::whoop_hz(0.9f) == 760, "whoop %f", gaps::whoop_hz(0.6999f));
  KERN_CHECK(std::fabs(gaps::siren_gain(0.06f, 0, 1) - 0.06f) < 1e-6f && gaps::siren_gain(0.06f, 261, 1) == 0, "level law");
  KERN_CHECK(std::fabs(gaps::siren_gain(0.06f, 14, 1) - 0.03f) < 1e-6f, "half at 14 m");
  KERN_CHECK(gaps::siren_cutoff(0) == 5200 && gaps::siren_cutoff(400) == 900 && std::fabs(gaps::siren_cutoff(100) - 3600) < 1e-3f, "cutoff");
  gaps::SirenVoice v;
  for (int i = 0; i < 120; ++i) v.update(100 - i * 0.25f, 1.0f / 60, gaps::car_gain(), 1);  // closing at 15 m/s
  KERN_CHECK(v.doppler > 1.03f && v.doppler < 1.05f, "approaching raises the pitch %f", v.doppler);
  KERN_CHECK(gaps::post_arrival_grace_ms(1) == 675 && gaps::post_arrival_grace_ms(5) == 495, "grace");
}

KERN_TEST(gaps_smoke) {
  kern::test::BlueprintRun bp("Sirens", &gaps::bind_sirens);
  gaps::SmokeField f = gaps::SmokeField::of(78);
  const f32 at = f.start("car-1", {0, 0, 1}, 0.92f, 2.45f, 10);
  KERN_CHECK(std::fabs(at - 12.45f) < 1e-4f && f.live_puffs() == 1, "first puff at once");
  KERN_CHECK(std::fabs(f.start("car-1", {1, 0, 1}, 0.92f, 9, 11) - 12.45f) < 1e-4f, "a running effect keeps its clock");
  u32 embers = 0;
  for (f32 t = 10; t < 12.4f; t += 1.0f / 60) {
    f.update(t, 1.0f / 60);
    for (const gaps::Puff& p : f.puffs()) embers += p.alive && p.ember ? 1 : 0;
  }
  KERN_CHECK(f.live_puffs() > 12 && f.live_puffs() <= 78, "puffs %u", f.live_puffs());
  KERN_CHECK(embers > 0, "embers late in the countdown");
  KERN_CHECK(f.accelerate("car-1", 0, 12.3f) && f.effects() == 1, "accelerate");
  f.update(12.49f, 0.01f);
  KERN_CHECK(f.effects() == 0, "expired");
  KERN_CHECK(gaps::organic_rate(gaps::Organic::bird) == 1.16f && gaps::organic_rate(gaps::Organic::animal) == 0.94f, "rates");
}

KERN_TEST(gaps_visual_pose) {
  gaps::VisualPose v;
  Vec3 p;
  Quat q;
  v.update({0, 0, 1}, Quat{}, {0, 0, 1}, 1, 1.0f / 60, p, q);
  KERN_CHECK(kern::length(p - Vec3{0, 0, 1}) < 1e-5f, "first frame: the raw pose");
  // a sudden 0.5 m jump of the body: the drawn airframe follows, but never more than 0.10 m behind
  v.update({0.5f, 0, 1}, Quat{}, {0, 0, 1}, 1, 1.0f / 60, p, q);
  KERN_CHECK(p.x > 0.39f && p.x < 0.5f, "capped lag %f", p.x);
  // a 30 deg roll: the rotation error is capped at 10 deg
  const Quat roll = Quat::axis_angle({1, 0, 0}, 30 * kDeg);
  v.update({0.5f, 0, 1}, roll, {0, 0, 1}, 1, 1.0f / 60, p, q);
  KERN_CHECK(gaps::quat_angle(q, roll) <= 10 * kDeg + 1e-3f && gaps::quat_angle(q, roll) > 5 * kDeg, "rot err %f", gaps::quat_angle(q, roll) / kDeg);
  for (int i = 0; i < 120; ++i) v.update({0.5f, 0, 1}, roll, {0, 0, 1}, 1, 1.0f / 60, p, q);
  KERN_CHECK(gaps::quat_angle(q, roll) < 0.5f * kDeg && std::fabs(p.x - 0.5f) < 1e-3f, "settles");
}

KERN_TEST(gaps_street_lamps) {
  kern::test::BlueprintRun bp("Lamps", &lamps::bind);
  KERN_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Lamps Blueprint compiles and runs: %s", bp.problems.c_str());
  KERN_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
  lamps::Field f;
  // a straight 200 m street, 8 m wide: lamps from 17 m on every 34 m, alternating kerbs, 1.15 m out
  f.place_road({{0, 0}, {200, 0}}, 8, 0, 2.0f);
  KERN_CHECK(f.all().size() == 6, "lamps %zu", f.all().size());
  const lamps::Lamp& a = f.all()[0];
  KERN_CHECK(std::fabs(a.foot.x - 17) < 1e-3f && std::fabs(std::fabs(a.foot.y) - 5.15f) < 1e-3f && a.foot.z == 2, "first lamp %f %f", a.foot.x, a.foot.y);
  KERN_CHECK(f.all()[0].foot.y * f.all()[1].foot.y < 0, "alternating sides");
  KERN_CHECK(std::fabs(std::fabs(a.head.y) - (5.15f - 1.42f)) < 1e-3f, "the arm reaches over the road");
  // placing the same street again adds nothing (one lamp per 8 m cell)
  f.place_road({{0, 0}, {200, 0}}, 8, 1, 0);
  KERN_CHECK(f.all().size() == 6, "dedup");
  // a round through the head
  const Vec3 h{a.head.x, a.head.y, a.foot.z + lamps::kHeadZ};
  const i32 hit = f.shot(h - Vec3{0, 20, 0}, h + Vec3{0, 20, 0});
  KERN_CHECK(hit == 0 && f.set_state(hit, 1) && !f.set_state(hit, 1) && f.shot(h - Vec3{0, 20, 0}, h + Vec3{0, 20, 0}) == -1, "shot out once");
  // a blast: posts within 60 % go down, further ones only darken
  const auto b = f.blast({51, 0, 0}, 12);
  KERN_CHECK(b.size() == 1 && b[0].state == 2 && b[0].lamp == 1, "blast %zu", b.size());
  // a car at 10 m/s driving into a post topples it; at 2 m/s it is stopped
  const lamps::Lamp& c = f.all()[2];
  auto fast = f.car({c.foot.x - 1.5f, c.foot.y, 0}, 0, {10, 0, 0}, 1.88f, 0.86f, 1350);
  KERN_CHECK(fast.size() == 1 && fast[0].topple && fast[0].impulse.x < 0, "sheared off");
  auto slow = f.car({c.foot.x - 1.5f, c.foot.y, 0}, 0, {2, 0, 0}, 1.88f, 0.86f, 1350);
  KERN_CHECK(slow.size() == 1 && !slow[0].topple && std::fabs(slow[0].impulse.x + 1350 * 2 * 1.05f) < 1, "stopped %f", slow.empty() ? 0.0f : slow[0].impulse.x);
  // broken lamps stay broken when the street is placed again; RESET puts them back
  f.clear();
  f.place_road({{0, 0}, {200, 0}}, 8, 0, 0);
  KERN_CHECK(f.all()[0].state == 1, "remembered");
  f.reset_broken();
  KERN_CHECK(f.all()[0].state == 0 && f.broken() == 0, "reset");
}

KERN_TEST(gaps_jet_seats) {
  using seats::Result;
  using seats::Seat;
  kern::test::BlueprintRun bp("JetSeats", &seats::bind);
  KERN_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's JetSeats Blueprint compiles and runs: %s", bp.problems.c_str());
  KERN_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
  KERN_CHECK(seats::result_name(Result::owner_only_pilot) == "owner-only-pilot" && seats::result_name(Result::ok) == "ok", "a result by its name");
  seats::Table t;
  KERN_CHECK(t.claim(1, 2, Seat::pilot) == Result::owner_only_pilot, "only the owner flies");
  KERN_CHECK(t.claim(1, 2, Seat::passenger) == Result::no_pilot, "no ride without a pilot");
  KERN_CHECK(t.claim(1, 1, Seat::pilot) == Result::ok && t.claim(1, 1, Seat::pilot) == Result::unchanged, "pilot");
  KERN_CHECK(t.claim(1, 2, Seat::passenger) == Result::ok, "passenger");
  KERN_CHECK(t.claim(1, 3, Seat::passenger) == Result::seat_occupied, "one passenger");
  KERN_CHECK(t.claim(3, 2, Seat::pilot) == Result::owner_only_pilot && t.claim(2, 2, Seat::pilot) == Result::already_seated, "one seat per player");
  seats::Row r;
  Seat s;
  KERN_CHECK(t.seat_of(2, r, s) && r.owner == 1 && s == Seat::passenger, "seat_of");
  // the pilot leaving takes the passenger out (no unmanaged flight)
  KERN_CHECK(t.release(1, 1, Seat::pilot) == Result::ok && !t.seat_of(2, r, s) && t.rows().empty(), "failsafe");
  KERN_CHECK(t.claim(255, 255, Seat::pilot) == Result::ok && t.claim(255, 0, Seat::passenger) == Result::ok, "the host's jet");
  KERN_CHECK(t.drop(0) && t.get(255).passenger == seats::kNone && t.get(255).pilot == 255, "a leaving passenger");
  seats::Table u;
  KERN_CHECK(u.load(t.revision(), t.rows()) && u.get(255).pilot == 255, "load");
  KERN_CHECK(!u.load(9, {{1, 2, seats::kNone}}), "a pilot must be the owner");
}
