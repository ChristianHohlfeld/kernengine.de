struct engine_physics_physics : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(physics_Body) { Int64 id = Int64{0x0}; };
  KERN_ENUM(physics_BodyType, fixed, kinematic, dynamic);
  KERN_STRUCT(physics_Joint) { Int64 id = Int64{0x0}; };
  KERN_ENUM(physics_JointType, revolute, distance, prismatic, wheel, weld, motor, spherical, parallel, filter);
  KERN_STRUCT(physics_QueryFilter) { Int64 mask_bits = Int64{0xffffffffffffffff}; Int64 category_bits = Int64{0xffffffffffffffff}; };
  KERN_STRUCT(physics_Shape) { Int64 id = Int64{0x0}; };
  KERN_ENUM(physics_ShapeKind, box, sphere, capsule, hull, mesh);
  KERN_ENUM(physics_Surface, none, cloth, tpu, floor_);
  KERN_STRUCT(physics_BodySettings) { physics_BodyType type = physics_BodyType::dynamic; Vector position = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; Vector linear_velocity = Vector{0.0, 0.0, 0.0}; Vector angular_velocity = Vector{0.0, 0.0, 0.0}; Float linear_damping = 0.0; Float angular_damping = 0.0; Float gravity_scale = 1.0; Bool enable_sleep = true; Bool awake = true; Bool enabled = true; Bool bullet = false; Int64 user = Int64{0x0}; };
  KERN_STRUCT(physics_Pose) { Vector position = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_STRUCT(physics_RayHit) { Bool hit = false; physics_Shape shape = physics_Shape{Int64{0x0}}; physics_Body body = physics_Body{Int64{0x0}}; Vector point = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 0.0}; Float fraction = 0.0; };
  KERN_STRUCT(physics_ShapeMaterial) { Float density = 1.0; Float friction = 0.6; Float restitution = 0.0; Float rolling_resistance = 0.0; Bool sensor = false; Int64 category_bits = Int64{0x1}; Int64 mask_bits = Int64{0xffffffffffffffff}; Int group_index = 0; Bool contact_events = true; Bool sensor_events = true; Bool hit_events = false; Int64 user = Int64{0x0}; physics_Surface surface = physics_Surface::none; };
  KERN_STRUCT(physics_JointSettings) { physics_JointType type = physics_JointType::revolute; physics_Body body_a = physics_Body{Int64{0x0}}; physics_Body body_b = physics_Body{Int64{0x0}}; physics_Pose frame_a = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}; physics_Pose frame_b = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}; Bool collide_connected = false; Bool enable_spring = false; Float hertz = 0.0; Float damping_ratio = 0.0; Bool enable_limit = false; Float lower = 0.0; Float upper = 0.0; Bool enable_motor = false; Float motor_speed = 0.0; Float max_motor_torque = 0.0; Float max_motor_force = 0.0; Vector motor_velocity = Vector{0.0, 0.0, 0.0}; Float length = 1.0; Float linear_hertz = 0.0; Float angular_hertz = 0.0; Float linear_damping_ratio = 1.0; Float angular_damping_ratio = 1.0; Vector linear_velocity = Vector{0.0, 0.0, 0.0}; Vector angular_velocity = Vector{0.0, 0.0, 0.0}; Float max_velocity_force = 0.0; Float max_velocity_torque = 0.0; Float max_spring_force = 0.0; Float max_spring_torque = 0.0; Bool enable_steering = false; Bool enable_steering_limit = false; Float steering_hertz = 0.0; Float steering_damping_ratio = 0.0; Float target_steering = 0.0; Float max_steering_torque = 0.0; Float steering_lower = 0.0; Float steering_upper = 0.0; Float max_torque = 0.0; Float constraint_hertz = 0.0; Float constraint_damping_ratio = 0.0; Int64 user = Int64{0x0}; };
  KERN_STRUCT(physics_ShapeSettings) { physics_ShapeKind kind = physics_ShapeKind::box; Vector center = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; Vector size = Vector{1.0, 1.0, 1.0}; Float radius = 0.5; Float length = 1.0; List<Vector> points = List<Vector>{}; List<Int> indices = List<Int>{}; physics_ShapeMaterial material = physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}; };

  // ---- functions ----
  struct Body_validResult { Bool Result = false; };
  KERN_PURE Body_validResult Body_valid(const physics_Body& self) {
    return {logic_not(int64_equal(self.id, Int64{0x0}))};
  }

  struct Body_equalResult { Bool Result = false; };
  KERN_PURE Body_equalResult Body_equal(const physics_Body& self, const physics_Body& o) {
    return {int64_equal(self.id, o.id)};
  }

  struct QueryFilter_newResult { physics_QueryFilter Result = physics_QueryFilter{Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}}; };
  KERN_PURE QueryFilter_newResult QueryFilter_new(Int64 mask = Int64{0x0}, Int64 category = Int64{0x0}) {
    physics_QueryFilter self = physics_QueryFilter{Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}};
    self.mask_bits = mask;
    self.category_bits = category;
    return {self};
  }
};
