struct rigid_spec : virtual kern::Library, virtual engine_physics_physics {
  // ---- types ----
  KERN_STRUCT(MassSpec) { f32 mass = 1.0f; Vector center = Vector{0.0, 0.0, 0.0}; Fixed<f32, 6> inertia = List<f32>{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f}; };
  KERN_STRUCT(ColliderSpec) { physics_ShapeKind kind = physics_ShapeKind::box; Vector center = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0f, 0.0f, 0.0f, 1.0f}; Vector size = Vector{1.0, 1.0, 1.0}; f32 radius = 0.5f; f32 length = 1.0f; List<Vector> points = List<Vector>{}; f32 friction = 0.6f; f32 restitution = 0.0f; };
  KERN_STRUCT(RigidSpec) { List<ColliderSpec> colliders = List<ColliderSpec>{}; MassSpec mass = MassSpec{1.0f, Vector{0.0, 0.0, 0.0}, List<f32>{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f}}; f32 linear_damping = 0.0f; f32 angular_damping = 0.0f; bool bullet = false; };

  // ---- variables ----
  KERN_CONST f32 kPi = 3.1415927f;

  // ---- functions ----
  struct capsule_betweenResult { ColliderSpec Result = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f}; };
  /// A capsule from a to b (radius r) as a collider spec (capsules run along the shape's Z).
  KERN_PURE capsule_betweenResult capsule_between(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, f32 r = 0.0f) {
    ColliderSpec c = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
    Vector d = Vector{0.0, 0.0, 0.0};
    f32 len = 0.0f;
    f32 length = 0.0f;
    Vector z = Vector{0.0, 0.0, 0.0};
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector normalize = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Vector cross = Vector{0.0, 0.0, 0.0};
    f32 s = 0.0f;
    f32 length_2 = 0.0f;
    f32 co = 0.0f;
    f32 dot = 0.0f;
    Quat axis_angle = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    Quat axis_angle_2 = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    Quat axis_angle_3 = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    Quat axis_angle_4 = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    Quat chosen = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    Quat chosen_2 = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    c = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
    c.kind = physics_ShapeKind::capsule;
    c.center = vector_scale(vector_add(a, b), 0.5f);
    d = vector_sub(b, a);
    length = engine_length_2(d);
    len = length;
    c.length = len;
    c.radius = r;
    z = Vector{0.0, 0.0, 1.0};
    normalize = engine_normalize(d, Vector{0.0, 0.0, 1.0});
    n = normalize;
    cross = engine_cross_2(z, n);
    axis = cross;
    length_2 = engine_length_2(axis);
    s = length_2;
    dot = engine_dot_2(z, n);
    co = dot;
    if (math_compare<">", f32>(s, 1e-06f)) {
      axis_angle_2 = engine_quat_axis_angle(vector_scale(axis, kern::formula<f32>([](auto s) { return (1.0f / s); }, s)), kern::formula<f32, f32>([](auto s, auto co) { return atan2(s, co); }, s, co));
      chosen_2 = axis_angle_2;
    } else {
      if (math_compare<"<", f32>(co, 0.0f)) {
        axis_angle_4 = engine_quat_axis_angle(Vector{1.0, 0.0, 0.0}, kPi);
        chosen = axis_angle_4;
      } else {
        chosen = struct_make<Quat>(0.0f, 0.0f, 0.0f, 1.0f);
      }
      chosen_2 = chosen;
    }
    c.rotation = chosen_2;
    return {c};
  }

  struct create_bodyResult { physics_Body Result = physics_Body{0}; };
  /// Creates the body in a bare physics world (tests, tools).
  create_bodyResult create_body(const RigidSpec& spec, Object<kern::physics::World> w = Object<kern::physics::World>{}, Vector position = Vector{0.0, 0.0, 0.0}, Quat rotation = Quat{0.0f, 0.0f, 0.0f, 1.0f}, u64 category = 0, u64 mask = 0) {
    Object<kern::physics::World> w_ = Object<kern::physics::World>{};
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 1.0f, true, true, true, false, 0};
    physics_Body b = physics_Body{0};
    physics_Body create_body_2 = physics_Body{0};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, List<u32>{}, physics_ShapeMaterial{1.0f, 0.6f, 0.0f, 0.0f, false, 1, 18446744073709551615u, 0, true, true, false, 0, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{0};
    w_ = w;
    bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 1.0f, true, true, true, false, 0};
    bs.type = physics_BodyType::dynamic;
    bs.position = position;
    bs.rotation = rotation;
    bs.linear_damping = spec.linear_damping;
    bs.angular_damping = spec.angular_damping;
    bs.enable_sleep = false;
    bs.bullet = spec.bullet;
    engine_world_create_body(n10, w_, bs);
    create_body_2 = n10.result;
    b = create_body_2;
    for (auto it11 = flow_for(n11, 0, math_sub<i64>(list_length<ColliderSpec>(spec.colliders), 1)); it11.next();) {
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, List<u32>{}, physics_ShapeMaterial{1.0f, 0.6f, 0.0f, 0.0f, false, 1, 18446744073709551615u, 0, true, true, false, 0, physics_Surface::none}};
      ss.kind = spec.colliders[n11.index].kind;
      ss.center = spec.colliders[n11.index].center;
      ss.rotation = spec.colliders[n11.index].rotation;
      ss.size = spec.colliders[n11.index].size;
      ss.radius = spec.colliders[n11.index].radius;
      ss.length = spec.colliders[n11.index].length;
      ss.points = spec.colliders[n11.index].points;
      ss.material.friction = spec.colliders[n11.index].friction;
      ss.material.restitution = spec.colliders[n11.index].restitution;
      ss.material.density = 100.0f;
      ss.material.category_bits = category;
      ss.material.mask_bits = mask;
      engine_world_create_shape(n12, w_, b, ss);
      create_shape = n12.result;
    }
    engine_world_set_mass_data(w_, b, spec.mass.mass, spec.mass.center, spec.mass.inertia);
    return {b};
  }

  /// Solid box inertia (xx yy zz) of mass m and full extents s, into mass.inertia.
  void box_inertia(MassSpec& mass, f32 m = 0.0f, Vector s = Vector{0.0, 0.0, 0.0}) {
    mass.inertia[0] = kern::formula<f32, f32, f32>([](auto m, auto y, auto z) { return ((m * ((y * y) + (z * z))) / 12.0f); }, m, vector_break(s).y, vector_break(s).z);
    mass.inertia[1] = kern::formula<f32, f32, f32>([](auto m, auto x, auto z) { return ((m * ((x * x) + (z * z))) / 12.0f); }, m, vector_break(s).x, vector_break(s).z);
    mass.inertia[2] = kern::formula<f32, f32, f32>([](auto m, auto x, auto y) { return ((m * ((x * x) + (y * y))) / 12.0f); }, m, vector_break(s).x, vector_break(s).y);
    mass.inertia[3] = 0.0f;
    mass.inertia[4] = 0.0f;
    mass.inertia[5] = 0.0f;
  }

  // ---- what nodes give and keep ----
  struct { physics_Body result = physics_Body{0}; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  struct { physics_Shape result = physics_Shape{0}; } n12;
};
