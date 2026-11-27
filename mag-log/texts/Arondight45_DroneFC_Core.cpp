struct Arondight45_DroneFC_Core : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(fc_ArmState) { bool armed_ = false; bool saw_low_ = false; u64 since_us_ = 0; };
  KERN_STRUCT(fc_Attitude) { f32 roll = 0.0f; f32 pitch = 0.0f; f32 yaw = 0.0f; };
  KERN_STRUCT(fc_Command) { f32 roll = 0.0f; f32 pitch = 0.0f; f32 throttle = 0.0f; f32 yaw = 0.0f; bool arm = false; };
  KERN_ENUM(fc_FaultCode, kFaultNone, kFaultProtocol, kFaultImu, kFaultTiming, kFaultRate, kFaultTilt);
  KERN_STRUCT(fc_Gains) { f32 kp = 0.0f; f32 ki = 0.0f; f32 kd = 0.0f; f32 integral_limit = 0.0f; f32 output_limit = 0.0f; f32 d_cutoff_hz = 0.0f; };
  KERN_STRUCT(fc_Mix) { Fixed<f32, 4> motor = List<f32>{}; };
  KERN_STRUCT(fc_PT1) { f32 cutoff_hz_ = 0.0f; f32 y_ = 0.0f; bool initialized_ = false; };
  KERN_STRUCT(fc_RC) { Fixed<u16, 16> ch = List<u16>{}; bool lost = false; bool failsafe = false; bool valid = false; u64 us = 0; };
  KERN_STRUCT(fc_SbusParser) { Fixed<u8, 25> buffer_ = List<u8>{}; u64 size_ = 0; u64 last_us_ = 0; };
  KERN_ENUM(fc_StateBits, kStateArmed, kStateCalibrating, kStateFault, kStateRcValid, kStateImuValid);
  KERN_STRUCT(fc_V3) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; };
  KERN_STRUCT(the_fc_ArmState_Result_) { bool armed = false; bool armed_now = false; bool disarmed_now = false; };
  KERN_STRUCT(fc_Filters) { Fixed<fc_PT1, 3> gyro = List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}; Fixed<fc_PT1, 3> accel = List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}; };
  KERN_STRUCT(fc_Imu) { fc_V3 a = fc_V3{0.0f, 0.0f, 0.0f}; fc_V3 g = fc_V3{0.0f, 0.0f, 0.0f}; };
  KERN_STRUCT(fc_PID) { fc_Gains gains_ = fc_Gains{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; fc_PT1 d_filter_ = fc_PT1{0.0f, 0.0f, false}; f32 integral_ = 0.0f; f32 previous_ = 0.0f; bool have_previous_ = false; };
  KERN_STRUCT(fc_RuntimeOutput) { Fixed<u16, 4> motor_us = List<u16>{1000, 1000, 1000, 1000}; Fixed<i16, 3> attitude_cdeg = List<i16>{}; u16 state = 0; fc_FaultCode fault = fc_FaultCode::kFaultNone; bool armed = false; };
  KERN_STRUCT(fc_Controller) { fc_Attitude attitude = fc_Attitude{0.0f, 0.0f, 0.0f}; fc_PID roll_pid_ = fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}; fc_PID pitch_pid_ = fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}; fc_PID yaw_pid_ = fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}; };
  KERN_STRUCT(fc_RuntimeInput) { fc_Imu raw = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}; fc_RC rc = fc_RC{List<u16>{}, false, false, false, 0}; u64 now_us = 0; u32 dt_us = 1000; u32 missed_samples = 0; bool imu_valid = true; bool rc_fresh = false; };
  KERN_STRUCT(fc_Runtime) { fc_Filters filters_ = fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}; fc_Controller controller_ = fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}; fc_ArmState arm_ = fc_ArmState{false, false, 0}; bool calibrated_ = false; u32 calibration_count_ = 0; fc_V3 gyro_sum_ = fc_V3{0.0f, 0.0f, 0.0f}; fc_V3 gyro_sq_sum_ = fc_V3{0.0f, 0.0f, 0.0f}; fc_V3 accel_sum_ = fc_V3{0.0f, 0.0f, 0.0f}; fc_V3 gyro_bias_ = fc_V3{0.0f, 0.0f, 0.0f}; u32 bad_timing_ = 0; fc_FaultCode fault_ = fc_FaultCode::kFaultNone; bool armed_ = false; };

  // ---- variables ----
  KERN_CONST f32 kPi = 3.1415927f;
  KERN_CONST f32 kManualAttitudeCommandScale = 0.8f;
  /// Attitude-priority mixing (as Betaflight/PX4 desaturation): in flight the collective yields to the attitude correction. At full collective (a full-power climb) the motors keep their differential authority by giving up a little thrust instead of losing all torque — the old throttle-priority mix scaled corrections to zero at t = 1, so a full-power takeoff could not hold level and pitched over. On the ground / at idle (t <= 5 %) the collective is never raised for a correction (no hopping), corrections scale down instead.
  KERN_CONST f32 kMixFlightCollective = 0.05f;
  KERN_CONST u16 kEscIdleUs = 1050;
  KERN_CONST u16 kEscMaxUs = 2000;
  KERN_CONST u16 kEscMinUs = 1000;
  KERN_CONST f32 kInnerMaxAttitudeDeg = 40.0f;
  /// The number of each value of fc_StateBits, in their order
  KERN_CONST List<i64> fc_StateBits_numbers = List<i64>{1, 2, 4, 8, 16};
  KERN_CONST u32 kCalibrationSamples = 2000;
  KERN_CONST u32 kMinDtUs = 600;
  KERN_CONST u32 kMaxDtUs = 1600;
  KERN_CONST u32 kNominalDtUs = 1000;
  KERN_CONST u32 kRcTimeoutUs = 100000;
  KERN_CONST f32 kManualMaxAttitudeDeg = 32.0f;

  // ---- functions ----
  struct magResult { f32 Result = 0.0f; };
  KERN_PURE magResult mag(fc_V3 v = fc_V3{0.0f, 0.0f, 0.0f}) {
    return {kern::formula<f32, f32, f32>([](auto x, auto y, auto z) { return sqrt((((x * x) + (y * y)) + (z * z))); }, v.x, v.y, v.z)};
  }

  struct finiteResult { bool Result = false; };
  KERN_PURE finiteResult finite(fc_V3 v = fc_V3{0.0f, 0.0f, 0.0f}) {
    return {logic_and(logic_and(kern::formula<f32>([](auto x) { return isfinite(x); }, v.x), kern::formula<f32>([](auto y) { return isfinite(y); }, v.y)), kern::formula<f32>([](auto z) { return isfinite(z); }, v.z))};
  }

  struct PT1_newResult { fc_PT1 Result = fc_PT1{0.0f, 0.0f, false}; };
  KERN_PURE PT1_newResult PT1_new(f32 cutoff_hz = 0.0f) {
    fc_PT1 self = fc_PT1{0.0f, 0.0f, false};
    self.cutoff_hz_ = cutoff_hz;
    return {self};
  }

  struct PT1_runResult { f32 Result = 0.0f; };
  PT1_runResult PT1_run(fc_PT1& self, f32 x = 0.0f, f32 dt = 0.0f) {
    f32 rc = 0.0f;
    f32 k = 0.0f;
    if (logic_or(logic_or(logic_not(self.initialized_), logic_not(math_compare<">", f32>(dt, 0.0f))), logic_not(kern::formula<f32>([](auto dt) { return isfinite(dt); }, dt)))) {
      self.y_ = x;
      self.initialized_ = true;
      return {self.y_};
    }
    rc = kern::formula<f32, f32>([](auto kPi, auto cutoff_hz_) { return (1.0f / ((2.0f * kPi) * cutoff_hz_)); }, kPi, self.cutoff_hz_);
    k = kern::formula<f32>([](auto a) { return (a < 0.0f ? 0.0f : (1.0f < a ? 1.0f : a)); }, kern::formula<f32, f32>([](auto dt, auto rc) { return (dt / (rc + dt)); }, dt, rc));
    self.y_ = kern::formula<f32, f32, f32>([](auto y_, auto k, auto x) { return (y_ + (k * (x - y_))); }, self.y_, k, x);
    return {self.y_};
  }

  void PT1_reset(fc_PT1& self) {
    self.y_ = 0.0f;
    self.initialized_ = false;
  }

  struct Filters_runResult { fc_Imu Result = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}; };
  Filters_runResult Filters_run(fc_Filters& self, fc_Imu s = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}, f32 dt = 0.0f) {
    n10 = PT1_run(self.accel[0], s.a.x, dt);
    n11 = PT1_run(self.accel[1], s.a.y, dt);
    n12 = PT1_run(self.accel[2], s.a.z, dt);
    n13 = PT1_run(self.gyro[0], s.g.x, dt);
    n14 = PT1_run(self.gyro[1], s.g.y, dt);
    n15 = PT1_run(self.gyro[2], s.g.z, dt);
    return {struct_make<fc_Imu>(struct_make<fc_V3>(n10.Result, n11.Result, n12.Result), struct_make<fc_V3>(n13.Result, n14.Result, n15.Result))};
  }

  void Attitude_reset(fc_Attitude& self, fc_V3 a = fc_V3{0.0f, 0.0f, 0.0f}) {
    self.roll = kern::formula<f32, f32, f32>([](auto y, auto z, auto kPi) { return ((atan2(y, z) * 180.0f) / kPi); }, a.y, a.z, kPi);
    self.pitch = kern::formula<f32, f32, f32, f32>([](auto x, auto y, auto z, auto kPi) { return ((atan2((-x), sqrt(((y * y) + (z * z)))) * 180.0f) / kPi); }, a.x, a.y, a.z, kPi);
    self.yaw = 0.0f;
  }

  void Attitude_run(fc_Attitude& self, fc_Imu s = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}, f32 dt = 0.0f) {
    f32 phi = 0.0f;
    f32 theta = 0.0f;
    f32 sin_phi = 0.0f;
    f32 cos_phi = 0.0f;
    f32 sin_theta = 0.0f;
    f32 cos_theta = 0.0f;
    f32 tan_theta = 0.0f;
    f32 roll_rate = 0.0f;
    f32 pitch_rate = 0.0f;
    f32 yaw_rate = 0.0f;
    f32 n = 0.0f;
    f32 accel_roll = 0.0f;
    f32 accel_pitch = 0.0f;
    f32 tau = 0.0f;
    f32 k = 0.0f;
    phi = kern::formula<f32, f32>([](auto roll, auto kPi) { return ((roll * kPi) / 180.0f); }, self.roll, kPi);
    theta = kern::formula<f32, f32>([](auto pitch, auto kPi) { return ((pitch * kPi) / 180.0f); }, self.pitch, kPi);
    sin_phi = kern::formula<f32>([](auto phi) { return sin(phi); }, phi);
    cos_phi = kern::formula<f32>([](auto phi) { return cos(phi); }, phi);
    sin_theta = kern::formula<f32>([](auto theta) { return sin(theta); }, theta);
    cos_theta = kern::formula<f32>([](auto theta) { return cos(theta); }, theta);
    if (math_compare<"<", f32>(kern::formula<f32>([](auto cos_theta) { return abs(cos_theta); }, cos_theta), 0.05f)) {
      cos_theta = kern::formula<f32, f32>([](auto cos_theta, auto a0f) { return copysign(0.05f, ((cos_theta == 0.0f) ? a0f : cos_theta)); }, cos_theta, 1.0f);
    }
    tan_theta = kern::formula<f32, f32>([](auto sin_theta, auto cos_theta) { return (sin_theta / cos_theta); }, sin_theta, cos_theta);
    roll_rate = kern::formula<f32, f32, f32, f32, f32, f32>([](auto x, auto sin_phi, auto tan_theta, auto y, auto cos_phi, auto z) { return ((x + ((sin_phi * tan_theta) * y)) + ((cos_phi * tan_theta) * z)); }, s.g.x, sin_phi, tan_theta, s.g.y, cos_phi, s.g.z);
    pitch_rate = kern::formula<f32, f32, f32, f32>([](auto cos_phi, auto y, auto sin_phi, auto z) { return ((cos_phi * y) - (sin_phi * z)); }, cos_phi, s.g.y, sin_phi, s.g.z);
    yaw_rate = kern::formula<f32, f32, f32, f32, f32>([](auto sin_phi, auto cos_theta, auto y, auto cos_phi, auto z) { return (((sin_phi / cos_theta) * y) + ((cos_phi / cos_theta) * z)); }, sin_phi, cos_theta, s.g.y, cos_phi, s.g.z);
    self.roll = kern::formula<f32, f32, f32>([](auto roll, auto roll_rate, auto dt) { return (roll + (roll_rate * dt)); }, self.roll, roll_rate, dt);
    self.pitch = kern::formula<f32, f32, f32>([](auto pitch, auto pitch_rate, auto dt) { return (pitch + (pitch_rate * dt)); }, self.pitch, pitch_rate, dt);
    self.yaw = kern::formula<f32, f32, f32>([](auto yaw, auto yaw_rate, auto dt) { return (yaw + (yaw_rate * dt)); }, self.yaw, yaw_rate, dt);
    while (loop(math_compare<">", f32>(self.yaw, 180.0f))) {
      self.yaw = kern::formula<f32>([](auto yaw) { return (yaw - 360.0f); }, self.yaw);
    }
    while (loop(math_compare<"<", f32>(self.yaw, -180.0f))) {
      self.yaw = kern::formula<f32>([](auto yaw) { return (yaw + 360.0f); }, self.yaw);
    }
    n = mag(s.a).Result;
    if (logic_and(math_compare<">", f32>(n, 0.8f), math_compare<"<", f32>(n, 1.2f))) {
      accel_roll = kern::formula<f32, f32, f32>([](auto y, auto z, auto kPi) { return ((atan2(y, z) * 180.0f) / kPi); }, s.a.y, s.a.z, kPi);
      accel_pitch = kern::formula<f32, f32, f32, f32>([](auto x, auto y, auto z, auto kPi) { return ((atan2((-x), sqrt(((y * y) + (z * z)))) * 180.0f) / kPi); }, s.a.x, s.a.y, s.a.z, kPi);
      tau = kern::formula<f32>([](auto kPi) { return (1.0f / ((2.0f * kPi) * 0.02f)); }, kPi);
      k = kern::formula<f32>([](auto a) { return (a < 0.0f ? 0.0f : (0.02f < a ? 0.02f : a)); }, kern::formula<f32, f32>([](auto dt, auto tau) { return (dt / (tau + dt)); }, dt, tau));
      self.roll = kern::formula<f32, f32, f32>([](auto roll, auto k, auto accel_roll) { return (roll + (k * (accel_roll - roll))); }, self.roll, k, accel_roll);
      self.pitch = kern::formula<f32, f32, f32>([](auto pitch, auto k, auto accel_pitch) { return (pitch + (k * (accel_pitch - pitch))); }, self.pitch, k, accel_pitch);
    }
  }

  struct PID_newResult { fc_PID Result = fc_PID{fc_Gains{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, fc_PT1{0.0f, 0.0f, false}, 0.0f, 0.0f, false}; };
  KERN_PURE PID_newResult PID_new(fc_Gains gains = fc_Gains{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}) {
    fc_PID self = fc_PID{fc_Gains{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}, fc_PT1{0.0f, 0.0f, false}, 0.0f, 0.0f, false};
    self.gains_ = gains;
    self.d_filter_ = PT1_new(gains.d_cutoff_hz).Result;
    return {self};
  }

  struct PID_runResult { f32 Result = 0.0f; };
  PID_runResult PID_run(fc_PID& self, f32 setpoint = 0.0f, f32 measurement = 0.0f, f32 dt = 0.0f, bool integrate = false) {
    f32 error = 0.0f;
    f32 derivative = 0.0f;
    f32 pre = 0.0f;
    if (logic_or(logic_or(logic_not(math_compare<">", f32>(dt, 0.0f)), logic_not(kern::formula<f32>([](auto setpoint) { return isfinite(setpoint); }, setpoint))), logic_not(kern::formula<f32>([](auto measurement) { return isfinite(measurement); }, measurement)))) {
      return {0.0f};
    }
    error = kern::formula<f32, f32>([](auto setpoint, auto measurement) { return (setpoint - measurement); }, setpoint, measurement);
    derivative = kern::formula<bool, f32, f32, f32, f32>([](auto have_previous_, auto measurement, auto previous_, auto dt, auto a0f) { return (have_previous_ ? ((-(measurement - previous_)) / dt) : a0f); }, self.have_previous_, measurement, self.previous_, dt, 0.0f);
    self.previous_ = measurement;
    self.have_previous_ = true;
    n16 = PT1_run(self.d_filter_, derivative, dt);
    derivative = n16.Result;
    pre = kern::formula<f32, f32, f32, f32, f32>([](auto kp, auto error, auto integral_, auto kd, auto derivative) { return (((kp * error) + integral_) + (kd * derivative)); }, self.gains_.kp, error, self.integral_, self.gains_.kd, derivative);
    if (logic_and(integrate, logic_not(logic_or(logic_and(math_compare<">=", f32>(pre, self.gains_.output_limit), math_compare<">", f32>(error, 0.0f)), logic_and(math_compare<"<=", f32>(pre, kern::formula<f32>([](auto output_limit) { return (-output_limit); }, self.gains_.output_limit)), math_compare<"<", f32>(error, 0.0f)))))) {
      self.integral_ = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto integral_limit) { return (a < a_2 ? a_2 : (integral_limit < a ? integral_limit : a)); }, kern::formula<f32, f32, f32, f32>([](auto integral_, auto ki, auto error, auto dt) { return (integral_ + ((ki * error) * dt)); }, self.integral_, self.gains_.ki, error, dt), kern::formula<f32>([](auto integral_limit) { return (-integral_limit); }, self.gains_.integral_limit), self.gains_.integral_limit);
    }
    return {kern::formula<f32, f32, f32>([](auto a, auto a_2, auto output_limit) { return (a < a_2 ? a_2 : (output_limit < a ? output_limit : a)); }, kern::formula<f32, f32, f32, f32, f32>([](auto kp, auto error, auto integral_, auto kd, auto derivative) { return (((kp * error) + integral_) + (kd * derivative)); }, self.gains_.kp, error, self.integral_, self.gains_.kd, derivative), kern::formula<f32>([](auto output_limit) { return (-output_limit); }, self.gains_.output_limit), self.gains_.output_limit)};
  }

  void PID_reset(fc_PID& self) {
    self.integral_ = 0.0f;
    self.previous_ = 0.0f;
    self.have_previous_ = false;
    PT1_reset(self.d_filter_);
  }

  struct PID_integralResult { f32 Result = 0.0f; };
  KERN_PURE PID_integralResult PID_integral(const fc_PID& self) {
    return {self.integral_};
  }

  struct sbus_endResult { bool Result = false; };
  KERN_PURE sbus_endResult sbus_end(u8 b = 0) {
    return {logic_or(logic_or(logic_or(logic_or(math_compare<"==", i32>(num_cast<i32, u8>(b), 0), math_compare<"==", i32>(num_cast<i32, u8>(b), 4)), math_compare<"==", i32>(num_cast<i32, u8>(b), 20)), math_compare<"==", i32>(num_cast<i32, u8>(b), 36)), math_compare<"==", i32>(num_cast<i32, u8>(b), 52))};
  }

  struct decode_sbusResult { bool Result = false; };
  decode_sbusResult decode_sbus(fc_RC& out, List<u8> p = List<u8>{}, i64 p_at = 0) {
    if (logic_or(logic_or(logic_not(math_compare<">=", i64>(p_at, 0)), math_compare<"!=", i32>(num_cast<i32, u8>(p[p_at]), 15)), logic_not(sbus_end(p[math_add<i64>(p_at, 24)]).Result))) {
      return {false};
    }
    out.ch[0] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>(((num_cast<i32, u8>(a) | (num_cast<i32, u8>(a_2) << 8)) & 2047)); }, p[math_add<i64>(p_at, 1)], p[math_add<i64>(p_at, 2)]);
    out.ch[1] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 3) | (num_cast<i32, u8>(a_2) << 5)) & 2047)); }, p[math_add<i64>(p_at, 2)], p[math_add<i64>(p_at, 3)]);
    out.ch[2] = kern::formula<u8, u8, u8>([](auto a, auto a_2, auto a_3) { return num_cast<u16, i32>(((((num_cast<i32, u8>(a) >> 6) | (num_cast<i32, u8>(a_2) << 2)) | (num_cast<i32, u8>(a_3) << 10)) & 2047)); }, p[math_add<i64>(p_at, 3)], p[math_add<i64>(p_at, 4)], p[math_add<i64>(p_at, 5)]);
    out.ch[3] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 1) | (num_cast<i32, u8>(a_2) << 7)) & 2047)); }, p[math_add<i64>(p_at, 5)], p[math_add<i64>(p_at, 6)]);
    out.ch[4] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 4) | (num_cast<i32, u8>(a_2) << 4)) & 2047)); }, p[math_add<i64>(p_at, 6)], p[math_add<i64>(p_at, 7)]);
    out.ch[5] = kern::formula<u8, u8, u8>([](auto a, auto a_2, auto a_3) { return num_cast<u16, i32>(((((num_cast<i32, u8>(a) >> 7) | (num_cast<i32, u8>(a_2) << 1)) | (num_cast<i32, u8>(a_3) << 9)) & 2047)); }, p[math_add<i64>(p_at, 7)], p[math_add<i64>(p_at, 8)], p[math_add<i64>(p_at, 9)]);
    out.ch[6] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 2) | (num_cast<i32, u8>(a_2) << 6)) & 2047)); }, p[math_add<i64>(p_at, 9)], p[math_add<i64>(p_at, 10)]);
    out.ch[7] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 5) | (num_cast<i32, u8>(a_2) << 3)) & 2047)); }, p[math_add<i64>(p_at, 10)], p[math_add<i64>(p_at, 11)]);
    out.ch[8] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>(((num_cast<i32, u8>(a) | (num_cast<i32, u8>(a_2) << 8)) & 2047)); }, p[math_add<i64>(p_at, 12)], p[math_add<i64>(p_at, 13)]);
    out.ch[9] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 3) | (num_cast<i32, u8>(a_2) << 5)) & 2047)); }, p[math_add<i64>(p_at, 13)], p[math_add<i64>(p_at, 14)]);
    out.ch[10] = kern::formula<u8, u8, u8>([](auto a, auto a_2, auto a_3) { return num_cast<u16, i32>(((((num_cast<i32, u8>(a) >> 6) | (num_cast<i32, u8>(a_2) << 2)) | (num_cast<i32, u8>(a_3) << 10)) & 2047)); }, p[math_add<i64>(p_at, 14)], p[math_add<i64>(p_at, 15)], p[math_add<i64>(p_at, 16)]);
    out.ch[11] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 1) | (num_cast<i32, u8>(a_2) << 7)) & 2047)); }, p[math_add<i64>(p_at, 16)], p[math_add<i64>(p_at, 17)]);
    out.ch[12] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 4) | (num_cast<i32, u8>(a_2) << 4)) & 2047)); }, p[math_add<i64>(p_at, 17)], p[math_add<i64>(p_at, 18)]);
    out.ch[13] = kern::formula<u8, u8, u8>([](auto a, auto a_2, auto a_3) { return num_cast<u16, i32>(((((num_cast<i32, u8>(a) >> 7) | (num_cast<i32, u8>(a_2) << 1)) | (num_cast<i32, u8>(a_3) << 9)) & 2047)); }, p[math_add<i64>(p_at, 18)], p[math_add<i64>(p_at, 19)], p[math_add<i64>(p_at, 20)]);
    out.ch[14] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 2) | (num_cast<i32, u8>(a_2) << 6)) & 2047)); }, p[math_add<i64>(p_at, 20)], p[math_add<i64>(p_at, 21)]);
    out.ch[15] = kern::formula<u8, u8>([](auto a, auto a_2) { return num_cast<u16, i32>((((num_cast<i32, u8>(a) >> 5) | (num_cast<i32, u8>(a_2) << 3)) & 2047)); }, p[math_add<i64>(p_at, 21)], p[math_add<i64>(p_at, 22)]);
    out.lost = math_compare<"!=", i32>(kern::formula<u8>([](auto a) { return (num_cast<i32, u8>(a) & 4); }, p[math_add<i64>(p_at, 23)]), 0);
    out.failsafe = math_compare<"!=", i32>(kern::formula<u8>([](auto a) { return (num_cast<i32, u8>(a) & 8); }, p[math_add<i64>(p_at, 23)]), 0);
    out.valid = logic_and(logic_not(out.lost), logic_not(out.failsafe));
    return {true};
  }

  struct SbusParser_feedResult { bool Result = false; };
  SbusParser_feedResult SbusParser_feed(fc_SbusParser& self, fc_RC& out, u8 byte = 0, u64 us = 0) {
    u64 was = 0;
    u64 was_2 = 0;
    u64 i = 0;
    bool stop = false;
    u64 keep = 0;
    i64 count = 0;
    i64 to = 0;
    i64 from = 0;
    i64 last = 0;
    i64 k = 0;
    if (logic_and(logic_and(num_cast<bool, u64>(self.size_), math_compare<">", u64>(us, self.last_us_)), math_compare<">", u64>(kern::formula<u64, u64>([](auto us, auto last_us_) { return (us - last_us_); }, us, self.last_us_), 3000))) {
      self.size_ = 0;
    }
    self.last_us_ = us;
    if (logic_not(num_cast<bool, u64>(self.size_))) {
      if (math_compare<"!=", i32>(num_cast<i32, u8>(byte), 15)) {
        return {false};
      }
      was = self.size_;
      self.size_ = kern::formula<u64>([](auto size_) { return (size_ + 1); }, self.size_);
      self.buffer_[num_cast<i64, u64>(was)] = byte;
      return {false};
    }
    was_2 = self.size_;
    self.size_ = kern::formula<u64>([](auto size_) { return (size_ + 1); }, self.size_);
    self.buffer_[num_cast<i64, u64>(was_2)] = byte;
    if (math_compare<"<", u64>(self.size_, num_cast<u64, i64>(list_length<u8>(self.buffer_)))) {
      return {false};
    }
    self.size_ = 0;
    n17 = decode_sbus(out, self.buffer_, 0);
    if (n17.Result) {
      out.us = us;
      return {true};
    }
    i = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<u8>(self.buffer_)))))) {
      if (math_compare<"==", i32>(num_cast<i32, u8>(self.buffer_[num_cast<i64, u64>(i)]), 15)) {
        keep = kern::formula<u64, u64>([](auto a, auto i) { return (a - i); }, num_cast<u64, i64>(list_length<u8>(self.buffer_)), i);
        count = num_cast<i64, u64>(keep);
        to = 0;
        from = num_cast<i64, u64>(i);
        last = math_sub<i64>(count, 1);
        if (math_compare<"<=", i64>(to, from)) {
          for (auto it18 = flow_for(n18, 0, last); it18.next();) {
            self.buffer_[math_add<i64>(to, n18.index)] = list_get<u8>(self.buffer_, math_add<i64>(from, n18.index));
          }
        } else {
          k = last;
          while (loop(math_compare<">=", i64>(k, 0))) {
            self.buffer_[math_add<i64>(to, k)] = list_get<u8>(self.buffer_, math_add<i64>(from, k));
            k = math_sub<i64>(k, 1);
          }
        }
        self.size_ = keep;
        stop = true;
      }
      if (logic_not(stop)) {
        i += 1;
      }
    }
    return {false};
  }

  struct centeredResult { f32 Result = 0.0f; };
  KERN_PURE centeredResult centered(u16 v = 0) {
    return {kern::formula<f32>([](auto a) { return (a < (-1.0f) ? (-1.0f) : (1.0f < a ? 1.0f : a)); }, kern::formula<u16>([](auto v) { return ((num_cast<f32, u16>(v) - 992.0f) / 820.0f); }, v))};
  }

  struct throttleResult { f32 Result = 0.0f; };
  KERN_PURE throttleResult throttle(u16 v = 0) {
    return {kern::formula<f32>([](auto a) { return (a < 0.0f ? 0.0f : (1.0f < a ? 1.0f : a)); }, kern::formula<u16>([](auto v) { return ((num_cast<f32, u16>(v) - 172.0f) / 1639.0f); }, v))};
  }

  struct shapeResult { f32 Result = 0.0f; };
  KERN_PURE shapeResult shape(f32 x = 0.0f, f32 deadband = 0.0f, f32 expo = 0.0f) {
    f32 a = 0.0f;
    f32 t = 0.0f;
    f32 v = 0.0f;
    a = kern::formula<f32>([](auto x) { return abs(x); }, x);
    if (math_compare<"<=", f32>(a, deadband)) {
      return {0.0f};
    }
    t = kern::formula<f32, f32>([](auto a, auto deadband) { return ((a - deadband) / (1.0f - deadband)); }, a, deadband);
    v = kern::formula<f32, f32>([](auto t, auto expo) { return ((t * (1.0f - expo)) + (((t * t) * t) * expo)); }, t, expo);
    return {kern::formula<f32, f32>([](auto v, auto x) { return copysign((v < 0.0f ? 0.0f : (1.0f < v ? 1.0f : v)), x); }, v, x)};
  }

  struct finite_2Result { bool Result = false; };
  KERN_PURE finite_2Result finite_2(fc_Command c = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}) {
    return {logic_and(logic_and(logic_and(kern::formula<f32>([](auto roll) { return isfinite(roll); }, c.roll), kern::formula<f32>([](auto pitch) { return isfinite(pitch); }, c.pitch)), kern::formula<f32>([](auto throttle) { return isfinite(throttle); }, c.throttle)), kern::formula<f32>([](auto yaw) { return isfinite(yaw); }, c.yaw))};
  }

  struct sanitizeResult { fc_Command Result = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}; };
  KERN_PURE sanitizeResult sanitize(fc_Command c = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}) {
    fc_Command c_ = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false};
    c_ = c;
    c_.roll = kern::formula<f32>([](auto roll) { return (roll < (-1.0f) ? (-1.0f) : (1.0f < roll ? 1.0f : roll)); }, c_.roll);
    c_.pitch = kern::formula<f32>([](auto pitch) { return (pitch < (-1.0f) ? (-1.0f) : (1.0f < pitch ? 1.0f : pitch)); }, c_.pitch);
    c_.throttle = kern::formula<f32>([](auto throttle) { return (throttle < 0.0f ? 0.0f : (1.0f < throttle ? 1.0f : throttle)); }, c_.throttle);
    c_.yaw = kern::formula<f32>([](auto yaw) { return (yaw < (-1.0f) ? (-1.0f) : (1.0f < yaw ? 1.0f : yaw)); }, c_.yaw);
    return {c_};
  }

  struct commandResult { fc_Command Result = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}; };
  KERN_PURE commandResult command(const fc_RC& r) {
    return {struct_make<fc_Command>(kern::formula<f32, f32>([](auto the_result, auto kManualAttitudeCommandScale) { return (the_result * kManualAttitudeCommandScale); }, shape(centered(r.ch[0]).Result, 0.035f, 0.3f).Result, kManualAttitudeCommandScale), kern::formula<f32, f32>([](auto the_result, auto kManualAttitudeCommandScale) { return ((-the_result) * kManualAttitudeCommandScale); }, shape(centered(r.ch[1]).Result, 0.035f, 0.3f).Result, kManualAttitudeCommandScale), throttle(r.ch[2]).Result, shape(centered(r.ch[3]).Result, 0.045f, 0.2f).Result, math_compare<">", i32>(num_cast<i32, u16>(r.ch[4]), 1300))};
  }

  struct mixResult { fc_Mix Result = fc_Mix{List<f32>{}}; };
  KERN_PURE mixResult mix(f32 t = 0.0f, f32 roll = 0.0f, f32 pitch = 0.0f, f32 yaw = 0.0f) {
    f32 t_ = 0.0f;
    Fixed<f32, 4> correction = Fixed<f32, 4>{};
    f32 cmax = 0.0f;
    f32 cmin = 0.0f;
    f32 v = 0.0f;
    f32 scale_factor = 0.0f;
    f32 range = 0.0f;
    f32 hi = 0.0f;
    f32 lo = 0.0f;
    f32 v_2 = 0.0f;
    fc_Mix out = fc_Mix{List<f32>{}};
    t_ = t;
    t_ = kern::formula<f32>([](auto t_) { return (t_ < 0.0f ? 0.0f : (1.0f < t_ ? 1.0f : t_)); }, t_);
    correction = list_make<f32, "4">(kern::formula<f32, f32, f32>([](auto pitch, auto roll, auto yaw) { return ((pitch - roll) - (0.65f * yaw)); }, pitch, roll, yaw), kern::formula<f32, f32, f32>([](auto pitch, auto roll, auto yaw) { return ((pitch + roll) + (0.65f * yaw)); }, pitch, roll, yaw), kern::formula<f32, f32, f32>([](auto pitch, auto roll, auto yaw) { return (((-pitch) + roll) - (0.65f * yaw)); }, pitch, roll, yaw), kern::formula<f32, f32, f32>([](auto pitch, auto roll, auto yaw) { return (((-pitch) - roll) + (0.65f * yaw)); }, pitch, roll, yaw));
    cmax = correction[0];
    cmin = correction[0];
    for (auto it19 = flow_for(n19, 0, math_sub<i64>(list_length<f32>(correction), 1)); it19.next();) {
      v = correction[n19.index];
      cmax = kern::formula<f32, f32>([](auto cmax, auto v) { return max(cmax, v); }, cmax, v);
      cmin = kern::formula<f32, f32>([](auto cmin, auto v) { return min(cmin, v); }, cmin, v);
    }
    scale_factor = 1.0f;
    if (math_compare<">", f32>(t_, kMixFlightCollective)) {
      range = kern::formula<f32, f32>([](auto cmax, auto cmin) { return (cmax - cmin); }, cmax, cmin);
      if (math_compare<">", f32>(range, 1.0f)) {
        scale_factor = kern::formula<f32>([](auto range) { return (1.0f / range); }, range);
      }
      hi = kern::formula<f32, f32>([](auto cmax, auto scale_factor) { return (cmax * scale_factor); }, cmax, scale_factor);
      lo = kern::formula<f32, f32>([](auto cmin, auto scale_factor) { return (cmin * scale_factor); }, cmin, scale_factor);
      if (math_compare<">", f32>(kern::formula<f32, f32>([](auto t_, auto hi) { return (t_ + hi); }, t_, hi), 1.0f)) {
        t_ = kern::formula<f32>([](auto hi) { return (1.0f - hi); }, hi);
      }
      if (math_compare<"<", f32>(kern::formula<f32, f32>([](auto t_, auto lo) { return (t_ + lo); }, t_, lo), 0.0f)) {
        t_ = kern::formula<f32>([](auto lo) { return (-lo); }, lo);
      }
    } else {
      for (auto it20 = flow_for(n20, 0, math_sub<i64>(list_length<f32>(correction), 1)); it20.next();) {
        v_2 = correction[n20.index];
        if (math_compare<">", f32>(v_2, 0.0f)) {
          scale_factor = kern::formula<f32, f32, f32>([](auto scale_factor, auto t_, auto v_2) { return min(scale_factor, ((1.0f - t_) / v_2)); }, scale_factor, t_, v_2);
        } else if (math_compare<"<", f32>(v_2, 0.0f)) {
          scale_factor = kern::formula<f32, f32, f32>([](auto scale_factor, auto t_, auto v_2) { return min(scale_factor, (t_ / (-v_2))); }, scale_factor, t_, v_2);
        }
      }
      scale_factor = kern::formula<f32>([](auto scale_factor) { return (scale_factor < 0.0f ? 0.0f : (1.0f < scale_factor ? 1.0f : scale_factor)); }, scale_factor);
    }
    out = fc_Mix{List<f32>{}};
    for (auto it21 = flow_for(n21, 0, 3); it21.next();) {
      out.motor[n21.index] = kern::formula<f32>([](auto a) { return (a < 0.0f ? 0.0f : (1.0f < a ? 1.0f : a)); }, kern::formula<f32, f32, f32>([](auto t_, auto scale_factor, auto a) { return (t_ + (scale_factor * a)); }, t_, scale_factor, correction[n21.index]));
    }
    return {out};
  }

  struct pulseResult { u16 Result = 0; };
  KERN_PURE pulseResult pulse(f32 value = 0.0f, bool armed = false) {
    return {kern::formula<bool, u16, f32, u16, u16>([](auto armed, auto kEscIdleUs, auto value, auto kEscMaxUs, auto kEscMinUs) { return (armed ? num_cast<u16, f32>(((num_cast<f32, i32>(num_cast<i32, u16>(kEscIdleUs)) + ((value < 0.0f ? 0.0f : (1.0f < value ? 1.0f : value)) * num_cast<f32, i32>((num_cast<i32, u16>(kEscMaxUs) - num_cast<i32, u16>(kEscIdleUs))))) + 0.5f)) : kEscMinUs); }, armed, kEscIdleUs, value, kEscMaxUs, kEscMinUs)};
  }

  struct ArmState_runResult { the_fc_ArmState_Result_ Result = the_fc_ArmState_Result_{false, false, false}; };
  ArmState_runResult ArmState_run(fc_ArmState& self, u64 us = 0, bool control_valid = false, fc_Command cmd = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}, bool imu_ok = false, f32 roll = 0.0f, f32 pitch = 0.0f) {
    the_fc_ArmState_Result_ result = the_fc_ArmState_Result_{false, false, false};
    bool ok = false;
    result = struct_make<the_fc_ArmState_Result_>(self.armed_, false, false);
    if (logic_not(control_valid)) {
      result.disarmed_now = self.armed_;
      self.armed_ = false;
      self.saw_low_ = false;
      self.since_us_ = 0;
      result.armed = false;
      return {result};
    }
    if (logic_not(cmd.arm)) {
      self.saw_low_ = true;
      self.since_us_ = 0;
      result.disarmed_now = self.armed_;
      self.armed_ = false;
      result.armed = false;
      return {result};
    }
    if (self.armed_) {
      return {result};
    }
    ok = logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(self.saw_low_, math_compare<"<=", f32>(cmd.throttle, 0.035f)), math_compare<"<", f32>(kern::formula<f32>([](auto roll) { return abs(roll); }, cmd.roll), 0.12f)), math_compare<"<", f32>(kern::formula<f32>([](auto pitch) { return abs(pitch); }, cmd.pitch), 0.12f)), math_compare<"<", f32>(kern::formula<f32>([](auto yaw) { return abs(yaw); }, cmd.yaw), 0.15f)), math_compare<"<", f32>(kern::formula<f32>([](auto roll) { return abs(roll); }, roll), 20.0f)), math_compare<"<", f32>(kern::formula<f32>([](auto pitch) { return abs(pitch); }, pitch), 20.0f)), imu_ok);
    if (logic_not(ok)) {
      self.since_us_ = 0;
      return {result};
    }
    if (logic_not(num_cast<bool, u64>(self.since_us_))) {
      self.since_us_ = us;
    }
    if (math_compare<">=", u64>(kern::formula<u64, u64>([](auto us, auto since_us_) { return (us - since_us_); }, us, self.since_us_), 1000000)) {
      self.armed_ = true;
      result.armed = true;
      result.armed_now = true;
    }
    return {result};
  }

  void Controller_reset(fc_Controller& self) {
    PID_reset(self.roll_pid_);
    PID_reset(self.pitch_pid_);
    PID_reset(self.yaw_pid_);
  }

  struct Controller_runResult { fc_Mix Result = fc_Mix{List<f32>{}}; };
  Controller_runResult Controller_run(fc_Controller& self, fc_Imu s = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}, fc_Command cmd = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}, f32 dt = 0.0f, bool integrate = false) {
    f32 kAngleToRate = 0.0f;
    f32 roll_rate = 0.0f;
    f32 pitch_rate = 0.0f;
    f32 yaw_rate = 0.0f;
    f32 arg = 0.0f;
    kAngleToRate = 4.5f;
    roll_rate = kern::formula<f32>([](auto a) { return (a < (-240.0f) ? (-240.0f) : (240.0f < a ? 240.0f : a)); }, kern::formula<f32, f32, f32, f32>([](auto roll, auto kInnerMaxAttitudeDeg, auto roll_2, auto kAngleToRate) { return (((roll * kInnerMaxAttitudeDeg) - roll_2) * kAngleToRate); }, cmd.roll, kInnerMaxAttitudeDeg, self.attitude.roll, kAngleToRate));
    pitch_rate = kern::formula<f32>([](auto a) { return (a < (-240.0f) ? (-240.0f) : (240.0f < a ? 240.0f : a)); }, kern::formula<f32, f32, f32, f32>([](auto pitch, auto kInnerMaxAttitudeDeg, auto pitch_2, auto kAngleToRate) { return (((pitch * kInnerMaxAttitudeDeg) - pitch_2) * kAngleToRate); }, cmd.pitch, kInnerMaxAttitudeDeg, self.attitude.pitch, kAngleToRate));
    yaw_rate = kern::formula<f32>([](auto yaw) { return (yaw * 180.0f); }, cmd.yaw);
    arg = cmd.throttle;
    n22 = PID_run(self.roll_pid_, roll_rate, s.g.x, dt, integrate);
    n23 = PID_run(self.pitch_pid_, pitch_rate, s.g.y, dt, integrate);
    n24 = PID_run(self.yaw_pid_, yaw_rate, s.g.z, dt, integrate);
    return {mix(arg, n22.Result, n23.Result, n24.Result).Result};
  }

  void Runtime_reset(fc_Runtime& self) {
    self.filters_ = struct_make<fc_Filters>(List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}});
    self.controller_ = fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}};
    self.arm_ = fc_ArmState{false, false, 0};
    self.calibrated_ = false;
    self.calibration_count_ = 0;
    self.gyro_sum_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
    self.gyro_sq_sum_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
    self.accel_sum_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
    self.gyro_bias_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
    self.bad_timing_ = 0;
    self.fault_ = fc_FaultCode::kFaultNone;
    self.armed_ = false;
  }

  struct Runtime_newResult { fc_Runtime Result = fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}; };
  Runtime_newResult Runtime_new() {
    fc_Runtime self = fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false};
    Runtime_reset(self);
    return {self};
  }

  struct Runtime_to_cdegResult { i16 Result = 0; };
  KERN_PURE Runtime_to_cdegResult Runtime_to_cdeg(f32 degrees = 0.0f) {
    f32 value = 0.0f;
    value = kern::formula<f32>([](auto a) { return (a < (-32767.0f) ? (-32767.0f) : (32767.0f < a ? 32767.0f : a)); }, kern::formula<f32>([](auto degrees) { return (degrees * 100.0f); }, degrees));
    return {kern::formula<f32>([](auto value) { return num_cast<i16, i64>(num_cast<i64, f32>(round(value))); }, value)};
  }

  struct Runtime_finalizeResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false}; };
  KERN_PURE Runtime_finalizeResult Runtime_finalize(const fc_Runtime& self, fc_RuntimeOutput out = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false}, bool control_ok = false, bool imu_ok = false) {
    fc_RuntimeOutput out_ = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    out_ = out;
    out_.armed = logic_and(self.armed_, math_compare<"==", i32>(num_cast<i32, i64>(enum_index<fc_FaultCode>(self.fault_)), num_cast<i32, i64>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone))));
    out_.fault = self.fault_;
    if (out_.armed) {
      out_.state = num_cast<u16, i32>(kern::formula<u16, i32>([](auto state, auto a) { return (num_cast<i32, u16>(state) | a); }, out_.state, num_cast<i32, i64>(list_get<i64>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateArmed)))));
    }
    if (logic_not(self.calibrated_)) {
      out_.state = num_cast<u16, i32>(kern::formula<u16, i32>([](auto state, auto a) { return (num_cast<i32, u16>(state) | a); }, out_.state, num_cast<i32, i64>(list_get<i64>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateCalibrating)))));
    }
    if (math_compare<"!=", i32>(num_cast<i32, i64>(enum_index<fc_FaultCode>(self.fault_)), num_cast<i32, i64>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
      out_.state = num_cast<u16, i32>(kern::formula<u16, i32>([](auto state, auto a) { return (num_cast<i32, u16>(state) | a); }, out_.state, num_cast<i32, i64>(list_get<i64>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateFault)))));
      out_.state = num_cast<u16, i32>(kern::formula<u16, u16>([](auto state, auto a) { return (num_cast<i32, u16>(state) | (num_cast<i32, u16>(a) << 8)); }, out_.state, num_cast<u16, i64>(enum_index<fc_FaultCode>(self.fault_))));
      out_.motor_us = list_make<u16, "4">(kEscMinUs, kEscMinUs, kEscMinUs, kEscMinUs);
    }
    if (control_ok) {
      out_.state = num_cast<u16, i32>(kern::formula<u16, i32>([](auto state, auto a) { return (num_cast<i32, u16>(state) | a); }, out_.state, num_cast<i32, i64>(list_get<i64>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateRcValid)))));
    }
    if (imu_ok) {
      out_.state = num_cast<u16, i32>(kern::formula<u16, i32>([](auto state, auto a) { return (num_cast<i32, u16>(state) | a); }, out_.state, num_cast<i32, i64>(list_get<i64>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateImuValid)))));
    }
    out_.attitude_cdeg = list_make<i16, "3">(Runtime_to_cdeg(self.controller_.attitude.roll).Result, Runtime_to_cdeg(self.controller_.attitude.pitch).Result, Runtime_to_cdeg(self.controller_.attitude.yaw).Result);
    return {out_};
  }

  void Runtime_set_fault(fc_Runtime& self, fc_FaultCode fault = fc_FaultCode::kFaultNone) {
    if (math_compare<"==", i32>(num_cast<i32, i64>(enum_index<fc_FaultCode>(self.fault_)), num_cast<i32, i64>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
      self.fault_ = fault;
    }
    self.armed_ = false;
  }

  void Runtime_calibrate(fc_Runtime& self, const fc_Imu& sample) {
    f32 k = 0.0f;
    fc_V3 accel_mean = fc_V3{0.0f, 0.0f, 0.0f};
    fc_V3 gyro_sd = fc_V3{0.0f, 0.0f, 0.0f};
    bool still = false;
    self.gyro_sum_.x = kern::formula<f32, f32>([](auto x, auto x_2) { return (x + x_2); }, self.gyro_sum_.x, sample.g.x);
    self.gyro_sum_.y = kern::formula<f32, f32>([](auto y, auto y_2) { return (y + y_2); }, self.gyro_sum_.y, sample.g.y);
    self.gyro_sum_.z = kern::formula<f32, f32>([](auto z, auto z_2) { return (z + z_2); }, self.gyro_sum_.z, sample.g.z);
    self.gyro_sq_sum_.x = kern::formula<f32, f32>([](auto x, auto x_2) { return (x + (x_2 * x_2)); }, self.gyro_sq_sum_.x, sample.g.x);
    self.gyro_sq_sum_.y = kern::formula<f32, f32>([](auto y, auto y_2) { return (y + (y_2 * y_2)); }, self.gyro_sq_sum_.y, sample.g.y);
    self.gyro_sq_sum_.z = kern::formula<f32, f32>([](auto z, auto z_2) { return (z + (z_2 * z_2)); }, self.gyro_sq_sum_.z, sample.g.z);
    self.accel_sum_.x = kern::formula<f32, f32>([](auto x, auto x_2) { return (x + x_2); }, self.accel_sum_.x, sample.a.x);
    self.accel_sum_.y = kern::formula<f32, f32>([](auto y, auto y_2) { return (y + y_2); }, self.accel_sum_.y, sample.a.y);
    self.accel_sum_.z = kern::formula<f32, f32>([](auto z, auto z_2) { return (z + z_2); }, self.accel_sum_.z, sample.a.z);
    self.calibration_count_ = kern::formula<u32>([](auto calibration_count_) { return (calibration_count_ + 1); }, self.calibration_count_);
    if (math_compare<"<", u32>(self.calibration_count_, kCalibrationSamples)) {
      return;
    }
    k = kern::formula<u32>([](auto calibration_count_) { return (1.0f / num_cast<f32, u32>(calibration_count_)); }, self.calibration_count_);
    self.gyro_bias_ = struct_make<fc_V3>(kern::formula<f32, f32>([](auto x, auto k) { return (x * k); }, self.gyro_sum_.x, k), kern::formula<f32, f32>([](auto y, auto k) { return (y * k); }, self.gyro_sum_.y, k), kern::formula<f32, f32>([](auto z, auto k) { return (z * k); }, self.gyro_sum_.z, k));
    accel_mean = struct_make<fc_V3>(kern::formula<f32, f32>([](auto x, auto k) { return (x * k); }, self.accel_sum_.x, k), kern::formula<f32, f32>([](auto y, auto k) { return (y * k); }, self.accel_sum_.y, k), kern::formula<f32, f32>([](auto z, auto k) { return (z * k); }, self.accel_sum_.z, k));
    gyro_sd = struct_make<fc_V3>(kern::formula<f32, f32, f32>([](auto x, auto k, auto x_2) { return sqrt(max(0.0f, ((x * k) - (x_2 * x_2)))); }, self.gyro_sq_sum_.x, k, self.gyro_bias_.x), kern::formula<f32, f32, f32>([](auto y, auto k, auto y_2) { return sqrt(max(0.0f, ((y * k) - (y_2 * y_2)))); }, self.gyro_sq_sum_.y, k, self.gyro_bias_.y), kern::formula<f32, f32, f32>([](auto z, auto k, auto z_2) { return sqrt(max(0.0f, ((z * k) - (z_2 * z_2)))); }, self.gyro_sq_sum_.z, k, self.gyro_bias_.z));
    still = logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(math_compare<"<", f32>(kern::formula<f32>([](auto x) { return abs(x); }, self.gyro_bias_.x), 15.0f), math_compare<"<", f32>(kern::formula<f32>([](auto y) { return abs(y); }, self.gyro_bias_.y), 15.0f)), math_compare<"<", f32>(kern::formula<f32>([](auto z) { return abs(z); }, self.gyro_bias_.z), 15.0f)), math_compare<"<", f32>(gyro_sd.x, 0.8f)), math_compare<"<", f32>(gyro_sd.y, 0.8f)), math_compare<"<", f32>(gyro_sd.z, 0.8f)), math_compare<">", f32>(mag(accel_mean).Result, 0.85f)), math_compare<"<", f32>(mag(accel_mean).Result, 1.15f));
    if (logic_not(still)) {
      Runtime_set_fault(self, fc_FaultCode::kFaultImu);
      return;
    }
    self.filters_ = struct_make<fc_Filters>(List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}});
    self.controller_ = fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}};
    Attitude_reset(self.controller_.attitude, accel_mean);
    self.calibrated_ = true;
  }

  struct Runtime_step_commandResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false}; };
  Runtime_step_commandResult Runtime_step_command(fc_Runtime& self, const fc_RuntimeInput& input, fc_Command cmd = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}, bool command_valid = false) {
    fc_Command cmd_ = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false};
    fc_RuntimeOutput out = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    f32 dt = 0.0f;
    fc_Imu corrected = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}};
    fc_Imu imu = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}};
    bool accel_ok = false;
    the_fc_ArmState_Result_ arm_result = the_fc_ArmState_Result_{false, false, false};
    fc_Mix mixed = fc_Mix{List<f32>{}};
    cmd_ = cmd;
    out = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    if (math_compare<"!=", i32>(num_cast<i32, i64>(enum_index<fc_FaultCode>(self.fault_)), num_cast<i32, i64>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
      return {Runtime_finalize(self, out, false, false).Result};
    }
    if (logic_or(logic_or(logic_not(input.imu_valid), logic_not(finite(input.raw.a).Result)), logic_not(finite(input.raw.g).Result))) {
      Runtime_set_fault(self, fc_FaultCode::kFaultImu);
      return {Runtime_finalize(self, out, command_valid, false).Result};
    }
    if (logic_and(command_valid, logic_not(finite_2(cmd_).Result))) {
      Runtime_set_fault(self, fc_FaultCode::kFaultProtocol);
      return {Runtime_finalize(self, out, false, true).Result};
    }
    cmd_ = logic_select<fc_Command>(command_valid, sanitize(cmd_).Result, struct_make<fc_Command>(0.0f, 0.0f, 0.0f, 0.0f, false));
    dt = kern::formula<u32, f32>([](auto dt_us, auto a001f) { return (((dt_us > 0) && (dt_us < 100000)) ? (num_cast<f32, u32>(dt_us) * 1e-06f) : a001f); }, input.dt_us, 0.001f);
    if (logic_not(self.calibrated_)) {
      Runtime_calibrate(self, input.raw);
      n25 = ArmState_run(self.arm_, input.now_us, command_valid, cmd_, false, 0.0f, 0.0f);
      return {Runtime_finalize(self, out, command_valid, true).Result};
    }
    if (self.armed_) {
      if (logic_or(logic_or(math_compare<"<", u32>(input.dt_us, kMinDtUs), math_compare<">", u32>(input.dt_us, kMaxDtUs)), math_compare<">", u32>(input.missed_samples, 2))) {
        self.bad_timing_ = kern::formula<u32>([](auto bad_timing_) { return (bad_timing_ + 1); }, self.bad_timing_);
        if (math_compare<">=", u32>(self.bad_timing_, 5)) {
          Runtime_set_fault(self, fc_FaultCode::kFaultTiming);
        }
      } else {
        self.bad_timing_ = 0;
      }
    } else {
      self.bad_timing_ = 0;
    }
    if (math_compare<"!=", i32>(num_cast<i32, i64>(enum_index<fc_FaultCode>(self.fault_)), num_cast<i32, i64>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
      return {Runtime_finalize(self, out, command_valid, true).Result};
    }
    corrected = input.raw;
    corrected.g.x = kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, corrected.g.x, self.gyro_bias_.x);
    corrected.g.y = kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, corrected.g.y, self.gyro_bias_.y);
    corrected.g.z = kern::formula<f32, f32>([](auto z, auto z_2) { return (z - z_2); }, corrected.g.z, self.gyro_bias_.z);
    n26 = Filters_run(self.filters_, corrected, dt);
    imu = n26.Result;
    if (logic_or(logic_or(logic_or(logic_or(logic_not(finite(imu.a).Result), logic_not(finite(imu.g).Result)), math_compare<">", f32>(kern::formula<f32>([](auto x) { return abs(x); }, imu.g.x), 1750.0f)), math_compare<">", f32>(kern::formula<f32>([](auto y) { return abs(y); }, imu.g.y), 1750.0f)), math_compare<">", f32>(kern::formula<f32>([](auto z) { return abs(z); }, imu.g.z), 1750.0f))) {
      Runtime_set_fault(self, fc_FaultCode::kFaultRate);
      return {Runtime_finalize(self, out, command_valid, true).Result};
    }
    Attitude_run(self.controller_.attitude, imu, dt);
    accel_ok = logic_and(math_compare<">", f32>(mag(imu.a).Result, 0.7f), math_compare<"<", f32>(mag(imu.a).Result, 1.3f));
    n27 = ArmState_run(self.arm_, input.now_us, command_valid, cmd_, accel_ok, self.controller_.attitude.roll, self.controller_.attitude.pitch);
    arm_result = n27.Result;
    self.armed_ = arm_result.armed;
    if (logic_not(self.armed_)) {
      Controller_reset(self.controller_);
    }
    if (self.armed_) {
      mixed = struct_make<fc_Mix>(List<f32>{});
      if (math_compare<"<=", f32>(cmd_.throttle, 0.02f)) {
        Controller_reset(self.controller_);
      } else {
        n28 = Controller_run(self.controller_, imu, cmd_, dt, math_compare<">", f32>(cmd_.throttle, 0.05f));
        mixed = n28.Result;
      }
      for (auto it29 = flow_for(n29, 0, 3); it29.next();) {
        out.motor_us[n29.index] = pulse(mixed.motor[n29.index], true).Result;
      }
    }
    return {Runtime_finalize(self, out, command_valid, true).Result};
  }

  struct Runtime_stepResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false}; };
  /// Receiver/SBUS is only an input adapter. Once decoded, the flight runtime operates on a physical normalized Command. Higher-level controllers use step_command() directly instead of fabricating receiver frames.
  Runtime_stepResult Runtime_step(fc_Runtime& self, const fc_RuntimeInput& input) {
    fc_RC rc = fc_RC{List<u16>{}, false, false, false, 0};
    rc = input.rc;
    rc.valid = logic_and(rc.valid, input.rc_fresh);
    n30 = Runtime_step_command(self, input, command(rc).Result, rc.valid);
    return {n30.Result};
  }

  struct Runtime_calibratedResult { bool Result = false; };
  KERN_PURE Runtime_calibratedResult Runtime_calibrated(const fc_Runtime& self) {
    return {self.calibrated_};
  }

  struct Runtime_armedResult { bool Result = false; };
  KERN_PURE Runtime_armedResult Runtime_armed(const fc_Runtime& self) {
    return {self.armed_};
  }

  struct Runtime_faultResult { fc_FaultCode Result = fc_FaultCode::kFaultNone; };
  KERN_PURE Runtime_faultResult Runtime_fault(const fc_Runtime& self) {
    return {self.fault_};
  }

  struct Runtime_fault_nameResult { Text Result = ""; };
  KERN_PURE Runtime_fault_nameResult Runtime_fault_name(fc_FaultCode fault = fc_FaultCode::kFaultNone) {
    i32 selected = 0;
    selected = num_cast<i32, i64>(enum_index<fc_FaultCode>(fault));
    if (math_compare<"==", i32>(selected, 0)) {
      return {"none"};
    } else if (math_compare<"==", i32>(selected, 1)) {
      return {"protocol"};
    } else if (math_compare<"==", i32>(selected, 2)) {
      return {"imu"};
    } else if (math_compare<"==", i32>(selected, 3)) {
      return {"deadline"};
    } else if (math_compare<"==", i32>(selected, 4)) {
      return {"rate"};
    } else if (math_compare<"==", i32>(selected, 5)) {
      return {"tilt"};
    }
    return {"unknown"};
  }

  // ---- what nodes give and keep ----
  PT1_runResult n10;
  PT1_runResult n11;
  PT1_runResult n12;
  PT1_runResult n13;
  PT1_runResult n14;
  PT1_runResult n15;
  PT1_runResult n16;
  decode_sbusResult n17;
  struct : kern::Node<18> { i64 index = 0; } n18;
  struct : kern::Node<19> { i64 index = 0; } n19;
  struct : kern::Node<20> { i64 index = 0; } n20;
  struct : kern::Node<21> { i64 index = 0; } n21;
  PID_runResult n22;
  PID_runResult n23;
  PID_runResult n24;
  ArmState_runResult n25;
  Filters_runResult n26;
  ArmState_runResult n27;
  Controller_runResult n28;
  struct : kern::Node<29> { i64 index = 0; } n29;
  Runtime_step_commandResult n30;
};
