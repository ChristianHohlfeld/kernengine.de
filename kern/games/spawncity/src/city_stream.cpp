// Streamed city (see city_stream.h).
#include "city_stream.h"

#include <algorithm>
#include <cstdio>

#include "kern/core/base.h"
#include "kern/geo/earcut.h"

namespace sc {

namespace {

kern::BuildingStyle konstanz_style() {
  // Old-town plaster facades and tiled roofs (colours, not data: the shapes and heights are OSM's).
  kern::BuildingStyle s;
  s.walls = {{0.86, 0.82, 0.74}, {0.84, 0.74, 0.58}, {0.80, 0.66, 0.58}, {0.74, 0.76, 0.74}, {0.90, 0.88, 0.84},
             {0.70, 0.78, 0.72}, {0.82, 0.70, 0.48}, {0.78, 0.72, 0.66}};
  s.roofs = {{0.55, 0.27, 0.18}, {0.47, 0.24, 0.17}, {0.33, 0.30, 0.30}, {0.42, 0.30, 0.24}};
  s.trim = {0.95, 0.93, 0.89};
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
  terrain_.clear();
  terrain_changes_.clear();
}

void CityStream::remove(Tile& t) {
  if (r_)
    for (const std::vector<u32>* list : {&t.meshes, &t.lay_meshes, &t.old_lay})
      for (u32 id : *list) r_->remove(id);
  t.meshes.clear();
  t.lay_meshes.clear();
  t.old_lay.clear();
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
  requests_.clear();
  return s + "]";
}

void CityStream::provide(const TileKey& key, const u8* bytes, std::size_t length) {
  Tile* t = tile(key);
  if (!t || t->state != Tile::State::requested) return;
  t->bytes.assign(bytes, bytes + length);
  static const std::vector<std::string> wanted = {"building", "transportation", "water", "waterway", "park", "landcover", "landuse"};
  t->decoder.begin(t->bytes.data(), t->bytes.size(), wanted);
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
  if (offline_) {
    for (const auto& [k, t] : tiles_)
      if (t->procedural) return t->state == Tile::State::ready;
    return false;
  }
  f64 lon, lat;
  unproject(proj_, p.x, p.y, lon, lat);
  const TileKey c = tile_at(lon, lat);
  auto it = tiles_.find(c.hash());
  return it != tiles_.end() && (it->second->state == Tile::State::ready || it->second->state == Tile::State::failed);
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
  for (MapFeatures::Building& b : t.features.buildings) {
    // terrain_craters.mjs buildingGroundBase: a house stands on the lowest ground of its outline.
    if (!b.rings.empty()) {
      const f32 g = terrain_.footprint_base(b.rings[0]);
      b.base += g;
      b.top += g;
    }
    t.footprints.begin(b.seed, b.base, b.top);
    for (const auto& ring : b.rings) t.footprints.ring(ring.data(), static_cast<u32>(ring.size()));
  }
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
  kern::BuildingOptions bo;
  bo.chunk_size = 320;
  bo.max_buildings = 12000;
  bo.style = konstanz_style();
  t.buildings.begin(t.footprints.data(), t.footprints.size(), bo);
  t.state = Tile::State::building;
}

void CityStream::upload(Tile& t, kern::GeoChunk& chunk, const kern::Material& material, bool walls) {
  if (chunk.mesh.vertex_count() == 0) return;
  kern::weld(chunk.mesh);
  const u32 id = r_->allocate_id();
  kern::MeshInput in = chunk.mesh.as_input(walls ? 0 : kern::kMaxExtensionAttributes);
  in.material = material;
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
  ro.center_x = t.center.x;
  ro.center_y = t.center.y;
  ro.radius = 4000;
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

bool CityStream::build_collision(Tile& t, f64 deadline) {
  if (!t.body.valid()) {
    kern::physics::BodySettings bs;
    bs.type = kern::physics::BodyType::fixed;
    t.body = world_->create_body(bs);
  }
  kern::Earcut earcut;
  std::vector<f64> xy;
  std::vector<u32> holes, tris, idx;
  std::vector<Vec3> pts;
  while (t.collide_next < t.features.buildings.size()) {
    if (static_cast<i32>(t.collide_next) == t.skip) { ++t.collide_next; continue; }  // an entered interior (M3 life)
    const MapFeatures::Building& b = t.features.buildings[t.collide_next++];
    xy.clear(); holes.clear(); tris.clear(); idx.clear(); pts.clear();
    std::vector<std::vector<Vec2>> rings = b.rings;
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
      if (world_->create_shape(t.body, ss).valid()) ++t.colliders;
    }
    if (kern::now_ms() > deadline) return false;
  }
  return true;
}

bool CityStream::work(Tile& t, f64 deadline) {
  using S = Tile::State;
  switch (t.state) {
    case S::requested:
    case S::ready:
    case S::failed: return true;
    case S::decoding:
      if (!t.decoder.step(std::max(0.5, deadline - kern::now_ms()))) return false;
      if (t.decoder.failed()) { t.state = S::failed; return true; }
      extract(t.decoder.tile(), t.key, proj_, t.features);
      t.decoder = mvt::Decoder{};
      t.bytes.clear();
      t.bytes.shrink_to_fit();
      ++real_tiles_;
      feed_terrain(t);
      t.state = S::terrain_wait;
      return kern::now_ms() > deadline ? false : work(t, deadline);
    case S::terrain_wait:
      // Buildings and streets stand on the terrain: wait until its shape is decided.
      if (!terrain_.settled()) return true;
      finish_features(t);
      return kern::now_ms() > deadline ? false : work(t, deadline);
    case S::building: {
      if (!t.buildings.step(std::max(0.5, deadline - kern::now_ms()))) return false;
      // One chunk mesh at a time (welding and the picking BVH take a few milliseconds each).
      kern::Material m;
      m.roughness = 0.82f;
      auto& chunks = t.buildings.chunks();
      while (t.upload_next < chunks.size()) {
        upload(t, chunks[t.upload_next], m, true);
        chunks[t.upload_next++].mesh = kern::MeshData{};
        if (kern::now_ms() > deadline) return false;
      }
      chunks.clear();
      chunks.shrink_to_fit();
      t.upload_next = 0;
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
      t.state = S::collision;
      return kern::now_ms() > deadline ? false : work(t, deadline);
    case S::collision:
      if (!build_collision(t, deadline)) return false;
      t.state = S::ready;
      t.features.roads.clear();  // areas stay for re-laying them on a changed ground
      return true;
  }
  return true;
}

void CityStream::update(Vec3 focus, f64 budget_ms) {
  if (!r_ || !world_) return;
  const f64 start = kern::now_ms(), deadline = start + budget_ms;
  request_around(focus);
  unload_far(focus);
  terrain_.update(focus, anchor, std::fmax(1.0, budget_ms * 0.45));
  terrain_.take_changes(terrain_changes_);
  for (const kern::Vec4& r : terrain_changes_) relay(r);
  terrain_changes_.clear();
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
  // Interiors (M3 life): a tile whose collision leaves a building out is rebuilt beside the old
  // body (at least 1.5 ms a frame), then the old body goes.
  for (auto& [k, t] : tiles_) {
    if (!t->recollide || t->state != Tile::State::ready) continue;
    if (!build_collision(*t, std::fmax(deadline, kern::now_ms() + 1.5))) break;
    if (t->old_body.valid() && world_->body_valid(t->old_body)) world_->destroy_body(t->old_body);
    t->old_body = {};
    t->recollide = false;
  }
  stats_ = {};
  for (auto& [k, t] : tiles_) {
    if (t->state == Tile::State::ready) ++stats_.tiles_ready;
    else if (t->state == Tile::State::failed) ++stats_.tiles_failed;
    else ++stats_.tiles_loading;
    stats_.buildings += static_cast<u32>(t->features.buildings.size());
    stats_.colliders += t->colliders;
    stats_.triangles += t->triangles;
    stats_.meshes += static_cast<u32>(t->meshes.size() + t->lay_meshes.size());
  }
  stats_.last_slice_ms = kern::now_ms() - start;
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

void CityStream::recollide(Tile& t) {
  if (t.state != Tile::State::ready) return;  // not built yet: the skip applies as it is built
  if (t.recollide) {
    // A rebuild in progress: drop the partial body, keep the complete old one.
    if (t.body.valid() && world_->body_valid(t.body)) world_->destroy_body(t.body);
  } else {
    t.old_body = t.body;
  }
  t.body = {};
  t.collide_next = 0;
  t.colliders = 0;
  t.recollide = true;
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

bool CityStream::exclusion_done() const {
  for (const auto& [k, t] : tiles_)
    if (t->recollide) return false;
  return true;
}

}  // namespace sc
