// SpawnCity's input maths (see controls.h; values from the files named there).
#include "controls.h"

#include <cstring>

namespace sc::controls {

f32 smoothstep01(f32 t) {
  t = clampf(t, 0, 1);
  return t * t * (3 - 2 * t);
}

f32 walk_pad_axis(f32 v) {
  const f32 x = clampf(v, -1, 1), a = std::fabs(x);
  if (a <= 0.10f) return 0;
  return sign0(x) * std::pow((a - 0.10f) / 0.90f, 1.28f);
}

Vec2 shape_first_person_move(f32 x, f32 y, f32 move_fineness, bool invert_h) {
  x = clampf(x, -1, 1);
  y = clampf(y, -1, 1);
  if (invert_h) x = -x;
  const f32 m = std::fmin(1.0f, std::hypot(x, y));
  if (m <= 1e-9f) return {};
  const f32 expo = 0.60f * (clampf(move_fineness, 1, 10) - 1) / 9;
  const f32 shaped = m * (1 - expo) + m * m * m * expo;
  return {x * shaped / m, y * shaped / m};
}

f32 fps_dynamic_curve(f32 n) {
  static const f32 k[5][3] = {{0, 0, 1.0f}, {0.3f, 0.24f, 0.62f}, {0.7f, 0.5f, 1.1f}, {0.9f, 0.89f, 1.5f}, {1, 1, 1.0f}};
  const f32 x = clampf(n, 0, 1);
  for (int i = 0; i < 4; ++i) {
    const f32 a = k[i][0], fa = k[i][1], da = k[i][2], b = k[i + 1][0], fb = k[i + 1][1], db = k[i + 1][2];
    if (x <= b) {
      const f32 h = b - a, t = (x - a) / h, t2 = t * t, t3 = t2 * t;
      return clampf((2 * t3 - 3 * t2 + 1) * fa + (t3 - 2 * t2 + t) * h * da + (-2 * t3 + 3 * t2) * fb + (t3 - t2) * h * db, 0, 1);
    }
  }
  return 1;
}

Vec2 shape_fps_stick(f32 x, f32 y, f32 inner, f32 curve_strength) {
  x = clampf(x, -1, 1);
  y = clampf(y, -1, 1);
  const f32 raw = std::fmin(1.0f, std::hypot(x, y));
  inner = clampf(inner, 0, 0.45f);
  const f32 outer = clampf(0.97f, inner + 0.01f, 1);
  if (raw <= inner) return {};
  const f32 n = clampf((raw - inner) / (outer - inner), 0, 1), kk = clampf(curve_strength, 0, 0.6f), anti = 0.045f;
  const f32 w = 0.5f + kk / 0.6f * 0.5f;
  const f32 curve = fps_dynamic_curve(n) * w + n * (1 - w);
  const f32 curved = n >= 1 ? 1.0f : clampf(anti + (1 - anti) * curve, 0, 1);
  const f32 s = curved / std::fmax(raw, 1e-9f);
  return {x * s, y * s};
}

f32 LookAxis::step(f32 target, f32 dt) {
  const f32 d = clampf(dt, 0, 0.25f);
  if (d <= 0) return 0;
  const f32 from = velocity;
  const bool reversing = from * target < 0, growing = std::fabs(target) > std::fabs(from);
  const f32 rate = reversing ? 40.0f : growing ? 26.0f : 40.0f;
  const f32 decay = std::exp(-rate * d);
  velocity = target + (from - target) * decay;
  return target * d + (from - target) * (1 - decay) / rate;
}

void touch_look_delta(f32 dx, f32 dy, f32 speed_px_s, f32& yaw, f32& pitch) {
  const f32 gain = 1 + 0.35f * smoothstep01((speed_px_s - 700) / 2200);
  yaw = dx * 0.0068f * gain;
  pitch = -dy * 0.0059f * gain;
}

f32 gamepad_axis(f32 v, f32 dz) {
  const f32 x = clampf(v, -1, 1), a = std::fabs(x), z = clampf(dz, 0, 0.5f);
  return a <= z ? 0.0f : sign0(x) * (a - z) / (1 - z);
}

f32 phone_axis(f32 v, f32 fineness) {
  const f32 x = clampf(v, -1, 1), e = 0.70f * (clampf(std::round(fineness), 1, 10) - 1) / 9 * 0.38f;
  return clampf(x * (1 - e) + x * x * x * e, -1, 1);
}

f32 game_state_stick_magnitude(f32 a) {
  a = clampf(std::fabs(a), 0, 1);
  if (a <= 0.035f) return 0;
  const f32 t = (a - 0.035f) / (1 - 0.035f);
  return clampf(t * (1 - 0.25f) + t * t * t * 0.25f, 0, 1);
}

f32 inverse_game_state_stick_magnitude(f32 v) {
  const f32 target = clampf(std::fabs(v), 0, 1);
  if (target == 0 || target == 1) return target;
  f32 lo = 0, hi = 1;
  for (int i = 0; i < 30; ++i) {
    const f32 mid = (lo + hi) / 2;
    if (game_state_stick_magnitude(mid) < target) lo = mid;
    else hi = mid;
  }
  return (lo + hi) / 2;
}

namespace {
Vec2 unit_disc(f32 x, f32 y) {
  const f32 l = std::hypot(x, y);
  if (l > 1) { x /= l; y /= l; }
  return {clampf(x, -1, 1), clampf(y, -1, 1)};
}
}  // namespace

void apply_game_stick_left(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, f32 max_kmh) {
  const Vec2 p = unit_disc(x, y);
  const f32 px = invert_h ? -p.x : p.x, fwd = -p.y, m = std::hypot(px, fwd);
  const f32 frac = phone_axis(m, fineness) * (clampf(max_kmh, 5, 90) / 90);
  const f32 enc = inverse_game_state_stick_magnitude(frac);
  const f32 k = m > 1e-9f ? enc / m : 0;
  out.roll = px * k;
  out.pitch = fwd * k;
}

void apply_game_stick_right(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, bool invert_v) {
  const Vec2 p = unit_disc(x, y);
  const f32 px = invert_h ? -p.x : p.x, py = invert_v ? -p.y : p.y;
  out.yaw = phone_axis(-px, fineness);
  out.body_pitch = phone_axis(-py, fineness);
}

void RateStick::step(Vec2 px, f32 dt) {
  if (dt <= 0) return;
  const f32 a = 1 - std::exp(-dt / tau);
  rate.x += (clampf(px.x / dt / full_px_s, -1, 1) - rate.x) * a;
  rate.y += (clampf(px.y / dt / full_px_s, -1, 1) - rate.y) * a;
}

void WorldLook::add(f32 dx, f32 dy, f32 kx, f32 ky) {
  owned = true;
  idle = 0;
  snapping = false;
  yaw_deg = std::fmod(yaw_deg + dx * kx + 540, 360.0f);
  if (yaw_deg < 0) yaw_deg += 360;
  yaw_deg -= 180;
  pitch_deg = clampf(pitch_deg - dy * ky, -75, 60);
}

void WorldLook::set(f32 yaw, f32 pitch) {
  owned = true;
  idle = 0;
  snapping = false;
  yaw_deg = std::fmod(yaw + 540, 360.0f);
  if (yaw_deg < 0) yaw_deg += 360;
  yaw_deg -= 180;
  pitch_deg = clampf(pitch, -75, 60);
}

void WorldLook::step(f32 dt, bool keep) {
  if (owned) {
    idle += dt;
    if (idle > 1.4f) release();  // desktop_controls.mjs LOOK_IDLE_SNAP_MS
  }
  if (snapping && !keep) {
    const f32 k = 1 - std::exp(-8 * dt);  // real_world_bootstrap.mjs WORLD_LOOK_SNAP_RATE
    yaw_deg -= yaw_deg * k;
    pitch_deg -= pitch_deg * k;
    if (std::fabs(yaw_deg) < 0.05f && std::fabs(pitch_deg) < 0.05f) { yaw_deg = pitch_deg = 0; snapping = false; }
  }
}

void CarHeadSpring::step(f32 x, f32 y, f32 dt) {
  const f32 k = 1 - std::exp(-dt * (std::fabs(x) + std::fabs(y) > 0.05f ? 9.0f : 6.0f));
  yaw += (x * 1.75f - yaw) * k;
  pitch += (y * 0.7f - pitch) * k;
}

int ViewButton::step(bool down, f32 now) {
  int out = 0;
  if (down && !was) { down_at = now; held_map = false; }
  if (down && !held_map && now - down_at > 0.42f) { held_map = true; map_at = now; out = 2; }
  if (!down && was && !held_map && now - down_at < 0.4f && !(now - map_at < 1.5f)) out = 1;
  was = down;
  return out;
}

VoiceChord::Out VoiceChord::step(bool r3_now, bool y_now, bool allowed) {
  Out o;
  o.mute = allowed && r3_now && y_now && !y;
  const bool next_latched = r3_now && (latched || o.mute);
  o.mic = allowed && !r3_now && r3 && !latched;
  o.suppress_weapon = o.mute;
  r3 = r3_now;
  y = y_now;
  latched = next_latched;
  return o;
}

bool ReleaseLatch::step(const f32* values, int n) {
  if (!armed) return false;
  for (int i = 0; i < n; ++i)
    if (values[i] > 0.18f) return true;
  armed = false;
  return false;
}

bool GrenadeCook::pull(f32 now, int from) {
  if (cooking || carry <= 0) return false;
  --carry;
  if (regen_at <= 0) regen_at = now + kRegen;
  cooking = true;
  at = now;
  power = 0.7f;
  source = from;
  return true;
}

f32 GrenadeCook::release(f32 now) {
  if (!cooking) return 0;
  cooking = false;
  return std::fmax(0.0f, kFuse - (now - at));
}

bool GrenadeCook::step(f32 now) {
  if (carry < kCarry && regen_at > 0 && now >= regen_at) {
    ++carry;
    regen_at = carry < kCarry ? now + kRegen : 0;
  }
  if (cooking && now - at >= kFuse) { cooking = false; return true; }
  return false;
}

void Devices::platform(bool fine_pointer, bool coarse_pointer, i32 max_touch_points) {
  fine = fine_pointer;
  coarse = coarse_pointer;
  touch_points = max_touch_points;
  set(fine || touch_points == 0);
}

std::string hint_line(const char* mode, const char* weapon, bool dead) {
  if (dead) return "DOWN · SPACE / ENTER / R = RESPAWN";
  const char* text = "";
  bool prefix = false;
  if (!std::strcmp(mode, "foot")) {
    text = "WASD move · SHIFT sprint · SPACE jump (2× jetpack) · LMB fire · RMB aim / scope · 1 MP 2 Glock 3 Sniper 5 Granatwerfer 6 Raketenwerfer "
           "7 Fäuste (LMB links · RMB rechts) 8 Messer · Z / MMB ping · T halten Granate / Q / WHEEL · F vehicle · V drone · 4 strike · G EMP · "
           "P multiplayer · M map · ESC menu";
    prefix = true;
  } else if (!std::strcmp(mode, "drone")) {
    text = "WASD fly · SPACE / SHIFT up / down · MOUSE turn · RMB look · LMB fire · R arm · C camera · 1-3 / Q weapon · V on foot · M map · ESC menu";
    prefix = true;
  } else if (!std::strcmp(mode, "car")) {
    text = "W / S gas / brake · A / D steer · SPACE handbrake · MOUSE look · C camera · F exit · M map · ESC menu";
  } else if (!std::strcmp(mode, "jet")) {
    text = "W / S throttle · A / D rudder · MOUSE pitch / roll · LMB / SPACE gun · R rocket · B bomb · V hover / flight · C camera · F exit / eject · M map · ESC menu";
  }
  std::string s;
  if (prefix && weapon && *weapon) s = std::string("[ ") + weapon + " ]  ";
  return s + text;
}

}  // namespace sc::controls
