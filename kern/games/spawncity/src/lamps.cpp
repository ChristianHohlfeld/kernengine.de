// Street lamp rules (see lamps.h); values from street_lamps.mjs.
#include "lamps.h"

#include <algorithm>
#include <cmath>

#include "blueprint_call.h"

namespace sc::lamps {

namespace {
using rules::decimal;
rules::Blueprint g_rules{"Lamps"};  // what happens to one lamp
/// The Blueprint's function Worse: may a lamp in `state` become `to`?
bool worse(u8 state, u8 to) {
  bool worse = false;
  g_rules.call("Worse", std::tie(worse), state, to);
  return worse;
}
i64 grid_key(f32 x, f32 y) { return (static_cast<i64>(std::floor(x / kGridM)) << 32) ^ (static_cast<i64>(std::floor(y / kGridM)) & 0xffffffffll); }
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }
kern::logic::Instance* blueprint() { return g_rules.running; }
f32 pole_radius() { return static_cast<f32>(g_rules.get<f64>("PoleRadius")); }

i64 cell_key(f32 x, f32 y) { return (static_cast<i64>(std::lround(x / 8)) << 32) ^ (static_cast<i64>(std::lround(y / 8)) & 0xffffffffll); }

void Field::place_road(const std::vector<Vec2>& pts, f32 width, u32 lane, f32 ground) {
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
      lp.foot = {x, y, ground};
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

void Field::rebuild(const std::vector<Spot>& spots) {
  clear();
  lamps_.reserve(spots.size());
  for (const Spot& s : spots) {
    Lamp lp;
    lp.foot = {s.x, s.y, s.z};
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

void Field::reground(f32 x0, f32 y0, f32 x1, f32 y1, const CityStream& city) {
  for (Lamp& l : lamps_)
    if (l.foot.x >= x0 && l.foot.x <= x1 && l.foot.y >= y0 && l.foot.y <= y1) l.foot.z = city.ground_height(l.foot.x, l.foot.y);
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
  if (!worse(l.state, state)) return false;
  l.state = state;
  broken_[l.key] = state;
  return true;
}

std::vector<std::pair<i64, u8>> Field::broken_list() const {
  std::vector<std::pair<i64, u8>> out(broken_.begin(), broken_.end());
  std::sort(out.begin(), out.end());
  return out;
}

bool Field::merge_broken(i64 key, u8 state) {
  const auto was = broken_.find(key);
  if (!worse(was == broken_.end() ? 0 : was->second, state)) return false;
  broken_[key] = state;
  for (Lamp& l : lamps_)
    if (l.key == key && worse(l.state, state)) l.state = state;
  return true;
}

i32 Field::shot(Vec3 a, Vec3 b) const {
  const f32 L = kern::length(b - a), mx = (a.x + b.x) / 2, my = (a.y + b.y) / 2;
  i32 best = -1;
  f64 first = 1e9;
  for (i32 k : near(mx, my, L / 2 + 2)) {
    const Lamp& l = lamps_[static_cast<std::size_t>(k)];
    // (the Blueprint's ShotAt: does the round pass this lamp's head, and how far down its path)
    bool hit = false;
    f64 along = 0;
    g_rules.call("ShotAt", std::tie(hit, along), a, b, Vec3{l.head.x, l.head.y, l.foot.z + kHeadZ}, l.state);
    if (!hit) continue;
    if (along < first) { first = along; best = k; }
  }
  return best;
}

std::vector<Field::BlastHit> Field::blast(Vec3 p, f32 radius) const {
  std::vector<BlastHit> out;
  f64 blast_reach = 0;
  g_rules.call("BlastReach", std::tie(blast_reach), decimal(radius));
  const f32 reach = static_cast<f32>(blast_reach);
  for (i32 k : near(p.x, p.y, reach)) {
    const Lamp& l = lamps_[static_cast<std::size_t>(k)];
    u8 state = 0;
    Vec3 push{};
    f64 speed = 0;
    g_rules.call("BlastFate", std::tie(state, push, speed), l.foot, p, decimal(reach));
    out.push_back({k, state, {push.x, push.y}, static_cast<f32>(speed)});
  }
  return out;
}

std::vector<Field::CarHit> Field::car(Vec3 pos, f32 yaw, Vec3 v, f32 hl, f32 hw, f32 m) const {
  std::vector<CarHit> out;
  for (i32 k : near(pos.x, pos.y, hl + 1)) {
    const Lamp& l = lamps_[static_cast<std::size_t>(k)];
    bool hit = false;
    CarHit h;
    g_rules.call("CarFate", std::tie(hit, h.topple, h.impulse, h.clang), l.foot, l.state, pos, decimal(yaw), v, decimal(hl), decimal(hw), decimal(m));
    if (!hit) continue;
    h.lamp = k;
    out.push_back(h);
  }
  return out;
}

}  // namespace sc::lamps
