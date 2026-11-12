// SpawnCity's input maths, exactly as spawncity.de computes it (see CONTROLS.md for the table of
// every device and mode). The rules are a Blueprint: the level's actor "Controls"
// (level/spawncity.klevel, open it in Kern Studio) - its variables hold the dead zones, curves and
// times, its functions the laws. What follows is only how the game's C++ calls them, and the state
// its modes keep between two frames; game_controls.cpp wires them to the modes.
//
//   fps_control_math.mjs            ShapeFpsStick, FpsDynamicCurve, LookAxisStep, TouchLookDelta,
//                                   FpsAimAssist
//   first_person_control_settings   ShapeFirstPersonMove, LookCurveStrength
//   player_walk_mode_v4.mjs         WalkPadAxis
//   xbox_gamepad.mjs                GamepadAxis, HeightAxis, ReleaseLatchStep
//   player_car_mode.mjs / jet_mode  HardCut, CarHeadStep
//   control_semantics.mjs           PhoneAxis, GameStickMagnitude (+ inverse), GameStickLeft / Right,
//                                   ClearancePadAxis
//   desktop_controls.mjs            RateStickStep, LookAdd / LookSet / LookRelease / LookStep,
//                                   DevicePlatform, DeviceInput, HintLine
//   pad_actions.mjs                 ViewButtonStep
//   voice_control_bindings.mjs      VoiceChordStep
//   hand_grenade.mjs                GrenadePull, GrenadeDrag, GrenadeRelease, GrenadeStep
#pragma once

#include <string>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::controls {

/// The running Blueprint of the actor "Controls" (the game binds it once its world has started;
/// tests bind the one they run). Unbound, every rule answers 0.
void bind(kern::logic::Instance* blueprint);

f32 smoothstep01(f32 t);

// ---- on foot -------------------------------------------------------------------------------
f32 walk_pad_axis(f32 v);
Vec2 shape_first_person_move(f32 x, f32 y, f32 move_fineness, bool invert_h);
f32 fps_dynamic_curve(f32 n);
f32 look_curve_strength(f32 look_fineness);
Vec2 shape_fps_stick(f32 x, f32 y, f32 inner, f32 curve_strength);

/// One axis of the look rate (rad/s).
struct LookAxis {
  f32 velocity = 0;
  /// Integrates towards `target` (rad/s) for dt; returns the angle turned (rad).
  f32 step(f32 target, f32 dt);
};

/// Out: yaw right-positive, pitch up-positive (rad).
void touch_look_delta(f32 dx, f32 dy, f32 speed_px_s, f32& yaw, f32& pitch);

/// Errors and inputs: yaw right-positive, pitch up-positive (rad, rad/s); s = percent / 100.
struct AimAssist { bool active = false; f32 slowdown = 1, yaw = 0, pitch = 0; };
AimAssist fps_aim_assist(f32 yaw_err, f32 pitch_err, f32 dist, f32 input, f32 in_yaw, f32 in_pitch, f32 s);
/// The candidate's place in the window (>= 1: outside).
f32 aim_ellipse(f32 yaw_err, f32 pitch_err);

// ---- gamepad dead zones --------------------------------------------------------------------
f32 gamepad_axis(f32 v, f32 dz = 0.14f);
f32 hard_cut(f32 v, f32 dz);
f32 height_axis(f32 rt, f32 lt);

// ---- drone ----------------------------------------------------------------------------------
f32 phone_axis(f32 v, f32 fineness);
f32 game_state_stick_magnitude(f32 a);
f32 inverse_game_state_stick_magnitude(f32 v);
struct DroneAxes { f32 roll = 0, pitch = 0, yaw = 0, body_pitch = 0; };
/// Left stick (x right, y down, unit disc): translation, pre-compensated for the FC.
void apply_game_stick_left(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, f32 max_kmh);
/// Right stick: yaw and body pitch.
void apply_game_stick_right(DroneAxes& out, f32 x, f32 y, f32 fineness, bool invert_h, bool invert_v);
f32 clearance_pad_axis(f32 centre_y, f32 y, f32 height);

// ---- mouse ----------------------------------------------------------------------------------
/// The mouse as a stick (drone 1500 px/s, 50 ms; jet 730 px/s, 25 ms).
struct RateStick {
  f32 full_px_s, tau;
  Vec2 rate;
  void step(Vec2 px, f32 dt);
  void reset() { rate = {}; }
};
/// The shared world head look (car mouse, drone RMB, minimap drag): the Blueprint's struct Look.
struct WorldLook {
  f32 yaw_deg = 0, pitch_deg = 0;
  bool snapping = false;
  f32 idle = 0;
  bool owned = false;
  void add(f32 dx_px, f32 dy_px, f32 deg_per_px_x, f32 deg_per_px_y);  // the desktop owner
  void set(f32 yaw, f32 pitch);
  void release();
  void step(f32 dt, bool keep);
};
/// The head look in the car (stick / pad), rad.
struct CarHeadSpring {
  f32 yaw = 0, pitch = 0;
  void step(f32 look_x, f32 look_y, f32 dt);
};

// ---- pad buttons ----------------------------------------------------------------------------
struct ViewButton {
  bool was = false, held_map = false;
  f32 down_at = 0, map_at = -99;
  /// Returns 1 for a camera tap, 2 when the hold opens the map, 0 otherwise.
  int step(bool down, f32 now);
};
struct VoiceChord {
  bool r3 = false, y = false, latched = false;
  struct Out { bool mic = false, mute = false, suppress_weapon = false; };
  Out step(bool r3_now, bool y_now, bool allowed);
};
struct ReleaseLatch {
  bool armed = false;
  /// values: the nine buttons' values. True while the pad must stay neutral.
  bool step(const f32* values, int n);
};

// ---- hand grenade: the Blueprint's struct Cook ------------------------------------------------
struct GrenadeCook {
  int carry = 3;
  f32 regen_at = 0;          // 0: not regenerating
  bool cooking = false;
  f32 at = 0, power = 0.7f;  // pulled at (s), throw strength 0..1
  int source = 0;            // 0 keyboard, 1 pad, 2 touch
  bool pull(f32 now, int from);
  /// Drag "up" by d px in the game's frame.
  void drag(f32 up_px);
  /// Ends the cook (thrown): returns the fuse time left (s).
  f32 release(f32 now);
  /// Per frame: regeneration; true when the fuse burnt down in the hand (cooked off).
  bool step(f32 now);
  f32 left(f32 now) const;
  f32 speed() const;
  void reset() { *this = GrenadeCook{}; }
};
/// The Blueprint's variable GrenadeFuse (s).
f32 grenade_fuse();

// ---- devices --------------------------------------------------------------------------------
/// The last device used wins: desktop (mouse + keyboard) or touch, plus "pad-input" on top. What
/// each event does is the Blueprint's function DeviceInput (its enum DeviceEvent, in this order).
struct Devices {
  enum class Event : u8 { touch_down, mouse_down, mouse_move, key, pad_button };
  bool fine = true, coarse = false;
  i32 touch_points = 0;
  bool desktop = true, pad = false;
  u32 changes = 0;
  void platform(bool fine_pointer, bool coarse_pointer, i32 max_touch_points);
  void touch_down() { input(Event::touch_down); }
  void mouse_down() { input(Event::mouse_down); }
  /// The mouse moved by itself (not locked, not over the UI).
  void mouse_move() { input(Event::mouse_move); }
  /// A real key (any device, phones too).
  void key() { input(Event::key); }
  void pad_button() { input(Event::pad_button); }
  void set(bool d) { if (d != desktop) { desktop = d; ++changes; } }
  void input(Event e);
};

// ---- hint line ------------------------------------------------------------------------------
/// The key help per mode ("foot", "drone", "car", "jet", "parachute").
std::string hint_line(const char* mode, const char* weapon, bool dead);

}  // namespace sc::controls
