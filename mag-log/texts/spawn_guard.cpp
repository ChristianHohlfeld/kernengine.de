struct spawn_guard : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(spawn_Viewer) { Vector eye = Vector{0.0, 0.0, 0.0}; Vector forward = Vector{1.0, 0.0, 0.0}; Float half_angle = 1.5707963267948966; Float far = 1500.0; Bool local = false; };
};
