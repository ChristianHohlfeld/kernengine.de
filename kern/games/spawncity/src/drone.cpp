// SpawnCity drone plant + the Arondight45 firmware (see drone.h).
#include "drone.h"

#include <cstring>

#include "../firmware/Arondight45_FirmwareRuntime.hpp"

namespace sc {

namespace {

constexpr f32 kBearingDrag = 1.5e-7f;          // propulsion_authority.mjs MOTOR_BEARING_DRAG_NM_PER_RAD_S
constexpr f32 kMotorYawSign[4] = {-1, 1, -1, 1};  // simulator.mjs MOTOR_YAW_SIGN
constexpr f32 kNavRayMax = 215.0f;               // ceil(120 / cos^2(40 deg) + 10), control_semantics.mjs
constexpr f32 kMinClearance = 0.5f, kMaxClearance = 120.0f;

f32 battery_ocv(f32 soc, f32 cells) {
  const f32 s = clampf(soc, 0, 1);
  return cells * std::fmin(4.2f, 3.2625f + 0.9f * s + 0.0375f * std::tanh((s - 0.12f) * 18));
}
f32 battery_under_load(f32 ocv, f32 amps, f32 r, f32 cells) {
  return clampf(ocv - std::fmax(0.0f, amps) * r, 3 * cells, 4.2f * cells);
}

void encode_sbus(const u16 ch[16], u8* p) {
  std::memset(p, 0, 25);
  p[0] = 0x0F;
  for (int c = 0; c < 16; ++c)
    for (int bit = 0; bit < 11; ++bit)
      if (ch[c] & (1u << bit)) {
        const int k = 8 + c * 11 + bit;
        p[k / 8] |= static_cast<u8>(1u << (k % 8));
      }
}

void put_be16(u8* p, f32 v) {
  const long r = std::lround(clampf(v, -32767, 32767));
  const u16 u = static_cast<u16>(static_cast<int16_t>(r));
  p[0] = static_cast<u8>(u >> 8);
  p[1] = static_cast<u8>(u & 0xff);
}

f32 quat_yaw_deg(Quat q) {
  return std::atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z)) / kDeg;
}

}  // namespace

class DroneFirmware {
 public:
  fc::FirmwareRuntime runtime;
};

f32 clearance_rate(f32 axis) {
  const f32 x = clampf(axis, -1, 1), m = std::fabs(x);
  if (m <= 0.08f) return 0;
  const f32 unit = (m - 0.08f) / (1 - 0.08f);
  return sign0(x) * 12.0f * unit * (0.25f + 0.75f * unit);
}

f32 clearance_lead(f32 v) {
  const bool up = v >= 0;
  const f32 a = up ? 6.0f : 4.0f, tau = up ? 0.25f : 0.125f, k = 2.0f, s = std::fabs(v);
  const f32 e = std::fmax(s / k, (s * s + 2 * a * tau * s) / (2 * a));
  return up ? e : -e;
}

Drone::Drone(const DroneParams& p) : p_(p), fw_(new DroneFirmware()) {
  const f32 a = p_.span / (2 * std::sqrt(2.0f));
  motor_pos_[0] = {-a, -a, 0};
  motor_pos_[1] = {-a, a, 0};
  motor_pos_[2] = {a, a, 0};
  motor_pos_[3] = {a, -a, 0};
  back_emf_ = 60.0f / (2 * kPi * p_.kv);
  d4_ = std::pow(p_.prop_d, 4.0f);
  d5_ = std::pow(p_.prop_d, 5.0f);
  cdA_ = Vec3{0.035f, 0.035f, 0.07f} * p_.drag_scale;
  reset();
}

Drone::~Drone() { delete fw_; }

RigidSpec Drone::rigid_spec(const DroneParams& p) {
  RigidSpec r;
  const f32 a = p.span / (2 * std::sqrt(2.0f));
  const Vec3 motors[4] = {{-a, -a, 0}, {-a, a, 0}, {a, a, 0}, {a, -a, 0}};
  ColliderSpec box;
  box.kind = kern::physics::ShapeKind::box;
  box.size = {0.11f, 0.09f, 0.044f};
  box.friction = 0.65f;
  box.restitution = 0.08f;
  r.colliders.push_back(box);
  for (const Vec3& m : motors) {
    ColliderSpec arm = capsule_between({0, 0, 0}, m, 0.008f);
    arm.friction = 0.65f;
    arm.restitution = 0.08f;
    r.colliders.push_back(arm);
    ColliderSpec bell;
    bell.kind = kern::physics::ShapeKind::sphere;
    bell.center = m;
    bell.radius = 0.018f;
    bell.friction = 0.65f;
    bell.restitution = 0.08f;
    r.colliders.push_back(bell);
    // The propeller's swept disc (airframe_collision_envelope.mjs): a flat box over the rotor.
    ColliderSpec sweep;
    sweep.kind = kern::physics::ShapeKind::box;
    sweep.center = m + Vec3{0, 0, 0.014f};
    sweep.size = {p.prop_d * 0.9f, p.prop_d * 0.9f, 0.008f};
    sweep.friction = 0.65f;
    sweep.restitution = 0.08f;
    r.colliders.push_back(sweep);
  }
  // Mass properties from the components (component_mass_model.mjs buildQuadComponents).
  struct Part { f32 m; Vec3 pos, size; bool disk; };
  std::vector<Part> parts;
  parts.push_back({p.m_frame, {0, 0, 0}, {p.span * 0.62f, p.span * 0.48f, 0.018f}, false});
  for (const Vec3& m : motors) {
    parts.push_back({p.m_motor, m, {0.036f, 0.036f, 0.025f}, false});
    parts.push_back({p.m_prop, {m.x, m.y, 0.014f}, {p.prop_d * 0.5f, 0.002f, 0}, true});
    parts.push_back({p.m_wiring / 4, {m.x / 2, m.y / 2, -0.002f}, {p.span * 0.245f, 0.016f, 0.008f}, false});
  }
  parts.push_back({p.m_battery, {p.battery_x, 0, p.battery_z}, {0.105f, 0.035f, 0.032f}, false});
  parts.push_back({p.m_esc, {0, 0, -0.002f}, {0.05f, 0.05f, 0.008f}, false});
  parts.push_back({p.m_fc_rx, {0, 0, 0.008f}, {0.04f, 0.04f, 0.012f}, false});
  parts.push_back({p.m_cam_vtx, {p.camera_x, 0, p.camera_z}, {0.04f, 0.03f, 0.025f}, false});
  f32 mass = 0;
  Vec3 c;
  for (const Part& q : parts) { mass += q.m; c += q.pos * q.m; }
  c = c * (1.0f / mass);
  f32 I[6] = {};
  for (const Part& q : parts) {
    f32 l[3];
    if (q.disk) {
      const f32 rr = q.size.x, t = q.size.y;
      l[0] = l[1] = q.m * (3 * rr * rr + t * t) / 12;
      l[2] = q.m * rr * rr / 2;
    } else {
      l[0] = q.m * (q.size.y * q.size.y + q.size.z * q.size.z) / 12;
      l[1] = q.m * (q.size.x * q.size.x + q.size.z * q.size.z) / 12;
      l[2] = q.m * (q.size.x * q.size.x + q.size.y * q.size.y) / 12;
    }
    const Vec3 o = q.pos - c;
    I[0] += l[0] + q.m * (o.y * o.y + o.z * o.z);
    I[1] += l[1] + q.m * (o.x * o.x + o.z * o.z);
    I[2] += l[2] + q.m * (o.x * o.x + o.y * o.y);
    I[3] += -q.m * o.x * o.y;
    I[4] += -q.m * o.x * o.z;
    I[5] += -q.m * o.y * o.z;
  }
  r.mass.mass = mass;
  r.mass.center = c;
  for (int i = 0; i < 6; ++i) r.mass.inertia[i] = I[i];
  r.linear_damping = 0.002f;
  r.angular_damping = 0.002f;
  r.bullet = true;  // continuous collision: a racer crosses its own size in a step
  return r;
}

void Drone::reset() {
  fw_->runtime.reset();
  for (int i = 0; i < 4; ++i) { omega_[i] = current_[i] = 0; pulses_[i] = 1000; }
  soc_ = 1;
  vbatt_ = battery_ocv(1, p_.cells);
  ibatt_ = 0;
  have_prev_ = false;
  world_accel_ = prev_omega_body_ = nav_filtered_ = {};
  now_us_ = 0;
  sequence_ = 1;
  sbus_elapsed_ = nav_elapsed_ = 0.01f;
  nav_seq_ = 1;
  reset_flag_ = true;
  noise_ = Rng(0x45a31f27u);
  nav_noise_ = Rng(0x7193ab21u);
  heading_noise_ = Rng(0x45a1d1a5u);
  gyro_bias_[0] = 0.08f; gyro_bias_[1] = -0.05f; gyro_bias_[2] = 0.04f;
  acc_bias_[0] = 0.001f; acc_bias_[1] = -0.0015f; acc_bias_[2] = 0.002f;
  parked_steps_ = 0;
  parked_ = false;
  tele_ = {};
}

void Drone::imu_bytes(f32 dt, const BodyState& s, u8 out[14]) {
  for (int i = 0; i < 3; ++i) {
    gyro_bias_[i] += noise_.gaussian() * 0.002f * std::sqrt(dt);
    acc_bias_[i] += noise_.gaussian() * 0.00002f * std::sqrt(dt);
  }
  const Vec3 omega_body = to_local(s.rotation, s.angular);
  const Vec3 alpha = (omega_body - prev_omega_body_) * (1.0f / dt);
  prev_omega_body_ = omega_body;
  const Vec3 specific = to_local(s.rotation, world_accel_ - Vec3{0, 0, -kG});
  const Vec3 off{0, 0, 0.008f};
  Vec3 accel = specific + kern::cross(alpha, off) + kern::cross(omega_body, kern::cross(omega_body, off));
  Vec3 gyro = omega_body * (1.0f / kDeg);
  for (int i = 0; i < 3; ++i) {
    accel[i] += acc_bias_[i] * kG + noise_.gaussian() * 0.0025f * kG;
    gyro[i] += gyro_bias_[i] + noise_.gaussian() * 0.035f;
  }
  out[0] = out[1] = 0;
  put_be16(out + 2, accel.x / kG * 2048);
  put_be16(out + 4, accel.y / kG * 2048);
  put_be16(out + 6, accel.z / kG * 2048);
  put_be16(out + 8, gyro.x * 16.4f);
  put_be16(out + 10, gyro.y * 16.4f);
  put_be16(out + 12, gyro.z * 16.4f);
}

void Drone::motor_tick(f32 dt, const BodyState& s, f32 altitude, f32 thrust_out[4], f32 torque_out[4]) {
  const f32 ocv = battery_ocv(soc_, p_.cells);
  const f32 limit = std::fmin(p_.motor_limit_a, p_.esc_limit_a);
  f32 total = 0;
  for (int pass = 0; pass < 2; ++pass) {
    total = 0;
    for (int i = 0; i < 4; ++i) {
      const f32 command = clampf((pulses_[i] - 1000.0f) / 1000.0f, 0, 1);
      const f32 volts = command * (pass ? vbatt_ : ocv);
      current_[i] = clampf((volts - back_emf_ * omega_[i]) / p_.R, 0, limit);
      total += current_[i];
    }
    if (total > p_.battery_max_a && total > 1e-12f) {
      const f32 k = p_.battery_max_a / total;
      for (f32& c : current_) c *= k;
      total = p_.battery_max_a;
    }
    vbatt_ = battery_under_load(ocv, total, p_.battery_r, p_.cells);
  }
  ibatt_ = total;
  soc_ = clampf(soc_ - total * dt / (p_.capacity_ah * 3600), 0, 1);
  const Vec3 local_v = to_local(s.rotation, s.velocity);
  for (int i = 0; i < 4; ++i) {
    const f32 rev = omega_[i] / (2 * kPi);
    const f32 prop_torque = p_.Cq * p_.rho * rev * rev * d5_;
    const f32 motor_torque = back_emf_ * current_[i];  // Kt = Ke in SI units
    omega_[i] = std::fmax(0.0f, omega_[i] + (motor_torque - prop_torque - kBearingDrag * omega_[i]) * dt / p_.J);
    const f32 n = omega_[i] / (2 * kPi);
    f32 thrust = p_.Ct * p_.rho * n * n * d4_;
    const f32 advance = local_v.z / std::fmax(1.0f, n * p_.prop_d);
    thrust *= clampf(1 - 0.12f * advance, 0.55f, 1.25f);
    thrust *= 1 + p_.ground_effect * std::exp(-altitude / std::fmax(0.02f, 0.75f * p_.prop_d));
    thrust_out[i] += thrust;
    torque_out[i] += kMotorYawSign[i] * motor_torque;
    rotor_angle_[i] = std::fmod(rotor_angle_[i] + (i % 2 ? -1.0f : 1.0f) * omega_[i] * dt, 2 * kPi);
  }
}

void Drone::step(kern::physics::World& world, kern::physics::Body body, f32 h, const DroneSticks& sticks) {
  const BodyState s = read_body(world, body);
  if (have_prev_ && h > 0) world_accel_ = (s.velocity - prev_velocity_) * (1.0f / h);
  prev_velocity_ = s.velocity;
  have_prev_ = true;

  // Rangefinder: a Box3D ray along the body's -Z from just below the frame (groundRange).
  const Vec3 down = s.rotation.rotate({0, 0, -1});
  const f32 vertical = -down.z;
  RayResult range;
  if (vertical > 0.55f) range = cast(world, s.position + s.rotation.rotate({0, 0, -0.018f}), down, kNavRayMax, body);
  const bool agl_valid = range.hit && range.distance >= 0.001f;
  const f32 true_agl = agl_valid ? range.distance * vertical : 999.0f;
  // Truth altitude above whatever is straight below (ground effect).
  const RayResult below = cast(world, s.position, {0, 0, -1}, 50.0f, body);
  const f32 altitude = std::fmax(0.001f, below.hit ? below.distance : 50.0f);

  // SBUS channels (simulator.mjs controls()).
  u16 ch[16];
  for (u16& c : ch) c = 992;
  auto centered = [](f32 v) { return static_cast<u16>(std::lround(992 + 820 * clampf(v, -1, 1))); };
  ch[0] = centered(sticks.roll);
  ch[1] = centered(sticks.pitch);
  ch[3] = centered(sticks.yaw);
  ch[4] = sticks.arm && !sticks.kill ? 1811 : 172;
  if (sticks.game_mode) {
    ch[2] = 172;
    const f32 clearance = clampf(sticks.clearance, kMinClearance, kMaxClearance);
    ch[5] = static_cast<u16>(std::lround(172 + 1639 * (clearance - kMinClearance) / (kMaxClearance - kMinClearance)));
    ch[6] = 1811;
    ch[7] = centered(sticks.body_pitch);
  } else {
    ch[2] = static_cast<u16>(std::lround(172 + 1639 * clampf(sticks.throttle, 0, 1)));
  }

  const int ticks = std::max(1, static_cast<int>(std::lround(h * 1000.0f)));
  const f32 dt = h / static_cast<f32>(ticks);
  f32 thrust[4] = {}, yaw_torque[4] = {};
  fc::RuntimeOutput out{};
  for (int t = 0; t < ticks; ++t) {
    fc::HardwareFrame hw{};
    now_us_ += static_cast<f64>(dt) * 1e6;
    hw.now_us = static_cast<uint64_t>(now_us_);
    hw.dt_us = static_cast<uint32_t>(std::lround(dt * 1e6f));
    u8 imu[14];
    imu_bytes(dt, s, imu);
    std::memcpy(hw.imu_registers.data(), imu, 14);
    hw.imu_present = true;
    sbus_elapsed_ += dt;
    if (sbus_elapsed_ >= 0.01f) {
      sbus_elapsed_ -= 0.01f;
      encode_sbus(ch, hw.sbus_frame.data());
      hw.sbus_present = true;
    }
    nav_elapsed_ += dt;
    if (nav_elapsed_ >= 0.01f) {
      nav_elapsed_ -= 0.01f;
      for (int i = 0; i < 3; ++i) {
        const f32 measured = s.velocity[i] + nav_noise_.gaussian() * 0.025f;
        nav_filtered_[i] += 0.42f * (measured - nav_filtered_[i]);
      }
      f32 agl = 0;
      if (agl_valid) agl = std::fmax(0.0f, range.distance + nav_noise_.gaussian() * 0.004f) * vertical;
      const f32 heading = quat_yaw_deg(s.rotation) + heading_noise_.gaussian() * 0.12f;
      hw.navigation_frame = hwcontract::encode_navigation_wire(nav_seq_++, nav_filtered_.x, nav_filtered_.y, nav_filtered_.z, agl, true, agl_valid,
                                                               heading, true);
      hw.navigation_present = true;
    }
    if (reset_flag_) { fw_->runtime.reset(); reset_flag_ = false; }
    out = fw_->runtime.step(hw);
    for (int i = 0; i < 4; ++i) pulses_[i] = sticks.kill ? 1000 : out.motor_us[i];
    motor_tick(dt, s, altitude, thrust, yaw_torque);
  }

  // A latched firmware fault on a disarmed airframe at rest (calibration while it was still
  // settling, a crash): the pilot power-cycles it, as with the real quad - the FC boots and
  // calibrates again.
  if (out.fault != fc::kFaultNone && !out.armed && kern::length(s.velocity) < 0.05f && kern::length(s.angular) < 0.05f) reset_flag_ = true;

  // Parked (motors stopped, at rest for 0.5 s): no forces to apply.
  bool commanded = false;
  for (int i = 0; i < 4; ++i) commanded |= pulses_[i] > 1000 || omega_[i] > 1e-3f;
  if (commanded) parked_steps_ = 0, parked_ = false;
  else if (kern::length(s.velocity) < 2e-3f && kern::length(s.angular) < 2e-3f) parked_ = ++parked_steps_ > static_cast<u32>(0.5f / h);
  else parked_steps_ = 0, parked_ = false;

  const f32 inv = 1.0f / static_cast<f32>(ticks);
  if (commanded) {
    for (int i = 0; i < 4; ++i) {
      world.apply_force(body, s.rotation.rotate({0, 0, thrust[i] * inv}), s.position + s.rotation.rotate(motor_pos_[i]));
      world.apply_torque(body, s.rotation.rotate({0, 0, yaw_torque[i] * inv}));
    }
  }
  const Vec3 rel = to_local(s.rotation, s.velocity - p_.wind);
  const f32 hs = std::hypot(rel.x, rel.y);
  const Vec3 drag{-0.5f * p_.rho * cdA_.x * rel.x * hs, -0.5f * p_.rho * cdA_.y * rel.y * hs, -0.5f * p_.rho * cdA_.z * rel.z * std::fabs(rel.z)};
  world.apply_force_to_center(body, s.rotation.rotate(drag));
  const Vec3 w = to_local(s.rotation, s.angular);
  world.apply_torque(body, s.rotation.rotate({-0.0012f * w.x * std::fabs(w.x), -0.0012f * w.y * std::fabs(w.y), -0.0012f * w.z * std::fabs(w.z)}));

  tele_.state = out.state;
  tele_.armed = out.armed;
  tele_.calibrating = (out.state & fc::kStateCalibrating) != 0;
  tele_.fault = (out.state & fc::kStateFault) != 0;
  tele_.game = (out.state & fc::kStateGameMode) != 0;
  tele_.nav_valid = (out.state & fc::kStateNavigationValid) != 0;
  for (int i = 0; i < 4; ++i) { tele_.pulses[i] = pulses_[i]; tele_.omega[i] = omega_[i]; }
  tele_.battery_v = vbatt_;
  tele_.battery_soc = soc_;
  tele_.current_a = ibatt_;
  tele_.agl = agl_valid ? true_agl : -1;
  tele_.speed = kern::length(s.velocity);
  tele_.vz = s.velocity.z;
  for (int i = 0; i < 3; ++i) tele_.attitude_deg[i] = out.attitude_cdeg[i] * 0.01f;
}

}  // namespace sc
