struct car_cpp : virtual kern::Library, virtual car, virtual common, virtual engine_math_math, virtual engine_physics_physics {
  // ---- variables ----
  KERN_CONST Float kKappaPeak = 0.1;
  KERN_CONST Float kAlphaPeak = 0.1139356083016455;
  KERN_CONST Float kSlideGrip = 0.78;

  // ---- functions ----
  /// Combined-slip tyre force (longitudinal, lateral) on the friction ellipse.
  void tyre(Fixed<Float, 2>& out, Float kappa = 0.0, Float tan_a = 0.0, Float muN = 0.0) {
    Float sx = 0.0;
    Float sy = 0.0;
    Float s = 0.0;
    Float g = 0.0;
    Float F = 0.0;
    sx = kern::formula([](Float kappa, Float kKappaPeak) -> Float { return kern::math::div(kappa, kKappaPeak); }, kappa, kKappaPeak);
    sy = kern::formula([](Float tan_a, Float kAlphaPeak) -> Float { return kern::math::div(tan_a, kAlphaPeak); }, tan_a, kAlphaPeak);
    s = kern::formula([](Float sx, Float sy) -> Float { return kern::math::sqrt((sx * sx) + (sy * sy)); }, sx, sy);
    if (logic_or(math_compare<"<">(s, 1e-06), logic_not(math_compare<">">(muN, 0.0)))) {
      out[0] = 0.0;
      out[1] = 0.0;
      return;
    }
    g = kern::formula([](Float s, Float kSlideGrip) -> Float { return ((s < 1.0) ? (s * (2.0 - s)) : (1.0 - ((1.0 - kSlideGrip) * kern::math::min(1.0, kern::math::div((s - 1.0), 2.5))))); }, s, kSlideGrip);
    F = kern::formula([](Float muN, Float g) -> Float { return (muN * g); }, muN, g);
    out[0] = kern::formula([](Float f, Float sx, Float s) -> Float { return kern::math::div((f * sx), s); }, F, sx, s);
    out[1] = kern::formula([](Float f, Float sy, Float s) -> Float { return kern::math::div(((-f) * sy), s); }, F, sy, s);
  }

  void Car_reapply(const Car& self, Int world = 0, physics_Body body = physics_Body{Int64{0x0}}) {
    Int world_ = 0;
    BodyState st = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector f = Vector{0.0, 0.0, 0.0};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    world_ = world;
    if (logic_or(math_compare<"!=">(kern::to<Float>(self.rig_world_), kern::to<Float>(world_)), logic_not(engine_physics_physics::Body_equal(self.rig_chassis_, body).Result))) {
      return;
    }
    st = common::read_body(world_, body).Result;
    if (logic_and(self.has_last_, math_compare<">">(vector_length(vector_sub(st.position, self.last_pos_)), kern::formula([](Float a) -> Float { return ((kern::math::div(a, 60.0) * 2.0) + 0.6); }, vector_length(st.velocity))))) {
      return;
    }
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(self.n_applied_)))); it10.next();) {
      f = engine_math_math::Quat_rotate(st.rotation, self.applied_[n10.index].f).Result;
      if (math_compare<"<">(kern::to<Float>(self.applied_[n10.index].on), 0.0)) {
        engine_world_apply_force_to_center(world_, body, f);
      } else {
        both = engine_physics_physics::Body_valid(self.rig_[self.applied_[n10.index].on].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(world_, self.rig_[self.applied_[n10.index].on].body);
          both = body_valid_2;
        }
        if (both) {
          engine_world_apply_force(world_, self.rig_[self.applied_[n10.index].on].body, f, vector_add(st.position, engine_math_math::Quat_rotate(st.rotation, self.applied_[n10.index].p).Result));
        }
      }
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
