struct engine_gfx_renderer : virtual kern::Library, virtual engine_gfx_passes {
  // ---- types ----
  KERN_STRUCT(RendererOptions) { Int width = 960; Int height = 540; Bool shadows = false; ShadowOptions shadow = ShadowOptions{512, 120.0, 0.0012, 9}; BloomOptions bloom = BloomOptions{true, false, true}; AoOptions ao = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}; Bool grade = true; };
};
