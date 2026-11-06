// The controls: curves, dead zones, latches and the touch controls against spawncity.de's numbers
// (controls.h / touch_play.h, CONTROLS.md).
#include <string>

#include "../src/controls.h"
#include "../src/touch_play.h"
#include "test_util.h"

using namespace sc;
using namespace sc::controls;

namespace {
bool near(f32 a, f32 b, f32 eps = 1e-4f) { return std::fabs(a - b) <= eps; }
}  // namespace

SC_TEST(controls_dead_zones) {
  // player_walk_mode_v4.mjs axis(): .10, exponent 1.28
  SC_CHECK(walk_pad_axis(0.1f) == 0 && near(walk_pad_axis(1), 1) && near(walk_pad_axis(-1), -1), "walk ends");
  SC_CHECK(near(walk_pad_axis(0.55f), std::pow(0.5f, 1.28f)), "walk mid %g", walk_pad_axis(0.55f));
  // xbox_gamepad.mjs .14 linear, car .08, jet .12 hard cut, height axis .05
  SC_CHECK(gamepad_axis(0.14f) == 0 && near(gamepad_axis(0.57f), 0.5f) && near(gamepad_axis(-1), -1), "drone .14 linear");
  SC_CHECK(near(gamepad_axis(0.54f, 0.08f), 0.5f), "car .08 linear");
  SC_CHECK(hard_cut(0.11f, 0.12f) == 0 && hard_cut(0.13f, 0.12f) == 0.13f, "jet hard cut, no rescale");
  SC_CHECK(height_axis(0.52f, 0.5f) == 0 && near(height_axis(1, 0.25f), 0.75f), "height axis .05");
}

SC_TEST(controls_fps_stick) {
  // fps_control_math.mjs: the dynamic curve passes its knots; inner dead zone; anti dead zone .045
  SC_CHECK(near(fps_dynamic_curve(0.3f), 0.24f) && near(fps_dynamic_curve(0.7f), 0.5f) && near(fps_dynamic_curve(0.9f), 0.89f) && fps_dynamic_curve(1) == 1, "knots");
  f32 last = 0;
  bool mono = true;
  for (int i = 1; i <= 100; ++i) { const f32 v = fps_dynamic_curve(i / 100.0f); mono &= v >= last - 1e-6f; last = v; }
  SC_CHECK(mono, "monotone");
  SC_CHECK(shape_fps_stick(0.04f, 0, 0.05f, 0.27f).x == 0, "inside the dead zone");
  SC_CHECK(near(shape_fps_stick(1, 0, 0.05f, 0.27f).x, 1), "full");
  SC_CHECK(near(shape_fps_stick(0.0501f, 0, 0.05f, 0.27f).x, 0.045f, 2e-3f), "anti dead zone %g", shape_fps_stick(0.0501f, 0, 0.05f, 0.27f).x);
  // look fineness 6 -> k = .2667
  SC_CHECK(near(look_curve_strength(6), 0.48f * 5 / 9), "curve strength");
  // first_person_control_settings.mjs: fineness 1 is linear, invert flips x
  const Vec2 m = shape_first_person_move(0.5f, 0, 1, true);
  SC_CHECK(near(m.x, -0.5f) && near(shape_first_person_move(0.5f, 0, 10, false).x, 0.5f * 0.4f + 0.125f * 0.6f), "move fineness");
}

SC_TEST(controls_look_rates) {
  // integrateFpsLookVelocity: growing at 26 /s
  LookAxis a;
  const f32 dt = 1 / 60.0f;
  const f32 d = a.step(1, dt);
  SC_CHECK(near(d, dt - (1 - std::exp(-26 * dt)) / 26, 1e-6f), "first step %g", d);
  SC_CHECK(near(a.velocity, 1 - std::exp(-26 * dt), 1e-6f), "velocity");
  a.step(-1, dt);  // reversing: 40 /s
  SC_CHECK(a.velocity < 0, "reverses fast");
  // fpsTouchLookDelta: 0.0068 / 0.0059 rad per px; Kern: up to 1.6x for flicks (CoD Mobile acceleration)
  f32 y = 0, p = 0;
  touch_look_delta(10, 10, 100, y, p);
  SC_CHECK(near(y, 0.068f) && near(p, -0.059f), "slow drag 1:1");
  touch_look_delta(10, 0, 5000, y, p);
  SC_CHECK(near(y, 0.068f * 1.6f), "flick gain");
}

SC_TEST(controls_drone_sticks) {
  // control_semantics.mjs
  SC_CHECK(near(phone_axis(1, 10), 1) && near(phone_axis(0.5f, 1), 0.5f), "phone axis ends");
  const f32 e = 0.70f * 0.38f;
  SC_CHECK(near(phone_axis(0.5f, 10), 0.5f * (1 - e) + 0.125f * e), "phone axis expo");
  SC_CHECK(game_state_stick_magnitude(0.035f) == 0 && near(game_state_stick_magnitude(1), 1), "FC shaping");
  for (f32 v : {0.1f, 0.3f, 0.5f, 0.9f}) SC_CHECK(near(game_state_stick_magnitude(inverse_game_state_stick_magnitude(v)), v, 1e-5f), "inverse %g", v);
  DroneAxes a;
  apply_game_stick_left(a, 0, -1, 10, false, 90);
  SC_CHECK(near(a.pitch, 1) && a.roll == 0, "full forward = full pitch");
  apply_game_stick_left(a, 0, -1, 10, false, 45);
  SC_CHECK(near(game_state_stick_magnitude(a.pitch), 0.5f, 1e-4f), "45 km/h = half the FC's speed");
  apply_game_stick_left(a, 1, 0, 1, true, 90);
  SC_CHECK(near(a.roll, -1), "invert left horizontal");
  apply_game_stick_right(a, 1, 0, 1, false, true);
  SC_CHECK(near(a.yaw, -1) && a.body_pitch == 0, "right: yaw = -x");
  apply_game_stick_right(a, 0, 1, 1, false, true);
  SC_CHECK(near(a.body_pitch, 1), "body pitch (inverted by default) = +y");
  SC_CHECK(near(clearance_pad_axis(66, 66 - 0.4f * 132 / 2, 132), 0.5f), "altitude pad");
}

SC_TEST(controls_mouse_and_look) {
  // desktop_controls.mjs drone rate: 1500 px/s full, tau 50 ms
  RateStick r{1500, 0.05f, {}};
  for (int i = 0; i < 120; ++i) r.step({750.0f / 60, 0}, 1 / 60.0f);
  SC_CHECK(near(r.rate.x, 0.5f, 1e-3f), "rate %g", r.rate.x);
  WorldLook w;
  w.add(100, 100, 0.12f, 0.12f * 0.85f);
  SC_CHECK(near(w.yaw_deg, 12) && near(w.pitch_deg, -10.2f), "0.12 deg/px, pitch x .85");
  w.step(1.0f, false);
  SC_CHECK(near(w.yaw_deg, 12), "held for 1.4 s");
  w.step(0.5f, false);
  w.step(0.5f, false);
  SC_CHECK(w.yaw_deg < 12 * 0.05f, "springs back at 8 /s");
  CarHeadSpring c;
  for (int i = 0; i < 120; ++i) c.step(1, 0, 1 / 60.0f);
  SC_CHECK(near(c.yaw, 1.75f, 1e-3f), "car head 1.75 rad");
}

SC_TEST(controls_pad_buttons) {
  ViewButton v;
  SC_CHECK(v.step(true, 0) == 0 && v.step(false, 0.2f) == 1, "VIEW tap = camera on release");
  SC_CHECK(v.step(true, 1) == 0 && v.step(true, 1.5f) == 2 && v.step(false, 1.6f) == 0, "VIEW hold = map, no tap");
  SC_CHECK(v.step(true, 2) == 0 && v.step(false, 2.1f) == 0, "no tap within 1.5 s after the map");
  VoiceChord c;
  SC_CHECK(!c.step(true, false, true).mic && c.step(false, false, true).mic, "R3 tap: mic on release");
  c.step(true, false, true);
  SC_CHECK(c.step(true, true, true).mute, "R3 + Y mutes");
  SC_CHECK(!c.step(false, false, true).mic, "and the release is not a mic toggle");
  ReleaseLatch l;
  l.armed = true;
  const f32 held[9] = {1, 0, 0, 0, 0, 0, 0, 0, 0}, free[9] = {0, 0, 0, 0, 0, 0.1f, 0, 0, 0};
  SC_CHECK(l.step(held, 9) && !l.step(free, 9) && !l.armed, "latch until below .18");
}

SC_TEST(controls_grenade_cook) {
  GrenadeCook g;
  SC_CHECK(g.pull(0, 0) && g.carry == 2 && !g.pull(0.1f, 0), "pull once");
  g.drag(110);
  SC_CHECK(near(g.power, 1) && near(g.speed(), 22), "drag 110 px = full power");
  SC_CHECK(near(g.release(1.5f), 2.5f), "4 s fuse, cooked 1.5 s");
  g.pull(2, 0);
  SC_CHECK(!g.step(5.9f) && g.step(6.0f) && !g.cooking, "cooks off at 4 s");
  g.pull(7, 0);
  g.release(7);
  SC_CHECK(g.carry == 0 && !g.pull(8, 0), "3 carried");
  g.step(20.0f);
  SC_CHECK(g.carry == 1, "one back after 20 s (%d)", g.carry);
}

SC_TEST(controls_devices) {
  Devices d;
  d.platform(false, true, 5);
  SC_CHECK(!d.desktop, "a phone starts with touch");
  d.mouse_down();
  SC_CHECK(!d.desktop, "a mouse does not switch a coarse device");
  d.key();
  SC_CHECK(d.desktop, "a key does");
  d.touch_down();
  d.pad_button();
  SC_CHECK(!d.desktop && d.pad, "touch, then pad-input");
  d.key();
  SC_CHECK(!d.pad, "a key clears pad-input");
  Devices pc;
  pc.platform(false, false, 0);
  SC_CHECK(pc.desktop, "no touch: desktop");
  SC_CHECK(hint_line("foot", "SMG", false).rfind("[ SMG ]  WASD move", 0) == 0 && hint_line("car", "SMG", false).rfind("W / S", 0) == 0, "hint lines");
  SC_CHECK(hint_line("jet", "", true) == "DOWN · SPACE / ENTER / R = RESPAWN", "dead line");
}

SC_TEST(controls_touch_foot) {
  TouchPlay t;
  t.set_frame(844, 390, 0, 0, 0, 0);
  TouchLayout l;
  l.mode = TouchMode::foot;
  l.weapon = "Glock";
  l.grenade = true;
  l.grenades = 3;
  t.set_layout(l);
  auto has = [&](const char* id) { for (const TouchBtn& b : t.buttons()) if (b.id == id) return true; return false; };
  SC_CHECK(has("jump") && has("wpn") && has("mode") && has("strike") && has("nade") && !has("scope") && !has("emp"), "foot buttons");
  // the move stick: deflection from where the thumb lands, radius .42 * 168
  const f32 cx = 84, cy = 390 - 84;
  t.pointer(0, 1, cx + 20, cy, 0);
  SC_CHECK(t.move().x == 0, "landing does not jump");
  t.pointer(1, 1, cx + 20 + 0.42f * 168 * 0.5f, cy, 16);
  SC_CHECK(near(t.move().x, 0.5f), "half deflection %g", t.move().x);
  t.pointer(1, 1, cx + 20, cy - 200, 32);
  SC_CHECK(t.sprint(), "rim forwards sprints");
  t.pointer(2, 1, cx, cy - 200, 40);
  SC_CHECK(t.move().x == 0 && t.move().y == 0, "released");
  // the screen fires at the finger, dragging moves the aim point (no look)
  t.pointer(0, 2, 500, 150, 50);
  SC_CHECK(t.fires().size() == 1 && t.fires()[0].hand == 0, "screen fire, right half");
  t.pointer(1, 2, 510, 160, 66);
  SC_CHECK(t.fires()[0].x == 510 && t.take_drag().x == 0, "fire drag aims, does not look");
  t.pointer(2, 2, 430, 160, 80);
  // the look stick: 1:1 drag
  const f32 rx = 844 - 84;
  t.pointer(0, 3, rx, cy, 100);
  t.pointer(1, 3, rx + 10, cy, 200);
  SC_CHECK(near(t.take_drag().x, 0.068f), "look drag %g", 0.068f);
  t.pointer(1, 3, rx + 200, cy, 300);
  SC_CHECK(t.look_edge().x > 0.99f, "edge turn past the ring");
  t.pointer(2, 3, rx + 200, cy, 310);
  // JUMP is held
  for (const TouchBtn& b : t.buttons())
    if (b.id == "jump") t.pointer(0, 4, b.x + b.w / 2, b.y + b.h / 2, 400);
  SC_CHECK(t.held("jump") && t.take_pressed("jump"), "jump held and pressed");
  t.pointer(2, 4, 0, 0, 450);
  SC_CHECK(!t.held("jump"), "jump released");
}

SC_TEST(controls_touch_vehicles) {
  TouchPlay t;
  t.set_frame(844, 390, 0, 0, 0, 0);
  TouchLayout l;
  l.mode = TouchMode::drone;
  l.weapon = "MG";
  t.set_layout(l);
  // altitude pad: up from its centre
  const TouchBtn* pad = nullptr;
  for (const TouchBtn& b : t.buttons())
    if (b.id == "clearance") pad = &b;
  SC_CHECK(pad && near(pad->w, 48) && near(pad->h, 132), "altitude pad 48 x 132");
  if (pad) {
    const f32 x = pad->x + 24, c = pad->y + 66;
    t.pointer(0, 1, x, c, 0);
    t.pointer(1, 1, x, c - 0.4f * 132, 10);
    SC_CHECK(near(t.clearance(), 1), "pad full up %g", t.clearance());
    t.pointer(2, 1, x, c, 20);
    SC_CHECK(t.clearance() == 0, "pad springs back");
  }
  // jet: EJECT on a tap (the ejection seat as you press it)
  l.mode = TouchMode::jet;
  t.set_layout(l);
  const TouchBtn* ej = nullptr;
  for (const TouchBtn& b : t.buttons())
    if (b.id == "eject") ej = &b;
  SC_CHECK(ej != nullptr, "eject button");
  if (ej) {
    const f32 x = ej->x + 10, y = ej->y + 10;
    t.pointer(0, 2, x, y, 1000);
    t.pointer(2, 2, x, y, 1150);
    SC_CHECK(t.take_pressed("eject"), "a tap ejects");
  }
  // car: two free fingers pinch the chase camera
  l.mode = TouchMode::car;
  t.set_layout(l);
  t.pointer(0, 5, 400, 100, 0);
  t.pointer(0, 6, 500, 100, 0);
  t.pointer(1, 6, 600, 100, 10);
  SC_CHECK(near(t.take_zoom(), 0.5f), "pinch out zooms in");
}

SC_TEST(controls_aim_assist) {
  // fpsAimAssist: inactive outside the window, beyond 90 m or without input; a pull towards the
  // target, capped at .38 s rad/s, and a slowdown of up to .30 s on top of it
  SC_CHECK(!fps_aim_assist(0.2f, 0, 20, 1, 1, 0, 0.55f).active, "outside the 7 deg window");
  SC_CHECK(!fps_aim_assist(0.01f, 0, 95, 1, 1, 0, 0.55f).active && !fps_aim_assist(0.01f, 0, 20, 0.01f, 1, 0, 0.55f).active, "too far / no input");
  const AimAssist a = fps_aim_assist(0.05f, 0, 8, 1, 1, 0, 1);
  SC_CHECK(a.active && a.yaw > 0 && a.yaw <= 0.38f + 1e-6f && a.slowdown < 1 && a.slowdown >= 0.7f, "pull %g, slowdown %g", a.yaw, a.slowdown);
  SC_CHECK(near(fps_aim_assist(0, 0, 8, 1, 1, 0, 1).slowdown, 0.7f), "on the target: slowdown 1 - .30");
  const AimAssist away = fps_aim_assist(0.05f, 0, 8, 1, -1, 0, 1);
  SC_CHECK(away.yaw < a.yaw, "moving away tracks less (%g < %g)", away.yaw, a.yaw);
}
