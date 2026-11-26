// Terrain: crater profile, bare earth (roofs removed, hills kept), Terrarium tiles -> relative
// heights, ground tiles that a ray hits at height(), water basins, bridges and the current. The terrain is
// the level's Blueprint terrain_cpp (its structs: terrain), called as its class - as the program calls it
// (level_blueprints.h), with the game's loop guard - on the engine itself: the renderer and the physics world
// are the test's, given to it as its objects of the engine (Object<kern::Renderer>, Object<kern::physics::World>).
#include <cstring>
#include <memory>

#include "../src/blueprint_call.h"
#include "kern/core/base.h"
#include "kern/gfx/renderer.h"
#include "kern/physics/physics.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

namespace {
using Tr = blueprints::terrain_cpp;
using Common = blueprints::common;   // (the ray onto the world, cast, is the project's Blueprint common's - as the program calls it)
/// terrain_cpp running as its class, on the engine itself (kern::test::BlueprintRun::Engine::direct), common beside it.
struct TerrainBlueprint {
  kern::test::BlueprintRun rules{"terrain_cpp", nullptr, true, {"common"}, kern::test::BlueprintRun::Engine::direct};
  bool bound = false;
  TerrainBlueprint() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Tr>(rules.rt) && kern::logic::bind_blueprint<Common>(rules.rt);
  }
  Tr& operator*() { return kern::logic::blueprint<Tr>(); }
  Common& common() { return kern::logic::blueprint<Common>(); }
};
/// Konstanz, where the game starts (tiles.h).
constexpr f64 kLon = 9.1766, kLat = 47.6600;
/// The renderer and the physics world of a test, the terrain started on them.
struct TerrainRig {
  TerrainBlueprint bp;
  kern::Renderer r;
  kern::physics::World world;
  kern::logic::Object<kern::physics::World> world_object;
  std::unique_ptr<Tr::Terrain> t = std::make_unique<Tr::Terrain>();
  explicit TerrainRig(bool dem) {
    r.init(kern::RendererOptions{});
    world.create(kern::physics::WorldSettings{});
    if (!bp.bound) return;
    Tr::GeoProjector proj;
    (*bp).GeoProjector_init(proj, kLon, kLat);
    world_object = bp.rules.object(world);
    (*bp).Terrain_start(*t, proj, bp.rules.object(r), world_object, dem);
  }
  /// A ray straight down onto the world, as the program casts it (common: cast).
  Common::the_RayResult_ down(f64 x, f64 y, f64 from, f64 len) { return bp.common().cast(world_object, {static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(from)}, {0, 0, -1}, len, {}, Common::Int64{~0ull}).Result; }
};
Tr::List<Tr::U8> bytes_of(const std::vector<u8>& v) {
  Tr::List<Tr::U8> out;
  for (u8 b : v) out.push_back(Tr::U8(b));
  return out;
}
}  // namespace

KERN_TEST(terrain_crater_profile) {
  TerrainBlueprint bp;
  KERN_CHECK(bp.bound, "terrain_cpp and common run as their classes: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  Tr& tr = *bp;
  auto crater_profile = [&](f64 r, f64 radius, f64 depth, f64 rim) { return static_cast<f64>(tr.crater_profile(r, radius, depth, rim).Result); };
  KERN_CHECK(std::fabs(crater_profile(0, 10, 2, 0.6) + 2) < 1e-4f, "centre at -depth");
  KERN_CHECK(crater_profile(10, 10, 2, 0.6) == 0 && crater_profile(30, 10, 2, 0.6) == 0, "nothing beyond the radius");
  f64 rim = -1;
  for (f32 r = 5; r < 10; r += 0.1f) rim = std::fmax(rim, crater_profile(r, 10, 2, 0.6));
  KERN_CHECK(rim > 0.3f, "a raised rim (%.2f m)", rim);
}

KERN_TEST(terrain_bare_earth) {
  TerrainBlueprint bp;
  KERN_CHECK(bp.bound, "terrain_cpp and common run as their classes: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  Tr& tr = *bp;
  const u32 n = 201;
  const f32 step = 10;
  Tr::List<Tr::Float> h(std::vector<f64>(n * n));
  for (u32 j = 0; j < n; ++j)
    for (u32 i = 0; i < n; ++i) {
      const f32 x = (static_cast<f32>(i) - 100) * step, y = (static_cast<f32>(j) - 100) * step;
      h[j * n + i] = 400 + 60 * std::exp(-(x * x + y * y) / (2 * 400.0f * 400.0f));  // a real hill
    }
  Tr::List<Tr::Float> bare = h;
  tr.bare_earth(bare, n, step, Tr::kOpen, Tr::kBlur);
  // a city block (40 x 40 m, 15 m high) somewhere on the slope
  for (u32 j = 130; j < 134; ++j)
    for (u32 i = 60; i < 64; ++i) h[j * n + i] += 15;
  const f64 hill = h[100 * n + 100];
  tr.bare_earth(h, n, step, Tr::kOpen, Tr::kBlur);
  KERN_CHECK(std::fabs(h[100 * n + 100] - hill) < 6, "the hill stays (%.1f -> %.1f)", hill, h[100 * n + 100]);
  KERN_CHECK(std::fabs(h[132 * n + 62] - bare[132 * n + 62]) < 0.5f, "the block is gone (%.1f vs %.1f without it)", h[132 * n + 62], bare[132 * n + 62]);
}

namespace {
// Terrarium tile with h = 400 m + 0.5 m per z13 pixel eastwards.
std::vector<u8> ramp_tile(const Tr::TileKey& k) {
  std::vector<u8> px(256 * 256 * 4);
  for (u32 y = 0; y < 256; ++y)
    for (u32 x = 0; x < 256; ++x) {
      const f64 v = 400 + 0.5 * (static_cast<f64>(k.x) * 256 + x) - 0.5 * 256.0 * 4380 + 32768;
      u8* p = &px[(y * 256 + x) * 4];
      p[0] = static_cast<u8>(static_cast<u32>(v) / 256);
      p[1] = static_cast<u8>(static_cast<u32>(v) % 256);
      p[2] = static_cast<u8>((v - std::floor(v)) * 256);
      p[3] = 255;
    }
  return px;
}
}  // namespace

KERN_TEST(terrain_dem_tiles) {
  TerrainRig rig(true);
  KERN_CHECK(rig.bp.bound, "terrain_cpp and common run as their classes: %s", rig.bp.rules.problems.c_str());
  if (!rig.bp.bound) return;
  Tr& tr = *rig.bp;
  Tr::Terrain& t = *rig.t;
  u8 px[4];
  const f64 v = 512.25 + 32768;
  px[0] = static_cast<u8>(static_cast<u32>(v) / 256); px[1] = static_cast<u8>(static_cast<u32>(v) % 256); px[2] = 64;
  const f64 decoded = tr.terrarium_height(px[0], px[1], px[2]).Result;
  KERN_CHECK(std::fabs(decoded - 512.25f) < 0.01f, "terrarium decode %.2f", decoded);

  KERN_CHECK(!tr.Terrain_settled(t).Result, "waits for its elevation");
  tr.Terrain_update(t, {0, 0, 0}, {0, 0, 0}, 4);
  std::string req = tr.Terrain_take_requests(t).Result.str();
  KERN_CHECK(req.size() > 10 && req.find("[13,") != std::string::npos, "asks for z13 tiles: %s", req.c_str());
  // answer every request
  for (std::size_t at = 0; (at = req.find('[', at + 1)) != std::string::npos;) {
    int z, x, y;
    if (std::sscanf(req.c_str() + at, "[%d,%d,%d]", &z, &x, &y) != 3) continue;
    const Tr::TileKey k{z, x, y};
    tr.Terrain_provide(t, k, bytes_of(ramp_tile(k)), 256, 256);
  }
  int frames = 0;
  // the elevation grid is built in a job (another thread here): wait for it by the clock
  const f64 give_up = kern::now_ms() + 60000;
  while (!tr.Terrain_ready_at(t, {0, 0, 0}).Result && kern::now_ms() < give_up) { tr.Terrain_update(t, {0, 0, 0}, {0, 0, 0}, 3); ++frames; }
  KERN_CHECK(tr.Terrain_settled(t).Result && tr.Terrain_has_dem(t).Result, "elevation loaded after %d frames", frames);
  const f64 origin = tr.Terrain_elevation(t, 0, 0).Result;
  KERN_CHECK(std::fabs(origin) < 0.05f, "origin at z = 0 (%.3f)", origin);
  // z13 pixel at 47.66 N: 40075017 * cos(47.66) / 2^21 = 12.88 m -> 0.5 / 12.88 per metre
  const f64 east = tr.Terrain_elevation(t, 1000, 0).Result, north = tr.Terrain_elevation(t, 0, 1000).Result;
  KERN_CHECK(std::fabs(east - 38.8f) < 3, "1 km east is higher (%.1f m)", east);
  KERN_CHECK(std::fabs(north) < 1, "1 km north the same (%.1f m)", north);
  KERN_CHECK(tr.Terrain_ready_at(t, {0, 0, 0}).Result, "ground collision under the origin");
  for (Vec2 p : {Vec2{37, -21}, Vec2{-120.5f, 64.2f}, Vec2{3, 3}}) {
    const Common::the_RayResult_ h = rig.down(p.x, p.y, 500, 1000);
    const f64 at = tr.Terrain_height(t, p.x, p.y).Result;
    KERN_CHECK(h.hit && std::fabs(h.point.z - at) < 0.05f, "ray hits the ground at height() (%.2f vs %.2f)", static_cast<f64>(h.point.z), at);
  }
  for (int i = 0; i < 300; ++i) tr.Terrain_update(t, {0, 0, 0}, {0, 0, 0}, 4);
  KERN_CHECK(t.stats_.render_tiles > 20, "render tiles around the player (%u)", static_cast<unsigned>(t.stats_.render_tiles));
}

KERN_TEST(terrain_water) {
  TerrainRig rig(false);
  KERN_CHECK(rig.bp.bound, "terrain_cpp and common run as their classes: %s", rig.bp.rules.problems.c_str());
  if (!rig.bp.bound) return;
  Tr& tr = *rig.bp;
  Tr::Terrain& t = *rig.t;
  auto height = [&](f64 x, f64 y) { return static_cast<f64>(tr.Terrain_height(t, x, y).Result); };
  auto water_at = [&](f64 x, f64 y) { return static_cast<bool>(tr.Terrain_water_at(t, x, y).Result); };
  KERN_CHECK(tr.Terrain_settled(t).Result, "flat ground is settled at once");
  Tr::List<Tr::Vector_row> lake;
  lake.push_back(Tr::Vector_row{Tr::List<Tr::Vector>(std::vector<Vec3>{{0, 0, 0}, {80, 0, 0}, {80, 60, 0}, {0, 60, 0}})});
  tr.Terrain_add_water_polygon(t, lake);
  KERN_CHECK(water_at(40, 30) && !water_at(100, 30), "lake cells");
  KERN_CHECK(std::fabs(height(40, 30) + Tr::kWaterBed) < 0.01f, "a basin %.2f m deep", -height(40, 30));
  KERN_CHECK(std::fabs(tr.Terrain_water_level(t, 40, 30).Result - Tr::kWaterLevel) < 1e-4f, "surface just below the bank");
  tr.Terrain_add_bridge(t, {40, -10, 0}, {40, 70, 0}, 6);
  KERN_CHECK(!water_at(40, 30) && std::fabs(height(40, 30)) < 0.01f, "the bridge keeps the ground");
  tr.Terrain_add_waterway(t, Tr::List<Tr::Vector>(std::vector<Vec3>{{-200, 200, 0}, {200, 200, 0}}), 18, 1.6);
  KERN_CHECK(water_at(0, 202) && !water_at(0, 220), "river 18 m wide");
  const Vec3 f = tr.Terrain_flow_at(t, 0, 200).Result;
  KERN_CHECK(f.x > 1.2f && std::fabs(f.y) < 0.01f, "the river flows east (%.2f, %.2f)", f.x, f.y);
  tr.Terrain_add_crater(t, {300, 300, 0}, 8, 2);
  KERN_CHECK(height(300, 300) < -1.9f && tr.Terrain_surface_at(t, 300, 300).Result == 2, "a crater (%.2f)", height(300, 300));
  for (int i = 0; i < 400 && !tr.Terrain_ready_at(t, {40, 30, 0}).Result; ++i) tr.Terrain_update(t, {40, 30, 0}, {40, 30, 0}, 4);
  const Common::the_RayResult_ h = rig.down(20, 20, 50, 100);
  KERN_CHECK(h.hit && std::fabs(h.point.z + Tr::kWaterBed) < 0.1f, "the lake bed is solid (%.2f)", static_cast<f64>(h.point.z));
}

// world_ground.mjs: the aerial tiles of the 1600 m around the player are asked for once (z 15),
// averaged to an albedo per node; the ground is rebuilt with them and nothing else is fetched.
KERN_TEST(terrain_ground_imagery) {
  TerrainRig rig(false);
  KERN_CHECK(rig.bp.bound, "terrain_cpp and common run as their classes: %s", rig.bp.rules.problems.c_str());
  if (!rig.bp.bound) return;
  Tr& tr = *rig.bp;
  Tr::Terrain& t = *rig.t;
  auto requests = [&] { return tr.Terrain_take_image_requests(t).Result.str(); };
  tr.Terrain_update(t, {0, 0, 0}, {0, 0, 0}, 50);
  const std::string req = requests();
  u32 n = 0;
  for (char c : req) n += c == '[';
  n -= 1;  // the outer bracket
  KERN_CHECK(n >= 4 && n <= 9 && req.find("[15,") != std::string::npos, "the z15 tiles around the player: %s", req.c_str());
  // a green field everywhere
  std::vector<u8> px(256 * 256 * 4);
  for (std::size_t i = 0; i < px.size(); i += 4) { px[i] = 60; px[i + 1] = 110; px[i + 2] = 40; px[i + 3] = 255; }
  const Tr::List<Tr::U8> field = bytes_of(px);
  std::size_t at = 1;
  while ((at = req.find('[', at)) != std::string::npos) {
    int z, x, y;
    if (std::sscanf(req.c_str() + at, "[%d,%d,%d]", &z, &x, &y) == 3) tr.Terrain_provide_image(t, Tr::TileKey{z, x, y}, field, 256, 256);
    ++at;
  }
  for (int i = 0; i < 50; ++i) tr.Terrain_update(t, {0, 0, 0}, {0, 0, 0}, 20);
  KERN_CHECK(requests() == "[]", "nothing more to fetch while the player stays");
  tr.Terrain_update(t, {200, 0, 0}, {200, 0, 0}, 20);
  KERN_CHECK(requests() == "[]", "200 m is not 350 m: no new window");
  tr.Terrain_update(t, {2000, 0, 0}, {2000, 0, 0}, 20);
  KERN_CHECK(requests() != "[]", "a new window after 350 m");
}
