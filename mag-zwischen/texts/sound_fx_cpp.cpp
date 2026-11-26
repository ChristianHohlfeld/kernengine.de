struct sound_fx_cpp : virtual kern::Library, virtual engine_audio_audio, virtual sound_fx {
  // ---- functions ----
  void SoundFx_drive(SoundFx& self, SoundFx_Loop& l, Bool on = false, Vector pos = Vector{0.0, 0.0, 0.0}, Vector vel = Vector{0.0, 0.0, 0.0}, Float volume = 0.0, Float pitch = 0.0, Bool spatial = false, Float max_distance = 0.0) {
    Bool playing = false;
    Bool playing_2 = false;
    Bool either = false;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    if (logic_or(logic_not(math_compare<"!=">(kern::to<Float>(self.mixer_), 0.0)), logic_not(math_compare<"!=">(kern::to<Float>(l.clip), 0.0)))) {
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
      engine_mixer_play(n10, self.mixer_, l.clip, p);
      play = n10.result;
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
  struct { Int64 result = Int64{0x0}; } n10;
};
