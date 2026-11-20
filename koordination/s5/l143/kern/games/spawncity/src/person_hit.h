// The hit shape of a person (pedestrians, police, soldiers, zombies, the mates in third person):
// the same pose PeopleArt draws - legs, a torso that leans forward when running, the head on top -
// as two boxes and a sphere turned with the body. A round hits where it enters; the part it
// enters decides the zone (a steep shot down through the head is a head shot).
#pragma once

#include <algorithm>
#include <cmath>

#include "common.h"

namespace sc {

enum class BodyPart : u8 { none, legs, torso, head };

namespace person_hit {

constexpr f32 kHipZ = 0.92f, kHeadUp = 0.73f, kHeadR = 0.155f;

/// The body's forward lean (people_art.cpp: the running lean).
inline f32 lean(f32 speed) { return std::clamp(speed - 2.5f, 0.0f, 5.0f) * 0.035f; }

/// Entry of the segment from + seg * t (t in 0..1) into a box (centre c, half extents h) whose
/// axes are ax, ay, az; false when it misses.
inline bool box_entry(Vec3 from, Vec3 seg, Vec3 c, Vec3 ax, Vec3 ay, Vec3 az, Vec3 h, f32& t_out) {
  const Vec3 o = from - c;
  const f32 po[3] = {kern::dot(o, ax), kern::dot(o, ay), kern::dot(o, az)}, pd[3] = {kern::dot(seg, ax), kern::dot(seg, ay), kern::dot(seg, az)};
  const f32 hh[3] = {h.x, h.y, h.z};
  f32 t0 = 0, t1 = 1;
  for (int a = 0; a < 3; ++a) {
    if (std::fabs(pd[a]) < 1e-7f) { if (std::fabs(po[a]) > hh[a]) return false; continue; }
    f32 ta = (-hh[a] - po[a]) / pd[a], tb = (hh[a] - po[a]) / pd[a];
    if (ta > tb) std::swap(ta, tb);
    t0 = std::fmax(t0, ta);
    t1 = std::fmin(t1, tb);
    if (t0 > t1) return false;
  }
  t_out = t0;
  return true;
}

inline bool sphere_entry(Vec3 from, Vec3 seg, Vec3 c, f32 r, f32& t_out) {
  const Vec3 o = from - c;
  const f32 a = kern::dot(seg, seg), b = kern::dot(o, seg), cc = kern::dot(o, o) - r * r;
  if (a < 1e-9f) return false;
  if (cc <= 0) { t_out = 0; return true; }
  const f32 disc = b * b - a * cc;
  if (disc < 0 || b > 0) return false;
  const f32 t = (-b - std::sqrt(disc)) / a;
  if (t > 1) return false;
  t_out = std::fmax(0.0f, t);
  return true;
}

/// The part of a standing person (feet at `feet`, heading `yaw` rad - forward is (cos, sin) -
/// walking at `speed` m/s, `scale` 1 for an adult) the segment from + seg enters first.
inline BodyPart hit(Vec3 feet, f32 yaw, f32 speed, Vec3 from, Vec3 seg, f32& t_out, f32 pad = 1.0f, f32 scale = 1.0f) {
  const f32 cy = std::cos(yaw), sy = std::sin(yaw), l = lean(speed), cl = std::cos(l), sl = std::sin(l);
  const Vec3 fx{cy, sy, 0}, fy{-sy, cy, 0}, up{0, 0, 1};
  // the leaning body frame: up tilts forward
  const Vec3 bz = up * cl + fx * sl, bx = fx * cl - up * sl;
  const Vec3 hip = feet + up * (kHipZ * scale);
  BodyPart best = BodyPart::none;
  f32 bt = 2, t;
  if (sphere_entry(from, seg, hip + bz * (kHeadUp * scale), kHeadR * pad * scale, t) && t < bt) { bt = t; best = BodyPart::head; }
  // torso 0.24 x 0.42 x 0.6 from the hip up, with the arms beside it (to 0.33 either side)
  if (box_entry(from, seg, hip + bz * (0.29f * scale), bx, fy, bz, Vec3{0.15f, 0.31f, 0.31f} * (pad * scale), t) && t < bt - 1e-5f) { bt = t; best = BodyPart::torso; }
  // the legs, hip to shoe
  if (box_entry(from, seg, feet + up * (0.47f * scale), fx, fy, up, Vec3{0.15f, 0.2f, 0.47f} * (pad * scale), t) && t < bt - 1e-5f) { bt = t; best = BodyPart::legs; }
  t_out = bt;
  return best;
}

}  // namespace person_hit
}  // namespace sc
