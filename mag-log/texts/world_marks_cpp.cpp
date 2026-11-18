struct world_marks_cpp : virtual kern::Library, virtual city_stream_cpp, virtual common, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual terrain_cpp, virtual world_art_cpp, virtual world_marks {
  // ---- types ----
  KERN_STRUCT(Marks_add_breach_C) { Int w = 0; Float u = 0.0; Float d = 0.0; };
  KERN_STRUCT(Marks_add_soot_C) { Int w = 0; Float u = 0.0; Float d = 0.0; };

  // ---- variables ----
  KERN_CONST Float kRubbleSeconds = 25.0;
  /// world_impacts.mjs / facade_marks.mjs
  KERN_CONST Int kRubble = 36;
  /// world_impacts.mjs / facade_marks.mjs
  KERN_CONST Int kCracks = 160;
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Vector kRubbleColor = Vector{0.5411764705882353, 0.5215686274509804, 0.48627450980392156};

  // ---- functions ----
  struct raggedResult { Float Result = 0.0; };
  /// The irregular soft edge of soot and scorch (facade_marks.mjs SOOT_FS edge).
  KERN_PURE raggedResult ragged(Float a = 0.0, Float seed = 0.0) {
    return {kern::formula([](Float a, Float seed) -> Float { return (((0.74 + (0.13 * kern::math::sin(((a * 5.0) + (seed * 7.0))))) + (0.08 * kern::math::sin(((a * 11.0) + (seed * 3.0))))) + (0.05 * kern::math::sin(((a * 23.0) + seed)))); }, a, seed)};
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
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(count)))); it15.next();) {
      n16 = common::Rng_unit(rng);
      n17 = common::Rng_unit(rng);
      n18 = common::Rng_unit(rng);
      half = vector_make(kern::formula([](Float the_result) -> Float { return (0.06 + (the_result * 0.12)); }, n16.Result), kern::formula([](Float the_result) -> Float { return (0.06 + (the_result * 0.1)); }, n17.Result), kern::formula([](Float the_result) -> Float { return (0.05 + (the_result * 0.08)); }, n18.Result));
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs.type = physics_BodyType::dynamic;
      n19 = common::Rng_unit(rng);
      n20 = common::Rng_unit(rng);
      n21 = common::Rng_unit(rng);
      bs.position = vector_add(at, vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.8); }, n19.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.8); }, n20.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.8); }, n21.Result)));
      n22 = common::Rng_unit(rng);
      bs.rotation = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, kern::formula([](Float the_result) -> Float { return (the_result * 6.28); }, n22.Result)).Result;
      n23 = common::Rng_unit(rng);
      sp = kern::formula([](Float the_result) -> Float { return (3.0 + (the_result * 6.0)); }, n23.Result);
      n24 = common::Rng_unit(rng);
      n25 = common::Rng_unit(rng);
      n26 = common::Rng_unit(rng);
      bs.linear_velocity = vector_make(kern::formula([](Float x, Float sp, Float the_result) -> Float { return ((x * sp) + ((the_result - 0.5) * 3.0)); }, vector_break(n).x, sp, n24.Result), kern::formula([](Float y, Float sp, Float the_result) -> Float { return ((y * sp) + ((the_result - 0.5) * 3.0)); }, vector_break(n).y, sp, n25.Result), kern::formula([](Float the_result) -> Float { return (1.0 + (the_result * 4.0)); }, n26.Result));
      n27 = common::Rng_unit(rng);
      n28 = common::Rng_unit(rng);
      n29 = common::Rng_unit(rng);
      bs.angular_velocity = vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 12.0); }, n27.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 12.0); }, n28.Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 12.0); }, n29.Result));
      bs.linear_damping = 0.05;
      bs.angular_damping = 0.2;
      engine_world_create_body(n30, w_, bs);
      create_body = n30.result;
      b = create_body;
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss.kind = physics_ShapeKind::box;
      ss.size = vector_scale(half, 2.0);
      ss.material.density = 2400.0;
      ss.material.friction = 0.8;
      ss.material.category_bits = Int64{0x2};
      engine_world_create_shape(n31, w_, b, ss);
      create_shape = n31.result;
      c = struct_make<Marks_Chunk>(b, half, kern::formula([](Float now, Float kRubbleSeconds) -> Float { return (now + kRubbleSeconds); }, now, kRubbleSeconds));
      if (math_compare<">">(kern::formula([](Float a, Float kRubble) -> Float { return ((a < 0.0) != (kRubble < 0.0) ? (kRubble < 0.0) : (a < kRubble)); }, kern::to<Float>(list_length<Marks_Chunk>(self.rubble_)), kern::to<Float>(kRubble)), 0.5)) {
        list_add(self.rubble_, c);
      } else {
        oldest = 0;
        for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Marks_Chunk>(self.rubble_))))); it32.next();) {
          if (math_compare<"<">(self.rubble_[n32.index].until, self.rubble_[oldest].until)) {
            oldest = n32.index;
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

  struct Marks_bulletResult { Bool Result = false; };
  Marks_bulletResult Marks_bullet(Marks& self, const CityStream& city, Rng& rng, Vector p = Vector{0.0, 0.0, 0.0}) {
    Bool skip = false;
    Vector rel = Vector{0.0, 0.0, 0.0};
    Float u = 0.0;
    Float off = 0.0;
    Marks_WallMark m = Marks_WallMark{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0, false, Int64{0x0}};
    self.struck_ = true;
    city_stream_cpp::CityStream_walls_near(city, self.near_, vector_make(vector_break(p).x, vector_break(p).y, 0.0), 0.6);
    for (auto it33 = flow_for(n33, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_Wall>(self.near_))))); it33.next();) {
      skip = false;
      rel = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p).x, vector_break(self.near_[n33.index].a).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p).y, vector_break(self.near_[n33.index].a).y), 0.0);
      u = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(rel).x, vector_break(self.near_[n33.index].dir).x, vector_break(rel).y, vector_break(self.near_[n33.index].dir).y);
      off = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return ((x * x_2) + (y * y_2)); }, vector_break(rel).x, vector_break(self.near_[n33.index].normal).x, vector_break(rel).y, vector_break(self.near_[n33.index].normal).y);
      if (logic_or(logic_or(logic_or(logic_or(math_compare<"<">(u, 0.0), math_compare<">">(u, self.near_[n33.index].len)), math_compare<">">(kern::formula([](Float off) -> Float { return kern::math::abs(off); }, off), 0.3)), math_compare<"<">(vector_break(p).z, self.near_[n33.index].base)), math_compare<">">(vector_break(p).z, self.near_[n33.index].top))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_not(facade_glass(u, self.near_[n33.index].len, kern::formula([](Float z, Float a) -> Float { return (z - a); }, vector_break(p).z, self.near_[n33.index].base), kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.near_[n33.index].top, self.near_[n33.index].base), self.near_[n33.index].seed).Result)) {
          return {false};
        }
        m = Marks_WallMark{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0, false, Int64{0x0}};
        m.c = vector_make(kern::formula([](Float x, Float x_2, Float u, Float x_3) -> Float { return ((x + (x_2 * u)) + (x_3 * 0.03)); }, vector_break(self.near_[n33.index].a).x, vector_break(self.near_[n33.index].dir).x, u, vector_break(self.near_[n33.index].normal).x), kern::formula([](Float y, Float y_2, Float u, Float y_3) -> Float { return ((y + (y_2 * u)) + (y_3 * 0.03)); }, vector_break(self.near_[n33.index].a).y, vector_break(self.near_[n33.index].dir).y, u, vector_break(self.near_[n33.index].normal).y), vector_break(p).z);
        m.n = self.near_[n33.index].normal;
        m.d = self.near_[n33.index].dir;
        n34 = common::Rng_unit(rng);
        m.size = kern::formula([](Float the_result) -> Float { return (0.16 + (the_result * 0.1)); }, n34.Result);
        n35 = common::Rng_next(rng);
        m.seed = n35.Result;
        if (math_compare<">">(kern::formula([](Float a, Float kCracks) -> Float { return ((a < 0.0) != (kCracks < 0.0) ? (kCracks < 0.0) : (a < kCracks)); }, kern::to<Float>(list_length<Marks_WallMark>(self.cracks_)), kern::to<Float>(kCracks)), 0.5)) {
          list_add(self.cracks_, m);
        } else {
          self.cracks_[kern::to<Int>(kern::formula([](Float crack_cursor_, Float kCracks) -> Float { return kern::math::mod(crack_cursor_, kCracks); }, kern::to<Float>(self.crack_cursor_), kern::to<Float>(kCracks)))] = m;
        }
        self.crack_cursor_ = kern::to<Int>(kern::formula([](Float crack_cursor_) -> Float { return kern::math::wrap32(crack_cursor_ + 1.0); }, kern::to<Float>(self.crack_cursor_)));
        self.dirty_ = true;
        return {true};
      }
    }
    return {false};
  }

  struct prune_walls_goneResult { Bool Result = false; };
  prune_walls_goneResult prune_walls_gone(const CityStream& city, List<CityStream_Wall>& walls, const Marks_WallMark& m) {
    city_stream_cpp::CityStream_walls_near(city, walls, vector_make(vector_break(m.c).x, vector_break(m.c).y, 0.0), 0.8);
    for (auto it36 = flow_for(n36, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_Wall>(walls))))); it36.next();) {
      if (logic_and(math_compare<">=">(walls[n36.index].top, kern::formula([](Float z) -> Float { return (z - 0.15); }, vector_break(m.c).z)), math_compare<"<=">(walls[n36.index].base, kern::formula([](Float z) -> Float { return (z + 0.5); }, vector_break(m.c).z)))) {
        return {false};
      }
    }
    return {true};
  }

  struct rebuild_atResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rebuild_atResult rebuild_at(const Terrain& t, Float x = 0.0, Float y = 0.0) {
    return {vector_make(x, y, kern::formula([](Float the_result) -> Float { return (the_result + 0.16); }, terrain_cpp::Terrain_height(t, x, y).Result))};
  }

  struct rebuild_ring_atResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rebuild_ring_atResult rebuild_ring_at(const Marks_Scorch& self_scorches_n38_index, const Terrain& t, Int kSeg = 0, Float sd = 0.0, Float k = 0.0, Int i = 0) {
    Float a = 0.0;
    Float rr = 0.0;
    a = kern::formula([](Float kPi, Float i, Float kSeg) -> Float { return kern::math::div(((2.0 * kPi) * i), kSeg); }, kPi, kern::to<Float>(i), kern::to<Float>(kSeg));
    rr = kern::formula([](Float r, Float k, Float the_result) -> Float { return ((r * k) * the_result); }, self_scorches_n38_index.r, k, ragged(a, sd).Result);
    return {rebuild_at(t, kern::formula([](Float x, Float a, Float rr) -> Float { return (x + (kern::math::cos(a) * rr)); }, vector_break(self_scorches_n38_index.c).x, a, rr), kern::formula([](Float y, Float a, Float rr) -> Float { return (y + (kern::math::sin(a) * rr)); }, vector_break(self_scorches_n38_index.c).y, a, rr)).Result};
  }

  struct rebuild_char_colResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rebuild_char_colResult rebuild_char_col(Float k = 0.0) {
    Float u = 0.0;
    Float c = 0.0;
    u = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float k) -> Float { return kern::math::div((1.0 - k), 0.65); }, k), 0.0, 1.0));
    c = kern::formula([](Float u) -> Float { return ((u * u) * (3.0 - (2.0 * u))); }, u);
    return {vector_add(vector_scale(Vector{0.05, 0.045, 0.04}, kern::formula([](Float c) -> Float { return (1.0 - c); }, c)), vector_scale(Vector{0.012, 0.01, 0.01}, c))};
  }

  void rebuild_band(const Marks_Scorch& self_scorches_n38_index, const Terrain& t, MeshData& g, Int kSeg = 0, Float sd = 0.0, Float k0 = 0.0, Float k1 = 0.0) {
    Int base = 0;
    Int j = 0;
    Int a0 = 0;
    Int a1 = 0;
    Int b0 = 0;
    Int b1 = 0;
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    base = engine_geo_mesh_data::MeshData_vertex_count(g).Result;
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it39.next();) {
      engine_geo_mesh_data::MeshData_push(g, rebuild_ring_at(self_scorches_n38_index, t, kSeg, sd, k0, n39.index).Result, Vector{0.0, 0.0, 1.0}, rebuild_char_col(k0).Result, Vector{0.0, 0.0, 0.0});
    }
    for (auto it40 = flow_for(n40, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it40.next();) {
      engine_geo_mesh_data::MeshData_push(g, rebuild_ring_at(self_scorches_n38_index, t, kSeg, sd, k1, n40.index).Result, Vector{0.0, 0.0, 1.0}, rebuild_char_col(k1).Result, Vector{0.0, 0.0, 0.0});
    }
    for (auto it41 = flow_for(n41, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it41.next();) {
      j = kern::to<Int>(kern::formula([](Float index, Float kSeg) -> Float { return kern::math::wrap32(kern::math::mod((index + 1.0), kSeg)); }, kern::to<Float>(n41.index), kern::to<Float>(kSeg)));
      a0 = kern::to<Int>(kern::formula([](Float base, Float index) -> Float { return kern::math::wrap32(base + kern::math::wrap32(index)); }, kern::to<Float>(base), kern::to<Float>(n41.index)));
      a1 = kern::to<Int>(kern::formula([](Float base, Float j) -> Float { return kern::math::wrap32(base + j); }, kern::to<Float>(base), kern::to<Float>(j)));
      b0 = kern::to<Int>(kern::formula([](Float a0, Float kSeg) -> Float { return kern::math::wrap32(a0 + kern::math::wrap32(kSeg)); }, kern::to<Float>(a0), kern::to<Float>(kSeg)));
      b1 = kern::to<Int>(kern::formula([](Float a1, Float kSeg) -> Float { return kern::math::wrap32(a1 + kern::math::wrap32(kSeg)); }, kern::to<Float>(a1), kern::to<Float>(kSeg)));
      place = list_length<Int>(g.indices);
      item = a0;
      item_2 = b0;
      item_3 = b1;
      item_4 = a0;
      item_5 = b1;
      item_6 = a1;
      list_insert_list(g.indices, place, list_make<Int, "6">(item, item_2, item_3, item_4, item_5, item_6));
    }
  }

  struct rebuild_wall_disc_PResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rebuild_wall_disc_PResult rebuild_wall_disc_P(const Marks_WallMark& m, Float x = 0.0, Float y = 0.0) {
    return {vector_add(m.c, vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x * x_2); }, vector_break(m.d).x, x), kern::formula([](Float y, Float x) -> Float { return (y * x); }, vector_break(m.d).y, x), y))};
  }

  /// Wall marks in the wall's plane (dir along the wall, z up).
  void Marks_rebuild_wall_disc(Marks& self, const Marks_WallMark& m, Int kSeg = 0, Float inner = 0.0, Vector c_in = Vector{0.0, 0.0, 0.0}, Float outer_k = 0.0, Vector c_out = Vector{0.0, 0.0, 0.0}, Float seed = 0.0) {
    Int base = 0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Float a = 0.0;
    Float e = 0.0;
    Float a_2 = 0.0;
    Float e_2 = 0.0;
    Int j = 0;
    Int a0 = 0;
    Int a1 = 0;
    Int b0 = 0;
    Int b1 = 0;
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Int item_7 = 0;
    Int item_8 = 0;
    Int item_9 = 0;
    base = engine_geo_mesh_data::MeshData_vertex_count(self.wall_data_).Result;
    n = vector_make(vector_break(m.n).x, vector_break(m.n).y, 0.0);
    engine_geo_mesh_data::MeshData_push(self.wall_data_, m.c, n, c_in, Vector{0.0, 0.0, 0.0});
    for (auto it44 = flow_for(n44, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it44.next();) {
      a = kern::formula([](Float kPi, Float index, Float kSeg) -> Float { return kern::math::div(((2.0 * kPi) * index), kSeg); }, kPi, kern::to<Float>(n44.index), kern::to<Float>(kSeg));
      e = ragged(a, seed).Result;
      engine_geo_mesh_data::MeshData_push(self.wall_data_, rebuild_wall_disc_P(m, kern::formula([](Float a, Float size, Float inner, Float e) -> Float { return (((kern::math::cos(a) * size) * inner) * e); }, a, m.size, inner, e), kern::formula([](Float a, Float size, Float inner, Float e) -> Float { return (((kern::math::sin(a) * size) * inner) * e); }, a, m.size, inner, e)).Result, n, c_in, Vector{0.0, 0.0, 0.0});
    }
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it45.next();) {
      a_2 = kern::formula([](Float kPi, Float index, Float kSeg) -> Float { return kern::math::div(((2.0 * kPi) * index), kSeg); }, kPi, kern::to<Float>(n45.index), kern::to<Float>(kSeg));
      e_2 = ragged(a_2, seed).Result;
      engine_geo_mesh_data::MeshData_push(self.wall_data_, rebuild_wall_disc_P(m, kern::formula([](Float a_2, Float size, Float outer_k, Float e_2) -> Float { return (((kern::math::cos(a_2) * size) * outer_k) * e_2); }, a_2, m.size, outer_k, e_2), kern::formula([](Float a_2, Float size, Float outer_k, Float e_2) -> Float { return (((kern::math::sin(a_2) * size) * outer_k) * e_2); }, a_2, m.size, outer_k, e_2)).Result, n, c_out, Vector{0.0, 0.0, 0.0});
    }
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it46.next();) {
      j = kern::to<Int>(kern::formula([](Float index, Float kSeg) -> Float { return kern::math::wrap32(kern::math::mod((index + 1.0), kSeg)); }, kern::to<Float>(n46.index), kern::to<Float>(kSeg)));
      a0 = kern::to<Int>(kern::formula([](Float base, Float index) -> Float { return kern::math::wrap32(kern::math::wrap32(base + 1.0) + kern::math::wrap32(index)); }, kern::to<Float>(base), kern::to<Float>(n46.index)));
      a1 = kern::to<Int>(kern::formula([](Float base, Float j) -> Float { return kern::math::wrap32(kern::math::wrap32(base + 1.0) + j); }, kern::to<Float>(base), kern::to<Float>(j)));
      b0 = kern::to<Int>(kern::formula([](Float a0, Float kSeg) -> Float { return kern::math::wrap32(a0 + kern::math::wrap32(kSeg)); }, kern::to<Float>(a0), kern::to<Float>(kSeg)));
      b1 = kern::to<Int>(kern::formula([](Float a1, Float kSeg) -> Float { return kern::math::wrap32(a1 + kern::math::wrap32(kSeg)); }, kern::to<Float>(a1), kern::to<Float>(kSeg)));
      place = list_length<Int>(self.wall_data_.indices);
      item = base;
      item_2 = a0;
      item_3 = a1;
      item_4 = a0;
      item_5 = b0;
      item_6 = b1;
      item_7 = a0;
      item_8 = b1;
      item_9 = a1;
      list_insert_list(self.wall_data_.indices, place, list_make<Int, "9">(item, item_2, item_3, item_4, item_5, item_6, item_7, item_8, item_9));
    }
  }

  void Marks_rebuild_line(Marks& self, const Marks_WallMark& self_cracks_n54_index, Vector n_2 = Vector{0.0, 0.0, 0.0}, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float w = 0.0, Vector col = Vector{0.0, 0.0, 0.0}) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Vector s = Vector{0.0, 0.0, 0.0};
    Int base = 0;
    List<Vector> list = List<Vector>{};
    Vector q = Vector{0.0, 0.0, 0.0};
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    d = vector_sub(b, a);
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::max(0.0001, kern::math::sqrt((x * x) + (y * y))); }, vector_break(d).x, vector_break(d).y);
    s = vector_make(kern::formula([](Float y, Float l, Float w) -> Float { return (kern::math::div((-y), l) * w); }, vector_break(d).y, l, w), kern::formula([](Float x, Float l, Float w) -> Float { return (kern::math::div(x, l) * w); }, vector_break(d).x, l, w), 0.0);
    base = engine_geo_mesh_data::MeshData_vertex_count(self.wall_data_).Result;
    list = list_make<Vector, "4">(vector_add(a, s), vector_sub(a, s), vector_sub(b, s), vector_add(b, s));
    for (auto it55 = flow_for(n55, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(list))))); it55.next();) {
      q = list[n55.index];
      engine_geo_mesh_data::MeshData_push(self.wall_data_, vector_add(self_cracks_n54_index.c, vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x * x_2); }, vector_break(self_cracks_n54_index.d).x, vector_break(q).x), kern::formula([](Float y, Float x) -> Float { return (y * x); }, vector_break(self_cracks_n54_index.d).y, vector_break(q).x), vector_break(q).y)), n_2, col, Vector{0.0, 0.0, 0.0});
    }
    place = list_length<Int>(self.wall_data_.indices);
    item = base;
    item_2 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
    item_3 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
    item_4 = base;
    item_5 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
    item_6 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 3.0); }, kern::to<Float>(base)));
    list_insert_list(self.wall_data_.indices, place, list_make<Int, "6">(item, item_2, item_3, item_4, item_5, item_6));
  }

  void Marks_rebuild(Marks& self, const CityStream& city) {
    Terrain t = Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}};
    Int kSeg = 0;
    Float sd = 0.0;
    Int base = 0;
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Float sd_2 = 0.0;
    Float k = 0.0;
    Float sd_3 = 0.0;
    Float hole = 0.0;
    Float g = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Rng cr = Rng{2654435769, 0.0, false};
    Int cracks = 0;
    Float a = 0.0;
    Float r0 = 0.0;
    Float r1 = 0.0;
    Float wdt = 0.0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Vector side = Vector{0.0, 0.0, 0.0};
    Int base_2 = 0;
    List<Vector> list = List<Vector>{};
    Vector q = Vector{0.0, 0.0, 0.0};
    Int place_2 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Int item_7 = 0;
    Int item_8 = 0;
    Int item_9 = 0;
    Vector n_2 = Vector{0.0, 0.0, 0.0};
    Rng cr_2 = Rng{2654435769, 0.0, false};
    Vector c = Vector{0.0, 0.0, 0.0};
    Int spokes = 0;
    List<Vector> ends = List<Vector>{};
    Float a_2 = 0.0;
    Float r = 0.0;
    Vector e = Vector{0.0, 0.0, 0.0};
    Vector a_3 = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    t = city.terrain_;
    for (auto it37 = flow_for(n37, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MeshData>(self.ground_data_))))); it37.next();) {
      engine_geo_mesh_data::MeshData_clear(self.ground_data_[n37.index]);
    }
    engine_geo_mesh_data::MeshData_clear(self.wall_data_);
    kSeg = 18;
    for (auto it38 = flow_for(n38, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Marks_Scorch>(self.scorches_))))); it38.next();) {
      sd = kern::formula([](Float a) -> Float { return kern::math::div(kern::math::mod(a, 1000.0), 100.0); }, kern::to<Float>(self.scorches_[n38.index].seed));
      base = engine_geo_mesh_data::MeshData_vertex_count(self.ground_data_[0]).Result;
      engine_geo_mesh_data::MeshData_push(self.ground_data_[0], rebuild_at(t, vector_break(self.scorches_[n38.index].c).x, vector_break(self.scorches_[n38.index].c).y).Result, Vector{0.0, 0.0, 1.0}, rebuild_char_col(0.0).Result, Vector{0.0, 0.0, 0.0});
      for (auto it42 = flow_for(n42, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it42.next();) {
        engine_geo_mesh_data::MeshData_push(self.ground_data_[0], rebuild_ring_at(self.scorches_[n38.index], t, kSeg, sd, 0.35, n42.index).Result, Vector{0.0, 0.0, 1.0}, rebuild_char_col(0.35).Result, Vector{0.0, 0.0, 0.0});
      }
      for (auto it43 = flow_for(n43, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kSeg)))); it43.next();) {
        place = list_length<Int>(self.ground_data_[0].indices);
        item = base;
        item_2 = kern::to<Int>(kern::formula([](Float base, Float index) -> Float { return kern::math::wrap32(kern::math::wrap32(base + 1.0) + kern::math::wrap32(index)); }, kern::to<Float>(base), kern::to<Float>(n43.index)));
        item_3 = kern::to<Int>(kern::formula([](Float base, Float index, Float kSeg) -> Float { return kern::math::wrap32(kern::math::wrap32(base + 1.0) + kern::math::wrap32(kern::math::mod((index + 1.0), kSeg))); }, kern::to<Float>(base), kern::to<Float>(n43.index), kern::to<Float>(kSeg)));
        list_insert_list(self.ground_data_[0].indices, place, list_make<Int, "3">(item, item_2, item_3));
      }
      rebuild_band(self.scorches_[n38.index], t, self.ground_data_[0], kSeg, sd, 0.35, 0.7);
      rebuild_band(self.scorches_[n38.index], t, self.ground_data_[1], kSeg, sd, 0.7, 0.86);
      rebuild_band(self.scorches_[n38.index], t, self.ground_data_[2], kSeg, sd, 0.86, 1.0);
    }
    for (auto it47 = flow_for(n47, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Marks_WallMark>(self.soots_))))); it47.next();) {
      sd_2 = kern::formula([](Float a) -> Float { return kern::math::div(kern::math::mod(a, 1000.0), 100.0); }, kern::to<Float>(self.soots_[n47.index].seed));
      k = kern::formula([](Float a) -> Float { return (0.06 + (0.05 * (1.0 - a))); }, self.soots_[n47.index].level);
      Marks_rebuild_wall_disc(self, self.soots_[n47.index], kSeg, 0.55, Vector{0.015, 0.012, 0.01}, 1.0, vector_make(k, kern::formula([](Float k) -> Float { return (k * 0.86); }, k), kern::formula([](Float k) -> Float { return (k * 0.72); }, k)), sd_2);
    }
    for (auto it48 = flow_for(n48, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Marks_WallMark>(self.breaches_))))); it48.next();) {
      sd_3 = kern::formula([](Float a) -> Float { return kern::math::div(kern::math::mod(a, 1000.0), 100.0); }, kern::to<Float>(self.breaches_[n48.index].seed));
      hole = kern::formula([](Float a) -> Float { return (0.18 + (0.42 * a)); }, self.breaches_[n48.index].level);
      g = kern::formula([](Float a) -> Float { return (0.46 - (0.2 * a)); }, self.breaches_[n48.index].level);
      Marks_rebuild_wall_disc(self, self.breaches_[n48.index], kSeg, hole, Vector{0.035, 0.03, 0.028}, 0.82, vector_make(g, kern::formula([](Float g) -> Float { return (g * 0.96); }, g), kern::formula([](Float g) -> Float { return (g * 0.9); }, g)), sd_3);
      n = vector_make(vector_break(self.breaches_[n48.index].n).x, vector_break(self.breaches_[n48.index].n).y, 0.0);
      cr = struct_make<Rng>(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::bor(a, 1.0); }, kern::to<Float>(self.breaches_[n48.index].seed))), 0.0, false);
      n49 = common::Rng_next(cr);
      cracks = kern::to<Int>(kern::formula([](Float the_result) -> Float { return (5.0 + kern::math::wrap32s(kern::math::mod(the_result, 5.0))); }, kern::to<Float>(n49.Result)));
      for (auto it50 = flow_for(n50, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(cracks)))); it50.next();) {
        n51 = common::Rng_unit(cr);
        a = kern::formula([](Float kPi, Float index, Float the_result, Float cracks) -> Float { return kern::math::div(((2.0 * kPi) * (index + (the_result * 0.7))), cracks); }, kPi, kern::to<Float>(n50.index), n51.Result, kern::to<Float>(cracks));
        r0 = kern::formula([](Float a, Float hole) -> Float { return ((a * hole) * 0.9); }, self.breaches_[n48.index].size, hole);
        n52 = common::Rng_unit(cr);
        r1 = kern::formula([](Float a, Float the_result, Float a_2) -> Float { return ((a * (0.55 + (0.45 * the_result))) * (1.0 + (0.4 * a_2))); }, self.breaches_[n48.index].size, n52.Result, self.breaches_[n48.index].level);
        wdt = kern::formula([](Float a) -> Float { return (a * 0.025); }, self.breaches_[n48.index].size);
        dir = vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, a), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, a), 0.0);
        side = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(dir).y), vector_break(dir).x, 0.0);
        base_2 = engine_geo_mesh_data::MeshData_vertex_count(self.wall_data_).Result;
        list = list_make<Vector, "4">(vector_add(vector_scale(dir, r0), vector_scale(side, wdt)), vector_sub(vector_scale(dir, r0), vector_scale(side, wdt)), vector_sub(vector_scale(dir, r1), vector_scale(side, kern::formula([](Float wdt) -> Float { return (wdt * 0.2); }, wdt))), vector_add(vector_scale(dir, r1), vector_scale(side, kern::formula([](Float wdt) -> Float { return (wdt * 0.2); }, wdt))));
        for (auto it53 = flow_for(n53, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(list))))); it53.next();) {
          q = list[n53.index];
          engine_geo_mesh_data::MeshData_push(self.wall_data_, vector_add(vector_add(self.breaches_[n48.index].c, vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x * x_2); }, vector_break(self.breaches_[n48.index].d).x, vector_break(q).x), kern::formula([](Float y, Float x) -> Float { return (y * x); }, vector_break(self.breaches_[n48.index].d).y, vector_break(q).x), vector_break(q).y)), vector_scale(n, 0.002)), n, Vector{0.03, 0.028, 0.026}, Vector{0.0, 0.0, 0.0});
        }
        place_2 = list_length<Int>(self.wall_data_.indices);
        item_4 = base_2;
        item_5 = kern::to<Int>(kern::formula([](Float base_2) -> Float { return kern::math::wrap32(base_2 + 1.0); }, kern::to<Float>(base_2)));
        item_6 = kern::to<Int>(kern::formula([](Float base_2) -> Float { return kern::math::wrap32(base_2 + 2.0); }, kern::to<Float>(base_2)));
        item_7 = base_2;
        item_8 = kern::to<Int>(kern::formula([](Float base_2) -> Float { return kern::math::wrap32(base_2 + 2.0); }, kern::to<Float>(base_2)));
        item_9 = kern::to<Int>(kern::formula([](Float base_2) -> Float { return kern::math::wrap32(base_2 + 3.0); }, kern::to<Float>(base_2)));
        list_insert_list(self.wall_data_.indices, place_2, list_make<Int, "6">(item_4, item_5, item_6, item_7, item_8, item_9));
      }
    }
    for (auto it54 = flow_for(n54, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Marks_WallMark>(self.cracks_))))); it54.next();) {
      n_2 = vector_make(vector_break(self.cracks_[n54.index].n).x, vector_break(self.cracks_[n54.index].n).y, 0.0);
      cr_2 = struct_make<Rng>(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::bor(a, 1.0); }, kern::to<Float>(self.cracks_[n54.index].seed))), 0.0, false);
      c = Vector{0.82, 0.86, 0.9};
      n56 = common::Rng_next(cr_2);
      spokes = kern::to<Int>(kern::formula([](Float the_result) -> Float { return (7.0 + kern::math::wrap32s(kern::math::mod(the_result, 4.0))); }, kern::to<Float>(n56.Result)));
      ends = List<Vector>{};
      for (auto it57 = flow_for(n57, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(spokes)))); it57.next();) {
        n58 = common::Rng_unit(cr_2);
        a_2 = kern::formula([](Float kPi, Float index, Float the_result, Float spokes) -> Float { return kern::math::div(((2.0 * kPi) * (index + ((the_result - 0.5) * 0.6))), spokes); }, kPi, kern::to<Float>(n57.index), n58.Result, kern::to<Float>(spokes));
        n59 = common::Rng_unit(cr_2);
        r = kern::formula([](Float a, Float the_result) -> Float { return (a * (0.6 + (0.4 * the_result))); }, self.cracks_[n54.index].size, n59.Result);
        e = vector_make(kern::formula([](Float a_2, Float r) -> Float { return (kern::math::cos(a_2) * r); }, a_2, r), kern::formula([](Float a_2, Float r) -> Float { return (kern::math::sin(a_2) * r); }, a_2, r), 0.0);
        Marks_rebuild_line(self, self.cracks_[n54.index], n_2, vector_scale(e, 0.08), vector_scale(e, 0.55), 0.004, c);
        Marks_rebuild_line(self, self.cracks_[n54.index], n_2, vector_scale(e, 0.55), e, 0.003, c);
        list_add(ends, e);
      }
      for (auto it60 = flow_for(n60, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(ends))))); it60.next();) {
        a_3 = ends[n60.index];
        b = ends[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n60.index), kern::to<Float>(list_length<Vector>(ends))))];
        Marks_rebuild_line(self, self.cracks_[n54.index], n_2, vector_scale(a_3, 0.35), vector_scale(b, 0.35), 0.003, c);
        n61 = common::Rng_unit(cr_2);
        if (math_compare<"<">(n61.Result, 0.7)) {
          Marks_rebuild_line(self, self.cracks_[n54.index], n_2, vector_scale(a_3, 0.7), vector_scale(b, 0.7), 0.0025, c);
        }
      }
      Marks_rebuild_line(self, self.cracks_[n54.index], n_2, Vector{-0.01, 0.0, 0.0}, Vector{0.01, 0.0, 0.0}, 0.01, Vector{0.95, 0.97, 1.0});
    }
  }

  void Marks_draw(Marks& self, WorldArt& art, const CityStream& city, Int w = 0, Float now = 0.0) {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Fixed<Float, 3> alphas = Fixed<Float, 3>{};
    Material g = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Bool skip = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool either = false;
    physics_Pose p = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float fade = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_and(self.dirty_, self.struck_)) {
      Marks_rebuild(self, city);
      m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
      m.alpha = AlphaMode::blend;
      m.color = engine_math_math::Vec4_new(1.0, 1.0, 1.0, 0.88).Result;
      m.roughness = 1.0;
      m.cast_shadow = false;
      m.double_sided = true;
      world_art_cpp::WorldArt_set_mesh(art, self.wall_mesh_, self.wall_data_, m);
      alphas = list_make<Float, "3">(0.6, 0.4, 0.15);
      for (auto it62 = flow_for(n62, 0, 2); it62.next();) {
        g = m;
        g.color = engine_math_math::Vec4_new(1.0, 1.0, 1.0, alphas[n62.index]).Result;
        world_art_cpp::WorldArt_set_mesh(art, self.ground_mesh_[n62.index], self.ground_data_[n62.index], g);
      }
      self.dirty_ = false;
    }
    for (auto it63 = flow_for(n63, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Marks_Chunk>(self.rubble_))))); it63.next();) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(self.rubble_[n63.index].body).Result);
      if (logic_not(either)) {
        body_valid_2 = engine_world_body_valid(w, self.rubble_[n63.index].body);
        either = logic_not(body_valid_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        pose = engine_world_pose(w, self.rubble_[n63.index].body);
        p = pose;
        fade = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float now) -> Float { return kern::math::div((a - now), 2.5); }, self.rubble_[n63.index].until, now), 0.0, 1.0));
        a = engine_math_math::Mat4_trs(p.position, p.rotation, vector_scale(self.rubble_[n63.index].half, kern::formula([](Float fade) -> Float { return (2.0 * fade); }, fade))).Result;
        world_art_cpp::WorldArt_put(art, a, art.cube, kRubbleColor);
      }
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<15> { Int index = 0; } n15;
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
  common::Rng_unitResult n28;
  common::Rng_unitResult n29;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n30;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  common::Rng_unitResult n34;
  common::Rng_nextResult n35;
  struct : kern::Node<36> { Int index = 0; } n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct : kern::Node<40> { Int index = 0; } n40;
  struct : kern::Node<41> { Int index = 0; } n41;
  struct : kern::Node<42> { Int index = 0; } n42;
  struct : kern::Node<43> { Int index = 0; } n43;
  struct : kern::Node<44> { Int index = 0; } n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  struct : kern::Node<47> { Int index = 0; } n47;
  struct : kern::Node<48> { Int index = 0; } n48;
  common::Rng_nextResult n49;
  struct : kern::Node<50> { Int index = 0; } n50;
  common::Rng_unitResult n51;
  common::Rng_unitResult n52;
  struct : kern::Node<53> { Int index = 0; } n53;
  struct : kern::Node<54> { Int index = 0; } n54;
  struct : kern::Node<55> { Int index = 0; } n55;
  common::Rng_nextResult n56;
  struct : kern::Node<57> { Int index = 0; } n57;
  common::Rng_unitResult n58;
  common::Rng_unitResult n59;
  struct : kern::Node<60> { Int index = 0; } n60;
  common::Rng_unitResult n61;
  struct : kern::Node<62> { Int index = 0; } n62;
  struct : kern::Node<63> { Int index = 0; } n63;
};
