// Traffic rules at junctions (see world_traffic.h).
#include "world_traffic.h"

#include <algorithm>
#include <cmath>

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
  // Streets that cross without a shared vertex (a tile cut, the stand-in town's straight streets
  // laid across each other) meet where their segments intersect: that point is a node with an arm
  // along each street in both directions.
  {
    struct Seg { Vec2 a, b; f32 width; u32 lane; };
    std::vector<Seg> segs;
    std::unordered_map<u64, std::vector<u32>> buckets;
    for (u32 li = 0; li < lanes.size(); ++li) {
      const Lane& l = lanes[li];
      if (!l.alive || l.points.size() < 2) continue;
      for (std::size_t i = 0; i + 1 < l.points.size(); ++i) {
        const Seg s{l.points[i], l.points[i + 1], l.width, li};
        const u32 si = static_cast<u32>(segs.size());
        segs.push_back(s);
        const i32 x0 = static_cast<i32>(std::floor(std::fmin(s.a.x, s.b.x) / kCell)), x1 = static_cast<i32>(std::floor(std::fmax(s.a.x, s.b.x) / kCell));
        const i32 y0 = static_cast<i32>(std::floor(std::fmin(s.a.y, s.b.y) / kCell)), y1 = static_cast<i32>(std::floor(std::fmax(s.a.y, s.b.y) / kCell));
        for (i32 x = x0; x <= x1; ++x)
          for (i32 y = y0; y <= y1; ++y) buckets[cell(x, y)].push_back(si);
      }
    }
    for (const auto& [key, list] : buckets) {
      (void)key;
      for (std::size_t m = 0; m < list.size(); ++m)
        for (std::size_t n = m + 1; n < list.size(); ++n) {
          const Seg& s = segs[list[m]];
          const Seg& t = segs[list[n]];
          if (s.lane == t.lane) continue;
          const Vec2 r = s.b - s.a, q = t.b - t.a;
          const f32 den = r.x * q.y - r.y * q.x;
          if (std::fabs(den) < 1e-6f) continue;  // parallel
          const Vec2 w = t.a - s.a;
          const f32 u = (w.x * q.y - w.y * q.x) / den, v = (w.x * r.y - w.y * r.x) / den;
          if (u < 0 || u > 1 || v < 0 || v > 1) continue;
          const Vec2 p = s.a + r * u;
          const i32 ni = node_at(p);
          for (const Seg* g : {&s, &t})
            for (const Vec2 end : {g->a, g->b}) {
              const f32 d = std::hypot(end.x - p.x, end.y - p.y);
              if (d < 0.5f) continue;
              const Vec2 dir{(end.x - p.x) / d, (end.y - p.y) / d};
              bool dup = false;
              for (const Arm& a : nodes[ni].arms) dup = dup || a.dir.x * dir.x + a.dir.y * dir.y > 0.97f;
              if (!dup) nodes[ni].arms.push_back({dir, g->width});
            }
        }
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
    // The axis: the widest arm (the main road); of several as wide, the one with the smallest
    // bearing - the same on every machine, whatever order its streets came in
    {
      f32 best = 1e9f;
      for (const Arm& a : n.arms) {
        if (a.width < wmax) continue;
        f32 bearing = std::atan2(a.dir.y, a.dir.x);
        if (bearing < 0) bearing += 3.14159265f;
        if (bearing >= 3.14159265f - 1e-3f) bearing = 0;
        if (bearing < best) { best = bearing; j.axis = a.dir; }
      }
    }
    // The signal posts: each on the kerb corner right of its arm's incoming traffic - between
    // this arm and the next one round the junction, 1.2 m off both carriageways (never on a
    // street: not on its own arm, not on the one crossing it).
    for (Arm& a : j.arms) {
      const f32 aa = std::atan2(a.dir.y, a.dir.x);
      f32 best = 6.2831853f;
      const Arm* next = nullptr;
      for (const Arm& b : j.arms) {
        if (&b == &a) continue;
        f32 d = std::atan2(b.dir.y, b.dir.x) - aa;
        while (d <= 1e-4f) d += 6.2831853f;
        if (d < best) { best = d; next = &b; }
      }
      const f32 half = best / 2;
      if (!next || best >= 3.1415927f) {
        // no arm on that side: abeam of the junction, beside the arm's own kerb
        a.pole = Vec2{-a.dir.y, a.dir.x} * (a.width * 0.5f + 1.2f);
        a.has_pole = true;
        continue;
      }
      const f32 s = std::sin(half);
      const f32 r = (std::fmax(a.width, next->width) * 0.5f + 1.2f) / std::fmax(s, 0.05f);
      if (r > 30) continue;  // two arms nearly along each other: no corner to stand on
      a.pole = Vec2{std::cos(aa + half), std::sin(aa + half)} * r;
      a.has_pole = true;
    }
    const u32 h = static_cast<u32>(static_cast<i32>(std::lround(n.p.x / 3))) * 73856093u ^ static_cast<u32>(static_cast<i32>(std::lround(n.p.y / 3))) * 19349663u;
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
  // (the shared wall clock when the game has set it: every machine shows the same phase)
  f32 t = std::fmod((lights_time >= 0 ? lights_time : now) + j.offset, kCycle);
  if (!along) t = std::fmod(t + kCycle - (kGreen + kAmber + kAllRed), kCycle);
  if (t < kGreen) return 0;
  if (t < kGreen + kAmber) return 1;
  return 2;
}

TrafficRules::Pursuit TrafficRules::pursuit(const Lane& lane, i32 index, i32 dir, Vec2 pos, f32 look, f32 off) {
  Pursuit out;
  const i32 n = static_cast<i32>(lane.points.size());
  if (n < 2) { out.target = n ? lane.points[0] : pos; out.passed = true; return out; }
  dir = dir < 0 ? -1 : 1;
  i32 i = std::clamp(index, 0, n - 1);
  if (i - dir < 0 || i - dir >= n) i += dir;  // (the lane's first point in this direction: the stretch after it)
  Vec2 a = lane.points[i - dir], b = lane.points[i];
  auto along = [](Vec2 from, Vec2 to, Vec2& t) { const Vec2 d = to - from; const f32 l = std::hypot(d.x, d.y); if (l > 1e-3f) t = d * (1.0f / l); return l; };
  Vec2 t{1, 0};
  f32 len = along(a, b, t);
  // where the car is along the stretch that leads to lane point `index`
  const f32 at = (pos.x - a.x) * t.x + (pos.y - a.y) * t.y;
  out.passed = i != std::clamp(index, 0, n - 1) ? false : at >= len - 0.5f;
  const f32 s = clampf(at, 0, len);
  Vec2 cur = a + t * s;
  f32 left = len - s, want = std::fmax(0.0f, look);
  out.end = left;
  for (i32 k = i; k + dir >= 0 && k + dir < n; k += dir) out.end += std::hypot(lane.points[k + dir].x - lane.points[k].x, lane.points[k + dir].y - lane.points[k].y);
  while (want > left) {
    if (i + dir < 0 || i + dir >= n) { want = left; break; }  // the lane ends here
    want -= left;
    cur = b;
    a = b;
    i += dir;
    b = lane.points[i];
    left = along(a, b, t);
  }
  out.tangent = t;
  out.target = cur + t * want + Vec2{t.y, -t.x} * off;
  return out;
}

f32 TrafficRules::gap_speed(i32 self, Vec2 pos, Vec2 fwd, bool bus, const std::vector<Vehicle>& all, i32* limiter) {
  const f32 half = bus ? 1.6f : 1.25f, own = bus ? 4.0f : 1.9f;
  f32 gap = 1e9f;
  i32 who = -1;
  for (std::size_t i = 0; i < all.size(); ++i) {
    const Vehicle& o = all[i];
    if (!o.there || static_cast<i32>(i) == self) continue;
    const f32 dx = o.p.x - pos.x, dy = o.p.y - pos.y;
    if (std::fabs(dx) > 40 || std::fabs(dy) > 40) continue;
    const f32 along = dx * fwd.x + dy * fwd.y;
    if (along < 1 || along > 40) continue;
    if (std::fabs(-dx * fwd.y + dy * fwd.x) > half + 0.6f) continue;
    const f32 g = along - ((o.bus ? 4.0f : 1.9f) + own);
    if (g < gap) { gap = g; who = static_cast<i32>(i); }
  }
  if (limiter) *limiter = who;
  if (who < 0) return 1e9f;
  return std::fmax(0.0f, gap - 2.5f) / 1.6f;
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
