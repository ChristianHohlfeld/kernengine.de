// Street lamp rules (see lamps.h); values from street_lamps.mjs.
#include "lamps.h"

#include <algorithm>
#include <cmath>

namespace sc::lamps {

namespace {
i64 grid_key(f32 x, f32 y) { return (static_cast<i64>(std::floor(x / kGridM)) << 32) ^ (static_cast<i64>(std::floor(y / kGridM)) & 0xffffffffll); }
}  // namespace

i64 cell_key(f32 x, f32 y) { return (static_cast<i64>(std::lround(x / 8)) << 32) ^ (static_cast<i64>(std::lround(y / 8)) & 0xffffffffll); }

void Field::place_road(const std::vector<Vec2>& pts, f32 width, u32 lane, const std::function<f32(f32, f32)>& ground) {
  f32 carry = kSpacing * 0.5f, side = 1;
  for (std::size_t i = 0; i + 1 < pts.size() && lamps_.size() < kMax * 8; ++i) {
    const Vec2 a = pts[i], b = pts[i + 1];
    const f32 dx = b.x - a.x, dy = b.y - a.y, l = std::hypot(dx, dy);
    if (l < 0.5f) continue;
    const f32 ux = dx / l, uy = dy / l, nx = -uy, ny = ux;
    for (f32 t = carry; t < l; t += kSpacing) {
      side = -side;
      const f32 off = width / 2 + kKerb, x = a.x + ux * t + nx * off * side, y = a.y + uy * t + ny * off * side;
      const i64 key = cell_key(x, y);
      if (!seen_.insert(key).second) continue;
      Lamp lp;
      lp.foot = {x, y, ground ? ground(x, y) : 0};
      lp.yaw = std::atan2(-ny * side, -nx * side);
      lp.head = {x + std::cos(lp.yaw) * kArm, y + std::sin(lp.yaw) * kArm};
      lp.key = key;
      lp.lane = lane;
      const auto br = broken_.find(key);
      if (br != broken_.end()) lp.state = br->second;
      grid_[grid_key(x, y)].push_back(static_cast<i32>(lamps_.size()));
      lamps_.push_back(lp);
    }
    carry = std::fmod(std::fmod(carry - l, kSpacing) + kSpacing, kSpacing);
  }
}

void Field::rebuild(const std::vector<Spot>& spots, const std::function<f32(f32, f32)>& ground) {
  clear();
  lamps_.reserve(spots.size());
  for (const Spot& s : spots) {
    Lamp lp;
    lp.foot = {s.x, s.y, ground ? ground(s.x, s.y) : 0};
    lp.yaw = s.yaw;
    lp.head = {s.x + std::cos(s.yaw) * kArm, s.y + std::sin(s.yaw) * kArm};
    lp.key = cell_key(s.x, s.y);
    if (!seen_.insert(lp.key).second) continue;
    const auto br = broken_.find(lp.key);
    if (br != broken_.end()) lp.state = br->second;
    grid_[grid_key(s.x, s.y)].push_back(static_cast<i32>(lamps_.size()));
    lamps_.push_back(lp);
  }
}

void Field::reground(f32 x0, f32 y0, f32 x1, f32 y1, const std::function<f32(f32, f32)>& ground) {
  if (!ground) return;
  for (Lamp& l : lamps_)
    if (l.foot.x >= x0 && l.foot.x <= x1 && l.foot.y >= y0 && l.foot.y <= y1) l.foot.z = ground(l.foot.x, l.foot.y);
}

std::vector<i32> Field::near(f32 x, f32 y, f32 r) const {
  std::vector<i32> out;
  const i64 g0 = static_cast<i64>(std::floor((x - r) / kGridM)), g1 = static_cast<i64>(std::floor((x + r) / kGridM));
  const i64 h0 = static_cast<i64>(std::floor((y - r) / kGridM)), h1 = static_cast<i64>(std::floor((y + r) / kGridM));
  for (i64 i = g0; i <= g1; ++i)
    for (i64 j = h0; j <= h1; ++j) {
      const auto it = grid_.find((i << 32) ^ (j & 0xffffffffll));
      if (it == grid_.end()) continue;
      for (i32 k : it->second) {
        const Lamp& l = lamps_[static_cast<std::size_t>(k)];
        if (std::hypot(l.foot.x - x, l.foot.y - y) <= r) out.push_back(k);
      }
    }
  return out;
}

bool Field::set_state(i32 i, u8 state) {
  if (i < 0 || i >= static_cast<i32>(lamps_.size())) return false;
  Lamp& l = lamps_[static_cast<std::size_t>(i)];
  if (state <= l.state) return false;
  l.state = state;
  broken_[l.key] = state;
  return true;
}

i32 Field::shot(Vec3 a, Vec3 b) const {
  const Vec3 d = b - a;
  const f32 L = kern::length(d);
  if (L < 0.1f) return -1;
  const f32 mx = (a.x + b.x) / 2, my = (a.y + b.y) / 2;
  i32 best = -1;
  f32 bt = 1e9f;
  for (i32 k : near(mx, my, L / 2 + 2)) {
    const Lamp& l = lamps_[static_cast<std::size_t>(k)];
    if (l.state >= 1) continue;
    const Vec3 h{l.head.x, l.head.y, l.foot.z + kHeadZ};
    const f32 t = clampf(kern::dot(h - a, d) / (L * L), 0, 1);
    if (kern::length(a + d * t - h) < kHeadHitR && t < bt) { bt = t; best = k; }
  }
  return best;
}

std::vector<Field::BlastHit> Field::blast(Vec3 p, f32 radius) const {
  std::vector<BlastHit> out;
  const f32 r = std::fmax(3.0f, radius > 0 ? radius : 8);
  for (i32 k : near(p.x, p.y, r)) {
    const Lamp& l = lamps_[static_cast<std::size_t>(k)];
    const f32 dist = std::hypot(l.foot.x - p.x, l.foot.y - p.y);
    if (dist < r * 0.6f) out.push_back({k, 2, {l.foot.x - p.x, l.foot.y - p.y}, 9 * (1 - dist / r) + 3});
    else out.push_back({k, 1, {}, 0});
  }
  return out;
}

std::vector<Field::CarHit> Field::car(Vec3 pos, f32 yaw, Vec3 v, f32 hl, f32 hw, f32 m) const {
  std::vector<CarHit> out;
  const f32 c = std::cos(yaw), s = std::sin(yaw), sp = std::hypot(v.x, v.y);
  for (i32 k : near(pos.x, pos.y, hl + 1)) {
    const Lamp& l = lamps_[static_cast<std::size_t>(k)];
    if (l.state == 2) continue;
    const f32 dx = l.foot.x - pos.x, dy = l.foot.y - pos.y, u = dx * c + dy * s, w = -dx * s + dy * c;
    if (std::fabs(u) > hl + kPoleR || std::fabs(w) > hw + kPoleR) continue;
    const f32 dd = std::fmax(1e-3f, std::hypot(dx, dy)), nx = dx / dd, ny = dy / dd, toward = v.x * nx + v.y * ny;
    CarHit h;
    h.lamp = k;
    if (sp > kToppleMps && toward > 1) {
      h.topple = true;
      const f32 loss = std::fmin(sp * 0.22f, 2.4f);
      h.impulse = {-v.x / sp * m * loss, -v.y / sp * m * loss, 0};
    } else if (toward > 0) {
      h.impulse = {-nx * m * toward * 1.05f, -ny * m * toward * 1.05f, 0};
      h.clang = toward > 1;
    } else continue;
    out.push_back(h);
  }
  return out;
}

}  // namespace sc::lamps
