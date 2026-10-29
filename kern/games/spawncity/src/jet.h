// The VTOL fighter: SpawnCity's 6-DoF flight model (sim/jet_flight_dynamics.mjs, version
// "jet-6dof-arcade-fbw-responsive-v2") ported to C++ on a Box3D body.
//
// Box3D owns the body (mass, inertia, gravity, integration, collisions). Every fixed step the
// model turns the body's state into the forces and moments that act on an aircraft:
// aerodynamics from coefficients (lift with stall and flat-plate post-stall, induced, parasitic
// and transonic wave drag, side force; static stability and rate damping; elevator, ailerons,
// rudder with deflection and rate limits), one vectored engine (nozzles aft..down, spool lag,
// density lapse, reheat), reaction control puffers while jet-borne, and three landing-gear struts
// as ray-cast spring-dampers with tyre friction, brakes and nose-wheel steering. A fly-by-wire
// computer turns the stick into surface / puffer / throttle / nozzle commands.
//
// Body frame: +X right wing, +Y nose, +Z up.
#pragma once

#include "common.h"
#include "rigid_spec.h"

namespace sc {

struct JetSpec {
  f32 mass = 12000;
  Vec3 inertia{1.0e5f, 1.8e4f, 1.15e5f};  // about X (pitch), Y (roll), Z (yaw)
  f32 wing_area = 26, span = 9.2f, chord = 3.3f;
  f32 thrust_dry = 105000, thrust_max = 185000, spool_up = 0.34f, spool_down = 0.3f;
  f32 CL0 = 0.1f, CLa = 4.0f, alpha_stall = 0.42f, CD0 = 0.022f, K = 0.14f, CYb = -0.8f, gear_CD = 0.02f, brake_CD = 0.15f;
  f32 Cm0 = 0, Cma = -0.4f, Cmq = -12, Cmde = -0.9f;
  f32 Clb = -0.06f, Clp = -0.35f, Clr = 0.08f, Clda = 0.14f;
  f32 Cnb = 0.12f, Cnr = -0.25f, Cnp = -0.03f, Cndr = 0.07f;
  f32 de_max = 0.48f, da_max = 0.48f, dr_max = 0.5f, surface_rate = 7.5f;
  Vec3 rcs_max{70000, 45000, 150000};
  f32 nozzle_rate = 1.0f;
  struct Gear { Vec3 p; f32 load; bool steer; };
  Gear gear[3] = {{{0, 5.2f, -0.6f}, 0.212f, true}, {{1.9f, -1.4f, -0.6f}, 0.394f, false}, {{-1.9f, -1.4f, -0.6f}, 0.394f, false}};
  f32 strut_rest = 1.6f, gear_height = 2.05f, strut_static = 0.15f, strut_max = 0.4f, strut_zeta = 0.6f, tyre_mu = 0.8f, roll_mu = 0.02f, brake_mu = 0.5f;
  f32 g_limit = 9, g_push = 3, alpha_limit = 0.38f, p_max = 3.4f, q_max = 1.7f;
  f32 hover_speed = 14, hover_climb = 10, hover_sink = 6, hover_tilt = 0.35f;
  f32 wing_borne_v = 85, hover_law_v = 50, transition_nozzle = 1.05f, braking_nozzle = 1.71f;
};

struct JetInput {
  f32 lift = 0;     // left stick vertical: jet-borne vertical speed / wing-borne throttle lever
  f32 yaw = 0;      // rudder / nose wheel
  f32 pitch = 0;    // stick pitch (+ = nose up)
  f32 roll = 0;     // stick roll (+ = right)
  f32 brake = 0;    // wheel brakes 0..1
};

struct JetTelemetry {
  f32 V = 0, agl = 0, mach = 0, alpha = 0, beta = 0, nz = 1, thrust = 0, nozzle = 0;
  i32 contacts = 0;
  f32 pitch = 0, roll = 0, heading = 0, w_flight = 0;
};

class Jet {
 public:
  enum class Mode : u8 { hover, flight };
  explicit Jet(const JetSpec& s = {}) : s_(s) { reset(); }
  static RigidSpec rigid_spec(const JetSpec& s = {});

  void reset();
  /// Computes and applies the forces for the coming Box3D step (call once per fixed step).
  void step(kern::physics::World& world, kern::physics::Body body, f32 h, const JetInput& in);
  void set_mode(Mode m);
  Mode mode() const { return mode_; }
  void toggle_mode() { set_mode(mode_ == Mode::hover ? Mode::flight : Mode::hover); }

  bool engine_on = true;
  bool pilot = true;
  f32 thrust_scale = 1;           // damage reduces it (jet_mode.mjs)
  f32 fuel = 1;                   // 0..1 (HUD); the SpawnCity jet does not burn fuel
  const JetTelemetry& telemetry() const { return tele_; }
  bool landed() const { return landed_; }
  bool gear_down() const { return gear_down_; }
  f32 gear_impact() const { return gear_impact_; }
  f32 nozzle() const { return nozzle_; }
  f32 thrust() const { return thrust_; }
  f32 lever() const { return lever_; }
  const JetSpec& spec() const { return s_; }

 private:
  struct Aero { f32 qbar, alpha, beta, CL, CD, CY; Vec3 moment; };
  Aero aero(Vec3 vb, Vec3 wb, f32 V, f32 rho, f32 de, f32 da, f32 dr) const;

  JetSpec s_;
  Mode mode_ = Mode::hover;
  f32 lever_ = 0, thrust_ = 0, thrust_cmd_ = 0, nozzle_ = kPi / 2, de_ = 0, da_ = 0, dr_ = 0, steer_ = 0, vz_i_ = 0, vz_cmd_ = 0;
  bool vz_cmd_valid_ = false, gamma_valid_ = false;
  f32 gamma_hold_ = 0;
  bool gear_down_ = true, speed_brake_ = false, landed_ = true;
  i32 contacts_ = 0;
  f32 gear_impact_ = 0;
  JetTelemetry tele_;
};

f32 air_density(f32 h);

}  // namespace sc
