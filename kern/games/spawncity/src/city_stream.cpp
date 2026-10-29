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

}  // namespace

struct CityStream::Tile {
  enum class State : u8 { requested, decoding, building, roads, areas, collision, ready, failed } state = State::requested;
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
};

CityStream::CityStream() = default;
CityStream::~CityStream() = default;

void CityStream::start(kern::Renderer& renderer, kern::physics::World& world, f64 lon, f64 lat) {
  clear();
  r_ = &renderer;
  world_ = &world;
  proj_.init(lon, lat);
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
}

void CityStream::remove(Tile& t) {
  if (r_)
    for (u32 id : t.meshes) r_->remove(id);
  t.meshes.clear();
  if (world_ && t.body.valid() && world_->body_valid(t.body)) world_->destroy_body(t.body);
  t.body = {};
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
  lanes_.erase(std::remove_if(lanes_.begin(), lanes_.end(), [](const Lane& l) { return !l.alive; }), lanes_.end());
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
  auto t = std::make_unique<Tile>();
  t->procedural = true;
  t->key = TileKey{0, -1, -1};
  procedural_city(0, 0, 760, 20261029, t->features);
  t->state = Tile::State::building;
  finish_features(*t);
  tiles_[t->key.hash()] = std::move(t);
}

bool CityStream::ready_at(Vec3 p) const {
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

void CityStream::finish_features(Tile& t) {
  // Footprint and road streams for the engine's builders, lanes for the traffic.
  for (const MapFeatures::Building& b : t.features.buildings) {
    t.footprints.begin(b.seed, b.base, b.top);
    for (const auto& ring : b.rings) t.footprints.ring(ring.data(), static_cast<u32>(ring.size()));
  }
  for (const MapFeatures::Road& r : t.features.roads) {
    const f32 w = road_width(r.cls);
    const bool water = road_is_water(r.cls);
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
  for (const MapFeatures::Area& a : t.features.areas) {
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
      for (int k = 0; k < 3; ++k) mesh.push(p[k], {0, 0, 1}, c, {p[k].x * 0.1f, p[k].y * 0.1f});
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
  if (r_->upsert(id, in)) t.meshes.push_back(id);
  else r_->release_id(id);
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
      kern::RoadOptions ro;
      ro.center_x = t.center.x;
      ro.center_y = t.center.y;
      ro.radius = 4000;
      ro.palette = road_palette();
      kern::HeightSampler flat;
      t.road_builder.begin(t.roads.data(), t.roads.size(), ro, flat);
      t.state = S::roads;
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
        upload(t, chunks[t.upload_next], m, false);
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
      t.state = S::collision;
      return kern::now_ms() > deadline ? false : work(t, deadline);
    case S::collision:
      if (!build_collision(t, deadline)) return false;
      t.state = S::ready;
      t.features.areas.clear();
      t.features.roads.clear();
      return true;
  }
  return true;
}

void CityStream::update(Vec3 focus, f64 budget_ms) {
  if (!r_ || !world_) return;
  const f64 start = kern::now_ms(), deadline = start + budget_ms;
  request_around(focus);
  unload_far(focus);
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
  stats_ = {};
  for (auto& [k, t] : tiles_) {
    if (t->state == Tile::State::ready) ++stats_.tiles_ready;
    else if (t->state == Tile::State::failed) ++stats_.tiles_failed;
    else ++stats_.tiles_loading;
    stats_.buildings += static_cast<u32>(t->features.buildings.size());
    stats_.colliders += t->colliders;
    stats_.triangles += t->triangles;
    stats_.meshes += static_cast<u32>(t->meshes.size());
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

}  // namespace sc
