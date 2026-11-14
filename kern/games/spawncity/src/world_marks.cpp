// What blasts leave behind (see world_marks.h).
#include "world_marks.h"

#include <algorithm>

namespace sc {

namespace {
constexpr u32 kScorches = 40, kSoots = 48, kBreaches = 64, kRubble = 36, kCracks = 160;  // world_impacts.mjs / facade_marks.mjs
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
  cracks_.clear();
  crack_cursor_ = 0;
  scorch_cursor_ = soot_cursor_ = breach_cursor_ = rubble_cursor_ = 0;
  dirty_ = true;
  stats_ = {};
}

void Marks::blast(Vec3 at, f32 r, f32 strength, CityStream& city, kern::physics::World& w, Rng& rng, f32 now, bool craters) {
  struck_ = true;
  Terrain& t = city.terrain();
  r = clampf(r, 1.5f, 400);
  const f32 above = at.z - t.height(at.x, at.y);
  // The big ones dig into the ground (terrain_craters.mjs profile, scaled to the blast).
  if (craters && r >= 8 && above < 1.5f) {  // WORLD: DESTRUCTIBLE TERRAIN
    // The ground is a 5 m node grid (one height function for Box3D and the mesh): a bowl smaller
    // than a cell falls between the nodes and leaves no dent at all. So the crater sits on the node
    // nearest the blast and spans at least 5.5 m (its bowl then always reaches that node).
    const f32 R = std::fmax(std::fmin(r * 0.45f, 8.0f), 5.5f);
    const Vec2 c{std::round(at.x / 5) * 5, std::round(at.y / 5) * 5};
    t.add_crater(c, R, R * 0.18f);
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
  std::stable_sort(cand.begin(), cand.end(), [](const C& a, const C& b) { return a.d < b.d; });
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
  std::stable_sort(cand.begin(), cand.end(), [](const C& a, const C& b) { return a.d < b.d; });
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
      // full: the oldest chunk (the soonest to expire) makes room
      Chunk* old = &rubble_[0];
      for (Chunk& x : rubble_) if (x.until < old->until) old = &x;
      if (old->body.valid() && w.body_valid(old->body)) w.destroy_body(old->body);
      *old = c;
    }
  }
}

namespace {
f32 fh(f32 n) { const f32 v = std::sin(n * 127.1f) * 43758.5453f; return v - std::floor(v); }
// facades.cpp FACADE_GLSL's window mask on the CPU (world_city_buildings.mjs facadeGlassAt): is
// this point of a wall glass? u along the wall, z above the base, H the wall's height.
bool facade_glass(f32 u, f32 len, f32 z, f32 H, u32 seed) {
  if (len <= 0 || z < 0 || z > H) return false;
  const f32 sdq = static_cast<f32>(seed % 997) / 997.0f;
  const f32 floor_h = 3.1f + 0.5f * (static_cast<f32>((seed >> 12) % 1000) / 1000.0f);
  const bool pil = !(u >= 0.38f && u <= len - 0.38f);
  if (z < 4 && len > 3) {
    const f32 sp = 4.4f, ns = std::fmax(1.0f, std::floor(len / sp)), sm = (len - ns * sp) * 0.5f;
    const f32 su = (u - sm) / sp - std::floor((u - sm) / sp);
    const bool in_s = u - sm >= 0 && u - sm <= ns * sp;
    return in_s && su >= 0.08f && su <= 0.92f && z >= 0.35f && z <= 3.0f;
  }
  if (H >= 26) {
    const f32 fz = (z - 4) / floor_h - std::floor((z - 4) / floor_h);
    const f32 mu = u / 1.6f - std::floor(u / 1.6f);
    return fz < 0.8f && mu < 0.95f && !pil;
  }
  const f32 pitch = 2.7f + (3.7f - 2.7f) * fh(sdq * 7.0f);
  const f32 n_win = std::fmax(1.0f, std::floor((len - 0.8f) / pitch)), margin = (len - n_win * pitch) * 0.5f;
  const f32 lu = (u - margin) / pitch, fu = lu - std::floor(lu);
  if (lu < 0 || lu > n_win || z > H - 1.2f) return false;
  const f32 fz = (z - 4) / floor_h - std::floor((z - 4) / floor_h);
  const f32 ww = 0.38f + (0.6f - 0.38f) * fh(sdq * 5.0f), wh = 0.42f + (0.6f - 0.42f) * fh(sdq * 9.0f);
  return fu >= 0.5f - ww * 0.5f && fu <= 0.5f + ww * 0.5f && fz >= 0.3f && fz <= 0.3f + wh;
}
}  // namespace

bool Marks::bullet(Vec3 p, const CityStream& city, Rng& rng) {
  struck_ = true;
  city.walls_near({p.x, p.y}, 0.6f, near_);
  for (const CityStream::Wall& w : near_) {
    const Vec2 rel{p.x - w.a.x, p.y - w.a.y};
    const f32 u = rel.x * w.dir.x + rel.y * w.dir.y, off = rel.x * w.normal.x + rel.y * w.normal.y;
    if (u < 0 || u > w.len || std::fabs(off) > 0.3f || p.z < w.base || p.z > w.top) continue;
    if (!facade_glass(u, w.len, p.z - w.base, w.top - w.base, w.seed)) return false;
    // addGlassCrack: on the pane (the outer wall plane), size .16 .. .26 m
    WallMark m;
    m.c = Vec3{w.a.x + w.dir.x * u + w.normal.x * 0.03f, w.a.y + w.dir.y * u + w.normal.y * 0.03f, p.z};
    m.n = w.normal;
    m.d = w.dir;
    m.size = 0.16f + rng.unit() * 0.1f;
    m.seed = rng.next();
    if (cracks_.size() < kCracks) cracks_.push_back(m);
    else cracks_[crack_cursor_ % kCracks] = m;
    ++crack_cursor_;
    dirty_ = true;
    return true;
  }
  return false;
}

void Marks::update(f32 now, const CityStream& city, kern::physics::World& w) {
  for (auto it = rubble_.begin(); it != rubble_.end();) {
    if (now > it->until) {
      if (it->body.valid() && w.body_valid(it->body)) w.destroy_body(it->body);
      it = rubble_.erase(it);
    } else ++it;
  }
  if (struck_ && city.damage_serial() != damage_seen_) { damage_seen_ = city.damage_serial(); prune_walls(city); }
  stats_.scorches = static_cast<u32>(scorches_.size());
  stats_.soots = static_cast<u32>(soots_.size());
  stats_.breaches = static_cast<u32>(breaches_.size());
  stats_.rubble = static_cast<u32>(rubble_.size());
}

void Marks::prune_walls(const CityStream& city) {
  // soot, breaches and window cracks sit on a wall: when the building lost the floors they were on
  // (or fell to its stump), they go - nothing stays hanging in the air where the facade was
  std::vector<CityStream::Wall> walls;
  auto gone = [&](const WallMark& m) {
    city.walls_near({m.c.x, m.c.y}, 0.8f, walls);
    for (const CityStream::Wall& w : walls)
      if (w.top >= m.c.z - 0.15f && w.base <= m.c.z + 0.5f) return false;
    return true;
  };
  for (std::vector<WallMark>* v : {&soots_, &breaches_, &cracks_}) {
    const std::size_t before = v->size();
    v->erase(std::remove_if(v->begin(), v->end(), gone), v->end());
    if (v->size() != before) dirty_ = true;
  }
  soot_cursor_ = std::min<u32>(soot_cursor_, static_cast<u32>(soots_.size()));
  breach_cursor_ = std::min<u32>(breach_cursor_, static_cast<u32>(breaches_.size()));
  crack_cursor_ = std::min<u32>(crack_cursor_, static_cast<u32>(cracks_.size()));
}

void Marks::rebuild(const CityStream& city) {
  const Terrain& t = city.terrain();
  for (kern::MeshData& g : ground_data_) g.clear();
  wall_data_.clear();
  constexpr i32 kSeg = 18;
  // Ground scorches (world_impacts.mjs SCORCH_FS): a ragged disc draped on the terrain, charred dark
  // in the middle (.012) to soot at the rim (.05), alpha .6 that fades out over the outer 30 % -
  // "softer than a painted disc: the ground still shows through at the rim". Kern has no per-vertex
  // alpha: the char and the two outer bands are three blended meshes (alpha .6 / .4 / .15).
  for (const Scorch& s : scorches_) {
    const f32 sd = static_cast<f32>(s.seed % 1000) / 100;
    auto at = [&](f32 x, f32 y) { return Vec3{x, y, t.height(x, y) + 0.16f}; };
    auto ring_at = [&](f32 k, i32 i) {
      const f32 a = 2 * kPi * static_cast<f32>(i) / kSeg, rr = s.r * k * ragged(a, sd);
      return at(s.c.x + std::cos(a) * rr, s.c.y + std::sin(a) * rr);
    };
    auto char_col = [](f32 k) {  // mix(.05, .012, smoothstep(1, .35, k))
      const f32 u = clampf((1 - k) / 0.65f, 0, 1), c = u * u * (3 - 2 * u);
      return Vec3{0.05f, 0.045f, 0.04f} * (1 - c) + Vec3{0.012f, 0.01f, 0.01f} * c;
    };
    auto band = [&](kern::MeshData& g, f32 k0, f32 k1) {
      const u32 base = g.vertex_count();
      for (i32 i = 0; i < kSeg; ++i) g.push(ring_at(k0, i), {0, 0, 1}, char_col(k0), {});
      for (i32 i = 0; i < kSeg; ++i) g.push(ring_at(k1, i), {0, 0, 1}, char_col(k1), {});
      for (i32 i = 0; i < kSeg; ++i) {
        const u32 j = (i + 1) % kSeg, a0 = base + i, a1 = base + j, b0 = a0 + kSeg, b1 = a1 + kSeg;
        g.indices.insert(g.indices.end(), {a0, b0, b1, a0, b1, a1});
      }
    };
    kern::MeshData& c = ground_data_[0];
    const u32 base = c.vertex_count();
    c.push(at(s.c.x, s.c.y), {0, 0, 1}, char_col(0), {});
    for (i32 i = 0; i < kSeg; ++i) c.push(ring_at(0.35f, i), {0, 0, 1}, char_col(0.35f), {});
    for (i32 i = 0; i < kSeg; ++i) c.indices.insert(c.indices.end(), {base, base + 1 + i, base + 1 + static_cast<u32>((i + 1) % kSeg)});
    band(c, 0.35f, 0.7f);
    band(ground_data_[1], 0.7f, 0.86f);
    band(ground_data_[2], 0.86f, 1.0f);
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
    Rng cr{m.seed | 1};
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
  // facade_marks.mjs CRACK_FS: radial spokes, two broken rings, a tiny pit - light on the pane
  for (const WallMark& m : cracks_) {
    const Vec3 n{m.n.x, m.n.y, 0};
    Rng cr{m.seed | 1};
    auto line = [&](Vec2 a, Vec2 b, f32 w, Vec3 col) {
      const Vec2 d = b - a;
      const f32 l = std::fmax(1e-4f, std::hypot(d.x, d.y));
      const Vec2 s{-d.y / l * w, d.x / l * w};
      const u32 base = wall_data_.vertex_count();
      for (const Vec2 q : {a + s, a - s, b - s, b + s}) wall_data_.push(m.c + Vec3{m.d.x * q.x, m.d.y * q.x, q.y}, n, col, {});
      wall_data_.indices.insert(wall_data_.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
    };
    const Vec3 c{0.82f, 0.86f, 0.9f};
    const i32 spokes = 7 + static_cast<i32>(cr.next() % 4);
    std::vector<Vec2> ends;
    for (i32 i = 0; i < spokes; ++i) {
      const f32 a = 2 * kPi * (static_cast<f32>(i) + (cr.unit() - 0.5f) * 0.6f) / static_cast<f32>(spokes);
      const f32 r = m.size * (0.6f + 0.4f * cr.unit());
      const Vec2 e{std::cos(a) * r, std::sin(a) * r};
      line(e * 0.08f, e * 0.55f, 0.004f, c);
      line(e * 0.55f, e, 0.003f, c);
      ends.push_back(e);
    }
    for (std::size_t i = 0; i < ends.size(); ++i) {
      const Vec2 a = ends[i], b = ends[(i + 1) % ends.size()];
      line(a * 0.35f, b * 0.35f, 0.003f, c);
      if (cr.unit() < 0.7f) line(a * 0.7f, b * 0.7f, 0.0025f, c);
    }
    line({-0.01f, 0}, {0.01f, 0}, 0.01f, {0.95f, 0.97f, 1.0f});  // the pit
  }
}

void Marks::draw(WorldArt& art, const CityStream& city, kern::physics::World& w, f32 now) {
  if (dirty_ && struck_) {
    rebuild(city);
    kern::Material m;
    m.alpha = kern::AlphaMode::blend;
    m.color = {1, 1, 1, 0.88f};
    m.roughness = 1;
    m.cast_shadow = false;
    m.double_sided = true;
    art.set_mesh(wall_mesh_, wall_data_, m);
    const f32 alphas[3] = {0.6f, 0.4f, 0.15f};
    for (int k = 0; k < 3; ++k) {
      kern::Material g = m;
      g.color = {1, 1, 1, alphas[k]};
      art.set_mesh(ground_mesh_[k], ground_data_[k], g);
    }
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
