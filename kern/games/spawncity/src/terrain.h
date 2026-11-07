// The ground of the streamed city: real elevation, water basins, craters - one height function
// shared by physics, rendering and everything standing on it (SpawnCity terrain_elevation.mjs,
// terrain_craters.mjs, terrain_tiles.mjs, terrain_surface.mjs, world_water.mjs).
//
//  * Elevation: the public Terrarium tiles (Mapzen / AWS Open Data "Terrain Tiles": SRTM,
//    EU-DEM, 3DEP and others merged). The page fetches the PNGs the game asks for and hands over
//    the RGBA pixels; C++ decodes them (h = R*256 + G + B/256 - 32768), samples a regular grid
//    around the player and turns the radar surface model into bare earth (morphological opening
//    over 160 m, light blur over 30 m: terrain_elevation.mjs bareEarth). Heights are relative to
//    the elevation at the world origin, so the origin stays at z = 0.
//  * Water: rivers, canals and streams (lines with a width per class) and lakes / harbours
//    (polygons) from the vector tiles are rasterised on a 4 m grid; the ground under them is a
//    basin 2.6 m deep, the surface lies 0.18 m below the bank (world_water.mjs, WATER_LEVEL_M /
//    WATER_BED_M). Bridges keep the ground. The game applies buoyancy and drag (game_world.cpp).
//  * Craters: blasts dig a bowl with a raised rim (craterProfile of terrain_craters.mjs).
//  * Ground tiles: render meshes (5 m nodes near the player, coarser further out, skirts against
//    cracks) and static Box3D triangle meshes (5 m nodes, one cell of overlap with a sunk outer
//    ring so seams are interior edges: terrain_tiles.mjs) around the player and his focus. Both
//    triangulate along the same SW-NE diagonal as height() interpolates, so what is drawn is what
//    is hit. Kern has no height-field shape yet (README: "Spaeter in die Engine").
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common.h"
#include "kern/geo/mesh_data.h"
#include "kern/geo/roads.h"
#include "kern/gfx/renderer.h"
#include "tiles.h"

namespace sc {

struct TerrainStats {
  u32 dem_tiles = 0, dem_failed = 0, render_tiles = 0, collision_tiles = 0, water_cells = 0, craters = 0, rebuilds = 0;
  f32 min_height = 0, max_height = 0;
  bool dem = false, settled = false;
};

class Terrain {
 public:
  static constexpr u32 kDemJob = 3;            // the elevation grid job (kern/core/jobs.h; city jobs are 1, 2)
  static constexpr i32 kDemZoom = 13;          // see README: the city ring is ~5 km, bare earth is smooth below 160 m
  static constexpr f32 kDemStep = 10;          // grid spacing of the sampled elevation (m)
  static constexpr f32 kDemSize = 8000;        // grid window around the player (m)
  static constexpr f32 kDemReload = 2000;      // reload after this much travel (terrain_elevation.mjs RELOAD_MOVE_M analogue)
  static constexpr f32 kOpen = 160, kBlur = 30;  // terrain_elevation.mjs OPEN_M, BLUR_M
  static constexpr f32 kStep = 5;              // terrain_craters.mjs TERRAIN_FIELD_STEP_M
  static constexpr f32 kWaterLevel = -0.18f;   // terrain_craters.mjs WATER_LEVEL_M
  static constexpr f32 kWaterBed = 2.6f;       // terrain_craters.mjs WATER_BED_M
  static constexpr f32 kWaterCell = 4;         // world_water.mjs CELL
  static constexpr f32 kWaterRenderZ = 0.052f; // world_water.mjs WATER_RENDER_Z (above the level)
  static constexpr f32 kSkirtDrop = 0.05f;     // terrain_tiles.mjs SKIRT_DROP_M

  Terrain();
  ~Terrain();
  /// New world (origin of the local metres). `dem`: fetch real elevation (else flat ground).
  void start(kern::Renderer* renderer, kern::physics::World* world, const kern::GeoProjector& proj, bool dem);
  void clear();
  /// No elevation service (offline, dem=0): flat ground from now on.
  void disable_dem();

  // ---- host side ----
  std::string take_requests();
  // ---- ground colour (world_ground.mjs): aerial imagery of the 1600 m around the player, one
  // albedo per 5 m node (never shown as a texture); rebuilt after 350 m of travel ----
  static constexpr i32 kImageZoom = 15, kImagePixels = 64;
  std::string take_image_requests();
  /// RGBA of a requested imagery tile (any size; averaged down to 64 x 64 as the original's mosaic).
  void provide_image(const TileKey& key, const u8* rgba, u32 width, u32 height);
  void fail_image(const TileKey& key);
  void provide(const TileKey& key, const u8* rgba, u32 width, u32 height);
  void fail(const TileKey& key);

  /// The ground's shape is decided (elevation loaded, or flat because there is none).
  bool settled() const { return settled_; }
  bool has_dem() const { return grid_ready_; }
  /// Collision under p is built.
  bool ready_at(Vec3 p) const;

  // ---- heights (one truth for physics, rendering, walkers, cars) ----
  f32 elevation(f32 x, f32 y) const;
  /// Height of a 5 m node: elevation + craters, water basins carved (terrainNodeHeightAt).
  f32 node_height(f32 x, f32 y) const;
  /// The static ground surface at (x, y): the node heights on the SW-NE triangles.
  f32 height(f32 x, f32 y) const;
  Vec3 normal(f32 x, f32 y) const;
  /// Where a building stands: the lowest ground along its outline (buildingGroundBase).
  f32 footprint_base(const std::vector<Vec2>& ring) const;
  kern::HeightSampler sampler(f32 offset = 0) const;

  // ---- water ----
  void add_water_polygon(const std::vector<std::vector<Vec2>>& rings);
  /// A waterway line `width` wide; `speed` m/s current along it (river 1.6, canal 0.5, else 1.1).
  void add_waterway(const std::vector<Vec2>& points, f32 width, f32 speed);
  void add_bridge(Vec2 a, Vec2 b, f32 width);
  bool water_at(f32 x, f32 y) const;  // open water here (not under a bridge)
  f32 water_level(f32 x, f32 y) const { return elevation(x, y) + kWaterLevel; }
  Vec2 flow_at(f32 x, f32 y) const;
  /// Ground surface kind for tyres etc. (groundSurfaceAt): 0 ground, 1 water, 2 soil (crater).
  i32 surface_at(f32 x, f32 y) const;

  // ---- map areas (world_ground.mjs paintAreas) ----
  /// Parks, woods, pitches, sand tint the ground's vertex colours (node * .45 + colour * .55,
  /// linear): the ground already is the surface, so they cost no triangles. `owner`: the map tile.
  void add_tint(u64 owner, const std::vector<std::vector<Vec2>>& rings, Vec3 srgb);
  void remove_tints(u64 owner);

  // ---- craters ----
  /// A bowl of `radius` (rim included) and `depth` (terrain_craters.mjs craterProfile, scaled).
  void add_crater(Vec2 at, f32 radius, f32 depth);
  /// Whether a circle (x, y, r) touches a crater (the roads drape finely there: city_stream.cpp).
  bool deformed(f32 x, f32 y, f32 r) const {
    for (const Crater& c : craters_) if (std::hypot(x - c.c.x, y - c.c.y) < c.r + r + 2.5f) return true;
    return false;
  }
  u32 crater_count() const { return static_cast<u32>(craters_.size()); }
  /// One crater as it goes over the network (destruction sync: game_destruction_sync.cpp).
  struct CraterRec { f32 x = 0, y = 0, r = 0, depth = 0; bool operator==(const CraterRec& o) const { return x == o.x && y == o.y && r == o.r && depth == o.depth; } };
  void crater_list(std::vector<CraterRec>& out) const;
  /// The host's craters, in his order: only what differs is dug or filled (its tiles rebuilt).
  void set_craters(const std::vector<CraterRec>& list);
  /// Regions changed since the last call (city roads/areas re-lay themselves on them).
  void take_changes(std::vector<kern::Vec4>& out) { out.insert(out.end(), changes_.begin(), changes_.end()); changes_.clear(); }

  /// Builds elevation, collision tiles around focus and anchor, render tiles, water meshes.
  void update(Vec3 focus, Vec3 anchor, f64 budget_ms);
  /// Where the focus will be in a while (its velocity x 12 s): the elevation window is loaded
  /// around there, so a fast jet never flies past the edge of the known ground.
  Vec3 lead;
  /// MP host: the other players - ground collision 300 m around each of them too (his traffic
  /// drives there with real physics: game_worldsync.cpp).
  std::vector<Vec3> extra;
  /// Whether ground collision stands at (x, y) (a car may run its physics there).
  bool has_collision(f32 x, f32 y) const;
  /// Whether the ground at (x, y) is final: the elevation there is loaded (no reload running that
  /// would move it) and its collision stands - a player may be put down there.
  bool ground_ready(f32 x, f32 y) const;
  const TerrainStats& stats() const { return stats_; }

 private:
  struct DemTile { TileKey key; std::vector<f32> h; u32 w = 0, hgt = 0; bool failed = false, requested = false; };
  struct Grid { f32 x0 = 0, y0 = 0, step = 10; u32 n = 0; std::vector<f32> h; f32 cx = 0, cy = 0; };
  struct Job;
  struct RenderTile { u32 mesh = 0; bool skirt = true; };
  struct CollisionTile { kern::physics::Body body; };
  struct WaterBlock { u64 bits[4] = {}; std::vector<Vec2> flow; std::vector<f32> speed; };
  struct Bridge { Vec2 c; f32 hl, hw, yaw; };
  struct Crater { Vec2 c; f32 r, depth, rim; };
  struct Tint { u64 owner; kern::Vec4 box; Vec3 lin; std::vector<std::vector<Vec2>> rings; };
  struct Image { TileKey key; std::vector<f32> albedo; bool requested = false, failed = false; };  // 64 x 64 linear rgb
  std::unordered_map<u64, Image> images_;
  std::vector<TileKey> image_requests_;
  Vec2 image_at_{1e9f, 1e9f};
  void plan_images(Vec3 focus);
  bool ground_albedo(f32 x, f32 y, Vec3& lin) const;
  kern::Vec4 image_bounds(const TileKey& k) const;

  void plan_dem(Vec3 focus, Vec3 player, bool mates);
  bool run_job(f64 deadline);
  f32 sample_dem(f64 lon, f64 lat, bool& ok) const;
  f32 grid_at(const Grid& g, f32 x, f32 y) const;
  bool water_cell(i32 cx, i32 cy) const;
  mutable u64 wc_key_ = ~0ull;            // water_cell's last block (its key, its bits or none)
  mutable const u64* wc_bits_ = nullptr;
  void set_water_cell(i32 cx, i32 cy);
  bool on_bridge(f32 x, f32 y) const;
  void dirty(f32 x0, f32 y0, f32 x1, f32 y1);
  void build_render_tile(i32 level, i32 i, i32 j);
  void build_collision_tile(i32 i, i32 j);
  void build_water_chunk(i32 i, i32 j, bool detail);
  void want_tiles(Vec3 focus, Vec3 anchor);

  kern::Renderer* r_ = nullptr;
  kern::physics::World* world_ = nullptr;
  kern::GeoProjector proj_;
  bool dem_ = false, settled_ = true, grid_ready_ = false;
  f32 ref_height_ = 0;
  bool ref_ready_ = false;
  Grid grid_;
  std::unique_ptr<Job> job_;
  std::unordered_map<u64, DemTile> dem_tiles_;
  std::vector<TileKey> requests_;
  u32 dem_failures_ = 0;
  f64 retry_at_ = 0;
  // water raster: 64 m blocks of 16 x 16 cells (4 m)
  std::unordered_map<u64, WaterBlock> water_;
  std::vector<Bridge> bridges_;
  std::vector<Crater> craters_;
  std::vector<Tint> tints_;
  void repaint(const kern::Vec4& box);
  // tiles: render keyed by (level, i, j), collision by (i, j)
  std::unordered_map<u64, RenderTile> render_;
  std::unordered_map<u64, CollisionTile> collision_;
  std::unordered_map<u64, u32> water_meshes_;
  std::unordered_map<u64, bool> water_detail_;  // the chunk is drawn cell by cell (world_water.mjs, 1000 m)
  std::unordered_set<u64> dirty_render_, dirty_collision_, dirty_water_;
  std::vector<u64> want_render_, want_collision_, want_water_;
  Vec3 planned_focus_{1e9f, 1e9f, 0}, planned_anchor_{1e9f, 1e9f, 0};
  std::vector<Vec3> planned_extra_;
  bool replan_ = true;
  std::vector<kern::Vec4> changes_;
  TerrainStats stats_;
};

/// Bowl + raised rim, 0 beyond the radius (terrain_craters.mjs craterProfile, CRATER_R 210 /
/// DEPTH 22 / RIM 7 there; scaled by radius here).
f32 crater_profile(f32 r, f32 radius, f32 depth, f32 rim);

/// Terrarium RGB -> metres.
inline f32 terrarium_height(const u8* px) { return static_cast<f32>(px[0]) * 256.0f + static_cast<f32>(px[1]) + static_cast<f32>(px[2]) / 256.0f - 32768.0f; }

/// Morphological opening + two box blurs over an n x n grid (terrain_elevation.mjs bareEarth).
void bare_earth(std::vector<f32>& h, u32 n, f32 step, f32 open_m, f32 blur_m);

}  // namespace sc
