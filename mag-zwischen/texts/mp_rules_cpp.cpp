struct mp_rules_cpp : virtual kern::Library, virtual blueprint_call, virtual common, virtual mp_rules {
  // ---- types ----
  KERN_STRUCT(mp_Along) { Float along = 0.0; Float distance = 1e+30; };

  // ---- variables ----
  KERN_CONST U32 kMaxSnapshots = 18;
  /// ---- pose timeline (vs_pose_sync.mjs) ----------------------------------------------------------
  KERN_CONST Float kInterpolationDelayMs = 90.0;
  /// VS_POSE_STALE_HOLD_MS
  KERN_CONST Float kStaleHoldMs = 3000.0;
  /// VS_POSE_MAX_EXTRAPOLATION_MS
  KERN_CONST Float kMaxExtrapolationMs = 100.0;
  /// vs_multiplayer.mjs: a jump > 35 m resets the timeline
  KERN_CONST Float kTeleportM = 35.0;
  /// ---- proximity voice (voice_codec.mjs, proximity_voice.mjs) -------------------------------------
  KERN_CONST U32 kVoiceRate = 8000;
  /// 80 ms per packet (proximity_voice.mjs onAudio)
  KERN_CONST U32 kVoiceFrame = 640;
  KERN_CONST Float kDeg = 0.017453292519943295;

  // ---- functions ----
  struct Multiplayer_ColorSlotsResult { List<U32> colors = List<U32>{}; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("ColorSlots") Multiplayer_ColorSlotsResult Multiplayer_ColorSlots(List<U32> numbers = List<U32>{});

  struct Multiplayer_WeaponLabelResult { Text label = ""; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("WeaponLabel") Multiplayer_WeaponLabelResult Multiplayer_WeaponLabel(mp_Weapon weapon = mp_Weapon::other);

  struct Multiplayer_ZoneNameResult { Text name = ""; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("ZoneName") Multiplayer_ZoneNameResult Multiplayer_ZoneName(mp_Zone zone = mp_Zone::torso);

  struct Multiplayer_RoundDamageResult { Float damage = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("RoundDamage") Multiplayer_RoundDamageResult Multiplayer_RoundDamage(mp_Weapon weapon = mp_Weapon::other, mp_Zone zone = mp_Zone::torso);

  struct Multiplayer_BlastDamageResult { Float damage = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("BlastDamage") Multiplayer_BlastDamageResult Multiplayer_BlastDamage(Float damage_2 = 0.0);

  struct Multiplayer_JudgeHitResult { mp_HitVerdict verdict = mp_HitVerdict::applied; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("JudgeHit") Multiplayer_JudgeHitResult Multiplayer_JudgeHit(Bool targetKnown = false, Bool shooterKnown = false, Bool selfDamage = false, Bool friendlyFire = false, Float nowMs = 0.0, Float protectedUntilMs = 0.0, Float targetHp = 0.0);

  struct Multiplayer_RegenResult { Float hp = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("Regen") Multiplayer_RegenResult Multiplayer_Regen(Float hp = 0.0, Float nowMs = 0.0, Float lastHurtMs = 0.0, Float dt = 0.0);

  struct Multiplayer_RespawnOffsetResult { Float x = 0.0; Float y = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("RespawnOffset") Multiplayer_RespawnOffsetResult Multiplayer_RespawnOffset(Float r1 = 0.0, Float r2 = 0.0);

  struct Multiplayer_SpawnRingOffsetResult { Float x = 0.0; Float y = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("SpawnRingOffset") Multiplayer_SpawnRingOffsetResult Multiplayer_SpawnRingOffset(Int slot = 0);

  struct Multiplayer_CreateBallotResult { Bool created = false; mp_Ballot ballot = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("CreateBallot") Multiplayer_CreateBallotResult Multiplayer_CreateBallot(Int id = 0, Int key_2 = 0, Bool value = false, Int host = 0, List<U8> members = List<U8>{}, Float nowMs = 0.0);

  struct Multiplayer_CastVoteResult { mp_Ballot ballot = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}; mp_Ballot_Status did = mp_Ballot_Status::pending; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("CastVote") Multiplayer_CastVoteResult Multiplayer_CastVote(mp_Ballot ballot = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}, Int voter = 0, Bool approve = false, List<U8> current = List<U8>{}, Float nowMs = 0.0);

  struct Multiplayer_StaleResult { Bool stale = false; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("Stale") Multiplayer_StaleResult Multiplayer_Stale(mp_Ballot ballot = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}, List<U8> current = List<U8>{}, Float nowMs = 0.0);

  struct Multiplayer_BallotNameResult { Text name = ""; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("BallotName") Multiplayer_BallotNameResult Multiplayer_BallotName(mp_Ballot_Status status = mp_Ballot_Status::pending);

  struct Multiplayer_VoiceGainResult { Float gain = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("VoiceGain") Multiplayer_VoiceGainResult Multiplayer_VoiceGain(Float distance_2 = 0.0, Bool everywhere = false);

  struct Multiplayer_BearingArrowResult { Text arrow = ""; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("BearingArrow") Multiplayer_BearingArrowResult Multiplayer_BearingArrow(Float bearing = 0.0);

  struct Multiplayer_LocateMapPeerResult { Bool valid = false; Float x = 0.0; Float y = 0.0; Bool offmap = false; Float rotationDeg = 0.0; Text arrow = ""; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("LocateMapPeer") Multiplayer_LocateMapPeerResult Multiplayer_LocateMapPeer(Float x = 0.0, Float y = 0.0, Float width_2 = 0.0, Float height = 0.0, Float insetX = 0.0, Float insetTop = 0.0, Float insetBottom = 0.0);

  struct Multiplayer_SafeMobileMarkerPointResult { Float x = 0.0; Float y = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("SafeMobileMarkerPoint") Multiplayer_SafeMobileMarkerPointResult Multiplayer_SafeMobileMarkerPoint(Float x = 0.0, Float y = 0.0, Float width_2 = 0.0, Float height = 0.0, Bool mobile = false);

  struct Multiplayer_PingTogglesResult { Bool removes = false; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("PingToggles") Multiplayer_PingTogglesResult Multiplayer_PingToggles(Float distance_2 = 0.0);

  struct Multiplayer_PingExpiredResult { Bool expired = false; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("PingExpired") Multiplayer_PingExpiredResult Multiplayer_PingExpired(Float ageMs = 0.0);

  struct Multiplayer_PingFadeResult { Float fade = 0.0; };
  /// (of the Blueprint of the actor Multiplayer, called on it)
  KERN_EXTERN_AS("PingFade") Multiplayer_PingFadeResult Multiplayer_PingFade(Float ageMs = 0.0);

  struct spawn_protect_msResult { Float Result = 0.0; };
  KERN_PURE spawn_protect_msResult spawn_protect_ms() {
    return {func_get_on<"SpawnProtectMs", Float>(actor_find("Multiplayer"))};
  }

  struct ballot_msResult { Float Result = 0.0; };
  KERN_PURE ballot_msResult ballot_ms() {
    return {func_get_on<"BallotMs", Float>(actor_find("Multiplayer"))};
  }

  struct voice_radiusResult { Float Result = 0.0; };
  KERN_PURE voice_radiusResult voice_radius() {
    return {func_get_on<"VoiceRadius", Float>(actor_find("Multiplayer"))};
  }

  struct fmin2Result { Float Result = 0.0; };
  KERN_PURE fmin2Result fmin2(Float a = 0.0, Float b = 0.0) {
    return {kern::formula([](Float a, Float b) -> Float { return ((a < b) ? a : b); }, a, b)};
  }

  struct clampdResult { Float Result = 0.0; };
  KERN_PURE clampdResult clampd(Float v = 0.0, Float lo = 0.0, Float hi = 0.0) {
    return {kern::formula([](Float v, Float lo, Float hi) -> Float { return ((v < lo) ? lo : ((v > hi) ? hi : v)); }, v, lo, hi)};
  }

  struct nlerp_slerpResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE nlerp_slerpResult nlerp_slerp(Quat a = Quat{0.0, 0.0, 0.0, 1.0}, Quat b = Quat{0.0, 0.0, 0.0, 1.0}, Float t = 0.0) {
    return {common::slerp(a, b, t).Result};
  }

  struct PoseTimeline_pushResult { Bool Result = false; };
  PoseTimeline_pushResult PoseTimeline_push(mp_PoseTimeline& self, Vector p = Vector{0.0, 0.0, 0.0}, Quat q = Quat{0.0, 0.0, 0.0, 1.0}, Vector v = Vector{0.0, 0.0, 0.0}, Bool has_v = false, Float source = 0.0, Float received = 0.0) {
    Quat q_ = Quat{0.0, 0.0, 0.0, 1.0};
    Int prev_at = 0;
    Vector measured = Vector{0.0, 0.0, 0.0};
    Bool has_measured = false;
    Float dt = 0.0;
    Vector target = Vector{0.0, 0.0, 0.0};
    Vector vel = Vector{0.0, 0.0, 0.0};
    Float the_floor = 0.0;
    Int best = 0;
    Int at = 0;
    Int end = 0;
    Float late = 0.0;
    Int best_2 = 0;
    Int at_2 = 0;
    Int end_2 = 0;
    Float gap = 0.0;
    Float want = 0.0;
    q_ = q;
    if (logic_or(logic_or(logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).x), 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).y), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).z), 0.0)))) {
      return {false};
    }
    q_ = common::qnormalize(q_).Result;
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_)), 0.0))) {
      prev_at = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))));
      if (logic_or(math_compare<"<=">(received, self.snaps_[prev_at].received), math_compare<"<=">(source, self.snaps_[prev_at].source))) {
        return {false};
      }
    }
    measured = Vector{0.0, 0.0, 0.0};
    has_measured = false;
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_)), 0.0))) {
      dt = kern::formula([](Float source, Float a) -> Float { return kern::math::div((source - a), 1000.0); }, source, self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))))].source);
      if (logic_and(math_compare<">">(dt, 0.001), math_compare<"<">(dt, 1.0))) {
        measured = vector_scale(vector_sub(p, self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))))].p), kern::formula([](Float dt) -> Float { return kern::math::div(1.0, dt); }, dt));
        has_measured = true;
      }
    }
    target = logic_select<Vector>(logic_and(has_v, has_measured), vector_add(vector_scale(v, 0.7), vector_scale(measured, 0.3)), logic_select<Vector>(has_v, v, logic_select<Vector>(has_measured, measured, Vector{0.0, 0.0, 0.0})));
    vel = logic_select<Vector>(math_compare<"==">(kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_)), 0.0), target, vector_add(self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))))].v, vector_scale(vector_sub(target, self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))))].v), 0.35)));
    list_add(self.offsets_, kern::formula([](Float received, Float source) -> Float { return (received - source); }, received, source));
    while (loop(math_compare<">">(kern::formula([](Float kMaxSnapshots, Float a) -> Float { return ((kMaxSnapshots < 0.0) != (a < 0.0) ? (a < 0.0) : (kMaxSnapshots < a)); }, kern::to<Float>(kMaxSnapshots), kern::to<Float>(list_length<Float>(self.offsets_))), 0.5))) {
      list_remove_at(self.offsets_, 0);
    }
    best = 0;
    end = list_length<Float>(self.offsets_);
    at = kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(best)));
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (math_compare<"<">(self.offsets_[at], self.offsets_[best])) {
        best = at;
      }
      at += 1;
    }
    the_floor = self.offsets_[best];
    if (logic_not(self.have_offset_)) {
      self.offset_ = the_floor;
      self.have_offset_ = true;
    } else if (math_compare<"<">(the_floor, self.offset_)) {
      self.offset_ = the_floor;
    } else {
      self.offset_ = kern::formula([](Float offset_, Float the_result) -> Float { return (offset_ + the_result); }, self.offset_, fmin2(1.0, kern::formula([](Float the_floor, Float offset_) -> Float { return ((the_floor - offset_) * 0.08); }, the_floor, self.offset_)).Result);
    }
    list_add(self.lags_, kern::formula([](Float received, Float source, Float offset_) -> Float { return kern::math::max(0.0, ((received - source) - offset_)); }, received, source, self.offset_));
    while (loop(math_compare<">">(kern::formula([](Float kMaxSnapshots, Float a) -> Float { return ((kMaxSnapshots < 0.0) != (a < 0.0) ? (a < 0.0) : (kMaxSnapshots < a)); }, kern::to<Float>(kMaxSnapshots), kern::to<Float>(list_length<Float>(self.lags_))), 0.5))) {
      list_remove_at(self.lags_, 0);
    }
    best_2 = 0;
    end_2 = list_length<Float>(self.lags_);
    at_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(best_2)));
    while (loop(math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2)))) {
      if (math_compare<"<">(self.lags_[best_2], self.lags_[at_2])) {
        best_2 = at_2;
      }
      at_2 += 1;
    }
    late = self.lags_[best_2];
    gap = kern::formula([](Float a, Float source, Float a_2) -> Float { return ((a != 0.0) ? 33.0 : kern::math::min(250.0, (source - a_2))); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_)), 0.0)), source, self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))))].source);
    want = clampd(kern::formula([](Float late, Float gap) -> Float { return ((late + gap) + 15.0); }, late, gap), kInterpolationDelayMs, 320.0).Result;
    self.delay_ = kern::formula([](Float delay_, Float want) -> Float { return (delay_ + ((want > delay_) ? kern::math::min((want - delay_), 40.0) : kern::math::max((want - delay_), (-3.0)))); }, self.delay_, want);
    list_add(self.snaps_, struct_make<mp_PoseTimeline_Snap>(p, q_, vel, received, source));
    while (loop(math_compare<">">(kern::formula([](Float kMaxSnapshots, Float a) -> Float { return ((kMaxSnapshots < 0.0) != (a < 0.0) ? (a < 0.0) : (kMaxSnapshots < a)); }, kern::to<Float>(kMaxSnapshots), kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))), 0.5))) {
      list_remove_at(self.snaps_, 0);
    }
    return {true};
  }

  struct PoseTimeline_sampleResult { mp_PoseSample Result = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false}; };
  KERN_PURE PoseTimeline_sampleResult PoseTimeline_sample(const mp_PoseTimeline& self, Float now = 0.0) {
    mp_PoseSample out = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false};
    Int latest_at = 0;
    Float target = 0.0;
    Int no_right = 0;
    Int left_at = 0;
    Int right_at = 0;
    Int i = 0;
    Bool stop = false;
    Float span = 0.0;
    Float a = 0.0;
    Float T = 0.0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float flown = 0.0;
    Float a2 = 0.0;
    Float a3 = 0.0;
    Float ex = 0.0;
    out = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false};
    if (math_compare<"==">(kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_)), 0.0)) {
      return {out};
    }
    latest_at = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))));
    out.valid = true;
    out.age_ms = kern::formula([](Float now, Float a) -> Float { return ((now > a) ? (now - a) : 0.0); }, now, self.snaps_[latest_at].received);
    out.stale = math_compare<">">(out.age_ms, kStaleHoldMs);
    target = kern::formula([](Float now, Float offset_, Float delay_) -> Float { return ((now - offset_) - delay_); }, now, self.offset_, self.delay_);
    no_right = -1;
    left_at = 0;
    right_at = no_right;
    i = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))), 0.5)))) {
      if (math_compare<">=">(self.snaps_[i].source, target)) {
        right_at = i;
        stop = true;
      }
      if (logic_not(stop)) {
        left_at = i;
      }
      if (logic_not(stop)) {
        i += 1;
      }
    }
    if (math_compare<"!=">(kern::to<Float>(right_at), kern::to<Float>(no_right))) {
      span = kern::formula([](Float a, Float a_2) -> Float { return kern::math::max(0.001, (a - a_2)); }, self.snaps_[right_at].source, self.snaps_[left_at].source);
      a = clampd(kern::formula([](Float target, Float a, Float span) -> Float { return kern::math::div((target - a), span); }, target, self.snaps_[left_at].source, span), 0.0, 1.0).Result;
      T = kern::formula([](Float span) -> Float { return kern::math::div(span, 1000.0); }, span);
      d = vector_sub(self.snaps_[right_at].p, self.snaps_[left_at].p);
      flown = kern::formula([](Float a, Float a_2, Float t) -> Float { return (((a + a_2) * 0.5) * t); }, vector_length(self.snaps_[left_at].v), vector_length(self.snaps_[right_at].v), T);
      if (logic_and(math_compare<"<">(T, 0.6), math_compare<"<=">(vector_length(d), kern::formula([](Float flown) -> Float { return ((flown * 2.5) + 0.5); }, flown)))) {
        a2 = kern::formula([](Float a) -> Float { return (a * a); }, a);
        a3 = kern::formula([](Float a2, Float a) -> Float { return (a2 * a); }, a2, a);
        out.p = vector_add(vector_add(vector_add(vector_scale(self.snaps_[left_at].p, kern::formula([](Float a3, Float a2) -> Float { return (((2.0 * a3) - (3.0 * a2)) + 1.0); }, a3, a2)), vector_scale(self.snaps_[left_at].v, kern::formula([](Float a3, Float a2, Float a, Float t) -> Float { return (((a3 - (2.0 * a2)) + a) * t); }, a3, a2, a, T))), vector_scale(self.snaps_[right_at].p, kern::formula([](Float a3, Float a2) -> Float { return (((-2.0) * a3) + (3.0 * a2)); }, a3, a2))), vector_scale(self.snaps_[right_at].v, kern::formula([](Float a3, Float a2, Float t) -> Float { return ((a3 - a2) * t); }, a3, a2, T)));
      } else {
        out.p = vector_add(self.snaps_[left_at].p, vector_scale(d, a));
      }
      out.q = nlerp_slerp(self.snaps_[left_at].q, self.snaps_[right_at].q, a).Result;
      return {out};
    }
    ex = clampd(kern::formula([](Float target, Float a) -> Float { return (target - a); }, target, self.snaps_[latest_at].source), 0.0, kMaxExtrapolationMs).Result;
    out.p = vector_add(self.snaps_[latest_at].p, vector_scale(self.snaps_[latest_at].v, kern::formula([](Float ex) -> Float { return kern::math::div(ex, 1000.0); }, ex)));
    out.q = self.snaps_[latest_at].q;
    out.extrapolated = math_compare<">">(ex, 0.0);
    return {out};
  }

  struct NavTimeline_pushResult { Bool Result = false; };
  NavTimeline_pushResult NavTimeline_push(mp_NavTimeline& self, Vector p = Vector{0.0, 0.0, 0.0}, Float heading = 0.0, U8 mode = 0, Float at = 0.0) {
    if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_)), 0.0)), math_compare<"<=">(at, self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_))))].at))) {
      return {false};
    }
    if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_)), 0.0)), logic_or(math_compare<"!=">(kern::to<Float>(self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_))))].mode), kern::to<Float>(mode)), math_compare<">">(vector_length(vector_sub(self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_))))].p, p)), kTeleportM)))) {
      mp_rules::NavTimeline_reset(self);
    }
    list_add(self.snaps_, struct_make<mp_NavTimeline_Snap>(p, common::wrap_pi(heading).Result, mode, at));
    while (loop(math_compare<">">(kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_)), 10.0))) {
      list_remove_at(self.snaps_, 0);
    }
    return {true};
  }

  struct NavTimeline_sampleResult { Bool Result = false; };
  NavTimeline_sampleResult NavTimeline_sample(const mp_NavTimeline& self, Vector& p, Float& heading, Float now = 0.0) {
    Float at = 0.0;
    Bool skip = false;
    Float t = 0.0;
    if (math_compare<"==">(kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_)), 0.0)) {
      return {false};
    }
    at = kern::formula([](Float now, Float kInterpolationDelayMs) -> Float { return (now - kInterpolationDelayMs); }, now, kInterpolationDelayMs);
    if (math_compare<"<=">(at, self.snaps_[0].at)) {
      p = self.snaps_[0].p;
      heading = self.snaps_[0].heading;
      return {true};
    }
    for (auto it10 = flow_for(n10, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_))))); it10.next();) {
      skip = false;
      if (math_compare<">">(at, self.snaps_[n10.index].at)) {
        skip = true;
      }
      if (logic_not(skip)) {
        t = clampd(kern::formula([](Float at, Float a, Float a_2) -> Float { return kern::math::div((at - a), (a_2 - a)); }, at, self.snaps_[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n10.index)))].at, self.snaps_[n10.index].at), 0.0, 1.0).Result;
        p = vector_add(self.snaps_[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n10.index)))].p, vector_scale(vector_sub(self.snaps_[n10.index].p, self.snaps_[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n10.index)))].p), t));
        heading = common::wrap_pi(kern::formula([](Float a, Float the_result, Float t) -> Float { return (a + (the_result * t)); }, self.snaps_[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n10.index)))].heading, common::wrap_pi(kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.snaps_[n10.index].heading, self.snaps_[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n10.index)))].heading)).Result, t)).Result;
        return {true};
      }
    }
    p = self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_))))].p;
    heading = self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_NavTimeline_Snap>(self.snaps_))))].heading;
    return {true};
  }

  struct hash_idResult { U32 Result = 0; };
  KERN_PURE hash_idResult hash_id(const Text& id) {
    U32 h = 0;
    Int c = 0;
    h = 2166136261;
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(id))))); it11.next();) {
      c = text_code_at(id, n11.index);
      h = kern::to<Int>(kern::formula([](Float h, Float c) -> Float { return kern::math::bxor(h, c); }, kern::to<Float>(h), kern::to<Float>(c)));
      h = kern::to<Int>(kern::formula([](Float h) -> Float { return kern::math::mul32(h, 16777619.0); }, kern::to<Float>(h)));
    }
    return {h};
  }

  struct assign_colorsResult { List<U32> Result = List<U32>{}; };
  assign_colorsResult assign_colors(const List<Text>& ids) {
    List<U32> numbers = List<U32>{};
    List<U32> out = List<U32>{};
    numbers = List<U32>{};
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text>(ids))))); it12.next();) {
      list_add(numbers, hash_id(ids[n12.index]).Result);
    }
    out = List<U32>{};
    func_call_on<"Multiplayer_ColorSlots">(actor_find("Multiplayer"), kern::out(n13.colors), numbers);
    out = n13.colors;
    return {out};
  }

  struct color_ofResult { U32 Result = 0; };
  color_ofResult color_of(const Text& id) {
    List<U32> one = List<U32>{};
    List<Text> a = List<Text>{};
    a = list_make<Text, "1">(id);
    n14 = assign_colors(a);
    one = n14.Result;
    return {kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? 0.0 : a_2); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<U32>(one)), 0.0)), kern::to<Float>(one[0])))};
  }

  struct weapon_labelResult { Text Result = ""; };
  weapon_labelResult weapon_label(mp_Weapon w = mp_Weapon::other) {
    Text out = "";
    out = "";
    func_call_on<"Multiplayer_WeaponLabel">(actor_find("Multiplayer"), kern::out(n15.label), w);
    out = n15.label;
    return {out};
  }

  struct zone_nameResult { Text Result = ""; };
  zone_nameResult zone_name(mp_Zone z = mp_Zone::torso) {
    Text out = "";
    out = "";
    func_call_on<"Multiplayer_ZoneName">(actor_find("Multiplayer"), kern::out(n16.name), z);
    out = n16.name;
    return {out};
  }

  struct round_damageResult { Float Result = 0.0; };
  round_damageResult round_damage(mp_Weapon w = mp_Weapon::other, mp_Zone z = mp_Zone::torso) {
    Float damage = 0.0;
    damage = 0.0;
    func_call_on<"Multiplayer_RoundDamage">(actor_find("Multiplayer"), kern::out(n17.damage), w, z);
    damage = n17.damage;
    return {damage};
  }

  struct blast_damageResult { Float Result = 0.0; };
  blast_damageResult blast_damage(Float damage = 0.0) {
    Float dealt = 0.0;
    dealt = 0.0;
    func_call_on<"Multiplayer_BlastDamage">(actor_find("Multiplayer"), kern::out(n18.damage), blueprint_call::decimal(damage).Result);
    dealt = n18.damage;
    return {dealt};
  }

  struct judge_hitResult { mp_HitVerdict Result = mp_HitVerdict::applied; };
  judge_hitResult judge_hit(Bool target_known = false, Bool shooter_known = false, Bool self_damage = false, Bool friendly_fire = false, Float now = 0.0, Float protected_until = 0.0, Float hp = 0.0) {
    mp_HitVerdict verdict = mp_HitVerdict::applied;
    verdict = mp_HitVerdict::applied;
    func_call_on<"Multiplayer_JudgeHit">(actor_find("Multiplayer"), kern::out(n19.verdict), target_known, shooter_known, self_damage, friendly_fire, now, protected_until, blueprint_call::decimal(hp).Result);
    verdict = n19.verdict;
    return {verdict};
  }

  struct regenResult { Float Result = 0.0; };
  regenResult regen(Float hp = 0.0, Float now = 0.0, Float last_hurt = 0.0, Float dt = 0.0) {
    Float healed = 0.0;
    healed = 0.0;
    func_call_on<"Multiplayer_Regen">(actor_find("Multiplayer"), kern::out(n20.hp), blueprint_call::decimal(hp).Result, now, last_hurt, blueprint_call::decimal(dt).Result);
    healed = n20.hp;
    return {healed};
  }

  struct respawn_offsetResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  respawn_offsetResult respawn_offset(Rng& rng) {
    Float r1 = 0.0;
    Float r2 = 0.0;
    Float x = 0.0;
    Float y = 0.0;
    n21 = common::Rng_unit(rng);
    r1 = n21.Result;
    n22 = common::Rng_unit(rng);
    r2 = n22.Result;
    x = 0.0;
    y = 0.0;
    func_call_on<"Multiplayer_RespawnOffset">(actor_find("Multiplayer"), kern::out(n23.x, n23.y), blueprint_call::decimal(r1).Result, blueprint_call::decimal(r2).Result);
    x = n23.x;
    y = n23.y;
    return {vector_make(x, y, 0.0)};
  }

  void Scoreboard_ensure(mp_Scoreboard& self, U8 slot = 0) {
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Score>(self.rows_))))); it24.next();) {
      if (math_compare<"==">(kern::to<Float>(self.rows_[n24.index].slot), kern::to<Float>(slot))) {
        return;
      }
    }
    list_add(self.rows_, struct_make<mp_Score>(slot, 0, 0));
  }

  struct remove_matchResult { Bool Result = false; };
  KERN_PURE remove_matchResult remove_match(const mp_Score& s, U8 slot = 0) {
    return {math_compare<"==">(kern::to<Float>(s.slot), kern::to<Float>(slot))};
  }

  void Scoreboard_remove(mp_Scoreboard& self, U8 slot = 0) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    at = 0;
    kept = at;
    end = list_length<mp_Score>(self.rows_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(remove_match(self.rows_[at], slot).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n25, self.rows_[at]);
          self.rows_[kept] = n25.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.rows_, kept, mp_Score{0, 0, 0});
  }

  void Scoreboard_died(mp_Scoreboard& self, U8 victim = 0, I32 by = 0) {
    Scoreboard_ensure(self, victim);
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Score>(self.rows_))))); it26.next();) {
      if (math_compare<"==">(kern::to<Float>(self.rows_[n26.index].slot), kern::to<Float>(victim))) {
        self.rows_[n26.index].deaths = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(self.rows_[n26.index].deaths)));
      }
    }
    if (logic_and(math_compare<">=">(kern::to<Float>(by), 0.0), math_compare<"!=">(kern::to<Float>(by), kern::to<Float>(victim)))) {
      Scoreboard_ensure(self, kern::to<Int>(kern::formula([](Float by) -> Float { return kern::math::band(by, 255.0); }, kern::to<Float>(by))));
      for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Score>(self.rows_))))); it27.next();) {
        if (math_compare<"==">(kern::to<Float>(self.rows_[n27.index].slot), kern::to<Float>(by))) {
          self.rows_[n27.index].kills = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(self.rows_[n27.index].kills)));
        }
      }
    }
  }

  struct Scoreboard_ofResult { mp_Score Result = mp_Score{0, 0, 0}; };
  KERN_PURE Scoreboard_ofResult Scoreboard_of(const mp_Scoreboard& self, U8 slot = 0) {
    for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Score>(self.rows_))))); it28.next();) {
      if (math_compare<"==">(kern::to<Float>(self.rows_[n28.index].slot), kern::to<Float>(slot))) {
        return {self.rows_[n28.index]};
      }
    }
    return {struct_make<mp_Score>(slot, 0, 0)};
  }

  struct ranked_matchResult { Bool Result = false; };
  KERN_PURE ranked_matchResult ranked_match(const mp_Score& a, const mp_Score& b) {
    return {logic_select<Bool>(math_compare<"!=">(kern::to<Float>(a.kills), kern::to<Float>(b.kills)), math_compare<">">(kern::to<Float>(a.kills), kern::to<Float>(b.kills)), math_compare<"<">(kern::to<Float>(a.deaths), kern::to<Float>(b.deaths)))};
  }

  struct Scoreboard_rankedResult { List<mp_Score> Result = List<mp_Score>{}; };
  KERN_PURE Scoreboard_rankedResult Scoreboard_ranked(const mp_Scoreboard& self) {
    List<mp_Score> r = List<mp_Score>{};
    r = self.rows_;
    list_sort<&mp_rules_cpp::ranked_match>(*this, r);
    return {r};
  }

  struct group_spawn_centerResult { Bool Result = false; };
  group_spawn_centerResult group_spawn_center(const List<Vector>& pts, Vector& out, Float link = 0.0) {
    List<Bool> seen = List<Bool>{};
    List<Bool> made = List<Bool>{};
    List<Vector> best = List<Vector>{};
    Bool skip = false;
    List<Int> queue = List<Int>{};
    List<Vector> comp = List<Vector>{};
    Int k = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Float score = 0.0;
    Float d = 0.0;
    if (math_compare<"==">(kern::to<Float>(list_length<Vector>(pts)), 0.0)) {
      return {false};
    }
    made = List<Bool>{};
    list_resize(made, list_length<Vector>(pts), false);
    seen = made;
    best = List<Vector>{};
    for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it29.next();) {
      skip = false;
      if (seen[n29.index]) {
        skip = true;
      }
      if (logic_not(skip)) {
        queue = list_make<Int, "1">(n29.index);
        seen[n29.index] = true;
        comp = List<Vector>{};
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Int>(queue))), 0.5))) {
          a = pts[queue[k]];
          list_add(comp, a);
          for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it30.next();) {
            if (logic_and(logic_not(seen[n30.index]), math_compare<"<=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(pts[n30.index]).x, vector_break(a).y, vector_break(pts[n30.index]).y), link))) {
              seen[n30.index] = true;
              list_add(queue, n30.index);
            }
          }
          k += 1;
        }
        if (math_compare<">">(kern::to<Float>(list_length<Vector>(comp)), kern::to<Float>(list_length<Vector>(best)))) {
          best = comp;
        }
      }
    }
    score = 1e+30;
    for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(best))))); it31.next();) {
      d = 0.0;
      for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(best))))); it32.next();) {
        d += kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(best[n31.index]).x, vector_break(best[n32.index]).x, vector_break(best[n31.index]).y, vector_break(best[n32.index]).y);
      }
      if (math_compare<"<">(d, score)) {
        score = d;
        out = best[n31.index];
      }
    }
    return {true};
  }

  struct spawn_ring_offsetResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  spawn_ring_offsetResult spawn_ring_offset(U32 slot = 0) {
    Float x = 0.0;
    Float y = 0.0;
    x = 0.0;
    y = 0.0;
    func_call_on<"Multiplayer_SpawnRingOffset">(actor_find("Multiplayer"), kern::out(n33.x, n33.y), slot);
    x = n33.x;
    y = n33.y;
    return {vector_make(x, y, 0.0)};
  }

  /// A roster as the vote compares it: in order, each player once.
  void roster(List<U8>& members) {
    Int kept = 0;
    Int count = 0;
    list_sort<nullptr>(members);
    count = list_length<U8>(members);
    if (math_compare<">">(kern::to<Float>(count), 0.0)) {
      kept = 0;
      for (auto it34 = flow_for(n34, 1, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)))); it34.next();) {
        if (logic_not(math_compare<"==">(kern::to<Float>(members[kept]), kern::to<Float>(members[n34.index])))) {
          kept += 1;
          members[kept] = members[n34.index];
        }
      }
      list_resize(members, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(kept))), 0);
    }
  }

  struct create_ballotResult { Bool Result = false; };
  create_ballotResult create_ballot(mp_Ballot& b, U32 id = 0, U8 key = 0, Bool value = false, U8 host = 0, List<U8> members = List<U8>{}, Float now = 0.0) {
    List<U8> members_ = List<U8>{};
    Bool created = false;
    members_ = members;
    roster(members_);
    created = false;
    func_call_on<"Multiplayer_CreateBallot">(actor_find("Multiplayer"), kern::out(n35.created, n35.ballot), id, key, value, host, members_, now);
    created = n35.created;
    b = n35.ballot;
    return {created};
  }

  struct cast_voteResult { mp_Ballot_Status Result = mp_Ballot_Status::pending; };
  cast_voteResult cast_vote(mp_Ballot& b, mp_Ballot_Status& did, U8 voter = 0, Bool approve = false, List<U8> current = List<U8>{}, Float now = 0.0) {
    List<U8> current_ = List<U8>{};
    current_ = current;
    roster(current_);
    func_call_on<"Multiplayer_CastVote">(actor_find("Multiplayer"), kern::out(n36.ballot, n36.did), b, voter, approve, current_, now);
    b = n36.ballot;
    did = n36.did;
    return {b.status};
  }

  struct cast_vote_2Result { mp_Ballot_Status Result = mp_Ballot_Status::pending; };
  cast_vote_2Result cast_vote_2(mp_Ballot& b, U8 voter = 0, Bool approve = false, List<U8> current = List<U8>{}, Float now = 0.0) {
    mp_Ballot_Status did = mp_Ballot_Status::pending;
    did = b.status;
    n37 = cast_vote(b, did, voter, approve, current, now);
    return {n37.Result};
  }

  struct ballot_staleResult { Bool Result = false; };
  ballot_staleResult ballot_stale(const mp_Ballot& b, const List<U8>& current, Float now = 0.0) {
    func_call_on<"Multiplayer_Stale">(actor_find("Multiplayer"), kern::out(n38.stale), b, current, now);
    return {n38.stale};
  }

  struct ballot_nameResult { Text Result = ""; };
  ballot_nameResult ballot_name(mp_Ballot_Status s = mp_Ballot_Status::pending) {
    Text out = "";
    out = "";
    func_call_on<"Multiplayer_BallotName">(actor_find("Multiplayer"), kern::out(n39.name), s);
    out = n39.name;
    return {out};
  }

  struct encode_mulawResult { U8 Result = 0; };
  KERN_PURE encode_mulawResult encode_mulaw(Float sample = 0.0) {
    Float v = 0.0;
    I32 x = 0;
    I32 the_exp = 0;
    I32 mask = 0;
    I32 mant = 0;
    v = kern::formula([](Float sample) -> Float { return (sample < (-1.0) ? (-1.0) : (1.0 < sample ? 1.0 : sample)); }, sample);
    x = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::min(32635.0, kern::math::wrap32s(kern::math::round((kern::math::abs(v) * 32767.0)))); }, v));
    the_exp = 7;
    x += 132;
    mask = 16384;
    while (loop(logic_and(math_compare<">">(kern::to<Float>(the_exp), 0.0), logic_not(math_compare<">">(kern::formula([](Float x, Float mask) -> Float { return (kern::math::band(x, mask) != 0.0); }, kern::to<Float>(x), kern::to<Float>(mask)), 0.5))))) {
      the_exp = kern::to<Int>(kern::formula([](Float the_exp) -> Float { return (the_exp - 1.0); }, kern::to<Float>(the_exp)));
      mask = kern::to<Int>(kern::formula([](Float mask) -> Float { return kern::math::shr(mask, 1.0); }, kern::to<Float>(mask)));
    }
    mant = kern::to<Int>(kern::formula([](Float x, Float the_exp) -> Float { return kern::math::band(kern::math::shr(x, (the_exp + 3.0)), 15.0); }, kern::to<Float>(x), kern::to<Float>(the_exp)));
    return {kern::to<Int>(kern::formula([](Float v, Float the_exp, Float mant) -> Float { return kern::math::band(kern::math::band(kern::math::bxor(((v < 0.0) ? 127.0 : 255.0), kern::math::bor(kern::math::wrap32s(kern::math::shl(the_exp, 4.0)), mant)), 255.0), 255.0); }, v, kern::to<Float>(the_exp), kern::to<Float>(mant)))};
  }

  struct decode_mulawResult { Float Result = 0.0; };
  KERN_PURE decode_mulawResult decode_mulaw(U8 byte = 0) {
    I32 x = 0;
    I32 the_sign = 0;
    I32 the_exp = 0;
    I32 mant = 0;
    I32 pcm = 0;
    x = kern::to<Int>(kern::formula([](Float byte) -> Float { return kern::math::band(kern::math::bxor(byte, -1.0), 255.0); }, kern::to<Float>(byte)));
    the_sign = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 128.0); }, kern::to<Float>(x)));
    the_exp = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(kern::math::shr(x, 4.0), 7.0); }, kern::to<Float>(x)));
    mant = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 15.0); }, kern::to<Float>(x)));
    pcm = kern::to<Int>(kern::formula([](Float mant, Float the_exp) -> Float { return kern::math::wrap32s(kern::math::shl((kern::math::wrap32s(kern::math::shl(mant, 3.0)) + 132.0), the_exp)); }, kern::to<Float>(mant), kern::to<Float>(the_exp)));
    return {kern::formula([](Float the_sign, Float pcm) -> Float { return kern::math::div(((the_sign != 0.0) ? (132.0 - pcm) : (pcm - 132.0)), 32768.0); }, kern::to<Float>(the_sign), kern::to<Float>(pcm))};
  }

  struct voice_gainResult { Float Result = 0.0; };
  voice_gainResult voice_gain(Float d = 0.0, Bool everywhere = false) {
    Float d_ = 0.0;
    d_ = d;
    if (logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, d_), 0.0))) {
      if (logic_not(everywhere)) {
        return {0.0};
      }
      d_ = 1000000000.0;
    }
    func_call_on<"Multiplayer_VoiceGain">(actor_find("Multiplayer"), kern::out(n40.gain), blueprint_call::decimal(d_).Result, everywhere);
    return {n40.gain};
  }

  struct proximity_gainResult { Float Result = 0.0; };
  proximity_gainResult proximity_gain(Float d = 0.0) {
    n41 = voice_gain(d, false);
    return {n41.Result};
  }

  struct VoiceStream_pushResult { Bool Result = false; };
  VoiceStream_pushResult VoiceStream_push(mp_VoiceStream& self, const List<U8>& mulaw, U32 seq = 0, Float gain = 0.0) {
    Float queued = 0.0;
    Float min_lead = 0.0;
    Float max_lead = 0.0;
    mp_VoiceStream_Chunk c = mp_VoiceStream_Chunk{List<Float>{}, 0.0};
    if (logic_or(logic_or(math_compare<"<=">(gain, 0.0), math_compare<"==">(kern::to<Float>(list_length<U8>(mulaw)), 0.0)), math_compare<">">(kern::to<Float>(list_length<U8>(mulaw)), 800.0))) {
      return {false};
    }
    if (logic_and(self.have_seq_, math_compare<"<=">(kern::to<Float>(seq), kern::to<Float>(self.last_seq_)))) {
      return {false};
    }
    self.have_seq_ = true;
    self.last_seq_ = seq;
    queued = kern::formula([](Float pos_) -> Float { return (-pos_); }, self.pos_);
    for (auto it42 = flow_for(n42, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_))))); it42.next();) {
      queued += kern::to<Float>(list_length<Float>(self.queue_[n42.index].pcm));
    }
    queued += self.lead_;
    min_lead = kern::formula([](Float kVoiceRate) -> Float { return (0.03 * kVoiceRate); }, kern::to<Float>(kVoiceRate));
    max_lead = kern::formula([](Float kVoiceRate) -> Float { return (0.15 * kVoiceRate); }, kern::to<Float>(kVoiceRate));
    if (math_compare<"==">(kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_)), 0.0)) {
      self.lead_ = min_lead;
      self.pos_ = 0.0;
    } else if (math_compare<">">(queued, kern::formula([](Float max_lead, Float kVoiceFrame) -> Float { return (max_lead + kVoiceFrame); }, max_lead, kern::to<Float>(kVoiceFrame)))) {
      while (loop(logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_)), 0.0)), math_compare<">">(queued, max_lead)))) {
        queued = kern::formula([](Float queued, Float a, Float pos_) -> Float { return (queued - ((a < 0.0 ? a + 18446744073709551616.0 : a) - pos_)); }, queued, kern::to<Float>(list_length<Float>(self.queue_[0].pcm)), self.pos_);
        self.pos_ = 0.0;
        list_remove_at(self.queue_, 0);
      }
    }
    c = mp_VoiceStream_Chunk{List<Float>{}, 0.0};
    c.gain = gain;
    list_resize(c.pcm, list_length<U8>(mulaw), 0.0);
    for (auto it43 = flow_for(n43, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U8>(mulaw))))); it43.next();) {
      c.pcm[n43.index] = decode_mulaw(mulaw[n43.index]).Result;
    }
    list_add(self.queue_, c);
    return {true};
  }

  struct VoiceStream_renderResult { Bool Result = false; };
  VoiceStream_renderResult VoiceStream_render(mp_VoiceStream& self, List<Float>& out, U32 frames = 0, U32 rate = 0) {
    Float step = 0.0;
    U32 i = 0;
    Bool skip = false;
    Int k = 0;
    Float frac = 0.0;
    Float a = 0.0;
    Float b = 0.0;
    Float s = 0.0;
    if (logic_or(math_compare<"==">(kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_)), 0.0), math_compare<"==">(kern::to<Float>(rate), 0.0))) {
      return {false};
    }
    step = kern::formula([](Float kVoiceRate, Float rate) -> Float { return kern::math::div(kVoiceRate, rate); }, kern::to<Float>(kVoiceRate), kern::to<Float>(rate));
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(frames)), logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_)), 0.0))))) {
      skip = false;
      if (math_compare<">">(self.lead_, 0.0)) {
        self.lead_ = kern::formula([](Float lead_, Float step) -> Float { return (lead_ - step); }, self.lead_, step);
        skip = true;
      }
      if (logic_not(skip)) {
        k = kern::to<Int>(kern::formula([](Float pos_) -> Float { return kern::math::trunc(pos_); }, self.pos_));
        frac = kern::formula([](Float pos_, Float k) -> Float { return (pos_ - (k < 0.0 ? k + 18446744073709551616.0 : k)); }, self.pos_, kern::to<Float>(k));
        a = self.queue_[0].pcm[k];
        b = kern::formula([](Float k, Float a, Float a_2, Float a_3, Float a_4, Float a_5) -> Float { return (((k + 1.0) < a) ? a_2 : ((a_3 > 1.0) ? a_4 : a_5)); }, kern::to<Float>(k), kern::to<Float>(list_length<Float>(self.queue_[0].pcm)), self.queue_[0].pcm[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_)), self.queue_[1].pcm[0], a);
        s = kern::formula([](Float a, Float b, Float frac, Float a_2) -> Float { return ((a + ((b - a) * frac)) * a_2); }, a, b, frac, self.queue_[0].gain);
        out[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mul32(2.0, i); }, kern::to<Float>(i)))] = kern::formula([](Float a, Float s) -> Float { return (a + s); }, out[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mul32(2.0, i); }, kern::to<Float>(i)))], s);
        out[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32(kern::math::mul32(2.0, i) + 1.0); }, kern::to<Float>(i)))] = kern::formula([](Float a, Float s) -> Float { return (a + s); }, out[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32(kern::math::mul32(2.0, i) + 1.0); }, kern::to<Float>(i)))], s);
        self.pos_ = kern::formula([](Float pos_, Float step) -> Float { return (pos_ + step); }, self.pos_, step);
        if (math_compare<">=">(self.pos_, kern::to<Float>(list_length<Float>(self.queue_[0].pcm)))) {
          self.pos_ = kern::formula([](Float pos_, Float a) -> Float { return (pos_ - (a < 0.0 ? a + 18446744073709551616.0 : a)); }, self.pos_, kern::to<Float>(list_length<Float>(self.queue_[0].pcm)));
          list_remove_at(self.queue_, 0);
        }
      }
      i += 1;
    }
    return {logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_)), 0.0))};
  }

  void VoiceCapture_push(mp_VoiceCapture& self, List<Float> samples = List<Float>{}, U32 rate = 0) {
    U32 count = 0;
    Float stride = 0.0;
    Float i = 0.0;
    List<U8> f = List<U8>{};
    List<U8> made = List<U8>{};
    if (logic_not(math_compare<"!=">(kern::to<Float>(rate), 0.0))) {
      return;
    }
    count = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(samples))));
    stride = kern::formula([](Float rate, Float kVoiceRate) -> Float { return kern::math::div(rate, kVoiceRate); }, kern::to<Float>(rate), kern::to<Float>(kVoiceRate));
    i = self.phase_;
    while (loop(math_compare<"<">(i, kern::to<Float>(count)))) {
      list_add(self.pending_, samples[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(i); }, i))]);
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(self.pending_)), kern::to<Float>(kVoiceFrame))) {
        made = List<U8>{};
        list_resize(made, kVoiceFrame, 0);
        f = made;
        for (auto it44 = flow_for(n44, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kVoiceFrame)))); it44.next();) {
          f[n44.index] = encode_mulaw(self.pending_[n44.index]).Result;
        }
        list_add(self.frames_, struct_make<U8_row>(f));
        list_clear(self.pending_);
        if (math_compare<">">(kern::to<Float>(list_length<U8_row>(self.frames_)), 8.0)) {
          list_remove_at(self.frames_, 0);
        }
      }
      i += stride;
    }
    self.phase_ = kern::formula([](Float i, Float count) -> Float { return (i - count); }, i, kern::to<Float>(count));
  }

  struct VoiceCapture_takeResult { Bool Result = false; };
  VoiceCapture_takeResult VoiceCapture_take(mp_VoiceCapture& self, List<U8>& frame) {
    if (math_compare<"==">(kern::to<Float>(list_length<U8_row>(self.frames_)), 0.0)) {
      return {false};
    }
    var_take(n45, self.frames_[0].items);
    frame = n45.value;
    list_remove_at(self.frames_, 0);
    return {true};
  }

  struct project_onResult { mp_Along Result = mp_Along{0.0, 1e+30}; };
  project_onResult project_on(const List<Vector>& pts, List<Float>& cum, Vector pos = Vector{0.0, 0.0, 0.0}) {
    Int count = 0;
    Float fill = 0.0;
    Int i = 0;
    mp_Along best = mp_Along{0.0, 1e+30};
    Int i_2 = 0;
    Bool skip = false;
    Float len = 0.0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float t = 0.0;
    Float d = 0.0;
    count = 1;
    fill = 0.0;
    list_clear(cum);
    list_resize(cum, count, fill);
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(pts))), 0.5))) {
      list_add(cum, kern::formula([](Float a, Float x, Float x_2, Float y, Float y_2) -> Float { return (a + kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, cum[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(cum))))], vector_break(pts[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))]).x, vector_break(pts[i]).x, vector_break(pts[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))]).y, vector_break(pts[i]).y));
      i += 1;
    }
    best = mp_Along{0.0, 1e+30};
    i_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2))))), kern::to<Float>(list_length<Vector>(pts))), 0.5))) {
      skip = false;
      len = kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, cum[kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2)))], cum[i_2]);
      if (math_compare<"<">(len, 0.001)) {
        skip = true;
      }
      if (logic_not(skip)) {
        a = pts[i_2];
        b = pts[kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2)))];
        t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float len) -> Float { return kern::math::div((((x - x_2) * (x_3 - x_2)) + ((y - y_2) * (y_3 - y_2))), (len * len)); }, vector_break(pos).x, vector_break(a).x, vector_break(b).x, vector_break(pos).y, vector_break(a).y, vector_break(b).y, len), 0.0, 1.0));
        d = kern::formula([](Float x, Float x_2, Float x_3, Float t, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt(((x - (x_2 + ((x_3 - x_2) * t))) * (x - (x_2 + ((x_3 - x_2) * t)))) + ((y - (y_2 + ((y_3 - y_2) * t))) * (y - (y_2 + ((y_3 - y_2) * t))))); }, vector_break(pos).x, vector_break(a).x, vector_break(b).x, t, vector_break(pos).y, vector_break(a).y, vector_break(b).y);
        if (math_compare<"<">(d, best.distance)) {
          best.distance = d;
          best.along = kern::formula([](Float a, Float len, Float t) -> Float { return (a + (len * t)); }, cum[i_2], len, t);
        }
      }
      i_2 += 1;
    }
    return {best};
  }

  struct point_atResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE point_atResult point_at(const List<Vector>& pts, const List<Float>& cum, Float d = 0.0) {
    Float d_ = 0.0;
    Int i = 0;
    Float len = 0.0;
    Float t = 0.0;
    d_ = d;
    d_ = kern::formula([](Float d_, Float a) -> Float { return (d_ < 0.0 ? 0.0 : (a < d_ ? a : d_)); }, d_, cum[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(cum))))]);
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Float>(cum))), 0.5), math_compare<"<">(cum[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], d_)))) {
      i += 1;
    }
    len = kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, cum[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], cum[i]);
    t = kern::formula([](Float len, Float a) -> Float { return ((len > 1e-06) ? a : 0.0); }, len, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float d_, Float a, Float len) -> Float { return kern::math::div((d_ - a), len); }, d_, cum[i], len), 0.0, 1.0));
    return {vector_make(kern::formula([](Float x, Float x_2, Float t) -> Float { return (x + ((x_2 - x) * t)); }, vector_break(pts[i]).x, vector_break(pts[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))]).x, t), kern::formula([](Float y, Float y_2, Float t) -> Float { return (y + ((y_2 - y) * t)); }, vector_break(pts[i]).y, vector_break(pts[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))]).y, t), 0.0)};
  }

  struct sample_breadcrumbsResult { List<mp_Crumb> Result = List<mp_Crumb>{}; };
  sample_breadcrumbsResult sample_breadcrumbs(const List<Vector>& pts, Vector pos = Vector{0.0, 0.0, 0.0}, U32 count = 0, Float spacing = 0.0, Float start = 0.0) {
    List<mp_Crumb> out = List<mp_Crumb>{};
    List<Float> cum = List<Float>{};
    mp_Along a = mp_Along{0.0, 1e+30};
    Float total = 0.0;
    U32 n = 0;
    Float step = 0.0;
    Float first = 0.0;
    U32 j = 0;
    Bool stop = false;
    Float at = 0.0;
    Int i = 0;
    Bool stop_2 = false;
    Bool skip = false;
    Float len = 0.0;
    Float t = 0.0;
    Vector p0 = Vector{0.0, 0.0, 0.0};
    Vector p1 = Vector{0.0, 0.0, 0.0};
    out = List<mp_Crumb>{};
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(pts)), 2.0)) {
      return {out};
    }
    cum = List<Float>{};
    n46 = project_on(pts, cum, pos);
    a = n46.Result;
    total = cum[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(cum))))];
    if (math_compare<"<">(total, 0.01)) {
      return {out};
    }
    n = kern::to<Int>(kern::formula([](Float count) -> Float { return kern::math::min(count, 8.0); }, kern::to<Float>(count)));
    step = kern::formula([](Float spacing) -> Float { return (spacing < 2.0 ? 2.0 : (500.0 < spacing ? 500.0 : spacing)); }, spacing);
    first = kern::formula([](Float start) -> Float { return (start < 1.0 ? 1.0 : (500.0 < start ? 500.0 : start)); }, start);
    j = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(j), kern::to<Float>(n))))) {
      at = kern::formula([](Float along, Float first, Float j, Float step) -> Float { return ((along + first) + (j * step)); }, a.along, first, kern::to<Float>(j), step);
      if (math_compare<">">(at, kern::formula([](Float total) -> Float { return (total + 0.001); }, total))) {
        stop = true;
      }
      if (logic_not(stop)) {
        i = 0;
        stop_2 = false;
        while (loop(logic_and(logic_not(stop_2), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(pts))), 0.5)))) {
          skip = false;
          len = kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, cum[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], cum[i]);
          if (logic_or(math_compare<">">(at, kern::formula([](Float a) -> Float { return (a + 0.0001); }, cum[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))])), math_compare<"<">(len, 0.001))) {
            skip = true;
          }
          if (logic_not(logic_or(stop_2, skip))) {
            t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float at, Float a, Float len) -> Float { return kern::math::div((at - a), len); }, at, cum[i], len), 0.0, 1.0));
            p0 = pts[i];
            p1 = pts[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
            list_add(out, struct_make<mp_Crumb>(vector_make(kern::formula([](Float x, Float x_2, Float t) -> Float { return (x + ((x_2 - x) * t)); }, vector_break(p0).x, vector_break(p1).x, t), kern::formula([](Float y, Float y_2, Float t) -> Float { return (y + ((y_2 - y) * t)); }, vector_break(p0).y, vector_break(p1).y, t), 0.0), kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::atan2((x - x_2), (y - y_2)); }, vector_break(p1).x, vector_break(p0).x, vector_break(p1).y, vector_break(p0).y), kern::formula([](Float total, Float at) -> Float { return (total - at); }, total, at)));
            stop_2 = true;
          }
          if (logic_not(stop_2)) {
            i += 1;
          }
        }
      }
      if (logic_not(stop)) {
        j += 1;
      }
    }
    return {out};
  }

  struct upcoming_turnResult { mp_Turn Result = mp_Turn{mp_Turn_Dir::none, 0.0, 0.0}; };
  upcoming_turnResult upcoming_turn(const List<Vector>& pts, Vector pos = Vector{0.0, 0.0, 0.0}, Float scan = 0.0, Float window = 0.0, Float min_angle = 0.0) {
    Float scan_ = 0.0;
    Float window_ = 0.0;
    Float min_angle_ = 0.0;
    mp_Turn out = mp_Turn{mp_Turn_Dir::none, 0.0, 0.0};
    List<Float> cum = List<Float>{};
    mp_Along a = mp_Along{0.0, 1e+30};
    Int i = 0;
    Bool skip = false;
    Float away = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector prev = Vector{0.0, 0.0, 0.0};
    Vector next = Vector{0.0, 0.0, 0.0};
    Float bx = 0.0;
    Float by = 0.0;
    Float ax = 0.0;
    Float ay = 0.0;
    Float angle = 0.0;
    scan_ = scan;
    window_ = window;
    min_angle_ = min_angle;
    out = mp_Turn{mp_Turn_Dir::none, 0.0, 0.0};
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(pts)), 3.0)) {
      return {out};
    }
    cum = List<Float>{};
    n47 = project_on(pts, cum, pos);
    a = n47.Result;
    if (math_compare<"<">(cum[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(cum))))], 2.0)) {
      return {out};
    }
    scan_ = kern::formula([](Float scan_) -> Float { return (scan_ < 5.0 ? 5.0 : (1000.0 < scan_ ? 1000.0 : scan_)); }, scan_);
    window_ = kern::formula([](Float window_) -> Float { return (window_ < 3.0 ? 3.0 : (80.0 < window_ ? 80.0 : window_)); }, window_);
    min_angle_ = kern::formula([](Float min_angle_) -> Float { return (min_angle_ < 15.0 ? 15.0 : (160.0 < min_angle_ ? 160.0 : min_angle_)); }, min_angle_);
    i = 1;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(pts))), 0.5))) {
      skip = false;
      away = kern::formula([](Float a, Float along) -> Float { return (a - along); }, cum[i], a.along);
      if (logic_or(math_compare<"<">(away, 1.0), math_compare<">">(away, scan_))) {
        skip = true;
      }
      if (logic_not(skip)) {
        c = pts[i];
        prev = point_at(pts, cum, kern::formula([](Float a, Float window_) -> Float { return (a - window_); }, cum[i], window_)).Result;
        next = point_at(pts, cum, kern::formula([](Float a, Float window_) -> Float { return (a + window_); }, cum[i], window_)).Result;
        bx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(c).x, vector_break(prev).x);
        by = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(c).y, vector_break(prev).y);
        ax = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(next).x, vector_break(c).x);
        ay = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(next).y, vector_break(c).y);
        if (logic_or(math_compare<"<">(kern::formula([](Float bx, Float by) -> Float { return kern::math::sqrt((bx * bx) + (by * by)); }, bx, by), 2.0), math_compare<"<">(kern::formula([](Float ax, Float ay) -> Float { return kern::math::sqrt((ax * ax) + (ay * ay)); }, ax, ay), 2.0))) {
          skip = true;
        }
        if (logic_not(skip)) {
          angle = kern::formula([](Float bx, Float ay, Float by, Float ax, Float kDeg) -> Float { return kern::math::div(kern::math::atan2(((bx * ay) - (by * ax)), ((bx * ax) + (by * ay))), kDeg); }, bx, ay, by, ax, kDeg);
          if (math_compare<"<">(kern::formula([](Float angle) -> Float { return kern::math::abs(angle); }, angle), min_angle_)) {
            skip = true;
          }
          if (logic_not(skip)) {
            out.dir = logic_select<mp_Turn_Dir>(math_compare<">">(kern::formula([](Float angle) -> Float { return kern::math::abs(angle); }, angle), 155.0), mp_Turn_Dir::u_turn, logic_select<mp_Turn_Dir>(math_compare<"<">(angle, 0.0), mp_Turn_Dir::right, mp_Turn_Dir::left));
            out.distance = away;
            out.angle_deg = kern::formula([](Float angle) -> Float { return kern::math::abs(angle); }, angle);
            return {out};
          }
        }
      }
      i += 1;
    }
    return {out};
  }

  struct route_leftResult { Float Result = 0.0; };
  route_leftResult route_left(const List<Vector>& pts, Vector pos = Vector{0.0, 0.0, 0.0}) {
    List<Float> cum = List<Float>{};
    mp_Along a = mp_Along{0.0, 1e+30};
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(pts)), 2.0)) {
      return {kern::formula([](Float a, Float x, Float x_2, Float y, Float y_2) -> Float { return ((a != 0.0) ? 0.0 : kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<Vector>(pts)), 0.0)), vector_break(pts[0]).x, vector_break(pos).x, vector_break(pts[0]).y, vector_break(pos).y)};
    }
    cum = List<Float>{};
    n48 = project_on(pts, cum, pos);
    a = n48.Result;
    return {kern::formula([](Float a, Float along, Float distance) -> Float { return ((a - along) + distance); }, cum[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(cum))))], a.along, a.distance)};
  }

  struct bearing_arrowResult { Text Result = ""; };
  bearing_arrowResult bearing_arrow(Float b = 0.0) {
    Text out = "";
    out = "";
    func_call_on<"Multiplayer_BearingArrow">(actor_find("Multiplayer"), kern::out(n49.arrow), blueprint_call::decimal(b).Result);
    out = n49.arrow;
    return {out};
  }

  struct locate_map_peerResult { mp_MapEdge Result = mp_MapEdge{Vector{0.0, 0.0, 0.0}, false, 0.0, "", false}; };
  locate_map_peerResult locate_map_peer(Vector p = Vector{0.0, 0.0, 0.0}, Float w = 0.0, Float h = 0.0, Float ix = 0.0, Float it = 0.0, Float ib = 0.0) {
    mp_MapEdge out = mp_MapEdge{Vector{0.0, 0.0, 0.0}, false, 0.0, "", false};
    Float x = 0.0;
    Float y = 0.0;
    out = mp_MapEdge{Vector{0.0, 0.0, 0.0}, false, 0.0, "", false};
    if (logic_not(logic_and(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).x), 0.0), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).y), 0.0)))) {
      return {out};
    }
    x = 0.0;
    y = 0.0;
    func_call_on<"Multiplayer_LocateMapPeer">(actor_find("Multiplayer"), kern::out(n50.valid, n50.x, n50.y, n50.offmap, n50.rotationDeg, n50.arrow), blueprint_call::decimal(vector_break(p).x).Result, blueprint_call::decimal(vector_break(p).y).Result, blueprint_call::decimal(w).Result, blueprint_call::decimal(h).Result, blueprint_call::decimal(ix).Result, blueprint_call::decimal(it).Result, blueprint_call::decimal(ib).Result);
    out.valid = n50.valid;
    x = n50.x;
    y = n50.y;
    out.offmap = n50.offmap;
    out.rotation_deg = n50.rotationDeg;
    out.arrow = n50.arrow;
    out.p = vector_make(x, y, 0.0);
    return {out};
  }

  struct safe_mobile_marker_pointResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  safe_mobile_marker_pointResult safe_mobile_marker_point(Vector p = Vector{0.0, 0.0, 0.0}, Float w = 0.0, Float h = 0.0, Bool mobile = false) {
    Float x = 0.0;
    Float y = 0.0;
    x = 0.0;
    y = 0.0;
    func_call_on<"Multiplayer_SafeMobileMarkerPoint">(actor_find("Multiplayer"), kern::out(n51.x, n51.y), blueprint_call::decimal(vector_break(p).x).Result, blueprint_call::decimal(vector_break(p).y).Result, blueprint_call::decimal(w).Result, blueprint_call::decimal(h).Result, mobile);
    x = n51.x;
    y = n51.y;
    return {vector_make(x, y, 0.0)};
  }

  struct format_distanceResult { Text Result = ""; };
  KERN_PURE format_distanceResult format_distance(Float m = 0.0) {
    Text b = "";
    Text s = "";
    b = "";
    if (math_compare<">=">(m, 10000.0)) {
      b = text_format<"%.0f km">(31, kern::formula([](Float m) -> Float { return kern::math::div(m, 1000.0); }, m));
    } else if (math_compare<">=">(m, 1000.0)) {
      b = text_format<"%.1f km">(31, kern::formula([](Float m) -> Float { return kern::math::div(m, 1000.0); }, m));
      s = b;
      s = text_replace(s, text_find(s, ".", 0), 1, ",");
      return {s};
    } else {
      b = text_format<"%.0f m">(31, kern::formula([](Float m) -> Float { return kern::math::max(10.0, (kern::math::round(kern::math::div(m, 10.0)) * 10.0)); }, m));
    }
    return {b};
  }

  struct clear_matchResult { Bool Result = false; };
  KERN_PURE clear_matchResult clear_match(const mp_Ping& q, U8 owner = 0) {
    return {math_compare<"==">(kern::to<Float>(q.owner), kern::to<Float>(owner))};
  }

  void PingBoard_clear(mp_PingBoard& self, U8 owner = 0) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    at = 0;
    kept = at;
    end = list_length<mp_Ping>(self.pings_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(clear_match(self.pings_[at], owner).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n55, self.pings_[at]);
          self.pings_[kept] = n55.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.pings_, kept, mp_Ping{0, Vector{0.0, 0.0, 0.0}, 0, 0.0});
  }

  void PingBoard_set(mp_PingBoard& self, U8 owner = 0, Vector p = Vector{0.0, 0.0, 0.0}, U32 color = 0, Float now = 0.0) {
    for (auto it56 = flow_for(n56, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Ping>(self.pings_))))); it56.next();) {
      if (math_compare<"==">(kern::to<Float>(self.pings_[n56.index].owner), kern::to<Float>(owner))) {
        self.pings_[n56.index].p = p;
        self.pings_[n56.index].color = color;
        self.pings_[n56.index].at_ms = now;
        return;
      }
    }
    list_add(self.pings_, struct_make<mp_Ping>(owner, p, color, now));
  }

  struct PingBoard_toggleResult { Bool Result = false; };
  PingBoard_toggleResult PingBoard_toggle(mp_PingBoard& self, U8 owner = 0, Vector p = Vector{0.0, 0.0, 0.0}, U32 color = 0, Float now = 0.0) {
    Bool both = false;
    for (auto it52 = flow_for(n52, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Ping>(self.pings_))))); it52.next();) {
      both = math_compare<"==">(kern::to<Float>(self.pings_[n52.index].owner), kern::to<Float>(owner));
      if (both) {
        func_call_on<"Multiplayer_PingToggles">(actor_find("Multiplayer"), kern::out(n54.removes), blueprint_call::decimal(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.pings_[n52.index].p).x, vector_break(p).x, vector_break(self.pings_[n52.index].p).y, vector_break(p).y)).Result);
        both = n54.removes;
      }
      if (both) {
        PingBoard_clear(self, owner);
        return {false};
      }
    }
    PingBoard_set(self, owner, p, color, now);
    return {true};
  }

  struct expire_matchResult { Bool Result = false; };
  expire_matchResult expire_match(const mp_Ping& q, Float now = 0.0) {
    func_call_on<"Multiplayer_PingExpired">(actor_find("Multiplayer"), kern::out(n57.expired), kern::formula([](Float now, Float at_ms) -> Float { return (now - at_ms); }, now, q.at_ms));
    return {n57.expired};
  }

  void PingBoard_expire(mp_PingBoard& self, Float now = 0.0) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    at = 0;
    kept = at;
    end = list_length<mp_Ping>(self.pings_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      n58 = expire_match(self.pings_[at], now);
      if (logic_not(n58.Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n59, self.pings_[at]);
          self.pings_[kept] = n59.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.pings_, kept, mp_Ping{0, Vector{0.0, 0.0, 0.0}, 0, 0.0});
  }

  struct PingBoard_fadeResult { Float Result = 0.0; };
  PingBoard_fadeResult PingBoard_fade(const mp_Ping& p, Float now_ms = 0.0) {
    func_call_on<"Multiplayer_PingFade">(actor_find("Multiplayer"), kern::out(n60.fade), kern::formula([](Float now_ms, Float at_ms) -> Float { return (now_ms - at_ms); }, now_ms, p.at_ms));
    return {n60.fade};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { List<U32> colors = List<U32>{}; } n13;
  assign_colorsResult n14;
  struct { Text label = ""; } n15;
  struct { Text name = ""; } n16;
  struct { Float damage = 0.0; } n17;
  struct { Float damage = 0.0; } n18;
  struct { mp_HitVerdict verdict = mp_HitVerdict::applied; } n19;
  struct { Float hp = 0.0; } n20;
  common::Rng_unitResult n21;
  common::Rng_unitResult n22;
  struct { Float x = 0.0; Float y = 0.0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct { mp_Score value = mp_Score{0, 0, 0}; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct { Float x = 0.0; Float y = 0.0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
  struct { Bool created = false; mp_Ballot ballot = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}; } n35;
  struct { mp_Ballot ballot = mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}; mp_Ballot_Status did = mp_Ballot_Status::pending; } n36;
  cast_voteResult n37;
  struct { Bool stale = false; } n38;
  struct { Text name = ""; } n39;
  struct { Float gain = 0.0; } n40;
  voice_gainResult n41;
  struct : kern::Node<42> { Int index = 0; } n42;
  struct : kern::Node<43> { Int index = 0; } n43;
  struct : kern::Node<44> { Int index = 0; } n44;
  struct { List<U8> value = List<U8>{}; } n45;
  project_onResult n46;
  project_onResult n47;
  project_onResult n48;
  struct { Text arrow = ""; } n49;
  struct { Bool valid = false; Float x = 0.0; Float y = 0.0; Bool offmap = false; Float rotationDeg = 0.0; Text arrow = ""; } n50;
  struct { Float x = 0.0; Float y = 0.0; } n51;
  struct : kern::Node<52> { Int index = 0; } n52;
  struct { Bool removes = false; } n54;
  struct { mp_Ping value = mp_Ping{0, Vector{0.0, 0.0, 0.0}, 0, 0.0}; } n55;
  struct : kern::Node<56> { Int index = 0; } n56;
  struct { Bool expired = false; } n57;
  expire_matchResult n58;
  struct { mp_Ping value = mp_Ping{0, Vector{0.0, 0.0, 0.0}, 0, 0.0}; } n59;
  struct { Float fade = 0.0; } n60;
};
