struct engine_gfx_passes : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(AoQuality, low, medium, high, ultra);
  KERN_STRUCT(BloomOptions) { Bool enabled = true; Bool forced = false; Bool grade = true; };
  KERN_STRUCT(ShadowOptions) { Int resolution = 512; Float extent = 120.0; Float bias = 0.0012; Int filter = 9; };
  KERN_STRUCT(AoOptions) { Bool enabled = true; AoQuality quality = AoQuality::medium; Bool half_resolution = true; Float radius = 1.6; Float power = 2.2; Bool adaptive = true; };
};
