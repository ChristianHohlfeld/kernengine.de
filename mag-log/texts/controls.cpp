struct controls : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(controls_Devices) { Bool fine = true; Bool coarse = false; I32 touch_points = 0; Bool desktop = true; Bool pad = false; U32 changes = 0; };
  KERN_STRUCT(controls_GrenadeCook) { I32 carry = 3; Float regen_at = 0.0; Bool cooking = false; Float at = 0.0; Float power = 0.7; I32 source = 0; };
  KERN_STRUCT(controls_RateStick) { Float full_px_s = 0.0; Float tau = 0.0; Vector rate = Vector{0.0, 0.0, 0.0}; };

  // ---- functions ----
  void RateStick_reset(controls_RateStick& self) {
    self.rate = Vector{0.0, 0.0, 0.0};
  }

  void GrenadeCook_reset(controls_GrenadeCook& self) {
    self = struct_make<controls_GrenadeCook>(3, 0.0, false, 0.0, 0.7, 0);
  }

  void Devices_set(controls_Devices& self, Bool d = false) {
    if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(d))), kern::to<Float>(value_int(kern::to<Int>(self.desktop))))) {
      self.desktop = d;
      self.changes = kern::to<Int>(kern::formula([](Float changes) -> Float { return (changes + 1.0); }, kern::to<Float>(self.changes)));
    }
  }
};
