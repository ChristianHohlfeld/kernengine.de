// City jobs (see city_jobs.h).
#include "city_jobs.h"

#include <cstring>

#include "kern/core/jobs.h"
#include "kern/geo/mesh_data.h"
#include "mvt.h"
#include "wire.h"

namespace sc::cityjobs {

namespace {

// ---- writing a job's bytes: every value as it lies in memory, one after the other - each number's bytes
// go on the end (wire.h: one growth of the bytes each, as resizing them by its size does)
/// A list of numbers: its count (u32), then the bytes of all of them, in one go (one growth of the bytes).
void put_f32s(std::vector<u8>& b, const std::vector<f32>& v) {
  wire::put_u32(b, static_cast<u32>(v.size()));
  const std::size_t n = b.size();
  b.resize(n + v.size() * sizeof(f32));
  if (!v.empty()) std::memcpy(b.data() + n, v.data(), v.size() * sizeof(f32));
}
void put_u32s(std::vector<u8>& b, const std::vector<u32>& v) {
  wire::put_u32(b, static_cast<u32>(v.size()));
  const std::size_t n = b.size();
  b.resize(n + v.size() * sizeof(u32));
  if (!v.empty()) std::memcpy(b.data() + n, v.data(), v.size() * sizeof(u32));
}
void put_f64s(std::vector<u8>& b, const std::vector<f64>& v) {
  wire::put_u32(b, static_cast<u32>(v.size()));
  const std::size_t n = b.size();
  b.resize(n + v.size() * sizeof(f64));
  if (!v.empty()) std::memcpy(b.data() + n, v.data(), v.size() * sizeof(f64));
}
void put_points(std::vector<u8>& b, const std::vector<Vec2>& v) {
  wire::put_u32(b, static_cast<u32>(v.size()));
  for (Vec2 p : v) { wire::put_f32(b, p.x); wire::put_f32(b, p.y); }
}
void put_spots(std::vector<u8>& b, const std::vector<MapFeatures::Spot>& v) {
  wire::put_u32(b, static_cast<u32>(v.size()));
  for (const auto& sp : v) { wire::put_f32(b, sp.p.x); wire::put_f32(b, sp.p.y); wire::put_f32(b, sp.a); }
}

/// Reading them back: a cursor in the bytes - where it reads next, as an offset into the bytes
/// its functions are given (the same ones every time). A number that is not there - the bytes end
/// before it - is 0, and the reader is no longer ok; it then stays where it was (wire::Reader, over
/// the bytes a job is given).
struct Reader {
  std::size_t p = 0;
  bool ok = true;
  u8 get_u8(kern::Span<const u8> b) {
    if (b.size < p + 1) { ok = false; return 0; }
    const u8 v = b[p];
    p += 1;
    return v;
  }
  i32 get_i32(kern::Span<const u8> b) {
    if (b.size < p + 4) { ok = false; return 0; }
    wire::I32 x;
    std::memcpy(&x, b.data + p, sizeof x);
    p += 4;
    return x.v;
  }
  u32 get_u32(kern::Span<const u8> b) {
    if (b.size < p + 4) { ok = false; return 0; }
    wire::U32 x;
    std::memcpy(&x, b.data + p, sizeof x);
    p += 4;
    return x.v;
  }
  f32 get_f32(kern::Span<const u8> b) {
    if (b.size < p + 4) { ok = false; return 0; }
    wire::F32 x;
    std::memcpy(&x, b.data + p, sizeof x);
    p += 4;
    return x.v;
  }
  f64 get_f64(kern::Span<const u8> b) {
    if (b.size < p + 8) { ok = false; return 0; }
    wire::F64 x;
    std::memcpy(&x, b.data + p, sizeof x);
    p += 8;
    return x.v;
  }
  /// A list of numbers (put_f32s ...): its count, then their bytes, read in one go.
  void f32s(kern::Span<const u8> b, std::vector<f32>& v) {
    const u32 n = get_u32(b);
    if (!ok || b.size - p < static_cast<std::size_t>(n) * sizeof(f32)) { ok = false; return; }
    v.resize(n);
    if (n) std::memcpy(v.data(), b.data + p, n * sizeof(f32));
    p += n * sizeof(f32);
  }
  void u32s(kern::Span<const u8> b, std::vector<u32>& v) {
    const u32 n = get_u32(b);
    if (!ok || b.size - p < static_cast<std::size_t>(n) * sizeof(u32)) { ok = false; return; }
    v.resize(n);
    if (n) std::memcpy(v.data(), b.data + p, n * sizeof(u32));
    p += n * sizeof(u32);
  }
  void f64s(kern::Span<const u8> b, std::vector<f64>& v) {
    const u32 n = get_u32(b);
    if (!ok || b.size - p < static_cast<std::size_t>(n) * sizeof(f64)) { ok = false; return; }
    v.resize(n);
    if (n) std::memcpy(v.data(), b.data + p, n * sizeof(f64));
    p += n * sizeof(f64);
  }
  void points(kern::Span<const u8> b, std::vector<Vec2>& v) {
    const u32 n = get_u32(b);
    if (!ok || b.size - p < static_cast<std::size_t>(n) * 8) { ok = false; return; }
    v.resize(n);
    for (u32 i = 0; i < n; ++i) { v[i].x = get_f32(b); v[i].y = get_f32(b); }
  }
  /// False: a broken count (the reader may still be ok).
  bool spots(kern::Span<const u8> b, std::vector<MapFeatures::Spot>& v) {
    const u32 n = get_u32(b);
    if (!ok || n > 200000) return false;
    v.resize(n);
    for (auto& sp : v) { sp.p.x = get_f32(b); sp.p.y = get_f32(b); sp.a = get_f32(b); }
    return true;
  }
};

/// One chunk of the kBuildings output, as the job wrote it.
void read_one_chunk(kern::Span<const u8> in, Reader& r, kern::GeoChunk& c) {
  c.x = r.get_i32(in); c.y = r.get_i32(in); c.items = r.get_u32(in); c.triangles = r.get_u32(in);
  c.mesh.extra_size = r.get_u32(in);
  r.f32s(in, c.mesh.positions); r.f32s(in, c.mesh.normals); r.f32s(in, c.mesh.colors); r.f32s(in, c.mesh.uvs); r.f32s(in, c.mesh.extra); r.u32s(in, c.mesh.indices);
  c.welded = true;
}

// ---- kTile: MVT bytes -> features (mvt.cpp + tiles.cpp extract) ----
void run_tile(kern::Span<const u8> in, std::vector<u8>& out) {
  Reader r;
  TileKey key;
  key.z = r.get_i32(in);
  key.x = r.get_i32(in);
  key.y = r.get_i32(in);
  const f64 lon0 = r.get_f64(in), lat0 = r.get_f64(in);
  if (!r.ok) return;
  kern::GeoProjector proj;
  if (!proj.init(lon0, lat0)) return;
  static const std::vector<std::string> wanted = {"building", "transportation", "water", "waterway", "park", "landcover", "landuse"};
  mvt::Decoder d;
  d.begin(in, r.p, wanted);  // (the tile's bytes follow the header)
  d.step(in, 0);
  if (d.failed()) { wire::put_u8(out, 0); return; }
  MapFeatures f;
  extract(d.tile(), key, proj, f);
  wire::put_u8(out, 1);
  wire::put_u8(out, f.procedural ? 1 : 0);
  wire::put_u32(out, static_cast<u32>(f.buildings.size()));
  for (const auto& b : f.buildings) {
    wire::put_f32(out, b.base); wire::put_f32(out, b.top); wire::put_u32(out, b.seed);
    wire::put_u32(out, static_cast<u32>(b.rings.size()));
    for (const auto& ring : b.rings) put_points(out, ring);
  }
  wire::put_u32(out, static_cast<u32>(f.roads.size()));
  for (const auto& rd : f.roads) { wire::put_u8(out, static_cast<u8>(rd.cls)); wire::put_u8(out, rd.bridge ? 1 : 0); put_points(out, rd.points); }
  wire::put_u32(out, static_cast<u32>(f.areas.size()));
  for (const auto& a : f.areas) {
    wire::put_u8(out, static_cast<u8>(a.kind));
    wire::put_u32(out, static_cast<u32>(a.rings.size()));
    for (const auto& ring : a.rings) put_points(out, ring);
  }
  put_spots(out, f.trees);
  put_spots(out, f.lamps);
}

// ---- kBuildings: footprints -> welded chunk meshes (kern::BuildingBuilder + kern::weld) ----
void run_buildings(kern::Span<const u8> in, std::vector<u8>& out) {
  Reader r;
  kern::BuildingOptions o;
  o.chunk_size = r.get_f64(in);
  o.max_buildings = r.get_u32(in);
  o.cornice = r.get_u8(in) != 0;
  o.wall_records = r.get_u8(in) != 0;
  auto rgb = [&] { kern::Rgb c; c.r = r.get_f64(in); c.g = r.get_f64(in); c.b = r.get_f64(in); return c; };
  o.style.walls.resize(r.get_u32(in));
  for (auto& c : o.style.walls) c = rgb();
  o.style.roofs.resize(r.get_u32(in));
  for (auto& c : o.style.roofs) c = rgb();
  o.style.trim = rgb();
  o.style.trim_mix = r.get_f64(in);
  std::vector<f64> stream;
  r.f64s(in, stream);
  if (!r.ok || o.style.walls.empty() || o.style.roofs.empty()) return;
  kern::BuildingBuilder b;
  if (!b.begin(stream.data(), stream.size(), o)) return;
  while (!b.step(1e9)) {}
  auto& chunks = b.chunks();
  wire::put_u32(out, static_cast<u32>(chunks.size()));
  for (auto& c : chunks) {
    kern::weld(c.mesh);
    wire::put_i32(out, c.x); wire::put_i32(out, c.y); wire::put_u32(out, c.items); wire::put_u32(out, c.triangles);
    wire::put_u32(out, c.mesh.extra_size);
    put_f32s(out, c.mesh.positions); put_f32s(out, c.mesh.normals); put_f32s(out, c.mesh.colors); put_f32s(out, c.mesh.uvs); put_f32s(out, c.mesh.extra); put_u32s(out, c.mesh.indices);
  }
}

KERN_JOB(kTile, run_tile);
KERN_JOB(kBuildings, run_buildings);

}  // namespace

std::vector<u8> tile_input(const TileKey& key, f64 lon0, f64 lat0, kern::Span<const u8> mvt) {
  std::vector<u8> b;
  b.reserve(mvt.size + 28);
  wire::put_i32(b, key.z); wire::put_i32(b, key.x); wire::put_i32(b, key.y);
  wire::put_f64(b, lon0); wire::put_f64(b, lat0);
  b.insert(b.end(), mvt.begin(), mvt.end());
  return b;
}

bool read_features(const std::vector<u8>& output, MapFeatures& f) {
  Reader r;
  if (r.get_u8(output) != 1) return false;
  f.procedural = r.get_u8(output) != 0;
  f.buildings.resize(r.get_u32(output));
  for (auto& b : f.buildings) {
    b.base = r.get_f32(output); b.top = r.get_f32(output); b.seed = r.get_u32(output);
    b.rings.resize(r.get_u32(output));
    for (auto& ring : b.rings) r.points(output, ring);
    if (!r.ok) return false;
  }
  f.roads.resize(r.get_u32(output));
  for (auto& rd : f.roads) { rd.cls = static_cast<RoadClass>(r.get_u8(output)); rd.bridge = r.get_u8(output) != 0; r.points(output, rd.points); if (!r.ok) return false; }
  f.areas.resize(r.get_u32(output));
  for (auto& a : f.areas) {
    a.kind = static_cast<AreaKind>(r.get_u8(output));
    a.rings.resize(r.get_u32(output));
    for (auto& ring : a.rings) r.points(output, ring);
    if (!r.ok) return false;
  }
  if (!r.spots(output, f.trees) || !r.spots(output, f.lamps)) return false;
  return r.ok;
}

std::vector<u8> buildings_input(const kern::FootprintStream& footprints, const kern::BuildingOptions& o) {
  std::vector<u8> b;
  wire::put_f64(b, o.chunk_size); wire::put_u32(b, o.max_buildings); wire::put_u8(b, o.cornice ? 1 : 0); wire::put_u8(b, o.wall_records ? 1 : 0);
  auto rgb = [&](const kern::Rgb& c) { wire::put_f64(b, c.r); wire::put_f64(b, c.g); wire::put_f64(b, c.b); };
  wire::put_u32(b, static_cast<u32>(o.style.walls.size()));
  for (const auto& c : o.style.walls) rgb(c);
  wire::put_u32(b, static_cast<u32>(o.style.roofs.size()));
  for (const auto& c : o.style.roofs) rgb(c);
  rgb(o.style.trim);
  wire::put_f64(b, o.style.trim_mix);
  std::vector<f64> stream(footprints.data(), footprints.data() + footprints.size());
  put_f64s(b, stream);
  return b;
}

bool read_chunks(const std::vector<u8>& output, std::vector<kern::GeoChunk>& out) {
  Reader r;
  out.resize(r.get_u32(output));
  for (auto& c : out) {
    read_one_chunk(output, r, c);
    if (!r.ok) return false;
  }
  return r.ok;
}

u32 chunk_count(const std::vector<u8>& output) {
  // (the count the job put first: its four bytes as they lie in memory - little-endian, as on every
  // target of the engine)
  if (output.size() < 4) return 0;
  return u32{output[0]} | u32{output[1]} << 8 | u32{output[2]} << 16 | u32{output[3]} << 24;
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
