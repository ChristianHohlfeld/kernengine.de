struct engine_gfx_mesh : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_ENUM(AlphaMode, opaque, mask, blend);
  KERN_STRUCT(Material) { Vec4 color = Vec4{1.0f, 1.0f, 1.0f, 1.0f}; AlphaMode alpha = AlphaMode::opaque; f32 alpha_cutoff = 0.5f; f32 roughness = 1.0f; f32 metalness = 0.0f; Vector emissive = Vector{0.0, 0.0, 0.0}; bool unlit = false; bool double_sided = false; bool cast_shadow = true; bool frustum_culled = true; bool viewmodel = false; bool linear_colors = false; bool lines = false; Fixed<i32, 8> params = List<i32>{}; };
};
