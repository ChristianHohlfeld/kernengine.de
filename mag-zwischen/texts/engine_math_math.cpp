struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Mat4) { Fixed<f32, 16> m = List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_STRUCT(Quat) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 1.0f; };
  KERN_STRUCT(Vec4) { f32 x = 0.0f; f32 y = 0.0f; f32 z = 0.0f; f32 w = 0.0f; };
  KERN_STRUCT(Camera) { Vector eye = Vector{0.0, -18.0, 8.0}; Vector target = Vector{0.0, 0.0, 2.0}; Vector up = Vector{0.0, 0.0, 1.0}; f32 fov = 1.0471976f; f32 near = 0.05f; f32 far = 1800.0f; bool orthographic = false; f32 ortho_height = 20.0f; bool has_projection = false; Mat4 projection = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; };

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

  struct Mat4_mulResult { Mat4 Result = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; };
  KERN_PURE Mat4_mulResult Mat4_mul(const Mat4& self, const Mat4& b) {
    Mat4 r = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    f32 b0 = 0.0f;
    f32 b1 = 0.0f;
    f32 b2 = 0.0f;
    f32 b3 = 0.0f;
    r = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    for (auto it10 = flow_for(n10, 0, 3); it10.next();) {
      b0 = b.m[num_cast<i64, i32>(kern::formula<i64>([](auto index) { return (4 * num_cast<i32, i64>(index)); }, n10.index))];
      b1 = b.m[num_cast<i64, i32>(kern::formula<i64>([](auto index) { return ((4 * num_cast<i32, i64>(index)) + 1); }, n10.index))];
      b2 = b.m[num_cast<i64, i32>(kern::formula<i64>([](auto index) { return ((4 * num_cast<i32, i64>(index)) + 2); }, n10.index))];
      b3 = b.m[num_cast<i64, i32>(kern::formula<i64>([](auto index) { return ((4 * num_cast<i32, i64>(index)) + 3); }, n10.index))];
      for (auto it11 = flow_for(n11, 0, 3); it11.next();) {
        r.m[num_cast<i64, i32>(kern::formula<i64, i64>([](auto index, auto index_2) { return ((4 * num_cast<i32, i64>(index)) + num_cast<i32, i64>(index_2)); }, n10.index, n11.index))] = kern::formula<f32, f32, f32, f32, f32, f32, f32, f32>([](auto a, auto b0, auto a_2, auto b1, auto a_3, auto b2, auto a_4, auto b3) { return ((((a * b0) + (a_2 * b1)) + (a_3 * b2)) + (a_4 * b3)); }, self.m[num_cast<i64, i32>(num_cast<i32, i64>(n11.index))], b0, self.m[num_cast<i64, i32>(kern::formula<i64>([](auto index) { return (4 + num_cast<i32, i64>(index)); }, n11.index))], b1, self.m[num_cast<i64, i32>(kern::formula<i64>([](auto index) { return (8 + num_cast<i32, i64>(index)); }, n11.index))], b2, self.m[num_cast<i64, i32>(kern::formula<i64>([](auto index) { return (12 + num_cast<i32, i64>(index)); }, n11.index))], b3);
      }
    }
    return {r};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
};
