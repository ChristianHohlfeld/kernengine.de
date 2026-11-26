struct engine_audio_audio : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(audio_Bus, master, music, sfx, ambience, ui, voice, count);
  KERN_STRUCT(audio_BusSettings) { Float volume = 1.0; Bool muted = false; Float lowpass_hz = 0.0; Float reverb_send = 0.0; };
  KERN_ENUM(audio_Rolloff, inverse, linear, exponential);
  KERN_STRUCT(audio_PlayParams) { Float volume = 1.0; Float pitch = 1.0; Float pan = 0.0; Bool the_loop = false; audio_Bus bus = audio_Bus::sfx; Float fade_in = 0.0; Float start = 0.0; I32 priority = 0; Bool spatial = false; Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; Float min_distance = 1.0; Float max_distance = 50.0; audio_Rolloff rolloff = audio_Rolloff::inverse; Float spread = 0.0; Float doppler = 1.0; };
};
