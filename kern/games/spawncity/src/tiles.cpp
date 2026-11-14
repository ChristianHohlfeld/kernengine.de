// Map features from vector tiles and the procedural stand-in city (see tiles.h).
#include <array>
#include "tiles.h"

#include <cmath>
#include <cstring>
#include <string_view>

#include "kern/core/map.h"

namespace sc {

namespace {
constexpr f64 kPiD = 3.14159265358979323846;
}

TileKey tile_at(f64 lon, f64 lat, i32 z) {
  const f64 n = std::ldexp(1.0, z);
  const f64 lr = lat * kPiD / 180;
  TileKey t;
  t.z = z;
  t.x = static_cast<i32>(std::floor((lon + 180) / 360 * n));
  t.y = static_cast<i32>(std::floor((1 - std::asinh(std::tan(lr)) / kPiD) / 2 * n));
  return t;
}

void tile_lonlat(const TileKey& t, f64 fx, f64 fy, f64& lon, f64& lat) {
  const f64 n = std::ldexp(1.0, t.z);
  lon = (t.x + fx) / n * 360 - 180;
  lat = std::atan(std::sinh(kPiD * (1 - 2 * (t.y + fy) / n))) * 180 / kPiD;
}

void unproject(const kern::GeoProjector& p, f64 x, f64 y, f64& lon, f64& lat) {
  lon = p.lon0 + x / (kern::GeoProjector::kEarthRadius * p.cos_lat) / kern::GeoProjector::kRad;
  lat = p.lat0 + y / kern::GeoProjector::kEarthRadius / kern::GeoProjector::kRad;
}

f32 road_width(RoadClass c) {
  switch (c) {
    case RoadClass::motorway: return 14;
    case RoadClass::trunk: return 12;
    case RoadClass::primary: return 11;
    case RoadClass::secondary: return 9;
    case RoadClass::tertiary: return 8;
    case RoadClass::minor: return 6.5f;
    case RoadClass::service: return 4.2f;
    case RoadClass::track: return 3;
    case RoadClass::path: return 2.2f;
    case RoadClass::pedestrian: return 3.6f;
    case RoadClass::river: return 18;
    case RoadClass::canal: return 12;
    case RoadClass::stream: return 4.5f;
  }
  return 6;
}
bool road_is_water(RoadClass c) { return c == RoadClass::river || c == RoadClass::canal || c == RoadClass::stream; }
bool road_drivable(RoadClass c) { return static_cast<u8>(c) <= static_cast<u8>(RoadClass::service); }

namespace {

bool parse_road(std::string_view cls, RoadClass& out) {
  struct M { const char* n; RoadClass c; };
  static const M map[] = {{"motorway", RoadClass::motorway}, {"trunk", RoadClass::trunk},       {"primary", RoadClass::primary},
                          {"secondary", RoadClass::secondary}, {"tertiary", RoadClass::tertiary}, {"minor", RoadClass::minor},
                          {"service", RoadClass::service},   {"track", RoadClass::track},       {"path", RoadClass::path},
                          {"pedestrian", RoadClass::pedestrian}, {"raceway", RoadClass::tertiary}, {"busway", RoadClass::minor}};
  for (const M& m : map)
    if (cls == m.n) { out = m.c; return true; }
  return false;
}

/// Groups MVT polygon parts into polygons (exterior ring: positive area in tile space, y down).
void polygons(const mvt::Feature& f, std::vector<std::vector<std::vector<Vec2>>>& out) {
  out.clear();
  for (const auto& part : f.parts) {
    if (part.size() < 3) continue;
    const f64 a = mvt::ring_area(part);
    if (a > 0 || out.empty()) out.emplace_back();
    out.back().push_back(part);
  }
}

/// Tile units -> local metres. Longitude is linear in tile x; latitude is web-mercator in tile y,
/// interpolated quadratically through the tile's top, middle and bottom (sub-centimetre over a
/// z14 tile) so a dense tile projects without a trig call per vertex.
struct Projector {
  f64 x0, x1, y0, ym, y1, inv_extent;
  Projector(const TileKey& key, const kern::GeoProjector& proj, f64 extent) : inv_extent(1.0 / extent) {
    f64 lon, lat, x;
    tile_lonlat(key, 0, 0, lon, lat);
    proj.project(lon, lat, x, y0);
    x0 = x;
    tile_lonlat(key, 1, 0.5, lon, lat);
    proj.project(lon, lat, x, ym);
    x1 = x;
    tile_lonlat(key, 1, 1, lon, lat);
    proj.project(lon, lat, x, y1);
  }
  Vec2 operator()(Vec2 t) const {
    const f64 fx = t.x * inv_extent, fy = t.y * inv_extent;
    // Lagrange through (0, y0), (0.5, ym), (1, y1).
    const f64 y = y0 * (2 * fy - 1) * (fy - 1) + ym * 4 * fy * (1 - fy) + y1 * fy * (2 * fy - 1);
    return {static_cast<f32>(x0 + fx * (x1 - x0)), static_cast<f32>(y)};
  }
};

// A polygon ring clipped to the tile's own square [0, ext]^2 (Sutherland-Hodgman, tile units).
// Vector tiles repeat a building that crosses a tile edge in both tiles (clipped at the buffer):
// drawn twice, the copies' walls lie in one plane and flicker (z-fighting). Clipped to the square,
// the two halves meet at the edge, their cut walls inside the house facing away from each other.
std::vector<Vec2> clip_to_tile(const std::vector<Vec2>& ring, f32 ext) {
  std::vector<Vec2> a = ring, b;
  auto pass = [&](auto inside, auto cut) {
    b.clear();
    for (std::size_t i = 0; i < a.size(); ++i) {
      const Vec2 p = a[i], q = a[(i + 1) % a.size()];
      const bool pi = inside(p), qi = inside(q);
      if (pi) b.push_back(p);
      if (pi != qi) b.push_back(cut(p, q));
    }
    a.swap(b);
  };
  auto lerp_x = [](Vec2 p, Vec2 q, f32 x) { const f32 t = (x - p.x) / (q.x - p.x); return Vec2{x, p.y + (q.y - p.y) * t}; };
  auto lerp_y = [](Vec2 p, Vec2 q, f32 y) { const f32 t = (y - p.y) / (q.y - p.y); return Vec2{p.x + (q.x - p.x) * t, y}; };
  if (a.size() >= 3) pass([](Vec2 p) { return p.x >= 0; }, [&](Vec2 p, Vec2 q) { return lerp_x(p, q, 0); });
  if (a.size() >= 3) pass([&](Vec2 p) { return p.x <= ext; }, [&](Vec2 p, Vec2 q) { return lerp_x(p, q, ext); });
  if (a.size() >= 3) pass([](Vec2 p) { return p.y >= 0; }, [&](Vec2 p, Vec2 q) { return lerp_y(p, q, 0); });
  if (a.size() >= 3) pass([&](Vec2 p) { return p.y <= ext; }, [&](Vec2 p, Vec2 q) { return lerp_y(p, q, ext); });
  return a;
}

u32 hash_u32(u64 v) {
  v ^= v >> 33; v *= 0xff51afd7ed558ccdull; v ^= v >> 33; v *= 0xc4ceb9fe1a85ec53ull; v ^= v >> 33;
  return static_cast<u32>(v);
}

}  // namespace

void extract(const mvt::Tile& tile, const TileKey& key, const kern::GeoProjector& proj, MapFeatures& out) {
  std::vector<std::vector<std::vector<Vec2>>> polys;
  std::vector<std::vector<Vec2>> apron;
  if (const i32 at = tile.layer("building"); at >= 0) {
    const mvt::Layer& l = tile.layers[static_cast<std::size_t>(at)];
    const Projector P(key, proj, static_cast<f64>(l.extent));
    for (const mvt::Feature& f : l.features) {
      if (f.type != mvt::GeomType::polygon) continue;
      const i32 hide = l.tag(f, "hide_3d");
      if (hide >= 0 && l.values[static_cast<std::size_t>(hide)].kind == mvt::Value::Kind::boolean && l.values[static_cast<std::size_t>(hide)].b) continue;
      f32 top = static_cast<f32>(l.number(f, "render_height", -1));
      if (!(top > 0)) top = 8;  // SpawnCity's default (real_world_bootstrap.mjs:443)
      const f32 base = static_cast<f32>(std::fmax(0.0, l.number(f, "render_min_height", 0)));
      polygons(f, polys);
      u32 n = 0;
      for (auto& poly : polys) {
        MapFeatures::Building b;
        b.base = base;
        b.top = std::fmax(top, base + 1.0f);
        b.seed = hash_u32(f.id * 31 + (n++) + static_cast<u64>(key.x) * 7919 + static_cast<u64>(key.y) * 104729);
        const f32 ext = static_cast<f32>(l.extent);
        if (!poly.empty()) {
          // an outline that reaches over the tile's edge: kept whole for the furniture's clearance
          bool over = false;
          for (Vec2 q : poly[0]) over = over || q.x < 0 || q.y < 0 || q.x > ext || q.y > ext;
          if (over) { std::vector<Vec2> m; for (Vec2 q : poly[0]) m.push_back(P(q)); apron.push_back(std::move(m)); }
        }
        for (auto& ring : poly) {
          const std::vector<Vec2> clipped = clip_to_tile(ring, ext);
          if (clipped.size() < 3) { if (b.rings.empty()) break; continue; }  // the outline outside: the house is the other tile's
          std::vector<Vec2> m;
          m.reserve(clipped.size());
          for (Vec2 p : clipped) m.push_back(P(p));
          b.rings.push_back(std::move(m));
        }
        if (!b.rings.empty()) out.buildings.push_back(std::move(b));
      }
    }
  }
  if (const i32 at = tile.layer("transportation"); at >= 0) {
    const mvt::Layer& l = tile.layers[static_cast<std::size_t>(at)];
    const Projector P(key, proj, static_cast<f64>(l.extent));
    for (const mvt::Feature& f : l.features) {
      if (f.type != mvt::GeomType::line) continue;
      RoadClass c;
      if (!parse_road(l.text(f, "class"), c)) continue;
      const std::string_view brunnel = l.text(f, "brunnel");
      if (brunnel == "tunnel") continue;
      for (const auto& part : f.parts) {
        if (part.size() < 2) continue;
        MapFeatures::Road r;
        r.cls = c;
        r.bridge = brunnel == "bridge";
        for (Vec2 p : part) r.points.push_back(P(p));
        out.roads.push_back(std::move(r));
      }
    }
  }
  if (const i32 at = tile.layer("waterway"); at >= 0) {
    const mvt::Layer& l = tile.layers[static_cast<std::size_t>(at)];
    const Projector P(key, proj, static_cast<f64>(l.extent));
    for (const mvt::Feature& f : l.features) {
      if (f.type != mvt::GeomType::line) continue;
      const std::string_view cls = l.text(f, "class");
      const RoadClass c = cls == "river" ? RoadClass::river : cls == "canal" ? RoadClass::canal : RoadClass::stream;
      if (l.text(f, "brunnel") == "tunnel") continue;
      for (const auto& part : f.parts) {
        if (part.size() < 2) continue;
        MapFeatures::Road r;
        r.cls = c;
        for (Vec2 p : part) r.points.push_back(P(p));
        out.roads.push_back(std::move(r));
      }
    }
  }
  auto areas = [&](const char* layer, auto kind_of) {
    const i32 at = tile.layer(layer);
    if (at < 0) return;
    const mvt::Layer& l = tile.layers[static_cast<std::size_t>(at)];
    const Projector P(key, proj, static_cast<f64>(l.extent));
    for (const mvt::Feature& f : l.features) {
      if (f.type != mvt::GeomType::polygon) continue;
      AreaKind kind;
      if (!kind_of(l, f, kind)) continue;
      polygons(f, polys);
      for (auto& poly : polys) {
        MapFeatures::Area a;
        a.kind = kind;
        for (auto& ring : poly) {
          std::vector<Vec2> m;
          for (Vec2 p : ring) m.push_back(P(p));
          a.rings.push_back(std::move(m));
        }
        out.areas.push_back(std::move(a));
      }
    }
  };
  areas("water", [](const mvt::Layer&, const mvt::Feature&, AreaKind& k) { k = AreaKind::water; return true; });
  areas("park", [](const mvt::Layer&, const mvt::Feature&, AreaKind& k) { k = AreaKind::park; return true; });
  areas("landcover", [](const mvt::Layer& l, const mvt::Feature& f, AreaKind& k) {
    const std::string_view c = l.text(f, "class");
    if (c == "wood") k = AreaKind::wood;
    else if (c == "grass") k = AreaKind::grass;
    else if (c == "sand") k = AreaKind::sand;
    else return false;
    return true;
  });
  areas("landuse", [](const mvt::Layer& l, const mvt::Feature& f, AreaKind& k) {
    const std::string_view c = l.text(f, "class");
    if (c == "pitch" || c == "playground") k = AreaKind::pitch;
    else if (c == "cemetery" || c == "garden") k = AreaKind::grass;
    else return false;
    return true;
  });
  {
    const Projector P(key, proj, 1.0);
    const Vec2 a = P({0, 0}), b = P({1, 1});
    furnish(out, {std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmax(a.x, b.x), std::fmax(a.y, b.y)}, apron);
  }
}

// ---------------------------------------------------------------------------------------------
// street furniture

namespace {
struct FurnishIndex {
  static constexpr f32 kCell = 20;
  struct Seg { Vec2 a, b; f32 half; u32 road; bool paved; };
  /// A house's outline: the outer ring of the tile's building `index`, or the apron's ring `index`
  /// (the outlines that reach over the tile's edge), with its bounding box.
  struct Ring { bool apron; u32 index; kern::Vec4 box; };
  std::vector<Seg> segs;
  std::vector<Ring> rings;
  kern::Map<u64, std::vector<u32>> seg_cells, ring_cells;
  static u64 key(i32 x, i32 y) { return (static_cast<u64>(static_cast<u32>(x)) << 32) | static_cast<u32>(y); }
  static i32 cell(f32 v) { return static_cast<i32>(std::floor(v / kCell)); }
  void add_seg(Vec2 a, Vec2 b, f32 half, u32 road, bool paved) {
    const u32 id = static_cast<u32>(segs.size());
    segs.push_back({a, b, half, road, paved});
    const f32 m = half + 3;
    for (i32 x = cell(std::fmin(a.x, b.x) - m); x <= cell(std::fmax(a.x, b.x) + m); ++x)
      for (i32 y = cell(std::fmin(a.y, b.y) - m); y <= cell(std::fmax(a.y, b.y) + m); ++y) seg_cells[key(x, y)].push_back(id);
  }
  void add_ring(const std::vector<Vec2>& r, bool apron, u32 index) {
    if (r.size() < 3) return;
    kern::Vec4 box{1e30f, 1e30f, -1e30f, -1e30f};
    for (Vec2 p : r) box = {std::fmin(box.x, p.x), std::fmin(box.y, p.y), std::fmax(box.z, p.x), std::fmax(box.w, p.y)};
    if (box.z - box.x > 4000 || box.w - box.y > 4000) return;
    const u32 id = static_cast<u32>(rings.size());
    rings.push_back({apron, index, box});
    for (i32 x = cell(box.x); x <= cell(box.z); ++x)
      for (i32 y = cell(box.y); y <= cell(box.w); ++y) ring_cells[key(x, y)].push_back(id);
  }
  /// Inside a house (of the tile's `buildings` or the `apron` the rings were added from).
  bool built(Vec2 p, const std::vector<MapFeatures::Building>& buildings, const std::vector<std::vector<Vec2>>& apron) const {
    const auto it = ring_cells.find(key(cell(p.x), cell(p.y)));
    if (it == ring_cells.end()) return false;
    for (u32 id : it->second) {
      const Ring& g = rings[id];
      if (p.x < g.box.x || p.x > g.box.z || p.y < g.box.y || p.y > g.box.w) continue;
      const std::vector<Vec2>& r = g.apron ? apron[g.index] : buildings[g.index].rings[0];
      bool c = false;
      for (std::size_t i = 0, j = r.size() - 1; i < r.size(); j = i++)
        if ((r[i].y > p.y) != (r[j].y > p.y) && p.x < (r[j].x - r[i].x) * (p.y - r[i].y) / (r[j].y - r[i].y + 1e-12f) + r[i].x) c = !c;
      if (c) return true;
    }
    return false;
  }
  /// Clear of every street by `paved` beyond its carriageway and of every path by `unpaved`.
  bool off_roads(Vec2 p, f32 paved, f32 unpaved) const {
    const auto it = seg_cells.find(key(cell(p.x), cell(p.y)));
    if (it == seg_cells.end()) return true;
    for (u32 id : it->second) {
      const Seg& s = segs[id];
      const Vec2 d = s.b - s.a;
      const f32 l2 = d.x * d.x + d.y * d.y;
      const f32 t = l2 > 1e-6f ? clampf(((p.x - s.a.x) * d.x + (p.y - s.a.y) * d.y) / l2, 0, 1) : 0;
      if (std::hypot(p.x - s.a.x - d.x * t, p.y - s.a.y - d.y * t) < s.half + (s.paved ? paved : unpaved)) return false;
    }
    return true;
  }
};
}  // namespace

void furnish(MapFeatures& out, const kern::Vec4& sq, const std::vector<std::vector<Vec2>>& apron) {
  out.trees.clear();
  out.lamps.clear();
  FurnishIndex ix;
  for (u32 ri = 0; ri < out.roads.size(); ++ri) {
    const MapFeatures::Road& r = out.roads[ri];
    if (road_is_water(r.cls)) continue;
    const bool paved = r.cls != RoadClass::path && r.cls != RoadClass::track;
    for (std::size_t k = 0; k + 1 < r.points.size(); ++k) ix.add_seg(r.points[k], r.points[k + 1], road_width(r.cls) / 2, ri, paved);
  }
  for (u32 bi = 0; bi < out.buildings.size(); ++bi)
    if (!out.buildings[bi].rings.empty()) ix.add_ring(out.buildings[bi].rings[0], false, bi);
  for (u32 ai = 0; ai < apron.size(); ++ai) ix.add_ring(apron[ai], true, ai);
  auto inside = [&](Vec2 p) { return p.x >= sq.x && p.x < sq.z && p.y >= sq.y && p.y < sq.w; };
  constexpr f32 kPi2 = 6.2831853f;
  kern::Set<u64> lamp_cells;
  for (u32 ri = 0; ri < out.roads.size(); ++ri) {
    const MapFeatures::Road& r = out.roads[ri];
    if (road_is_water(r.cls) || r.points.size() < 2) continue;
    const f32 w = road_width(r.cls);
    // ---- trees beside the streets cars drive (world_liveliness.mjs: trunk 2.3 m, crown 0.92 m,
    // 2.4 m clear of houses): every 37 m, alternating sides, beyond the pavement
    if (road_drivable(r.cls) && !r.bridge && w <= 14) {
      f32 carry = 17;
      u32 i = ri * 7;
      for (std::size_t k = 0; k + 1 < r.points.size(); ++k) {
        const Vec2 a = r.points[k], d = r.points[k + 1] - a;
        const f32 len = std::hypot(d.x, d.y);
        if (len < 1) continue;
        const Vec2 u = d * (1 / len), right{u.y, -u.x};
        for (f32 t = carry; t < len; t += 37, ++i) {
          const f32 side = (i & 1) ? 1.0f : -1.0f, off = w * 0.5f + 2.2f + static_cast<f32>(i % 4) * 0.4f;
          const Vec2 c = a + u * t + right * (side * off);
          if (!inside(c) || !ix.off_roads(c, 1.9f, 0.6f) || ix.built(c, out.buildings, apron)) continue;
          bool free = true;
          for (int j = 0; j < 6 && free; ++j) free = !ix.built(c + Vec2{std::cos(static_cast<f32>(j) * kPi2 / 6), std::sin(static_cast<f32>(j) * kPi2 / 6)} * 2.4f, out.buildings, apron);
          if (free) out.trees.push_back({c, 0.82f + static_cast<f32>(i % 5) * 0.045f});
        }
        carry = std::fmod(carry + 37 - std::fmod(len, 37.0f), 37.0f);
      }
    }
    // ---- street lamps (street_lamps.mjs setLampRoads): every street but paths and tracks
    if (r.cls != RoadClass::path && r.cls != RoadClass::track) {
      f32 carry = 34 * 0.5f, side = 1;
      for (std::size_t k = 0; k + 1 < r.points.size(); ++k) {
        const Vec2 a = r.points[k], d = r.points[k + 1] - a;
        const f32 len = std::hypot(d.x, d.y);
        if (len < 0.5f) continue;
        const Vec2 u = d * (1 / len), n{-u.y, u.x};
        for (f32 t = carry; t < len; t += 34) {
          side = -side;
          const Vec2 c = a + u * t + n * ((w / 2 + 1.15f) * side);
          if (!inside(c)) continue;
          const u64 cell = (static_cast<u64>(static_cast<u32>(std::lround(c.x / 8))) << 32) | static_cast<u32>(std::lround(c.y / 8));
          // on its kerb: off every other street's carriageway, not in a house (nor its arm's foot)
          if (!ix.off_roads(c, 0.35f, 0.3f) || ix.built(c, out.buildings, apron)) continue;
          if (!lamp_cells.insert(cell).second) continue;
          out.lamps.push_back({c, std::atan2(-n.y * side, -n.x * side)});
        }
        carry = std::fmod(std::fmod(carry - len, 34.0f) + 34.0f, 34.0f);
      }
    }
  }
}

void training_level(MapFeatures& out) {
  out.procedural = true;
  auto road = [&](f32 x, f32 y, f32 w, f32 h, RoadClass c) {
    // the plane's long side is the centreline (these match trainingRoutes())
    MapFeatures::Road r;
    r.cls = c;
    if (w >= h) r.points = {{x - w / 2, y}, {x + w / 2, y}};
    else r.points = {{x, y - h / 2}, {x, y + h / 2}};
    out.roads.push_back(std::move(r));
  };
  road(0, -38, 124, 8, RoadClass::secondary);
  road(0, 38, 124, 8, RoadClass::secondary);
  road(-62, 0, 8, 76, RoadClass::secondary);
  road(62, 0, 8, 76, RoadClass::secondary);
  road(0, 0, 168, 11, RoadClass::primary);
  road(0, 0, 10, 152, RoadClass::primary);
  road(0, 54, 140, 8, RoadClass::tertiary);
  {
    // the pedestrian bridge over the pond (5 x 42 m)
    MapFeatures::Road b;
    b.cls = RoadClass::pedestrian;
    b.bridge = true;
    b.points = {{34, 31 - 21}, {34, 31 + 21}};
    out.roads.push_back(std::move(b));
  }
  auto rect = [](f32 x, f32 y, f32 w, f32 h) { return std::vector<Vec2>{{x - w / 2, y - h / 2}, {x + w / 2, y - h / 2}, {x + w / 2, y + h / 2}, {x - w / 2, y + h / 2}}; };
  for (const auto& [x, y, w, h] : std::initializer_list<std::array<f32, 4>>{{-38, 28, 42, 30}, {48, -25, 32, 22}}) {
    MapFeatures::Area a;
    a.kind = AreaKind::park;
    a.rings.push_back(rect(x, y, w, h));
    out.areas.push_back(std::move(a));
  }
  {
    // the saturated harbour pond: a circle of 20 m stretched 1.55 x .92 at (34, 31)
    MapFeatures::Area pond;
    pond.kind = AreaKind::water;
    std::vector<Vec2> ring;
    for (int i = 0; i < 48; ++i) {
      const f32 a = 2 * kPi * static_cast<f32>(i) / 48;
      ring.push_back({34 + std::cos(a) * 20 * 1.55f, 31 + std::sin(a) * 20 * 0.92f});
    }
    pond.rings.push_back(std::move(ring));
    out.areas.push_back(std::move(pond));
  }
  // the compact skyline: x, y, width, depth, height
  const std::array<f32, 5> houses[] = {{-47, -22, 15, 13, 10}, {-27, -23, 13, 12, 15}, {31, -23, 15, 13, 12}, {50, -22, 12, 12, 18}, {-51, 18, 13, 12, 14},
                                       {-18, 24, 14, 11, 20},  {54, 15, 13, 12, 11},   {-55, 61, 16, 11, 9},  {55, 64, 18, 12, 13}};
  u32 n = 0;
  for (const auto& [x, y, w, d, h] : houses) {
    MapFeatures::Building b;
    b.rings.push_back(rect(x, y, w, d));
    b.base = 0;
    b.top = h;
    b.seed = 0x7a1e0u + n++ * 2654435761u;
    out.buildings.push_back(std::move(b));
  }
}

void procedural_city(f64 cx, f64 cy, f64 size, u32 seed, MapFeatures& out) {
  // A walled-town grid with a market square: blocks of narrow terraced houses (3-6 storeys),
  // a ring road, a park and a lake shore to the north. Nothing here comes from real map data.
  Rng rng = Rng::seeded(seed);
  out.procedural = true;
  const f32 pitch = 76, street = 12;
  const int blocks = std::max(3, static_cast<int>(size / pitch));
  const f32 half = blocks * pitch * 0.5f;
  const f32 x0 = static_cast<f32>(cx) - half, y0 = static_cast<f32>(cy) - half;
  for (int i = 0; i <= blocks; ++i) {
    const f32 t = -half + i * pitch;
    MapFeatures::Road a, b;
    const bool ring = i == 0 || i == blocks;
    a.cls = b.cls = ring ? RoadClass::secondary : (i == blocks / 2 ? RoadClass::tertiary : RoadClass::minor);
    a.points = {{static_cast<f32>(cx) + t, static_cast<f32>(cy) - half}, {static_cast<f32>(cx) + t, static_cast<f32>(cy) + half}};
    b.points = {{static_cast<f32>(cx) - half, static_cast<f32>(cy) + t}, {static_cast<f32>(cx) + half, static_cast<f32>(cy) + t}};
    out.roads.push_back(a);
    out.roads.push_back(b);
  }
  for (int bx = 0; bx < blocks; ++bx)
    for (int by = 0; by < blocks; ++by) {
      const f32 a0 = x0 + bx * pitch + street * 0.5f + 2, b0 = y0 + by * pitch + street * 0.5f + 2;
      const f32 inner = pitch - street - 4;
      const bool market = bx == blocks / 2 && by == blocks / 2;
      const bool park = (bx == 1 && by == blocks - 2);
      if (market || park) {
        MapFeatures::Area a;
        a.kind = market ? AreaKind::sand : AreaKind::park;
        a.rings.push_back({{a0, b0}, {a0 + inner, b0}, {a0 + inner, b0 + inner}, {a0, b0 + inner}});
        out.areas.push_back(a);
        continue;
      }
      // Terraced houses along the four sides of the block, a courtyard in the middle.
      const f32 depth = rng.range(10, 14);
      for (int side = 0; side < 4; ++side) {
        f32 along = 0;
        while (along < inner - 4) {
          const f32 w = std::fmin(rng.range(7, 13), inner - along);
          f32 ax, ay, bx2, by2;
          switch (side) {
            case 0: ax = a0 + along; ay = b0; bx2 = ax + w; by2 = b0 + depth; break;
            case 1: ax = a0 + inner - depth; ay = b0 + along; bx2 = a0 + inner; by2 = ay + w; break;
            case 2: ax = a0 + inner - along - w; ay = b0 + inner - depth; bx2 = ax + w; by2 = b0 + inner; break;
            default: ax = a0; ay = b0 + inner - along - w; bx2 = a0 + depth; by2 = ay + w; break;
          }
          // Skip the corner overlaps of side 1..3 with side 0 / 2.
          if ((side == 1 || side == 3) && (ay < b0 + depth || by2 > b0 + inner - depth)) { along += w; continue; }
          MapFeatures::Building b;
          b.rings.push_back({{ax, ay}, {bx2, ay}, {bx2, by2}, {ax, by2}});
          b.top = 3.2f * static_cast<f32>(3 + static_cast<int>(rng.unit() * 4)) + rng.range(0, 1.5f);
          b.seed = rng.next();
          out.buildings.push_back(std::move(b));
          along += w;
        }
      }
    }
  // Lake shore to the north (a stand-in for the Bodensee).
  MapFeatures::Area lake;
  lake.kind = AreaKind::water;
  const f32 ly = static_cast<f32>(cy) + half + 30;
  lake.rings.push_back({{static_cast<f32>(cx) - half * 2, ly}, {static_cast<f32>(cx) + half * 2, ly}, {static_cast<f32>(cx) + half * 2, ly + 900},
                        {static_cast<f32>(cx) - half * 2, ly + 900}});
  out.areas.push_back(lake);
}

std::vector<std::vector<Vec2>> clip_line(const std::vector<Vec2>& line, const kern::Vec4& rect) {
  // Liang-Barsky per stretch; stretches that follow each other inside stay one part
  std::vector<std::vector<Vec2>> out;
  std::vector<Vec2> cur;
  auto close = [&] { if (cur.size() >= 2) out.push_back(std::move(cur)); cur.clear(); };
  for (std::size_t i = 0; i + 1 < line.size(); ++i) {
    const Vec2 a = line[i], b = line[i + 1];
    const f32 dx = b.x - a.x, dy = b.y - a.y;
    f32 t0 = 0, t1 = 1;
    bool in = true;
    const f32 p[4] = {-dx, dx, -dy, dy}, q[4] = {a.x - rect.x, rect.z - a.x, a.y - rect.y, rect.w - a.y};
    for (int k = 0; k < 4 && in; ++k) {
      if (p[k] == 0) { in = q[k] >= 0; continue; }
      const f32 r = q[k] / p[k];
      if (p[k] < 0) { if (r > t1) in = false; else if (r > t0) t0 = r; }
      else { if (r < t0) in = false; else if (r < t1) t1 = r; }
    }
    if (!in || t1 <= t0) { close(); continue; }
    const Vec2 ca{a.x + dx * t0, a.y + dy * t0}, cb{a.x + dx * t1, a.y + dy * t1};
    if (cur.empty()) cur.push_back(ca);
    else if (t0 > 0) { close(); cur.push_back(ca); }  // (it came back in: a new part)
    cur.push_back(cb);
    if (t1 < 1) close();
  }
  close();
  return out;
}

}  // namespace sc
