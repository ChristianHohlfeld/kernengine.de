struct game_projsync_cpp : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Game_projsync_update_Rec) { U16 id = 0; U8 kind = 0; U8 flags = 0; Vector p = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; };

  // ---- functions ----
  struct dmsResult { I16 Result = 0; };
  KERN_PURE dmsResult dms(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 65535.0), 32768.0) - 32768.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v) -> Float { return (v * 10.0); }, v), -32000.0, 32000.0)))};
  }
};
