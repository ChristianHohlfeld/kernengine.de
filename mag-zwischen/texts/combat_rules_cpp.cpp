struct combat_rules_cpp : virtual kern::Library, virtual blueprint_call, virtual combat_rules {
  // ---- types ----
  KERN_STRUCT(combat_ChaosRun) { I32 score = 0; I32 combo = 1; Float combo_until = -1.0; Float flow = 0.0; };
  KERN_STRUCT(combat_Clock) { Bool active = false; Float since5 = -1.0; Float alert_at = -1.0; Float next_reinforce = 0.0; };
  KERN_STRUCT(combat_ContractRow) { Text id = ""; Text name = ""; Text hint = ""; I32 target = 0; I32 reward = 0; };
  KERN_STRUCT(combat_EnemyRow) { Text name = ""; Float hp = 0.0; Float speed = 0.0; Float range = 0.0; Float shot_s = 0.0; I32 burst = 0; Float dmg = 0.0; Float acc = 0.0; I32 score = 0; Float scale = 0.0; Vector color = Vector{0.0, 0.0, 0.0}; Vector stripe = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(combat_FunRun) { I32 score = 0; I32 at_risk = 0; I32 high_score = 0; I32 stars = 0; I32 max_stars = 0; I32 combo = 0; I32 best_combo = 0; I32 momentum = 0; Float combo_expires = -1000000000.0; Float overdrive_until = -1000000000.0; I32 contract_index = 0; I32 contract_progress = 0; I32 contracts_completed = 0; I32 mercy = 0; I32 last_award = 0; Float run_damage = 0.0; Bool contract_complete = false; Text last_label = "READY"; };
  KERN_STRUCT(combat_GateRow) { Text name = ""; Vector center = Vector{0.0, 0.0, 0.0}; Float width = 0.0; Float height = 0.0; Bool start = false; };
  KERN_STRUCT(combat_RaceRun) { Bool started = false; Bool finished = false; I32 lap = 0; I32 next = 0; Float race_start = 0.0; Float lap_start = 0.0; Float finish_time = -1.0; Float last_lap = -1.0; Float best_lap = -1.0; Float last_pass = -99.0; Bool have_prev = false; Vector prev = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(combat_Rounds) { Bool active = false; I32 the_round = 0; I32 to_spawn = 0; I32 points = 0; Float next_spawn = 0.0; Float break_until = -1.0; };

  // ---- functions ----
  struct Military_UpdateResult { combat_Clock clock = combat_Clock{false, -1.0, -1.0, 0.0}; combat_MilEvent event = combat_MilEvent::none; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("Update") Military_UpdateResult Military_Update(combat_Clock clock = combat_Clock{false, -1.0, -1.0, 0.0}, Float now = 0.0, Int stars_2 = 0, Float heat_2 = 0.0, Bool enabled = false);

  struct Military_HeliHitChanceResult { Float chance = 0.0; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("HeliHitChance") Military_HeliHitChanceResult Military_HeliHitChance(Float d = 0.0, Float speed_2 = 0.0);

  struct Military_SoldierHitChanceResult { Float chance = 0.0; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("SoldierHitChance") Military_SoldierHitChanceResult Military_SoldierHitChance(Float d = 0.0, Float speed_2 = 0.0);

  struct Military_TankAimErrorResult { Float metres = 0.0; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("TankAimError") Military_TankAimErrorResult Military_TankAimError(Float d = 0.0, Float speed_2 = 0.0);

  struct Military_GangsterHitChanceResult { Float chance = 0.0; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("GangsterHitChance") Military_GangsterHitChanceResult Military_GangsterHitChance(Float d = 0.0, Float speed_2 = 0.0);

  struct Military_StrikeLaunchTimeResult { Float time = 0.0; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("StrikeLaunchTime") Military_StrikeLaunchTimeResult Military_StrikeLaunchTime(Float now = 0.0);

  struct Military_FlakBurstDamageResult { Float damage = 0.0; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("FlakBurstDamage") Military_FlakBurstDamageResult Military_FlakBurstDamage(Float d = 0.0);

  struct Military_FlakHasGunResult { Bool gun = false; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("FlakHasGun") Military_FlakHasGunResult Military_FlakHasGun(Float roll = 0.0);

  struct Military_TemperOfResult { combat_Temper temper = combat_Temper::fearful; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("TemperOf") Military_TemperOfResult Military_TemperOf(Float r = 0.0);

  struct Military_ArmsOfResult { combat_Arms arms = combat_Arms::fists; };
  /// (of the Blueprint of the actor Military, called on it)
  KERN_EXTERN_AS("ArmsOf") Military_ArmsOfResult Military_ArmsOf(combat_Temper temper = combat_Temper::fearful, Float r = 0.0);

  struct Zombies_UpdateResult { combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}; combat_ZombieEvent event = combat_ZombieEvent::none; };
  /// (of the Blueprint of the actor Zombies, called on it)
  KERN_EXTERN_AS("Update") Zombies_UpdateResult Zombies_Update(combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}, Float now = 0.0, Bool night = false, Int alive_2 = 0);

  struct Zombies_SpawnedResult { combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}; };
  /// (of the Blueprint of the actor Zombies, called on it)
  KERN_EXTERN_AS("Spawned") Zombies_SpawnedResult Zombies_Spawned(combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}, Float now = 0.0);

  struct Zombies_NextRoundResult { combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}; };
  /// (of the Blueprint of the actor Zombies, called on it)
  KERN_EXTERN_AS("NextRound") Zombies_NextRoundResult Zombies_NextRound(combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}, Float now = 0.0);

  struct Zombies_HpResult { Float hp = 0.0; };
  /// (of the Blueprint of the actor Zombies, called on it)
  KERN_EXTERN_AS("Hp") Zombies_HpResult Zombies_Hp(Int round = 0);

  struct Zombies_SpeedResult { Float mps = 0.0; };
  /// (of the Blueprint of the actor Zombies, called on it)
  KERN_EXTERN_AS("Speed") Zombies_SpeedResult Zombies_Speed(Int round = 0, Float rFast = 0.0, Float rSpeed = 0.0);

  struct Sandbox_ChooseEnemyResult { combat_EnemyType type = combat_EnemyType::cop; };
  /// (of the Blueprint of the actor Sandbox, called on it)
  KERN_EXTERN_AS("ChooseEnemy") Sandbox_ChooseEnemyResult Sandbox_ChooseEnemy(Int stars_2 = 0, Float r = 0.0, Int heavies = 0);

  struct Sandbox_EnemyHitChanceResult { Float chance = 0.0; };
  /// (of the Blueprint of the actor Sandbox, called on it)
  KERN_EXTERN_AS("EnemyHitChance") Sandbox_EnemyHitChanceResult Sandbox_EnemyHitChance(Float acc = 0.0, Float d = 0.0, Float moving = 0.0);

  struct Sandbox_BudgetResult { Int enemies = 0; };
  /// (of the Blueprint of the actor Sandbox, called on it)
  KERN_EXTERN_AS("Budget") Sandbox_BudgetResult Sandbox_Budget(Int stars_2 = 0);

  struct Sandbox_AwardResult { combat_ChaosRun chaos = combat_ChaosRun{0, 1, -1.0, 0.0}; Int gained = 0; };
  /// (of the Blueprint of the actor Sandbox, called on it)
  KERN_EXTERN_AS("Award") Sandbox_AwardResult Sandbox_Award(combat_ChaosRun chaos = combat_ChaosRun{0, 1, -1.0, 0.0}, Int points = 0, Float now = 0.0);

  struct Sandbox_TickResult { combat_ChaosRun chaos = combat_ChaosRun{0, 1, -1.0, 0.0}; };
  /// (of the Blueprint of the actor Sandbox, called on it)
  KERN_EXTERN_AS("Tick") Sandbox_TickResult Sandbox_Tick(combat_ChaosRun chaos = combat_ChaosRun{0, 1, -1.0, 0.0}, Float dt = 0.0, Float now = 0.0);

  struct Fun_ApplyResult { combat_FunRun run = combat_FunRun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}; Text line = ""; };
  /// (of the Blueprint of the actor Fun, called on it)
  KERN_EXTERN_AS("Apply") Fun_ApplyResult Fun_Apply(combat_FunRun run = combat_FunRun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, combat_FunEvent event = combat_FunEvent::tick, Float now = 0.0, Int stars_2 = 0, Float amount = 0.0);

  struct Fun_MultiplierResult { Float times = 0.0; };
  /// (of the Blueprint of the actor Fun, called on it)
  KERN_EXTERN_AS("Multiplier") Fun_MultiplierResult Fun_Multiplier(combat_FunRun run = combat_FunRun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, Float now = 0.0);

  struct Fun_PoliceDamageScaleResult { Float scale = 0.0; };
  /// (of the Blueprint of the actor Fun, called on it)
  KERN_EXTERN_AS("PoliceDamageScale") Fun_PoliceDamageScaleResult Fun_PoliceDamageScale(Int mercy = 0);

  struct Race_GateAxesResult { Vector normal = Vector{0.0, 0.0, 0.0}; Vector right = Vector{0.0, 0.0, 0.0}; };
  /// (of the Blueprint of the actor Race, called on it)
  KERN_EXTERN_AS("GateAxes") Race_GateAxesResult Race_GateAxes(Int index = 0);

  struct Race_CrossedResult { Bool crossed = false; };
  /// (of the Blueprint of the actor Race, called on it)
  KERN_EXTERN_AS("Crossed") Race_CrossedResult Race_Crossed(Int index = 0, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0});

  struct Race_UpdateResult { combat_RaceRun run = combat_RaceRun{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}; Int passed = 0; };
  /// (of the Blueprint of the actor Race, called on it)
  KERN_EXTERN_AS("Update") Race_UpdateResult Race_Update(combat_RaceRun run = combat_RaceRun{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, Vector p = Vector{0.0, 0.0, 0.0}, Float t = 0.0);

  struct MilitaryClock_updateResult { combat_MilEvent Result = combat_MilEvent::none; };
  MilitaryClock_updateResult MilitaryClock_update(combat_MilitaryClock& self, Float now = 0.0, I32 stars = 0, Float heat = 0.0, Bool enabled = false) {
    combat_Clock clock = combat_Clock{false, -1.0, -1.0, 0.0};
    combat_MilEvent event = combat_MilEvent::none;
    clock = combat_Clock{false, -1.0, -1.0, 0.0};
    event = combat_MilEvent::none;
    func_call_on<"Military_Update">(actor_find("Military"), kern::out(n10.clock, n10.event), struct_make<combat_Clock>(self.active, blueprint_call::decimal(self.since5).Result, blueprint_call::decimal(self.alert_at).Result, blueprint_call::decimal(self.next_reinforce).Result), blueprint_call::decimal(now).Result, stars, blueprint_call::decimal(heat).Result, enabled);
    clock = n10.clock;
    event = n10.event;
    self.active = clock.active;
    self.since5 = clock.since5;
    self.alert_at = clock.alert_at;
    self.next_reinforce = clock.next_reinforce;
    return {event};
  }

  struct heli_hit_chanceResult { Float Result = 0.0; };
  heli_hit_chanceResult heli_hit_chance(Float d = 0.0, Float speed = 0.0) {
    Float chance = 0.0;
    chance = 0.0;
    func_call_on<"Military_HeliHitChance">(actor_find("Military"), kern::out(n11.chance), blueprint_call::decimal(d).Result, blueprint_call::decimal(speed).Result);
    chance = n11.chance;
    return {chance};
  }

  struct soldier_hit_chanceResult { Float Result = 0.0; };
  soldier_hit_chanceResult soldier_hit_chance(Float d = 0.0, Float speed = 0.0) {
    Float chance = 0.0;
    chance = 0.0;
    func_call_on<"Military_SoldierHitChance">(actor_find("Military"), kern::out(n12.chance), blueprint_call::decimal(d).Result, blueprint_call::decimal(speed).Result);
    chance = n12.chance;
    return {chance};
  }

  struct tank_aim_errorResult { Float Result = 0.0; };
  tank_aim_errorResult tank_aim_error(Float d = 0.0, Float speed = 0.0) {
    Float metres = 0.0;
    metres = 0.0;
    func_call_on<"Military_TankAimError">(actor_find("Military"), kern::out(n13.metres), blueprint_call::decimal(d).Result, blueprint_call::decimal(speed).Result);
    metres = n13.metres;
    return {metres};
  }

  struct gangster_hit_chanceResult { Float Result = 0.0; };
  gangster_hit_chanceResult gangster_hit_chance(Float d = 0.0, Float speed = 0.0) {
    Float chance = 0.0;
    chance = 0.0;
    func_call_on<"Military_GangsterHitChance">(actor_find("Military"), kern::out(n14.chance), blueprint_call::decimal(d).Result, blueprint_call::decimal(speed).Result);
    chance = n14.chance;
    return {chance};
  }

  struct strike_launch_timeResult { Float Result = 0.0; };
  strike_launch_timeResult strike_launch_time(Float now = 0.0) {
    Float time = 0.0;
    time = 0.0;
    func_call_on<"Military_StrikeLaunchTime">(actor_find("Military"), kern::out(n15.time), blueprint_call::decimal(now).Result);
    time = n15.time;
    return {time};
  }

  struct flak_burst_damageResult { Float Result = 0.0; };
  flak_burst_damageResult flak_burst_damage(Float d = 0.0) {
    Float damage = 0.0;
    damage = 0.0;
    func_call_on<"Military_FlakBurstDamage">(actor_find("Military"), kern::out(n16.damage), blueprint_call::decimal(d).Result);
    damage = n16.damage;
    return {damage};
  }

  struct hash_textResult { U32 Result = 0; };
  /// flak_battery.mjs hash() and rand(): a cell's own numbers
  KERN_PURE hash_textResult hash_text(const Text& s) {
    U32 h = 0;
    Int c = 0;
    h = 2166136261;
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it17.next();) {
      c = text_code_at(s, n17.index);
      h = kern::to<Int>(kern::formula([](Float h, Float c) -> Float { return kern::math::bxor(h, c); }, kern::to<Float>(h), kern::to<Float>(c)));
      h = kern::to<Int>(kern::formula([](Float h) -> Float { return kern::math::mul32(h, 16777619.0); }, kern::to<Float>(h)));
    }
    return {h};
  }

  struct cell_randResult { Float Result = 0.0; };
  KERN_PURE cell_randResult cell_rand(U32 seed = 0, U32 i = 0) {
    U32 x = 0;
    x = kern::to<Int>(kern::formula([](Float seed, Float i) -> Float { return kern::math::bxor(seed, kern::math::mul32(kern::math::wrap32(i + 1.0), 2654435761.0)); }, kern::to<Float>(seed), kern::to<Float>(i)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2146121005.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 15.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2221713035.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    return {kern::formula([](Float x) -> Float { return kern::math::div(x, 4294967296.0); }, kern::to<Float>(x))};
  }

  struct flak_cellResult { Bool Result = false; };
  flak_cellResult flak_cell(U32& seed, Int64 cx = Int64{0x0}, Int64 cy = Int64{0x0}) {
    Text key = "";
    Text a = "";
    Bool gun = false;
    key = "";
    key = text_format<"flak:l%s:%s">(63, int64_text_signed(cx), int64_text_signed(cy));
    a = key;
    seed = hash_text(a).Result;
    gun = false;
    func_call_on<"Military_FlakHasGun">(actor_find("Military"), kern::out(n18.gun), blueprint_call::decimal(cell_rand(seed, 0).Result).Result);
    gun = n18.gun;
    return {gun};
  }

  struct agent_randomResult { Float Result = 0.0; };
  /// pedestrian_agents.mjs agentRandom: a person's own number
  KERN_PURE agent_randomResult agent_random(U32 seed = 0) {
    return {kern::formula([](Float seed) -> Float { return (kern::math::shr(kern::math::mul32(seed, 2246822519.0), 8.0) * kern::math::div(1.0, 16777216.0)); }, kern::to<Float>(seed))};
  }

  struct temper_ofResult { combat_Temper Result = combat_Temper::fearful; };
  temper_ofResult temper_of(U32 seed = 0) {
    combat_Temper temper = combat_Temper::fearful;
    temper = combat_Temper::fearful;
    func_call_on<"Military_TemperOf">(actor_find("Military"), kern::out(n19.temper), blueprint_call::decimal(agent_random(seed).Result).Result);
    temper = n19.temper;
    return {temper};
  }

  struct arms_ofResult { combat_Arms Result = combat_Arms::fists; };
  arms_ofResult arms_of(U32 seed = 0) {
    U32 h = 0;
    combat_Arms arms = combat_Arms::fists;
    h = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::mul32(kern::math::bxor(kern::math::trunc((the_result * 4294967296.0)), 2654435769.0), 2654435761.0); }, agent_random(seed).Result));
    h = kern::to<Int>(kern::formula([](Float h) -> Float { return kern::math::bxor(h, kern::math::shr(h, 15.0)); }, kern::to<Float>(h)));
    arms = combat_Arms::fists;
    n20 = temper_of(seed);
    func_call_on<"Military_ArmsOf">(actor_find("Military"), kern::out(n21.arms), n20.Result, kern::formula([](Float h) -> Float { return kern::math::div(kern::math::mod(h, 10000.0), 10000.0); }, kern::to<Float>(h)));
    arms = n21.arms;
    return {arms};
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

  struct ZombieRounds_updateResult { combat_ZombieEvent Result = combat_ZombieEvent::none; };
  ZombieRounds_updateResult ZombieRounds_update(combat_ZombieRounds& self, Float now = 0.0, Bool night = false, I32 alive = 0) {
    combat_Rounds r = combat_Rounds{false, 0, 0, 0, 0.0, -1.0};
    combat_ZombieEvent event = combat_ZombieEvent::none;
    r = combat_Rounds{false, 0, 0, 0, 0.0, -1.0};
    event = combat_ZombieEvent::none;
    func_call_on<"Zombies_Update">(actor_find("Zombies"), kern::out(n22.rounds, n22.event), rounds_of(self).Result, blueprint_call::decimal(now).Result, night, alive);
    r = n22.rounds;
    event = n22.event;
    take(r, self);
    return {event};
  }

  void ZombieRounds_spawned(combat_ZombieRounds& self, Float now = 0.0) {
    combat_Rounds r = combat_Rounds{false, 0, 0, 0, 0.0, -1.0};
    r = combat_Rounds{false, 0, 0, 0, 0.0, -1.0};
    func_call_on<"Zombies_Spawned">(actor_find("Zombies"), kern::out(n23.rounds), rounds_of(self).Result, blueprint_call::decimal(now).Result);
    r = n23.rounds;
    take(r, self);
  }

  void ZombieRounds_next_round(combat_ZombieRounds& self, Float now = 0.0) {
    combat_Rounds r = combat_Rounds{false, 0, 0, 0, 0.0, -1.0};
    r = combat_Rounds{false, 0, 0, 0, 0.0, -1.0};
    func_call_on<"Zombies_NextRound">(actor_find("Zombies"), kern::out(n24.rounds), rounds_of(self).Result, blueprint_call::decimal(now).Result);
    r = n24.rounds;
    take(r, self);
  }

  struct ZombieRounds_hpResult { Float Result = 0.0; };
  ZombieRounds_hpResult ZombieRounds_hp(const combat_ZombieRounds& self) {
    Float hp = 0.0;
    hp = 0.0;
    func_call_on<"Zombies_Hp">(actor_find("Zombies"), kern::out(n25.hp), self.the_round);
    hp = n25.hp;
    return {hp};
  }

  struct ZombieRounds_speedResult { Float Result = 0.0; };
  ZombieRounds_speedResult ZombieRounds_speed(const combat_ZombieRounds& self, Float r_fast = 0.0, Float r_speed = 0.0) {
    Float mps = 0.0;
    mps = 0.0;
    func_call_on<"Zombies_Speed">(actor_find("Zombies"), kern::out(n26.mps), self.the_round, blueprint_call::decimal(r_fast).Result, blueprint_call::decimal(r_speed).Result);
    mps = n26.mps;
    return {mps};
  }

  struct choose_enemyResult { combat_EnemyType Result = combat_EnemyType::cop; };
  choose_enemyResult choose_enemy(I32 stars = 0, Float r = 0.0, I32 heavies = 0) {
    combat_EnemyType type = combat_EnemyType::cop;
    type = combat_EnemyType::cop;
    func_call_on<"Sandbox_ChooseEnemy">(actor_find("Sandbox"), kern::out(n27.type), stars, blueprint_call::decimal(r).Result, heavies);
    type = n27.type;
    return {type};
  }

  struct enemy_hit_chanceResult { Float Result = 0.0; };
  enemy_hit_chanceResult enemy_hit_chance(const combat_EnemySpec& s, Float d = 0.0, Float moving = 0.0) {
    Float chance = 0.0;
    chance = 0.0;
    func_call_on<"Sandbox_EnemyHitChance">(actor_find("Sandbox"), kern::out(n28.chance), blueprint_call::decimal(s.acc).Result, blueprint_call::decimal(d).Result, blueprint_call::decimal(moving).Result);
    chance = n28.chance;
    return {chance};
  }

  struct enemy_budgetResult { I32 Result = 0; };
  enemy_budgetResult enemy_budget(I32 stars = 0) {
    I32 enemies = 0;
    enemies = 0;
    func_call_on<"Sandbox_Budget">(actor_find("Sandbox"), kern::out(n29.enemies), stars);
    enemies = n29.enemies;
    return {enemies};
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

  struct Chaos_awardResult { I32 Result = 0; };
  Chaos_awardResult Chaos_award(combat_Chaos& self, I32 points = 0, Float now = 0.0) {
    combat_ChaosRun r = combat_ChaosRun{0, 1, -1.0, 0.0};
    I32 gained = 0;
    r = combat_ChaosRun{0, 1, -1.0, 0.0};
    gained = 0;
    func_call_on<"Sandbox_Award">(actor_find("Sandbox"), kern::out(n30.chaos, n30.gained), run_of(self).Result, points, blueprint_call::decimal(now).Result);
    r = n30.chaos;
    gained = n30.gained;
    take_2(r, self);
    return {gained};
  }

  void Chaos_tick(combat_Chaos& self, Float dt = 0.0, Float now = 0.0) {
    combat_ChaosRun r = combat_ChaosRun{0, 1, -1.0, 0.0};
    r = combat_ChaosRun{0, 1, -1.0, 0.0};
    func_call_on<"Sandbox_Tick">(actor_find("Sandbox"), kern::out(n31.chaos), run_of(self).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(now).Result);
    r = n31.chaos;
    take_2(r, self);
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

  struct Fun_applyResult { Text Result = ""; };
  Fun_applyResult Fun_apply(combat_Fun& self, combat_FunEvent e = combat_FunEvent::tick, Float now = 0.0, I32 stars_now = 0, Float amount = 0.0) {
    combat_FunRun r = combat_FunRun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"};
    Text line = "";
    r = combat_FunRun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"};
    line = "";
    func_call_on<"Fun_Apply">(actor_find("Fun"), kern::out(n32.run, n32.line), run_of_2(self).Result, e, blueprint_call::decimal(now).Result, stars_now, blueprint_call::decimal(amount).Result);
    r = n32.run;
    line = n32.line;
    take_3(r, self);
    return {line};
  }

  struct Fun_multiplierResult { Float Result = 0.0; };
  Fun_multiplierResult Fun_multiplier(const combat_Fun& self, Float now = 0.0) {
    func_call_on<"Fun_Multiplier">(actor_find("Fun"), kern::out(n33.times), run_of_2(self).Result, blueprint_call::decimal(now).Result);
    return {n33.times};
  }

  struct Fun_police_damage_scaleResult { Float Result = 0.0; };
  Fun_police_damage_scaleResult Fun_police_damage_scale(const combat_Fun& self) {
    Float scale = 0.0;
    scale = 0.0;
    func_call_on<"Fun_PoliceDamageScale">(actor_find("Fun"), kern::out(n34.scale), self.mercy);
    scale = n34.scale;
    return {scale};
  }

  void gate_axes(Vector& normal, Vector& right, I32 i = 0) {
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    n = Vector{0.0, 0.0, 0.0};
    r = Vector{0.0, 0.0, 0.0};
    func_call_on<"Race_GateAxes">(actor_find("Race"), kern::out(n35.normal, n35.right), i);
    n = n35.normal;
    r = n35.right;
    normal = vector_make(vector_break(n).x, vector_break(n).y, 0.0);
    right = vector_make(vector_break(r).x, vector_break(r).y, 0.0);
  }

  void Race_reset(combat_Race& self) {
    self = struct_make<combat_Race>(false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0});
  }

  struct Race_crossedResult { Bool Result = false; };
  Race_crossedResult Race_crossed(I32 gi = 0, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    Bool crossed = false;
    crossed = false;
    func_call_on<"Race_Crossed">(actor_find("Race"), kern::out(n36.crossed), gi, a, b);
    crossed = n36.crossed;
    return {crossed};
  }

  struct Race_updateResult { I32 Result = 0; };
  Race_updateResult Race_update(combat_Race& self, Vector p = Vector{0.0, 0.0, 0.0}, Float t = 0.0) {
    combat_RaceRun r = combat_RaceRun{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}};
    I32 passed = 0;
    r = combat_RaceRun{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}};
    passed = 0;
    func_call_on<"Race_Update">(actor_find("Race"), kern::out(n37.run, n37.passed), struct_make<combat_RaceRun>(self.started, self.finished, self.lap, self.next, blueprint_call::decimal(self.race_start).Result, blueprint_call::decimal(self.lap_start).Result, blueprint_call::decimal(self.finish_time).Result, blueprint_call::decimal(self.last_lap).Result, blueprint_call::decimal(self.best_lap).Result, blueprint_call::decimal(self.last_pass).Result, self.have_prev, self.prev), p, blueprint_call::decimal(t).Result);
    r = n37.run;
    passed = n37.passed;
    self.started = r.started;
    self.finished = r.finished;
    self.lap = r.lap;
    self.next = r.next;
    self.race_start = r.race_start;
    self.lap_start = r.lap_start;
    self.finish_time = r.finish_time;
    self.last_lap = r.last_lap;
    self.best_lap = r.best_lap;
    self.last_pass = r.last_pass;
    self.have_prev = r.have_prev;
    self.prev = r.prev;
    return {passed};
  }

  struct race_timeResult { Text Result = ""; };
  /// (how a time is written: "01:15.500")
  KERN_PURE race_timeResult race_time(Float s = 0.0) {
    I32 m = 0;
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
  struct { combat_Clock clock = combat_Clock{false, -1.0, -1.0, 0.0}; combat_MilEvent event = combat_MilEvent::none; } n10;
  struct { Float chance = 0.0; } n11;
  struct { Float chance = 0.0; } n12;
  struct { Float metres = 0.0; } n13;
  struct { Float chance = 0.0; } n14;
  struct { Float time = 0.0; } n15;
  struct { Float damage = 0.0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct { Bool gun = false; } n18;
  struct { combat_Temper temper = combat_Temper::fearful; } n19;
  temper_ofResult n20;
  struct { combat_Arms arms = combat_Arms::fists; } n21;
  struct { combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}; combat_ZombieEvent event = combat_ZombieEvent::none; } n22;
  struct { combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}; } n23;
  struct { combat_Rounds rounds = combat_Rounds{false, 0, 0, 0, 0.0, -1.0}; } n24;
  struct { Float hp = 0.0; } n25;
  struct { Float mps = 0.0; } n26;
  struct { combat_EnemyType type = combat_EnemyType::cop; } n27;
  struct { Float chance = 0.0; } n28;
  struct { Int enemies = 0; } n29;
  struct { combat_ChaosRun chaos = combat_ChaosRun{0, 1, -1.0, 0.0}; Int gained = 0; } n30;
  struct { combat_ChaosRun chaos = combat_ChaosRun{0, 1, -1.0, 0.0}; } n31;
  struct { combat_FunRun run = combat_FunRun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}; Text line = ""; } n32;
  struct { Float times = 0.0; } n33;
  struct { Float scale = 0.0; } n34;
  struct { Vector normal = Vector{0.0, 0.0, 0.0}; Vector right = Vector{0.0, 0.0, 0.0}; } n35;
  struct { Bool crossed = false; } n36;
  struct { combat_RaceRun run = combat_RaceRun{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}; Int passed = 0; } n37;
};
