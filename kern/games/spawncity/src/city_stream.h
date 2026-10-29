// The streamed real city: vector tiles around the player are requested from the host (the browser
// fetches OpenFreeMap), decoded, extruded with Kern's BuildingBuilder / RoadBuilder, uploaded as
// chunk meshes and given static Box3D collision (one triangle-mesh shape per building) - every
// stage in time slices, so loading never stalls a frame. Tiles far behind are unloaded.
// Without map data the procedural stand-in town is built through the same pipeline.
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "kern/geo/buildings.h"
#include "kern/geo/roads.h"
#include "kern/gfx/renderer.h"
#include "tiles.h"

namespace sc {

/// A drivable street centre line (traffic follows it on the right-hand side).
struct Lane {
  std::vector<Vec2> points;
  f32 width = 6;
  u64 tile = 0;
  bool alive = true;
};

struct StreamStats {
  u32 tiles_ready = 0, tiles_loading = 0, tiles_failed = 0, buildings = 0, colliders = 0, triangles = 0, meshes = 0;
  f64 last_slice_ms = 0;
};

class CityStream {
 public:
  CityStream();
  ~CityStream();

  /// Starts a world around lon/lat (origin of the local metres). `world` gets the static collision.
  void start(kern::Renderer& renderer, kern::physics::World& world, f64 lon, f64 lat);
  void clear();
  const kern::GeoProjector& projector() const { return proj_; }

  /// Requests, builds and unloads around `focus` for about `budget_ms`.
  void update(Vec3 focus, f64 budget_ms);

  // ---- host side ----
  /// Tiles to fetch now, as JSON [[z,x,y],...] (each tile once).
  std::string take_requests();
  /// Uncompressed MVT bytes of a requested tile (copied).
  void provide(const TileKey& key, const u8* bytes, std::size_t length);
  void fail(const TileKey& key);
  /// No map service: the procedural stand-in town around the origin.
  void go_offline();
  bool offline() const { return offline_; }
  bool has_real_data() const { return real_tiles_ > 0; }
  /// True once the tile under the player is built (or the stand-in town is).
  bool ready_at(Vec3 p) const;

  // ---- gameplay queries ----
  const std::vector<Lane>& lanes() const { return lanes_; }
  /// A point on a drivable street within `radius` of `near` (heading along the street).
  bool street_point(Rng& rng, Vec3 near, f32 radius, Vec3& point, f32& heading) const;
  /// Ground height (flat in this milestone; the DEM comes later).
  f32 ground_height(f32 x, f32 y) const { (void)x; (void)y; return 0; }
  const StreamStats& stats() const { return stats_; }
  i32 ring = 1;  // tiles requested around the player's tile (1 = 3 x 3)

 private:
  struct Tile;
  Tile* tile(const TileKey& k);
  void request_around(Vec3 focus);
  void unload_far(Vec3 focus);
  bool work(Tile& t, f64 deadline);
  void finish_features(Tile& t);
  bool build_collision(Tile& t, f64 deadline);
  void upload(Tile& t, kern::GeoChunk& chunk, const kern::Material& material, bool walls);
  void build_areas(Tile& t);
  void remove(Tile& t);

  kern::Renderer* r_ = nullptr;
  kern::physics::World* world_ = nullptr;
  kern::GeoProjector proj_;
  TileKey origin_tile_;
  std::unordered_map<u64, std::unique_ptr<Tile>> tiles_;
  std::vector<TileKey> requests_;
  std::vector<Lane> lanes_;
  bool offline_ = false;
  u32 real_tiles_ = 0;
  StreamStats stats_;
};

}  // namespace sc
