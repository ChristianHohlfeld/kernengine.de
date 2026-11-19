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

  struct Quat_rotateResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Quat_rotateResult Quat_rotate(const Quat& self, Vector v = Vector{0.0, 0.0, 0.0}) {
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector t = Vector{0.0, 0.0, 0.0};
    q = vector_make(self.x, self.y, self.z);
    t = vector_scale(vector_cross(q, v), 2.0);
    return {vector_add(vector_add(v, vector_scale(t, self.w)), vector_cross(q, t))};
  }
};
