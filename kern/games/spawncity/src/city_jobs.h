// The city's heavy work as Kern jobs (kern/core/jobs.h), off the frame thread like MapLibre's tile
// workers do it for the original: decoding a vector tile into features, extruding the drawn
// buildings into welded chunk meshes. Pure functions (bytes in, bytes out); the same code runs in
// a Web Worker, a native thread, or - without either - in the frame's slices.
#pragma once

#include <vector>

#include "kern/geo/buildings.h"
#include "tiles.h"

namespace sc::cityjobs {

constexpr u32 kTile = 1, kBuildings = 2;

/// kTile input: the tile, the projection origin and the uncompressed MVT bytes.
std::vector<u8> tile_input(const TileKey& key, f64 lon0, f64 lat0, kern::Span<const u8> mvt);
/// kTile output -> features (false: the tile could not be decoded).
bool read_features(const std::vector<u8>& output, MapFeatures& out);

/// kBuildings input: a footprint stream and the builder's options.
std::vector<u8> buildings_input(const kern::FootprintStream& footprints, const kern::BuildingOptions& options);
/// kBuildings output -> welded chunk meshes.
bool read_chunks(const std::vector<u8>& output, std::vector<kern::GeoChunk>& out);
/// The same, one chunk at a time (the frame uploads one per step): the count at offset 0, then
/// read_chunk advances `at` (start with at = 4). False at the end or on a broken buffer.
u32 chunk_count(const std::vector<u8>& output);
bool read_chunk(const std::vector<u8>& output, std::size_t& at, kern::GeoChunk& out);

}  // namespace sc::cityjobs
