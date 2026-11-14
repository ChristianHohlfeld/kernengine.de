// City jobs (see city_jobs.h).
#include "city_jobs.h"

#include <cstring>

#include "kern/core/jobs.h"
#include "kern/geo/mesh_data.h"
#include "mvt.h"

namespace sc::cityjobs {

namespace {

// ---- writing a job's bytes: every value as it lies in memory, one after the other
template <class T> void put(std::vector<u8>& b, T v) {
  const std::size_t n = b.size();
  b.resize(n + sizeof(T));
  std::memcpy(b.data() + n, &v, sizeof(T));
}
template <class T> void put_array(std::vector<u8>& b, const std::vector<T>& v) {
  put<u32>(b, static_cast<u32>(v.size()));
  const std::size_t n = b.size();
  b.resize(n + v.size() * sizeof(T));
  if (!v.empty()) std::memcpy(b.data() + n, v.data(), v.size() * sizeof(T));
}
void put_points(std::vector<u8>& b, const std::vector<Vec2>& v) {
  put<u32>(b, static_cast<u32>(v.size()));
  for (Vec2 p : v) { put<f32>(b, p.x); put<f32>(b, p.y); }
}
void put_spots(std::vector<u8>& b, const std::vector<MapFeatures::Spot>& v) {
  put<u32>(b, static_cast<u32>(v.size()));
  for (const auto& sp : v) { put<f32>(b, sp.p.x); put<f32>(b, sp.p.y); put<f32>(b, sp.a); }
}

/// Reading them back: a cursor in the bytes - where it reads next, as an offset into the bytes
/// its functions are given (the same ones every time).
struct Reader {
  std::size_t p = 0;
  bool ok = true;
  template <class T> T get(kern::Span<const u8> b) {
    T v{};
    if (b.size < p + sizeof(T)) { ok = false; return v; }
    std::memcpy(&v, b.data + p, sizeof(T));
    p += sizeof(T);
    return v;
  }
  template <class T> void array(kern::Span<const u8> b, std::vector<T>& v) {
    const u32 n = get<u32>(b);
    if (!ok || b.size - p < static_cast<std::size_t>(n) * sizeof(T)) { ok = false; return; }
    v.resize(n);
    if (n) std::memcpy(v.data(), b.data + p, n * sizeof(T));
    p += n * sizeof(T);
  }
  void points(kern::Span<const u8> b, std::vector<Vec2>& v) {
    const u32 n = get<u32>(b);
    if (!ok || b.size - p < static_cast<std::size_t>(n) * 8) { ok = false; return; }
    v.resize(n);
    for (u32 i = 0; i < n; ++i) { v[i].x = get<f32>(b); v[i].y = get<f32>(b); }
  }
  /// False: a broken count (the reader may still be ok).
  bool spots(kern::Span<const u8> b, std::vector<MapFeatures::Spot>& v) {
    const u32 n = get<u32>(b);
    if (!ok || n > 200000) return false;
    v.resize(n);
    for (auto& sp : v) { sp.p.x = get<f32>(b); sp.p.y = get<f32>(b); sp.a = get<f32>(b); }
    return true;
  }
};

/// One chunk of the kBuildings output, as the job wrote it.
void read_one_chunk(kern::Span<const u8> in, Reader& r, kern::GeoChunk& c) {
  c.x = r.get<i32>(in); c.y = r.get<i32>(in); c.items = r.get<u32>(in); c.triangles = r.get<u32>(in);
  c.mesh.extra_size = r.get<u32>(in);
  r.array(in, c.mesh.positions); r.array(in, c.mesh.normals); r.array(in, c.mesh.colors); r.array(in, c.mesh.uvs); r.array(in, c.mesh.extra); r.array(in, c.mesh.indices);
  c.welded = true;
}

// ---- kTile: MVT bytes -> features (mvt.cpp + tiles.cpp extract) ----
void run_tile(kern::Span<const u8> in, std::vector<u8>& out) {
  Reader r;
  TileKey key;
  key.z = r.get<i32>(in);
  key.x = r.get<i32>(in);
  key.y = r.get<i32>(in);
  const f64 lon0 = r.get<f64>(in), lat0 = r.get<f64>(in);
  if (!r.ok) return;
  kern::GeoProjector proj;
  if (!proj.init(lon0, lat0)) return;
  static const std::vector<std::string> wanted = {"building", "transportation", "water", "waterway", "park", "landcover", "landuse"};
  mvt::Decoder d;
  d.begin(in, r.p, wanted);  // (the tile's bytes follow the header)
  d.step(in, 0);
  if (d.failed()) { put<u8>(out, 0); return; }
  MapFeatures f;
  extract(d.tile(), key, proj, f);
  put<u8>(out, 1);
  put<u8>(out, f.procedural ? 1 : 0);
  put<u32>(out, static_cast<u32>(f.buildings.size()));
  for (const auto& b : f.buildings) {
    put<f32>(out, b.base); put<f32>(out, b.top); put<u32>(out, b.seed);
    put<u32>(out, static_cast<u32>(b.rings.size()));
    for (const auto& ring : b.rings) put_points(out, ring);
  }
  put<u32>(out, static_cast<u32>(f.roads.size()));
  for (const auto& rd : f.roads) { put<u8>(out, static_cast<u8>(rd.cls)); put<u8>(out, rd.bridge ? 1 : 0); put_points(out, rd.points); }
  put<u32>(out, static_cast<u32>(f.areas.size()));
  for (const auto& a : f.areas) {
    put<u8>(out, static_cast<u8>(a.kind));
    put<u32>(out, static_cast<u32>(a.rings.size()));
    for (const auto& ring : a.rings) put_points(out, ring);
  }
  put_spots(out, f.trees);
  put_spots(out, f.lamps);
}

// ---- kBuildings: footprints -> welded chunk meshes (kern::BuildingBuilder + kern::weld) ----
void run_buildings(kern::Span<const u8> in, std::vector<u8>& out) {
  Reader r;
  kern::BuildingOptions o;
  o.chunk_size = r.get<f64>(in);
  o.max_buildings = r.get<u32>(in);
  o.cornice = r.get<u8>(in) != 0;
  o.wall_records = r.get<u8>(in) != 0;
  auto rgb = [&] { kern::Rgb c; c.r = r.get<f64>(in); c.g = r.get<f64>(in); c.b = r.get<f64>(in); return c; };
  o.style.walls.resize(r.get<u32>(in));
  for (auto& c : o.style.walls) c = rgb();
  o.style.roofs.resize(r.get<u32>(in));
  for (auto& c : o.style.roofs) c = rgb();
  o.style.trim = rgb();
  o.style.trim_mix = r.get<f64>(in);
  std::vector<f64> stream;
  r.array(in, stream);
  if (!r.ok || o.style.walls.empty() || o.style.roofs.empty()) return;
  kern::BuildingBuilder b;
  if (!b.begin(stream.data(), stream.size(), o)) return;
  while (!b.step(1e9)) {}
  auto& chunks = b.chunks();
  put<u32>(out, static_cast<u32>(chunks.size()));
  for (auto& c : chunks) {
    kern::weld(c.mesh);
    put<i32>(out, c.x); put<i32>(out, c.y); put<u32>(out, c.items); put<u32>(out, c.triangles);
    put<u32>(out, c.mesh.extra_size);
    put_array(out, c.mesh.positions); put_array(out, c.mesh.normals); put_array(out, c.mesh.colors); put_array(out, c.mesh.uvs); put_array(out, c.mesh.extra); put_array(out, c.mesh.indices);
  }
}

KERN_JOB(kTile, run_tile);
KERN_JOB(kBuildings, run_buildings);

}  // namespace

std::vector<u8> tile_input(const TileKey& key, f64 lon0, f64 lat0, kern::Span<const u8> mvt) {
  std::vector<u8> b;
  b.reserve(mvt.size + 28);
  put<i32>(b, key.z); put<i32>(b, key.x); put<i32>(b, key.y);
  put<f64>(b, lon0); put<f64>(b, lat0);
  b.insert(b.end(), mvt.begin(), mvt.end());
  return b;
}

bool read_features(const std::vector<u8>& output, MapFeatures& f) {
  const kern::Span<const u8> in(output);
  Reader r;
  if (r.get<u8>(in) != 1) return false;
  f.procedural = r.get<u8>(in) != 0;
  f.buildings.resize(r.get<u32>(in));
  for (auto& b : f.buildings) {
    b.base = r.get<f32>(in); b.top = r.get<f32>(in); b.seed = r.get<u32>(in);
    b.rings.resize(r.get<u32>(in));
    for (auto& ring : b.rings) r.points(in, ring);
    if (!r.ok) return false;
  }
  f.roads.resize(r.get<u32>(in));
  for (auto& rd : f.roads) { rd.cls = static_cast<RoadClass>(r.get<u8>(in)); rd.bridge = r.get<u8>(in) != 0; r.points(in, rd.points); if (!r.ok) return false; }
  f.areas.resize(r.get<u32>(in));
  for (auto& a : f.areas) {
    a.kind = static_cast<AreaKind>(r.get<u8>(in));
    a.rings.resize(r.get<u32>(in));
    for (auto& ring : a.rings) r.points(in, ring);
    if (!r.ok) return false;
  }
  if (!r.spots(in, f.trees) || !r.spots(in, f.lamps)) return false;
  return r.ok;
}

std::vector<u8> buildings_input(const kern::FootprintStream& footprints, const kern::BuildingOptions& o) {
  std::vector<u8> b;
  put<f64>(b, o.chunk_size); put<u32>(b, o.max_buildings); put<u8>(b, o.cornice ? 1 : 0); put<u8>(b, o.wall_records ? 1 : 0);
  auto rgb = [&](const kern::Rgb& c) { put<f64>(b, c.r); put<f64>(b, c.g); put<f64>(b, c.b); };
  put<u32>(b, static_cast<u32>(o.style.walls.size()));
  for (const auto& c : o.style.walls) rgb(c);
  put<u32>(b, static_cast<u32>(o.style.roofs.size()));
  for (const auto& c : o.style.roofs) rgb(c);
  rgb(o.style.trim);
  put<f64>(b, o.style.trim_mix);
  std::vector<f64> stream(footprints.data(), footprints.data() + footprints.size());
  put_array(b, stream);
  return b;
}

bool read_chunks(const std::vector<u8>& output, std::vector<kern::GeoChunk>& out) {
  const kern::Span<const u8> in(output);
  Reader r;
  out.resize(r.get<u32>(in));
  for (auto& c : out) {
    read_one_chunk(in, r, c);
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
  Reader r{at};
  read_one_chunk(output, r, c);
  if (!r.ok) return false;
  at = r.p;
  return true;
}

}  // namespace sc::cityjobs
