// Car: settles on its springs at the design ride height, accelerates power-limited, brakes with
// ABS, steers to the left with a positive stick, the AI driver reaches a target; the bus too. The car is
// the level's Blueprint car_cpp (its structs: car), called as its class - as the program calls it
// (level_blueprints.h) - on the engine itself: the physics world is the test's, given to it by its handle.
#include "../src/blueprint_call.h"
#include "kern/physics/physics.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
using Cr = blueprints::car_cpp;
constexpr f32 kH = 1.0f / 240.0f;
/// car_cpp running as its class, on the engine itself (RuleBlueprint::Engine::direct).
struct CarBlueprint {
  RuleBlueprint rules{"car_cpp", nullptr, true, {}, RuleBlueprint::Engine::direct};
  bool bound = false;
  CarBlueprint() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Cr>(rules.rt);
  }
  Cr& operator*() { return kern::logic::blueprint<Cr>(); }
};
struct CarRig {
  CarBlueprint bp;
  kern::physics::World world;
  i64 w = 0;
  Cr::physics_Body body;
  Cr::Car car;
  explicit CarRig(bool bus = false) {
    make_flat_world(world);
    if (!bp.bound) return;
    Cr& c = *bp;
    const Cr::CarSpec s = bus ? c.CarSpec_bus().Result : c.CarSpec_car().Result;
    car = c.Car_new(s).Result;
    w = bp.rules.host.objects.adopt(world);
    // (create_body's category 1 and mask ~0, as rigid_spec.h gives them)
    body = c.create_body(c.Car_rigid_spec(s).Result, w, {0, 0, static_cast<f32>(c.Car_ground_offset(s).Result + 0.05)}, {}, Cr::Int64{1}, Cr::Int64{~0ull}).Result;
  }
  ~CarRig() { if (bp.bound) (*bp).Car_end(car); }
  void step(f32 h, const Cr::CarInput& in) { (*bp).Car_step(car, in, w, body, h); }
  void reapply() { (*bp).Car_reapply(car, w, body); }
  void run(f32 seconds, const Cr::CarInput& in) {
    const int n = static_cast<int>(seconds / kH + 0.5f);
    for (int i = 0; i < n; ++i) {
      step(kH, in);
      world.step(kH, 1);
    }
  }
  Cr::BodyState st() { return (*bp).read_body(w, body).Result; }
  f32 forward_speed() { return static_cast<f32>((*bp).Car_forward_speed(car).Result); }
  f32 steer_angle() { return static_cast<f32>((*bp).Car_steer_angle(car).Result); }
  f32 ground_offset() { return static_cast<f32>((*bp).Car_ground_offset(car.s_).Result); }
  f32 heading(Vec3 axis) { return static_cast<f32>((*bp).heading_of(axis).Result); }
  Vec3 rotate(const Cr::Quat& q, Vec3 v) { return (*bp).Quat_rotate(q, v).Result; }
  f32 wrap(f32 a) { return static_cast<f32>((*bp).wrap_pi(a).Result); }
};
/// No pedal, no steering: car.h CarInput{} (the Blueprint's struct has the same defaults: max_speed 32, max_reverse 9).
Cr::CarInput input() { return Cr::CarInput{}; }
}  // namespace

#define CAR_BOUND(r) \
  SC_CHECK((r).bp.bound, "car_cpp runs as its class: %s", (r).bp.rules.problems.c_str()); \
  if (!(r).bp.bound) return

SC_TEST(car_ride_height) {
  CarRig r;
  CAR_BOUND(r);
  r.run(3.0f, input());
  const Cr::BodyState s = r.st();
  SC_CHECK(std::fabs(s.position.z - r.ground_offset()) < 0.05f, "ride height %.3f (design %.3f)", s.position.z,
           r.ground_offset());
  SC_CHECK(kern::length(s.velocity) < 0.05f, "settled %.3f", kern::length(s.velocity));
  for (int i = 0; i < 4; ++i) SC_CHECK(r.car.w_[i].contact, "wheel %d on the ground", i);
}

SC_TEST(car_accelerates_and_brakes) {
  CarRig r;
  CAR_BOUND(r);
  r.run(1.0f, input());
  Cr::CarInput in = input();
  in.pedal = 1;
  r.run(6.0f, in);
  const f32 v6 = r.forward_speed();
  SC_CHECK(v6 > 18.0f && v6 < 32.5f, "0-6 s: %.1f m/s", v6);
  const Vec3 fwd = r.rotate(r.st().rotation, {1, 0, 0});
  SC_CHECK(fwd.x > 0.98f, "straight line: heading x %.3f", fwd.x);
  in.pedal = -1;  // brake (moving forward: brake, not reverse)
  f32 t = 0;
  while (r.forward_speed() > 0.3f && t < 8) { r.run(0.1f, in); t += 0.1f; }
  SC_CHECK(t < 4.5f, "brakes from %.0f m/s in %.1f s", v6, t);
  r.run(2.0f, in);
  SC_CHECK(r.forward_speed() < -0.5f, "then reverses: %.2f m/s", r.forward_speed());
}

SC_TEST(car_steers_left) {
  CarRig r;
  CAR_BOUND(r);
  r.run(1.0f, input());
  Cr::CarInput in = input();
  in.pedal = 0.5f;
  r.run(3.0f, in);
  in.steer = 1;
  const f32 h0 = r.heading(r.rotate(r.st().rotation, {1, 0, 0}));
  r.run(2.0f, in);
  const f32 h1 = r.heading(r.rotate(r.st().rotation, {1, 0, 0}));
  SC_CHECK(r.wrap(h1 - h0) > 0.5f, "turns left: %.2f rad", r.wrap(h1 - h0));
  SC_CHECK(r.steer_angle() > 0.05f, "front wheels point left: %.3f", r.steer_angle());
  const Vec3 up = r.rotate(r.st().rotation, {0, 0, 1});
  SC_CHECK(up.z > 0.9f, "stays on its wheels: %.3f", up.z);
}

SC_TEST(car_ai_driver_reaches_target) {
  CarRig r;
  CAR_BOUND(r);
  r.run(1.0f, input());
  const Vec3 target{60, 40, 0};
  bool reached = false;
  for (int i = 0; i < 240 * 20 && !reached; ++i) {
    const Cr::BodyState s = r.st();
    const Cr::CarInput in = (*r.bp).ai_drive(s.position, r.rotate(s.rotation, {1, 0, 0}), r.forward_speed(), target, 12).Result;
    r.step(kH, in);
    r.world.step(kH, 1);
    reached = std::hypot(s.position.x - target.x, s.position.y - target.y) < 4;
  }
  SC_CHECK(reached, "AI driver reaches the target, ends at %.1f %.1f", r.st().position.x, r.st().position.y);
}

SC_TEST(bus_drives) {
  CarRig r(true);
  CAR_BOUND(r);
  r.run(2.0f, input());
  SC_CHECK(std::fabs(r.st().position.z - r.ground_offset()) < 0.08f, "bus ride height %.3f", r.st().position.z);
  Cr::CarInput in = input();
  in.pedal = 1;
  r.run(8.0f, in);
  SC_CHECK(r.forward_speed() > 8.0f, "bus accelerates: %.1f m/s", r.forward_speed());
}

// The suspension is Box3D's own (wheel joints, soft constraints): a bus that hits its bump stops -
// dropped, or driven over a kerb - loses energy there and stays on the road; it is never thrown up.
SC_TEST(bus_dropped_does_not_bounce_higher) {
  CarRig r(true);
  CAR_BOUND(r);
  r.run(1.5f, input());
  const f32 rest = r.st().position.z;
  (*r.bp).engine_world_set_pose(r.w, r.body, Cr::physics_Pose{{0, 0, rest + 0.8f}, Cr::Quat{0, 0, 0, 1}});
  (*r.bp).engine_world_set_linear_velocity(r.w, r.body, {});
  f32 top_after = -1e9f, low = 1e9f;
  bool landed = false;
  for (int i = 0; i < 240 * 5; ++i) {
    r.step(kH, input());
    r.world.step(kH, 1);
    const Cr::BodyState s = r.st();
    low = std::fmin(low, s.position.z);
    if (!landed && s.velocity.z > 0.05f && s.position.z < rest + 0.3f) landed = true;
    if (landed) top_after = std::fmax(top_after, s.position.z);
  }
  SC_CHECK(landed, "the bus lands");
  SC_CHECK(top_after < rest + 0.35f, "rebound stays low: %.2f m over its ride height (dropped from 0.80)", top_after - rest);
  SC_CHECK(std::fabs(r.st().position.z - rest) < 0.05f && std::fabs(r.st().velocity.z) < 0.05f, "settles at its ride height: %.3f (rest %.3f)", r.st().position.z, rest);
}

SC_TEST(bus_takes_kerbs_without_flying) {
  CarRig r(true);
  CAR_BOUND(r);
  // kerbs across the road, 12 cm high, every 15 m
  for (int k = 0; k < 6; ++k) make_static_box(r.world, {25.0f + 15.0f * k, 0, 0.06f}, {0.6f, 30, 0.12f});
  r.run(1.5f, input());
  const f32 rest = r.st().position.z;
  Cr::CarInput in = input();
  in.pedal = 1;
  in.max_speed = 14;
  f32 top = -1e9f, up = 0;
  for (int i = 0; i < 240 * 14; ++i) {
    r.step(kH, in);
    r.world.step(kH, 1);
    const Cr::BodyState s = r.st();
    top = std::fmax(top, s.position.z);
    up = std::fmax(up, s.velocity.z);
  }
  SC_CHECK(r.st().position.x > 80, "the bus drove over the kerbs: x %.1f", r.st().position.x);
  SC_CHECK(top < rest + 0.45f, "never thrown up: at most %.2f m over its ride height", top - rest);
  SC_CHECK(up < 2.0f, "no upward kick: at most %.2f m/s", up);
}

// Traffic runs its tyres at 60 Hz and holds the forces over the 240 Hz steps in between (game.cpp
// drive_traffic): the same bus, driven that way.
SC_TEST(bus_at_traffic_rate_takes_kerbs) {
  CarRig r(true);
  CAR_BOUND(r);
  for (int k = 0; k < 6; ++k) make_static_box(r.world, {25.0f + 15.0f * k, 0, 0.06f}, {0.6f, 30, 0.12f});
  Cr::CarInput in = input();
  in.pedal = 0.7f;
  in.max_speed = 9;
  f32 top = -1e9f, up = 0, rest = r.ground_offset();
  for (int i = 0; i < 240 * 20; ++i) {
    if (i % 4 == 0) r.step(kH * 4, in);
    else r.reapply();
    r.world.step(kH, 1);
    if (i < 360) continue;
    const Cr::BodyState s = r.st();
    top = std::fmax(top, s.position.z);
    up = std::fmax(up, s.velocity.z);
  }
  SC_CHECK(r.st().position.x > 100, "the bus drove over the kerbs: x %.1f", r.st().position.x);
  SC_CHECK(top < rest + 0.45f, "never thrown up: at most %.2f m over its ride height", top - rest);
  SC_CHECK(up < 2.0f, "no upward kick: at most %.2f m/s", up);
}
