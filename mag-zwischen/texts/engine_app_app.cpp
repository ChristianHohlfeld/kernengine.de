struct engine_app_app : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Frame) { f64 time = 0.0; f32 dt = 0.0f; u64 index = 0; };
};
