struct perf : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(perf_Governor) { I32 tier_ = 4; I32 max_ = 4; Bool auto_ = true; Float avg_dt_ = 0.016666666666666666; Float clock_ = 0.0; Float since_change_ = 0.0; Float low_ = 0.0; Float high_ = 0.0; Fixed<Float, 5> block_until_ = List<Float>{}; Fixed<Float, 5> block_len_ = List<Float>{}; U32 changes_ = 0; };
  KERN_STRUCT(perf_Meter) { Float start_ = 0.0; Float last_ = 0.0; Fixed<Float, 4> cur_ = List<Float>{}; Fixed<Float, 4> avg_ = List<Float>{}; Float update_ = 0.0; Float render_ = 0.0; Float gpu_ = -1.0; };
  KERN_ENUM(perf_Slot, logic, physics, stream, draw, count);
  KERN_STRUCT(perf_Tier) { Text name = ""; I32 people = 0; Float shadow_m = 0.0; I32 shadow_filter = 0; I32 ao = 0; Float lamp_m = 0.0; Float people_m = 0.0; I32 lights = 0; Float render_scale = 0.0; };

  // ---- variables ----
  KERN_CONST I32 kTiers = 5;

  // ---- functions ----
  void Meter_begin_frame(perf_Meter& self, Float now_ms = 0.0) {
    self.last_ = now_ms;
    self.start_ = self.last_;
  }

  struct Meter_msResult { Float Result = 0.0; };
  KERN_PURE Meter_msResult Meter_ms(const perf_Meter& self, perf_Slot s = perf_Slot::logic) {
    return {self.avg_[enum_index<perf_Slot>(s)]};
  }

  struct Meter_update_msResult { Float Result = 0.0; };
  KERN_PURE Meter_update_msResult Meter_update_ms(const perf_Meter& self) {
    return {self.update_};
  }

  struct Meter_render_msResult { Float Result = 0.0; };
  KERN_PURE Meter_render_msResult Meter_render_ms(const perf_Meter& self) {
    return {self.render_};
  }

  struct Meter_gpu_msResult { Float Result = 0.0; };
  KERN_PURE Meter_gpu_msResult Meter_gpu_ms(const perf_Meter& self) {
    return {self.gpu_};
  }

  struct Governor_tierResult { I32 Result = 0; };
  KERN_PURE Governor_tierResult Governor_tier(const perf_Governor& self) {
    return {self.tier_};
  }

  struct Governor_max_tierResult { I32 Result = 0; };
  KERN_PURE Governor_max_tierResult Governor_max_tier(const perf_Governor& self) {
    return {self.max_};
  }

  struct Governor_automaticResult { Bool Result = false; };
  KERN_PURE Governor_automaticResult Governor_automatic(const perf_Governor& self) {
    return {self.auto_};
  }

  void Governor_set_auto(perf_Governor& self) {
    self.auto_ = true;
    self.since_change_ = 0.0;
    self.high_ = 0.0;
    self.low_ = self.high_;
  }

  struct Governor_fpsResult { Float Result = 0.0; };
  KERN_PURE Governor_fpsResult Governor_fps(const perf_Governor& self) {
    return {kern::formula([](Float avg_dt_) -> Float { return ((avg_dt_ > 0.0) ? kern::math::div(1.0, avg_dt_) : 0.0); }, self.avg_dt_)};
  }

  struct Governor_changesResult { U32 Result = 0; };
  KERN_PURE Governor_changesResult Governor_changes(const perf_Governor& self) {
    return {self.changes_};
  }

  struct Governor_blockedResult { Float Result = 0.0; };
  /// Seconds until `tier` may be tried again (0 = free).
  KERN_PURE Governor_blockedResult Governor_blocked(const perf_Governor& self, I32 tier = 0) {
    return {kern::formula([](Float tier, Float kTiers, Float a, Float clock_) -> Float { return (((tier >= 0.0) && (tier < kTiers)) ? kern::math::max(0.0, (a - clock_)) : 0.0); }, kern::to<Float>(tier), kern::to<Float>(kTiers), self.block_until_[tier], self.clock_)};
  }
};
