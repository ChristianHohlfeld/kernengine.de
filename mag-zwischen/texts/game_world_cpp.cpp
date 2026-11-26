struct game_world_cpp : virtual kern::Library, virtual engine_audio_audio {
  // ---- functions ----
  void play_at(Int mixer = 0, U32 clip = 0, Vector at = Vector{0.0, 0.0, 0.0}, Float volume = 0.0, Float pitch = 0.0, Float max_distance = 0.0) {
    Int mixer_ = 0;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    mixer_ = mixer;
    if (logic_not(math_compare<"!=">(kern::to<Float>(clip), 0.0))) {
      return;
    }
    p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    p.volume = volume;
    p.pitch = pitch;
    p.spatial = true;
    p.position = at;
    p.min_distance = 2.0;
    p.max_distance = max_distance;
    engine_mixer_play(n10, mixer_, clip, p);
    play = n10.result;
  }

  // ---- what nodes give and keep ----
  struct { Int64 result = Int64{0x0}; } n10;
};
