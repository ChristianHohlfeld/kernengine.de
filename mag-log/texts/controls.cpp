struct controls : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(controls_Devices) { bool fine = true; bool coarse = false; i32 touch_points = 0; bool desktop = true; bool pad = false; u32 changes = 0; };
  KERN_STRUCT(controls_GrenadeCook) { i32 carry = 3; f32 regen_at = 0.0f; bool cooking = false; f32 at = 0.0f; f32 power = 0.7f; i32 source = 0; };
  KERN_STRUCT(controls_RateStick) { f32 full_px_s = 0.0f; f32 tau = 0.0f; Vector rate = Vector{0.0, 0.0, 0.0}; };

  // ---- functions ----
  void RateStick_reset(controls_RateStick& self) {
    self.rate = Vector{0.0, 0.0, 0.0};
  }

  void GrenadeCook_reset(controls_GrenadeCook& self) {
    self = struct_make<controls_GrenadeCook>(3, 0.0f, false, 0.0f, 0.7f, 0);
  }

  void Devices_set(controls_Devices& self, bool d = false) {
    if (math_compare<"!=", i32>(num_cast<i32, bool>(d), num_cast<i32, bool>(self.desktop))) {
      self.desktop = d;
      self.changes = kern::formula<u32>([](auto changes) { return (changes + 1); }, self.changes);
    }
  }
};
