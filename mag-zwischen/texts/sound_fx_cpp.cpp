struct sound_fx_cpp : virtual kern::Library, virtual engine_audio_audio, virtual engine_audio_synth, virtual sound_fx {
  // ---- functions ----
  struct loop_ofResult { List<Float> Result = List<Float>{}; };
  loop_ofResult loop_of(List<audio_SynthPatch> layers = List<audio_SynthPatch>{}, U32 rate = 0, Float seconds = 0.0) {
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
    engine_audio_normalize(n13, out, 0.8);
    out = n13.samples;
    return {out};
  }

  struct SoundFx_initResult { Bool Result = false; };
  SoundFx_initResult SoundFx_init(SoundFx& self, Object<kern::audio::Mixer> mixer = Object<kern::audio::Mixer>{}) {
    Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{};
    U32 rate = 0;
    Int sample_rate = 0;
    audio_SynthPatch motor = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch harm = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip = 0;
    audio_SynthPatch wash = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_2 = 0;
    audio_SynthPatch base = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch rumble = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch intake = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_3 = 0;
    audio_SynthPatch whine = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch whine2 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_4 = 0;
    audio_SynthPatch roar = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch crackle = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_5 = 0;
    audio_SynthPatch air = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_6 = 0;
    audio_SynthPatch wail = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch grit = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_7 = 0;
    mixer_ = mixer;
    self.mixer_ = mixer_;
    sample_rate = engine_mixer_sample_rate(mixer_);
    rate = sample_rate;
    if (logic_not(math_compare<"!=">(kern::to<Float>(rate), 0.0))) {
      return {false};
    }
    motor = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    motor.wave = audio_Wave::saw;
    motor.frequency = 140.0;
    motor.lowpass = 2600.0;
    motor.resonance = 0.35;
    motor.tremolo_depth = 0.45;
    motor.tremolo_rate = 100.0;
    motor.volume = 0.6;
    harm = motor;
    harm.wave = audio_Wave::square;
    harm.frequency = 280.0;
    harm.duty = 0.3;
    harm.volume = 0.18;
    harm.lowpass = 3800.0;
    n14 = loop_of(list_make<audio_SynthPatch, "2">(motor, harm), rate, 1.0);
    engine_mixer_adopt_clip(n15, mixer_, n14.Result, 1, "spawncity:rotor", true);
    adopt_clip = n15.result;
    self.rotor_.clip = adopt_clip;
    wash = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    wash.wave = audio_Wave::pink;
    wash.lowpass = 1400.0;
    wash.highpass = 120.0;
    wash.volume = 0.5;
    wash.seed = 7;
    n16 = loop_of(list_make<audio_SynthPatch, "1">(wash), rate, 2.0);
    engine_mixer_adopt_clip(n17, mixer_, n16.Result, 1, "spawncity:propwash", true);
    adopt_clip_2 = n17.result;
    self.prop_wash_.clip = adopt_clip_2;
    base = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    base.wave = audio_Wave::pulse;
    base.duty = 0.22;
    base.frequency = 55.0;
    base.lowpass = 900.0;
    base.resonance = 0.2;
    base.volume = 0.6;
    rumble = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    rumble.wave = audio_Wave::sine;
    rumble.frequency = 27.5;
    rumble.volume = 0.45;
    intake = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    intake.wave = audio_Wave::noise;
    intake.lowpass = 600.0;
    intake.volume = 0.12;
    intake.seed = 3;
    n18 = loop_of(list_make<audio_SynthPatch, "3">(base, rumble, intake), rate, 1.0);
    engine_mixer_adopt_clip(n19, mixer_, n18.Result, 1, "spawncity:engine", true);
    adopt_clip_3 = n19.result;
    self.engine_.clip = adopt_clip_3;
    whine = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    whine.wave = audio_Wave::sine;
    whine.frequency = 1180.0;
    whine.vibrato_depth = 0.05;
    whine.vibrato_rate = 3.0;
    whine.volume = 0.35;
    whine2 = whine;
    whine2.frequency = 2240.0;
    whine2.volume = 0.15;
    n20 = loop_of(list_make<audio_SynthPatch, "2">(whine, whine2), rate, 1.0);
    engine_mixer_adopt_clip(n21, mixer_, n20.Result, 1, "spawncity:turbine", true);
    adopt_clip_4 = n21.result;
    self.turbine_.clip = adopt_clip_4;
    roar = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    roar.wave = audio_Wave::pink;
    roar.lowpass = 700.0;
    roar.volume = 0.9;
    roar.seed = 11;
    crackle = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    crackle.wave = audio_Wave::noise;
    crackle.lowpass = 2400.0;
    crackle.highpass = 900.0;
    crackle.volume = 0.12;
    crackle.seed = 13;
    n22 = loop_of(list_make<audio_SynthPatch, "2">(roar, crackle), rate, 2.0);
    engine_mixer_adopt_clip(n23, mixer_, n22.Result, 1, "spawncity:roar", true);
    adopt_clip_5 = n23.result;
    self.roar_.clip = adopt_clip_5;
    air = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    air.wave = audio_Wave::pink;
    air.lowpass = 900.0;
    air.highpass = 60.0;
    air.volume = 0.6;
    air.seed = 21;
    n24 = loop_of(list_make<audio_SynthPatch, "1">(air), rate, 3.0);
    engine_mixer_adopt_clip(n25, mixer_, n24.Result, 1, "spawncity:wind", true);
    adopt_clip_6 = n25.result;
    self.wind_.clip = adopt_clip_6;
    wail = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    wail.wave = audio_Wave::triangle;
    wail.frequency = 970.0;
    wail.vibrato_depth = 7.0;
    wail.vibrato_rate = 0.5;
    wail.lowpass = 3200.0;
    wail.volume = 0.7;
    grit = wail;
    grit.wave = audio_Wave::square;
    grit.duty = 0.5;
    grit.lowpass = 2200.0;
    grit.volume = 0.12;
    n26 = loop_of(list_make<audio_SynthPatch, "2">(wail, grit), rate, 4.0);
    engine_mixer_adopt_clip(n27, mixer_, n26.Result, 1, "spawncity:siren", true);
    adopt_clip_7 = n27.result;
    self.siren_.clip = adopt_clip_7;
    return {logic_and(logic_and(logic_and(logic_and(math_compare<"!=">(kern::to<Float>(self.rotor_.clip), 0.0), math_compare<"!=">(kern::to<Float>(self.engine_.clip), 0.0)), math_compare<"!=">(kern::to<Float>(self.turbine_.clip), 0.0)), math_compare<"!=">(kern::to<Float>(self.roar_.clip), 0.0)), math_compare<"!=">(kern::to<Float>(self.wind_.clip), 0.0))};
  }

  void SoundFx_drive(SoundFx& self, SoundFx_Loop& l, Bool on = false, Vector pos = Vector{0.0, 0.0, 0.0}, Vector vel = Vector{0.0, 0.0, 0.0}, Float volume = 0.0, Float pitch = 0.0, Bool spatial = false, Float max_distance = 0.0) {
    Bool playing = false;
    Bool playing_2 = false;
    Bool either = false;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    if (logic_or(logic_not(object_valid<kern::audio::Mixer>(self.mixer_)), logic_not(math_compare<"!=">(kern::to<Float>(l.clip), 0.0)))) {
      return;
    }
    if (logic_or(logic_not(on), math_compare<"<">(volume, 0.002))) {
      if (logic_not(int64_equal(l.voice, Int64{0x0}))) {
        engine_mixer_stop(self.mixer_, l.voice, 0.15);
        l.voice = Int64{0x0};
      }
      return;
    }
    either = logic_not(logic_not(int64_equal(l.voice, Int64{0x0})));
    if (logic_not(either)) {
      playing_2 = engine_mixer_playing(self.mixer_, l.voice);
      either = logic_not(playing_2);
    }
    if (either) {
      p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
      p.the_loop = true;
      p.volume = volume;
      p.pitch = pitch;
      p.spatial = spatial;
      p.position = pos;
      p.velocity = vel;
      p.min_distance = 3.0;
      p.max_distance = max_distance;
      p.bus = audio_Bus::sfx;
      p.fade_in = 0.1;
      engine_mixer_play(n28, self.mixer_, l.clip, p);
      play = n28.result;
      l.voice = play;
      return;
    }
    engine_mixer_set_volume(self.mixer_, l.voice, volume, 0.03);
    engine_mixer_set_pitch(self.mixer_, l.voice, kern::formula([](Float pitch) -> Float { return (pitch < 0.05 ? 0.05 : (8.0 < pitch ? 8.0 : pitch)); }, pitch));
    if (spatial) {
      engine_mixer_set_position(self.mixer_, l.voice, pos, vel);
    }
  }

  void SoundFx_update(SoundFx& self, const SoundState& s, Float dt = 0.0) {
    SoundFx_drive(self, self.rotor_, s.drone.active, s.drone.position, s.drone.velocity, kern::formula([](Float drone_mix, Float a) -> Float { return ((drone_mix * 0.5) * a); }, s.drone_mix, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float rate) -> Float { return (0.15 + rate); }, s.drone.rate), 0.0, 1.0)), kern::formula([](Float rate) -> Float { return (0.35 + (1.9 * rate)); }, s.drone.rate), true, 140.0);
    SoundFx_drive(self, self.prop_wash_, s.drone.active, s.drone.position, s.drone.velocity, kern::formula([](Float drone_mix, Float load) -> Float { return ((drone_mix * 0.35) * load); }, s.drone_mix, s.drone.load), kern::formula([](Float rate) -> Float { return (0.7 + (0.6 * rate)); }, s.drone.rate), true, 90.0);
    SoundFx_drive(self, self.engine_, s.car.active, s.car.position, s.car.velocity, kern::formula([](Float vehicle_mix, Float load) -> Float { return ((vehicle_mix * 0.58) * (0.35 + (0.65 * load))); }, s.vehicle_mix, s.car.load), kern::formula([](Float rate) -> Float { return (0.75 + (2.2 * rate)); }, s.car.rate), true, 120.0);
    SoundFx_drive(self, self.turbine_, s.jet.active, s.jet.position, s.jet.velocity, kern::formula([](Float vehicle_mix, Float a) -> Float { return ((vehicle_mix * 0.45) * a); }, s.vehicle_mix, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float rate) -> Float { return (0.2 + rate); }, s.jet.rate), 0.0, 1.0)), kern::formula([](Float rate) -> Float { return (0.55 + (0.8 * rate)); }, s.jet.rate), true, 900.0);
    SoundFx_drive(self, self.roar_, s.jet.active, s.jet.position, s.jet.velocity, kern::formula([](Float vehicle_mix, Float load) -> Float { return ((vehicle_mix * 0.9) * load); }, s.vehicle_mix, s.jet.load), kern::formula([](Float load) -> Float { return (0.7 + (0.5 * load)); }, s.jet.load), true, 1500.0);
    SoundFx_drive(self, self.siren_, s.siren.active, s.siren.position, s.siren.velocity, 0.42, 1.0, true, 260.0);
    SoundFx_drive(self, self.wind_, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float wind) -> Float { return kern::math::div((wind - 6.0), 60.0); }, s.wind), 0.0, 0.7)), kern::formula([](Float wind) -> Float { return (0.8 + kern::math::min(0.6, kern::math::div(wind, 120.0))); }, s.wind), false, 0.0);
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct { List<Float> dst = List<Float>{}; } n11;
  struct { List<Float> samples = List<Float>{}; } n12;
  struct { List<Float> samples = List<Float>{}; } n13;
  loop_ofResult n14;
  struct { Int result = 0; } n15;
  loop_ofResult n16;
  struct { Int result = 0; } n17;
  loop_ofResult n18;
  struct { Int result = 0; } n19;
  loop_ofResult n20;
  struct { Int result = 0; } n21;
  loop_ofResult n22;
  struct { Int result = 0; } n23;
  loop_ofResult n24;
  struct { Int result = 0; } n25;
  loop_ofResult n26;
  struct { Int result = 0; } n27;
  struct { Int64 result = Int64{0x0}; } n28;
};
