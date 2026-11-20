struct tiles : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(RoadClass, motorway, trunk, primary, secondary, tertiary, minor, service, track, path, pedestrian, river, canal, stream);
  KERN_STRUCT(TileKey) { Int z = 14; Int x = 0; Int y = 0; };
  KERN_STRUCT(MapFeatures_Road) { List<Vector> points = List<Vector>{}; RoadClass cls = RoadClass::minor; Bool bridge = false; };

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
