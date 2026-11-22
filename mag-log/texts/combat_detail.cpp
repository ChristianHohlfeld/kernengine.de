struct combat_detail : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(detail_CriticalClock) { Float at = -1.0; Float last_hp = -1.0; };

  // ---- functions ----
  struct CriticalClock_armedResult { Bool Result = false; };
  KERN_PURE CriticalClock_armedResult CriticalClock_armed(const detail_CriticalClock& self) {
    return {math_compare<">=">(self.at, 0.0)};
  }

  void CriticalClock_reset(detail_CriticalClock& self) {
    self.at = -1.0;
    self.last_hp = -1.0;
  }
};
