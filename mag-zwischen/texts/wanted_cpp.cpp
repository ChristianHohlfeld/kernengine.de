struct wanted_cpp : virtual kern::Library, virtual blueprint_call, virtual wanted {
  // ---- functions ----
  struct Wanted_SeverityResult { Int severity = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Severity") Wanted_SeverityResult Wanted_Severity(Int crime = 0);

  struct Wanted_StarsForHeatResult { Int stars = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("StarsForHeat") Wanted_StarsForHeatResult Wanted_StarsForHeat(Float heat_2 = 0.0);

  struct Wanted_PoliceCountResult { Int count = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("PoliceCount") Wanted_PoliceCountResult Wanted_PoliceCount(Int stars_2 = 0);

  struct Wanted_DetectionRadiusResult { Float meters = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("DetectionRadius") Wanted_DetectionRadiusResult Wanted_DetectionRadius(Int stars_2 = 0);

  struct Wanted_EscapeMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("EscapeMs") Wanted_EscapeMsResult Wanted_EscapeMs(Int stars_2 = 0);

  struct Wanted_PoliceDamageResult { Float damage = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("PoliceDamage") Wanted_PoliceDamageResult Wanted_PoliceDamage(Int stars_2 = 0);

  struct Wanted_SpawnRadiusResult { Float meters = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("SpawnRadius") Wanted_SpawnRadiusResult Wanted_SpawnRadius(Int index = 0);

  struct Wanted_ArrivalDelayMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("ArrivalDelayMs") Wanted_ArrivalDelayMsResult Wanted_ArrivalDelayMs(Int stars_2 = 0);

  struct Wanted_EngageDelayMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("EngageDelayMs") Wanted_EngageDelayMsResult Wanted_EngageDelayMs(Int stars_2 = 0);

  struct Wanted_WaveBreakMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("WaveBreakMs") Wanted_WaveBreakMsResult Wanted_WaveBreakMs(Int stars_2 = 0);

  struct Wanted_FireRangeResult { Float meters = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("FireRange") Wanted_FireRangeResult Wanted_FireRange(Int stars_2 = 0);

  struct Wanted_FireIntervalMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("FireIntervalMs") Wanted_FireIntervalMsResult Wanted_FireIntervalMs(Int stars_2 = 0);

  struct Wanted_HitChanceResult { Float chance = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("HitChance") Wanted_HitChanceResult Wanted_HitChance(Int stars_2 = 0, Float distance = 0.0, Float speed = 0.0);

  struct Wanted_EmpImpulseResult { Float impulse = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("EmpImpulse") Wanted_EmpImpulseResult Wanted_EmpImpulse(Float distance = 0.0);

  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Report") void Wanted_Report(Int crime = 0, Bool witnessed = false);

  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Update") void Wanted_Update(Float dt = 0.0, Bool seen = false);

  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Clear") void Wanted_Clear();

  struct Wanted_StarsResult { Int stars = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Stars") Wanted_StarsResult Wanted_Stars();

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("Wanted"))};
  }

  struct max_starsResult { Int Result = 0; };
  KERN_PURE max_starsResult max_stars() {
    return {func_get_on<"MaxStars", Int>(actor_find("Wanted"))};
  }

  struct emp_rangeResult { Float Result = 0.0; };
  KERN_PURE emp_rangeResult emp_range() {
    return {func_get_on<"EmpRange", Float>(actor_find("Wanted"))};
  }

  struct emp_cooldown_msResult { Float Result = 0.0; };
  KERN_PURE emp_cooldown_msResult emp_cooldown_ms() {
    return {func_get_on<"EmpCooldownMs", Float>(actor_find("Wanted"))};
  }

  struct max_fire_rangeResult { Float Result = 0.0; };
  KERN_PURE max_fire_rangeResult max_fire_range() {
    return {func_get_on<"MaxFireRange", Float>(actor_find("Wanted"))};
  }

  struct severityResult { Int Result = 0; };
  severityResult severity(wanted_Crime c = wanted_Crime::person) {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_Severity">(actor_find("Wanted"), kern::out(n10.severity), enum_index<wanted_Crime>(c));
    r = n10.severity;
    return {r};
  }

  struct stars_for_heatResult { Int Result = 0; };
  stars_for_heatResult stars_for_heat(Float heat_2 = 0.0) {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_StarsForHeat">(actor_find("Wanted"), kern::out(n11.stars), blueprint_call::decimal(heat_2).Result);
    r = n11.stars;
    return {r};
  }

  struct police_countResult { Int Result = 0; };
  police_countResult police_count(Int stars_2 = 0) {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_PoliceCount">(actor_find("Wanted"), kern::out(n12.count), stars_2);
    r = n12.count;
    return {r};
  }

  struct detection_radiusResult { Float Result = 0.0; };
  detection_radiusResult detection_radius(Int stars_2 = 0) {
    func_call_on<"Wanted_DetectionRadius">(actor_find("Wanted"), kern::out(n13.meters), stars_2);
    return {n13.meters};
  }

  struct escape_msResult { Float Result = 0.0; };
  escape_msResult escape_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_EscapeMs">(actor_find("Wanted"), kern::out(n14.ms), stars_2);
    return {n14.ms};
  }

  struct police_damageResult { Float Result = 0.0; };
  police_damageResult police_damage(Int stars_2 = 0) {
    func_call_on<"Wanted_PoliceDamage">(actor_find("Wanted"), kern::out(n15.damage), stars_2);
    return {n15.damage};
  }

  struct spawn_radiusResult { Float Result = 0.0; };
  spawn_radiusResult spawn_radius(Int index = 0) {
    func_call_on<"Wanted_SpawnRadius">(actor_find("Wanted"), kern::out(n16.meters), index);
    return {n16.meters};
  }

  struct arrival_delay_msResult { Float Result = 0.0; };
  arrival_delay_msResult arrival_delay_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_ArrivalDelayMs">(actor_find("Wanted"), kern::out(n17.ms), stars_2);
    return {n17.ms};
  }

  struct engage_delay_msResult { Float Result = 0.0; };
  engage_delay_msResult engage_delay_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_EngageDelayMs">(actor_find("Wanted"), kern::out(n18.ms), stars_2);
    return {n18.ms};
  }

  struct wave_break_msResult { Float Result = 0.0; };
  wave_break_msResult wave_break_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_WaveBreakMs">(actor_find("Wanted"), kern::out(n19.ms), stars_2);
    return {n19.ms};
  }

  struct fire_rangeResult { Float Result = 0.0; };
  fire_rangeResult fire_range(Int stars_2 = 0) {
    func_call_on<"Wanted_FireRange">(actor_find("Wanted"), kern::out(n20.meters), stars_2);
    return {n20.meters};
  }

  struct fire_interval_msResult { Float Result = 0.0; };
  fire_interval_msResult fire_interval_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_FireIntervalMs">(actor_find("Wanted"), kern::out(n21.ms), stars_2);
    return {n21.ms};
  }

  struct hit_chanceResult { Float Result = 0.0; };
  hit_chanceResult hit_chance(Int stars_2 = 0, Float distance_m = 0.0, Float player_speed = 0.0) {
    func_call_on<"Wanted_HitChance">(actor_find("Wanted"), kern::out(n22.chance), stars_2, blueprint_call::decimal(distance_m).Result, blueprint_call::decimal(player_speed).Result);
    return {n22.chance};
  }

  struct emp_impulseResult { Float Result = 0.0; };
  emp_impulseResult emp_impulse(Float distance_m = 0.0) {
    func_call_on<"Wanted_EmpImpulse">(actor_find("Wanted"), kern::out(n23.impulse), blueprint_call::decimal(distance_m).Result);
    return {n23.impulse};
  }

  void report(wanted_Crime c = wanted_Crime::person, Bool witnessed = false) {
    func_call_on<"Wanted_Report">(actor_find("Wanted"), kern::out(), enum_index<wanted_Crime>(c), witnessed);
  }

  void update(Float dt = 0.0, Bool seen = false) {
    func_call_on<"Wanted_Update">(actor_find("Wanted"), kern::out(), blueprint_call::decimal(dt).Result, seen);
  }

  void clear() {
    func_call_on<"Wanted_Clear">(actor_find("Wanted"), kern::out());
  }

  struct hidden_msResult { Float Result = 0.0; };
  KERN_PURE hidden_msResult hidden_ms() {
    return {func_get_on<"HiddenMs", Float>(actor_find("Wanted"))};
  }

  struct heatResult { Float Result = 0.0; };
  KERN_PURE heatResult heat() {
    return {func_get_on<"Heat", Float>(actor_find("Wanted"))};
  }

  struct starsResult { Int Result = 0; };
  starsResult stars() {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_Stars">(actor_find("Wanted"), kern::out(n27.stars));
    r = n27.stars;
    return {r};
  }

  // ---- what nodes give and keep ----
  struct { Int severity = 0; } n10;
  struct { Int stars = 0; } n11;
  struct { Int count = 0; } n12;
  struct { Float meters = 0.0; } n13;
  struct { Float ms = 0.0; } n14;
  struct { Float damage = 0.0; } n15;
  struct { Float meters = 0.0; } n16;
  struct { Float ms = 0.0; } n17;
  struct { Float ms = 0.0; } n18;
  struct { Float ms = 0.0; } n19;
  struct { Float meters = 0.0; } n20;
  struct { Float ms = 0.0; } n21;
  struct { Float chance = 0.0; } n22;
  struct { Float impulse = 0.0; } n23;
  struct { Int stars = 0; } n27;
};
