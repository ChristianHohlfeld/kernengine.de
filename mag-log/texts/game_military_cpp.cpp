struct game_military_cpp : virtual kern::Library, virtual common {
  // ---- functions ----
  struct dist2dResult { Float Result = 0.0; };
  KERN_PURE dist2dResult dist2d(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct approach_angleResult { Float Result = 0.0; };
  KERN_PURE approach_angleResult approach_angle(Float cur = 0.0, Float target = 0.0, Float max_step = 0.0) {
    return {kern::formula([](Float cur, Float the_result, Float max_step) -> Float { return (cur + (the_result < (-max_step) ? (-max_step) : (max_step < the_result ? max_step : the_result))); }, cur, common::wrap_pi(kern::formula([](Float target, Float cur) -> Float { return (target - cur); }, target, cur)).Result, max_step)};
  }
};
