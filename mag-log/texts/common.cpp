struct common : virtual kern::Library, virtual engine_math_math, virtual engine_physics_physics {
  // ---- types ----
  KERN_STRUCT(Rng) { U32 s = 2654435769; Float spare_ = 0.0; Bool has_spare_ = false; };
  KERN_STRUCT(the_RayResult_) { Bool hit = false; Float distance = 0.0; Vector point = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 1.0}; };
  KERN_STRUCT(BodyState) { Vector position = Vector{0.0, 0.0, 0.0}; Vector velocity = Vector{0.0, 0.0, 0.0}; Vector angular = Vector{0.0, 0.0, 0.0}; Quat rotation = Quat{0.0, 0.0, 0.0, 1.0}; };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct clampfResult { Float Result = 0.0; };
  KERN_PURE clampfResult clampf(Float v = 0.0, Float lo = 0.0, Float hi = 0.0) {
    return {kern::formula([](Float v, Float lo, Float hi) -> Float { return ((v < lo) ? lo : ((v > hi) ? hi : v)); }, v, lo, hi)};
  }

  struct sign0Result { Float Result = 0.0; };
  KERN_PURE sign0Result sign0(Float v = 0.0) {
    return {kern::formula([](Float v) -> Float { return ((v > 0.0) ? 1.0 : ((v < 0.0) ? (-1.0) : 0.0)); }, v)};
  }

  struct rate_limitResult { Float Result = 0.0; };
  KERN_PURE rate_limitResult rate_limit(Float cur = 0.0, Float target = 0.0, Float max_step = 0.0) {
    return {kern::formula([](Float cur, Float a) -> Float { return (cur + a); }, cur, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float target, Float cur) -> Float { return (target - cur); }, target, cur), kern::formula([](Float max_step) -> Float { return (-max_step); }, max_step), max_step))};
  }

  struct damp_alphaResult { Float Result = 0.0; };
  /// Exponential approach: the share of the remaining distance covered in dt at `rate` per second.
  KERN_PURE damp_alphaResult damp_alpha(Float rate = 0.0, Float dt = 0.0) {
    return {kern::formula([](Float rate, Float dt) -> Float { return (((rate > 0.0) && (dt > 0.0)) ? (1.0 - kern::math::exp(((-rate) * dt))) : 0.0); }, rate, dt)};
  }

  struct wrap_piResult { Float Result = 0.0; };
  KERN_PURE wrap_piResult wrap_pi(Float a = 0.0) {
    Float a_ = 0.0;
    a_ = a;
    while (loop(math_compare<">">(a_, kPi))) {
      a_ = kern::formula([](Float a_, Float kPi) -> Float { return (a_ - (2.0 * kPi)); }, a_, kPi);
    }
    while (loop(math_compare<"<">(a_, kern::formula([](Float kPi) -> Float { return (-kPi); }, kPi)))) {
      a_ += kern::formula([](Float kPi) -> Float { return (2.0 * kPi); }, kPi);
    }
    return {a_};
  }

  struct conjResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE conjResult conj(Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    return {struct_make<Quat>(kern::formula([](Float x) -> Float { return (-x); }, q.x), kern::formula([](Float y) -> Float { return (-y); }, q.y), kern::formula([](Float z) -> Float { return (-z); }, q.z), q.w)};
  }

  struct to_localResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE to_localResult to_local(Quat q = Quat{0.0, 0.0, 0.0, 1.0}, Vector v = Vector{0.0, 0.0, 0.0}) {
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    a = conj(q).Result;
    return {engine_math_math::Quat_rotate(a, v).Result};
  }

  struct qnormalizeResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE qnormalizeResult qnormalize(Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    Float l = 0.0;
    l = kern::formula([](Float x, Float y, Float z, Float w) -> Float { return kern::math::sqrt(((((x * x) + (y * y)) + (z * z)) + (w * w))); }, q.x, q.y, q.z, q.w);
    return {logic_select<Quat>(math_compare<">">(l, 1e-09), struct_make<Quat>(kern::formula([](Float x, Float l) -> Float { return kern::math::div(x, l); }, q.x, l), kern::formula([](Float y, Float l) -> Float { return kern::math::div(y, l); }, q.y, l), kern::formula([](Float z, Float l) -> Float { return kern::math::div(z, l); }, q.z, l), kern::formula([](Float w, Float l) -> Float { return kern::math::div(w, l); }, q.w, l)), struct_make<Quat>(0.0, 0.0, 0.0, 1.0))};
  }

  struct yaw_quatResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE yaw_quatResult yaw_quat(Float yaw = 0.0) {
    return {engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, yaw).Result};
  }

  struct heading_ofResult { Float Result = 0.0; };
  /// Heading (radians, CCW from +X) of a body axis projected on the ground plane.
  KERN_PURE heading_ofResult heading_of(Vector axis = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(axis).y, vector_break(axis).x)};
  }

  struct slerpResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE slerpResult slerp(Quat a = Quat{0.0, 0.0, 0.0, 1.0}, Quat b = Quat{0.0, 0.0, 0.0, 1.0}, Float t = 0.0) {
    Quat b_ = Quat{0.0, 0.0, 0.0, 1.0};
    Float d = 0.0;
    Float th = 0.0;
    Float s = 0.0;
    Float wa = 0.0;
    Float wb = 0.0;
    b_ = b;
    d = kern::formula([](Float x, Float x_2, Float y, Float y_2, Float z, Float z_2, Float w, Float w_2) -> Float { return ((((x * x_2) + (y * y_2)) + (z * z_2)) + (w * w_2)); }, a.x, b_.x, a.y, b_.y, a.z, b_.z, a.w, b_.w);
    if (math_compare<"<">(d, 0.0)) {
      b_ = struct_make<Quat>(kern::formula([](Float x) -> Float { return (-x); }, b_.x), kern::formula([](Float y) -> Float { return (-y); }, b_.y), kern::formula([](Float z) -> Float { return (-z); }, b_.z), kern::formula([](Float w) -> Float { return (-w); }, b_.w));
      d = kern::formula([](Float d) -> Float { return (-d); }, d);
    }
    if (math_compare<">">(d, 0.9995)) {
      return {qnormalize(struct_make<Quat>(kern::formula([](Float x, Float x_2, Float t) -> Float { return (x + ((x_2 - x) * t)); }, a.x, b_.x, t), kern::formula([](Float y, Float y_2, Float t) -> Float { return (y + ((y_2 - y) * t)); }, a.y, b_.y, t), kern::formula([](Float z, Float z_2, Float t) -> Float { return (z + ((z_2 - z) * t)); }, a.z, b_.z, t), kern::formula([](Float w, Float w_2, Float t) -> Float { return (w + ((w_2 - w) * t)); }, a.w, b_.w, t))).Result};
    }
    th = kern::formula([](Float d) -> Float { return kern::math::acos(d); }, d);
    s = kern::formula([](Float th) -> Float { return kern::math::sin(th); }, th);
    wa = kern::formula([](Float t, Float th, Float s) -> Float { return kern::math::div(kern::math::sin(((1.0 - t) * th)), s); }, t, th, s);
    wb = kern::formula([](Float t, Float th, Float s) -> Float { return kern::math::div(kern::math::sin((t * th)), s); }, t, th, s);
    return {struct_make<Quat>(kern::formula([](Float x, Float wa, Float x_2, Float wb) -> Float { return ((x * wa) + (x_2 * wb)); }, a.x, wa, b_.x, wb), kern::formula([](Float y, Float wa, Float y_2, Float wb) -> Float { return ((y * wa) + (y_2 * wb)); }, a.y, wa, b_.y, wb), kern::formula([](Float z, Float wa, Float z_2, Float wb) -> Float { return ((z * wa) + (z_2 * wb)); }, a.z, wa, b_.z, wb), kern::formula([](Float w, Float wa, Float w_2, Float wb) -> Float { return ((w * wa) + (w_2 * wb)); }, a.w, wa, b_.w, wb))};
  }

  struct read_bodyResult { BodyState Result = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE read_bodyResult read_body(Int w = 0, physics_Body b = physics_Body{Int64{0x0}}) {
    physics_Pose p = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    pose = engine_world_pose(w, b);
    p = pose;
    linear_velocity = engine_world_linear_velocity(w, b);
    angular_velocity = engine_world_angular_velocity(w, b);
    return {struct_make<BodyState>(p.position, linear_velocity, angular_velocity, p.rotation)};
  }

  struct Rng_seededResult { Rng Result = Rng{2654435769, 0.0, false}; };
  /// The generator for a seed that may be 0 (0: the default seed).
  KERN_PURE Rng_seededResult Rng_seeded(U32 seed = 0) {
    return {struct_make<Rng>(kern::to<Int>(kern::formula([](Float seed) -> Float { return ((seed != 0.0) ? seed : 2654435769.0); }, kern::to<Float>(seed))), 0.0, false)};
  }

  struct Rng_nextResult { U32 Result = 0; };
  Rng_nextResult Rng_next(Rng& self) {
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 13.0))); }, kern::to<Float>(self.s)));
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::shr(s, 17.0)); }, kern::to<Float>(self.s)));
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 5.0))); }, kern::to<Float>(self.s)));
    return {self.s};
  }

  struct Rng_unitResult { Float Result = 0.0; };
  Rng_unitResult Rng_unit(Rng& self) {
    n10 = Rng_next(self);
    return {kern::formula([](Float the_result) -> Float { return (kern::math::shr(the_result, 8.0) * kern::math::div(1.0, 16777216.0)); }, kern::to<Float>(n10.Result))};
  }

  struct Rng_rangeResult { Float Result = 0.0; };
  Rng_rangeResult Rng_range(Rng& self, Float a = 0.0, Float b = 0.0) {
    n11 = Rng_unit(self);
    return {kern::formula([](Float a, Float b, Float the_result) -> Float { return (a + ((b - a) * the_result)); }, a, b, n11.Result)};
  }

  struct Rng_gaussianResult { Float Result = 0.0; };
  /// Standard normal (Box-Muller), as SpawnCity's sensor noise.
  Rng_gaussianResult Rng_gaussian(Rng& self) {
    Float u = 0.0;
    Float v = 0.0;
    Float r = 0.0;
    if (self.has_spare_) {
      self.has_spare_ = false;
      return {self.spare_};
    }
    n12 = Rng_unit(self);
    u = kern::formula([](Float the_result) -> Float { return kern::math::max(1e-12, the_result); }, n12.Result);
    n13 = Rng_unit(self);
    v = n13.Result;
    r = kern::formula([](Float u) -> Float { return kern::math::sqrt(((-2.0) * kern::math::log(u))); }, u);
    self.spare_ = kern::formula([](Float r, Float kPi, Float v) -> Float { return (r * kern::math::sin(((2.0 * kPi) * v))); }, r, kPi, v);
    self.has_spare_ = true;
    return {kern::formula([](Float r, Float kPi, Float v) -> Float { return (r * kern::math::cos(((2.0 * kPi) * v))); }, r, kPi, v)};
  }

  struct castResult { the_RayResult_ Result = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}}; };
  KERN_PURE castResult cast(Int w = 0, Vector origin = Vector{0.0, 0.0, 0.0}, Vector dir = Vector{0.0, 0.0, 0.0}, Float max_len = 0.0, physics_Body self = physics_Body{Int64{0x0}}, Int64 mask = Int64{0x0}) {
    physics_RayHit h = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    physics_RayHit cast_ray = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    cast_ray = engine_world_cast_ray(w, origin, vector_scale(dir, max_len), engine_physics_physics::QueryFilter_new(mask, Int64{0xffffffffffffffff}).Result, self);
    h = cast_ray;
    r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    if (logic_not(h.hit)) {
      return {r};
    }
    r.hit = true;
    r.distance = kern::formula([](Float fraction, Float max_len) -> Float { return (fraction * max_len); }, h.fraction, max_len);
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
