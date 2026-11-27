struct controls : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(controls_AimAssist) { bool active = false; f32 slowdown = 1.0f; f32 yaw = 0.0f; f32 pitch = 0.0f; };
  KERN_STRUCT(controls_CarHeadSpring) { f32 yaw = 0.0f; f32 pitch = 0.0f; };
  KERN_STRUCT(controls_Devices) { bool fine = true; bool coarse = false; i32 touch_points = 0; bool desktop = true; bool pad = false; u32 changes = 0; };
  KERN_ENUM(controls_Devices_Event, touch_down, mouse_down, mouse_move, key, pad_button);
  KERN_STRUCT(controls_DroneAxes) { f32 roll = 0.0f; f32 pitch = 0.0f; f32 yaw = 0.0f; f32 body_pitch = 0.0f; };
  KERN_STRUCT(controls_GrenadeCook) { i32 carry = 3; f32 regen_at = 0.0f; bool cooking = false; f32 at = 0.0f; f32 power = 0.7f; i32 source = 0; };
  KERN_STRUCT(controls_LookAxis) { f32 velocity = 0.0f; };
  KERN_STRUCT(controls_RateStick) { f32 full_px_s = 0.0f; f32 tau = 0.0f; Vector rate = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(controls_ReleaseLatch) { bool armed = false; };
  KERN_STRUCT(controls_ViewButton) { bool was = false; bool held_map = false; f32 down_at = 0.0f; f32 map_at = -99.0f; };
  KERN_STRUCT(controls_VoiceChord) { bool r3 = false; bool y = false; bool latched = false; };
  KERN_STRUCT(controls_VoiceChord_Out) { bool mic = false; bool mute = false; bool suppress_weapon = false; };
  KERN_STRUCT(controls_WorldLook) { f32 yaw_deg = 0.0f; f32 pitch_deg = 0.0f; bool snapping = false; f32 idle = 0.0f; bool owned = false; };

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
