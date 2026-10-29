// Car: settles on its springs at the design ride height, accelerates power-limited, brakes with
// ABS, steers to the left with a positive stick, the AI driver reaches a target; the bus too.
#include "../src/car.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
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
      world.step(kH, 4);
    }
  }
  BodyState st() const { return read_body(world, body); }
};
}  // namespace

SC_TEST(car_ride_height) {
  CarRig r;
  r.run(3.0f, {});
  const BodyState s = r.st();
  SC_CHECK(std::fabs(s.position.z - Car::ground_offset(r.car.spec())) < 0.05f, "ride height %.3f (design %.3f)", s.position.z,
           Car::ground_offset(r.car.spec()));
  SC_CHECK(kern::length(s.velocity) < 0.05f, "settled %.3f", kern::length(s.velocity));
  for (int i = 0; i < 4; ++i) SC_CHECK(r.car.wheel(i).contact, "wheel %d on the ground", i);
}

SC_TEST(car_accelerates_and_brakes) {
  CarRig r;
  r.run(1.0f, {});
  CarInput in;
  in.pedal = 1;
  r.run(6.0f, in);
  const f32 v6 = r.car.forward_speed();
  SC_CHECK(v6 > 18.0f && v6 < 32.5f, "0-6 s: %.1f m/s", v6);
  const Vec3 fwd = r.st().rotation.rotate({1, 0, 0});
  SC_CHECK(fwd.x > 0.98f, "straight line: heading x %.3f", fwd.x);
  in.pedal = -1;  // brake (moving forward: brake, not reverse)
  f32 t = 0;
  while (r.car.forward_speed() > 0.3f && t < 8) { r.run(0.1f, in); t += 0.1f; }
  SC_CHECK(t < 4.5f, "brakes from %.0f m/s in %.1f s", v6, t);
  r.run(2.0f, in);
  SC_CHECK(r.car.forward_speed() < -0.5f, "then reverses: %.2f m/s", r.car.forward_speed());
}

SC_TEST(car_steers_left) {
  CarRig r;
  r.run(1.0f, {});
  CarInput in;
  in.pedal = 0.5f;
  r.run(3.0f, in);
  in.steer = 1;
  const f32 h0 = heading_of(r.st().rotation.rotate({1, 0, 0}));
  r.run(2.0f, in);
  const f32 h1 = heading_of(r.st().rotation.rotate({1, 0, 0}));
  SC_CHECK(wrap_pi(h1 - h0) > 0.5f, "turns left: %.2f rad", wrap_pi(h1 - h0));
  SC_CHECK(r.car.steer_angle() > 0.05f, "front wheels point left: %.3f", r.car.steer_angle());
  const Vec3 up = r.st().rotation.rotate({0, 0, 1});
  SC_CHECK(up.z > 0.9f, "stays on its wheels: %.3f", up.z);
}

SC_TEST(car_ai_driver_reaches_target) {
  CarRig r;
  r.run(1.0f, {});
  const Vec3 target{60, 40, 0};
  bool reached = false;
  for (int i = 0; i < 240 * 20 && !reached; ++i) {
    const BodyState s = r.st();
    const CarInput in = ai_drive(s.position, s.rotation.rotate({1, 0, 0}), r.car.forward_speed(), target, 12);
    r.car.step(r.world, r.body, kH, in);
    r.world.step(kH, 4);
    reached = std::hypot(s.position.x - target.x, s.position.y - target.y) < 4;
  }
  SC_CHECK(reached, "AI driver reaches the target, ends at %.1f %.1f", r.st().position.x, r.st().position.y);
}

SC_TEST(bus_drives) {
  CarRig r(CarSpec::bus());
  r.run(2.0f, {});
  SC_CHECK(std::fabs(r.st().position.z - Car::ground_offset(r.car.spec())) < 0.08f, "bus ride height %.3f", r.st().position.z);
  CarInput in;
  in.pedal = 1;
  r.run(8.0f, in);
  SC_CHECK(r.car.forward_speed() > 8.0f, "bus accelerates: %.1f m/s", r.car.forward_speed());
}
