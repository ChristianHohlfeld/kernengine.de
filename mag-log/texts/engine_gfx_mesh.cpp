struct engine_gfx_mesh : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_ENUM(AlphaMode, opaque, mask, blend);
  KERN_STRUCT(Material) { Vec4 color = Vec4{1.0, 1.0, 1.0, 1.0}; AlphaMode alpha = AlphaMode::opaque; Float alpha_cutoff = 0.5; Float roughness = 1.0; Float metalness = 0.0; Vector emissive = Vector{0.0, 0.0, 0.0}; Bool unlit = false; Bool double_sided = false; Bool cast_shadow = true; Bool frustum_culled = true; Bool viewmodel = false; Bool linear_colors = false; Bool lines = false; Fixed<Int, 8> params = List<Int>{}; };
};
