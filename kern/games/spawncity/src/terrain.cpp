// The ground of the streamed city (see terrain.h).
#include "terrain.h"

#include <algorithm>
#include <cstdio>

#include "kern/core/base.h"
#include "kern/core/jobs.h"
#include <cstring>
#include <unordered_map>

namespace sc {

namespace {

constexpr f64 kPiD = 3.14159265358979323846;
constexpr f32 kTile = 160;       // collision / near render tile (32 cells of 5 m: terrain_tiles.mjs TILE_CELLS)
constexpr f32 kBlock = 640;      // far render tile
constexpr i32 kCells = 32;
constexpr f32 kWaterBlock = 64;  // 16 x 16 water cells
constexpr f32 kWaterChunk = 320;

u64 key2(i32 i, i32 j) { return (static_cast<u64>(static_cast<u32>(i)) << 32) | static_cast<u32>(j); }
u64 key3(i32 level, i32 i, i32 j) {
  return (static_cast<u64>(level) << 56) | ((static_cast<u64>(static_cast<u32>(i)) & 0xFFFFFFF) << 28) | (static_cast<u64>(static_cast<u32>(j)) & 0xFFFFFFF);
}
i32 sx28(u64 v) { return static_cast<i32>(static_cast<u32>(v << 4)) >> 4; }
void unkey3(u64 k, i32& level, i32& i, i32& j) {
  level = static_cast<i32>(k >> 56);
  i = sx28((k >> 28) & 0xFFFFFFF);
  j = sx28(k & 0xFFFFFFF);
}
f32 rect_distance(f32 x, f32 y, f32 x0, f32 y0, f32 x1, f32 y1) {
  const f32 dx = std::fmax(0.0f, std::fmax(x0 - x, x - x1)), dy = std::fmax(0.0f, std::fmax(y0 - y, y - y1));
  return std::sqrt(dx * dx + dy * dy);
}
f64 merc_y(f64 lat, i32 z) {
  const f64 r = lat * kPiD / 180;
  return (1 - std::asinh(std::tan(r)) / kPiD) / 2 * std::ldexp(1.0, z);
}
f64 merc_x(f64 lon, i32 z) { return (lon + 180) / 360 * std::ldexp(1.0, z); }

// Sliding-window min (op < 0) / max (op > 0) along lines (monotonic deque), window 2r+1.
void slide(const std::vector<f32>& src, std::vector<f32>& dst, u32 n, u32 stride, u32 line_stride, i32 r, i32 op, std::vector<i32>& q) {
  q.resize(n);
  for (u32 l = 0; l < n; ++l) {
    const u32 base = l * line_stride;
    i32 qh = 0, qt = 0;
    i32 j = 0;
    for (i32 i = 0; i < static_cast<i32>(n); ++i) {
      const i32 hi = std::min(static_cast<i32>(n) - 1, i + r);
      for (; j <= hi; ++j) {
        const f32 v = src[base + j * stride];
        while (qt > qh && (op > 0 ? src[base + q[qt - 1] * stride] <= v : src[base + q[qt - 1] * stride] >= v)) --qt;
        q[qt++] = j;
      }
      while (q[qh] < i - r) ++qh;
      dst[base + i * stride] = src[base + q[qh] * stride];
    }
  }
}
void box_blur(const std::vector<f32>& src, std::vector<f32>& dst, u32 n, u32 stride, u32 line_stride, i32 r) {
  for (u32 l = 0; l < n; ++l) {
    const u32 base = l * line_stride;
    f64 sum = 0;
    i32 cnt = 0;
    for (i32 j = 0; j <= std::min(static_cast<i32>(n) - 1, r); ++j) { sum += src[base + j * stride]; ++cnt; }
    for (i32 i = 0; i < static_cast<i32>(n); ++i) {
      dst[base + i * stride] = static_cast<f32>(sum / cnt);
      const i32 add = i + r + 1, rem = i - r;
      if (add < static_cast<i32>(n)) { sum += src[base + add * stride]; ++cnt; }
      if (rem >= 0) { sum -= src[base + rem * stride]; --cnt; }
    }
  }
}
// One filter pass of bare_earth (0..7).
void filter_pass(std::vector<f32>& h, std::vector<f32>& t, u32 n, i32 pass, i32 ro, i32 rb, std::vector<i32>& q) {
  switch (pass) {
    case 0: slide(h, t, n, 1, n, ro, -1, q); break;  // erode rows
    case 1: slide(t, h, n, n, 1, ro, -1, q); break;  // erode columns
    case 2: slide(h, t, n, 1, n, ro, 1, q); break;   // dilate rows
    case 3: slide(t, h, n, n, 1, ro, 1, q); break;   // dilate columns
    case 4: case 6: box_blur(h, t, n, 1, n, rb); break;
    case 5: case 7: box_blur(t, h, n, n, 1, rb); break;
  }
}

const Vec3 kGround{0.437f, 0.485f, 0.349f};  // world_ground.mjs fallbackColors (.16, .2, .1 linear) in sRGB
const Vec3 kBed{0.30f, 0.29f, 0.22f};
const Vec3 kSoil = kern::hex_color(0x6b5a44);  // world_impacts.mjs ground chip colour

}  // namespace

f32 crater_profile(f32 r, f32 radius, f32 depth, f32 rim) {
  const f32 bowl = radius * 0.5f;
  if (r < bowl) return -depth * (1 - (r / bowl) * (r / bowl));
  if (r >= radius) return 0;
  const f32 x = (r - bowl) / (radius - bowl), k = (x - 0.12f) / 0.14f;
  return rim * std::exp(-(k * k)) * (1 - x);
}

void bare_earth(std::vector<f32>& h, u32 n, f32 step, f32 open_m, f32 blur_m) {
  std::vector<f32> t(h.size());
  std::vector<i32> q;
  const i32 ro = std::max(1, static_cast<i32>(std::lround(open_m / step / 2))), rb = std::max(1, static_cast<i32>(std::lround(blur_m / step / 2)));
  for (i32 p = 0; p < 8; ++p) filter_pass(h, t, n, p, ro, rb, q);
}

namespace {
/// A Terrarium tile in kDemJob's input: its size and where its heights lie in the bytes.
struct DemBytes { u32 w, h; std::size_t at; };

// kDemJob: Terrarium tiles -> the bare-earth grid (sample_dem + bare_earth, without engine state).
void dem_grid_job(kern::Span<const u8> in, std::vector<u8>& out) {
  std::size_t p = 0;  // where the next value lies in the bytes
  bool ok = true;
  auto get = [&](void* v, std::size_t n) { if (in.size - p < n) { ok = false; return; } std::memcpy(v, in.data + p, n); p += n; };
  f64 lon0 = 0, lat0 = 0;
  f32 x0 = 0, y0 = 0, step = 10, open = 160, blur = 30;
  u32 n = 0, count = 0;
  get(&lon0, 8); get(&lat0, 8); get(&x0, 4); get(&y0, 4); get(&step, 4); get(&n, 4); get(&open, 4); get(&blur, 4); get(&count, 4);
  if (!ok || n < 2 || n > 4096) return;
  std::unordered_map<u64, DemBytes> tiles;
  for (u32 k = 0; k < count && ok; ++k) {
    u32 x = 0, y = 0, w = 0, h = 0;
    get(&x, 4); get(&y, 4); get(&w, 4); get(&h, 4);
    const std::size_t bytes = static_cast<std::size_t>(w) * h * 4;
    if (!ok || w < 2 || h < 2 || in.size - p < bytes) return;
    tiles[TileKey{Terrain::kDemZoom, static_cast<i32>(x), static_cast<i32>(y)}.hash()] = {w, h, p};
    p += bytes;
  }
  kern::GeoProjector proj;
  if (!proj.init(lon0, lat0)) return;
  std::vector<f32> grid(static_cast<std::size_t>(n) * n);
  for (u32 row = 0; row < n; ++row) {
    const f32 y = y0 + static_cast<f32>(row) * step;
    for (u32 i = 0; i < n; ++i) {
      f64 lon, lat;
      unproject(proj, x0 + static_cast<f32>(i) * step, y, lon, lat);
      const f64 fx = merc_x(lon, Terrain::kDemZoom), fy = merc_y(lat, Terrain::kDemZoom);
      const i32 tx = static_cast<i32>(std::floor(fx)), ty = static_cast<i32>(std::floor(fy));
      auto it = tiles.find(TileKey{Terrain::kDemZoom, tx, ty}.hash());
      if (it == tiles.end()) return;  // a hole: no grid (retried)
      const DemBytes& t = it->second;
      // terrain_elevation.mjs: bilinear in the tile's pixels, clamped inside the tile
      const f64 px = std::min(t.w - 1.001, (fx - tx) * t.w), py = std::min(t.h - 1.001, (fy - ty) * t.h);
      const u32 a = static_cast<u32>(px), b = static_cast<u32>(py);
      const f32 ax = static_cast<f32>(px - a), ay = static_cast<f32>(py - b);
      f32 v0, v1, v2, v3;
      const std::size_t h = t.at + (static_cast<std::size_t>(b) * t.w + a) * 4;
      std::memcpy(&v0, in.data + h, 4); std::memcpy(&v1, in.data + h + 4, 4); std::memcpy(&v2, in.data + h + static_cast<std::size_t>(t.w) * 4, 4); std::memcpy(&v3, in.data + h + (static_cast<std::size_t>(t.w) + 1) * 4, 4);
      grid[static_cast<std::size_t>(row) * n + i] = (v0 * (1 - ax) + v1 * ax) * (1 - ay) + (v2 * (1 - ax) + v3 * ax) * ay;
    }
  }
  bare_earth(grid, n, step, open, blur);
  out.resize(grid.size() * 4);
  std::memcpy(out.data(), grid.data(), out.size());
}
KERN_JOB(Terrain::kDemJob, dem_grid_job);
}  // namespace

Terrain::Terrain() = default;
Terrain::~Terrain() = default;

void Terrain::start(kern::Renderer* renderer, kern::physics::World* world, const kern::GeoProjector& proj, bool dem) {
  clear();
  r_ = renderer;
  world_ = world;
  proj_ = proj;
  dem_ = dem;
  settled_ = !dem;
}

void Terrain::clear() {
  if (r_) {
    for (auto& [k, t] : render_) if (t.mesh) r_->remove(t.mesh);
    for (auto& [k, m] : water_meshes_) if (m) r_->remove(m);
  }
  if (world_)
    for (auto& [k, c] : collision_)
      if (c.body.valid() && world_->body_valid(c.body)) world_->destroy_body(c.body);
  render_.clear();
  collision_.clear();
  water_meshes_.clear();
  water_detail_.clear();
  dirty_render_.clear();
  dirty_collision_.clear();
  dirty_water_.clear();
  want_render_.clear();
  want_collision_.clear();
  want_water_.clear();
  water_.clear();
  wc_key_ = ~0ull;
  bridges_.clear();
  craters_.clear();
  tints_.clear();
  images_.clear();
  image_requests_.clear();
  image_at_ = {1e9f, 1e9f};
  dem_tiles_.clear();
  requests_.clear();
  if (job_ && job_->id) kern::jobs::cancel(job_->id);
  job_.reset();
  grid_ = Grid{};
  side_.clear();
  corridors_.clear();
  corridor_hit_ = -1;
  grid_ready_ = false;
  ref_ready_ = false;
  ref_height_ = 0;
  dem_failures_ = 0;
  retry_at_ = 0;
  changes_.clear();
  replan_ = true;
  planned_focus_ = planned_anchor_ = {1e9f, 1e9f, 0};
  stats_ = {};
  settled_ = !dem_;
}

void Terrain::disable_dem() {
  dem_ = false;
  if (job_ && job_->id) kern::jobs::cancel(job_->id);
  job_.reset();
  requests_.clear();
  settled_ = true;
}

// ---------------------------------------------------------------------------------------------
// elevation tiles

std::string Terrain::take_requests() {
  std::string s = "[";
  for (std::size_t i = 0; i < requests_.size(); ++i) {
    char b[64];
    std::snprintf(b, sizeof b, "%s[%d,%d,%d]", i ? "," : "", requests_[i].z, requests_[i].x, requests_[i].y);
    s += b;
  }
  requests_.clear();
  return s + "]";
}

void Terrain::provide(const TileKey& key, kern::Span<const u8> rgba, u32 width, u32 height) {
  auto it = dem_tiles_.find(key.hash());
  if (it == dem_tiles_.end() || width == 0 || height == 0) return;
  DemTile& t = it->second;
  t.w = width;
  t.hgt = height;
  t.h.resize(static_cast<std::size_t>(width) * height);
  for (std::size_t i = 0; i < t.h.size(); ++i) t.h[i] = terrarium_height(rgba[i * 4], rgba[i * 4 + 1], rgba[i * 4 + 2]);
  t.failed = false;
}

void Terrain::fail(const TileKey& key) {
  auto it = dem_tiles_.find(key.hash());
  if (it == dem_tiles_.end()) return;
  it->second.failed = true;
  ++stats_.dem_failed;
}

f32 Terrain::sample_dem(f64 lon, f64 lat, bool& ok) const {
  const f64 fx = merc_x(lon, kDemZoom), fy = merc_y(lat, kDemZoom);
  const i32 tx = static_cast<i32>(std::floor(fx)), ty = static_cast<i32>(std::floor(fy));
  auto it = dem_tiles_.find(TileKey{kDemZoom, tx, ty}.hash());
  if (it == dem_tiles_.end() || it->second.h.empty()) { ok = false; return 0; }
  const DemTile& t = it->second;
  // terrain_elevation.mjs: bilinear in the tile's pixels, clamped inside the tile
  const f64 px = std::min(t.w - 1.001, (fx - tx) * t.w), py = std::min(t.hgt - 1.001, (fy - ty) * t.hgt);
  const u32 i = static_cast<u32>(px), j = static_cast<u32>(py);
  const f32 ax = static_cast<f32>(px - i), ay = static_cast<f32>(py - j);
  const std::size_t at = static_cast<std::size_t>(j) * t.w + i;
  return (t.h[at] * (1 - ax) + t.h[at + 1] * ax) * (1 - ay) + (t.h[at + t.w] * (1 - ax) + t.h[at + t.w + 1] * ax) * ay;
}

// Host: the elevation round a mate too far for the player's own window (8 km) - a window of his own
// (built the same way, after the player's), so the ground under his cars and people is the real
// one here as on his machine. Windows nobody is near any more are dropped.
// ---- the elevation windows ----------------------------------------------------------------------
// The ground's height is a function of the place alone: every window samples the same 10 m
// lattice (its corner is a multiple of 50 m) through the same filter, so two windows agree
// wherever both are further than the filter's reach (kDemMargin) from their edges - their
// "interior". Only the interior counts as known ground: the city builds a map tile once its whole
// square lies in an interior (covers), and a player is put down only there (ground_ready). So
// nothing ever stands on a height that a later window would change, and nothing has to be moved
// when one arrives.
namespace {
kern::Vec4 interior_of(f32 x0, f32 y0, f32 span, f32 margin) { return {x0 + margin, y0 + margin, x0 + span - margin, y0 + span - margin}; }
bool rect_in(const kern::Vec4& in, const kern::Vec4& out) { return in.x >= out.x && in.y >= out.y && in.z <= out.z && in.w <= out.w; }
}  // namespace

kern::Vec4 Terrain::interior(const Grid& g) const {
  return interior_of(g.x0, g.y0, g.step * static_cast<f32>(g.n > 0 ? g.n - 1 : 0), kDemMargin);
}

bool Terrain::covers(const kern::Vec4& rect) const {
  if (!settled_) return false;
  if (!dem_) return true;  // flat ground: known everywhere
  if (!grid_ready_) return false;
  if (rect_in(rect, interior(grid_))) return true;
  for (const Grid& g : side_) if (rect_in(rect, interior(g))) return true;
  return false;
}

void Terrain::plan_windows(Vec3 focus) {
  if (!dem_ || job_) return;
  // what must be known: the city's tiles round the player (needs[0]) and round each far mate
  // (host); without a city (tests) a square round the focus
  kern::Vec4 own{focus.x - 1500, focus.y - 1500, focus.x + 1500, focus.y + 1500};
  if (!needs.empty()) own = needs[0];
  // one window for everybody when they fit in it, else the player's own and one per far mate
  kern::Vec4 all = own;
  for (std::size_t i = 1; i < needs.size(); ++i) all = {std::fmin(all.x, needs[i].x), std::fmin(all.y, needs[i].y), std::fmax(all.z, needs[i].z), std::fmax(all.w, needs[i].w)};
  const bool together = needs.size() > 1 && all.z - all.x < kDemMaxSize - 2 * kDemMargin - 800 && all.w - all.y < kDemMaxSize - 2 * kDemMargin - 800;
  const kern::Vec4 want = together ? all : own;
  // the player's window: a new one when what is needed comes within the slack of its interior's
  // edge (a tile's width ahead is loaded with it, so the next row of tiles is known before the
  // player reaches it)
  const f32 tile = std::fmax(400.0f, (own.z - own.x) / 3), slack = std::fmin(400.0f, tile * 0.25f);
  bool planned = false;
  if (!grid_ready_ || !rect_in({want.x - slack, want.y - slack, want.z + slack, want.w + slack}, interior(grid_))) {
    // (a window that already holds what is needed is only replaced by one that holds more of it)
    Vec2 c{(want.x + want.z) / 2, (want.y + want.w) / 2};
    // a fast craft: the window leads it, as far as what is needed now stays inside
    const f32 lead_max = tile;
    c.x += clampf(lead.x, -lead_max, lead_max);
    c.y += clampf(lead.y, -lead_max, lead_max);
    const f32 half_need = std::fmax(want.z - want.x, want.w - want.y) / 2 + lead_max + kDemMargin + tile;
    const f32 half = clampf(std::ceil(std::fmax(kDemSize / 2, half_need) / 50) * 50, kDemSize / 2, kDemMaxSize / 2);
    planned = plan_dem(c, half, false);
    if (planned || job_) return;
  }
  if (together || !grid_ready_) { side_.clear(); return; }
  // far mates: their own windows (the ground there is the real one for the host's traffic too)
  side_.erase(std::remove_if(side_.begin(), side_.end(), [&](const Grid& g) {
    for (std::size_t i = 1; i < needs.size(); ++i) if (rect_in(needs[i], interior(g))) return false;
    return true;
  }), side_.end());
  for (std::size_t i = 1; i < needs.size(); ++i) {
    const kern::Vec4& n = needs[i];
    if (covers(n) || side_.size() >= 3) continue;
    const f32 half = clampf(std::ceil((std::fmax(n.z - n.x, n.w - n.y) / 2 + kDemMargin + 400) / 50) * 50, 1500.0f, kDemMaxSize / 2);
    if (plan_dem({(n.x + n.z) / 2, (n.y + n.w) / 2}, half, true) || job_) return;
  }
}

bool Terrain::plan_dem(Vec2 centre, f32 half, bool side) {
  if (!dem_ || job_) return false;
  const f64 now = kern::now_ms();
  if (now < retry_at_) return false;
  const f32 cx = std::round(centre.x / 50) * 50, cy = std::round(centre.y / 50) * 50;
  f64 lon_a, lat_a, lon_b, lat_b;
  unproject(proj_, cx - half, cy + half, lon_a, lat_a);
  unproject(proj_, cx + half, cy - half, lon_b, lat_b);
  const i32 ix0 = static_cast<i32>(std::floor(merc_x(lon_a, kDemZoom))), iy0 = static_cast<i32>(std::floor(merc_y(lat_a, kDemZoom)));
  const i32 ix1 = static_cast<i32>(std::floor(merc_x(lon_b, kDemZoom))), iy1 = static_cast<i32>(std::floor(merc_y(lat_b, kDemZoom)));
  if ((ix1 - ix0 + 1) * (iy1 - iy0 + 1) > 100) return false;
  bool all = true, failed = false;
  for (i32 y = iy0; y <= iy1; ++y)
    for (i32 x = ix0; x <= ix1; ++x) {
      const TileKey k{kDemZoom, x, y};
      DemTile& t = dem_tiles_[k.hash()];
      if (!t.requested) { t.requested = true; t.key = k; requests_.push_back(k); }
      if (t.failed) failed = true;
      if (t.h.empty()) all = false;
    }
  if (failed) {
    // Never invent heights: without its first elevation the world stays flat for good (the
    // buildings already stand on it); a failed reload keeps the last surface and is asked again
    // (what lies outside the known ground waits for it: covers).
    ++dem_failures_;
    if (!grid_ready_) { disable_dem(); return false; }
    for (auto& [k, t] : dem_tiles_) if (t.failed) { t.failed = false; t.requested = false; }
    retry_at_ = now + std::min(15000.0, 1500.0 * dem_failures_);
    return false;
  }
  if (!all) return false;
  dem_failures_ = 0;
  // Drop tiles far from this window (memory: a z13 tile is 256 KB of floats).
  if (!side) for (auto it = dem_tiles_.begin(); it != dem_tiles_.end();) {
    const i32 x = it->second.key.x, y = it->second.key.y;
    if (x < ix0 - 1 || x > ix1 + 1 || y < iy0 - 1 || y > iy1 + 1) it = dem_tiles_.erase(it);
    else ++it;
  }
  job_ = std::make_unique<Job>();
  job_->side = side;
  job_->x0 = ix0; job_->y0 = iy0; job_->x1 = ix1; job_->y1 = iy1;
  Grid& g = job_->g;
  g.step = kDemStep;
  g.n = static_cast<u32>(std::lround(half * 2 / kDemStep)) + 1;
  g.x0 = cx - half;
  g.y0 = cy - half;
  g.cx = cx;
  g.cy = cy;
  g.h.resize(static_cast<std::size_t>(g.n) * g.n);
  return true;
}

bool Terrain::run_job(f64 deadline) {
  (void)deadline;
  Job& j = *job_;
  Grid& g = j.g;
  if (!j.id) {
    // Sampling the window (641k points) and the bare-earth filter run off the frame (a Web
    // Worker / thread), like MapLibre's raster-dem workers do it for the original.
    std::vector<u8> in;
    auto put = [&](const void* p, std::size_t n) { const std::size_t at = in.size(); in.resize(at + n); std::memcpy(in.data() + at, p, n); };
    auto put32 = [&](u32 v) { put(&v, 4); };
    auto putf = [&](f32 v) { put(&v, 4); };
    put(&proj_.lon0, 8); put(&proj_.lat0, 8);
    putf(g.x0); putf(g.y0); putf(g.step); put32(g.n); putf(kOpen); putf(kBlur);
    u32 count = 0;
    auto mine = [&](const DemTile& t) { return !t.h.empty() && t.key.x >= j.x0 && t.key.x <= j.x1 && t.key.y >= j.y0 && t.key.y <= j.y1; };
    for (const auto& [k, t] : dem_tiles_) count += mine(t) ? 1 : 0;
    put32(count);
    for (const auto& [k, t] : dem_tiles_) {
      if (!mine(t)) continue;
      put32(static_cast<u32>(t.key.x)); put32(static_cast<u32>(t.key.y)); put32(t.w); put32(t.hgt);
      put(t.h.data(), t.h.size() * 4);
    }
    j.id = kern::jobs::submit(kDemJob, std::move(in));
    return false;
  }
  std::vector<u8> out;
  if (!kern::jobs::take(j.id, out)) return false;
  j.id = 0;
  if (out.size() != static_cast<std::size_t>(g.n) * g.n * 4) { job_.reset(); retry_at_ = kern::now_ms() + 1500; return true; }
  std::memcpy(g.h.data(), out.data(), out.size());
  if (j.side) {
    // a far mate's window: kept beside the player's; the ground there is built again from it
    const f32 span = g.step * static_cast<f32>(g.n - 1);
    const f32 x0 = g.x0, y0 = g.y0;
    side_.push_back(std::move(g));
    job_.reset();
    dirty(x0, y0, x0 + span, y0 + span);
    return true;
  }
  // Commit. The reference (z = 0) is the bare earth at the world origin, fixed for this origin.
  if (!ref_ready_) {
    ref_height_ = grid_at(g, 0, 0);
    ref_ready_ = true;
  }
  const bool reload = grid_ready_;
  const kern::Vec4 known = reload ? interior(grid_) : kern::Vec4{0, 0, 0, 0};
  grid_ = std::move(g);
  grid_ready_ = true;
  settled_ = true;
  job_.reset();
  f32 lo = 1e9f, hi = -1e9f;
  for (f32 v : grid_.h) { lo = std::fmin(lo, v - ref_height_); hi = std::fmax(hi, v - ref_height_); }
  stats_.min_height = lo;
  stats_.max_height = hi;
  ++stats_.windows;
  if (reload) {
    // Inside the old window's interior the ground is the same as before (see plan_windows):
    // only what lay outside it is built again.
    const f32 big = 1e9f;
    dirty(-big, -big, known.x, big);
    dirty(known.z, -big, big, big);
    dirty(known.x, -big, known.z, known.y);
    dirty(known.x, known.w, known.z, big);
  }
  return true;
}

f32 Terrain::grid_at(const Grid& g, f32 x, f32 y) const {
  if (g.n < 2) return 0;
  const f32 lim = static_cast<f32>(g.n) - 1.001f;
  const f32 fx = clampf((x - g.x0) / g.step, 0, lim), fy = clampf((y - g.y0) / g.step, 0, lim);
  const u32 i = static_cast<u32>(fx), j = static_cast<u32>(fy);
  const f32 tx = fx - i, ty = fy - j;
  const std::size_t at = static_cast<std::size_t>(j) * g.n + i;
  return (g.h[at] * (1 - tx) + g.h[at + 1] * tx) * (1 - ty) + (g.h[at + g.n] * (1 - tx) + g.h[at + g.n + 1] * tx) * ty;
}

// ---------------------------------------------------------------------------------------------
// heights

f32 Terrain::elevation(f32 x, f32 y) const {
  if (!grid_ready_) return 0.0f;
  // the window that knows the place (its interior); else any that holds it; else the player's,
  // held at its edge (far ground nobody stands on: the horizon)
  auto in = [&](const kern::Vec4& r) { return x >= r.x && y >= r.y && x <= r.z && y <= r.w; };
  if (side_.empty() || in(interior(grid_))) return grid_at(grid_, x, y) - ref_height_;
  for (const Grid& g : side_) if (in(interior(g))) return grid_at(g, x, y) - ref_height_;
  if (in(interior_of(grid_.x0, grid_.y0, grid_.step * static_cast<f32>(grid_.n - 1), 0))) return grid_at(grid_, x, y) - ref_height_;
  for (const Grid& g : side_) if (in(interior_of(g.x0, g.y0, g.step * static_cast<f32>(g.n - 1), 0))) return grid_at(g, x, y) - ref_height_;
  return grid_at(grid_, x, y) - ref_height_;
}

// ---- road corridors -----------------------------------------------------------------------------
namespace {
constexpr f32 kRoadGrid = 24, kRoadBlend = 4.5f;  // terrain_craters.mjs RG, ROAD_BLEND_M
u64 road_key(i32 ix, i32 iy) { return (static_cast<u64>(static_cast<u32>(ix)) << 32) | static_cast<u32>(iy); }
}  // namespace

f32 Terrain::road_field(f32 x, f32 y) const {
  // terrain_craters.mjs roadField: the ground smoothed over ~36 m (tent kernel) - a street's
  // profile along its length; one field for all streets, so crossing streets agree at a junction
  f32 sum = 0, wsum = 0;
  for (int j = -2; j <= 2; ++j)
    for (int i = -2; i <= 2; ++i) {
      const f32 w = static_cast<f32>((3 - std::abs(i)) * (3 - std::abs(j)));
      sum += elevation(x + static_cast<f32>(i) * 9, y + static_cast<f32>(j) * 9) * w;
      wsum += w;
    }
  return sum / wsum;
}

void Terrain::set_corridors(u64 owner, const kern::Vec4& bounds, const std::vector<RoadLine>& roads) {
  remove_corridors(owner);
  auto set = std::make_unique<CorridorSet>();
  set->owner = owner;
  set->bounds = bounds;
  set->box = {1e30f, 1e30f, -1e30f, -1e30f};
  std::vector<Vec2> pts;
  std::vector<f32> hs;
  for (const RoadLine& r : roads) {
    if (!r.points || r.points->size() < 2 || !(r.width > 0)) continue;
    // the centre line every 5 m, its heights from the smooth field
    pts.clear();
    for (std::size_t i = 0; i + 1 < r.points->size(); ++i) {
      const Vec2 a = (*r.points)[i], b = (*r.points)[i + 1];
      const i32 k = std::max(1, static_cast<i32>(std::ceil(std::hypot(b.x - a.x, b.y - a.y) / 5)));
      for (i32 q = 0; q < k; ++q) pts.push_back(a + (b - a) * (static_cast<f32>(q) / static_cast<f32>(k)));
    }
    pts.push_back(r.points->back());
    hs.resize(pts.size());
    const f32 half = r.width / 2, reach = half + kRoadBlend;
    // (only what can reach into the tile's own square)
    auto near = [&](Vec2 p) { return p.x > bounds.x - reach - 6 && p.x < bounds.z + reach + 6 && p.y > bounds.y - reach - 6 && p.y < bounds.w + reach + 6; };
    for (std::size_t i = 0; i < pts.size(); ++i) hs[i] = near(pts[i]) ? road_field(pts[i].x, pts[i].y) : 0;
    for (std::size_t i = 0; i + 1 < pts.size(); ++i) {
      const Vec2 a = pts[i], b = pts[i + 1];
      if (!near(a) || !near(b)) continue;
      const f32 dx = b.x - a.x, dy = b.y - a.y, l2 = dx * dx + dy * dy;
      if (l2 < 1e-4f) continue;
      const CorridorSeg seg{a.x, a.y, dx, dy, l2, hs[i], hs[i + 1], half};
      const f32 x0 = std::fmin(a.x, b.x) - reach, x1 = std::fmax(a.x, b.x) + reach, y0 = std::fmin(a.y, b.y) - reach, y1 = std::fmax(a.y, b.y) + reach;
      set->box = {std::fmin(set->box.x, x0), std::fmin(set->box.y, y0), std::fmax(set->box.z, x1), std::fmax(set->box.w, y1)};
      for (i32 gx = static_cast<i32>(std::floor(x0 / kRoadGrid)); gx <= static_cast<i32>(std::floor(x1 / kRoadGrid)); ++gx)
        for (i32 gy = static_cast<i32>(std::floor(y0 / kRoadGrid)); gy <= static_cast<i32>(std::floor(y1 / kRoadGrid)); ++gy) set->grid[road_key(gx, gy)].push_back(seg);
    }
  }
  const kern::Vec4 box = set->box;
  corridor_hit_ = -1;
  corridors_.push_back(std::move(set));
  // the ground of the tile's square changed where its streets run (nobody else's ground did)
  if (box.z > box.x) {
    const kern::Vec4 c{std::fmax(box.x, bounds.x), std::fmax(box.y, bounds.y), std::fmin(box.z, bounds.z), std::fmin(box.w, bounds.w)};
    if (c.z > c.x && c.w > c.y) dirty(c.x, c.y, c.z, c.w, false);
  }
}

void Terrain::remove_corridors(u64 owner) {
  for (auto it = corridors_.begin(); it != corridors_.end(); ++it) {
    if ((*it)->owner != owner) continue;
    const kern::Vec4 box = (*it)->box, bounds = (*it)->bounds;
    corridor_hit_ = -1;
    corridors_.erase(it);
    if (box.z > box.x) {
      const kern::Vec4 c{std::fmax(box.x, bounds.x), std::fmax(box.y, bounds.y), std::fmin(box.z, bounds.z), std::fmin(box.w, bounds.w)};
      if (c.z > c.x && c.w > c.y) dirty(c.x, c.y, c.z, c.w, false);
    }
    return;
  }
}

i32 Terrain::corridor_set(f32 x, f32 y) const {
  // the map tile whose square holds the point (the squares tile the plane: west / south edge in)
  auto in = [&](const CorridorSet& s) { return x >= s.bounds.x && x < s.bounds.z && y >= s.bounds.y && y < s.bounds.w; };
  if (corridor_hit_ >= 0 && in(*corridors_[static_cast<std::size_t>(corridor_hit_)])) return corridor_hit_;
  for (std::size_t i = 0; i < corridors_.size(); ++i)
    if (in(*corridors_[i])) {
      corridor_hit_ = static_cast<i32>(i);
      return corridor_hit_;
    }
  return -1;
}

bool Terrain::corridors_at(f32 x, f32 y) const { return !corridors_expected || corridor_set(x, y) >= 0; }

f32 Terrain::road_level(f32 x, f32 y, f32 h) const {
  // terrain_craters.mjs roadLevel: every corridor covering the point contributes (weighted), so
  // junctions blend smoothly, no steps
  const i32 at = corridors_.empty() ? -1 : corridor_set(x, y);
  if (at < 0) return h;
  const CorridorSet& set = *corridors_[static_cast<std::size_t>(at)];
  const auto it = set.grid.find(road_key(static_cast<i32>(std::floor(x / kRoadGrid)), static_cast<i32>(std::floor(y / kRoadGrid))));
  if (it == set.grid.end()) return h;
  f32 best = 0, sw = 0, sh = 0;
  for (const CorridorSeg& s : it->second) {
    const f32 t = clampf(((x - s.ax) * s.dx + (y - s.ay) * s.dy) / s.l2, 0, 1);
    const f32 px = s.ax + s.dx * t, py = s.ay + s.dy * t, d = std::hypot(x - px, y - py);
    const f32 w = d <= s.half ? 1 : d >= s.half + kRoadBlend ? 0 : 1 - (d - s.half) / kRoadBlend;
    if (w <= 0) continue;
    const f32 ws = w * w * (3 - 2 * w);
    sw += ws;
    sh += ws * (s.ha + (s.hb - s.ha) * t);
    best = std::fmax(best, w);
  }
  if (sw <= 0) return h;
  return h + (sh / sw - h) * (best * best * (3 - 2 * best));
}

f32 Terrain::node_height(f32 x, f32 y) const {
  const f32 e = elevation(x, y);
  f32 h = road_level(x, y, e);
  for (const Crater& c : craters_) {
    const f32 dx = x - c.c.x, dy = y - c.c.y;
    if (std::fabs(dx) >= c.r || std::fabs(dy) >= c.r) continue;
    const f32 r = std::sqrt(dx * dx + dy * dy);
    if (r < c.r) h += crater_profile(r, c.r, c.depth, c.rim);
  }
  if (!water_.empty() && water_at(x, y)) return std::fmin(h, e - kWaterBed);
  return h;
}

f32 Terrain::height(f32 x, f32 y) const {
  // terrain_surface.mjs triangleHeightAt: the SW-NE diagonal of every 5 m cell.
  const f32 x0 = std::floor(x / kStep) * kStep, y0 = std::floor(y / kStep) * kStep;
  const f32 u = (x - x0) / kStep, v = (y - y0) / kStep;
  const f32 a = node_height(x0, y0), d = node_height(x0 + kStep, y0 + kStep);
  return u >= v ? a + (node_height(x0 + kStep, y0) - a) * (u - v) + (d - a) * v : a + (node_height(x0, y0 + kStep) - a) * (v - u) + (d - a) * u;
}

Vec3 Terrain::normal(f32 x, f32 y) const {
  const f32 e = 0.5f, hx = height(x + e, y) - height(x - e, y), hy = height(x, y + e) - height(x, y - e);
  return kern::normalize(Vec3{-hx, -hy, 2 * e});
}

f32 Terrain::footprint_base(const std::vector<Vec2>& ring) const {
  if (!grid_ready_ && craters_.empty()) return 0;
  f32 m = 1e9f;
  for (std::size_t i = 0; i < ring.size(); ++i) {
    const Vec2 a = ring[i], b = ring[(i + 1) % ring.size()];
    // (the natural ground, or the street's corridor where it cuts lower: never floating)
    const Vec2 c = (a + b) * 0.5f;
    const f32 ea = elevation(a.x, a.y), ec = elevation(c.x, c.y);
    m = std::fmin(m, std::fmin(std::fmin(ea, road_level(a.x, a.y, ea)), std::fmin(ec, road_level(c.x, c.y, ec))));
  }
  return m < 1e8f ? m : 0.0f;
}

kern::HeightSampler Terrain::sampler(f32 offset) const {
  (void)offset;
  kern::HeightSampler s;
  s.user = const_cast<Terrain*>(this);
  s.fn = [](void* u, f64 x, f64 y) -> f64 { return static_cast<const Terrain*>(u)->height(static_cast<f32>(x), static_cast<f32>(y)); };
  return s;
}

// ---------------------------------------------------------------------------------------------
// water

bool Terrain::water_cell(i32 cx, i32 cy) const {
  // The hottest lookup of the city's streets (every draped road point asks it, ?perf=1 / the CPU
  // profile): consecutive points almost always fall in the same 64 m block, so the last block is
  // remembered - its 256 bits (set_water_cell / clear forget it).
  const u64 k = key2(cx >> 4, cy >> 4);
  if (k != wc_key_) {
    auto it = water_.find(k);
    wc_key_ = k;
    wc_has_ = it != water_.end();
    if (wc_has_)
      for (std::size_t w = 0; w < 4; ++w) wc_bits_[w] = it->second.bits[w];
  }
  if (!wc_has_) return false;
  const u32 idx = static_cast<u32>((cy & 15) * 16 + (cx & 15));
  return (wc_bits_[idx >> 6] >> (idx & 63)) & 1u;
}

void Terrain::set_water_cell(i32 cx, i32 cy) {
  wc_key_ = ~0ull;  // the remembered block (water_cell) may be new now
  WaterBlock& b = water_[key2(cx >> 4, cy >> 4)];
  const u32 idx = static_cast<u32>((cy & 15) * 16 + (cx & 15));
  if (!((b.bits[idx >> 6] >> (idx & 63)) & 1u)) ++stats_.water_cells;
  b.bits[idx >> 6] |= 1ull << (idx & 63);
}

bool Terrain::on_bridge(f32 x, f32 y) const {
  for (const Bridge& b : bridges_) {
    const f32 dx = x - b.c.x, dy = y - b.c.y;
    if (std::fabs(dx) > b.hl + b.hw || std::fabs(dy) > b.hl + b.hw) continue;
    const f32 c = std::cos(b.yaw), s = std::sin(b.yaw), u = dx * c + dy * s, v = -dx * s + dy * c;
    if (std::fabs(u) <= b.hl && std::fabs(v) <= b.hw) return true;
  }
  return false;
}

bool Terrain::water_at(f32 x, f32 y) const {
  if (water_.empty()) return false;
  if (!water_cell(static_cast<i32>(std::floor(x / kWaterCell)), static_cast<i32>(std::floor(y / kWaterCell)))) return false;
  return bridges_.empty() || !on_bridge(x, y);
}

Vec2 Terrain::flow_at(f32 x, f32 y) const {
  const i32 cx = static_cast<i32>(std::floor(x / kWaterCell)), cy = static_cast<i32>(std::floor(y / kWaterCell));
  auto it = water_.find(key2(cx >> 4, cy >> 4));
  if (it == water_.end() || it->second.flow.empty() || !water_at(x, y)) return {};
  const u32 idx = static_cast<u32>((cy & 15) * 16 + (cx & 15));
  return it->second.flow[idx] * it->second.speed[idx];
}

i32 Terrain::surface_at(f32 x, f32 y) const {
  if (water_at(x, y)) return 1;
  for (const Crater& c : craters_)
    if (std::hypot(x - c.c.x, y - c.c.y) < c.r * 0.8f) return 2;
  return 0;
}

void Terrain::add_water_polygon(const std::vector<std::vector<Vec2>>& rings) {
  if (rings.empty() || rings[0].size() < 3) return;
  f32 y_lo = 1e9f, y_hi = -1e9f, x_lo = 1e9f, x_hi = -1e9f;
  for (Vec2 p : rings[0]) { y_lo = std::fmin(y_lo, p.y); y_hi = std::fmax(y_hi, p.y); x_lo = std::fmin(x_lo, p.x); x_hi = std::fmax(x_hi, p.x); }
  // Scanline fill (even-odd over all rings: courtyards / islands stay dry), cell centres.
  std::vector<f32> xs;
  const i32 j0 = static_cast<i32>(std::floor(y_lo / kWaterCell)), j1 = static_cast<i32>(std::floor(y_hi / kWaterCell));
  for (i32 j = j0; j <= j1; ++j) {
    const f32 y = (static_cast<f32>(j) + 0.5f) * kWaterCell;
    xs.clear();
    for (const auto& ring : rings)
      for (std::size_t a = 0, c = ring.size() - 1; a < ring.size(); c = a++) {
        const f32 ya = ring[a].y, yc = ring[c].y;
        if ((ya > y) != (yc > y)) xs.push_back(ring[a].x + (y - ya) / (yc - ya) * (ring[c].x - ring[a].x));
      }
    std::sort(xs.begin(), xs.end());
    for (std::size_t k = 0; k + 1 < xs.size(); k += 2) {
      const i32 ia = static_cast<i32>(std::ceil(xs[k] / kWaterCell - 0.5f)), ib = static_cast<i32>(std::floor(xs[k + 1] / kWaterCell - 0.5f));
      for (i32 i = ia; i <= ib; ++i) set_water_cell(i, j);
    }
  }
  dirty(x_lo, y_lo, x_hi, y_hi);
}

void Terrain::add_waterway(const std::vector<Vec2>& pts, f32 width, f32 speed) {
  f32 x_lo = 1e9f, y_lo = 1e9f, x_hi = -1e9f, y_hi = -1e9f;
  for (std::size_t k = 0; k + 1 < pts.size(); ++k) {
    const Vec2 a = pts[k], c = pts[k + 1];
    const f32 l = std::hypot(c.x - a.x, c.y - a.y);
    if (l < 0.2f) continue;
    const Vec2 u{(c.x - a.x) / l, (c.y - a.y) / l};
    const f32 half = width / 2, reach = half + 30;  // world_water.mjs: flow felt 30 m beyond the banks
    const i32 i0 = static_cast<i32>(std::floor((std::fmin(a.x, c.x) - reach) / kWaterCell)), i1 = static_cast<i32>(std::floor((std::fmax(a.x, c.x) + reach) / kWaterCell));
    const i32 j0 = static_cast<i32>(std::floor((std::fmin(a.y, c.y) - reach) / kWaterCell)), j1 = static_cast<i32>(std::floor((std::fmax(a.y, c.y) + reach) / kWaterCell));
    x_lo = std::fmin(x_lo, std::fmin(a.x, c.x) - half); x_hi = std::fmax(x_hi, std::fmax(a.x, c.x) + half);
    y_lo = std::fmin(y_lo, std::fmin(a.y, c.y) - half); y_hi = std::fmax(y_hi, std::fmax(a.y, c.y) + half);
    for (i32 i = i0; i <= i1; ++i)
      for (i32 j = j0; j <= j1; ++j) {
        const f32 x = (static_cast<f32>(i) + 0.5f) * kWaterCell, y = (static_cast<f32>(j) + 0.5f) * kWaterCell;
        const f32 t = clampf((x - a.x) * u.x + (y - a.y) * u.y, 0, l);
        const f32 d = std::hypot(x - a.x - u.x * t, y - a.y - u.y * t);
        if (d <= half) set_water_cell(i, j);
        if (d < reach) {
          auto it = water_.find(key2(i >> 4, j >> 4));
          if (it == water_.end()) continue;
          WaterBlock& b = it->second;
          if (b.flow.empty()) { b.flow.assign(256, Vec2{}); b.speed.assign(256, 0.0f); }
          const u32 idx = static_cast<u32>((j & 15) * 16 + (i & 15));
          const f32 sp = speed * (1 - d / reach);
          if (b.speed[idx] < sp) { b.flow[idx] = u; b.speed[idx] = sp; }
        }
      }
  }
  if (x_lo < x_hi) dirty(x_lo, y_lo, x_hi, y_hi);
}

void Terrain::add_bridge(Vec2 a, Vec2 b, f32 width) {
  const f32 l = std::hypot(b.x - a.x, b.y - a.y);
  if (l < 0.5f) return;
  // world_water.mjs: deck = the segment +1 m long, +1.5 m each side
  bridges_.push_back({(a + b) * 0.5f, l / 2 + 1, width / 2 + 1.5f, std::atan2(b.y - a.y, b.x - a.x)});
  const f32 r = l / 2 + width;
  dirty((a.x + b.x) / 2 - r, (a.y + b.y) / 2 - r, (a.x + b.x) / 2 + r, (a.y + b.y) / 2 + r);
}

// ---------------------------------------------------------------------------------------------
// ground colour (world_ground.mjs)

namespace {
// Satellite RGB -> albedo (world_ground.mjs albedo): lift shadows, remove haze, plausible ranges
Vec3 albedo(Vec3 c) {
  const f32 l = 0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z;
  const f32 target = std::fmin(0.24f, std::fmax(0.07f, l * 0.8f + 0.03f)), k = target / std::fmax(0.02f, l);
  f32 R = c.x * k, G = c.y * k, B = c.z * k;
  const f32 m = (R + G + B) / 3, sat = std::fmax(R, std::fmax(G, B)) - std::fmin(R, std::fmin(G, B));
  if (sat < 0.05f) { R = R * 0.85f + m * 0.15f; G = G * 0.85f + m * 0.15f; B = B * 0.85f + m * 0.15f; }
  if (B > G && B > R && sat > 0.04f) B *= 0.85f;  // haze / water-ish blue cast -> calmer
  return {std::fmin(0.45f, R), std::fmin(0.45f, G), std::fmin(0.45f, B)};
}
f32 srgb_lin8(u8 v) { const f32 x = v / 255.0f; return x <= 0.04045f ? x / 12.92f : std::pow((x + 0.055f) / 1.055f, 2.4f); }
}  // namespace

std::string Terrain::take_image_requests() {
  std::string s = "[";
  for (std::size_t i = 0; i < image_requests_.size(); ++i) {
    char b[64];
    std::snprintf(b, sizeof b, "%s[%d,%d,%d]", i ? "," : "", image_requests_[i].z, image_requests_[i].x, image_requests_[i].y);
    s += b;
  }
  image_requests_.clear();
  return s + "]";
}

kern::Vec4 Terrain::image_bounds(const TileKey& k) const {
  f64 lon0, lat0, lon1, lat1;
  tile_lonlat(k, 0, 0, lon0, lat0);
  tile_lonlat(k, 1, 1, lon1, lat1);
  f64 ax, ay, bx, by;
  proj_.project(lon0, lat0, ax, ay);
  proj_.project(lon1, lat1, bx, by);
  return {static_cast<f32>(std::fmin(ax, bx)), static_cast<f32>(std::fmin(ay, by)), static_cast<f32>(std::fmax(ax, bx)), static_cast<f32>(std::fmax(ay, by))};
}

void Terrain::plan_images(Vec3 focus) {
  // SIZE_M 1600 around the focus (10 m grid of it), REBUILD_MOVE_M 350
  if (std::hypot(focus.x - image_at_.x, focus.y - image_at_.y) < 350) return;
  image_at_ = {std::round(focus.x / 10) * 10, std::round(focus.y / 10) * 10};
  constexpr f32 half = 800;
  f64 lon_a, lat_a, lon_b, lat_b;
  unproject(proj_, image_at_.x - half, image_at_.y + half, lon_a, lat_a);
  unproject(proj_, image_at_.x + half, image_at_.y - half, lon_b, lat_b);
  const i32 ix0 = static_cast<i32>(std::floor(merc_x(lon_a, kImageZoom))), iy0 = static_cast<i32>(std::floor(merc_y(lat_a, kImageZoom)));
  const i32 ix1 = static_cast<i32>(std::floor(merc_x(lon_b, kImageZoom))), iy1 = static_cast<i32>(std::floor(merc_y(lat_b, kImageZoom)));
  if ((ix1 - ix0 + 1) * (iy1 - iy0 + 1) > 25) return;
  for (i32 y = iy0; y <= iy1; ++y)
    for (i32 x = ix0; x <= ix1; ++x) {
      const TileKey k{kImageZoom, x, y};
      Image& im = images_[k.hash()];
      if (!im.requested) { im.requested = true; im.key = k; image_requests_.push_back(k); }
    }
  // the cache keeps what is near (tileCache: 64 tiles)
  for (auto it = images_.begin(); it != images_.end();) {
    const TileKey& k = it->second.key;
    if (k.x < ix0 - 2 || k.x > ix1 + 2 || k.y < iy0 - 2 || k.y > iy1 + 2) { if (!it->second.albedo.empty()) repaint(image_bounds(k)); it = images_.erase(it); }
    else ++it;
  }
}

void Terrain::provide_image(const TileKey& key, kern::Span<const u8> rgba, u32 width, u32 height) {
  auto it = images_.find(key.hash());
  if (it == images_.end() || width == 0 || height == 0) return;
  // the mosaic: 64 px per tile (an area average in the image's own sRGB, as the canvas draws it)
  constexpr u32 P = kImagePixels;
  std::vector<u8> small(P * P * 4);
  for (u32 j = 0; j < P; ++j)
    for (u32 i = 0; i < P; ++i) {
      const u32 x0 = i * width / P, x1 = std::max(x0 + 1, (i + 1) * width / P), y0 = j * height / P, y1 = std::max(y0 + 1, (j + 1) * height / P);
      u32 sum[4] = {}, n = 0;
      for (u32 y = y0; y < y1; ++y)
        for (u32 x = x0; x < x1; ++x) { const std::size_t at = (static_cast<std::size_t>(y) * width + x) * 4; for (int c = 0; c < 4; ++c) sum[c] += rgba[at + c]; ++n; }
      for (int c = 0; c < 4; ++c) small[(j * P + i) * 4 + c] = static_cast<u8>(sum[c] / std::max(1u, n));
    }
  // per pixel: the 3 x 3 around it in linear light (transparent ones skipped), then the albedo
  Image& im = it->second;
  im.albedo.assign(P * P * 3, 0);
  for (u32 j = 0; j < P; ++j)
    for (u32 i = 0; i < P; ++i) {
      Vec3 s{};
      u32 n = 0;
      for (i32 dy = -1; dy <= 1; ++dy)
        for (i32 dx = -1; dx <= 1; ++dx) {
          const u32 qx = static_cast<u32>(std::clamp<i32>(static_cast<i32>(i) + dx, 0, P - 1)), qy = static_cast<u32>(std::clamp<i32>(static_cast<i32>(j) + dy, 0, P - 1));
          const std::size_t at = (qy * P + qx) * 4;
          if (small[at + 3] < 10) continue;
          s = s + Vec3{srgb_lin8(small[at]), srgb_lin8(small[at + 1]), srgb_lin8(small[at + 2])};
          ++n;
        }
      const Vec3 a = albedo(n ? s * (1.0f / static_cast<f32>(n)) : Vec3{0.12f, 0.16f, 0.08f});
      im.albedo[(j * P + i) * 3] = a.x;
      im.albedo[(j * P + i) * 3 + 1] = a.y;
      im.albedo[(j * P + i) * 3 + 2] = a.z;
    }
  repaint(image_bounds(key));
}

void Terrain::fail_image(const TileKey& key) {
  auto it = images_.find(key.hash());
  if (it != images_.end()) it->second.failed = true;  // the ground keeps its fallback colour
}

bool Terrain::ground_albedo(f32 x, f32 y, Vec3& lin) const {
  if (images_.empty()) return false;
  f64 lon, lat;
  unproject(proj_, x, y, lon, lat);
  const f64 fx = merc_x(lon, kImageZoom), fy = merc_y(lat, kImageZoom);
  const i32 tx = static_cast<i32>(std::floor(fx)), ty = static_cast<i32>(std::floor(fy));
  auto it = images_.find(TileKey{kImageZoom, tx, ty}.hash());
  if (it == images_.end() || it->second.albedo.empty()) return false;
  const i32 px = std::clamp(static_cast<i32>((fx - tx) * kImagePixels), 0, kImagePixels - 1), py = std::clamp(static_cast<i32>((fy - ty) * kImagePixels), 0, kImagePixels - 1);
  const std::vector<f32>& texels = it->second.albedo;
  const std::size_t at = (static_cast<std::size_t>(py) * kImagePixels + px) * 3;
  lin = {texels[at], texels[at + 1], texels[at + 2]};
  return true;
}

// ---------------------------------------------------------------------------------------------
// map areas

namespace {
f32 srgb_to_linear(f32 v) { return v <= 0.04045f ? v / 12.92f : std::pow((v + 0.055f) / 1.055f, 2.4f); }
f32 linear_to_srgb(f32 v) { return v <= 0.0031308f ? v * 12.92f : 1.055f * std::pow(v, 1 / 2.4f) - 0.055f; }
Vec3 to_linear(Vec3 c) { return {srgb_to_linear(c.x), srgb_to_linear(c.y), srgb_to_linear(c.z)}; }
Vec3 to_srgb(Vec3 c) { return {linear_to_srgb(c.x), linear_to_srgb(c.y), linear_to_srgb(c.z)}; }
}  // namespace

void Terrain::add_tint(u64 owner, const std::vector<std::vector<Vec2>>& rings, Vec3 srgb) {
  if (rings.empty() || rings[0].size() < 3) return;
  Tint t{owner, {1e30f, 1e30f, -1e30f, -1e30f}, to_linear(srgb), rings};
  for (Vec2 p : rings[0]) {
    t.box.x = std::fmin(t.box.x, p.x); t.box.y = std::fmin(t.box.y, p.y);
    t.box.z = std::fmax(t.box.z, p.x); t.box.w = std::fmax(t.box.w, p.y);
  }
  repaint(t.box);
  tints_.push_back(std::move(t));
}

void Terrain::remove_tints(u64 owner) {
  for (auto it = tints_.begin(); it != tints_.end();) {
    if (it->owner != owner) { ++it; continue; }
    repaint(it->box);
    it = tints_.erase(it);
  }
}

void Terrain::repaint(const kern::Vec4& b) {
  // only the render tiles: the shape (collision, roads on it) does not change
  for (auto& [k, t] : render_) {
    (void)t;
    i32 level, i, j;
    unkey3(k, level, i, j);
    const f32 size = level == 2 ? kBlock : kTile;
    if (i * size <= b.z && (i + 1) * size >= b.x && j * size <= b.w && (j + 1) * size >= b.y) dirty_render_.insert(k);
  }
}

// ---------------------------------------------------------------------------------------------
// craters

void Terrain::add_crater(Vec2 at, f32 radius, f32 depth) {
  if (!(radius > 0.5f)) return;
  // Rim height in the same proportion as SpawnCity's (7 m on a 22 m deep bowl).
  craters_.push_back({at, radius, depth, depth * 7.0f / 22.0f});
  while (craters_.size() > 32) {
    const Crater old = craters_.front();
    craters_.erase(craters_.begin());
    dirty(old.c.x - old.r, old.c.y - old.r, old.c.x + old.r, old.c.y + old.r);
  }
  stats_.craters = static_cast<u32>(craters_.size());
  dirty(at.x - radius, at.y - radius, at.x + radius, at.y + radius);
}

void Terrain::crater_list(std::vector<CraterRec>& out) const {
  out.clear();
  for (const Crater& c : craters_) out.push_back({c.c.x, c.c.y, c.r, c.depth});
}

void Terrain::set_craters(const std::vector<CraterRec>& list) {
  std::vector<CraterRec> mine;
  crater_list(mine);
  if (mine == list) return;
  auto has = [](const std::vector<CraterRec>& v, const CraterRec& c) { return std::find(v.begin(), v.end(), c) != v.end(); };
  // the ground changes only where a crater came or went (a reorder alone changes nothing: the
  // height adds every bowl up)
  for (const CraterRec& c : mine) if (!has(list, c)) dirty(c.x - c.r, c.y - c.r, c.x + c.r, c.y + c.r);
  for (const CraterRec& c : list) if (!has(mine, c)) dirty(c.x - c.r, c.y - c.r, c.x + c.r, c.y + c.r);
  craters_.clear();
  for (const CraterRec& c : list) if (c.r > 0.5f && std::isfinite(c.x) && std::isfinite(c.y) && std::isfinite(c.depth)) craters_.push_back({{c.x, c.y}, c.r, c.depth, c.depth * 7.0f / 22.0f});
  stats_.craters = static_cast<u32>(craters_.size());
}

// ---------------------------------------------------------------------------------------------
// tiles

void Terrain::dirty(f32 x0, f32 y0, f32 x1, f32 y1, bool notify) {
  const f32 m = kStep * 2;
  x0 -= m; y0 -= m; x1 += m; y1 += m;
  for (auto& [k, t] : render_) {
    i32 level, i, j;
    unkey3(k, level, i, j);
    const f32 size = level == 2 ? kBlock : kTile;
    if (i * size <= x1 && (i + 1) * size >= x0 && j * size <= y1 && (j + 1) * size >= y0) dirty_render_.insert(k);
  }
  for (auto& [k, c] : collision_) {
    const i32 i = static_cast<i32>(k >> 32), j = static_cast<i32>(k & 0xffffffffu);
    if (i * kTile - kStep <= x1 && (i + 1) * kTile + kStep >= x0 && j * kTile - kStep <= y1 && (j + 1) * kTile + kStep >= y0) dirty_collision_.insert(k);
  }
  for (auto& [k, m2] : water_meshes_) {
    (void)m2;
    const i32 i = static_cast<i32>(k >> 32), j = static_cast<i32>(k & 0xffffffffu);
    if (i * kWaterChunk <= x1 && (i + 1) * kWaterChunk >= x0 && j * kWaterChunk <= y1 && (j + 1) * kWaterChunk >= y0) dirty_water_.insert(k);
  }
  if (notify) changes_.push_back({x0, y0, x1, y1});
  replan_ = true;
}

bool Terrain::ground_ready(f32 x, f32 y) const {
  // known ground (an interior: no later window changes it) with its collision standing
  if (!covers({x, y, x, y}) || !corridors_at(x, y)) return false;
  return has_collision(x, y);
}

bool Terrain::has_collision(f32 x, f32 y) const {
  const u64 k = key2(static_cast<i32>(std::floor(x / kTile)), static_cast<i32>(std::floor(y / kTile)));
  return collision_.count(k) && !dirty_collision_.count(k);
}

namespace {
/// A tile want_tiles asks for: its distance from where it is wanted and its key.
struct WantedTile { f32 d; u64 k; };
}  // namespace

void Terrain::want_tiles(Vec3 focus, Vec3 anchor) {
  std::vector<WantedTile> render, coll, water;
  // Render: 640 m blocks out to 3.2 km; blocks near the focus split into 160 m tiles (5 m nodes
  // near, 20 m further out).
  const i32 bi = static_cast<i32>(std::floor(focus.x / kBlock)), bj = static_cast<i32>(std::floor(focus.y / kBlock));
  for (i32 j = bj - 5; j <= bj + 5; ++j)
    for (i32 i = bi - 5; i <= bi + 5; ++i) {
      const f32 d = rect_distance(focus.x, focus.y, i * kBlock, j * kBlock, (i + 1) * kBlock, (j + 1) * kBlock);
      if (d > 3200) continue;
      if (d < 200) {
        for (i32 b = 0; b < 4; ++b)
          for (i32 a = 0; a < 4; ++a) {
            const i32 ti = i * 4 + a, tj = j * 4 + b;
            const f32 td = rect_distance(focus.x, focus.y, ti * kTile, tj * kTile, (ti + 1) * kTile, (tj + 1) * kTile);
            render.push_back({td, key3(td < 260 ? 0 : 1, ti, tj)});
          }
      } else {
        render.push_back({d, key3(2, i, j)});
      }
    }
  // Collision: 160 m tiles within 420 m of the focus (traffic lives there) and 200 m of the
  // anchor (the pilot on foot while his drone is away).
  std::vector<std::pair<Vec3, f32>> around{{focus, 420.0f}, {anchor, 200.0f}};
  for (const Vec3& m : extra) around.push_back({m, 300.0f});  // the mates (MP host): his traffic there
  for (const auto& [p, reach] : around) {
    const i32 ci = static_cast<i32>(std::floor(p.x / kTile)), cj = static_cast<i32>(std::floor(p.y / kTile));
    const i32 rr = static_cast<i32>(std::ceil(reach / kTile));
    for (i32 j = cj - rr; j <= cj + rr; ++j)
      for (i32 i = ci - rr; i <= ci + rr; ++i) {
        const f32 d = rect_distance(p.x, p.y, i * kTile, j * kTile, (i + 1) * kTile, (j + 1) * kTile);
        if (d <= reach) coll.push_back({d, key2(i, j)});
      }
  }
  // Water: 320 m chunks holding water within 2.4 km.
  std::unordered_set<u64> chunks;
  for (const auto& [k, b] : water_) {
    (void)b;
    const i32 bx = static_cast<i32>(k >> 32), by = static_cast<i32>(k & 0xffffffffu);
    const i32 ci = static_cast<i32>(std::floor(bx * kWaterBlock / kWaterChunk)), cj = static_cast<i32>(std::floor(by * kWaterBlock / kWaterChunk));
    chunks.insert(key2(ci, cj));
  }
  for (u64 k : chunks) {
    const i32 i = static_cast<i32>(k >> 32), j = static_cast<i32>(k & 0xffffffffu);
    const f32 d = rect_distance(focus.x, focus.y, i * kWaterChunk, j * kWaterChunk, (i + 1) * kWaterChunk, (j + 1) * kWaterChunk);
    if (d < 2400) water.push_back({d, k});
  }
  auto finish = [](std::vector<WantedTile>& v, std::vector<u64>& out) {
    std::sort(v.begin(), v.end(), [](const WantedTile& a, const WantedTile& b) { return a.d < b.d; });
    out.clear();
    for (const WantedTile& w : v)
      if (std::find(out.begin(), out.end(), w.k) == out.end()) out.push_back(w.k);
  };
  finish(render, want_render_);
  finish(coll, want_collision_);
  finish(water, want_water_);
  planned_focus_ = focus;
  planned_anchor_ = anchor;
  planned_extra_ = extra;
  replan_ = false;
}

void Terrain::build_render_tile(i32 level, i32 ti, i32 tj) {
  const f32 size = level == 2 ? kBlock : kTile;
  const i32 cells = level == 0 ? kCells : level == 1 ? 8 : 16;
  const f32 step = size / static_cast<f32>(cells), x0 = ti * size, y0 = tj * size;
  const i32 n = cells + 1;
  kern::MeshData m;
  std::vector<f32> h(static_cast<std::size_t>(n + 2) * (n + 2));
  // Coarse tiles sample the surface they stand for at their own nodes; a border ring for normals.
  for (i32 j = -1; j <= n; ++j)
    for (i32 i = -1; i <= n; ++i) h[static_cast<std::size_t>(j + 1) * (n + 2) + (i + 1)] = node_height(x0 + i * step, y0 + j * step);
  auto H = [&](i32 i, i32 j) { return h[static_cast<std::size_t>(j + 1) * (n + 2) + (i + 1)]; };
  // world_ground.mjs paintAreas: every node inside a map area (outer ring, not in a hole) takes
  // node * .45 + area * .55 (linear), area after area.
  // world_ground.mjs: the aerial albedo per node (the fallback grass/soil where none loaded), then
  // the map areas tint it
  std::vector<Vec3> tint(static_cast<std::size_t>(n) * n);
  for (i32 j = 0; j < n; ++j)
    for (i32 i = 0; i < n; ++i) {
      Vec3& c = tint[static_cast<std::size_t>(j) * n + i];
      if (!ground_albedo(x0 + i * step, y0 + j * step, c)) c = {0.16f, 0.2f, 0.1f};
    }
  {
    std::vector<f32> xs;
    const f32 x1 = x0 + size, y1 = y0 + size;
    for (const Tint& a : tints_) {
      if (a.box.x > x1 || a.box.z < x0 || a.box.y > y1 || a.box.w < y0) continue;
      for (i32 j = 0; j < n; ++j) {
        const f32 y = y0 + j * step;
        if (y < a.box.y || y > a.box.w) continue;
        xs.clear();
        for (const auto& ring : a.rings)
          for (std::size_t q = 0, r = ring.size() - 1; q < ring.size(); r = q++) {
            const Vec2 p = ring[q], o = ring[r];
            if ((p.y > y) != (o.y > y)) xs.push_back(p.x + (y - p.y) * (o.x - p.x) / (o.y - p.y));
          }
        std::sort(xs.begin(), xs.end());
        for (std::size_t q = 0; q + 1 < xs.size(); q += 2) {
          const i32 i0 = std::max(0, static_cast<i32>(std::ceil((xs[q] - x0) / step)));
          const i32 i1 = std::min(n - 1, static_cast<i32>(std::floor((xs[q + 1] - x0) / step)));
          for (i32 i = i0; i <= i1; ++i) {
            Vec3& c = tint[static_cast<std::size_t>(j) * n + i];
            c = c * 0.45f + a.lin * 0.55f;
          }
        }
      }
    }
  }
  for (i32 j = 0; j < n; ++j)
    for (i32 i = 0; i < n; ++i) {
      const f32 x = x0 + i * step, y = y0 + j * step, z = H(i, j);
      const f32 dx = H(i + 1, j) - H(i - 1, j), dy = H(i, j + 1) - H(i, j - 1);
      const Vec3 nn = kern::normalize(Vec3{-dx, -dy, 2 * step});
      Vec3 c = to_srgb(tint[static_cast<std::size_t>(j) * n + i]);
      if (z < elevation(x, y) - 0.6f) c = water_at(x, y) ? kBed : kSoil;
      else if (surface_at(x, y) == 2) c = kSoil;
      m.push({x, y, z}, nn, c, {x * 0.1f, y * 0.1f});
    }
  for (i32 j = 0; j < cells; ++j)
    for (i32 i = 0; i < cells; ++i) {
      const u32 a = static_cast<u32>(j * n + i), b = a + 1, c = a + static_cast<u32>(n), d = c + 1;
      m.indices.insert(m.indices.end(), {a, b, d, a, d, c});  // SW-NE diagonal, counter-clockwise from above
    }
  // Skirts: a curtain down from every edge hides the cracks between tiles of different detail.
  const f32 drop = level == 0 ? 2.0f : level == 1 ? 4.0f : 10.0f;
  auto skirt = [&](i32 i0, i32 j0, i32 di, i32 dj, bool flip) {
    for (i32 k = 0; k < cells; ++k) {
      const i32 ia = i0 + di * k, ja = j0 + dj * k, ib = ia + di, jb = ja + dj;
      const u32 top_a = static_cast<u32>(ja * n + ia), top_b = static_cast<u32>(jb * n + ib);
      const u32 base = m.vertex_count();
      for (u32 v : {top_a, top_b}) {
        const Vec3 p{m.positions[v * 3], m.positions[v * 3 + 1], m.positions[v * 3 + 2] - drop};
        m.push(p, {0, 0, 1}, kGround, {p.x * 0.1f, p.y * 0.1f});
      }
      if (flip) m.indices.insert(m.indices.end(), {top_a, base, top_b, top_b, base, base + 1});
      else m.indices.insert(m.indices.end(), {top_a, top_b, base, top_b, base + 1, base});
    }
  };
  skirt(0, 0, 1, 0, false);
  skirt(0, n - 1, 1, 0, true);
  skirt(0, 0, 0, 1, true);
  skirt(n - 1, 0, 0, 1, false);
  kern::MeshInput in = m.as_input();
  in.material.roughness = 1;
  in.material.cast_shadow = false;
  in.material.double_sided = true;
  const u32 id = r_->allocate_id();
  if (!r_->upsert(id, in)) { r_->release_id(id); return; }
  RenderTile& t = render_[key3(level, ti, tj)];
  if (t.mesh) r_->remove(t.mesh);
  t.mesh = id;
}

void Terrain::build_collision_tile(i32 ti, i32 tj) {
  // terrain_tiles.mjs: 32 cells of 5 m plus one cell into each neighbour with the outer ring sunk,
  // so the seam is an interior edge of both tiles (no ghost contacts on fast wheels).
  const i32 N = kCells + 3;
  const f32 x0 = ti * kTile - kStep, y0 = tj * kTile - kStep;
  std::vector<Vec3> pts(static_cast<std::size_t>(N) * N);
  for (i32 r = 0; r < N; ++r)
    for (i32 c = 0; c < N; ++c) {
      const f32 x = x0 + c * kStep, y = y0 + r * kStep;
      f32 z = node_height(x, y);
      if (r == 0 || c == 0 || r == N - 1 || c == N - 1) z -= kSkirtDrop;
      pts[static_cast<std::size_t>(r) * N + c] = {x, y, z};
    }
  std::vector<u32> idx;
  idx.reserve(static_cast<std::size_t>(N - 1) * (N - 1) * 6);
  for (i32 r = 0; r + 1 < N; ++r)
    for (i32 c = 0; c + 1 < N; ++c) {
      const u32 a = static_cast<u32>(r * N + c), b = a + 1, cc = a + static_cast<u32>(N), d = cc + 1;
      idx.insert(idx.end(), {a, b, d, a, d, cc});
    }
  kern::physics::BodySettings bs;
  bs.type = kern::physics::BodyType::fixed;
  const kern::physics::Body body = world_->create_body(bs);
  kern::physics::ShapeSettings ss;
  ss.kind = kern::physics::ShapeKind::mesh;
  ss.points = kern::Span<const Vec3>(pts.data(), pts.size());
  ss.indices = kern::Span<const u32>(idx.data(), idx.size());
  ss.material.friction = 0.8f;  // the level's ground friction so far
  ss.material.category_bits = kCatWorld;
  world_->create_shape(body, ss);
  // New field first, then the old one goes (terrain_tiles.mjs writeTile order).
  CollisionTile& t = collision_[key2(ti, tj)];
  if (t.body.valid() && world_->body_valid(t.body)) world_->destroy_body(t.body);
  t.body = body;
}

void Terrain::build_water_chunk(i32 ci, i32 cj, bool detail) {
  // world_water.mjs: within RADIUS_M 1000 one quad per 4 m water cell, the corners carrying the
  // shore distance (two-pass chamfer, cells) and the flow - what the water shader reads (aWater:
  // shore + 1, flow x, flow y; water_look.h). Farther out the cells merge into rows (deep water,
  // still: the same look without the per-cell mesh).
  kern::MeshData m;
  m.extra_size = 3;
  const Vec3 color = kern::hex_color(0x176b98);  // world_water.mjs material colour
  const i32 per = static_cast<i32>(kWaterChunk / kWaterCell);  // 80 cells
  const i32 cx0 = ci * per, cy0 = cj * per;
  auto wet = [&](i32 cx, i32 cy) {
    if (!water_cell(cx, cy)) return false;
    return bridges_.empty() || !on_bridge((cx + 0.5f) * kWaterCell, (cy + 0.5f) * kWaterCell);
  };
  if (!detail) {
    const f32 deep[3] = {13, 0, 0};
    auto quad = [&](f32 xa, f32 ya, f32 xb, f32 yb) {
      const u32 base = m.vertex_count();
      for (const Vec2 p : {Vec2{xa, ya}, Vec2{xb, ya}, Vec2{xb, yb}, Vec2{xa, yb}}) {
        m.push({p.x, p.y, water_level(p.x, p.y) + kWaterRenderZ}, {0, 0, 1}, color, {p.x * 0.05f, p.y * 0.05f});
        m.push_extra(deep);
      }
      m.indices.insert(m.indices.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
    };
    for (i32 y = cy0; y < cy0 + per; ++y) {
      i32 x = cx0;
      while (x < cx0 + per) {
        if (!wet(x, y)) { ++x; continue; }
        i32 e = x;
        while (e < cx0 + per && wet(e, y)) ++e;
        quad(x * kWaterCell, y * kWaterCell, e * kWaterCell, (y + 1) * kWaterCell);
        x = e;
      }
    }
  } else {
    // the cells of the chunk and a 3-cell margin (the shore distance across the chunk's edge)
    constexpr i32 B = 3;
    const i32 W = per + 2 * B;
    std::vector<u8> water(static_cast<std::size_t>(W) * W);
    std::vector<f32> dist(water.size());
    bool any = false;
    for (i32 j = 0; j < W; ++j)
      for (i32 i = 0; i < W; ++i) {
        const bool w = wet(cx0 - B + i, cy0 - B + j);
        water[static_cast<std::size_t>(j) * W + i] = w;
        dist[static_cast<std::size_t>(j) * W + i] = w ? 1e6f : 0;
        any = any || (w && i >= B && j >= B && i < W - B && j < W - B);
      }
    if (any) {
      auto at = [&](i32 i, i32 j) -> f32& { return dist[static_cast<std::size_t>(j) * W + i]; };
      for (i32 j = 0; j < W; ++j)
        for (i32 i = 0; i < W; ++i) {
          if (!water[static_cast<std::size_t>(j) * W + i]) continue;
          f32 d = at(i, j);
          if (i > 0) d = std::fmin(d, at(i - 1, j) + 1);
          if (j > 0) d = std::fmin(d, at(i, j - 1) + 1);
          if (i > 0 && j > 0) d = std::fmin(d, at(i - 1, j - 1) + 1.414f);
          if (i < W - 1 && j > 0) d = std::fmin(d, at(i + 1, j - 1) + 1.414f);
          at(i, j) = d;
        }
      for (i32 j = W - 1; j >= 0; --j)
        for (i32 i = W - 1; i >= 0; --i) {
          if (!water[static_cast<std::size_t>(j) * W + i]) continue;
          f32 d = at(i, j);
          if (i < W - 1) d = std::fmin(d, at(i + 1, j) + 1);
          if (j < W - 1) d = std::fmin(d, at(i, j + 1) + 1);
          if (i < W - 1 && j < W - 1) d = std::fmin(d, at(i + 1, j + 1) + 1.414f);
          if (i > 0 && j < W - 1) d = std::fmin(d, at(i - 1, j + 1) + 1.414f);
          at(i, j) = d;
        }
      // corners shared by their cells (vtx in world_water.mjs)
      std::vector<i32> corner(static_cast<std::size_t>(per + 1) * (per + 1), -1);
      auto vtx = [&](i32 a, i32 b) -> u32 {
        i32& v = corner[static_cast<std::size_t>(b) * (per + 1) + a];
        if (v >= 0) return static_cast<u32>(v);
        f32 d = 0;
        Vec2 fl{};
        i32 n = 0;
        for (const auto [di, dj] : {std::pair<i32, i32>{0, 0}, {-1, 0}, {0, -1}, {-1, -1}}) {
          const i32 i = a + di + B, j = b + dj + B;
          if (!water[static_cast<std::size_t>(j) * W + i]) continue;
          d += at(i, j);
          const f32 px = (cx0 + a + di + 0.5f) * kWaterCell, py = (cy0 + b + dj + 0.5f) * kWaterCell;
          fl = fl + flow_at(px, py);
          ++n;
        }
        const f32 x = (cx0 + a) * kWaterCell, y = (cy0 + b) * kWaterCell;
        const f32 shore = n == 4 ? std::fmin(d / 4 * kWaterCell, 40.0f) : 0;
        const f32 rec[3] = {shore + 1, n ? fl.x / static_cast<f32>(n) : 0, n ? fl.y / static_cast<f32>(n) : 0};
        v = static_cast<i32>(m.vertex_count());
        m.push({x, y, water_level(x, y) + kWaterRenderZ}, {0, 0, 1}, color, {x * 0.05f, y * 0.05f});
        m.push_extra(rec);
        return static_cast<u32>(v);
      };
      for (i32 b = 0; b < per; ++b)
        for (i32 a = 0; a < per; ++a) {
          if (!water[static_cast<std::size_t>(b + B) * W + (a + B)]) continue;
          const u32 p = vtx(a, b), q = vtx(a + 1, b), r = vtx(a + 1, b + 1), s = vtx(a, b + 1);
          m.indices.insert(m.indices.end(), {p, q, r, p, r, s});
        }
    }
  }
  const u64 key = key2(ci, cj);
  u32& slot = water_meshes_[key];
  water_detail_[key] = detail;
  if (m.vertex_count() == 0) { if (slot) r_->remove(slot); slot = 0; return; }
  kern::MeshInput in = m.as_input(1);  // the second extension's stream (main.cpp: facades, water)
  in.material.roughness = 0.08f;  // world_water.mjs: roughness .08, metalness 0, transparent .94
  in.material.metalness = 0;
  in.material.cast_shadow = false;
  in.material.alpha = kern::AlphaMode::blend;
  const u32 id = r_->allocate_id();
  if (!r_->upsert(id, in)) { r_->release_id(id); return; }
  if (slot) r_->remove(slot);
  slot = id;
}

bool Terrain::ready_at(Vec3 p) const {
  if (!settled_) return false;
  if (!world_) return true;
  // The tile under p and its neighbours (the start spot is searched around p).
  const i32 ci = static_cast<i32>(std::floor(p.x / kTile)), cj = static_cast<i32>(std::floor(p.y / kTile));
  for (i32 j = cj - 1; j <= cj + 1; ++j)
    for (i32 i = ci - 1; i <= ci + 1; ++i) {
      const u64 k = key2(i, j);
      if (!collision_.count(k) || dirty_collision_.count(k)) return false;
    }
  return true;
}

void Terrain::update(Vec3 focus, Vec3 anchor, f64 budget_ms) {
  if (!r_ || !world_) return;
  const f64 deadline = kern::now_ms() + budget_ms;
  plan_windows(focus);
  plan_images(focus);
  if (job_) run_job(deadline);
  if (!settled_) return;
  bool extra_moved = extra.size() != planned_extra_.size();
  for (std::size_t i = 0; !extra_moved && i < extra.size(); ++i) extra_moved = std::hypot(extra[i].x - planned_extra_[i].x, extra[i].y - planned_extra_[i].y) > 40;
  if (replan_ || extra_moved || std::hypot(focus.x - planned_focus_.x, focus.y - planned_focus_.y) > 40 ||
      std::hypot(anchor.x - planned_anchor_.x, anchor.y - planned_anchor_.y) > 40)
    want_tiles(focus, anchor);
  bool missing = false;
  // 0. the ground right under the focus and the anchor (and their neighbours) at once, whatever
  // the budget: a jet at 250 m/s or a reload of the elevation never leaves a hole under anybody
  std::vector<Vec3> under{focus, anchor};
  under.insert(under.end(), extra.begin(), extra.end());
  for (const Vec3& at : under) {
    const i32 ci = static_cast<i32>(std::floor(at.x / kTile)), cj = static_cast<i32>(std::floor(at.y / kTile));
    for (i32 j = cj - 1; j <= cj + 1; ++j)
      for (i32 i = ci - 1; i <= ci + 1; ++i) {
        const u64 k = key2(i, j);
        if (collision_.count(k) && !dirty_collision_.count(k)) continue;
        build_collision_tile(i, j);
        dirty_collision_.erase(k);
        ++stats_.rebuilds;
      }
  }
  // 1. collision (what stands on it must not fall), 2. render, 3. water
  for (u64 k : want_collision_) {
    if (collision_.count(k) && !dirty_collision_.count(k)) continue;
    if (kern::now_ms() > deadline) { missing = true; break; }
    build_collision_tile(static_cast<i32>(k >> 32), static_cast<i32>(k & 0xffffffffu));
    dirty_collision_.erase(k);
    ++stats_.rebuilds;
  }
  for (u64 k : want_render_) {
    if (render_.count(k) && !dirty_render_.count(k)) continue;
    if (kern::now_ms() > deadline) { missing = true; break; }
    i32 level, i, j;
    unkey3(k, level, i, j);
    build_render_tile(level, i, j);
    dirty_render_.erase(k);
    ++stats_.rebuilds;
  }
  for (u64 k : want_water_) {
    const i32 wi = static_cast<i32>(k >> 32), wj = static_cast<i32>(k & 0xffffffffu);
    const bool detail = rect_distance(focus.x, focus.y, wi * kWaterChunk, wj * kWaterChunk, (wi + 1) * kWaterChunk, (wj + 1) * kWaterChunk) < 1000;
    if (water_meshes_.count(k) && !dirty_water_.count(k) && water_detail_[k] == detail) continue;
    if (kern::now_ms() > deadline) { missing = true; break; }
    build_water_chunk(wi, wj, detail);
    dirty_water_.erase(k);
  }
  if (!missing) {
    // Everything wanted is there: drop what is no longer wanted (never before - no holes).
    std::unordered_set<u64> keep(want_render_.begin(), want_render_.end());
    for (auto it = render_.begin(); it != render_.end();) {
      if (!keep.count(it->first)) { if (it->second.mesh) r_->remove(it->second.mesh); dirty_render_.erase(it->first); it = render_.erase(it); }
      else ++it;
    }
    keep = std::unordered_set<u64>(want_collision_.begin(), want_collision_.end());
    for (auto it = collision_.begin(); it != collision_.end();) {
      if (!keep.count(it->first)) {
        if (it->second.body.valid() && world_->body_valid(it->second.body)) world_->destroy_body(it->second.body);
        dirty_collision_.erase(it->first);
        it = collision_.erase(it);
      } else ++it;
    }
    keep = std::unordered_set<u64>(want_water_.begin(), want_water_.end());
    for (auto it = water_meshes_.begin(); it != water_meshes_.end();) {
      if (!keep.count(it->first)) { if (it->second) r_->remove(it->second); dirty_water_.erase(it->first); water_detail_.erase(it->first); it = water_meshes_.erase(it); }
      else ++it;
    }
  }
  stats_.render_tiles = static_cast<u32>(render_.size());
  stats_.collision_tiles = static_cast<u32>(collision_.size());
  stats_.dem = grid_ready_;
  stats_.settled = settled_;
  stats_.dem_tiles = 0;
  for (const auto& [k, t] : dem_tiles_) stats_.dem_tiles += t.h.empty() ? 0 : 1;
}

}  // namespace sc
