struct sound_fx : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(SoundFx_Loop) { Int clip = 0; Int64 voice = Int64{0x0}; Float volume = 0.0; };
  KERN_STRUCT(SoundFx) { Int mixer_ = 0; SoundFx_Loop rotor_ = SoundFx_Loop{0, Int64{0x0}, 0.0}; SoundFx_Loop prop_wash_ = SoundFx_Loop{0, Int64{0x0}, 0.0}; SoundFx_Loop engine_ = SoundFx_Loop{0, Int64{0x0}, 0.0}; SoundFx_Loop turbine_ = SoundFx_Loop{0, Int64{0x0}, 0.0}; SoundFx_Loop roar_ = SoundFx_Loop{0, Int64{0x0}, 0.0}; SoundFx_Loop wind_ = SoundFx_Loop{0, Int64{0x0}, 0.0}; SoundFx_Loop siren_ = SoundFx_Loop{0, Int64{0x0}, 0.0}; };

  // ---- functions ----
  struct SoundFx_readyResult { Bool Result = false; };
  KERN_PURE SoundFx_readyResult SoundFx_ready(const SoundFx& self) {
    return {math_compare<"!=">(kern::to<Float>(self.mixer_), 0.0)};
  }
};
