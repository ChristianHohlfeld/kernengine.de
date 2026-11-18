struct gaps_rules_cpp : virtual kern::Library, virtual gaps_rules {
  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST gaps_VisualProfile kVisualThird = gaps_VisualProfile{5.2, 7.0, 0.05, 0.18, 7.0, 0.08726646259971647, 0.24434609527920614, 7.5};
  KERN_CONST gaps_VisualProfile kVisualFollow = gaps_VisualProfile{9.0, 12.0, 0.025, 0.1, 9.0, 0.05235987755982989, 0.17453292519943295, 10.0};

  // ---- functions ----
  struct hash_textResult { Int Result = 0; };
  /// ---- world_critical_damage_fx.mjs
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

  struct unit_hashResult { Float Result = 0.0; };
  KERN_PURE unit_hashResult unit_hash(Int x = 0) {
    Int x_ = 0;
    x_ = x;
    x_ = kern::to<Int>(kern::formula([](Float x_) -> Float { return kern::math::bxor(x_, kern::math::shr(x_, 16.0)); }, kern::to<Float>(x_)));
    x_ = kern::to<Int>(kern::formula([](Float x_) -> Float { return kern::math::mul32(x_, 2146121005.0); }, kern::to<Float>(x_)));
    x_ = kern::to<Int>(kern::formula([](Float x_) -> Float { return kern::math::bxor(x_, kern::math::shr(x_, 15.0)); }, kern::to<Float>(x_)));
    x_ = kern::to<Int>(kern::formula([](Float x_) -> Float { return kern::math::mul32(x_, 2221713035.0); }, kern::to<Float>(x_)));
    x_ = kern::to<Int>(kern::formula([](Float x_) -> Float { return kern::math::bxor(x_, kern::math::shr(x_, 16.0)); }, kern::to<Float>(x_)));
    return {kern::formula([](Float x_) -> Float { return kern::math::div(x_, 4294967295.0); }, kern::to<Float>(x_))};
  }

  struct rgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rgbResult rgb(Int hex = 0) {
    return {vector_make(kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(kern::math::shr(hex, 16.0), 255.0), 255.0); }, kern::to<Float>(hex)), kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(kern::math::shr(hex, 8.0), 255.0), 255.0); }, kern::to<Float>(hex)), kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(hex, 255.0), 255.0); }, kern::to<Float>(hex)))};
  }

  void SmokeField_emit(gaps_SmokeField& self, gaps_SmokeEffect& e, Float now = 0.0) {
    Int at = 0;
    Int i = 0;
    Bool stop = false;
    Float progress = 0.0;
    Int serial = 0;
    Int seed = 0;
    Text a = "";
    Float a_2 = 0.0;
    Float side = 0.0;
    Bool ember = false;
    if (math_compare<"==">(kern::to<Float>(list_length<gaps_Puff>(self.puffs_)), 0.0)) {
      return;
    }
    at = -1;
    i = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<gaps_Puff>(self.puffs_))), 0.5)))) {
      if (logic_not(self.puffs_[i].alive)) {
        at = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i)));
        stop = true;
      }
      if (logic_not(stop)) {
        i += 1;
      }
    }
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      at = 0;
      for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(self.puffs_))))); it12.next();) {
        if (math_compare<"<">(self.puffs_[n12.index].born, self.puffs_[at].born)) {
          at = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n12.index)));
        }
      }
    }
    progress = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float started, Float expires) -> Float { return kern::math::div((now - started), kern::math::max(0.001, (expires - started))); }, now, e.started, e.expires), 0.0, 1.0));
    serial = e.serial;
    e.serial = kern::to<Int>(kern::formula([](Float serial) -> Float { return kern::math::wrap32(serial + 1.0); }, kern::to<Float>(e.serial)));
    a = text_append(text_append(e.id, ":", ""), text_format<"%u">(-1, serial), "");
    seed = hash_text(a).Result;
    a_2 = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, unit_hash(seed).Result, kPi);
    side = kern::formula([](Float the_result, Float scale) -> Float { return ((the_result - 0.5) * (0.22 + (scale * 0.18))); }, unit_hash(kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(seed + 17.0); }, kern::to<Float>(seed)))).Result, e.scale);
    ember = logic_and(math_compare<">">(progress, 0.67), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float serial) -> Float { return kern::math::mod(serial, 5.0); }, kern::to<Float>(serial))))), 0.0));
    self.puffs_[at].alive = true;
    self.puffs_[at].ember = ember;
    self.puffs_[at].pos = vector_add(e.at, vector_make(kern::formula([](Float a_2, Float side) -> Float { return (kern::math::cos(a_2) * side); }, a_2, side), kern::formula([](Float a_2, Float side) -> Float { return (kern::math::sin(a_2) * side); }, a_2, side), 0.0));
    self.puffs_[at].vel = vector_make(kern::formula([](Float a_2, Float the_result) -> Float { return (kern::math::cos(a_2) * (0.07 + (the_result * 0.19))); }, a_2, unit_hash(kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(seed + 31.0); }, kern::to<Float>(seed)))).Result), kern::formula([](Float a_2, Float the_result) -> Float { return (kern::math::sin(a_2) * (0.07 + (the_result * 0.19))); }, a_2, unit_hash(kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(seed + 47.0); }, kern::to<Float>(seed)))).Result), kern::formula([](Float the_result, Float progress) -> Float { return ((0.46 + (the_result * 0.4)) + (progress * 0.3)); }, unit_hash(kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(seed + 61.0); }, kern::to<Float>(seed)))).Result, progress));
    self.puffs_[at].born = now;
    self.puffs_[at].expires = kern::formula([](Float now, Float ember, Float the_result) -> Float { return (now + ((ember != 0.0) ? 0.42 : (1.25 + (the_result * 0.52)))); }, now, kern::to<Float>(ember), unit_hash(kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(seed + 79.0); }, kern::to<Float>(seed)))).Result);
    self.puffs_[at].s0 = kern::formula([](Float scale, Float ember, Float the_result) -> Float { return (scale * ((ember != 0.0) ? 0.1 : (0.38 + (the_result * 0.19)))); }, e.scale, kern::to<Float>(ember), unit_hash(kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(seed + 97.0); }, kern::to<Float>(seed)))).Result);
    self.puffs_[at].s1 = kern::formula([](Float scale, Float ember, Float the_result) -> Float { return (scale * ((ember != 0.0) ? 0.035 : (1.32 + (the_result * 0.42)))); }, e.scale, kern::to<Float>(ember), unit_hash(kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::wrap32(seed + 113.0); }, kern::to<Float>(seed)))).Result);
    self.puffs_[at].serial = serial;
    self.puffs_[at].rotation = 0.0;
    self.puffs_[at].color = logic_select<Vector>(ember, rgb(16747069).Result, logic_select<Vector>(math_compare<">">(progress, 0.55), rgb(12172998).Result, rgb(9409692).Result));
    self.puffs_[at].opacity = kern::formula([](Float ember, Float progress) -> Float { return ((ember != 0.0) ? 0.94 : (0.54 + (progress * 0.18))); }, kern::to<Float>(ember), progress);
  }

  struct SmokeField_startResult { Float Result = 0.0; };
  SmokeField_startResult SmokeField_start(gaps_SmokeField& self, const Text& id, Vector at = Vector{0.0, 0.0, 0.0}, Float scale = 0.0, Float delay_s = 0.0, Float now = 0.0) {
    gaps_SmokeEffect e = gaps_SmokeEffect{"", Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, -1000000000.0, 0, 0};
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it11.next();) {
      if (text_equal(self.effects_[n11.index].id, id)) {
        self.effects_[n11.index].at = at;
        return {self.effects_[n11.index].expires};
      }
    }
    e = gaps_SmokeEffect{"", Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, -1000000000.0, 0, 0};
    e.id = id;
    e.at = at;
    e.scale = kern::formula([](Float scale) -> Float { return kern::math::max(0.35, scale); }, scale);
    e.started = now;
    e.expires = kern::formula([](Float now, Float delay_s) -> Float { return (now + kern::math::max(0.25, delay_s)); }, now, delay_s);
    e.seed = hash_text(id).Result;
    list_add(self.effects_, e);
    SmokeField_emit(self, self.effects_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))], now);
    self.effects_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))].last_emit = now;
    return {e.expires};
  }

  struct SmokeField_accelerateResult { Bool Result = false; };
  SmokeField_accelerateResult SmokeField_accelerate(gaps_SmokeField& self, const Text& id, Float expires_at = 0.0, Float now = 0.0) {
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it13.next();) {
      if (text_equal(self.effects_[n13.index].id, id)) {
        self.effects_[n13.index].expires = kern::formula([](Float a, Float now, Float expires_at) -> Float { return kern::math::min(a, kern::math::max((now + 0.18), expires_at)); }, self.effects_[n13.index].expires, now, expires_at);
        return {true};
      }
    }
    return {false};
  }

  struct stop_matchResult { Bool Result = false; };
  KERN_PURE stop_matchResult stop_match(const Text& id, const gaps_SmokeEffect& e) {
    return {text_equal(e.id, id)};
  }

  void SmokeField_stop(gaps_SmokeField& self, const Text& id) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    at = 0;
    kept = at;
    end = list_length<gaps_SmokeEffect>(self.effects_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(stop_match(id, self.effects_[at]).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n14, self.effects_[at]);
          self.effects_[kept] = n14.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.effects_, kept, gaps_SmokeEffect{"", Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, -1000000000.0, 0, 0});
  }

  void SmokeField_move(gaps_SmokeField& self, const Text& id, Vector at = Vector{0.0, 0.0, 0.0}) {
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it15.next();) {
      if (text_equal(self.effects_[n15.index].id, id)) {
        self.effects_[n15.index].at = at;
      }
    }
  }

  struct SmokeField_activeResult { Bool Result = false; };
  KERN_PURE SmokeField_activeResult SmokeField_active(const gaps_SmokeField& self, const Text& id) {
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it16.next();) {
      if (text_equal(self.effects_[n16.index].id, id)) {
        return {true};
      }
    }
    return {false};
  }

  struct SmokeField_opacityResult { Float Result = 0.0; };
  KERN_PURE SmokeField_opacityResult SmokeField_opacity(const gaps_Puff& p, Float now = 0.0) {
    Float t = 0.0;
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float born, Float expires) -> Float { return kern::math::div((now - born), kern::math::max(0.001, (expires - born))); }, now, p.born, p.expires), 0.0, 1.0));
    return {kern::formula([](Float ember, Float kPi, Float t) -> Float { return ((((ember != 0.0) ? 0.94 : 0.66) * kern::math::sin((kPi * kern::math::min(1.0, (t * 1.12))))) * (1.0 - (t * 0.48))); }, kern::to<Float>(p.ember), kPi, t)};
  }

  void SmokeField_update(gaps_SmokeField& self, Float now = 0.0, Float dt = 0.0) {
    Int i = 0;
    Bool skip = false;
    Float progress = 0.0;
    Float interval = 0.0;
    Bool skip_2 = false;
    Float k = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))), 0.5))) {
      skip = false;
      if (math_compare<">=">(now, self.effects_[i].expires)) {
        list_remove_at(self.effects_, i);
        skip = true;
      }
      if (logic_not(skip)) {
        progress = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a, Float a_2) -> Float { return kern::math::div((now - a), kern::math::max(0.001, (a_2 - a))); }, now, self.effects_[i].started, self.effects_[i].expires), 0.0, 1.0));
        interval = kern::formula([](Float progress) -> Float { return kern::math::div((155.0 - (progress * 72.0)), 1000.0); }, progress);
        if (math_compare<">=">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.effects_[i].last_emit), interval)) {
          self.effects_[i].last_emit = now;
          SmokeField_emit(self, self.effects_[i], now);
          if (math_compare<">">(progress, 0.76)) {
            SmokeField_emit(self, self.effects_[i], kern::formula([](Float now) -> Float { return (now + 0.001); }, now));
          }
        }
        i += 1;
      }
    }
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(self.puffs_))))); it17.next();) {
      skip_2 = false;
      if (logic_not(self.puffs_[n17.index].alive)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (math_compare<">=">(now, self.puffs_[n17.index].expires)) {
          self.puffs_[n17.index].alive = false;
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          self.puffs_[n17.index].pos = vector_add(self.puffs_[n17.index].pos, vector_scale(self.puffs_[n17.index].vel, dt));
          k = kern::formula([](Float dt) -> Float { return kern::math::exp(((-0.8) * dt)); }, dt);
          self.puffs_[n17.index].vel = vector_make(kern::formula([](Float x, Float k) -> Float { return (x * k); }, vector_break(self.puffs_[n17.index].vel).x, k), vector_break(self.puffs_[n17.index].vel).y, vector_break(self.puffs_[n17.index].vel).z);
          self.puffs_[n17.index].vel = vector_make(vector_break(self.puffs_[n17.index].vel).x, kern::formula([](Float y, Float k) -> Float { return (y * k); }, vector_break(self.puffs_[n17.index].vel).y, k), vector_break(self.puffs_[n17.index].vel).z);
          self.puffs_[n17.index].rotation = kern::formula([](Float a, Float dt, Float a_2) -> Float { return (a + (dt * ((kern::math::band(a_2, 1.0) != 0.0) ? (-0.42) : 0.42))); }, self.puffs_[n17.index].rotation, dt, kern::to<Float>(self.puffs_[n17.index].serial));
          self.puffs_[n17.index].opacity = SmokeField_opacity(self.puffs_[n17.index], now).Result;
        }
      }
    }
  }

  struct SmokeField_live_puffsResult { Int Result = 0; };
  KERN_PURE SmokeField_live_puffsResult SmokeField_live_puffs(const gaps_SmokeField& self) {
    Int n = 0;
    n = 0;
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(self.puffs_))))); it18.next();) {
      n += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(self.puffs_[n18.index].alive)));
    }
    return {n};
  }

  struct SmokeField_sizeResult { Float Result = 0.0; };
  KERN_PURE SmokeField_sizeResult SmokeField_size(const gaps_Puff& p, Float now = 0.0) {
    Float t = 0.0;
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float born, Float expires) -> Float { return kern::math::div((now - born), kern::math::max(0.001, (expires - born))); }, now, p.born, p.expires), 0.0, 1.0));
    return {kern::formula([](Float s0, Float s1, Float t) -> Float { return (s0 + ((s1 - s0) * (1.0 - ((1.0 - t) * (1.0 - t))))); }, p.s0, p.s1, t)};
  }

  struct dampingResult { Float Result = 0.0; };
  KERN_PURE dampingResult damping(Float rate = 0.0, Float dt = 0.0) {
    Float r = 0.0;
    Float d = 0.0;
    r = kern::formula([](Float rate) -> Float { return kern::math::max(0.0, rate); }, rate);
    d = kern::formula([](Float dt) -> Float { return (dt < 0.0 ? 0.0 : (0.1 < dt ? 0.1 : dt)); }, dt);
    return {kern::formula([](Float r, Float d) -> Float { return (((r > 0.0) && (d > 0.0)) ? (1.0 - kern::math::exp(((-r) * d))) : 0.0); }, r, d)};
  }

  struct normalizedResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE normalizedResult normalized(Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    Float l = 0.0;
    l = kern::formula([](Float x, Float y, Float z, Float w) -> Float { return kern::math::sqrt(((((x * x) + (y * y)) + (z * z)) + (w * w))); }, q.x, q.y, q.z, q.w);
    if (logic_not(math_compare<">">(l, 1e-09))) {
      return {struct_make<Quat>(0.0, 0.0, 0.0, 1.0)};
    }
    return {struct_make<Quat>(kern::formula([](Float x, Float l) -> Float { return kern::math::div(x, l); }, q.x, l), kern::formula([](Float y, Float l) -> Float { return kern::math::div(y, l); }, q.y, l), kern::formula([](Float z, Float l) -> Float { return kern::math::div(z, l); }, q.z, l), kern::formula([](Float w, Float l) -> Float { return kern::math::div(w, l); }, q.w, l))};
  }

  struct slerpResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE slerpResult slerp(Quat a = Quat{0.0, 0.0, 0.0, 1.0}, Quat b = Quat{0.0, 0.0, 0.0, 1.0}, Float t = 0.0) {
    Quat b_ = Quat{0.0, 0.0, 0.0, 1.0};
    Float d = 0.0;
    Float th = 0.0;
    Float s = 0.0;
    Float wa = 0.0;
    Float wb = 0.0;
    b_ = b;
    d = kern::formula([](Float x, Float x_2, Float y, Float y_2, Float z, Float z_2, Float w, Float w_2) -> Float { return ((((x * x_2) + (y * y_2)) + (z * z_2)) + (w * w_2)); }, a.x, b_.x, a.y, b_.y, a.z, b_.z, a.w, b_.w);
    if (math_compare<"<">(d, 0.0)) {
      d = kern::formula([](Float d) -> Float { return (-d); }, d);
      b_ = struct_make<Quat>(kern::formula([](Float x) -> Float { return (-x); }, b_.x), kern::formula([](Float y) -> Float { return (-y); }, b_.y), kern::formula([](Float z) -> Float { return (-z); }, b_.z), kern::formula([](Float w) -> Float { return (-w); }, b_.w));
    }
    d = kern::formula([](Float d) -> Float { return (d < (-1.0) ? (-1.0) : (1.0 < d ? 1.0 : d)); }, d);
    if (math_compare<">">(d, 0.9995)) {
      return {normalized(struct_make<Quat>(kern::formula([](Float x, Float x_2, Float t) -> Float { return (x + ((x_2 - x) * t)); }, a.x, b_.x, t), kern::formula([](Float y, Float y_2, Float t) -> Float { return (y + ((y_2 - y) * t)); }, a.y, b_.y, t), kern::formula([](Float z, Float z_2, Float t) -> Float { return (z + ((z_2 - z) * t)); }, a.z, b_.z, t), kern::formula([](Float w, Float w_2, Float t) -> Float { return (w + ((w_2 - w) * t)); }, a.w, b_.w, t))).Result};
    }
    th = kern::formula([](Float d) -> Float { return kern::math::acos(d); }, d);
    s = kern::formula([](Float th) -> Float { return kern::math::sin(th); }, th);
    wa = kern::formula([](Float t, Float th, Float s) -> Float { return kern::math::div(kern::math::sin(((1.0 - t) * th)), s); }, t, th, s);
    wb = kern::formula([](Float t, Float th, Float s) -> Float { return kern::math::div(kern::math::sin((t * th)), s); }, t, th, s);
    return {normalized(struct_make<Quat>(kern::formula([](Float x, Float wa, Float x_2, Float wb) -> Float { return ((x * wa) + (x_2 * wb)); }, a.x, wa, b_.x, wb), kern::formula([](Float y, Float wa, Float y_2, Float wb) -> Float { return ((y * wa) + (y_2 * wb)); }, a.y, wa, b_.y, wb), kern::formula([](Float z, Float wa, Float z_2, Float wb) -> Float { return ((z * wa) + (z_2 * wb)); }, a.z, wa, b_.z, wb), kern::formula([](Float w, Float wa, Float w_2, Float wb) -> Float { return ((w * wa) + (w_2 * wb)); }, a.w, wa, b_.w, wb))).Result};
  }

  struct quat_angleResult { Float Result = 0.0; };
  KERN_PURE quat_angleResult quat_angle(Quat a = Quat{0.0, 0.0, 0.0, 1.0}, Quat b = Quat{0.0, 0.0, 0.0, 1.0}) {
    return {kern::formula([](Float a) -> Float { return (2.0 * kern::math::acos(a)); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float y, Float y_2, Float z, Float z_2, Float w, Float w_2) -> Float { return kern::math::abs(((((x * x_2) + (y * y_2)) + (z * z_2)) + (w * w_2))); }, a.x, b.x, a.y, b.y, a.z, b.z, a.w, b.w), -1.0, 1.0))};
  }

  void VisualPose_update(gaps_VisualPose& self, Vector& out_p, Quat& out_q, Vector position = Vector{0.0, 0.0, 0.0}, Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}, Vector anchor = Vector{0.0, 0.0, 0.0}, Int mode = 0, Float dt = 0.0) {
    gaps_VisualProfile p = gaps_VisualProfile{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Quat target = Quat{0.0, 0.0, 0.0, 1.0};
    Vector raw = Vector{0.0, 0.0, 0.0};
    Float step = 0.0;
    Vector e = Vector{0.0, 0.0, 0.0};
    Float err = 0.0;
    Float ang = 0.0;
    p = logic_select<gaps_VisualProfile>(math_compare<"==">(kern::to<Float>(mode), 2.0), kVisualThird, kVisualFollow);
    target = normalized(rotation).Result;
    raw = vector_sub(position, anchor);
    if (logic_or(logic_not(self.init_), math_compare<"!=">(kern::to<Float>(self.mode_), kern::to<Float>(mode)))) {
      self.offset_ = raw;
      self.q_ = target;
      self.mode_ = mode;
      self.init_ = true;
      out_p = position;
      out_q = self.q_;
      return;
    }
    step = kern::formula([](Float dt) -> Float { return (dt < 0.0 ? 0.0 : (0.1 < dt ? 0.1 : dt)); }, dt);
    self.offset_ = vector_add(self.offset_, vector_scale(vector_sub(raw, self.offset_), damping(p.offset_rate, step).Result));
    e = vector_sub(raw, self.offset_);
    err = vector_length(e);
    if (math_compare<">">(err, p.soft_pos)) {
      self.offset_ = vector_add(self.offset_, vector_scale(e, kern::formula([](Float err, Float soft_pos, Float the_result) -> Float { return kern::math::div(((err - soft_pos) * the_result), err); }, err, p.soft_pos, damping(p.pos_recovery, step).Result)));
    }
    e = vector_sub(raw, self.offset_);
    err = vector_length(e);
    if (math_compare<">">(err, p.max_pos)) {
      self.offset_ = vector_add(self.offset_, vector_scale(e, kern::formula([](Float err, Float max_pos) -> Float { return kern::math::div((err - max_pos), err); }, err, p.max_pos)));
    }
    self.q_ = slerp(self.q_, target, damping(p.rotation_rate, step).Result).Result;
    ang = quat_angle(self.q_, target).Result;
    if (math_compare<">">(ang, p.soft_rot)) {
      self.q_ = slerp(self.q_, target, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float ang, Float soft_rot, Float the_result) -> Float { return kern::math::div(((ang - soft_rot) * the_result), ang); }, ang, p.soft_rot, damping(p.rot_recovery, step).Result), 0.0, 1.0))).Result;
    }
    ang = quat_angle(self.q_, target).Result;
    if (math_compare<">">(ang, p.max_rot)) {
      self.q_ = slerp(self.q_, target, kern::formula([](Float max_rot, Float ang) -> Float { return (1.0 - kern::math::div(max_rot, ang)); }, p.max_rot, ang)).Result;
    }
    out_p = vector_add(anchor, self.offset_);
    out_q = self.q_;
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct { gaps_SmokeEffect value = gaps_SmokeEffect{"", Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, -1000000000.0, 0, 0}; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
};
