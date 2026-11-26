struct drone_cpp : virtual kern::Library, virtual common, virtual drone, virtual rigid_spec {
  // ---- types ----
  KERN_STRUCT(MassPart) { Float m = 0.0; Vector pos = Vector{0.0, 0.0, 0.0}; Vector size = Vector{0.0, 0.0, 0.0}; Bool disk = false; };

  // ---- variables ----
  KERN_CONST Float kDeg = 0.017453292519943295;
  KERN_CONST Float kG = 9.80665;

  // ---- functions ----
  struct battery_under_loadResult { Float Result = 0.0; };
  KERN_PURE battery_under_loadResult battery_under_load(Float ocv = 0.0, Float amps = 0.0, Float r = 0.0, Float cells = 0.0) {
    return {kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float ocv, Float amps, Float r) -> Float { return (ocv - (kern::math::max(0.0, amps) * r)); }, ocv, amps, r), kern::formula([](Float cells) -> Float { return (3.0 * cells); }, cells), kern::formula([](Float cells) -> Float { return (4.2 * cells); }, cells)))};
  }

  void encode_sbus(const Fixed<U16, 16>& ch, Fixed<U8, 25>& p) {
    U8 fill = 0;
    Int last = 0;
    I32 k = 0;
    fill = 0;
    last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(p))));
    for (auto it10 = flow_for(n10, 0, last); it10.next();) {
      p[n10.index] = fill;
    }
    p[0] = 15;
    for (auto it11 = flow_for(n11, 0, 15); it11.next();) {
      for (auto it12 = flow_for(n12, 0, 10); it12.next();) {
        if (math_compare<">">(kern::formula([](Float a, Float index) -> Float { return (kern::math::band(a, kern::math::wrap32(kern::math::shl(1.0, index))) != 0.0); }, kern::to<Float>(ch[n11.index]), kern::to<Float>(n12.index)), 0.5)) {
          k = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((8.0 + (index * 11.0)) + index_2); }, kern::to<Float>(n11.index), kern::to<Float>(n12.index)));
          p[kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::trunc(kern::math::div(k, 8.0)); }, kern::to<Float>(k)))] = kern::to<Int>(kern::formula([](Float a, Float k) -> Float { return kern::math::bor(a, kern::math::band(kern::math::wrap32(kern::math::shl(1.0, kern::math::mod(k, 8.0))), 255.0)); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::trunc(kern::math::div(k, 8.0)); }, kern::to<Float>(k)))]), kern::to<Float>(k)));
        }
      }
    }
  }

  /// a big-endian 16-bit register pair at `at`
  void put_be16(Fixed<U8, 14>& out, U32 at = 0, Float v = 0.0) {
    Int r = 0;
    U16 u = 0;
    r = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::round((v < (-32767.0) ? (-32767.0) : (32767.0 < v ? 32767.0 : v))); }, v));
    u = kern::to<Int>(kern::formula([](Float r) -> Float { return kern::math::band((kern::math::bxor(kern::math::band(r, 65535.0), 32768.0) - 32768.0), 65535.0); }, kern::to<Float>(r)));
    out[at] = kern::to<Int>(kern::formula([](Float u) -> Float { return kern::math::band(kern::math::shr(u, 8.0), 255.0); }, kern::to<Float>(u)));
    out[kern::to<Int>(kern::formula([](Float at) -> Float { return kern::math::wrap32(at + 1.0); }, kern::to<Float>(at)))] = kern::to<Int>(kern::formula([](Float u) -> Float { return kern::math::band(kern::math::band(u, 255.0), 255.0); }, kern::to<Float>(u)));
  }

  struct quat_yaw_degResult { Float Result = 0.0; };
  KERN_PURE quat_yaw_degResult quat_yaw_deg(Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    return {kern::formula([](Float w, Float z, Float x, Float y, Float kDeg) -> Float { return kern::math::div(kern::math::atan2((2.0 * ((w * z) + (x * y))), (1.0 - (2.0 * ((y * y) + (z * z))))), kDeg); }, q.w, q.z, q.x, q.y, kDeg)};
  }

  struct clearance_rateResult { Float Result = 0.0; };
  KERN_PURE clearance_rateResult clearance_rate(Float axis = 0.0) {
    Float x = 0.0;
    Float m = 0.0;
    Float unit = 0.0;
    x = kern::formula([](Float axis) -> Float { return (axis < (-1.0) ? (-1.0) : (1.0 < axis ? 1.0 : axis)); }, axis);
    m = kern::formula([](Float x) -> Float { return kern::math::abs(x); }, x);
    if (math_compare<"<=">(m, 0.08)) {
      return {0.0};
    }
    unit = kern::formula([](Float m) -> Float { return kern::math::div((m - 0.08), (1.0 - 0.08)); }, m);
    return {kern::formula([](Float x, Float unit) -> Float { return (((kern::math::sign(x) * 12.0) * unit) * (0.25 + (0.75 * unit))); }, x, unit)};
  }

  struct clearance_leadResult { Float Result = 0.0; };
  KERN_PURE clearance_leadResult clearance_lead(Float v = 0.0) {
    Bool up = false;
    Float a = 0.0;
    Float tau = 0.0;
    Float k = 0.0;
    Float s = 0.0;
    Float e = 0.0;
    up = math_compare<">=">(v, 0.0);
    a = kern::formula([](Float up) -> Float { return ((up != 0.0) ? 6.0 : 4.0); }, kern::to<Float>(up));
    tau = kern::formula([](Float up) -> Float { return ((up != 0.0) ? 0.25 : 0.125); }, kern::to<Float>(up));
    k = 2.0;
    s = kern::formula([](Float v) -> Float { return kern::math::abs(v); }, v);
    e = kern::formula([](Float s, Float k, Float a, Float tau) -> Float { return kern::math::max(kern::math::div(s, k), kern::math::div(((s * s) + (((2.0 * a) * tau) * s)), (2.0 * a))); }, s, k, a, tau);
    return {kern::formula([](Float up, Float e) -> Float { return ((up != 0.0) ? e : (-e)); }, kern::to<Float>(up), e)};
  }

  struct Drone_rigid_specResult { RigidSpec Result = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false}; };
  KERN_PURE Drone_rigid_specResult Drone_rigid_spec(const DroneParams& p) {
    RigidSpec r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false};
    Float a = 0.0;
    Fixed<Vector, 4> motors = Fixed<Vector, 4>{};
    ColliderSpec box = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    ColliderSpec arm = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    ColliderSpec bell = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    ColliderSpec sweep = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    List<MassPart> parts = List<MassPart>{};
    Float mass = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Fixed<Float, 6> I = Fixed<Float, 6>{};
    Fixed<Float, 3> l = Fixed<Float, 3>{};
    Float rr = 0.0;
    Float t = 0.0;
    Vector o = Vector{0.0, 0.0, 0.0};
    r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false};
    a = kern::formula([](Float span) -> Float { return kern::math::div(span, (2.0 * kern::math::sqrt(2.0))); }, p.span);
    motors = list_make<Vector, "4">(vector_make(kern::formula([](Float a) -> Float { return (-a); }, a), kern::formula([](Float a) -> Float { return (-a); }, a), 0.0), vector_make(kern::formula([](Float a) -> Float { return (-a); }, a), a, 0.0), vector_make(a, a, 0.0), vector_make(a, kern::formula([](Float a) -> Float { return (-a); }, a), 0.0));
    box = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    box.kind = physics_ShapeKind::box;
    box.size = Vector{0.11, 0.09, 0.044};
    box.friction = 0.65;
    box.restitution = 0.08;
    list_add(r.colliders, box);
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(motors))))); it15.next();) {
      arm = rigid_spec::capsule_between(Vector{0.0, 0.0, 0.0}, motors[n15.index], 0.008).Result;
      arm.friction = 0.65;
      arm.restitution = 0.08;
      list_add(r.colliders, arm);
      bell = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
      bell.kind = physics_ShapeKind::sphere;
      bell.center = motors[n15.index];
      bell.radius = 0.018;
      bell.friction = 0.65;
      bell.restitution = 0.08;
      list_add(r.colliders, bell);
      sweep = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
      sweep.kind = physics_ShapeKind::box;
      sweep.center = vector_add(motors[n15.index], Vector{0.0, 0.0, 0.014});
      sweep.size = vector_make(kern::formula([](Float prop_d) -> Float { return (prop_d * 0.9); }, p.prop_d), kern::formula([](Float prop_d) -> Float { return (prop_d * 0.9); }, p.prop_d), 0.008);
      sweep.friction = 0.65;
      sweep.restitution = 0.08;
      list_add(r.colliders, sweep);
    }
    parts = List<MassPart>{};
    list_add(parts, struct_make<MassPart>(p.m_frame, Vector{0.0, 0.0, 0.0}, vector_make(kern::formula([](Float span) -> Float { return (span * 0.62); }, p.span), kern::formula([](Float span) -> Float { return (span * 0.48); }, p.span), 0.018), false));
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(motors))))); it16.next();) {
      list_add(parts, struct_make<MassPart>(p.m_motor, motors[n16.index], Vector{0.036, 0.036, 0.025}, false));
      list_add(parts, struct_make<MassPart>(p.m_prop, vector_make(vector_break(motors[n16.index]).x, vector_break(motors[n16.index]).y, 0.014), vector_make(kern::formula([](Float prop_d) -> Float { return (prop_d * 0.5); }, p.prop_d), 0.002, 0.0), true));
      list_add(parts, struct_make<MassPart>(kern::formula([](Float m_wiring) -> Float { return kern::math::div(m_wiring, 4.0); }, p.m_wiring), vector_make(kern::formula([](Float x) -> Float { return kern::math::div(x, 2.0); }, vector_break(motors[n16.index]).x), kern::formula([](Float y) -> Float { return kern::math::div(y, 2.0); }, vector_break(motors[n16.index]).y), -0.002), vector_make(kern::formula([](Float span) -> Float { return (span * 0.245); }, p.span), 0.016, 0.008), false));
    }
    list_add(parts, struct_make<MassPart>(p.m_battery, vector_make(p.battery_x, 0.0, p.battery_z), Vector{0.105, 0.035, 0.032}, false));
    list_add(parts, struct_make<MassPart>(p.m_esc, Vector{0.0, 0.0, -0.002}, Vector{0.05, 0.05, 0.008}, false));
    list_add(parts, struct_make<MassPart>(p.m_fc_rx, Vector{0.0, 0.0, 0.008}, Vector{0.04, 0.04, 0.012}, false));
    list_add(parts, struct_make<MassPart>(p.m_cam_vtx, vector_make(p.camera_x, 0.0, p.camera_z), Vector{0.04, 0.03, 0.025}, false));
    mass = 0.0;
    c = Vector{0.0, 0.0, 0.0};
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MassPart>(parts))))); it17.next();) {
      mass += parts[n17.index].m;
      c = vector_add(c, vector_scale(parts[n17.index].pos, parts[n17.index].m));
    }
    c = vector_scale(c, kern::formula([](Float mass) -> Float { return kern::math::div(1.0, mass); }, mass));
    I = List<Float>{};
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MassPart>(parts))))); it18.next();) {
      l = List<Float>{};
      if (parts[n18.index].disk) {
        rr = vector_break(parts[n18.index].size).x;
        t = vector_break(parts[n18.index].size).y;
        l[1] = kern::formula([](Float a, Float rr, Float t) -> Float { return kern::math::div((a * (((3.0 * rr) * rr) + (t * t))), 12.0); }, parts[n18.index].m, rr, t);
        l[0] = l[1];
        l[2] = kern::formula([](Float a, Float rr) -> Float { return kern::math::div(((a * rr) * rr), 2.0); }, parts[n18.index].m, rr);
      } else {
        l[0] = kern::formula([](Float a, Float y, Float z) -> Float { return kern::math::div((a * ((y * y) + (z * z))), 12.0); }, parts[n18.index].m, vector_break(parts[n18.index].size).y, vector_break(parts[n18.index].size).z);
        l[1] = kern::formula([](Float a, Float x, Float z) -> Float { return kern::math::div((a * ((x * x) + (z * z))), 12.0); }, parts[n18.index].m, vector_break(parts[n18.index].size).x, vector_break(parts[n18.index].size).z);
        l[2] = kern::formula([](Float a, Float x, Float y) -> Float { return kern::math::div((a * ((x * x) + (y * y))), 12.0); }, parts[n18.index].m, vector_break(parts[n18.index].size).x, vector_break(parts[n18.index].size).y);
      }
      o = vector_sub(parts[n18.index].pos, c);
      I[0] = kern::formula([](Float a, Float a_2, Float a_3, Float y, Float z) -> Float { return (a + (a_2 + (a_3 * ((y * y) + (z * z))))); }, I[0], l[0], parts[n18.index].m, vector_break(o).y, vector_break(o).z);
      I[1] = kern::formula([](Float a, Float a_2, Float a_3, Float x, Float z) -> Float { return (a + (a_2 + (a_3 * ((x * x) + (z * z))))); }, I[1], l[1], parts[n18.index].m, vector_break(o).x, vector_break(o).z);
      I[2] = kern::formula([](Float a, Float a_2, Float a_3, Float x, Float y) -> Float { return (a + (a_2 + (a_3 * ((x * x) + (y * y))))); }, I[2], l[2], parts[n18.index].m, vector_break(o).x, vector_break(o).y);
      I[3] = kern::formula([](Float a, Float a_2, Float x, Float y) -> Float { return (a + (((-a_2) * x) * y)); }, I[3], parts[n18.index].m, vector_break(o).x, vector_break(o).y);
      I[4] = kern::formula([](Float a, Float a_2, Float x, Float z) -> Float { return (a + (((-a_2) * x) * z)); }, I[4], parts[n18.index].m, vector_break(o).x, vector_break(o).z);
      I[5] = kern::formula([](Float a, Float a_2, Float y, Float z) -> Float { return (a + (((-a_2) * y) * z)); }, I[5], parts[n18.index].m, vector_break(o).y, vector_break(o).z);
    }
    r.mass.mass = mass;
    r.mass.center = c;
    for (auto it19 = flow_for(n19, 0, 5); it19.next();) {
      r.mass.inertia[n19.index] = I[n19.index];
    }
    r.linear_damping = 0.002;
    r.angular_damping = 0.002;
    r.bullet = true;
    return {r};
  }

  void Drone_imu_bytes(Drone& self, const BodyState& s, Fixed<U8, 14>& out, Float dt = 0.0) {
    Vector omega_body = Vector{0.0, 0.0, 0.0};
    Vector alpha = Vector{0.0, 0.0, 0.0};
    Vector specific = Vector{0.0, 0.0, 0.0};
    Vector off = Vector{0.0, 0.0, 0.0};
    Vector accel = Vector{0.0, 0.0, 0.0};
    Vector gyro = Vector{0.0, 0.0, 0.0};
    Float at = 0.0;
    Float put = 0.0;
    Float at_2 = 0.0;
    Float put_2 = 0.0;
    for (auto it20 = flow_for(n20, 0, 2); it20.next();) {
      n21 = common::Rng_gaussian(self.noise_);
      self.gyro_bias_[n20.index] = kern::formula([](Float a, Float the_result, Float dt) -> Float { return (a + ((the_result * 0.002) * kern::math::sqrt(dt))); }, self.gyro_bias_[n20.index], n21.Result, dt);
      n22 = common::Rng_gaussian(self.noise_);
      self.acc_bias_[n20.index] = kern::formula([](Float a, Float the_result, Float dt) -> Float { return (a + ((the_result * 2e-05) * kern::math::sqrt(dt))); }, self.acc_bias_[n20.index], n22.Result, dt);
    }
    omega_body = common::to_local(s.rotation, s.angular).Result;
    alpha = vector_scale(vector_sub(omega_body, self.prev_omega_body_), kern::formula([](Float dt) -> Float { return kern::math::div(1.0, dt); }, dt));
    self.prev_omega_body_ = omega_body;
    specific = common::to_local(s.rotation, vector_sub(self.world_accel_, vector_make(0.0, 0.0, kern::formula([](Float kG) -> Float { return (-kG); }, kG)))).Result;
    off = Vector{0.0, 0.0, 0.008};
    accel = vector_add(vector_add(specific, vector_cross(alpha, off)), vector_cross(omega_body, vector_cross(omega_body, off)));
    gyro = vector_scale(omega_body, kern::formula([](Float kDeg) -> Float { return kern::math::div(1.0, kDeg); }, kDeg));
    for (auto it23 = flow_for(n23, 0, 2); it23.next();) {
      n24 = common::Rng_gaussian(self.noise_);
      at = kern::to<Float>(n23.index);
      put = kern::formula([](Float a, Float a_2, Float kG, Float the_result) -> Float { return (a + ((a_2 * kG) + ((the_result * 0.0025) * kG))); }, kern::formula([](Float i, Float x, Float y, Float z) -> Float { return i == 0.0 ? x : (i == 1.0 ? y : z); }, kern::to<Float>(n23.index), vector_break(accel).x, vector_break(accel).y, vector_break(accel).z), self.acc_bias_[n23.index], kG, n24.Result);
      accel = vector_make(kern::formula([](Float i, Float v, Float c) -> Float { return i == 0.0 ? v : c; }, at, put, vector_break(accel).x), kern::formula([](Float i, Float v, Float c) -> Float { return i == 1.0 ? v : c; }, at, put, vector_break(accel).y), kern::formula([](Float i, Float v, Float c) -> Float { return i != 0.0 && i != 1.0 ? v : c; }, at, put, vector_break(accel).z));
      n25 = common::Rng_gaussian(self.noise_);
      at_2 = kern::to<Float>(n23.index);
      put_2 = kern::formula([](Float a, Float a_2, Float the_result) -> Float { return (a + (a_2 + (the_result * 0.035))); }, kern::formula([](Float i, Float x, Float y, Float z) -> Float { return i == 0.0 ? x : (i == 1.0 ? y : z); }, kern::to<Float>(n23.index), vector_break(gyro).x, vector_break(gyro).y, vector_break(gyro).z), self.gyro_bias_[n23.index], n25.Result);
      gyro = vector_make(kern::formula([](Float i, Float v, Float c) -> Float { return i == 0.0 ? v : c; }, at_2, put_2, vector_break(gyro).x), kern::formula([](Float i, Float v, Float c) -> Float { return i == 1.0 ? v : c; }, at_2, put_2, vector_break(gyro).y), kern::formula([](Float i, Float v, Float c) -> Float { return i != 0.0 && i != 1.0 ? v : c; }, at_2, put_2, vector_break(gyro).z));
    }
    out[0] = 0;
    out[1] = 0;
    put_be16(out, 2, kern::formula([](Float x, Float kG) -> Float { return (kern::math::div(x, kG) * 2048.0); }, vector_break(accel).x, kG));
    put_be16(out, 4, kern::formula([](Float y, Float kG) -> Float { return (kern::math::div(y, kG) * 2048.0); }, vector_break(accel).y, kG));
    put_be16(out, 6, kern::formula([](Float z, Float kG) -> Float { return (kern::math::div(z, kG) * 2048.0); }, vector_break(accel).z, kG));
    put_be16(out, 8, kern::formula([](Float x) -> Float { return (x * 16.4); }, vector_break(gyro).x));
    put_be16(out, 10, kern::formula([](Float y) -> Float { return (y * 16.4); }, vector_break(gyro).y));
    put_be16(out, 12, kern::formula([](Float z) -> Float { return (z * 16.4); }, vector_break(gyro).z));
  }

  struct step_centeredResult { U16 Result = 0; };
  KERN_PURE step_centeredResult step_centered(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::band(kern::math::round((992.0 + (820.0 * (v < (-1.0) ? (-1.0) : (1.0 < v ? 1.0 : v))))), 65535.0); }, v))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  common::Rng_gaussianResult n21;
  common::Rng_gaussianResult n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  common::Rng_gaussianResult n24;
  common::Rng_gaussianResult n25;
};
