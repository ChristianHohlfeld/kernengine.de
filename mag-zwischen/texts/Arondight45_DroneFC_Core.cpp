struct Arondight45_DroneFC_Core : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(fc_ArmState) { Bool armed_ = false; Bool saw_low_ = false; Int64 since_us_ = Int64{0x0}; };
  KERN_STRUCT(fc_Attitude) { Float roll = 0.0; Float pitch = 0.0; Float yaw = 0.0; };
  KERN_STRUCT(fc_Command) { Float roll = 0.0; Float pitch = 0.0; Float throttle = 0.0; Float yaw = 0.0; Bool arm = false; };
  KERN_ENUM(fc_FaultCode, kFaultNone, kFaultProtocol, kFaultImu, kFaultTiming, kFaultRate, kFaultTilt);
  KERN_STRUCT(fc_Gains) { Float kp = 0.0; Float ki = 0.0; Float kd = 0.0; Float integral_limit = 0.0; Float output_limit = 0.0; Float d_cutoff_hz = 0.0; };
  KERN_STRUCT(fc_Mix) { Fixed<Float, 4> motor = List<Float>{}; };
  KERN_STRUCT(fc_PT1) { Float cutoff_hz_ = 0.0; Float y_ = 0.0; Bool initialized_ = false; };
  KERN_STRUCT(fc_RC) { Fixed<Int, 16> ch = List<Int>{}; Bool lost = false; Bool failsafe = false; Bool valid = false; Int64 us = Int64{0x0}; };
  KERN_STRUCT(fc_SbusParser) { Fixed<Int, 25> buffer_ = List<Int>{}; Int size_ = 0; Int64 last_us_ = Int64{0x0}; };
  KERN_ENUM(fc_StateBits, kStateArmed, kStateCalibrating, kStateFault, kStateRcValid, kStateImuValid);
  KERN_STRUCT(fc_V3) { Float x = 0.0; Float y = 0.0; Float z = 0.0; };
  KERN_STRUCT(the_fc_ArmState_Result_) { Bool armed = false; Bool armed_now = false; Bool disarmed_now = false; };
  KERN_STRUCT(fc_Filters) { Fixed<fc_PT1, 3> gyro = List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}; Fixed<fc_PT1, 3> accel = List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}; };
  KERN_STRUCT(fc_Imu) { fc_V3 a = fc_V3{0.0, 0.0, 0.0}; fc_V3 g = fc_V3{0.0, 0.0, 0.0}; };
  KERN_STRUCT(fc_PID) { fc_Gains gains_ = fc_Gains{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; fc_PT1 d_filter_ = fc_PT1{0.0, 0.0, false}; Float integral_ = 0.0; Float previous_ = 0.0; Bool have_previous_ = false; };
  KERN_STRUCT(fc_RuntimeOutput) { Fixed<Int, 4> motor_us = List<Int>{1000, 1000, 1000, 1000}; Fixed<Int, 3> attitude_cdeg = List<Int>{}; Int state = 0; fc_FaultCode fault = fc_FaultCode::kFaultNone; Bool armed = false; };
  KERN_STRUCT(fc_Controller) { fc_Attitude attitude = fc_Attitude{0.0, 0.0, 0.0}; fc_PID roll_pid_ = fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}; fc_PID pitch_pid_ = fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}; fc_PID yaw_pid_ = fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}; };
  KERN_STRUCT(fc_RuntimeInput) { fc_Imu raw = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}; fc_RC rc = fc_RC{List<Int>{}, false, false, false, Int64{0x0}}; Int64 now_us = Int64{0x0}; Int dt_us = 1000; Int missed_samples = 0; Bool imu_valid = true; Bool rc_fresh = false; };
  KERN_STRUCT(fc_Runtime) { fc_Filters filters_ = fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}; fc_Controller controller_ = fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}; fc_ArmState arm_ = fc_ArmState{false, false, Int64{0x0}}; Bool calibrated_ = false; Int calibration_count_ = 0; fc_V3 gyro_sum_ = fc_V3{0.0, 0.0, 0.0}; fc_V3 gyro_sq_sum_ = fc_V3{0.0, 0.0, 0.0}; fc_V3 accel_sum_ = fc_V3{0.0, 0.0, 0.0}; fc_V3 gyro_bias_ = fc_V3{0.0, 0.0, 0.0}; Int bad_timing_ = 0; fc_FaultCode fault_ = fc_FaultCode::kFaultNone; Bool armed_ = false; };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Float kManualAttitudeCommandScale = 0.8;
  /// Attitude-priority mixing (as Betaflight/PX4 desaturation): in flight the collective yields to the attitude correction. At full collective (a full-power climb) the motors keep their differential authority by giving up a little thrust instead of losing all torque — the old throttle-priority mix scaled corrections to zero at t = 1, so a full-power takeoff could not hold level and pitched over. On the ground / at idle (t <= 5 %) the collective is never raised for a correction (no hopping), corrections scale down instead.
  KERN_CONST Float kMixFlightCollective = 0.05;
  KERN_CONST Int kEscIdleUs = 1050;
  KERN_CONST Int kEscMaxUs = 2000;
  KERN_CONST Int kEscMinUs = 1000;
  KERN_CONST Float kInnerMaxAttitudeDeg = 40.0;
  /// The number of each value of fc_StateBits, in their order
  KERN_CONST List<Int> fc_StateBits_numbers = List<Int>{1, 2, 4, 8, 16};
  KERN_CONST Int kCalibrationSamples = 2000;
  KERN_CONST Int kMinDtUs = 600;
  KERN_CONST Int kMaxDtUs = 1600;

  // ---- functions ----
  struct magResult { Float Result = 0.0; };
  KERN_PURE magResult mag(fc_V3 v = fc_V3{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float y, Float z) -> Float { return kern::math::sqrt((((x * x) + (y * y)) + (z * z))); }, v.x, v.y, v.z)};
  }

  struct finiteResult { Bool Result = false; };
  KERN_PURE finiteResult finite(fc_V3 v = fc_V3{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, v.x), 0.0), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, v.y), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, v.z), 0.0))};
  }

  struct PT1_newResult { fc_PT1 Result = fc_PT1{0.0, 0.0, false}; };
  KERN_PURE PT1_newResult PT1_new(Float cutoff_hz = 0.0) {
    fc_PT1 self = fc_PT1{0.0, 0.0, false};
    self.cutoff_hz_ = cutoff_hz;
    return {self};
  }

  struct PT1_runResult { Float Result = 0.0; };
  PT1_runResult PT1_run(fc_PT1& self, Float x = 0.0, Float dt = 0.0) {
    Float rc = 0.0;
    Float k = 0.0;
    if (logic_or(logic_or(logic_not(self.initialized_), logic_not(math_compare<">">(dt, 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, dt), 0.0)))) {
      self.y_ = x;
      self.initialized_ = true;
      return {self.y_};
    }
    rc = kern::formula([](Float kPi, Float cutoff_hz_) -> Float { return kern::math::div(1.0, ((2.0 * kPi) * cutoff_hz_)); }, kPi, self.cutoff_hz_);
    k = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dt, Float rc) -> Float { return kern::math::div(dt, (rc + dt)); }, dt, rc), 0.0, 1.0));
    self.y_ = kern::formula([](Float y_, Float k, Float x) -> Float { return (y_ + (k * (x - y_))); }, self.y_, k, x);
    return {self.y_};
  }

  void PT1_reset(fc_PT1& self) {
    self.y_ = 0.0;
    self.initialized_ = false;
  }

  struct Filters_runResult { fc_Imu Result = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}; };
  Filters_runResult Filters_run(fc_Filters& self, fc_Imu s = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}, Float dt = 0.0) {
    n10 = PT1_run(self.accel[0], s.a.x, dt);
    n11 = PT1_run(self.accel[1], s.a.y, dt);
    n12 = PT1_run(self.accel[2], s.a.z, dt);
    n13 = PT1_run(self.gyro[0], s.g.x, dt);
    n14 = PT1_run(self.gyro[1], s.g.y, dt);
    n15 = PT1_run(self.gyro[2], s.g.z, dt);
    return {struct_make<fc_Imu>(struct_make<fc_V3>(n10.Result, n11.Result, n12.Result), struct_make<fc_V3>(n13.Result, n14.Result, n15.Result))};
  }

  void Attitude_reset(fc_Attitude& self, fc_V3 a = fc_V3{0.0, 0.0, 0.0}) {
    self.roll = kern::formula([](Float y, Float z, Float kPi) -> Float { return kern::math::div((kern::math::atan2(y, z) * 180.0), kPi); }, a.y, a.z, kPi);
    self.pitch = kern::formula([](Float x, Float y, Float z, Float kPi) -> Float { return kern::math::div((kern::math::atan2((-x), kern::math::sqrt(((y * y) + (z * z)))) * 180.0), kPi); }, a.x, a.y, a.z, kPi);
    self.yaw = 0.0;
  }

  void Attitude_run(fc_Attitude& self, fc_Imu s = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}, Float dt = 0.0) {
    Float phi = 0.0;
    Float theta = 0.0;
    Float sin_phi = 0.0;
    Float cos_phi = 0.0;
    Float sin_theta = 0.0;
    Float cos_theta = 0.0;
    Float tan_theta = 0.0;
    Float roll_rate = 0.0;
    Float pitch_rate = 0.0;
    Float yaw_rate = 0.0;
    Float n = 0.0;
    Float accel_roll = 0.0;
    Float accel_pitch = 0.0;
    Float tau = 0.0;
    Float k = 0.0;
    phi = kern::formula([](Float roll, Float kPi) -> Float { return kern::math::div((roll * kPi), 180.0); }, self.roll, kPi);
    theta = kern::formula([](Float pitch, Float kPi) -> Float { return kern::math::div((pitch * kPi), 180.0); }, self.pitch, kPi);
    sin_phi = kern::formula([](Float phi) -> Float { return kern::math::sin(phi); }, phi);
    cos_phi = kern::formula([](Float phi) -> Float { return kern::math::cos(phi); }, phi);
    sin_theta = kern::formula([](Float theta) -> Float { return kern::math::sin(theta); }, theta);
    cos_theta = kern::formula([](Float theta) -> Float { return kern::math::cos(theta); }, theta);
    if (math_compare<"<">(kern::formula([](Float cos_theta) -> Float { return kern::math::abs(cos_theta); }, cos_theta), 0.05)) {
      cos_theta = kern::formula([](Float cos_theta) -> Float { return kern::math::copysign(0.05, ((cos_theta == 0.0) ? 1.0 : cos_theta)); }, cos_theta);
    }
    tan_theta = kern::formula([](Float sin_theta, Float cos_theta) -> Float { return kern::math::div(sin_theta, cos_theta); }, sin_theta, cos_theta);
    roll_rate = kern::formula([](Float x, Float sin_phi, Float tan_theta, Float y, Float cos_phi, Float z) -> Float { return ((x + ((sin_phi * tan_theta) * y)) + ((cos_phi * tan_theta) * z)); }, s.g.x, sin_phi, tan_theta, s.g.y, cos_phi, s.g.z);
    pitch_rate = kern::formula([](Float cos_phi, Float y, Float sin_phi, Float z) -> Float { return ((cos_phi * y) - (sin_phi * z)); }, cos_phi, s.g.y, sin_phi, s.g.z);
    yaw_rate = kern::formula([](Float sin_phi, Float cos_theta, Float y, Float cos_phi, Float z) -> Float { return ((kern::math::div(sin_phi, cos_theta) * y) + (kern::math::div(cos_phi, cos_theta) * z)); }, sin_phi, cos_theta, s.g.y, cos_phi, s.g.z);
    self.roll = kern::formula([](Float roll, Float roll_rate, Float dt) -> Float { return (roll + (roll_rate * dt)); }, self.roll, roll_rate, dt);
    self.pitch = kern::formula([](Float pitch, Float pitch_rate, Float dt) -> Float { return (pitch + (pitch_rate * dt)); }, self.pitch, pitch_rate, dt);
    self.yaw = kern::formula([](Float yaw, Float yaw_rate, Float dt) -> Float { return (yaw + (yaw_rate * dt)); }, self.yaw, yaw_rate, dt);
    while (loop(math_compare<">">(self.yaw, 180.0))) {
      self.yaw = kern::formula([](Float yaw) -> Float { return (yaw - 360.0); }, self.yaw);
    }
    while (loop(math_compare<"<">(self.yaw, -180.0))) {
      self.yaw = kern::formula([](Float yaw) -> Float { return (yaw + 360.0); }, self.yaw);
    }
    n = mag(s.a).Result;
    if (logic_and(math_compare<">">(n, 0.8), math_compare<"<">(n, 1.2))) {
      accel_roll = kern::formula([](Float y, Float z, Float kPi) -> Float { return kern::math::div((kern::math::atan2(y, z) * 180.0), kPi); }, s.a.y, s.a.z, kPi);
      accel_pitch = kern::formula([](Float x, Float y, Float z, Float kPi) -> Float { return kern::math::div((kern::math::atan2((-x), kern::math::sqrt(((y * y) + (z * z)))) * 180.0), kPi); }, s.a.x, s.a.y, s.a.z, kPi);
      tau = kern::formula([](Float kPi) -> Float { return kern::math::div(1.0, ((2.0 * kPi) * 0.02)); }, kPi);
      k = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dt, Float tau) -> Float { return kern::math::div(dt, (tau + dt)); }, dt, tau), 0.0, 0.02));
      self.roll = kern::formula([](Float roll, Float k, Float accel_roll) -> Float { return (roll + (k * (accel_roll - roll))); }, self.roll, k, accel_roll);
      self.pitch = kern::formula([](Float pitch, Float k, Float accel_pitch) -> Float { return (pitch + (k * (accel_pitch - pitch))); }, self.pitch, k, accel_pitch);
    }
  }

  struct PID_newResult { fc_PID Result = fc_PID{fc_Gains{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, fc_PT1{0.0, 0.0, false}, 0.0, 0.0, false}; };
  KERN_PURE PID_newResult PID_new(fc_Gains gains = fc_Gains{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}) {
    fc_PID self = fc_PID{fc_Gains{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, fc_PT1{0.0, 0.0, false}, 0.0, 0.0, false};
    self.gains_ = gains;
    self.d_filter_ = PT1_new(gains.d_cutoff_hz).Result;
    return {self};
  }

  struct PID_runResult { Float Result = 0.0; };
  PID_runResult PID_run(fc_PID& self, Float setpoint = 0.0, Float measurement = 0.0, Float dt = 0.0, Bool integrate = false) {
    Float error = 0.0;
    Float derivative = 0.0;
    Float pre = 0.0;
    if (logic_or(logic_or(logic_not(math_compare<">">(dt, 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, setpoint), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, measurement), 0.0)))) {
      return {0.0};
    }
    error = kern::formula([](Float setpoint, Float measurement) -> Float { return (setpoint - measurement); }, setpoint, measurement);
    derivative = kern::formula([](Float have_previous_, Float measurement, Float previous_, Float dt) -> Float { return ((have_previous_ != 0.0) ? kern::math::div((-(measurement - previous_)), dt) : 0.0); }, kern::to<Float>(self.have_previous_), measurement, self.previous_, dt);
    self.previous_ = measurement;
    self.have_previous_ = true;
    n16 = PT1_run(self.d_filter_, derivative, dt);
    derivative = n16.Result;
    pre = kern::formula([](Float kp, Float error, Float integral_, Float kd, Float derivative) -> Float { return (((kp * error) + integral_) + (kd * derivative)); }, self.gains_.kp, error, self.integral_, self.gains_.kd, derivative);
    if (logic_and(integrate, logic_not(logic_or(logic_and(math_compare<">=">(pre, self.gains_.output_limit), math_compare<">">(error, 0.0)), logic_and(math_compare<"<=">(pre, kern::formula([](Float output_limit) -> Float { return (-output_limit); }, self.gains_.output_limit)), math_compare<"<">(error, 0.0)))))) {
      self.integral_ = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float integral_, Float ki, Float error, Float dt) -> Float { return (integral_ + ((ki * error) * dt)); }, self.integral_, self.gains_.ki, error, dt), kern::formula([](Float integral_limit) -> Float { return (-integral_limit); }, self.gains_.integral_limit), self.gains_.integral_limit));
    }
    return {kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float kp, Float error, Float integral_, Float kd, Float derivative) -> Float { return (((kp * error) + integral_) + (kd * derivative)); }, self.gains_.kp, error, self.integral_, self.gains_.kd, derivative), kern::formula([](Float output_limit) -> Float { return (-output_limit); }, self.gains_.output_limit), self.gains_.output_limit))};
  }

  void PID_reset(fc_PID& self) {
    self.integral_ = 0.0;
    self.previous_ = 0.0;
    self.have_previous_ = false;
    PT1_reset(self.d_filter_);
  }

  struct PID_integralResult { Float Result = 0.0; };
  KERN_PURE PID_integralResult PID_integral(const fc_PID& self) {
    return {self.integral_};
  }

  struct sbus_endResult { Bool Result = false; };
  KERN_PURE sbus_endResult sbus_end(Int b = 0) {
    return {logic_or(logic_or(logic_or(logic_or(math_compare<"==">(kern::to<Float>(b), 0.0), math_compare<"==">(kern::to<Float>(b), 4.0)), math_compare<"==">(kern::to<Float>(b), 20.0)), math_compare<"==">(kern::to<Float>(b), 36.0)), math_compare<"==">(kern::to<Float>(b), 52.0))};
  }

  struct decode_sbusResult { Bool Result = false; };
  decode_sbusResult decode_sbus(const Fixed<Int, 25>& p, fc_RC& out, Int p_at = 0) {
    if (logic_or(logic_or(logic_not(math_compare<">=">(kern::to<Float>(p_at), 0.0)), math_compare<"!=">(kern::to<Float>(p[p_at]), 15.0)), logic_not(sbus_end(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 24.0; }, kern::to<Float>(p_at)))]).Result))) {
      return {false};
    }
    out.ch[0] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(a, kern::math::wrap32s(kern::math::shl(a_2, 8.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 2.0; }, kern::to<Float>(p_at)))])));
    out.ch[1] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 3.0), kern::math::wrap32s(kern::math::shl(a_2, 5.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 2.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 3.0; }, kern::to<Float>(p_at)))])));
    out.ch[2] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::bor(kern::math::shr(a, 6.0), kern::math::wrap32s(kern::math::shl(a_2, 2.0))), kern::math::wrap32s(kern::math::shl(a_3, 10.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 3.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 4.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 5.0; }, kern::to<Float>(p_at)))])));
    out.ch[3] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 1.0), kern::math::wrap32s(kern::math::shl(a_2, 7.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 5.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 6.0; }, kern::to<Float>(p_at)))])));
    out.ch[4] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 4.0), kern::math::wrap32s(kern::math::shl(a_2, 4.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 6.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 7.0; }, kern::to<Float>(p_at)))])));
    out.ch[5] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::bor(kern::math::shr(a, 7.0), kern::math::wrap32s(kern::math::shl(a_2, 1.0))), kern::math::wrap32s(kern::math::shl(a_3, 9.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 7.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 8.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 9.0; }, kern::to<Float>(p_at)))])));
    out.ch[6] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 2.0), kern::math::wrap32s(kern::math::shl(a_2, 6.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 9.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 10.0; }, kern::to<Float>(p_at)))])));
    out.ch[7] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 5.0), kern::math::wrap32s(kern::math::shl(a_2, 3.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 10.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 11.0; }, kern::to<Float>(p_at)))])));
    out.ch[8] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(a, kern::math::wrap32s(kern::math::shl(a_2, 8.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 12.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 13.0; }, kern::to<Float>(p_at)))])));
    out.ch[9] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 3.0), kern::math::wrap32s(kern::math::shl(a_2, 5.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 13.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 14.0; }, kern::to<Float>(p_at)))])));
    out.ch[10] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::bor(kern::math::shr(a, 6.0), kern::math::wrap32s(kern::math::shl(a_2, 2.0))), kern::math::wrap32s(kern::math::shl(a_3, 10.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 14.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 15.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 16.0; }, kern::to<Float>(p_at)))])));
    out.ch[11] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 1.0), kern::math::wrap32s(kern::math::shl(a_2, 7.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 16.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 17.0; }, kern::to<Float>(p_at)))])));
    out.ch[12] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 4.0), kern::math::wrap32s(kern::math::shl(a_2, 4.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 17.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 18.0; }, kern::to<Float>(p_at)))])));
    out.ch[13] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::bor(kern::math::shr(a, 7.0), kern::math::wrap32s(kern::math::shl(a_2, 1.0))), kern::math::wrap32s(kern::math::shl(a_3, 9.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 18.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 19.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 20.0; }, kern::to<Float>(p_at)))])));
    out.ch[14] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 2.0), kern::math::wrap32s(kern::math::shl(a_2, 6.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 20.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 21.0; }, kern::to<Float>(p_at)))])));
    out.ch[15] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::band(kern::math::bor(kern::math::shr(a, 5.0), kern::math::wrap32s(kern::math::shl(a_2, 3.0))), 2047.0), 65535.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 21.0; }, kern::to<Float>(p_at)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 22.0; }, kern::to<Float>(p_at)))])));
    out.lost = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 4.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 23.0; }, kern::to<Float>(p_at)))]))))), 0.0);
    out.failsafe = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 8.0); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float a) -> Float { return a + 23.0; }, kern::to<Float>(p_at)))]))))), 0.0);
    out.valid = logic_and(logic_not(out.lost), logic_not(out.failsafe));
    return {true};
  }

  struct SbusParser_feedResult { Bool Result = false; };
  SbusParser_feedResult SbusParser_feed(fc_SbusParser& self, fc_RC& out, Int byte = 0, Int64 us = Int64{0x0}) {
    Int was = 0;
    Int was_2 = 0;
    Int i = 0;
    Bool stop = false;
    Int keep = 0;
    Int count = 0;
    Int to = 0;
    Int from = 0;
    Int last = 0;
    Int k = 0;
    if (logic_and(logic_and(math_compare<"!=">(kern::to<Float>(self.size_), 0.0), int64_less_unsigned(self.last_us_, us)), int64_less_unsigned(Int64{0xbb8}, int64_sub(us, self.last_us_)))) {
      self.size_ = 0;
    }
    self.last_us_ = us;
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.size_), 0.0))) {
      if (math_compare<"!=">(kern::to<Float>(byte), 15.0)) {
        return {false};
      }
      was = self.size_;
      self.size_ = kern::to<Int>(kern::formula([](Float size_) -> Float { return (size_ + 1.0); }, kern::to<Float>(self.size_)));
      self.buffer_[was] = byte;
      return {false};
    }
    was_2 = self.size_;
    self.size_ = kern::to<Int>(kern::formula([](Float size_) -> Float { return (size_ + 1.0); }, kern::to<Float>(self.size_)));
    self.buffer_[was_2] = byte;
    if (math_compare<">">(kern::formula([](Float size_, Float a) -> Float { return ((size_ < 0.0) != (a < 0.0) ? (a < 0.0) : (size_ < a)); }, kern::to<Float>(self.size_), kern::to<Float>(list_length<Int>(self.buffer_))), 0.5)) {
      return {false};
    }
    self.size_ = 0;
    n17 = decode_sbus(self.buffer_, out, 0);
    if (n17.Result) {
      out.us = us;
      return {true};
    }
    i = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Int>(self.buffer_))), 0.5)))) {
      if (math_compare<"==">(kern::to<Float>(self.buffer_[i]), 15.0)) {
        keep = kern::to<Int>(kern::formula([](Float a, Float i) -> Float { return (a - i); }, kern::to<Float>(list_length<Int>(self.buffer_)), kern::to<Float>(i)));
        count = keep;
        to = 0;
        from = i;
        last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)));
        if (math_compare<"<=">(kern::to<Float>(to), kern::to<Float>(from))) {
          for (auto it18 = flow_for(n18, 0, last); it18.next();) {
            self.buffer_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(to), kern::to<Float>(n18.index)))] = list_get<Int>(self.buffer_, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(from), kern::to<Float>(n18.index))));
          }
        } else {
          k = last;
          while (loop(math_compare<">=">(kern::to<Float>(k), 0.0))) {
            self.buffer_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(to), kern::to<Float>(k)))] = list_get<Int>(self.buffer_, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(from), kern::to<Float>(k))));
            k = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(k)));
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

  struct centeredResult { Float Result = 0.0; };
  KERN_PURE centeredResult centered(Int v = 0) {
    return {kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v) -> Float { return kern::math::div((v - 992.0), 820.0); }, kern::to<Float>(v)), -1.0, 1.0))};
  }

  struct throttleResult { Float Result = 0.0; };
  KERN_PURE throttleResult throttle(Int v = 0) {
    return {kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v) -> Float { return kern::math::div((v - 172.0), (1811.0 - 172.0)); }, kern::to<Float>(v)), 0.0, 1.0))};
  }

  struct shapeResult { Float Result = 0.0; };
  KERN_PURE shapeResult shape(Float x = 0.0, Float deadband = 0.0, Float expo = 0.0) {
    Float a = 0.0;
    Float t = 0.0;
    Float v = 0.0;
    a = kern::formula([](Float x) -> Float { return kern::math::abs(x); }, x);
    if (math_compare<"<=">(a, deadband)) {
      return {0.0};
    }
    t = kern::formula([](Float a, Float deadband) -> Float { return kern::math::div((a - deadband), (1.0 - deadband)); }, a, deadband);
    v = kern::formula([](Float t, Float expo) -> Float { return ((t * (1.0 - expo)) + (((t * t) * t) * expo)); }, t, expo);
    return {kern::formula([](Float v, Float x) -> Float { return kern::math::copysign((v < 0.0 ? 0.0 : (1.0 < v ? 1.0 : v)), x); }, v, x)};
  }

  struct finite_2Result { Bool Result = false; };
  KERN_PURE finite_2Result finite_2(fc_Command c = fc_Command{0.0, 0.0, 0.0, 0.0, false}) {
    return {logic_and(logic_and(logic_and(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, c.roll), 0.0), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, c.pitch), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, c.throttle), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, c.yaw), 0.0))};
  }

  struct sanitizeResult { fc_Command Result = fc_Command{0.0, 0.0, 0.0, 0.0, false}; };
  KERN_PURE sanitizeResult sanitize(fc_Command c = fc_Command{0.0, 0.0, 0.0, 0.0, false}) {
    fc_Command c_ = fc_Command{0.0, 0.0, 0.0, 0.0, false};
    c_ = c;
    c_.roll = kern::formula([](Float roll) -> Float { return (roll < (-1.0) ? (-1.0) : (1.0 < roll ? 1.0 : roll)); }, c_.roll);
    c_.pitch = kern::formula([](Float pitch) -> Float { return (pitch < (-1.0) ? (-1.0) : (1.0 < pitch ? 1.0 : pitch)); }, c_.pitch);
    c_.throttle = kern::formula([](Float throttle) -> Float { return (throttle < 0.0 ? 0.0 : (1.0 < throttle ? 1.0 : throttle)); }, c_.throttle);
    c_.yaw = kern::formula([](Float yaw) -> Float { return (yaw < (-1.0) ? (-1.0) : (1.0 < yaw ? 1.0 : yaw)); }, c_.yaw);
    return {c_};
  }

  struct commandResult { fc_Command Result = fc_Command{0.0, 0.0, 0.0, 0.0, false}; };
  KERN_PURE commandResult command(const fc_RC& r) {
    return {struct_make<fc_Command>(kern::formula([](Float the_result, Float kManualAttitudeCommandScale) -> Float { return (the_result * kManualAttitudeCommandScale); }, shape(centered(r.ch[0]).Result, 0.035, 0.3).Result, kManualAttitudeCommandScale), kern::formula([](Float the_result, Float kManualAttitudeCommandScale) -> Float { return ((-the_result) * kManualAttitudeCommandScale); }, shape(centered(r.ch[1]).Result, 0.035, 0.3).Result, kManualAttitudeCommandScale), throttle(r.ch[2]).Result, shape(centered(r.ch[3]).Result, 0.045, 0.2).Result, math_compare<">">(kern::to<Float>(r.ch[4]), 1300.0))};
  }

  struct mixResult { fc_Mix Result = fc_Mix{List<Float>{}}; };
  KERN_PURE mixResult mix(Float t = 0.0, Float roll = 0.0, Float pitch = 0.0, Float yaw = 0.0) {
    Float t_ = 0.0;
    Fixed<Float, 4> correction = Fixed<Float, 4>{};
    Float cmax = 0.0;
    Float cmin = 0.0;
    Float v = 0.0;
    Float scale_factor = 0.0;
    Float range = 0.0;
    Float hi = 0.0;
    Float lo = 0.0;
    Float v_2 = 0.0;
    fc_Mix out = fc_Mix{List<Float>{}};
    t_ = t;
    t_ = kern::formula([](Float t_) -> Float { return (t_ < 0.0 ? 0.0 : (1.0 < t_ ? 1.0 : t_)); }, t_);
    correction = list_make<Float, "4">(kern::formula([](Float pitch, Float roll, Float yaw) -> Float { return ((pitch - roll) - (0.65 * yaw)); }, pitch, roll, yaw), kern::formula([](Float pitch, Float roll, Float yaw) -> Float { return ((pitch + roll) + (0.65 * yaw)); }, pitch, roll, yaw), kern::formula([](Float pitch, Float roll, Float yaw) -> Float { return (((-pitch) + roll) - (0.65 * yaw)); }, pitch, roll, yaw), kern::formula([](Float pitch, Float roll, Float yaw) -> Float { return (((-pitch) - roll) + (0.65 * yaw)); }, pitch, roll, yaw));
    cmax = correction[0];
    cmin = correction[0];
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(correction))))); it19.next();) {
      v = correction[n19.index];
      cmax = kern::formula([](Float cmax, Float v) -> Float { return kern::math::max(cmax, v); }, cmax, v);
      cmin = kern::formula([](Float cmin, Float v) -> Float { return kern::math::min(cmin, v); }, cmin, v);
    }
    scale_factor = 1.0;
    if (math_compare<">">(t_, kMixFlightCollective)) {
      range = kern::formula([](Float cmax, Float cmin) -> Float { return (cmax - cmin); }, cmax, cmin);
      if (math_compare<">">(range, 1.0)) {
        scale_factor = kern::formula([](Float range) -> Float { return kern::math::div(1.0, range); }, range);
      }
      hi = kern::formula([](Float cmax, Float scale_factor) -> Float { return (cmax * scale_factor); }, cmax, scale_factor);
      lo = kern::formula([](Float cmin, Float scale_factor) -> Float { return (cmin * scale_factor); }, cmin, scale_factor);
      if (math_compare<">">(kern::formula([](Float t_, Float hi) -> Float { return (t_ + hi); }, t_, hi), 1.0)) {
        t_ = kern::formula([](Float hi) -> Float { return (1.0 - hi); }, hi);
      }
      if (math_compare<"<">(kern::formula([](Float t_, Float lo) -> Float { return (t_ + lo); }, t_, lo), 0.0)) {
        t_ = kern::formula([](Float lo) -> Float { return (-lo); }, lo);
      }
    } else {
      for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(correction))))); it20.next();) {
        v_2 = correction[n20.index];
        if (math_compare<">">(v_2, 0.0)) {
          scale_factor = kern::formula([](Float scale_factor, Float t_, Float v_2) -> Float { return kern::math::min(scale_factor, kern::math::div((1.0 - t_), v_2)); }, scale_factor, t_, v_2);
        } else if (math_compare<"<">(v_2, 0.0)) {
          scale_factor = kern::formula([](Float scale_factor, Float t_, Float v_2) -> Float { return kern::math::min(scale_factor, kern::math::div(t_, (-v_2))); }, scale_factor, t_, v_2);
        }
      }
      scale_factor = kern::formula([](Float scale_factor) -> Float { return (scale_factor < 0.0 ? 0.0 : (1.0 < scale_factor ? 1.0 : scale_factor)); }, scale_factor);
    }
    out = fc_Mix{List<Float>{}};
    for (auto it21 = flow_for(n21, 0, 3); it21.next();) {
      out.motor[n21.index] = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float t_, Float scale_factor, Float a) -> Float { return (t_ + (scale_factor * a)); }, t_, scale_factor, correction[n21.index]), 0.0, 1.0));
    }
    return {out};
  }

  struct pulseResult { Int Result = 0; };
  KERN_PURE pulseResult pulse(Float value = 0.0, Bool armed = false) {
    return {kern::to<Int>(kern::formula([](Float armed, Float kEscIdleUs, Float value, Float kEscMaxUs, Float kEscMinUs) -> Float { return ((armed != 0.0) ? kern::math::trunc(((kEscIdleUs + ((value < 0.0 ? 0.0 : (1.0 < value ? 1.0 : value)) * (kEscMaxUs - kEscIdleUs))) + 0.5)) : kEscMinUs); }, kern::to<Float>(armed), kern::to<Float>(kEscIdleUs), value, kern::to<Float>(kEscMaxUs), kern::to<Float>(kEscMinUs)))};
  }

  struct ArmState_runResult { the_fc_ArmState_Result_ Result = the_fc_ArmState_Result_{false, false, false}; };
  ArmState_runResult ArmState_run(fc_ArmState& self, Int64 us = Int64{0x0}, Bool control_valid = false, fc_Command cmd = fc_Command{0.0, 0.0, 0.0, 0.0, false}, Bool imu_ok = false, Float roll = 0.0, Float pitch = 0.0) {
    the_fc_ArmState_Result_ result = the_fc_ArmState_Result_{false, false, false};
    Bool ok = false;
    result = struct_make<the_fc_ArmState_Result_>(self.armed_, false, false);
    if (logic_not(control_valid)) {
      result.disarmed_now = self.armed_;
      self.armed_ = false;
      self.saw_low_ = false;
      self.since_us_ = Int64{0x0};
      result.armed = false;
      return {result};
    }
    if (logic_not(cmd.arm)) {
      self.saw_low_ = true;
      self.since_us_ = Int64{0x0};
      result.disarmed_now = self.armed_;
      self.armed_ = false;
      result.armed = false;
      return {result};
    }
    if (self.armed_) {
      return {result};
    }
    ok = logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(self.saw_low_, math_compare<"<=">(cmd.throttle, 0.035)), math_compare<"<">(kern::formula([](Float roll) -> Float { return kern::math::abs(roll); }, cmd.roll), 0.12)), math_compare<"<">(kern::formula([](Float pitch) -> Float { return kern::math::abs(pitch); }, cmd.pitch), 0.12)), math_compare<"<">(kern::formula([](Float yaw) -> Float { return kern::math::abs(yaw); }, cmd.yaw), 0.15)), math_compare<"<">(kern::formula([](Float roll) -> Float { return kern::math::abs(roll); }, roll), 20.0)), math_compare<"<">(kern::formula([](Float pitch) -> Float { return kern::math::abs(pitch); }, pitch), 20.0)), imu_ok);
    if (logic_not(ok)) {
      self.since_us_ = Int64{0x0};
      return {result};
    }
    if (logic_not(logic_not(int64_equal(self.since_us_, Int64{0x0})))) {
      self.since_us_ = us;
    }
    if (logic_not(int64_less_unsigned(int64_sub(us, self.since_us_), Int64{0xf4240}))) {
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

  struct Controller_runResult { fc_Mix Result = fc_Mix{List<Float>{}}; };
  Controller_runResult Controller_run(fc_Controller& self, fc_Imu s = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}, fc_Command cmd = fc_Command{0.0, 0.0, 0.0, 0.0, false}, Float dt = 0.0, Bool integrate = false) {
    Float kAngleToRate = 0.0;
    Float roll_rate = 0.0;
    Float pitch_rate = 0.0;
    Float yaw_rate = 0.0;
    kAngleToRate = 4.5;
    roll_rate = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float roll, Float kInnerMaxAttitudeDeg, Float roll_2, Float kAngleToRate) -> Float { return (((roll * kInnerMaxAttitudeDeg) - roll_2) * kAngleToRate); }, cmd.roll, kInnerMaxAttitudeDeg, self.attitude.roll, kAngleToRate), -240.0, 240.0));
    pitch_rate = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float pitch, Float kInnerMaxAttitudeDeg, Float pitch_2, Float kAngleToRate) -> Float { return (((pitch * kInnerMaxAttitudeDeg) - pitch_2) * kAngleToRate); }, cmd.pitch, kInnerMaxAttitudeDeg, self.attitude.pitch, kAngleToRate), -240.0, 240.0));
    yaw_rate = kern::formula([](Float yaw) -> Float { return (yaw * 180.0); }, cmd.yaw);
    n22 = PID_run(self.roll_pid_, roll_rate, s.g.x, dt, integrate);
    n23 = PID_run(self.pitch_pid_, pitch_rate, s.g.y, dt, integrate);
    n24 = PID_run(self.yaw_pid_, yaw_rate, s.g.z, dt, integrate);
    return {mix(cmd.throttle, n22.Result, n23.Result, n24.Result).Result};
  }

  void Runtime_reset(fc_Runtime& self) {
    self.filters_ = struct_make<fc_Filters>(List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}});
    self.controller_ = fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}};
    self.arm_ = fc_ArmState{false, false, Int64{0x0}};
    self.calibrated_ = false;
    self.calibration_count_ = 0;
    self.gyro_sum_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
    self.gyro_sq_sum_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
    self.accel_sum_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
    self.gyro_bias_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
    self.bad_timing_ = 0;
    self.fault_ = fc_FaultCode::kFaultNone;
    self.armed_ = false;
  }

  struct Runtime_newResult { fc_Runtime Result = fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}; };
  Runtime_newResult Runtime_new() {
    fc_Runtime self = fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false};
    Runtime_reset(self);
    return {self};
  }

  struct Runtime_to_cdegResult { Int Result = 0; };
  KERN_PURE Runtime_to_cdegResult Runtime_to_cdeg(Float degrees = 0.0) {
    Float value = 0.0;
    value = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float degrees) -> Float { return (degrees * 100.0); }, degrees), -32767.0, 32767.0));
    return {kern::to<Int>(kern::formula([](Float value) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(value), 65535.0), 32768.0) - 32768.0); }, value))};
  }

  struct Runtime_finalizeResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false}; };
  KERN_PURE Runtime_finalizeResult Runtime_finalize(const fc_Runtime& self, fc_RuntimeOutput out = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false}, Bool control_ok = false, Bool imu_ok = false) {
    fc_RuntimeOutput out_ = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false};
    out_ = out;
    out_.armed = logic_and(self.armed_, math_compare<"==">(kern::to<Float>(enum_index<fc_FaultCode>(self.fault_)), kern::to<Float>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone))));
    out_.fault = self.fault_;
    if (out_.armed) {
      out_.state = kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::bor(state, a); }, kern::to<Float>(out_.state), kern::to<Float>(list_get<Int>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateArmed)))));
    }
    if (logic_not(self.calibrated_)) {
      out_.state = kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::bor(state, a); }, kern::to<Float>(out_.state), kern::to<Float>(list_get<Int>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateCalibrating)))));
    }
    if (math_compare<"!=">(kern::to<Float>(enum_index<fc_FaultCode>(self.fault_)), kern::to<Float>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
      out_.state = kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::bor(state, a); }, kern::to<Float>(out_.state), kern::to<Float>(list_get<Int>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateFault)))));
      out_.state = kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::bor(state, kern::math::wrap32s(kern::math::shl(a, 8.0))); }, kern::to<Float>(out_.state), kern::to<Float>(enum_index<fc_FaultCode>(self.fault_))));
      out_.motor_us = list_make<Int, "4">(kEscMinUs, kEscMinUs, kEscMinUs, kEscMinUs);
    }
    if (control_ok) {
      out_.state = kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::bor(state, a); }, kern::to<Float>(out_.state), kern::to<Float>(list_get<Int>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateRcValid)))));
    }
    if (imu_ok) {
      out_.state = kern::to<Int>(kern::formula([](Float state, Float a) -> Float { return kern::math::bor(state, a); }, kern::to<Float>(out_.state), kern::to<Float>(list_get<Int>(fc_StateBits_numbers, enum_index<fc_StateBits>(fc_StateBits::kStateImuValid)))));
    }
    out_.attitude_cdeg = list_make<Int, "3">(Runtime_to_cdeg(self.controller_.attitude.roll).Result, Runtime_to_cdeg(self.controller_.attitude.pitch).Result, Runtime_to_cdeg(self.controller_.attitude.yaw).Result);
    return {out_};
  }

  void Runtime_set_fault(fc_Runtime& self, fc_FaultCode fault = fc_FaultCode::kFaultNone) {
    if (math_compare<"==">(kern::to<Float>(enum_index<fc_FaultCode>(self.fault_)), kern::to<Float>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
      self.fault_ = fault;
    }
    self.armed_ = false;
  }

  void Runtime_calibrate(fc_Runtime& self, const fc_Imu& sample) {
    Float k = 0.0;
    fc_V3 accel_mean = fc_V3{0.0, 0.0, 0.0};
    fc_V3 gyro_sd = fc_V3{0.0, 0.0, 0.0};
    Bool still = false;
    self.gyro_sum_.x = kern::formula([](Float x, Float x_2) -> Float { return (x + x_2); }, self.gyro_sum_.x, sample.g.x);
    self.gyro_sum_.y = kern::formula([](Float y, Float y_2) -> Float { return (y + y_2); }, self.gyro_sum_.y, sample.g.y);
    self.gyro_sum_.z = kern::formula([](Float z, Float z_2) -> Float { return (z + z_2); }, self.gyro_sum_.z, sample.g.z);
    self.gyro_sq_sum_.x = kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * x_2)); }, self.gyro_sq_sum_.x, sample.g.x);
    self.gyro_sq_sum_.y = kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * y_2)); }, self.gyro_sq_sum_.y, sample.g.y);
    self.gyro_sq_sum_.z = kern::formula([](Float z, Float z_2) -> Float { return (z + (z_2 * z_2)); }, self.gyro_sq_sum_.z, sample.g.z);
    self.accel_sum_.x = kern::formula([](Float x, Float x_2) -> Float { return (x + x_2); }, self.accel_sum_.x, sample.a.x);
    self.accel_sum_.y = kern::formula([](Float y, Float y_2) -> Float { return (y + y_2); }, self.accel_sum_.y, sample.a.y);
    self.accel_sum_.z = kern::formula([](Float z, Float z_2) -> Float { return (z + z_2); }, self.accel_sum_.z, sample.a.z);
    self.calibration_count_ = kern::to<Int>(kern::formula([](Float calibration_count_) -> Float { return kern::math::wrap32(calibration_count_ + 1.0); }, kern::to<Float>(self.calibration_count_)));
    if (math_compare<"<">(kern::to<Float>(self.calibration_count_), kern::to<Float>(kCalibrationSamples))) {
      return;
    }
    k = kern::formula([](Float calibration_count_) -> Float { return kern::math::div(1.0, calibration_count_); }, kern::to<Float>(self.calibration_count_));
    self.gyro_bias_ = struct_make<fc_V3>(kern::formula([](Float x, Float k) -> Float { return (x * k); }, self.gyro_sum_.x, k), kern::formula([](Float y, Float k) -> Float { return (y * k); }, self.gyro_sum_.y, k), kern::formula([](Float z, Float k) -> Float { return (z * k); }, self.gyro_sum_.z, k));
    accel_mean = struct_make<fc_V3>(kern::formula([](Float x, Float k) -> Float { return (x * k); }, self.accel_sum_.x, k), kern::formula([](Float y, Float k) -> Float { return (y * k); }, self.accel_sum_.y, k), kern::formula([](Float z, Float k) -> Float { return (z * k); }, self.accel_sum_.z, k));
    gyro_sd = struct_make<fc_V3>(kern::formula([](Float x, Float k, Float x_2) -> Float { return kern::math::sqrt(kern::math::max(0.0, ((x * k) - (x_2 * x_2)))); }, self.gyro_sq_sum_.x, k, self.gyro_bias_.x), kern::formula([](Float y, Float k, Float y_2) -> Float { return kern::math::sqrt(kern::math::max(0.0, ((y * k) - (y_2 * y_2)))); }, self.gyro_sq_sum_.y, k, self.gyro_bias_.y), kern::formula([](Float z, Float k, Float z_2) -> Float { return kern::math::sqrt(kern::math::max(0.0, ((z * k) - (z_2 * z_2)))); }, self.gyro_sq_sum_.z, k, self.gyro_bias_.z));
    still = logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(math_compare<"<">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, self.gyro_bias_.x), 15.0), math_compare<"<">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, self.gyro_bias_.y), 15.0)), math_compare<"<">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, self.gyro_bias_.z), 15.0)), math_compare<"<">(gyro_sd.x, 0.8)), math_compare<"<">(gyro_sd.y, 0.8)), math_compare<"<">(gyro_sd.z, 0.8)), math_compare<">">(mag(accel_mean).Result, 0.85)), math_compare<"<">(mag(accel_mean).Result, 1.15));
    if (logic_not(still)) {
      Runtime_set_fault(self, fc_FaultCode::kFaultImu);
      return;
    }
    self.filters_ = struct_make<fc_Filters>(List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}});
    self.controller_ = fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}};
    Attitude_reset(self.controller_.attitude, accel_mean);
    self.calibrated_ = true;
  }

  struct Runtime_step_commandResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false}; };
  Runtime_step_commandResult Runtime_step_command(fc_Runtime& self, const fc_RuntimeInput& input, fc_Command cmd = fc_Command{0.0, 0.0, 0.0, 0.0, false}, Bool command_valid = false) {
    fc_Command cmd_ = fc_Command{0.0, 0.0, 0.0, 0.0, false};
    fc_RuntimeOutput out = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false};
    Float dt = 0.0;
    fc_Imu corrected = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}};
    fc_Imu imu = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}};
    Bool accel_ok = false;
    the_fc_ArmState_Result_ arm_result = the_fc_ArmState_Result_{false, false, false};
    fc_Mix mixed = fc_Mix{List<Float>{}};
    cmd_ = cmd;
    out = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false};
    if (math_compare<"!=">(kern::to<Float>(enum_index<fc_FaultCode>(self.fault_)), kern::to<Float>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
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
    cmd_ = logic_select<fc_Command>(command_valid, sanitize(cmd_).Result, struct_make<fc_Command>(0.0, 0.0, 0.0, 0.0, false));
    dt = kern::formula([](Float dt_us) -> Float { return (((dt_us > 0.0) && (dt_us < 100000.0)) ? (dt_us * 1e-06) : 0.001); }, kern::to<Float>(input.dt_us));
    if (logic_not(self.calibrated_)) {
      Runtime_calibrate(self, input.raw);
      n25 = ArmState_run(self.arm_, input.now_us, command_valid, cmd_, false, 0.0, 0.0);
      return {Runtime_finalize(self, out, command_valid, true).Result};
    }
    if (self.armed_) {
      if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(input.dt_us), kern::to<Float>(kMinDtUs)), math_compare<">">(kern::to<Float>(input.dt_us), kern::to<Float>(kMaxDtUs))), math_compare<">">(kern::to<Float>(input.missed_samples), 2.0))) {
        self.bad_timing_ = kern::to<Int>(kern::formula([](Float bad_timing_) -> Float { return kern::math::wrap32(bad_timing_ + 1.0); }, kern::to<Float>(self.bad_timing_)));
        if (math_compare<">=">(kern::to<Float>(self.bad_timing_), 5.0)) {
          Runtime_set_fault(self, fc_FaultCode::kFaultTiming);
        }
      } else {
        self.bad_timing_ = 0;
      }
    } else {
      self.bad_timing_ = 0;
    }
    if (math_compare<"!=">(kern::to<Float>(enum_index<fc_FaultCode>(self.fault_)), kern::to<Float>(enum_index<fc_FaultCode>(fc_FaultCode::kFaultNone)))) {
      return {Runtime_finalize(self, out, command_valid, true).Result};
    }
    corrected = input.raw;
    corrected.g.x = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, corrected.g.x, self.gyro_bias_.x);
    corrected.g.y = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, corrected.g.y, self.gyro_bias_.y);
    corrected.g.z = kern::formula([](Float z, Float z_2) -> Float { return (z - z_2); }, corrected.g.z, self.gyro_bias_.z);
    n26 = Filters_run(self.filters_, corrected, dt);
    imu = n26.Result;
    if (logic_or(logic_or(logic_or(logic_or(logic_not(finite(imu.a).Result), logic_not(finite(imu.g).Result)), math_compare<">">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, imu.g.x), 1750.0)), math_compare<">">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, imu.g.y), 1750.0)), math_compare<">">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, imu.g.z), 1750.0))) {
      Runtime_set_fault(self, fc_FaultCode::kFaultRate);
      return {Runtime_finalize(self, out, command_valid, true).Result};
    }
    Attitude_run(self.controller_.attitude, imu, dt);
    accel_ok = logic_and(math_compare<">">(mag(imu.a).Result, 0.7), math_compare<"<">(mag(imu.a).Result, 1.3));
    n27 = ArmState_run(self.arm_, input.now_us, command_valid, cmd_, accel_ok, self.controller_.attitude.roll, self.controller_.attitude.pitch);
    arm_result = n27.Result;
    self.armed_ = arm_result.armed;
    if (logic_not(self.armed_)) {
      Controller_reset(self.controller_);
    }
    if (self.armed_) {
      mixed = struct_make<fc_Mix>(List<Float>{});
      if (math_compare<"<=">(cmd_.throttle, 0.02)) {
        Controller_reset(self.controller_);
      } else {
        n28 = Controller_run(self.controller_, imu, cmd_, dt, math_compare<">">(cmd_.throttle, 0.05));
        mixed = n28.Result;
      }
      for (auto it29 = flow_for(n29, 0, 3); it29.next();) {
        out.motor_us[n29.index] = pulse(mixed.motor[n29.index], true).Result;
      }
    }
    return {Runtime_finalize(self, out, command_valid, true).Result};
  }

  struct Runtime_stepResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false}; };
  /// Receiver/SBUS is only an input adapter. Once decoded, the flight runtime operates on a physical normalized Command. Higher-level controllers use step_command() directly instead of fabricating receiver frames.
  Runtime_stepResult Runtime_step(fc_Runtime& self, const fc_RuntimeInput& input) {
    fc_RC rc = fc_RC{List<Int>{}, false, false, false, Int64{0x0}};
    rc = input.rc;
    rc.valid = logic_and(rc.valid, input.rc_fresh);
    n30 = Runtime_step_command(self, input, command(rc).Result, rc.valid);
    return {n30.Result};
  }

  struct Runtime_calibratedResult { Bool Result = false; };
  KERN_PURE Runtime_calibratedResult Runtime_calibrated(const fc_Runtime& self) {
    return {self.calibrated_};
  }

  struct Runtime_armedResult { Bool Result = false; };
  KERN_PURE Runtime_armedResult Runtime_armed(const fc_Runtime& self) {
    return {self.armed_};
  }

  struct Runtime_faultResult { fc_FaultCode Result = fc_FaultCode::kFaultNone; };
  KERN_PURE Runtime_faultResult Runtime_fault(const fc_Runtime& self) {
    return {self.fault_};
  }

  struct Runtime_fault_nameResult { Text Result = ""; };
  KERN_PURE Runtime_fault_nameResult Runtime_fault_name(fc_FaultCode fault = fc_FaultCode::kFaultNone) {
    Int selected = 0;
    selected = enum_index<fc_FaultCode>(fault);
    if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
      return {"none"};
    } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
      return {"protocol"};
    } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
      return {"imu"};
    } else if (math_compare<"==">(kern::to<Float>(selected), 3.0)) {
      return {"deadline"};
    } else if (math_compare<"==">(kern::to<Float>(selected), 4.0)) {
      return {"rate"};
    } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
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
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  PID_runResult n22;
  PID_runResult n23;
  PID_runResult n24;
  ArmState_runResult n25;
  Filters_runResult n26;
  ArmState_runResult n27;
  Controller_runResult n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  Runtime_step_commandResult n30;
};
