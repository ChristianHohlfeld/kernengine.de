struct engine_game_level : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(game_Transform) { Vector position = Vector{0.0, 0.0, 0.0}; Vector rotation = Vector{0.0, 0.0, 0.0}; Vector scale = Vector{1.0, 1.0, 1.0}; };
};
