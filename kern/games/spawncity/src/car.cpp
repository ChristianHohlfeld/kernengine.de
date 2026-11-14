// SpawnCity's vehicle dynamics on a ray-cast chassis (see car.h).
#include "car.h"

namespace sc {

namespace {
constexpr f32 kKappaPeak = 0.1f, kSlideGrip = 0.78f, kLoadSensitivity = 0.15f, kVReg = 2, kTcsSlip = 0.11f, kAbsSlip = 0.11f;
constexpr int kTyreSubsteps = 5;
constexpr f32 kPneumaticTrail = 0.03f, kCasterTrail = 0.025f, kSteerJacking = 0.18f, kTrailCollapse = 3;
constexpr f32 kBodyDamping = 0.45f;
const f32 kAlphaPeak = std::tan(6.5f * kPi / 180);
constexpr f32 kAsphaltMu = 1.0f;  // terrain_craters.mjs groundSurfaceAt: asphalt / default ground

/// Combined-slip tyre force (longitudinal, lateral) on the friction ellipse.
void tyre(f32 kappa, f32 tan_a, f32 muN, f32 (&out)[2]) {
  const f32 sx = kappa / kKappaPeak, sy = tan_a / kAlphaPeak, s = std::hypot(sx, sy);
  if (s < 1e-6f || !(muN > 0)) {
    out[0] = 0;
    out[1] = 0;
    return;
  }
  const f32 g = s < 1 ? s * (2 - s) : 1 - (1 - kSlideGrip) * std::fmin(1.0f, (s - 1) / 2.5f);
  const f32 F = muN * g;
  out[0] = F * sx / s;
  out[1] = -F * sy / s;
}
f32 steer_torque_at(const CarSpec& s, f32 v) { const f32 x = v / s.steer_assist_speed; return s.steer_hand + s.steer_assist / (1 + x * x); }
}  // namespace

CarSpec CarSpec::bus() {
  CarSpec s;
  s.kind = "bus";
  s.half = {4, 1.17f, 1.2f};
  s.mass = 9200; s.com_z = -0.52f; s.com_x = 0;
  s.radius = 0.46f; s.wheel_width = 0.3f; s.wheel_mass = 85; s.attach_z = -1.1f;
  s.wheels[0] = {2.6f, 1.08f, true}; s.wheels[1] = {2.6f, -1.08f, true}; s.wheels[2] = {-2.55f, 1.08f, false}; s.wheels[3] = {-2.55f, -1.08f, false};
  s.susp_lower = -0.18f; s.susp_upper = 0.14f; s.sag = 0.08f;
  s.torque_front = 0; s.torque_rear = 4200; s.power = 230000;
  s.steer_lag = 0.06f; s.steer_damp_speed = 20; s.drag_area = 6.2f; s.rolling_resistance = 0.008f; s.wheel_inertia = 14;
  s.brake = 9500; s.handbrake = 12000; s.coast = 200; s.steer_lock = 0.44f; s.steer_hand = 2600; s.steer_assist = 5200; s.steer_assist_speed = 6;
  s.friction = 1.0f; s.rack_rate = 1.45f; s.steer_servo = 6000;
  s.upper_mass = 450;
  s.upper_profile = {{-4, 2.6f}, {-4, 2.9f}, {-3.85f, 3.05f}, {3.8f, 3.05f}, {4, 2.85f}, {4, 2.6f}};
  s.upper_half_width = 1.21f; s.upper_dz = 1.48f;
  return s;
}

RigidSpec Car::rigid_spec(const CarSpec& s) {
  RigidSpec r;
  ColliderSpec box;
  box.kind = kern::physics::ShapeKind::box;
  box.size = s.half * 2.0f;
  box.friction = 0.6f;
  r.colliders.push_back(box);
  ColliderSpec upper;
  upper.kind = kern::physics::ShapeKind::hull;
  for (f32 y : {-s.upper_half_width, s.upper_half_width})
    for (const Vec2& p : s.upper_profile) upper.points.push_back({p.x, y, p.y - s.upper_dz});
  upper.friction = 0.5f;
  r.colliders.push_back(upper);
  // The chassis alone (world_vehicle_dynamics.mjs: spec.mass with its upper body); the wheels are
  // bodies of their own on the wheel joints.
  const f32 total = s.mass;
  r.mass.mass = total;
  r.mass.center = {s.com_x, 0, s.com_z};
  box_inertia(total, s.half * 2.0f, r.mass.inertia);
  r.linear_damping = 0;
  r.angular_damping = 0.05f;
  return r;
}

void Car::reset() {
  for (WheelState& w : w_) w = WheelState{};
  steer_ = steer_eq_f_ = vf_ = throttle_ = 0;
  gear_ = 1;
  n_applied_ = 0;
  has_last_ = false;  // wherever the chassis is now: the wheels are put under it again
}

f32 Car::wheel_speed() const {
  f32 m = 0;
  for (const WheelState& w : w_) m = std::fmax(m, std::fabs(w.omega));
  return m * s_.radius;
}

// ---- the wheels on their joints (world_vehicle_dynamics.mjs attachWheels) ------------------------
namespace {
// Joint frame: local x -> up (suspension + steering axis), local z -> left (wheel spin axis),
// local y -> forward. The same frame on chassis and wheel.
const Quat kWheelFrame{-0.5f, -0.5f, -0.5f, 0.5f};
constexpr f32 kSpringZeta = 0.06f, kStrutShare = 0.15f, kStrutM = 0.55f, kRubberFriction = 0.9f;
struct Tuning { f32 hertz, damping_ratio; };
// Box3D's joint spring is a soft constraint: its hertz and damping ratio act on the joint's
// effective mass (about the light wheel), not on the chassis. They are picked so the spring
// really carries a quarter of the chassis at the target sag with a well damped body motion
// (world_vehicle_dynamics.mjs suspensionTuning).
Tuning suspension_tuning(const CarSpec& s, f32 zeta, f32 k_share) {
  const f32 q = s.mass / 4, m = s.wheel_mass * q / (s.wheel_mass + q), k = q * 9.81f / s.sag * k_share, w = std::sqrt(k / m);
  const f32 c = 2 * zeta * std::sqrt(q * q * 9.81f / s.sag);
  return {w / (2 * kPi), c / (2 * m * w)};
}
}  // namespace

void Car::attach(kern::physics::World& world, kern::physics::Body chassis) {
  detach();
  const CarSpec& s = s_;
  const BodyState st = read_body(world, chassis);
  const f32 r = s.radius, volume = 4.0f / 3.0f * kPi * r * r * r;
  for (int i = 0; i < 4; ++i) {
    const Vec3 local{s.wheels[i].x, s.wheels[i].y, s.attach_z};
    kern::physics::BodySettings bs;
    bs.type = kern::physics::BodyType::dynamic;
    // (the hub where the spring holds it under the standing car - `sag` up its travel -, or where
    // it was: a car put down at its ride height has its tyres on the ground, not in it)
    bs.position = st.position + st.rotation.rotate(local + Vec3{0, 0, clampf(w_[i].has_travel ? w_[i].travel : s.sag, s.susp_lower, s.susp_upper)});
    bs.rotation = st.rotation;
    bs.linear_velocity = st.velocity;
    bs.angular_damping = 0.05f;
    bs.linear_damping = 0.02f;
    Rig& g = rig_[i];
    g.body = world.create_body(bs);
    // Two shapes, one sphere: the tyre on the ground has no solver friction (its grip is the tyre
    // model's: rubber x surface, slip, load); against everything else (other cars, props) the
    // same tyre rubs with rubber's friction.
    kern::physics::ShapeSettings ground;
    ground.kind = kern::physics::ShapeKind::sphere;
    ground.radius = r;
    ground.material.density = s.wheel_mass / volume;
    ground.material.friction = 0;
    ground.material.restitution = 0;
    ground.material.category_bits = kCatVehicle;
    ground.material.mask_bits = kCatWorld;
    ground.material.user = hit_user;
    world.create_shape(g.body, ground);
    kern::physics::ShapeSettings rubber = ground;
    rubber.material.density = 0;
    rubber.material.friction = kRubberFriction;
    rubber.material.restitution = 0.05f;
    rubber.material.mask_bits = ~static_cast<u64>(kCatWorld);
    world.create_shape(g.body, rubber);
    // the wheel joint: suspension axis, spring, bump stops, the spin held (the wheel's rotation is
    // the tyre model's own degree of freedom), the steering servo on the front axle
    kern::physics::JointSettings js;
    js.type = kern::physics::JointType::wheel;
    js.body_a = chassis;
    js.body_b = g.body;
    js.frame_a = {local, kWheelFrame};
    js.frame_b = {{0, 0, 0}, kWheelFrame};
    js.collide_connected = false;
    const Tuning spring = suspension_tuning(s, kSpringZeta, 1 - kStrutShare);
    js.enable_spring = true;
    js.hertz = spring.hertz;
    js.damping_ratio = spring.damping_ratio;
    js.enable_limit = true;
    js.lower = s.susp_lower;
    js.upper = s.susp_upper;
    js.enable_motor = true;
    js.motor_speed = 0;
    js.max_motor_torque = s.mass * 40;
    if (s.wheels[i].front) {
      js.enable_steering = true;
      js.steering_hertz = 9;
      js.steering_damping_ratio = 0.95f;
      js.max_steering_torque = s.steer_servo;
      js.target_steering = steer_;
      js.enable_steering_limit = true;
      js.steering_lower = -s.steer_lock;
      js.steering_upper = s.steer_lock;
    }
    g.joint = world.create_joint(js);
    // the damper strut: a chassis mount 0.55 m over the hub -> the hub, at rest at zero travel; a
    // soft spring-damper (implicit: stable though the light hub reacts faster than a step) that
    // carries the body damping and a share of the stiffness
    kern::physics::JointSettings ds;
    ds.type = kern::physics::JointType::distance;
    ds.body_a = chassis;
    ds.body_b = g.body;
    ds.frame_a = {{local.x, local.y, local.z + kStrutM}, Quat{}};
    ds.frame_b = {{0, 0, 0}, Quat{}};
    ds.collide_connected = false;
    ds.length = kStrutM;
    const Tuning strut = suspension_tuning(s, std::fmax(0.0f, kBodyDamping - kSpringZeta), kStrutShare);
    ds.enable_spring = true;
    ds.hertz = strut.hertz;
    ds.damping_ratio = strut.damping_ratio;
    ds.enable_limit = false;
    ds.lower = kStrutM - s.susp_upper - 0.25f;
    ds.upper = kStrutM - s.susp_lower + 0.25f;
    g.strut = world.create_joint(ds);
  }
  rig_world_ = &world;
  rig_chassis_ = chassis;
  last_pos_ = st.position;
  has_last_ = true;
}

void Car::detach() {
  if (!rig_world_) return;
  kern::physics::World& w = *rig_world_;
  for (Rig& g : rig_) {
    // (destroying the body takes its joints along)
    if (g.body.valid() && w.body_valid(g.body)) w.destroy_body(g.body);
    g = Rig{};
  }
  rig_world_ = nullptr;
  rig_chassis_ = {};
  has_last_ = false;
}

void Car::place(kern::physics::World& world, const BodyState& st) {
  // the wheels back under the chassis, moving with it (placeWheels: a teleport, a reset)
  for (int i = 0; i < 4; ++i) {
    const Rig& g = rig_[i];
    if (!g.body.valid() || !world.body_valid(g.body)) continue;
    const Vec3 local{s_.wheels[i].x, s_.wheels[i].y, s_.attach_z + clampf(w_[i].has_travel ? w_[i].travel : s_.sag, s_.susp_lower, s_.susp_upper)};
    const Vec3 arm = st.rotation.rotate(local);
    world.set_pose(g.body, {st.position + arm, st.rotation});
    world.set_linear_velocity(g.body, st.velocity + kern::cross(st.angular, arm));
    world.set_angular_velocity(g.body, st.angular);
    world.set_awake(g.body, true);
  }
}

bool Car::ensure_rig(kern::physics::World& world, kern::physics::Body chassis, f32 dt) {
  if (world.body_type(chassis) != kern::physics::BodyType::dynamic) { detach(); return false; }  // moved by somebody else: no wheels to drive
  bool whole = rig_world_ == &world && rig_chassis_ == chassis;
  for (int i = 0; whole && i < 4; ++i) whole = rig_[i].body.valid() && world.body_valid(rig_[i].body) && world.joint_valid(rig_[i].joint);
  const BodyState st = read_body(world, chassis);
  if (!whole) {
    attach(world, chassis);
    return true;
  }
  // The chassis was put somewhere else since the last step (a teleport, a respawn): the wheels go
  // with it before the solver sees the joints stretched.
  if (!has_last_ || kern::length(st.position - last_pos_) > kern::length(st.velocity) * std::fmax(dt, 1 / 240.0f) * 2 + 0.6f) place(world, st);
  last_pos_ = st.position;
  has_last_ = true;
  return true;
}

void Car::push(kern::physics::World& world, kern::physics::Body body, const Quat& q, Vec3 origin, Vec3 f, Vec3 p, i32 on) {
  if (on < 0) world.apply_force_to_center(body, f);
  else world.apply_force(rig_[on].body, f, p);
  if (n_applied_ < 6) applied_[n_applied_++] = {to_local(q, p - origin), to_local(q, f), on};
}

void Car::reapply(kern::physics::World& world, kern::physics::Body body) const {
  if (rig_world_ != &world || rig_chassis_ != body) return;
  const BodyState st = read_body(world, body);
  // (the chassis was put elsewhere in between: nothing is pushed until the next step has put the
  // wheels under it again)
  if (has_last_ && kern::length(st.position - last_pos_) > kern::length(st.velocity) / 60 * 2 + 0.6f) return;
  for (i32 i = 0; i < n_applied_; ++i) {
    const Applied& a = applied_[i];
    const Vec3 f = st.rotation.rotate(a.f);
    if (a.on < 0) world.apply_force_to_center(body, f);
    else if (rig_[a.on].body.valid() && world.body_valid(rig_[a.on].body)) world.apply_force(rig_[a.on].body, f, st.position + st.rotation.rotate(a.p));
  }
}

void Car::step(kern::physics::World& world, kern::physics::Body body, f32 dt, const CarInput& input) {
  const CarSpec& spec = s_;
  n_applied_ = 0;
  if (!ensure_rig(world, body, dt)) return;
  const BodyState st = read_body(world, body);
  const Quat q = st.rotation;
  const Vec3 forward = q.rotate({1, 0, 0}), left = q.rotate({0, 1, 0}), up = q.rotate({0, 0, 1});
  const Vec3 com = st.position + q.rotate(Vec3{spec.com_x, 0, spec.com_z});
  auto point_velocity = [&](Vec3 p) { return st.velocity + kern::cross(st.angular, p - com); };
  const f32 vf = kern::dot(st.velocity, forward), speed = kern::length(st.velocity);
  const f32 r = spec.radius, pedal = clampf(input.pedal, -1, 1);
  const bool handbrake = input.handbrake;
  const f32 max_speed = clampf(input.max_speed, 3, 80), max_reverse = clampf(input.max_reverse, 1, 30);
  const f32 stick = clampf(input.steer, -1, 1), rack = spec.rack_rate * dt;

  // Automatic direction selector: D <-> R only when (nearly) stopped.
  gear_ = gear_ == -1 ? -1 : 1;
  enum { coast, drive, reverse, brake } mode = coast;
  if (std::fabs(pedal) > 0.02f) {
    const i32 desired = pedal > 0 ? 1 : -1;
    if (gear_ != desired) {
      if (speed > 0.65f) mode = brake;
      else { gear_ = desired; mode = desired > 0 ? drive : reverse; }
    } else mode = desired > 0 ? drive : reverse;
  }
  throttle_ = mode == drive || mode == reverse ? std::fabs(pedal) : 0;

  const f32 drive_sum = 2 * (spec.torque_front + spec.torque_rear) > 0 ? 2 * (spec.torque_front + spec.torque_rear) : 1;
  const f32 mq = spec.mass / 4;
  const f32 k = mq * kG / spec.sag, c_damp = 2 * kBodyDamping * std::sqrt(k * mq);
  const f32 I = spec.wheel_inertia, h = dt / kTyreSubsteps;

  // ---- the wheels: where each hub is on its suspension, and the load it carries (the
  // suspension's own spring + damper force at that travel: what presses the tyre on the ground)
  Vec3 patch[4], hub[4], normal[4];
  for (int i = 0; i < 4; ++i) {
    WheelState& w = w_[i];
    const Vec3 attach = st.position + q.rotate(Vec3{spec.wheels[i].x, spec.wheels[i].y, spec.attach_z});
    hub[i] = world.pose(rig_[i].body).position;
    const f32 travel = kern::dot(hub[i] - attach, up);
    const f32 rate = w.has_travel ? clampf((travel - w.travel) / std::fmax(dt, 1e-4f), -3, 3) : 0;
    w.travel = travel;
    w.has_travel = true;
    f32 N = k * travel + c_damp * rate;
    N = N > 0 ? N + spec.wheel_mass * kG : 0;
    w.load = N;
    w.contact = N > 0;
    w.hub = hub[i];
    normal[i] = up;
    patch[i] = hub[i] - up * r;
  }

  // ---- steering: equilibrium of hand torque + assist against the aligning torque
  struct Front { f32 pf, pl, N, muN, kappa; };
  Front fronts[2];
  int nf = 0;
  for (int i = 0; i < 4; ++i) {
    if (!spec.wheels[i].front) continue;
    WheelState& w = w_[i];
    const Vec3 pv = point_velocity(patch[i]);
    if (w.load_f < 0) w.load_f = mq * kG;
    w.load_f += (std::fmax(0.0f, w.load) - w.load_f) * std::fmin(1.0f, dt / 0.1f);
    const f32 N = w.load_f;
    fronts[nf++] = {kern::dot(pv, forward), kern::dot(pv, left), N, spec.friction * kAsphaltMu * std::pow(clampf(N / (mq * kG), 0.2f, 3), -kLoadSensitivity) * N,
                    w.kappa};
  }
  auto align_at = [&](f32 d) {
    f32 M = 0, tf[2];
    for (int i = 0; i < nf; ++i) {
      const Front& f = fronts[i];
      const f32 ca = std::cos(d), sa = std::sin(d), vx = f.pf * ca + f.pl * sa, vy = -f.pf * sa + f.pl * ca, ta = vy / std::fmax(std::fabs(vx), kVReg);
      tyre(f.kappa, ta, f.muN, tf);
      M += (kCasterTrail + kPneumaticTrail * std::fmax(0.0f, 1 - std::fabs(ta) / (kTrailCollapse * kAlphaPeak))) * tf[1] + f.N * kSteerJacking * std::sin(d);
    }
    return M;
  };
  f32 steer_eq = 0;
  if (nf && (std::fabs(stick) > 1e-3f || speed > 0.5f)) {
    const f32 T = stick * steer_torque_at(spec, speed), L = spec.steer_lock;
    const int n = 40;
    auto f = [&](f32 d) { return T - align_at(d); };
    f32 prev_d = -L, prev = f(-L);
    bool found = false;
    f32 best = 0;
    if (prev < 0) { best = -L; found = true; }
    for (int i = 1; i <= n; ++i) {
      const f32 d = -L + 2 * L * static_cast<f32>(i) / n, fd = f(d);
      if (fd == 0 || sign0(fd) != sign0(prev)) {
        f32 lo = prev_d, hi = d, flo = prev;
        for (int it = 0; it < 12; ++it) {
          const f32 m = (lo + hi) / 2, fm = f(m);
          if (sign0(fm) == sign0(flo)) { lo = m; flo = fm; } else hi = m;
        }
        const f32 root = (lo + hi) / 2;
        if (!found || std::fabs(root - steer_) < std::fabs(best - steer_)) best = root;
        found = true;
      }
      prev_d = d;
      prev = fd;
    }
    steer_eq = found ? best : (prev > 0 ? L : -L);
  }
  steer_eq_f_ += (steer_eq - steer_eq_f_) * std::fmin(1.0f, dt / (spec.steer_lag * (1 + speed / spec.steer_damp_speed)));
  steer_ += clampf(steer_eq_f_ - steer_, -rack, rack);

  // ---- tyres
  for (int i = 0; i < 4; ++i) {
    WheelState& w = w_[i];
    const bool front = spec.wheels[i].front;
    const f32 N = w.load;
    const f32 mu = spec.friction * kAsphaltMu * std::pow(clampf(N / (mq * kG), 0.2f, 3), -kLoadSensitivity), muN = mu * N;
    if (front) world.set_wheel_steering(rig_[i].joint, steer_);
    const f32 sa_ = front ? world.wheel_steering(rig_[i].joint) : 0, ca = std::cos(sa_), sa = std::sin(sa_);
    const Vec3 n = normal[i];
    Vec3 fw = forward * ca + left * sa;
    fw = kern::normalize(fw - n * kern::dot(fw, n));
    const Vec3 lw = kern::cross(n, fw);
    const Vec3 pv = point_velocity(patch[i]);
    const f32 vx = kern::dot(pv, fw), vy = kern::dot(pv, lw), V = std::fmax(std::fabs(vx), kVReg), tan_a = vy / V;
    const f32 drive_t = front ? spec.torque_front : spec.torque_rear, share = drive_t / drive_sum;
    f32 brake_t = (mode == brake ? spec.brake * std::fabs(pedal) : 0) + (drive_t > 0 && mode == coast ? spec.coast : 0) + spec.wheel_mass * 0.15f;
    if (handbrake && !front) brake_t = std::fmax(brake_t, spec.handbrake);
    f32 om = w.omega, fx_sum = 0, fy_sum = 0, tf[2], tf2[2];
    for (int sub = 0; sub < kTyreSubsteps; ++sub) {
      const f32 kap = (om * r - vx) / V;
      f32 Td = 0;
      if (drive_t > 0 && (mode == drive || mode == reverse)) {
        const f32 dir = mode == drive ? 1.0f : -1.0f, lim = mode == drive ? max_speed : max_reverse;
        Td = dir * std::fabs(pedal) * std::fmin(drive_t * (dir < 0 ? 0.7f : 1.0f), spec.power * share / std::fmax(4 / r, std::fabs(om)));
        if (dir * om * r > lim) Td = 0;
        const f32 ks = dir * kap;
        if (ks > kTcsSlip) Td *= clampf(1 - (ks - kTcsSlip) / 0.12f, 0, 1);
      }
      f32 Tb = brake_t;
      const f32 sgn = sign0(om != 0 ? om : vx);
      if (mode == brake && sgn * kap < -kAbsSlip) Tb *= clampf(1 - (-sgn * kap - kAbsSlip) / 0.12f, 0.12f, 1);
      tyre(kap, tan_a, muN, tf);
      const f32 fx0 = tf[0];
      tyre(kap + 0.02f / V * r, tan_a, muN, tf2);
      const f32 J = -(tf2[0] - fx0) / 0.02f * r;
      f32 wn = om + h * (Td - fx0 * r) / (I - h * std::fmin(0.0f, J));
      const f32 db = h * Tb / I;
      wn = std::fabs(wn) <= db ? 0 : wn - sign0(wn) * db;
      om = wn;
      tyre((om * r - vx) / V, tan_a, muN, tf);
      fx_sum += tf[0];
      fy_sum += tf[1];
    }
    w.omega = om;
    w.angle = std::fmod(w.angle + om * dt, 2 * kPi);
    f32 fx = fx_sum / kTyreSubsteps, fy = fy_sum / kTyreSubsteps;
    const f32 lim_y = mq * std::fabs(vy) / dt + 60;
    if (fx * vx < 0 && std::fabs(om * r) < std::fabs(vx)) {
      const f32 lim_x = mq * std::fabs(vx) / dt + 60;
      fx = clampf(fx, -lim_x, lim_x);
    }
    fy = clampf(fy, -lim_y, lim_y);
    fx -= spec.rolling_resistance * N * std::tanh(vx * 2);
    w.fx = fx;
    w.fy = fy;
    w.kappa = (om * r - vx) / V;
    w.alpha = std::atan(tan_a);
    // the tyre's force on the wheel at the contact patch; the joints carry it into the chassis
    if (N > 0) push(world, body, q, st.position, fw * fx + lw * fy, patch[i], i);
  }
  if (spec.drag_area > 0 && speed > 0.5f) push(world, body, q, st.position, st.velocity * (-0.5f * 1.2f * spec.drag_area * speed), st.position, -1);
  if (pedal != 0 || input.steer != 0 || handbrake || speed > 0.05f) world.set_awake(body, true);
  vf_ = vf;
}

CarInput ai_drive(Vec3 position, Vec3 forward, f32 vf, Vec3 target, f32 target_speed) {
  CarInput in;
  const f32 dx = target.x - position.x, dy = target.y - position.y, dist = std::hypot(dx, dy);
  if (dist < 0.4f) {
    in.pedal = -0.4f * sign0(vf);
    in.max_speed = std::fmax(4.0f, target_speed * 1.2f);
    in.max_reverse = 4;
    return in;
  }
  const f32 cr = forward.x * dy - forward.y * dx, dt = forward.x * dx + forward.y * dy, err = std::atan2(cr, dt);
  const f32 steer = clampf(err * 1.9f, -1, 1);
  const f32 R = dist / std::fmax(1e-3f, 2 * std::fabs(std::sin(err))), v_bend = std::sqrt(0.45f * 9.81f * R);
  const f32 ce = std::cos(err);
  const f32 want = std::fmin(target_speed * std::fmax(0.25f, ce * ce), std::fmax(2.5f, v_bend));
  const f32 pedal = clampf((want - vf) * 0.45f, -1, 1);
  const bool behind = dt < 0 && dist < 6;
  in.pedal = behind ? -0.6f : pedal;
  in.steer = behind ? -steer : steer;
  in.max_speed = std::fmax(6.0f, target_speed * 1.25f);
  in.max_reverse = 5;
  return in;
}

}  // namespace sc
