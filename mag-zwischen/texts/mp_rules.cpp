struct mp_rules : virtual kern::Library, virtual engine_math_math, virtual shapes {
  // ---- types ----
  KERN_ENUM(mp_Ballot_Status, pending, approved, rejected, cancelled, ignored, invalid);
  KERN_STRUCT(mp_Crumb) { Vector p = Vector{0.0, 0.0, 0.0}; Float bearing = 0.0; Float left_m = 0.0; };
  KERN_ENUM(mp_HitVerdict, applied, unknown_player, friendly_fire_off, protected_target, already_down);
  KERN_STRUCT(mp_MapEdge) { Vector p = Vector{0.0, 0.0, 0.0}; Bool offmap = false; Float rotation_deg = 0.0; Text arrow = ""; Bool valid = false; };
  KERN_STRUCT(mp_NavTimeline_Snap) { Vector p = Vector{0.0, 0.0, 0.0}; Float heading = 0.0; U8 mode = 0; Float at = 0.0; };
  KERN_STRUCT(mp_Ping) { U8 owner = 0; Vector p = Vector{0.0, 0.0, 0.0}; U32 color = 0; Float at_ms = 0.0; };
  KERN_STRUCT(mp_Score) { U8 slot = 0; U16 kills = 0; U16 deaths = 0; };
  KERN_ENUM(mp_Turn_Dir, none, left, right, u_turn);
  KERN_STRUCT(mp_VoiceStream_Chunk) { List<Float> pcm = List<Float>{}; Float gain = 0.0; };
  KERN_ENUM(mp_Weapon, other, smg, glock, sniper, fists, knife, launcher, rocket, grenade, drone_gun, drone_rocket, jet_gun, jet_missile, jet_bomb, explosion);
  KERN_ENUM(mp_Zone, torso, head, legs);
  KERN_STRUCT(mp_Ballot) { U32 id = 0; U8 key = 0; Bool value = false; U8 host = 0; List<U8> members = List<U8>{}; List<U8> yes = List<U8>{}; Float expires_ms = 0.0; mp_Ballot_Status status = mp_Ballot_Status::pending; };
  KERN_STRUCT(mp_NavTimeline) { List<mp_NavTimeline_Snap> snaps_ = List<mp_NavTimeline_Snap>{}; };
  KERN_STRUCT(mp_PingBoard) { List<mp_Ping> pings_ = List<mp_Ping>{}; };
  KERN_STRUCT(mp_PoseSample) { Vector p = Vector{0.0, 0.0, 0.0}; Quat q = Quat{0.0, 0.0, 0.0, 1.0}; Float age_ms = 0.0; Bool stale = false; Bool extrapolated = false; Bool valid = false; };
  KERN_STRUCT(mp_PoseTimeline_Snap) { Vector p = Vector{0.0, 0.0, 0.0}; Quat q = Quat{0.0, 0.0, 0.0, 1.0}; Vector v = Vector{0.0, 0.0, 0.0}; Float received = 0.0; Float source = 0.0; };
  KERN_STRUCT(mp_Scoreboard) { List<mp_Score> rows_ = List<mp_Score>{}; };
  KERN_STRUCT(mp_Turn) { mp_Turn_Dir dir = mp_Turn_Dir::none; Float distance = 0.0; Float angle_deg = 0.0; };
  KERN_STRUCT(mp_VoiceCapture) { List<Float> pending_ = List<Float>{}; List<U8_row> frames_ = List<U8_row>{}; Float phase_ = 0.0; };
  KERN_STRUCT(mp_VoiceStream) { List<mp_VoiceStream_Chunk> queue_ = List<mp_VoiceStream_Chunk>{}; Float pos_ = 0.0; Float lead_ = 0.0; U32 last_seq_ = 0; Bool have_seq_ = false; };
  KERN_STRUCT(mp_PoseTimeline) { List<mp_PoseTimeline_Snap> snaps_ = List<mp_PoseTimeline_Snap>{}; List<Float> offsets_ = List<Float>{}; Float offset_ = 0.0; Float delay_ = 90.0; List<Float> lags_ = List<Float>{}; Bool have_offset_ = false; };

  // ---- variables ----
  /// ---- pose timeline (vs_pose_sync.mjs) ----------------------------------------------------------
  KERN_CONST Float kInterpolationDelayMs = 90.0;

  // ---- functions ----
  void PoseTimeline_reset(mp_PoseTimeline& self) {
    list_clear(self.snaps_);
    list_clear(self.offsets_);
    list_clear(self.lags_);
    self.have_offset_ = false;
    self.delay_ = kInterpolationDelayMs;
  }

  struct PoseTimeline_emptyResult { Bool Result = false; };
  KERN_PURE PoseTimeline_emptyResult PoseTimeline_empty(const mp_PoseTimeline& self) {
    return {math_compare<"==">(kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_)), 0.0)};
  }

  struct PoseTimeline_last_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE PoseTimeline_last_positionResult PoseTimeline_last_position(const mp_PoseTimeline& self) {
    return {logic_select<Vector>(math_compare<"==">(kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_)), 0.0), Vector{0.0, 0.0, 0.0}, self.snaps_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_PoseTimeline_Snap>(self.snaps_))))].p)};
  }

  struct PoseTimeline_clock_offset_msResult { Float Result = 0.0; };
  KERN_PURE PoseTimeline_clock_offset_msResult PoseTimeline_clock_offset_ms(const mp_PoseTimeline& self) {
    return {self.offset_};
  }

  struct PoseTimeline_delay_msResult { Float Result = 0.0; };
  KERN_PURE PoseTimeline_delay_msResult PoseTimeline_delay_ms(const mp_PoseTimeline& self) {
    return {self.delay_};
  }

  void NavTimeline_reset(mp_NavTimeline& self) {
    list_clear(self.snaps_);
  }

  struct rgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rgbResult rgb(U32 c = 0) {
    return {vector_make(kern::formula([](Float c) -> Float { return kern::math::div(kern::math::band(kern::math::shr(c, 16.0), 255.0), 255.0); }, kern::to<Float>(c)), kern::formula([](Float c) -> Float { return kern::math::div(kern::math::band(kern::math::shr(c, 8.0), 255.0), 255.0); }, kern::to<Float>(c)), kern::formula([](Float c) -> Float { return kern::math::div(kern::math::band(c, 255.0), 255.0); }, kern::to<Float>(c)))};
  }

  void Scoreboard_clear(mp_Scoreboard& self) {
    list_clear(self.rows_);
  }

  void Scoreboard_set(mp_Scoreboard& self, const List<mp_Score>& rows) {
    self.rows_ = rows;
  }

  struct VoiceStream_speakingResult { Bool Result = false; };
  KERN_PURE VoiceStream_speakingResult VoiceStream_speaking(const mp_VoiceStream& self) {
    return {logic_not(math_compare<"==">(kern::to<Float>(list_length<mp_VoiceStream_Chunk>(self.queue_)), 0.0))};
  }

  void VoiceStream_reset(mp_VoiceStream& self) {
    list_clear(self.queue_);
    self.have_seq_ = false;
    self.lead_ = 0.0;
  }

  void VoiceCapture_clear(mp_VoiceCapture& self) {
    list_clear(self.pending_);
    list_clear(self.frames_);
    self.phase_ = 0.0;
  }

  void PingBoard_clear_all(mp_PingBoard& self) {
    list_clear(self.pings_);
  }
};
