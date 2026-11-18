struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Mat4) { Fixed<Float, 16> m = List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}; };
  KERN_STRUCT(Quat) { Float x = 0.0; Float y = 0.0; Float z = 0.0; Float w = 1.0; };
  KERN_STRUCT(Vec4) { Float x = 0.0; Float y = 0.0; Float z = 0.0; Float w = 0.0; };

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

  struct Mat4_trsResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  /// Translation * rotation * scale.
  KERN_PURE Mat4_trsResult Mat4_trs(Vector t = Vector{0.0, 0.0, 0.0}, Quat q = Quat{0.0, 0.0, 0.0, 1.0}, Vector s = Vector{0.0, 0.0, 0.0}) {
    Float x2 = 0.0;
    Float y2 = 0.0;
    Float z2 = 0.0;
    Float xx = 0.0;
    Float xy = 0.0;
    Float xz = 0.0;
    Float yy = 0.0;
    Float yz = 0.0;
    Float zz = 0.0;
    Float wx = 0.0;
    Float wy = 0.0;
    Float wz = 0.0;
    Mat4 r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    x2 = kern::formula([](Float x) -> Float { return (x + x); }, q.x);
    y2 = kern::formula([](Float y) -> Float { return (y + y); }, q.y);
    z2 = kern::formula([](Float z) -> Float { return (z + z); }, q.z);
    xx = kern::formula([](Float x, Float x2) -> Float { return (x * x2); }, q.x, x2);
    xy = kern::formula([](Float x, Float y2) -> Float { return (x * y2); }, q.x, y2);
    xz = kern::formula([](Float x, Float z2) -> Float { return (x * z2); }, q.x, z2);
    yy = kern::formula([](Float y, Float y2) -> Float { return (y * y2); }, q.y, y2);
    yz = kern::formula([](Float y, Float z2) -> Float { return (y * z2); }, q.y, z2);
    zz = kern::formula([](Float z, Float z2) -> Float { return (z * z2); }, q.z, z2);
    wx = kern::formula([](Float w, Float x2) -> Float { return (w * x2); }, q.w, x2);
    wy = kern::formula([](Float w, Float y2) -> Float { return (w * y2); }, q.w, y2);
    wz = kern::formula([](Float w, Float z2) -> Float { return (w * z2); }, q.w, z2);
    r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r.m[0] = kern::formula([](Float yy, Float zz, Float x) -> Float { return (((1.0 - yy) - zz) * x); }, yy, zz, vector_break(s).x);
    r.m[1] = kern::formula([](Float xy, Float wz, Float x) -> Float { return ((xy + wz) * x); }, xy, wz, vector_break(s).x);
    r.m[2] = kern::formula([](Float xz, Float wy, Float x) -> Float { return ((xz - wy) * x); }, xz, wy, vector_break(s).x);
    r.m[3] = 0.0;
    r.m[4] = kern::formula([](Float xy, Float wz, Float y) -> Float { return ((xy - wz) * y); }, xy, wz, vector_break(s).y);
    r.m[5] = kern::formula([](Float xx, Float zz, Float y) -> Float { return (((1.0 - xx) - zz) * y); }, xx, zz, vector_break(s).y);
    r.m[6] = kern::formula([](Float yz, Float wx, Float y) -> Float { return ((yz + wx) * y); }, yz, wx, vector_break(s).y);
    r.m[7] = 0.0;
    r.m[8] = kern::formula([](Float xz, Float wy, Float z) -> Float { return ((xz + wy) * z); }, xz, wy, vector_break(s).z);
    r.m[9] = kern::formula([](Float yz, Float wx, Float z) -> Float { return ((yz - wx) * z); }, yz, wx, vector_break(s).z);
    r.m[10] = kern::formula([](Float xx, Float yy, Float z) -> Float { return (((1.0 - xx) - yy) * z); }, xx, yy, vector_break(s).z);
    r.m[11] = 0.0;
    r.m[12] = vector_break(t).x;
    r.m[13] = vector_break(t).y;
    r.m[14] = vector_break(t).z;
    r.m[15] = 1.0;
    return {r};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};
