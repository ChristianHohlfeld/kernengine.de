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
void tyre(f32 kappa, f32 tan_a, f32 muN, f32 out[2]) {
  const f32 sx = kappa / kKappaPeak, sy = tan_a / kAlphaPeak, s = std::hypot(sx, sy);
  if (s < 1e-6f || !(muN > 0)) { out[0] = out[1] = 0; return; }
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
  s.friction = 1.0f; s.rack_rate = 1.45f;
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
  // The wheels are no bodies here (ray-cast suspension): their mass rides on the chassis.
  const f32 total = s.mass + s.upper_mass + 4 * s.wheel_mass;
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
}

f32 Car::wheel_speed() const {
  f32 m = 0;
  for (const WheelState& w : w_) m = std::fmax(m, std::fabs(w.omega));
  return m * s_.radius;
}

void Car::step(kern::physics::World& world, kern::physics::Body body, f32 dt, const CarInput& input) {
  const CarSpec& spec = s_;
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
  const f32 mq = (spec.mass + spec.upper_mass + 4 * spec.wheel_mass) / 4;
  const f32 k = mq * kG / spec.sag, c_damp = 2 * kBodyDamping * std::sqrt(k * mq);
  const f32 I = spec.wheel_inertia, h = dt / kTyreSubsteps;

  // ---- suspension rays: contact, travel, load
  Vec3 patch[4], hub[4], normal[4];
  for (int i = 0; i < 4; ++i) {
    WheelState& w = w_[i];
    const Vec3 attach = st.position + q.rotate(Vec3{spec.wheels[i].x, spec.wheels[i].y, spec.attach_z});
    const Vec3 top = attach + up * spec.susp_upper;
    const f32 reach = spec.susp_upper - spec.susp_lower + r;
    const RayResult hit = cast(world, top, -up, reach + 0.05f, body);
    f32 travel = spec.susp_lower;
    w.contact = hit.hit && hit.distance <= reach;
    if (w.contact) travel = spec.susp_upper - (hit.distance - r);
    const f32 rate = w.has_travel ? clampf((travel - w.travel) / std::fmax(dt, 1e-4f), -3, 3) : 0;
    w.travel = travel;
    w.has_travel = true;
    f32 N = 0;
    if (w.contact) {
      N = k * travel + c_damp * rate;
      if (travel > spec.susp_upper) N += k * 20 * (travel - spec.susp_upper);  // bump stop
      N = std::fmax(0.0f, N);
    }
    w.load = N;
    hub[i] = attach + up * clampf(travel, spec.susp_lower, spec.susp_upper);
    w.hub = hub[i];
    normal[i] = w.contact ? kern::normalize(hit.normal) : up;
    patch[i] = w.contact ? hit.point : hub[i] - up * r;
    if (N > 0) world.apply_force(body, up * N, attach);  // the spring and damper push the chassis at the mount
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
    const f32 sa_ = front ? steer_ : 0, ca = std::cos(sa_), sa = std::sin(sa_);
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
    if (N > 0 && w.contact) world.apply_force(body, fw * fx + lw * fy, patch[i]);
  }
  if (spec.drag_area > 0 && speed > 0.5f) world.apply_force_to_center(body, st.velocity * (-0.5f * 1.2f * spec.drag_area * speed));
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
