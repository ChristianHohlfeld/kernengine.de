struct engine_audio_audio : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(audio_Bus, master, music, sfx, ambience, ui, voice, count);
  KERN_STRUCT(audio_BusSettings) { f32 volume = 1.0f; bool muted = false; f32 lowpass_hz = 0.0f; f32 reverb_send = 0.0f; };
  KERN_ENUM(audio_Rolloff, inverse, linear, exponential);
  KERN_STRUCT(audio_PlayParams) { f32 volume = 1.0f; f32 pitch = 1.0f; f32 pan = 0.0f; bool the_loop = false; audio_Bus bus = audio_Bus::sfx; f32 fade_in = 0.0f; f32 start = 0.0f; i32 priority = 0; bool spatial = false; Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; f32 min_distance = 1.0f; f32 max_distance = 50.0f; audio_Rolloff rolloff = audio_Rolloff::inverse; f32 spread = 0.0f; f32 doppler = 1.0f; };
};
