// Map data for the streamed city: web-mercator tile keys, the OpenMapTiles layers of a vector
// tile turned into game features (building footprints with heights, streets by class, water,
// parks and woods) in the game's local metres, and the procedural stand-in city used when no map
// data can be loaded (clearly marked as such in the HUD).
#pragma once

#include <string>
#include <vector>

#include "common.h"
#include "kern/geo/roads.h"
#include "mvt.h"

namespace sc {

constexpr i32 kTileZoom = 14;  // OpenFreeMap's planet tiles go up to z14 (overzoomed beyond)

struct TileKey {
  i32 z = kTileZoom, x = 0, y = 0;
  bool operator==(const TileKey& o) const { return z == o.z && x == o.x && y == o.y; }
  u64 hash() const { return (static_cast<u64>(static_cast<u32>(x)) << 32) ^ static_cast<u64>(static_cast<u32>(y)) ^ (static_cast<u64>(z) << 58); }
};

TileKey tile_at(f64 lon, f64 lat, i32 z = kTileZoom);
/// lon/lat of a point inside a tile (fx, fy in 0..1 from the tile's north-west corner).
void tile_lonlat(const TileKey& t, f64 fx, f64 fy, f64& lon, f64& lat);

/// Local metres <-> lon/lat around the projector's origin (kern::GeoProjector, equirectangular).
void unproject(const kern::GeoProjector& p, f64 x, f64 y, f64& lon, f64& lat);

enum class RoadClass : u8 { motorway, trunk, primary, secondary, tertiary, minor, service, track, path, pedestrian, river, canal, stream };
f32 road_width(RoadClass c);   // SpawnCity world_city_roads.mjs WIDTH (water: world_water.mjs)
bool road_is_water(RoadClass c);
bool road_drivable(RoadClass c);

enum class AreaKind : u8 { water, park, wood, grass, sand, pitch };

struct MapFeatures {
  struct Building {
    std::vector<std::vector<Vec2>> rings;  // outer first, then courtyards (metres)
    f32 base = 0, top = 8;
    u32 seed = 0;
  };
  struct Road {
    std::vector<Vec2> points;
    RoadClass cls = RoadClass::minor;
    bool bridge = false;
  };
  struct Area {
    std::vector<std::vector<Vec2>> rings;
    AreaKind kind = AreaKind::park;
  };
  std::vector<Building> buildings;
  std::vector<Road> roads;
  std::vector<Area> areas;
  bool procedural = false;  // the stand-in city, not real map data
};

/// OpenMapTiles layers (building, transportation, water, waterway, park, landcover) -> features.
void extract(const mvt::Tile& tile, const TileKey& key, const kern::GeoProjector& proj, MapFeatures& out);

/// The offline stand-in: a procedural European-style town filling a square of `size` metres
/// centred on (cx, cy). Deterministic per seed.
void procedural_city(f64 cx, f64 cy, f64 size, u32 seed, MapFeatures& out);

/// Default start: Konstanz, Marktstaette (old town by the harbour).
constexpr f64 kKonstanzLon = 9.1766, kKonstanzLat = 47.6600;

}  // namespace sc
