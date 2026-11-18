struct combat_rules_cpp : virtual kern::Library, virtual blueprint_call, virtual combat_rules {
  // ---- types ----
  KERN_STRUCT(combat_ChaosRun) { Int score = 0; Int combo = 1; Float combo_until = -1.0; Float flow = 0.0; };
  KERN_STRUCT(combat_FunRun) { Int score = 0; Int at_risk = 0; Int high_score = 0; Int stars = 0; Int max_stars = 0; Int combo = 0; Int best_combo = 0; Int momentum = 0; Float combo_expires = -1000000000.0; Float overdrive_until = -1000000000.0; Int contract_index = 0; Int contract_progress = 0; Int contracts_completed = 0; Int mercy = 0; Int last_award = 0; Float run_damage = 0.0; Bool contract_complete = false; Text last_label = "READY"; };
  KERN_STRUCT(combat_Rounds) { Bool active = false; Int the_round = 0; Int to_spawn = 0; Int points = 0; Float next_spawn = 0.0; Float break_until = -1.0; };

  // ---- functions ----
  struct hash_textResult { Int Result = 0; };
  /// flak_battery.mjs hash() and rand(): a cell's own numbers
  KERN_PURE hash_textResult hash_text(const Text& s) {
    Int h = 0;
    Int c = 0;
    h = 2166136261;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = text_code_at(s, n10.index);
      h = kern::to<Int>(kern::formula([](Float h, Float c) -> Float { return kern::math::bxor(h, c); }, kern::to<Float>(h), kern::to<Float>(c)));
      h = kern::to<Int>(kern::formula([](Float h) -> Float { return kern::math::mul32(h, 16777619.0); }, kern::to<Float>(h)));
    }
    return {h};
  }

  struct cell_randResult { Float Result = 0.0; };
  KERN_PURE cell_randResult cell_rand(Int seed = 0, Int i = 0) {
    Int x = 0;
    x = kern::to<Int>(kern::formula([](Float seed, Float i) -> Float { return kern::math::bxor(seed, kern::math::mul32(kern::math::wrap32(i + 1.0), 2654435761.0)); }, kern::to<Float>(seed), kern::to<Float>(i)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2146121005.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 15.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2221713035.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    return {kern::formula([](Float x) -> Float { return kern::math::div(x, 4294967296.0); }, kern::to<Float>(x))};
  }

  struct agent_randomResult { Float Result = 0.0; };
  /// pedestrian_agents.mjs agentRandom: a person's own number
  KERN_PURE agent_randomResult agent_random(Int seed = 0) {
    return {kern::formula([](Float seed) -> Float { return (kern::math::shr(kern::math::mul32(seed, 2246822519.0), 8.0) * kern::math::div(1.0, 16777216.0)); }, kern::to<Float>(seed))};
  }

  struct rounds_ofResult { combat_Rounds Result = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}; };
  KERN_PURE rounds_ofResult rounds_of(const combat_ZombieRounds& z) {
    return {struct_make<combat_Rounds>(z.active, z.the_round, z.to_spawn, z.points, blueprint_call::decimal(z.next_spawn).Result, blueprint_call::decimal(z.break_until).Result)};
  }

  void take(const combat_Rounds& r, combat_ZombieRounds& z) {
    z.active = r.active;
    z.the_round = r.the_round;
    z.to_spawn = r.to_spawn;
    z.points = r.points;
    z.next_spawn = r.next_spawn;
    z.break_until = r.break_until;
  }

  struct run_ofResult { combat_ChaosRun Result = combat_ChaosRun{0, 1, -1.0, 0.0}; };
  KERN_PURE run_ofResult run_of(const combat_Chaos& c) {
    return {struct_make<combat_ChaosRun>(c.score, c.combo, blueprint_call::decimal(c.combo_until).Result, blueprint_call::decimal(c.flow).Result)};
  }

  void take_2(const combat_ChaosRun& r, combat_Chaos& c) {
    c.score = r.score;
    c.combo = r.combo;
    c.combo_until = r.combo_until;
    c.flow = r.flow;
  }

  struct run_of_2Result { combat_FunRun Result = combat_FunRun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}; };
  KERN_PURE run_of_2Result run_of_2(const combat_Fun& s) {
    return {struct_make<combat_FunRun>(s.score, s.at_risk, s.high_score, s.stars, s.max_stars, s.combo, s.best_combo, s.momentum, blueprint_call::decimal(s.combo_expires).Result, blueprint_call::decimal(s.overdrive_until).Result, s.contract_index, s.contract_progress, s.contracts_completed, s.mercy, s.last_award, blueprint_call::decimal(s.run_damage).Result, s.contract_complete, s.last_label)};
  }

  void take_3(const combat_FunRun& r, combat_Fun& s) {
    s.score = r.score;
    s.at_risk = r.at_risk;
    s.high_score = r.high_score;
    s.stars = r.stars;
    s.max_stars = r.max_stars;
    s.combo = r.combo;
    s.best_combo = r.best_combo;
    s.momentum = r.momentum;
    s.combo_expires = r.combo_expires;
    s.overdrive_until = r.overdrive_until;
    s.contract_index = r.contract_index;
    s.contract_progress = r.contract_progress;
    s.contracts_completed = r.contracts_completed;
    s.mercy = r.mercy;
    s.last_award = r.last_award;
    s.run_damage = r.run_damage;
    s.contract_complete = r.contract_complete;
    s.last_label = r.last_label;
  }

  void Race_reset(combat_Race& self) {
    self = struct_make<combat_Race>(false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0});
  }

  struct race_timeResult { Text Result = ""; };
  /// (how a time is written: "01:15.500")
  KERN_PURE race_timeResult race_time(Float s = 0.0) {
    Int m = 0;
    Text b = "";
    if (logic_not(math_compare<">=">(s, 0.0))) {
      return {"-"};
    }
    m = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::trunc(kern::math::div(s, 60.0)); }, s));
    b = "";
    b = text_format<"%02d:%06.3f">(31, m, kern::formula([](Float s, Float m) -> Float { return (s - (m * 60.0)); }, s, kern::to<Float>(m)));
    return {b};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
