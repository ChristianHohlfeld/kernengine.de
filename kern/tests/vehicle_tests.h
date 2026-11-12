// Kern's vehicle models (kern/game/vehicles): the car on its wheel joints, the quad flown by its
// flight controller firmware, the jet - each alone in a bare physics world.
// (Included by test_main.cpp.)
#include "kern/game/vehicles/car.h"
#include "kern/game/vehicles/drone.h"
#include "kern/game/vehicles/jet.h"

namespace vehicle_tests {
using namespace kern::game::vehicles;

/// A check that says what it measured when it fails.
#define VEHICLE_CHECK(cond, ...)                \
  do {                                          \
    const bool vehicle_ok = (cond);             \
    if (!vehicle_ok) { std::printf("  "); std::printf(__VA_ARGS__); std::printf("\n"); } \
    CHECK(vehicle_ok && #cond);                 \
  } while (0)

// Test worlds: flat ground (a large static box, top at z = 0).
inline kern::physics::Body make_static_box(kern::physics::World& w, Vec3 center, Vec3 size, f32 friction = 0.7f) {
  kern::physics::BodySettings bs;
  bs.type = kern::physics::BodyType::fixed;
  bs.position = center;
  const kern::physics::Body b = w.create_body(bs);
  kern::physics::ShapeSettings ss;
  ss.kind = kern::physics::ShapeKind::box;
  ss.size = size;
  ss.material.friction = friction;
  w.create_shape(b, ss);
  return b;
}
inline void make_flat_world(kern::physics::World& w) {
  kern::physics::WorldSettings ws;
  w.create(ws);
  make_static_box(w, {0, 0, -1}, {6000, 6000, 2});
}

// ---- car ----------------------------------------------------------------------
// Car: settles on its springs at the design ride height, accelerates power-limited, brakes with
// ABS, steers to the left with a positive stick, the AI driver reaches a target; the bus too.
namespace car_tests {
namespace car_rig {
constexpr f32 kH = 1.0f / 240.0f;
struct CarRig {
  kern::physics::World world;
  kern::physics::Body body;
  Car car;
  explicit CarRig(const CarSpec& s = CarSpec::car()) : car(s) {
    make_flat_world(world);
    body = create_body(world, Car::rigid_spec(s), {0, 0, Car::ground_offset(s) + 0.05f}, {});
  }
  void run(f32 seconds, const CarInput& in) {
    const int n = static_cast<int>(seconds / kH + 0.5f);
    for (int i = 0; i < n; ++i) {
      car.step(world, body, kH, in);
      world.step(kH, 1);
    }
  }
  BodyState st() const { return read_body(world, body); }
};
}  // namespace car_rig
using namespace car_rig;

TEST(vehicle_car_ride_height) {
  CarRig r;
  r.run(3.0f, {});
  const BodyState s = r.st();
  VEHICLE_CHECK(std::fabs(s.position.z - Car::ground_offset(r.car.spec())) < 0.05f, "ride height %.3f (design %.3f)", s.position.z,
           Car::ground_offset(r.car.spec()));
  VEHICLE_CHECK(kern::length(s.velocity) < 0.05f, "settled %.3f", kern::length(s.velocity));
  for (int i = 0; i < 4; ++i) VEHICLE_CHECK(r.car.wheel(i).contact, "wheel %d on the ground", i);
}

TEST(vehicle_car_accelerates_and_brakes) {
  CarRig r;
  r.run(1.0f, {});
  CarInput in;
  in.pedal = 1;
  r.run(6.0f, in);
  const f32 v6 = r.car.forward_speed();
  VEHICLE_CHECK(v6 > 18.0f && v6 < 32.5f, "0-6 s: %.1f m/s", v6);
  const Vec3 fwd = r.st().rotation.rotate({1, 0, 0});
  VEHICLE_CHECK(fwd.x > 0.98f, "straight line: heading x %.3f", fwd.x);
  in.pedal = -1;  // brake (moving forward: brake, not reverse)
  f32 t = 0;
  while (r.car.forward_speed() > 0.3f && t < 8) { r.run(0.1f, in); t += 0.1f; }
  VEHICLE_CHECK(t < 4.5f, "brakes from %.0f m/s in %.1f s", v6, t);
  r.run(2.0f, in);
  VEHICLE_CHECK(r.car.forward_speed() < -0.5f, "then reverses: %.2f m/s", r.car.forward_speed());
}

TEST(vehicle_car_steers_left) {
  CarRig r;
  r.run(1.0f, {});
  CarInput in;
  in.pedal = 0.5f;
  r.run(3.0f, in);
  in.steer = 1;
  const f32 h0 = heading_of(r.st().rotation.rotate({1, 0, 0}));
  r.run(2.0f, in);
  const f32 h1 = heading_of(r.st().rotation.rotate({1, 0, 0}));
  VEHICLE_CHECK(wrap_pi(h1 - h0) > 0.5f, "turns left: %.2f rad", wrap_pi(h1 - h0));
  VEHICLE_CHECK(r.car.steer_angle() > 0.05f, "front wheels point left: %.3f", r.car.steer_angle());
  const Vec3 up = r.st().rotation.rotate({0, 0, 1});
  VEHICLE_CHECK(up.z > 0.9f, "stays on its wheels: %.3f", up.z);
}

TEST(vehicle_car_ai_driver_reaches_target) {
  CarRig r;
  r.run(1.0f, {});
  const Vec3 target{60, 40, 0};
  bool reached = false;
  for (int i = 0; i < 240 * 20 && !reached; ++i) {
    const BodyState s = r.st();
    const CarInput in = ai_drive(s.position, s.rotation.rotate({1, 0, 0}), r.car.forward_speed(), target, 12);
    r.car.step(r.world, r.body, kH, in);
    r.world.step(kH, 1);
    reached = std::hypot(s.position.x - target.x, s.position.y - target.y) < 4;
  }
  VEHICLE_CHECK(reached, "AI driver reaches the target, ends at %.1f %.1f", r.st().position.x, r.st().position.y);
}

TEST(vehicle_bus_drives) {
  CarRig r(CarSpec::bus());
  r.run(2.0f, {});
  VEHICLE_CHECK(std::fabs(r.st().position.z - Car::ground_offset(r.car.spec())) < 0.08f, "bus ride height %.3f", r.st().position.z);
  CarInput in;
  in.pedal = 1;
  r.run(8.0f, in);
  VEHICLE_CHECK(r.car.forward_speed() > 8.0f, "bus accelerates: %.1f m/s", r.car.forward_speed());
}

// The suspension is Box3D's own (wheel joints, soft constraints): a bus that hits its bump stops -
// dropped, or driven over a kerb - loses energy there and stays on the road; it is never thrown up.
TEST(vehicle_bus_dropped_does_not_bounce_higher) {
  CarRig r(CarSpec::bus());
  r.run(1.5f, {});
  const f32 rest = r.st().position.z;
  r.world.set_pose(r.body, {{0, 0, rest + 0.8f}, Quat{}});
  r.world.set_linear_velocity(r.body, {});
  f32 top_after = -1e9f, low = 1e9f;
  bool landed = false;
  for (int i = 0; i < 240 * 5; ++i) {
    r.car.step(r.world, r.body, kH, {});
    r.world.step(kH, 1);
    const BodyState s = r.st();
    low = std::fmin(low, s.position.z);
    if (!landed && s.velocity.z > 0.05f && s.position.z < rest + 0.3f) landed = true;
    if (landed) top_after = std::fmax(top_after, s.position.z);
  }
  VEHICLE_CHECK(landed, "the bus lands");
  VEHICLE_CHECK(top_after < rest + 0.35f, "rebound stays low: %.2f m over its ride height (dropped from 0.80)", top_after - rest);
  VEHICLE_CHECK(std::fabs(r.st().position.z - rest) < 0.05f && std::fabs(r.st().velocity.z) < 0.05f, "settles at its ride height: %.3f (rest %.3f)", r.st().position.z, rest);
}

TEST(vehicle_bus_takes_kerbs_without_flying) {
  CarRig r(CarSpec::bus());
  // kerbs across the road, 12 cm high, every 15 m
  for (int k = 0; k < 6; ++k) make_static_box(r.world, {25.0f + 15.0f * k, 0, 0.06f}, {0.6f, 30, 0.12f});
  r.run(1.5f, {});
  const f32 rest = r.st().position.z;
  CarInput in;
  in.pedal = 1;
  in.max_speed = 14;
  f32 top = -1e9f, up = 0;
  for (int i = 0; i < 240 * 14; ++i) {
    r.car.step(r.world, r.body, kH, in);
    r.world.step(kH, 1);
    const BodyState s = r.st();
    top = std::fmax(top, s.position.z);
    up = std::fmax(up, s.velocity.z);
  }
  VEHICLE_CHECK(r.st().position.x > 80, "the bus drove over the kerbs: x %.1f", r.st().position.x);
  VEHICLE_CHECK(top < rest + 0.45f, "never thrown up: at most %.2f m over its ride height", top - rest);
  VEHICLE_CHECK(up < 2.0f, "no upward kick: at most %.2f m/s", up);
}

// Traffic runs its tyres at 60 Hz and holds the forces over the 240 Hz steps in between (game.cpp
// drive_traffic): the same bus, driven that way.
TEST(vehicle_bus_at_traffic_rate_takes_kerbs) {
  CarRig r(CarSpec::bus());
  for (int k = 0; k < 6; ++k) make_static_box(r.world, {25.0f + 15.0f * k, 0, 0.06f}, {0.6f, 30, 0.12f});
  CarInput in;
  in.pedal = 0.7f;
  in.max_speed = 9;
  f32 top = -1e9f, up = 0, rest = Car::ground_offset(r.car.spec());
  for (int i = 0; i < 240 * 20; ++i) {
    if (i % 4 == 0) r.car.step(r.world, r.body, kH * 4, in);
    else r.car.reapply(r.world, r.body);
    r.world.step(kH, 1);
    if (i < 360) continue;
    const BodyState s = r.st();
    top = std::fmax(top, s.position.z);
    up = std::fmax(up, s.velocity.z);
  }
  VEHICLE_CHECK(r.st().position.x > 100, "the bus drove over the kerbs: x %.1f", r.st().position.x);
  VEHICLE_CHECK(top < rest + 0.45f, "never thrown up: at most %.2f m over its ride height", top - rest);
  VEHICLE_CHECK(up < 2.0f, "no upward kick: at most %.2f m/s", up);
}
}  // namespace car_tests

// ---- drone ----------------------------------------------------------------------
// Drone: the Arondight45 firmware calibrates, arms, holds height in GAME mode, flies forward along
// the nose and lands; the plant's static thrust matches SpawnCity's propulsion authority.
namespace drone_tests {
namespace drone_rig {
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
      world.step(kH, 1);
    }
  }
  BodyState state() const { return read_body(world, body); }
};
}  // namespace drone_rig
using namespace drone_rig;

TEST(vehicle_drone_mass_properties) {
  const RigidSpec r = Drone::rigid_spec();
  VEHICLE_CHECK(std::fabs(r.mass.mass - 0.72f) < 0.01f, "mass %.3f", r.mass.mass);
  VEHICLE_CHECK(r.mass.inertia[0] > 0 && r.mass.inertia[1] > 0 && r.mass.inertia[2] > r.mass.inertia[0], "inertia %g %g %g", r.mass.inertia[0],
           r.mass.inertia[1], r.mass.inertia[2]);
}

TEST(vehicle_drone_calibrates_arms_and_hovers) {
  Rig rig;
  DroneSticks s;
  s.game_mode = true;
  s.clearance = 3.0f;
  rig.run(2.5f, s);  // 2000 calibration samples at ~1 kHz with the arm switch low
  VEHICLE_CHECK(!rig.drone.telemetry().calibrating, "still calibrating");
  VEHICLE_CHECK(!rig.drone.telemetry().fault, "fault state %u", rig.drone.telemetry().state);
  s.arm = true;
  rig.run(1.5f, s);
  VEHICLE_CHECK(rig.drone.telemetry().armed, "not armed, state %u", rig.drone.telemetry().state);
  rig.run(6.0f, s);
  const BodyState st = rig.state();
  VEHICLE_CHECK(std::fabs(st.position.z - 3.0f) < 0.6f, "hover height %.2f", st.position.z);
  VEHICLE_CHECK(kern::length(st.velocity) < 0.6f, "hover drift %.2f m/s", kern::length(st.velocity));
  const Vec3 up = st.rotation.rotate({0, 0, 1});
  VEHICLE_CHECK(up.z > 0.97f, "level hover up.z %.3f", up.z);
}

TEST(vehicle_drone_flies_along_its_nose) {
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
  VEHICLE_CHECK(along > 12.0f, "forward speed %.1f m/s (v %.1f %.1f %.1f)", along, st.velocity.x, st.velocity.y, st.velocity.z);
  VEHICLE_CHECK(along < 27.0f, "GAME envelope is 25 m/s, got %.1f", along);
  VEHICLE_CHECK(st.position.z > 2.0f, "keeps height while cruising %.2f", st.position.z);
  s.pitch = 0;
  rig.run(3.0f, s);
  const f32 v3 = kern::length(rig.state().velocity);
  rig.run(5.0f, s);
  VEHICLE_CHECK(kern::length(rig.state().velocity) < 1.0f && kern::length(rig.state().velocity) < v3 + 0.1f, "brakes to a hover, %.2f m/s (after 3 s: %.2f)", kern::length(rig.state().velocity), v3);
}

TEST(vehicle_drone_yaw_and_climb) {
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
  VEHICLE_CHECK(wrap_pi(yaw1 - yaw0) > 0.4f, "positive yaw stick turns the nose left (counter-clockwise from above): %.2f rad", wrap_pi(yaw1 - yaw0));
  s.yaw = 0;
  s.clearance = 20.0f;
  rig.run(8.0f, s);
  VEHICLE_CHECK(std::fabs(rig.state().position.z - 20.0f) < 2.0f, "climbs to 20 m: %.2f", rig.state().position.z);
}

TEST(vehicle_drone_angle_mode_throttle) {
  Rig rig;
  DroneSticks s;
  s.game_mode = false;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(1.5f, s);
  VEHICLE_CHECK(rig.drone.telemetry().armed, "armed in ANGLE mode");
  s.throttle = 0.7f;
  rig.run(1.0f, s);
  VEHICLE_CHECK(rig.state().position.z > 2.0f, "full-ish throttle climbs: %.2f", rig.state().position.z);
  VEHICLE_CHECK(rig.drone.telemetry().current_a > 20.0f, "draws current %.1f A", rig.drone.telemetry().current_a);
  VEHICLE_CHECK(rig.drone.telemetry().battery_v < 16.8f, "pack sags %.2f V", rig.drone.telemetry().battery_v);
}

TEST(vehicle_drone_clearance_law) {
  VEHICLE_CHECK(clearance_rate(0.05f) == 0, "deadband");
  VEHICLE_CHECK(std::fabs(clearance_rate(1.0f) - 12.0f) < 1e-4f, "full stick = 12 m/s");
  VEHICLE_CHECK(clearance_lead(0) == 0, "no lead at rest");
  VEHICLE_CHECK(clearance_lead(4) > 2 && clearance_lead(-4) < -2, "lead %.2f %.2f", clearance_lead(4), clearance_lead(-4));
}

TEST(vehicle_drone_roll_moves_right) {
  Rig rig;
  DroneSticks s;
  s.clearance = 3.0f;
  rig.run(2.5f, s);
  s.arm = true;
  rig.run(4.0f, s);
  s.roll = 1.0f;
  rig.run(2.0f, s);
  const BodyState st = rig.state();
  const Vec3 nose = st.rotation.rotate({-1, 0, 0}), right = kern::cross(nose, Vec3{0, 0, 1});
  VEHICLE_CHECK(kern::dot(st.velocity, right) > 5.0f, "positive roll flies to the right of the nose: %.1f m/s", kern::dot(st.velocity, right));
}
}  // namespace drone_tests

// ---- jet ----------------------------------------------------------------------
// Jet: stands on its gear, takes off vertically, hovers, transitions to wing-borne flight,
// climbs with the stick, and lands softly jet-borne.
namespace jet_tests {
namespace jet_rig {
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
      world.step(kH, 1);
    }
  }
  BodyState st() const { return read_body(world, body); }
};
}  // namespace jet_rig
using namespace jet_rig;

TEST(vehicle_jet_rests_on_gear) {
  JetRig r;
  r.run(3.0f, {});
  const BodyState s = r.st();
  VEHICLE_CHECK(r.jet.telemetry().contacts == 3, "three wheels on the ground (%d)", r.jet.telemetry().contacts);
  VEHICLE_CHECK(std::fabs(s.position.z - 2.05f) < 0.15f, "rests at gear height: %.2f", s.position.z);
  VEHICLE_CHECK(kern::length(s.velocity) < 0.05f, "at rest %.3f", kern::length(s.velocity));
}

TEST(vehicle_jet_vertical_takeoff_and_hover) {
  JetRig r;
  r.run(1.0f, {});
  JetInput in;
  in.lift = 1;
  r.run(3.0f, in);
  VEHICLE_CHECK(r.st().position.z > 12.0f, "climbs jet-borne: %.1f m", r.st().position.z);
  in.lift = 0;
  r.run(4.0f, in);
  const BodyState s = r.st();
  VEHICLE_CHECK(std::fabs(s.velocity.z) < 1.0f, "holds height: vz %.2f", s.velocity.z);
  VEHICLE_CHECK(std::hypot(s.velocity.x, s.velocity.y) < 1.5f, "no drift: %.2f", std::hypot(s.velocity.x, s.velocity.y));
  const Vec3 up = s.rotation.rotate({0, 0, 1});
  VEHICLE_CHECK(up.z > 0.97f, "level: %.3f", up.z);
}

TEST(vehicle_jet_transition_and_climb) {
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
  VEHICLE_CHECK(V > 120.0f, "wing-borne speed %.0f m/s", V);
  VEHICLE_CHECK(r.jet.telemetry().w_flight > 0.99f, "wing-borne law %.2f", r.jet.telemetry().w_flight);
  VEHICLE_CHECK(s.position.z > 10.0f, "did not sink into the ground: %.0f m", s.position.z);
  const f32 z0 = s.position.z;
  in.pitch = 0.4f;  // arcade: the stick is a rate (1.6 rad/s full) - a short pull, then level climb
  r.run(1.0f, in);
  in.pitch = 0;
  r.run(2.0f, in);
  VEHICLE_CHECK(r.st().position.z > z0 + 40.0f, "pulls up: %.0f -> %.0f m", z0, r.st().position.z);
  in.roll = 0.3f;
  const f32 h0 = r.jet.telemetry().heading;
  r.run(1.0f, in);
  in.roll = 0;
  r.run(3.0f, in);
  VEHICLE_CHECK(std::fabs(wrap_pi(r.jet.telemetry().heading - h0)) > 0.1f, "banks and turns: %.2f rad", wrap_pi(r.jet.telemetry().heading - h0));
}

TEST(vehicle_jet_arcade_dogfight_turns) {
  // arcade handling: the hover turns on the spot, wing-borne a bank and a pull bring it round fast
  JetRig r;
  JetInput in;
  in.lift = 1;
  r.run(3.0f, in);
  in.lift = 0;
  r.run(1.0f, in);
  f32 h0 = r.jet.telemetry().heading;
  in.yaw = 1;
  r.run(1.5f, in);
  in.yaw = 0;
  VEHICLE_CHECK(std::fabs(wrap_pi(r.jet.telemetry().heading - h0)) > 1.5f, "hover yaw: %.2f rad in 1.5 s", wrap_pi(r.jet.telemetry().heading - h0));
  r.run(1.0f, in);
  r.jet.set_mode(Jet::Mode::flight);
  in.lift = 1;
  r.run(10.0f, in);
  in.lift = 0;
  h0 = r.jet.telemetry().heading;
  in.roll = 1;
  r.run(0.45f, in);
  in.roll = 0;
  in.pitch = 1;
  r.run(2.5f, in);
  in.pitch = 0;
  const f32 turned = std::fabs(wrap_pi(r.jet.telemetry().heading - h0));
  VEHICLE_CHECK(turned > 2.0f, "a bank and a pull: %.2f rad in 3 s", turned);
  VEHICLE_CHECK(r.st().position.z > 5.0f, "still in the air: %.0f m", r.st().position.z);
}

TEST(vehicle_jet_soft_landing) {
  JetRig r;
  JetInput in;
  in.lift = 1;
  r.run(2.5f, in);
  in.lift = -1;  // descend: the sink rate tapers near the ground
  f32 worst = 0;
  for (int i = 0; i < 240 * 10; ++i) {
    r.jet.step(r.world, r.body, kH, in);
    r.world.step(kH, 1);
    worst = std::fmax(worst, r.jet.gear_impact());
  }
  VEHICLE_CHECK(r.jet.telemetry().contacts >= 2, "on the ground");
  VEHICLE_CHECK(worst < 4.0f, "soft touchdown (hard landing damage starts at 4 m/s): %.2f", worst);
}

TEST(vehicle_jet_air_density) {
  VEHICLE_CHECK(std::fabs(air_density(0) - 1.225f) < 1e-6f, "sea level");
  VEHICLE_CHECK(std::fabs(air_density(8500) - 1.225f / 2.718281828f) < 1e-3f, "scale height");
}
}  // namespace jet_tests

}  // namespace vehicle_tests
