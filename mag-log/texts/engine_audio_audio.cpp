struct engine_audio_audio : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(audio_Bus, master, music, sfx, ambience, ui, voice, count);
  KERN_ENUM(audio_Rolloff, inverse, linear, exponential);
};
