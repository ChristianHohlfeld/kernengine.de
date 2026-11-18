struct Game : virtual kern::Blueprint, virtual city_stream_cpp, virtual common, virtual engine_game_runtime, virtual engine_math_math, virtual engine_physics_physics, virtual game, virtual game_controls, virtual jet, virtual net_session, virtual terrain_cpp {
  // ---- variables ----
  Int world_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, Map<Int, CityStream_Damage>{}, List<Int>{}, false, 0, 0, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<Int64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}, List<Vec4>{}, Int64{0x0}, -1};
  Int soldier_ = 0;
  Int jet_actor_ = 0;
  Int chute_actor_ = 0;
  Jet jet_ = Jet{true, true, true, 1.0, 1.0, JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}, Jet_Mode::hover, 0.0, 0.0, 0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, true, false, true, 0, 0.0, JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}};
  Mode mode_ = Mode::foot;
  Bool chute_open_ = false;
  Float chute_time_ = 0.0;
  Vector chute_steer_ = Vector{0.0, 0.0, 0.0};
  Vector cam_anchor_ = Vector{0.0, 0.0, 0.0};
  Vector cam_vel_ = Vector{0.0, 0.0, 0.0};
  Vector cam_heading_ = Vector{1.0, 0.0, 0.0};
  Bool rig_init_ = false;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// x, y, yaw (deg)
  Vector spawn_guard_ = Vector{0.0, 0.0, 0.0};
  Float spawn_guard_until_ = -1.0;
  Float spawn_guard_ready_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  /// (kept by mp_rules)
  List<Int> mp_rules_g_palette = List<Int>{};
  KERN_CONST Float kRespawnCooldown = 3.0;
  KERN_CONST Float kPi = 3.141592653589793;
  /// m: the safe margin a player is put down over the ground
  KERN_CONST Float kSpawnLift = 0.35;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;

  // ---- functions ----
  /// Mouse movement under the pointer lock (CSS px, clamped per event) and the lock state (game_controls.cpp).
  void mouse_move(Float dx = 0.0, Float dy = 0.0) {
    ctl_.mouse = vector_make(kern::formula([](Float x, Float dx) -> Float { return (x + dx); }, vector_break(ctl_.mouse).x, dx), vector_break(ctl_.mouse).y, vector_break(ctl_.mouse).z);
    ctl_.mouse = vector_make(vector_break(ctl_.mouse).x, kern::formula([](Float y, Float dy) -> Float { return (y + dy); }, vector_break(ctl_.mouse).y, dy), vector_break(ctl_.mouse).z);
  }

  void pointer_locked(Bool locked = false) {
    ctl_.locked = locked;
  }

  struct bodyResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  KERN_PURE bodyResult body(Int a = 0) {
    physics_Body actor_body = physics_Body{Int64{0x0}};
    actor_body = engine_game_world_actor_body(world_, a);
    return {actor_body};
  }

  struct state_ofResult { BodyState Result = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}; };
  state_ofResult state_of(Int a = 0) {
    Int physics = 0;
    engine_game_world_physics(n10, world_);
    physics = n10.result;
    return {common::read_body(physics, body(a).Result).Result};
  }

  struct player_drivingResult { Bool Result = false; };
  KERN_PURE player_drivingResult player_driving() {
    return {logic_or(logic_or(enum_equal<Mode>(mode_, Mode::car), enum_equal<Mode>(mode_, Mode::jet)), enum_equal<Mode>(mode_, Mode::drone))};
  }

  struct respawn_waitResult { Float Result = 0.0; };
  KERN_PURE respawn_waitResult respawn_wait() {
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    Float chosen = 0.0;
    if (logic_and(player_dead_, math_compare<">=">(died_at_, 0.0))) {
      sim_time_2 = engine_game_world_sim_time(world_);
      chosen = kern::formula([](Float kRespawnCooldown, Float sim_time_2, Float died_at_) -> Float { return kern::math::max(0.0, (kRespawnCooldown - (sim_time_2 - died_at_))); }, kRespawnCooldown, sim_time_2, died_at_);
    } else {
      chosen = -1.0;
    }
    return {kern::formula([](Float chosen) -> Float { return chosen; }, chosen)};
  }

  struct matrix_ofResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  matrix_ofResult matrix_of(Int a = 0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool actor_matrix = false;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    engine_game_world_actor_matrix(n1048586, world_, a, m);
    actor_matrix = n1048586.result;
    m = n1048586.out;
    return {m};
  }

  struct Game_clear_spot_on_roadResult { Bool Result = false; };
  KERN_PURE Game_clear_spot_on_roadResult Game_clear_spot_on_road(Bool off_road = false, Vector half = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}) {
    Float reach = 0.0;
    Bool skip = false;
    Int k = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector ab = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    if (logic_not(off_road)) {
      return {false};
    }
    reach = kern::formula([](Float x, Float y) -> Float { return (kern::math::sqrt((x * x) + (y * y)) + 2.0); }, vector_break(half).x, vector_break(half).y);
    for (auto it1048591 = flow_for(n1048591, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it1048591.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n1048591.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n1048591.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n1048591.index].points))), 0.5))) {
          a = city_.lanes_[n1048591.index].points[k];
          b = city_.lanes_[n1048591.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n1048591.index].width))) {
            return {true};
          }
          k += 1;
        }
      }
    }
    return {false};
  }

  struct clear_spotResult { Bool Result = false; };
  clear_spotResult clear_spot(Game_Spot& out, Vector center = Vector{0.0, 0.0, 0.0}, Vector half = Vector{0.0, 0.0, 0.0}, Float search = 0.0, Bool off_road = false) {
    Int w = 0;
    Int physics = 0;
    Float r = 0.0;
    Int steps = 0;
    Float a = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Bool ok = false;
    Int ix = 0;
    Int iy = 0;
    Vector o = Vector{0.0, 0.0, 0.0};
    the_RayResult_ h = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    engine_game_world_physics(n1048592, world_);
    physics = n1048592.result;
    w = physics;
    r = 0.0;
    while (loop(math_compare<"<=">(r, search))) {
      steps = kern::to<Int>(kern::formula([](Float r, Float kPi) -> Float { return ((r == 0.0) ? 1.0 : kern::math::trunc(kern::math::div(((2.0 * kPi) * r), 8.0))); }, r, kPi));
      for (auto it1048593 = flow_for(n1048593, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(steps)))); it1048593.next();) {
        a = kern::formula([](Float kPi, Float index, Float steps) -> Float { return kern::math::div(((2.0 * kPi) * index), steps); }, kPi, kern::to<Float>(n1048593.index), kern::to<Float>(steps));
        c = vector_make(kern::formula([](Float x, Float r, Float a) -> Float { return (x + (r * kern::math::cos(a))); }, vector_break(center).x, r, a), kern::formula([](Float y, Float r, Float a) -> Float { return (y + (r * kern::math::sin(a))); }, vector_break(center).y, r, a), 0.0);
        c = vector_make(vector_break(c).x, vector_break(c).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(c).x, vector_break(c).y).Result);
        ok = true;
        ix = -2;
        while (loop(logic_and(math_compare<"<=">(kern::to<Float>(ix), 2.0), ok))) {
          iy = -2;
          while (loop(logic_and(math_compare<"<=">(kern::to<Float>(iy), 2.0), ok))) {
            o = vector_make(kern::formula([](Float x, Float x_2, Float ix) -> Float { return (x + ((x_2 * ix) * 0.5)); }, vector_break(c).x, vector_break(half).x, kern::to<Float>(ix)), kern::formula([](Float y, Float y_2, Float iy) -> Float { return (y + ((y_2 * iy) * 0.5)); }, vector_break(c).y, vector_break(half).y, kern::to<Float>(iy)), kern::formula([](Float z) -> Float { return (z + 80.0); }, vector_break(c).z));
            h = common::cast(w, o, Vector{0.0, 0.0, -1.0}, 90.0, struct_make<physics_Body>(Int64{0x0}), Int64{0xffffffffffffffff}).Result;
            ok = logic_and(logic_and(h.hit, math_compare<"<">(vector_break(h.point).z, kern::formula([](Float the_result) -> Float { return (the_result + 0.3); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(o).x, vector_break(o).y).Result))), logic_not(terrain_cpp::Terrain_water_at(city_.terrain_, vector_break(o).x, vector_break(o).y).Result));
            iy += 1;
          }
          ix += 1;
        }
        if (logic_and(ok, logic_not(Game_clear_spot_on_road(off_road, half, vector_make(vector_break(c).x, vector_break(c).y, 0.0)).Result))) {
          out = struct_make<Game_Spot>(c, 0.0);
          return {true};
        }
      }
      r += 6.0;
    }
    return {false};
  }

  struct standing_feetResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  standing_feetResult standing_feet(Vector feet = Vector{0.0, 0.0, 0.0}) {
    Float ground = 0.0;
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    Float z = 0.0;
    ground = city_stream_cpp::CityStream_ground_height(city_, vector_break(feet).x, vector_break(feet).y).Result;
    engine_game_world_physics(n1048602, world_);
    physics = n1048602.result;
    r = common::cast(physics, vector_make(vector_break(feet).x, vector_break(feet).y, kern::formula([](Float z, Float ground) -> Float { return (kern::math::max(z, ground) + 1.0); }, vector_break(feet).z, ground)), Vector{0.0, 0.0, -1.0}, kern::formula([](Float z, Float ground) -> Float { return ((1.0 + kern::math::abs((z - ground))) + 1.0); }, vector_break(feet).z, ground), body(soldier_).Result, Int64{0x1}).Result;
    z = ground;
    if (logic_and(logic_and(r.hit, math_compare<">">(vector_break(r.normal).z, 0.6)), math_compare<">">(vector_break(r.point).z, kern::formula([](Float ground) -> Float { return (ground - 0.05); }, ground)))) {
      z = vector_break(r.point).z;
    }
    return {vector_make(vector_break(feet).x, vector_break(feet).y, kern::formula([](Float z) -> Float { return (z + 0.02); }, z))};
  }

  void put_down(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Vector rot = Vector{0.0, 0.0, 0.0};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    Float sim_time = 0.0;
    rot = vector_make(0.0, 0.0, yaw_deg);
    n1048603 = standing_feet(feet);
    engine_game_world_teleport(n1048604, world_, soldier_, vector_add(n1048603.Result, vector_make(0.0, 0.0, kern::formula([](Float kSpawnLift) -> Float { return (0.73 + kSpawnLift); }, kSpawnLift))), list_make<Vector, "1">(rot));
    teleport = n1048604.result;
    engine_game_world_set_actor_velocity(n1048605, world_, soldier_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n1048605.result;
    spawn_guard_ = vector_make(vector_break(feet).x, vector_break(feet).y, yaw_deg);
    sim_time = engine_game_world_sim_time(world_);
    spawn_guard_until_ = kern::formula([](Float sim_time) -> Float { return (sim_time + 20.0); }, sim_time);
    spawn_guard_ready_at_ = -1.0;
  }

  struct Game_place_world_takenResult { Bool Result = false; };
  Game_place_world_takenResult Game_place_world_taken(Vector at = Vector{0.0, 0.0, 0.0}) {
    for (auto it1048606 = flow_for(n1048606, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it1048606.next();) {
      if (logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n1048606.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))), 0.5), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(net_.remotes_[n1048606.index].state.jet_pos).x, vector_break(at).x, vector_break(net_.remotes_[n1048606.index].state.jet_pos).y, vector_break(at).y), 24.0))) {
        return {true};
      }
    }
    return {false};
  }

  void hide_soldier() {
    Bool set_mode = false;
    engine_game_world_set_alive(world_, soldier_, false);
    engine_game_world_set_mode(n1048613, world_, game_SimMode::simulate);
    set_mode = n1048613.result;
  }

  void show_soldier(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Bool actor_alive = false;
    Bool set_mode = false;
    Vector rot = Vector{0.0, 0.0, 0.0};
    actor_alive = engine_game_world_actor_alive(world_, soldier_);
    if (logic_not(actor_alive)) {
      engine_game_world_set_alive(world_, soldier_, true);
    }
    engine_game_world_set_mode(n1048614, world_, game_SimMode::play);
    set_mode = n1048614.result;
    rot = vector_make(0.0, 0.0, yaw_deg);
    put_down(feet, yaw_deg);
  }

  void spawn_guard_step() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Vector at = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector c = Vector{0.0, 0.0, 0.0};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector feet = Vector{0.0, 0.0, 0.0};
    Bool ready = false;
    Vector stand = Vector{0.0, 0.0, 0.0};
    Vector hold = Vector{0.0, 0.0, 0.0};
    Vector rot = Vector{0.0, 0.0, 0.0};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    Bool teleport_2 = false;
    Bool set_actor_velocity_2 = false;
    if (logic_or(logic_or(math_compare<"<">(spawn_guard_until_, 0.0), logic_not(enum_equal<Mode>(mode_, Mode::foot))), player_dead_)) {
      spawn_guard_until_ = -1.0;
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n1048615 = matrix_of(soldier_);
    a = n1048615.Result;
    at = engine_math_math::Mat4_translation(a).Result;
    if (logic_and(math_compare<">">(now, spawn_guard_until_), logic_or(math_compare<">=">(spawn_guard_ready_at_, 0.0), terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(at).x, vector_break(at).y).Result))) {
      spawn_guard_until_ = -1.0;
      return;
    }
    n1048616 = matrix_of(soldier_);
    a_2 = n1048616.Result;
    c = engine_math_math::Mat4_translation(a_2).Result;
    feet = vector_make(vector_break(c).x, vector_break(c).y, kern::formula([](Float z) -> Float { return (z - 0.73); }, vector_break(c).z));
    ready = terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(feet).x, vector_break(feet).y).Result;
    n1048617 = standing_feet(feet);
    stand = n1048617.Result;
    if (logic_not(ready)) {
      hold = vector_add(stand, vector_make(0.0, 0.0, kSpawnLift));
      if (math_compare<">">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(feet).z, vector_break(hold).z), 0.05)) {
        rot = vector_make(0.0, 0.0, vector_break(spawn_guard_).z);
        engine_game_world_teleport(n1048618, world_, soldier_, vector_add(hold, Vector{0.0, 0.0, 0.73}), list_make<Vector, "1">(rot));
        teleport = n1048618.result;
      }
      engine_game_world_set_actor_velocity(n1048619, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity = n1048619.result;
      spawn_guard_ready_at_ = -1.0;
      return;
    }
    if (math_compare<"<">(spawn_guard_ready_at_, 0.0)) {
      spawn_guard_ready_at_ = now;
    }
    if (math_compare<"<">(vector_break(feet).z, kern::formula([](Float z) -> Float { return (z - 0.25); }, vector_break(stand).z))) {
      engine_game_world_teleport(n1048620, world_, soldier_, vector_add(stand, Vector{0.0, 0.0, 0.83}), List<Vector>{});
      teleport_2 = n1048620.result;
      engine_game_world_set_actor_velocity(n1048621, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity_2 = n1048621.result;
    }
    if (math_compare<">">(kern::formula([](Float now, Float spawn_guard_ready_at_) -> Float { return (now - spawn_guard_ready_at_); }, now, spawn_guard_ready_at_), 2.0)) {
      spawn_guard_until_ = -1.0;
    }
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n1048625 = state_of(jet_actor_);
    s = n1048625.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n1048626, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n1048626.result;
    engine_game_world_set_actor_velocity(n1048627, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n1048627.result;
    chute_time_ = 0.0;
    chute_open_ = false;
    mode_ = Mode::parachute;
    rig_init_ = false;
    toast_ = "Schleudersitz!";
    toast_age_ = 0.0;
  }

  void step_parachute(Float h = 0.0) {
    Int w = 0;
    Int physics = 0;
    physics_Body b = physics_Body{Int64{0x0}};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float m = 0.0;
    Float mass = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    engine_game_world_physics(n1048631, world_);
    physics = n1048631.result;
    w = physics;
    b = body(chute_actor_).Result;
    if (logic_not(engine_physics_physics::Body_valid(b).Result)) {
      return;
    }
    s = common::read_body(w, b).Result;
    mass = engine_world_mass(w, b);
    m = mass;
    chute_time_ = kern::formula([](Float chute_time_, Float h) -> Float { return (chute_time_ + h); }, chute_time_, h);
    chute_open_ = math_compare<">=">(chute_time_, kChuteOpen);
    f = vector_make(kern::formula([](Float m, Float x) -> Float { return (((-m) * 0.25) * x); }, m, vector_break(s.velocity).x), kern::formula([](Float m, Float y) -> Float { return (((-m) * 0.25) * y); }, m, vector_break(s.velocity).y), 0.0);
    if (chute_open_) {
      f = vector_make(vector_break(f).x, vector_break(f).y, kern::formula([](Float m, Float kChuteSink, Float z) -> Float { return ((m * 1.6) * ((-kChuteSink) - z)); }, m, kChuteSink, vector_break(s.velocity).z));
      want = vector_scale(chute_steer_, kChuteSteer);
      f = vector_make(kern::formula([](Float x, Float m, Float x_2, Float x_3) -> Float { return (x + ((m * 0.8) * (x_2 - x_3))); }, vector_break(f).x, m, vector_break(want).x, vector_break(s.velocity).x), vector_break(f).y, vector_break(f).z);
      f = vector_make(vector_break(f).x, kern::formula([](Float y, Float m, Float y_2, Float y_3) -> Float { return (y + ((m * 0.8) * (y_2 - y_3))); }, vector_break(f).y, m, vector_break(want).y, vector_break(s.velocity).y), vector_break(f).z);
    }
    engine_world_apply_force_to_center(w, b, f);
  }

  void Game_update_camera_rig(Float dt = 0.0, Vector anchor = Vector{0.0, 0.0, 0.0}, Vector vel = Vector{0.0, 0.0, 0.0}, Vector heading = Vector{0.0, 0.0, 0.0}, Float pos_rate = 0.0, Float vel_rate = 0.0, Float head_rate = 0.0, Float max_lag = 0.0) {
    Vector predicted = Vector{0.0, 0.0, 0.0};
    Vector lag = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float ha = 0.0;
    Float hb = 0.0;
    Float h = 0.0;
    if (logic_not(rig_init_)) {
      cam_anchor_ = anchor;
      cam_vel_ = vel;
      cam_heading_ = heading;
      rig_init_ = true;
    }
    cam_vel_ = vector_add(cam_vel_, vector_scale(vector_sub(vel, cam_vel_), common::damp_alpha(vel_rate, dt).Result));
    predicted = vector_add(cam_anchor_, vector_scale(cam_vel_, dt));
    cam_anchor_ = vector_add(predicted, vector_scale(vector_sub(anchor, predicted), common::damp_alpha(pos_rate, dt).Result));
    lag = vector_sub(anchor, cam_anchor_);
    l = vector_length(lag);
    if (math_compare<">">(l, max_lag)) {
      cam_anchor_ = vector_sub(anchor, vector_scale(lag, kern::formula([](Float max_lag, Float l) -> Float { return kern::math::div(max_lag, l); }, max_lag, l)));
    }
    ha = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(cam_heading_).y, vector_break(cam_heading_).x);
    hb = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(heading).y, vector_break(heading).x);
    h = kern::formula([](Float ha, Float the_result, Float the_result_2) -> Float { return (ha + (the_result * the_result_2)); }, ha, common::wrap_pi(kern::formula([](Float hb, Float ha) -> Float { return (hb - ha); }, hb, ha)).Result, common::damp_alpha(head_rate, dt).Result);
    cam_heading_ = vector_make(kern::formula([](Float h) -> Float { return kern::math::cos(h); }, h), kern::formula([](Float h) -> Float { return kern::math::sin(h); }, h), 0.0);
  }

  struct Game_update_camera_collideResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  Game_update_camera_collideResult Game_update_camera_collide(Vector anchor = Vector{0.0, 0.0, 0.0}, Vector want = Vector{0.0, 0.0, 0.0}, physics_Body ignore = physics_Body{Int64{0x0}}) {
    Vector d = Vector{0.0, 0.0, 0.0};
    physics_RayHit h = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    Int physics = 0;
    physics_RayHit cast_sphere = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    d = vector_sub(want, anchor);
    engine_game_world_physics(n1048632, world_);
    physics = n1048632.result;
    cast_sphere = engine_world_cast_sphere(physics, anchor, 0.17, d, engine_physics_physics::QueryFilter_new(Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}).Result, ignore);
    h = cast_sphere;
    return {logic_select<Vector>(h.hit, vector_add(anchor, vector_scale(d, kern::formula([](Float fraction) -> Float { return kern::math::max(0.0, (fraction - 0.02)); }, h.fraction))), want)};
  }

  struct paletteResult { List<Int> Result = List<Int>{}; };
  KERN_PURE paletteResult palette() {
    return {mp_rules_g_palette};
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n1048586;
  struct : kern::Node<1048591> { Int index = 0; } n1048591;
  struct { Int result = 0; } n1048592;
  struct : kern::Node<1048593> { Int index = 0; } n1048593;
  struct { Int result = 0; } n1048602;
  standing_feetResult n1048603;
  struct { Bool result = false; } n1048604;
  struct { Bool result = false; } n1048605;
  struct : kern::Node<1048606> { Int index = 0; } n1048606;
  struct { Bool result = false; } n1048613;
  struct { Bool result = false; } n1048614;
  matrix_ofResult n1048615;
  matrix_ofResult n1048616;
  standing_feetResult n1048617;
  struct { Bool result = false; } n1048618;
  struct { Bool result = false; } n1048619;
  struct { Bool result = false; } n1048620;
  struct { Bool result = false; } n1048621;
  state_ofResult n1048625;
  struct { Bool result = false; } n1048626;
  struct { Bool result = false; } n1048627;
  struct { Int result = 0; } n1048631;
  struct { Int result = 0; } n1048632;
};
