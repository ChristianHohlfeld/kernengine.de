struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Mat4) { Fixed<Float, 16> m = List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}; };
  KERN_STRUCT(Quat) { Float x = 0.0; Float y = 0.0; Float z = 0.0; Float w = 1.0; };
  KERN_STRUCT(Vec4) { Float x = 0.0; Float y = 0.0; Float z = 0.0; Float w = 0.0; };
  KERN_STRUCT(Camera) { Vector eye = Vector{0.0, -18.0, 8.0}; Vector target = Vector{0.0, 0.0, 2.0}; Vector up = Vector{0.0, 0.0, 1.0}; Float fov = 1.0471975511965976; Float near = 0.05; Float far = 1800.0; Bool orthographic = false; Float ortho_height = 20.0; Bool has_projection = false; Mat4 projection = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };

  // ---- functions ----
  struct Vec4_newResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Vec4_newResult Vec4_new(Float x_ = 0.0, Float y_ = 0.0, Float z_ = 0.0, Float w_ = 0.0) {
    Vec4 self = Vec4{0.0, 0.0, 0.0, 0.0};
    self.x = x_;
    self.y = y_;
    self.z = z_;
    self.w = w_;
    return {self};
  }

  struct Quat_axis_angleResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE Quat_axis_angleResult Quat_axis_angle(Vector axis = Vector{0.0, 0.0, 0.0}, Float radians = 0.0) {
    Vector a = Vector{0.0, 0.0, 0.0};
    Float s = 0.0;
    a = logic_select<Vector>(math_compare<">">(vector_length(axis), 9.999999717180685e-10), vector_normalize(axis), Vector{0.0, 0.0, 1.0});
    s = kern::formula([](Float radians) -> Float { return kern::math::sin((radians * 0.5)); }, radians);
    return {struct_make<Quat>(kern::formula([](Float x, Float s) -> Float { return (x * s); }, vector_break(a).x, s), kern::formula([](Float y, Float s) -> Float { return (y * s); }, vector_break(a).y, s), kern::formula([](Float z, Float s) -> Float { return (z * s); }, vector_break(a).z, s), kern::formula([](Float radians) -> Float { return kern::math::cos((radians * 0.5)); }, radians))};
  }

  struct Quat_rotateResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Quat_rotateResult Quat_rotate(const Quat& self, Vector v = Vector{0.0, 0.0, 0.0}) {
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector t = Vector{0.0, 0.0, 0.0};
    q = vector_make(self.x, self.y, self.z);
    t = vector_scale(vector_cross(q, v), 2.0);
    return {vector_add(vector_add(v, vector_scale(t, self.w)), vector_cross(q, t))};
  }

  struct Mat4_translationResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Mat4_translationResult Mat4_translation(const Mat4& self) {
    return {vector_make(self.m[12], self.m[13], self.m[14])};
  }

  struct Mat4_mulResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_mulResult Mat4_mul(const Mat4& self, const Mat4& b) {
    Mat4 r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float b0 = 0.0;
    Float b1 = 0.0;
    Float b2 = 0.0;
    Float b3 = 0.0;
    r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it10 = flow_for(n10, 0, 3); it10.next();) {
      b0 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return (4.0 * index); }, kern::to<Float>(n10.index)))];
      b1 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 1.0); }, kern::to<Float>(n10.index)))];
      b2 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 2.0); }, kern::to<Float>(n10.index)))];
      b3 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return ((4.0 * index) + 3.0); }, kern::to<Float>(n10.index)))];
      for (auto it11 = flow_for(n11, 0, 3); it11.next();) {
        r.m[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((4.0 * index) + index_2); }, kern::to<Float>(n10.index), kern::to<Float>(n11.index)))] = kern::formula([](Float a, Float b0, Float a_2, Float b1, Float a_3, Float b2, Float a_4, Float b3) -> Float { return ((((a * b0) + (a_2 * b1)) + (a_3 * b2)) + (a_4 * b3)); }, self.m[n11.index], b0, self.m[kern::to<Int>(kern::formula([](Float index) -> Float { return (4.0 + index); }, kern::to<Float>(n11.index)))], b1, self.m[kern::to<Int>(kern::formula([](Float index) -> Float { return (8.0 + index); }, kern::to<Float>(n11.index)))], b2, self.m[kern::to<Int>(kern::formula([](Float index) -> Float { return (12.0 + index); }, kern::to<Float>(n11.index)))], b3);
      }
    }
    return {r};
  }

  struct Mat4_translateResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_translateResult Mat4_translate(Vector t = Vector{0.0, 0.0, 0.0}) {
    Mat4 r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r.m[12] = vector_break(t).x;
    r.m[13] = vector_break(t).y;
    r.m[14] = vector_break(t).z;
    return {r};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};
