struct guided_cpp : virtual kern::Library, virtual blueprint_call, virtual guided {
  // ---- types ----
  KERN_STRUCT(guided_MissileRun) { Vector p = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; Vector goal = Vector{0.0, 0.0, 0.0}; Float born = 0.0; Float cruise_z = 0.0; Bool motor = false; Bool pedestrian = false; Float dodge_seconds = 0.0; Bool dodge_lost = false; Vector dodge_last = Vector{0.0, 0.0, 0.0}; Bool tracking = true; };

  // ---- functions ----
  struct Guided_UpdateDodgeResult { Float seconds = 0.0; Bool lost = false; };
  /// (of the Blueprint of the actor Guided, called on it)
  KERN_EXTERN_AS("UpdateDodge") Guided_UpdateDodgeResult Guided_UpdateDodge(Float seconds = 0.0, Bool lost = false, Vector previous = Vector{0.0, 0.0, 0.0}, Vector current = Vector{0.0, 0.0, 0.0}, Float dt = 0.0, Bool pedestrian = false);

  struct Guided_PickScoreResult { Float px = 0.0; };
  /// (of the Blueprint of the actor Guided, called on it)
  KERN_EXTERN_AS("PickScore") Guided_PickScoreResult Guided_PickScore(Vector eye = Vector{0.0, 0.0, 0.0}, Vector forward = Vector{0.0, 0.0, 0.0}, Float fov = 0.0, Float heightPx = 0.0, Vector target = Vector{0.0, 0.0, 0.0});

  struct Guided_LaunchResult { guided_MissileRun missile = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; };
  /// (of the Blueprint of the actor Guided, called on it)
  KERN_EXTERN_AS("Launch") Guided_LaunchResult Guided_Launch(Vector start_2 = Vector{0.0, 0.0, 0.0}, Vector target = Vector{0.0, 0.0, 0.0}, Float now = 0.0, Bool pedestrian = false);

  struct Guided_TrackResult { guided_MissileRun missile = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; };
  /// (of the Blueprint of the actor Guided, called on it)
  KERN_EXTERN_AS("Track") Guided_TrackResult Guided_Track(guided_MissileRun m = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}, Vector at_2 = Vector{0.0, 0.0, 0.0}, Float dt = 0.0);

  struct Guided_StepResult { guided_MissileRun missile = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; the_guided_Result_ result = the_guided_Result_::flying; };
  /// (of the Blueprint of the actor Guided, called on it)
  KERN_EXTERN_AS("Step") Guided_StepResult Guided_Step(guided_MissileRun m = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}, Float now = 0.0, Float dt = 0.0, Float ground = 0.0);

  struct flight_ofResult { guided_MissileRun Result = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; };
  KERN_PURE flight_ofResult flight_of(const guided_Missile& m) {
    return {struct_make<guided_MissileRun>(m.p, m.v, m.goal, blueprint_call::decimal(m.born).Result, blueprint_call::decimal(m.cruise_z).Result, m.motor, m.pedestrian, blueprint_call::decimal(m.dodge.seconds).Result, m.dodge.lost, vector_make(vector_break(m.dodge_last).x, vector_break(m.dodge_last).y, 0.0), m.tracking)};
  }

  void take(const guided_MissileRun& f, guided_Missile& m) {
    m.p = f.p;
    m.v = f.v;
    m.goal = f.goal;
    m.born = f.born;
    m.cruise_z = f.cruise_z;
    m.motor = f.motor;
    m.pedestrian = f.pedestrian;
    m.dodge.seconds = f.dodge_seconds;
    m.dodge.lost = f.dodge_lost;
    m.dodge_last = vector_make(vector_break(f.dodge_last).x, vector_break(f.dodge_last).y, 0.0);
    m.tracking = f.tracking;
  }

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("Guided"))};
  }

  struct speedResult { Float Result = 0.0; };
  KERN_PURE speedResult speed() {
    return {func_get_on<"Speed", Float>(actor_find("Guided"))};
  }

  struct turn_gResult { Float Result = 0.0; };
  KERN_PURE turn_gResult turn_g() {
    return {func_get_on<"TurnG", Float>(actor_find("Guided"))};
  }

  struct blast_radiusResult { Float Result = 0.0; };
  KERN_PURE blast_radiusResult blast_radius() {
    return {func_get_on<"BlastRadius", Float>(actor_find("Guided"))};
  }

  struct damageResult { Float Result = 0.0; };
  KERN_PURE damageResult damage() {
    return {func_get_on<"Damage", Float>(actor_find("Guided"))};
  }

  struct cooldownResult { Float Result = 0.0; };
  KERN_PURE cooldownResult cooldown() {
    return {func_get_on<"Cooldown", Float>(actor_find("Guided"))};
  }

  struct pick_pxResult { Float Result = 0.0; };
  KERN_PURE pick_pxResult pick_px() {
    return {func_get_on<"PickPx", Float>(actor_find("Guided"))};
  }

  struct rangeResult { Float Result = 0.0; };
  KERN_PURE rangeResult range() {
    return {func_get_on<"Range", Float>(actor_find("Guided"))};
  }

  struct igniteResult { Float Result = 0.0; };
  KERN_PURE igniteResult ignite() {
    return {func_get_on<"Ignite", Float>(actor_find("Guided"))};
  }

  struct update_dodgeResult { guided_Dodge Result = guided_Dodge{0.0, false}; };
  update_dodgeResult update_dodge(guided_Dodge state = guided_Dodge{0.0, false}, Vector previous = Vector{0.0, 0.0, 0.0}, Vector current = Vector{0.0, 0.0, 0.0}, Float dt = 0.0, Bool pedestrian = false) {
    Float seconds = 0.0;
    Bool lost = false;
    seconds = 0.0;
    lost = false;
    func_call_on<"Guided_UpdateDodge">(actor_find("Guided"), kern::out(n10.seconds, n10.lost), blueprint_call::decimal(state.seconds).Result, state.lost, vector_make(vector_break(previous).x, vector_break(previous).y, 0.0), vector_make(vector_break(current).x, vector_break(current).y, 0.0), blueprint_call::decimal(dt).Result, pedestrian);
    seconds = n10.seconds;
    lost = n10.lost;
    return {struct_make<guided_Dodge>(seconds, lost)};
  }

  struct pick_scoreResult { Float Result = 0.0; };
  pick_scoreResult pick_score(Vector eye = Vector{0.0, 0.0, 0.0}, Vector forward = Vector{0.0, 0.0, 0.0}, Float fov = 0.0, Float height_px = 0.0, Vector target = Vector{0.0, 0.0, 0.0}) {
    func_call_on<"Guided_PickScore">(actor_find("Guided"), kern::out(n11.px), eye, forward, blueprint_call::decimal(fov).Result, blueprint_call::decimal(height_px).Result, target);
    return {n11.px};
  }

  struct launchResult { guided_Missile Result = guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true}; };
  launchResult launch(Vector start = Vector{0.0, 0.0, 0.0}, Vector target = Vector{0.0, 0.0, 0.0}, Float now = 0.0, Bool pedestrian = false) {
    guided_MissileRun f = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true};
    guided_Missile m = guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true};
    f = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true};
    func_call_on<"Guided_Launch">(actor_find("Guided"), kern::out(n12.missile), start, target, blueprint_call::decimal(now).Result, pedestrian);
    f = n12.missile;
    m = guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true};
    take(f, m);
    return {m};
  }

  void track(guided_Missile& m, Vector at = Vector{0.0, 0.0, 0.0}, Float dt = 0.0) {
    guided_MissileRun f = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true};
    f = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true};
    func_call_on<"Guided_Track">(actor_find("Guided"), kern::out(n13.missile), flight_of(m).Result, at, blueprint_call::decimal(dt).Result);
    f = n13.missile;
    take(f, m);
  }

  struct stepResult { the_guided_Result_ Result = the_guided_Result_::flying; };
  stepResult step(guided_Missile& m, Float now = 0.0, Float dt = 0.0, Float ground = 0.0) {
    guided_MissileRun f = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true};
    the_guided_Result_ result = the_guided_Result_::flying;
    f = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true};
    result = the_guided_Result_::flying;
    func_call_on<"Guided_Step">(actor_find("Guided"), kern::out(n14.missile, n14.result), flight_of(m).Result, blueprint_call::decimal(now).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(ground).Result);
    f = n14.missile;
    result = n14.result;
    take(f, m);
    return {result};
  }

  // ---- what nodes give and keep ----
  struct { Float seconds = 0.0; Bool lost = false; } n10;
  struct { Float px = 0.0; } n11;
  struct { guided_MissileRun missile = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; } n12;
  struct { guided_MissileRun missile = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; } n13;
  struct { guided_MissileRun missile = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; the_guided_Result_ result = the_guided_Result_::flying; } n14;
};
