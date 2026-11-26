struct combat_detail : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(detail_Critical, car, bus, police_drone, drone);
  KERN_STRUCT(detail_CriticalClock) { Float at = -1.0; Float last_hp = -1.0; };
  KERN_ENUM(detail_Round, nine_mm, five_five_six, fifty_bmg);

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
