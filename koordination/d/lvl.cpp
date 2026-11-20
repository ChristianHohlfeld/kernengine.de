struct Level : kern::Blueprint {
  Int Count = 0;

  void Note() {
    engine_game_world_events(n2, 1);
    list_add(n2.result, "x");
    Count = list_length<Text>(n2.result);
  }

  struct { engine_game_world_events_place<List<Text>> result = {}; } n2;
};
