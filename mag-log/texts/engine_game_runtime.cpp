struct engine_game_runtime : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(game_ShotEvent) { Vector from = Vector{0.0, 0.0, 0.0}; Vector to = Vector{0.0, 0.0, 0.0}; Int shooter = 0; Int hit = 0; Bool impact = false; Bool head = false; Vector normal = Vector{0.0, 0.0, 1.0}; Int surface = 0; };
  KERN_ENUM(game_SimMode, none, simulate, play, replica);
};
