struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Vector_f32_pair) { Vector first = Vector{0.0, 0.0, 0.0}; f32 second = 0.0f; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
  KERN_STRUCT(u32_u32_pair) { u32 first = 0; u32 second = 0; };
  KERN_STRUCT(u64_u32_pair) { u64 first = 0; u32 second = 0; };
  KERN_STRUCT(u8_row) { List<u8> items = List<u8>{}; };
  KERN_STRUCT(Vector_row_row) { List<Vector_row> items = List<Vector_row>{}; };
};
