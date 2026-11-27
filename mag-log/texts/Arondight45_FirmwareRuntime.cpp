struct Arondight45_FirmwareRuntime : virtual kern::Library, virtual Arondight45_DroneFC_Core, virtual Arondight45_HardwareSensors, virtual Arondight45_StateControl {
  // ---- types ----
  KERN_STRUCT(fc_HardwareFrame) { u64 now_us = 0; u32 dt_us = 1000; u32 missed_samples = 0; Fixed<u8, 14> imu_registers = List<u8>{}; Fixed<u8, 25> sbus_frame = List<u8>{}; hwcontract_NavigationWireFrame navigation_frame = hwcontract_NavigationWireFrame{827736398, 1, 0, 0, 0, 0, 0, 0, 0}; bool imu_present = false; bool sbus_present = false; bool navigation_present = false; };
  KERN_STRUCT(fc_FirmwareRuntime) { fc_StateRuntime runtime_ = fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}; fc_RC rc_snapshot_ = fc_RC{List<u16>{}, false, false, false, 0}; fc_NavigationState navigation_snapshot_ = fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}; u64 navigation_us_ = 0; u16 last_navigation_sequence_ = 0; bool have_navigation_sequence_ = false; };

  // ---- variables ----
  KERN_CONST u32 kRcTimeoutUs = 100000;
  KERN_CONST u32 kNavigationTimeoutUs = 60000;

  // ---- functions ----
  void FirmwareRuntime_reset(fc_FirmwareRuntime& self) {
    Arondight45_StateControl::StateRuntime_reset(self.runtime_);
    self.rc_snapshot_ = struct_make<fc_RC>(List<u16>{}, false, false, false, 0);
    self.navigation_snapshot_ = struct_make<fc_NavigationState>(fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false);
    self.navigation_us_ = 0;
    self.last_navigation_sequence_ = 0;
    self.have_navigation_sequence_ = false;
  }

  struct FirmwareRuntime_newResult { fc_FirmwareRuntime Result = fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}, fc_RC{List<u16>{}, false, false, false, 0}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}, 0, 0, false}; };
  FirmwareRuntime_newResult FirmwareRuntime_new() {
    fc_FirmwareRuntime self = fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}, fc_RC{List<u16>{}, false, false, false, 0}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}, 0, 0, false};
    n10 = Arondight45_StateControl::StateRuntime_new();
    self.runtime_ = n10.Result;
    FirmwareRuntime_reset(self);
    return {self};
  }

  struct FirmwareRuntime_stepResult { fc_RuntimeOutput Result = fc_RuntimeOutput{List<u16>{1000, 1000, 1000, 1000}, List<i16>{}, 0, fc_FaultCode::kFaultNone, false}; };
  FirmwareRuntime_stepResult FirmwareRuntime_step(fc_FirmwareRuntime& self, const fc_HardwareFrame& hardware) {
    fc_RC decoded = fc_RC{List<u16>{}, false, false, false, 0};
    fc_NavigationState decoded_2 = fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false};
    u16 seq = 0;
    bool newer = false;
    fc_Imu imu = fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}};
    bool imu_valid = false;
    bool both = false;
    fc_RC rc = fc_RC{List<u16>{}, false, false, false, 0};
    bool rc_fresh = false;
    fc_NavigationState navigation = fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false};
    fc_StateRuntimeInput input = fc_StateRuntimeInput{fc_RuntimeInput{fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}, fc_RC{List<u16>{}, false, false, false, 0}, 0, 1000, 0, true, false}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}};
    if (hardware.sbus_present) {
      decoded = struct_make<fc_RC>(List<u16>{}, false, false, false, 0);
      n11 = Arondight45_DroneFC_Core::decode_sbus(decoded, hardware.sbus_frame, 0);
      if (n11.Result) {
        decoded.us = hardware.now_us;
        self.rc_snapshot_ = decoded;
      }
    }
    if (hardware.navigation_present) {
      decoded_2 = struct_make<fc_NavigationState>(fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false);
      n12 = Arondight45_HardwareSensors::decode_navigation_wire(hardware.navigation_frame, decoded_2);
      if (n12.Result) {
        seq = hardware.navigation_frame.sequence;
        newer = logic_or(logic_not(self.have_navigation_sequence_), math_compare<"<", u32>(kern::formula<u16, u16>([](auto seq, auto last_navigation_sequence_) { return num_cast<u32, u16>(num_cast<u16, i32>((num_cast<i32, u16>(seq) - num_cast<i32, u16>(last_navigation_sequence_)))); }, seq, self.last_navigation_sequence_), 32768));
        if (logic_and(newer, logic_or(logic_not(self.have_navigation_sequence_), math_compare<"!=", i32>(num_cast<i32, u16>(seq), num_cast<i32, u16>(self.last_navigation_sequence_))))) {
          self.navigation_snapshot_ = decoded_2;
          self.navigation_us_ = hardware.now_us;
          self.last_navigation_sequence_ = seq;
          self.have_navigation_sequence_ = true;
        }
      }
    }
    imu = struct_make<fc_Imu>(fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f});
    both = hardware.imu_present;
    if (both) {
      n14 = Arondight45_HardwareSensors::decode_icm42688_registers(imu, hardware.imu_registers, 0);
      both = n14.Result;
    }
    imu_valid = both;
    rc = self.rc_snapshot_;
    rc_fresh = logic_and(logic_and(rc.valid, math_compare<">=", u64>(hardware.now_us, rc.us)), math_compare<"<=", u64>(kern::formula<u64, u64>([](auto now_us, auto us) { return (now_us - us); }, hardware.now_us, rc.us), num_cast<u64, u32>(kRcTimeoutUs)));
    navigation = self.navigation_snapshot_;
    if (logic_or(logic_or(logic_not(self.have_navigation_sequence_), math_compare<"<", u64>(hardware.now_us, self.navigation_us_)), math_compare<">", u64>(kern::formula<u64, u64>([](auto now_us, auto navigation_us_) { return (now_us - navigation_us_); }, hardware.now_us, self.navigation_us_), num_cast<u64, u32>(kNavigationTimeoutUs)))) {
      navigation.valid = false;
      navigation.velocity_valid = false;
      navigation.agl_valid = false;
    }
    input = struct_make<fc_StateRuntimeInput>(fc_RuntimeInput{fc_Imu{fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}}, fc_RC{List<u16>{}, false, false, false, 0}, 0, 1000, 0, true, false}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false});
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
