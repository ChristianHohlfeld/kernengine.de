struct game : virtual kern::Library, virtual car {
  // ---- types ----
  KERN_STRUCT(Game_Lamp) { Vector foot = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Int lane = 0; Int state = 0; };
  KERN_ENUM(Mode, foot, drone, car, jet, parachute);
  KERN_STRUCT(PadState) { Bool connected = false; Float lx = 0.0; Float ly = 0.0; Float rx = 0.0; Float ry = 0.0; Float lt = 0.0; Float rt = 0.0; Int buttons = 0; Int pressed = 0; Bool active = false; };
  KERN_STRUCT(PoliceDrone) { Bool active = false; Bool sees = false; Bool emp = false; Bool retreating = false; Vector pos = Vector{0.0, 0.0, 0.0}; Vector vel = Vector{0.0, 0.0, 0.0}; Vector target = Vector{0.0, 0.0, 0.0}; Vector retreat_goal = Vector{0.0, 0.0, 0.0}; Float hp = 100.0; Float yaw = 0.0; Float next_sensor = 0.0; Float next_shot = 0.0; Float sight_since = 0.0; Float destroyed_until = -1.0; Float retreat_until = -1.0; Float distance = 0.0; Float roof = 0.0; Float roof_at = -1.0; Int shots = 0; Int serial = 0; };
  KERN_STRUCT(PoliceUnit_Cop) { Int person = -1; Int seed = 0; Bool inside = true; Float next_shot = 0.0; Float los_at = 0.0; Bool los = false; Int seat = 1; Int id = 0; };
  KERN_ENUM(PoliceUnit_State, pursuit, stopping, engage, back, leave, abandoned, wreck);
  KERN_STRUCT(Game_VrHand) { Bool valid = false; Bool had = false; Bool world_ok = false; Bool grab_was = false; Bool swish = false; Vector p = Vector{0.0, 0.0, 0.0}; Vector prev = Vector{0.0, 0.0, 0.0}; Vector prev_strike = Vector{0.0, 0.0, 0.0}; Vector world_prev = Vector{0.0, 0.0, 0.0}; Vector grab_local = Vector{0.0, 0.0, 0.0}; Vector throw_v = Vector{0.0, 0.0, 0.0}; Quat q = Quat{0.0, 0.0, 0.0, 1.0}; Float swing = 0.0; Float strike_at = -9.0; Float grab_t = 0.0; physics_Body grab = physics_Body{Int64{0x0}}; Int grab_person = -1; Int grab_slot = -1; Int bird = -1; Float grab_mate_at = 0.0; };
  KERN_STRUCT(PoliceUnit) { Int car = -1; PoliceUnit_State state = PoliceUnit_State::pursuit; Fixed<PoliceUnit_Cop, 2> cops = List<PoliceUnit_Cop>{}; List<Vector> path = List<Vector>{}; Float path_at = -99.0; Float recover_until = -1.0; Float recover_start = 0.0; Float stuck_since = -1.0; Float turn_since = -1.0; Float corner_speed = 17.0; Vector last_target = Vector{0.0, 0.0, 0.0}; Vector progress_at = Vector{0.0, 0.0, 0.0}; Float progress_t = 0.0; Float left_at = -1.0; };
  KERN_STRUCT(TrafficCar) { Int actor = 0; List<Car> car = List<Car>{}; Vector color = Vector{0.7, 0.1, 0.1}; Bool placed = false; Bool driver = true; Int lane = -1; Int index = 0; Int dir = 1; Float target_speed = 10.0; Float want = 0.0; Int ahead = -1; Float peak_up = 0.0; Float peak_lift = 0.0; Bool far = false; Float far_speed = 0.0; List<Int64> solid_masks = List<Int64>{}; CarInput input = CarInput{0.0, 0.0, false, 32.0, 9.0}; Bool police = false; Int unit = -1; Float hp = 300.0; Float burn_until = -1.0; Float wreck_at = -1.0; Bool wreck = false; Bool hit_by_player = false; Vector last_velocity = Vector{0.0, 0.0, 0.0}; };

  // ---- functions ----
  struct PadState_downResult { Bool Result = false; };
  KERN_PURE PadState_downResult PadState_down(const PadState& self, Int b = 0) {
    return {math_compare<">">(kern::formula([](Float buttons, Float b) -> Float { return (kern::math::band(buttons, kern::math::wrap32(kern::math::shl(1.0, b))) != 0.0); }, kern::to<Float>(self.buttons), kern::to<Float>(b)), 0.5)};
  }

  struct PadState_hitResult { Bool Result = false; };
  KERN_PURE PadState_hitResult PadState_hit(const PadState& self, Int b = 0) {
    return {math_compare<">">(kern::formula([](Float pressed, Float b) -> Float { return (kern::math::band(pressed, kern::math::wrap32(kern::math::shl(1.0, b))) != 0.0); }, kern::to<Float>(self.pressed), kern::to<Float>(b)), 0.5)};
  }
};
