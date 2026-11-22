struct people_cpp : virtual kern::Library, virtual city_stream, virtual common, virtual people, virtual person_hit {
  // ---- variables ----
  /// wanted_system_logic: a witness within sight
  KERN_CONST Float kWitness = 45.0;
  KERN_CONST Float kCapsuleR = 0.32;
  KERN_CONST Fixed<Vector, 5> kSkins = List<Vector>{Vector{0.94, 0.76, 0.62}, Vector{0.78, 0.58, 0.43}, Vector{0.66, 0.44, 0.32}, Vector{0.48, 0.31, 0.22}, Vector{0.86, 0.66, 0.5}};
  KERN_CONST Fixed<Vector, 10> kShirts = List<Vector>{Vector{0.62, 0.12, 0.1}, Vector{0.16, 0.28, 0.55}, Vector{0.85, 0.82, 0.74}, Vector{0.2, 0.42, 0.25}, Vector{0.9, 0.62, 0.15}, Vector{0.35, 0.35, 0.38}, Vector{0.55, 0.3, 0.55}, Vector{0.1, 0.1, 0.12}, Vector{0.75, 0.45, 0.4}, Vector{0.3, 0.6, 0.7}};
  KERN_CONST Fixed<Vector, 6> kPants = List<Vector>{Vector{0.12, 0.14, 0.22}, Vector{0.2, 0.2, 0.22}, Vector{0.38, 0.3, 0.22}, Vector{0.08, 0.08, 0.09}, Vector{0.45, 0.45, 0.48}, Vector{0.25, 0.3, 0.45}};
  /// pedestrian_agents.mjs / world_procedural_population.mjs world_procedural_population.mjs PEOPLE_SHOW_M / PEOPLE_HIDE_M: 170 / 190 m (phone 140 / 160), set_ranges
  KERN_CONST Float kPavement = 1.6;

  // ---- functions ----
  struct dist2dResult { Float Result = 0.0; };
  KERN_PURE dist2dResult dist2d(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct People_witnessedResult { Bool Result = false; };
  KERN_PURE People_witnessedResult People_witnessed(const People& self, Vector at = Vector{0.0, 0.0, 0.0}) {
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it10.next();) {
      if (logic_and(logic_and(enum_equal<People_Kind>(self.people_[n10.index].kind, People_Kind::citizen), logic_or(logic_or(enum_equal<People_State>(self.people_[n10.index].state, People_State::walk), enum_equal<People_State>(self.people_[n10.index].state, People_State::wait)), enum_equal<People_State>(self.people_[n10.index].state, People_State::flee))), math_compare<"<">(dist2d(self.people_[n10.index].pos, at).Result, kWitness))) {
        return {true};
      }
    }
    return {false};
  }

  void People_frighten(People& self, Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float now = 0.0, Float strength = 0.0) {
    Bool skip = false;
    Float d = 0.0;
    U32 roll_bits = 0;
    Float roll = 0.0;
    Vector away = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float s = 0.0;
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it11.next();) {
      skip = false;
      if (logic_or(logic_or(logic_or(logic_not(enum_equal<People_Kind>(self.people_[n11.index].kind, People_Kind::citizen)), enum_equal<People_State>(self.people_[n11.index].state, People_State::down)), enum_equal<People_State>(self.people_[n11.index].state, People_State::gone)), enum_equal<People_State>(self.people_[n11.index].state, People_State::fight))) {
        skip = true;
      }
      if (logic_not(skip)) {
        d = dist2d(self.people_[n11.index].pos, at).Result;
        if (math_compare<">">(d, radius)) {
          skip = true;
        }
        if (logic_not(skip)) {
          roll_bits = kern::to<Int>(kern::formula([](Float a, Float now) -> Float { return kern::math::mul32(kern::math::bxor(a, kern::math::trunc((now * 1000.0))), 2246822519.0); }, kern::to<Float>(self.people_[n11.index].seed), now));
          roll = kern::formula([](Float roll_bits) -> Float { return kern::math::div(kern::math::shr(roll_bits, 8.0), 16777216.0); }, kern::to<Float>(roll_bits));
          if (logic_and(math_compare<">">(d, kern::formula([](Float radius) -> Float { return (radius * 0.45); }, radius)), math_compare<"<">(roll, 0.5))) {
            skip = true;
          }
          if (logic_not(skip)) {
            away = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(self.people_[n11.index].pos).x, vector_break(at).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(self.people_[n11.index].pos).y, vector_break(at).y), 0.0);
            l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(away).x, vector_break(away).y);
            away = logic_select<Vector>(math_compare<">">(l, 0.3), vector_scale(away, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l)), vector_make(kern::formula([](Float roll) -> Float { return kern::math::cos((roll * 6.28)); }, roll), kern::formula([](Float roll) -> Float { return kern::math::sin((roll * 6.28)); }, roll), 0.0));
            s = strength;
            self.people_[n11.index].flee_dir = away;
            self.people_[n11.index].flee_speed = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float s, Float a) -> Float { return ((3.4 + (1.5 * s)) * a); }, s, self.people_[n11.index].fitness), 2.8, 7.2));
            self.people_[n11.index].flee_until = kern::formula([](Float a, Float now, Float roll) -> Float { return kern::math::max(a, (now + (5.6 * (1.3 + (0.7 * roll))))); }, self.people_[n11.index].flee_until, now, roll);
            if (math_compare<">=">(s, 1.0)) {
              self.people_[n11.index].burst_until = kern::formula([](Float a, Float now, Float s, Float roll) -> Float { return kern::math::max(a, ((now + 1.8) + ((2.6 * kern::math::min(1.6, s)) * roll))); }, self.people_[n11.index].burst_until, now, s, roll);
              if (math_compare<"<">(roll, 0.55)) {
                self.people_[n11.index].hands_up_until = kern::formula([](Float now, Float roll) -> Float { return ((now + 0.9) + (1.8 * roll)); }, now, roll);
              }
            }
            self.people_[n11.index].shaken_until = kern::formula([](Float a, Float roll) -> Float { return ((a + 12.0) + (10.0 * roll)); }, self.people_[n11.index].flee_until, roll);
            self.people_[n11.index].state = People_State::flee;
          }
        }
      }
    }
  }

  void People_kill(People& self, I32 i = 0, Vector from_dir = Vector{0.0, 0.0, 0.0}, Bool by_player = false, Float now = 0.0, Bool hit_by_car = false) {
    Vector at = Vector{0.0, 0.0, 0.0};
    Bool officer = false;
    Float l = 0.0;
    wanted_Crime c = wanted_Crime::person;
    if (logic_or(enum_equal<People_State>(self.people_[i].state, People_State::down), enum_equal<People_State>(self.people_[i].state, People_State::gone))) {
      return;
    }
    at = self.people_[i].pos;
    officer = enum_equal<People_Kind>(self.people_[i].kind, People_Kind::officer);
    self.people_[i].state = People_State::down;
    self.people_[i].hp = 0.0;
    self.people_[i].down_at = now;
    self.people_[i].v = 0.0;
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(from_dir).x, vector_break(from_dir).y);
    self.people_[i].fall_dir = logic_select<Vector>(math_compare<">">(l, 0.001), vector_make(kern::formula([](Float x, Float l) -> Float { return kern::math::div(x, l); }, vector_break(from_dir).x, l), kern::formula([](Float y, Float l) -> Float { return kern::math::div(y, l); }, vector_break(from_dir).y, l), 0.0), vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, self.people_[i].yaw), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, self.people_[i].yaw), 0.0));
    list_add(self.deaths_, struct_make<People_Death>(i, self.people_[i].kind, by_player, self.last_head_));
    list_add(self.falls_, struct_make<People_Fall>(i, from_dir, hit_by_car, false));
    if (logic_and(by_player, logic_or(enum_equal<People_Kind>(self.people_[i].kind, People_Kind::citizen), officer))) {
      c = logic_select<wanted_Crime>(officer, wanted_Crime::police_officer, wanted_Crime::person);
      list_add(self.reports_, struct_make<CrimeReport>(c, at, logic_or(officer, People_witnessed(self, at).Result)));
    } else if (logic_and(by_player, enum_equal<People_Kind>(self.people_[i].kind, People_Kind::enemy))) {
      list_add(self.reports_, struct_make<CrimeReport>(wanted_Crime::person_hit, at, true));
    }
    People_frighten(self, at, 22.0, now, 1.2);
  }

  /// Damage to person `i` from somewhere else (a mate's round on our zombie): down at 0.
  void People_hurt(People& self, I32 i = 0, Float damage = 0.0, Vector from_dir = Vector{0.0, 0.0, 0.0}, Bool by_player = false, Float now = 0.0) {
    if (logic_or(enum_equal<People_State>(self.people_[i].state, People_State::down), enum_equal<People_State>(self.people_[i].state, People_State::gone))) {
      return;
    }
    self.people_[i].hp = kern::formula([](Float a, Float damage) -> Float { return (a - damage); }, self.people_[i].hp, damage);
    self.people_[i].hurt_at = now;
    if (math_compare<"<=">(self.people_[i].hp, 0.0)) {
      People_kill(self, i, from_dir, by_player, now, false);
    }
  }

  struct approach_angleResult { Float Result = 0.0; };
  KERN_PURE approach_angleResult approach_angle(Float cur = 0.0, Float target = 0.0, Float max_step = 0.0) {
    return {kern::formula([](Float cur, Float the_result, Float max_step) -> Float { return (cur + (the_result < (-max_step) ? (-max_step) : (max_step < the_result ? max_step : the_result))); }, cur, common::wrap_pi(kern::formula([](Float target, Float cur) -> Float { return (target - cur); }, target, cur)).Result, max_step)};
  }

  struct inside_buildingResult { Bool Result = false; };
  KERN_PURE inside_buildingResult inside_building(Int w = 0, Vector p = Vector{0.0, 0.0, 0.0}, Float ground = 0.0) {
    the_RayResult_ h = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    h = common::cast(w, vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float ground) -> Float { return (ground + 60.0); }, ground)), Vector{0.0, 0.0, -1.0}, 70.0, struct_make<physics_Body>(Int64{0x0}), Int64{0x1}).Result;
    return {logic_and(h.hit, math_compare<">">(vector_break(h.point).z, kern::formula([](Float ground) -> Float { return (ground + 0.35); }, ground)))};
  }

  struct People_route_pointResult { Bool Result = false; };
  People_route_pointResult People_route_point(const People_Person& p, const List<Lane>& lanes, Vector& out, Float& heading) {
    Lane L = Lane{List<Vector>{}, 6.0, Int64{0x0}, true};
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector u = Vector{0.0, 0.0, 0.0};
    Vector walk = Vector{0.0, 0.0, 0.0};
    Vector right = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    if (logic_or(math_compare<"<">(kern::to<Float>(p.lane), 0.0), math_compare<">=">(kern::to<Float>(p.lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(lanes))))))))) {
      return {false};
    }
    L = lanes[p.lane];
    if (logic_or(logic_or(logic_not(L.alive), math_compare<"<">(kern::to<Float>(p.seg), 0.0)), math_compare<">=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float seg) -> Float { return (seg + 1.0); }, kern::to<Float>(p.seg))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Vector>(L.points))))))))) {
      return {false};
    }
    a = L.points[p.seg];
    b = L.points[kern::to<Int>(kern::formula([](Float seg) -> Float { return (seg + 1.0); }, kern::to<Float>(p.seg)))];
    d = vector_sub(b, a);
    len = kern::formula([](Float x, Float y) -> Float { return kern::math::max(0.001, kern::math::sqrt((x * x) + (y * y))); }, vector_break(d).x, vector_break(d).y);
    u = vector_scale(d, kern::formula([](Float len) -> Float { return kern::math::div(1.0, len); }, len));
    walk = vector_scale(u, kern::to<Float>(p.dir));
    right = vector_make(vector_break(walk).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(walk).x), 0.0);
    c = vector_add(vector_add(a, vector_scale(u, kern::formula([](Float along, Float len) -> Float { return (along < 0.0 ? 0.0 : (len < along ? len : along)); }, p.along, len))), vector_scale(right, kern::formula([](Float side, Float off) -> Float { return (side * off); }, p.side, p.off)));
    out = vector_make(vector_break(c).x, vector_break(c).y, 0.0);
    heading = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(walk).y, vector_break(walk).x);
    return {true};
  }

  struct People_free_moveResult { Bool Result = false; };
  People_free_moveResult People_free_move(People_Person& p, Int w = 0, Vector dir = Vector{0.0, 0.0, 0.0}, Float step = 0.0, Bool keep = false) {
    Vector dir_ = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Float item_5 = 0.0;
    Float item_6 = 0.0;
    Float item_7 = 0.0;
    Fixed<Float, 7> items = Fixed<Float, 7>{};
    Bool skip = false;
    Float ang = 0.0;
    Float c = 0.0;
    Float s = 0.0;
    Vector d = Vector{0.0, 0.0, 0.0};
    dir_ = dir;
    l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(dir_).x, vector_break(dir_).y);
    if (logic_or(math_compare<"<">(l, 1e-05), math_compare<"<=">(step, 0.0))) {
      return {true};
    }
    dir_ = vector_scale(dir_, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l));
    item = 0.0;
    item_2 = 0.5;
    item_3 = -0.5;
    item_4 = 1.0;
    item_5 = -1.0;
    item_6 = 1.6;
    item_7 = -1.6;
    items = list_make<Float, "7">(item, item_2, item_3, item_4, item_5, item_6, item_7);
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it12.next();) {
      ang = items[n12.index];
      skip = false;
      c = kern::formula([](Float ang) -> Float { return kern::math::cos(ang); }, ang);
      s = kern::formula([](Float ang) -> Float { return kern::math::sin(ang); }, ang);
      d = vector_make(kern::formula([](Float x, Float c, Float y, Float s) -> Float { return ((x * c) - (y * s)); }, vector_break(dir_).x, c, vector_break(dir_).y, s), kern::formula([](Float x, Float s, Float y, Float c) -> Float { return ((x * s) + (y * c)); }, vector_break(dir_).x, s, vector_break(dir_).y, c), 0.0);
      if (struct_break<the_RayResult_>(common::cast(w, vector_add(p.pos, Vector{0.0, 0.0, 1.0}), vector_make(vector_break(d).x, vector_break(d).y, 0.0), kern::formula([](Float step, Float kCapsuleR) -> Float { return (step + kCapsuleR); }, step, kCapsuleR), struct_make<physics_Body>(Int64{0x0}), Int64{0x1}).Result).hit) {
        skip = true;
      }
      if (logic_not(skip)) {
        p.pos = vector_make(kern::formula([](Float x, Float x_2, Float step) -> Float { return (x + (x_2 * step)); }, vector_break(p.pos).x, vector_break(d).x, step), vector_break(p.pos).y, vector_break(p.pos).z);
        p.pos = vector_make(vector_break(p.pos).x, kern::formula([](Float y, Float y_2, Float step) -> Float { return (y + (y_2 * step)); }, vector_break(p.pos).y, vector_break(d).y, step), vector_break(p.pos).z);
        if (logic_and(keep, math_compare<">">(kern::formula([](Float ang) -> Float { return (ang != 0.0); }, ang), 0.5))) {
          p.flee_dir = d;
        }
        return {true};
      }
    }
    return {false};
  }

  struct People_add_officerResult { I32 Result = 0; };
  People_add_officerResult People_add_officer(People& self, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, I32 unit = 0, U32 seed = 0) {
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p.kind = People_Kind::officer;
    p.state = People_State::officer;
    p.pos = at;
    p.yaw = yaw;
    p.hp = 100.0;
    p.unit = unit;
    p.seed = seed;
    p.shirt = Vector{0.14, 0.19, 0.31};
    p.pants = Vector{0.1, 0.13, 0.19};
    p.skin = list_get<Vector>(kSkins, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(seed, 5.0); }, kern::to<Float>(seed))));
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it13.next();) {
      if (enum_equal<People_State>(self.people_[n13.index].state, People_State::gone)) {
        self.people_[n13.index] = p;
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n13.index)))};
      }
    }
    list_add(self.people_, p);
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<People_Person>(self.people_))))};
  }

  struct People_add_citizenResult { I32 Result = 0; };
  People_add_citizenResult People_add_citizen(People& self, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, U32 seed = 0) {
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p.kind = People_Kind::citizen;
    p.state = People_State::walk;
    p.pos = at;
    p.yaw = yaw;
    p.seed = seed;
    p.speed = kern::formula([](Float seed) -> Float { return (1.0 + kern::math::div(kern::math::mod(kern::math::shr(seed, 7.0), 66.0), 100.0)); }, kern::to<Float>(seed));
    p.fitness = kern::formula([](Float seed) -> Float { return (0.75 + kern::math::div(kern::math::mod(kern::math::shr(seed, 11.0), 51.0), 100.0)); }, kern::to<Float>(seed));
    p.shirt = list_get<Vector>(kShirts, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(kern::math::shr(seed, 3.0), 10.0); }, kern::to<Float>(seed))));
    p.pants = list_get<Vector>(kPants, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(kern::math::shr(seed, 9.0), 6.0); }, kern::to<Float>(seed))));
    p.skin = list_get<Vector>(kSkins, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(kern::math::shr(seed, 13.0), 5.0); }, kern::to<Float>(seed))));
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it14.next();) {
      if (enum_equal<People_State>(self.people_[n14.index].state, People_State::gone)) {
        self.people_[n14.index] = p;
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n14.index)))};
      }
    }
    list_add(self.people_, p);
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<People_Person>(self.people_))))};
  }

  struct People_add_scriptedResult { I32 Result = 0; };
  People_add_scriptedResult People_add_scripted(People& self, People_Kind k = People_Kind::citizen, Vector at = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float hp = 0.0, U32 seed = 0, Vector shirt = Vector{0.0, 0.0, 0.0}, Vector pants = Vector{0.0, 0.0, 0.0}, Vector skin = Vector{0.0, 0.0, 0.0}, U8 role = 0) {
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p.kind = k;
    p.state = People_State::officer;
    p.pos = at;
    p.yaw = yaw;
    p.hp = hp;
    p.seed = seed;
    p.shirt = shirt;
    p.pants = pants;
    p.skin = skin;
    p.role = role;
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it15.next();) {
      if (enum_equal<People_State>(self.people_[n15.index].state, People_State::gone)) {
        self.people_[n15.index] = p;
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n15.index)))};
      }
    }
    list_add(self.people_, p);
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<People_Person>(self.people_))))};
  }

  struct People_countResult { U32 Result = 0; };
  KERN_PURE People_countResult People_count(const People& self, People_Kind k = People_Kind::citizen, Bool alive_only = false) {
    U32 n = 0;
    n = 0;
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it16.next();) {
      if (logic_and(logic_and(enum_equal<People_Kind>(self.people_[n16.index].kind, k), logic_not(enum_equal<People_State>(self.people_[n16.index].state, People_State::gone))), logic_or(logic_not(alive_only), logic_not(enum_equal<People_State>(self.people_[n16.index].state, People_State::down))))) {
        n += 1;
      }
    }
    return {n};
  }

  struct near_anyResult { Float Result = 0.0; };
  /// the nearest player (MP: the host's people live around every player)
  KERN_PURE near_anyResult near_any(const PeopleView& v, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float d = 0.0;
    d = dist2d(p, v.focus).Result;
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(v.foci))))); it17.next();) {
      d = kern::formula([](Float d, Float the_result) -> Float { return kern::math::min(d, the_result); }, d, dist2d(p, v.foci[n17.index]).Result);
    }
    return {d};
  }

  void People_dress(People_Person& p) {
    U32 seed = 0;
    seed = p.seed;
    p.speed = kern::formula([](Float seed) -> Float { return (1.0 + kern::math::div(kern::math::mod(kern::math::shr(seed, 7.0), 66.0), 100.0)); }, kern::to<Float>(seed));
    p.fitness = kern::formula([](Float seed) -> Float { return (0.75 + kern::math::div(kern::math::mod(kern::math::shr(seed, 11.0), 51.0), 100.0)); }, kern::to<Float>(seed));
    p.shirt = list_get<Vector>(kShirts, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(kern::math::shr(seed, 3.0), 10.0); }, kern::to<Float>(seed))));
    p.pants = list_get<Vector>(kPants, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(kern::math::shr(seed, 9.0), 6.0); }, kern::to<Float>(seed))));
    p.skin = list_get<Vector>(kSkins, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(kern::math::shr(seed, 13.0), 5.0); }, kern::to<Float>(seed))));
  }

  void People_apply_snapshot(People& self, const List<People_NetPerson>& list, Float now = 0.0) {
    List<Bool> seen = List<Bool>{};
    List<Bool> made = List<Bool>{};
    Bool skip = false;
    I32 at = 0;
    Int i = 0;
    Bool stop = false;
    Bool host_down = false;
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    Bool placed = false;
    Int i_2 = 0;
    Bool stop_2 = false;
    Int i_3 = 0;
    Bool skip_2 = false;
    made = List<Bool>{};
    list_resize(made, list_length<People_Person>(self.people_), false);
    seen = made;
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_NetPerson>(list))))); it18.next();) {
      skip = false;
      at = -1;
      i = 0;
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<People_Person>(self.people_))), 0.5)))) {
        if (logic_and(logic_and(enum_equal<People_Kind>(self.people_[i].kind, People_Kind::citizen), math_compare<"==">(kern::to<Float>(self.people_[i].net), kern::to<Float>(list[n18.index].idx))), logic_not(enum_equal<People_State>(self.people_[i].state, People_State::gone)))) {
          at = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i)));
          stop = true;
        }
        if (logic_not(stop)) {
          i += 1;
        }
      }
      host_down = math_compare<"==">(kern::to<Float>(list[n18.index].state), kern::to<Float>(enum_index<People_State>(People_State::down)));
      if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
        if (host_down) {
          skip = true;
        }
        if (logic_not(skip)) {
          p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
          p.kind = People_Kind::citizen;
          p.seed = list[n18.index].seed;
          People_dress(p);
          p.net = list[n18.index].idx;
          p.net_to = list[n18.index].pos;
          p.pos = p.net_to;
          p.yaw = list[n18.index].yaw;
          p.state = enum_from_index<People_State>(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a <= a_2) ? a : 0.0); }, kern::to<Float>(list[n18.index].state), kern::to<Float>(enum_index<People_State>(People_State::fight))))).value;
          placed = false;
          i_2 = 0;
          stop_2 = false;
          while (loop(logic_and(logic_not(stop_2), math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<People_Person>(self.people_))), 0.5)))) {
            if (enum_equal<People_State>(self.people_[i_2].state, People_State::gone)) {
              self.people_[i_2] = p;
              at = kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::wrap32s(i_2); }, kern::to<Float>(i_2)));
              placed = true;
              stop_2 = true;
            }
            if (logic_not(stop_2)) {
              i_2 += 1;
            }
          }
          if (logic_not(placed)) {
            list_add(self.people_, p);
            list_add(seen, false);
            at = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<People_Person>(self.people_))));
          }
        }
      }
      if (logic_not(skip)) {
        seen[at] = true;
        if (math_compare<"!=">(kern::to<Float>(self.people_[at].seed), kern::to<Float>(list[n18.index].seed))) {
          self.people_[at].seed = list[n18.index].seed;
          People_dress(self.people_[at]);
          self.people_[at].pos = list[n18.index].pos;
        }
        if (enum_equal<People_State>(self.people_[at].state, People_State::down)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (host_down) {
            People_kill(self, at, vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, list[n18.index].yaw), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, list[n18.index].yaw), 0.0), false, now, false);
            skip = true;
          }
          if (logic_not(skip)) {
            if (logic_and(math_compare<">">(kern::formula([](Float a) -> Float { return (kern::math::band(a, 1.0) != 0.0); }, kern::to<Float>(list[n18.index].flags)), 0.5), math_compare<">=">(now, self.people_[at].knocked_until))) {
              people::People_knock(self, at, vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, list[n18.index].yaw), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, list[n18.index].yaw), 0.5), now);
              skip = true;
            }
            if (logic_not(skip)) {
              self.people_[at].net_to = list[n18.index].pos;
              self.people_[at].net_at = list[n18.index].at;
              self.people_[at].v = list[n18.index].v;
              if (math_compare<">=">(now, self.people_[at].knocked_until)) {
                self.people_[at].state = enum_from_index<People_State>(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a <= a_2) ? a : 0.0); }, kern::to<Float>(list[n18.index].state), kern::to<Float>(enum_index<People_State>(People_State::fight))))).value;
                if (math_compare<">">(dist2d(self.people_[at].pos, list[n18.index].pos).Result, 6.0)) {
                  self.people_[at].pos = list[n18.index].pos;
                }
                self.people_[at].yaw = list[n18.index].yaw;
              }
            }
          }
        }
      }
    }
    i_3 = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i_3, Float a) -> Float { return ((i_3 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_3 < a)); }, kern::to<Float>(i_3), kern::to<Float>(list_length<Bool>(seen))), 0.5), math_compare<">">(kern::formula([](Float i_3, Float a) -> Float { return ((i_3 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_3 < a)); }, kern::to<Float>(i_3), kern::to<Float>(list_length<People_Person>(self.people_))), 0.5)))) {
      skip_2 = false;
      if (logic_or(logic_or(seen[i_3], logic_not(enum_equal<People_Kind>(self.people_[i_3].kind, People_Kind::citizen))), math_compare<"<">(kern::to<Float>(self.people_[i_3].net), 0.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (logic_or(enum_equal<People_State>(self.people_[i_3].state, People_State::down), math_compare<"<">(now, self.people_[i_3].knocked_until))) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          self.people_[i_3].state = People_State::gone;
        }
      }
      i_3 += 1;
    }
  }

  void People_mate_hurt(People& self, I32 index = 0, U32 seed = 0, Float damage = 0.0, People_HurtBy by = People_HurtBy::round_, Vector push = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(index), 0.0), math_compare<">=">(kern::to<Float>(index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(self.people_)))))))), logic_not(math_compare<">">(damage, 0.0)))) {
      return;
    }
    if (logic_or(logic_or(logic_or(logic_or(logic_not(enum_equal<People_Kind>(self.people_[index].kind, People_Kind::citizen)), math_compare<"!=">(kern::to<Float>(self.people_[index].seed), kern::to<Float>(seed))), enum_equal<People_State>(self.people_[index].state, People_State::down)), enum_equal<People_State>(self.people_[index].state, People_State::gone)), math_compare<"<">(now, self.people_[index].knocked_until))) {
      return;
    }
    self.people_[index].hp = kern::formula([](Float a, Float damage) -> Float { return (a - damage); }, self.people_[index].hp, damage);
    self.people_[index].hurt_at = now;
    if (math_compare<"<=">(self.people_[index].hp, 0.0)) {
      People_kill(self, index, push, false, now, enum_equal<People_HurtBy>(by, People_HurtBy::vehicle));
      return;
    }
    if (enum_equal<People_HurtBy>(by, People_HurtBy::vehicle)) {
      list_add(self.falls_, struct_make<People_Fall>(index, push, true, true));
      self.people_[index].knocked_until = kern::formula([](Float now) -> Float { return (now + 6.0); }, now);
      self.people_[index].v = 0.0;
      People_frighten(self, self.people_[index].pos, 8.0, now, 1.4);
      return;
    }
    if (logic_and(enum_equal<People_HurtBy>(by, People_HurtBy::blow), math_compare<">">(self.people_[index].hp, 35.0))) {
      self.people_[index].v = 0.0;
    } else {
      people::People_knock(self, index, push, now);
    }
    People_frighten(self, self.people_[index].pos, 3.0, now, 1.6);
  }

  struct People_shotResult { I32 Result = 0; };
  People_shotResult People_shot(People& self, Vector& to, Vector from = Vector{0.0, 0.0, 0.0}, Float damage = 0.0, Bool by_player = false, Float now = 0.0, Float melee_ns = 0.0, Float self_r = 0.0) {
    I32 best = 0;
    Float best_s = 0.0;
    Bool head = false;
    Vector hit_point = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Float s = 0.0;
    BodyPart part = BodyPart::none;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Float hd = 0.0;
    Vector flat = Vector{0.0, 0.0, 0.0};
    Float hp_before = 0.0;
    Float k = 0.0;
    Float step = 0.0;
    Float dmg = 0.0;
    Float dealt = 0.0;
    Float hits = 0.0;
    best = -1;
    best_s = 2.0;
    head = false;
    hit_point = Vector{0.0, 0.0, 0.0};
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it25.next();) {
      skip = false;
      if (logic_or(enum_equal<People_State>(self.people_[n25.index].state, People_State::down), enum_equal<People_State>(self.people_[n25.index].state, People_State::gone))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"<">(dist2d(self.people_[n25.index].pos, from).Result, self_r)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_and(enum_equal<People_Kind>(self.people_[n25.index].kind, People_Kind::citizen), math_compare<"<">(now, self.people_[n25.index].knocked_until))) {
            skip = true;
          }
          if (logic_not(skip)) {
            s = 2.0;
            n26 = person_hit::hit(s, self.people_[n25.index].pos, self.people_[n25.index].yaw, self.people_[n25.index].v, from, vector_sub(to, from), 1.08, 1.0);
            part = n26.Result;
            if (logic_and(logic_not(enum_equal<BodyPart>(part, BodyPart::none)), math_compare<"<">(s, best_s))) {
              best_s = s;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n25.index)));
              head = enum_equal<BodyPart>(part, BodyPart::head);
              hit_point = vector_add(from, vector_scale(vector_sub(to, from), s));
            }
          }
        }
      }
    }
    self.last_head_ = false;
    if (math_compare<"<">(kern::to<Float>(best), 0.0)) {
      return {-1};
    }
    to = hit_point;
    self.last_head_ = head;
    if (logic_or(math_compare<">">(damage, 0.0), math_compare<">=">(melee_ns, 0.0))) {
      self.people_[best].hurt_at = now;
    }
    if (logic_and(by_player, enum_equal<People_Kind>(self.people_[best].kind, People_Kind::citizen))) {
      self.people_[best].provoked_at = now;
      self.people_[best].provoked_hit = true;
    }
    dir = vector_sub(to, from);
    if (logic_and(enum_equal<People_Kind>(self.people_[best].kind, People_Kind::citizen), math_compare<">">(damage, 0.0))) {
      hd = kern::formula([](Float x, Float y) -> Float { return kern::math::max(0.001, kern::math::sqrt((x * x) + (y * y))); }, vector_break(dir).x, vector_break(dir).y);
      flat = vector_make(kern::formula([](Float x, Float hd) -> Float { return kern::math::div(x, hd); }, vector_break(dir).x, hd), kern::formula([](Float y, Float hd) -> Float { return kern::math::div(y, hd); }, vector_break(dir).y, hd), 0.0);
      hp_before = self.people_[best].hp;
      if (math_compare<">=">(melee_ns, 0.0)) {
        k = kern::formula([](Float melee_ns) -> Float { return kern::math::max(0.5, kern::math::div(melee_ns, 30.0)); }, melee_ns);
        self.people_[best].hp = kern::formula([](Float a, Float damage) -> Float { return (a - damage); }, self.people_[best].hp, damage);
        if (math_compare<"<=">(self.people_[best].hp, 0.0)) {
          People_kill(self, best, dir, by_player, now, false);
        } else if (math_compare<"<=">(self.people_[best].hp, 35.0)) {
          people::People_knock(self, best, vector_add(vector_scale(flat, kern::formula([](Float k) -> Float { return (2.0 * k); }, k)), Vector{0.0, 0.0, 0.9}), now);
        } else {
          step = kern::formula([](Float k) -> Float { return kern::math::min(0.6, (0.22 * k)); }, k);
          self.people_[best].pos = vector_add(self.people_[best].pos, vector_scale(flat, step));
          self.people_[best].v = 0.0;
        }
      } else {
        dmg = kern::formula([](Float head, Float damage) -> Float { return ((head != 0.0) ? ((damage >= 26.0) ? 999.0 : 165.0) : ((damage >= 90.0) ? 200.0 : 55.0)); }, kern::to<Float>(head), damage);
        self.people_[best].hp = kern::formula([](Float a, Float dmg) -> Float { return (a - dmg); }, self.people_[best].hp, dmg);
        if (math_compare<"<=">(self.people_[best].hp, 0.0)) {
          People_kill(self, best, dir, by_player, now, false);
        } else {
          people::People_knock(self, best, vector_add(vector_scale(flat, 2.5), Vector{0.0, 0.0, 1.4}), now);
        }
      }
      if (logic_not(enum_equal<People_State>(self.people_[best].state, People_State::down))) {
        if (by_player) {
          list_add(self.reports_, struct_make<CrimeReport>(wanted_Crime::person_hit, self.people_[best].pos, true));
        }
        People_frighten(self, self.people_[best].pos, 3.0, now, 1.6);
        if (logic_and(logic_and(by_player, self.puppets_), math_compare<">=">(kern::to<Float>(self.people_[best].net), 0.0))) {
          list_add(self.hurts_, struct_make<People_Hurt>(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(self.people_[best].net))), self.people_[best].seed, kern::formula([](Float hp_before, Float a) -> Float { return (hp_before - a); }, hp_before, self.people_[best].hp), logic_select<People_HurtBy>(math_compare<">=">(melee_ns, 0.0), People_HurtBy::blow, People_HurtBy::round_), logic_select<Vector>(math_compare<">=">(melee_ns, 0.0), vector_add(vector_scale(flat, kern::formula([](Float melee_ns) -> Float { return (2.0 * kern::math::max(0.5, kern::math::div(melee_ns, 30.0))); }, melee_ns)), Vector{0.0, 0.0, 0.9}), vector_add(vector_scale(flat, 2.5), Vector{0.0, 0.0, 1.4}))));
        }
      }
      return {best};
    }
    dealt = kern::formula([](Float damage, Float head) -> Float { return (damage * ((head != 0.0) ? 3.0 : 1.0)); }, damage, kern::to<Float>(head));
    if (logic_and(logic_and(enum_equal<People_Kind>(self.people_[best].kind, People_Kind::officer), math_compare<">">(damage, 0.0)), math_compare<"<">(melee_ns, 0.0))) {
      hits = kern::formula([](Float damage, Float head) -> Float { return ((damage >= 26.0) ? ((head != 0.0) ? 10.0 : 6.0) : ((head != 0.0) ? 3.0 : 1.0)); }, damage, kern::to<Float>(head));
      dealt = kern::formula([](Float hits) -> Float { return (25.0 * hits); }, hits);
    }
    if (self.people_[best].remote) {
      if (logic_and(by_player, math_compare<">">(dealt, 0.0))) {
        list_add(self.remote_hits_, struct_make<People_RemoteHit>(best, dealt));
      }
      return {best};
    }
    self.people_[best].hp = kern::formula([](Float a, Float dealt) -> Float { return (a - dealt); }, self.people_[best].hp, dealt);
    if (math_compare<"<=">(self.people_[best].hp, 0.0)) {
      People_kill(self, best, dir, by_player, now, false);
    } else {
      if (logic_and(by_player, enum_equal<People_Kind>(self.people_[best].kind, People_Kind::citizen))) {
        list_add(self.reports_, struct_make<CrimeReport>(wanted_Crime::person_hit, self.people_[best].pos, true));
      }
      if (enum_equal<People_Kind>(self.people_[best].kind, People_Kind::citizen)) {
        People_frighten(self, self.people_[best].pos, 3.0, now, 1.6);
      }
    }
    return {best};
  }

  void People_blast(People& self, Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float damage = 0.0, Bool by_player = false, Float now = 0.0) {
    Bool skip = false;
    Float d = 0.0;
    Float t = 0.0;
    Float r = 0.0;
    Float d2 = 0.0;
    for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(self.people_))))); it27.next();) {
      skip = false;
      if (logic_or(logic_or(enum_equal<People_State>(self.people_[n27.index].state, People_State::down), enum_equal<People_State>(self.people_[n27.index].state, People_State::gone)), self.people_[n27.index].remote)) {
        skip = true;
      }
      if (logic_not(skip)) {
        d = vector_length(vector_sub(vector_add(self.people_[n27.index].pos, Vector{0.0, 0.0, 0.9}), at));
        if (math_compare<">=">(d, radius)) {
          skip = true;
        }
        if (logic_not(skip)) {
          t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float d, Float radius) -> Float { return (1.0 - kern::math::div(d, radius)); }, d, radius), 0.0, 1.0));
          if (enum_equal<People_Kind>(self.people_[n27.index].kind, People_Kind::citizen)) {
            if (logic_or(math_compare<"<=">(kern::formula([](Float t) -> Float { return ((t * t) * (3.0 - (2.0 * t))); }, t), 0.22), math_compare<"<">(now, self.people_[n27.index].knocked_until))) {
              skip = true;
            }
            if (logic_not(skip)) {
              self.people_[n27.index].hp = kern::formula([](Float a) -> Float { return (a - 55.0); }, self.people_[n27.index].hp);
              if (math_compare<"<=">(self.people_[n27.index].hp, 0.0)) {
                People_kill(self, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n27.index))), vector_sub(self.people_[n27.index].pos, at), by_player, now, false);
              } else {
                people::People_knock(self, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n27.index))), vector_sub(self.people_[n27.index].pos, at), now);
              }
              skip = true;
            }
          }
          if (logic_not(skip)) {
            if (enum_equal<People_Kind>(self.people_[n27.index].kind, People_Kind::officer)) {
              r = kern::formula([](Float radius) -> Float { return (radius < 2.0 ? 2.0 : (60.0 < radius ? 60.0 : radius)); }, radius);
              d2 = dist2d(self.people_[n27.index].pos, at).Result;
              if (math_compare<"<">(d2, r)) {
                self.people_[n27.index].hp = kern::formula([](Float a, Float d2, Float r) -> Float { return (a - (((100.0 * (1.0 - kern::math::div(d2, r))) * 1.6) + 20.0)); }, self.people_[n27.index].hp, d2, r);
              }
            } else {
              self.people_[n27.index].hp = kern::formula([](Float a, Float damage, Float t) -> Float { return (a - (((damage * t) * t) * (3.0 - (2.0 * t)))); }, self.people_[n27.index].hp, damage, t);
            }
            if (math_compare<"<=">(self.people_[n27.index].hp, 0.0)) {
              People_kill(self, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n27.index))), vector_sub(self.people_[n27.index].pos, at), by_player, now, false);
            }
          }
        }
      }
    }
    People_frighten(self, at, kern::formula([](Float radius) -> Float { return (radius * 2.5); }, radius), now, 1.6);
  }

  struct walk_seg_lenResult { Float Result = 0.0; };
  KERN_PURE walk_seg_lenResult walk_seg_len(const Lane& L_2, I32 k = 0) {
    Vector d = Vector{0.0, 0.0, 0.0};
    d = vector_sub(L_2.points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], L_2.points[k]);
    return {kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y)};
  }

  void People_walk(People& self, People_Person& p, const PeopleView& view, const CityStream& city, Rng& rng, Float dt = 0.0, Float now = 0.0) {
    List<Lane> lanes = List<Lane>{};
    Int w = 0;
    Float top = 0.0;
    Float best = 0.0;
    Bool skip = false;
    Lane L = Lane{List<Vector>{}, 6.0, Int64{0x0}, true};
    Int k = 0;
    Bool skip_2 = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    Float dd = 0.0;
    Float cross = 0.0;
    Float right_of_lane = 0.0;
    Float hurry = 0.0;
    Lane L_2 = Lane{List<Vector>{}, 6.0, Int64{0x0}, true};
    I32 n = 0;
    Vector target = Vector{0.0, 0.0, 0.0};
    Float heading = 0.0;
    Float behind = 0.0;
    Bool stop = false;
    Bool stop_2 = false;
    Vector end = Vector{0.0, 0.0, 0.0};
    I32 next = 0;
    I32 next_dir = 0;
    U32 start = 0;
    Int k_2 = 0;
    Bool skip_3 = false;
    Int j = 0;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector z = Vector{0.0, 0.0, 0.0};
    Lane N = Lane{List<Vector>{}, 6.0, Int64{0x0}, true};
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Vector to = Vector{0.0, 0.0, 0.0};
    Float dl = 0.0;
    Float step = 0.0;
    lanes = city.lanes_;
    w = view.physics;
    if (enum_equal<People_State>(p.state, People_State::flee)) {
      if (math_compare<"<">(now, p.flee_until)) {
        top = kern::formula([](Float now, Float burst_until, Float flee_speed, Float fitness) -> Float { return ((now < burst_until) ? kern::math::max(flee_speed, kern::math::min(8.5, (6.6 * fitness))) : flee_speed); }, now, p.burst_until, p.flee_speed, p.fitness);
        p.v = kern::formula([](Float v, Float a) -> Float { return (v + a); }, p.v, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float top, Float v) -> Float { return (top - v); }, top, p.v), kern::formula([](Float dt) -> Float { return ((-3.0) * dt); }, dt), kern::formula([](Float dt) -> Float { return (4.2 * dt); }, dt)));
        n28 = People_free_move(p, w, p.flee_dir, kern::formula([](Float v, Float dt) -> Float { return (v * dt); }, p.v, dt), true);
        if (logic_not(n28.Result)) {
          p.flee_dir = vector_scale(p.flee_dir, -1.0);
        }
        p.yaw = approach_angle(p.yaw, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(p.flee_dir).y, vector_break(p.flee_dir).x), kern::formula([](Float dt) -> Float { return (dt * 6.0); }, dt)).Result;
        return;
      }
      best = 30.0;
      for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it29.next();) {
        skip = false;
        L = lanes[n29.index];
        if (logic_not(L.alive)) {
          skip = true;
        }
        if (logic_not(skip)) {
          k = 0;
          while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(L.points))), 0.5))) {
            skip_2 = false;
            a = L.points[k];
            b = L.points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
            d = vector_sub(b, a);
            l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
            if (math_compare<"<">(l2, 1.0)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(p.pos).x, vector_break(a).x, vector_break(d).x, vector_break(p.pos).y, vector_break(a).y, vector_break(d).y, l2), 0.0, 1.0));
              q = vector_add(a, vector_scale(d, t));
              dd = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(p.pos).x, vector_break(q).x, vector_break(p.pos).y, vector_break(q).y);
              if (math_compare<"<">(dd, best)) {
                best = dd;
                p.lane = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n29.index)));
                p.seg = kern::to<Int>(kern::formula([](Float k) -> Float { return kern::math::wrap32s(k); }, kern::to<Float>(k)));
                p.along = kern::formula([](Float t, Float l2) -> Float { return (t * kern::math::sqrt(l2)); }, t, l2);
                n30 = common::Rng_unit(rng);
                p.dir = kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 1.0 : (-1.0)); }, n30.Result));
                cross = kern::formula([](Float x, Float y, Float y_2, Float y_3, Float x_2, Float x_3) -> Float { return ((x * (y - y_2)) - (y_3 * (x_2 - x_3))); }, vector_break(d).x, vector_break(p.pos).y, vector_break(a).y, vector_break(d).y, vector_break(p.pos).x, vector_break(a).x);
                right_of_lane = kern::formula([](Float cross) -> Float { return ((cross < 0.0) ? 1.0 : (-1.0)); }, cross);
                p.side = kern::formula([](Float right_of_lane, Float dir) -> Float { return (right_of_lane * dir); }, right_of_lane, kern::to<Float>(p.dir));
                p.off = kern::formula([](Float width, Float kPavement) -> Float { return ((width * 0.5) + kPavement); }, L.width, kPavement);
              }
            }
            k += 1;
          }
        }
      }
      p.state = People_State::wait;
      n31 = common::Rng_unit(rng);
      p.wait_until = kern::formula([](Float now, Float the_result) -> Float { return ((now + 0.8) + (the_result * 1.5)); }, now, n31.Result);
      return;
    }
    if (enum_equal<People_State>(p.state, People_State::wait)) {
      p.v = kern::formula([](Float v, Float dt) -> Float { return kern::math::max(0.0, (v - (3.0 * dt))); }, p.v, dt);
      if (math_compare<">=">(now, p.wait_until)) {
        p.state = People_State::walk;
      }
      return;
    }
    hurry = kern::formula([](Float now, Float shaken_until) -> Float { return ((now < shaken_until) ? 1.45 : 1.0); }, now, p.shaken_until);
    p.v = kern::formula([](Float v, Float a) -> Float { return (v + a); }, p.v, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float speed, Float hurry, Float v) -> Float { return ((speed * hurry) - v); }, p.speed, hurry, p.v), kern::formula([](Float dt) -> Float { return ((-3.0) * dt); }, dt), kern::formula([](Float dt) -> Float { return (1.5 * dt); }, dt)));
    if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(p.lane), 0.0), math_compare<">=">(kern::to<Float>(p.lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(lanes)))))))), logic_not(lanes[p.lane].alive))) {
      p.state = People_State::gone;
      return;
    }
    L_2 = lanes[p.lane];
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Vector>(L_2.points))));
    target = Vector{0.0, 0.0, 0.0};
    heading = 0.0;
    n32 = People_route_point(p, lanes, target, heading);
    behind = dist2d(target, p.pos).Result;
    if (math_compare<"<">(behind, 1.6)) {
      p.along = kern::formula([](Float along, Float dir, Float v, Float dt) -> Float { return (along + ((dir * v) * dt)); }, p.along, kern::to<Float>(p.dir), p.v, dt);
      stop = false;
      while (loop(logic_and(logic_not(stop), logic_and(math_compare<">">(kern::to<Float>(p.dir), 0.0), math_compare<">">(p.along, walk_seg_len(L_2, p.seg).Result))))) {
        p.along = kern::formula([](Float along, Float the_result) -> Float { return (along - the_result); }, p.along, walk_seg_len(L_2, p.seg).Result);
        p.seg = kern::to<Int>(kern::formula([](Float seg) -> Float { return (seg + 1.0); }, kern::to<Float>(p.seg)));
        if (math_compare<">=">(kern::to<Float>(p.seg), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))) {
          stop = true;
        }
      }
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), logic_and(math_compare<"<">(kern::to<Float>(p.dir), 0.0), math_compare<"<">(p.along, 0.0))))) {
        p.seg = kern::to<Int>(kern::formula([](Float seg) -> Float { return (seg - 1.0); }, kern::to<Float>(p.seg)));
        if (math_compare<"<">(kern::to<Float>(p.seg), 0.0)) {
          stop_2 = true;
        }
        if (logic_not(stop_2)) {
          p.along = kern::formula([](Float along, Float the_result) -> Float { return (along + the_result); }, p.along, walk_seg_len(L_2, p.seg).Result);
        }
      }
      if (logic_or(math_compare<"<">(kern::to<Float>(p.seg), 0.0), math_compare<">=">(kern::to<Float>(p.seg), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n)))))))) {
        end = logic_select<Vector>(math_compare<"<">(kern::to<Float>(p.seg), 0.0), L_2.points[0], L_2.points[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(L_2.points))))]);
        next = -1;
        next_dir = 1;
        n33 = common::Rng_next(rng);
        start = n33.Result;
        k_2 = 0;
        while (loop(logic_and(math_compare<">">(kern::formula([](Float k_2, Float a) -> Float { return ((k_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (k_2 < a)); }, kern::to<Float>(k_2), kern::to<Float>(list_length<Lane>(lanes))), 0.5), math_compare<"<">(kern::to<Float>(next), 0.0)))) {
          skip_3 = false;
          j = kern::to<Int>(kern::formula([](Float start, Float k_2, Float a) -> Float { return kern::math::mod((start + k_2), a); }, kern::to<Float>(start), kern::to<Float>(k_2), kern::to<Float>(list_length<Lane>(lanes))));
          if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32s(j); }, kern::to<Float>(j))))), kern::to<Float>(p.lane)), logic_not(lanes[j].alive)), math_compare<"<">(kern::to<Float>(list_length<Vector>(lanes[j].points)), 2.0))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            a_2 = lanes[j].points[0];
            z = lanes[j].points[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(lanes[j].points))))];
            if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a_2).x, vector_break(end).x, vector_break(a_2).y, vector_break(end).y), 4.0)) {
              next = kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32s(j); }, kern::to<Float>(j)));
              next_dir = 1;
            } else if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(z).x, vector_break(end).x, vector_break(z).y, vector_break(end).y), 4.0)) {
              next = kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32s(j); }, kern::to<Float>(j)));
              next_dir = -1;
            }
          }
          k_2 += 1;
        }
        if (math_compare<">=">(kern::to<Float>(next), 0.0)) {
          p.lane = next;
          p.dir = next_dir;
          N = lanes[next];
          p.seg = kern::to<Int>(kern::formula([](Float next_dir, Float a) -> Float { return ((next_dir > 0.0) ? 0.0 : (kern::math::wrap32s(a) - 2.0)); }, kern::to<Float>(next_dir), kern::to<Float>(list_length<Vector>(N.points))));
          d_2 = vector_sub(N.points[kern::to<Int>(kern::formula([](Float seg) -> Float { return (seg + 1.0); }, kern::to<Float>(p.seg)))], N.points[p.seg]);
          p.along = kern::formula([](Float next_dir, Float x, Float y) -> Float { return ((next_dir > 0.0) ? 0.0 : kern::math::sqrt((x * x) + (y * y))); }, kern::to<Float>(next_dir), vector_break(d_2).x, vector_break(d_2).y);
          p.off = kern::formula([](Float width, Float kPavement) -> Float { return ((width * 0.5) + kPavement); }, N.width, kPavement);
        } else {
          p.dir = kern::to<Int>(kern::formula([](Float dir) -> Float { return (-dir); }, kern::to<Float>(p.dir)));
          p.side = kern::formula([](Float side) -> Float { return (-side); }, p.side);
          p.seg = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(p.seg), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 2.0); }, kern::to<Float>(n))))))));
          p.along = kern::formula([](Float dir, Float the_result) -> Float { return ((dir > 0.0) ? 0.0 : the_result); }, kern::to<Float>(p.dir), walk_seg_len(L_2, p.seg).Result);
        }
        n34 = common::Rng_unit(rng);
        if (math_compare<"<">(n34.Result, 0.25)) {
          p.state = People_State::wait;
          n35 = common::Rng_unit(rng);
          p.wait_until = kern::formula([](Float now, Float the_result) -> Float { return ((now + 1.0) + (the_result * 2.5)); }, now, n35.Result);
        }
      }
      n36 = People_route_point(p, lanes, target, heading);
    }
    to = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(target).x, vector_break(p.pos).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(target).y, vector_break(p.pos).y), 0.0);
    dl = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(to).x, vector_break(to).y);
    if (math_compare<">">(dl, 0.02)) {
      step = kern::formula([](Float dl, Float v, Float dt, Float behind) -> Float { return kern::math::min(dl, ((v * dt) * ((behind < 1.6) ? 1.25 : 1.6))); }, dl, p.v, dt, behind);
      if (math_compare<"<">(behind, 1.6)) {
        p.pos = vector_make(kern::formula([](Float x, Float x_2, Float dl, Float step) -> Float { return (x + (kern::math::div(x_2, dl) * step)); }, vector_break(p.pos).x, vector_break(to).x, dl, step), vector_break(p.pos).y, vector_break(p.pos).z);
        p.pos = vector_make(vector_break(p.pos).x, kern::formula([](Float y, Float y_2, Float dl, Float step) -> Float { return (y + (kern::math::div(y_2, dl) * step)); }, vector_break(p.pos).y, vector_break(to).y, dl, step), vector_break(p.pos).z);
      } else {
        n37 = People_free_move(p, w, vector_make(kern::formula([](Float x, Float dl) -> Float { return kern::math::div(x, dl); }, vector_break(to).x, dl), kern::formula([](Float y, Float dl) -> Float { return kern::math::div(y, dl); }, vector_break(to).y, dl), 0.0), step, false);
      }
      p.yaw = approach_angle(p.yaw, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(to).y, vector_break(to).x), kern::formula([](Float dt) -> Float { return (dt * 4.5); }, dt)).Result;
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<25> { Int index = 0; } n25;
  person_hit::hitResult n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  People_free_moveResult n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  common::Rng_unitResult n30;
  common::Rng_unitResult n31;
  People_route_pointResult n32;
  common::Rng_nextResult n33;
  common::Rng_unitResult n34;
  common::Rng_unitResult n35;
  People_route_pointResult n36;
  People_free_moveResult n37;
};
