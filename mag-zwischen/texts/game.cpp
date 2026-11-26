struct game : virtual kern::Library, virtual car, virtual net_session, virtual weapons {
  // ---- types ----
  KERN_STRUCT(Game_BounceFx) { Vector p = Vector{0.0, 0.0, 0.0}; Vector n = Vector{0.0, 0.0, 1.0}; Float born = -99.0; Float strength = 1.0; };
  KERN_STRUCT(Game_Lamp) { Vector foot = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; U32 lane = 0; U8 state = 0; };
  KERN_STRUCT(Game_VisualBlast) { Vector p = Vector{0.0, 0.0, 0.0}; Float born = 0.0; Float scale = 1.0; };
  KERN_ENUM(Mode, foot, drone, car, jet, parachute);
  KERN_STRUCT(PadState) { Bool connected = false; Float lx = 0.0; Float ly = 0.0; Float rx = 0.0; Float ry = 0.0; Float lt = 0.0; Float rt = 0.0; U32 buttons = 0; U32 pressed = 0; Bool active = false; };
  KERN_STRUCT(PoliceDrone) { Bool active = false; Bool sees = false; Bool emp = false; Bool retreating = false; Vector pos = Vector{0.0, 0.0, 0.0}; Vector vel = Vector{0.0, 0.0, 0.0}; Vector target = Vector{0.0, 0.0, 0.0}; Vector retreat_goal = Vector{0.0, 0.0, 0.0}; Float hp = 100.0; Float yaw = 0.0; Float next_sensor = 0.0; Float next_shot = 0.0; Float sight_since = 0.0; Float destroyed_until = -1.0; Float retreat_until = -1.0; Float distance = 0.0; Float roof = 0.0; Float roof_at = -1.0; U32 shots = 0; U32 serial = 0; };
  KERN_STRUCT(PoliceUnit_Cop) { I32 person = -1; U32 seed = 0; Bool inside = true; Float next_shot = 0.0; Float los_at = 0.0; Bool los = false; I32 seat = 1; U16 id = 0; };
  KERN_ENUM(PoliceUnit_State, pursuit, stopping, engage, back, leave, abandoned, wreck);
  KERN_STRUCT(Game_RemoteUnit) { U16 id = 0; net_SwarmKind kind = net_SwarmKind::zombie; U8 flags = 0; Vector p = Vector{0.0, 0.0, 0.0}; Vector tp = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; Float tyaw = 0.0; Float aux = 0.0; Float rotor = 0.0; I32 person = -1; Float wreck_at = -1.0; Bool fallen = false; };
  KERN_STRUCT(Game_VrHand) { Bool valid = false; Bool had = false; Bool world_ok = false; Bool grab_was = false; Bool swish = false; Vector p = Vector{0.0, 0.0, 0.0}; Vector prev = Vector{0.0, 0.0, 0.0}; Vector prev_strike = Vector{0.0, 0.0, 0.0}; Vector world_prev = Vector{0.0, 0.0, 0.0}; Vector grab_local = Vector{0.0, 0.0, 0.0}; Vector throw_v = Vector{0.0, 0.0, 0.0}; Quat q = Quat{0.0, 0.0, 0.0, 1.0}; Float swing = 0.0; Float strike_at = -9.0; Float grab_t = 0.0; physics_Body grab = physics_Body{Int64{0x0}}; I32 grab_person = -1; I32 grab_slot = -1; I32 bird = -1; Float grab_mate_at = 0.0; };
  KERN_STRUCT(PoliceUnit) { I32 car = -1; PoliceUnit_State state = PoliceUnit_State::pursuit; Fixed<PoliceUnit_Cop, 2> cops = List<PoliceUnit_Cop>{}; List<Vector> path = List<Vector>{}; Float path_at = -99.0; Float recover_until = -1.0; Float recover_start = 0.0; Float stuck_since = -1.0; Float turn_since = -1.0; Float corner_speed = 17.0; Vector last_target = Vector{0.0, 0.0, 0.0}; Vector progress_at = Vector{0.0, 0.0, 0.0}; Float progress_t = 0.0; Float left_at = -1.0; };
  KERN_STRUCT(Projectile) { weapons_Id id = weapons_Id::grenade_launcher; Vector pos = Vector{0.0, 0.0, 0.0}; Vector vel = Vector{0.0, 0.0, 0.0}; Float born = 0.0; Float fuse = 12.0; Bool by_player = true; Bool gravity = false; Bool guided = false; Bool alive = true; I32 bounces = 0; Bool resting = false; Float next_collision = 0.0; Float smoke_at = -1.0; physics_Body ignore = physics_Body{Int64{0x0}}; U32 net = 0; Bool ghost = false; };
  KERN_STRUCT(Game_RemoteSwarm) { Float seen = 0.0; List<Game_RemoteUnit> units = List<Game_RemoteUnit>{}; };
  KERN_STRUCT(Game_SwarmFallen) { net_SwarmUnit unit = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}; Float until = 0.0; };
  KERN_STRUCT(Game_SwarmSent) { net_SwarmUnit unit = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}; I32 person = -1; U32 seed = 0; };
  KERN_STRUCT(TrafficCar) { U32 actor = 0; List<Car> car = List<Car>{}; Vector color = Vector{0.7, 0.1, 0.1}; Bool placed = false; Bool driver = true; I32 lane = -1; I32 index = 0; I32 dir = 1; Float target_speed = 10.0; Float want = 0.0; I32 ahead = -1; Float peak_up = 0.0; Float peak_lift = 0.0; Bool far = false; Float far_speed = 0.0; List<Int64> solid_masks = List<Int64>{}; CarInput input = CarInput{0.0, 0.0, false, 32.0, 9.0}; Bool police = false; I32 unit = -1; Float hp = 300.0; Float burn_until = -1.0; Float wreck_at = -1.0; Bool wreck = false; Bool hit_by_player = false; Vector last_velocity = Vector{0.0, 0.0, 0.0}; };

  // ---- variables ----
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// m: the safe margin a player is put down over the ground
  KERN_CONST Float kSpawnLift = 0.35;

  // ---- functions ----
  struct PadState_downResult { Bool Result = false; };
  KERN_PURE PadState_downResult PadState_down(const PadState& self, I32 b = 0) {
    return {math_compare<">">(kern::formula([](Float buttons, Float b) -> Float { return (kern::math::band(buttons, kern::math::wrap32(kern::math::shl(1.0, b))) != 0.0); }, kern::to<Float>(self.buttons), kern::to<Float>(b)), 0.5)};
  }

  struct PadState_hitResult { Bool Result = false; };
  KERN_PURE PadState_hitResult PadState_hit(const PadState& self, I32 b = 0) {
    return {math_compare<">">(kern::formula([](Float pressed, Float b) -> Float { return (kern::math::band(pressed, kern::math::wrap32(kern::math::shl(1.0, b))) != 0.0); }, kern::to<Float>(self.pressed), kern::to<Float>(b)), 0.5)};
  }
};
