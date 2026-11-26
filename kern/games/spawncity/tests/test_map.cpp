// Map data: the MVT decoder reads what the writer wrote (round trip with zig-zag deltas, packed
// tags, every value type), OpenMapTiles layers become buildings / roads / areas in metres at the
// right place and size, tiles and the procedural stand-in city. The tile maths, the stand-in city and the
// clipping are the level's Blueprint tiles_cpp, called as its class - as the program calls it
// (level_blueprints.h), with the game's loop guard. (The round trip still runs the C++: the writer's
// end_layer waits - mvt_cpp Writer::end_layer, put_bytes.)
#include "../src/blueprint_call.h"
#include "../src/tiles.h"
#include "level_blueprints.h"
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

using namespace sc;

namespace {
using Tiles = blueprints::tiles_cpp;
/// tiles_cpp running as its class in a logic of its own.
struct TilesBlueprint {
  kern::test::BlueprintRun rules{"tiles_cpp", nullptr, true};
  bool bound = false;
  TilesBlueprint() {
    rules.rt.budget = rules::kLoopBudget;
    bound = rules.problems.empty() && kern::logic::bind_blueprint<Tiles>(rules.rt);
  }
  Tiles& operator*() { return kern::logic::blueprint<Tiles>(); }
};
/// Konstanz, where the game starts (tiles.h), and the zoom of its tiles.
constexpr f64 kLon = 9.1766, kLat = 47.6600;
constexpr i32 kZoom = 14;
Tiles::List<Tiles::Vector> line_of(std::initializer_list<Vec3> points) { return Tiles::List<Tiles::Vector>(std::vector<Vec3>(points)); }
}  // namespace

KERN_TEST(map_tile_math) {
  TilesBlueprint bp;
  KERN_CHECK(bp.bound, "tiles_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  Tiles& tl = *bp;
  const Tiles::TileKey t = tl.tile_at(kLon, kLat, kZoom).Result;
  KERN_CHECK(t.z == 14 && t.x == 8609 && t.y == 5718, "Konstanz tile %d/%d/%d", static_cast<int>(t.z), static_cast<int>(t.x), static_cast<int>(t.y));
  f64 lon = 0, lat = 0;
  tl.tile_lonlat(t, lon, lat, 0, 0);
  KERN_CHECK(lon <= kLon && lat >= kLat, "north-west corner %.5f %.5f", lon, lat);
  Tiles::GeoProjector p;
  tl.GeoProjector_init(p, kLon, kLat);
  f64 x = 0, y = 0, lo = 0, la = 0;
  tl.GeoProjector_project(p, x, y, 9.18, 47.665);
  tl.unproject(p, lo, la, x, y);
  KERN_CHECK(std::fabs(lo - 9.18) < 1e-9 && std::fabs(la - 47.665) < 1e-9, "unproject inverts project");
}

KERN_TEST(map_mvt_round_trip) {
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
  KERN_CHECK(mvt::decode(w.bytes(), tile), "decodes");
  KERN_CHECK(tile.layers.size() == 4, "four layers (%zu)", tile.layers.size());
  const i32 building = tile.layer("building");
  const mvt::Layer* b = building >= 0 ? &tile.layers[static_cast<std::size_t>(building)] : nullptr;
  KERN_CHECK(b && b->features.size() == 1 && b->features[0].id == 42 && b->features[0].parts.size() == 2, "building with courtyard");
  KERN_CHECK(b && std::fabs(b->number(b->features[0], "render_height", 0) - 15) < 1e-9, "height tag");
  KERN_CHECK(b && b->features[0].parts[0][2].x == 1100 && b->features[0].parts[0][2].y == 1100, "delta-coded vertices");

  // Garbage must not crash or loop.
  std::vector<u8> junk(w.bytes().begin(), w.bytes().begin() + w.bytes().size() / 2);
  mvt::Tile bad;
  mvt::decode(junk, bad);
  KERN_CHECK(true, "truncated input handled");

  kern::GeoProjector p;
  const TileKey key = tile_at(kKonstanzLon, kKonstanzLat);
  f64 lon, lat;
  tile_lonlat(key, 0.5, 0.5, lon, lat);
  p.init(lon, lat);
  MapFeatures f;
  extract(tile, key, p, f);
  KERN_CHECK(f.buildings.size() == 1 && f.buildings[0].rings.size() == 2, "one building, one courtyard");
  KERN_CHECK(f.buildings.size() == 1 && f.buildings[0].top == 15, "height 15 m");
  if (!f.buildings.empty()) {
    const auto& r = f.buildings[0].rings[0];
    const f32 side = std::fabs(r[1].x - r[0].x);
    const f32 unit = static_cast<f32>(40075016.686 * std::cos(lat * 3.14159265358979 / 180) / 16384 / 4096);
    KERN_CHECK(std::fabs(side - 100 * unit) < 0.05f, "100 tile units = %.2f m (expected %.2f)", side, 100 * unit);
    KERN_CHECK(r[0].y > r[2].y, "tile y down -> north up");
  }
  KERN_CHECK(f.roads.size() == 1 && f.roads[0].cls == RoadClass::primary && f.roads[0].bridge, "primary bridge kept, tunnel dropped");
  KERN_CHECK(f.roads.size() == 1 && std::fabs(f.roads[0].points[0].y) < 0.5f, "road through the tile centre (y %.2f)", f.roads.empty() ? 0.f : f.roads[0].points[0].y);
  KERN_CHECK(f.areas.size() == 1 && f.areas[0].kind == AreaKind::water, "water area");
}

KERN_TEST(map_procedural_city) {
  TilesBlueprint bp;
  KERN_CHECK(bp.bound, "tiles_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  Tiles& tl = *bp;
  Tiles::MapFeatures f;
  tl.procedural_city(f, 0, 0, 600, 7);
  KERN_CHECK(f.procedural, "marked as procedural");
  KERN_CHECK(f.buildings.size() > 150, "a town of buildings (%zu)", f.buildings.size());
  KERN_CHECK(f.roads.size() > 10, "streets (%zu)", f.roads.size());
  bool water = false;
  for (const auto& a : f.areas) water |= a.kind == Tiles::AreaKind::water;
  KERN_CHECK(water, "lake shore");
  for (const auto& b : f.buildings) {
    if (!(b.top >= 8 && b.top < 25)) { KERN_CHECK(false, "storey heights %.1f", static_cast<f64>(b.top)); break; }
  }
}


KERN_TEST(map_line_clipped_to_square) {
  TilesBlueprint bp;
  KERN_CHECK(bp.bound, "tiles_cpp runs as its class: %s", bp.rules.problems.c_str());
  if (!bp.bound) return;
  Tiles& tl = *bp;
  // a street as far as it lies in its tile's square: in order, a part per stay inside
  const Tiles::Vec4 sq{0, 0, 100, 100};
  auto parts = tl.clip_line(line_of({{-20, 50, 0}, {50, 50, 0}, {50, 130, 0}, {80, 130, 0}, {80, 90, 0}, {120, 90, 0}}), sq).Result;
  KERN_CHECK(parts.size() == 2, "two stays inside (%zu)", parts.size());
  if (parts.size() == 2) {
    const auto& a = parts[0].items;
    const auto& b = parts[1].items;
    KERN_CHECK(a.size() == 3 && a[0].x == 0 && a[0].y == 50 && a[2].x == 50 && a[2].y == 100, "in at the west edge, out at the north (%zu points)", a.size());
    KERN_CHECK(b.size() == 3 && b[0].y == 100 && b[2].x == 100 && b[2].y == 90, "back in from the north, out at the east (%zu points)", b.size());
  }
  KERN_CHECK(tl.clip_line(line_of({{-5, -5, 0}, {-1, 200, 0}}), sq).Result.empty(), "outside: nothing");
  parts = tl.clip_line(line_of({{10, 10, 0}, {20, 20, 0}, {30, 10, 0}}), sq).Result;
  KERN_CHECK(parts.size() == 1 && parts[0].items.size() == 3, "inside: the whole line");
  // two tiles side by side cut the same street at the same point of their common edge
  const auto left = tl.clip_line(line_of({{60, 40, 0}, {140, 60, 0}}), sq).Result, right = tl.clip_line(line_of({{60, 40, 0}, {140, 60, 0}}), Tiles::Vec4{100, 0, 200, 100}).Result;
  KERN_CHECK(left.size() == 1 && right.size() == 1 && std::fabs(left[0].items.back().x - right[0].items.front().x) < 1e-3f &&
               std::fabs(left[0].items.back().y - right[0].items.front().y) < 1e-3f,
           "the parts meet on the edge");
}
