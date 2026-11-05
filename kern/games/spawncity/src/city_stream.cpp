// Streamed city (see city_stream.h).
#include "city_stream.h"

#include <algorithm>
#include <cstdio>

#include "city_jobs.h"
#include "kern/core/base.h"
#include "kern/core/jobs.h"
#include "kern/geo/earcut.h"

namespace sc {

namespace {

kern::BuildingStyle konstanz_style() {
  // world_city_buildings.mjs WALLS / ROOFS (sRGB) and the cornice mix with #f4efe4 (0.45)
  auto hex = [](u32 h) { return kern::Rgb{((h >> 16) & 255) / 255.0, ((h >> 8) & 255) / 255.0, (h & 255) / 255.0}; };
  kern::BuildingStyle s;
  s.walls = {hex(0xc9bfae), hex(0xb9a88c), hex(0x8e5443), hex(0x9c9a94), hex(0xd8d2c4), hex(0xa87c5f), hex(0x7d8590), hex(0xcbb79a)};
  s.roofs = {hex(0x4a4c50), hex(0x3b3d40), hex(0x6e4334), hex(0x55585c)};
  s.trim = hex(0xf4efe4);
  s.trim_mix = 0.45;
  return s;
}

// RoadBuilder palette (world_city_roads.mjs colours).
enum : u32 { kAsphalt = 0, kAsphaltMajor, kLine, kPath, kWater, kPaving };
std::vector<kern::Rgb> road_palette() {
  auto rgb = [](u32 h) { return kern::Rgb{((h >> 16) & 255) / 255.0, ((h >> 8) & 255) / 255.0, (h & 255) / 255.0}; };
  return {rgb(0x3c3f44), rgb(0x34373c), rgb(0xe6e4dc), rgb(0x86837c), rgb(0x2f5f86), rgb(0x9a948a)};
}

Vec3 area_color(AreaKind k) {
  switch (k) {
    case AreaKind::water: return kern::hex_color(0x2f5f86);
    case AreaKind::park: return kern::hex_color(0x4f6e35);
    case AreaKind::wood: return kern::hex_color(0x3a5729);
    case AreaKind::grass: return kern::hex_color(0x5a7a3c);
    case AreaKind::sand: return kern::hex_color(0xc9b88f);
    case AreaKind::pitch: return kern::hex_color(0x4f7a35);
  }
  return {0.5f, 0.5f, 0.5f};
}
f32 area_z(AreaKind k) {
  switch (k) {
    case AreaKind::water: return 0.06f;
    case AreaKind::sand: return 0.045f;
    case AreaKind::pitch: return 0.04f;
    case AreaKind::wood: return 0.035f;
    default: return 0.03f;
  }
}

f64 signed_area(const std::vector<Vec2>& r) {
  f64 a = 0;
  for (std::size_t i = 0, j = r.size() - 1; i < r.size(); j = i++) a += static_cast<f64>(r[j].x) * r[i].y - static_cast<f64>(r[i].x) * r[j].y;
  return a * 0.5;
}

// Convex polygon clip against a half plane (distance >= 0 kept).
template <class F>
void clip_poly(const std::vector<Vec2>& in, std::vector<Vec2>& out, F distance) {
  out.clear();
  if (in.empty()) return;
  Vec2 a = in.back();
  f32 da = distance(a);
  for (Vec2 b : in) {
    const f32 db = distance(b);
    if ((da >= 0) != (db >= 0)) {
      const f32 t = da / (da - db);
      out.push_back({a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t});
    }
    if (db >= 0) out.push_back(b);
    a = b;
    da = db;
  }
}

/// terrain_surface.mjs drapeTerrainTriangle: the triangle cut along grid lines and diagonals.
template <class Emit>
void drape_triangle(Vec2 a, Vec2 b, Vec2 c, f32 step, std::vector<Vec2>& p, std::vector<Vec2>& q, Emit emit) {
  const i32 lo_x = static_cast<i32>(std::floor(std::fmin(a.x, std::fmin(b.x, c.x)) / step)), hi_x = static_cast<i32>(std::floor(std::fmax(a.x, std::fmax(b.x, c.x)) / step));
  const i32 lo_y = static_cast<i32>(std::floor(std::fmin(a.y, std::fmin(b.y, c.y)) / step)), hi_y = static_cast<i32>(std::floor(std::fmax(a.y, std::fmax(b.y, c.y)) / step));
  for (i32 iy = lo_y; iy <= hi_y; ++iy)
    for (i32 ix = lo_x; ix <= hi_x; ++ix) {
      const f32 x = ix * step, y = iy * step;
      p = {a, b, c};
      clip_poly(p, q, [&](Vec2 v) { return v.x - x; });
      clip_poly(q, p, [&](Vec2 v) { return x + step - v.x; });
      clip_poly(p, q, [&](Vec2 v) { return v.y - y; });
      clip_poly(q, p, [&](Vec2 v) { return y + step - v.y; });
      const std::vector<Vec2> cell = p;
      for (f32 sign : {1.0f, -1.0f}) {
        clip_poly(cell, q, [&](Vec2 v) { return sign * ((v.x - x) - (v.y - y)); });
        for (std::size_t i = 1; i + 1 < q.size(); ++i) {
          const Vec2 A = q[0], B = q[i], C = q[i + 1];
          if (std::fabs((B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x)) < 1e-6f) continue;
          emit(A, B, C);
        }
      }
    }
}

}  // namespace

struct CityStream::Tile {
  enum class State : u8 { requested, decoding, terrain_wait, building, roads, areas, collision, ready, failed } state = State::requested;
  TileKey key;
  bool procedural = false;
  std::vector<u8> bytes;
  mvt::Decoder decoder;
  MapFeatures features;
  kern::FootprintStream footprints;
  kern::RoadStream roads;
  kern::BuildingBuilder buildings;
  kern::RoadBuilder road_builder;
  std::vector<u32> meshes;
  kern::physics::Body body;
  std::size_t collide_next = 0, upload_next = 0;
  u32 colliders = 0, triangles = 0;
  Vec2 center;
  // terrain: road and area meshes are re-laid when the ground under them changes (craters, water)
  kern::Vec4 bounds{-1e9f, -1e9f, 1e9f, 1e9f};
  std::vector<u32> lay_meshes, old_lay;
  bool relay = false;
  // the drawn subset (indices into features.buildings) and its centres; a re-sync swaps the meshes
  std::vector<u32> vis, vis_built;
  std::vector<Vec2> cent;
  std::vector<u32> old_meshes;
  // jobs off the frame thread (city_jobs.h): the tile's decoding, the drawn buildings' extrusion
  u32 tile_job = 0, build_job = 0;
  std::vector<kern::GeoChunk> built;
  bool rebuilding = false, vis_dirty = false, prepared = false;
  // interiors (M3 life): one building left out, the collision rebuilt beside the old body
  i32 skip = -1;
  kern::physics::Body old_body;
  bool recollide = false;
};

CityStream::CityStream() = default;
CityStream::~CityStream() = default;

void CityStream::start(kern::Renderer& renderer, kern::physics::World& world, f64 lon, f64 lat, bool dem) {
  clear();
  r_ = &renderer;
  world_ = &world;
  start_ms_ = kern::now_ms();
  proj_.init(lon, lat);
  terrain_.start(&renderer, &world, proj_, dem);
  origin_tile_ = tile_at(lon, lat);
  offline_ = false;
  real_tiles_ = 0;
}

void CityStream::clear() {
  for (auto& [k, t] : tiles_) remove(*t);
  tiles_.clear();
  requests_.clear();
  lanes_.clear();
  stats_ = {};
  ms_ = {};
  terrain_.clear();
  terrain_changes_.clear();
  if (world_) for (kern::physics::Body b : {near_.body, near_.old_body}) if (b.valid() && world_->body_valid(b)) world_->destroy_body(b);
  near_ = {};
  vis_at_ = {1e9f, 1e9f};
  vis_ms_ = -1e9;
}

void CityStream::remove(Tile& t) {
  if (r_)
    for (const std::vector<u32>* list : {&t.meshes, &t.lay_meshes, &t.old_lay})
      for (u32 id : *list) r_->remove(id);
  for (u32* j : {&t.tile_job, &t.build_job}) if (*j) { kern::jobs::cancel(*j); *j = 0; }
  if (r_) for (u32 id : t.old_meshes) r_->remove(id);
  t.old_meshes.clear();
  t.meshes.clear();
  t.lay_meshes.clear();
  t.old_lay.clear();
  near_.dirty = true;  // the collision set may hold this tile's buildings
  if (world_ && t.body.valid() && world_->body_valid(t.body)) world_->destroy_body(t.body);
  t.body = {};
  if (world_ && t.old_body.valid() && world_->body_valid(t.old_body)) world_->destroy_body(t.old_body);  // M3 life
  t.old_body = {};
  for (Lane& l : lanes_)
    if (l.tile == t.key.hash()) l.alive = false;
}

CityStream::Tile* CityStream::tile(const TileKey& k) {
  auto it = tiles_.find(k.hash());
  return it == tiles_.end() ? nullptr : it->second.get();
}

void CityStream::request_around(Vec3 focus) {
  if (offline_) return;
  f64 lon, lat;
  unproject(proj_, focus.x, focus.y, lon, lat);
  const TileKey c = tile_at(lon, lat);
  // Nearest first: the player's tile, then the ring.
  std::vector<std::pair<i32, TileKey>> want;
  for (i32 dy = -ring; dy <= ring; ++dy)
    for (i32 dx = -ring; dx <= ring; ++dx) want.push_back({dx * dx + dy * dy, TileKey{c.z, c.x + dx, c.y + dy}});
  std::sort(want.begin(), want.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
  for (const auto& [d, k] : want) {
    if (tile(k)) continue;
    auto t = std::make_unique<Tile>();
    t->key = k;
    f64 lo, la, x, y;
    tile_lonlat(k, 0.5, 0.5, lo, la);
    proj_.project(lo, la, x, y);
    t->center = {static_cast<f32>(x), static_cast<f32>(y)};
    f64 x0, y0, x1, y1;
    tile_lonlat(k, 0, 1, lo, la);
    proj_.project(lo, la, x0, y0);
    tile_lonlat(k, 1, 0, lo, la);
    proj_.project(lo, la, x1, y1);
    t->bounds = {static_cast<f32>(x0), static_cast<f32>(y0), static_cast<f32>(x1), static_cast<f32>(y1)};
    tiles_[k.hash()] = std::move(t);
    requests_.push_back(k);
  }
}

void CityStream::unload_far(Vec3 focus) {
  f64 lon, lat;
  unproject(proj_, focus.x, focus.y, lon, lat);
  const TileKey c = tile_at(lon, lat);
  for (auto it = tiles_.begin(); it != tiles_.end();) {
    Tile& t = *it->second;
    const i32 d = std::max(std::abs(t.key.x - c.x), std::abs(t.key.y - c.y));
    if (!t.procedural && d > ring + 1) {
      remove(t);
      it = tiles_.erase(it);
    } else ++it;
  }
  // Lanes of unloaded tiles stay as dead entries: traffic holds indices into the list.
}

std::string CityStream::take_requests() {
  std::string s = "[";
  for (std::size_t i = 0; i < requests_.size(); ++i) {
    char b[64];
    std::snprintf(b, sizeof b, "%s[%d,%d,%d]", i ? "," : "", requests_[i].z, requests_[i].x, requests_[i].y);
    s += b;
  }
  if (!requests_.empty() && ms_.t_request < 0) ms_.t_request = kern::now_ms() - start_ms_;
  requests_.clear();
  return s + "]";
}

void CityStream::provide(const TileKey& key, const u8* bytes, std::size_t length) {
  Tile* t = tile(key);
  if (!t || t->state != Tile::State::requested) return;
  if (key == origin_tile_ && ms_.t_data < 0) ms_.t_data = kern::now_ms() - start_ms_;
  // decoded off the frame (a Web Worker / thread): the frame only takes the finished features
  t->tile_job = kern::jobs::submit(cityjobs::kTile, cityjobs::tile_input(key, proj_.lon0, proj_.lat0, bytes, length));
  t->state = Tile::State::decoding;
}

void CityStream::fail(const TileKey& key) {
  if (Tile* t = tile(key)) t->state = Tile::State::failed;
  // Nothing came for the player's own start tile: build the stand-in town instead.
  if (key == origin_tile_ && real_tiles_ == 0) go_offline();
}

void CityStream::go_offline() {
  if (offline_) return;
  offline_ = true;
  for (auto& [k, t] : tiles_) remove(*t);
  tiles_.clear();
  requests_.clear();
  lanes_.clear();
  // No map service: no elevation service either (flat ground, the stand-in town's lake).
  terrain_.clear();
  terrain_.disable_dem();
  auto t = std::make_unique<Tile>();
  t->procedural = true;
  t->key = TileKey{0, -1, -1};
  procedural_city(0, 0, 760, 20261029, t->features);
  feed_terrain(*t);
  t->state = Tile::State::building;
  finish_features(*t);
  tiles_[t->key.hash()] = std::move(t);
}

bool CityStream::ready_at(Vec3 p) const {
  if (!terrain_.ready_at(p)) return false;
  const bool solid = near_.built && !near_.building && !near_.dirty && std::hypot(near_.at.x - p.x, near_.at.y - p.y) < 140;
  if (offline_) {
    for (const auto& [k, t] : tiles_)
      if (t->procedural) return t->state == Tile::State::ready && solid;
    return false;
  }
  f64 lon, lat;
  unproject(proj_, p.x, p.y, lon, lat);
  const TileKey c = tile_at(lon, lat);
  auto it = tiles_.find(c.hash());
  (void)it;
  (void)solid;
  // As the original: the game starts as soon as the ground under the player is there; the city
  // (drawn set, collision set) streams in around him (world_spawn_guard: see Game::spawn_guard).
  return true;
}

void CityStream::feed_terrain(Tile& t) {
  // world_water.mjs: lakes / harbours (polygons) and waterways (lines with a width per class)
  // become basins in the terrain; bridges keep a solid road over them.
  for (const MapFeatures::Area& a : t.features.areas)
    if (a.kind == AreaKind::water) terrain_.add_water_polygon(a.rings);
  for (const MapFeatures::Road& r : t.features.roads) {
    if (road_is_water(r.cls)) terrain_.add_waterway(r.points, road_width(r.cls), r.cls == RoadClass::river ? 1.6f : r.cls == RoadClass::canal ? 0.5f : 1.1f);
    else if (r.bridge)
      for (std::size_t i = 0; i + 1 < r.points.size(); ++i) terrain_.add_bridge(r.points[i], r.points[i + 1], road_width(r.cls));
  }
}

void CityStream::finish_features(Tile& t) {
  // Footprint and road streams for the engine's builders, lanes for the traffic.
  t.cent.clear();
  for (MapFeatures::Building& b : t.features.buildings) {
    // terrain_craters.mjs buildingGroundBase: a house stands on the lowest ground of its outline.
    if (!b.rings.empty()) {
      const f32 g = terrain_.footprint_base(b.rings[0]);
      b.base += g;
      b.top += g;
    }
    Vec2 c{};
    if (!b.rings.empty() && !b.rings[0].empty()) { for (Vec2 q : b.rings[0]) c = c + q; c = c * (1.0f / static_cast<f32>(b.rings[0].size())); }
    t.cent.push_back(c);
  }
  t.prepared = true;
  for (const MapFeatures::Road& r : t.features.roads) {
    const f32 w = road_width(r.cls);
    const bool water = road_is_water(r.cls);
    if (water) continue;  // drawn by the terrain's water surface
    const bool major = static_cast<u8>(r.cls) <= static_cast<u8>(RoadClass::tertiary);
    const u32 color = water ? kWater : r.cls == RoadClass::pedestrian ? kPaving
                      : (r.cls == RoadClass::path || r.cls == RoadClass::track) ? kPath
                      : major ? kAsphaltMajor : kAsphalt;
    t.roads.road(r.points.data(), static_cast<u32>(r.points.size()), w, !water && road_drivable(r.cls), color, major && !water ? static_cast<i32>(kLine) : -1);
    if (road_drivable(r.cls)) lanes_.push_back(Lane{r.points, w, t.key.hash(), true});
  }
  select_visual(focus_);
  begin_visual(t);
}

void CityStream::begin_visual(Tile& t) {
  // world_city_buildings.mjs: the selected footprints, at most MAX_VERTICES 96 per ring
  t.footprints = kern::FootprintStream{};
  std::vector<Vec2> thin;
  for (u32 i : t.vis) {
    const MapFeatures::Building& b = t.features.buildings[i];
    t.footprints.begin(b.seed, b.base, b.top);
    for (const auto& ring : b.rings) {
      if (ring.size() <= 96) { t.footprints.ring(ring.data(), static_cast<u32>(ring.size())); continue; }
      const std::size_t step = (ring.size() + 95) / 96;
      thin.clear();
      for (std::size_t k = 0; k < ring.size(); k += step) thin.push_back(ring[k]);
      t.footprints.ring(thin.data(), static_cast<u32>(thin.size()));
    }
  }
  t.vis_built = t.vis;
  t.vis_dirty = false;
  kern::BuildingOptions bo;
  bo.chunk_size = 160;  // smaller uploads per frame (the original: 360 m; it only copies buffers)
  bo.max_buildings = 12000;
  bo.style = konstanz_style();
  // extruded and welded off the frame (city_jobs.h); the frame only uploads the finished chunks
  if (t.build_job) kern::jobs::cancel(t.build_job);
  t.build_job = kern::jobs::submit(cityjobs::kBuildings, cityjobs::buildings_input(t.footprints, bo));
  t.built.clear();
  t.upload_next = 0;
  t.state = Tile::State::building;
}

void CityStream::select_visual(Vec3 focus) {
  // The nearest MAX_FOOTPRINTS buildings within VISUAL_RADIUS_M of all decoded tiles.
  struct Pick { f32 d2; Tile* t; u32 i; };
  std::vector<Pick> all;
  constexpr f32 kR = 900;
  for (auto& [k, tp] : tiles_) {
    Tile& t = *tp;
    if (!t.prepared) continue;
    for (u32 i = 0; i < t.cent.size(); ++i) {
      const f32 dx = t.cent[i].x - focus.x, dy = t.cent[i].y - focus.y, d2 = dx * dx + dy * dy;
      if (d2 <= kR * kR) all.push_back({d2, &t, i});
    }
  }
  constexpr std::size_t kMax = 2600;
  if (all.size() > kMax) { std::nth_element(all.begin(), all.begin() + kMax, all.end(), [](const Pick& a, const Pick& b) { return a.d2 < b.d2; }); all.resize(kMax); }
  for (auto& [k, tp] : tiles_) tp->vis.clear();
  for (const Pick& p : all) p.t->vis.push_back(p.i);
  for (auto& [k, tp] : tiles_) {
    Tile& t = *tp;
    std::sort(t.vis.begin(), t.vis.end());
    if (t.state == Tile::State::ready && t.vis != t.vis_built) t.vis_dirty = true;
  }
  vis_at_ = {focus.x, focus.y};
  vis_ms_ = kern::now_ms();
}

void CityStream::upload(Tile& t, kern::GeoChunk& chunk, const kern::Material& material, bool walls) {
  if (chunk.mesh.vertex_count() == 0) return;
  if (!chunk.welded) kern::weld(chunk.mesh);
  const u32 id = r_->allocate_id();
  kern::MeshInput in = chunk.mesh.as_input(walls ? 0 : kern::kMaxExtensionAttributes);
  in.material = material;
  in.pick_bvh = false;  // hits are Box3D rays (the collision set); no picking BVH to build per chunk
  if (!r_->upsert(id, in)) { r_->release_id(id); return; }
  t.meshes.push_back(id);
  t.triangles += chunk.mesh.triangle_count();
}

void CityStream::build_areas(Tile& t) {
  kern::MeshData mesh;
  kern::Earcut earcut;
  std::vector<f64> xy;
  std::vector<u32> holes, tris;
  const bool drape = terrain_.has_dem() || terrain_.crater_count() > 0;
  std::vector<Vec2> poly, clipped;
  for (const MapFeatures::Area& a : t.features.areas) {
    if (a.kind == AreaKind::water) continue;  // the terrain's water surface
    xy.clear();
    holes.clear();
    tris.clear();
    for (std::size_t r = 0; r < a.rings.size(); ++r) {
      if (a.rings[r].size() < 3) continue;
      if (r) holes.push_back(static_cast<u32>(xy.size() / 2));
      for (Vec2 p : a.rings[r]) xy.insert(xy.end(), {p.x, p.y});
    }
    if (xy.size() < 6) continue;
    earcut.triangulate(xy.data(), static_cast<u32>(xy.size() / 2), holes.data(), static_cast<u32>(holes.size()), tris);
    const Vec3 c = area_color(a.kind);
    const f32 z = area_z(a.kind);
    for (std::size_t i = 0; i + 2 < tris.size(); i += 3) {
      Vec3 p[3];
      for (int k = 0; k < 3; ++k) p[k] = {static_cast<f32>(xy[tris[i + k] * 2]), static_cast<f32>(xy[tris[i + k] * 2 + 1]), z};
      // Faces up: counter-clockwise seen from above.
      if (kern::cross(p[1] - p[0], p[2] - p[0]).z < 0) std::swap(p[1], p[2]);
      if (!drape) {
        for (int k = 0; k < 3; ++k) mesh.push(p[k], {0, 0, 1}, c, {p[k].x * 0.1f, p[k].y * 0.1f});
        continue;
      }
      // terrain_surface.mjs drapeTerrainTriangle: split along the 5 m grid and the cells'
      // SW-NE diagonal, so the patch lies on the ground instead of cutting through it.
      drape_triangle({p[0].x, p[0].y}, {p[1].x, p[1].y}, {p[2].x, p[2].y}, Terrain::kStep, poly, clipped, [&](Vec2 q0, Vec2 q1, Vec2 q2) {
        for (Vec2 q : {q0, q1, q2}) mesh.push({q.x, q.y, terrain_.height(q.x, q.y) + z}, terrain_.normal(q.x, q.y), c, {q.x * 0.1f, q.y * 0.1f});
      });
    }
  }
  if (mesh.vertex_count() == 0) return;
  kern::weld(mesh);
  kern::Material m;
  m.roughness = 0.9f;
  m.cast_shadow = false;
  const u32 id = r_->allocate_id();
  kern::MeshInput in = mesh.as_input(kern::kMaxExtensionAttributes);
  in.material = m;
  if (r_->upsert(id, in)) t.lay_meshes.push_back(id);
  else r_->release_id(id);
}

void CityStream::begin_roads(Tile& t) {
  kern::RoadOptions ro;
  ro.center_x = focus_.x;  // world_city_roads.mjs RADIUS_M 720 around the player
  ro.center_y = focus_.y;
  ro.radius = 720;
  ro.segment_length = 4;  // follows the 5 m terrain nodes
  ro.palette = road_palette();
  t.road_builder.begin(t.roads.data(), t.roads.size(), ro, terrain_.sampler());
  t.upload_next = 0;
  t.state = Tile::State::roads;
}

void CityStream::relay(const kern::Vec4& rect) {
  for (auto& [k, tp] : tiles_) {
    Tile& t = *tp;
    if (t.state != Tile::State::ready || t.relay) continue;
    if (rect.x > t.bounds.z || rect.z < t.bounds.x || rect.y > t.bounds.w || rect.w < t.bounds.y) continue;
    t.relay = true;
    t.old_lay = std::move(t.lay_meshes);
    t.lay_meshes.clear();
    begin_roads(t);
  }
}

void CityStream::update_collision(Vec3 focus, f64 deadline) {
  NearCollision& n = near_;
  const f64 now = kern::now_ms();
  const bool moved = std::hypot(focus.x - n.at.x, focus.y - n.at.y) >= 140;
  if (!n.building && (moved || (n.dirty && now - n.at_ms >= 250))) {
    // world_building_collisions.mjs: the nearest MAX_FOOTPRINTS 480 within RADIUS_M 340
    struct Pick { f32 d2; u64 tile; u32 i; };
    std::vector<Pick> all;
    for (auto& [k, tp] : tiles_) {
      const Tile& t = *tp;
      if (t.state != Tile::State::ready && t.state != Tile::State::building && t.state != Tile::State::roads && t.state != Tile::State::areas) continue;
      if (!t.prepared) continue;
      for (u32 i = 0; i < t.cent.size(); ++i) {
        if (k == excl_tile_ && static_cast<i32>(i) == excl_index_) continue;  // an entered interior (M3 life)
        const f32 dx = t.cent[i].x - focus.x, dy = t.cent[i].y - focus.y, d2 = dx * dx + dy * dy;
        if (d2 <= 340.0f * 340.0f) all.push_back({d2, k, i});
      }
    }
    std::sort(all.begin(), all.end(), [](const Pick& a, const Pick& b) { return a.d2 < b.d2; });
    if (all.size() > 480) all.resize(480);
    n.list.clear();
    for (const Pick& p : all) n.list.push_back({p.tile, p.i});
    n.old_body = n.body;
    n.body = {};
    n.next = 0;
    n.colliders = 0;
    n.building = true;
    n.dirty = false;
    n.at = {focus.x, focus.y};
    n.at_ms = now;
  }
  if (n.building && build_near(deadline)) {
    if (n.old_body.valid() && world_->body_valid(n.old_body)) world_->destroy_body(n.old_body);
    n.old_body = {};
    n.building = false;
    n.built = true;
    if (ms_.t_collision < 0 && !n.list.empty()) ms_.t_collision = kern::now_ms() - start_ms_;
  }
}

bool CityStream::build_near(f64 deadline) {
  NearCollision& n = near_;
  if (!n.body.valid()) {
    kern::physics::BodySettings bs;
    bs.type = kern::physics::BodyType::fixed;
    n.body = world_->create_body(bs);
  }
  kern::Earcut earcut;
  std::vector<f64> xy;
  std::vector<u32> holes, tris, idx;
  std::vector<Vec3> pts;
  while (n.next < n.list.size()) {
    const auto [tile_key, bi] = n.list[n.next++];
    auto it = tiles_.find(tile_key);
    if (it == tiles_.end() || bi >= it->second->features.buildings.size()) continue;
    const MapFeatures::Building& b = it->second->features.buildings[bi];
    xy.clear(); holes.clear(); tris.clear(); idx.clear(); pts.clear();
    std::vector<std::vector<Vec2>> rings = b.rings;
    // MAX_VERTICES 64 per ring
    for (auto& ring : rings)
      if (ring.size() > 64) { const std::size_t step = (ring.size() + 63) / 64; std::vector<Vec2> thin; for (std::size_t k = 0; k < ring.size(); k += step) thin.push_back(ring[k]); ring = std::move(thin); }
    for (std::size_t r = 0; r < rings.size(); ++r) {
      auto& ring = rings[r];
      if (ring.size() < 3) continue;
      const f64 a = signed_area(ring);
      if (std::fabs(a) < 0.5) continue;
      if ((r == 0) != (a > 0)) std::reverse(ring.begin(), ring.end());  // outer CCW, courtyards CW
      if (r) holes.push_back(static_cast<u32>(xy.size() / 2));
      for (Vec2 p : ring) xy.insert(xy.end(), {p.x, p.y});
      // Walls: quads per edge, outward = right of the edge for CCW outer / CW holes.
      for (std::size_t i = 0; i < ring.size(); ++i) {
        const Vec2 p0 = ring[i], p1 = ring[(i + 1) % ring.size()];
        const u32 base = static_cast<u32>(pts.size());
        pts.push_back({p0.x, p0.y, b.base});
        pts.push_back({p1.x, p1.y, b.base});
        pts.push_back({p1.x, p1.y, b.top});
        pts.push_back({p0.x, p0.y, b.top});
        idx.insert(idx.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
      }
    }
    if (xy.size() >= 6) {
      earcut.triangulate(xy.data(), static_cast<u32>(xy.size() / 2), holes.data(), static_cast<u32>(holes.size()), tris);
      const u32 base = static_cast<u32>(pts.size());
      for (std::size_t i = 0; i < xy.size() / 2; ++i) pts.push_back({static_cast<f32>(xy[i * 2]), static_cast<f32>(xy[i * 2 + 1]), b.top});
      for (std::size_t i = 0; i + 2 < tris.size(); i += 3) {
        u32 a = base + tris[i], c = base + tris[i + 1], d = base + tris[i + 2];
        if (kern::cross(pts[c] - pts[a], pts[d] - pts[a]).z < 0) std::swap(c, d);
        idx.insert(idx.end(), {a, c, d});
      }
    }
    if (!idx.empty()) {
      kern::physics::ShapeSettings ss;
      ss.kind = kern::physics::ShapeKind::mesh;
      ss.points = kern::Span<const Vec3>(pts.data(), pts.size());
      ss.indices = kern::Span<const u32>(idx.data(), idx.size());
      ss.material.friction = 0.7f;
      ss.material.category_bits = kCatWorld;
      if (world_->create_shape(n.body, ss).valid()) ++n.colliders;
    }
    if (kern::now_ms() > deadline) return false;
  }
  return true;
}

bool CityStream::work(Tile& t, f64 deadline) {
  using S = Tile::State;
  switch (t.state) {
    case S::ready:
      if (t.vis_dirty && !t.relay) {
        t.old_meshes = std::move(t.meshes);
        t.meshes.clear();
        t.triangles = 0;
        t.rebuilding = true;
        begin_visual(t);
        return kern::now_ms() > deadline ? false : work(t, deadline);
      }
      return true;
    case S::requested:
    case S::failed: return true;
    case S::decoding: {
      std::vector<u8> out;
      if (!kern::jobs::take(t.tile_job, out)) return true;  // still decoding off the frame: next tile
      t.tile_job = 0;
      if (!cityjobs::read_features(out, t.features)) { t.state = S::failed; return true; }
      ++real_tiles_;
      if (t.key == origin_tile_ && ms_.t_decoded < 0) ms_.t_decoded = kern::now_ms() - start_ms_;
      feed_terrain(t);
      t.state = S::terrain_wait;
      return kern::now_ms() > deadline ? false : work(t, deadline);
    }
    case S::terrain_wait:
      // Buildings and streets stand on the terrain: wait until its shape is decided.
      if (!terrain_.settled()) return true;
      if (ms_.t_terrain < 0) ms_.t_terrain = kern::now_ms() - start_ms_;
      finish_features(t);
      return kern::now_ms() > deadline ? false : work(t, deadline);
    case S::building: {
      if (t.build_job) {
        std::vector<u8> out;
        if (!kern::jobs::take(t.build_job, out)) return true;  // still extruding off the frame
        t.build_job = 0;
        if (!cityjobs::read_chunks(out, t.built)) t.built.clear();
        t.upload_next = 0;
      }
      // One finished chunk mesh at a time (the upload; the picking BVH is skipped for buildings).
      kern::Material m;
      m.roughness = 0.82f;
      auto& chunks = t.built;
      while (t.upload_next < chunks.size()) {
        upload(t, chunks[t.upload_next], m, true);
        chunks[t.upload_next++].mesh = kern::MeshData{};
        if (kern::now_ms() > deadline) return false;
      }
      chunks.clear();
      chunks.shrink_to_fit();
      t.upload_next = 0;
      if (t.rebuilding) {
        // the re-synced set is complete: the old meshes go in the same frame (nothing pops out)
        for (u32 id : t.old_meshes) r_->remove(id);
        t.old_meshes.clear();
        t.rebuilding = false;
        // the streets follow the player too (world_city_roads.mjs): re-laid beside the old ones
        t.relay = true;
        t.old_lay = std::move(t.lay_meshes);
        t.lay_meshes.clear();
      }
      begin_roads(t);
      return kern::now_ms() > deadline ? false : work(t, deadline);
    }
    case S::roads: {
      if (!t.road_builder.step(std::max(0.5, deadline - kern::now_ms()))) return false;
      kern::Material m;
      m.roughness = 0.95f;
      m.cast_shadow = false;
      m.double_sided = true;
      auto& chunks = t.road_builder.chunks();
      while (t.upload_next < chunks.size()) {
        const std::size_t before = t.meshes.size();
        upload(t, chunks[t.upload_next], m, false);
        // road meshes are the re-layable ones
        t.lay_meshes.insert(t.lay_meshes.end(), t.meshes.begin() + static_cast<std::ptrdiff_t>(before), t.meshes.end());
        t.meshes.resize(before);
        chunks[t.upload_next++].mesh = kern::MeshData{};
        if (kern::now_ms() > deadline) return false;
      }
      chunks.clear();
      chunks.shrink_to_fit();
      t.upload_next = 0;
      t.state = S::areas;
      return kern::now_ms() > deadline ? false : work(t, deadline);
    }
    case S::areas:
      build_areas(t);
      if (t.relay) {
        // New layer in place: the old one goes now (no frame without roads).
        for (u32 id : t.old_lay) r_->remove(id);
        t.old_lay.clear();
        t.relay = false;
        t.state = S::ready;
        return true;
      }
      t.state = S::ready;  // collision: the working set around the player (update_collision)
      // a new set only when this tile brings buildings into the 340 m around the player
      for (Vec2 c : t.cent)
        if (std::hypot(c.x - focus_.x, c.y - focus_.y) <= 340) { near_.dirty = true; break; }
      return true;
    case S::collision:
      t.state = S::ready;
      return true;
  }
  return true;
}

void CityStream::update(Vec3 focus, f64 budget_ms) {
  if (!r_ || !world_) return;
  const f64 start = kern::now_ms(), deadline = start + budget_ms;
  // without workers / threads the jobs run here, inside the frame's budget
  kern::jobs::pump(budget_ms * 0.5);
  focus_ = focus;
  // RESYNC_MOVE_M 140 / RESYNC_MS 2500 (world_city_buildings.mjs)
  if (std::hypot(focus.x - vis_at_.x, focus.y - vis_at_.y) >= 140 || start - vis_ms_ >= 2500) select_visual(focus);
  request_around(focus);
  unload_far(focus);
  terrain_.update(focus, anchor, std::fmax(1.0, budget_ms * 0.45));
  terrain_.take_changes(terrain_changes_);
  for (const kern::Vec4& r : terrain_changes_) relay(r);
  terrain_changes_.clear();
  // The collision around the player first (the player must never stand where no wall is): half
  // the budget while a set is built, at least 1.5 ms.
  update_collision(focus, kern::now_ms() + std::fmax(1.5, budget_ms * 0.5));
  // Nearest unfinished tile first.
  std::vector<Tile*> order;
  for (auto& [k, t] : tiles_) order.push_back(t.get());
  std::sort(order.begin(), order.end(), [&](Tile* a, Tile* b) {
    const f32 da = (a->center.x - focus.x) * (a->center.x - focus.x) + (a->center.y - focus.y) * (a->center.y - focus.y);
    const f32 db = (b->center.x - focus.x) * (b->center.x - focus.x) + (b->center.y - focus.y) * (b->center.y - focus.y);
    return da < db;
  });
  for (Tile* t : order) {
    if (kern::now_ms() > deadline) break;
    if (!work(*t, deadline)) break;
  }
  // what is left of the budget goes on with the collision set
  if (kern::now_ms() < deadline) update_collision(focus, deadline);
  stats_ = {};
  for (auto& [k, t] : tiles_) {
    if (t->state == Tile::State::ready) ++stats_.tiles_ready;
    else if (t->state == Tile::State::failed) ++stats_.tiles_failed;
    else ++stats_.tiles_loading;
    stats_.buildings += static_cast<u32>(t->features.buildings.size());

    stats_.triangles += t->triangles;
    stats_.meshes += static_cast<u32>(t->meshes.size() + t->lay_meshes.size());
  }
  stats_.colliders = near_.colliders;
  ms_.work_ms += kern::now_ms() - start;
  if (ms_.t_built < 0) { auto it = tiles_.find(origin_tile_.hash()); if (it != tiles_.end() && it->second->state == Tile::State::ready) ms_.t_built = kern::now_ms() - start_ms_; }
  stats_.t_request = ms_.t_request; stats_.t_data = ms_.t_data; stats_.t_decoded = ms_.t_decoded; stats_.t_terrain = ms_.t_terrain;
  stats_.t_built = ms_.t_built; stats_.t_collision = ms_.t_collision; stats_.work_ms = ms_.work_ms;
  stats_.last_slice_ms = kern::now_ms() - start;
  if (kern::now_ms() - worst_at_ > 2000) { worst_ = 0; worst_at_ = kern::now_ms(); }
  worst_ = std::fmax(worst_, stats_.last_slice_ms);
  stats_.worst_ms = worst_;
}

void CityStream::walls_near(Vec2 p, f32 r, std::vector<Wall>& out) const {
  out.clear();
  for (const auto& [k, tp] : tiles_) {
    const Tile& t = *tp;
    if (t.state != Tile::State::ready) continue;
    if (p.x < t.bounds.x - r - 200 || p.x > t.bounds.z + r + 200 || p.y < t.bounds.y - r - 200 || p.y > t.bounds.w + r + 200) continue;
    for (const MapFeatures::Building& b : t.features.buildings) {
      if (b.rings.empty() || b.rings[0].empty()) continue;
      const Vec2 q = b.rings[0][0];
      if (std::fabs(q.x - p.x) > r + 150 || std::fabs(q.y - p.y) > r + 150) continue;
      u32 edge = 0;
      for (std::size_t ri = 0; ri < b.rings.size(); ++ri) {
        const auto& ring = b.rings[ri];
        if (ring.size() < 3) continue;
        // Outward: right of the edge for a counter-clockwise outer ring (left for courtyards).
        const bool ccw = signed_area(ring) > 0;
        const f32 s = (ccw == (ri == 0)) ? 1.0f : -1.0f;
        for (std::size_t i = 0; i < ring.size(); ++i, ++edge) {
          const Vec2 a = ring[i], c = ring[(i + 1) % ring.size()];
          const f32 len = std::hypot(c.x - a.x, c.y - a.y);
          if (len < 0.05f) continue;
          const Vec2 d{(c.x - a.x) / len, (c.y - a.y) / len};
          const f32 u = (p.x - a.x) * d.x + (p.y - a.y) * d.y;
          const Vec2 n{d.y * s, -d.x * s};
          const f32 side = (p.x - a.x) * n.x + (p.y - a.y) * n.y;
          if (u < -r || u > len + r || std::fabs(side) > r) continue;
          out.push_back({a, d, n, len, b.base, b.top, b.seed, edge});
        }
      }
    }
  }
}

bool CityStream::building_at(Vec2 p, std::vector<Vec2>* ring_out, f32* base, f32* top, u32* seed) const {
  for (const auto& [k, tp] : tiles_) {
    const Tile& t = *tp;
    if (t.state != Tile::State::ready) continue;
    if (p.x < t.bounds.x - 200 || p.x > t.bounds.z + 200 || p.y < t.bounds.y - 200 || p.y > t.bounds.w + 200) continue;
    for (const MapFeatures::Building& b : t.features.buildings) {
      if (b.rings.empty() || b.rings[0].size() < 3) continue;
      const auto& ring = b.rings[0];
      if (std::fabs(ring[0].x - p.x) > 150 || std::fabs(ring[0].y - p.y) > 150) continue;
      bool inside = false;
      for (std::size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++)
        if ((ring[i].y > p.y) != (ring[j].y > p.y) && p.x < (ring[j].x - ring[i].x) * (p.y - ring[i].y) / (ring[j].y - ring[i].y) + ring[i].x) inside = !inside;
      if (!inside) continue;
      if (ring_out) *ring_out = ring;
      if (base) *base = b.base;
      if (top) *top = b.top;
      if (seed) *seed = b.seed;
      return true;
    }
  }
  return false;
}

bool CityStream::street_point(Rng& rng, Vec3 near, f32 radius, Vec3& point, f32& heading) const {
  if (lanes_.empty()) return false;
  for (int attempt = 0; attempt < 48; ++attempt) {
    const Lane& l = lanes_[rng.next() % lanes_.size()];
    if (!l.alive || l.points.size() < 2) continue;
    const std::size_t i = rng.next() % (l.points.size() - 1);
    const Vec2 a = l.points[i], b = l.points[i + 1];
    const f32 t = rng.unit();
    const Vec2 p = a + (b - a) * t;
    const f32 d = std::hypot(p.x - near.x, p.y - near.y);
    if (d > radius || std::hypot(b.x - a.x, b.y - a.y) < 4) continue;
    heading = std::atan2(b.y - a.y, b.x - a.x);
    // Right-hand traffic: the lane centre right of the street's centre line.
    const f32 off = std::fmin(l.width * 0.25f, 1.8f);
    point = {p.x + std::sin(heading) * off, p.y - std::cos(heading) * off, ground_height(p.x, p.y)};
    return true;
  }
  return false;
}

// ---------------------------------------------------------------------------------------------
// street life and interiors (M3 life)

void CityStream::buildings_near(Vec2 p, f32 radius, std::vector<BuildingRef>& out) const {
  for (const auto& [k, t] : tiles_) {
    if (t->state != Tile::State::ready && t->state != Tile::State::collision) continue;
    for (std::size_t i = 0; i < t->features.buildings.size(); ++i) {
      const MapFeatures::Building& b = t->features.buildings[i];
      if (b.rings.empty() || b.rings[0].size() < 3) continue;
      f32 x0 = 1e9f, y0 = 1e9f, x1 = -1e9f, y1 = -1e9f;
      for (Vec2 q : b.rings[0]) { x0 = std::fmin(x0, q.x); y0 = std::fmin(y0, q.y); x1 = std::fmax(x1, q.x); y1 = std::fmax(y1, q.y); }
      if (p.x < x0 - radius || p.x > x1 + radius || p.y < y0 - radius || p.y > y1 + radius) continue;
      out.push_back({k, static_cast<u32>(i), &b});
    }
  }
}

void CityStream::ready_features(std::vector<const MapFeatures*>& out) const {
  for (const auto& [k, t] : tiles_)
    if (t->state == Tile::State::ready || t->state == Tile::State::collision) out.push_back(&t->features);
}

void CityStream::areas_near(Vec2 p, f32 radius, AreaKind kind, std::vector<AreaRef>& out) const {
  for (const auto& [k, t] : tiles_) {
    if (t->state != Tile::State::ready) continue;
    for (std::size_t i = 0; i < t->features.areas.size(); ++i) {
      const MapFeatures::Area& a = t->features.areas[i];
      if (a.kind != kind || a.rings.empty() || a.rings[0].size() < 3) continue;
      f32 x0 = 1e9f, y0 = 1e9f, x1 = -1e9f, y1 = -1e9f;
      for (Vec2 q : a.rings[0]) { x0 = std::fmin(x0, q.x); y0 = std::fmin(y0, q.y); x1 = std::fmax(x1, q.x); y1 = std::fmax(y1, q.y); }
      if (p.x < x0 - radius || p.x > x1 + radius || p.y < y0 - radius || p.y > y1 + radius) continue;
      out.push_back({k * 65536 + i, &a});
    }
  }
}

void CityStream::recollide(Tile&) {
  // the collision is the working set around the player: a new set without the entered building
  near_.dirty = true;
  near_.at_ms = -1e9;
}

void CityStream::exclude_building(u64 tile, i32 index) {
  if (tile == excl_tile_ && index == excl_index_) return;
  if (excl_tile_) {
    auto it = tiles_.find(excl_tile_);
    if (it != tiles_.end()) { it->second->skip = -1; recollide(*it->second); }
  }
  excl_tile_ = tile;
  excl_index_ = tile ? index : -1;
  if (tile) {
    auto it = tiles_.find(tile);
    if (it != tiles_.end()) { it->second->skip = index; recollide(*it->second); }
  }
}

bool CityStream::exclusion_done() const { return !near_.building && !near_.dirty; }

}  // namespace sc
