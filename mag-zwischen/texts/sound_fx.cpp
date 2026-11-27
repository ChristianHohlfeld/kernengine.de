struct sound_fx : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(EngineSound) { bool active = false; Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; f32 rate = 0.0f; f32 load = 0.0f; };
  KERN_STRUCT(SoundFx_Loop) { u32 clip = 0; u64 voice = 0; f32 volume = 0.0f; };
  KERN_STRUCT(SoundFx) { Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{}; SoundFx_Loop rotor_ = SoundFx_Loop{0, 0, 0.0f}; SoundFx_Loop prop_wash_ = SoundFx_Loop{0, 0, 0.0f}; SoundFx_Loop engine_ = SoundFx_Loop{0, 0, 0.0f}; SoundFx_Loop turbine_ = SoundFx_Loop{0, 0, 0.0f}; SoundFx_Loop roar_ = SoundFx_Loop{0, 0, 0.0f}; SoundFx_Loop wind_ = SoundFx_Loop{0, 0, 0.0f}; SoundFx_Loop siren_ = SoundFx_Loop{0, 0, 0.0f}; };
  KERN_STRUCT(SoundState) { EngineSound drone = EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}; EngineSound car = EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}; EngineSound jet = EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}; EngineSound siren = EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}; f32 wind = 0.0f; f32 drone_mix = 1.0f; f32 vehicle_mix = 1.0f; };

  // ---- functions ----
  struct SoundFx_readyResult { bool Result = false; };
  KERN_PURE SoundFx_readyResult SoundFx_ready(const SoundFx& self) {
    return {object_valid<kern::audio::Mixer>(self.mixer_)};
  }
};
