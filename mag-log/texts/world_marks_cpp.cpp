struct world_marks_cpp : virtual kern::Library, virtual common, virtual engine_math_math, virtual engine_physics_physics, virtual world_marks {
  // ---- types ----
  KERN_STRUCT(Marks_add_breach_C) { Int w = 0; Float u = 0.0; Float d = 0.0; };
  KERN_STRUCT(Marks_add_soot_C) { Int w = 0; Float u = 0.0; Float d = 0.0; };

  // ---- variables ----
  KERN_CONST Float kRubbleSeconds = 25.0;
  /// world_impacts.mjs / facade_marks.mjs
  KERN_CONST Int kRubble = 36;

  // ---- functions ----
  struct raggedResult { Float Result = 0.0; };
  /// The irregular soft edge of soot and scorch (facade_marks.mjs SOOT_FS edge).
  KERN_PURE raggedResult ragged(Float a = 0.0, Float seed = 0.0) {
    return {kern::formula([](Float a, Float seed) -> Float { return (((0.74 + (0.13 * kern::math::sin(((a * 5.0) + (seed * 7.0))))) + (0.08 * kern::math::sin(((a * 11.0) + (seed * 3.0))))) + (0.05 * kern::math::sin(((a * 23.0) + seed)))); }, a, seed)};
  }

  void Marks_ring_put_Scorch(List<Marks_Scorch>& v, Int& cursor, const Marks_Scorch& item, Int cap = 0) {
    if (math_compare<">">(kern::formula([](Float a, Float cap) -> Float { return ((a < 0.0) != (cap < 0.0) ? (cap < 0.0) : (a < cap)); }, kern::to<Float>(list_length<Marks_Scorch>(v)), kern::to<Float>(cap)), 0.5)) {
      list_add(v, item);
    } else {
      v[kern::to<Int>(kern::formula([](Float cursor, Float cap) -> Float { return kern::math::mod(cursor, cap); }, kern::to<Float>(cursor), kern::to<Float>(cap)))] = item;
    }
    cursor += 1;
  }

  void Marks_ring_put_WallMark(List<Marks_WallMark>& v, Int& cursor, const Marks_WallMark& item, Int cap = 0) {
    if (math_compare<">">(kern::formula([](Float a, Float cap) -> Float { return ((a < 0.0) != (cap < 0.0) ? (cap < 0.0) : (a < cap)); }, kern::to<Float>(list_length<Marks_WallMark>(v)), kern::to<Float>(cap)), 0.5)) {
      list_add(v, item);
    } else {
      v[kern::to<Int>(kern::formula([](Float cursor, Float cap) -> Float { return kern::math::mod(cursor, cap); }, kern::to<Float>(cursor), kern::to<Float>(cap)))] = item;
    }
    cursor += 1;
  }

  void Marks_clear(Marks& self, Int w = 0) {
    Int w_ = 0;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    w_ = w;
    if (math_compare<"!=">(kern::to<Float>(w_), 0.0)) {
      for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Marks_Chunk>(self.rubble_))))); it10.next();) {
        both = engine_physics_physics::Body_valid(self.rubble_[n10.index].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(w_, self.rubble_[n10.index].body);
          both = body_valid_2;
        }
        if (both) {
          engine_world_destroy_body(w_, self.rubble_[n10.index].body);
        }
      }
    }
    list_clear(self.rubble_);
    list_clear(self.scorches_);
    list_clear(self.soots_);
    list_clear(self.breaches_);
    list_clear(self.cracks_);
    self.crack_cursor_ = 0;
    self.rubble_cursor_ = 0;
    self.breach_cursor_ = self.rubble_cursor_;
    self.soot_cursor_ = self.breach_cursor_;
    self.scorch_cursor_ = self.soot_cursor_;
    self.dirty_ = true;
    self.stats_ = struct_make<Marks_Stats>(0, 0, 0, 0, 0);
  }

  struct add_soot_matchResult { Bool Result = false; };
  KERN_PURE add_soot_matchResult add_soot_match(const Marks_add_soot_C& a, const Marks_add_soot_C& b) {
    return {math_compare<"<">(a.d, b.d)};
  }

  struct add_breach_matchResult { Bool Result = false; };
  KERN_PURE add_breach_matchResult add_breach_match(const Marks_add_breach_C& a, const Marks_add_breach_C& b) {
    return {math_compare<"<">(a.d, b.d)};
  }

  void Marks_spawn_rubble(Marks& self, Rng& rng, Vector at = Vector{0.0, 0.0, 0.0}, Vector n = Vector{0.0, 0.0, 0.0}, Int count = 0, Int w = 0, Float now = 0.0) {
    Int w_ = 0;
    Vector half = Vector{0.0, 0.0, 0.0};
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    Float sp = 0.0;
    physics_Body b = physics_Body{Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    Marks_Chunk c = Marks_Chunk{physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, 0.0};
    Int oldest = 0;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    w_ = w;
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(count)))); it13.next();) {
      n14 = common::Rng_unit(rng);
      n15 = common::Rng_unit(rng);
      n16 = common::Rng_unit(rng);
      half = vector_make(kern::formula([](Float the_result) -> Float { return (0.06 + (the_result * 0.12)); }, n14.Result), kern::formula([](Float the_result) -> Float { return (0.06 + (the_result * 0.1)); }, n15.Result), kern::formula([](Float the_result) -> Float { return (0.05 + (the_result * 0.08)); }, n16.Result));
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs.type = physics_BodyType::dynamic;
      n17 = common::Rng_unit(rng);
      n18 = common::Rng_unit(rng);
      n19 = common::Rng_unit(rng);
      bs.position = vector_add(at, vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.8); }, n17.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.8); }, n18.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.8); }, n19.Result)));
      n20 = common::Rng_unit(rng);
      bs.rotation = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, kern::formula([](Float the_result) -> Float { return (the_result * 6.28); }, n20.Result)).Result;
      n21 = common::Rng_unit(rng);
      sp = kern::formula([](Float the_result) -> Float { return (3.0 + (the_result * 6.0)); }, n21.Result);
      n22 = common::Rng_unit(rng);
      n23 = common::Rng_unit(rng);
      n24 = common::Rng_unit(rng);
      bs.linear_velocity = vector_make(kern::formula([](Float x, Float sp, Float the_result) -> Float { return ((x * sp) + ((the_result - 0.5) * 3.0)); }, vector_break(n).x, sp, n22.Result), kern::formula([](Float y, Float sp, Float the_result) -> Float { return ((y * sp) + ((the_result - 0.5) * 3.0)); }, vector_break(n).y, sp, n23.Result), kern::formula([](Float the_result) -> Float { return (1.0 + (the_result * 4.0)); }, n24.Result));
      n25 = common::Rng_unit(rng);
      n26 = common::Rng_unit(rng);
      n27 = common::Rng_unit(rng);
      bs.angular_velocity = vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 12.0); }, n25.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 12.0); }, n26.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 12.0); }, n27.Result));
      bs.linear_damping = 0.05;
      bs.angular_damping = 0.2;
      engine_world_create_body(n28, w_, bs);
      create_body = n28.result;
      b = create_body;
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss.kind = physics_ShapeKind::box;
      ss.size = vector_scale(half, 2.0);
      ss.material.density = 2400.0;
      ss.material.friction = 0.8;
      ss.material.category_bits = Int64{0x2};
      engine_world_create_shape(n29, w_, b, ss);
      create_shape = n29.result;
      c = struct_make<Marks_Chunk>(b, half, kern::formula([](Float now, Float kRubbleSeconds) -> Float { return (now + kRubbleSeconds); }, now, kRubbleSeconds));
      if (math_compare<">">(kern::formula([](Float a, Float kRubble) -> Float { return ((a < 0.0) != (kRubble < 0.0) ? (kRubble < 0.0) : (a < kRubble)); }, kern::to<Float>(list_length<Marks_Chunk>(self.rubble_)), kern::to<Float>(kRubble)), 0.5)) {
        list_add(self.rubble_, c);
      } else {
        oldest = 0;
        for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Marks_Chunk>(self.rubble_))))); it30.next();) {
          if (math_compare<"<">(self.rubble_[n30.index].until, self.rubble_[oldest].until)) {
            oldest = n30.index;
          }
        }
        both = engine_physics_physics::Body_valid(self.rubble_[oldest].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(w_, self.rubble_[oldest].body);
          both = body_valid_2;
        }
        if (both) {
          engine_world_destroy_body(w_, self.rubble_[oldest].body);
        }
        self.rubble_[oldest] = c;
      }
    }
  }

  struct fhResult { Float Result = 0.0; };
  KERN_PURE fhResult fh(Float n = 0.0) {
    Float v = 0.0;
    v = kern::formula([](Float n) -> Float { return (kern::math::sin((n * 127.1)) * 43758.5453); }, n);
    return {kern::formula([](Float v) -> Float { return (v - kern::math::floor(v)); }, v)};
  }

  struct facade_glassResult { Bool Result = false; };
  /// facades.cpp FACADE_GLSL's window mask on the CPU (world_city_buildings.mjs facadeGlassAt): is this point of a wall glass? u along the wall, z above the base, H the wall's height.
  KERN_PURE facade_glassResult facade_glass(Float u = 0.0, Float len = 0.0, Float z = 0.0, Float H = 0.0, Int seed = 0) {
    Float sdq = 0.0;
    Float floor_h = 0.0;
    Bool pil = false;
    Float sp = 0.0;
    Float ns = 0.0;
    Float sm = 0.0;
    Float su = 0.0;
    Bool in_s = false;
    Float fz = 0.0;
    Float mu = 0.0;
    Float pitch = 0.0;
    Float n_win = 0.0;
    Float margin = 0.0;
    Float lu = 0.0;
    Float fu = 0.0;
    Float fz_2 = 0.0;
    Float ww = 0.0;
    Float wh = 0.0;
    if (logic_or(logic_or(math_compare<"<=">(len, 0.0), math_compare<"<">(z, 0.0)), math_compare<">">(z, H))) {
      return {false};
    }
    sdq = kern::formula([](Float seed) -> Float { return kern::math::div(kern::math::mod(seed, 997.0), 997.0); }, kern::to<Float>(seed));
    floor_h = kern::formula([](Float seed) -> Float { return (3.1 + (0.5 * kern::math::div(kern::math::mod(kern::math::shr(seed, 12.0), 1000.0), 1000.0))); }, kern::to<Float>(seed));
    pil = logic_not(logic_and(math_compare<">=">(u, 0.38), math_compare<"<=">(u, kern::formula([](Float len) -> Float { return (len - 0.38); }, len))));
    if (logic_and(math_compare<"<">(z, 4.0), math_compare<">">(len, 3.0))) {
      sp = 4.4;
      ns = kern::formula([](Float len, Float sp) -> Float { return kern::math::max(1.0, kern::math::floor(kern::math::div(len, sp))); }, len, sp);
      sm = kern::formula([](Float len, Float ns, Float sp) -> Float { return ((len - (ns * sp)) * 0.5); }, len, ns, sp);
      su = kern::formula([](Float u, Float sm, Float sp) -> Float { return (kern::math::div((u - sm), sp) - kern::math::floor(kern::math::div((u - sm), sp))); }, u, sm, sp);
      in_s = logic_and(math_compare<">=">(kern::formula([](Float u, Float sm) -> Float { return (u - sm); }, u, sm), 0.0), math_compare<"<=">(kern::formula([](Float u, Float sm) -> Float { return (u - sm); }, u, sm), kern::formula([](Float ns, Float sp) -> Float { return (ns * sp); }, ns, sp)));
      return {logic_and(logic_and(logic_and(logic_and(in_s, math_compare<">=">(su, 0.08)), math_compare<"<=">(su, 0.92)), math_compare<">=">(z, 0.35)), math_compare<"<=">(z, 3.0))};
    }
    if (math_compare<">=">(H, 26.0)) {
      fz = kern::formula([](Float z, Float floor_h) -> Float { return (kern::math::div((z - 4.0), floor_h) - kern::math::floor(kern::math::div((z - 4.0), floor_h))); }, z, floor_h);
      mu = kern::formula([](Float u) -> Float { return (kern::math::div(u, 1.6) - kern::math::floor(kern::math::div(u, 1.6))); }, u);
      return {logic_and(logic_and(math_compare<"<">(fz, 0.8), math_compare<"<">(mu, 0.95)), logic_not(pil))};
    }
    pitch = kern::formula([](Float the_result) -> Float { return (2.7 + ((3.7 - 2.7) * the_result)); }, fh(kern::formula([](Float sdq) -> Float { return (sdq * 7.0); }, sdq)).Result);
    n_win = kern::formula([](Float len, Float pitch) -> Float { return kern::math::max(1.0, kern::math::floor(kern::math::div((len - 0.8), pitch))); }, len, pitch);
    margin = kern::formula([](Float len, Float n_win, Float pitch) -> Float { return ((len - (n_win * pitch)) * 0.5); }, len, n_win, pitch);
    lu = kern::formula([](Float u, Float margin, Float pitch) -> Float { return kern::math::div((u - margin), pitch); }, u, margin, pitch);
    fu = kern::formula([](Float lu) -> Float { return (lu - kern::math::floor(lu)); }, lu);
    if (logic_or(logic_or(math_compare<"<">(lu, 0.0), math_compare<">">(lu, n_win)), math_compare<">">(z, kern::formula([](Float h) -> Float { return (h - 1.2); }, H)))) {
      return {false};
    }
    fz_2 = kern::formula([](Float z, Float floor_h) -> Float { return (kern::math::div((z - 4.0), floor_h) - kern::math::floor(kern::math::div((z - 4.0), floor_h))); }, z, floor_h);
    ww = kern::formula([](Float the_result) -> Float { return (0.38 + ((0.6 - 0.38) * the_result)); }, fh(kern::formula([](Float sdq) -> Float { return (sdq * 5.0); }, sdq)).Result);
    wh = kern::formula([](Float the_result) -> Float { return (0.42 + ((0.6 - 0.42) * the_result)); }, fh(kern::formula([](Float sdq) -> Float { return (sdq * 9.0); }, sdq)).Result);
    return {logic_and(logic_and(logic_and(math_compare<">=">(fu, kern::formula([](Float ww) -> Float { return (0.5 - (ww * 0.5)); }, ww)), math_compare<"<=">(fu, kern::formula([](Float ww) -> Float { return (0.5 + (ww * 0.5)); }, ww))), math_compare<">=">(fz_2, 0.3)), math_compare<"<=">(fz_2, kern::formula([](Float wh) -> Float { return (0.3 + wh); }, wh)))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<13> { Int index = 0; } n13;
  common::Rng_unitResult n14;
  common::Rng_unitResult n15;
  common::Rng_unitResult n16;
  common::Rng_unitResult n17;
  common::Rng_unitResult n18;
  common::Rng_unitResult n19;
  common::Rng_unitResult n20;
  common::Rng_unitResult n21;
  common::Rng_unitResult n22;
  common::Rng_unitResult n23;
  common::Rng_unitResult n24;
  common::Rng_unitResult n25;
  common::Rng_unitResult n26;
  common::Rng_unitResult n27;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n28;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
};
