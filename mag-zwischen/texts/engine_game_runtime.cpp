struct engine_game_runtime : virtual kern::Library, virtual engine_audio_audio {
  // ---- types ----
  KERN_STRUCT(game_ShotEvent) { Vector from = Vector{0.0, 0.0, 0.0}; Vector to = Vector{0.0, 0.0, 0.0}; U32 shooter = 0; U32 hit = 0; Bool impact = false; Bool head = false; Vector normal = Vector{0.0, 0.0, 1.0}; U8 surface = 0; };
  KERN_STRUCT(game_SoldierState) { Float health = 100.0; Float max_health = 100.0; Float armor = 0.0; I32 ammo = 0; I32 magazine = 0; I32 reserve = 0; Float reload = -1.0; Float spread = 0.0; Float respawn_in = -1.0; U8 hits = 0; U8 damages = 0; Bool hit_head = false; Bool hit_kill = false; Bool dead = false; Float damage_yaw = 0.0; };
  KERN_STRUCT(game_SoundCue) { Text sound = ""; Vector position = Vector{0.0, 0.0, 0.0}; Float volume = 1.0; Float pitch = 1.0; Bool spatial = true; audio_Bus bus = audio_Bus::sfx; U32 actor = 0; Bool voice = false; Bool silence = false; };
};
