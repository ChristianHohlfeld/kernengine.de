struct drone_cpp : virtual kern::Library, virtual Arondight45_FirmwareRuntime, virtual Arondight45_HardwareSensors, virtual common, virtual drone, virtual engine_physics_physics, virtual rigid_spec {
  // ---- types ----
  KERN_STRUCT(MassPart) { f32 m = 0.0f; Vector pos = Vector{0.0, 0.0, 0.0}; Vector size = Vector{0.0, 0.0, 0.0}; bool disk = false; };

  // ---- variables ----
  KERN_CONST f32 kDeg = 0.017453292f;
  KERN_CONST f32 kPi = 3.1415927f;
  KERN_CONST f32 kG = 9.80665f;
  /// propulsion_authority.mjs MOTOR_BEARING_DRAG_NM_PER_RAD_S
  KERN_CONST f32 kBearingDrag = 1.5e-07f;
  /// simulator.mjs MOTOR_YAW_SIGN
  KERN_CONST Fixed<f32, 4> kMotorYawSign = List<f32>{-1.0f, 1.0f, -1.0f, 1.0f};
  /// ceil(120 / cos^2(40 deg) + 10), control_semantics.mjs
  KERN_CONST f32 kNavRayMax = 215.0f;
  KERN_CONST f32 kMinClearance = 0.5f;
  KERN_CONST f32 kMaxClearance = 120.0f;
  /// The number of each value of fc_StateBits, in their order
  KERN_CONST List<i64> fc_StateBits_numbers = List<i64>{1, 2, 4, 8, 16};
  KERN_CONST u16 kStateGameMode = 64;
  KERN_CONST u16 kStateNavigationValid = 32;

  // ---- functions ----
  struct battery_ocvResult { f32 Result = 0.0f; };
  KERN_PURE battery_ocvResult battery_ocv(f32 soc = 0.0f, f32 cells = 0.0f) {
    f32 s = 0.0f;
    s = common::clampf(soc, 0.0f, 1.0f).Result;
    return {kern::formula<f32, f32>([](auto cells, auto s) { return (cells * fmin(4.2f, ((3.2625f + (0.9f * s)) + (0.0375f * tanh(((s - 0.12f) * 18.0f)))))); }, cells, s)};
  }

  struct battery_under_loadResult { f32 Result = 0.0f; };
  KERN_PURE battery_under_loadResult battery_under_load(f32 ocv = 0.0f, f32 amps = 0.0f, f32 r = 0.0f, f32 cells = 0.0f) {
    return {common::clampf(kern::formula<f32, f32, f32>([](auto ocv, auto amps, auto r) { return (ocv - (fmax(0.0f, amps) * r)); }, ocv, amps, r), kern::formula<f32>([](auto cells) { return (3.0f * cells); }, cells), kern::formula<f32>([](auto cells) { return (4.2f * cells); }, cells)).Result};
  }

  void encode_sbus(const Fixed<u16, 16>& ch, Fixed<u8, 25>& p) {
    u8 fill = 0;
    i64 last = 0;
    i32 k = 0;
    fill = 0;
    last = math_sub<i64>(list_length<u8>(p), 1);
    for (auto it10 = flow_for(n10, 0, last); it10.next();) {
      p[n10.index] = fill;
    }
    p[0] = 15;
    for (auto it11 = flow_for(n11, 0, 15); it11.next();) {
      for (auto it12 = flow_for(n12, 0, 10); it12.next();) {
        if (kern::formula<u16, u32, i64>([](auto a, auto a1, auto index) { return num_cast<bool, u32>((num_cast<u32, u16>(a) & (a1 << num_cast<i32, i64>(index)))); }, ch[num_cast<i64, i32>(num_cast<i32, i64>(n11.index))], 1, n12.index)) {
          k = kern::formula<i64, i64>([](auto index, auto index_2) { return ((8 + (num_cast<i32, i64>(index) * 11)) + num_cast<i32, i64>(index_2)); }, n11.index, n12.index);
          p[num_cast<i64, u64>(kern::formula<i32>([](auto k) { return num_cast<u64, i32>((k / 8)); }, k))] = num_cast<u8, i32>(kern::formula<u8, u32, i32>([](auto a, auto a1, auto k) { return (num_cast<i32, u8>(a) | num_cast<i32, u8>(num_cast<u8, u32>((a1 << (k % 8))))); }, p[num_cast<i64, u64>(kern::formula<i32>([](auto k) { return num_cast<u64, i32>((k / 8)); }, k))], 1, k));
        }
      }
    }
  }

  /// a big-endian 16-bit register pair at `at`
  void put_be16(Fixed<u8, 14>& out, u32 at = 0, f32 v = 0.0f) {
    i64 r = 0;
    u16 u = 0;
    r = kern::formula<f32>([](auto the_result) { return num_cast<i64, f32>(round(the_result)); }, common::clampf(v, -32767.0f, 32767.0f).Result);
    u = num_cast<u16, i16>(num_cast<i16, i64>(r));
    out[num_cast<i64, u32>(at)] = kern::formula<u16>([](auto u) { return num_cast<u8, i32>((num_cast<i32, u16>(u) >> 8)); }, u);
    out[num_cast<i64, u64>(kern::formula<u32>([](auto at) { return num_cast<u64, u32>((at + 1)); }, at))] = kern::formula<u16>([](auto u) { return num_cast<u8, i32>((num_cast<i32, u16>(u) & 255)); }, u);
  }

  struct quat_yaw_degResult { f32 Result = 0.0f; };
  KERN_PURE quat_yaw_degResult quat_yaw_deg(Quat q = Quat{0.0f, 0.0f, 0.0f, 1.0f}) {
    return {kern::formula<f32, f32, f32, f32, f32>([](auto w, auto z, auto x, auto y, auto kDeg) { return (atan2((2.0f * ((w * z) + (x * y))), (1.0f - (2.0f * ((y * y) + (z * z))))) / kDeg); }, q.w, q.z, q.x, q.y, kDeg)};
  }

  struct clearance_rateResult { f32 Result = 0.0f; };
  KERN_PURE clearance_rateResult clearance_rate(f32 axis = 0.0f) {
    f32 x = 0.0f;
    f32 m = 0.0f;
    f32 unit = 0.0f;
    x = common::clampf(axis, -1.0f, 1.0f).Result;
    m = kern::formula<f32>([](auto x) { return abs(x); }, x);
    if (math_compare<"<=", f32>(m, 0.08f)) {
      return {0.0f};
    }
    unit = kern::formula<f32>([](auto m) { return ((m - 0.08f) / 0.92f); }, m);
    return {kern::formula<f32, f32>([](auto the_result, auto unit) { return (((the_result * 12.0f) * unit) * (0.25f + (0.75f * unit))); }, common::sign0(x).Result, unit)};
  }

  struct clearance_leadResult { f32 Result = 0.0f; };
  KERN_PURE clearance_leadResult clearance_lead(f32 v = 0.0f) {
    bool up = false;
    f32 a = 0.0f;
    f32 tau = 0.0f;
    f32 k = 0.0f;
    f32 s = 0.0f;
    f32 e = 0.0f;
    up = math_compare<">=", f32>(v, 0.0f);
    a = kern::formula<bool, f32, f32>([](auto up, auto a0f, auto a0f_2) { return (up ? a0f : a0f_2); }, up, 6.0f, 4.0f);
    tau = kern::formula<bool, f32, f32>([](auto up, auto a25f, auto a125f) { return (up ? a25f : a125f); }, up, 0.25f, 0.125f);
    k = 2.0f;
    s = kern::formula<f32>([](auto v) { return abs(v); }, v);
    e = kern::formula<f32, f32, f32, f32>([](auto s, auto k, auto a, auto tau) { return fmax((s / k), (((s * s) + (((2.0f * a) * tau) * s)) / (2.0f * a))); }, s, k, a, tau);
    return {kern::formula<bool, f32>([](auto up, auto e) { return (up ? e : (-e)); }, up, e)};
  }

  void Drone_reset(Drone& self) {
    Arondight45_FirmwareRuntime::FirmwareRuntime_reset(self.fw_);
    for (auto it14 = flow_for(n14, 0, 3); it14.next();) {
      self.prop_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))] = 0.0f;
      self.motor_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))] = self.prop_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))];
      self.current_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))] = self.motor_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))];
      self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))] = self.current_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))];
      self.pulses_[num_cast<i64, i32>(num_cast<i32, i64>(n14.index))] = 1000;
    }
    self.soc_ = 1.0f;
    self.vbatt_ = battery_ocv(1.0f, self.p_.cells).Result;
    self.ibatt_ = 0.0f;
    self.have_prev_ = false;
    self.nav_filtered_ = Vector{0.0, 0.0, 0.0};
    self.prev_omega_body_ = self.nav_filtered_;
    self.world_accel_ = self.prev_omega_body_;
    self.now_us_ = 0.0;
    self.sequence_ = 1;
    self.nav_elapsed_ = 0.01f;
    self.sbus_elapsed_ = self.nav_elapsed_;
    self.nav_seq_ = 1;
    self.reset_flag_ = true;
    self.noise_ = struct_make<Rng>(1168318247, 0.0f, false);
    self.nav_noise_ = struct_make<Rng>(1905503009, 0.0f, false);
    self.heading_noise_ = struct_make<Rng>(1168232869, 0.0f, false);
    self.gyro_bias_[0] = 0.08f;
    self.gyro_bias_[1] = -0.05f;
    self.gyro_bias_[2] = 0.04f;
    self.acc_bias_[0] = 0.001f;
    self.acc_bias_[1] = -0.0015f;
    self.acc_bias_[2] = 0.002f;
    self.parked_steps_ = 0;
    self.parked_ = false;
    self.tele_ = struct_make<DroneTelemetry>(false, true, false, false, false, 0, List<u16>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, List<f32>{});
  }

  struct Drone_newResult { Drone Result = Drone{DroneParams{0.225f, 0.127f, 2300.0f, 0.08f, 1.1e-05f, 0.105f, 0.012f, 45.0f, 45.0f, 4.0f, 1.5f, 0.055f, 180.0f, 1.225f, 0.35f, 0.18f, Vector{0.0, 0.0, 0.0}, 0.18f, 0.035f, 0.005f, 0.2f, 0.03f, 0.02f, 0.03f, 0.1f, 0.015f, -0.015f, -0.07f, 0.028f}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}, fc_RC{List<u16>{}, false, false, false, 0}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}, 0, 0, false}, List<Vector>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 1.0f, 16.8f, 0.0f, 0.0f, 0.0f, 0.0f, Vector{0.0, 0.0, 0.0}, List<u16>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01f, 0.01f, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0f, false}, Rng{1905503009, 0.0f, false}, Rng{1168232869, 0.0f, false}, List<f32>{0.08f, -0.05f, 0.04f}, List<f32>{0.001f, -0.0015f, 0.002f}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<u16>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, List<f32>{}}}; };
  Drone_newResult Drone_new(const DroneParams& p) {
    Drone self = Drone{DroneParams{0.225f, 0.127f, 2300.0f, 0.08f, 1.1e-05f, 0.105f, 0.012f, 45.0f, 45.0f, 4.0f, 1.5f, 0.055f, 180.0f, 1.225f, 0.35f, 0.18f, Vector{0.0, 0.0, 0.0}, 0.18f, 0.035f, 0.005f, 0.2f, 0.03f, 0.02f, 0.03f, 0.1f, 0.015f, -0.015f, -0.07f, 0.028f}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}, fc_RC{List<u16>{}, false, false, false, 0}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}, 0, 0, false}, List<Vector>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 1.0f, 16.8f, 0.0f, 0.0f, 0.0f, 0.0f, Vector{0.0, 0.0, 0.0}, List<u16>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01f, 0.01f, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0f, false}, Rng{1905503009, 0.0f, false}, Rng{1168232869, 0.0f, false}, List<f32>{0.08f, -0.05f, 0.04f}, List<f32>{0.001f, -0.0015f, 0.002f}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<u16>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, List<f32>{}}};
    f32 a = 0.0f;
    self.p_ = p;
    n13 = Arondight45_FirmwareRuntime::FirmwareRuntime_new();
    self.fw_ = n13.Result;
    a = kern::formula<f32>([](auto span) { return (span / 2.828427f); }, self.p_.span);
    self.motor_pos_[0] = vector_make(kern::formula<f32>([](auto a) { return (-a); }, a), kern::formula<f32>([](auto a) { return (-a); }, a), 0.0f);
    self.motor_pos_[1] = vector_make(kern::formula<f32>([](auto a) { return (-a); }, a), a, 0.0f);
    self.motor_pos_[2] = vector_make(a, a, 0.0f);
    self.motor_pos_[3] = vector_make(a, kern::formula<f32>([](auto a) { return (-a); }, a), 0.0f);
    self.back_emf_ = kern::formula<f32, f32>([](auto kPi, auto kv) { return (60.0f / ((2.0f * kPi) * kv)); }, kPi, self.p_.kv);
    self.d4_ = kern::formula<f32>([](auto prop_d) { return pow(prop_d, 4.0f); }, self.p_.prop_d);
    self.d5_ = kern::formula<f32>([](auto prop_d) { return pow(prop_d, 5.0f); }, self.p_.prop_d);
    self.cdA_ = vector_scale(Vector{0.035, 0.035, 0.07}, self.p_.drag_scale);
    Drone_reset(self);
    return {self};
  }

  struct Drone_rigid_specResult { RigidSpec Result = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0f, Vector{0.0, 0.0, 0.0}, List<f32>{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, 0.0f, false}; };
  KERN_PURE Drone_rigid_specResult Drone_rigid_spec(const DroneParams& p) {
    RigidSpec r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0f, Vector{0.0, 0.0, 0.0}, List<f32>{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, 0.0f, false};
    f32 a = 0.0f;
    Fixed<Vector, 4> motors = Fixed<Vector, 4>{};
    ColliderSpec box = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
    ColliderSpec arm = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
    ColliderSpec bell = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
    ColliderSpec sweep = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
    List<MassPart> parts = List<MassPart>{};
    f32 mass = 0.0f;
    Vector c = Vector{0.0, 0.0, 0.0};
    Fixed<f32, 6> I = Fixed<f32, 6>{};
    Fixed<f32, 3> l = Fixed<f32, 3>{};
    f32 rr = 0.0f;
    f32 t = 0.0f;
    Vector o = Vector{0.0, 0.0, 0.0};
    r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0f, Vector{0.0, 0.0, 0.0}, List<f32>{1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, 0.0f, false};
    a = kern::formula<f32>([](auto span) { return (span / 2.828427f); }, p.span);
    motors = list_make<Vector, "4">(vector_make(kern::formula<f32>([](auto a) { return (-a); }, a), kern::formula<f32>([](auto a) { return (-a); }, a), 0.0f), vector_make(kern::formula<f32>([](auto a) { return (-a); }, a), a, 0.0f), vector_make(a, a, 0.0f), vector_make(a, kern::formula<f32>([](auto a) { return (-a); }, a), 0.0f));
    box = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
    box.kind = physics_ShapeKind::box;
    box.size = Vector{0.11, 0.09, 0.044};
    box.friction = 0.65f;
    box.restitution = 0.08f;
    list_add(r.colliders, box);
    for (auto it15 = flow_for(n15, 0, math_sub<i64>(list_length<Vector>(motors), 1)); it15.next();) {
      arm = rigid_spec::capsule_between(Vector{0.0, 0.0, 0.0}, motors[n15.index], 0.008f).Result;
      arm.friction = 0.65f;
      arm.restitution = 0.08f;
      list_add(r.colliders, arm);
      bell = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
      bell.kind = physics_ShapeKind::sphere;
      bell.center = motors[n15.index];
      bell.radius = 0.018f;
      bell.friction = 0.65f;
      bell.restitution = 0.08f;
      list_add(r.colliders, bell);
      sweep = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, 0.6f, 0.0f};
      sweep.kind = physics_ShapeKind::box;
      sweep.center = vector_add(motors[n15.index], Vector{0.0, 0.0, 0.014});
      sweep.size = vector_make(kern::formula<f32>([](auto prop_d) { return (prop_d * 0.9f); }, p.prop_d), kern::formula<f32>([](auto prop_d) { return (prop_d * 0.9f); }, p.prop_d), 0.008f);
      sweep.friction = 0.65f;
      sweep.restitution = 0.08f;
      list_add(r.colliders, sweep);
    }
    parts = List<MassPart>{};
    list_add(parts, struct_make<MassPart>(p.m_frame, Vector{0.0, 0.0, 0.0}, vector_make(kern::formula<f32>([](auto span) { return (span * 0.62f); }, p.span), kern::formula<f32>([](auto span) { return (span * 0.48f); }, p.span), 0.018f), false));
    for (auto it16 = flow_for(n16, 0, math_sub<i64>(list_length<Vector>(motors), 1)); it16.next();) {
      list_add(parts, struct_make<MassPart>(p.m_motor, motors[n16.index], Vector{0.036, 0.036, 0.025}, false));
      list_add(parts, struct_make<MassPart>(p.m_prop, vector_make(vector_break(motors[n16.index]).x, vector_break(motors[n16.index]).y, 0.014f), vector_make(kern::formula<f32>([](auto prop_d) { return (prop_d * 0.5f); }, p.prop_d), 0.002f, 0.0f), true));
      list_add(parts, struct_make<MassPart>(kern::formula<f32>([](auto m_wiring) { return (m_wiring / 4.0f); }, p.m_wiring), vector_make(kern::formula<f32>([](auto x) { return (x / 2.0f); }, vector_break(motors[n16.index]).x), kern::formula<f32>([](auto y) { return (y / 2.0f); }, vector_break(motors[n16.index]).y), -0.002f), vector_make(kern::formula<f32>([](auto span) { return (span * 0.245f); }, p.span), 0.016f, 0.008f), false));
    }
    list_add(parts, struct_make<MassPart>(p.m_battery, vector_make(p.battery_x, 0.0f, p.battery_z), Vector{0.105, 0.035, 0.032}, false));
    list_add(parts, struct_make<MassPart>(p.m_esc, Vector{0.0, 0.0, -0.002}, Vector{0.05, 0.05, 0.008}, false));
    list_add(parts, struct_make<MassPart>(p.m_fc_rx, Vector{0.0, 0.0, 0.008}, Vector{0.04, 0.04, 0.012}, false));
    list_add(parts, struct_make<MassPart>(p.m_cam_vtx, vector_make(p.camera_x, 0.0f, p.camera_z), Vector{0.04, 0.03, 0.025}, false));
    mass = 0.0f;
    c = Vector{0.0, 0.0, 0.0};
    for (auto it17 = flow_for(n17, 0, math_sub<i64>(list_length<MassPart>(parts), 1)); it17.next();) {
      mass += parts[n17.index].m;
      c = vector_add(c, vector_scale(parts[n17.index].pos, parts[n17.index].m));
    }
    c = vector_scale(c, kern::formula<f32>([](auto mass) { return (1.0f / mass); }, mass));
    I = List<f32>{};
    for (auto it18 = flow_for(n18, 0, math_sub<i64>(list_length<MassPart>(parts), 1)); it18.next();) {
      l = List<f32>{};
      if (parts[n18.index].disk) {
        rr = vector_break(parts[n18.index].size).x;
        t = vector_break(parts[n18.index].size).y;
        l[1] = kern::formula<f32, f32, f32>([](auto a, auto rr, auto t) { return ((a * (((3.0f * rr) * rr) + (t * t))) / 12.0f); }, parts[n18.index].m, rr, t);
        l[0] = l[1];
        l[2] = kern::formula<f32, f32>([](auto a, auto rr) { return (((a * rr) * rr) / 2.0f); }, parts[n18.index].m, rr);
      } else {
        l[0] = kern::formula<f32, f32, f32>([](auto a, auto y, auto z) { return ((a * ((y * y) + (z * z))) / 12.0f); }, parts[n18.index].m, vector_break(parts[n18.index].size).y, vector_break(parts[n18.index].size).z);
        l[1] = kern::formula<f32, f32, f32>([](auto a, auto x, auto z) { return ((a * ((x * x) + (z * z))) / 12.0f); }, parts[n18.index].m, vector_break(parts[n18.index].size).x, vector_break(parts[n18.index].size).z);
        l[2] = kern::formula<f32, f32, f32>([](auto a, auto x, auto y) { return ((a * ((x * x) + (y * y))) / 12.0f); }, parts[n18.index].m, vector_break(parts[n18.index].size).x, vector_break(parts[n18.index].size).y);
      }
      o = vector_sub(parts[n18.index].pos, c);
      I[0] = kern::formula<f32, f32, f32, f32, f32>([](auto a, auto a_2, auto a_3, auto y, auto z) { return (a + (a_2 + (a_3 * ((y * y) + (z * z))))); }, I[0], l[0], parts[n18.index].m, vector_break(o).y, vector_break(o).z);
      I[1] = kern::formula<f32, f32, f32, f32, f32>([](auto a, auto a_2, auto a_3, auto x, auto z) { return (a + (a_2 + (a_3 * ((x * x) + (z * z))))); }, I[1], l[1], parts[n18.index].m, vector_break(o).x, vector_break(o).z);
      I[2] = kern::formula<f32, f32, f32, f32, f32>([](auto a, auto a_2, auto a_3, auto x, auto y) { return (a + (a_2 + (a_3 * ((x * x) + (y * y))))); }, I[2], l[2], parts[n18.index].m, vector_break(o).x, vector_break(o).y);
      I[3] = kern::formula<f32, f32, f32, f32>([](auto a, auto a_2, auto x, auto y) { return (a + (((-a_2) * x) * y)); }, I[3], parts[n18.index].m, vector_break(o).x, vector_break(o).y);
      I[4] = kern::formula<f32, f32, f32, f32>([](auto a, auto a_2, auto x, auto z) { return (a + (((-a_2) * x) * z)); }, I[4], parts[n18.index].m, vector_break(o).x, vector_break(o).z);
      I[5] = kern::formula<f32, f32, f32, f32>([](auto a, auto a_2, auto y, auto z) { return (a + (((-a_2) * y) * z)); }, I[5], parts[n18.index].m, vector_break(o).y, vector_break(o).z);
    }
    r.mass.mass = mass;
    r.mass.center = c;
    for (auto it19 = flow_for(n19, 0, 5); it19.next();) {
      r.mass.inertia[num_cast<i64, i32>(num_cast<i32, i64>(n19.index))] = I[num_cast<i64, i32>(num_cast<i32, i64>(n19.index))];
    }
    r.linear_damping = 0.002f;
    r.angular_damping = 0.002f;
    r.bullet = true;
    return {r};
  }

  void Drone_imu_bytes(Drone& self, const BodyState& s, Fixed<u8, 14>& out, f32 dt = 0.0f) {
    Vector omega_body = Vector{0.0, 0.0, 0.0};
    Vector alpha = Vector{0.0, 0.0, 0.0};
    Vector specific = Vector{0.0, 0.0, 0.0};
    Vector off = Vector{0.0, 0.0, 0.0};
    Vector accel = Vector{0.0, 0.0, 0.0};
    Vector cross = Vector{0.0, 0.0, 0.0};
    Vector cross_2 = Vector{0.0, 0.0, 0.0};
    Vector cross_3 = Vector{0.0, 0.0, 0.0};
    Vector gyro = Vector{0.0, 0.0, 0.0};
    i32 at = 0;
    f32 put = 0.0f;
    i32 at_2 = 0;
    f32 put_2 = 0.0f;
    for (auto it20 = flow_for(n20, 0, 2); it20.next();) {
      n21 = common::Rng_gaussian(self.noise_);
      self.gyro_bias_[num_cast<i64, i32>(num_cast<i32, i64>(n20.index))] = kern::formula<f32, f32, f32>([](auto a, auto the_result, auto dt) { return (a + ((the_result * 0.002f) * sqrt(dt))); }, self.gyro_bias_[num_cast<i64, i32>(num_cast<i32, i64>(n20.index))], n21.Result, dt);
      n22 = common::Rng_gaussian(self.noise_);
      self.acc_bias_[num_cast<i64, i32>(num_cast<i32, i64>(n20.index))] = kern::formula<f32, f32, f32>([](auto a, auto the_result, auto dt) { return (a + ((the_result * 2e-05f) * sqrt(dt))); }, self.acc_bias_[num_cast<i64, i32>(num_cast<i32, i64>(n20.index))], n22.Result, dt);
    }
    omega_body = common::to_local(s.rotation, s.angular).Result;
    alpha = vector_scale(vector_sub(omega_body, self.prev_omega_body_), kern::formula<f32>([](auto dt) { return (1.0f / dt); }, dt));
    self.prev_omega_body_ = omega_body;
    specific = common::to_local(s.rotation, vector_sub(self.world_accel_, vector_make(0.0f, 0.0f, kern::formula<f32>([](auto kG) { return (-kG); }, kG)))).Result;
    off = Vector{0.0, 0.0, 0.008};
    cross = engine_cross_2(alpha, off);
    cross_2 = engine_cross_2(omega_body, off);
    cross_3 = engine_cross_2(omega_body, cross_2);
    accel = vector_add(vector_add(specific, cross), cross_3);
    gyro = vector_scale(omega_body, kern::formula<f32>([](auto kDeg) { return (1.0f / kDeg); }, kDeg));
    for (auto it23 = flow_for(n23, 0, 2); it23.next();) {
      n24 = common::Rng_gaussian(self.noise_);
      at = num_cast<i32, i64>(n23.index);
      put = kern::formula<f32, f32, f32, f32>([](auto a, auto a_2, auto kG, auto the_result) { return (a + ((a_2 * kG) + ((the_result * 0.0025f) * kG))); }, kern::formula<i32, f32, f32, f32>([](auto i, auto x, auto y, auto z) { return i == 0 ? x : (i == 1 ? y : z); }, num_cast<i32, i64>(n23.index), vector_break(accel).x, vector_break(accel).y, vector_break(accel).z), self.acc_bias_[num_cast<i64, i32>(num_cast<i32, i64>(n23.index))], kG, n24.Result);
      accel = vector_make(kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i == 0 ? v : c; }, at, put, vector_break(accel).x), kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i == 1 ? v : c; }, at, put, vector_break(accel).y), kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i != 0 && i != 1 ? v : c; }, at, put, vector_break(accel).z));
      n25 = common::Rng_gaussian(self.noise_);
      at_2 = num_cast<i32, i64>(n23.index);
      put_2 = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto the_result) { return (a + (a_2 + (the_result * 0.035f))); }, kern::formula<i32, f32, f32, f32>([](auto i, auto x, auto y, auto z) { return i == 0 ? x : (i == 1 ? y : z); }, num_cast<i32, i64>(n23.index), vector_break(gyro).x, vector_break(gyro).y, vector_break(gyro).z), self.gyro_bias_[num_cast<i64, i32>(num_cast<i32, i64>(n23.index))], n25.Result);
      gyro = vector_make(kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i == 0 ? v : c; }, at_2, put_2, vector_break(gyro).x), kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i == 1 ? v : c; }, at_2, put_2, vector_break(gyro).y), kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i != 0 && i != 1 ? v : c; }, at_2, put_2, vector_break(gyro).z));
    }
    out[0] = 0;
    out[1] = 0;
    put_be16(out, 2, kern::formula<f32, f32>([](auto x, auto kG) { return ((x / kG) * 2048.0f); }, vector_break(accel).x, kG));
    put_be16(out, 4, kern::formula<f32, f32>([](auto y, auto kG) { return ((y / kG) * 2048.0f); }, vector_break(accel).y, kG));
    put_be16(out, 6, kern::formula<f32, f32>([](auto z, auto kG) { return ((z / kG) * 2048.0f); }, vector_break(accel).z, kG));
    put_be16(out, 8, kern::formula<f32>([](auto x) { return (x * 16.4f); }, vector_break(gyro).x));
    put_be16(out, 10, kern::formula<f32>([](auto y) { return (y * 16.4f); }, vector_break(gyro).y));
    put_be16(out, 12, kern::formula<f32>([](auto z) { return (z * 16.4f); }, vector_break(gyro).z));
  }

  void Drone_motor_tick(Drone& self, const BodyState& s, Fixed<f32, 4>& thrust_out, Fixed<f32, 4>& torque_out, f32 dt = 0.0f, f32 altitude = 0.0f) {
    f32 ocv = 0.0f;
    f32 limit = 0.0f;
    f32 total = 0.0f;
    f32 command = 0.0f;
    f32 volts = 0.0f;
    f32 k = 0.0f;
    Vector local_v = Vector{0.0, 0.0, 0.0};
    f32 rev = 0.0f;
    f32 prop_torque = 0.0f;
    f32 motor_torque = 0.0f;
    f32 n = 0.0f;
    f32 thrust = 0.0f;
    f32 advance = 0.0f;
    ocv = battery_ocv(self.soc_, self.p_.cells).Result;
    limit = kern::formula<f32, f32>([](auto motor_limit_a, auto esc_limit_a) { return fmin(motor_limit_a, esc_limit_a); }, self.p_.motor_limit_a, self.p_.esc_limit_a);
    total = 0.0f;
    for (auto it26 = flow_for(n26, 0, 1); it26.next();) {
      total = 0.0f;
      for (auto it27 = flow_for(n27, 0, 3); it27.next();) {
        command = common::clampf(kern::formula<u16>([](auto a) { return ((num_cast<f32, i32>(num_cast<i32, u16>(a)) - 1000.0f) / 1000.0f); }, self.pulses_[num_cast<i64, i32>(num_cast<i32, i64>(n27.index))]), 0.0f, 1.0f).Result;
        volts = kern::formula<f32, i64, f32, f32>([](auto command, auto index, auto vbatt_, auto ocv) { return (command * (num_cast<bool, i32>(num_cast<i32, i64>(index)) ? vbatt_ : ocv)); }, command, n26.index, self.vbatt_, ocv);
        self.current_[num_cast<i64, i32>(num_cast<i32, i64>(n27.index))] = common::clampf(kern::formula<f32, f32, f32, f32>([](auto volts, auto back_emf_, auto a, auto r) { return ((volts - (back_emf_ * a)) / r); }, volts, self.back_emf_, self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n27.index))], self.p_.R), 0.0f, limit).Result;
        total += self.current_[num_cast<i64, i32>(num_cast<i32, i64>(n27.index))];
      }
      if (logic_and(math_compare<">", f32>(total, self.p_.battery_max_a), math_compare<">", f32>(total, 1e-12f))) {
        k = kern::formula<f32, f32>([](auto battery_max_a, auto total) { return (battery_max_a / total); }, self.p_.battery_max_a, total);
        for (auto it28 = flow_for(n28, 0, math_sub<i64>(list_length<f32>(self.current_), 1)); it28.next();) {
          self.current_[n28.index] = kern::formula<f32, f32>([](auto a, auto k) { return (a * k); }, self.current_[n28.index], k);
        }
        total = self.p_.battery_max_a;
      }
      self.vbatt_ = battery_under_load(ocv, total, self.p_.battery_r, self.p_.cells).Result;
    }
    self.ibatt_ = total;
    self.soc_ = common::clampf(kern::formula<f32, f32, f32, f32>([](auto soc_, auto total, auto dt, auto capacity_ah) { return (soc_ - ((total * dt) / (capacity_ah * 3600.0f))); }, self.soc_, total, dt, self.p_.capacity_ah), 0.0f, 1.0f).Result;
    local_v = common::to_local(s.rotation, s.velocity).Result;
    for (auto it29 = flow_for(n29, 0, 3); it29.next();) {
      rev = kern::formula<f32, f32>([](auto a, auto kPi) { return (a / (2.0f * kPi)); }, self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))], kPi);
      prop_torque = kern::formula<f32, f32, f32, f32>([](auto cq, auto rho, auto rev, auto d5_) { return ((((cq * rho) * rev) * rev) * d5_); }, self.p_.Cq, self.p_.rho, rev, self.d5_);
      motor_torque = kern::formula<f32, f32>([](auto back_emf_, auto a) { return (back_emf_ * a); }, self.back_emf_, self.current_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))]);
      self.motor_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))] = motor_torque;
      self.prop_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))] = prop_torque;
      self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))] = kern::formula<f32, f32, f32, f32, f32, f32>([](auto a, auto motor_torque, auto prop_torque, auto kBearingDrag, auto dt, auto j) { return fmax(0.0f, (a + ((((motor_torque - prop_torque) - (kBearingDrag * a)) * dt) / j))); }, self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))], motor_torque, prop_torque, kBearingDrag, dt, self.p_.J);
      n = kern::formula<f32, f32>([](auto a, auto kPi) { return (a / (2.0f * kPi)); }, self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))], kPi);
      thrust = kern::formula<f32, f32, f32, f32>([](auto ct, auto rho, auto n, auto d4_) { return ((((ct * rho) * n) * n) * d4_); }, self.p_.Ct, self.p_.rho, n, self.d4_);
      advance = kern::formula<f32, f32, f32>([](auto z, auto n, auto prop_d) { return (z / fmax(1.0f, (n * prop_d))); }, vector_break(local_v).z, n, self.p_.prop_d);
      thrust = kern::formula<f32, f32>([](auto thrust, auto the_result) { return (thrust * the_result); }, thrust, common::clampf(kern::formula<f32>([](auto advance) { return (1.0f - (0.12f * advance)); }, advance), 0.55f, 1.25f).Result);
      thrust = kern::formula<f32, f32, f32, f32>([](auto thrust, auto ground_effect, auto altitude, auto prop_d) { return (thrust * (1.0f + (ground_effect * exp(((-altitude) / fmax(0.02f, (0.75f * prop_d))))))); }, thrust, self.p_.ground_effect, altitude, self.p_.prop_d);
      thrust_out[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))] = kern::formula<f32, f32>([](auto a, auto thrust) { return (a + thrust); }, thrust_out[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))], thrust);
      torque_out[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))] = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto motor_torque) { return (a + (a_2 * motor_torque)); }, torque_out[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))], list_get<f32>(kMotorYawSign, num_cast<i64, i32>(num_cast<i32, i64>(n29.index))), motor_torque);
      self.rotor_angle_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))] = kern::formula<f32, i64, f32, f32, f32, f32, f32>([](auto a, auto index, auto a_2, auto a0f, auto a_3, auto dt, auto kPi) { return fmod((a + (((num_cast<bool, i32>((num_cast<i32, i64>(index) % 2)) ? a_2 : a0f) * a_3) * dt)), (2.0f * kPi)); }, self.rotor_angle_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))], n29.index, -1.0f, 1.0f, self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n29.index))], dt, kPi);
    }
  }

  struct step_centeredResult { u16 Result = 0; };
  KERN_PURE step_centeredResult step_centered(f32 v = 0.0f) {
    return {kern::formula<f32>([](auto the_result) { return num_cast<u16, i64>(num_cast<i64, f32>(round((992.0f + (820.0f * the_result))))); }, common::clampf(v, -1.0f, 1.0f).Result)};
  }

  void Drone_step(Drone& self, const DroneSticks& sticks, Object<kern::physics::World> world = Object<kern::physics::World>{}, physics_Body body = physics_Body{0}, f32 h = 0.0f) {
    Object<kern::physics::World> world_ = Object<kern::physics::World>{};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}};
    Vector down = Vector{0.0, 0.0, 0.0};
    Vector rotate = Vector{0.0, 0.0, 0.0};
    f32 vertical = 0.0f;
    the_RayResult_ range = the_RayResult_{false, 0.0f, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Vector o = Vector{0.0, 0.0, 0.0};
    Vector rotate_2 = Vector{0.0, 0.0, 0.0};
    f32 travelled = 0.0f;
    i32 k = 0;
    bool stop = false;
    physics_RayHit h_2 = physics_RayHit{false, physics_Shape{0}, physics_Body{0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f};
    physics_RayHit cast_ray = physics_RayHit{false, physics_Shape{0}, physics_Body{0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f};
    f32 d = 0.0f;
    bool valid = false;
    physics_BodyType body_type = physics_BodyType::fixed;
    physics_BodyType body_type_2 = physics_BodyType::fixed;
    bool either = false;
    bool agl_valid = false;
    f32 true_agl = 0.0f;
    the_RayResult_ below = the_RayResult_{false, 0.0f, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    f32 altitude = 0.0f;
    Fixed<u16, 16> ch = Fixed<u16, 16>{};
    f32 clearance = 0.0f;
    i32 ticks = 0;
    f32 dt = 0.0f;
    Fixed<f32, 4> thrust = Fixed<f32, 4>{};
    Fixed<f32, 4> yaw_torque = Fixed<f32, 4>{};
    fc_RuntimeOutput out = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    fc_HardwareFrame hw = fc_HardwareFrame{0, 1000, 0, List<u8>{}, List<u8>{}, hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}, false, false, false};
    f32 measured = 0.0f;
    i32 at = 0;
    f32 put = 0.0f;
    f32 agl = 0.0f;
    f32 heading = 0.0f;
    u16 was = 0;
    f32 length = 0.0f;
    f32 length_2 = 0.0f;
    bool both = false;
    f32 length_3 = 0.0f;
    f32 length_4 = 0.0f;
    bool both_2 = false;
    bool commanded = false;
    f32 length_5 = 0.0f;
    f32 length_6 = 0.0f;
    f32 length_7 = 0.0f;
    bool both_3 = false;
    f32 inv = 0.0f;
    Vector rotate_3 = Vector{0.0, 0.0, 0.0};
    Vector rotate_4 = Vector{0.0, 0.0, 0.0};
    Vector rotate_5 = Vector{0.0, 0.0, 0.0};
    Vector rel = Vector{0.0, 0.0, 0.0};
    f32 hs = 0.0f;
    Vector drag = Vector{0.0, 0.0, 0.0};
    Vector rotate_6 = Vector{0.0, 0.0, 0.0};
    Vector w = Vector{0.0, 0.0, 0.0};
    Vector rotate_7 = Vector{0.0, 0.0, 0.0};
    f32 length_8 = 0.0f;
    world_ = world;
    s = common::read_body(world_, body).Result;
    if (logic_and(self.have_prev_, math_compare<">", f32>(h, 0.0f))) {
      self.world_accel_ = vector_scale(vector_sub(s.velocity, self.prev_velocity_), kern::formula<f32>([](auto h) { return (1.0f / h); }, h));
    }
    self.prev_velocity_ = s.velocity;
    self.have_prev_ = true;
    rotate = engine_quat_rotate(s.rotation, Vector{0.0, 0.0, -1.0});
    down = rotate;
    vertical = kern::formula<f32>([](auto z) { return (-z); }, vector_break(down).z);
    range = the_RayResult_{false, 0.0f, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    if (math_compare<">", f32>(vertical, 0.55f)) {
      rotate_2 = engine_quat_rotate(s.rotation, Vector{0.0, 0.0, -0.018});
      o = vector_add(s.position, rotate_2);
      travelled = 0.0f;
      k = 0;
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<"<", i32>(k, 6)))) {
        cast_ray = engine_world_cast_ray(world_, o, vector_scale(down, kern::formula<f32, f32>([](auto kNavRayMax, auto travelled) { return (kNavRayMax - travelled); }, kNavRayMax, travelled)), engine_physics_physics::QueryFilter_new(1, 18446744073709551615u).Result, body);
        h_2 = cast_ray;
        if (logic_not(h_2.hit)) {
          stop = true;
        }
        if (logic_not(stop)) {
          d = kern::formula<f32, f32, f32>([](auto fraction, auto kNavRayMax, auto travelled) { return (fraction * (kNavRayMax - travelled)); }, h_2.fraction, kNavRayMax, travelled);
          valid = engine_body_valid(h_2.body);
          either = logic_not(valid);
          if (logic_not(either)) {
            body_type_2 = engine_world_body_type(world_, h_2.body);
            either = enum_equal<physics_BodyType>(body_type_2, physics_BodyType::fixed);
          }
          if (either) {
            range.hit = true;
            range.distance = kern::formula<f32, f32>([](auto travelled, auto d) { return (travelled + d); }, travelled, d);
            range.point = h_2.point;
            range.normal = h_2.normal;
            stop = true;
          }
          if (logic_not(stop)) {
            travelled += kern::formula<f32>([](auto d) { return (d + 0.02f); }, d);
            o = vector_add(h_2.point, vector_scale(down, 0.02f));
            if (math_compare<">=", f32>(travelled, kNavRayMax)) {
              stop = true;
            }
          }
        }
        if (logic_not(stop)) {
          k += 1;
        }
      }
    }
    agl_valid = logic_and(range.hit, math_compare<">=", f32>(range.distance, 0.001f));
    true_agl = kern::formula<bool, f32, f32, f32>([](auto agl_valid, auto distance, auto vertical, auto a0f) { return (agl_valid ? (distance * vertical) : a0f); }, agl_valid, range.distance, vertical, 999.0f);
    below = common::cast(world_, s.position, Vector{0.0, 0.0, -1.0}, 50.0f, body, 18446744073709551615u).Result;
    altitude = kern::formula<bool, f32, f32>([](auto hit, auto distance, auto a0f) { return fmax(0.001f, (hit ? distance : a0f)); }, below.hit, below.distance, 50.0f);
    ch = List<u16>{};
    for (auto it30 = flow_for(n30, 0, math_sub<i64>(list_length<u16>(ch), 1)); it30.next();) {
      ch[n30.index] = 992;
    }
    ch[0] = step_centered(sticks.roll).Result;
    ch[1] = step_centered(sticks.pitch).Result;
    ch[3] = step_centered(sticks.yaw).Result;
    ch[4] = kern::formula<bool, bool, i32, i32>([](auto arm, auto kill, auto a1811, auto a172) { return num_cast<u16, i32>(((arm && (!kill)) ? a1811 : a172)); }, sticks.arm, sticks.kill, 1811, 172);
    if (sticks.game_mode) {
      ch[2] = 172;
      clearance = common::clampf(sticks.clearance, kMinClearance, kMaxClearance).Result;
      ch[5] = kern::formula<f32, f32, f32>([](auto clearance, auto kMinClearance, auto kMaxClearance) { return num_cast<u16, i64>(num_cast<i64, f32>(round((172.0f + ((1639.0f * (clearance - kMinClearance)) / (kMaxClearance - kMinClearance)))))); }, clearance, kMinClearance, kMaxClearance);
      ch[6] = 1811;
      ch[7] = step_centered(sticks.body_pitch).Result;
    } else {
      ch[2] = kern::formula<f32>([](auto the_result) { return num_cast<u16, i64>(num_cast<i64, f32>(round((172.0f + (1639.0f * the_result))))); }, common::clampf(sticks.throttle, 0.0f, 1.0f).Result);
    }
    ticks = kern::formula<f32>([](auto h) { return max(1, num_cast<i32, i64>(num_cast<i64, f32>(round((h * 1000.0f))))); }, h);
    dt = kern::formula<f32, i32>([](auto h, auto ticks) { return (h / num_cast<f32, i32>(ticks)); }, h, ticks);
    thrust = List<f32>{};
    yaw_torque = List<f32>{};
    out = struct_make<fc_RuntimeOutput>(List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false);
    for (auto it31 = flow_for(n31, 0, math_sub<i64>(num_cast<i64, i32>(ticks), 1)); it31.next();) {
      hw = struct_make<fc_HardwareFrame>(0, 1000, 0, List<u8>{}, List<u8>{}, hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}, false, false, false);
      self.now_us_ = kern::formula<f64, f32>([](auto now_us_, auto dt) { return (now_us_ + (num_cast<f64, f32>(dt) * 1000000.0)); }, self.now_us_, dt);
      hw.now_us = num_cast<u64, f64>(self.now_us_);
      hw.dt_us = kern::formula<f32>([](auto dt) { return num_cast<u32, i64>(num_cast<i64, f32>(round((dt * 1e+06f)))); }, dt);
      Drone_imu_bytes(self, s, hw.imu_registers, dt);
      hw.imu_present = true;
      self.sbus_elapsed_ = kern::formula<f32, f32>([](auto sbus_elapsed_, auto dt) { return (sbus_elapsed_ + dt); }, self.sbus_elapsed_, dt);
      if (math_compare<">=", f32>(self.sbus_elapsed_, 0.01f)) {
        self.sbus_elapsed_ = kern::formula<f32>([](auto sbus_elapsed_) { return (sbus_elapsed_ - 0.01f); }, self.sbus_elapsed_);
        encode_sbus(ch, hw.sbus_frame);
        hw.sbus_present = true;
      }
      self.nav_elapsed_ = kern::formula<f32, f32>([](auto nav_elapsed_, auto dt) { return (nav_elapsed_ + dt); }, self.nav_elapsed_, dt);
      if (math_compare<">=", f32>(self.nav_elapsed_, 0.01f)) {
        self.nav_elapsed_ = kern::formula<f32>([](auto nav_elapsed_) { return (nav_elapsed_ - 0.01f); }, self.nav_elapsed_);
        for (auto it32 = flow_for(n32, 0, 2); it32.next();) {
          n33 = common::Rng_gaussian(self.nav_noise_);
          measured = kern::formula<f32, f32>([](auto a, auto the_result) { return (a + (the_result * 0.025f)); }, kern::formula<i32, f32, f32, f32>([](auto i, auto x, auto y, auto z) { return i == 0 ? x : (i == 1 ? y : z); }, num_cast<i32, i64>(n32.index), vector_break(s.velocity).x, vector_break(s.velocity).y, vector_break(s.velocity).z), n33.Result);
          at = num_cast<i32, i64>(n32.index);
          put = kern::formula<f32, f32>([](auto a, auto measured) { return (a + (0.42f * (measured - a))); }, kern::formula<i32, f32, f32, f32>([](auto i, auto x, auto y, auto z) { return i == 0 ? x : (i == 1 ? y : z); }, num_cast<i32, i64>(n32.index), vector_break(self.nav_filtered_).x, vector_break(self.nav_filtered_).y, vector_break(self.nav_filtered_).z), measured);
          self.nav_filtered_ = vector_make(kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i == 0 ? v : c; }, at, put, vector_break(self.nav_filtered_).x), kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i == 1 ? v : c; }, at, put, vector_break(self.nav_filtered_).y), kern::formula<i32, f32, f32>([](auto i, auto v, auto c) { return i != 0 && i != 1 ? v : c; }, at, put, vector_break(self.nav_filtered_).z));
        }
        agl = 0.0f;
        if (agl_valid) {
          n34 = common::Rng_gaussian(self.nav_noise_);
          agl = kern::formula<f32, f32, f32>([](auto distance, auto the_result, auto vertical) { return (fmax(0.0f, (distance + (the_result * 0.004f))) * vertical); }, range.distance, n34.Result, vertical);
        }
        n35 = common::Rng_gaussian(self.heading_noise_);
        heading = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result + (the_result_2 * 0.12f)); }, quat_yaw_deg(s.rotation).Result, n35.Result);
        was = self.nav_seq_;
        self.nav_seq_ = num_cast<u16, i32>(kern::formula<u16>([](auto nav_seq_) { return (num_cast<i32, u16>(nav_seq_) + 1); }, self.nav_seq_));
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
        self.pulses_[num_cast<i64, i32>(num_cast<i32, i64>(n37.index))] = kern::formula<bool, i32, u16>([](auto kill, auto a1000, auto a) { return num_cast<u16, i32>((kill ? a1000 : num_cast<i32, u16>(a))); }, sticks.kill, 1000, out.motor_us[num_cast<i64, i32>(num_cast<i32, i64>(n37.index))]);
      }
      Drone_motor_tick(self, s, thrust, yaw_torque, dt, altitude);
    }
    both = logic_and(math_compare<"!=", i32>(num_cast<i32, i64>(enum_index<fc_FaultCode>(out.fault)), num_cast<i32, i64>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone))), logic_not(out.armed));
    if (both) {
      length_2 = engine_length_2(s.velocity);
      both = math_compare<"<", f32>(length_2, 0.05f);
    }
    both_2 = both;
    if (both_2) {
      length_4 = engine_length_2(s.angular);
      both_2 = math_compare<"<", f32>(length_4, 0.05f);
    }
    if (both_2) {
      self.reset_flag_ = true;
    }
    commanded = false;
    for (auto it38 = flow_for(n38, 0, 3); it38.next();) {
      commanded = num_cast<bool, i32>(kern::formula<bool, u16, f32>([](auto commanded, auto a, auto a_2) { return (num_cast<i32, bool>(commanded) | num_cast<i32, bool>(((num_cast<i32, u16>(a) > 1000) || (a_2 > 0.001f)))); }, commanded, self.pulses_[num_cast<i64, i32>(num_cast<i32, i64>(n38.index))], self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n38.index))]));
    }
    if (commanded) {
      self.parked_steps_ = 0;
      self.parked_ = false;
    } else {
      length_5 = engine_length_2(s.velocity);
      both_3 = math_compare<"<", f32>(length_5, 0.002f);
      if (both_3) {
        length_7 = engine_length_2(s.angular);
        both_3 = math_compare<"<", f32>(length_7, 0.002f);
      }
      if (both_3) {
        self.parked_steps_ = kern::formula<u32>([](auto parked_steps_) { return (parked_steps_ + 1); }, self.parked_steps_);
        self.parked_ = math_compare<">", u32>(self.parked_steps_, kern::formula<f32>([](auto h) { return num_cast<u32, f32>((0.5f / h)); }, h));
      } else {
        self.parked_steps_ = 0;
        self.parked_ = false;
      }
    }
    inv = kern::formula<i32>([](auto ticks) { return (1.0f / num_cast<f32, i32>(ticks)); }, ticks);
    if (commanded) {
      for (auto it39 = flow_for(n39, 0, 3); it39.next();) {
        rotate_3 = engine_quat_rotate(s.rotation, vector_make(0.0f, 0.0f, kern::formula<f32, f32>([](auto a, auto inv) { return (a * inv); }, thrust[num_cast<i64, i32>(num_cast<i32, i64>(n39.index))], inv)));
        rotate_4 = engine_quat_rotate(s.rotation, self.motor_pos_[num_cast<i64, i32>(num_cast<i32, i64>(n39.index))]);
        engine_world_apply_force(world_, body, rotate_3, vector_add(s.position, rotate_4));
        rotate_5 = engine_quat_rotate(s.rotation, vector_make(0.0f, 0.0f, kern::formula<f32, f32>([](auto a, auto inv) { return (a * inv); }, yaw_torque[num_cast<i64, i32>(num_cast<i32, i64>(n39.index))], inv)));
        engine_world_apply_torque(world_, body, rotate_5);
      }
    }
    rel = common::to_local(s.rotation, vector_sub(s.velocity, self.p_.wind)).Result;
    hs = kern::formula<f32, f32>([](auto x, auto y) { return hypot(x, y); }, vector_break(rel).x, vector_break(rel).y);
    drag = vector_make(kern::formula<f32, f32, f32, f32>([](auto rho, auto x, auto x_2, auto hs) { return (((((-0.5f) * rho) * x) * x_2) * hs); }, self.p_.rho, vector_break(self.cdA_).x, vector_break(rel).x, hs), kern::formula<f32, f32, f32, f32>([](auto rho, auto y, auto y_2, auto hs) { return (((((-0.5f) * rho) * y) * y_2) * hs); }, self.p_.rho, vector_break(self.cdA_).y, vector_break(rel).y, hs), kern::formula<f32, f32, f32>([](auto rho, auto z, auto z_2) { return (((((-0.5f) * rho) * z) * z_2) * abs(z_2)); }, self.p_.rho, vector_break(self.cdA_).z, vector_break(rel).z));
    rotate_6 = engine_quat_rotate(s.rotation, drag);
    engine_world_apply_force_to_center(world_, body, rotate_6);
    w = common::to_local(s.rotation, s.angular).Result;
    rotate_7 = engine_quat_rotate(s.rotation, vector_make(kern::formula<f32>([](auto x) { return (((-0.0012f) * x) * abs(x)); }, vector_break(w).x), kern::formula<f32>([](auto y) { return (((-0.0012f) * y) * abs(y)); }, vector_break(w).y), kern::formula<f32>([](auto z) { return (((-0.0012f) * z) * abs(z)); }, vector_break(w).z)));
    engine_world_apply_torque(world_, body, rotate_7);
    self.tele_.state = num_cast<u32, u16>(out.state);
    self.tele_.armed = out.armed;
    self.tele_.calibrating = math_compare<"!=", i32>(kern::formula<u16, i32>([](auto state, auto a) { return (num_cast<i32, u16>(state) & a); }, out.state, num_cast<i32, i64>(list_get<i64>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateCalibrating)))), 0);
    self.tele_.fault = math_compare<"!=", i32>(kern::formula<u16, i32>([](auto state, auto a) { return (num_cast<i32, u16>(state) & a); }, out.state, num_cast<i32, i64>(list_get<i64>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateFault)))), 0);
    self.tele_.game = math_compare<"!=", i32>(kern::formula<u16, u16>([](auto state, auto kStateGameMode) { return (num_cast<i32, u16>(state) & num_cast<i32, u16>(kStateGameMode)); }, out.state, kStateGameMode), 0);
    self.tele_.nav_valid = math_compare<"!=", i32>(kern::formula<u16, u16>([](auto state, auto kStateNavigationValid) { return (num_cast<i32, u16>(state) & num_cast<i32, u16>(kStateNavigationValid)); }, out.state, kStateNavigationValid), 0);
    for (auto it40 = flow_for(n40, 0, 3); it40.next();) {
      self.tele_.pulses[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))] = self.pulses_[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))];
      self.tele_.omega[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))] = self.omega_[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))];
      self.tele_.motor_torque[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))] = self.motor_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))];
      self.tele_.prop_torque[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))] = self.prop_torque_[num_cast<i64, i32>(num_cast<i32, i64>(n40.index))];
    }
    self.tele_.battery_v = self.vbatt_;
    self.tele_.battery_soc = self.soc_;
    self.tele_.current_a = self.ibatt_;
    self.tele_.agl = kern::formula<bool, f32, f32>([](auto agl_valid, auto true_agl, auto a) { return (agl_valid ? true_agl : a); }, agl_valid, true_agl, -1.0f);
    length_8 = engine_length_2(s.velocity);
    self.tele_.speed = length_8;
    self.tele_.vz = vector_break(s.velocity).z;
    for (auto it41 = flow_for(n41, 0, 2); it41.next();) {
      self.tele_.attitude_deg[num_cast<i64, i32>(num_cast<i32, i64>(n41.index))] = kern::formula<i16>([](auto a) { return (num_cast<f32, i32>(num_cast<i32, i16>(a)) * 0.01f); }, out.attitude_cdeg[num_cast<i64, i32>(num_cast<i32, i64>(n41.index))]);
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  struct : kern::Node<12> { i64 index = 0; } n12;
  Arondight45_FirmwareRuntime::FirmwareRuntime_newResult n13;
  struct : kern::Node<14> { i64 index = 0; } n14;
  struct : kern::Node<15> { i64 index = 0; } n15;
  struct : kern::Node<16> { i64 index = 0; } n16;
  struct : kern::Node<17> { i64 index = 0; } n17;
  struct : kern::Node<18> { i64 index = 0; } n18;
  struct : kern::Node<19> { i64 index = 0; } n19;
  struct : kern::Node<20> { i64 index = 0; } n20;
  common::Rng_gaussianResult n21;
  common::Rng_gaussianResult n22;
  struct : kern::Node<23> { i64 index = 0; } n23;
  common::Rng_gaussianResult n24;
  common::Rng_gaussianResult n25;
  struct : kern::Node<26> { i64 index = 0; } n26;
  struct : kern::Node<27> { i64 index = 0; } n27;
  struct : kern::Node<28> { i64 index = 0; } n28;
  struct : kern::Node<29> { i64 index = 0; } n29;
  struct : kern::Node<30> { i64 index = 0; } n30;
  struct : kern::Node<31> { i64 index = 0; } n31;
  struct : kern::Node<32> { i64 index = 0; } n32;
  common::Rng_gaussianResult n33;
  common::Rng_gaussianResult n34;
  common::Rng_gaussianResult n35;
  Arondight45_FirmwareRuntime::FirmwareRuntime_stepResult n36;
  struct : kern::Node<37> { i64 index = 0; } n37;
  struct : kern::Node<38> { i64 index = 0; } n38;
  struct : kern::Node<39> { i64 index = 0; } n39;
  struct : kern::Node<40> { i64 index = 0; } n40;
  struct : kern::Node<41> { i64 index = 0; } n41;
};
