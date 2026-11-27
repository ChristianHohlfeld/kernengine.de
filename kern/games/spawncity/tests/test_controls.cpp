// The controls: curves, dead zones, latches and the touch controls against spawncity.de's numbers
// (CONTROLS.md). The numbers are the level's Blueprint Controls; the program asks it through controls.cpp and
// touch_play.cpp, bound to the actor "Controls" of the level's world as the game binds them (controls::bind(world)).
// Those functions are the program's Blueprint SpawnCity (and touch_play_cpp), compared with this source bit for bit
// (tests/converted/test_SpawnCity__controls_cpp.cpp, test_touch_play_cpp.cpp) - here they are called as the game
// calls them, the Controls Blueprint running as its C++ class in the world.
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "../src/blueprint_call.h"
#include "../src/controls.h"
#include "../src/touch_play.h"
#include "level_blueprints.h"
#include "rules_world.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

namespace {
using Ct = blueprints::controls_cpp;
bool near(f64 a, f64 b, f64 eps = 1e-4f) { return std::fabs(a - b) <= eps; }
/// A Blueprint of the controls running as its class, beside the level's Controls Blueprint.
template <class T>
struct ControlsBlueprint {
  kern::test::BlueprintRun rules;
  bool bound = false;
  explicit ControlsBlueprint(const char* actor) : rules("Controls", nullptr, true, {actor}) {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<T>(rules.rt);
  }
  T& operator*() { return kern::logic::blueprint<T>(); }
};
/// The level's world with the program's controls bound to its actor "Controls".
struct Bound {
  sc::test::RulesWorld level;
  bool ok = false;
  Bound() { ok = level.ok && controls::bind(level.world()); }
};
}  // namespace

#define CONTROLS_BOUND(T, actor)                                                                  \
  ControlsBlueprint<T> b(actor);                                                                  \
  kern::test::BlueprintRun& bp = b.rules;                                                                    \
  (void)bp;                                                                                       \
  KERN_CHECK(b.bound, actor " runs as its class: %s", b.rules.problems.c_str());                    \
  if (!b.bound) return;                                                                           \
  T& k = *b

#define CONTROLS_WORLD()                                                                          \
  Bound world;                                                                                    \
  KERN_CHECK(world.ok, "the level's world starts and the controls find the Controls Blueprint: %s", kern::last_error()); \
  if (!world.ok) return

KERN_TEST(controls_dead_zones) {
  {
    CONTROLS_BOUND(Ct, "controls_cpp");
    KERN_CHECK(bp.inst && bp.problems.empty() && !bp.inst->disabled(), "the level's Controls Blueprint compiles and runs: %s", bp.problems.c_str());
    KERN_CHECK(bp.inst && std::string(bp.inst->tier()) == "native C++", "it runs as its C++ class");
    (void)k;
  }
  CONTROLS_WORLD();
  auto walk_pad_axis = [&](f32 v) { return static_cast<f64>(controls::walk_pad_axis(v)); };
  auto gamepad_axis = [&](f32 v, f32 dz = 0.14f) { return static_cast<f64>(controls::gamepad_axis(v, dz)); };
  // player_walk_mode_v4.mjs axis(): .10, exponent 1.28
  KERN_CHECK(walk_pad_axis(0.1f) == 0 && near(walk_pad_axis(1), 1) && near(walk_pad_axis(-1), -1), "walk ends");
  KERN_CHECK(near(walk_pad_axis(0.55f), std::pow(0.5f, 1.28f)), "walk mid %g", walk_pad_axis(0.55f));
  // xbox_gamepad.mjs .14 linear, car .08, jet .12 hard cut, height axis .05
  KERN_CHECK(gamepad_axis(0.14f) == 0 && near(gamepad_axis(0.57f), 0.5f) && near(gamepad_axis(-1), -1), "drone .14 linear");
  KERN_CHECK(near(gamepad_axis(0.54f, 0.08f), 0.5f), "car .08 linear");
  KERN_CHECK(controls::hard_cut(0.11f, 0.12f) == 0 && controls::hard_cut(0.13f, 0.12f) == 0.13f, "jet hard cut, no rescale");
  KERN_CHECK(controls::height_axis(0.52f, 0.5f) == 0 && near(controls::height_axis(1, 0.25f), 0.75f), "height axis .05");
}

KERN_TEST(controls_fps_stick) {
  CONTROLS_WORLD();
  auto fps_dynamic_curve = [&](f32 n) { return static_cast<f64>(controls::fps_dynamic_curve(n)); };
  auto shape_fps_stick = [&](f32 x, f32 y, f32 inner, f32 s) { return controls::shape_fps_stick(x, y, inner, s); };
  // fps_control_math.mjs: the dynamic curve passes its knots; inner dead zone; anti dead zone .045
  KERN_CHECK(near(fps_dynamic_curve(0.3f), 0.24f) && near(fps_dynamic_curve(0.7f), 0.5f) && near(fps_dynamic_curve(0.9f), 0.89f) && fps_dynamic_curve(1) == 1, "knots");
  f64 last = 0;
  bool mono = true;
  for (int i = 1; i <= 100; ++i) { const f64 v = fps_dynamic_curve(i / 100.0f); mono &= v >= last - 1e-6f; last = v; }
  KERN_CHECK(mono, "monotone");
  KERN_CHECK(shape_fps_stick(0.04f, 0, 0.05f, 0.27f).x == 0, "inside the dead zone");
  KERN_CHECK(near(shape_fps_stick(1, 0, 0.05f, 0.27f).x, 1), "full");
  KERN_CHECK(near(shape_fps_stick(0.0501f, 0, 0.05f, 0.27f).x, 0.045f, 2e-3f), "anti dead zone %g", static_cast<f64>(shape_fps_stick(0.0501f, 0, 0.05f, 0.27f).x));
  // look fineness 6 -> k = .2667
  KERN_CHECK(near(controls::look_curve_strength(6), 0.48f * 5 / 9), "curve strength");
  // first_person_control_settings.mjs: fineness 1 is linear, invert flips x
  const Vec2 m = controls::shape_first_person_move(0.5f, 0, 1, true);
  KERN_CHECK(near(m.x, -0.5f) && near(controls::shape_first_person_move(0.5f, 0, 10, false).x, 0.5f * 0.4f + 0.125f * 0.6f), "move fineness");
}

KERN_TEST(controls_look_rates) {
  CONTROLS_WORLD();
  // integrateFpsLookVelocity: growing at 26 /s
  controls::LookAxis a;
  const f32 dt = 1 / 60.0f;
  const f64 d = a.step(1, dt);
  KERN_CHECK(near(d, dt - (1 - std::exp(-26 * dt)) / 26, 1e-6f), "first step %g", d);
  KERN_CHECK(near(a.velocity, 1 - std::exp(-26 * dt), 1e-6f), "velocity");
  a.step(-1, dt);  // reversing: 40 /s
  KERN_CHECK(a.velocity < 0, "reverses fast");
  // fpsTouchLookDelta: 0.0068 / 0.0059 rad per px; Kern: up to 1.6x for flicks (CoD Mobile acceleration)
  f32 y = 0, p = 0;
  controls::touch_look_delta(10, 10, 100, y, p);
  KERN_CHECK(near(y, 0.068f) && near(p, -0.059f), "slow drag 1:1");
  controls::touch_look_delta(10, 0, 5000, y, p);
  KERN_CHECK(near(y, 0.068f * 1.6f), "flick gain");
}

KERN_TEST(controls_drone_sticks) {
  CONTROLS_WORLD();
  auto phone_axis = [&](f32 v, f32 f) { return static_cast<f64>(controls::phone_axis(v, f)); };
  auto magnitude = [&](f64 a) { return static_cast<f64>(controls::game_state_stick_magnitude(static_cast<f32>(a))); };
  // control_semantics.mjs
  KERN_CHECK(near(phone_axis(1, 10), 1) && near(phone_axis(0.5f, 1), 0.5f), "phone axis ends");
  const f32 e = 0.70f * 0.38f;
  KERN_CHECK(near(phone_axis(0.5f, 10), 0.5f * (1 - e) + 0.125f * e), "phone axis expo");
  KERN_CHECK(magnitude(0.035f) == 0 && near(magnitude(1), 1), "FC shaping");
  for (f32 v : {0.1f, 0.3f, 0.5f, 0.9f}) KERN_CHECK(near(magnitude(controls::inverse_game_state_stick_magnitude(v)), v, 1e-5f), "inverse %g", static_cast<f64>(v));
  controls::DroneAxes a;
  controls::apply_game_stick_left(a, 0, -1, 10, false, 90);
  KERN_CHECK(near(a.pitch, 1) && a.roll == 0, "full forward = full pitch");
  controls::apply_game_stick_left(a, 0, -1, 10, false, 45);
  KERN_CHECK(near(magnitude(a.pitch), 0.5f, 1e-4f), "45 km/h = half the FC's speed");
  controls::apply_game_stick_left(a, 1, 0, 1, true, 90);
  KERN_CHECK(near(a.roll, -1), "invert left horizontal");
  controls::apply_game_stick_right(a, 1, 0, 1, false, true);
  KERN_CHECK(near(a.yaw, -1) && a.body_pitch == 0, "right: yaw = -x");
  controls::apply_game_stick_right(a, 0, 1, 1, false, true);
  KERN_CHECK(near(a.body_pitch, 1), "body pitch (inverted by default) = +y");
  KERN_CHECK(near(controls::clearance_pad_axis(66, 66 - 0.4f * 132 / 2, 132), 0.5f), "altitude pad");
}

KERN_TEST(controls_mouse_and_look) {
  CONTROLS_WORLD();
  // desktop_controls.mjs drone rate: 1500 px/s full, tau 50 ms
  controls::RateStick r;
  r.full_px_s = 1500;
  r.tau = 0.05f;
  r.rate = {0, 0};
  for (int i = 0; i < 120; ++i) r.step({750.0f / 60, 0}, 1 / 60.0f);
  KERN_CHECK(near(r.rate.x, 0.5f, 1e-3f), "rate %g", static_cast<f64>(r.rate.x));
  controls::WorldLook w;
  w.add(100, 100, 0.12f, 0.12f * 0.85f);
  KERN_CHECK(near(w.yaw_deg, 12) && near(w.pitch_deg, -10.2f), "0.12 deg/px, pitch x .85");
  w.step(1.0f, false);
  KERN_CHECK(near(w.yaw_deg, 12), "held for 1.4 s");
  w.step(0.5f, false);
  w.step(0.5f, false);
  KERN_CHECK(w.yaw_deg < 12 * 0.05f, "springs back at 8 /s");
  controls::CarHeadSpring c;
  for (int i = 0; i < 120; ++i) c.step(1, 0, 1 / 60.0f);
  KERN_CHECK(near(c.yaw, 1.75f, 1e-3f), "car head 1.75 rad");
}

KERN_TEST(controls_pad_buttons) {
  CONTROLS_WORLD();
  controls::ViewButton v;
  auto view = [&](bool down, f32 now) { return v.step(down, now); };
  KERN_CHECK(view(true, 0) == 0 && view(false, 0.2f) == 1, "VIEW tap = camera on release");
  KERN_CHECK(view(true, 1) == 0 && view(true, 1.5f) == 2 && view(false, 1.6f) == 0, "VIEW hold = map, no tap");
  KERN_CHECK(view(true, 2) == 0 && view(false, 2.1f) == 0, "no tap within 1.5 s after the map");
  controls::VoiceChord c;
  auto chord = [&](bool r3, bool y, bool allowed) { return c.step(r3, y, allowed); };
  KERN_CHECK(!chord(true, false, true).mic && chord(false, false, true).mic, "R3 tap: mic on release");
  chord(true, false, true);
  KERN_CHECK(chord(true, true, true).mute, "R3 + Y mutes");
  KERN_CHECK(!chord(false, false, true).mic, "and the release is not a mic toggle");
  controls::ReleaseLatch l;
  l.armed = true;
  const std::array<f32, 9> held{1, 0, 0, 0, 0, 0, 0, 0, 0}, free{0, 0, 0, 0, 0, 0.1f, 0, 0, 0};
  KERN_CHECK(l.step(held) && !l.step(free) && !l.armed, "latch until below .18");
}

KERN_TEST(controls_grenade_cook) {
  CONTROLS_WORLD();
  controls::GrenadeCook g;
  auto pull = [&](f32 now, int from) { return g.pull(now, from); };
  auto step = [&](f32 now) { return g.step(now); };
  KERN_CHECK(pull(0, 0) && g.carry == 2 && !pull(0.1f, 0), "pull once");
  g.drag(110);
  KERN_CHECK(near(g.power, 1) && near(g.speed(), 22), "drag 110 px = full power");
  KERN_CHECK(near(g.release(1.5f), 2.5f), "4 s fuse, cooked 1.5 s");
  pull(2, 0);
  KERN_CHECK(!step(5.9f) && step(6.0f) && !g.cooking, "cooks off at 4 s");
  pull(7, 0);
  g.release(7);
  KERN_CHECK(g.carry == 0 && !pull(8, 0), "3 carried");
  step(20.0f);
  KERN_CHECK(g.carry == 1, "one back after 20 s (%d)", static_cast<int>(g.carry));
}

KERN_TEST(controls_devices) {
  CONTROLS_WORLD();
  controls::Devices d;
  d.platform(false, true, 5);
  KERN_CHECK(!d.desktop, "a phone starts with touch");
  d.mouse_down();
  KERN_CHECK(!d.desktop, "a mouse does not switch a coarse device");
  d.key();
  KERN_CHECK(d.desktop, "a key does");
  d.touch_down();
  d.pad_button();
  KERN_CHECK(!d.desktop && d.pad, "touch, then pad-input");
  d.key();
  KERN_CHECK(!d.pad, "a key clears pad-input");
  controls::Devices pc;
  pc.platform(false, false, 0);
  KERN_CHECK(pc.desktop, "no touch: desktop");
  auto hint_line = [&](const char* mode, const char* weapon, bool dead) { return controls::hint_line(mode, weapon, dead); };
  KERN_CHECK(hint_line("foot", "SMG", false).rfind("[ SMG ]  WASD move", 0) == 0 && hint_line("car", "SMG", false).rfind("W / S", 0) == 0, "hint lines");
  KERN_CHECK(hint_line("jet", "", true) == "DOWN · SPACE / ENTER / R = RESPAWN", "dead line");
}

namespace {
/// The touch controls' state, as the game holds it (touch_play.h).
struct Touch {
  TouchPlay t;
  bool pointer(u32 type, i32 id, f32 x, f32 y, f64 t_ms) { return t.pointer(type, id, x, y, t_ms); }
  const TouchBtn* button(const char* id) const {
    for (const TouchBtn& b : t.buttons())
      if (b.id == id) return &b;
    return nullptr;
  }
};
}  // namespace

KERN_TEST(controls_touch_foot) {
  CONTROLS_WORLD();
  Touch t;
  t.t.set_frame(844, 390, 0, 0, 0, 0);
  TouchLayout l;
  l.mode = TouchMode::foot;
  l.weapon = "Glock";
  l.grenade = true;
  l.grenades = 3;
  t.t.set_layout(l);
  auto has = [&](const char* id) { return t.button(id) != nullptr; };
  auto move = [&] { return t.t.move(); };
  KERN_CHECK(has("jump") && has("wpn") && has("mode") && has("strike") && has("nade") && !has("scope") && !has("emp"), "foot buttons");
  // the move stick: deflection from where the thumb lands, radius .42 * 168
  const f32 cx = 84, cy = 390 - 84;
  t.pointer(0, 1, cx + 20, cy, 0);
  KERN_CHECK(move().x == 0, "landing does not jump");
  t.pointer(1, 1, cx + 20 + 0.42f * 168 * 0.5f, cy, 16);
  KERN_CHECK(near(move().x, 0.5f), "half deflection %g", static_cast<f64>(move().x));
  t.pointer(1, 1, cx + 20, cy - 200, 32);
  KERN_CHECK(t.t.sprint(), "rim forwards sprints");
  t.pointer(2, 1, cx, cy - 200, 40);
  KERN_CHECK(move().x == 0 && move().y == 0, "released");
  // the screen fires at the finger, dragging moves the aim point (no look)
  t.pointer(0, 2, 500, 150, 50);
  KERN_CHECK(t.t.fires().size() == 1 && t.t.fires()[0].hand == 0, "screen fire, right half");
  t.pointer(1, 2, 510, 160, 66);
  KERN_CHECK(t.t.fires()[0].x == 510 && t.t.take_drag().x == 0, "fire drag aims, does not look");
  t.pointer(2, 2, 430, 160, 80);
  // the look stick: 1:1 drag
  const f32 rx = 844 - 84;
  t.pointer(0, 3, rx, cy, 100);
  t.pointer(1, 3, rx + 10, cy, 200);
  KERN_CHECK(near(t.t.take_drag().x, 0.068f), "look drag %g", 0.068f);
  t.pointer(1, 3, rx + 200, cy, 300);
  KERN_CHECK(t.t.look_edge().x > 0.99f, "edge turn past the ring");
  t.pointer(2, 3, rx + 200, cy, 310);
  // JUMP is held
  if (const TouchBtn* b = t.button("jump")) {
    const f32 x = b->x + b->w / 2, y = b->y + b->h / 2;
    t.pointer(0, 4, x, y, 400);
  }
  KERN_CHECK(t.t.held("jump") && t.t.take_pressed("jump"), "jump held and pressed");
  t.pointer(2, 4, 0, 0, 450);
  KERN_CHECK(!t.t.held("jump"), "jump released");
}

// The drone of the small game: the altitude pad, ARM, STOP and the camera - no weapon, no mode switch, no air strike
// (their areas are not in it, BLUEPRINTS.md); a tap on the camera is the next camera, ARM is a press.
KERN_TEST(controls_touch_drone) {
  CONTROLS_WORLD();
  Touch t;
  t.t.set_frame(844, 390, 0, 0, 0, 0);
  TouchLayout l;
  l.mode = TouchMode::drone;
  l.camera = "FOLLOW";
  t.t.set_layout(l);
  auto has = [&](const char* id) { return t.button(id) != nullptr; };
  KERN_CHECK(has("clearance") && has("arm") && has("stop") && has("cam"), "the drone's buttons: pad, ARM, STOP, camera");
  KERN_CHECK(!has("wpn") && !has("mode") && !has("strike") && !has("emp") && t.t.buttons().size() == 4, "nothing of the removed areas (%zu buttons)", t.t.buttons().size());
  const TouchBtn* cam = t.button("cam");
  KERN_CHECK(cam && cam->label == "🎥 FOLLOW", "the camera's name on its button");
  auto tap = [&](const char* id, i32 finger, f64 at) {
    const TouchBtn* b = t.button(id);
    if (!b) return false;
    const f32 x = b->x + b->w / 2, y = b->y + b->h / 2;
    const bool owned = t.pointer(0, finger, x, y, at);
    t.pointer(2, finger, x, y, at + 80);
    return owned;
  };
  KERN_CHECK(tap("cam", 1, 0) && t.t.take_pressed("cam") && !t.t.take_pressed("cam"), "a tap on the camera: one press");
  KERN_CHECK(tap("arm", 2, 200) && t.t.take_pressed("arm"), "a tap on ARM");
  // the camera stands above the look stick, clear of it and of ARM / STOP
  cam = t.button("cam");
  const TouchBtn* arm = t.button("arm");
  KERN_CHECK(cam && arm && cam->y + cam->h <= 390 - 168 && cam->y + cam->h <= arm->y, "the camera above the stick and ARM");
  // XBOX CONTROLLER on: no sticks, no pad, no ARM / STOP - the camera stays
  l.pad_drone = true;
  t.t.set_layout(l);
  KERN_CHECK(has("cam") && !has("arm") && !has("clearance") && t.t.buttons().size() == 1, "with the pad: the camera alone");
}

KERN_TEST(controls_touch_vehicles) {
  CONTROLS_WORLD();
  Touch t;
  t.t.set_frame(844, 390, 0, 0, 0, 0);
  TouchLayout l;
  l.mode = TouchMode::drone;
  l.weapon = "MG";
  t.t.set_layout(l);
  // altitude pad: up from its centre
  const TouchBtn* pad = t.button("clearance");
  KERN_CHECK(pad && near(pad->w, 48) && near(pad->h, 132), "altitude pad 48 x 132");
  if (pad) {
    const f32 x = pad->x + 24, c = pad->y + 66;
    t.pointer(0, 1, x, c, 0);
    t.pointer(1, 1, x, c - 0.4f * 132, 10);
    const f64 up = t.t.clearance();
    KERN_CHECK(near(up, 1), "pad full up %g", up);
    t.pointer(2, 1, x, c, 20);
    KERN_CHECK(t.t.clearance() == 0, "pad springs back");
  }
  // jet: EJECT on a tap (the ejection seat as you press it)
  l.mode = TouchMode::jet;
  t.t.set_layout(l);
  const TouchBtn* ej = t.button("eject");
  KERN_CHECK(ej != nullptr, "eject button");
  if (ej) {
    const f32 x = ej->x + 10, y = ej->y + 10;
    t.pointer(0, 2, x, y, 1000);
    t.pointer(2, 2, x, y, 1150);
    KERN_CHECK(t.t.take_pressed("eject"), "a tap ejects");
  }
  // car: two free fingers pinch the chase camera
  l.mode = TouchMode::car;
  t.t.set_layout(l);
  t.pointer(0, 5, 400, 100, 0);
  t.pointer(0, 6, 500, 100, 0);
  t.pointer(1, 6, 600, 100, 10);
  KERN_CHECK(near(t.t.take_zoom(), 0.5f), "pinch out zooms in");
}

KERN_TEST(controls_aim_assist) {
  CONTROLS_WORLD();
  auto fps_aim_assist = [&](f32 yaw, f32 pitch, f32 dist, f32 input, f32 in_yaw, f32 in_pitch, f32 s) {
    return controls::fps_aim_assist(yaw, pitch, dist, input, in_yaw, in_pitch, s);
  };
  // fpsAimAssist: inactive outside the window, beyond 90 m or without input; a pull towards the
  // target, capped at .38 s rad/s, and a slowdown of up to .30 s on top of it
  KERN_CHECK(!fps_aim_assist(0.2f, 0, 20, 1, 1, 0, 0.55f).active, "outside the 7 deg window");
  KERN_CHECK(!fps_aim_assist(0.01f, 0, 95, 1, 1, 0, 0.55f).active && !fps_aim_assist(0.01f, 0, 20, 0.01f, 1, 0, 0.55f).active, "too far / no input");
  const controls::AimAssist a = fps_aim_assist(0.05f, 0, 8, 1, 1, 0, 1);
  KERN_CHECK(a.active && a.yaw > 0 && a.yaw <= 0.38f + 1e-6f && a.slowdown < 1 && a.slowdown >= 0.7f, "pull %g, slowdown %g", static_cast<f64>(a.yaw), static_cast<f64>(a.slowdown));
  KERN_CHECK(near(fps_aim_assist(0, 0, 8, 1, 1, 0, 1).slowdown, 0.7f), "on the target: slowdown 1 - .30");
  const controls::AimAssist away = fps_aim_assist(0.05f, 0, 8, 1, -1, 0, 1);
  KERN_CHECK(away.yaw < a.yaw, "moving away tracks less (%g < %g)", static_cast<f64>(away.yaw), static_cast<f64>(a.yaw));
}

// What asking the Blueprint costs (printed; `SC_TEST_RELEASE=1 run_tests.sh controls_call_cost` for
// the times of a build like the game's): a single formula, a function with a loop and a second
// function inside, and one that takes and returns a struct - each as the game calls it, the Controls Blueprint
// running as its C++ class in the world.
KERN_TEST(controls_call_cost) {
  CONTROLS_WORLD();
  auto ns = [](auto&& call) {
    const int rounds = 20000;
    f32 sum = 0;
    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < rounds; ++i) sum += call(static_cast<f32>(i % 100) / 100);
    const f64 each = std::chrono::duration<f64, std::nano>(std::chrono::steady_clock::now() - t0).count() / rounds;
    return std::pair<f64, f32>(each, sum);
  };
  const auto axis = ns([&](f32 x) { return controls::gamepad_axis(x, 0.14f); });
  const auto stick = ns([&](f32 x) { return controls::shape_fps_stick(x, 0.2f, 0.05f, 0.27f).x; });
  controls::GrenadeCook g;
  const auto cook = ns([&](f32 x) { return g.step(x) ? 1.0f : 0.0f; });
  std::printf("     Controls as its C++ class, per call: GamepadAxis %.0f ns, ShapeFpsStick %.0f ns, GrenadeStep %.0f ns\n", axis.first, stick.first, cook.first);
  KERN_CHECK(axis.second > 0 && stick.second > 0 && axis.first < 1e6 && stick.first < 1e6 && cook.first < 1e6, "the calls answer");
}

// A 32-bit number of the game enters a Blueprint as the decimal it stands for (the library blueprint_call
// the Blueprints share): on a threshold it is on the threshold, next to it on its side - as in 32 bit.
KERN_TEST(controls_numbers_cross_exactly) {
  CONTROLS_BOUND(Ct, "controls_cpp");
  auto decimal = [&](f32 x) { return static_cast<f64>(k.decimal(x).Result); };
  KERN_CHECK(decimal(0.1f) == 0.1 && decimal(0.035f) == 0.035 && decimal(2.6f) == 2.6 && decimal(0.7071f) == 0.7071 && decimal(-0.42f) == -0.42,
           "the decimal a 32-bit number stands for");
  KERN_CHECK(decimal(0) == 0 && decimal(3) == 3 && decimal(-1e9f) == -1e9 && decimal(16777216.0f) == 16777216.0, "whole numbers are themselves");
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
  KERN_CHECK(same, "at and around every threshold the comparison answers as in 32 bit");
  KERN_CHECK(back && ordered, "the way is one to one and keeps the order");
  u32 seed = 12345;
  bool all = true;
  for (int i = 0; i < 200000; ++i) {
    seed = seed * 1664525u + 1013904223u;
    const f32 x = (static_cast<f32>(seed >> 8) / 16777216.0f - 0.5f) * (i % 3 == 0 ? 2.0f : i % 3 == 1 ? 400.0f : 0.01f);
    all &= static_cast<f32>(decimal(x)) == x;
  }
  KERN_CHECK(all, "200000 numbers come back as the 32-bit numbers they were");
}
