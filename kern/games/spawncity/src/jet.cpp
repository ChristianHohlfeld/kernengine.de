// SpawnCity's VTOL fighter flight model in C++ (see jet.h). A line-by-line port of
// sim/jet_flight_dynamics.mjs airframeStep(); comments there apply here.
#include "jet.h"

namespace sc {

namespace {
constexpr f32 G = 9.81f;   // the jet model's own constant (jet_flight_dynamics.mjs)
constexpr f32 kSound = 340;
f32 wave_cd(f32 m) { const f32 x = (m - 1.02f) / 0.16f; return std::exp(-(x * x)) * 0.012f + (m > 1.02f ? 0.006f : 0.0f); }
Vec3 nrm(Vec3 a) { const f32 l = kern::length(a); return l > 1e-9f ? a * (1.0f / l) : Vec3{}; }
}  // namespace

f32 air_density(f32 h) { return 1.225f * std::exp(-std::fmax(0.0f, h) / 8500.0f); }

RigidSpec Jet::rigid_spec(const JetSpec& s) {
  RigidSpec r;
  ColliderSpec fuselage;
  fuselage.kind = kern::physics::ShapeKind::hull;
  fuselage.points = {{-.8f, -7.2f, -.8f}, {.8f, -7.2f, -.8f}, {.8f, -7.2f, .8f}, {-.8f, -7.2f, .8f}, {-.8f, 5.4f, -.8f}, {.8f, 5.4f, -.8f},
                     {.8f, 5.4f, .8f},    {-.8f, 5.4f, .8f},  {-.15f, 9, -.15f}, {.15f, 9, -.15f},  {.15f, 9, .15f},     {-.15f, 9, .15f}};
  fuselage.friction = 0.5f;
  ColliderSpec wing;
  wing.kind = kern::physics::ShapeKind::hull;
  wing.points = {{-6, -3.2f, -.2f}, {-6, -4.2f, -.2f}, {-.4f, 1.6f, -.2f}, {-.4f, -4, -.2f}, {6, -3.2f, -.2f}, {6, -4.2f, -.2f}, {.4f, 1.6f, -.2f}, {.4f, -4, -.2f},
                 {-6, -3.2f, .1f},  {-6, -4.2f, .1f},  {-.4f, 1.6f, .1f},  {-.4f, -4, .1f},  {6, -3.2f, .1f},  {6, -4.2f, .1f},  {.4f, 1.6f, .1f},  {.4f, -4, .1f}};
  wing.friction = 0.5f;
  r.colliders = {fuselage, wing};
  r.mass.mass = s.mass;
  r.mass.center = {};
  r.mass.inertia[0] = s.inertia.x;
  r.mass.inertia[1] = s.inertia.y;
  r.mass.inertia[2] = s.inertia.z;
  r.mass.inertia[3] = r.mass.inertia[4] = r.mass.inertia[5] = 0;
  r.bullet = true;
  return r;
}

void Jet::reset() {
  mode_ = Mode::hover;
  lever_ = thrust_ = thrust_cmd_ = 0;
  nozzle_ = kPi / 2;
  de_ = da_ = dr_ = steer_ = vz_i_ = vz_cmd_ = 0;
  vz_cmd_valid_ = gamma_valid_ = false;
  gamma_hold_ = 0;
  gear_down_ = true;
  speed_brake_ = false;
  landed_ = true;
  contacts_ = 0;
  gear_impact_ = 0;
  engine_on = true;
  pilot = true;
  thrust_scale = 1;
  tele_ = {};
}

void Jet::set_mode(Mode m) {
  mode_ = m;
  if (mode_ == Mode::flight) lever_ = std::fmax(lever_, 0.9f);
}

Jet::Aero Jet::aero(Vec3 vb, Vec3 wb, f32 V, f32 rho, f32 de, f32 da, f32 dr) const {
  const JetSpec& s = s_;
  Aero a{};
  a.qbar = 0.5f * rho * V * V;
  const f32 u = vb.y, vs = vb.x, wu = vb.z;
  a.alpha = std::atan2(-wu, u);
  a.beta = V > 1e-3f ? std::asin(clampf(vs / V, -1, 1)) : 0;
  const f32 am = clampf(a.alpha, -1, 1);
  const f32 sig = 1.0f / (1.0f + std::exp(-(std::fabs(a.alpha) - s.alpha_stall) / 0.04f));
  const f32 CLlin = s.CL0 + s.CLa * am, CLfp = std::sin(2 * a.alpha);
  a.CL = (1 - sig) * CLlin + sig * CLfp;
  const f32 sa = std::sin(a.alpha);
  a.CD = s.CD0 + wave_cd(V / kSound) + (1 - sig) * s.K * CLlin * CLlin + sig * 2 * sa * sa + (gear_down_ ? s.gear_CD : 0) + (speed_brake_ ? s.brake_CD : 0);
  a.CY = s.CYb * a.beta;
  const f32 hv = V > 1 ? 0.5f / V : 0;
  const f32 qh = wb.x * s.chord * hv, ph = wb.y * s.span * hv, rh = -wb.z * s.span * hv;
  const f32 Cm = s.Cm0 + s.Cma * am + s.Cmq * qh + s.Cmde * de;
  const f32 Cl = s.Clb * a.beta + s.Clp * ph + s.Clr * rh + s.Clda * da;
  const f32 Cn = s.Cnb * a.beta + s.Cnr * rh + s.Cnp * ph + s.Cndr * dr;
  a.moment = {a.qbar * s.wing_area * s.chord * Cm, a.qbar * s.wing_area * s.span * Cl, -a.qbar * s.wing_area * s.span * Cn};
  return a;
}

void Jet::step(kern::physics::World& world, kern::physics::Body body, f32 dt, const JetInput& inp) {
  const JetSpec& s = s_;
  const BodyState st = read_body(world, body);
  const f32 m = s.mass;
  const Vec3 I = s.inertia;
  const Quat q = st.rotation;
  const Vec3 pos = st.position, v = st.velocity, w = st.angular;
  const Vec3 fwd = q.rotate({0, 1, 0}), right = q.rotate({1, 0, 0}), up = q.rotate({0, 0, 1});
  const Vec3 vb = to_local(q, v), wb = to_local(q, w);
  const f32 V = kern::length(v), rho = air_density(pos.z);
  const f32 att_pitch = std::asin(clampf(fwd.z, -1, 1)), att_roll = std::atan2(-right.z, up.z), att_heading = std::atan2(-fwd.x, fwd.y);
  Vec3 force, torque;

  // ---------------- landing gear (ray-cast struts)
  gear_down_ = mode_ == Mode::hover || V < s.wing_borne_v || contacts_ > 0;
  i32 contacts = 0;
  f32 gear_impact = 0;
  const f32 steer_target = V < 30 ? clampf(inp.yaw, -1, 1) * 0.5f : clampf(inp.yaw, -1, 1) * 0.08f;
  steer_ = rate_limit(steer_, steer_target, dt * 1.5f);
  if (gear_down_) {
    const Vec3 down = -up;
    RayResult hits[3];
    i32 n_contact = 0;
    for (int i = 0; i < 3; ++i) {
      const Vec3 attach = pos + q.rotate(s.gear[i].p);
      const RayResult h = cast(world, attach, down, s.strut_rest + 0.4f, body);
      if (h.hit && h.distance < s.strut_rest) { hits[i] = h; ++n_contact; }
    }
    for (int i = 0; i < 3; ++i) {
      if (!hits[i].hit) continue;
      const RayResult& hit = hits[i];
      const f32 comp = s.strut_rest - hit.distance;
      const Vec3 n = nrm(hit.normal);
      const Vec3 rc = hit.point - pos;
      const Vec3 vp = kern::cross(w, rc) + v;
      const f32 comp_rate = kern::dot(vp, down);
      const f32 share = s.gear[i].load * m, k = share * G / s.strut_static, c = 2 * s.strut_zeta * std::sqrt(k * share);
      const f32 stiff = comp > s.strut_max ? k * 12 * (comp - s.strut_max) : 0;
      if (comp_rate > gear_impact) gear_impact = comp_rate;
      const f32 N = std::fmax(0.0f, k * comp + stiff + c * comp_rate);
      ++contacts;
      Vec3 fw = nrm(fwd - n * kern::dot(fwd, n));
      if (s.gear[i].steer && steer_ != 0) {
        const f32 cs = std::cos(steer_), sn = std::sin(steer_);
        const Vec3 side = kern::cross(fw, n);
        fw = nrm(fw * cs + side * sn);
      }
      const Vec3 sw = kern::cross(fw, n);
      const f32 vf = kern::dot(vp, fw), vsd = kern::dot(vp, sw);
      auto m_at = [&](Vec3 d) {
        const Vec3 u = to_local(q, kern::cross(rc, d));
        return 1.0f / (1.0f / m + u.x * u.x / I.x + u.y * u.y / I.y + u.z * u.z / I.z);
      };
      const f32 share3 = 0.8f / std::fmax(1.0f, static_cast<f32>(n_contact));
      const f32 stop_l = m_at(fw) * share3 / std::fmax(1e-3f, dt), stop_s = m_at(sw) * share3 / std::fmax(1e-3f, dt);
      const f32 brake = clampf(inp.brake, 0, 1), long_max = (s.roll_mu + brake * s.brake_mu) * N, lat_max = s.tyre_mu * N;
      const f32 Fl = -sign0(vf) * std::fmin(long_max, std::fabs(vf) * stop_l), Fs = -sign0(vsd) * std::fmin(lat_max, std::fabs(vsd) * stop_s);
      const Vec3 F = n * N + fw * Fl + sw * Fs;
      force += F;
      torque += kern::cross(rc, F);
    }
  }
  gear_impact_ = contacts > 0 && contacts_ == 0 ? gear_impact : 0;
  contacts_ = contacts;

  // ---------------- flight computer
  const RayResult radar = cast(world, pos, {0, 0, -1}, 400, body);
  const f32 agl = radar.hit ? std::fmax(0.0f, radar.distance - s.gear_height) : 400;
  const bool pilot_ok = pilot && engine_on;
  const f32 w_flight = clampf((V - s.hover_law_v) / (s.wing_borne_v - s.hover_law_v), 0, 1);
  const bool on_ground = contacts >= 2;
  const f32 nozzle_target = !engine_on ? nozzle_
                            : mode_ == Mode::hover ? (V > s.hover_speed + 5 ? s.braking_nozzle : kPi / 2)
                                                   : s.transition_nozzle * clampf(1 - V / (s.wing_borne_v - 5), 0, 1);
  nozzle_ = rate_limit(nozzle_, nozzle_target, s.nozzle_rate * dt);
  const f32 cn = std::cos(nozzle_), sn = std::sin(nozzle_);
  const Vec3 thrust_dir = fwd * cn + up * sn;
  const f32 lapse = std::pow(rho / 1.225f, 0.7f) * clampf(thrust_scale, 0, 1), Tmax = s.thrust_max * lapse;
  if (mode_ == Mode::flight && pilot_ok) lever_ = clampf(lever_ + inp.lift * dt * 0.55f, 0, 1);
  speed_brake_ = (mode_ == Mode::flight && lever_ <= 0.001f && inp.lift < -0.4f) || (mode_ == Mode::hover && V > 30);
  const f32 vz_want = mode_ == Mode::flight ? 0
                      : inp.lift > 0 ? inp.lift * s.hover_climb
                                     : std::fmax(inp.lift * s.hover_sink, -std::fmin(std::sqrt(3 * agl) + 0.6f, 0.6f * agl + 0.8f));
  vz_cmd_ = rate_limit(vz_cmd_valid_ ? vz_cmd_ : v.z, vz_want, 11 * dt);
  vz_cmd_valid_ = true;
  const f32 vz_cmd = vz_cmd_;
  const f32 az_now = thrust_ * thrust_dir.z / m - G, vz_err = vz_cmd - (v.z + 0.8f * s.spool_up * az_now);
  const f32 tvert_raw = m * (G + 2.4f * vz_err + vz_i_) / std::fmax(0.25f, thrust_dir.z);
  if (!on_ground && tvert_raw > 0 && tvert_raw < Tmax) vz_i_ = 0;
  if (on_ground && inp.lift <= 0.05f) { vz_i_ = 0; vz_cmd_ = 0; }
  const f32 tvert = on_ground && inp.lift <= 0.05f ? 0 : clampf(tvert_raw, 0, Tmax);
  const f32 tlever = (lever_ > 0.86f ? s.thrust_dry + (s.thrust_max - s.thrust_dry) * (lever_ - 0.86f) / 0.14f : s.thrust_dry * lever_ / 0.86f) * lapse;
  const f32 support = mode_ == Mode::flight ? clampf(sn * 2.5f, 0, 1) : 1;
  thrust_cmd_ = !engine_on ? 0 : clampf(mode_ == Mode::flight ? tlever + support * std::fmax(0.0f, tvert - tlever) : tvert, 0, Tmax);
  thrust_ += (thrust_cmd_ - thrust_) * (1 - std::exp(-dt / (thrust_cmd_ > thrust_ ? s.spool_up : s.spool_down)));
  force += thrust_dir * thrust_;

  // rate commands: hover law blended into the flight law
  f32 pC = 0, qC = 0, rC = 0;
  {
    const f32 h = att_heading;
    const Vec2 fh{-std::sin(h), std::cos(h)}, rh{std::cos(h), std::sin(h)};
    const f32 vF = v.x * fh.x + v.y * fh.y, vR = v.x * rh.x + v.y * rh.y;
    const f32 aF = clampf(1.1f * (-inp.pitch * s.hover_speed - vF), -4, 4), aR = clampf(1.1f * (inp.roll * s.hover_speed - vR), -4, 4);
    const f32 tilt = s.hover_tilt * clampf(0.25f + agl / 8, 0, 1) * clampf((42 - V) / 20, 0.12f, 1);
    const bool trc = mode_ == Mode::hover;
    const f32 pitch_cmd = trc ? clampf(-std::atan(aF / G) + inp.pitch * 0.07f, -tilt, tilt) : 0.05f + inp.pitch * 0.3f;
    const f32 roll_cmd = trc ? clampf(std::atan(aR / G) + inp.roll * 0.33f, -tilt, tilt) : inp.roll * 0.5f;
    const f32 hq = 2.6f * (pitch_cmd - att_pitch), hp = 2.6f * (roll_cmd - att_roll), hr = inp.yaw * 0.8f;
    pC += (1 - w_flight) * hp;
    qC += (1 - w_flight) * hq;
    rC += (1 - w_flight) * hr;
  }
  {
    if (w_flight < 0.05f) gamma_valid_ = false;
    const f32 Vs = std::fmax(V, 30.0f), gamma = V > 1 ? std::asin(clampf(v.z / V, -1, 1)) : 0, phi = att_roll;
    f32 qf;
    if (std::fabs(inp.pitch) > 0.05f) {
      gamma_valid_ = false;
      qf = inp.pitch > 0 ? inp.pitch * std::fmin(s.q_max, s.g_limit * G / Vs) : inp.pitch * std::fmin(0.8f, s.g_push * G / Vs);
    } else {
      if (mode_ == Mode::hover) { gamma_hold_ = std::asin(clampf(vz_cmd / Vs, -0.5f, 0.5f)); gamma_valid_ = true; }
      else if (!gamma_valid_ || w_flight < 0.99f) { gamma_hold_ = w_flight < 0.99f ? 0 : gamma; gamma_valid_ = true; }
      const f32 level = std::fabs(phi) < 1.4f ? G * std::sin(phi) * std::tan(clampf(phi, -1.3f, 1.3f)) / Vs : 0;
      qf = std::fabs(phi) < 1.4f ? level + clampf(1.2f * (gamma_hold_ - gamma), -0.15f, 0.15f) * std::cos(phi) : 0;
    }
    const f32 a = std::atan2(-vb.z, std::fmax(0.5f, vb.y));
    if (a > s.alpha_limit) qf = std::fmin(qf, 2.5f * (s.alpha_limit - a));
    if (a < -0.2f) qf = std::fmax(qf, 2.5f * (-0.2f - a));
    const f32 rf = G * std::sin(phi) * std::cos(att_pitch) / Vs + inp.yaw * 0.35f;
    pC += w_flight * inp.roll * s.p_max;
    qC += w_flight * qf;
    rC += w_flight * rf;
  }
  const f32 p = wb.y, qq = wb.x, r = -wb.z;
  f32 want_x = I.x * 4.5f * (qC - qq), want_y = I.y * 35 * (pC - p), want_z = -I.z * 3.2f * (rC - r);
  const f32 beta = V > 1 ? std::asin(clampf(vb.x / V, -1, 1)) : 0;
  want_z += -I.z * w_flight * 1.8f * beta * std::fmin(1.0f, V / 60);
  const Vec3 Iw{I.x * wb.x, I.y * wb.y, I.z * wb.z}, gyro = kern::cross(wb, Iw);
  want_x += gyro.x;
  want_y += gyro.y;
  want_z += gyro.z;
  const bool ctl = pilot_ok && !(on_ground && (mode_ == Mode::hover || V < 40));
  const Aero base = aero(vb, wb, std::fmax(V, 1e-3f), rho, 0, 0, 0);
  const f32 qS = base.qbar * s.wing_area;
  f32 deT = 0, daT = 0, drT = 0;
  Vec3 rcs;
  if (ctl) {
    if (qS > 1) {
      deT = clampf((want_x - base.moment.x) / (qS * s.chord * s.Cmde), -s.de_max, s.de_max);
      daT = clampf((want_y - base.moment.y) / (qS * s.span * s.Clda), -s.da_max, s.da_max);
      drT = clampf(-(want_z - base.moment.z) / (qS * s.span * s.Cndr), -s.dr_max, s.dr_max);
    }
    const f32 avail = std::sin(nozzle_) * clampf(thrust_ / (0.4f * s.thrust_max), 0, 1);
    const Vec3 left{want_x - base.moment.x - qS * s.chord * s.Cmde * deT, want_y - base.moment.y - qS * s.span * s.Clda * daT,
                    want_z - base.moment.z + qS * s.span * s.Cndr * drT};
    rcs = {clampf(left.x, -s.rcs_max.x * avail, s.rcs_max.x * avail), clampf(left.y, -s.rcs_max.y * avail, s.rcs_max.y * avail),
           clampf(left.z, -s.rcs_max.z * avail, s.rcs_max.z * avail)};
  }
  de_ = rate_limit(de_, deT, s.surface_rate * dt);
  da_ = rate_limit(da_, daT, s.surface_rate * dt);
  dr_ = rate_limit(dr_, drT, s.surface_rate * dt);

  // ---------------- aerodynamics with the real surface positions
  Aero A{};
  bool has_aero = false;
  if (V > 0.5f) {
    A = aero(vb, wb, V, rho, de_, da_, dr_);
    has_aero = true;
    const Vec3 vh = v * (1.0f / V);
    const f32 qS2 = A.qbar * s.wing_area;
    const Vec3 lift_dir = nrm(up - vh * kern::dot(up, vh)), side_dir = nrm(right - vh * kern::dot(right, vh));
    force += lift_dir * (qS2 * A.CL);
    force += vh * (-qS2 * A.CD);
    force += side_dir * (qS2 * A.CY);
    torque += q.rotate(A.moment);
  }
  torque += q.rotate(rcs);

  world.apply_force_to_center(body, force);
  world.apply_torque(body, torque);

  landed_ = contacts >= 2 && V < 1.5f && mode_ == Mode::hover && inp.lift <= 0.15f;
  tele_.V = V;
  tele_.agl = agl;
  tele_.mach = V / kSound;
  tele_.alpha = has_aero ? A.alpha : 0;
  tele_.beta = has_aero ? A.beta : 0;
  tele_.nz = kern::dot(force, up) / (m * G);
  tele_.thrust = thrust_;
  tele_.nozzle = nozzle_;
  tele_.contacts = contacts;
  tele_.pitch = att_pitch;
  tele_.roll = att_roll;
  tele_.heading = att_heading;
  tele_.w_flight = w_flight;
}

}  // namespace sc
