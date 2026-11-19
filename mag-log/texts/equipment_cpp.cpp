struct equipment_cpp : virtual kern::Library, virtual blueprint_call, virtual equipment {
  // ---- types ----
  KERN_ENUM(Genre, synthwave, house, lofi, chip, techno);
  KERN_STRUCT(Prog) { Genre genre = Genre::synthwave; Float bpm = 0.0; Int seed = 0; Int root = 0; Bool minor = false; };

  // ---- variables ----
  KERN_CONST Float kPathInterval = 0.5;
  KERN_CONST Int kMaxPath = 600;
  KERN_CONST Int kMaxFlights = 50;
  KERN_CONST Int kMaxEvents = 120;
  KERN_CONST Int kMenu = -1;
  KERN_CONST Fixed<MusicSynth_Station, 5> kStations = List<MusicSynth_Station>{MusicSynth_Station{"KERN FM", "Synthwave", 100.0, 11}, MusicSynth_Station{"BODENSEE BEATS", "House", 124.0, 23}, MusicSynth_Station{"LOFI LAGUNE", "Lo-Fi", 78.0, 37}, MusicSynth_Station{"8-BIT RADIO", "Chiptune", 144.0, 41}, MusicSynth_Station{"NACHTSCHICHT", "Techno", 132.0, 53}};
  /// i - VI - III - VII
  KERN_CONST Fixed<Int, 4> kMinorProg = List<Int>{0, 8, 3, 10};
  /// I - V - vi - IV
  KERN_CONST Fixed<Int, 4> kMajorProg = List<Int>{0, 7, 9, 5};
  KERN_CONST Fixed<Int, 7> kMinorScale = List<Int>{0, 2, 3, 5, 7, 8, 10};
  KERN_CONST Fixed<Int, 7> kMajorScale = List<Int>{0, 2, 4, 5, 7, 9, 11};

  // ---- functions ----
  struct Jetpack_IgniteResult { Jetpack_Pack pack = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0}; Bool lit = false; };
  /// (of the Blueprint of the actor Jetpack, called on it)
  KERN_EXTERN_AS("Ignite") Jetpack_IgniteResult Jetpack_Ignite(Jetpack_Pack pack_5 = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0});

  struct Jetpack_UpdateResult { Jetpack_Pack pack = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0}; Float thrust = 0.0; };
  /// (of the Blueprint of the actor Jetpack, called on it)
  KERN_EXTERN_AS("Update") Jetpack_UpdateResult Jetpack_Update(Jetpack_Pack pack_5 = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0}, Float dt = 0.0, Float now = 0.0, Bool airborne = false, Float vz = 0.0, Float height = 0.0, Bool hold = false);

  struct Jetpack_bindResult { Bool Result = false; };
  KERN_PURE Jetpack_bindResult Jetpack_bind() {
    return {func_has_blueprint(actor_find("Jetpack"))};
  }

  struct Jetpack_accResult { Float Result = 0.0; };
  KERN_PURE Jetpack_accResult Jetpack_acc() {
    return {func_get_on<"Acc", Float>(actor_find("Jetpack"))};
  }

  struct Jetpack_air_accelResult { Float Result = 0.0; };
  KERN_PURE Jetpack_air_accelResult Jetpack_air_accel() {
    return {func_get_on<"AirAccel", Float>(actor_find("Jetpack"))};
  }

  struct Jetpack_max_climbResult { Float Result = 0.0; };
  KERN_PURE Jetpack_max_climbResult Jetpack_max_climb() {
    return {func_get_on<"MaxClimb", Float>(actor_find("Jetpack"))};
  }

  struct Jetpack_gravityResult { Float Result = 0.0; };
  KERN_PURE Jetpack_gravityResult Jetpack_gravity() {
    return {func_get_on<"Gravity", Float>(actor_find("Jetpack"))};
  }

  struct Jetpack_fall_safeResult { Float Result = 0.0; };
  KERN_PURE Jetpack_fall_safeResult Jetpack_fall_safe() {
    return {func_get_on<"FallSafe", Float>(actor_find("Jetpack"))};
  }

  struct Jetpack_ignition_dampResult { Float Result = 0.0; };
  KERN_PURE Jetpack_ignition_dampResult Jetpack_ignition_damp() {
    return {func_get_on<"IgnitionDamp", Float>(actor_find("Jetpack"))};
  }

  struct Jetpack_packResult { Jetpack_Pack Result = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0}; };
  KERN_PURE Jetpack_packResult Jetpack_pack(const Jetpack& self) {
    return {struct_make<Jetpack_Pack>(blueprint_call::decimal(self.fuel).Result, self.thrusting, self.used, self.ignitions, blueprint_call::decimal(self.burst_left_).Result, blueprint_call::decimal(self.ground_since_).Result, blueprint_call::decimal(self.last_thrust_).Result)};
  }

  void Jetpack_take(Jetpack& self, const Jetpack_Pack& p) {
    self.fuel = p.fuel;
    self.thrusting = p.thrusting;
    self.used = p.used;
    self.ignitions = p.ignitions;
    self.burst_left_ = p.burst_left;
    self.ground_since_ = p.ground_since;
    self.last_thrust_ = p.last_thrust;
  }

  struct Jetpack_igniteResult { Bool Result = false; };
  Jetpack_igniteResult Jetpack_ignite(Jetpack& self, Float vz = 0.0) {
    Jetpack_Pack p = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0};
    Bool lit = false;
    p = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0};
    lit = false;
    func_call_on<"Jetpack_Ignite">(actor_find("Jetpack"), kern::out(n10.pack, n10.lit), Jetpack_pack(self).Result);
    p = n10.pack;
    lit = n10.lit;
    Jetpack_take(self, p);
    return {lit};
  }

  struct Jetpack_updateResult { Float Result = 0.0; };
  Jetpack_updateResult Jetpack_update(Jetpack& self, Float dt = 0.0, Float now = 0.0, Bool airborne = false, Float vz = 0.0, Float height = 0.0, Bool hold = false) {
    Jetpack_Pack p = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0};
    Float thrust = 0.0;
    p = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0};
    thrust = 0.0;
    func_call_on<"Jetpack_Update">(actor_find("Jetpack"), kern::out(n11.pack, n11.thrust), Jetpack_pack(self).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(now).Result, airborne, blueprint_call::decimal(vz).Result, blueprint_call::decimal(height).Result, hold);
    p = n11.pack;
    thrust = n11.thrust;
    Jetpack_take(self, p);
    return {thrust};
  }

  void FlightLog_record(FlightLog& self, const FlightSample& s, Bool force = false) {
    Float step = 0.0;
    if (self.have_last_) {
      step = vector_length(vector_sub(s.pos, self.last_));
      if (math_compare<"<">(step, 20.0)) {
        self.cur_.distance = kern::formula([](Float distance, Float step) -> Float { return (distance + step); }, self.cur_.distance, step);
      }
    }
    self.last_ = s.pos;
    self.have_last_ = true;
    self.last_t_ = s.t;
    self.cur_.duration = kern::formula([](Float t, Float start) -> Float { return (t - start); }, s.t, self.cur_.start);
    self.cur_.max_speed = kern::formula([](Float max_speed, Float a) -> Float { return kern::math::max(max_speed, a); }, self.cur_.max_speed, vector_length(s.vel));
    if (math_compare<">=">(s.agl, 0.0)) {
      self.cur_.max_agl = kern::formula([](Float max_agl, Float agl) -> Float { return kern::math::max(max_agl, agl); }, self.cur_.max_agl, s.agl);
    }
    self.cur_.max_alt = kern::formula([](Float max_alt, Float z) -> Float { return kern::math::max(max_alt, z); }, self.cur_.max_alt, vector_break(s.pos).z);
    self.cur_.volts_end = s.volts;
    self.cur_.volts_min = kern::formula([](Float volts_min, Float volts) -> Float { return kern::math::min(volts_min, volts); }, self.cur_.volts_min, s.volts);
    if (logic_and(logic_or(force, math_compare<">=">(kern::formula([](Float t, Float last_path_) -> Float { return (t - last_path_); }, s.t, self.last_path_), kPathInterval)), math_compare<">">(kern::formula([](Float a, Float kMaxPath) -> Float { return ((a < 0.0) != (kMaxPath < 0.0) ? (kMaxPath < 0.0) : (a < kMaxPath)); }, kern::to<Float>(list_length<Vector>(self.cur_.path)), kern::to<Float>(kMaxPath)), 0.5))) {
      self.last_path_ = s.t;
      list_add(self.cur_.path, vector_make(vector_break(s.pos).x, vector_break(s.pos).y, s.agl));
    }
  }

  void FlightLog_finish(FlightLog& self, Text reason = "") {
    Int place = 0;
    Flight item = Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}};
    if (logic_not(self.active_)) {
      return;
    }
    self.active_ = false;
    self.cur_.reason = reason;
    place = 0;
    item = self.cur_;
    list_insert(self.flights_, place, item);
    if (math_compare<">">(kern::formula([](Float kMaxFlights, Float a) -> Float { return ((kMaxFlights < 0.0) != (a < 0.0) ? (a < 0.0) : (kMaxFlights < a)); }, kern::to<Float>(kMaxFlights), kern::to<Float>(list_length<Flight>(self.flights_))), 0.5)) {
      list_resize(self.flights_, kMaxFlights, Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}});
    }
    self.serial_ = kern::to<Int>(kern::formula([](Float serial_) -> Float { return kern::math::wrap32(serial_ + 1.0); }, kern::to<Float>(self.serial_)));
  }

  void FlightLog_observe(FlightLog& self, const FlightSample& s, Text disarm_reason = "") {
    Int was = 0;
    if (logic_and(s.armed, logic_not(self.active_))) {
      self.cur_ = struct_make<Flight>(0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{});
      was = self.next_id_;
      self.next_id_ = kern::to<Int>(kern::formula([](Float next_id_) -> Float { return kern::math::wrap32(next_id_ + 1.0); }, kern::to<Float>(self.next_id_)));
      self.cur_.id = was;
      self.cur_.start = s.t;
      self.cur_.volts_min = s.volts;
      self.cur_.volts_end = self.cur_.volts_min;
      self.cur_.volts_start = self.cur_.volts_end;
      self.cur_.max_alt = vector_break(s.pos).z;
      self.active_ = true;
      self.have_last_ = false;
      self.last_path_ = -1000000000.0;
      FlightLog_record(self, s, true);
      return;
    }
    if (logic_not(self.active_)) {
      return;
    }
    if (math_compare<"<">(kern::formula([](Float t) -> Float { return (t + 0.0001); }, s.t), self.last_t_)) {
      FlightLog_finish(self, "SIM_RESET");
      return;
    }
    FlightLog_record(self, s, false);
    if (logic_not(s.armed)) {
      FlightLog_finish(self, disarm_reason);
    }
  }

  void FlightLog_network(FlightLog& self, const Text& stage, Float t = 0.0) {
    Int place = 0;
    FlightLog_Event item = FlightLog_Event{0.0, ""};
    place = 0;
    item = struct_make<FlightLog_Event>(t, stage);
    list_insert(self.events_, place, item);
    if (math_compare<">">(kern::formula([](Float kMaxEvents, Float a) -> Float { return ((kMaxEvents < 0.0) != (a < 0.0) ? (a < 0.0) : (kMaxEvents < a)); }, kern::to<Float>(kMaxEvents), kern::to<Float>(list_length<FlightLog_Event>(self.events_))), 0.5)) {
      list_resize(self.events_, kMaxEvents, FlightLog_Event{0.0, ""});
    }
    self.serial_ = kern::to<Int>(kern::formula([](Float serial_) -> Float { return kern::math::wrap32(serial_ + 1.0); }, kern::to<Float>(self.serial_)));
  }

  struct FlightLog_jsonResult { Text Result = ""; };
  KERN_PURE FlightLog_jsonResult FlightLog_json(const FlightLog& self) {
    Text o = "";
    Text b = "";
    Text stage = "";
    Int c = 0;
    o = "{\"flights\":[";
    b = "";
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Flight>(self.flights_))))); it12.next();) {
      b = text_format<"%s{\"id\":%u,\"start\":%.2f,\"duration\":%.2f,\"distance\":%.1f,\"maxSpeed\":%.2f,\"maxAgl\":%.2f,\"maxAlt\":%.2f,\"voltsStart\":%.2f,\"voltsEnd\":%.2f,\"voltsMin\":%.2f,\"reason\":\"%s\",\"path\":[">(255, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n12.index), 0.0), ",", ""), self.flights_[n12.index].id, self.flights_[n12.index].start, self.flights_[n12.index].duration, self.flights_[n12.index].distance, self.flights_[n12.index].max_speed, self.flights_[n12.index].max_agl, self.flights_[n12.index].max_alt, self.flights_[n12.index].volts_start, self.flights_[n12.index].volts_end, self.flights_[n12.index].volts_min, self.flights_[n12.index].reason);
      o = text_append(o, b, "");
      for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(self.flights_[n12.index].path))))); it13.next();) {
        b = text_format<"%s[%.1f,%.1f,%.1f]">(255, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n13.index), 0.0), ",", ""), vector_break(self.flights_[n12.index].path[n13.index]).x, vector_break(self.flights_[n12.index].path[n13.index]).y, vector_break(self.flights_[n12.index].path[n13.index]).z);
        o = text_append(o, b, "");
      }
      o = text_append(o, "]}", "");
    }
    o = text_append(o, "],\"events\":[", "");
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<FlightLog_Event>(self.events_))))); it14.next();) {
      stage = "";
      for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(self.events_[n14.index].stage))))); it15.next();) {
        c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(self.events_[n14.index].stage, n15.index))));
        stage = text_append(stage, text_of_code(kern::to<Int>(kern::formula([](Float c) -> Float { return (((c == 34.0) || (c == 92.0)) ? 39.0 : ((kern::math::band(c, 255.0) < 32.0) ? 32.0 : c)); }, kern::to<Float>(c)))), "");
      }
      b = text_format<"%s{\"t\":%.1f,\"stage\":\"">(255, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n14.index), 0.0), ",", ""), self.events_[n14.index].t);
      o = text_append(o, text_append(text_append(b, stage, ""), "\"}", ""), "");
    }
    return {text_append(o, "]}", "")};
  }

  struct MusicSynth_station_countResult { Int Result = 0; };
  KERN_PURE MusicSynth_station_countResult MusicSynth_station_count() {
    return {5};
  }

  struct prog_ofResult { Prog Result = Prog{Genre::synthwave, 0.0, 0, 0, false}; };
  KERN_PURE prog_ofResult prog_of(Int p = 0) {
    MusicSynth_Station s = MusicSynth_Station{"", "", 0.0, 0};
    Genre g = Genre::synthwave;
    if (math_compare<"==">(kern::to<Float>(p), kern::to<Float>(kMenu))) {
      return {struct_make<Prog>(Genre::chip, 140.0, 7, 57, true)};
    }
    s = list_get<MusicSynth_Station>(kStations, p);
    g = enum_from_index<Genre>(kern::to<Int>(kern::formula([](Float p) -> Float { return kern::math::mod(p, 5.0); }, kern::to<Float>(p)))).value;
    return {struct_make<Prog>(g, s.bpm, s.seed, kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return ((a == a_2) ? 53.0 : ((a == a_3) ? 55.0 : ((a == a_4) ? 50.0 : 52.0))); }, kern::to<Float>(enum_index<Genre>(g)), kern::to<Float>(enum_index<Genre>(Genre::house)), kern::to<Float>(enum_index<Genre>(Genre::lofi)), kern::to<Float>(enum_index<Genre>(Genre::techno)))), math_compare<"!=">(kern::to<Float>(enum_index<Genre>(g)), kern::to<Float>(enum_index<Genre>(Genre::house))))};
  }

  struct midiResult { Float Result = 0.0; };
  KERN_PURE midiResult midi(Float n = 0.0) {
    return {kern::formula([](Float n) -> Float { return (440.0 * kern::math::pow(2.0, kern::math::div((n - 69.0), 12.0))); }, n)};
  }

  void MusicSynth_init(MusicSynth& self, Int rate = 0) {
    self.rate_ = rate;
  }

  void MusicSynth_select(MusicSynth& self, Int programme = 0, Float level = 0.0) {
    self.want_ = programme;
    self.target_ = kern::formula([](Float level) -> Float { return (level < 0.0 ? 0.0 : (1.0 < level ? 1.0 : level)); }, level);
  }

  struct MusicSynth_step_rndResult { Float Result = 0.0; };
  MusicSynth_step_rndResult MusicSynth_step_rnd(MusicSynth& self) {
    self.rng_ = kern::to<Int>(kern::formula([](Float rng_) -> Float { return kern::math::wrap32(kern::math::mul32(rng_, 1664525.0) + 1013904223.0); }, kern::to<Float>(self.rng_)));
    return {kern::formula([](Float rng_) -> Float { return kern::math::div(kern::math::shr(rng_, 8.0), 16777216.0); }, kern::to<Float>(self.rng_))};
  }

  struct step_decayResult { Float Result = 0.0; };
  KERN_PURE step_decayResult step_decay(Float rate = 0.0, Float tau = 0.0) {
    return {kern::formula([](Float tau, Float rate) -> Float { return kern::math::exp(kern::math::div((-1.0), (tau * rate))); }, tau, rate)};
  }

  void MusicSynth_step(MusicSynth& self) {
    Prog p = Prog{Genre::synthwave, 0.0, 0, 0, false};
    Int pos = 0;
    Int bar = 0;
    Int chord = 0;
    Float rate = 0.0;
    Bool kick = false;
    Bool snare = false;
    Bool hat = false;
    Int selected = 0;
    Bool both = false;
    Bool chosen = false;
    Bool bass_hit = false;
    Float lead_prob = 0.0;
    Bool both_2 = false;
    Int the_deg = 0;
    Int oct = 0;
    Int third = 0;
    Fixed<Int, 3> tones = Fixed<Int, 3>{};
    p = prog_of(self.programme_).Result;
    pos = kern::to<Int>(kern::formula([](Float step_) -> Float { return kern::math::mod(step_, 16.0); }, kern::to<Float>(self.step_)));
    bar = kern::to<Int>(kern::formula([](Float step_) -> Float { return kern::math::trunc(kern::math::div(step_, 16.0)); }, kern::to<Float>(self.step_)));
    chord = list_get<Int>(logic_select<List<Int>>(p.minor, kMinorProg, kMajorProg), kern::to<Int>(kern::formula([](Float bar) -> Float { return kern::math::mod(bar, 4.0); }, kern::to<Float>(bar))));
    rate = kern::to<Float>(self.rate_);
    kick = false;
    snare = logic_or(math_compare<"==">(kern::to<Float>(pos), 4.0), math_compare<"==">(kern::to<Float>(pos), 12.0));
    hat = false;
    selected = enum_index<Genre>(p.genre);
    if (logic_or(math_compare<"==">(kern::to<Float>(selected), 1.0), math_compare<"==">(kern::to<Float>(selected), 4.0))) {
      kick = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 4.0); }, kern::to<Float>(pos))))), 0.0);
      hat = logic_select<Bool>(math_compare<"==">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::house))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 4.0); }, kern::to<Float>(pos))))), 2.0), true);
    } else if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
      kick = logic_or(logic_or(math_compare<"==">(kern::to<Float>(pos), 0.0), math_compare<"==">(kern::to<Float>(pos), 8.0)), math_compare<"==">(kern::to<Float>(pos), 10.0));
      hat = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 2.0); }, kern::to<Float>(pos))))), 0.0);
    } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
      kick = logic_or(logic_or(math_compare<"==">(kern::to<Float>(pos), 0.0), math_compare<"==">(kern::to<Float>(pos), 7.0)), math_compare<"==">(kern::to<Float>(pos), 10.0));
      both = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 2.0); }, kern::to<Float>(pos))))), 0.0);
      if (both) {
        n17 = MusicSynth_step_rnd(self);
        both = math_compare<"<">(n17.Result, 0.8);
      }
      hat = both;
    } else if (math_compare<"==">(kern::to<Float>(selected), 3.0)) {
      kick = logic_or(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 8.0); }, kern::to<Float>(pos))))), 0.0), math_compare<"==">(kern::to<Float>(pos), 14.0));
      hat = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 2.0); }, kern::to<Float>(pos))))), 1.0);
    }
    if (math_compare<"==">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::techno)))) {
      if (logic_or(math_compare<"==">(kern::to<Float>(pos), 4.0), math_compare<"==">(kern::to<Float>(pos), 12.0))) {
        n19 = MusicSynth_step_rnd(self);
        chosen = math_compare<"<">(n19.Result, 0.5);
      } else {
        chosen = false;
      }
      snare = chosen;
    }
    if (kick) {
      self.kick_.env = 1.0;
      self.kick_.phase = 0.0;
      self.kick_.decay = step_decay(rate, 0.16).Result;
    }
    if (snare) {
      self.snare_.env = 1.0;
      self.snare_.decay = step_decay(rate, kern::formula([](Float a, Float a_2) -> Float { return ((a == a_2) ? 0.09 : 0.12); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::lofi)))).Result;
    }
    if (hat) {
      self.hat_.env = kern::formula([](Float a, Float a_2, Float pos) -> Float { return (((a == a_2) && (kern::math::mod(pos, 2.0) != 0.0)) ? 0.5 : 1.0); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::techno)), kern::to<Float>(pos));
      self.hat_.decay = step_decay(rate, kern::formula([](Float a, Float a_2) -> Float { return ((a == a_2) ? 0.03 : 0.045); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::techno)))).Result;
    }
    bass_hit = logic_select<Bool>(math_compare<"==">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::house))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 4.0); }, kern::to<Float>(pos))))), 2.0), logic_select<Bool>(math_compare<"==">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::techno))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 2.0); }, kern::to<Float>(pos))))), 1.0), logic_select<Bool>(math_compare<"==">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::lofi))), logic_or(math_compare<"==">(kern::to<Float>(pos), 0.0), math_compare<"==">(kern::to<Float>(pos), 10.0)), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 2.0); }, kern::to<Float>(pos))))), 0.0))));
    if (bass_hit) {
      self.bass_.freq = midi(kern::formula([](Float root, Float chord) -> Float { return ((root - 12.0) + chord); }, kern::to<Float>(p.root), kern::to<Float>(chord))).Result;
      self.bass_.env = 1.0;
      self.bass_.decay = step_decay(rate, kern::formula([](Float a, Float a_2) -> Float { return ((a == a_2) ? 0.5 : 0.18); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::lofi)))).Result;
    }
    lead_prob = kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return ((a == a_2) ? 0.85 : ((a == a_3) ? 0.55 : ((a == a_4) ? 0.3 : 0.25))); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::chip)), kern::to<Float>(enum_index<Genre>(Genre::synthwave)), kern::to<Float>(enum_index<Genre>(Genre::lofi)));
    both_2 = logic_or(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pos) -> Float { return kern::math::mod(pos, 2.0); }, kern::to<Float>(pos))))), 0.0), math_compare<"==">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::chip))));
    if (both_2) {
      n21 = MusicSynth_step_rnd(self);
      both_2 = math_compare<"<">(n21.Result, lead_prob);
    }
    if (both_2) {
      n22 = MusicSynth_step_rnd(self);
      the_deg = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::mod(kern::math::trunc((the_result * 7.0)), 7.0); }, n22.Result));
      n23 = MusicSynth_step_rnd(self);
      oct = kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result < 0.3) ? 12.0 : 0.0); }, n23.Result));
      self.lead_.freq = midi(kern::formula([](Float root, Float chord, Float minor, Float a, Float a_2, Float oct) -> Float { return ((((root + 12.0) + chord) + ((minor != 0.0) ? a : a_2)) + oct); }, kern::to<Float>(p.root), kern::to<Float>(chord), kern::to<Float>(p.minor), kern::to<Float>(list_get<Int>(kMinorScale, the_deg)), kern::to<Float>(list_get<Int>(kMajorScale, the_deg)), kern::to<Float>(oct))).Result;
      self.lead_.env = 1.0;
      self.lead_.decay = step_decay(rate, kern::formula([](Float a, Float a_2) -> Float { return ((a == a_2) ? 0.09 : 0.22); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::chip)))).Result;
    }
    if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(pos), 0.0), math_compare<"!=">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::chip)))), math_compare<"!=">(kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::techno))))) {
      third = kern::to<Int>(kern::formula([](Float minor) -> Float { return ((minor != 0.0) ? 3.0 : 4.0); }, kern::to<Float>(p.minor)));
      tones = list_make<Int, "3">(0, third, 7);
      for (auto it24 = flow_for(n24, 0, 2); it24.next();) {
        self.pad_[n24.index].freq = midi(kern::formula([](Float root, Float chord, Float a) -> Float { return ((root + chord) + a); }, kern::to<Float>(p.root), kern::to<Float>(chord), kern::to<Float>(tones[n24.index]))).Result;
        self.pad_[n24.index].env = 1.0;
        self.pad_[n24.index].decay = step_decay(rate, 1.6).Result;
      }
    }
    self.step_ = kern::to<Int>(kern::formula([](Float step_) -> Float { return kern::math::wrap32(step_ + 1.0); }, kern::to<Float>(self.step_)));
  }

  struct MusicSynth_next_sample_noiseResult { Float Result = 0.0; };
  MusicSynth_next_sample_noiseResult MusicSynth_next_sample_noise(MusicSynth& self) {
    self.rng_ = kern::to<Int>(kern::formula([](Float rng_) -> Float { return kern::math::wrap32(kern::math::mul32(rng_, 1664525.0) + 1013904223.0); }, kern::to<Float>(self.rng_)));
    return {kern::formula([](Float rng_) -> Float { return kern::math::div(kern::math::wrap32s(rng_), 2147483648.0); }, kern::to<Float>(self.rng_))};
  }

  struct MusicSynth_next_sampleResult { Float Result = 0.0; };
  MusicSynth_next_sampleResult MusicSynth_next_sample(MusicSynth& self, Float& right) {
    Prog p = Prog{Genre::synthwave, 0.0, 0, 0, false};
    Float inv = 0.0;
    Float s = 0.0;
    Float f = 0.0;
    Float n = 0.0;
    Float raw = 0.0;
    Float lead = 0.0;
    Float duty = 0.0;
    Float sq = 0.0;
    Float tri = 0.0;
    Float pad = 0.0;
    Bool skip = false;
    Int echo_len = 0;
    Float echo = 0.0;
    Float mono = 0.0;
    p = prog_of(self.programme_).Result;
    inv = kern::formula([](Float rate_) -> Float { return kern::math::div(1.0, rate_); }, kern::to<Float>(self.rate_));
    s = 0.0;
    if (math_compare<">">(self.kick_.env, 0.0001)) {
      f = kern::formula([](Float env) -> Float { return (45.0 + ((105.0 * env) * env)); }, self.kick_.env);
      self.kick_.phase = kern::formula([](Float phase, Float f, Float inv) -> Float { return (phase + (f * inv)); }, self.kick_.phase, f, inv);
      s += kern::formula([](Float phase, Float env) -> Float { return ((kern::math::sin((phase * 6.2831853)) * env) * 0.9); }, self.kick_.phase, self.kick_.env);
      self.kick_.env = kern::formula([](Float env, Float decay) -> Float { return (env * decay); }, self.kick_.env, self.kick_.decay);
    }
    if (math_compare<">">(self.snare_.env, 0.0001)) {
      n25 = MusicSynth_next_sample_noise(self);
      s += kern::formula([](Float the_result, Float env) -> Float { return ((the_result * env) * 0.32); }, n25.Result, self.snare_.env);
      self.snare_.env = kern::formula([](Float env, Float decay) -> Float { return (env * decay); }, self.snare_.env, self.snare_.decay);
    }
    if (math_compare<">">(self.hat_.env, 0.0001)) {
      n26 = MusicSynth_next_sample_noise(self);
      n = n26.Result;
      s += kern::formula([](Float n, Float phase, Float env) -> Float { return (((n - phase) * env) * 0.12); }, n, self.hat_.phase, self.hat_.env);
      self.hat_.phase = n;
      self.hat_.env = kern::formula([](Float env, Float decay) -> Float { return (env * decay); }, self.hat_.env, self.hat_.decay);
    }
    if (math_compare<">">(self.bass_.env, 0.0001)) {
      self.bass_.phase = kern::formula([](Float phase, Float freq, Float inv) -> Float { return (phase + (freq * inv)); }, self.bass_.phase, self.bass_.freq, inv);
      self.bass_.phase = kern::formula([](Float phase) -> Float { return (phase - kern::math::floor(phase)); }, self.bass_.phase);
      raw = kern::formula([](Float a, Float a_2, Float phase) -> Float { return ((a == a_2) ? ((phase < 0.5) ? ((4.0 * phase) - 1.0) : (3.0 - (4.0 * phase))) : ((2.0 * phase) - 1.0)); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::chip)), self.bass_.phase);
      self.bass_lp_ = kern::formula([](Float bass_lp_, Float raw, Float a, Float a_2) -> Float { return (bass_lp_ + ((raw - bass_lp_) * ((a == a_2) ? 0.04 : 0.12))); }, self.bass_lp_, raw, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::lofi)));
      s += kern::formula([](Float bass_lp_, Float env) -> Float { return ((bass_lp_ * env) * 0.45); }, self.bass_lp_, self.bass_.env);
      self.bass_.env = kern::formula([](Float env, Float decay) -> Float { return (env * decay); }, self.bass_.env, self.bass_.decay);
    }
    lead = 0.0;
    if (math_compare<">">(self.lead_.env, 0.0001)) {
      self.lead_.phase = kern::formula([](Float phase, Float freq, Float inv) -> Float { return (phase + (freq * inv)); }, self.lead_.phase, self.lead_.freq, inv);
      self.lead_.phase = kern::formula([](Float phase) -> Float { return (phase - kern::math::floor(phase)); }, self.lead_.phase);
      duty = kern::formula([](Float a, Float a_2) -> Float { return ((a == a_2) ? 0.25 : 0.5); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::chip)));
      sq = kern::formula([](Float phase, Float duty) -> Float { return ((phase < duty) ? 1.0 : (-1.0)); }, self.lead_.phase, duty);
      tri = kern::formula([](Float phase) -> Float { return ((phase < 0.5) ? ((4.0 * phase) - 1.0) : (3.0 - (4.0 * phase))); }, self.lead_.phase);
      lead = kern::formula([](Float a, Float a_2, Float a_3, Float sq, Float tri, Float env) -> Float { return (((((a == a_2) || (a == a_3)) ? ((sq * 0.6) + (tri * 0.4)) : tri) * env) * 0.16); }, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::chip)), kern::to<Float>(enum_index<Genre>(Genre::synthwave)), sq, tri, self.lead_.env);
      self.lead_.env = kern::formula([](Float env, Float decay) -> Float { return (env * decay); }, self.lead_.env, self.lead_.decay);
    }
    pad = 0.0;
    for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MusicSynth_Voice>(self.pad_))))); it27.next();) {
      skip = false;
      if (math_compare<"<">(self.pad_[n27.index].env, 0.0001)) {
        skip = true;
      }
      if (logic_not(skip)) {
        self.pad_[n27.index].phase = kern::formula([](Float a, Float a_2, Float inv) -> Float { return (a + (a_2 * inv)); }, self.pad_[n27.index].phase, self.pad_[n27.index].freq, inv);
        self.pad_[n27.index].phase2 = kern::formula([](Float a, Float a_2, Float inv) -> Float { return (a + ((a_2 * 1.006) * inv)); }, self.pad_[n27.index].phase2, self.pad_[n27.index].freq, inv);
        self.pad_[n27.index].phase = kern::formula([](Float a) -> Float { return (a - kern::math::floor(a)); }, self.pad_[n27.index].phase);
        self.pad_[n27.index].phase2 = kern::formula([](Float a) -> Float { return (a - kern::math::floor(a)); }, self.pad_[n27.index].phase2);
        pad += kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (((((2.0 * a) - 1.0) + ((2.0 * a_2) - 1.0)) * a_3) * 0.035); }, self.pad_[n27.index].phase, self.pad_[n27.index].phase2, self.pad_[n27.index].env);
        self.pad_[n27.index].env = kern::formula([](Float a, Float a_2) -> Float { return (a * a_2); }, self.pad_[n27.index].env, self.pad_[n27.index].decay);
      }
    }
    self.lp_ = kern::formula([](Float lp_, Float pad, Float a, Float a_2) -> Float { return (lp_ + ((pad - lp_) * ((a == a_2) ? 0.03 : 0.08))); }, self.lp_, pad, kern::to<Float>(enum_index<Genre>(p.genre)), kern::to<Float>(enum_index<Genre>(Genre::lofi)));
    echo_len = kern::to<Int>(kern::formula([](Float samples_per_step_) -> Float { return kern::math::min(kern::math::wrap32(24000.0), kern::math::mul32(samples_per_step_, 3.0)); }, kern::to<Float>(self.samples_per_step_)));
    echo = 0.0;
    if (math_compare<">">(kern::to<Float>(echo_len), 0.0)) {
      self.echo_at_ = kern::to<Int>(kern::formula([](Float echo_at_, Float echo_len) -> Float { return kern::math::mod(echo_at_, echo_len); }, kern::to<Float>(self.echo_at_), kern::to<Float>(echo_len)));
      echo = self.echo_[self.echo_at_];
      self.echo_[self.echo_at_] = kern::formula([](Float lead, Float echo) -> Float { return (lead + (echo * 0.38)); }, lead, echo);
      self.echo_at_ = kern::to<Int>(kern::formula([](Float echo_at_) -> Float { return kern::math::wrap32(echo_at_ + 1.0); }, kern::to<Float>(self.echo_at_)));
    }
    mono = kern::formula([](Float s, Float lp_) -> Float { return (s + lp_); }, s, self.lp_);
    right = kern::formula([](Float mono, Float lead, Float echo) -> Float { return ((mono + (lead * 0.9)) + (echo * 0.5)); }, mono, lead, echo);
    return {kern::formula([](Float mono, Float lead, Float echo) -> Float { return ((mono + lead) + (echo * 0.3)); }, mono, lead, echo)};
  }

  void MusicSynth_render_add(MusicSynth& self, List<Float>& out, Int frames = 0) {
    Bool skip = false;
    Prog p = Prog{Genre::synthwave, 0.0, 0, 0, false};
    Float r = 0.0;
    Float l = 0.0;
    Float g = 0.0;
    self.peak = 0.0;
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.rate_), 0.0))) {
      return;
    }
    for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(frames)))); it28.next();) {
      skip = false;
      if (math_compare<"!=">(kern::to<Float>(self.want_), kern::to<Float>(self.programme_))) {
        self.fade_ = kern::formula([](Float fade_, Float rate_) -> Float { return (fade_ - kern::math::div(1.0, (0.4 * rate_))); }, self.fade_, kern::to<Float>(self.rate_));
        if (logic_or(math_compare<"<=">(self.fade_, 0.0), math_compare<"==">(kern::to<Float>(self.programme_), -2.0))) {
          self.programme_ = self.want_;
          self.fade_ = 0.0;
          self.step_ = 0;
          self.until_step_ = 0;
          if (math_compare<"!=">(kern::to<Float>(self.programme_), -2.0)) {
            p = prog_of(self.programme_).Result;
            self.rng_ = kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(kern::math::mul32(seed, 2654435761.0) + 1.0); }, kern::to<Float>(p.seed)));
            self.samples_per_step_ = kern::to<Int>(kern::formula([](Float rate_, Float bpm) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::div(kern::math::div((rate_ * 60.0), bpm), 4.0))); }, kern::to<Float>(self.rate_), p.bpm));
          }
          self.lead_ = struct_make<MusicSynth_Voice>(0.0, 0.0, 0.0, 0.9995, 0.0, 0.0);
          self.bass_ = self.lead_;
          self.hat_ = self.bass_;
          self.snare_ = self.hat_;
          self.kick_ = self.snare_;
          for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MusicSynth_Voice>(self.pad_))))); it29.next();) {
            self.pad_[n29.index] = struct_make<MusicSynth_Voice>(0.0, 0.0, 0.0, 0.9995, 0.0, 0.0);
          }
        }
      } else if (math_compare<"<">(self.fade_, 1.0)) {
        self.fade_ = kern::formula([](Float fade_, Float rate_) -> Float { return kern::math::min(1.0, (fade_ + kern::math::div(1.0, (0.6 * rate_)))); }, self.fade_, kern::to<Float>(self.rate_));
      }
      self.gain_ = kern::formula([](Float gain_, Float target_) -> Float { return (gain_ + ((target_ - gain_) * 0.0005)); }, self.gain_, self.target_);
      if (logic_or(math_compare<"==">(kern::to<Float>(self.programme_), -2.0), math_compare<"<">(self.gain_, 0.0001))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"==">(kern::to<Float>(self.until_step_), 0.0)) {
          MusicSynth_step(self);
          self.until_step_ = self.samples_per_step_;
        }
        self.until_step_ = kern::to<Int>(kern::formula([](Float until_step_) -> Float { return kern::math::wrap32(until_step_ - 1.0); }, kern::to<Float>(self.until_step_)));
        r = 0.0;
        n30 = MusicSynth_next_sample(self, r);
        l = n30.Result;
        g = kern::formula([](Float gain_, Float fade_) -> Float { return ((gain_ * fade_) * 0.7); }, self.gain_, self.fade_);
        out[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mul32(2.0, index); }, kern::to<Float>(n28.index)))] = kern::formula([](Float a, Float l, Float g) -> Float { return (a + (l * g)); }, out[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mul32(2.0, index); }, kern::to<Float>(n28.index)))], l, g);
        out[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(kern::math::mul32(2.0, index) + 1.0); }, kern::to<Float>(n28.index)))] = kern::formula([](Float a, Float r, Float g) -> Float { return (a + (r * g)); }, out[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(kern::math::mul32(2.0, index) + 1.0); }, kern::to<Float>(n28.index)))], r, g);
        self.peak = kern::formula([](Float peak, Float l, Float g) -> Float { return kern::math::max(peak, kern::math::abs((l * g))); }, self.peak, l, g);
      }
    }
  }

  // ---- what nodes give and keep ----
  struct { Jetpack_Pack pack = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0}; Bool lit = false; } n10;
  struct { Jetpack_Pack pack = Jetpack_Pack{1.0, false, false, 0, 0.0, -1.0, -99.0}; Float thrust = 0.0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  MusicSynth_step_rndResult n17;
  MusicSynth_step_rndResult n19;
  MusicSynth_step_rndResult n21;
  MusicSynth_step_rndResult n22;
  MusicSynth_step_rndResult n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  MusicSynth_next_sample_noiseResult n25;
  MusicSynth_next_sample_noiseResult n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  MusicSynth_next_sampleResult n30;
};
