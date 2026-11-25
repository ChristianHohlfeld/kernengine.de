struct engine_gfx_passes : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(AoQuality, low, medium, high, ultra);
  KERN_STRUCT(AoOptions) { Bool enabled = true; AoQuality quality = AoQuality::medium; Bool half_resolution = true; Float radius = 1.6; Float power = 2.2; Bool adaptive = true; };
};
