struct combat_audio : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(combat_audio_Kind, shot, pistol, hit, crack, damage, scream, explosion, step, bounce, reward, fail, count);
};
