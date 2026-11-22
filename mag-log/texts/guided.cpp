struct guided : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(guided_TargetKind, point, person, car, police_drone, player);
  KERN_STRUCT(guided_Target) { guided_TargetKind kind = guided_TargetKind::point; I32 index = -1; U8 slot = 0; Text label = "ZIELPUNKT"; };
};
