struct Level : kern::Blueprint {
  KERN_EVENT(on1, event_begin)
  void on1() {
    game_message("Level 2: das blaue Feld wirft dich hoch (+5)");
  }
};
