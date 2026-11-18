struct engine_gfx_passes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(SkyState) { Vector sun = Vector{-0.45, -0.52, 0.72}; Vector moon = Vector{0.4, 0.5, 0.76}; Float night = 0.0; Bool clouds = true; Vector blast_direction = Vector{0.0, 0.0, -1.0}; Float blast_age = -1.0; Vector wind = Vector{0.012, 0.004, 0.0}; };
};
