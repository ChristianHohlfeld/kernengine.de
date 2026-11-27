struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Quat) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 1.0f; };
};
