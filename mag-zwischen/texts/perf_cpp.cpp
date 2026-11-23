struct perf_cpp : virtual kern::Library, virtual common, virtual perf {
  // ---- variables ----
  KERN_CONST I32 kTiers = 5;
  /// s after any change before the next decision (new level settles)
  KERN_CONST Float kSettle = 3.0;
  /// s after start / reset without decisions (loading, shader warm-up)
  KERN_CONST Float kWarmup = 3.0;
  /// The governor's limits.
  KERN_CONST Float kAverageTau = 1.0;
  /// below this (averaged) ...
  KERN_CONST Float kDownFps = 50.0;
  /// at or above this (vsync 60 Hz shows ~59-60) ...
  KERN_CONST Float kUpFps = 57.0;
  /// ... for this long: one level down
  KERN_CONST Float kDownHold = 2.0;
  /// a level that was too heavy is not tried again for 30 s ...
  KERN_CONST Float kBlockFirst = 30.0;
  /// ... doubling each time it fails again, up to 4 min
  KERN_CONST Float kBlockMax = 240.0;
  /// ... for this long: one level up
  KERN_CONST Float kUpHold = 8.0;

  // ---- functions ----
  struct slot_nameResult { Text Result = ""; };
  KERN_PURE slot_nameResult slot_name(perf_Slot s = perf_Slot::logic) {
    switch (flow_switch_enum<perf_Slot>(s)) {
      case perf_Slot::logic: {
        return {"logic"};
      } break;
      case perf_Slot::physics: {
        return {"physics"};
      } break;
      case perf_Slot::stream: {
        return {"stream"};
      } break;
      case perf_Slot::draw: {
        return {"draw"};
      } break;
      case perf_Slot::count: {
        return {"?"};
      } break;
    }
  }

  void Meter_lap(perf_Meter& self, perf_Slot s = perf_Slot::logic, Float now_ms = 0.0) {
    if (math_compare<">=">(kern::to<Float>(enum_index<perf_Slot>(s)), kern::to<Float>(enum_index<perf_Slot>(perf_Slot::count)))) {
      return;
    }
    self.cur_[enum_index<perf_Slot>(s)] = kern::formula([](Float a, Float now_ms, Float last_) -> Float { return (a + (now_ms - last_)); }, self.cur_[enum_index<perf_Slot>(s)], now_ms, self.last_);
    self.last_ = now_ms;
  }

  void Meter_end_frame(perf_Meter& self, Float now_ms = 0.0, Float render_cpu_ms = 0.0, Float gpu_ms = 0.0) {
    Float a = 0.0;
    a = 0.1;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<perf_Slot>(perf_Slot::count))))); it10.next();) {
      self.avg_[n10.index] = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (a + ((a_2 - a) * a_3)); }, self.avg_[n10.index], self.cur_[n10.index], a);
      self.cur_[n10.index] = 0.0;
    }
    self.update_ = kern::formula([](Float update_, Float now_ms, Float start_, Float a) -> Float { return (update_ + (((now_ms - start_) - update_) * a)); }, self.update_, now_ms, self.start_, a);
    if (math_compare<">=">(render_cpu_ms, 0.0)) {
      self.render_ = kern::formula([](Float render_, Float render_cpu_ms, Float a) -> Float { return (render_ + ((render_cpu_ms - render_) * a)); }, self.render_, render_cpu_ms, a);
    }
    self.gpu_ = kern::formula([](Float gpu_ms, Float gpu_, Float a) -> Float { return ((gpu_ms < 0.0) ? (-1.0) : ((gpu_ < 0.0) ? gpu_ms : (gpu_ + ((gpu_ms - gpu_) * a)))); }, gpu_ms, self.gpu_, a);
  }

  void Governor_reset(perf_Governor& self, I32 tier = 0, I32 max_tier = 0) {
    self.max_ = kern::to<Int>(kern::formula([](Float max_tier, Float kTiers) -> Float { return ((max_tier < 0.0) ? 0.0 : ((max_tier >= kTiers) ? (kTiers - 1.0) : max_tier)); }, kern::to<Float>(max_tier), kern::to<Float>(kTiers)));
    self.tier_ = kern::to<Int>(kern::formula([](Float tier, Float max_) -> Float { return ((tier < 0.0) ? 0.0 : ((tier > max_) ? max_ : tier)); }, kern::to<Float>(tier), kern::to<Float>(self.max_)));
    self.auto_ = true;
    self.avg_dt_ = 0.016666666666666666;
    self.clock_ = 0.0;
    self.since_change_ = kern::formula([](Float kSettle, Float kWarmup) -> Float { return (kSettle - kWarmup); }, kSettle, kWarmup);
    self.high_ = 0.0;
    self.low_ = self.high_;
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kTiers)))); it11.next();) {
      self.block_len_[n11.index] = 0.0;
      self.block_until_[n11.index] = self.block_len_[n11.index];
    }
  }

  void Governor_set_fixed(perf_Governor& self, I32 tier = 0) {
    self.auto_ = false;
    self.tier_ = kern::to<Int>(kern::formula([](Float tier, Float kTiers) -> Float { return ((tier < 0.0) ? 0.0 : ((tier >= kTiers) ? (kTiers - 1.0) : tier)); }, kern::to<Float>(tier), kern::to<Float>(kTiers)));
  }

  struct Governor_updateResult { Bool Result = false; };
  Governor_updateResult Governor_update(perf_Governor& self, Float dt = 0.0) {
    Float fps = 0.0;
    if (logic_not(math_compare<">">(dt, 0.0))) {
      return {false};
    }
    self.clock_ = kern::formula([](Float clock_, Float dt) -> Float { return (clock_ + dt); }, self.clock_, dt);
    self.since_change_ = kern::formula([](Float since_change_, Float dt) -> Float { return (since_change_ + dt); }, self.since_change_, dt);
    self.avg_dt_ = kern::formula([](Float avg_dt_, Float dt, Float the_result) -> Float { return (avg_dt_ + ((dt - avg_dt_) * the_result)); }, self.avg_dt_, dt, common::damp_alpha(kern::formula([](Float kAverageTau) -> Float { return kern::math::div(1.0, kAverageTau); }, kAverageTau), dt).Result);
    if (logic_not(self.auto_)) {
      return {false};
    }
    if (math_compare<"<">(self.since_change_, kSettle)) {
      self.high_ = 0.0;
      self.low_ = self.high_;
      return {false};
    }
    fps = kern::formula([](Float avg_dt_) -> Float { return kern::math::div(1.0, avg_dt_); }, self.avg_dt_);
    self.low_ = kern::formula([](Float fps, Float kDownFps, Float low_, Float dt) -> Float { return ((fps < kDownFps) ? (low_ + dt) : 0.0); }, fps, kDownFps, self.low_, dt);
    self.high_ = kern::formula([](Float fps, Float kUpFps, Float high_, Float dt) -> Float { return ((fps >= kUpFps) ? (high_ + dt) : 0.0); }, fps, kUpFps, self.high_, dt);
    if (logic_and(math_compare<">=">(self.low_, kDownHold), math_compare<">">(kern::to<Float>(self.tier_), 0.0))) {
      self.block_len_[self.tier_] = kern::formula([](Float a, Float kBlockFirst, Float kBlockMax) -> Float { return ((a <= 0.0) ? kBlockFirst : kern::math::min(kBlockMax, (a * 2.0))); }, self.block_len_[self.tier_], kBlockFirst, kBlockMax);
      self.block_until_[self.tier_] = kern::formula([](Float clock_, Float a) -> Float { return (clock_ + a); }, self.clock_, self.block_len_[self.tier_]);
      self.tier_ = kern::to<Int>(kern::formula([](Float tier_) -> Float { return (tier_ - 1.0); }, kern::to<Float>(self.tier_)));
      self.since_change_ = 0.0;
      self.high_ = 0.0;
      self.low_ = self.high_;
      self.changes_ = kern::to<Int>(kern::formula([](Float changes_) -> Float { return (changes_ + 1.0); }, kern::to<Float>(self.changes_)));
      return {true};
    }
    if (logic_and(logic_and(math_compare<">=">(self.high_, kUpHold), math_compare<"<">(kern::to<Float>(self.tier_), kern::to<Float>(self.max_))), math_compare<">=">(self.clock_, self.block_until_[kern::to<Int>(kern::formula([](Float tier_) -> Float { return (tier_ + 1.0); }, kern::to<Float>(self.tier_)))]))) {
      self.tier_ = kern::to<Int>(kern::formula([](Float tier_) -> Float { return (tier_ + 1.0); }, kern::to<Float>(self.tier_)));
      self.since_change_ = 0.0;
      self.high_ = 0.0;
      self.low_ = self.high_;
      self.changes_ = kern::to<Int>(kern::formula([](Float changes_) -> Float { return (changes_ + 1.0); }, kern::to<Float>(self.changes_)));
      return {true};
    }
    return {false};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};
