struct Game : virtual kern::Blueprint, virtual common, virtual engine_game_runtime, virtual engine_math_math, virtual engine_physics_physics, virtual game, virtual game_controls, virtual game_traffic_cpp, virtual game_world, virtual game_worldsync_cpp, virtual jet, virtual net_session, virtual terrain {
  // ---- variables ----
  /// client: the host's wall clock minus our real clock (kern::now_ms)
  Float host_epoch_off_ = 0.0;
  Bool host_epoch_ok_ = false;
  List<WorldLife> life_ = List<WorldLife>{};
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
  Bool rig_init_ = false;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  KERN_CONST Float kPi = 3.141592653589793;

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

  void hide_soldier() {
    Bool set_mode = false;
    engine_game_world_set_alive(world_, soldier_, false);
    engine_game_world_set_mode(n1048595, world_, game_SimMode::simulate);
    set_mode = n1048595.result;
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n1048600 = state_of(jet_actor_);
    s = n1048600.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n1048601, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n1048601.result;
    engine_game_world_set_actor_velocity(n1048602, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n1048602.result;
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
    engine_game_world_physics(n1048605, world_);
    physics = n1048605.result;
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

  struct traffic_lane_distanceResult { Float Result = 0.0; };
  traffic_lane_distanceResult traffic_lane_distance(const TrafficCar& c) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_or(math_compare<"<">(kern::to<Float>(c.lane), 0.0), math_compare<">=">(kern::to<Float>(c.lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(city_.lanes_))))))))) {
      return {-1.0};
    }
    n2097162 = matrix_of(c.actor);
    a = n2097162.Result;
    p = engine_math_math::Mat4_translation(a).Result;
    return {game_traffic_cpp::lane_distance(city_.lanes_[c.lane], vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result};
  }

  struct car_towedResult { Bool Result = false; };
  KERN_PURE car_towedResult car_towed(Int car = 0) {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return {false};
    }
    for (auto it2097178 = flow_for(n2097178, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it2097178.next();) {
      if (math_compare<"==">(kern::to<Float>(life_[0].trucks[n2097178.index].job), kern::to<Float>(car))) {
        return {true};
      }
    }
    return {false};
  }

  void hud_world(Text& s) {
    Text b = "";
    b = "";
    b = text_format<",\"terrain\":{\"dem\":%s,\"settled\":%s,\"min\":%.1f,\"max\":%.1f,\"render\":%u,\"collision\":%u,\"water\":%u,\"craters\":%u,\"demTiles\":%u,\"demFailed\":%u}">(399, logic_select<Text>(city_.terrain_.stats_.dem, "true", "false"), logic_select<Text>(terrain::Terrain_settled(city_.terrain_).Result, "true", "false"), city_.terrain_.stats_.min_height, city_.terrain_.stats_.max_height, city_.terrain_.stats_.render_tiles, city_.terrain_.stats_.collision_tiles, city_.terrain_.stats_.water_cells, city_.terrain_.stats_.craters, city_.terrain_.stats_.dem_tiles, city_.terrain_.stats_.dem_failed);
    s = text_append(s, b, "");
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return;
    }
    b = text_format<",\"swimming\":%s,\"splashes\":%u">(399, logic_select<Text>(life_[0].swimming, "true", "false"), life_[0].splashes);
    s = text_append(s, b, "");
    b = text_format<",\"marks\":{\"scorch\":%u,\"soot\":%u,\"breach\":%u,\"rubble\":%u,\"craters\":%u}">(399, life_[0].marks.stats_.scorches, life_[0].marks.stats_.soots, life_[0].marks.stats_.breaches, life_[0].marks.stats_.rubble, life_[0].marks.stats_.craters);
    s = text_append(s, b, "");
  }

  struct world_command_startsResult { Bool Result = false; };
  KERN_PURE world_command_startsResult world_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct mate_fociResult { List<Vector> Result = List<Vector>{}; };
  KERN_PURE mate_fociResult mate_foci() {
    List<Vector> out = List<Vector>{};
    Bool skip = false;
    out = List<Vector>{};
    if (logic_not(net_session::Session_active(net_).Result)) {
      return {out};
    }
    for (auto it4194314 = flow_for(n4194314, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it4194314.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n4194314.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        list_add(out, logic_select<Vector>(logic_or(enum_equal<net_PlayerMode>(net_.remotes_[n4194314.index].state.mode, net_PlayerMode::foot), enum_equal<net_PlayerMode>(net_.remotes_[n4194314.index].state.mode, net_PlayerMode::drone)), net_.remotes_[n4194314.index].person_shown, net_.remotes_[n4194314.index].shown));
      }
    }
    return {out};
  }

  struct world_puppetResult { Bool Result = false; };
  KERN_PURE world_puppetResult world_puppet() {
    return {logic_and(net_session::Session_active(net_).Result, logic_not(net_session::Session_hosting(net_).Result))};
  }

  struct life_epochResult { Float Result = 0.0; };
  KERN_PURE life_epochResult life_epoch() {
    Float now_ms = 0.0;
    Float wall_ms = 0.0;
    if (logic_and(logic_and(net_session::Session_active(net_).Result, logic_not(net_session::Session_hosting(net_).Result)), host_epoch_ok_)) {
      now_ms = engine_now_ms();
      return {kern::formula([](Float now_ms, Float host_epoch_off_) -> Float { return (kern::math::div(now_ms, 1000.0) + host_epoch_off_); }, now_ms, host_epoch_off_)};
    }
    wall_ms = engine_wall_ms();
    return {kern::formula([](Float wall_ms) -> Float { return kern::math::div(wall_ms, 1000.0); }, wall_ms)};
  }

  struct worldsync_update_ang16Result { Int Result = 0; };
  KERN_PURE worldsync_update_ang16Result worldsync_update_ang16(Float a = 0.0) {
    return {kern::to<Int>(kern::formula([](Float the_result, Float kPi) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round((kern::math::div(the_result, kPi) * 32767.0)), 65535.0), 32768.0) - 32768.0); }, common::wrap_pi(a).Result, kPi))};
  }

  struct worldsync_update_ang8Result { Int Result = 0; };
  KERN_PURE worldsync_update_ang8Result worldsync_update_ang8(Float a = 0.0) {
    return {kern::to<Int>(kern::formula([](Float the_result, Float kPi) -> Float { return kern::math::band(kern::math::band(kern::math::wrap32s(kern::math::round((kern::math::div(the_result, (2.0 * kPi)) * 256.0))), 255.0), 255.0); }, common::wrap_pi(a).Result, kPi))};
  }

  struct worldsync_update_p5Result { Int Result = 0; };
  /// 5 cm
  KERN_PURE worldsync_update_p5Result worldsync_update_p5(Float v = 0.0, Float base = 0.0) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 65535.0), 32768.0) - 32768.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v, Float base) -> Float { return ((v - base) * 20.0); }, v, base), -32000.0, 32000.0)))};
  }

  struct worldsync_update_p1Result { Int Result = 0; };
  /// 1 cm
  KERN_PURE worldsync_update_p1Result worldsync_update_p1(Float v = 0.0, Float base = 0.0) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 65535.0), 32768.0) - 32768.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v, Float base) -> Float { return ((v - base) * 100.0); }, v, base), -32000.0, 32000.0)))};
  }

  struct worldsync_update_nearResult { Bool Result = false; };
  KERN_PURE worldsync_update_nearResult worldsync_update_near(Vector at_2 = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}, Float reach = 0.0) {
    return {math_compare<"<">(game_worldsync_cpp::dist2d(p, at_2).Result, reach)};
  }

  struct the_worldsync_update_list_Result { Game_worldsync_update_List Result = Game_worldsync_update_List{0, 0, Vector{0.0, 0.0, 0.0}, 0, List<Int>{}, List<Int>{}, 0, 0}; };
  KERN_PURE the_worldsync_update_list_Result the_worldsync_update_list_(Int to = 0, Vector o = Vector{0.0, 0.0, 0.0}, Int t_ms = 0, Int type = 0) {
    Game_worldsync_update_List l = Game_worldsync_update_List{0, 0, Vector{0.0, 0.0, 0.0}, 0, List<Int>{}, List<Int>{}, 0, 0};
    l = struct_make<Game_worldsync_update_List>(to, type, o, t_ms, List<Int>{}, List<Int>{}, 0, 0);
    return {l};
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n1048586;
  struct : kern::Node<1048591> { Int index = 0; } n1048591;
  struct { Bool result = false; } n1048595;
  state_ofResult n1048600;
  struct { Bool result = false; } n1048601;
  struct { Bool result = false; } n1048602;
  struct { Int result = 0; } n1048605;
  matrix_ofResult n2097162;
  struct : kern::Node<2097178> { Int index = 0; } n2097178;
  struct : kern::Node<4194314> { Int index = 0; } n4194314;
};
