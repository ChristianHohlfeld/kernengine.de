// SpawnCity's input maths: the Blueprint of the level's actor "Controls" computes it (see controls.h).
#include "controls.h"

#include "blueprint_call.h"

namespace sc::controls {
namespace {
using kern::logic::Value;
rules::Blueprint g_rules;
Value N(f32 x) { return rules::number(x); }
Value B(bool x) { return Value::boolean(x); }
f32 num(const char* function, const std::vector<Value>& args) { return static_cast<f32>(g_rules.number(function, args)); }
f32 f(const Value& v) { return static_cast<f32>(v.as_number()); }
/// A function with two numbers as results.
Vec2 pair(const char* function, const std::vector<Value>& args) {
  std::vector<Value> r;
  if (!g_rules.call(function, args, r) || r.size() < 2) return {};
  return {f(r[0]), f(r[1])};
}
Value to_value(const WorldLook& l) { return Value::structure({N(l.yaw_deg), N(l.pitch_deg), B(l.snapping), N(l.idle), B(l.owned)}); }
void from_value(const Value& v, WorldLook& l) {
  if (v.fields() < 5) return;
  l.yaw_deg = f(*v.field(0));
  l.pitch_deg = f(*v.field(1));
  l.snapping = v.field(2)->b;
  l.idle = f(*v.field(3));
  l.owned = v.field(4)->b;
}
void look(const char* function, const std::vector<Value>& args, WorldLook& l) {
  std::vector<Value> r;
  if (g_rules.call(function, args, r) && !r.empty()) from_value(r[0], l);
}
Value to_value(const GrenadeCook& g) {
  return Value::structure({Value::integer(g.carry), N(g.regen_at), B(g.cooking), N(g.at), N(g.power), Value::integer(g.source)});
}
void from_value(const Value& v, GrenadeCook& g) {
  if (v.fields() < 6) return;
  g.carry = static_cast<int>(v.field(0)->i);
  g.regen_at = f(*v.field(1));
  g.cooking = v.field(2)->b;
  g.at = f(*v.field(3));
  g.power = f(*v.field(4));
  g.source = static_cast<int>(v.field(5)->i);
}
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }

f32 smoothstep01(f32 t) { return num("Smoothstep01", {N(t)}); }
f32 walk_pad_axis(f32 v) { return num("WalkPadAxis", {N(v)}); }
Vec2 shape_first_person_move(f32 x, f32 y, f32 move_fineness, bool invert_h) { return pair("ShapeFirstPersonMove", {N(x), N(y), N(move_fineness), B(invert_h)}); }
f32 fps_dynamic_curve(f32 n) { return num("FpsDynamicCurve", {N(n)}); }
f32 look_curve_strength(f32 look_fineness) { return num("LookCurveStrength", {N(look_fineness)}); }
Vec2 shape_fps_stick(f32 x, f32 y, f32 inner, f32 curve_strength) { return pair("ShapeFpsStick", {N(x), N(y), N(inner), N(curve_strength)}); }

f32 LookAxis::step(f32 target, f32 dt) {
  const Vec2 r = pair("LookAxisStep", {N(velocity), N(target), N(dt)});
  velocity = r.x;
  return r.y;
}

void touch_look_delta(f32 dx, f32 dy, f32 speed_px_s, f32& yaw, f32& pitch) {
  const Vec2 r = pair("TouchLookDelta", {N(dx), N(dy), N(speed_px_s)});
  yaw = r.x;
  pitch = r.y;
}

AimAssist fps_aim_assist(f32 yaw, f32 pitch, f32 dist, f32 input, f32 in_yaw, f32 in_pitch, f32 s) {
  AimAssist a;
  std::vector<Value> r;
  if (!g_rules.call("FpsAimAssist", {N(yaw), N(pitch), N(dist), N(input), N(in_yaw), N(in_pitch), N(s)}, r) || r.size() < 4) return a;
  a.active = r[0].b;
  a.slowdown = f(r[1]);
  a.yaw = f(r[2]);
  a.pitch = f(r[3]);
  return a;
}
f32 aim_ellipse(f32 yaw_err, f32 pitch_err) { return num("AimEllipse", {N(yaw_err), N(pitch_err)}); }

f32 gamepad_axis(f32 v, f32 dz) { return num("GamepadAxis", {N(v), N(dz)}); }
f32 hard_cut(f32 v, f32 dz) { return num("HardCut", {N(v), N(dz)}); }
f32 height_axis(f32 rt, f32 lt) { return num("HeightAxis", {N(rt), N(lt)}); }
f32 phone_axis(f32 v, f32 fineness) { return num("PhoneAxis", {N(v), N(fineness)}); }
f32 game_state_stick_magnitude(f32 a) { return num("GameStickMagnitude", {N(a)}); }
f32 inverse_game_state_stick_magnitude(f32 v) { return num("InverseGameStickMagnitude", {N(v)}); }

void apply_game_stick_left(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, f32 max_kmh) {
  const Vec2 r = pair("GameStickLeft", {N(x), N(y), N(fineness), B(invert_h), N(max_kmh)});
  out.roll = r.x;
  out.pitch = r.y;
}
void apply_game_stick_right(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, bool invert_v) {
  const Vec2 r = pair("GameStickRight", {N(x), N(y), N(fineness), B(invert_h), B(invert_v)});
  out.yaw = r.x;
  out.body_pitch = r.y;
}
f32 clearance_pad_axis(f32 centre_y, f32 y, f32 height) { return num("ClearancePadAxis", {N(centre_y), N(y), N(height)}); }

void RateStick::step(Vec2 px, f32 dt) {
  rate.x = num("RateStickStep", {N(rate.x), N(px.x), N(dt), N(full_px_s), N(tau)});
  rate.y = num("RateStickStep", {N(rate.y), N(px.y), N(dt), N(full_px_s), N(tau)});
}

void WorldLook::add(f32 dx, f32 dy, f32 kx, f32 ky) { look("LookAdd", {to_value(*this), N(dx), N(dy), N(kx), N(ky)}, *this); }
void WorldLook::set(f32 yaw, f32 pitch) { look("LookSet", {N(yaw), N(pitch)}, *this); }
void WorldLook::release() { look("LookRelease", {to_value(*this)}, *this); }
void WorldLook::step(f32 dt, bool keep) { look("LookStep", {to_value(*this), N(dt), B(keep)}, *this); }

void CarHeadSpring::step(f32 x, f32 y, f32 dt) {
  const Vec2 r = pair("CarHeadStep", {N(yaw), N(pitch), N(x), N(y), N(dt)});
  yaw = r.x;
  pitch = r.y;
}

int ViewButton::step(bool down, f32 now) {
  std::vector<Value> r;
  if (!g_rules.call("ViewButtonStep", {B(was), B(held_map), N(down_at), N(map_at), B(down), N(now)}, r) || r.size() < 5) return 0;
  was = r[1].b;
  held_map = r[2].b;
  down_at = f(r[3]);
  map_at = f(r[4]);
  return static_cast<int>(r[0].i);
}

VoiceChord::Out VoiceChord::step(bool r3_now, bool y_now, bool allowed) {
  Out o;
  std::vector<Value> r;
  if (!g_rules.call("VoiceChordStep", {B(r3), B(y), B(latched), B(r3_now), B(y_now), B(allowed)}, r) || r.size() < 6) return o;
  o.mic = r[0].b;
  o.mute = r[1].b;
  o.suppress_weapon = r[2].b;
  r3 = r[3].b;
  y = r[4].b;
  latched = r[5].b;
  return o;
}

bool ReleaseLatch::step(const f32* values, int n) {
  std::vector<f64> list(values, values + n);
  std::vector<Value> r;
  if (!g_rules.call("ReleaseLatchStep", {B(armed), Value::of(std::move(list))}, r) || r.size() < 2) return false;
  armed = r[1].b;
  return r[0].b;
}

bool GrenadeCook::pull(f32 now, int from) {
  std::vector<Value> r;
  if (!g_rules.call("GrenadePull", {to_value(*this), N(now), Value::integer(from)}, r) || r.size() < 2) return false;
  from_value(r[0], *this);
  return r[1].b;
}
void GrenadeCook::drag(f32 up_px) { power = num("GrenadeDrag", {N(up_px)}); }
f32 GrenadeCook::release(f32 now) {
  std::vector<Value> r;
  if (!g_rules.call("GrenadeRelease", {to_value(*this), N(now)}, r) || r.size() < 2) return 0;
  from_value(r[0], *this);
  return f(r[1]);
}
bool GrenadeCook::step(f32 now) {
  std::vector<Value> r;
  if (!g_rules.call("GrenadeStep", {to_value(*this), N(now)}, r) || r.size() < 2) return false;
  from_value(r[0], *this);
  return r[1].b;
}
f32 GrenadeCook::left(f32 now) const { return num("GrenadeLeft", {to_value(*this), N(now)}); }
f32 GrenadeCook::speed() const { return num("GrenadeSpeed", {N(power)}); }
f32 grenade_fuse() { return static_cast<f32>(g_rules.variable("GrenadeFuse")); }

void Devices::platform(bool fine_pointer, bool coarse_pointer, i32 max_touch_points) {
  fine = fine_pointer;
  coarse = coarse_pointer;
  touch_points = max_touch_points;
  set(g_rules.flag("DevicePlatform", {B(fine), Value::integer(touch_points)}));
}
void Devices::input(Event e) {
  std::vector<Value> r;
  if (!g_rules.call("DeviceInput", {rules::of_enum(e), B(fine), B(coarse), B(desktop), B(pad)}, r) || r.size() < 2) return;
  set(r[0].b);
  pad = r[1].b;
}

std::string hint_line(const char* mode, const char* weapon, bool dead) {
  std::vector<Value> r;
  if (!g_rules.call("HintLine", {Value::text(mode ? mode : ""), Value::text(weapon ? weapon : ""), B(dead)}, r) || r.empty()) return {};
  return r[0].s;
}

}  // namespace sc::controls
