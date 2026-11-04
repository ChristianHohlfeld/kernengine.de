// GUIDED missile logic (guided_missile.mjs, guided_balance.mjs) - see guided.h.
#include "guided.h"

#include <cmath>

namespace sc::guided {

Dodge update_dodge(Dodge state, Vec2 previous, Vec2 current, f32 dt, bool pedestrian) {
  // updatePedestrianDodge: a valid sample is a pedestrian with 0 < dt <= 0.1 s.
  const f32 distance = std::hypot(current.x - previous.x, current.y - previous.y);
  const bool valid = pedestrian && dt > 0 && dt <= 0.1f && std::isfinite(distance);
  const bool sprint = valid && distance / dt >= kDodgeSpeed;
  Dodge out;
  out.seconds = sprint ? state.seconds + dt : 0;
  out.lost = state.lost || out.seconds >= kDodgeHold;
  return out;
}

f32 pick_score(Vec3 eye, Vec3 forward, f32 fov, f32 height_px, Vec3 target) {
  const Vec3 d = target - eye;
  const f32 dist = kern::length(d);
  if (dist < 1e-3f) return -1;
  const f32 ahead = kern::dot(d, forward);
  if (ahead <= 0) return -1;
  // Off-axis distance on the image plane, in pixels (the projection's focal length is
  // (height / 2) / tan(fov / 2)).
  const f32 off = std::sqrt(std::fmax(0.0f, dist * dist - ahead * ahead)) / ahead;
  const f32 px = off * (height_px * 0.5f) / std::tan(fov * 0.5f);
  return std::fmax(0.0f, px - std::fmin(18.0f, 600.0f / dist));  // small far things get a little slack
}

Missile launch(Vec3 start, Vec3 target, f32 now, bool pedestrian) {
  Missile m;
  m.p = start;
  m.goal = target;
  Vec2 dir{target.x - start.x, target.y - start.y};
  const f32 l = std::hypot(dir.x, dir.y);
  dir = l > 1e-4f ? dir * (1.0f / l) : Vec2{1, 0};
  m.v = {dir.x * 6, dir.y * 6, -3};  // off the rail, then the motor lights
  m.born = now;
  m.cruise_z = std::fmax(start.z + kCruiseAboveLaunch, target.z + kCruiseAboveTarget);
  m.pedestrian = pedestrian;
  m.dodge_last = {target.x, target.y};
  return m;
}

void track(Missile& m, Vec3 at, f32 dt) {
  if (!m.tracking) return;
  m.dodge = update_dodge(m.dodge, m.dodge_last, {at.x, at.y}, dt, m.pedestrian);
  m.dodge_last = {at.x, at.y};
  if (m.dodge.lost) { m.tracking = false; return; }  // flies on to the last place it knew
  m.goal = at;
}

Result step(Missile& m, f32 now, f32 dt, f32 ground) {
  if (now < m.born + kIgnite) {
    m.v.z -= 9.81f * dt;
    m.p = m.p + m.v * dt;
    return Result::flying;
  }
  m.motor = true;
  const Vec3 to = m.goal - m.p;
  const f32 horiz = std::hypot(to.x, to.y);
  const bool diving = horiz < kDiveRange || kern::length(to) < kDiveRange;
  Vec3 aim = m.goal;
  if (!diving) aim.z = std::fmax(m.cruise_z, ground + kCruiseAgl);
  const Vec3 want = kern::normalize(aim - m.p, {1, 0, 0}) * (kSpeed * (diving ? 1.12f : 1.0f));
  Vec3 dv = want - m.v;
  const f32 max_dv = 9.81f * kTurnG * dt * (diving ? 1.4f : 1.0f);
  const f32 dl = kern::length(dv);
  if (dl > max_dv) dv = dv * (max_dv / dl);
  m.v = m.v + dv;
  const f32 speed = kern::length(m.v), boost = std::fmin(kSpeed, speed + 40 * dt);
  m.v = kern::normalize(m.v, {1, 0, 0}) * std::fmax(boost, 8.0f);
  m.p = m.p + m.v * dt;
  if (kern::length(m.goal - m.p) < kFuze) return Result::fuze;
  if (m.p.z < ground + 0.3f) { m.p.z = ground + 0.3f; return Result::ground; }
  if (now - m.born > kTtl) return Result::timeout;
  return Result::flying;
}

}  // namespace sc::guided
