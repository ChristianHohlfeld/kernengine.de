// Traffic rules at junctions (see world_traffic.h).
#include "world_traffic.h"

#include <algorithm>

namespace sc {

namespace {
constexpr f32 kCell = 40;       // junction lookup grid
constexpr f32 kMerge = 3;       // street vertices closer than this are one node
constexpr f32 kDecel = 3.5f;    // comfortable braking to a stop line (m/s^2)
constexpr f32 kGreen = 14, kAmber = 3, kAllRed = 2, kCycle = 2 * (kGreen + kAmber + kAllRed);
u64 cell(i32 x, i32 y) { return (static_cast<u64>(static_cast<u32>(x)) << 32) | static_cast<u32>(y); }
}  // namespace

void TrafficRules::rebuild(const std::vector<Lane>& lanes) {
  junctions_.clear();
  grid_.clear();
  blocked_.clear();
  struct Node { Vec2 p; std::vector<Arm> arms; };
  std::vector<Node> nodes;
  std::unordered_map<u64, std::vector<i32>> cells;
  auto node_at = [&](Vec2 p) -> i32 {
    const i32 cx = static_cast<i32>(std::floor(p.x / kMerge)), cy = static_cast<i32>(std::floor(p.y / kMerge));
    for (i32 dy = -1; dy <= 1; ++dy)
      for (i32 dx = -1; dx <= 1; ++dx) {
        auto it = cells.find(cell(cx + dx, cy + dy));
        if (it == cells.end()) continue;
        for (i32 i : it->second)
          if (std::hypot(nodes[i].p.x - p.x, nodes[i].p.y - p.y) < kMerge) return i;
      }
    nodes.push_back({p, {}});
    cells[cell(cx, cy)].push_back(static_cast<i32>(nodes.size() - 1));
    return static_cast<i32>(nodes.size() - 1);
  };
  for (const Lane& l : lanes) {
    if (!l.alive || l.points.size() < 2) continue;
    for (std::size_t i = 0; i < l.points.size(); ++i) {
      const Vec2 p = l.points[i];
      auto arm = [&](Vec2 q) {
        const f32 d = std::hypot(q.x - p.x, q.y - p.y);
        if (d < 0.5f) return;
        Node& n = nodes[node_at(p)];
        const Vec2 dir{(q.x - p.x) / d, (q.y - p.y) / d};
        for (const Arm& a : n.arms)
          if (a.dir.x * dir.x + a.dir.y * dir.y > 0.97f) return;  // the same street again (tile border)
        n.arms.push_back({dir, l.width});
      };
      if (i > 0) arm(l.points[i - 1]);
      if (i + 1 < l.points.size()) arm(l.points[i + 1]);
    }
  }
  for (const Node& n : nodes) {
    if (n.arms.size() < 3) continue;
    Junction j;
    j.p = n.p;
    j.arms = n.arms;
    f32 wmax = 0;
    for (const Arm& a : n.arms) wmax = std::fmax(wmax, a.width);
    j.lights = wmax >= 9;
    j.radius = std::fmax(6.0f, wmax * 0.5f + 3.5f);
    // The axis: the widest arm (the main road)
    for (const Arm& a : n.arms)
      if (a.width >= wmax) { j.axis = a.dir; break; }
    const u32 h = static_cast<u32>(static_cast<i32>(n.p.x * 7.0f)) * 73856093u ^ static_cast<u32>(static_cast<i32>(n.p.y * 7.0f)) * 19349663u;
    j.offset = static_cast<f32>(h % 3800) / 100.0f;
    junctions_.push_back(j);
    const i32 id = static_cast<i32>(junctions_.size() - 1);
    grid_[cell(static_cast<i32>(std::floor(n.p.x / kCell)), static_cast<i32>(std::floor(n.p.y / kCell)))].push_back(id);
  }
}

void TrafficRules::near(Vec2 p, f32 r, std::vector<i32>& out) const {
  out.clear();
  const i32 cx0 = static_cast<i32>(std::floor((p.x - r) / kCell)), cx1 = static_cast<i32>(std::floor((p.x + r) / kCell));
  const i32 cy0 = static_cast<i32>(std::floor((p.y - r) / kCell)), cy1 = static_cast<i32>(std::floor((p.y + r) / kCell));
  for (i32 y = cy0; y <= cy1; ++y)
    for (i32 x = cx0; x <= cx1; ++x) {
      auto it = grid_.find(cell(x, y));
      if (it == grid_.end()) continue;
      for (i32 j : it->second)
        if (std::hypot(junctions_[j].p.x - p.x, junctions_[j].p.y - p.y) <= r) out.push_back(j);
    }
}

i32 TrafficRules::light(const Junction& j, Vec2 heading, f32 now) const {
  if (!j.lights) return -1;
  const bool along = std::fabs(heading.x * j.axis.x + heading.y * j.axis.y) > 0.7071f;
  f32 t = std::fmod(now + j.offset, kCycle);
  if (!along) t = std::fmod(t + kCycle - (kGreen + kAmber + kAllRed), kCycle);
  if (t < kGreen) return 0;
  if (t < kGreen + kAmber) return 1;
  return 2;
}

f32 TrafficRules::limit(i32 car, Vec2 pos, Vec2 fwd, f32 speed, bool bus, f32 now, const std::vector<Vec3>& cars, const std::vector<Vec2>& people, Vec2 player,
                        bool player_on_foot, View& view) {
  view = View{};
  f32 lim = 1e9f;
  const f32 v = std::fmax(0.0f, speed);
  // 1. people in the corridor (yieldsToPedestrian)
  {
    const f32 reach = (bus ? 9.0f : 6.0f) + v * 0.5f + v * v / 9, half = bus ? 1.95f : 1.6f;
    auto in_lane = [&](Vec2 q) {
      const f32 dx = q.x - pos.x, dy = q.y - pos.y, along = dx * fwd.x + dy * fwd.y;
      return along >= -0.5f && along <= reach && std::fabs(-dx * fwd.y + dy * fwd.x) < half;
    };
    bool by_player = player_on_foot && in_lane(player);
    bool yielding = by_player;
    for (std::size_t i = 0; i < people.size() && !yielding; ++i) yielding = in_lane(people[i]);
    Blocked& b = blocked_[car];
    if (yielding) {
      lim = 0;
      view.yielding = true;
      if (b.since < 0) b.since = now;
      // honkCheck: the player sooner, a pedestrian after a while; then 9..16 s apart
      if (now - b.since > (by_player ? 2.6f : 7.0f) && now >= b.next_honk) {
        view.honk = true;
        b.next_honk = now + 9 + static_cast<f32>((static_cast<u32>(car) * 2654435761u + static_cast<u32>(now)) % 700) / 100;
      }
    } else b.since = -1;
  }
  // 2. the next junction ahead
  std::vector<i32> cand;
  near(pos + fwd * 25.0f, 34, cand);
  f32 best = 1e9f;
  for (i32 ji : cand) {
    Junction& j = junctions_[ji];
    const Vec2 d{j.p.x - pos.x, j.p.y - pos.y};
    const f32 along = d.x * fwd.x + d.y * fwd.y, lateral = std::fabs(-d.x * fwd.y + d.y * fwd.x);
    const f32 dist = std::hypot(d.x, d.y);
    // inside: claim it (first come, first served) and drive on
    if (dist < j.radius) {
      if (j.claim < 0 || j.claim == car || now - j.claim_at > 8) { j.claim = car; j.claim_at = now; }
      continue;
    }
    if (j.claim == car && dist > j.radius + 3) j.claim = -1;  // left it
    if (along < 0 || lateral > j.radius + 2 || along >= best) continue;
    best = along;
    view.junction = ji;
    view.distance = along - j.radius;
  }
  if (view.junction < 0) blocked_[car].wait_since = -1;
  if (view.junction >= 0) {
    Junction& j = junctions_[view.junction];
    const f32 stop = std::fmax(0.0f, view.distance - 1.5f);  // the stop line 1.5 m before the junction
    bool must_stop = false;
    view.light = light(j, fwd, now);
    if (view.light == 2) must_stop = true;
    else if (view.light == 1) must_stop = stop > v * v / (2 * 4.0f);  // amber: stop if it can
    else if (view.light < 0 && view.distance < 14) {
      // Unsignalled: wait while someone else holds the junction (and is still in it).
      if (j.claim >= 0 && j.claim != car && j.claim < static_cast<i32>(cars.size()) && now - j.claim_at < 8) {
        const Vec3 c = cars[j.claim];
        if (c.z > -1e8f && std::hypot(c.x - j.p.x, c.y - j.p.y) < j.radius + 3) must_stop = true;
      }
      // Right before left: a car close to the edge on the right goes first (after 4 s of waiting
      // a driver goes anyway: four cars at a four-way junction would otherwise wait for ever).
      Blocked& b = blocked_[car];
      if (!must_stop && (b.wait_since < 0 || now - b.wait_since < 4))
        for (std::size_t k = 0; k < cars.size(); ++k) {
          if (static_cast<i32>(k) == car || cars[k].z < -1e8f) continue;
          const Vec2 o{cars[k].x - j.p.x, cars[k].y - j.p.y};
          const f32 od = std::hypot(o.x, o.y);
          if (od < j.radius || od > j.radius + 10 || od > view.distance + j.radius + 2) continue;
          const Vec2 me{pos.x - j.p.x, pos.y - j.p.y};
          if (me.x * o.y - me.y * o.x > 0 && std::fabs((me.x * o.x + me.y * o.y) / (std::hypot(me.x, me.y) * od)) < 0.5f) { must_stop = true; break; }
        }
    }
    {
      Blocked& b = blocked_[car];
      if (must_stop && v < 0.5f && b.wait_since < 0) b.wait_since = now;
      if (view.distance > 14 || view.light >= 0) b.wait_since = -1;
    }
    if (must_stop) {
      view.wait = true;
      lim = std::fmin(lim, std::sqrt(2 * kDecel * stop));
    }
  }
  return lim;
}

}  // namespace sc
