struct game_town_cpp : virtual kern::Library, virtual common {
  // ---- types ----
  KERN_STRUCT(Game_minimap_json_Feat) { Int kind = 0; Bool poly = false; Int64 tile = Int64{0x0}; U32 index = 0; Float d = 0.0; };

  // ---- functions ----
  struct map_layerResult { I32 Result = 0; };
  /// The minimap's draw order of a feature's kind, as the original's map layers stand (water, green, roads, buildings).
  KERN_PURE map_layerResult map_layer(Int kind = 0) {
    return {kern::to<Int>(kern::formula([](Float kind) -> Float { return ((kind == 119.0) ? 0.0 : ((kind == 103.0) ? 1.0 : ((kind == 114.0) ? 2.0 : 3.0))); }, kern::to<Float>(kind)))};
  }

  struct dist2dResult { Float Result = 0.0; };
  KERN_PURE dist2dResult dist2d(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct approach_angleResult { Float Result = 0.0; };
  KERN_PURE approach_angleResult approach_angle(Float cur = 0.0, Float target = 0.0, Float max_step = 0.0) {
    return {kern::formula([](Float cur, Float the_result, Float max_step) -> Float { return (cur + (the_result < (-max_step) ? (-max_step) : (max_step < the_result ? max_step : the_result))); }, cur, common::wrap_pi(kern::formula([](Float target, Float cur) -> Float { return (target - cur); }, target, cur)).Result, max_step)};
  }
};
