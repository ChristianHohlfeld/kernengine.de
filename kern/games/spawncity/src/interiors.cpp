// Walkable buildings (see interiors.h). Values from building_interiors.mjs.
#include "interiors.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "kern/geo/earcut.h"
#include "life_art.h"

namespace sc {

namespace {
namespace ph = kern::physics;
using kern::Mat4;

// building_interiors.mjs
constexpr f32 kActivate = 30, kDoorsRadius = 95, kDoorW = 1.8f, kDoorH = 2.55f, kWallIn = 0.3f;
constexpr u32 kMaxClosedDoors = 24, kMaxLevels = 12;
constexpr f32 kLane = 1.2f, kCoreW = 2 * kLane + 0.2f, kLand = 1.3f, kRun = 3.6f, kCoreL = 2 * kLand + kRun, kSlab = 0.25f, kRoofHouse = 2.7f;
constexpr f32 kRelease = 45;  // a little more than ACTIVATE: no flicker at the edge

Vec3 rgb(u32 h) { return {((h >> 16) & 255) / 255.0f, ((h >> 8) & 255) / 255.0f, (h & 255) / 255.0f}; }
const Vec3 kWall = rgb(0xe9e4da), kStairWall = rgb(0xdad8d2), kAccent = rgb(0x7f9a8a), kOak = rgb(0x8a5f3a), kStone = rgb(0x7a746b), kCeil = rgb(0xedebe6),
           kConcrete = rgb(0x767675), kRiser = rgb(0xa9a39a), kDark = rgb(0x24262a), kTrim = rgb(0x5e3f2a);

f32 signed_area(const std::vector<Vec2>& r) {
  f32 a = 0;
  for (std::size_t i = 0, j = r.size() - 1; i < r.size(); j = i++) a += r[j].x * r[i].y - r[i].x * r[j].y;
  return a * 0.5f;
}
std::vector<Vec2> clean(const std::vector<Vec2>& in) {
  std::vector<Vec2> r;
  for (Vec2 p : in) {
    if (!std::isfinite(p.x) || !std::isfinite(p.y)) continue;
    if (!r.empty() && std::hypot(r.back().x - p.x, r.back().y - p.y) < 0.05f) continue;
    r.push_back(p);
  }
  if (r.size() > 2 && std::hypot(r[0].x - r.back().x, r[0].y - r.back().y) < 0.05f) r.pop_back();
  return r;
}
bool pip(Vec2 p, const std::vector<Vec2>& r) {
  bool c = false;
  for (std::size_t i = 0, j = r.size() - 1; i < r.size(); j = i++) {
    const Vec2 a = r[i], b = r[j];
    if ((a.y > p.y) != (b.y > p.y) && p.x < (b.x - a.x) * (p.y - a.y) / ((b.y - a.y) == 0 ? 1e-9f : (b.y - a.y)) + a.x) c = !c;
  }
  return c;
}
f32 seg_dist(Vec2 p, Vec2 a, Vec2 b) {
  const Vec2 d = b - a;
  const f32 l = std::fmax(1e-9f, d.x * d.x + d.y * d.y), t = clampf(((p.x - a.x) * d.x + (p.y - a.y) * d.y) / l, 0, 1);
  return std::hypot(p.x - a.x - d.x * t, p.y - a.y - d.y * t);
}
f32 ring_dist(Vec2 p, const std::vector<Vec2>& r) {
  f32 d = 1e9f;
  for (std::size_t i = 0, j = r.size() - 1; i < r.size(); j = i++) d = std::fmin(d, seg_dist(p, r[j], r[i]));
  return d;
}
// Offset to the LEFT of the travel direction (inside of a CCW outer ring, solid side of a CW hole
// ring) with clamped miters (building_interiors.mjs offsetRing).
std::vector<Vec2> offset_ring(const std::vector<Vec2>& r, f32 d) {
  const std::size_t n = r.size();
  std::vector<Vec2> out;
  for (std::size_t i = 0; i < n; ++i) {
    const Vec2 p = r[(i + n - 1) % n], c = r[i], q = r[(i + 1) % n];
    Vec2 a = c - p, b = q - c;
    const f32 la = std::fmax(1e-6f, std::hypot(a.x, a.y)), lb = std::fmax(1e-6f, std::hypot(b.x, b.y));
    a = a * (1 / la);
    b = b * (1 / lb);
    const Vec2 n0{-a.y, a.x}, n1{-b.y, b.x};
    const f32 dot = n0.x * n1.x + n0.y * n1.y;
    const f32 k = 1 / std::fmax(0.25f, 1 + dot);
    out.push_back(c + (n0 + n1) * (k * d));
  }
  return out;
}
u32 hash_key(u32 seed, u32 salt) {
  u32 x = seed * 0x9e3779b1u + salt * 0x85ebca6bu;
  x ^= x >> 16; x *= 0x7feb352du; x ^= x >> 15; x *= 0x846ca68bu; x ^= x >> 16;
  return x;
}
f32 h01(u32 seed, u32 salt) { return static_cast<f32>(hash_key(seed, salt) >> 8) / 16777216.0f; }

// ---- geometry sinks: the triangles of a plan - drawn (its mesh) and for collision (its points
// and indices)
void vtri(Interiors::Plan& P, Vec3 a, Vec3 b, Vec3 c, Vec3 col) {
  const Vec3 n = kern::normalize(kern::cross(b - a, c - a), {0, 0, 1});
  for (Vec3 p : {a, b, c}) P.mesh.push(p, n, col, {p.x * 0.5f + p.z * 0.3f, p.y * 0.5f});
}
void vquad(Interiors::Plan& P, Vec3 a, Vec3 b, Vec3 c, Vec3 d, Vec3 col) { vtri(P, a, b, c, col); vtri(P, a, c, d, col); }
void ctri(Interiors::Plan& P, Vec3 a, Vec3 b, Vec3 c) {
  if (kern::length(kern::cross(b - a, c - a)) < 1e-4f) return;  // Box3D rejects degenerate triangles
  const u32 base = static_cast<u32>(P.points.size());
  P.points.insert(P.points.end(), {a, b, c});
  P.indices.insert(P.indices.end(), {base, base + 1, base + 2});
}
void cquad(Interiors::Plan& P, Vec3 a, Vec3 b, Vec3 c, Vec3 d, bool two_sided) {
  ctri(P, a, b, c); ctri(P, a, c, d);
  if (two_sided) { ctri(P, a, c, b); ctri(P, a, d, c); }
}
// a vertical wall piece between two ground points
void wall(Interiors::Plan& P, Vec2 p0, Vec2 p1, f32 z0, f32 z1, Vec3 col, bool visual, bool collide) {
  if (z1 - z0 < 0.01f || std::hypot(p1.x - p0.x, p1.y - p0.y) < 0.01f) return;
  const Vec3 a{p0.x, p0.y, z0}, b{p1.x, p1.y, z0}, c{p1.x, p1.y, z1}, d{p0.x, p0.y, z1};
  if (visual) vquad(P, a, b, c, d, col);
  if (collide) cquad(P, a, b, c, d, true);
}
// a horizontal polygon with holes
void slab(Interiors::Plan& P, const std::vector<Vec2>& outer, const std::vector<std::vector<Vec2>>& holes, f32 z, Vec3 col, bool up, bool visual, bool collide) {
  std::vector<f64> xy;
  std::vector<u32> hs, tris;
  for (Vec2 p : outer) xy.insert(xy.end(), {p.x, p.y});
  for (const auto& h : holes) {
    if (h.size() < 3) continue;
    hs.push_back(static_cast<u32>(xy.size() / 2));
    for (Vec2 p : h) xy.insert(xy.end(), {p.x, p.y});
  }
  if (xy.size() < 6) return;
  kern::Earcut ec;
  ec.triangulate(xy.data(), static_cast<u32>(xy.size() / 2), hs.data(), static_cast<u32>(hs.size()), tris);
  for (std::size_t i = 0; i + 2 < tris.size(); i += 3) {
    Vec3 p[3];
    for (int k = 0; k < 3; ++k) p[k] = {static_cast<f32>(xy[tris[i + k] * 2]), static_cast<f32>(xy[tris[i + k] * 2 + 1]), z};
    if ((kern::cross(p[1] - p[0], p[2] - p[0]).z < 0) == up) std::swap(p[1], p[2]);
    if (visual) vtri(P, p[0], p[1], p[2], col);
    if (collide) ctri(P, p[0], p[1], p[2]);
  }
}
void box(Interiors::Plan& P, Vec3 c, Vec3 size, f32 yaw, Vec3 col, bool collide) {
  const Vec3 h = size * 0.5f;
  const Quat q = yaw_quat(yaw);
  Vec3 v[8];
  for (int i = 0; i < 8; ++i) v[i] = c + q.rotate({i & 1 ? h.x : -h.x, i & 2 ? h.y : -h.y, i & 4 ? h.z : -h.z});
  const int f[6][4] = {{4, 5, 7, 6}, {0, 2, 3, 1}, {0, 1, 5, 4}, {2, 6, 7, 3}, {1, 3, 7, 5}, {0, 4, 6, 2}};
  for (const auto& q4 : f) {
    vquad(P, v[q4[0]], v[q4[1]], v[q4[2]], v[q4[3]], col);
    if (collide && q4[0] != 0) cquad(P, v[q4[0]], v[q4[1]], v[q4[2]], v[q4[3]], false);
  }
}

struct CoreFit { bool ok = false; Interiors::Core core; };

CoreFit find_core(const std::vector<Vec2>& inner, const std::vector<std::vector<Vec2>>& inner_holes, const std::vector<std::vector<Vec2>>& holes,
                  const std::vector<Interiors::Door>& doors, Vec2 centroid, Vec2 axis) {
  f32 x0 = 1e9f, y0 = 1e9f, x1 = -1e9f, y1 = -1e9f;
  for (Vec2 p : inner) { x0 = std::fmin(x0, p.x); x1 = std::fmax(x1, p.x); y0 = std::fmin(y0, p.y); y1 = std::fmax(y1, p.y); }
  const f32 span = std::fmax(x1 - x0, y1 - y0), step = std::fmax(0.8f, span / 18);
  std::vector<std::pair<f32, Vec2>> cands;
  for (f32 x = x0; x <= x1; x += step)
    for (f32 y = y0; y <= y1; y += step) cands.push_back({std::hypot(x - centroid.x, y - centroid.y), {x, y}});
  std::stable_sort(cands.begin(), cands.end(), [](const std::pair<f32, Vec2>& a, const std::pair<f32, Vec2>& b) { return a.first < b.first; });
  auto ok_point = [&](Vec2 p, f32 clear) {
    if (!pip(p, inner) || ring_dist(p, inner) < clear) return false;
    for (const auto& h : inner_holes)
      if (pip(p, h) || ring_dist(p, h) < clear) return false;
    return true;
  };
  const Vec2 dirs[4] = {axis, axis * -1.0f, {-axis.y, axis.x}, {axis.y, -axis.x}};
  for (std::size_t i = 0; i < cands.size() && i < 500; ++i) {
    const Vec2 c = cands[i].second;
    bool near_door = false;
    for (const Interiors::Door& d : doors) near_door = near_door || std::hypot(d.c.x - c.x, d.c.y - c.y) < kCoreL * 0.5f + 3.2f;
    if (near_door) continue;
    for (Vec2 v : dirs) {
      const Vec2 u{-v.y, v.x};
      const Vec2 o = c - u * (kCoreW / 2) - v * (kCoreL / 2);
      auto P = [&](f32 a, f32 b) { return o + u * a + v * b; };
      const Vec2 pts[7] = {P(0, 0), P(kCoreW, 0), P(0, kCoreL), P(kCoreW, kCoreL), P(kCoreW / 2, kCoreL / 2), P(0, kCoreL / 2), P(kCoreW, kCoreL / 2)};
      bool all = true;
      for (Vec2 p : pts) all = all && ok_point(p, 0.12f);
      if (!all || !ok_point(P(kCoreW / 2, -1.4f), 0.7f)) continue;
      const std::vector<Vec2> rect = {P(0, 0), P(kCoreW, 0), P(kCoreW, kCoreL), P(0, kCoreL)};
      bool hole_in = false;
      for (const auto& h : holes)
        for (Vec2 p : h) hole_in = hole_in || pip(p, rect);
      if (hole_in) continue;
      return {true, {o, u, v}};
    }
  }
  return {};
}

// door gap [t0, t1] along the edge a->b (metres from a) for doors on it, else none
bool door_gap(const std::vector<Interiors::Door>& doors, Vec2 a, Vec2 b, f32 inset, f32& t0, f32& t1) {
  const Vec2 d = b - a;
  const f32 len = std::hypot(d.x, d.y);
  if (len < 1e-3f) return false;
  const Vec2 u = d * (1 / len);
  for (const Interiors::Door& door : doors) {
    const Vec2 c = door.c - door.n * inset;  // the door centre moved onto this ring
    if (seg_dist(c, a, b) > 0.15f + inset * 0.2f) continue;
    if (std::fabs(u.x * door.d.x + u.y * door.d.y) < 0.9f) continue;
    const f32 t = (c.x - a.x) * u.x + (c.y - a.y) * u.y;
    t0 = t - kDoorW / 2;
    t1 = t + kDoorW / 2;
    return true;
  }
  return false;
}
}  // namespace

std::vector<Interiors::Door> Interiors::doors_of(const std::vector<Vec2>& outer, f32 area) {
  struct Edge { f32 len; Vec2 a, d, n; };
  std::vector<Edge> edges;
  for (std::size_t i = 0; i < outer.size(); ++i) {
    const Vec2 a = outer[i], b = outer[(i + 1) % outer.size()];
    const f32 len = std::hypot(b.x - a.x, b.y - a.y);
    if (len < kDoorW + 1.4f) continue;
    const Vec2 d = (b - a) * (1 / len);
    edges.push_back({len, a, d, {d.y, -d.x}});
  }
  std::stable_sort(edges.begin(), edges.end(), [](const Edge& p, const Edge& q) { return p.len > q.len; });
  std::vector<Door> out;
  if (edges.empty()) return out;
  auto mk = [](const Edge& e) { return Door{e.a + e.d * (e.len / 2), e.d, e.n}; };
  out.push_back(mk(edges[0]));
  if (area > 260)
    for (std::size_t i = 1; i < edges.size(); ++i)
      if (edges[i].n.x * edges[0].n.x + edges[i].n.y * edges[0].n.y < -0.3f) { out.push_back(mk(edges[i])); break; }
  return out;
}

bool Interiors::build(const MapFeatures::Building& b, f32 door_ground, Plan& P) {
  P = Plan{};
  if (b.rings.empty()) return false;
  P.outer = clean(b.rings[0]);
  if (P.outer.size() < 3) return false;
  const bool reversed = signed_area(P.outer) < 0;  // Kern's facade counts along the original ring
  if (reversed) std::reverse(P.outer.begin(), P.outer.end());
  for (std::size_t i = 1; i < b.rings.size(); ++i) {
    std::vector<Vec2> h = clean(b.rings[i]);
    if (h.size() < 3) continue;
    if (signed_area(h) > 0) std::reverse(h.begin(), h.end());
    P.holes.push_back(std::move(h));
  }
  P.area = signed_area(P.outer);
  for (const auto& h : P.holes) P.area -= std::fabs(signed_area(h));
  if (P.area < 24 || P.area > 40000) return false;
  P.inner = offset_ring(P.outer, kWallIn);
  for (const auto& h : P.holes) P.inner_holes.push_back(offset_ring(h, kWallIn));
  if (signed_area(P.inner) < 8) return false;
  P.doors = doors_of(P.outer, P.area);
  if (P.doors.empty()) return false;
  Vec2 centroid{};
  for (Vec2 p : P.outer) centroid = centroid + p * (1.0f / static_cast<f32>(P.outer.size()));
  Vec2 axis{1, 0};
  f32 best = 0;
  for (std::size_t i = 0; i < P.outer.size(); ++i) {
    const Vec2 d = P.outer[(i + 1) % P.outer.size()] - P.outer[i];
    const f32 l = std::hypot(d.x, d.y);
    if (l > best) { best = l; axis = d * (1 / l); }
  }
  // storeys line up with the facade: Kern's BuildingBuilder draws every storey at
  // 3.1 + 0.5 * ((seed >> 12) % 1000) / 1000 m (the windows' rows), as SpawnCity's interiors follow
  // its facade shader (building_interiors.mjs: facadeParams floorH)
  const u32 seed = b.seed;
  P.floor_h = 3.1f + 0.5f * static_cast<f32>((seed >> 12) % 1000) / 1000.0f;
  const f32 ground_storey = P.floor_h;
  const f32 base0 = std::fmax(b.base, door_ground) + 0.03f;
  P.base = b.base;
  P.top = b.top;
  if (P.top - base0 < 2.8f) return false;
  P.levels.push_back(base0);
  bool capped = false;
  CoreFit core;
  if (P.top - base0 > ground_storey + 2.7f) core = find_core(P.inner, P.inner_holes, P.holes, P.doors, centroid, axis);
  if (core.ok) {
    for (f32 z = std::fmax(P.base + ground_storey, base0 + 2.7f); z + 2.7f <= P.top - 0.05f; z += P.floor_h) {
      if (P.levels.size() >= kMaxLevels) { capped = true; break; }
      P.levels.push_back(z);
    }
  }
  P.has_core = core.ok && P.levels.size() > 1;
  P.core = core.core;
  const u32 F = static_cast<u32>(P.levels.size());
  P.roof_access = P.has_core && !capped && P.top - P.levels[F - 1] <= P.floor_h + 1.8f && P.area < 12000;
  const f32 ceil_top = capped ? P.levels[F - 1] + P.floor_h - kSlab : F == 1 && P.top - base0 > ground_storey + 0.5f ? P.base + ground_storey - kSlab : P.top - 0.3f;
  auto ceil_of = [&](u32 k) { return k + 1 < F ? P.levels[k + 1] - kSlab : ceil_top; };
  const Core& C = P.core;
  auto CP = [&](f32 u, f32 v) { return C.o + C.u * u + C.v * v; };
  auto rect = [&](f32 u0, f32 v0, f32 u1, f32 v1) {
    std::vector<Vec2> r = {CP(u0, v0), CP(u1, v0), CP(u1, v1), CP(u0, v1)};
    if (signed_area(r) < 0) std::reverse(r.begin(), r.end());
    return r;
  };
  auto cw = [](std::vector<Vec2> r) { if (signed_area(r) > 0) std::reverse(r.begin(), r.end()); return r; };
  const f32 wall_top = P.roof_access ? P.top + kRoofHouse : ceil_top;
  // ---- walls: the facade line and the inner face, each with the door openings
  for (int ring = 0; ring < 2; ++ring) {
    const std::vector<Vec2>& R = ring == 0 ? P.outer : P.inner;
    const f32 inset = ring == 0 ? 0 : kWallIn;
    for (std::size_t i = 0; i < R.size(); ++i) {
      const Vec2 a = R[i], bb = R[(i + 1) % R.size()];
      const Vec2 d = bb - a;
      const f32 len = std::hypot(d.x, d.y);
      if (len < 0.02f) continue;
      const Vec2 u = d * (1 / len);
      f32 g0 = 0, g1 = 0;
      const bool gap = door_gap(P.doors, a, bb, inset, g0, g1);
      std::vector<std::pair<f32, f32>> solid;  // along the edge, below the lintel
      if (gap) { solid.push_back({0, std::fmax(0.0f, g0)}); solid.push_back({std::fmin(len, g1), len}); }
      else solid.push_back({0, len});
      // collision: from the footprint base to the roof
      for (auto [s0, s1] : solid) wall(P, a + u * s0, a + u * s1, P.base - 0.5f, P.top, kWall, false, true);
      if (gap) wall(P, a + u * g0, a + u * g1, base0 + kDoorH, P.top, kWall, false, true);
      if (ring == 0) continue;
      // inside: per storey, window openings exactly where the facade draws its windows (Kern's
      // facade: a column every 3 m from the ring's edge start, the pane over 22-78 % of it and 28-78 %
      // of the storey, from the first storey up to 1 m under the roof) - they show the city
      const f32 len_o = std::hypot(P.outer[(i + 1) % P.outer.size()].x - P.outer[i].x, P.outer[(i + 1) % P.outer.size()].y - P.outer[i].y);
      const f32 to_inner = len / std::fmax(0.01f, len_o);
      for (u32 k = 0; k < F; ++k) {
        const f32 z0 = P.levels[k], z1 = ceil_of(k), zn = k + 1 < F ? P.levels[k + 1] : z1 + 0.3f;
        const f32 row = std::floor((z0 + 0.5f - P.base) / P.floor_h);
        const f32 sill = P.base + (row + 0.28f) * P.floor_h, head = std::fmin(z1 - 0.05f, P.base + (row + 0.78f) * P.floor_h);
        std::vector<std::pair<f32, f32>> wins;
        const bool rows = row >= 1 && P.base + (row + 0.78f) * P.floor_h <= P.top - 1 && head > sill + 0.3f;
        for (f32 c0 = 0; rows && c0 + 2.34f <= len_o; c0 += 3) {
          f32 w0 = c0 + 0.66f, w1 = c0 + 2.34f;
          if (reversed) { const f32 t0 = len_o - w1; w1 = len_o - w0; w0 = t0; }
          wins.push_back({w0 * to_inner, w1 * to_inner});
        }
        std::stable_sort(wins.begin(), wins.end(), [](const std::pair<f32, f32>& a, const std::pair<f32, f32>& b) { return a.first < b.first || (!(b.first < a.first) && a.second < b.second); });
        // the full band below the sill and above the head, solid pieces between the windows
        f32 t = 0;
        for (auto [w0, w1] : wins) {
          wall(P, a + u * t, a + u * w0, z0, z1, kWall, true, false);
          wall(P, a + u * w0, a + u * w1, z0, sill, kWall, true, false);
          wall(P, a + u * w0, a + u * w1, head, z1, kWall, true, false);
          // the pane (building_interiors.mjs glass: clear, slightly tinted, one transparent mesh),
          // halfway between the inner face and the facade
          const Vec2 g0 = a + u * w0 + Vec2{u.y, -u.x} * (kWallIn * 0.5f), g1 = a + u * w1 + Vec2{u.y, -u.x} * (kWallIn * 0.5f);
          const Vec3 q[4] = {{g0.x, g0.y, sill}, {g1.x, g1.y, sill}, {g1.x, g1.y, head}, {g0.x, g0.y, head}};
          const Vec3 gn = kern::normalize(kern::cross(q[1] - q[0], q[2] - q[0]), {1, 0, 0});
          for (int v : {0, 1, 2, 0, 2, 3}) P.glass.push(q[v], gn, {0.62f, 0.70f, 0.74f}, {0, 0});
          t = w1;
        }
        if (k == 0 && gap) {
          // the door opening in the lobby wall
          if (t < g0) wall(P, a + u * t, a + u * g0, z0, z1, kWall, true, false);
          wall(P, a + u * g0, a + u * g1, z0 + kDoorH, z1, kWall, true, false);
          t = std::fmax(t, g1);
        }
        wall(P, a + u * t, a + u * len, z0, z1, kWall, true, false);
        wall(P, a, bb, z1, zn, kWall, true, false);  // slab edge band
        wall(P, a, bb, z0, z0 + 0.1f, kTrim, true, false);  // skirting
      }
    }
  }
  // courtyards: walls without doors
  for (std::size_t h = 0; h < P.holes.size(); ++h)
    for (int ring = 0; ring < 2; ++ring) {
      const std::vector<Vec2>& R = ring == 0 ? P.holes[h] : P.inner_holes[h];
      for (std::size_t i = 0; i < R.size(); ++i) wall(P, R[i], R[(i + 1) % R.size()], ring ? base0 : P.base - 0.5f, ring ? ceil_top : P.top, kWall, ring == 1, true);
    }
  // door reveals (walls of the opening between the two rings), lintel, threshold
  for (const Door& d : P.doors) {
    const Vec2 o0 = d.c - d.d * (kDoorW / 2), o1 = d.c + d.d * (kDoorW / 2), i0 = o0 - d.n * kWallIn, i1 = o1 - d.n * kWallIn;
    wall(P, o0, i0, base0, base0 + kDoorH, kStone, true, true);
    wall(P, i1, o1, base0, base0 + kDoorH, kStone, true, true);
    vquad(P, {o0.x, o0.y, base0 + kDoorH}, {o1.x, o1.y, base0 + kDoorH}, {i1.x, i1.y, base0 + kDoorH}, {i0.x, i0.y, base0 + kDoorH}, kStone);
    const Vec2 f0 = o0 + d.n * 0.6f, f1 = o1 + d.n * 0.6f;
    vquad(P, {f0.x, f0.y, base0 - 0.01f}, {f1.x, f1.y, base0 - 0.01f}, {i1.x, i1.y, base0}, {i0.x, i0.y, base0}, kStone);
    cquad(P, {f0.x, f0.y, base0}, {f1.x, f1.y, base0}, {i1.x, i1.y, base0}, {i0.x, i0.y, base0}, false);
  }
  // ---- floors, ceilings, the roof
  std::vector<std::vector<Vec2>> floor_holes;
  for (const auto& h : P.inner_holes) floor_holes.push_back(cw(h));
  std::vector<std::vector<Vec2>> core_holes = floor_holes;
  if (P.has_core) core_holes.push_back(cw(rect(0, 0, kCoreW, kCoreL)));
  for (u32 k = 0; k < F; ++k) {
    const f32 z = P.levels[k];
    slab(P, P.inner, k == 0 ? floor_holes : core_holes, z, k == 0 ? kStone : kOak, true, true, true);
    if (k > 0) {
      slab(P, P.inner, core_holes, z - kSlab, kCeil, false, true, false);  // the ceiling of the storey below
      // the near landing in the core at this floor
      slab(P, rect(0, 0, kCoreW, kLand), {}, z, kConcrete, true, true, true);
      slab(P, rect(0, 0, kCoreW, kLand), {}, z - kSlab, kCeil, false, true, false);
    }
  }
  std::vector<std::vector<Vec2>> roof_holes;
  for (const auto& h : P.holes) roof_holes.push_back(h);
  if (P.roof_access) roof_holes.push_back(cw(rect(0, kLand, kCoreW, kCoreL)));
  slab(P, P.inner, P.roof_access ? std::vector<std::vector<Vec2>>{floor_holes.begin(), floor_holes.end()} : floor_holes, ceil_top, kCeil, false, true, false);
  if (P.roof_access) slab(P, rect(0, 0, kCoreW, kLand), {}, P.top, kConcrete, true, true, false);
  slab(P, P.outer, roof_holes, P.top, kConcrete, true, false, true);
  // ---- the stair core: walls, divider, flights, landings
  if (P.has_core) {
    const f32 z0 = base0;
    auto cwall = [&](f32 u0, f32 v0, f32 u1, f32 v1, f32 za, f32 zb, Vec3 col) { wall(P, CP(u0, v0), CP(u1, v1), za, zb, col, true, true); };
    cwall(0, 0, 0, kCoreL, z0, wall_top, kStairWall);
    cwall(kCoreW, 0, kCoreW, kCoreL, z0, wall_top, kStairWall);
    cwall(0, kCoreL, kCoreW, kCoreL, z0, wall_top, kStairWall);
    cwall(kLane + 0.1f, kLand, kLane + 0.1f, kLand + kRun, z0, wall_top - (P.roof_access ? kRoofHouse : 0), kAccent);
    std::vector<std::pair<f32, f32>> stairs;
    for (u32 k = 0; k + 1 < F; ++k) stairs.push_back({P.levels[k], P.levels[k + 1] - P.levels[k]});
    if (P.roof_access) stairs.push_back({P.levels[F - 1], P.top - P.levels[F - 1]});
    for (auto [z, R] : stairs) {
      const f32 half = R / 2;
      auto P3 = [&](f32 u, f32 v, f32 h) { const Vec2 q = CP(u, v); return Vec3{q.x, q.y, h}; };
      // ramps for walking (collision), steps for the eye
      cquad(P, P3(0, kLand, z), P3(kLane, kLand, z), P3(kLane, kLand + kRun, z + half), P3(0, kLand + kRun, z + half), true);
      cquad(P, P3(kCoreW - kLane, kLand + kRun, z + half), P3(kCoreW, kLand + kRun, z + half), P3(kCoreW, kLand, z + R), P3(kCoreW - kLane, kLand, z + R), true);
      slab(P, rect(0, kLand + kRun, kCoreW, kCoreL), {}, z + half, kConcrete, true, true, true);
      slab(P, rect(0, kLand + kRun, kCoreW, kCoreL), {}, z + half - 0.2f, kCeil, false, true, false);
      const u32 n = std::max<u32>(6, static_cast<u32>(std::lround(half / 0.18f)));
      const f32 rise = half / static_cast<f32>(n), tread = kRun / static_cast<f32>(n);
      const f32 yaw = std::atan2(C.v.y, C.v.x);
      for (u32 s = 0; s < n; ++s) {
        const f32 top_a = z + rise * static_cast<f32>(s + 1), top_b = z + half + rise * static_cast<f32>(s + 1);
        const Vec2 ca = CP(kLane / 2, kLand + tread * (static_cast<f32>(s) + 0.5f)), cb = CP(kCoreW - kLane / 2, kLand + kRun - tread * (static_cast<f32>(s) + 0.5f));
        box(P, {ca.x, ca.y, top_a - rise * 0.5f}, {tread, kLane, rise}, yaw, s % 2 ? kRiser : kConcrete, false);
        box(P, {cb.x, cb.y, top_b - rise * 0.5f}, {tread, kLane, rise}, yaw, s % 2 ? kRiser : kConcrete, false);
      }
    }
    if (!P.roof_access) cwall(0, kLand, kLane, kLand, P.levels[F - 1], P.levels[F - 1] + 1.1f, kDark);  // the railing at the top
    else {
      // the stair house on the roof: a lid over the core
      slab(P, rect(0, 0, kCoreW, kCoreL), {}, P.top + kRoofHouse, kConcrete, false, true, true);
      slab(P, rect(0, 0, kCoreW, kCoreL), {}, P.top + kRoofHouse + 0.01f, kConcrete, true, true, true);
    }
  }
  // ---- furniture and ceiling lights (building_interiors.mjs placeProps: desks, sofas, shelves,
  // beds, tables, plants; panels on a 3.6 m grid)
  f32 x0 = 1e9f, y0 = 1e9f, x1 = -1e9f, y1 = -1e9f;
  for (Vec2 p : P.inner) { x0 = std::fmin(x0, p.x); x1 = std::fmax(x1, p.x); y0 = std::fmin(y0, p.y); y1 = std::fmax(y1, p.y); }
  auto free_spot = [&](Vec2 p, f32 clear) {
    if (!pip(p, P.inner) || ring_dist(p, P.inner) < clear) return false;
    for (const auto& h : P.inner_holes)
      if (pip(p, h) || ring_dist(p, h) < clear) return false;
    for (const Door& d : P.doors)
      if (std::hypot(d.c.x - p.x, d.c.y - p.y) < 3.2f) return false;
    if (P.has_core) {
      const Vec2 q = p - C.o;
      const f32 u = q.x * C.u.x + q.y * C.u.y, v = q.x * C.v.x + q.y * C.v.y;
      if (u > -1.5f && u < kCoreW + 1.5f && v > -3.0f && v < kCoreL + 1.5f) return false;
    }
    return true;
  };
  const f32 yaw0 = std::atan2(axis.y, axis.x);
  for (u32 k = 0; k < F; ++k) {
    const f32 z = P.levels[k], zc = ceil_of(k);
    u32 placed = 0, lamps = 0;
    for (f32 x = x0 + 1.8f; x < x1 && placed < 10; x += 3.4f)
      for (f32 y = y0 + 1.8f; y < y1 && placed < 10; y += 3.4f) {
        const Vec2 p{x, y};
        const u32 h = hash_key(seed + k * 131, static_cast<u32>(static_cast<i32>(x * 7) * 31 + static_cast<i32>(y * 13)));
        if (h % 100 > 38 || !free_spot(p, 1.3f)) continue;
        const f32 yaw = yaw0 + (h >> 8 & 3) * kPi / 2;
        // the lobby gets a reception desk first (building_interiors.mjs reception), then lounge pieces
        const u32 kind = k == 0 ? (placed == 0 ? 9u : 3u + (h >> 12) % 3 + ((h >> 14) & 1) * 3u) : (h >> 12) % 9;
        switch (kind) {
          case 0:  // desk with a screen and a chair
            box(P, {p.x, p.y, z + 0.37f}, {1.6f, 0.8f, 0.74f}, yaw, kOak, true);
            box(P, Vec3{p.x, p.y, z + 1.0f} + yaw_quat(yaw).rotate({0, 0.22f, 0}), {0.62f, 0.035f, 0.38f}, yaw, rgb(0x111214), false);
            box(P, Vec3{p.x, p.y, z + 0.25f} + yaw_quat(yaw).rotate({0, -0.7f, 0}), {0.5f, 0.5f, 0.5f}, yaw, rgb(0x3a3f47), false);
            break;
          case 1:  // bed
            box(P, {p.x, p.y, z + 0.27f}, {1.72f, 2.18f, 0.54f}, yaw, rgb(0xeeece6), true);
            box(P, Vec3{p.x, p.y, z + 0.55f} + yaw_quat(yaw).rotate({0, 1.05f, 0}), {1.74f, 0.08f, 1.1f}, yaw, rgb(0x5e3f2a), false);
            break;
          case 2:  // shelf
            box(P, {p.x, p.y, z + 0.95f}, {1.2f, 0.36f, 1.9f}, yaw, rgb(0xeeece6), true);
            break;
          case 3:  // sofa
            box(P, {p.x, p.y, z + 0.3f}, {2.1f, 0.92f, 0.6f}, yaw, rgb(0x4f6d7a + (h & 0x0f0f0f)), true);
            box(P, Vec3{p.x, p.y, z + 0.66f} + yaw_quat(yaw).rotate({0, 0.36f, 0}), {2.1f, 0.2f, 0.5f}, yaw, rgb(0x4f6d7a + (h & 0x0f0f0f)), false);
            break;
          case 4:  // table
            box(P, {p.x, p.y, z + 0.37f}, {0.9f, 0.9f, 0.74f}, yaw, rgb(0xefe9df), true);
            break;
          case 6: {  // armchair and a floor lamp
            const Quat q = yaw_quat(yaw);
            box(P, {p.x, p.y, z + 0.22f}, {0.86f, 0.84f, 0.44f}, yaw, rgb(0x8a4f3c + (h & 0x070707)), true);
            box(P, Vec3{p.x, p.y, z + 0.66f} + q.rotate({0, 0.34f, 0}), {0.86f, 0.18f, 0.56f}, yaw, rgb(0x8a4f3c + (h & 0x070707)), false);
            box(P, Vec3{p.x, p.y, z + 0.75f} + q.rotate({0.75f, 0.2f, 0}), {0.04f, 0.04f, 1.45f}, yaw, kDark, false);
            box(P, Vec3{p.x, p.y, z + 1.55f} + q.rotate({0.75f, 0.2f, 0}), {0.36f, 0.36f, 0.26f}, yaw, rgb(0xf4e4c2), false);
            break;
          }
          case 7: {  // dining table with four chairs
            const Quat q = yaw_quat(yaw);
            box(P, {p.x, p.y, z + 0.38f}, {1.8f, 0.9f, 0.76f}, yaw, rgb(0x5e3f2a), true);
            for (f32 cx : {-0.45f, 0.45f})
              for (f32 cy : {-0.68f, 0.68f}) box(P, Vec3{p.x, p.y, z + 0.45f} + q.rotate({cx, cy, 0}), {0.44f, 0.42f, 0.9f}, yaw, rgb(0xb98b5c), false);
            break;
          }
          case 8: {  // kitchen counter
            box(P, {p.x, p.y, z + 0.45f}, {2.4f, 0.6f, 0.9f}, yaw, rgb(0xeeece6), true);
            box(P, {p.x, p.y, z + 0.915f}, {2.44f, 0.64f, 0.04f}, yaw, rgb(0x3d3f42), false);
            break;
          }
          case 9: {  // reception desk with a brass band
            const Quat q = yaw_quat(yaw);
            box(P, {p.x, p.y, z + 0.55f}, {2.8f, 0.75f, 1.1f}, yaw, rgb(0x5e3f2a), true);
            box(P, {p.x, p.y, z + 1.12f}, {2.9f, 0.85f, 0.05f}, yaw, rgb(0xd9d6cf), false);
            box(P, Vec3{p.x, p.y, z + 0.55f} + q.rotate({0, -0.39f, 0}), {2.6f, 0.01f, 0.5f}, yaw, rgb(0xc7a35a), false);
            break;
          }
          default:  // plant
            box(P, {p.x, p.y, z + 0.21f}, {0.4f, 0.4f, 0.42f}, yaw, rgb(0xd9d4ca), true);
            box(P, {p.x, p.y, z + 0.9f}, {0.6f, 0.6f, 0.9f}, yaw + 0.4f, rgb(0x3d6b33), false);
            break;
        }
        ++placed;
        ++P.furniture;
      }
    for (f32 x = x0 + 1.8f; x < x1 && lamps < 24; x += 3.6f)
      for (f32 y = y0 + 1.8f; y < y1 && lamps < 24; y += 3.6f)
        if (pip({x, y}, P.inner) && ring_dist({x, y}, P.inner) > 0.8f) { P.lamps.push_back({x, y, zc - 0.02f}); ++lamps; }
  }
  // air conditioners on the roof (building_interiors.mjs ac), clear of the stair house
  if (P.roof_access) {
    u32 ac = 0;
    for (f32 x = x0 + 2; x < x1 - 1 && ac < 3; x += 5)
      for (f32 y = y0 + 2; y < y1 - 1 && ac < 3; y += 5) {
        const Vec2 q{x, y};
        if (!pip(q, P.inner) || ring_dist(q, P.inner) < 1.5f) continue;
        const Vec2 c = q - C.o;
        const f32 cu = c.x * C.u.x + c.y * C.u.y, cv = c.x * C.v.x + c.y * C.v.y;
        if (cu > -2.5f && cu < kCoreW + 2.5f && cv > -3.5f && cv < kCoreL + 2.5f) continue;
        if (h01(seed, 900 + ac * 7 + static_cast<u32>(x * 3 + y)) > 0.5f) continue;
        box(P, {q.x, q.y, P.top + 0.55f}, {1.5f, 1.1f, 1.1f}, yaw0, rgb(0xa9adb1), true);
        box(P, {q.x, q.y, P.top + 1.12f}, {0.8f, 0.8f, 0.05f}, yaw0, rgb(0x2c2f33), false);
        ++ac;
      }
  }
  return !P.points.empty();
}

// ---------------------------------------------------------------------------------------------
// the active building in the game

void Interiors::clear() {
  if (r_ && mesh_id_) { r_->remove(mesh_id_); mesh_id_ = 0; }
  if (r_ && glass_id_) { r_->remove(glass_id_); glass_id_ = 0; }
  if (world_ && body_.valid() && world_->body_valid(body_)) world_->destroy_body(body_);
  body_ = {};
  active_ = false;
  closed_.clear();
}

void Interiors::release(CityStream& city) {
  clear();
  city.exclude_building(0, -1);
}

void Interiors::activate(const CityStream::BuildingRef& ref, CityStream& city, f32 ground) {
  Plan plan;
  if (!build(city.building(ref), ground, plan)) return;
  clear();
  plan_ = std::move(plan);
  ph::BodySettings bs;
  bs.type = ph::BodyType::fixed;
  body_ = world_->create_body(bs);
  ph::ShapeSettings ss;
  ss.kind = ph::ShapeKind::mesh;
  ss.points = kern::Span<const Vec3>(plan_.points.data(), plan_.points.size());
  ss.indices = kern::Span<const u32>(plan_.indices.data(), plan_.indices.size());
  ss.material.friction = 0.7f;
  ss.material.category_bits = kCatWorld;
  if (!world_->create_shape(body_, ss).valid()) { clear(); return; }
  kern::weld(plan_.mesh);
  mesh_id_ = r_->allocate_id();
  kern::MeshLook look;
  look.material.roughness = 0.85f;
  look.material.double_sided = true;
  look.material.cast_shadow = false;
  look.extra_slot = kern::kMaxExtensionAttributes;
  if (!kern::upsert(*r_, mesh_id_, plan_.mesh, look)) { r_->release_id(mesh_id_); mesh_id_ = 0; }
  plan_.mesh = kern::MeshData{};
  if (plan_.glass.vertex_count()) {
    glass_id_ = r_->allocate_id();
    kern::MeshLook glass;
    glass.material.roughness = 0.05f;
    glass.material.color = {1, 1, 1, 0.22f};
    glass.material.alpha = kern::AlphaMode::blend;
    glass.material.double_sided = true;
    glass.material.cast_shadow = false;
    glass.material.emissive = {0.02f, 0.025f, 0.03f};
    glass.extra_slot = kern::kMaxExtensionAttributes;
    if (!kern::upsert(*r_, glass_id_, plan_.glass, glass)) { r_->release_id(glass_id_); glass_id_ = 0; }
    plan_.glass = kern::MeshData{};
  }
  active_ = true;
  tile_ = ref.tile;
  index_ = ref.index;
  door_angle_.assign(plan_.doors.size(), 0);
  city.exclude_building(tile_, static_cast<i32>(index_));
  ++built_;
}

bool Interiors::inside(Vec3 p) const {
  if (!active_ || !pip({p.x, p.y}, plan_.outer)) return false;
  for (const auto& h : plan_.holes)
    if (pip({p.x, p.y}, h)) return false;
  return p.z > plan_.base - 1 && p.z < plan_.top + kRoofHouse + 1;
}

i32 Interiors::level_at(Vec3 p) const {
  if (!inside(p)) return -1;
  i32 k = 0;
  for (std::size_t i = 0; i < plan_.levels.size(); ++i)
    if (p.z >= plan_.levels[i] - 0.3f) k = static_cast<i32>(i);
  if (plan_.roof_access && p.z >= plan_.top - 0.3f) k = static_cast<i32>(plan_.levels.size());
  return k;
}

void Interiors::update(f32 dt, f32 now, Vec3 feet, bool on_foot, CityStream& city) {
  feet_ = feet;
  const Vec2 f{feet.x, feet.y};
  if (active_) {
    for (std::size_t i = 0; i < plan_.doors.size(); ++i) {
      const Door& d = plan_.doors[i];
      const bool near = std::hypot(f.x - d.c.x, f.y - d.c.y) < 2.6f && std::fabs(feet.z - plan_.levels[0]) < 2;
      door_angle_[i] += clampf((near ? 1.45f : 0.0f) - door_angle_[i], -dt * 3.2f, dt * 3.2f);
    }
    const bool in = inside(feet);
    if (in && !was_inside_) ++entered_;
    was_inside_ = in;
  }
  if (now < next_scan_) return;
  next_scan_ = now + 0.5f;
  std::vector<CityStream::BuildingRef> near;
  city.buildings_near(f, kDoorsRadius, near);
  // which building is the player at? (inside it, or the nearest outline within ACTIVATE)
  i32 best = -1;  // (its place in `near`)
  f32 best_d = kActivate;
  for (std::size_t i = 0; i < near.size(); ++i) {
    const std::vector<Vec2>& o = city.building(near[i]).rings[0];
    const f32 d = pip(f, o) ? 0 : ring_dist(f, o);
    if (d < best_d) { best_d = d; best = static_cast<i32>(i); }
  }
  if (active_) {
    const f32 d = pip(f, plan_.outer) ? 0 : ring_dist(f, plan_.outer);
    const bool keep = inside(feet) || d < kRelease;
    const bool other = best >= 0 && !(near[static_cast<std::size_t>(best)].tile == tile_ && near[static_cast<std::size_t>(best)].index == index_) && best_d < 0.5f * d && !inside(feet);
    if (!keep || other || (!on_foot && d > kActivate)) release(city);
  }
  if (!active_ && on_foot && best >= 0) {
    const f32 ground = city.ground_height(f.x, f.y);
    activate(near[static_cast<std::size_t>(best)], city, ground);
  }
  // closed doors of the buildings around (the nearest MAX_CLOSED_DOORS)
  std::vector<std::pair<f32, Closed>> doors;
  for (const CityStream::BuildingRef& r : near) {
    if (active_ && r.tile == tile_ && r.index == index_) continue;
    const MapFeatures::Building& b = city.building(r);
    std::vector<Vec2> o = clean(b.rings[0]);
    if (o.size() < 3) continue;
    if (signed_area(o) < 0) std::reverse(o.begin(), o.end());
    const f32 area = signed_area(o);
    if (area < 24 || area > 40000 || b.top - b.base < 2.8f) continue;
    for (const Door& d : doors_of(o, area)) {
      const f32 dist = std::hypot(d.c.x - f.x, d.c.y - f.y);
      if (dist > kDoorsRadius) continue;
      doors.push_back({dist, {{d.c.x, d.c.y, std::fmax(b.base, city.ground_height(d.c.x + d.n.x, d.c.y + d.n.y))}, std::atan2(d.d.y, d.d.x)}});
    }
  }
  std::stable_sort(doors.begin(), doors.end(), [](const std::pair<f32, Closed>& a, const std::pair<f32, Closed>& b) { return a.first < b.first; });
  closed_.clear();
  for (std::size_t i = 0; i < doors.size() && i < kMaxClosedDoors; ++i) closed_.push_back(doors[i].second);
}

void Interiors::draw(LifeArt& art, f32 now, Vec3 eye) const {
  (void)now;
  // door unit (building_interiors.mjs doorGeometries): dark frame, canopy, a light, two leaves
  auto unit = [&](Vec3 at, f32 yaw, f32 angle, bool open_dark) {
    const Mat4 M = Mat4::trs(at, yaw_quat(yaw), {1, 1, 1});  // x along the wall, +y outward... (wall dir)
    // local: x along the wall, y outward (the door's normal is right of the wall direction: -y here)
    auto bx = [&](Vec3 c, Vec3 s, Vec3 t) { art.box(M * Mat4::translate(c) * Mat4::scale(s), t); };
    const f32 W = kDoorW, H = kDoorH;
    bx({-W / 2 - 0.05f, -0.04f, H / 2}, {0.1f, 0.14f, H}, rgb(0x1d1f22));
    bx({W / 2 + 0.05f, -0.04f, H / 2}, {0.1f, 0.14f, H}, rgb(0x1d1f22));
    bx({0, -0.04f, H + 0.06f}, {W + 0.2f, 0.14f, 0.12f}, rgb(0x1d1f22));
    bx({0, -0.6f, H + 0.42f}, {W + 0.9f, 1.2f, 0.08f}, rgb(0x2a2c30));
    art.glow(M * Mat4::translate({0, -0.75f, H + 0.36f}) * Mat4::scale({0.3f, 0.08f, 0.04f}), {1.0f, 0.95f, 0.8f});
    if (open_dark) bx({0, -0.015f, H / 2}, {W, 0.01f, H}, rgb(0x0b0c0e));  // the dim lobby behind the opening
    for (f32 s : {-1.0f, 1.0f}) {
      // leaves hinge at the frame and swing outward
      const Mat4 L = M * Mat4::translate({s * W / 2, -0.03f, 0}) * Mat4::trs({}, Quat::axis_angle({0, 0, 1}, s * angle), {1, 1, 1});
      art.box(L * Mat4::translate({-s * W / 4, 0, H / 2}) * Mat4::scale({W / 2 - 0.04f, 0.05f, H - 0.04f}), rgb(0x1e2a33));
    }
  };
  for (const Closed& c : closed_) {
    if (kern::length(c.at - eye) > kDoorsRadius + 20) continue;
    unit(c.at, c.yaw, 0, false);
  }
  if (!active_) return;
  for (std::size_t i = 0; i < plan_.doors.size(); ++i) {
    const Door& d = plan_.doors[i];
    unit({d.c.x, d.c.y, plan_.levels[0]}, std::atan2(d.d.y, d.d.x), door_angle_[i], door_angle_[i] > 0.05f && !inside(eye));
  }
  // ceiling lights of the storeys around the player
  for (const Vec3& l : plan_.lamps)
    if (std::fabs(l.z - feet_.z) < 7 && kern::length(l - eye) < 40) art.glow(Mat4::translate(l) * Mat4::scale({1.4f, 0.6f, 0.03f}), {1.0f, 0.95f, 0.85f});
}

void Interiors::hud(std::string& s) const {
  char b[256];
  std::snprintf(b, sizeof b, ",\"interior\":{\"active\":%s,\"levels\":%u,\"roof\":%s,\"doors\":%u,\"closedDoors\":%u,\"inside\":%s,\"level\":%d,\"entered\":%u,\"built\":%u,\"furniture\":%u}",
                active_ ? "true" : "false", active_ ? static_cast<u32>(plan_.levels.size()) : 0, active_ && plan_.roof_access ? "true" : "false",
                active_ ? static_cast<u32>(plan_.doors.size()) : 0, static_cast<u32>(closed_.size()), inside(feet_) ? "true" : "false", level_at(feet_), entered_, built_,
                active_ ? plan_.furniture : 0);
  s += b;
}

bool Interiors::command(const std::string& cmd, Vec3 me, std::string& out, std::optional<Vec3>& go) {
  char b[256];
  if (cmd == "interior") {
    std::snprintf(b, sizeof b, "{\"active\":%s,\"inside\":%s,\"level\":%d,\"levels\":%u,\"roof\":%s,\"z\":%.2f,\"floor0\":%.2f,\"top\":%.2f}", active_ ? "true" : "false",
                  inside(me) ? "true" : "false", level_at(me), active_ ? static_cast<u32>(plan_.levels.size()) : 0, active_ && plan_.roof_access ? "true" : "false", me.z,
                  active_ ? plan_.levels[0] : 0.0f, active_ ? plan_.top : 0.0f);
    out = b;
    return true;
  }
  if (!active_) { out = "no building"; return true; }
  const Door& d = plan_.doors[0];
  Vec3 to;
  if (cmd == "interior door") to = {d.c.x + d.n.x * 1.5f, d.c.y + d.n.y * 1.5f, plan_.levels[0]};
  else if (cmd == "interior in") to = {d.c.x - d.n.x * 2.0f, d.c.y - d.n.y * 2.0f, plan_.levels[0] + 0.05f};
  else if (cmd == "interior up" || cmd == "interior roof") {
    if (!plan_.has_core) { out = "no stairs"; return true; }
    const Core& c = plan_.core;
    const bool roof = cmd == "interior roof";
    if (roof && !plan_.roof_access) { out = "no roof access"; return true; }
    const Vec2 q = c.o + c.u * (kCoreW / 2) + c.v * (kLand / 2);
    to = {q.x, q.y, (roof ? plan_.top : plan_.levels[1]) + 0.05f};
  } else { out = "?"; return true; }
  go = to;
  return true;
}

}  // namespace sc
