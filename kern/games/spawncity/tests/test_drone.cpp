// Drone: the Arondight45 firmware calibrates, arms, holds height in GAME mode, flies forward along
// the nose and lands; the plant's static thrust matches SpawnCity's propulsion authority.
#include "../src/drone.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
constexpr f32 kH = 1.0f / 240.0f;

struct Rig {
  kern::physics::World world;
  kern::physics::Body body;
  Drone drone;
  Rig() {
    make_flat_world(world);
    body = create_body(world, Drone::rigid_spec(), {0, 0, 0.0225f}, {});
  }
  void run(f32 seconds, const DroneSticks& s) {
    const int n = static_cast<int>(seconds / kH + 0.5f);
    for (int i = 0; i < n; ++i) {
      drone.step(world, body, kH, s);
      world.step(kH, 4);
    }
  }
  BodyState state() const { return read_body(world, body); }
};
}  // namespace

SC_TEST(drone_mass_properties) {
  const RigidSpec r = Drone::rigid_spec();
  SC_CHECK(std::fabs(r.mass.mass - 0.72f) < 0.01f, "mass %.3f", r.mass.mass);
  SC_CHECK(r.mass.inertia[0] > 0 && r.mass.inertia[1] > 0 && r.mass.inertia[2] > r.mass.inertia[0], "inertia %g %g %g", r.mass.inertia[0],
           r.mass.inertia[1], r.mass.inertia[2]);
}

SC_TEST(drone_calibrates_arms_and_hovers) {
  Rig rig;
  DroneSticks s;
  s.game_mode = true;
  s.clearance = 3.0f;
  rig.run(2.5f, s);  // 2000 calibration samples at ~1 kHz with the arm switch low
  SC_CHECK(!rig.drone.telemetry().calibrating, "still calibrating");
  SC_CHECK(!rig.drone.telemetry().fault, "fault state %u", rig.drone.telemetry().state);
  s.arm = true;
  rig.run(1.5f, s);
  SC_CHECK(rig.drone.telemetry().armed, "not armed, state %u", rig.drone.telemetry().state);
  rig.run(6.0f, s);
  const BodyState st = rig.state();
  SC_CHECK(std::fabs(st.position.z - 3.0f) < 0.6f, "hover height %.2f", st.position.z);
  SC_CHECK(kern::length(st.velocity) < 0.6f, "hover drift %.2f m/s", kern::length(st.velocity));
  const Vec3 up = st.rotation.rotate({0, 0, 1});
  SC_CHECK(up.z > 0.97f, "level hover up.z %.3f", up.z);
}

SC_TEST(drone_flies_along_its_nose) {
  Rig rig;
  DroneSticks s;
  s.clearance = 4.0f;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(5.0f, s);
  s.pitch = 1.0f;  // full forward
  rig.run(4.0f, s);
  const BodyState st = rig.state();
  const Vec3 nose = st.rotation.rotate({-1, 0, 0});
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
  DroneSticks s;
  s.clearance = 2.0f;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(4.0f, s);
  const f32 yaw0 = heading_of(rig.state().rotation.rotate({-1, 0, 0}));
  s.yaw = 0.5f;
  rig.run(1.0f, s);
  const f32 yaw1 = heading_of(rig.state().rotation.rotate({-1, 0, 0}));
  SC_CHECK(std::fabs(wrap_pi(yaw1 - yaw0)) > 0.4f, "yaw rate command turns the nose (%.2f rad)", wrap_pi(yaw1 - yaw0));
  s.yaw = 0;
  s.clearance = 20.0f;
  rig.run(8.0f, s);
  SC_CHECK(std::fabs(rig.state().position.z - 20.0f) < 2.0f, "climbs to 20 m: %.2f", rig.state().position.z);
}

SC_TEST(drone_angle_mode_throttle) {
  Rig rig;
  DroneSticks s;
  s.game_mode = false;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(1.5f, s);
  SC_CHECK(rig.drone.telemetry().armed, "armed in ANGLE mode");
  s.throttle = 0.7f;
  rig.run(1.0f, s);
  SC_CHECK(rig.state().position.z > 2.0f, "full-ish throttle climbs: %.2f", rig.state().position.z);
  SC_CHECK(rig.drone.telemetry().current_a > 20.0f, "draws current %.1f A", rig.drone.telemetry().current_a);
  SC_CHECK(rig.drone.telemetry().battery_v < 16.8f, "pack sags %.2f V", rig.drone.telemetry().battery_v);
}

SC_TEST(drone_clearance_law) {
  SC_CHECK(clearance_rate(0.05f) == 0, "deadband");
  SC_CHECK(std::fabs(clearance_rate(1.0f) - 12.0f) < 1e-4f, "full stick = 12 m/s");
  SC_CHECK(clearance_lead(0) == 0, "no lead at rest");
  SC_CHECK(clearance_lead(4) > 2 && clearance_lead(-4) < -2, "lead %.2f %.2f", clearance_lead(4), clearance_lead(-4));
}
