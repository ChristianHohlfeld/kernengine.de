// Multiplayer art (see mp_art.h).
#include "mp_art.h"

#include <cmath>

#include "kern/geo/primitives.h"

namespace sc {

namespace {
using kern::Mat4;
namespace prim = kern::primitives;
kern::Material glow(Vec3 emissive, f32 alpha = 1) {
  kern::Material m;
  m.roughness = 0.5f;
  m.emissive = emissive;
  m.unlit = true;
  m.cast_shadow = false;
  m.color = {1, 1, 1, alpha};
  return m;
}
}  // namespace

bool MpArt::make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m) {
  p.id = r.allocate_id();
  kern::MeshInput in = mesh.as_input(kern::kMaxExtensionAttributes);
  in.material = m;
  in.visible = false;
  static const Mat4 parked = Mat4::translate({0, 0, -1000});
  in.transforms = kern::Span<const f32>(parked.m, 16);
  if (!r.upsert(p.id, in)) { r.release_id(p.id); p.id = 0; return false; }
  return true;
}

bool MpArt::init(kern::Renderer& r) {
  r_ = &r;
  const Vec3 w{1, 1, 1};
  // ping_marker.mjs build(): post 0.05 x 1 m, octahedron 0.45 (z x1.5), ring 0.55..0.75
  kern::MeshData post;
  prim::append(post, prim::cylinder(0.05f, 1.0f, 6, w), Mat4::translate({0, 0, 0.5f}));
  if (!make(r, post_, post, glow({1.2f, 1.2f, 1.2f}, 0.55f))) return false;
  if (!make(r, gem_, prim::sphere(0.45f, 4, 2, w), glow({1.6f, 1.6f, 1.6f}, 0.95f))) return false;
  if (!make(r, ring_, prim::torus(0.65f, 0.1f, 24, 4, w), glow({1.4f, 1.4f, 1.4f}, 0.7f))) return false;
  // vs_multiplayer.mjs tracer (cylinder along the flight) and explosion sphere
  if (!make(r, tracer_, prim::cylinder(0.03f, 1.0f, 6, w), glow({3.0f, 2.4f, 1.2f}, 0.9f))) return false;
  if (!make(r, flash_, prim::sphere(1.0f, 10, 6, w), glow({4.0f, 1.5f, 0.6f}, 0.8f))) return false;
  kern::Material accent;
  accent.roughness = 0.35f;
  accent.metalness = 0.25f;
  accent.emissive = {0.35f, 0.35f, 0.35f};
  if (!make(r, canopy_, prim::box({0.085f, 0.06f, 0.026f}, w), accent)) return false;
  if (!make(r, led_, prim::torus(0.03f, 0.004f, 24, 6, w), glow({2, 2, 2}))) return false;
  if (!make(r, shield_, prim::sphere(1.0f, 16, 10, w), glow({0.5f, 0.8f, 1.0f}, 0.18f))) return false;
  return true;
}

void MpArt::clear(Part& p) {
  p.transforms.clear();
  p.tints.clear();
}

void MpArt::begin() {
  clear(post_);
  clear(gem_);
  clear(ring_);
  clear(tracer_);
  clear(flash_);
  clear(canopy_);
  clear(led_);
  clear(shield_);
}

void MpArt::put(Part& p, const Mat4& m, Vec3 t) {
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  p.tints.insert(p.tints.end(), {t.x, t.y, t.z, 1});
}

void MpArt::ping(Vec3 foot, f32 s, Vec3 c, f32 fade, f32 now) {
  const Vec3 t = c * fade;
  put(post_, Mat4::trs(foot, Quat{}, {s, s, 2.2f * s}), Vec3{1, 1, 1} * fade);
  const f32 bob = std::sin(now / 0.3f) * 0.15f * s;
  put(gem_, Mat4::trs(foot + Vec3{0, 0, 2.6f * s + bob}, yaw_quat(now / 0.7f), {s, s, 1.5f * s}), t);
  const f32 rs = s * (1 + 0.25f * std::sin(now / 0.22f));
  put(ring_, Mat4::trs(foot + Vec3{0, 0, 0.05f}, Quat{}, {rs, rs, 0.3f * rs}), t);
}

void MpArt::tracer(Vec3 at, Vec3 dir, f32 len, Vec3 c) {
  // The cylinder stands along +Z: turn Z onto the flight direction.
  const f32 dl = kern::length(dir);
  if (dl < 1e-6f) return;
  const Vec3 d = dir * (1 / dl);
  const Vec3 axis{-d.y, d.x, 0};  // cross(z, d)
  const f32 s = kern::length(axis), a = std::atan2(s, d.z);
  const Quat q = s > 1e-5f ? Quat::axis_angle(axis * (1 / s), a) : (d.z < 0 ? Quat::axis_angle({1, 0, 0}, kPi) : Quat{});
  put(tracer_, Mat4::trs(at, q, {1, 1, len}), c);
}

void MpArt::flash(Vec3 at, f32 radius, f32 t) {
  const f32 r = radius * (0.35f + 0.65f * t);
  put(flash_, Mat4::trs(at, Quat{}, {r, r, r}), Vec3{1, 1, 1} * std::fmax(0.0f, 1 - t));
}

void MpArt::drone_accent(const Mat4& body, Vec3 c) {
  put(canopy_, body * Mat4::translate({0.005f, 0, 0.027f}), c);
  put(led_, body * Mat4::translate({0.005f, 0, 0.042f}), c);
}

void MpArt::shield(Vec3 center, f32 radius, f32 now) {
  const f32 r = radius * (1 + 0.04f * std::sin(now * 9));
  put(shield_, Mat4::trs(center, Quat{}, {r, r, r * 1.3f}), {0.6f, 0.85f, 1});
}

void MpArt::upload(Part& p) {
  if (!p.id) return;
  if (p.transforms.empty()) { r_->set_visible(p.id, false); return; }
  r_->update_instances(p.id, kern::Span<const f32>(p.transforms), kern::Span<const f32>(p.tints));
  r_->set_visible(p.id, true);
}

void MpArt::end() {
  upload(post_);
  upload(gem_);
  upload(ring_);
  upload(tracer_);
  upload(flash_);
  upload(canopy_);
  upload(led_);
  upload(shield_);
}

}  // namespace sc
