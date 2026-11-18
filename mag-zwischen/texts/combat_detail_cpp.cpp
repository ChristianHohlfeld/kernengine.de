struct combat_detail_cpp : virtual kern::Library, virtual blueprint_call, virtual combat_detail {
  // ---- functions ----
  struct CombatDetail_IsCriticalResult { Bool critical = false; };
  /// (of the Blueprint of the actor CombatDetail, called on it)
  KERN_EXTERN_AS("IsCritical") CombatDetail_IsCriticalResult CombatDetail_IsCritical(detail_Critical kind = detail_Critical::car, Float hp = 0.0, Float maxHp = 0.0);

  struct CombatDetail_DetonationAtResult { Float at = 0.0; };
  /// (of the Blueprint of the actor CombatDetail, called on it)
  KERN_EXTERN_AS("DetonationAt") CombatDetail_DetonationAtResult CombatDetail_DetonationAt(detail_Critical kind = detail_Critical::car, Float now = 0.0);

  struct CombatDetail_AccelerateResult { Float at = 0.0; };
  /// (of the Blueprint of the actor CombatDetail, called on it)
  KERN_EXTERN_AS("Accelerate") CombatDetail_AccelerateResult CombatDetail_Accelerate(detail_Critical kind = detail_Critical::car, Float currentAt = 0.0, Float now = 0.0);

  struct CombatDetail_ClockUpdateResult { Bool fired = false; Float at = 0.0; Float lastHp = 0.0; };
  /// (of the Blueprint of the actor CombatDetail, called on it)
  KERN_EXTERN_AS("ClockUpdate") CombatDetail_ClockUpdateResult CombatDetail_ClockUpdate(detail_Critical kind = detail_Critical::car, Float hp = 0.0, Float maxHp = 0.0, Float now = 0.0, Float at = 0.0, Float lastHp = 0.0);

  struct CombatDetail_MomentumNsResult { Float ns = 0.0; };
  /// (of the Blueprint of the actor CombatDetail, called on it)
  KERN_EXTERN_AS("MomentumNs") CombatDetail_MomentumNsResult CombatDetail_MomentumNs(detail_Round round = detail_Round::nine_mm);

  struct CombatDetail_BulletImpulseResult { Vector impulse = Vector{0.0, 0.0, 0.0}; };
  /// (of the Blueprint of the actor CombatDetail, called on it)
  KERN_EXTERN_AS("BulletImpulse") CombatDetail_BulletImpulseResult CombatDetail_BulletImpulse(Vector direction = Vector{0.0, 0.0, 0.0}, detail_Round round = detail_Round::nine_mm);

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("CombatDetail"))};
  }

  struct is_criticalResult { Bool Result = false; };
  is_criticalResult is_critical(detail_Critical k = detail_Critical::car, Float hp = 0.0, Float max_hp = 0.0) {
    Bool critical = false;
    critical = false;
    func_call_on<"CombatDetail_IsCritical">(actor_find("CombatDetail"), kern::out(n10.critical), k, blueprint_call::decimal(hp).Result, blueprint_call::decimal(max_hp).Result);
    critical = n10.critical;
    return {critical};
  }

  struct detonation_atResult { Float Result = 0.0; };
  detonation_atResult detonation_at(detail_Critical k = detail_Critical::car, Float now = 0.0) {
    Float at = 0.0;
    at = 0.0;
    func_call_on<"CombatDetail_DetonationAt">(actor_find("CombatDetail"), kern::out(n11.at), k, blueprint_call::decimal(now).Result);
    at = n11.at;
    return {at};
  }

  struct accelerateResult { Float Result = 0.0; };
  accelerateResult accelerate(detail_Critical k = detail_Critical::car, Float current_at = 0.0, Float now = 0.0) {
    Float at = 0.0;
    at = 0.0;
    func_call_on<"CombatDetail_Accelerate">(actor_find("CombatDetail"), kern::out(n12.at), k, blueprint_call::decimal(current_at).Result, blueprint_call::decimal(now).Result);
    at = n12.at;
    return {at};
  }

  struct CriticalClock_updateResult { Bool Result = false; };
  CriticalClock_updateResult CriticalClock_update(detail_CriticalClock& self, detail_Critical k = detail_Critical::car, Float hp = 0.0, Float max_hp = 0.0, Float now = 0.0) {
    Bool fires = false;
    Float next_at = 0.0;
    Float next_hp = 0.0;
    fires = false;
    next_at = 0.0;
    next_hp = 0.0;
    func_call_on<"CombatDetail_ClockUpdate">(actor_find("CombatDetail"), kern::out(n13.fired, n13.at, n13.lastHp), k, blueprint_call::decimal(hp).Result, blueprint_call::decimal(max_hp).Result, blueprint_call::decimal(now).Result, blueprint_call::decimal(self.at).Result, blueprint_call::decimal(self.last_hp).Result);
    fires = n13.fired;
    next_at = n13.at;
    next_hp = n13.lastHp;
    self.at = next_at;
    self.last_hp = next_hp;
    return {fires};
  }

  struct momentum_nsResult { Float Result = 0.0; };
  momentum_nsResult momentum_ns(detail_Round r = detail_Round::nine_mm) {
    Float ns = 0.0;
    ns = 0.0;
    func_call_on<"CombatDetail_MomentumNs">(actor_find("CombatDetail"), kern::out(n14.ns), r);
    ns = n14.ns;
    return {ns};
  }

  struct bullet_impulseResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  bullet_impulseResult bullet_impulse(Vector direction = Vector{0.0, 0.0, 0.0}, detail_Round the_round = detail_Round::nine_mm) {
    Vector impulse = Vector{0.0, 0.0, 0.0};
    impulse = Vector{0.0, 0.0, 0.0};
    func_call_on<"CombatDetail_BulletImpulse">(actor_find("CombatDetail"), kern::out(n15.impulse), direction, the_round);
    impulse = n15.impulse;
    return {impulse};
  }

  struct projectile_gravityResult { Float Result = 0.0; };
  KERN_PURE projectile_gravityResult projectile_gravity() {
    return {func_get_on<"ProjectileGravity", Float>(actor_find("CombatDetail"))};
  }

  // ---- what nodes give and keep ----
  struct { Bool critical = false; } n10;
  struct { Float at = 0.0; } n11;
  struct { Float at = 0.0; } n12;
  struct { Bool fired = false; Float at = 0.0; Float lastHp = 0.0; } n13;
  struct { Float ns = 0.0; } n14;
  struct { Vector impulse = Vector{0.0, 0.0, 0.0}; } n15;
};
