struct engine_physics_physics : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(physics_Body) { Int64 id = Int64{0x0}; };
  KERN_STRUCT(physics_QueryFilter) { Int64 mask_bits = Int64{0xffffffffffffffff}; Int64 category_bits = Int64{0xffffffffffffffff}; };
  KERN_STRUCT(physics_Shape) { Int64 id = Int64{0x0}; };
  KERN_STRUCT(physics_Pose) { Vector position = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_STRUCT(physics_RayHit) { Bool hit = false; physics_Shape shape = physics_Shape{Int64{0x0}}; physics_Body body = physics_Body{Int64{0x0}}; Vector point = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 0.0}; Float fraction = 0.0; };

  // ---- functions ----
  struct Body_validResult { Bool Result = false; };
  KERN_PURE Body_validResult Body_valid(const physics_Body& self) {
    return {logic_not(int64_equal(self.id, Int64{0x0}))};
  }

  struct QueryFilter_newResult { physics_QueryFilter Result = physics_QueryFilter{Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}}; };
  KERN_PURE QueryFilter_newResult QueryFilter_new(Int64 mask = Int64{0x0}, Int64 category = Int64{0x0}) {
    physics_QueryFilter self = physics_QueryFilter{Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}};
    self.mask_bits = mask;
    self.category_bits = category;
    return {self};
  }
};
