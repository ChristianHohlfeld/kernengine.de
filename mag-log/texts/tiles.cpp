struct tiles : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(TileKey) { i32 z = 14; i32 x = 0; i32 y = 0; };

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
