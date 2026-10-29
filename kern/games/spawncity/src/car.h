// Cars and buses: SpawnCity's vehicle dynamics (sim/world_vehicle_dynamics.mjs v4.1) as a
// ray-cast vehicle on one Box3D chassis.
//
// SpawnCity hung four wheel bodies on Box3D wheel joints with a steering motor. Kern's physics
// layer exposes the wheel joint without its steering (later in the engine, see README), so the
// suspension here is the same spring and damper per corner acting along a ray from the chassis
// mount: stiffness q*g/sag (q = a quarter of the mass), body damping ratio 0.45, travel limits
// with bump stops. Everything above the suspension is SpawnCity's model unchanged: per-wheel spin
// degree of freedom (I*dw/dt = T_engine - T_brake - Fx*r, integrated implicitly in 5 substeps),
// the combined-slip tyre curve (kappa_peak 0.1, alpha_peak 6.5 deg, sliding grip 0.78, load
// sensitivity 0.15), traction control and ABS at 11 % slip, power-limited low gear, automatic
// D/R that only changes below 0.65 m/s, electric power steering solved as the equilibrium of
// hand torque + assist against the tyres' aligning torque, rack rate limit, aero drag and rolling
// resistance. The tyre forces act on the chassis at the contact patches, so load transfer, pitch,
// roll and yaw come out of Box3D.
//
// Chassis frame: +X forward, +Y left, +Z up.
#pragma once

#include "common.h"
#include "rigid_spec.h"

namespace sc {

struct CarSpec {
  const char* kind = "car";
  Vec3 half{1.78f, 0.82f, 0.36f};
  f32 mass = 1350, com_z = -0.06f, com_x = 0.15f;
  f32 radius = 0.34f, wheel_width = 0.22f, wheel_mass = 22, attach_z = -0.26f;
  struct Wheel { f32 x, y; bool front; };
  Wheel wheels[4] = {{1.2f, 0.78f, true}, {1.2f, -0.78f, true}, {-1.15f, 0.78f, false}, {-1.15f, -0.78f, false}};
  f32 susp_lower = -0.15f, susp_upper = 0.11f, sag = 0.06f;
  f32 torque_front = 550, torque_rear = 850, power = 140000;
  f32 steer_lag = 0.15f, steer_damp_speed = 15, drag_area = 0.68f, rolling_resistance = 0.013f, wheel_inertia = 1.3f;
  f32 brake = 2400, handbrake = 3200, coast = 30, steer_lock = 0.58f, steer_hand = 430, steer_assist = 600, steer_assist_speed = 6, friction = 1.1f;
  f32 rack_rate = 2.8f;
  f32 upper_mass = 95;  // the greenhouse (glass, pillars, roof) as its own hull
  std::vector<Vec2> upper_profile{{-1.02f, 0.8f}, {-0.64f, 1.37f}, {0.3f, 1.37f}, {0.86f, 0.8f}};
  f32 upper_half_width = 0.71f, upper_dz = 0.54f;

  static CarSpec car() { return {}; }
  static CarSpec bus();
};

struct CarInput {
  f32 pedal = 0;      // -1..1: throttle forward / brake / reverse (automatic)
  f32 steer = 0;      // -1..1 (+ = left)
  bool handbrake = false;
  f32 max_speed = 32; // m/s rev limiter (player: 115 km/h)
  f32 max_reverse = 9;
};

struct WheelState {
  f32 omega = 0, angle = 0, travel = 0, load = 0, load_f = -1, fx = 0, fy = 0, kappa = 0, alpha = 0;
  bool contact = false, has_travel = false;
  Vec3 hub;  // world position for drawing
};

class Car {
 public:
  explicit Car(const CarSpec& s = {}) : s_(s) {}
  static RigidSpec rigid_spec(const CarSpec& s);
  /// Height of the chassis centre above flat ground at rest.
  static f32 ground_offset(const CarSpec& s) { return s.radius - s.attach_z - s.sag; }

  void reset();
  void step(kern::physics::World& world, kern::physics::Body body, f32 h, const CarInput& in);
  const CarSpec& spec() const { return s_; }
  const WheelState& wheel(int i) const { return w_[i]; }
  f32 steer_angle() const { return steer_; }
  f32 forward_speed() const { return vf_; }
  i32 gear() const { return gear_; }
  /// Engine load 0..1 and the wheel speed for the engine sound.
  f32 throttle() const { return throttle_; }
  f32 wheel_speed() const;

 private:
  CarSpec s_;
  WheelState w_[4];
  f32 steer_ = 0, steer_eq_f_ = 0, vf_ = 0, throttle_ = 0;
  i32 gear_ = 1;
};

/// SpawnCity's AI driver (world_vehicle_dynamics.mjs aiInput): steer to the target, slow for bends.
CarInput ai_drive(Vec3 position, Vec3 forward, f32 vf, Vec3 target, f32 target_speed);

}  // namespace sc
