// sc-m3-gaps: spawn visibility and vehicle clearance (spawn_guard.h), sirens, dispatch, critical
// smoke, organic hit audio (gaps_rules.h). What the program's C++ asks of the rules is the level's Blueprints
// world_traffic_cpp, spawn_guard_cpp, gaps_rules_cpp, lamps_cpp and jet_seats_cpp, called as their classes - as
// the program calls them (level_blueprints.h) - beside the rule actors they ask (Call on Actor).
#include <memory>

#include "../src/jet_seats.h"  // (seats::kNone)
#include "../src/rules.h"
#include "kern/game/runtime.h"
#include "kern/kern.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"

using namespace sc;

namespace {
using Tf = blueprints::world_traffic_cpp;
using Sg = blueprints::spawn_guard_cpp;
using Gp = blueprints::gaps_rules_cpp;
using Lp = blueprints::lamps_cpp;
using Js = blueprints::jet_seats_cpp;
/// The class T of a program file's Blueprint, running beside the rule actor it asks (`rule`).
template <class T> struct ProgramClass {
  RuleBlueprint rules;
  bool bound = false;
  ProgramClass(const char* actor, const char* rule) : rules(actor, nullptr, true, {rule}) {
    bound = rules.problems.empty() && kern::logic::bind_blueprint<T>(rules.rt);
  }
  T& operator*() { return kern::logic::blueprint<T>(); }
};
#define CLASS_BOUND(c, name) \
  SC_CHECK((c).bound, name " runs as its class, beside its rule actor: %s", (c).rules.problems.c_str()); \
  if (!(c).bound) return
Gp::Quat bp_quat(const Quat& q) { return Gp::Quat{q.x, q.y, q.z, q.w}; }
}  // namespace

SC_TEST(gaps_crossing_junctions) {
  ProgramClass<Tf> tc("world_traffic_cpp", "Traffic");
  CLASS_BOUND(tc, "world_traffic_cpp");
  SC_CHECK((*tc).TrafficRules_bind().Result, "it finds the Traffic Blueprint");
  // two streets laid across each other without a shared vertex still meet in a junction
  Tf::List<Tf::Lane> lanes(std::vector<Tf::Lane>(2));
  lanes[0].points = Tf::List<Vec3>(std::vector<Vec3>{{-100, 0, 0}, {100, 0, 0}});
  lanes[0].width = 12;
  lanes[1].points = Tf::List<Vec3>(std::vector<Vec3>{{0, -100, 0}, {0, 100, 0}});
  lanes[1].width = 6;
  Tf::TrafficRules t;
  (*tc).TrafficRules_rebuild(t, lanes);
  const auto& junctions = t.junctions_;
  SC_CHECK(junctions.size() == 1 && junctions[0].arms.size() == 4 && junctions[0].lights, "junctions %zu", static_cast<std::size_t>(junctions.size()));
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
    if (!rules::actor_graph("SpawnGuard", *guard.logic, error)) problems = error;
    for (const kern::logic::Issue& i : kern::logic::check(*guard.logic)) problems += (i.error ? "error: " : "warning: ") + i.text + "\n";
    const kern::game::ActorId id = level.add(guard).id;
    // the program's Blueprint spawn_guard_cpp and the libraries it uses (the level's actors of those names), as in the game
    for (const char* name : {"spawn_guard_cpp", "spawn_guard", "blueprint_call"}) {
      kern::game::Actor program;
      program.name = name;
      program.logic.emplace();
      if (!rules::actor_graph(name, *program.logic, error)) problems += error + "\n";
      level.add(program);
    }
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
    bound = world->logic_runtime() && kern::logic::bind_blueprint<Sg>(*world->logic_runtime()) && kern::logic::blueprint<Sg>().bind().Result;
  }
  bool bound = false;
  Sg& operator*() { return kern::logic::blueprint<Sg>(); }
  ~SpawnGuardWorld() {
    kern::logic::bind_blueprint<Sg>(no_logic());
    world.reset();
    renderer.shutdown();
  }
};
}  // namespace

SC_TEST(spawn_guard_rules) {
  SpawnGuardWorld open(false);
  SC_CHECK(open.inst && open.problems.empty() && !open.inst->disabled(), "the level's SpawnGuard Blueprint compiles and runs: %s", open.problems.c_str());
  if (!open.inst) return;
  SC_CHECK(std::string(open.inst->tier()) == "native C++", "the SpawnGuard Blueprint runs as its C++ class (it runs: %s)", open.inst->tier());
  SC_CHECK(open.bound, "spawn_guard_cpp runs as its class in that world");
  if (!open.bound) return;
  Sg& sg = *open;
  using Viewers = Sg::List<Sg::spawn_Viewer>;
  // (the class bound last: a world of its own below binds it to that world, as spawn::bind did)
  const auto can_spawn = [](const Viewers& vs, Vec3 p, f64 far_entry, f64 height) { return kern::logic::blueprint<Sg>().can_spawn(vs, p, far_entry, height).Result; };
  Sg::spawn_Viewer v;
  v.eye = {0, 0, 1.7f};
  v.forward = {1, 0, 0};
  v.half_angle = sg.cone_half_angle(60 * kDeg, 16.0f / 9).Result;
  v.local = true;
  const Viewers vs(std::vector<Sg::spawn_Viewer>{v});
  // the 60 deg vertical / 16:9 view is ~91 deg wide: half 45.7 + 20 deg
  SC_CHECK(std::fabs(v.half_angle - (45.7f + 20) * kDeg) < 0.6f * kDeg, "half %.2f", v.half_angle / kDeg);
  SC_CHECK(std::fabs(sg.default_far().Result - 1500) < 1e-3f, "the distances are its variables");
  SC_CHECK(!can_spawn(vs, {10, -15, 0}, 0, 2.2f), "closer than 24 m is always seen");
  SC_CHECK(!can_spawn(vs, {-20, 5, 0}, 0, 2.2f), "behind but within 24 m");
  SC_CHECK(can_spawn(vs, {-60, 0, 0}, 0, 2.2f), "behind, far enough");
  SC_CHECK(!can_spawn(vs, {80, 20, 0}, 0, 2.2f), "in view");
  SC_CHECK(sg.seen_by(v, {80, 20, 0}, 0, 2.2f).Result && !sg.seen_by(v, {-60, 0, 0}, 0, 2.2f).Result, "one viewer alone");
  SC_CHECK(can_spawn(vs, {1600, 0, 0}, 0, 2.2f), "beyond the far plane");
  SC_CHECK(can_spawn(vs, {300, 0, 0}, 250, 2.2f), "far entry allowed in view");
  // a mate looking the other way sees what we do not
  Sg::spawn_Viewer mate;
  mate.eye = {0, 0, 1.7f};
  mate.forward = {-1, 0, 0};
  mate.half_angle = v.half_angle;
  SC_CHECK(!can_spawn(Viewers(std::vector<Sg::spawn_Viewer>{v, mate}), {-60, 0, 0}, 0, 2.2f), "in the mate's view");
  {
    // a building in between hides it - both the feet and the top
    SpawnGuardWorld town(true);
    SC_CHECK(town.inst && town.problems.empty() && town.bound, "(a world with a building: %s)", town.problems.c_str());
    if (!town.bound) return;
    SC_CHECK(can_spawn(vs, {80, 20, 0}, 0, 2.2f), "hidden by a building");
    SC_CHECK(!can_spawn(vs, {80, 20, 0}, 0, 200.0f), "something taller than the building shows above it");
    SC_CHECK(!can_spawn(vs, {500, 0, 0}, 0, 2.2f), "beyond 420 m buildings do not count");
    SC_CHECK(!can_spawn(vs, {30, 5, 0}, 0, 2.2f), "in front of the building");
  }
  {
    // only buildings hide: a thing of another collision channel in the way does not
    SpawnGuardWorld traffic(true, true);
    SC_CHECK(traffic.bound, "(a world with a vehicle in between: %s)", traffic.problems.c_str());
    if (!traffic.bound) return;
    SC_CHECK(!can_spawn(vs, {80, 20, 0}, 0, 2.2f), "a vehicle in between hides nothing");
  }
}

SC_TEST(vehicle_clearance_sat) {
  SpawnGuardWorld bp(false);
  SC_CHECK(bp.bound, "spawn_guard_cpp runs as its class: %s", bp.problems.c_str());
  if (!bp.bound) return;
  Sg& sg = *bp;
  using Footprint = Sg::spawn_Footprint;
  // (footprints_overlap's margin 0.35, as spawn_guard.h gives it)
  const auto footprints_overlap = [&](const Footprint& a, const Footprint& b) { return sg.footprints_overlap(a, b, 0.35f).Result; };
  const Footprint a{{0, 0, 0}, 0, {2.3f, 0.95f, 0}};
  SC_CHECK(footprints_overlap(a, {{4.0f, 0, 0}, 0, {2.3f, 0.95f, 0}}), "nose to tail 4 m apart overlap (4.6 m long)");
  SC_CHECK(!footprints_overlap(a, {{5.2f, 0, 0}, 0, {2.3f, 0.95f, 0}}), "5.2 m apart: clear with 0.35 margin");
  SC_CHECK(footprints_overlap(a, {{4.9f, 0, 0}, 0, {2.3f, 0.95f, 0}}), "4.9 m apart: inside the margin");
  SC_CHECK(!footprints_overlap(a, {{0, 2.5f, 0}, 0, {2.3f, 0.95f, 0}}), "side by side 2.5 m");
  SC_CHECK(footprints_overlap(a, {{2.5f, 1.4f, 0}, kPi / 2, {2.3f, 0.95f, 0}}), "crossing at a right angle");
}

SC_TEST(gaps_sirens) {
  ProgramClass<Gp> gc("gaps_rules_cpp", "Sirens");
  CLASS_BOUND(gc, "gaps_rules_cpp");
  RuleBlueprint& bp = gc.rules;
  kern::logic::Instance* sirens = bp.find("Sirens");
  SC_CHECK(sirens && !sirens->disabled(), "the level's Sirens Blueprint compiles and runs: %s", bp.problems.c_str());
  SC_CHECK(sirens && std::string(sirens->tier()) == "native C++", "it runs as its C++ class");
  Gp& g = *gc;
  SC_CHECK(g.bind_sirens().Result, "the program finds it");
  SC_CHECK(g.horn_hz(0.1f).Result == g.horn_low().Result && g.horn_hz(0.7f).Result == g.horn_high().Result && g.horn_hz(1.4f).Result == g.horn_low().Result, "two tones of 0.66 s");
  SC_CHECK(std::fabs(g.whoop_hz(0).Result - 760) < 1 && std::fabs(g.whoop_hz(0.6999f).Result - 1480) < 2 && g.whoop_hz(0.9f).Result == 760, "whoop %f", g.whoop_hz(0.6999f).Result);
  SC_CHECK(std::fabs(g.siren_gain(0.06f, 0, 1).Result - 0.06f) < 1e-6f && g.siren_gain(0.06f, 261, 1).Result == 0, "level law");
  SC_CHECK(std::fabs(g.siren_gain(0.06f, 14, 1).Result - 0.03f) < 1e-6f, "half at 14 m");
  SC_CHECK(g.siren_cutoff(0).Result == 5200 && g.siren_cutoff(400).Result == 900 && std::fabs(g.siren_cutoff(100).Result - 3600) < 1e-3f, "cutoff");
  Gp::gaps_SirenVoice v;
  for (int i = 0; i < 120; ++i) g.SirenVoice_update(v, 100 - i * 0.25f, 1.0f / 60, g.car_gain().Result, 1);  // closing at 15 m/s
  SC_CHECK(v.doppler > 1.03f && v.doppler < 1.05f, "approaching raises the pitch %f", v.doppler);
  SC_CHECK(g.post_arrival_grace_ms(1).Result == 675 && g.post_arrival_grace_ms(5).Result == 495, "grace");
}

SC_TEST(gaps_smoke) {
  ProgramClass<Gp> gc("gaps_rules_cpp", "Sirens");
  CLASS_BOUND(gc, "gaps_rules_cpp");
  Gp& g = *gc;
  const auto live_puffs = [&](const Gp::gaps_SmokeField& f) { return static_cast<u32>(g.SmokeField_live_puffs(f).Result); };
  Gp::gaps_SmokeField f = g.SmokeField_of(78).Result;
  const f64 at = g.SmokeField_start(f, "car-1", {0, 0, 1}, 0.92f, 2.45f, 10).Result;
  SC_CHECK(std::fabs(at - 12.45f) < 1e-4f && live_puffs(f) == 1, "first puff at once");
  SC_CHECK(std::fabs(g.SmokeField_start(f, "car-1", {1, 0, 1}, 0.92f, 9, 11).Result - 12.45f) < 1e-4f, "a running effect keeps its clock");
  u32 embers = 0;
  for (f32 t = 10; t < 12.4f; t += 1.0f / 60) {
    g.SmokeField_update(f, t, 1.0f / 60);
    for (const Gp::gaps_Puff& p : f.puffs_) embers += p.alive && p.ember ? 1 : 0;
  }
  SC_CHECK(live_puffs(f) > 12 && live_puffs(f) <= 78, "puffs %u", live_puffs(f));
  SC_CHECK(embers > 0, "embers late in the countdown");
  SC_CHECK(g.SmokeField_accelerate(f, "car-1", 0, 12.3f).Result && g.SmokeField_effects(f).Result == 1, "accelerate");
  g.SmokeField_update(f, 12.49f, 0.01f);
  SC_CHECK(g.SmokeField_effects(f).Result == 0, "expired");
  SC_CHECK(g.organic_rate(Gp::gaps_Organic::bird).Result == 1.16f && g.organic_rate(Gp::gaps_Organic::animal).Result == 0.94f, "rates");
}

SC_TEST(gaps_visual_pose) {
  ProgramClass<Gp> gc("gaps_rules_cpp", "Sirens");
  CLASS_BOUND(gc, "gaps_rules_cpp");
  Gp& g = *gc;
  Gp::gaps_VisualPose v;
  Vec3 p;
  Gp::Quat q;
  const auto quat_angle = [&](const Gp::Quat& a, const Quat& b) { return static_cast<f32>(g.quat_angle(a, bp_quat(b)).Result); };
  g.VisualPose_update(v, p, q, {0, 0, 1}, bp_quat(Quat{}), {0, 0, 1}, 1, 1.0f / 60);
  SC_CHECK(kern::length(p - Vec3{0, 0, 1}) < 1e-5f, "first frame: the raw pose");
  // a sudden 0.5 m jump of the body: the drawn airframe follows, but never more than 0.10 m behind
  g.VisualPose_update(v, p, q, {0.5f, 0, 1}, bp_quat(Quat{}), {0, 0, 1}, 1, 1.0f / 60);
  SC_CHECK(p.x > 0.39f && p.x < 0.5f, "capped lag %f", p.x);
  // a 30 deg roll: the rotation error is capped at 10 deg
  const Quat roll = Quat::axis_angle({1, 0, 0}, 30 * kDeg);
  g.VisualPose_update(v, p, q, {0.5f, 0, 1}, bp_quat(roll), {0, 0, 1}, 1, 1.0f / 60);
  SC_CHECK(quat_angle(q, roll) <= 10 * kDeg + 1e-3f && quat_angle(q, roll) > 5 * kDeg, "rot err %f", quat_angle(q, roll) / kDeg);
  for (int i = 0; i < 120; ++i) g.VisualPose_update(v, p, q, {0.5f, 0, 1}, bp_quat(roll), {0, 0, 1}, 1, 1.0f / 60);
  SC_CHECK(quat_angle(q, roll) < 0.5f * kDeg && std::fabs(p.x - 0.5f) < 1e-3f, "settles");
}

SC_TEST(gaps_street_lamps) {
  ProgramClass<Lp> lc("lamps_cpp", "Lamps");
  CLASS_BOUND(lc, "lamps_cpp");
  RuleBlueprint& bp = lc.rules;
  kern::logic::Instance* rule = bp.find("Lamps");
  SC_CHECK(rule && !rule->disabled(), "the level's Lamps Blueprint compiles and runs: %s", bp.problems.c_str());
  SC_CHECK(rule && std::string(rule->tier()) == "native C++", "it runs as its C++ class");
  Lp& l = *lc;
  SC_CHECK(l.bind().Result, "the program finds it");
  const auto road = [](Vec3 a, Vec3 b) { return Lp::List<Vec3>(std::vector<Vec3>{a, b}); };
  const auto count = [](const Lp::lamps_Field& f) { return static_cast<std::size_t>(f.lamps_.size()); };
  Lp::lamps_Field f;
  // a straight 200 m street, 8 m wide: lamps from 17 m on every 34 m, alternating kerbs, 1.15 m out
  l.Field_place_road(f, road({0, 0, 0}, {200, 0, 0}), 8, 0, 2.0f);
  SC_CHECK(count(f) == 6, "lamps %zu", count(f));
  const Lp::lamps_Lamp a = f.lamps_[0];
  SC_CHECK(std::fabs(a.foot.x - 17) < 1e-3f && std::fabs(std::fabs(a.foot.y) - 5.15f) < 1e-3f && a.foot.z == 2, "first lamp %f %f", a.foot.x, a.foot.y);
  SC_CHECK(f.lamps_[0].foot.y * f.lamps_[1].foot.y < 0, "alternating sides");
  SC_CHECK(std::fabs(std::fabs(a.head.y) - (5.15f - 1.42f)) < 1e-3f, "the arm reaches over the road");
  // placing the same street again adds nothing (one lamp per 8 m cell)
  l.Field_place_road(f, road({0, 0, 0}, {200, 0, 0}), 8, 1, 0);
  SC_CHECK(count(f) == 6, "dedup");
  // a round through the head
  const Vec3 h{a.head.x, a.head.y, static_cast<f32>(a.foot.z + Lp::kHeadZ)};
  const i32 hit = static_cast<i32>(l.Field_shot(f, h - Vec3{0, 20, 0}, h + Vec3{0, 20, 0}).Result);
  SC_CHECK(hit == 0 && l.Field_set_state(f, hit, 1).Result && !l.Field_set_state(f, hit, 1).Result && static_cast<i32>(l.Field_shot(f, h - Vec3{0, 20, 0}, h + Vec3{0, 20, 0}).Result) == -1, "shot out once");
  // a blast: posts within 60 % go down, further ones only darken
  const auto b = l.Field_blast(f, {51, 0, 0}, 12).Result;
  SC_CHECK(b.size() == 1 && static_cast<u32>(b[0].state) == 2 && static_cast<i32>(b[0].lamp) == 1, "blast %zu", static_cast<std::size_t>(b.size()));
  // a car at 10 m/s driving into a post topples it; at 2 m/s it is stopped
  const Lp::lamps_Lamp c = f.lamps_[2];
  auto fast = l.Field_car(f, {c.foot.x - 1.5f, c.foot.y, 0}, 0, {10, 0, 0}, 1.88f, 0.86f, 1350).Result;
  SC_CHECK(fast.size() == 1 && fast[0].topple && fast[0].impulse.x < 0, "sheared off");
  auto slow = l.Field_car(f, {c.foot.x - 1.5f, c.foot.y, 0}, 0, {2, 0, 0}, 1.88f, 0.86f, 1350).Result;
  SC_CHECK(slow.size() == 1 && !slow[0].topple && std::fabs(slow[0].impulse.x + 1350 * 2 * 1.05f) < 1, "stopped %f", slow.empty() ? 0.0f : slow[0].impulse.x);
  // broken lamps stay broken when the street is placed again; RESET puts them back
  l.Field_clear(f);
  l.Field_place_road(f, road({0, 0, 0}, {200, 0, 0}), 8, 0, 0);
  SC_CHECK(static_cast<u32>(f.lamps_[0].state) == 1, "remembered");
  l.Field_reset_broken(f);
  SC_CHECK(static_cast<u32>(f.lamps_[0].state) == 0 && l.Field_broken(f).Result == 0, "reset");
}

SC_TEST(gaps_jet_seats) {
  using Result = Js::the_seats_Result_;
  using Seat = Js::seats_Seat;
  ProgramClass<Js> jc("jet_seats_cpp", "JetSeats");
  CLASS_BOUND(jc, "jet_seats_cpp");
  RuleBlueprint& bp = jc.rules;
  kern::logic::Instance* rule = bp.find("JetSeats");
  SC_CHECK(rule && !rule->disabled(), "the level's JetSeats Blueprint compiles and runs: %s", bp.problems.c_str());
  SC_CHECK(rule && std::string(rule->tier()) == "native C++", "it runs as its C++ class");
  Js& j = *jc;
  SC_CHECK(j.bind().Result, "the program finds it");
  SC_CHECK(j.result_name(Result::owner_only_pilot).Result.str() == "owner-only-pilot" && j.result_name(Result::ok).Result.str() == "ok", "a result by its name");
  /// seats::Table on the Blueprint's struct (claim, release, drop, get, seat_of, rows, revision, load).
  struct Table {
    Js& j;
    Js::seats_Table t;
    Result claim(u8 owner, u8 player, Seat seat) { return j.Table_claim(t, owner, player, seat).Result; }
    Result release(u8 owner, u8 player, Seat seat) { return j.Table_release(t, owner, player, seat).Result; }
    bool drop(u8 player) { return j.Table_drop(t, player).Result; }
    Js::seats_Row get(u8 owner) { return j.Table_get(t, owner).Result; }
    bool seat_of(u8 player, Js::seats_Row& r, Seat& s) { return j.Table_seat_of(t, r, s, player).Result; }
    const Js::List<Js::seats_Row>& rows() const { return t.rows_; }
    u32 revision() { return static_cast<u32>(j.Table_revision(t).Result); }
    bool load(u32 revision, const Js::List<Js::seats_Row>& rows) { return j.Table_load(t, rows, revision).Result; }
  };
  Table t{j, {}};
  SC_CHECK(t.claim(1, 2, Seat::pilot) == Result::owner_only_pilot, "only the owner flies");
  SC_CHECK(t.claim(1, 2, Seat::passenger) == Result::no_pilot, "no ride without a pilot");
  SC_CHECK(t.claim(1, 1, Seat::pilot) == Result::ok && t.claim(1, 1, Seat::pilot) == Result::unchanged, "pilot");
  SC_CHECK(t.claim(1, 2, Seat::passenger) == Result::ok, "passenger");
  SC_CHECK(t.claim(1, 3, Seat::passenger) == Result::seat_occupied, "one passenger");
  SC_CHECK(t.claim(3, 2, Seat::pilot) == Result::owner_only_pilot && t.claim(2, 2, Seat::pilot) == Result::already_seated, "one seat per player");
  Js::seats_Row r;
  Seat s = Seat::pilot;
  SC_CHECK(t.seat_of(2, r, s) && static_cast<u32>(r.owner) == 1 && s == Seat::passenger, "seat_of");
  // the pilot leaving takes the passenger out (no unmanaged flight)
  SC_CHECK(t.release(1, 1, Seat::pilot) == Result::ok && !t.seat_of(2, r, s) && t.rows().empty(), "failsafe");
  SC_CHECK(t.claim(255, 255, Seat::pilot) == Result::ok && t.claim(255, 0, Seat::passenger) == Result::ok, "the host's jet");
  // (seats::kNone - the Blueprint's variable None, of no class yet: the C++ constant until the level has it)
  SC_CHECK(t.drop(0) && static_cast<u32>(t.get(255).passenger) == seats::kNone && static_cast<u32>(t.get(255).pilot) == 255, "a leaving passenger");
  Table u{j, {}};
  SC_CHECK(u.load(t.revision(), t.rows()) && static_cast<u32>(u.get(255).pilot) == 255, "load");
  SC_CHECK(!u.load(9, Js::List<Js::seats_Row>(std::vector<Js::seats_Row>{{1, 2, seats::kNone}})), "a pilot must be the owner");
}
