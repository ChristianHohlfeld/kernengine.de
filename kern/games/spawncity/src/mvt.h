// Mapbox Vector Tile (MVT 2.1) decoder: the protobuf of one tile into layers of features with
// their tags and geometry in tile coordinates (0..extent, y down). No allocation per vertex beyond
// the output vectors, no dependencies. The player's browser fetches OpenFreeMap's tiles
// (OpenMapTiles schema, (c) OpenStreetMap contributors, ODbL) and hands the bytes to the game.
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "common.h"

namespace sc::mvt {

enum class GeomType : u8 { unknown = 0, point = 1, line = 2, polygon = 3 };

struct Value {
  enum class Kind : u8 { none, string, number, boolean } kind = Kind::none;
  std::string s;
  f64 n = 0;
  bool b = false;
};

struct Feature {
  u64 id = 0;
  GeomType type = GeomType::unknown;
  std::vector<std::pair<u32, u32>> tags;  // key index, value index into the layer
  /// Geometry as rings / lines / points: `parts[i]` holds xy pairs (tile units).
  std::vector<std::vector<Vec2>> parts;
};

struct Layer {
  std::string name;
  u32 extent = 4096;
  std::vector<std::string> keys;
  std::vector<Value> values;
  std::vector<Feature> features;
  /// The value of a tag on a feature (nullptr when absent).
  const Value* tag(const Feature& f, std::string_view key) const;
  std::string_view text(const Feature& f, std::string_view key) const;
  f64 number(const Feature& f, std::string_view key, f64 fallback) const;
};

struct Tile {
  std::vector<Layer> layers;
  const Layer* layer(std::string_view name) const;
};

/// Decodes `bytes` (uncompressed protobuf). Layers not in `wanted` (when non-empty) are skipped.
bool decode(const u8* bytes, std::size_t length, Tile& out, const std::vector<std::string>& wanted = {});

/// The same decoding in time slices (a dense city tile takes tens of milliseconds): the bytes
/// must stay alive until done() / failed().
class Decoder {
 public:
  void begin(const u8* bytes, std::size_t length, const std::vector<std::string>& wanted = {});
  /// Decodes for about `budget_ms` (0 = to the end). True when finished (check failed()).
  bool step(f64 budget_ms);
  bool done() const { return done_; }
  bool failed() const { return failed_; }
  Tile& tile() { return tile_; }

 private:
  struct Span { const u8* p; const u8* end; };
  std::vector<Span> layers_;
  std::vector<Span> features_;
  std::size_t layer_ = 0, feature_ = 0;
  bool layer_open_ = false, done_ = true, failed_ = false;
  Tile tile_;
};

/// Signed area of a ring in tile coordinates (y down): > 0 for MVT exterior rings.
f64 ring_area(const std::vector<Vec2>& ring);

// ---- encoding (tests and the offline sample) ----
/// Minimal MVT writer: enough to build tiles for tests.
class Writer {
 public:
  void begin_layer(const std::string& name, u32 extent = 4096);
  /// Adds a feature: geometry parts in tile units, tags as key/value strings or numbers.
  void feature(GeomType type, const std::vector<std::vector<Vec2>>& parts, const std::vector<std::pair<std::string, Value>>& tags, u64 id = 0);
  void end_layer();
  const std::vector<u8>& bytes() const { return out_; }

 private:
  struct PendingLayer {
    std::string name;
    u32 extent = 4096;
    std::vector<std::string> keys;
    std::vector<Value> values;
    std::vector<std::vector<u8>> features;
  };
  PendingLayer layer_;
  std::vector<u8> out_;
};

}  // namespace sc::mvt
