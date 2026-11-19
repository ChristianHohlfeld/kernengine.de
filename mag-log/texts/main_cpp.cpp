struct main_cpp : virtual kern::Library, virtual engine_gfx_renderer {
  // ---- functions ----
  struct SpawnCityApp_configureResult { RendererOptions Result = RendererOptions{960, 540, false, ShadowOptions{512, 120.0, 0.0012, 9}, BloomOptions{true, false, true}, AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}, true}; };
  KERN_PURE SpawnCityApp_configureResult SpawnCityApp_configure() {
    RendererOptions o = RendererOptions{960, 540, false, ShadowOptions{512, 120.0, 0.0012, 9}, BloomOptions{true, false, true}, AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}, true};
    o = RendererOptions{960, 540, false, ShadowOptions{512, 120.0, 0.0012, 9}, BloomOptions{true, false, true}, AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}, true};
    o.shadows = true;
    o.shadow.resolution = 2048;
    o.shadow.extent = 110.0;
    o.shadow.bias = 0.0014;
    o.bloom.enabled = true;
    return {o};
  }
};
