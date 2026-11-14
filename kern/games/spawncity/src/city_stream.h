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
#include "terrain.h"
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
  // the start's milestones in ms since start() (-1: not yet): requested / data arrived / decoded /
  // ground settled / built / collision around the player - where a slow start spends its time
  f64 t_request = -1, t_data = -1, t_decoded = -1, t_terrain = -1, t_built = -1, t_collision = -1;
  f64 work_ms = 0;  // streaming time spent so far (all slices)
  f64 worst_ms = 0; // longest slice of the last 2 s (a hitch shows here)
  const char* worst_step = "";  // the step that took longest in that slice (where a hitch comes from)
  f64 worst_step_ms = 0;
  u32 rebuilds = 0, relays = 0;  // building re-syncs and street layings so far
  u32 uploads = 0, upload_vertices = 0;  // meshes uploaded so far, their vertices (thousands)
};

class CityStream {
 public:
  CityStream();
  ~CityStream();

  /// Starts a world around lon/lat (origin of the local metres). `world` gets the static collision.
  /// `dem`: ask the host for real elevation (terrain.h), else flat ground.
  void start(kern::Renderer& renderer, kern::physics::World& world, f64 lon, f64 lat, bool dem = false);
  void clear();
  const kern::GeoProjector& projector() const { return proj_; }

  /// Requests, builds and unloads around `focus` for about `budget_ms`.
  void update(Vec3 focus, f64 budget_ms);

  // ---- host side ----
  /// Tiles to fetch now, as JSON [[z,x,y],...] (each tile once).
  std::string take_requests();
  /// Uncompressed MVT bytes of a requested tile (copied).
  void provide(const TileKey& key, kern::Span<const u8> bytes);
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
  /// Ground height: the terrain's static surface (elevation, water basins, craters).
  f32 ground_height(f32 x, f32 y) const { return terrain_.height(x, y); }
  /// The ground (elevation tiles, water, craters, ground tiles): terrain.h.
  Terrain& terrain() { return terrain_; }
  const Terrain& terrain() const { return terrain_; }
  /// A second point that keeps ground collision under it (the pilot while his drone flies).
  Vec3 anchor;
  const StreamStats& stats() const { return stats_; }
  /// A building wall (one outline edge) as the facade marks see it (facade_marks.mjs wall index):
  /// start, unit direction, outward normal, length, base / top of the building, its seed.
  struct Wall { Vec2 a, dir, normal; f32 len = 0, base = 0, top = 0; u32 seed = 0, edge = 0; };
  /// Walls of loaded buildings whose outline comes within `r` of p.
  void walls_near(Vec2 p, f32 r, std::vector<Wall>& out) const;
  /// The building whose footprint contains p (-1: none); its outer ring, base and top.
  bool building_at(Vec2 p, std::vector<Vec2>* ring = nullptr, f32* base = nullptr, f32* top = nullptr, u32* seed = nullptr) const;
  /// The street furniture (tiles.h furnish) of every tile whose ground is final, in a fixed
  /// order; `furniture_serial` changes whenever that set does.
  struct Furniture { u64 tile; const std::vector<MapFeatures::Spot>* trees; const std::vector<MapFeatures::Spot>* lamps; };
  void furniture(std::vector<Furniture>& out) const;
  u32 furniture_serial() const { return furniture_serial_; }
  /// Where the ground changed since the last call (a crater, a new elevation window): what stands
  /// on it is put on it again.
  void take_ground_changes(std::vector<kern::Vec4>& out) { out.insert(out.end(), ground_changes_.begin(), ground_changes_.end()); ground_changes_.clear(); }
  /// Where the player will be in a few seconds, relative to the focus (his velocity x 4 s, the
  /// game sets it): the drawn city and its streets are built along the way there, so nothing is
  /// missing where a fast car arrives.
  Vec3 view_lead;
  /// How many buildings are drawn at most (world_city_buildings.mjs MAX_FOOTPRINTS 2600).
  u32 building_budget = 2600;
  i32 ring = 1;  // tiles requested around the player's tile (1 = 3 x 3)
  /// MP host: the other players - the town is loaded around them too (ring `mate_ring`), so the
  /// host's traffic and people live there (the authority over the whole town: game_worldsync.cpp)
  std::vector<Vec3> extra_foci;
  i32 mate_ring = 1;
  bool training_offline = false;  // offline: the original's training level instead of the stand-in town

  // ---- street life and interiors (M3 life: life.h, interiors.h) ----
  /// A building or area of a built tile: tile hash + index (stable while the tile is loaded).
  struct BuildingRef { u64 tile = 0; u32 index = 0; const MapFeatures::Building* building = nullptr; };
  struct AreaRef { u64 key = 0; const MapFeatures::Area* area = nullptr; };
  /// Buildings of ready tiles whose outline's bounding box comes within `radius` of `p`.
  void buildings_near(Vec2 p, f32 radius, std::vector<BuildingRef>& out) const;
  /// The map features of every built tile (the minimap, web/view.mjs).
  void ready_features(std::vector<const MapFeatures*>& out) const;
  /// Areas of a kind (meadows: grass) of ready tiles within `radius` of `p`.
  void areas_near(Vec2 p, f32 radius, AreaKind kind, std::vector<AreaRef>& out) const;
  /// Leaves one building out of the static collision (an entered interior brings its own); the
  /// tile's collision is rebuilt in time slices. `tile` 0 restores every building.
  void exclude_building(u64 tile, i32 index);
  /// True once the collision matches the last exclude_building() call.
  bool exclusion_done() const;

  // ---- destruction (destruction.h) ----
  /// Lowers a building to `new_top` (only ever lower; `leveled`: collapsed to its stump). One
  /// truth for everything that reads the building (drawing, collision, minimap, walls, interiors):
  /// its map feature. Its 160 m mesh chunk is rebuilt as a job and swapped in one step, its
  /// collider replaced alone. The damage outlives the tile (a re-streamed tile keeps it) until
  /// clear_damage().
  bool damage_building(u64 tile, u32 index, f32 new_top, bool leveled);
  /// Every building back to its full height (RESET).
  void clear_damage();
  u32 damaged_buildings() const { return static_cast<u32>(damage_.size()); }
  /// Counts every height a building lost (marks on the walls drop what hangs in the air: world_marks.cpp).
  u32 damage_serial() const { return damage_serial_; }
  /// Destruction sync (game_destruction_sync.cpp): one building's damage as it goes over the
  /// network - by its seed, the height above its base that is left, collapsed or not.
  struct DamageRec { u32 seed = 0; f32 height = 0; bool leveled = false; };
  /// Every damaged building (also those whose tile is not loaded here).
  void damage_list(std::vector<DamageRec>& out) const;
  /// The seeds whose damage changed since the last call (what this machine has to tell).
  std::vector<u32> take_damage_log() { return std::move(damage_log_); }
  bool damage_of(u32 seed, DamageRec& out) const;
  /// Merge damage from another machine: only ever lower (the lowest height anybody saw wins, so
  /// every machine ends at the same city whatever order the reports come in). A loaded building
  /// is lowered at once (drawing, collider), an unloaded one when its tile streams in. Returns
  /// whether anything changed. `log`: whether this change is ours to tell again (host relays).
  bool merge_damage(const std::vector<DamageRec>& list, bool log);

 private:
  u32 damage_serial_ = 0;
  u32 furniture_serial_ = 0;
  std::vector<kern::Vec4> ground_changes_;
  struct Tile;
  void request_around(Vec3 focus, i32 ring_of);
  void unload_far(Vec3 focus);
  kern::Vec4 ring_bounds(Vec3 focus, i32 r) const;
  bool work(Tile& t, f64 deadline);
  void finish_features(Tile& t);
  void upload(Tile& t, kern::GeoChunk& chunk, const kern::Material& material, bool walls);
  void remove(Tile& t);
  void feed_terrain(Tile& t);
  void begin_roads(Tile& t);
  void relay(const kern::Vec4& rect);
  // The original's working sets around the focus: the drawn buildings (world_city_buildings.mjs:
  // VISUAL_RADIUS_M 900, MAX_FOOTPRINTS 2600, MAX_VERTICES 96, re-synced after RESYNC_MOVE_M 140
  // or RESYNC_MS 2500) and the collision (world_building_collisions.mjs: RADIUS_M 340,
  // MAX_FOOTPRINTS 480, MAX_VERTICES 64), the streets (world_city_roads.mjs: RADIUS_M 720).
  struct NearCollision {
    kern::physics::Body body, old_body;
    Vec2 at{1e9f, 1e9f}, ahead{1e9f, 1e9f};
    f64 at_ms = -1e9;
    std::vector<std::pair<u64, u32>> list;
    std::vector<kern::physics::Shape> shapes;  // list[i]'s collider (destruction replaces one)
    std::size_t next = 0;
    bool building = false, dirty = true, built = false;
    std::vector<Vec3> mates_at;  // where the mates were at the last pick (MP host)
    u32 colliders = 0;
  };
  NearCollision near_;
  StreamStats ms_;          // the milestones (copied into stats_ every update)
  f64 start_ms_ = 0;
  f64 worst_ = 0, worst_at_ = 0;
  const char* worst_step_ = "";
  f64 worst_step_ms_ = 0;
  u32 rebuilds_ = 0, relays_ = 0, upload_count_ = 0;
  u64 upload_vertices_ = 0;
  void step_time(const char* name, f64 t0);
  Vec3 focus_{};
  Vec2 vis_at_{1e9f, 1e9f}, vis_ahead_{1e9f, 1e9f};
  f64 vis_ms_ = -1e9;
  void select_cells(Vec3 focus, bool force);
  void drop_cell(Tile& t, u64 cell);
  void update_collision(Vec3 focus, f64 deadline);
  bool build_near(f64 deadline);
  kern::physics::Shape building_shape(kern::physics::Body body, const MapFeatures::Building& b);
  // damage by building seed: the height above its base that is left, and whether it collapsed
  struct Damage { f32 height = 0; bool leveled = false; };
  std::unordered_map<u32, Damage> damage_;
  std::vector<u32> damage_log_;
  bool damage_quiet_ = false;  // merge_damage(.., false): not logged
  void begin_cells(Tile& t);
  bool finish_cells(Tile& t, f64 deadline);

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
  Terrain terrain_;
  std::vector<kern::Vec4> terrain_changes_;
  void recollide(Tile& t);
  u64 excl_tile_ = 0;  // the building left out of the collision (interiors)
  i32 excl_index_ = -1;
};

}  // namespace sc
