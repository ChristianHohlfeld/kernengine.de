struct Level : kern::Blueprint {
  KERN_EVENT(on1, event_begin)
  void on1() {
    game_message("Level 1: sammle alle Muenzen, dann ins Ziel");
    actor_spawn<"Muenze">(Vector{0.0, 3.0, 0.5}, Vector{90.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0});
    actor_spawn<"Muenze">(Vector{0.0, 6.0, 0.5}, Vector{90.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0});
    actor_spawn<"Muenze">(Vector{0.0, 9.0, 0.5}, Vector{90.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0});
  }
};
