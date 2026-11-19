struct Game : virtual kern::Blueprint, virtual city_stream, virtual common, virtual engine_game_runtime, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual game_combat, virtual game_controls, virtual game_ui, virtual game_vehicle_damage_cpp, virtual jet, virtual net_session, virtual people {
  // ---- variables ----
  Int engine_ = 0;
  Int world_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, Map<Int, CityStream_Damage>{}, List<Int>{}, false, 0, 0, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<Int64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}, List<Vec4>{}, Int64{0x0}, -1};
  Int soldier_ = 0;
  Int drone_actor_ = 0;
  Int jet_actor_ = 0;
  Int chute_actor_ = 0;
  Jet jet_ = Jet{true, true, true, 1.0, 1.0, JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}, Jet_Mode::hover, 0.0, 0.0, 0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, true, false, true, 0, 0.0, JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}};
  List<TrafficCar> cars_ = List<TrafficCar>{};
  Mode mode_ = Mode::foot;
  /// the car the player drives
  Int car_index_ = -1;
  Bool drone_out_ = false;
  Bool chute_open_ = false;
  Float chute_time_ = 0.0;
  Vector chute_steer_ = Vector{0.0, 0.0, 0.0};
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Bool rig_init_ = false;
  Float head_yaw_ = 0.0;
  Float head_pitch_ = 0.0;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<Int>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  List<PoliceUnit> units_ = List<PoliceUnit>{};
  Fixed<PoliceDrone, 5> pdrones_ = List<PoliceDrone>{};
  Float next_drone_wave_ = -1.0;
  Bool drone_wave_live_ = false;
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  /// jet: jet_mode.mjs HP_MAX 400
  Float star_since_ = -1.0;
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  Rng rng_ = Rng{20261029, 0.0, false};
  /// the original's sound on the page (game_audio_web.cpp): events and state for web/audio.mjs
  Bool web_audio_ = false;
  Text osfx_ = "";
  /// menus, settings, start, equipment, presentation (game_ui.h)
  UiState ui_ = UiState{Settings{List<Float>{}, 1}, SettingsTable{List<Option>{}, List<OptSpec>{}}, 0, false, false, 0, false, 0.0, 0.0, 0, 0, 0, Vector{1000000000.0, 1000000000.0, 0.0}, 0.0, -1, -1, GpsLive{GpsLive_View::eye, 0, 0, false, false, false, false, 0.0, -1000000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}, 0, "", Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, 0, 0, 0, List<Vector>{}, false, 0.0, false, Jetpack{1.0, false, false, 0, 0.0, -1.0, -99.0}, false, false, 0.0, List<Int64>{}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, 0, 0, 0, 0, 0, 0, FlightLog{List<Flight>{}, List<FlightLog_Event>{}, Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, -1000000000.0, 1, 0}, "", MusicSynth{0.0, 0, -2, -2, 0.0, 0.0, 1.0, Int64{0x0}, 0, 1, 0, 1, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, List<MusicSynth_Voice>{}, 0.0, 0.0, List<Float>{}, 0}, 0, true, "", "", 99.0, Vector{0.0, 0.0, 0.0}, false, 0};
  /// (of the program: combat::kStrikeCooldownS)
  Float combat_kStrikeCooldownS = 0.0;
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

  void osfx(Text fn = "", Vector at = Vector{0.0, 0.0, 0.0}, Float a = 0.0, Float b = 0.0, Float c = 0.0) {
    Text buf = "";
    if (logic_or(logic_not(web_audio_), math_compare<">">(kern::formula([](Float a) -> Float { return ((262144.0 < 0.0) != (a < 0.0) ? (a < 0.0) : (262144.0 < a)); }, kern::to<Float>(text_length(osfx_))), 0.5))) {
      return;
    }
    buf = "";
    buf = text_format<"%s[\"%s\",%.2f,%.2f,%.2f,%.5g,%.5g,%.5g]">(191, logic_select<Text>(math_compare<"==">(kern::to<Float>(text_length(osfx_)), 0.0), "", ","), fn, vector_break(at).x, vector_break(at).y, vector_break(at).z, a, b, c);
    osfx_ = text_append(osfx_, buf, "");
  }

  void web_audio_state_v3(Text& b, Text& s, Vector p = Vector{0.0, 0.0, 0.0}) {
    b = text_format<"[%.2f,%.2f,%.2f]">(511, vector_break(p).x, vector_break(p).y, vector_break(p).z);
    s = text_append(s, b, "");
  }

  struct Game_ctl_discrete_phResult { Bool Result = false; };
  Game_ctl_discrete_phResult Game_ctl_discrete_ph(Int b = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pad_hit, Float b) -> Float { return kern::math::band(pad_hit, kern::math::wrap32(kern::math::shl(1.0, b))); }, kern::to<Float>(ctl_.pad_hit), kern::to<Float>(b))))), 0.0)};
  }

  void ctl_strike_start() {
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    sim_time = engine_game_world_sim_time(world_);
    if (math_compare<"<">(kern::formula([](Float sim_time, Float last_strike) -> Float { return (sim_time - last_strike); }, sim_time, combat_.last_strike), combat_kStrikeCooldownS)) {
      sim_time_2 = engine_game_world_sim_time(world_);
      toast_ = text_append(text_append("Luftschlag laedt (", text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float combat_kStrikeCooldownS, Float sim_time_2, Float last_strike) -> Float { return kern::math::trunc((combat_kStrikeCooldownS - (sim_time_2 - last_strike))); }, combat_kStrikeCooldownS, sim_time_2, combat_.last_strike))), ""), " s)", "");
      toast_age_ = 0.0;
      osfx("fighter_jets.beep", cam_.eye, 2.0, 0.0, 0.0);
      return;
    }
    ctl_.aiming = true;
    ctl_.aim_lock_until = kern::formula([](Float clock) -> Float { return (clock + 0.5); }, ctl_.clock);
    osfx("fighter_jets.beep", cam_.eye, 4.0, 0.0, 0.0);
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
    engine_game_world_actor_matrix(n3145738, world_, a, m);
    actor_matrix = n3145738.result;
    m = n3145738.out;
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
    for (auto it3145743 = flow_for(n3145743, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it3145743.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n3145743.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n3145743.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n3145743.index].points))), 0.5))) {
          a = city_.lanes_[n3145743.index].points[k];
          b = city_.lanes_[n3145743.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n3145743.index].width))) {
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
    engine_game_world_set_mode(n3145747, world_, game_SimMode::simulate);
    set_mode = n3145747.result;
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n3145752 = state_of(jet_actor_);
    s = n3145752.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n3145753, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n3145753.result;
    engine_game_world_set_actor_velocity(n3145754, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n3145754.result;
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
    n3145757 = common::Rng_next(rng_);
    start_2 = n3145757.Result;
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
    engine_game_world_physics(n3145758, world_);
    physics = n3145758.result;
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

  struct combat_targetResult { Bool Result = false; };
  combat_targetResult combat_target(Vector& at, Vector& vel, Bool& drone, Bool& air) {
    Int a = 0;
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    drone = logic_and(enum_equal<Mode>(mode_, Mode::drone), drone_out_);
    air = logic_or(drone, enum_equal<Mode>(mode_, Mode::jet));
    if (player_dead_) {
      return {false};
    }
    a = kern::to<Int>(kern::formula([](Float drone, Float drone_actor_, Float a, Float car_index_, Float a_2, Float a_3, Float jet_actor_, Float a_4, Float chute_actor_, Float soldier_) -> Float { return ((drone != 0.0) ? drone_actor_ : (((a != 0.0) && (car_index_ >= 0.0)) ? a_2 : ((a_3 != 0.0) ? jet_actor_ : ((a_4 != 0.0) ? chute_actor_ : soldier_)))); }, kern::to<Float>(drone), kern::to<Float>(drone_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::car)), kern::to<Float>(car_index_), kern::to<Float>(cars_[car_index_].actor), kern::to<Float>(enum_equal<Mode>(mode_, Mode::jet)), kern::to<Float>(jet_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::parachute)), kern::to<Float>(chute_actor_), kern::to<Float>(soldier_)));
    n4194314 = matrix_of(a);
    a_2 = n4194314.Result;
    at = engine_math_math::Mat4_translation(a_2).Result;
    the_actor_velocity = engine_game_world_actor_velocity(world_, a);
    vel = the_actor_velocity;
    return {true};
  }

  void combat_tracer(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector color = Vector{0.0, 0.0, 0.0}) {
    Float sim_time = 0.0;
    if (math_compare<">">(kern::to<Float>(list_length<Tracer>(combat_.tracers)), 96.0)) {
      list_remove_at(combat_.tracers, 0);
    }
    sim_time = engine_game_world_sim_time(world_);
    list_add(combat_.tracers, struct_make<Tracer>(a, b, kern::formula([](Float sim_time) -> Float { return (sim_time + 0.08); }, sim_time), color));
  }

  void combat_banner(const Text& text) {
    Float sim_time = 0.0;
    combat_.banner = text;
    sim_time = engine_game_world_sim_time(world_);
    combat_.banner_at = sim_time;
  }

  struct combat_shot_near_segResult { Bool Result = false; };
  combat_shot_near_segResult combat_shot_near_seg(const game_ShotEvent& s, Float& t, Vector seg = Vector{0.0, 0.0, 0.0}, Float l2 = 0.0, Vector c = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float l2) -> Float { return kern::math::div(a, l2); }, vector_dot(vector_sub(c, s.from), seg), l2), 0.0, 1.0));
    return {math_compare<"<">(vector_length(vector_sub(vector_add(s.from, vector_scale(seg, t)), c)), r)};
  }

  struct line_of_sightResult { Bool Result = false; };
  line_of_sightResult line_of_sight(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Int me = 0;
    Int physics = 0;
    d = vector_sub(b, a);
    l = vector_length(d);
    if (math_compare<"<">(l, 0.05)) {
      return {true};
    }
    me = kern::to<Int>(kern::formula([](Float a, Float car_index_, Float a_2, Float a_3, Float jet_actor_, Float a_4, Float drone_actor_, Float a_5, Float chute_actor_, Float soldier_) -> Float { return (((a != 0.0) && (car_index_ >= 0.0)) ? a_2 : ((a_3 != 0.0) ? jet_actor_ : ((a_4 != 0.0) ? drone_actor_ : ((a_5 != 0.0) ? chute_actor_ : soldier_)))); }, kern::to<Float>(enum_equal<Mode>(mode_, Mode::car)), kern::to<Float>(car_index_), kern::to<Float>(cars_[car_index_].actor), kern::to<Float>(enum_equal<Mode>(mode_, Mode::jet)), kern::to<Float>(jet_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::drone)), kern::to<Float>(drone_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::parachute)), kern::to<Float>(chute_actor_), kern::to<Float>(soldier_)));
    engine_game_world_physics(n5242890, world_);
    physics = n5242890.result;
    return {logic_not(struct_break<the_RayResult_>(common::cast(physics, a, vector_scale(d, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l)), l, body(me).Result, Int64{0x1}).Result).hit)};
  }

  struct request_respawnResult { Bool Result = false; };
  request_respawnResult request_respawn() {
    Bool either = false;
    Bool respawn_now = false;
    either = logic_not(player_dead_);
    if (logic_not(either)) {
      either = math_compare<">">(respawn_wait().Result, 0.0);
    }
    if (either) {
      return {false};
    }
    engine_game_world_respawn_now(n5242897, world_, soldier_);
    respawn_now = n5242897.result;
    return {respawn_now};
  }

  struct ui_command_startsResult { Bool Result = false; };
  KERN_PURE ui_command_startsResult ui_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  void ui_draw_gps_make(Int& id, const MeshData& m, Int r = 0, Vector color = Vector{0.0, 0.0, 0.0}, Float alpha = 0.0) {
    Int r_ = 0;
    Material mat = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    mat = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    mat.unlit = true;
    mat.cast_shadow = false;
    mat.color = engine_math_math::Vec4_new(vector_break(color).x, vector_break(color).y, vector_break(color).z, alpha).Result;
    mat.alpha = logic_select<AlphaMode>(math_compare<"<">(alpha, 1.0), AlphaMode::blend, AlphaMode::opaque);
    mat.double_sided = true;
    engine_renderer_allocate_id(n6291466, r_);
    allocate_id = n6291466.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = mat;
    look.visible = false;
    engine_upsert(n6291467, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n6291467.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  /// (colored: every point has its colour in col; else all are `color`)
  void ui_draw_helpers_line_mesh(Int& id, const List<Float>& pos, const List<Float>& col, Int r = 0, Bool colored = false, Vector color = Vector{0.0, 0.0, 0.0}) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshData m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Int i = 0;
    Vector c = Vector{0.0, 0.0, 0.0};
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (logic_not(math_compare<"!=">(kern::to<Float>(id), 0.0))) {
      engine_renderer_allocate_id(n6291471, r_);
      allocate_id = n6291471.result;
      id = allocate_id;
    }
    m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Float>(pos))), 0.5))) {
      c = logic_select<Vector>(colored, vector_make(col[i], col[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], col[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i)))]), color);
      engine_geo_mesh_data::MeshData_push(m, vector_make(pos[i], pos[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], pos[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i)))]), Vector{0.0, 0.0, 1.0}, c, Vector{0.0, 0.0, 0.0});
      i += 3;
    }
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material.lines = true;
    look.material.unlit = true;
    look.material.cast_shadow = false;
    look.material.frustum_culled = false;
    look.material.linear_colors = colored;
    look.visible = logic_not(math_compare<"==">(kern::to<Float>(list_length<Float>(pos)), 0.0));
    engine_upsert(n6291472, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n6291472.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  void ui_before_input(Int e = 0) {
    ui_.head_yaw_before = head_yaw_;
    ui_.head_pitch_before = head_pitch_;
  }

  void ui_clear_police() {
    for (auto it6291473 = flow_for(n6291473, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it6291473.next();) {
      if (logic_and(math_compare<">=">(kern::to<Float>(units_[n6291473.index].car), 0.0), math_compare<"!=">(kern::to<Float>(units_[n6291473.index].car), kern::to<Float>(car_index_)))) {
        engine_game_world_set_alive(world_, cars_[units_[n6291473.index].car].actor, false);
        cars_[units_[n6291473.index].car].placed = false;
      }
      if (math_compare<">=">(kern::to<Float>(units_[n6291473.index].car), 0.0)) {
        cars_[units_[n6291473.index].car].unit = -1;
      }
    }
    list_clear(units_);
    for (auto it6291474 = flow_for(n6291474, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it6291474.next();) {
      pdrones_[n6291474.index].active = false;
    }
    drone_wave_live_ = false;
    next_drone_wave_ = -1.0;
    star_since_ = -1.0;
  }

  struct car_max_hpResult { Float Result = 0.0; };
  KERN_PURE car_max_hpResult car_max_hp(const TrafficCar& c) {
    return {kern::formula([](Float police, Float a) -> Float { return ((police != 0.0) ? 300.0 : ((a != 0.0) ? 320.0 : 180.0)); }, kern::to<Float>(c.police), kern::to<Float>(game_vehicle_damage_cpp::is_bus(c).Result))};
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n3145738;
  struct : kern::Node<3145743> { Int index = 0; } n3145743;
  struct { Bool result = false; } n3145747;
  state_ofResult n3145752;
  struct { Bool result = false; } n3145753;
  struct { Bool result = false; } n3145754;
  common::Rng_nextResult n3145757;
  struct { Int result = 0; } n3145758;
  matrix_ofResult n4194314;
  struct { Int result = 0; } n5242890;
  struct { Bool result = false; } n5242897;
  struct { Int result = 0; } n6291466;
  struct { Bool result = false; } n6291467;
  struct { Int result = 0; } n6291471;
  struct { Bool result = false; } n6291472;
  struct : kern::Node<6291473> { Int index = 0; } n6291473;
  struct : kern::Node<6291474> { Int index = 0; } n6291474;
};
