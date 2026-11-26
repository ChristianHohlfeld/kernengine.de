// The drone's model (see drone_art.h).
#include "drone_art.h"

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

bool DroneArt::make(kern::Renderer& r, Part& p, const MeshData& mesh, const kern::Material& m, bool tinted) {
  p.id = r.allocate_id();
  p.tinted = tinted;
  kern::MeshLook look;
  look.material = m;
  look.extra_slot = kern::kMaxExtensionAttributes;
  look.visible = false;
  const Mat4 parked = Mat4::translate({0, 0, -1000});  // hidden until the first frame places it
  if (!kern::upsert(r, p.id, mesh, look, kern::Span<const f32>(parked.m, 16))) { r.release_id(p.id); p.id = 0; return false; }
  return true;
}

bool DroneArt::init(kern::Renderer& r) {
  r_ = &r;
  const Vec3 white{1, 1, 1};
  // -X nose
  MeshData frame;
  prim::append(frame, prim::box({0.11f, 0.06f, 0.03f}, {0.1f, 0.1f, 0.11f}), Mat4{});
  prim::append(frame, prim::box({0.045f, 0.035f, 0.035f}, {0.95f, 0.45f, 0.1f}), at({-0.07f, 0, 0.028f}));  // FPV camera
  prim::append(frame, prim::box({0.1f, 0.035f, 0.03f}, {0.2f, 0.25f, 0.6f}), at({0.015f, 0, -0.03f}));      // battery
  const f32 a = 0.225f / (2 * std::sqrt(2.0f));
  for (Vec2 m : {Vec2{-a, -a}, Vec2{-a, a}, Vec2{a, a}, Vec2{a, -a}}) {
    const f32 len = std::sqrt(m.x * m.x + m.y * m.y);
    prim::append(frame, prim::box({len, 0.016f, 0.008f}, {0.12f, 0.12f, 0.13f}), at({m.x / 2, m.y / 2, 0}) * rot({0, 0, 1}, std::atan2(m.y, m.x)));
  }
  if (!make(r, frame_, frame, mat(0.5f, 0.2f), false)) return false;
  if (!make(r, motor_, prim::cylinder(0.014f, 0.02f, 12, {0.35f, 0.35f, 0.38f}), mat(0.3f, 0.8f), false)) return false;
  kern::Material prop = mat(0.4f);
  prop.alpha = kern::AlphaMode::blend;
  prop.color = {1, 1, 1, 0.72f};
  prop.cast_shadow = false;
  if (!make(r, prop_a_, prim::box({1.0f, 0.012f, 0.002f}, {1.0f, 0.64f, 0.3f}), prop, false)) return false;
  if (!make(r, prop_b_, prim::box({1.0f, 0.012f, 0.002f}, {0.3f, 0.84f, 1.0f}), prop, false)) return false;
  kern::Material led = mat(0.4f, 0, {0.2f, 2.5f, 0.6f});
  led.unlit = true;
  if (!make(r, led_, prim::box({0.012f, 0.012f, 0.006f}, white), led, true)) return false;
  return true;
}

void DroneArt::clear(Part& p) {
  p.transforms.clear();
  p.tints.clear();
}

void DroneArt::put(Part& p, const Mat4& m, Vec3 tint) {
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  if (p.tinted) p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, 1});
}

void DroneArt::upload(Part& p) {
  if (!p.id) return;
  if (p.transforms.empty()) { r_->set_visible(p.id, false); return; }
  r_->update_instances(p.id, kern::Span<const f32>(p.transforms), kern::Span<const f32>(p.tints));
  r_->set_visible(p.id, true);
}

void DroneArt::draw(const Mat4& body, const std::array<Vec3, 4>& motors, const std::array<f32, 4>& rotor_angles, f32 prop_d, bool armed, bool fpv_view) {
  clear(frame_);
  clear(motor_);
  clear(prop_a_);
  clear(prop_b_);
  clear(led_);
  if (!fpv_view) {
    put(frame_, body);
    for (int i = 0; i < 4; ++i) {
      put(motor_, body * at(motors[i] + Vec3{0, 0, -0.004f}));
      const Mat4 p = body * at(motors[i] + Vec3{0, 0, 0.018f}) * rot({0, 0, 1}, rotor_angles[i]) * Mat4::scale({prop_d, 1, 1});
      if (i % 2) put(prop_b_, p);
      else put(prop_a_, p);
    }
    put(led_, body * at({0.06f, 0, 0.018f}), armed ? Vec3{4, 0.2f, 0.1f} : Vec3{0.3f, 0.3f, 0.3f});
  }
  upload(frame_);
  upload(motor_);
  upload(prop_a_);
  upload(prop_b_);
  upload(led_);
}

void DroneArt::hide() {
  clear(frame_);
  clear(motor_);
  clear(prop_a_);
  clear(prop_b_);
  clear(led_);
  upload(frame_);
  upload(motor_);
  upload(prop_a_);
  upload(prop_b_);
  upload(led_);
}

}  // namespace sc
