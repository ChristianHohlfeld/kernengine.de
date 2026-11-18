struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Quat) { Float x = 0.0; Float y = 0.0; Float z = 0.0; Float w = 1.0; };
  KERN_STRUCT(Vec4) { Float x = 0.0; Float y = 0.0; Float z = 0.0; Float w = 0.0; };

  // ---- functions ----
  struct Quat_axis_angleResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE Quat_axis_angleResult Quat_axis_angle(Vector axis = Vector{0.0, 0.0, 0.0}, Float radians = 0.0) {
    Vector a = Vector{0.0, 0.0, 0.0};
    Float s = 0.0;
    a = logic_select<Vector>(math_compare<">">(vector_length(axis), 9.999999717180685e-10), vector_normalize(axis), Vector{0.0, 0.0, 1.0});
    s = kern::formula([](Float radians) -> Float { return kern::math::sin((radians * 0.5)); }, radians);
    return {struct_make<Quat>(kern::formula([](Float x, Float s) -> Float { return (x * s); }, vector_break(a).x, s), kern::formula([](Float y, Float s) -> Float { return (y * s); }, vector_break(a).y, s), kern::formula([](Float z, Float s) -> Float { return (z * s); }, vector_break(a).z, s), kern::formula([](Float radians) -> Float { return kern::math::cos((radians * 0.5)); }, radians))};
  }

  struct Quat_mulResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE Quat_mulResult Quat_mul(const Quat& self, const Quat& b) {
    return {struct_make<Quat>(kern::formula([](Float w, Float x, Float x_2, Float w_2, Float y, Float z, Float z_2, Float y_2) -> Float { return ((((w * x) + (x_2 * w_2)) + (y * z)) - (z_2 * y_2)); }, self.w, b.x, self.x, b.w, self.y, b.z, self.z, b.y), kern::formula([](Float w, Float y, Float x, Float z, Float y_2, Float w_2, Float z_2, Float x_2) -> Float { return ((((w * y) - (x * z)) + (y_2 * w_2)) + (z_2 * x_2)); }, self.w, b.y, self.x, b.z, self.y, b.w, self.z, b.x), kern::formula([](Float w, Float z, Float x, Float y, Float y_2, Float x_2, Float z_2, Float w_2) -> Float { return ((((w * z) + (x * y)) - (y_2 * x_2)) + (z_2 * w_2)); }, self.w, b.z, self.x, b.y, self.y, b.x, self.z, b.w), kern::formula([](Float w, Float w_2, Float x, Float x_2, Float y, Float y_2, Float z, Float z_2) -> Float { return ((((w * w_2) - (x * x_2)) - (y * y_2)) - (z * z_2)); }, self.w, b.w, self.x, b.x, self.y, b.y, self.z, b.z))};
  }

  struct Quat_rotateResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Quat_rotateResult Quat_rotate(const Quat& self, Vector v = Vector{0.0, 0.0, 0.0}) {
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector t = Vector{0.0, 0.0, 0.0};
    q = vector_make(self.x, self.y, self.z);
    t = vector_scale(vector_cross(q, v), 2.0);
    return {vector_add(vector_add(v, vector_scale(t, self.w)), vector_cross(q, t))};
  }
};
