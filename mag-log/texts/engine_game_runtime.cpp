struct engine_game_runtime : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(game_SimMode, none, simulate, play, replica);
  KERN_STRUCT(game_SoldierState) { Float health = 100.0; Float max_health = 100.0; Float armor = 0.0; Int ammo = 0; Int magazine = 0; Int reserve = 0; Float reload = -1.0; Float spread = 0.0; Float respawn_in = -1.0; Int hits = 0; Int damages = 0; Bool hit_head = false; Bool hit_kill = false; Bool dead = false; Float damage_yaw = 0.0; };
};
