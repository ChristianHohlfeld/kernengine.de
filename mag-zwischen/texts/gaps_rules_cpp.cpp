struct gaps_rules_cpp : virtual kern::Library, virtual blueprint_call, virtual gaps_rules {
  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST gaps_VisualProfile kVisualThird = gaps_VisualProfile{5.2, 7.0, 0.05, 0.18, 7.0, 0.08726646259971647, 0.24434609527920614, 7.5};
  KERN_CONST gaps_VisualProfile kVisualFollow = gaps_VisualProfile{9.0, 12.0, 0.025, 0.1, 9.0, 0.05235987755982989, 0.17453292519943295, 10.0};

  // ---- functions ----
  struct Sirens_HornHzResult { Float hz = 0.0; };
  /// (of the Blueprint of the actor Sirens, called on it)
  KERN_EXTERN_AS("HornHz") Sirens_HornHzResult Sirens_HornHz(Float t = 0.0);

  struct Sirens_WhoopHzResult { Float hz = 0.0; };
  /// (of the Blueprint of the actor Sirens, called on it)
  KERN_EXTERN_AS("WhoopHz") Sirens_WhoopHzResult Sirens_WhoopHz(Float t = 0.0);

  struct Sirens_SirenGainResult { Float gain = 0.0; };
  /// (of the Blueprint of the actor Sirens, called on it)
  KERN_EXTERN_AS("SirenGain") Sirens_SirenGainResult Sirens_SirenGain(Float base = 0.0, Float d = 0.0, Float level = 0.0);

  struct Sirens_SirenCutoffResult { Float hz = 0.0; };
  /// (of the Blueprint of the actor Sirens, called on it)
  KERN_EXTERN_AS("SirenCutoff") Sirens_SirenCutoffResult Sirens_SirenCutoff(Float d = 0.0);

  struct Sirens_VoiceUpdateResult { Float gain = 0.0; Bool haveD = false; Float lastD = 0.0; Float doppler = 0.0; };
  /// (of the Blueprint of the actor Sirens, called on it)
  KERN_EXTERN_AS("VoiceUpdate") Sirens_VoiceUpdateResult Sirens_VoiceUpdate(Float d = 0.0, Float dt = 0.0, Float base = 0.0, Float level = 0.0, Bool haveD = false, Float lastD = 0.0, Float doppler = 0.0);

  struct Sirens_PostArrivalGraceMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Sirens, called on it)
  KERN_EXTERN_AS("PostArrivalGraceMs") Sirens_PostArrivalGraceMsResult Sirens_PostArrivalGraceMs(Int stars_2 = 0);

  struct Sirens_OrganicRateResult { Float rate = 0.0; };
  /// (of the Blueprint of the actor Sirens, called on it)
  KERN_EXTERN_AS("OrganicRate") Sirens_OrganicRateResult Sirens_OrganicRate(gaps_Organic kind = gaps_Organic::person);

  struct bind_sirensResult { Bool Result = false; };
  KERN_PURE bind_sirensResult bind_sirens() {
    return {func_has_blueprint(actor_find("Sirens"))};
  }

  struct horn_lowResult { Float Result = 0.0; };
  KERN_PURE horn_lowResult horn_low() {
    return {func_get_on<"HornLow", Float>(actor_find("Sirens"))};
  }

  struct horn_highResult { Float Result = 0.0; };
  KERN_PURE horn_highResult horn_high() {
    return {func_get_on<"HornHigh", Float>(actor_find("Sirens"))};
  }

  struct car_gainResult { Float Result = 0.0; };
  KERN_PURE car_gainResult car_gain() {
    return {func_get_on<"CarGain", Float>(actor_find("Sirens"))};
  }

  struct drone_gainResult { Float Result = 0.0; };
  KERN_PURE drone_gainResult drone_gain() {
    return {func_get_on<"DroneGain", Float>(actor_find("Sirens"))};
  }

  struct whoop_baseResult { Float Result = 0.0; };
  KERN_PURE whoop_baseResult whoop_base() {
    return {func_get_on<"WhoopBase", Float>(actor_find("Sirens"))};
  }

  struct organic_gainResult { Float Result = 0.0; };
  KERN_PURE organic_gainResult organic_gain() {
    return {func_get_on<"OrganicGain", Float>(actor_find("Sirens"))};
  }

  struct organic_min_gap_sResult { Float Result = 0.0; };
  KERN_PURE organic_min_gap_sResult organic_min_gap_s() {
    return {func_get_on<"OrganicMinGapS", Float>(actor_find("Sirens"))};
  }

  struct horn_hzResult { Float Result = 0.0; };
  horn_hzResult horn_hz(Float t = 0.0) {
    Float hz = 0.0;
    hz = 0.0;
    func_call_on<"Sirens_HornHz">(actor_find("Sirens"), kern::out(n10.hz), blueprint_call::decimal(t).Result);
    hz = n10.hz;
    return {hz};
  }

  struct whoop_hzResult { Float Result = 0.0; };
  whoop_hzResult whoop_hz(Float t = 0.0) {
    Float hz = 0.0;
    hz = 0.0;
    func_call_on<"Sirens_WhoopHz">(actor_find("Sirens"), kern::out(n11.hz), blueprint_call::decimal(t).Result);
    hz = n11.hz;
    return {hz};
  }

  struct siren_gainResult { Float Result = 0.0; };
  siren_gainResult siren_gain(Float base = 0.0, Float d = 0.0, Float level = 0.0) {
    Float gain = 0.0;
    gain = 0.0;
    func_call_on<"Sirens_SirenGain">(actor_find("Sirens"), kern::out(n12.gain), blueprint_call::decimal(base).Result, blueprint_call::decimal(d).Result, blueprint_call::decimal(level).Result);
    gain = n12.gain;
    return {gain};
  }

  struct siren_cutoffResult { Float Result = 0.0; };
  siren_cutoffResult siren_cutoff(Float d = 0.0) {
    Float hz = 0.0;
    hz = 0.0;
    func_call_on<"Sirens_SirenCutoff">(actor_find("Sirens"), kern::out(n13.hz), blueprint_call::decimal(d).Result);
    hz = n13.hz;
    return {hz};
  }

  struct SirenVoice_updateResult { Float Result = 0.0; };
  SirenVoice_updateResult SirenVoice_update(gaps_SirenVoice& self, Float d = 0.0, Float dt = 0.0, Float base = 0.0, Float level = 0.0) {
    Float gain = 0.0;
    Float next_last_d = 0.0;
    Float next_doppler = 0.0;
    gain = 0.0;
    next_last_d = 0.0;
    next_doppler = 0.0;
    func_call_on<"Sirens_VoiceUpdate">(actor_find("Sirens"), kern::out(n14.gain, n14.haveD, n14.lastD, n14.doppler), blueprint_call::decimal(d).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(base).Result, blueprint_call::decimal(level).Result, self.have_d, blueprint_call::decimal(self.last_d).Result, blueprint_call::decimal(self.doppler).Result);
    gain = n14.gain;
    self.have_d = n14.haveD;
    next_last_d = n14.lastD;
    next_doppler = n14.doppler;
    self.last_d = next_last_d;
    self.doppler = next_doppler;
    return {gain};
  }

  struct post_arrival_grace_msResult { Float Result = 0.0; };
  post_arrival_grace_msResult post_arrival_grace_ms(Int stars = 0) {
    Float ms = 0.0;
    ms = 0.0;
    func_call_on<"Sirens_PostArrivalGraceMs">(actor_find("Sirens"), kern::out(n15.ms), stars);
    ms = n15.ms;
    return {ms};
  }

  struct organic_rateResult { Float Result = 0.0; };
  organic_rateResult organic_rate(gaps_Organic k = gaps_Organic::person) {
    Float rate = 0.0;
    rate = 0.0;
    func_call_on<"Sirens_OrganicRate">(actor_find("Sirens"), kern::out(n16.rate), k);
    rate = n16.rate;
    return {rate};
  }

  struct hash_textResult { Int Result = 0; };
  /// ---- world_critical_damage_fx.mjs
  KERN_PURE hash_textResult hash_text(const Text& s) {
    Int h = 0;
    Int c = 0;
    h = 2166136261;
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it17.next();) {
      c = text_code_at(s, n17.index);
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
      for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(self.puffs_))))); it19.next();) {
        if (math_compare<"<">(self.puffs_[n19.index].born, self.puffs_[at].born)) {
          at = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n19.index)));
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
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it18.next();) {
      if (text_equal(self.effects_[n18.index].id, id)) {
        self.effects_[n18.index].at = at;
        return {self.effects_[n18.index].expires};
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
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it20.next();) {
      if (text_equal(self.effects_[n20.index].id, id)) {
        self.effects_[n20.index].expires = kern::formula([](Float a, Float now, Float expires_at) -> Float { return kern::math::min(a, kern::math::max((now + 0.18), expires_at)); }, self.effects_[n20.index].expires, now, expires_at);
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
          var_take(n21, self.effects_[at]);
          self.effects_[kept] = n21.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.effects_, kept, gaps_SmokeEffect{"", Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, -1000000000.0, 0, 0});
  }

  void SmokeField_move(gaps_SmokeField& self, const Text& id, Vector at = Vector{0.0, 0.0, 0.0}) {
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it22.next();) {
      if (text_equal(self.effects_[n22.index].id, id)) {
        self.effects_[n22.index].at = at;
      }
    }
  }

  struct SmokeField_activeResult { Bool Result = false; };
  KERN_PURE SmokeField_activeResult SmokeField_active(const gaps_SmokeField& self, const Text& id) {
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_SmokeEffect>(self.effects_))))); it23.next();) {
      if (text_equal(self.effects_[n23.index].id, id)) {
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
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(self.puffs_))))); it24.next();) {
      skip_2 = false;
      if (logic_not(self.puffs_[n24.index].alive)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (math_compare<">=">(now, self.puffs_[n24.index].expires)) {
          self.puffs_[n24.index].alive = false;
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          self.puffs_[n24.index].pos = vector_add(self.puffs_[n24.index].pos, vector_scale(self.puffs_[n24.index].vel, dt));
          k = kern::formula([](Float dt) -> Float { return kern::math::exp(((-0.8) * dt)); }, dt);
          self.puffs_[n24.index].vel = vector_make(kern::formula([](Float x, Float k) -> Float { return (x * k); }, vector_break(self.puffs_[n24.index].vel).x, k), vector_break(self.puffs_[n24.index].vel).y, vector_break(self.puffs_[n24.index].vel).z);
          self.puffs_[n24.index].vel = vector_make(vector_break(self.puffs_[n24.index].vel).x, kern::formula([](Float y, Float k) -> Float { return (y * k); }, vector_break(self.puffs_[n24.index].vel).y, k), vector_break(self.puffs_[n24.index].vel).z);
          self.puffs_[n24.index].rotation = kern::formula([](Float a, Float dt, Float a_2) -> Float { return (a + (dt * ((kern::math::band(a_2, 1.0) != 0.0) ? (-0.42) : 0.42))); }, self.puffs_[n24.index].rotation, dt, kern::to<Float>(self.puffs_[n24.index].serial));
          self.puffs_[n24.index].opacity = SmokeField_opacity(self.puffs_[n24.index], now).Result;
        }
      }
    }
  }

  struct SmokeField_live_puffsResult { Int Result = 0; };
  KERN_PURE SmokeField_live_puffsResult SmokeField_live_puffs(const gaps_SmokeField& self) {
    Int n = 0;
    n = 0;
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(self.puffs_))))); it25.next();) {
      n = kern::to<Int>(kern::formula([](Float n, Float a) -> Float { return kern::math::wrap32(n + kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0))); }, kern::to<Float>(n), kern::to<Float>(self.puffs_[n25.index].alive)));
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
  struct { Float hz = 0.0; } n10;
  struct { Float hz = 0.0; } n11;
  struct { Float gain = 0.0; } n12;
  struct { Float hz = 0.0; } n13;
  struct { Float gain = 0.0; Bool haveD = false; Float lastD = 0.0; Float doppler = 0.0; } n14;
  struct { Float ms = 0.0; } n15;
  struct { Float rate = 0.0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct { gaps_SmokeEffect value = gaps_SmokeEffect{"", Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, -1000000000.0, 0, 0}; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
};
