// Drone: the Arondight45 firmware calibrates, arms, holds height in GAME mode, flies forward along
// the nose and lands; the plant's static thrust matches SpawnCity's propulsion authority. The drone is the
// level's Blueprint drone_cpp (its structs: drone; the firmware's Blueprints Arondight45_*), called as its
// class - as the program calls it (level_blueprints.h) - on the engine itself: the physics world is the
// test's, given to it by its handle.
#include <memory>

#include "../src/blueprint_call.h"
#include "kern/physics/physics.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
using Dr = blueprints::drone_cpp;
constexpr f32 kH = 1.0f / 240.0f;
/// drone_cpp running as its class, on the engine itself (RuleBlueprint::Engine::direct).
struct DroneBlueprint {
  RuleBlueprint rules{"drone_cpp", nullptr, true, {}, RuleBlueprint::Engine::direct};
  bool bound = false;
  DroneBlueprint() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Dr>(rules.rt);
  }
  Dr& operator*() { return kern::logic::blueprint<Dr>(); }
};

struct Rig {
  DroneBlueprint bp;
  kern::physics::World world;
  i64 w = 0;
  Dr::physics_Body body;
  std::unique_ptr<Dr::Drone> drone = std::make_unique<Dr::Drone>();
  Rig() {
    make_flat_world(world);
    if (!bp.bound) return;
    Dr& d = *bp;
    // (Drone(const DroneParams& p = {}), rigid_spec(p = {}); create_body's category 1 and mask ~0, as rigid_spec.h gives them)
    *drone = d.Drone_new(Dr::DroneParams{}).Result;
    w = bp.rules.host.objects.adopt(world);
    body = d.create_body(d.Drone_rigid_spec(Dr::DroneParams{}).Result, w, {0, 0, 0.0225f}, Dr::Quat{0, 0, 0, 1}, Dr::Int64{1}, Dr::Int64{~0ull}).Result;
  }
  void run(f32 seconds, const Dr::DroneSticks& s) {
    const int n = static_cast<int>(seconds / kH + 0.5f);
    for (int i = 0; i < n; ++i) {
      (*bp).Drone_step(*drone, s, w, body, kH);
      world.step(kH, 1);
    }
  }
  const Dr::DroneTelemetry& telemetry() const { return drone->tele_; }
  Dr::BodyState state() { return (*bp).read_body(w, body).Result; }
  Vec3 rotate(const Dr::Quat& q, Vec3 v) { return (*bp).Quat_rotate(q, v).Result; }
  f32 heading(Vec3 axis) { return static_cast<f32>((*bp).heading_of(axis).Result); }
  f32 wrap(f32 a) { return static_cast<f32>((*bp).wrap_pi(a).Result); }
};
}  // namespace

#define DRONE_BOUND(r) \
  SC_CHECK((r).bp.bound, "drone_cpp runs as its class: %s", (r).bp.rules.problems.c_str()); \
  if (!(r).bp.bound) return

SC_TEST(drone_mass_properties) {
  DroneBlueprint bp;
  SC_CHECK(bp.bound, "drone_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  const Dr::RigidSpec r = (*bp).Drone_rigid_spec(Dr::DroneParams{}).Result;
  SC_CHECK(std::fabs(r.mass.mass - 0.72f) < 0.01f, "mass %.3f", r.mass.mass);
  SC_CHECK(r.mass.inertia[0] > 0 && r.mass.inertia[1] > 0 && r.mass.inertia[2] > r.mass.inertia[0], "inertia %g %g %g", r.mass.inertia[0],
           r.mass.inertia[1], r.mass.inertia[2]);
}

SC_TEST(drone_calibrates_arms_and_hovers) {
  Rig rig;
  DRONE_BOUND(rig);
  Dr::DroneSticks s;
  s.game_mode = true;
  s.clearance = 3.0f;
  rig.run(2.5f, s);  // 2000 calibration samples at ~1 kHz with the arm switch low
  SC_CHECK(!rig.telemetry().calibrating, "still calibrating");
  SC_CHECK(!rig.telemetry().fault, "fault state %u", static_cast<u32>(rig.telemetry().state));
  s.arm = true;
  rig.run(1.5f, s);
  SC_CHECK(rig.telemetry().armed, "not armed, state %u", static_cast<u32>(rig.telemetry().state));
  rig.run(6.0f, s);
  const Dr::BodyState st = rig.state();
  SC_CHECK(std::fabs(st.position.z - 3.0f) < 0.6f, "hover height %.2f", st.position.z);
  SC_CHECK(kern::length(st.velocity) < 0.6f, "hover drift %.2f m/s", kern::length(st.velocity));
  const Vec3 up = rig.rotate(st.rotation, {0, 0, 1});
  SC_CHECK(up.z > 0.97f, "level hover up.z %.3f", up.z);
}

SC_TEST(drone_flies_along_its_nose) {
  Rig rig;
  DRONE_BOUND(rig);
  Dr::DroneSticks s;
  s.clearance = 4.0f;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(5.0f, s);
  s.pitch = 1.0f;  // full forward
  rig.run(4.0f, s);
  const Dr::BodyState st = rig.state();
  const Vec3 nose = rig.rotate(st.rotation, {-1, 0, 0});
  const f32 along = kern::dot(st.velocity, Vec3{nose.x, nose.y, 0});
  SC_CHECK(along > 12.0f, "forward speed %.1f m/s (v %.1f %.1f %.1f)", along, st.velocity.x, st.velocity.y, st.velocity.z);
  SC_CHECK(along < 27.0f, "GAME envelope is 25 m/s, got %.1f", along);
  SC_CHECK(st.position.z > 2.0f, "keeps height while cruising %.2f", st.position.z);
  s.pitch = 0;
  rig.run(3.0f, s);
  const f32 v3 = kern::length(rig.state().velocity);
  rig.run(5.0f, s);
  SC_CHECK(kern::length(rig.state().velocity) < 1.0f && kern::length(rig.state().velocity) < v3 + 0.1f, "brakes to a hover, %.2f m/s (after 3 s: %.2f)", kern::length(rig.state().velocity), v3);
}

SC_TEST(drone_yaw_and_climb) {
  Rig rig;
  DRONE_BOUND(rig);
  Dr::DroneSticks s;
  s.clearance = 2.0f;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(4.0f, s);
  const f32 yaw0 = rig.heading(rig.rotate(rig.state().rotation, {-1, 0, 0}));
  s.yaw = 0.5f;
  rig.run(1.0f, s);
  const f32 yaw1 = rig.heading(rig.rotate(rig.state().rotation, {-1, 0, 0}));
  SC_CHECK(rig.wrap(yaw1 - yaw0) > 0.4f, "positive yaw stick turns the nose left (counter-clockwise from above): %.2f rad", rig.wrap(yaw1 - yaw0));
  s.yaw = 0;
  s.clearance = 20.0f;
  rig.run(8.0f, s);
  SC_CHECK(std::fabs(rig.state().position.z - 20.0f) < 2.0f, "climbs to 20 m: %.2f", rig.state().position.z);
}

SC_TEST(drone_angle_mode_throttle) {
  Rig rig;
  DRONE_BOUND(rig);
  Dr::DroneSticks s;
  s.game_mode = false;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(1.5f, s);
  SC_CHECK(rig.telemetry().armed, "armed in ANGLE mode");
  s.throttle = 0.7f;
  rig.run(1.0f, s);
  SC_CHECK(rig.state().position.z > 2.0f, "full-ish throttle climbs: %.2f", rig.state().position.z);
  SC_CHECK(rig.telemetry().current_a > 20.0f, "draws current %.1f A", rig.telemetry().current_a);
  SC_CHECK(rig.telemetry().battery_v < 16.8f, "pack sags %.2f V", rig.telemetry().battery_v);
}

SC_TEST(drone_clearance_law) {
  DroneBlueprint bp;
  SC_CHECK(bp.bound, "drone_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  const auto clearance_rate = [&](f64 axis) { return (*bp).clearance_rate(axis).Result; };
  const auto clearance_lead = [&](f64 v) { return (*bp).clearance_lead(v).Result; };
  SC_CHECK(clearance_rate(0.05f) == 0, "deadband");
  SC_CHECK(std::fabs(clearance_rate(1.0f) - 12.0f) < 1e-4f, "full stick = 12 m/s");
  SC_CHECK(clearance_lead(0) == 0, "no lead at rest");
  SC_CHECK(clearance_lead(4) > 2 && clearance_lead(-4) < -2, "lead %.2f %.2f", clearance_lead(4), clearance_lead(-4));
}

SC_TEST(drone_roll_moves_right) {
  Rig rig;
  DRONE_BOUND(rig);
  Dr::DroneSticks s;
  s.clearance = 3.0f;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(4.0f, s);
  s.roll = 1.0f;
  rig.run(2.0f, s);
  const Dr::BodyState st = rig.state();
  const Vec3 nose = rig.rotate(st.rotation, {-1, 0, 0}), right = kern::cross(nose, Vec3{0, 0, 1});
  SC_CHECK(kern::dot(st.velocity, right) > 5.0f, "positive roll flies to the right of the nose: %.1f m/s", kern::dot(st.velocity, right));
}
