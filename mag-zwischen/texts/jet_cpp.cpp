struct jet_cpp : virtual kern::Library, virtual common, virtual engine_math_math, virtual jet, virtual rigid_spec {
  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Float kSound = 340.0;
  /// the jet model's own constant (jet_flight_dynamics.mjs)
  KERN_CONST Float G = 9.81;

  // ---- functions ----
  void Jet_reset(Jet& self) {
    self.mode_ = Jet_Mode::hover;
    self.thrust_cmd_ = 0.0;
    self.thrust_ = self.thrust_cmd_;
    self.lever_ = self.thrust_;
    self.nozzle_ = kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi);
    self.vz_cmd_ = 0.0;
    self.vz_i_ = self.vz_cmd_;
    self.steer_ = self.vz_i_;
    self.dr_ = self.steer_;
    self.da_ = self.dr_;
    self.de_ = self.da_;
    self.gamma_valid_ = false;
    self.vz_cmd_valid_ = self.gamma_valid_;
    self.gamma_hold_ = 0.0;
    self.gear_down_ = true;
    self.speed_brake_ = false;
    self.landed_ = true;
    self.contacts_ = 0;
    self.gear_impact_ = 0.0;
    self.engine_on = true;
    self.pilot = true;
    self.thrust_scale = 1.0;
    self.tele_ = struct_make<JetTelemetry>(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0);
  }

  struct Jet_newResult { Jet Result = Jet{true, true, true, 1.0, 1.0, JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}, Jet_Mode::hover, 0.0, 0.0, 0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, true, false, true, 0, 0.0, JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}}; };
  Jet_newResult Jet_new(const JetSpec& s) {
    Jet self = Jet{true, true, true, 1.0, 1.0, JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}, Jet_Mode::hover, 0.0, 0.0, 0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, true, false, true, 0, 0.0, JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}};
    self.s_ = s;
    Jet_reset(self);
    return {self};
  }

  void Jet_set_mode(Jet& self, Jet_Mode m = Jet_Mode::hover) {
    self.mode_ = m;
    if (enum_equal<Jet_Mode>(self.mode_, Jet_Mode::flight)) {
      self.lever_ = kern::formula([](Float lever_) -> Float { return kern::math::max(lever_, 0.9); }, self.lever_);
    }
  }

  void Jet_toggle_mode(Jet& self) {
    Jet_set_mode(self, logic_select<Jet_Mode>(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover), Jet_Mode::flight, Jet_Mode::hover));
  }

  struct wave_cdResult { Float Result = 0.0; };
  KERN_PURE wave_cdResult wave_cd(Float m = 0.0) {
    Float x = 0.0;
    x = kern::formula([](Float m) -> Float { return kern::math::div((m - 1.02), 0.16); }, m);
    return {kern::formula([](Float x, Float m) -> Float { return ((kern::math::exp((-(x * x))) * 0.012) + ((m > 1.02) ? 0.006 : 0.0)); }, x, m)};
  }

  struct nrmResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE nrmResult nrm(Vector a = Vector{0.0, 0.0, 0.0}) {
    Float l = 0.0;
    l = vector_length(a);
    return {logic_select<Vector>(math_compare<">">(l, 1e-09), vector_scale(a, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l)), Vector{0.0, 0.0, 0.0})};
  }

  struct air_densityResult { Float Result = 0.0; };
  KERN_PURE air_densityResult air_density(Float h = 0.0) {
    return {kern::formula([](Float h) -> Float { return (1.225 * kern::math::exp(kern::math::div((-kern::math::max(0.0, h)), 8500.0))); }, h)};
  }

  struct Jet_rigid_specResult { RigidSpec Result = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false}; };
  KERN_PURE Jet_rigid_specResult Jet_rigid_spec(const JetSpec& s) {
    RigidSpec r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false};
    ColliderSpec fuselage = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    ColliderSpec wing = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    r = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false};
    fuselage = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    fuselage.kind = physics_ShapeKind::hull;
    fuselage.points = list_make<Vector, "12">(Vector{-0.8, -7.2, -0.8}, Vector{0.8, -7.2, -0.8}, Vector{0.8, -7.2, 0.8}, Vector{-0.8, -7.2, 0.8}, Vector{-0.8, 5.4, -0.8}, Vector{0.8, 5.4, -0.8}, Vector{0.8, 5.4, 0.8}, Vector{-0.8, 5.4, 0.8}, Vector{-0.15, 9.0, -0.15}, Vector{0.15, 9.0, -0.15}, Vector{0.15, 9.0, 0.15}, Vector{-0.15, 9.0, 0.15});
    fuselage.friction = 0.5;
    wing = ColliderSpec{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 0.6, 0.0};
    wing.kind = physics_ShapeKind::hull;
    wing.points = list_make<Vector, "16">(Vector{-6.0, -3.2, -0.2}, Vector{-6.0, -4.2, -0.2}, Vector{-0.4, 1.6, -0.2}, Vector{-0.4, -4.0, -0.2}, Vector{6.0, -3.2, -0.2}, Vector{6.0, -4.2, -0.2}, Vector{0.4, 1.6, -0.2}, Vector{0.4, -4.0, -0.2}, Vector{-6.0, -3.2, 0.1}, Vector{-6.0, -4.2, 0.1}, Vector{-0.4, 1.6, 0.1}, Vector{-0.4, -4.0, 0.1}, Vector{6.0, -3.2, 0.1}, Vector{6.0, -4.2, 0.1}, Vector{0.4, 1.6, 0.1}, Vector{0.4, -4.0, 0.1});
    wing.friction = 0.5;
    r.colliders = list_make<ColliderSpec, "2">(fuselage, wing);
    r.mass.mass = s.mass;
    r.mass.center = Vector{0.0, 0.0, 0.0};
    r.mass.inertia[0] = vector_break(s.inertia).x;
    r.mass.inertia[1] = vector_break(s.inertia).y;
    r.mass.inertia[2] = vector_break(s.inertia).z;
    r.mass.inertia[5] = 0.0;
    r.mass.inertia[4] = r.mass.inertia[5];
    r.mass.inertia[3] = r.mass.inertia[4];
    r.bullet = true;
    return {r};
  }

  struct Jet_aeroResult { Jet_Aero Result = Jet_Aero{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}; };
  KERN_PURE Jet_aeroResult Jet_aero(const Jet& self, Vector vb = Vector{0.0, 0.0, 0.0}, Vector wb = Vector{0.0, 0.0, 0.0}, Float V = 0.0, Float rho = 0.0, Float de = 0.0, Float da = 0.0, Float dr = 0.0) {
    Jet_Aero a = Jet_Aero{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
    Float u = 0.0;
    Float vs = 0.0;
    Float wu = 0.0;
    Float am = 0.0;
    Float sig = 0.0;
    Float CLlin = 0.0;
    Float CLfp = 0.0;
    Float sa = 0.0;
    Float hv = 0.0;
    Float qh = 0.0;
    Float ph = 0.0;
    Float rh = 0.0;
    Float Cm = 0.0;
    Float Cl = 0.0;
    Float Cn = 0.0;
    a = struct_make<Jet_Aero>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0});
    a.qbar = kern::formula([](Float rho, Float v) -> Float { return (((0.5 * rho) * v) * v); }, rho, V);
    u = vector_break(vb).y;
    vs = vector_break(vb).x;
    wu = vector_break(vb).z;
    a.alpha = kern::formula([](Float wu, Float u) -> Float { return kern::math::atan2((-wu), u); }, wu, u);
    a.beta = kern::formula([](Float v, Float a) -> Float { return ((v > 0.001) ? kern::math::asin(a) : 0.0); }, V, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float vs, Float v) -> Float { return kern::math::div(vs, v); }, vs, V), -1.0, 1.0));
    am = kern::formula([](Float alpha) -> Float { return (alpha < (-1.0) ? (-1.0) : (1.0 < alpha ? 1.0 : alpha)); }, a.alpha);
    sig = kern::formula([](Float alpha, Float alpha_stall) -> Float { return kern::math::div(1.0, (1.0 + kern::math::exp(kern::math::div((-(kern::math::abs(alpha) - alpha_stall)), 0.04)))); }, a.alpha, self.s_.alpha_stall);
    CLlin = kern::formula([](Float cL0, Float cLa, Float am) -> Float { return (cL0 + (cLa * am)); }, self.s_.CL0, self.s_.CLa, am);
    CLfp = kern::formula([](Float alpha) -> Float { return kern::math::sin((2.0 * alpha)); }, a.alpha);
    a.CL = kern::formula([](Float sig, Float cLlin, Float cLfp) -> Float { return (((1.0 - sig) * cLlin) + (sig * cLfp)); }, sig, CLlin, CLfp);
    sa = kern::formula([](Float alpha) -> Float { return kern::math::sin(alpha); }, a.alpha);
    a.CD = kern::formula([](Float cD0, Float the_result, Float sig, Float k, Float cLlin, Float sa, Float gear_down_, Float gear_CD, Float speed_brake_, Float brake_CD) -> Float { return (((((cD0 + the_result) + ((((1.0 - sig) * k) * cLlin) * cLlin)) + (((sig * 2.0) * sa) * sa)) + ((gear_down_ != 0.0) ? gear_CD : 0.0)) + ((speed_brake_ != 0.0) ? brake_CD : 0.0)); }, self.s_.CD0, wave_cd(kern::formula([](Float v, Float kSound) -> Float { return kern::math::div(v, kSound); }, V, kSound)).Result, sig, self.s_.K, CLlin, sa, kern::to<Float>(self.gear_down_), self.s_.gear_CD, kern::to<Float>(self.speed_brake_), self.s_.brake_CD);
    a.CY = kern::formula([](Float cYb, Float beta) -> Float { return (cYb * beta); }, self.s_.CYb, a.beta);
    hv = kern::formula([](Float v) -> Float { return ((v > 1.0) ? kern::math::div(0.5, v) : 0.0); }, V);
    qh = kern::formula([](Float x, Float chord, Float hv) -> Float { return ((x * chord) * hv); }, vector_break(wb).x, self.s_.chord, hv);
    ph = kern::formula([](Float y, Float span, Float hv) -> Float { return ((y * span) * hv); }, vector_break(wb).y, self.s_.span, hv);
    rh = kern::formula([](Float z, Float span, Float hv) -> Float { return (((-z) * span) * hv); }, vector_break(wb).z, self.s_.span, hv);
    Cm = kern::formula([](Float cm0, Float cma, Float am, Float cmq, Float qh, Float cmde, Float de) -> Float { return (((cm0 + (cma * am)) + (cmq * qh)) + (cmde * de)); }, self.s_.Cm0, self.s_.Cma, am, self.s_.Cmq, qh, self.s_.Cmde, de);
    Cl = kern::formula([](Float clb, Float beta, Float clp, Float ph, Float clr, Float rh, Float clda, Float da) -> Float { return ((((clb * beta) + (clp * ph)) + (clr * rh)) + (clda * da)); }, self.s_.Clb, a.beta, self.s_.Clp, ph, self.s_.Clr, rh, self.s_.Clda, da);
    Cn = kern::formula([](Float cnb, Float beta, Float cnr, Float rh, Float cnp, Float ph, Float cndr, Float dr) -> Float { return ((((cnb * beta) + (cnr * rh)) + (cnp * ph)) + (cndr * dr)); }, self.s_.Cnb, a.beta, self.s_.Cnr, rh, self.s_.Cnp, ph, self.s_.Cndr, dr);
    a.moment = vector_make(kern::formula([](Float qbar, Float wing_area, Float chord, Float cm) -> Float { return (((qbar * wing_area) * chord) * cm); }, a.qbar, self.s_.wing_area, self.s_.chord, Cm), kern::formula([](Float qbar, Float wing_area, Float span, Float cl) -> Float { return (((qbar * wing_area) * span) * cl); }, a.qbar, self.s_.wing_area, self.s_.span, Cl), kern::formula([](Float qbar, Float wing_area, Float span, Float cn) -> Float { return ((((-qbar) * wing_area) * span) * cn); }, a.qbar, self.s_.wing_area, self.s_.span, Cn));
    return {a};
  }

  struct step_m_atResult { Float Result = 0.0; };
  KERN_PURE step_m_atResult step_m_at(const Quat& q, Vector rc = Vector{0.0, 0.0, 0.0}, Float m = 0.0, Vector I = Vector{0.0, 0.0, 0.0}, Vector d = Vector{0.0, 0.0, 0.0}) {
    Vector u = Vector{0.0, 0.0, 0.0};
    u = common::to_local(q, vector_cross(rc, d)).Result;
    return {kern::formula([](Float m, Float x, Float x_2, Float y, Float y_2, Float z, Float z_2) -> Float { return kern::math::div(1.0, (((kern::math::div(1.0, m) + kern::math::div((x * x), x_2)) + kern::math::div((y * y), y_2)) + kern::math::div((z * z), z_2))); }, m, vector_break(u).x, vector_break(I).x, vector_break(u).y, vector_break(I).y, vector_break(u).z, vector_break(I).z)};
  }

  void Jet_step(Jet& self, const JetInput& inp, Object<kern::physics::World> world = Object<kern::physics::World>{}, physics_Body body = physics_Body{Int64{0x0}}, Float dt = 0.0) {
    Object<kern::physics::World> world_ = Object<kern::physics::World>{};
    BodyState st = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float m = 0.0;
    Vector I = Vector{0.0, 0.0, 0.0};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Vector pos = Vector{0.0, 0.0, 0.0};
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector w = Vector{0.0, 0.0, 0.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector right = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Vector vb = Vector{0.0, 0.0, 0.0};
    Vector wb = Vector{0.0, 0.0, 0.0};
    Float V = 0.0;
    Float rho = 0.0;
    Float att_pitch = 0.0;
    Float att_roll = 0.0;
    Float att_heading = 0.0;
    Vector force = Vector{0.0, 0.0, 0.0};
    Vector torque = Vector{0.0, 0.0, 0.0};
    I32 contacts = 0;
    Float gear_impact = 0.0;
    Float steer_target = 0.0;
    Vector down = Vector{0.0, 0.0, 0.0};
    Fixed<the_RayResult_, 3> hits = Fixed<the_RayResult_, 3>{};
    I32 n_contact = 0;
    Vector attach = Vector{0.0, 0.0, 0.0};
    the_RayResult_ h = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Bool skip = false;
    the_RayResult_ hit = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Float comp = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector rc = Vector{0.0, 0.0, 0.0};
    Vector vp = Vector{0.0, 0.0, 0.0};
    Float comp_rate = 0.0;
    Float share = 0.0;
    Float k = 0.0;
    Float c = 0.0;
    Float stiff = 0.0;
    Float N = 0.0;
    Vector fw = Vector{0.0, 0.0, 0.0};
    Float cs = 0.0;
    Float sn = 0.0;
    Vector side = Vector{0.0, 0.0, 0.0};
    Vector sw = Vector{0.0, 0.0, 0.0};
    Float vf = 0.0;
    Float vsd = 0.0;
    Float share3 = 0.0;
    Float stop_l = 0.0;
    Float stop_s = 0.0;
    Float brake = 0.0;
    Float long_max = 0.0;
    Float lat_max = 0.0;
    Float Fl = 0.0;
    Float Fs = 0.0;
    Vector F = Vector{0.0, 0.0, 0.0};
    the_RayResult_ radar = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Float agl = 0.0;
    Bool pilot_ok = false;
    Float w_flight = 0.0;
    Bool on_ground = false;
    Float nozzle_target = 0.0;
    Float cn = 0.0;
    Float sn_2 = 0.0;
    Vector thrust_dir = Vector{0.0, 0.0, 0.0};
    Float lapse = 0.0;
    Float Tmax = 0.0;
    Float vz_want = 0.0;
    Float vz_cmd = 0.0;
    Float az_now = 0.0;
    Float vz_err = 0.0;
    Float tvert_raw = 0.0;
    Float tvert = 0.0;
    Float tlever = 0.0;
    Float support = 0.0;
    Float pC = 0.0;
    Float qC = 0.0;
    Float rC = 0.0;
    Float h_2 = 0.0;
    Vector fh = Vector{0.0, 0.0, 0.0};
    Vector rh = Vector{0.0, 0.0, 0.0};
    Float vF = 0.0;
    Float vR = 0.0;
    Float aF = 0.0;
    Float aR = 0.0;
    Float tilt = 0.0;
    Bool trc = false;
    Float pitch_cmd = 0.0;
    Float roll_cmd = 0.0;
    Float hq = 0.0;
    Float hp = 0.0;
    Float hr = 0.0;
    Float Vs = 0.0;
    Float gamma = 0.0;
    Float phi = 0.0;
    Float qf = 0.0;
    Float level = 0.0;
    Float a = 0.0;
    Float rf = 0.0;
    Float p = 0.0;
    Float qq = 0.0;
    Float r = 0.0;
    Float want_x = 0.0;
    Float want_y = 0.0;
    Float want_z = 0.0;
    Float beta = 0.0;
    Vector Iw = Vector{0.0, 0.0, 0.0};
    Vector gyro = Vector{0.0, 0.0, 0.0};
    Bool ctl = false;
    Jet_Aero base = Jet_Aero{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
    Float qS = 0.0;
    Float deT = 0.0;
    Float daT = 0.0;
    Float drT = 0.0;
    Vector rcs = Vector{0.0, 0.0, 0.0};
    Float avail = 0.0;
    Vector left = Vector{0.0, 0.0, 0.0};
    Jet_Aero A = Jet_Aero{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
    Bool has_aero = false;
    Vector vh = Vector{0.0, 0.0, 0.0};
    Float qS2 = 0.0;
    Vector lift_dir = Vector{0.0, 0.0, 0.0};
    Vector side_dir = Vector{0.0, 0.0, 0.0};
    Float k_2 = 0.0;
    Float Vs_2 = 0.0;
    Float turn = 0.0;
    Vector want = Vector{0.0, 0.0, 0.0};
    Float yaw_rate = 0.0;
    Vector vd = Vector{0.0, 0.0, 0.0};
    Float kv = 0.0;
    Vector nd = Vector{0.0, 0.0, 0.0};
    world_ = world;
    st = common::read_body(world_, body).Result;
    m = self.s_.mass;
    I = self.s_.inertia;
    q = st.rotation;
    pos = st.position;
    v = st.velocity;
    w = st.angular;
    fwd = engine_math_math::Quat_rotate(q, Vector{0.0, 1.0, 0.0}).Result;
    right = engine_math_math::Quat_rotate(q, Vector{1.0, 0.0, 0.0}).Result;
    up = engine_math_math::Quat_rotate(q, Vector{0.0, 0.0, 1.0}).Result;
    vb = common::to_local(q, v).Result;
    wb = common::to_local(q, w).Result;
    V = vector_length(v);
    rho = air_density(vector_break(pos).z).Result;
    att_pitch = kern::formula([](Float z) -> Float { return kern::math::asin((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(fwd).z);
    att_roll = kern::formula([](Float z, Float z_2) -> Float { return kern::math::atan2((-z), z_2); }, vector_break(right).z, vector_break(up).z);
    att_heading = kern::formula([](Float x, Float y) -> Float { return kern::math::atan2((-x), y); }, vector_break(fwd).x, vector_break(fwd).y);
    force = Vector{0.0, 0.0, 0.0};
    torque = Vector{0.0, 0.0, 0.0};
    self.gear_down_ = logic_or(logic_or(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover), math_compare<"<">(V, self.s_.wing_borne_v)), math_compare<">">(kern::to<Float>(self.contacts_), 0.0));
    contacts = 0;
    gear_impact = 0.0;
    steer_target = kern::formula([](Float v, Float yaw) -> Float { return ((v < 30.0) ? ((yaw < (-1.0) ? (-1.0) : (1.0 < yaw ? 1.0 : yaw)) * 0.5) : ((yaw < (-1.0) ? (-1.0) : (1.0 < yaw ? 1.0 : yaw)) * 0.08)); }, V, inp.yaw);
    self.steer_ = common::rate_limit(self.steer_, steer_target, kern::formula([](Float dt) -> Float { return (dt * 1.5); }, dt)).Result;
    if (self.gear_down_) {
      down = vector_scale(up, -1.0);
      hits = List<the_RayResult_>{};
      n_contact = 0;
      for (auto it10 = flow_for(n10, 0, 2); it10.next();) {
        attach = vector_add(pos, engine_math_math::Quat_rotate(q, self.s_.gear[n10.index].p).Result);
        h = common::cast(world_, attach, down, kern::formula([](Float strut_rest) -> Float { return (strut_rest + 0.4); }, self.s_.strut_rest), body, Int64{0xffffffffffffffff}).Result;
        if (logic_and(h.hit, math_compare<"<">(h.distance, self.s_.strut_rest))) {
          hits[n10.index] = h;
          n_contact += 1;
        }
      }
      for (auto it11 = flow_for(n11, 0, 2); it11.next();) {
        skip = false;
        if (logic_not(hits[n11.index].hit)) {
          skip = true;
        }
        if (logic_not(skip)) {
          hit = hits[n11.index];
          comp = kern::formula([](Float strut_rest, Float distance) -> Float { return (strut_rest - distance); }, self.s_.strut_rest, hit.distance);
          n = nrm(hit.normal).Result;
          rc = vector_sub(hit.point, pos);
          vp = vector_add(vector_cross(w, rc), v);
          comp_rate = vector_dot(vp, down);
          share = kern::formula([](Float a, Float m) -> Float { return (a * m); }, self.s_.gear[n11.index].load, m);
          k = kern::formula([](Float share, Float g, Float strut_static) -> Float { return kern::math::div((share * g), strut_static); }, share, G, self.s_.strut_static);
          c = kern::formula([](Float strut_zeta, Float k, Float share) -> Float { return ((2.0 * strut_zeta) * kern::math::sqrt((k * share))); }, self.s_.strut_zeta, k, share);
          stiff = kern::formula([](Float comp, Float strut_max, Float k) -> Float { return ((comp > strut_max) ? ((k * 12.0) * (comp - strut_max)) : 0.0); }, comp, self.s_.strut_max, k);
          if (math_compare<">">(comp_rate, gear_impact)) {
            gear_impact = comp_rate;
          }
          N = kern::formula([](Float k, Float comp, Float stiff, Float c, Float comp_rate) -> Float { return kern::math::max(0.0, (((k * comp) + stiff) + (c * comp_rate))); }, k, comp, stiff, c, comp_rate);
          contacts += 1;
          fw = nrm(vector_sub(fwd, vector_scale(n, vector_dot(fwd, n)))).Result;
          if (logic_and(self.s_.gear[n11.index].steer, math_compare<">">(kern::formula([](Float steer_) -> Float { return (steer_ != 0.0); }, self.steer_), 0.5))) {
            cs = kern::formula([](Float steer_) -> Float { return kern::math::cos(steer_); }, self.steer_);
            sn = kern::formula([](Float steer_) -> Float { return kern::math::sin(steer_); }, self.steer_);
            side = vector_cross(fw, n);
            fw = nrm(vector_add(vector_scale(fw, cs), vector_scale(side, sn))).Result;
          }
          sw = vector_cross(fw, n);
          vf = vector_dot(vp, fw);
          vsd = vector_dot(vp, sw);
          share3 = kern::formula([](Float n_contact) -> Float { return kern::math::div(0.8, kern::math::max(1.0, n_contact)); }, kern::to<Float>(n_contact));
          stop_l = kern::formula([](Float the_result, Float share3, Float dt) -> Float { return kern::math::div((the_result * share3), kern::math::max(0.001, dt)); }, step_m_at(q, rc, m, I, fw).Result, share3, dt);
          stop_s = kern::formula([](Float the_result, Float share3, Float dt) -> Float { return kern::math::div((the_result * share3), kern::math::max(0.001, dt)); }, step_m_at(q, rc, m, I, sw).Result, share3, dt);
          brake = kern::formula([](Float brake) -> Float { return (brake < 0.0 ? 0.0 : (1.0 < brake ? 1.0 : brake)); }, inp.brake);
          long_max = kern::formula([](Float roll_mu, Float brake, Float brake_mu, Float n) -> Float { return ((roll_mu + (brake * brake_mu)) * n); }, self.s_.roll_mu, brake, self.s_.brake_mu, N);
          lat_max = kern::formula([](Float tyre_mu, Float n) -> Float { return (tyre_mu * n); }, self.s_.tyre_mu, N);
          Fl = kern::formula([](Float vf, Float long_max, Float stop_l) -> Float { return ((-kern::math::sign(vf)) * kern::math::min(long_max, (kern::math::abs(vf) * stop_l))); }, vf, long_max, stop_l);
          Fs = kern::formula([](Float vsd, Float lat_max, Float stop_s) -> Float { return ((-kern::math::sign(vsd)) * kern::math::min(lat_max, (kern::math::abs(vsd) * stop_s))); }, vsd, lat_max, stop_s);
          F = vector_add(vector_add(vector_scale(n, N), vector_scale(fw, Fl)), vector_scale(sw, Fs));
          force = vector_add(force, F);
          torque = vector_add(torque, vector_cross(rc, F));
        }
      }
    }
    self.gear_impact_ = kern::formula([](Float contacts, Float contacts_, Float gear_impact) -> Float { return (((contacts > 0.0) && (contacts_ == 0.0)) ? gear_impact : 0.0); }, kern::to<Float>(contacts), kern::to<Float>(self.contacts_), gear_impact);
    self.contacts_ = contacts;
    radar = common::cast(world_, pos, Vector{0.0, 0.0, -1.0}, 400.0, body, Int64{0xffffffffffffffff}).Result;
    agl = kern::formula([](Float hit, Float distance, Float gear_height) -> Float { return ((hit != 0.0) ? kern::math::max(0.0, (distance - gear_height)) : 400.0); }, kern::to<Float>(radar.hit), radar.distance, self.s_.gear_height);
    pilot_ok = logic_and(self.pilot, self.engine_on);
    w_flight = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v, Float hover_law_v, Float wing_borne_v) -> Float { return kern::math::div((v - hover_law_v), (wing_borne_v - hover_law_v)); }, V, self.s_.hover_law_v, self.s_.wing_borne_v), 0.0, 1.0));
    on_ground = math_compare<">=">(kern::to<Float>(contacts), 2.0);
    nozzle_target = kern::formula([](Float engine_on, Float nozzle_, Float a, Float v, Float hover_speed, Float braking_nozzle, Float kPi, Float transition_nozzle, Float a_2) -> Float { return ((!(engine_on != 0.0)) ? nozzle_ : ((a != 0.0) ? ((v > (hover_speed + 5.0)) ? braking_nozzle : kern::math::div(kPi, 2.0)) : (transition_nozzle * a_2))); }, kern::to<Float>(self.engine_on), self.nozzle_, kern::to<Float>(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover)), V, self.s_.hover_speed, self.s_.braking_nozzle, kPi, self.s_.transition_nozzle, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v, Float wing_borne_v) -> Float { return (1.0 - kern::math::div(v, (wing_borne_v - 5.0))); }, V, self.s_.wing_borne_v), 0.0, 1.0));
    self.nozzle_ = common::rate_limit(self.nozzle_, nozzle_target, kern::formula([](Float nozzle_rate, Float dt) -> Float { return (nozzle_rate * dt); }, self.s_.nozzle_rate, dt)).Result;
    cn = kern::formula([](Float nozzle_) -> Float { return kern::math::cos(nozzle_); }, self.nozzle_);
    sn_2 = kern::formula([](Float nozzle_) -> Float { return kern::math::sin(nozzle_); }, self.nozzle_);
    thrust_dir = vector_add(vector_scale(fwd, cn), vector_scale(up, sn_2));
    lapse = kern::formula([](Float rho, Float thrust_scale) -> Float { return (kern::math::pow(kern::math::div(rho, 1.225), 0.7) * (thrust_scale < 0.0 ? 0.0 : (1.0 < thrust_scale ? 1.0 : thrust_scale))); }, rho, self.thrust_scale);
    Tmax = kern::formula([](Float thrust_max, Float lapse) -> Float { return (thrust_max * lapse); }, self.s_.thrust_max, lapse);
    if (logic_and(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::flight), pilot_ok)) {
      self.lever_ = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float lever_, Float lift, Float dt) -> Float { return (lever_ + ((lift * dt) * 0.55)); }, self.lever_, inp.lift, dt), 0.0, 1.0));
    }
    self.speed_brake_ = logic_or(logic_and(logic_and(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::flight), math_compare<"<=">(self.lever_, 0.001)), math_compare<"<">(inp.lift, -0.4)), logic_and(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover), math_compare<">">(V, 30.0)));
    vz_want = kern::formula([](Float a, Float lift, Float hover_climb, Float hover_sink, Float agl) -> Float { return ((a != 0.0) ? 0.0 : ((lift > 0.0) ? (lift * hover_climb) : kern::math::max((lift * hover_sink), (-kern::math::min((kern::math::sqrt((3.0 * agl)) + 0.6), ((0.6 * agl) + 0.8)))))); }, kern::to<Float>(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::flight)), inp.lift, self.s_.hover_climb, self.s_.hover_sink, agl);
    self.vz_cmd_ = common::rate_limit(kern::formula([](Float vz_cmd_valid_, Float vz_cmd_, Float z) -> Float { return ((vz_cmd_valid_ != 0.0) ? vz_cmd_ : z); }, kern::to<Float>(self.vz_cmd_valid_), self.vz_cmd_, vector_break(v).z), vz_want, kern::formula([](Float dt) -> Float { return (11.0 * dt); }, dt)).Result;
    self.vz_cmd_valid_ = true;
    vz_cmd = self.vz_cmd_;
    az_now = kern::formula([](Float thrust_, Float z, Float m, Float g) -> Float { return (kern::math::div((thrust_ * z), m) - g); }, self.thrust_, vector_break(thrust_dir).z, m, G);
    vz_err = kern::formula([](Float vz_cmd, Float z, Float spool_up, Float az_now) -> Float { return (vz_cmd - (z + ((0.8 * spool_up) * az_now))); }, vz_cmd, vector_break(v).z, self.s_.spool_up, az_now);
    tvert_raw = kern::formula([](Float m, Float g, Float vz_err, Float vz_i_, Float z) -> Float { return kern::math::div((m * ((g + (2.4 * vz_err)) + vz_i_)), kern::math::max(0.25, z)); }, m, G, vz_err, self.vz_i_, vector_break(thrust_dir).z);
    if (logic_and(logic_and(logic_not(on_ground), math_compare<">">(tvert_raw, 0.0)), math_compare<"<">(tvert_raw, Tmax))) {
      self.vz_i_ = 0.0;
    }
    if (logic_and(on_ground, math_compare<"<=">(inp.lift, 0.05))) {
      self.vz_i_ = 0.0;
      self.vz_cmd_ = 0.0;
    }
    tvert = kern::formula([](Float on_ground, Float lift, Float tvert_raw, Float tmax) -> Float { return (((on_ground != 0.0) && (lift <= 0.05)) ? 0.0 : (tvert_raw < 0.0 ? 0.0 : (tmax < tvert_raw ? tmax : tvert_raw))); }, kern::to<Float>(on_ground), inp.lift, tvert_raw, Tmax);
    tlever = kern::formula([](Float lever_, Float thrust_dry, Float thrust_max, Float lapse) -> Float { return (((lever_ > 0.86) ? (thrust_dry + kern::math::div(((thrust_max - thrust_dry) * (lever_ - 0.86)), 0.14)) : kern::math::div((thrust_dry * lever_), 0.86)) * lapse); }, self.lever_, self.s_.thrust_dry, self.s_.thrust_max, lapse);
    support = kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? a_2 : 1.0); }, kern::to<Float>(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::flight)), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float sn_2) -> Float { return (sn_2 * 2.5); }, sn_2), 0.0, 1.0));
    self.thrust_cmd_ = kern::formula([](Float engine_on, Float a) -> Float { return ((!(engine_on != 0.0)) ? 0.0 : a); }, kern::to<Float>(self.engine_on), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float tlever, Float support, Float tvert) -> Float { return ((a != 0.0) ? (tlever + (support * kern::math::max(0.0, (tvert - tlever)))) : tvert); }, kern::to<Float>(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::flight)), tlever, support, tvert), 0.0, Tmax));
    self.thrust_ = kern::formula([](Float thrust_, Float thrust_cmd_, Float dt, Float spool_up, Float spool_down) -> Float { return (thrust_ + ((thrust_cmd_ - thrust_) * (1.0 - kern::math::exp(kern::math::div((-dt), ((thrust_cmd_ > thrust_) ? spool_up : spool_down)))))); }, self.thrust_, self.thrust_cmd_, dt, self.s_.spool_up, self.s_.spool_down);
    force = vector_add(force, vector_scale(thrust_dir, self.thrust_));
    pC = 0.0;
    qC = 0.0;
    rC = 0.0;
    h_2 = att_heading;
    fh = vector_make(kern::formula([](Float h_2) -> Float { return (-kern::math::sin(h_2)); }, h_2), kern::formula([](Float h_2) -> Float { return kern::math::cos(h_2); }, h_2), 0.0);
    rh = vector_make(kern::formula([](Float h_2) -> Float { return kern::math::cos(h_2); }, h_2), kern::formula([](Float h_2) -> Float { return kern::math::sin(h_2); }, h_2), 0.0);
    vF = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(v).x, vector_break(fh).x, vector_break(v).y, vector_break(fh).y);
    vR = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(v).x, vector_break(rh).x, vector_break(v).y, vector_break(rh).y);
    aF = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float pitch, Float hover_speed, Float vF) -> Float { return (1.1 * (((-pitch) * hover_speed) - vF)); }, inp.pitch, self.s_.hover_speed, vF), -4.0, 4.0));
    aR = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float roll, Float hover_speed, Float vR) -> Float { return (1.1 * ((roll * hover_speed) - vR)); }, inp.roll, self.s_.hover_speed, vR), -4.0, 4.0));
    tilt = kern::formula([](Float hover_tilt, Float a, Float a_2) -> Float { return ((hover_tilt * a) * a_2); }, self.s_.hover_tilt, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float agl) -> Float { return (0.25 + kern::math::div(agl, 8.0)); }, agl), 0.0, 1.0), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v) -> Float { return kern::math::div((42.0 - v), 20.0); }, V), 0.12, 1.0));
    trc = enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover);
    pitch_cmd = kern::formula([](Float trc, Float a, Float pitch) -> Float { return ((trc != 0.0) ? a : (0.05 + (pitch * 0.3))); }, kern::to<Float>(trc), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float aF, Float g, Float pitch) -> Float { return ((-kern::math::atan(kern::math::div(aF, g))) + (pitch * 0.07)); }, aF, G, inp.pitch), kern::formula([](Float tilt) -> Float { return (-tilt); }, tilt), tilt), inp.pitch);
    roll_cmd = kern::formula([](Float trc, Float a, Float roll) -> Float { return ((trc != 0.0) ? a : (roll * 0.5)); }, kern::to<Float>(trc), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float aR, Float g, Float roll) -> Float { return (kern::math::atan(kern::math::div(aR, g)) + (roll * 0.33)); }, aR, G, inp.roll), kern::formula([](Float tilt) -> Float { return (-tilt); }, tilt), tilt), inp.roll);
    hq = kern::formula([](Float pitch_cmd, Float att_pitch) -> Float { return (2.6 * (pitch_cmd - att_pitch)); }, pitch_cmd, att_pitch);
    hp = kern::formula([](Float roll_cmd, Float att_roll) -> Float { return (2.6 * (roll_cmd - att_roll)); }, roll_cmd, att_roll);
    hr = kern::formula([](Float yaw) -> Float { return (yaw * 0.8); }, inp.yaw);
    pC += kern::formula([](Float w_flight, Float hp) -> Float { return ((1.0 - w_flight) * hp); }, w_flight, hp);
    qC += kern::formula([](Float w_flight, Float hq) -> Float { return ((1.0 - w_flight) * hq); }, w_flight, hq);
    rC += kern::formula([](Float w_flight, Float hr) -> Float { return ((1.0 - w_flight) * hr); }, w_flight, hr);
    if (math_compare<"<">(w_flight, 0.05)) {
      self.gamma_valid_ = false;
    }
    Vs = kern::formula([](Float v) -> Float { return kern::math::max(v, 30.0); }, V);
    gamma = kern::formula([](Float v, Float a) -> Float { return ((v > 1.0) ? kern::math::asin(a) : 0.0); }, V, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float z, Float v) -> Float { return kern::math::div(z, v); }, vector_break(v).z, V), -1.0, 1.0));
    phi = att_roll;
    qf = 0.0;
    if (math_compare<">">(kern::formula([](Float pitch) -> Float { return kern::math::abs(pitch); }, inp.pitch), 0.05)) {
      self.gamma_valid_ = false;
      qf = kern::formula([](Float pitch, Float q_max, Float g_limit, Float g, Float vs, Float g_push) -> Float { return ((pitch > 0.0) ? (pitch * kern::math::min(q_max, kern::math::div((g_limit * g), vs))) : (pitch * kern::math::min(0.8, kern::math::div((g_push * g), vs)))); }, inp.pitch, self.s_.q_max, self.s_.g_limit, G, Vs, self.s_.g_push);
    } else {
      if (enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover)) {
        self.gamma_hold_ = kern::formula([](Float a) -> Float { return kern::math::asin(a); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float vz_cmd, Float vs) -> Float { return kern::math::div(vz_cmd, vs); }, vz_cmd, Vs), -0.5, 0.5));
        self.gamma_valid_ = true;
      } else if (logic_or(logic_not(self.gamma_valid_), math_compare<"<">(w_flight, 0.99))) {
        self.gamma_hold_ = kern::formula([](Float w_flight, Float gamma) -> Float { return ((w_flight < 0.99) ? 0.0 : gamma); }, w_flight, gamma);
        self.gamma_valid_ = true;
      }
      level = kern::formula([](Float phi, Float g, Float vs) -> Float { return ((kern::math::abs(phi) < 1.4) ? kern::math::div(((g * kern::math::sin(phi)) * kern::math::tan((phi < (-1.3) ? (-1.3) : (1.3 < phi ? 1.3 : phi)))), vs) : 0.0); }, phi, G, Vs);
      qf = kern::formula([](Float phi, Float level, Float a) -> Float { return ((kern::math::abs(phi) < 1.4) ? (level + (a * kern::math::cos(phi))) : 0.0); }, phi, level, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float gamma_hold_, Float gamma) -> Float { return (1.2 * (gamma_hold_ - gamma)); }, self.gamma_hold_, gamma), -0.15, 0.15));
    }
    a = kern::formula([](Float z, Float y) -> Float { return kern::math::atan2((-z), kern::math::max(0.5, y)); }, vector_break(vb).z, vector_break(vb).y);
    if (math_compare<">">(a, self.s_.alpha_limit)) {
      qf = kern::formula([](Float qf, Float alpha_limit, Float a) -> Float { return kern::math::min(qf, (2.5 * (alpha_limit - a))); }, qf, self.s_.alpha_limit, a);
    }
    if (math_compare<"<">(a, -0.2)) {
      qf = kern::formula([](Float qf, Float a) -> Float { return kern::math::max(qf, (2.5 * ((-0.2) - a))); }, qf, a);
    }
    rf = kern::formula([](Float g, Float phi, Float att_pitch, Float vs, Float yaw) -> Float { return (kern::math::div(((g * kern::math::sin(phi)) * kern::math::cos(att_pitch)), vs) + (yaw * 0.35)); }, G, phi, att_pitch, Vs, inp.yaw);
    pC += kern::formula([](Float w_flight, Float roll, Float p_max) -> Float { return ((w_flight * roll) * p_max); }, w_flight, inp.roll, self.s_.p_max);
    qC += kern::formula([](Float w_flight, Float qf) -> Float { return (w_flight * qf); }, w_flight, qf);
    rC += kern::formula([](Float w_flight, Float rf) -> Float { return (w_flight * rf); }, w_flight, rf);
    p = vector_break(wb).y;
    qq = vector_break(wb).x;
    r = kern::formula([](Float z) -> Float { return (-z); }, vector_break(wb).z);
    want_x = kern::formula([](Float x, Float qC, Float qq) -> Float { return ((x * 4.5) * (qC - qq)); }, vector_break(I).x, qC, qq);
    want_y = kern::formula([](Float y, Float pC, Float p) -> Float { return ((y * 35.0) * (pC - p)); }, vector_break(I).y, pC, p);
    want_z = kern::formula([](Float z, Float rC, Float r) -> Float { return (((-z) * 3.2) * (rC - r)); }, vector_break(I).z, rC, r);
    beta = kern::formula([](Float v, Float a) -> Float { return ((v > 1.0) ? kern::math::asin(a) : 0.0); }, V, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float v) -> Float { return kern::math::div(x, v); }, vector_break(vb).x, V), -1.0, 1.0));
    want_z += kern::formula([](Float z, Float w_flight, Float beta, Float v) -> Float { return (((((-z) * w_flight) * 1.8) * beta) * kern::math::min(1.0, kern::math::div(v, 60.0))); }, vector_break(I).z, w_flight, beta, V);
    Iw = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x * x_2); }, vector_break(I).x, vector_break(wb).x), kern::formula([](Float y, Float y_2) -> Float { return (y * y_2); }, vector_break(I).y, vector_break(wb).y), kern::formula([](Float z, Float z_2) -> Float { return (z * z_2); }, vector_break(I).z, vector_break(wb).z));
    gyro = vector_cross(wb, Iw);
    want_x += vector_break(gyro).x;
    want_y += vector_break(gyro).y;
    want_z += vector_break(gyro).z;
    ctl = logic_and(pilot_ok, logic_not(logic_and(on_ground, logic_or(enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover), math_compare<"<">(V, 40.0)))));
    base = Jet_aero(self, vb, wb, kern::formula([](Float v) -> Float { return kern::math::max(v, 0.001); }, V), rho, 0.0, 0.0, 0.0).Result;
    qS = kern::formula([](Float qbar, Float wing_area) -> Float { return (qbar * wing_area); }, base.qbar, self.s_.wing_area);
    deT = 0.0;
    daT = 0.0;
    drT = 0.0;
    rcs = Vector{0.0, 0.0, 0.0};
    if (ctl) {
      if (math_compare<">">(qS, 1.0)) {
        deT = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float want_x, Float x, Float qS, Float chord, Float cmde) -> Float { return kern::math::div((want_x - x), ((qS * chord) * cmde)); }, want_x, vector_break(base.moment).x, qS, self.s_.chord, self.s_.Cmde), kern::formula([](Float de_max) -> Float { return (-de_max); }, self.s_.de_max), self.s_.de_max));
        daT = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float want_y, Float y, Float qS, Float span, Float clda) -> Float { return kern::math::div((want_y - y), ((qS * span) * clda)); }, want_y, vector_break(base.moment).y, qS, self.s_.span, self.s_.Clda), kern::formula([](Float da_max) -> Float { return (-da_max); }, self.s_.da_max), self.s_.da_max));
        drT = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float want_z, Float z, Float qS, Float span, Float cndr) -> Float { return kern::math::div((-(want_z - z)), ((qS * span) * cndr)); }, want_z, vector_break(base.moment).z, qS, self.s_.span, self.s_.Cndr), kern::formula([](Float dr_max) -> Float { return (-dr_max); }, self.s_.dr_max), self.s_.dr_max));
      }
      avail = kern::formula([](Float nozzle_, Float a) -> Float { return (kern::math::sin(nozzle_) * a); }, self.nozzle_, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float thrust_, Float thrust_max) -> Float { return kern::math::div(thrust_, (0.4 * thrust_max)); }, self.thrust_, self.s_.thrust_max), 0.0, 1.0));
      left = vector_make(kern::formula([](Float want_x, Float x, Float qS, Float chord, Float cmde, Float deT) -> Float { return ((want_x - x) - (((qS * chord) * cmde) * deT)); }, want_x, vector_break(base.moment).x, qS, self.s_.chord, self.s_.Cmde, deT), kern::formula([](Float want_y, Float y, Float qS, Float span, Float clda, Float daT) -> Float { return ((want_y - y) - (((qS * span) * clda) * daT)); }, want_y, vector_break(base.moment).y, qS, self.s_.span, self.s_.Clda, daT), kern::formula([](Float want_z, Float z, Float qS, Float span, Float cndr, Float drT) -> Float { return ((want_z - z) + (((qS * span) * cndr) * drT)); }, want_z, vector_break(base.moment).z, qS, self.s_.span, self.s_.Cndr, drT));
      rcs = vector_make(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, vector_break(left).x, kern::formula([](Float x, Float avail) -> Float { return ((-x) * avail); }, vector_break(self.s_.rcs_max).x, avail), kern::formula([](Float x, Float avail) -> Float { return (x * avail); }, vector_break(self.s_.rcs_max).x, avail))), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, vector_break(left).y, kern::formula([](Float y, Float avail) -> Float { return ((-y) * avail); }, vector_break(self.s_.rcs_max).y, avail), kern::formula([](Float y, Float avail) -> Float { return (y * avail); }, vector_break(self.s_.rcs_max).y, avail))), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, vector_break(left).z, kern::formula([](Float z, Float avail) -> Float { return ((-z) * avail); }, vector_break(self.s_.rcs_max).z, avail), kern::formula([](Float z, Float avail) -> Float { return (z * avail); }, vector_break(self.s_.rcs_max).z, avail))));
    }
    self.de_ = common::rate_limit(self.de_, deT, kern::formula([](Float surface_rate, Float dt) -> Float { return (surface_rate * dt); }, self.s_.surface_rate, dt)).Result;
    self.da_ = common::rate_limit(self.da_, daT, kern::formula([](Float surface_rate, Float dt) -> Float { return (surface_rate * dt); }, self.s_.surface_rate, dt)).Result;
    self.dr_ = common::rate_limit(self.dr_, drT, kern::formula([](Float surface_rate, Float dt) -> Float { return (surface_rate * dt); }, self.s_.surface_rate, dt)).Result;
    A = struct_make<Jet_Aero>(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0});
    has_aero = false;
    if (math_compare<">">(V, 0.5)) {
      A = Jet_aero(self, vb, wb, V, rho, self.de_, self.da_, self.dr_).Result;
      has_aero = true;
      vh = vector_scale(v, kern::formula([](Float v) -> Float { return kern::math::div(1.0, v); }, V));
      qS2 = kern::formula([](Float qbar, Float wing_area) -> Float { return (qbar * wing_area); }, A.qbar, self.s_.wing_area);
      lift_dir = nrm(vector_sub(up, vector_scale(vh, vector_dot(up, vh)))).Result;
      side_dir = nrm(vector_sub(right, vector_scale(vh, vector_dot(right, vh)))).Result;
      force = vector_add(force, vector_scale(lift_dir, kern::formula([](Float qS2, Float cL) -> Float { return (qS2 * cL); }, qS2, A.CL)));
      force = vector_add(force, vector_scale(vh, kern::formula([](Float qS2, Float cD) -> Float { return ((-qS2) * cD); }, qS2, A.CD)));
      force = vector_add(force, vector_scale(side_dir, kern::formula([](Float qS2, Float cY) -> Float { return (qS2 * cY); }, qS2, A.CY)));
      torque = vector_add(torque, engine_math_math::Quat_rotate(q, A.moment).Result);
    }
    torque = vector_add(torque, engine_math_math::Quat_rotate(q, rcs).Result);
    engine_world_apply_force_to_center(world_, body, force);
    engine_world_apply_torque(world_, body, torque);
    if (logic_and(logic_and(self.arcade, pilot_ok), logic_not(on_ground))) {
      k_2 = kern::formula([](Float dt) -> Float { return (1.0 - kern::math::exp(((-dt) * 9.0))); }, dt);
      Vs_2 = kern::formula([](Float v) -> Float { return kern::math::max(v, 30.0); }, V);
      turn = kern::formula([](Float w_flight, Float g, Float att_roll, Float att_pitch, Float vs_2) -> Float { return kern::math::div((((w_flight * g) * kern::math::sin(att_roll)) * kern::math::cos(att_pitch)), vs_2); }, w_flight, G, att_roll, att_pitch, Vs_2);
      want = wb;
      want = vector_make(kern::formula([](Float x, Float w_flight, Float pitch, Float x_2) -> Float { return (x + (w_flight * (((pitch < (-1.0) ? (-1.0) : (1.0 < pitch ? 1.0 : pitch)) * 1.6) - x_2))); }, vector_break(want).x, w_flight, inp.pitch, vector_break(wb).x), vector_break(want).y, vector_break(want).z);
      want = vector_make(vector_break(want).x, kern::formula([](Float y, Float w_flight, Float roll, Float y_2) -> Float { return (y + (w_flight * (((roll < (-1.0) ? (-1.0) : (1.0 < roll ? 1.0 : roll)) * 3.4) - y_2))); }, vector_break(want).y, w_flight, inp.roll, vector_break(wb).y), vector_break(want).z);
      yaw_rate = kern::formula([](Float w_flight, Float a, Float yaw, Float turn) -> Float { return ((((1.0 - w_flight) * a) * 1.5) + (w_flight * (((yaw < (-1.0) ? (-1.0) : (1.0 < yaw ? 1.0 : yaw)) * 0.7) + turn))); }, w_flight, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float yaw, Float roll) -> Float { return (yaw + (roll * 0.7)); }, inp.yaw, inp.roll), -1.0, 1.0), inp.yaw, turn);
      want = vector_make(vector_break(want).x, vector_break(want).y, kern::formula([](Float z, Float yaw_rate, Float z_2) -> Float { return (z + ((-yaw_rate) - z_2)); }, vector_break(want).z, yaw_rate, vector_break(wb).z));
      engine_world_set_angular_velocity(world_, body, engine_math_math::Quat_rotate(q, vector_add(wb, vector_scale(vector_sub(want, wb), k_2))).Result);
      if (logic_and(math_compare<">">(V, 20.0), math_compare<">">(w_flight, 0.0))) {
        vd = vector_scale(v, kern::formula([](Float v) -> Float { return kern::math::div(1.0, v); }, V));
        kv = kern::formula([](Float dt, Float w_flight) -> Float { return (1.0 - kern::math::exp((((-dt) * 3.2) * w_flight))); }, dt, w_flight);
        nd = nrm(vector_add(vd, vector_scale(vector_sub(fwd, vd), kv))).Result;
        if (math_compare<">">(vector_dot(nd, nd), 0.5)) {
          engine_world_set_linear_velocity(world_, body, vector_scale(nd, V));
        }
      }
    }
    self.landed_ = logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(contacts), 2.0), math_compare<"<">(V, 1.5)), enum_equal<Jet_Mode>(self.mode_, Jet_Mode::hover)), math_compare<"<=">(inp.lift, 0.15));
    self.tele_.V = V;
    self.tele_.agl = agl;
    self.tele_.mach = kern::formula([](Float v, Float kSound) -> Float { return kern::math::div(v, kSound); }, V, kSound);
    self.tele_.alpha = kern::formula([](Float has_aero, Float alpha) -> Float { return ((has_aero != 0.0) ? alpha : 0.0); }, kern::to<Float>(has_aero), A.alpha);
    self.tele_.beta = kern::formula([](Float has_aero, Float beta) -> Float { return ((has_aero != 0.0) ? beta : 0.0); }, kern::to<Float>(has_aero), A.beta);
    self.tele_.nz = kern::formula([](Float a, Float m, Float g) -> Float { return kern::math::div(a, (m * g)); }, vector_dot(force, up), m, G);
    self.tele_.thrust = self.thrust_;
    self.tele_.nozzle = self.nozzle_;
    self.tele_.contacts = contacts;
    self.tele_.pitch = att_pitch;
    self.tele_.roll = att_roll;
    self.tele_.heading = att_heading;
    self.tele_.w_flight = w_flight;
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};
