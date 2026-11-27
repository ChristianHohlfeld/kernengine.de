struct sound_fx_cpp : virtual kern::Library, virtual common, virtual engine_audio_audio, virtual engine_audio_synth, virtual sound_fx {
  // ---- functions ----
  struct loop_ofResult { List<f32> Result = List<f32>{}; };
  loop_ofResult loop_of(List<audio_SynthPatch> layers = List<audio_SynthPatch>{}, u32 rate = 0, f32 seconds = 0.0f) {
    List<f32> out = List<f32>{};
    List<f32> made = List<f32>{};
    audio_SynthPatch p = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    List<f32> synthesize = List<f32>{};
    made = List<f32>{};
    list_resize(made, num_cast<i64, u64>(kern::formula<f32, u32>([](auto seconds, auto rate) { return num_cast<u64, f32>((seconds * num_cast<f32, u32>(rate))); }, seconds, rate)), 0.0f);
    out = made;
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(list_length<audio_SynthPatch>(layers), 1)); it10.next();) {
      p = layers[n10.index];
      p.attack = 0.0f;
      p.sustain = kern::formula<f32>([](auto seconds) { return (seconds + 0.25f); }, seconds);
      p.release = 0.0f;
      synthesize = engine_audio_synthesize(p, rate);
      engine_audio_mix_into(n11, out, synthesize, 0.0f, rate, 1.0f);
      out = n11.dst;
    }
    list_resize(out, num_cast<i64, u64>(kern::formula<f32, u32>([](auto seconds, auto rate) { return num_cast<u64, f32>((seconds * num_cast<f32, u32>(rate))); }, seconds, rate)), 0.0f);
    engine_audio_make_seamless(n12, out, rate, 0.12f);
    out = n12.samples;
    engine_audio_normalize(n13, out, 0.8f);
    out = n13.samples;
    return {out};
  }

  struct SoundFx_initResult { bool Result = false; };
  SoundFx_initResult SoundFx_init(SoundFx& self, Object<kern::audio::Mixer> mixer = Object<kern::audio::Mixer>{}) {
    Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{};
    u32 rate = 0;
    u32 sample_rate = 0;
    audio_SynthPatch motor = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    audio_SynthPatch harm = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    u32 adopt_clip = 0;
    audio_SynthPatch wash = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    u32 adopt_clip_2 = 0;
    audio_SynthPatch base = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    audio_SynthPatch rumble = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    audio_SynthPatch intake = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    u32 adopt_clip_3 = 0;
    audio_SynthPatch whine = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    audio_SynthPatch whine2 = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    u32 adopt_clip_4 = 0;
    audio_SynthPatch roar = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    audio_SynthPatch crackle = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    u32 adopt_clip_5 = 0;
    audio_SynthPatch air = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    u32 adopt_clip_6 = 0;
    audio_SynthPatch wail = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    audio_SynthPatch grit = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    u32 adopt_clip_7 = 0;
    mixer_ = mixer;
    self.mixer_ = mixer_;
    sample_rate = engine_mixer_sample_rate(mixer_);
    rate = sample_rate;
    if (logic_not(num_cast<bool, u32>(rate))) {
      return {false};
    }
    motor = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    motor.wave = audio_Wave::saw;
    motor.frequency = 140.0f;
    motor.lowpass = 2600.0f;
    motor.resonance = 0.35f;
    motor.tremolo_depth = 0.45f;
    motor.tremolo_rate = 100.0f;
    motor.volume = 0.6f;
    harm = motor;
    harm.wave = audio_Wave::square;
    harm.frequency = 280.0f;
    harm.duty = 0.3f;
    harm.volume = 0.18f;
    harm.lowpass = 3800.0f;
    n14 = loop_of(list_make<audio_SynthPatch, "2">(motor, harm), rate, 1.0f);
    engine_mixer_adopt_clip(n15, mixer_, n14.Result, 1, "spawncity:rotor", true);
    adopt_clip = n15.result;
    self.rotor_.clip = adopt_clip;
    wash = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    wash.wave = audio_Wave::pink;
    wash.lowpass = 1400.0f;
    wash.highpass = 120.0f;
    wash.volume = 0.5f;
    wash.seed = 7;
    n16 = loop_of(list_make<audio_SynthPatch, "1">(wash), rate, 2.0f);
    engine_mixer_adopt_clip(n17, mixer_, n16.Result, 1, "spawncity:propwash", true);
    adopt_clip_2 = n17.result;
    self.prop_wash_.clip = adopt_clip_2;
    base = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    base.wave = audio_Wave::pulse;
    base.duty = 0.22f;
    base.frequency = 55.0f;
    base.lowpass = 900.0f;
    base.resonance = 0.2f;
    base.volume = 0.6f;
    rumble = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    rumble.wave = audio_Wave::sine;
    rumble.frequency = 27.5f;
    rumble.volume = 0.45f;
    intake = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    intake.wave = audio_Wave::noise;
    intake.lowpass = 600.0f;
    intake.volume = 0.12f;
    intake.seed = 3;
    n18 = loop_of(list_make<audio_SynthPatch, "3">(base, rumble, intake), rate, 1.0f);
    engine_mixer_adopt_clip(n19, mixer_, n18.Result, 1, "spawncity:engine", true);
    adopt_clip_3 = n19.result;
    self.engine_.clip = adopt_clip_3;
    whine = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    whine.wave = audio_Wave::sine;
    whine.frequency = 1180.0f;
    whine.vibrato_depth = 0.05f;
    whine.vibrato_rate = 3.0f;
    whine.volume = 0.35f;
    whine2 = whine;
    whine2.frequency = 2240.0f;
    whine2.volume = 0.15f;
    n20 = loop_of(list_make<audio_SynthPatch, "2">(whine, whine2), rate, 1.0f);
    engine_mixer_adopt_clip(n21, mixer_, n20.Result, 1, "spawncity:turbine", true);
    adopt_clip_4 = n21.result;
    self.turbine_.clip = adopt_clip_4;
    roar = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    roar.wave = audio_Wave::pink;
    roar.lowpass = 700.0f;
    roar.volume = 0.9f;
    roar.seed = 11;
    crackle = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    crackle.wave = audio_Wave::noise;
    crackle.lowpass = 2400.0f;
    crackle.highpass = 900.0f;
    crackle.volume = 0.12f;
    crackle.seed = 13;
    n22 = loop_of(list_make<audio_SynthPatch, "2">(roar, crackle), rate, 2.0f);
    engine_mixer_adopt_clip(n23, mixer_, n22.Result, 1, "spawncity:roar", true);
    adopt_clip_5 = n23.result;
    self.roar_.clip = adopt_clip_5;
    air = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    air.wave = audio_Wave::pink;
    air.lowpass = 900.0f;
    air.highpass = 60.0f;
    air.volume = 0.6f;
    air.seed = 21;
    n24 = loop_of(list_make<audio_SynthPatch, "1">(air), rate, 3.0f);
    engine_mixer_adopt_clip(n25, mixer_, n24.Result, 1, "spawncity:wind", true);
    adopt_clip_6 = n25.result;
    self.wind_.clip = adopt_clip_6;
    wail = audio_SynthPatch{audio_Wave::square, 0.5f, 0.005f, 0.1f, 0.0f, 0.2f, 440.0f, 0.0f, 0.0f, 20.0f, 0.0f, 6.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1};
    wail.wave = audio_Wave::triangle;
    wail.frequency = 970.0f;
    wail.vibrato_depth = 7.0f;
    wail.vibrato_rate = 0.5f;
    wail.lowpass = 3200.0f;
    wail.volume = 0.7f;
    grit = wail;
    grit.wave = audio_Wave::square;
    grit.duty = 0.5f;
    grit.lowpass = 2200.0f;
    grit.volume = 0.12f;
    n26 = loop_of(list_make<audio_SynthPatch, "2">(wail, grit), rate, 4.0f);
    engine_mixer_adopt_clip(n27, mixer_, n26.Result, 1, "spawncity:siren", true);
    adopt_clip_7 = n27.result;
    self.siren_.clip = adopt_clip_7;
    return {logic_and(logic_and(logic_and(logic_and(num_cast<bool, u32>(self.rotor_.clip), num_cast<bool, u32>(self.engine_.clip)), num_cast<bool, u32>(self.turbine_.clip)), num_cast<bool, u32>(self.roar_.clip)), num_cast<bool, u32>(self.wind_.clip))};
  }

  void SoundFx_drive(SoundFx& self, SoundFx_Loop& l, bool on = false, Vector pos = Vector{0.0, 0.0, 0.0}, Vector vel = Vector{0.0, 0.0, 0.0}, f32 volume = 0.0f, f32 pitch = 0.0f, bool spatial = false, f32 max_distance = 0.0f) {
    bool playing = false;
    bool playing_2 = false;
    bool either = false;
    audio_PlayParams p = audio_PlayParams{1.0f, 1.0f, 0.0f, false, audio_Bus::sfx, 0.0f, 0.0f, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0f, 50.0f, audio_Rolloff::inverse, 0.0f, 1.0f};
    u64 play = 0;
    if (logic_or(logic_not(object_valid<kern::audio::Mixer>(self.mixer_)), logic_not(num_cast<bool, u32>(l.clip)))) {
      return;
    }
    if (logic_or(logic_not(on), math_compare<"<", f32>(volume, 0.002f))) {
      if (num_cast<bool, u64>(l.voice)) {
        engine_mixer_stop(self.mixer_, l.voice, 0.15f);
        l.voice = 0;
      }
      return;
    }
    either = logic_not(num_cast<bool, u64>(l.voice));
    if (logic_not(either)) {
      playing_2 = engine_mixer_playing(self.mixer_, l.voice);
      either = logic_not(playing_2);
    }
    if (either) {
      p = audio_PlayParams{1.0f, 1.0f, 0.0f, false, audio_Bus::sfx, 0.0f, 0.0f, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0f, 50.0f, audio_Rolloff::inverse, 0.0f, 1.0f};
      p.the_loop = true;
      p.volume = volume;
      p.pitch = pitch;
      p.spatial = spatial;
      p.position = pos;
      p.velocity = vel;
      p.min_distance = 3.0f;
      p.max_distance = max_distance;
      p.bus = audio_Bus::sfx;
      p.fade_in = 0.1f;
      engine_mixer_play(n28, self.mixer_, l.clip, p);
      play = n28.result;
      l.voice = play;
      return;
    }
    engine_mixer_set_volume(self.mixer_, l.voice, volume, 0.03f);
    engine_mixer_set_pitch(self.mixer_, l.voice, common::clampf(pitch, 0.05f, 8.0f).Result);
    if (spatial) {
      engine_mixer_set_position(self.mixer_, l.voice, pos, vel);
    }
  }

  void SoundFx_update(SoundFx& self, const SoundState& s, f32 dt = 0.0f) {
    SoundFx_drive(self, self.rotor_, s.drone.active, s.drone.position, s.drone.velocity, kern::formula<f32, f32>([](auto drone_mix, auto the_result) { return ((drone_mix * 0.5f) * the_result); }, s.drone_mix, common::clampf(kern::formula<f32>([](auto rate) { return (0.15f + rate); }, s.drone.rate), 0.0f, 1.0f).Result), kern::formula<f32>([](auto rate) { return (0.35f + (1.9f * rate)); }, s.drone.rate), true, 140.0f);
    SoundFx_drive(self, self.prop_wash_, s.drone.active, s.drone.position, s.drone.velocity, kern::formula<f32, f32>([](auto drone_mix, auto load) { return ((drone_mix * 0.35f) * load); }, s.drone_mix, s.drone.load), kern::formula<f32>([](auto rate) { return (0.7f + (0.6f * rate)); }, s.drone.rate), true, 90.0f);
    SoundFx_drive(self, self.engine_, s.car.active, s.car.position, s.car.velocity, kern::formula<f32, f32>([](auto vehicle_mix, auto load) { return ((vehicle_mix * 0.58f) * (0.35f + (0.65f * load))); }, s.vehicle_mix, s.car.load), kern::formula<f32>([](auto rate) { return (0.75f + (2.2f * rate)); }, s.car.rate), true, 120.0f);
    SoundFx_drive(self, self.turbine_, s.jet.active, s.jet.position, s.jet.velocity, kern::formula<f32, f32>([](auto vehicle_mix, auto the_result) { return ((vehicle_mix * 0.45f) * the_result); }, s.vehicle_mix, common::clampf(kern::formula<f32>([](auto rate) { return (0.2f + rate); }, s.jet.rate), 0.0f, 1.0f).Result), kern::formula<f32>([](auto rate) { return (0.55f + (0.8f * rate)); }, s.jet.rate), true, 900.0f);
    SoundFx_drive(self, self.roar_, s.jet.active, s.jet.position, s.jet.velocity, kern::formula<f32, f32>([](auto vehicle_mix, auto load) { return ((vehicle_mix * 0.9f) * load); }, s.vehicle_mix, s.jet.load), kern::formula<f32>([](auto load) { return (0.7f + (0.5f * load)); }, s.jet.load), true, 1500.0f);
    SoundFx_drive(self, self.siren_, s.siren.active, s.siren.position, s.siren.velocity, 0.42f, 1.0f, true, 260.0f);
    SoundFx_drive(self, self.wind_, true, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, common::clampf(kern::formula<f32>([](auto wind) { return ((wind - 6.0f) / 60.0f); }, s.wind), 0.0f, 0.7f).Result, kern::formula<f32>([](auto wind) { return (0.8f + fmin(0.6f, (wind / 120.0f))); }, s.wind), false, 0.0f);
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct { List<f32> dst = List<f32>{}; } n11;
  struct { List<f32> samples = List<f32>{}; } n12;
  struct { List<f32> samples = List<f32>{}; } n13;
  loop_ofResult n14;
  struct { u32 result = 0; } n15;
  loop_ofResult n16;
  struct { u32 result = 0; } n17;
  loop_ofResult n18;
  struct { u32 result = 0; } n19;
  loop_ofResult n20;
  struct { u32 result = 0; } n21;
  loop_ofResult n22;
  struct { u32 result = 0; } n23;
  loop_ofResult n24;
  struct { u32 result = 0; } n25;
  loop_ofResult n26;
  struct { u32 result = 0; } n27;
  struct { u64 result = 0; } n28;
};
