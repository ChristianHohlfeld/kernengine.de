struct town_sync : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(town_Cause, other, round_, blast);
  KERN_ENUM(town_Kind, none, car_hit, person_hit, lamps, plank_step, planks, state, ask);
  KERN_STRUCT(town_LampRec) { Int64 key = Int64{0x0}; Int state = 0; };
  KERN_STRUCT(town_MarkRec) { Vector at = Vector{0.0, 0.0, 0.0}; Float radius = 0.0; Float strength = 0.0; };
  KERN_STRUCT(town_PersonHit) { Int person = 0; Int seed = 0; Float damage = 0.0; Int by = 0; Vector push = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(town_PlankRec) { Int64 key = Int64{0x0}; Int door = 0; Int doors = 0; Int planks = 0; };
  KERN_STRUCT(town_PlankStep) { Int64 key = Int64{0x0}; Int door = 0; Int doors = 0; Int step = 0; };
  KERN_STRUCT(town_CarHit) { Int car = 0; Float damage = 0.0; town_Cause cause = town_Cause::other; };
  KERN_STRUCT(town_State) { Int epoch = 0; Bool first = false; Bool last = false; List<town_LampRec> lamps = List<town_LampRec>{}; List<town_PlankRec> planks = List<town_PlankRec>{}; List<town_MarkRec> marks = List<town_MarkRec>{}; };
};
