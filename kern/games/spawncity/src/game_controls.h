// State of the player's controls (game_controls.cpp): devices, pointer lock, the mouse stream,
// pad edges and latches, look integrators, the hand grenade cook, air-strike aiming, the head look.
// Owned by Game as `ctl_` (see CONTROLS.md for the table of every device and mode).
#pragma once

#include <string>

#include "controls.h"

namespace sc {

struct ControlsState {
  controls::Devices dev;
  bool near_car = false, near_jet = false;  // on foot within reach (the page's EINSTEIGEN buttons, web/view.mjs)
  // mouse + keyboard (desktop_controls.mjs)
  bool locked = false;          // the page reports document.pointerLockElement
  bool swallow_lmb = false;     // the click that captured the mouse never fires
  Vec2 mouse;                   // movementX/Y under the lock (CSS px, clamped +-420 per event)
  // the pad: own edges (the page polls at its own rate)
  u32 pad_prev = 0;
  bool pad_rt = false, pad_lt = false, pad_lb = false;  // trigger / shoulder latches
  controls::ViewButton view;
  controls::VoiceChord voice;
  controls::ReleaseLatch latch;
  bool was_paused = false;
  // on foot look (fps_control_math.mjs) and fire
  controls::LookAxis pad_yaw, pad_pitch, touch_yaw, touch_pitch;
  bool pad_fire_was = false, pad_lt_fire_was = false;
  // drone / jet mouse rate sticks (desktop_controls.mjs, jet_mode.mjs)
  controls::RateStick drone_mouse{1500, 0.05f, {}};
  controls::RateStick jet_mouse{730, 0.025f, {}};
  // the shared head look (car mouse, drone RMB / AIM scheme, minimap drag) and the car's stick spring
  controls::WorldLook look;
  controls::CarHeadSpring car_head;
  f32 car_zoom = 1;             // chase camera zoom (0.55 .. 2.6)
  bool aim_scheme = false;      // xbox_control_mode.mjs AIM (LB = head look in the drone)
  bool pad_drone = false;       // XBOX CONTROLLER switch: on with the first pad press
  bool keep_look = false;       // WORLD: KEEP 360 LOOK
  // hand grenade (hand_grenade.mjs)
  controls::GrenadeCook nade;
  // air strike aiming (fighter_jets.mjs)
  bool aiming = false;
  f32 aim_lock_until = 0;       // lockInput: weapons rest while aiming and just after
  // jet weapons (jet_mode.mjs): RAKETE selected, mouse / pad latches
  bool rocket = false, rocket_mouse_latch = false, rocket_suppress = false;
  // a ray through a screen point for the game's own weapons (touch fire at the finger)
  bool aim_point = false;
  f32 aim_x = 0, aim_y = 0;     // CSS px
  bool jet_aim = false;         // jet tap fire: aim through the finger within 35 deg of the nose
  // touch fire repeat (foot_touch_controller.mjs) and the drone's STOP
  f32 kill_until = -1;
  // web/view_modes.mjs (the original's HUD modules): counters and timers the page cannot see
  f32 emp_miss_until = -1;      // wanted_police_drones.mjs empFeedbackUntil (sim time)
  f32 guided_denied_until = -1; // guided_missile.mjs body.guided-denied (900 ms)
  u32 drone_hits = 0;           // flight_fire_fx.mjs hit-confirm on the crosshair
  bool assist_on = false;       // player_walk_mode_v4.mjs #footReticle.aim-assisted
  Vec3 impacts[8];              // flight_fire_fx.mjs screenImpact: where the drone's rounds struck
  u32 impact_serial = 0;
  // hint line (desktop_controls.mjs): text and when it last changed
  std::string hint;
  f32 hint_at = 0;
  f32 clock = 0;                // seconds since start (page time)
  // this frame (read_input)
  u32 pad_now = 0, pad_hit = 0;
  bool lmb = false, rmb = false, lmb_hit = false, rmb_hit = false, mmb_hit = false;
  Vec2 md;                      // mouse this frame, sensitivity and invert Y applied
  f32 wheel = 0;
  bool scope_on = false;        // sniper SCOPE toggle (touch)
  bool pad_a_was = false;
  i32 drone_cam_cycles = 0;
};

}  // namespace sc
