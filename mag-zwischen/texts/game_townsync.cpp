struct game_townsync : virtual kern::Library, virtual town_sync {
  // ---- types ----
  KERN_STRUCT(TownSync) { U32 epoch = 1; Bool was_active = false; Bool hosting = false; Bool marks_taken = false; List<U8> known = List<U8>{}; List<town_LampRec> lamps_out = List<town_LampRec>{}; List<town_MarkRec> marks = List<town_MarkRec>{}; List<town_MarkRec> marks_in = List<town_MarkRec>{}; Fixed<U32, 3> car_hits = List<U32>{0, 0, 0}; U32 person_hits = 0; U32 sent_states = 0; U32 got_states = 0; U32 asks = 0; };
};
