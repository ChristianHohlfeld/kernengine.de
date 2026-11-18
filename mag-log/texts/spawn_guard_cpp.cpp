struct spawn_guard_cpp : virtual kern::Library, virtual blueprint_call, virtual spawn_guard {
  // ---- types ----
  KERN_STRUCT(spawn_Looker) { Vector eye = Vector{0.0, 0.0, 0.0}; Vector forward = Vector{0.0, 0.0, 0.0}; Float half_angle = 0.0; Float far = 0.0; Bool local = false; };
  KERN_STRUCT(spawn_Print) { Vector center = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Vector half = Vector{0.0, 0.0, 0.0}; };

  // ---- functions ----
  struct looker_ofResult { spawn_Looker Result = spawn_Looker{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; };
  KERN_PURE looker_ofResult looker_of(const spawn_Viewer& v) {
    return {struct_make<spawn_Looker>(v.eye, v.forward, blueprint_call::decimal(v.half_angle).Result, blueprint_call::decimal(v.far).Result, v.local)};
  }

  struct print_ofResult { spawn_Print Result = spawn_Print{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}}; };
  KERN_PURE print_ofResult print_of(const spawn_Footprint& f) {
    return {struct_make<spawn_Print>(vector_make(vector_break(f.center).x, vector_break(f.center).y, 0.0), blueprint_call::decimal(f.yaw).Result, vector_make(vector_break(f.half).x, vector_break(f.half).y, 0.0))};
  }
};
