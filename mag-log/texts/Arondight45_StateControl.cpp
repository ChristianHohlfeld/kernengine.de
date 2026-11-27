struct Arondight45_StateControl : virtual kern::Library, virtual Arondight45_DroneFC_Core {
  // ---- types ----
  KERN_STRUCT(fc_StateControllerDebug) { f32 desired_forward_mps = 0.0f; f32 measured_forward_mps = 0.0f; f32 desired_right_mps = 0.0f; f32 measured_right_mps = 0.0f; f32 target_yaw_deg = 0.0f; f32 measured_yaw_deg = 0.0f; f32 target_agl_m = 0.0f; f32 measured_agl_m = 0.0f; f32 target_vz_mps = 0.0f; f32 throttle = 0.0f; f32 roll_command = 0.0f; f32 pitch_command = 0.0f; f32 yaw_command = 0.0f; f32 forward_accel_mps2 = 0.0f; f32 right_accel_mps2 = 0.0f; f32 vertical_accel_mps2 = 0.0f; };
  KERN_STRUCT(fc_StateIntent) { f32 right_mps = 0.0f; f32 forward_mps = 0.0f; f32 yaw_rate_dps = 0.0f; f32 body_pitch_deg = 0.0f; f32 clearance_m = 2.0f; bool arm = false; bool game_mode = false; };
  KERN_STRUCT(fc_NavigationState) { fc_V3 velocity_world_mps = fc_V3{0.0f, 0.0f, 0.0f}; f32 agl_m = 0.0f; bool valid = false; bool velocity_valid = false; bool agl_valid = false; f32 heading_deg = 0.0f; bool heading_valid = false; };
  KERN_STRUCT(fc_StateController) { f32 agl_estimate_m_ = 0.0f; bool agl_estimate_valid_ = false; bool active_ = false; bool heading_source_valid_ = false; bool manual_pitch_active_ = false; bool acceleration_estimator_valid_ = false; bool horizontal_target_initialized_ = false; bool vertical_accel_initialized_ = false; f32 acceleration_sample_dt_s_ = 0.0f; fc_V3 previous_velocity_world_mps_ = fc_V3{0.0f, 0.0f, 0.0f}; fc_V3 measured_acceleration_world_mps2_ = fc_V3{0.0f, 0.0f, 0.0f}; f32 horizontal_integral_forward_mps2_ = 0.0f; f32 horizontal_integral_right_mps2_ = 0.0f; f32 commanded_forward_mps_ = 0.0f; f32 commanded_right_mps_ = 0.0f; f32 vertical_accel_state_mps2_ = 0.0f; f32 target_yaw_deg_ = 0.0f; f32 hover_trim_ = 0.39f; fc_StateControllerDebug debug_ = fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}; };
  KERN_STRUCT(fc_StateRuntimeInput) { fc_RuntimeInput flight = fc_RuntimeInput{fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}, fc_RC{List<u16>{}, false, false, false, 0}, 0, 1000, 0, true, false}; fc_NavigationState navigation = fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}; };
  KERN_STRUCT(fc_StateRuntime) { fc_Runtime runtime_ = fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}; fc_StateController state_controller_ = fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}; f32 last_yaw_deg_ = 0.0f; bool game_active_ = false; };

  // ---- variables ----
  KERN_CONST i32 kStateClearanceChannel = 5;
  KERN_CONST i32 kStateBodyPitchChannel = 7;
  KERN_CONST f32 kStateMaxBodyPitchDeg = 25.0f;
  /// Shared Production/HIL/WASM velocity envelope. GAME phone settings encode a fraction of this real velocity target; the FC remains the only authority that turns velocity error into bounded acceleration, attitude and motor commands.
  KERN_CONST f32 kStateMaxHorizontalSpeedMps = 25.0f;
  KERN_CONST f32 kStateMaxYawRateDps = 140.0f;
  KERN_CONST f32 kStateMinClearanceM = 0.5f;
  KERN_CONST f32 kStateMaxClearanceM = 120.0f;
  KERN_CONST i32 kStateModeChannel = 6;
  KERN_CONST f32 kInitialHoverThrottle = 0.39f;
  KERN_CONST f32 kPi = 3.1415927f;
  KERN_CONST f32 kManualPitchOverrideThresholdDeg = 0.5f;
  KERN_CONST f32 kMaxNavigationAccelSampleMps2 = 15.0f;
  KERN_CONST f32 kMeasuredAccelerationFilterTauS = 0.06f;
  /// Braking used by the kinematic altitude law: a climb is braked by cutting thrust (bounded by the descent reserve), a sink by thrust surplus.
  KERN_CONST f32 kClimbBrakeAccelerationMps2 = 6.0f;
  KERN_CONST f32 kDescentBrakeAccelerationMps2 = 4.0f;
  KERN_CONST f32 kVerticalVelocityGain = 4.0f;
  KERN_CONST f32 kVerticalDescentVelocityGain = 8.0f;
  KERN_CONST f32 kAglToVerticalSpeed = 2.0f;
  KERN_CONST f32 kMaxVerticalDescentSpeedMps = 9.0f;
  KERN_CONST f32 kMaxVerticalSpeedMps = 30.0f;
  KERN_CONST f32 kMaxVerticalDescentAccelerationMps2 = 6.8f;
  KERN_CONST f32 kMaxVerticalAccelerationMps2 = 50.0f;
  KERN_CONST f32 kVerticalJerkLimitMps3 = 1200.0f;
  KERN_CONST f32 kGravityMps2 = 9.80665f;
  /// Keep enough collective thrust during aggressive descent for the inner attitude loop to retain real motor/torque authority. 0.5 m/s² was nearly free-fall and allowed the physical airframe to tumble well beyond the bounded attitude target. 3.0 m/s² (≈30 % hover thrust) keeps the props spinning with differential authority while sinking hard.
  KERN_CONST f32 kMinSpecificUpMps2 = 3.0f;
  KERN_CONST f32 kMaxSpecificUpMps2 = 60.0f;
  KERN_CONST f32 kHorizontalIntegralNeutralTargetMps = 0.05f;
  KERN_CONST f32 kHorizontalNeutralDampingScale = 1.0f;
  KERN_CONST f32 kHorizontalVelocityGain = 0.8f;
  KERN_CONST f32 kHorizontalAccelerationDamping = 0.55f;
  KERN_CONST f32 kMaxHorizontalAccelerationMps2 = 7.5f;
  KERN_CONST f32 kMaxAutoTranslationTiltTangent = 0.46630767f;
  KERN_CONST f32 kHorizontalIntegralGain = 0.0f;
  KERN_CONST f32 kHorizontalAntiWindupGain = 0.0f;
  KERN_CONST f32 kHorizontalIntegralLimitMps2 = 7.0f;
  KERN_CONST f32 kMaxTiltDeg = 40.0f;
  KERN_CONST f32 kInnerAttitudeRangeDeg = 40.0f;
  KERN_CONST f32 kMaxAttitudeCommand = 1.0f;
  KERN_CONST f32 kHeadingKp = 2.2f;
  KERN_CONST f32 kHoverAdapt = 0.12f;
  KERN_CONST f32 kMinHoverTrim = 0.25f;
  KERN_CONST f32 kMaxHoverTrim = 0.65f;
  KERN_CONST f32 kEscCommandOffset = 0.05f;
  KERN_CONST f32 kEscCommandScale = 0.95f;
  KERN_CONST f32 kMinFlightThrottle = 0.08f;
  KERN_CONST f32 kMaxFlightThrottle = 1.0f;
  KERN_CONST f32 kDegradedMaxTiltDeg = 12.0f;
  KERN_CONST f32 kDegradedBodyPitchScale = 0.35f;
  KERN_CONST u16 kStateGameMode = 64;
  KERN_CONST u16 kStateNavigationDegraded = 128;
  KERN_CONST u16 kStateNavigationValid = 32;
  KERN_CONST f32 kMaxAutoTranslationTiltDeg = 25.0f;

  // ---- functions ----
  struct navigation_velocity_validResult { bool Result = false; };
  KERN_PURE navigation_velocity_validResult navigation_velocity_valid(const fc_NavigationState& n) {
    return {logic_and(logic_or(n.velocity_valid, n.valid), Arondight45_DroneFC_Core::finite(n.velocity_world_mps).Result)};
  }

  struct navigation_agl_validResult { bool Result = false; };
  KERN_PURE navigation_agl_validResult navigation_agl_valid(const fc_NavigationState& n) {
    return {logic_and(logic_and(logic_and(logic_or(n.agl_valid, n.valid), kern::formula<f32>([](auto agl_m) { return isfinite(agl_m); }, n.agl_m)), math_compare<">=", f32>(n.agl_m, 0.0f)), math_compare<"<", f32>(n.agl_m, 1000.0f))};
  }

  struct navigation_heading_validResult { bool Result = false; };
  KERN_PURE navigation_heading_validResult navigation_heading_valid(const fc_NavigationState& n) {
    return {logic_and(logic_and(logic_and(n.heading_valid, kern::formula<f32>([](auto heading_deg) { return isfinite(heading_deg); }, n.heading_deg)), math_compare<">=", f32>(n.heading_deg, -180.0f)), math_compare<"<=", f32>(n.heading_deg, 180.0f))};
  }

  struct finiteResult { bool Result = false; };
  KERN_PURE finiteResult finite(const fc_NavigationState& n) {
    return {logic_and(navigation_velocity_valid(n).Result, navigation_agl_valid(n).Result)};
  }

  struct wrap_degreesResult { f32 Result = 0.0f; };
  KERN_PURE wrap_degreesResult wrap_degrees(f32 value = 0.0f) {
    f32 value_ = 0.0f;
    value_ = value;
    while (loop(math_compare<">", f32>(value_, 180.0f))) {
      value_ = kern::formula<f32>([](auto value_) { return (value_ - 360.0f); }, value_);
    }
    while (loop(math_compare<"<", f32>(value_, -180.0f))) {
      value_ += 360.0f;
    }
    return {value_};
  }

  struct state_intentResult { fc_StateIntent Result = fc_StateIntent{0.0f, 0.0f, 0.0f, 0.0f, 2.0f, false, false}; };
  KERN_PURE state_intentResult state_intent(const fc_RC& rc) {
    f32 right = 0.0f;
    f32 forward = 0.0f;
    f32 magnitude = 0.0f;
    f32 shaped_magnitude = 0.0f;
    f32 radial_scale = 0.0f;
    f32 clearance01 = 0.0f;
    f32 body_pitch_deg = 0.0f;
    right = Arondight45_DroneFC_Core::centered(rc.ch[0]).Result;
    forward = Arondight45_DroneFC_Core::centered(rc.ch[1]).Result;
    magnitude = kern::formula<f32, f32>([](auto forward, auto right) { return sqrt(((forward * forward) + (right * right))); }, forward, right);
    if (math_compare<">", f32>(magnitude, 1.0f)) {
      forward = kern::formula<f32, f32>([](auto forward, auto magnitude) { return (forward / magnitude); }, forward, magnitude);
      right = kern::formula<f32, f32>([](auto right, auto magnitude) { return (right / magnitude); }, right, magnitude);
      magnitude = 1.0f;
    }
    if (math_compare<">", f32>(magnitude, 1e-06f)) {
      shaped_magnitude = Arondight45_DroneFC_Core::shape(magnitude, 0.035f, 0.25f).Result;
      radial_scale = kern::formula<f32, f32>([](auto shaped_magnitude, auto magnitude) { return (shaped_magnitude / magnitude); }, shaped_magnitude, magnitude);
      forward = kern::formula<f32, f32>([](auto forward, auto radial_scale) { return (forward * radial_scale); }, forward, radial_scale);
      right = kern::formula<f32, f32>([](auto right, auto radial_scale) { return (right * radial_scale); }, right, radial_scale);
    } else {
      forward = 0.0f;
      right = 0.0f;
    }
    clearance01 = Arondight45_DroneFC_Core::throttle(rc.ch[num_cast<i64, i32>(kStateClearanceChannel)]).Result;
    body_pitch_deg = kern::formula<f32, f32>([](auto the_result, auto kStateMaxBodyPitchDeg) { return (the_result * kStateMaxBodyPitchDeg); }, Arondight45_DroneFC_Core::shape(Arondight45_DroneFC_Core::centered(rc.ch[num_cast<i64, i32>(kStateBodyPitchChannel)]).Result, 0.045f, 0.2f).Result, kStateMaxBodyPitchDeg);
    return {struct_make<fc_StateIntent>(kern::formula<f32, f32>([](auto right, auto kStateMaxHorizontalSpeedMps) { return (right * kStateMaxHorizontalSpeedMps); }, right, kStateMaxHorizontalSpeedMps), kern::formula<f32, f32>([](auto forward, auto kStateMaxHorizontalSpeedMps) { return (forward * kStateMaxHorizontalSpeedMps); }, forward, kStateMaxHorizontalSpeedMps), kern::formula<f32, f32>([](auto the_result, auto kStateMaxYawRateDps) { return (the_result * kStateMaxYawRateDps); }, Arondight45_DroneFC_Core::shape(Arondight45_DroneFC_Core::centered(rc.ch[3]).Result, 0.045f, 0.2f).Result, kStateMaxYawRateDps), body_pitch_deg, kern::formula<f32, f32, f32>([](auto kStateMinClearanceM, auto clearance01, auto kStateMaxClearanceM) { return (kStateMinClearanceM + (clearance01 * (kStateMaxClearanceM - kStateMinClearanceM))); }, kStateMinClearanceM, clearance01, kStateMaxClearanceM), math_compare<">", i32>(num_cast<i32, u16>(rc.ch[4]), 1300), math_compare<">", i32>(num_cast<i32, u16>(rc.ch[num_cast<i64, i32>(kStateModeChannel)]), 1300))};
  }

  void StateController_reset_horizontal_state(fc_StateController& self) {
    self.acceleration_estimator_valid_ = false;
    self.acceleration_sample_dt_s_ = 0.0f;
    self.previous_velocity_world_mps_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
    self.measured_acceleration_world_mps2_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
    self.horizontal_integral_forward_mps2_ = 0.0f;
    self.horizontal_integral_right_mps2_ = 0.0f;
  }

  void StateController_reset_command_targets(fc_StateController& self) {
    self.horizontal_target_initialized_ = false;
    self.vertical_accel_initialized_ = false;
    self.commanded_forward_mps_ = 0.0f;
    self.commanded_right_mps_ = 0.0f;
    self.vertical_accel_state_mps2_ = 0.0f;
  }

  void StateController_reset(fc_StateController& self) {
    self.active_ = false;
    self.heading_source_valid_ = false;
    self.manual_pitch_active_ = false;
    self.target_yaw_deg_ = 0.0f;
    self.hover_trim_ = kInitialHoverThrottle;
    StateController_reset_horizontal_state(self);
    StateController_reset_command_targets(self);
    self.debug_ = struct_make<fc_StateControllerDebug>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  }

  void StateController_leave_mode(fc_StateController& self) {
    self.active_ = false;
    self.heading_source_valid_ = false;
    self.manual_pitch_active_ = false;
    StateController_reset_horizontal_state(self);
    StateController_reset_command_targets(self);
    self.debug_ = struct_make<fc_StateControllerDebug>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  }

  void StateController_update_command_targets(fc_StateController& self, const fc_StateIntent& intent, f32 arg3 = 0.0f) {
    self.commanded_forward_mps_ = intent.forward_mps;
    self.commanded_right_mps_ = intent.right_mps;
    self.horizontal_target_initialized_ = true;
  }

  void StateController_update_acceleration_estimator(fc_StateController& self, fc_V3 velocity_world_mps = fc_V3{0.0f, 0.0f, 0.0f}, f32 dt = 0.0f) {
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    f32 dz = 0.0f;
    f32 sample_dt = 0.0f;
    f32 inv_dt = 0.0f;
    fc_V3 sample = fc_V3{0.0f, 0.0f, 0.0f};
    f32 horizontal_sample = 0.0f;
    f32 alpha = 0.0f;
    if (logic_not(self.acceleration_estimator_valid_)) {
      self.previous_velocity_world_mps_ = velocity_world_mps;
      self.acceleration_estimator_valid_ = true;
      self.acceleration_sample_dt_s_ = 0.0f;
      return;
    }
    self.acceleration_sample_dt_s_ = kern::formula<f32, f32>([](auto acceleration_sample_dt_s_, auto dt) { return min((acceleration_sample_dt_s_ + dt), 0.05f); }, self.acceleration_sample_dt_s_, dt);
    dx = kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, velocity_world_mps.x, self.previous_velocity_world_mps_.x);
    dy = kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, velocity_world_mps.y, self.previous_velocity_world_mps_.y);
    dz = kern::formula<f32, f32>([](auto z, auto z_2) { return (z - z_2); }, velocity_world_mps.z, self.previous_velocity_world_mps_.z);
    if (logic_or(math_compare<"<", f32>(kern::formula<f32, f32, f32>([](auto dx, auto dy, auto dz) { return (((dx * dx) + (dy * dy)) + (dz * dz)); }, dx, dy, dz), 1e-10f), math_compare<"<", f32>(self.acceleration_sample_dt_s_, 0.002f))) {
      return;
    }
    sample_dt = self.acceleration_sample_dt_s_;
    inv_dt = kern::formula<f32>([](auto sample_dt) { return (1.0f / sample_dt); }, sample_dt);
    sample = struct_make<fc_V3>(kern::formula<f32, f32>([](auto dx, auto inv_dt) { return (dx * inv_dt); }, dx, inv_dt), kern::formula<f32, f32>([](auto dy, auto inv_dt) { return (dy * inv_dt); }, dy, inv_dt), kern::formula<f32, f32>([](auto dz, auto inv_dt) { return (dz * inv_dt); }, dz, inv_dt));
    self.previous_velocity_world_mps_ = velocity_world_mps;
    self.acceleration_sample_dt_s_ = 0.0f;
    horizontal_sample = kern::formula<f32, f32>([](auto x, auto y) { return sqrt(((x * x) + (y * y))); }, sample.x, sample.y);
    if (logic_or(logic_not(Arondight45_DroneFC_Core::finite(sample).Result), math_compare<">", f32>(horizontal_sample, kMaxNavigationAccelSampleMps2))) {
      self.measured_acceleration_world_mps2_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
      return;
    }
    alpha = kern::formula<f32>([](auto a) { return (a < 0.0f ? 0.0f : (1.0f < a ? 1.0f : a)); }, kern::formula<f32, f32>([](auto sample_dt, auto kMeasuredAccelerationFilterTauS) { return (sample_dt / (kMeasuredAccelerationFilterTauS + sample_dt)); }, sample_dt, kMeasuredAccelerationFilterTauS));
    self.measured_acceleration_world_mps2_.x = kern::formula<f32, f32, f32>([](auto x, auto alpha, auto x_2) { return (x + (alpha * (x_2 - x))); }, self.measured_acceleration_world_mps2_.x, alpha, sample.x);
    self.measured_acceleration_world_mps2_.y = kern::formula<f32, f32, f32>([](auto y, auto alpha, auto y_2) { return (y + (alpha * (y_2 - y))); }, self.measured_acceleration_world_mps2_.y, alpha, sample.y);
    self.measured_acceleration_world_mps2_.z = kern::formula<f32, f32, f32>([](auto z, auto alpha, auto z_2) { return (z + (alpha * (z_2 - z))); }, self.measured_acceleration_world_mps2_.z, alpha, sample.z);
  }

  struct StateController_kinematic_vertical_speedResult { f32 Result = 0.0f; };
  /// min(linear, braking curve): the braking curve v = -a·tau + sqrt((a·tau)^2 + 2·a·e) already budgets the vertical-velocity loop's own lag tau = 1/gain, so a full-rate sink is stopped at the target, not below it.
  KERN_PURE StateController_kinematic_vertical_speedResult StateController_kinematic_vertical_speed(f32 agl_error = 0.0f) {
    bool up = false;
    f32 a = 0.0f;
    f32 tau = 0.0f;
    f32 e = 0.0f;
    f32 at = 0.0f;
    f32 v = 0.0f;
    up = math_compare<">=", f32>(agl_error, 0.0f);
    a = kern::formula<bool, f32, f32>([](auto up, auto kClimbBrakeAccelerationMps2, auto kDescentBrakeAccelerationMps2) { return (up ? kClimbBrakeAccelerationMps2 : kDescentBrakeAccelerationMps2); }, up, kClimbBrakeAccelerationMps2, kDescentBrakeAccelerationMps2);
    tau = kern::formula<bool, f32, f32>([](auto up, auto kVerticalVelocityGain, auto kVerticalDescentVelocityGain) { return (1.0f / (up ? kVerticalVelocityGain : kVerticalDescentVelocityGain)); }, up, kVerticalVelocityGain, kVerticalDescentVelocityGain);
    e = kern::formula<f32>([](auto agl_error) { return abs(agl_error); }, agl_error);
    at = kern::formula<f32, f32>([](auto a, auto tau) { return (a * tau); }, a, tau);
    v = kern::formula<f32, f32, f32, f32>([](auto kAglToVerticalSpeed, auto e, auto at, auto a) { return min((kAglToVerticalSpeed * e), ((-at) + sqrt(((at * at) + ((2.0f * a) * e))))); }, kAglToVerticalSpeed, e, at, a);
    return {kern::formula<bool, f32>([](auto up, auto v) { return (up ? v : (-v)); }, up, v)};
  }

  struct StateController_limit_vertical_accel_jerkResult { f32 Result = 0.0f; };
  StateController_limit_vertical_accel_jerkResult StateController_limit_vertical_accel_jerk(fc_StateController& self, f32 requested_accel = 0.0f, f32 dt = 0.0f) {
    f32 max_delta = 0.0f;
    if (logic_not(self.vertical_accel_initialized_)) {
      self.vertical_accel_state_mps2_ = requested_accel;
      self.vertical_accel_initialized_ = true;
      return {self.vertical_accel_state_mps2_};
    }
    max_delta = kern::formula<f32, f32>([](auto kVerticalJerkLimitMps3, auto dt) { return (kVerticalJerkLimitMps3 * dt); }, kVerticalJerkLimitMps3, dt);
    self.vertical_accel_state_mps2_ = kern::formula<f32, f32, f32, f32>([](auto vertical_accel_state_mps2_, auto a, auto a_2, auto max_delta) { return (vertical_accel_state_mps2_ + (a < a_2 ? a_2 : (max_delta < a ? max_delta : a))); }, self.vertical_accel_state_mps2_, kern::formula<f32, f32>([](auto requested_accel, auto vertical_accel_state_mps2_) { return (requested_accel - vertical_accel_state_mps2_); }, requested_accel, self.vertical_accel_state_mps2_), kern::formula<f32>([](auto max_delta) { return (-max_delta); }, max_delta), max_delta);
    return {self.vertical_accel_state_mps2_};
  }

  void StateController_clamp_horizontal_integral(fc_StateController& self) {
    f32 magnitude = 0.0f;
    f32 scale = 0.0f;
    magnitude = kern::formula<f32, f32>([](auto horizontal_integral_forward_mps2_, auto horizontal_integral_right_mps2_) { return sqrt(((horizontal_integral_forward_mps2_ * horizontal_integral_forward_mps2_) + (horizontal_integral_right_mps2_ * horizontal_integral_right_mps2_))); }, self.horizontal_integral_forward_mps2_, self.horizontal_integral_right_mps2_);
    if (logic_and(math_compare<">", f32>(magnitude, kHorizontalIntegralLimitMps2), math_compare<">", f32>(magnitude, 1e-06f))) {
      scale = kern::formula<f32, f32>([](auto kHorizontalIntegralLimitMps2, auto magnitude) { return (kHorizontalIntegralLimitMps2 / magnitude); }, kHorizontalIntegralLimitMps2, magnitude);
      self.horizontal_integral_forward_mps2_ = kern::formula<f32, f32>([](auto horizontal_integral_forward_mps2_, auto scale) { return (horizontal_integral_forward_mps2_ * scale); }, self.horizontal_integral_forward_mps2_, scale);
      self.horizontal_integral_right_mps2_ = kern::formula<f32, f32>([](auto horizontal_integral_right_mps2_, auto scale) { return (horizontal_integral_right_mps2_ * scale); }, self.horizontal_integral_right_mps2_, scale);
    }
  }

  struct StateController_runResult { fc_Command Result = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}; };
  StateController_runResult StateController_run(fc_StateController& self, const fc_RC& receiver, const fc_NavigationState& nav, f32 yaw_deg = 0.0f, bool inner_armed = false, f32 dt = 0.0f, bool agl_valid = false) {
    f32 dt_ = 0.0f;
    fc_StateIntent intent = fc_StateIntent{0.0f, 0.0f, 0.0f, 0.0f, 2.0f, false, false};
    bool absolute_heading_valid = false;
    f32 measured_yaw_deg = 0.0f;
    f32 yaw_rad = 0.0f;
    f32 c = 0.0f;
    f32 s = 0.0f;
    f32 measured_forward = 0.0f;
    f32 measured_right = 0.0f;
    bool manual_pitch_active = false;
    bool manual_pitch_released = false;
    f32 measured_forward_accel = 0.0f;
    f32 measured_right_accel = 0.0f;
    bool agl_usable = false;
    f32 agl_m = 0.0f;
    f32 agl_error = 0.0f;
    f32 target_vz = 0.0f;
    f32 vz_error = 0.0f;
    f32 vertical_velocity_gain = 0.0f;
    f32 requested_vertical_accel = 0.0f;
    f32 vertical_accel = 0.0f;
    f32 specific_up = 0.0f;
    f32 forward_error = 0.0f;
    f32 right_error = 0.0f;
    f32 pilot_horizontal_speed = 0.0f;
    f32 neutral_damping_scale = 0.0f;
    f32 forward_feedback_unsat = 0.0f;
    f32 right_unsat = 0.0f;
    f32 forward_accel = 0.0f;
    f32 right_accel = 0.0f;
    f32 allowed_horizontal_accel = 0.0f;
    f32 horizontal_accel = 0.0f;
    f32 the_vector_scale = 0.0f;
    f32 auto_pitch_target_deg = 0.0f;
    f32 pitch_target_deg = 0.0f;
    f32 roll_target_deg = 0.0f;
    f32 roll_command = 0.0f;
    f32 pitch_command = 0.0f;
    f32 yaw_error = 0.0f;
    f32 desired_yaw_rate = 0.0f;
    f32 yaw_command = 0.0f;
    bool hovering = false;
    bool cannot_lift = false;
    f32 roll_rad = 0.0f;
    f32 pitch_rad = 0.0f;
    f32 vertical_thrust_fraction = 0.0f;
    f32 required_specific_force = 0.0f;
    f32 thrust_ratio = 0.0f;
    f32 hover_motor_command = 0.0f;
    f32 required_motor_command = 0.0f;
    f32 throttle_command = 0.0f;
    dt_ = dt;
    intent = state_intent(receiver).Result;
    absolute_heading_valid = navigation_heading_valid(nav).Result;
    measured_yaw_deg = wrap_degrees(kern::formula<bool, f32, f32>([](auto absolute_heading_valid, auto heading_deg, auto yaw_deg) { return (absolute_heading_valid ? heading_deg : yaw_deg); }, absolute_heading_valid, nav.heading_deg, yaw_deg)).Result;
    if (logic_or(logic_not(self.active_), math_compare<"!=", i32>(num_cast<i32, bool>(self.heading_source_valid_), num_cast<i32, bool>(absolute_heading_valid)))) {
      self.target_yaw_deg_ = measured_yaw_deg;
      self.heading_source_valid_ = absolute_heading_valid;
      self.manual_pitch_active_ = false;
      StateController_reset_horizontal_state(self);
      StateController_reset_command_targets(self);
      self.active_ = true;
    }
    dt_ = kern::formula<f32>([](auto dt_) { return (dt_ < 0.0002f ? 0.0002f : (0.02f < dt_ ? 0.02f : dt_)); }, dt_);
    self.target_yaw_deg_ = wrap_degrees(kern::formula<f32, f32, f32>([](auto target_yaw_deg_, auto yaw_rate_dps, auto dt_) { return (target_yaw_deg_ + (yaw_rate_dps * dt_)); }, self.target_yaw_deg_, intent.yaw_rate_dps, dt_)).Result;
    yaw_rad = kern::formula<f32, f32>([](auto measured_yaw_deg, auto kPi) { return ((measured_yaw_deg * kPi) / 180.0f); }, measured_yaw_deg, kPi);
    c = kern::formula<f32>([](auto yaw_rad) { return cos(yaw_rad); }, yaw_rad);
    s = kern::formula<f32>([](auto yaw_rad) { return sin(yaw_rad); }, yaw_rad);
    measured_forward = kern::formula<f32, f32, f32, f32>([](auto c, auto x, auto s, auto y) { return (((-c) * x) - (s * y)); }, c, nav.velocity_world_mps.x, s, nav.velocity_world_mps.y);
    measured_right = kern::formula<f32, f32, f32, f32>([](auto s, auto x, auto c, auto y) { return (((-s) * x) + (c * y)); }, s, nav.velocity_world_mps.x, c, nav.velocity_world_mps.y);
    if (logic_not(inner_armed)) {
      self.agl_estimate_valid_ = false;
      StateController_reset_horizontal_state(self);
      StateController_reset_command_targets(self);
      self.manual_pitch_active_ = false;
      self.target_yaw_deg_ = measured_yaw_deg;
      self.heading_source_valid_ = absolute_heading_valid;
      self.debug_ = struct_make<fc_StateControllerDebug>(intent.forward_mps, measured_forward, intent.right_mps, measured_right, self.target_yaw_deg_, measured_yaw_deg, intent.clearance_m, nav.agl_m, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
      return {struct_make<fc_Command>(0.0f, 0.0f, 0.0f, 0.0f, intent.arm)};
    }
    manual_pitch_active = math_compare<">=", f32>(kern::formula<f32>([](auto body_pitch_deg) { return abs(body_pitch_deg); }, intent.body_pitch_deg), kManualPitchOverrideThresholdDeg);
    manual_pitch_released = logic_and(self.manual_pitch_active_, logic_not(manual_pitch_active));
    self.manual_pitch_active_ = manual_pitch_active;
    if (manual_pitch_released) {
      self.commanded_forward_mps_ = measured_forward;
      self.horizontal_target_initialized_ = true;
      self.acceleration_estimator_valid_ = false;
      self.acceleration_sample_dt_s_ = 0.0f;
      self.previous_velocity_world_mps_ = nav.velocity_world_mps;
      self.measured_acceleration_world_mps2_ = struct_make<fc_V3>(0.0f, 0.0f, 0.0f);
      self.horizontal_integral_forward_mps2_ = 0.0f;
    }
    StateController_update_command_targets(self, intent, dt_);
    if (manual_pitch_active) {
      self.commanded_forward_mps_ = measured_forward;
      self.horizontal_integral_forward_mps2_ = 0.0f;
    }
    StateController_update_acceleration_estimator(self, nav.velocity_world_mps, dt_);
    measured_forward_accel = kern::formula<f32, f32, f32, f32>([](auto c, auto x, auto s, auto y) { return (((-c) * x) - (s * y)); }, c, self.measured_acceleration_world_mps2_.x, s, self.measured_acceleration_world_mps2_.y);
    measured_right_accel = kern::formula<f32, f32, f32, f32>([](auto s, auto x, auto c, auto y) { return (((-s) * x) + (c * y)); }, s, self.measured_acceleration_world_mps2_.x, c, self.measured_acceleration_world_mps2_.y);
    agl_usable = agl_valid;
    agl_m = nav.agl_m;
    if (agl_valid) {
      self.agl_estimate_m_ = nav.agl_m;
      self.agl_estimate_valid_ = true;
    } else if (logic_and(self.agl_estimate_valid_, kern::formula<f32>([](auto z) { return isfinite(z); }, nav.velocity_world_mps.z))) {
      self.agl_estimate_m_ = kern::formula<f32, f32, f32>([](auto agl_estimate_m_, auto z, auto dt_) { return (agl_estimate_m_ + (z * dt_)); }, self.agl_estimate_m_, nav.velocity_world_mps.z, dt_);
      agl_m = self.agl_estimate_m_;
      agl_usable = true;
    }
    agl_error = kern::formula<bool, f32, f32, f32>([](auto agl_usable, auto clearance_m, auto agl_m, auto a0f) { return (agl_usable ? (clearance_m - agl_m) : a0f); }, agl_usable, intent.clearance_m, agl_m, 0.0f);
    target_vz = kern::formula<bool, f32, f32, f32, f32>([](auto agl_usable, auto the_result, auto a, auto kMaxVerticalSpeedMps, auto a0f) { return (agl_usable ? (the_result < a ? a : (kMaxVerticalSpeedMps < the_result ? kMaxVerticalSpeedMps : the_result)) : a0f); }, agl_usable, StateController_kinematic_vertical_speed(agl_error).Result, kern::formula<f32>([](auto kMaxVerticalDescentSpeedMps) { return (-kMaxVerticalDescentSpeedMps); }, kMaxVerticalDescentSpeedMps), kMaxVerticalSpeedMps, 0.0f);
    vz_error = kern::formula<f32, f32>([](auto target_vz, auto z) { return (target_vz - z); }, target_vz, nav.velocity_world_mps.z);
    vertical_velocity_gain = kern::formula<f32, f32, f32>([](auto target_vz, auto kVerticalDescentVelocityGain, auto kVerticalVelocityGain) { return ((target_vz < 0.0f) ? kVerticalDescentVelocityGain : kVerticalVelocityGain); }, target_vz, kVerticalDescentVelocityGain, kVerticalVelocityGain);
    requested_vertical_accel = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto kMaxVerticalAccelerationMps2) { return (a < a_2 ? a_2 : (kMaxVerticalAccelerationMps2 < a ? kMaxVerticalAccelerationMps2 : a)); }, kern::formula<f32, f32>([](auto vertical_velocity_gain, auto vz_error) { return (vertical_velocity_gain * vz_error); }, vertical_velocity_gain, vz_error), kern::formula<f32>([](auto kMaxVerticalDescentAccelerationMps2) { return (-kMaxVerticalDescentAccelerationMps2); }, kMaxVerticalDescentAccelerationMps2), kMaxVerticalAccelerationMps2);
    n10 = StateController_limit_vertical_accel_jerk(self, requested_vertical_accel, dt_);
    vertical_accel = n10.Result;
    specific_up = kern::formula<f32, f32, f32>([](auto a, auto kMinSpecificUpMps2, auto kMaxSpecificUpMps2) { return (a < kMinSpecificUpMps2 ? kMinSpecificUpMps2 : (kMaxSpecificUpMps2 < a ? kMaxSpecificUpMps2 : a)); }, kern::formula<f32, f32>([](auto kGravityMps2, auto vertical_accel) { return (kGravityMps2 + vertical_accel); }, kGravityMps2, vertical_accel), kMinSpecificUpMps2, kMaxSpecificUpMps2);
    forward_error = kern::formula<f32, f32>([](auto commanded_forward_mps_, auto measured_forward) { return (commanded_forward_mps_ - measured_forward); }, self.commanded_forward_mps_, measured_forward);
    right_error = kern::formula<f32, f32>([](auto commanded_right_mps_, auto measured_right) { return (commanded_right_mps_ - measured_right); }, self.commanded_right_mps_, measured_right);
    pilot_horizontal_speed = kern::formula<f32, f32>([](auto forward_mps, auto right_mps) { return sqrt(((forward_mps * forward_mps) + (right_mps * right_mps))); }, intent.forward_mps, intent.right_mps);
    if (math_compare<"<=", f32>(pilot_horizontal_speed, kHorizontalIntegralNeutralTargetMps)) {
      self.horizontal_integral_forward_mps2_ = 0.0f;
      self.horizontal_integral_right_mps2_ = 0.0f;
    }
    neutral_damping_scale = kern::formula<f32, f32, f32, f32>([](auto pilot_horizontal_speed, auto kHorizontalIntegralNeutralTargetMps, auto kHorizontalNeutralDampingScale, auto a0f) { return ((pilot_horizontal_speed <= kHorizontalIntegralNeutralTargetMps) ? kHorizontalNeutralDampingScale : a0f); }, pilot_horizontal_speed, kHorizontalIntegralNeutralTargetMps, kHorizontalNeutralDampingScale, 1.0f);
    forward_feedback_unsat = kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto kHorizontalVelocityGain, auto commanded_forward_mps_, auto measured_forward, auto horizontal_integral_forward_mps2_, auto kHorizontalAccelerationDamping, auto neutral_damping_scale, auto measured_forward_accel) { return (((kHorizontalVelocityGain * (commanded_forward_mps_ - measured_forward)) + horizontal_integral_forward_mps2_) - ((kHorizontalAccelerationDamping * neutral_damping_scale) * measured_forward_accel)); }, kHorizontalVelocityGain, self.commanded_forward_mps_, measured_forward, self.horizontal_integral_forward_mps2_, kHorizontalAccelerationDamping, neutral_damping_scale, measured_forward_accel);
    right_unsat = kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto kHorizontalVelocityGain, auto commanded_right_mps_, auto measured_right, auto horizontal_integral_right_mps2_, auto kHorizontalAccelerationDamping, auto neutral_damping_scale, auto measured_right_accel) { return (((kHorizontalVelocityGain * (commanded_right_mps_ - measured_right)) + horizontal_integral_right_mps2_) - ((kHorizontalAccelerationDamping * neutral_damping_scale) * measured_right_accel)); }, kHorizontalVelocityGain, self.commanded_right_mps_, measured_right, self.horizontal_integral_right_mps2_, kHorizontalAccelerationDamping, neutral_damping_scale, measured_right_accel);
    forward_accel = kern::formula<bool, f32, f32>([](auto manual_pitch_active, auto a0f, auto forward_feedback_unsat) { return (manual_pitch_active ? a0f : forward_feedback_unsat); }, manual_pitch_active, 0.0f, forward_feedback_unsat);
    right_accel = right_unsat;
    allowed_horizontal_accel = kern::formula<f32, f32, f32>([](auto kMaxHorizontalAccelerationMps2, auto specific_up, auto kMaxAutoTranslationTiltTangent) { return min(kMaxHorizontalAccelerationMps2, (specific_up * kMaxAutoTranslationTiltTangent)); }, kMaxHorizontalAccelerationMps2, specific_up, kMaxAutoTranslationTiltTangent);
    horizontal_accel = kern::formula<f32, f32>([](auto forward_accel, auto right_accel) { return sqrt(((forward_accel * forward_accel) + (right_accel * right_accel))); }, forward_accel, right_accel);
    if (logic_and(math_compare<">", f32>(horizontal_accel, allowed_horizontal_accel), math_compare<">", f32>(horizontal_accel, 1e-06f))) {
      the_vector_scale = kern::formula<f32, f32>([](auto allowed_horizontal_accel, auto horizontal_accel) { return (allowed_horizontal_accel / horizontal_accel); }, allowed_horizontal_accel, horizontal_accel);
      forward_accel = kern::formula<f32, f32>([](auto forward_accel, auto the_vector_scale) { return (forward_accel * the_vector_scale); }, forward_accel, the_vector_scale);
      right_accel = kern::formula<f32, f32>([](auto right_accel, auto the_vector_scale) { return (right_accel * the_vector_scale); }, right_accel, the_vector_scale);
    }
    if (math_compare<">", f32>(pilot_horizontal_speed, kHorizontalIntegralNeutralTargetMps)) {
      if (logic_not(manual_pitch_active)) {
        self.horizontal_integral_forward_mps2_ = kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto horizontal_integral_forward_mps2_, auto kHorizontalIntegralGain, auto forward_error, auto kHorizontalAntiWindupGain, auto forward_accel, auto forward_feedback_unsat, auto dt_) { return (horizontal_integral_forward_mps2_ + (((kHorizontalIntegralGain * forward_error) + (kHorizontalAntiWindupGain * (forward_accel - forward_feedback_unsat))) * dt_)); }, self.horizontal_integral_forward_mps2_, kHorizontalIntegralGain, forward_error, kHorizontalAntiWindupGain, forward_accel, forward_feedback_unsat, dt_);
      }
      self.horizontal_integral_right_mps2_ = kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto horizontal_integral_right_mps2_, auto kHorizontalIntegralGain, auto right_error, auto kHorizontalAntiWindupGain, auto right_accel, auto right_unsat, auto dt_) { return (horizontal_integral_right_mps2_ + (((kHorizontalIntegralGain * right_error) + (kHorizontalAntiWindupGain * (right_accel - right_unsat))) * dt_)); }, self.horizontal_integral_right_mps2_, kHorizontalIntegralGain, right_error, kHorizontalAntiWindupGain, right_accel, right_unsat, dt_);
      StateController_clamp_horizontal_integral(self);
    }
    auto_pitch_target_deg = kern::formula<f32, f32, f32>([](auto forward_accel, auto specific_up, auto kPi) { return (((-atan2(forward_accel, specific_up)) * 180.0f) / kPi); }, forward_accel, specific_up, kPi);
    pitch_target_deg = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto kMaxTiltDeg) { return (a < a_2 ? a_2 : (kMaxTiltDeg < a ? kMaxTiltDeg : a)); }, kern::formula<f32, f32>([](auto auto_pitch_target_deg, auto body_pitch_deg) { return (auto_pitch_target_deg + body_pitch_deg); }, auto_pitch_target_deg, intent.body_pitch_deg), kern::formula<f32>([](auto kMaxTiltDeg) { return (-kMaxTiltDeg); }, kMaxTiltDeg), kMaxTiltDeg);
    roll_target_deg = kern::formula<f32, f32, f32, f32>([](auto right_accel, auto specific_up, auto forward_accel, auto kPi) { return (((-atan2(right_accel, sqrt(((specific_up * specific_up) + (forward_accel * forward_accel))))) * 180.0f) / kPi); }, right_accel, specific_up, forward_accel, kPi);
    roll_command = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto kMaxAttitudeCommand) { return (a < a_2 ? a_2 : (kMaxAttitudeCommand < a ? kMaxAttitudeCommand : a)); }, kern::formula<f32, f32>([](auto roll_target_deg, auto kInnerAttitudeRangeDeg) { return (roll_target_deg / kInnerAttitudeRangeDeg); }, roll_target_deg, kInnerAttitudeRangeDeg), kern::formula<f32>([](auto kMaxAttitudeCommand) { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand);
    pitch_command = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto kMaxAttitudeCommand) { return (a < a_2 ? a_2 : (kMaxAttitudeCommand < a ? kMaxAttitudeCommand : a)); }, kern::formula<f32, f32>([](auto pitch_target_deg, auto kInnerAttitudeRangeDeg) { return (pitch_target_deg / kInnerAttitudeRangeDeg); }, pitch_target_deg, kInnerAttitudeRangeDeg), kern::formula<f32>([](auto kMaxAttitudeCommand) { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand);
    yaw_error = wrap_degrees(kern::formula<f32, f32>([](auto target_yaw_deg_, auto measured_yaw_deg) { return (target_yaw_deg_ - measured_yaw_deg); }, self.target_yaw_deg_, measured_yaw_deg)).Result;
    desired_yaw_rate = kern::formula<f32>([](auto a) { return (a < (-180.0f) ? (-180.0f) : (180.0f < a ? 180.0f : a)); }, kern::formula<f32, f32, f32>([](auto yaw_rate_dps, auto kHeadingKp, auto yaw_error) { return (yaw_rate_dps + (kHeadingKp * yaw_error)); }, intent.yaw_rate_dps, kHeadingKp, yaw_error));
    yaw_command = kern::formula<f32>([](auto desired_yaw_rate) { return (desired_yaw_rate / 180.0f); }, desired_yaw_rate);
    hovering = math_compare<"<", f32>(kern::formula<f32>([](auto target_vz) { return abs(target_vz); }, target_vz), 1.5f);
    cannot_lift = logic_and(logic_and(math_compare<">", f32>(target_vz, 0.0f), math_compare<">", f32>(vz_error, 0.0f)), math_compare<"<", f32>(nav.velocity_world_mps.z, 0.5f));
    if (logic_and(agl_usable, logic_or(hovering, cannot_lift))) {
      self.hover_trim_ = kern::formula<f32, f32, f32>([](auto a, auto kMinHoverTrim, auto kMaxHoverTrim) { return (a < kMinHoverTrim ? kMinHoverTrim : (kMaxHoverTrim < a ? kMaxHoverTrim : a)); }, kern::formula<f32, f32, f32, f32>([](auto hover_trim_, auto kHoverAdapt, auto vz_error, auto dt_) { return (hover_trim_ + ((kHoverAdapt * vz_error) * dt_)); }, self.hover_trim_, kHoverAdapt, vz_error, dt_), kMinHoverTrim, kMaxHoverTrim);
    }
    roll_rad = kern::formula<f32, f32>([](auto roll_target_deg, auto kPi) { return ((roll_target_deg * kPi) / 180.0f); }, roll_target_deg, kPi);
    pitch_rad = kern::formula<f32, f32>([](auto pitch_target_deg, auto kPi) { return ((pitch_target_deg * kPi) / 180.0f); }, pitch_target_deg, kPi);
    vertical_thrust_fraction = kern::formula<f32, f32>([](auto roll_rad, auto pitch_rad) { return max(0.35f, (cos(roll_rad) * cos(pitch_rad))); }, roll_rad, pitch_rad);
    required_specific_force = kern::formula<f32, f32>([](auto specific_up, auto vertical_thrust_fraction) { return (specific_up / vertical_thrust_fraction); }, specific_up, vertical_thrust_fraction);
    thrust_ratio = kern::formula<f32, f32>([](auto required_specific_force, auto kGravityMps2) { return (required_specific_force / kGravityMps2); }, required_specific_force, kGravityMps2);
    hover_motor_command = kern::formula<f32, f32, f32>([](auto kEscCommandOffset, auto kEscCommandScale, auto hover_trim_) { return (kEscCommandOffset + (kEscCommandScale * hover_trim_)); }, kEscCommandOffset, kEscCommandScale, self.hover_trim_);
    required_motor_command = kern::formula<f32, f32>([](auto hover_motor_command, auto thrust_ratio) { return (hover_motor_command * sqrt(thrust_ratio)); }, hover_motor_command, thrust_ratio);
    throttle_command = kern::formula<f32, f32, f32>([](auto a, auto kMinFlightThrottle, auto kMaxFlightThrottle) { return (a < kMinFlightThrottle ? kMinFlightThrottle : (kMaxFlightThrottle < a ? kMaxFlightThrottle : a)); }, kern::formula<f32, f32, f32>([](auto required_motor_command, auto kEscCommandOffset, auto kEscCommandScale) { return ((required_motor_command - kEscCommandOffset) / kEscCommandScale); }, required_motor_command, kEscCommandOffset, kEscCommandScale), kMinFlightThrottle, kMaxFlightThrottle);
    self.debug_ = struct_make<fc_StateControllerDebug>(self.commanded_forward_mps_, measured_forward, self.commanded_right_mps_, measured_right, self.target_yaw_deg_, measured_yaw_deg, intent.clearance_m, agl_m, target_vz, throttle_command, roll_command, pitch_command, yaw_command, forward_accel, right_accel, vertical_accel);
    return {Arondight45_DroneFC_Core::sanitize(struct_make<fc_Command>(roll_command, pitch_command, throttle_command, yaw_command, intent.arm)).Result};
  }

  struct StateController_degraded_attitude_commandResult { fc_Command Result = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false}; };
  KERN_PURE StateController_degraded_attitude_commandResult StateController_degraded_attitude_command(const fc_StateController& self, const fc_RC& receiver) {
    fc_StateIntent intent = fc_StateIntent{0.0f, 0.0f, 0.0f, 0.0f, 2.0f, false, false};
    f32 right_unit = 0.0f;
    f32 forward_unit = 0.0f;
    f32 roll_target_deg = 0.0f;
    f32 pitch_target_deg = 0.0f;
    f32 roll_command = 0.0f;
    f32 pitch_command = 0.0f;
    f32 vertical_fraction = 0.0f;
    f32 hover_motor_command = 0.0f;
    f32 required_motor_command = 0.0f;
    f32 throttle_command = 0.0f;
    intent = state_intent(receiver).Result;
    right_unit = kern::formula<f32>([](auto a) { return (a < (-1.0f) ? (-1.0f) : (1.0f < a ? 1.0f : a)); }, kern::formula<f32, f32>([](auto right_mps, auto kStateMaxHorizontalSpeedMps) { return (right_mps / kStateMaxHorizontalSpeedMps); }, intent.right_mps, kStateMaxHorizontalSpeedMps));
    forward_unit = kern::formula<f32>([](auto a) { return (a < (-1.0f) ? (-1.0f) : (1.0f < a ? 1.0f : a)); }, kern::formula<f32, f32>([](auto forward_mps, auto kStateMaxHorizontalSpeedMps) { return (forward_mps / kStateMaxHorizontalSpeedMps); }, intent.forward_mps, kStateMaxHorizontalSpeedMps));
    roll_target_deg = kern::formula<f32, f32>([](auto right_unit, auto kDegradedMaxTiltDeg) { return ((-right_unit) * kDegradedMaxTiltDeg); }, right_unit, kDegradedMaxTiltDeg);
    pitch_target_deg = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto kDegradedMaxTiltDeg) { return (a < a_2 ? a_2 : (kDegradedMaxTiltDeg < a ? kDegradedMaxTiltDeg : a)); }, kern::formula<f32, f32, f32, f32>([](auto forward_unit, auto kDegradedMaxTiltDeg, auto body_pitch_deg, auto kDegradedBodyPitchScale) { return (((-forward_unit) * kDegradedMaxTiltDeg) + (body_pitch_deg * kDegradedBodyPitchScale)); }, forward_unit, kDegradedMaxTiltDeg, intent.body_pitch_deg, kDegradedBodyPitchScale), kern::formula<f32>([](auto kDegradedMaxTiltDeg) { return (-kDegradedMaxTiltDeg); }, kDegradedMaxTiltDeg), kDegradedMaxTiltDeg);
    roll_command = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto kMaxAttitudeCommand) { return (a < a_2 ? a_2 : (kMaxAttitudeCommand < a ? kMaxAttitudeCommand : a)); }, kern::formula<f32, f32>([](auto roll_target_deg, auto kInnerAttitudeRangeDeg) { return (roll_target_deg / kInnerAttitudeRangeDeg); }, roll_target_deg, kInnerAttitudeRangeDeg), kern::formula<f32>([](auto kMaxAttitudeCommand) { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand);
    pitch_command = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto kMaxAttitudeCommand) { return (a < a_2 ? a_2 : (kMaxAttitudeCommand < a ? kMaxAttitudeCommand : a)); }, kern::formula<f32, f32>([](auto pitch_target_deg, auto kInnerAttitudeRangeDeg) { return (pitch_target_deg / kInnerAttitudeRangeDeg); }, pitch_target_deg, kInnerAttitudeRangeDeg), kern::formula<f32>([](auto kMaxAttitudeCommand) { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand);
    vertical_fraction = kern::formula<f32, f32, f32>([](auto roll_target_deg, auto kPi, auto pitch_target_deg) { return max(0.5f, (cos(((roll_target_deg * kPi) / 180.0f)) * cos(((pitch_target_deg * kPi) / 180.0f)))); }, roll_target_deg, kPi, pitch_target_deg);
    hover_motor_command = kern::formula<f32, f32, f32>([](auto kEscCommandOffset, auto kEscCommandScale, auto hover_trim_) { return (kEscCommandOffset + (kEscCommandScale * hover_trim_)); }, kEscCommandOffset, kEscCommandScale, self.hover_trim_);
    required_motor_command = kern::formula<f32, f32>([](auto hover_motor_command, auto vertical_fraction) { return (hover_motor_command / sqrt(vertical_fraction)); }, hover_motor_command, vertical_fraction);
    throttle_command = kern::formula<f32, f32, f32>([](auto a, auto kMinFlightThrottle, auto kMaxFlightThrottle) { return (a < kMinFlightThrottle ? kMinFlightThrottle : (kMaxFlightThrottle < a ? kMaxFlightThrottle : a)); }, kern::formula<f32, f32, f32>([](auto required_motor_command, auto kEscCommandOffset, auto kEscCommandScale) { return ((required_motor_command - kEscCommandOffset) / kEscCommandScale); }, required_motor_command, kEscCommandOffset, kEscCommandScale), kMinFlightThrottle, kMaxFlightThrottle);
    return {Arondight45_DroneFC_Core::sanitize(struct_make<fc_Command>(roll_command, pitch_command, throttle_command, kern::formula<f32>([](auto yaw_rate_dps) { return (yaw_rate_dps / 180.0f); }, intent.yaw_rate_dps), intent.arm)).Result};
  }

  struct StateController_hover_trimResult { f32 Result = 0.0f; };
  KERN_PURE StateController_hover_trimResult StateController_hover_trim(const fc_StateController& self) {
    return {self.hover_trim_};
  }

  void StateRuntime_reset(fc_StateRuntime& self) {
    Arondight45_DroneFC_Core::Runtime_reset(self.runtime_);
    StateController_reset(self.state_controller_);
    self.last_yaw_deg_ = 0.0f;
    self.game_active_ = false;
  }

  struct StateRuntime_newResult { fc_StateRuntime Result = fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}; };
  StateRuntime_newResult StateRuntime_new() {
    fc_StateRuntime self = fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false};
    n11 = Arondight45_DroneFC_Core::Runtime_new();
    self.runtime_ = n11.Result;
    StateRuntime_reset(self);
    return {self};
  }

  void StateRuntime_update_attitude(fc_StateRuntime& self, const fc_RuntimeOutput& out) {
    self.last_yaw_deg_ = kern::formula<i16>([](auto a) { return (num_cast<f32, i16>(a) * 0.01f); }, out.attitude_cdeg[2]);
  }

  struct StateRuntime_stepResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false}; };
  StateRuntime_stepResult StateRuntime_step(fc_StateRuntime& self, fc_StateRuntimeInput input = fc_StateRuntimeInput{fc_RuntimeInput{fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}, fc_RC{List<u16>{}, false, false, false, 0}, 0, 1000, 0, true, false}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}}) {
    fc_StateIntent intent = fc_StateIntent{0.0f, 0.0f, 0.0f, 0.0f, 2.0f, false, false};
    fc_RuntimeOutput out = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    bool receiver_valid = false;
    fc_RuntimeOutput out_2 = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    bool velocity_valid = false;
    bool agl_valid = false;
    bool heading_valid = false;
    bool horizontal_navigation = false;
    bool full_navigation = false;
    fc_RuntimeOutput out_3 = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    f32 dt = 0.0f;
    fc_Command physical_command = fc_Command{0.0f, 0.0f, 0.0f, 0.0f, false};
    fc_RuntimeOutput out_4 = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false};
    intent = state_intent(input.flight.rc).Result;
    if (logic_not(intent.game_mode)) {
      if (self.game_active_) {
        StateController_leave_mode(self.state_controller_);
      }
      self.game_active_ = false;
      n12 = Arondight45_DroneFC_Core::Runtime_step(self.runtime_, input.flight);
      out = n12.Result;
      StateRuntime_update_attitude(self, out);
      return {out};
    }
    self.game_active_ = true;
    receiver_valid = logic_and(input.flight.rc.valid, input.flight.rc_fresh);
    if (logic_not(receiver_valid)) {
      StateController_leave_mode(self.state_controller_);
      n13 = Arondight45_DroneFC_Core::Runtime_step_command(self.runtime_, input.flight, struct_make<fc_Command>(0.0f, 0.0f, 0.0f, 0.0f, false), false);
      out_2 = n13.Result;
      out_2.state = num_cast<u16, i32>(kern::formula<u16, u16>([](auto state, auto kStateGameMode) { return (num_cast<i32, u16>(state) | num_cast<i32, u16>(kStateGameMode)); }, out_2.state, kStateGameMode));
      StateRuntime_update_attitude(self, out_2);
      return {out_2};
    }
    velocity_valid = navigation_velocity_valid(input.navigation).Result;
    agl_valid = navigation_agl_valid(input.navigation).Result;
    heading_valid = navigation_heading_valid(input.navigation).Result;
    horizontal_navigation = logic_and(velocity_valid, heading_valid);
    full_navigation = logic_and(horizontal_navigation, agl_valid);
    if (logic_and(logic_not(Arondight45_DroneFC_Core::Runtime_armed(self.runtime_).Result), logic_not(full_navigation))) {
      StateController_leave_mode(self.state_controller_);
      n14 = Arondight45_DroneFC_Core::Runtime_step_command(self.runtime_, input.flight, struct_make<fc_Command>(0.0f, 0.0f, 0.0f, 0.0f, false), false);
      out_3 = n14.Result;
      out_3.state = num_cast<u16, i32>(kern::formula<u16, u16, u16>([](auto state, auto kStateGameMode, auto kStateNavigationDegraded) { return (num_cast<i32, u16>(state) | (num_cast<i32, u16>(kStateGameMode) | num_cast<i32, u16>(kStateNavigationDegraded))); }, out_3.state, kStateGameMode, kStateNavigationDegraded));
      StateRuntime_update_attitude(self, out_3);
      return {out_3};
    }
    dt = kern::formula<u32, f32>([](auto dt_us, auto a001f) { return (((dt_us > 0) && (dt_us < 100000)) ? (num_cast<f32, u32>(dt_us) * 1e-06f) : a001f); }, input.flight.dt_us, 0.001f);
    physical_command = struct_make<fc_Command>(0.0f, 0.0f, 0.0f, 0.0f, false);
    if (horizontal_navigation) {
      n15 = StateController_run(self.state_controller_, input.flight.rc, input.navigation, self.last_yaw_deg_, Arondight45_DroneFC_Core::Runtime_armed(self.runtime_).Result, dt, agl_valid);
      physical_command = n15.Result;
    } else {
      StateController_leave_mode(self.state_controller_);
      physical_command = StateController_degraded_attitude_command(self.state_controller_, input.flight.rc).Result;
    }
    n16 = Arondight45_DroneFC_Core::Runtime_step_command(self.runtime_, input.flight, physical_command, true);
    out_4 = n16.Result;
    out_4.state = num_cast<u16, i32>(kern::formula<u16, u16>([](auto state, auto kStateGameMode) { return (num_cast<i32, u16>(state) | num_cast<i32, u16>(kStateGameMode)); }, out_4.state, kStateGameMode));
    if (full_navigation) {
      out_4.state = num_cast<u16, i32>(kern::formula<u16, u16>([](auto state, auto kStateNavigationValid) { return (num_cast<i32, u16>(state) | num_cast<i32, u16>(kStateNavigationValid)); }, out_4.state, kStateNavigationValid));
    } else {
      out_4.state = num_cast<u16, i32>(kern::formula<u16, u16>([](auto state, auto kStateNavigationDegraded) { return (num_cast<i32, u16>(state) | num_cast<i32, u16>(kStateNavigationDegraded)); }, out_4.state, kStateNavigationDegraded));
    }
    StateRuntime_update_attitude(self, out_4);
    return {out_4};
  }

  struct StateRuntime_game_activeResult { bool Result = false; };
  KERN_PURE StateRuntime_game_activeResult StateRuntime_game_active(const fc_StateRuntime& self) {
    return {self.game_active_};
  }

  // ---- what nodes give and keep ----
  StateController_limit_vertical_accel_jerkResult n10;
  Arondight45_DroneFC_Core::Runtime_newResult n11;
  Arondight45_DroneFC_Core::Runtime_stepResult n12;
  Arondight45_DroneFC_Core::Runtime_step_commandResult n13;
  Arondight45_DroneFC_Core::Runtime_step_commandResult n14;
  StateController_runResult n15;
  Arondight45_DroneFC_Core::Runtime_step_commandResult n16;
};
