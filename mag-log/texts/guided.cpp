struct guided : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(guided_Dodge) { Float seconds = 0.0; Bool lost = false; };
  KERN_ENUM(guided_TargetKind, point, person, car, police_drone, player);
  KERN_STRUCT(guided_Missile) { Vector p = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; Vector goal = Vector{0.0, 0.0, 0.0}; Float born = 0.0; Float cruise_z = 0.0; Bool motor = false; Bool pedestrian = false; guided_Dodge dodge = guided_Dodge{0.0, false}; Vector dodge_last = Vector{0.0, 0.0, 0.0}; Bool tracking = true; };
  KERN_STRUCT(guided_Target) { guided_TargetKind kind = guided_TargetKind::point; Int index = -1; Int slot = 0; Text label = "ZIELPUNKT"; };
  KERN_STRUCT(guided_Flight) { guided_Missile m = guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true}; guided_Target t = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"}; Vector prev = Vector{0.0, 0.0, 0.0}; };
};
