struct Game : virtual kern::Blueprint, virtual city_stream, virtual common, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual game, virtual game_controls, virtual game_gaps, virtual game_weapons_cpp, virtual jet, virtual net_session, virtual people {
  // ---- variables ----
  List<GapsState> gaps_ = List<GapsState>{};
  Int engine_ = 0;
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
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  Fixed<PoliceDrone, 5> pdrones_ = List<PoliceDrone>{};
  weapons_Id foot_weapon_ = weapons_Id::glock;
  Fixed<Int, 16> ammo_mag_ = List<Int>{};
  Fixed<Int, 16> ammo_reserve_ = List<Int>{};
  Int grenades_ = 4;
  Float next_attack_ = 0.0;
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  Rng rng_ = Rng{20261029, 0.0, false};
  Int bank_ = 0;
  KERN_CONST Float kRespawnCooldown = 3.0;
  KERN_CONST Float kRad = 57.29577951308238;
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;
  /// Magazines and spare rounds on foot (gameplay_final_runtime_v2.mjs WEAPONS).
  KERN_CONST Fixed<Loadout, 7> kFoot = List<Loadout>{Loadout{weapons_Id::smg, 30, 150}, Loadout{weapons_Id::glock, 17, 85}, Loadout{weapons_Id::sniper, 5, 25}, Loadout{weapons_Id::grenade_launcher, 6, 18}, Loadout{weapons_Id::rocket, 1, 6}, Loadout{weapons_Id::fists, -1, -1}, Loadout{weapons_Id::knife, -1, -1}};
  /// SpawnCity's numbers. Damage of the hitscan guns is SpawnCity's multiplayer damage (VS_WEAPON_DAMAGE smg 13, glock 26, sniper 95) times the zone (head 3, torso 1, legs .65).
  KERN_CONST Fixed<weapons_Spec, 14> kTable = List<weapons_Spec>{weapons_Spec{"MP", 55.0, 180.0, 13.0, 0.0, 240.0, 1}, weapons_Spec{"Glock", 110.0, 240.0, 26.0, 0.0, 0.0, 2}, weapons_Spec{"Sniper .50", 1150.0, 1500.0, 95.0, 0.0, 850.0, 3}, weapons_Spec{"Granatwerfer 40 mm", 760.0, 300.0, 125.0, 8.5, 40.0, 5}, weapons_Spec{"Raketenwerfer", 900.0, 420.0, 160.0, 9.5, 40.0, 6}, weapons_Spec{"Faeuste", 300.0, 1.8, 24.0, 0.0, 0.0, 7}, weapons_Spec{"Messer", 380.0, 1.6, 45.0, 0.0, 0.0, 8}, weapons_Spec{"Handgranate", 4000.0, 40.0, 125.0, 8.5, 0.0, 0}, weapons_Spec{"Drohnen-MG", 92.0, 650.0, 13.0, 0.0, 0.0, 0}, weapons_Spec{"Drohnen-Raketen", 950.0, 220.0, 100.0, 8.0, 31.0, 0}, weapons_Spec{"GUIDED", 5000.0, 600.0, 34.0, 4.0, 34.0, 0}, weapons_Spec{"Bordkanone", 55.0, 1800.0, 30.0, 0.0, 0.0, 0}, weapons_Spec{"Luft-Rakete", 2400.0, 1300.0, 120.0, 8.0, 260.0, 0}, weapons_Spec{"Bombe", 450.0, 0.0, 170.0, 17.0, 0.0, 0}};

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

  struct Game_ctl_discrete_phResult { Bool Result = false; };
  Game_ctl_discrete_phResult Game_ctl_discrete_ph(Int b = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pad_hit, Float b) -> Float { return kern::math::band(pad_hit, kern::math::wrap32(kern::math::shl(1.0, b))); }, kern::to<Float>(ctl_.pad_hit), kern::to<Float>(b))))), 0.0)};
  }

  struct Game_ctl_assist_pointResult { Bool Result = false; };
  Game_ctl_assist_pointResult Game_ctl_assist_point(Vector& p, Int kind = 0, Int i = 0) {
    if (math_compare<"==">(kern::to<Float>(kind), 1.0)) {
      if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(people_.people_)))))))), enum_equal<People_State>(people_.people_[i].state, People_State::down)), enum_equal<People_State>(people_.people_[i].state, People_State::gone))) {
        return {false};
      }
      p = vector_add(people_.people_[i].pos, Vector{0.0, 0.0, 1.24});
      return {true};
    }
    if (math_compare<"==">(kern::to<Float>(kind), 2.0)) {
      if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), 5.0)), logic_not(pdrones_[i].active))) {
        return {false};
      }
      p = pdrones_[i].pos;
      return {true};
    }
    if (math_compare<"==">(kern::to<Float>(kind), 3.0)) {
      if (logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))))))) {
        return {false};
      }
      p = logic_select<Vector>(enum_equal<net_PlayerMode>(net_.remotes_[i].state.mode, net_PlayerMode::foot), vector_add(net_.remotes_[i].person_shown, Vector{0.0, 0.0, 1.24}), net_.remotes_[i].shown);
      return {true};
    }
    return {false};
  }

  void ctl_assist_errors(Float& ye, Float& pe, Float& d, Vector eye = Vector{0.0, 0.0, 0.0}, Float fyaw = 0.0, Float fpitch = 0.0, Vector p = Vector{0.0, 0.0, 0.0}) {
    Vector to = Vector{0.0, 0.0, 0.0};
    to = vector_sub(p, eye);
    d = vector_length(to);
    ye = kern::formula([](Float the_result) -> Float { return (-the_result); }, common::wrap_pi(kern::formula([](Float y, Float x, Float fyaw) -> Float { return (kern::math::atan2(y, x) - fyaw); }, vector_break(to).y, vector_break(to).x, fyaw)).Result);
    pe = kern::formula([](Float a, Float fpitch) -> Float { return (kern::math::asin(a) - fpitch); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float z, Float d) -> Float { return kern::math::div(z, kern::math::max(d, 0.001)); }, vector_break(to).z, d), -1.0, 1.0), fpitch);
  }

  void ctl_camera(Camera& c) {
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Float yaw = 0.0;
    Float pitch = 0.0;
    Vector turned = Vector{0.0, 0.0, 0.0};
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    Vector side = Vector{0.0, 0.0, 0.0};
    Vector out = Vector{0.0, 0.0, 0.0};
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    if (logic_or(enum_equal<Mode>(mode_, Mode::foot), enum_equal<Mode>(mode_, Mode::car))) {
      return;
    }
    both = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
    if (both) {
      xr_2 = engine_engine_xr(engine_);
      both = xr_2;
    }
    if (both) {
      return;
    }
    if (logic_and(math_compare<">">(kern::formula([](Float yaw_deg) -> Float { return (yaw_deg == 0.0); }, ctl_.look.yaw_deg), 0.5), math_compare<">">(kern::formula([](Float pitch_deg) -> Float { return (pitch_deg == 0.0); }, ctl_.look.pitch_deg), 0.5))) {
      return;
    }
    d = vector_sub(c.target, c.eye);
    len = vector_length(d);
    if (math_compare<"<">(len, 0.0001)) {
      return;
    }
    f = vector_scale(d, kern::formula([](Float len) -> Float { return kern::math::div(1.0, len); }, len));
    yaw = kern::formula([](Float yaw_deg, Float kRad) -> Float { return kern::math::div((-yaw_deg), kRad); }, ctl_.look.yaw_deg, kRad);
    pitch = kern::formula([](Float pitch_deg, Float kRad) -> Float { return kern::math::div(pitch_deg, kRad); }, ctl_.look.pitch_deg, kRad);
    a = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, yaw).Result;
    turned = engine_math_math::Quat_rotate(a, f).Result;
    side = vector_cross(turned, Vector{0.0, 0.0, 1.0});
    side = logic_select<Vector>(math_compare<">">(vector_length(side), 9.999999717180685e-10), vector_normalize(side), Vector{1.0, 0.0, 0.0});
    a_2 = engine_math_math::Quat_axis_angle(side, pitch).Result;
    out = engine_math_math::Quat_rotate(a_2, turned).Result;
    c.target = vector_add(c.eye, vector_scale(out, len));
  }

  struct ctl_command_startsResult { Bool Result = false; };
  KERN_PURE ctl_command_startsResult ctl_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct matrix_ofResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  matrix_ofResult matrix_of(Int a = 0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool actor_matrix = false;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    engine_game_world_actor_matrix(n2097162, world_, a, m);
    actor_matrix = n2097162.result;
    m = n2097162.out;
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
    for (auto it2097167 = flow_for(n2097167, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it2097167.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n2097167.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n2097167.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n2097167.index].points))), 0.5))) {
          a = city_.lanes_[n2097167.index].points[k];
          b = city_.lanes_[n2097167.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n2097167.index].width))) {
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
    engine_game_world_set_mode(n2097171, world_, game_SimMode::simulate);
    set_mode = n2097171.result;
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n2097177 = state_of(jet_actor_);
    s = n2097177.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n2097178, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n2097178.result;
    engine_game_world_set_actor_velocity(n2097179, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n2097179.result;
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
    n2097182 = common::Rng_next(rng_);
    start_2 = n2097182.Result;
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
    engine_game_world_physics(n2097183, world_);
    physics = n2097183.result;
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

  void gaps_init_make_part(const MeshData& disc, GapsState_Part& p, Int e = 0, Bool ember = false) {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Int renderer = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int renderer_2 = 0;
    Bool upsert = false;
    Int renderer_3 = 0;
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    m.alpha = AlphaMode::blend;
    m.unlit = true;
    m.double_sided = true;
    m.cast_shadow = false;
    if (ember) {
      m.emissive = Vector{1.0, 0.54, 0.24};
    }
    renderer = engine_engine_renderer(e);
    engine_renderer_allocate_id(n3145739, renderer);
    allocate_id = n3145739.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_2 = engine_engine_renderer(e);
    engine_upsert(n3145740, renderer_2, p.id, disc, look, parked.m, List<Float>{});
    upsert = n3145740.result;
    if (logic_not(upsert)) {
      renderer_3 = engine_engine_renderer(e);
      engine_renderer_release_id(renderer_3, p.id);
      p.id = 0;
    }
  }

  void gaps_guided_sounds() {
    Int rate = 0;
    List<Float> lock = List<Float>{};
    List<Float> made = List<Float>{};
    List<Float> bang = List<Float>{};
    List<Float> made_2 = List<Float>{};
    Int n = 0;
    Float ph = 0.0;
    Int i = 0;
    Float t = 0.0;
    Bool set_pcm = false;
    Bool set_pcm_2 = false;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), gaps_[0].guided_sounds), logic_not(math_compare<"!=">(kern::to<Float>(bank_), 0.0)))) {
      return;
    }
    rate = 44100;
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float rate) -> Float { return kern::math::trunc((rate * 0.15)); }, kern::to<Float>(rate))), 0.0);
    lock = made;
    made_2 = List<Float>{};
    list_resize(made_2, kern::to<Int>(kern::formula([](Float rate) -> Float { return kern::math::trunc((rate * 0.4)); }, kern::to<Float>(rate))), 0.0);
    bang = made_2;
    for (auto it3145741 = flow_for(n3145741, 0, 1); it3145741.next();) {
      for (auto it3145742 = flow_for(n3145742, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 0.05); }, kern::to<Float>(rate)))))))); it3145742.next();) {
        n = kern::to<Int>(kern::formula([](Float index, Float rate, Float index_2) -> Float { return (kern::math::trunc(((index * 0.09) * rate)) + index_2); }, kern::to<Float>(n3145741.index), kern::to<Float>(rate), kern::to<Float>(n3145742.index)));
        if (math_compare<">">(kern::formula([](Float n, Float a) -> Float { return ((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(lock))), 0.5)) {
          lock[n] = kern::formula([](Float index, Float rate) -> Float { return ((kern::math::mod(kern::math::div((index * 1760.0), rate), 1.0) < 0.5) ? 0.05 : (-0.05)); }, kern::to<Float>(n3145742.index), kern::to<Float>(rate));
        }
      }
    }
    ph = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(bang))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      ph += kern::formula([](Float t, Float rate) -> Float { return kern::math::div((110.0 * kern::math::pow(kern::math::div(40.0, 110.0), kern::math::min(1.0, kern::math::div(t, 0.3)))), rate); }, t, kern::to<Float>(rate));
      bang[i] = kern::formula([](Float ph, Float t) -> Float { return ((kern::math::sin(((2.0 * 3.14159265358979) * ph)) * 0.6) * kern::math::pow(kern::math::div(0.001, 0.6), kern::math::min(1.0, kern::math::div(t, 0.35)))); }, ph, t);
      i += 1;
    }
    engine_sound_bank_request(bank_, "sc:locktone");
    engine_sound_bank_set_pcm(n3145743, bank_, "sc:locktone", lock, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(lock)))), 1, rate);
    set_pcm = n3145743.result;
    engine_sound_bank_request(bank_, "sc:guidedbang");
    engine_sound_bank_set_pcm(n3145744, bank_, "sc:guidedbang", bang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(bang)))), 1, rate);
    set_pcm_2 = n3145744.result;
    gaps_[0].guided_sounds = true;
  }

  struct gaps_command_startsResult { Bool Result = false; };
  KERN_PURE gaps_command_startsResult gaps_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  void init_weapons() {
    List<Loadout> list = List<Loadout>{};
    list = kFoot;
    for (auto it4194314 = flow_for(n4194314, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Loadout>(list))))); it4194314.next();) {
      ammo_mag_[enum_index<weapons_Id>(list[n4194314.index].id)] = list[n4194314.index].magazine;
      ammo_reserve_[enum_index<weapons_Id>(list[n4194314.index].id)] = list[n4194314.index].reserve;
    }
    grenades_ = 4;
    foot_weapon_ = weapons_Id::glock;
  }

  void select_foot_weapon(weapons_Id id = weapons_Id::smg) {
    game_SoldierState st = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0};
    Bool soldier_state = false;
    weapons_Spec s = weapons_Spec{"", 0.0, 0.0, 0.0, 0.0, 0.0, 0};
    Text kind = "";
    Bool set_property_text = false;
    Bool set_property_number = false;
    Bool set_property_number_2 = false;
    Bool set_property_flag = false;
    Bool set_property_number_3 = false;
    Bool set_property_number_4 = false;
    List<Loadout> list = List<Loadout>{};
    Bool set_property_number_5 = false;
    Bool set_property_text_2 = false;
    Bool set_property_number_6 = false;
    Bool set_property_number_7 = false;
    Bool set_property_number_8 = false;
    Bool set_property_number_9 = false;
    Bool set_property_number_10 = false;
    Bool soldier_state_2 = false;
    Float sim_time = 0.0;
    if (enum_equal<weapons_Id>(id, foot_weapon_)) {
      return;
    }
    st = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0};
    engine_game_world_soldier_state(n4194315, world_, soldier_, st);
    soldier_state = n4194315.result;
    st = n4194315.out;
    if (logic_and(logic_and(soldier_state, game_weapons_cpp::hitscan_on_foot(foot_weapon_).Result), math_compare<">=">(kern::to<Float>(st.ammo), 0.0))) {
      ammo_mag_[enum_index<weapons_Id>(foot_weapon_)] = st.ammo;
      ammo_reserve_[enum_index<weapons_Id>(foot_weapon_)] = st.reserve;
    }
    foot_weapon_ = id;
    s = list_get<weapons_Spec>(kTable, enum_index<weapons_Id>(id));
    kind = logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::smg), "rifle", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::sniper), "sniper", logic_select<Text>(logic_or(enum_equal<weapons_Id>(id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(id, weapons_Id::rocket)), "shotgun", "pistol")));
    engine_game_world_set_property_text(n4194316, world_, soldier_, "components.weapon.kind", kind);
    set_property_text = n4194316.result;
    if (game_weapons_cpp::hitscan_on_foot(id).Result) {
      engine_game_world_set_property_number(n4194317, world_, soldier_, "components.weapon.damage", s.damage);
      set_property_number = n4194317.result;
      engine_game_world_set_property_number(n4194318, world_, soldier_, "components.weapon.fireRate", kern::formula([](Float interval_ms) -> Float { return kern::math::div(60000.0, interval_ms); }, s.interval_ms));
      set_property_number_2 = n4194318.result;
      engine_game_world_set_property_flag(n4194319, world_, soldier_, "components.weapon.automatic", enum_equal<weapons_Id>(id, weapons_Id::smg));
      set_property_flag = n4194319.result;
      engine_game_world_set_property_number(n4194320, world_, soldier_, "components.weapon.range", s.range_m);
      set_property_number_3 = n4194320.result;
      engine_game_world_set_property_number(n4194321, world_, soldier_, "components.weapon.headshot", 3.0);
      set_property_number_4 = n4194321.result;
      list = kFoot;
      for (auto it4194322 = flow_for(n4194322, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Loadout>(list))))); it4194322.next();) {
        if (enum_equal<weapons_Id>(list[n4194322.index].id, id)) {
          engine_game_world_set_property_number(n4194323, world_, soldier_, "components.weapon.magazine", kern::to<Float>(list[n4194322.index].magazine));
          set_property_number_5 = n4194323.result;
        }
      }
      engine_game_world_set_property_text(n4194324, world_, soldier_, "components.weapon.fireSound", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::smg), "sc:shot0", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::sniper), "builtin:sniper", "sc:pistol0")));
      set_property_text_2 = n4194324.result;
      engine_game_world_set_property_number(n4194325, world_, soldier_, "components.weapon.pellets", 1.0);
      set_property_number_6 = n4194325.result;
      engine_game_world_set_property_number(n4194326, world_, soldier_, "components.weapon.aimFov", kern::formula([](Float a) -> Float { return ((a != 0.0) ? 8.09 : 42.6); }, kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::sniper))));
      set_property_number_7 = n4194326.result;
      engine_game_world_set_property_number(n4194327, world_, soldier_, "components.weapon.spread", 0.0);
      set_property_number_8 = n4194327.result;
      engine_game_world_set_property_number(n4194328, world_, soldier_, "components.weapon.aimSpread", 0.0);
      set_property_number_9 = n4194328.result;
      engine_game_world_set_property_number(n4194329, world_, soldier_, "components.weapon.recoil", 0.0);
      set_property_number_10 = n4194329.result;
      engine_game_world_soldier_state(n4194330, world_, soldier_, st);
      soldier_state_2 = n4194330.result;
      st = n4194330.out;
      if (soldier_state_2) {
        st.ammo = ammo_mag_[enum_index<weapons_Id>(id)];
        st.reserve = ammo_reserve_[enum_index<weapons_Id>(id)];
        engine_game_world_apply_soldier_state(world_, st);
      }
    }
    toast_ = text_append("Waffe: ", s.name, "");
    toast_age_ = 0.0;
    sim_time = engine_game_world_sim_time(world_);
    next_attack_ = kern::formula([](Float next_attack_, Float sim_time) -> Float { return kern::math::max(next_attack_, (sim_time + 0.25)); }, next_attack_, sim_time);
  }

  void cycle_foot_weapon(Int step = 0) {
    Int n = 0;
    Int cur = 0;
    n = 7;
    cur = 0;
    for (auto it4194331 = flow_for(n4194331, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it4194331.next();) {
      if (enum_equal<weapons_Id>(struct_break<Loadout>(list_get<Loadout>(kFoot, n4194331.index)).id, foot_weapon_)) {
        cur = n4194331.index;
      }
    }
    select_foot_weapon(struct_break<Loadout>(list_get<Loadout>(kFoot, kern::to<Int>(kern::formula([](Float cur, Float step, Float n) -> Float { return kern::math::mod((kern::math::mod((cur + step), n) + n), n); }, kern::to<Float>(cur), kern::to<Float>(step), kern::to<Float>(n))))).id);
  }

  struct game_weapon_in_handResult { Bool Result = false; };
  KERN_PURE game_weapon_in_handResult game_weapon_in_hand() {
    return {logic_and(enum_equal<Mode>(mode_, Mode::foot), logic_not(game_weapons_cpp::hitscan_on_foot(foot_weapon_).Result))};
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n2097162;
  struct : kern::Node<2097167> { Int index = 0; } n2097167;
  struct { Bool result = false; } n2097171;
  state_ofResult n2097177;
  struct { Bool result = false; } n2097178;
  struct { Bool result = false; } n2097179;
  common::Rng_nextResult n2097182;
  struct { Int result = 0; } n2097183;
  struct { Int result = 0; } n3145739;
  struct { Bool result = false; } n3145740;
  struct : kern::Node<3145741> { Int index = 0; } n3145741;
  struct : kern::Node<3145742> { Int index = 0; } n3145742;
  struct { Bool result = false; } n3145743;
  struct { Bool result = false; } n3145744;
  struct : kern::Node<4194314> { Int index = 0; } n4194314;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n4194315;
  struct { Bool result = false; } n4194316;
  struct { Bool result = false; } n4194317;
  struct { Bool result = false; } n4194318;
  struct { Bool result = false; } n4194319;
  struct { Bool result = false; } n4194320;
  struct { Bool result = false; } n4194321;
  struct : kern::Node<4194322> { Int index = 0; } n4194322;
  struct { Bool result = false; } n4194323;
  struct { Bool result = false; } n4194324;
  struct { Bool result = false; } n4194325;
  struct { Bool result = false; } n4194326;
  struct { Bool result = false; } n4194327;
  struct { Bool result = false; } n4194328;
  struct { Bool result = false; } n4194329;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n4194330;
  struct : kern::Node<4194331> { Int index = 0; } n4194331;
};
