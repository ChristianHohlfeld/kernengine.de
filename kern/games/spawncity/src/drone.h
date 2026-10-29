// The FPV drone: SpawnCity's quad plant (sim/simulator.mjs PhysicsModel) flown by the real
// Arondight45 flight controller firmware (firmware/, unchanged from spawncity-de/esp32).
//
// Box3D owns the airframe (mass, inertia, gravity, collisions). Every fixed physics step this
// model runs the firmware at about 1 kHz (n ticks of h/n, 600..1600 us as the FC requires):
// raw ICM-42688 register bytes from the body's specific force and rates (with the sensor's noise
// and bias walk), SBUS frames at 100 Hz from the pilot's sticks, NAV1 frames at 100 Hz (velocity,
// rangefinder AGL by a Box3D ray). The FC's ESC pulses drive the motor model (back-EMF, winding
// resistance, pack sag and current limits, rotor inertia, prop torque); the four thrusts act at
// the arm tips, the reaction torques about the motor axes, plus body drag and rotational damping.
// Box3D clears forces after every step, so they are applied anew in each fixed step.
//
// Body frame (SpawnCity's): -X is the nose, +Z up; motors at (-a,-a) (-a,a) (a,a) (a,-a).
#pragma once

#include <array>

#include "common.h"
#include "rigid_spec.h"

namespace sc {

/// drone_simulator.html:409-410 defaults (5" 4S racer, ~0.72 kg).
struct DroneParams {
  f32 span = 0.225f;           // motor-to-motor diagonal (m)
  f32 prop_d = 5.0f * 0.0254f;
  f32 kv = 2300, R = 0.080f, J = 0.000011f;
  f32 Ct = 0.105f, Cq = 0.012f;
  f32 motor_limit_a = 45, esc_limit_a = 45;
  f32 cells = 4, capacity_ah = 1.5f, battery_r = 0.055f, battery_max_a = 180;
  f32 rho = 1.225f, drag_scale = 0.35f, ground_effect = 0.18f;
  Vec3 wind;
  // component masses (kg) and placements (m), component_mass_model.mjs
  f32 m_frame = 0.180f, m_motor = 0.035f, m_prop = 0.005f, m_battery = 0.200f, m_esc = 0.030f, m_fc_rx = 0.020f, m_cam_vtx = 0.030f,
      m_wiring = 0.100f;
  f32 battery_x = 0.015f, battery_z = -0.015f, camera_x = -0.070f, camera_z = 0.028f;
};

/// What the pilot commands (SpawnCity control_semantics / simulator.mjs controls()).
struct DroneSticks {
  f32 roll = 0, pitch = 0, yaw = 0;   // -1..1 (GAME: right / forward velocity, yaw rate)
  f32 throttle = 0;                   // 0..1 (ANGLE mode)
  f32 body_pitch = 0;                 // -1..1 (GAME: body pitch override)
  f32 clearance = 1.2f;               // GAME: height above ground (m), 0.5..120
  bool arm = false;
  bool game_mode = true;              // GAME (state controller, height hold) or ANGLE (manual throttle)
  bool kill = false;                  // motors off now
};

struct DroneTelemetry {
  bool armed = false, calibrating = true, fault = false, game = false, nav_valid = false;
  u32 state = 0;
  std::array<u16, 4> pulses{};
  std::array<f32, 4> omega{};         // rad/s per motor
  f32 battery_v = 0, battery_soc = 1, current_a = 0;
  f32 agl = 0;                        // rangefinder AGL (m), < 0 = no return
  f32 speed = 0, vz = 0;
  f32 attitude_deg[3] = {};           // FC estimate: roll, pitch, yaw (deg)
};

class DroneFirmware;  // the vendored FC stack (drone.cpp)

class Drone {
 public:
  explicit Drone(const DroneParams& p = {});
  ~Drone();
  Drone(const Drone&) = delete;
  Drone& operator=(const Drone&) = delete;

  /// Colliders and mass properties of the airframe (airframe_collision_envelope.mjs, component masses).
  static RigidSpec rigid_spec(const DroneParams& p = {});

  /// Fresh firmware (calibrates again), stopped motors, full pack.
  void reset();
  /// One fixed physics step of `h` seconds: runs the firmware ticks and applies the forces for the
  /// coming Box3D step. Call it once per fixed step (after the previous step, before the next).
  void step(kern::physics::World& world, kern::physics::Body body, f32 h, const DroneSticks& sticks);
  const DroneTelemetry& telemetry() const { return tele_; }
  const DroneParams& params() const { return p_; }
  Vec3 motor_position(int i) const { return motor_pos_[i]; }
  /// Rotor angle (radians) for drawing the props.
  f32 rotor_angle(int i) const { return rotor_angle_[i]; }
  /// The motors are stopped and the body is at rest (parked: nothing to simulate).
  bool parked() const { return parked_; }

 private:
  void motor_tick(f32 dt, const BodyState& s, f32 altitude, f32 thrust_out[4], f32 torque_out[4]);
  void imu_bytes(f32 dt, const BodyState& s, u8 out[14]);

  DroneParams p_;
  DroneFirmware* fw_ = nullptr;
  Vec3 motor_pos_[4];
  f32 omega_[4] = {}, current_[4] = {}, rotor_angle_[4] = {};
  f32 soc_ = 1, vbatt_ = 16.8f, ibatt_ = 0;
  f32 back_emf_ = 0, d4_ = 0, d5_ = 0;
  Vec3 cdA_;
  u16 pulses_[4] = {1000, 1000, 1000, 1000};
  Vec3 prev_velocity_, world_accel_, prev_omega_body_;
  bool have_prev_ = false;
  f64 now_us_ = 0;
  u32 sequence_ = 1;
  f32 sbus_elapsed_ = 0.01f, nav_elapsed_ = 0.01f;
  Vec3 nav_filtered_;
  u16 nav_seq_ = 1;
  bool reset_flag_ = true;
  Rng noise_{0x45a31f27u}, nav_noise_{0x7193ab21u}, heading_noise_{0x45a1d1a5u};
  f32 gyro_bias_[3] = {0.08f, -0.05f, 0.04f}, acc_bias_[3] = {0.001f, -0.0015f, 0.002f};
  u32 parked_steps_ = 0;
  bool parked_ = false;
  DroneTelemetry tele_;
};

/// SpawnCity's GAME height stick (control_semantics.mjs): vertical-speed command -> AGL lead.
f32 clearance_rate(f32 axis);
f32 clearance_lead(f32 vz);

}  // namespace sc
