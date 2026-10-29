// SpawnCity on Kern - shared helpers for every game module.
//
// Conventions (Kern's): metres, seconds, kilograms, Z up. The world's X axis
// points east and Y north (kern::GeoProjector). Licensed under the Apache
// License 2.0 like Kern (see LICENSE in the repository root).
#pragma once

#include <cmath>
#include <cstdint>

#include "kern/math/math.h"
#include "kern/physics/physics.h"

namespace sc {

using kern::f32;
using kern::f64;
using kern::i32;
using kern::u32;
using kern::u64;
using kern::u8;
using kern::u16;
using kern::Quat;
using kern::Vec2;
using kern::Vec3;

constexpr f32 kG = 9.80665f;
constexpr f32 kPi = 3.14159265358979323846f;
constexpr f32 kDeg = kPi / 180.0f;

// Collision categories (SpawnCity world_rigid_body_physics.mjs: terrain 1, vehicle 2, drone 4,
// query 8). Kern's GameWorld puts the ground and every level collider in category 1; the
// streamed city uses it too.
enum : u64 { kCatWorld = 1, kCatVehicle = 2, kCatDrone = 4, kCatQuery = 8 };

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

/// xorshift32: deterministic per seed, the same on every platform.
struct Rng {
  u32 s;
  explicit Rng(u32 seed = 0x9e3779b9u) : s(seed ? seed : 0x9e3779b9u) {}
  u32 next() { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
  f32 unit() { return static_cast<f32>(next() >> 8) * (1.0f / 16777216.0f); }
  f32 range(f32 a, f32 b) { return a + (b - a) * unit(); }
  /// Standard normal (Box-Muller), as SpawnCity's sensor noise.
  f32 gaussian() {
    if (has_spare_) { has_spare_ = false; return spare_; }
    const f32 u = std::fmax(1e-12f, unit()), v = unit();
    const f32 r = std::sqrt(-2.0f * std::log(u));
    spare_ = r * std::sin(2 * kPi * v);
    has_spare_ = true;
    return r * std::cos(2 * kPi * v);
  }

 private:
  f32 spare_ = 0;
  bool has_spare_ = false;
};

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

}  // namespace sc
