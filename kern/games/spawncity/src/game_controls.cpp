// SpawnCity on Kern: the player's controls on every device and in every mode, exactly as on
// spawncity.de (CONTROLS.md lists every key, axis and gesture with its source). The maths lives in
// controls.cpp, the touch controls in touch_play.cpp; here they meet the modes.
//
//   devices      desktop_controls.mjs (last device wins, pointer lock), pad_actions.mjs (pad-input)
//   global       pad_actions.mjs (pad buttons, F, G, 4), fighter_jets.mjs (J, strike aiming),
//                voice_control_bindings.mjs (H, K, R3, R3+Y), ping_marker.mjs (Z, MMB, D-pad right),
//                world_map_full.mjs (M, VIEW hold)
//   on foot      player_walk_mode_v4.mjs, first_person_controller_v5.mjs, foot_touch_controller.mjs,
//                hand_grenade.mjs, sniper_scope.mjs, gameplay_final_runtime_v2.mjs (Q)
//   drone        desktop_controls.mjs, simulator.mjs, xbox_gamepad.mjs, xbox_control_mode.mjs,
//                control_semantics.mjs, drone_weapon_modes.mjs, flight_fire_fx.mjs
//   car          player_car_mode.mjs, shared_sticks.mjs
//   jet + chute  jet_mode.mjs
#include <algorithm>
#include <cstdio>
#include <cstring>

#include "game.h"

namespace sc {

using kern::Key;
using kern::game::PlayInput;
using namespace combat;

namespace {

constexpr f32 kEnterCarM = 5.2f, kEnterJetM = 9;  // player_car_mode.mjs ENTER_RADIUS_M, jet_mode.mjs ENTER_M
constexpr f32 kMouseRadPx = 0.0026f;             // player_walk_mode_v4.mjs applyMouseLook: 0.0026 rad per px
constexpr f32 kLookDegPx = 0.12f;                // desktop_controls.mjs LOOK_DEG_PER_PX (pitch x .85)
constexpr f32 kRad = 180.0f / 3.14159265358979f;

f32 key(const kern::Input& in, Key k) { return in.down(k) ? 1.0f : 0.0f; }

std::string g_last_exit;
// player_walk_mode_v4.mjs scanAimTarget: the assist target, rescanned every 115 ms
struct AimTarget { int kind = 0; i32 index = -1; f32 scan_at = -99; } g_aim;
f32 g_touch_next = 0;  // the next touch shot the gun in hand allows (sim time)  // what left the last vehicle (the "controls" state for tests)

const char* hint_weapon(Mode m, weapons::Id foot, i32 drone) {
  // desktop_controls.mjs WEAPON_NAMES (sniper, fists, knife and GUIDED have none)
  if (m == Mode::drone) return drone == 0 ? "GUN" : drone == 1 ? "MISSILE" : "";
  switch (foot) {
    case weapons::Id::smg: return "SMG";
    case weapons::Id::glock: return "PISTOL";
    case weapons::Id::grenade_launcher: return "GRENADE LAUNCHER";
    case weapons::Id::rocket: return "ROCKET LAUNCHER";
    default: return "";
  }
}

}  // namespace

// ---------------------------------------------------------------------------------------------
// per frame

void Game::read_input(kern::Engine& e, f32 dt) {
  ControlsState& C = ctl_;
  kern::Input& in = e.input;
  C.clock += dt;
  // A menu was open: the drone pad waits until its buttons are let go (xbox_gamepad.mjs).
  if (C.was_paused) { C.latch.armed = true; C.was_paused = false; }

  // ---- devices: the last one used wins (desktop_controls.mjs, pad_actions.mjs) ----
  for (u32 k = 1; k < static_cast<u32>(Key::count); ++k)
    if (in.pressed(static_cast<Key>(k))) { C.dev.key(); break; }
  C.pad_now = pad.connected ? pad.buttons : 0;
  C.pad_hit = C.pad_now & ~C.pad_prev;
  C.pad_prev = C.pad_now;
  if (C.pad_now || (pad.connected && (pad.lt > 0.5f || pad.rt > 0.5f))) {
    C.dev.pad_button();
    C.pad_drone = true;  // pad_actions.mjs enablePadPlay: XBOX CONTROLLER on with the first press (stays on)
  }
  if (in.button_pressed(0) || in.button_pressed(1) || in.button_pressed(2)) C.dev.mouse_down();
  touch_mode_ = !C.dev.desktop;

  // ---- pointer lock: desktop only, in the game, LMB / RMB on the play surface ----
  if (touch_mode_) {
    if (C.locked) e.request_pointer_lock(false);
    C.swallow_lmb = false;
  } else if (!C.locked && (in.button_pressed(0) || in.button_pressed(1))) {
    e.request_pointer_lock(true);
    if (in.button_pressed(0)) C.swallow_lmb = true;  // the capture click never fires
  }
  if (!in.button(0)) C.swallow_lmb = false;
  const bool desk = !touch_mode_;
  C.lmb = desk && C.locked && in.button(0) && !C.swallow_lmb;
  C.lmb_hit = desk && C.locked && in.button_pressed(0) && !C.swallow_lmb;
  C.rmb = desk && in.button(1);
  C.rmb_hit = desk && C.locked && in.button_pressed(1);
  C.mmb_hit = desk && C.locked && in.button_pressed(2);
  // mouse_settings.mjs scaleMouseDelta: sensitivity %, invert Y (every mode)
  C.md = C.locked && desk ? C.mouse * sensitivity_ : Vec2{};
  if (ui_.settings.on(Opt::mouse_invert_y)) C.md.y = -C.md.y;
  C.mouse = {};
  C.wheel = C.locked && desk ? in.wheel() : 0;

  ctl_discrete(e, dt);

  play_in_ = PlayInput{};
  switch (mode_) {
    case Mode::foot: play_in_ = foot_input(e, dt); break;
    case Mode::drone: drone_input(e, dt); break;
    case Mode::car: car_input(e, dt); break;
    case Mode::jet: jet_input(e, dt); break;
    case Mode::parachute: parachute_input(e, dt); break;
  }
  C.look.step(dt, C.keep_look);
  // hand grenade (hand_grenade.mjs): T held / LB held / the button held = cooking, let go = throw;
  // regeneration in every mode, a cooking grenade drops when the pilot leaves the foot mode
  ctl_grenade(mode_ == Mode::foot && in.down(Key::t), mode_ == Mode::foot && pad.connected && pad.down(kPadLB), dt);
  C.keep_look = ui_.settings.on(Opt::view_keep_look);
  C.aim_scheme = ui_.settings.on(Opt::pad_aim_scheme);  // xbox_control_mode.mjs CLASSIC / AIM  // WORLD: KEEP 360 LOOK (the head look stays)
  ui_after_input(dt);  // vehicle settings, the jetpack (game_ui.cpp, game_equipment.cpp)
  grenades_ = C.nade.carry;
}

// ---------------------------------------------------------------------------------------------
// actions shared by the modes

void Game::ctl_enter(bool jet_first) {
  // pad_actions.mjs: F boards a jet first, E (and X) a nearby car first.
  const Vec3 me = matrix_of(soldier_).translation();
  bool near_jet = jet_placed_ && std::hypot(matrix_of(jet_actor_).translation().x - me.x, matrix_of(jet_actor_).translation().y - me.y) < kEnterJetM;
  i32 best = -1;
  f32 best_d = kEnterCarM;
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    if (!cars_[i].placed) continue;
    const Vec3 p = matrix_of(cars_[i].actor).translation();
    const f32 d = std::hypot(p.x - me.x, p.y - me.y) - (cars_[i].car->spec().half.x - 1.78f);
    if (d < best_d) { best_d = d; best = static_cast<i32>(i); }
  }
  if (near_jet && (jet_first || best < 0)) { switch_mode(Mode::jet); return; }
  if (best >= 0) switch_mode(Mode::car, best);
}

void Game::ctl_discrete(kern::Engine& e, f32 dt) {
  (void)dt;
  ControlsState& C = ctl_;
  kern::Input& in = e.input;
  const f32 now = C.clock;
  auto ph = [&](int b) { return (C.pad_hit & (1u << b)) != 0; };
  const bool alive = !player_dead_;
  const bool on_foot = mode_ == Mode::foot, drone = mode_ == Mode::drone;

  // ---- voice (voice_control_bindings.mjs): H / K, R3 tap on release, R3 + Y mute ----
  const controls::VoiceChord::Out v = C.voice.step(pad.down(kPadR3), pad.down(kPadY), alive);
  if (v.mute) command("voice mute");
  if (v.mic) command("voice mic");
  if (in.pressed(Key::h)) command("voice mic");
  if (in.pressed(Key::k)) command("voice mute");
  // ---- ping (ping_marker.mjs: Z, middle mouse; pad_actions.mjs: D-pad right) and the map ----
  if (in.pressed(Key::z) || C.mmb_hit || (alive && ph(kPadRight))) command("ping");
  if (in.pressed(Key::m)) command("bigmap toggle");
  // VIEW: a tap is the camera (on release), holding it opens the big map (world_map_full.mjs)
  const int view = C.view.step(pad.down(kPadView), now);
  if (view == 2) command("bigmap toggle");
  bool camera = in.pressed(Key::c) || view == 1 || pad_touch_.take_pressed("cam");

  // ---- dead: R / Enter / Space and A / Y respawn; nothing else plays (desktop_controls.mjs:165) ----
  if (!alive) {
    C.aiming = false;
    return;
  }

  // ---- air strike (fighter_jets.mjs): aim the laser, confirm or cancel ----
  if (C.aiming) {
    f32 tx = 0, ty = 0;
    const bool tap = pad_touch_.take_strike_tap(tx, ty);
    const bool confirm = in.pressed(Key::j) || in.pressed(Key::digit4) || in.pressed(Key::enter) || C.lmb_hit || ph(kPadA) || ph(kPadUp) ||
                         (pad.rt > 0.5f && !C.pad_rt);
    const bool cancel = C.rmb_hit || ph(kPadB) || pad_touch_.take_pressed("strike_cancel");
    if (tap) ctl_strike_confirm(true, tx, ty);
    else if (confirm) ctl_strike_confirm(false, 0, 0);
    else if (cancel) { C.aiming = false; C.aim_lock_until = now + 0.5f; }
    C.pad_rt = pad.rt > 0.5f;
    if (C.aiming) C.aim_lock_until = now + 0.4f;
    // the pad belongs to the aiming (pad_actions.mjs returns here)
    C.pad_hit = 0;
  } else if (in.pressed(Key::j) || in.pressed(Key::digit4) || ph(kPadUp) || pad_touch_.take_pressed("strike")) {
    ctl_strike_start();
  }

  // ---- enter / exit, foot <-> drone, camera ----
  const bool key_e = in.pressed(Key::e), key_f = in.pressed(Key::f);
  if (key_e || key_f || ph(kPadX)) {
    if (on_foot) ctl_enter(key_f && !key_e);
    else if (mode_ == Mode::car || mode_ == Mode::jet) { g_last_exit = key_e ? "E" : key_f ? "F" : "pad X"; exit_vehicle(); }
  }
  if (on_foot && pad_touch_.take_pressed("enter")) ctl_enter(false);
  if (on_foot && pad_touch_.take_pressed("jet")) ctl_enter(true);
  if (mode_ == Mode::car && pad_touch_.take_pressed("enter")) { g_last_exit = "touch exit"; exit_vehicle(); }
  if (mode_ == Mode::jet && pad_touch_.take_pressed("eject")) { g_last_exit = "touch eject"; exit_vehicle(); }  // held 650 ms (touch_play.cpp)
  if (in.pressed(Key::v)) {
    if (on_foot || drone) toggle_drone();
    else if (mode_ == Mode::jet) jet_.toggle_mode();
  }
  if (ph(kPadDown) || pad_touch_.take_pressed("mode")) {
    // the mode button: foot <-> drone; in the car / jet it leaves the vehicle
    if (mode_ == Mode::foot || mode_ == Mode::drone) toggle_drone();
    else if (mode_ == Mode::car || mode_ == Mode::jet) { g_last_exit = ph(kPadDown) ? "pad down" : "touch mode"; exit_vehicle(); }
  }
  if (camera) {
    if (mode_ == Mode::drone) { drone_cam_ = (drone_cam_ + 1) % 3; rig_init_ = false; }
    else if (mode_ == Mode::car) car_cockpit_ = !car_cockpit_;
    else if (mode_ == Mode::jet) jet_cockpit_ = !jet_cockpit_;
  }
  if (mode_ == Mode::jet && pad_touch_.take_pressed("vtol")) jet_.toggle_mode();

  // ---- EMP (G, pad B, the EMP button: foot and drone) ----
  if ((on_foot || drone) && (in.pressed(Key::g) || ph(kPadB) || pad_touch_.take_pressed("emp"))) emp_pulse();

  // ---- weapons: Q / wheel / Y (not in the R3+Y chord) / D-pad left / the thumb button ----
  if (on_foot || drone) {
    i32 step = 0;
    if (in.pressed(Key::q) || (ph(kPadY) && !v.suppress_weapon && !pad.down(kPadR3)) || pad_touch_.take_pressed("wpn")) step = 1;
    else if (ph(kPadLeft)) step = -1;
    if (C.wheel != 0) step = C.wheel > 0 ? 1 : -1;
    if (step != 0) {
      if (on_foot) { cycle_foot_weapon(step); C.scope_on = false; }
      else drone_weapon_ = (drone_weapon_ + 1) % 3;  // drone_weapon_modes.mjs toggle: GUN -> ROCKETS -> GUIDED
    }
  }
}

void Game::ctl_strike_start() {
  ControlsState& C = ctl_;
  // fighter_jets.mjs setTargeting: not while the strike recharges
  if (world_->sim_time() - combat_.last_strike < kStrikeCooldownS) {
    toast_ = "Luftschlag laedt (" + std::to_string(static_cast<int>(kStrikeCooldownS - (world_->sim_time() - combat_.last_strike))) + " s)";
    toast_age_ = 0;
    return;
  }
  C.aiming = true;
  C.aim_lock_until = C.clock + 0.5f;
}

void Game::ctl_strike_confirm(bool at_point, f32 x, f32 y) {
  ControlsState& C = ctl_;
  Vec3 from, dir;
  if (at_point) ctl_screen_ray(x, y, from, dir);
  else {
    from = cam_.eye;
    dir = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
  }
  const RayResult r = cast(world_->physics(), from, dir, 1500, {}, kCatWorld);
  const Vec3 at = r.hit ? r.point : from + dir * 180.0f;
  C.aim_lock_until = C.clock + 0.6f;
  if (air_strike(at)) C.aiming = false;
}

bool Game::ctl_screen_ray(f32 x, f32 y, Vec3& from, Vec3& dir) const {
  // The inverse of project_screen (game_net.cpp): a ray through a CSS px point of the view.
  const Vec3 f = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
  Vec3 r{f.y * cam_.up.z - f.z * cam_.up.y, f.z * cam_.up.x - f.x * cam_.up.z, f.x * cam_.up.y - f.y * cam_.up.x};
  r = kern::normalize(r, {0, 1, 0});
  const Vec3 u{r.y * f.z - r.z * f.y, r.z * f.x - r.x * f.z, r.x * f.y - r.y * f.x};
  const f32 t = std::tan(cam_.fov / 2), aspect = frame_w_ / std::fmax(1.0f, frame_h_);
  const f32 nx = 2 * x / std::fmax(1.0f, frame_w_) - 1, ny = 1 - 2 * y / std::fmax(1.0f, frame_h_);
  from = cam_.eye;
  dir = kern::normalize(f + r * (nx * t * aspect) + u * (ny * t), f);
  return true;
}

void Game::ctl_grenade(bool hold_key, bool hold_pad, f32 dt) {
  (void)dt;
  ControlsState& C = ctl_;
  controls::GrenadeCook& g = C.nade;
  const f32 now = C.clock;
  const bool can = mode_ == Mode::foot && !player_dead_;
  static bool key_was = false, pad_was = false;
  const bool touch_down = pad_touch_.take_pressed("nade"), touch_up = pad_touch_.take_released("nade");
  if (can) {
    if (hold_key && !key_was) g.pull(now, 0);
    else if (hold_pad && !pad_was) g.pull(now, 1);
    else if (touch_down) g.pull(now, 2);
  }
  key_was = hold_key;
  pad_was = hold_pad;
  if (g.cooking && g.source == 2) g.drag(pad_touch_.grenade_drag());
  const bool let_go = g.cooking && ((g.source == 0 && !hold_key) || (g.source == 1 && !hold_pad) || (g.source == 2 && touch_up));
  Vec3 dir;
  const Vec3 eye = aim_ray(dir);
  if (g.cooking && !can) {
    // hand_grenade.mjs cookOff("dropped"): it falls at the feet with the fuse burning
    const f32 left = g.release(now);
    fire_projectile(weapons::Id::hand_grenade, eye + Vec3{0, 0, -1}, {0, 0, -1}, true, body(soldier_));
    if (!projectiles_.empty()) projectiles_.back().fuse = left;
  } else if (let_go) {
    const f32 speed = g.speed(), left = g.release(now);
    Vec3 d = dir;
    d.z += 0.26f;
    d = kern::normalize(d, dir);
    const Vec3 fwd = kern::normalize(Vec3{dir.x, dir.y, 0}, {1, 0, 0}), right{fwd.y, -fwd.x, 0};
    const Vec3 origin = eye + fwd * 0.45f + right * 0.2f + Vec3{0, 0, -0.12f};
    const Vec3 jv = world_->actor_velocity(soldier_);
    fire_projectile(weapons::Id::hand_grenade, origin, d * speed + Vec3{jv.x, jv.y, 0}, true, body(soldier_));
    if (!projectiles_.empty()) projectiles_.back().fuse = left;
  }
  // cooked off in the hand: explodeAt = GRENADE_BLAST_RADIUS_M 8.5 / GRENADE_MAX_DAMAGE 125 (gameplay_final_runtime_v2.mjs)
  if (g.step(now)) explosion(eye + dir * 0.45f, weapons::spec(weapons::Id::hand_grenade).radius_m, weapons::spec(weapons::Id::hand_grenade).damage, true);
}

// ---------------------------------------------------------------------------------------------
// on foot

PlayInput Game::foot_input(kern::Engine& e, f32 dt) {
  ControlsState& C = ctl_;
  kern::Input& in = e.input;
  const Settings& st = ui_.settings;
  const f32 now = C.clock;
  PlayInput p;
  const f32 hs = st.get(Opt::foot_look_h) / 100, vs = st.get(Opt::foot_look_v) / 100;
  const f32 ih = st.on(Opt::foot_invert_look_h) ? -1.0f : 1.0f, iv = st.on(Opt::foot_invert_look_v) ? -1.0f : 1.0f;
  const f32 move_f = st.get(Opt::foot_move_fineness), k = controls::look_curve_strength(st.get(Opt::foot_look_fineness));
  const bool inv_move = st.on(Opt::foot_invert_move_h);
  const weapons::Id w = foot_weapon_;
  const bool melee = w == weapons::Id::fists || w == weapons::Id::knife;
  C.assist_on = false;
  const bool locked = C.aim_lock_until > now;  // strike aiming: weapons rest (fighter_jets.mjs lockInput)
  // keyboard (player_walk_mode_v4.mjs update): WASD, Shift sprint, Space jump / jetpack
  f32 forward = key(in, Key::w) - key(in, Key::s), strafe = key(in, Key::d) - key(in, Key::a);
  p.jump = in.down(Key::space);
  p.boost = in.down(Key::shift_left) || in.down(Key::shift_right);
  // mouse: raw 0.0026 rad per px x look % (no acceleration, no assist); Kern divides by the ADS zoom
  p.look_x = C.md.x * kMouseRadPx * hs * ih * kRad;
  p.look_y = C.md.y * kMouseRadPx * vs * iv * kRad;
  bool fire = C.lmb, fire_edge = C.lmb_hit;
  // RMB: aim / scope; with the fists the right hand (LMB the left), the knife has no sights
  p.aim = C.rmb && !melee;
  if (w == weapons::Id::fists && C.rmb_hit) { fire = true; fire_edge = true; }
  // pad (player_walk_mode_v4.mjs:302)
  if (pad.connected) {  // on foot the pad is always read (player_walk_mode_v4.mjs), touch or not
    const Vec2 mv = controls::shape_first_person_move(controls::walk_pad_axis(pad.lx), controls::walk_pad_axis(-pad.ly), move_f, inv_move);
    forward += -mv.y;
    strafe += mv.x;
    p.boost |= pad.down(kPadL3);
    const bool rt = pad.rt > 0.5f;
    if (rt) fire = true;
    if (rt && !C.pad_fire_was) fire_edge = true;
    C.pad_fire_was = rt;
    const bool lt = pad.lt > 0.35f;
    if (w == weapons::Id::fists) {
      if (lt && !C.pad_lt_fire_was) { fire = true; fire_edge = true; }  // boxing: LT the left hand
    } else if (!melee) p.aim |= lt;
    C.pad_lt_fire_was = lt;
    p.jump |= pad.down(kPadA);
    // right stick (fps_control_math.mjs): dead zone %, dynamic curve, 3.75 / 3.40 rad/s, rate ramp
    const f32 rx = pad.rx * ih, ry = -pad.ry * iv;  // raw axes: y down
    const Vec2 s = controls::shape_fps_stick(rx, ry, st.get(Opt::foot_deadzone) / 100, k);
    // desired rate x slowdown + the assist's correction, then the rate ramp (player_walk_mode_v4.mjs:242)
    const f32 want_yaw = s.x * 3.75f * hs, want_pitch = -s.y * 3.40f * vs;
    const controls::AimAssist as = ctl_assist(want_yaw, want_pitch, std::fmin(1.0f, std::hypot(s.x, s.y)));
    const f32 dyaw = C.pad_yaw.step(want_yaw * as.slowdown + as.yaw, dt), dpitch = C.pad_pitch.step(want_pitch * as.slowdown + as.pitch, dt);
    p.look_x += dyaw * kRad;
    p.look_y -= dpitch * kRad;
    if (std::hypot(pad.rx, pad.ry) > 0.22f && pad_touch_.look_live()) pad_touch_.release_all();  // gameplay_final_input_extras.mjs
    p.gamepad = true;
  }
  // touch (foot_touch_controller.mjs)
  bool touch_fire = false, touch_fresh = false;
  if (touch_mode_) {
    const Vec2 m = pad_touch_.move();
    const Vec2 mv = controls::shape_first_person_move(m.x, m.y, move_f, inv_move);
    forward += -mv.y;
    strafe += mv.x;
    p.boost |= pad_touch_.sprint();
    // look: the 1:1 drag plus the edge turn (touch profile: inner .025, 4.2 / 3.3 rad/s)
    const Vec2 d = pad_touch_.take_drag();
    const f32 ld = std::clamp(dt, 1 / 240.0f, 0.05f);
    if (d.x != 0 || d.y != 0) {
      // applyProfiledLookDelta (touch drag): input |px| / 20, the slowdown only a hint (x .3)
      const f32 yaw = d.x * hs * ih, pitch = d.y * vs * iv;
      const f32 input = clampf(std::hypot(d.x / 0.0068f, d.y / 0.0059f) / 20, 0, 1);
      const controls::AimAssist as = ctl_assist(yaw / ld, pitch / ld, input);
      const f32 slow = 1 - (1 - as.slowdown) * 0.3f;
      p.look_x += (yaw * slow + as.yaw * ld) * kRad;
      p.look_y -= (pitch * slow + as.pitch * ld) * kRad;
    }
    const Vec2 edge = pad_touch_.look_edge();
    if (edge.x != 0 || edge.y != 0) {
      // applyTouchLookStick: touch profile (inner .025), 4.2 / 3.3 rad/s, the assist as for the pad
      const Vec2 s = controls::shape_fps_stick(edge.x * ih, edge.y * iv, 0.025f, k);
      const f32 yaw = s.x * 4.2f * hs, pitch = -s.y * 3.3f * vs;
      const controls::AimAssist as = ctl_assist(yaw, pitch, std::fmin(1.0f, std::hypot(s.x, s.y)));
      p.look_x += (yaw * as.slowdown + as.yaw) * std::clamp(dt, 0.0f, 0.05f) * kRad;
      p.look_y -= (pitch * as.slowdown + as.pitch) * std::clamp(dt, 0.0f, 0.05f) * kRad;
    }
    p.jump |= pad_touch_.held("jump");
    if (pad_touch_.take_pressed("scope") && w == weapons::Id::sniper) C.scope_on = !C.scope_on;
    if (w != weapons::Id::sniper) C.scope_on = false;
    p.aim |= C.scope_on;
    // the screen fires at the finger (foot_touch_controller.mjs): SMG every 55 ms (missed shots are
    // caught up, at most 4 per frame), Glock every 170 ms, every other weapon once per touch. The
    // shot is a ray through the finger: the game fires it (Kern's soldier shoots through the centre).
    const bool kern_gun = !game_weapon_in_hand();
    for (TouchFire& f : pad_touch_.fires()) {
      const f32 repeat = w == weapons::Id::smg ? 0.055f : w == weapons::Id::glock ? 0.17f : -1;
      int shots = 0;
      if (f.fresh) { shots = 1; f.last_shot = now; }
      else if (repeat > 0) {
        while (now - f.last_shot >= repeat && shots < 4) { f.last_shot += repeat; ++shots; }
        if (now - f.last_shot >= repeat) f.last_shot = now;
      }
      f.fresh = false;
      if (shots == 0 || locked) continue;
      if (kern_gun) {
        for (int i = 0; i < shots; ++i) touch_fire |= ctl_touch_shot(f.x, f.y);
      } else {
        touch_fresh = true;
        C.aim_point = true;
        C.aim_x = f.x;
        C.aim_y = f.y;
      }
    }
  }
  p.move_x = clampf(strafe, -1, 1);
  p.move_y = clampf(forward, -1, 1);
  const f32 mm = std::hypot(p.move_x, p.move_y);
  if (mm > 1) { p.move_x /= mm; p.move_y /= mm; }
  // weapons: 1 MP, 2 Glock, 3 Sniper, 5 grenade launcher, 6 rocket launcher, 7 fists, 8 knife (4 is the strike)
  const std::pair<Key, weapons::Id> keys[] = {{Key::digit1, weapons::Id::smg}, {Key::digit2, weapons::Id::glock}, {Key::digit3, weapons::Id::sniper},
                                              {Key::digit5, weapons::Id::grenade_launcher}, {Key::digit6, weapons::Id::rocket}, {Key::digit7, weapons::Id::fists},
                                              {Key::digit8, weapons::Id::knife}};
  for (const auto& [kk, id] : keys)
    if (in.pressed(kk)) { select_foot_weapon(id); C.scope_on = false; }
  if (locked) { fire = fire_edge = touch_fresh = false; }
  if (game_weapon_in_hand()) {
    // launchers and melee: the game fires them (at the finger on touch); Kern's trigger stays off
    if (touch_fresh) foot_attack(true, true);
    else { C.aim_point = false; foot_attack(fire, fire_edge); }
    p.fire = false;
  } else {
    // Kern fires the guns (keyboard / mouse / pad); an empty magazine on touch pulls Kern's trigger
    // once so that the soldier reloads (touch_fire)
    p.fire = fire || touch_fire;
  }
  C.aim_point = false;
  trigger_was_ = p.fire;
  return p;
}

controls::AimAssist Game::ctl_assist(f32 in_yaw, f32 in_pitch, f32 input) {
  // player_walk_mode_v4.mjs aimAssistForInput / scanAimTarget: people, police drones and the other
  // players (aim point +1.24 m on people), the best of ellipse + distance / 90 * .12 in the window,
  // in line of sight, rescanned every 115 ms; fps_control_math.mjs fpsAimAssist (pad and touch only).
  const f32 s = ui_.settings.get(Opt::foot_aim_assist) / 100;
  if (s <= 0 || input <= 0.02f || player_dead_) return {};
  const Vec3 eye = cam_.eye, fwd = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
  const f32 fyaw = std::atan2(fwd.y, fwd.x), fpitch = std::asin(clampf(fwd.z, -1, 1));
  auto point = [&](int kind, i32 i, Vec3& p) {
    if (kind == 1) {
      const auto& all = people_.all();
      if (i < 0 || i >= static_cast<i32>(all.size()) || all[i].state == People::State::down || all[i].state == People::State::gone) return false;
      p = all[i].pos + Vec3{0, 0, 1.24f};
      return true;
    }
    if (kind == 2) {
      if (i < 0 || i >= 5 || !pdrones_[i].active) return false;
      p = pdrones_[i].pos;
      return true;
    }
    if (kind == 3) {
      const auto& r = net_.remotes();
      if (i < 0 || i >= static_cast<i32>(r.size())) return false;
      p = r[i].state.mode == net::PlayerMode::foot ? r[i].person_shown + Vec3{0, 0, 1.24f} : r[i].shown;
      return true;
    }
    return false;
  };
  auto errors = [&](Vec3 p, f32& ye, f32& pe, f32& d) {
    const Vec3 to = p - eye;
    d = kern::length(to);
    ye = -wrap_pi(std::atan2(to.y, to.x) - fyaw);  // right-positive
    pe = std::asin(clampf(to.z / std::fmax(d, 1e-3f), -1, 1)) - fpitch;
  };
  Vec3 p;
  f32 ye = 0, pe = 0, d = 0;
  const f32 now = ctl_.clock;
  if (now - g_aim.scan_at >= 0.115f || !point(g_aim.kind, g_aim.index, p)) {
    g_aim = AimTarget{};
    g_aim.scan_at = now;
    f32 best = 1e9f;
    auto consider = [&](int kind, i32 i) {
      Vec3 q;
      if (!point(kind, i, q)) return;
      f32 y, pt, dd;
      errors(q, y, pt, dd);
      const f32 e = controls::aim_ellipse(y, pt);
      if (e >= 1 || dd > 90 || dd < 0.5f) return;
      const f32 score = e + dd / 90 * 0.12f;
      if (score >= best) return;
      if (cast(world_->physics(), eye, (q - eye) * (1 / dd), dd - 0.4f, body(soldier_), kCatWorld).hit) return;  // behind a wall
      best = score;
      g_aim.kind = kind;
      g_aim.index = i;
    };
    for (i32 i = 0; i < static_cast<i32>(people_.all().size()); ++i) consider(1, i);
    for (i32 i = 0; i < 5; ++i) consider(2, i);
    for (i32 i = 0; i < static_cast<i32>(net_.remotes().size()); ++i) consider(3, i);
    if (!point(g_aim.kind, g_aim.index, p)) return {};
  }
  errors(p, ye, pe, d);
  const controls::AimAssist a = controls::fps_aim_assist(ye, pe, d, input, in_yaw, in_pitch, s);
  ctl_.assist_on |= a.active && 1 - a.slowdown > 0.0015f;  // #footReticle.aim-assisted (strength > .015)
  return a;
}

namespace {
// What the original's HUD modules show and the game state does not say directly (ctl_view_json).
struct ViewTrack {
  i32 last_stars = 0;
  f32 banner_until = -1;
  bool busted = false;
  f32 last_hp = -1, last_drone_hp = -1, last_car_hp = -1, last_jet_hp = -1;
  u32 dmg = 0;
  bool was_dead = false;
  Fun fun;
  bool fun_valid = false;
  u32 toast_serial = 0;
  std::vector<std::string> toasts;  // JSON objects, newest last (gameplay_fun_runtime.mjs toastFor)
  bool drone_was_alive = true;
} g_view;

std::string jesc(const std::string& s) {
  std::string o;
  for (char c : s) {
    if (c == '"' || c == '\\') o += '\\';
    if (static_cast<unsigned char>(c) < 0x20) continue;
    o += c;
  }
  return o;
}
}  // namespace

std::string Game::ctl_view_json() {
  ControlsState& C = ctl_;
  ViewTrack& V = g_view;
  const f32 now = world_->sim_time();
  std::string s = ",\"view\":{";
  char b[640];
  auto add = [&](const char* fmt, auto... v) { std::snprintf(b, sizeof b, fmt, v...); s += b; };
  auto screen = [&](Vec3 p, f32& x, f32& y) {
    f32 nx, ny, depth;
    const bool on = project_screen(p, nx, ny, depth);
    x = (nx + 1) / 2 * frame_w_;
    y = (1 - ny) / 2 * frame_h_;
    return on && depth > 0;
  };
  // ---- the camera (speed_lines.mjs measures its speed itself)
  add("\"eye\":[%.2f,%.2f,%.2f]", cam_.eye.x, cam_.eye.y, cam_.eye.z);
  // ---- damage taken (flight_fire_fx.mjs arondight:combat-damage -> .combat-damage-vignette)
  {
    const f32 hp = world_->hud().health, dhp = drone_hp_, jhp = jet_hp_, chp = car_index_ >= 0 ? cars_[car_index_].hp : -1;
    if ((V.last_hp >= 0 && hp >= 0 && hp < V.last_hp - 0.01f) || (mode_ == Mode::drone && dhp < V.last_drone_hp - 0.01f) || (mode_ == Mode::jet && jhp < V.last_jet_hp - 0.01f) ||
        (mode_ == Mode::car && chp >= 0 && V.last_car_hp >= 0 && chp < V.last_car_hp - 0.01f))
      ++V.dmg;
    V.last_hp = hp; V.last_drone_hp = dhp; V.last_jet_hp = jhp; V.last_car_hp = chp;
  }
  add(",\"dmg\":%u,\"droneHits\":%u,\"assist\":%s", V.dmg, C.drone_hits, C.assist_on ? "true" : "false");
  // ---- car (player_car_mode.mjs commandSpeed: along the car)
  if (mode_ == Mode::car && car_index_ >= 0) add(",\"carKmh\":%.0f", std::fabs(cars_[car_index_].car->forward_speed()) * 3.6f);
  add(",\"cockpit\":%s", car_cockpit_ ? "true" : "false");
  // xbox_control_mode.mjs: the AIM scheme's crosshair while LB looks (the drone)
  add(",\"xboxAim\":%s", C.aim_scheme && mode_ == Mode::drone && pad.connected && pad.down(kPadLB) ? "true" : "false");
  // ---- jet (jet_mode.mjs renderHud / renderCityCue / renderButtons)
  if (mode_ == Mode::jet && jet_placed_) {
    const JetTelemetry& t = jet_.telemetry();
    const BodyState st = state_of(jet_actor_);
    const f32 v = kern::length(st.velocity);
    f32 fx = 0, fy = 0;
    bool fpm = false;
    if (v > 15) fpm = screen(st.position + st.velocity * 4.0f, fx, fy);
    const bool landed = jet_.landed() || (t.contacts >= 2 && v < 4);
    const f32 rocket_left = std::fmax(0.0f, next_jet_missile_ - now) * 1000;
    add(",\"jet\":{\"kmh\":%.0f,\"mach\":%.3f,\"agl\":%.1f,\"nz\":%.2f,\"alpha\":%.2f,\"flight\":%s,\"nozzleCos\":%.3f,\"lever\":%.3f,"
        "\"thrust\":%.3f,\"landed\":%s,\"hp\":%.0f,\"burning\":%s,\"rocketLeft\":%.0f,\"fpm\":%s,\"fx\":%.1f,\"fy\":%.1f}",
        v * 3.6f, t.mach, t.agl, t.nz, v > 20 ? t.alpha / kDeg : 0.0f, jet_.mode() == Jet::Mode::flight ? "true" : "false", std::cos(jet_.nozzle()), jet_.lever(),
        clampf(jet_.thrust() / jet_.spec().thrust_max, 0, 1), landed ? "true" : "false", std::round(jet_hp_ / 400 * 100),
        gaps_ && gaps_->jet_burning ? "true" : "false", rocket_left, fpm ? "true" : "false", fx, fy);
  }
  // ---- wanted (wanted_police_drones.mjs renderHud / renderEmpControl)
  {
    const i32 stars = wanted_.stars();
    if (V.last_stars > 0 && stars == 0) { V.banner_until = now + 2.1f; V.busted = player_dead_; }
    V.last_stars = stars;
    i32 active = 0, in_range = 0;
    f32 inbound = 1e9f;
    const Vec3 me = player_position();
    for (const PoliceDrone& d : pdrones_) {
      if (!d.active) continue;
      ++active;
      if (!d.emp && kern::length(d.pos - me) <= wanted::kEmpRange) ++in_range;
      if (d.shots == 0) inbound = std::fmin(inbound, d.next_shot - now);
    }
    const bool searching = stars > 0 && wanted_.hidden_ms() > 0;
    const f32 escape_left = stars > 0 ? std::fmax(0.0f, wanted::escape_ms(stars) - wanted_.hidden_ms()) : 0;
    const f32 next_wave = active == 0 && next_drone_wave_ > now ? (next_drone_wave_ - now) * 1000 : -1;
    add(",\"wanted\":{\"stars\":%d,\"active\":%d,\"searching\":%s,\"escapeMs\":%.0f,\"inboundMs\":%.0f,\"nextWaveMs\":%.0f,\"banner\":%s,\"busted\":%s,"
        "\"empMs\":%.0f,\"inRange\":%d,\"miss\":%s}",
        stars, active, searching ? "true" : "false", escape_left, inbound < 1e8f ? std::fmax(0.0f, inbound) * 1000 : 0.0f, next_wave, now < V.banner_until ? "true" : "false",
        V.busted ? "true" : "false", std::fmax(0.0f, emp_ready_at_ - now) * 1000, in_range, now < C.emp_miss_until ? "true" : "false");
  }
  // ---- the contract and its toasts (gameplay_fun_runtime.mjs render / toastFor)
  {
    const Fun& f = combat_.fun;
    const Contract& k = contract(f.contract_index);
    if (V.fun_valid) {
      const Fun& p = V.fun;
      auto toast = [&](const std::string& body) {
        V.toasts.push_back("{\"id\":" + std::to_string(++V.toast_serial) + "," + body + "}");
        if (V.toasts.size() > 5) V.toasts.erase(V.toasts.begin());
      };
      const bool died = player_dead_ && !V.was_dead;
      const bool drone_lost = !drone_out_ && V.drone_was_alive && drone_hp_ <= 0;
      if (f.contract_complete && !p.contract_complete) {
        const Contract& pk = contract(p.contract_index);
        toast("\"type\":\"contractComplete\",\"name\":\"" + jesc(pk.name) + "\",\"reward\":" + std::to_string(pk.reward));
      }
      if (died && p.at_risk > 0) toast("\"type\":\"insurance\",\"insured\":" + std::to_string(std::max(0, f.score - p.score)) + ",\"lost\":" + std::to_string(std::max(0, p.at_risk - f.at_risk - std::max(0, f.score - p.score))));
      else if (drone_lost && p.at_risk > f.at_risk) toast("\"type\":\"setback\",\"label\":\"" + jesc(f.last_label) + "\",\"lost\":" + std::to_string(p.at_risk - f.at_risk) + ",\"mercy\":" + std::to_string(f.mercy));
      else if (f.score > p.score && f.at_risk == 0 && p.at_risk > 0)
        toast("\"type\":\"bank\",\"points\":" + std::to_string(f.score - p.score) + ",\"peakStars\":" + std::to_string(std::max(p.max_stars, p.stars)) + ",\"score\":" + std::to_string(f.score));
      if (f.overdrive_until > p.overdrive_until + 0.01f && f.overdrive_until > now)
        toast("\"type\":\"overdrive\",\"durationMs\":" + std::to_string(static_cast<int>(kOverdriveS * 1000)) + ",\"bonus\":" + std::to_string(std::max(0, f.at_risk - p.at_risk)));
      else if (f.last_award != p.last_award && f.combo >= 2 && f.at_risk > p.at_risk) {
        std::snprintf(b, sizeof b, "\"type\":\"award\",\"points\":%d,\"label\":\"%s\",\"combo\":%d,\"multiplier\":%.2f", f.at_risk - p.at_risk, jesc(f.last_label).c_str(), f.combo, f.multiplier(now));
        toast(b);
      }
      if (p.combo >= 2 && f.combo == 0 && f.score == p.score && !died) toast("\"type\":\"comboBreak\",\"combo\":" + std::to_string(p.combo));
    }
    V.fun = f;
    V.fun_valid = true;
    V.was_dead = player_dead_;
    V.drone_was_alive = drone_out_;
    add(",\"fun\":{\"reward\":%d,\"complete\":%s,\"overdrive\":%s,\"momentum\":%d,\"mercy\":%d,\"damageScale\":%.2f,\"risk\":%d,\"score\":%d,\"high\":%d,"
        "\"mult\":%.2f,\"name\":\"%s\",\"hint\":\"%s\",\"progress\":%d,\"target\":%d,\"stars\":%d,\"toasts\":[",
        k.reward, f.contract_complete ? "true" : "false", now < f.overdrive_until ? "true" : "false", f.momentum, f.mercy, f.police_damage_scale(), f.at_risk, f.score,
        f.high_score, f.multiplier(now), jesc(k.name).c_str(), jesc(k.hint).c_str(), f.contract_progress, k.target, wanted_.stars());
    for (std::size_t i = 0; i < V.toasts.size(); ++i) s += (i ? "," : "") + V.toasts[i];
    s += "]}";
  }
  // ---- GUIDED (guided_missile.mjs renderLock / renderCooldown, gameplay_final_runtime_v2.mjs updateDroneAim)
  if (mode_ == Mode::drone) {
    f32 lx = 0, ly = 0, ld = 0;
    bool lock = false;
    std::string label;
    if (!guided_.empty() && guided_.back().m.tracking) {
      const guided::Flight& g = guided_.back();
      lock = screen(g.m.goal, lx, ly);
      ld = kern::length(g.m.goal - cam_.eye);
      label = g.t.label;
    }
    // the centre ray: a vehicle, a drone or a person under the crosshair = "ZIEL"
    Vec3 dir = kern::normalize(cam_.target - cam_.eye, {1, 0, 0});
    const RayResult r = cast(world_->physics(), cam_.eye, dir, 600, body(drone_actor_), kCatVehicle | kCatDrone | kCatWorld);
    bool actor = false;
    for (const TrafficCar& c : cars_)
      if (r.hit && c.placed && kern::length(matrix_of(c.actor).translation() - r.point) < c.car->spec().half.x + 1.5f) actor = true;
    const f32 reach = r.hit ? r.distance : 600;
    for (const auto& q : people_.all()) {
      if (q.state == People::State::gone || q.state == People::State::down) continue;
      const Vec3 to = q.pos + Vec3{0, 0, 1} - cam_.eye;
      const f32 along = kern::dot(to, dir);
      if (along > 0 && along < reach && kern::length(to - dir * along) < 1.2f) { actor = true; break; }
    }
    add(",\"guidedView\":{\"lock\":%s,\"x\":%.1f,\"y\":%.1f,\"d\":%.0f,\"label\":\"%s\",\"denied\":%s,\"actor\":%s,\"cdMs\":%.0f}", lock ? "true" : "false", lx, ly, ld,
        jesc(label).c_str(), now < C.guided_denied_until ? "true" : "false", actor ? "true" : "false", std::fmax(0.0f, guided_last_ + guided::kCooldown - now) * 1000);
  }
  // ---- the strike's recharge (fighter_jets.mjs renderButton --cool)
  add(",\"strikeK\":%.3f", clampf((now - combat_.last_strike) / kStrikeCooldownS, 0, 1));
  // the drone's last impact marks [serial, x, y] (screen px); the page shows the ones it has not shown
  s += ",\"impacts\":[";
  bool first = true;
  for (u32 i = C.impact_serial > 8 ? C.impact_serial - 8 : 0; i < C.impact_serial; ++i) {
    f32 x, y;
    if (!screen(C.impacts[i % 8], x, y)) continue;
    add("%s[%u,%.0f,%.0f]", first ? "" : ",", i + 1, x, y);
    first = false;
  }
  s += "]";
  return s + "}";
}

bool Game::ctl_touch_shot(f32 x, f32 y) {
  // One round of the gun in hand through a screen point: Kern's magazine, SpawnCity's damage and
  // range (weapons.h), the shot event for the town / the mates (handle_shots), the gun's sound.
  // Returns true when the magazine is empty (Kern's trigger then reloads it).
  kern::game::SoldierState st;
  if (!world_->soldier_state(soldier_, st) || st.dead) return false;
  if (st.reload >= 0) return false;
  if (st.ammo <= 0) return true;
  // the weapon's own rate (first_person_weapon_runtime_v3.mjs: a shot inside its interval is dropped)
  const f32 t = world_->sim_time();
  if (t < g_touch_next && foot_weapon_ != weapons::Id::smg) return false;  // SMG: missed rounds are caught up
  g_touch_next = t + weapons::spec(foot_weapon_).interval_ms / 1000 * 0.95f;
  --st.ammo;
  world_->apply_soldier_state(st);
  const weapons::Spec& s = weapons::spec(foot_weapon_);
  Vec3 from, dir;
  ctl_screen_ray(x, y, from, dir);
  kern::game::ShotEvent e;
  hit_scan(from, dir, s.range_m, s.damage, true, body(soldier_), e);
  world_->shots().push_back(e);
  const char* snd = foot_weapon_ == weapons::Id::smg ? "sc:shot0" : foot_weapon_ == weapons::Id::sniper ? "builtin:sniper" : "sc:pistol0";
  world_->play_sound(snd, from, 0.9f, 1, true);
  world_->shake_camera(foot_weapon_ == weapons::Id::sniper ? 0.25f : 0.08f);
  return false;
}

// ---------------------------------------------------------------------------------------------
// drone

void Game::drone_input(kern::Engine& e, f32 dt) {
  ControlsState& C = ctl_;
  kern::Input& in = e.input;
  const Settings& st = ui_.settings;
  const DroneTelemetry& t = drone_.telemetry();
  const f32 now = C.clock;
  // pad sample (xbox_gamepad.mjs): neutral while the release latch holds
  const f32 latch_vals[9] = {pad.down(kPadA) ? 1.0f : 0.0f, pad.down(kPadB) ? 1.0f : 0.0f, pad.down(kPadX) ? 1.0f : 0.0f, pad.down(kPadY) ? 1.0f : 0.0f,
                             pad.down(kPadRB) ? 1.0f : 0.0f, pad.lt, pad.rt, pad.down(kPadView) ? 1.0f : 0.0f, pad.down(kPadMenu) ? 1.0f : 0.0f};
  const bool pad_on = pad.connected && !C.latch.step(latch_vals, 9);
  const bool aim = pad_on && C.aim_scheme && pad.down(kPadLB);
  // left stick: WASD (x = D - A, y = S - W), the touch stick, LS
  Vec2 left{key(in, Key::d) - key(in, Key::a), key(in, Key::s) - key(in, Key::w)};
  if (touch_mode_) left += pad_touch_.move();
  if (pad_on) left += Vec2{controls::gamepad_axis(pad.lx), controls::gamepad_axis(-pad.ly)};
  // right stick: arrows, the mouse rate (1500 px/s, 50 ms) unless RMB looks, the touch stick, RS
  Vec2 right{key(in, Key::right) - key(in, Key::left), key(in, Key::down) - key(in, Key::up)};
  if (C.rmb) {
    C.look.add(C.md.x, C.md.y, kLookDegPx, kLookDegPx * 0.85f);
    C.drone_mouse.reset();
  } else {
    if (C.look.owned && !C.rmb) C.look.release();
    C.drone_mouse.step(C.md, dt);
    right += C.drone_mouse.rate;
  }
  if (touch_mode_) right += pad_touch_.look();
  if (pad_on) {
    const Vec2 rs{controls::gamepad_axis(pad.rx), controls::gamepad_axis(-pad.ry)};
    if (aim) C.look.set(C.look.yaw_deg + rs.x * 105 * dt, C.look.pitch_deg - rs.y * 82 * dt);  // AIM: LB looks (real_world_bootstrap.mjs)
    else right += rs;
  }
  right = {clampf(right.x, -1, 1), clampf(right.y, -1, 1)};
  controls::DroneAxes a;
  controls::apply_game_stick_left(a, left.x, left.y, st.get(Opt::drone_left_fineness), st.on(Opt::drone_invert_roll), st.get(Opt::drone_max_speed_kmh));
  controls::apply_game_stick_right(a, right.x, right.y, st.get(Opt::drone_right_fineness), st.on(Opt::drone_invert_yaw), st.on(Opt::drone_invert_pitch));
  sticks_.roll = a.roll;
  sticks_.pitch = a.pitch;
  sticks_.yaw = a.yaw;
  sticks_.body_pitch = a.body_pitch;
  // height axis: Space / Shift, RT - LT, the touch altitude pad
  f32 axis = key(in, Key::space) - ((in.down(Key::shift_left) || in.down(Key::shift_right)) ? 1.0f : 0.0f);
  if (pad_on) axis += controls::height_axis(pad.rt, pad.lt);
  if (touch_mode_) axis += pad_touch_.clearance();
  axis = clampf(axis, -1, 1);
  // weapons: 1 gun, 2 rockets (3 selects nothing: desktop_controls.mjs order gun, missile)
  if (in.pressed(Key::digit1)) drone_weapon_ = 0;
  if (in.pressed(Key::digit2)) drone_weapon_ = 1;
  // fire: LMB, RB, a finger on the screen (at the finger: flight_fire_fx.mjs, MG every 92 ms)
  const bool locked = C.aim_lock_until > now;
  bool trigger = C.lmb || (pad_on && pad.down(kPadRB));
  bool pressed = C.lmb_hit || (pad_on && (C.pad_hit & (1u << kPadRB)));
  for (TouchFire& f : pad_touch_.fires()) {
    const bool due = f.fresh || (drone_weapon_ == 0 && now - f.last_shot >= 0.092f);
    if (due) {
      trigger = true;
      pressed |= f.fresh;
      f.last_shot = now;
      C.aim_point = true;
      C.aim_x = f.x;
      C.aim_y = f.y;
    }
    f.fresh = false;
  }
  if (locked) trigger = pressed = false;
  // guided_missile.mjs press(): denied while it reloads -> body.guided-denied for 900 ms
  if (pressed && drone_weapon_ == 2 && world_->sim_time() < guided_last_ + guided::kCooldown) C.guided_denied_until = world_->sim_time() + 0.9f;
  drone_attack(trigger, pressed);
  C.aim_point = false;
  trigger_was_ = trigger;
  // ARM (R, A, the ARM button): arm on the ground; in the air a controlled landing (simulator.mjs)
  const bool arm_key = in.pressed(Key::r) || (pad_on && (C.pad_hit & (1u << kPadA))) || pad_touch_.take_pressed("arm");
  const f32 agl = t.agl >= 0 ? t.agl : 0;
  if (arm_key) {
    if (!t.armed && !sticks_.arm) { sticks_.arm = true; landing_ = false; }
    else if (t.armed && agl > 0.7f && !landing_) landing_ = true;
    else if (landing_) landing_ = false;  // a second tap cancels the landing
    else { sticks_.arm = false; landing_ = false; }
  }
  // STOP (touch): the hard kill
  if (pad_touch_.take_pressed("stop")) { C.kill_until = now + 0.3f; sticks_.arm = false; landing_ = false; }
  sticks_.kill = now < C.kill_until;
  if (axis > 0.3f && !t.armed && !t.calibrating && !t.fault && !landing_ && !sticks_.kill) sticks_.arm = true;
  if (t.fault && !t.armed) sticks_.arm = false;  // the switch goes low for the power cycle
  if (sticks_.game_mode) {
    if (landing_) {
      axis = -(agl > 6 ? 1.0f : std::fmax(0.35f, agl / 6));
      if (agl <= 0.7f && std::fabs(t.vz) < 0.35f) landing_still_ += dt;
      else landing_still_ = 0;
      if (landing_still_ > 0.25f) { sticks_.arm = false; landing_ = false; landing_still_ = 0; }
    }
    const bool have_agl = t.agl >= 0;
    if (std::fabs(axis) > 1e-4f) {
      height_held_ = true;
      const f32 rate = clearance_rate(axis);
      sticks_.clearance = have_agl ? clampf(agl + clearance_lead(rate), 0.5f, 120) : clampf(sticks_.clearance + rate * std::clamp(dt, 0.0f, 0.05f), 0.5f, 120);
    } else if (height_held_) {
      height_held_ = false;
      if (have_agl) sticks_.clearance = clampf(agl + clearance_lead(t.vz), 0.5f, 120);
    }
  } else {
    sticks_.throttle = clampf(sticks_.throttle + axis * dt * 0.8f, 0, 1);
  }
}

// ---------------------------------------------------------------------------------------------
// car

void Game::car_input(kern::Engine& e, f32 dt) {
  ControlsState& C = ctl_;
  kern::Input& in = e.input;
  if (car_index_ < 0) return;
  TrafficCar& c = cars_[car_index_];
  CarInput ci;
  ci.max_speed = 115 / 3.6f;  // the settings override it (game_ui.cpp)
  ci.max_reverse = 32 / 3.6f;
  // player_car_mode.mjs controls(): keys + the claimed sticks + the pad (dead zone .08, linear)
  f32 steer = key(in, Key::d) - key(in, Key::a), pedal = key(in, Key::w) - key(in, Key::s);
  f32 look_x = 0, look_y = 0;
  if (touch_mode_) {
    steer += pad_touch_.move().x;
    pedal += -pad_touch_.move().y;
    look_x += pad_touch_.look().x;
    look_y += -pad_touch_.look().y;
  }
  bool brake = in.down(Key::space) || pad_touch_.held("hb");
  if (pad.connected) {
    steer += controls::gamepad_axis(pad.lx, 0.08f);
    pedal += pad.rt - pad.lt;
    look_x += controls::gamepad_axis(pad.rx, 0.08f);
    look_y += controls::gamepad_axis(pad.ry, 0.08f);  // pad.ry is up-positive: -axis(axes[3])
    brake = brake || pad.down(kPadB);
  }
  ci.steer = -clampf(steer, -1, 1);  // Kern's car: + steers left
  ci.pedal = clampf(pedal, -1, 1);
  ci.handbrake = brake;
  c.input = ci;
  // head look: the stick spring plus the desktop mouse (0.12 deg/px, back after 1.4 s idle)
  C.car_head.step(clampf(look_x, -1, 1), clampf(look_y, -1, 1), dt);
  if (C.md.x != 0 || C.md.y != 0) C.look.add(C.md.x, C.md.y, kLookDegPx, kLookDegPx * 0.85f);
  head_yaw_ = -(C.car_head.yaw + C.look.yaw_deg / kRad);
  head_pitch_ = C.car_head.pitch + C.look.pitch_deg / kRad;
  // chase zoom: wheel x 1.1 (+ - in ctl_command), two fingers (pinch)
  if (C.wheel != 0) C.car_zoom *= C.wheel > 0 ? 1.1f : 1 / 1.1f;
  if (touch_mode_) C.car_zoom *= pad_touch_.take_zoom();
  C.car_zoom = clampf(C.car_zoom, 0.55f, 2.6f);
}

// ---------------------------------------------------------------------------------------------
// jet and parachute

void Game::jet_input(kern::Engine& e, f32 dt) {
  ControlsState& C = ctl_;
  kern::Input& in = e.input;
  const f32 now = C.clock;
  // jet_mode.mjs readInputs: left stick lift + rudder, right stick pitch + roll (pull back = nose up)
  f32 lift = key(in, Key::w) - key(in, Key::s), yaw = key(in, Key::d) - key(in, Key::a);
  f32 pitch = key(in, Key::down) - key(in, Key::up), roll = key(in, Key::right) - key(in, Key::left);
  if (touch_mode_) {
    lift += -pad_touch_.move().y;
    yaw += pad_touch_.move().x;
    pitch += pad_touch_.look().y;
    roll += pad_touch_.look().x;
  }
  // mouse: a rate stick (730 px/s, 25 ms), mouse up = nose up
  if (C.locked && !touch_mode_) {
    C.jet_mouse.step(C.md, dt);
    roll += C.jet_mouse.rate.x;
    pitch -= C.jet_mouse.rate.y;
  } else C.jet_mouse.reset();
  if (pad.connected) {
    // the pad: dead zone .12 as a hard cut (no rescale)
    lift += controls::hard_cut(pad.ly, 0.12f);
    yaw += controls::hard_cut(pad.lx, 0.12f);
    roll += controls::hard_cut(pad.rx, 0.12f);
    pitch += controls::hard_cut(-pad.ry, 0.12f);
  }
  JetInput j;
  j.lift = clampf(lift, -1, 1);
  j.yaw = clampf(yaw, -1, 1);
  j.pitch = clampf(pitch, -1, 1);
  j.roll = clampf(roll, -1, 1);
  const bool ground = jet_.telemetry().contacts >= 2;
  j.brake = ground && ((jet_.mode() == Jet::Mode::hover && j.lift <= 0.05f) || (jet_.mode() == Jet::Mode::flight && jet_.lever() < 0.05f && j.lift < 0)) ? 1.0f : 0.0f;
  jet_in_ = j;
  // weapons: Space / LMB / RT / KANONE / a finger = gun; R / LT / RAKETE selects the rocket (then
  // LMB, RT or a tap launches it); B / RB / BOMBE bomb; LB / V / FLUG hover <-> flight
  if (in.pressed(Key::r) || (pad.connected && pad.lt > 0.5f && !C.pad_lt) || pad_touch_.take_pressed("rkt")) C.rocket = !C.rocket;
  C.pad_lt = pad.connected && pad.lt > 0.5f;
  if (pad.connected && (C.pad_hit & (1u << kPadLB))) jet_.toggle_mode();
  const bool bomb = in.pressed(Key::b) || (pad.connected && (C.pad_hit & (1u << kPadRB))) || pad_touch_.take_pressed("bomb");
  const bool rt = pad.connected && pad.rt > 0.4f;
  bool launch = false;
  if (C.rocket && (C.lmb_hit || (rt && !C.pad_rt))) launch = true;
  bool tap = false;
  for (TouchFire& f : pad_touch_.fires()) {
    if (f.fresh && C.rocket) launch = true;
    f.fresh = false;
    tap = !C.rocket;
    C.jet_aim = true;
    C.aim_x = f.x;
    C.aim_y = f.y;
  }
  if (!tap) C.jet_aim = false;
  if (C.lmb_hit && C.rocket) C.rocket_suppress = true;  // the click that launched does not fire the gun
  if (!C.lmb) C.rocket_suppress = false;
  C.pad_rt = rt;
  const bool gun = !C.rocket && !C.rocket_suppress && (C.lmb || in.down(Key::space) || rt || pad_touch_.held("fire") || tap);
  const bool locked = C.aim_lock_until > now;
  bool missile = launch && !locked && world_->sim_time() >= next_jet_missile_;
  jet_attack(gun && !locked, missile, bomb);
  if (missile) C.rocket = false;  // jet_mode.mjs launchSelectedRocket: the selection ends
}

void Game::parachute_input(kern::Engine& e, f32 dt) {
  ControlsState& C = ctl_;
  kern::Input& in = e.input;
  // jet_mode.mjs stepPara: the right-hand inputs steer (arrows, mouse, RS, the right stick)
  f32 roll = key(in, Key::right) - key(in, Key::left), pitch = key(in, Key::down) - key(in, Key::up);
  if (touch_mode_) { roll += pad_touch_.look().x; pitch += pad_touch_.look().y; }
  if (C.locked && !touch_mode_) {
    C.jet_mouse.step(C.md, dt);
    roll += C.jet_mouse.rate.x;
    pitch -= C.jet_mouse.rate.y;
  }
  if (pad.connected) { roll += controls::hard_cut(pad.rx, 0.12f); pitch += controls::hard_cut(-pad.ry, 0.12f); }
  chute_steer_ = chute_open_ ? Vec2{clampf(roll, -1, 1), -clampf(pitch, -1, 1)} : Vec2{};
  head_yaw_ = 0;
  // Landed: on foot again where the pilot touched down.
  const BodyState s = state_of(chute_actor_);
  const RayResult g = cast(world_->physics(), s.position, {0, 0, -1}, 2.0f, body(chute_actor_));
  if (g.hit && g.distance < 1.0f && chute_time_ > 0.5f) {
    world_->set_alive(chute_actor_, false);
    show_soldier(g.point, world_->camera_yaw());
    mode_ = Mode::foot;
  }
}

// ---------------------------------------------------------------------------------------------
// the head look in the drone, jet and parachute views (real_world_bootstrap.mjs world look)

void Game::ctl_camera(kern::Camera& c) {
  const ControlsState& C = ctl_;
  if (mode_ == Mode::foot || mode_ == Mode::car) return;
  if (C.look.yaw_deg == 0 && C.look.pitch_deg == 0) return;
  const Vec3 d = c.target - c.eye;
  const f32 len = kern::length(d);
  if (len < 1e-4f) return;
  const Vec3 f = d * (1 / len);
  const f32 yaw = -C.look.yaw_deg / kRad, pitch = C.look.pitch_deg / kRad;
  const Vec3 turned = Quat::axis_angle({0, 0, 1}, yaw).rotate(f);
  Vec3 side = kern::cross(turned, {0, 0, 1});
  side = kern::normalize(side, {1, 0, 0});
  const Vec3 out = Quat::axis_angle(side, pitch).rotate(turned);
  c.target = c.eye + out * len;
}

// ---------------------------------------------------------------------------------------------
// touch

void Game::set_touch_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb, f32 dpr) {
  frame_w_ = w;
  frame_h_ = h;
  dpr_ = dpr > 0 ? dpr : 1;
  pad_touch_.set_frame(w, h, sl, st, sr, sb);
}

bool Game::touch(u32 type, i32 id, f32 x, f32 y, f64 t_ms) {
  if (type == 0) ctl_.dev.touch_down();
  touch_mode_ = !ctl_.dev.desktop;
  return pad_touch_.pointer(type, id, x, y, t_ms);
}

void Game::update_touch_layout() {
  TouchLayout l;
  if (touch_mode_ && world_placed_) {
    switch (mode_) {
      case Mode::foot: l.mode = TouchMode::foot; break;
      case Mode::drone: l.mode = TouchMode::drone; break;
      case Mode::car: l.mode = TouchMode::car; break;
      case Mode::jet: l.mode = TouchMode::jet; break;
      case Mode::parachute: l.mode = TouchMode::chute; break;
    }
  }
  l.alive = !player_dead_;
  l.pad_drone = ctl_.pad_drone;
  l.aiming = ctl_.aiming;
  if (mode_ == Mode::foot) {
    const Vec3 me = matrix_of(soldier_).translation();
    l.near_jet = jet_placed_ && std::hypot(matrix_of(jet_actor_).translation().x - me.x, matrix_of(jet_actor_).translation().y - me.y) < kEnterJetM;
    for (const TrafficCar& c : cars_)
      if (c.placed) { const Vec3 p = matrix_of(c.actor).translation(); l.near_car |= std::hypot(p.x - me.x, p.y - me.y) < kEnterCarM; }
    l.weapon = weapons::spec(foot_weapon_).name;
    l.sniper = foot_weapon_ == weapons::Id::sniper;
    l.scope_on = ctl_.scope_on;
    l.grenade = true;
    l.grenades = ctl_.nade.carry;
    l.grenade_left = ctl_.nade.cooking ? ctl_.nade.left(ctl_.clock) : -1;
    l.drone_cooldown = world_->sim_time() < combat_.drone_ready_at;
  } else if (mode_ == Mode::drone) {
    l.weapon = drone_weapon_ == 0 ? "MG" : drone_weapon_ == 1 ? "RAKETEN" : "GUIDED";
    l.armed = drone_.telemetry().armed;
  }
  ctl_.near_car = l.near_car;
  ctl_.near_jet = l.near_jet;
  l.emp = wanted_.stars() > 0 && (mode_ == Mode::foot || mode_ == Mode::drone);
  l.strike_ready = world_->sim_time() - combat_.last_strike >= kStrikeCooldownS;
  l.cockpit = car_cockpit_;
  l.hover = jet_.mode() == Jet::Mode::hover;
  l.rocket = ctl_.rocket;
  l.ground = jet_.telemetry().contacts >= 2;
  pad_touch_.set_layout(l);
}

std::string Game::touch_view_json() {
  if (!touch_mode_) return "null";
  // A label's line break ("▲\nHÖHE\n▼") must reach JSON.parse escaped (it broke the page's parse).
  const std::string raw = pad_touch_.view_json();
  std::string out;
  out.reserve(raw.size() + 16);
  for (char c : raw) {
    if (c == '\n') out += "\\n";
    else if (static_cast<unsigned char>(c) < 0x20) out += ' ';
    else out += c;
  }
  return out;
}

// ---------------------------------------------------------------------------------------------
// HUD and commands

std::string Game::ctl_hud_json() {
  ControlsState& C = ctl_;
  if (ui_.paused) C.was_paused = true;
  const char* m = mode_ == Mode::parachute ? "jet" : mode_name(mode_);  // under the canopy the jet's line
  const std::string hint = controls::hint_line(m, hint_weapon(mode_, foot_weapon_, drone_weapon_), player_dead_);
  if (hint != C.hint) { C.hint = hint; C.hint_at = C.clock; }
  std::string s = ",\"ctl\":{\"device\":\"";
  s += C.dev.desktop ? "desktop" : "touch";
  char b[256];
  std::snprintf(b, sizeof b, "\",\"pad\":%s,\"locked\":%s,\"aiming\":%s,\"dead\":%s,\"rocket\":%s,\"scope\":%s,\"zoom\":%.3f,\"look\":[%.2f,%.2f],\"nade\":{\"carry\":%d,\"left\":%.2f},",
                C.dev.pad ? "true" : "false", C.locked ? "true" : "false", C.aiming ? "true" : "false", player_dead_ ? "true" : "false", C.rocket ? "true" : "false",
                C.scope_on ? "true" : "false", C.car_zoom, C.look.yaw_deg, C.look.pitch_deg, C.nade.carry, C.nade.cooking ? C.nade.left(C.clock) : -1.0f);
  s += b;
  const bool fade = C.clock - C.hint_at > 12 && !player_dead_;
  s += "\"lastExit\":\"" + g_last_exit + "\",";
  s += "\"hint\":\"" + C.hint + "\",\"hintFaded\":";
  s += fade ? "true" : "false";
  s += C.near_car ? ",\"nearCar\":true" : ",\"nearCar\":false";
  s += C.near_jet ? ",\"nearJet\":true" : ",\"nearJet\":false";
  s += C.pad_drone ? ",\"padDrone\":true" : ",\"padDrone\":false";
  std::snprintf(b, sizeof b, ",\"sticks\":[%.3f,%.3f,%.3f,%.3f],\"head\":[%.3f,%.3f]", sticks_.roll, sticks_.pitch, sticks_.yaw, sticks_.body_pitch, head_yaw_, head_pitch_);
  s += b;
  s += ctl_view_json();
  return s + "}";
}

bool Game::ctl_command(const std::string& cmd, std::string& out) {
  ControlsState& C = ctl_;
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (starts("device ")) {
    // the page: "device touch|mouse|mousemove|key|pad", "device platform <fine> <coarse> <touchPoints>"
    const std::string a = cmd.substr(7);
    if (a == "touch") C.dev.touch_down();
    else if (a == "mouse") C.dev.mouse_down();
    else if (a == "mousemove") C.dev.mouse_move();
    else if (a == "key") C.dev.key();
    else if (a == "pad") C.dev.pad_button();
    else if (a.rfind("platform ", 0) == 0) {
      int fine = 1, coarse = 0, tp = 0;
      std::sscanf(a.c_str() + 9, "%d %d %d", &fine, &coarse, &tp);
      C.dev.platform(fine != 0, coarse != 0, tp);
    }
    touch_mode_ = !C.dev.desktop;
    out = C.dev.desktop ? "desktop" : "touch";
    return true;
  }
  if (starts("zoom ")) {
    // player_car_mode.mjs: + = x 1 / 1.15, - = x 1.15
    if (mode_ == Mode::car) C.car_zoom = clampf(C.car_zoom * (cmd == "zoom in" ? 1 / 1.15f : 1.15f), 0.55f, 2.6f);
    out = std::to_string(C.car_zoom);
    return true;
  }
  if (starts("look ")) {
    // the minimap drag (real_world_bootstrap.mjs): "look set <yawDeg> <pitchDeg>", "look release", "look keep 0|1"
    f32 y = 0, p = 0;
    int k = 0;
    if (std::sscanf(cmd.c_str(), "look set %f %f", &y, &p) == 2) C.look.set(y, p);
    else if (cmd == "look release") C.look.release();
    else if (std::sscanf(cmd.c_str(), "look keep %d", &k) == 1) C.keep_look = k != 0;
    char b[64];
    std::snprintf(b, sizeof b, "%.2f %.2f", C.look.yaw_deg, C.look.pitch_deg);
    out = b;
    return true;
  }
  if (starts("strike")) {
    // "strike aim" (4 / J / D-pad up), "strike go", "strike cancel" (Esc, RMB, B)
    f32 sx = 0, sy = 0;
    if (std::sscanf(cmd.c_str(), "strike at %f %f", &sx, &sy) == 2) {
      // fighter_jets.mjs overlay pointerup: the laser on the ground under the finger, then the run
      if (C.aiming) ctl_strike_confirm(true, sx, sy);
    } else if (cmd == "strike aim") ctl_strike_start();
    else if (cmd == "strike go") ctl_strike_confirm(false, 0, 0);
    else if (cmd == "strike cancel") C.aiming = false;
    else return false;
    out = C.aiming ? "aiming" : "idle";
    return true;
  }
  if (starts("scheme ")) {
    // xbox_control_mode.mjs: CLASSIC / AIM (the setting pad.aimScheme)
    ui_.settings.set(Opt::pad_aim_scheme, cmd == "scheme aim" ? 1.0f : 0.0f);
    out = cmd == "scheme aim" ? "aim" : "classic";
    return true;
  }
  if (starts("press ")) {
    // web/view.mjs: a click on the original's button (ARM, STOP, weapon, mode, EINSTEIGEN, ...)
    pad_touch_.inject(cmd.substr(6));
    out = "ok";
    return true;
  }
  if (starts("touch rects")) {
    // web/view.mjs: "touch rects id x y w h;id x y w h;..." - where the page drew each button
    std::vector<TouchPlay::DomRect> rects;
    std::size_t i = 11;
    while (i < cmd.size()) {
      std::size_t j = cmd.find(';', i);
      if (j == std::string::npos) j = cmd.size();
      char id[32] = {};
      TouchPlay::DomRect d;
      if (std::sscanf(cmd.substr(i, j - i).c_str(), " %31s %f %f %f %f", id, &d.x, &d.y, &d.w, &d.h) == 5) { d.id = id; rects.push_back(d); }
      i = j + 1;
    }
    pad_touch_.set_dom_rects(std::move(rects));
    out = "ok";
    return true;
  }
  {
    // jet_mode.mjs renderCityCue: a place on the screen ("project <lon> <lat>"; the page looks the
    // town up, the game knows where it is): {"x","y","front","dist"} in CSS px, behind = mirrored
    f64 lon = 0, lat = 0;
    if (std::sscanf(cmd.c_str(), "project %lf %lf", &lon, &lat) == 2) {
      f64 x = 0, y = 0;
      city_.projector().project(lon, lat, x, y);
      const Vec3 me = mode_ == Mode::parachute ? matrix_of(chute_actor_).translation() : mode_ == Mode::jet ? matrix_of(jet_actor_).translation() : player_position();
      const Vec3 p{static_cast<f32>(x), static_cast<f32>(y), city_.ground_height(static_cast<f32>(x), static_cast<f32>(y)) + 20};
      f32 nx, ny, depth;
      project_screen(p, nx, ny, depth);
      char b[160];
      std::snprintf(b, sizeof b, "{\"x\":%.1f,\"y\":%.1f,\"front\":%s,\"dist\":%.0f}", (nx + 1) / 2 * frame_w_, (1 - ny) / 2 * frame_h_, depth > 0 ? "true" : "false",
                    std::hypot(p.x - me.x, p.y - me.y));
      out = b;
      return true;
    }
  }
  if (cmd == "touch release") {
    // foot_touch_controller.mjs releaseAll: the page is hidden, or no finger is left on the glass
    pad_touch_.release_all();
    out = "ok";
    return true;
  }
  if (starts("xbox ")) {
    // the XBOX CONTROLLER switch (drone_control_settings.mjs xboxControllerEnabled): hides the drone's touch controls
    C.pad_drone = cmd == "xbox on";
    out = C.pad_drone ? "on" : "off";
    return true;
  }
  if (starts("dronemode")) {
    // GAME / ANGLE (no key on spawncity.de; kept as a command)
    sticks_.game_mode = !sticks_.game_mode;
    out = sticks_.game_mode ? "game" : "angle";
    return true;
  }
  if (starts("contract")) {
    combat_.pop = combat_.fun.apply(FunEvent::cycle_contract, world_->sim_time(), wanted_.stars());
    combat_.pop_at = world_->sim_time();
    out = combat_.pop;  // the new contract's line ("AUFTRAG ...")
    return true;
  }
  if (cmd == "controls") {
    out = ctl_hud_json().substr(7);
    return true;
  }
  return false;
}

}  // namespace sc
