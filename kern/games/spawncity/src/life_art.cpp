// Drawing of street life (see life_art.h). Shapes after SpawnCity's own code meshes
// (cows.mjs bodyGeometry/headGeometry/legGeometry, pigeons.mjs, world_liveliness.mjs).
#include "life_art.h"

#include <cmath>

#include "kern/geo/primitives.h"

namespace sc {

namespace {
using kern::Mat4;
namespace prim = kern::primitives;
Mat4 rot(Vec3 axis, f32 a) { return Mat4::trs({}, Quat::axis_angle(axis, a), {1, 1, 1}); }
Mat4 at(Vec3 p) { return Mat4::translate(p); }
Mat4 sc3(Vec3 s) { return Mat4::scale(s); }
Vec3 rgb(u32 h) { return {((h >> 16) & 255) / 255.0f, ((h >> 8) & 255) / 255.0f, (h & 255) / 255.0f}; }
}  // namespace

bool LifeArt::make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m) {
  p.id = r.allocate_id();
  kern::MeshInput in = mesh.as_input(kern::kMaxExtensionAttributes);
  in.material = m;
  in.visible = false;
  static const Mat4 parked = Mat4::translate({0, 0, -1000});
  in.transforms = kern::Span<const f32>(parked.m, 16);
  if (!r.upsert(p.id, in)) { r.release_id(p.id); p.id = 0; return false; }
  return true;
}

bool LifeArt::init(kern::Renderer& r) {
  r_ = &r;
  const Vec3 white{1, 1, 1};
  kern::Material m;
  m.roughness = 0.85f;
  if (!make(r, box_, prim::box({1, 1, 1}, white), m)) return false;
  if (!make(r, ball_, prim::sphere(0.5f, 10, 7, white), m)) return false;
  if (!make(r, cyl_, prim::cylinder(0.5f, 1, 8, white), m)) return false;
  kern::Material g = m;
  g.emissive = {2.6f, 2.4f, 2.0f};
  g.unlit = true;
  g.cast_shadow = false;
  return make(r, glow_, prim::box({1, 1, 1}, white), g);
}

void LifeArt::begin() {
  for (Part* p : {&box_, &ball_, &cyl_, &glow_}) { p->transforms.clear(); p->tints.clear(); }
}

void LifeArt::put(Part& p, const Mat4& m, Vec3 tint) {
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, 1});
}

void LifeArt::cow(const Life::Cow& c, f32 now) {
  // x forward, origin between the feet; coat tint, dark spots, pink udder and muzzle
  Mat4 M;
  if (c.dead) M = Mat4::trs(c.pos, c.rot, {1, 1, 1}) * at({0, 0, -1.12f});  // the Box3D body (centre 1.12 m over the feet)
  else {
    const f32 bob = c.walking && now < c.flee_until ? std::fabs(std::sin(c.phase)) * 0.05f : 0;
    M = at(c.pos + Vec3{0, 0, bob}) * rot({0, 0, 1}, c.yaw);
    const f32 ft = (now - c.flinch_at) / 0.26f;
    if (ft < 1) M = M * rot({1, 0, 0}, std::sin(ft * kPi) * 0.12f);  // the hit rocks the body sideways
  }
  const Vec3 coat = c.color, dark = rgb(0x1a1714);
  auto part = [&](Vec3 size, Vec3 p, Vec3 tint) { box(M * at(p) * sc3(size), tint); };
  part({1.62f, 0.66f, 0.7f}, {0, 0, 1.15f}, coat);
  part({0.5f, 0.672f, 0.4f}, {-0.25f, 0, 1.28f}, dark);
  part({0.36f, 0.672f, 0.3f}, {0.42f, 0, 1.08f}, dark);
  part({0.3f, 0.3f, 0.16f}, {-0.35f, 0, 0.74f}, rgb(0xd99a9a));
  part({0.05f, 0.05f, 0.6f}, {-0.83f, 0, 1.05f}, coat);  // tail
  part({0.3f, 0.5f, 0.12f}, {0.72f, 0, 1.5f}, coat);
  const bool graze = !c.dead && !c.walking;
  const f32 pitch = c.dead ? 0.2f : graze ? 0.95f + 0.08f * std::sin(now / 0.6f + c.phase) : 0.12f;
  const Mat4 H = M * at({0.86f, 0, 1.36f}) * rot({0, 1, 0}, pitch);
  box(H * at({0.2f, 0, 0}) * sc3({0.42f, 0.32f, 0.34f}), coat);
  box(H * at({0.46f, 0, -0.07f}) * sc3({0.16f, 0.3f, 0.2f}), rgb(0xe2a7a2));
  box(H * at({0.08f, 0.15f, 0.18f}) * sc3({0.04f, 0.04f, 0.12f}), rgb(0xe9e2cf));
  box(H * at({0.08f, -0.15f, 0.18f}) * sc3({0.04f, 0.04f, 0.12f}), rgb(0xe9e2cf));
  const f32 amp = c.dead ? 0 : now < c.flee_until ? 0.6f : c.walking ? 0.32f : 0;
  for (int k = 0; k < 4; ++k) {
    const f32 hx = k < 2 ? 0.6f : -0.58f, hy = k % 2 ? 0.2f : -0.2f;
    const f32 sw = amp * std::sin(c.phase + (k == 0 || k == 3 ? 0 : kPi));
    const Mat4 L = M * at({hx, hy, 0.8f}) * rot({0, 1, 0}, sw);
    box(L * at({0, 0, -0.35f}) * sc3({0.14f, 0.14f, 0.7f}), coat);
    box(L * at({0, 0, -0.75f}) * sc3({0.15f, 0.15f, 0.1f}), rgb(0x2a2522));
  }
}

void LifeArt::life(const Life& life, f32 now, Vec3 eye, f32 range) {
  auto near = [&](Vec3 p) { return std::fabs(p.x - eye.x) < range && std::fabs(p.y - eye.y) < range; };
  for (const Life::Cow& c : life.cows()) {
    if (!near(c.pos)) continue;
    cow(c, now);
  }
  // pigeons: a plump grey body, darker head, beak; wings only in the air
  const Vec3 grey = rgb(0x8d93a0), head = rgb(0x5d6575), breast = rgb(0x7d6f8c), wing = rgb(0x7f8592);
  for (const Life::Flock& f : life.flocks()) {
    for (const Life::Pigeon& b : f.birds) {
      if (!near(b.pos)) continue;
      const bool moving = std::hypot(b.v.x, b.v.y) > 0.05f;
      const f32 nod = f.air ? -0.25f : moving ? std::sin(b.peck) * 0.12f : (std::sin(b.peck * 0.7f) > 0.85f ? 0.55f : 0.0f);
      // a dead pigeon lies on its side
      const Mat4 M = b.dead_at >= 0 ? at(b.pos + Vec3{0, 0, 0.06f}) * rot({0, 0, 1}, b.yaw) * rot({1, 0, 0}, 1.45f) : at(b.pos) * rot({0, 0, 1}, b.yaw) * rot({0, 1, 0}, nod);
      ball(M * at({0, 0, 0.13f}) * sc3({0.3f, 0.19f, 0.18f}), grey);
      ball(M * at({0.07f, 0, 0.15f}) * sc3({0.14f, 0.14f, 0.14f}), breast);
      ball(M * at({0.15f, 0, 0.24f}) * sc3({0.1f, 0.1f, 0.1f}), head);
      box(M * at({0.205f, 0, 0.235f}) * sc3({0.04f, 0.015f, 0.015f}), rgb(0x3a3330));
      box(M * at({-0.17f, 0, 0.12f}) * sc3({0.13f, 0.09f, 0.018f}), rgb(0x4f5562));
      if (f.air && b.dead_at < 0) {
        const f32 flap = std::sin(b.flap) * 0.9f;
        for (f32 s : {-1.0f, 1.0f}) box(M * at({0, s * 0.02f, 0.16f}) * rot({1, 0, 0}, s * flap) * at({0, s * 0.18f, 0}) * sc3({0.16f, 0.36f, 0.01f}), wing);
      }
    }
  }
  // birds over the roofs: a dark V that flaps
  for (const Life::Bird& b : life.birds()) {
    if (now < b.dead_until && now >= b.dying_until) continue;
    if (!near(b.pos)) continue;
    const f32 s = 0.48f + static_cast<f32>(b.seed % 14) / 100;
    const Vec3 col = rgb(0x2f3942 + (b.seed % 5) * 0x101010);
    const Mat4 M = at(b.pos) * rot({0, 0, 1}, b.yaw) * sc3({s, s, s});
    box(M * sc3({0.6f, 0.12f, 0.08f}), col);
    const f32 flap = now < b.dying_until ? 0.2f : (b.flap - 0.82f) * 3;
    for (f32 sd : {-1.0f, 1.0f}) box(M * rot({1, 0, 0}, sd * flap) * at({-0.08f, sd * 0.42f, 0}) * sc3({0.32f, 0.78f, 0.03f}), col);
  }
  // dogs and cats (world_procedural_population.mjs quadrupedBody/Head/Leg, gait, the black cat)
  for (std::size_t i = 0; i < life.pets().size(); ++i) {
    const Life::Pet& p = life.pets()[i];
    if (!p.shown || !near(p.pos)) continue;
    const bool dead = p.dead_at >= 0;
    if (dead && now - p.dead_at > 7) continue;
    const bool dog = life.species(static_cast<u32>(i), life.epoch()) == Life::Species::dog;
    const f32 sc = dog ? p.scale : p.scale * 0.62f;
    Mat4 M = dead ? Mat4::trs(p.pos, p.rot, {sc, sc, sc}) * at({0, 0, -0.27f}) : at(p.pos) * rot({0, 0, 1}, p.yaw) * sc3({sc, sc, sc});
    const Life::CatAttack& cat = life.cat_attack();
    if (!dead && i == 1 && cat.on && cat.phase == 1) M = M * rot({0, 1, 0}, -0.5f);
    const f32 bob = dead ? 0 : std::fabs(std::sin(p.gait * kPi * 2)) * std::fmin(0.03f, p.speed * 0.01f);
    const f32 torso = 0.25f + 0.11f + bob;  // the box over the ground (.25) + ANIMAL_TORSO_UP
    box(M * at({0, 0, torso}) * sc3({0.66f, 0.26f, 0.27f}), p.color);
    box(M * at({-0.44f, 0, torso + 0.1f}) * rot({0, 1, 0}, -0.6f) * sc3({0.3f, 0.055f, 0.055f}), p.color);  // tail
    box(M * at({0.3f, 0, torso + 0.12f}) * rot({0, 1, 0}, 0.5f) * sc3({0.16f, 0.16f, 0.2f}), p.color);    // neck
    const Mat4 H = M * at({0.43f, 0, torso + 0.16f});
    ball(H * sc3({0.3f, 0.3f, 0.3f}), p.color);
    box(H * at({0.13f, 0, -0.04f}) * sc3({0.15f, 0.1f, 0.09f}), p.color);
    for (f32 s : {-1.0f, 1.0f}) box(H * at({-0.02f, s * 0.08f, 0.14f}) * sc3({0.06f, 0.05f, 0.11f}), p.color);  // ears
    const f32 amp = dead ? 0 : std::fmin(0.7f, p.speed * 0.35f);
    const f32 offs[4] = {0.25f, 0.75f, 0, 0.5f};
    const f32 hips[4][2] = {{0.24f, 0.09f}, {0.24f, -0.09f}, {-0.24f, 0.09f}, {-0.24f, -0.09f}};
    for (int k = 0; k < 4; ++k) {
      const f32 sw = dead ? 0.35f * (k < 2 ? 1 : -1) : amp * std::sin((p.gait + offs[k]) * kPi * 2);
      box(M * at({hips[k][0], hips[k][1], torso - 0.1f}) * rot({0, 1, 0}, sw) * at({0, 0, -0.135f}) * sc3({0.075f, 0.075f, 0.27f}), p.color);
    }
  }
  // trees: trunk 2.3 m and a crown
  for (const Life::Tree& t : life.trees()) {
    if (!near(t.foot)) continue;
    const f32 s = t.scale;
    // the unit cylinder stands on z = 0 (prim::cylinder): the trunk from the foot up 2.3 m, its foot
    // 0.4 m in the ground so a slope (or a crater since) never shows it standing on air
    cylinder(at(t.foot - Vec3{0, 0, 0.4f}) * sc3({0.25f * s, 0.25f * s, 2.3f * s + 0.4f}), rgb(0x6a523d));
    ball(at(t.foot + Vec3{0, 0, 2.75f * s}) * sc3({1.95f * s, 1.95f * s, 2.35f * s}), rgb(0x4f8658));
  }
  // ragdolls
  bones_.clear();
  life.ragdolls().draw(bones_);
  for (const Ragdolls::Draw& d : bones_) {
    if (d.sphere) ball(d.m, d.tint);
    else box(d.m, d.tint);
  }
}

void LifeArt::end() {
  for (Part* p : {&box_, &ball_, &cyl_, &glow_}) {
    if (!p->id) continue;
    const bool any = !p->transforms.empty();
    if (any) r_->update_instances(p->id, kern::Span<const f32>(p->transforms.data(), p->transforms.size()), kern::Span<const f32>(p->tints.data(), p->tints.size()));
    r_->set_visible(p->id, any);
  }
}

}  // namespace sc
