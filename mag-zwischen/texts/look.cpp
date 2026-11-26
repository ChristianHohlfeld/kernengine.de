struct look : virtual kern::Library, virtual common {
  // ---- types ----
  KERN_STRUCT(look_Shake) { Float trauma = 0.0; Float seed = 17.3; };
  KERN_STRUCT(look_SpeedLines_Streak) { Float a = 0.0; Float r = 0.0; Float len = 0.0; Float v = 0.0; Bool warm = false; };
  KERN_STRUCT(look_Stabilizer) { Bool init = false; Vector pos = Vector{0.0, 0.0, 0.0}; Vector target = Vector{0.0, 0.0, 0.0}; Vector up = Vector{0.0, 0.0, 0.0}; Float lag = 0.0; };
  KERN_STRUCT(look_SpeedLines) { Fixed<look_SpeedLines_Streak, 26> s = List<look_SpeedLines_Streak>{}; Bool seeded = false; Bool have_last = false; Vector last = Vector{0.0, 0.0, 0.0}; Float speed = 0.0; Float k = 0.0; Rng rng = Rng{24301, 0.0, false}; };

  // ---- variables ----
  KERN_CONST Float kDecayPerS = 1.35;
  KERN_CONST Float kMaxAngle = 0.055;
  KERN_CONST Float kMaxOffset = 0.09;
  KERN_CONST Float kFreq = 19.0;
  KERN_CONST Float kPositionHz = 55.0;
  KERN_CONST Float kOrientationHz = 45.0;
  KERN_CONST Float kTeleport = 0.35;
  KERN_CONST Float kTargetTeleport = 2.5;
  /// m/s
  KERN_CONST Float kStart = 13.0;
  /// m/s
  KERN_CONST Float kFull = 40.0;
  KERN_CONST I32 kCount = 26;
  /// ---- day and night: one clock (480 s per day, SpawnCity) the game owns ----
  KERN_CONST Float kDayLength = 480.0;

  // ---- functions ----
  struct impact_traumaResult { Float Result = 0.0; };
  /// Hard impacts of the player's car / drone (onImpact): 0 below 2.5 m/s of velocity change.
  KERN_PURE impact_traumaResult impact_trauma(Float dv = 0.0) {
    return {kern::formula([](Float dv, Float a) -> Float { return ((dv < 2.5) ? 0.0 : a); }, dv, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dv) -> Float { return kern::math::div((dv - 2.5), 14.0); }, dv), 0.0, 0.7))};
  }

  void Stabilizer_reset(look_Stabilizer& self) {
    self.init = false;
    self.lag = 0.0;
  }

  struct lamp_glowResult { Float Result = 0.0; };
  /// Street lamps glow from dusk (scene_lighting.mjs): (night - 0.15) / 0.5.
  KERN_PURE lamp_glowResult lamp_glow(Float night = 0.0) {
    return {kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float night) -> Float { return kern::math::div((night - 0.15), 0.5); }, night), 0.0, 1.0))};
  }
};
