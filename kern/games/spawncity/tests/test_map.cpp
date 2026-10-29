// Map data: the MVT decoder reads what the writer wrote (round trip with zig-zag deltas, packed
// tags, every value type), OpenMapTiles layers become buildings / roads / areas in metres at the
// right place and size, tiles and the procedural stand-in city.
#include "../src/tiles.h"
#include "test_util.h"

using namespace sc;

SC_TEST(map_tile_math) {
  const TileKey t = tile_at(kKonstanzLon, kKonstanzLat);
  SC_CHECK(t.z == 14 && t.x == 8609 && t.y == 5718, "Konstanz tile %d/%d/%d", t.z, t.x, t.y);
  f64 lon, lat;
  tile_lonlat(t, 0, 0, lon, lat);
  SC_CHECK(lon <= kKonstanzLon && lat >= kKonstanzLat, "north-west corner %.5f %.5f", lon, lat);
  kern::GeoProjector p;
  p.init(kKonstanzLon, kKonstanzLat);
  f64 x, y, lo, la;
  p.project(9.18, 47.665, x, y);
  unproject(p, x, y, lo, la);
  SC_CHECK(std::fabs(lo - 9.18) < 1e-9 && std::fabs(la - 47.665) < 1e-9, "unproject inverts project");
}

SC_TEST(map_mvt_round_trip) {
  mvt::Writer w;
  w.begin_layer("building");
  mvt::Value h; h.kind = mvt::Value::Kind::number; h.n = 15;
  // exterior (positive area, y down) with a courtyard (negative area)
  w.feature(mvt::GeomType::polygon, {{{1000, 1000}, {1100, 1000}, {1100, 1100}, {1000, 1100}}, {{1020, 1020}, {1020, 1080}, {1080, 1080}, {1080, 1020}}},
            {{"render_height", h}}, 42);
  w.end_layer();
  w.begin_layer("transportation");
  mvt::Value cls; cls.kind = mvt::Value::Kind::string; cls.s = "primary";
  mvt::Value br; br.kind = mvt::Value::Kind::string; br.s = "bridge";
  w.feature(mvt::GeomType::line, {{{0, 2048}, {4096, 2048}}}, {{"class", cls}, {"brunnel", br}});
  mvt::Value tun; tun.kind = mvt::Value::Kind::string; tun.s = "tunnel";
  w.feature(mvt::GeomType::line, {{{0, 100}, {4096, 100}}}, {{"class", cls}, {"brunnel", tun}});
  w.end_layer();
  w.begin_layer("water");
  w.feature(mvt::GeomType::polygon, {{{0, 0}, {4096, 0}, {4096, 500}, {0, 500}}}, {});
  w.end_layer();
  w.begin_layer("poi");  // ignored by the game
  w.feature(mvt::GeomType::point, {{{5, 5}}}, {});
  w.end_layer();

  mvt::Tile tile;
  SC_CHECK(mvt::decode(w.bytes().data(), w.bytes().size(), tile), "decodes");
  SC_CHECK(tile.layers.size() == 4, "four layers (%zu)", tile.layers.size());
  const mvt::Layer* b = tile.layer("building");
  SC_CHECK(b && b->features.size() == 1 && b->features[0].id == 42 && b->features[0].parts.size() == 2, "building with courtyard");
  SC_CHECK(b && std::fabs(b->number(b->features[0], "render_height", 0) - 15) < 1e-9, "height tag");
  SC_CHECK(b && b->features[0].parts[0][2].x == 1100 && b->features[0].parts[0][2].y == 1100, "delta-coded vertices");

  // Garbage must not crash or loop.
  std::vector<u8> junk(w.bytes().begin(), w.bytes().begin() + w.bytes().size() / 2);
  mvt::Tile bad;
  mvt::decode(junk.data(), junk.size(), bad);
  SC_CHECK(true, "truncated input handled");

  kern::GeoProjector p;
  const TileKey key = tile_at(kKonstanzLon, kKonstanzLat);
  f64 lon, lat;
  tile_lonlat(key, 0.5, 0.5, lon, lat);
  p.init(lon, lat);
  MapFeatures f;
  extract(tile, key, p, f);
  SC_CHECK(f.buildings.size() == 1 && f.buildings[0].rings.size() == 2, "one building, one courtyard");
  SC_CHECK(f.buildings.size() == 1 && f.buildings[0].top == 15, "height 15 m");
  if (!f.buildings.empty()) {
    const auto& r = f.buildings[0].rings[0];
    const f32 side = std::fabs(r[1].x - r[0].x);
    const f32 unit = static_cast<f32>(40075016.686 * std::cos(lat * 3.14159265358979 / 180) / 16384 / 4096);
    SC_CHECK(std::fabs(side - 100 * unit) < 0.05f, "100 tile units = %.2f m (expected %.2f)", side, 100 * unit);
    SC_CHECK(r[0].y > r[2].y, "tile y down -> north up");
  }
  SC_CHECK(f.roads.size() == 1 && f.roads[0].cls == RoadClass::primary && f.roads[0].bridge, "primary bridge kept, tunnel dropped");
  SC_CHECK(f.roads.size() == 1 && std::fabs(f.roads[0].points[0].y) < 0.5f, "road through the tile centre (y %.2f)", f.roads.empty() ? 0.f : f.roads[0].points[0].y);
  SC_CHECK(f.areas.size() == 1 && f.areas[0].kind == AreaKind::water, "water area");
}

SC_TEST(map_procedural_city) {
  MapFeatures f;
  procedural_city(0, 0, 600, 7, f);
  SC_CHECK(f.procedural, "marked as procedural");
  SC_CHECK(f.buildings.size() > 150, "a town of buildings (%zu)", f.buildings.size());
  SC_CHECK(f.roads.size() > 10, "streets (%zu)", f.roads.size());
  bool water = false;
  for (const auto& a : f.areas) water |= a.kind == AreaKind::water;
  SC_CHECK(water, "lake shore");
  for (const auto& b : f.buildings) {
    if (!(b.top >= 8 && b.top < 25)) { SC_CHECK(false, "storey heights %.1f", b.top); break; }
  }
}
