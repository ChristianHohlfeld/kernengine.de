// Cars and buses: SpawnCity's vehicle dynamics (sim/world_vehicle_dynamics.mjs v4.1) - real wheeled
// vehicles on Box3D joints, after Box3D's own driving setup (its manual: "The wheel joint is
// designed specifically for vehicles", a sphere per wheel).
//
// A dynamic chassis and four dynamic wheels (spheres) on Box3D wheel joints: the joint guides the
// wheel along the suspension axis, its spring carries the car, its limits are the bump stops, its
// steering servo turns the front wheels; a damper strut (a soft distance joint from a chassis
// mount over the wheel to the hub) carries the damping. Springs, dampers and stops are Box3D's
// soft constraints - solved implicitly inside the solver, stable for any stiffness and any step -
// and the wheels meet the ground through Box3D's contacts. (An earlier version pushed the chassis
// with spring forces computed outside the solver from one ray per wheel; held over several steps
// and 20 times stiffer on the bump stop, such a spring feeds energy in instead of taking it out -
// a heavy bus that touched its stops was thrown into the air.)
//
// Everything above the suspension is SpawnCity's model unchanged: per-wheel spin degree of freedom
// (I*dw/dt = T_engine - T_brake - Fx*r, integrated implicitly in 5 substeps; the wheel bodies
// themselves do not spin and do not rub on the ground), the combined-slip tyre curve (kappa_peak
// 0.1, alpha_peak 6.5 deg, sliding grip 0.78, load sensitivity 0.15), traction control and ABS at
// 11 % slip, power-limited low gear, automatic D/R that only changes below 0.65 m/s, electric
// power steering solved as the equilibrium of hand torque + assist against the tyres' aligning
// torque, rack rate limit, aero drag and rolling resistance. The tyre forces act on the wheels at
// the contact patches and reach the chassis through the joints, so load transfer, pitch, roll and
// yaw come out of Box3D.
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
  f32 steer_servo = 3000;  // the steering servo's torque limit (world_vehicle_dynamics.mjs steerTorque)
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
  ~Car() { detach(); }
  Car(const Car&) = delete;
  Car& operator=(const Car&) = delete;
  /// Takes the wheels (bodies, joints) off: the chassis is no longer driven (parked far away
  /// without physics, moved by somebody else, removed). step() puts them on again.
  void detach();
  bool rigged() const { return rig_world_ != nullptr; }
  /// A car somebody else drives (shown only): its steering angle, and its wheels rolling at `v`.
  void show(f32 steer) { steer_ = steer; }
  void roll(f32 v, f32 dt) { for (WheelState& w : w_) { w.omega = v / s_.radius; w.angle = std::fmod(w.angle + w.omega * dt, 2 * kPi); } }
  /// What a hit on a wheel reports as the shape's user (the game: the car's actor, so a round
  /// into a tyre is a hit on the car).
  u64 hit_user = 0;
  /// One wheel body of the rig (valid while rigged): for whoever must know it (a ray's ignore).
  kern::physics::Body wheel_body(int i) const { return rig_[i].body; }
  static RigidSpec rigid_spec(const CarSpec& s);
  /// Height of the chassis centre above flat ground at rest.
  static f32 ground_offset(const CarSpec& s) { return s.radius - s.attach_z - s.sag; }

  void reset();
  void step(kern::physics::World& world, kern::physics::Body body, f32 h, const CarInput& in);
  /// The forces of the last step() again, in the chassis frame (no rays): AI traffic runs its
  /// tyres at 60 Hz as the original (world_rigid_body_runtime.mjs FIXED_DT 1/60) and holds them
  /// over Kern's 240 Hz steps in between (game.cpp drive_traffic).
  void reapply(kern::physics::World& world, kern::physics::Body body) const;
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
  struct Applied { Vec3 p, f; i32 on; };  // chassis-local point and force; on: wheel 0..3, or -1 the chassis' centre
  Applied applied_[6];
  i32 n_applied_ = 0;
  void push(kern::physics::World& world, kern::physics::Body body, const Quat& q, Vec3 origin, Vec3 f, Vec3 p, i32 on);
  // the wheels on their joints
  struct Rig { kern::physics::Body body; kern::physics::Joint joint, strut; };
  Rig rig_[4];
  kern::physics::World* rig_world_ = nullptr;
  kern::physics::Body rig_chassis_;
  Vec3 last_pos_;
  bool has_last_ = false;
  void attach(kern::physics::World& world, kern::physics::Body chassis);
  void place(kern::physics::World& world, const BodyState& st);
  bool ensure_rig(kern::physics::World& world, kern::physics::Body chassis, f32 dt);
};

/// SpawnCity's AI driver (world_vehicle_dynamics.mjs aiInput): steer to the target, slow for bends.
CarInput ai_drive(Vec3 position, Vec3 forward, f32 vf, Vec3 target, f32 target_speed);

}  // namespace sc
