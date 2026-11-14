// Buildings broken by weapons: destruction.h.
#include "destruction.h"

#include <algorithm>
#include <cmath>

#include "kern/geo/earcut.h"

namespace sc {

namespace {
constexpr f32 kGravity = 9.81f;
constexpr f32 kPanelMax = 7;   // facade panels at most 7 m wide
constexpr u32 kMaxPieces = 40;  // per broken building
// the pieces' colours: concrete, a few warm embers, a few dim ones
const Vec3 kConcrete{0.62f, 0.58f, 0.52f}, kEmber = kern::hex_color(0xff8a3d), kDim = kern::hex_color(0x6b5a4a);

Vec2 norm2(Vec2 v, Vec2 fallback) {
  const f32 l = std::hypot(v.x, v.y);
  return l > 1e-3f ? Vec2{v.x / l, v.y / l} : fallback;
}
}  // namespace

bool Destruction::merge_points(const std::vector<std::pair<u32, f32>>& list, bool log) {
  bool changed = false;
  for (const auto& [seed, hp] : list) {
    if (!std::isfinite(hp) || hp < 0) continue;
    auto [it, fresh] = points_.try_emplace(seed, hp);
    if (!fresh) { if (hp >= it->second - 0.01f) continue; it->second = hp; }
    changed = true;
    if (log) points_log_.emplace_back(seed, hp);
  }
  return changed;
}

u32 Destruction::blast(Vec3 at, f32 radius, f32 damage, bool missile, CityStream& city, Rng& rng) {
  const f32 r = std::clamp(radius, 2.0f, 40.0f);
  const f32 power = std::clamp(damage, 10.0f, 400.0f) * (missile ? 2.2f : 1.0f);
  std::vector<CityStream::BuildingRef> near;
  city.buildings_near({at.x, at.y}, r, near);
  u32 touched = 0;
  for (const CityStream::BuildingRef& ref : near) {
    const MapFeatures::Building& b = city.building(ref);
    if (b.rings.empty() || b.rings[0].size() < 3 || b.leveled) continue;
    Vec2 lo{1e30f, 1e30f}, hi{-1e30f, -1e30f};
    for (Vec2 p : b.rings[0]) { lo = {std::fmin(lo.x, p.x), std::fmin(lo.y, p.y)}; hi = {std::fmax(hi.x, p.x), std::fmax(hi.y, p.y)}; }
    const f32 dx = std::fmax(std::fmax(lo.x - at.x, 0.0f), at.x - hi.x), dy = std::fmax(std::fmax(lo.y - at.y, 0.0f), at.y - hi.y);
    const f32 d = std::sqrt(dx * dx + dy * dy + std::pow(std::fmax(0.0f, at.z - b.top), 2.0f));
    if (d > r) continue;
    // the points belong to the whole building (its undamaged volume)
    const f32 height = std::fmax(1.0f, b.full_top - b.base);
    const f32 full = std::fmax(220.0f, (hi.x - lo.x) * (hi.y - lo.y) * height * 0.22f);
    auto [it, fresh] = points_.try_emplace(b.seed, full);
    (void)fresh;
    const f32 hp = std::fmax(0.0f, it->second - power * std::pow(1 - d / r, 1.5f) * 2.6f);
    it->second = hp;
    points_log_.emplace_back(b.seed, hp);
    const f32 keep = hp / full;
    const f32 target = hp <= 0 ? b.base + std::fmin(kRubble, height) : b.base + std::fmax(kRubble, std::ceil(height * keep / kFloor) * kFloor);
    if (target >= b.top - 0.4f) continue;
    Broken br;
    br.rings = b.rings;
    br.lo = lo;
    br.hi = hi;
    br.top = b.top;
    br.new_top = target;
    br.intensity = 0.35f + 0.6f * (1 - d / r);
    Vec2 c{};
    u32 n = 0;
    for (const auto& ring : b.rings) for (Vec2 p : ring) { c = c + p; ++n; }
    br.c = n ? c * (1.0f / static_cast<f32>(n)) : Vec2{at.x, at.y};
    if (!city.damage_building(ref.tile, ref.index, target, hp <= 0)) continue;
    ++touched;
    stats_.collapsed += hp <= 0;
    collapse(br, {at.x, at.y, 0}, city, rng);
  }
  stats_.hits += touched;
  return touched;
}

void Destruction::collapse(const Broken& b, Vec3 center, CityStream& city, Rng& rng) {
  (void)city;
  const f32 removed = b.top - b.new_top;
  if (removed < 0.5f) return;
  // a big part comes off as one piece first (it covers the frames until the chunk is swapped)
  if (removed > 2 && b.intensity < 2.4f && torn_.size() < kTorn && tear(b, center, rng)) return;
  fracture(b, center, {}, 1, rng);
}

bool Destruction::tear(const Broken& b, Vec3 center, Rng& rng) {
  if (!r_) return false;
  // the largest ring is the piece's outline
  const std::vector<Vec2>* ring = nullptr;
  f32 best = -1;
  for (const auto& rr : b.rings) {
    f32 a = 0;
    for (std::size_t i = 0; i < rr.size(); ++i) { const Vec2 p = rr[i], q = rr[(i + 1) % rr.size()]; a += p.x * q.y - q.x * p.y; }
    if (std::fabs(a) > best) { best = std::fabs(a); ring = &rr; }
  }
  if (!ring || ring->size() < 3) return false;
  const Vec2 dir = norm2(b.c - Vec2{center.x, center.y}, {1, 0});
  // it pivots on its leeward edge: the outline's farthest point along the blast direction
  f32 reach = -1e30f;
  for (Vec2 p : *ring) reach = std::fmax(reach, (p.x - b.c.x) * dir.x + (p.y - b.c.y) * dir.y);
  const Vec3 pivot{b.c.x + dir.x * reach, b.c.y + dir.y * reach, b.new_top};
  const f32 h = b.top - b.new_top;
  std::vector<Vec2> pts;
  for (Vec2 p : *ring) pts.push_back({p.x - pivot.x, p.y - pivot.y});
  if (pts.size() > 3 && std::hypot(pts.front().x - pts.back().x, pts.front().y - pts.back().y) < 1e-4f) pts.pop_back();
  if (pts.size() < 3) return false;
  f32 area = 0;
  for (std::size_t i = 0; i < pts.size(); ++i) { const Vec2 p = pts[i], q = pts[(i + 1) % pts.size()]; area += p.x * q.y - q.x * p.y; }
  if (area < 0) std::reverse(pts.begin(), pts.end());  // counter-clockwise from above
  std::vector<f64> xy;
  for (Vec2 p : pts) xy.insert(xy.end(), {p.x, p.y});
  std::vector<u32> tris;
  kern::Earcut earcut;
  earcut.triangulate(xy.data(), static_cast<u32>(pts.size()), nullptr, 0, tris);
  if (tris.empty()) return false;
  const Vec3 col = kern::hex_color(0xcdb99a);
  kern::MeshData m;
  for (std::size_t i = 0; i + 2 < tris.size(); i += 3) {
    const Vec2 a = pts[tris[i]], c = pts[tris[i + 1]], d = pts[tris[i + 2]];
    for (Vec2 q : {a, c, d}) m.push({q.x, q.y, h}, {0, 0, 1}, col, {q.x * 0.1f, q.y * 0.1f});
    for (Vec2 q : {a, d, c}) m.push({q.x, q.y, 0}, {0, 0, -1}, col, {q.x * 0.1f, q.y * 0.1f});
  }
  for (std::size_t i = 0; i < pts.size(); ++i) {
    const Vec2 a = pts[i], c = pts[(i + 1) % pts.size()];
    const Vec2 e = norm2(c - a, {1, 0});
    const Vec3 nrm{e.y, -e.x, 0};  // outward of a counter-clockwise outline
    for (Vec3 q : {Vec3{a.x, a.y, 0}, Vec3{c.x, c.y, 0}, Vec3{c.x, c.y, h}, Vec3{a.x, a.y, 0}, Vec3{c.x, c.y, h}, Vec3{a.x, a.y, h}})
      m.push(q, nrm, col, {q.x * 0.1f + q.y * 0.1f, q.z * 0.1f});
  }
  Torn t;
  t.mesh = r_->allocate_id();
  kern::MeshInput in = m.as_input();
  in.material.roughness = 0.9f;
  in.material.double_sided = true;
  const kern::Mat4 at = kern::Mat4::translate(pivot);
  in.transforms = kern::Span<const f32>(at.m, 16);
  if (!r_->upsert(t.mesh, in)) { r_->release_id(t.mesh); return false; }
  const f32 I = std::fmin(b.intensity, 2.4f);
  t.b = b;
  t.center = center;
  t.dir = {dir.x, dir.y, 0};
  t.axis = {-dir.y, dir.x, 0};
  t.pivot = pivot;
  t.tilt = std::fmin(1.45f, 0.35f + 0.55f * I + 0.25f * std::fmin(1.0f, h / 30)) * rng.range(0.8f, 1.1f);
  t.speed = (6 + 22 * I) * rng.range(0.75f, 1.15f);
  t.lift = rng.range(1, 4) + 3 * I;
  t.break_at = std::fmax(0.28f, 1.15f - 0.38f * I) * rng.range(0.85f, 1.2f);
  torn_.push_back(std::move(t));
  return true;
}

void Destruction::fracture(const Broken& b, Vec3 center, Vec3 offset, f32 boost, Rng& rng) {
  const f32 removed = b.top - b.new_top;
  if (removed < 0.5f) return;
  const Vec2 outward = norm2(b.c - Vec2{center.x, center.y}, {1, 0});
  const f32 I = std::fmin(b.intensity, 2.5f);
  struct Seg { Vec2 m; f32 len, yaw; };
  std::vector<Seg> segs;
  for (const auto& ring : b.rings)
    for (std::size_t i = 0; i < ring.size(); ++i) {
      const Vec2 a = ring[i], c = ring[(i + 1) % ring.size()];
      const f32 len = std::hypot(c.x - a.x, c.y - a.y);
      if (len < 0.4f) continue;
      const u32 parts = std::max<u32>(1, static_cast<u32>(std::ceil(len / kPanelMax)));
      for (u32 k = 0; k < parts; ++k) {
        const f32 t = (k + 0.5f) / static_cast<f32>(parts);
        segs.push_back({a + (c - a) * t, len / static_cast<f32>(parts), std::atan2(c.y - a.y, c.x - a.x)});
      }
    }
  u32 floors = std::max<u32>(1, static_cast<u32>(std::lround(removed / kFloor)));
  const u32 roof = std::clamp<u32>(static_cast<u32>(std::lround((b.hi.x - b.lo.x) * (b.hi.y - b.lo.y) / 60)), 1, 10);
  while (segs.size() * floors + roof > kMaxPieces && floors > 1) --floors;
  const std::size_t stride = std::max<std::size_t>(1, static_cast<std::size_t>(std::ceil(static_cast<f32>(segs.size() * floors) / static_cast<f32>(kMaxPieces - roof))));
  const f32 band = removed / static_cast<f32>(floors);
  auto launch = [&](Vec3 at, Vec3 size, f32 yaw, Vec3 tint) {
    at = at + offset;
    const Vec2 radial = norm2({at.x - center.x, at.y - center.y}, outward);
    const f32 speed = (9 + 30 * I) * rng.range(0.55f, 1.2f) * boost;
    const Vec3 v{radial.x * speed + rng.range(-5, 5), radial.y * speed + rng.range(-5, 5), rng.range(2, 8) + 8 * I * rng.range(0.2f, 1)};
    spawn(at, v, size, yaw, tint, rng);
  };
  auto tint = [&](f32 ember) { const f32 u = rng.unit(); return u < ember ? kEmber : u < 0.35f ? kDim : kConcrete; };
  for (u32 f = 0; f < floors; ++f)
    for (std::size_t i = f % stride; i < segs.size(); i += stride) {
      const Seg& s = segs[i];
      launch({s.m.x, s.m.y, b.new_top + band * (static_cast<f32>(f) + 0.5f)}, {s.len * rng.range(0.85f, 1), 0.45f, band * rng.range(0.8f, 1)}, s.yaw, tint(0.18f));
    }
  for (u32 i = 0; i < roof; ++i)
    launch({rng.range(b.lo.x, b.hi.x), rng.range(b.lo.y, b.hi.y), b.top - 0.3f}, {rng.range(3, 7), rng.range(3, 7), 0.5f}, rng.unit() * 6.28f, tint(0.25f));
}

void Destruction::spawn(Vec3 at, Vec3 v, Vec3 size, f32 yaw, Vec3 tint, Rng& rng) {
  if (pieces_.size() >= kPieces) return;  // the pool is full: no more pieces (as the original)
  Piece p;
  p.p = at;
  p.v = v;
  p.size = size;
  p.yaw = yaw;
  p.axis = kern::normalize(Vec3{rng.range(-1, 1), rng.range(-1, 1), rng.range(-1, 1)}, {1, 0, 0});
  p.angle = rng.range(0, 6.28f);
  p.spin = rng.range(-9, 9);
  p.tint = tint;
  pieces_.push_back(p);
}

void Destruction::free_torn(Torn& t) {
  if (t.mesh && r_) r_->remove(t.mesh);
  t.mesh = 0;
}

void Destruction::update(f32 dt, const CityStream& city, Rng& rng) {
  if (dt <= 0) return;
  // torn pieces: tip over the leeward edge, carried outward, then break up
  for (std::size_t i = 0; i < torn_.size();) {
    Torn& t = torn_[i];
    t.age += dt;
    const f32 k = std::fmin(1.0f, t.age / t.break_at), ease = k * k * (3 - 2 * k);
    t.offset = t.dir * (t.speed * t.age * t.age / (2 * t.break_at));
    t.offset.z = t.lift * t.age - 0.5f * kGravity * t.age * t.age * 0.35f;
    if (t.age >= t.break_at) {
      fracture(t.b, t.center, t.offset, 1 + 0.35f * t.tilt, rng);
      free_torn(t);
      torn_.erase(torn_.begin() + static_cast<std::ptrdiff_t>(i));
      continue;
    }
    const kern::Mat4 m = kern::Mat4::translate(t.pivot + t.offset) * kern::Mat4::trs({}, kern::Quat::axis_angle(t.axis, t.tilt * ease), {1, 1, 1});
    if (r_) r_->update_instances(t.mesh, kern::Span<const f32>(m.m, 16), {});
    ++i;
  }
  // debris: ballistic, lands on the ground and rests, sinks away in its last 3 s
  for (std::size_t i = 0; i < pieces_.size();) {
    Piece& p = pieces_[i];
    p.age += dt;
    if (p.age > kPieceLife) { p = pieces_.back(); pieces_.pop_back(); continue; }
    if (!p.rest) {
      p.v.z -= kGravity * dt;
      p.v = p.v * std::exp(-0.08f * dt);
      p.p = p.p + p.v * dt;
      p.angle += p.spin * dt;
      const f32 floor = city.ground_height(p.p.x, p.p.y) + p.size.z * 0.5f;
      if (p.p.z < floor) {
        p.p.z = floor;
        if (std::fabs(p.v.z) < 2.2f && std::hypot(p.v.x, p.v.y) < 1.5f) { p.rest = true; p.v = {}; p.spin = 0; }
        else { p.v = {p.v.x * 0.55f, p.v.y * 0.55f, -p.v.z * 0.28f}; p.spin *= 0.5f; }
      }
    }
    ++i;
  }
  stats_.pieces = static_cast<u32>(pieces_.size());
  stats_.torn = static_cast<u32>(torn_.size());
}

void Destruction::draw(CombatArt& art) const {
  for (const Piece& p : pieces_) {
    const f32 sink = p.age > kPieceLife - 3 ? (p.age - (kPieceLife - 3)) / 3 * p.size.z : 0;
    const kern::Quat q = kern::Quat::axis_angle(p.axis, p.angle) * kern::Quat::axis_angle({0, 0, 1}, p.yaw);
    art.put(CombatArt::box, kern::Mat4::trs(p.p - Vec3{0, 0, sink}, q, p.size), p.tint);
  }
}

void Destruction::reset(CityStream& city) {
  for (Torn& t : torn_) free_torn(t);
  torn_.clear();
  pieces_.clear();
  points_.clear();
  points_log_.clear();
  city.clear_damage();
  stats_ = {};
}

}  // namespace sc
