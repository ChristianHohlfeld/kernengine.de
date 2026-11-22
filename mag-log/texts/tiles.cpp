struct tiles : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_ENUM(AreaKind, water, park, wood, grass, sand, pitch);
  KERN_STRUCT(MapFeatures_Spot) { Vector p = Vector{0.0, 0.0, 0.0}; Float a = 0.0; };
  KERN_ENUM(RoadClass, motorway, trunk, primary, secondary, tertiary, minor, service, track, path, pedestrian, river, canal, stream);
  KERN_STRUCT(TileKey) { I32 z = 14; I32 x = 0; I32 y = 0; };
  KERN_STRUCT(MapFeatures_Area) { List<Vector_row> rings = List<Vector_row>{}; AreaKind kind = AreaKind::park; };
  KERN_STRUCT(MapFeatures_Building) { List<Vector_row> rings = List<Vector_row>{}; Float base = 0.0; Float top = 8.0; U32 seed = 0; Float full_top = 0.0; Bool leveled = false; };
  KERN_STRUCT(MapFeatures_Road) { List<Vector> points = List<Vector>{}; RoadClass cls = RoadClass::minor; Bool bridge = false; };
  KERN_STRUCT(MapFeatures) { List<MapFeatures_Building> buildings = List<MapFeatures_Building>{}; List<MapFeatures_Road> roads = List<MapFeatures_Road>{}; List<MapFeatures_Area> areas = List<MapFeatures_Area>{}; List<MapFeatures_Spot> trees = List<MapFeatures_Spot>{}; List<MapFeatures_Spot> lamps = List<MapFeatures_Spot>{}; Bool procedural = false; };

  // ---- functions ----
  struct TileKey_equalResult { Bool Result = false; };
  KERN_PURE TileKey_equalResult TileKey_equal(const TileKey& self, const TileKey& o) {
    return {logic_and(logic_and(math_compare<"==">(kern::to<Float>(self.z), kern::to<Float>(o.z)), math_compare<"==">(kern::to<Float>(self.x), kern::to<Float>(o.x))), math_compare<"==">(kern::to<Float>(self.y), kern::to<Float>(o.y)))};
  }

  struct TileKey_hashResult { Int64 Result = Int64{0x0}; };
  KERN_PURE TileKey_hashResult TileKey_hash(const TileKey& self) {
    return {int64_xor(int64_xor(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(self.x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(self.y))))), int64_shift_left(int64_of_int(self.z), 58))};
  }
};
