struct game_townsync : virtual kern::Library, virtual town_sync {
  // ---- types ----
  KERN_STRUCT(TownSync) { Int epoch = 1; Bool was_active = false; Bool hosting = false; Bool marks_taken = false; List<Int> known = List<Int>{}; List<town_LampRec> lamps_out = List<town_LampRec>{}; List<town_MarkRec> marks = List<town_MarkRec>{}; List<town_MarkRec> marks_in = List<town_MarkRec>{}; Fixed<Int, 3> car_hits = List<Int>{0, 0, 0}; Int person_hits = 0; Int sent_states = 0; Int got_states = 0; Int asks = 0; };
};
