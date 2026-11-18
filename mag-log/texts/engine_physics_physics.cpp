struct engine_physics_physics : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(physics_Body) { Int64 id = Int64{0x0}; };
  KERN_STRUCT(physics_Shape) { Int64 id = Int64{0x0}; };
};
