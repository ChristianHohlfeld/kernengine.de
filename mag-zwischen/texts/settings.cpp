struct settings : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(Opt, mouse_sensitivity, mouse_invert_y, drone_left_fineness, drone_right_fineness, drone_invert_yaw, drone_invert_roll, drone_invert_pitch, drone_max_speed_kmh, drone_hover_agl, foot_move_fineness, foot_look_fineness, foot_look_h, foot_look_v, foot_deadzone, foot_invert_move_h, foot_invert_look_h, foot_invert_look_v, car_steering, car_throttle, car_max_kmh, car_reverse_kmh, car_cam_distance, car_cam_height, cam_fpv_tilt, cam_fov, cam_third_distance, cam_stereo, audio_on, audio_drone, audio_shots, audio_fx, audio_steps, audio_vehicle, audio_ambient, music_on, music_volume, world_people, world_traffic, world_dogs, world_cats, world_birds, world_zombies, world_police, world_military, world_flak, world_jetpack, world_friendly_fire, world_terrain, world_guided, world_always_day, foot_aim_assist, pad_aim_scheme, view_grid, view_keep_look, view_minimap_north, debug_colliders, vr_arms, vr_level, vr_tunnel, vr_smooth_turn, count);
  KERN_STRUCT(OptSpec) { Text key = ""; Text section = ""; Text label = ""; Text help = ""; f32 the_min = 0.0f; f32 the_max = 0.0f; f32 step = 0.0f; f32 def = 0.0f; bool toggle = false; bool live = false; };
  KERN_STRUCT(Option) { Text key = ""; Text section = ""; Text label = ""; Text help = ""; f64 the_min = 0.0; f64 the_max = 0.0; f64 step = 0.0; f64 def = 0.0; bool toggle = false; bool live = false; };
  KERN_STRUCT(Settings) { Fixed<f32, 60> v_ = List<f32>{}; u32 version_ = 1; };
  KERN_STRUCT(SettingsTable) { List<Option> rows = List<Option>{}; List<OptSpec> specs = List<OptSpec>{}; };

  // ---- functions ----
  struct Settings_getResult { f32 Result = 0.0f; };
  KERN_PURE Settings_getResult Settings_get(const Settings& self, Opt o = Opt::mouse_sensitivity) {
    return {self.v_[num_cast<i64, u32>(num_cast<u32, i64>(enum_index<Opt>(o)))]};
  }

  struct Settings_onResult { bool Result = false; };
  KERN_PURE Settings_onResult Settings_on(const Settings& self, Opt o = Opt::mouse_sensitivity) {
    return {math_compare<">", f32>(Settings_get(self, o).Result, 0.5f)};
  }

  struct Settings_versionResult { u32 Result = 0; };
  /// Increases on every change (the game re-applies what depends on the settings).
  KERN_PURE Settings_versionResult Settings_version(const Settings& self) {
    return {self.version_};
  }
};
