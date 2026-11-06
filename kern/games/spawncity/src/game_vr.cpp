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

namespace sc {

using kern::Mat4;

namespace {
constexpr f32 kSnapDeg = 30;          // snap turn
constexpr f32 kSwingWalk = 0.55f;     // m/s of hand speed (smoothed) that counts as a jogging arm
constexpr f32 kSwingFull = 1.4f;      // ... and the full pace
constexpr f32 kSwingTau = 0.22f;      // s: smoothing of the arm speed
}  // namespace

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

void Game::vr_update_view() {
  // the first-person gun in the right hand (Kern's soldier), or back in front of the camera
  Mat4 m;
  if (vr_ && mode_ == Mode::foot && vr_hand_matrix(1, m)) world_->set_view_hand(&m);
  else world_->set_view_hand(nullptr);
  if (engine_) engine_->xr_follow_head = vr_ && mode_ == Mode::foot && !player_dead_;
}

}  // namespace sc
