struct engine_physics_physics : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(physics_Body) { u64 id = 0; };
  KERN_STRUCT(physics_QueryFilter) { u64 mask_bits = 18446744073709551615u; u64 category_bits = 18446744073709551615u; };
  KERN_STRUCT(physics_Shape) { u64 id = 0; };
  KERN_STRUCT(physics_Pose) { Vector position = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_STRUCT(physics_RayHit) { bool hit = false; physics_Shape shape = physics_Shape{0}; physics_Body body = physics_Body{0}; Vector point = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 0.0}; f32 fraction = 0.0f; };

  // ---- functions ----
  struct QueryFilter_newResult { physics_QueryFilter Result = physics_QueryFilter{18446744073709551615u, 18446744073709551615u}; };
  KERN_PURE QueryFilter_newResult QueryFilter_new(u64 mask = 0, u64 category = 0) {
    physics_QueryFilter self = physics_QueryFilter{18446744073709551615u, 18446744073709551615u};
    self.mask_bits = mask;
    self.category_bits = category;
    return {self};
  }
};
