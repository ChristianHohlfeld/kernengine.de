struct spawn_guard_cpp : virtual kern::Library, virtual blueprint_call, virtual spawn_guard {
  // ---- types ----
  KERN_STRUCT(spawn_Looker) { Vector eye = Vector{0.0, 0.0, 0.0}; Vector forward = Vector{0.0, 0.0, 0.0}; Float half_angle = 0.0; Float far = 0.0; Bool local = false; };
  KERN_STRUCT(spawn_Print) { Vector center = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Vector half = Vector{0.0, 0.0, 0.0}; };

  // ---- functions ----
  struct SpawnGuard_ConeHalfAngleResult { Float half = 0.0; };
  /// (of the Blueprint of the actor SpawnGuard, called on it)
  KERN_EXTERN_AS("ConeHalfAngle") SpawnGuard_ConeHalfAngleResult SpawnGuard_ConeHalfAngle(Float fov = 0.0, Float aspect = 0.0);

  struct SpawnGuard_SeenByResult { Bool seen = false; };
  /// (of the Blueprint of the actor SpawnGuard, called on it)
  KERN_EXTERN_AS("SeenBy") SpawnGuard_SeenByResult SpawnGuard_SeenBy(spawn_Looker viewer = spawn_Looker{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}, Vector point = Vector{0.0, 0.0, 0.0}, Float farEntry = 0.0, Float height = 0.0);

  struct SpawnGuard_CanSpawnResult { Bool ok = false; };
  /// (of the Blueprint of the actor SpawnGuard, called on it)
  KERN_EXTERN_AS("CanSpawn") SpawnGuard_CanSpawnResult SpawnGuard_CanSpawn(List<spawn_Looker> viewers = List<spawn_Looker>{}, Vector point = Vector{0.0, 0.0, 0.0}, Float farEntry = 0.0, Float height = 0.0);

  struct SpawnGuard_FootprintsOverlapResult { Bool overlap = false; };
  /// (of the Blueprint of the actor SpawnGuard, called on it)
  KERN_EXTERN_AS("FootprintsOverlap") SpawnGuard_FootprintsOverlapResult SpawnGuard_FootprintsOverlap(spawn_Print a = spawn_Print{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}}, spawn_Print b = spawn_Print{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}}, Float margin = 0.0);

  struct looker_ofResult { spawn_Looker Result = spawn_Looker{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; };
  KERN_PURE looker_ofResult looker_of(const spawn_Viewer& v) {
    return {struct_make<spawn_Looker>(v.eye, v.forward, blueprint_call::decimal(v.half_angle).Result, blueprint_call::decimal(v.far).Result, v.local)};
  }

  struct print_ofResult { spawn_Print Result = spawn_Print{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}}; };
  KERN_PURE print_ofResult print_of(const spawn_Footprint& f) {
    return {struct_make<spawn_Print>(vector_make(vector_break(f.center).x, vector_break(f.center).y, 0.0), blueprint_call::decimal(f.yaw).Result, vector_make(vector_break(f.half).x, vector_break(f.half).y, 0.0))};
  }

  struct bindResult { Bool Result = false; };
  bindResult bind() {
    return {func_has_blueprint(actor_find("SpawnGuard"))};
  }

  struct default_farResult { Float Result = 0.0; };
  KERN_PURE default_farResult default_far() {
    return {func_get_on<"DefaultFar", Float>(actor_find("SpawnGuard"))};
  }

  struct cone_half_angleResult { Float Result = 0.0; };
  cone_half_angleResult cone_half_angle(Float vertical_fov = 0.0, Float aspect = 0.0) {
    func_call_on<"SpawnGuard_ConeHalfAngle">(actor_find("SpawnGuard"), kern::out(n10.half), blueprint_call::decimal(vertical_fov).Result, blueprint_call::decimal(aspect).Result);
    return {n10.half};
  }

  struct seen_byResult { Bool Result = false; };
  seen_byResult seen_by(const spawn_Viewer& v, Vector point = Vector{0.0, 0.0, 0.0}, Float far_entry = 0.0, Float height = 0.0) {
    func_call_on<"SpawnGuard_SeenBy">(actor_find("SpawnGuard"), kern::out(n11.seen), looker_of(v).Result, point, blueprint_call::decimal(far_entry).Result, blueprint_call::decimal(height).Result);
    return {n11.seen};
  }

  struct can_spawnResult { Bool Result = false; };
  can_spawnResult can_spawn(const List<spawn_Viewer>& viewers, Vector point = Vector{0.0, 0.0, 0.0}, Float far_entry = 0.0, Float height = 0.0) {
    List<spawn_Looker> lookers = List<spawn_Looker>{};
    lookers = List<spawn_Looker>{};
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<spawn_Viewer>(viewers))))); it12.next();) {
      list_add(lookers, looker_of(viewers[n12.index]).Result);
    }
    func_call_on<"SpawnGuard_CanSpawn">(actor_find("SpawnGuard"), kern::out(n13.ok), lookers, point, blueprint_call::decimal(far_entry).Result, blueprint_call::decimal(height).Result);
    return {n13.ok};
  }

  struct footprints_overlapResult { Bool Result = false; };
  footprints_overlapResult footprints_overlap(const spawn_Footprint& a, const spawn_Footprint& b, Float margin = 0.0) {
    func_call_on<"SpawnGuard_FootprintsOverlap">(actor_find("SpawnGuard"), kern::out(n14.overlap), print_of(a).Result, print_of(b).Result, blueprint_call::decimal(margin).Result);
    return {n14.overlap};
  }

  // ---- what nodes give and keep ----
  struct { Float half = 0.0; } n10;
  struct { Bool seen = false; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Bool ok = false; } n13;
  struct { Bool overlap = false; } n14;
};
