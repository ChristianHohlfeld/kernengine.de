struct drone_cpp : virtual kern::Library, virtual Arondight45_FirmwareRuntime, virtual Arondight45_HardwareSensors, virtual common, virtual drone, virtual engine_math_math, virtual engine_physics_physics, virtual rigid_spec {
  // ---- types ----
  KERN_STRUCT(MassPart) { Float m = 0.0; Vector pos = Vector{0.0, 0.0, 0.0}; Vector size = Vector{0.0, 0.0, 0.0}; Bool disk = false; };

  // ---- variables ----
  KERN_CONST Float kDeg = 0.017453292519943295;
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Float kG = 9.80665;
  /// propulsion_authority.mjs MOTOR_BEARING_DRAG_NM_PER_RAD_S
  KERN_CONST Float kBearingDrag = 1.5e-07;
  /// simulator.mjs MOTOR_YAW_SIGN
  KERN_CONST Fixed<Float, 4> kMotorYawSign = List<Float>{-1.0, 1.0, -1.0, 1.0};
  /// ceil(120 / cos^2(40 deg) + 10), control_semantics.mjs
  KERN_CONST Float kNavRayMax = 215.0;
  KERN_CONST Float kMinClearance = 0.5;
  KERN_CONST Float kMaxClearance = 120.0;
  /// The number of each value of fc_StateBits, in their order
  KERN_CONST List<Int> fc_StateBits_numbers = List<Int>{1, 2, 4, 8, 16};
  KERN_CONST Int kStateGameMode = 64;
  KERN_CONST Int kStateNavigationValid = 32;

  // ---- functions ----
  struct battery_ocvResult { Float Result = 0.0; };
  KERN_PURE battery_ocvResult battery_ocv(Float soc = 0.0, Float cells = 0.0) {
    Float s = 0.0;
    s = kern::formula([](Float soc) -> Float { return (soc < 0.0 ? 0.0 : (1.0 < soc ? 1.0 : soc)); }, soc);
    return {kern::formula([](Float cells, Float s) -> Float { return (cells * kern::math::min(4.2, ((3.2625 + (0.9 * s)) + (0.0375 * kern::math::div(kern::math::exp(2.0 * ((s - 0.12) * 18.0)) - 1.0, kern::math::exp(2.0 * ((s - 0.12) * 18.0)) + 1.0))))); }, cells, s)};
  }

  struct battery_under_loadResult { Float Result = 0.0; };
  KERN_PURE battery_under_loadResult battery_under_load(Float ocv = 0.0, Float amps = 0.0, Float r = 0.0, Float cells = 0.0) {
    return {kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float ocv, Float amps, Float r) -> Float { return (ocv - (kern::math::max(0.0, amps) * r)); }, ocv, amps, r), kern::formula([](Float cells) -> Float { return (3.0 * cells); }, cells), kern::formula([](Float cells) -> Float { return (4.2 * cells); }, cells)))};
  }

  void encode_sbus(const Fixed<Int, 16>& ch, Fixed<Int, 25>& p) {
    Int fill = 0;
    Int last = 0;
    Int k = 0;
    fill = 0;
    last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(p))));
    for (auto it10 = flow_for(n10, 0, last); it10.next();) {
      p[n10.index] = fill;
    }
    p[0] = 15;
    for (auto it11 = flow_for(n11, 0, 15); it11.next();) {
      for (auto it12 = flow_for(n12, 0, 10); it12.next();) {
        if (math_compare<">">(kern::formula([](Float a, Float index) -> Float { return (kern::math::band(a, kern::math::wrap32(kern::math::shl(1.0, index))) != 0.0); }, kern::to<Float>(ch[n11.index]), kern::to<Float>(n12.index)), 0.5)) {
          k = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((8.0 + (index * 11.0)) + index_2); }, kern::to<Float>(n11.index), kern::to<Float>(n12.index)));
          p[kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::trunc(kern::math::div(k, 8.0)); }, kern::to<Float>(k)))] = kern::to<Int>(kern::formula([](Float a, Float k) -> Float { return kern::math::band(kern::math::bor(a, kern::math::band(kern::math::wrap32(kern::math::shl(1.0, kern::math::mod(k, 8.0))), 255.0)), 255.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::trunc(kern::math::div(k, 8.0)); }, kern::to<Float>(k)))]), kern::to<Float>(k)));
        }
      }
    }
  }

  /// a big-endian 16-bit register pair at `at`
  void put_be16(Fixed<Int, 14>& out, Int at = 0, Float v = 0.0) {
    Int r = 0;
    Int u = 0;
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

  void Drone_reset(Drone& self) {
    Arondight45_FirmwareRuntime::FirmwareRuntime_reset(self.fw_);
    for (auto it14 = flow_for(n14, 0, 3); it14.next();) {
      self.prop_torque_[n14.index] = 0.0;
      self.motor_torque_[n14.index] = self.prop_torque_[n14.index];
      self.current_[n14.index] = self.motor_torque_[n14.index];
      self.omega_[n14.index] = self.current_[n14.index];
      self.pulses_[n14.index] = 1000;
    }
    self.soc_ = 1.0;
    self.vbatt_ = battery_ocv(1.0, self.p_.cells).Result;
    self.ibatt_ = 0.0;
    self.have_prev_ = false;
    self.nav_filtered_ = Vector{0.0, 0.0, 0.0};
    self.prev_omega_body_ = self.nav_filtered_;
    self.world_accel_ = self.prev_omega_body_;
    self.now_us_ = 0.0;
    self.sequence_ = 1;
    self.nav_elapsed_ = 0.01;
    self.sbus_elapsed_ = self.nav_elapsed_;
    self.nav_seq_ = 1;
    self.reset_flag_ = true;
    self.noise_ = struct_make<Rng>(1168318247, 0.0, false);
    self.nav_noise_ = struct_make<Rng>(1905503009, 0.0, false);
    self.heading_noise_ = struct_make<Rng>(1168232869, 0.0, false);
    self.gyro_bias_[0] = 0.08;
    self.gyro_bias_[1] = -0.05;
    self.gyro_bias_[2] = 0.04;
    self.acc_bias_[0] = 0.001;
    self.acc_bias_[1] = -0.0015;
    self.acc_bias_[2] = 0.002;
    self.parked_steps_ = 0;
    self.parked_ = false;
    self.tele_ = struct_make<DroneTelemetry>(false, true, false, false, false, 0, List<Int>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, List<Float>{});
  }

  struct Drone_newResult { Drone Result = Drone{DroneParams{0.225, 0.127, 2300.0, 0.08, 1.1e-05, 0.105, 0.012, 45.0, 45.0, 4.0, 1.5, 0.055, 180.0, 1.225, 0.35, 0.18, Vector{0.0, 0.0, 0.0}, 0.18, 0.035, 0.005, 0.2, 0.03, 0.02, 0.03, 0.1, 0.015, -0.015, -0.07, 0.028}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}, fc_RC{List<Int>{}, false, false, false, Int64{0x0}}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}, Int64{0x0}, 0, false}, List<Vector>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 1.0, 16.8, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, List<Int>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01, 0.01, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0, false}, Rng{1905503009, 0.0, false}, Rng{1168232869, 0.0, false}, List<Float>{0.08, -0.05, 0.04}, List<Float>{0.001, -0.0015, 0.002}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<Int>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, List<Float>{}}}; };
  Drone_newResult Drone_new(const DroneParams& p) {
    Drone self = Drone{DroneParams{0.225, 0.127, 2300.0, 0.08, 1.1e-05, 0.105, 0.012, 45.0, 45.0, 4.0, 1.5, 0.055, 180.0, 1.225, 0.35, 0.18, Vector{0.0, 0.0, 0.0}, 0.18, 0.035, 0.005, 0.2, 0.03, 0.02, 0.03, 0.1, 0.015, -0.015, -0.07, 0.028}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}, fc_RC{List<Int>{}, false, false, false, Int64{0x0}}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}, Int64{0x0}, 0, false}, List<Vector>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 1.0, 16.8, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, List<Int>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01, 0.01, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0, false}, Rng{1905503009, 0.0, false}, Rng{1168232869, 0.0, false}, List<Float>{0.08, -0.05, 0.04}, List<Float>{0.001, -0.0015, 0.002}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<Int>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, List<Float>{}}};
    Float a = 0.0;
    self.p_ = p;
    n13 = Arondight45_FirmwareRuntime::FirmwareRuntime_new();
    self.fw_ = n13.Result;
    a = kern::formula([](Float span) -> Float { return kern::math::div(span, (2.0 * kern::math::sqrt(2.0))); }, self.p_.span);
    self.motor_pos_[0] = vector_make(kern::formula([](Float a) -> Float { return (-a); }, a), kern::formula([](Float a) -> Float { return (-a); }, a), 0.0);
    self.motor_pos_[1] = vector_make(kern::formula([](Float a) -> Float { return (-a); }, a), a, 0.0);
    self.motor_pos_[2] = vector_make(a, a, 0.0);
    self.motor_pos_[3] = vector_make(a, kern::formula([](Float a) -> Float { return (-a); }, a), 0.0);
    self.back_emf_ = kern::formula([](Float kPi, Float kv) -> Float { return kern::math::div(60.0, ((2.0 * kPi) * kv)); }, kPi, self.p_.kv);
    self.d4_ = kern::formula([](Float prop_d) -> Float { return kern::math::pow(prop_d, 4.0); }, self.p_.prop_d);
    self.d5_ = kern::formula([](Float prop_d) -> Float { return kern::math::pow(prop_d, 5.0); }, self.p_.prop_d);
    self.cdA_ = vector_scale(Vector{0.035, 0.035, 0.07}, self.p_.drag_scale);
    Drone_reset(self);
    return {self};
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

  void Drone_imu_bytes(Drone& self, const BodyState& s, Fixed<Int, 14>& out, Float dt = 0.0) {
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

  void Drone_motor_tick(Drone& self, const BodyState& s, Fixed<Float, 4>& thrust_out, Fixed<Float, 4>& torque_out, Float dt = 0.0, Float altitude = 0.0) {
    Float ocv = 0.0;
    Float limit = 0.0;
    Float total = 0.0;
    Float command = 0.0;
    Float volts = 0.0;
    Float k = 0.0;
    Vector local_v = Vector{0.0, 0.0, 0.0};
    Float rev = 0.0;
    Float prop_torque = 0.0;
    Float motor_torque = 0.0;
    Float n = 0.0;
    Float thrust = 0.0;
    Float advance = 0.0;
    ocv = battery_ocv(self.soc_, self.p_.cells).Result;
    limit = kern::formula([](Float motor_limit_a, Float esc_limit_a) -> Float { return kern::math::min(motor_limit_a, esc_limit_a); }, self.p_.motor_limit_a, self.p_.esc_limit_a);
    total = 0.0;
    for (auto it26 = flow_for(n26, 0, 1); it26.next();) {
      total = 0.0;
      for (auto it27 = flow_for(n27, 0, 3); it27.next();) {
        command = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a) -> Float { return kern::math::div((a - 1000.0), 1000.0); }, kern::to<Float>(self.pulses_[n27.index])), 0.0, 1.0));
        volts = kern::formula([](Float command, Float index, Float vbatt_, Float ocv) -> Float { return (command * ((index != 0.0) ? vbatt_ : ocv)); }, command, kern::to<Float>(n26.index), self.vbatt_, ocv);
        self.current_[n27.index] = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float volts, Float back_emf_, Float a, Float r) -> Float { return kern::math::div((volts - (back_emf_ * a)), r); }, volts, self.back_emf_, self.omega_[n27.index], self.p_.R), 0.0, limit));
        total += self.current_[n27.index];
      }
      if (logic_and(math_compare<">">(total, self.p_.battery_max_a), math_compare<">">(total, 1e-12))) {
        k = kern::formula([](Float battery_max_a, Float total) -> Float { return kern::math::div(battery_max_a, total); }, self.p_.battery_max_a, total);
        for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(self.current_))))); it28.next();) {
          self.current_[n28.index] = kern::formula([](Float a, Float k) -> Float { return (a * k); }, self.current_[n28.index], k);
        }
        total = self.p_.battery_max_a;
      }
      self.vbatt_ = battery_under_load(ocv, total, self.p_.battery_r, self.p_.cells).Result;
    }
    self.ibatt_ = total;
    self.soc_ = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float soc_, Float total, Float dt, Float capacity_ah) -> Float { return (soc_ - kern::math::div((total * dt), (capacity_ah * 3600.0))); }, self.soc_, total, dt, self.p_.capacity_ah), 0.0, 1.0));
    local_v = common::to_local(s.rotation, s.velocity).Result;
    for (auto it29 = flow_for(n29, 0, 3); it29.next();) {
      rev = kern::formula([](Float a, Float kPi) -> Float { return kern::math::div(a, (2.0 * kPi)); }, self.omega_[n29.index], kPi);
      prop_torque = kern::formula([](Float cq, Float rho, Float rev, Float d5_) -> Float { return ((((cq * rho) * rev) * rev) * d5_); }, self.p_.Cq, self.p_.rho, rev, self.d5_);
      motor_torque = kern::formula([](Float back_emf_, Float a) -> Float { return (back_emf_ * a); }, self.back_emf_, self.current_[n29.index]);
      self.motor_torque_[n29.index] = motor_torque;
      self.prop_torque_[n29.index] = prop_torque;
      self.omega_[n29.index] = kern::formula([](Float a, Float motor_torque, Float prop_torque, Float kBearingDrag, Float dt, Float j) -> Float { return kern::math::max(0.0, (a + kern::math::div((((motor_torque - prop_torque) - (kBearingDrag * a)) * dt), j))); }, self.omega_[n29.index], motor_torque, prop_torque, kBearingDrag, dt, self.p_.J);
      n = kern::formula([](Float a, Float kPi) -> Float { return kern::math::div(a, (2.0 * kPi)); }, self.omega_[n29.index], kPi);
      thrust = kern::formula([](Float ct, Float rho, Float n, Float d4_) -> Float { return ((((ct * rho) * n) * n) * d4_); }, self.p_.Ct, self.p_.rho, n, self.d4_);
      advance = kern::formula([](Float z, Float n, Float prop_d) -> Float { return kern::math::div(z, kern::math::max(1.0, (n * prop_d))); }, vector_break(local_v).z, n, self.p_.prop_d);
      thrust = kern::formula([](Float thrust, Float a) -> Float { return (thrust * a); }, thrust, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float advance) -> Float { return (1.0 - (0.12 * advance)); }, advance), 0.55, 1.25));
      thrust = kern::formula([](Float thrust, Float ground_effect, Float altitude, Float prop_d) -> Float { return (thrust * (1.0 + (ground_effect * kern::math::exp(kern::math::div((-altitude), kern::math::max(0.02, (0.75 * prop_d))))))); }, thrust, self.p_.ground_effect, altitude, self.p_.prop_d);
      thrust_out[n29.index] = kern::formula([](Float a, Float thrust) -> Float { return (a + thrust); }, thrust_out[n29.index], thrust);
      torque_out[n29.index] = kern::formula([](Float a, Float a_2, Float motor_torque) -> Float { return (a + (a_2 * motor_torque)); }, torque_out[n29.index], list_get<Float>(kMotorYawSign, n29.index), motor_torque);
      self.rotor_angle_[n29.index] = kern::formula([](Float a, Float index, Float a_2, Float dt, Float kPi) -> Float { return kern::math::mod((a + ((((kern::math::mod(index, 2.0) != 0.0) ? (-1.0) : 1.0) * a_2) * dt)), (2.0 * kPi)); }, self.rotor_angle_[n29.index], kern::to<Float>(n29.index), self.omega_[n29.index], dt, kPi);
    }
  }

  struct step_centeredResult { Int Result = 0; };
  KERN_PURE step_centeredResult step_centered(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::band(kern::math::round((992.0 + (820.0 * (v < (-1.0) ? (-1.0) : (1.0 < v ? 1.0 : v))))), 65535.0); }, v))};
  }

  void Drone_step(Drone& self, const DroneSticks& sticks, Int world = 0, physics_Body body = physics_Body{Int64{0x0}}, Float h = 0.0) {
    Int world_ = 0;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector down = Vector{0.0, 0.0, 0.0};
    Float vertical = 0.0;
    the_RayResult_ range = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Vector o = Vector{0.0, 0.0, 0.0};
    Float travelled = 0.0;
    Int k = 0;
    Bool stop = false;
    physics_RayHit h_2 = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    physics_RayHit cast_ray = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    Float d = 0.0;
    physics_BodyType body_type = physics_BodyType::fixed;
    physics_BodyType body_type_2 = physics_BodyType::fixed;
    Bool either = false;
    Bool agl_valid = false;
    Float true_agl = 0.0;
    the_RayResult_ below = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Float altitude = 0.0;
    Fixed<Int, 16> ch = Fixed<Int, 16>{};
    Float clearance = 0.0;
    Int ticks = 0;
    Float dt = 0.0;
    Fixed<Float, 4> thrust = Fixed<Float, 4>{};
    Fixed<Float, 4> yaw_torque = Fixed<Float, 4>{};
    fc_RuntimeOutput out = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false};
    fc_HardwareFrame hw = fc_HardwareFrame{Int64{0x0}, 1000, 0, List<Int>{}, List<Int>{}, hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}, false, false, false};
    Float measured = 0.0;
    Float at = 0.0;
    Float put = 0.0;
    Float agl = 0.0;
    Float heading = 0.0;
    Int was = 0;
    Bool commanded = false;
    Float inv = 0.0;
    Vector rel = Vector{0.0, 0.0, 0.0};
    Float hs = 0.0;
    Vector drag = Vector{0.0, 0.0, 0.0};
    Vector w = Vector{0.0, 0.0, 0.0};
    world_ = world;
    s = common::read_body(world_, body).Result;
    if (logic_and(self.have_prev_, math_compare<">">(h, 0.0))) {
      self.world_accel_ = vector_scale(vector_sub(s.velocity, self.prev_velocity_), kern::formula([](Float h) -> Float { return kern::math::div(1.0, h); }, h));
    }
    self.prev_velocity_ = s.velocity;
    self.have_prev_ = true;
    down = engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, -1.0}).Result;
    vertical = kern::formula([](Float z) -> Float { return (-z); }, vector_break(down).z);
    range = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    if (math_compare<">">(vertical, 0.55)) {
      o = vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, -0.018}).Result);
      travelled = 0.0;
      k = 0;
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(k), 6.0)))) {
        cast_ray = engine_world_cast_ray(world_, o, vector_scale(down, kern::formula([](Float kNavRayMax, Float travelled) -> Float { return (kNavRayMax - travelled); }, kNavRayMax, travelled)), engine_physics_physics::QueryFilter_new(Int64{0x1}, Int64{0xffffffffffffffff}).Result, body);
        h_2 = cast_ray;
        if (logic_not(h_2.hit)) {
          stop = true;
        }
        if (logic_not(stop)) {
          d = kern::formula([](Float fraction, Float kNavRayMax, Float travelled) -> Float { return (fraction * (kNavRayMax - travelled)); }, h_2.fraction, kNavRayMax, travelled);
          either = logic_not(engine_physics_physics::Body_valid(h_2.body).Result);
          if (logic_not(either)) {
            body_type_2 = engine_world_body_type(world_, h_2.body);
            either = enum_equal<physics_BodyType>(body_type_2, physics_BodyType::fixed);
          }
          if (either) {
            range.hit = true;
            range.distance = kern::formula([](Float travelled, Float d) -> Float { return (travelled + d); }, travelled, d);
            range.point = h_2.point;
            range.normal = h_2.normal;
            stop = true;
          }
          if (logic_not(stop)) {
            travelled += kern::formula([](Float d) -> Float { return (d + 0.02); }, d);
            o = vector_add(h_2.point, vector_scale(down, 0.02));
            if (math_compare<">=">(travelled, kNavRayMax)) {
              stop = true;
            }
          }
        }
        if (logic_not(stop)) {
          k += 1;
        }
      }
    }
    agl_valid = logic_and(range.hit, math_compare<">=">(range.distance, 0.001));
    true_agl = kern::formula([](Float agl_valid, Float distance, Float vertical) -> Float { return ((agl_valid != 0.0) ? (distance * vertical) : 999.0); }, kern::to<Float>(agl_valid), range.distance, vertical);
    below = common::cast(world_, s.position, Vector{0.0, 0.0, -1.0}, 50.0, body, Int64{0xffffffffffffffff}).Result;
    altitude = kern::formula([](Float hit, Float distance) -> Float { return kern::math::max(0.001, ((hit != 0.0) ? distance : 50.0)); }, kern::to<Float>(below.hit), below.distance);
    ch = List<Int>{};
    for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(ch))))); it30.next();) {
      ch[n30.index] = 992;
    }
    ch[0] = step_centered(sticks.roll).Result;
    ch[1] = step_centered(sticks.pitch).Result;
    ch[3] = step_centered(sticks.yaw).Result;
    ch[4] = kern::to<Int>(kern::formula([](Float arm, Float kill) -> Float { return kern::math::band((((arm != 0.0) && (!(kill != 0.0))) ? 1811.0 : 172.0), 65535.0); }, kern::to<Float>(sticks.arm), kern::to<Float>(sticks.kill)));
    if (sticks.game_mode) {
      ch[2] = 172;
      clearance = kern::formula([](Float clearance, Float kMinClearance, Float kMaxClearance) -> Float { return (clearance < kMinClearance ? kMinClearance : (kMaxClearance < clearance ? kMaxClearance : clearance)); }, sticks.clearance, kMinClearance, kMaxClearance);
      ch[5] = kern::to<Int>(kern::formula([](Float clearance, Float kMinClearance, Float kMaxClearance) -> Float { return kern::math::band(kern::math::round((172.0 + kern::math::div((1639.0 * (clearance - kMinClearance)), (kMaxClearance - kMinClearance)))), 65535.0); }, clearance, kMinClearance, kMaxClearance));
      ch[6] = 1811;
      ch[7] = step_centered(sticks.body_pitch).Result;
    } else {
      ch[2] = kern::to<Int>(kern::formula([](Float throttle) -> Float { return kern::math::band(kern::math::round((172.0 + (1639.0 * (throttle < 0.0 ? 0.0 : (1.0 < throttle ? 1.0 : throttle))))), 65535.0); }, sticks.throttle));
    }
    ticks = kern::to<Int>(kern::formula([](Float h) -> Float { return kern::math::max(1.0, kern::math::wrap32s(kern::math::round((h * 1000.0)))); }, h));
    dt = kern::formula([](Float h, Float ticks) -> Float { return kern::math::div(h, ticks); }, h, kern::to<Float>(ticks));
    thrust = List<Float>{};
    yaw_torque = List<Float>{};
    out = struct_make<fc_RuntimeOutput>(List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false);
    for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(ticks)))); it31.next();) {
      hw = struct_make<fc_HardwareFrame>(Int64{0x0}, 1000, 0, List<Int>{}, List<Int>{}, hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}, false, false, false);
      self.now_us_ = kern::formula([](Float now_us_, Float dt) -> Float { return (now_us_ + (dt * 1000000.0)); }, self.now_us_, dt);
      hw.now_us = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, self.now_us_));
      hw.dt_us = kern::to<Int>(kern::formula([](Float dt) -> Float { return kern::math::wrap32(kern::math::round((dt * 1000000.0))); }, dt));
      Drone_imu_bytes(self, s, hw.imu_registers, dt);
      hw.imu_present = true;
      self.sbus_elapsed_ = kern::formula([](Float sbus_elapsed_, Float dt) -> Float { return (sbus_elapsed_ + dt); }, self.sbus_elapsed_, dt);
      if (math_compare<">=">(self.sbus_elapsed_, 0.01)) {
        self.sbus_elapsed_ = kern::formula([](Float sbus_elapsed_) -> Float { return (sbus_elapsed_ - 0.01); }, self.sbus_elapsed_);
        encode_sbus(ch, hw.sbus_frame);
        hw.sbus_present = true;
      }
      self.nav_elapsed_ = kern::formula([](Float nav_elapsed_, Float dt) -> Float { return (nav_elapsed_ + dt); }, self.nav_elapsed_, dt);
      if (math_compare<">=">(self.nav_elapsed_, 0.01)) {
        self.nav_elapsed_ = kern::formula([](Float nav_elapsed_) -> Float { return (nav_elapsed_ - 0.01); }, self.nav_elapsed_);
        for (auto it32 = flow_for(n32, 0, 2); it32.next();) {
          n33 = common::Rng_gaussian(self.nav_noise_);
          measured = kern::formula([](Float a, Float the_result) -> Float { return (a + (the_result * 0.025)); }, kern::formula([](Float i, Float x, Float y, Float z) -> Float { return i == 0.0 ? x : (i == 1.0 ? y : z); }, kern::to<Float>(n32.index), vector_break(s.velocity).x, vector_break(s.velocity).y, vector_break(s.velocity).z), n33.Result);
          at = kern::to<Float>(n32.index);
          put = kern::formula([](Float a, Float measured) -> Float { return (a + (0.42 * (measured - a))); }, kern::formula([](Float i, Float x, Float y, Float z) -> Float { return i == 0.0 ? x : (i == 1.0 ? y : z); }, kern::to<Float>(n32.index), vector_break(self.nav_filtered_).x, vector_break(self.nav_filtered_).y, vector_break(self.nav_filtered_).z), measured);
          self.nav_filtered_ = vector_make(kern::formula([](Float i, Float v, Float c) -> Float { return i == 0.0 ? v : c; }, at, put, vector_break(self.nav_filtered_).x), kern::formula([](Float i, Float v, Float c) -> Float { return i == 1.0 ? v : c; }, at, put, vector_break(self.nav_filtered_).y), kern::formula([](Float i, Float v, Float c) -> Float { return i != 0.0 && i != 1.0 ? v : c; }, at, put, vector_break(self.nav_filtered_).z));
        }
        agl = 0.0;
        if (agl_valid) {
          n34 = common::Rng_gaussian(self.nav_noise_);
          agl = kern::formula([](Float distance, Float the_result, Float vertical) -> Float { return (kern::math::max(0.0, (distance + (the_result * 0.004))) * vertical); }, range.distance, n34.Result, vertical);
        }
        n35 = common::Rng_gaussian(self.heading_noise_);
        heading = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result + (the_result_2 * 0.12)); }, quat_yaw_deg(s.rotation).Result, n35.Result);
        was = self.nav_seq_;
        self.nav_seq_ = kern::to<Int>(kern::formula([](Float nav_seq_) -> Float { return kern::math::band((nav_seq_ + 1.0), 65535.0); }, kern::to<Float>(self.nav_seq_)));
        hw.navigation_frame = Arondight45_HardwareSensors::encode_navigation_wire(was, vector_break(self.nav_filtered_).x, vector_break(self.nav_filtered_).y, vector_break(self.nav_filtered_).z, agl, true, agl_valid, heading, true).Result;
        hw.navigation_present = true;
      }
      if (self.reset_flag_) {
        Arondight45_FirmwareRuntime::FirmwareRuntime_reset(self.fw_);
        self.reset_flag_ = false;
      }
      n36 = Arondight45_FirmwareRuntime::FirmwareRuntime_step(self.fw_, hw);
      out = n36.Result;
      for (auto it37 = flow_for(n37, 0, 3); it37.next();) {
        self.pulses_[n37.index] = kern::to<Int>(kern::formula([](Float kill, Float a) -> Float { return kern::math::band(((kill != 0.0) ? 1000.0 : a), 65535.0); }, kern::to<Float>(sticks.kill), kern::to<Float>(out.motor_us[n37.index])));
      }
      Drone_motor_tick(self, s, thrust, yaw_torque, dt, altitude);
    }
    if (logic_and(logic_and(logic_and(math_compare<"!=">(kern::to<Float>(enum_index<fc_FaultCode>(out.fault)), kern::to<Float>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone))), logic_not(out.armed)), math_compare<"<">(vector_length(s.velocity), 0.05)), math_compare<"<">(vector_length(s.angular), 0.05))) {
      self.reset_flag_ = true;
    }
    commanded = false;
    for (auto it38 = flow_for(n38, 0, 3); it38.next();) {
      commanded = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float commanded, Float a, Float a_2) -> Float { return kern::math::bor(commanded, ((a > 1000.0) || (a_2 > 0.001))); }, kern::to<Float>(commanded), kern::to<Float>(self.pulses_[n38.index]), self.omega_[n38.index])))), 0.0);
    }
    if (commanded) {
      self.parked_steps_ = 0;
      self.parked_ = false;
    } else if (logic_and(math_compare<"<">(vector_length(s.velocity), 0.002), math_compare<"<">(vector_length(s.angular), 0.002))) {
      self.parked_steps_ = kern::to<Int>(kern::formula([](Float parked_steps_) -> Float { return kern::math::wrap32(parked_steps_ + 1.0); }, kern::to<Float>(self.parked_steps_)));
      self.parked_ = math_compare<">">(kern::to<Float>(self.parked_steps_), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float h) -> Float { return kern::math::trunc(kern::math::div(0.5, h)); }, h)))));
    } else {
      self.parked_steps_ = 0;
      self.parked_ = false;
    }
    inv = kern::formula([](Float ticks) -> Float { return kern::math::div(1.0, ticks); }, kern::to<Float>(ticks));
    if (commanded) {
      for (auto it39 = flow_for(n39, 0, 3); it39.next();) {
        engine_world_apply_force(world_, body, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kern::formula([](Float a, Float inv) -> Float { return (a * inv); }, thrust[n39.index], inv))).Result, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, self.motor_pos_[n39.index]).Result));
        engine_world_apply_torque(world_, body, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kern::formula([](Float a, Float inv) -> Float { return (a * inv); }, yaw_torque[n39.index], inv))).Result);
      }
    }
    rel = common::to_local(s.rotation, vector_sub(s.velocity, self.p_.wind)).Result;
    hs = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(rel).x, vector_break(rel).y);
    drag = vector_make(kern::formula([](Float rho, Float x, Float x_2, Float hs) -> Float { return (((((-0.5) * rho) * x) * x_2) * hs); }, self.p_.rho, vector_break(self.cdA_).x, vector_break(rel).x, hs), kern::formula([](Float rho, Float y, Float y_2, Float hs) -> Float { return (((((-0.5) * rho) * y) * y_2) * hs); }, self.p_.rho, vector_break(self.cdA_).y, vector_break(rel).y, hs), kern::formula([](Float rho, Float z, Float z_2) -> Float { return (((((-0.5) * rho) * z) * z_2) * kern::math::abs(z_2)); }, self.p_.rho, vector_break(self.cdA_).z, vector_break(rel).z));
    engine_world_apply_force_to_center(world_, body, engine_math_math::Quat_rotate(s.rotation, drag).Result);
    w = common::to_local(s.rotation, s.angular).Result;
    engine_world_apply_torque(world_, body, engine_math_math::Quat_rotate(s.rotation, vector_make(kern::formula([](Float x) -> Float { return (((-0.0012) * x) * kern::math::abs(x)); }, vector_break(w).x), kern::formula([](Float y) -> Float { return (((-0.0012) * y) * kern::math::abs(y)); }, vector_break(w).y), kern::formula([](Float z) -> Float { return (((-0.0012) * z) * kern::math::abs(z)); }, vector_break(w).z))).Result);
    self.tele_.state = out.state;
    self.tele_.armed = out.armed;
    self.tele_.calibrating = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::band(state, a); }, kern::to<Float>(out.state), kern::to<Float>(list_get<Int>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateCalibrating))))))), 0.0);
    self.tele_.fault = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::band(state, a); }, kern::to<Float>(out.state), kern::to<Float>(list_get<Int>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateFault))))))), 0.0);
    self.tele_.game = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float state, Float kStateGameMode) -> Float { return kern::math::band(state, kStateGameMode); }, kern::to<Float>(out.state), kern::to<Float>(kStateGameMode))))), 0.0);
    self.tele_.nav_valid = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float state, Float kStateNavigationValid) -> Float { return kern::math::band(state, kStateNavigationValid); }, kern::to<Float>(out.state), kern::to<Float>(kStateNavigationValid))))), 0.0);
    for (auto it40 = flow_for(n40, 0, 3); it40.next();) {
      self.tele_.pulses[n40.index] = self.pulses_[n40.index];
      self.tele_.omega[n40.index] = self.omega_[n40.index];
      self.tele_.motor_torque[n40.index] = self.motor_torque_[n40.index];
      self.tele_.prop_torque[n40.index] = self.prop_torque_[n40.index];
    }
    self.tele_.battery_v = self.vbatt_;
    self.tele_.battery_soc = self.soc_;
    self.tele_.current_a = self.ibatt_;
    self.tele_.agl = kern::formula([](Float agl_valid, Float true_agl) -> Float { return ((agl_valid != 0.0) ? true_agl : (-1.0)); }, kern::to<Float>(agl_valid), true_agl);
    self.tele_.speed = vector_length(s.velocity);
    self.tele_.vz = vector_break(s.velocity).z;
    for (auto it41 = flow_for(n41, 0, 2); it41.next();) {
      self.tele_.attitude_deg[n41.index] = kern::formula([](Float a) -> Float { return (a * 0.01); }, kern::to<Float>(out.attitude_cdeg[n41.index]));
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  Arondight45_FirmwareRuntime::FirmwareRuntime_newResult n13;
  struct : kern::Node<14> { Int index = 0; } n14;
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
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  common::Rng_gaussianResult n33;
  common::Rng_gaussianResult n34;
  common::Rng_gaussianResult n35;
  Arondight45_FirmwareRuntime::FirmwareRuntime_stepResult n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct : kern::Node<40> { Int index = 0; } n40;
  struct : kern::Node<41> { Int index = 0; } n41;
};
