// Map features from vector tiles and the procedural stand-in city (see tiles.h).
#include "tiles.h"

#include <cstring>
#include <string_view>

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

u32 hash_u32(u64 v) {
  v ^= v >> 33; v *= 0xff51afd7ed558ccdull; v ^= v >> 33; v *= 0xc4ceb9fe1a85ec53ull; v ^= v >> 33;
  return static_cast<u32>(v);
}

}  // namespace

void extract(const mvt::Tile& tile, const TileKey& key, const kern::GeoProjector& proj, MapFeatures& out) {
  std::vector<std::vector<std::vector<Vec2>>> polys;
  if (const mvt::Layer* l = tile.layer("building")) {
    const Projector P(key, proj, static_cast<f64>(l->extent));
    for (const mvt::Feature& f : l->features) {
      if (f.type != mvt::GeomType::polygon) continue;
      const mvt::Value* hide = l->tag(f, "hide_3d");
      if (hide && hide->kind == mvt::Value::Kind::boolean && hide->b) continue;
      f32 top = static_cast<f32>(l->number(f, "render_height", -1));
      if (!(top > 0)) top = 8;  // SpawnCity's default (real_world_bootstrap.mjs:443)
      const f32 base = static_cast<f32>(std::fmax(0.0, l->number(f, "render_min_height", 0)));
      polygons(f, polys);
      u32 n = 0;
      for (auto& poly : polys) {
        MapFeatures::Building b;
        b.base = base;
        b.top = std::fmax(top, base + 1.0f);
        b.seed = hash_u32(f.id * 31 + (n++) + static_cast<u64>(key.x) * 7919 + static_cast<u64>(key.y) * 104729);
        for (auto& ring : poly) {
          std::vector<Vec2> m;
          m.reserve(ring.size());
          for (Vec2 p : ring) m.push_back(P(p));
          b.rings.push_back(std::move(m));
        }
        out.buildings.push_back(std::move(b));
      }
    }
  }
  if (const mvt::Layer* l = tile.layer("transportation")) {
    const Projector P(key, proj, static_cast<f64>(l->extent));
    for (const mvt::Feature& f : l->features) {
      if (f.type != mvt::GeomType::line) continue;
      RoadClass c;
      if (!parse_road(l->text(f, "class"), c)) continue;
      const std::string_view brunnel = l->text(f, "brunnel");
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
  if (const mvt::Layer* l = tile.layer("waterway")) {
    const Projector P(key, proj, static_cast<f64>(l->extent));
    for (const mvt::Feature& f : l->features) {
      if (f.type != mvt::GeomType::line) continue;
      const std::string_view cls = l->text(f, "class");
      const RoadClass c = cls == "river" ? RoadClass::river : cls == "canal" ? RoadClass::canal : RoadClass::stream;
      if (l->text(f, "brunnel") == "tunnel") continue;
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
    const mvt::Layer* l = tile.layer(layer);
    if (!l) return;
    const Projector P(key, proj, static_cast<f64>(l->extent));
    for (const mvt::Feature& f : l->features) {
      if (f.type != mvt::GeomType::polygon) continue;
      AreaKind kind;
      if (!kind_of(*l, f, kind)) continue;
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
}

void procedural_city(f64 cx, f64 cy, f64 size, u32 seed, MapFeatures& out) {
  // A walled-town grid with a market square: blocks of narrow terraced houses (3-6 storeys),
  // a ring road, a park and a lake shore to the north. Nothing here comes from real map data.
  Rng rng(seed);
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

}  // namespace sc
