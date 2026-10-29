// City streaming: a requested tile arrives, is decoded, extruded, uploaded and given collision in
// time slices; a ray hits the roof at the building's height; far tiles unload; the stand-in town.
#include "../src/city_stream.h"
#include <cstdlib>

#include "kern/core/base.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
std::vector<u8> sample_tile() {
  mvt::Writer w;
  w.begin_layer("building");
  mvt::Value h; h.kind = mvt::Value::Kind::number; h.n = 21;
  // a 40 x 40 tile-unit block (about 16 m) at the tile centre
  w.feature(mvt::GeomType::polygon, {{{2028, 2028}, {2068, 2028}, {2068, 2068}, {2028, 2068}}}, {{"render_height", h}}, 7);
  for (int i = 0; i < 400; ++i) {
    const f32 x = 200 + (i % 20) * 180.0f, y = 200 + (i / 20) * 180.0f;
    w.feature(mvt::GeomType::polygon, {{{x, y}, {x + 60, y}, {x + 60, y + 50}, {x, y + 50}}}, {}, 100 + i);
  }
  w.end_layer();
  w.begin_layer("transportation");
  mvt::Value cls; cls.kind = mvt::Value::Kind::string; cls.s = "secondary";
  w.feature(mvt::GeomType::line, {{{0, 2200}, {4096, 2200}}}, {{"class", cls}});
  w.end_layer();
  return w.bytes();
}
}  // namespace

SC_TEST(stream_tile_pipeline) {
  kern::Renderer r;
  r.init(kern::RendererOptions{});
  kern::physics::World world;
  make_flat_world(world);
  CityStream city;
  const TileKey key = tile_at(kKonstanzLon, kKonstanzLat);
  f64 lon, lat;
  tile_lonlat(key, 0.5, 0.5, lon, lat);
  city.start(r, world, lon, lat);
  city.ring = 0;
  city.update({0, 0, 0}, 2);
  const std::string req = city.take_requests();
  char want[64];
  std::snprintf(want, sizeof want, "[[%d,%d,%d]]", key.z, key.x, key.y);
  SC_CHECK(req == want, "requests the player's tile: %s", req.c_str());
  const std::vector<u8> bytes = sample_tile();
  city.provide(key, bytes.data(), bytes.size());
  int frames = 0;
  f64 worst = 0;
  while (!city.ready_at({0, 0, 0}) && frames < 2000) {
    const f64 t0 = kern::now_ms();
    city.update({0, 0, 0}, 3);
    const f64 took = kern::now_ms() - t0;
    if (took > worst) { worst = took; if (std::getenv("SC_VERBOSE")) std::printf("  frame %d took %.1f ms\n", frames, took); }
    ++frames;
  }
  SC_CHECK(city.ready_at({0, 0, 0}), "tile built after %d frames", frames);
  SC_CHECK(frames > 1, "built over several frames (%d)", frames);
  SC_CHECK(worst < 40, "no long frame (worst slice %.1f ms, budget 3 ms; sanitizers on)", worst);
  SC_CHECK(city.stats().colliders == 401, "one collider per building (%u)", city.stats().colliders);
  SC_CHECK(city.stats().meshes >= 2, "building and road meshes (%u)", city.stats().meshes);
  SC_CHECK(city.lanes().size() == 1, "one drivable lane");
  // The block in the middle: 21 m high.
  const RayResult hit = cast(world, {0, 0, 100}, {0, 0, -1}, 200, {});
  SC_CHECK(hit.hit && std::fabs(hit.point.z - 21) < 0.05f, "roof at 21 m (hit %d z %.2f)", hit.hit, hit.point.z);
  // Walls stop a horizontal ray.
  const RayResult wall = cast(world, {-30, 0.5f, 5}, {1, 0, 0}, 60, {});
  SC_CHECK(wall.hit && wall.normal.x < -0.9f, "wall facing west (hit %d n %.2f)", wall.hit, wall.normal.x);
  Rng rng(3);
  Vec3 p; f32 heading;
  SC_CHECK(city.street_point(rng, {0, 0, 0}, 500, p, heading), "street point");
  // Moving far away unloads it.
  city.update({20000, 0, 0}, 1);
  city.take_requests();
  SC_CHECK(city.stats().colliders == 0 && city.stats().meshes == 0, "far tile unloaded (%u colliders)", city.stats().colliders);
}

SC_TEST(stream_offline_town) {
  kern::Renderer r;
  r.init(kern::RendererOptions{});
  kern::physics::World world;
  make_flat_world(world);
  CityStream city;
  city.start(r, world, kKonstanzLon, kKonstanzLat);
  city.update({0, 0, 0}, 1);
  const TileKey key = tile_at(kKonstanzLon, kKonstanzLat);
  city.fail(key);  // no map service
  SC_CHECK(city.offline(), "falls back to the stand-in town");
  int frames = 0;
  while (!city.ready_at({0, 0, 0}) && frames++ < 3000) city.update({0, 0, 0}, 4);
  SC_CHECK(city.ready_at({0, 0, 0}), "stand-in town built");
  SC_CHECK(city.stats().colliders > 100, "with collision (%u)", city.stats().colliders);
  SC_CHECK(!city.has_real_data(), "marked as not real map data");
}
