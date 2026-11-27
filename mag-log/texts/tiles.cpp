struct tiles : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_ENUM(AreaKind, water, park, wood, grass, sand, pitch);
  KERN_STRUCT(MapFeatures_Spot) { Vector p = Vector{0.0, 0.0, 0.0}; f32 a = 0.0f; };
  KERN_ENUM(RoadClass, motorway, trunk, primary, secondary, tertiary, minor, service, track, path, pedestrian, river, canal, stream);
  KERN_STRUCT(TileKey) { i32 z = 14; i32 x = 0; i32 y = 0; };
  KERN_STRUCT(MapFeatures_Area) { List<Vector_row> rings = List<Vector_row>{}; AreaKind kind = AreaKind::park; };
  KERN_STRUCT(MapFeatures_Building) { List<Vector_row> rings = List<Vector_row>{}; f32 base = 0.0f; f32 top = 8.0f; u32 seed = 0; f32 full_top = 0.0f; bool leveled = false; };
  KERN_STRUCT(MapFeatures_Road) { List<Vector> points = List<Vector>{}; RoadClass cls = RoadClass::minor; bool bridge = false; };
  KERN_STRUCT(MapFeatures) { List<MapFeatures_Building> buildings = List<MapFeatures_Building>{}; List<MapFeatures_Road> roads = List<MapFeatures_Road>{}; List<MapFeatures_Area> areas = List<MapFeatures_Area>{}; List<MapFeatures_Spot> trees = List<MapFeatures_Spot>{}; List<MapFeatures_Spot> lamps = List<MapFeatures_Spot>{}; bool procedural = false; };

  // ---- variables ----
  /// OpenFreeMap's planet tiles go up to z14 (overzoomed beyond)
  KERN_CONST i32 kTileZoom = 14;
  /// Default start: Konstanz, Marktstaette (old town by the harbour).
  KERN_CONST f64 kKonstanzLon = 9.1766;
  /// Default start: Konstanz, Marktstaette (old town by the harbour).
  KERN_CONST f64 kKonstanzLat = 47.66;

  // ---- functions ----
  struct TileKey_equalResult { bool Result = false; };
  KERN_PURE TileKey_equalResult TileKey_equal(const TileKey& self, const TileKey& o) {
    return {logic_and(logic_and(math_compare<"==", i32>(self.z, o.z), math_compare<"==", i32>(self.x, o.x)), math_compare<"==", i32>(self.y, o.y))};
  }

  struct TileKey_hashResult { u64 Result = 0; };
  KERN_PURE TileKey_hashResult TileKey_hash(const TileKey& self) {
    return {kern::formula<i32, i32, i32>([](auto x, auto y, auto z) { return (((num_cast<u64, u32>(num_cast<u32, i32>(x)) << 32) ^ num_cast<u64, u32>(num_cast<u32, i32>(y))) ^ (num_cast<u64, i32>(z) << 58)); }, self.x, self.y, self.z)};
  }
};
