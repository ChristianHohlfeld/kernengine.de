struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Mat4) { Fixed<f32, 16> m = List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_STRUCT(Quat) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 1.0f; };
  KERN_STRUCT(Vec4) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 0.0f; };
  KERN_STRUCT(Camera) { Vector eye = Vector{0.0, -18.0, 8.0}; Vector target = Vector{0.0, 0.0, 2.0}; Vector up = Vector{0.0, 0.0, 1.0}; f32 fov = 1.0471976f; f32 near = 0.05f; f32 far = 1800.0f; bool orthographic = false; f32 ortho_height = 20.0f; bool has_projection = false; Mat4 projection = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; };
};
