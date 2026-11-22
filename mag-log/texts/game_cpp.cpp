struct game_cpp : virtual kern::Library, virtual engine_game_level, virtual game, virtual rigid_spec {
  // ---- variables ----
  KERN_CONST Fixed<Vector, 10> c = List<Vector>{Vector{0.62, 0.05, 0.04}, Vector{0.08, 0.16, 0.42}, Vector{0.85, 0.85, 0.86}, Vector{0.06, 0.06, 0.07}, Vector{0.45, 0.47, 0.5}, Vector{0.12, 0.32, 0.18}, Vector{0.9, 0.62, 0.08}, Vector{0.3, 0.5, 0.75}, Vector{0.55, 0.36, 0.22}, Vector{0.7, 0.7, 0.66}};

  // ---- functions ----
  struct car_colorResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE car_colorResult car_color(U32 i = 0) {
    return {list_get<Vector>(c, kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mod(i, 10.0); }, kern::to<Float>(i))))};
  }

  struct vehicle_actorResult { game_Actor Result = game_Actor{0, "", 0, "", false, false, game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}}, List<Text>{}, List<game_MeshComponent>{}, List<game_LightComponent>{}, List<game_BodyComponent>{}, List<game_ColliderComponent>{}, List<game_JointComponent>{}, List<game_ControllerComponent>{}, List<game_PickupComponent>{}, List<game_TriggerComponent>{}, List<game_RotatorComponent>{}, List<game_MoverComponent>{}, List<game_CityComponent>{}, List<game_AudioComponent>{}, List<game_WeaponComponent>{}, List<game_HealthComponent>{}, List<logic_Graph>{}}; };
  KERN_PURE vehicle_actorResult vehicle_actor(const RigidSpec& spec, Text name = "", Vector pos = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    game_Actor a = game_Actor{0, "", 0, "", false, false, game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}}, List<Text>{}, List<game_MeshComponent>{}, List<game_LightComponent>{}, List<game_BodyComponent>{}, List<game_ColliderComponent>{}, List<game_JointComponent>{}, List<game_ControllerComponent>{}, List<game_PickupComponent>{}, List<game_TriggerComponent>{}, List<game_RotatorComponent>{}, List<game_MoverComponent>{}, List<game_CityComponent>{}, List<game_AudioComponent>{}, List<game_WeaponComponent>{}, List<game_HealthComponent>{}, List<logic_Graph>{}};
    game_BodyComponent b = game_BodyComponent{game_Motion::dynamic, 0.0, 0.0, 0.05, 1.0, false, true, true, List<Bool>{false, false, false}, List<Bool>{false, false, false}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, "builtin:impact", 2.5, Vector{0.0, 0.0, 0.0}, List<Float>{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
    game_ColliderComponent cc = game_ColliderComponent{game_ColliderShape::automatic, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 1000.0, 0.6, 0.0, 0.0, "", false, 1, 4294967295, 0, "auto"};
    Vector quat_to_euler = Vector{0.0, 0.0, 0.0};
    a = game_Actor{0, "", 0, "", false, false, game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}}, List<Text>{}, List<game_MeshComponent>{}, List<game_LightComponent>{}, List<game_BodyComponent>{}, List<game_ColliderComponent>{}, List<game_JointComponent>{}, List<game_ControllerComponent>{}, List<game_PickupComponent>{}, List<game_TriggerComponent>{}, List<game_RotatorComponent>{}, List<game_MoverComponent>{}, List<game_CityComponent>{}, List<game_AudioComponent>{}, List<game_WeaponComponent>{}, List<game_HealthComponent>{}, List<logic_Graph>{}};
    a.name = name;
    a.transform.position = pos;
    a.transform.rotation = vector_make(0.0, 0.0, yaw_deg);
    b = game_BodyComponent{game_Motion::dynamic, 0.0, 0.0, 0.05, 1.0, false, true, true, List<Bool>{false, false, false}, List<Bool>{false, false, false}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, "builtin:impact", 2.5, Vector{0.0, 0.0, 0.0}, List<Float>{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
    b.motion = game_Motion::dynamic;
    b.mass = spec.mass.mass;
    b.center_of_mass = spec.mass.center;
    for (auto it10 = flow_for(n10, 0, 5); it10.next();) {
      b.inertia[n10.index] = spec.mass.inertia[n10.index];
    }
    b.linear_damping = spec.linear_damping;
    b.angular_damping = spec.angular_damping;
    b.bullet = spec.bullet;
    b.allow_sleep = false;
    a.body = list_make<game_BodyComponent, "1">(b);
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<ColliderSpec>(spec.colliders))))); it11.next();) {
      cc = game_ColliderComponent{game_ColliderShape::automatic, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 1000.0, 0.6, 0.0, 0.0, "", false, 1, 4294967295, 0, "auto"};
      switch (flow_switch_enum<physics_ShapeKind>(spec.colliders[n11.index].kind)) {
        case physics_ShapeKind::box: {
          cc.shape = game_ColliderShape::box;
        } break;
        case physics_ShapeKind::sphere: {
          cc.shape = game_ColliderShape::sphere;
        } break;
        case physics_ShapeKind::capsule: {
          cc.shape = game_ColliderShape::capsule;
        } break;
        case physics_ShapeKind::hull: {
          cc.shape = game_ColliderShape::hull;
        } break;
        case physics_ShapeKind::mesh: {
          cc.shape = game_ColliderShape::hull;
        } break;
      }
      cc.center = spec.colliders[n11.index].center;
      quat_to_euler = engine_game_quat_to_euler(spec.colliders[n11.index].rotation);
      cc.rotation = quat_to_euler;
      cc.size = spec.colliders[n11.index].size;
      cc.radius = spec.colliders[n11.index].radius;
      cc.length = spec.colliders[n11.index].length;
      cc.points = spec.colliders[n11.index].points;
      cc.friction = spec.colliders[n11.index].friction;
      cc.restitution = spec.colliders[n11.index].restitution;
      cc.density = 100.0;
      list_add(a.colliders, cc);
    }
    return {a};
  }

  struct json_escapeResult { Text Result = ""; };
  KERN_PURE json_escapeResult json_escape(const Text& s) {
    Text o = "";
    Bool skip = false;
    Int c_2 = 0;
    o = "";
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it12.next();) {
      c_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n12.index))));
      skip = false;
      if (logic_or(math_compare<"==">(kern::to<Float>(c_2), 34.0), math_compare<"==">(kern::to<Float>(c_2), 92.0))) {
        o = text_append(o, text_of_code(92), "");
      }
      if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c_2) -> Float { return kern::math::band(c_2, 255.0); }, kern::to<Float>(c_2))))), 32.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        o = text_append(o, text_of_code(c_2), "");
      }
    }
    return {o};
  }

  struct mode_nameResult { Text Result = ""; };
  KERN_PURE mode_nameResult mode_name(Mode m = Mode::foot) {
    switch (flow_switch_enum<Mode>(m)) {
      case Mode::foot: {
        return {"foot"};
      } break;
      case Mode::drone: {
        return {"drone"};
      } break;
      case Mode::car: {
        return {"car"};
      } break;
      case Mode::jet: {
        return {"jet"};
      } break;
      case Mode::parachute: {
        return {"parachute"};
      } break;
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
};
