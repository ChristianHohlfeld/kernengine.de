// Terrain: crater profile, bare earth (roofs removed, hills kept), Terrarium tiles -> relative
// heights, ground tiles that a ray hits at height(), water basins, bridges and the current.
#include "../src/terrain.h"

#include <cstring>

#include "kern/core/base.h"
#include "test_util.h"

using namespace sc;

SC_TEST(terrain_crater_profile) {
  SC_CHECK(std::fabs(crater_profile(0, 10, 2, 0.6f) + 2) < 1e-4f, "centre at -depth");
  SC_CHECK(crater_profile(10, 10, 2, 0.6f) == 0 && crater_profile(30, 10, 2, 0.6f) == 0, "nothing beyond the radius");
  f32 rim = -1;
  for (f32 r = 5; r < 10; r += 0.1f) rim = std::fmax(rim, crater_profile(r, 10, 2, 0.6f));
  SC_CHECK(rim > 0.3f, "a raised rim (%.2f m)", rim);
}

SC_TEST(terrain_bare_earth) {
  const u32 n = 201;
  const f32 step = 10;
  std::vector<f32> h(n * n);
  for (u32 j = 0; j < n; ++j)
    for (u32 i = 0; i < n; ++i) {
      const f32 x = (static_cast<f32>(i) - 100) * step, y = (static_cast<f32>(j) - 100) * step;
      h[j * n + i] = 400 + 60 * std::exp(-(x * x + y * y) / (2 * 400.0f * 400.0f));  // a real hill
    }
  std::vector<f32> bare = h;
  bare_earth(bare, n, step, Terrain::kOpen, Terrain::kBlur);
  // a city block (40 x 40 m, 15 m high) somewhere on the slope
  for (u32 j = 130; j < 134; ++j)
    for (u32 i = 60; i < 64; ++i) h[j * n + i] += 15;
  const f32 hill = h[100 * n + 100];
  bare_earth(h, n, step, Terrain::kOpen, Terrain::kBlur);
  SC_CHECK(std::fabs(h[100 * n + 100] - hill) < 6, "the hill stays (%.1f -> %.1f)", hill, h[100 * n + 100]);
  SC_CHECK(std::fabs(h[132 * n + 62] - bare[132 * n + 62]) < 0.5f, "the block is gone (%.1f vs %.1f without it)", h[132 * n + 62], bare[132 * n + 62]);
}

namespace {
// Terrarium tile with h = 400 m + 0.5 m per z13 pixel eastwards.
std::vector<u8> ramp_tile(const TileKey& k) {
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

SC_TEST(terrain_dem_tiles) {
  u8 px[4];
  const f64 v = 512.25 + 32768;
  px[0] = static_cast<u8>(static_cast<u32>(v) / 256); px[1] = static_cast<u8>(static_cast<u32>(v) % 256); px[2] = 64;
  SC_CHECK(std::fabs(terrarium_height(px) - 512.25f) < 0.01f, "terrarium decode %.2f", terrarium_height(px));

  kern::Renderer r;
  r.init(kern::RendererOptions{});
  kern::physics::World world;
  world.create(kern::physics::WorldSettings{});
  kern::GeoProjector proj;
  proj.init(kKonstanzLon, kKonstanzLat);
  Terrain t;
  t.start(&r, &world, proj, true);
  SC_CHECK(!t.settled(), "waits for its elevation");
  t.update({0, 0, 0}, {0, 0, 0}, 4);
  std::string req = t.take_requests();
  SC_CHECK(req.size() > 10 && req.find("[13,") != std::string::npos, "asks for z13 tiles: %s", req.c_str());
  // answer every request
  for (std::size_t at = 0; (at = req.find('[', at + 1)) != std::string::npos;) {
    int z, x, y;
    if (std::sscanf(req.c_str() + at, "[%d,%d,%d]", &z, &x, &y) != 3) continue;
    const TileKey k{z, x, y};
    const std::vector<u8> tile = ramp_tile(k);
    t.provide(k, tile.data(), 256, 256);
  }
  int frames = 0;
  while (!t.ready_at({0, 0, 0}) && frames < 4000) { t.update({0, 0, 0}, {0, 0, 0}, 3); ++frames; }
  SC_CHECK(t.settled() && t.has_dem(), "elevation loaded after %d frames", frames);
  SC_CHECK(std::fabs(t.elevation(0, 0)) < 0.05f, "origin at z = 0 (%.3f)", t.elevation(0, 0));
  // z13 pixel at 47.66 N: 40075017 * cos(47.66) / 2^21 = 12.88 m -> 0.5 / 12.88 per metre
  const f32 east = t.elevation(1000, 0), north = t.elevation(0, 1000);
  SC_CHECK(std::fabs(east - 38.8f) < 3, "1 km east is higher (%.1f m)", east);
  SC_CHECK(std::fabs(north) < 1, "1 km north the same (%.1f m)", north);
  SC_CHECK(t.ready_at({0, 0, 0}), "ground collision under the origin");
  for (Vec2 p : {Vec2{37, -21}, Vec2{-120.5f, 64.2f}, Vec2{3, 3}}) {
    const RayResult h = cast(world, {p.x, p.y, 500}, {0, 0, -1}, 1000, {});
    SC_CHECK(h.hit && std::fabs(h.point.z - t.height(p.x, p.y)) < 0.05f, "ray hits the ground at height() (%.2f vs %.2f)", h.point.z, t.height(p.x, p.y));
  }
  for (int i = 0; i < 300; ++i) t.update({0, 0, 0}, {0, 0, 0}, 4);
  SC_CHECK(t.stats().render_tiles > 20, "render tiles around the player (%u)", t.stats().render_tiles);
}

SC_TEST(terrain_water) {
  kern::Renderer r;
  r.init(kern::RendererOptions{});
  kern::physics::World world;
  world.create(kern::physics::WorldSettings{});
  kern::GeoProjector proj;
  proj.init(kKonstanzLon, kKonstanzLat);
  Terrain t;
  t.start(&r, &world, proj, false);
  SC_CHECK(t.settled(), "flat ground is settled at once");
  t.add_water_polygon({{{0, 0}, {80, 0}, {80, 60}, {0, 60}}});
  SC_CHECK(t.water_at(40, 30) && !t.water_at(100, 30), "lake cells");
  SC_CHECK(std::fabs(t.height(40, 30) + Terrain::kWaterBed) < 0.01f, "a basin %.2f m deep", -t.height(40, 30));
  SC_CHECK(std::fabs(t.water_level(40, 30) - Terrain::kWaterLevel) < 1e-4f, "surface just below the bank");
  t.add_bridge({40, -10}, {40, 70}, 6);
  SC_CHECK(!t.water_at(40, 30) && std::fabs(t.height(40, 30)) < 0.01f, "the bridge keeps the ground");
  t.add_waterway({{-200, 200}, {200, 200}}, 18, 1.6f);
  SC_CHECK(t.water_at(0, 202) && !t.water_at(0, 220), "river 18 m wide");
  const Vec2 f = t.flow_at(0, 200);
  SC_CHECK(f.x > 1.2f && std::fabs(f.y) < 0.01f, "the river flows east (%.2f, %.2f)", f.x, f.y);
  t.add_crater({300, 300}, 8, 2);
  SC_CHECK(t.height(300, 300) < -1.9f && t.surface_at(300, 300) == 2, "a crater (%.2f)", t.height(300, 300));
  for (int i = 0; i < 400 && !t.ready_at({40, 30, 0}); ++i) t.update({40, 30, 0}, {40, 30, 0}, 4);
  const RayResult h = cast(world, {20, 20, 50}, {0, 0, -1}, 100, {});
  SC_CHECK(h.hit && std::fabs(h.point.z + Terrain::kWaterBed) < 0.1f, "the lake bed is solid (%.2f)", h.point.z);
}
