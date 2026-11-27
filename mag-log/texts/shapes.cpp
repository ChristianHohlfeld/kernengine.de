struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Vector_f32_pair) { Vector first = Vector{0.0, 0.0, 0.0}; f32 second = 0.0f; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
  KERN_STRUCT(u32_u32_pair) { u32 first = 0; u32 second = 0; };
};
