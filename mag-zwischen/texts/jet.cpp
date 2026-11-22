struct jet : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(JetInput) { Float lift = 0.0; Float yaw = 0.0; Float pitch = 0.0; Float roll = 0.0; Float brake = 0.0; };
  KERN_STRUCT(JetSpec_Gear) { Vector p = Vector{0.0, 0.0, 0.0}; Float load = 0.0; Bool steer = false; };
  KERN_STRUCT(JetTelemetry) { Float V = 0.0; Float agl = 0.0; Float mach = 0.0; Float alpha = 0.0; Float beta = 0.0; Float nz = 1.0; Float thrust = 0.0; Float nozzle = 0.0; I32 contacts = 0; Float pitch = 0.0; Float roll = 0.0; Float heading = 0.0; Float w_flight = 0.0; };
  KERN_STRUCT(Jet_Aero) { Float qbar = 0.0; Float alpha = 0.0; Float beta = 0.0; Float CL = 0.0; Float CD = 0.0; Float CY = 0.0; Vector moment = Vector{0.0, 0.0, 0.0}; };
  KERN_ENUM(Jet_Mode, hover, flight);
  KERN_STRUCT(JetSpec) { Float mass = 12000.0; Vector inertia = Vector{100000.0, 18000.0, 115000.0}; Float wing_area = 26.0; Float span = 9.2; Float chord = 3.3; Float thrust_dry = 105000.0; Float thrust_max = 185000.0; Float spool_up = 0.34; Float spool_down = 0.3; Float CL0 = 0.1; Float CLa = 4.0; Float alpha_stall = 0.42; Float CD0 = 0.022; Float K = 0.14; Float CYb = -0.8; Float gear_CD = 0.02; Float brake_CD = 0.15; Float Cm0 = 0.0; Float Cma = -0.4; Float Cmq = -12.0; Float Cmde = -0.9; Float Clb = -0.06; Float Clp = -0.35; Float Clr = 0.08; Float Clda = 0.14; Float Cnb = 0.12; Float Cnr = -0.25; Float Cnp = -0.03; Float Cndr = 0.07; Float de_max = 0.48; Float da_max = 0.48; Float dr_max = 0.5; Float surface_rate = 7.5; Vector rcs_max = Vector{70000.0, 45000.0, 150000.0}; Float nozzle_rate = 1.0; Fixed<JetSpec_Gear, 3> gear = List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}; Float strut_rest = 1.6; Float gear_height = 2.05; Float strut_static = 0.15; Float strut_max = 0.4; Float strut_zeta = 0.6; Float tyre_mu = 0.8; Float roll_mu = 0.02; Float brake_mu = 0.5; Float g_limit = 9.0; Float g_push = 3.0; Float alpha_limit = 0.38; Float p_max = 3.4; Float q_max = 1.7; Float hover_speed = 22.0; Float hover_climb = 10.0; Float hover_sink = 6.0; Float hover_tilt = 0.35; Float wing_borne_v = 85.0; Float hover_law_v = 50.0; Float transition_nozzle = 1.05; Float braking_nozzle = 1.71; };
  KERN_STRUCT(Jet) { Bool engine_on = true; Bool pilot = true; Bool arcade = true; Float thrust_scale = 1.0; Float fuel = 1.0; JetSpec s_ = JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}; Jet_Mode mode_ = Jet_Mode::hover; Float lever_ = 0.0; Float thrust_ = 0.0; Float thrust_cmd_ = 0.0; Float nozzle_ = 1.5707963267948966; Float de_ = 0.0; Float da_ = 0.0; Float dr_ = 0.0; Float steer_ = 0.0; Float vz_i_ = 0.0; Float vz_cmd_ = 0.0; Bool vz_cmd_valid_ = false; Bool gamma_valid_ = false; Float gamma_hold_ = 0.0; Bool gear_down_ = true; Bool speed_brake_ = false; Bool landed_ = true; I32 contacts_ = 0; Float gear_impact_ = 0.0; JetTelemetry tele_ = JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}; };

  // ---- functions ----
  struct Jet_modeResult { Jet_Mode Result = Jet_Mode::hover; };
  KERN_PURE Jet_modeResult Jet_mode(const Jet& self) {
    return {self.mode_};
  }

  struct Jet_landedResult { Bool Result = false; };
  KERN_PURE Jet_landedResult Jet_landed(const Jet& self) {
    return {self.landed_};
  }

  struct Jet_gear_downResult { Bool Result = false; };
  KERN_PURE Jet_gear_downResult Jet_gear_down(const Jet& self) {
    return {self.gear_down_};
  }

  struct Jet_gear_impactResult { Float Result = 0.0; };
  KERN_PURE Jet_gear_impactResult Jet_gear_impact(const Jet& self) {
    return {self.gear_impact_};
  }

  struct Jet_nozzleResult { Float Result = 0.0; };
  KERN_PURE Jet_nozzleResult Jet_nozzle(const Jet& self) {
    return {self.nozzle_};
  }

  struct Jet_thrustResult { Float Result = 0.0; };
  KERN_PURE Jet_thrustResult Jet_thrust(const Jet& self) {
    return {self.thrust_};
  }

  struct Jet_leverResult { Float Result = 0.0; };
  KERN_PURE Jet_leverResult Jet_lever(const Jet& self) {
    return {self.lever_};
  }
};
