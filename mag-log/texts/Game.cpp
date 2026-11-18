struct Game : virtual kern::Blueprint, virtual city_stream, virtual common, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual game, virtual game_barricades_cpp, virtual game_combat, virtual game_controls, virtual game_gaps, virtual game_gaps_cpp, virtual game_guided_cpp, virtual game_lamps_cpp, virtual game_life, virtual game_look_cpp, virtual game_ui, virtual gaps_rules, virtual guided, virtual interiors, virtual jet, virtual lamps, virtual net_session {
  // ---- variables ----
  PadState pad = PadState{false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, false};
  List<GapsState> gaps_ = List<GapsState>{};
  Int engine_ = 0;
  /// the page runs a WebXR session ("vr 1")
  Bool vr_ = false;
  Fixed<Game_VrHand, 2> vr_hands_ = List<Game_VrHand>{};
  /// the wrist menu's KAMERA (game_controls.cpp)
  Bool vr_scoped_ = false;
  Int vr_scope_tex_ = 0;
  Int vr_scope_mesh_ = 0;
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
  Float head_yaw_ = 0.0;
  Float head_pitch_ = 0.0;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  Float frame_w_ = 960.0;
  Float frame_h_ = 540.0;
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<Int>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  List<PoliceUnit> units_ = List<PoliceUnit>{};
  Fixed<PoliceDrone, 5> pdrones_ = List<PoliceDrone>{};
  Float next_drone_wave_ = -1.0;
  Bool drone_wave_live_ = false;
  weapons_Id foot_weapon_ = weapons_Id::glock;
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  List<Game_Lamp> lamps_ = List<Game_Lamp>{};
  /// jet: jet_mode.mjs HP_MAX 400
  Float star_since_ = -1.0;
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  Int bank_ = 0;
  Bool mobile_ = false;
  /// menus, settings, start, equipment, presentation (game_ui.h)
  UiState ui_ = UiState{Settings{List<Float>{}, 1}, SettingsTable{List<Option>{}, List<OptSpec>{}}, 0, false, false, 0, false, 0.0, 0.0, 0, 0, 0, Vector{1000000000.0, 1000000000.0, 0.0}, 0.0, -1, -1, GpsLive{GpsLive_View::eye, 0, 0, false, false, false, false, 0.0, -1000000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}, 0, "", Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, 0, 0, 0, List<Vector>{}, false, 0.0, false, Jetpack{1.0, false, false, 0, 0.0, -1.0, -99.0}, false, false, 0.0, List<Int64>{}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FlightLog{List<Flight>{}, List<FlightLog_Event>{}, Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, -1000000000.0, 1, 0}, "", MusicSynth{0.0, 0, -2, -2, 0.0, 0.0, 1.0, Int64{0x0}, 0, 1, 0, 1, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, List<MusicSynth_Voice>{}, 0.0, 0.0, List<Float>{}, 0}, 0, true, "", "", 99.0, Vector{0.0, 0.0, 0.0}, false, 0};
  /// M3 life (last: destroyed before world_)
  List<GameLife> glife_ = List<GameLife>{};
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// building_interiors.mjs PLANKS
  KERN_CONST Int kPlanks = 6;
  /// DOOR_W
  KERN_CONST Float kDoorW = 1.8;
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;
  KERN_CONST Float kRad = 57.29577951308238;
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kPoleH = 6.2;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;
  KERN_CONST Float kDeg = 0.017453292519943295;
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

  struct gaps_barricade_doorsResult { Bool Result = false; };
  gaps_barricade_doorsResult gaps_barricade_doors(List<GapsState_DoorView>& out) {
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    Int64 key_3 = Int64{0x0};
    List<Int> t = List<Int>{};
    Int64 key_4 = Int64{0x0};
    List<Int> put = List<Int>{};
    Int count = 0;
    Int fill = 0;
    Int it = 0;
    Bool stop_2 = false;
    Int at = 0;
    Int64 key_5 = Int64{0x0};
    list_clear(out);
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))), logic_not(interiors::Interiors_active(glife_[0].interiors).Result))) {
      return {false};
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors)), 0.0)) {
      return {false};
    }
    key = game_barricades_cpp::building_key(glife_[0].interiors.plan_).Result;
    key_2 = key;
    map_add(n1048586, gaps_[0].planks, key_2);
    key_3 = key;
    map_add(n1048587, gaps_[0].planks, key_3);
    if (math_compare<"!=">(kern::to<Float>(list_length<Int>(gaps_[0].planks[n1048587.index])), kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors)))) {
      key_4 = key;
      map_add(n1048588, gaps_[0].planks, key_4);
      put = t;
      t = gaps_[0].planks[n1048588.index];
      count = list_length<Interiors_Door>(glife_[0].interiors.plan_.doors);
      fill = kern::to<Int>(kern::formula([](Float kPlanks) -> Float { return kern::math::band(kPlanks, 255.0); }, kern::to<Float>(kPlanks)));
      list_clear(t);
      list_resize(t, count, fill);
      gaps_[0].planks[n1048588.index] = put;
    }
    if (math_compare<">">(kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks)), 400.0)) {
      it = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks))))))) {
        if (logic_not(int64_equal(int64_of_int(int64_to_int(map_key_at<Int64, List<Int>>(gaps_[0].planks, it))), key))) {
          at = it;
          map_remove_at(gaps_[0].planks, at);
          stop_2 = true;
        }
        if (logic_not(stop_2)) {
          it += 1;
        }
      }
    }
    for (auto it1048589 = flow_for(n1048589, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors))))); it1048589.next();) {
      key_5 = key;
      map_add(n1048590, gaps_[0].planks, key_5);
      list_add(out, struct_make<GapsState_DoorView>(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n1048589.index))), glife_[0].interiors.plan_.doors[n1048589.index].c, glife_[0].interiors.plan_.doors[n1048589.index].d, glife_[0].interiors.plan_.doors[n1048589.index].n, list_get<Int>(gaps_[0].planks[n1048590.index], n1048589.index), kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? a_2 : a_3); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<Float>(glife_[0].interiors.plan_.levels)), 0.0)), glife_[0].interiors.plan_.base, glife_[0].interiors.plan_.levels[0]), key));
    }
    return {true};
  }

  void gaps_barricades_draw() {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    List<GapsState_DoorView> doors = List<GapsState_DoorView>{};
    Bool both = false;
    Float yaw = 0.0;
    Float along = 0.0;
    Float tilt = 0.0;
    Vector at = Vector{0.0, 0.0, 0.0};
    Mat4 m_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].renderer), 0.0)))) {
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].plank_part.id), 0.0))) {
      m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
      m.roughness = 0.85;
      box_2 = engine_primitives_box(vector_make(kern::formula([](Float kDoorW) -> Float { return (kDoorW + 0.5); }, kDoorW), 0.06, 0.2), Vector{0.42, 0.29, 0.17});
      box = box_2;
      engine_renderer_allocate_id(n1048594, gaps_[0].renderer);
      allocate_id = n1048594.result;
      gaps_[0].plank_part.id = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material = m;
      look.extra_slot = kMaxExtensionAttributes;
      look.visible = false;
      engine_upsert(n1048595, gaps_[0].renderer, gaps_[0].plank_part.id, box, look, List<Float>{}, List<Float>{});
      upsert = n1048595.result;
      if (logic_not(upsert)) {
        engine_renderer_release_id(gaps_[0].renderer, gaps_[0].plank_part.id);
        gaps_[0].plank_part.id = 0;
        return;
      }
    }
    list_clear(gaps_[0].plank_part.transforms);
    list_clear(gaps_[0].plank_part.tints);
    doors = List<GapsState_DoorView>{};
    both = combat_.zombies.active;
    if (both) {
      n1048597 = gaps_barricade_doors(doors);
      both = n1048597.Result;
    }
    if (both) {
      for (auto it1048598 = flow_for(n1048598, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_DoorView>(doors))))); it1048598.next();) {
        yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(doors[n1048598.index].d).y, vector_break(doors[n1048598.index].d).x);
        for (auto it1048599 = flow_for(n1048599, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(doors[n1048598.index].planks)))); it1048599.next();) {
          along = kern::formula([](Float index) -> Float { return ((kern::math::mod(index, 2.0) != 0.0) ? 0.06 : (-0.06)); }, kern::to<Float>(n1048599.index));
          tilt = kern::formula([](Float index) -> Float { return (((kern::math::mod(index, 2.0) != 0.0) ? 1.0 : (-1.0)) * (0.08 + (0.05 * kern::math::mod(index, 3.0)))); }, kern::to<Float>(n1048599.index));
          at = vector_make(kern::formula([](Float x, Float x_2, Float along, Float x_3) -> Float { return ((x + (x_2 * along)) + (x_3 * 0.22)); }, vector_break(doors[n1048598.index].c).x, vector_break(doors[n1048598.index].d).x, along, vector_break(doors[n1048598.index].n).x), kern::formula([](Float y, Float y_2, Float along, Float y_3) -> Float { return ((y + (y_2 * along)) + (y_3 * 0.22)); }, vector_break(doors[n1048598.index].c).y, vector_break(doors[n1048598.index].d).y, along, vector_break(doors[n1048598.index].n).y), kern::formula([](Float a, Float index) -> Float { return ((a + 0.35) + (index * 0.36)); }, doors[n1048598.index].the_floor, kern::to<Float>(n1048599.index)));
          a = engine_math_math::Mat4_trs(at, engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
          a_2 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, tilt).Result, Vector{1.0, 1.0, 1.0}).Result;
          m_2 = engine_math_math::Mat4_mul(a, a_2).Result;
          place = list_length<Float>(gaps_[0].plank_part.transforms);
          list_insert_list(gaps_[0].plank_part.transforms, place, m_2.m);
          place_2 = list_length<Float>(gaps_[0].plank_part.tints);
          item = 1.0;
          item_2 = 1.0;
          item_3 = 1.0;
          item_4 = 1.0;
          list_insert_list(gaps_[0].plank_part.tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
        }
      }
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].plank_part.transforms)), 0.0)) {
      engine_renderer_set_visible(n1048600, gaps_[0].renderer, gaps_[0].plank_part.id, false);
      set_visible = n1048600.result;
      return;
    }
    engine_renderer_update_instances(n1048601, gaps_[0].renderer, gaps_[0].plank_part.id, gaps_[0].plank_part.transforms, gaps_[0].plank_part.tints);
    update_instances = n1048601.result;
    engine_renderer_set_visible(n1048602, gaps_[0].renderer, gaps_[0].plank_part.id, true);
    set_visible_2 = n1048602.result;
  }

  struct Game_ctl_discrete_phResult { Bool Result = false; };
  Game_ctl_discrete_phResult Game_ctl_discrete_ph(Int b = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float pad_hit, Float b) -> Float { return kern::math::band(pad_hit, kern::math::wrap32(kern::math::shl(1.0, b))); }, kern::to<Float>(ctl_.pad_hit), kern::to<Float>(b))))), 0.0)};
  }

  struct ctl_screen_rayResult { Bool Result = false; };
  ctl_screen_rayResult ctl_screen_ray(Vector& from, Vector& dir, Float x = 0.0, Float y = 0.0) {
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Bool both_2 = false;
    Vector xr_eye = Vector{0.0, 0.0, 0.0};
    Vector xr_forward = Vector{0.0, 0.0, 0.0};
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Float t = 0.0;
    Float aspect = 0.0;
    Float nx = 0.0;
    Float ny = 0.0;
    both = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
    if (both) {
      xr_2 = engine_engine_xr(engine_);
      both = xr_2;
    }
    if (both) {
      both_2 = enum_equal<Mode>(mode_, Mode::foot);
      if (both_2) {
        n2097165 = vr_hand_ray(from, dir);
        both_2 = n2097165.Result;
      }
      if (both_2) {
        return {true};
      }
      xr_eye = engine_engine_xr_eye(engine_);
      from = xr_eye;
      xr_forward = engine_engine_xr_forward(engine_);
      dir = xr_forward;
      return {true};
    }
    f = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
    r = vector_make(kern::formula([](Float y, Float z, Float z_2, Float y_2) -> Float { return ((y * z) - (z_2 * y_2)); }, vector_break(f).y, vector_break(cam_.up).z, vector_break(f).z, vector_break(cam_.up).y), kern::formula([](Float z, Float x, Float x_2, Float z_2) -> Float { return ((z * x) - (x_2 * z_2)); }, vector_break(f).z, vector_break(cam_.up).x, vector_break(f).x, vector_break(cam_.up).z), kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return ((x * y) - (y_2 * x_2)); }, vector_break(f).x, vector_break(cam_.up).y, vector_break(f).y, vector_break(cam_.up).x));
    r = logic_select<Vector>(math_compare<">">(vector_length(r), 9.999999717180685e-10), vector_normalize(r), Vector{0.0, 1.0, 0.0});
    u = vector_make(kern::formula([](Float y, Float z, Float z_2, Float y_2) -> Float { return ((y * z) - (z_2 * y_2)); }, vector_break(r).y, vector_break(f).z, vector_break(r).z, vector_break(f).y), kern::formula([](Float z, Float x, Float x_2, Float z_2) -> Float { return ((z * x) - (x_2 * z_2)); }, vector_break(r).z, vector_break(f).x, vector_break(r).x, vector_break(f).z), kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return ((x * y) - (y_2 * x_2)); }, vector_break(r).x, vector_break(f).y, vector_break(r).y, vector_break(f).x));
    t = kern::formula([](Float fov) -> Float { return kern::math::tan(kern::math::div(fov, 2.0)); }, cam_.fov);
    aspect = kern::formula([](Float frame_w_, Float frame_h_) -> Float { return kern::math::div(frame_w_, kern::math::max(1.0, frame_h_)); }, frame_w_, frame_h_);
    nx = kern::formula([](Float x, Float frame_w_) -> Float { return (kern::math::div((2.0 * x), kern::math::max(1.0, frame_w_)) - 1.0); }, x, frame_w_);
    ny = kern::formula([](Float y, Float frame_h_) -> Float { return (1.0 - kern::math::div((2.0 * y), kern::math::max(1.0, frame_h_))); }, y, frame_h_);
    from = cam_.eye;
    dir = logic_select<Vector>(math_compare<">">(vector_length(vector_add(vector_add(f, vector_scale(r, kern::formula([](Float nx, Float t, Float aspect) -> Float { return ((nx * t) * aspect); }, nx, t, aspect))), vector_scale(u, kern::formula([](Float ny, Float t) -> Float { return (ny * t); }, ny, t)))), 9.999999717180685e-10), vector_normalize(vector_add(vector_add(f, vector_scale(r, kern::formula([](Float nx, Float t, Float aspect) -> Float { return ((nx * t) * aspect); }, nx, t, aspect))), vector_scale(u, kern::formula([](Float ny, Float t) -> Float { return (ny * t); }, ny, t)))), f);
    return {true};
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

  void step_parachute(Float h = 0.0) {
    Int w = 0;
    Int physics = 0;
    physics_Body b = physics_Body{Int64{0x0}};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float m = 0.0;
    Float mass = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    engine_game_world_physics(n3145757, world_);
    physics = n3145757.result;
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

  struct details_command_startsResult { Bool Result = false; };
  KERN_PURE details_command_startsResult details_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
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
    engine_renderer_allocate_id(n5242891, renderer);
    allocate_id = n5242891.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_2 = engine_engine_renderer(e);
    engine_upsert(n5242892, renderer_2, p.id, disc, look, parked.m, List<Float>{});
    upsert = n5242892.result;
    if (logic_not(upsert)) {
      renderer_3 = engine_engine_renderer(e);
      engine_renderer_release_id(renderer_3, p.id);
      p.id = 0;
    }
  }

  void gaps_init(Int e = 0) {
    Int renderer = 0;
    MeshData disc = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Int renderer_2 = 0;
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Int renderer_3 = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int renderer_4 = 0;
    Bool upsert = false;
    Int renderer_5 = 0;
    Int renderer_6 = 0;
    Int allocate_id_2 = 0;
    Int renderer_7 = 0;
    Bool upsert_2 = false;
    Int renderer_8 = 0;
    gaps_ = list_make<GapsState, "1">(GapsState{gaps_SirenVoice{false, 0.0, 1.0, 0.0}, gaps_SirenVoice{false, 0.0, 1.0, 0.0}, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0, List<Float>{-1.0, -1.0, -1.0, -1.0, -1.0}, gaps_SmokeField{List<gaps_SmokeEffect>{}, List<gaps_Puff>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, 0, List<Float>{}, false, -1.0, -99.0, -99.0, 0, List<GapsState_Burst>{}, 0, -1.0, 0, 0, 0, false, gaps_VisualPose{false, 0, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, 0.016666666666666666, 0, 0, List<GapsState_JetVoice>{}, 0, List<GapsState_TrailPuff>{}, 0, List<GapsState_MotorVoice>{}, 0, false, GapsState_Part{0, List<Float>{}, List<Float>{}}, 0.0, 0, false, Map<Int64, List<Int>>{}, Map<Int64, Float>{}, Map<Int, GapsState_ZombieDoor>{}, 0, GapsState_Part{0, List<Float>{}, List<Float>{}}, false, false, false, -10.0, 0, 0, -1.0, 0.0, lamps_Field{List<lamps_Lamp>{}, Set<Int64>{}, Map<Int64, Int>{}, Map<Int64, List<Int>>{}}, 4294967295, false, false, -1.0, 0, List<physics_Body>{}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, List<Float>{-99.0, -99.0}, -99.0, -1.0, false, 0.0, 0.0, List<GapsState_RocketPuff>{}, 0, 0, List<GapsState_SniperRound>{}, GapsState_Part{0, List<Float>{}, List<Float>{}}});
    gaps_[0].smoke = gaps_rules::SmokeField_of(kern::to<Int>(kern::formula([](Float mobile_) -> Float { return kern::math::wrap32(((mobile_ != 0.0) ? 48.0 : 78.0)); }, kern::to<Float>(mobile_)))).Result;
    renderer = engine_engine_renderer(e);
    gaps_[0].renderer = renderer;
    n5242890 = game_gaps_cpp::smoke_disc();
    disc = n5242890.Result;
    gaps_init_make_part(disc, gaps_[0].disc, e, false);
    gaps_init_make_part(disc, gaps_[0].ember, e, true);
    renderer_2 = engine_engine_renderer(e);
    gaps_lamps_init(renderer_2);
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    m.alpha = AlphaMode::blend;
    m.unlit = true;
    m.double_sided = true;
    m.cast_shadow = false;
    m.color = engine_math_math::Vec4_new(0.851, 0.863, 0.875, 1.0).Result;
    renderer_3 = engine_engine_renderer(e);
    engine_renderer_allocate_id(n5242893, renderer_3);
    allocate_id = n5242893.result;
    gaps_[0].trail_part.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_4 = engine_engine_renderer(e);
    engine_upsert(n5242894, renderer_4, gaps_[0].trail_part.id, disc, look, parked.m, List<Float>{});
    upsert = n5242894.result;
    if (logic_not(upsert)) {
      renderer_5 = engine_engine_renderer(e);
      engine_renderer_release_id(renderer_5, gaps_[0].trail_part.id);
      gaps_[0].trail_part.id = 0;
    }
    m.color = engine_math_math::Vec4_new(0.847, 0.847, 0.847, 1.0).Result;
    look.material = m;
    renderer_6 = engine_engine_renderer(e);
    engine_renderer_allocate_id(n5242895, renderer_6);
    allocate_id_2 = n5242895.result;
    gaps_[0].rocket_part.id = allocate_id_2;
    renderer_7 = engine_engine_renderer(e);
    engine_upsert(n5242896, renderer_7, gaps_[0].rocket_part.id, disc, look, parked.m, List<Float>{});
    upsert_2 = n5242896.result;
    if (logic_not(upsert_2)) {
      renderer_8 = engine_engine_renderer(e);
      engine_renderer_release_id(renderer_8, gaps_[0].rocket_part.id);
      gaps_[0].rocket_part.id = 0;
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
    for (auto it5242897 = flow_for(n5242897, 0, 1); it5242897.next();) {
      for (auto it5242898 = flow_for(n5242898, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 0.05); }, kern::to<Float>(rate)))))))); it5242898.next();) {
        n = kern::to<Int>(kern::formula([](Float index, Float rate, Float index_2) -> Float { return (kern::math::trunc(((index * 0.09) * rate)) + index_2); }, kern::to<Float>(n5242897.index), kern::to<Float>(rate), kern::to<Float>(n5242898.index)));
        if (math_compare<">">(kern::formula([](Float n, Float a) -> Float { return ((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(lock))), 0.5)) {
          lock[n] = kern::formula([](Float index, Float rate) -> Float { return ((kern::math::mod(kern::math::div((index * 1760.0), rate), 1.0) < 0.5) ? 0.05 : (-0.05)); }, kern::to<Float>(n5242898.index), kern::to<Float>(rate));
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
    engine_sound_bank_set_pcm(n5242899, bank_, "sc:locktone", lock, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(lock)))), 1, rate);
    set_pcm = n5242899.result;
    engine_sound_bank_request(bank_, "sc:guidedbang");
    engine_sound_bank_set_pcm(n5242900, bank_, "sc:guidedbang", bang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(bang)))), 1, rate);
    set_pcm_2 = n5242900.result;
    gaps_[0].guided_sounds = true;
  }

  struct gaps_command_startsResult { Bool Result = false; };
  KERN_PURE gaps_command_startsResult gaps_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct guided_target_atResult { Bool Result = false; };
  guided_target_atResult guided_target_at(const guided_Target& t, Vector& at) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    switch (flow_switch_enum<guided_TargetKind>(t.kind)) {
      case guided_TargetKind::point: {
        return {false};
      } break;
      case guided_TargetKind::person: {
        if (logic_or(math_compare<"<">(kern::to<Float>(t.index), 0.0), math_compare<">=">(kern::to<Float>(t.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(people_.people_))))))))) {
          return {false};
        }
        if (logic_not(game_guided_cpp::person_up(people_.people_[t.index]).Result)) {
          return {false};
        }
        at = vector_add(people_.people_[t.index].pos, Vector{0.0, 0.0, 1.1});
        return {true};
      } break;
      case guided_TargetKind::car: {
        if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(t.index), 0.0), math_compare<">=">(kern::to<Float>(t.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<TrafficCar>(cars_)))))))), logic_not(cars_[t.index].placed))) {
          return {false};
        }
        n6291466 = matrix_of(cars_[t.index].actor);
        a = n6291466.Result;
        at = vector_add(engine_math_math::Mat4_translation(a).Result, Vector{0.0, 0.0, 0.8});
        return {true};
      } break;
      case guided_TargetKind::police_drone: {
        if (logic_or(logic_or(math_compare<"<">(kern::to<Float>(t.index), 0.0), math_compare<">=">(kern::to<Float>(t.index), 5.0)), logic_not(pdrones_[t.index].active))) {
          return {false};
        }
        at = pdrones_[t.index].pos;
        return {true};
      } break;
      case guided_TargetKind::player: {
        for (auto it6291467 = flow_for(n6291467, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it6291467.next();) {
          if (math_compare<"==">(kern::to<Float>(net_.remotes_[n6291467.index].state.slot), kern::to<Float>(t.slot))) {
            at = vector_add(net_.remotes_[n6291467.index].shown, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.1 : 0.0); }, kern::to<Float>(enum_equal<net_PlayerMode>(net_.remotes_[n6291467.index].state.mode, net_PlayerMode::foot)))));
            return {true};
          }
        }
        return {false};
      } break;
    }
  }

  void gaps_lamps_init_make(GapsState_Part& p, const MeshData& mesh, Int r = 0, Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n7340042, r_);
    allocate_id = n7340042.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    engine_upsert(n7340043, r_, p.id, mesh, look, parked.m, List<Float>{});
    upsert = n7340043.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, p.id);
      p.id = 0;
    }
  }

  void gaps_lamps_init(Int r = 0) {
    Int r_ = 0;
    Vector steel = Vector{0.0, 0.0, 0.0};
    Vector head = Vector{0.0, 0.0, 0.0};
    MeshData pole = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material pm = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Material lens = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material pool = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    r_ = r;
    steel = game_lamps_cpp::rgb(2829873).Result;
    head = game_lamps_cpp::rgb(2763304).Result;
    pole = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    cylinder = engine_primitives_cylinder(0.09, kPoleH, 7, steel);
    engine_primitives_append(n7340044, pole, cylinder, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    pole = n7340044.dst;
    box = engine_primitives_box(Vector{1.5, 0.09, 0.09}, steel);
    engine_primitives_append(n7340045, pole, box, engine_math_math::Mat4_translate(vector_make(0.72, 0.0, kern::formula([](Float kPoleH) -> Float { return (kPoleH - 0.1); }, kPoleH))).Result);
    pole = n7340045.dst;
    box_2 = engine_primitives_box(Vector{0.6, 0.26, 0.14}, head);
    engine_primitives_append(n7340046, pole, box_2, engine_math_math::Mat4_translate(vector_make(kArm, 0.0, kHeadZ)).Result);
    pole = n7340046.dst;
    pm = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    pm.roughness = 0.55;
    pm.metalness = 0.6;
    gaps_lamps_init_make(gaps_[0].lamp_pole, pole, r_, pm);
    gaps_lamps_init_make(gaps_[0].lamp_fallen, pole, r_, pm);
    lens = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    lens.unlit = true;
    lens.cast_shadow = false;
    lens.emissive = vector_scale(game_lamps_cpp::rgb(16757594).Result, 3.2);
    box_3 = engine_primitives_box(Vector{0.6, 0.26, 0.02}, Vector{1.0, 1.0, 1.0});
    a = box_3;
    gaps_lamps_init_make(gaps_[0].lamp_glow, a, r_, lens);
    pool = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    pool.alpha = AlphaMode::blend;
    pool.unlit = true;
    pool.cast_shadow = false;
    pool.double_sided = true;
    pool.color = engine_math_math::Vec4_new(1.0, 0.72, 0.38, 1.0).Result;
    cylinder_2 = engine_primitives_cylinder(0.5, 0.01, 20, Vector{1.0, 1.0, 1.0});
    a_2 = cylinder_2;
    gaps_lamps_init_make(gaps_[0].lamp_pool, a_2, r_, pool);
  }

  void gaps_lamps_sounds() {
    Int rate = 0;
    List<Float> glass = List<Float>{};
    List<Float> clang = List<Float>{};
    Bool set_pcm = false;
    Bool set_pcm_2 = false;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), gaps_[0].lamp_sounds), logic_not(math_compare<"!=">(kern::to<Float>(bank_), 0.0)))) {
      return;
    }
    rate = 44100;
    glass = game_lamps_cpp::glass_pcm(rate).Result;
    clang = game_lamps_cpp::clang_pcm(rate).Result;
    engine_sound_bank_request(bank_, "sc:lampglass");
    engine_sound_bank_set_pcm(n7340047, bank_, "sc:lampglass", glass, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(glass)))), 1, rate);
    set_pcm = n7340047.result;
    engine_sound_bank_request(bank_, "sc:lampclang");
    engine_sound_bank_set_pcm(n7340048, bank_, "sc:lampclang", clang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(clang)))), 1, rate);
    set_pcm_2 = n7340048.result;
    gaps_[0].lamp_sounds = true;
  }

  void gaps_lamps_mirror() {
    list_clear(lamps_);
    for (auto it7340049 = flow_for(n7340049, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it7340049.next();) {
      list_add(lamps_, struct_make<Game_Lamp>(gaps_[0].lamps.lamps_[n7340049.index].foot, gaps_[0].lamps.lamps_[n7340049.index].yaw, gaps_[0].lamps.lamps_[n7340049.index].lane, gaps_[0].lamps.lamps_[n7340049.index].state));
    }
    gaps_[0].lamps_dirty = false;
  }

  void gaps_lamps_reset() {
    Int w = 0;
    Int physics = 0;
    Bool body_valid = false;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))) {
      return;
    }
    engine_game_world_physics(n7340052, world_);
    physics = n7340052.result;
    w = physics;
    for (auto it7340053 = flow_for(n7340053, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it7340053.next();) {
      body_valid = engine_world_body_valid(w, gaps_[0].fallen[n7340053.index]);
      if (body_valid) {
        engine_world_destroy_body(w, gaps_[0].fallen[n7340053.index]);
      }
    }
    list_clear(gaps_[0].fallen);
    lamps::Field_reset_broken(gaps_[0].lamps);
    gaps_[0].lamps_dirty = true;
  }

  void gaps_lamps_draw_clear(GapsState_Part& p) {
    list_clear(p.transforms);
    list_clear(p.tints);
  }

  void gaps_lamps_draw_put(GapsState_Part& p, const Mat4& m, Vec4 tint = Vec4{0.0, 0.0, 0.0, 0.0}) {
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    place = list_length<Float>(p.transforms);
    list_insert_list(p.transforms, place, m.m);
    place_2 = list_length<Float>(p.tints);
    item = tint.x;
    item_2 = tint.y;
    item_3 = tint.z;
    item_4 = tint.w;
    list_insert_list(p.tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
  }

  void look_view_models_make(const MeshData& d, Int& id, Int r = 0, Int n8388620_index = 0, Bool g = false) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(d.positions)), 0.0)) {
      return;
    }
    engine_renderer_allocate_id(n8388622, r_);
    allocate_id = n8388622.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = game_look_cpp::vm_material(g, math_compare<"==">(kern::to<Float>(n8388620_index), 1.0)).Result;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    engine_upsert(n8388623, r_, id, d, look, List<Float>{}, List<Float>{});
    upsert = n8388623.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  struct look_command_startsResult { Bool Result = false; };
  KERN_PURE look_command_startsResult look_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
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
    engine_renderer_allocate_id(n9437194, r_);
    allocate_id = n9437194.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = mat;
    look.visible = false;
    engine_upsert(n9437195, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n9437195.result;
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
      engine_renderer_allocate_id(n9437199, r_);
      allocate_id = n9437199.result;
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
    engine_upsert(n9437200, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n9437200.result;
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
    for (auto it9437201 = flow_for(n9437201, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it9437201.next();) {
      if (logic_and(math_compare<">=">(kern::to<Float>(units_[n9437201.index].car), 0.0), math_compare<"!=">(kern::to<Float>(units_[n9437201.index].car), kern::to<Float>(car_index_)))) {
        engine_game_world_set_alive(world_, cars_[units_[n9437201.index].car].actor, false);
        cars_[units_[n9437201.index].car].placed = false;
      }
      if (math_compare<">=">(kern::to<Float>(units_[n9437201.index].car), 0.0)) {
        cars_[units_[n9437201.index].car].unit = -1;
      }
    }
    list_clear(units_);
    for (auto it9437202 = flow_for(n9437202, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it9437202.next();) {
      pdrones_[n9437202.index].active = false;
    }
    drone_wave_live_ = false;
    next_drone_wave_ = -1.0;
    star_since_ = -1.0;
  }

  void vr_room(Vector& f, Vector& r, Vector& up) {
    Bool xr_follow_head = false;
    Bool xr_follow_head_2 = false;
    Bool both = false;
    Float a = 0.0;
    Float xr_head_yaw = 0.0;
    Float c = 0.0;
    Float s = 0.0;
    Bool xr_full_room = false;
    Bool xr_full_room_2 = false;
    Bool both_2 = false;
    Vector ff = Vector{0.0, 0.0, 0.0};
    Vector rr = Vector{0.0, 0.0, 0.0};
    f = vector_sub(cam_.target, cam_.eye);
    f = vector_make(vector_break(f).x, vector_break(f).y, 0.0);
    f = logic_select<Vector>(math_compare<">">(vector_length(f), 9.999999717180685e-10), vector_normalize(f), Vector{0.0, 1.0, 0.0});
    up = Vector{0.0, 0.0, 1.0};
    both = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
    if (both) {
      xr_follow_head_2 = engine_engine_xr_follow_head(engine_);
      both = xr_follow_head_2;
    }
    if (both) {
      xr_head_yaw = engine_engine_xr_head_yaw(engine_);
      a = kern::formula([](Float xr_head_yaw, Float kDeg) -> Float { return ((-xr_head_yaw) * kDeg); }, xr_head_yaw, kDeg);
      c = kern::formula([](Float a) -> Float { return kern::math::cos(a); }, a);
      s = kern::formula([](Float a) -> Float { return kern::math::sin(a); }, a);
      f = logic_select<Vector>(math_compare<">">(vector_length(vector_make(kern::formula([](Float x, Float c, Float y, Float s) -> Float { return ((x * c) - (y * s)); }, vector_break(f).x, c, vector_break(f).y, s), kern::formula([](Float x, Float s, Float y, Float c) -> Float { return ((x * s) + (y * c)); }, vector_break(f).x, s, vector_break(f).y, c), 0.0)), 9.999999717180685e-10), vector_normalize(vector_make(kern::formula([](Float x, Float c, Float y, Float s) -> Float { return ((x * c) - (y * s)); }, vector_break(f).x, c, vector_break(f).y, s), kern::formula([](Float x, Float s, Float y, Float c) -> Float { return ((x * s) + (y * c)); }, vector_break(f).x, s, vector_break(f).y, c), 0.0)), Vector{0.0, 1.0, 0.0});
    } else {
      both_2 = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
      if (both_2) {
        xr_full_room_2 = engine_engine_xr_full_room(engine_);
        both_2 = xr_full_room_2;
      }
      if (both_2) {
        ff = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), f);
        rr = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(ff, cam_.up)), 9.999999717180685e-10), vector_normalize(vector_cross(ff, cam_.up)), vector_make(vector_break(f).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(f).x), 0.0));
        f = ff;
        up = vector_cross(rr, ff);
      }
    }
    r = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(f, up)), 9.999999717180685e-10), vector_normalize(vector_cross(f, up)), vector_make(vector_break(f).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(f).x), 0.0));
  }

  struct vr_hand_matrix_WResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE vr_hand_matrix_WResult vr_hand_matrix_W(Vector r = Vector{0.0, 0.0, 0.0}, Vector up = Vector{0.0, 0.0, 0.0}, Vector f = Vector{0.0, 0.0, 0.0}, Vector v = Vector{0.0, 0.0, 0.0}) {
    return {vector_add(vector_add(vector_scale(r, vector_break(v).x), vector_scale(up, vector_break(v).y)), vector_scale(f, kern::formula([](Float z) -> Float { return (-z); }, vector_break(v).z)))};
  }

  struct vr_hand_matrixResult { Bool Result = false; };
  vr_hand_matrixResult vr_hand_matrix(Mat4& out, Int side = 0) {
    Bool xr = false;
    Bool xr_2 = false;
    Bool either = false;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Vector x = Vector{0.0, 0.0, 0.0};
    Vector y = Vector{0.0, 0.0, 0.0};
    Vector z = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    either = logic_or(logic_not(vr_), logic_not(math_compare<"!=">(kern::to<Float>(engine_), 0.0)));
    if (logic_not(either)) {
      xr_2 = engine_engine_xr(engine_);
      either = logic_not(xr_2);
    }
    if (logic_or(logic_or(logic_or(either, math_compare<"<">(kern::to<Float>(side), 0.0)), math_compare<">">(kern::to<Float>(side), 1.0)), logic_not(vr_hands_[side].valid))) {
      return {false};
    }
    f = Vector{0.0, 0.0, 0.0};
    r = Vector{0.0, 0.0, 0.0};
    up = Vector{0.0, 0.0, 0.0};
    vr_room(f, r, up);
    x = vr_hand_matrix_W(r, up, f, engine_math_math::Quat_rotate(vr_hands_[side].q, Vector{1.0, 0.0, 0.0}).Result).Result;
    y = vr_hand_matrix_W(r, up, f, engine_math_math::Quat_rotate(vr_hands_[side].q, Vector{0.0, 1.0, 0.0}).Result).Result;
    z = vr_hand_matrix_W(r, up, f, engine_math_math::Quat_rotate(vr_hands_[side].q, Vector{0.0, 0.0, 1.0}).Result).Result;
    p = vector_add(cam_.eye, vr_hand_matrix_W(r, up, f, vr_hands_[side].p).Result);
    out = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    out.m[0] = vector_break(x).x;
    out.m[1] = vector_break(x).y;
    out.m[2] = vector_break(x).z;
    out.m[4] = vector_break(y).x;
    out.m[5] = vector_break(y).y;
    out.m[6] = vector_break(y).z;
    out.m[8] = vector_break(z).x;
    out.m[9] = vector_break(z).y;
    out.m[10] = vector_break(z).z;
    out.m[12] = vector_break(p).x;
    out.m[13] = vector_break(p).y;
    out.m[14] = vector_break(p).z;
    return {true};
  }

  struct vr_grab_validResult { Bool Result = false; };
  KERN_PURE vr_grab_validResult vr_grab_valid(Int w = 0, physics_Body b = physics_Body{Int64{0x0}}) {
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    physics_BodyType body_type = physics_BodyType::fixed;
    physics_BodyType body_type_2 = physics_BodyType::fixed;
    Bool both_2 = false;
    both = engine_physics_physics::Body_valid(b).Result;
    if (both) {
      body_valid_2 = engine_world_body_valid(w, b);
      both = body_valid_2;
    }
    both_2 = both;
    if (both_2) {
      body_type_2 = engine_world_body_type(w, b);
      both_2 = enum_equal<physics_BodyType>(body_type_2, physics_BodyType::dynamic);
    }
    return {both_2};
  }

  void vr_set_hand(Int side = 0, Bool valid = false, Vector p = Vector{0.0, 0.0, 0.0}, Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    if (logic_or(math_compare<"<">(kern::to<Float>(side), 0.0), math_compare<">">(kern::to<Float>(side), 1.0))) {
      return;
    }
    vr_hands_[side].valid = valid;
    if (logic_not(valid)) {
      vr_hands_[side].had = false;
      return;
    }
    vr_hands_[side].p = p;
    vr_hands_[side].q = q;
  }

  struct vr_room_forwardResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  vr_room_forwardResult vr_room_forward() {
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    f = Vector{0.0, 0.0, 0.0};
    r = Vector{0.0, 0.0, 0.0};
    up = Vector{0.0, 0.0, 0.0};
    vr_room(f, r, up);
    return {f};
  }

  struct vr_hand_rayResult { Bool Result = false; };
  vr_hand_rayResult vr_hand_ray(Vector& from, Vector& dir) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    n10485773 = vr_hand_matrix(m, 1);
    if (logic_not(n10485773.Result)) {
      return {false};
    }
    from = engine_math_math::Mat4_translation(m).Result;
    dir = logic_select<Vector>(math_compare<">">(vector_length(vector_make(kern::formula([](Float a) -> Float { return (-a); }, m.m[8]), kern::formula([](Float a) -> Float { return (-a); }, m.m[9]), kern::formula([](Float a) -> Float { return (-a); }, m.m[10]))), 9.999999717180685e-10), vector_normalize(vector_make(kern::formula([](Float a) -> Float { return (-a); }, m.m[8]), kern::formula([](Float a) -> Float { return (-a); }, m.m[9]), kern::formula([](Float a) -> Float { return (-a); }, m.m[10]))), Vector{1.0, 0.0, 0.0});
    return {true};
  }

  void vr_scope_update() {
    Int r = 0;
    Int renderer = 0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool on = false;
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Bool both_2 = false;
    Bool set_visible = false;
    Int fbo = 0;
    Int create_render_target = 0;
    MeshData d = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Vector n = Vector{0.0, 0.0, 0.0};
    Int kSeg = 0;
    Float a = 0.0;
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    Vector x = Vector{0.0, 0.0, 0.0};
    Vector y = Vector{0.0, 0.0, 0.0};
    Vector z = Vector{0.0, 0.0, 0.0};
    Vector at = Vector{0.0, 0.0, 0.0};
    Vector eyepiece = Vector{0.0, 0.0, 0.0};
    Vector objective = Vector{0.0, 0.0, 0.0};
    Camera c = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
    Vector eye = Vector{0.0, 0.0, 0.0};
    Vector xr_eye = Vector{0.0, 0.0, 0.0};
    Bool ads = false;
    Float lens = 0.0;
    Vector c0 = Vector{0.0, 0.0, 0.0};
    Fixed<Float, 16> lm = Fixed<Float, 16>{};
    Bool update_instances = false;
    Bool set_visible_2 = false;
    renderer = engine_engine_renderer(engine_);
    r = renderer;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    both = vr_;
    if (both) {
      xr_2 = engine_engine_xr(engine_);
      both = xr_2;
    }
    both_2 = logic_and(logic_and(logic_and(both, enum_equal<Mode>(mode_, Mode::foot)), logic_not(player_dead_)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper));
    if (both_2) {
      n10485775 = vr_hand_matrix(m, 1);
      both_2 = n10485775.Result;
    }
    on = both_2;
    engine_engine_set_xr_scope(engine_, false);
    if (logic_not(on)) {
      if (math_compare<"!=">(kern::to<Float>(vr_scope_mesh_), 0.0)) {
        engine_renderer_set_visible(n10485776, r, vr_scope_mesh_, false);
        set_visible = n10485776.result;
      }
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_tex_), 0.0))) {
      fbo = 0;
      engine_renderer_create_render_target(n10485777, r, 512, fbo);
      create_render_target = n10485777.result;
      fbo = n10485777.framebuffer;
      vr_scope_tex_ = create_render_target;
      engine_engine_set_xr_scope_fbo(engine_, fbo);
      engine_engine_set_xr_scope_size(engine_, 512);
      if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_tex_), 0.0))) {
        return;
      }
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_mesh_), 0.0))) {
      d = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
      n = Vector{0.0, -1.0, 0.0};
      engine_geo_mesh_data::MeshData_push(d, Vector{0.0, 0.0, 0.0}, n, Vector{1.0, 1.0, 1.0}, Vector{0.5, 0.5, 0.0});
      kSeg = 28;
      for (auto it10485778 = flow_for(n10485778, 0, kSeg); it10485778.next();) {
        a = kern::formula([](Float index, Float kSeg, Float kPi) -> Float { return ((kern::math::div(index, kSeg) * 2.0) * kPi); }, kern::to<Float>(n10485778.index), kern::to<Float>(kSeg), kPi);
        engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.5); }, a), 0.0, kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.5); }, a)), n, Vector{1.0, 1.0, 1.0}, vector_make(kern::formula([](Float a) -> Float { return (0.5 + (kern::math::cos(a) * 0.5)); }, a), kern::formula([](Float a) -> Float { return (0.5 - (kern::math::sin(a) * 0.5)); }, a), 0.0));
        if (math_compare<">">(kern::to<Float>(n10485778.index), 0.0)) {
          place = list_length<Int>(d.indices);
          item = 0;
          item_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n10485778.index)));
          item_3 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32((index + 1.0)); }, kern::to<Float>(n10485778.index)));
          list_insert_list(d.indices, place, list_make<Int, "3">(item, item_2, item_3));
        }
      }
      engine_renderer_allocate_id(n10485779, r);
      allocate_id = n10485779.result;
      vr_scope_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_scope_tex_;
      engine_upsert(n10485780, r, vr_scope_mesh_, d, look, List<Float>{}, List<Float>{});
      upsert = n10485780.result;
      if (logic_not(upsert)) {
        engine_renderer_release_id(r, vr_scope_mesh_);
        vr_scope_mesh_ = 0;
        return;
      }
    }
    x = vector_make(m.m[0], m.m[1], m.m[2]);
    y = vector_make(m.m[4], m.m[5], m.m[6]);
    z = vector_make(m.m[8], m.m[9], m.m[10]);
    at = engine_math_math::Mat4_translation(m).Result;
    eyepiece = vector_add(vector_add(at, vector_scale(y, 0.075)), vector_scale(z, 0.09));
    objective = vector_sub(vector_add(at, vector_scale(y, 0.075)), vector_scale(z, 0.3));
    c = cam_;
    c.eye = objective;
    c.target = vector_sub(objective, z);
    c.up = y;
    c.fov = kern::formula([](Float kDeg) -> Float { return (5.0 * kDeg); }, kDeg);
    c.near = 0.05;
    engine_engine_set_xr_scope_camera(engine_, c);
    engine_engine_set_xr_scope(engine_, true);
    xr_eye = engine_engine_xr_eye(engine_);
    eye = xr_eye;
    ads = logic_or(logic_and(pad.connected, math_compare<">">(pad.lt, 0.5)), math_compare<"<">(vector_length(vector_sub(eyepiece, eye)), 0.12));
    lens = kern::formula([](Float ads) -> Float { return ((ads != 0.0) ? 0.16 : 0.034); }, kern::to<Float>(ads));
    c0 = logic_select<Vector>(ads, vector_sub(eye, vector_scale(z, 0.3)), eyepiece);
    lm = list_make<Float, "16">(kern::formula([](Float x, Float lens) -> Float { return (x * lens); }, vector_break(x).x, lens), kern::formula([](Float y, Float lens) -> Float { return (y * lens); }, vector_break(x).y, lens), kern::formula([](Float z, Float lens) -> Float { return (z * lens); }, vector_break(x).z, lens), 0.0, kern::formula([](Float x) -> Float { return (-x); }, vector_break(z).x), kern::formula([](Float y) -> Float { return (-y); }, vector_break(z).y), kern::formula([](Float z) -> Float { return (-z); }, vector_break(z).z), 0.0, kern::formula([](Float x, Float lens) -> Float { return (x * lens); }, vector_break(y).x, lens), kern::formula([](Float y, Float lens) -> Float { return (y * lens); }, vector_break(y).y, lens), kern::formula([](Float z, Float lens) -> Float { return (z * lens); }, vector_break(y).z, lens), 0.0, vector_break(c0).x, vector_break(c0).y, vector_break(c0).z, 1.0);
    vr_scoped_ = ads;
    engine_renderer_update_instances(n10485781, r, vr_scope_mesh_, lm, List<Float>{});
    update_instances = n10485781.result;
    engine_renderer_set_visible(n10485782, r, vr_scope_mesh_, true);
    set_visible_2 = n10485782.result;
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool added = false; Int index = 0; } n1048586;
  struct { Bool added = false; Int index = 0; } n1048587;
  struct { Bool added = false; Int index = 0; } n1048588;
  struct : kern::Node<1048589> { Int index = 0; } n1048589;
  struct { Bool added = false; Int index = 0; } n1048590;
  struct { Int result = 0; } n1048594;
  struct { Bool result = false; } n1048595;
  gaps_barricade_doorsResult n1048597;
  struct : kern::Node<1048598> { Int index = 0; } n1048598;
  struct : kern::Node<1048599> { Int index = 0; } n1048599;
  struct { Bool result = false; } n1048600;
  struct { Bool result = false; Bool changed = false; } n1048601;
  struct { Bool result = false; } n1048602;
  vr_hand_rayResult n2097165;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n3145738;
  struct : kern::Node<3145743> { Int index = 0; } n3145743;
  struct { Bool result = false; } n3145747;
  state_ofResult n3145752;
  struct { Bool result = false; } n3145753;
  struct { Bool result = false; } n3145754;
  struct { Int result = 0; } n3145757;
  game_gaps_cpp::smoke_discResult n5242890;
  struct { Int result = 0; } n5242891;
  struct { Bool result = false; } n5242892;
  struct { Int result = 0; } n5242893;
  struct { Bool result = false; } n5242894;
  struct { Int result = 0; } n5242895;
  struct { Bool result = false; } n5242896;
  struct : kern::Node<5242897> { Int index = 0; } n5242897;
  struct : kern::Node<5242898> { Int index = 0; } n5242898;
  struct { Bool result = false; } n5242899;
  struct { Bool result = false; } n5242900;
  matrix_ofResult n6291466;
  struct : kern::Node<6291467> { Int index = 0; } n6291467;
  struct { Int result = 0; } n7340042;
  struct { Bool result = false; } n7340043;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n7340044;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n7340045;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n7340046;
  struct { Bool result = false; } n7340047;
  struct { Bool result = false; } n7340048;
  struct : kern::Node<7340049> { Int index = 0; } n7340049;
  struct { Int result = 0; } n7340052;
  struct : kern::Node<7340053> { Int index = 0; } n7340053;
  struct { Int result = 0; } n8388622;
  struct { Bool result = false; } n8388623;
  struct { Int result = 0; } n9437194;
  struct { Bool result = false; } n9437195;
  struct { Int result = 0; } n9437199;
  struct { Bool result = false; } n9437200;
  struct : kern::Node<9437201> { Int index = 0; } n9437201;
  struct : kern::Node<9437202> { Int index = 0; } n9437202;
  vr_hand_matrixResult n10485773;
  vr_hand_matrixResult n10485775;
  struct { Bool result = false; } n10485776;
  struct { Int result = 0; Int framebuffer = 0; } n10485777;
  struct : kern::Node<10485778> { Int index = 0; } n10485778;
  struct { Int result = 0; } n10485779;
  struct { Bool result = false; } n10485780;
  struct { Bool result = false; Bool changed = false; } n10485781;
  struct { Bool result = false; } n10485782;
};
