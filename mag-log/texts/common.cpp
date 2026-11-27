struct common : virtual kern::Library, virtual engine_physics_physics {
  // ---- types ----
  KERN_STRUCT(Rng) { u32 s = 2654435769; f32 spare_ = 0.0f; bool has_spare_ = false; };
  KERN_STRUCT(the_RayResult_) { bool hit = false; f32 distance = 0.0f; Vector point = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 1.0}; };
  KERN_STRUCT(BodyState) { Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; Vector angular = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };

  // ---- variables ----
  KERN_CONST f32 kPi = 3.1415927f;
  KERN_CONST f32 kG = 9.80665f;
  KERN_CONST f32 kDeg = 0.017453292f;

  // ---- functions ----
  struct clampfResult { f32 Result = 0.0f; };
  KERN_PURE clampfResult clampf(f32 v = 0.0f, f32 lo = 0.0f, f32 hi = 0.0f) {
    return {kern::formula<f32, f32, f32>([](auto v, auto lo, auto hi) { return ((v < lo) ? lo : ((v > hi) ? hi : v)); }, v, lo, hi)};
  }

  struct sign0Result { f32 Result = 0.0f; };
  KERN_PURE sign0Result sign0(f32 v = 0.0f) {
    return {kern::formula<f32, f32, f32, f32>([](auto v, auto a0f, auto a, auto a0f_2) { return ((v > 0.0f) ? a0f : ((v < 0.0f) ? a : a0f_2)); }, v, 1.0f, -1.0f, 0.0f)};
  }

  struct rate_limitResult { f32 Result = 0.0f; };
  KERN_PURE rate_limitResult rate_limit(f32 cur = 0.0f, f32 target = 0.0f, f32 max_step = 0.0f) {
    return {kern::formula<f32, f32>([](auto cur, auto the_result) { return (cur + the_result); }, cur, clampf(kern::formula<f32, f32>([](auto target, auto cur) { return (target - cur); }, target, cur), kern::formula<f32>([](auto max_step) { return (-max_step); }, max_step), max_step).Result)};
  }

  struct damp_alphaResult { f32 Result = 0.0f; };
  /// Exponential approach: the share of the remaining distance covered in dt at `rate` per second.
  KERN_PURE damp_alphaResult damp_alpha(f32 rate = 0.0f, f32 dt = 0.0f) {
    return {kern::formula<f32, f32, f32>([](auto rate, auto dt, auto a0f) { return (((rate > 0.0f) && (dt > 0.0f)) ? (1.0f - exp(((-rate) * dt))) : a0f); }, rate, dt, 0.0f)};
  }

  struct wrap_piResult { f32 Result = 0.0f; };
  KERN_PURE wrap_piResult wrap_pi(f32 a = 0.0f) {
    f32 a_ = 0.0f;
    a_ = a;
    while (loop(math_compare<">", f32>(a_, kPi))) {
      a_ = kern::formula<f32, f32>([](auto a_, auto kPi) { return (a_ - (2.0f * kPi)); }, a_, kPi);
    }
    while (loop(math_compare<"<", f32>(a_, kern::formula<f32>([](auto kPi) { return (-kPi); }, kPi)))) {
      a_ += kern::formula<f32>([](auto kPi) { return (2.0f * kPi); }, kPi);
    }
    return {a_};
  }

  struct conjResult { Quat Result = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_PURE conjResult conj(Quat q = Quat{0.0f, 0.0f, 0.0f, 1.0f}) {
    return {struct_make<Quat>(kern::formula<f32>([](auto x) { return (-x); }, q.x), kern::formula<f32>([](auto y) { return (-y); }, q.y), kern::formula<f32>([](auto z) { return (-z); }, q.z), q.w)};
  }

  struct to_localResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE to_localResult to_local(Quat q = Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector v = Vector{0.0, 0.0, 0.0}) {
    Vector rotate = Vector{0.0, 0.0, 0.0};
    rotate = engine_quat_rotate(conj(q).Result, v);
    return {rotate};
  }

  struct qnormalizeResult { Quat Result = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_PURE qnormalizeResult qnormalize(Quat q = Quat{0.0f, 0.0f, 0.0f, 1.0f}) {
    f32 l = 0.0f;
    l = kern::formula<f32, f32, f32, f32>([](auto x, auto y, auto z, auto w) { return sqrt(((((x * x) + (y * y)) + (z * z)) + (w * w))); }, q.x, q.y, q.z, q.w);
    return {logic_select<Quat>(math_compare<">", f32>(l, 1e-09f), struct_make<Quat>(kern::formula<f32, f32>([](auto x, auto l) { return (x / l); }, q.x, l), kern::formula<f32, f32>([](auto y, auto l) { return (y / l); }, q.y, l), kern::formula<f32, f32>([](auto z, auto l) { return (z / l); }, q.z, l), kern::formula<f32, f32>([](auto w, auto l) { return (w / l); }, q.w, l)), struct_make<Quat>(0.0f, 0.0f, 0.0f, 1.0f))};
  }

  struct yaw_quatResult { Quat Result = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_PURE yaw_quatResult yaw_quat(f32 yaw = 0.0f) {
    Quat axis_angle = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    axis_angle = engine_quat_axis_angle(Vector{0.0, 0.0, 1.0}, yaw);
    return {axis_angle};
  }

  struct heading_ofResult { f32 Result = 0.0f; };
  /// Heading (radians, CCW from +X) of a body axis projected on the ground plane.
  KERN_PURE heading_ofResult heading_of(Vector axis = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula<f32, f32>([](auto y, auto x) { return atan2(y, x); }, vector_break(axis).y, vector_break(axis).x)};
  }

  struct slerpResult { Quat Result = Quat{0.0f, 0.0f, 0.0f, 1.0f}; };
  KERN_PURE slerpResult slerp(Quat a = Quat{0.0f, 0.0f, 0.0f, 1.0f}, Quat b = Quat{0.0f, 0.0f, 0.0f, 1.0f}, f32 t = 0.0f) {
    Quat b_ = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    f32 d = 0.0f;
    f32 th = 0.0f;
    f32 s = 0.0f;
    f32 wa = 0.0f;
    f32 wb = 0.0f;
    b_ = b;
    d = kern::formula<f32, f32, f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2, auto z, auto z_2, auto w, auto w_2) { return ((((x * x_2) + (y * y_2)) + (z * z_2)) + (w * w_2)); }, a.x, b_.x, a.y, b_.y, a.z, b_.z, a.w, b_.w);
    if (math_compare<"<", f32>(d, 0.0f)) {
      b_ = struct_make<Quat>(kern::formula<f32>([](auto x) { return (-x); }, b_.x), kern::formula<f32>([](auto y) { return (-y); }, b_.y), kern::formula<f32>([](auto z) { return (-z); }, b_.z), kern::formula<f32>([](auto w) { return (-w); }, b_.w));
      d = kern::formula<f32>([](auto d) { return (-d); }, d);
    }
    if (math_compare<">", f32>(d, 0.9995f)) {
      return {qnormalize(struct_make<Quat>(kern::formula<f32, f32, f32>([](auto x, auto x_2, auto t) { return (x + ((x_2 - x) * t)); }, a.x, b_.x, t), kern::formula<f32, f32, f32>([](auto y, auto y_2, auto t) { return (y + ((y_2 - y) * t)); }, a.y, b_.y, t), kern::formula<f32, f32, f32>([](auto z, auto z_2, auto t) { return (z + ((z_2 - z) * t)); }, a.z, b_.z, t), kern::formula<f32, f32, f32>([](auto w, auto w_2, auto t) { return (w + ((w_2 - w) * t)); }, a.w, b_.w, t))).Result};
    }
    th = kern::formula<f32>([](auto d) { return acos(d); }, d);
    s = kern::formula<f32>([](auto th) { return sin(th); }, th);
    wa = kern::formula<f32, f32, f32>([](auto t, auto th, auto s) { return (sin(((1.0f - t) * th)) / s); }, t, th, s);
    wb = kern::formula<f32, f32, f32>([](auto t, auto th, auto s) { return (sin((t * th)) / s); }, t, th, s);
    return {struct_make<Quat>(kern::formula<f32, f32, f32, f32>([](auto x, auto wa, auto x_2, auto wb) { return ((x * wa) + (x_2 * wb)); }, a.x, wa, b_.x, wb), kern::formula<f32, f32, f32, f32>([](auto y, auto wa, auto y_2, auto wb) { return ((y * wa) + (y_2 * wb)); }, a.y, wa, b_.y, wb), kern::formula<f32, f32, f32, f32>([](auto z, auto wa, auto z_2, auto wb) { return ((z * wa) + (z_2 * wb)); }, a.z, wa, b_.z, wb), kern::formula<f32, f32, f32, f32>([](auto w, auto wa, auto w_2, auto wb) { return ((w * wa) + (w_2 * wb)); }, a.w, wa, b_.w, wb))};
  }

  struct read_bodyResult { BodyState Result = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}}; };
  KERN_PURE read_bodyResult read_body(Object<kern::physics::World> w = Object<kern::physics::World>{}, physics_Body b = physics_Body{0}) {
    physics_Pose p = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    pose = engine_world_pose(w, b);
    p = pose;
    linear_velocity = engine_world_linear_velocity(w, b);
    angular_velocity = engine_world_angular_velocity(w, b);
    return {struct_make<BodyState>(p.position, linear_velocity, angular_velocity, p.rotation)};
  }

  struct Rng_seededResult { Rng Result = Rng{2654435769, 0.0f, false}; };
  /// The generator for a seed that may be 0 (0: the default seed).
  KERN_PURE Rng_seededResult Rng_seeded(u32 seed = 0) {
    return {struct_make<Rng>(kern::formula<u32, u32>([](auto seed, auto a2654435769) { return (num_cast<bool, u32>(seed) ? seed : a2654435769); }, seed, 2654435769), 0.0f, false)};
  }

  struct Rng_nextResult { u32 Result = 0; };
  Rng_nextResult Rng_next(Rng& self) {
    self.s = kern::formula<u32>([](auto s) { return (s ^ (s << 13)); }, self.s);
    self.s = kern::formula<u32>([](auto s) { return (s ^ (s >> 17)); }, self.s);
    self.s = kern::formula<u32>([](auto s) { return (s ^ (s << 5)); }, self.s);
    return {self.s};
  }

  struct Rng_unitResult { f32 Result = 0.0f; };
  Rng_unitResult Rng_unit(Rng& self) {
    n10 = Rng_next(self);
    return {kern::formula<u32>([](auto the_result) { return (num_cast<f32, u32>((the_result >> 8)) * 5.9604645e-08f); }, n10.Result)};
  }

  struct Rng_rangeResult { f32 Result = 0.0f; };
  Rng_rangeResult Rng_range(Rng& self, f32 a = 0.0f, f32 b = 0.0f) {
    n11 = Rng_unit(self);
    return {kern::formula<f32, f32, f32>([](auto a, auto b, auto the_result) { return (a + ((b - a) * the_result)); }, a, b, n11.Result)};
  }

  struct Rng_gaussianResult { f32 Result = 0.0f; };
  /// Standard normal (Box-Muller), as SpawnCity's sensor noise.
  Rng_gaussianResult Rng_gaussian(Rng& self) {
    f32 u = 0.0f;
    f32 v = 0.0f;
    f32 r = 0.0f;
    if (self.has_spare_) {
      self.has_spare_ = false;
      return {self.spare_};
    }
    n12 = Rng_unit(self);
    u = kern::formula<f32>([](auto the_result) { return fmax(1e-12f, the_result); }, n12.Result);
    n13 = Rng_unit(self);
    v = n13.Result;
    r = kern::formula<f32>([](auto u) { return sqrt(((-2.0f) * log(u))); }, u);
    self.spare_ = kern::formula<f32, f32, f32>([](auto r, auto kPi, auto v) { return (r * sin(((2.0f * kPi) * v))); }, r, kPi, v);
    self.has_spare_ = true;
    return {kern::formula<f32, f32, f32>([](auto r, auto kPi, auto v) { return (r * cos(((2.0f * kPi) * v))); }, r, kPi, v)};
  }

  struct castResult { the_RayResult_ Result = the_RayResult_{false, 0.0f, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}}; };
  KERN_PURE castResult cast(Object<kern::physics::World> w = Object<kern::physics::World>{}, Vector origin = Vector{0.0, 0.0, 0.0}, Vector dir = Vector{0.0, 0.0, 0.0}, f32 max_len = 0.0f, physics_Body self = physics_Body{0}, u64 mask = 0) {
    physics_RayHit h = physics_RayHit{false, physics_Shape{0}, physics_Body{0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f};
    physics_RayHit cast_ray = physics_RayHit{false, physics_Shape{0}, physics_Body{0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f};
    the_RayResult_ r = the_RayResult_{false, 0.0f, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    cast_ray = engine_world_cast_ray(w, origin, vector_scale(dir, max_len), engine_physics_physics::QueryFilter_new(mask, 18446744073709551615u).Result, self);
    h = cast_ray;
    r = the_RayResult_{false, 0.0f, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    if (logic_not(h.hit)) {
      return {r};
    }
    r.hit = true;
    r.distance = kern::formula<f32, f32>([](auto fraction, auto max_len) { return (fraction * max_len); }, h.fraction, max_len);
    r.point = h.point;
    r.normal = h.normal;
    return {r};
  }

  // ---- what nodes give and keep ----
  Rng_nextResult n10;
  Rng_unitResult n11;
  Rng_unitResult n12;
  Rng_unitResult n13;
};
