// City jobs (see city_jobs.h).
#include "city_jobs.h"

#include <cstring>

#include "kern/core/jobs.h"
#include "kern/geo/mesh_data.h"
#include "mvt.h"

namespace sc::cityjobs {

namespace {

struct Writer {
  std::vector<u8>& b;
  template <class T> void put(T v) { const std::size_t n = b.size(); b.resize(n + sizeof(T)); std::memcpy(b.data() + n, &v, sizeof(T)); }
  template <class T> void array(const std::vector<T>& v) {
    put<u32>(static_cast<u32>(v.size()));
    const std::size_t n = b.size();
    b.resize(n + v.size() * sizeof(T));
    if (!v.empty()) std::memcpy(b.data() + n, v.data(), v.size() * sizeof(T));
  }
  void points(const std::vector<Vec2>& v) {
    put<u32>(static_cast<u32>(v.size()));
    for (Vec2 p : v) { put<f32>(p.x); put<f32>(p.y); }
  }
};

struct Reader {
  const u8* p;
  const u8* end;
  bool ok = true;
  template <class T> T get() {
    T v{};
    if (end - p < static_cast<std::ptrdiff_t>(sizeof(T))) { ok = false; return v; }
    std::memcpy(&v, p, sizeof(T));
    p += sizeof(T);
    return v;
  }
  template <class T> void array(std::vector<T>& v) {
    const u32 n = get<u32>();
    if (!ok || static_cast<std::size_t>(end - p) < static_cast<std::size_t>(n) * sizeof(T)) { ok = false; return; }
    v.resize(n);
    if (n) std::memcpy(v.data(), p, n * sizeof(T));
    p += n * sizeof(T);
  }
  void points(std::vector<Vec2>& v) {
    const u32 n = get<u32>();
    if (!ok || static_cast<std::size_t>(end - p) < static_cast<std::size_t>(n) * 8) { ok = false; return; }
    v.resize(n);
    for (u32 i = 0; i < n; ++i) { v[i].x = get<f32>(); v[i].y = get<f32>(); }
  }
};

// ---- kTile: MVT bytes -> features (mvt.cpp + tiles.cpp extract) ----
void run_tile(kern::Span<const u8> in, std::vector<u8>& out) {
  Reader r{in.data, in.data + in.size};
  TileKey key;
  key.z = r.get<i32>();
  key.x = r.get<i32>();
  key.y = r.get<i32>();
  const f64 lon0 = r.get<f64>(), lat0 = r.get<f64>();
  if (!r.ok) return;
  kern::GeoProjector proj;
  if (!proj.init(lon0, lat0)) return;
  static const std::vector<std::string> wanted = {"building", "transportation", "water", "waterway", "park", "landcover", "landuse"};
  mvt::Decoder d;
  d.begin(r.p, static_cast<std::size_t>(r.end - r.p), wanted);
  d.step(0);
  Writer w{out};
  if (d.failed()) { w.put<u8>(0); return; }
  MapFeatures f;
  extract(d.tile(), key, proj, f);
  w.put<u8>(1);
  w.put<u8>(f.procedural ? 1 : 0);
  w.put<u32>(static_cast<u32>(f.buildings.size()));
  for (const auto& b : f.buildings) {
    w.put<f32>(b.base); w.put<f32>(b.top); w.put<u32>(b.seed);
    w.put<u32>(static_cast<u32>(b.rings.size()));
    for (const auto& ring : b.rings) w.points(ring);
  }
  w.put<u32>(static_cast<u32>(f.roads.size()));
  for (const auto& rd : f.roads) { w.put<u8>(static_cast<u8>(rd.cls)); w.put<u8>(rd.bridge ? 1 : 0); w.points(rd.points); }
  w.put<u32>(static_cast<u32>(f.areas.size()));
  for (const auto& a : f.areas) {
    w.put<u8>(static_cast<u8>(a.kind));
    w.put<u32>(static_cast<u32>(a.rings.size()));
    for (const auto& ring : a.rings) w.points(ring);
  }
  for (const auto* list : {&f.trees, &f.lamps}) {
    w.put<u32>(static_cast<u32>(list->size()));
    for (const auto& sp : *list) { w.put<f32>(sp.p.x); w.put<f32>(sp.p.y); w.put<f32>(sp.a); }
  }
}

// ---- kBuildings: footprints -> welded chunk meshes (kern::BuildingBuilder + kern::weld) ----
void run_buildings(kern::Span<const u8> in, std::vector<u8>& out) {
  Reader r{in.data, in.data + in.size};
  kern::BuildingOptions o;
  o.chunk_size = r.get<f64>();
  o.max_buildings = r.get<u32>();
  o.cornice = r.get<u8>() != 0;
  o.wall_records = r.get<u8>() != 0;
  auto rgb = [&] { kern::Rgb c; c.r = r.get<f64>(); c.g = r.get<f64>(); c.b = r.get<f64>(); return c; };
  o.style.walls.resize(r.get<u32>());
  for (auto& c : o.style.walls) c = rgb();
  o.style.roofs.resize(r.get<u32>());
  for (auto& c : o.style.roofs) c = rgb();
  o.style.trim = rgb();
  o.style.trim_mix = r.get<f64>();
  std::vector<f64> stream;
  r.array(stream);
  if (!r.ok || o.style.walls.empty() || o.style.roofs.empty()) return;
  kern::BuildingBuilder b;
  if (!b.begin(stream.data(), stream.size(), o)) return;
  while (!b.step(1e9)) {}
  Writer w{out};
  auto& chunks = b.chunks();
  w.put<u32>(static_cast<u32>(chunks.size()));
  for (auto& c : chunks) {
    kern::weld(c.mesh);
    w.put<i32>(c.x); w.put<i32>(c.y); w.put<u32>(c.items); w.put<u32>(c.triangles);
    w.put<u32>(c.mesh.extra_size);
    w.array(c.mesh.positions); w.array(c.mesh.normals); w.array(c.mesh.colors); w.array(c.mesh.uvs); w.array(c.mesh.extra); w.array(c.mesh.indices);
  }
}

KERN_JOB(kTile, run_tile);
KERN_JOB(kBuildings, run_buildings);

}  // namespace

std::vector<u8> tile_input(const TileKey& key, f64 lon0, f64 lat0, const u8* mvt, std::size_t length) {
  std::vector<u8> b;
  b.reserve(length + 28);
  Writer w{b};
  w.put<i32>(key.z); w.put<i32>(key.x); w.put<i32>(key.y);
  w.put<f64>(lon0); w.put<f64>(lat0);
  b.insert(b.end(), mvt, mvt + length);
  return b;
}

bool read_features(const std::vector<u8>& output, MapFeatures& f) {
  Reader r{output.data(), output.data() + output.size()};
  if (r.get<u8>() != 1) return false;
  f.procedural = r.get<u8>() != 0;
  f.buildings.resize(r.get<u32>());
  for (auto& b : f.buildings) {
    b.base = r.get<f32>(); b.top = r.get<f32>(); b.seed = r.get<u32>();
    b.rings.resize(r.get<u32>());
    for (auto& ring : b.rings) r.points(ring);
    if (!r.ok) return false;
  }
  f.roads.resize(r.get<u32>());
  for (auto& rd : f.roads) { rd.cls = static_cast<RoadClass>(r.get<u8>()); rd.bridge = r.get<u8>() != 0; r.points(rd.points); if (!r.ok) return false; }
  f.areas.resize(r.get<u32>());
  for (auto& a : f.areas) {
    a.kind = static_cast<AreaKind>(r.get<u8>());
    a.rings.resize(r.get<u32>());
    for (auto& ring : a.rings) r.points(ring);
    if (!r.ok) return false;
  }
  for (auto* list : {&f.trees, &f.lamps}) {
    const u32 n = r.get<u32>();
    if (!r.ok || n > 200000) return false;
    list->resize(n);
    for (auto& sp : *list) { sp.p.x = r.get<f32>(); sp.p.y = r.get<f32>(); sp.a = r.get<f32>(); }
  }
  return r.ok;
}

std::vector<u8> buildings_input(const kern::FootprintStream& footprints, const kern::BuildingOptions& o) {
  std::vector<u8> b;
  Writer w{b};
  w.put<f64>(o.chunk_size); w.put<u32>(o.max_buildings); w.put<u8>(o.cornice ? 1 : 0); w.put<u8>(o.wall_records ? 1 : 0);
  auto rgb = [&](const kern::Rgb& c) { w.put<f64>(c.r); w.put<f64>(c.g); w.put<f64>(c.b); };
  w.put<u32>(static_cast<u32>(o.style.walls.size()));
  for (const auto& c : o.style.walls) rgb(c);
  w.put<u32>(static_cast<u32>(o.style.roofs.size()));
  for (const auto& c : o.style.roofs) rgb(c);
  rgb(o.style.trim);
  w.put<f64>(o.style.trim_mix);
  std::vector<f64> stream(footprints.data(), footprints.data() + footprints.size());
  w.array(stream);
  return b;
}

bool read_chunks(const std::vector<u8>& output, std::vector<kern::GeoChunk>& out) {
  Reader r{output.data(), output.data() + output.size()};
  out.resize(r.get<u32>());
  for (auto& c : out) {
    c.x = r.get<i32>(); c.y = r.get<i32>(); c.items = r.get<u32>(); c.triangles = r.get<u32>();
    c.mesh.extra_size = r.get<u32>();
    r.array(c.mesh.positions); r.array(c.mesh.normals); r.array(c.mesh.colors); r.array(c.mesh.uvs); r.array(c.mesh.extra); r.array(c.mesh.indices);
    c.welded = true;
    if (!r.ok) return false;
  }
  return r.ok;
}

u32 chunk_count(const std::vector<u8>& output) {
  u32 n = 0;
  if (output.size() >= 4) std::memcpy(&n, output.data(), 4);
  return n;
}

bool read_chunk(const std::vector<u8>& output, std::size_t& at, kern::GeoChunk& c) {
  if (at >= output.size()) return false;
  Reader r{output.data() + at, output.data() + output.size()};
  c.x = r.get<i32>(); c.y = r.get<i32>(); c.items = r.get<u32>(); c.triangles = r.get<u32>();
  c.mesh.extra_size = r.get<u32>();
  r.array(c.mesh.positions); r.array(c.mesh.normals); r.array(c.mesh.colors); r.array(c.mesh.uvs); r.array(c.mesh.extra); r.array(c.mesh.indices);
  c.welded = true;
  if (!r.ok) return false;
  at = static_cast<std::size_t>(r.p - output.data());
  return true;
}

}  // namespace sc::cityjobs
