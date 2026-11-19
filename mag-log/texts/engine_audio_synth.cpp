struct engine_audio_synth : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(audio_Wave, square, saw, sine, triangle, noise, pink, pulse);
  KERN_STRUCT(audio_SynthPatch) { audio_Wave wave = audio_Wave::square; Float volume = 0.5; Float attack = 0.005; Float sustain = 0.1; Float punch = 0.0; Float release = 0.2; Float frequency = 440.0; Float slide = 0.0; Float slide_accel = 0.0; Float min_frequency = 20.0; Float vibrato_depth = 0.0; Float vibrato_rate = 6.0; Float arp_ratio = 1.0; Float arp_time = 0.0; Float duty = 0.5; Float duty_sweep = 0.0; Float fm_ratio = 0.0; Float fm_depth = 0.0; Float lowpass = 0.0; Float lowpass_sweep = 0.0; Float resonance = 0.0; Float highpass = 0.0; Float tremolo_depth = 0.0; Float tremolo_rate = 0.0; Float repeat = 0.0; Float bitcrush = 0.0; Int seed = 1; };
};
