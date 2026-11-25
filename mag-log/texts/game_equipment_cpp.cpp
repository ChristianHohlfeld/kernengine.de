struct game_equipment_cpp : virtual kern::Library, virtual engine_audio_audio, virtual engine_audio_synth {
  // ---- functions ----
  struct render_loopResult { List<Float> Result = List<Float>{}; };
  render_loopResult render_loop(List<audio_SynthPatch> layers = List<audio_SynthPatch>{}, U32 rate = 0, Float seconds = 0.0, Float peak = 0.0) {
    List<Float> out = List<Float>{};
    List<Float> made = List<Float>{};
    audio_SynthPatch p = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    List<Float> synthesize = List<Float>{};
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float seconds, Float rate) -> Float { return kern::math::trunc((seconds * rate)); }, seconds, kern::to<Float>(rate))), 0.0);
    out = made;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(layers))))); it10.next();) {
      p = layers[n10.index];
      p.attack = 0.0;
      p.sustain = kern::formula([](Float seconds) -> Float { return (seconds + 0.25); }, seconds);
      p.release = 0.0;
      synthesize = engine_audio_synthesize(p, rate);
      engine_audio_mix_into(n11, out, synthesize, 0.0, rate, 1.0);
      out = n11.dst;
    }
    list_resize(out, kern::to<Int>(kern::formula([](Float seconds, Float rate) -> Float { return kern::math::trunc((seconds * rate)); }, seconds, kern::to<Float>(rate))), 0.0);
    engine_audio_make_seamless(n12, out, rate, 0.12);
    out = n12.samples;
    engine_audio_normalize(n13, out, peak);
    out = n13.samples;
    return {out};
  }

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
      engine_mixer_play(n14, m_, clip, p);
      play = n14.result;
      v = play;
      return;
    }
    engine_mixer_set_volume(m_, v, volume, 0.2);
    if (spatial) {
      engine_mixer_set_position(m_, v, at, Vector{0.0, 0.0, 0.0});
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct { List<Float> dst = List<Float>{}; } n11;
  struct { List<Float> samples = List<Float>{}; } n12;
  struct { List<Float> samples = List<Float>{}; } n13;
  struct { Int64 result = Int64{0x0}; } n14;
};
