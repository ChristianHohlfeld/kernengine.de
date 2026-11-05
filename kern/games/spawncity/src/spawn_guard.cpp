// Spawn rules (see spawn_guard.h).
#include "spawn_guard.h"

#include <cmath>

namespace sc::spawn {

f32 cone_half_angle(f32 vertical_fov, f32 aspect) {
  const f32 v = vertical_fov, h = 2 * std::atan(std::tan(v / 2) * std::fmax(0.2f, aspect));
  return std::fmin(kPi * 0.92f, std::fmax(v, h) / 2 + kConeMargin);
}

bool seen_by(const Viewer& v, Vec3 point, f32 far_entry, f32 height, const Blocked& blocked) {
  const Vec3 to = point - v.eye;
  const f32 d = kern::length(to);
  if (d < kMinDistance) return true;
  if (d > v.far) return false;
  if (far_entry > 0 && d >= far_entry) return false;
  const f32 cos_toward = kern::dot(to * (1.0f / std::fmax(d, 1e-6f)), v.forward);
  bool inside = cos_toward >= std::cos(v.half_angle);
  if (!inside && v.local) {
    // the frustum test of the sphere at half height: the cone widened by the sphere's angular radius
    const Vec3 c = point + Vec3{0, 0, height * 0.5f};
    const Vec3 tc = c - v.eye;
    const f32 dc = std::fmax(1e-3f, kern::length(tc));
    inside = kern::dot(tc * (1.0f / dc), v.forward) >= std::cos(std::fmin(kPi, v.half_angle + std::asin(std::fmin(1.0f, kFrustumSphere / dc))));
  }
  if (!inside) return false;
  if (d > kOcclusionMax || !blocked) return true;
  // hiddenBehindBuildings: both the feet (+0.3 m) and the top must be hidden
  for (f32 dz : {0.3f, height})
    if (!blocked(v.eye, point + Vec3{0, 0, dz})) return true;
  return false;
}

bool can_spawn(const std::vector<Viewer>& viewers, Vec3 point, f32 far_entry, f32 height, const Blocked& blocked) {
  for (const Viewer& v : viewers)
    if (seen_by(v, point, far_entry, height, blocked)) return false;
  return true;
}

bool footprints_overlap(const Footprint& a, const Footprint& b, f32 margin) {
  const Vec2 aa[2] = {{std::cos(a.yaw), std::sin(a.yaw)}, {-std::sin(a.yaw), std::cos(a.yaw)}};
  const Vec2 bb[2] = {{std::cos(b.yaw), std::sin(b.yaw)}, {-std::sin(b.yaw), std::cos(b.yaw)}};
  const f32 dx = b.center.x - a.center.x, dy = b.center.y - a.center.y;
  auto radius = [](const Footprint& p, const Vec2* basis, Vec2 u) {
    return p.half.x * std::fabs(basis[0].x * u.x + basis[0].y * u.y) + p.half.y * std::fabs(basis[1].x * u.x + basis[1].y * u.y);
  };
  for (const Vec2* set : {aa, bb})
    for (int i = 0; i < 2; ++i) {
      const Vec2 u = set[i];
      if (!(std::fabs(dx * u.x + dy * u.y) < radius(a, aa, u) + radius(b, bb, u) + margin)) return false;
    }
  return true;
}

}  // namespace sc::spawn
