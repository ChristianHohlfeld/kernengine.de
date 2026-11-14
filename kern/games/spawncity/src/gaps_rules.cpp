// Pure rules of sc-m3-gaps (see gaps_rules.h). Values from the SpawnCity modules named there.
#include "gaps_rules.h"

#include <algorithm>
#include <cmath>

#include "blueprint_call.h"

namespace sc::gaps {

// ---- police_sirens.mjs, the dispatch grace, the organic crack: the Blueprint of the actor "Sirens"
namespace {
using rules::decimal;
rules::Blueprint g_sirens{"Sirens"};
}  // namespace
void bind_sirens(kern::logic::Instance* blueprint) { g_sirens.running = blueprint; }
f32 horn_low() { return static_cast<f32>(g_sirens.get<f64>("HornLow")); }
f32 horn_high() { return static_cast<f32>(g_sirens.get<f64>("HornHigh")); }
f32 car_gain() { return static_cast<f32>(g_sirens.get<f64>("CarGain")); }
f32 drone_gain() { return static_cast<f32>(g_sirens.get<f64>("DroneGain")); }
f32 whoop_base() { return static_cast<f32>(g_sirens.get<f64>("WhoopBase")); }
f32 organic_gain() { return static_cast<f32>(g_sirens.get<f64>("OrganicGain")); }
f32 organic_min_gap_s() { return static_cast<f32>(g_sirens.get<f64>("OrganicMinGapS")); }

f32 horn_hz(f32 t) {
  f64 hz = 0;
  g_sirens.call("HornHz", std::tie(hz), decimal(t));
  return static_cast<f32>(hz);
}
f32 whoop_hz(f32 t) {
  f64 hz = 0;
  g_sirens.call("WhoopHz", std::tie(hz), decimal(t));
  return static_cast<f32>(hz);
}
f32 siren_gain(f32 base, f32 d, f32 level) {
  f64 gain = 0;
  g_sirens.call("SirenGain", std::tie(gain), decimal(base), decimal(d), decimal(level));
  return static_cast<f32>(gain);
}
f32 siren_cutoff(f32 d) {
  f64 hz = 0;
  g_sirens.call("SirenCutoff", std::tie(hz), decimal(d));
  return static_cast<f32>(hz);
}

f32 SirenVoice::update(f32 d, f32 dt, f32 base, f32 level) {
  f64 gain = 0, next_last_d = 0, next_doppler = 0;
  g_sirens.call("VoiceUpdate", std::tie(gain, have_d, next_last_d, next_doppler), decimal(d), decimal(dt), decimal(base), decimal(level), have_d, decimal(last_d), decimal(doppler));
  last_d = static_cast<f32>(next_last_d);
  doppler = static_cast<f32>(next_doppler);
  return static_cast<f32>(gain);
}

f32 post_arrival_grace_ms(i32 stars) {
  f64 ms = 0;
  g_sirens.call("PostArrivalGraceMs", std::tie(ms), stars);
  return static_cast<f32>(ms);
}
f32 organic_rate(Organic k) {
  f64 rate = 0;
  g_sirens.call("OrganicRate", std::tie(rate), k);
  return static_cast<f32>(rate);
}

// ---- world_critical_damage_fx.mjs
u32 hash_text(const std::string& s) {
  u32 h = 2166136261u;
  for (unsigned char c : s) { h ^= c; h *= 16777619u; }
  return h;
}

f32 unit_hash(u32 x) {
  x ^= x >> 16;
  x *= 0x7feb352du;
  x ^= x >> 15;
  x *= 0x846ca68bu;
  x ^= x >> 16;
  return static_cast<f32>(static_cast<f64>(x) / 4294967295.0);
}

namespace {
Vec3 rgb(u32 hex) { return {static_cast<f32>((hex >> 16) & 255) / 255, static_cast<f32>((hex >> 8) & 255) / 255, static_cast<f32>(hex & 255) / 255}; }
}  // namespace

f32 SmokeField::start(const std::string& id, Vec3 at, f32 scale, f32 delay_s, f32 now) {
  for (SmokeEffect& e : effects_)
    if (e.id == id) { e.at = at; return e.expires; }
  SmokeEffect e;
  e.id = id;
  e.at = at;
  e.scale = std::fmax(0.35f, scale);
  e.started = now;
  e.expires = now + std::fmax(0.25f, delay_s);
  e.seed = hash_text(id);
  effects_.push_back(e);
  emit(effects_.back(), now);
  effects_.back().last_emit = now;
  return e.expires;
}

bool SmokeField::accelerate(const std::string& id, f32 expires_at, f32 now) {
  for (SmokeEffect& e : effects_)
    if (e.id == id) { e.expires = std::fmin(e.expires, std::fmax(now + 0.18f, expires_at)); return true; }
  return false;
}

void SmokeField::stop(const std::string& id) {
  effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [&](const SmokeEffect& e) { return e.id == id; }), effects_.end());
}

void SmokeField::move(const std::string& id, Vec3 at) {
  for (SmokeEffect& e : effects_)
    if (e.id == id) e.at = at;
}

bool SmokeField::active(const std::string& id) const {
  for (const SmokeEffect& e : effects_)
    if (e.id == id) return true;
  return false;
}

void SmokeField::emit(SmokeEffect& e, f32 now) {
  if (puffs_.empty()) return;
  // choosePuff: a free one, else the oldest
  i32 at = -1;  // (its place in puffs_)
  for (std::size_t i = 0; i < puffs_.size(); ++i)
    if (!puffs_[i].alive) { at = static_cast<i32>(i); break; }
  if (at < 0) {
    at = 0;
    for (std::size_t i = 0; i < puffs_.size(); ++i)
      if (puffs_[i].born < puffs_[static_cast<std::size_t>(at)].born) at = static_cast<i32>(i);
  }
  const f32 progress = clampf((now - e.started) / std::fmax(1e-3f, e.expires - e.started), 0, 1);
  const u32 serial = e.serial;
  e.serial += 1;
  const u32 seed = hash_text(e.id + ":" + std::to_string(serial));
  const f32 a = unit_hash(seed) * 2 * kPi, side = (unit_hash(seed + 17) - 0.5f) * (0.22f + e.scale * 0.18f);
  const bool ember = progress > 0.67f && serial % 5 == 0;
  Puff& p = puffs_[static_cast<std::size_t>(at)];
  p.alive = true;
  p.ember = ember;
  p.pos = e.at + Vec3{std::cos(a) * side, std::sin(a) * side, 0};
  p.vel = {std::cos(a) * (0.07f + unit_hash(seed + 31) * 0.19f), std::sin(a) * (0.07f + unit_hash(seed + 47) * 0.19f), 0.46f + unit_hash(seed + 61) * 0.40f + progress * 0.30f};
  p.born = now;
  p.expires = now + (ember ? 0.42f : 1.25f + unit_hash(seed + 79) * 0.52f);
  p.s0 = e.scale * (ember ? 0.10f : 0.38f + unit_hash(seed + 97) * 0.19f);
  p.s1 = e.scale * (ember ? 0.035f : 1.32f + unit_hash(seed + 113) * 0.42f);
  p.serial = serial;
  p.rotation = 0;
  p.color = ember ? rgb(0xff8a3d) : progress > 0.55f ? rgb(0xb9bec6) : rgb(0x8f949c);
  p.opacity = ember ? 0.94f : 0.54f + progress * 0.18f;
}

void SmokeField::update(f32 now, f32 dt) {
  for (std::size_t i = 0; i < effects_.size();) {
    SmokeEffect& e = effects_[i];
    if (now >= e.expires) { effects_.erase(effects_.begin() + static_cast<std::ptrdiff_t>(i)); continue; }
    const f32 progress = clampf((now - e.started) / std::fmax(1e-3f, e.expires - e.started), 0, 1);
    const f32 interval = (155 - progress * 72) / 1000;
    if (now - e.last_emit >= interval) {
      e.last_emit = now;
      emit(e, now);
      if (progress > 0.76f) emit(e, now + 0.001f);
    }
    ++i;
  }
  for (Puff& p : puffs_) {
    if (!p.alive) continue;
    if (now >= p.expires) { p.alive = false; continue; }
    p.pos += p.vel * dt;
    const f32 k = std::exp(-0.8f * dt);
    p.vel.x *= k;
    p.vel.y *= k;
    p.rotation += dt * (p.serial & 1 ? -0.42f : 0.42f);
    p.opacity = opacity(p, now);
  }
}

u32 SmokeField::live_puffs() const {
  u32 n = 0;
  for (const Puff& p : puffs_) n += p.alive ? 1 : 0;
  return n;
}

f32 SmokeField::opacity(const Puff& p, f32 now) {
  const f32 t = clampf((now - p.born) / std::fmax(1e-3f, p.expires - p.born), 0, 1);
  return (p.ember ? 0.94f : 0.66f) * std::sin(kPi * std::fmin(1.0f, t * 1.12f)) * (1 - t * 0.48f);
}

f32 SmokeField::size(const Puff& p, f32 now) {
  const f32 t = clampf((now - p.born) / std::fmax(1e-3f, p.expires - p.born), 0, 1);
  return p.s0 + (p.s1 - p.s0) * (1 - (1 - t) * (1 - t));
}

// ---- visual_pose_stabilization.mjs
namespace {
f32 damping(f32 rate, f32 dt) { const f32 r = std::fmax(0.0f, rate), d = clampf(dt, 0, 0.1f); return r > 0 && d > 0 ? 1 - std::exp(-r * d) : 0; }
Quat normalized(Quat q) {
  const f32 l = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  if (!(l > 1e-9f)) return Quat{};
  return Quat{q.x / l, q.y / l, q.z / l, q.w / l};
}
}  // namespace

Quat slerp(Quat a, Quat b, f32 t) {
  f32 d = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  if (d < 0) { d = -d; b = Quat{-b.x, -b.y, -b.z, -b.w}; }
  d = clampf(d, -1, 1);
  if (d > 0.9995f) return normalized(Quat{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t});
  const f32 th = std::acos(d), s = std::sin(th), wa = std::sin((1 - t) * th) / s, wb = std::sin(t * th) / s;
  return normalized(Quat{a.x * wa + b.x * wb, a.y * wa + b.y * wb, a.z * wa + b.z * wb, a.w * wa + b.w * wb});
}

f32 quat_angle(Quat a, Quat b) { return 2 * std::acos(clampf(std::fabs(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w), -1, 1)); }

void VisualPose::update(Vec3 position, Quat rotation, Vec3 anchor, i32 mode, f32 dt, Vec3& out_p, Quat& out_q) {
  const VisualProfile& p = mode == 2 ? kVisualThird : kVisualFollow;
  const Quat target = normalized(rotation);
  const Vec3 raw = position - anchor;
  if (!init_ || mode_ != mode) {
    offset_ = raw;
    q_ = target;
    mode_ = mode;
    init_ = true;
    out_p = position;
    out_q = q_;
    return;
  }
  const f32 step = clampf(dt, 0, 0.1f);
  offset_ += (raw - offset_) * damping(p.offset_rate, step);
  Vec3 e = raw - offset_;
  f32 err = kern::length(e);
  if (err > p.soft_pos) offset_ += e * ((err - p.soft_pos) * damping(p.pos_recovery, step) / err);
  e = raw - offset_;
  err = kern::length(e);
  if (err > p.max_pos) offset_ += e * ((err - p.max_pos) / err);
  q_ = slerp(q_, target, damping(p.rotation_rate, step));
  f32 ang = quat_angle(q_, target);
  if (ang > p.soft_rot) q_ = slerp(q_, target, clampf((ang - p.soft_rot) * damping(p.rot_recovery, step) / ang, 0, 1));
  ang = quat_angle(q_, target);
  if (ang > p.max_rot) q_ = slerp(q_, target, 1 - p.max_rot / ang);
  out_p = anchor + offset_;
  out_q = q_;
}

// ---- organic hit

}  // namespace sc::gaps
