struct Arondight45_StateControl : virtual kern::Library, virtual Arondight45_DroneFC_Core {
  // ---- types ----
  KERN_STRUCT(fc_StateControllerDebug) { Float desired_forward_mps = 0.0; Float measured_forward_mps = 0.0; Float desired_right_mps = 0.0; Float measured_right_mps = 0.0; Float target_yaw_deg = 0.0; Float measured_yaw_deg = 0.0; Float target_agl_m = 0.0; Float measured_agl_m = 0.0; Float target_vz_mps = 0.0; Float throttle = 0.0; Float roll_command = 0.0; Float pitch_command = 0.0; Float yaw_command = 0.0; Float forward_accel_mps2 = 0.0; Float right_accel_mps2 = 0.0; Float vertical_accel_mps2 = 0.0; };
  KERN_STRUCT(fc_StateIntent) { Float right_mps = 0.0; Float forward_mps = 0.0; Float yaw_rate_dps = 0.0; Float body_pitch_deg = 0.0; Float clearance_m = 2.0; Bool arm = false; Bool game_mode = false; };
  KERN_STRUCT(fc_NavigationState) { fc_V3 velocity_world_mps = fc_V3{0.0, 0.0, 0.0}; Float agl_m = 0.0; Bool valid = false; Bool velocity_valid = false; Bool agl_valid = false; Float heading_deg = 0.0; Bool heading_valid = false; };
  KERN_STRUCT(fc_StateController) { Float agl_estimate_m_ = 0.0; Bool agl_estimate_valid_ = false; Bool active_ = false; Bool heading_source_valid_ = false; Bool manual_pitch_active_ = false; Bool acceleration_estimator_valid_ = false; Bool horizontal_target_initialized_ = false; Bool vertical_accel_initialized_ = false; Float acceleration_sample_dt_s_ = 0.0; fc_V3 previous_velocity_world_mps_ = fc_V3{0.0, 0.0, 0.0}; fc_V3 measured_acceleration_world_mps2_ = fc_V3{0.0, 0.0, 0.0}; Float horizontal_integral_forward_mps2_ = 0.0; Float horizontal_integral_right_mps2_ = 0.0; Float commanded_forward_mps_ = 0.0; Float commanded_right_mps_ = 0.0; Float vertical_accel_state_mps2_ = 0.0; Float target_yaw_deg_ = 0.0; Float hover_trim_ = 0.39; fc_StateControllerDebug debug_ = fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; };
  KERN_STRUCT(fc_StateRuntimeInput) { fc_RuntimeInput flight = fc_RuntimeInput{fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}, fc_RC{List<U16>{}, false, false, false, Int64{0x0}}, Int64{0x0}, 1000, 0, true, false}; fc_NavigationState navigation = fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}; };
  KERN_STRUCT(fc_StateRuntime) { fc_Runtime runtime_ = fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}; fc_StateController state_controller_ = fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}; Float last_yaw_deg_ = 0.0; Bool game_active_ = false; };

  // ---- variables ----
  KERN_CONST I32 kStateClearanceChannel = 5;
  KERN_CONST I32 kStateBodyPitchChannel = 7;
  KERN_CONST Float kStateMaxBodyPitchDeg = 25.0;
  /// Shared Production/HIL/WASM velocity envelope. GAME phone settings encode a fraction of this real velocity target; the FC remains the only authority that turns velocity error into bounded acceleration, attitude and motor commands.
  KERN_CONST Float kStateMaxHorizontalSpeedMps = 25.0;
  KERN_CONST Float kStateMaxYawRateDps = 140.0;
  KERN_CONST Float kStateMinClearanceM = 0.5;
  KERN_CONST Float kStateMaxClearanceM = 120.0;
  KERN_CONST I32 kStateModeChannel = 6;
  KERN_CONST Float kInitialHoverThrottle = 0.39;
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Float kManualPitchOverrideThresholdDeg = 0.5;
  KERN_CONST Float kMaxNavigationAccelSampleMps2 = 15.0;
  KERN_CONST Float kMeasuredAccelerationFilterTauS = 0.06;
  /// Braking used by the kinematic altitude law: a climb is braked by cutting thrust (bounded by the descent reserve), a sink by thrust surplus.
  KERN_CONST Float kClimbBrakeAccelerationMps2 = 6.0;
  KERN_CONST Float kDescentBrakeAccelerationMps2 = 4.0;
  KERN_CONST Float kVerticalVelocityGain = 4.0;
  KERN_CONST Float kVerticalDescentVelocityGain = 8.0;
  KERN_CONST Float kAglToVerticalSpeed = 2.0;
  KERN_CONST Float kMaxVerticalDescentSpeedMps = 9.0;
  KERN_CONST Float kMaxVerticalSpeedMps = 30.0;
  KERN_CONST Float kMaxVerticalDescentAccelerationMps2 = 6.8;
  KERN_CONST Float kMaxVerticalAccelerationMps2 = 50.0;
  KERN_CONST Float kVerticalJerkLimitMps3 = 1200.0;
  KERN_CONST Float kGravityMps2 = 9.80665;
  /// Keep enough collective thrust during aggressive descent for the inner attitude loop to retain real motor/torque authority. 0.5 m/s² was nearly free-fall and allowed the physical airframe to tumble well beyond the bounded attitude target. 3.0 m/s² (≈30 % hover thrust) keeps the props spinning with differential authority while sinking hard.
  KERN_CONST Float kMinSpecificUpMps2 = 3.0;
  KERN_CONST Float kMaxSpecificUpMps2 = 60.0;
  KERN_CONST Float kHorizontalIntegralNeutralTargetMps = 0.05;
  KERN_CONST Float kHorizontalNeutralDampingScale = 1.0;
  KERN_CONST Float kHorizontalVelocityGain = 0.8;
  KERN_CONST Float kHorizontalAccelerationDamping = 0.55;
  KERN_CONST Float kMaxHorizontalAccelerationMps2 = 7.5;
  KERN_CONST Float kMaxAutoTranslationTiltTangent = 0.46630766;
  KERN_CONST Float kHorizontalIntegralGain = 0.0;
  KERN_CONST Float kHorizontalAntiWindupGain = 0.0;
  KERN_CONST Float kHorizontalIntegralLimitMps2 = 7.0;
  KERN_CONST Float kMaxTiltDeg = 40.0;
  KERN_CONST Float kInnerAttitudeRangeDeg = 40.0;
  KERN_CONST Float kMaxAttitudeCommand = 1.0;
  KERN_CONST Float kHeadingKp = 2.2;
  KERN_CONST Float kHoverAdapt = 0.12;
  KERN_CONST Float kMinHoverTrim = 0.25;
  KERN_CONST Float kMaxHoverTrim = 0.65;
  KERN_CONST Float kEscCommandOffset = 0.05;
  KERN_CONST Float kEscCommandScale = 0.95;
  KERN_CONST Float kMinFlightThrottle = 0.08;
  KERN_CONST Float kMaxFlightThrottle = 1.0;
  KERN_CONST Float kDegradedMaxTiltDeg = 12.0;
  KERN_CONST Float kDegradedBodyPitchScale = 0.35;
  KERN_CONST U16 kStateGameMode = 64;
  KERN_CONST U16 kStateNavigationDegraded = 128;
  KERN_CONST U16 kStateNavigationValid = 32;
  KERN_CONST Float kMaxAutoTranslationTiltDeg = 25.0;

  // ---- functions ----
  struct navigation_velocity_validResult { Bool Result = false; };
  KERN_PURE navigation_velocity_validResult navigation_velocity_valid(const fc_NavigationState& n) {
    return {logic_and(logic_or(n.velocity_valid, n.valid), Arondight45_DroneFC_Core::finite(n.velocity_world_mps).Result)};
  }

  struct navigation_agl_validResult { Bool Result = false; };
  KERN_PURE navigation_agl_validResult navigation_agl_valid(const fc_NavigationState& n) {
    return {logic_and(logic_and(logic_and(logic_or(n.agl_valid, n.valid), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, n.agl_m), 0.0)), math_compare<">=">(n.agl_m, 0.0)), math_compare<"<">(n.agl_m, 1000.0))};
  }

  struct navigation_heading_validResult { Bool Result = false; };
  KERN_PURE navigation_heading_validResult navigation_heading_valid(const fc_NavigationState& n) {
    return {logic_and(logic_and(logic_and(n.heading_valid, math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, n.heading_deg), 0.0)), math_compare<">=">(n.heading_deg, -180.0)), math_compare<"<=">(n.heading_deg, 180.0))};
  }

  struct finiteResult { Bool Result = false; };
  KERN_PURE finiteResult finite(const fc_NavigationState& n) {
    return {logic_and(navigation_velocity_valid(n).Result, navigation_agl_valid(n).Result)};
  }

  struct wrap_degreesResult { Float Result = 0.0; };
  KERN_PURE wrap_degreesResult wrap_degrees(Float value = 0.0) {
    Float value_ = 0.0;
    value_ = value;
    while (loop(math_compare<">">(value_, 180.0))) {
      value_ = kern::formula([](Float value_) -> Float { return (value_ - 360.0); }, value_);
    }
    while (loop(math_compare<"<">(value_, -180.0))) {
      value_ += 360.0;
    }
    return {value_};
  }

  struct state_intentResult { fc_StateIntent Result = fc_StateIntent{0.0, 0.0, 0.0, 0.0, 2.0, false, false}; };
  KERN_PURE state_intentResult state_intent(const fc_RC& rc) {
    Float right = 0.0;
    Float forward = 0.0;
    Float magnitude = 0.0;
    Float shaped_magnitude = 0.0;
    Float radial_scale = 0.0;
    Float clearance01 = 0.0;
    Float body_pitch_deg = 0.0;
    right = Arondight45_DroneFC_Core::centered(rc.ch[0]).Result;
    forward = Arondight45_DroneFC_Core::centered(rc.ch[1]).Result;
    magnitude = kern::formula([](Float forward, Float right) -> Float { return kern::math::sqrt(((forward * forward) + (right * right))); }, forward, right);
    if (math_compare<">">(magnitude, 1.0)) {
      forward = kern::formula([](Float forward, Float magnitude) -> Float { return kern::math::div(forward, magnitude); }, forward, magnitude);
      right = kern::formula([](Float right, Float magnitude) -> Float { return kern::math::div(right, magnitude); }, right, magnitude);
      magnitude = 1.0;
    }
    if (math_compare<">">(magnitude, 1e-06)) {
      shaped_magnitude = Arondight45_DroneFC_Core::shape(magnitude, 0.035, 0.25).Result;
      radial_scale = kern::formula([](Float shaped_magnitude, Float magnitude) -> Float { return kern::math::div(shaped_magnitude, magnitude); }, shaped_magnitude, magnitude);
      forward = kern::formula([](Float forward, Float radial_scale) -> Float { return (forward * radial_scale); }, forward, radial_scale);
      right = kern::formula([](Float right, Float radial_scale) -> Float { return (right * radial_scale); }, right, radial_scale);
    } else {
      forward = 0.0;
      right = 0.0;
    }
    clearance01 = Arondight45_DroneFC_Core::throttle(rc.ch[kStateClearanceChannel]).Result;
    body_pitch_deg = kern::formula([](Float the_result, Float kStateMaxBodyPitchDeg) -> Float { return (the_result * kStateMaxBodyPitchDeg); }, Arondight45_DroneFC_Core::shape(Arondight45_DroneFC_Core::centered(rc.ch[kStateBodyPitchChannel]).Result, 0.045, 0.2).Result, kStateMaxBodyPitchDeg);
    return {struct_make<fc_StateIntent>(kern::formula([](Float right, Float kStateMaxHorizontalSpeedMps) -> Float { return (right * kStateMaxHorizontalSpeedMps); }, right, kStateMaxHorizontalSpeedMps), kern::formula([](Float forward, Float kStateMaxHorizontalSpeedMps) -> Float { return (forward * kStateMaxHorizontalSpeedMps); }, forward, kStateMaxHorizontalSpeedMps), kern::formula([](Float the_result, Float kStateMaxYawRateDps) -> Float { return (the_result * kStateMaxYawRateDps); }, Arondight45_DroneFC_Core::shape(Arondight45_DroneFC_Core::centered(rc.ch[3]).Result, 0.045, 0.2).Result, kStateMaxYawRateDps), body_pitch_deg, kern::formula([](Float kStateMinClearanceM, Float clearance01, Float kStateMaxClearanceM) -> Float { return (kStateMinClearanceM + (clearance01 * (kStateMaxClearanceM - kStateMinClearanceM))); }, kStateMinClearanceM, clearance01, kStateMaxClearanceM), math_compare<">">(kern::to<Float>(rc.ch[4]), 1300.0), math_compare<">">(kern::to<Float>(rc.ch[kStateModeChannel]), 1300.0))};
  }

  void StateController_reset_horizontal_state(fc_StateController& self) {
    self.acceleration_estimator_valid_ = false;
    self.acceleration_sample_dt_s_ = 0.0;
    self.previous_velocity_world_mps_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
    self.measured_acceleration_world_mps2_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
    self.horizontal_integral_forward_mps2_ = 0.0;
    self.horizontal_integral_right_mps2_ = 0.0;
  }

  void StateController_reset_command_targets(fc_StateController& self) {
    self.horizontal_target_initialized_ = false;
    self.vertical_accel_initialized_ = false;
    self.commanded_forward_mps_ = 0.0;
    self.commanded_right_mps_ = 0.0;
    self.vertical_accel_state_mps2_ = 0.0;
  }

  void StateController_reset(fc_StateController& self) {
    self.active_ = false;
    self.heading_source_valid_ = false;
    self.manual_pitch_active_ = false;
    self.target_yaw_deg_ = 0.0;
    self.hover_trim_ = kInitialHoverThrottle;
    StateController_reset_horizontal_state(self);
    StateController_reset_command_targets(self);
    self.debug_ = struct_make<fc_StateControllerDebug>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  }

  void StateController_leave_mode(fc_StateController& self) {
    self.active_ = false;
    self.heading_source_valid_ = false;
    self.manual_pitch_active_ = false;
    StateController_reset_horizontal_state(self);
    StateController_reset_command_targets(self);
    self.debug_ = struct_make<fc_StateControllerDebug>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  }

  void StateController_update_command_targets(fc_StateController& self, const fc_StateIntent& intent, Float arg3 = 0.0) {
    self.commanded_forward_mps_ = intent.forward_mps;
    self.commanded_right_mps_ = intent.right_mps;
    self.horizontal_target_initialized_ = true;
  }

  void StateController_update_acceleration_estimator(fc_StateController& self, fc_V3 velocity_world_mps = fc_V3{0.0, 0.0, 0.0}, Float dt = 0.0) {
    Float dx = 0.0;
    Float dy = 0.0;
    Float dz = 0.0;
    Float sample_dt = 0.0;
    Float inv_dt = 0.0;
    fc_V3 sample = fc_V3{0.0, 0.0, 0.0};
    Float horizontal_sample = 0.0;
    Float alpha = 0.0;
    if (logic_not(self.acceleration_estimator_valid_)) {
      self.previous_velocity_world_mps_ = velocity_world_mps;
      self.acceleration_estimator_valid_ = true;
      self.acceleration_sample_dt_s_ = 0.0;
      return;
    }
    self.acceleration_sample_dt_s_ = kern::formula([](Float acceleration_sample_dt_s_, Float dt) -> Float { return kern::math::min((acceleration_sample_dt_s_ + dt), 0.05); }, self.acceleration_sample_dt_s_, dt);
    dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, velocity_world_mps.x, self.previous_velocity_world_mps_.x);
    dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, velocity_world_mps.y, self.previous_velocity_world_mps_.y);
    dz = kern::formula([](Float z, Float z_2) -> Float { return (z - z_2); }, velocity_world_mps.z, self.previous_velocity_world_mps_.z);
    if (logic_or(math_compare<"<">(kern::formula([](Float dx, Float dy, Float dz) -> Float { return (((dx * dx) + (dy * dy)) + (dz * dz)); }, dx, dy, dz), 1e-10), math_compare<"<">(self.acceleration_sample_dt_s_, 0.002))) {
      return;
    }
    sample_dt = self.acceleration_sample_dt_s_;
    inv_dt = kern::formula([](Float sample_dt) -> Float { return kern::math::div(1.0, sample_dt); }, sample_dt);
    sample = struct_make<fc_V3>(kern::formula([](Float dx, Float inv_dt) -> Float { return (dx * inv_dt); }, dx, inv_dt), kern::formula([](Float dy, Float inv_dt) -> Float { return (dy * inv_dt); }, dy, inv_dt), kern::formula([](Float dz, Float inv_dt) -> Float { return (dz * inv_dt); }, dz, inv_dt));
    self.previous_velocity_world_mps_ = velocity_world_mps;
    self.acceleration_sample_dt_s_ = 0.0;
    horizontal_sample = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt(((x * x) + (y * y))); }, sample.x, sample.y);
    if (logic_or(logic_not(Arondight45_DroneFC_Core::finite(sample).Result), math_compare<">">(horizontal_sample, kMaxNavigationAccelSampleMps2))) {
      self.measured_acceleration_world_mps2_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
      return;
    }
    alpha = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float sample_dt, Float kMeasuredAccelerationFilterTauS) -> Float { return kern::math::div(sample_dt, (kMeasuredAccelerationFilterTauS + sample_dt)); }, sample_dt, kMeasuredAccelerationFilterTauS), 0.0, 1.0));
    self.measured_acceleration_world_mps2_.x = kern::formula([](Float x, Float alpha, Float x_2) -> Float { return (x + (alpha * (x_2 - x))); }, self.measured_acceleration_world_mps2_.x, alpha, sample.x);
    self.measured_acceleration_world_mps2_.y = kern::formula([](Float y, Float alpha, Float y_2) -> Float { return (y + (alpha * (y_2 - y))); }, self.measured_acceleration_world_mps2_.y, alpha, sample.y);
    self.measured_acceleration_world_mps2_.z = kern::formula([](Float z, Float alpha, Float z_2) -> Float { return (z + (alpha * (z_2 - z))); }, self.measured_acceleration_world_mps2_.z, alpha, sample.z);
  }

  struct StateController_kinematic_vertical_speedResult { Float Result = 0.0; };
  /// min(linear, braking curve): the braking curve v = -a·tau + sqrt((a·tau)^2 + 2·a·e) already budgets the vertical-velocity loop's own lag tau = 1/gain, so a full-rate sink is stopped at the target, not below it.
  KERN_PURE StateController_kinematic_vertical_speedResult StateController_kinematic_vertical_speed(Float agl_error = 0.0) {
    Bool up = false;
    Float a = 0.0;
    Float tau = 0.0;
    Float e = 0.0;
    Float at = 0.0;
    Float v = 0.0;
    up = math_compare<">=">(agl_error, 0.0);
    a = kern::formula([](Float up, Float kClimbBrakeAccelerationMps2, Float kDescentBrakeAccelerationMps2) -> Float { return ((up != 0.0) ? kClimbBrakeAccelerationMps2 : kDescentBrakeAccelerationMps2); }, kern::to<Float>(up), kClimbBrakeAccelerationMps2, kDescentBrakeAccelerationMps2);
    tau = kern::formula([](Float up, Float kVerticalVelocityGain, Float kVerticalDescentVelocityGain) -> Float { return kern::math::div(1.0, ((up != 0.0) ? kVerticalVelocityGain : kVerticalDescentVelocityGain)); }, kern::to<Float>(up), kVerticalVelocityGain, kVerticalDescentVelocityGain);
    e = kern::formula([](Float agl_error) -> Float { return kern::math::abs(agl_error); }, agl_error);
    at = kern::formula([](Float a, Float tau) -> Float { return (a * tau); }, a, tau);
    v = kern::formula([](Float kAglToVerticalSpeed, Float e, Float at, Float a) -> Float { return kern::math::min((kAglToVerticalSpeed * e), ((-at) + kern::math::sqrt(((at * at) + ((2.0 * a) * e))))); }, kAglToVerticalSpeed, e, at, a);
    return {kern::formula([](Float up, Float v) -> Float { return ((up != 0.0) ? v : (-v)); }, kern::to<Float>(up), v)};
  }

  struct StateController_limit_vertical_accel_jerkResult { Float Result = 0.0; };
  StateController_limit_vertical_accel_jerkResult StateController_limit_vertical_accel_jerk(fc_StateController& self, Float requested_accel = 0.0, Float dt = 0.0) {
    Float max_delta = 0.0;
    if (logic_not(self.vertical_accel_initialized_)) {
      self.vertical_accel_state_mps2_ = requested_accel;
      self.vertical_accel_initialized_ = true;
      return {self.vertical_accel_state_mps2_};
    }
    max_delta = kern::formula([](Float kVerticalJerkLimitMps3, Float dt) -> Float { return (kVerticalJerkLimitMps3 * dt); }, kVerticalJerkLimitMps3, dt);
    self.vertical_accel_state_mps2_ = kern::formula([](Float vertical_accel_state_mps2_, Float a) -> Float { return (vertical_accel_state_mps2_ + a); }, self.vertical_accel_state_mps2_, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float requested_accel, Float vertical_accel_state_mps2_) -> Float { return (requested_accel - vertical_accel_state_mps2_); }, requested_accel, self.vertical_accel_state_mps2_), kern::formula([](Float max_delta) -> Float { return (-max_delta); }, max_delta), max_delta));
    return {self.vertical_accel_state_mps2_};
  }

  void StateController_clamp_horizontal_integral(fc_StateController& self) {
    Float magnitude = 0.0;
    Float scale = 0.0;
    magnitude = kern::formula([](Float horizontal_integral_forward_mps2_, Float horizontal_integral_right_mps2_) -> Float { return kern::math::sqrt(((horizontal_integral_forward_mps2_ * horizontal_integral_forward_mps2_) + (horizontal_integral_right_mps2_ * horizontal_integral_right_mps2_))); }, self.horizontal_integral_forward_mps2_, self.horizontal_integral_right_mps2_);
    if (logic_and(math_compare<">">(magnitude, kHorizontalIntegralLimitMps2), math_compare<">">(magnitude, 1e-06))) {
      scale = kern::formula([](Float kHorizontalIntegralLimitMps2, Float magnitude) -> Float { return kern::math::div(kHorizontalIntegralLimitMps2, magnitude); }, kHorizontalIntegralLimitMps2, magnitude);
      self.horizontal_integral_forward_mps2_ = kern::formula([](Float horizontal_integral_forward_mps2_, Float scale) -> Float { return (horizontal_integral_forward_mps2_ * scale); }, self.horizontal_integral_forward_mps2_, scale);
      self.horizontal_integral_right_mps2_ = kern::formula([](Float horizontal_integral_right_mps2_, Float scale) -> Float { return (horizontal_integral_right_mps2_ * scale); }, self.horizontal_integral_right_mps2_, scale);
    }
  }

  struct StateController_runResult { fc_Command Result = fc_Command{0.0, 0.0, 0.0, 0.0, false}; };
  StateController_runResult StateController_run(fc_StateController& self, const fc_RC& receiver, const fc_NavigationState& nav, Float yaw_deg = 0.0, Bool inner_armed = false, Float dt = 0.0, Bool agl_valid = false) {
    Float dt_ = 0.0;
    fc_StateIntent intent = fc_StateIntent{0.0, 0.0, 0.0, 0.0, 2.0, false, false};
    Bool absolute_heading_valid = false;
    Float measured_yaw_deg = 0.0;
    Float yaw_rad = 0.0;
    Float c = 0.0;
    Float s = 0.0;
    Float measured_forward = 0.0;
    Float measured_right = 0.0;
    Bool manual_pitch_active = false;
    Bool manual_pitch_released = false;
    Float measured_forward_accel = 0.0;
    Float measured_right_accel = 0.0;
    Bool agl_usable = false;
    Float agl_m = 0.0;
    Float agl_error = 0.0;
    Float target_vz = 0.0;
    Float vz_error = 0.0;
    Float vertical_velocity_gain = 0.0;
    Float requested_vertical_accel = 0.0;
    Float vertical_accel = 0.0;
    Float specific_up = 0.0;
    Float forward_error = 0.0;
    Float right_error = 0.0;
    Float pilot_horizontal_speed = 0.0;
    Float neutral_damping_scale = 0.0;
    Float forward_feedback_unsat = 0.0;
    Float right_unsat = 0.0;
    Float forward_accel = 0.0;
    Float right_accel = 0.0;
    Float allowed_horizontal_accel = 0.0;
    Float horizontal_accel = 0.0;
    Float the_vector_scale = 0.0;
    Float auto_pitch_target_deg = 0.0;
    Float pitch_target_deg = 0.0;
    Float roll_target_deg = 0.0;
    Float roll_command = 0.0;
    Float pitch_command = 0.0;
    Float yaw_error = 0.0;
    Float desired_yaw_rate = 0.0;
    Float yaw_command = 0.0;
    Bool hovering = false;
    Bool cannot_lift = false;
    Float roll_rad = 0.0;
    Float pitch_rad = 0.0;
    Float vertical_thrust_fraction = 0.0;
    Float required_specific_force = 0.0;
    Float thrust_ratio = 0.0;
    Float hover_motor_command = 0.0;
    Float required_motor_command = 0.0;
    Float throttle_command = 0.0;
    dt_ = dt;
    intent = state_intent(receiver).Result;
    absolute_heading_valid = navigation_heading_valid(nav).Result;
    measured_yaw_deg = wrap_degrees(kern::formula([](Float absolute_heading_valid, Float heading_deg, Float yaw_deg) -> Float { return ((absolute_heading_valid != 0.0) ? heading_deg : yaw_deg); }, kern::to<Float>(absolute_heading_valid), nav.heading_deg, yaw_deg)).Result;
    if (logic_or(logic_not(self.active_), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(self.heading_source_valid_))), kern::to<Float>(value_int(kern::to<Int>(absolute_heading_valid)))))) {
      self.target_yaw_deg_ = measured_yaw_deg;
      self.heading_source_valid_ = absolute_heading_valid;
      self.manual_pitch_active_ = false;
      StateController_reset_horizontal_state(self);
      StateController_reset_command_targets(self);
      self.active_ = true;
    }
    dt_ = kern::formula([](Float dt_) -> Float { return (dt_ < 0.0002 ? 0.0002 : (0.02 < dt_ ? 0.02 : dt_)); }, dt_);
    self.target_yaw_deg_ = wrap_degrees(kern::formula([](Float target_yaw_deg_, Float yaw_rate_dps, Float dt_) -> Float { return (target_yaw_deg_ + (yaw_rate_dps * dt_)); }, self.target_yaw_deg_, intent.yaw_rate_dps, dt_)).Result;
    yaw_rad = kern::formula([](Float measured_yaw_deg, Float kPi) -> Float { return kern::math::div((measured_yaw_deg * kPi), 180.0); }, measured_yaw_deg, kPi);
    c = kern::formula([](Float yaw_rad) -> Float { return kern::math::cos(yaw_rad); }, yaw_rad);
    s = kern::formula([](Float yaw_rad) -> Float { return kern::math::sin(yaw_rad); }, yaw_rad);
    measured_forward = kern::formula([](Float c, Float x, Float s, Float y) -> Float { return (((-c) * x) - (s * y)); }, c, nav.velocity_world_mps.x, s, nav.velocity_world_mps.y);
    measured_right = kern::formula([](Float s, Float x, Float c, Float y) -> Float { return (((-s) * x) + (c * y)); }, s, nav.velocity_world_mps.x, c, nav.velocity_world_mps.y);
    if (logic_not(inner_armed)) {
      self.agl_estimate_valid_ = false;
      StateController_reset_horizontal_state(self);
      StateController_reset_command_targets(self);
      self.manual_pitch_active_ = false;
      self.target_yaw_deg_ = measured_yaw_deg;
      self.heading_source_valid_ = absolute_heading_valid;
      self.debug_ = struct_make<fc_StateControllerDebug>(intent.forward_mps, measured_forward, intent.right_mps, measured_right, self.target_yaw_deg_, measured_yaw_deg, intent.clearance_m, nav.agl_m, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      return {struct_make<fc_Command>(0.0, 0.0, 0.0, 0.0, intent.arm)};
    }
    manual_pitch_active = math_compare<">=">(kern::formula([](Float body_pitch_deg) -> Float { return kern::math::abs(body_pitch_deg); }, intent.body_pitch_deg), kManualPitchOverrideThresholdDeg);
    manual_pitch_released = logic_and(self.manual_pitch_active_, logic_not(manual_pitch_active));
    self.manual_pitch_active_ = manual_pitch_active;
    if (manual_pitch_released) {
      self.commanded_forward_mps_ = measured_forward;
      self.horizontal_target_initialized_ = true;
      self.acceleration_estimator_valid_ = false;
      self.acceleration_sample_dt_s_ = 0.0;
      self.previous_velocity_world_mps_ = nav.velocity_world_mps;
      self.measured_acceleration_world_mps2_ = struct_make<fc_V3>(0.0, 0.0, 0.0);
      self.horizontal_integral_forward_mps2_ = 0.0;
    }
    StateController_update_command_targets(self, intent, dt_);
    if (manual_pitch_active) {
      self.commanded_forward_mps_ = measured_forward;
      self.horizontal_integral_forward_mps2_ = 0.0;
    }
    StateController_update_acceleration_estimator(self, nav.velocity_world_mps, dt_);
    measured_forward_accel = kern::formula([](Float c, Float x, Float s, Float y) -> Float { return (((-c) * x) - (s * y)); }, c, self.measured_acceleration_world_mps2_.x, s, self.measured_acceleration_world_mps2_.y);
    measured_right_accel = kern::formula([](Float s, Float x, Float c, Float y) -> Float { return (((-s) * x) + (c * y)); }, s, self.measured_acceleration_world_mps2_.x, c, self.measured_acceleration_world_mps2_.y);
    agl_usable = agl_valid;
    agl_m = nav.agl_m;
    if (agl_valid) {
      self.agl_estimate_m_ = nav.agl_m;
      self.agl_estimate_valid_ = true;
    } else if (logic_and(self.agl_estimate_valid_, math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, nav.velocity_world_mps.z), 0.0))) {
      self.agl_estimate_m_ = kern::formula([](Float agl_estimate_m_, Float z, Float dt_) -> Float { return (agl_estimate_m_ + (z * dt_)); }, self.agl_estimate_m_, nav.velocity_world_mps.z, dt_);
      agl_m = self.agl_estimate_m_;
      agl_usable = true;
    }
    agl_error = kern::formula([](Float agl_usable, Float clearance_m, Float agl_m) -> Float { return ((agl_usable != 0.0) ? (clearance_m - agl_m) : 0.0); }, kern::to<Float>(agl_usable), intent.clearance_m, agl_m);
    target_vz = kern::formula([](Float agl_usable, Float the_result, Float kMaxVerticalDescentSpeedMps, Float kMaxVerticalSpeedMps) -> Float { return ((agl_usable != 0.0) ? (the_result < (-kMaxVerticalDescentSpeedMps) ? (-kMaxVerticalDescentSpeedMps) : (kMaxVerticalSpeedMps < the_result ? kMaxVerticalSpeedMps : the_result)) : 0.0); }, kern::to<Float>(agl_usable), StateController_kinematic_vertical_speed(agl_error).Result, kMaxVerticalDescentSpeedMps, kMaxVerticalSpeedMps);
    vz_error = kern::formula([](Float target_vz, Float z) -> Float { return (target_vz - z); }, target_vz, nav.velocity_world_mps.z);
    vertical_velocity_gain = kern::formula([](Float target_vz, Float kVerticalDescentVelocityGain, Float kVerticalVelocityGain) -> Float { return ((target_vz < 0.0) ? kVerticalDescentVelocityGain : kVerticalVelocityGain); }, target_vz, kVerticalDescentVelocityGain, kVerticalVelocityGain);
    requested_vertical_accel = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float vertical_velocity_gain, Float vz_error) -> Float { return (vertical_velocity_gain * vz_error); }, vertical_velocity_gain, vz_error), kern::formula([](Float kMaxVerticalDescentAccelerationMps2) -> Float { return (-kMaxVerticalDescentAccelerationMps2); }, kMaxVerticalDescentAccelerationMps2), kMaxVerticalAccelerationMps2));
    n10 = StateController_limit_vertical_accel_jerk(self, requested_vertical_accel, dt_);
    vertical_accel = n10.Result;
    specific_up = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float kGravityMps2, Float vertical_accel) -> Float { return (kGravityMps2 + vertical_accel); }, kGravityMps2, vertical_accel), kMinSpecificUpMps2, kMaxSpecificUpMps2));
    forward_error = kern::formula([](Float commanded_forward_mps_, Float measured_forward) -> Float { return (commanded_forward_mps_ - measured_forward); }, self.commanded_forward_mps_, measured_forward);
    right_error = kern::formula([](Float commanded_right_mps_, Float measured_right) -> Float { return (commanded_right_mps_ - measured_right); }, self.commanded_right_mps_, measured_right);
    pilot_horizontal_speed = kern::formula([](Float forward_mps, Float right_mps) -> Float { return kern::math::sqrt(((forward_mps * forward_mps) + (right_mps * right_mps))); }, intent.forward_mps, intent.right_mps);
    if (math_compare<"<=">(pilot_horizontal_speed, kHorizontalIntegralNeutralTargetMps)) {
      self.horizontal_integral_forward_mps2_ = 0.0;
      self.horizontal_integral_right_mps2_ = 0.0;
    }
    neutral_damping_scale = kern::formula([](Float pilot_horizontal_speed, Float kHorizontalIntegralNeutralTargetMps, Float kHorizontalNeutralDampingScale) -> Float { return ((pilot_horizontal_speed <= kHorizontalIntegralNeutralTargetMps) ? kHorizontalNeutralDampingScale : 1.0); }, pilot_horizontal_speed, kHorizontalIntegralNeutralTargetMps, kHorizontalNeutralDampingScale);
    forward_feedback_unsat = kern::formula([](Float kHorizontalVelocityGain, Float commanded_forward_mps_, Float measured_forward, Float horizontal_integral_forward_mps2_, Float kHorizontalAccelerationDamping, Float neutral_damping_scale, Float measured_forward_accel) -> Float { return (((kHorizontalVelocityGain * (commanded_forward_mps_ - measured_forward)) + horizontal_integral_forward_mps2_) - ((kHorizontalAccelerationDamping * neutral_damping_scale) * measured_forward_accel)); }, kHorizontalVelocityGain, self.commanded_forward_mps_, measured_forward, self.horizontal_integral_forward_mps2_, kHorizontalAccelerationDamping, neutral_damping_scale, measured_forward_accel);
    right_unsat = kern::formula([](Float kHorizontalVelocityGain, Float commanded_right_mps_, Float measured_right, Float horizontal_integral_right_mps2_, Float kHorizontalAccelerationDamping, Float neutral_damping_scale, Float measured_right_accel) -> Float { return (((kHorizontalVelocityGain * (commanded_right_mps_ - measured_right)) + horizontal_integral_right_mps2_) - ((kHorizontalAccelerationDamping * neutral_damping_scale) * measured_right_accel)); }, kHorizontalVelocityGain, self.commanded_right_mps_, measured_right, self.horizontal_integral_right_mps2_, kHorizontalAccelerationDamping, neutral_damping_scale, measured_right_accel);
    forward_accel = kern::formula([](Float manual_pitch_active, Float forward_feedback_unsat) -> Float { return ((manual_pitch_active != 0.0) ? 0.0 : forward_feedback_unsat); }, kern::to<Float>(manual_pitch_active), forward_feedback_unsat);
    right_accel = right_unsat;
    allowed_horizontal_accel = kern::formula([](Float kMaxHorizontalAccelerationMps2, Float specific_up, Float kMaxAutoTranslationTiltTangent) -> Float { return kern::math::min(kMaxHorizontalAccelerationMps2, (specific_up * kMaxAutoTranslationTiltTangent)); }, kMaxHorizontalAccelerationMps2, specific_up, kMaxAutoTranslationTiltTangent);
    horizontal_accel = kern::formula([](Float forward_accel, Float right_accel) -> Float { return kern::math::sqrt(((forward_accel * forward_accel) + (right_accel * right_accel))); }, forward_accel, right_accel);
    if (logic_and(math_compare<">">(horizontal_accel, allowed_horizontal_accel), math_compare<">">(horizontal_accel, 1e-06))) {
      the_vector_scale = kern::formula([](Float allowed_horizontal_accel, Float horizontal_accel) -> Float { return kern::math::div(allowed_horizontal_accel, horizontal_accel); }, allowed_horizontal_accel, horizontal_accel);
      forward_accel = kern::formula([](Float forward_accel, Float the_vector_scale) -> Float { return (forward_accel * the_vector_scale); }, forward_accel, the_vector_scale);
      right_accel = kern::formula([](Float right_accel, Float the_vector_scale) -> Float { return (right_accel * the_vector_scale); }, right_accel, the_vector_scale);
    }
    if (math_compare<">">(pilot_horizontal_speed, kHorizontalIntegralNeutralTargetMps)) {
      if (logic_not(manual_pitch_active)) {
        self.horizontal_integral_forward_mps2_ = kern::formula([](Float horizontal_integral_forward_mps2_, Float kHorizontalIntegralGain, Float forward_error, Float kHorizontalAntiWindupGain, Float forward_accel, Float forward_feedback_unsat, Float dt_) -> Float { return (horizontal_integral_forward_mps2_ + (((kHorizontalIntegralGain * forward_error) + (kHorizontalAntiWindupGain * (forward_accel - forward_feedback_unsat))) * dt_)); }, self.horizontal_integral_forward_mps2_, kHorizontalIntegralGain, forward_error, kHorizontalAntiWindupGain, forward_accel, forward_feedback_unsat, dt_);
      }
      self.horizontal_integral_right_mps2_ = kern::formula([](Float horizontal_integral_right_mps2_, Float kHorizontalIntegralGain, Float right_error, Float kHorizontalAntiWindupGain, Float right_accel, Float right_unsat, Float dt_) -> Float { return (horizontal_integral_right_mps2_ + (((kHorizontalIntegralGain * right_error) + (kHorizontalAntiWindupGain * (right_accel - right_unsat))) * dt_)); }, self.horizontal_integral_right_mps2_, kHorizontalIntegralGain, right_error, kHorizontalAntiWindupGain, right_accel, right_unsat, dt_);
      StateController_clamp_horizontal_integral(self);
    }
    auto_pitch_target_deg = kern::formula([](Float forward_accel, Float specific_up, Float kPi) -> Float { return kern::math::div(((-kern::math::atan2(forward_accel, specific_up)) * 180.0), kPi); }, forward_accel, specific_up, kPi);
    pitch_target_deg = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float auto_pitch_target_deg, Float body_pitch_deg) -> Float { return (auto_pitch_target_deg + body_pitch_deg); }, auto_pitch_target_deg, intent.body_pitch_deg), kern::formula([](Float kMaxTiltDeg) -> Float { return (-kMaxTiltDeg); }, kMaxTiltDeg), kMaxTiltDeg));
    roll_target_deg = kern::formula([](Float right_accel, Float specific_up, Float forward_accel, Float kPi) -> Float { return kern::math::div(((-kern::math::atan2(right_accel, kern::math::sqrt(((specific_up * specific_up) + (forward_accel * forward_accel))))) * 180.0), kPi); }, right_accel, specific_up, forward_accel, kPi);
    roll_command = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float roll_target_deg, Float kInnerAttitudeRangeDeg) -> Float { return kern::math::div(roll_target_deg, kInnerAttitudeRangeDeg); }, roll_target_deg, kInnerAttitudeRangeDeg), kern::formula([](Float kMaxAttitudeCommand) -> Float { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand));
    pitch_command = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float pitch_target_deg, Float kInnerAttitudeRangeDeg) -> Float { return kern::math::div(pitch_target_deg, kInnerAttitudeRangeDeg); }, pitch_target_deg, kInnerAttitudeRangeDeg), kern::formula([](Float kMaxAttitudeCommand) -> Float { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand));
    yaw_error = wrap_degrees(kern::formula([](Float target_yaw_deg_, Float measured_yaw_deg) -> Float { return (target_yaw_deg_ - measured_yaw_deg); }, self.target_yaw_deg_, measured_yaw_deg)).Result;
    desired_yaw_rate = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float yaw_rate_dps, Float kHeadingKp, Float yaw_error) -> Float { return (yaw_rate_dps + (kHeadingKp * yaw_error)); }, intent.yaw_rate_dps, kHeadingKp, yaw_error), -180.0, 180.0));
    yaw_command = kern::formula([](Float desired_yaw_rate) -> Float { return kern::math::div(desired_yaw_rate, 180.0); }, desired_yaw_rate);
    hovering = math_compare<"<">(kern::formula([](Float target_vz) -> Float { return kern::math::abs(target_vz); }, target_vz), 1.5);
    cannot_lift = logic_and(logic_and(math_compare<">">(target_vz, 0.0), math_compare<">">(vz_error, 0.0)), math_compare<"<">(nav.velocity_world_mps.z, 0.5));
    if (logic_and(agl_usable, logic_or(hovering, cannot_lift))) {
      self.hover_trim_ = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float hover_trim_, Float kHoverAdapt, Float vz_error, Float dt_) -> Float { return (hover_trim_ + ((kHoverAdapt * vz_error) * dt_)); }, self.hover_trim_, kHoverAdapt, vz_error, dt_), kMinHoverTrim, kMaxHoverTrim));
    }
    roll_rad = kern::formula([](Float roll_target_deg, Float kPi) -> Float { return kern::math::div((roll_target_deg * kPi), 180.0); }, roll_target_deg, kPi);
    pitch_rad = kern::formula([](Float pitch_target_deg, Float kPi) -> Float { return kern::math::div((pitch_target_deg * kPi), 180.0); }, pitch_target_deg, kPi);
    vertical_thrust_fraction = kern::formula([](Float roll_rad, Float pitch_rad) -> Float { return kern::math::max(0.35, (kern::math::cos(roll_rad) * kern::math::cos(pitch_rad))); }, roll_rad, pitch_rad);
    required_specific_force = kern::formula([](Float specific_up, Float vertical_thrust_fraction) -> Float { return kern::math::div(specific_up, vertical_thrust_fraction); }, specific_up, vertical_thrust_fraction);
    thrust_ratio = kern::formula([](Float required_specific_force, Float kGravityMps2) -> Float { return kern::math::div(required_specific_force, kGravityMps2); }, required_specific_force, kGravityMps2);
    hover_motor_command = kern::formula([](Float kEscCommandOffset, Float kEscCommandScale, Float hover_trim_) -> Float { return (kEscCommandOffset + (kEscCommandScale * hover_trim_)); }, kEscCommandOffset, kEscCommandScale, self.hover_trim_);
    required_motor_command = kern::formula([](Float hover_motor_command, Float thrust_ratio) -> Float { return (hover_motor_command * kern::math::sqrt(thrust_ratio)); }, hover_motor_command, thrust_ratio);
    throttle_command = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float required_motor_command, Float kEscCommandOffset, Float kEscCommandScale) -> Float { return kern::math::div((required_motor_command - kEscCommandOffset), kEscCommandScale); }, required_motor_command, kEscCommandOffset, kEscCommandScale), kMinFlightThrottle, kMaxFlightThrottle));
    self.debug_ = struct_make<fc_StateControllerDebug>(self.commanded_forward_mps_, measured_forward, self.commanded_right_mps_, measured_right, self.target_yaw_deg_, measured_yaw_deg, intent.clearance_m, agl_m, target_vz, throttle_command, roll_command, pitch_command, yaw_command, forward_accel, right_accel, vertical_accel);
    return {Arondight45_DroneFC_Core::sanitize(struct_make<fc_Command>(roll_command, pitch_command, throttle_command, yaw_command, intent.arm)).Result};
  }

  struct StateController_degraded_attitude_commandResult { fc_Command Result = fc_Command{0.0, 0.0, 0.0, 0.0, false}; };
  KERN_PURE StateController_degraded_attitude_commandResult StateController_degraded_attitude_command(const fc_StateController& self, const fc_RC& receiver) {
    fc_StateIntent intent = fc_StateIntent{0.0, 0.0, 0.0, 0.0, 2.0, false, false};
    Float right_unit = 0.0;
    Float forward_unit = 0.0;
    Float roll_target_deg = 0.0;
    Float pitch_target_deg = 0.0;
    Float roll_command = 0.0;
    Float pitch_command = 0.0;
    Float vertical_fraction = 0.0;
    Float hover_motor_command = 0.0;
    Float required_motor_command = 0.0;
    Float throttle_command = 0.0;
    intent = state_intent(receiver).Result;
    right_unit = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float right_mps, Float kStateMaxHorizontalSpeedMps) -> Float { return kern::math::div(right_mps, kStateMaxHorizontalSpeedMps); }, intent.right_mps, kStateMaxHorizontalSpeedMps), -1.0, 1.0));
    forward_unit = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float forward_mps, Float kStateMaxHorizontalSpeedMps) -> Float { return kern::math::div(forward_mps, kStateMaxHorizontalSpeedMps); }, intent.forward_mps, kStateMaxHorizontalSpeedMps), -1.0, 1.0));
    roll_target_deg = kern::formula([](Float right_unit, Float kDegradedMaxTiltDeg) -> Float { return ((-right_unit) * kDegradedMaxTiltDeg); }, right_unit, kDegradedMaxTiltDeg);
    pitch_target_deg = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float forward_unit, Float kDegradedMaxTiltDeg, Float body_pitch_deg, Float kDegradedBodyPitchScale) -> Float { return (((-forward_unit) * kDegradedMaxTiltDeg) + (body_pitch_deg * kDegradedBodyPitchScale)); }, forward_unit, kDegradedMaxTiltDeg, intent.body_pitch_deg, kDegradedBodyPitchScale), kern::formula([](Float kDegradedMaxTiltDeg) -> Float { return (-kDegradedMaxTiltDeg); }, kDegradedMaxTiltDeg), kDegradedMaxTiltDeg));
    roll_command = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float roll_target_deg, Float kInnerAttitudeRangeDeg) -> Float { return kern::math::div(roll_target_deg, kInnerAttitudeRangeDeg); }, roll_target_deg, kInnerAttitudeRangeDeg), kern::formula([](Float kMaxAttitudeCommand) -> Float { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand));
    pitch_command = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float pitch_target_deg, Float kInnerAttitudeRangeDeg) -> Float { return kern::math::div(pitch_target_deg, kInnerAttitudeRangeDeg); }, pitch_target_deg, kInnerAttitudeRangeDeg), kern::formula([](Float kMaxAttitudeCommand) -> Float { return (-kMaxAttitudeCommand); }, kMaxAttitudeCommand), kMaxAttitudeCommand));
    vertical_fraction = kern::formula([](Float roll_target_deg, Float kPi, Float pitch_target_deg) -> Float { return kern::math::max(0.5, (kern::math::cos(kern::math::div((roll_target_deg * kPi), 180.0)) * kern::math::cos(kern::math::div((pitch_target_deg * kPi), 180.0)))); }, roll_target_deg, kPi, pitch_target_deg);
    hover_motor_command = kern::formula([](Float kEscCommandOffset, Float kEscCommandScale, Float hover_trim_) -> Float { return (kEscCommandOffset + (kEscCommandScale * hover_trim_)); }, kEscCommandOffset, kEscCommandScale, self.hover_trim_);
    required_motor_command = kern::formula([](Float hover_motor_command, Float vertical_fraction) -> Float { return kern::math::div(hover_motor_command, kern::math::sqrt(vertical_fraction)); }, hover_motor_command, vertical_fraction);
    throttle_command = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float required_motor_command, Float kEscCommandOffset, Float kEscCommandScale) -> Float { return kern::math::div((required_motor_command - kEscCommandOffset), kEscCommandScale); }, required_motor_command, kEscCommandOffset, kEscCommandScale), kMinFlightThrottle, kMaxFlightThrottle));
    return {Arondight45_DroneFC_Core::sanitize(struct_make<fc_Command>(roll_command, pitch_command, throttle_command, kern::formula([](Float yaw_rate_dps) -> Float { return kern::math::div(yaw_rate_dps, 180.0); }, intent.yaw_rate_dps), intent.arm)).Result};
  }

  struct StateController_hover_trimResult { Float Result = 0.0; };
  KERN_PURE StateController_hover_trimResult StateController_hover_trim(const fc_StateController& self) {
    return {self.hover_trim_};
  }

  void StateRuntime_reset(fc_StateRuntime& self) {
    Arondight45_DroneFC_Core::Runtime_reset(self.runtime_);
    StateController_reset(self.state_controller_);
    self.last_yaw_deg_ = 0.0;
    self.game_active_ = false;
  }

  struct StateRuntime_newResult { fc_StateRuntime Result = fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}; };
  StateRuntime_newResult StateRuntime_new() {
    fc_StateRuntime self = fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false};
    n11 = Arondight45_DroneFC_Core::Runtime_new();
    self.runtime_ = n11.Result;
    StateRuntime_reset(self);
    return {self};
  }

  void StateRuntime_update_attitude(fc_StateRuntime& self, const fc_RuntimeOutput& out) {
    self.last_yaw_deg_ = kern::formula([](Float a) -> Float { return (a * 0.01); }, kern::to<Float>(out.attitude_cdeg[2]));
  }

  struct StateRuntime_stepResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<U16>{1000, 1000, 1000, 1000}, List<I16>{}, 0, fc_FaultCode::kFaultNone, false}; };
  StateRuntime_stepResult StateRuntime_step(fc_StateRuntime& self, fc_StateRuntimeInput input = fc_StateRuntimeInput{fc_RuntimeInput{fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}, fc_RC{List<U16>{}, false, false, false, Int64{0x0}}, Int64{0x0}, 1000, 0, true, false}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}}) {
    fc_StateIntent intent = fc_StateIntent{0.0, 0.0, 0.0, 0.0, 2.0, false, false};
    fc_RuntimeOutput out = fc_RuntimeOutput{List<U16>{1000, 1000, 1000, 1000}, List<I16>{}, 0, fc_FaultCode::kFaultNone, false};
    Bool receiver_valid = false;
    fc_RuntimeOutput out_2 = fc_RuntimeOutput{List<U16>{1000, 1000, 1000, 1000}, List<I16>{}, 0, fc_FaultCode::kFaultNone, false};
    Bool velocity_valid = false;
    Bool agl_valid = false;
    Bool heading_valid = false;
    Bool horizontal_navigation = false;
    Bool full_navigation = false;
    fc_RuntimeOutput out_3 = fc_RuntimeOutput{List<U16>{1000, 1000, 1000, 1000}, List<I16>{}, 0, fc_FaultCode::kFaultNone, false};
    Float dt = 0.0;
    fc_Command physical_command = fc_Command{0.0, 0.0, 0.0, 0.0, false};
    fc_RuntimeOutput out_4 = fc_RuntimeOutput{List<U16>{1000, 1000, 1000, 1000}, List<I16>{}, 0, fc_FaultCode::kFaultNone, false};
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
      n13 = Arondight45_DroneFC_Core::Runtime_step_command(self.runtime_, input.flight, struct_make<fc_Command>(0.0, 0.0, 0.0, 0.0, false), false);
      out_2 = n13.Result;
      out_2.state = kern::to<Int>(kern::formula([](Float state, Float kStateGameMode) -> Float { return kern::math::bor(state, kStateGameMode); }, kern::to<Float>(out_2.state), kern::to<Float>(kStateGameMode)));
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
      n14 = Arondight45_DroneFC_Core::Runtime_step_command(self.runtime_, input.flight, struct_make<fc_Command>(0.0, 0.0, 0.0, 0.0, false), false);
      out_3 = n14.Result;
      out_3.state = kern::to<Int>(kern::formula([](Float state, Float kStateGameMode, Float kStateNavigationDegraded) -> Float { return kern::math::bor(state, kern::math::bor(kStateGameMode, kStateNavigationDegraded)); }, kern::to<Float>(out_3.state), kern::to<Float>(kStateGameMode), kern::to<Float>(kStateNavigationDegraded)));
      StateRuntime_update_attitude(self, out_3);
      return {out_3};
    }
    dt = kern::formula([](Float dt_us) -> Float { return (((dt_us > 0.0) && (dt_us < 100000.0)) ? (dt_us * 1e-06) : 0.001); }, kern::to<Float>(input.flight.dt_us));
    physical_command = struct_make<fc_Command>(0.0, 0.0, 0.0, 0.0, false);
    if (horizontal_navigation) {
      n15 = StateController_run(self.state_controller_, input.flight.rc, input.navigation, self.last_yaw_deg_, Arondight45_DroneFC_Core::Runtime_armed(self.runtime_).Result, dt, agl_valid);
      physical_command = n15.Result;
    } else {
      StateController_leave_mode(self.state_controller_);
      physical_command = StateController_degraded_attitude_command(self.state_controller_, input.flight.rc).Result;
    }
    n16 = Arondight45_DroneFC_Core::Runtime_step_command(self.runtime_, input.flight, physical_command, true);
    out_4 = n16.Result;
    out_4.state = kern::to<Int>(kern::formula([](Float state, Float kStateGameMode) -> Float { return kern::math::bor(state, kStateGameMode); }, kern::to<Float>(out_4.state), kern::to<Float>(kStateGameMode)));
    if (full_navigation) {
      out_4.state = kern::to<Int>(kern::formula([](Float state, Float kStateNavigationValid) -> Float { return kern::math::bor(state, kStateNavigationValid); }, kern::to<Float>(out_4.state), kern::to<Float>(kStateNavigationValid)));
    } else {
      out_4.state = kern::to<Int>(kern::formula([](Float state, Float kStateNavigationDegraded) -> Float { return kern::math::bor(state, kStateNavigationDegraded); }, kern::to<Float>(out_4.state), kern::to<Float>(kStateNavigationDegraded)));
    }
    StateRuntime_update_attitude(self, out_4);
    return {out_4};
  }

  struct StateRuntime_game_activeResult { Bool Result = false; };
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
