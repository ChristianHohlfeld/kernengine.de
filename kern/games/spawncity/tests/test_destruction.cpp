// Buildings broken by weapons (destruction.h): a blast takes whole floors off the house it hits,
// the collider follows at once, the drawn chunk is swapped after its job, the debris lands, more
// blasts level it to its stump, the damage outlives nothing but RESET; and nothing leaks.
#include "../src/destruction.h"
#include "../src/world_marks.h"

#include "kern/core/base.h"
#include "test_util.h"
#include "world_util.h"

using namespace sc;

namespace {
std::vector<u8> one_house_tile() {
  mvt::Writer w;
  w.begin_layer("building");
  mvt::Value h; h.kind = mvt::Value::Kind::number; h.n = 21;
  // a 40 x 40 tile-unit block (about 16 m) at the tile centre, 21 m high
  w.feature(mvt::GeomType::polygon, {{{2028, 2028}, {2068, 2028}, {2068, 2068}, {2028, 2068}}}, {{"render_height", h}}, 7);
  w.end_layer();
  return w.bytes();
}

f32 roof(kern::physics::World& world) {
  const RayResult hit = cast(world, {0, 0, 100}, {0, 0, -1}, 200, {});
  return hit.hit ? hit.point.z : -1;
}
}  // namespace

SC_TEST(destruction_floors_collapse_and_reset) {
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
  city.take_requests();
  const std::vector<u8> bytes = one_house_tile();
  city.provide(key, bytes.data(), bytes.size());
  const f64 give_up = kern::now_ms() + 60000;
  while ((city.stats().t_built < 0 || city.stats().t_collision < 0) && kern::now_ms() < give_up) city.update({0, 0, 0}, 3);
  SC_CHECK(std::fabs(roof(world) - 21) < 0.05f, "the house stands 21 m (%.2f)", roof(world));
  // facade_marks.mjs glassAt / addGlassCrack: rounds on a wall crack its panes, not its piers
  {
    Marks marks;
    Rng mr{9};
    std::vector<CityStream::Wall> walls;
    city.walls_near({0, 0}, 20, walls);
    SC_CHECK(!walls.empty(), "the house has walls");
    u32 glass = 0, tries = 0;
    if (!walls.empty()) {
      const CityStream::Wall& w = walls[0];
      for (f32 z = w.base + 4.5f; z < w.top - 1.5f; z += 0.4f)
        for (f32 u = 0.2f; u < w.len - 0.2f; u += 0.3f) {
          ++tries;
          glass += marks.bullet({w.a.x + w.dir.x * u, w.a.y + w.dir.y * u, z}, city, mr) ? 1 : 0;
        }
    }
    SC_CHECK(glass > tries / 20 && glass < tries * 3 / 4, "some hits are panes (%u of %u)", glass, tries);
  }
  const u32 meshes0 = city.stats().meshes;
  const std::size_t phys0 = world.mesh_count();

  Destruction d;
  d.init(r);
  Rng rng{5};
  // a rocket on the roof (blast 9.5 m, 160): whole floors off, the collider at once
  SC_CHECK(d.blast({0, 0, 22}, 9.5f, 160, false, city, rng) == 1, "one building hit");
  const f32 after = roof(world);
  SC_CHECK(after < 20.6f && after > 1 && std::fabs(std::remainder(after, 3.2f)) < 0.05f, "whole floors off: roof %.2f (a multiple of 3.2 m)", after);
  SC_CHECK(world.mesh_count() == phys0, "one collider replaced, none left behind (%zu -> %zu)", phys0, world.mesh_count());
  // a weak blast far off the house does nothing
  SC_CHECK(d.blast({30, 30, 0}, 6, 60, false, city, rng) == 0, "out of reach");
  // the drawn chunk is swapped after its job; the debris flies and lands
  for (int i = 0; i < 400; ++i) { city.update({0, 0, 0}, 3); d.update(1 / 60.0f, city, rng); }
  SC_CHECK(city.stats().meshes == meshes0, "the chunk swapped, not added (%u -> %u meshes)", meshes0, city.stats().meshes);
  SC_CHECK(d.stats().pieces > 0 && d.stats().torn == 0, "debris out (%u), the torn piece broke up", d.stats().pieces);
  // more rockets: it falls to its 0.8 m stump and takes no more
  for (int k = 0; k < 20 && roof(world) > 1; ++k) d.blast({0, 0, roof(world) + 1}, 9.5f, 160, false, city, rng);
  SC_CHECK(std::fabs(roof(world) - 0.8f) < 0.05f, "levelled to the rubble stump (%.2f)", roof(world));
  SC_CHECK(d.blast({0, 0, 2}, 9.5f, 160, false, city, rng) == 0, "a stump takes nothing more");
  SC_CHECK(city.damaged_buildings() == 1, "one damaged building on record");
  SC_CHECK(world.mesh_count() == phys0, "still no collider left behind (%zu)", world.mesh_count());
  // the debris goes after 22 s
  for (int i = 0; i < 60 * 23; ++i) d.update(1 / 60.0f, city, rng);
  SC_CHECK(d.stats().pieces == 0, "debris gone after 22 s (%u)", d.stats().pieces);
  // RESET: whole again
  d.reset(city);
  const f64 until = kern::now_ms() + 20000;
  while (std::fabs(roof(world) - 21) > 0.05f && kern::now_ms() < until) city.update({0, 0, 0}, 3);
  SC_CHECK(std::fabs(roof(world) - 21) < 0.05f, "standing again after RESET (%.2f)", roof(world));
  SC_CHECK(city.damaged_buildings() == 0, "no damage on record");
}
