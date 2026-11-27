struct engine_physics_physics : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(physics_Body) { u64 id = 0; };
  KERN_ENUM(physics_BodyType, fixed, kinematic, dynamic);
  KERN_STRUCT(physics_QueryFilter) { u64 mask_bits = 18446744073709551615u; u64 category_bits = 18446744073709551615u; };
  KERN_STRUCT(physics_Shape) { u64 id = 0; };
  KERN_ENUM(physics_ShapeKind, box, sphere, capsule, hull, mesh);
  KERN_ENUM(physics_Surface, none, cloth, tpu, floor_);
  KERN_STRUCT(physics_BodySettings) { physics_BodyType type = physics_BodyType::dynamic; Vector position = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0f, 0.0f, 0.0f, 1.0f}; Vector linear_velocity = Vector{0.0, 0.0, 0.0}; Vector angular_velocity = Vector{0.0, 0.0, 0.0}; f32 linear_damping = 0.0f; f32 angular_damping = 0.0f; f32 gravity_scale = 1.0f; bool enable_sleep = true; bool awake = true; bool enabled = true; bool bullet = false; u64 user = 0; };
  KERN_STRUCT(physics_Pose) { Vector position = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_STRUCT(physics_RayHit) { bool hit = false; physics_Shape shape = physics_Shape{0}; physics_Body body = physics_Body{0}; Vector point = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 0.0}; f32 fraction = 0.0f; };
  KERN_STRUCT(physics_ShapeMaterial) { f32 density = 1.0f; f32 friction = 0.6f; f32 restitution = 0.0f; f32 rolling_resistance = 0.0f; bool sensor = false; u64 category_bits = 1; u64 mask_bits = 18446744073709551615u; i32 group_index = 0; bool contact_events = true; bool sensor_events = true; bool hit_events = false; u64 user = 0; physics_Surface surface = physics_Surface::none; };
  KERN_STRUCT(physics_ShapeSettings) { physics_ShapeKind kind = physics_ShapeKind::box; Vector center = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0f, 0.0f, 0.0f, 1.0f}; Vector size = Vector{1.0, 1.0, 1.0}; f32 radius = 0.5f; f32 length = 1.0f; List<Vector> points = List<Vector>{}; List<u32> indices = List<u32>{}; physics_ShapeMaterial material = physics_ShapeMaterial{1.0f, 0.6f, 0.0f, 0.0f, false, 1, 18446744073709551615u, 0, true, true, false, 0, physics_Surface::none}; };

  // ---- functions ----
  struct QueryFilter_newResult { physics_QueryFilter Result = physics_QueryFilter{18446744073709551615u, 18446744073709551615u}; };
  KERN_PURE QueryFilter_newResult QueryFilter_new(u64 mask = 0, u64 category = 0) {
    physics_QueryFilter self = physics_QueryFilter{18446744073709551615u, 18446744073709551615u};
    self.mask_bits = mask;
    self.category_bits = category;
    return {self};
  }
};
