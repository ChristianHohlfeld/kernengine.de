struct Level : kern::Blueprint {
  KERN_EVENT(on1, event_begin)
  void on1() {
    game_message("Level 2: das blaue Feld wirft dich hoch (+5)");
    actor_spawn<"Muenze">(Vector{0.0, 4.0, 0.5}, Vector{90.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0});
    actor_spawn<"Muenze">(Vector{3.0, 8.0, 0.5}, Vector{90.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0});
    actor_spawn<"Muenze">(Vector{3.0, 12.0, 0.5}, Vector{90.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0});
  }
};
