struct Sprungfeld : kern::Blueprint {
  /// Wie oft das Feld jemanden hochgeworfen hat.
  i64 Spruenge = 0;

  KERN_EVENT(on1, game_overlap_begin)
  void on1(Actor actor, Actor other) {
    Spruenge += 1;
    actor_impulse(other, Vector{0.0, 0.0, 6.0});
    game_sound<"builtin:boost">(actor_location(actor), 1.0, 1.0, true);
    game_add_score(5.0);
  }
};
