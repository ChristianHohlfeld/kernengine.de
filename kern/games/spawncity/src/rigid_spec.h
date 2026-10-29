// Body descriptions shared by the game (level actors in kern::game::GameWorld) and the native
// tests (bodies created directly in a kern::physics::World): one source of truth for every
// vehicle's colliders and mass properties.
#pragma once

#include <vector>

#include "common.h"

namespace sc {

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

}  // namespace sc
