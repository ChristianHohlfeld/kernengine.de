// Jet: stands on its gear, takes off vertically, hovers, transitions to wing-borne flight,
// climbs with the stick, and lands softly jet-borne.
#include "../src/jet.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
constexpr f32 kH = 1.0f / 240.0f;
struct JetRig {
  kern::physics::World world;
  kern::physics::Body body;
  Jet jet;
  JetRig() {
    make_flat_world(world);
    body = create_body(world, Jet::rigid_spec(), {0, 0, 2.1f}, {});
  }
  void run(f32 seconds, const JetInput& in) {
    const int n = static_cast<int>(seconds / kH + 0.5f);
    for (int i = 0; i < n; ++i) {
      jet.step(world, body, kH, in);
      world.step(kH, 4);
    }
  }
  BodyState st() const { return read_body(world, body); }
};
}  // namespace

SC_TEST(jet_rests_on_gear) {
  JetRig r;
  r.run(3.0f, {});
  const BodyState s = r.st();
  SC_CHECK(r.jet.telemetry().contacts == 3, "three wheels on the ground (%d)", r.jet.telemetry().contacts);
  SC_CHECK(std::fabs(s.position.z - 2.05f) < 0.15f, "rests at gear height: %.2f", s.position.z);
  SC_CHECK(kern::length(s.velocity) < 0.05f, "at rest %.3f", kern::length(s.velocity));
}

SC_TEST(jet_vertical_takeoff_and_hover) {
  JetRig r;
  r.run(1.0f, {});
  JetInput in;
  in.lift = 1;
  r.run(3.0f, in);
  SC_CHECK(r.st().position.z > 12.0f, "climbs jet-borne: %.1f m", r.st().position.z);
  in.lift = 0;
  r.run(4.0f, in);
  const BodyState s = r.st();
  SC_CHECK(std::fabs(s.velocity.z) < 1.0f, "holds height: vz %.2f", s.velocity.z);
  SC_CHECK(std::hypot(s.velocity.x, s.velocity.y) < 1.5f, "no drift: %.2f", std::hypot(s.velocity.x, s.velocity.y));
  const Vec3 up = s.rotation.rotate({0, 0, 1});
  SC_CHECK(up.z > 0.97f, "level: %.3f", up.z);
}

SC_TEST(jet_transition_and_climb) {
  JetRig r;
  JetInput in;
  in.lift = 1;
  r.run(4.0f, in);  // up to ~30 m
  in.lift = 0;
  r.run(1.0f, in);
  r.jet.set_mode(Jet::Mode::flight);
  in.lift = 1;      // lever forward
  r.run(14.0f, in);
  const BodyState s = r.st();
  const f32 V = kern::length(s.velocity);
  SC_CHECK(V > 120.0f, "wing-borne speed %.0f m/s", V);
  SC_CHECK(r.jet.telemetry().w_flight > 0.99f, "wing-borne law %.2f", r.jet.telemetry().w_flight);
  SC_CHECK(s.position.z > 10.0f, "did not sink into the ground: %.0f m", s.position.z);
  const f32 z0 = s.position.z;
  in.pitch = 0.6f;
  r.run(2.0f, in);
  in.pitch = 0;
  r.run(2.0f, in);
  SC_CHECK(r.st().position.z > z0 + 40.0f, "pulls up: %.0f -> %.0f m", z0, r.st().position.z);
  in.roll = 0.3f;
  const f32 h0 = r.jet.telemetry().heading;
  r.run(1.0f, in);
  in.roll = 0;
  r.run(3.0f, in);
  SC_CHECK(std::fabs(wrap_pi(r.jet.telemetry().heading - h0)) > 0.1f, "banks and turns: %.2f rad", wrap_pi(r.jet.telemetry().heading - h0));
}

SC_TEST(jet_soft_landing) {
  JetRig r;
  JetInput in;
  in.lift = 1;
  r.run(2.5f, in);
  in.lift = -1;  // descend: the sink rate tapers near the ground
  f32 worst = 0;
  for (int i = 0; i < 240 * 10; ++i) {
    r.jet.step(r.world, r.body, kH, in);
    r.world.step(kH, 4);
    worst = std::fmax(worst, r.jet.gear_impact());
  }
  SC_CHECK(r.jet.telemetry().contacts >= 2, "on the ground");
  SC_CHECK(worst < 4.0f, "soft touchdown (hard landing damage starts at 4 m/s): %.2f", worst);
}

SC_TEST(jet_air_density) {
  SC_CHECK(std::fabs(air_density(0) - 1.225f) < 1e-6f, "sea level");
  SC_CHECK(std::fabs(air_density(8500) - 1.225f / 2.718281828f) < 1e-3f, "scale height");
}
