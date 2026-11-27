struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
  KERN_STRUCT(u32_u32_pair) { u32 first = 0; u32 second = 0; };
  KERN_STRUCT(u64_u32_pair) { u64 first = 0; u32 second = 0; };
};
