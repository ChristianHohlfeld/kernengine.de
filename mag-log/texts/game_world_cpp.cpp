struct game_world_cpp : virtual kern::Library, virtual engine_audio_audio, virtual engine_audio_synth, virtual game_world {
  // ---- functions ----
  struct one_shotResult { List<Float> Result = List<Float>{}; };
  one_shotResult one_shot(List<audio_SynthPatch> layers = List<audio_SynthPatch>{}, U32 rate = 0) {
    List<Float> out = List<Float>{};
    List<Float> synthesize = List<Float>{};
    out = List<Float>{};
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(layers))))); it10.next();) {
      synthesize = engine_audio_synthesize(layers[n10.index], rate);
      engine_audio_mix_into(n11, out, synthesize, 0.0, rate, 1.0);
      out = n11.dst;
    }
    engine_audio_normalize(n12, out, 0.8);
    out = n12.samples;
    return {out};
  }

  void init_world_sounds(WorldSounds& s, Int mixer = 0) {
    Int mixer_ = 0;
    U32 rate = 0;
    Int sample_rate = 0;
    audio_SynthPatch n = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch plop = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip = 0;
    Int adopt_clip_2 = 0;
    audio_SynthPatch h = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch h2 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_3 = 0;
    mixer_ = mixer;
    sample_rate = engine_mixer_sample_rate(mixer_);
    rate = sample_rate;
    if (logic_not(math_compare<"!=">(kern::to<Float>(rate), 0.0))) {
      return;
    }
    n = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    n.wave = audio_Wave::noise;
    n.attack = 0.004;
    n.sustain = 0.12;
    n.release = 0.5;
    n.lowpass = 5200.0;
    n.lowpass_sweep = -2.4;
    n.highpass = 180.0;
    n.volume = 0.7;
    n.seed = 11;
    plop = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    plop.wave = audio_Wave::sine;
    plop.frequency = 180.0;
    plop.slide = -2.5;
    plop.sustain = 0.06;
    plop.release = 0.15;
    plop.volume = 0.5;
    n13 = one_shot(list_make<audio_SynthPatch, "2">(n, plop), rate);
    engine_mixer_adopt_clip(n14, mixer_, n13.Result, 1, "spawncity:splash", false);
    adopt_clip = n14.result;
    s.splash = adopt_clip;
    n.sustain = 0.03;
    n.release = 0.18;
    n.lowpass = 7000.0;
    n15 = one_shot(list_make<audio_SynthPatch, "1">(n), rate);
    engine_mixer_adopt_clip(n16, mixer_, n15.Result, 1, "spawncity:splash_small", false);
    adopt_clip_2 = n16.result;
    s.splash_small = adopt_clip_2;
    h = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    h.wave = audio_Wave::square;
    h.frequency = 392.0;
    h.sustain = 0.45;
    h.release = 0.05;
    h.lowpass = 2400.0;
    h.volume = 0.4;
    h2 = h;
    h2.frequency = 494.0;
    n17 = one_shot(list_make<audio_SynthPatch, "2">(h, h2), rate);
    engine_mixer_adopt_clip(n18, mixer_, n17.Result, 1, "spawncity:horn", false);
    adopt_clip_3 = n18.result;
    s.horn = adopt_clip_3;
    s.ready = true;
  }

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
    engine_mixer_play(n19, mixer_, clip, p);
    play = n19.result;
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct { List<Float> dst = List<Float>{}; } n11;
  struct { List<Float> samples = List<Float>{}; } n12;
  one_shotResult n13;
  struct { Int result = 0; } n14;
  one_shotResult n15;
  struct { Int result = 0; } n16;
  one_shotResult n17;
  struct { Int result = 0; } n18;
  struct { Int64 result = Int64{0x0}; } n19;
};
