// SpawnCity on Kern: playing in a VR headset (WebXR, Meta Quest) - the rig.
//  * the room: on foot the body faces where the head does (Engine.xr_follow_head): turning in the
//    real room turns the player, his third-person body and the first-person view together; the
//    right stick turns in 30 degree snaps (comfort), the left stick walks like any first-person
//    controller - where the head looks.
//  * the hands: the controllers' pointing rays (sc_vr_hand); the gun, the launchers and the knife
//    sit in the right hand, the fists in both, and every round leaves the right hand along its ray.
//  * arm swing (toggle, both stick clicks): jogging with the left arm walks briskly, with both arms
//    (the gun hand too, it still shoots) it runs; the stick does not move then.
#include <cmath>

#include "game.h"
#include "game_life.h"

namespace sc {

using kern::Mat4;

namespace {
constexpr f32 kSnapDeg = 30;          // snap turn
constexpr f32 kSwingWalk = 0.55f;     // m/s of hand speed (smoothed) that counts as a jogging arm
constexpr f32 kSwingFull = 1.4f;      // ... and the full pace
constexpr f32 kSwingTau = 0.22f;      // s: smoothing of the arm speed
constexpr f32 kStrikeSpeed = 2.3f;    // m/s of a controller that is a punch / a slash (a boxing game, not a trigger)
constexpr f32 kStrikeRecover = 0.28f; // s between two blows of one hand
constexpr i32 kMenuRows = 7;  // MULTI, drone / on foot, air strike, radio, locomotion, RESET, out of VR (host.mjs draws them)
constexpr f32 kMenuTop = 72, kMenuRow = 44;
}  // namespace

void Game::vr_melee(i32 side, f32 dt) {
  // a blow is the controller's own speed: the fist (or the knife in the right hand) sweeps from
  // where it was to where it is, a little past; whoever it crosses takes it - harder the faster
  VrHand& h = vr_hands_[side];
  kern::Mat4 m;
  if (!vr_hand_matrix(side, m)) { h.world_ok = false; return; }
  const Vec3 at = m.translation();
  const Vec3 prev = h.world_ok ? h.world_prev : at;
  h.world_prev = at;
  h.world_ok = true;
  const f32 now = world_->sim_time();
  const bool knife = foot_weapon_ == weapons::Id::knife && side == 1;
  const bool fists = foot_weapon_ == weapons::Id::fists || (foot_weapon_ == weapons::Id::knife && side == 0);
  if (!(knife || fists) || player_dead_ || dt <= 1e-4f) return;
  const f32 speed = h.had ? kern::length(h.p - h.prev_strike) / dt : 0;
  h.prev_strike = h.p;
  if (speed < kStrikeSpeed || now - h.strike_at < kStrikeRecover) return;
  const Vec3 d = at - prev;
  const f32 dl = kern::length(d);
  if (dl < 1e-3f) return;
  const Vec3 dir = d * (1.0f / dl);
  const Vec3 from = prev - dir * 0.05f, to = at + dir * (knife ? 0.32f : 0.14f);  // the blade reaches further
  const f32 k = clampf(speed / 4.0f, 0.6f, 1.6f);
  const f32 dmg = knife ? 100.0f : (side == 1 ? 24.0f : 16.0f) * k;  // KNIFE_DMG, PUNCH_DMG right / left
  const f32 ns = (knife ? 12.0f : 34.0f) * k;
  Vec3 end = to;
  i32 hit = people_.shot(from, end, dmg, true, now, ns);
  if (hit < 0) { end = to; if (life_punch(from, end)) hit = 0; }
  if (hit < 0) {
    const kern::physics::RayHit rh = world_->physics().cast_ray(from, to - from, ~0ull, body(soldier_));
    if (rh.hit && rh.body.valid()) { world_->physics().apply_linear_impulse(rh.body, dir * ns, rh.point); hit = 0; }
  }
  h.strike_at = now;
  look_.vm_fire_at = now;
  osfx(knife ? "gameplay_final_runtime_v2.knifeSwish" : "gameplay_final_runtime_v2.punchWhoosh", at, side == 1 ? 0 : 1, 0);
  if (hit >= 0) {
    osfx(knife ? "gameplay_final_runtime_v2.knifeHit" : "gameplay_final_runtime_v2.punchThud", at, side == 1 ? 0 : 1, 0);
    world_->play_sound(combat_sfx(combat_audio::Kind::hit), at, 0.7f, knife ? 1.3f : 0.8f, true);
  }
}

void Game::vr_grab(i32 side, bool want, f32 dt) {
  // Physics grabbing: the hand closes on what is in reach - a loose body (a crate, an animal, a
  // ragdoll), a person (who goes down as a ragdoll and is held by the chest), a car (only pushed:
  // the hand's force is a person's) - and drags it with a spring towards itself; letting go throws
  // it with the hand's speed.
  VrHand& h = vr_hands_[side];
  kern::Mat4 m;
  const bool hand = vr_hand_matrix(side, m);
  const Vec3 at = hand ? m.translation() : Vec3{};
  const f32 now = world_->sim_time();
  kern::physics::World& w = world_->physics();
  auto valid = [&](kern::physics::Body b) { return b.valid() && w.body_valid(b) && w.body_type(b) == kern::physics::BodyType::dynamic; };
  if (!want || !hand || player_dead_) {
    if (h.grab.valid() && valid(h.grab)) {
      // the throw: whatever the hand's speed was (a little more - a swing reads short in the room)
      w.set_linear_velocity(h.grab, w.linear_velocity(h.grab) * 1.15f);
      w.set_awake(h.grab, true);
    }
    h.grab = {};
    h.grab_person = -1;
    h.grab_was = false;
    return;
  }
  const bool fresh = !h.grab_was;
  h.grab_was = true;
  // a person grabbed a moment ago: his doll appears next frame - take its chest then
  if (h.grab_person >= 0 && !h.grab.valid() && glife_) {
    const kern::physics::Body b = glife_->life.ragdolls().bone_body(static_cast<u64>(h.grab_person) + 1, Ragdolls::chest);
    if (valid(b)) { h.grab = b; h.grab_local = {}; }
    else if (now - h.grab_t > 0.4f) h.grab_person = -1;
  }
  if (fresh && !h.grab.valid() && h.grab_person < 0) {
    h.grab_t = now;
    // 1. a loose body within a hand's reach (a small sphere swept a little through the hand)
    const kern::physics::RayHit rh = w.cast_sphere(at + Vec3{0, 0, 0.06f}, 0.13f, {0, 0, -0.12f}, ~0ull, body(soldier_));
    if (rh.hit && valid(rh.body)) { h.grab = rh.body; h.grab_local = w.local_point(rh.body, rh.point); }
    else {
      // 2. a person within 0.7 m of the hand (chest or head height)
      i32 best = -1;
      f32 bd = 0.7f;
      const auto& all = people_.all();
      for (std::size_t i = 0; i < all.size(); ++i) {
        const People::Person& p = all[i];
        if (p.state == People::State::down || p.state == People::State::gone || now < p.knocked_until) continue;
        const f32 d = kern::length(p.pos + Vec3{0, 0, 1.25f} - at);
        if (d < bd) { bd = d; best = static_cast<i32>(i); }
      }
      if (best >= 0) {
        people_.grab_knock(best, (at - people_.all()[static_cast<std::size_t>(best)].pos) * 0.5f + Vec3{0, 0, 0.6f}, now);
        h.grab_person = best;
      }
    }
  }
  if (!h.grab.valid()) return;
  if (!valid(h.grab)) { h.grab = {}; return; }
  // the spring: the grabbed point to the hand, the velocity change capped by a strong person's
  // force (2.5 kN) - a dog flies, a ragdoll swings, a car only rocks
  const Vec3 p = w.world_point(h.grab, h.grab_local);
  const Vec3 v = w.linear_velocity(h.grab);
  const Vec3 hv = h.world_ok && dt > 1e-4f ? (at - h.world_prev) * (1.0f / dt) : Vec3{};
  const f32 mass = std::fmax(0.5f, w.mass(h.grab));
  const Vec3 want_v = (at - p) * 12.0f + hv;
  Vec3 dv = want_v - v;
  const f32 cap = 2500.0f * std::fmax(dt, 1e-3f) / mass;
  const f32 l = kern::length(dv);
  if (l > cap) dv = dv * (cap / l);
  w.set_linear_velocity(h.grab, v + dv);
  w.set_angular_velocity(h.grab, w.angular_velocity(h.grab) * 0.9f);
  w.set_awake(h.grab, true);
}

void Game::vr_menu_input() {
  // Y (the left hand's upper button) opens the menu on the wrist; the right hand points, its trigger
  // (or A) picks: MULTI, the drone or on foot, the locomotion, RESET, leaving VR
  if (!vr_ || !engine_ || !engine_->xr) { vr_menu_open_ = false; return; }
  if (pad.connected && (pad.pressed & (1u << kPadY))) vr_menu_open_ = !vr_menu_open_;
  if (!vr_menu_open_) { vr_menu_hover_ = -1; return; }
  vr_menu_hover_ = -1;
  Vec3 from, dir;
  if (vr_menu_valid_ && vr_hand_ray(from, dir)) {
    const f32 dn = kern::dot(dir, vr_menu_n_);
    if (std::fabs(dn) > 1e-4f) {
      const f32 t = kern::dot(vr_menu_c_ - from, vr_menu_n_) / dn;
      if (t > 0 && t < 3) {
        const Vec3 p = from + dir * t - vr_menu_c_;
        const f32 u = kern::dot(p, vr_menu_r_) / vr_menu_w_ + 0.5f, v = 0.5f - kern::dot(p, vr_menu_u_) / vr_menu_h_;
        // the canvas (host.mjs vrHudDraw): the title to 72 px of 384, then the rows of 44 px
        const f32 py = v * 384;
        if (u >= 0 && u <= 1 && py >= kMenuTop && py < kMenuTop + kMenuRows * kMenuRow) vr_menu_hover_ = static_cast<i32>((py - kMenuTop) / kMenuRow);
      }
    }
  }
  const bool pick = pad.connected && ((pad.rt > 0.6f && !vr_menu_trigger_) || (pad.pressed & (1u << kPadA)));
  vr_menu_trigger_ = pad.connected && pad.rt > 0.6f;
  if (!pick || vr_menu_hover_ < 0) return;
  switch (vr_menu_hover_) {
    case 0: vr_action_ = "multi"; break;
    case 1: toggle_drone(); vr_menu_open_ = false; break;
    case 2: {
      // the air strike on what you look at (the first hit along the head's ray, else 180 m out)
      const Vec3 eye = engine_->xr_eye, f = engine_->xr_forward;
      const RayResult rr = cast(world_->physics(), eye, f, 1500, body(soldier_));
      Vec3 at = rr.hit ? rr.point : eye + f * 180.0f;
      at.z = city_.ground_height(at.x, at.y);
      if (!air_strike(at)) { toast_ = "LUFTSCHLAG LAEDT NOCH"; toast_age_ = 0; }
      vr_menu_open_ = false;
      break;
    }
    case 3: vr_action_ = "radio"; break;
    case 4: command(std::string("set vr.arms ") + (ui_.settings.on(Opt::vr_arms) ? "0" : "1")); break;
    case 5: ui_reset(); vr_menu_open_ = false; break;
    default: vr_action_ = "exit"; vr_menu_open_ = false; break;
  }
}

std::string Game::vr_menu_json() {
  char b[200];
  std::snprintf(b, sizeof b, ",\"vr\":{\"menu\":%d,\"hover\":%d,\"arms\":%d,\"action\":\"%s\"}", vr_menu_open_ ? 1 : 0, vr_menu_hover_,
                ui_.settings.on(Opt::vr_arms) ? 1 : 0, vr_action_.c_str());
  vr_action_.clear();
  return b;
}

void Game::vr_set_hand(i32 side, bool valid, Vec3 p, Quat q) {
  if (side < 0 || side > 1) return;
  VrHand& h = vr_hands_[side];
  h.valid = valid;
  if (!valid) { h.had = false; return; }
  h.p = p;
  h.q = q;
}

Vec3 Game::vr_room_forward() const {
  // what kern_app_frame_xr will use this frame: the body's heading (less the head's yaw on foot)
  Vec3 f = cam_.target - cam_.eye;
  f.z = 0;
  f = kern::normalize(f, {0, 1, 0});
  if (engine_ && engine_->xr_follow_head) {
    const f32 a = -engine_->xr_head_yaw * kDeg, c = std::cos(a), s = std::sin(a);
    f = kern::normalize(Vec3{f.x * c - f.y * s, f.x * s + f.y * c, 0}, {0, 1, 0});
  }
  return f;
}

bool Game::vr_hand_matrix(i32 side, Mat4& out) const {
  if (!vr_ || !engine_ || !engine_->xr || side < 0 || side > 1 || !vr_hands_[side].valid) return false;
  const VrHand& h = vr_hands_[side];
  const Vec3 f = vr_room_forward(), r{f.y, -f.x, 0}, up{0, 0, 1};
  auto W = [&](Vec3 v) { return r * v.x + up * v.y + f * (-v.z); };
  const Vec3 x = W(h.q.rotate({1, 0, 0})), y = W(h.q.rotate({0, 1, 0})), z = W(h.q.rotate({0, 0, 1})), p = cam_.eye + W(h.p);
  out = Mat4{};
  out.m[0] = x.x; out.m[1] = x.y; out.m[2] = x.z;
  out.m[4] = y.x; out.m[5] = y.y; out.m[6] = y.z;
  out.m[8] = z.x; out.m[9] = z.y; out.m[10] = z.z;
  out.m[12] = p.x; out.m[13] = p.y; out.m[14] = p.z;
  return true;
}

bool Game::vr_hand_ray(Vec3& from, Vec3& dir) const {
  Mat4 m;
  if (!vr_hand_matrix(1, m)) return false;
  from = m.translation();
  dir = kern::normalize(Vec3{-m.m[8], -m.m[9], -m.m[10]}, {1, 0, 0});
  return true;
}

void Game::vr_foot(kern::game::PlayInput& p, f32& forward, f32& strafe, f32 dt) {
  if (!engine_ || !engine_->xr) return;
  // no stick look in the headset: the head looks, the body follows it
  p.look_x = 0;
  p.look_y = 0;
  p.look_stick_x = p.look_stick_y = 0;
  // the room turn: the body to the room's heading plus the head's yaw in it (exact, every frame)
  const Vec3 rf = engine_->xr_room_forward;
  const f32 room = std::atan2(rf.y, rf.x) / kDeg;
  const f32 body = world_->camera_yaw();
  f32 turn = std::remainder(room + engine_->xr_head_yaw - body, 360.0f);
  // snap turn on the right stick: a flick past .7, again only after it came back under .3
  const f32 rx = pad.connected ? pad.rx : 0;
  if (std::fabs(rx) < 0.3f) vr_snap_armed_ = true;
  if (vr_snap_armed_ && std::fabs(rx) > 0.7f) { vr_snap_armed_ = false; turn += rx > 0 ? -kSnapDeg : kSnapDeg; }
  p.turn_deg = turn;
  // the locomotion is a setting (VR: Laufen mit den Armen); both stick clicks switch it in the headset
  vr_arms_ = ui_.settings.on(Opt::vr_arms);
  const bool both = pad.connected && pad.down(kPadL3) && pad.down(kPadR3);
  if (both && !vr_both_clicks_) {
    command(std::string("set vr.arms ") + (vr_arms_ ? "0" : "1"));
    vr_arms_ = ui_.settings.on(Opt::vr_arms);
    toast_ = vr_arms_ ? "VR: LAUFEN MIT DEN ARMEN (links joggen = gehen, beide = rennen)" : "VR: STICK-STEUERUNG (links gehen, rechts drehen)";
    toast_age_ = 0;
  }
  vr_both_clicks_ = both;
  // the arms' speed (smoothed), whatever the mode (the HUD can show it)
  const f32 k = 1 - std::exp(-std::fmax(dt, 1e-4f) / kSwingTau);
  for (VrHand& h : vr_hands_) {
    if (!h.valid) { h.swing += (0 - h.swing) * k; continue; }
    const f32 v = h.had && dt > 1e-4f ? kern::length(h.p - h.prev) / dt : 0;
    h.prev = h.p;
    h.had = true;
    h.swing += (std::fmin(v, 4.0f) - h.swing) * k;
  }
  // grabbing: the grip, or the trigger of a free hand (the left always; the right with the fists)
  {
    const bool fists = foot_weapon_ == weapons::Id::fists;
    const bool lg = pad.connected && (pad.down(kPadLB) || pad.lt > 0.5f), rg = pad.connected && (pad.down(kPadRB) || (fists && pad.rt > 0.5f));
    vr_grab(0, lg, dt);
    vr_grab(1, rg && (fists || foot_weapon_ == weapons::Id::knife ? true : pad.down(kPadRB)), dt);
  }
  vr_melee(0, dt);
  vr_melee(1, dt);
  if (vr_arms_) {
    // no stick walking, no stick strafing, no stick sprint: only the arms (and the real body)
    const VrHand &L = vr_hands_[0], &R = vr_hands_[1];
    const bool left = L.valid && L.swing > kSwingWalk, right = R.valid && R.swing > kSwingWalk;
    forward = 0;
    strafe = 0;
    p.boost = false;
    if (left) {
      forward = clampf((L.swing - kSwingWalk) / (kSwingFull - kSwingWalk) * 0.6f + 0.4f, 0.4f, 1);  // brisk walking
      if (right) { forward = 1; p.boost = true; }                                                    // both arms: run
    }
  }
}

void Game::vr_scope_update() {
  // The sniper's scope in VR: a second camera looks out of the objective along the barrel (5 deg),
  // its picture lies on the lens at the eyepiece - bring the rifle to the eye and look through it,
  // take it down and you see the world again (no zoom of the whole view: no sickness)
  kern::Renderer& r = engine_->renderer;
  kern::Mat4 m;
  const bool on = vr_ && engine_->xr && mode_ == Mode::foot && !player_dead_ && foot_weapon_ == weapons::Id::sniper && vr_hand_matrix(1, m);
  engine_->xr_scope = false;
  if (!on) { if (vr_scope_mesh_) r.set_visible(vr_scope_mesh_, false); return; }
  if (!vr_scope_tex_) {
    u32 fbo = 0;
    vr_scope_tex_ = r.create_render_target(512, fbo);
    engine_->xr_scope_fbo = fbo;
    engine_->xr_scope_size = 512;
    if (!vr_scope_tex_) return;
  }
  if (!vr_scope_mesh_) {
    kern::MeshData d;  // the lens: a disc (a fan), the picture mapped across it
    const Vec3 n{0, -1, 0};
    d.push({0, 0, 0}, n, {1, 1, 1}, {0.5f, 0.5f});
    constexpr int kSeg = 28;
    for (int i = 0; i <= kSeg; ++i) {
      const f32 a = static_cast<f32>(i) / kSeg * 2 * kPi;
      d.push({std::cos(a) * 0.5f, 0, std::sin(a) * 0.5f}, n, {1, 1, 1}, {0.5f + std::cos(a) * 0.5f, 0.5f - std::sin(a) * 0.5f});
      if (i > 0) d.indices.insert(d.indices.end(), {0u, static_cast<u32>(i), static_cast<u32>(i + 1)});
    }
    vr_scope_mesh_ = r.allocate_id();
    kern::MeshInput in = d.as_input(kern::kMaxExtensionAttributes);
    in.material.unlit = true;
    in.material.double_sided = true;
    in.material.cast_shadow = false;
    in.material.frustum_culled = false;
    in.texture = vr_scope_tex_;
    if (!r.upsert(vr_scope_mesh_, in)) { r.release_id(vr_scope_mesh_); vr_scope_mesh_ = 0; return; }
  }
  const Vec3 x{m.m[0], m.m[1], m.m[2]}, y{m.m[4], m.m[5], m.m[6]}, z{m.m[8], m.m[9], m.m[10]}, at = m.translation();
  // the scope sits on the rifle above the grip: the eyepiece 9 cm behind it, the objective 30 cm ahead
  const Vec3 eyepiece = at + y * 0.075f + z * 0.09f, objective = at + y * 0.075f - z * 0.3f;
  kern::Camera c = cam_;
  c.eye = objective;
  c.target = objective - z;
  c.up = y;
  c.fov = 5 * kDeg;
  c.near = 0.05f;
  engine_->xr_scope_camera = c;
  engine_->xr_scope = true;
  constexpr f32 kLens = 0.034f;  // m across
  const f32 lm[16] = {x.x * kLens, x.y * kLens, x.z * kLens, 0, -z.x, -z.y, -z.z, 0, y.x * kLens, y.y * kLens, y.z * kLens, 0, eyepiece.x, eyepiece.y, eyepiece.z, 1};
  r.update_instances(vr_scope_mesh_, {lm, 16}, {});
  r.set_visible(vr_scope_mesh_, true);
}

void Game::vr_update_view() {
  if (engine_) vr_scope_update();
  // the first-person gun in the right hand (Kern's soldier), or back in front of the camera
  Mat4 m;
  // (the game's own weapons - launchers, knife, fists - are drawn by game_look.cpp; then Kern's
  // gun stays a hidden view model)
  if (vr_ && mode_ == Mode::foot && !game_weapon_in_hand() && vr_hand_matrix(1, m)) world_->set_view_hand(&m);
  else world_->set_view_hand(nullptr);
  if (engine_) {
    engine_->xr_follow_head = vr_ && mode_ == Mode::foot && !player_dead_;
    engine_->xr_hold_room = vr_ && player_dead_;  // dying the camera falls and turns: the room stays (no spin)
  }
}

}  // namespace sc
