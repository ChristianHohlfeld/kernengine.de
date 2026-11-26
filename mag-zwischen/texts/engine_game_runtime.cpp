struct engine_game_runtime : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(game_PlayInput) { Float move_x = 0.0; Float move_y = 0.0; Float up = 0.0; Bool jump = false; Bool boost = false; Bool camera = false; Bool handbrake = false; U32 robot = 0; Float look_x = 0.0; Float look_y = 0.0; Float turn_deg = 0.0; Float speed_scale = 1.0; Float zoom = 0.0; Float view_yaw = 0.0; Bool has_view_yaw = false; Float view_pitch = 0.0; Bool fire = false; Bool aim = false; Bool reload = false; Bool crouch = false; Bool interact = false; Float look_stick_x = 0.0; Float look_stick_y = 0.0; Bool gamepad = false; Float rewind = 0.0; };
};
