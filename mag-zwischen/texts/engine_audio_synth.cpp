struct engine_audio_synth : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(audio_Wave, square, saw, sine, triangle, noise, pink, pulse);
  KERN_STRUCT(audio_SynthPatch) { audio_Wave wave = audio_Wave::square; f32 volume = 0.5f; f32 attack = 0.005f; f32 sustain = 0.1f; f32 punch = 0.0f; f32 release = 0.2f; f32 frequency = 440.0f; f32 slide = 0.0f; f32 slide_accel = 0.0f; f32 min_frequency = 20.0f; f32 vibrato_depth = 0.0f; f32 vibrato_rate = 6.0f; f32 arp_ratio = 1.0f; f32 arp_time = 0.0f; f32 duty = 0.5f; f32 duty_sweep = 0.0f; f32 fm_ratio = 0.0f; f32 fm_depth = 0.0f; f32 lowpass = 0.0f; f32 lowpass_sweep = 0.0f; f32 resonance = 0.0f; f32 highpass = 0.0f; f32 tremolo_depth = 0.0f; f32 tremolo_rate = 0.0f; f32 repeat = 0.0f; f32 bitcrush = 0.0f; u32 seed = 1; };
};
