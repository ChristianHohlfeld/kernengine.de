// SpawnCity's input maths, exactly as spawncity.de computes it (see CONTROLS.md for the table of
// every device and mode). Pure functions and small state machines without GameWorld, so the native
// tests check them directly; game_controls.cpp wires them to the modes.
//
//   fps_control_math.mjs            shape_fps_stick, fps_dynamic_curve, LookAxis (rate integration),
//                                   touch_look_delta
//   first_person_control_settings   shape_first_person_move, look curve strength
//   player_walk_mode_v4.mjs         walk_pad_axis (.10, ^1.28)
//   xbox_gamepad.mjs                gamepad_axis (.14 linear), height axis, ReleaseLatch
//   player_car_mode.mjs / jet_mode  car (.08 linear) / jet (.12 hard cut) dead zones, CarHeadSpring
//   control_semantics.mjs           phone_axis, game_state_stick_magnitude (+ inverse),
//                                   apply_game_stick_left / _right, clearance pad axis
//   desktop_controls.mjs            RateStick (drone 1500 px/s, 50 ms; jet 730 px/s, 25 ms),
//                                   WorldLook (0.12 deg/px, snaps back after 1.4 s at 8/s)
//   pad_actions.mjs                 ViewButton (tap < 400 ms = camera, hold > 0.42 s = map)
//   voice_control_bindings.mjs      VoiceChord (R3 tap on release, R3+Y mute)
//   hand_grenade.mjs                GrenadeCook (4 s fuse, 3 carried, +1 per 20 s, drag power)
#pragma once

#include <string>

#include "common.h"

namespace sc::controls {

constexpr f32 kPi = 3.14159265358979f;

f32 smoothstep01(f32 t);

// ---- on foot -------------------------------------------------------------------------------
/// player_walk_mode_v4.mjs axis(): |v| <= .10 -> 0, sign * ((|v| - .10) / .90)^1.28.
f32 walk_pad_axis(f32 v);
/// first_person_control_settings.mjs shapeFirstPersonMove: expo .60 (f - 1) / 9 on the magnitude.
Vec2 shape_first_person_move(f32 x, f32 y, f32 move_fineness, bool invert_h);
/// Monotone cubic Hermite through (0,0,1) (.3,.24,.62) (.7,.5,1.1) (.9,.89,1.5) (1,1,1).
f32 fps_dynamic_curve(f32 n);
/// first_person_control_settings.mjs: curve strength .48 (lookFineness - 1) / 9.
inline f32 look_curve_strength(f32 look_fineness) { return 0.48f * (clampf(look_fineness, 1, 10) - 1) / 9; }
/// shapeFpsStick ("dynamic"): inner dead zone, outer .97, anti dead zone .045.
Vec2 shape_fps_stick(f32 x, f32 y, f32 inner, f32 curve_strength);

/// integrateFpsLookVelocity: one axis of the look rate (rad/s), 26 /s growing, 40 /s releasing.
struct LookAxis {
  f32 velocity = 0;
  /// Integrates towards `target` (rad/s) for dt (<= .25 s); returns the angle turned (rad).
  f32 step(f32 target, f32 dt);
};

/// fpsTouchLookDelta: 1:1 drag (0.0068 / 0.0059 rad per px), flick gain up to 1.35x from 700 px/s.
/// Out: yaw right-positive, pitch up-positive (rad).
void touch_look_delta(f32 dx, f32 dy, f32 speed_px_s, f32& yaw, f32& pitch);

/// fpsAimAssist (fps_control_math.mjs) with the profile scaled by the strength s = percent / 100
/// (first_person_control_settings.mjs): window 7 x 5.6 deg, 90 m, slowdown .30 s, gain 4 s, cap .38 s rad/s.
/// Errors and inputs: yaw right-positive, pitch up-positive (rad, rad/s).
struct AimAssist { bool active = false; f32 slowdown = 1, yaw = 0, pitch = 0; };
AimAssist fps_aim_assist(f32 yaw_err, f32 pitch_err, f32 dist, f32 input, f32 in_yaw, f32 in_pitch, f32 s);
/// The candidate's place in the window (>= 1: outside) and its score (ellipse + dist / 90 * .12).
inline f32 aim_ellipse(f32 yaw_err, f32 pitch_err) { return std::hypot(yaw_err / (7.0f * kPi / 180), pitch_err / (5.6f * kPi / 180)); }

// ---- gamepad dead zones --------------------------------------------------------------------
/// Linear rescale outside the dead zone (xbox_gamepad.mjs gamepadAxis, car .08, map .15).
f32 gamepad_axis(f32 v, f32 dz = 0.14f);
/// jet_mode.mjs: |a| < .12 -> 0, otherwise unchanged (no rescale).
inline f32 hard_cut(f32 v, f32 dz) { return std::fabs(v) < dz ? 0.0f : v; }
/// xbox_gamepad.mjs heightAxis: |RT - LT| < .05 -> 0.
inline f32 height_axis(f32 rt, f32 lt) { const f32 d = rt - lt; return std::fabs(d) < 0.05f ? 0.0f : clampf(d, -1, 1); }

// ---- drone (control_semantics.mjs) ----------------------------------------------------------
f32 phone_axis(f32 v, f32 fineness);
f32 game_state_stick_magnitude(f32 a);           // the FC's GAME stick shaping (deadband .035, expo .25)
f32 inverse_game_state_stick_magnitude(f32 v);  // bisection, 30 steps
struct DroneAxes { f32 roll = 0, pitch = 0, yaw = 0, body_pitch = 0; };
/// Left stick (x right, y down, unit disc): translation, pre-compensated for the FC.
void apply_game_stick_left(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, f32 max_kmh);
/// Right stick: yaw = phoneAxis(-x), bodyPitch = phoneAxis(-y) (invertRightVertical flips y).
void apply_game_stick_right(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, bool invert_v);
/// simulator.mjs altitude pad: axis = clamp((centre - pos) / (.40 h), -1, 1).
inline f32 clearance_pad_axis(f32 centre_y, f32 y, f32 height) { return clampf((centre_y - y) / std::fmax(1.0f, 0.40f * height), -1, 1); }

// ---- mouse ----------------------------------------------------------------------------------
/// desktop_controls.mjs rate stick: rate += (clamp(px / dt / full, +-1) - rate) (1 - exp(-dt / tau)).
struct RateStick {
  f32 full_px_s, tau;
  Vec2 rate;
  void step(Vec2 px, f32 dt);
  void reset() { rate = {}; }
};
/// The shared world head look (car mouse, drone RMB, minimap drag): degrees, yaw wrapped to +-180,
/// pitch -75..60. Idle for 1.4 s (desktop) or released (minimap): springs back at 8 /s unless kept.
struct WorldLook {
  f32 yaw_deg = 0, pitch_deg = 0;
  bool snapping = false;
  f32 idle = 0;
  bool owned = false;
  void add(f32 dx_px, f32 dy_px, f32 deg_per_px_x, f32 deg_per_px_y);  // the desktop owner
  void set(f32 yaw, f32 pitch);
  void release() { if (owned) snapping = true; owned = false; }
  void step(f32 dt, bool keep);
};
/// player_car_mode.mjs head look spring (stick / pad): targets 1.75 rad yaw, 0.7 rad pitch.
struct CarHeadSpring {
  f32 yaw = 0, pitch = 0;  // rad
  void step(f32 look_x, f32 look_y, f32 dt);
};

// ---- pad buttons ----------------------------------------------------------------------------
/// pad_actions.mjs VIEW: a tap (released before 400 ms) = camera, unless within 1.5 s after the map
/// opened by holding (world_map_full.mjs: hold > 0.42 s).
struct ViewButton {
  bool was = false, held_map = false;
  f32 down_at = 0, map_at = -99;
  /// Returns 1 for a camera tap, 2 when the hold opens the map, 0 otherwise.
  int step(bool down, f32 now);
};
/// voice_control_bindings.mjs nextVoicePadInput.
struct VoiceChord {
  bool r3 = false, y = false, latched = false;
  struct Out { bool mic = false, mute = false, suppress_weapon = false; };
  Out step(bool r3_now, bool y_now, bool allowed);
};
/// xbox_gamepad.mjs release latch: after a menu closes, the drone pad stays neutral until every
/// one of A/B/X/Y/RB/LT/RT/VIEW/MENU reads below .18.
struct ReleaseLatch {
  bool armed = false;
  /// values: the nine buttons' values. True while the pad must stay neutral.
  bool step(const f32* values, int n);
};

// ---- hand grenade (hand_grenade.mjs) --------------------------------------------------------
struct GrenadeCook {
  static constexpr f32 kFuse = 4.0f, kRegen = 20.0f, kDragPx = 110;
  static constexpr int kCarry = 3;
  int carry = kCarry;
  f32 regen_at = 0;          // 0: not regenerating
  bool cooking = false;
  f32 at = 0, power = 0.7f;  // pulled at (s), throw strength 0..1
  int source = 0;            // 0 keyboard, 1 pad, 2 touch
  bool pull(f32 now, int from);
  /// Drag "up" by d px in the game's frame: power = clamp(.7 + d / 110 * .6, 0, 1).
  void drag(f32 up_px) { power = clampf(0.7f + up_px / kDragPx * 0.6f, 0, 1); }
  /// Ends the cook (thrown): returns the fuse time left (s).
  f32 release(f32 now);
  /// Per frame: regeneration; true when the fuse burnt down in the hand (cooked off).
  bool step(f32 now);
  f32 left(f32 now) const { return cooking ? std::fmax(0.0f, kFuse - (now - at)) : 0.0f; }
  /// hand_grenade.mjs release(): speed 9 + 13 power.
  f32 speed() const { return 9 + 13 * clampf(power, 0, 1); }
  void reset() { *this = GrenadeCook{}; }
};

// ---- devices (desktop_controls.mjs setDesktop, pad_actions.mjs pad-input) --------------------
/// The last device used wins: desktop (mouse + keyboard) or touch, plus "pad-input" on top.
struct Devices {
  bool fine = true, coarse = false;
  i32 touch_points = 0;
  bool desktop = true, pad = false;
  u32 changes = 0;
  /// Start: desktop with a fine hovering primary pointer or without touch (desktop_controls.mjs:245).
  void platform(bool fine_pointer, bool coarse_pointer, i32 max_touch_points);
  void touch_down() { set(false); }
  /// A mouse button: desktop unless the primary pointer is coarse; pad-input off.
  void mouse_down() { pad = false; if (!coarse) set(true); }
  /// The mouse moved by itself (not locked, not over the UI).
  void mouse_move() { if (!coarse && fine) set(true); }
  /// A real key (any device, phones too); pad-input off.
  void key() { set(true); pad = false; }
  void pad_button() { pad = true; }
  void set(bool d) { if (d != desktop) { desktop = d; ++changes; } }
};

// ---- hint line (desktop_controls.mjs HINTS) -------------------------------------------------
/// The key help per mode ("foot", "drone", "car", "jet", "parachute"), "[ WEAPON ]" first on foot /
/// drone, the respawn line while dead.
std::string hint_line(const char* mode, const char* weapon, bool dead);

}  // namespace sc::controls
