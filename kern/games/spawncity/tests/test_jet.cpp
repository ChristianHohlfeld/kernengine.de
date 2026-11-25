// Jet: stands on its gear, takes off vertically, hovers, transitions to wing-borne flight,
// climbs with the stick, and lands softly jet-borne. The jet is the level's Blueprint jet_cpp (its structs:
// jet), called as its class - as the program calls it (level_blueprints.h) - on the engine itself: the
// physics world is the test's, given to it by its handle.
#include "../src/blueprint_call.h"
#include "kern/physics/physics.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
using Jt = blueprints::jet_cpp;
constexpr f32 kH = 1.0f / 240.0f;
/// jet_cpp running as its class, on the engine itself (RuleBlueprint::Engine::direct).
struct JetBlueprint {
  RuleBlueprint rules{"jet_cpp", nullptr, true, {}, RuleBlueprint::Engine::direct};
  bool bound = false;
  JetBlueprint() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Jt>(rules.rt);
  }
  Jt& operator*() { return kern::logic::blueprint<Jt>(); }
};
struct JetRig {
  JetBlueprint bp;
  kern::physics::World world;
  i64 w = 0;
  Jt::physics_Body body;
  Jt::Jet jet;
  JetRig() {
    make_flat_world(world);
    if (!bp.bound) return;
    Jt& j = *bp;
    // (Jet(const JetSpec& s = {}), rigid_spec(s = {}); create_body's category 1 and mask ~0, as rigid_spec.h gives them)
    jet = j.Jet_new(Jt::JetSpec{}).Result;
    w = bp.rules.host.objects.adopt(world);
    body = j.create_body(j.Jet_rigid_spec(Jt::JetSpec{}).Result, w, {0, 0, 2.1f}, Jt::Quat{0, 0, 0, 1}, Jt::Int64{1}, Jt::Int64{~0ull}).Result;
  }
  void step(const Jt::JetInput& in) { (*bp).Jet_step(jet, in, w, body, kH); }
  void run(f32 seconds, const Jt::JetInput& in) {
    const int n = static_cast<int>(seconds / kH + 0.5f);
    for (int i = 0; i < n; ++i) {
      step(in);
      world.step(kH, 1);
    }
  }
  const Jt::JetTelemetry& telemetry() const { return jet.tele_; }
  i32 contacts() const { return static_cast<i32>(jet.tele_.contacts); }
  void set_mode(Jt::Jet_Mode m) { (*bp).Jet_set_mode(jet, m); }
  f32 gear_impact() { return static_cast<f32>((*bp).Jet_gear_impact(jet).Result); }
  Jt::BodyState st() { return (*bp).read_body(w, body).Result; }
  Vec3 rotate(const Jt::Quat& q, Vec3 v) { return (*bp).Quat_rotate(q, v).Result; }
  f32 wrap(f64 a) { return static_cast<f32>((*bp).wrap_pi(a).Result); }
};
/// No stick, no lever: jet.h JetInput{} (the Blueprint's struct has the same defaults).
Jt::JetInput input() { return Jt::JetInput{}; }
}  // namespace

#define JET_BOUND(r) \
  SC_CHECK((r).bp.bound, "jet_cpp runs as its class: %s", (r).bp.rules.problems.c_str()); \
  if (!(r).bp.bound) return

SC_TEST(jet_rests_on_gear) {
  JetRig r;
  JET_BOUND(r);
  r.run(3.0f, input());
  const Jt::BodyState s = r.st();
  SC_CHECK(r.contacts() == 3, "three wheels on the ground (%d)", r.contacts());
  SC_CHECK(std::fabs(s.position.z - 2.05f) < 0.15f, "rests at gear height: %.2f", s.position.z);
  SC_CHECK(kern::length(s.velocity) < 0.05f, "at rest %.3f", kern::length(s.velocity));
}

SC_TEST(jet_vertical_takeoff_and_hover) {
  JetRig r;
  JET_BOUND(r);
  r.run(1.0f, input());
  Jt::JetInput in = input();
  in.lift = 1;
  r.run(3.0f, in);
  SC_CHECK(r.st().position.z > 12.0f, "climbs jet-borne: %.1f m", r.st().position.z);
  in.lift = 0;
  r.run(4.0f, in);
  const Jt::BodyState s = r.st();
  SC_CHECK(std::fabs(s.velocity.z) < 1.0f, "holds height: vz %.2f", s.velocity.z);
  SC_CHECK(std::hypot(s.velocity.x, s.velocity.y) < 1.5f, "no drift: %.2f", std::hypot(s.velocity.x, s.velocity.y));
  const Vec3 up = r.rotate(s.rotation, {0, 0, 1});
  SC_CHECK(up.z > 0.97f, "level: %.3f", up.z);
}

SC_TEST(jet_transition_and_climb) {
  JetRig r;
  JET_BOUND(r);
  Jt::JetInput in = input();
  in.lift = 1;
  r.run(4.0f, in);  // up to ~30 m
  in.lift = 0;
  r.run(1.0f, in);
  r.set_mode(Jt::Jet_Mode::flight);
  in.lift = 1;      // lever forward
  r.run(14.0f, in);
  const Jt::BodyState s = r.st();
  const f32 V = kern::length(s.velocity);
  SC_CHECK(V > 120.0f, "wing-borne speed %.0f m/s", V);
  SC_CHECK(r.telemetry().w_flight > 0.99f, "wing-borne law %.2f", r.telemetry().w_flight);
  SC_CHECK(s.position.z > 10.0f, "did not sink into the ground: %.0f m", s.position.z);
  const f32 z0 = s.position.z;
  in.pitch = 0.4f;  // arcade: the stick is a rate (1.6 rad/s full) - a short pull, then level climb
  r.run(1.0f, in);
  in.pitch = 0;
  r.run(2.0f, in);
  SC_CHECK(r.st().position.z > z0 + 40.0f, "pulls up: %.0f -> %.0f m", z0, r.st().position.z);
  in.roll = 0.3f;
  const f32 h0 = r.telemetry().heading;
  r.run(1.0f, in);
  in.roll = 0;
  r.run(3.0f, in);
  SC_CHECK(std::fabs(r.wrap(r.telemetry().heading - h0)) > 0.1f, "banks and turns: %.2f rad", r.wrap(r.telemetry().heading - h0));
}

SC_TEST(jet_arcade_dogfight_turns) {
  // arcade handling: the hover turns on the spot, wing-borne a bank and a pull bring it round fast
  JetRig r;
  JET_BOUND(r);
  Jt::JetInput in = input();
  in.lift = 1;
  r.run(3.0f, in);
  in.lift = 0;
  r.run(1.0f, in);
  f32 h0 = r.telemetry().heading;
  in.yaw = 1;
  r.run(1.5f, in);
  in.yaw = 0;
  SC_CHECK(std::fabs(r.wrap(r.telemetry().heading - h0)) > 1.5f, "hover yaw: %.2f rad in 1.5 s", r.wrap(r.telemetry().heading - h0));
  r.run(1.0f, in);
  r.set_mode(Jt::Jet_Mode::flight);
  in.lift = 1;
  r.run(10.0f, in);
  in.lift = 0;
  h0 = r.telemetry().heading;
  in.roll = 1;
  r.run(0.45f, in);
  in.roll = 0;
  in.pitch = 1;
  r.run(2.5f, in);
  in.pitch = 0;
  const f32 turned = std::fabs(r.wrap(r.telemetry().heading - h0));
  SC_CHECK(turned > 2.0f, "a bank and a pull: %.2f rad in 3 s", turned);
  SC_CHECK(r.st().position.z > 5.0f, "still in the air: %.0f m", r.st().position.z);
}

SC_TEST(jet_soft_landing) {
  JetRig r;
  JET_BOUND(r);
  Jt::JetInput in = input();
  in.lift = 1;
  r.run(2.5f, in);
  in.lift = -1;  // descend: the sink rate tapers near the ground
  f32 worst = 0;
  for (int i = 0; i < 240 * 10; ++i) {
    r.step(in);
    r.world.step(kH, 1);
    worst = std::fmax(worst, r.gear_impact());
  }
  SC_CHECK(r.contacts() >= 2, "on the ground");
  SC_CHECK(worst < 4.0f, "soft touchdown (hard landing damage starts at 4 m/s): %.2f", worst);
}

SC_TEST(jet_air_density) {
  JetBlueprint bp;
  SC_CHECK(bp.bound, "jet_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  const auto air_density = [&](f64 h) { return (*bp).air_density(h).Result; };
  SC_CHECK(std::fabs(air_density(0) - 1.225f) < 1e-6f, "sea level");
  SC_CHECK(std::fabs(air_density(8500) - 1.225f / 2.718281828f) < 1e-3f, "scale height");
}
