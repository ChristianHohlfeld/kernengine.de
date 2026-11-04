// Street graph and A* (see road_graph.h).
#include "road_graph.h"

#include <algorithm>
#include <cmath>
#include <queue>

namespace sc {

namespace {
constexpr f32 kCell = 3.5f;  // vertices closer than this (same grid cell or a neighbour) merge
u64 cell_key(i32 x, i32 y) { return (static_cast<u64>(static_cast<u32>(x)) << 32) | static_cast<u32>(y); }
f32 dist(Vec2 a, Vec2 b) { return std::hypot(a.x - b.x, a.y - b.y); }
}  // namespace

u64 lane_signature(const std::vector<Lane>& lanes) {
  u64 h = 1469598103934665603ull;
  for (std::size_t i = 0; i < lanes.size(); ++i) {
    h ^= (static_cast<u64>(i) << 1) | (lanes[i].alive ? 1u : 0u);
    h *= 1099511628211ull;
  }
  return h;
}

i32 RoadGraph::node_at(Vec2 p) {
  const i32 cx = static_cast<i32>(std::floor(p.x / kCell)), cy = static_cast<i32>(std::floor(p.y / kCell));
  for (i32 dx = -1; dx <= 1; ++dx)
    for (i32 dy = -1; dy <= 1; ++dy) {
      auto it = cells_.find(cell_key(cx + dx, cy + dy));
      if (it != cells_.end() && dist(nodes_[it->second].p, p) < kCell) return it->second;
    }
  const i32 id = static_cast<i32>(nodes_.size());
  nodes_.push_back({p, {}});
  cells_[cell_key(cx, cy)] = id;
  return id;
}

void RoadGraph::rebuild(const std::vector<Lane>& lanes) {
  nodes_.clear();
  cells_.clear();
  for (const Lane& l : lanes) {
    if (!l.alive || l.points.size() < 2) continue;
    i32 prev = node_at(l.points[0]);
    for (std::size_t i = 1; i < l.points.size(); ++i) {
      const i32 cur = node_at(l.points[i]);
      if (cur == prev) continue;
      const f32 len = dist(nodes_[prev].p, nodes_[cur].p);
      auto link = [&](i32 a, i32 b) {
        for (auto& e : nodes_[a].edges)
          if (e.first == b) return;
        nodes_[a].edges.push_back({b, len});
      };
      link(prev, cur);
      link(cur, prev);
      prev = cur;
    }
  }
  built_from = lane_signature(lanes);
}

i32 RoadGraph::nearest(Vec2 p, f32 max_m) const {
  i32 best = -1;
  f32 bd = max_m;
  for (std::size_t i = 0; i < nodes_.size(); ++i) {
    const f32 d = dist(nodes_[i].p, p);
    if (d < bd) { bd = d; best = static_cast<i32>(i); }
  }
  return best;
}

bool RoadGraph::path(Vec2 from, Vec2 to, std::vector<Vec2>& out) const {
  out.clear();
  const i32 a = nearest(from), b = nearest(to);
  if (a < 0 || b < 0) return false;
  const std::size_t n = nodes_.size();
  std::vector<f32> g(n, 1e30f);
  std::vector<i32> came(n, -1);
  std::vector<u8> closed(n, 0);
  using Item = std::pair<f32, i32>;
  std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
  g[a] = 0;
  open.push({dist(nodes_[a].p, nodes_[b].p), a});
  u32 expanded = 0;
  while (!open.empty()) {
    const i32 cur = open.top().second;
    open.pop();
    if (closed[cur]) continue;
    closed[cur] = 1;
    if (cur == b || ++expanded > 20000) break;
    for (const auto& [nb, len] : nodes_[cur].edges) {
      const f32 ng = g[cur] + len;
      if (ng < g[nb]) {
        g[nb] = ng;
        came[nb] = cur;
        open.push({ng + dist(nodes_[nb].p, nodes_[b].p), nb});
      }
    }
  }
  if (a != b && came[b] < 0) return false;
  std::vector<Vec2> rev;
  for (i32 c = b; c >= 0; c = c == a ? -1 : came[c]) rev.push_back(nodes_[c].p);
  out.push_back(from);
  for (auto it = rev.rbegin(); it != rev.rend(); ++it) out.push_back(*it);
  out.push_back(to);
  return true;
}

Vec2 RoadGraph::pursuit_point(const std::vector<Vec2>& path, Vec2 p, f32 look) {
  if (path.empty()) return p;
  if (path.size() < 2) return path[0];
  std::size_t bi = 0;
  f32 bt = 0, bd = 1e30f;
  for (std::size_t i = 0; i + 1 < path.size(); ++i) {
    const Vec2 a = path[i], b = path[i + 1], d = b - a;
    const f32 l2 = std::fmax(1e-6f, d.x * d.x + d.y * d.y);
    const f32 t = clampf(((p.x - a.x) * d.x + (p.y - a.y) * d.y) / l2, 0, 1);
    const f32 dd = dist(p, a + d * t);
    if (dd < bd) { bd = dd; bi = i; bt = t; }
  }
  f32 left = look, t = bt;
  for (std::size_t i = bi; i + 1 < path.size(); ++i) {
    const Vec2 a = path[i], b = path[i + 1];
    const f32 l = std::fmax(1e-6f, dist(a, b)), rest = (1 - t) * l;
    if (rest >= left) return a + (b - a) * (t + left / l);
    left -= rest;
    t = 0;
  }
  return path.back();
}

}  // namespace sc
