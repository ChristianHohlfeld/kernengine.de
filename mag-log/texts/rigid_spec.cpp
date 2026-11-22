struct rigid_spec : virtual kern::Library, virtual engine_math_math, virtual engine_physics_physics {
  // ---- types ----
  KERN_STRUCT(MassSpec) { Float mass = 1.0; Vector center = Vector{0.0, 0.0, 0.0}; Fixed<Float, 6> inertia = List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}; };
  KERN_STRUCT(ColliderSpec) { physics_ShapeKind kind = physics_ShapeKind::box; Vector center = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; Vector size = Vector{1.0, 1.0, 1.0}; Float radius = 0.5; Float length = 1.0; List<Vector> points = List<Vector>{}; Float friction = 0.6; Float restitution = 0.0; };
  KERN_STRUCT(RigidSpec) { List<ColliderSpec> colliders = List<ColliderSpec>{}; MassSpec mass = MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}; Float linear_damping = 0.0; Float angular_damping = 0.0; Bool bullet = false; };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct capsule_betweenResult { ColliderSpec Result = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0}; };
  /// A capsule from a to b (radius r) as a collider spec (capsules run along the shape's Z).
  KERN_PURE capsule_betweenResult capsule_between(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    ColliderSpec c = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector z = Vector{0.0, 0.0, 0.0};
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Float s = 0.0;
    Float co = 0.0;
    c = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    c.kind = physics_ShapeKind::capsule;
    c.center = vector_scale(vector_add(a, b), 0.5);
    d = vector_sub(b, a);
    len = vector_length(d);
    c.length = len;
    c.radius = r;
    z = Vector{0.0, 0.0, 1.0};
    n = logic_select<Vector>(math_compare<">">(vector_length(d), 9.999999717180685e-10), vector_normalize(d), Vector{0.0, 0.0, 1.0});
    axis = vector_cross(z, n);
    s = vector_length(axis);
    co = vector_dot(z, n);
    c.rotation = logic_select<Quat>(math_compare<">">(s, 1e-06), engine_math_math::Quat_axis_angle(vector_scale(axis, kern::formula([](Float s) -> Float { return kern::math::div(1.0, s); }, s)), kern::formula([](Float s, Float co) -> Float { return kern::math::atan2(s, co); }, s, co)).Result, logic_select<Quat>(math_compare<"<">(co, 0.0), engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, kPi).Result, struct_make<Quat>(0.0, 0.0, 0.0, 1.0)));
    return {c};
  }

  struct create_bodyResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  /// Creates the body in a bare physics world (tests, tools).
  create_bodyResult create_body(const RigidSpec& spec, Int w = 0, Vector position = Vector{0.0, 0.0, 0.0}, Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}, Int64 category = Int64{0x0}, Int64 mask = Int64{0x0}) {
    Int w_ = 0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body b = physics_Body{Int64{0x0}};
    physics_Body create_body_2 = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    w_ = w;
    bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
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
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<ColliderSpec>(spec.colliders))))); it11.next();) {
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss.kind = spec.colliders[n11.index].kind;
      ss.center = spec.colliders[n11.index].center;
      ss.rotation = spec.colliders[n11.index].rotation;
      ss.size = spec.colliders[n11.index].size;
      ss.radius = spec.colliders[n11.index].radius;
      ss.length = spec.colliders[n11.index].length;
      ss.points = spec.colliders[n11.index].points;
      ss.material.friction = spec.colliders[n11.index].friction;
      ss.material.restitution = spec.colliders[n11.index].restitution;
      ss.material.density = 100.0;
      ss.material.category_bits = category;
      ss.material.mask_bits = mask;
      engine_world_create_shape(n12, w_, b, ss);
      create_shape = n12.result;
    }
    engine_world_set_mass_data(w_, b, spec.mass.mass, spec.mass.center, spec.mass.inertia);
    return {b};
  }

  /// Solid box inertia (xx yy zz) of mass m and full extents s, into mass.inertia.
  void box_inertia(MassSpec& mass, Float m = 0.0, Vector s = Vector{0.0, 0.0, 0.0}) {
    mass.inertia[0] = kern::formula([](Float m, Float y, Float z) -> Float { return kern::math::div((m * ((y * y) + (z * z))), 12.0); }, m, vector_break(s).y, vector_break(s).z);
    mass.inertia[1] = kern::formula([](Float m, Float x, Float z) -> Float { return kern::math::div((m * ((x * x) + (z * z))), 12.0); }, m, vector_break(s).x, vector_break(s).z);
    mass.inertia[2] = kern::formula([](Float m, Float x, Float y) -> Float { return kern::math::div((m * ((x * x) + (y * y))), 12.0); }, m, vector_break(s).x, vector_break(s).y);
    mass.inertia[3] = 0.0;
    mass.inertia[4] = 0.0;
    mass.inertia[5] = 0.0;
  }

  // ---- what nodes give and keep ----
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n12;
};
