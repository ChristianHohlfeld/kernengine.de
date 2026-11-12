// Kern vehicles: what the vehicle models share - units and small helpers, a rigid body's state as
// a force model reads it, and the description of a body (colliders, mass properties) that the
// level's actor and a test's bare physics body are both made from.
//
// Conventions (Kern's): metres, seconds, kilograms, Z up.
#pragma once

#include <cmath>
#include <vector>

#include "kern/game/geometry.h"
#include "kern/math/math.h"
#include "kern/physics/physics.h"

namespace kern::game::vehicles {

using kern::game::Rng;

constexpr f32 kG = 9.80665f;
constexpr f32 kPi = 3.14159265358979323846f;
constexpr f32 kDeg = kPi / 180.0f;

inline f32 clampf(f32 v, f32 lo, f32 hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline f32 sign0(f32 v) { return v > 0 ? 1.0f : (v < 0 ? -1.0f : 0.0f); }
inline f32 rate_limit(f32 cur, f32 target, f32 max_step) { return cur + clampf(target - cur, -max_step, max_step); }
/// Exponential approach: the share of the remaining distance covered in dt at `rate` per second.
inline f32 damp_alpha(f32 rate, f32 dt) { return rate > 0 && dt > 0 ? 1.0f - std::exp(-rate * dt) : 0.0f; }
inline f32 wrap_pi(f32 a) {
  while (a > kPi) a -= 2 * kPi;
  while (a < -kPi) a += 2 * kPi;
  return a;
}

inline Quat conj(Quat q) { return {-q.x, -q.y, -q.z, q.w}; }
inline Vec3 to_local(Quat q, Vec3 v) { return conj(q).rotate(v); }
inline Quat qnormalize(Quat q) {
  const f32 l = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
  return l > 1e-9f ? Quat{q.x / l, q.y / l, q.z / l, q.w / l} : Quat{};
}
inline Quat yaw_quat(f32 yaw) { return Quat::axis_angle({0, 0, 1}, yaw); }
/// Heading (radians, CCW from +X) of a body axis projected on the ground plane.
inline f32 heading_of(Vec3 axis) { return std::atan2(axis.y, axis.x); }
inline Quat slerp(Quat a, Quat b, f32 t) {
  f32 d = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
  if (d < 0) { b = {-b.x, -b.y, -b.z, -b.w}; d = -d; }
  if (d > 0.9995f) return qnormalize({a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t});
  const f32 th = std::acos(d), s = std::sin(th);
  const f32 wa = std::sin((1 - t) * th) / s, wb = std::sin(t * th) / s;
  return {a.x * wa + b.x * wb, a.y * wa + b.y * wb, a.z * wa + b.z * wb, a.w * wa + b.w * wb};
}

/// A rigid body's state as the force models read it.
struct BodyState {
  Vec3 position, velocity, angular;
  Quat rotation;
};

inline BodyState read_body(const kern::physics::World& w, kern::physics::Body b) {
  const kern::physics::Pose p = w.pose(b);
  return {p.position, w.linear_velocity(b), w.angular_velocity(b), p.rotation};
}

/// Closest hit below/along a ray against the world, ignoring `self` (and sensors).
struct RayResult {
  bool hit = false;
  f32 distance = 0;
  Vec3 point, normal{0, 0, 1};
};

inline RayResult cast(const kern::physics::World& w, Vec3 origin, Vec3 dir, f32 max_len, kern::physics::Body self, u64 mask = ~0ull) {
  const kern::physics::RayHit h = w.cast_ray(origin, dir * max_len, mask, self);
  RayResult r;
  if (!h.hit) return r;
  r.hit = true;
  r.distance = h.fraction * max_len;
  r.point = h.point;
  r.normal = h.normal;
  return r;
}

// ---- body descriptions: one source of truth for a vehicle's colliders and mass properties ----
struct ColliderSpec {
  kern::physics::ShapeKind kind = kern::physics::ShapeKind::box;
  Vec3 center;               // body frame
  Quat rotation;
  Vec3 size{1, 1, 1};        // box: full extents
  f32 radius = 0.5f, length = 1;  // sphere, capsule (along the shape's Z)
  std::vector<Vec3> points;  // hull
  f32 friction = 0.6f, restitution = 0;
};

struct MassSpec {
  f32 mass = 1;
  Vec3 center;               // centre of mass, body frame
  f32 inertia[6] = {1, 1, 1, 0, 0, 0};  // about the centre of mass: xx yy zz xy xz yz
};

struct RigidSpec {
  std::vector<ColliderSpec> colliders;
  MassSpec mass;
  f32 linear_damping = 0, angular_damping = 0;
  bool bullet = false;
};

/// A capsule from a to b (radius r) as a collider spec (capsules run along the shape's Z).
inline ColliderSpec capsule_between(Vec3 a, Vec3 b, f32 r) {
  ColliderSpec c;
  c.kind = kern::physics::ShapeKind::capsule;
  c.center = (a + b) * 0.5f;
  const Vec3 d = b - a;
  const f32 len = kern::length(d);
  c.length = len;
  c.radius = r;
  const Vec3 z{0, 0, 1}, n = kern::normalize(d);
  const Vec3 axis = kern::cross(z, n);
  const f32 s = kern::length(axis), co = kern::dot(z, n);
  c.rotation = s > 1e-6f ? Quat::axis_angle(axis * (1.0f / s), std::atan2(s, co)) : (co < 0 ? Quat::axis_angle({1, 0, 0}, kPi) : Quat{});
  return c;
}

/// Creates the body in a bare physics world (tests, tools).
inline kern::physics::Body create_body(kern::physics::World& w, const RigidSpec& spec, Vec3 position, Quat rotation, u64 category = 1, u64 mask = ~0ull) {
  kern::physics::BodySettings bs;
  bs.type = kern::physics::BodyType::dynamic;
  bs.position = position;
  bs.rotation = rotation;
  bs.linear_damping = spec.linear_damping;
  bs.angular_damping = spec.angular_damping;
  bs.enable_sleep = false;
  bs.bullet = spec.bullet;
  const kern::physics::Body b = w.create_body(bs);
  for (const ColliderSpec& c : spec.colliders) {
    kern::physics::ShapeSettings ss;
    ss.kind = c.kind;
    ss.center = c.center;
    ss.rotation = c.rotation;
    ss.size = c.size;
    ss.radius = c.radius;
    ss.length = c.length;
    ss.points = kern::Span<const Vec3>(c.points.data(), c.points.size());
    ss.material.friction = c.friction;
    ss.material.restitution = c.restitution;
    ss.material.density = 100;
    ss.material.category_bits = category;
    ss.material.mask_bits = mask;
    w.create_shape(b, ss);
  }
  w.set_mass_data(b, spec.mass.mass, spec.mass.center, spec.mass.inertia);
  return b;
}

/// Solid box inertia (xx yy zz) of mass m and full extents s.
inline void box_inertia(f32 m, Vec3 s, f32 out[6]) {
  out[0] = m * (s.y * s.y + s.z * s.z) / 12;
  out[1] = m * (s.x * s.x + s.z * s.z) / 12;
  out[2] = m * (s.x * s.x + s.y * s.y) / 12;
  out[3] = out[4] = out[5] = 0;
}

}  // namespace kern::game::vehicles
