struct Level : kern::Blueprint {
  KERN_EVENT(on1, event_begin)
  void on1() {
    game_message("Level 1: sammle alle Muenzen, dann ins Ziel");
  }
};
