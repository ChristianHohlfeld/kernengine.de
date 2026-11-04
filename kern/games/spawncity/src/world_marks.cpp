// What blasts leave behind (see world_marks.h).
#include "world_marks.h"

#include <algorithm>

namespace sc {

namespace {
constexpr u32 kScorches = 40, kSoots = 48, kBreaches = 64, kRubble = 36;  // world_impacts.mjs / facade_marks.mjs
constexpr f32 kRubbleSeconds = 25;
const Vec3 kRubbleColor = kern::hex_color(0x8a857c);

template <class T>
void ring_put(std::vector<T>& v, u32& cursor, u32 cap, const T& item) {
  if (v.size() < cap) v.push_back(item);
  else v[cursor % cap] = item;
  ++cursor;
}

// The irregular soft edge of soot and scorch (facade_marks.mjs SOOT_FS edge).
f32 ragged(f32 a, f32 seed) { return 0.74f + 0.13f * std::sin(a * 5 + seed * 7) + 0.08f * std::sin(a * 11 + seed * 3) + 0.05f * std::sin(a * 23 + seed); }
}  // namespace

void Marks::clear(kern::physics::World* w) {
  if (w)
    for (Chunk& c : rubble_)
      if (c.body.valid() && w->body_valid(c.body)) w->destroy_body(c.body);
  rubble_.clear();
  scorches_.clear();
  soots_.clear();
  breaches_.clear();
  scorch_cursor_ = soot_cursor_ = breach_cursor_ = rubble_cursor_ = 0;
  dirty_ = true;
  stats_ = {};
}

void Marks::blast(Vec3 at, f32 r, f32 strength, CityStream& city, kern::physics::World& w, Rng& rng, f32 now) {
  city_ = &city;
  Terrain& t = city.terrain();
  r = clampf(r, 1.5f, 400);
  const f32 above = at.z - t.height(at.x, at.y);
  // The big ones dig into the ground (terrain_craters.mjs profile, scaled to the blast).
  if (r >= 8 && above < 1.5f) {
    const f32 R = std::fmin(r * 0.45f, 8.0f);
    t.add_crater({at.x, at.y}, R, R * 0.18f);
    ++stats_.craters;
  }
  // world_impacts.mjs: a scorch if the blast is low enough
  if (above < r * 0.6f) ring_put(scorches_, scorch_cursor_, kScorches, Scorch{{at.x, at.y}, clampf(std::fmin(5.5f, r * 0.5f) * 0.72f, 0.5f, 4.2f), rng.next()});
  city.walls_near({at.x, at.y}, r, near_);
  add_soot(at, r, near_, t, rng);
  add_breach(at, r, clampf(strength, 0, 1.4f), near_, t, w, rng, now);
  dirty_ = true;
}

void Marks::add_soot(Vec3 at, f32 r, const std::vector<CityStream::Wall>& walls, const Terrain& t, Rng& rng) {
  struct C { const CityStream::Wall* w; f32 u, d; };
  std::vector<C> cand;
  for (const CityStream::Wall& wl : walls) {
    const f32 rx = at.x - wl.a.x, ry = at.y - wl.a.y, u = rx * wl.dir.x + ry * wl.dir.y, side = rx * wl.normal.x + ry * wl.normal.y;
    if (side <= 0.05f || u < 0 || u > wl.len) continue;
    if (side < r * 0.55f) cand.push_back({&wl, u, side});
  }
  std::sort(cand.begin(), cand.end(), [](const C& a, const C& b) { return a.d < b.d; });
  i32 placed = 0;
  for (const C& c : cand) {
    if (placed >= 2) break;
    const CityStream::Wall& wl = *c.w;
    const Vec2 p = wl.a + wl.dir * c.u;
    const f32 ground = t.height(p.x + wl.normal.x * 0.3f, p.y + wl.normal.y * 0.3f);
    // clipped to the wall: never wider than it, never above the roof edge
    const f32 rr = std::fmin(std::fmin(r * 0.3f, c.u), std::fmin(wl.len - c.u, (wl.top - ground) * 0.5f)) * (1 - c.d / (r * 0.55f) * 0.35f);
    if (!(rr >= 0.4f)) continue;
    const f32 zc = std::fmin(std::fmax(at.z, ground + rr * 0.35f), wl.top - rr);
    if (zc + rr > wl.top + 1e-3f || zc < ground - rr * 0.5f) continue;
    WallMark m;
    m.c = {p.x + wl.normal.x * 0.01f, p.y + wl.normal.y * 0.01f, zc};
    m.n = wl.normal;
    m.d = wl.dir;
    m.size = rr;
    m.level = 0.7f * std::fmax(0.4f, 1 - c.d / (r * 0.55f));
    m.seed = rng.next();
    ring_put(soots_, soot_cursor_, kSoots, m);
    ++placed;
  }
}

void Marks::add_breach(Vec3 at, f32 r, f32 strength, const std::vector<CityStream::Wall>& walls, const Terrain& t, kern::physics::World& w, Rng& rng, f32 now) {
  struct C { const CityStream::Wall* w; f32 u, d; };
  std::vector<C> cand;
  for (const CityStream::Wall& wl : walls) {
    const f32 rx = at.x - wl.a.x, ry = at.y - wl.a.y, u = rx * wl.dir.x + ry * wl.dir.y, side = rx * wl.normal.x + ry * wl.normal.y;
    if (side <= -0.3f || u < 0 || u > wl.len) continue;
    if (side < r * 0.5f) cand.push_back({&wl, u, std::fmax(0.0f, side)});
  }
  std::sort(cand.begin(), cand.end(), [](const C& a, const C& b) { return a.d < b.d; });
  i32 placed = 0;
  for (const C& c : cand) {
    if (placed >= 2) break;
    const CityStream::Wall& wl = *c.w;
    const Vec2 foot = wl.a + wl.dir * c.u;
    const f32 ground = t.height(foot.x + wl.normal.x * 0.3f, foot.y + wl.normal.y * 0.3f);
    const f32 zc = std::fmin(std::fmax(at.z, ground + 0.4f), wl.top - 0.3f);
    if (zc > wl.top || zc < ground - 0.2f) continue;
    // Every 2 m patch of wall keeps its damage: hit again, it opens further.
    const u64 cell = (static_cast<u64>(wl.seed) << 32) ^ (static_cast<u64>(wl.edge) << 20) ^ (static_cast<u64>(static_cast<u32>(std::lround(c.u / 2))) << 10) ^
                     static_cast<u64>(static_cast<u32>(std::lround(zc / 2)));
    const f32 hit = std::fmax(0.15f, strength * (1 - c.d / (r * 0.5f)));
    WallMark* m = nullptr;
    for (WallMark& b : breaches_)
      if (b.cell == cell) { m = &b; break; }
    if (!m) {
      WallMark nb;
      nb.cell = cell;
      nb.breach = true;
      nb.seed = rng.next();
      nb.c = {foot.x + wl.normal.x * 0.015f, foot.y + wl.normal.y * 0.015f, zc};
      nb.n = wl.normal;
      nb.d = wl.dir;
      ring_put(breaches_, breach_cursor_, kBreaches, nb);
      m = &breaches_[(breach_cursor_ - 1) % kBreaches];
    }
    m->level = std::fmin(1.0f, m->level + hit * 0.45f);
    m->size = std::fmax(0.35f, std::fmin(std::fmin(r * (0.18f + 0.22f * m->level), c.u + 0.6f), std::fmin(wl.len - c.u + 0.6f, (wl.top - ground) * 0.6f)));
    spawn_rubble({foot.x + wl.normal.x * 0.5f, foot.y + wl.normal.y * 0.5f, zc}, wl.normal, static_cast<i32>(std::lround(2 + 5 * hit)), w, rng, now);
    ++placed;
  }
}

void Marks::spawn_rubble(Vec3 at, Vec2 n, i32 count, kern::physics::World& w, Rng& rng, f32 now) {
  for (i32 k = 0; k < count; ++k) {
    // facade_marks.mjs spawnRubble: concrete chunks (2400 kg/m^3), thrown out of the wall
    const Vec3 half{0.06f + rng.unit() * 0.12f, 0.06f + rng.unit() * 0.1f, 0.05f + rng.unit() * 0.08f};
    kern::physics::BodySettings bs;
    bs.type = kern::physics::BodyType::dynamic;
    bs.position = at + Vec3{(rng.unit() - 0.5f) * 0.8f, (rng.unit() - 0.5f) * 0.8f, (rng.unit() - 0.5f) * 0.8f};
    bs.rotation = Quat::axis_angle({0, 0, 1}, rng.unit() * 6.28f);
    const f32 sp = 3 + rng.unit() * 6;
    bs.linear_velocity = {n.x * sp + (rng.unit() - 0.5f) * 3, n.y * sp + (rng.unit() - 0.5f) * 3, 1 + rng.unit() * 4};
    bs.angular_velocity = {(rng.unit() - 0.5f) * 12, (rng.unit() - 0.5f) * 12, (rng.unit() - 0.5f) * 12};
    bs.linear_damping = 0.05f;
    bs.angular_damping = 0.2f;
    const kern::physics::Body b = w.create_body(bs);
    kern::physics::ShapeSettings ss;
    ss.kind = kern::physics::ShapeKind::box;
    ss.size = half * 2.0f;
    ss.material.density = 2400;
    ss.material.friction = 0.8f;
    ss.material.category_bits = kCatVehicle;  // debris: not a wall for the walkers' rays
    w.create_shape(b, ss);
    Chunk c{b, half, now + kRubbleSeconds};
    if (rubble_.size() < kRubble) rubble_.push_back(c);
    else {
      Chunk& old = rubble_[rubble_cursor_ % kRubble];
      if (old.body.valid() && w.body_valid(old.body)) w.destroy_body(old.body);
      old = c;
    }
    ++rubble_cursor_;
  }
}

void Marks::update(f32 now, kern::physics::World& w) {
  for (auto it = rubble_.begin(); it != rubble_.end();) {
    if (now > it->until) {
      if (it->body.valid() && w.body_valid(it->body)) w.destroy_body(it->body);
      it = rubble_.erase(it);
      rubble_cursor_ = static_cast<u32>(rubble_.size());
    } else ++it;
  }
  stats_.scorches = static_cast<u32>(scorches_.size());
  stats_.soots = static_cast<u32>(soots_.size());
  stats_.breaches = static_cast<u32>(breaches_.size());
  stats_.rubble = static_cast<u32>(rubble_.size());
}

void Marks::rebuild(const CityStream& city) {
  const Terrain& t = city.terrain();
  ground_data_.clear();
  wall_data_.clear();
  constexpr i32 kSeg = 18;
  // Ground scorches: a ragged disc draped on the terrain (above the street surface).
  for (const Scorch& s : scorches_) {
    const f32 sd = static_cast<f32>(s.seed % 1000) / 100;
    const u32 base = ground_data_.vertex_count();
    auto at = [&](f32 x, f32 y) { return Vec3{x, y, t.height(x, y) + 0.16f}; };
    ground_data_.push(at(s.c.x, s.c.y), {0, 0, 1}, {0.015f, 0.012f, 0.01f}, {});
    const f32 rings[3] = {0.35f, 0.7f, 1.0f};
    const Vec3 cols[3] = {{0.02f, 0.017f, 0.014f}, {0.04f, 0.034f, 0.028f}, {0.1f, 0.085f, 0.065f}};
    for (i32 k = 0; k < 3; ++k)
      for (i32 i = 0; i < kSeg; ++i) {
        const f32 a = 2 * kPi * static_cast<f32>(i) / kSeg, rr = s.r * rings[k] * ragged(a, sd);
        ground_data_.push(at(s.c.x + std::cos(a) * rr, s.c.y + std::sin(a) * rr), {0, 0, 1}, cols[k], {});
      }
    for (i32 i = 0; i < kSeg; ++i) {
      const u32 j = (i + 1) % kSeg;
      ground_data_.indices.insert(ground_data_.indices.end(), {base, base + 1 + i, base + 1 + j});
      for (u32 k = 0; k < 2; ++k) {
        const u32 a0 = base + 1 + k * kSeg + i, a1 = base + 1 + k * kSeg + j, b0 = a0 + kSeg, b1 = a1 + kSeg;
        ground_data_.indices.insert(ground_data_.indices.end(), {a0, b0, b1, a0, b1, a1});
      }
    }
  }
  // Wall marks in the wall's plane (dir along the wall, z up).
  auto wall_disc = [&](const WallMark& m, f32 inner, Vec3 c_in, f32 outer_k, Vec3 c_out, f32 seed) {
    const u32 base = wall_data_.vertex_count();
    const Vec3 n{m.n.x, m.n.y, 0};
    auto P = [&](f32 x, f32 y) { return m.c + Vec3{m.d.x * x, m.d.y * x, y}; };
    wall_data_.push(m.c, n, c_in, {});
    for (i32 i = 0; i < kSeg; ++i) {
      const f32 a = 2 * kPi * static_cast<f32>(i) / kSeg, e = ragged(a, seed);
      wall_data_.push(P(std::cos(a) * m.size * inner * e, std::sin(a) * m.size * inner * e), n, c_in, {});
    }
    for (i32 i = 0; i < kSeg; ++i) {
      const f32 a = 2 * kPi * static_cast<f32>(i) / kSeg, e = ragged(a, seed);
      wall_data_.push(P(std::cos(a) * m.size * outer_k * e, std::sin(a) * m.size * outer_k * e), n, c_out, {});
    }
    for (i32 i = 0; i < kSeg; ++i) {
      const u32 j = (i + 1) % kSeg, a0 = base + 1 + i, a1 = base + 1 + j, b0 = a0 + kSeg, b1 = a1 + kSeg;
      wall_data_.indices.insert(wall_data_.indices.end(), {base, a0, a1, a0, b0, b1, a0, b1, a1});
    }
  };
  for (const WallMark& m : soots_) {
    const f32 sd = static_cast<f32>(m.seed % 1000) / 100;
    const f32 k = 0.06f + 0.05f * (1 - m.level);
    wall_disc(m, 0.55f, {0.015f, 0.012f, 0.01f}, 1.0f, {k, k * 0.86f, k * 0.72f}, sd);
  }
  for (const WallMark& m : breaches_) {
    const f32 sd = static_cast<f32>(m.seed % 1000) / 100;
    // facade_marks.mjs BREACH_FS: dark hole growing with the damage, broken concrete rim, cracks
    const f32 hole = 0.18f + 0.42f * m.level;
    const f32 g = 0.46f - 0.2f * m.level;
    wall_disc(m, hole, {0.035f, 0.03f, 0.028f}, 0.82f, {g, g * 0.96f, g * 0.9f}, sd);
    const Vec3 n{m.n.x, m.n.y, 0};
    Rng cr(m.seed | 1);
    const i32 cracks = 5 + static_cast<i32>(cr.next() % 5);
    for (i32 i = 0; i < cracks; ++i) {
      const f32 a = 2 * kPi * (static_cast<f32>(i) + cr.unit() * 0.7f) / static_cast<f32>(cracks);
      const f32 r0 = m.size * hole * 0.9f, r1 = m.size * (0.55f + 0.45f * cr.unit()) * (1.0f + 0.4f * m.level), wdt = m.size * 0.025f;
      const Vec2 dir{std::cos(a), std::sin(a)}, side{-dir.y, dir.x};
      const u32 base = wall_data_.vertex_count();
      for (const Vec2 q : {dir * r0 + side * wdt, dir * r0 - side * wdt, dir * r1 - side * (wdt * 0.2f), dir * r1 + side * (wdt * 0.2f)})
        wall_data_.push(m.c + Vec3{m.d.x * q.x, m.d.y * q.x, q.y} + n * 0.002f, n, {0.03f, 0.028f, 0.026f}, {});
      wall_data_.indices.insert(wall_data_.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
    }
  }
}

void Marks::draw(WorldArt& art, kern::physics::World& w, f32 now) {
  if (dirty_ && city_) {
    rebuild(*city_);
    kern::Material m;
    m.alpha = kern::AlphaMode::blend;
    m.color = {1, 1, 1, 0.88f};
    m.roughness = 1;
    m.cast_shadow = false;
    m.double_sided = true;
    art.set_mesh(ground_mesh_, ground_data_, m);
    art.set_mesh(wall_mesh_, wall_data_, m);
    dirty_ = false;
  }
  for (const Chunk& c : rubble_) {
    if (!c.body.valid() || !w.body_valid(c.body)) continue;
    const kern::physics::Pose p = w.pose(c.body);
    const f32 fade = clampf((c.until - now) / 2.5f, 0, 1);
    art.put(art.cube, kern::Mat4::trs(p.position, p.rotation, c.half * (2 * fade)), kRubbleColor);
  }
}

}  // namespace sc
