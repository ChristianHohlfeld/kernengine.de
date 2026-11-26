struct car_cpp : virtual kern::Library, virtual car, virtual common, virtual engine_math_math, virtual engine_physics_physics, virtual rigid_spec {
  // ---- types ----
  KERN_STRUCT(Car_step_Front) { Float pf = 0.0; Float pl = 0.0; Float N = 0.0; Float muN = 0.0; Float kappa = 0.0; };
  KERN_STRUCT(Tuning) { Float hertz = 0.0; Float damping_ratio = 0.0; };

  // ---- variables ----
  KERN_CONST Float kKappaPeak = 0.1;
  KERN_CONST Float kAlphaPeak = 0.1139356083016455;
  KERN_CONST Float kSlideGrip = 0.78;
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Float kRubberFriction = 0.9;
  /// Joint frame: local x -> up (suspension + steering axis), local z -> left (wheel spin axis), local y -> forward. The same frame on chassis and wheel.
  KERN_CONST Quat kWheelFrame = Quat{-0.5, -0.5, -0.5, 0.5};
  KERN_CONST Float kSpringZeta = 0.06;
  KERN_CONST Float kStrutShare = 0.15;
  KERN_CONST Float kStrutM = 0.55;
  KERN_CONST Float kBodyDamping = 0.45;
  KERN_CONST Float kVReg = 2.0;
  KERN_CONST Float kCasterTrail = 0.025;
  KERN_CONST Float kPneumaticTrail = 0.03;
  KERN_CONST Float kTrailCollapse = 3.0;
  KERN_CONST Float kSteerJacking = 0.18;

  // ---- functions ----
  void Car_detach(Car& self) {
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    if (logic_not(object_valid<kern::physics::World>(self.rig_world_))) {
      return;
    }
    w = self.rig_world_;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Car_Rig>(self.rig_))))); it10.next();) {
      both = engine_physics_physics::Body_valid(self.rig_[n10.index].body).Result;
      if (both) {
        body_valid_2 = engine_world_body_valid(w, self.rig_[n10.index].body);
        both = body_valid_2;
      }
      if (both) {
        engine_world_destroy_body(w, self.rig_[n10.index].body);
      }
      self.rig_[n10.index] = struct_make<Car_Rig>(struct_make<physics_Body>(Int64{0x0}), struct_make<physics_Joint>(Int64{0x0}), struct_make<physics_Joint>(Int64{0x0}));
    }
    self.rig_world_ = Object<kern::physics::World>{};
    self.rig_chassis_ = struct_make<physics_Body>(Int64{0x0});
    self.has_last_ = false;
  }

  void Car_end(Car& self) {
    Car_detach(self);
  }

  /// Combined-slip tyre force (longitudinal, lateral) on the friction ellipse.
  void tyre(Fixed<Float, 2>& out, Float kappa = 0.0, Float tan_a = 0.0, Float muN = 0.0) {
    Float sx = 0.0;
    Float sy = 0.0;
    Float s = 0.0;
    Float g = 0.0;
    Float F = 0.0;
    sx = kern::formula([](Float kappa, Float kKappaPeak) -> Float { return kern::math::div(kappa, kKappaPeak); }, kappa, kKappaPeak);
    sy = kern::formula([](Float tan_a, Float kAlphaPeak) -> Float { return kern::math::div(tan_a, kAlphaPeak); }, tan_a, kAlphaPeak);
    s = kern::formula([](Float sx, Float sy) -> Float { return kern::math::sqrt((sx * sx) + (sy * sy)); }, sx, sy);
    if (logic_or(math_compare<"<">(s, 1e-06), logic_not(math_compare<">">(muN, 0.0)))) {
      out[0] = 0.0;
      out[1] = 0.0;
      return;
    }
    g = kern::formula([](Float s, Float kSlideGrip) -> Float { return ((s < 1.0) ? (s * (2.0 - s)) : (1.0 - ((1.0 - kSlideGrip) * kern::math::min(1.0, kern::math::div((s - 1.0), 2.5))))); }, s, kSlideGrip);
    F = kern::formula([](Float muN, Float g) -> Float { return (muN * g); }, muN, g);
    out[0] = kern::formula([](Float f, Float sx, Float s) -> Float { return kern::math::div((f * sx), s); }, F, sx, s);
    out[1] = kern::formula([](Float f, Float sy, Float s) -> Float { return kern::math::div(((-f) * sy), s); }, F, sy, s);
  }

  struct steer_torque_atResult { Float Result = 0.0; };
  KERN_PURE steer_torque_atResult steer_torque_at(const CarSpec& s, Float v = 0.0) {
    Float x = 0.0;
    x = kern::formula([](Float v, Float steer_assist_speed) -> Float { return kern::math::div(v, steer_assist_speed); }, v, s.steer_assist_speed);
    return {kern::formula([](Float steer_hand, Float steer_assist, Float x) -> Float { return (steer_hand + kern::math::div(steer_assist, (1.0 + (x * x)))); }, s.steer_hand, s.steer_assist, x)};
  }

  struct CarSpec_busResult { CarSpec Result = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54}; };
  KERN_PURE CarSpec_busResult CarSpec_bus() {
    CarSpec s = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54};
    s = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54};
    s.kind = "bus";
    s.half = Vector{4.0, 1.17, 1.2};
    s.mass = 9200.0;
    s.com_z = -0.52;
    s.com_x = 0.0;
    s.radius = 0.46;
    s.wheel_width = 0.3;
    s.wheel_mass = 85.0;
    s.attach_z = -1.1;
    s.wheels[0] = struct_make<CarSpec_Wheel>(2.6, 1.08, true);
    s.wheels[1] = struct_make<CarSpec_Wheel>(2.6, -1.08, true);
    s.wheels[2] = struct_make<CarSpec_Wheel>(-2.55, 1.08, false);
    s.wheels[3] = struct_make<CarSpec_Wheel>(-2.55, -1.08, false);
    s.susp_lower = -0.18;
    s.susp_upper = 0.14;
    s.sag = 0.08;
    s.torque_front = 0.0;
    s.torque_rear = 4200.0;
    s.power = 230000.0;
    s.steer_lag = 0.06;
    s.steer_damp_speed = 20.0;
    s.drag_area = 6.2;
    s.rolling_resistance = 0.008;
    s.wheel_inertia = 14.0;
    s.brake = 9500.0;
    s.handbrake = 12000.0;
    s.coast = 200.0;
    s.steer_lock = 0.44;
    s.steer_hand = 2600.0;
    s.steer_assist = 5200.0;
    s.steer_assist_speed = 6.0;
    s.friction = 1.0;
    s.rack_rate = 1.45;
    s.steer_servo = 6000.0;
    s.upper_mass = 450.0;
    s.upper_profile = list_make<Vector, "6">(Vector{-4.0, 2.6, 0.0}, Vector{-4.0, 2.9, 0.0}, Vector{-3.85, 3.05, 0.0}, Vector{3.8, 3.05, 0.0}, Vector{4.0, 2.85, 0.0}, Vector{4.0, 2.6, 0.0});
    s.upper_half_width = 1.21;
    s.upper_dz = 1.48;
    return {s};
  }

  struct Car_rigid_specResult { RigidSpec Result = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false}; };
  Car_rigid_specResult Car_rigid_spec(const CarSpec& s) {
    RigidSpec r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false};
    ColliderSpec box = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    ColliderSpec upper = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float y = 0.0;
    Float total = 0.0;
    r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false};
    box = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    box.kind = physics_ShapeKind::box;
    box.size = vector_scale(s.half, 2.0);
    box.friction = 0.6;
    list_add(r.colliders, box);
    upper = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    upper.kind = physics_ShapeKind::hull;
    item = kern::formula([](Float upper_half_width) -> Float { return (-upper_half_width); }, s.upper_half_width);
    item_2 = s.upper_half_width;
    items = list_make<Float, "2">(item, item_2);
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it11.next();) {
      y = items[n11.index];
      for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(s.upper_profile))))); it12.next();) {
        list_add(upper.points, vector_make(vector_break(s.upper_profile[n12.index]).x, y, kern::formula([](Float y, Float upper_dz) -> Float { return (y - upper_dz); }, vector_break(s.upper_profile[n12.index]).y, s.upper_dz)));
      }
    }
    upper.friction = 0.5;
    list_add(r.colliders, upper);
    total = s.mass;
    r.mass.mass = total;
    r.mass.center = vector_make(s.com_x, 0.0, s.com_z);
    rigid_spec::box_inertia(r.mass, total, vector_scale(s.half, 2.0));
    r.linear_damping = 0.0;
    r.angular_damping = 0.05;
    return {r};
  }

  void Car_reset(Car& self) {
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WheelState>(self.w_))))); it13.next();) {
      self.w_[n13.index] = struct_make<WheelState>(0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, false, false, Vector{0.0, 0.0, 0.0});
    }
    self.throttle_ = 0.0;
    self.vf_ = self.throttle_;
    self.steer_eq_f_ = self.vf_;
    self.steer_ = self.steer_eq_f_;
    self.gear_ = 1;
    self.n_applied_ = 0;
    self.has_last_ = false;
  }

  struct Car_wheel_speedResult { Float Result = 0.0; };
  KERN_PURE Car_wheel_speedResult Car_wheel_speed(const Car& self) {
    Float m = 0.0;
    m = 0.0;
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WheelState>(self.w_))))); it14.next();) {
      m = kern::formula([](Float m, Float a) -> Float { return kern::math::max(m, kern::math::abs(a)); }, m, self.w_[n14.index].omega);
    }
    return {kern::formula([](Float m, Float radius) -> Float { return (m * radius); }, m, self.s_.radius)};
  }

  struct suspension_tuningResult { Tuning Result = Tuning{0.0, 0.0}; };
  /// Box3D's joint spring is a soft constraint: its hertz and damping ratio act on the joint's effective mass (about the light wheel), not on the chassis. They are picked so the spring really carries a quarter of the chassis at the target sag with a well damped body motion (world_vehicle_dynamics.mjs suspensionTuning).
  KERN_PURE suspension_tuningResult suspension_tuning(const CarSpec& s, Float zeta = 0.0, Float k_share = 0.0) {
    Float q = 0.0;
    Float m = 0.0;
    Float k = 0.0;
    Float w = 0.0;
    Float c = 0.0;
    q = kern::formula([](Float mass) -> Float { return kern::math::div(mass, 4.0); }, s.mass);
    m = kern::formula([](Float wheel_mass, Float q) -> Float { return kern::math::div((wheel_mass * q), (wheel_mass + q)); }, s.wheel_mass, q);
    k = kern::formula([](Float q, Float sag, Float k_share) -> Float { return (kern::math::div((q * 9.81), sag) * k_share); }, q, s.sag, k_share);
    w = kern::formula([](Float k, Float m) -> Float { return kern::math::sqrt(kern::math::div(k, m)); }, k, m);
    c = kern::formula([](Float zeta, Float q, Float sag) -> Float { return ((2.0 * zeta) * kern::math::sqrt(kern::math::div(((q * q) * 9.81), sag))); }, zeta, q, s.sag);
    return {struct_make<Tuning>(kern::formula([](Float w, Float kPi) -> Float { return kern::math::div(w, (2.0 * kPi)); }, w, kPi), kern::formula([](Float c, Float m, Float w) -> Float { return kern::math::div(c, ((2.0 * m) * w)); }, c, m, w))};
  }

  void Car_attach(Car& self, Object<kern::physics::World> world = Object<kern::physics::World>{}, physics_Body chassis = physics_Body{Int64{0x0}}) {
    Object<kern::physics::World> world_ = Object<kern::physics::World>{};
    BodyState st = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float r = 0.0;
    Float volume = 0.0;
    Vector local = Vector{0.0, 0.0, 0.0};
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ground = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    physics_ShapeSettings rubber = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape_2 = physics_Shape{Int64{0x0}};
    physics_JointSettings js = physics_JointSettings{physics_JointType::revolute, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, false, false, 0.0, 0.0, false, 0.0, 0.0, false, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, 1.0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}};
    Tuning spring = Tuning{0.0, 0.0};
    physics_Joint create_joint = physics_Joint{Int64{0x0}};
    physics_JointSettings ds = physics_JointSettings{physics_JointType::revolute, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, false, false, 0.0, 0.0, false, 0.0, 0.0, false, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, 1.0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}};
    Tuning strut = Tuning{0.0, 0.0};
    physics_Joint create_joint_2 = physics_Joint{Int64{0x0}};
    world_ = world;
    Car_detach(self);
    st = common::read_body(world_, chassis).Result;
    r = self.s_.radius;
    volume = kern::formula([](Float kPi, Float r) -> Float { return ((((kern::math::div(4.0, 3.0) * kPi) * r) * r) * r); }, kPi, r);
    for (auto it15 = flow_for(n15, 0, 3); it15.next();) {
      local = vector_make(self.s_.wheels[n15.index].x, self.s_.wheels[n15.index].y, self.s_.attach_z);
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs.type = physics_BodyType::dynamic;
      bs.position = vector_add(st.position, engine_math_math::Quat_rotate(st.rotation, vector_add(local, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float a_2, Float sag) -> Float { return ((a != 0.0) ? a_2 : sag); }, kern::to<Float>(self.w_[n15.index].has_travel), self.w_[n15.index].travel, self.s_.sag), self.s_.susp_lower, self.s_.susp_upper))))).Result);
      bs.rotation = st.rotation;
      bs.linear_velocity = st.velocity;
      bs.angular_damping = 0.05;
      bs.linear_damping = 0.02;
      engine_world_create_body(n16, world_, bs);
      create_body = n16.result;
      self.rig_[n15.index].body = create_body;
      ground = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ground.kind = physics_ShapeKind::sphere;
      ground.radius = r;
      ground.material.density = kern::formula([](Float wheel_mass, Float volume) -> Float { return kern::math::div(wheel_mass, volume); }, self.s_.wheel_mass, volume);
      ground.material.friction = 0.0;
      ground.material.restitution = 0.0;
      ground.material.category_bits = Int64{0x2};
      ground.material.mask_bits = Int64{0x1};
      ground.material.user = self.hit_user;
      engine_world_create_shape(n17, world_, self.rig_[n15.index].body, ground);
      create_shape = n17.result;
      rubber = ground;
      rubber.material.density = 0.0;
      rubber.material.friction = kRubberFriction;
      rubber.material.restitution = 0.05;
      rubber.material.mask_bits = int64_not(Int64{0x1});
      engine_world_create_shape(n18, world_, self.rig_[n15.index].body, rubber);
      create_shape_2 = n18.result;
      js = physics_JointSettings{physics_JointType::revolute, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, false, false, 0.0, 0.0, false, 0.0, 0.0, false, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, 1.0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}};
      js.type = physics_JointType::wheel;
      js.body_a = chassis;
      js.body_b = self.rig_[n15.index].body;
      js.frame_a = struct_make<physics_Pose>(local, kWheelFrame);
      js.frame_b = struct_make<physics_Pose>(Vector{0.0, 0.0, 0.0}, kWheelFrame);
      js.collide_connected = false;
      spring = suspension_tuning(self.s_, kSpringZeta, kern::formula([](Float kStrutShare) -> Float { return (1.0 - kStrutShare); }, kStrutShare)).Result;
      js.enable_spring = true;
      js.hertz = spring.hertz;
      js.damping_ratio = spring.damping_ratio;
      js.enable_limit = true;
      js.lower = self.s_.susp_lower;
      js.upper = self.s_.susp_upper;
      js.enable_motor = true;
      js.motor_speed = 0.0;
      js.max_motor_torque = kern::formula([](Float mass) -> Float { return (mass * 40.0); }, self.s_.mass);
      if (self.s_.wheels[n15.index].front) {
        js.enable_steering = true;
        js.steering_hertz = 9.0;
        js.steering_damping_ratio = 0.95;
        js.max_steering_torque = self.s_.steer_servo;
        js.target_steering = self.steer_;
        js.enable_steering_limit = true;
        js.steering_lower = kern::formula([](Float steer_lock) -> Float { return (-steer_lock); }, self.s_.steer_lock);
        js.steering_upper = self.s_.steer_lock;
      }
      engine_world_create_joint(n19, world_, js);
      create_joint = n19.result;
      self.rig_[n15.index].joint = create_joint;
      ds = physics_JointSettings{physics_JointType::revolute, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, false, false, 0.0, 0.0, false, 0.0, 0.0, false, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, 1.0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}};
      ds.type = physics_JointType::distance;
      ds.body_a = chassis;
      ds.body_b = self.rig_[n15.index].body;
      ds.frame_a = struct_make<physics_Pose>(vector_make(vector_break(local).x, vector_break(local).y, kern::formula([](Float z, Float kStrutM) -> Float { return (z + kStrutM); }, vector_break(local).z, kStrutM)), struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
      ds.frame_b = struct_make<physics_Pose>(Vector{0.0, 0.0, 0.0}, struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
      ds.collide_connected = false;
      ds.length = kStrutM;
      strut = suspension_tuning(self.s_, kern::formula([](Float kBodyDamping, Float kSpringZeta) -> Float { return kern::math::max(0.0, (kBodyDamping - kSpringZeta)); }, kBodyDamping, kSpringZeta), kStrutShare).Result;
      ds.enable_spring = true;
      ds.hertz = strut.hertz;
      ds.damping_ratio = strut.damping_ratio;
      ds.enable_limit = false;
      ds.lower = kern::formula([](Float kStrutM, Float susp_upper) -> Float { return ((kStrutM - susp_upper) - 0.25); }, kStrutM, self.s_.susp_upper);
      ds.upper = kern::formula([](Float kStrutM, Float susp_lower) -> Float { return ((kStrutM - susp_lower) + 0.25); }, kStrutM, self.s_.susp_lower);
      engine_world_create_joint(n20, world_, ds);
      create_joint_2 = n20.result;
      self.rig_[n15.index].strut = create_joint_2;
    }
    self.rig_world_ = world_;
    self.rig_chassis_ = chassis;
    self.last_pos_ = st.position;
    self.has_last_ = true;
  }

  void Car_place(Car& self, const BodyState& st, Object<kern::physics::World> world = Object<kern::physics::World>{}) {
    Object<kern::physics::World> world_ = Object<kern::physics::World>{};
    Bool skip = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool either = false;
    Vector local = Vector{0.0, 0.0, 0.0};
    Vector arm = Vector{0.0, 0.0, 0.0};
    world_ = world;
    for (auto it21 = flow_for(n21, 0, 3); it21.next();) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(self.rig_[n21.index].body).Result);
      if (logic_not(either)) {
        body_valid_2 = engine_world_body_valid(world_, self.rig_[n21.index].body);
        either = logic_not(body_valid_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        local = vector_make(self.s_.wheels[n21.index].x, self.s_.wheels[n21.index].y, kern::formula([](Float attach_z, Float a) -> Float { return (attach_z + a); }, self.s_.attach_z, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float a_2, Float sag) -> Float { return ((a != 0.0) ? a_2 : sag); }, kern::to<Float>(self.w_[n21.index].has_travel), self.w_[n21.index].travel, self.s_.sag), self.s_.susp_lower, self.s_.susp_upper)));
        arm = engine_math_math::Quat_rotate(st.rotation, local).Result;
        engine_world_set_pose(world_, self.rig_[n21.index].body, struct_make<physics_Pose>(vector_add(st.position, arm), st.rotation));
        engine_world_set_linear_velocity(world_, self.rig_[n21.index].body, vector_add(st.velocity, vector_cross(st.angular, arm)));
        engine_world_set_angular_velocity(world_, self.rig_[n21.index].body, st.angular);
        engine_world_set_awake(world_, self.rig_[n21.index].body, true);
      }
    }
  }

  struct Car_ensure_rigResult { Bool Result = false; };
  Car_ensure_rigResult Car_ensure_rig(Car& self, Object<kern::physics::World> world = Object<kern::physics::World>{}, physics_Body chassis = physics_Body{Int64{0x0}}, Float dt = 0.0) {
    Object<kern::physics::World> world_ = Object<kern::physics::World>{};
    physics_BodyType body_type = physics_BodyType::fixed;
    Bool whole = false;
    I32 i = 0;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Bool joint_valid = false;
    Bool joint_valid_2 = false;
    Bool both_2 = false;
    BodyState st = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    world_ = world;
    body_type = engine_world_body_type(world_, chassis);
    if (logic_not(enum_equal<physics_BodyType>(body_type, physics_BodyType::dynamic))) {
      Car_detach(self);
      return {false};
    }
    whole = logic_and(object_equal<kern::physics::World>(self.rig_world_, world_), engine_physics_physics::Body_equal(self.rig_chassis_, chassis).Result);
    i = 0;
    while (loop(logic_and(whole, math_compare<"<">(kern::to<Float>(i), 4.0)))) {
      both = engine_physics_physics::Body_valid(self.rig_[i].body).Result;
      if (both) {
        body_valid_2 = engine_world_body_valid(world_, self.rig_[i].body);
        both = body_valid_2;
      }
      both_2 = both;
      if (both_2) {
        joint_valid_2 = engine_world_joint_valid(world_, self.rig_[i].joint);
        both_2 = joint_valid_2;
      }
      whole = both_2;
      i += 1;
    }
    st = common::read_body(world_, chassis).Result;
    if (logic_not(whole)) {
      Car_attach(self, world_, chassis);
      return {true};
    }
    if (logic_or(logic_not(self.has_last_), math_compare<">">(vector_length(vector_sub(st.position, self.last_pos_)), kern::formula([](Float a, Float dt) -> Float { return (((a * kern::math::max(dt, kern::math::div(1.0, 240.0))) * 2.0) + 0.6); }, vector_length(st.velocity), dt)))) {
      Car_place(self, st, world_);
    }
    self.last_pos_ = st.position;
    self.has_last_ = true;
    return {true};
  }

  void Car_push(Car& self, const Quat& q, Object<kern::physics::World> world = Object<kern::physics::World>{}, physics_Body body = physics_Body{Int64{0x0}}, Vector origin = Vector{0.0, 0.0, 0.0}, Vector f = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}, I32 on = 0) {
    Object<kern::physics::World> world_ = Object<kern::physics::World>{};
    Int was = 0;
    world_ = world;
    if (math_compare<"<">(kern::to<Float>(on), 0.0)) {
      engine_world_apply_force_to_center(world_, body, f);
    } else {
      engine_world_apply_force(world_, self.rig_[on].body, f, p);
    }
    if (math_compare<"<">(kern::to<Float>(self.n_applied_), 6.0)) {
      was = self.n_applied_;
      self.n_applied_ = kern::to<Int>(kern::formula([](Float n_applied_) -> Float { return (n_applied_ + 1.0); }, kern::to<Float>(self.n_applied_)));
      self.applied_[was] = struct_make<Car_Applied>(common::to_local(q, vector_sub(p, origin)).Result, common::to_local(q, f).Result, on);
    }
  }

  void Car_reapply(const Car& self, Object<kern::physics::World> world = Object<kern::physics::World>{}, physics_Body body = physics_Body{Int64{0x0}}) {
    Object<kern::physics::World> world_ = Object<kern::physics::World>{};
    BodyState st = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector f = Vector{0.0, 0.0, 0.0};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    world_ = world;
    if (logic_or(logic_not(object_equal<kern::physics::World>(self.rig_world_, world_)), logic_not(engine_physics_physics::Body_equal(self.rig_chassis_, body).Result))) {
      return;
    }
    st = common::read_body(world_, body).Result;
    if (logic_and(self.has_last_, math_compare<">">(vector_length(vector_sub(st.position, self.last_pos_)), kern::formula([](Float a) -> Float { return ((kern::math::div(a, 60.0) * 2.0) + 0.6); }, vector_length(st.velocity))))) {
      return;
    }
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(self.n_applied_)))); it22.next();) {
      f = engine_math_math::Quat_rotate(st.rotation, self.applied_[n22.index].f).Result;
      if (math_compare<"<">(kern::to<Float>(self.applied_[n22.index].on), 0.0)) {
        engine_world_apply_force_to_center(world_, body, f);
      } else {
        both = engine_physics_physics::Body_valid(self.rig_[self.applied_[n22.index].on].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(world_, self.rig_[self.applied_[n22.index].on].body);
          both = body_valid_2;
        }
        if (both) {
          engine_world_apply_force(world_, self.rig_[self.applied_[n22.index].on].body, f, vector_add(st.position, engine_math_math::Quat_rotate(st.rotation, self.applied_[n22.index].p).Result));
        }
      }
    }
  }

  struct step_point_velocityResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE step_point_velocityResult step_point_velocity(const BodyState& st, Vector com = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}) {
    return {vector_add(st.velocity, vector_cross(st.angular, vector_sub(p, com)))};
  }

  struct step_align_atResult { Float Result = 0.0; };
  step_align_atResult step_align_at(const Fixed<Car_step_Front, 2>& fronts, I32 nf = 0, Float d = 0.0) {
    Float M = 0.0;
    Fixed<Float, 2> tf = Fixed<Float, 2>{};
    Car_step_Front f = Car_step_Front{0.0, 0.0, 0.0, 0.0, 0.0};
    Float ca = 0.0;
    Float sa = 0.0;
    Float vx = 0.0;
    Float vy = 0.0;
    Float ta = 0.0;
    M = 0.0;
    tf = List<Float>{};
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(nf)))); it26.next();) {
      f = fronts[n26.index];
      ca = kern::formula([](Float d) -> Float { return kern::math::cos(d); }, d);
      sa = kern::formula([](Float d) -> Float { return kern::math::sin(d); }, d);
      vx = kern::formula([](Float pf, Float ca, Float pl, Float sa) -> Float { return ((pf * ca) + (pl * sa)); }, f.pf, ca, f.pl, sa);
      vy = kern::formula([](Float pf, Float sa, Float pl, Float ca) -> Float { return (((-pf) * sa) + (pl * ca)); }, f.pf, sa, f.pl, ca);
      ta = kern::formula([](Float vy, Float vx, Float kVReg) -> Float { return kern::math::div(vy, kern::math::max(kern::math::abs(vx), kVReg)); }, vy, vx, kVReg);
      tyre(tf, f.kappa, ta, f.muN);
      M += kern::formula([](Float kCasterTrail, Float kPneumaticTrail, Float ta, Float kTrailCollapse, Float kAlphaPeak, Float a, Float n, Float kSteerJacking, Float d) -> Float { return (((kCasterTrail + (kPneumaticTrail * kern::math::max(0.0, (1.0 - kern::math::div(kern::math::abs(ta), (kTrailCollapse * kAlphaPeak)))))) * a) + ((n * kSteerJacking) * kern::math::sin(d))); }, kCasterTrail, kPneumaticTrail, ta, kTrailCollapse, kAlphaPeak, tf[1], f.N, kSteerJacking, d);
    }
    return {M};
  }

  struct step_fResult { Float Result = 0.0; };
  step_fResult step_f(const Fixed<Car_step_Front, 2>& fronts, Float T = 0.0, I32 nf = 0, Float d = 0.0) {
    n27 = step_align_at(fronts, nf, d);
    return {kern::formula([](Float t, Float the_result) -> Float { return (t - the_result); }, T, n27.Result)};
  }

  struct ai_driveResult { CarInput Result = CarInput{0.0, 0.0, false, 32.0, 9.0}; };
  KERN_PURE ai_driveResult ai_drive(Vector position = Vector{0.0, 0.0, 0.0}, Vector forward = Vector{0.0, 0.0, 0.0}, Float vf = 0.0, Vector target = Vector{0.0, 0.0, 0.0}, Float target_speed = 0.0) {
    CarInput in = CarInput{0.0, 0.0, false, 32.0, 9.0};
    Float dx = 0.0;
    Float dy = 0.0;
    Float dist = 0.0;
    Float cr = 0.0;
    Float dt = 0.0;
    Float err = 0.0;
    Float steer = 0.0;
    Float R = 0.0;
    Float v_bend = 0.0;
    Float ce = 0.0;
    Float want = 0.0;
    Float pedal = 0.0;
    Bool behind = false;
    in = CarInput{0.0, 0.0, false, 32.0, 9.0};
    dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(target).x, vector_break(position).x);
    dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(target).y, vector_break(position).y);
    dist = kern::formula([](Float dx, Float dy) -> Float { return kern::math::sqrt((dx * dx) + (dy * dy)); }, dx, dy);
    if (math_compare<"<">(dist, 0.4)) {
      in.pedal = kern::formula([](Float vf) -> Float { return ((-0.4) * kern::math::sign(vf)); }, vf);
      in.max_speed = kern::formula([](Float target_speed) -> Float { return kern::math::max(4.0, (target_speed * 1.2)); }, target_speed);
      in.max_reverse = 4.0;
      return {in};
    }
    cr = kern::formula([](Float x, Float dy, Float y, Float dx) -> Float { return ((x * dy) - (y * dx)); }, vector_break(forward).x, dy, vector_break(forward).y, dx);
    dt = kern::formula([](Float x, Float dx, Float y, Float dy) -> Float { return ((x * dx) + (y * dy)); }, vector_break(forward).x, dx, vector_break(forward).y, dy);
    err = kern::formula([](Float cr, Float dt) -> Float { return kern::math::atan2(cr, dt); }, cr, dt);
    steer = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float err) -> Float { return (err * 1.9); }, err), -1.0, 1.0));
    R = kern::formula([](Float dist, Float err) -> Float { return kern::math::div(dist, kern::math::max(0.001, (2.0 * kern::math::abs(kern::math::sin(err))))); }, dist, err);
    v_bend = kern::formula([](Float r) -> Float { return kern::math::sqrt(((0.45 * 9.81) * r)); }, R);
    ce = kern::formula([](Float err) -> Float { return kern::math::cos(err); }, err);
    want = kern::formula([](Float target_speed, Float ce, Float v_bend) -> Float { return kern::math::min((target_speed * kern::math::max(0.25, (ce * ce))), kern::math::max(2.5, v_bend)); }, target_speed, ce, v_bend);
    pedal = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float want, Float vf) -> Float { return ((want - vf) * 0.45); }, want, vf), -1.0, 1.0));
    behind = logic_and(math_compare<"<">(dt, 0.0), math_compare<"<">(dist, 6.0));
    in.pedal = kern::formula([](Float behind, Float pedal) -> Float { return ((behind != 0.0) ? (-0.6) : pedal); }, kern::to<Float>(behind), pedal);
    in.steer = kern::formula([](Float behind, Float steer) -> Float { return ((behind != 0.0) ? (-steer) : steer); }, kern::to<Float>(behind), steer);
    in.max_speed = kern::formula([](Float target_speed) -> Float { return kern::math::max(6.0, (target_speed * 1.25)); }, target_speed);
    in.max_reverse = 5.0;
    return {in};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n16;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n17;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n18;
  struct { physics_Joint result = physics_Joint{Int64{0x0}}; } n19;
  struct { physics_Joint result = physics_Joint{Int64{0x0}}; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<26> { Int index = 0; } n26;
  step_align_atResult n27;
};
