struct engine_game_runtime : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(game_SimMode, none, simulate, play, replica);
};
