// SpawnCity on Kern: camera shake, FPV stabilizer, speed streaks, day/night and light helpers (look.h).
#include "look.h"

#include <cstdio>
#include <cstdlib>

namespace sc::look {

namespace {
Vec3 mix3(Vec3 a, Vec3 b, f32 t) { return a + (b - a) * t; }
f32 smooth(f32 e0, f32 e1, f32 x) {
  const f32 t = clampf((x - e0) / (e1 - e0), 0, 1);
  return t * t * (3 - 2 * t);
}
f32 noise(f32 t, f32 o) { return std::sin(t + o) * 0.5f + std::sin(t * 2.31f + o * 1.7f) * 0.3f + std::sin(t * 4.17f + o * 2.3f) * 0.2f; }
Vec3 rotate_about(Vec3 v, Vec3 axis, f32 a) { return Quat::axis_angle(axis, a).rotate(v); }
}  // namespace

// ---------------------------------------------------------------------------------------------
// camera shake (camera_shake.mjs)

void Shake::add(f32 amount, f32 cap) {
  if (!(amount > 0)) return;
  trauma = std::fmax(trauma, std::fmin(cap, trauma + amount));
}

bool Shake::sample(f32 dt, f64 time_s, f32 scale, Vec3& angles, Vec2& offset) {
  angles = {};
  offset = {};
  if (trauma <= 0) return false;
  trauma = std::fmax(0.0f, trauma - kDecayPerS * clampf(dt, 0, 0.1f));
  const f32 s = trauma * trauma;
  if (s < 1e-4f) return false;
  const f32 t = static_cast<f32>(std::fmod(time_s, 1000.0)) * kFreq;
  angles = {kMaxAngle * s * scale * noise(t, seed), kMaxAngle * s * scale * noise(t, seed + 11), kMaxAngle * 0.7f * s * scale * noise(t, seed + 23)};
  offset = {kMaxOffset * s * scale * noise(t * 0.8f, seed + 37), kMaxOffset * s * scale * noise(t * 0.8f, seed + 51)};
  return true;
}

f32 weapon_trauma(bool full_auto, bool heavy, bool drone, f32& cap) {
  // Full-auto builds up to a steady rumble (capped at 0.42), heavy weapons punch.
  if (full_auto) { cap = 0.42f; return drone ? 0.07f : 0.035f; }
  cap = 1;
  return heavy ? 0.28f : 0.2f;
}

f32 blast_trauma(f32 distance, f32 radius) {
  radius = clampf(radius, 1, 400);
  const f32 reach = radius * 9 + 30;
  if (distance > reach) return 0;
  const f32 k = std::pow(1 - distance / reach, 1.6f), size = clampf(radius / 12, 0.25f, 0.9f);
  return k * size;
}

void shake_camera(kern::Camera& c, Vec3 angles, Vec2 offset) {
  const f32 dist = std::fmax(0.5f, kern::length(c.target - c.eye));
  Vec3 f = kern::normalize(c.target - c.eye, {1, 0, 0});
  Vec3 r = kern::normalize(kern::cross(f, c.up), {0, -1, 0});
  Vec3 u = kern::cross(r, f);
  // three.js order: rotateX (pitch about the camera's right), rotateY (yaw about its up), rotateZ (roll).
  f = rotate_about(f, r, angles.x);
  u = rotate_about(u, r, angles.x);
  f = rotate_about(f, u, angles.y);
  r = rotate_about(r, u, angles.y);
  u = rotate_about(u, f, angles.z);
  r = rotate_about(r, f, angles.z);
  c.eye += r * offset.x + u * offset.y;
  c.target = c.eye + f * dist;
  c.up = u;
}

// ---------------------------------------------------------------------------------------------
// FPV optical stabilizer (fpv_optical_stabilizer.mjs)

void Stabilizer::apply(kern::Camera& c, f32 dt) {
  const Vec3 raw_pos = c.eye, raw_target = c.target, raw_up = c.up;
  const bool teleport = !init || kern::length(pos - raw_pos) > kTeleport || kern::length(target - raw_target) > kTargetTeleport;
  if (teleport) {
    pos = raw_pos;
    target = raw_target;
    up = kern::normalize(raw_up, {0, 0, 1});
    init = true;
  } else {
    const f32 pa = damp_alpha(kPositionHz, dt), oa = damp_alpha(kOrientationHz, dt);
    pos += (raw_pos - pos) * pa;
    target += (raw_target - target) * oa;
    up = kern::normalize(up + (raw_up - up) * oa, {0, 0, 1});
  }
  lag = kern::length(pos - raw_pos);
  c.eye = pos;
  c.target = target;
  c.up = up;
}

// ---------------------------------------------------------------------------------------------
// speed streaks (speed_lines.mjs)

void SpeedLines::respawn(int i) {
  Streak& t = s[i];
  t.a = rng.unit() * 2 * kPi;
  t.r = 0.15f + rng.unit() * 0.5f;
  t.len = 0.08f + rng.unit() * 0.22f;
  t.v = 0.9f + rng.unit() * 1.4f;
  t.warm = i % 3 == 0;
}

f32 SpeedLines::update(Vec3 eye, f32 dt) {
  if (!seeded) { for (int i = 0; i < kCount; ++i) respawn(i); seeded = true; }
  if (have_last && dt > 0) {
    const f32 d = kern::length(eye - last) / dt;
    speed += (std::fmin(d, 90.0f) - speed) * std::fmin(1.0f, dt * 4);
  }
  last = eye;
  have_last = true;
  k = clampf((speed - kStart) / (kFull - kStart), 0, 1);
  if (k <= 0) return 0;
  for (int i = 0; i < kCount; ++i) {
    s[i].r += s[i].v * dt * (0.6f + 1.8f * k);
    if (s[i].r > 1.15f) respawn(i);
  }
  return k;
}

// ---------------------------------------------------------------------------------------------
// day and night

f32 day_lighting(f32 tod, f32 sun_azimuth_deg, f32 fog_density, f32 exposure, kern::Lighting& l, kern::SkyState& sky) {
  // Kern's GameWorld::apply_lighting (runtime.cpp) with the game's clock: 0.25 sunrise, 0.5 noon, 0.75 sunset.
  const f32 angle = (tod - 0.25f) * 2 * kPi;
  const f32 az = sun_azimuth_deg * kDeg;
  const Vec3 across{std::cos(az + kPi * 0.5f), std::sin(az + kPi * 0.5f), 0};
  const Vec3 toward{std::cos(az), std::sin(az), 0};
  const Vec3 sun = kern::normalize(across * (-std::cos(angle) * 0.62f) + toward * 0.38f + Vec3{0, 0, std::sin(angle)}, {0, 0, 1});
  const Vec3 moon = kern::normalize(Vec3{-sun.x, -sun.y, std::fmax(0.35f, -sun.z)}, {0, 0, 1});
  const f32 night = smooth(0.06f, -0.16f, sun.z);
  const f32 dusk = clampf(1 - std::fabs(sun.z) * 4, 0, 1) * (1 - night * 0.6f);
  const Vec3 day_sun{4.6f, 4.05f, 3.46f}, dusk_sun{3.75f, 2.05f, 1.02f}, moon_light{0.16f, 0.2f, 0.32f};
  l.sun_direction = sun.z > -0.04f ? sun : moon;
  l.sun_color = mix3(mix3(day_sun, dusk_sun, dusk), moon_light, night);
  l.sky_color = Vec3{0.035f, 0.05f, 0.1f} * night;
  l.ground_color = Vec3{0.012f, 0.012f, 0.02f} * night;
  l.fog_color = mix3(mix3({0.474f, 0.584f, 0.716f}, {0.62f, 0.45f, 0.36f}, dusk * 0.6f), {0.03f, 0.04f, 0.07f}, night);
  l.fog_density = fog_density;
  l.fog_height_falloff = 0.012f;
  l.fog_sun_scatter = 0.55f * (1 - night * 0.7f);
  l.fog_horizon = 0.85f;
  l.env_intensity = 0.53f - 0.41f * night;
  l.env_zenith = mix3({0.05f, 0.2f, 0.62f}, {0.012f, 0.022f, 0.06f}, night);
  l.env_horizon = mix3({0.52f, 0.66f, 0.86f}, {0.05f, 0.075f, 0.13f}, night);
  {
    const auto lum = [](Vec3 c) { return 0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z; };
    const f32 sky_radiance = lum(mix3(l.env_horizon, l.env_zenith, 0.35f)) * l.env_intensity;
    const f32 irradiance = lum(l.sun_color) * std::fmax(sun.z, 0.0f) + kPi * sky_radiance;
    const Vec3 bounce = Vec3{1.0f, 0.95f, 0.85f} * (0.25f * irradiance / kPi / std::fmax(l.env_intensity, 1e-3f));
    l.env_ground = mix3(bounce, {0.105f, 0.127f, 0.084f}, night);
  }
  l.exposure = exposure * (1.0f + 0.35f * night);
  sky.sun = sun;
  sky.moon = moon;
  sky.night = night;
  return night;
}

f32 follow_phase(f32 mine, f32 authority) {
  f32 d = authority - mine;
  d -= std::round(d);
  if (std::fabs(d) < 0.0005f) return mine;
  f32 p = mine + (std::fabs(d) < 0.01f ? d * 0.5f : d);
  p -= std::floor(p);
  return p;
}

bool parse_time(const char* text, f32& phase) {
  while (*text == ' ') ++text;
  if (!*text) return false;
  char* end = nullptr;
  const f64 a = std::strtod(text, &end);
  if (end == text) return false;
  if (*end == ':') {
    const f64 m = std::strtod(end + 1, &end);
    if (a < 0 || a > 24 || m < 0 || m >= 60) return false;
    phase = static_cast<f32>(std::fmod((a + m / 60) / 24, 1.0));
    return true;
  }
  if (a < 0) return false;
  // Up to 1: a phase; otherwise hours.
  phase = static_cast<f32>(a <= 1 ? std::fmod(a, 1.0) : std::fmod(a / 24, 1.0));
  return true;
}

// ---------------------------------------------------------------------------------------------
// lights

f32 kern_intensity(f32 three_intensity, f32 range, f32 d_ref) {
  const f32 d2 = std::fmax(d_ref * d_ref, 0.04f), r2 = std::fmax(0.01f, range * range);
  const f32 w = 1 - d2 / r2;
  const f32 att = w > 0 ? w * w / (1 + d2 / r2) : 0;
  if (att <= 0) return 0;
  // three: E = I / d^2, diffuse BRDF albedo / pi. Kern: point = I_k 0.12 att (times pi / pi).
  return three_intensity / (kPi * d2) / (0.12f * att);
}

Vec3 hex_linear(u32 rgb) {
  auto lin = [](f32 c) { return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f); };
  return {lin(static_cast<f32>((rgb >> 16) & 255) / 255), lin(static_cast<f32>((rgb >> 8) & 255) / 255), lin(static_cast<f32>(rgb & 255) / 255)};
}

int police_flash(f32 now_s) {
  const i64 slot = static_cast<i64>(std::floor(now_s * 1000 / 110)) % 6;
  return slot == 0 || slot == 2 ? 1 : slot == 3 || slot == 5 ? 2 : 0;
}

int drone_flash(f32 now_s, int index) {
  const i64 slot = ((static_cast<i64>(std::floor((now_s * 1000 + index * 37) / 92)) % 8) + 8) % 8;
  return slot <= 1 ? 1 : slot == 4 || slot == 5 ? 2 : 0;
}

}  // namespace sc::look
