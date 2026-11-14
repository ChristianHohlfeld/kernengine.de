// Procedural combat art (see combat_art.h).
#include "combat_art.h"

#include <cmath>

#include "kern/geo/primitives.h"

namespace sc {

namespace {
using kern::Mat4;
namespace prim = kern::primitives;
Mat4 rot(Vec3 axis, f32 a) { return Mat4::trs({}, Quat::axis_angle(axis, a), {1, 1, 1}); }
Mat4 at(Vec3 p) { return Mat4::translate(p); }
Mat4 sc3(f32 x, f32 y, f32 z) { return Mat4::scale({x, y, z}); }
const Vec3 kOlive{0.29f, 0.34f, 0.2f}, kDark{0.13f, 0.15f, 0.11f}, kGlass{0.1f, 0.13f, 0.16f}, kSteel{0.16f, 0.17f, 0.18f}, kSand{0.71f, 0.61f, 0.42f};
const Vec3 kJetMetal{0.36f, 0.39f, 0.42f}, kJetDark{0.16f, 0.18f, 0.21f};
}  // namespace

bool CombatArt::init(kern::Renderer& r) {
  r_ = &r;
  const Vec3 white{1, 1, 1};
  kern::MeshData meshes[kShapes] = {prim::box({1, 1, 1}, white),  prim::sphere(0.5f, 14, 10, white), prim::cylinder(0.5f, 1, 14, white),
                                    prim::cone(0.5f, 1, 14, white), prim::box({1, 1, 1}, white),      prim::sphere(0.5f, 12, 8, white),
                                    prim::sphere(0.5f, 10, 7, white),  prim::sphere(0.5f, 12, 8, white),
                                    prim::torus(0.8f, 0.2f, 24, 6, white)};
  for (int i = 0; i < kShapes; ++i) {
    kern::Material m;
    m.roughness = 0.7f;
    m.metalness = 0.25f;
    if (i == glow_box || i == glow_sphere) {
      m.emissive = {1, 1, 1};
      m.unlit = true;
      m.cast_shadow = false;
    }
    if (i == fade_sphere || i == fade_ring) {
      m.emissive = {1, 1, 1};
      m.unlit = true;
      m.cast_shadow = false;
      m.double_sided = true;
      m.alpha = kern::AlphaMode::blend;
    }
    if (i == smoke) {
      m.roughness = 1;
      m.metalness = 0;
      m.cast_shadow = false;
    }
    Part& p = parts_[i];
    p.id = r.allocate_id();
    kern::MeshLook look;
    look.material = m;
    look.extra_slot = kern::kMaxExtensionAttributes;
    look.visible = false;
    const Mat4 parked = Mat4::translate({0, 0, -1000});
    if (!kern::upsert(r, p.id, meshes[i], look, kern::Span<const f32>(parked.m, 16))) { r.release_id(p.id); p.id = 0; return false; }
  }
  return true;
}

void CombatArt::begin() {
  for (Part& p : parts_) { p.transforms.clear(); p.tints.clear(); }
}

void CombatArt::put(Shape s, const Mat4& m, Vec3 tint) {
  Part& p = parts_[s];
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, 1});
}

void CombatArt::put(Shape s, const Mat4& m, Vec3 tint, f32 alpha) {
  Part& p = parts_[s];
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, clampf(alpha, 0, 1)});
}

void CombatArt::beam(Vec3 a, Vec3 b, f32 width, Vec3 tint) {
  const Vec3 d = b - a;
  const f32 l = kern::length(d);
  if (l < 0.05f) return;
  const Vec3 n = d * (1.0f / l);
  const Vec3 axis = kern::cross(Vec3{0, 0, 1}, n);
  const f32 ang = std::acos(clampf(n.z, -1, 1));
  put(glow_box, at((a + b) * 0.5f) * rot(kern::normalize(axis, {1, 0, 0}), ang) * sc3(width, width, l), tint);
}

void CombatArt::end() {
  for (Part& p : parts_) {
    if (!p.id) continue;
    if (p.transforms.empty()) { r_->set_visible(p.id, false); continue; }
    r_->update_instances(p.id, kern::Span<const f32>(p.transforms.data(), p.transforms.size()), kern::Span<const f32>(p.tints.data(), p.tints.size()));
    r_->set_visible(p.id, true);
  }
}

void CombatArt::heli(const Mat4& m, f32 rotor, bool wreck) {
  // military_response.mjs buildHeli: capsule body 4.2 m, glass nose, tail boom, fin, stub wings
  // with pods, skids, chin gun, two-blade main rotor 12.5 m, tail rotor.
  const Vec3 o = wreck ? kOlive * 0.35f : kOlive, dk = wreck ? kDark * 0.5f : kDark;
  put(box, m * sc3(1.9f, 5.6f, 1.9f), o);
  put(sphere, m * at({0, -2.8f, 0}) * sc3(1.9f, 1.9f, 1.9f), o);
  put(sphere, m * at({0, 2.6f, 0.2f}) * sc3(1.5f, 2.2f, 1.6f), o);
  put(sphere, m * at({0, 2.3f, 0.45f}) * sc3(1.2f, 1.9f, 1.2f), kGlass);
  put(cylinder, m * at({0, -2.2f, 0.35f}) * rot({1, 0, 0}, kPi / 2) * sc3(0.6f, 0.6f, 5.2f) * at({0, 0, 0}), o);
  put(box, m * at({0, -7, 0.95f}) * sc3(0.12f, 1.1f, 1.5f), o);
  for (f32 s : {-1.0f, 1.0f}) {
    put(box, m * at({s * 1.4f, 0.2f, -0.15f}) * sc3(1.7f, 0.5f, 0.12f), o);
    put(cylinder, m * at({s * 2.1f, -0.4f, -0.35f}) * rot({1, 0, 0}, -kPi / 2) * sc3(0.32f, 0.32f, 1.6f), dk);
    put(box, m * at({s * 0.9f, 0, -1.3f}) * sc3(0.08f, 3.2f, 0.08f), dk);
  }
  put(cylinder, m * at({0, 2.7f, -0.8f}) * rot({1, 0, 0}, -kPi / 2) * sc3(0.16f, 0.16f, 1.1f), dk);
  if (!wreck) {
    const Mat4 mast = m * at({0, 0.2f, 1.25f}) * rot({0, 0, 1}, rotor);
    put(box, mast * sc3(12.5f, 0.32f, 0.04f), kSteel * 0.6f);
    put(box, mast * rot({0, 0, 1}, kPi / 2) * sc3(12.5f, 0.32f, 0.04f), kSteel * 0.6f);
    put(box, m * at({0.18f, -7, 1.1f}) * rot({1, 0, 0}, rotor * 1.7f) * sc3(0.04f, 0.22f, 2.2f), kSteel * 0.6f);
    // red anti-collision light (military_response.mjs requestLight 400 ms blink)
    if (std::fmod(rotor, 4.4f) < 2.2f) put(glow_sphere, m * at({0, -7, 1.75f}) * sc3(0.25f, 0.25f, 0.25f), {3.0f, 0.2f, 0.1f});
  }
}

void CombatArt::tank(const Mat4& m, f32 turret, bool wreck) {
  // buildTank: hull 6.6 x 3.1 x 1.0, tracks, glacis, turret 2.8 x 2.3 x 0.75, 4.4 m gun.
  const Vec3 o = wreck ? kOlive * 0.3f : kOlive, dk = wreck ? kDark * 0.5f : kDark;
  put(box, m * at({0, 0, 0.95f}) * sc3(6.6f, 3.1f, 1.0f), o);
  put(box, m * at({0, 1.25f, 0.55f}) * sc3(6.9f, 0.75f, 0.85f), dk);
  put(box, m * at({0, -1.25f, 0.55f}) * sc3(6.9f, 0.75f, 0.85f), dk);
  put(box, m * at({2.9f, 0, 1.25f}) * rot({0, 1, 0}, -0.5f) * sc3(1.4f, 2.9f, 0.5f), o);
  const Mat4 t = m * at({-0.3f, 0, 1.65f}) * rot({0, 0, 1}, turret);
  put(box, t * at({0, 0, 0.2f}) * sc3(2.8f, 2.3f, 0.75f), o);
  put(cylinder, t * at({0.8f, 0, 0.3f}) * rot({0, 1, 0}, kPi / 2) * sc3(0.3f, 0.3f, 4.4f), dk);
  put(cylinder, t * at({-0.6f, 0.5f, 0.55f}) * sc3(0.7f, 0.7f, 0.4f), dk);
}

void CombatArt::flak(Vec3 p, f32 base_yaw, f32 yaw, f32 pitch, bool dead, bool flash) {
  // flak_battery.mjs buildGun: sandbag ring (14 bags, two gaps), base, pedestal, shield, four
  // 2.3 m barrels with muzzle brakes, magazines, muzzle flash.
  const f32 k = dead ? 0.35f : 1.0f;
  const Mat4 base = at(p) * rot({0, 0, 1}, base_yaw);
  for (int i = 0; i < 14; ++i) {
    if (i == 3 || i == 4) continue;
    const f32 a = static_cast<f32>(i) / 14 * 2 * kPi;
    const Mat4 bag = base * at({std::cos(a) * 2.6f, std::sin(a) * 2.6f, 0.3f}) * rot({0, 0, 1}, a + kPi / 2) * sc3(1.2f, 0.56f, 0.5f);
    put(sphere, bag, kSand * k);
    put(sphere, base * at({std::cos(a) * 2.6f, std::sin(a) * 2.6f, 0.75f}) * rot({0, 0, 1}, a + kPi / 2 + 0.2f) * sc3(1.2f, 0.56f, 0.48f), kSand * k);
  }
  put(cylinder, at(p) * sc3(2.4f, 2.4f, 0.35f), kOlive * k);
  const Mat4 y = at(p + Vec3{0, 0, 0.35f}) * rot({0, 0, 1}, yaw);
  put(box, y * at({0, 0, 0.45f}) * sc3(1.2f, 1.4f, 0.8f), kOlive * k);
  put(box, y * at({0.55f, 0, 1.25f}) * sc3(0.08f, 1.9f, 1.1f), kOlive * k);
  put(box, y * at({-0.5f, 0, 0.95f}) * sc3(0.4f, 0.5f, 0.12f), kSteel * k);
  const Mat4 pt = y * at({0.1f, 0, 1.15f}) * rot({0, 1, 0}, -(dead ? -0.35f : pitch));
  put(box, pt * sc3(1.0f, 0.9f, 0.5f), kSteel * k);
  for (const Vec2 q : {Vec2{-0.28f, 0.17f}, Vec2{0.28f, 0.17f}, Vec2{-0.28f, -0.17f}, Vec2{0.28f, -0.17f}}) {
    put(cylinder, pt * at({0.25f, q.x, q.y}) * rot({0, 1, 0}, kPi / 2) * sc3(0.1f, 0.1f, 2.3f), kSteel * k);
    put(cylinder, pt * at({2.45f, q.x, q.y}) * rot({0, 1, 0}, kPi / 2) * sc3(0.15f, 0.15f, 0.22f), kSteel * k);
    put(box, pt * at({0.2f, q.x, q.y + (q.y > 0 ? 0.28f : -0.28f)}) * sc3(0.25f, 0.08f, 0.3f), kOlive * k);
  }
  if (flash) put(glow_sphere, pt * at({2.75f, 0, 0}) * sc3(1.5f, 1.0f, 1.0f), {3.2f, 2.2f, 0.9f});
}

void CombatArt::fighter(Vec3 p, f32 flame) {
  // fighter_jets.mjs jetModel (forward +Y): fuselage 11 m, nose cone, canopy, swept wings,
  // canted fins, stabilisers, twin nozzles, afterburner flame.
  const Mat4 m = at(p);
  put(cylinder, m * at({0, -5.5f, 0}) * rot({1, 0, 0}, -kPi / 2) * sc3(1.5f, 1.5f, 11), kJetMetal);
  put(cone, m * at({0, 5.5f, 0}) * rot({1, 0, 0}, -kPi / 2) * sc3(1.24f, 1.24f, 3.6f), kJetMetal);
  put(sphere, m * at({0, 3.4f, 0.55f}) * sc3(1.0f, 3.0f, 0.9f), kGlass);
  for (f32 s : {-1.0f, 1.0f}) {
    put(box, m * at({s * 3.2f, -1.6f, -0.1f}) * rot({0, 0, 1}, s * 0.62f) * sc3(5.8f, 2.4f, 0.16f), kJetMetal);
    put(box, m * at({s * 0.9f, -5.0f, 1.3f}) * rot({0, 1, 0}, s * 0.32f) * sc3(0.12f, 2.6f, 2.4f), kJetDark);
    put(box, m * at({s * 1.6f, -5.4f, -0.1f}) * sc3(2.2f, 1.6f, 0.1f), kJetDark);
    put(cylinder, m * at({s * 0.45f, -6.2f, -0.05f}) * rot({1, 0, 0}, -kPi / 2) * sc3(0.9f, 0.9f, 1.2f), kJetDark);
  }
  put(glow_sphere, m * at({0, -8.3f, -0.05f}) * sc3(0.8f, 3.2f * flame, 0.8f), {3.2f, 1.6f, 0.5f});
}

void CombatArt::gate(Vec3 c, f32 yaw, f32 w, f32 h, Vec3 color) {
  // race_track.mjs gateFrame: four 5.5 cm bars around the opening (local +X across, +Y forward).
  constexpr f32 kPost = 0.055f;
  const Mat4 m = at(c) * rot({0, 0, 1}, yaw);
  put(glow_box, m * at({0, 0, h / 2}) * sc3(w + kPost * 2, kPost, kPost), color);
  put(glow_box, m * at({0, 0, -h / 2}) * sc3(w + kPost * 2, kPost, kPost), color);
  put(glow_box, m * at({-w / 2, 0, 0}) * sc3(kPost, kPost, h + kPost * 2), color);
  put(glow_box, m * at({w / 2, 0, 0}) * sc3(kPost, kPost, h + kPost * 2), color);
  // a post down to the ground so the gate is not hanging in the air
  put(box, at({c.x, c.y, 0}) * at({w / 2 * std::cos(yaw), w / 2 * std::sin(yaw), 0}) * sc3(kPost, kPost, std::fmax(0.01f, c.z - h / 2)) * at({0, 0, 0.5f}), {0.18f, 0.25f, 0.31f});
}

}  // namespace sc
