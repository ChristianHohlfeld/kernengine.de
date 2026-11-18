struct town_sync : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(town_LampRec) { Int64 key = Int64{0x0}; Int state = 0; };
  KERN_STRUCT(town_MarkRec) { Vector at = Vector{0.0, 0.0, 0.0}; Float radius = 0.0; Float strength = 0.0; };
  KERN_STRUCT(town_PlankRec) { Int64 key = Int64{0x0}; Int door = 0; Int doors = 0; Int planks = 0; };
};
