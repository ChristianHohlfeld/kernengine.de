struct ragdoll_cpp : virtual kern::Library, virtual common, virtual engine_math_math, virtual engine_physics_physics, virtual ragdoll {
  // ---- types ----
  KERN_STRUCT(BoneDef) { Vector center = Vector{0.0, 0.0, 0.0}; Vector half = Vector{0.0, 0.0, 0.0}; Bool capsule = false; };
  KERN_STRUCT(JointDef) { Int parent = 0; Vector anchor = Vector{0.0, 0.0, 0.0}; Int kind = 0; Float lo = 0.0; Float hi = 0.0; };

  // ---- variables ----
  KERN_CONST Fixed<BoneDef, 11> kDefs = List<BoneDef>{BoneDef{Vector{0.0, 0.0, 0.98}, Vector{0.11, 0.17, 0.11}, false}, BoneDef{Vector{0.0, 0.0, 1.3}, Vector{0.14, 0.14, 0.24}, true}, BoneDef{Vector{0.0, 0.0, 1.74}, Vector{0.11, 0.11, 0.11}, true}, BoneDef{Vector{0.0, 0.225, 1.35}, Vector{0.05, 0.05, 0.17}, true}, BoneDef{Vector{0.0, -0.225, 1.35}, Vector{0.05, 0.05, 0.17}, true}, BoneDef{Vector{0.0, 0.225, 1.065}, Vector{0.045, 0.045, 0.16}, true}, BoneDef{Vector{0.0, -0.225, 1.065}, Vector{0.045, 0.045, 0.16}, true}, BoneDef{Vector{0.0, 0.1, 0.715}, Vector{0.07, 0.07, 0.24}, true}, BoneDef{Vector{0.0, -0.1, 0.715}, Vector{0.07, 0.07, 0.24}, true}, BoneDef{Vector{0.0, 0.1, 0.27}, Vector{0.06, 0.06, 0.24}, true}, BoneDef{Vector{0.0, -0.1, 0.27}, Vector{0.06, 0.06, 0.24}, true}};
  /// per-joint velocity noise
  KERN_CONST Float kNoise = 0.65;
  KERN_CONST Fixed<JointDef, 11> kJoints = List<JointDef>{JointDef{-1, Vector{0.0, 0.0, 0.0}, 0, 0.0, 0.0}, JointDef{0, Vector{0.0, 0.0, 1.08}, 0, 0.0, 0.5}, JointDef{1, Vector{0.0, 0.0, 1.6}, 0, 0.0, 0.6}, JointDef{1, Vector{0.0, 0.225, 1.5}, 0, 0.0, 1.6}, JointDef{1, Vector{0.0, -0.225, 1.5}, 0, 0.0, 1.6}, JointDef{3, Vector{0.0, 0.225, 1.2}, 1, -2.4, 0.05}, JointDef{4, Vector{0.0, -0.225, 1.2}, 1, -2.4, 0.05}, JointDef{0, Vector{0.0, 0.1, 0.94}, 0, 0.0, 1.2}, JointDef{0, Vector{0.0, -0.1, 0.94}, 0, 0.0, 1.2}, JointDef{7, Vector{0.0, 0.1, 0.49}, 1, -0.05, 2.4}, JointDef{8, Vector{0.0, -0.1, 0.49}, 1, -0.05, 2.4}};
  /// createHuman frictionTorque: 8
  KERN_CONST Float kFriction = 8.0;
  KERN_CONST Float kPi = 3.141592653589793;
  /// GETUP_MS
  KERN_CONST Float kGetup = 1.15;
  /// settledMs > 350 at < 0.06 m/s
  KERN_CONST Float kSettleSpeed = 0.06;
  /// settledMs > 350 at < 0.06 m/s
  KERN_CONST Float kSettleTime = 0.35;
  /// recover.afterMs default
  KERN_CONST Float kRecoverAfter = 2.6;

  // ---- functions ----
  struct Ragdolls_findResult { Int Result = 0; };
  KERN_PURE Ragdolls_findResult Ragdolls_find(const Ragdolls& self, Int64 key = Int64{0x0}) {
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))))); it10.next();) {
      if (logic_and(self.dolls_[n10.index].active, int64_equal(self.dolls_[n10.index].key, key))) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n10.index)))};
      }
    }
    return {-1};
  }

  struct Ragdolls_hasResult { Bool Result = false; };
  KERN_PURE Ragdolls_hasResult Ragdolls_has(const Ragdolls& self, Int64 key = Int64{0x0}) {
    return {math_compare<">=">(kern::to<Float>(Ragdolls_find(self, key).Result), 0.0)};
  }

  struct Ragdolls_bone_bodyResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  /// A bone's body (VR grab: the hand takes the doll by its chest), invalid when there is none.
  KERN_PURE Ragdolls_bone_bodyResult Ragdolls_bone_body(const Ragdolls& self, Int64 key = Int64{0x0}, Ragdolls_Bone b = Ragdolls_Bone::pelvis) {
    Int i = 0;
    i = Ragdolls_find(self, key).Result;
    return {logic_select<physics_Body>(logic_and(logic_and(math_compare<">=">(kern::to<Float>(i), 0.0), self.dolls_[i].active), logic_not(self.dolls_[i].getting_up)), self.dolls_[i].bodies[enum_index<Ragdolls_Bone>(b)], struct_make<physics_Body>(Int64{0x0}))};
  }

  struct hash_noiseResult { Float Result = 0.0; };
  KERN_PURE hash_noiseResult hash_noise(Int64 seed = Int64{0x0}, Int i = 0) {
    Int x = 0;
    x = kern::to<Int>(kern::formula([](Float a, Float i) -> Float { return kern::math::wrap32(a + kern::math::mul32(kern::math::wrap32(i + 1.0), 2654435761.0)); }, kern::to<Float>(int64_low(int64_xor(seed, int64_shift_right_unsigned(seed, 32)))), kern::to<Float>(i)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2146121005.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 15.0)); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::mul32(x, 2221713035.0); }, kern::to<Float>(x)));
    x = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::bxor(x, kern::math::shr(x, 16.0)); }, kern::to<Float>(x)));
    return {kern::formula([](Float x) -> Float { return ((kern::math::div(x, 4294967295.0) * 2.0) - 1.0); }, kern::to<Float>(x))};
  }

  struct smoothResult { Float Result = 0.0; };
  KERN_PURE smoothResult smooth(Float t = 0.0) {
    Float t_ = 0.0;
    t_ = t;
    t_ = kern::formula([](Float t_) -> Float { return (t_ < 0.0 ? 0.0 : (1.0 < t_ ? 1.0 : t_)); }, t_);
    return {kern::formula([](Float t_) -> Float { return ((t_ * t_) * (3.0 - (2.0 * t_))); }, t_)};
  }

  struct Ragdolls_rest_centerResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Ragdolls_rest_centerResult Ragdolls_rest_center(Int b = 0) {
    return {struct_break<BoneDef>(list_get<BoneDef>(kDefs, b)).center};
  }

  struct Ragdolls_rest_halfResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Ragdolls_rest_halfResult Ragdolls_rest_half(Int b = 0) {
    return {struct_break<BoneDef>(list_get<BoneDef>(kDefs, b)).half};
  }

  void Ragdolls_release(Ragdolls& self, Ragdolls_Doll& d) {
    Bool joint_valid = false;
    Bool joint_valid_2 = false;
    Bool both = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both_2 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.world_), 0.0))) {
      return;
    }
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it11.next();) {
      both = engine_physics_physics::Joint_valid(d.joints[n11.index]).Result;
      if (both) {
        joint_valid_2 = engine_world_joint_valid(self.world_, d.joints[n11.index]);
        both = joint_valid_2;
      }
      if (both) {
        engine_world_destroy_joint(self.world_, d.joints[n11.index], false);
      }
      d.joints[n11.index] = struct_make<physics_Joint>(Int64{0x0});
    }
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it12.next();) {
      both_2 = engine_physics_physics::Body_valid(d.bodies[n12.index]).Result;
      if (both_2) {
        body_valid_2 = engine_world_body_valid(self.world_, d.bodies[n12.index]);
        both_2 = body_valid_2;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, d.bodies[n12.index]);
      }
      d.bodies[n12.index] = struct_make<physics_Body>(Int64{0x0});
    }
  }

  void Ragdolls_clear(Ragdolls& self) {
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))))); it13.next();) {
      Ragdolls_release(self, self.dolls_[n13.index]);
      self.dolls_[n13.index].active = false;
    }
    list_clear(self.dolls_);
    list_clear(self.ups_);
  }

  void Ragdolls_remove(Ragdolls& self, Int64 key = Int64{0x0}) {
    Int i = 0;
    i = Ragdolls_find(self, key).Result;
    if (math_compare<"<">(kern::to<Float>(i), 0.0)) {
      return;
    }
    Ragdolls_release(self, self.dolls_[i]);
    self.dolls_[i].active = false;
  }

  struct Ragdolls_whereResult { Bool Result = false; };
  Ragdolls_whereResult Ragdolls_where(const Ragdolls& self, Vector& p, Float& yaw, Int64 key = Int64{0x0}) {
    Int i = 0;
    Vector up = Vector{0.0, 0.0, 0.0};
    i = Ragdolls_find(self, key).Result;
    if (math_compare<"<">(kern::to<Float>(i), 0.0)) {
      return {false};
    }
    p = self.dolls_[i].pose[enum_index<Ragdolls_Bone>(Ragdolls_Bone::pelvis)].position;
    up = vector_sub(self.dolls_[i].pose[enum_index<Ragdolls_Bone>(Ragdolls_Bone::chest)].position, self.dolls_[i].pose[enum_index<Ragdolls_Bone>(Ragdolls_Bone::pelvis)].position);
    yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(up).y, vector_break(up).x);
    return {true};
  }

  struct Ragdolls_spawnResult { Bool Result = false; };
  Ragdolls_spawnResult Ragdolls_spawn(Ragdolls& self, const Ragdolls_Colors& colors, Int64 key = Int64{0x0}, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector velocity = Vector{0.0, 0.0, 0.0}, Bool recover = false, Float now = 0.0) {
    Int at = 0;
    Int i = 0;
    Bool stop = false;
    Int old = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float y = 0.0;
    Int was = 0;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    BoneDef def = BoneDef{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false};
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    Float boost = 0.0;
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    JointDef j = JointDef{0, Vector{0.0, 0.0, 0.0}, 0, 0.0, 0.0};
    physics_JointSettings js = physics_JointSettings{physics_JointType::revolute, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, false, false, 0.0, 0.0, false, 0.0, 0.0, false, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, 1.0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}};
    Vector la = Vector{0.0, 0.0, 0.0};
    Vector lb = Vector{0.0, 0.0, 0.0};
    physics_Pose frame_from_axis = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose frame_from_axis_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector axis = Vector{0.0, 0.0, 0.0};
    physics_Pose frame_from_axis_3 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose frame_from_axis_4 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Joint create_joint = physics_Joint{Int64{0x0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.world_), 0.0))) {
      return {false};
    }
    Ragdolls_remove(self, key);
    at = -1;
    i = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))), 0.5)))) {
      if (logic_not(self.dolls_[i].active)) {
        at = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i)));
        stop = true;
      }
      if (logic_not(stop)) {
        i += 1;
      }
    }
    if (logic_and(math_compare<"<">(kern::to<Float>(at), 0.0), math_compare<">">(kern::formula([](Float a, Float max_) -> Float { return ((a < 0.0) != (max_ < 0.0) ? (max_ < 0.0) : (a < max_)); }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_)), kern::to<Float>(self.max_)), 0.5))) {
      list_add(self.dolls_, struct_make<Ragdolls_Doll>(false, false, false, Int64{0x0}, list_make<physics_Body, "11">(physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}), list_make<physics_Joint, "11">(physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}), list_make<physics_Pose, "11">(physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}), list_make<physics_Pose, "11">(physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}), list_make<physics_Pose, "11">(physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}), struct_make<Ragdolls_Colors>(Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false), 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 0));
      at = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::wrap32s(a) - 1.0); }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))));
    }
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      old = 0;
      for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))))); it14.next();) {
        if (math_compare<"<">(self.dolls_[n14.index].born, self.dolls_[old].born)) {
          old = n14.index;
        }
      }
      if (self.dolls_[old].recover) {
        p = Vector{0.0, 0.0, 0.0};
        y = 0.0;
        n15 = Ragdolls_where(self, p, y, self.dolls_[old].key);
        list_add(self.ups_, struct_make<Ragdolls_Up>(self.dolls_[old].key, vector_make(vector_break(p).x, vector_break(p).y, vector_break(feet).z), y));
      }
      Ragdolls_release(self, self.dolls_[old]);
      at = kern::to<Int>(kern::formula([](Float old) -> Float { return kern::math::wrap32s(old); }, kern::to<Float>(old)));
    }
    self.dolls_[at] = struct_make<Ragdolls_Doll>(false, false, false, Int64{0x0}, list_make<physics_Body, "11">(physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}), list_make<physics_Joint, "11">(physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}, physics_Joint{Int64{0x0}}), list_make<physics_Pose, "11">(physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}), list_make<physics_Pose, "11">(physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}), list_make<physics_Pose, "11">(physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}), struct_make<Ragdolls_Colors>(Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false), 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 0);
    self.dolls_[at].active = true;
    self.dolls_[at].key = key;
    self.dolls_[at].recover = recover;
    self.dolls_[at].colors = colors;
    self.dolls_[at].born = now;
    was = self.group_;
    self.group_ = kern::to<Int>(kern::formula([](Float group_) -> Float { return (group_ + 1.0); }, kern::to<Float>(self.group_)));
    self.dolls_[at].group = kern::to<Int>(kern::formula([](Float was) -> Float { return (-(1.0 + kern::math::mod(was, 30000.0))); }, kern::to<Float>(was)));
    self.spawned_ = kern::to<Int>(kern::formula([](Float spawned_) -> Float { return kern::math::wrap32(spawned_ + 1.0); }, kern::to<Float>(self.spawned_)));
    q = common::yaw_quat(yaw).Result;
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it16.next();) {
      def = list_get<BoneDef>(kDefs, n16.index);
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs.type = physics_BodyType::dynamic;
      bs.position = vector_add(feet, engine_math_math::Quat_rotate(q, def.center).Result);
      bs.rotation = q;
      boost = kern::formula([](Float index, Float a, Float a_2, Float a_3, Float a_4, Float a_5, Float a_6) -> Float { return (((index == a) || (index == a_2)) ? 1.15 : (((((index == a_3) || (index == a_4)) || (index == a_5)) || (index == a_6)) ? 1.28 : 1.0)); }, kern::to<Float>(n16.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::chest)), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::head)), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::lower_arm_l)), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::lower_arm_r)), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::shin_l)), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::shin_r)));
      bs.linear_velocity = vector_make(kern::formula([](Float x, Float boost, Float the_result, Float kNoise) -> Float { return ((x * boost) + (the_result * kNoise)); }, vector_break(velocity).x, boost, hash_noise(key, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mul32(index, 3.0); }, kern::to<Float>(n16.index)))).Result, kNoise), kern::formula([](Float y, Float boost, Float the_result, Float kNoise) -> Float { return ((y * boost) + (the_result * kNoise)); }, vector_break(velocity).y, boost, hash_noise(key, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(kern::math::mul32(index, 3.0) + 1.0); }, kern::to<Float>(n16.index)))).Result, kNoise), kern::formula([](Float z, Float boost, Float the_result) -> Float { return (((z * boost) + 1.1) + (kern::math::abs(the_result) * 0.9)); }, vector_break(velocity).z, boost, hash_noise(key, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(kern::math::mul32(index, 3.0) + 2.0); }, kern::to<Float>(n16.index)))).Result));
      bs.angular_velocity = vector_make(kern::formula([](Float the_result) -> Float { return (the_result * 2.0); }, hash_noise(key, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(40.0 + index); }, kern::to<Float>(n16.index)))).Result), kern::formula([](Float the_result) -> Float { return (the_result * 2.0); }, hash_noise(key, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(60.0 + index); }, kern::to<Float>(n16.index)))).Result), hash_noise(key, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(80.0 + index); }, kern::to<Float>(n16.index)))).Result);
      bs.linear_damping = 0.05;
      bs.angular_damping = 0.4;
      engine_world_create_body(n17, self.world_, bs);
      create_body = n17.result;
      self.dolls_[at].bodies[n16.index] = create_body;
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      if (math_compare<"==">(kern::to<Float>(n16.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::head)))) {
        ss.kind = physics_ShapeKind::sphere;
        ss.radius = vector_break(def.half).x;
      } else if (def.capsule) {
        ss.kind = physics_ShapeKind::capsule;
        ss.radius = vector_break(def.half).x;
        ss.length = kern::formula([](Float z, Float x) -> Float { return (2.0 * (z - x)); }, vector_break(def.half).z, vector_break(def.half).x);
      } else {
        ss.kind = physics_ShapeKind::box;
        ss.size = vector_scale(def.half, 2.0);
      }
      ss.material.density = 1000.0;
      ss.material.friction = 0.72;
      ss.material.restitution = 0.05;
      ss.material.group_index = self.dolls_[at].group;
      ss.material.category_bits = Int64{0x1};
      engine_world_create_shape(n18, self.world_, self.dolls_[at].bodies[n16.index], ss);
      create_shape = n18.result;
    }
    for (auto it19 = flow_for(n19, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it19.next();) {
      j = list_get<JointDef>(kJoints, n19.index);
      js = physics_JointSettings{physics_JointType::revolute, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, false, false, 0.0, 0.0, false, 0.0, 0.0, false, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, 1.0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}};
      js.body_a = self.dolls_[at].bodies[j.parent];
      js.body_b = self.dolls_[at].bodies[n19.index];
      la = vector_sub(j.anchor, struct_break<BoneDef>(list_get<BoneDef>(kDefs, j.parent)).center);
      lb = vector_sub(j.anchor, struct_break<BoneDef>(list_get<BoneDef>(kDefs, n19.index)).center);
      if (math_compare<"==">(kern::to<Float>(j.kind), 1.0)) {
        js.type = physics_JointType::revolute;
        frame_from_axis = engine_physics_frame_from_axis(la, Vector{0.0, 1.0, 0.0}, false);
        js.frame_a = frame_from_axis;
        frame_from_axis_2 = engine_physics_frame_from_axis(lb, Vector{0.0, 1.0, 0.0}, false);
        js.frame_b = frame_from_axis_2;
        js.enable_limit = true;
        js.lower = j.lo;
        js.upper = j.hi;
        js.enable_motor = true;
        js.motor_speed = 0.0;
        js.max_motor_torque = kern::formula([](Float kFriction) -> Float { return (kFriction * 0.5); }, kFriction);
      } else {
        js.type = physics_JointType::spherical;
        axis = logic_select<Vector>(logic_or(math_compare<"==">(kern::to<Float>(n19.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::chest))), math_compare<"==">(kern::to<Float>(n19.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::head)))), Vector{0.0, 0.0, 1.0}, Vector{0.0, 0.0, -1.0});
        frame_from_axis_3 = engine_physics_frame_from_axis(la, axis, false);
        js.frame_a = frame_from_axis_3;
        frame_from_axis_4 = engine_physics_frame_from_axis(lb, axis, false);
        js.frame_b = frame_from_axis_4;
        js.enable_limit = true;
        js.upper = j.hi;
        js.enable_motor = true;
        js.max_motor_torque = kern::formula([](Float kFriction) -> Float { return (kFriction * 0.5); }, kFriction);
      }
      engine_world_create_joint(n20, self.world_, js);
      create_joint = n20.result;
      self.dolls_[at].joints[n19.index] = create_joint;
    }
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it21.next();) {
      pose = engine_world_pose(self.world_, self.dolls_[at].bodies[n21.index]);
      self.dolls_[at].pose[n21.index] = pose;
    }
    return {true};
  }

  void Ragdolls_begin_getup(Ragdolls& self, Ragdolls_Doll& d, Float now = 0.0) {
    Vector left = Vector{0.0, 0.0, 0.0};
    Float yaw = 0.0;
    Vector pp = Vector{0.0, 0.0, 0.0};
    Float low = 0.0;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it22.next();) {
      d.from[n22.index] = d.pose[n22.index];
    }
    left = engine_math_math::Quat_rotate(d.pose[enum_index<Ragdolls_Bone>(Ragdolls_Bone::pelvis)].rotation, Vector{0.0, 1.0, 0.0}).Result;
    yaw = kern::formula([](Float y, Float x, Float kPi) -> Float { return (kern::math::atan2(y, x) - kern::math::div(kPi, 2.0)); }, vector_break(left).y, vector_break(left).x, kPi);
    if (logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, yaw), 0.0))) {
      yaw = 0.0;
    }
    pp = d.pose[enum_index<Ragdolls_Bone>(Ragdolls_Bone::pelvis)].position;
    d.stand_yaw = yaw;
    d.stand_feet = vector_make(vector_break(pp).x, vector_break(pp).y, kern::formula([](Float z, Float z_2) -> Float { return ((z - 0.12) - (z_2 * 0.0)); }, vector_break(pp).z, vector_break(struct_break<BoneDef>(list_get<BoneDef>(kDefs, enum_index<Ragdolls_Bone>(Ragdolls_Bone::pelvis))).center).z));
    low = 1000000000.0;
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it23.next();) {
      low = kern::formula([](Float low, Float z, Float x) -> Float { return kern::math::min(low, (z - x)); }, low, vector_break(d.pose[n23.index].position).z, vector_break(struct_break<BoneDef>(list_get<BoneDef>(kDefs, n23.index)).half).x);
    }
    d.stand_feet = vector_make(vector_break(d.stand_feet).x, vector_break(d.stand_feet).y, low);
    q = common::yaw_quat(yaw).Result;
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it24.next();) {
      d.stand[n24.index] = struct_make<physics_Pose>(vector_add(d.stand_feet, engine_math_math::Quat_rotate(q, struct_break<BoneDef>(list_get<BoneDef>(kDefs, n24.index)).center).Result), q);
    }
    Ragdolls_release(self, d);
    d.getting_up = true;
    d.getup_at = now;
  }

  void Ragdolls_update(Ragdolls& self, Float dt = 0.0, Float now = 0.0) {
    Bool skip = false;
    Float t = 0.0;
    Float k = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float vmax = 0.0;
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool awake = false;
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))))); it25.next();) {
      skip = false;
      if (logic_not(self.dolls_[n25.index].active)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (self.dolls_[n25.index].getting_up) {
          t = kern::formula([](Float now, Float a, Float kGetup) -> Float { return kern::math::div((now - a), kGetup); }, now, self.dolls_[n25.index].getup_at, kGetup);
          k = smooth(t).Result;
          for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it26.next();) {
            p = vector_add(self.dolls_[n25.index].from[n26.index].position, vector_scale(vector_sub(self.dolls_[n25.index].stand[n26.index].position, self.dolls_[n25.index].from[n26.index].position), k));
            if (math_compare<"<">(t, 0.45)) {
              p = vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float z, Float z_2, Float z_3, Float the_result) -> Float { return (z + (((z_2 + (z_3 * 0.55)) - z) * the_result)); }, vector_break(self.dolls_[n25.index].from[n26.index].position).z, vector_break(self.dolls_[n25.index].stand_feet).z, vector_break(struct_break<BoneDef>(list_get<BoneDef>(kDefs, n26.index)).center).z, smooth(kern::formula([](Float t) -> Float { return kern::math::div(t, 0.45); }, t)).Result));
            } else {
              p = vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float z, Float z_2, Float the_result) -> Float { return (z + (z_2 * (0.55 + (0.45 * the_result)))); }, vector_break(self.dolls_[n25.index].stand_feet).z, vector_break(struct_break<BoneDef>(list_get<BoneDef>(kDefs, n26.index)).center).z, smooth(kern::formula([](Float t) -> Float { return kern::math::div((t - 0.45), 0.55); }, t)).Result));
            }
            self.dolls_[n25.index].pose[n26.index] = struct_make<physics_Pose>(p, common::slerp(self.dolls_[n25.index].from[n26.index].rotation, self.dolls_[n25.index].stand[n26.index].rotation, k).Result);
          }
          if (math_compare<">=">(t, 1.0)) {
            list_add(self.ups_, struct_make<Ragdolls_Up>(self.dolls_[n25.index].key, self.dolls_[n25.index].stand_feet, self.dolls_[n25.index].stand_yaw));
            self.dolls_[n25.index].active = false;
          }
          skip = true;
        }
        if (logic_not(skip)) {
          vmax = 0.0;
          for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it27.next();) {
            pose = engine_world_pose(self.world_, self.dolls_[n25.index].bodies[n27.index]);
            self.dolls_[n25.index].pose[n27.index] = pose;
            awake = engine_world_awake(self.world_, self.dolls_[n25.index].bodies[n27.index]);
            if (awake) {
              linear_velocity = engine_world_linear_velocity(self.world_, self.dolls_[n25.index].bodies[n27.index]);
              vmax = kern::formula([](Float vmax, Float a) -> Float { return kern::math::max(vmax, a); }, vmax, vector_length(linear_velocity));
            }
          }
          self.dolls_[n25.index].settled = kern::formula([](Float vmax, Float kSettleSpeed, Float a, Float dt) -> Float { return ((vmax < kSettleSpeed) ? (a + dt) : 0.0); }, vmax, kSettleSpeed, self.dolls_[n25.index].settled, dt);
          if (self.dolls_[n25.index].recover) {
            if (logic_or(math_compare<">">(self.dolls_[n25.index].settled, kSettleTime), math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, self.dolls_[n25.index].born), kern::formula([](Float kRecoverAfter) -> Float { return (kRecoverAfter + 2.0); }, kRecoverAfter)))) {
              Ragdolls_begin_getup(self, self.dolls_[n25.index], now);
            }
          }
        }
      }
    }
  }

  void Ragdolls_blast(Ragdolls& self, Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float strength = 0.0) {
    Bool skip = false;
    Bool skip_2 = false;
    Bool body_valid = false;
    physics_BodyType body_type = physics_BodyType::fixed;
    physics_BodyType body_type_2 = physics_BodyType::fixed;
    Bool either = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector dir = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float k = 0.0;
    Float mass = 0.0;
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.world_), 0.0))) {
      return;
    }
    for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))))); it28.next();) {
      skip = false;
      if (logic_or(logic_not(self.dolls_[n28.index].active), self.dolls_[n28.index].getting_up)) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it29.next();) {
          skip_2 = false;
          body_valid = engine_world_body_valid(self.world_, self.dolls_[n28.index].bodies[n29.index]);
          either = logic_not(body_valid);
          if (logic_not(either)) {
            body_type_2 = engine_world_body_type(self.world_, self.dolls_[n28.index].bodies[n29.index]);
            either = logic_not(enum_equal<physics_BodyType>(body_type_2, physics_BodyType::dynamic));
          }
          if (either) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            pose = engine_world_pose(self.world_, self.dolls_[n28.index].bodies[n29.index]);
            p = struct_break<physics_Pose>(pose).position;
            dir = vector_sub(p, at);
            l = vector_length(dir);
            if (math_compare<">">(l, radius)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              mass = engine_world_mass(self.world_, self.dolls_[n28.index].bodies[n29.index]);
              k = kern::formula([](Float strength, Float l, Float radius, Float mass) -> Float { return ((strength * (1.0 - kern::math::div(l, radius))) * mass); }, strength, l, radius, mass);
              engine_world_apply_linear_impulse_to_center(self.world_, self.dolls_[n28.index].bodies[n29.index], vector_add(vector_scale(logic_select<Vector>(math_compare<">">(l, 0.001), vector_scale(dir, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l)), Vector{0.0, 0.0, 1.0}), k), vector_make(0.0, 0.0, kern::formula([](Float k) -> Float { return (k * 0.5); }, k))));
            }
          }
        }
      }
    }
  }

  struct Ragdolls_physicalResult { Bool Result = false; };
  KERN_PURE Ragdolls_physicalResult Ragdolls_physical(const Ragdolls& self, Int64 key = Int64{0x0}) {
    Int i = 0;
    i = Ragdolls_find(self, key).Result;
    return {logic_and(math_compare<">=">(kern::to<Float>(i), 0.0), logic_not(self.dolls_[i].getting_up))};
  }

  struct Ragdolls_activeResult { Int Result = 0; };
  KERN_PURE Ragdolls_activeResult Ragdolls_active(const Ragdolls& self) {
    Int n = 0;
    n = 0;
    for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))))); it30.next();) {
      n += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(self.dolls_[n30.index].active)));
    }
    return {n};
  }

  void Ragdolls_draw(const Ragdolls& self, List<Ragdolls_Draw>& out) {
    Bool skip = false;
    Vector h = Vector{0.0, 0.0, 0.0};
    Vector tint = Vector{0.0, 0.0, 0.0};
    for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Ragdolls_Doll>(self.dolls_))))); it31.next();) {
      skip = false;
      if (logic_not(self.dolls_[n31.index].active)) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it32.next();) {
          h = struct_break<BoneDef>(list_get<BoneDef>(kDefs, n32.index)).half;
          tint = logic_select<Vector>(logic_or(logic_or(math_compare<"==">(kern::to<Float>(n32.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::head))), math_compare<"==">(kern::to<Float>(n32.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::lower_arm_l)))), math_compare<"==">(kern::to<Float>(n32.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::lower_arm_r)))), self.dolls_[n31.index].colors.skin, logic_select<Vector>(logic_or(logic_or(math_compare<"==">(kern::to<Float>(n32.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::chest))), math_compare<"==">(kern::to<Float>(n32.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::upper_arm_l)))), math_compare<"==">(kern::to<Float>(n32.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::upper_arm_r)))), self.dolls_[n31.index].colors.shirt, self.dolls_[n31.index].colors.pants));
          list_add(out, struct_make<Ragdolls_Draw>(engine_math_math::Mat4_trs(self.dolls_[n31.index].pose[n32.index].position, self.dolls_[n31.index].pose[n32.index].rotation, vector_scale(h, 2.0)).Result, tint, math_compare<"==">(kern::to<Float>(n32.index), kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::head))), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band(index, 255.0); }, kern::to<Float>(n32.index))), n31.index, engine_math_math::Mat4_trs(self.dolls_[n31.index].pose[n32.index].position, self.dolls_[n31.index].pose[n32.index].rotation, Vector{1.0, 1.0, 1.0}).Result));
        }
      }
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  Ragdolls_whereResult n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n17;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct { physics_Joint result = physics_Joint{Int64{0x0}}; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
};
