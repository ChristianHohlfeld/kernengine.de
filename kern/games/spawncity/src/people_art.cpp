// Procedural people (see people_art.h).
#include "people_art.h"

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
  // Pivots: a leg hangs from the hip (its top at the origin), an arm from the shoulder.
  MeshData leg;
  prim::append(leg, prim::box({0.15f, 0.15f, 0.86f}, white), at({0, 0, -0.43f}));
  prim::append(leg, prim::box({0.24f, 0.13f, 0.08f}, {0.12f, 0.1f, 0.09f}), at({0.05f, 0, -0.86f}));  // shoe
  if (!make(r, leg_, leg, mat(0.85f), true)) return false;
  MeshData torso;
  prim::append(torso, prim::box({0.24f, 0.42f, 0.6f}, white), at({0, 0, 0.3f}));
  if (!make(r, torso_, torso, mat(0.8f), true)) return false;
  MeshData arm;
  prim::append(arm, prim::box({0.11f, 0.11f, 0.62f}, white), at({0, 0, -0.31f}));
  if (!make(r, arm_, arm, mat(0.8f), true)) return false;
  if (!make(r, head_, prim::sphere(0.115f, 12, 8, white), mat(0.6f), true)) return false;
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
  prim::append(pd, prim::box({0.42f, 0.26f, 0.12f}, {0.08f, 0.1f, 0.16f}), Mat4{});
  prim::append(pd, prim::box({0.9f, 0.05f, 0.04f}, {0.12f, 0.12f, 0.13f}), rot({0, 0, 1}, kPi / 4));
  prim::append(pd, prim::box({0.9f, 0.05f, 0.04f}, {0.12f, 0.12f, 0.13f}), rot({0, 0, 1}, -kPi / 4));
  prim::append(pd, prim::box({0.1f, 0.3f, 0.03f}, {0.85f, 0.86f, 0.9f}), at({0, 0, 0.075f}));  // white stripe
  if (!make(r, pd_body_, pd, mat(0.45f, 0.3f), false)) return false;
  kern::Material disc = mat(0.6f);
  disc.color = {1, 1, 1, 0.5f};
  disc.cast_shadow = false;
  if (!make(r, pd_rotor_, prim::cylinder(0.15f, 0.01f, 14, {0.25f, 0.25f, 0.27f}), disc, false)) return false;
  MeshData shell;
  prim::append(shell, prim::capsule(1.0f, 2.0f, 10, 5, {0.25f, 0.27f, 0.22f}), Mat4{});
  if (!make(r, shell_, shell, mat(0.6f, 0.3f), false)) return false;
  return true;
}

void PeopleArt::begin() {
  for (Part* p : {&leg_, &torso_, &arm_, &head_, &cap_, &bar_, &lamp_, &flame_, &pd_body_, &pd_rotor_, &shell_}) {
    p->transforms.clear();
    p->tints.clear();
  }
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
  if (hands_up) {
    put(arm_, shoulder * at({0, 0.27f, 0}) * rot({1, 0, 0}, 2.8f), p.shirt);
    put(arm_, shoulder * at({0, -0.27f, 0}) * rot({1, 0, 0}, -2.8f), p.shirt);
  } else if (p.aiming && !down) {
    // Both arms forward, the pistol at eye height.
    put(arm_, shoulder * at({0, 0.2f, 0}) * rot({0, 1, 0}, -1.45f) * rot({1, 0, 0}, -0.25f), p.shirt);
    put(arm_, shoulder * at({0, -0.2f, 0}) * rot({0, 1, 0}, -1.45f) * rot({1, 0, 0}, 0.25f), p.shirt);
  } else {
    put(arm_, shoulder * at({0, 0.27f, 0}) * rot({0, 1, 0}, -swing * 0.9f), p.shirt);
    put(arm_, shoulder * at({0, -0.27f, 0}) * rot({0, 1, 0}, swing * 0.9f), p.shirt);
  }
  const Mat4 head = body * at({0, 0, 0.73f});
  put(head_, head, p.skin);
  if (p.kind == People::Kind::officer) put(cap_, head * at({0, 0, 0.06f}));
}

void PeopleArt::light_bar(const Mat4& chassis, bool on, f32 now) {
  const Mat4 m = chassis * at({-0.2f, 0, 0.86f});
  put(bar_, m);
  // Alternating blue / red at 2.5 Hz (police_ground_units.mjs light bar).
  const bool phase = std::fmod(now * 2.5f, 1.0f) < 0.5f;
  const Vec3 dim{0.12f, 0.12f, 0.14f};
  put(lamp_, m * at({0, 0.3f, 0.03f}), on ? (phase ? Vec3{0.1f, 0.25f, 2.6f} : Vec3{0.05f, 0.08f, 0.5f}) : dim);
  put(lamp_, m * at({0, -0.3f, 0.03f}), on ? (!phase ? Vec3{2.6f, 0.08f, 0.06f} : Vec3{0.5f, 0.04f, 0.03f}) : dim);
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

void PeopleArt::end() {
  for (Part* p : {&leg_, &torso_, &arm_, &head_, &cap_, &bar_, &lamp_, &flame_, &pd_body_, &pd_rotor_, &shell_}) {
    if (!p->id) continue;
    if (p->transforms.empty()) { r_->set_visible(p->id, false); continue; }
    r_->update_instances(p->id, kern::Span<const f32>(p->transforms.data(), p->transforms.size()),
                         kern::Span<const f32>(p->tints.data(), p->tints.size()));
    r_->set_visible(p->id, true);
  }
}

}  // namespace sc
