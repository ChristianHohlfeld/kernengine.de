struct Arondight45_FirmwareRuntime : virtual kern::Library, virtual Arondight45_DroneFC_Core, virtual Arondight45_HardwareSensors, virtual Arondight45_StateControl {
  // ---- types ----
  KERN_STRUCT(fc_HardwareFrame) { Int64 now_us = Int64{0x0}; Int dt_us = 1000; Int missed_samples = 0; Fixed<Int, 14> imu_registers = List<Int>{}; Fixed<Int, 25> sbus_frame = List<Int>{}; hwcontract_NavigationWireFrame navigation_frame = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}; Bool imu_present = false; Bool sbus_present = false; Bool navigation_present = false; };
  KERN_STRUCT(fc_FirmwareRuntime) { fc_StateRuntime runtime_ = fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}; fc_RC rc_snapshot_ = fc_RC{List<Int>{}, false, false, false, Int64{0x0}}; fc_NavigationState navigation_snapshot_ = fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}; Int64 navigation_us_ = Int64{0x0}; Int last_navigation_sequence_ = 0; Bool have_navigation_sequence_ = false; };

  // ---- variables ----
  KERN_CONST Int kRcTimeoutUs = 100000;
  KERN_CONST Int kNavigationTimeoutUs = 60000;

  // ---- functions ----
  void FirmwareRuntime_reset(fc_FirmwareRuntime& self) {
    Arondight45_StateControl::StateRuntime_reset(self.runtime_);
    self.rc_snapshot_ = struct_make<fc_RC>(List<Int>{}, false, false, false, Int64{0x0});
    self.navigation_snapshot_ = struct_make<fc_NavigationState>(fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false);
    self.navigation_us_ = Int64{0x0};
    self.last_navigation_sequence_ = 0;
    self.have_navigation_sequence_ = false;
  }

  struct FirmwareRuntime_newResult { fc_FirmwareRuntime Result = fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}, fc_RC{List<Int>{}, false, false, false, Int64{0x0}}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}, Int64{0x0}, 0, false}; };
  FirmwareRuntime_newResult FirmwareRuntime_new() {
    fc_FirmwareRuntime self = fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}, fc_RC{List<Int>{}, false, false, false, Int64{0x0}}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}, Int64{0x0}, 0, false};
    n10 = Arondight45_StateControl::StateRuntime_new();
    self.runtime_ = n10.Result;
    FirmwareRuntime_reset(self);
    return {self};
  }

  struct FirmwareRuntime_stepResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<Int>{1000, 1000, 1000, 1000}, List<Int>{}, 0, fc_FaultCode::kFaultNone, false}; };
  FirmwareRuntime_stepResult FirmwareRuntime_step(fc_FirmwareRuntime& self, const fc_HardwareFrame& hardware) {
    fc_RC decoded = fc_RC{List<Int>{}, false, false, false, Int64{0x0}};
    fc_NavigationState decoded_2 = fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false};
    Int seq = 0;
    Bool newer = false;
    fc_Imu imu = fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}};
    Bool imu_valid = false;
    Bool both = false;
    fc_RC rc = fc_RC{List<Int>{}, false, false, false, Int64{0x0}};
    Bool rc_fresh = false;
    fc_NavigationState navigation = fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false};
    fc_StateRuntimeInput input = fc_StateRuntimeInput{fc_RuntimeInput{fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}, fc_RC{List<Int>{}, false, false, false, Int64{0x0}}, Int64{0x0}, 1000, 0, true, false}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}};
    if (hardware.sbus_present) {
      decoded = struct_make<fc_RC>(List<Int>{}, false, false, false, Int64{0x0});
      n11 = Arondight45_DroneFC_Core::decode_sbus(hardware.sbus_frame, decoded, 0);
      if (n11.Result) {
        decoded.us = hardware.now_us;
        self.rc_snapshot_ = decoded;
      }
    }
    if (hardware.navigation_present) {
      decoded_2 = struct_make<fc_NavigationState>(fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false);
      n12 = Arondight45_HardwareSensors::decode_navigation_wire(hardware.navigation_frame, decoded_2);
      if (n12.Result) {
        seq = hardware.navigation_frame.sequence;
        newer = logic_or(logic_not(self.have_navigation_sequence_), math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float seq, Float last_navigation_sequence_) -> Float { return kern::math::band((seq - last_navigation_sequence_), 65535.0); }, kern::to<Float>(seq), kern::to<Float>(self.last_navigation_sequence_))))), 32768.0));
        if (logic_and(newer, logic_or(logic_not(self.have_navigation_sequence_), math_compare<"!=">(kern::to<Float>(seq), kern::to<Float>(self.last_navigation_sequence_))))) {
          self.navigation_snapshot_ = decoded_2;
          self.navigation_us_ = hardware.now_us;
          self.last_navigation_sequence_ = seq;
          self.have_navigation_sequence_ = true;
        }
      }
    }
    imu = struct_make<fc_Imu>(fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0});
    both = hardware.imu_present;
    if (both) {
      n14 = Arondight45_HardwareSensors::decode_icm42688_registers(hardware.imu_registers, imu, 0);
      both = n14.Result;
    }
    imu_valid = both;
    rc = self.rc_snapshot_;
    rc_fresh = logic_and(logic_and(rc.valid, logic_not(int64_less_unsigned(hardware.now_us, rc.us))), logic_not(int64_less_unsigned(int64_of_int(kRcTimeoutUs), int64_sub(hardware.now_us, rc.us))));
    navigation = self.navigation_snapshot_;
    if (logic_or(logic_or(logic_not(self.have_navigation_sequence_), int64_less_unsigned(hardware.now_us, self.navigation_us_)), int64_less_unsigned(int64_of_int(kNavigationTimeoutUs), int64_sub(hardware.now_us, self.navigation_us_)))) {
      navigation.valid = false;
      navigation.velocity_valid = false;
      navigation.agl_valid = false;
    }
    input = struct_make<fc_StateRuntimeInput>(fc_RuntimeInput{fc_Imu{fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}}, fc_RC{List<Int>{}, false, false, false, Int64{0x0}}, Int64{0x0}, 1000, 0, true, false}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false});
    input.flight.raw = imu;
    input.flight.rc = rc;
    input.flight.now_us = hardware.now_us;
    input.flight.dt_us = hardware.dt_us;
    input.flight.missed_samples = hardware.missed_samples;
    input.flight.imu_valid = imu_valid;
    input.flight.rc_fresh = rc_fresh;
    input.navigation = navigation;
    n15 = Arondight45_StateControl::StateRuntime_step(self.runtime_, input);
    return {n15.Result};
  }

  // ---- what nodes give and keep ----
  Arondight45_StateControl::StateRuntime_newResult n10;
  Arondight45_DroneFC_Core::decode_sbusResult n11;
  Arondight45_HardwareSensors::decode_navigation_wireResult n12;
  Arondight45_HardwareSensors::decode_icm42688_registersResult n14;
  Arondight45_StateControl::StateRuntime_stepResult n15;
};
