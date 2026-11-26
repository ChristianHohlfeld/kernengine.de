struct game_equipment_cpp : virtual kern::Library, virtual engine_audio_audio {
  // ---- functions ----
  void drive_loop(Int64& v, Int m = 0, U32 clip = 0, Float volume = 0.0, audio_Bus bus = audio_Bus::master, Bool spatial = false, Vector at = Vector{0.0, 0.0, 0.0}) {
    Int m_ = 0;
    Bool playing = false;
    Bool playing_2 = false;
    Bool either = false;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    m_ = m;
    if (logic_not(math_compare<"!=">(kern::to<Float>(clip), 0.0))) {
      return;
    }
    if (math_compare<"<">(volume, 0.002)) {
      if (logic_not(int64_equal(v, Int64{0x0}))) {
        engine_mixer_stop(m_, v, 0.3);
        v = Int64{0x0};
      }
      return;
    }
    either = logic_not(logic_not(int64_equal(v, Int64{0x0})));
    if (logic_not(either)) {
      playing_2 = engine_mixer_playing(m_, v);
      either = logic_not(playing_2);
    }
    if (either) {
      p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
      p.the_loop = true;
      p.volume = volume;
      p.bus = bus;
      p.fade_in = 0.4;
      p.spatial = spatial;
      p.position = at;
      p.min_distance = 2.0;
      p.max_distance = 80.0;
      engine_mixer_play(n10, m_, clip, p);
      play = n10.result;
      v = play;
      return;
    }
    engine_mixer_set_volume(m_, v, volume, 0.2);
    if (spatial) {
      engine_mixer_set_position(m_, v, at, Vector{0.0, 0.0, 0.0});
    }
  }

  // ---- what nodes give and keep ----
  struct { Int64 result = Int64{0x0}; } n10;
};
