// The controls: curves, dead zones, latches and the touch controls against spawncity.de's numbers
// (CONTROLS.md). They are the level's Blueprints controls_cpp and touch_play_cpp, called as their classes - as
// the program calls them (level_blueprints.h) - beside the level's Controls Blueprint they ask.
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "../src/blueprint_call.h"
#include "level_blueprints.h"
#include "rule_blueprint.h"
#include "test_util.h"

using namespace sc;

namespace {
using Ct = blueprints::controls_cpp;
using Tp = blueprints::touch_play_cpp;
bool near(f64 a, f64 b, f64 eps = 1e-4f) { return std::fabs(a - b) <= eps; }
/// A Blueprint of the controls running as its class, beside the level's Controls Blueprint.
template <class T>
struct ControlsBlueprint {
  RuleBlueprint rules;
  bool bound = false;
  explicit ControlsBlueprint(const char* actor) : rules("Controls", nullptr, true, {actor}) {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<T>(rules.rt);
  }
  T& operator*() { return kern::logic::blueprint<T>(); }
};
std::string str(const kern::logic::Text& t) { return std::string(t.view()); }
}  // namespace

#define CONTROLS_BOUND(T, actor)                                                                  \
  ControlsBlueprint<T> b(actor);                                                                  \
  RuleBlueprint& bp = b.rules;                                                                    \
  (void)bp;                                                                                       \
  SC_CHECK(b.bound, actor " runs as its class: %s", b.rules.problems.c_str());                    \
  if (!b.bound) return;                                                                           \
  T& k = *b

SC_TEST(controls_dead_zones) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  SC_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Controls Blueprint compiles and runs: %s", bp.problems.c_str());
  SC_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
  SC_CHECK(k.bind().Result, "controls_cpp finds the Controls Blueprint");
  auto walk_pad_axis = [&](f32 v) { return static_cast<f64>(k.walk_pad_axis(v).Result); };
  auto gamepad_axis = [&](f32 v, f32 dz = 0.14f) { return static_cast<f64>(k.gamepad_axis(v, dz).Result); };
  // player_walk_mode_v4.mjs axis(): .10, exponent 1.28
  SC_CHECK(walk_pad_axis(0.1f) == 0 && near(walk_pad_axis(1), 1) && near(walk_pad_axis(-1), -1), "walk ends");
  SC_CHECK(near(walk_pad_axis(0.55f), std::pow(0.5f, 1.28f)), "walk mid %g", walk_pad_axis(0.55f));
  // xbox_gamepad.mjs .14 linear, car .08, jet .12 hard cut, height axis .05
  SC_CHECK(gamepad_axis(0.14f) == 0 && near(gamepad_axis(0.57f), 0.5f) && near(gamepad_axis(-1), -1), "drone .14 linear");
  SC_CHECK(near(gamepad_axis(0.54f, 0.08f), 0.5f), "car .08 linear");
  SC_CHECK(k.hard_cut(0.11f, 0.12f).Result == 0 && k.hard_cut(0.13f, 0.12f).Result == 0.13f, "jet hard cut, no rescale");
  SC_CHECK(k.height_axis(0.52f, 0.5f).Result == 0 && near(k.height_axis(1, 0.25f).Result, 0.75f), "height axis .05");
}

SC_TEST(controls_fps_stick) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  auto fps_dynamic_curve = [&](f32 n) { return static_cast<f64>(k.fps_dynamic_curve(n).Result); };
  auto shape_fps_stick = [&](f32 x, f32 y, f32 inner, f32 s) { return k.shape_fps_stick(x, y, inner, s).Result; };
  // fps_control_math.mjs: the dynamic curve passes its knots; inner dead zone; anti dead zone .045
  SC_CHECK(near(fps_dynamic_curve(0.3f), 0.24f) && near(fps_dynamic_curve(0.7f), 0.5f) && near(fps_dynamic_curve(0.9f), 0.89f) && fps_dynamic_curve(1) == 1, "knots");
  f64 last = 0;
  bool mono = true;
  for (int i = 1; i <= 100; ++i) { const f64 v = fps_dynamic_curve(i / 100.0f); mono &= v >= last - 1e-6f; last = v; }
  SC_CHECK(mono, "monotone");
  SC_CHECK(shape_fps_stick(0.04f, 0, 0.05f, 0.27f).x == 0, "inside the dead zone");
  SC_CHECK(near(shape_fps_stick(1, 0, 0.05f, 0.27f).x, 1), "full");
  SC_CHECK(near(shape_fps_stick(0.0501f, 0, 0.05f, 0.27f).x, 0.045f, 2e-3f), "anti dead zone %g", static_cast<f64>(shape_fps_stick(0.0501f, 0, 0.05f, 0.27f).x));
  // look fineness 6 -> k = .2667
  SC_CHECK(near(k.look_curve_strength(6).Result, 0.48f * 5 / 9), "curve strength");
  // first_person_control_settings.mjs: fineness 1 is linear, invert flips x
  const Vec3 m = k.shape_first_person_move(0.5f, 0, 1, true).Result;
  SC_CHECK(near(m.x, -0.5f) && near(k.shape_first_person_move(0.5f, 0, 10, false).Result.x, 0.5f * 0.4f + 0.125f * 0.6f), "move fineness");
}

SC_TEST(controls_look_rates) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  // integrateFpsLookVelocity: growing at 26 /s
  Ct::controls_LookAxis a;
  const f32 dt = 1 / 60.0f;
  const f64 d = k.LookAxis_step(a, 1, dt).Result;
  SC_CHECK(near(d, dt - (1 - std::exp(-26 * dt)) / 26, 1e-6f), "first step %g", d);
  SC_CHECK(near(a.velocity, 1 - std::exp(-26 * dt), 1e-6f), "velocity");
  k.LookAxis_step(a, -1, dt);  // reversing: 40 /s
  SC_CHECK(a.velocity < 0, "reverses fast");
  // fpsTouchLookDelta: 0.0068 / 0.0059 rad per px; Kern: up to 1.6x for flicks (CoD Mobile acceleration)
  f64 y = 0, p = 0;
  k.touch_look_delta(y, p, 10, 10, 100);
  SC_CHECK(near(y, 0.068f) && near(p, -0.059f), "slow drag 1:1");
  k.touch_look_delta(y, p, 10, 0, 5000);
  SC_CHECK(near(y, 0.068f * 1.6f), "flick gain");
}

SC_TEST(controls_drone_sticks) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  auto phone_axis = [&](f32 v, f32 f) { return static_cast<f64>(k.phone_axis(v, f).Result); };
  auto magnitude = [&](f64 a) { return static_cast<f64>(k.game_state_stick_magnitude(a).Result); };
  // control_semantics.mjs
  SC_CHECK(near(phone_axis(1, 10), 1) && near(phone_axis(0.5f, 1), 0.5f), "phone axis ends");
  const f32 e = 0.70f * 0.38f;
  SC_CHECK(near(phone_axis(0.5f, 10), 0.5f * (1 - e) + 0.125f * e), "phone axis expo");
  SC_CHECK(magnitude(0.035f) == 0 && near(magnitude(1), 1), "FC shaping");
  for (f32 v : {0.1f, 0.3f, 0.5f, 0.9f}) SC_CHECK(near(magnitude(k.inverse_game_state_stick_magnitude(v).Result), v, 1e-5f), "inverse %g", static_cast<f64>(v));
  Ct::controls_DroneAxes a;
  k.apply_game_stick_left(a, 0, -1, 10, false, 90);
  SC_CHECK(near(a.pitch, 1) && a.roll == 0, "full forward = full pitch");
  k.apply_game_stick_left(a, 0, -1, 10, false, 45);
  SC_CHECK(near(magnitude(a.pitch), 0.5f, 1e-4f), "45 km/h = half the FC's speed");
  k.apply_game_stick_left(a, 1, 0, 1, true, 90);
  SC_CHECK(near(a.roll, -1), "invert left horizontal");
  k.apply_game_stick_right(a, 1, 0, 1, false, true);
  SC_CHECK(near(a.yaw, -1) && a.body_pitch == 0, "right: yaw = -x");
  k.apply_game_stick_right(a, 0, 1, 1, false, true);
  SC_CHECK(near(a.body_pitch, 1), "body pitch (inverted by default) = +y");
  SC_CHECK(near(k.clearance_pad_axis(66, 66 - 0.4f * 132 / 2, 132).Result, 0.5f), "altitude pad");
}

SC_TEST(controls_mouse_and_look) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  // desktop_controls.mjs drone rate: 1500 px/s full, tau 50 ms
  Ct::controls_RateStick r;
  r.full_px_s = 1500;
  r.tau = 0.05f;
  r.rate = {0, 0, 0};
  for (int i = 0; i < 120; ++i) k.RateStick_step(r, {750.0f / 60, 0, 0}, 1 / 60.0f);
  SC_CHECK(near(r.rate.x, 0.5f, 1e-3f), "rate %g", static_cast<f64>(r.rate.x));
  Ct::controls_WorldLook w;
  k.WorldLook_add(w, 100, 100, 0.12f, 0.12f * 0.85f);
  SC_CHECK(near(w.yaw_deg, 12) && near(w.pitch_deg, -10.2f), "0.12 deg/px, pitch x .85");
  k.WorldLook_step(w, 1.0f, false);
  SC_CHECK(near(w.yaw_deg, 12), "held for 1.4 s");
  k.WorldLook_step(w, 0.5f, false);
  k.WorldLook_step(w, 0.5f, false);
  SC_CHECK(w.yaw_deg < 12 * 0.05f, "springs back at 8 /s");
  Ct::controls_CarHeadSpring c;
  for (int i = 0; i < 120; ++i) k.CarHeadSpring_step(c, 1, 0, 1 / 60.0f);
  SC_CHECK(near(c.yaw, 1.75f, 1e-3f), "car head 1.75 rad");
}

SC_TEST(controls_pad_buttons) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  Ct::controls_ViewButton v;
  auto view = [&](bool down, f32 now) { return static_cast<int>(k.ViewButton_step(v, down, now).Result); };
  SC_CHECK(view(true, 0) == 0 && view(false, 0.2f) == 1, "VIEW tap = camera on release");
  SC_CHECK(view(true, 1) == 0 && view(true, 1.5f) == 2 && view(false, 1.6f) == 0, "VIEW hold = map, no tap");
  SC_CHECK(view(true, 2) == 0 && view(false, 2.1f) == 0, "no tap within 1.5 s after the map");
  Ct::controls_VoiceChord c;
  auto chord = [&](bool r3, bool y, bool allowed) { return k.VoiceChord_step(c, r3, y, allowed).Result; };
  SC_CHECK(!chord(true, false, true).mic && chord(false, false, true).mic, "R3 tap: mic on release");
  chord(true, false, true);
  SC_CHECK(chord(true, true, true).mute, "R3 + Y mutes");
  SC_CHECK(!chord(false, false, true).mic, "and the release is not a mic toggle");
  Ct::controls_ReleaseLatch l;
  l.armed = true;
  const Ct::Fixed<Ct::Float, 9> held{1, 0, 0, 0, 0, 0, 0, 0, 0}, free{0, 0, 0, 0, 0, 0.1f, 0, 0, 0};
  SC_CHECK(k.ReleaseLatch_step(l, held).Result && !k.ReleaseLatch_step(l, free).Result && !l.armed, "latch until below .18");
}

SC_TEST(controls_grenade_cook) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  Ct::controls_GrenadeCook g;
  auto pull = [&](f32 now, int from) { return static_cast<bool>(k.GrenadeCook_pull(g, now, from).Result); };
  auto step = [&](f32 now) { return static_cast<bool>(k.GrenadeCook_step(g, now).Result); };
  SC_CHECK(pull(0, 0) && g.carry == 2 && !pull(0.1f, 0), "pull once");
  k.GrenadeCook_drag(g, 110);
  SC_CHECK(near(g.power, 1) && near(k.GrenadeCook_speed(g).Result, 22), "drag 110 px = full power");
  SC_CHECK(near(k.GrenadeCook_release(g, 1.5f).Result, 2.5f), "4 s fuse, cooked 1.5 s");
  pull(2, 0);
  SC_CHECK(!step(5.9f) && step(6.0f) && !g.cooking, "cooks off at 4 s");
  pull(7, 0);
  k.GrenadeCook_release(g, 7);
  SC_CHECK(g.carry == 0 && !pull(8, 0), "3 carried");
  step(20.0f);
  SC_CHECK(g.carry == 1, "one back after 20 s (%d)", static_cast<int>(g.carry));
}

SC_TEST(controls_devices) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  Ct::controls_Devices d;
  k.Devices_platform(d, false, true, 5);
  SC_CHECK(!d.desktop, "a phone starts with touch");
  k.Devices_mouse_down(d);
  SC_CHECK(!d.desktop, "a mouse does not switch a coarse device");
  k.Devices_key(d);
  SC_CHECK(d.desktop, "a key does");
  k.Devices_touch_down(d);
  k.Devices_pad_button(d);
  SC_CHECK(!d.desktop && d.pad, "touch, then pad-input");
  k.Devices_key(d);
  SC_CHECK(!d.pad, "a key clears pad-input");
  Ct::controls_Devices pc;
  k.Devices_platform(pc, false, false, 0);
  SC_CHECK(pc.desktop, "no touch: desktop");
  auto hint_line = [&](const char* mode, const char* weapon, bool dead) { return str(k.hint_line(std::string(mode), std::string(weapon), dead).Result); };
  SC_CHECK(hint_line("foot", "SMG", false).rfind("[ SMG ]  WASD move", 0) == 0 && hint_line("car", "SMG", false).rfind("W / S", 0) == 0, "hint lines");
  SC_CHECK(hint_line("jet", "", true) == "DOWN · SPACE / ENTER / R = RESPAWN", "dead line");
}

namespace {
/// The touch controls' state, called as touch_play_cpp's class (the C++'s accessors read its fields).
struct Touch {
  Tp& k;
  Tp::TouchPlay t;
  bool pointer(u32 type, i32 id, f32 x, f32 y, f64 t_ms) { return k.TouchPlay_pointer(t, type, id, x, y, t_ms).Result; }
  const Tp::TouchBtn* button(const char* id) const {
    for (const Tp::TouchBtn& b : t.buttons_)
      if (str(b.id) == id) return &b;
    return nullptr;
  }
};
}  // namespace

SC_TEST(controls_touch_foot) {
  CONTROLS_BOUND(Tp, "touch_play_cpp");
  Touch t{k, {}};
  k.TouchPlay_set_frame(t.t, 844, 390, 0, 0, 0, 0);
  Tp::TouchLayout l;
  l.mode = Tp::TouchMode::foot;
  l.weapon = "Glock";
  l.grenade = true;
  l.grenades = 3;
  k.TouchPlay_set_layout(t.t, l);
  auto has = [&](const char* id) { return t.button(id) != nullptr; };
  auto move = [&] { return k.TouchPlay_move(t.t).Result; };
  SC_CHECK(has("jump") && has("wpn") && has("mode") && has("strike") && has("nade") && !has("scope") && !has("emp"), "foot buttons");
  // the move stick: deflection from where the thumb lands, radius .42 * 168
  const f32 cx = 84, cy = 390 - 84;
  t.pointer(0, 1, cx + 20, cy, 0);
  SC_CHECK(move().x == 0, "landing does not jump");
  t.pointer(1, 1, cx + 20 + 0.42f * 168 * 0.5f, cy, 16);
  SC_CHECK(near(move().x, 0.5f), "half deflection %g", static_cast<f64>(move().x));
  t.pointer(1, 1, cx + 20, cy - 200, 32);
  SC_CHECK(k.TouchPlay_sprint(t.t).Result, "rim forwards sprints");
  t.pointer(2, 1, cx, cy - 200, 40);
  SC_CHECK(move().x == 0 && move().y == 0, "released");
  // the screen fires at the finger, dragging moves the aim point (no look)
  t.pointer(0, 2, 500, 150, 50);
  SC_CHECK(t.t.fires_.size() == 1 && t.t.fires_[0].hand == 0, "screen fire, right half");
  t.pointer(1, 2, 510, 160, 66);
  SC_CHECK(t.t.fires_[0].x == 510 && k.TouchPlay_take_drag(t.t).Result.x == 0, "fire drag aims, does not look");
  t.pointer(2, 2, 430, 160, 80);
  // the look stick: 1:1 drag
  const f32 rx = 844 - 84;
  t.pointer(0, 3, rx, cy, 100);
  t.pointer(1, 3, rx + 10, cy, 200);
  SC_CHECK(near(k.TouchPlay_take_drag(t.t).Result.x, 0.068f), "look drag %g", 0.068f);
  t.pointer(1, 3, rx + 200, cy, 300);
  SC_CHECK(k.TouchPlay_look_edge(t.t).Result.x > 0.99f, "edge turn past the ring");
  t.pointer(2, 3, rx + 200, cy, 310);
  // JUMP is held
  if (const Tp::TouchBtn* b = t.button("jump")) {
    const f32 x = static_cast<f32>(b->x + b->w / 2), y = static_cast<f32>(b->y + b->h / 2);
    t.pointer(0, 4, x, y, 400);
  }
  SC_CHECK(k.TouchPlay_held(t.t, "jump").Result && k.TouchPlay_take_pressed(t.t, "jump").Result, "jump held and pressed");
  t.pointer(2, 4, 0, 0, 450);
  SC_CHECK(!k.TouchPlay_held(t.t, "jump").Result, "jump released");
}

SC_TEST(controls_touch_vehicles) {
  CONTROLS_BOUND(Tp, "touch_play_cpp");
  Touch t{k, {}};
  k.TouchPlay_set_frame(t.t, 844, 390, 0, 0, 0, 0);
  Tp::TouchLayout l;
  l.mode = Tp::TouchMode::drone;
  l.weapon = "MG";
  k.TouchPlay_set_layout(t.t, l);
  // altitude pad: up from its centre
  const Tp::TouchBtn* pad = t.button("clearance");
  SC_CHECK(pad && near(pad->w, 48) && near(pad->h, 132), "altitude pad 48 x 132");
  if (pad) {
    const f32 x = static_cast<f32>(pad->x + 24), c = static_cast<f32>(pad->y + 66);
    t.pointer(0, 1, x, c, 0);
    t.pointer(1, 1, x, c - 0.4f * 132, 10);
    const f64 up = k.TouchPlay_clearance(t.t).Result;
    SC_CHECK(near(up, 1), "pad full up %g", up);
    t.pointer(2, 1, x, c, 20);
    SC_CHECK(k.TouchPlay_clearance(t.t).Result == 0, "pad springs back");
  }
  // jet: EJECT on a tap (the ejection seat as you press it)
  l.mode = Tp::TouchMode::jet;
  k.TouchPlay_set_layout(t.t, l);
  const Tp::TouchBtn* ej = t.button("eject");
  SC_CHECK(ej != nullptr, "eject button");
  if (ej) {
    const f32 x = static_cast<f32>(ej->x + 10), y = static_cast<f32>(ej->y + 10);
    t.pointer(0, 2, x, y, 1000);
    t.pointer(2, 2, x, y, 1150);
    SC_CHECK(k.TouchPlay_take_pressed(t.t, "eject").Result, "a tap ejects");
  }
  // car: two free fingers pinch the chase camera
  l.mode = Tp::TouchMode::car;
  k.TouchPlay_set_layout(t.t, l);
  t.pointer(0, 5, 400, 100, 0);
  t.pointer(0, 6, 500, 100, 0);
  t.pointer(1, 6, 600, 100, 10);
  SC_CHECK(near(k.TouchPlay_take_zoom(t.t).Result, 0.5f), "pinch out zooms in");
}

SC_TEST(controls_aim_assist) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  auto fps_aim_assist = [&](f32 yaw, f32 pitch, f32 dist, f32 input, f32 in_yaw, f32 in_pitch, f32 s) {
    return k.fps_aim_assist(yaw, pitch, dist, input, in_yaw, in_pitch, s).Result;
  };
  // fpsAimAssist: inactive outside the window, beyond 90 m or without input; a pull towards the
  // target, capped at .38 s rad/s, and a slowdown of up to .30 s on top of it
  SC_CHECK(!fps_aim_assist(0.2f, 0, 20, 1, 1, 0, 0.55f).active, "outside the 7 deg window");
  SC_CHECK(!fps_aim_assist(0.01f, 0, 95, 1, 1, 0, 0.55f).active && !fps_aim_assist(0.01f, 0, 20, 0.01f, 1, 0, 0.55f).active, "too far / no input");
  const Ct::controls_AimAssist a = fps_aim_assist(0.05f, 0, 8, 1, 1, 0, 1);
  SC_CHECK(a.active && a.yaw > 0 && a.yaw <= 0.38f + 1e-6f && a.slowdown < 1 && a.slowdown >= 0.7f, "pull %g, slowdown %g", static_cast<f64>(a.yaw), static_cast<f64>(a.slowdown));
  SC_CHECK(near(fps_aim_assist(0, 0, 8, 1, 1, 0, 1).slowdown, 0.7f), "on the target: slowdown 1 - .30");
  const Ct::controls_AimAssist away = fps_aim_assist(0.05f, 0, 8, 1, -1, 0, 1);
  SC_CHECK(away.yaw < a.yaw, "moving away tracks less (%g < %g)", static_cast<f64>(away.yaw), static_cast<f64>(a.yaw));
}

// What asking the Blueprint costs (printed; `SC_TEST_RELEASE=1 run_tests.sh controls_call_cost` for
// the times of a build like the game's): a single formula, a function with a loop and a second
// function inside, and one that takes and returns a struct.
SC_TEST(controls_call_cost) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  auto ns = [](auto&& call) {
    const int rounds = 20000;
    f32 sum = 0;
    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < rounds; ++i) sum += call(static_cast<f32>(i % 100) / 100);
    const f64 each = std::chrono::duration<f64, std::nano>(std::chrono::steady_clock::now() - t0).count() / rounds;
    return std::pair<f64, f32>(each, sum);
  };
  const auto axis = ns([&](f32 x) { return static_cast<f32>(k.gamepad_axis(x, 0.14f).Result); });
  const auto stick = ns([&](f32 x) { return static_cast<f32>(k.shape_fps_stick(x, 0.2f, 0.05f, 0.27f).Result.x); });
  Ct::controls_GrenadeCook g;
  const auto cook = ns([&](f32 x) { return k.GrenadeCook_step(g, x).Result ? 1.0f : 0.0f; });
  std::printf("     Controls as its C++ class, per call: GamepadAxis %.0f ns, ShapeFpsStick %.0f ns, GrenadeStep %.0f ns\n", axis.first, stick.first, cook.first);
  SC_CHECK(axis.second > 0 && stick.second > 0 && axis.first < 1e6 && stick.first < 1e6 && cook.first < 1e6, "the calls answer");
}

// A 32-bit number of the game enters a Blueprint as the decimal it stands for (the library blueprint_call
// the Blueprints share): on a threshold it is on the threshold, next to it on its side - as in 32 bit.
SC_TEST(controls_numbers_cross_exactly) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  auto decimal = [&](f32 x) { return static_cast<f64>(k.decimal(x).Result); };
  SC_CHECK(decimal(0.1f) == 0.1 && decimal(0.035f) == 0.035 && decimal(2.6f) == 2.6 && decimal(0.7071f) == 0.7071 && decimal(-0.42f) == -0.42,
           "the decimal a 32-bit number stands for");
  SC_CHECK(decimal(0) == 0 && decimal(3) == 3 && decimal(-1e9f) == -1e9 && decimal(16777216.0f) == 16777216.0, "whole numbers are themselves");
  // every threshold a rule names, and the 32-bit numbers around it: the same answer as in 32 bit
  const f32 thresholds[] = {0.02f, 0.035f, 0.05f, 0.06f, 0.08f, 0.1f, 0.12f, 0.14f, 0.18f, 0.2f, 0.22f, 0.32f, 0.36f, 0.4f, 0.42f, 0.5f, 0.55f, 0.7071f, 0.97f, 1.4f, 1.6f, 2.5f, 2.6f, 3.5f, 7.7f, 9.81f, 14.0f, 36.0f, 260.0f};
  bool same = true, back = true, ordered = true;
  for (f32 c : thresholds) {
    const f64 exact = std::strtod(std::to_string(static_cast<f64>(c)).c_str(), nullptr);  // (the decimal as a rule writes it; six digits after the point print them all)
    f32 x = c;
    for (int k = 0; k < 4; ++k) x = std::nextafter(x, -1e30f);
    f64 last = -1e300;
    for (int k = 0; k < 9; ++k, x = std::nextafter(x, 1e30f)) {
      const f64 d = decimal(x);
      same &= (x < c) == (d < exact) && (x <= c) == (d <= exact) && (x == c) == (d == exact);
      back &= static_cast<f32>(d) == x;
      ordered &= d > last;
      last = d;
    }
  }
  SC_CHECK(same, "at and around every threshold the comparison answers as in 32 bit");
  SC_CHECK(back && ordered, "the way is one to one and keeps the order");
  u32 seed = 12345;
  bool all = true;
  for (int i = 0; i < 200000; ++i) {
    seed = seed * 1664525u + 1013904223u;
    const f32 x = (static_cast<f32>(seed >> 8) / 16777216.0f - 0.5f) * (i % 3 == 0 ? 2.0f : i % 3 == 1 ? 400.0f : 0.01f);
    all &= static_cast<f32>(decimal(x)) == x;
  }
  SC_CHECK(all, "200000 numbers come back as the 32-bit numbers they were");
}
