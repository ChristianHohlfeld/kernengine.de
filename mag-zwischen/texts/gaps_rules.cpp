struct gaps_rules : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_ENUM(gaps_Organic, person, animal, bird);
  KERN_STRUCT(gaps_Puff) { Bool alive = false; Bool ember = false; Vector pos = Vector{0.0, 0.0, 0.0}; Vector vel = Vector{0.0, 0.0, 0.0}; Float born = 0.0; Float expires = 0.0; Float s0 = 1.0; Float s1 = 2.0; Float opacity = 0.0; Float rotation = 0.0; U32 serial = 0; Vector color = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(gaps_SirenVoice) { Bool have_d = false; Float last_d = 0.0; Float doppler = 1.0; Float gain = 0.0; };
  KERN_STRUCT(gaps_SmokeEffect) { Text id = ""; Vector at = Vector{0.0, 0.0, 0.0}; Float scale = 1.0; Float started = 0.0; Float expires = 0.0; Float last_emit = -1000000000.0; U32 serial = 0; U32 seed = 0; };
  KERN_STRUCT(gaps_VisualProfile) { Float offset_rate = 0.0; Float rotation_rate = 0.0; Float soft_pos = 0.0; Float max_pos = 0.0; Float pos_recovery = 0.0; Float soft_rot = 0.0; Float max_rot = 0.0; Float rot_recovery = 0.0; };
  KERN_STRUCT(gaps_SmokeField) { List<gaps_SmokeEffect> effects_ = List<gaps_SmokeEffect>{}; List<gaps_Puff> puffs_ = List<gaps_Puff>{}; };
  KERN_STRUCT(gaps_VisualPose) { Bool init_ = false; I32 mode_ = 0; Vector offset_ = Vector{0.0, 0.0, 0.0}; Quat q_ = Quat{0.0, 0.0, 0.0, 1.0}; };

  // ---- functions ----
  struct SmokeField_ofResult { gaps_SmokeField Result = gaps_SmokeField{List<gaps_SmokeEffect>{}, List<gaps_Puff>{}}; };
  /// A field with a pool of `pool` puffs (MAX_PUFFS: 78, on a phone 48). A field of its own has none yet: it emits nothing.
  KERN_PURE SmokeField_ofResult SmokeField_of(U32 pool = 0) {
    gaps_SmokeField f = gaps_SmokeField{List<gaps_SmokeEffect>{}, List<gaps_Puff>{}};
    f = gaps_SmokeField{List<gaps_SmokeEffect>{}, List<gaps_Puff>{}};
    list_resize(f.puffs_, pool, gaps_Puff{false, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0, Vector{0.0, 0.0, 0.0}});
    return {f};
  }

  struct SmokeField_effectsResult { Int Result = 0; };
  KERN_PURE SmokeField_effectsResult SmokeField_effects(const gaps_SmokeField& self) {
    return {list_length<gaps_SmokeEffect>(self.effects_)};
  }

  void VisualPose_invalidate(gaps_VisualPose& self) {
    self.init_ = false;
  }
};
