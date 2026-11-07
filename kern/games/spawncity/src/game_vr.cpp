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
constexpr f32 kSwingSprint = 2.6f;    // ... and the hardest sprint (1.6 x the walk)
constexpr f32 kSwingTau = 0.22f;      // s: smoothing of the arm speed
constexpr f32 kStrikeSpeed = 1.8f;    // m/s of a controller that is a punch / a slash (a boxing game, not a trigger)
constexpr f32 kStrikeRecover = 0.28f; // s between two blows of one hand
constexpr i32 kMenuRows = 9;  // the rows of a page (host.mjs vrHudDraw draws them, the same order)
constexpr f32 kMenuTop = 66, kMenuRow = 35;
constexpr f32 kResetHold = 1.0f;  // s of Y held = RESET
}  // namespace

void Game::vr_melee(i32 side, f32 dt) {
  // A blow is the controller's own motion, as in the boxing games - no trigger, no button: while
  // the fist (or the knife in the right hand) moves fast, every frame its path from where it was to
  // where it is - a fist-wide tube, a little past - is swept; the first body it crosses takes the
  // blow, harder the faster the hand, and is shoved along the hand's direction. Only a landed blow
  // starts the hand's recovery (a fast start of the swing that has not reached anyone yet does not
  // use the blow up).
  VrHand& h = vr_hands_[side];
  kern::Mat4 m;
  if (!vr_hand_matrix(side, m)) { h.world_ok = false; h.swish = false; return; }
  const Vec3 at = m.translation();
  const Vec3 prev = h.world_ok ? h.world_prev : at;
  h.world_prev = at;
  h.world_ok = true;
  const f32 now = world_->sim_time();
  const bool knife = foot_weapon_ == weapons::Id::knife && side == 1;
  const bool fists = foot_weapon_ == weapons::Id::fists || (foot_weapon_ == weapons::Id::knife && side == 0);
  if (!(knife || fists) || player_dead_ || dt <= 1e-4f || h.grab.valid() || h.grab_person >= 0) { h.prev_strike = h.p; h.swish = false; return; }
  // the hand's speed in the room (the body's own walking left out): a jab is 3-6 m/s
  const f32 speed = h.had ? kern::length(h.p - h.prev_strike) / dt : 0;
  h.prev_strike = h.p;
  if (speed < kStrikeSpeed) { h.swish = false; return; }
  if (!h.swish) {
    h.swish = true;
    osfx(knife ? "gameplay_final_runtime_v2.knifeSwish" : "gameplay_final_runtime_v2.punchWhoosh", at, side == 1 ? 0 : 1, 0);
  }
  if (now - h.strike_at < kStrikeRecover) return;
  const Vec3 d = at - prev;
  const f32 dl = kern::length(d);
  if (dl < 1e-3f) return;
  const Vec3 dir = d * (1.0f / dl);
  const f32 reach = knife ? 0.3f : 0.1f;  // the blade reaches further than the knuckles
  const Vec3 a = kern::normalize(kern::cross(dir, Vec3{0, 0, 1}), Vec3{1, 0, 0}), b = kern::cross(a, dir);
  const f32 r = knife ? 0.03f : 0.06f;
  const Vec3 offs[5] = {{}, a * r, a * -r, b * r, b * -r};
  const f32 k = clampf(speed / 4.0f, 0.6f, 1.8f);
  const f32 dmg = knife ? 100.0f : (side == 1 ? 26.0f : 18.0f) * k;  // KNIFE_DMG, PUNCH_DMG right / left
  const f32 ns = (knife ? 12.0f : 34.0f) * k;
  i32 hit = -1;
  Vec3 at_hit = at;
  for (const Vec3& o : offs) {
    const Vec3 from = prev + o - dir * 0.04f, to = at + o + dir * reach;
    // a mate in the way takes the blow (damage by the authority, and a shove on his machine)
    Vec3 mt = to;
    if (hit_remotes(from, mt, dmg)) {
      hit = 0;
      at_hit = mt;
      for (const net::Remote& rm : net_.remotes())
        if (kern::length(rm.person_shown + Vec3{0, 0, 1.2f} - mt) < 1.2f) {
          net::Fx f;
          f.kind = net::Fx::push;
          f.p = dir * (knife ? 1.5f : 3.5f * k) + Vec3{0, 0, 0.8f};
          f.dir = {static_cast<f32>(rm.state.slot), 0, 0};
          net_.send_fx(f);
        }
      break;
    }
    Vec3 end = to;
    if ((hit = people_.shot(from, end, dmg, true, now, ns, 0)) >= 0) { at_hit = end; break; }
    end = to;
    if (life_punch(from, end)) { hit = 0; at_hit = end; break; }
    // anything loose (a crate, a ragdoll, a car): the hand's momentum - a fist and forearm of some
    // 4 kg at the hand's speed, a blade less
    const kern::physics::RayHit rh = world_->physics().cast_ray(from, to - from, ~0ull, body(soldier_));
    if (rh.hit && rh.body.valid()) {
      world_->physics().apply_linear_impulse(rh.body, dir * (speed * (knife ? 1.5f : 4.0f) * 1.0f), rh.point);
      world_->physics().set_awake(rh.body, true);
      hit = 0;
      at_hit = rh.point;
      break;
    }
  }
  if (hit < 0) return;
  h.strike_at = now;
  look_.vm_fire_at = now;
  osfx(knife ? "gameplay_final_runtime_v2.knifeHit" : "gameplay_final_runtime_v2.punchThud", at_hit, side == 1 ? 0 : 1, 0);
  world_->play_sound(combat_sfx(combat_audio::Kind::hit), at_hit, 0.7f, knife ? 1.3f : 0.8f, true);
  world_->shake_camera(0.04f * k);  // the jolt of a landed blow (the hand has no haptics from here)
  vr_pulse_[side] = std::fmin(1.0f, 0.4f + 0.3f * k);  // and the controller's rumble (host.mjs)
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
  auto push_mate = [&](u8 slot, Vec3 dv) {
    // player_push.mjs: the mate's own machine applies the velocity change (it is replicated back)
    net::Fx f;
    f.kind = net::Fx::push;
    f.p = {clampf(dv.x, -12, 12), clampf(dv.y, -12, 12), clampf(dv.z, -6, 9)};
    f.dir = {static_cast<f32>(slot), 0, 0};
    f.a = 0;
    net_.send_fx(f);
  };
  const Vec3 hand_v = h.world_ok && dt > 1e-4f ? (at - h.world_prev) * (1.0f / dt) : Vec3{};
  // the hand's velocity over the last ~0.1 s (a throw is the swing, not the last frame's jitter)
  h.throw_v = h.throw_v + (hand_v - h.throw_v) * (1 - std::exp(-std::fmax(dt, 1e-4f) / 0.06f));
  if (!want || !hand || player_dead_) {
    // the throw: the swing's velocity, a game's arm (x 1.6 - a real swing in a room is short, and a
    // body is light in a hand that never tires); a held person's whole doll flies, not one bone
    const Vec3 tv = h.throw_v * 1.6f + Vec3{0, 0, kern::length(h.throw_v) > 2 ? 0.8f : 0.0f};
    if (h.grab_slot >= 0) push_mate(static_cast<u8>(h.grab_slot), tv);
    h.grab_slot = -1;
    if (h.grab.valid() && valid(h.grab)) {
      const bool swung = kern::length(h.throw_v) > 1.2f;
      if (h.grab_person >= 0 && glife_) {
        for (u8 b = 0; b < Ragdolls::kBones; ++b) {
          const kern::physics::Body bb = glife_->life.ragdolls().bone_body(static_cast<u64>(h.grab_person) + 1, static_cast<Ragdolls::Bone>(b));
          if (valid(bb)) { if (swung) w.set_linear_velocity(bb, tv); w.set_awake(bb, true); }
        }
      } else if (swung) w.set_linear_velocity(h.grab, tv);
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
    else if (const kern::physics::Body ab = glife_ ? glife_->life.grab_animal(at, 0.45f) : kern::physics::Body{}; valid(ab)) {
      // a live dog, cat or cow: picked up as a body (it gets up where it lands, life.cpp)
      h.grab = ab;
      h.grab_local = w.local_point(ab, at);
    } else {
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
      } else if (net_.active()) {
        // 3. a mate on foot: held by the collar - pulled and shoved on his own machine
        f32 md = 0.75f;
        for (const net::Remote& rm : net_.remotes()) {
          if (rm.state.mode != net::PlayerMode::foot || (rm.state.flags & net::kDead)) continue;
          const f32 d = kern::length(rm.person_shown + Vec3{0, 0, 1.3f} - at);
          if (d < md) { md = d; h.grab_slot = rm.state.slot; }
        }
        h.grab_mate_at = 0;
      }
    }
  }
  if (h.grab_slot >= 0) {
    // the mate follows the hand: every 80 ms a small velocity change towards it
    if (now - h.grab_mate_at > 0.08f) {
      h.grab_mate_at = now;
      for (const net::Remote& rm : net_.remotes())
        if (rm.state.slot == h.grab_slot) {
          const Vec3 chest = rm.person_shown + Vec3{0, 0, 1.3f};
          Vec3 dv = (at - chest) * 2.0f + hand_v * 0.35f;
          const f32 l = kern::length(dv);
          if (l > 3) dv = dv * (3 / l);
          if (kern::length(at - chest) > 1.6f) { h.grab_slot = -1; break; }  // torn loose
          push_mate(rm.state.slot, dv);
        }
    }
    return;
  }
  if (!h.grab.valid()) return;
  if (!valid(h.grab)) { h.grab = {}; return; }
  // the spring: the grabbed point to the hand, the velocity change capped by a strong person's
  // force (2.5 kN) - a dog flies, a ragdoll swings, a car only rocks
  const Vec3 p = w.world_point(h.grab, h.grab_local);
  const Vec3 v = w.linear_velocity(h.grab);
  const Vec3 hv = hand_v;
  const f32 mass = std::fmax(0.5f, w.mass(h.grab));
  const Vec3 want_v = (at - p) * 12.0f + hv;
  Vec3 dv = want_v - v;
  const f32 cap = 6000.0f * std::fmax(dt, 1e-3f) / mass;  // a strong arm: a person swings, a cow lifts slowly
  const f32 l = kern::length(dv);
  if (l > cap) dv = dv * (cap / l);
  w.set_linear_velocity(h.grab, v + dv);
  w.set_angular_velocity(h.grab, w.angular_velocity(h.grab) * 0.9f);
  w.set_awake(h.grab, true);
  // a held person: the rest of the doll follows the chest (a limp body, not a chest alone)
  if (h.grab_person >= 0 && glife_)
    for (u8 b = 0; b < Ragdolls::kBones; ++b) {
      if (b == Ragdolls::chest) continue;
      const kern::physics::Body bb = glife_->life.ragdolls().bone_body(static_cast<u64>(h.grab_person) + 1, static_cast<Ragdolls::Bone>(b));
      if (valid(bb)) { const Vec3 bv = w.linear_velocity(bb); w.set_linear_velocity(bb, bv + (v + dv - bv) * 0.25f); w.set_awake(bb, true); }
    }
}

void Game::vr_menu_input(f32 dt) {
  // The wrist menu (best practice: a watch on the off hand, a laser from the other one): Y taps it
  // open and shut, the right hand points, its trigger (or A) picks. Two pages - the game (MULTI,
  // the drone, in / out of a vehicle, the air strike, the radio, the EMP, the settings, RESET, out
  // of VR) and the settings (locomotion, turning, horizon, tunnel, always day, sound, music).
  // Holding Y for a second is RESET from anywhere, menu or not (stuck in a wall, fallen through).
  if (!vr_ || !engine_ || !engine_->xr) { vr_menu_open_ = false; vr_y_held_ = 0; return; }
  const bool y = pad.connected && pad.down(kPadY);
  if (y) {
    vr_y_held_ += dt;
    if (vr_y_held_ >= kResetHold && !vr_y_fired_) {
      vr_y_fired_ = true;
      ui_reset();
      vr_menu_open_ = false;
      vr_menu_page_ = 0;
      toast_ = "RESET";
      toast_age_ = 0;
      vr_pulse_[0] = 0.8f;
    }
  } else {
    if (vr_y_held_ > 0 && !vr_y_fired_) { vr_menu_open_ = !vr_menu_open_; vr_menu_page_ = 0; }  // a tap
    vr_y_held_ = 0;
    vr_y_fired_ = false;
  }
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
        // the canvas (host.mjs vrHudDraw): the title to kMenuTop px of 384, then the rows
        const f32 py = v * 384;
        if (u >= 0 && u <= 1 && py >= kMenuTop && py < kMenuTop + kMenuRows * kMenuRow) vr_menu_hover_ = static_cast<i32>((py - kMenuTop) / kMenuRow);
      }
    }
  }
  if (vr_menu_hover_ != vr_menu_hover_was_ && vr_menu_hover_ >= 0) vr_pulse_[1] = std::fmax(vr_pulse_[1], 0.12f);  // a tick under the finger
  vr_menu_hover_was_ = vr_menu_hover_;
  // the right stick up / down steps through the rows as well (no pointing needed)
  {
    const f32 ry = pad.connected ? pad.ry : 0;
    if (std::fabs(ry) < 0.3f) vr_menu_stick_armed_ = true;
    if (vr_menu_stick_armed_ && std::fabs(ry) > 0.7f) {
      vr_menu_stick_armed_ = false;
      vr_menu_sel_ = (vr_menu_sel_ + (ry > 0 ? kMenuRows - 1 : 1)) % kMenuRows;
    }
    if (vr_menu_hover_ >= 0) vr_menu_sel_ = vr_menu_hover_;
    else vr_menu_hover_ = vr_menu_sel_;
  }
  const bool pick = pad.connected && ((pad.rt > 0.6f && !vr_menu_trigger_) || (pad.pressed & (1u << kPadA)));
  vr_menu_trigger_ = pad.connected && pad.rt > 0.6f;
  if (pad.connected && (pad.pressed & (1u << kPadB))) {  // B: back / close
    if (vr_menu_page_ == 1) vr_menu_page_ = 0;
    else vr_menu_open_ = false;
    return;
  }
  if (!pick || vr_menu_hover_ < 0) return;
  vr_pulse_[1] = 0.35f;
  auto flip = [&](const char* key, Opt o) { command(std::string("set ") + key + (ui_.settings.on(o) ? " 0" : " 1")); };
  if (vr_menu_page_ == 1) {
    switch (vr_menu_hover_) {
      case 0: vr_menu_page_ = 0; break;
      case 1: flip("vr.arms", Opt::vr_arms); break;
      case 2: flip("vr.smoothTurn", Opt::vr_smooth_turn); break;
      case 3: flip("vr.level", Opt::vr_level); break;
      case 4: flip("vr.tunnel", Opt::vr_tunnel); break;
      case 5: flip("world.alwaysDay", Opt::world_always_day); break;
      case 6: flip("audio.on", Opt::audio_on); break;
      case 7: flip("music.on", Opt::music_on); break;
      default: vr_action_ = "radio_off"; break;
    }
    return;
  }
  switch (vr_menu_hover_) {
    case 0: vr_action_ = "multi"; break;
    case 1: toggle_drone(); vr_menu_open_ = false; break;
    case 2:  // in / out: the nearest car or the jet, or out of the one you sit in
      if (mode_ == Mode::foot) ctl_enter(false);
      else if (mode_ == Mode::car || mode_ == Mode::jet) exit_vehicle();
      vr_menu_open_ = false;
      break;
    case 3: {
      // the air strike on what you look at (the first hit along the head's ray, else 180 m out)
      const Vec3 eye = engine_->xr_eye, f = engine_->xr_forward;
      const RayResult rr = cast(world_->physics(), eye, f, 1500, body(soldier_));
      Vec3 at = rr.hit ? rr.point : eye + f * 180.0f;
      at.z = city_.ground_height(at.x, at.y);
      osfx("fighter_jets.beep", at, 0);  // the original's place() and confirmStrike() beeps
      if (!air_strike(at)) { toast_ = "LUFTSCHLAG LAEDT NOCH"; toast_age_ = 0; osfx("fighter_jets.beep", at, 1); }
      else osfx("fighter_jets.beep", at, 5);
      vr_menu_open_ = false;
      break;
    }
    case 4: vr_action_ = "radio"; break;  // the next station (car, jet, drone)
    case 5:  // on foot the EMP, in a vehicle its view (cockpit / outside, the drone's three)
      if (mode_ == Mode::foot) { emp_pulse(); vr_menu_open_ = false; }
      else vr_cam_request_ = true;
      break;
    case 6: vr_menu_page_ = 1; break;
    case 7: ui_reset(); vr_menu_open_ = false; break;
    default: vr_action_ = "exit"; vr_menu_open_ = false; break;
  }
}

std::string Game::vr_menu_json() {
  char b[400];
  const Settings& st = ui_.settings;
  std::snprintf(b, sizeof b,
                ",\"vr\":{\"menu\":%d,\"page\":%d,\"hover\":%d,\"arms\":%d,\"smooth\":%d,\"level\":%d,\"tunnel\":%d,\"day\":%d,\"sound\":%d,\"music\":%d,\"rocket\":%d,"
                "\"reset\":%.2f,\"pulse\":[%.2f,%.2f],\"near\":%d,\"cam\":\"%s\",\"action\":\"%s\"}",
                vr_menu_open_ ? 1 : 0, vr_menu_page_, vr_menu_hover_, st.on(Opt::vr_arms) ? 1 : 0, st.on(Opt::vr_smooth_turn) ? 1 : 0, st.on(Opt::vr_level) ? 1 : 0,
                st.on(Opt::vr_tunnel) ? 1 : 0, st.on(Opt::world_always_day) ? 1 : 0, st.on(Opt::audio_on) ? 1 : 0, st.on(Opt::music_on) ? 1 : 0, ctl_.rocket ? 1 : 0,
                vr_y_fired_ ? 1.0f : clampf(vr_y_held_ / kResetHold, 0, 1), vr_pulse_[0], vr_pulse_[1], vr_near_vehicle_ ? 1 : 0,
                mode_ == Mode::drone ? (drone_cam_ == 0 ? "FPV" : drone_cam_ == 1 ? "FOLGEN" : "AUSSEN") : mode_ == Mode::car ? (car_cockpit_ ? "INNEN" : "AUSSEN")
                : mode_ == Mode::jet ? (jet_cockpit_ ? "COCKPIT" : "AUSSEN") : "", vr_action_.c_str());
  vr_action_.clear();
  vr_pulse_[0] = vr_pulse_[1] = 0;
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

void Game::vr_room(Vec3& f, Vec3& r, Vec3& up) const {
  // what kern_app_frame_xr will use this frame: the body's heading (less the head's yaw on foot),
  // or - in a cockpit with the horizon free - the craft's whole attitude
  f = cam_.target - cam_.eye;
  f.z = 0;
  f = kern::normalize(f, {0, 1, 0});
  up = {0, 0, 1};
  if (engine_ && engine_->xr_follow_head) {
    const f32 a = -engine_->xr_head_yaw * kDeg, c = std::cos(a), s = std::sin(a);
    f = kern::normalize(Vec3{f.x * c - f.y * s, f.x * s + f.y * c, 0}, {0, 1, 0});
  } else if (engine_ && engine_->xr_full_room) {
    const Vec3 ff = kern::normalize(cam_.target - cam_.eye, f);
    const Vec3 rr = kern::normalize(kern::cross(ff, cam_.up), Vec3{f.y, -f.x, 0});
    f = ff;
    up = kern::cross(rr, ff);
  }
  r = kern::normalize(kern::cross(f, up), Vec3{f.y, -f.x, 0});
}

Vec3 Game::vr_room_forward() const {
  Vec3 f, r, up;
  vr_room(f, r, up);
  return f;
}

bool Game::vr_hand_matrix(i32 side, Mat4& out) const {
  if (!vr_ || !engine_ || !engine_->xr || side < 0 || side > 1 || !vr_hands_[side].valid) return false;
  const VrHand& h = vr_hands_[side];
  Vec3 f, r, up;
  vr_room(f, r, up);
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
  if (ui_.settings.on(Opt::vr_smooth_turn)) {
    // smooth turn (a setting): up to 120 deg/s, a dead zone of .2, the curve soft near the centre
    const f32 a = std::fabs(rx) < 0.2f ? 0 : (std::fabs(rx) - 0.2f) / 0.8f;
    turn -= (rx > 0 ? 1 : -1) * a * a * 120 * dt;
  } else if (vr_snap_armed_ && std::fabs(rx) > 0.7f) { vr_snap_armed_ = false; turn += rx > 0 ? -kSnapDeg : kSnapDeg; }
  p.turn_deg = turn;
  // the weapon on the right stick, up / down (the turn has its sideways half): one step per flick,
  // again after it came back under .3 - the hand stays on the grip, the trigger stays free
  {
    const f32 ry = pad.connected ? pad.ry : 0;  // up-positive
    if (std::fabs(ry) < 0.3f) vr_wpn_armed_ = true;
    if (vr_wpn_armed_ && std::fabs(ry) > 0.75f && std::fabs(ry) > std::fabs(rx)) {
      vr_wpn_armed_ = false;
      cycle_foot_weapon(ry > 0 ? 1 : -1);
      ctl_.scope_on = false;
    }
  }
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
    // the left arm sets the pace - jogging walks, pumping it harder runs faster - so the right hand
    // stays on the gun, aims and fires while running; both arms count too, the left stick's click
    // held runs
    const bool left = L.valid && L.swing > kSwingWalk, right = R.valid && R.swing > kSwingWalk;
    forward = 0;
    strafe = 0;
    p.boost = false;
    if (left) {
      // the pace is the arms' power: jogging walks (0.4 .. 1 of the walk), pumping harder runs -
      // up to 1.6 x the walk (8.8 m/s) for a hard sprint; both arms count with their mean. Not
      // Kern's sprint (that lowers the gun): you can aim and fire at a run.
      const f32 sw = right ? std::fmax(L.swing, (L.swing + R.swing) * 0.5f) : L.swing;
      forward = clampf((sw - kSwingWalk) / (kSwingFull - kSwingWalk) * 0.6f + 0.4f, 0.4f, 1);
      f32 run = clampf((sw - kSwingFull) / (kSwingSprint - kSwingFull), 0, 1);
      if (pad.connected && pad.down(kPadL3)) run = std::fmax(run, 0.6f);
      p.speed_scale = 1 + 0.6f * run;
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
    // the jet's cockpit and the FPV drone with the horizon free (VR: Horizont halten off): you sit
    // in the craft, it is the rest frame - the world turns round it, loops and rolls included
    engine_->xr_full_room = vr_ && !player_dead_ && !ui_.settings.on(Opt::vr_level) &&
                            ((mode_ == Mode::jet && jet_cockpit_) || (mode_ == Mode::drone && drone_cam_ == 0));
  }
}

void Game::vr_comfort(f32 dt) {
  // The comfort tunnel (Meta's locomotion guidance): the view's rim darkens while the game - not
  // the player's own head - turns or speeds the view: a stick turn, a snap, a car taking a corner,
  // a drone braking. Turning your head, walking in the room, cruising straight leave it open.
  if (!engine_) return;
  kern::Renderer& r = engine_->renderer;
  const bool on = vr_ && engine_->xr && ui_.settings.on(Opt::vr_tunnel) && !player_dead_;
  if (!on) {
    vr_tunnel_ = 0;
    vr_prev_ok_ = false;
    engine_->xr_vignette = 0;
    return;
  }
  if (!vr_tunnel_mesh_) {
    // a black ring: see-through inside radius 1, opaque from 2 to 8 (the texture's u is the radius)
    if (!vr_tunnel_tex_) {
      u8 px[64 * 2 * 4];
      for (int y = 0; y < 2; ++y)
        for (int x = 0; x < 64; ++x) {
          const f32 t = static_cast<f32>(x) / 63, a = t * t * (3 - 2 * t);
          u8* p = px + (y * 64 + x) * 4;
          p[0] = p[1] = p[2] = 0;
          p[3] = static_cast<u8>(a * 255 + 0.5f);
        }
      kern::TextureDesc td;
      td.rgba = px;
      td.width = 64;
      td.height = 2;
      td.mipmaps = false;
      vr_tunnel_tex_ = r.create_texture(td);
      if (!vr_tunnel_tex_) return;
    }
    kern::MeshData d;
    const Vec3 n{0, -1, 0};
    constexpr int kSeg = 40;
    const f32 radii[3] = {1, 2, 8}, us[3] = {0.5f / 64, 1 - 0.5f / 64, 1 - 0.5f / 64};
    for (int i = 0; i <= kSeg; ++i) {
      const f32 a = static_cast<f32>(i) / kSeg * 2 * kPi, c = std::cos(a), sn = std::sin(a);
      for (int k = 0; k < 3; ++k) d.push({c * radii[k], 0, sn * radii[k]}, n, {0, 0, 0}, {us[k], 0.5f});
      if (i > 0) {
        const u32 b = static_cast<u32>((i - 1) * 3), e = static_cast<u32>(i * 3);
        for (u32 k = 0; k < 2; ++k) d.indices.insert(d.indices.end(), {b + k, e + k, e + k + 1, b + k, e + k + 1, b + k + 1});
      }
    }
    vr_tunnel_mesh_ = r.allocate_id();
    kern::MeshInput in = d.as_input(kern::kMaxExtensionAttributes);
    in.material.unlit = true;
    in.material.double_sided = true;
    in.material.cast_shadow = false;
    in.material.frustum_culled = false;
    in.material.alpha = kern::AlphaMode::blend;
    in.texture = vr_tunnel_tex_;
    if (!r.upsert(vr_tunnel_mesh_, in)) { r.release_id(vr_tunnel_mesh_); vr_tunnel_mesh_ = 0; return; }
    r.set_visible(vr_tunnel_mesh_, false);
    engine_->xr_vignette_mesh = vr_tunnel_mesh_;
  }
  // this frame's room and the camera (the room's anchor) against the last
  Vec3 f, rt, up;
  vr_room(f, rt, up);
  const Vec3 eye = cam_.eye;
  f32 want = 0;
  if (vr_prev_ok_ && dt > 1e-4f) {
    const f32 turn = std::acos(clampf(kern::dot(f, vr_prev_f_), -1, 1)) + std::acos(clampf(kern::dot(up, vr_prev_up_), -1, 1));
    const f32 rate = turn / dt / kDeg;  // deg/s the game turns the room
    const Vec3 v = (eye - vr_prev_eye_) * (1 / dt);
    const f32 jump = kern::length(eye - vr_prev_eye_);
    if (jump < 4) {  // not a respawn, a seat change, a teleport
      const f32 acc = kern::length(v - vr_prev_v_) / dt;
      const f32 speed = kern::length(Vec3{v.x, v.y, 0});
      want = std::fmax(want, clampf((rate - 25) / 110, 0, 1));
      want = std::fmax(want, clampf((acc - 4) / 18, 0, 1) * 0.8f);
      // on foot the stick walk itself (a constant artificial motion): a light rim while moving
      if (mode_ == Mode::foot) want = std::fmax(want, clampf((speed - 0.8f) / 6, 0, 1) * 0.55f);
    }
    vr_prev_v_ = v;
  }
  vr_prev_f_ = f;
  vr_prev_up_ = up;
  vr_prev_eye_ = eye;
  vr_prev_ok_ = true;
  // in fast (80 ms), out slow (350 ms): no pumping
  const f32 tau = want > vr_tunnel_ ? 0.08f : 0.35f;
  vr_tunnel_ += (want - vr_tunnel_) * (1 - std::exp(-dt / tau));
  engine_->xr_vignette = vr_tunnel_ < 0.02f ? 0 : vr_tunnel_;
}

}  // namespace sc
