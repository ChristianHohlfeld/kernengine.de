struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Quat) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 1.0f; };
  KERN_STRUCT(Vec4) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 0.0f; };

  // ---- functions ----
  struct Vec4_newResult { Vec4 Result = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; };
  KERN_PURE Vec4_newResult Vec4_new(f32 x_ = 0.0f, f32 y_ = 0.0f, f32 z_ = 0.0f, f32 w_ = 0.0f) {
    Vec4 self = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    self.x = x_;
    self.y = y_;
    self.z = z_;
    self.w = w_;
    return {self};
  }
};
