struct Game : virtual kern::Blueprint, virtual car, virtual city_stream, virtual common, virtual engine_math_math, virtual engine_physics_physics, virtual game_combat, virtual game_controls, virtual game_gaps, virtual game_net, virtual game_swarm_cpp, virtual game_townsync, virtual jet, virtual lamps, virtual net_session, virtual people, virtual person_hit, virtual settings {
  // ---- variables ----
  List<TownSync> townsync_ = List<TownSync>{};
  Bool puppet_world_ = false;
  /// client: the host's index each of our cars plays (-1 none)
  List<Int> car_net_ = List<Int>{};
  Map<Int, Game_RemoteSwarm> swarms_ = Map<Int, Game_RemoteSwarm>{};
  Int swarm_serial_ = 0;
  List<Game_SwarmSent> swarm_sent_ = List<Game_SwarmSent>{};
  List<Game_SwarmFallen> swarm_fallen_ = List<Game_SwarmFallen>{};
  List<GapsState> gaps_ = List<GapsState>{};
  Int world_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, Map<Int, CityStream_Damage>{}, List<Int>{}, false, 0, 0, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<Int64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}, List<Vec4>{}, Int64{0x0}, -1};
  Int soldier_ = 0;
  Int jet_actor_ = 0;
  Int chute_actor_ = 0;
  Jet jet_ = Jet{true, true, true, 1.0, 1.0, JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}, Jet_Mode::hover, 0.0, 0.0, 0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, true, false, true, 0, 0.0, JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}};
  List<TrafficCar> cars_ = List<TrafficCar>{};
  Mode mode_ = Mode::foot;
  /// the car the player drives
  Int car_index_ = -1;
  Bool chute_open_ = false;
  Float chute_time_ = 0.0;
  Vector chute_steer_ = Vector{0.0, 0.0, 0.0};
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Bool rig_init_ = false;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  Float frame_w_ = 960.0;
  Float frame_h_ = 540.0;
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<Int>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  List<PoliceUnit> units_ = List<PoliceUnit>{};
  Fixed<PoliceDrone, 5> pdrones_ = List<PoliceDrone>{};
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  /// pings, feed, voice, big map (game_net.h)
  List<MpLocal> mp_ = List<MpLocal>{};
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  Float time_ = 0.0;
  Rng rng_ = Rng{20261029, 0.0, false};
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;
  /// world.* switches that make the world (friendly fire is the rule voted on: game_rules_sync)
  KERN_CONST Fixed<Opt, 13> kWorldRules = List<Opt>{Opt::world_people, Opt::world_traffic, Opt::world_dogs, Opt::world_cats, Opt::world_birds, Opt::world_zombies, Opt::world_police, Opt::world_military, Opt::world_flak, Opt::world_jetpack, Opt::world_terrain, Opt::world_guided, Opt::world_always_day};
  /// The number of each value of net_SwarmFlag, in their order
  KERN_CONST List<Int> net_SwarmFlag_numbers = List<Int>{128, 1, 2, 4, 3};
  KERN_CONST Float kFallenMs = 1500.0;
  KERN_CONST Int kPlanksMax = 6;

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

  void traffic_advance(TrafficCar& c) {
    Int n = 0;
    Int L_at = 0;
    Vector end = Vector{0.0, 0.0, 0.0};
    Int next = 0;
    Int next_dir = 0;
    Int start_2 = 0;
    Int k = 0;
    Bool skip = false;
    Int j = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector z = Vector{0.0, 0.0, 0.0};
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Vector>(city_.lanes_[c.lane].points))));
    L_at = c.lane;
    c.index = kern::to<Int>(kern::formula([](Float index, Float dir) -> Float { return (index + dir); }, kern::to<Float>(c.index), kern::to<Float>(c.dir)));
    if (logic_and(math_compare<">=">(kern::to<Float>(c.index), 0.0), math_compare<"<">(kern::to<Float>(c.index), kern::to<Float>(n)))) {
      return;
    }
    end = city_.lanes_[L_at].points[kern::to<Int>(kern::formula([](Float index, Float n) -> Float { return ((index < 0.0) ? 0.0 : (n - 1.0)); }, kern::to<Float>(c.index), kern::to<Float>(n)))];
    next = -1;
    next_dir = 1;
    n1048605 = common::Rng_next(rng_);
    start_2 = n1048605.Result;
    k = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Lane>(city_.lanes_))), 0.5), math_compare<"<">(kern::to<Float>(next), 0.0)))) {
      skip = false;
      j = kern::to<Int>(kern::formula([](Float start_2, Float k, Float a) -> Float { return kern::math::mod((start_2 + k), a); }, kern::to<Float>(start_2), kern::to<Float>(k), kern::to<Float>(list_length<Lane>(city_.lanes_))));
      if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32s(j); }, kern::to<Float>(j))))), kern::to<Float>(c.lane)), logic_not(city_.lanes_[j].alive)), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[j].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        a = city_.lanes_[j].points[0];
        z = city_.lanes_[j].points[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(city_.lanes_[j].points))))];
        if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(a).x, vector_break(end).x, vector_break(a).y, vector_break(end).y), 4.0)) {
          next = kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32s(j); }, kern::to<Float>(j)));
          next_dir = 1;
        } else if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(z).x, vector_break(end).x, vector_break(z).y, vector_break(end).y), 4.0)) {
          next = kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32s(j); }, kern::to<Float>(j)));
          next_dir = -1;
        }
      }
      k += 1;
    }
    if (math_compare<">=">(kern::to<Float>(next), 0.0)) {
      c.lane = next;
      c.dir = next_dir;
      c.index = kern::to<Int>(kern::formula([](Float next_dir, Float a) -> Float { return ((next_dir > 0.0) ? 1.0 : (kern::math::wrap32s(a) - 2.0)); }, kern::to<Float>(next_dir), kern::to<Float>(list_length<Vector>(city_.lanes_[next].points))));
    } else {
      c.dir = kern::to<Int>(kern::formula([](Float dir) -> Float { return (-dir); }, kern::to<Float>(c.dir)));
      c.index = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index, Float dir) -> Float { return (index + (2.0 * dir)); }, kern::to<Float>(c.index), kern::to<Float>(c.dir))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))));
    }
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
    engine_game_world_physics(n1048606, world_);
    physics = n1048606.result;
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

  struct voice_mic_onResult { Bool Result = false; };
  KERN_PURE voice_mic_onResult voice_mic_on() {
    return {logic_and(logic_and(logic_and(math_compare<">">(kern::to<Float>(list_length<MpLocal>(mp_)), 0.0), mp_[0].mic), logic_not(mp_[0].muted)), net_session::Session_active(net_).Result)};
  }

  struct world_rule_lockedResult { Bool Result = false; };
  KERN_PURE world_rule_lockedResult world_rule_locked(Opt o = Opt::mouse_sensitivity) {
    List<Opt> list = List<Opt>{};
    Opt w = Opt::mouse_sensitivity;
    if (logic_or(logic_not(net_session::Session_active(net_).Result), net_session::Session_hosting(net_).Result)) {
      return {false};
    }
    list = kWorldRules;
    for (auto it2097162 = flow_for(n2097162, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Opt>(list))))); it2097162.next();) {
      w = list[n2097162.index];
      if (enum_equal<Opt>(w, o)) {
        return {true};
      }
    }
    return {false};
  }

  struct remote_on_segment_zonesResult { Bool Result = false; };
  remote_on_segment_zonesResult remote_on_segment_zones(Float& t_hit, mp_Zone& z, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0) {
    BodyPart part = BodyPart::none;
    n2097167 = person_hit::hit(t_hit, feet, yaw, speed, from, seg, 1.16, 1.0);
    part = n2097167.Result;
    z = logic_select<mp_Zone>(enum_equal<BodyPart>(part, BodyPart::head), mp_Zone::head, logic_select<mp_Zone>(enum_equal<BodyPart>(part, BodyPart::legs), mp_Zone::legs, mp_Zone::torso));
    return {logic_not(enum_equal<BodyPart>(part, BodyPart::none))};
  }

  struct project_screenResult { Bool Result = false; };
  project_screenResult project_screen(Float& x, Float& y, Float& depth, Vector p = Vector{0.0, 0.0, 0.0}) {
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float t = 0.0;
    Float aspect = 0.0;
    Float z = 0.0;
    Float nx = 0.0;
    Float ny = 0.0;
    Float l = 0.0;
    f = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{0.0, 0.0, 1.0});
    r = vector_make(kern::formula([](Float y, Float z, Float z_2, Float y_2) -> Float { return ((y * z) - (z_2 * y_2)); }, vector_break(f).y, vector_break(cam_.up).z, vector_break(f).z, vector_break(cam_.up).y), kern::formula([](Float z, Float x, Float x_2, Float z_2) -> Float { return ((z * x) - (x_2 * z_2)); }, vector_break(f).z, vector_break(cam_.up).x, vector_break(f).x, vector_break(cam_.up).z), kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return ((x * y) - (y_2 * x_2)); }, vector_break(f).x, vector_break(cam_.up).y, vector_break(f).y, vector_break(cam_.up).x));
    r = logic_select<Vector>(math_compare<">">(vector_length(r), 9.999999717180685e-10), vector_normalize(r), Vector{0.0, 0.0, 1.0});
    u = vector_make(kern::formula([](Float y, Float z, Float z_2, Float y_2) -> Float { return ((y * z) - (z_2 * y_2)); }, vector_break(r).y, vector_break(f).z, vector_break(r).z, vector_break(f).y), kern::formula([](Float z, Float x, Float x_2, Float z_2) -> Float { return ((z * x) - (x_2 * z_2)); }, vector_break(r).z, vector_break(f).x, vector_break(r).x, vector_break(f).z), kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return ((x * y) - (y_2 * x_2)); }, vector_break(r).x, vector_break(f).y, vector_break(r).y, vector_break(f).x));
    d = vector_sub(p, cam_.eye);
    depth = vector_dot(d, f);
    t = kern::formula([](Float fov) -> Float { return kern::math::tan(kern::math::div(fov, 2.0)); }, cam_.fov);
    aspect = kern::formula([](Float frame_w_, Float frame_h_) -> Float { return kern::math::div(frame_w_, kern::math::max(1.0, frame_h_)); }, frame_w_, frame_h_);
    z = kern::formula([](Float depth) -> Float { return ((kern::math::abs(depth) < 0.001) ? 0.001 : kern::math::abs(depth)); }, depth);
    nx = kern::formula([](Float a, Float z, Float t, Float aspect) -> Float { return kern::math::div(a, ((z * t) * aspect)); }, vector_dot(d, r), z, t, aspect);
    ny = kern::formula([](Float a, Float z, Float t) -> Float { return kern::math::div(a, (z * t)); }, vector_dot(d, u), z, t);
    if (math_compare<"<">(depth, 0.05)) {
      nx = vector_dot(d, r);
      ny = depth;
      l = kern::formula([](Float nx, Float ny) -> Float { return kern::math::sqrt((nx * nx) + (ny * ny)); }, nx, ny);
      if (math_compare<"<">(l, 0.001)) {
        nx = 0.0;
        ny = -1.0;
      } else {
        nx = kern::formula([](Float nx, Float l) -> Float { return (kern::math::div(nx, l) * 4.0); }, nx, l);
        ny = kern::formula([](Float ny, Float l) -> Float { return (kern::math::div(ny, l) * 4.0); }, ny, l);
      }
    }
    x = nx;
    y = ny;
    return {logic_and(logic_and(math_compare<">">(depth, 0.0), math_compare<"<">(kern::formula([](Float nx) -> Float { return kern::math::abs(nx); }, nx), 0.97)), math_compare<"<">(kern::formula([](Float ny) -> Float { return kern::math::abs(ny); }, ny), 0.93))};
  }

  void swarm_clear() {
    for (auto it3145738 = flow_for(n3145738, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it3145738.next();) {
      for (auto it3145739 = flow_for(n3145739, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n3145738.index].units))))); it3145739.next();) {
        if (math_compare<">=">(kern::to<Float>(swarms_[n3145738.index].units[n3145739.index].person), 0.0)) {
          people_.people_[swarms_[n3145738.index].units[n3145739.index].person].state = People_State::gone;
        }
      }
    }
    map_clear(swarms_);
    list_clear(swarm_sent_);
    list_clear(swarm_fallen_);
  }

  struct Game_swarm_collect_next_idResult { Int Result = 0; };
  Game_swarm_collect_next_idResult Game_swarm_collect_next_id() {
    swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return kern::math::band((swarm_serial_ + 1.0), 65535.0); }, kern::to<Float>(swarm_serial_)));
    if (math_compare<"==">(kern::to<Float>(swarm_serial_), 0.0)) {
      swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return kern::math::band((swarm_serial_ + 1.0), 65535.0); }, kern::to<Float>(swarm_serial_)));
    }
    return {swarm_serial_};
  }

  struct swarm_collect_matchResult { Bool Result = false; };
  KERN_PURE swarm_collect_matchResult swarm_collect_match(const Game_SwarmSent& swarm_sent_n3145756_index, const Game_SwarmSent& s) {
    return {logic_and(math_compare<"==">(kern::to<Float>(s.unit.id), kern::to<Float>(swarm_sent_n3145756_index.unit.id)), enum_equal<net_SwarmKind>(s.unit.kind, swarm_sent_n3145756_index.unit.kind))};
  }

  struct swarm_collect_match_2Result { Bool Result = false; };
  KERN_PURE swarm_collect_match_2Result swarm_collect_match_2(const Game_SwarmFallen& f, Float now = 0.0) {
    return {math_compare<">=">(now, f.until)};
  }

  void swarm_collect(List<net_SwarmUnit>& out) {
    Float st = 0.0;
    Float sim_time = 0.0;
    Float now = 0.0;
    List<Game_SwarmSent> sent = List<Game_SwarmSent>{};
    Bool skip = false;
    Bool skip_2 = false;
    Bool skip_3 = false;
    Bool skip_4 = false;
    Int anim = 0;
    Bool skip_5 = false;
    Bool skip_6 = false;
    Int p_at = 0;
    Int flags = 0;
    Bool skip_7 = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector f = Vector{0.0, 0.0, 0.0};
    Bool lights = false;
    Int flags_2 = 0;
    Bool skip_8 = false;
    Int p_at_2 = 0;
    Int flags_3 = 0;
    Bool skip_9 = false;
    Int at = 0;
    Int end = 0;
    Bool go = false;
    net_SwarmUnit u = net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    Int at_2 = 0;
    Int kept = 0;
    Int end_2 = 0;
    sim_time = engine_game_world_sim_time(world_);
    st = sim_time;
    now = kern::formula([](Float time_) -> Float { return (time_ * 1000.0); }, time_);
    sent = List<Game_SwarmSent>{};
    for (auto it3145740 = flow_for(n3145740, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<ZombieAi>(combat_.zombie_ai))))); it3145740.next();) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(combat_.zombie_ai[n3145740.index].person), 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.zombie_ai[n3145740.index].id), 0.0))) {
          n3145741 = Game_swarm_collect_next_id();
          combat_.zombie_ai[n3145740.index].id = n3145741.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.zombie_ai[n3145740.index].id, net_SwarmKind::zombie, kern::to<Int>(kern::formula([](Float st, Float a) -> Float { return kern::math::band((((st - a) < 0.6) ? 1.0 : 0.0), 255.0); }, st, combat_.zombie_ai[n3145740.index].swing)), people_.people_[combat_.zombie_ai[n3145740.index].person].pos, common::wrap_pi(people_.people_[combat_.zombie_ai[n3145740.index].person].yaw).Result, people_.people_[combat_.zombie_ai[n3145740.index].person].v));
      }
    }
    for (auto it3145742 = flow_for(n3145742, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Heli>(combat_.helis))))); it3145742.next();) {
      skip_2 = false;
      if (combat_.helis[n3145742.index].dead) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.helis[n3145742.index].id), 0.0))) {
          n3145743 = Game_swarm_collect_next_id();
          combat_.helis[n3145742.index].id = n3145743.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.helis[n3145742.index].id, net_SwarmKind::heli, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(enum_equal<Heli_State>(combat_.helis[n3145742.index].state, Heli_State::down)))), combat_.helis[n3145742.index].p, common::wrap_pi(combat_.helis[n3145742.index].yaw).Result, 0.0));
      }
    }
    for (auto it3145744 = flow_for(n3145744, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tank>(combat_.tanks))))); it3145744.next();) {
      skip_3 = false;
      if (combat_.tanks[n3145744.index].dead) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.tanks[n3145744.index].id), 0.0))) {
          n3145745 = Game_swarm_collect_next_id();
          combat_.tanks[n3145744.index].id = n3145745.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.tanks[n3145744.index].id, net_SwarmKind::tank, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(combat_.tanks[n3145744.index].wreck))), combat_.tanks[n3145744.index].p, common::wrap_pi(combat_.tanks[n3145744.index].yaw).Result, common::wrap_pi(combat_.tanks[n3145744.index].turret).Result));
      }
    }
    for (auto it3145746 = flow_for(n3145746, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<SoldierAi>(combat_.soldiers))))); it3145746.next();) {
      skip_4 = false;
      if (math_compare<"<">(kern::to<Float>(combat_.soldiers[n3145746.index].person), 0.0)) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.soldiers[n3145746.index].id), 0.0))) {
          n3145747 = Game_swarm_collect_next_id();
          combat_.soldiers[n3145746.index].id = n3145747.Result;
        }
        if (logic_or(enum_equal<People_State>(people_.people_[combat_.soldiers[n3145746.index].person].state, People_State::down), enum_equal<People_State>(people_.people_[combat_.soldiers[n3145746.index].person].state, People_State::gone))) {
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          anim = kern::to<Int>(kern::formula([](Float a, Float st, Float a_2, Float a_3) -> Float { return kern::math::band((((a != 0.0) || ((st - a_2) < 0.5)) ? 1.0 : ((a_3 > 3.0) ? 2.0 : ((a_3 > 0.0) ? 3.0 : 0.0))), 255.0); }, kern::to<Float>(combat_.soldiers[n3145746.index].los), st, combat_.soldiers[n3145746.index].last_shot, people_.people_[combat_.soldiers[n3145746.index].person].v));
          list_add(out, struct_make<net_SwarmUnit>(combat_.soldiers[n3145746.index].id, net_SwarmKind::soldier, anim, people_.people_[combat_.soldiers[n3145746.index].person].pos, common::wrap_pi(people_.people_[combat_.soldiers[n3145746.index].person].yaw).Result, people_.people_[combat_.soldiers[n3145746.index].person].v));
        }
      }
    }
    for (auto it3145748 = flow_for(n3145748, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it3145748.next();) {
      skip_5 = false;
      if (math_compare<"<">(kern::to<Float>(units_[n3145748.index].car), 0.0)) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        for (auto it3145749 = flow_for(n3145749, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit_Cop>(units_[n3145748.index].cops))))); it3145749.next();) {
          skip_6 = false;
          if (logic_or(math_compare<"<">(kern::to<Float>(units_[n3145748.index].cops[n3145749.index].person), 0.0), units_[n3145748.index].cops[n3145749.index].inside)) {
            skip_6 = true;
          }
          if (logic_not(skip_6)) {
            p_at = units_[n3145748.index].cops[n3145749.index].person;
            if (logic_or(logic_or(math_compare<"!=">(kern::to<Float>(people_.people_[p_at].seed), kern::to<Float>(units_[n3145748.index].cops[n3145749.index].seed)), enum_equal<People_State>(people_.people_[p_at].state, People_State::down)), enum_equal<People_State>(people_.people_[p_at].state, People_State::gone))) {
              skip_6 = true;
            }
            if (logic_not(skip_6)) {
              if (logic_not(math_compare<"!=">(kern::to<Float>(units_[n3145748.index].cops[n3145749.index].id), 0.0))) {
                n3145750 = Game_swarm_collect_next_id();
                units_[n3145748.index].cops[n3145749.index].id = n3145750.Result;
              }
              flags = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::band(kern::math::bor(kern::math::wrap32(((a != 0.0) ? a_2 : 0.0)), kern::math::wrap32(kern::math::shl(kern::math::mod(a_3, 5.0), a_2))), 255.0); }, kern::to<Float>(people_.people_[p_at].aiming), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))), kern::to<Float>(units_[n3145748.index].cops[n3145749.index].seed)));
              list_add(out, struct_make<net_SwarmUnit>(units_[n3145748.index].cops[n3145749.index].id, net_SwarmKind::officer, flags, people_.people_[p_at].pos, common::wrap_pi(people_.people_[p_at].yaw).Result, people_.people_[p_at].v));
              list_add(sent, struct_make<Game_SwarmSent>(out[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_SwarmUnit>(out))))], units_[n3145748.index].cops[n3145749.index].person, people_.people_[p_at].seed));
            }
          }
        }
      }
    }
    for (auto it3145751 = flow_for(n3145751, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it3145751.next();) {
      skip_7 = false;
      either = logic_or(logic_not(cars_[n3145751.index].police), logic_not(cars_[n3145751.index].placed));
      if (logic_not(either)) {
        actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n3145751.index].actor);
        either = logic_not(actor_alive_2);
      }
      if (logic_or(either, logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n3145751.index))))))))) {
        skip_7 = true;
      }
      if (logic_not(skip_7)) {
        n3145752 = state_of(cars_[n3145751.index].actor);
        s = n3145752.Result;
        f = engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result;
        lights = logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[n3145751.index].unit), 0.0), math_compare<"<">(kern::to<Float>(cars_[n3145751.index].unit), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<PoliceUnit>(units_)))))))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n3145751.index].unit].state, PoliceUnit_State::abandoned))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n3145751.index].unit].state, PoliceUnit_State::wreck))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n3145751.index].unit].state, PoliceUnit_State::leave)));
        flags_2 = kern::to<Int>(kern::formula([](Float lights, Float a, Float a_2, Float a_3, Float a_4, Float a_5) -> Float { return kern::math::band(kern::math::bor(kern::math::bor(((lights != 0.0) ? a : 0.0), ((a_2 != 0.0) ? a_3 : 0.0)), ((a_4 >= 0.0) ? a_5 : 0.0)), 255.0); }, kern::to<Float>(lights), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))), kern::to<Float>(cars_[n3145751.index].wreck), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck))), cars_[n3145751.index].burn_until, kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserBurns)))));
        list_add(out, struct_make<net_SwarmUnit>(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band((index + 1.0), 65535.0); }, kern::to<Float>(n3145751.index))), net_SwarmKind::cruiser, flags_2, s.position, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(f).y, vector_break(f).x), car::Car_steer_angle(cars_[n3145751.index].car[0]).Result));
      }
    }
    for (auto it3145753 = flow_for(n3145753, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it3145753.next();) {
      if (pdrones_[n3145753.index].active) {
        list_add(out, struct_make<net_SwarmUnit>(game_swarm_cpp::drone_id(pdrones_[n3145753.index]).Result, net_SwarmKind::police_drone, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(((a != 0.0) ? a_2 : 0.0), 255.0); }, kern::to<Float>(pdrones_[n3145753.index].emp), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))))), pdrones_[n3145753.index].pos, common::wrap_pi(pdrones_[n3145753.index].yaw).Result, 0.0));
      }
    }
    for (auto it3145754 = flow_for(n3145754, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<EnemyAi>(combat_.enemies))))); it3145754.next();) {
      skip_8 = false;
      if (math_compare<"<">(kern::to<Float>(combat_.enemies[n3145754.index].person), 0.0)) {
        skip_8 = true;
      }
      if (logic_not(skip_8)) {
        p_at_2 = combat_.enemies[n3145754.index].person;
        if (logic_or(logic_or(logic_not(enum_equal<People_Kind>(people_.people_[p_at_2].kind, People_Kind::enemy)), enum_equal<People_State>(people_.people_[p_at_2].state, People_State::down)), enum_equal<People_State>(people_.people_[p_at_2].state, People_State::gone))) {
          skip_8 = true;
        }
        if (logic_not(skip_8)) {
          if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.enemies[n3145754.index].id), 0.0))) {
            n3145755 = Game_swarm_collect_next_id();
            combat_.enemies[n3145754.index].id = n3145755.Result;
          }
          flags_3 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::band(kern::math::bor(kern::math::band(a, a_2), ((a_3 != 0.0) ? a_4 : 0.0)), 255.0); }, kern::to<Float>(enum_index<combat_EnemyType>(combat_.enemies[n3145754.index].type)), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kEnemyTypeMask))), kern::to<Float>(people_.people_[p_at_2].aiming), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserBurns)))));
          list_add(out, struct_make<net_SwarmUnit>(combat_.enemies[n3145754.index].id, net_SwarmKind::enemy, flags_3, people_.people_[p_at_2].pos, common::wrap_pi(people_.people_[p_at_2].yaw).Result, people_.people_[p_at_2].v));
          list_add(sent, struct_make<Game_SwarmSent>(out[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_SwarmUnit>(out))))], combat_.enemies[n3145754.index].person, people_.people_[p_at_2].seed));
        }
      }
    }
    for (auto it3145756 = flow_for(n3145756, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_SwarmSent>(swarm_sent_))))); it3145756.next();) {
      skip_9 = false;
      at = 0;
      end = list_length<Game_SwarmSent>(sent);
      go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      while (loop(go)) {
        if (swarm_collect_match(swarm_sent_[n3145756.index], sent[at]).Result) {
          go = false;
        } else {
          at += 1;
          go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
        }
      }
      if (logic_not(math_compare<"==">(kern::to<Float>(at), kern::to<Float>(end)))) {
        skip_9 = true;
      }
      if (logic_not(skip_9)) {
        if (logic_or(math_compare<"<">(kern::to<Float>(swarm_sent_[n3145756.index].person), 0.0), math_compare<">=">(kern::to<Float>(swarm_sent_[n3145756.index].person), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(people_.people_))))))))) {
          skip_9 = true;
        }
        if (logic_not(skip_9)) {
          if (logic_or(math_compare<"!=">(kern::to<Float>(people_.people_[swarm_sent_[n3145756.index].person].seed), kern::to<Float>(swarm_sent_[n3145756.index].seed)), logic_not(enum_equal<People_State>(people_.people_[swarm_sent_[n3145756.index].person].state, People_State::down)))) {
            skip_9 = true;
          }
          if (logic_not(skip_9)) {
            u = swarm_sent_[n3145756.index].unit;
            u.flags = kern::to<Int>(kern::formula([](Float flags, Float a) -> Float { return kern::math::band(kern::math::bor(flags, a), 255.0); }, kern::to<Float>(u.flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kUnitDead)))));
            u.p = people_.people_[swarm_sent_[n3145756.index].person].pos;
            u.aux = 0.0;
            list_add(swarm_fallen_, struct_make<Game_SwarmFallen>(u, kern::formula([](Float now, Float kFallenMs) -> Float { return (now + kFallenMs); }, now, kFallenMs)));
          }
        }
      }
    }
    swarm_sent_ = sent;
    at_2 = 0;
    kept = at_2;
    end_2 = list_length<Game_SwarmFallen>(swarm_fallen_);
    while (loop(math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2)))) {
      if (logic_not(swarm_collect_match_2(swarm_fallen_[at_2], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at_2))) {
          var_take(n3145757, swarm_fallen_[at_2]);
          swarm_fallen_[kept] = n3145757.value;
        }
        kept += 1;
      }
      at_2 += 1;
    }
    list_resize(swarm_fallen_, kept, Game_SwarmFallen{net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0});
    for (auto it3145758 = flow_for(n3145758, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_SwarmFallen>(swarm_fallen_))))); it3145758.next();) {
      list_add(out, swarm_fallen_[n3145758.index].unit);
    }
  }

  void townsync_ready() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<TownSync>(townsync_)), 0.0))) {
      townsync_ = list_make<TownSync, "1">(TownSync{1, false, false, false, List<Int>{}, List<town_LampRec>{}, List<town_MarkRec>{}, List<town_MarkRec>{}, List<Int>{0, 0, 0}, 0, 0, 0, 0});
    }
  }

  struct town_car_remoteResult { Bool Result = false; };
  KERN_PURE town_car_remoteResult town_car_remote(Int car = 0) {
    return {logic_and(logic_and(logic_and(logic_and(puppet_world_, math_compare<">=">(kern::to<Float>(car), 0.0)), math_compare<">">(kern::formula([](Float car, Float a) -> Float { return ((car < 0.0) != (a < 0.0) ? (a < 0.0) : (car < a)); }, kern::to<Float>(car), kern::to<Float>(list_length<Int>(car_net_))), 0.5)), math_compare<">=">(kern::to<Float>(car_net_[car]), 0.0)), logic_not(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car), kern::to<Float>(car_index_)))))};
  }

  void townsync_lamp(Int64 key = Int64{0x0}, Int state = 0) {
    if (net_session::Session_active(net_).Result) {
      townsync_ready();
      list_add(townsync_[0].lamps_out, struct_make<town_LampRec>(key, state));
    }
  }

  void Game_townsync_update_set_planks(const town_PlankRec& r) {
    Int64 key = Int64{0x0};
    Int st_row = 0;
    Int count = 0;
    Int fill = 0;
    if (math_compare<"!=">(kern::to<Float>(list_length<Int>(gaps_[0].planks[st_row])), kern::to<Float>(r.doors))) {
      count = r.doors;
      fill = kPlanksMax;
      list_clear(gaps_[0].planks[st_row]);
      list_resize(gaps_[0].planks[st_row], count, fill);
    }
    gaps_[0].planks[st_row][r.door] = r.planks;
  }

  struct townsync_jsonResult { Text Result = ""; };
  townsync_jsonResult townsync_json() {
    Int open_doors = 0;
    Int n = 0;
    Text b = "";
    townsync_ready();
    open_doors = 0;
    if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
      for (auto it4194317 = flow_for(n4194317, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks))))); it4194317.next();) {
        for (auto it4194318 = flow_for(n4194318, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(gaps_[0].planks[n4194317.index]))))); it4194318.next();) {
          n = gaps_[0].planks[n4194317.index][n4194318.index];
          open_doors += kern::to<Int>(math_compare<"<">(kern::to<Float>(n), kern::to<Float>(kPlanksMax)));
        }
      }
    }
    b = "";
    b = text_format<"{\"epoch\":%u,\"lamps\":%zu,\"doors\":%zu,\"marks\":%zu,\"carHits\":[%u,%u,%u],\"personHits\":%u,\"sent\":%u,\"got\":%u,\"asks\":%u,\"refused\":%u}">(255, townsync_[0].epoch, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? a_2 : 0.0); }, kern::to<Float>(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), kern::to<Float>(lamps::Field_broken(gaps_[0].lamps).Result))), open_doors, list_length<town_MarkRec>(townsync_[0].marks), townsync_[0].car_hits[0], townsync_[0].car_hits[1], townsync_[0].car_hits[2], townsync_[0].person_hits, townsync_[0].sent_states, townsync_[0].got_states, townsync_[0].asks, net_session::Session_send_refused(net_).Result);
    return {b};
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n1048586;
  struct : kern::Node<1048591> { Int index = 0; } n1048591;
  struct { Bool result = false; } n1048595;
  state_ofResult n1048600;
  struct { Bool result = false; } n1048601;
  struct { Bool result = false; } n1048602;
  common::Rng_nextResult n1048605;
  struct { Int result = 0; } n1048606;
  struct : kern::Node<2097162> { Int index = 0; } n2097162;
  person_hit::hitResult n2097167;
  struct : kern::Node<3145738> { Int index = 0; } n3145738;
  struct : kern::Node<3145739> { Int index = 0; } n3145739;
  struct : kern::Node<3145740> { Int index = 0; } n3145740;
  Game_swarm_collect_next_idResult n3145741;
  struct : kern::Node<3145742> { Int index = 0; } n3145742;
  Game_swarm_collect_next_idResult n3145743;
  struct : kern::Node<3145744> { Int index = 0; } n3145744;
  Game_swarm_collect_next_idResult n3145745;
  struct : kern::Node<3145746> { Int index = 0; } n3145746;
  Game_swarm_collect_next_idResult n3145747;
  struct : kern::Node<3145748> { Int index = 0; } n3145748;
  struct : kern::Node<3145749> { Int index = 0; } n3145749;
  Game_swarm_collect_next_idResult n3145750;
  struct : kern::Node<3145751> { Int index = 0; } n3145751;
  state_ofResult n3145752;
  struct : kern::Node<3145753> { Int index = 0; } n3145753;
  struct : kern::Node<3145754> { Int index = 0; } n3145754;
  Game_swarm_collect_next_idResult n3145755;
  struct : kern::Node<3145756> { Int index = 0; } n3145756;
  struct { Game_SwarmFallen value = Game_SwarmFallen{net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0}; } n3145757;
  struct : kern::Node<3145758> { Int index = 0; } n3145758;
  struct : kern::Node<4194317> { Int index = 0; } n4194317;
  struct : kern::Node<4194318> { Int index = 0; } n4194318;
};
