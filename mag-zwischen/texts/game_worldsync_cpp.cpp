struct game_worldsync_cpp : virtual kern::Library {
  // ---- functions ----
  struct dist2dResult { Float Result = 0.0; };
  KERN_PURE dist2dResult dist2d(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct q16Result { Int Result = 0; };
  KERN_PURE q16Result q16(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float v) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(((v < (-1.0) ? (-1.0) : (1.0 < v ? 1.0 : v)) * 32767.0)), 65535.0), 32768.0) - 32768.0); }, v))};
  }

  struct uq16Result { Float Result = 0.0; };
  KERN_PURE uq16Result uq16(Int v = 0) {
    return {kern::formula([](Float v) -> Float { return kern::math::div(v, 32767.0); }, kern::to<Float>(v))};
  }

  struct cmsResult { Int Result = 0; };
  KERN_PURE cmsResult cms(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 65535.0), 32768.0) - 32768.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v) -> Float { return (v * 100.0); }, v), -32000.0, 32000.0)))};
  }

  struct col8Result { Int Result = 0; };
  KERN_PURE col8Result col8(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::band(kern::math::round(((v < 0.0 ? 0.0 : (1.0 < v ? 1.0 : v)) * 255.0)), 255.0); }, v))};
  }
};
