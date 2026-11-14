// SpawnCity's input maths: the Blueprint of the level's actor "Controls" computes it (see controls.h).
#include "controls.h"

#include "blueprint_call.h"

namespace sc::controls {
namespace {
rules::Blueprint g_rules{"Controls"};
/// The Look of the Blueprint, as it takes and gives it.
struct Look {
  f64 yaw = 0, pitch = 0;
  bool snapping = false;
  f64 idle = 0;
  bool owned = false;
};
Look look_of(const WorldLook& l) { return {rules::decimal(l.yaw_deg), rules::decimal(l.pitch_deg), l.snapping, rules::decimal(l.idle), l.owned}; }
void take(const Look& v, WorldLook& l) {
  l.yaw_deg = static_cast<f32>(v.yaw);
  l.pitch_deg = static_cast<f32>(v.pitch);
  l.snapping = v.snapping;
  l.idle = static_cast<f32>(v.idle);
  l.owned = v.owned;
}
/// The Cook of the Blueprint, as it takes and gives it.
struct Cook {
  i32 carry = 0;
  f64 regen_at = 0;
  bool cooking = false;
  f64 at = 0, power = 0;
  i32 source = 0;
};
Cook cook_of(const GrenadeCook& g) { return {g.carry, rules::decimal(g.regen_at), g.cooking, rules::decimal(g.at), rules::decimal(g.power), g.source}; }
void take(const Cook& c, GrenadeCook& g) {
  g.carry = c.carry;
  g.regen_at = static_cast<f32>(c.regen_at);
  g.cooking = c.cooking;
  g.at = static_cast<f32>(c.at);
  g.power = static_cast<f32>(c.power);
  g.source = c.source;
}
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }

f32 smoothstep01(f32 t) { return static_cast<f32>(g_rules.number("Smoothstep01", rules::decimal(static_cast<f32>(t)))); }
f32 walk_pad_axis(f32 v) { return static_cast<f32>(g_rules.number("WalkPadAxis", rules::decimal(static_cast<f32>(v)))); }
Vec2 shape_first_person_move(f32 x, f32 y, f32 move_fineness, bool invert_h) {
  f64 rx = 0, ry = 0;
  g_rules.call("ShapeFirstPersonMove", std::tie(rx, ry), rules::decimal(x), rules::decimal(y), rules::decimal(move_fineness), invert_h);
  return {static_cast<f32>(rx), static_cast<f32>(ry)};
}
f32 fps_dynamic_curve(f32 n) { return static_cast<f32>(g_rules.number("FpsDynamicCurve", rules::decimal(static_cast<f32>(n)))); }
f32 look_curve_strength(f32 look_fineness) { return static_cast<f32>(g_rules.number("LookCurveStrength", rules::decimal(static_cast<f32>(look_fineness)))); }
Vec2 shape_fps_stick(f32 x, f32 y, f32 inner, f32 curve_strength) {
  f64 rx = 0, ry = 0;
  g_rules.call("ShapeFpsStick", std::tie(rx, ry), rules::decimal(x), rules::decimal(y), rules::decimal(inner), rules::decimal(curve_strength));
  return {static_cast<f32>(rx), static_cast<f32>(ry)};
}

f32 LookAxis::step(f32 target, f32 dt) {
  f64 rx = 0, ry = 0;
  g_rules.call("LookAxisStep", std::tie(rx, ry), rules::decimal(velocity), rules::decimal(target), rules::decimal(dt));
  const Vec2 r{static_cast<f32>(rx), static_cast<f32>(ry)};
  velocity = r.x;
  return r.y;
}

void touch_look_delta(f32 dx, f32 dy, f32 speed_px_s, f32& yaw, f32& pitch) {
  f64 rx = 0, ry = 0;
  g_rules.call("TouchLookDelta", std::tie(rx, ry), rules::decimal(dx), rules::decimal(dy), rules::decimal(speed_px_s));
  const Vec2 r{static_cast<f32>(rx), static_cast<f32>(ry)};
  yaw = r.x;
  pitch = r.y;
}

AimAssist fps_aim_assist(f32 yaw, f32 pitch, f32 dist, f32 input, f32 in_yaw, f32 in_pitch, f32 s) {
  AimAssist a;
  f64 slowdown = 0, turn_yaw = 0, turn_pitch = 0;
  g_rules.call("FpsAimAssist", std::tie(a.active, slowdown, turn_yaw, turn_pitch), rules::decimal(yaw), rules::decimal(pitch), rules::decimal(dist), rules::decimal(input), rules::decimal(in_yaw),
               rules::decimal(in_pitch), rules::decimal(s));
  a.slowdown = static_cast<f32>(slowdown);
  a.yaw = static_cast<f32>(turn_yaw);
  a.pitch = static_cast<f32>(turn_pitch);
  return a;
}
f32 aim_ellipse(f32 yaw_err, f32 pitch_err) { return static_cast<f32>(g_rules.number("AimEllipse", rules::decimal(static_cast<f32>(yaw_err)), rules::decimal(static_cast<f32>(pitch_err)))); }

f32 gamepad_axis(f32 v, f32 dz) { return static_cast<f32>(g_rules.number("GamepadAxis", rules::decimal(static_cast<f32>(v)), rules::decimal(static_cast<f32>(dz)))); }
f32 hard_cut(f32 v, f32 dz) { return static_cast<f32>(g_rules.number("HardCut", rules::decimal(static_cast<f32>(v)), rules::decimal(static_cast<f32>(dz)))); }
f32 height_axis(f32 rt, f32 lt) { return static_cast<f32>(g_rules.number("HeightAxis", rules::decimal(static_cast<f32>(rt)), rules::decimal(static_cast<f32>(lt)))); }
f32 phone_axis(f32 v, f32 fineness) { return static_cast<f32>(g_rules.number("PhoneAxis", rules::decimal(static_cast<f32>(v)), rules::decimal(static_cast<f32>(fineness)))); }
f32 game_state_stick_magnitude(f32 a) { return static_cast<f32>(g_rules.number("GameStickMagnitude", rules::decimal(static_cast<f32>(a)))); }
f32 inverse_game_state_stick_magnitude(f32 v) { return static_cast<f32>(g_rules.number("InverseGameStickMagnitude", rules::decimal(static_cast<f32>(v)))); }

void apply_game_stick_left(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, f32 max_kmh) {
  f64 rx = 0, ry = 0;
  g_rules.call("GameStickLeft", std::tie(rx, ry), rules::decimal(x), rules::decimal(y), rules::decimal(fineness), invert_h, rules::decimal(max_kmh));
  const Vec2 r{static_cast<f32>(rx), static_cast<f32>(ry)};
  out.roll = r.x;
  out.pitch = r.y;
}
void apply_game_stick_right(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, bool invert_v) {
  f64 rx = 0, ry = 0;
  g_rules.call("GameStickRight", std::tie(rx, ry), rules::decimal(x), rules::decimal(y), rules::decimal(fineness), invert_h, invert_v);
  const Vec2 r{static_cast<f32>(rx), static_cast<f32>(ry)};
  out.yaw = r.x;
  out.body_pitch = r.y;
}
f32 clearance_pad_axis(f32 centre_y, f32 y, f32 height) { return static_cast<f32>(g_rules.number("ClearancePadAxis", rules::decimal(static_cast<f32>(centre_y)), rules::decimal(static_cast<f32>(y)), rules::decimal(static_cast<f32>(height)))); }

void RateStick::step(Vec2 px, f32 dt) {
  rate.x = static_cast<f32>(g_rules.number("RateStickStep", rules::decimal(static_cast<f32>(rate.x)), rules::decimal(static_cast<f32>(px.x)), rules::decimal(static_cast<f32>(dt)), rules::decimal(static_cast<f32>(full_px_s)), rules::decimal(static_cast<f32>(tau))));
  rate.y = static_cast<f32>(g_rules.number("RateStickStep", rules::decimal(static_cast<f32>(rate.y)), rules::decimal(static_cast<f32>(px.y)), rules::decimal(static_cast<f32>(dt)), rules::decimal(static_cast<f32>(full_px_s)), rules::decimal(static_cast<f32>(tau))));
}

void WorldLook::add(f32 dx, f32 dy, f32 kx, f32 ky) {
  Look l;
  g_rules.call("LookAdd", std::tie(l), look_of(*this), rules::decimal(dx), rules::decimal(dy), rules::decimal(kx), rules::decimal(ky));
  take(l, *this);
}
void WorldLook::set(f32 yaw, f32 pitch) {
  Look l;
  g_rules.call("LookSet", std::tie(l), rules::decimal(yaw), rules::decimal(pitch));
  take(l, *this);
}
void WorldLook::release() {
  Look l;
  g_rules.call("LookRelease", std::tie(l), look_of(*this));
  take(l, *this);
}
void WorldLook::step(f32 dt, bool keep) {
  Look l;
  g_rules.call("LookStep", std::tie(l), look_of(*this), rules::decimal(dt), keep);
  take(l, *this);
}

void CarHeadSpring::step(f32 x, f32 y, f32 dt) {
  f64 rx = 0, ry = 0;
  g_rules.call("CarHeadStep", std::tie(rx, ry), rules::decimal(yaw), rules::decimal(pitch), rules::decimal(x), rules::decimal(y), rules::decimal(dt));
  const Vec2 r{static_cast<f32>(rx), static_cast<f32>(ry)};
  yaw = r.x;
  pitch = r.y;
}

int ViewButton::step(bool down, f32 now) {
  i32 out = 0;
  bool now_was = false, now_held_map = false;
  f64 now_down_at = 0, now_map_at = 0;
  g_rules.call("ViewButtonStep", std::tie(out, now_was, now_held_map, now_down_at, now_map_at), was, held_map, rules::decimal(down_at), rules::decimal(map_at), down, rules::decimal(now));
  was = now_was;
  held_map = now_held_map;
  down_at = static_cast<f32>(now_down_at);
  map_at = static_cast<f32>(now_map_at);
  return out;
}

VoiceChord::Out VoiceChord::step(bool r3_now, bool y_now, bool allowed) {
  Out o;
  g_rules.call("VoiceChordStep", std::tie(o.mic, o.mute, o.suppress_weapon, r3, y, latched), r3, y, latched, r3_now, y_now, allowed);
  return o;
}

bool ReleaseLatch::step(const std::array<f32, 9>& values) {
  std::vector<f64> list;
  for (f32 v : values) list.push_back(static_cast<f64>(v));
  bool neutral = false;
  g_rules.call("ReleaseLatchStep", std::tie(neutral, armed), armed, list);
  return neutral;
}

bool GrenadeCook::pull(f32 now, int from) {
  Cook c;
  bool pulled = false;
  g_rules.call("GrenadePull", std::tie(c, pulled), cook_of(*this), rules::decimal(now), from);
  take(c, *this);
  return pulled;
}
void GrenadeCook::drag(f32 up_px) { power = static_cast<f32>(g_rules.number("GrenadeDrag", rules::decimal(static_cast<f32>(up_px)))); }
f32 GrenadeCook::release(f32 now) {
  Cook c;
  f64 left = 0;
  g_rules.call("GrenadeRelease", std::tie(c, left), cook_of(*this), rules::decimal(now));
  take(c, *this);
  return static_cast<f32>(left);
}
bool GrenadeCook::step(f32 now) {
  Cook c;
  bool cooked_off = false;
  g_rules.call("GrenadeStep", std::tie(c, cooked_off), cook_of(*this), rules::decimal(now));
  take(c, *this);
  return cooked_off;
}
f32 GrenadeCook::left(f32 now) const { return static_cast<f32>(g_rules.number("GrenadeLeft", cook_of(*this), rules::decimal(now))); }
f32 GrenadeCook::speed() const { return static_cast<f32>(g_rules.number("GrenadeSpeed", rules::decimal(static_cast<f32>(power)))); }
f32 grenade_fuse() { return static_cast<f32>(g_rules.get<f64>("GrenadeFuse")); }

void Devices::platform(bool fine_pointer, bool coarse_pointer, i32 max_touch_points) {
  fine = fine_pointer;
  coarse = coarse_pointer;
  touch_points = max_touch_points;
  set(g_rules.flag("DevicePlatform", fine, static_cast<i64>(touch_points)));
}
void Devices::input(Event e) {
  bool now_desktop = false, now_pad = false;
  g_rules.call("DeviceInput", std::tie(now_desktop, now_pad), e, fine, coarse, desktop, pad);
  set(now_desktop);
  pad = now_pad;
}

std::string hint_line(const char* mode, const char* weapon, bool dead) {
  std::string line;
  g_rules.call("HintLine", std::tie(line), mode, weapon, dead);
  return line;
}

}  // namespace sc::controls
