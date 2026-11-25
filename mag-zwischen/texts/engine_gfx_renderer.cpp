struct engine_gfx_renderer : virtual kern::Library, virtual engine_gfx_passes {
  // ---- types ----
  KERN_STRUCT(Lighting) { Vector sun_direction = Vector{-0.45, -0.52, 0.72}; Vector sun_color = Vector{2.8, 2.47, 2.11}; Vector sky_color = Vector{0.469, 0.609, 0.85}; Vector ground_color = Vector{0.125, 0.094, 0.054}; Vector fog_color = Vector{0.474, 0.584, 0.716}; Float fog_density = 0.00085; Float exposure = 1.0; Float env_intensity = 0.9; Vector env_zenith = Vector{0.05, 0.2, 0.62}; Vector env_horizon = Vector{0.52, 0.66, 0.86}; Vector env_ground = Vector{0.105, 0.127, 0.084}; Float fog_height_falloff = 0.0; Float fog_sun_scatter = 0.0; Float fog_horizon = 0.0; };
  KERN_STRUCT(PointLights) { U32 count = 0; Fixed<Float, 64> pos_range = List<Float>{}; Fixed<Float, 64> color_intensity = List<Float>{}; };
  KERN_STRUCT(RendererOptions) { I32 width = 960; I32 height = 540; Bool shadows = false; ShadowOptions shadow = ShadowOptions{512, 120.0, 0.0012, 9}; BloomOptions bloom = BloomOptions{true, false, true}; AoOptions ao = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}; Bool grade = true; };
};
