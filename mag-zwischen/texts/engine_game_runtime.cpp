struct engine_game_runtime : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(game_PlayInput) { f32 move_x = 0.0f; f32 move_y = 0.0f; f32 up = 0.0f; bool jump = false; bool boost = false; bool camera = false; bool handbrake = false; u32 robot = 0; f32 look_x = 0.0f; f32 look_y = 0.0f; f32 turn_deg = 0.0f; f32 speed_scale = 1.0f; f32 zoom = 0.0f; f32 view_yaw = 0.0f; bool has_view_yaw = false; f32 view_pitch = 0.0f; bool fire = false; bool aim = false; bool reload = false; bool crouch = false; bool interact = false; f32 look_stick_x = 0.0f; f32 look_stick_y = 0.0f; bool gamepad = false; f32 rewind = 0.0f; };
  KERN_ENUM(game_SimMode, none, simulate, play, replica);
};
