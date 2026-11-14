// Procedural people (see people_art.h).
#include "people_art.h"

#include "weapons.h"

#include <cmath>

#include "kern/geo/primitives.h"

namespace sc {

namespace {
using kern::Mat4;
using kern::MeshData;
namespace prim = kern::primitives;
Mat4 rot(Vec3 axis, f32 a) { return Mat4::trs({}, Quat::axis_angle(axis, a), {1, 1, 1}); }
Mat4 at(Vec3 p) { return Mat4::translate(p); }
kern::Material mat(f32 roughness, f32 metalness = 0, Vec3 emissive = {}) {
  kern::Material m;
  m.roughness = roughness;
  m.metalness = metalness;
  m.emissive = emissive;
  return m;
}
}  // namespace

bool PeopleArt::make(kern::Renderer& r, Part& p, const MeshData& mesh, const kern::Material& m, bool tinted) {
  p.id = r.allocate_id();
  p.tinted = tinted;
  kern::MeshInput in = mesh.as_input(kern::kMaxExtensionAttributes);
  in.material = m;
  in.visible = false;
  static const Mat4 parked = Mat4::translate({0, 0, -1000});
  in.transforms = kern::Span<const f32>(parked.m, 16);
  if (!r.upsert(p.id, in)) { r.release_id(p.id); p.id = 0; return false; }
  return true;
}

bool PeopleArt::init(kern::Renderer& r) {
  r_ = &r;
  const Vec3 white{1, 1, 1};
  // Pivots: a leg hangs from the hip (its top at the origin), an arm from the shoulder. The same
  // proportions as before (person_hit.h hits this pose), now rounded: thigh and shin, a shoe with a
  // toe cap, half the pelvis with every leg; belly and chest with the shoulders; upper and lower arm
  // with a hand; a head with neck, nose, eyes, ears and hair.
  const Vec3 shoe{0.12f, 0.1f, 0.09f}, sole{0.05f, 0.05f, 0.05f};
  auto scl = [](Vec3 s) { return Mat4::scale(s); };
  MeshData leg;
  prim::append(leg, prim::box({0.25f, 0.21f, 0.2f}, white), at({0, 0, -0.04f}));                       // half the pelvis
  prim::append(leg, prim::capsule(0.088f, 0.34f, 12, 6, white), at({0, 0, -0.23f}));                   // thigh
  prim::append(leg, prim::capsule(0.07f, 0.36f, 12, 6, white), at({0.005f, 0, -0.62f}));               // shin
  prim::append(leg, prim::box({0.22f, 0.12f, 0.075f}, shoe), at({0.035f, 0, -0.875f}));                  // shoe
  prim::append(leg, prim::sphere(0.062f, 10, 6, shoe), at({0.14f, 0, -0.88f}) * scl({1.1f, 0.95f, 0.62f}));  // toe cap
  prim::append(leg, prim::box({0.27f, 0.13f, 0.02f}, sole), at({0.05f, 0, -0.915f}));                    // sole
  if (!make(r, leg_, leg, mat(0.85f), true)) return false;
  MeshData torso;
  prim::append(torso, prim::capsule(0.12f, 0.16f, 14, 6, white), at({0, 0, 0.2f}) * scl({0.95f, 1.6f, 1.0f}));   // belly
  prim::append(torso, prim::capsule(0.13f, 0.14f, 14, 6, white), at({0.005f, 0, 0.42f}) * scl({0.98f, 1.62f, 1.0f}));  // chest
  for (f32 y : {0.225f, -0.225f}) prim::append(torso, prim::sphere(0.066f, 10, 6, white), at({0, y, 0.535f}) * scl({1.0f, 1.0f, 0.85f}));     // shoulders
  prim::append(torso, prim::box({0.2f, 0.36f, 0.05f}, {0.55f, 0.55f, 0.55f}), at({0, 0, 0.03f}));                 // the belt (darker)
  if (!make(r, torso_, torso, mat(0.8f), true)) return false;
  MeshData arm;
  prim::append(arm, prim::capsule(0.056f, 0.24f, 10, 6, white), at({0, 0, -0.15f}));   // upper arm
  prim::append(arm, prim::capsule(0.047f, 0.22f, 10, 6, white), at({0, 0, -0.42f}));   // forearm
  if (!make(r, arm_, arm, mat(0.8f), true)) return false;
  MeshData hand;
  prim::append(hand, prim::sphere(0.052f, 10, 6, white), at({0, 0, -0.6f}) * scl({0.75f, 0.55f, 1.15f}));
  if (!make(r, hand_, hand, mat(0.6f), true)) return false;
  MeshData head_mesh;
  prim::append(head_mesh, prim::sphere(0.115f, 16, 10, white), scl({0.94f, 0.88f, 1.05f}));
  prim::append(head_mesh, prim::cylinder(0.052f, 0.13f, 10, white), at({-0.01f, 0, -0.205f}));                   // neck
  prim::append(head_mesh, prim::box({0.045f, 0.03f, 0.05f}, white), at({0.11f, 0, -0.015f}));                    // nose
  for (f32 y : {0.038f, -0.038f}) {
    prim::append(head_mesh, prim::box({0.012f, 0.026f, 0.02f}, {0.06f, 0.05f, 0.05f}), at({0.104f, y, 0.012f}));  // eyes
    prim::append(head_mesh, prim::sphere(0.026f, 6, 4, white), at({-0.005f, y * 2.65f, 0.0f}) * scl({0.6f, 0.5f, 1.0f}));  // ears
  }
  prim::append(head_mesh, prim::box({0.012f, 0.05f, 0.012f}, {0.45f, 0.28f, 0.26f}), at({0.104f, 0, -0.06f}));     // mouth
  if (!make(r, head_, head_mesh, mat(0.6f), true)) return false;
  MeshData hair;
  prim::append(hair, prim::sphere(0.123f, 14, 8, white), at({-0.018f, 0, 0.088f}) * scl({1.0f, 0.95f, 0.5f}));  // the top
  prim::append(hair, prim::sphere(0.118f, 14, 8, white), at({-0.04f, 0, 0.0f}) * scl({0.82f, 0.93f, 1.0f}));      // the back
  if (!make(r, hair_, hair, mat(0.9f), true)) return false;
  // a ragdoll's bones (ragdoll.cpp kDefs, each about its own centre): the standing body's pieces cut
  // at the joints - the belly and the pelvis round, a shoe on each shin, a hand on each forearm
  {
    MeshData m;
    prim::append(m, prim::capsule(0.115f, 0.11f, 14, 6, white), rot({1, 0, 0}, kPi / 2));
    prim::append(m, prim::box({0.2f, 0.34f, 0.05f}, {0.55f, 0.55f, 0.55f}), at({0, 0, 0.08f}));  // the belt
    if (!make(r, rd_pelvis_, m, mat(0.85f), true)) return false;
  }
  {
    MeshData m;
    prim::append(m, prim::capsule(0.12f, 0.16f, 14, 6, white), at({0, 0, -0.1f}) * scl({0.95f, 1.6f, 1.0f}));   // belly
    prim::append(m, prim::capsule(0.13f, 0.12f, 14, 6, white), at({0.005f, 0, 0.1f}) * scl({0.98f, 1.62f, 1.0f}));  // chest
    for (f32 y : {0.225f, -0.225f}) prim::append(m, prim::sphere(0.066f, 10, 6, white), at({0, y, 0.2f}) * scl({1.0f, 1.0f, 0.85f}));
    if (!make(r, rd_chest_, m, mat(0.8f), true)) return false;
  }
  if (!make(r, rd_upper_, prim::capsule(0.056f, 0.22f, 10, 6, white), mat(0.8f), true)) return false;
  {
    MeshData m;
    prim::append(m, prim::capsule(0.047f, 0.22f, 10, 6, white), Mat4{});
    prim::append(m, prim::sphere(0.052f, 10, 6, {0.82f, 0.66f, 0.53f}), at({0, 0, -0.2f}) * scl({0.75f, 0.55f, 1.15f}));  // the hand
    if (!make(r, rd_fore_, m, mat(0.7f), true)) return false;
  }
  if (!make(r, rd_thigh_, prim::capsule(0.088f, 0.32f, 12, 6, white), mat(0.85f), true)) return false;
  {
    MeshData m;
    prim::append(m, prim::capsule(0.07f, 0.34f, 12, 6, white), Mat4{});
    prim::append(m, prim::box({0.22f, 0.12f, 0.075f}, shoe), at({0.035f, 0, -0.205f}));
    prim::append(m, prim::sphere(0.062f, 10, 6, shoe), at({0.14f, 0, -0.21f}) * scl({1.1f, 0.95f, 0.62f}));
    prim::append(m, prim::box({0.27f, 0.13f, 0.02f}, sole), at({0.05f, 0, -0.245f}));
    if (!make(r, rd_shin_, m, mat(0.85f), true)) return false;
  }
  {
    // a mate's beacon (MP): a unit column, unlit and glowing in his colour
    kern::Material bm = mat(0.3f, 0, {1.6f, 1.6f, 1.6f});
    bm.unlit = true;
    bm.cast_shadow = false;
    if (!make(r, beacon_, prim::cylinder(1.0f, 1.0f, 10, white), bm, true)) return false;
  }
  // the weapons a mate carries (seen in third person, MP): built along +x from the grip
  {
    const Vec3 gm{0.09f, 0.09f, 0.1f}, gd{0.2f, 0.21f, 0.23f}, ol{0.26f, 0.3f, 0.2f};
    MeshData m;
    prim::append(m, prim::box({0.17f, 0.035f, 0.045f}, gm), at({0.06f, 0, 0.03f}));    // slide
    prim::append(m, prim::box({0.04f, 0.032f, 0.1f}, gd), at({-0.005f, 0, -0.03f}) * rot({0, 1, 0}, -0.25f));  // grip
    if (!make(r, gun_pistol_, m, mat(0.45f, 0.4f), false)) return false;
    MeshData s2;
    prim::append(s2, prim::box({0.42f, 0.05f, 0.08f}, gm), at({0.13f, 0, 0.03f}));      // body and barrel
    prim::append(s2, prim::box({0.045f, 0.04f, 0.16f}, gd), at({0.12f, 0, -0.07f}));    // magazine
    prim::append(s2, prim::box({0.04f, 0.035f, 0.1f}, gd), at({-0.01f, 0, -0.03f}) * rot({0, 1, 0}, -0.25f));
    prim::append(s2, prim::box({0.16f, 0.04f, 0.06f}, gd), at({-0.14f, 0, 0.02f}));     // stock
    if (!make(r, gun_smg_, s2, mat(0.45f, 0.4f), false)) return false;
    MeshData sn;
    prim::append(sn, prim::cylinder(0.012f, 0.75f, 8, gm), at({0.18f, 0, 0.04f}) * rot({0, 1, 0}, kPi / 2));  // barrel
    prim::append(sn, prim::box({0.42f, 0.055f, 0.08f}, ol), at({0.0f, 0, 0.02f}));      // body
    prim::append(sn, prim::cylinder(0.025f, 0.26f, 10, gd), at({-0.06f, 0, 0.1f}) * rot({0, 1, 0}, kPi / 2));  // scope
    prim::append(sn, prim::box({0.04f, 0.035f, 0.1f}, gd), at({-0.03f, 0, -0.04f}) * rot({0, 1, 0}, -0.25f));
    prim::append(sn, prim::box({0.22f, 0.045f, 0.09f}, ol), at({-0.3f, 0, 0.0f}));      // stock
    if (!make(r, gun_sniper_, sn, mat(0.5f, 0.3f), false)) return false;
    MeshData gl;
    prim::append(gl, prim::cylinder(0.045f, 0.42f, 12, ol), at({-0.08f, 0, 0.05f}) * rot({0, 1, 0}, kPi / 2));  // the 40 mm tube
    prim::append(gl, prim::box({0.04f, 0.035f, 0.1f}, gd), at({0.0f, 0, -0.03f}) * rot({0, 1, 0}, -0.25f));
    prim::append(gl, prim::box({0.18f, 0.05f, 0.08f}, gd), at({-0.2f, 0, 0.02f}));
    if (!make(r, gun_tube_, gl, mat(0.55f, 0.2f), false)) return false;
    MeshData rk;
    prim::append(rk, prim::cylinder(0.06f, 1.0f, 12, ol), at({-0.45f, 0, 0.09f}) * rot({0, 1, 0}, kPi / 2));   // over the shoulder
    prim::append(rk, prim::cylinder(0.075f, 0.12f, 12, gd), at({0.43f, 0, 0.09f}) * rot({0, 1, 0}, kPi / 2));  // the muzzle ring
    prim::append(rk, prim::box({0.04f, 0.035f, 0.1f}, gd), at({0.0f, 0, -0.01f}) * rot({0, 1, 0}, -0.25f));
    if (!make(r, gun_rocket_, rk, mat(0.6f, 0.15f), false)) return false;
    MeshData kn;
    prim::append(kn, prim::box({0.17f, 0.006f, 0.032f}, {0.75f, 0.77f, 0.8f}), at({0.11f, 0, 0.0f}));
    prim::append(kn, prim::box({0.1f, 0.025f, 0.035f}, gd), at({-0.02f, 0, 0.0f}));
    if (!make(r, gun_knife_, kn, mat(0.3f, 0.7f), false)) return false;
  }
  // police_ground_units.mjs POLICE_COLORS: the cops' black vest over the dark blue shirt
  MeshData vest;
  prim::append(vest, prim::box({0.29f, 0.45f, 0.36f}, {0.082f, 0.094f, 0.118f}), at({0.005f, 0, 0.36f}));
  prim::append(vest, prim::box({0.02f, 0.3f, 0.07f}, {0.85f, 0.86f, 0.88f}), at({0.15f, 0, 0.44f}));  // the light POLIZEI patch on the chest
  if (!make(r, vest_, vest, mat(0.7f), false)) return false;
  // buildCruiser: the white door panels (1.7 x .02 x .42) on the black body
  MeshData door;
  for (f32 y : {0.835f, -0.835f}) prim::append(door, prim::box({1.7f, 0.02f, 0.42f}, {0.933f, 0.941f, 0.949f}), at({-0.05f, y, 0.02f}));
  if (!make(r, door_, door, mat(0.4f, 0.2f), false)) return false;
  MeshData cap;
  prim::append(cap, prim::cylinder(0.125f, 0.07f, 12, {0.08f, 0.1f, 0.16f}), Mat4{});
  prim::append(cap, prim::box({0.12f, 0.2f, 0.015f}, {0.05f, 0.05f, 0.06f}), at({0.12f, 0, 0.005f}));
  if (!make(r, cap_, cap, mat(0.5f), false)) return false;
  MeshData bar;
  prim::append(bar, prim::box({0.26f, 1.1f, 0.09f}, {0.08f, 0.08f, 0.09f}), Mat4{});
  if (!make(r, bar_, bar, mat(0.4f, 0.3f), false)) return false;
  kern::Material lamp = mat(0.3f, 0, {2.4f, 2.4f, 2.4f});
  lamp.unlit = true;
  if (!make(r, lamp_, prim::box({0.22f, 0.42f, 0.1f}, white), lamp, true)) return false;
  kern::Material flame = mat(0.9f, 0, {4.0f, 1.6f, 0.3f});
  flame.unlit = true;
  flame.cast_shadow = false;
  if (!make(r, flame_, prim::sphere(1.0f, 10, 6, white), flame, true)) return false;
  // Police drone: a 0.9 m quad (body, arms, guards) - not the player's airframe.
  MeshData pd;
  // wanted_police_drones.mjs (white-black + red-blue light bar): a white shell, a black belly and
  // arms, motor pods at the tips, a blue band with the light bar on top
  prim::append(pd, prim::capsule(0.12f, 0.2f, 14, 6, {0.9f, 0.91f, 0.93f}), rot({0, 1, 0}, kPi / 2) * Mat4::scale({1, 1.15f, 0.62f}));
  prim::append(pd, prim::box({0.36f, 0.22f, 0.05f}, {0.06f, 0.065f, 0.075f}), at({0, 0, -0.06f}));  // belly
  prim::append(pd, prim::box({0.38f, 0.235f, 0.035f}, {0.12f, 0.3f, 0.85f}), at({0, 0, 0.02f}));   // blue band
  for (f32 a : {kPi / 4, -kPi / 4}) prim::append(pd, prim::box({0.9f, 0.045f, 0.035f}, {0.07f, 0.07f, 0.08f}), rot({0, 0, 1}, a));
  for (int i = 0; i < 4; ++i) {
    const f32 a = kPi / 4 + static_cast<f32>(i) * kPi / 2;
    prim::append(pd, prim::cylinder(0.045f, 0.06f, 10, {0.08f, 0.08f, 0.09f}), at({std::cos(a) * 0.32f, std::sin(a) * 0.32f, -0.02f}));
  }
  prim::append(pd, prim::sphere(0.04f, 10, 6, {0.05f, 0.06f, 0.08f}), at({-0.19f, 0, -0.03f}));  // the camera eye
  if (!make(r, pd_body_, pd, mat(0.35f, 0.05f), false)) return false;
  kern::Material disc = mat(0.6f);
  disc.color = {1, 1, 1, 0.5f};
  disc.cast_shadow = false;
  if (!make(r, pd_rotor_, prim::cylinder(0.15f, 0.01f, 14, {0.25f, 0.25f, 0.27f}), disc, false)) return false;
  MeshData shell;
  prim::append(shell, prim::capsule(1.0f, 2.0f, 10, 5, {0.25f, 0.27f, 0.22f}), Mat4{});
  if (!make(r, shell_, shell, mat(0.6f, 0.3f), false)) return false;
  // Street lamp: a 6 m pole with an arm over the road and a lamp head.
  MeshData pole;
  prim::append(pole, prim::cylinder(0.07f, 6.0f, 8, {0.22f, 0.24f, 0.26f}), Mat4{});
  prim::append(pole, prim::box({1.3f, 0.07f, 0.07f}, {0.22f, 0.24f, 0.26f}), at({0.62f, 0, 5.95f}));
  if (!make(r, pole_, pole, mat(0.5f, 0.6f), false)) return false;
  kern::Material head = mat(0.3f, 0, {3.2f, 2.7f, 1.8f});
  head.unlit = true;
  head.cast_shadow = false;
  if (!make(r, lamp_head_, prim::box({0.42f, 0.2f, 0.08f}, white), head, true)) return false;
  return true;
}

void PeopleArt::street_lamp(Vec3 foot, f32 arm_yaw, f32 night) {
  const Mat4 m = at(foot) * rot({0, 0, 1}, arm_yaw);
  put(pole_, m);
  const f32 k = 0.18f + 0.82f * clampf(night, 0, 1);  // dull by day, bright at night
  put(lamp_head_, m * at({1.2f, 0, 5.88f}), Vec3{k, k * 0.92f, k * 0.75f});
}

void PeopleArt::clear(Part& p) {
  p.transforms.clear();
  p.tints.clear();
}

void PeopleArt::begin() {
  clear(leg_);
  clear(torso_);
  clear(arm_);
  clear(head_);
  clear(hand_);
  clear(hair_);
  clear(vest_);
  clear(door_);
  clear(cap_);
  clear(bar_);
  clear(lamp_);
  clear(flame_);
  clear(pd_body_);
  clear(pd_rotor_);
  clear(shell_);
  clear(pole_);
  clear(lamp_head_);
  clear(rd_pelvis_);
  clear(rd_chest_);
  clear(rd_upper_);
  clear(rd_fore_);
  clear(rd_thigh_);
  clear(rd_shin_);
  clear(beacon_);
  clear(gun_pistol_);
  clear(gun_smg_);
  clear(gun_sniper_);
  clear(gun_tube_);
  clear(gun_rocket_);
  clear(gun_knife_);
}

void PeopleArt::put(Part& p, const Mat4& m, Vec3 tint) {
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  if (p.tinted) p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, 1});
}

void PeopleArt::person(const People::Person& p, f32 now) {
  if (p.state == People::State::gone) return;
  Mat4 base = at(p.pos) * rot({0, 0, 1}, p.yaw);
  const bool down = p.state == People::State::down;
  if (down) {
    // Lying where they fell: the body's up axis along the fall direction, 0.15 m over the ground.
    const f32 fall = std::atan2(p.fall_dir.y, p.fall_dir.x);
    base = at(p.pos + Vec3{0, 0, 0.15f}) * rot({0, 0, 1}, fall) * rot({0, 1, 0}, kPi / 2) * at({0, 0, -0.95f});
  }
  // Gait: legs and arms swing with the pace; running swings wider.
  const f32 pace = down ? 0 : clampf(p.v / 1.4f, 0, 2.2f);
  const f32 swing = std::sin(p.phase) * clampf(0.25f + 0.18f * pace, 0, 0.75f) * (pace > 0.05f ? 1.0f : 0.0f);
  const f32 lean = down ? 0 : clampf(p.v - 2.5f, 0, 5) * 0.035f;
  const Mat4 hip = base * at({0, 0, 0.92f});
  put(leg_, hip * at({0, 0.1f, 0}) * rot({0, 1, 0}, swing), p.pants);
  put(leg_, hip * at({0, -0.1f, 0}) * rot({0, 1, 0}, -swing), p.pants);
  const Mat4 body = hip * rot({0, 1, 0}, lean);
  put(torso_, body, p.shirt);
  const Mat4 shoulder = body * at({0, 0, 0.56f});
  const bool hands_up = !down && p.kind == People::Kind::citizen && now < p.hands_up_until;
  Mat4 arm_l, arm_r;
  if (hands_up) {
    arm_l = shoulder * at({0, 0.27f, 0}) * rot({1, 0, 0}, 2.8f);
    arm_r = shoulder * at({0, -0.27f, 0}) * rot({1, 0, 0}, -2.8f);
  } else if (p.aiming && !down) {
    // Both arms forward along the aim (a mate's view pitch: up and down), the weapon at eye height.
    const f32 ap = clampf(p.aim_pitch, -1.2f, 1.2f);
    arm_l = shoulder * at({0, 0.2f, 0}) * rot({0, 1, 0}, -1.45f + ap) * rot({1, 0, 0}, -0.25f);
    arm_r = shoulder * at({0, -0.2f, 0}) * rot({0, 1, 0}, -1.45f + ap) * rot({1, 0, 0}, 0.25f);
  } else if (p.hands && !down) {
    // a VR mate: each arm from its shoulder to his real hand (the arm 0.6 m: stretched or bent
    // within 0.5 .. 1.15 of it)
    auto reach = [&](f32 side_y, Vec3 hand) {
      const Vec3 s0 = (shoulder * at({0, side_y, 0})).translation();
      const Vec3 d = hand - s0;
      const f32 len = kern::length(d);
      const Vec3 z = len > 1e-3f ? d * (-1.0f / len) : Vec3{0, 0, 1};          // the arm runs along -z
      const Vec3 fwd{std::cos(p.yaw), std::sin(p.yaw), 0};
      Vec3 x = kern::normalize(kern::cross(Vec3{0, 0, 1}, z), fwd);
      if (kern::length(kern::cross(Vec3{0, 0, 1}, z)) < 1e-3f) x = fwd;
      const Vec3 y = kern::cross(z, x);
      const f32 k = clampf(len / 0.6f, 0.5f, 1.15f);
      Mat4 m;
      m.m[0] = x.x; m.m[1] = x.y; m.m[2] = x.z; m.m[3] = 0;
      m.m[4] = y.x; m.m[5] = y.y; m.m[6] = y.z; m.m[7] = 0;
      m.m[8] = z.x * k; m.m[9] = z.y * k; m.m[10] = z.z * k; m.m[11] = 0;
      m.m[12] = s0.x; m.m[13] = s0.y; m.m[14] = s0.z; m.m[15] = 1;
      return m;
    };
    arm_l = reach(0.2f, p.hand_l);
    arm_r = reach(-0.2f, p.hand_r);
  } else if (p.punch && !down) {
    // a mate's blow: the striking arm straight out at the target, the other one guarding
    const bool r = p.punch & 1, l = p.punch & 2, slash = p.punch & 4;
    arm_r = r || slash ? shoulder * at({0, -0.2f, 0}) * rot({0, 1, 0}, slash ? -1.2f : -1.62f) * rot({1, 0, 0}, slash ? 0.6f : 0.12f)
                       : shoulder * at({0, -0.22f, 0}) * rot({0, 1, 0}, -0.9f) * rot({1, 0, 0}, 0.5f);
    arm_l = l ? shoulder * at({0, 0.2f, 0}) * rot({0, 1, 0}, -1.62f) * rot({1, 0, 0}, -0.12f)
              : shoulder * at({0, 0.22f, 0}) * rot({0, 1, 0}, -0.9f) * rot({1, 0, 0}, -0.5f);
  } else {
    arm_l = shoulder * at({0, 0.27f, 0}) * rot({0, 1, 0}, -swing * 0.9f) * rot({1, 0, 0}, 0.06f);
    arm_r = shoulder * at({0, -0.27f, 0}) * rot({0, 1, 0}, swing * 0.9f) * rot({1, 0, 0}, -0.06f);
  }
  put(arm_, arm_l, p.shirt);
  put(arm_, arm_r, p.shirt);
  put(hand_, arm_l, p.skin);
  put(hand_, arm_r, p.skin);
  const Mat4 head = body * at({0, 0, 0.73f}) * rot({0, 1, 0}, down ? 0.0f : clampf(p.aim_pitch, -1.2f, 1.2f) * 0.6f);
  if (p.held >= 0 && !down) {
    // the weapon in the right hand: at the hands where the aiming arms meet, along the aim; held
    // low and forward when not aiming (knife, fists, a launcher on the shoulder)
    using W = weapons::Id;
    const W w = static_cast<W>(p.held);
    const f32 ap = p.aiming || p.hands ? clampf(p.aim_pitch, -1.2f, 1.2f) : 0.5f;
    // a VR mate: the weapon in his real right hand along his aim; else at the meeting hands
    const Mat4 g = p.hands ? at(p.hand_r) * rot({0, 0, 1}, p.yaw) * rot({0, 1, 0}, ap) * at({w == W::rocket ? -0.35f : 0.0f, 0, w == W::rocket ? 0.1f : 0.0f})
                           : shoulder * at({0, -0.06f, 0}) * rot({0, 1, 0}, ap) * at({w == W::rocket ? 0.05f : 0.5f, 0, w == W::rocket ? 0.12f : -0.02f});
    if (w == W::glock) put(gun_pistol_, g);
    else if (w == W::smg) put(gun_smg_, g);
    else if (w == W::sniper) put(gun_sniper_, g);
    else if (w == W::grenade_launcher) put(gun_tube_, g);
    else if (w == W::rocket) put(gun_rocket_, g);
    else if (w == W::knife) put(gun_knife_, g);
  }
  put(head_, head, p.skin);
  put(hair_, head, hair_color(p));
  if (p.kind == People::Kind::officer && !down) put(vest_, body);
  if (p.kind == People::Kind::officer || p.kind == People::Kind::soldier || p.kind == People::Kind::enemy) put(cap_, head * at({0, 0, 0.06f}));
}

Vec3 PeopleArt::hair_color(const People::Person& p) {
  // hair by the person: black, dark and light brown, blond, auburn, grey
  static const Vec3 kHair[6] = {{0.05f, 0.045f, 0.04f}, {0.18f, 0.11f, 0.06f}, {0.36f, 0.24f, 0.13f}, {0.72f, 0.58f, 0.32f}, {0.45f, 0.17f, 0.08f}, {0.55f, 0.55f, 0.56f}};
  if (p.kind == People::Kind::zombie) return {0.2f, 0.22f, 0.18f};
  return kHair[(p.seed >> 5) % 6];
}

void PeopleArt::ragdolls(const std::vector<Ragdolls::Draw>& bones, const Ragdolls& dolls) {
  for (const Ragdolls::Draw& d : bones) {
    const Ragdolls::Colors& c = dolls.colors(d.doll);
    switch (d.bone) {
      case Ragdolls::pelvis: put(rd_pelvis_, d.pose, c.pants); break;
      case Ragdolls::chest:
        put(rd_chest_, d.pose, c.shirt);
        if (c.vest) put(vest_, d.pose * at({0, 0, -0.36f}));
        break;
      case Ragdolls::head: {
        // the head bone sits where the standing head's centre is (the face along +x)
        put(head_, d.pose, c.skin);
        put(hair_, d.pose, c.hair);
        if (c.cap) put(cap_, d.pose * at({0, 0, 0.06f}));
        break;
      }
      case Ragdolls::upper_arm_l: case Ragdolls::upper_arm_r: put(rd_upper_, d.pose, c.shirt); break;
      case Ragdolls::lower_arm_l: case Ragdolls::lower_arm_r: put(rd_fore_, d.pose, c.shirt); break;
      case Ragdolls::thigh_l: case Ragdolls::thigh_r: put(rd_thigh_, d.pose, c.pants); break;
      default: put(rd_shin_, d.pose, c.pants); break;
    }
  }
}

void PeopleArt::beacon(Vec3 foot, Vec3 color, f32 alpha) {
  // from 2.6 m over him 90 m up, 0.18 m thick - thin near, a line far away (tinted, brighter with alpha)
  put(beacon_, at(foot + Vec3{0, 0, 2.6f}) * Mat4::scale({0.18f, 0.18f, 90.0f}), color * (0.6f + 1.4f * alpha));
}

void PeopleArt::light_bar(const Mat4& chassis, bool on, f32 now) {
  const Mat4 m = chassis * at({-0.2f, 0, 0.86f});
  put(bar_, m);
  put(door_, chassis);
  // flashLights: 110 ms slots in sixes - red in 0 and 2, blue in 3 and 5 (police_ground_units.mjs)
  const i32 slot = static_cast<i32>(std::floor(now * 1000 / 110)) % 6;
  const bool red = on && (slot == 0 || slot == 2), blue = on && (slot == 3 || slot == 5);
  put(lamp_, m * at({0, 0.25f, 0.05f}), blue ? Vec3{0.16f, 0.42f, 3.0f} : Vec3{0.024f, 0.063f, 0.18f});
  put(lamp_, m * at({0, -0.25f, 0.05f}), red ? Vec3{3.0f, 0.1f, 0.16f} : Vec3{0.2f, 0.024f, 0.03f});
}

void PeopleArt::fire(Vec3 p, f32 size, f32 now) {
  for (int i = 0; i < 3; ++i) {
    const f32 t = now * (3.1f + static_cast<f32>(i)) + static_cast<f32>(i) * 2.1f;
    const f32 s = size * (0.55f + 0.25f * std::sin(t));
    put(flame_, at(p + Vec3{0.3f * std::sin(t * 1.3f), 0.3f * std::cos(t * 0.9f), s * 0.6f + static_cast<f32>(i) * size * 0.35f}) * Mat4::scale({s, s, s * 1.4f}),
        i == 0 ? Vec3{1.4f, 0.6f, 0.15f} : Vec3{1.0f, 0.35f, 0.08f});
  }
}

void PeopleArt::police_drone(Vec3 p, f32 yaw, f32 tilt_x, f32 tilt_y, bool lights, f32 now, u32 index) {
  const Mat4 m = at(p) * rot({0, 0, 1}, yaw) * rot({0, 1, 0}, tilt_y) * rot({1, 0, 0}, tilt_x);
  put(pd_body_, m);
  for (int i = 0; i < 4; ++i) {
    const f32 a = kPi / 4 + static_cast<f32>(i) * kPi / 2;
    put(pd_rotor_, m * at({std::cos(a) * 0.32f, std::sin(a) * 0.32f, 0.04f}));
  }
  // Strobes: red, then blue, 92 ms slots (wanted_police_drones.mjs).
  const int slot = static_cast<int>(std::floor((now * 1000 + static_cast<f32>(index) * 37) / 92)) % 8;
  const Vec3 off{0.1f, 0.1f, 0.12f};
  put(lamp_, m * at({-0.16f, 0.1f, 0.07f}) * Mat4::scale({0.25f, 0.2f, 0.4f}), lights && (slot == 0 || slot == 1) ? Vec3{3.0f, 0.1f, 0.08f} : off);
  put(lamp_, m * at({-0.16f, -0.1f, 0.07f}) * Mat4::scale({0.25f, 0.2f, 0.4f}), lights && (slot == 4 || slot == 5) ? Vec3{0.1f, 0.3f, 3.0f} : off);
}

void PeopleArt::projectile(Vec3 p, Vec3 dir, f32 radius, f32 length, bool flame) {
  const f32 l = kern::length(dir);
  const Vec3 n = l > 1e-5f ? dir * (1.0f / l) : Vec3{0, 0, 1};
  const Vec3 axis = kern::cross(Vec3{0, 0, 1}, n);
  const f32 ang = std::acos(clampf(n.z, -1, 1));
  const Mat4 m = at(p) * rot(kern::normalize(axis, {1, 0, 0}), ang);
  // The capsule mesh is 4 m long overall (radius 1): scale to size.
  put(shell_, m * Mat4::scale({radius, radius, length / 4}));
  if (flame) put(flame_, m * at({0, 0, -length * 0.6f}) * Mat4::scale({radius * 1.6f, radius * 1.6f, radius * 3.2f}), Vec3{1.6f, 0.7f, 0.2f});
}

void PeopleArt::upload(Part& p) {
  if (!p.id) return;
  if (p.transforms.empty()) { r_->set_visible(p.id, false); return; }
  r_->update_instances(p.id, kern::Span<const f32>(p.transforms), kern::Span<const f32>(p.tints));
  r_->set_visible(p.id, true);
}

void PeopleArt::end() {
  upload(leg_);
  upload(torso_);
  upload(arm_);
  upload(head_);
  upload(hand_);
  upload(hair_);
  upload(vest_);
  upload(door_);
  upload(cap_);
  upload(bar_);
  upload(lamp_);
  upload(flame_);
  upload(pd_body_);
  upload(pd_rotor_);
  upload(shell_);
  upload(pole_);
  upload(lamp_head_);
  upload(rd_pelvis_);
  upload(rd_chest_);
  upload(rd_upper_);
  upload(rd_fore_);
  upload(rd_thigh_);
  upload(rd_shin_);
  upload(beacon_);
  upload(gun_pistol_);
  upload(gun_smg_);
  upload(gun_sniper_);
  upload(gun_tube_);
  upload(gun_rocket_);
  upload(gun_knife_);
}

}  // namespace sc
